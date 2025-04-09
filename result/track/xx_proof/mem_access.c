#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define M         300    // 轮数
#define SLEEP_US  10000  // 每轮休眠100ms

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

    size_t memory_size_gb = atoi(argv[1]);
    size_t num_pages = (memory_size_gb * 1024UL * 1024 * 1024) / PAGE_SIZE;
    size_t total_size = num_pages * PAGE_SIZE;

    printf("Allocating %zu GB memory (%zu pages)...\n", memory_size_gb, num_pages);
    char *memory = malloc(total_size);
    if (!memory) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    printf("Memory allocated. Starting sequential traversal...\n");

    uint64_t start_time = get_time_ns();

    for (int round = 0; round < M; ++round) {
        // if (round % 100 == 0) {
        //     fprintf(stderr, "time = %.2f\n", (get_time_ns() - start_time) / 1e9);
        // }
        for (size_t i = 0; i < num_pages; ++i) {
            memory[i * PAGE_SIZE] += 1;
        }
        usleep(SLEEP_US);
    }

    uint64_t end_time = get_time_ns();
    double time = (end_time - start_time) / 1e9;
    printf("Sequential traversal completed in %.2f seconds.\n", time);
    fprintf(stderr, "Sequential traversal completed in %.2f seconds. (%.2f)\n", time, time * 1.05);

    free(memory);
    return EXIT_SUCCESS;
}
