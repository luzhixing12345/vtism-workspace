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

# if [ "$kernel_name" == "nomad" ]; then
#     sudo insmod async_promote.ko
#     echo "insmod async_promote.ko"
# fi

run_program() {
    r_ratio=$1
    ./numa_promote 0 2 $r_ratio > $kernel_name.$r_ratio.log
    echo "finish $1 r ratio"
}

run_program 0
# run_program 10
run_program 20
# run_program 30
run_program 40
# run_program 50
run_program 60
# run_program 70
run_program 80
# run_program 90
run_program 100

echo "scp vm:~/$kernel_name.*.log ./"