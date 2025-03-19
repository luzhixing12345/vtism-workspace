#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run gups ---"

gups_exe=${current_dir}/benchmark/gups_bench/gups

gups_args="34 1000000 1024"
run mpirun "-np 4 ${gups_exe} ${gups_args}"

# RSS: 128GB
# CPU: low cpu usage
# Time: very long