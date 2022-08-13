#include "app_main.h"
/*	请在这里包含驱动	*/
#include "lcd_common.h"
#include "usb_device.h"

unsigned long FreeRTOSRunTimeTicks = 0;
char InfoBuffer[512] = {0};
char InfoBuffer2[512]= {0};

extern SemaphoreHandle_t	sMotor_goBottom;
volatile uint8_t RX_LEN = 0;  		//接收一帧数据的长度
volatile uint8_t RECV_END_FLAG = 0; //一帧数据接收完成标志
uint8_t RX_BUF[RX_BUFFER_SIZE]={0};	//接收数据缓存数组

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-11
* 函 数 名: app_init
* 功能说明: 外设初始化
*
* 形    参: 
*
* 返 回 值:
*			
*********************************************************************************************************
*/
void app_init(void)
{
	HAL_Delay(2000);
	MX_USB_DEVICE_Init();
	lp_filter_Init();							/*	滤波器初始化	*/		
	Enable_ADC_DMA();							/*	Get Voltage	*/
	vgus_dev_t.dev_init();						/*	串口屏，创建队列	*/
	motor_initialize();							/*	初始化电机	*/
	motor_controller_init();					/*	电机控制信号量初始化	*/
	get_fitting_oil_index();					/*	获取液压校正系数	*/
	tmy_database_init();
	
	vgus_lcd_reboot();							/*	屏幕重启	*/
	xSemaphoreGive(sMotor_goBottom);			/*	丝杆回退至底部	*/
}
