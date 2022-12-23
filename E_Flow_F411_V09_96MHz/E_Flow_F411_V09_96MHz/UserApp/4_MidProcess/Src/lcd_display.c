#include "lcd_display.h"
#include "vgus_lcd_if.h"
#include "user_data.h"
#include "get_voltage.h"
#include "power_control.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

#include "movmean.h"

VGUS_PAGE_S *vgus_page_objs = NULL;
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-11-25
* �� �� ��: creat_page_objs
* ����˵��: ��ʼ���������
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void Init_Page_Objs(void)
{
	vgus_page_objs = (VGUS_PAGE_S *)malloc(sizeof(VGUS_PAGE_S)*N_PAGE);	//	ʵ�������󣬷����ڴ�ؼ�
	
	//	����0: PwrOn_End 		��������װ��
	vgus_page_objs[PAGE_PowerOn].obj = PAGE_PowerOn;
	vgus_page_objs[PAGE_PowerOn].obj_num = 1;
	vgus_page_objs[PAGE_PowerOn].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_PowerOn].obj_value[0] = 0x00;
	
	//	����1��CheckHandle_1		��������װ��
	vgus_page_objs[PAGE_CheckHandle].obj = PAGE_CheckHandle;
	vgus_page_objs[PAGE_CheckHandle].obj_num = 2;
	vgus_page_objs[PAGE_CheckHandle].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckHandle].obj_value[0] = 1;
	vgus_page_objs[PAGE_CheckHandle].obj_id[1] = 0x02;
	vgus_page_objs[PAGE_CheckHandle].obj_value[1] = 0;
	//	����2�� CheckHandle_2	��������װ��:�����ڽ����л�Ч��
	vgus_page_objs[PAGE_CheckHandle2].obj = PAGE_CheckHandle2;
	vgus_page_objs[PAGE_CheckHandle2].obj_num = 1;
	vgus_page_objs[PAGE_CheckHandle2].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckHandle2].obj_value[0] = 1;
	
	//	����3�� CheckORing_1		��������װ��
	vgus_page_objs[PAGE_CheckOring].obj = PAGE_CheckOring;
	vgus_page_objs[PAGE_CheckOring].obj_num = 2;
	vgus_page_objs[PAGE_CheckOring].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckOring].obj_value[0] = 1;
	vgus_page_objs[PAGE_CheckOring].obj_id[1] = 0x02;
	vgus_page_objs[PAGE_CheckOring].obj_value[1] = 0;
	//	����4��	CheckORing_2	��������װ��:�����ڽ����л�Ч��
	vgus_page_objs[PAGE_CheckOring2].obj = PAGE_CheckOring2;
	vgus_page_objs[PAGE_CheckOring2].obj_num = 1;
	vgus_page_objs[PAGE_CheckOring2].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckOring2].obj_value[0] = 1;
	
	//	����5��	Mode1
	vgus_page_objs[PAGE_Mode].obj = PAGE_Mode;
	vgus_page_objs[PAGE_Mode].obj_num = 3;			//	��3�����󣺻�������ť������ֵ------�����������ҩҺ��������̬ѹ����kPa����̤���ٶȣ�
	vgus_page_objs[PAGE_Mode].obj_id[0] = 0x01;		//	����
	vgus_page_objs[PAGE_Mode].obj_value[0] = 0;			//	0 or 1
	vgus_page_objs[PAGE_Mode].obj_id[1] = 0x07;		//	��ť
	vgus_page_objs[PAGE_Mode].obj_value[1] = 0;			//	0x00��0x01��0x02��0x04��0x08��0x10��0x20������״̬
	vgus_page_objs[PAGE_Mode].obj_id[2] = 0x08;		//	����ֵ
	vgus_page_objs[PAGE_Mode].obj_value[2] = 0;			//	0 -- 35
	
	//	����6��	Mode2
	vgus_page_objs[PAGE_Mode2].obj = PAGE_Mode2;
	vgus_page_objs[PAGE_Mode2].obj_num = 0;			//	PAGE_Mode2��ʾ�Ķ�����ڽ���PAGE_Mode��
	
	//	����7�� Setting
	vgus_page_objs[PAGE_Setting].obj = PAGE_Setting;	//	��4�������������ѡ�ѵ��ģʽ�����ȡ�����
	vgus_page_objs[PAGE_Setting].obj_num = 4;
	vgus_page_objs[PAGE_Setting].obj_id[0] = 0x03;		//	��������id
	vgus_page_objs[PAGE_Setting].obj_value[0] = 0;			//	0 or 1
	vgus_page_objs[PAGE_Setting].obj_id[1] = 0x04;		//	ѵ��ģʽid
	vgus_page_objs[PAGE_Setting].obj_value[1] = 0;			//	0 or 1
	vgus_page_objs[PAGE_Setting].obj_id[2] = 0x05;		//	����id
	vgus_page_objs[PAGE_Setting].obj_value[2] = 0;			//	index = {0,1,2,3,4,5}
	vgus_page_objs[PAGE_Setting].obj_id[3] = 0x06;		//	����id
	vgus_page_objs[PAGE_Setting].obj_value[3] = 0;			//	index = {0,1,2,3,4,5}
	
	//	����8�� Calibration
	vgus_page_objs[PAGE_MotorCalib].obj = PAGE_MotorCalib;//	��1�����󣺽�̤У��
	vgus_page_objs[PAGE_MotorCalib].obj_num = 1;
	vgus_page_objs[PAGE_MotorCalib].obj_id[0] = 0x01;	//	��̤У����ťid
	vgus_page_objs[PAGE_MotorCalib].obj_value[0] = 0;		//	0 or 1
	
	//	����9�� Aging
	vgus_page_objs[PAGE_Aging].obj = PAGE_Aging;		//	��2�������ϻ���ͣ��ť
	vgus_page_objs[PAGE_Aging].obj_num = 1;
	vgus_page_objs[PAGE_Aging].obj_id[0] = 0x03;		//	�ϻ���ͣ��ťid
	vgus_page_objs[PAGE_Aging].obj_value[0] = 0;			//	0 or 1

}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.2
* ��    �ڣ�2022-09-16
* �� �� ��: display_battary_state
* ����˵��: ��ʾ���״̬
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void display_battary_state(void)
{
	static bool already_notice_bat = false;
	static bool charging_flag = false;
	static float buf[5] = {0};				//	���ڻ����˲��Ļ���
	static uint8_t cnt,cnt2 = 0;			//	ÿ150ms����ѯһ��
	static uint8_t display_battery_id = 5;	//	ʵ����ʾ�ĵ��ID
	uint8_t now_battery_id = 0;				//	��ǰ��ص�ѹͼ��ID
	
	if(++cnt % N_UPDATE_BAT == 0){
		float bat_vol = Get_Battery_Vol(); 
		float chg_vol = Get_Charger_Vol();
		
		bat_vol = Movmean_Filter3(bat_vol, buf, 5);	// �Ե�ص�ѹ���л����˲�
		
		//	1. �жϵ�ǰ��������ѹ
		if(chg_vol > 19.2f){
			charging_flag = true;		//	������������
			already_notice_bat = false;
			sys_bat_too_low = false;	//	�����ص�ѹ���ͱ���
			ENABLE_CHARGE();
			//	2. ����������ʱ����ص�ѹ��̧��
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
			charging_flag = false;		//	δ����������
			DISABLE_CHARGE();
			//	2. �жϵ�ǰ��ص�ѹ
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
		
		//	3. ��̬���µ��ͼ��
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
		Write_Two_82H_Cmd(PAGE_BAT_ID, display_battery_id, (Is_Autoback_Off() == true ? 1: 0));
		
		//	4. �͵�������
		if((charging_flag == false)&&(already_notice_bat == false)&&(now_battery_id == 0)&&((Is_Smart_Reminder_Open() == true)||(Is_Train_Mode_Open() == true))){
			Play_Now_Music(BATTERY_LOW_INDEX, system_volume[Get_System_Vol()]);
			already_notice_bat = true;
		}else
			;
	}else
		return ;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-11-24
* �� �� ��: battery_and_charge_monitor
* ����˵��: ��ص����Լ�������,���ʱ��ص�ѹ��ⲽ�裺ʧ�ܳ�� --> ����ѹ --> ʹ�ܳ��
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
static uint8_t now_battery_id = 0;			//	��ǰ��ص�ѹͼ��ID
static uint8_t end_battery_id = 0;
static bool valid_charge_adapter = false;	//	�����������Ч��ʶ��
static void battery_and_charge_monitor(void)
{
	float chg_vol = 0;
	static float bat_vol = 0;
	static uint8_t time_cnt = 0;
	static float buf[5] = {0};						//	���ڻ����˲��Ļ���

	/*	��һ���� ����Ƿ���Ч������������	*/
	chg_vol = Get_Charger_Vol();
	if((chg_vol >= 16.0f)&&(chg_vol <= 24.0f)){
		valid_charge_adapter = true;
		ENABLE_CHARGE();							//	�������
	}else{
		valid_charge_adapter = false;
		DISABLE_CHARGE();							//	�رճ��
	}
	
	/*	�ڶ����� ����ص�ѹ	*/
	if(valid_charge_adapter == true){
		time_cnt ++;
		if(time_cnt % 66 == 1){						//	10s���һ��
			DISABLE_CHARGE();						//	�رճ��
			time_cnt &= 0x01;
			osDelay(300);
			bat_vol = Get_Battery_Vol();
			ENABLE_CHARGE();						//	�������
		}
	}else
		bat_vol = Get_Battery_Vol();
		
	/*	�������� ȷ����ص���	*/
	bat_vol = Movmean_Filter3(bat_vol, buf, 5);		// �Ե�ص�ѹ���л����˲�
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
	
	if(valid_charge_adapter == true){
		end_battery_id = 9;
		now_battery_id += 5;
		sys_bat_too_low = false;
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-11-24
* �� �� ��: Display_Battery
* ����˵��: ������ʾ
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void Display_Battery(void)
{
	uint8_t now_display_bat_id = 0;			//	��ǰ��ʾ�ĵ��id
	static uint8_t n_update_cnt = 0;
	static uint8_t cnt = 0;
	static bool already_notice_bat = false;
	static uint8_t last_display_bat_id = 4;	//	���ڼ�¼�ϴ���ʾ�ĵ��id����Ҫ���ڷǳ��״̬
	
	if(++n_update_cnt % N_UPDATE_BAT == 0){
		battery_and_charge_monitor();
		now_display_bat_id = now_battery_id;
		if(valid_charge_adapter == true){	//	���״̬����̬���µ���ͼ��
			already_notice_bat = false;
			last_display_bat_id = 4;
			now_display_bat_id = ((now_battery_id + cnt) > end_battery_id ? now_battery_id : (now_battery_id + cnt));
			Write_Two_82H_Cmd(PAGE_BAT_ID, now_display_bat_id, (Is_Autoback_Off() == true ? 1: 0));
			if(now_display_bat_id >= end_battery_id)
				cnt = 0;
			else
				cnt ++;
		}else{	//	�ǳ��״̬: ���ͼ��ֻ����������ٵķ�����ʾ
			now_display_bat_id = (now_display_bat_id <= last_display_bat_id ? now_display_bat_id : last_display_bat_id);
			Write_Two_82H_Cmd(PAGE_BAT_ID, now_display_bat_id, (Is_Autoback_Off() == true ? 1: 0));
			last_display_bat_id = now_display_bat_id;
		}
		
		if((sys_bat_too_low == true)&&(already_notice_bat == false)&&((Is_Smart_Reminder_Open() == true)||(Is_Train_Mode_Open() == true))){
			Play_Now_Music(BATTERY_LOW_INDEX, system_volume[Get_System_Vol()]);
			already_notice_bat = true;
		}	
	}
}


