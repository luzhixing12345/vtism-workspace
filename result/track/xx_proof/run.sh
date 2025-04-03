#!/bin/bash

# 清空 dmesg
sudo dmesg -C

for i in {1..32}
do
    echo "Running test for ${i}GB..."

    # 记录开始时间
    time ./mem_access $i > ${i}.log

    # 加载内核模块
    sudo insmod pt_scan.ko

    # 再次执行 mem_access
    time ./mem_access $i >> ${i}.log

    # 卸载内核模块
    sudo rmmod pt_scan

    # 记录 dmesg 输出到日志
    sudo dmesg | tail -n 10 | head -n 4 >> ${i}.log
    sudo dmesg | tail -n 10 | head -n 4

    # 复制日志文件到本地
    # echo "scp vm:~/${i}.log ./"
    sudo dmesg -C
done

