
#include "ivshmem-drv.h"

static int test_length = 64;
module_param(test_length, int, 0644);
MODULE_PARM_DESC(test_length, "narrow input string test_length");

struct ivshmem_data *ivshmem_dev;
void __iomem *pci_ioremap_bar_test(struct pci_dev *pdev, int bar) {
    struct resource *res = &pdev->resource[bar];

    /*
     * Make sure the BAR is actually a memory resource, not an IO resource
     */
    if (res->flags & IORESOURCE_UNSET || !(res->flags & IORESOURCE_MEM)) {
        pci_warn(pdev, "iaaa can't ioremap BAR %d: %pR\n", bar, res);
        return NULL;
    }
    printk(KERN_INFO "res->start = 0x%08llx\n", res->start);
    printk(KERN_INFO "shared memory size = 0x%08llx(%lluMB)\n", resource_size(res), resource_size(res) >> 20);

    // https://github.com/newfriday/misc/issues/8
    return ioremap(res->start, resource_size(res));
}

static int ivshmem_dump_devid(struct pci_dev *pdev) {
    pr_info("read str:%s\n", (char *)ivshmem_dev->hw_addr);
    return 0;
}

static int ivshmem_show(struct seq_file *m, void *v) {
    ivshmem_dump_devid(ivshmem_dev->pci_dev);
    return 0;
}

static int ivshmem_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, ivshmem_show, NULL);
}

/*
** This function will be called when we close the Device file
*/
static int ivshmem_proc_release(struct inode *inode, struct file *file) {
    return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t ivshmem_proc_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    ivshmem_dump_devid(ivshmem_dev->pci_dev);
    return 0;
}

char write_buffer[4096] = {0};
ssize_t ivshmem_proc_write(struct file *file, const char __user *buf, size_t len, loff_t *l) {
    int length;

    if (len > test_length + 1) {
        pr_info("the argument is too long. max size is %d\n", test_length);
        return -1;
    }

    length = strncpy_from_user(write_buffer, buf, len);
    write_buffer[length - 1] = '\0';
    pr_info("write str:%s\n", write_buffer);
    memcpy(ivshmem_dev->hw_addr, write_buffer, length);

    return len;
}

static const struct proc_ops ivshmem_proc_fops = {
    .proc_open = ivshmem_proc_open,
    .proc_read = ivshmem_proc_read,
    .proc_write = ivshmem_proc_write,
    .proc_release = ivshmem_proc_release,
};

static int ivshmem_dev_probe(struct pci_dev *pdev, const struct pci_device_id *ent) {
    int err = -EIO;

    dev_info(&pdev->dev, "ivshmem_dev probe ....\n");
    printk(KERN_INFO "ivshmem_dev probe ....\n");

    ivshmem_dev = kzalloc(sizeof(*ivshmem_dev), GFP_KERNEL);
    if (!ivshmem_dev) {
        dev_err(&pdev->dev, "Failed to allocate device data\n");
        err = -ENOMEM;
        goto err_out_mem;
    }
    ivshmem_dev->pci_dev = pdev;

    if ((err = pci_enable_device(pdev))) {
        dev_err(&pdev->dev, "Cannot enable ivshmem PCI device, aborting\n");
        goto err_out_free_dev;
    }

    if ((pci_resource_flags(pdev, IVSHMEM_REGISTER_BAR) & IORESOURCE_MEM) == 0 ||
        (pci_resource_flags(pdev, IVSHMEM_MEMORY_BAR) & IORESOURCE_MEM) == 0) {
        dev_err(&pdev->dev, "no memory in bar");
        goto err_out_disable_pdev;
    }

    if ((err = pci_request_regions(pdev, DRV_NAME))) {
        dev_err(&pdev->dev, "Cannot obtain PCI resources, aborting\n");
        goto err_out_disable_pdev;
    }

    ivshmem_dev->hw_addr = pci_ioremap_bar_test(pdev, IVSHMEM_MEMORY_BAR);
    if (!ivshmem_dev->hw_addr) {
        dev_err(&pdev->dev, "aaaa Cannot ioremap PCI resources, aborting\n");
        goto err_out_free_res;
    }

    ivshmem_dev->proc_parent = proc_mkdir(PROC_INTERFACE_NAME, NULL);
    if (ivshmem_dev->proc_parent == NULL) {
        pr_info("Error creating proc entry");
        goto err_out_free_proc;
    }

    /*Creating Proc entry under /proc/ivshmem_demo/bar2 */
    proc_create("bar2", 0666, ivshmem_dev->proc_parent, &ivshmem_proc_fops);
    ivshmem_dump_devid(pdev);
    pci_set_drvdata(pdev, ivshmem_dev);
    err = 0;
    dev_info(&pdev->dev, " ivshmem_dev_probe success\n");
    return err;

err_out_free_proc:
    proc_remove(ivshmem_dev->proc_parent);
err_out_free_res:
    pci_release_regions(pdev);
err_out_disable_pdev:
    pci_disable_device(pdev);
err_out_free_dev:
    kfree(ivshmem_dev);
err_out_mem:
    dev_err(&pdev->dev, " ivshmem_dev_probe fail\n");
    return err;
}
static void ivshmem_dev_remove(struct pci_dev *pdev) {
    proc_remove(ivshmem_dev->proc_parent);
    /* Free IO remapping */
    iounmap(ivshmem_dev->hw_addr);
    /* Free up the mem region */
    pci_release_regions(pdev);

    pci_disable_device(pdev);

    kfree(ivshmem_dev);
    ivshmem_dev = NULL;
    dev_info(&pdev->dev, "Removed\n");
}

static const struct pci_device_id ivshmem_pci_id_table[] = {{PCI_DEVICE(IVSHMEM_PCI_VENDOR_ID, IVSHMEM_PCI_DEVICE_ID)},
                                                            {0}};

MODULE_DEVICE_TABLE(pci, ivshmem_pci_id_table);

struct pci_driver ivshmem_demo_driver = {
    .name = DRV_NAME,
    .id_table = ivshmem_pci_id_table,
    .probe = ivshmem_dev_probe,
    .remove = ivshmem_dev_remove,
};

// module_pci_driver(ivshmem_demo_driver);

MODULE_DESCRIPTION("just for ivshmem demo driver");
MODULE_LICENSE("GPL");