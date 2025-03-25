#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run liblinear ---"

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

run_liblinear

# RSS 9.65GB