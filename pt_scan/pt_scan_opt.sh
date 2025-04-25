#!/bin/bash

scan_type=$1

if [ $scan_type == "base" ]; then
    echo "load pt_scan.ko"
    scan_type=""
else
    echo "load pt_scan_opt.ko"
    scan_type="_opt"
fi

sudo dmesg -C
sudo insmod pt_scan${scan_type}.ko

benchmark=$2
cd workspace

if [ $benchmark == "redis" ]; then
    echo "run redis benchmark"
    ./scripts/run/redis.sh
elif [ $benchmark == "graph500" ]; then
    echo "run graph500 benchmark"
    ./scripts/run/graph500.sh
elif [ $benchmark == "pr" ]; then
    echo "run pr benchmark"
    ./scripts/run/pr.sh
elif [ $benchmark == "xsbench" ]; then
    echo "run xsbench benchmark"
    ./scripts/run/xsbench.sh
else
    echo "unknown benchmark"
fi
sudo rmmod pt_scan
sudo dmesg > ${benchmark}${scan_type}.log
echo "scp vm:~/workspace/result/pt_scan/${benchmark}${scan_type}.log ."