#ifndef __MOTOR_MISC_H
#define __MOTOR_MISC_H


#include "customer_control.h"	/*	电机 */
#include "control.h"
#include "macros.h"
#include "mp6570.h"

typedef enum		
{
	MOTOR_IDLE = 0,
	MOTOR_FWD,
	MOTOR_REV
}MOTOR_DIRECTION_T;		//	定义电机运转的方向：这个枚举应该放在电机库中,是一种通用运行状态

/*	新增于2022-04-27，用于存储电机运行的圈数和角度	*/
extern uint16_t motor_cycle;
extern uint16_t motor_angle;
extern MOTOR_DIRECTION_T motor_run_direction;

void motor_initialize(void);
void set_motor_run_in_speed_mode(void);
void set_motor_stop(void);
void set_speed_mode_speed(int speed);

#endif
