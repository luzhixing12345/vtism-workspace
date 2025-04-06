#define _GNU_SOURCE
#include <numa.h>
#include <numaif.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define SIZE_MB    1024l  // Allocate 2048MB (2GB) memory
#define PAGE_SIZE  4096   // Page size: 4KB
#define ACCESS_CNT 10000  // Number of memory accesses per iteration
#define ITER_CNT   100

int r_ratio;
pthread_t monitor_thread;

// Global variables for NUMA status tracking
int *status = NULL;
void **pages = NULL;
int num_nodes;
unsigned int *node_counts;
int total_pages;
int cpu_node;
bool stop = false;

// Function to bind CPU to NUMA node
void bind_cpu_to_node(int cpu_node) {
    cpu_set_t mask;
    CPU_ZERO(&mask);

    struct bitmask *cpu_mask = numa_allocate_cpumask();
    if (numa_node_to_cpus(cpu_node, cpu_mask) != 0) {
        fprintf(stderr, "Failed to get CPU for NUMA node %d\n", cpu_node);
        exit(1);
    }

    int first_cpu = -1;
    for (int i = 0; i < cpu_mask->size; i++) {
        if (numa_bitmask_isbitset(cpu_mask, i)) {
            first_cpu = i;
            break;
        }
    }
    numa_free_cpumask(cpu_mask);

    if (first_cpu == -1) {
        fprintf(stderr, "No available CPU on NUMA node %d\n", cpu_node);
        exit(1);
    }

    CPU_SET(first_cpu, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity failed");
        exit(1);
    }

    printf("CPU bound to NUMA node %d (CPU %d)\n", cpu_node, first_cpu);
}

// Function to allocate memory on a specific NUMA node
void *allocate_memory_node(int mem_node, size_t size) {
    void *mem = malloc(size);
    memset(mem, 0, size);

    long page_size = sysconf(_SC_PAGESIZE);
    long num_pages = size / page_size;

    // Allocate memory for page addresses
    void **pages = malloc(num_pages * sizeof(void *));
    if (!pages) {
        perror("malloc failed");
        free(mem);
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < num_pages; i++) {
        pages[i] = (char *)mem + i * page_size;
    }

    // Set target NUMA node
    int *nodes = malloc(num_pages * sizeof(int));
    if (!nodes) {
        perror("malloc failed");
        free(pages);
        free(mem);
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < num_pages; i++) {
        nodes[i] = mem_node;
    }

    // Move pages to the specified NUMA node
    int *status = malloc(num_pages * sizeof(int));
    int ret = move_pages(0, num_pages, pages, nodes, status, MPOL_MF_MOVE);
    if (ret != 0) {
        perror("move_pages failed");
    } else {
        printf("Successfully migrated %ld MB memory to NUMA node %d\n", size / (1024 * 1024), mem_node);
    }

    free(pages);
    free(nodes);
    return mem;
}

// Initialize NUMA status
void init_numa_status(void *mem, size_t size) {
    num_nodes = numa_max_node() + 1;
    total_pages = size / PAGE_SIZE;

    status = (int *)calloc(total_pages, sizeof(int));
    node_counts = (unsigned int *)calloc(num_nodes, sizeof(int));
    pages = (void **)malloc(total_pages * sizeof(void *));
    for (int k = 0; k < total_pages; k++) {
        pages[k] = (char *)mem + k * PAGE_SIZE;
    }
}

// Function to show NUMA memory information
void show_mem_numa_info(void *mem, size_t size) {
    printf("[NUMA Page Distribution]\n");
    memset(node_counts, 0, sizeof(int) * num_nodes);
    memset(status, 0, total_pages * sizeof(int));

    if (move_pages(0, total_pages, pages, NULL, status, 0) == 0) {
        for (int k = 0; k < total_pages; k++) {
            if (status[k] >= 0) {
                node_counts[status[k]]++;
            } else {
                fprintf(stderr, "Error retrieving NUMA status\n");
            }
        }
        for (int n = 0; n < num_nodes; n++) {
            printf("NUMA Node %d: %d pages (%.2f MB)\n",
                   n,
                   node_counts[n],
                   node_counts[n] * PAGE_SIZE / (1024.0 * 1024.0));
        }
        if (node_counts[cpu_node] == total_pages) {
            stop = true;
        }
    } else {
        perror("move_pages failed");
    }
}

void touch_memory(void *mem, size_t size) {
    volatile char *arr = (volatile char *)mem;
    size_t num_pages = size / PAGE_SIZE;  // 计算总页数
    size_t step = PAGE_SIZE;              // **步长默认 4KB，可改大**

    printf("Starting sequential memory access to observe NUMA migration\n");
    for (int count = 1; count <= ITER_CNT && !stop; count++) {
        for (size_t i = 0; i < num_pages && !stop; i += (step / PAGE_SIZE)) {
            bool r = (rand() % 100) < r_ratio;
            if (r) {
                volatile int x = arr[i * PAGE_SIZE];  // **逐页访问**
            } else {
                arr[i * PAGE_SIZE] += 1;  // **逐页访问**
            }
            usleep(10);

            // if (count % 10 == 0) {  // 每 10 轮显示 NUMA 迁移状态
            //     printf("\nIteration %d/%d completed\n", count / 10, ITER_CNT / 10);
            //     show_mem_numa_info(mem, size);
            // }
        }
        usleep(10000);  // 休眠 10ms，减少 CPU 负载
    }
    printf("finished touch memory\n");
}

// Thread function to monitor page location every second
void *monitor_page_location(void *arg) {
    while (1) {
        if (stop) {
            break;
        }
        sleep(1);
        show_mem_numa_info(arg, SIZE_MB * 1024 * 1024);  // Periodically check page locations
    }
    printf("finished thread monitoring\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <CPU_NODE> <MEM_NODE> <R_RATIO>\n", argv[0]);
        exit(1);
    }

    cpu_node = atoi(argv[1]);
    int mem_node = atoi(argv[2]);
    r_ratio = atoi(argv[3]);
    size_t size = SIZE_MB * 1024 * 1024;

    bind_cpu_to_node(cpu_node);
    void *mem = allocate_memory_node(mem_node, size);
    if (!mem) {
        printf("mmap failed\n");
        return 0;
    }
    init_numa_status(mem, size);

    // Start the page location monitoring thread
    pthread_create(&monitor_thread, NULL, monitor_page_location, mem);

    // Perform memory access in the main thread
    touch_memory(mem, size);

    pthread_join(monitor_thread, NULL);  // Wait for monitor thread to finish

    numa_free(mem, size);
    printf("Memory successfully freed\n");

    return 0;
}
