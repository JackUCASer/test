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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-11-25
* 函 数 名: creat_page_objs
* 功能说明: 初始化界面对象
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void Init_Page_Objs(void)
{
	vgus_page_objs = (VGUS_PAGE_S *)malloc(sizeof(VGUS_PAGE_S)*N_PAGE);	//	实例化对象，分配内存控件
	
	//	界面0: PwrOn_End 		无需重新装载
	vgus_page_objs[PAGE_PowerOn].obj = PAGE_PowerOn;
	vgus_page_objs[PAGE_PowerOn].obj_num = 1;
	vgus_page_objs[PAGE_PowerOn].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_PowerOn].obj_value[0] = 0x00;
	
	//	界面1：CheckHandle_1		无需重新装载
	vgus_page_objs[PAGE_CheckHandle].obj = PAGE_CheckHandle;
	vgus_page_objs[PAGE_CheckHandle].obj_num = 2;
	vgus_page_objs[PAGE_CheckHandle].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckHandle].obj_value[0] = 1;
	vgus_page_objs[PAGE_CheckHandle].obj_id[1] = 0x02;
	vgus_page_objs[PAGE_CheckHandle].obj_value[1] = 0;
	//	界面2： CheckHandle_2	无需重新装载:用作于界面切换效果
	vgus_page_objs[PAGE_CheckHandle2].obj = PAGE_CheckHandle2;
	vgus_page_objs[PAGE_CheckHandle2].obj_num = 1;
	vgus_page_objs[PAGE_CheckHandle2].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckHandle2].obj_value[0] = 1;
	
	//	界面3： CheckORing_1		无需重新装载
	vgus_page_objs[PAGE_CheckOring].obj = PAGE_CheckOring;
	vgus_page_objs[PAGE_CheckOring].obj_num = 2;
	vgus_page_objs[PAGE_CheckOring].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckOring].obj_value[0] = 1;
	vgus_page_objs[PAGE_CheckOring].obj_id[1] = 0x02;
	vgus_page_objs[PAGE_CheckOring].obj_value[1] = 0;
	//	界面4：	CheckORing_2	无需重新装载:用作于界面切换效果
	vgus_page_objs[PAGE_CheckOring2].obj = PAGE_CheckOring2;
	vgus_page_objs[PAGE_CheckOring2].obj_num = 1;
	vgus_page_objs[PAGE_CheckOring2].obj_id[0] = 0x01;
	vgus_page_objs[PAGE_CheckOring2].obj_value[0] = 1;
	
	//	界面5：	Mode1
	vgus_page_objs[PAGE_Mode].obj = PAGE_Mode;
	vgus_page_objs[PAGE_Mode].obj_num = 3;			//	共3个对象：回吸、按钮、定量值------其它无需管理（药液雨量、动态压力、kPa、脚踏、速度）
	vgus_page_objs[PAGE_Mode].obj_id[0] = 0x01;		//	回吸
	vgus_page_objs[PAGE_Mode].obj_value[0] = 0;			//	0 or 1
	vgus_page_objs[PAGE_Mode].obj_id[1] = 0x07;		//	按钮
	vgus_page_objs[PAGE_Mode].obj_value[1] = 0;			//	0x00、0x01、0x02、0x04、0x08、0x10、0x20共七种状态
	vgus_page_objs[PAGE_Mode].obj_id[2] = 0x08;		//	定量值
	vgus_page_objs[PAGE_Mode].obj_value[2] = 0;			//	0 -- 35
	
	//	界面6：	Mode2
	vgus_page_objs[PAGE_Mode2].obj = PAGE_Mode2;
	vgus_page_objs[PAGE_Mode2].obj_num = 0;			//	PAGE_Mode2显示的对象均在界面PAGE_Mode中
	
	//	界面7： Setting
	vgus_page_objs[PAGE_Setting].obj = PAGE_Setting;	//	共4个对象：智能提醒、训练模式、亮度、音量
	vgus_page_objs[PAGE_Setting].obj_num = 4;
	vgus_page_objs[PAGE_Setting].obj_id[0] = 0x03;		//	智能提醒id
	vgus_page_objs[PAGE_Setting].obj_value[0] = 0;			//	0 or 1
	vgus_page_objs[PAGE_Setting].obj_id[1] = 0x04;		//	训练模式id
	vgus_page_objs[PAGE_Setting].obj_value[1] = 0;			//	0 or 1
	vgus_page_objs[PAGE_Setting].obj_id[2] = 0x05;		//	亮度id
	vgus_page_objs[PAGE_Setting].obj_value[2] = 0;			//	index = {0,1,2,3,4,5}
	vgus_page_objs[PAGE_Setting].obj_id[3] = 0x06;		//	音量id
	vgus_page_objs[PAGE_Setting].obj_value[3] = 0;			//	index = {0,1,2,3,4,5}
	
	//	界面8： Calibration
	vgus_page_objs[PAGE_MotorCalib].obj = PAGE_MotorCalib;//	共1个对象：脚踏校正
	vgus_page_objs[PAGE_MotorCalib].obj_num = 1;
	vgus_page_objs[PAGE_MotorCalib].obj_id[0] = 0x01;	//	脚踏校正按钮id
	vgus_page_objs[PAGE_MotorCalib].obj_value[0] = 0;		//	0 or 1
	
	//	界面9： Aging
	vgus_page_objs[PAGE_Aging].obj = PAGE_Aging;		//	共2个对象：老化启停按钮
	vgus_page_objs[PAGE_Aging].obj_num = 1;
	vgus_page_objs[PAGE_Aging].obj_id[0] = 0x03;		//	老化启停按钮id
	vgus_page_objs[PAGE_Aging].obj_value[0] = 0;			//	0 or 1

}
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
		float bat_vol = Get_Battery_Vol(); 
		float chg_vol = Get_Charger_Vol();
		
		bat_vol = Movmean_Filter3(bat_vol, buf, 5);	// 对电池电压进行滑动滤波
		
		//	1. 判断当前适配器电压
		if(chg_vol > 19.2f){
			charging_flag = true;		//	有适配器接入
			already_notice_bat = false;
			sys_bat_too_low = false;	//	解除电池电压过低保护
			ENABLE_CHARGE();
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
			DISABLE_CHARGE();
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
		Write_Two_82H_Cmd(PAGE_BAT_ID, display_battery_id, (Is_Autoback_Off() == true ? 1: 0));
		
		//	4. 低电量提醒
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-11-24
* 函 数 名: battery_and_charge_monitor
* 功能说明: 电池电量以及其充电监测,充电时电池电压检测步骤：失能充电 --> 检测电压 --> 使能充电
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
static uint8_t now_battery_id = 0;			//	当前电池电压图标ID
static uint8_t end_battery_id = 0;
static bool valid_charge_adapter = false;	//	充电适配器有效标识符
static void battery_and_charge_monitor(void)
{
	float chg_vol = 0;
	static float bat_vol = 0;
	static uint8_t time_cnt = 0;
	static float buf[5] = {0};						//	用于滑动滤波的缓存

	/*	第一步： 检测是否有效的适配器接入	*/
	chg_vol = Get_Charger_Vol();
	if((chg_vol >= 16.0f)&&(chg_vol <= 24.0f)){
		valid_charge_adapter = true;
		ENABLE_CHARGE();							//	开启充电
	}else{
		valid_charge_adapter = false;
		DISABLE_CHARGE();							//	关闭充电
	}
	
	/*	第二步： 检测电池电压	*/
	if(valid_charge_adapter == true){
		time_cnt ++;
		if(time_cnt % 66 == 1){						//	10s检测一次
			DISABLE_CHARGE();						//	关闭充电
			time_cnt &= 0x01;
			osDelay(300);
			bat_vol = Get_Battery_Vol();
			ENABLE_CHARGE();						//	开启充电
		}
	}else
		bat_vol = Get_Battery_Vol();
		
	/*	第三步： 确定电池电量	*/
	bat_vol = Movmean_Filter3(bat_vol, buf, 5);		// 对电池电压进行滑动滤波
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-11-24
* 函 数 名: Display_Battery
* 功能说明: 电量显示
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void Display_Battery(void)
{
	uint8_t now_display_bat_id = 0;			//	当前显示的电池id
	static uint8_t n_update_cnt = 0;
	static uint8_t cnt = 0;
	static bool already_notice_bat = false;
	static uint8_t last_display_bat_id = 4;	//	用于记录上次显示的电池id，主要用于非充电状态
	
	if(++n_update_cnt % N_UPDATE_BAT == 0){
		battery_and_charge_monitor();
		now_display_bat_id = now_battery_id;
		if(valid_charge_adapter == true){	//	充电状态：动态更新电量图标
			already_notice_bat = false;
			last_display_bat_id = 4;
			now_display_bat_id = ((now_battery_id + cnt) > end_battery_id ? now_battery_id : (now_battery_id + cnt));
			Write_Two_82H_Cmd(PAGE_BAT_ID, now_display_bat_id, (Is_Autoback_Off() == true ? 1: 0));
			if(now_display_bat_id >= end_battery_id)
				cnt = 0;
			else
				cnt ++;
		}else{	//	非充电状态: 电池图标只能向电量减少的方向显示
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


