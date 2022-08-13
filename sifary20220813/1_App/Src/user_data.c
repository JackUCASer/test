#include "user_data.h"
#include <flashdb.h>		/*	数据库依赖	*/
#include "lcd_display.h"
#include "motor_misc.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"


//	系统参数: 需写入数据库
SYS_PARA_T sys_para_t = {0};

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
bool handle_in_top = false;
bool handle_in_bottom = false;
bool handle_in_origin = false;
bool sys_para_update_flag = false;


//	系统中的电机转速：不牵涉校正的问题，所以不用写入数据库
/* 	LOW,	MID,	HIGH	*/
const uint32_t injector_speed_t[4][3] = {
	{50*F_DIV,	50*F_DIV,	350*F_DIV},				/*	Normal	速度	[LOW, LOW, MID]	*/
	{50*F_DIV,	350*F_DIV,	700*F_DIV},				/*	Turbo 	速度	[LOW, MID, HIGH]*/
	{50*F_DIV,	50*F_DIV,	50*F_DIV},				/*	STA   	速度	[LOW, LOW, LOW]	*/
	{100*F_DIV,	500*F_DIV,	700*F_DIV},				/*	ASP		空载1		空载2	*/
	};

//	串口屏亮度和音量列表
const uint8_t system_light[MAX_LIGHT_INDEX+1] = {10, 20, 30, 40, 52, 64};
const uint8_t system_volume[MAX_VOLUME_INDEX+1]= {0, 30, 40, 48, 56, 64};

/**************************以下关于数据库配置************************/
#define FDB_LOG_TAG "[main]"

static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};

