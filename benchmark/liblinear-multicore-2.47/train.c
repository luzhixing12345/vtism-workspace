#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "linear.h"
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL

void print_null(const char *s) {}

void exit_with_help()
{
	printf(
	"Usage: train [options] training_set_file [model_file]\n"
	"options:\n"
	"-s type : set type of solver (default 1)\n"
	"  for multi-class classification\n"
	"        0 -- L2-regularized logistic regression (primal)\n"
	"        1 -- L2-regularized L2-loss support vector classification (dual)\n"
	"        2 -- L2-regularized L2-loss support vector classification (primal)\n"
	"        3 -- L2-regularized L1-loss support vector classification (dual)\n"
	"        4 -- support vector classification by Crammer and Singer\n"
	"        5 -- L1-regularized L2-loss support vector classification\n"
	"        6 -- L1-regularized logistic regression\n"
	"        7 -- L2-regularized logistic regression (dual)\n"
	"  for regression\n"
	"       11 -- L2-regularized L2-loss support vector regression (primal)\n"
	"       12 -- L2-regularized L2-loss support vector regression (dual)\n"
	"       13 -- L2-regularized L1-loss support vector regression (dual)\n"
	"  for outlier detection\n"
	"       21 -- one-class support vector machine (dual)\n"
	"-c cost : set the parameter C (default 1)\n"
	"-p epsilon : set the epsilon in loss function of SVR (default 0.1)\n"
	"-n nu : set the parameter nu of one-class SVM (default 0.5)\n"
	"-e epsilon : set tolerance of termination criterion\n"
	"       -s 0 and 2\n"
	"               |f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2,\n"
	"               where f is the primal function and pos/neg are # of\n"
	"               positive/negative data (default 0.01)\n"
	"       -s 11\n"
	"               |f'(w)|_2 <= eps*|f'(w0)|_2 (default 0.0001)\n"
	"       -s 1, 3, 4, 7, and 21\n"
	"               Dual maximal violation <= eps; similar to libsvm (default 0.1 except 0.01 for -s 21)\n"
	"      -s 5 and 6\n"
	"               |f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1,\n"
	"               where f is the primal function (default 0.01)\n"
	"       -s 12 and 13\n"
	"               |f'(alpha)|_1 <= eps |f'(alpha0)|,\n"
	"               where f is the dual function (default 0.1)\n"
	"-B bias : if bias >= 0, instance x becomes [x; bias]; if < 0, no bias term added (default -1)\n"
	"-R : not regularize the bias; must with -B 1 to have the bias; DON'T use this unless you know what it is\n"
	"       (for -s 0, 2, 5, 6, 11)\n"
	"-wi weight: weights adjust the parameter C of different classes (see README for details)\n"
	"-v n: n-fold cross validation mode\n"
	"-C : find parameters (C for -s 0, 2 and C, p for -s 11)\n"
	"-m nr_thread : parallel version with [nr_thread] threads (default 1; only for -s 0, 1, 2, 3, 5, 6, 11, 21)\n"
	"-q : quiet mode (no outputs)\n"
	);
	exit(1);
}

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

static char *line = NULL;
static int max_line_len;

