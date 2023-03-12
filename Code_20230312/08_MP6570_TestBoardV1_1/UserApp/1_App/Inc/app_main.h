#ifndef __APP_MAIN_H
#define __APP_MAIN_H
#include "main.h"

typedef enum{
	RUN_IDLE = 0,
	
	FORWARD_WITH_LOW_SPEED = 1,
	BACK_WITH_LOW_SPEED = 2,
	
	FORWARD_WITH_MIDDLE_SPEED = 3,
	BACK_WITH_MIDDLE_SPEED = 4,
	
	FORWARD_WITH_HIGH_SPEED = 5,
	BACK_WITH_HIGH_SPEED = 6,
	
	KEEP_RUNNING = 7
}RUN_STATE_T;


void app_init(void);
void app_exec(void);
#endif
