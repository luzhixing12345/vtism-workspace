#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run btree ---"

btree_exe=${current_dir}/benchmark/btree/btree

# -n 1<<30
btree_args="-- -n 1073741824 -l 50000000"
run ${btree_exe} ${btree_args}

# RSS: 64GB
# CPU: low cpu usage
# Time: very long