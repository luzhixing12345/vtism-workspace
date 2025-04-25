#!/bin/bash

scan_type=$1
interval=$2

if [ $scan_type == "base" ]; then
    echo "load pt_scan_${interval}.ko"
    scan_type=""
else
    echo "load pt_scan_${interval}_opt.ko"
    scan_type="_opt"
fi

sudo dmesg -C
sudo insmod pt_scan_${interval}${scan_type}.ko

benchmark=$3
cd workspace

if [ $benchmark == "redis" ]; then
    echo "run redis benchmark"
    ./scripts/run/redis.sh
elif [ $benchmark == "btree" ]; then
    echo "run btree benchmark"
    ./scripts/run/btree.sh
elif [ $benchmark == "graph500" ]; then
    echo "run graph500 benchmark"
    ./scripts/run/graph500.sh
elif [ $benchmark == "pr" ]; then
    echo "run pr benchmark"
    ./scripts/run/pr.sh
elif [ $benchmark == "xsbench" ]; then
    echo "run xsbench benchmark"
    ./scripts/run/xsbench.sh
elif [ $benchmark == "gups" ]; then
    echo "run gups benchmark"
    ./scripts/run/gups.sh
elif [ $benchmark == "bc" ]; then
    echo "run bc benchmark"
    ./scripts/run/bc.sh
else
    echo "unknown benchmark"
    echo "available benchmarks: redis, btree, graph500, pr, xsbench, gups, bc"
fi
sudo rmmod pt_scan
sudo dmesg > ${benchmark}${interval}${scan_type}.log
echo "scp vm:~/workspace/${benchmark}${interval}${scan_type}.log ."
