#!/bin/bash

rm *.log

for i in {1..24}
do
    logfile=${i}.log
    echo "-------------start running test for ${i}-------------"
    ./mem_access ${i} >> ${logfile}
    sudo dmesg -C
    sudo insmod pt_scan.ko
    ./mem_access ${i} >> ${logfile}
    sudo dmesg >> ${i}.log
    sudo rmmod pt_scan
done



