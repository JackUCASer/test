#include "Error.h"
#include "datatype.h"
#include "MenuData.h"
#include "beep.h"
#include "battery.h"
#include "work_mode.h"


uint16_t HwFault_cnt=0;

/**********************************************************************************************************
*	�� �� ��: void CheckLowBattery()
*	����˵��: ���й����е������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20211224         
**********************************************************************************************************/
void CheckLowBattery()
{
    static uint8_t lowp_cnt = 0;
    uint16_t BatVol_mv;
    BatVol_mv = (ADC_ConvertedValue[0]*Ref_v)>>11;		//���������˵�ѹ��mv
    if(BatVol_mv < 2500)
    {
		if(ADC_ConvertedValue[0] > 100)					//���ֵ�ص�ѹ�����쳣�����ۼ�
		{
			lowp_cnt++;
		}
        if(lowp_cnt > 50)			//�ۻ�����
        {
            lowp_cnt = 0;
            sys.error = 1;
            sys.error_num = e_LowPower;
        }
    }
    else 
	{
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
    static	uint8_t over_cnt = 0,low_cnt = 0;
    /* ��������*/
    if(motor_iq >= 280 || motor_iq <= -280)
	{											//����������ֵ
        over_cnt++;
		if(over_cnt >= 50)
		{
			over_cnt = 50;
			low_cnt = 0;
			sys.error = 1;
			sys.error_num = e_OverCurrent;
		}
    }
    else if((motor_iq <= 200) && (motor_iq >= -300))
	{
		low_cnt++;
		if(low_cnt >= 50)
		{
			over_cnt = 0;
			low_cnt = 50;
		}
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
WorkMode_e  CheckHardwareFault(WorkMode_e  mode)
{
    CheckLowBattery();							//��������е�ص�ѹ�Ƿ����
    OverCurentProtect();						//�������Ƿ����

    if(sys.error==1)
	{											//�������󣬽������ģʽ
        mode = Fault_mode;
        ApexRing(0,0);
        Ring.long_ring = 1;
    }
    return mode;
}





























