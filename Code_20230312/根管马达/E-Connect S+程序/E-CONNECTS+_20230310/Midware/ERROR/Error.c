#include "Error.h"
#include "datatype.h"
#include "MenuData.h"
#include "beep.h"
#include "battery.h"
#include "work_mode.h"
#include "control.h"
#include "customer_control.h"
#include "macros.h"

uint16_t HwFault_cnt=0;
extern uint8_t LVGL_ON_OFF;
extern unsigned int reciprocate_sw;					// ����ȫ�ֱ���

/**********************************************************************************************************
*	�� �� ��: void CheckLowBattery()
*	����˵��: ���й����е������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: �����в����͵�����ֻ�ڴ���ʱ���
*	�༭����: 1: 20211224         
**********************************************************************************************************/
uint8_t Battery_low_flag = 0;					//��1���͵�ѹ����
void CheckLowBattery()
{
    static uint8_t lowp_cnt = 0;
    uint16_t BatVol_mv;
    BatVol_mv = (ADC_ConvertedValue[0]*Ref_v)>>11;		//���������˵�ѹ��mv
    if(BatVol_mv < 3200)
    {
		if(ADC_ConvertedValue[0] > 100)					//���ֵ�ص�ѹ�����쳣�����ۼ�
		{
			lowp_cnt++;
		}
        if(lowp_cnt > 50)			//�ۻ�����
        {
            lowp_cnt = 0;
//            sys.error = 1;
//            sys.error_num = e_LowPower;
			Battery_low_flag = 1;
        }
    }
    else 
	{
		Battery_low_flag = 0;
        lowp_cnt = 0;
    }
}
/**********************************************************************************************************
*	�� �� ��: void CheckLowBattery()
*	����˵��: ���й����е������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20211224         
**********************************************************************************************************/
void OverCurentProtect()
{
    static	uint16_t over_cnt = 0,low_cnt = 0;
	if((sel.OperaMode == 0)||(sel.OperaMode == 1)||(sel.OperaMode == 3))
	{
		if(motor_iq >= 350 || motor_iq <= -350)
		{											//����������ֵ
			over_cnt++;
			if(over_cnt >= 1000)
			{
				over_cnt = 1000;
				low_cnt = 0;
				sys.error = 1;
				sys.error_num = e_OverCurrent;
			}
		}
		else if((motor_iq <= 200) && (motor_iq >= -200))
		{
			low_cnt++;
			if(low_cnt >= 300)
			{
				over_cnt = 0;
				low_cnt = 300;
			}
		}
	}else if(sel.OperaMode == 2)
	{
		if(motor_settings.mode == 0)
		{	
			/* ��������*/
			if(motor_iq >= 350 || motor_iq <= -350)
			{											//����������ֵ
				over_cnt++;
				if(over_cnt >= 10000)
				{
					over_cnt = 10000;
					low_cnt = 0;
					sys.error = 1;
					sys.error_num = e_OverCurrent;
				}
			}
			else if((motor_iq <= 200) && (motor_iq >= -200))
			{
				low_cnt++;
				if(low_cnt >= 300)
				{
					over_cnt = 0;
					low_cnt = 300;
				}
			}
		}else if(motor_settings.mode == 1)
		{
			/* ��������*/
			if(motor_iq >= 370 || motor_iq <= -370)
			{											//����������ֵ
				over_cnt++;
				if(over_cnt >= 150)
				{
					over_cnt = 150;
					low_cnt = 0;
					stop();
					reciprocate_sw = 1;
				}
			}
			else if((motor_iq <= 280) && (motor_iq >= -280))
			{
				low_cnt++;
				if(low_cnt >= 50)
				{
					over_cnt = 0;
					low_cnt = 50;
				}
			}
		}
	}
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e  CheckHardwareFault(WorkMode_e  mode)
*	����˵��: ���й����е������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20211224         
**********************************************************************************************************/
WorkMode_e  CheckHardwareFault(WorkMode_e  mode)
{
//    CheckLowBattery();							//��������е�ص�ѹ�Ƿ����
    OverCurentProtect();						//�������Ƿ����

    if(sys.error == 1)
	{											//�������󣬽������ģʽ
        mode = Fault_mode;
        ApexRing(0,0);
        Ring.long_ring = 1;
		LVGL_ON_OFF = 1;
    }
    return mode;
}





























