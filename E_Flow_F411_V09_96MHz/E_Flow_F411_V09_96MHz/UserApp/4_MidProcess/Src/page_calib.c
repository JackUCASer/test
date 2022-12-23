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
	footPedal_vol = Get_Foot_Pedal_Vol();
	f_footPedal_vol = Movmean_Filter(footPedal_vol);
	
	
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
* 日    期：2022-07-12
* 函 数 名： check_reach_endpoint
* 功能说明： 检查电机是否到达端点
* 形    参: 
*			motor_dir_t: 电机运转方向
* 说    明：
			
* 返 回 值: 
*			电机是否处于达到端点
*********************************************************************************************************
*/	
static REACH_ENDPOINT_T check_reach_endpoint(MOTOR_DIRECTION_T motor_dir_t)
{
//	CALIB_PRINTF("motor_cycle = %d, motor_to_origin_cycle = %d\r\n", motor_cycle, motor_to_origin_cycle);
	CALIB_PRINTF("motor_cycle = %d, oil_pressure = %d\r\n", motor_cycle, (uint16_t)dynamic_pressure_kPa);
	switch(motor_dir_t){
		case MOTOR_CALIB_TOP: return REACH_TOP;		
		
		case MOTOR_CALIB_BOTTOM: return REACH_BOTTOM;
		
		case MOTOR_CALIB_ORIGIN: return REACH_ORIGIN;
		
		default: return KEEP_RUNNING;
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.1
* 日    期：2022-08-29
* 函 数 名： oil_Pressure_Calib_Motor_Para_Calucate
* 功能说明： 
*	1. 计算电机处于丝杆最底部时所处的位置[圈数，角度]，并将结果放在全局变量handle_injector_t.ORIGIN_LOC中
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
	
	handle_injector_t.ORIGIN_LOC = motor_loc_t->REV_2nd_LOC;
	
	/*	2. 获取丝杆最顶部，电机圈数和角度寄存器对应的数值	*/
	handle_injector_t.TOP_LOC = motor_loc_t->FWD_1st_LOC;
	
	/*	3. 获取丝杆可执行总行程电机可执行圈数 DISTANCE	*/
	handle_injector_t.DISTANCE = handle_injector_t.TOP_LOC - handle_injector_t.ORIGIN_LOC;				//	校正时顶端到底端整个行程
	handle_injector_t.BOTTM2ORIGIN = handle_injector_t.DISTANCE - ((MAX_DIS_CYCLES + UP_CYCLE) << 16);	//	底部到原点的距离
	
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-29
* 函 数 名： oil_Pressure_Calibration
* 功能说明： 液压行程校正程序
*			Step 1: 电机前进至顶端； 
*			Step 2: 电机回退至底端；
*			Step 3: 电机前进至原点；
*			Step 4: 计算行程校正结果并保存，重置电机位置参考。
* 形    参：
*			
* 说    明：
*			
* 返 回 值：
*			OIL_PRESS_CALIB_STATE_T* 类型的校正步骤（状态）。
*********************************************************************************************************
*/
#define FWD_SPD				injector_speed_t[3][2]	/*	校正过程中，电机正转速度	*/
#define REV_SPD				injector_speed_t[3][2]	/*	校正过程中，电机反转速度	*/
#define REV_SPD_LOW			injector_speed_t[2][0]	/*	校正过程中，电机反转低速	*/
#define CHG_REV_SPD_TIME	20						/*	改变反转速度的等待时间	*/

static OIL_PRESS_CALIB_STATE_T oil_press_calib_state = CALIB_READY;

static OIL_PRESS_CALIB_STATE_T* oil_Pressure_Calibration(void)
{
	static REACH_ENDPOINT_T motor_reach_endpoint_t = KEEP_RUNNING;
	static OIL_PRESS_CALIB_MOTOR_LOC_T oil_press_calib_motor_loc_t = {0};
	static uint8_t delay_10ms_cnt = 0;
	
	switch(oil_press_calib_state)
	{
		case CALIB_READY:{
				//	1. 启动电机正转
			set_speed_mode_speed(100, MOTOR_CALIB_FWD);					/*	1）、首先低速前进	*/
			osDelay(200);												/*	2）、延迟200ms	*/
			set_speed_mode_speed(FWD_SPD, MOTOR_CALIB_FWD);				/*	3）、然后以正常速度前进	*/
			oil_press_calib_state = CALIB_FWD_1st;
		}break;
		
		case CALIB_FWD_1st:{
			motor_reach_endpoint_t = check_reach_endpoint(motor_run_direction);
			if(motor_reach_endpoint_t == REACH_TOP){
				oil_press_calib_state = CALIB_FWD_IDLE;
				delay_10ms_cnt = 0;
			}
		}break;
		
		case CALIB_FWD_IDLE:{
			delay_10ms_cnt ++;
			check_reach_endpoint(motor_run_direction);
			if(delay_10ms_cnt > 30){	//	延迟300ms
				// 2. 记录顶部位置
				motor_cycle = TOP_REF_CYCLE;
				oil_press_calib_motor_loc_t.FWD_1st_LOC = (motor_cycle << 16) + motor_angle;
				set_speed_mode_speed(-REV_SPD_LOW, MOTOR_CALIB_REV);	/*	1）、首先低速回退	*/
				oil_press_calib_state = CALIB_FWD_IDLE2;
				delay_10ms_cnt = 0;
			}
		}break;
		
		case CALIB_FWD_IDLE2:{
			delay_10ms_cnt ++;
			check_reach_endpoint(motor_run_direction);
			if(delay_10ms_cnt > 100){	//	延迟1s
				set_speed_mode_speed(-REV_SPD, MOTOR_CALIB_REV);		/*	2）、然后以正常速度回退	*/
				oil_press_calib_state = CALIB_REV_2nd;
				delay_10ms_cnt = 0;
			}
		}break;

		case CALIB_REV_2nd:{
			motor_reach_endpoint_t = check_reach_endpoint(motor_run_direction);
			if(motor_reach_endpoint_t == REACH_BOTTOM){
				oil_press_calib_state = CALIB_REV_IDLE;
				delay_10ms_cnt = 0;
//				osDelay(300);
//				// 3. 记录底部位置
//				oil_press_calib_motor_loc_t.REV_2nd_LOC = (motor_cycle <<16) + motor_angle;
//				
//				// 4. 计算手柄注射器行程: 
//				oil_Pressure_Calib_Motor_Para_Calucate(&oil_press_calib_motor_loc_t);
//				motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
//				set_speed_mode_speed(FWD_SPD, MOTOR_CALIB_TO_ORIGIN);	/*	开始前进至原点Origin	*/
//				oil_press_calib_state = CALIB_FWD_3rd;
			}
		}break;
		
		case CALIB_REV_IDLE:{
			delay_10ms_cnt ++;
			check_reach_endpoint(motor_run_direction);
			if(delay_10ms_cnt > 30){
				// 3. 记录底部位置
				oil_press_calib_motor_loc_t.REV_2nd_LOC = (motor_cycle <<16) + motor_angle;
				
				// 4. 计算手柄注射器行程: 
				oil_Pressure_Calib_Motor_Para_Calucate(&oil_press_calib_motor_loc_t);
				motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
				set_speed_mode_speed(FWD_SPD, MOTOR_CALIB_TO_ORIGIN);	/*	开始前进至原点Origin	*/
				oil_press_calib_state = CALIB_FWD_3rd;
			}
		}break;
		
		case CALIB_FWD_3rd:
			motor_reach_endpoint_t = check_reach_endpoint(motor_run_direction);
			if(motor_reach_endpoint_t == REACH_ORIGIN){
				handle_injector_t.DISTANCE = (MAX_DIS_CYCLES << 16);
				Cycle_Reset_To_Origin();
				oil_press_calib_state = CALIB_FINISH;
			}
		break;
	
		case CALIB_FINISH:{
			motor_reach_endpoint_t = KEEP_RUNNING;	
		}break;
	}
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

void PAGE_Calibration(void)
{
	static bool keep_FP_calib = 0;				/*	保持脚踏校正	*/
	static bool keep_OP_calib = 0;				/*	保持液压校正	*/
	static uint16_t footPedal_vol = 0;
	static uint16_t u16_f_fp_vol = 0;
	int8_t i8_result = 0;
	uint8_t cnt = 0;
	OIL_PRESS_CALIB_STATE_T*	oil_calib_state_t = NULL;

	if(xSemaphoreTake(sPage_calib, (TickType_t)0) == pdTRUE)
	{
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	清除按键历史	*/
		oil_press_calib_state = CALIB_READY;
		motor_run_direction = MOTOR_IDLE;					/*	更新电机状态为：IDLE模式	*/
		
		if(now_display_page == PAGE_MotorCalib){			//	当前处于电机校正界面
			MP6570_AutoThetaBias(0x00, 200, 2000); 
			set_motor_run_in_speed_mode();
			keep_OP_calib = 1;
			CALIB_PRINTF("\r\n--------------------Now in Page Calibration--------------------\r\n");
			Init_ADC_IC();				/*	初始化CS1231：用于获取液压		*/
			for(;;){
				/*---------------------------------------1. 液压校正相关---------------------------------*/
				if(keep_OP_calib)					/*	检查是否到达完成状态	*/
					oil_calib_state_t = oil_Pressure_Calibration();
				
				/*---------------------------------------2. 校正界面FINISH按键被按下----------------------*/
				if((lcd_touch_t.page_id == PAGE_MotorCalib)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_CALIB_FINISH_ID)){
					lcd_touch_t.touch_wait_read = 0;
					
					/*	记录液压数据	*/
					if(*oil_calib_state_t == CALIB_FINISH){		/*	仅当校正完成，点击Finish按钮才可保存数据	*/
						i8_result = Write_Handle_Injector_Data();	/*	保存数据至数据库，行程在oil_Pressure_Calibration()里更新	*/
						if(i8_result)
							CALIB_PRINTF("\r\ntmy_write_user_data  handle_injector_t error!\r\n");
						else{
							CALIB_PRINTF("\r\n--ORIGIN_LOC= 0x%08x,\r\n--TOP_LOC= 0x%08x,\r\n--DISTANCE= %d,\r\n--LAST_LOC= %d,\r\n--BOTTM2ORIGIN= %d\r\n",\
								handle_injector_t.ORIGIN_LOC, handle_injector_t.TOP_LOC,\
								(handle_injector_t.DISTANCE>>16), (handle_injector_t.LAST_LOC>>16), \
								(handle_injector_t.BOTTM2ORIGIN>>16));
						}
					}
					
					*oil_calib_state_t = CALIB_READY;
					Clear_Lcd_Cmd_SendQueue();
					break;
				}
					/*	等待液压获取完成	*/
				if(Get_Oil_Pressure(&dynamic_pressure) == New_Data_Read_Over){					
					dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);				// mV
					dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;
					if(++cnt > 8){
						Add_Single_Line_Data(0, (uint16_t)dynamic_pressure_kPa);
						cnt = 0;
					}
				}
				osDelay(10);
			}
			keep_OP_calib = 0;
			set_motor_stop();
			
		}else if(now_display_page == PAGE_FP_CALIB){
			for(;;){
				/*---------------------------------------1. 脚踏校正相关---------------------------------*/
				if((lcd_touch_t.page_id == PAGE_FP_CALIB)&& lcd_touch_t.touch_wait_read && (lcd_touch_t.touch_id == PAGE_CALIB_PEDAL_ID)){
//					vgus_page_objs[PAGE_MotorCalib].obj_value[0] = lcd_touch_t.touch_state;	//	记录按钮控件值
					lcd_touch_t.touch_wait_read = 0;
					if(lcd_touch_t.touch_state)
						keep_FP_calib = 1;
					else
						keep_FP_calib = 0;	
				}	
				if(keep_FP_calib)
					foot_Pedal_Calibration();		/*	保持脚踏校正		*/
				
				/*---------------------------------------2. 校正界面FINISH按键被按下----------------------*/
				if((lcd_touch_t.page_id == PAGE_FP_CALIB)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_PEDAL_CALIB_FINISH_ID)){
					lcd_touch_t.touch_wait_read = 0;
					/*	记录脚踏数据	*/
					if(keep_FP_calib){				/*	脚踏校正过程中，完成按钮被按下，说明脚踏校正成功	*/
						i8_result = Write_Foot_Pedal_Data();	/*	保存数据至数据库	*/
						if(i8_result)
							CALIB_PRINTF("\r\ntmy_write_user_data  foot_pedal_t error!\r\n");
						else{
							CALIB_PRINTF("\r\n--foot_pedal_t.MAX= %d,\r\n--foot_pedal_t.MIN= %d,\r\n--foot_pedal_t.QUARTER_DELTA= %d\r\n",\
											foot_pedal_t.MAX, 		foot_pedal_t.MIN, 			foot_pedal_t.QUARTER_DELTA);
						}
					}
					Clear_Lcd_Cmd_SendQueue();
					break;
				}else if((lcd_touch_t.page_id == PAGE_FP_CALIB)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_PEDAL_CALIB_CANCEL_ID)){
					lcd_touch_t.touch_wait_read = 0;
					Clear_Lcd_Cmd_SendQueue();
					break;
				}
					/*	显示脚踏压力曲线和具体数值	*/
				if(++cnt > 5){
					footPedal_vol = Get_Foot_Pedal_Vol();
					u16_f_fp_vol = Movmean_Filter(footPedal_vol);
					Add_Single_Line_Data(2, u16_f_fp_vol);
					Show_uint16_t_Data(0x0D04, footPedal_vol);
					cnt = 0;
				}
				osDelay(10);
				
			}
			keep_FP_calib = 0;
			
		}else
			;
		Set_VGUS_Page(PAGE_Setting);
	}
}




