#ifndef __MOTOR_MISC_H
#define __MOTOR_MISC_H


#include "customer_control.h"	/*	电机 */
#include "control.h"
#include "macros.h"
#include "mp6570.h"

typedef enum		
{
	MOTOR_IDLE = 0,
	// 正常过程
	MOTOR_FWD,				/*	电机正转	*/
	MOTOR_REV,				/*	电机反转	*/
	MOTOR_TOP,				/*	电机到达顶端	*/
	MOTOR_BOTTOM,			/*	电机到达底端	*/
	MOTOR_ORIGIN,			/*	电机到达原点	*/
	MOTOR_FWD_TO_ORIGIN,	/*	电机运行至原点	*/
	MOTOR_FWD_TO_GOAL,		/*	电机正转至目标位置	*/
	MOTOR_REV_TO_GOAL,		/*	电机反转至目标位置	*/
	MOTOR_FWD_GOAL,			/*	电机正转到达目标位置	*/
	MOTOR_REV_GOAL,			/*	电机反转到达目标位置	*/
	
	// 校正过程
	MOTOR_CALIB_FWD,		/*	电机正转	*/
	MOTOR_CALIB_REV,		/*	电机反转	*/
	MOTOR_CALIB_TOP,		/*	电机到达顶端	*/
	MOTOR_CALIB_BOTTOM,		/*	电机到达底端	*/
	MOTOR_CALIB_TO_ORIGIN,	/*	运行至电机原点	*/
	MOTOR_CALIB_ORIGIN,		/*	电机到达原点	*/
	
	// 老化过程
	MOTOR_AGING_FWD,		/*	电机正转	*/
	MOTOR_AGING_REV,		/*	电机反转	*/
	MOTOR_AGING_TOP,		/*	电机到达顶端	*/
	MOTOR_AGING_BOTTOM,		/*	电机到达底端	*/
	MOTOR_AGING_TO_ORIGIN,	/*	运行至电机原点	*/
	MOTOR_AGING_ORIGIN,		/*	电机到达原点	*/
	
}MOTOR_DIRECTION_T;		//	定义电机运转的方向：这个枚举应该放在电机库中,是一种通用运行状态

#define CYCLE_BIAS			10				/*	位于液压原点时，电机圈数值*/
#define UP_CYCLE			20				/*	顶部预留圈数	*/
#define DOWN_CYCLE			120				/*	底部预留圈数	*/

/*	新增于2022-04-27，用于存储电机运行的圈数和角度	*/
extern uint16_t motor_cycle;
extern uint16_t motor_angle;
extern MOTOR_DIRECTION_T motor_run_direction;
extern uint16_t motor_to_origin_cycle;		/*	电机至 origin 位置时，motor_cycle对应的数值	*/
extern uint16_t motor_to_goal_cycle;		/*	电机至  goal  位置时，motor_cycle对应的数值	*/

void motor_initialize(void);
void set_motor_run_in_speed_mode(void);
void set_motor_stop(void);
void set_speed_mode_speed(int speed, MOTOR_DIRECTION_T motor_direction_t);
void reset_cycle_to_bias(void);

#endif
