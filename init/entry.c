/*
 * =====================================================================================
 *       Filename:  entry.c
 *    Description:  内核入口函数
 *        Version:  1.0
 *        Created:  2018年03月02日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "gdt.h"
#include "console.h"
#include "debug.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "string.h"
#include "vmm.h"

//内核初始化函数
void kern_init();

//开启分页机制后的Multiboot数据指针
multiboot_t *glb_mboot_ptr = NULL;

//开启分页机制后的内核栈
char kern_stack[STACK_SIZE];

//内核使用的临时页表和页目录，该地址必须是页对齐的地址，内存0-640k肯定是空闲的
__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_hign = (pgd_t *)0x3000;

//内核入口函数 
__attribute__((section(".init.text"))) void kern_entry()
{
	pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_hign | PAGE_PRESENT | PAGE_WRITE;

	//映射内核虚拟地址4MB到物理地址前4MB
	int i = 0 ;
	for(i=0; i<1024; i++)
	{
		pte_low[i] = (i<<12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//映射0x00000000-0x00400000的物理地址到虚拟地址0xc0000000-0xc0400000
	for (i=0; i<1024; i++)
	{
		pte_hign[i] = (i<<12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//设置临时页表
	asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));//cr3页对齐，即高20位有效

	uint32_t cr0;
	//启用分页机制
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));

	//切换内核栈
	uint32_t kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
	asm volatile ("mov %0, %%esp\n\t"
					"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

	//更新全局multiboot_t指针
	glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;

	//调用内核初始化函数
	kern_init();
}

void kern_init()
{
	init_debug();
	init_gdt();
	init_idt();

	console_clear();
	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");
	asm volatile ("int $0x3");
	asm volatile ("int $0x4");
	init_timer(200);
	//asm volatile ("sti");
	//panic("TEST");
	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
	printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
	
	show_memory_map();
	init_pmm();

	printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);

	uint32_t allc_addr = NULL;
	printk_color(rc_black, rc_light_brown, "Test Physical Memory Alloc :\n");
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);

	while(1)
	{
		asm volatile ("hlt");
	}
}