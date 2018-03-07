/*
 * =====================================================================================
 *       Filename:  console.c
 *    Description:  屏幕操作的相关函数
 *        Version:  1.0
 *        Created:  2018年03月02日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#include "common.h"
#include "console.h"

static uint16_t *video_memory = (uint16_t *)(0xB8000+0xc0000000);		//VGA的显示缓冲起点是0xB8000
static uint8_t cursor_x = 0;		//屏幕光标的坐标
static uint8_t cursor_y = 0;

static void move_cursor()
{
	//屏幕是80字节宽
	uint16_t cursorLocation = cursor_y*80  +cursor_x;

	//在这里用到的两个内部寄存器 的编号位域分别表示光标位置1415的高位和低位
	outb(0x3D4, 14);	//告诉VGA我们要设置光标高字节
	outb(0x3D5, cursorLocation>>8);
	outb(0x3D4, 15);
	outb(0x3D5, cursorLocation);
}

void console_clear()
{
	uint8_t attribute_byte = (0<<4) | (15&0x0f);
	uint16_t blank = 0x20 | (attribute_byte <<8);
	int i = 0 ;

	for(i=0; i<80*25; i++)
	{
		video_memory[i] = blank;	//白底黑色的空格符填充整个屏幕区域
	}

	cursor_x = 0;
	cursor_y = 0;
	move_cursor();		//光标移至(0,0)
}

static void scroll()	//将后24行上移一行，最后一行清空
{
	uint8_t attribute_byte = (0<<4) | (15&0x0f);
	uint16_t blank = (0x20) | (attribute_byte<<8);
	int i = 0;

	//cursor_y>=25时换行
	if(cursor_y >= 25)
	{
		for(i=0*80; i<24*80; i++)		//将所有行的显示数据复制到上一行，第1行永远消失
		{
			video_memory[i] = video_memory[i+80];
		}

		for(i=24*80; i<25*80; i++)	//第25行用空格填充
		{
			video_memory[i] = blank;
		}

		cursor_y = 24;		//向上移动一行后，cursor_y=24
	}
}

void console_putc_color(char c, real_color_t back, real_color_t fore)
{
	uint8_t back_color = (uint8_t)back;
	uint8_t fore_color   = (uint8_t)fore;
	uint8_t attribute_byte = (back_color<<4) | (fore_color&0x0f);
	uint16_t attribute = attribute_byte<<8;

	//0x08为退格键的ASCII
	//0x09为TAB键的ASCII
	if(c==0x08&&cursor_x)
	{
		cursor_x--;
	}
	else if (c==0x09)
	{
		cursor_x = (cursor_x+8) & ~(8-1);
	}
	else if(c=='\r')
	{
		cursor_x = 0;
	}
	else if (c=='\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	else if (c>=' ')
	{
		video_memory[cursor_y*80+cursor_x] = c | attribute;
		cursor_x++;
	}

	if(cursor_x>=80)	//80个字符换行
	{
		cursor_y++;
		cursor_x = 0;
	}
	scroll();
	move_cursor();
}

void console_write(char *cstr)
{
	while(*cstr)
	{
		console_putc_color(*cstr++, rc_black, rc_white);
	}
}

void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
	while(*cstr)
	{
		console_putc_color(*cstr++, back, fore);
	}
}

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n, real_color_t back, real_color_t fore)
{
	int tmp;
	char noZeroes = 1;

	console_write_color("0x", back, fore);

	int i;
	for (i = 28; i >= 0; i -= 4) {
		tmp = (n >> i) & 0xF;
		if (tmp == 0 && noZeroes != 0) {
		      continue;
		}
		noZeroes = 0;
		if (tmp >= 0xA) {
		      console_putc_color(tmp-0xA+'a', back, fore);
		} else {
		      console_putc_color(tmp+'0', back, fore);
		}
	}
}

// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
	if (n == 0) {
		console_putc_color('0', back, fore);
		return;
	}

	uint32_t acc = n;
	char c[32];
	int i = 0;
	while (acc > 0) {
		c[i] = '0' + acc % 10;
		acc /= 10;
		i++;
	}
	c[i] = 0;

	char c2[32];
	c2[i--] = 0;

	int j = 0;
	while(i >= 0) {
	      c2[i--] = c[j++];
	}

	console_write_color(c2, back, fore);
}