
////////////////////////////////////////////////////////////////////
/// @file app.c
/// @brief 顶层应用程序
/// 
/// 文件详细描述：	
///					
///
///
///
///
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210825
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
////////////////////////////////////////////////////////////////////

//	应用层
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
//	硬件层
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "battery.h"
//	标准库
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

unsigned char LVGL_Flag = 0;									// LVGL的任务处理器 (Task Handler)执行周期，这里取5ms
unsigned char LVGL_TL_Flag = 0;									// LVGL的任务处理器 (Task Handler)执行周期，这里取5ms
/**********************************************************************************************************
*	函 数 名: void App_Init(void)
*	功能说明: 初始化系统
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
void App_Init(void)
{
	uint16_t bl_flag = 0;
	delay_init(84);
	
	HAL_TIM_Base_Start_IT(&htim5); 								// 1ms定时器
	HAL_TIM_Base_Start_IT(&htim10); 							// 2ms
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);    				// root
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);    				// 通过调节LCD的PWM波形来控制LCD的亮度
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);    				// beep
	HAL_ADC_Start_DMA(&hadc1,ADC_ConvertedValue,ADC_NUM_CH); 	// 初始化ADC
	
	data_init();												// 数据初始化，读EEPROM，设置初始模式、菜单显示初始化等	
	
	power_on();													// 开机，决定进哪种模式——待机、设置、校准

//	Lcd_Init();  												// 初始化OLED
//	Get_KeyVaule();												// 按键初始化
	LVGL_ON_OFF = 1;
	lv_port_disp_init();										// 初始化UI接口
	Power_mode_select();
	lv_task_handler();        									//用户图形界面刷新
	for(bl_flag = 0; bl_flag <= d_backlight[set.backlight]; bl_flag++)  				//做二十次背光渐亮
	{
		TIM2->CCR1 = bl_flag;
		delay_ms(1);
	}
	HAL_IWDG_Refresh(&hiwdg);									// 喂狗  2s会饿
	init_MC();
	Apex_cal();
	Battery_init();												// 读取电池电量参数
	
	HAL_Delay(100);
	HAL_IWDG_Refresh(&hiwdg);									//喂狗  //2s会饿
	
	sys.FlagThetaBias = 0;
	Time_cnt = 0;												//自动关机计时清零
}

/**********************************************************************************************************
*	函 数 名: void AppTest(void)
*	功能说明: 主函数循环调用函数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210825         
**********************************************************************************************************/
uint8_t LVGL_task_Flag = 0;
void AppTest(void)
{
//	Key_Scan();																				//按键扫描
	
	SamBattery();																			//电池电压监测
	if((LVGL_Flag >= 5))											//LVGL定时调用
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
	HAL_IWDG_Refresh(&hiwdg);		//喂狗  2s会饿
	switch(WorkMode)
	{
		case Rotory_mode:		{ WorkMode = Rotory(WorkMode);		break;}					// 连续运动模式，包括正转、反转
		case Reciproc_mode:		{ WorkMode = Reciproc(WorkMode);	break;}					// 往复运动模式
		case Atc_mode:			{ WorkMode = ATC_RUN(WorkMode);		break;}					// ATC运动模式
		case Measure_mode:		{ WorkMode = Measure(WorkMode);		break;}					// 根测模式
		case RunWithApex_mode:	{ WorkMode = RunWithApex(WorkMode); break;}					// 运行+根测模式
		case RecWithApex_mode:	{ WorkMode = RecWithApex(WorkMode);	break;}					// 往复+根测模式
		case ATRwithApex_mode: 	{ WorkMode = ATCwithApex(WorkMode);	break;}					// ATC+根测模式
		case Standby_mode:		{ WorkMode = Standby(WorkMode);		break;}					// 待机模式	
		case Charging_mode:		{ WorkMode = Charging(WorkMode);	break;}					// 充电模式
		case Fault_mode:		{ WorkMode = Fault(WorkMode);		break;}					// 运行错误模式
		case PowerOff_mode:		{ WorkMode = PowerOff(WorkMode);	break;}					// 关机预备模式
		case DispOff_mode:		{ WorkMode = DispOff(WorkMode);		break;}					// 关闭显示模式
		case DisplayLogo_mode:	{ WorkMode = Displaylogo(WorkMode);	break;}					// 显示LOGO模式
		default:break;
	}	
}




