#ifndef __APP_MAIN_H
#define __APP_MAIN_H

#include "string.h"
#include "usart.h"

//	Ӳ����
#include "power_control.h"		/*	PWR ON/OFF	*/
#include "get_voltage.h"		/*	��ѹ��ȡ	*/
#include "vgus_lcd_if.h"		/*	���Դ�����	*/
#include "motor_misc.h"			/*	���	*/
#include "eeprom.h"				/*	EEPROM	*/

/*	���ݽṹ���㷨��	*/
#include "lp_filter.h"			/*	��ͨ�˲�	*/
#include "hampel_filter.h"		/*	hampel�˲���	*/

/*	Ӧ�ò�	*/
#include "GL_Config.h"			/*	Global ����	*/
#include "user_data.h"			/*	User����	*/

/*	���̽ӿ�		*/
#include "page_calib.h"			/*	У��	*/
#include "page_aging.h"			/*	�ϻ�	*/
#include "page_mode.h"			/*	ģʽ	*/
#include "motor_controller.h"	/*	���������	*/
#include "page_setting.h"		/*	���ý���	*/
#include "lcd_display.h"		/*	*/
#include "alarm_module.h"		/*	����ϵͳ	*/

//	����������ϵͳ��Ϣ����
extern unsigned long FreeRTOSRunTimeTicks;
extern char InfoBuffer[512];
extern char InfoBuffer2[512];

//	����DMAר��
#define RX_BUFFER_SIZE 100
extern  volatile uint8_t RX_LEN ;  		//	����һ֡���ݵĳ���
extern volatile uint8_t RECV_END_FLAG; 	//	һ֡���ݽ�����ɱ�־
extern uint8_t RX_BUF[RX_BUFFER_SIZE];  //	�������ݻ�������

void Before_HAL_Init(void);
void app_init(void);



#endif
