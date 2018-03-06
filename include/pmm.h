/*
 * =====================================================================================
 *       Filename:  pmm.h
 *    Description:  内存管理
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

//内核文件在内存中的起始和结束位置，在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];

//输出BIOS提供的内存布局
void show_memory_map();

#endif