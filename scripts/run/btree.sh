#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run btree ---"

btree_exe=${current_dir}/benchmark/btree/btree

# -n 1<<27 8GB
n=$((1 << 27))
btree_args="-- -n $n -l 5000000000"
run ${btree_exe} ${btree_args}

# RSS: 16GB
# CPU: low cpu usage
# Time: very long