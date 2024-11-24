#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#define DEFAULT_PERIOD 5
#define NS_PER_SEC     1000000000L
#define SIZE           1024  // 假设页面数为1024
#define size_of_page   4096  // 假设页面大小为4096字节

struct page {
    unsigned long entry[512];
};

void synthetic_workload(int wi, long period_ns, std::vector<double> &throughput_result) {  // wi 为写操作的概率百分比
    // 声明并初始化变量
    unsigned long nbOps = 0, nbOps_prev = 0;
    double throughput = 0.0;
    long ns, ns_prev = 0;

    void *tab;
    struct page *temp;
    struct timespec start, end;

    // 获取当前时间作为开始时间
    clock_gettime(CLOCK_MONOTONIC, &start);

    do {
        // 分配内存，按页面对齐
        if (posix_memalign(&tab, size_of_page, SIZE * size_of_page) != 0) {
            perror("posix_memalign failed");
            exit(EXIT_FAILURE);
        }
        memset(tab, 0, SIZE * size_of_page);  // 初始化内存

        // 遍历每个页面并执行读写操作
        for (int i = 0; i < SIZE; i++) {
            temp = (struct page *)((char *)tab + size_of_page * i);
            int op = rand() % 100;  // 生成随机数以决定读写操作

            if (op < wi) {
                // 写操作
                temp->entry[rand() % 512] = rand();  // 写入随机值
            } else {
                // 读操作
                unsigned long read = temp->entry[rand() % 512];
                (void)read;  // 防止未使用的变量警告
            }
            nbOps++;  // 操作计数
        }

        // 获取当前时间作为结束时间
        clock_gettime(CLOCK_MONOTONIC, &end);

        // 计算操作的持续时间（以纳秒为单位）
        ns = (end.tv_sec - start.tv_sec) * NS_PER_SEC + (end.tv_nsec - start.tv_nsec);

        // 计算吞吐量（操作次数/时间间隔）
        throughput = (nbOps - nbOps_prev) / (double)(ns - ns_prev) * 1e9;  // 每秒操作次数
        // printf("Throughput: %.2f ops/sec\n", throughput);
        throughput_result.push_back(throughput);

        // 更新前一个时间和操作计数
        ns_prev = ns;
        nbOps_prev = nbOps;

        // 释放分配的内存
        free(tab);
    } while (ns < period_ns);  // 运行直到超过指定的时间段
}

void save_result(int wi, std::vector<double> &throughput_result) {
    std::string filename = "wi_" + std::to_string(wi) + ".log";
    FILE *fp = fopen(filename.c_str(), "w");

    double average = 0.0;
    for (int i = 0; i < throughput_result.size(); i++) {
        fprintf(fp, "%.2f\n", throughput_result[i]);
        average += throughput_result[i];
    }

    average /= throughput_result.size();
    printf("Average throughput: %.2f ops/sec\n", average);
    printf("record length: %lu\n", throughput_result.size());
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc > 3 || argc < 2) {
        printf("Usage: %s <wi> <period>\n", argv[0]);
        printf("wi: 10 - 100 (interval: 10)\n");
        printf("period (s)\n");
        return 1;
    }

    int wi = atoi(argv[1]);
    int period;
    if (argc == 2) {
        period = DEFAULT_PERIOD;
    } else {
        period = atoi(argv[2]);
    }

    printf("period: %ds\n", period);
    printf("[r:w] = %d:%d\n", (100 - wi) / 10, wi / 10);

    long period_ns = (long)period * 1000000000;
    std::vector<double> throughput_result;
    throughput_result.reserve(10000);
    synthetic_workload(wi, period_ns, throughput_result);
    save_result(wi, throughput_result);
    return 0;
}