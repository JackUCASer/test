#include "page_setting.h"

#include "user_data.h"
/*		FreeRTOS		*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

#include <string.h>

static PAGE_SETTING_SELECT touch_widget_scan(void);
static void play_chkOring_voice(void);
extern SemaphoreHandle_t	sPage_sett;

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: page_setting
* 功能说明: setting界面下的程序
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern LCD_TOUCH_T 	lcd_touch_t;
static uint8_t key_secret = 0;		/*	记录密码按键		*/
void page_setting(void)
{
	bool end_cycle = false;
	PAGE_SETTING_SELECT page_sett_select = SETTING_IDLE;
	if(xSemaphoreTake(sPage_sett, (TickType_t)0) == pdTRUE){
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));
		key_secret = 0;
		for(;;){
			page_sett_select = touch_widget_scan();
			switch(page_sett_select){
				case SETTING_EXIT:
					set_now_page(PAGE_Mode);
					if(sys_para_update_flag == true)
						write_sys_para_t_data();		/*	保存用户设置	*/
					end_cycle = true;
				break;
			
				case SETTING_2_CALIBRATION:
					set_now_page(PAGE_Calibration);
					end_cycle = true;
				break;
				
				case SETTING_2_AGING:
					set_now_page(PAGE_Aging);
					end_cycle = true;
				break;
				
				case SETTING_2_CHECKHANDLE:
					set_now_page(PAGE_CheckHandle);
					end_cycle = true;
				break;
				
				case SETTING_2_UPGRADE:
					set_now_page(PAGE_UPGRADE);
					osDelay(100);
					write_two_82H_cmd(PAGE_UPGRADE_ID, 0, 0);
					osDelay(100);
					HAL_NVIC_SystemReset();				/*	直接设备重启，跳转至bootloader	*/
					end_cycle = true;
				break;
				
				default: break;
			}
			
			if(end_cycle == true){						/*	判断是否结束循环	*/
				end_cycle = false;
				break;
			}
			
			osDelay(50);
		}
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-12
* 函 数 名: touch_widget_scan
* 功能说明: setting界面下触摸控件扫描
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static PAGE_SETTING_SELECT touch_widget_scan(void)
{
	if(lcd_touch_t.touch_wait_read){
		switch(lcd_touch_t.page_id){
			case PAGE_Setting:
				lcd_touch_t.touch_wait_read = 0;
				switch(lcd_touch_t.touch_id){
					case PAGE_SETT_BACK_ID:				/*	返回按钮被按下	*/
						key_secret = 0;	// 密钥清零
					return SETTING_EXIT;
					
					case PAGE_SETT_SMART_REMD:
						if(lcd_touch_t.touch_state == 1)
							open_smart_reminder();		/*	开启智能提醒	*/
						else
							close_smart_reminder();		/*	关闭智能提醒	*/
					break;
					
					case PAGE_SETT_TRAIN_MODE:	
						if(lcd_touch_t.touch_state == 1)
							open_train_mode();			/*	开启训练模式	*/
						else
							close_train_mode();			/*	关闭训练模式	*/
					break;
					
					case PAGE_SETT_LIGHT_ID:			/*	设置亮度		*/
						set_system_light(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SETT_VOL_ID:				/*	设置音量		*/
						set_system_vol(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SECRET_KEY1_ID:			/*	密码“1”对应的按键被按下	*/
						key_secret = (key_secret << 1);
						key_secret += 1;
					break;
					
					case PAGE_SECRET_KEY2_ID:			/*	密码“0”对应的按键被按下	*/
						key_secret = (key_secret << 1);
						key_secret += 0;
					break;
					
					case PAGE_CHECK_O_RING_ID:
						key_secret = 0;		/*	密钥清零	*/
						play_chkOring_voice();
					return SETTING_2_CHECKHANDLE;
					
					case PAGE_SETT_BACK_OFF_ID:
						key_secret += 15;
					break;
					
					case PAGE_SETT_UPGEADE_ID:			/*	连续点击四次进入升级界面	*/
						key_secret += 17;		//	4*17 = 68 = 0x44 = 2b0100 0100
					break;
					
					default: break;
				}
			break;
				
			default: 	/*	在当前界面收到其他界面按钮响应，直接切换为当前界面		*/
				lcd_touch_t.touch_wait_read = 0;
				set_now_page(PAGE_Setting); 
			break;		
		}
	}

	/*	2. 密码识别	*/
	switch(key_secret){
		case 0xAC:
			key_secret = 0;
			return SETTING_2_CALIBRATION;
		
		case 0xA3:
			key_secret = 0;
			return SETTING_2_AGING;
		
		case 0x44:
			key_secret = 0;
			return SETTING_2_UPGRADE;
		
		case 0x4B:
			if(is_autoback_off()== true){
				open_autoback();
				show_variable_icon(0x00, PAGE_AUTOBACK_ID, 0);
			}else{
				close_autoback();
				show_variable_icon(0x00, PAGE_AUTOBACK_ID, 1);
			}
			key_secret = 0;
		break;
		
		default: break;
	}
	return SETTING_KEEP;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-08-03
* 函 数 名: play_chkOring_voice
* 功能说明: 播放check_o_ring 音频
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void play_chkOring_voice(void)
{
	if(is_train_mode_open() == true)
		play_now_music(CHECK_O_RING_INDEX, system_volume[get_system_vol()]);
	else if(is_smart_reminder_open() == true)
		play_now_music(CHECK_O_RING_INDEX, system_volume[get_system_vol()]);
	else 
		return ;
}


