
////////////////////////////////////////////////////////////////////
/// @file preSetting.c
/// @brief ϵͳ����������ǰ��Ԥ�����������ݳ�ʼ���ȵ�
/// 
/// �ļ���ϸ������	void power_on()����������⣬��ͬ������ϣ����벻ͬ��ģʽ
///					
///
///
///
///
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////

#include "datatype.h"
#include "key.h"
#include "beep.h"
#include "delay.h"
#include "lcd.h"
#include "../lvgl/lvgl.h"
#include "menudata.h"
#include "main.h"
#include "eeprom.h"
#include "datastorage.h"
#include "macros.h"
#include "delay.h"
#include "mp6570.h"
#include "control.h"
#include "customer_control.h"
#include "apex.h"
#include "preSetting.h"
#include "datastorage.h"
#include "work_mode.h"
#include "Menu.h"
#include "Multiple_Styles.h"

uint8_t F_SetMode = 0;								// =0:����ģʽ��=1:����ģʽ��=2:����ģʽ
uint8_t ReStoreCheck[2] = {0}; 						// EEPROM���ݸ��±�־�洢
extern uint8_t KeyFuncIndex ;  						// ������Menu.c����ʼ��ʾ����
extern IWDG_HandleTypeDef hiwdg;					// ϵͳ���Ź�
extern ADC_HandleTypeDef 	hadc1;					// ADת��


/**********************************************************************************************************
*	�� �� ��: void power_on()
*	����˵��: ������ͬʱ���¿����������ü�������ϵͳģʽ��ͬʱ���¿����������ϼ������ü������빤��ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210812         
**********************************************************************************************************/
void power_on(void)
{
	uint8_t key_cnt;
	HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_SET); 				// ϵͳά�ֹ���
	//HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_SET);					// RELAY ON �л�����׼
	
	//�������¿�������s���л��� ϵͳ����ģʽ
	while(	(HAL_GPIO_ReadPin( KEY_SEL_GPIO_Port,KEY_SEL_Pin) == 0) 				// SEL������
		  &&(HAL_GPIO_ReadPin( KEY_UP_GPIO_Port, KEY_UP_Pin)  == 1) 				// UP ������
		  &&(HAL_GPIO_ReadPin( KEY_ON_GPIO_Port, KEY_ON_Pin)  == 0))				// ON ������
	{
		HAL_IWDG_Refresh(&hiwdg);													// ι��  2s���
		HAL_Delay(10);
		key_cnt++;
		if(key_cnt > 50)
		{
			F_SetMode = 1;
			KeyFuncIndex = 51;	    																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									//����ָ����������
			break;
		}
	}
	
	key_cnt = 0;
	
	//s >  on ���빤��ģʽ
	while(	(HAL_GPIO_ReadPin( KEY_SEL_GPIO_Port,KEY_SEL_Pin)  == 0) 				// SEL������
		  &&(HAL_GPIO_ReadPin( KEY_UP_GPIO_Port, KEY_UP_Pin)   == 0)				// UP ������ 
		  &&(HAL_GPIO_ReadPin( KEY_ON_GPIO_Port, KEY_ON_Pin)   == 0))				// ON ������
	{
		HAL_IWDG_Refresh(&hiwdg);													// ι��  2s���
		HAL_Delay(10);
		key_cnt++;
		if(key_cnt > 50)
		{
			F_SetMode = 2;
			KeyFuncIndex = 124;	    																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									//����ָ����������
			break;
		}
	}	
	
	if((HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==1)&&(HAL_GPIO_ReadPin( KEY_SEL_GPIO_Port,KEY_SEL_Pin)  == 0))
	{
		SettingRead();
		if(set.EMC_SCR_PROTECT == 0)
		{
			set.EMC_SCR_PROTECT = 1;
		}else
		{
			set.EMC_SCR_PROTECT = 0;
		}
		SettingSave();
	}
	
	Ring.short_ring=1;						//BEEP����
	HAL_Delay(100);
	Ring.short_ring=1;

	Get_KeyVaule();																	//�ն��������ֵ	
}

