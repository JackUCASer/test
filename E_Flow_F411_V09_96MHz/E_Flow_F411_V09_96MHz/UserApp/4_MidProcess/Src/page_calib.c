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
	footPedal_vol = Get_Foot_Pedal_Vol();
	f_footPedal_vol = Movmean_Filter(footPedal_vol);
	
	
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
* ��    �ڣ�2022-07-12
* �� �� ���� check_reach_endpoint
* ����˵���� ������Ƿ񵽴�˵�
* ��    ��: 
*			motor_dir_t: �����ת����
* ˵    ����
			
* �� �� ֵ: 
*			����Ƿ��ڴﵽ�˵�
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-29
* �� �� ���� oil_Pressure_Calib_Motor_Para_Calucate
* ����˵���� 
*	1. ����������˿����ײ�ʱ������λ��[Ȧ�����Ƕ�]�������������ȫ�ֱ���handle_injector_t.ORIGIN_LOC��
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
	
	handle_injector_t.ORIGIN_LOC = motor_loc_t->REV_2nd_LOC;
	
	/*	2. ��ȡ˿����������Ȧ���ͽǶȼĴ�����Ӧ����ֵ	*/
	handle_injector_t.TOP_LOC = motor_loc_t->FWD_1st_LOC;
	
	/*	3. ��ȡ˿�˿�ִ�����г̵����ִ��Ȧ�� DISTANCE	*/
	handle_injector_t.DISTANCE = handle_injector_t.TOP_LOC - handle_injector_t.ORIGIN_LOC;				//	У��ʱ���˵��׶������г�
	handle_injector_t.BOTTM2ORIGIN = handle_injector_t.DISTANCE - ((MAX_DIS_CYCLES + UP_CYCLE) << 16);	//	�ײ���ԭ��ľ���
	
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-29
* �� �� ���� oil_Pressure_Calibration
* ����˵���� Һѹ�г�У������
*			Step 1: ���ǰ�������ˣ� 
*			Step 2: ����������׶ˣ�
*			Step 3: ���ǰ����ԭ�㣻
*			Step 4: �����г�У����������棬���õ��λ�òο���
* ��    �Σ�
*			
* ˵    ����
*			
* �� �� ֵ��
*			OIL_PRESS_CALIB_STATE_T* ���͵�У�����裨״̬����
*********************************************************************************************************
*/
#define FWD_SPD				injector_speed_t[3][2]	/*	У�������У������ת�ٶ�	*/
#define REV_SPD				injector_speed_t[3][2]	/*	У�������У������ת�ٶ�	*/
#define REV_SPD_LOW			injector_speed_t[2][0]	/*	У�������У������ת����	*/
#define CHG_REV_SPD_TIME	20						/*	�ı䷴ת�ٶȵĵȴ�ʱ��	*/

static OIL_PRESS_CALIB_STATE_T oil_press_calib_state = CALIB_READY;

