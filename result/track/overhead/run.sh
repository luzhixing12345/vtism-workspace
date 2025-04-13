#!/bin/bash

./warmup 28

echo "finish warmup"

cd workspace

benchmarks=("redis" "pr" "graph500" "liblinear" "xsbench" "btree" "gups")

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

    sudo dmesg -C
    sudo insmod ../pt_scan.ko

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

    sudo rmmod pt_scan
done
