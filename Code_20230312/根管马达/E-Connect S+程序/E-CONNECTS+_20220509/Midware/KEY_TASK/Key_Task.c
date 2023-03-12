
////////////////////////////////////////////////////////////////////
/// @file key_Task.c
/// @brief ������������
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
#include "delay.h"
#include "key_Task.h"
#include "key.h"
#include "beep.h"
#include "Menu.h"
#include "MenuData.h"
#include "Multiple_Styles.h"
#include "macros.h"
#include "mp6570.h"
#include "control.h"
#include "customer_control.h"
#include "DataStorage.h"

WorkMode_e gTempMode;									//ģʽ����ȫ�ֱ���
uint8_t NeedFlash = 0;               					//��ת�����Ӳ˵� ˢ�±�־  >1ˢ��
//uint8_t language = EN;           						//���Ա�־
uint8_t KeyFuncIndex = 0;        						//����
uint8_t LastKeyFuncIndex = 0;        					//�ϸ�����
uint8_t LastKeyFuncIndex_sel = 6;        				//�ϸ�����
uint8_t BlinkCtrl=0;
uint8_t Charge_ctrl = 0;
extern uint8_t ChargDisp_flag,ChargDisp_cnt;
extern  uint8_t Last_apex_pos;
extern KbdTabStruct KeyTab[];
void (*KeyFuncPtr)(); //��������ָ��
uint16_t Changing_disp_cnt = 0;

/**********************************************************************************************************
*	�� �� ��: void Jump2Menu(uint8_t Index)
*	����˵��: ��ת��ָ���˵�
*	��    ��: Index  ָ���Ĳ˵�����
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
void Jump2Menu(uint8_t Index)
{
    KeyFuncIndex = Index;
}

/**********************************************************************************************************
*	�� �� ��: uint8_t Get_Index(void)
*	����˵��: ��ȡ��ǰ�Ĳ˵�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
uint8_t Get_Index(void)
{
   return KeyFuncIndex;
}
uint8_t Versions_Flag = 0;
/**********************************************************************************************************
*	�� �� ��: WorkMode_e CheckKey_StandbyMode(WorkMode_e mode)
*	����˵��: ����ģʽ�µİ���ɨ��
*	��    ��: WorkMode_e mode	ϵͳ����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210824         
**********************************************************************************************************/
WorkMode_e CheckKey_StandbyMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; 											/* �������� */
	gTempMode = mode;
	IsKey = 1;
	ucKeyCode = Get_KeyVaule();
    if((ucKeyCode!=none) || (sys.Charging==1))
	{
		Time_cnt = 0;							//�а������£��Զ��ػ���ʱ����
	}
    if(-1==WaitTimeOut() && KeyFuncIndex<=50&& KeyFuncIndex>0 /*&& sys.DispBattFlag!=1*/ )		
	{			//�жϳ�ʱ�������ڲ�������ģʽ��ϵͳ�����޳�ʱ��,����ģʽ��
			        /***********��ת��Ť������**************/
        if(sel.Speed>s700 && sel.Torque>t20)		//750rpm 2.0Ncm
        {
           sel.Torque=t20;
        }
        MemorySave(mem.MemoryNum);		//�������
        Jump2Menu(TOP_MENU_INDEX);
        NeedFlash=1;
		sel_page_flash  = 1;
    }

	switch(ucKeyCode) 
	{
		/*******************************KEY_ON��������*****************************************************************************************/
		case key_on: 		{ 	//������,�ҳ��µĲ˵�״̬���
									Ring.short_ring = 1;
									TimeOutCnt=0;
									if((KeyFuncIndex >= 6) && (KeyFuncIndex <= 46))
									{
										LastKeyFuncIndex_sel = KeyFuncIndex;													//�������ǰ������,����sel���ü���ǰ�β���λ��
									}
									KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
									KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																			

									break;
								}
		
		/*******************************KEY_SEL��������*****************************************************************************************/						
		case key_sel: 			{ 	//ѡ���,�ҳ��µĲ˵�״̬���
									Ring.short_ring = 1;
									TimeOutCnt=0;
//									if((mem.MemoryNum == 0) && (sel.OperaMode == 4) && (KeyFuncIndex == 0)) 
//									{
//										
//									}
//									else 
									{
										KeyFuncIndex = KeyTab[KeyFuncIndex].KeySelState;
										KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
										(*KeyFuncPtr)();//ִ�е�ǰ�����Ĳ���
									}
									
									break;
								}
		
		/*******************************KEY_UP��������*****************************************************************************************/
		case key_up: 		{ 	//���ϼ�,�ҳ��µĲ˵�״̬���
									Ring.short_ring = 1;
									TimeOutCnt=0;
									LastKeyFuncIndex = KeyFuncIndex;												//�������ǰ������
									KeyFuncIndex     = KeyTab[KeyFuncIndex].KeyUpState;
									KeyFuncPtr       = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																//�8��е�ǰ�����Ĳ���
									Jump2Menu(LastKeyFuncIndex);
			
									break;
								}
		
		/*******************************KEY_DOWN��������*****************************************************************************************/
		case key_down: 	{ 	//���¼�,�ҳ��µĲ˵�״̬���
									Ring.short_ring = 1;
									TimeOutCnt=0;
									LastKeyFuncIndex = KeyFuncIndex;													//�������ǰ������
									KeyFuncIndex     = KeyTab[KeyFuncIndex].KeyDnState;
									KeyFuncPtr       = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																	//ִ�е�ǰ�����Ĳ���
									Jump2Menu(LastKeyFuncIndex);														//�ָ�����ǰ����
									break;
								}

		/*******************************KEY_SET��ϰ�������*****************************************************************************************/
		case key_sel_long: 		{
									Ring.short_ring = 1;
									TimeOutCnt=0;
									sys.FlagThetaBias = 1;
									
									break;
								}
		case key_sel_long_on: 	{
									gTempMode = PowerOff_mode;
									break;
								}

	//---------------------------------------------------------------------------
		default: 				//��������Ĵ���
								IsKey = 0;
								break;
	}

	if(NeedFlash) 																					//ˢ��ҳ��
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//ִ�е�ǰ�����Ĳ���
		NeedFlash = 0;
	}

	
	if((sys.Charging == 1)&& (KeyFuncIndex == 0))
	{
		if(Changing_disp_cnt > 500)
		{
			DispBatery_charge(1, sys.BatteryLevel, sys.Charging);										//��ʾ��ص���
			Changing_disp_cnt = 0;
		}	
	}else if((sys.Charging == 0) && (sys.DispBattFlag == 1) && (KeyFuncIndex == 0))
	{
		sys.DispBattFlag = 0;
		DispBatery_charge(1, sys.BatteryLevel, sys.Charging);										//��ʾ��ص���
	}
	
	if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==0 && Charge_ctrl == 1)
	{
		Charge_ctrl = 0;
	}
    else if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==1 && Charge_ctrl == 0)
    {
			Charge_ctrl = 1;
			EndoRing(0,0);	
			sys.Charging = 1;		//�����ӵ�Դ���ţ��ߵ�ƽ��ʾ���ڳ��
			Ring.short_ring = 1;
			ChargDisp_flag = 0;			//��������ʾ��־
			ChargDisp_cnt = 0;				//�������
			gTempMode = Charging_mode;
    }

	mode = gTempMode;	
   
  return mode;
}
/**************************************************************
  *Function:   ������ģʽ�µİ���ɨ��
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
WorkMode_e CheckKey_RunMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* �������� */

	IsKey = 1;
	Time_cnt = 0;							//�Զ��ػ���ʱ����
	ucKeyCode = Get_KeyVaule();
 //   if(Key!=none)ClearClock();			//�а������£��Զ��ػ���ʱ����
	switch(ucKeyCode) 
	{
		case key_on: 	{ //������,�ҳ��µĲ˵�״̬���
							Ring.short_ring = 1;
							TimeOutCnt = 0;
							KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
							KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
							Last_apex_pos = 24;			//��ʼ������λ��
							stop();
							delay_ms(1);
							LOOPTIMER_DISABLE;

							NeedFlash = 1;
							sel_page_flash = 1;
							(*KeyFuncPtr)();//ִ�е�ǰ�����Ĳ���
							mode = Standby_mode;
							break;
						}
//---------------------------------------------------------------------------
		default: //��������Ĵ���
									IsKey = 0;
									break;
	}
	if(NeedFlash) 																					//ˢ��ҳ��
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//ִ�е�ǰ�����Ĳ���
		NeedFlash = 0;
	}
  return mode;
}

