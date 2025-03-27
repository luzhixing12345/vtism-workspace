#!/bin/bash

interval=$1

if [ $interval != "0" ]; then
    sudo insmod pt_scan_${interval}_opt.ko
fi

cd workspace

./userspace/mem_access_1s