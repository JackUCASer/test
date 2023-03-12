#include "customer_control.h"
#include "mp6570.h"
#include "control.h"
#include "common.h"
#include "key.h"
#include "main.h"
#include "datatype.h"
#include "macros.h"
#include "menudata.h"
#include "work_mode.h"
#include "beep.h"
extern WorkMode_e WorkMode;
uint16_t ceshi_6570;
unsigned short status_mp6570 = 0;

const unsigned char ratio = 6;
unsigned short forward_speed; 
unsigned short reverse_speed;
int	forward_position;
int reverse_position;
unsigned short upper_threshold;		//iq upper threshold
unsigned short lower_threshold;
//unsigned char autorev_mode;
u16 threshold_times = 3000;   //delay counter x * 100us
u16 threshold_times_H = 3000;   //delay counter x * 100us
u16 threshold_times_L = 3000;   //delay counter x * 100us
u16 reach_upper_times = 0;
u16 reach_lower_times = 0;
MotorStatus_TypeDef motor_status;
MotorSettings_TypeDef motor_settings;
unsigned int reciprocate_sw=0;

unsigned char torque_reach(void);
unsigned char toggle_torque_reach(void);
void set_torque_limit(float upper_limit, float lower_limit); //set torque limit in speed mode 
void set_speed(int forward, int reverse);//set forward and backward speed
void set_position(int forward, int reverse);//set forward and backward position
void set_toggle_mode_speed(int speed);
void mode_select(unsigned char mode);//set working mode. 0:speed mode, 1: position back and forth mode;
/*****************************set working mode********************************************
  * @brief  Set working mode.
  * @param  mode: 0:speed mode, 1: position back and forth mode;
  * @retval None
*****************************************************************************************/
void mode_select(unsigned char mode)
{
	//zhuomuniao_mode = mode;
	motor_status.mode = mode;
	if(mode == 1)
		regi[0x34] = 0x0131;
	else if(mode == 0)
		regi[0x34] = 0x0130;
	else if(mode == 2)
		regi[0x34] = 0x0130;
}

/*****************************start the motor********************************************
  * @brief  start the motor.
  * @param  None
  * @retval None
*****************************************************************************************/
void start()
{
	regi[0x70] = 0x0001;
	motor_status.status = 1;
}

/*****************************stop the motor********************************************
  * @brief  stop the motor.
  * @param  None
  * @retval None
*****************************************************************************************/
void stop()
{
	regi[0x70] = 0x0000;
	motor_status.status = 0;
}

/*****************************set torque limit in speed mode*****************************
  * @brief  set torque limit in speed mode.
  * @param  upper_limit: when the motor torque higher than this value, motor start to turn backward
	* @Param	lower_limit: when the motor torque lower than this value, motor start to turn forward
  * @retval None
*****************************************************************************************/
void set_torque_limit(float upper_limit, float lower_limit)
{
	if(lower_limit<0.5)
		lower_limit = 0.5;
	//upper_threshold = upper_limit * 56;
	//lower_threshold = lower_limit * 56;
	 upper_threshold = upper_limit ;
	 lower_threshold = lower_limit ;
}

void set_toggle_mode_speed(int speed)
{
	regi[0x4C] = (speed*7*ratio*13*25/12/24)>>6;
}
/*****************************set forward and backward speed******************************
  * @brief  set forward and backward speed
	* @param  forward: forward speed. 
	* @param  reverse: reverse speed. 
*****************************************************************************************/
void set_speed(int forward, int reverse)
{
	forward_speed = (forward * 7*ratio*13*25/12/24)>>6;
	reverse_speed = (reverse * 7*ratio*13*25/12/24)>>6;
}
/*****************************set forward and backward speed******************************
  * @brief  set speed ramp slope.
	* @param  rpmPerMs: speed slope(unit rpm/ms), range 1-91rpm/ms. 
  * @retval None
*****************************************************************************************/
void set_speed_slope(int rpmPerMs)
{
	regi[0x4F] = rpmPerMs*716;
}
/*****************************set forward and backward position******************************
  * @brief  set forward and backward position
	* @param  forward: forward position. 
	* @param  reverse: reverse position. 
  * @retval None
*****************************************************************************************/
void set_position(int forward, int reverse)
{
	//motor_status.forward_position = forward;
	//motor_status.reverse_position = reverse;
	forward_position = forward * 182 * ratio/12*13/24*25;
	reverse_position = reverse * 182 * ratio/12*13/24*25;
}

