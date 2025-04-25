
#pragma once

#include <asm/page.h>
#include <linux/mm.h>
#include <linux/pagewalk.h>
#include <linux/types.h>


struct pt_addr_t {
    uint64_t addr;
    // total addr has 8 bytes, and low 12 bits is not used
    // 0-1 bits is page type
    // 2-3 bits is ref
};

#define GET_PAGE_TYPE(addr) ((addr) & 0b11)
#define SET_PAGE_TYPE(addr, type) (addr |= ((type) & 0b11))

#define GET_PAGE_REF(addr) ((addr & 0b1111) >> 2)
#define SET_PAGE_REF(addr, ref) (addr |= ((ref) << 2))


typedef struct pt_addr_t pt_addr_t;

struct pt_result {
    uint64_t pte_num;
    pt_addr_t *pte_addr;  // physical address
    uint64_t pte_cnt;     // array len pointer
    uint64_t total_pte_num;
};

enum page_type {
    UNKNOWN_PAGE,
    FILE_PAGE,
    ANON_PAGE,
};

int send_to_host(struct pt_result *result);
int init_pt_result(struct pt_result *result);
int reset_pt_result(struct pt_result *result);
int free_pt_result(struct pt_result *result);