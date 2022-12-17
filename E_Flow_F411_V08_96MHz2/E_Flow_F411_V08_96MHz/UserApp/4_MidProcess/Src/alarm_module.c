#include "alarm_module.h"	/*	使用串口进行声音提醒	*/
#include "Page_Mode.h"
#include "Motor_Controller.h"


void Check_Alarm_Event(void)
{
	Page_Mode_Alarm();
	Motor_Controller_Alarm();
}


