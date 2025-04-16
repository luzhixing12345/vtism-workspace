#!/bin/bash

./warmup 28

for mod in opt; do
    echo "=============================="
    echo "Starting run with mode: $mod"
    echo "=============================="
    # 插入对应模块（如果需要）
    if [[ "$mod" == "static" ]]; then
        echo "Inserting pt_scan.ko"
        sudo insmod pt_scan.ko
    elif [[ "$mod" == "opt" ]]; then
        echo "Inserting pt_scan_opt.ko"
        sudo insmod pt_scan_opt.ko
    else
        echo "Running in base mode (no module inserted)"
    fi

    cd workspace
    # 执行 graph500 脚本
    ./scripts/run/graph500.sh
    ./scripts/run/pr.sh
    
    # 卸载模块（如果有）
    if [[ "$mod" == "static" ]]; then
        echo "Removing pt_scan.ko"
        sudo rmmod pt_scan
    elif [[ "$mod" == "opt" ]]; then
        echo "Removing pt_scan_opt.ko"
        sudo rmmod pt_scan
    fi

    echo "Finished run with mode: $mod"
    echo
    cd ..
done
