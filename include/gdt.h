/*
 * =====================================================================================
 *       Filename:  gdt.h
 *    Description:  全局描述符数据结构
 *        Version:  1.0
 *        Created:  2018年03月05日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_ 

#include "types.h"

//全局描述符类型
typedef struct gdt_entry_t
{
	uint16_t limit_low;	//段界限15~0
	uint16_t base_low;	//段基址15~0
	uint8_t  base_middle;//段基址23~16
	uint8_t  access;	//段存在位、描述符特权级、描述符类型、描述符子类别
	uint8_t  granularity;//其他标志、段界限19~16
	uint8_t  base_high;	//段基址31~24
}__attribute__(packed) gdt_entry_t;

typedef struct gdt_ptr_t
{
	uint16_t limit;	//全局描述符表限长
	uint32_t base;	//全局描述符表32位基地址
}__attribute__(packed) gdt_ptr_t;

//初始化全局描述符
void init_gdt();

//GDT加载到GDTR的函数汇编实现
extern void gdt_flush(uint32_t);

#endif	//end of INCLUDE_GDT_H_

