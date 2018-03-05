/*
 * =====================================================================================
 *       Filename:  debug.h
 *    Description:  内核级屏幕打印函数头文件
 *        Version:  1.0
 *        Created:  2018年03月02日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "console.h"
#include "vargs.h"
#include "elf.h"

#define assert(x,info)	\
		do 				\
		{				\
			if(!(x))	\
			{			\
				panic(info);\
			}				\
		}while(0)

//编译期间静态检测
#define	static_assert(x)	\
		switch (x) { case 0: case (x):;}

//初始化Debug信息
void init_debug();

//打印当前的函数调用栈信息
void panic(const char* msg);

//打印当前的段寄存器值
void print_cur_status();

//内核的打印函数
void printk(const char *format,...);

//内核的打印函数带颜色
void printk_color(real_color_t back, real_color_t fore, const char *format,...);