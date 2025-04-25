#!/bin/bash
sudo dmesg -C

echo "---------overhead start--------"


interval=$1
if [ $interval != "0" ]; then
    sudo insmod pt_scan_${interval}_opt.ko
fi

benchmark=$2
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

if [ $interval != "0" ]; then
    sudo rmmod pt_scan
fi

echo "---------overhead done--------"