/*****************************customer control logic******************************
  * @brief  customer control logic function
						Note: Don't add delay in this function and any function called by this function should not
						has delay.
	* @param  None. 
  * @retval None
*****************************************************************************************/
uint16_t ATC_cnt = 0;
uint16_t IQ_Cnt = 0; 													//电流采样计数，防止因加速导致转矩误报
uint16_t ATC_REC_Cnt = 0; 													//电流采样计数，防止因加速导致转矩误报
void customer_control()
{
  // get_keys();														//handle key press logic, see the function definition for detail.
	motor_iq = iq;
	ceshi_6570 = MP6570_ReadRegister(0x00,0x11);
	status_mp6570 = MP6570_ReadRegister(0x00,0x54);
	motor_status.reach_target = regi[0x69] & 0x0001;  					//whether motor reach target position
	
	/**************根据模式获取扭矩********************************/
//	if(IQ_Cnt > 40)														//加速平稳后，再进行扭矩判断
//	{
		if(motor_settings.mode == 1)  										//如果是往复模式
		{
			motor_status.reach_torque = toggle_torque_reach();  			// 判断是否达到往复扭矩
		}
		else
		{
			motor_status.reach_torque = torque_reach();						//判断是否达到扭矩//whether motor reach target torque
		}
//	}
/******************************************************************/	
	if(motor_status.mode == 0) 											//如果正反转
	{
		if(sel.OperaMode == 3)
		{
			if(motor_status.reach_torque == 1)
			{
				stop();
				reciprocate_sw = 2;
			}	
		}
	}
	else if(motor_status.mode == 1) 											//如果是往复模式
	{
		if(sel.OperaMode == 2)
		{
			if(motor_status.reach_torque == 1)  							//已经到达了扭矩
			{ 
				stop();
				reciprocate_sw = 1;
			}
		}
		if(sel.OperaMode == 3)
		{
			if(motor_status.reach_torque == 1)
			{
				ATC_cnt = 0;
				ATC_REC_Cnt++;
			}else if(motor_status.reach_torque == 0)
			{
//				ATC_REC_Cnt++;
//				if(ATC_REC_Cnt>)
//				ATC_cnt++;
//				if(ATC_cnt > 1000)
//				{
//					stop();
//					reciprocate_sw = 3;	
//				}
			}else if(motor_status.reach_torque == 2)
			{
				ATC_cnt++;
				if(ATC_cnt > 1)
				{
					ATC_cnt = 0;
					stop();
					reciprocate_sw = 3;	
				}
			}
		}
	}


	//往复模式
 
 	if((motor_status.mode == 1) && motor_status.reach_target && (motor_status.status!=0)) 	//往复模式 & 位置到达 &不在停止状态
	{
			if(motor_status.status == 1)
				motor_status.status = Status_REVERSE;   									//启动时先设定电机为反转方向
			if(motor_status.status == Status_REVERSE)  										//如果电机反转方向
			{
				regi[0x4A] = (forward_position>>16); 										//position rounds
				regi[0x4B] = forward_position; 
				motor_status.status = Status_FORWARD; 	 									//更改电机方向为正转
			}
			else if(motor_status.status == Status_FORWARD)  								//如果电机正转方向
			{
				regi[0x4A] = reverse_position>>16;
				regi[0x4B] = reverse_position; 
				motor_status.status = Status_REVERSE;  										//更改电机方向为反转
			}
			update_command = 1;
	}	
	else if((motor_status.mode == 0) &&(motor_status.status != 0))							//速度模式
	{
		if(motor_status.status == Status_START)
		{
			if(motor_status.need_reverse == 0)
			{
				motor_status.status = 2;  													//正转
			}
			else
			{
				motor_status.status = 3;  													//反转
			}
		}
		else if(motor_status.status == 2) 													//正转
		{
			regi[0x4d] = forward_speed;  													//传递正转速度
			motor_status.status = Status_FORWARD;  
			update_command = 1;
//			if((motor_settings.autorev_mode ==2) && (motor_status.reach_torque == 1))  		//自动停止模式 & 扭矩到达
//				stop();
			if((motor_settings.autorev_mode == 0 || motor_settings.autorev_mode == 1)&& motor_status.reach_torque == 1) //到达扭矩反转 & 扭矩到达
			{
				motor_status.status = 3;  													//反转
			}
		}
		else if(motor_status.status == 3)
		{
			regi[0x4d] = reverse_speed;
			motor_status.status = Status_REVERSE;
			update_command = 1;
			if((motor_settings.autorev_mode == 0 || motor_settings.autorev_mode == 1) && motor_status.reach_torque == 2)
			{
				motor_status.status = 2;
			}
//			if(motor_settings.autorev_mode == 1 && motor_status.reach_torque == 2)
//				stop();
		}			
	}
}

