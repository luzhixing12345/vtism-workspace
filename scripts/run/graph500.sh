#!/bin/bash
current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run graph500 ---"

graph500_exe=${current_dir}/benchmark/graph500/src/graph500_reference_bfs
graph500_args="27 25"
run mpirun "-np 4 ${graph500_exe} ${graph500_args}"
# RSS: 38 GB
# CPU: low cpu usage