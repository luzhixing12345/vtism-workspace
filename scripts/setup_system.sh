
# 定义字典 (关联数组) 映射内核名称到 setup 函数
declare -A available_kernel_setups=(
    ["5.13.0-rc6tpp"]="setup_tpp"
    ["5.13.0-rc6nomad"]="setup_nomad"
    ["5.15.19-htmm"]="setup_htmm"       # 你可以在这里添加相应的函数
    ["5.3.0-autotiering"]="setup_autotiering"
    ["5.6.0-rc6nimble+"]="setup_nimble"
    ["6.6.0damon+"]="setup_base"
)

choose_kernel_setup() {
    KERNEL_VERSION=$(uname -r)
    if [[ -n "${available_kernel_setups[$KERNEL_VERSION]}" ]]; then
        # 动态调用字典中存储的函数名
        ${available_kernel_setups[$KERNEL_VERSION]}
    else
        echo "Kernel version ${KERNEL_VERSION} not found in available kernels: ${!available_kernel_setups[@]}"
        exit 1
    fi
}

# 示例 setup 函数
setup_tpp() {
    echo 1 >/sys/kernel/mm/numa/demotion_enabled
    echo 2 >/proc/sys/kernel/numa_balancing
    swapoff -a
    echo 1000 >/proc/sys/vm/demote_scale_factor
}

setup_nomad() {
    cd src/nomad_module && make clean && make && insmod async_promote.ko
    echo 1 >/sys/kernel/mm/numa/demotion_enabled
    echo 2 >/proc/sys/kernel/numa_balancing
    swapoff -a
    echo 1000 >/proc/sys/vm/demote_scale_factor
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

setup_base() {
    echo "Setup for base"
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
