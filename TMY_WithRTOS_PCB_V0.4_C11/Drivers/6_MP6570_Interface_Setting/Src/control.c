#include "control.h"
#include "spi.h"
#include "mp6570.h"
//#include "delay.h"
#include "pid.h"
#include "tracking_filter.h"
#include "rc_filter.h"
#include "customer_control.h"
#include "common.h"
#include "macros.h"
#include "MP6570_SPI.h"
#include "motor_misc.h"

void update_status(void);
unsigned short error_led_times(unsigned char error_status);

#define CONTROL_MODE_SPEED  0
#define	CONTROL_MODE_POSITION	1
#define	CONTROL_MODE_TORQUE	2


u16 regi[128];		//Three Loop registers
u8 Mode = 0;//Mode 0: MP6570 Normal operating Mode; 1: INL Mode; 2:Three-Loop Mode
u8 control_mode;	//indicate control mode
u8 run = 0;
u8 brake = 0;
u8 update_command = 0;

/************************************
	error handler variables
************************************/
u8 error_status = 0;						//used for error status record, not used yet.
u16 error_temp = 0;

u8 errorStatusLatch = 0;
//long time over load protection
int ocp_cnt = 0;
unsigned char ocp_status = 0;
u16 iq_limit = 20;
u32 ocp_time_limit = 20000;
u8 over_load_en = 0;
u32 error_disp_cnt = 0;

//stall protection
u8 lock_flag = 0;
u32 stall_counter = 0;
int position_stall_last = 0;

//fault indication
u8 error_clock;
u8 error_led_state;

//retry control
u8 start_retry_cnt = 0;					//Fault retry counter start flag
u32 retry_cnt =0;								//retry counter value


/************************************
	position/speed/torque reference
************************************/
int position_ref = 0;
int speed_ref = 0; 
int iq_ref = 0;

//position feedback
u16 angle_raw = 0;
int angle, angle_last, position_last;					//current sensor angle and last sensor angle used to avoid large angle jump
int position = 0, position_temp = 0, position_temp_last = 0;								//motor position including rounds

/************************************
	loop controllers and filters
************************************/
PID pid_default = PID_DEFAULTS;
RC_TypeDef rc_default = RC_DEFAULTS;
TRACKING_TypeDef track_default = TRACKING_DEFAULTS;
TRACKING_TypeDef track_speed = TRACKING_DEFAULTS;				//speed tracking filter
TRACKING_TypeDef track_position = TRACKING_DEFAULTS;		//position tracking filter
RC_TypeDef rc_speed = RC_DEFAULTS;											//speed rc filter	
RC_TypeDef rc_speed_show = RC_DEFAULTS;								//speed rc filter used for speed show on GUI
PID pid_speed = PID_DEFAULTS;														//speed loop pid controller
PID pid_position = PID_DEFAULTS;												//position loop pid controller

/************************************
	s-curve
************************************/
int position_speed =0;
int position_acce = 10;
int v0;
long long int position_ref_e = 0;
int position_ref_e1 = 0;


/************************************
	Woodpecker control variables
************************************/

int iq = 0;
RC_TypeDef rc_iq = RC_DEFAULTS;								//speed rc filter used for speed show on GUI


