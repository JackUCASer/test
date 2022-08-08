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
	key_secret = 0;
	if(xSemaphoreTake(sPage_sett, (TickType_t)0) == pdTRUE)	/*	��ȡ�ź����ɹ�	*/
	{
		/*	װ�س�ʼֵ	*/
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	���������ʷ	*/
		myPrintf("\r\n--------------------Now in Page Setting--------------------\r\n");
		for(;;){
			page_sett_select = touch_widget_scan();	/*	���ý��水��ɨ��	*/
			switch(page_sett_select){
				case SETTING_EXIT:
					set_now_page(PAGE_Mode);
					end_cycle = true;
					if(sys_para_update_flag == true)
						write_sys_para_t_data();		/*	�����û�����	*/
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
				
				default: break;
			}
			
			if(end_cycle == true){					/*	�ж��Ƿ����ѭ��	*/
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
			return SETTING_2_CALIBRATION;
		
		case 0xA3:
			return SETTING_2_AGING;
		
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


