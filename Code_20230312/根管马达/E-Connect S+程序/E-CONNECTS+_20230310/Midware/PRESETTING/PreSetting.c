
////////////////////////////////////////////////////////////////////
/// @file preSetting.c
/// @brief 系统进入主程序前的预处理，包括数据初始化等等
/// 
/// 文件详细描述：	void power_on()开机按键检测，不同按键组合，进入不同的模式
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

uint8_t F_SetMode = 0;								// =0:待机模式，=1:设置模式，=2:工厂模式
uint8_t ReStoreCheck[2] = {0}; 						// EEPROM数据更新标志存储
extern uint8_t KeyFuncIndex ;  						// 定义在Menu.c，初始显示索引
extern IWDG_HandleTypeDef hiwdg;					// 系统看门狗
extern ADC_HandleTypeDef 	hadc1;					// AD转换


/**********************************************************************************************************
*	函 数 名: void power_on()
*	功能说明: 开机，同时按下开机键和设置键，进入系统模式；同时按下开机键、向上键和设置键，进入工厂模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void power_on(void)
{
	uint8_t key_cnt;
	HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_SET); 				// 系统维持供电
	//HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_SET);					// RELAY ON 切换到基准
	
	//开机按下开机键和s键切换到 系统设置模式
	while(	(HAL_GPIO_ReadPin( KEY_SEL_GPIO_Port,KEY_SEL_Pin) == 0) 				// SEL键按下
		  &&(HAL_GPIO_ReadPin( KEY_UP_GPIO_Port, KEY_UP_Pin)  == 1) 				// UP 键保持
		  &&(HAL_GPIO_ReadPin( KEY_ON_GPIO_Port, KEY_ON_Pin)  == 0))				// ON 键按下
	{
		HAL_IWDG_Refresh(&hiwdg);													// 喂狗  2s会饿
		HAL_Delay(10);
		key_cnt++;
		if(key_cnt > 50)
		{
			F_SetMode = 1;
			KeyFuncIndex = 51;	    																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									//索引指到功能设置
			break;
		}
	}
	
	key_cnt = 0;
	
	//s >  on 进入工厂模式
	while(	(HAL_GPIO_ReadPin( KEY_SEL_GPIO_Port,KEY_SEL_Pin)  == 0) 				// SEL键按下
		  &&(HAL_GPIO_ReadPin( KEY_UP_GPIO_Port, KEY_UP_Pin)   == 0)				// UP 键按下 
		  &&(HAL_GPIO_ReadPin( KEY_ON_GPIO_Port, KEY_ON_Pin)   == 0))				// ON 键按下
	{
		HAL_IWDG_Refresh(&hiwdg);													// 喂狗  2s会饿
		HAL_Delay(10);
		key_cnt++;
		if(key_cnt > 50)
		{
			F_SetMode = 2;
			KeyFuncIndex = 124;	    																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									//索引指到功能设置
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
	
	Ring.short_ring=1;						//BEEP短响
	HAL_Delay(100);
	Ring.short_ring=1;

	Get_KeyVaule();																	//空读，清除键值	
}

/**********************************************************************************************************
*	函 数 名: void Power_mode_select(void)
*	功能说明: 根据开机按键，进入不同工作模式
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210817         
**********************************************************************************************************/
void Power_mode_select(void)
{
	if(F_SetMode == 0 )
	{
		DispLogo();												//显示“八颗牙”标志
		sel_page_flash = 1;										//显示完LOGO后，进入待机模式，因此初始化sel_page_flash为1
		sel_eeprom_flash = 0;									//无需刷新
	
		set_page_flash = 0;										//set页面刷新
		set_eeprom_flash = 0;									//无需与set页面相关的EEPROM刷新
		
		cal_page_flash = 0;										//cal页面刷新
		cal_eeprom_flash = 0;									//无需与cal页面相关的EEPROM刷新
	}
	else if(F_SetMode == 1)
	{
		DispSetting();											//显示“设置”
		sel_page_flash = 0;										//显示完LOGO后，进入待机模式，因此初始化sel_page_flash为1
		sel_eeprom_flash = 0;									//无需刷新
	
		set_page_flash = 1;										//set页面刷新
		set_eeprom_flash = 0;									//无需与set页面相关的EEPROM刷新
		
		cal_page_flash = 0;										//cal页面刷新
		cal_eeprom_flash = 0;									//无需与cal页面相关的EEPROM刷新
	}
	else if(F_SetMode == 2)
	{
		DispFactory();											//显示“工厂模式”
		sel_page_flash = 0;										//显示完LOGO后，进入待机模式，因此初始化sel_page_flash为1
		sel_eeprom_flash = 0;									//无需刷新
	
		set_page_flash = 0;										//set页面刷新
		set_eeprom_flash = 0;									//无需与set页面相关的EEPROM刷新
		
		cal_page_flash = 1;										//cal页面刷新
		cal_eeprom_flash = 0;									//无需与cal页面相关的EEPROM刷新
	}	
}

/**********************************************************************************************************
*	函 数 名: void data_init()
*	功能说明: 数据初始化，读EEPROM，设置初始模式等
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210817         
**********************************************************************************************************/
void data_init(void)
{
	static uint8_t i;
	
	WorkMode = DisplayLogo_mode;												//初始化为显示LOGO模式

	MenuDataInit();																//菜单显示数据初始化
	HAL_IWDG_Refresh(&hiwdg);													//喂狗  2s会饿

	EEPROM_Read(RECOVERY_ADDR,ReStoreCheck,2);									//读取更新标志
	if((ReStoreCheck[0] != 0xA1) && (ReStoreCheck[1] != 0xC1))					//判断是否存在标志，没有标志就进入更新程序
	{
		Store2EepromInit();
		InitReName();
		ReStoreCheck[0] = 0xA1;
		ReStoreCheck[1] = 0xC1;
		EEPROM_Write(RECOVERY_ADDR,ReStoreCheck,2);								//更新完成，置标志
	}
	SettingRead();	
	if(set.StartupMemory==1)
	{
		ReadLastMem();
	}else																		//如果是 Last ，读出上次记忆的位置
	{
		mem.MemoryNum = 1;
	}
	MemoryRead(mem.MemoryNum);															//读取设置
}

/**********************************************************************************************************
*	函 数 名: void Start_Adc(void)
*	功能说明: 启动1次ADC
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210820         
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
*	函 数 名: void init_MC(void)
*	功能说明: 初始化电机参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
extern uint16_t Round_Cnt;
void init_MC(void)
{
	init_registers();														//初始化MP6570内部的寄存器
	set_speed_slope(40);													//初始化电机加速斜率
	//nSLEEP_SET; 															//enable pre-driver nsleep
	HAL_Delay(1);	
	//mp6570_enable();  													//enable mp6570
	HAL_Delay(2);
	mp6570_init(0x00);														//写入MP6570内部的寄存器
	MP6570_WriteRegister(0x00,0x60,0x0000);									//使MP6570进入正常工作模式
	HAL_Delay(10);															//delay_1ms 100ms wait for sensor to be stable	
	reset_parameters();
	Round_Cnt = 0;
//	LOOPTIMER_ENABLE; 														//在此处不能使能，会出现开机不成功，因为中断中的SPI读与正常SPI冲突//enable loop time
//	HAL_Delay(20);
//	LOOPTIMER_DISABLE;
}

