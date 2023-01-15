#include "user_data.h"
#include "lcd_display.h"
#include "motor_misc.h"
#include "eeprom.h"
#include "power_control.h"
#include "get_voltage.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"


//	系统参数: 需写入数据库
SYS_PARA_T sys_para_t = {0};
uint32_t IDLE_TIME_CNT = 0;			//	用于系统空闲计数器，如果连续5分钟无操作，系统自动关机
CHARGE_STATE_E CHARGE_STATE = RIGHT_CHARGE;

//	脚踏: 需写入数据库
FOOT_PEDAL_T	foot_pedal_t = {0};

//	手柄: 需写入数据库: 用于记录手柄注射器的总行程[电机可运行圈数，角度]
HANDLE_INJECTOR_T handle_injector_t = {0};

//	液压参数
float32_t dynamic_pressure_mV = 0;
float32_t dynamic_pressure_kPa = 0;
uint32_t  dynamic_pressure = 0;
uint16_t  dynamic_pressure_kPa_F = 0;		/*	Hampel滤波后的液压值	*/

//	全局标志位
bool handle_in_top = false;				/*	丝杆处于顶端	*/
bool handle_in_bottom = false;			/*	丝杆处于底端	*/
bool handle_in_origin = false;			/*	丝杆处于原点	*/
bool sys_para_update_flag = false;		/*	程序升级标志	*/
bool sys_bat_too_low = false;			/*	电池电量过低	*/

//	软件版本
const uint8_t EB_VERSION[] = "1.1.1.001";

//	系统中的电机转速：不牵涉校正的问题，所以不用写入数据库
/* 	LOW,	MID,	HIGH	*/
const uint32_t injector_speed_t[4][3] = {
	{31.5*F_DIV,	31.5*F_DIV,		181*F_DIV},				/*	Normal	速度	[LOW, LOW, MID]	*/
	{181*F_DIV,		181*F_DIV,		380*F_DIV},				/*	Turbo 	速度	[MID, MID, HIGH]*/
	{31.5*F_DIV,	31.5*F_DIV,		31.5*F_DIV},			/*	STA   	速度	[LOW, LOW, LOW]	*/
	{200*F_DIV,		500*F_DIV,		700*F_DIV},				/*	ASP		空载1		空载2	*/
	};

//	串口屏亮度和音量列表
const uint8_t system_light[MAX_LIGHT_INDEX+1] = {10, 20, 30, 40, 52, 63};
const uint8_t system_volume[MAX_VOLUME_INDEX+1]= {0, 30, 40, 48, 56, 63};

/**************************以下关于数据库配置************************/
/*	1. Aspirate回吸功能	*/
bool Open_Asp(void)
{
	sys_para_t.ASPIRATE = 1;
	if(Is_Train_Mode_Open() == true)
		Play_Now_Music(ASPIRATE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Mode].obj_value[0] = sys_para_t.ASPIRATE;
	return true;
}

bool Close_Asp(void)
{
	sys_para_t.ASPIRATE = 0;
	if(Is_Train_Mode_Open() == true)
		Play_Now_Music(ASPIRATE_OFF_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Mode].obj_value[0] = sys_para_t.ASPIRATE;
	return false;
}
bool Is_Asp_Open(void)
{
	vgus_page_objs[PAGE_Mode].obj_value[0] = sys_para_t.ASPIRATE;
	if(sys_para_t.ASPIRATE == 1)
		return true;
	else
		return false;
}

