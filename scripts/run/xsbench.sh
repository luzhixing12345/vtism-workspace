#!/bin/bash
current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run xsbench ---"

# too large
# https://github.com/mitosis-project/vmitosis-asplos21-artifact/blob/HEAD/evaluation/reference/data/xsbench/mirage-config-xsbench-LRI-20201202-154540/perflog-xsbench-mirage-LRI.dat#L6
# ./XSBench -t 48 -g 680000 -p 15000000

xsbench_exe=${current_dir}/benchmark/xsbench/src/XSBench
xsbench_arg="-t 16 -g 1200000 -p 15000000"
run ${xsbench_exe} ${xsbench_arg}

# RSS: 44GB
# CPU: low cpu usage
# Time: 675
