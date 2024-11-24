#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh
source ${current_dir}/scripts/check_env.sh
source ${current_dir}/scripts/setup_system.sh
log_init

echo "---- basic info ----"
echo "Kernel version: ${KERNEL_VERSION}"
echo "Log file: ${LOG_FILE}"
system_init

declare -A benchmarks=(
    ["gapbs"]=false
    ["graph500"]=false
    ["NPB3.0-omp-C"]=false
    ["redis"]=true
    ["liblinear"]=true
    ["hpcg"]=true
)

echo ""
# 计算benchmark名字的最大长度，用于对齐
max_length=0
for benchmark in "${!benchmarks[@]}"; do
    if [ ${#benchmark} -gt $max_length ]; then
        max_length=${#benchmark}
    fi
done

# 输出所有benchmark的状态，使用右对齐格式
echo "--- benchmark status ---"
echo "Benchmark Execution Plan:"
for benchmark in "${!benchmarks[@]}"; do
    if [ "${benchmarks[$benchmark]}" == "true" ]; then
        # green
        printf "  %-${max_length}s : \033[1;32mYes\033[1;0m\n" "$benchmark"
    else
        printf "  %-${max_length}s : \033[1;31mNo\033[1;0m\n" "$benchmark"
    fi
done
# exit

# ask sudo permission
sudo echo ""
flush_cache

# gapbs
pr_exe=${current_dir}/benchmark/gapbs/pr
pr_args="-u26 -k20 -i10 -n100"
if [ "${benchmarks[gapbs]}" == "true" ]; then
    run ${pr_exe} ${pr_args}
fi

# graph500
graph500_exe=${current_dir}/benchmark/graph500/src/graph500_reference_bfs
graph500_args="20 16"
if [ "${benchmarks[graph500]}" == "true" ]; then
    run ${graph500_exe} ${graph500_args}
fi

# NPB3.0-omp-C
bt_exe=${current_dir}/benchmark/NPB3.0-omp-C/bin/bt.B
bt_args="${current_dir}/benchmark/NPB3.0-omp-C/BT/input.data"
if [ "${benchmarks[NPB3.0-omp-C]}" == "true" ]; then
    run ${bt_exe} ${bt_args}
fi

# check_redis_loading() {
#     redis_path="${current_dir}/benchmark/redis-7.4.0"
#     redis_cli="${redis_path}/src/redis-cli"
    
#     # wait until loading is done
#     while true; do
#         loading_status=$(${redis_cli} info persistence | grep "loading:1" | wc -l)
#         echo "Redis loading status: ${loading_status}"
#         if [ ${loading_status} -eq 0 ]; then
#             break
#         else
#             sleep 2
#         fi
#     done
# }


run_redis() {
    redis_path=${current_dir}/benchmark/redis-7.4.0
    redis_server_exe=${redis_path}/src/redis-server
    redis_server_args=${redis_path}/redis.conf

    ycsb_exe=${current_dir}/benchmark/ycsb-0.17.0/bin/ycsb
    workload_path=${redis_path}/workloada.large
    ycsb_args="load redis -s -P $workload_path -threads 10 -p redis.host=localhost -p redis.port=6379 -p redis.timeout=3600000"

    # run redis server in backend
    echo "start redis server in backend"
    ${redis_server_exe} ${redis_server_args} &

    # should wait until loading is done
    # or cause error: LOADING Redis is loading the dataset in memory
    # check_redis_loading
    sleep 10

    run ${ycsb_exe} ${ycsb_args}
    
    kill -9 `pgrep redis`
}

if [ "${benchmarks[redis]}" == "true" ]; then
    flush_cache
    run_redis
fi

# liblinear-multicore

run_liblinear() {
    liblinear_path=${current_dir}/benchmark/liblinear-multicore-2.47
    liblinear_exe=${liblinear_path}/train
    liblinear_args="-s 6 -m 16 -e 0.000001 ${liblinear_path}/HIGGS"
    rm -f /tmp/liblinear_initialized
    rm -f /tmp/liblinear_thrashed
    flush_cache
    run_backend ${liblinear_exe} ${liblinear_args}
    PID=$(ps | grep 'train' | head -n 1 | awk '{print $1}')
    echo "liblinear PID: ${PID}"
    while [ ! -e "/tmp/liblinear_initialized" ]
    do
        sleep 1
    done
    echo please run > /tmp/liblinear_thrashed
    echo "start training"
    
    echo "waiting for ${PID}"
    while [ -e /proc/$PID ]
    do
        sleep 1
    done
    real_time=$(awk '/real/ {print $2}' /tmp/backend_time.log)
    exe_name=$(basename $liblinear_exe)
    echo "[$exe_name]: ${real_time}" >> "$LOG_FILE"
    rm -f /tmp/backend_time.log
    flush_cache
}

if [ "${benchmarks[liblinear]}" == "true" ]; then
    run_liblinear
fi

# hpcg
run_hpcg() {
    hpcg_path=${current_dir}/benchmark/hpcg
    hpcg_exe="./xhpcg"
    cd ${hpcg_path}
    run ${hpcg_exe}
    cd ${current_dir}
}

if [ "${benchmarks[hpcg]}" == "true" ]; then
    run_hpcg
fi

echo "Done"