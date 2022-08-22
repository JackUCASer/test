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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: page_setting
* ����˵��: setting�����µĳ���
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern LCD_TOUCH_T 	lcd_touch_t;
static uint8_t key_secret = 0;		/*	��¼���밴��		*/
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
						write_sys_para_t_data();		/*	�����û�����	*/
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
					HAL_NVIC_SystemReset();				/*	ֱ���豸��������ת��bootloader	*/
					end_cycle = true;
				break;
				
				default: break;
			}
			
			if(end_cycle == true){						/*	�ж��Ƿ����ѭ��	*/
				end_cycle = false;
				break;
			}
			
			osDelay(50);
		}
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-04-12
* �� �� ��: touch_widget_scan
* ����˵��: setting�����´����ؼ�ɨ��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static PAGE_SETTING_SELECT touch_widget_scan(void)
{
	if(lcd_touch_t.touch_wait_read){
		switch(lcd_touch_t.page_id){
			case PAGE_Setting:
				lcd_touch_t.touch_wait_read = 0;
				switch(lcd_touch_t.touch_id){
					case PAGE_SETT_BACK_ID:				/*	���ذ�ť������	*/
						key_secret = 0;	// ��Կ����
					return SETTING_EXIT;
					
					case PAGE_SETT_SMART_REMD:
						if(lcd_touch_t.touch_state == 1)
							open_smart_reminder();		/*	������������	*/
						else
							close_smart_reminder();		/*	�ر���������	*/
					break;
					
					case PAGE_SETT_TRAIN_MODE:	
						if(lcd_touch_t.touch_state == 1)
							open_train_mode();			/*	����ѵ��ģʽ	*/
						else
							close_train_mode();			/*	�ر�ѵ��ģʽ	*/
					break;
					
					case PAGE_SETT_LIGHT_ID:			/*	��������		*/
						set_system_light(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SETT_VOL_ID:				/*	��������		*/
						set_system_vol(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SECRET_KEY1_ID:			/*	���롰1����Ӧ�İ���������	*/
						key_secret = (key_secret << 1);
						key_secret += 1;
					break;
					
					case PAGE_SECRET_KEY2_ID:			/*	���롰0����Ӧ�İ���������	*/
						key_secret = (key_secret << 1);
						key_secret += 0;
					break;
					
					case PAGE_CHECK_O_RING_ID:
						key_secret = 0;		/*	��Կ����	*/
						play_chkOring_voice();
					return SETTING_2_CHECKHANDLE;
					
					case PAGE_SETT_BACK_OFF_ID:
						key_secret += 15;
					break;
					
					case PAGE_SETT_UPGEADE_ID:			/*	��������Ĵν�����������	*/
						key_secret += 17;		//	4*17 = 68 = 0x44 = 2b0100 0100
					break;
					
					default: break;
				}
			break;
				
			default: 	/*	�ڵ�ǰ�����յ��������水ť��Ӧ��ֱ���л�Ϊ��ǰ����		*/
				lcd_touch_t.touch_wait_read = 0;
				set_now_page(PAGE_Setting); 
			break;		
		}
	}

	/*	2. ����ʶ��	*/
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-08-03
* �� �� ��: play_chkOring_voice
* ����˵��: ����check_o_ring ��Ƶ
*
* ��    ��: 
      
* �� �� ֵ: 
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


