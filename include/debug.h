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

//内核的打印函数
void printk(const char *format,...);

//内核的打印函数带颜色
void printk_color(real_color_t back, real_color_t fore, const char *format,...);