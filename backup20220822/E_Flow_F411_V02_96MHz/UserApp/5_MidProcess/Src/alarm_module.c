#include "alarm_module.h"	/*	ʹ�ô��ڽ�����������	*/
#include "page_mode.h"
#include "motor_controller.h"


void check_alarm_event(void)
{
	page_mode_alarm();
	motor_controller_alarm();
}


