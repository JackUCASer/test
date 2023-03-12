#include "MenuFunc.h"
#include "Menu.h"
#include "MenuData.h"
#include "lcd.h"
#include "MenuDisp.h"
#include "../lvgl/lvgl.h"
#include "datatype.h"
#include "datastorage.h"
#include "mp6570.h"
#include "Multiple_Styles.h"
#include "eeprom.h"
#include "key_Task.h"
#include "preSetting.h"
#include <string.h>
#include "DataStorage.h"
#include "delay.h"
#include "control.h"
#include "customer_control.h"
#include "macros.h"
#include "beep.h"
#include "key.h"
#include "spi.h"

SET_MODE_e set_mode_flag;							// 设置参数跳转标志位
NEW_FILE_e new_file_flag;							// 新建锉文件夹标志位
int name_str = 0;									// 锉文件列表指针
int Disp_list_str = 0;								// 锉文件列表的滚动指针
uint8_t yes_on_flag = 0;
uint8_t Outside_file_add_num[1] = {0};				// 外部增加文件夹个数
int Seq_Disp_str = 0;								// 锉文件下显示里面锉的滚动指针
int Seq_parameter_str = 0;							// 锉参数显示界面的锉序列指针
int Seq_set_str = 0;								// 锉参数设置界面的锉序列指针
uint8_t file_seq_flag;								// 判断时文件删除，还是文件下得单个锉删除，文件删除：file_seq_flag=0；单个锉删除：file_seq_flag = 1；

extern uint8_t speed_cnt;
extern IWDG_HandleTypeDef hiwdg;

