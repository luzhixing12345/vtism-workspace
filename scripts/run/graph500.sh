#!/bin/bash
current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run graph500 ---"

graph500_exe=${current_dir}/benchmark/graph500/src/graph500_reference_bfs
graph500_args="28 26"
run mpirun "-np 4 ${graph500_exe} ${graph500_args}"
# RSS: 80 GB
# CPU: low cpu usage