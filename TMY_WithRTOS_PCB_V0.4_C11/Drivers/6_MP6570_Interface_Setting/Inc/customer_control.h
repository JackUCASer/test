#ifndef _CUSTOMER_CONTROL_H_
#define _CUSTOMER_CONTROL_H_

extern unsigned short status_mp6570;


typedef struct
{
	unsigned char status;  				//0:stop 1:start；2:forward 3:reverse
	unsigned char mode;					//0:toggle position mode  1:auto_reverse speed mode 
										//2:reverse_one speedmode 3:no reverse mode
	unsigned char need_reverse; 		//0:no need to reverse  1:need to revese
	unsigned char reach_target;
	unsigned char reach_torque;

	
	unsigned char delay_cnt;		//delay counter
} MotorStatus_TypeDef;

typedef struct
{
	unsigned char mode;					//0:toggle position mode  1:auto_reverse speed mode 
										//2:reverse_one speed mode 3:no reverse mode
	unsigned char autorev_mode; 		//0:达到转矩后反转，泄力后正转 1：达到转矩反转，泄力后停止 2：达到转矩就停止
	int forward_speed; 
	int reverse_speed;
	int	forward_position;
	int reverse_position;
	float upper_threshold;				//iq upper threshold
	float lower_threshold;
	int toggle_mode_speed;
	
} MotorSettings_TypeDef;

typedef enum
{ Status_STOP = 0x0,
  Status_START = 0x01,
  Status_FORWARD = 0x02,
  Status_REVERSE = 0x03,
}Status_TypeDef;

extern MotorSettings_TypeDef motor_settings;
extern MotorStatus_TypeDef motor_status;

void start(void); //start the motr
void stop(void);  //stop the motor
void set_speed_slope(int rpmPerMs);

void init_registers(void);//init register values
void customer_control(void);//customer control code, this function will be called by the 100us interrupt 

void update_settings(MotorSettings_TypeDef *setting);
#endif
