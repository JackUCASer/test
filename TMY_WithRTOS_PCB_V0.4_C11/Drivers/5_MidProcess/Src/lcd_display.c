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
	
	uint8_t display_icon_id = 0;
	uint8_t now_icon_id = 4;
	
	float bat_vol = get_battery_voltage(); 
	float chg_vol = get_charge_voltage();
	
	bat_vol = movmean_filter3(bat_vol, buf, 5);	/*	ȡ��ֵ����ֹ��ѹ����	*/
	
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
	
	switch(now_display_page){	/*	������������ѹ�͵�ص�ѹ��ʾ����ͼ��	*/
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


