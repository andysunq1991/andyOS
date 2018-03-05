/*
 * =====================================================================================
 *       Filename:  gdt.c
 *    Description:  全局描述符相关函数
 *        Version:  1.0
 *        Created:  2018年03月05日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "gdt.h"
#include "string.h"

#define GDT_LENGTH 5	//全局描述符表长度

gdt_entry_t gdt_entries[GDT_LENGTH];	//全局描述符表定义

gdt_ptr_t gdt_ptr;		//GDTR

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

extern uint32_t stack;	//声明内核栈地址

void init_gdt()
{
	gdt_ptr.limit = sizeof(gdt_entry_t)*(GDT_LENGTH-1);
	gdt_ptr.base = (uint32_t)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);//intel要求第一个描述符全为0
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9a, 0xcf);	//z指令段
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xcf);	//数据段
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xfa, 0xcf);	//用户模式代码段
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xf2, 0xcf);	//用户模式数据段

	gdt_flush((uint32_t)&gdt_ptr);	//加载全局描述符表地址到GPTR寄存器
}

// 全局描述符表构造函数，根据下标构造
// 参数分别是 数组下标、基地址、限长、访问标志，其它访问标志
/* 结构体定义如下：
typedef struct
{
	uint16_t limit_low;     // 段界限   15～0
	uint16_t base_low;      // 段基地址 15～0
	uint8_t  base_middle;   // 段基地址 23～16
	uint8_t  access;        // 段存在位、描述符特权级、描述符类型、描述符子类别
	uint8_t  granularity; 	// 其他标志、段界限 19～16
	uint8_t  base_high;     // 段基地址 31～24
} __attribute__((packed)) gdt_entry_t;
*/
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].base_low     = (base & 0xFFFF);
	gdt_entries[num].base_middle  = (base >> 16) & 0xFF;
	gdt_entries[num].base_high    = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low    = (limit & 0xFFFF);
	gdt_entries[num].granularity  = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access       = access;
}
