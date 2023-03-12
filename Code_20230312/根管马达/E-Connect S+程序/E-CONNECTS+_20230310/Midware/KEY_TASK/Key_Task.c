
////////////////////////////////////////////////////////////////////
/// @file key_Task.c
/// @brief 按键任务处理函数
/// 
/// 文件详细描述：	
///					
///
///
///
///
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
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

WorkMode_e gTempMode;									//模式更改全局变量
uint8_t NeedFlash = 0;               					//跳转进入子菜单 刷新标志  >1刷新
//uint8_t language = EN;           						//语言标志
uint8_t KeyFuncIndex = 0;        						//索引
uint8_t LastKeyFuncIndex = 0;        					//上个索引
uint8_t LastKeyFuncIndex_sel = 6;        				//上个索引
uint8_t BlinkCtrl=0;
uint8_t Charge_ctrl = 0;
extern uint8_t ChargDisp_flag,ChargDisp_cnt;
extern  uint8_t Last_apex_pos;
extern KbdTabStruct KeyTab[];
void (*KeyFuncPtr)(); //按键功能指针
uint16_t Changing_disp_cnt = 0;

/**********************************************************************************************************
*	函 数 名: void Jump2Menu(uint8_t Index)
*	功能说明: 跳转到指定菜单
*	形    参: Index  指定的菜单索引
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void Jump2Menu(uint8_t Index)
{
    KeyFuncIndex = Index;
}

/**********************************************************************************************************
*	函 数 名: uint8_t Get_Index(void)
*	功能说明: 获取当前的菜单索引
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
uint8_t Get_Index(void)
{
   return KeyFuncIndex;
}
uint8_t Versions_Flag = 0;
/**********************************************************************************************************
*	函 数 名: WorkMode_e CheckKey_StandbyMode(WorkMode_e mode)
*	功能说明: 待机模式下的按键扫描
*	形    参: WorkMode_e mode	系统运行模式
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
WorkMode_e CheckKey_StandbyMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; 											/* 按键代码 */
	gTempMode = mode;
	IsKey = 1;
	ucKeyCode = Get_KeyVaule();
    if((ucKeyCode!=none) || (sys.Charging==1))
	{
		Time_cnt = 0;							//有按键按下，自动关机计时清零
	}
    if(-1==WaitTimeOut() && KeyFuncIndex<=50&& KeyFuncIndex>0 /*&& sys.DispBattFlag!=1*/ )		
	{			//判断超时条件：在参数设置模式（系统设置无超时）,待机模式下
			        /***********高转速扭矩限制**************/
        if(sel.Speed>s700 && sel.Torque>t20)		//750rpm 2.0Ncm
        {
           sel.Torque=t20;
        }
        MemorySave(mem.MemoryNum);		//保存参数
        Jump2Menu(TOP_MENU_INDEX);
        NeedFlash=1;
		sel_page_flash  = 1;
    }

	switch(ucKeyCode) 
	{
		/*******************************KEY_ON按键处理*****************************************************************************************/
		case key_on: 		{ 	//启动键,找出新的菜单状态编号
									Ring.short_ring = 1;
									TimeOutCnt=0;
									if((KeyFuncIndex >= 6) && (KeyFuncIndex <= 46))
									{
										LastKeyFuncIndex_sel = KeyFuncIndex;													//保存操作前的索引,用于sel设置记忆前次操作位置
									}
									KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
									KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																			

									break;
								}
		
		/*******************************KEY_SEL按键处理*****************************************************************************************/						
		case key_sel: 			{ 	//选择键,找出新的菜单状态编号
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
										(*KeyFuncPtr)();//执行当前按键的操作
									}
									
									break;
								}
		
		/*******************************KEY_UP按键处理*****************************************************************************************/
		case key_up: 		{ 	//向上键,找出新的菜单状态编号
									Ring.short_ring = 1;
									TimeOutCnt=0;
									LastKeyFuncIndex = KeyFuncIndex;												//保存操作前的索引
									KeyFuncIndex     = KeyTab[KeyFuncIndex].KeyUpState;
									KeyFuncPtr       = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																//�8葱械鼻鞍醇牟僮�
									Jump2Menu(LastKeyFuncIndex);
			
									break;
								}
		
		/*******************************KEY_DOWN按键处理*****************************************************************************************/
		case key_down: 	{ 	//向下键,找出新的菜单状态编号
									Ring.short_ring = 1;
									TimeOutCnt=0;
									LastKeyFuncIndex = KeyFuncIndex;													//保存操作前的索引
									KeyFuncIndex     = KeyTab[KeyFuncIndex].KeyDnState;
									KeyFuncPtr       = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																	//执行当前按键的操作
									Jump2Menu(LastKeyFuncIndex);														//恢复操作前索引
									break;
								}

		/*******************************KEY_SET组合按键处理*****************************************************************************************/
		case key_sel_long: 		{
									Ring.short_ring = 1;
									TimeOutCnt=0;
									if((mem.MemoryNum == 0) && (sel.OperaMode == 4) && (KeyFuncIndex == 0)) 
									{
										KeyFuncIndex = KeyTab[KeyFuncIndex].KeyLongSelState;
										KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
										sel_page_flash = 1;
										(*KeyFuncPtr)();//执行当前按键的操作
									}
									else 
									{
										KeyFuncIndex = KeyTab[KeyFuncIndex].KeyLongSelState;
										KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
										sel_page_flash = 1;
										if(KeyFuncIndex == 51)
										{
											Versions_Flag = 1;
											set_page_flash = 1;
										}
										(*KeyFuncPtr)();//执行当前按键的操作
									}
									
									break;
								}
		case key_sel_long_on: 	{
									gTempMode = PowerOff_mode;
									break;
								}

	//---------------------------------------------------------------------------
		default: 				//按键错误的处理
								IsKey = 0;
								break;
	}

	if(NeedFlash) 																					//刷新页面
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//执行当前按键的操作
		NeedFlash = 0;
	}

	
	if((sys.Charging == 1)&& (KeyFuncIndex == 0))
	{
		if(Changing_disp_cnt > 500)
		{
			DispBatery_charge(1, sys.BatteryLevel, sys.Charging);										//显示电池电量
			Changing_disp_cnt = 0;
		}	
	}else if((sys.Charging == 0) && (sys.DispBattFlag == 1) && (KeyFuncIndex == 0))
	{
		sys.DispBattFlag = 0;
		DispBatery_charge(1, sys.BatteryLevel, sys.Charging);										//显示电池电量
	}
	
	if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==0 && Charge_ctrl == 1)
	{
		Charge_ctrl = 0;
	}
    else if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==1 && Charge_ctrl == 0)
    {
			Charge_ctrl = 1;
			EndoRing(0,0);	
			sys.Charging = 1;		//检测外接电源引脚，高电平表示正在充电
			Ring.short_ring = 1;
			ChargDisp_flag = 0;			//清零充电显示标志
			ChargDisp_cnt = 0;				//清零计数
			gTempMode = Charging_mode;
    }

	mode = gTempMode;	
   
  return mode;
}
/**************************************************************
  *Function:   在运行模式下的按键扫描
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
extern uint8_t RoDebug_pw;										// 在运行模式下，检测按键（M10模式，速度1000，密码检测用）
WorkMode_e CheckKey_RunMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* 按键代码 */

	IsKey = 1;
	Time_cnt = 0;							//自动关机计时清零
	ucKeyCode = Get_KeyVaule();
 //   if(Key!=none)ClearClock();			//有按键按下，自动关机计时清零
	switch(ucKeyCode) 
	{
		case key_on: 	{ //启动键,找出新的菜单状态编号
							Ring.short_ring = 1;
							TimeOutCnt = 0;
							KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
							KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
							Last_apex_pos = 24;			//初始化根测位置
							RoDebug_pw = 0;
							stop();
							delay_ms(100);
							LOOPTIMER_DISABLE;
							EndoRing(0 ,0);
							Ring.long_ring = 0;

							NeedFlash = 1;
							sel_page_flash = 1;
							(*KeyFuncPtr)();//执行当前按键的操作
							mode = Standby_mode;
							break;
						}
		case key_sel: 	{ //启动键,找出新的菜单状态编号
							RoDebug_pw = 1;
							break;
						}
		case key_up: 	{ //启动键,找出新的菜单状态编号
							RoDebug_pw = 2;       
							break;
						}
		case key_down: 	{ //启动键,找出新的菜单状态编号
							RoDebug_pw = 3;    
							break;
						}
//---------------------------------------------------------------------------
		default: //按键错误的处理
									IsKey = 0;
									break;
	}
	if(NeedFlash) 																					//刷新页面
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//执行当前按键的操作
		NeedFlash = 0;
	}
  return mode;
}

