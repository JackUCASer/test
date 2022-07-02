#include "page_aging.h"
#include "lcd_display.h"		/*	����ʾ��ص�ȫ������	*/

/*	��������	*/
#include "get_voltage.h"		/*	��ѹ����̤����ء������	*/
#include "oil_cs1231.h"			/*	Һѹ������	*/
#include "motor_misc.h"			/*	������ƽӿ�	*/

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	���ݽṹ���㷨��		*/
#include "lp_filter.h"			/*	��ͨ�˲�		*/
#include "movmean.h"			/*	����ƽ��		*/
#include <string.h>

/*	APP&DATA	*/
#include "user_data.h"			/*	�û���ϵͳ����	*/

//	����ԭ������
static AGING_M_CHANGE_DIR_T agig_position_compare(MOTOR_DIRECTION_T motor_dir_t);
static AGING_MODE get_page_mode_bottom_val(LCD_TOUCH_T * touch_t);
static void show_remain_dose(void);

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-11-08
* �� �� ��: TMY_AGing
* ����˵��: �������ϻ�����
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
#define FWD_SPD				injector_speed_t[1][1]
#define REV_SPD				injector_speed_t[1][1]
extern SemaphoreHandle_t	sPage_aging;				/*	�������ź�����֪ͨ��������ϻ�����	*/
extern LCD_TOUCH_T lcd_touch_t;	

void page_aging(void)
{
	MOTOR_DIRECTION_T motor_dir_t = MOTOR_IDLE;						/*	�����ת����	*/
	AGING_MODE aging_mode_t = AGING_IDLE;							/*	�ϻ�״̬	*/
	AGING_M_CHANGE_DIR_T agig_m_change_dir_t = AGING_M_NEED_IDLE;	/*	�ϻ������е����Ҫ������״̬	*/
	
	float32_t dynamic_pressure_mV = 0;
	float32_t dynamic_pressure_kPa = 0;
	uint32_t dynamic_pressure = 0;
	static uint16_t footPedal_vol = 0;
	static uint16_t u16_f_fp_vol = 0;
	static uint16_t cnt,cnt2 = 0;
	static uint32_t fre_cnt = 0;

	if(xSemaphoreTake(sPage_aging, (TickType_t)0) == pdTRUE){
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));				/*	���������ʷ	*/				
		show_uint32_t_data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);	/*	��ʾ���ϻ���Ȧ��	*/
		
		AGING_PRINTF("\r\n--------------------Now in Page Aging--------------------\r\n");
		for(;;){
			aging_mode_t = get_page_mode_bottom_val(&lcd_touch_t);
			
			if(++cnt > 100){
				cnt = 0;	
				show_uint32_t_data(PAGE_AGING_COMM_ID, ++cnt2);/*	����ͨ��	*/
			}
			
			switch(aging_mode_t){	
				case AGING_IDLE:
				break;
				
				case AGING_START:
					Init_adcIC();
					set_speed_mode_speed(FWD_SPD);
					motor_dir_t = MOTOR_FWD;
					aging_mode_t = AGING_IDLE;
				break;
				
				case AGING_STOP:
					set_motor_stop();
					StandBy_adcIC();
				break;
				
				case AGING_FINISH:
					aging_mode_t = EXIT_AGING;
				break;
				
				default: break;
			}
			
			if(aging_mode_t == EXIT_AGING){
				aging_mode_t = AGING_IDLE;
				motor_dir_t = MOTOR_IDLE;
				agig_m_change_dir_t = AGING_M_NEED_IDLE;
				break;
			}
			
			/*	1. �����ת�����л�״̬��	*/
			agig_m_change_dir_t = agig_position_compare(motor_dir_t);
			
			switch(agig_m_change_dir_t){
				case AGING_M_NEED_IDLE: 
				break;
				
				case AGING_M_NEED_FWD: 
					sys_para_t.AGING_CYCLE ++;
					if(sys_para_t.AGING_CYCLE % 100 == 0)
						write_sys_para_t_data();		/*	����Flash�е��ϻ�Ȧ��	*/
					set_motor_stop();
					show_uint32_t_data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);/*	��Ļ��ʾ������Ȧ��	*/
					set_speed_mode_speed(FWD_SPD);
					osDelay(100);
					motor_dir_t = MOTOR_FWD;
					agig_m_change_dir_t = AGING_M_NEED_IDLE;
				break;
				
				case AGING_M_NEED_REV:
					set_motor_stop();
					osDelay(200);
					set_speed_mode_speed(-REV_SPD);
					osDelay(100);
					motor_dir_t = MOTOR_REV;
					agig_m_change_dir_t = AGING_M_NEED_IDLE;
				break;
				
				default: break;
			}
			
			/*	�ȴ�Һѹ��ȡ���	*/
			if(get_oil_pressure(&dynamic_pressure) == New_Data_Read_Over){					
				dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
				dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa 
				footPedal_vol = get_footpedal_voltage();
				u16_f_fp_vol = movmean_filter(footPedal_vol);
				AGING_PRINTF("/*%d,%3.1f,%d,%3.2f,%3.2f,%d,%d,%d,%d*/\n\r",\
						motor_cycle, 			motor_angle/65536.0f*360, 	iq, 		dynamic_pressure_mV,\
						dynamic_pressure_kPa,	footPedal_vol, 			u16_f_fp_vol, 	sys_para_t.AGING_CYCLE, fre_cnt);
				show_remain_dose();
				add_two_line_data(1, (uint16_t)dynamic_pressure_kPa, 2, u16_f_fp_vol);
				show_uint16_t_data(PAGE_AGING_kPa_ID, (uint16_t)dynamic_pressure_kPa);
				show_uint16_t_data(PAGE_AGING_mV_ID, footPedal_vol);
				fre_cnt = 0;
			}else{
				if(++fre_cnt > 10){
					Init_adcIC();
					fre_cnt = 0;
				}	
			}
			
			osDelay(50);
		}
		set_motor_stop();
		StandBy_adcIC();
		set_now_page(PAGE_Setting);
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-11-09
* �� �� ��: TMY_AGing
* ����˵��: �������ϻ�������λ�öԱ�
*
* ��    ��: 
*			motor_dir_t: ��ǰ����ķ���
      
