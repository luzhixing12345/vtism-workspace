#!/bin/bash

# 定义要修改的 SCAN_K 值
intervals=(800 1000)
make clean

# 遍历每个 interval 值
for interval in "${intervals[@]}"; do
    # 使用 sed 修改 main.c 中的 SCAN_K 值
    sed -i "s/^#define SCAN_K\s*[0-9]\+/#define SCAN_K $interval/" main.c
    
    # 进行编译
    make
    
    # 检查是否成功生成了 pt_scan.ko
    if [[ -f pt_scan.ko ]]; then
        # 重新命名生成的 .ko 文件
        mv pt_scan.ko "pt_scan_${interval}_opt.ko"
        echo "Built and renamed: pt_scan_${interval}_opt.ko"
    else
        echo "Failed to build for SCAN_K=$interval"
    fi
done
