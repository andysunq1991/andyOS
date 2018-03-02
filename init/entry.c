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

#include "types.h"
#include "console.h"

int kern_entry()
{
	console_clear();
	console_write_color("Hello, OS kernel!\nhello\n", rc_black, rc_white);
	return 0;
}

