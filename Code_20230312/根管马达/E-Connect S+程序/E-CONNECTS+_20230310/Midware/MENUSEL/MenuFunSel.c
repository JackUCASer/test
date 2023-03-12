
////////////////////////////////////////////////////////////////////
/// @file MenuFunSel.c
/// @brief �����������صĲ˵�����
/// 
/// �ļ���ϸ������	
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

#include "MenuFunc.h"
#include "Menu.h"
#include "MenuData.h"
#include "lcd.h"
#include "MenuDisp.h"
#include "datatype.h"
#include "MenuUI.h"
#include "../lvgl/lvgl.h"
#include "delay.h"
#include "datastorage.h"
#include "presetting.h"
#include "Multiple_Styles.h"
#include "key_Task.h"
//#include "customer_control.h"

extern IWDG_HandleTypeDef hiwdg;
SEL_MODE_e sel_mode_flag; 
/**********************************************************************************************************
*	�� �� ��: void DispMenuTop(void)
*	����˵��: ���������棬�տ���ʱ��ʾ������ʱ��ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTop(void)
{
	HAL_IWDG_Refresh(&hiwdg);										//ι��  2s���
	if(sel_page_flash)
	{
		MemoryRead(mem.MemoryNum);									//��ȡ�洢����
		ReadStandyReName(mem.MemoryNum);							//��ȡﱵ�����
		sel_page_flash = 0;
		
		Standy_init(mem.MemoryNum, sel);
		DispBatery_init(1, sys.BatteryLevel,sys.Charging);			//��ʾ��ص���
		DispVol(1, set.BeepVol);									//��ʾ����
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTopUp(void)
*	����˵��: �����������£���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopUp(void)
{
	mem.MemoryNum ++;
	if(mem.MemoryNum > 10) 
	{
		mem.MemoryNum = 0;
	}
	MemoryRead(mem.MemoryNum);										//��ȡ�洢����
	ReadStandyReName(mem.MemoryNum);								//��ȡﱵ�����
	Standy_init(mem.MemoryNum, sel);
	DispBatery_init(1, sys.BatteryLevel,sys.Charging);				//��ʾ��ص���
	DispVol(1, set.BeepVol);										//��ʾ����
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTopDown(void)
*	����˵��: �����������£���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopDown(void)
{
	mem.MemoryNum --;
	if(mem.MemoryNum < 0) 
	{
		mem.MemoryNum = 10;
	}
	MemoryRead(mem.MemoryNum);										//��ȡ�洢����
	ReadStandyReName(mem.MemoryNum);										//��ȡﱵ�����
	Standy_init(mem.MemoryNum, sel);	
	DispBatery_init(1, sys.BatteryLevel,sys.Charging);				//��ʾ��ص���
	DispVol(1, set.BeepVol);										//��ʾ����
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTopSel(void)
*	����˵��: �ɴ�������������ý���Ĺ��ɣ���ʼ�����ݺͳ�ʼ�����ý������õ���ҳ����ʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopSel(void)
{
	if(sel.OperaMode == 4)
	{
		Jump2Menu(0);
//		sel.AutoStart=0;
//	    sel.AutoStop=0;
//		
//		Jump2Menu(46);
//		sel_mode_flag = BarPosition_flag;								//��ʾ����ο���λ��
	}else
	{
		Jump2Menu(6);				
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTopON(void)
*	����˵��: ���������£����¡�ON����������������ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopON(void)
{
	Standy_ON(0,1);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTopLongSel(void)
*	����˵��: ���������£�������SEL������������ѡ�ģʽ���û���ѡ���õ�ﱣ�Ҳ�����Զ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopLongSel(void)
{
	if(sel.OperaMode == 4)
	{
		sel.AutoStart=0;
	    sel.AutoStop=0;
		
		Jump2Menu(46);
		sel_mode_flag = BarPosition_flag;								//��ʾ����ο���λ��
	}else
	{
		Jump2Menu(118);
	}
	NeedFlash = 1;
	set_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuOperationModeInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ����ģʽ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);	
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuOperationModeON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeON(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1))
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
	}else if(sel.OperaMode == 2)
	{
		/***********��ת��Ť������**************/
		if(sel.RecSpeed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t30;
			}	
		}
		if(sel.Torque < t30)
		{
			sel.Torque = t30;
		}
	}else if(sel.OperaMode == 4)
	{
		sel.AutoStart=1;
	    sel.AutoStop=0;
	}

	MemorySave(mem.MemoryNum);		
	Jump2Menu(0);
	sel_mode_flag = OperationMode_flag;				//��ʾ����ģʽ����
	NeedFlash = 1;
	sel_page_flash = 1;
 
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuOperationModeSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeSel(void)
{
	if(sel.OperaMode >= 0 && sel.OperaMode <= 3)
	{
		Jump2Menu(11);
		sel_mode_flag = Speed_flag;										//��ʾ�ٶȲ�������
	}
	else if(sel.OperaMode == 4)
	{
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����
	}
	else
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		MemorySave(mem.MemoryNum);		//
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����
	}
	NeedFlash = 1;
	sel_page_flash = 1;

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuOperationModeUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeUp(void)
{
	if((sel.FileSave == 0)||(sel.FileSave == 1))
	{
		sel.OperaMode++;
		if(sel.OperaMode >= sel.OperaMode_len)			//����EAL
		{
			sel.OperaMode = 0;
		}
	}

	Standy_sel_init(sel); 
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuOperationModeDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeDown(void)
{
	if((sel.FileSave == 0)||(sel.FileSave == 1))
	{
		sel.OperaMode --;
		if(sel.OperaMode<0)			//����EAL
		{
			sel.OperaMode = sel.OperaMode_len - 1;				//�ְ汾������EAL
		}
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuSpeedInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ�ٶ�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuSpeedON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuSpeedSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1))
	{
		Jump2Menu(16);
		sel_mode_flag = Torque_flag;									//��ʾŤ�ز�������
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
	}else if((sel.OperaMode == 2)||(sel.OperaMode == 3))
	{
		Jump2Menu(16);
		if(sel.OperaMode == 2)
		{
			if(sel.Torque < t30)
			{
				sel.Torque = t30;
			}
		}
		sel_mode_flag = Torque_flag;									//��ʾŤ�ز�������
		/***********��ת��Ť������**************/
		if(sel.RecSpeed > rs400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}	
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuSpeedUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedUp(void)
{
	if(sel.OperaMode == 2)
	{
		sel.RecSpeed++;
		if(sel.RecSpeed > sel.RecSpeed_len)
		{
			sel.RecSpeed=0;
		}
	}
	else if( sel.OperaMode == 3)
	{
		sel.RecSpeed++;
		if(sel.RecSpeed > sel.RecSpeed_len)
		{
			sel.RecSpeed = 0;
		}
	}
	else
	{
		sel.Speed++;
		if(sel.Speed > sel.Speed_len)
		{
			sel.Speed=0;
		}
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuSpeedDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedDown(void)
{
	if(sel.OperaMode == 2)
	{
		sel.RecSpeed--;
		if(sel.RecSpeed < 0)
		{
			sel.RecSpeed = sel.RecSpeed_len;
		}
	}
	else if(sel.OperaMode == 3)
	{
		sel.RecSpeed--;
		if(sel.RecSpeed < 0)
		{
			sel.RecSpeed = sel.RecSpeed_len;
		}
	}
	else
	{
		sel.Speed--;
		if(sel.Speed < 0)
		{
			sel.Speed = sel.Speed_len;
		}
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTorqueInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊת������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueInit(void)
{
 	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);;
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTorqueON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTorqueSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueSel(void)
{
	if(sel.OperaMode == 0 || sel.OperaMode == 1) 			//
	{
		Jump2Menu(31);
		sel_mode_flag = AutoStart_flag;									//��ʾ�Զ�������������
		
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed > s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
	}else if(sel.OperaMode == 2)
	{
		Jump2Menu(21);
		sel_mode_flag = CWAngle_flag;									//��ʾ���ǶȲ�������
		
		/***********��ת��Ť������**************/
		if(sel.RecSpeed > rs400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
	}else if(sel.OperaMode == 3)
	{
		Jump2Menu(21);
		sel_mode_flag = CWAngle_flag;									//��ʾ���ǶȲ�������
	}
	else
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed > s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
		
		MemorySave(mem.MemoryNum);		
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����		
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTorqueUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueUp(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1))
	{
		sel.Torque++;
		if(sel.Torque > sel.Torque_len)
		{
			sel.Torque = 0;
		}
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = 0;
			}
		}
		if(sel.Speed > s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = 0;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = 0;
			}
		}
		
	}else if(sel.OperaMode == 2)
	{
		sel.Torque++;
		
		if(sel.RecSpeed > rs400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t30;
			}
		}else
		{
			if(sel.Torque > sel.Torque_len-1)
			{
				sel.Torque = t30;
			}
		}
	}
	else if(sel.OperaMode == 3)
	{
		sel.AtrTorque++;
		if(sel.AtrTorque > sel.AtrTorque_len)
		{
			sel.AtrTorque = 0;
		}
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuTorqueDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueDown(void)
{
	if((sel.OperaMode==0) || (sel.OperaMode==1))
	{
		sel.Torque--;
		if(sel.Torque < 0)
		{
			sel.Torque = sel.Torque_len;
		}
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed > s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
	}else if(sel.OperaMode == 2)
	{
		sel.Torque--;
		
		if(sel.RecSpeed > rs400)
		{
			if(sel.Torque < t30)
			{
				sel.Torque = t40;
			}
		}else
		{
			if(sel.Torque < t30)
			{
				sel.Torque = sel.Torque_len-1;
			}
		}
	}
	else if(sel.OperaMode == 3)
	{
		sel.AtrTorque--;
		if(sel.AtrTorque<0)
		{
			sel.AtrTorque = sel.AtrTorque_len;
		}
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ���Ƕ�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		Jump2Menu(26);
		sel_mode_flag = CCWAngle_flag;									//��ʾ���ǶȲ�������
	}else if((sel.OperaMode == 2) || (sel.OperaMode == 3) ) 
	{
		if(sel.RecSpeed > s300)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		Jump2Menu(26);
		sel_mode_flag = CCWAngle_flag;									//��ʾ���ǶȲ�������
	}
	else
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		MemorySave(mem.MemoryNum);		
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleUp(void)
{
	sel.AngleCW++;
	if(sel.AngleCW > sel.AngleCW_len)
	{
		sel.AngleCW = 0;
	}
	if((sel.AngleCCW + sel.AngleCW) < 12)				//������С�ȽǶ�
	{				
		sel.AngleCW = 12 - sel.AngleCCW;
	}
	
	Standy_sel_init(sel);	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleDown(void)
{
	sel.AngleCW--;
	if(sel.AngleCW<0 || (sel.AngleCCW + sel.AngleCW) < 12)
	{
		sel.AngleCW = sel.AngleCW_len;
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCCWAngleInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ���Ƕ�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleInit(void)
{
 	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleON(void)
{
	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCCWAngleSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
		Jump2Menu(31);
		sel_mode_flag = AutoStart_flag;									//��ʾ�Զ�������������
	}else if((sel.OperaMode == 2) || (sel.OperaMode == 3) ) 
	{
		if(sel.RecSpeed > s300)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		Jump2Menu(31);
		sel_mode_flag = AutoStart_flag;									//��ʾ�Զ�������������
	}
	else
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
		MemorySave(mem.MemoryNum);	
		Jump2Menu(0);  
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����		
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCCWAngleUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleUp(void)
{
	sel.AngleCCW++;
	if(sel.AngleCCW > sel.AngleCCW_len)
	{
		sel.AngleCCW = 0;
	}
	if((sel.AngleCW + sel.AngleCCW) < 12)					//������С�ȽǶ�
	{				
		sel.AngleCCW = 12 - sel.AngleCW;
	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCCWAngleDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleDown(void)
{
	sel.AngleCCW--;
	if((sel.AngleCCW < 0) || (sel.AngleCW + sel.AngleCCW) < 12)
	{
		sel.AngleCCW = sel.AngleCCW_len;
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStartInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ�����Զ���ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartInit(void)
{
 	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCWAngleON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStartSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartSel(void)
{
	Jump2Menu(36);  
	sel_mode_flag = AutoStop_flag;									//��ʾ�Զ��رղ�������
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStartUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartUp(void)
{
//	if(sel.FileLibrary == 0)								//��m������£�����ģʽ������ѡ��
//	{
		sel.AutoStart++;
		if(sel.AutoStart > sel.AutoStart_len)				//����EAL
		{
			sel.AutoStart = 0;
		}
//	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStartDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartDown(void)
{
//	if(sel.FileLibrary == 0)								//��m������£�����ģʽ������ѡ��
//	{
		sel.AutoStart--;
		if(sel.AutoStart < 0)
		{
			sel.AutoStart = sel.AutoStart_len;				//�ְ汾������EAL
		}
//	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStopInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ�����Զ���ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStopON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopON(void)
{
	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStopSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
		Jump2Menu(41);  
		sel_mode_flag = ApicalMode_flag;								//��ʾ����㱣����������
	}else if((sel.OperaMode == 2)|| (sel.OperaMode == 3) ) 
	{
		if(sel.RecSpeed > s300)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		Jump2Menu(41);  
		sel_mode_flag = ApicalMode_flag;								//��ʾ����㱣����������
	}
	else
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
		MemorySave(mem.MemoryNum);	
		Jump2Menu(0); 
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����		
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStopUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopUp(void)
{
//	if(sel.FileLibrary == 0)								//��m������£�����ģʽ������ѡ��
//	{
		sel.AutoStop++;
		if(sel.AutoStop > sel.AutoStop_len)					//����EAL
		{
			sel.AutoStop = 0;
		}
//	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStopDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopDown(void)
{
// 	if(sel.FileLibrary == 0)								//��m������£�����ģʽ������ѡ��
//	{
		sel.AutoStop--;
		if(sel.AutoStop < 0)
		{
			sel.AutoStop = sel.AutoStop_len;				//�ְ汾������EAL
		}
//	} 
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApicalModeInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ�����Զ���ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApicalModeON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeON(void)
{
	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApicalModeSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
		Jump2Menu(46);
		sel_mode_flag = BarPosition_flag;								//��ʾ�����׼���������
	}else if((sel.OperaMode == 2) || (sel.OperaMode == 3) ) 
	{
		if(sel.RecSpeed > s300)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		Jump2Menu(46);
		sel_mode_flag = BarPosition_flag;								//��ʾ�����׼���������
	}
	else
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s700 && sel.Torque > t20)			//750rpm 2.0Ncm
		{		
			sel.Torque = t20;
		}
		MemorySave(mem.MemoryNum);	
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����        
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApicalModeUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeUp(void)
{
	if(sel.FileLibrary == 0)										//��m������£�����ģʽ������ѡ��
	{
		sel.ApicalMode++;
		if(sel.ApicalMode > sel.ApicalMode_len)						//����EAL
		{
			sel.ApicalMode = 0;
		}
	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApicalModeDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeDown(void)
{
	if(sel.FileLibrary == 0)										//��m������£�����ģʽ������ѡ��
	{
		sel.ApicalMode--;
		if(sel.ApicalMode < 0)
		{
			sel.ApicalMode = sel.ApicalMode_len;					//�ְ汾������EAL
		}
	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFlashBarPositionInit(void)
*	����˵��: ����ģʽ�£����ý����ʼ��	��Ϊ����λ������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(sel.FlashBarPosition > 8)
		{
			sel.FlashBarPosition = 8;
		}else if(sel.FlashBarPosition < 0)
		{
			sel.FlashBarPosition = 0;
		}
		Standy_sel_init(sel);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFlashBarPositionON(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionON(void)
{
	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFlashBarPositionSel(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionSel(void)
{
	/***********��ת��Ť������**************/
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********��ת��Ť������**************/
		if(sel.Speed > s400)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
		if(sel.Speed>s700)
		{
			if(sel.Torque > t20)
			{
				sel.Torque = t20;
			}
		}
		if(sel.Speed > s1200)
		{
			if(sel.Torque > t10)
			{
				sel.Torque = t10;
			}
		}
	}else if((sel.OperaMode == 2)|| (sel.OperaMode == 3) ) 
	{
		if(sel.RecSpeed > s300)
		{
			if(sel.Torque > t40)
			{
				sel.Torque = t40;
			}
		}
	}else if(sel.OperaMode == 4)
	{
		sel.AutoStart=1;
	    sel.AutoStop=0;
	}
	MemorySave(mem.MemoryNum);		
	Jump2Menu(0);
	sel_mode_flag = OperationMode_flag;								//��ʾ����ģʽ����
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFlashBarPositionUp(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionUp(void)
{
	sel.FlashBarPosition += 1;
	if(sel.FlashBarPosition > 8)
	{
		sel.FlashBarPosition = 0;
	}
	Standy_sel_init(sel);
	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFlashBarPositionDown(void)
*	����˵��: �ڴ���ģʽ�����ò������棬���¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionDown(void)
{
	sel.FlashBarPosition -= 1;
	if(sel.FlashBarPosition < 0)
	{
		sel.FlashBarPosition = 8;
	}
  	Standy_sel_init(sel);
};

/**************************************************************
*Function:
*PreCondition:
*Input:
*Output:
***************************************************************/
void DispMenuNoDefineInit(void)
{};
/**************************************************************
*Function:
*PreCondition:
*Input:
*Output:
***************************************************************/
void DispMenuNoDefineON(void)
{};
/**************************************************************
*Function:
*PreCondition:
*Input:
*Output:
***************************************************************/
void DispMenuNoDefineSel(void)
{};
/**************************************************************
*Function:
*PreCondition:
*Input:
*Output:
***************************************************************/
void DispMenuNoDefineUp(void)
{};
/**************************************************************
*Function:
*PreCondition:
*Input:
*Output:
***************************************************************/
void DispMenuNoDefineDown(void)
{};
	
	
	
