#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <numa.h>
#include <numaif.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

#define PAGE_SIZE 4096  // 4KB Page

// 绑定 CPU 到指定 NUMA 节点
void bind_cpu_to_node(int node) {
    struct bitmask *cpus = numa_allocate_cpumask();
    numa_node_to_cpus(node, cpus);

    cpu_set_t set;
    CPU_ZERO(&set);
    for (int i = 0; i < numa_num_task_cpus(); i++) {
        if (numa_bitmask_isbitset(cpus, i)) {
            CPU_SET(i, &set);
        }
    }
    sched_setaffinity(0, sizeof(set), &set);
    numa_free_cpumask(cpus);
}

// 访问内存，不断进行读写
void access_memory(volatile char *memory, size_t size) {
    size_t stride = PAGE_SIZE;
    size_t num_pages = size / stride;

    while (1) {
        for (size_t i = 0; i < num_pages; i++) {
            memory[i * stride] += 1;  // 修改数据，确保不会被优化
        }
        usleep(1000);  // 等待 1ms
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NUMA Node> <Memory Size (GB)>\n", argv[0]);
        return 1;
    }

    // 解析输入参数
    int node = atoi(argv[1]);
    size_t gb = atoi(argv[2]);

    if (numa_available() == -1) {
        fprintf(stderr, "NUMA is not supported on this system.\n");
        return 1;
    }
    if (node < 0 || node > numa_max_node()) {
        fprintf(stderr, "Invalid NUMA node: %d\n", node);
        return 1;
    }

    // 绑定 CPU 到指定 NUMA 节点
    bind_cpu_to_node(node);

    // 分配 NUMA 绑定的内存
    size_t size = gb * 1024 * 1024 * 1024;
    volatile char *memory = (volatile char *)numa_alloc_onnode(size, node);

    if (!memory) {
        fprintf(stderr, "Memory allocation failed for %lu GB on NUMA node %d\n", gb, node);
        return 1;
    }

    printf("Allocated %lu GB on NUMA node %d. Start accessing memory...\n", gb, node);

    // 初始化内存，避免懒分配
    memset((void *)memory, 1, size);

    // 访问内存（无限循环）
    access_memory(memory, size);

    // 释放内存（理论上永远不会执行到这里）
    numa_free((void *)memory, size);
    return 0;
}
