#!/bin/bash

kernel_name=$1

# kernel_name should be one of
names=("vtism" "nomad" "autonuma" "tpp")
if [[ ! " ${names[@]} " =~ " ${kernel_name} " ]]; then
    echo "Invalid kernel name"
    echo "available names: ${names[@]}"
    exit 1
fi

echo "use $kernel_name"

echo 2 | sudo tee /proc/sys/kernel/numa_balancing
# if name is vtism
if [ "$kernel_name" == "vtism" ]; then
    echo 1 > /sys/kernel/mm/vtism/migration_enable
    echo "use vtism migration"
fi

if [ "$kernel_name" == "nomad" ]; then
    sudo insmod async_promote.ko
    echo "insmod async_promote.ko"
fi

./numa_promote 0 2 0 > $kernel_name.0.log
echo "finish 0 r ratio"
./numa_promote 0 2 30 > $kernel_name.30.log
echo "finish 30 r ratio"
./numa_promote 0 2 50 > $kernel_name.50.log
echo "finish 50 r ratio"
./numa_promote 0 2 70 > $kernel_name.70.log
echo "finish 70 r ratio"
echo "scp vm:~/$kernel_name.*.log ./"