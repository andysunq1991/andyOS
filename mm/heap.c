/*
 * =====================================================================================
 *       Filename:  heap.c
 *    Description:  内核堆内存管理
 *        Version:  1.0
 *        Created:  2018年03月08日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "heap.h"
#include "debug.h"
#include "pmm.h"
#include "vmm.h"

static uint32_t heap_max = HEAP_START;

static header_t *heap_first = NULL;	//内存堆块管理头指针

//申请内存块
static void alloc_chunk(uint32_t start, uint32_t length);

//释放内存块
static void free_chunk(header_t *chunk);

//切分内存块
static void split(header_t *chunk, uint32_t len);

//合并内存块
static void glue(header_t *chunk);

viod init_heap()
{
	heap_first = 0;
}

void *kmalloc(uint32_t len)
{
	//所有申请的内存长度加上内存块管理结构的长度
	len += sizeof(header_t);

	header_t *cur_header = heap_first;
	header_t *prev_header = 0;

	while(cur_header)
	{
		if(cur_header->allocated==0 && cur_header->length>len)
		{
			split(cur_header,len);
			cur_header->allocated = 1;
			//返回的时候必须将指针挪到管理结构之后
			return (void *)((uint32_t)cur_header + sizeof(header_t));
		}
		//逐次推移指针
		prev_header = cur_header;
		cur_header = cur_header->next;
	}

	uint32_t chunk_start = NULL;
	//第一次运行该函数时则初始化内存块起始位置，之后根据当前指针加上申请长度即可
	if(prev_header)
	{
		chunk_start = (uint32_t)prev_header + prev_header->length;
	}
	else
	{
		chunk_start = HEAP_START
		heap_first = chunk_start;
	}

	//检查是否需要申请内存页
	alloc_chunk(chunk_start,len);
	cur_header = (header_t *)chunk_start;
	cur_header->prev = prev_header;
	cur_header->next = 0;
	cur_header->allocated = 1;
	cur_header->length = len;

	if(prev_header)//不是第一个块
	{
		prev_header->next = cur_header;
	}

	return (void *)(chunk_start+sizeof(header_t));
}

void kfree(void *p)
{
	header_t *header = (header_t *)((uint32_t)p-sizeof(header_t));
	header->allocated = 0;

	glue_chunk(header);
}

void alloc_chunk(uint32_t start, uint32_t len)
{
	while(start+len > heap_max)
	{
		uint32_t page = pmm_alloc_page();
		map(pgd_kern, heap_max, page, PAGE_PRESENT | PAGE_WRITE);
		heap_max += PGAE_SIZE;
	}
}

void free_chunk(header_t *chunk)
{
	if(chunk->prev == 0)
	{
		heap_first = 0;
	}
	else
	{
		chunk->prev->next = 0;
	}

	//空闲超过1页就释放
	while((heap_max-PGAE_SIZE) >= (uint32_t)chunk)
	{
		heap_max -= PGAE_SIZE;
		uint32_t page = 0;
		get_mapping(pgd_kern, heap_max, &page);
		umap(pgd_kern, heap_max);
		pmm_free_page(page);
	}
}

void split(header_t *chunk, uint32_t len)
{
	if(chunk->length-len > sizeof(header_t))//切分之前确保内存块够存放管理结构指针
	{
		header_t *new_chunk = (header_t *)(chunk->length + (uint32_t)chunk);
		new_chunk->prev = chunk;
		new_chunk->next = chunk->next;
		new_chunk->allocated = 0;
		new_chunk->length = chunk->length - len;

		chunk->next = new_chunk;
		chunk->length = len;
	}
}

void glue_chunk(header_t *chunk)
{
	// 如果该内存块后面有链内存块且未被使用则拼合
	if (chunk->next && chunk->next->allocated == 0) {
		chunk->length = chunk->length + chunk->next->length;
		if (chunk->next->next) {
			chunk->next->next->prev = chunk;
		}
		chunk->next = chunk->next->next;
	}

	// 如果该内存块前面有链内存块且未被使用则拼合
	if (chunk->prev && chunk->prev->allocated == 0) {
		chunk->prev->length = chunk->prev->length + chunk->length;
		chunk->prev->next = chunk->next;
		if (chunk->next) {
			chunk->next->prev = chunk->prev;
		}
		chunk = chunk->prev;
	}

	// 假如该内存后面没有链表内存块了直接释放掉
	if (chunk->next == 0) {
		free_chunk(chunk);
	}
}

void test_heap()
{
	printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

	void *addr1 = kmalloc(50);
	printk("kmalloc    50 byte in 0x%X\n", addr1);
	void *addr2 = kmalloc(500);
	printk("kmalloc   500 byte in 0x%X\n", addr2);
	void *addr3 = kmalloc(5000);
	printk("kmalloc  5000 byte in 0x%X\n", addr3);
	void *addr4 = kmalloc(50000);
	printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

	printk("free mem in 0x%X\n", addr1);
	kfree(addr1);
	printk("free mem in 0x%X\n", addr2);
	kfree(addr2);
	printk("free mem in 0x%X\n", addr3);
	kfree(addr3);
	printk("free mem in 0x%X\n\n", addr4);
	kfree(addr4);
}

