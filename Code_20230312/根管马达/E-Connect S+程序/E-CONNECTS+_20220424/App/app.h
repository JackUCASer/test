
////////////////////////////////////////////////////////////////////
/// @file app.h
/// @brief 顶层应用程序
/// 
/// 文件详细描述：	
///					
///
///
///
///
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210825
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
////////////////////////////////////////////////////////////////////

#ifndef __APP_H
#define __APP_H

#define PwrON()		LCD_BLK_Set()
#define	PwrOFF()	LCD_BLK_Clr()

void App_Init(void);
void AppTest(void);;



#endif
