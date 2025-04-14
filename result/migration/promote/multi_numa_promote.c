#define _GNU_SOURCE
#include <numa.h>
#include <numaif.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define SIZE_GB    2
#define SIZE_MB    ((SIZE_GB * 1024ul))
#define PAGE_SIZE  4096
#define ACCESS_CNT 10000
#define ITER_CNT   500

int r_ratio;
pthread_t monitor_thread;

int *status = NULL;
void **pages = NULL;
int num_nodes;
unsigned int *node_counts;
int total_pages;
int cpu_node = 0;
void *mem_1, *mem_2;
bool stop = false;

void bind_cpu_to_node(int cpu_node) {
    cpu_set_t mask;
    CPU_ZERO(&mask);

    struct bitmask *cpu_mask = numa_allocate_cpumask();
    if (numa_node_to_cpus(cpu_node, cpu_mask) != 0) {
        fprintf(stderr, "Failed to get CPU for NUMA node %d\n", cpu_node);
        exit(1);
    }

    int first_cpu = -1;
    for (int i = 0; i < cpu_mask->size; i++) {
        if (numa_bitmask_isbitset(cpu_mask, i)) {
            first_cpu = i;
            break;
        }
    }
    numa_free_cpumask(cpu_mask);

    if (first_cpu == -1) {
        fprintf(stderr, "No available CPU on NUMA node %d\n", cpu_node);
        exit(1);
    }

    CPU_SET(first_cpu, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity failed");
        exit(1);
    }

    printf("CPU bound to NUMA node %d (CPU %d)\n", cpu_node, first_cpu);
}

void *allocate_memory_node(int mem_node, size_t size) {
    void *mem = malloc(size);
    memset(mem, 0, size);

    long page_size = sysconf(_SC_PAGESIZE);
    long num_pages = size / page_size;

    void **pages = malloc(num_pages * sizeof(void *));
    if (!pages) {
        perror("malloc failed");
        free(mem);
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < num_pages; i++) {
        pages[i] = (char *)mem + i * page_size;
    }

    int *nodes = malloc(num_pages * sizeof(int));
    if (!nodes) {
        perror("malloc failed");
        free(pages);
        free(mem);
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < num_pages; i++) {
        nodes[i] = mem_node;
    }

    int *status = malloc(num_pages * sizeof(int));
    int ret = move_pages(0, num_pages, pages, nodes, status, MPOL_MF_MOVE);
    if (ret != 0) {
        perror("move_pages failed");
    } else {
        printf("Successfully migrated %ld MB memory to NUMA node %d\n", size / (1024 * 1024), mem_node);
    }

    free(pages);
    free(nodes);
    free(status);
    return mem;
}

void init_numa_status() {
    num_nodes = numa_max_node() + 1;
    total_pages = SIZE_MB * 1024 / 4;

    status = (int *)calloc(total_pages * 2, sizeof(int));
    node_counts = (unsigned int *)calloc(num_nodes, sizeof(int));
    pages = (void **)malloc(total_pages * 2 * sizeof(void *));
    for (int k = 0; k < total_pages; k++) {
        pages[k] = (char *)mem_1 + k * PAGE_SIZE;
        pages[total_pages + k] = (char *)mem_2 + k * PAGE_SIZE;
    }
}

void show_mem_numa_info() {
    printf("[NUMA Page Distribution]\n");
    memset(node_counts, 0, sizeof(int) * num_nodes);
    memset(status, 0, total_pages * 2 * sizeof(int));
    int err = 0;
    if (move_pages(0, total_pages * 2, pages, NULL, status, 0) == 0) {
        for (int k = 0; k < total_pages * 2; k++) {
            if (status[k] >= 0) {
                node_counts[status[k]]++;
            } else {
                err += 1;
            }
        }
        for (int n = 0; n < num_nodes; n++) {
            printf("NUMA Node %d: %d pages (%.2f MB)\n", n, node_counts[n], node_counts[n] * 4 / 1024.0);
        }
        printf("err = %d\n", err);
        if (node_counts[cpu_node] == total_pages * 2) {
            stop = true;
        }
    } else {
        perror("move_pages failed");
    }
}

void touch_memory_dual(void *mem1, void *mem2, size_t size) {
    volatile char *arr1 = (volatile char *)mem1;
    volatile char *arr2 = (volatile char *)mem2;
    size_t num_pages = size / PAGE_SIZE;
    size_t step = PAGE_SIZE;

    printf("Starting memory access on both memory regions\n");
    for (int count = 1; count <= ITER_CNT && !stop; count++) {
        for (size_t i = 0; i < num_pages && !stop; i += (step / PAGE_SIZE)) {
            bool r = (rand() % 100) < r_ratio;
            if (r) {
                volatile int x1 = arr1[i * PAGE_SIZE];
                volatile int x2 = arr2[i * PAGE_SIZE];
            } else {
                arr1[i * PAGE_SIZE] += 1;
                arr2[i * PAGE_SIZE] += 1;
            }
            usleep(1);
        }
        usleep(1000);
    }
    printf("Finished dual memory access\n");
}

void *monitor_page_location(void *arg) {
    while (1) {
        if (stop) {
            break;
        }
        sleep(1);
        show_mem_numa_info();
    }
    printf("finished thread monitoring\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <CPU_NODE> <MEM_NODE_1> <MEM_NODE_2> <R_RATIO>\n", argv[0]);
        exit(1);
    }

    cpu_node = atoi(argv[1]);
    int mem_node_1 = atoi(argv[2]);
    int mem_node_2 = atoi(argv[3]);
    r_ratio = atoi(argv[4]);

    size_t size = SIZE_MB * 1024 * 1024;

    bind_cpu_to_node(cpu_node);

    mem_1 = allocate_memory_node(mem_node_1, size);
    mem_2 = allocate_memory_node(mem_node_2, size);
    if (!mem_1 || !mem_2) {
        printf("memory allocation failed\n");
        return 0;
    }

    init_numa_status();

    pthread_create(&monitor_thread, NULL, monitor_page_location, NULL);
    touch_memory_dual(mem_1, mem_2, size);
    pthread_join(monitor_thread, NULL);

    free(mem_1);
    free(mem_2);
    printf("Memory successfully freed\n");

    return 0;
}
