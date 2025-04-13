#!/bin/bash

# 清空 dmesg
sudo dmesg -C
rm -f *.log

for interval in 1000 1500 2000 2500
do
    echo "--------interval=${interval}------------"
    for i in {1..24}
    do
        echo "--Running test for ${i}GB...--"
        
        # 第一次执行 mem_access（无内核模块）
        ./mem_access $i > "${interval}_${i}.log"
        
        # 加载内核模块
        sudo insmod "pt_scan_${interval}_opt.ko"
        
        # 第二次执行 mem_access（有内核模块）
        ./mem_access $i >> "${interval}_${i}.log"
        
        # 卸载内核模块
        sudo rmmod pt_scan
        
        # 清空 dmesg
        sudo dmesg -C
    done
done