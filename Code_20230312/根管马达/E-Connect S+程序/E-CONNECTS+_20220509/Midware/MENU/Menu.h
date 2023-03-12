#ifndef __MENU_H
#define __MENU_H
#include "stm32f4xx_hal.h"

//#include "datatype.h"
#define TOP_MENU_INDEX 0

extern uint8_t	sel_page_flash;																	//sel页面刷新
extern uint8_t	sel_eeprom_flash;																//与sel页面相关的EEPROM刷新

extern uint8_t	set_page_flash;																	//set页面刷新
extern uint8_t	set_eeprom_flash;																//与set页面相关的EEPROM刷新

extern uint8_t	cal_page_flash;																	//cal页面刷新
extern uint8_t	cal_eeprom_flash;																//与cal页面相关的EEPROM刷新

extern uint8_t 	NeedFlash ;

typedef enum
{
    Input_Password_flag = 1,							// 输入密码设置
    Error_Password_flag,								// 密码错误
	AutoThetaBias_flag,									// 电机角度校正
	ThetaBiasFish_flag,									// 电机角度校正完成
    Cal_Noload_flag,									// 空载电流
	Cal_Noload_Run_flag,								// 空载电流测试进行中
    Cal_Torque_flag,									// 扭矩测试
	Cal_Torque_Run_flag,								// 扭矩测试进行中
	Cal_Torque_Run1_flag,								// 扭矩测试进行中
	Cal_Saving,											// 数据保存中
	Cal_Fishing,										// 扭矩测试进行中
} CAL_MODE_e;                 							// 参数设置标志位
extern CAL_MODE_e Cal_mode_flag; 

typedef enum
{
    OperationMode_flag = 1,								// 操作模式选择设置
    Speed_flag,											// 速度设置
    Torque_flag,										// 转矩设置
    CWAngle_flag,										// 正角度设置
    CCWAngle_flag,										// 负角度设置
    AutoStart_flag,										// 自动开始设置
    AutoStop_flag,										// 自动停止设置
	ApicalMode_flag,									// 根测保护设置
    BarPosition_flag,									// 根测参考点设置
} SEL_MODE_e;                 							// 参数设置标志位
extern SEL_MODE_e sel_mode_flag; 


typedef enum
{
    Versions_flag = 1,									// 版本号显示
    AutoPowerOff_flag,									// 自动关机设置设置
    AutoStandy_flag,									// 跳回待机界面时间设置
    BeepVol_flag,										// 蜂鸣器设置
    Bl_flag,											// 背光设置
    Hand_flag,											// 左右手设置
    Apexsensitivity_flag,								// 根测灵敏度设置
	Language_flag,										// 语言设置
    Calibration_flag,									// 校准设置
	RestoreFactory_flag,								// 恢复出厂设置
	MenuStartup_flag,
} SET_MODE_e;                 							// 参数设置标志位
extern SET_MODE_e set_mode_flag; 


typedef enum
{
    File_name_flag = 1,									// 文件夹名称
    Seq_name_flag,										// 锉个数
    Taper_flag,											// 锥度
    Number_flag,										// 锉号
    OperaMode_flag,										// 操作模式
    RecSpeed_flag,										// 速度
    torque_flag,										// 转矩
	AngleCW_flag,										// 正角度
    AngleCCW_flag,										// 负角度
	color_flag,											// 颜色
} NEW_FILE_e;                 							// 参数设置标志位
extern NEW_FILE_e new_file_flag; 

typedef struct 
{
    uint8_t KeyStateIndex; 				//当前状态索引号
    uint8_t KeyONState; 				//按下"启动"键时转向的状态索引号
    uint8_t KeySelState; 				//按下“选择”键时转向的状态索引号
    uint8_t KeyDnState;  				//按下"向下“键时转向的状态索引号
    uint8_t KeyUpState;  				//按下"向上"键时转向的状态索引号
    uint8_t KeyLongSelState; 			//长按”选择”键时转向的状态索引号
    void (*CurrentOperate)(); 			//当前状态应该执行的功能操作
} KbdTabStruct;
int WaitTimeOut(void);
#endif















