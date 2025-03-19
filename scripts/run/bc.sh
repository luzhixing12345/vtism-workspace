#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

echo "--- run bc ---"

# gapbs
bc_exe=${current_dir}/benchmark/gapbs/bc
bc_args="-u29 -k20 -i10 -n10"
run ${bc_exe} ${bc_args}

# RSS: 82GB
# CPU: high cpu usage
# Time: very long