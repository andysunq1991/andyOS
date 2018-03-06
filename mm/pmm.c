/*
 * =====================================================================================
 *       Filename:  pmm.c
 *    Description:  内存管理
 *        Version:  1.0
 *        Created:  2018年03月06日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "pmm.h"
#include "muttiboot.h"
#include "debug.h"
#include "common.h"

void show_memory_map()
{
	uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
	uint32_t mmap_length = glb_mboot_ptr->mmap_length;

	printk("Memory map:\n");

	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for(mmap = (mmap_entry_t *)mmap_addr; mmap<mmap_addr + mmap_length; mmap++)
	{
		printk("base_addr = 0x%X%08x, length = 0x%X%08x, type = 0x%X\n",
			(uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low, (uint32_t)mmap->length_high,
			(uint32_t)mmap->length_low, (uint32_t)mmap->type);
	}
}