/*	2. smart_reminder智能提醒	*/
bool Open_Smart_Reminder(void)
{
	sys_para_t.SMART_REMINDER =1;
	Play_Now_Music(SMART_REMINDER_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[0] = sys_para_t.SMART_REMINDER;
	return true;
}

bool Close_Smart_Reminder(void)
{
	sys_para_t.SMART_REMINDER = 0;
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[0] = sys_para_t.SMART_REMINDER;
	return false;
}

bool Is_Smart_Reminder_Open(void)
{
	vgus_page_objs[PAGE_Setting].obj_value[0] = sys_para_t.SMART_REMINDER;
	if(sys_para_t.SMART_REMINDER == 1)
		return true;
	else
		return false;
}

/*	3. train mode训练模式 */
bool Open_Train_Mode(void)
{
	sys_para_t.TRAIN_MODE =1;
	Play_Now_Music(TRAIN_MODE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[1] = sys_para_t.TRAIN_MODE;
	return true;
}

bool Close_Train_Mode(void)
{
	sys_para_t.TRAIN_MODE = 0;
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[1] = sys_para_t.TRAIN_MODE;
	return false;
}

bool Is_Train_Mode_Open(void)
{
	vgus_page_objs[PAGE_Setting].obj_value[1] = sys_para_t.TRAIN_MODE;
	if(sys_para_t.TRAIN_MODE == 1)
		return true;
	else
		return false;
}

/*	4. 音量:0x00--0x40	*/
//	volume取值范围：0-5
bool Set_System_Vol(uint8_t volume)
{
	if(volume > MAX_VOLUME_INDEX)
		volume = MAX_VOLUME_INDEX;
	sys_para_t.VOLUME = volume;
	Play_Now_Music(0, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[3] = sys_para_t.VOLUME;		//	保存音量索引
	return true;
}

uint8_t Get_System_Vol(void)	/*	返回音量索引	*/
{
	vgus_page_objs[PAGE_Setting].obj_value[3] = sys_para_t.VOLUME;		//	保存音量索引
	return (sys_para_t.VOLUME&0x000000FF);
}

/*	5. 亮度:0x00--0x40  [0, 15, 30, 40, 52, 64]	*/
//	light取值范围：0-5
bool Set_System_Light(uint8_t light)
{
	if(light > MAX_LIGHT_INDEX)
		light = MAX_LIGHT_INDEX;
	sys_para_t.LIGHT = light;
	Set_VGUS_Brightness(system_light[sys_para_t.LIGHT]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[2] = sys_para_t.LIGHT;		//	保存亮度索引
	return true;
}
uint8_t Get_System_Light(void)
{
	if(sys_para_t.LIGHT > MAX_LIGHT_INDEX)
		sys_para_t.LIGHT = MAX_LIGHT_INDEX;
	vgus_page_objs[PAGE_Setting].obj_value[2] = sys_para_t.LIGHT;		//	保存亮度索引
	return sys_para_t.LIGHT;
}

/*	6. 用户使用次数	*/
void Set_Use_Times(void)
{
	sys_para_t.USE_TIMES ++;
}
uint32_t Read_Use_Times(void)
{
	return sys_para_t.USE_TIMES;
}

/*	7. 开机自动回退	*/
bool Open_Autoback(void)
{
	sys_para_t.AUTO_BACK = 1;
	return true;
}
bool Close_AutoBack(void)
{
	sys_para_t.AUTO_BACK = 0x10;
	return true;
}
bool Is_Autoback_Off(void)
{
	if(sys_para_t.AUTO_BACK == 0x10)
		return true;
	else
		return false;
}

/*	8. 充电时使用	*/
bool Open_UseInCharge(void)
{
	sys_para_t.USE_In_CHARGING = 0x10;
	return true;
}
bool Close_UseInCharge(void)
{
	sys_para_t.USE_In_CHARGING = 1;
	return true;
}
bool Is_UseInCharge(void)
{
	if(sys_para_t.USE_In_CHARGING == 0x10)
		return true;
	else
		return false;
}

/*	9. 系统更新	*/
bool Start_SysUpdate(void)
{
	uint8_t databuf = 1;
	taskENTER_CRITICAL();
	EEPROM_Write(0x2F, &databuf, 1);
	taskEXIT_CRITICAL();
}

bool Stop_SysUpdate(void)
{
	uint8_t databuf = 0;
	taskENTER_CRITICAL();
	EEPROM_Write(0x2F, &databuf, 1);
	taskEXIT_CRITICAL();
}

/* --------------------------------------------	以下提供数据读写接口	-------------------------------------------------	*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-8-16
* 函 数 名: TMY_DB_Init
* 功能说明: 推麻仪单片机内部Flash初始化程序更换为外部EEPROM存储
*
* 形    参: 
*
* 返 回 值:
*			-1： 初始化失败
*			0 ： 初始化成功
*********************************************************************************************************
*/
int8_t TMY_DB_Init(void)
{
	int8_t read_result = -1;

	read_result = Read_Sys_Para_Data();			/*	读取系统参数		*/
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data sys_para_t failed = %d!\r\n", read_result);
	else{
//		USRDATA_PRINTF("ASPIRATE= %d\r\n SMART_REMINDER= %d\r\n TRAIN_MODE= %d\r\n VOLUME= %d\r\n LIGHT= %d\r\n AGING_CYCLE= %d\r\n USE_TIMES=%d\r\n AUTO_BACK= %d\r\n",
//		sys_para_t.ASPIRATE, sys_para_t.SMART_REMINDER, sys_para_t.TRAIN_MODE, sys_para_t.VOLUME, sys_para_t.LIGHT, sys_para_t.AGING_CYCLE, sys_para_t.USE_TIMES, sys_para_t.AUTO_BACK);
	}
	
	read_result = Read_Handle_Injector_Data();		/*	手柄注射器数据		*/
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data handle_injector_t failed = %d!\r\n", read_result);
	else{
//		USRDATA_PRINTF("ORIGIN_LOC= 0x%08x\r\n TOP_LOC= 0x%08x\r\n DISTANCE= %d\r\n LAST_LOC= %d\r\n",
//		handle_injector_t.ORIGIN_LOC, handle_injector_t.TOP_LOC, (handle_injector_t.DISTANCE>>16), (handle_injector_t.LAST_LOC>>16));
	}
	
	read_result = Read_Foot_Pedal_Data();			/*	脚踏数据		*/;
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data foot_pedal_t failed!\r\n");
	else{
//		USRDATA_PRINTF("foot_pedal_t.MAX= %d\r\n MIN= %d\r\n QUARTER_DELTA= %d\r\n", foot_pedal_t.MAX,foot_pedal_t.MIN,foot_pedal_t.QUARTER_DELTA);
	}
	
	return 0;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: Read_Handle_Injector_Data
* 功能说明: 从数据库读 手柄注射器handle_injector信息
*	handle_injector:
*		ORIGIN_LOC 电机位置，
*		TOP_LOC 电机位置，
*		DISTANCE 电机可运行圈数
*		LAST_LOC 上次电机停转时所在位置
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t Read_Handle_Injector_Data(void)
{
	//	return tmy_read_user_data("handle_injector_t", &handle_injector_t, sizeof(handle_injector_t));
	static uint8_t databuf[20] = {0};
	REV_U32_T rev_u32_t;
	
	taskENTER_CRITICAL();
	EEPROM_Read(0x60, databuf,20);
	taskEXIT_CRITICAL();
	
	rev_u32_t.uc_rev_data[0] = databuf[0];
	rev_u32_t.uc_rev_data[1] = databuf[1];
	rev_u32_t.uc_rev_data[2] = databuf[2];
	rev_u32_t.uc_rev_data[3] = databuf[3];
	handle_injector_t.ORIGIN_LOC = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[4];
	rev_u32_t.uc_rev_data[1] = databuf[5];
	rev_u32_t.uc_rev_data[2] = databuf[6];
	rev_u32_t.uc_rev_data[3] = databuf[7];
	handle_injector_t.TOP_LOC = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[8];
	rev_u32_t.uc_rev_data[1] = databuf[9];
	rev_u32_t.uc_rev_data[2] = databuf[10];
	rev_u32_t.uc_rev_data[3] = databuf[11];
	handle_injector_t.DISTANCE = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[12];
	rev_u32_t.uc_rev_data[1] = databuf[13];
	rev_u32_t.uc_rev_data[2] = databuf[14];
	rev_u32_t.uc_rev_data[3] = databuf[15];
	handle_injector_t.LAST_LOC = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[16];
	rev_u32_t.uc_rev_data[1] = databuf[17];
	rev_u32_t.uc_rev_data[2] = databuf[18];
	rev_u32_t.uc_rev_data[3] = databuf[19];
	handle_injector_t.BOTTM2ORIGIN = rev_u32_t.u32_rev_data;
	if(handle_injector_t.BOTTM2ORIGIN > (MAX_BOTTOM_ORIGIN<<16))	
		handle_injector_t.BOTTM2ORIGIN = (handle_injector_t.BOTTM2ORIGIN & 0x0000FFFF) + (200<<16);
	
	return 0;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: Write_Handle_Injector_Data
* 功能说明: 往数据库写 手柄注射器handle_injector信息
*	handle_injector:
*		ORIGIN_LOC 电机位置，
*		TOP_LOC 电机位置，
*		DISTANCE 电机可运行圈数
*		LAST_LOC 上次电机停转时所在位置
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t Write_Handle_Injector_Data(void)
{
//	return tmy_write_user_data("handle_injector_t", &handle_injector_t, sizeof(handle_injector_t));
	static uint8_t databuf[20] = {0};
	REV_U32_T rev_u32_t;
	
	rev_u32_t.u32_rev_data = handle_injector_t.ORIGIN_LOC;
	databuf[0] = rev_u32_t.uc_rev_data[0];
	databuf[1] = rev_u32_t.uc_rev_data[1];
	databuf[2] = rev_u32_t.uc_rev_data[2];
	databuf[3] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.TOP_LOC;
	databuf[4] = rev_u32_t.uc_rev_data[0];
	databuf[5] = rev_u32_t.uc_rev_data[1];
	databuf[6] = rev_u32_t.uc_rev_data[2];
	databuf[7] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.DISTANCE;
	databuf[8] = rev_u32_t.uc_rev_data[0];
	databuf[9] = rev_u32_t.uc_rev_data[1];
	databuf[10] = rev_u32_t.uc_rev_data[2];
	databuf[11] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.LAST_LOC;
	databuf[12] = rev_u32_t.uc_rev_data[0];
	databuf[13] = rev_u32_t.uc_rev_data[1];
	databuf[14] = rev_u32_t.uc_rev_data[2];
	databuf[15] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.BOTTM2ORIGIN;
	databuf[16] = rev_u32_t.uc_rev_data[0];
	databuf[17] = rev_u32_t.uc_rev_data[1];
	databuf[18] = rev_u32_t.uc_rev_data[2];
	databuf[19] = rev_u32_t.uc_rev_data[3];
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x60, databuf, 20);
	taskEXIT_CRITICAL();
	
	return 0;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: Read_Foot_Pedal_Data
* 功能说明: 从数据库读 脚踏foot_pedal信息
*	handle_injector:
*		静息时电压值MIN，
*		踩至最底部电压值MAX，
*		变化量4等分值QUARTER_DELTA
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t Read_Foot_Pedal_Data(void)
{
//	return tmy_read_user_data("foot_pedal_t", &foot_pedal_t, sizeof(foot_pedal_t));
	static uint8_t databuf[12] = {0};
	REV_U32_T rev_u32_t;
	taskENTER_CRITICAL();
	EEPROM_Read(0x40, databuf,12);
	taskEXIT_CRITICAL();
	
	rev_u32_t.uc_rev_data[0] = databuf[0];
	rev_u32_t.uc_rev_data[1] = databuf[1];
	rev_u32_t.uc_rev_data[2] = databuf[2];
	rev_u32_t.uc_rev_data[3] = databuf[3];
	foot_pedal_t.MIN = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[4];
	rev_u32_t.uc_rev_data[1] = databuf[5];
	rev_u32_t.uc_rev_data[2] = databuf[6];
	rev_u32_t.uc_rev_data[3] = databuf[7];
	foot_pedal_t.MAX = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[8];
	rev_u32_t.uc_rev_data[1] = databuf[9];
	rev_u32_t.uc_rev_data[2] = databuf[10];
	rev_u32_t.uc_rev_data[3] = databuf[11];
	foot_pedal_t.QUARTER_DELTA = rev_u32_t.u32_rev_data;
	
	return 0;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: Write_Foot_Pedal_Data
* 功能说明: 往数据库写 脚踏foot_pedal信息：
*	handle_injector:
*		静息时电压值MIN，
*		踩至最底部电压值MAX，
*		变化量4等分值QUARTER_DELTA
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t Write_Foot_Pedal_Data(void)
{
//	return tmy_write_user_data("foot_pedal_t", &foot_pedal_t, sizeof(foot_pedal_t));
	static uint8_t databuf[12] = {0};
	REV_U32_T rev_u32_t;
	
	rev_u32_t.u32_rev_data = foot_pedal_t.MIN;
	databuf[0] = rev_u32_t.uc_rev_data[0];
	databuf[1] = rev_u32_t.uc_rev_data[1];
	databuf[2] = rev_u32_t.uc_rev_data[2];
	databuf[3] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = foot_pedal_t.MAX;
	databuf[4] = rev_u32_t.uc_rev_data[0];
	databuf[5] = rev_u32_t.uc_rev_data[1];
	databuf[6] = rev_u32_t.uc_rev_data[2];
	databuf[7] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = foot_pedal_t.QUARTER_DELTA;
	databuf[8] = rev_u32_t.uc_rev_data[0];
	databuf[9] = rev_u32_t.uc_rev_data[1];
	databuf[10] = rev_u32_t.uc_rev_data[2];
	databuf[11] = rev_u32_t.uc_rev_data[3];
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x40, databuf, 12);
	taskEXIT_CRITICAL();
	
	return 0;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: Read_Sys_Para_Data
* 功能说明: 从数据库读 sys_para_t信息
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t Read_Sys_Para_Data(void)
{
//	return tmy_read_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
	static uint8_t databuf[16] = {0};
	REV_U32_T rev_u32_t;
	
	taskENTER_CRITICAL();
	EEPROM_Read(0x20, databuf,16);
	taskEXIT_CRITICAL();
	
	sys_para_t.ASPIRATE = databuf[0];
	sys_para_t.SMART_REMINDER = databuf[1];
	sys_para_t.TRAIN_MODE = databuf[2];
	sys_para_t.VOLUME = databuf[3];
	sys_para_t.LIGHT = databuf[4];
	
	rev_u32_t.uc_rev_data[0] = databuf[5];
	rev_u32_t.uc_rev_data[1] = databuf[6];
	rev_u32_t.uc_rev_data[2] = databuf[7];
	rev_u32_t.uc_rev_data[3] = databuf[8];
	sys_para_t.AGING_CYCLE = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[9];
	rev_u32_t.uc_rev_data[1] = databuf[10];
	rev_u32_t.uc_rev_data[2] = databuf[11];
	rev_u32_t.uc_rev_data[3] = databuf[12];
	sys_para_t.USE_TIMES = rev_u32_t.u32_rev_data;
	
	sys_para_t.AUTO_BACK = databuf[13];
	sys_para_t.USE_In_CHARGING = databuf[14];
	sys_para_t.UPDATE_FLAG = databuf[15];
	
	return true;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: Read_Sys_Para_Data
* 功能说明: 往数据库写入 sys_para_t 信息
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t Write_Sys_Para_Data(void)
{
//	return tmy_write_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
	static uint8_t databuf[16] = {0};
	REV_U32_T rev_u32_t;
	
	databuf[0] = sys_para_t.ASPIRATE;
	databuf[1] = sys_para_t.SMART_REMINDER;
	databuf[2] = sys_para_t.TRAIN_MODE;
	databuf[3] = sys_para_t.VOLUME;
	databuf[4] = sys_para_t.LIGHT;
	
	rev_u32_t.u32_rev_data = sys_para_t.AGING_CYCLE;
	databuf[5] = rev_u32_t.uc_rev_data[0];
	databuf[6] = rev_u32_t.uc_rev_data[1];
	databuf[7] = rev_u32_t.uc_rev_data[2];
	databuf[8] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = sys_para_t.USE_TIMES;
	databuf[9] = rev_u32_t.uc_rev_data[0];
	databuf[10] = rev_u32_t.uc_rev_data[1];
	databuf[11] = rev_u32_t.uc_rev_data[2];
	databuf[12] = rev_u32_t.uc_rev_data[3];
	
	databuf[13] = sys_para_t.AUTO_BACK;
	databuf[14] = sys_para_t.USE_In_CHARGING;
	databuf[15] = sys_para_t.UPDATE_FLAG;
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x20, databuf, 16);
	taskEXIT_CRITICAL();
	
	sys_para_update_flag = false;
	return 0;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-01-21
* 函 数 名: Get_Current_Handle_Injector_Pos
* 功能说明: 获取当前丝杆所在位置
*
* 形    参: 
*
* 返 回 值:
*			当前丝杆所处位置
*********************************************************************************************************
*/
uint32_t Get_Current_Handle_Injector_Pos(void)
{
	uint32_t loc = (uint32_t)(motor_cycle << 16) + motor_angle;
	return (loc + handle_injector_t.LAST_LOC);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-07-13
* 函 数 名: Get_Current_Handle_Run_Distance
* 功能说明: 获取当前丝杆已运行的行程，单位：圈数
*
* 形    参: 
*
* 返 回 值:
*			当前丝杆已走圈数，单位：圈数
*********************************************************************************************************
*/
uint16_t Get_Current_Handle_Run_Distance(uint16_t current_pos)
{
	uint16_t ret = 0;
	uint16_t currnt_loc_cycle = current_pos;
	uint16_t bottom_loc_cycle = (handle_injector_t.ORIGIN_LOC >> 16);
	uint16_t distance_cycle = (handle_injector_t.DISTANCE >> 16);
	
	if(currnt_loc_cycle >= bottom_loc_cycle){
		ret = currnt_loc_cycle - bottom_loc_cycle;
		if(ret > distance_cycle)
			return distance_cycle;
		else
			return ret;
	}else{
		ret = 0xFFFF - bottom_loc_cycle + 1 + currnt_loc_cycle;
		if(ret > distance_cycle)
			return 0;
		else
			return ret;
	}	
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-03-15
* 函 数 名: Compute_Reamin_Dose
* 功能说明: 计算推麻仪剩余药量
*
* 形    参: 
*
* 返 回 值:
*			表示剩余药量的百分比：10-->10%, 20-->20%,...,100-->100%
*********************************************************************************************************
*/
uint8_t Compute_Reamin_Dose(uint16_t current_pos)
{
	uint16_t all_dis = (handle_injector_t.DISTANCE >> 16);
	uint16_t now_dis = Get_Current_Handle_Run_Distance(current_pos);
	uint16_t remain_dose = (all_dis - now_dis)*100 / all_dis;
	return remain_dose;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-01-21
* 函 数 名: save_bf_pwroff
* 功能说明: 关机前保存所有信息
*
* 形    参: 
*
* 返 回 值:
*			-1： 初始化失败
*			0 ： 初始化成功
*********************************************************************************************************
*/
static int8_t save_bf_pwroff(void)
{
	int8_t result = 0;
	handle_injector_t.LAST_LOC = Get_Current_Handle_Injector_Pos();			/*	校正时清零，关机时保存，开机是更新*/
	result = Write_Handle_Injector_Data();	
	if(!result)
		USRDATA_PRINTF("handle_injector_t.LAST_LOC = %d\r\n",(handle_injector_t.LAST_LOC>>16));
	
	result = Write_Sys_Para_Data();
	if(!result)
		USRDATA_PRINTF("sys_para_t.AGING_CYCLE = %d\r\n",sys_para_t.AGING_CYCLE);
	
	return result;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.2
* 日    期：2022-08-03
* 函 数 名: PWR_On_Off_Detect
* 功能说明: 开关机检测
*
* 形    参: 
*
* 返 回 值:
*			
*********************************************************************************************************
*/
void PWR_On_Off_Detect2(void)
{
	static bool already_pwr_on_flag = false;
	static bool disable_vgus_touch_false = false;
	static uint8_t time_cnt = 0;
	
	
	//	0,合适的适配器接入；1，适配器未接入或者适配器已坏；2，适配器电压偏低；3，适配器电压偏高
	CHARGE_STATE = Is_Charger_Adapter_Right();		
	/*	1. 手动上电	*/
	if(already_pwr_on_flag == false){				//	A. 关机状态-->上电开机状态
		PWR_ON();
		already_pwr_on_flag = true;
		osDelay(2000);
		if(isPWR_KEY_INVALID() == 1){	//	等待用户松手
			while(1){					
				osDelay(200);
				if(isPWR_KEY_INVALID()==0)
					break;
			}
		}
	}else{											//	B. 运行状态下
		if((isPWR_KEY_INVALID() == 1)&&(CHARGE_STATE != RIGHT_CHARGE)){
			osDelay(2000);
			if(isPWR_KEY_INVALID() == 1){	//	2s之后开关机按键仍然被按下，软件使能关机
				PWR_Off_Save();
				Clear_Lcd_Cmd_SendQueue();
				Set_VGUS_Page(PAGE_PowerOff, true);
				osDelay(3000);
				Close_VGUS_BACKLIGHT();
				while(1){				//	等待用户松手
					if(isPWR_KEY_INVALID()==0)
						PWR_OFF();
					osDelay(300);
				}
			}	
		}
	}
	
	if(++time_cnt > 35){	//	刚开始上电时不进行充电和电池监测
		/*	2. 充电使用功能是否开启	*/
		if(CHARGE_STATE != NO_CHARGE){
			if(Is_UseInCharge() == false){
				Disable_VGUS_Touch();
				disable_vgus_touch_false = true;
			}
		}else if(disable_vgus_touch_false == true){
			Clear_Lcd_Cmd_SendQueue();
			Enable_VGUS_Touch();
			disable_vgus_touch_false = false;
		}
			
		/*	3. 电池电压监测	*/
		Display_Battery();
		
		time_cnt = 35;
	}

	/*	4. 系统空闲监测	*/
	IDLE_TIME_CNT ++;
	if(IDLE_TIME_CNT > MAX_IDLE_TIME_CNT){
		PWR_Off_Save();
		Clear_Lcd_Cmd_SendQueue();
		Set_VGUS_Page(PAGE_PowerOff, true);
		osDelay(3000);
		Close_VGUS_BACKLIGHT();
		osDelay(300);
		PWR_OFF();
	}
		
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-16
* 函 数 名: get_Calibration_Oil_index
* 功能说明: 从eeprom中加载校正好的液压系数 Oil_K, Oil_b
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/

float Oil_K = 71.32f;
float Oil_b	= 3.602f;
uint8_t Get_Fitting_Oil_Index(void)
{
	static REV_FLOAT_T Oil_K_data;
	static REV_FLOAT_T Oil_b_data;
	static uint8_t temp[4] = {0};
	float temp_kPa = 0;
	uint8_t ret = 0;
	taskENTER_CRITICAL();
	ret = EEPROM_Read(0x01, temp, 4);		/*	读取Oil_K	*/
	taskEXIT_CRITICAL();
	Oil_K_data.uc_rev_data[0] = temp[0];
	Oil_K_data.uc_rev_data[1] = temp[1];
	Oil_K_data.uc_rev_data[2] = temp[2];
	Oil_K_data.uc_rev_data[3] = temp[3];
	
	taskENTER_CRITICAL();
	ret = EEPROM_Read(0x05, temp, 4);		/*	读取Oil_b	*/
	taskEXIT_CRITICAL();
	Oil_b_data.uc_rev_data[0] = temp[0];
	Oil_b_data.uc_rev_data[1] = temp[1];
	Oil_b_data.uc_rev_data[2] = temp[2];
	Oil_b_data.uc_rev_data[3] = temp[3];
	
	/*	这里是做一个验证，防止拟合系数与实际偏差较大	*/
	temp_kPa = Oil_K_data.f_rev_data*1.62f + Oil_b_data.f_rev_data;
	if( (temp_kPa <120) && (temp_kPa >80) ){
		Oil_K = Oil_K_data.f_rev_data;
		Oil_b = Oil_b_data.f_rev_data;
		ret = 1;
	}else{
		Oil_K = 71.32f;
		Oil_b = 3.602f;
		ret = 0;
	}
	USRDATA_PRINTF("System read oil calibration data over, Oil_K= %3.2f, Oil_b= %3.2f!\r\n", Oil_K, Oil_b);
	return ret;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-14
* 函 数 名: Get_Current_g_From_kPa
* 功能说明: 根据读取的液压转化为实际的推力
* 形    参: 
      返回值单位： 克g
* 返 回 值: 
*********************************************************************************************************
*/
float Power_K = 14.77;
float Power_b = -3596;
int32_t Get_Current_g_From_kPa(float kPa)
{
	static float ret = 0.0f;
	ret = Power_K * kPa + Power_b;
	
	return (int32_t)ret;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-08-11
* 函 数 名: Cycle_Reset_To_Origin
* 功能说明: 复位电机至液压原点
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void Cycle_Reset_To_Origin(void)
{
	uint16_t distance_cycle = (handle_injector_t.DISTANCE >> 16);
	handle_injector_t.LAST_LOC = 0;
	if((distance_cycle < 1000)||(distance_cycle > 1300))
		handle_injector_t.DISTANCE = (MAX_DIS_CYCLES << 16);
	handle_injector_t.ORIGIN_LOC = (0x0000FFFF&(handle_injector_t.ORIGIN_LOC)) + (CYCLE_BIAS << 16);
	handle_injector_t.TOP_LOC = handle_injector_t.DISTANCE + handle_injector_t.ORIGIN_LOC;
	reset_cycle_to_bias();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-08-16
* 函 数 名: PWR_Off_Save
* 功能说明: 关机
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void PWR_Off_Save(void)
{
	static int8_t i8_result = 0;
	set_motor_stop();
	Set_Use_Times();
	USRDATA_PRINTF("\r\nUser has use %d times\r\n", Read_Use_Times());
	i8_result = save_bf_pwroff();	//	F401执行该指令无法正常关机
	if(!i8_result)	USRDATA_PRINTF("\r\nSave data success!\r\n");
	else			USRDATA_PRINTF("\r\nSave data failed!\r\n");
}
