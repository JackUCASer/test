#include "app_main.h"
#include "GL_Config.h"
//	Hardware
#include "get_voltage.h"

//	MP6570
#include "motor_misc.h"

#define F_DIV		2	/*	����ж�ˢ�·�Ƶ����: 1,100us�ж�һ�Σ�2�� 200us�ж�һ��	*/
//	ϵͳ�еĵ��ת�٣���ǣ��У�������⣬���Բ���д�����ݿ�
/* 	LOW,	MID,	HIGH	*/
const uint32_t injector_speed_t[4][3] = {
	{31.5*F_DIV,	31.5*F_DIV,		181*F_DIV},				/*	Normal	�ٶ�	[LOW, LOW, MID]	*/
	{181*F_DIV,		181*F_DIV,		380*F_DIV},				/*	Turbo 	�ٶ�	[MID, MID, HIGH]*/
	{31.5*F_DIV,	31.5*F_DIV,		31.5*F_DIV},			/*	STA   	�ٶ�	[LOW, LOW, LOW]	*/
	{200*F_DIV,		500*F_DIV,		700*F_DIV},				/*	ASP		����1		����2	*/
	};

void app_init(void)
{
	motor_initialize();
	HAL_Delay(1000);
	MP6570_AutoThetaBias(0x00, 200, 2000); 
	set_motor_run_in_speed_mode();
	reset_cycle_to_bias();					//	�����תȦ������
}


void app_exec(void)
{
	static uint16_t current_pos = 0;
	static uint16_t origin_pos = 0;
	static uint16_t top_pos = 500;
	static RUN_STATE_T run_state = FORWARD_WITH_LOW_SPEED;
	static RUN_STATE_T last_run_state = RUN_IDLE;
	
	while(1){
		//	1. ״̬�л�
		switch(run_state){
			case FORWARD_WITH_LOW_SPEED: 
				motor_to_goal_cycle = top_pos -10;
				set_speed_mode_speed(injector_speed_t[0][1], MOTOR_FWD_TO_GOAL);
				last_run_state = FORWARD_WITH_LOW_SPEED;
				run_state = KEEP_RUNNING;
			break;
			
			case BACK_WITH_LOW_SPEED:
				motor_to_goal_cycle = origin_pos +10;
				set_speed_mode_speed(-injector_speed_t[0][1], MOTOR_REV_TO_GOAL);
				last_run_state = BACK_WITH_LOW_SPEED;
				run_state = KEEP_RUNNING;
			break;
			
			case FORWARD_WITH_MIDDLE_SPEED: 
				motor_to_goal_cycle = top_pos -10;
				set_speed_mode_speed(injector_speed_t[0][2], MOTOR_FWD_TO_GOAL);
				last_run_state = FORWARD_WITH_MIDDLE_SPEED;
				run_state = KEEP_RUNNING;
			break;
			
			case BACK_WITH_MIDDLE_SPEED:
				motor_to_goal_cycle = origin_pos +10;
				set_speed_mode_speed(-injector_speed_t[0][2], MOTOR_REV_TO_GOAL);
				last_run_state = BACK_WITH_MIDDLE_SPEED;
				run_state = KEEP_RUNNING;
			break;
			
			case FORWARD_WITH_HIGH_SPEED: 
				motor_to_goal_cycle = top_pos -10;
				set_speed_mode_speed(injector_speed_t[1][2], MOTOR_FWD_TO_GOAL);
				last_run_state = FORWARD_WITH_HIGH_SPEED;
				run_state = KEEP_RUNNING;
			break;
			
			case BACK_WITH_HIGH_SPEED:
				motor_to_goal_cycle = origin_pos +10;
				set_speed_mode_speed(-injector_speed_t[1][2], MOTOR_REV_TO_GOAL);
				last_run_state = BACK_WITH_HIGH_SPEED;
				run_state = KEEP_RUNNING;
			break;
			
			default: break;
		}
		
		
		// 2. ״̬ת��
		if(motor_run_direction == MOTOR_FWD_GOAL){
			if(last_run_state == FORWARD_WITH_LOW_SPEED)
				run_state = BACK_WITH_LOW_SPEED;
			else if(last_run_state == FORWARD_WITH_MIDDLE_SPEED)
				run_state = BACK_WITH_MIDDLE_SPEED;
			else if(last_run_state == FORWARD_WITH_HIGH_SPEED)
				run_state = BACK_WITH_HIGH_SPEED;
			else 
				;
		}else if(motor_run_direction == MOTOR_REV_GOAL){
			if(last_run_state == BACK_WITH_LOW_SPEED)
				run_state = FORWARD_WITH_MIDDLE_SPEED;
			else if(last_run_state == BACK_WITH_MIDDLE_SPEED)
				run_state = FORWARD_WITH_HIGH_SPEED;
			else if(last_run_state == BACK_WITH_HIGH_SPEED)
				run_state = RUN_IDLE;
			else 
				;
		}else
			;
		
		HAL_Delay(50);
		current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
		myPrintf("current iq = %d, top_pos = %d, and current_pos = %d\r\n", iq, top_pos, current_pos);
	}
}



