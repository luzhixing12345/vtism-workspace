#!/bin/bash

./warmup 28

echo "finish warmup"

export KERNEL_VERSION=$1

if [[ "$KERNEL_VERSION" == "vtism" ]]; then
    sudo insmod pt_scan.ko
    echo "insmod pt_scan"
fi

cd workspace

benchmarks=("redis" "pr" "graph500" "liblinear" "xsbench" "gups")

for benchmark in "${benchmarks[@]}"; do
    echo "Running $benchmark benchmark"
    case $benchmark in
        redis)
            ./scripts/run/redis.sh
            ;;
        pr)
            ./scripts/run/pr.sh
            ;;
        graph500)
            ./scripts/run/graph500.sh
            ;;
        liblinear)
            ./scripts/run/liblinear.sh
            ;;
        xsbench)
            ./scripts/run/xsbench.sh
            ;;
        btree)
            ./scripts/run/btree.sh
            ;;
        gups)
            ./scripts/run/gups.sh
            ;;
        *)
            echo "Unknown benchmark: $benchmark"
            ;;
    esac

done

if [[ "$KERNEL_VERSION" == "vtism" ]]; then
    sudo rmmod pt_scan
fi