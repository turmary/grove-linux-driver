/*
 * Copyright (C) 2021
 * Peter Yang <turmary@126.com>
 *
 * Released under the GPLv2
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>

static inline void set_csselr(unsigned cache_selector)
{
	// ARMv7 only
	// asm volatile("mcr p15, 2, %0, c0, c0, 0" : : "r" (cache_selector));

	// ARMv8
	asm volatile("MSR CSSELR_EL1, %0" :: "r"(cache_selector));
}

static inline unsigned read_ccsidr(void)
{
	unsigned val;

	// ARMv7 only
	// asm volatile("mrc p15, 1, %0, c0, c0, 0" : "=r" (val));

	// ARMv8
	asm volatile("MRS %0, CCSIDR_EL1" : "=r" (val));
	return val;
}

static inline unsigned read_clidr(void)
{
	unsigned val;

	asm volatile("MRS %0, CLIDR_EL1" : "=r" (val));
	return val;
}

static int __init ccsidr_init(void) {
	// Must run in kernel mode
	// Assumptions: L1 (instructions + data), L2 (check your CPU manual)
	set_csselr(0);
	printk(KERN_INFO "CCSIDR L1 data cache = 0x%.8X\n", read_ccsidr());
	set_csselr(1);
	printk(KERN_INFO "CCSIDR L1 instruction cache = 0x%.8X\n", read_ccsidr());
	set_csselr(2);
	printk(KERN_INFO "CCSIDR L2 unified = 0x%.8X\n", read_ccsidr());
	printk(KERN_INFO "CLIDR             = 0x%.8X\n", read_clidr());
	return 0;
}

static void __exit ccsidr_exit(void) {
	printk(KERN_INFO "bye, CCSIDR!\n");
}

module_init(ccsidr_init);
module_exit(ccsidr_exit);

MODULE_LICENSE("GPL v3");
MODULE_DESCRIPTION("ARM Cache size inquiry");
MODULE_AUTHOR("Peter Yang <turmary@126.com>");

