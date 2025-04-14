#!/bin/bash

kernel_name=$1
echo "$kernel_name"

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

run_program() {
    r_ratio=$1
    ./numa_promote 0 2 $r_ratio > $kernel_name.$r_ratio.log
    echo "finish $1 r ratio"
    # ./multi_numa_promote 0 2 3 $r_ratio | ./show $kernel_name.$r_ratio.log
    # echo "finish $1 r ratio mult"
}

./all_core &
ALL_CORE_PID=$!
echo "all_core 启动，PID=$ALL_CORE_PID"

run_program 0
run_program 50
run_program 100

# 杀死 all_core
echo "停止 all_core (PID=$ALL_CORE_PID)..."
kill $ALL_CORE_PID
# 可选：确认进程已终止
wait $ALL_CORE_PID 2>/dev/null
echo "all_core 已终止。"

echo "scp vm:~/$kernel_name.*.log ./"