extern char* keynameTab[34];						// 字母键盘小写
extern char* keynameTabBig[34];						// 字母键盘大写
extern char* key123nameTab[11];						// 数字键盘
extern uint8_t Versions_Flag;
//				功能设置函数
/**********************************************************************************************************
*	函 数 名: void DispMenuVersionsInit(void)
*	功能说明: 进入设置模式的主界面，显示版本号
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		set_mode_flag = Versions_flag;								//显示版本号
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuVersionsON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsON(void)
{
	uint8_t ReStoreCheck[2]= {0XAB,0XCD}; 							// EEPROM数据更新标志存储
	int cnt = 0, i = 0;
	long ave = 0;
	
	if(set.RestoreFactorySettings == 1)								// 恢复出厂设置标志位置1 
	{
		ReStoreCheck[0]=0X00;
		ReStoreCheck[1]=0X00;	
		EEPROM_Write(RECOVERY_ADDR,ReStoreCheck,2);					
		set.RestoreFactorySettings = 0;								// 恢复出厂设置标志位清零 							
		data_init();
		HAL_Delay(200);  
	}

	if(set.Calibration == 1)
	{
		lv_Calibrating_init();
		HAL_Delay(50);
		lv_task_handler();
		
		MX_SPI1_Init();
		HAL_Delay(100);
		init_MC();
		stop();
		HAL_Delay(200);
		LOOPTIMER_ENABLE;
		sel.Speed = s50;								      				//设置校准时的速度条件
		motor_settings.mode = 0;
		motor_settings.forward_speed = d_Speed[sel.Speed];
		motor_settings.upper_threshold = 5000; 								//设置一个比较大的值，使不反转
		motor_settings.lower_threshold = 2500;
		update_settings(&motor_settings);
		start(); 
		for(i = 0; i < 32; i++)
		{
			speed_cnt = i;

			motor_settings.forward_speed = d_Speed[i];
			update_settings(&motor_settings);
			HAL_Delay(500);

			/********************/
			HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
			for(cnt = 0; cnt < 30; cnt++)
			{
				ave += motor_iq;  											//累计电流
				HAL_Delay(20);
			}
			
			/********************/	
			HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
			ave = ave / 30;
			 
			NoLoadCurrent[i] = ave;											//空载电流值
			ave = 0;
		}
		stop();
		HAL_Delay(500);
		HAL_Delay(500);
		sel.Speed = s50;								      				//设置校准时的速度条件
		motor_settings.mode = 0;
		motor_settings.forward_speed = -d_Speed[sel.Speed];
		motor_settings.upper_threshold = 5000; 								//设置一个比较大的值，使不反转
		motor_settings.lower_threshold = 2500;
		update_settings(&motor_settings);
		start(); 
		for(i = 32; i < 64; i++)
		{
			speed_cnt = i;

			motor_settings.forward_speed = -d_Speed[i-32];
			update_settings(&motor_settings);
			
			HAL_Delay(500);

			/********************/
			HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
			for(cnt = 0; cnt < 30; cnt++)
			{
				ave += motor_iq;  											//累计电流
				HAL_Delay(20);
			}
			
			/********************/	
			HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
			ave = ave / 30;
			 
			NoLoadCurrent[i] = ave;											//空载电流值
			ave = 0;
		}
		stop();

		HAL_Delay(100);
		LOOPTIMER_DISABLE;
		Save1_Read0_NoloadTor(1);											//save
		Save1_Read0_NoloadTor(0);											//read
		lv_Finish_init();
		HAL_Delay(50);
		lv_task_handler();
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
		Ring.short_ring=1;
		HAL_Delay(100);
		Ring.short_ring=1;
		HAL_Delay(100);
		set.Calibration = 0;
		data_init();
		HAL_Delay(100);
		Get_KeyVaule();																	//空读，清除键值
	}
	
	Versions_Flag = 0;
	SettingSave();													// 将Setting（设置参数）保存到EEPROM中
	Jump2Menu(0);													// 跳转到待机界面
	set_mode_flag = 0;												// 初始化系统设置模式标志位
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuVersionsSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsSel(void)
{
	Versions_Flag = 0;
	set_page_flash = 1;
	Jump2Menu(56);													// 跳转到功能设置“自动关机功能设置”
	set_mode_flag = AutoPowerOff_flag;								// 显示自动关机设置
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuVersionsUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsUp(void)
{
   
};

/**********************************************************************************************************
*	函 数 名: void DispMenuVersionsDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsDown(void)
{
  
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoPowerOffInit(void)
*	功能说明: 在设置模式下，显示自动关机时间
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoPowerOffON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuVersionsSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffSel(void)
{
	set_page_flash = 1;
	Jump2Menu(61);													// 跳转到功能设置“自动待机时间设置“
	set_mode_flag = AutoStandy_flag;								// 跳回待机界面时间设置
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoPowerOffUp(void)
*	功能说明: 在设置模式下，在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffUp(void)
{
	set.AutoPowerOffTime ++;										
	
	if(set.AutoPowerOffTime > set.AutoPowerOffTime_len)
	{
		set.AutoPowerOffTime = 0;
	}	
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoPowerOffDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffDown(void)
{
	set.AutoPowerOffTime --;
	if(set.AutoPowerOffTime < 0)
	{
		set.AutoPowerOffTime = set.AutoPowerOffTime_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStandyInit(void)
*	功能说明: 在设置模式下，显示自动返回待机界面时间
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
}

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStandyON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyON(void)
{
	
}

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStandySel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandySel(void)
{
	set_page_flash = 1;	
	Jump2Menu(66);													// 跳转到功能设置“音量设置“
	set_mode_flag = BeepVol_flag;									// 蜂鸣器设置
	NeedFlash = 1;	
}

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStandyUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyUp(void)
{
	set.AutoStandyTime++;
	if(set.AutoStandyTime > set.AutoStandyTime_len)
	{
		set.AutoStandyTime = 0;
	}
	SYS_set_init(set);
}

/**********************************************************************************************************
*	函 数 名: void DispMenuAutoStandyDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyDown(void)
{
	set.AutoStandyTime--;
	if(set.AutoStandyTime < 0)
	{
		set.AutoStandyTime = set.AutoStandyTime_len;
	}
	SYS_set_init(set);
	
}

/**********************************************************************************************************
*	函 数 名: void DispMenuBeepVolInit(void)
*	功能说明: 在设置模式下，显示音量调节
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBeepVolON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuBeepVolSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolSel(void)
{
	set_page_flash = 1;
	Jump2Menu(71);													// 跳转到背光设置
	set_mode_flag = Bl_flag;										// 背光设置
	NeedFlash = 1;	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBeepVolUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolUp(void)
{
	set.BeepVol ++;
	if(set.BeepVol > 3)
	{
		set.BeepVol = 0;
	}
	
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBeepVolDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolDown(void)
{
	set.BeepVol --;
	if(set.BeepVol < 0)
	{
		set.BeepVol = 3;
	}
	
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBlInit(void)
*	功能说明: 在设置模式下，显示背光调节
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBlON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuBlSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlSel(void)
{
	set_page_flash = 1;
	Jump2Menu(76);
	set_mode_flag = Hand_flag;										//左右手设置
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBlUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlUp(void)
{
	set.backlight ++;
	if(set.backlight > set.backlight_len)
	{
		set.backlight = 0;
	}
	
	TIM2->CCR1 = d_backlight[set.backlight];
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBlDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlDown(void)
{
	set.backlight --;
	if(set.backlight < 0)
	{
		set.backlight = set.backlight_len;
	}
	
	TIM2->CCR1 = d_backlight[set.backlight];
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuHandInit(void)
*	功能说明: 在设置模式下，显示左右手操作
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuHandON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuHandSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandSel(void)
{
	set_page_flash = 1;
	Jump2Menu(81);
	set_mode_flag = Apexsensitivity_flag;							//根测灵敏度设置
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuHandUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandUp(void)
{
	set.Hand ++;
	if(set.Hand > set.Hand_len)
	{
		set.Hand = 0;
	}
	TIM5->CCR3 = 0;
	Lcd_Init();
	TIM5->CCR3 = d_backlight[set.backlight];
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuHandDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandDown(void)
{
	set.Hand --;
	if(set.Hand < 0)
	{
		set.Hand = set.Hand_len;
	}
	TIM5->CCR3 = 0;
	Lcd_Init();
	TIM5->CCR3 = d_backlight[set.backlight];
	SYS_set_init(set);
};


/**********************************************************************************************************
*	函 数 名: void DispMenuApexsensitivityInit(void)
*	功能说明: 在设置模式下，根测灵敏度设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApexsensitivityON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuApexsensitivitySel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivitySel(void)
{
	set_page_flash = 1;
	Jump2Menu(86);
	set_mode_flag = Language_flag;									//语言设置
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApexsensitivityUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityUp(void)
{
	set.ApexSense ++;
	if(set.ApexSense > set.ApexSense_len)
	{
		set.ApexSense = 0;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuApexsensitivityDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityDown(void)
{
	set.ApexSense --;
	if(set.ApexSense<0)
	{
		set.ApexSense = set.ApexSense_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuLanguageInit(void)
*	功能说明: 在设置模式下，显示语言设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
}

/**********************************************************************************************************
*	函 数 名: void DispMenuLanguageON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageON(void)
{
	
}

/**********************************************************************************************************
*	函 数 名: void DispMenuLanguageSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageSel(void)
{
	set_page_flash = 1;
//	Jump2Menu(96);
//	set_mode_flag = RestoreFactory_flag;							//恢复出厂设置
	
	// 屏蔽校准，校准不在设置模式里做
	Jump2Menu(91);
	set_mode_flag = Calibration_flag;								//校准设置
	NeedFlash = 1;	
}

/**********************************************************************************************************
*	函 数 名: void DispMenuLanguageUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageUp(void)
{
	set.Language++;
	if(set.Language > set.Language_len)
	{
		set.Language = 0;
	}
	SYS_set_init(set);
}

/**********************************************************************************************************
*	函 数 名: void DispMenuLanguageDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageDown(void)
{
	set.Language--;
	if(set.Language < 0)
	{
		set.Language = set.Language_len;
	}
	SYS_set_init(set);
}

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationInit(void)
*	功能说明: 在设置模式下，显示校准设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationON(void)
{	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationSel(void)
{
	set_page_flash = 1;
	Jump2Menu(96);
	set_mode_flag = RestoreFactory_flag;							//恢复出厂设置
	set.Calibration = 0;
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationUp(void)
{
	set.Calibration++;
	if(set.Calibration > set.Calibration_len)
	{
		set.Calibration = 0;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationDown(void)
{
	set.Calibration--;
	if(set.Calibration < 0)
	{
		set.Calibration = set.Calibration_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRestoreFactorySettingsInit(void)
*	功能说明: 在设置模式下，显示校准设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRestoreFactorySettingsON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsON(void)
{
    //	EEPROM_Write(RECOVERY_ADDR,ReStoreCheck,2);	//
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRestoreFactorySettingsSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsSel(void)
{
	Jump2Menu(101);
	set_mode_flag = MenuStartup_flag;								//版本号显示
	NeedFlash = 1;
	set_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsUp(void)
{
	set.RestoreFactorySettings ++;
	if(set.RestoreFactorySettings > set.RestoreFactorySettings_len)
	{
		set.RestoreFactorySettings = 0;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRestoreFactorySettingsDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsDown(void)
{
	set.RestoreFactorySettings--;
	if(set.RestoreFactorySettings < 0)
	{
		set.RestoreFactorySettings = set.RestoreFactorySettings_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuUndefineInit(void)
*	功能说明: 在设置模式下，参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuUndefineON(void)
*	功能说明: 在设置模式下，按下“ON”键，保存参数并返回待机主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineON(void)
{

};

/**********************************************************************************************************
*	函 数 名: void DispMenuRestoreFactorySettingsSel(void)
*	功能说明: 在设置模式下，按下“SEL”键，保存参数并进入下一个参数设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineSel(void)
{
	Jump2Menu(51);
	set_mode_flag = Versions_flag;								//版本号显示
	NeedFlash = 1;
	set_page_flash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuCalibrationUp(void)
*	功能说明: 在设置模式下，按下“UP”键，参数加1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineUp(void)
{
	set.StartupMemory++;
	if(set.StartupMemory > set.StartupMemory_len)
	{
		set.StartupMemory = 0;
	}
	SYS_set_init(set);
};
/**********************************************************************************************************
*	函 数 名: void DispMenuRestoreFactorySettingsDown(void)
*	功能说明: 在设置模式下，按下“DOWN”键，参数减1
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineDown(void)
{
	set.StartupMemory--;
	if(set.StartupMemory < 0)
	{
		set.StartupMemory = set.StartupMemory_len;
	}
	SYS_set_init(set);
};


/**********************************************************************************************************
*	函 数 名: void DispMenuBuiltInFileInit(void)
*	功能说明: 显示锉文件夹下得锉参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Seq_parameter_str = 0;																				// 锉文件下“第几个锉”清零
		Seq_Disp_str = 0;																					// 滚动指针清零
		if(name_str >= Outside_file_add_num[0])																// 如果文件夹名指针大于用户自定义锉文件夹总数，则进入内置锉显示
		{
			Seq_list_init(Seq_Inside, (name_str-Outside_file_add_num[0]), Seq_Disp_str, Seq_parameter_str);
		}else																								// 如果文件夹名指针小于用户自定义锉文件夹总数，则进入外置锉显示
		{
			ReadReName(name_str);																			// 读取EEPROM中的锉文件名
			ReadOutsideseq(name_str);																		// 从EEPROM中读取用户自定义锉参数
			Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);
		}
	}
};
/**********************************************************************************************************
*	函 数 名: void DispMenuBuiltInFileON(void)
*	功能说明: 显示锉文件夹下显示锉参数，按下“ON”键，保存参数并返回待机界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileON(void)
{
	Jump2Menu(0);																						// 不保存，直接返回主界面
	NeedFlash = 1;
	sel_page_flash = 1; 
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBuiltInFileSel(void)
*	功能说明: 显示锉文件夹下显示锉参数，按下“SEL”键，保存参数并返回待机界面，若用户自定义的锉，则跳转到设置界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileSel(void)
{
	/*
		首先判断此时是内置锉还是用户自定义锉，即name_str是否小于Outside_file_add_num[0]，小于是自定义锉，大于则是内置锉
		
		如果是用户自定义锉，则执行：
		此处就是判断锉文件夹下，排到第几个锉，如果锉指针与锉总个数相同，则表示光标移动到“+”添加锉位置。
		如果光标移动到“+”添加锉位置，且锉总个数已经等于6，即锉最大添加数量，则显示“内存已满”弹窗。
		如果光标移动到“+”添加锉位置，且锉总个数已经小于6，则正常添加锉。
		如果光标未移动到“+”添加锉位置，则保存参数并返回待机界面。
		
		如果是内置锉，则执行：
		添加参数保存并返回待机界面。
	*/
	if(name_str < Outside_file_add_num[0])									// name_str:锉文件名指针；Outside_file_add_num[0]:用户自定义锉文件总数
	{
		if(Seq_parameter_str == New_file_parameter.seq_Len)					//
		{
			if(New_file_parameter.seq_Len < 6)
			{
//				Jump2Menu(134);		
//				new_file_flag = Seq_name_flag;								//显示锉个数设置界面		
				Jump2Menu(139);		
				new_file_flag = Taper_flag;									//显示锉个数设置界面
//				New_file_parameter.seq_Len = 0;											//文件夹自身长度
				New_file_parameter.seq_new_Len = 1;											//文件夹新增长度				
				NeedFlash = 1;
				sel_page_flash = 1;
			}else
			{
				lv_memory_full();
				lv_task_handler();
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				Jump2Menu(106);
				NeedFlash = 1;
				sel_page_flash = 1;
			}				
		}else
		{
			//添加参数保存并返回
			sel.taper = New_file_parameter.fseq[Seq_parameter_str].taper;
			sel.number = New_file_parameter.fseq[Seq_parameter_str].number;
			sel.OperaMode = New_file_parameter.fseq[Seq_parameter_str].OperaMode;
			if((sel.OperaMode == 0) || (sel.OperaMode == 1))
			{
				sel.Speed = New_file_parameter.fseq[Seq_parameter_str].speed;
				sel.Vaule1 = sel.Speed;													//Vaule1记录速度数据
				sel.Torque = New_file_parameter.fseq[Seq_parameter_str].torque;
				sel.Vaule2 = sel.Torque;												//Vaule2记录转矩数据
				sel.AngleCW  = 0;
				sel.AngleCCW = 0;
			}
			else
			{
				sel.RecSpeed = 5;														//400rpm
				sel.Torque = 11;														//4.0N*cm
				sel.AngleCW  = New_file_parameter.fseq[Seq_parameter_str].AngleCW;
				sel.Vaule1 = sel.AngleCW;												//Vaule1记录正角度数据
				sel.AngleCCW = New_file_parameter.fseq[Seq_parameter_str].AngleCCW;
				sel.Vaule2 = sel.AngleCCW;												//Vaule2记录负角度数据
			}
			sel.AutoStart = 0;
			sel.AutoStop = 0;
			sel.FlashBarPosition = 0;
			sel.FileLibrary = name_str;
			sel.FileSeq = Seq_parameter_str;
			sel.color = New_file_parameter.fseq[Seq_parameter_str].color;
			sel.FileSave = 1;															//=0,默认锉；=1，用户自定义；=2，内部存储
			ReadReName(name_str);
			strcpy(ReStandyName[mem.MemoryNum-1],ReName[name_str]);
			SaveStandyReName(mem.MemoryNum);
			MemorySave(mem.MemoryNum);
			Jump2Menu(0);
			NeedFlash = 1;
			sel_page_flash = 1; 	
		}
	}else if(name_str > Outside_file_add_num[0])
	{
		//添加参数保存并返回
		
		sel.taper = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].taper;
		sel.number = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].number;
		sel.OperaMode = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].OperaMode;
		if((sel.OperaMode == 0) || (sel.OperaMode == 1))
		{
			sel.Speed = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].speed;
			sel.Vaule1 = sel.Speed;													//Vaule1记录速度数据
			sel.Torque = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].torque;
			sel.Vaule2 = sel.Torque;												//Vaule2记录转矩数据
			sel.AngleCW  = 0;
			sel.AngleCCW = 0;
		}
		else
		{
			sel.AtrTorque = t40;
			sel.RecSpeed = 5;				//400rpm
			sel.Torque = 11;				//4.0N*cm
			sel.AngleCW  = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].AngleCW;
			sel.Vaule1 = sel.AngleCW;												//Vaule1记录正角度数据
			sel.AngleCCW = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].AngleCCW;
			sel.Vaule2 = sel.AngleCCW;												//Vaule2记录负角度数据
		}
		sel.AutoStart = 0;
		sel.AutoStop = 0;
		sel.FlashBarPosition = 0;
		sel.FileLibrary = name_str-Outside_file_add_num[0];
		sel.FileSeq = Seq_parameter_str;
		sel.color = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].color;
		sel.FileSave = 2;																		//=0,默认锉；=1，用户自定义；=2，内部存储
		strcpy(ReStandyName[mem.MemoryNum-1],Seq_Inside[name_str-Outside_file_add_num[0]].Name);
		SaveStandyReName(mem.MemoryNum);
		MemorySave(mem.MemoryNum);
		Jump2Menu(0);
		NeedFlash = 1;
		sel_page_flash = 1; 
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuBuiltInFileUp(void)
*	功能说明: 显示锉文件夹下显示锉参数，按下“UP”键，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileUp(void)
{
	Seq_parameter_str ++;
	
	Seq_Disp_str++;
	if(Seq_Disp_str > 2)
	{
		Seq_Disp_str = 2;
	}
	if(name_str >= Outside_file_add_num[0])
	{
		if(Seq_parameter_str >= (Seq_Inside[name_str-Outside_file_add_num[0]].seq_Len))
		{
			Seq_parameter_str = Seq_Inside[name_str-Outside_file_add_num[0]].seq_Len - 1;
		}
		Seq_list_init(Seq_Inside, (name_str-Outside_file_add_num[0]), Seq_Disp_str, Seq_parameter_str);
	}else
	{
		if(Seq_parameter_str >= (New_file_parameter.seq_Len))
		{
			Seq_parameter_str = New_file_parameter.seq_Len ;
		}
		Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);
	}
};
/**********************************************************************************************************
*	函 数 名: void DispMenuBuiltInFileDown(void)
*	功能说明: 显示锉文件夹下显示锉参数，按下“DOWN”键，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileDown(void)
{
	Seq_parameter_str --;
	if(Seq_parameter_str < 0)
	{
		Seq_parameter_str = 0;
	}
	Seq_Disp_str--;
	if(Seq_Disp_str < 0)
	{
		Seq_Disp_str = 0;
	}
	if(name_str >= Outside_file_add_num[0])
	{
		Seq_list_init(Seq_Inside, (name_str-Outside_file_add_num[0]), Seq_Disp_str, Seq_parameter_str);
	}else
	{
		Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);
	}
};
/**************************************************************
  *Function:
  *PreCondition:  判断是否修改了内置程序参数
  *Input:
  *Output:
 ***************************************************************/
