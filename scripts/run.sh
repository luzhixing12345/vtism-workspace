#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh
log_init

# gapbs
pr_exe=${current_dir}/benchmark/gapbs/pr
pr_args="-u26 -k20 -i10 -n100"
# run ${pr_exe} ${pr_args}

# graph500
graph500_exe=${current_dir}/benchmark/graph500/src/graph500_reference_bfs
graph500_args="20 16"
# run ${graph500_exe} ${graph500_args}

# NPB3.0-omp-C

run_redis() {
    tag=$1
    redis_server_exe=${current_dir}/benchmark/redis-7.4.0/src/redis-server
    redis_server_args=${current_dir}/benchmark/redis-7.4.0/redis.conf
    run ${redis_server_exe} ${redis_server_args} &
    sleep 5
}

# redis
# run_redis "small"
# run_redis "large"

# liblinear-multicore

liblinear_path=${current_dir}/benchmark/liblinear-multicore-2.47
liblinear_exe=${liblinear_path}/train
liblinear_args="-s 6 -m 16 -e 0.000001 ${liblinear_path}/HIGGS"

run_liblinear() {
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
}
run_liblinear
echo "Done"