void update_settings(MotorSettings_TypeDef *setting)
{
	mode_select(setting->mode);
	set_speed(setting->forward_speed, setting->reverse_speed);
	set_position(setting->forward_position, setting->reverse_position);
	set_torque_limit(setting->upper_threshold, setting->lower_threshold);
	set_toggle_mode_speed(setting->toggle_mode_speed);
}

unsigned char torque_reach()  													//根据电流判断扭矩状态
{
	threshold_times = 3000;
	u8 status = 0; 																// 1- reach_upper  2- reach_lower  0: middle-state
	if ((iq > upper_threshold)||(iq < -upper_threshold))
	{
		reach_upper_times ++;
		if(reach_upper_times >=threshold_times)
		{
			reach_upper_times = threshold_times;
			reach_lower_times = 0;
			status = 1; 
			sys.reach_flag=1;  													//通知扭矩到达设定
		}
	}
	if((iq < lower_threshold)&&(iq > -lower_threshold))
	{
		reach_lower_times ++;
		if(reach_lower_times >= threshold_times)
		{
			reach_lower_times = threshold_times;
			reach_upper_times = 0;
			status = 2;
		}
	}
	return status;
}

unsigned int position_cnt = 0;
int32_t 	position_iq_SUM = 0;
uint16_t	position_SUM_cnt = 0;
uint16_t	position_SUM_cnt_limit = 0;
uint16_t	position_cnt_limit = 0;
uint8_t		position_SUM_Flag = 0;
int ceshi[20];
unsigned int ceshi1 = 0;
unsigned char toggle_torque_reach()
{
	threshold_times_H = 5*sel.RecSpeed;								//超限次数与往复速度正相关
	threshold_times_L = sel.RecSpeed+1;											//超限次数与往复速度正相关
	threshold_times = 3500;
	static uint8_t i = 0;
	u8 status = 0; 							// 1- reach_upper  2- reach_lower  0: middle-state
	u8 check_torque = 0;
	if(((-motor_settings.forward_position > motor_settings.reverse_position)&&(motor_status.status == Status_FORWARD)) 
	|| ((-motor_settings.forward_position < motor_settings.reverse_position)&&(motor_status.status == Status_REVERSE))
	|| ((-motor_settings.forward_position == motor_settings.reverse_position)&&(motor_status.status == Status_FORWARD)))
	//if(((motor_settings.forward_position > - motor_settings.reverse_position)&&(motor_status.status == Status_FORWARD)) || ((motor_settings.forward_position < - motor_settings.reverse_position)&&(motor_status.status == Status_REVERSE)))
		check_torque = 1;
	else
		check_torque = 0;
	if(check_torque)
	{
		if(ceshi1 < position_SUM_cnt)
		{
			ceshi1 = position_SUM_cnt;
		}
		position_SUM_cnt++;
		if(position_SUM_cnt>250)
		{
			position_iq_SUM += iq;
		}
		position_SUM_Flag = 0;
	}else
	{
		if(position_SUM_Flag == 0)
		{
			position_iq_SUM = position_iq_SUM / (position_SUM_cnt-250);
			
			ceshi[i] = position_iq_SUM;
			i++;
			if(i >= 20)
			{
				i = 0;
			}
			
			if ((position_iq_SUM > upper_threshold)||(position_iq_SUM < -upper_threshold))
			{
				reach_upper_times ++;
				if(reach_upper_times >= threshold_times_H)
				{
					reach_upper_times = threshold_times_H;
					reach_lower_times = 0;
					status = 1; 
				}
			}
			if((position_iq_SUM < lower_threshold)&&(position_iq_SUM > -lower_threshold))
			{
				reach_lower_times ++;
				if(reach_lower_times >= threshold_times_L)
				{
					reach_lower_times = threshold_times_L;
					reach_upper_times = 0;
					status = 2;
				}
			}
			position_SUM_Flag = 1;
			position_SUM_cnt = 0;
			position_iq_SUM = 0;
		}
	}
	return status;
}

