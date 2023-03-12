
////////////////////////////////////////////////////////////////////
/// @file battery.c
/// @brief ϵͳ��ѹ�������
/// 
/// �ļ���ϸ������	void ResBatSam(void)��λ��ص�ѹ���
///					void SamBattery(void)��ѹ��⺯��
///
///
///
///
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210820
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////


#include "battery.h"
#include "presetting.h"
#include "beep.h"
#include "menu.h"
#include "datatype.h"
#include "preSetting.h"
#include "key_Task.h"

extern uint32_t ADC_ConvertedValue[ADC_NUM_CH];
uint16_t	Disp_bat_cnt = 0 ;
uint8_t 	Battery_COUNT_IT = 0;

/**********************************************************************************************************
*	�� �� ��: void ResBatSam(void)
*	����˵��: ��λ��ص�ѹ���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210820         
**********************************************************************************************************/
void ResBatSam(void)
{
	Disp_bat_cnt=0;
	Battery_COUNT_IT = 0;
}

/**********************************************************************************************************
*	�� �� ��: void Battery_init(void)
*	����˵��: ��س�ʼ���������ͼ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210820         
**********************************************************************************************************/
void Battery_init(void)
{
	uint16_t BatVol_mv;
 	
	do
	{
		Start_Adc();																//����ADC����ص�ѹ
		BatVol_mv = (ADC_ConvertedValue[0] * Ref_v) >> 11;							//���������˵�ѹ��mv
		
		//�������
		if(BatVol_mv < 3400)														//�������֣�Ϊ��ʾ��׼��
		{
			sys.BatteryLevel = 0;
		}
		else if((BatVol_mv >= 3400) && (BatVol_mv < 3600))
		{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
			sys.BatteryLevel = 1;
		}
		else if((BatVol_mv >= 3600) && (BatVol_mv < 3900))
		{
			sys.BatteryLevel = 2;
		}
		else if((BatVol_mv >= 3900) && (BatVol_mv < 4100))
		{
			sys.BatteryLevel = 3;
		}
	
		if(BatVol_mv > 4100)
		{
			sys.BatteryLevel = 4;
			sys.FullCharge   = 1;
		}
		else 
		{
			sys.FullCharge = 0;
		}
		
		sys.DispBattFlag = 1;														//���µ����ʾ��־λ
		
		if(sys.Charging != 1)
		{																			//���ڳ��״̬����ʾ

		}
	}while(0);
	
}
/**********************************************************************************************************
*	�� �� ��: void SamBattery(void)
*	����˵��: ������ѹ��⣬2s���1�ε�ص�ѹ��10ms����Ƿ���ȵ�
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1:
*	�༭����: 1: 20210820         
**********************************************************************************************************/
void SamBattery(void)
{
	uint16_t BatVol_mv; 
	uint8_t Index_state = 1;
 	
	static uint8_t ring_sw = 0;
	// if(Get_Index()==0||Get_Index()==3 || Get_Index()==4)Index_state=0;
	if(Battery_COUNT_IT >= 10)
	{
		Battery_COUNT_IT = 0;
		Disp_bat_cnt++;
		
		if((Disp_bat_cnt > 200) &&(0 == Get_Index()))
		{	//2s ˢ��һ��
			Disp_bat_cnt = 0;															//��������
			Start_Adc();																//����ADC����ص�ѹ
			BatVol_mv = (ADC_ConvertedValue[0] * Ref_v) >> 11;							//���������˵�ѹ��mv
			
			//�������
			if(BatVol_mv < 3400)														//�������֣�Ϊ��ʾ��׼��
			{
				sys.BatteryLevel = 0;
			}
			else if((BatVol_mv >= 3400) && (BatVol_mv < 3600))
			{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
				sys.BatteryLevel = 1;
			}
			else if((BatVol_mv >= 3600) && (BatVol_mv < 3900))
			{
				sys.BatteryLevel = 2;
			}
			else if((BatVol_mv >= 3900) && (BatVol_mv < 4100))
			{
				sys.BatteryLevel = 3;
			}
		
			if(BatVol_mv > 4100)
			{
				sys.BatteryLevel = 4;
				sys.FullCharge   = 1;
			}
			else 
			{
				sys.FullCharge = 0;
			}
			
			sys.DispBattFlag = 1;														//���µ����ʾ��־λ
			
			if(sys.Charging != 1)
			{																			//���ڳ��״̬����ʾ

			}
		}
		
		if((HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin) == 1)	&& (sys.Charging == 0))						//�� Menu.c ������ת��������
		{
			if(ring_sw == 1)
			{
				ring_sw = 0;
				Ring.short_ring = 1;
			}
			sys.Charging = 1;															//�����ӵ�Դ���ţ��ߵ�ƽ��ʾ���ڳ��
		}
		else if((HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin) == 0))
		{
			ring_sw = 1;
			sys.Charging = 0;
		}
	}
}








