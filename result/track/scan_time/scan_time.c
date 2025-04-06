#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define GB (1024L * 1024L * 1024L)
#define MAX_GB 32
#define PAGE_SIZE 4096
#define RUN_INTERVAL 15

// 触发并持续访问所有已分配的内存块
void continuous_touch(char **blocks, int num_blocks, size_t block_size) {
    for (int b = 0; b < num_blocks; ++b) {
        char *mem = blocks[b];
        for (size_t i = 0; i < block_size; i += PAGE_SIZE) {
            mem[i] = (char)((i + b) % 256); // 写入一些内容以防优化
        }
    }
}

int main() {
    size_t block_size = GB;
    char **allocated_blocks = malloc(MAX_GB * sizeof(char *));
    if (!allocated_blocks) {
        perror("malloc failed for pointer array");
        return 1;
    }

    int allocated_gb = 0;
    time_t start_time = time(NULL);
    time_t last_increase = start_time;

    printf("开始分配和持续访存，总目标 %dGB，每 %d 秒增加 1GB...\n", MAX_GB, RUN_INTERVAL);

    while (allocated_gb < MAX_GB) {
        time_t now = time(NULL);

        // 每10秒分配并添加1GB
        if (now - last_increase >= RUN_INTERVAL) {
            char *block = malloc(block_size);
            if (!block) {
                perror("malloc failed for memory block");
                break;
            }
            memset(block, 0, block_size); // 初始化以触发分配
            allocated_blocks[allocated_gb] = block;
            allocated_gb++;

            printf("[%.2f s] 已分配 %d GB\n", difftime(now, start_time), allocated_gb);
            last_increase = now;
        }

        // 持续访存：遍历所有已分配内存
        continuous_touch(allocated_blocks, allocated_gb, block_size);
    }

    printf("完成 %dGB 分配，继续持续访存中，按 Ctrl+C 退出。\n", MAX_GB);
    while (1) {
        continuous_touch(allocated_blocks, allocated_gb, block_size);
    }

    // 理论上不会到达这里
    for (int i = 0; i < allocated_gb; ++i) {
        free(allocated_blocks[i]);
    }
    free(allocated_blocks);

    return 0;
}