uint8_t IsParameterChange(int FileLibraryNum)
{
    
}
/**********************************************************************************************************
*	函 数 名:void DispMenuBuiltInFileLongset(void)
*	功能说明: 显示锉文件夹下显示锉参数，长按下“SEL”键，询问是否删除该锉号，必须时用户自定义区
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/

void DispMenuBuiltInFileLongset(void)
{
	/*
	首先判断时内置锉还是用户自定义锉，即name_str是否大于等于Outside_file_add_num[0]，若大于为内置锉，若小于为用户自定义锉。
	
	如果是内置锉，不执行任何操作，跳转到106。
	如果是自定义锉，则判断锉指针处于什么位置。
	若锉指针Seq_parameter_str大于等于锉总数，或者锉总数等于1，则不执行任何操作，跳转到106。
	若锉指针Seq_parameter_str小于锉总数，且锉总数不等于1，则跳转到179，即询问是否删除操作。
	*/
	if(name_str >= Outside_file_add_num[0])
	{
		Jump2Menu(106);																				// 不执行任何操作返回
	}else
	{
		if((Seq_parameter_str >= New_file_parameter.seq_Len)||(New_file_parameter.seq_Len == 1))
		{
			Jump2Menu(106);
		}else
		{
			NeedFlash = 1;
			sel_page_flash = 1;
			Jump2Menu(179);																			// 询问是否删除
			file_seq_flag = 1;																		// file_seq_flag表示从什么地方进入删除操作，
																									// 若file_seq_flag = 0，表示从锉文件进入删除；
																									// 若file_seq_flag = 1，表示从锉文件下单个锉进入删除。
		}	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispMenuFilelistInit(void)
*	功能说明: 显示锉文件夹列表
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistInit(void)
{
	/*
		此处name_str = -1;表示指针处于添加锉文件按钮，在最顶部。
	*/
	int i=0;
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		name_str = -1;															//等于-1，时添加文件按钮
		Disp_list_str = 0;														//滚动指针清零
		Read_File_number(Outside_file_add_num);									//读取目前已添加的锉文件总数
		for(i=0; i < Outside_file_add_num[0]; i++)								//读取锉文件名
		{
			ReadReName(i);
		}
		
		Filelist_init(Seq_Inside, Disp_list_str, name_str , Outside_file_add_num[0]);
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFilelistON(void)
*	功能说明: 显示锉文件夹列表，按下“ON”键，返回主界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistON(void)
{
	Jump2Menu(0);														// 无保存返回
	sel_page_flash = 1;
	NeedFlash = 1;
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFilelistSel(void)
*	功能说明: 显示锉文件夹列表，按下“SEL”键，若选择“Myfile”，则跳转到主界面，若是新建文件夹，则跳到112，若是内置锉，则跳到锉参数显示
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistSel(void)
{
	/*
		首先判断锉文件的指针处于什么位置。
		如果name_str与Outside_file_add_num[0](用户自定义文件夹总数)相同，则处于“myfile”，则复位数据，还原锉设置到默认。
		如果name_str = -1，则出入“+”按钮，选择则进入文件添加操作。此时如果Outside_file_add_num[0]大于文件添加总数的最大值，则跳转到弹窗“内存已满”界面。然后返回锉文件列表界面。
		如果name_str不等于-1，不等于Outside_file_add_num[0]，则进入锉文件夹，程序跳转到106。
	*/
	if(name_str == Outside_file_add_num[0])
	{ 
		sel.taper = 0;
		sel.number = 0;
		sel.AutoStart = 0;
		sel.AutoStop = 0;
		sel.FlashBarPosition = 0;
		sel.FileLibrary = 0;
		sel.FileSeq = 0;
		sel.color = 0;
		sel.FileSave = 0;
		MemorySave(mem.MemoryNum);
		NeedFlash = 1;
		sel_page_flash = 1;
		Jump2Menu(0);
	}
	else if(name_str == -1)
	{
		Outside_file_add_num[0]++ ;										// 新建文件夹，文件夹总数加1	
		if(Outside_file_add_num[0] > 10)
		{
			Outside_file_add_num[0] --;
			lv_memory_full();											// 调用“内存已满”弹窗
			lv_task_handler();											// 调用一次刷屏任务
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			Jump2Menu(118);												// 再返回当前界面
			NeedFlash = 1;
			sel_page_flash = 1;
		}else
		{
			strcpy(ReName[Outside_file_add_num[0] - 1],"New file");		// 初始化将要命名的锉文件		
			SaveReName(Outside_file_add_num[0] - 1);					// 若此处不初始化，将会出现未知乱码
			Jump2Menu(112);												// 跳转到锉文件夹命名程序
		}
		sel_page_flash = 1;
	 	NeedFlash = 1;
	}else
	{
	 	Jump2Menu(106);													// 打开锉文件夹
		sel_page_flash = 1;
	 	NeedFlash = 1;
	}
}

/**********************************************************************************************************
*	函 数 名: void DispMenuFilelistSel(void)
*	功能说明: 显示锉文件夹列表，按下“UP”键，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistUp(void)
{
	name_str ++;
	if(name_str >=(Inside_file_num + Outside_file_add_num[0]))
	{
		name_str =  - 1;
		Disp_list_str = 0;
	}else
	{
		Disp_list_str++;
		if(Disp_list_str > 3)
		{
			Disp_list_str = 3;
		}
	}
	
    Filelist_init(Seq_Inside, Disp_list_str, name_str ,Outside_file_add_num[0]);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFilelistSel(void)
*	功能说明: 显示锉文件夹列表，按下“DOWN”键，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistDown(void)
{
	name_str --;
	if(name_str < -1)
	{
		name_str = Inside_file_num + Outside_file_add_num[0] - 1;
		Disp_list_str = 3;
	}else
	{
		Disp_list_str --;
		if(Disp_list_str  < 0)
		{
			Disp_list_str = 0;
		}
	}
	Filelist_init(Seq_Inside, Disp_list_str, name_str ,Outside_file_add_num[0]);
};

/**********************************************************************************************************
*	函 数 名: void DispMenuFilelistSel(void)
*	功能说明: 显示锉文件夹列表，长按下“SEL”键，询问是否删除
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistLongSel(void)
{
	/*
	首先判断name_str是否小于Outside_file_add_num[0](用户自定义锉文件总数)，若小于，且name_str != -1，程序跳转到179，询问是否删除。且file_seq_flag = 0，告知是从锉文件列表进入删除界面。
	如果不满足条件，则不执行任何操作。	
	*/
	if((name_str < Outside_file_add_num[0]) && (name_str != -1))
	{
		NeedFlash = 1;
		sel_page_flash = 1;
		Jump2Menu(179);	
		file_seq_flag = 0;																			// file_seq_flag表示从什么地方进入删除操作，
																									// 若file_seq_flag = 0，表示从锉文件进入删除；
																									// 若file_seq_flag = 1，表示从锉文件下单个锉进入删除。
	}else
	{
		Jump2Menu(118);
	}
//	Jump2Menu(118);	
	
}
/******************************* 锉命名 *************************************/
char * string ;
/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileInit(void)
*	功能说明: 设置文件
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileInit(void)
{
	if(sel_page_flash)
	{
		new_file_flag = File_name_flag;													// 设置新建锉文件夹处于命名阶段标志位
		sel_page_flash = 0;
		Seq_set_str = 0;																// 清零锉列表指针
		
		if(name_str == -1)																// 如果文件夹名指针=-1，则是添加锉，否则是用户自定义锉更改
		{
			ReadReName(Outside_file_add_num[0] - 1);									// 从EEPROM读取锉文件夹名
			string = ReName[Outside_file_add_num[0] - 1];								// 将字符串缓存
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			ReadReName(name_str);
			string = ReName[name_str];
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileON(void)
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileON(void)
{
	if(name_str != -1)
	{
		SaveReName(name_str);
	}	
	Jump2Menu(118);				
	NeedFlash = 1;
	sel_page_flash = 1;	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileSel(void)
*	功能说明: 设置文件,按下“SEL”，进入下一个设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileSel(void)
{
	/*
	判断name_str是否等于-1，如果name_str = -1，则进入下一个设置界面。
	如果name_str不等于-1，则保存参数并返回。
	*/
	if(name_str != -1)
	{
		SaveReName(name_str);
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}else
	{
//		Jump2Menu(134);				
//		new_file_flag = Seq_name_flag;								//显示锉个数设置界面
		Jump2Menu(139);				
		new_file_flag = Taper_flag;									//显示锉个数设置界面
		New_file_parameter.seq_Len = 0;								//文件夹新增长度
		New_file_parameter.seq_new_Len = 1;							//文件夹新增长度
		NeedFlash = 1;
		sel_page_flash = 1;
	}
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileUp(void)
*	功能说明: 设置文件,按下“Up”，无操作
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileUp(void)
{
	Jump2Menu(112);	
};
/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileDown(void)
*	功能说明: 设置文件,按下“DOWN”，无操作
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileDown(void)
{  
	Jump2Menu(112);	
};

/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileLongSel(void)
*	功能说明: 设置文件，长按下“SEL”键，进入文本设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileLongSel(void)
{
 	Jump2Menu(129);				// 跳转到键盘，编辑文本		
	NeedFlash = 1;
	sel_page_flash = 1;	
};

uint8_t ShiftBig;				//大小写切换
uint8_t key_str;				//按键指针
uint8_t key_Pr;					//数字、字母切换。数字为1，字母为0


/**********************************************************************************************************
*	函 数 名: void DispFileKeytextInit()
*	功能说明: 键盘界面
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(NeedFlash == 1)
		{	
			NeedFlash = 0;							
			ShiftBig = 0;								// 初始化大小写，默认小写
			key_str = 0;								// 初始化键盘按压位置，默认处于0位置
			key_Pr = 0;									// 数字、字母切换。数字为1，字母为0			
		}
		if(name_str == -1)								// 判断是否处于新建锉文件
		{
			New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
		}else
		{															
			New_File_abckeyname(ReName[name_str], 0, 0, 0);	
		}	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileKeytextON()
*	功能说明: 属于键盘快捷键，清除文字
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextON()
{
	uint8_t string_len;																				
	string_len = strlen(ReName[Outside_file_add_num[0] - 1]);										// 获取字符串长度
	ReName[Outside_file_add_num[0] - 1][string_len-1] = 0;											// 清除文字，将对应位直接设置为0
	
	Jump2Menu(129);																					// 跳转到键盘初始化位置
	if(name_str == -1)																				// 判断是否处于新建锉文件
	{	
		New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
	}else
	{
		New_File_abckeyname(ReName[name_str], 0, 0, 0);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispMenuRenameFileSel(void)
*	功能说明: 键盘界面,按下“SEL”，选择目前按下的文本
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextSel()
{
	uint8_t string_len;
	string_len = strlen(ReName[Outside_file_add_num[0] - 1]);										// 获取字符串长度
	if(key_Pr == 0)																					// 数字、字母切换。数字为1，字母为0							
	{
		if(key_str == 24)																			// 不保存退出
		{
			strcpy(ReName[Outside_file_add_num[0] - 1], string);																
			Jump2Menu(112);
			SaveReName(Outside_file_add_num[0] - 1);
			NeedFlash = 1;
			sel_page_flash = 1;	
		}else if(key_str == 33)																		// 保存退出
		{
			Jump2Menu(112);
			SaveReName(Outside_file_add_num[0] - 1);
			NeedFlash = 1;
			sel_page_flash = 1;	
		}
		else 
		{
			if(	((key_str >= 1) && (key_str <= 10))
			||	((key_str >= 13) && (key_str <= 23))
			||	((key_str >= 25) && (key_str <= 32)))
			{
				if(string_len < 10)																	// 长度小于10，可添加字符，否则保存不变
				{
					if(ShiftBig == 1)
					{
						strcat(ReName[Outside_file_add_num[0] - 1], keynameTabBig[key_str]);
					}else
					{
						strcat(ReName[Outside_file_add_num[0] - 1], keynameTab[key_str]);
					}	
				}	
			}else if(key_str == 11)																	// 清除单个字符
			{
				ReName[Outside_file_add_num[0] - 1][string_len-1] = 0;		
			}else if(key_str == 12)																	// 大写、小写字母切换
			{
				if(ShiftBig == 1)
				{
					ShiftBig = 0;
				}else
				{
					ShiftBig = 1;
				}		
			} else if(key_str == 0)																	// 数字键盘、字母键盘切换
			{
				key_str =0;
				key_Pr = 1;	
			}
			
			Jump2Menu(129);
			if(name_str == -1)
			{	
				New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
			}else
			{
				New_File_abckeyname(ReName[name_str], 0, 0, 0);	
			}
		}
	}
	else
	{
		if(key_str == 0)																			// 数字键盘
		{
			key_str =0;
			key_Pr = 0;
			ShiftBig = 0;
		}else
		{
			if(string_len < 10)
			{
				strcat(ReName[Outside_file_add_num[0] - 1], key123nameTab[key_str]);
			}	
		}
		Jump2Menu(129);
		if(name_str == -1)
		{	
			New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
		}else
		{
			New_File_abckeyname(ReName[name_str], 0, 0, 0);	
		}	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileKeytextDown()
*	功能说明: 设置文件,按下“DOWN”，按键下移
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextDown()
{
	key_str--;
	if(key_Pr == 0)																					// 字母键盘，33个一循环；数字键盘，10个一循环
	{
		if((key_str > 33) || (key_str < 0))
		{
			key_str = 33;		
		}
	}else
	{
		if((key_str > 10) || (key_str < 0))
		{
			key_str = 10;		
		}	
	}
	
	if(name_str == -1)
	{
		New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
	}else
	{
		New_File_abckeyname(ReName[name_str], 0, 0, 0);	
	}	

}
/**********************************************************************************************************
*	函 数 名: void DispFileKeytextUp()
*	功能说明: 设置文件,按下“UP”，按键上移
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextUp()
{
	key_str++;
	if(key_Pr == 0)
	{
		if(key_str > 33)
		{
			key_str = 0;		
		}
	}else
	{
		if(key_str > 10)
		{
			key_str = 0;		
		}
	}
	
	if(name_str == -1)
	{
		New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
	}else
	{
		New_File_abckeyname(ReName[name_str], 0, 0, 0);	
	}	
}

/**********************************************************************************************************
*	函 数 名: void DispFileKeytextUp()
*	功能说明: 设置锉名,按下“sel”，保存设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextLongSel()
{
	Jump2Menu(112);
	SaveReName(Outside_file_add_num[0] - 1);
	NeedFlash = 1;
	sel_page_flash = 1;	
}
/**********************************************************************************************************
*	函 数 名: void DispFileSeqnumberInit()
*	功能说明: 文件夹下锉参数设置模式，显示锉个数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/	
void DispFileSeqnumberInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)																
		{
			New_file_parameter.seq_Len = 0;												//文件夹自身长度
			New_file_parameter.seq_new_Len = 1;											//文件夹新增长度
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);

		}else 
		{
			New_file_parameter.seq_new_Len = 1;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
		}
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileSeqnumberON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}

/**********************************************************************************************************
*	函 数 名: DispFileSeqnumberSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入锥度设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberSel()
{
	Jump2Menu(139);				
	new_file_flag = Taper_flag;								//显示锥度设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileSeqnumberDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberDown()
{
	New_file_parameter.seq_new_Len--;
	if((New_file_parameter.seq_new_Len) < 1)
	{
		New_file_parameter.seq_new_Len = 1;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}	
}

/**********************************************************************************************************
*	函 数 名: void DispFileSeqnumberUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberUp()
{	
	New_file_parameter.seq_new_Len++;
	if((New_file_parameter.seq_Len + New_file_parameter.seq_new_Len) > 6)
	{
		New_file_parameter.seq_new_Len--;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileTaperInit()
*	功能说明: 文件夹下锥度设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/		
void DispFileTaperInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileTaperON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}

/**********************************************************************************************************
*	函 数 名: void DispFileTaperSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入锉号设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperSel()
{
	Jump2Menu(144);				
	new_file_flag = Number_flag;								//显示锉号设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileTaperDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper < 0) ||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper > 15))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileTaperUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper > 15)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 15;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileTaperInit()
*	功能说明: 文件夹下锥度设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/		
void DispFileTapernumberInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}

}

/**********************************************************************************************************
*	函 数 名: void DispFileTapernumberON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	函 数 名: void DispFileTaperSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入操作模式设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberSel()
{
	Jump2Menu(149);				
	new_file_flag = OperaMode_flag;								//显示操作模式设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileTapernumberDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number < 0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number >90))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileTapernumberUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number > 90)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 90;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileOperationInit()
*	功能说明: 文件夹下锥度设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/			
void DispFileOperationInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	

}
/**********************************************************************************************************
*	函 数 名: void DispFileTaperSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入速度设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationSel()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 1))
	{
		Jump2Menu(154);				
		new_file_flag = RecSpeed_flag;								//显示速度设置界面
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(164);				
		new_file_flag = AngleCW_flag;								//显示正角度设置界面
		NeedFlash = 1;
		sel_page_flash = 1;
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationDown()
{
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode < 1)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 3;
	}else
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode--;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode > 3)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileSpeedInit()
*	功能说明: 文件夹下锥度设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/	
void DispFileSpeedInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	函 数 名: void DispFileSpeedSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入转矩设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedSel()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 1))
	{
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s300)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t40)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t40;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed>s700)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t20)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t20;
			}
		}
	}
	Jump2Menu(159);				
	new_file_flag = torque_flag;								//显示转矩设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileSpeedDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > sel.Speed_len))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = sel.Speed_len;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileSpeedUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > sel.Speed_len)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileTorqueInit()
*	功能说明: 文件夹下转矩设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/		
void DispFileTorqueInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	函 数 名: void DispFileTorqueSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入颜色设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueSel()
{
	Jump2Menu(174);				
	new_file_flag = color_flag;								//显示颜色设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileTorqueDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueDown()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==1))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque--;
		if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > sel.Torque_len))
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = sel.Torque_len;
		}
		/***********高转速扭矩限制**************/
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s300)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t40)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t40;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s700)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t20)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t20;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s1200)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t10)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t10;
			}
		}
	}

	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileTorqueUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueUp()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==1))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque++;
		if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > sel.Torque_len))
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
		}
		/***********高转速扭矩限制**************/
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s300)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t40)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s700)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t20)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s1200)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t10)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			}
		}
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileCWAngleInit()
*	功能说明: 文件夹下正角度设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/		
void DispFileCWAngleInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	函 数 名: void DispFileTaperSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入负角度设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleSel()
{
	Jump2Menu(169);				
	new_file_flag = AngleCCW_flag;								//显示负角度设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileCWAngleDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW < sel.AngleCW_len))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = sel.AngleCW_len;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileCWAngleUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW > sel.AngleCW_len)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileCWAngleInit()
*	功能说明: 文件夹下正角度设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/		
void DispFileCCWAngleInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileOperationON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	函 数 名: void DispFileCCWAngleSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入颜色设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleSel()
{
	Jump2Menu(174);				
	new_file_flag = color_flag;								//显示颜色设置界面
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	函 数 名: void DispFileCCWAngleDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW > sel.AngleCCW_len))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = sel.AngleCCW_len;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileCCWAngleUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW > sel.AngleCCW_len)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileColorInit()
*	功能说明: 文件夹下颜色设置模式，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/		
void DispFileColorInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].color = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].color = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileColorON()
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileColorON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	函 数 名: void DispFileTaperSel()
*	功能说明: 文件夹下锉参数设置模式,按下“SEL”，将进入锉号设置模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileColorSel()
{
	Seq_set_str ++;
	if(Seq_set_str < New_file_parameter.seq_new_Len)
	{		
		Jump2Menu(139);
		new_file_flag = Taper_flag;								//显示锥度设置界面
		NeedFlash = 1;
		sel_page_flash = 1;	
	}
	else
	{
		if(name_str == -1)
		{
			New_file_parameter.seq_Len += New_file_parameter.seq_new_Len;
			SaveOutsideseq(Outside_file_add_num[0]-1);	
			Write_File_number(Outside_file_add_num);
			SaveReName(Outside_file_add_num[0]-1);
			
//			Outside_file_add_num[0] --;
//			Read_File_number(Outside_file_add_num);
			Seq_set_str = 0;									//复位指针
			Jump2Menu(118);										//跳转到文件夹列表
			NeedFlash = 1;
			sel_page_flash = 1;	
		}else
		{
			New_file_parameter.seq_Len += New_file_parameter.seq_new_Len;
			SaveOutsideseq(name_str);
			Seq_set_str = 0;									//复位指针
			Jump2Menu(106);										//跳转到文件夹列表
			NeedFlash = 1;
			sel_page_flash = 1;			
		}

	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileColorDown()
*	功能说明: 文件夹下锉参数设置模式,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileColorDown()
{
	New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color --;
	if((New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color < 0)||(New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color > 8))
	{
		New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color = 8;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileColorUp()
*	功能说明: 文件夹下锉参数设置模式,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20210917         
**********************************************************************************************************/
void DispFileColorUp()
{
	New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color ++;
	if(New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color > 8)
	{
		New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	函 数 名: void DispFileDeleteInit(void)
*	功能说明: 删除设置
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20211023         
**********************************************************************************************************/	
void DispFileDeleteInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		lv_delete_style(yes_on_flag);
	}	
}

/**********************************************************************************************************
*	函 数 名: void DispFileDeleteON(void)
*	功能说明: 设置文件，如果是新增，则不保存，如果是修改，则保存参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20211023          
**********************************************************************************************************/
void DispFileDeleteON(void)
{
	Jump2Menu(179);
	NeedFlash = 1;
	sel_page_flash = 1;	
}

/**********************************************************************************************************
*	函 数 名: void DispFileTaperSel()
*	功能说明: 删除提示下，确认选项
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20211023          
**********************************************************************************************************/
void DispFileDeleteSel(void)
{
	int i;
	char *name = "New file   ";
	if(file_seq_flag)
	{
		if(yes_on_flag)
		{
			if((New_file_parameter.seq_Len > 1)&&(Seq_parameter_str < (New_file_parameter.seq_Len-1)))
			{
				for(i = Seq_parameter_str; i < New_file_parameter.seq_Len; i++)
				{
					New_file_parameter.fseq[i].taper = New_file_parameter.fseq[i+1].taper;
					New_file_parameter.fseq[i].number = New_file_parameter.fseq[i+1].number;
					New_file_parameter.fseq[i].OperaMode = New_file_parameter.fseq[i+1].OperaMode;
					New_file_parameter.fseq[i].color = New_file_parameter.fseq[i+1].color;
					if((New_file_parameter.fseq[i+1].OperaMode == 0) || (New_file_parameter.fseq[i+1].OperaMode == 1))
					{
						New_file_parameter.fseq[i].speed = New_file_parameter.fseq[i+1].speed;
						New_file_parameter.fseq[i].torque = New_file_parameter.fseq[i+1].torque;
						New_file_parameter.fseq[i].AngleCW = 0;
						New_file_parameter.fseq[i].AngleCCW = 0;
					}else 
					{
						New_file_parameter.fseq[i].speed = 0;
						New_file_parameter.fseq[i].torque = 0;
						New_file_parameter.fseq[i].AngleCW = New_file_parameter.fseq[i+1].AngleCW;
						New_file_parameter.fseq[i].AngleCCW = New_file_parameter.fseq[i+1].AngleCCW;
					}
				}
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].taper = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].number = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].OperaMode = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].color = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].speed = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].torque = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCW = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCCW = 0;	
				
				New_file_parameter.seq_Len --;
				SaveOutsideseq(name_str);
			}
			else if((New_file_parameter.seq_Len > 1)&&(Seq_parameter_str == (New_file_parameter.seq_Len-1)))
			{
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].taper = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].number = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].OperaMode = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].color = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].speed = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].torque = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCW = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCCW = 0;	
				
				New_file_parameter.seq_Len --;
				Seq_parameter_str--;
				SaveOutsideseq(name_str);				
			}	
		}
		Jump2Menu(106);
		NeedFlash = 1;
		sel_page_flash = 1;	
