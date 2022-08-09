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
#include "hampel_filter.h"
#include "pressure_origin_search.h"

/*	APP&DATA	*/
#include "user_data.h"			/*	�û���ϵͳ����	*/

//	����ԭ������
static AGING_M_CHANGE_DIR_T agig_position_compare(MOTOR_DIRECTION_T motor_dir_t);
static AGING_MODE get_page_mode_bottom_val(LCD_TOUCH_T * touch_t);
static void show_remain_dose(void);

static int aging_speed = 0;

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
#define FWD_SPD				injector_speed_t[3][2]
#define REV_SPD				injector_speed_t[3][2]
extern SemaphoreHandle_t	sPage_aging;				/*	�������ź�����֪ͨ��������ϻ�����	*/
extern LCD_TOUCH_T lcd_touch_t;	

void page_aging(void)
{
	AGING_MODE aging_mode_t = AGING_IDLE;							/*	�ϻ�״̬	*/
	AGING_M_CHANGE_DIR_T agig_m_change_dir_t = AGING_M_NEED_IDLE;	/*	�ϻ������е����Ҫ������״̬	*/
	
	static uint16_t footPedal_vol = 0;
	static uint16_t u16_f_fp_vol = 0;
	static uint16_t cnt,cnt2 = 0;
	static uint32_t fre_cnt = 0;

	if(xSemaphoreTake(sPage_aging, (TickType_t)0) == pdTRUE){
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));						/*	���������ʷ	*/				
		show_uint32_t_data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);	/*	��ʾ���ϻ���Ȧ��	*/
		
		AGING_PRINTF("\r\n--------------------Now in Page Aging--------------------\r\n");
		for(;;){
			aging_mode_t = get_page_mode_bottom_val(&lcd_touch_t);
			
			if(++cnt > 100){
				cnt = 0;	
				show_uint32_t_data(PAGE_AGING_COMM_ID, ++cnt2);				/*	����ͨ��	*/
			}
			
			switch(aging_mode_t){	
				case AGING_IDLE:
				break;
				
				case AGING_START:
					Init_adcIC();
					set_speed_mode_speed(FWD_SPD);
					motor_run_direction = MOTOR_AGING_FWD;
					aging_mode_t = AGING_IDLE;
				break;
				
				case AGING_STOP:
					set_motor_stop();
					StandBy_adcIC();
					aging_speed = 0;
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
			
			/*	1. �����ת�����л�״̬��	*/
			agig_m_change_dir_t = agig_position_compare(motor_run_direction);
			
			switch(agig_m_change_dir_t){
				case AGING_M_NEED_IDLE: 
				break;
				
				case AGING_M_NEED_FWD:
					sys_para_t.AGING_CYCLE ++;
					if(sys_para_t.AGING_CYCLE % 100 == 0)
						write_sys_para_t_data();		/*	����Flash�е��ϻ�Ȧ��	*/
					show_uint32_t_data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);/*	��Ļ��ʾ������Ȧ��	*/
					osDelay(200);
					aging_speed = FWD_SPD;
					set_speed_mode_speed(FWD_SPD);
					motor_run_direction = MOTOR_AGING_FWD;
					agig_m_change_dir_t = AGING_M_NEED_IDLE;
				break;
				
				case AGING_M_NEED_REV:
					osDelay(600);
					aging_speed = -REV_SPD;
					set_speed_mode_speed(-REV_SPD);
					motor_run_direction = MOTOR_AGING_REV;
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
				show_remain_dose();
				dynamic_pressure_kPa_F = hampel_filter((uint16_t)dynamic_pressure_kPa, 3);
//				AGING_PRINTF("%d,%d\r\n", (uint16_t)dynamic_pressure_kPa, dynamic_pressure_kPa_F);		/*	�鿴hampel�˲���λ�ӳ٣�3��������*/
				add_two_line_data(1, dynamic_pressure_kPa_F, 2, u16_f_fp_vol);
				show_uint16_t_data(PAGE_AGING_kPa_ID, dynamic_pressure_kPa_F);
				show_uint16_t_data(PAGE_AGING_mV_ID, footPedal_vol);
				fre_cnt = 0;
			}else{
				if(++fre_cnt > 10){
					StandBy_adcIC();
					osDelay(100);
					Init_adcIC();
					osDelay(100);
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
* ��	  ����V1.1
* ��    �ڣ�2022-07-12
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
	AGING_PRINTF("/*%d,%d,%d,%d,%d,%d,%d,%d*/\r\n", \
	(handle_injector_t.TOP_LOC >>16), 		(handle_injector_t.BOTTOM_LOC >>16), \
	(get_current_handle_injector() >>16), 	get_current_handle_run_distance(get_current_handle_injector() >>16), \
	(uint16_t)dynamic_pressure_kPa, 		aging_speed, 	handle_in_origin, dynamic_pressure_kPa_F);
	
	switch(motor_dir_t){
		case MOTOR_AGING_TOP: 
			return AGING_M_NEED_REV;		
		
		case MOTOR_AGING_ORIGIN: 
			handle_in_origin = false; 
			return AGING_M_NEED_FWD;
		
		case MOTOR_AGING_BOTTOM:
			motor_to_origin_cycle = motor_cycle + DOWN_CYCLE;
			set_speed_mode_speed(FWD_SPD);
			motor_run_direction = MOTOR_AGING_TO_ORIGIN;
			
			return AGING_M_NEED_IDLE;
		
		default: 
			return AGING_M_NEED_IDLE;
	}
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
	now_dose = compute_remain_dose(current_pos) / 3;	/*	��100 �� 3 �ȼ�໮��	*/
	now_dose = (now_dose > 30 ? 30 : now_dose);
	
	if(last_dose != now_dose){
		show_variable_icon(PAGE_Aging, PAGE_AGING_DOSE_ID, now_dose);
		return ;
	}else
		return ;
}

