#include "app_main.h"
/*	请在这里包含驱动	*/
#include "usb_device.h"

#define VECT_TAB_OFFSET         0x00008000U

unsigned long FreeRTOSRunTimeTicks = 0;
char InfoBuffer[512] = {0};
char InfoBuffer2[512]= {0};

extern SemaphoreHandle_t	sMotor_goBottom;
volatile uint8_t RX_LEN = 0;  		//接收一帧数据的长度
volatile uint8_t RECV_END_FLAG = 0; //一帧数据接收完成标志
uint8_t RX_BUF[RX_BUFFER_SIZE]={0};	//接收数据缓存数组

void Before_HAL_Init(void)
{
#if USE_BOOTLOADER == 1
		SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
		__enable_irq();
#endif
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-11
* 函 数 名: App_Init
* 功能说明: 外设初始化
*
* 形    参: 
*
* 返 回 值:
*			
*********************************************************************************************************
*/
void App_Init(void)
{
	PWR_ON();
#if USE_BOOTLOADER != 1
	HAL_Delay(2000);							//	等待上电后串口屏稳定下来
#endif
	MX_USB_DEVICE_Init();
	Lowpass_Filter_Init();						/*	滤波器初始化	*/		
	Enable_ADC_DMA();							/*	Get Voltage	*/
	vgus_dev_t.dev_init();						/*	串口屏，创建队列	*/
	Init_Page_Objs();							/*	页面对象初始化	*/
	Motor_Controller_Init();					/*	电机控制信号量初始化	*/
	motor_initialize();							/*	初始化电机	*/
	Get_Fitting_Oil_Index();					/*	获取液压校正系数	*/
	TMY_DB_Init();								/*	数据库初始化	*/
	Open_VGUS_BACKLIGHT();						/*	开启背光	*/
	VGUS_LCD_Reboot();							/*	屏幕重启	*/
	MX_WWDG_Init();
	if(Is_Autoback_Off() == false)
		xSemaphoreGive(sMotor_goBottom);		/*	丝杆回退至底部	*/	
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
	HAL_WWDG_Refresh(hwwdg);
}
