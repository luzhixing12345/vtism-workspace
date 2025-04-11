#!/bin/bash

rm *.log

for k in {400,600,800,1000}
do
echo "-------------${k}x+b----------------"
for i in {1..16}
do
    logfile=${k}_${i}.log
    echo "-------------start running test for ${i}-------------"
    ./mem_access ${i} >> ${logfile}
    sudo dmesg -C
    sudo insmod pt_scan_${k}_opt.ko
    ./mem_access ${i} >> ${logfile}
    sudo dmesg >> ${i}.log
    sudo rmmod pt_scan
done
done
# echo "scp vm:~/*.log ${SCAN_K}/"