static OIL_PRESS_CALIB_STATE_T* oil_Pressure_Calibration(void)
{
	static REACH_ENDPOINT_T motor_reach_endpoint_t = KEEP_RUNNING;
	static OIL_PRESS_CALIB_MOTOR_LOC_T oil_press_calib_motor_loc_t = {0};
	static uint8_t delay_10ms_cnt = 0;
	
	switch(oil_press_calib_state)
	{
		case CALIB_READY:{
				//	1. ���������ת
			set_speed_mode_speed(100, MOTOR_CALIB_FWD);					/*	1�������ȵ���ǰ��	*/
			osDelay(200);												/*	2�����ӳ�200ms	*/
			set_speed_mode_speed(FWD_SPD, MOTOR_CALIB_FWD);				/*	3����Ȼ���������ٶ�ǰ��	*/
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
			if(delay_10ms_cnt > 30){	//	�ӳ�300ms
				// 2. ��¼����λ��
				motor_cycle = TOP_REF_CYCLE;
				oil_press_calib_motor_loc_t.FWD_1st_LOC = (motor_cycle << 16) + motor_angle;
				set_speed_mode_speed(-REV_SPD_LOW, MOTOR_CALIB_REV);	/*	1�������ȵ��ٻ���	*/
				oil_press_calib_state = CALIB_FWD_IDLE2;
				delay_10ms_cnt = 0;
			}
		}break;
		
		case CALIB_FWD_IDLE2:{
			delay_10ms_cnt ++;
			check_reach_endpoint(motor_run_direction);
			if(delay_10ms_cnt > 100){	//	�ӳ�1s
				set_speed_mode_speed(-REV_SPD, MOTOR_CALIB_REV);		/*	2����Ȼ���������ٶȻ���	*/
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
//				// 3. ��¼�ײ�λ��
//				oil_press_calib_motor_loc_t.REV_2nd_LOC = (motor_cycle <<16) + motor_angle;
//				
//				// 4. �����ֱ�ע�����г�: 
//				oil_Pressure_Calib_Motor_Para_Calucate(&oil_press_calib_motor_loc_t);
//				motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
//				set_speed_mode_speed(FWD_SPD, MOTOR_CALIB_TO_ORIGIN);	/*	��ʼǰ����ԭ��Origin	*/
//				oil_press_calib_state = CALIB_FWD_3rd;
			}
		}break;
		
		case CALIB_REV_IDLE:{
			delay_10ms_cnt ++;
			check_reach_endpoint(motor_run_direction);
			if(delay_10ms_cnt > 30){
				// 3. ��¼�ײ�λ��
				oil_press_calib_motor_loc_t.REV_2nd_LOC = (motor_cycle <<16) + motor_angle;
				
				// 4. �����ֱ�ע�����г�: 
				oil_Pressure_Calib_Motor_Para_Calucate(&oil_press_calib_motor_loc_t);
				motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
				set_speed_mode_speed(FWD_SPD, MOTOR_CALIB_TO_ORIGIN);	/*	��ʼǰ����ԭ��Origin	*/
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

void PAGE_Calibration(void)
{
	static bool keep_FP_calib = 0;				/*	���ֽ�̤У��	*/
	static bool keep_OP_calib = 0;				/*	����ҺѹУ��	*/
	static uint16_t footPedal_vol = 0;
	static uint16_t u16_f_fp_vol = 0;
	int8_t i8_result = 0;
	uint8_t cnt = 0;
	OIL_PRESS_CALIB_STATE_T*	oil_calib_state_t = NULL;

	if(xSemaphoreTake(sPage_calib, (TickType_t)0) == pdTRUE)
	{
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	���������ʷ	*/
		oil_press_calib_state = CALIB_READY;
		motor_run_direction = MOTOR_IDLE;					/*	���µ��״̬Ϊ��IDLEģʽ	*/
		
		if(now_display_page == PAGE_MotorCalib){			//	��ǰ���ڵ��У������
			MP6570_AutoThetaBias(0x00, 200, 2000); 
			set_motor_run_in_speed_mode();
			keep_OP_calib = 1;
			CALIB_PRINTF("\r\n--------------------Now in Page Calibration--------------------\r\n");
			Init_ADC_IC();				/*	��ʼ��CS1231�����ڻ�ȡҺѹ		*/
			for(;;){
				/*---------------------------------------1. ҺѹУ�����---------------------------------*/
				if(keep_OP_calib)					/*	����Ƿ񵽴����״̬	*/
					oil_calib_state_t = oil_Pressure_Calibration();
				
				/*---------------------------------------2. У������FINISH����������----------------------*/
				if((lcd_touch_t.page_id == PAGE_MotorCalib)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_CALIB_FINISH_ID)){
					lcd_touch_t.touch_wait_read = 0;
					
					/*	��¼Һѹ����	*/
					if(*oil_calib_state_t == CALIB_FINISH){		/*	����У����ɣ����Finish��ť�ſɱ�������	*/
						i8_result = Write_Handle_Injector_Data();	/*	�������������ݿ⣬�г���oil_Pressure_Calibration()�����	*/
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
					/*	�ȴ�Һѹ��ȡ���	*/
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
				/*---------------------------------------1. ��̤У�����---------------------------------*/
				if((lcd_touch_t.page_id == PAGE_FP_CALIB)&& lcd_touch_t.touch_wait_read && (lcd_touch_t.touch_id == PAGE_CALIB_PEDAL_ID)){
//					vgus_page_objs[PAGE_MotorCalib].obj_value[0] = lcd_touch_t.touch_state;	//	��¼��ť�ؼ�ֵ
					lcd_touch_t.touch_wait_read = 0;
					if(lcd_touch_t.touch_state)
						keep_FP_calib = 1;
					else
						keep_FP_calib = 0;	
				}	
				if(keep_FP_calib)
					foot_Pedal_Calibration();		/*	���ֽ�̤У��		*/
				
				/*---------------------------------------2. У������FINISH����������----------------------*/
				if((lcd_touch_t.page_id == PAGE_FP_CALIB)&& lcd_touch_t.touch_wait_read &&(lcd_touch_t.touch_id == PAGE_PEDAL_CALIB_FINISH_ID)){
					lcd_touch_t.touch_wait_read = 0;
					/*	��¼��̤����	*/
					if(keep_FP_calib){				/*	��̤У�������У���ɰ�ť�����£�˵����̤У���ɹ�	*/
						i8_result = Write_Foot_Pedal_Data();	/*	�������������ݿ�	*/
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
					/*	��ʾ��̤ѹ�����ߺ;�����ֵ	*/
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




