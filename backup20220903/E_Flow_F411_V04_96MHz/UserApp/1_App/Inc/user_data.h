#ifndef	__USER_DATA_H 
#define	__USER_DATA_H
#include <stdint.h>
#include <flashdb.h>		/*	数据库依赖	*/
#include "arm_math.h"		/*	ARM数学运算依赖	*/

#include "GL_Config.h"			/*	依赖myPrintf	*/
#define __DEBUG_USRDATA

#ifdef __DEBUG_USRDATA
#define USRDATA_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define USRDATA_PRINTF(format, ...)
#endif

/*	----------------------------------------用户数据结构-----------------------------------------------	*/
//	0. 系统设置相关参数
typedef struct
{
	uint32_t ASPIRATE;								/*	回吸aspirate: 0/1	*/
	
	uint32_t SMART_REMINDER;
	uint32_t TRAIN_MODE;
	uint32_t VOLUME;								/*	音量volume: 0-100	*/
	uint32_t LIGHT;									/*	亮度light: 0-100	*/
	
	uint32_t AGING_CYCLE;							/*	老化aging cycle: 老化圈数	*/
	uint32_t USE_TIMES;								/*	使用次数，the times of use	*/
	uint32_t AUTO_BACK;
}SYS_PARA_T;

//	1. 脚踏相关参数：校正后更新
typedef struct 
{
	uint32_t MIN;									/*	校正后，最小值	*/
	uint32_t MAX;									/*	校正后，最大值	*/
	uint32_t QUARTER_DELTA;							/*	校正后，(MAX - MIN)/4 */
}FOOT_PEDAL_T;

//	2. 手柄-注射器相关的参数：ORIGIN_LOC、TOP_LOC 和 DISTANCE 校正后更新，LAST_LOC关机时保存。
typedef struct
{
	uint32_t ORIGIN_LOC;							/*	手柄底部位置：仅校正后更新	*/
	uint32_t TOP_LOC;								/*	手柄顶部位置：仅校正后更新*/
	uint32_t DISTANCE;								/*	手柄-注射器 总行程：仅校正后更新	*/
	uint32_t LAST_LOC;								/*	上次电机停转时所在位置：关机时更新	*/
	uint32_t BOTTM2ORIGIN;
}HANDLE_INJECTOR_T;

//	3. 在stm32中float占4个字节
typedef union{
	float f_rev_data;
	uint8_t uc_rev_data[4];
}REV_FLOAT_T;
typedef union{
	uint32_t u32_rev_data;
	uint8_t uc_rev_data[4];
}REV_U32_T;

/*	------------------------------------------全局宏定义区---------------------------------------------------	*/			
#define F_DIV					2					/*	电机中断刷新分频因子: 1,100us中断一次；2， 200us中断一次	*/
#define N_TIME_CHECK_ORING		2					/*	定义用户使用N次检查一次O型圈	*/

/*	亮度和音量索引	*/
#define MAX_LIGHT_INDEX			5					/*	最大亮度索引	*/
#define MAX_VOLUME_INDEX		5					/*	最大音量索引	*/
/*	------------------------音乐索引------------------------	*/
/*	通用	*/
#define KEY_INDEX					0		/*	按键音ok	*/
#define ASPIRATE_BACK_INDEX			2		/*	回吸时的音乐索引ok	*/
#define CHANGE_INDEX				3		/*	换药瓶时的音乐（换药瓶、回退）ok	*/
#define REMAIN_INDEX				7		/*	剩余药量提醒	ok*/
#define SMART_REMINDER_ON_INDEX		19		/*	"smart reminder on 智能提醒开"ok	*/
#define TRAIN_MODE_ON_INDEX			20		/*	"train mode on 训练模式开"ok	*/
/*	常规	*/
#define LOW_INJECTION_INDEX			4		/*	低速注射时的音乐ok	*/
#define MIDDLE_INJECTION_INDEX		5		/*	中速注射时的音乐ok	*/
#define HIGH_INJECTION_INDEX		6		/*	高速注射时的音乐ok	*/
#define ALARM_INDEX					29		/*	注册过压提醒	ok*/
/*	常规与Smart Reminder共用	*/
#define PDL_INDEX					1		/*	"PDL"区域播报ok	*/
/*	Smart Reminder与Train Mode共用	*/
#define ASPIRATE_ON_INDEX			8		/*	"aspirate on  回吸开"ok	*/
#define ASPIRATE_OFF_INDEX			9		/*	"aspirate off 回吸关"ok	*/
#define NORMAL_MODE_INDEX			10		/*	"normal mode  常规模式"ok	*/
#define FAST_MODE_INDEX				11		/*	"fast mode	  快速模式"ok	*/
#define PDL_MODE_INDEX				12		/*	"pdl modepdl  模式"ok	*/
#define OVER_PRESSURE_INDEX			18		/*	"over pressure 注射区域组织过压"ok	*/
#define CHECK_O_RING_INDEX			21		/*	"check o ring 检查O型圈"	ok*/
#define BATTERY_LOW_INDEX			22		/*	"battery low 电量低"	*/
/*	Smart Reminder专用	*/
#define SPEED_WELL_INDEX			13		/*	"speed well 速度适宜"	*/
#define SPEED_QUICK_INDEX			14		/*	"speed quick 速度快"	*/
#define SPEED_HIGH_INDEX			15		/*	"speed high 速度高"	*/
#define REMAIN_LOW_INDEX			16		/*	"remain low 剩余药量低"ok	*/
#define CARTRIDGE_EMPTY_INDEX		17		/*	"cartridge empty 药瓶空"ok	*/
/*	Train Mode专用	*/
#define LOW_SPEED_INDEX				23		/*	"low speed 低速"ok	*/
#define MIDDLE_SPEED_INDEX			24		/*	"middle speed 中速"ok	*/
#define HIGH_SPEED_INDEX			25		/*	"high speed 高速"ok	*/
#define CARTRIDGE_REMAIN_LOW_INDEX	26		/*	"cartridge remian low 剩余药量低"ok	*/
#define CHANGE_CARTRIDGE_INDEX		27		/*	"please change the anesthesic cartridge 请更换药瓶"ok	*/
#define REACH_PDL_INDEX				28		/*	"reach pdl 到达PDL区域"ok	*/


