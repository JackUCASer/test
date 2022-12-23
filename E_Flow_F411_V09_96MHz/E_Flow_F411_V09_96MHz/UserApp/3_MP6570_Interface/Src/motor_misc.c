#include "motor_misc.h"

int32_t movemean_iq = 0;
uint16_t motor_cycle = 0;
uint16_t motor_angle = 0;
uint16_t delta_angle = 0;
uint16_t delta_angle_cnt = 0;
MOTOR_DIRECTION_T motor_run_direction = MOTOR_IDLE;


uint16_t motor_to_origin_cycle = 0;			/*	����� origin λ��ʱ��motor_cycle��Ӧ����ֵ	*/
uint16_t motor_to_goal_cycle = 0;			/*	�����  goal  λ��ʱ��motor_cycle��Ӧ����ֵ	*/


// ���ڵ�����Ͻ������ֳ��ָ�
MOTOR_DIRECTION_T recovery_motor_run_dir = MOTOR_IDLE;	/*	���Ͻ������������ת����	*/
int recover_speed = 0;									/*	���Ͻ������������ת�ٶ�	*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-05-13
* �� �� ��: motor_initialize
* ����˵��: �����ʼ��
*
* ��    ��: 
*
* �� �� ֵ:
*			
*********************************************************************************************************
*/
void motor_initialize(void)
{
	init_registers();									//��ʼ��MP6570�ڲ��ļĴ���
	set_speed_slope(20);								//��ʼ���������б��
	HAL_Delay(1);
	mp6570_enable();  									//enable mp6570
	HAL_Delay(2);
	mp6570_init(0x00);									//д��MP6570�ڲ��ļĴ���
	MP6570_WriteRegister(0x00,0x60,0x0000);				//ʹMP6570������������ģʽ
	HAL_Delay(100);										//delay_1ms 100ms wait for sensor to be stable	
	reset_parameters();									//����װ�ص�ǰ����Ĳ���
	LOOPTIMER_ENABLE;
	set_motor_run_in_speed_mode();
}

//	���õ���������ٶ�ģʽ
void set_motor_run_in_speed_mode(void)
{
	motor_settings.mode = 0;							//	�ٶ�ģʽ
	motor_settings.autorev_mode = 2;					//	�ﵽת�ؾ�ֹͣ
	motor_settings.forward_speed = 0;
	motor_settings.reverse_speed = 0;	
	motor_settings.upper_threshold =  5000;				//	ת�ر�����������
	motor_settings.lower_threshold =  2500;
	
	update_settings(&motor_settings);
//	start();
}

//	���ֹͣ
void set_motor_stop(void)
{
	stop();
	motor_run_direction = MOTOR_IDLE;
}

// speed >0, �����ת�� speed <0, �����ת
void set_speed_mode_speed(int speed, MOTOR_DIRECTION_T motor_direction_t)
{
	if(speed == 0){
		set_motor_stop();
		recovery_motor_run_dir = MOTOR_IDLE;
		recover_speed = 0;
		return ;
	}
	motor_run_direction = 	motor_direction_t;
		
	motor_settings.forward_speed = speed;
	motor_settings.reverse_speed = -speed;
	update_settings(&motor_settings);
	start();
	
	recovery_motor_run_dir = motor_direction_t;
	recover_speed = speed;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-07-19
* �� �� ��: reset_motor_cycle
* ����˵��: �����ʼ��
*
* ��    ��: 
*
* �� �� ֵ:
*			����motor_cycle�ĳ�ʼֵ
*********************************************************************************************************
*/
void reset_cycle_to_bias(void)
{
	motor_cycle = CYCLE_BIAS;
}
