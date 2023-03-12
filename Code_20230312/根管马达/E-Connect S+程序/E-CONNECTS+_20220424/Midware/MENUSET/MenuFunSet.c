#include "MenuFunc.h"
#include "Menu.h"
#include "MenuData.h"
#include "lcd.h"
#include "MenuDisp.h"
#include "../lvgl/lvgl.h"
#include "datatype.h"
#include "datastorage.h"
#include "mp6570.h"
#include "Multiple_Styles.h"
#include "eeprom.h"
#include "key_Task.h"
#include "preSetting.h"
#include <string.h>
#include "DataStorage.h"
#include "delay.h"
#include "control.h"
#include "customer_control.h"
#include "macros.h"
#include "beep.h"
#include "key.h"
#include "spi.h"

SET_MODE_e set_mode_flag;							// ���ò�����ת��־λ
NEW_FILE_e new_file_flag;							// �½���ļ��б�־λ
int name_str = 0;									// ��ļ��б�ָ��
int Disp_list_str = 0;								// ��ļ��б�Ĺ���ָ��
uint8_t yes_on_flag = 0;
uint8_t Outside_file_add_num[1] = {0};				// �ⲿ�����ļ��и���
int Seq_Disp_str = 0;								// ��ļ�����ʾ����ﱵĹ���ָ��
int Seq_parameter_str = 0;							// ﱲ�����ʾ����������ָ��
int Seq_set_str = 0;								// ﱲ������ý���������ָ��
uint8_t file_seq_flag;								// �ж�ʱ�ļ�ɾ���������ļ��µõ����ɾ�����ļ�ɾ����file_seq_flag=0�������ɾ����file_seq_flag = 1��

extern uint8_t speed_cnt;
extern IWDG_HandleTypeDef hiwdg;

