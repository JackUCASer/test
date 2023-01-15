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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: Page_Setting
* 功能说明: setting界面下的程序
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern LCD_TOUCH_T 	lcd_touch_t;
static uint8_t key_secret = 0;		/*	记录密码按键:用于校正和老化		*/
static uint8_t key_secret2= 0;		/*	记录密码按键2：用于自动回退和程序升级	*/
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
						Write_Sys_Para_Data();		/*	保存用户设置	*/
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
					Set_VGUS_Page(PAGE_UPGRADE, true);		//	切换至升级界面
					Start_SysUpdate();						//	设置更新标志位：执行更新程序一定要接电源，所以在启动更新前需要设置充电时可用
					osDelay(100);
					Write_Two_82H_Cmd(PAGE_UPGRADE_ID, 0, 0);
					osDelay(100);
					HAL_NVIC_SystemReset();				/*	直接设备重启，跳转至bootloader	*/
					end_cycle = true;
				break;
				
				case SETTING_2_FP_CALIB:
					Set_VGUS_Page(PAGE_FP_CALIB, true);
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
						key_secret2 =0;
					return SETTING_EXIT;
					
					case PAGE_SETT_SMART_REMD:
						vgus_page_objs[PAGE_Setting].obj_value[0] = lcd_touch_t.touch_state;
						if(lcd_touch_t.touch_state == 1)
							Open_Smart_Reminder();		/*	开启智能提醒	*/
						else
							Close_Smart_Reminder();		/*	关闭智能提醒	*/
					break;
					
					case PAGE_SETT_TRAIN_MODE:
						vgus_page_objs[PAGE_Setting].obj_value[1] = lcd_touch_t.touch_state;
						if(lcd_touch_t.touch_state == 1)
							Open_Train_Mode();			/*	开启训练模式	*/
						else
							Close_Train_Mode();			/*	关闭训练模式	*/
					break;
					
					case PAGE_SETT_LIGHT_ID:			/*	设置亮度		*/
						vgus_page_objs[PAGE_Setting].obj_value[2] = lcd_touch_t.touch_state;
						Set_System_Light(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SETT_VOL_ID:				/*	设置音量		*/
						vgus_page_objs[PAGE_Setting].obj_value[3] = lcd_touch_t.touch_state;
						Set_System_Vol(lcd_touch_t.touch_state);
					break;
					
					case PAGE_SECRET_KEY1_ID:			/*	密码“1”对应的按键被按下	*/
						key_secret = (key_secret << 1);
						key_secret += 1;
						key_secret2 = 0;
					break;
					
					case PAGE_SECRET_KEY2_ID:			/*	密码“0”对应的按键被按下	*/
						key_secret = (key_secret << 1);
						key_secret += 0;
						key_secret2 = 0;
					break;
					
					case PAGE_CHECK_O_RING_ID:
						key_secret = 0;		/*	密钥清零	*/
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
						key_secret = 0;	// 密钥清零
						key_secret2 =0;
					return SETTING_2_FP_CALIB;
					
					default: break;
				}
			break;
				
			default: 	/*	在当前界面收到其他界面按钮响应，直接切换为当前界面		*/
				lcd_touch_t.touch_wait_read = 0;
				Set_VGUS_Page(PAGE_Setting, true); 
			break;		
		}
	}

	/*	2. 密码识别	*/
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
	if(Is_Train_Mode_Open() == true)
		Play_Now_Music(CHECK_O_RING_INDEX, system_volume[Get_System_Vol()]);
	else if(Is_Smart_Reminder_Open() == true)
		Play_Now_Music(CHECK_O_RING_INDEX, system_volume[Get_System_Vol()]);
	else 
		return ;
}