//		Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);	
	}else
	{
		if(yes_on_flag)
		{
			for(i = name_str; i < Outside_file_add_num[0]-1; i++)
			{
				ReadReName(i+1);
				strcpy(ReName[i], ReName[i+1]);
				SaveReName(i);
				ReadOutsideseq(i+1);
				SaveOutsideseq(i);
			}
			strcpy(ReName[Outside_file_add_num[0] - 1], "New file");
			SaveReName(Outside_file_add_num[0] - 1);
			Outside_file_add_num[0]--;
			Write_File_number(Outside_file_add_num);
		}
		Jump2Menu(118);
		NeedFlash = 1;
		sel_page_flash = 1;	
//		Filelist_init(Seq_Inside, Disp_list_str, name_str ,Outside_file_add_num[0]);		
	}
}

/**********************************************************************************************************
*	函 数 名: void DispFileDeleteDown(void)
*	功能说明: 删除提示下,按下“DOWN”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20211023         
**********************************************************************************************************/
void DispFileDeleteDown(void)
{
	if(yes_on_flag)
	{
		yes_on_flag = 0;
	}else
	{
		yes_on_flag = 1;
	}
	lv_delete_style(yes_on_flag);
}

/**********************************************************************************************************
*	函 数 名: void DispFileDeleteUp(void)
*	功能说明: 删除提示下,按下“UP”，
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1：
*	编辑日期: 1: 20211023         
**********************************************************************************************************/
void DispFileDeleteUp(void)
{
	if(yes_on_flag)
	{
		yes_on_flag = 0;
	}else
	{
		yes_on_flag = 1;
	}
	lv_delete_style(yes_on_flag);	
}