extern char* keynameTab[34];						// ��ĸ����Сд
extern char* keynameTabBig[34];						// ��ĸ���̴�д
extern char* key123nameTab[11];						// ���ּ���
extern uint8_t Versions_Flag;
//				�������ú���
/**********************************************************************************************************
*	�� �� ��: void DispMenuVersionsInit(void)
*	����˵��: ��������ģʽ�������棬��ʾ�汾��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		set_mode_flag = Versions_flag;								//��ʾ�汾��
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuVersionsON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsON(void)
{
	uint8_t ReStoreCheck[2]= {0XAB,0XCD}; 							// EEPROM���ݸ��±�־�洢
	int cnt = 0, i = 0;
	long ave = 0;
	
	if(set.RestoreFactorySettings == 1)								// �ָ��������ñ�־λ��1 
	{
		ReStoreCheck[0]=0X00;
		ReStoreCheck[1]=0X00;	
		EEPROM_Write(RECOVERY_ADDR,ReStoreCheck,2);					
		set.RestoreFactorySettings = 0;								// �ָ��������ñ�־λ���� 							
		data_init();
		HAL_Delay(200);  
	}

	if(set.Calibration == 1)
	{
		lv_Calibrating_init();
		HAL_Delay(50);
		lv_task_handler();
		
		MX_SPI1_Init();
		HAL_Delay(100);
		init_MC();
		stop();
		HAL_Delay(200);
		LOOPTIMER_ENABLE;
		sel.Speed = s50;								      				//����У׼ʱ���ٶ�����
		motor_settings.mode = 0;
		motor_settings.forward_speed = d_Speed[sel.Speed];
		motor_settings.upper_threshold = 5000; 								//����һ���Ƚϴ��ֵ��ʹ����ת
		motor_settings.lower_threshold = 2500;
		update_settings(&motor_settings);
		start(); 
		for(i = 0; i < 32; i++)
		{
			speed_cnt = i;

			motor_settings.forward_speed = d_Speed[i];
			update_settings(&motor_settings);
			HAL_Delay(500);

			/********************/
			HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
			for(cnt = 0; cnt < 30; cnt++)
			{
				ave += motor_iq;  											//�ۼƵ���
				HAL_Delay(20);
			}
			
			/********************/	
			HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
			ave = ave / 30;
			 
			NoLoadCurrent[i] = ave;											//���ص���ֵ
			ave = 0;
		}
		stop();
		HAL_Delay(500);
		HAL_Delay(500);
		sel.Speed = s50;								      				//����У׼ʱ���ٶ�����
		motor_settings.mode = 0;
		motor_settings.forward_speed = -d_Speed[sel.Speed];
		motor_settings.upper_threshold = 5000; 								//����һ���Ƚϴ��ֵ��ʹ����ת
		motor_settings.lower_threshold = 2500;
		update_settings(&motor_settings);
		start(); 
		for(i = 32; i < 64; i++)
		{
			speed_cnt = i;

			motor_settings.forward_speed = -d_Speed[i-32];
			update_settings(&motor_settings);
			
			HAL_Delay(500);

			/********************/
			HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
			for(cnt = 0; cnt < 30; cnt++)
			{
				ave += motor_iq;  											//�ۼƵ���
				HAL_Delay(20);
			}
			
			/********************/	
			HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
			ave = ave / 30;
			 
			NoLoadCurrent[i] = ave;											//���ص���ֵ
			ave = 0;
		}
		stop();

		HAL_Delay(100);
		LOOPTIMER_DISABLE;
		Save1_Read0_NoloadTor(1);											//save
		Save1_Read0_NoloadTor(0);											//read
		lv_Finish_init();
		HAL_Delay(50);
		lv_task_handler();
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
		HAL_Delay(500);
		HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
		Ring.short_ring=1;
		HAL_Delay(100);
		Ring.short_ring=1;
		HAL_Delay(100);
		set.Calibration = 0;
		data_init();
		HAL_Delay(100);
		Get_KeyVaule();																	//�ն��������ֵ
	}
	
	Versions_Flag = 0;
	SettingSave();													// ��Setting�����ò��������浽EEPROM��
	Jump2Menu(0);													// ��ת����������
	set_mode_flag = 0;												// ��ʼ��ϵͳ����ģʽ��־λ
	NeedFlash = 1;
	sel_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuVersionsSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsSel(void)
{
	Versions_Flag = 0;
	set_page_flash = 1;
	Jump2Menu(56);													// ��ת���������á��Զ��ػ��������á�
	set_mode_flag = AutoPowerOff_flag;								// ��ʾ�Զ��ػ�����
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuVersionsUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsUp(void)
{
   
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuVersionsDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuVersionsDown(void)
{
  
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoPowerOffInit(void)
*	����˵��: ������ģʽ�£���ʾ�Զ��ػ�ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoPowerOffON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuVersionsSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffSel(void)
{
	set_page_flash = 1;
	Jump2Menu(61);													// ��ת���������á��Զ�����ʱ�����á�
	set_mode_flag = AutoStandy_flag;								// ���ش�������ʱ������
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoPowerOffUp(void)
*	����˵��: ������ģʽ�£�������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffUp(void)
{
	set.AutoPowerOffTime ++;										
	
	if(set.AutoPowerOffTime > set.AutoPowerOffTime_len)
	{
		set.AutoPowerOffTime = 0;
	}	
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoPowerOffDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoPowerOffDown(void)
{
	set.AutoPowerOffTime --;
	if(set.AutoPowerOffTime < 0)
	{
		set.AutoPowerOffTime = set.AutoPowerOffTime_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStandyInit(void)
*	����˵��: ������ģʽ�£���ʾ�Զ����ش�������ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStandyON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyON(void)
{
	
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStandySel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandySel(void)
{
	set_page_flash = 1;	
	Jump2Menu(66);													// ��ת���������á��������á�
	set_mode_flag = BeepVol_flag;									// ����������
	NeedFlash = 1;	
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStandyUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyUp(void)
{
	set.AutoStandyTime++;
	if(set.AutoStandyTime > set.AutoStandyTime_len)
	{
		set.AutoStandyTime = 0;
	}
	SYS_set_init(set);
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuAutoStandyDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuAutoStandyDown(void)
{
	set.AutoStandyTime--;
	if(set.AutoStandyTime < 0)
	{
		set.AutoStandyTime = set.AutoStandyTime_len;
	}
	SYS_set_init(set);
	
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuBeepVolInit(void)
*	����˵��: ������ģʽ�£���ʾ��������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBeepVolON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBeepVolSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolSel(void)
{
	set_page_flash = 1;
	Jump2Menu(71);													// ��ת����������
	set_mode_flag = Bl_flag;										// ��������
	NeedFlash = 1;	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBeepVolUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolUp(void)
{
	set.BeepVol ++;
	if(set.BeepVol > 3)
	{
		set.BeepVol = 0;
	}
	
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBeepVolDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBeepVolDown(void)
{
	set.BeepVol --;
	if(set.BeepVol < 0)
	{
		set.BeepVol = 3;
	}
	
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBlInit(void)
*	����˵��: ������ģʽ�£���ʾ�������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBlON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBlSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlSel(void)
{
	set_page_flash = 1;
	Jump2Menu(76);
	set_mode_flag = Hand_flag;										//����������
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBlUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlUp(void)
{
	set.backlight ++;
	if(set.backlight > set.backlight_len)
	{
		set.backlight = 0;
	}
	
	TIM2->CCR1 = d_backlight[set.backlight];
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBlDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuBlDown(void)
{
	set.backlight --;
	if(set.backlight < 0)
	{
		set.backlight = set.backlight_len;
	}
	
	TIM2->CCR1 = d_backlight[set.backlight];
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuHandInit(void)
*	����˵��: ������ģʽ�£���ʾ�����ֲ���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuHandON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuHandSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandSel(void)
{
	set_page_flash = 1;
	Jump2Menu(81);
	set_mode_flag = Apexsensitivity_flag;							//��������������
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuHandUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandUp(void)
{
	set.Hand ++;
	if(set.Hand > set.Hand_len)
	{
		set.Hand = 0;
	}
	TIM5->CCR3 = 0;
	Lcd_Init();
	TIM5->CCR3 = d_backlight[set.backlight];
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuHandDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuHandDown(void)
{
	set.Hand --;
	if(set.Hand < 0)
	{
		set.Hand = set.Hand_len;
	}
	TIM5->CCR3 = 0;
	Lcd_Init();
	TIM5->CCR3 = d_backlight[set.backlight];
	SYS_set_init(set);
};


/**********************************************************************************************************
*	�� �� ��: void DispMenuApexsensitivityInit(void)
*	����˵��: ������ģʽ�£���������������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApexsensitivityON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApexsensitivitySel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivitySel(void)
{
	set_page_flash = 1;
	Jump2Menu(86);
	set_mode_flag = Language_flag;									//��������
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApexsensitivityUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityUp(void)
{
	set.ApexSense ++;
	if(set.ApexSense > set.ApexSense_len)
	{
		set.ApexSense = 0;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuApexsensitivityDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuApexsensitivityDown(void)
{
	set.ApexSense --;
	if(set.ApexSense<0)
	{
		set.ApexSense = set.ApexSense_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuLanguageInit(void)
*	����˵��: ������ģʽ�£���ʾ��������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuLanguageON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageON(void)
{
	
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuLanguageSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageSel(void)
{
	set_page_flash = 1;
//	Jump2Menu(96);
//	set_mode_flag = RestoreFactory_flag;							//�ָ���������
	
	// ����У׼��У׼��������ģʽ����
	Jump2Menu(91);
	set_mode_flag = Calibration_flag;								//У׼����
	NeedFlash = 1;	
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuLanguageUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageUp(void)
{
	set.Language++;
	if(set.Language > set.Language_len)
	{
		set.Language = 0;
	}
	SYS_set_init(set);
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuLanguageDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuLanguageDown(void)
{
	set.Language--;
	if(set.Language < 0)
	{
		set.Language = set.Language_len;
	}
	SYS_set_init(set);
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationInit(void)
*	����˵��: ������ģʽ�£���ʾУ׼����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationON(void)
{	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationSel(void)
{
	set_page_flash = 1;
	Jump2Menu(96);
	set_mode_flag = RestoreFactory_flag;							//�ָ���������
	set.Calibration = 0;
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationUp(void)
{
	set.Calibration++;
	if(set.Calibration > set.Calibration_len)
	{
		set.Calibration = 0;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuCalibrationDown(void)
{
	set.Calibration--;
	if(set.Calibration < 0)
	{
		set.Calibration = set.Calibration_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRestoreFactorySettingsInit(void)
*	����˵��: ������ģʽ�£���ʾУ׼����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRestoreFactorySettingsON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsON(void)
{
    //	EEPROM_Write(RECOVERY_ADDR,ReStoreCheck,2);	//
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRestoreFactorySettingsSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsSel(void)
{
	Jump2Menu(101);
	set_mode_flag = MenuStartup_flag;								//�汾����ʾ
	NeedFlash = 1;
	set_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsUp(void)
{
	set.RestoreFactorySettings ++;
	if(set.RestoreFactorySettings > set.RestoreFactorySettings_len)
	{
		set.RestoreFactorySettings = 0;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRestoreFactorySettingsDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuRestoreFactorySettingsDown(void)
{
	set.RestoreFactorySettings--;
	if(set.RestoreFactorySettings < 0)
	{
		set.RestoreFactorySettings = set.RestoreFactorySettings_len;
	}
	SYS_set_init(set);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuUndefineInit(void)
*	����˵��: ������ģʽ�£���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineInit(void)
{
	if(set_page_flash)
	{
		set_page_flash = 0;
		SYS_set_init(set);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuUndefineON(void)
*	����˵��: ������ģʽ�£����¡�ON������������������ش���������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineON(void)
{

};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRestoreFactorySettingsSel(void)
*	����˵��: ������ģʽ�£����¡�SEL���������������������һ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineSel(void)
{
	Jump2Menu(51);
	set_mode_flag = Versions_flag;								//�汾����ʾ
	NeedFlash = 1;
	set_page_flash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuCalibrationUp(void)
*	����˵��: ������ģʽ�£����¡�UP������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineUp(void)
{
	set.StartupMemory++;
	if(set.StartupMemory > set.StartupMemory_len)
	{
		set.StartupMemory = 0;
	}
	SYS_set_init(set);
};
/**********************************************************************************************************
*	�� �� ��: void DispMenuRestoreFactorySettingsDown(void)
*	����˵��: ������ģʽ�£����¡�DOWN������������1
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210825         
**********************************************************************************************************/
void DispMenuUndefineDown(void)
{
	set.StartupMemory--;
	if(set.StartupMemory < 0)
	{
		set.StartupMemory = set.StartupMemory_len;
	}
	SYS_set_init(set);
};


/**********************************************************************************************************
*	�� �� ��: void DispMenuBuiltInFileInit(void)
*	����˵��: ��ʾ��ļ����µ�ﱲ���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		Seq_parameter_str = 0;																				// ��ļ��¡��ڼ���ﱡ�����
		Seq_Disp_str = 0;																					// ����ָ������
		if(name_str >= Outside_file_add_num[0])																// ����ļ�����ָ������û��Զ�����ļ���������������������ʾ
		{
			Seq_list_init(Seq_Inside, (name_str-Outside_file_add_num[0]), Seq_Disp_str, Seq_parameter_str);
		}else																								// ����ļ�����ָ��С���û��Զ�����ļ���������������������ʾ
		{
			ReadReName(name_str);																			// ��ȡEEPROM�е���ļ���
			ReadOutsideseq(name_str);																		// ��EEPROM�ж�ȡ�û��Զ���ﱲ���
			Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);
		}
	}
};
/**********************************************************************************************************
*	�� �� ��: void DispMenuBuiltInFileON(void)
*	����˵��: ��ʾ��ļ�������ʾﱲ��������¡�ON������������������ش�������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileON(void)
{
	Jump2Menu(0);																						// �����棬ֱ�ӷ���������
	NeedFlash = 1;
	sel_page_flash = 1; 
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBuiltInFileSel(void)
*	����˵��: ��ʾ��ļ�������ʾﱲ��������¡�SEL������������������ش������棬���û��Զ����ﱣ�����ת�����ý���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileSel(void)
{
	/*
		�����жϴ�ʱ������ﱻ����û��Զ���ﱣ���name_str�Ƿ�С��Outside_file_add_num[0]��С�����Զ���ﱣ��������������
		
		������û��Զ���ﱣ���ִ�У�
		�˴������ж���ļ����£��ŵ��ڼ���ﱣ�����ָ������ܸ�����ͬ�����ʾ����ƶ�����+������λ�á�
		�������ƶ�����+������λ�ã�����ܸ����Ѿ�����6���������������������ʾ���ڴ�������������
		�������ƶ�����+������λ�ã�����ܸ����Ѿ�С��6�����������ﱡ�
		������δ�ƶ�����+������λ�ã��򱣴���������ش������档
		
		���������ﱣ���ִ�У�
		��Ӳ������沢���ش������档
	*/
	if(name_str < Outside_file_add_num[0])									// name_str:��ļ���ָ�룻Outside_file_add_num[0]:�û��Զ�����ļ�����
	{
		if(Seq_parameter_str == New_file_parameter.seq_Len)					//
		{
			if(New_file_parameter.seq_Len < 6)
			{
//				Jump2Menu(134);		
//				new_file_flag = Seq_name_flag;								//��ʾﱸ������ý���		
				Jump2Menu(139);		
				new_file_flag = Taper_flag;									//��ʾﱸ������ý���
//				New_file_parameter.seq_Len = 0;											//�ļ���������
				New_file_parameter.seq_new_Len = 1;											//�ļ�����������				
				NeedFlash = 1;
				sel_page_flash = 1;
			}else
			{
				lv_memory_full();
				lv_task_handler();
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				Jump2Menu(106);
				NeedFlash = 1;
				sel_page_flash = 1;
			}				
		}else
		{
			//��Ӳ������沢����
			sel.taper = New_file_parameter.fseq[Seq_parameter_str].taper;
			sel.number = New_file_parameter.fseq[Seq_parameter_str].number;
			sel.OperaMode = New_file_parameter.fseq[Seq_parameter_str].OperaMode;
			if((sel.OperaMode == 0) || (sel.OperaMode == 1))
			{
				sel.Speed = New_file_parameter.fseq[Seq_parameter_str].speed;
				sel.Vaule1 = sel.Speed;													//Vaule1��¼�ٶ�����
				sel.Torque = New_file_parameter.fseq[Seq_parameter_str].torque;
				sel.Vaule2 = sel.Torque;												//Vaule2��¼ת������
				sel.AngleCW  = 0;
				sel.AngleCCW = 0;
			}
			else
			{
				sel.RecSpeed = 5;														//400rpm
				sel.Torque = 11;														//4.0N*cm
				sel.AngleCW  = New_file_parameter.fseq[Seq_parameter_str].AngleCW;
				sel.Vaule1 = sel.AngleCW;												//Vaule1��¼���Ƕ�����
				sel.AngleCCW = New_file_parameter.fseq[Seq_parameter_str].AngleCCW;
				sel.Vaule2 = sel.AngleCCW;												//Vaule2��¼���Ƕ�����
			}
			sel.AutoStart = 0;
			sel.AutoStop = 0;
			sel.FlashBarPosition = 0;
			sel.FileLibrary = name_str;
			sel.FileSeq = Seq_parameter_str;
			sel.color = New_file_parameter.fseq[Seq_parameter_str].color;
			sel.FileSave = 1;															//=0,Ĭ��ﱣ�=1���û��Զ��壻=2���ڲ��洢
			ReadReName(name_str);
			strcpy(ReStandyName[mem.MemoryNum-1],ReName[name_str]);
			SaveStandyReName(mem.MemoryNum);
			MemorySave(mem.MemoryNum);
			Jump2Menu(0);
			NeedFlash = 1;
			sel_page_flash = 1; 	
		}
	}else if(name_str > Outside_file_add_num[0])
	{
		//��Ӳ������沢����
		
		sel.taper = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].taper;
		sel.number = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].number;
		sel.OperaMode = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].OperaMode;
		if((sel.OperaMode == 0) || (sel.OperaMode == 1))
		{
			sel.Speed = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].speed;
			sel.Vaule1 = sel.Speed;													//Vaule1��¼�ٶ�����
			sel.Torque = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].torque;
			sel.Vaule2 = sel.Torque;												//Vaule2��¼ת������
			sel.AngleCW  = 0;
			sel.AngleCCW = 0;
		}
		else
		{
			sel.AtrTorque = t40;
			sel.RecSpeed = 5;				//400rpm
			sel.Torque = 11;				//4.0N*cm
			sel.AngleCW  = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].AngleCW;
			sel.Vaule1 = sel.AngleCW;												//Vaule1��¼���Ƕ�����
			sel.AngleCCW = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].AngleCCW;
			sel.Vaule2 = sel.AngleCCW;												//Vaule2��¼���Ƕ�����
		}
		sel.AutoStart = 0;
		sel.AutoStop = 0;
		sel.FlashBarPosition = 0;
		sel.FileLibrary = name_str-Outside_file_add_num[0];
		sel.FileSeq = Seq_parameter_str;
		sel.color = Seq_Inside[name_str-Outside_file_add_num[0]].fseq[Seq_parameter_str].color;
		sel.FileSave = 2;																		//=0,Ĭ��ﱣ�=1���û��Զ��壻=2���ڲ��洢
		strcpy(ReStandyName[mem.MemoryNum-1],Seq_Inside[name_str-Outside_file_add_num[0]].Name);
		SaveStandyReName(mem.MemoryNum);
		MemorySave(mem.MemoryNum);
		Jump2Menu(0);
		NeedFlash = 1;
		sel_page_flash = 1; 
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuBuiltInFileUp(void)
*	����˵��: ��ʾ��ļ�������ʾﱲ��������¡�UP������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileUp(void)
{
	Seq_parameter_str ++;
	
	Seq_Disp_str++;
	if(Seq_Disp_str > 2)
	{
		Seq_Disp_str = 2;
	}
	if(name_str >= Outside_file_add_num[0])
	{
		if(Seq_parameter_str >= (Seq_Inside[name_str-Outside_file_add_num[0]].seq_Len))
		{
			Seq_parameter_str = Seq_Inside[name_str-Outside_file_add_num[0]].seq_Len - 1;
		}
		Seq_list_init(Seq_Inside, (name_str-Outside_file_add_num[0]), Seq_Disp_str, Seq_parameter_str);
	}else
	{
		if(Seq_parameter_str >= (New_file_parameter.seq_Len))
		{
			Seq_parameter_str = New_file_parameter.seq_Len ;
		}
		Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);
	}
};
/**********************************************************************************************************
*	�� �� ��: void DispMenuBuiltInFileDown(void)
*	����˵��: ��ʾ��ļ�������ʾﱲ��������¡�DOWN������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuBuiltInFileDown(void)
{
	Seq_parameter_str --;
	if(Seq_parameter_str < 0)
	{
		Seq_parameter_str = 0;
	}
	Seq_Disp_str--;
	if(Seq_Disp_str < 0)
	{
		Seq_Disp_str = 0;
	}
	if(name_str >= Outside_file_add_num[0])
	{
		Seq_list_init(Seq_Inside, (name_str-Outside_file_add_num[0]), Seq_Disp_str, Seq_parameter_str);
	}else
	{
		Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);
	}
};
/**************************************************************
  *Function:
  *PreCondition:  �ж��Ƿ��޸������ó������
  *Input:
  *Output:
 ***************************************************************/
uint8_t IsParameterChange(int FileLibraryNum)
{
    
}
/**********************************************************************************************************
*	�� �� ��:void DispMenuBuiltInFileLongset(void)
*	����˵��: ��ʾ��ļ�������ʾﱲ����������¡�SEL������ѯ���Ƿ�ɾ����ﱺţ�����ʱ�û��Զ�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/

void DispMenuBuiltInFileLongset(void)
{
	/*
	�����ж�ʱ����ﱻ����û��Զ���ﱣ���name_str�Ƿ���ڵ���Outside_file_add_num[0]��������Ϊ����ﱣ���С��Ϊ�û��Զ���ﱡ�
	
	���������ﱣ���ִ���κβ�������ת��106��
	������Զ���ﱣ����ж��ָ�봦��ʲôλ�á�
	���ָ��Seq_parameter_str���ڵ�����������������������1����ִ���κβ�������ת��106��
	���ָ��Seq_parameter_strС����������������������1������ת��179����ѯ���Ƿ�ɾ��������
	*/
	if(name_str >= Outside_file_add_num[0])
	{
		Jump2Menu(106);																				// ��ִ���κβ�������
	}else
	{
		if((Seq_parameter_str >= New_file_parameter.seq_Len)||(New_file_parameter.seq_Len == 1))
		{
			Jump2Menu(106);
		}else
		{
			NeedFlash = 1;
			sel_page_flash = 1;
			Jump2Menu(179);																			// ѯ���Ƿ�ɾ��
			file_seq_flag = 1;																		// file_seq_flag��ʾ��ʲô�ط�����ɾ��������
																									// ��file_seq_flag = 0����ʾ����ļ�����ɾ����
																									// ��file_seq_flag = 1����ʾ����ļ��µ���ﱽ���ɾ����
		}	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuFilelistInit(void)
*	����˵��: ��ʾ��ļ����б�
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistInit(void)
{
	/*
		�˴�name_str = -1;��ʾָ�봦�������ļ���ť���������
	*/
	int i=0;
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		name_str = -1;															//����-1��ʱ����ļ���ť
		Disp_list_str = 0;														//����ָ������
		Read_File_number(Outside_file_add_num);									//��ȡĿǰ����ӵ���ļ�����
		for(i=0; i < Outside_file_add_num[0]; i++)								//��ȡ��ļ���
		{
			ReadReName(i);
		}
		
		Filelist_init(Seq_Inside, Disp_list_str, name_str , Outside_file_add_num[0]);
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFilelistON(void)
*	����˵��: ��ʾ��ļ����б����¡�ON����������������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistON(void)
{
	Jump2Menu(0);														// �ޱ��淵��
	sel_page_flash = 1;
	NeedFlash = 1;
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFilelistSel(void)
*	����˵��: ��ʾ��ļ����б����¡�SEL��������ѡ��Myfile��������ת�������棬�����½��ļ��У�������112����������ﱣ�������ﱲ�����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistSel(void)
{
	/*
		�����ж���ļ���ָ�봦��ʲôλ�á�
		���name_str��Outside_file_add_num[0](�û��Զ����ļ�������)��ͬ�����ڡ�myfile������λ���ݣ���ԭ����õ�Ĭ�ϡ�
		���name_str = -1������롰+����ť��ѡ��������ļ���Ӳ�������ʱ���Outside_file_add_num[0]�����ļ�������������ֵ������ת���������ڴ����������档Ȼ�󷵻���ļ��б���档
		���name_str������-1��������Outside_file_add_num[0]���������ļ��У�������ת��106��
	*/
	if(name_str == Outside_file_add_num[0])
	{ 
		sel.taper = 0;
		sel.number = 0;
		sel.AutoStart = 0;
		sel.AutoStop = 0;
		sel.FlashBarPosition = 0;
		sel.FileLibrary = 0;
		sel.FileSeq = 0;
		sel.color = 0;
		sel.FileSave = 0;
		MemorySave(mem.MemoryNum);
		NeedFlash = 1;
		sel_page_flash = 1;
		Jump2Menu(0);
	}
	else if(name_str == -1)
	{
		Outside_file_add_num[0]++ ;										// �½��ļ��У��ļ���������1	
		if(Outside_file_add_num[0] > 10)
		{
			Outside_file_add_num[0] --;
			lv_memory_full();											// ���á��ڴ�����������
			lv_task_handler();											// ����һ��ˢ������
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			Jump2Menu(118);												// �ٷ��ص�ǰ����
			NeedFlash = 1;
			sel_page_flash = 1;
		}else
		{
			strcpy(ReName[Outside_file_add_num[0] - 1],"New file");		// ��ʼ����Ҫ��������ļ�		
			SaveReName(Outside_file_add_num[0] - 1);					// ���˴�����ʼ�����������δ֪����
			Jump2Menu(112);												// ��ת����ļ�����������
		}
		sel_page_flash = 1;
	 	NeedFlash = 1;
	}else
	{
	 	Jump2Menu(106);													// ����ļ���
		sel_page_flash = 1;
	 	NeedFlash = 1;
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuFilelistSel(void)
*	����˵��: ��ʾ��ļ����б����¡�UP������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistUp(void)
{
	name_str ++;
	if(name_str >=(Inside_file_num + Outside_file_add_num[0]))
	{
		name_str =  - 1;
		Disp_list_str = 0;
	}else
	{
		Disp_list_str++;
		if(Disp_list_str > 3)
		{
			Disp_list_str = 3;
		}
	}
	
    Filelist_init(Seq_Inside, Disp_list_str, name_str ,Outside_file_add_num[0]);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFilelistSel(void)
*	����˵��: ��ʾ��ļ����б����¡�DOWN������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistDown(void)
{
	name_str --;
	if(name_str < -1)
	{
		name_str = Inside_file_num + Outside_file_add_num[0] - 1;
		Disp_list_str = 3;
	}else
	{
		Disp_list_str --;
		if(Disp_list_str  < 0)
		{
			Disp_list_str = 0;
		}
	}
	Filelist_init(Seq_Inside, Disp_list_str, name_str ,Outside_file_add_num[0]);
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuFilelistSel(void)
*	����˵��: ��ʾ��ļ����б������¡�SEL������ѯ���Ƿ�ɾ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuFilelistLongSel(void)
{
	/*
	�����ж�name_str�Ƿ�С��Outside_file_add_num[0](�û��Զ�����ļ�����)����С�ڣ���name_str != -1��������ת��179��ѯ���Ƿ�ɾ������file_seq_flag = 0����֪�Ǵ���ļ��б����ɾ�����档
	�����������������ִ���κβ�����	
	*/
	if((name_str < Outside_file_add_num[0]) && (name_str != -1))
	{
		NeedFlash = 1;
		sel_page_flash = 1;
		Jump2Menu(179);	
		file_seq_flag = 0;																			// file_seq_flag��ʾ��ʲô�ط�����ɾ��������
																									// ��file_seq_flag = 0����ʾ����ļ�����ɾ����
																									// ��file_seq_flag = 1����ʾ����ļ��µ���ﱽ���ɾ����
	}else
	{
		Jump2Menu(118);
	}
//	Jump2Menu(118);	
	
}
/******************************* ����� *************************************/
char * string ;
/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileInit(void)
*	����˵��: �����ļ�
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileInit(void)
{
	if(sel_page_flash)
	{
		new_file_flag = File_name_flag;													// �����½���ļ��д��������׶α�־λ
		sel_page_flash = 0;
		Seq_set_str = 0;																// ������б�ָ��
		
		if(name_str == -1)																// ����ļ�����ָ��=-1���������ﱣ��������û��Զ���ﱸ���
		{
			ReadReName(Outside_file_add_num[0] - 1);									// ��EEPROM��ȡ��ļ�����
			string = ReName[Outside_file_add_num[0] - 1];								// ���ַ�������
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			ReadReName(name_str);
			string = ReName[name_str];
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileON(void)
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileON(void)
{
	if(name_str != -1)
	{
		SaveReName(name_str);
	}	
	Jump2Menu(118);				
	NeedFlash = 1;
	sel_page_flash = 1;	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileSel(void)
*	����˵��: �����ļ�,���¡�SEL����������һ������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileSel(void)
{
	/*
	�ж�name_str�Ƿ����-1�����name_str = -1���������һ�����ý��档
	���name_str������-1���򱣴���������ء�
	*/
	if(name_str != -1)
	{
		SaveReName(name_str);
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}else
	{
//		Jump2Menu(134);				
//		new_file_flag = Seq_name_flag;								//��ʾﱸ������ý���
		Jump2Menu(139);				
		new_file_flag = Taper_flag;									//��ʾﱸ������ý���
		New_file_parameter.seq_Len = 0;								//�ļ�����������
		New_file_parameter.seq_new_Len = 1;							//�ļ�����������
		NeedFlash = 1;
		sel_page_flash = 1;
	}
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileUp(void)
*	����˵��: �����ļ�,���¡�Up�����޲���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileUp(void)
{
	Jump2Menu(112);	
};
/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileDown(void)
*	����˵��: �����ļ�,���¡�DOWN�����޲���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileDown(void)
{  
	Jump2Menu(112);	
};

/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileLongSel(void)
*	����˵��: �����ļ��������¡�SEL�����������ı�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispMenuRenameFileLongSel(void)
{
 	Jump2Menu(129);				// ��ת�����̣��༭�ı�		
	NeedFlash = 1;
	sel_page_flash = 1;	
};

uint8_t ShiftBig;				//��Сд�л�
uint8_t key_str;				//����ָ��
uint8_t key_Pr;					//���֡���ĸ�л�������Ϊ1����ĸΪ0


/**********************************************************************************************************
*	�� �� ��: void DispFileKeytextInit()
*	����˵��: ���̽���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(NeedFlash == 1)
		{	
			NeedFlash = 0;							
			ShiftBig = 0;								// ��ʼ����Сд��Ĭ��Сд
			key_str = 0;								// ��ʼ�����̰�ѹλ�ã�Ĭ�ϴ���0λ��
			key_Pr = 0;									// ���֡���ĸ�л�������Ϊ1����ĸΪ0			
		}
		if(name_str == -1)								// �ж��Ƿ����½���ļ�
		{
			New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
		}else
		{															
			New_File_abckeyname(ReName[name_str], 0, 0, 0);	
		}	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileKeytextON()
*	����˵��: ���ڼ��̿�ݼ����������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextON()
{
	uint8_t string_len;																				
	string_len = strlen(ReName[Outside_file_add_num[0] - 1]);										// ��ȡ�ַ�������
	ReName[Outside_file_add_num[0] - 1][string_len-1] = 0;											// ������֣�����Ӧλֱ������Ϊ0
	
	Jump2Menu(129);																					// ��ת�����̳�ʼ��λ��
	if(name_str == -1)																				// �ж��Ƿ����½���ļ�
	{	
		New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
	}else
	{
		New_File_abckeyname(ReName[name_str], 0, 0, 0);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispMenuRenameFileSel(void)
*	����˵��: ���̽���,���¡�SEL����ѡ��Ŀǰ���µ��ı�
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextSel()
{
	uint8_t string_len;
	string_len = strlen(ReName[Outside_file_add_num[0] - 1]);										// ��ȡ�ַ�������
	if(key_Pr == 0)																					// ���֡���ĸ�л�������Ϊ1����ĸΪ0							
	{
		if(key_str == 24)																			// �������˳�
		{
			strcpy(ReName[Outside_file_add_num[0] - 1], string);																
			Jump2Menu(112);
			SaveReName(Outside_file_add_num[0] - 1);
			NeedFlash = 1;
			sel_page_flash = 1;	
		}else if(key_str == 33)																		// �����˳�
		{
			Jump2Menu(112);
			SaveReName(Outside_file_add_num[0] - 1);
			NeedFlash = 1;
			sel_page_flash = 1;	
		}
		else 
		{
			if(	((key_str >= 1) && (key_str <= 10))
			||	((key_str >= 13) && (key_str <= 23))
			||	((key_str >= 25) && (key_str <= 32)))
			{
				if(string_len < 10)																	// ����С��10��������ַ������򱣴治��
				{
					if(ShiftBig == 1)
					{
						strcat(ReName[Outside_file_add_num[0] - 1], keynameTabBig[key_str]);
					}else
					{
						strcat(ReName[Outside_file_add_num[0] - 1], keynameTab[key_str]);
					}	
				}	
			}else if(key_str == 11)																	// ��������ַ�
			{
				ReName[Outside_file_add_num[0] - 1][string_len-1] = 0;		
			}else if(key_str == 12)																	// ��д��Сд��ĸ�л�
			{
				if(ShiftBig == 1)
				{
					ShiftBig = 0;
				}else
				{
					ShiftBig = 1;
				}		
			} else if(key_str == 0)																	// ���ּ��̡���ĸ�����л�
			{
				key_str =0;
				key_Pr = 1;	
			}
			
			Jump2Menu(129);
			if(name_str == -1)
			{	
				New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
			}else
			{
				New_File_abckeyname(ReName[name_str], 0, 0, 0);	
			}
		}
	}
	else
	{
		if(key_str == 0)																			// ���ּ���
		{
			key_str =0;
			key_Pr = 0;
			ShiftBig = 0;
		}else
		{
			if(string_len < 10)
			{
				strcat(ReName[Outside_file_add_num[0] - 1], key123nameTab[key_str]);
			}	
		}
		Jump2Menu(129);
		if(name_str == -1)
		{	
			New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
		}else
		{
			New_File_abckeyname(ReName[name_str], 0, 0, 0);	
		}	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileKeytextDown()
*	����˵��: �����ļ�,���¡�DOWN������������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextDown()
{
	key_str--;
	if(key_Pr == 0)																					// ��ĸ���̣�33��һѭ�������ּ��̣�10��һѭ��
	{
		if((key_str > 33) || (key_str < 0))
		{
			key_str = 33;		
		}
	}else
	{
		if((key_str > 10) || (key_str < 0))
		{
			key_str = 10;		
		}	
	}
	
	if(name_str == -1)
	{
		New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
	}else
	{
		New_File_abckeyname(ReName[name_str], 0, 0, 0);	
	}	

}
/**********************************************************************************************************
*	�� �� ��: void DispFileKeytextUp()
*	����˵��: �����ļ�,���¡�UP������������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextUp()
{
	key_str++;
	if(key_Pr == 0)
	{
		if(key_str > 33)
		{
			key_str = 0;		
		}
	}else
	{
		if(key_str > 10)
		{
			key_str = 0;		
		}
	}
	
	if(name_str == -1)
	{
		New_File_abckeyname(ReName[Outside_file_add_num[0] - 1], ShiftBig, key_str, key_Pr);
	}else
	{
		New_File_abckeyname(ReName[name_str], 0, 0, 0);	
	}	
}

/**********************************************************************************************************
*	�� �� ��: void DispFileKeytextUp()
*	����˵��: �������,���¡�sel������������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileKeytextLongSel()
{
	Jump2Menu(112);
	SaveReName(Outside_file_add_num[0] - 1);
	NeedFlash = 1;
	sel_page_flash = 1;	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileSeqnumberInit()
*	����˵��: �ļ�����ﱲ�������ģʽ����ʾﱸ���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/	
void DispFileSeqnumberInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)																
		{
			New_file_parameter.seq_Len = 0;												//�ļ���������
			New_file_parameter.seq_new_Len = 1;											//�ļ�����������
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);

		}else 
		{
			New_file_parameter.seq_new_Len = 1;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
		}
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileSeqnumberON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}

/**********************************************************************************************************
*	�� �� ��: DispFileSeqnumberSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL����������׶������ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberSel()
{
	Jump2Menu(139);				
	new_file_flag = Taper_flag;								//��ʾ׶�����ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileSeqnumberDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberDown()
{
	New_file_parameter.seq_new_Len--;
	if((New_file_parameter.seq_new_Len) < 1)
	{
		New_file_parameter.seq_new_Len = 1;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}	
}

/**********************************************************************************************************
*	�� �� ��: void DispFileSeqnumberUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSeqnumberUp()
{	
	New_file_parameter.seq_new_Len++;
	if((New_file_parameter.seq_Len + New_file_parameter.seq_new_Len) > 6)
	{
		New_file_parameter.seq_new_Len--;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileTaperInit()
*	����˵��: �ļ�����׶������ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/		
void DispFileTaperInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileTaperON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}

/**********************************************************************************************************
*	�� �� ��: void DispFileTaperSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL����������ﱺ�����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperSel()
{
	Jump2Menu(144);				
	new_file_flag = Number_flag;								//��ʾﱺ����ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTaperDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper < 0) ||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper > 15))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTaperUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTaperUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper > 15)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].taper = 15;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileTaperInit()
*	����˵��: �ļ�����׶������ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/		
void DispFileTapernumberInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}

}

/**********************************************************************************************************
*	�� �� ��: void DispFileTapernumberON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTaperSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL�������������ģʽ����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberSel()
{
	Jump2Menu(149);				
	new_file_flag = OperaMode_flag;								//��ʾ����ģʽ���ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTapernumberDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number < 0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number >90))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTapernumberUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTapernumberUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number > 90)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].number = 90;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileOperationInit()
*	����˵��: �ļ�����׶������ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/			
void DispFileOperationInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	

}
/**********************************************************************************************************
*	�� �� ��: void DispFileTaperSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL�����������ٶ�����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationSel()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 1))
	{
		Jump2Menu(154);				
		new_file_flag = RecSpeed_flag;								//��ʾ�ٶ����ý���
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(164);				
		new_file_flag = AngleCW_flag;								//��ʾ���Ƕ����ý���
		NeedFlash = 1;
		sel_page_flash = 1;
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationDown()
{
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode < 1)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 3;
	}else
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode--;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileOperationUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode > 3)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileSpeedInit()
*	����˵��: �ļ�����׶������ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/	
void DispFileSpeedInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileSpeedSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL����������ת������ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedSel()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode == 1))
	{
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s300)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t40)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t40;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed>s700)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t20)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t20;
			}
		}
	}
	Jump2Menu(159);				
	new_file_flag = torque_flag;								//��ʾת�����ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileSpeedDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > sel.Speed_len))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = sel.Speed_len;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileSpeedUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileSpeedUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > sel.Speed_len)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileTorqueInit()
*	����˵��: �ļ�����ת������ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/		
void DispFileTorqueInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTorqueSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL������������ɫ����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueSel()
{
	Jump2Menu(174);				
	new_file_flag = color_flag;								//��ʾ��ɫ���ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTorqueDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueDown()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==1))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque--;
		if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > sel.Torque_len))
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = sel.Torque_len;
		}
		/***********��ת��Ť������**************/
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s300)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t40)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t40;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s700)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t20)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t20;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s1200)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t10)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = t10;
			}
		}
	}

	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTorqueUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileTorqueUp()
{
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==0) || (New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].OperaMode==1))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque++;
		if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > sel.Torque_len))
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
		}
		/***********��ת��Ť������**************/
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s300)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t40)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s700)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t20)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			}
		}
		if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].speed > s1200)
		{
			if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque > t10)
			{
				New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].torque = 0;
			}
		}
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileCWAngleInit()
*	����˵��: �ļ��������Ƕ�����ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/		
void DispFileCWAngleInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTaperSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL���������븺�Ƕ�����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleSel()
{
	Jump2Menu(169);				
	new_file_flag = AngleCCW_flag;								//��ʾ���Ƕ����ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileCWAngleDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW < sel.AngleCW_len))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = sel.AngleCW_len;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileCWAngleUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCWAngleUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW > sel.AngleCW_len)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCW = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileCWAngleInit()
*	����˵��: �ļ��������Ƕ�����ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/		
void DispFileCCWAngleInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileOperationON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileCCWAngleSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL������������ɫ����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleSel()
{
	Jump2Menu(174);				
	new_file_flag = color_flag;								//��ʾ��ɫ���ý���
	NeedFlash = 1;
	sel_page_flash = 1;
}
/**********************************************************************************************************
*	�� �� ��: void DispFileCCWAngleDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleDown()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW --;
	if((New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW < 0)||(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW > sel.AngleCCW_len))
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = sel.AngleCCW_len;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileCCWAngleUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileCCWAngleUp()
{
	New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW ++;
	if(New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW > sel.AngleCCW_len)
	{
		New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].AngleCCW = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileColorInit()
*	����˵��: �ļ�������ɫ����ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/		
void DispFileColorInit()
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		if(name_str == -1)
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].color = 0;
			New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}else 
		{
			New_file_parameter.fseq[Seq_set_str + New_file_parameter.seq_Len].color = 0;
			New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
		}
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileColorON()
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileColorON()
{
	if(name_str == -1)
	{
		Jump2Menu(118);				
		NeedFlash = 1;
		sel_page_flash = 1;
	}else
	{
		Jump2Menu(106);				
		NeedFlash = 1;
		sel_page_flash = 1;	
	}	
}
/**********************************************************************************************************
*	�� �� ��: void DispFileTaperSel()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�SEL����������ﱺ�����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileColorSel()
{
	Seq_set_str ++;
	if(Seq_set_str < New_file_parameter.seq_new_Len)
	{		
		Jump2Menu(139);
		new_file_flag = Taper_flag;								//��ʾ׶�����ý���
		NeedFlash = 1;
		sel_page_flash = 1;	
	}
	else
	{
		if(name_str == -1)
		{
			New_file_parameter.seq_Len += New_file_parameter.seq_new_Len;
			SaveOutsideseq(Outside_file_add_num[0]-1);	
			Write_File_number(Outside_file_add_num);
			SaveReName(Outside_file_add_num[0]-1);
			
//			Outside_file_add_num[0] --;
//			Read_File_number(Outside_file_add_num);
			Seq_set_str = 0;									//��λָ��
			Jump2Menu(118);										//��ת���ļ����б�
			NeedFlash = 1;
			sel_page_flash = 1;	
		}else
		{
			New_file_parameter.seq_Len += New_file_parameter.seq_new_Len;
			SaveOutsideseq(name_str);
			Seq_set_str = 0;									//��λָ��
			Jump2Menu(106);										//��ת���ļ����б�
			NeedFlash = 1;
			sel_page_flash = 1;			
		}

	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileColorDown()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileColorDown()
{
	New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color --;
	if((New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color < 0)||(New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color > 8))
	{
		New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color = 8;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileColorUp()
*	����˵��: �ļ�����ﱲ�������ģʽ,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210917         
**********************************************************************************************************/
void DispFileColorUp()
{
	New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color ++;
	if(New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color > 8)
	{
		New_file_parameter.fseq[Seq_set_str+New_file_parameter.seq_Len].color = 0;
	}
	if(name_str == -1)
	{
		New_File_init(Outside_file_add_num[0] - 1, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);
	}else 
	{
		New_File_init(name_str, New_file_parameter, New_file_parameter.seq_Len ,Seq_set_str);	
	}
}
/**********************************************************************************************************
*	�� �� ��: void DispFileDeleteInit(void)
*	����˵��: ɾ������
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211023         
**********************************************************************************************************/	
void DispFileDeleteInit(void)
{
	if(sel_page_flash)
	{
		sel_page_flash = 0;
		lv_delete_style(yes_on_flag);
	}	
}

/**********************************************************************************************************
*	�� �� ��: void DispFileDeleteON(void)
*	����˵��: �����ļ���������������򲻱��棬������޸ģ��򱣴����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211023          
**********************************************************************************************************/
void DispFileDeleteON(void)
{
	Jump2Menu(179);
	NeedFlash = 1;
	sel_page_flash = 1;	
}

/**********************************************************************************************************
*	�� �� ��: void DispFileTaperSel()
*	����˵��: ɾ����ʾ�£�ȷ��ѡ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211023          
**********************************************************************************************************/
void DispFileDeleteSel(void)
{
	int i;
	char *name = "New file   ";
	if(file_seq_flag)
	{
		if(yes_on_flag)
		{
			if((New_file_parameter.seq_Len > 1)&&(Seq_parameter_str < (New_file_parameter.seq_Len-1)))
			{
				for(i = Seq_parameter_str; i < New_file_parameter.seq_Len; i++)
				{
					New_file_parameter.fseq[i].taper = New_file_parameter.fseq[i+1].taper;
					New_file_parameter.fseq[i].number = New_file_parameter.fseq[i+1].number;
					New_file_parameter.fseq[i].OperaMode = New_file_parameter.fseq[i+1].OperaMode;
					New_file_parameter.fseq[i].color = New_file_parameter.fseq[i+1].color;
					if((New_file_parameter.fseq[i+1].OperaMode == 0) || (New_file_parameter.fseq[i+1].OperaMode == 1))
					{
						New_file_parameter.fseq[i].speed = New_file_parameter.fseq[i+1].speed;
						New_file_parameter.fseq[i].torque = New_file_parameter.fseq[i+1].torque;
						New_file_parameter.fseq[i].AngleCW = 0;
						New_file_parameter.fseq[i].AngleCCW = 0;
					}else 
					{
						New_file_parameter.fseq[i].speed = 0;
						New_file_parameter.fseq[i].torque = 0;
						New_file_parameter.fseq[i].AngleCW = New_file_parameter.fseq[i+1].AngleCW;
						New_file_parameter.fseq[i].AngleCCW = New_file_parameter.fseq[i+1].AngleCCW;
					}
				}
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].taper = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].number = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].OperaMode = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].color = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].speed = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].torque = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCW = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCCW = 0;	
				
				New_file_parameter.seq_Len --;
				SaveOutsideseq(name_str);
			}
			else if((New_file_parameter.seq_Len > 1)&&(Seq_parameter_str == (New_file_parameter.seq_Len-1)))
			{
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].taper = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].number = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].OperaMode = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].color = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].speed = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].torque = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCW = 0;
				New_file_parameter.fseq[New_file_parameter.seq_Len-1].AngleCCW = 0;	
				
				New_file_parameter.seq_Len --;
				Seq_parameter_str--;
				SaveOutsideseq(name_str);				
			}	
		}
		Jump2Menu(106);
		NeedFlash = 1;
		sel_page_flash = 1;	
