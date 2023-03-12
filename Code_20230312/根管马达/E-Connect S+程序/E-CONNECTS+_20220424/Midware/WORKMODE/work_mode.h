
////////////////////////////////////////////////////////////////////
/// @file work_mode.h
/// @brief 工作模式，主要有待机模式、显示LOGO模式、运行模式等等
/// 
/// 文件详细描述：	WorkMode_e Rotory(WorkMode_e mode);					// 连续运动模式，包括正转、反转
/// 				WorkMode_e Reciproc(WorkMode_e mode);				// 往复运动模式
/// 				WorkMode_e ATC_RUN(WorkMode_e mode);				// ATC运动模式
/// 				WorkMode_e Measure(WorkMode_e mode);				// 根测模式
/// 				WorkMode_e RunWithApex(WorkMode_e mode);			// 运行+根测模式
/// 				WorkMode_e RecWithApex(WorkMode_e mode);			// 往复+根测模式
/// 				WorkMode_e ATCwithApex(WorkMode_e mode);			// ATC+根测模式
/// 				WorkMode_e Standby(WorkMode_e mode);				// 待机模式	
/// 				WorkMode_e Charging(WorkMode_e mode);				// 充电模式
/// 				WorkMode_e Fault(WorkMode_e mode);					// 运行错误模式
/// 				WorkMode_e PowerOff(WorkMode_e mode);				// 关机预备模式
/// 				WorkMode_e DispOff(WorkMode_e mode);				// 关闭显示模式
/// 				WorkMode_e Displaylogo(WorkMode_e mode);			// 显示LOGO模式
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

#ifndef __WORK_MODE_H_
#define __WORK_MODE_H_
#include "datatype.h"

typedef enum
{
    Rotory_mode = 0,								// 连续运动模式
    Reciproc_mode,									// 往复运动模式
    Atc_mode,										// ATC运动模式
    Measure_mode,									// 根测模式
    RunWithApex_mode,								// 运行+根测模式
    RecWithApex_mode,								// 往复+根测模式
    ATRwithApex_mode,								// ATC+根测模式
	Standby_mode,									// 待机模式
    Charging_mode,									// 充电模式
    Fault_mode,										// 错误模式
    PowerOff_mode,									// 自动关机模式
    DispOff_mode,									// 关闭显示模式
	DisplayLogo_mode,								// 显示LOGO模式
} WorkMode_e;                 						// 工作模式枚举
extern WorkMode_e WorkMode;

WorkMode_e Rotory(WorkMode_e mode);					// 连续运动模式，包括正转、反转
WorkMode_e Reciproc(WorkMode_e mode);				// 往复运动模式
WorkMode_e ATC_RUN(WorkMode_e mode);				// ATC运动模式
WorkMode_e Measure(WorkMode_e mode);				// 根测模式
WorkMode_e RunWithApex(WorkMode_e mode);			// 运行+根测模式
WorkMode_e RecWithApex(WorkMode_e mode);			// 往复+根测模式
WorkMode_e ATCwithApex(WorkMode_e mode);			// ATC+根测模式
WorkMode_e Standby(WorkMode_e mode);				// 待机模式	
WorkMode_e Charging(WorkMode_e mode);				// 充电模式
WorkMode_e Fault(WorkMode_e mode);					// 运行错误模式
WorkMode_e PowerOff(WorkMode_e mode);				// 关机预备模式
WorkMode_e DispOff(WorkMode_e mode);				// 关闭显示模式
WorkMode_e Displaylogo(WorkMode_e mode);			// 显示LOGO模式
WorkMode_e RunningTurn2Apex(WorkMode_e mode);		// 从正常运行模式检测到根测信号，转到边扩边测模式
WorkMode_e MonitorApexStart(WorkMode_e mode);		// 边扩边测监视程序,必须放在按键扫描函数后面
WorkMode_e MonitorApexStop(WorkMode_e mode);		// 边扩边测监视程序,离开根管，自动停止
void MonitorApexAction(WorkMode_e mode);			// 边扩边测监视程序,快到设定根测点时，选择如何调整电机状态
void Turn2Standby(WorkMode_e mode);					// 返回Standby前对参数归零
void Motor2Reverse();
void Motor2Forward();
void Motor2INREC();
void Motor2OUTREC();


#endif




