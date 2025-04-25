

#include "page_table.h"

#include <asm/pgtable.h>
#include <linux/highmem.h>
#include <linux/hugetlb.h>
#include <linux/mmu_notifier.h>
#include <linux/pagewalk.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "common.h"
#include "asm/pgtable_64_types.h"
#include "ivshmem-drv.h"
#include "linux/mm_types.h"

// 16KB
extern struct ivshmem_data *ivshmem_dev;

static inline int folio_lru_refs(struct folio *folio) {
    unsigned long flags = READ_ONCE(folio->flags);
    bool workingset = flags & BIT(PG_workingset);

    /*
     * Return the number of accesses beyond PG_referenced, i.e., N-1 if the
     * total number of accesses is N>1, since N=0,1 both map to the first
     * tier. lru_tier_from_refs() will account for this off-by-one. Also see
     * the comment on MAX_NR_TIERS.
     */
    return ((flags & LRU_REFS_MASK) >> LRU_REFS_PGOFF) + workingset;
}

int get_page_type(struct folio *folio) {
    // struct folio *folio = pfn_folio(pte_pfn(*pte));
    enum page_type page_type;
    if (unlikely(folio_test_ksm(folio))) {
        page_type = UNKNOWN_PAGE;
    } else if (folio_test_anon(folio)) {
        page_type = ANON_PAGE;
    } else {
        page_type = FILE_PAGE;
    }
    return page_type;
}

    // if (result->pte_cnt >= result->pte_num) {
int add_pte(struct pt_result *result, pte_t *pte) {
    //     uint64_t new_pte_num = result->pte_num * 2;
    //     pt_addr_t *new_pte_addr = (pt_addr_t *)vmalloc(new_pte_num * sizeof(pt_addr_t));
    //     if (!new_pte_addr) {
    //         return -ENOMEM;
    //     }
    //     memcpy(new_pte_addr, result->pte_addr, result->pte_num * sizeof(pt_addr_t));
    //     result->pte_num = new_pte_num;
    //     vfree(result->pte_addr);
    //     result->pte_addr = new_pte_addr;
    // }
    // INFO("pte_cnt: %lld, pte: %lx\n",result->pte_cnt, pte_val(*pte));
    result->pte_addr[result->pte_cnt].addr = (phys_addr_t)pte_pfn(*pte) << PAGE_SHIFT;
    // struct folio *folio = pfn_folio(pte_pfn(*pte));
    // SET_PAGE_TYPE(result->pte_addr[result->pte_cnt].addr, get_page_type(folio));
    // SET_PAGE_REF(result->pte_addr[result->pte_cnt].addr, folio_lru_refs(folio));
    // folio_clear_referenced(folio);
    // folio_test_clear_young(folio);
    // folio_put(folio);
    result->pte_cnt++;
    return 0;
}

int init_pt_result(struct pt_result *result) {
    // result->pte_num = DEFAULT_PT_SIZE;
    // result->pte_addr = (pt_addr_t *)vmalloc(result->pte_num * sizeof(pt_addr_t));
    // if (!result->pte_addr) {
    //     return -ENOMEM;
    // }
    result->pte_addr = (pt_addr_t *)ivshmem_dev->hw_addr + (sizeof(struct ivshmem_head) / 8);
    return 0;
}

int reset_pt_result(struct pt_result *result) {
    result->pte_cnt = 0;
    result->total_pte_num = 0;
    return 0;
}

int free_pt_result(struct pt_result *result) {
    // vfree(result->pte_addr);
    INFO("free pt result\n");
    return 0;
}

int pgd_entry(pgd_t *pgd, unsigned long addr, unsigned long next, struct mm_walk *walk) {
    if (!pgd_present(*pgd))
        return 0;

    struct mm_struct *mm = walk->mm;
    spin_lock(&mm->page_table_lock);
    pgdp_clear_young_notify(walk->vma, addr, pgd);
    spin_unlock(&mm->page_table_lock);
    return 0;
}

int p4d_entry(p4d_t *p4d, unsigned long addr, unsigned long next, struct mm_walk *walk) {
    if (!p4d_present(*p4d))
        return 0;
    // struct mm_struct *mm = walk->mm;
    // spin_lock(&mm->page_table_lock);
    // p4dp_clear_young_notify(walk->vma, addr, p4d);
    p4dp_test_and_clear_young(walk->vma, addr, p4d);
    // spin_unlock(&mm->page_table_lock);
    return 0;
}

int pud_entry(pud_t *pud, unsigned long addr, unsigned long next, struct mm_walk *walk) {
    if (!pud_present(*pud))
        return 0;
    // spinlock_t *ptl;
    // ptl = pud_lock(walk->mm, pud);
    // pudp_clear_young_notify(walk->vma, addr, pud);
    pudp_test_and_clear_young(walk->vma, addr, pud);
    // spin_unlock(ptl);
    return 0;
}
int pmd_entry(pmd_t *pmd, unsigned long addr, unsigned long next, struct mm_walk *walk) {
    if (!pmd_present(*pmd))
        return 0;
    // spinlock_t *ptl;
    // ptl = pmd_lock(walk->mm, pmd);
    // pmdp_clear_young_notify(walk->vma, addr, pmd);
    pmdp_test_and_clear_young(walk->vma, addr, pmd);
    // spin_unlock(ptl);
    return 0;
}

int pte_entry(pte_t *pte, unsigned long addr, unsigned long next, struct mm_walk *walk) {
    if (pte_none(*pte) || !pte_present(*pte)) {
        return 0;
    }
    struct pt_result *result = (struct pt_result *)walk->private;
    if (pte_young(*pte)) {
        // remove young bit
        // ptep_clear_young_notify(walk->vma, addr, pte);
        ptep_test_and_clear_young(walk->vma, addr, pte);
        if (add_pte(result, pte) < 0) {
            ERR("add pte failed\n");
            return -ENOMEM;
        }
    }
    // test_and_clear_page_young(page);
    // 	ClearPageReferenced(page);

    result->total_pte_num++;
    return 0;
}

struct mm_walk_ops walk_ops = {
    // .pgd_entry = pgd_entry,
    // .p4d_entry = p4d_entry,
    .pud_entry = pud_entry,
    .pmd_entry = pmd_entry,
    .pte_entry = pte_entry,
    .walk_lock = PGWALK_RDLOCK};
