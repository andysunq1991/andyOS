/*
 * =====================================================================================
 *       Filename:  timer.h
 *    Description:  定义 PIT(周期中断定时器) 相关函数
 *        Version:  1.0
 *        Created:  2018年03月06日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */
#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include "types.h"

void init_timer(uint32_t frequency);

#endif