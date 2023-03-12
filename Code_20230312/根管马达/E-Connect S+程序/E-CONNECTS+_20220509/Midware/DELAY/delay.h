
////////////////////////////////////////////////////////////////////
/// @file delay.h
/// @brief us、ms延时，非阻塞延时，SYSTICK计数
/// 
/// 文件详细描述：	初始化需调用delay_init(uint8_t SYSCLK)，SYSCLK为系统时钟
///					调用delay_us(uint32_t nus)、delay_ms(uint16_t nms)函数，可实现us、ms延时，
/// 
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210811
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
////////////////////////////////////////////////////////////////////

#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f4xx_hal.h"

void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif

