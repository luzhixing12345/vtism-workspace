#include <asm/pgtable.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pagewalk.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/types.h>
#include <uapi/linux/sched/types.h>

#include "common.h"
#include "communicate.h"
#include "linux/sched/mm.h"
#include "page_table.h"


// T_interval = (SCAN_K * T_scan + SCAN_K_BASE) * T_scan + SCAN_BASE_INTERVAL

#define LINEAR

#define DEBUG_INFO
#define SCAN_K             80
#define SCAN_K_SCALE       10  // 缩放因子
#define SCAN_BASE_INTERVAL 2500
#define SCAN_K_BASE        50

static struct task_struct *page_walk_thread;
static struct hrtimer page_walk_timer;
static u64 thread_interval_ms = SCAN_BASE_INTERVAL;

extern struct mm_walk_ops walk_ops;
extern struct pci_driver ivshmem_demo_driver;
struct pt_result pt_res = {0};

int show_task_info(struct task_struct *task, struct mm_struct *mm, uint64_t data) {
    unsigned long anon_pages = get_mm_counter(mm, MM_ANONPAGES);
    unsigned long file_pages = get_mm_counter(mm, MM_FILEPAGES);
    unsigned long shmem_pages = get_mm_counter(mm, MM_SHMEMPAGES);
    unsigned long rss_pages = get_mm_rss(mm);
    unsigned long rss_gb = rss_pages * (PAGE_SIZE / 1024) / 1024 / 1024;
    unsigned long anon_gb = anon_pages * (PAGE_SIZE / 1024) / 1024 / 1024;
    unsigned long file_gb = file_pages * (PAGE_SIZE / 1024) / 1024 / 1024;
    unsigned long shmem_gb = shmem_pages * (PAGE_SIZE / 1024) / 1024 / 1024;
    if (rss_gb == 0 && data == 0) {
        return -1;
    }
    INFO("task: %s[%d] T(A/F/S): %lu(%lu/%lu/%lu) | vma: %lld\n",
         task->comm,
         task->pid,
         rss_gb,
         anon_gb,
         file_gb,
         shmem_gb,
         data);
    return 0;
};

static int pt_scan(struct task_struct *task, struct pt_result *result) {
    struct mm_struct *mm = get_task_mm(task);
    if (!mm)
        return -1;
    if (!mm->pgd) {
        ERR("Failed to get the mm->pgd\n");
        return -1;
    }
    struct vm_area_struct *vma;
    VMA_ITERATOR(vmi, mm, 0);
    mmap_read_lock(mm);
    for_each_vma(vmi, vma) {
        // walk_page_vma(vma, &walk_ops, result);
        walk_page_vma_opt(vma, &walk_ops, result);
    }
    // pr_info("%s: A/T: %lld/%lld\n", task->comm, result->pte_cnt, result->total_pte_num);
    mmap_read_unlock(mm);
    mmput(mm);
    // walk_page_range(mm, 0, ULONG_MAX, &walk_ops, result);
    return 0;
}

// 定义内核线程函数
static int page_walk_thread_fn(void *data) {
    while (!kthread_should_stop()) {
        struct task_struct *task;
        ktime_t start_time, end_time;  // 定义开始和结束时间
        s64 elapsed_ns;                // 计算耗时（纳秒）

        start_time = ktime_get();
        for_each_process(task) {
            pt_scan(task, &pt_res);
        }
        end_time = ktime_get();
        elapsed_ns = ktime_to_ns(ktime_sub(end_time, start_time));
        u64 scan_time_ms = elapsed_ns / 1000000;
        // T_interval = (SCAN_K * T_scan + SCAN_K_BASE) * T_scan + SCAN_BASE_INTERVAL
        thread_interval_ms = ((SCAN_K * scan_time_ms / SCAN_K_SCALE) + SCAN_K_BASE) * scan_time_ms +
        SCAN_BASE_INTERVAL;
        // thread_interval_ms = (SCAN_K * scan_time_ms / SCAN_K_SCALE) + SCAN_BASE_INTERVAL;
        send_data_to_host(&pt_res);

#ifdef DEBUG_INFO
        INFO("A/T: %lld/%lld (%lld ms) [%lld] [%llu GB]\n",
             pt_res.pte_cnt,
             pt_res.total_pte_num,
             scan_time_ms,
             thread_interval_ms,
             pt_res.total_pte_num * 4 / 1024 / 1024);
#endif
        reset_pt_result(&pt_res);
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
    }
    return 0;
}

// 高分辨率定时器回调函数
static enum hrtimer_restart page_walk_timer_fn(struct hrtimer *timer) {
    if (page_walk_thread)
        wake_up_process(page_walk_thread);

    // 重新启动定时器
    hrtimer_forward_now(timer, ms_to_ktime(thread_interval_ms));
    return HRTIMER_RESTART;
}

// 模块初始化函数
static int __init page_walk_init(void) {
    int ret = 0;
    ret = pci_register_driver(&ivshmem_demo_driver);
    if (ret < 0) {
        ERR("pci_register_driver failed\n");
        return ret;
    }
    INFO("register pci driver\n");
    ret = init_pt_result(&pt_res);
    if (ret < 0) {
        return ret;
    }
    // 创建并运行内核线程
    page_walk_thread = kthread_run(page_walk_thread_fn, NULL, "page_walk_thread");
    if (IS_ERR(page_walk_thread)) {
        ERR("Failed to create page walk thread.\n");
        return PTR_ERR(page_walk_thread);
    }
    INFO("Page walk thread created.\n");
    INFO("SCAN_K = %d\n", SCAN_K / SCAN_K_SCALE);
    // 设置线程优先级
    struct sched_param param = {.sched_priority = 80};  // 设置优先级为80
    page_walk_thread->policy = SCHED_FIFO;              // 设置调度策略为实时FIFO
    page_walk_thread->rt_priority = param.sched_priority;

    // 初始化并启动高分辨率定时器
    hrtimer_init(&page_walk_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    page_walk_timer.function = page_walk_timer_fn;
    hrtimer_start(&page_walk_timer, ms_to_ktime(thread_interval_ms), HRTIMER_MODE_REL);

    return ret;
}

// 模块卸载函数
static void __exit page_walk_exit(void) {
    if (page_walk_thread) {
        kthread_stop(page_walk_thread);  // 停止内核线程
        INFO("Page walk thread stopped.\n");
    }
    free_pt_result(&pt_res);
    hrtimer_cancel(&page_walk_timer);
    pci_unregister_driver(&ivshmem_demo_driver);
}

module_init(page_walk_init);
module_exit(page_walk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kamilu");
MODULE_DESCRIPTION("Kernel module to periodically walk page tables and output total size");
