#include <stddef.h>
#define _GNU_SOURCE

#include <numa.h>
#include <numaif.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define SIZE_MB    2048l   // Allocate 2048MB (2GB) memory
#define PAGE_SIZE  4096    // Page size: 4KB
#define ACCESS_CNT 10000  // Number of memory accesses per iteration
#define ITER_CNT   500

int r_ratio;

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

void *allocate_memory_node(int mem_node, size_t size) {
    void *mem = malloc(size);
    memset(mem, 0, size);

    long page_size = sysconf(_SC_PAGESIZE);
    long num_pages = size / page_size;

    // Allocate memory for page addresses
    void **pages = malloc(num_pages * sizeof(void *));
    if (!pages) {
        perror("malloc failed");
        munmap(mem, size);
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
        munmap(mem, size);
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

// Global variables for NUMA status tracking
int *status = NULL;
void **pages = NULL;
int num_nodes;
unsigned int *node_counts;
int total_pages;

void init_numa_status(void *mem, size_t size) {
    num_nodes = numa_max_node() + 1;
    total_pages = size / PAGE_SIZE;

    status = (int *)calloc(num_nodes, sizeof(int));
    node_counts = (unsigned int *)calloc(num_nodes, sizeof(int));
    status = (int *)malloc(total_pages * sizeof(int));
    pages = (void **)malloc(total_pages * sizeof(void *));

    for (int k = 0; k < total_pages; k++) {
        pages[k] = (char *)mem + k * PAGE_SIZE;
    }
}

void show_mem_numa_info(void *mem, size_t size) {
    // Display NUMA page migration info
    printf("[NUMA Page Distribution]\n");
    memset(node_counts, 0, sizeof(int) * num_nodes);
    memset(status, 0, num_nodes * sizeof(int));

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
    } else {
        perror("move_pages failed");
    }
}

void touch_memory(void *mem, size_t size) {
    volatile char *arr = (volatile char *)mem;
    size_t num_pages = size / PAGE_SIZE;  // Calculate total number of pages
    srand(time(NULL));                    // Initialize random seed

    printf("Starting random memory access to observe NUMA migration\n");
    static int count = 1;
    while (count < ITER_CNT) {
        for (size_t i = 0; i < ACCESS_CNT; i++) {
            // Generate a random page index
            size_t rand_page = rand() % num_pages;
            bool r = (rand() % 100) < r_ratio;
            if (!r) {
                arr[rand_page * PAGE_SIZE] += 1;  // Access the page
            } else {
                volatile int x = arr[rand_page * PAGE_SIZE];
            }
        }

        // Print NUMA page distribution every 10 iterations

        if (count % 10 == 0) {
            printf("\nRandom access iteration %d/%d completed\n", count / 10, ITER_CNT / 10);
            show_mem_numa_info(mem, size);
        }
        count++;

        usleep(10000);  // Slow down memory access to reduce CPU load
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <CPU_NODE> <MEM_NODE> <R_RATIO>\n", argv[0]);
        exit(1);
    }

    int cpu_node = atoi(argv[1]);
    int mem_node = atoi(argv[2]);
    r_ratio = atoi(argv[3]);
    size_t size = SIZE_MB * 1024 * 1024;

    bind_cpu_to_node(cpu_node);
    void *mem = allocate_memory_node(mem_node, size);
    init_numa_status(mem, size);
    printf("\nRandom access iteration %d/%d completed\n", 0, ITER_CNT / 10);
    show_mem_numa_info(mem, size);

    touch_memory(mem, size);

    numa_free(mem, size);
    printf("Memory successfully freed\n");

    return 0;
}