* �� �� ֵ:
*			AGING_M_CHANGE_DIR_T ����Ƿ���Ҫ�л�����
*********************************************************************************************************
*/
static AGING_M_CHANGE_DIR_T agig_position_compare(MOTOR_DIRECTION_T motor_dir_t)
{
	static uint16_t current_pos = 0;
	static uint16_t bottom_pos = 0;
	static uint16_t top_pos = 0;
	
	current_pos = (get_current_handle_injector() >> 16);
	bottom_pos = (handle_injector_t.BOTTOM_LOC >> 16);
	top_pos = (handle_injector_t.TOP_LOC >> 16);
	
	switch(motor_dir_t){
		case MOTOR_FWD:{
			if((current_pos&0x8000)==(top_pos&0x8000))	/*	��ǰλ�õ����λ��top_pos���λ��ͬ	*/
				if(current_pos >= top_pos)
					return AGING_M_NEED_REV;	/*	����ѳ���top_pos�� ��Ҫ�л�����	*/
				else
					return AGING_M_NEED_IDLE;	/*	����Ҫ�л�����	*/
			else										/*	��ǰλ�õ����λ��top_pos���λ����ͬ	*/
				return AGING_M_NEED_IDLE;		/*	����Ҫ�л�����	*/
		}
		
		case MOTOR_REV:{
			if((current_pos&0x8000)==(bottom_pos&0x8000))	/*	��ǰλ�õ����λ��bottom_pos���λ��ͬ	*/
				if(current_pos <= bottom_pos)
					return AGING_M_NEED_FWD;	/*	��������ѳ���bottom_pos�� ��Ҫ�л�����	*/
				else
					return AGING_M_NEED_IDLE;	/*	����Ҫ�л�����	*/
			else										/*	��ǰλ�õ����λ��bottom_pos���λ��ͬ	*/
				return AGING_M_NEED_IDLE;		/*	����Ҫ�л�����	*/
		}
		
		case MOTOR_IDLE: 
			return AGING_M_NEED_IDLE;
	}
	
	return AGING_M_NEED_IDLE;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-11-08
* �� �� ��: get_page_mode_bottom_val
* ����˵��: agingҳ���°�����ȡ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern uint16_t usCurrent_page_id;
static AGING_MODE get_page_mode_bottom_val(LCD_TOUCH_T * touch_t)
{	
	if((touch_t->page_id == PAGE_Aging) && (touch_t->touch_wait_read))	/*	�����Aging����İ�ť������,���ǻ�δ��ȡ	*/
	{
		touch_t->touch_wait_read = 0;		/*	��ʾ��ť�ѱ���ȡ	*/
		switch(touch_t->touch_id){
			case PAGE_AGING_START_ID:		/*	�ϻ���ʼ��ť	*/
				if(touch_t->touch_state)
					return AGING_START;
				else
					return AGING_STOP;
			
			case PAGE_AGING_FINISH_ID:		/*	�ϻ���ɰ�ť	*/
				return AGING_FINISH;

			default: break;
		}
	}
	return AGING_RUNING;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-03-15
* �� �� ��: show_remain_dose
* ����˵��: ��ʾʣ��ҩ��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void show_remain_dose(void)
{
	static uint16_t current_pos = 0;
	static uint8_t last_dose = 0;
	static uint8_t now_dose = 0;
	
	last_dose = now_dose; 
	current_pos = (get_current_handle_injector() >> 16);
	now_dose = compute_remain_dose(current_pos);
	if(now_dose <= 4)
		now_dose = 0;
	else if((now_dose >4 ) && (now_dose <=20))
		now_dose = 1;
	else if((now_dose >20) && (now_dose <=36))
		now_dose = 2;
	else if((now_dose >36) && (now_dose <=52))
		now_dose = 3;
	else if((now_dose >52) && (now_dose <=68))
		now_dose = 4;
	else if((now_dose >68) && (now_dose <=84))
		now_dose = 5;
	else
		now_dose = 6;
	
	if(last_dose != now_dose){
		show_variable_icon(PAGE_Aging, PAGE_AGING_DOSE_ID, now_dose);
		return ;
	}else
		return ;
}

