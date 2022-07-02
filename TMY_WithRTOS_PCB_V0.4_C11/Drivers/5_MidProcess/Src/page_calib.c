#include "page_calib.h"

/*	��������	*/
#include "lcd_display.h"
#include "get_voltage.h"		/*	��ѹ����̤����ء������	*/
#include "oil_cs1231.h"			/*	Һѹ������	*/
#include "motor_misc.h"			/*	������ƽӿ�	*/

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	���ݽṹ���㷨��		*/
#include "movmean.h"			/*	����ƽ��		*/
#include "lp_filter.h"			/*	��ͨ�˲�		*/

/*	APP&DATA	*/
#include "user_data.h"			/*	�û���ϵͳ����	*/


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-14
* �� �� ��: foot_Pedal_Calibration
* ����˵��: ��̤У������
* ��    ��: 
*			��
* ˵    ����
			
* �� �� ֵ: 
*********************************************************************************************************
*/
static void foot_Pedal_Calibration(void)
{
	static uint16_t footPedal_vol = 0;
	static uint16_t f_footPedal_vol = 0;
	static uint16_t max_footPedal_vol = 0;
	static uint16_t min_footPedal_vol = 3300;
	/*	1. UI��ʾ�û���20s����ɽ�̤У��:��ȡ��Сֵ�����ֵ	*/
	footPedal_vol = get_footpedal_voltage();
	f_footPedal_vol = movmean_filter(footPedal_vol);
	
	
	/*	2. �뽫��̤������ײ�����ȡ��ѹ���ֵ	*/
	max_footPedal_vol = (max_footPedal_vol >= f_footPedal_vol) ? max_footPedal_vol : f_footPedal_vol;
	
	/*	3. ���ɿ���̤����ȡ��ѹ��Сֵ��ʵ�����ǻ�ȡ��ֵ̬	*/
	min_footPedal_vol = (min_footPedal_vol <= f_footPedal_vol) ? min_footPedal_vol : f_footPedal_vol;
	
	/*	4. ֪ͨ�û�����̤����У�����	*/
	foot_pedal_t.MAX = max_footPedal_vol;
	foot_pedal_t.MIN = min_footPedal_vol;
	foot_pedal_t.QUARTER_DELTA = ((foot_pedal_t.MAX - foot_pedal_t.MIN) >> 2);		/*	ȡ1/4	*/
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-29
* �� �� ���� check_reach_endpoint
* ����˵���� ������Ƿ񵽴�˵�
* ��    ��: 
*			motor_dir_t: �����ת����
* ˵    ����
			
* �� �� ֵ: 
*			����Ƿ��ڴﵽ�˵�
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-30
* �� �� ���� oil_Pressure_Calib_Motor_Para_Calucate
* ����˵���� 
*	1. ����������˿����ײ�ʱ������λ��[Ȧ�����Ƕ�]�������������ȫ�ֱ���handle_injector_t.BOTTOM_LOC��
*	2. ����������˿�����ʱ������λ��[Ȧ�����Ƕ�]�������������ȫ�ֱ���handle_injector_t.TOP_LOC��
*	3. ��������˿����ײ�������˿�������Ȧ����ע�⣺��Ȧ�����������������ȫ�ֱ���handle_injector_t.DISTANCE��
* ��    ��: 
*			��
* ˵    ����
			
* �� �� ֵ: 
*********************************************************************************************************
*/
static void oil_Pressure_Calib_Motor_Para_Calucate(OIL_PRESS_CALIB_MOTOR_LOC_T* motor_loc_t)
{
	
	handle_injector_t.BOTTOM_LOC = motor_loc_t->REV_2nd_LOC;
	
	/*	2. ��ȡ˿����������Ȧ���ͽǶȼĴ�����Ӧ����ֵ	*/
	handle_injector_t.TOP_LOC = motor_loc_t->FWD_1st_LOC;
	
	/*	3. ��ȡ˿�˿�ִ�����г̵����ִ��Ȧ�� DISTANCE	*/
	if(handle_injector_t.TOP_LOC > handle_injector_t.BOTTOM_LOC)
		handle_injector_t.DISTANCE = handle_injector_t.TOP_LOC - handle_injector_t.BOTTOM_LOC;
	else
		handle_injector_t.DISTANCE = (0xFFFFFFFF - handle_injector_t.BOTTOM_LOC) + handle_injector_t.TOP_LOC;
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-29
* �� �� ���� oil_Pressure_Calibration
* ����˵���� ҺѹУ������
*			1. �г̵Ĳ����� 
*			2. ��ͬλ�õ�Һѹ�����ĵ㣺Full, 3/4, 2/4, 1/4, Empty��
* ��    ��: 
*			��
* ˵    ����
			
* �� �� ֵ: 
*********************************************************************************************************
*/
#define UP_CYCLE			40				/*	��������Ȧ��	*/
#define DOWN_CYCLE			160				/*	�ײ�ǰ��Ȧ��:160	*/
#define UP_THRESHOLD		UP_CYCLE*360.0/m_gear_ratio
#define DOWM_THRESHOLD		DOWN_CYCLE*360.0/m_gear_ratio

#define FWD_SPD				injector_speed_t[1][1]	/*	У�������У������ת�ٶ�FWD_SPD�ͷ�ת�ٶ�REV_SPD	*/
#define REV_SPD				injector_speed_t[1][1]
static OIL_PRESS_CALIB_STATE_T oil_press_calib_state = CALIB_READY;
static OIL_PRESS_CALIB_STATE_T* oil_Pressure_Calibration(void)
{
	static REACH_ENDPOINT_T motor_reach_endpoint_t = KEEP_RUNNING;
	static OIL_PRESS_CALIB_MOTOR_LOC_T oil_press_calib_motor_loc_t = {0};
	
	switch(oil_press_calib_state)
	{
		case CALIB_READY:{
			set_speed_mode_speed(100);				/*	��ʼǰ��	*/
			osDelay(200);
			set_speed_mode_speed(FWD_SPD);			/*	��ʼǰ��	*/
			oil_press_calib_state = CALIB_FWD_1st;
		}break;
		
		case CALIB_FWD_1st:{
			motor_reach_endpoint_t = check_reach_endpoint(MOTOR_FWD);
			if(motor_reach_endpoint_t == REACH_TOP){
				set_motor_stop();
				osDelay(300);
				// 2. ��¼����λ��
				oil_press_calib_motor_loc_t.FWD_1st_LOC = (motor_cycle << 16) + motor_angle;
				set_speed_mode_speed(-100);
				osDelay(300);
				set_speed_mode_speed(-REV_SPD);		/*	��ʼ����	*/
				oil_press_calib_state = CALIB_REV_2nd;
			}
		}break;
	
		
		case CALIB_REV_2nd:{
			motor_reach_endpoint_t = check_reach_endpoint(MOTOR_REV);
			if(motor_reach_endpoint_t == REACH_BOTTOM){
				set_motor_stop();
				// 3. ��¼�ײ�λ��
				oil_press_calib_motor_loc_t.REV_2nd_LOC = (motor_cycle << 16) + motor_angle;
				
				// 4. �����ֱ�ע�����г�: �ȼ���ʵ���г̣��ټ�ȥ��βUP_DOWM_CYCLE���г�
				oil_Pressure_Calib_Motor_Para_Calucate(&oil_press_calib_motor_loc_t);
				osDelay(500);
				uint32_t end_cycle = motor_cycle + DOWN_CYCLE;
				if(end_cycle > 65530)
					end_cycle = 0;
				set_speed_mode_speed(FWD_SPD);		/*	��ʼǰ��DOWN_CYCLEȦ	*/
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-10-26
* �� �� ��: TMY_Calibration
* ����˵��: ��̤��ҺѹУ�����򣬽�У���Ľ����������Ƭ���ڲ�Flash��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sPage_calib;
extern LCD_TOUCH_T 			lcd_touch_t;		/*	0x10 ����	*/	
extern uint16_t				usCurrent_page_id;	/*	��ǰ�������������ڵĽ���ID	*/
void page_calibration(void)
{
	static bool keep_FP_calib = 0;				/*	���ֽ�̤У��	*/
	static bool keep_OP_calib = 0;				/*	����ҺѹУ��	*/
	int8_t i8_result = 0;
	OIL_PRESS_CALIB_STATE_T*	oil_calib_state_t = NULL;
	uint32_t dynamic_pressure = 0;
	float32_t dynamic_pressure_mV = 0;
	float32_t dynamic_pressure_kPa = 0;

	if(xSemaphoreTake(sPage_calib, (TickType_t)0) == pdTRUE)
	{
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	���������ʷ	*/
		oil_press_calib_state = CALIB_READY;
		MP6570_AutoThetaBias(0x00, 200, 2000); 
		set_motor_run_in_speed_mode();
		keep_OP_calib = 1;
		CALIB_PRINTF("\r\n--------------------Now in Page Calibration--------------------\r\n");
		Init_adcIC();				/*	��ʼ��CS1231�����ڻ�ȡҺѹ		*/
		for(;;){
			/*---------------------------------------1. ��̤У�����---------------------------------*/
			if((lcd_touch_t.page_id == PAGE_Calibration)&& lcd_touch_t.touch_wait_read && (lcd_touch_t.touch_id == PAGE_CALIB_PEDAL_ID)){
				if(lcd_touch_t.touch_state)
					keep_FP_calib = 1;
				else
					keep_FP_calib = 0;
				lcd_touch_t.touch_wait_read = 0;
			}	
			if(keep_FP_calib)
				foot_Pedal_Calibration();		/*	���ֽ�̤У��		*/
			
			/*---------------------------------------2. ҺѹУ�����---------------------------------*/
			if((lcd_touch_t.page_id == PAGE_Calibration)&& lcd_touch_t.touch_wait_read && (lcd_touch_t.touch_id == PAGE_CALIB_OILPS_ID)){
				if(lcd_touch_t.touch_state){
					keep_OP_calib = 1;
				}else
					keep_OP_calib = 0;
				lcd_touch_t.touch_wait_read = 0;
			}
			if(keep_OP_calib)					/*	����Ƿ񵽴����״̬	*/
				oil_calib_state_t = oil_Pressure_Calibration();	
					
			
			/*---------------------------------------3. У������FINISH����������----------------------*/
			if((lcd_touch_t.page_id == PAGE_Calibration)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_CALIB_FINISH_ID)){
				lcd_touch_t.touch_wait_read = 0;
				
				/*	3.1 ��¼��̤����	*/
				if(keep_FP_calib){				/*	��̤У�������У���ɰ�ť�����£�˵����̤У���ɹ�	*/
					i8_result = write_foot_pedal_t_data();	/*	�������������ݿ�	*/
					if(i8_result)
						CALIB_PRINTF("\r\ntmy_write_user_data  foot_pedal_t error!\r\n");
					else{
						CALIB_PRINTF("\r\nfoot_pedal_t.MAX           = %d\r\n", foot_pedal_t.MAX);
						CALIB_PRINTF("foot_pedal_t.MIN           = %d\r\n", foot_pedal_t.MIN);
						CALIB_PRINTF("foot_pedal_t.QUARTER_DELTA = %d\r\n", foot_pedal_t.QUARTER_DELTA);
					}
				}
				/*	3.2 ��¼Һѹ����	*/
				if(*oil_calib_state_t == CALIB_FINISH){		/*	����У����ɣ����Finish��ť�ſɱ�������	*/
					handle_injector_t.LAST_LOC = 0;					/*	У���꣬�ϴ�λ������	*/
					i8_result = write_handle_injector_data();		/*	�������������ݿ⣬�г���oil_Pressure_Calibration()�����	*/
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
			/*	�ȴ�Һѹ��ȡ���	*/
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




