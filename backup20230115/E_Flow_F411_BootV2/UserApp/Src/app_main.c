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

/*-------------------------���Ի�����-------------------------*/
static uint8_t goto_home_page[7] = {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, 0x00};
static uint8_t goto_upgrade_page[7] 	= {0xA5, 0x5A, 0x04, 0x80, 0x03, 0x00, VGUS_UPGRADE_PAGE_ID};
static uint8_t set_background_light[6]	= {0xA5, 0x5A, 0x03, 0x80, 0x01, 63};
static uint8_t wedget_cnt = 0;

#define PAGE_UPGRADE_ID		0x0C01			/*	��������ͼ����ʼID	*/
#define UPDATE_FLAG_ADDR	0x2F

/*-----------------------Bootloader���-----------------------*/
#define VECT_TAB_OFFSET         0x00000000U
user_app Jump_To_Application;		 		/* ��ת����ָ�� */
uint32_t JumpAddress;						/* ��ת��ַ */

int flag_iap_ok=0;
int flag_iap_fail=0;
uint32_t time_cnt=0;						//	��ʱ��
uint32_t start_time_cnt_flag=0;				//	������ʱ����־

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
	//	1. ��ȡ������Ϣ
	PWR_ON();											//	ʹ�ܿ���
	EEPROM_Read(UPDATE_FLAG_ADDR, &update_flag, 1);		//	��ȡEEPROM��־λ�Ƿ���Ҫ����
	
	//	2.1. �жϵ�ǰҳ���Ƿ�Ϊ��������
	if(update_flag != 1){	//	����Ҫ����
		if(((*(__IO uint32_t*)IN_FLASH_STAR) & 0x2FFE0000 ) == 0x20000000){	//�ж��Ƿ��Ѿ���ȷ������APP
			SysTick->CTRL = 0;							//	ʧ�ܵδ�ʱ��
			HAL_DeInit();								//	ʧ����������(��ѡ)
			HAL_NVIC_DisableIRQ(SysTick_IRQn);			//	ʧ�ܵδ�ʱ�ж�(��ѡ)
			HAL_NVIC_ClearPendingIRQ(SysTick_IRQn);		//	����δ�ʱ���жϱ�־λ(��ѡ)
			__disable_irq();							//	�ر����ж�
		
			JumpAddress = *(__IO uint32_t*) (IN_FLASH_STAR + 4);//	��ȡ�ж�������λ��ַ
			Jump_To_Application = (user_app) JumpAddress;		//	������ָ��
			__set_MSP(*(__IO uint32_t*) IN_FLASH_STAR );		//	ӳ���ַ
			Jump_To_Application();								//	����APP
		}else{
			HAL_Delay(3000);							//	�ȴ�ϵͳ�ȶ�
			while(HAL_UART_Transmit(&huart2, set_background_light, 6, 0xFFFF) != HAL_OK);
			MAL_Init(1);								//	Flash��ʽ����ģ���U��
			MX_USB_DEVICE_Init();						//	USB��ʼ��
			while(HAL_UART_Transmit(&huart2, goto_upgrade_page, 7, 0xFFFF) != HAL_OK);
		}
	}else{
		MAL_Init(1);									//	Flash��ʽ����ģ���U��
		MX_USB_DEVICE_Init();							//	USB��ʼ��
		while(HAL_UART_Transmit(&huart2, goto_upgrade_page, 7, 0xFFFF) != HAL_OK);
	}

}

void app_exec(void)
{
	static uint8_t update_flag = 0;
	if(flag_iap_ok == 1){			//	��������ɹ�
		start_time_cnt_flag = 1;
		flag_iap_ok = 0;
		EEPROM_Write(UPDATE_FLAG_ADDR, &update_flag, 1);//	���������־
	}
	
	if(start_time_cnt_flag){		//	��ʾ��������
		time_cnt ++;
		if(time_cnt > 40){
			time_cnt = 0;		
			write_two_82H_cmd(PAGE_UPGRADE_ID, wedget_cnt, (wedget_cnt +1)*5);	//	��ʾ��������
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

//	���������ڵ�ַ�Ŀռ䷢������
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
