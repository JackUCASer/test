#include "lcd_display.h"
#include "GL_Config.h"
#include "get_voltage.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

#include "movmean.h"

#ifndef LCD_PRINTF
#define LCD_PRINTF            myPrintf
#endif

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-04-26
* 函 数 名: display_battary_state
* 功能说明: 显示电池状态
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void display_battary_state(void)
{
	static uint8_t last_icon_id = 5;
	static float buf[5] = {0};
	
	uint8_t display_icon_id = 0;
	uint8_t now_icon_id = 4;
	
	float bat_vol = get_battery_voltage(); 
	float chg_vol = get_charge_voltage();
	
	bat_vol = movmean_filter3(bat_vol, buf, 5);	/*	取均值，防止电压波动	*/
	
	if(chg_vol > 19.2f){
		last_icon_id = 5;
		display_icon_id = 5;
	}else{
		if(bat_vol >= 12.0f)
			now_icon_id = 4;
		else if(bat_vol >= 11.4f)
			now_icon_id = 3;
		else if(bat_vol >= 10.8f)
			now_icon_id = 2;
		else if(bat_vol >= 10.2f)
			now_icon_id = 1;
		else
			now_icon_id = 0;
		
		if(now_icon_id <= last_icon_id){
			display_icon_id = now_icon_id;
			last_icon_id = now_icon_id;
		}else{
			display_icon_id = last_icon_id;
			last_icon_id = last_icon_id;
		}
	}
	
	switch(now_display_page){	/*	根据适配器电压和电池电压显示具体图标	*/
		case PAGE_Mode:
			show_variable_icon(PAGE_Mode, PAGE_MODE_BAT_ID, display_icon_id);
		break;
		case PAGE_Setting:
			show_variable_icon(PAGE_Setting, PAGE_SETTING_BAT_ID, display_icon_id);
		break;
		case PAGE_Calibration:
			show_variable_icon(PAGE_Calibration, PAGE_CALIBRATION_BAT_ID, display_icon_id);
		break;
		case PAGE_Aging:
			show_variable_icon(PAGE_Aging, PAGE_AGING_BAT_ID, display_icon_id);
		break;
		
		default: break;
	}
	
}


