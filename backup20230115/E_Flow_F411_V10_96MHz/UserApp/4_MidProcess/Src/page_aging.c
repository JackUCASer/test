#include "Page_Aging.h"
#include "lcd_display.h"		/*	与显示相关的全局设置	*/

/*	外设依赖	*/
#include "get_voltage.h"		/*	电压：脚踏、电池、充电器	*/
#include "oil_cs1231.h"			/*	液压传感器	*/
#include "motor_misc.h"			/*	电机控制接口	*/

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	数据结构与算法层		*/
#include "lp_filter.h"			/*	低通滤波		*/
#include "movmean.h"			/*	滑动平均		*/
#include <string.h>
#include "Hampel_Filter.h"
#include "Pressure_Origin_Search.h"

/*	APP&DATA	*/
#include "user_data.h"			/*	用户和系统数据	*/

//	函数原型声明
static AGING_M_CHANGE_DIR_T agig_position_compare(MOTOR_DIRECTION_T motor_dir_t);
static AGING_MODE get_page_mode_bottom_val(LCD_TOUCH_T * touch_t);
static void show_remain_dose(void);


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-11-08
* 函 数 名: TMY_AGing
* 功能说明: 推麻仪老化程序
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
#define FWD_SPD				injector_speed_t[3][2]
#define REV_SPD				injector_speed_t[3][2]
#define DELAY_ONE_MIN		230							//	一个老化周期时长 = 1分钟时对应的再延迟
extern SemaphoreHandle_t	sPage_aging;				/*	二进制信号量：通知程序进入老化程序	*/
extern LCD_TOUCH_T lcd_touch_t;	

void Page_Aging(void)
{
	AGING_MODE aging_mode_t = AGING_IDLE;							/*	老化状态	*/
	AGING_M_CHANGE_DIR_T agig_m_change_dir_t = AGING_M_NEED_IDLE;	/*	老化过程中电机需要调整的状态	*/
	
	static uint16_t footPedal_vol = 0;
	static uint16_t u16_f_fp_vol = 0;
	static uint16_t cnt,cnt2,cnt3 = 0;
	static uint32_t fre_cnt = 0;
	static bool get_origin_kPa_flag = false;
	static uint16_t origin_kPa = 0;

	if(xSemaphoreTake(sPage_aging, (TickType_t)0) == pdTRUE){
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));						/*	清除按键历史	*/				
		Show_uint32_t_Data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);	/*	显示已老化的圈数	*/
		
		AGING_PRINTF("\r\n--------------------Now in Page Aging--------------------\r\n");
		for(;;){
			aging_mode_t = get_page_mode_bottom_val(&lcd_touch_t);
			
			if(++cnt > 20){
				cnt = 0;	
				Show_uint32_t_Data(PAGE_AGING_COMM_ID, ++cnt2);				/*	测试通信	*/
			}
			
			switch(aging_mode_t){	
				case AGING_IDLE:
				break;
				
				case AGING_START:
					Init_ADC_IC();
					set_speed_mode_speed(FWD_SPD,MOTOR_AGING_FWD);
					aging_mode_t = AGING_IDLE;
				break;
				
				case AGING_STOP:
					set_motor_stop();
					Standby_ADC_IC();
				break;
				
				case AGING_FINISH:
					aging_mode_t = EXIT_AGING;
				break;
				
				default: break;
			}
			
			if(aging_mode_t == EXIT_AGING){
				aging_mode_t = AGING_IDLE;
				agig_m_change_dir_t = AGING_M_NEED_IDLE;
				break;
			}
			
			/*	1. 电机运转方向切换状态机	*/
			agig_m_change_dir_t = agig_position_compare(motor_run_direction);
			
			switch(agig_m_change_dir_t){
				case AGING_M_NEED_IDLE:
					cnt3 = 0;
				break;
				
				case AGING_M_NEED_FWD:
					if(++cnt3 > DELAY_ONE_MIN){
						if(sys_bat_too_low == false){
							get_origin_kPa_flag = true;				/*	通知记录原点处的液压	*/
							sys_para_t.AGING_CYCLE ++;
							if(sys_para_t.AGING_CYCLE % 50 == 0)	/*	更新Flash中的老化圈数	*/
								Write_Sys_Para_Data();										
							Show_uint32_t_Data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);
							set_speed_mode_speed(FWD_SPD, MOTOR_AGING_FWD);
							agig_m_change_dir_t = AGING_M_NEED_IDLE;
						}else	//	等待低电量保护机制解除
							cnt3 = DELAY_ONE_MIN;
					}else
						;
				break;
				
				case AGING_M_NEED_REV:
					if(++cnt3 > DELAY_ONE_MIN){
						set_speed_mode_speed(-REV_SPD, MOTOR_AGING_REV);
						agig_m_change_dir_t = AGING_M_NEED_IDLE;
					}else
						;
				break;
				
				default: break;
			}
			
			/*	等待液压获取完成	*/
			if(Get_Oil_Pressure(&dynamic_pressure) == New_Data_Read_Over){					
				dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
				dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa 
				footPedal_vol = Get_Foot_Pedal_Vol();
				u16_f_fp_vol = Movmean_Filter(footPedal_vol);
				show_remain_dose();
				dynamic_pressure_kPa_F = Hampel_Filter((uint16_t)dynamic_pressure_kPa, 3);
//				AGING_PRINTF("%d,%d\r\n", (uint16_t)dynamic_pressure_kPa, dynamic_pressure_kPa_F);		/*	查看hampel滤波相位延迟：3个样本点*/
				if(get_origin_kPa_flag == true){
					origin_kPa = dynamic_pressure_kPa_F;
					get_origin_kPa_flag = false;
				}
				if(origin_kPa != 0){
					dynamic_pressure_kPa_F = ((dynamic_pressure_kPa_F > origin_kPa) ? (dynamic_pressure_kPa_F - origin_kPa) : 0);
				}
				Add_Two_Line_Data(1, dynamic_pressure_kPa_F, 2, u16_f_fp_vol);
				Show_uint16_t_Data(PAGE_AGING_kPa_ID, dynamic_pressure_kPa_F);
				Show_uint16_t_Data(PAGE_AGING_mV_ID, footPedal_vol);
				fre_cnt = 0;
			}else{
				if(++fre_cnt > 10){
					Standby_ADC_IC();
					osDelay(100);
					Init_ADC_IC();
					osDelay(100);
					fre_cnt = 0;
				}else
					;
			}
			osDelay(50);
			IDLE_TIME_CNT = 0;
		}
		set_motor_stop();
		Standby_ADC_IC();
		Set_VGUS_Page(PAGE_Setting, true);
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-07-12
* 函 数 名: TMY_AGing
* 功能说明: 推麻仪老化程序中位置对比
*
* 形    参: 
*			motor_dir_t: 当前电机的方向
      
