#include "app_main.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usart.h"
#include <string.h>

#include "flash_if.h"
#include "platform_config.h"

#include "EEPROM.h"
#include "power_control.h"

/*-------------------------个性化设置-------------------------*/
static uint8_t goto_home_page[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x00};
static uint8_t goto_upgrade_page[7] 	= {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, VGUS_UPGRADE_PAGE_ID};
static uint8_t set_background_light[6]	= {0xA5, 0x5A, 0x03, 0x80, 0x01, 63};
static uint8_t wedget_cnt = 0;

#define PAGE_UPGRADE_ID		0x0C01			/*	升级界面图标起始ID	*/
#define UPDATE_FLAG_ADDR	0x2F

/*-----------------------Bootloader相关-----------------------*/
#define VECT_TAB_OFFSET         0x00000000U
user_app Jump_To_Application;		 		/* 跳转函数指针 */
uint32_t JumpAddress;						/* 跳转地址 */

int flag_iap_ok=0;
int flag_iap_fail=0;
uint32_t time_cnt=0;						//	计时器
uint32_t start_time_cnt_flag=0;				//	启动计时器标志

extern uint16_t MAL_Init(uint8_t lun);
extern USBD_HandleTypeDef hUsbDeviceFS;

void write_two_82H_cmd(uint16_t start_addr, uint16_t var1, uint16_t var2);

void Before_HAL_Init(void)
{
	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
	__enable_irq();
}


void app_init(void)
{
	uint8_t update_flag = 0;
	//	1. 读取升级信息
	PWR_ON();											//	使能开机
	EEPROM_Read(UPDATE_FLAG_ADDR, &update_flag, 1);		//	读取EEPROM标志位是否需要升级
	
	//	2.1. 判断当前页面是否为升级界面
	if(update_flag != 1){	//	不需要升级
		if(((*(__IO uint32_t*)IN_FLASH_STAR) & 0x2FFE0000 ) == 0x20000000){	//判断是否已经正确更新有APP
			SysTick->CTRL = 0;							//	失能滴答定时器
			HAL_DeInit();								//	失能所有外设(可选)
			HAL_NVIC_DisableIRQ(SysTick_IRQn);			//	失能滴答定时中断(可选)
			HAL_NVIC_ClearPendingIRQ(SysTick_IRQn);		//	清除滴答定时器中断标志位(可选)
			__disable_irq();							//	关闭总中断
		
			JumpAddress = *(__IO uint32_t*) (IN_FLASH_STAR + 4);//	获取中断向量复位地址
			Jump_To_Application = (user_app) JumpAddress;		//	区函数指针
			__set_MSP(*(__IO uint32_t*) IN_FLASH_STAR );		//	映射地址
			Jump_To_Application();								//	运行APP
		}else{
			HAL_Delay(3000);							//	等待系统稳定
			while(HAL_UART_Transmit(&huart2, set_background_light, 6, 0xFFFF) != HAL_OK);
			MAL_Init(1);								//	Flash格式化，模拟成U盘
			MX_USB_DEVICE_Init();						//	USB初始化
			while(HAL_UART_Transmit(&huart2, goto_upgrade_page, 7, 0xFFFF) != HAL_OK);
		}
	}else{
		MAL_Init(1);									//	Flash格式化，模拟成U盘
		MX_USB_DEVICE_Init();							//	USB初始化
		while(HAL_UART_Transmit(&huart2, goto_upgrade_page, 7, 0xFFFF) != HAL_OK);
	}

}

void app_exec(void)
{
	static uint8_t update_flag = 0;
	if(flag_iap_ok == 1){			//	如果升级成功
		start_time_cnt_flag = 1;
		flag_iap_ok = 0;
		EEPROM_Write(UPDATE_FLAG_ADDR, &update_flag, 1);//	清除升级标志
	}
	
	if(start_time_cnt_flag){		//	显示升级进程
		time_cnt ++;
		if(time_cnt > 40){
			time_cnt = 0;		
			write_two_82H_cmd(PAGE_UPGRADE_ID, wedget_cnt, (wedget_cnt +1)*5);	//	显示升级进度
			wedget_cnt ++;
			if(wedget_cnt == 0x14){
				HAL_Delay(2000);
				while(HAL_UART_Transmit(&huart2, goto_home_page, 7, 0xFFFF) != HAL_OK);
				start_time_cnt_flag = 0;
				HAL_NVIC_SystemReset();
			}
		}
		HAL_Delay(10);
	}
}

//	给两个相邻地址的空间发送数据
void write_two_82H_cmd(uint16_t start_addr, uint16_t var1, uint16_t var2)
{
	static uint8_t buf[10] = {0xA5, 0x5A, 0x07, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	buf[4] = (start_addr >> 8);
	buf[5] =  (start_addr &0x00FF);
	
	buf[6] = (var1 >> 8);
	buf[7] =  (var1 &0x00FF);
	
	buf[8] = (var2 >> 8);
	buf[9] =  (var2 &0x00FF);

	while(HAL_UART_Transmit(&huart2, buf, 10, 0xFFFF) != HAL_OK);
}
