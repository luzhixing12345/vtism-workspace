#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define PAGE_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mem_gb>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long long mem_gb = atol(argv[1]);
    if (mem_gb <= 0) {
        fprintf(stderr, "Invalid mem_gb: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    size_t total_bytes = (size_t)(mem_gb * 1024 * 1024 * 1024);
    size_t total_pages = total_bytes / PAGE_SIZE;

    printf("Allocating %lld GB\n", mem_gb);

    uint8_t *buffer = malloc(total_bytes);
    if (!buffer) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    // 逐页访问，确保内存页被实际分配
    for (size_t i = 0; i < total_pages; ++i) {
        buffer[i * PAGE_SIZE] = (uint8_t)(i & 0xFF);
    }

    printf("finish warmup\n");
    // getchar();

    free(buffer);
    return EXIT_SUCCESS;
}
