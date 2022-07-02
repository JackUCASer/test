#include "page_calib.h"

/*	外设依赖	*/
#include "lcd_display.h"
#include "get_voltage.h"		/*	电压：脚踏、电池、充电器	*/
#include "oil_cs1231.h"			/*	液压传感器	*/
#include "motor_misc.h"			/*	电机控制接口	*/

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	数据结构与算法层		*/
#include "movmean.h"			/*	滑动平均		*/
#include "lp_filter.h"			/*	低通滤波		*/

/*	APP&DATA	*/
#include "user_data.h"			/*	用户和系统数据	*/


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-14
* 函 数 名: foot_Pedal_Calibration
* 功能说明: 脚踏校正程序
* 形    参: 
*			无
* 说    明：
			
* 返 回 值: 
*********************************************************************************************************
*/
static void foot_Pedal_Calibration(void)
{
	static uint16_t footPedal_vol = 0;
	static uint16_t f_footPedal_vol = 0;
	static uint16_t max_footPedal_vol = 0;
	static uint16_t min_footPedal_vol = 3300;
	/*	1. UI提示用户在20s内完成脚踏校正:获取最小值和最大值	*/
	footPedal_vol = get_footpedal_voltage();
	f_footPedal_vol = movmean_filter(footPedal_vol);
	
	
	/*	2. 请将脚踏踩至最底部：获取电压最大值	*/
	max_footPedal_vol = (max_footPedal_vol >= f_footPedal_vol) ? max_footPedal_vol : f_footPedal_vol;
	
	/*	3. 请松开脚踏：获取电压最小值，实际上是获取稳态值	*/
	min_footPedal_vol = (min_footPedal_vol <= f_footPedal_vol) ? min_footPedal_vol : f_footPedal_vol;
	
	/*	4. 通知用户，脚踏数据校正完成	*/
	foot_pedal_t.MAX = max_footPedal_vol;
	foot_pedal_t.MIN = min_footPedal_vol;
	foot_pedal_t.QUARTER_DELTA = ((foot_pedal_t.MAX - foot_pedal_t.MIN) >> 2);		/*	取1/4	*/
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-29
* 函 数 名： check_reach_endpoint
* 功能说明： 检查电机是否到达端点
* 形    参: 
*			motor_dir_t: 电机运转方向
* 说    明：
			
* 返 回 值: 
*			电机是否处于达到端点
*********************************************************************************************************
*/
#define IQ_BUF_LEN	5
static int32_t f_iq_buf[IQ_BUF_LEN] = {0};	
static REACH_ENDPOINT_T check_reach_endpoint(MOTOR_DIRECTION_T motor_dir_t)
{

	static int32_t f_iq = 0;
	static uint32_t cnt = 0;
	static int32_t iq_temp = 0;
	iq_temp = iq;
	cnt ++;
	if(cnt > 10){
		cnt = 0;
		if(iq_temp > 140)
			iq_temp = 60;
		f_iq = movmean_filter2(iq_temp, f_iq_buf, IQ_BUF_LEN);
//		CALIB_PRINTF("f_iq = %d, and iq = %d\r\n", f_iq, iq);
	}
	
	switch(motor_dir_t){
		case MOTOR_FWD:{
			if(f_iq > 80){
				CALIB_PRINTF("MOTOR_FWD and f_iq is : %d\r\n",f_iq);
				return REACH_TOP;
			}		
		}break;
		
		case MOTOR_REV:{
			if(f_iq < -60){
				CALIB_PRINTF("MOTOR_REV and f_iq is : %d\r\n",f_iq);
				return REACH_BOTTOM;	
			}
		}break;
		
		case MOTOR_IDLE:
			break;
	}
	return KEEP_RUNNING;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-30
* 函 数 名： oil_Pressure_Calib_Motor_Para_Calucate
* 功能说明： 
*	1. 计算电机处于丝杆最底部时所处的位置[圈数，角度]，并将结果放在全局变量handle_injector_t.BOTTOM_LOC中
*	2. 计算电机处于丝杆最顶部时所处的位置[圈数，角度]，并将结果放在全局变量handle_injector_t.TOP_LOC中
*	3. 计算电机从丝杆最底部运行至丝杆最顶部的圈数（注意：仅圈数），并将结果放在全局变量handle_injector_t.DISTANCE中
* 形    参: 
*			无
* 说    明：
			
* 返 回 值: 
*********************************************************************************************************
*/
static void oil_Pressure_Calib_Motor_Para_Calucate(OIL_PRESS_CALIB_MOTOR_LOC_T* motor_loc_t)
{
	
	handle_injector_t.BOTTOM_LOC = motor_loc_t->REV_2nd_LOC;
	
	/*	2. 获取丝杆最顶部，电机圈数和角度寄存器对应的数值	*/
	handle_injector_t.TOP_LOC = motor_loc_t->FWD_1st_LOC;
	
	/*	3. 获取丝杆可执行总行程电机可执行圈数 DISTANCE	*/
	if(handle_injector_t.TOP_LOC > handle_injector_t.BOTTOM_LOC)
		handle_injector_t.DISTANCE = handle_injector_t.TOP_LOC - handle_injector_t.BOTTOM_LOC;
	else
		handle_injector_t.DISTANCE = (0xFFFFFFFF - handle_injector_t.BOTTOM_LOC) + handle_injector_t.TOP_LOC;
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-29
* 函 数 名： oil_Pressure_Calibration
* 功能说明： 液压校正程序
*			1. 行程的测量； 
*			2. 不同位置的液压（核心点：Full, 3/4, 2/4, 1/4, Empty）
* 形    参: 
*			无
* 说    明：
			
* 返 回 值: 
*********************************************************************************************************
*/
#define UP_CYCLE			40				/*	顶部回退圈数	*/
#define DOWN_CYCLE			160				/*	底部前进圈数:160	*/
#define UP_THRESHOLD		UP_CYCLE*360.0/m_gear_ratio
#define DOWM_THRESHOLD		DOWN_CYCLE*360.0/m_gear_ratio

#define FWD_SPD				injector_speed_t[1][1]	/*	校正过程中，电机正转速度FWD_SPD和反转速度REV_SPD	*/
#define REV_SPD				injector_speed_t[1][1]
static OIL_PRESS_CALIB_STATE_T oil_press_calib_state = CALIB_READY;
static OIL_PRESS_CALIB_STATE_T* oil_Pressure_Calibration(void)
{
	static REACH_ENDPOINT_T motor_reach_endpoint_t = KEEP_RUNNING;
	static OIL_PRESS_CALIB_MOTOR_LOC_T oil_press_calib_motor_loc_t = {0};
	
	switch(oil_press_calib_state)
	{
		case CALIB_READY:{
			set_speed_mode_speed(100);				/*	开始前进	*/
			osDelay(200);
			set_speed_mode_speed(FWD_SPD);			/*	开始前进	*/
			oil_press_calib_state = CALIB_FWD_1st;
		}break;
		
		case CALIB_FWD_1st:{
			motor_reach_endpoint_t = check_reach_endpoint(MOTOR_FWD);
			if(motor_reach_endpoint_t == REACH_TOP){
				set_motor_stop();
				osDelay(300);
				// 2. 记录顶部位置
				oil_press_calib_motor_loc_t.FWD_1st_LOC = (motor_cycle << 16) + motor_angle;
				set_speed_mode_speed(-100);
				osDelay(300);
				set_speed_mode_speed(-REV_SPD);		/*	开始回退	*/
				oil_press_calib_state = CALIB_REV_2nd;
			}
		}break;
	
		
		case CALIB_REV_2nd:{
			motor_reach_endpoint_t = check_reach_endpoint(MOTOR_REV);
			if(motor_reach_endpoint_t == REACH_BOTTOM){
				set_motor_stop();
				// 3. 记录底部位置
				oil_press_calib_motor_loc_t.REV_2nd_LOC = (motor_cycle << 16) + motor_angle;
				
				// 4. 计算手柄注射器行程: 先计算实际行程，再减去首尾UP_DOWM_CYCLE的行程
				oil_Pressure_Calib_Motor_Para_Calucate(&oil_press_calib_motor_loc_t);
				osDelay(500);
				uint32_t end_cycle = motor_cycle + DOWN_CYCLE;
				if(end_cycle > 65530)
					end_cycle = 0;
				set_speed_mode_speed(FWD_SPD);		/*	开始前进DOWN_CYCLE圈	*/
				while(1){
					if((motor_cycle&0x8000) == (end_cycle&0x8000)){
						if(motor_cycle >= end_cycle){
							set_motor_stop();
							break;
						}	
					}else
						osDelay(50);
				}
				handle_injector_t.BOTTOM_LOC = handle_injector_t.BOTTOM_LOC + (DOWN_CYCLE << 16);
				handle_injector_t.DISTANCE = handle_injector_t.DISTANCE - ((UP_CYCLE + DOWN_CYCLE) << 16);
				handle_injector_t.TOP_LOC = handle_injector_t.TOP_LOC - (UP_CYCLE << 16);
				
				oil_press_calib_state = CALIB_FINISH;
			}
		}break;
	
	
		case CALIB_FINISH:{
			
			motor_reach_endpoint_t = KEEP_RUNNING;	
		}break;
	}
	
//	CALIB_PRINTF("/*Cycle= %d, Angle= %3.1f, iq= %d,*/\n\r",motor_cycle, motor_angle/65536.0f*360, iq);
	return &oil_press_calib_state;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-10-26
* 函 数 名: TMY_Calibration
* 功能说明: 脚踏和液压校正程序，将校正的结果保存至单片机内部Flash中
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sPage_calib;
extern LCD_TOUCH_T 			lcd_touch_t;		/*	0x10 按键	*/	
extern uint16_t				usCurrent_page_id;	/*	当前返回数据所处于的界面ID	*/
void page_calibration(void)
{
	static bool keep_FP_calib = 0;				/*	保持脚踏校正	*/
	static bool keep_OP_calib = 0;				/*	保持液压校正	*/
	int8_t i8_result = 0;
	OIL_PRESS_CALIB_STATE_T*	oil_calib_state_t = NULL;
	uint32_t dynamic_pressure = 0;
	float32_t dynamic_pressure_mV = 0;
	float32_t dynamic_pressure_kPa = 0;

	if(xSemaphoreTake(sPage_calib, (TickType_t)0) == pdTRUE)
	{
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	清除按键历史	*/
		oil_press_calib_state = CALIB_READY;
		MP6570_AutoThetaBias(0x00, 200, 2000); 
		set_motor_run_in_speed_mode();
		keep_OP_calib = 1;
		CALIB_PRINTF("\r\n--------------------Now in Page Calibration--------------------\r\n");
		Init_adcIC();				/*	初始化CS1231：用于获取液压		*/
		for(;;){
			/*---------------------------------------1. 脚踏校正相关---------------------------------*/
			if((lcd_touch_t.page_id == PAGE_Calibration)&& lcd_touch_t.touch_wait_read && (lcd_touch_t.touch_id == PAGE_CALIB_PEDAL_ID)){
				if(lcd_touch_t.touch_state)
					keep_FP_calib = 1;
				else
					keep_FP_calib = 0;
				lcd_touch_t.touch_wait_read = 0;
			}	
			if(keep_FP_calib)
				foot_Pedal_Calibration();		/*	保持脚踏校正		*/
			
			/*---------------------------------------2. 液压校正相关---------------------------------*/
			if((lcd_touch_t.page_id == PAGE_Calibration)&& lcd_touch_t.touch_wait_read && (lcd_touch_t.touch_id == PAGE_CALIB_OILPS_ID)){
				if(lcd_touch_t.touch_state){
					keep_OP_calib = 1;
				}else
					keep_OP_calib = 0;
				lcd_touch_t.touch_wait_read = 0;
			}
			if(keep_OP_calib)					/*	检查是否到达完成状态	*/
				oil_calib_state_t = oil_Pressure_Calibration();	
					
			
			/*---------------------------------------3. 校正界面FINISH按键被按下----------------------*/
			if((lcd_touch_t.page_id == PAGE_Calibration)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_CALIB_FINISH_ID)){
				lcd_touch_t.touch_wait_read = 0;
				
				/*	3.1 记录脚踏数据	*/
				if(keep_FP_calib){				/*	脚踏校正过程中，完成按钮被按下，说明脚踏校正成功	*/
					i8_result = write_foot_pedal_t_data();	/*	保存数据至数据库	*/
					if(i8_result)
						CALIB_PRINTF("\r\ntmy_write_user_data  foot_pedal_t error!\r\n");
					else{
						CALIB_PRINTF("\r\nfoot_pedal_t.MAX           = %d\r\n", foot_pedal_t.MAX);
						CALIB_PRINTF("foot_pedal_t.MIN           = %d\r\n", foot_pedal_t.MIN);
						CALIB_PRINTF("foot_pedal_t.QUARTER_DELTA = %d\r\n", foot_pedal_t.QUARTER_DELTA);
					}
				}
				/*	3.2 记录液压数据	*/
				if(*oil_calib_state_t == CALIB_FINISH){		/*	仅当校正完成，点击Finish按钮才可保存数据	*/
					handle_injector_t.LAST_LOC = 0;					/*	校正完，上次位置清零	*/
					i8_result = write_handle_injector_data();		/*	保存数据至数据库，行程在oil_Pressure_Calibration()里更新	*/
					if(i8_result)
						CALIB_PRINTF("\r\ntmy_write_user_data  handle_injector_t error!\r\n");
					else{
						CALIB_PRINTF("\r\nhandle_injector_t.BOTTOM_LOC    = 0x%08x\r\n",handle_injector_t.BOTTOM_LOC);
						CALIB_PRINTF("handle_injector_t.TOP_LOC       = 0x%08x\r\n",handle_injector_t.TOP_LOC);
						CALIB_PRINTF("handle_injector_t.DISTANCE      = %d\r\n",(handle_injector_t.DISTANCE>>16));
						CALIB_PRINTF("hhandle_injector_t.LAST_LOC     = %d\r\n",(handle_injector_t.LAST_LOC>>16));
					}
				}
				*oil_calib_state_t = CALIB_READY;
				break;
			}
			/*	等待液压获取完成	*/
			if(get_oil_pressure(&dynamic_pressure) == New_Data_Read_Over){					
				dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);				// mV
				dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;
				add_single_line_data(0, (uint16_t)dynamic_pressure_kPa);
			}
			osDelay(10);
		}
		keep_FP_calib = 0;
		keep_OP_calib = 0;
		set_motor_stop();
		set_now_page(PAGE_Setting);
	}
}




