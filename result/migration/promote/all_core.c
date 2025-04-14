#define _GNU_SOURCE
#include <numa.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int cpu_id;
} thread_arg_t;

void bind_to_cpu(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
}

void *cpu_worker(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    bind_to_cpu(targ->cpu_id);

    volatile double sum = 0;
    while (1) {
        for (unsigned long i = 0; i < 1000000UL; ++i) {
            sum += i * 0.0000001;
        }
    }
    return NULL;
}

int main() {
    if (numa_available() == -1) {
        return 1;
    }

    int max_node = numa_max_node();
    pthread_t *threads = NULL;
    thread_arg_t *args = NULL;
    int thread_count = 0;

    // 统计总线程数
    for (int node = 0; node <= max_node; ++node) {
        struct bitmask *cpus = numa_allocate_cpumask();
        if (numa_node_to_cpus(node, cpus) == 0) {
            for (int i = 0; i < cpus->size; ++i) {
                if (numa_bitmask_isbitset(cpus, i)) {
                    thread_count++;
                }
            }
        }
        numa_free_cpumask(cpus);
    }

    threads = malloc(thread_count * sizeof(pthread_t));
    args = malloc(thread_count * sizeof(thread_arg_t));

    int idx = 0;
    for (int node = 0; node <= max_node; ++node) {
        struct bitmask *cpus = numa_allocate_cpumask();
        if (numa_node_to_cpus(node, cpus) == 0) {
            for (int i = 0; i < cpus->size; ++i) {
                if (numa_bitmask_isbitset(cpus, i)) {
                    args[idx].cpu_id = i;
                    pthread_create(&threads[idx], NULL, cpu_worker, &args[idx]);
                    idx++;
                }
            }
        }
        numa_free_cpumask(cpus);
    }

    // 主线程挂起
    for (;;) pause();

    return 0;
}
