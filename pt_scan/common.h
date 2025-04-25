
#pragma once

#include <linux/module.h>

#define MODULE_NAME "pt_scan"

#define INFO(...) printk(KERN_INFO MODULE_NAME ": " __VA_ARGS__)
#define ERR(...) printk(KERN_ERR MODULE_NAME ": " __VA_ARGS__)