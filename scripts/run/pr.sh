#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run pr ---"

# gapbs
pr_exe=${current_dir}/benchmark/gapbs/pr
pr_args="-u26 -k20 -i10 -n10"
run ${pr_exe} ${pr_args}

# RSS: 21GB
# CPU: high cpu usage
# Time: very long