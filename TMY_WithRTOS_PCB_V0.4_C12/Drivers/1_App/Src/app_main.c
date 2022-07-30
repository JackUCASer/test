#include "app_main.h"
/*	���������������	*/
#include "lcd_common.h"
#include "usb_device.h"

unsigned long FreeRTOSRunTimeTicks = 0;
char InfoBuffer[512] = {0};
char InfoBuffer2[512]= {0};
uint8_t aRxBuf = 0;


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-04-20
* �� �� ��: app_init
* ����˵��: �����ʼ��
*
* ��    ��: 
*
* �� �� ֵ:
*			
*********************************************************************************************************
*/
void app_init(void)
{
	HAL_Delay(2000);
	MX_USB_DEVICE_Init();
	lp_filter_Init();							/*	�˲�����ʼ��	*/		
	Enable_ADC_DMA();							/*	Get Voltage	*/
	vgus_dev_t.dev_init();						/*	����������������	*/
	HAL_UART_Receive_IT(&huart2, &aRxBuf, 1);	/*	���������жϽ���	*/
	motor_initialize();							/*	��ʼ�����	*/
	motor_controller_init();					/*	��������ź�����ʼ��	*/
	get_fitting_oil_index();					/*	��ȡҺѹУ��ϵ��	*/
	
	vgus_lcd_reboot();
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-04-20
* �� �� ��: HAL_UART_RxCpltCallback
* ����˵��: �����жϵĻص�����
*
* ��    ��: 
*
* �� �� ֵ:
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
	
	if(huart->Instance == USART2){			/*	�����ǰΪ����2�ж�	*/
		ucUsartRevBuff[uiRevCnt] = aRxBuf;
		
		if(data_rev_flag == 1)
			data_lenth --;
		
		
		if(data_start_flag == 1){			/*	��ǰ�ֽ�Ϊ�����յ����ݳ��ȣ�֪ͨ��ʼ��������	*/
			data_lenth = ucUsartRevBuff[uiRevCnt];
			data_rev_flag = 1;
			data_start_flag = 0;
		}
		
		uiRevStart = (uiRevStart << 8) + ucUsartRevBuff[uiRevCnt];

		if(uiRevStart == *data_frame_start)	/*	���յ�����֡ͷ��֪ͨ��һ�ֽ�Ϊ���ݳ���	*/
			data_start_flag = 1;
		
		uiRevCnt ++;
		uiRevCnt &= USART_REV_MAX_BUF_SIZE;	/*	��֤�����	*/
		
		if(data_lenth == 0){				/*	���ݽ������	*/								
			data_rev_flag = 0;
			usUsartRevCurrentPos = uiRevCnt;/*	�������ɶ�����	*/
		}
		
		HAL_UART_Receive_IT(&huart2, &aRxBuf, 1);
	}else{									/*	���������жϣ����	*/
		UNUSED(huart);
	}
	
}