/**************************************************************
  *Function:   在apex运行模式下的按键扫描
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
WorkMode_e CheckKey_RunWithApexMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* 按键代码 */

	IsKey = 1;
	Time_cnt = 0;							//自动关机计时清零
	ucKeyCode = Get_KeyVaule();
 //   if(Key!=none)ClearClock();			//有按键按下，自动关机计时清零
	switch(ucKeyCode) 
	{
    case key_on: { 	//启动键,找出新的菜单状态编号
					Ring.short_ring = 1;
					TimeOutCnt = 0;
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
					KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
					Last_apex_pos = 24;								//初始化根测位置
					stop();
					delay_ms(100);
					LOOPTIMER_DISABLE;
					
					NeedFlash = 1;
					sel_page_flash = 1;
					(*KeyFuncPtr)();								//执行当前按键的操作
					mode = Standby_mode;
					sys.FlagRunWithApex = 0;
					sys.ActiveShutRunning = 1;						//主动按下ON键，下次不自动根测启动，直到解除
					break;
				}
//---------------------------------------------------------------------------
		default: //按键错误的处理
					IsKey = 0;
					break;
	}	
	if(NeedFlash) 																					//刷新页面
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//执行当前按键的操作
		NeedFlash = 0;
	}
	return mode;
}


uint8_t CheckKey_Password(uint8_t pw)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* 按键代码 */

	IsKey = 1;
	ucKeyCode = Get_KeyVaule();
	if(ucKeyCode != none)
	{
		Time_cnt = 0;							//自动关机计时清零
//		ClearClock();							//有按键按下，自动关机计时清零
	}
	
	switch(ucKeyCode) 
	{
		case key_on: { //启动键,找出新的菜单状态编号
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 1;
										break;
									}
		case key_sel: { //启动键,找出新的菜单状态编号
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 2;
										break;
									}
		case key_up: { //启动键,找出新的菜单状态编号
										Ring.short_ring = 1;
										TimeOutCnt = 0;

										pw = 3;
										break;
									}
		case key_down: { //启动键,找出新的菜单状态编号
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 4;
										break;
									}


//---------------------------------------------------------------------------
    default: //按键错误的处理
									IsKey = 0;
									break;
	}
  return pw;
}

void NextOperate(uint8_t Index)			//仅适用于菜单查找
{
	KeyFuncIndex = Index;
	KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
	(*KeyFuncPtr)();//执行当前按键的操作
}


void InitLastIndex()
{
	LastKeyFuncIndex_sel=6;		// 初始化
}