static char* readline(FILE *input)
{
	int len;

	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
void read_problem(const char *filename);
void do_cross_validation();
void do_find_parameters();

struct feature_node *x_space;
struct parameter param;
struct problem prob;
struct model* model_;
int flag_cross_validation;
int flag_find_parameters;
int flag_omp;
int flag_C_specified;
int flag_p_specified;
int flag_solver_specified;
int nr_fold;
double bias;

int main(int argc, char **argv)
{
	char input_file_name[1024];
	char model_file_name[1024];
	const char *error_msg;
	clock_t begin = clock(), end = begin;
	bool allowed_to_run = false;
	int dummy_data = 1234;
	FILE* signal_file;
	parse_command_line(argc, argv, input_file_name, model_file_name);
	// prepare memory 
	read_problem(input_file_name);
	error_msg = check_parameter(&prob,&param);

	if(error_msg)
	{
		fprintf(stderr,"ERROR: %s\n",error_msg);
		exit(1);
	}
	signal_file = fopen("/tmp/liblinear_initialized", "a");
	if (!signal_file)
	{
		fprintf(stderr, "ERROR: %s, %d\n", __FILE__, __LINE__);
		exit(1);
	}
	if (fwrite(&dummy_data, sizeof(dummy_data), 1, signal_file) != 1)
	{
		fprintf(stderr, "ERROR: %s, %d\n", __FILE__, __LINE__);
		exit(1);
	};
	fclose(signal_file);
	while (!allowed_to_run)
	{
		int err = 0;
		struct stat buf;
		err = stat("/tmp/liblinear_thrashed", &buf);
		if(err == 0 && buf.st_size != 0){
			allowed_to_run = true;
		}else{
			sleep(1);
		}
	}
	begin = clock();
	if (flag_find_parameters)
	{
		do_find_parameters();
	}
	else if(flag_cross_validation)
	{
		do_cross_validation();
	}
	else
	{
		model_=train(&prob, &param);
		if(save_model(model_file_name, model_))
		{
			fprintf(stderr,"can't save model to file %s\n",model_file_name);
			exit(1);
		}
		free_and_destroy_model(&model_);
	}
	destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(line);
	end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("==================================");
 
	printf("execution time %f (s)",time_spent);
	return 0;
}

void do_find_parameters()
{
	double start_C, start_p, best_C, best_p, best_score;
	if (flag_C_specified)
		start_C = param.C;
	else
		start_C = -1.0;
	if (flag_p_specified)
		start_p = param.p;
	else
		start_p = -1.0;

	printf("Doing parameter search with %d-fold cross validation.\n", nr_fold);
	find_parameters(&prob, &param, nr_fold, start_C, start_p, &best_C, &best_p, &best_score);
	if(param.solver_type == L2R_LR || param.solver_type == L2R_L2LOSS_SVC)
		printf("Best C = %g  CV accuracy = %g%%\n", best_C, 100.0*best_score);
	else if(param.solver_type == L2R_L2LOSS_SVR)
		printf("Best C = %g Best p = %g  CV MSE = %g\n", best_C, best_p, best_score);
}

void do_cross_validation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double, prob.l);

	cross_validation(&prob,&param,nr_fold,target);
	if(param.solver_type == L2R_L2LOSS_SVR ||
	   param.solver_type == L2R_L1LOSS_SVR_DUAL ||
	   param.solver_type == L2R_L2LOSS_SVR_DUAL)
	{
		for(i=0;i<prob.l;i++)
		{
			double y = prob.y[i];
			double v = target[i];
			total_error += (v-y)*(v-y);
			sumv += v;
			sumy += y;
			sumvv += v*v;
			sumyy += y*y;
			sumvy += v*y;
		}
		printf("Cross Validation Mean squared error = %g\n",total_error/prob.l);
		printf("Cross Validation Squared correlation coefficient = %g\n",
				((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
				((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
			  );
	}
	else
	{
		for(i=0;i<prob.l;i++)
			if(target[i] == prob.y[i])
				++total_correct;
		printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
	}

	free(target);
}

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name)
{
	int i;
	void (*print_func)(const char*) = NULL;  // default printing to stdout

	// default values
	param.solver_type = L2R_L2LOSS_SVC_DUAL;
	param.C = 1;
	param.p = 0.1;
	param.nr_thread = 1;
	param.nu = 0.5;
	param.eps = INF; // see setting below
	param.nr_weight = 0;
	param.regularize_bias = 1;
	param.weight_label = NULL;
	param.weight = NULL;
	param.init_sol = NULL;
	flag_cross_validation = 0;
	flag_C_specified = 0;
	flag_p_specified = 0;
	flag_solver_specified = 0;
	flag_find_parameters = 0;
	flag_omp = 0;
	bias = -1;

	// parse options
	for(i=1;i<argc;i++)
	{
		if(argv[i][0] != '-') break;
		if(++i>=argc)
			exit_with_help();
		switch(argv[i-1][1])
		{
			case 's':
				param.solver_type = atoi(argv[i]);
				flag_solver_specified = 1;
				break;

			case 'c':
				param.C = atof(argv[i]);
				flag_C_specified = 1;
				break;

			case 'p':
				flag_p_specified = 1;
				param.p = atof(argv[i]);
				break;

			case 'n':
				param.nu = atof(argv[i]);
				break;

			case 'e':
				param.eps = atof(argv[i]);
				break;

			case 'B':
				bias = atof(argv[i]);
				break;

			case 'm':
				flag_omp = 1;
				param.nr_thread = atoi(argv[i]);
				break;

			case 'w':
				++param.nr_weight;
				param.weight_label = (int *) realloc(param.weight_label,sizeof(int)*param.nr_weight);
				param.weight = (double *) realloc(param.weight,sizeof(double)*param.nr_weight);
				param.weight_label[param.nr_weight-1] = atoi(&argv[i-1][2]);
				param.weight[param.nr_weight-1] = atof(argv[i]);
				break;

			case 'v':
				flag_cross_validation = 1;
				nr_fold = atoi(argv[i]);
				if(nr_fold < 2)
				{
					fprintf(stderr,"n-fold cross validation: n must >= 2\n");
					exit_with_help();
				}
				break;

			case 'q':
				print_func = &print_null;
				i--;
				break;

			case 'C':
				flag_find_parameters = 1;
				i--;
				break;

			case 'R':
				param.regularize_bias = 0;
				i--;
				break;

			default:
				fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
				exit_with_help();
				break;
		}
	}

	set_print_string_function(print_func);

	// determine filenames
	if(i>=argc)
		exit_with_help();

	strcpy(input_file_name, argv[i]);

	if(i<argc-1)
		strcpy(model_file_name,argv[i+1]);
	else
	{
		char *p = strrchr(argv[i],'/');
		if(p==NULL)
			p = argv[i];
		else
			++p;
		sprintf(model_file_name,"%s.model",p);
	}

	// default solver for parameter selection is L2R_L2LOSS_SVC
	if(flag_find_parameters)
	{
		if(!flag_cross_validation)
			nr_fold = 5;
		if(!flag_solver_specified)
		{
			fprintf(stderr, "Solver not specified. Using -s 2\n");
			param.solver_type = L2R_L2LOSS_SVC;
		}
		else if(param.solver_type != L2R_LR && param.solver_type != L2R_L2LOSS_SVC && param.solver_type != L2R_L2LOSS_SVR)
		{
			fprintf(stderr, "Warm-start parameter search only available for -s 0, -s 2 and -s 11\n");
			exit_with_help();
		}
	}

	int cvthreads = 1;
#ifdef CV_OMP
	if(flag_cross_validation || flag_find_parameters)
	{
		cvthreads = nr_fold;
		int maxthreads = omp_get_num_procs();
		if(flag_omp)
		{
			omp_set_nested(1);
			maxthreads = omp_get_num_procs()/param.nr_thread;
		}
		if(cvthreads > maxthreads)
			cvthreads = maxthreads;
		omp_set_num_threads(cvthreads);
	}
#endif

	//default solver for parallel execution is L2R_L2LOSS_SVC
	if(flag_omp)
	{
		if(!flag_solver_specified)
		{
			fprintf(stderr, "Solver not specified. Using -s 2\n");
			param.solver_type = L2R_L2LOSS_SVC;
		}
		else if(param.solver_type != L2R_LR &&
			param.solver_type != L2R_L2LOSS_SVC &&
			param.solver_type != L2R_L2LOSS_SVR &&
			param.solver_type != L2R_L1LOSS_SVC_DUAL &&
			param.solver_type != L2R_L2LOSS_SVC_DUAL &&
			param.solver_type != L1R_L2LOSS_SVC &&
			param.solver_type != L1R_LR &&
			param.solver_type != ONECLASS_SVM)
		{
			printf("WARNING: parallel solvers are only available for -s 0, 1, 2, 3, 5, 6, 11, 21 now; use single-core solvers instead.\n");
			param.nr_thread = 1;
		}
	}

	if(flag_cross_validation || flag_find_parameters)
		printf("Total threads used: %d (CV threads: %d, inner threads: %d)\n", cvthreads*param.nr_thread, cvthreads, param.nr_thread);
	else
		printf("Total threads used: %d\n", param.nr_thread);


	if(param.eps == INF)
	{
		switch(param.solver_type)
		{
			case L2R_LR:
			case L2R_L2LOSS_SVC:
				param.eps = 0.01;
				break;
			case L2R_L2LOSS_SVR:
				param.eps = 0.0001;
				break;
			case L2R_L2LOSS_SVC_DUAL:
			case L2R_L1LOSS_SVC_DUAL:
			case MCSVM_CS:
			case L2R_LR_DUAL:
				param.eps = 0.1;
				break;
			case L1R_L2LOSS_SVC:
			case L1R_LR:
				param.eps = 0.01;
				break;
			case L2R_L1LOSS_SVR_DUAL:
			case L2R_L2LOSS_SVR_DUAL:
				param.eps = 0.1;
				break;
			case ONECLASS_SVM:
				param.eps = 0.01;
				break;
		}
	}
}

// read in a problem (in libsvm format)
void read_problem(const char *filename)
{
	int max_index, inst_max_index, i;
	size_t elements, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;
	max_line_len = 1024;
	line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
	{
		char *p = strtok(line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			elements++;
		}
		elements++; // for bias term
		prob.l++;
	}
	rewind(fp);

	prob.bias=bias;

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct feature_node *,prob.l);
	x_space = Malloc(struct feature_node,elements+prob.l);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = 0; // strtol gives 0 if wrong format
		readline(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;

		if(prob.bias >= 0)
			x_space[j++].value = prob.bias;

		x_space[j++].index = -1;
	}

	if(prob.bias >= 0)
	{
		prob.n=max_index+1;
		for(i=1;i<prob.l;i++)
			(prob.x[i]-2)->index = prob.n;
		x_space[j-2].index = prob.n;
	}
	else
		prob.n=max_index;

	fclose(fp);
}
