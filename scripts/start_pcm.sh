#!/bin/bash

current_dir=`pwd`
source ${current_dir}/scripts/kernel_config.sh

sudo echo 

nohup sudo ${current_dir}/pcm/build/bin/pcm-memory > /dev/null 2>&1 &  

echo "start pcm-memory in background"

nohup sudo ${current_dir}/pcm/build/bin/pcm-raw -el ${current_dir}/pcm/event_file.txt -ext 2>/dev/null | python3 ${current_dir}/pcm/cal_latency.py > /dev/null 2>&1 &

echo "start pcm-raw in background"