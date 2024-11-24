#include <libvirt/libvirt.h>
#include <stdio.h>
#include "xargparse.h"

int main(int argc, const char *argv[]) {

    char *guest_name = "vm0";
    argparse_option options[] = {
        XBOX_ARG_BOOLEAN(NULL, "-h", "--help", "show help information", NULL, "help"),
        XBOX_ARG_BOOLEAN(NULL, "-v", "--version", "show version", NULL, "version"),
        XBOX_ARG_STR(&guest_name, "-n", "--name", "the name of the guest", NULL, "name"),
        XBOX_ARG_END()
    };
    
    XBOX_argparse parser;
    XBOX_argparse_init(&parser, options, 0);
    XBOX_argparse_describe(&parser,
                           "qemu_connect",
                           "\nConnect to the QEMU process and get its PID.",
                           "");
    XBOX_argparse_parse(&parser, argc, argv);

    if (XBOX_ismatch(&parser, "help")) {
        XBOX_argparse_info(&parser);
        exit(1);
    }

    virConnectPtr conn = virConnectOpen("qemu:///system"); // 连接到 libvirt
    if (conn == NULL) {
        fprintf(stderr, "Failed to open connection to qemu:///system\n");
        return 1;
    }

    virDomainPtr dom = virDomainLookupByName(conn, guest_name); // 用你的虚拟机名称替换
    if (dom == NULL) {
        fprintf(stderr, "Failed to find the domain\n");
        virConnectClose(conn);
        return 1;
    }

    int id = virDomainGetID(dom); // 获取 QEMU 进程的 PID
    if (id == -1) {
        fprintf(stderr, "Failed to get PID of the domain\n");
        virDomainFree(dom);
        virConnectClose(conn);
        return 1;
    }

    printf("QEMU Process ID: %d\n", id);

    virDomainInfo info;
    if (virDomainGetInfo(dom, &info) == -1) {
        fprintf(stderr, "Failed to get domain info\n");
        goto end;
    }
    printf("Domain state: %s\n", info.state == VIR_DOMAIN_RUNNING ? "running" : "stopped");
    printf("Domain Max Memory: %lu MB\n", info.maxMem / 1024);
    printf("Domain used Memory: %lu MB\n", info.memory / 1024);
    printf("Domain vCPU: %d\n", info.nrVirtCpu);
    printf("Domain CPU time: %llu us\n", info.cpuTime / 1000);


end:
    virDomainFree(dom);
    virConnectClose(conn);
    XBOX_free_argparse(&parser);
    return 0;
}
