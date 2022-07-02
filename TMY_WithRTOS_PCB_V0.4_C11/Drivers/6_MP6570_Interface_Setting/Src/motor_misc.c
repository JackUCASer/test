#include "motor_misc.h"

uint16_t motor_cycle = 0;
uint16_t motor_angle = 0;
MOTOR_DIRECTION_T motor_run_direction = MOTOR_IDLE;
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
	reset_parameters();
	LOOPTIMER_ENABLE;
	set_motor_run_in_speed_mode();
}

//	���õ���������ٶ�ģʽ
void set_motor_run_in_speed_mode(void)
{
	motor_settings.mode = 0;							//	�ٶ�ģʽ
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
}

// speed >0, �����ת�� speed <0, �����ת
void set_speed_mode_speed(int speed)
{
	if(speed > 0)
		motor_run_direction = MOTOR_FWD;
	else if(speed < 0)
		motor_run_direction = MOTOR_REV;
	
	motor_settings.forward_speed = speed;
	motor_settings.reverse_speed = -speed;
	update_settings(&motor_settings);
	start();
}
