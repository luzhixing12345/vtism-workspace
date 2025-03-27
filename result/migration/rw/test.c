#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 1024
#define NUM_ITERATIONS 100

// 数据结构，用于线程传递参数
typedef struct {
    char *memory;
    size_t num_pages;
    int sequential; // 1: 顺序访问, 0: 随机访问
} thread_args_t;

void *page_migration_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        for (size_t j = 0; j < args->num_pages; j++) {
            void *page = args->memory + j * PAGE_SIZE;

            // 模拟页面迁移：将页面移动到另一节点 (使用madvise模拟)
            if (madvise(page, PAGE_SIZE, MADV_DONTNEED) != 0) {
                perror("madvise failed");
                pthread_exit(NULL);
            }

            // 简单延迟模拟迁移时间
            usleep(1000); // 1ms 延迟
        }
    }
    return NULL;
}

void *read_write_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    srand(time(NULL));

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        for (size_t j = 0; j < args->num_pages; j++) {
            size_t index = args->sequential ? j : rand() % args->num_pages;
            char *page = args->memory + index * PAGE_SIZE;

            // 写操作
            memset(page, index % 256, PAGE_SIZE);

            // 读操作
            volatile char temp = page[0];
            (void)temp; // 防止编译器优化
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <0: random | 1: sequential>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int sequential = atoi(argv[1]);
    char *memory = mmap(NULL, NUM_PAGES * PAGE_SIZE, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        perror("mmap failed");
        return EXIT_FAILURE;
    }

    pthread_t migrator, reader_writer;
    thread_args_t args = {memory, NUM_PAGES, sequential};

    // 创建线程
    if (pthread_create(&migrator, NULL, page_migration_thread, &args) != 0) {
        perror("pthread_create for migrator failed");
        munmap(memory, NUM_PAGES * PAGE_SIZE);
        return EXIT_FAILURE;
    }

    if (pthread_create(&reader_writer, NULL, read_write_thread, &args) != 0) {
        perror("pthread_create for reader_writer failed");
        munmap(memory, NUM_PAGES * PAGE_SIZE);
        return EXIT_FAILURE;
    }

    // 等待线程结束
    pthread_join(migrator, NULL);
    pthread_join(reader_writer, NULL);

    munmap(memory, NUM_PAGES * PAGE_SIZE);
    printf("Test completed successfully.\n");

    return EXIT_SUCCESS;
}