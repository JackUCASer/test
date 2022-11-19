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
* 版	  本：V1.2
* 日    期：2022-09-16
* 函 数 名: display_battary_state
* 功能说明: 显示电池状态
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void display_battary_state(void)
{
	static bool already_notice_bat = false;
	static bool charging_flag = false;
	static float buf[5] = {0};				//	用于滑动滤波的缓存
	static uint8_t cnt,cnt2 = 0;			//	每150ms被轮询一次
	static uint8_t display_battery_id = 5;	//	实际显示的电池ID
	uint8_t now_battery_id = 0;				//	当前电池电压图标ID
	
	if(++cnt % N_UPDATE_BAT == 0){
		float bat_vol = get_battery_voltage(); 
		float chg_vol = get_charge_voltage();
		
		bat_vol = movmean_filter3(bat_vol, buf, 5);	// 对电池电压进行滑动滤波
		
		//	1. 判断当前适配器电压
		if(chg_vol > 19.2f){
			charging_flag = true;		//	有适配器接入
			already_notice_bat = false;
			sys_bat_too_low = false;	//	解除电池电压过低保护
			//	2. 适配器接入时，电池电压会抬升
			if(bat_vol >= 12.2f)
				now_battery_id = 4;
			else if(bat_vol >= 11.6f)
				now_battery_id = 3;
			else if(bat_vol >= 11.0f)
				now_battery_id = 2;
			else if(bat_vol >= 10.4f)
				now_battery_id = 1;
			else
				now_battery_id = 0;
		}else{
			charging_flag = false;		//	未接入适配器
			//	2. 判断当前电池电压
			if(bat_vol >= 12.0f){
				now_battery_id = 4;
				sys_bat_too_low = false;
			}else if(bat_vol >= 11.4f){
				now_battery_id = 3;
				sys_bat_too_low = false;
			}else if(bat_vol >= 10.8f){
				now_battery_id = 2;
				sys_bat_too_low = false;
			}else if(bat_vol >= 10.2f){
				now_battery_id = 1;
				sys_bat_too_low = false;
			}else{
				now_battery_id = 0;
				sys_bat_too_low = true;
			}
		}
		
		//	3. 动态更新电池图标
		if(charging_flag == true){
			display_battery_id = now_battery_id + 5 + cnt2;
			if(display_battery_id > 9){
				cnt2 = 0;
				display_battery_id = now_battery_id + 5;
			}
			cnt2 ++;
		}else{
			display_battery_id = (now_battery_id < display_battery_id ? now_battery_id : display_battery_id);
			cnt2 = 0;
		}
		write_two_82H_cmd(PAGE_BAT_ID, display_battery_id, (is_autoback_off() == true ? 1: 0));
		
		//	4. 低电量提醒
		if((charging_flag == false)&&(already_notice_bat == false)&&(now_battery_id == 0)&&((is_smart_reminder_open() == true)||(is_train_mode_open() == true))){
			play_now_music(BATTERY_LOW_INDEX, system_volume[get_system_vol()]);
			already_notice_bat = true;
		}else
			;
	}else
		return ;
}

