#ifndef __DATATYPE_H
#define __DATATYPE_H
#include "stm32f4xx_hal.h"


#define 	ADC_NUM_CH 	3
#define 	Iq_num		16
extern	uint32_t ADC_ConvertedValue[ADC_NUM_CH];
extern uint8_t flag_cal;
typedef struct FUR
{
    unsigned char FurSitting;					//
    unsigned char beep;
    unsigned char AutoPWRtime;
    unsigned char hand;
    unsigned char SetTime;
    unsigned char calibration;
} Str_FUR;
/******* APEX****************/

extern uint32_t  	Time_cnt;
extern uint16_t  	TimeOutCnt;	
extern int 			flag_bl_on;
extern int 			flag_bl_off;
extern Str_FUR 		fur_set;
extern int 			flag_gui_run;
extern int 			flag_logo_show;
extern int 			motor_iq;
extern uint8_t 		Iq_cnt;							// iq缓存计数
extern uint8_t 		Read_ig_flag;					// =0,正常读取。=1，停止读取
extern int			Iq_buf[Iq_num];						// 电机IQ电流读取缓存

extern int 			blink_flag;
#endif