/*	------------------------------------------全局变量及常量-------------------------------------------------	*/
extern SYS_PARA_T sys_para_t;						/*	系统参数	*/
extern FOOT_PEDAL_T	foot_pedal_t;					/*	脚踏参数	*/
extern HANDLE_INJECTOR_T handle_injector_t;			/*	注射器参数	*/

extern const uint32_t injector_speed_t[4][3];		/*	运行速度列表	*/
extern const uint8_t system_light[6];				/*	亮度列表	*/
extern const uint8_t system_volume[6];				/*	音量列表	*/

extern float Oil_K;									/*	液压系数 k	*/
extern float Oil_b;									/*	液压系数 b	*/

extern float32_t dynamic_pressure_mV;				/*	AD读取液压传感器的电压值，单位：mV	*/
extern float32_t dynamic_pressure_kPa;				/*	dynamic_pressure_mV转化的液压值，单位kPa, 单精度浮点型	*/
extern uint32_t  dynamic_pressure;					/*	dynamic_pressure_mV转化的液压值，单位kPa, 无符号整型	*/
extern uint16_t  dynamic_pressure_kPa_F;			/*	Hampel滤波后的液压，单位：kPa, 无符号整型		*/

extern bool handle_in_top;							/*	丝杆处于顶端标志位	*/
extern bool handle_in_bottom;						/*	丝杆处于底端标志位	*/
extern bool handle_in_origin;						/*	丝杆处于液压原点位置	*/
extern bool sys_para_update_flag;					/*	系统参数更新标致	*/

/*	------------------------------------------HMI接口-------------------------------------------------	*/
bool open_asp(void);								
bool close_asp(void);
bool is_asp_open(void);								/*	回吸功能		*/

bool open_smart_reminder(void);
bool close_smart_reminder(void);
bool is_smart_reminder_open(void);					/*	智能提醒功能	*/

bool open_train_mode(void);
bool close_train_mode(void);
bool is_train_mode_open(void);						/*	训练模式		*/

bool set_system_vol(uint8_t volume);
uint8_t get_system_vol(void);						/*	系统音量		*/

bool set_system_light(uint8_t light);
uint8_t get_system_light(void);						/*	系统亮度		*/

void set_use_times(void);
uint32_t read_use_times(void);						/*	获取用户使用次数	*/

bool open_autoback(void);							/*	开机自动回退	*/
bool close_autoback(void);
bool is_autoback_off(void);


/*	--------------------------------------数据库操作接口-----------------------------------------------	*/
int8_t tmy_database_init(void);										/*	数据库初始化操作	*/
int8_t read_handle_injector_data(void);								/*	读取手柄注射器相关参数： -1 失败； 0 成功	*/
int8_t write_handle_injector_data(void);							/*	写入手柄注射器相关参数： -1 失败； 0 成功	*/
int8_t read_foot_pedal_t_data(void);								/*	读取脚踏相关参数： -1 失败； 0 成功	*/
int8_t write_foot_pedal_t_data(void);								/*	写入脚踏相关参数： -1 失败； 0 成功	*/
int8_t read_sys_para_t_data(void);									/*	读取系统相关参数： -1 失败； 0 成功	*/
int8_t write_sys_para_t_data(void);									/*	写入系统相关参数： -1 失败； 0 成功	*/


/*	----------------------------------------其他接口---------------------------------------------------	*/
void power_on_off_detect(void);										/*	开关机及充电检测	*/
uint8_t get_fitting_oil_index(void);								/*	上电读取液压传感器拟合系数	*/

uint32_t get_current_handle_injector(void);							/*	获取当前丝杆位置	*/
uint16_t get_current_handle_run_distance(uint16_t current_pos);		/*	获取当前丝杆已走的圈数	*/
uint8_t compute_remain_dose(uint16_t current_pos);					/*	获取剩余药液余量	*/
int32_t current_g_from_kPa(float kPa);								/*	通过液压计算推力	*/

void cycle_reset_to_origin(void);									/*	复位电机至液压原点	*/
void PowerOFF(void);

#endif
