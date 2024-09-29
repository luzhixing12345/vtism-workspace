current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh


run() {
    # 参数1: 可执行文件路径
    executable=$1
    exe_name=$(basename $executable)
    shift 1
    # 获取中间的参数
    program_args="$@"
    echo "Running: $exe_name $program_args"
    # 执行程序并记录 real time，同时将 stdout 和 stderr 输出到日志文件
    {
        # 使用 time 统计执行时间，只记录 real time
        real_time=$( (time -p $executable $program_args > /dev/null 2>&1) 2>&1 | awk '/real/ {print $2}')
        echo "[$exe_name]: $real_time"
    } >> "$LOG_FILE" 2>&1
    flush_cache
}

log_init

# gapbs
pr_exe=${current_dir}/benchmark/gapbs/pr
pr_args="-u26 -k20 -i10 -n100"
# run ${pr_exe} ${pr_args}


# graph500
graph500_exe=${current_dir}/benchmark/graph500/src/graph500_reference_bfs
graph500_args="5 4"
run ${graph500_exe} ${graph500_args}

# NPB3.0-omp-C

echo "Done"