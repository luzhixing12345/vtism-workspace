#!/bin/bash

# 定义字典 (关联数组) 映射内核名称到 setup 函数
declare -A available_kernel_setups=(
    ["5.13.0-rc6tpp"]="setup_tpp"
    ["5.13.0-rc6nomad"]="setup_nomad"
    ["5.15.19-htmm"]="setup_htmm"       # 你可以在这里添加相应的函数
    ["5.3.0-autotiering"]="setup_autotiering"
    ["5.6.0-rc6nimble+"]="setup_nimble"
    ["5.13.1autonuma"]="setup_autonuma"
    ["6.6.0vtism+"]="setup_vtism"
)

# 检查是否使用了 root 权限
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

choose_kernel_setup() {
    KERNEL_VERSION=$(uname -r)
    echo "Kernel version: ${KERNEL_VERSION}"
    if [[ -n "${available_kernel_setups[$KERNEL_VERSION]}" ]]; then
        # 动态调用字典中存储的函数名
        ${available_kernel_setups[$KERNEL_VERSION]}
        echo "finish init setup for ${KERNEL_VERSION}"
    else
        echo "Kernel version ${KERNEL_VERSION} not found in available kernels: ${!available_kernel_setups[@]}"
        exit 1
    fi
}

# 示例 setup 函数
setup_tpp() {
    echo 1 >/sys/kernel/mm/numa/demotion_enabled
    # https://docs.kernel.org/admin-guide/sysctl/kernel.html#numa-balancing
    echo 2 >/proc/sys/kernel/numa_balancing
    swapoff -a
    echo 1000 >/proc/sys/vm/demote_scale_factor
}

setup_nomad() {
    sudo insmod kernel/async_promote.ko
    echo "insmod async_promote module"
    echo 1 >/sys/kernel/mm/numa/demotion_enabled
    echo 2 >/proc/sys/kernel/numa_balancing
    swapoff -a
    echo 1000 >/proc/sys/vm/demote_scale_factor
}

setup_autonuma() {
    echo 1 >/proc/sys/kernel/numa_balancing
    swapoff -a
}

setup_htmm() {
    echo "Setup for 5.15.19-htmm"
    # 在这里实现具体的设置逻辑
}

setup_autotiering() {
    echo "Setup for 5.3.0-autotiering"
    # 在这里实现具体的设置逻辑
}

setup_nimble() {
    echo "Setup for 5.6.0-rc6nimble+"
    # 在这里实现具体的设置逻辑
}

setup_vtism() {
    echo "Setup for vtism"
    echo 1 >/sys/kernel/mm/numa/demotion_enabled
    echo 2 >/proc/sys/kernel/numa_balancing
    swapoff -a
    echo 1000 > /proc/sys/vm/watermark_scale_factor
}

system_init() {
    echo "---- system init ---"
    # turn off hyperthreading and tune CPU to best performance
    echo "use performance governor"
    echo "performance" | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor > /dev/null
    # echo "turn off hyperthreading"
    # echo off | sudo tee /sys/devices/system/cpu/smt/control > /dev/null

    choose_kernel_setup
}

run_mem_stress() {
    echo "---- run mem stress ----"
    # left 10GB free memory for system
    nohup ./userspace/mem_stress 0 24 > /dev/null 2>&1 & 
    nohup ./userspace/mem_stress 1 24 > /dev/null 2>&1 &
}

system_init
run_mem_stress
