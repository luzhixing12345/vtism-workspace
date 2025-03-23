#!/bin/bash

current_dir=`pwd`

echo "---- run all ----"
# ./${current_dir}/scripts/btree.sh
./${current_dir}/scripts/redis.sh
./${current_dir}/scripts/graph500.sh
# ./${current_dir}/scripts/gups.sh
./${current_dir}/scripts/pr.sh
./${current_dir}/scripts/bc.sh
./${current_dir}/scripts/xsbench.sh