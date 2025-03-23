#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "---- run all ----"
# ./${current_dir}/scripts/btree.sh
./${current_dir}/scripts/redis.sh
./${current_dir}/scripts/graph500.sh
# ./${current_dir}/scripts/gups.sh
e2me run --subject "finished 2/5 of $KERNEL_VERSION"
./${current_dir}/scripts/pr.sh
./${current_dir}/scripts/bc.sh
./${current_dir}/scripts/xsbench.sh
e2me run --subject "finished all of $KERNEL_VERSION"