void init_registers()
{
	regi[0x00] = 0x6710; 			//Part Number
	regi[0x01] = m_Rs*500;//520;					//Phase resistor mOhm
	regi[0x02] = m_Lq; 					//Phase Inductor uH
	regi[0x03] = 20; 					//Power rating W
	regi[0x04] = m_J; 					// Inertia g*cm^2
	regi[0x05] = m_speed; 				//Nominal Speed rpm
	regi[0x06] = m_Vin;   				//Voltage V
	regi[0x07] = 8;  					//rated torque mNm
	regi[0x08] = m_p;    				//Pole Pairs p
	regi[0x09] = 5;   				//Maximum input voltage
	regi[0x0A] = 1;    				//Nominal current Arms
	regi[0x0B] = 3;   				//Peak current Arms
	regi[0x0C] = 220;  				//Input Capacitor uF
	regi[0x0D] = 10;   				//Sensing resistor mOhm
	regi[0x0E] = 7;    				//MOS Ron
	regi[0x0F] = 1;    				//interface information 0: RS485 1: RS485+PULSE
	regi[0x10] = 0;    				//Predriver-type 0:MP1907A
	regi[0x11] = 0x0101;			//PCB version
	regi[0x12] = 0;						//reserved	
	regi[0x13] = 0;     			//reserved
	regi[0x14] = 0x0051;			//Software version 
	regi[0x15] = 12;    			//resolution
	regi[0x16] = 100;   			//INL
	regi[0x17] = 20000; 			//maximum rpm
	regi[0x18] = 1;     			//minimum rpm
	regi[0x19] = 0;						//reserved
  	regi[0x1A] = KP_Position;//1500;//0x04F3;			//position loop Kp
  	regi[0x1B] =KP_Position_Gain;//0x000E;			//position loop Kp gain
 	regi[0x1C] = 0x156;			//position loop Maximum Limit
	regi[0x1D] = 0x001F;			//s-Curve accelaration rate
	regi[0x1E] = 3500;			//Position reach target threshold
	regi[0x1F] = 0;						//reserved
	regi[0x20] = 0;						//reserved
	regi[0x21] = 0;						//reserved
	regi[0x22] =  KP_Speed;//800;//0x08D2;			//Speed loop Kp
	regi[0x23] =  KP_Speed_Gain;//0x800C;			//Speed loop Kp gain
	regi[0x24] =  KI_Speed;//800;//0x0E30;			//Speed loop Ki
	regi[0x25] =  KI_Speed_Gain;//0x8014;			//Speed loop Ki gain
	regi[0x26] = 1000;				//Kc anti-integral gain
	regi[0x27] = 0x03FF;  		//Speed loop maximum limit
	regi[0x28] = 0;						//reserved
	regi[0x29] = 0x000A;			//Speed reach target threshold
	regi[0x2A] = 0;						//reserved
	regi[0x2B] = KP_Current;//300;//0x02A5;			//Current KP
  	regi[0x2C] =  KI_Current;//3000;//0x109A;			//Current Ki
	regi[0x2D] = 0x0064;			//high 16 HSB of maximum speed in torque mode, 
	regi[0x2E] = 10;					//Speed limit gain in torque mode
	regi[0x2F] = 0x0000;			//Torque reference IQ
	regi[0x30] = 0x0000;			//Torque reference ID
	regi[0x31] = 0;						//reserved
	regi[0x32] = 0;						//reserved
	regi[0x33] = 0x007F;			//reserved
	regi[0x34] = 0x0131; 			//Control Mode[0:1](00:speed, 01:position, 02:torque) reference Mode[2:3](00:digital, 10:Pulse, 11: PWM)
									//Position Mode[4](1:relative, 0:absolute) [5]: 1=standby mode; 0=non-standby   [7]: NSTEP set by switchs
	regi[0x35] = 0;			
	regi[0x36] = 0;						//reserved	
	regi[0x37] = 0;						//brake IQ current
	regi[0x38] = 0;						//min. brake speed
	regi[0x39] = 0;						//reserved
	regi[0x3A] = 0;						//reserved
	regi[0x3B] = 0;						//reserced
	regi[0x3C] = 0;						//reserved
	regi[0x3D] = 0;						//reserved
	regi[0x3E] = 0x00C8;			//I hold mA 
	regi[0x3F] = 1000;				//T hold ms
	
	regi[0x40] = 0x202B;			//position tracking filter alpha
	regi[0x41] = 0x0008;			//position tracking filter beta 
	regi[0x42] = 0x6082;			//speed tracking filter alpha
	regi[0x43] = 0x0018;			//speed tracking filter beta
	regi[0x44] = 0x6082;			//speed rc filter alpha
	regi[0x45] = 0;						//
	regi[0x46] = 0;						//
	regi[0x47] = 0;						//
	regi[0x48] = 0;						//
	regi[0x49] = 0;						//
	regi[0x4A] = 0x0000;			//poistion reference turns
	regi[0x4B] = 0x0000;			//position reference angle
	regi[0x4C] = 0x04B1;			//position reference slope LSB/100us
	regi[0x4D] = 0x00DA;			//Speed reference LSB/100us  high 16bits
	regi[0x4E] = 0x740D;			//Speed reference `` low 16bits
	regi[0x4F] = 0x3863;			//Speed reference slope
	
	regi[0x50] = 0x0007;			//reserved
	regi[0x51] = 0;						// reserved
	regi[0x52] = 0;						// reserved
	regi[0x53] = 0;						//fault status, [3]: lock, [2]: ocp, [1]: PSFT_6570, [0]: Memory fault_6570
	regi[0x54] = 0x0000;			//fault retry timer
	regi[0x55] = 0x0F01;			//stall detection timer
	regi[0x56] = 0x00F3;			//VDC protection
	regi[0x57] = 40;					//switching frequency kHz
	regi[0x58] = 32;					//Deadtime 12.5*x ns
	regi[0x59] = 7;						//AD_Gain
	regi[0x5A] = I_OCP;			//OCP current 
	regi[0x5B] = 0;						//reserved
	regi[0x5C] = 0x12;  			//reserved
	regi[0x5D] = 3600;  			//reserved
	regi[0x5E] = 0;     			//reserved
	regi[0x5F] = 0;     			//reserved
	
	regi[0x60] = 0;    				//sensor position after tracking filter
	regi[0x61] = 0;    				//sensor speed after tracking filter and rc filter low 16bits
	regi[0x62] = 0;    				//sensor speed after tracking filter and rc filter high 16bits
	regi[0x63] = 0;    				//IQ_LATCH
	regi[0x64] = 0;    				//ID_LATCH
	regi[0x65] = 0;    				//UQ_LATCH
	regi[0x66] = 0;    				//UD_LATCH
	regi[0x67] = 0x061D;    	//THETA_BIAS
	regi[0x68] = 0x0001;    	//THETA_DIR
	regi[0x69] = 0;    				//[0]:Reach position indication,Position; [1]:Reach speed indication.
	regi[0x6A] = 0;    				//system state, [4]: auto_aligning finished, [0]: 1=run 
	regi[0x6B] = 0;    				//
	regi[0x6C] = 0;    				//
	regi[0x6D] = 0;		 				//
	regi[0x6E] = 0;		 				//
	regi[0x6F] = 0;    				//

	
	regi[0x70] = 0;  					//EN/DISABLE  00: Disable 01: ENable
	regi[0x71] = 0;  					//BRAKE/RUN
	regi[0x72] = 0;  					//reserved
	regi[0x73] = 0;  					//reserved
	regi[0x74] = 0;  					//reserved
	regi[0x75] = 0;  					//reserved
	regi[0x76] = 0;  					//update command
	regi[0x77] = 0;  					//reserved
	regi[0x78] = 0;  					//reserved
	regi[0x79] = 0;  					//reserved
	regi[0x7A] = 0;  					//reserved
	regi[0x7B] = 0;  					//reserved
	regi[0x7C] = 0;  					//reserved
	regi[0x7D] = 0;  					//reserved
	regi[0x7E] = 0;  					//reserved
	regi[0x7F] = 0;  					//reserved
}
