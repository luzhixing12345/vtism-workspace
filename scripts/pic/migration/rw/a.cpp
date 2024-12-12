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
#include <random>
#include <thread>
#include <vector>

const int NUM_PAGES = 1000;
const int PAGE_SIZE = 4096;
void *pages[NUM_PAGES];
int status[NUM_PAGES];
int nodes[NUM_PAGES];
bool stop_thread = false;  // Control the read/write thread

// Thread function for reading/writing pages
void access_pages(double read_ratio, bool random_access) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, NUM_PAGES - 1);

    while (!stop_thread) {
        for (int i = 0; i < NUM_PAGES; i++) {
            if (rng() % 100 < read_ratio * 100) {  // Read operation
                volatile char value = *((char *)pages[random_access ? dist(rng) : i]);
            } else {  // Write operation
                *((char *)pages[random_access ? dist(rng) : i]) = (char)(rng() % 256);
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));  // Control frequency
    }
}

int migrate() {
    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Use move_pages system call to move pages
    if (syscall(SYS_move_pages, 0, NUM_PAGES, pages, nodes, status, 0) == -1) {
        perror("move_pages");
        return EXIT_FAILURE;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Check success
    int failed_pages = 0;
    for (int i = 0; i < NUM_PAGES; i++) {
        if (status[i] < 0) {
            failed_pages++;
        }
    }

    printf("success/failed: %d/%d\n", NUM_PAGES - failed_pages, failed_pages);
    printf("duration: %ld us\n", duration);

    return 0;
}

int main(int argc, char *argv[]) {
    // Initialize NUMA library
    if (numa_available() == -1) {
        fprintf(stderr, "NUMA is not available on this system.\n");
        return EXIT_FAILURE;
    }

    int node_alloc_on = 3;
    int node_migrate_to = 1;

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

    // Start the read/write thread
    // argv[1] is the read ratio
    double read_ratio = double(atof(argv[1]));    // 70% read, 30% write
    bool random_access = true;  // Set to false for sequential access
    std::thread access_thread(access_pages, read_ratio, random_access);

    sleep(1);
    printf("node %d -> node %d\n", node_alloc_on, node_migrate_to);
    migrate();

    // Stop the access thread before exiting
    stop_thread = true;
    access_thread.join();

    // Free allocated memory
    for (int i = 0; i < NUM_PAGES; i++) {
        numa_free(pages[i], PAGE_SIZE);
    }

    return EXIT_SUCCESS;
}