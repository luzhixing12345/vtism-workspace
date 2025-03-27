#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define GB         (1024L * 1024 * 1024)
#define ARRAY_SIZE (16L * GB / sizeof(uint64_t))  // 16GB array

uint64_t *array;

void random_memory_access() {
    struct timeval start, end;
    size_t index;

    int count = 0;

    while (1) {
        uint64_t accesses = 0;
        gettimeofday(&start, NULL);

        while (1) {
            index = rand() % ARRAY_SIZE;
            array[index]++;

            accesses++;

            gettimeofday(&end, NULL);
            double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
            if (elapsed >= 1.0) {
                printf("[%d] Max memory accesses in 1s: %lu\n", count, accesses);
                fflush(stdout);
                break;
            }
        }
        count++;
    }
}

int main() {
    // 分配大块内存
    array = (uint64_t *)malloc(16L * GB);
    if (!array) {
        perror("Memory allocation failed");
        return 1;
    }
    printf("Allocated 16GB memory\n");
    // 预热数组，防止首次访问缺页导致性能波动
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        array[i] = 0;
    }

    printf("Starting memory access benchmark...\n");
    random_memory_access();

    free(array);
    return 0;
}
