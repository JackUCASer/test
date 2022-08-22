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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-04-26
* �� �� ��: display_battary_state
* ����˵��: ��ʾ���״̬
* ��    ��: 
* �� �� ֵ: 
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
	
	bat_vol = movmean_filter3(bat_vol, buf, 5);	/*	ȡ��ֵ����ֹ��ѹ����	*/
	
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
	
	//	��ʾ���ͼ��
//	show_variable_icon(0x00, PAGE_BAT_ID, display_icon_id);
	write_two_82H_cmd(PAGE_BAT_ID, display_icon_id, (is_autoback_off() == true ? 1: 0));
	
	//	�͵�������
	if((already_notice_bat == false)&&(display_icon_id == 0)&&((is_smart_reminder_open() == true)||(is_train_mode_open() == true))){
		play_now_music(BATTERY_LOW_INDEX, system_volume[get_system_vol()]);
		already_notice_bat = true;
	}else
		;
}