* 返 回 值:
*			AGING_M_CHANGE_DIR_T 电机是否需要切换方向
*********************************************************************************************************
*/
static AGING_M_CHANGE_DIR_T agig_position_compare(MOTOR_DIRECTION_T motor_dir_t)
{
//	AGING_PRINTF("/*%d,%d,%d,%d,%d,%d,%d,%d*/\r\n", \
//	(handle_injector_t.TOP_LOC >>16), 		(handle_injector_t.ORIGIN_LOC >>16), \
//	(Get_Current_Handle_Injector_Pos() >>16), 	Get_Current_Handle_Run_Distance(Get_Current_Handle_Injector_Pos() >>16), \
//	(uint16_t)dynamic_pressure_kPa, 		dynamic_pressure_kPa_F, movemean_iq, error_status);
	
	switch(motor_dir_t){
		case MOTOR_AGING_TOP:
			return AGING_M_NEED_REV;		
		
		case MOTOR_AGING_ORIGIN: 
			Cycle_Reset_To_Origin();	//	位置重置
			handle_in_origin = false;
			return AGING_M_NEED_FWD;
		
		case MOTOR_AGING_BOTTOM:
			osDelay(600);
			motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
			set_speed_mode_speed(FWD_SPD, MOTOR_AGING_TO_ORIGIN);
			return AGING_M_NEED_IDLE;
		
		default: 
			return AGING_M_NEED_IDLE;
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-11-08
* 函 数 名: get_page_mode_bottom_val
* 功能说明: aging页面下按键获取
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern uint16_t usCurrent_page_id;
static AGING_MODE get_page_mode_bottom_val(LCD_TOUCH_T * touch_t)
{	
	if((touch_t->page_id == PAGE_Aging) && (touch_t->touch_wait_read))	/*	如果是Aging界面的按钮被按下,但是还未读取	*/
	{
		touch_t->touch_wait_read = 0;		/*	表示按钮已被读取	*/
		switch(touch_t->touch_id){
			case PAGE_AGING_START_ID:		/*	老化开始按钮	*/
				vgus_page_objs[PAGE_Aging].obj_value[0] = touch_t->touch_state;	//	记录按钮控件值
				if(touch_t->touch_state)
					return AGING_START;
				else
					return AGING_STOP;
				
			
			case PAGE_AGING_FINISH_ID:		/*	老化完成按钮	*/
				return AGING_FINISH;

			default: break;
		}
	}
	return AGING_RUNING;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-03-15
* 函 数 名: show_remain_dose
* 功能说明: 显示剩余药量
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void show_remain_dose(void)
{
	static uint16_t current_pos = 0;
	static uint8_t last_dose = 0;
	static uint8_t now_dose = 0;
	
	if(wait_send_over == 1)
		return ;
	
	last_dose = now_dose; 
	current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	now_dose = Compute_Reamin_Dose(current_pos) / 3;	/*	将100 以 3 等间距划分	*/
	now_dose = (now_dose > 30 ? 30 : now_dose);
	
	if(last_dose != now_dose){
		Show_Variable_Icon(PAGE_Aging, PAGE_AGING_DOSE_ID, now_dose);
		return ;
	}else
		return ;
}

