#!/bin/bash

interval=$1
if [ $interval != "0" ]; then
    sudo insmod pt_scan_${interval}.ko
fi

benchmark=$2
cd workspace

if [ $benchmark == "redis" ]; then
    echo "run redis benchmark"
    ./scripts/run/redis.sh
elif [ $benchmark == "liblinear" ]; then
    echo "run liblinear benchmark"
    ./scripts/run/liblinear.sh
elif [ $benchmark == "graph500" ]; then
    echo "run graph500 benchmark"
    ./scripts/run/graph500.sh
elif [ $benchmark == "pr" ]; then
    echo "run pr benchmark"
    ./scripts/run/pr.sh
elif [ $benchmark == "xsbench" ]; then
    echo "run xsbench benchmark"
    ./scripts/run/xsbench.sh
elif [ $benchmark == "memcached" ]; then
    echo "run memcached benchmark"
    ./scripts/run/memcached.sh
else
    echo "unknown benchmark"
fi

if [ $interval != "0" ]; then
    sudo rmmod pt_scan
fi