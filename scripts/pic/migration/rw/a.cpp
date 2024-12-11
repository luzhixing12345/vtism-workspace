#include <bits/types/time_t.h>
#include <errno.h>
#include <numa.h>
#include <numaif.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <chrono>

const int NUM_PAGES = 1000;
const int PAGE_SIZE = 4096;
void *pages[NUM_PAGES];
int status[NUM_PAGES];
int nodes[NUM_PAGES];

int migrate(int node_alloc_on, int node_migrate_to) {
    printf("node %d -> node %d\n", node_alloc_on, node_migrate_to);

    int count = 2000;
    long long total_duration = 0;  // To accumulate time
    int loop_counter = 0;          // To count the loops

    double sum_duration = 0;
    int interval = 500;

    while (loop_counter < count) {
        for (int i = 0; i < NUM_PAGES; i++) {
            pages[i] = numa_alloc_onnode(PAGE_SIZE, node_alloc_on);
            if (!pages[i]) {
                perror("numa_alloc_onnode");
                return EXIT_FAILURE;
            }
            // Initialize memory
            *((char *)pages[i]) = (char)i;
            nodes[i] = node_migrate_to;  // Target node for migration is node 1
        }

        // Start timing
        auto start = std::chrono::high_resolution_clock::now();

        // Use move_pages system call to move pages
        if (syscall(SYS_move_pages, 0, NUM_PAGES, pages, nodes, status, 0) == -1) {
            perror("move_pages");
            return EXIT_FAILURE;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        total_duration += duration;
        sum_duration += duration;
        loop_counter++;

        // check success
        int failed_pages = 0;
        for (int i = 0; i < NUM_PAGES; i++) {
            if (status[i] < 0) {
                failed_pages++;
            }
        }
        if (failed_pages > 0) {
            printf("Failed to move %d pages\n", failed_pages);
        }

        // Output the average duration every 100 loops
        if (loop_counter % interval == 0) {
            double average_duration = static_cast<double>(total_duration) / interval;
            printf("Average duration after %d loops: %.2f us\n", loop_counter, average_duration);
            total_duration = 0;
        }

        // Free allocated memory
        for (int i = 0; i < NUM_PAGES; i++) {
            numa_free(pages[i], PAGE_SIZE);
        }
    }

    // printf("result %.2f us\n", sum_duration / count);

    return 0;
}

int main() {
    // Initialize NUMA library
    if (numa_available() == -1) {
        fprintf(stderr, "NUMA is not available on this system.\n");
        return EXIT_FAILURE;
    }

    int node_alloc = 0;
    int node_migrate = 1;
    migrate(node_alloc, node_migrate);

    return EXIT_SUCCESS;
}