#include "Page_Aging.h"
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
#include "Hampel_Filter.h"
#include "Pressure_Origin_Search.h"

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
#define FWD_SPD				injector_speed_t[3][2]
#define REV_SPD				injector_speed_t[3][2]
#define DELAY_ONE_MIN		230							//	һ���ϻ�����ʱ�� = 1����ʱ��Ӧ�����ӳ�
extern SemaphoreHandle_t	sPage_aging;				/*	�������ź�����֪ͨ��������ϻ�����	*/
extern LCD_TOUCH_T lcd_touch_t;	

void Page_Aging(void)
{
	AGING_MODE aging_mode_t = AGING_IDLE;							/*	�ϻ�״̬	*/
	AGING_M_CHANGE_DIR_T agig_m_change_dir_t = AGING_M_NEED_IDLE;	/*	�ϻ������е����Ҫ������״̬	*/
	
	static uint16_t footPedal_vol = 0;
	static uint16_t u16_f_fp_vol = 0;
	static uint16_t cnt,cnt2,cnt3 = 0;
	static uint32_t fre_cnt = 0;
	static bool get_origin_kPa_flag = false;
	static uint16_t origin_kPa = 0;

	if(xSemaphoreTake(sPage_aging, (TickType_t)0) == pdTRUE){
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));						/*	���������ʷ	*/				
		Show_uint32_t_Data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);	/*	��ʾ���ϻ���Ȧ��	*/
		
		AGING_PRINTF("\r\n--------------------Now in Page Aging--------------------\r\n");
		for(;;){
			aging_mode_t = get_page_mode_bottom_val(&lcd_touch_t);
			
			if(++cnt > 20){
				cnt = 0;	
				Show_uint32_t_Data(PAGE_AGING_COMM_ID, ++cnt2);				/*	����ͨ��	*/
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
			
			/*	1. �����ת�����л�״̬��	*/
			agig_m_change_dir_t = agig_position_compare(motor_run_direction);
			
			switch(agig_m_change_dir_t){
				case AGING_M_NEED_IDLE:
					cnt3 = 0;
				break;
				
				case AGING_M_NEED_FWD:
					if(++cnt3 > DELAY_ONE_MIN){
						if(sys_bat_too_low == false){
							get_origin_kPa_flag = true;				/*	֪ͨ��¼ԭ�㴦��Һѹ	*/
							sys_para_t.AGING_CYCLE ++;
							if(sys_para_t.AGING_CYCLE % 50 == 0)	/*	����Flash�е��ϻ�Ȧ��	*/
								Write_Sys_Para_Data();										
							Show_uint32_t_Data(PAGE_AGING_CYCLE_ID, sys_para_t.AGING_CYCLE);
							set_speed_mode_speed(FWD_SPD, MOTOR_AGING_FWD);
							agig_m_change_dir_t = AGING_M_NEED_IDLE;
						}else	//	�ȴ��͵����������ƽ��
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
			
			/*	�ȴ�Һѹ��ȡ���	*/
			if(Get_Oil_Pressure(&dynamic_pressure) == New_Data_Read_Over){					
				dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
				dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa 
				footPedal_vol = Get_Foot_Pedal_Vol();
				u16_f_fp_vol = Movmean_Filter(footPedal_vol);
				show_remain_dose();
				dynamic_pressure_kPa_F = Hampel_Filter((uint16_t)dynamic_pressure_kPa, 3);
//				AGING_PRINTF("%d,%d\r\n", (uint16_t)dynamic_pressure_kPa, dynamic_pressure_kPa_F);		/*	�鿴hampel�˲���λ�ӳ٣�3��������*/
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
//	AGING_PRINTF("/*%d,%d,%d,%d,%d,%d,%d,%d*/\r\n", \
//	(handle_injector_t.TOP_LOC >>16), 		(handle_injector_t.ORIGIN_LOC >>16), \
//	(Get_Current_Handle_Injector_Pos() >>16), 	Get_Current_Handle_Run_Distance(Get_Current_Handle_Injector_Pos() >>16), \
//	(uint16_t)dynamic_pressure_kPa, 		dynamic_pressure_kPa_F, movemean_iq, error_status);
	
	switch(motor_dir_t){
		case MOTOR_AGING_TOP:
			return AGING_M_NEED_REV;		
		
		case MOTOR_AGING_ORIGIN: 
			Cycle_Reset_To_Origin();	//	λ������
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
				vgus_page_objs[PAGE_Aging].obj_value[0] = touch_t->touch_state;	//	��¼��ť�ؼ�ֵ
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
	
	if(wait_send_over == 1)
		return ;
	
	last_dose = now_dose; 
	current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	now_dose = Compute_Reamin_Dose(current_pos) / 3;	/*	��100 �� 3 �ȼ�໮��	*/
	now_dose = (now_dose > 30 ? 30 : now_dose);
	
	if(last_dose != now_dose){
		Show_Variable_Icon(PAGE_Aging, PAGE_AGING_DOSE_ID, now_dose);
		return ;
	}else
		return ;
}

