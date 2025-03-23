#!/bin/bash

current_dir=`pwd`
source scripts/kernel_config.sh
echo "log file: ${LOG_FILE}"

echo "---- run all ----"
# ./${current_dir}/scripts/btree.sh
${current_dir}/scripts/run/graph500.sh
${current_dir}/scripts/run/redis.sh
# ./${current_dir}/scripts/gups.sh
e2me run --subject "finished 2/5 of $KERNEL_VERSION"
${current_dir}/scripts/run/pr.sh
${current_dir}/scripts/run/bc.sh
${current_dir}/scripts/run/xsbench.sh
e2me run --subject "finished all of $KERNEL_VERSION"