void reset_parameters()
{
	control_mode = (regi[0x34] & 0x0003);
	//reset position feedback
	angle_raw = SPI_ByteRead(0x55); //read initial position back	
	angle = angle_raw;
	angle_last = angle_raw;
	position = angle_raw;	
	position_last = angle_raw;
	position_temp = angle_raw;
	position_temp_last = angle_raw;
	position_stall_last = angle_raw;
  
	//reset ref. in incremental mode with digital command
	position_ref = angle_raw;	
	//acce = 0x1F;//regi[0x1D];
	v0 = 0;
	//reset position tracking filter
	track_position = track_default;
	track_position.alpha = regi[0x40];
	track_position.beta = regi[0x41];
	track_position.position = angle_raw;   

	//reset velocity tracking filter 
	track_speed = track_default;
	track_speed.alpha = regi[0x42];
	track_speed.beta = regi[0x43];
	track_speed.position = angle_raw;

	//reset velocity rc filter
	rc_speed = rc_default;
	rc_speed.alpha = regi[0x44];
	//reset velocity rc filter for show on GUI
	//	rc_speed_show.out = 0;
	//	rc_speed_show.alpha = 4000;
  rc_iq.out = 0;
	rc_iq.alpha = 3276;
	//velocity PID controller
	pid_speed = pid_default;
	pid_speed.Kp = regi[0x22];
	pid_speed.Ki = regi[0x24];
	pid_speed.Kp_gain = regi[0x23];
	pid_speed.Ki_gain = regi[0x25];
	pid_speed.Kc = regi[0x26];
	pid_speed.pid_ref = 0;
	pid_speed.pid_fdb = 0;
	pid_speed.pid_out_max = regi[0x27];
	pid_speed.pid_out_min = -regi[0x27];


	//position PID controller
	pid_position = pid_default;
	pid_position.Kp = regi[0x1A];
	pid_position.Ki = 0;
	pid_position.pid_ref = angle_raw;
	pid_position.pid_fdb = angle_raw;
	pid_position.Kp_gain = regi[0x1B];
	pid_position.pid_out_max = (regi[0x1C]<<16);
	pid_position.pid_out_min = -(regi[0x1C]<<16);

	iq_limit = (regi[0x27]>>2)*3; //75% of the IQ_LIMIT
	ocp_time_limit = 10000*(((regi[0x55] & 0x0F00)>>8)+1);
	over_load_en = ((regi[0x55] & 0x1000)!=0);
}
void reset_ref_and_loop()
{	
	control_mode = (regi[0x34] & 0x0003);
	//reset position reference in PULSE mode	
	if((regi[0x34] & 0x000F) == 0x0009)
	{
		angle_last = angle;				
		position = angle;
		position_last = angle;
		position_temp = angle;
		position_temp_last = angle;
		position_stall_last = angle;

	//track_position = track_default;
		track_position.angle = angle;
		track_position.position = angle;
			
	//track_speed = track_default;
		track_speed.angle = angle;
		track_speed.position = angle;			
		
	  //position PID controller
	//pid_position = pid_default;			
		pid_position.pid_ref = position_ref;
		pid_position.pid_fdb = position_ref;			
	}
	else
	{
		position_ref = position;	

		pid_position.pid_ref = position;
		pid_position.pid_fdb = position;
	
	}
	
	//track position setting
	track_position.alpha = regi[0x40];
	track_position.beta = regi[0x41];

	//track speed setting
	track_speed.alpha = regi[0x42];
	track_speed.beta = regi[0x43];

	rc_speed.in = 0;
	rc_speed.alpha = regi[0x44];
	rc_speed.out = 0;

	//position PID setting
	pid_position.Kp = regi[0x1A];
	pid_position.Ki = 0;				
	pid_position.Kp_gain = regi[0x1B];
	pid_position.pid_out_max = (regi[0x1C]<<16);
	pid_position.pid_out_min = -(regi[0x1C]<<16);	
  pid_position.e = 0;
	
	//velocity PID controller
	pid_speed = pid_default;
	pid_speed.Kp = regi[0x22];
	pid_speed.Ki = regi[0x24];
	pid_speed.Kp_gain = regi[0x23];
	pid_speed.Ki_gain = regi[0x25];	
  pid_speed.Kc = regi[0x26];	
	pid_speed.pid_out_max = regi[0x27];
	pid_speed.pid_out_min = -regi[0x27];
	pid_speed.pid_ref = 0;
	pid_speed.pid_fdb = 0;
	pid_speed.e = 0;
}
u16 error_led_times(u8 error_status)
{
	u16 error_display = 0;
	if(error_status & 0x80)
		error_display = 0xFF;
	else if(error_status & 0x40)
		error_display = 0x7F;
	else if(error_status & 0x20)
		error_display = 0x3F;
	else if(error_status & 0x10)
		error_display = 0x1F;
	else if(error_status & 0x08)
		error_display = 0x0F;
	else if(error_status & 0x04)
		error_display = 0x07;
	else if(error_status & 0x02)
		error_display = 0x03;
	else if(error_status & 0x01)
		error_display = 0x01;
	return error_display;
}

