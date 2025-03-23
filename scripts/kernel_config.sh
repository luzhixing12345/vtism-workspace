KERNEL_VERSION="tpp"
# KERNEL_VERSION="nomad"
# KERNEL_VERSION="memtis"
# KERNEL_VERSION="autonuma"
# KERNEL_VERSION="autotiering"
LOG_FILE=${current_dir}/output/${KERNEL_VERSION}-benchmark.log
echo "log file: ${LOG_FILE}"

# if LOG_FILE DIRECTORY does not exist, create it
if [ ! -d ${current_dir}/output ]; then
    mkdir -p ${current_dir}/output
fi

flush_cache() {

    # 1: clean page cache
    # 2: clean directory entry cache
    # 3: clean page cache, directory entry cache and inode cache
    echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
    echo -e "\n---- flushed cache ----\n"
}


log_init() {
    
    # if output directory does not exist, create it
    mkdir -p ${current_dir}/output/
    rm -f ${LOG_FILE}
}

info() {
    echo "------------------------------------"
    echo "$1"
    echo "------------------------------------"
}

run() {
    # 参数1: 可执行文件路径
    executable=$1
    exe_name=$(basename $executable)
    shift 1
    # 获取中间的参数
    program_args="$@"
    echo "Running: $exe_name $program_args"
    # 执行程序并记录 real time,同时将 stdout 和 stderr 输出到日志文件
    {
        # 使用 time 统计执行时间,只记录 real time
        real_time=$((time -p $executable $program_args > /dev/null) 2>&1 | awk '/real/ {print $2}')
        echo -e "[benchmark]\ncmd: $exe_name $program_args\ntime: $real_time"
    } >> "$LOG_FILE" 2>&1

    echo -e "[benchmark]\ncmd:$exe_name $program_args\ntime: $real_time"
    flush_cache
    # e2me run
}

run_backend() {
    executable=$1
    exe_name=$(basename $executable)
    shift 1
    program_args="$@"
    echo "Running: $exe_name $program_args in backend"
    { time -p ${executable} ${program_args} > /dev/null 2>&1; } 2> /tmp/backend_time.log &
}

change_numa_balance() {
    # 0: disable
    # 1: balance
    value=$1
    echo $value | sudo tee /proc/sys/kernel/numa_balancing > /dev/null
    if [ $value -eq 0 ]; then
        echo "---- disable numa balance ----"
    else
        echo "---- enable numa balance ----"
    fi
}

change_hugepage() {
    # always
    # madvise
    # never
    value=$1
    echo $value | sudo tee /sys/kernel/mm/transparent_hugepage/enabled > /dev/null
    if [ $value == "never" ]; then
        echo "---- disable transparent_hugepage ----"
    else
        echo "---- enable transparent_hugepage ----"
    fi
}

change_hugepage_defrag() {
    # always
    # madvise
    # never
    value=$1
    echo $value | sudo tee /sys/kernel/mm/transparent_hugepage/defrag > /dev/null
    if [ $value == "never" ]; then
        echo "---- disable transparent_hugepage defrag ----"
    else
        echo "---- enable transparent_hugepage defrag ----"
    fi
}


finish() {
    e2me run
}