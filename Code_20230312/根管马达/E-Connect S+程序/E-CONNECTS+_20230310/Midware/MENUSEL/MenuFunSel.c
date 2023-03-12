
////////////////////////////////////////////////////////////////////
/// @file MenuFunSel.c
/// @brief 与待机界面相关的菜单处理
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
*	函 数 名: void DispMenuTop(void)
*	功能说明: 待机主界面，刚开机时显示，待机时显示
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTop(void)
{
	HAL_IWDG_Refresh(&hiwdg);										//喂狗  2s会饿
	if(sel_page_flash)
	{
		MemoryRead(mem.MemoryNum);									//读取存储内容
		ReadStandyReName(mem.MemoryNum);							//读取锉的名字
		sel_page_flash = 0;
		
		Standy_init(mem.MemoryNum, sel);
		DispBatery_init(1, sys.BatteryLevel,sys.Charging);			//显示电池电量
		DispVol(1, set.BeepVol);									//显示音量
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuTopUp(void)
*	功能说明: 待机主界面下，向上索引
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopUp(void)
{
	mem.MemoryNum ++;
	if(mem.MemoryNum > 10) 
	{
		mem.MemoryNum = 0;
	}
	MemoryRead(mem.MemoryNum);										//读取存储内容
	ReadStandyReName(mem.MemoryNum);								//读取锉的名字
	Standy_init(mem.MemoryNum, sel);
	DispBatery_init(1, sys.BatteryLevel,sys.Charging);				//显示电池电量
	DispVol(1, set.BeepVol);										//显示音量
};

/**********************************************************************************************************
*	函 数 名: void DispMenuTopDown(void)
*	功能说明: 待机主界面下，向下索引
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopDown(void)
{
	mem.MemoryNum --;
	if(mem.MemoryNum < 0) 
	{
		mem.MemoryNum = 10;
	}
	MemoryRead(mem.MemoryNum);										//读取存储内容
	ReadStandyReName(mem.MemoryNum);										//读取锉的名字
	Standy_init(mem.MemoryNum, sel);	
	DispBatery_init(1, sys.BatteryLevel,sys.Charging);				//显示电池电量
	DispVol(1, set.BeepVol);										//显示音量
};

/**********************************************************************************************************
*	函 数 名: void DispMenuTopSel(void)
*	功能说明: 由待机界面进入设置界面的过渡，初始化数据和初始化设置界面中用到的页面样式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
//		sel_mode_flag = BarPosition_flag;								//显示根测参考点位置
	}else
	{
		Jump2Menu(6);				
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuTopON(void)
*	功能说明: 待机界面下，按下“ON”按键，进入运行模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopON(void)
{
	Standy_ON(0,1);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuTopLongSel(void)
*	功能说明: 待机界面下，长按“SEL”按键，进入选锉模式，用户可选内置的锉，也可以自定义锉
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTopLongSel(void)
{
	if(sel.OperaMode == 4)
	{
		sel.AutoStart=0;
	    sel.AutoStop=0;
		
		Jump2Menu(46);
		sel_mode_flag = BarPosition_flag;								//显示根测参考点位置
	}else
	{
		Jump2Menu(118);
	}
	NeedFlash = 1;
	set_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuOperationModeInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为运行模式设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuOperationModeON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeON(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1))
	{
		/***********高转速扭矩限制**************/
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
		/***********高转速扭矩限制**************/
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
	sel_mode_flag = OperationMode_flag;				//显示运行模式设置
	NeedFlash = 1;
	sel_page_flash = 1;
 
};

/**********************************************************************************************************
*	函 数 名: void DispMenuOperationModeSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeSel(void)
{
	if(sel.OperaMode >= 0 && sel.OperaMode <= 3)
	{
		Jump2Menu(11);
		sel_mode_flag = Speed_flag;										//显示速度参数设置
	}
	else if(sel.OperaMode == 4)
	{
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置
	}
	else
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置
	}
	NeedFlash = 1;
	sel_page_flash = 1;

};

/**********************************************************************************************************
*	函 数 名: void DispMenuOperationModeUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeUp(void)
{
	if((sel.FileSave == 0)||(sel.FileSave == 1))
	{
		sel.OperaMode++;
		if(sel.OperaMode >= sel.OperaMode_len)			//屏蔽EAL
		{
			sel.OperaMode = 0;
		}
	}

	Standy_sel_init(sel); 
};

/**********************************************************************************************************
*	函 数 名: void DispMenuOperationModeDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuOperationModeDown(void)
{
	if((sel.FileSave == 0)||(sel.FileSave == 1))
	{
		sel.OperaMode --;
		if(sel.OperaMode<0)			//屏蔽EAL
		{
			sel.OperaMode = sel.OperaMode_len - 1;				//现版本，屏蔽EAL
		}
	}
	
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuSpeedInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为速度设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuSpeedON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuSpeedSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuSpeedSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1))
	{
		Jump2Menu(16);
		sel_mode_flag = Torque_flag;									//显示扭矩参数设置
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
		sel_mode_flag = Torque_flag;									//显示扭矩参数设置
		/***********高转速扭矩限制**************/
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
*	函 数 名: void DispMenuSpeedUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuSpeedDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuTorqueInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为转矩设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuTorqueON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuTorqueSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuTorqueSel(void)
{
	if(sel.OperaMode == 0 || sel.OperaMode == 1) 			//
	{
		Jump2Menu(31);
		sel_mode_flag = AutoStart_flag;									//显示自动启动参数设置
		
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = CWAngle_flag;									//显示正角度参数设置
		
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = CWAngle_flag;									//显示正角度参数设置
	}
	else
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置		
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuTorqueUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
		/***********高转速扭矩限制**************/
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
*	函 数 名: void DispMenuTorqueDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
		/***********高转速扭矩限制**************/
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
*	函 数 名: void DispMenuCWAngleInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为正角度设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuCWAngleON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuCWAngleSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = CCWAngle_flag;									//显示负角度参数设置
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
		sel_mode_flag = CCWAngle_flag;									//显示负角度参数设置
	}
	else
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCWAngleUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuCWAngleUp(void)
{
	sel.AngleCW++;
	if(sel.AngleCW > sel.AngleCW_len)
	{
		sel.AngleCW = 0;
	}
	if((sel.AngleCCW + sel.AngleCW) < 12)				//限制最小等角度
	{				
		sel.AngleCW = 12 - sel.AngleCCW;
	}
	
	Standy_sel_init(sel);	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCWAngleDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuCCWAngleInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为负角度设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuCWAngleON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleON(void)
{
	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCCWAngleSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = AutoStart_flag;									//显示自动启动参数设置
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
		sel_mode_flag = AutoStart_flag;									//显示自动启动参数设置
	}
	else
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置		
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCCWAngleUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuCCWAngleUp(void)
{
	sel.AngleCCW++;
	if(sel.AngleCCW > sel.AngleCCW_len)
	{
		sel.AngleCCW = 0;
	}
	if((sel.AngleCW + sel.AngleCCW) < 12)					//限制最小等角度
	{				
		sel.AngleCCW = 12 - sel.AngleCW;
	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCCWAngleDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuAutoStartInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为根测自动开始设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuCWAngleON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStartSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartSel(void)
{
	Jump2Menu(36);  
	sel_mode_flag = AutoStop_flag;									//显示自动关闭参数设置
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStartUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartUp(void)
{
//	if(sel.FileLibrary == 0)								//在m程序号下，工作模式可自由选择
//	{
		sel.AutoStart++;
		if(sel.AutoStart > sel.AutoStart_len)				//屏蔽EAL
		{
			sel.AutoStart = 0;
		}
//	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStartDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStartDown(void)
{
//	if(sel.FileLibrary == 0)								//在m程序号下，工作模式可自由选择
//	{
		sel.AutoStart--;
		if(sel.AutoStart < 0)
		{
			sel.AutoStart = sel.AutoStart_len;				//现版本，屏蔽EAL
		}
//	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStopInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为根测自动开始设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuAutoStopON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopON(void)
{
	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStopSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = ApicalMode_flag;								//显示根测点保护参数设置
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
		sel_mode_flag = ApicalMode_flag;								//显示根测点保护参数设置
	}
	else
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置		
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStopUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopUp(void)
{
//	if(sel.FileLibrary == 0)								//在m程序号下，工作模式可自由选择
//	{
		sel.AutoStop++;
		if(sel.AutoStop > sel.AutoStop_len)					//屏蔽EAL
		{
			sel.AutoStop = 0;
		}
//	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStopDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuAutoStopDown(void)
{
// 	if(sel.FileLibrary == 0)								//在m程序号下，工作模式可自由选择
//	{
		sel.AutoStop--;
		if(sel.AutoStop < 0)
		{
			sel.AutoStop = sel.AutoStop_len;				//现版本，屏蔽EAL
		}
//	} 
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApicalModeInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为根测自动开始设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuApicalModeON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeON(void)
{
	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApicalModeSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeSel(void)
{
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********高转速扭矩限制**************/
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
		sel_mode_flag = BarPosition_flag;								//显示根测基准点参数设置
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
		sel_mode_flag = BarPosition_flag;								//显示根测基准点参数设置
	}
	else
	{
		/***********高转速扭矩限制**************/
		if(sel.Speed > s700 && sel.Torque > t20)			//750rpm 2.0Ncm
		{		
			sel.Torque = t20;
		}
		MemorySave(mem.MemoryNum);	
		Jump2Menu(0);
		sel_mode_flag = OperationMode_flag;								//显示运行模式设置        
	}
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApicalModeUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeUp(void)
{
	if(sel.FileLibrary == 0)										//在m程序号下，工作模式可自由选择
	{
		sel.ApicalMode++;
		if(sel.ApicalMode > sel.ApicalMode_len)						//屏蔽EAL
		{
			sel.ApicalMode = 0;
		}
	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApicalModeDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuApicalModeDown(void)
{
	if(sel.FileLibrary == 0)										//在m程序号下，工作模式可自由选择
	{
		sel.ApicalMode--;
		if(sel.ApicalMode < 0)
		{
			sel.ApicalMode = sel.ApicalMode_len;					//现版本，屏蔽EAL
		}
	}
	Standy_sel_init(sel);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFlashBarPositionInit(void)
*	功能说明: 待机模式下，设置界面初始化	此为根测位置设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuFlashBarPositionON(void)
*	功能说明: 在待机模式的设置参数界面，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionON(void)
{
	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFlashBarPositionSel(void)
*	功能说明: 在待机模式的设置参数界面，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
**********************************************************************************************************/
void DispMenuFlashBarPositionSel(void)
{
	/***********高转速扭矩限制**************/
	if((sel.OperaMode == 0) || (sel.OperaMode == 1) ) 
	{
		/***********高转速扭矩限制**************/
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
	sel_mode_flag = OperationMode_flag;								//显示运行模式设置
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFlashBarPositionUp(void)
*	功能说明: 在待机模式的设置参数界面，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
*	函 数 名: void DispMenuFlashBarPositionDown(void)
*	功能说明: 在待机模式的设置参数界面，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210824         
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
	
	
	
