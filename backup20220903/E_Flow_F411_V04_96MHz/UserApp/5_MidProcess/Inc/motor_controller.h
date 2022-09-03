/*	µç»ú¿ØÖÆÆ÷	*/
#ifndef __MOTOR_CONTROLLER_H
#define __MOTOR_CONTROLLER_H
#include <stdint.h>
#include "GL_Config.h"

//#define __DEBUG_CONTROLLER

#ifdef __DEBUG_CONTROLLER
#define CONTROLLER_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define CONTROLLER_PRINTF(format, ...)
#endif


typedef enum
{
	BACK_FUNCTION_NULL = 0,
	BACK_FUNCTION_ASP,
	BACK_FUNCTION_BOTTOM

}BACK_FUNCTION_T;

typedef struct
{
	int back_distance;
	BACK_FUNCTION_T bf_t;
}MOTOR_BACK_T;



void motor_controller_alarm(void);
void motor_controller_init(void);
void motor_controller(void);

#endif
