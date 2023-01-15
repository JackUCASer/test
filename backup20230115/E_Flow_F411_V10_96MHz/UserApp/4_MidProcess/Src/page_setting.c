#include "Page_Setting.h"

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
* �� �� ��: Page_Setting
* ����˵��: setting�����µĳ���
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern LCD_TOUCH_T 	lcd_touch_t;
static uint8_t key_secret = 0;		/*	��¼���밴��:����У�����ϻ�		*/
static uint8_t key_secret2= 0;		/*	��¼���밴��2�������Զ����˺ͳ�������	*/
void Page_Setting(void)
{
	bool end_cycle = false;
	PAGE_SETTING_SELECT page_sett_select = SETTING_IDLE;
	if(xSemaphoreTake(sPage_sett, (TickType_t)0) == pdTRUE){
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));
		key_secret = 0;
		key_secret2 = 0;
		for(;;){
			page_sett_select = touch_widget_scan();
			switch(page_sett_select){
				case SETTING_EXIT:
					Set_VGUS_Page(PAGE_Mode, true);
					if(sys_para_update_flag == true)
						Write_Sys_Para_Data();		/*	�����û�����	*/
					end_cycle = true;
				break;
			
				case SETTING_2_MTORCALIB:
					Set_VGUS_Page(PAGE_MotorCalib, true);
					end_cycle = true;
				break;
				
				case SETTING_2_AGING:
					Set_VGUS_Page(PAGE_Aging, true);
					end_cycle = true;
				break;
				
				case SETTING_2_CHECKHANDLE:
					Set_VGUS_Page(PAGE_CheckHandle, true);
					end_cycle = true;
				break;
				
				case SETTING_2_UPGRADE:
					Set_VGUS_Page(PAGE_UPGRADE, true);		//	�л�����������
					Start_SysUpdate();						//	���ø��±�־λ��ִ�и��³���һ��Ҫ�ӵ�Դ����������������ǰ��Ҫ���ó��ʱ����
					osDelay(100);
					Write_Two_82H_Cmd(PAGE_UPGRADE_ID, 0, 0);
					osDelay(100);
					HAL_NVIC_SystemReset();				/*	ֱ���豸��������ת��bootloader	*/
					end_cycle = true;
				break;
				
				case SETTING_2_FP_CALIB:
					Set_VGUS_Page(PAGE_FP_CALIB, true);
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
						key_secret2 =0;
					return SETTING_EXIT;
					
					case PAGE_SETT_SMART_REMD:
						vgus_page_objs[PAGE_Setting].obj_value[0] = lcd_touch_t.touch_state;
						if(lcd_touch_t.touch_state == 1)
							Open_Smart_Reminder();		/*	������������	*/
						else
							Close_Smart_Reminder();		/*	�ر���������	*/
					break;
					
					case PAGE_SETT_TRAIN_MODE:
						vgus_page_objs[PAGE_Setting].obj_value[1] = lcd_touch_t.touch_state;
						if(lcd_touch_t.touch_state == 1)
							Open_Train_Mode();			/*	����ѵ��ģʽ	*/
						else
							Close_Train_Mode();			/*	�ر�ѵ��ģʽ	*/
					break;
					
					case PAGE_SETT_LIGHT_ID:			/*	��������		*/
						vgus_page_objs[PAGE_Setting].obj_value[2] = lcd_touch_t.touch_state;
						Set_System_Light(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SETT_VOL_ID:				/*	��������		*/
						vgus_page_objs[PAGE_Setting].obj_value[3] = lcd_touch_t.touch_state;
						Set_System_Vol(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SECRET_KEY1_ID:			/*	���롰1����Ӧ�İ���������	*/
						key_secret = (key_secret << 1);
						key_secret += 1;
						key_secret2 = 0;
					break;
					
					case PAGE_SECRET_KEY2_ID:			/*	���롰0����Ӧ�İ���������	*/
						key_secret = (key_secret << 1);
						key_secret += 0;
						key_secret2 = 0;
					break;
					
					case PAGE_CHECK_O_RING_ID:
						key_secret = 0;		/*	��Կ����	*/
						play_chkOring_voice();
					return SETTING_2_CHECKHANDLE;
					
					case PAGE_SETT_BACK_OFF_ID:
						key_secret2 += 15;
						key_secret = 0;
					break;
					
					case PAGE_SETT_CHARGEUSE_ID:
						key_secret2 += 17;		//	4*17 = 68 = 0x44 = 2b0100 0100
						key_secret = 0;
					break;
					
					case PAGE_SETT_PEDALCALIB_ID:
						key_secret = 0;	// ��Կ����
						key_secret2 =0;
					return SETTING_2_FP_CALIB;
					
					default: break;
				}
			break;
				
			default: 	/*	�ڵ�ǰ�����յ��������水ť��Ӧ��ֱ���л�Ϊ��ǰ����		*/
				lcd_touch_t.touch_wait_read = 0;
				Set_VGUS_Page(PAGE_Setting, true); 
			break;		
		}
	}

	/*	2. ����ʶ��	*/
	switch(key_secret){
		case 0xAC:
			key_secret = 0;
			return SETTING_2_MTORCALIB;
		
		case 0xA3:
			key_secret = 0;
			return SETTING_2_AGING;
		
		default: break;
	}
	switch(key_secret2){
		case 0x3C:	//	60
			if(Is_Autoback_Off()== true){
				Open_Autoback();
				Show_Variable_Icon(0x00, PAGE_AUTOBACK_ID, 0);
			}else{
				Close_AutoBack();
				Show_Variable_Icon(0x00, PAGE_AUTOBACK_ID, 1);
			}
			key_secret2 = 0;
		break;
			
		case 0x40:	//	64
			key_secret2 = 0;
		return SETTING_2_UPGRADE;
			
		case 0x44:	//	68
			if(Is_UseInCharge()== true){
				Close_UseInCharge();
				Show_Variable_Icon(0x00, PAGE_CHARGE_USE_ID, 0);
			}else{
				Open_UseInCharge();
				Show_Variable_Icon(0x00, PAGE_CHARGE_USE_ID, 1);
			}
			key_secret2 = 0;
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
	if(Is_Train_Mode_Open() == true)
		Play_Now_Music(CHECK_O_RING_INDEX, system_volume[Get_System_Vol()]);
	else if(Is_Smart_Reminder_Open() == true)
		Play_Now_Music(CHECK_O_RING_INDEX, system_volume[Get_System_Vol()]);
	else 
		return ;
}


