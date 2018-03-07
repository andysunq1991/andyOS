/*
 * =====================================================================================
 *       Filename:  vmm.c
 *    Description:  虚拟内存管理
 *        Version:  1.0
 *        Created:  2018年03月07日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "idt.h"
#include "vmm.h"
#include "string.h"
#include "debug.h"
#include "pmm.h"

//内核页目录区域
pgd_t pgd_kern[PGD_SIZE] __attribute__((aligned(PAGE_SIZE)));

//内核页表区域
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__((aligned)(PAGE_SIZE)));

void init_vmm()
{
	//0xc0000000这个地址在页目录的索引
	uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);

	uint32_t i = 0;
	uint32_t j = 0;

	for(i=kern_pte_first_idx,j=0; i<kern_pte_first_idx+PTE_COUNT; i++,j++)//初始化页目录
	{
		pgd_kern[i] = (uint32_t *)pte_kern[i] - PAGE_OFFSET | PAGE_PRESENT | PAGE_WRITE;
	}

	uint32_t *pte = (uint32_t *)pte_kern;
	//不映射第0页，便于追踪NULL指针
	for(i=1; i<PTE_COUNT*PAGE_SIZE; i++)//初始化页表
	{
		pte[i] = (i<<12) | PAGE_PRESENT | PAGE_WRITE;
	}

	uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;

	register_interrupt_handler(14, &page_fault);

	swith_pgd(pgd_kern_phy_addr);
}

void swith_pgd(uint32_t pd)
{
	asm volatile ("mov %0, %%cr3" : : "r" (pd));
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	if(!pte)
	{
		pte = (pte_t *)pmm_alloc_page();
		pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;

		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
		bzero(pte, PAGE_SIZE);
	}
	else
	{
		//转换到内核线性地址
		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pa & PAGE_MASK) | flags;

	asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now, uint32_t va)
{
		uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

	if (!pte) {
		return;
	}

	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

	pte[pte_idx] = 0;

	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	if (!pte) {
	      return 0;
	}
	
	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

	// 如果地址有效而且指针不为NULL，则返回地址
	if (pte[pte_idx] != 0 && pa) {
		 *pa = pte[pte_idx] & PAGE_MASK;
		return 1;
	}

	return 0;
}