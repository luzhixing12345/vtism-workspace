#define _GNU_SOURCE
#include <numa.h>
#include <numaif.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define GB           (1024L * 1024 * 1024)
#define PAGE_SIZE    (4L * 1024)  // 4KB Page
#define NUM_ACCESSES 1000000      // 随机访问次数
#define SLEEP_TIME   5            // 每轮 sleep 5 秒

void pin_to_numa_node(int node) {
    if (numa_available() == -1) {
        fprintf(stderr, "NUMA not available!\n");
        exit(EXIT_FAILURE);
    }

    // 绑定 CPU 到 node1
    struct bitmask *cpumask = numa_allocate_cpumask();
    numa_node_to_cpus(node, cpumask);
    if (numa_sched_setaffinity(0, cpumask) < 0) {
        perror("Failed to set CPU affinity");
        exit(EXIT_FAILURE);
    }
    numa_free_cpumask(cpumask);

    // 绑定内存到 node1
    struct bitmask *memmask = numa_allocate_nodemask();
    numa_bitmask_setbit(memmask, node);
    numa_set_membind(memmask);
    numa_free_nodemask(memmask);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <numa_node> <size>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int node = atoi(argv[1]);
    int size = atoi(argv[2]);
    printf("Binding to NUMA node %d...\n", node);
    pin_to_numa_node(node);

    printf("Allocating %dGB of memory...\n", size);
    long long memory_size = size * GB;
    char *buffer = (char *)numa_alloc_onnode(memory_size, node);
    if (!buffer) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    printf("Initializing memory...\n");
    for (long i = 0; i < memory_size; i += PAGE_SIZE) {
        buffer[i] = (char)(rand() % 256);  // 初始化
    }

    printf("Starting random memory access...\n");
    srand(time(NULL));

    while (1) {
        for (long i = 0; i < NUM_ACCESSES; i++) {
            long index = (rand() % (memory_size / PAGE_SIZE)) * PAGE_SIZE;
            buffer[index]++;  // 读写操作
        }
        printf("Sleeping for %d seconds...\n", SLEEP_TIME);
        sleep(SLEEP_TIME);
    }

    numa_free(buffer, memory_size);
    return 0;
}
