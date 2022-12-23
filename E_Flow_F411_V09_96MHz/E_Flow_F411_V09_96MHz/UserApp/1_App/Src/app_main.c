#include "app_main.h"
/*	���������������	*/
#include "usb_device.h"

#define VECT_TAB_OFFSET         0x00008000U

unsigned long FreeRTOSRunTimeTicks = 0;
char InfoBuffer[512] = {0};
char InfoBuffer2[512]= {0};

extern SemaphoreHandle_t	sMotor_goBottom;
volatile uint8_t RX_LEN = 0;  		//����һ֡���ݵĳ���
volatile uint8_t RECV_END_FLAG = 0; //һ֡���ݽ�����ɱ�־
uint8_t RX_BUF[RX_BUFFER_SIZE]={0};	//�������ݻ�������

void Before_HAL_Init(void)
{
#if USE_BOOTLOADER == 1
		SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
		__enable_irq();
#endif
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-11
* �� �� ��: App_Init
* ����˵��: �����ʼ��
*
* ��    ��: 
*
* �� �� ֵ:
*			
*********************************************************************************************************
*/
void App_Init(void)
{
	PWR_ON();
#if USE_BOOTLOADER != 1
	HAL_Delay(2000);							//	�ȴ��ϵ�󴮿����ȶ�����
#endif
	MX_USB_DEVICE_Init();
	Lowpass_Filter_Init();						/*	�˲�����ʼ��	*/		
	Enable_ADC_DMA();							/*	Get Voltage	*/
	vgus_dev_t.dev_init();						/*	����������������	*/
	Init_Page_Objs();							/*	ҳ������ʼ��	*/
	Motor_Controller_Init();					/*	��������ź�����ʼ��	*/
	motor_initialize();							/*	��ʼ�����	*/
	Get_Fitting_Oil_Index();					/*	��ȡҺѹУ��ϵ��	*/
	TMY_DB_Init();								/*	���ݿ��ʼ��	*/
	Open_VGUS_BACKLIGHT();						/*	��������	*/
	VGUS_LCD_Reboot();							/*	��Ļ����	*/
	MX_WWDG_Init();
	if(Is_Autoback_Off() == false)
		xSemaphoreGive(sMotor_goBottom);		/*	˿�˻������ײ�	*/	
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
	HAL_WWDG_Refresh(hwwdg);
}
