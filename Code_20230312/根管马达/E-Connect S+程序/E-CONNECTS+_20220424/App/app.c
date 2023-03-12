
////////////////////////////////////////////////////////////////////
/// @file app.c
/// @brief ����Ӧ�ó���
/// 
/// �ļ���ϸ������	
///					
///
///
///
///
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210825
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////

//	Ӧ�ò�
#include "app.h"
#include "../lvgl/lvgl.h"
#include "stm32f4xx_hal.h"
#include "lv_port_disp_template.h"
#include "Multiple_Styles.h"
#include "key_Task.h"
#include "preSetting.h"
#include "MenuData.h"
#include "work_mode.h"
#include "menu.h"
#include "apex.h"
//	Ӳ����
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "battery.h"
//	��׼��
#include "mp6570.h"
#include "customer_control.h"

static int disp_state_cnt = 0;
//static int disp_state_cnt_time = 0;
static u8 disp_id_num = 0;
static u8 disp_correct_num = 0;
static u16 disp_error_cnt = 0;

extern FUN_SEL sel;
extern IWDG_HandleTypeDef hiwdg;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim10;
extern 	uint8_t  F_SetMode;
extern uint8_t LVGL_ON_OFF;

unsigned char LVGL_Flag = 0;									// LVGL���������� (Task Handler)ִ�����ڣ�����ȡ5ms
unsigned char LVGL_TL_Flag = 0;									// LVGL���������� (Task Handler)ִ�����ڣ�����ȡ5ms
/**********************************************************************************************************
*	�� �� ��: void App_Init(void)
*	����˵��: ��ʼ��ϵͳ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void App_Init(void)
{
	uint16_t bl_flag = 0;
	delay_init(84);
	
	HAL_TIM_Base_Start_IT(&htim5); 								// 1ms��ʱ��
	HAL_TIM_Base_Start_IT(&htim10); 							// 2ms
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);    				// root
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);    				// ͨ������LCD��PWM����������LCD������
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);    				// beep
	HAL_ADC_Start_DMA(&hadc1,ADC_ConvertedValue,ADC_NUM_CH); 	// ��ʼ��ADC
	
	data_init();												// ���ݳ�ʼ������EEPROM�����ó�ʼģʽ���˵���ʾ��ʼ����	
	
	power_on();													// ����������������ģʽ�������������á�У׼

//	Lcd_Init();  												// ��ʼ��OLED
//	Get_KeyVaule();												// ������ʼ��
	LVGL_ON_OFF = 1;
	lv_port_disp_init();										// ��ʼ��UI�ӿ�
	Power_mode_select();
	lv_task_handler();        									//�û�ͼ�ν���ˢ��
	for(bl_flag = 0; bl_flag <= d_backlight[set.backlight]; bl_flag++)  				//����ʮ�α��⽥��
	{
		TIM2->CCR1 = bl_flag;
		delay_ms(1);
	}
	HAL_IWDG_Refresh(&hiwdg);									// ι��  2s���
	init_MC();
	Apex_cal();
	Battery_init();												// ��ȡ��ص�������
	
	HAL_Delay(100);
	HAL_IWDG_Refresh(&hiwdg);									//ι��  //2s���
	
	sys.FlagThetaBias = 0;
	Time_cnt = 0;												//�Զ��ػ���ʱ����
}

/**********************************************************************************************************
*	�� �� ��: void AppTest(void)
*	����˵��: ������ѭ�����ú���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
uint8_t LVGL_task_Flag = 0;
void AppTest(void)
{
//	Key_Scan();																				//����ɨ��
	
	SamBattery();																			//��ص�ѹ���
	if((LVGL_Flag >= 5))											//LVGL��ʱ����
	{

		lv_task_handler();
		LVGL_Flag = 0;	
		if(		(WorkMode == Rotory_mode)
			||	(WorkMode == Reciproc_mode)
			||	(WorkMode == Atc_mode)
			||	(WorkMode == Measure_mode)
			||	(WorkMode == RunWithApex_mode)
			||	(WorkMode == RecWithApex_mode)
			||	(WorkMode == ATRwithApex_mode))
		{
			LVGL_TL_Flag = 1;
		}else
		{
			LVGL_TL_Flag = 0;
		}
	}
	
	if(Time_cnt >= d_PowerOffTime[set.AutoPowerOffTime]*60000)
	{
		WorkMode = PowerOff_mode;
	}
	HAL_IWDG_Refresh(&hiwdg);		//ι��  2s���
	switch(WorkMode)
	{
		case Rotory_mode:		{ WorkMode = Rotory(WorkMode);		break;}					// �����˶�ģʽ��������ת����ת
		case Reciproc_mode:		{ WorkMode = Reciproc(WorkMode);	break;}					// �����˶�ģʽ
		case Atc_mode:			{ WorkMode = ATC_RUN(WorkMode);		break;}					// ATC�˶�ģʽ
		case Measure_mode:		{ WorkMode = Measure(WorkMode);		break;}					// ����ģʽ
		case RunWithApex_mode:	{ WorkMode = RunWithApex(WorkMode); break;}					// ����+����ģʽ
		case RecWithApex_mode:	{ WorkMode = RecWithApex(WorkMode);	break;}					// ����+����ģʽ
		case ATRwithApex_mode: 	{ WorkMode = ATCwithApex(WorkMode);	break;}					// ATC+����ģʽ
		case Standby_mode:		{ WorkMode = Standby(WorkMode);		break;}					// ����ģʽ	
		case Charging_mode:		{ WorkMode = Charging(WorkMode);	break;}					// ���ģʽ
		case Fault_mode:		{ WorkMode = Fault(WorkMode);		break;}					// ���д���ģʽ
		case PowerOff_mode:		{ WorkMode = PowerOff(WorkMode);	break;}					// �ػ�Ԥ��ģʽ
		case DispOff_mode:		{ WorkMode = DispOff(WorkMode);		break;}					// �ر���ʾģʽ
		case DisplayLogo_mode:	{ WorkMode = Displaylogo(WorkMode);	break;}					// ��ʾLOGOģʽ
		default:break;
	}	
}




