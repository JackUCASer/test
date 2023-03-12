
////////////////////////////////////////////////////////////////////
/// @file preSetting.h
/// @brief 系统进入主程序前的预处理，包括数据初始化等等
/// 
/// 文件详细描述：	void power_on()开机按键检测，不同按键组合，进入不同的模式
///					
///
///
///
///
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
////////////////////////////////////////////////////////////////////

#ifndef __PRESETTING_H_
#define __PRESETTING_H_



void power_on(void);	
void Power_mode_select(void);							// 开机根据不同按键组合，进入不同工作模式
void data_init(void);									// 数据初始化，读EEPROM，设置初始模式、菜单显示初始化等
void Start_Adc(void);									// ADC启动1次
void init_MC(void);										// 初始化电机参数

#endif