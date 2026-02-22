#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/memory.h>

int init_module(void) {
    // PAGE_OFFSET is the start of the linear map (the offset we need)
    // PHYS_OFFSET is the start of physical RAM (usually 0x800000000 on FZ3)
    printk(KERN_INFO "----------------------------------------\n");
    printk(KERN_INFO "MELTDOWN_HELP: PAGE_OFFSET: 0x%llx\n", (unsigned long long)PAGE_OFFSET);
    printk(KERN_INFO "MELTDOWN_HELP: PHYS_OFFSET: 0x%llx\n", (unsigned long long)PHYS_OFFSET);
    printk(KERN_INFO "MELTDOWN_HELP: USE THIS OFFSET -> 0x%llx\n", (unsigned long long)(PAGE_OFFSET - PHYS_OFFSET));
    printk(KERN_INFO "----------------------------------------\n");
    return 0;
}

void cleanup_module(void) {}

MODULE_LICENSE("GPL");