void control_loop()
{
	static u16 error_display = 0x00;	
	static int info =0;
	static u8 counters = 0; 				  //counter for 100us timer interrupt
	int temp1,temp4;
	update_status();
	
	static uint16_t last_motor_cycle = 0;
	static uint16_t now_motor_cycle = 0;
	
	//read sensor angle for feedback
	angle_raw = SPI_ByteRead(0x55);	
	angle = angle_raw;
	
	//add turns value when value crosses 65535.
	temp1 = (int)angle - position_temp;
	if((temp1 & 0x8000) == 0x8000)
		temp1 = temp1 | 0x0000FFFF; 		//when value jump from 0 to 65535, add 65536 to value 
	else
		temp1 = temp1 & 0xFFFF0000; 		//when value jump from 65535 to 0, add -65536 to value  
	
	position_temp = angle - temp1;
  
	
	// angle estimation when large jump happens
	temp4 = position_temp - position_temp_last;
	temp4 = temp4 - (track_speed.velocity>>16);
	if((temp4 > 100)|| (temp4 < -100)) //100LSB<->0.55deg<->915rpm@100us
	{
		angle = angle_last + (track_speed.velocity>>16); //regi[0x32];		
		if(angle > 65535)
			angle = angle - 65536;
		if(angle < 0)
			angle = angle + 65536;
	}
	position_temp_last = position_temp;
	angle_last    = angle;	
	

	//add turns value when value crosses 65535.
	temp1 = (int)angle - position;
	if((temp1 & 0x8000) == 0x8000)
		temp1 = temp1 | 0x0000FFFF; //when value jump from 0 to 65535, add 65536 to value 
	else
		temp1 = temp1 & 0xFFFF0000; //when value jump from 65535 to 0, add -65536 to value  
	
	position = angle - temp1;
	
	if(regi[0x63] >= 0x0800)
		iq = regi[0x63]-0x0FFF;
	else
		iq = regi[0x63];
	rc_iq.in = iq;
	rc_iq.calc(&rc_iq);
	iq = rc_iq.out;
	customer_control();
	

	//position tracking filter  //3.28us
	track_position.angle = position;
	track_position.calc(&track_position);
	regi[0x5F] = track_position.position >> 16;
	regi[0x60] = track_position.position; //uppdate register of position filter value
	/*	ÐÂÔöÓÚ2022-04-27	*/
	last_motor_cycle = now_motor_cycle;
	now_motor_cycle = regi[0x5F];
	switch(motor_run_direction){
		case MOTOR_IDLE: break;
		case MOTOR_FWD:
			if(last_motor_cycle != now_motor_cycle)
				motor_cycle ++;
		break;
			
		case MOTOR_REV:
			if(last_motor_cycle != now_motor_cycle)
				motor_cycle --;
		break;
		
		default: break;
	}
	motor_angle = regi[0x60];
	
	//velocity tracking filter  //3.15us
	track_speed.angle = position;
	track_speed.calc(&track_speed);
	
	//velocity RC filter   2.14us 
	rc_speed.in = track_speed.velocity;
	rc_speed.calc(&rc_speed);
	regi[0x61] = rc_speed.out;  //update velocity registers
	regi[0x62] = rc_speed.out>>16;

	//Update reference
	//set different position reference at different mode  and also clear turns.
	if(control_mode == CONTROL_MODE_POSITION) //position mode
	{
			if(((regi[0x34] & 0x000C) == 0x0008)) //pusle input mode
			{
				if(track_position.position >= 0x3FFFFFFF)
				{
					//counter_position = counter_position - (0x3FFFFFFF>>NSTEP);
					track_position.position = track_position.position - 0x3FFFFFFF;
					track_speed.position = track_speed.position - 0x3FFFFFFF;
					position = position - 0x3FFFFFFF;
					pid_position.pid_ref = pid_position.pid_ref - 0x3FFFFFFF;
					stall_counter = 0;
					position_last = position;
				}
				if(track_position.position <= -0x3FFFFFFF)
				{			
					//counter_position = counter_position + (0x3FFFFFFF>>NSTEP);
					track_position.position = track_position.position + 0x3FFFFFFF;
					track_speed.position = track_speed.position + 0x3FFFFFFF;
					position = position + 0x3FFFFFFF;
					pid_position.pid_ref = pid_position.pid_ref + 0x3FFFFFFF;
					stall_counter = 0;
					position_last = position;
				}												
				//position_ref = (counter_position << NSTEP);
			}
			else //digital command
			{			
				if(update_command)
				{
					if((regi[0x34] & 0x0010) == 0x0000) // absolute mode 
					{
							position_ref = (regi[0x4A]<<16) + regi[0x4B] + (regi[0x48]<<16) + regi[0x49];					
					}
					else  //relative mode
					{
						position_ref = position_ref + (int)((regi[0x4A]<<16) + regi[0x4B]);								
					}				
				}
				
				if(track_position.position >= 0x3FFFFFFF)
				{
					position_ref = position_ref - 0x3FFFFFFF;
					track_position.position = track_position.position - (0x3FFFFFFF);
					track_speed.position = track_speed.position - (0x3FFFFFFF);
					position = position - (0x3FFFFFFF);
					pid_position.pid_ref = pid_position.pid_ref - (0x3FFFFFFF);
					stall_counter = 0;
					position_last = position;
				}
				if(track_position.position <= (-0x3FFFFFFF))
				{
					position_ref = position_ref + 0x3FFFFFFF;
					track_position.position = track_position.position+(0x3FFFFFFF);
					track_speed.position = track_speed.position+(0x3FFFFFFF);
					position = position + (0x3FFFFFFF);
					pid_position.pid_ref = pid_position.pid_ref + (0x3FFFFFFF);
					stall_counter = 0;
					position_last = position;
				}				
			}			
		update_command = 0;		
	}
	else //non-position mode
	{
		if(update_command && ((regi[0x34] & 0x000C) == 0x0000))//update speed reference
		{
			speed_ref = (regi[0x4D]<<16) + regi[0x4E]; 
			update_command = 0;			
		}
		if(track_position.position >= 0x03FFFFFF)
		{
			track_position.position = track_position.position - (0x03FFFFFF);
			track_speed.position = track_speed.position - (0x03FFFFFF);
			position = position - (0x03FFFFFF);
			stall_counter = 0;
			position_last = position_last - (0x03FFFFFF);
		}
		if(track_position.position <= -0x03FFFFFF)
		{
			track_position.position = track_position.position + (0x03FFFFFF);
			track_speed.position = track_speed.position + (0x03FFFFFF);
			position = position + (0x03FFFFFF);
			stall_counter = 0;
			position_last = position_last + 0x03FFFFFF;
		}
	}		
	//Check error status and retry and other informations 
	switch(info)
	{
		case 0:
			error_temp = SPI_ByteRead(0x53); //read 6570 fault register
			error_status = (ocp_status<<4) + lock_flag + (error_temp & 0x07);
			regi[0x53] = error_status & 0xff;
      error_display = error_led_times(error_status);
			break;
		case 1:
			regi[0x64] = SPI_ByteRead(0x69);//ID_LATCH
			break;
		case 2:
			regi[0x63] = SPI_ByteRead(0x6A);//IQ_LATCH
			break;
		case 3:
			regi[0x66] = SPI_ByteRead(0x6B);//UD_LATCH
			break;
		case 4:
			regi[0x65] = SPI_ByteRead(0x6C);//UQ_LATCH
			break;
		default:
			break;
	}
	
	info++;
	if(info>4)
		info = 0;
	//fault display on red led
	
	error_disp_cnt++;
	if(error_disp_cnt >(12<<12))
		error_disp_cnt = 0;
	error_clock = error_disp_cnt>>11;
	
	error_led_state = (error_display & (1<<(error_clock>>1)))&&(error_clock & 0x01);
//	if(error_led_state)
//		ERROR_SET;
//	else
//		ERROR_RESET;
	
 //fault happens
	if(error_status != 0 && start_retry_cnt == 0)  
	{
		errorStatusLatch = error_status;
		retry_cnt = 0;
		start_retry_cnt = 1;
		SPI_ByteWrite(0x11,0,0);
		SPI_ByteWrite(0x61,0,0); //turn off 6570
		SPI_ByteWrite(0x60,0,0); //turn on 6570 to clear fault flag in 6570
	  //reset_parameters();
	}
	
	//fault retry 
	if (retry_cnt >= 50*((regi[0x54] & 0x000F)+1))
	{
		error_status = 0;
		errorStatusLatch = 0;
		lock_flag = 0;
		ocp_status = 0;
		retry_cnt = 0;
		start_retry_cnt = 0;
		reset_parameters();
	}
	
	if(start_retry_cnt) //in fault retry
	{
		if((regi[0x54]&0x0010) == 0x0000 && !ocp_status) //if retry enable
			retry_cnt++;
	}
	else
	{
		if(run)
		{						
			//lock detection check in speed mode
			if(control_mode == CONTROL_MODE_SPEED)
			{
				if((regi[0x55] & 0x0010) && ((speed_ref > 17896) || (speed_ref < -17896))) //enable lock detection >20rpm
					stall_counter++;
				if(stall_counter >= (5000 * (regi[0x55]&0x000F) + 5000))
				{
					stall_counter = 0;
					if((position - position_stall_last < 910) && (position - position_stall_last > -910)) //less than 5deg in detection time
						lock_flag = 8;
					position_stall_last = position;
				}
			}
			
			//lock detection check in position mode
			if((control_mode == CONTROL_MODE_POSITION) && ((regi[0x34]&0x0008) == 0))
			{
				if (((pid_position.pid_fdb)- position_ref > regi[0x1E]) || ((pid_position.pid_fdb)- position_ref < -regi[0x1E])) //detect lock only when reach_target = 0
				{
					if(regi[0x55] & 0x0010)
						stall_counter++;
					if(stall_counter >=5000 * (regi[0x55]&0x000F) + 5000)
					{
						stall_counter = 0;
						if((position - position_stall_last < 910) && (position - position_stall_last > -910))
							lock_flag = 8;
						position_stall_last = position;
					}
				}
				else
				{
					stall_counter = 0;
				}
			}
			
			if(control_mode != CONTROL_MODE_TORQUE)
			{
				pid_position.pid_fdb = track_position.position;					
				pid_position.calc(&pid_position);		
				pid_speed.pid_fdb = rc_speed.out;
				if(control_mode == CONTROL_MODE_POSITION)
					pid_speed.pid_ref = pid_position.pid_out;	
				pid_speed.calc(&pid_speed);					
			}
			
			//used for reach position indication
			if (((pid_position.pid_fdb)- position_ref < regi[0x1E]) && ((pid_position.pid_fdb)- position_ref > -regi[0x1E]))
			{
				regi[0x69] = regi[0x69] | 0x01;
//				PEND_SET;
			}
			else
			{
				regi[0x69] = regi[0x69] & 0xFE;
//				PEND_RESET;
			}				
			//update reach speed signal
			if ((pid_speed.pid_fdb - speed_ref < (regi[0x29]<<16)) && (pid_speed.pid_fdb - speed_ref > -(regi[0x29]<<16)))
				regi[0x69] = regi[0x69]|0x02; //speed reach target
			else
				regi[0x69] = regi[0x69]&0xFD;
			
			//iq_ref update
			if (control_mode == CONTROL_MODE_TORQUE)
			{						
				if((regi[0x2F]<0x8000)) 																	//positive torque
				{								
					if(rc_speed.out > (regi[0x2D]<<16)) 													//max. speed limit 
						iq_ref = regi[0x2F] - (regi[0x2E]*((rc_speed.out>>16) - regi[0x2D]));
					else
						iq_ref = regi[0x2F];
					if(iq_ref<0)
						iq_ref =0;
				}
				else 
				{
					if((regi[0x2D]<<16) + rc_speed.out < 0)													//max. speed limit 
						iq_ref = ((int)regi[0x2F] - 65536) - (regi[0x2E]*((rc_speed.out>>16) + regi[0x2D]));
					else
						iq_ref = (int)regi[0x2F] - 65536;
					if(iq_ref>0)
						iq_ref = 0;
				}																
			}
			else
			{
					iq_ref = pid_speed.pid_out;
			}					

			//update iq_ref in brake mode
			if(regi[0x71]) //brake
			{
				if(regi[0x62] < 0x8000) //positive filter speed
				{
					if(regi[0x62] > regi[0x38]) //speed > min brake speed
						iq_ref = -regi[0x37];
					else
					{
						brake = 1;
					}
				}
				else//negative filter speed
				{
					if(((int)regi[0x62] - 65536) < -regi[0x38]) 						
						iq_ref = regi[0x37];
					else
					{
						brake = 1;
					}
				}
			}
			if(iq_ref == 0)
				iq_ref = 1;
			//Set current loop reference
			SPI_ByteWrite(0x11, (u8)(iq_ref>>8),(u8)iq_ref);
			
			//long time large current protection
			if((iq_ref > iq_limit) ||(iq_ref<-iq_limit))
				ocp_cnt++;
			else
				ocp_cnt = 0;
			if((ocp_cnt>ocp_time_limit)&& over_load_en)
			{
				ocp_status = 1;
			}
			//position and speed reference calculation and update according to the slope
			counters++;				//position_slope_counter++;
			if(counters >= 1) //100us step calculation for reference, maximum 1.8us
			{
				counters = 0;
				if(control_mode == CONTROL_MODE_SPEED)
				{
					if(speed_ref >= pid_speed.pid_ref)
					{
						pid_speed.pid_ref = pid_speed.pid_ref + regi[0x4F]; //speed slope, LSBs/100us/100us
						if(pid_speed.pid_ref >= speed_ref)
							pid_speed.pid_ref  = speed_ref;
					}
					else
					{
						pid_speed.pid_ref = pid_speed.pid_ref - regi[0x4F];//speed slope, LSBs/100us/100us
						if(pid_speed.pid_ref <= speed_ref)
							pid_speed.pid_ref  = speed_ref;
					}
				}
				else if(control_mode == CONTROL_MODE_POSITION)
				{
					position_ref_e = (long long int)((long long int)position_ref - (long long int)pid_position.pid_ref);
					if(position_ref_e >0x001FFFFF)
						position_ref_e1 = 0x001FFFFF;
					else if(position_ref_e < -0x001FFFFF)
						position_ref_e1 = -0x001FFFFF;
					else
						position_ref_e1 = position_ref_e;
					if (v0>=0)
					{
						if(v0*v0 < position_ref_e1*regi[0x1D]*32)
							position_acce = regi[0x1D];//acce;
						else
							position_acce = -regi[0x1D];//acce;
					}
					else
					{
						if(-v0*v0 < position_ref_e1*regi[0x1D]*32)
							position_acce = regi[0x1D];//acce;
						else
							position_acce = -regi[0x1D];//acce;
					}
					v0 = v0 + position_acce;
					position_speed = (v0>>4);
					if(position_speed > regi[0x4C])
					{
						position_speed = regi[0x4C];
						v0 = regi[0x4C]<<4;
					}
					if(position_speed <- regi[0x4C])
					{
						position_speed = -regi[0x4C];
						v0 = (-regi[0x4C])<<4;
					}
					pid_position.pid_ref = pid_position.pid_ref + position_speed;
				}
			}		
		}
		else //motor stop 
		{
			SPI_ByteWrite(0x11,0,0); //coasting off	
			v0 = 0;			
			stall_counter = 0;
			retry_cnt =0;				
		}
	}	
}

void update_status()
{
	u8 run_temp = 0;
	static u8 run_cnt = 0;
	if(run == 0)
	{
		regi[0x6A] = regi[0x6A] & 0xFFFE; //reset run indication bit
	}
	else
		regi[0x6A] = regi[0x6A] | 0x0001; //set run indication bit
		run_temp = (regi[0x70] && (!brake));
	
	if(run_temp == 1)
	{
		if(run_cnt == 0)
		{
			reset_ref_and_loop();
			run_cnt = 1;
		}
		run = 1;
	}
	else
	{
		run = 0;
		run_cnt = 0;
	}                                                                        
}
