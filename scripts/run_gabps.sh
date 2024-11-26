#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/check_env.sh
source ${current_dir}/scripts/kernel_config.sh

# gapbs
pr_exe=${current_dir}/benchmark/gapbs/pr
pr_args="-u26 -k20 -i10 -n100"
run ${pr_exe} ${pr_args}

