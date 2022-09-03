#include "lcd_display.h"
#include "user_data.h"
#include "get_voltage.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

#include "movmean.h"

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
	static bool already_notice_bat = false;
	
	uint8_t display_icon_id = 0;
	uint8_t now_icon_id = 4;
	
	float bat_vol = get_battery_voltage(); 
	float chg_vol = get_charge_voltage();
	
	bat_vol = movmean_filter3(bat_vol, buf, 5);	/*	取均值，防止电压波动	*/
	
	if(chg_vol > 19.2f){
		last_icon_id = 5;
		display_icon_id = 5;
		already_notice_bat = false;
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
	
	//	显示电池图标
//	show_variable_icon(0x00, PAGE_BAT_ID, display_icon_id);
	write_two_82H_cmd(PAGE_BAT_ID, display_icon_id, (is_autoback_off() == true ? 1: 0));
	
	//	低电量提醒
	if((already_notice_bat == false)&&(display_icon_id == 0)&&((is_smart_reminder_open() == true)||(is_train_mode_open() == true))){
		play_now_music(BATTERY_LOW_INDEX, system_volume[get_system_vol()]);
		already_notice_bat = true;
	}else
		;
}