/**************************************************************
  *Function:   ��apex����ģʽ�µİ���ɨ��
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
WorkMode_e CheckKey_RunWithApexMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* �������� */

	IsKey = 1;
	Time_cnt = 0;							//�Զ��ػ���ʱ����
	ucKeyCode = Get_KeyVaule();
 //   if(Key!=none)ClearClock();			//�а������£��Զ��ػ���ʱ����
	switch(ucKeyCode) 
	{
    case key_on: { 	//������,�ҳ��µĲ˵�״̬���
					Ring.short_ring = 1;
					TimeOutCnt = 0;
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
					KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
					Last_apex_pos = 24;								//��ʼ������λ��
					stop();
					delay_ms(1);
					LOOPTIMER_DISABLE;

					NeedFlash = 1;
					sel_page_flash = 1;
					(*KeyFuncPtr)();								//ִ�е�ǰ�����Ĳ���
					mode = Standby_mode;
					sys.FlagRunWithApex = 0;
					sys.ActiveShutRunning = 1;						//��������ON�����´β��Զ�����������ֱ�����
					break;
				}
//---------------------------------------------------------------------------
		default: //��������Ĵ���
					IsKey = 0;
					break;
	}	
	if(NeedFlash) 																					//ˢ��ҳ��
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//ִ�е�ǰ�����Ĳ���
		NeedFlash = 0;
	}
	return mode;
}


uint8_t CheckKey_Password(uint8_t pw)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* �������� */

	IsKey = 1;
	ucKeyCode = Get_KeyVaule();
	if(ucKeyCode != none)
	{
		Time_cnt = 0;							//�Զ��ػ���ʱ����
//		ClearClock();							//�а������£��Զ��ػ���ʱ����
	}
	
	switch(ucKeyCode) 
	{
		case key_on: { //������,�ҳ��µĲ˵�״̬���
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 1;
										break;
									}
		case key_sel: { //������,�ҳ��µĲ˵�״̬���
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 2;
										break;
									}
		case key_up: { //������,�ҳ��µĲ˵�״̬���
										Ring.short_ring = 1;
										TimeOutCnt = 0;

										pw = 3;
										break;
									}
		case key_down: { //������,�ҳ��µĲ˵�״̬���
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 4;
										break;
									}


//---------------------------------------------------------------------------
    default: //��������Ĵ���
									IsKey = 0;
									break;
	}
  return pw;
}

void NextOperate(uint8_t Index)			//�������ڲ˵�����
{
	KeyFuncIndex = Index;
	KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
	(*KeyFuncPtr)();//ִ�е�ǰ�����Ĳ���
}


void InitLastIndex()
{
	LastKeyFuncIndex_sel=6;		// ��ʼ��
}














