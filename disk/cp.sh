#!/bin/bash

# sudo cp $file tmp/home/kamilu

# file=/home/lzx/workspace/gpt-trace/pt_scan/run.sh
# sudo cp $file tmp/home/kamilu

cpfile() {
    file=$1
    sudo cp $file tmp/home/kamilu
    echo "copy $1"
}

# file=/home/lzx/workspace/gpt-trace/pt_scan/overhead.sh
# file=/home/lzx/workspace/result/trace/pt_scan/pt_scan_opt.sh
# cpfile /home/lzx/workspace/result/track/linear_not_good/mem_access
# cpfile /home/lzx/workspace/result/track/linear_not_good/run.sh
cpfile "/home/lzx/workspace/gpt-trace/pt_scan/*.ko"
# cpfile /home/lzx/workspace/result/migration/promote/numa_promote
# cpfile /home/lzx/workspace/result/migration/promote/run.sh
# cpfile /home/lzx/workspace/result/track/adaptive_scan/mem_access
# cpfile /home/lzx/workspace/result/track/adaptive_scan/run.sh
# cpfile /home/lzx/workspace/result/track/base_interval/mem_access
# cpfile /home/lzx/workspace/result/track/base_interval/run.sh


# cpfile /home/lzx/workspace/result/track/overhead/run.sh
# cpfile /home/lzx/workspace/result/track/overhead/warmup
# cpfile /home/lzx/workspace/result/track/xx_proof/run.sh
# cpfile /home/lzx/workspace/result/track/xx_proof/mem_access
# cpfile /home/lzx/workspace/result/track/pt_scan/pt_scan_opt.sh
# cpfile /home/lzx/workspace/result/track/xx_proof/mem_access

# cpfile /home/lzx/workspace/result/migration/promote/all_core
# cpfile /home/lzx/workspace/result/migration/promote/run.sh
# cpfile /home/lzx/workspace/result/migration/promote/mem_access
# cpfile /home/lzx/workspace/result/migration/promote/numa_promote
# cpfile /home/lzx/workspace/result/migration/promote/multi_numa_promote
# cpfile /home/lzx/workspace/result/migration/promote/show
# cpfile /home/lzx/workspace/kernel/async_promote.ko

# cpfile /home/lzx/workspace/result/ablation/run.sh

cpfile /home/lzx/workspace/result/benchmark/run.sh