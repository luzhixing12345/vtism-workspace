#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <time.h>

#define PAGE_SIZE 4096  // 4KB 页面大小
#define M 100  // 遍历次数

// 获取当前时间（纳秒）
static inline uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <memory_size_GB>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 解析输入的内存大小（GB）
    size_t memory_size_gb = atoi(argv[1]);
    size_t num_pages = (memory_size_gb * 1024 * 1024 * 1024ULL) / PAGE_SIZE;  
    size_t total_size = num_pages * PAGE_SIZE;

    printf("Allocating %zu GB memory (%zu pages)...\n", memory_size_gb, num_pages);

    // 使用 mmap 分配大块内存
    char *memory = (char *)mmap(NULL, total_size, PROT_READ | PROT_WRITE, 
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        perror("mmap failed");
        return EXIT_FAILURE;
    }

    printf("Memory allocated. Starting traversal...\n");

    // 记录遍历时间
    uint64_t start_time = get_time_ns();

    // 遍历 M 轮
    for (int round = 0; round < M; round++) {
        if (round % 10 == 0) {
            printf("round = %d\n", round);
        }
        for (size_t i = 0; i < num_pages; i++) {
            memory[i * PAGE_SIZE]++;  // 访问每个页面的第一个字节
        }
    }

    uint64_t end_time = get_time_ns();
    printf("Traversal completed in %.2f seconds.\n", (end_time - start_time) / 1e9);

    // 释放内存
    munmap(memory, total_size);
    return EXIT_SUCCESS;
}
