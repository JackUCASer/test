#ifndef __APP_MAIN_H
#define __APP_MAIN_H

#include "string.h"
#include "usart.h"

//	硬件层
#include "power_control.h"		/*	PWR ON/OFF	*/
#include "w25qxx.h"				/*	Flash	*/
#include "get_voltage.h"		/*	电压获取	*/
#include "vgus_lcd_if.h"		/*	中显串口屏	*/
#include "motor_misc.h"			/*	电机	*/
#include "eeprom.h"				/*	EEPROM	*/

/*	数据结构与算法层	*/
#include "lp_filter.h"			/*	低通滤波	*/
#include "hampel_filter.h"		/*	hampel滤波器	*/

/*	应用层	*/
#include "GL_Config.h"			/*	Global 设置	*/
#include "user_data.h"			/*	User数据	*/

/*	进程接口		*/
#include "page_calib.h"			/*	校正	*/
#include "page_aging.h"			/*	老化	*/
#include "page_mode.h"			/*	模式	*/
#include "motor_controller.h"	/*	电机控制器	*/
#include "page_setting.h"		/*	设置界面	*/
#include "lcd_display.h"		/*	*/
#include "alarm_module.h"		/*	警报系统	*/

extern unsigned long FreeRTOSRunTimeTicks;
extern char InfoBuffer[512];
extern char InfoBuffer2[512];
extern uint8_t aRxBuf;

void app_init(void);



#endif
