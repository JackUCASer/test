#ifndef __MOTOR_MISC_H
#define __MOTOR_MISC_H


#include "customer_control.h"	/*	��� */
#include "control.h"
#include "macros.h"
#include "mp6570.h"

typedef enum		
{
	MOTOR_IDLE = 0,
	// ��������
	MOTOR_FWD,				/*	����1�������ת	*/
	MOTOR_REV,				/*	����2�������ת	*/
	MOTOR_TOP,				/*	������ﶥ��	*/
	MOTOR_BOTTOM,			/*	�������׶�	*/
	MOTOR_ORIGIN,			/*	�������ԭ��	*/
	MOTOR_FWD_TO_ORIGIN,	/*	����3�����ǰ����ԭ��	*/
	MOTOR_REV_TO_ORIGIN,	/*	����4�����������ԭ��*/
	MOTOR_FWD_TO_GOAL,		/*	����5�������ת��Ŀ��λ��	*/
	MOTOR_REV_TO_GOAL,		/*	����6�������ת��Ŀ��λ��	*/
	MOTOR_FWD_GOAL,			/*	�����ת����Ŀ��λ��	*/
	MOTOR_REV_GOAL,			/*	�����ת����Ŀ��λ��	*/
	
	// У������
	MOTOR_CALIB_FWD,		/*	����1�������ת	*/
	MOTOR_CALIB_REV,		/*	����2�������ת	*/
	MOTOR_CALIB_TOP,		/*	������ﶥ��	*/
	MOTOR_CALIB_BOTTOM,		/*	�������׶�	*/
	MOTOR_CALIB_TO_ORIGIN,	/*	����3�����������ԭ��	*/
	MOTOR_CALIB_ORIGIN,		/*	�������ԭ��	*/
	
	// �ϻ�����
	MOTOR_AGING_FWD,		/*	����1�������ת	*/
	MOTOR_AGING_REV,		/*	����2�������ת	*/
	MOTOR_AGING_TOP,		/*	������ﶥ��	*/
	MOTOR_AGING_BOTTOM,		/*	�������׶�	*/
	MOTOR_AGING_TO_ORIGIN,	/*	����3�����������ԭ��	*/
	MOTOR_AGING_ORIGIN,		/*	�������ԭ��	*/
	
}MOTOR_DIRECTION_T;		//	��������ת�ķ������ö��Ӧ�÷��ڵ������,��һ��ͨ������״̬

#define MAX_DIS_CYCLES		1080			/*	˿�����г�	*/
#define CYCLE_BIAS			10				/*	λ��Һѹԭ��ʱ�����Ȧ��ֵ*/
#define UP_CYCLE			20				/*	����Ԥ��Ȧ��	*/
#define DOWN_CYCLE			33				/*	�ײ�Ԥ��Ȧ��	*/
#define TOP_REF_CYCLE		65535			/*	����������ʱ�����õ��Ȧ����ֵ	*/
#define MAX_BOTTOM_ORIGIN	350				/*	�׶˵�ԭ�㣬������(���Ȧ��)*/

/*	������2022-04-27�����ڴ洢������е�Ȧ���ͽǶ�	*/
extern int32_t movemean_iq;
extern uint16_t motor_cycle;
extern uint16_t motor_angle;
extern uint16_t delta_angle;
extern uint16_t delta_angle_cnt;
extern uint16_t motor_to_origin_cycle;		/*	����� origin λ��ʱ��motor_cycle��Ӧ����ֵ	*/
extern uint16_t motor_to_goal_cycle;		/*	�����  goal  λ��ʱ��motor_cycle��Ӧ����ֵ	*/
extern MOTOR_DIRECTION_T motor_run_direction;
extern MOTOR_DIRECTION_T recovery_motor_run_dir;	/*	���Ͻ������������ת����	*/
extern int recover_speed;							/*	���Ͻ������������ת�ٶ�	*/


/*	�ⲿ�����ӿ�	*/
void motor_initialize(void);
void set_motor_run_in_speed_mode(void);
void set_motor_stop(void);
void set_speed_mode_speed(int speed, MOTOR_DIRECTION_T motor_direction_t);
void reset_cycle_to_bias(void);
uint32_t Get_Current_Handle_Injector_Pos(void);
#endif