/* default KV nodes */
/*	初始化数据库时，会根据该内容进行核对，如果已存在该信息，则该扇区不需要再初始化*/
static struct fdb_default_kv_node default_kv_table[] = {	
        {"username", "E-Flow", 0}, 	/* string KV */
        {"password", "Sifary", 0}, 	/* string KV */
        {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
        {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};

/* KVDB object */
static struct fdb_kvdb kvdb = { 0 };

/*	1. Aspirate回吸功能	*/
bool open_asp(void)
{
	sys_para_t.ASPIRATE = 1;
	if((is_train_mode_open() == true)||(is_smart_reminder_open() == true))
		play_now_music(ASPIRATE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

bool close_asp(void)
{
	sys_para_t.ASPIRATE = 0;
	if((is_train_mode_open() == true)||(is_smart_reminder_open() == true))
		play_now_music(ASPIRATE_OFF_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return false;
}
bool is_asp_open(void)
{
	if(sys_para_t.ASPIRATE == 1)
		return true;
	else
		return false;
}

/*	2. smart_reminder智能提醒	*/
bool open_smart_reminder(void)
{
	sys_para_t.SMART_REMINDER =1;
	play_now_music(SMART_REMINDER_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

bool close_smart_reminder(void)
{
	sys_para_t.SMART_REMINDER = 0;
	sys_para_update_flag = true;
	return false;
}

bool is_smart_reminder_open(void)
{
	if(sys_para_t.SMART_REMINDER == 1)
		return true;
	else
		return false;
}

/*	3. train mode训练模式 */
bool open_train_mode(void)
{
	sys_para_t.TRAIN_MODE =1;
	play_now_music(TRAIN_MODE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

bool close_train_mode(void)
{
	sys_para_t.TRAIN_MODE = 0;
	sys_para_update_flag = true;
	return false;
}

bool is_train_mode_open(void)
{
	if(sys_para_t.TRAIN_MODE == 1)
		return true;
	else
		return false;
}

/*	4. 音量:0x00--0x40	*/
//	volume取值范围：0-5
bool set_system_vol(uint8_t volume)
{
	if(volume > MAX_VOLUME_INDEX)
		volume = MAX_VOLUME_INDEX;
	sys_para_t.VOLUME = volume;
	play_now_music(0, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

uint8_t get_system_vol(void)	/*	返回音量索引	*/
{
	return (sys_para_t.VOLUME&0x000000FF);
}

/*	5. 亮度:0x00--0x40  [0, 15, 30, 40, 52, 64]	*/
//	light取值范围：0-5
bool set_system_light(uint8_t light)
{
	if(light > MAX_LIGHT_INDEX)
		light = MAX_LIGHT_INDEX;
	sys_para_t.LIGHT = light;
	set_now_brightness(system_light[sys_para_t.LIGHT]);
	sys_para_update_flag = true;
	return true;
}
uint8_t get_system_light(void)
{
	return sys_para_t.LIGHT;
}

/*	6. 用户使用次数	*/
void set_use_times(void)
{
	sys_para_t.USE_TIMES ++;
}
uint32_t read_use_times(void)
{
	return sys_para_t.USE_TIMES;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: tmy_read_user_data
* 功能说明: 从数据库中读取用户
*
* 形    参: 
*			key：		数据名称
*			value_buf：	数据地址
*			buf_len：	数据大小
* 返 回 值:
*			-1： 读取数据失败
*			0 ： 读取数据成功
*********************************************************************************************************
*/
static int8_t tmy_read_user_data(const char* key, const void* value_buf, size_t buf_len)
{
	size_t read_len;
	struct fdb_blob blob;
	read_len = fdb_kv_get_blob(&kvdb, key, fdb_blob_make(&blob, value_buf, buf_len));	/*	返回读取的字节长度	*/
	if (read_len != buf_len) {	
		return -1;
	}
	return 0;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: tmy_write_user_data
* 功能说明: 写用户数据到数据库中
*
* 形    参: 
*			key：		数据名称
*			value_buf：	数据地址
*			buf_len：	数据大小
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
static int8_t tmy_write_user_data(const char* key, const void* value_buf, size_t buf_len)
{
	fdb_err_t result;
	struct fdb_blob blob;
	result = fdb_kv_set_blob(&kvdb, key, fdb_blob_make(&blob, value_buf, buf_len));
	if (result != FDB_NO_ERR) {
		return -1;
	}
	return 0;
}

/* --------------------------------------------	以下提供数据读写接口	-------------------------------------------------	*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: tmy_MCUflash_database_init
* 功能说明: 推麻仪单片机内部Flash初始化程序
*
* 形    参: 
*
* 返 回 值:
*			-1： 初始化失败
*			0 ： 初始化成功
*********************************************************************************************************
*/
int8_t tmy_database_init(void)
{
	fdb_err_t result;
	int8_t read_result = -1;
	/* KVDB Sample */
	struct fdb_default_kv default_kv;

	default_kv.kvs = default_kv_table;
	default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
	/* set the lock and unlock function if you want */
	fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, NULL);
	fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, NULL);
	/* Key-Value database initialization
	 *
	 *       &kvdb: database object
	 *       "env": database name
	 * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
	 *              Please change to YOUR partition name.
	 * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
	 *        NULL: The user data if you need, now is empty.
	 */
	result = fdb_kvdb_init(&kvdb, "env", "fdb_kvdb1", &default_kv, NULL);

	if (result != FDB_NO_ERR) {
		return -1;
	}
	
	read_result = read_sys_para_t_data();			/*	读取系统参数		*/
	if(read_result)
		FDB_PRINT("tmy_read_user_data sys_para_t failed = %d!\r\n", read_result);
//	else{
//		FDB_PRINT("sys_para_t.VOLUME == %d \r\n",sys_para_t.VOLUME);
//		FDB_PRINT("sys_para_t.LIGHT == %d \r\n",sys_para_t.LIGHT);
//		FDB_PRINT("sys_para_t.ASPIRATE == %d \r\n",sys_para_t.ASPIRATE);
//		FDB_PRINT("sys_para_t.AGING_CYCLE == %d \r\n",sys_para_t.AGING_CYCLE);
//		FDB_PRINT("sys_para_t.USE_TIMES == %d \r\n",sys_para_t.USE_TIMES);
//	}
	
	read_result = read_handle_injector_data();		/*	手柄注射器数据		*/
	if(read_result)
		FDB_PRINT("tmy_read_user_data handle_injector_t failed = %d!\r\n", read_result);
//	else{
//		FDB_PRINT("handle_injector_t.BOTTOM_LOC    = 0x%08x\r\n",handle_injector_t.BOTTOM_LOC);
//		FDB_PRINT("handle_injector_t.TOP_LOC       = 0x%08x\r\n",handle_injector_t.TOP_LOC);
//		FDB_PRINT("handle_injector_t.DISTANCE      = %d\r\n",(handle_injector_t.DISTANCE>>16));
//		FDB_PRINT("hhandle_injector_t.LAST_LOC     = %d\r\n",(handle_injector_t.LAST_LOC>>16));
//	}
	
	read_result = read_foot_pedal_t_data();			/*	脚踏数据		*/;
	if(read_result)
		FDB_PRINT("tmy_read_user_data foot_pedal_t failed!\r\n");
//	else{
//		FDB_PRINT("foot_pedal_t.MAX           = %d\r\n", foot_pedal_t.MAX);
//		FDB_PRINT("foot_pedal_t.MIN           = %d\r\n", foot_pedal_t.MIN);
//		FDB_PRINT("foot_pedal_t.QUARTER_DELTA = %d\r\n", foot_pedal_t.QUARTER_DELTA);
//	}
	
	return 0;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: read_handle_injector_data
* 功能说明: 从数据库读 手柄注射器handle_injector信息
*	handle_injector:
*		BOTTOM_LOC 电机位置，
*		TOP_LOC 电机位置，
*		DISTANCE 电机可运行圈数
*		LAST_LOC 上次电机停转时所在位置
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t read_handle_injector_data(void)
{
	return tmy_read_user_data("handle_injector_t", &handle_injector_t, sizeof(handle_injector_t));
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: write_handle_injector_data
* 功能说明: 往数据库写 手柄注射器handle_injector信息
*	handle_injector:
*		BOTTOM_LOC 电机位置，
*		TOP_LOC 电机位置，
*		DISTANCE 电机可运行圈数
*		LAST_LOC 上次电机停转时所在位置
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t write_handle_injector_data(void)
{
	return tmy_write_user_data("handle_injector_t", &handle_injector_t, sizeof(handle_injector_t));
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: read_foot_pedal_t_data
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
int8_t read_foot_pedal_t_data(void)
{
	return tmy_read_user_data("foot_pedal_t", &foot_pedal_t, sizeof(foot_pedal_t));
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: write_foot_pedal_t_data
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
int8_t write_foot_pedal_t_data(void)
{
	return tmy_write_user_data("foot_pedal_t", &foot_pedal_t, sizeof(foot_pedal_t));
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: read_sys_para_t_data
* 功能说明: 从数据库读 sys_para_t信息
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t read_sys_para_t_data(void)
{
	return tmy_read_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-12-27
* 函 数 名: read_sys_para_t_data
* 功能说明: 往数据库写入 sys_para_t 信息
* 返 回 值:
*			-1： 保存数据失败
*			0 ： 保存数据成功
*********************************************************************************************************
*/
int8_t write_sys_para_t_data(void)
{
	sys_para_update_flag = false;
	return tmy_write_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-01-21
* 函 数 名: get_current_handle_injector
* 功能说明: 获取当前丝杆所在位置
*
* 形    参: 
*
* 返 回 值:
*			当前丝杆所处位置
*********************************************************************************************************
*/
extern uint16_t motor_cycle;
extern uint16_t motor_angle;
uint32_t get_current_handle_injector(void)
{
	uint32_t loc = (uint32_t)(motor_cycle << 16) + motor_angle;
	return (loc + handle_injector_t.LAST_LOC);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-07-13
* 函 数 名: get_current_handle_run_distance
* 功能说明: 获取当前丝杆已运行的行程，单位：圈数
*
* 形    参: 
*
* 返 回 值:
*			当前丝杆已走圈数，单位：圈数
*********************************************************************************************************
*/
uint16_t get_current_handle_run_distance(uint16_t current_pos)
{
	uint16_t ret = 0;
	uint16_t currnt_loc_cycle = current_pos;
	uint16_t bottom_loc_cycle = (handle_injector_t.BOTTOM_LOC >> 16);
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
* 函 数 名: compute_remain_dose
* 功能说明: 计算推麻仪剩余药量
*
* 形    参: 
*
* 返 回 值:
*			表示剩余药量的百分比：10-->10%, 20-->20%,...,100-->100%
*********************************************************************************************************
*/
uint8_t compute_remain_dose(uint16_t current_pos)
{
	uint16_t all_dis = (handle_injector_t.DISTANCE >> 16);
	uint16_t now_dis = get_current_handle_run_distance(current_pos);
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
	handle_injector_t.LAST_LOC = get_current_handle_injector();			/*	校正时清零，关机时保存，开机是更新*/
	result = write_handle_injector_data();	
	if(!result)
		FDB_PRINT("handle_injector_t.LAST_LOC = %d\r\n",(handle_injector_t.LAST_LOC>>16));
	
	result = write_sys_para_t_data();
	if(!result)
		FDB_PRINT("sys_para_t.AGING_CYCLE = %d\r\n",sys_para_t.AGING_CYCLE);
	
	return result;
}

#include "power_control.h"
#include "get_voltage.h"
typedef enum
{
	POWER_CHARGE_IDLE = 0,
	OFF1_NO_CHG,			/*	1:	关机键有效，适配器未接入	*/
	OFF2_LOW_CHG,			/*	2:	关机键有效，适配器接入电压低 */
	OFF3_HIGH_CHG,			/*	3:	关机键有效，适配器接入电压高	*/
	ON1_CHG,				/*	4:	关机键有/无效，正确的适配器接入	*/
	ON2_NO_CHG,				/*	5:	关机键无效，适配器未接入	*/
	ON3_LOW_CHG,			/*	6:	关机键无效，适配器接入电压低	*/
	ON4_HIGH_CHG			/*	7:	关机键无效，适配器接入电压高	*/
}POWER_CHARGER_T;	/*	开关机及充电状态		*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.2
* 日    期：2022-08-03
* 函 数 名: power_on_off_detect
* 功能说明: 开关机检测
*
* 形    参: 
*
* 返 回 值:
*			
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sMotor_goOrigin;
void power_on_off_detect(void)
{
	static int8_t i8_result = 0;
	static POWER_CHARGER_T power_charger_t = POWER_CHARGE_IDLE;
	static bool already_pwr_on_flase = false;
	/*	1. 状态检测	*/
	if( isPWR_OFF() ){
		switch( is_charger_adapter_right() ){
			case 1:	power_charger_t = OFF1_NO_CHG; break;			/*	1:	关机键有效，适配器未接入，可关机	*/
			case 2: power_charger_t = OFF2_LOW_CHG; break;			/*	2:	关机键有效，适配器接入电压低，但是当前无法关机	 */
			case 3: power_charger_t = OFF3_HIGH_CHG; break;			/*	3:	关机键有效，适配器接入电压高，但是当前无法关机		*/
			default: power_charger_t = ON1_CHG; break;				/*	4:	关机键有效，正确的适配器接入，但是当前无法关机		*/
		}
	}else{
		switch( is_charger_adapter_right() ){
			case 1: power_charger_t = ON2_NO_CHG; break;			/*	5:	关机键无效，适配器未接入	*/
			case 2: power_charger_t = ON3_LOW_CHG; break;			/*	6:	关机键无效，适配器接入电压低	*/
			case 3: power_charger_t = ON4_HIGH_CHG; break;			/*	7:	关机键无效，适配器接入电压高	*/
			default: power_charger_t = ON1_CHG; break;				/*	4:	关机键无效，正确的适配器接入	*/
		}
	}
	/*	2. 状态执行	*/
	switch(power_charger_t){
		case POWER_CHARGE_IDLE: break;
		
		case OFF1_NO_CHG:	/*	关机键有效，正常关机	*/
			set_now_page(PAGE_PowerOff_End);
			set_motor_stop();
			set_use_times();
			FDB_PRINT("\r\nUser has use %d times\r\n", read_use_times());
			i8_result = save_bf_pwroff();
			if(!i8_result)	FDB_PRINT("\r\nSave data success!\r\n");
			else			FDB_PRINT("\r\nSave data failed!\r\n");
			osDelay(300);
			
//			handle_in_bottom = false;
//			xSemaphoreGive(sMotor_goOrigin);			/*	使电机回退至底部	*/
//			while(handle_in_bottom == false)
//				osDelay(50);

			PWR_OFF();
		break;
		
		case OFF2_LOW_CHG:
		case OFF3_HIGH_CHG:
		case ON1_CHG:
		case ON2_NO_CHG:
		case ON3_LOW_CHG:
		case ON4_HIGH_CHG:
			if(already_pwr_on_flase == false){
				PWR_ON();
				already_pwr_on_flase = true;
			}else
				;
		break;
	}
	
	/*	3. 电池电压监测	*/
	display_battary_state();
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-04
* 函 数 名: get_Calibration_Oil_index
* 功能说明: 从eeprom中加载校正好的液压系数 Oil_K, Oil_b
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
#include "eeprom.h"
float Oil_K = 71.32f;
float Oil_b	= 3.602f;
uint8_t get_fitting_oil_index(void)
{
	static REV_FLOAT_T Oil_K_data;
	static REV_FLOAT_T Oil_b_data;
	static uint8_t temp[4] = {0};
	float temp_kPa = 0;
	uint8_t ret = 0;
	
	ret = EEPROM_Read(0x01, temp, 4);		/*	读取Oil_K	*/
	Oil_K_data.uc_rev_data[0] = temp[0];
	Oil_K_data.uc_rev_data[1] = temp[1];
	Oil_K_data.uc_rev_data[2] = temp[2];
	Oil_K_data.uc_rev_data[3] = temp[3];
	
	ret = EEPROM_Read(0x05, temp, 4);		/*	读取Oil_b	*/
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
	FDB_PRINT("System read oil calibration data over, Oil_K= %3.2f, Oil_b= %3.2f!\r\n", Oil_K, Oil_b);
	return ret;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-14
* 函 数 名: current_g_from_kPa
* 功能说明: 根据读取的液压转化为实际的推力
* 形    参: 
      返回值单位： 克g
* 返 回 值: 
*********************************************************************************************************
*/
float Power_K = 14.77;
float Power_b = -3596;
int32_t current_g_from_kPa(float kPa)
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
* 函 数 名: cycle_reset_to_origin
* 功能说明: 复位电机至液压原点
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
void cycle_reset_to_origin(void)
{
	uint16_t distance_cycle = (handle_injector_t.DISTANCE >> 16);
	handle_injector_t.LAST_LOC = 0;
	if((distance_cycle < 1000)||(distance_cycle > 1300))
		handle_injector_t.DISTANCE = (MAX_DIS_CYCLES << 16);
	reset_cycle_to_bias();
}
