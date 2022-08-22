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
//	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
//	__enable_irq();
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-11
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
	HAL_Delay(2000);							//	����ʱ�򿪣�����ʱ����
	MX_USB_DEVICE_Init();
	lp_filter_Init();							/*	�˲�����ʼ��	*/		
	Enable_ADC_DMA();							/*	Get Voltage	*/
	vgus_dev_t.dev_init();						/*	����������������	*/
	motor_initialize();							/*	��ʼ�����	*/
	motor_controller_init();					/*	��������ź�����ʼ��	*/
	get_fitting_oil_index();					/*	��ȡҺѹУ��ϵ��	*/
	tmy_database_init();
	
	vgus_lcd_reboot();							/*	��Ļ����	*/
	if(is_autoback_off() == false)
		xSemaphoreGive(sMotor_goBottom);		/*	˿�˻������ײ�	*/	
}