/**********************************************************************************************************
*	�� �� ��: void Power_mode_select(void)
*	����˵��: ���ݿ������������벻ͬ����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210817         
**********************************************************************************************************/
void Power_mode_select(void)
{
	if(F_SetMode == 0 )
	{
		DispLogo();												//��ʾ���˿�������־
		sel_page_flash = 1;										//��ʾ��LOGO�󣬽������ģʽ����˳�ʼ��sel_page_flashΪ1
		sel_eeprom_flash = 0;									//����ˢ��
	
		set_page_flash = 0;										//setҳ��ˢ��
		set_eeprom_flash = 0;									//������setҳ����ص�EEPROMˢ��
		
		cal_page_flash = 0;										//calҳ��ˢ��
		cal_eeprom_flash = 0;									//������calҳ����ص�EEPROMˢ��
	}
	else if(F_SetMode == 1)
	{
		DispSetting();											//��ʾ�����á�
		sel_page_flash = 0;										//��ʾ��LOGO�󣬽������ģʽ����˳�ʼ��sel_page_flashΪ1
		sel_eeprom_flash = 0;									//����ˢ��
	
		set_page_flash = 1;										//setҳ��ˢ��
		set_eeprom_flash = 0;									//������setҳ����ص�EEPROMˢ��
		
		cal_page_flash = 0;										//calҳ��ˢ��
		cal_eeprom_flash = 0;									//������calҳ����ص�EEPROMˢ��
	}
	else if(F_SetMode == 2)
	{
		DispFactory();											//��ʾ������ģʽ��
		sel_page_flash = 0;										//��ʾ��LOGO�󣬽������ģʽ����˳�ʼ��sel_page_flashΪ1
		sel_eeprom_flash = 0;									//����ˢ��
	
		set_page_flash = 0;										//setҳ��ˢ��
		set_eeprom_flash = 0;									//������setҳ����ص�EEPROMˢ��
		
		cal_page_flash = 1;										//calҳ��ˢ��
		cal_eeprom_flash = 0;									//������calҳ����ص�EEPROMˢ��
	}	
}

/**********************************************************************************************************
*	�� �� ��: void data_init()
*	����˵��: ���ݳ�ʼ������EEPROM�����ó�ʼģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210817         
**********************************************************************************************************/
void data_init(void)
{
	static uint8_t i;
	
	WorkMode = DisplayLogo_mode;												//��ʼ��Ϊ��ʾLOGOģʽ

	MenuDataInit();																//�˵���ʾ���ݳ�ʼ��
	HAL_IWDG_Refresh(&hiwdg);													//ι��  2s���

	EEPROM_Read(RECOVERY_ADDR,ReStoreCheck,2);									//��ȡ���±�־
	if((ReStoreCheck[0] != 0xA1) && (ReStoreCheck[1] != 0xC1))					//�ж��Ƿ���ڱ�־��û�б�־�ͽ�����³���
	{
		Store2EepromInit();
		InitReName();
		ReStoreCheck[0] = 0xA1;
		ReStoreCheck[1] = 0xC1;
		EEPROM_Write(RECOVERY_ADDR,ReStoreCheck,2);								//������ɣ��ñ�־
	}
	SettingRead();	
	if(set.StartupMemory==1)
	{
		ReadLastMem();
	}else																		//����� Last �������ϴμ����λ��
	{
		mem.MemoryNum = 1;
	}
	MemoryRead(mem.MemoryNum);															//��ȡ����
}

/**********************************************************************************************************
*	�� �� ��: void Start_Adc(void)
*	����˵��: ����1��ADC
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210820         
**********************************************************************************************************/
void Start_Adc(void)
{
	uint32_t i = 0;
	HAL_ADC_Start_DMA(&hadc1,ADC_ConvertedValue,ADC_NUM_CH);
	while(i < 10)
	{
		i++;
	}
}

/**********************************************************************************************************
*	�� �� ��: void init_MC(void)
*	����˵��: ��ʼ���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210825         
**********************************************************************************************************/
extern uint16_t Round_Cnt;
void init_MC(void)
{
	init_registers();														//��ʼ��MP6570�ڲ��ļĴ���
	set_speed_slope(40);													//��ʼ���������б��
	//nSLEEP_SET; 															//enable pre-driver nsleep
	HAL_Delay(1);	
	//mp6570_enable();  													//enable mp6570
	HAL_Delay(2);
	mp6570_init(0x00);														//д��MP6570�ڲ��ļĴ���
	MP6570_WriteRegister(0x00,0x60,0x0000);									//ʹMP6570������������ģʽ
	HAL_Delay(10);															//delay_1ms 100ms wait for sensor to be stable	
	reset_parameters();
	Round_Cnt = 0;
//	LOOPTIMER_ENABLE; 														//�ڴ˴�����ʹ�ܣ�����ֿ������ɹ�����Ϊ�ж��е�SPI��������SPI��ͻ//enable loop time
//	HAL_Delay(20);
//	LOOPTIMER_DISABLE;
}