//		Seq_Outsidelist_init(New_file_parameter, name_str, Seq_Disp_str, Seq_parameter_str);	
	}else
	{
		if(yes_on_flag)
		{
			for(i = name_str; i < Outside_file_add_num[0]-1; i++)
			{
				ReadReName(i+1);
				strcpy(ReName[i], ReName[i+1]);
				SaveReName(i);
				ReadOutsideseq(i+1);
				SaveOutsideseq(i);
			}
			strcpy(ReName[Outside_file_add_num[0] - 1], "New file");
			SaveReName(Outside_file_add_num[0] - 1);
			Outside_file_add_num[0]--;
			Write_File_number(Outside_file_add_num);
		}
		Jump2Menu(118);
		NeedFlash = 1;
		sel_page_flash = 1;	
//		Filelist_init(Seq_Inside, Disp_list_str, name_str ,Outside_file_add_num[0]);		
	}
}

/**********************************************************************************************************
*	�� �� ��: void DispFileDeleteDown(void)
*	����˵��: ɾ����ʾ��,���¡�DOWN����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211023         
**********************************************************************************************************/
void DispFileDeleteDown(void)
{
	if(yes_on_flag)
	{
		yes_on_flag = 0;
	}else
	{
		yes_on_flag = 1;
	}
	lv_delete_style(yes_on_flag);
}

/**********************************************************************************************************
*	�� �� ��: void DispFileDeleteUp(void)
*	����˵��: ɾ����ʾ��,���¡�UP����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211023         
**********************************************************************************************************/
void DispFileDeleteUp(void)
{
	if(yes_on_flag)
	{
		yes_on_flag = 0;
	}else
	{
		yes_on_flag = 1;
	}
	lv_delete_style(yes_on_flag);	
}









