
////////////////////////////////////////////////////////////////////
/// @file delay.h
/// @brief us��ms��ʱ����������ʱ��SYSTICK����
/// 
/// �ļ���ϸ������	��ʼ�������delay_init(uint8_t SYSCLK)��SYSCLKΪϵͳʱ��
///					����delay_us(uint32_t nus)��delay_ms(uint16_t nms)��������ʵ��us��ms��ʱ��
/// 
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210811
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////

#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f4xx_hal.h"

void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif

