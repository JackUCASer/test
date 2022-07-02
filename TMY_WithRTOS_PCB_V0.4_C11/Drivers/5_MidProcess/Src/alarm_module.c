#include "alarm_module.h"	/*	使用串口进行声音提醒	*/
#include "page_mode.h"
#include "motor_controller.h"


void check_alarm_event(void)
{
	page_mode_alarm();
	motor_controller_alarm();
}


