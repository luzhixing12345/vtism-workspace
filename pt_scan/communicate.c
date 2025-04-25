#include "communicate.h"

#include <linux/file.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "ivshmem-drv.h"
#include "page_table.h"

static uint64_t package_id = 0;
extern struct ivshmem_data *ivshmem_dev;

#define UINT64_MAX 0xffffffffffffffff

int write_to_ivshmem(void *buf, uint64_t size, uint64_t offset) {
    memcpy(ivshmem_dev->hw_addr + offset, buf, size);
    return 0;
}

int send_data_to_host(struct pt_result *result) {
    struct ivshmem_head head;
    head.id = package_id++;
    if (package_id == UINT64_MAX) {
        package_id = 0;
    }
    head.pte_num = result->pte_cnt;
    write_to_ivshmem(&head, sizeof(head), 0);
    // INFO("id: %lld, pte_num: %lld\n", head.id, head.pte_num);
    // uint64_t pte_addr_size = result->pte_cnt * sizeof(pt_addr_t);
    // if (pte_addr_size > MAX_BUFFER_SIZE) {
    //     ERR("pte_addr_size: %lld >= 1GB\n", pte_addr_size);
    //     return -1;
    // }
    // if (pte_addr_size > 0) {
    //     write_to_ivshmem(result->pte_addr, pte_addr_size, sizeof(head) / 8);
    // }
    // for (int i = 0; i < 4; i++) {
    //     INFO("%016llx %lld\n", result->pte_addr[i].addr, result->pte_addr[i].type);
    // }

    // uint64_t anon_pte_num = 0;
    // uint64_t file_pte_num = 0;
    // for (int i = 0; i < result->pte_cnt; i++) {
    //     if (result->pte_addr[i].type == ANON_PAGE) {
    //         anon_pte_num++;
    //     } else if (result->pte_addr[i].type == FILE_PAGE) {
    //         file_pte_num++;
    //     } else {
    //         ERR("unknown page type: %llu\n", result->pte_addr[i].type);
    //     }
    // }
    // INFO("A/F: %lld/%lld\n", anon_pte_num, file_pte_num);
    return 0;
}
