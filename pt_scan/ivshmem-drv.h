
#pragma once

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#define IVSHMEM_PCI_VENDOR_ID 0x1af4
#define IVSHMEM_PCI_DEVICE_ID 0x1110
#define DRV_NAME              "ivshmem_demo_driver"
#define IVSHMEM_REGISTER_BAR  0
#define IVSHMEM_MEMORY_BAR    2

#define PROC_INTERFACE_NAME   "ivshmem"
#define IVHSMEM_PROC_PATH     "/proc/ivshmem/bar2"
#define MAX_BUFFER_SIZE       (1024 * 1024 * 1024)  // 1GB

struct ivshmem_data {
    struct pci_dev *pci_dev;
    void __iomem *hw_addr;
    struct proc_dir_entry *proc_parent;
};

struct ivshmem_head {
    uint64_t id;
    uint64_t pte_num;
};
