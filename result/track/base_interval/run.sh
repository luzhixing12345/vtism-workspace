#!/bin/bash

# interval=$1

for i in {1000,1500,2000,2500}
do
echo "${i} interval"
./mem_access 1

sudo dmesg -C
sudo insmod pt_scan_${i}_opt.ko

./mem_access 1

sudo rmmod pt_scan

done
