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

int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();

	console_clear();
	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");
	asm volatile ("int $0x3");
	asm volatile ("int $0x4");
	init_timer(200);
	asm volatile ("sti");
	//panic("TEST");
	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
	printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
	
	show_memory_map();
	return 0;
}

