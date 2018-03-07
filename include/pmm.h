/*
 * =====================================================================================
 *       Filename:  pmm.h
 *    Description:  物理内存管理
 *        Version:  1.0
 *        Created:  2018年03月06日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include "multiboot.h"

#define STACK_SIZE 8192		//线程栈的大小

#define PMM_MAX_SIZE 0x20000000	//支持的最大物理内存

#define PMM_PAGE_SIZE 0x1000	//物理内存页框大小

#define PAGE_MAX_SIZE (PMM_MAX_SIZE/PMM_PAGE_SIZE)	//最多支持的物理页面个数

#define PHY_PAGE_MASK 0xFFFFF000	//页掩码按照4096字节对齐

extern uint32_t phy_page_count;
//内核文件在内存中的起始和结束位置，在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];

//输出BIOS提供的内存布局
void show_memory_map();

void init_pmm();	//初始化物理内存管理

uint32_t pmm_alloc_page();	//返回一个内存页的物理地址

void pmm_free_page(uint32_t p);	//释放申请的内存

#endif