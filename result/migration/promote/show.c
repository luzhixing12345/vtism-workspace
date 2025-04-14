#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    stop = 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <输出文件名>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    FILE *outfile = fopen(filename, "a");
    if (!outfile) {
        perror("无法打开输出文件");
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_sigint);

    char buffer[4096];
    while (!stop && fgets(buffer, sizeof(buffer), stdin)) {
        fputs(buffer, stdout);   // 打印到标准输出
        fputs(buffer, outfile);  // 写入文件
        fflush(stdout);
        fflush(outfile);
    }

    fclose(outfile);
    printf("\n接收到中断信号，程序已退出。\n");
    return EXIT_SUCCESS;
}
