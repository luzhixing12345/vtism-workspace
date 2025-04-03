#!/bin/bash

# file=/home/lzx/workspace/gpt-trace/pt_scan/*.ko
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
cpfile /home/lzx/workspace/result/track/xx_proof/mem_access_1s
cpfile /home/lzx/workspace/result/track/xx_proof/run.sh