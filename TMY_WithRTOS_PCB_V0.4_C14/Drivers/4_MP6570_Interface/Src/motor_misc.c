#include "motor_misc.h"

uint16_t motor_cycle = 0;
uint16_t motor_angle = 0;
MOTOR_DIRECTION_T motor_run_direction = MOTOR_IDLE;
uint16_t motor_to_origin_cycle = 0;			/*	电机至 origin 位置时，motor_cycle对应的数值	*/
uint16_t motor_to_goal_cycle = 0;			/*	电机至  goal  位置时，motor_cycle对应的数值	*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-05-13
* 函 数 名: motor_initialize
* 功能说明: 电机初始化
*
* 形    参: 
*
* 返 回 值:
*			
*********************************************************************************************************
*/
void motor_initialize(void)
{
	init_registers();									//初始化MP6570内部的寄存器
	set_speed_slope(20);								//初始化电机加速斜率
	HAL_Delay(1);
	mp6570_enable();  									//enable mp6570
	HAL_Delay(2);
	mp6570_init(0x00);									//写入MP6570内部的寄存器
	MP6570_WriteRegister(0x00,0x60,0x0000);				//使MP6570进入正常工作模式
	HAL_Delay(100);										//delay_1ms 100ms wait for sensor to be stable	
	reset_parameters();
	LOOPTIMER_ENABLE;
	set_motor_run_in_speed_mode();
}

//	设置电机工作在速度模式
void set_motor_run_in_speed_mode(void)
{
	motor_settings.mode = 0;							//	速度模式
	motor_settings.forward_speed = 0;
	motor_settings.reverse_speed = 0;	
	motor_settings.upper_threshold =  5000;				//	转矩保护上限设置
	motor_settings.lower_threshold =  2500;
	
	update_settings(&motor_settings);
//	start();
}

//	电机停止
void set_motor_stop(void)
{
	stop();
	motor_run_direction = MOTOR_IDLE;
}

// speed >0, 电机正转； speed <0, 电机反转
void set_speed_mode_speed(int speed, MOTOR_DIRECTION_T motor_direction_t)
{
	if(speed == 0){
		set_motor_stop();
		return ;
	}
	motor_run_direction = 	motor_direction_t;
		
	motor_settings.forward_speed = speed;
	motor_settings.reverse_speed = -speed;
	update_settings(&motor_settings);
	start();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2022-07-19
* 函 数 名: reset_motor_cycle
* 功能说明: 电机初始化
*
* 形    参: 
*
* 返 回 值:
*			返回motor_cycle的初始值
*********************************************************************************************************
*/
void reset_cycle_to_bias(void)
{
	motor_cycle = CYCLE_BIAS;
}
