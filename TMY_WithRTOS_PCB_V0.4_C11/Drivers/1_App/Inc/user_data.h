#ifndef	__USER_DATA_H 
#define	__USER_DATA_H
#include <stdint.h>
#include <flashdb.h>		/*	数据库依赖	*/

/*	----------------------------------------用户数据结构-----------------------------------------------	*/
//	0. 系统设置相关参数
typedef struct
{
	uint32_t ASPIRATE;	/*	回吸aspirate: 0/1	*/
	
	uint32_t SMART_REMINDER;
	uint32_t TRAIN_MODE;
	uint32_t VOLUME;	/*	音量volume: 0-100	*/
	uint32_t LIGHT;	/*	亮度light: 0-100	*/
	
	uint32_t AGING_CYCLE;	/*	老化aging cycle: 老化圈数	*/
	uint32_t USE_TIMES;	/*	使用次数，the times of use	*/	
}SYS_PARA_T;

//	1. 脚踏相关参数：校正后更新
typedef struct 
{
	uint32_t MIN;			/*	校正后，最小值	*/
	uint32_t MAX;			/*	校正后，最大值	*/
	uint32_t QUARTER_DELTA;	/*	校正后，(MAX - MIN)/4 */
}FOOT_PEDAL_T;

//	2. 手柄-注射器相关的参数：BOTTOM_LOC、TOP_LOC 和 DISTANCE 校正后更新，LAST_LOC关机时保存。
typedef struct
{
	uint32_t BOTTOM_LOC;			/*	手柄底部位置：仅校正后更新	*/
	uint32_t TOP_LOC;				/*	手柄顶部位置：仅校正后更新*/
	uint32_t DISTANCE;				/*	手柄-注射器 总行程：仅校正后更新	*/
	uint32_t LAST_LOC;				/*	上次电机停转时所在位置：关机时更新	*/
}HANDLE_INJECTOR_T;

//	3. 在stm32中float占4个字节
typedef union{
	float f_rev_data;
	uint8_t uc_rev_data[4];
}REV_FLOAT_T;

/*	---------------------------------------全局变量区-----------------------------------------------	*/
#define F_DIV	2								/*	电机中断刷新分频因子: 1,100us中断一次；2， 200us中断一次	*/
#define N_TIME_CHECK_ORING		2				/*	定义用户使用N次检查一次O型圈	*/
#define MAX_LIGHT_INDEX			5				/*	最大亮度索引	*/
#define MAX_VOLUME_INDEX		5				/*	最大音量索引	*/
/*	------------------------音乐索引------------------------	*/
#define KEY_INDEX			0
#define PDL_INDEX			1
#define PRESS_INDEX			2
#define ASP_INDEX			3
#define TOP_BOTTOM_INDEX	4
#define LOW_INDEX			5
#define MID_INDEX			6
#define HIGH_INDEX			7


extern SYS_PARA_T sys_para_t;					/*	系统参数	*/
extern FOOT_PEDAL_T	foot_pedal_t;				/*	脚踏参数	*/
extern HANDLE_INJECTOR_T handle_injector_t;		/*	注册器参数	*/
extern const uint32_t injector_speed_t[4][3];	/*	运行速度列表	*/
extern const uint8_t system_light[6];			/*	亮度列表	*/
extern const uint8_t system_volume[6];			/*	音量列表	*/
extern float Oil_K;
extern float Oil_b;
extern bool handle_in_top;
extern bool handle_in_bottom;


bool open_asp(void);
bool close_asp(void);
bool is_asp_open(void);

bool open_smart_reminder(void);
bool close_smart_reminder(void);
bool is_smart_reminder_open(void);

bool open_train_mode(void);
bool close_train_mode(void);
bool is_train_mode_open(void);

bool set_system_vol(uint8_t volume);
uint8_t get_system_vol(void);

bool set_system_light(uint8_t light);
uint8_t get_system_light(void);

void set_use_times(void);
uint32_t read_use_times(void);				/*	获取用户使用次数	*/



uint8_t compute_remain_dose(uint16_t current_pos);
/*	--------------------------------------数据库操作接口-----------------------------------------------	*/
int8_t read_handle_injector_data(void);		/*	读取手柄注射器相关参数： -1 失败； 0 成功	*/
int8_t write_handle_injector_data(void);	/*	写入手柄注射器相关参数： -1 失败； 0 成功	*/
int8_t read_foot_pedal_t_data(void);		/*	读取脚踏相关参数： -1 失败； 0 成功	*/
int8_t write_foot_pedal_t_data(void);		/*	写入脚踏相关参数： -1 失败； 0 成功	*/
int8_t read_sys_para_t_data(void);			/*	读取系统相关参数： -1 失败； 0 成功	*/
int8_t write_sys_para_t_data(void);			/*	写入系统相关参数： -1 失败； 0 成功	*/

uint32_t get_current_handle_injector(void);	/*	获取当前丝杆位置	*/
void power_on_off_detect(void);				/*	开关机及充电检测	*/
uint8_t get_fitting_oil_index(void);		/*	上电读取液压传感器拟合系数	*/
int32_t current_g_from_kPa(float kPa);		/*	通过液压计算推力	*/

#endif
