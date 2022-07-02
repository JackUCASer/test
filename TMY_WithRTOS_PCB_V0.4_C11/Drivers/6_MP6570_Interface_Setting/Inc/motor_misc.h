#ifndef __MOTOR_MISC_H
#define __MOTOR_MISC_H


#include "customer_control.h"	/*	��� */
#include "control.h"
#include "macros.h"
#include "mp6570.h"

typedef enum		
{
	MOTOR_IDLE = 0,
	MOTOR_FWD,
	MOTOR_REV
}MOTOR_DIRECTION_T;		//	��������ת�ķ������ö��Ӧ�÷��ڵ������,��һ��ͨ������״̬

/*	������2022-04-27�����ڴ洢������е�Ȧ���ͽǶ�	*/
extern uint16_t motor_cycle;
extern uint16_t motor_angle;
extern MOTOR_DIRECTION_T motor_run_direction;

void motor_initialize(void);
void set_motor_run_in_speed_mode(void);
void set_motor_stop(void);
void set_speed_mode_speed(int speed);

#endif
