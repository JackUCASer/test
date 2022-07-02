#include "app_main.h"
/*	请在这里包含驱动	*/
#include "lcd_common.h"
#include "usb_device.h"

unsigned long FreeRTOSRunTimeTicks = 0;
char InfoBuffer[512] = {0};
char InfoBuffer2[512]= {0};
uint8_t aRxBuf = 0;


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-04-20
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
	HAL_UART_Receive_IT(&huart2, &aRxBuf, 1);	/*	开启串口中断接收	*/
	motor_initialize();							/*	初始化电机	*/
	motor_controller_init();					/*	电机控制信号量初始化	*/
	get_fitting_oil_index();					/*	获取液压校正系数	*/
	
	vgus_lcd_reboot();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-04-20
* 函 数 名: HAL_UART_RxCpltCallback
* 功能说明: 串口中断的回调函数
*
* 形    参: 
*
* 返 回 值:
*			
*********************************************************************************************************
*/
#include "vgus_lcd_if.h"
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint16_t uiRevCnt = 0;
	static uint16_t uiRevStart = 0;
	static uint8_t data_start_flag = 0;
	static uint8_t data_rev_flag = 0;
	static uint8_t data_lenth = 0;
	
	if(huart->Instance == USART2){			/*	如果当前为串口2中断	*/
		ucUsartRevBuff[uiRevCnt] = aRxBuf;
		
		if(data_rev_flag == 1)
			data_lenth --;
		
		
		if(data_start_flag == 1){			/*	当前字节为待接收的数据长度，通知开始接收数据	*/
			data_lenth = ucUsartRevBuff[uiRevCnt];
			data_rev_flag = 1;
			data_start_flag = 0;
		}
		
		uiRevStart = (uiRevStart << 8) + ucUsartRevBuff[uiRevCnt];

		if(uiRevStart == *data_frame_start)	/*	接收到数据帧头，通知下一字节为数据长度	*/
			data_start_flag = 1;
		
		uiRevCnt ++;
		uiRevCnt &= USART_REV_MAX_BUF_SIZE;	/*	保证不溢出	*/
		
		if(data_lenth == 0){				/*	数据接收完毕	*/								
			data_rev_flag = 0;
			usUsartRevCurrentPos = uiRevCnt;/*	更新最大可读索引	*/
		}
		
		HAL_UART_Receive_IT(&huart2, &aRxBuf, 1);
	}else{									/*	其他串口中断，清空	*/
		UNUSED(huart);
	}
	
}



