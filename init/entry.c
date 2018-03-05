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

#include "console.h"
#include "debug.h"

int kern_entry()
{
	init_debug();
	console_clear();
	print_color(rc_black, rc_green, "Hello, OS kernel!\n", );
	panic("TEST");
	return 0;
}

