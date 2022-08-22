#include "page_mode.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	����Ӳ��		*/
#include "get_voltage.h"		/*	��ѹ����̤����ء������	*/
#include "oil_cs1231.h"			/*	Һѹ������	*/
#include "motor_misc.h"			/*	������ƽӿ�	*/

/*	APP&DATA	*/
#include "user_data.h"			/*	�û���ϵͳ����	*/

/*	���ݽṹ���㷨��		*/
#include "lp_filter.h"			/*	��ͨ�˲�		*/
#include "movmean.h"			/*	����ƽ��		*/
#include <string.h>
#include "hampel_filter.h"

typedef struct{
	uint32_t last_speed;
	uint32_t now_speed;
}SET_SPEED_T;

typedef struct{
	bool PDL;			/*	PDL��������	*/
	bool OVERPRESSURE;	/*	over pressure ע���ѹ	*/
	bool PRESSURE;		/*	ѹ�����ѣ�ȡ��	*/
	bool SPEED;			/*	�ٶ�����	*/
	bool ASP;			/*	��������	*/
	bool REMAIN_LOW;	/*	ʣ��ҩ����	*/
	bool REMAIN_EMPTY;	/*	ʣ��ҩ����	*/
}MATCH_MUSIC_T;

typedef void (*DO_SELECTED_MODE)(void);	/*	����һ��ָ���������Ϊvoid���ͣ�����ֵΪvoid �ĺ���ָ������	*/

//	����ԭ������
static PAGE_MODE_SELECT touch_btm_scan(LCD_TOUCH_T * lcd_touch_btm);				/*	����ɨ��	*/
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol);							/*	��̤ɨ��	*/
static void run_normal_mode(void);													/*	����Normalģʽ	*/
static void run_fast_mode(void);													/*	����fastģʽ	*/
static void run_pdl_mode(void);														/*	����pdlģʽ	*/
static void run_dosage_mode(void);
static void stop_dosage_mode(void);
static void run_change_cartridge(void);												/*	���и���ҩƿģʽ	*/
static void run_idle_mode(void);													/*	��ģʽ	*/
static MODE_M_CHANGE_DIR_T Mode_Position_Compare(MOTOR_DIRECTION_T motor_dir_t);	/*	ʵʱλ�ü��	*/
static void motor_speed_adjust_accordingto_fp(PAGE_MODE_SELECT page_mode_select);	/*	��̤ʵʱ����	*/
static void motor_asp_response(SET_SPEED_T sp_t);									/*	��������	*/
static void show_remain_dose(void);													/*	��ʾҩҺ����	*/
static void show_oil_pressure(uint16_t oil_pressure);									/*	��ʾ����	*/
static void play_music_at_same_intervals(uint8_t f_100ms, bool do_play_music);		/*	�ȼ����������	*/
static void clear_match_music(void);												/*	������ֱ�־λ	*/
static void play_mode_choice_voice(PAGE_MODE_SELECT mode_choice);

// �ⲿ����	
extern SemaphoreHandle_t 	sPage_mode;
extern SemaphoreHandle_t	sMotor_goOrigin;
extern SemaphoreHandle_t	sMotor_ASP;
extern LCD_TOUCH_T 			lcd_touch_t;

bool GO_TOP_FLAG = false;
bool GO_ORIGIN_FLAG = false;
bool START_ASP_FLAG = false;
int16_t asp_cycle = 0;
// �ڲ�����
static DO_SELECTED_MODE run_mode = run_idle_mode;
static MODE_M_CHANGE_DIR_T  mode_m_change_dir_t = MODE_M_NEED_IDLE;
static bool first_run_idle = false; 								/*	�״����п���ģʽ	*/
static FOOT_PEDAL_STATE_T fp_state_t = FP_IDLE;						/*	��̤״̬	*/
static PAGE_MODE_SELECT music_according_to_mode = MODE_IDLE;		/*	���ݵ�ǰģʽѡ������	*/
static PAGE_MODE_SELECT press_according_to_mode = MODE_IDLE;		/*	���ݵ�ǰģʽ��ʾѹ��	*/

// ���ֲ������
static bool DO_PLAY_MUSIC = false;
static float MUSIC_F_100MS = 0.0f;
static MATCH_MUSIC_T match_music_t = {false};
static uint8_t SPEED_INDEX = 0;

//	�붨��ע�����
static bool DOSAGE_FLAG = false;
static uint16_t fix_dose = 0;
// ----------------------------mL: 0  0.1 0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.0  1.1  1.2  1.3  1.4  1.5  1.6  1.7
static uint16_t ADJUST_DOSE[18] = {0, 60, 120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720, 780, 840, 900, 960, 1020};

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: page_mode
* ����˵��: mode�����µĳ���
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void page_mode(void)
{	
	static PAGE_MODE_SELECT mode_selected = MODE_IDLE;
	static uint16_t footPedal_vol = 0;
	static uint16_t f_footPedal_vol = 0;
	static uint32_t fre_cnt = 0;
	if(xSemaphoreTake(sPage_mode, (TickType_t)0) == pdTRUE)	/*	��ȡ�ź����ɹ�	*/
	{
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	���������ʷ	*/
		clear_match_music();
		DO_PLAY_MUSIC = true;
		first_run_idle = true;
		START_ASP_FLAG = false;
		Init_adcIC();
//		MODE_PRINTF("\r\n--------------------Now in Page Mode--------------------\r\n");
		for(;;){
			mode_selected = touch_btm_scan(&lcd_touch_t);
			play_mode_choice_voice(mode_selected);
			if(mode_selected == MODE_SETTING){
				break;	/*	����ѭ��	*/
			}else{
				switch(mode_selected){
					case MODE_NORMAL:
						run_mode = run_normal_mode;			/*	����Normal����ģʽ	*/
						music_according_to_mode = MODE_NORMAL;
						press_according_to_mode = MODE_NORMAL;
						show_variable_icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_OTHER_START_INDEX);
						break;
					
					case MODE_FAST:
						run_mode = run_fast_mode;			/*	����Fast����ģʽ	*/
						music_according_to_mode = MODE_FAST;
						press_according_to_mode = MODE_FAST;
						show_variable_icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_OTHER_START_INDEX);
						break;
					
					case MODE_PDL:					
						run_mode = run_pdl_mode;			/*	����Pdl����ģʽ	*/
						music_according_to_mode = MODE_PDL;
						press_according_to_mode = MODE_PDL;
						show_variable_icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_PDL_START_INDEX);
						break;
					
					case MODE_DOSAGE:						
						run_mode = run_dosage_mode;			/*	���붨��ע�����	*/
						music_according_to_mode = MODE_FAST;
						press_according_to_mode = MODE_FAST;
						break;
					
					case MODE_CHANGE_CARTRIDGE:				/*	�������ҩƿģʽ	*/
						run_mode = run_change_cartridge;
						music_according_to_mode = MODE_IDLE;
						press_according_to_mode = MODE_IDLE;
						break;
					
					case MODE_IDLE:
						first_run_idle = true;
						run_mode = run_idle_mode;			/*	����idle����ģʽ	*/
						music_according_to_mode = MODE_IDLE;
						press_according_to_mode = MODE_IDLE;
						break;
					
					case MODE_KEEP: 						/*	���ֵ�ǰģʽ�������ת���򲻱�	*/
						break;
					
					default:break;
				}
			}
			fp_state_t = FootPedal_Scan(f_footPedal_vol);	/*	��̤ɨ��	*/
			if(!START_ASP_FLAG) // ��ǰ���Ǵ��ڻ�������
				run_mode();
			
			if(get_oil_pressure(&dynamic_pressure) == New_Data_Read_Over){			
				dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
				dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa
				footPedal_vol = get_footpedal_voltage();
				f_footPedal_vol = movmean_filter(footPedal_vol);
				dynamic_pressure_kPa_F = hampel_filter((uint16_t)dynamic_pressure_kPa, 3);
				if((!START_ASP_FLAG)&&(!GO_ORIGIN_FLAG))		// �����ͻ�ҩƿ���̣�ҩҺ��������
					show_remain_dose();	// ��ʾʣ��ҩ��
				show_oil_pressure(dynamic_pressure_kPa_F);	// ��ʾѹ��
//				MODE_PRINTF("footPedal_vol = %d, fp_state_t = %d\r\n", f_footPedal_vol, fp_state_t);	//	���Խ�̤�ĸо�
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
//		set_motor_stop();
		StandBy_adcIC();
		DO_PLAY_MUSIC = false;
		clear_match_music();
		run_mode = run_idle_mode;
		set_now_page(PAGE_Setting);
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-08-07
* �� �� ��: compute_end_cycle
* ����˵��: ���ݵ���ˣ�ת�ӣ�ǰ����Ȧ���������յ�Ȧ��
*
* ��    ��: 
*			cycles: ���������ת��Ȧ��
* �� �� ֵ:
*		����ˣ�ת�ӣ��յ����ڵ�Ȧ��
*********************************************************************************************************
*/
static uint16_t compute_end_loc_cycle(uint16_t cycles)
{
	uint16_t end_cycle = 0;
	uint16_t buf2 = (uint16_t)(get_current_handle_injector() >> 16);	/*	��ǰ�������λ�ã� ��λ��Ȧ��	*/
	uint16_t top_cycle = (handle_injector_t.TOP_LOC >>16);
	uint16_t bottom_cycle = (handle_injector_t.BOTTOM_LOC >>16);
	
	end_cycle = (uint16_t)(buf2 + cycles);
	MODE_PRINTF("current= %d, need= %d, end= %d\r\n", buf2, cycles, end_cycle);
	if((end_cycle <= top_cycle)&&(end_cycle > bottom_cycle))
		return end_cycle;
	else
		return top_cycle;
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-21
* �� �� ��: touch_btm_scan
* ����˵��: ģʽѡ��
*
* ��    ��: 
*			LCD_TOUCH_T * lcd_touch_btm�� ��ť�ṹ�����
      
* �� �� ֵ: 
*			PAGE_MODE_SELECT �û�ѡ���ģʽ
*********************************************************************************************************
*/
static PAGE_MODE_SELECT touch_btm_scan(LCD_TOUCH_T * lcd_touch_btm)
{
	static PAGE_MODE_SELECT last_mode_select = MODE_IDLE;
	PAGE_MODE_SELECT now_mode_select = MODE_IDLE;
	if((lcd_touch_btm->page_id == PAGE_Mode) && (lcd_touch_btm->touch_wait_read)){
		lcd_touch_btm->touch_wait_read = 0;
		switch(lcd_touch_btm->touch_id){
			case PAGE_MODE_FUNC_ID:
				switch(lcd_touch_btm->touch_state){
					case 0x01:
						stop_dosage_mode();
						now_mode_select = MODE_NORMAL;			/*	���Normal��ťΪ����״̬	*/
						if(now_display_page != PAGE_Mode)
							set_now_page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x02:
						stop_dosage_mode();
						now_mode_select = MODE_FAST;			/*	���FAST��ťΪ����״̬	*/
						if(now_display_page != PAGE_Mode)
							set_now_page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x04:
						stop_dosage_mode();
						now_mode_select = MODE_PDL;				/*	���PDL��ťΪ����״̬	*/
						if(now_display_page != PAGE_Mode)
							set_now_page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x08:
						stop_dosage_mode();
						now_mode_select = MODE_DOSAGE;			/*	���DOSAGE��ť�����£��Զ���ת������ע�����	*/
						now_display_page = PAGE_Mode2;
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						DOSAGE_FLAG = (last_mode_select == MODE_DOSAGE ? true: false);
						show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
						return last_mode_select;			
					
					case 0x10:
						stop_dosage_mode();
						now_mode_select = MODE_CHANGE_CARTRIDGE;/*	����ҩƿ��ť����	*/
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x20:
						stop_dosage_mode();
						last_mode_select = MODE_IDLE;			/*	���Setting��ť������	*/
						return MODE_SETTING;
						
					default: break;
				}
				return MODE_IDLE;
					
			case PAGE_MODE_ASP_ID:								/*	���ASP��ť(����)Ϊ����״̬	*/
				if(lcd_touch_btm->touch_state)
					open_asp();
				else
					close_asp();
				return MODE_KEEP;
			
			case PAGE_MODE_ADJECT_DOSE_ID:						/*	����ע�䰴ť��ѡ��	*/
				show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x08);
				DOSAGE_FLAG = true;
				fix_dose = ADJUST_DOSE[lcd_touch_btm->touch_state];
				motor_to_goal_cycle = compute_end_loc_cycle(fix_dose);
				motor_run_direction = MOTOR_FWD_TO_GOAL;		/*	���MOTOR_FWD_GOAL��־	*/
				return MODE_DOSAGE;
				
			default: return MODE_KEEP;
		}
	}else
		return MODE_KEEP;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-24
* �� �� ��: FootPedal_Scan
* ����˵��: ��̤ɨ��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	FOOT_PEDAL_STATE_T result = FP_IDLE;
	
	fp_vol = (fp_vol > foot_pedal_t.MAX) ? foot_pedal_t.MAX : fp_vol;
	
	if(fp_vol < 100.0f)
		result = FP_NO_CONNECT;											/*	С��x00mV����Ϊ��̤δ����	*/
	else if(fp_vol < (foot_pedal_t.MIN + foot_pedal_t.QUARTER_DELTA))
		result = FP_CONNECTED;											/*	С��min+1*Delta,��Ϊ��̤����	*/
	else if(fp_vol < (foot_pedal_t.MIN + 2*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_1;											/*	С��min+2*Delta,��Ϊ��̤����1	*/
	else if(fp_vol < (foot_pedal_t.MIN + 3*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_2;											/*	С��min+3*Delta,��Ϊ��̤����2	*/
	else
		result = FP_DELTA_3;											/*	����min+3*Delta,��Ϊ��̤����3	*/
	
	if(last_fp_state != result){
		switch(result){
			case FP_NO_CONNECT: 
				show_variable_icon(PAGE_Mode, PAGE_MODE_FP_ID, 0);
			break;
			
			case FP_CONNECTED: 
				show_variable_icon(PAGE_Mode, PAGE_MODE_FP_ID, 1);
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:
			case FP_DELTA_3:
				show_variable_icon(PAGE_Mode, PAGE_MODE_FP_ID, 2);
			break;
			
			default: break;
		}
	}
	
	last_fp_state = result;
	return result;
}
/*-----------------------------------------------------------��������ģʽ----------------------------------------------------------------*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: run_normal_mode
* ����˵��: mode�����£�normal ģʽ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_normal_mode(void)
{
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}
	
	/*	2. ���ݵ�ѹ���е���	*/
	match_music_t.SPEED = true;
	motor_speed_adjust_accordingto_fp(MODE_NORMAL);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: run_fast_mode
* ����˵��: mode�����£�fast ģʽ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_fast_mode(void)
{
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}

	/*	2. ���ݵ�ѹ���е���	*/
	match_music_t.SPEED = true;
	motor_speed_adjust_accordingto_fp(MODE_FAST);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: run_pdl_mode
* ����˵��: mode�����£�pdl ģʽ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_pdl_mode(void)
{
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}
	
	/*	2. ѹ������		*/
	match_music_t.SPEED = true;
	motor_speed_adjust_accordingto_fp(MODE_PDL);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-08-06
* �� �� ��: run_dose_mode
* ����˵��: mode�����£�dosage ģʽ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_dosage_mode(void)
{
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if((mode_m_change_dir_t == MODE_M_FWD_GOAL)||(mode_m_change_dir_t == MODE_M_NEED_REV)){
		fix_dose = 0;
		show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}

	/*	2. ���ݵ�ѹ���е���	*/
	if((DOSAGE_FLAG == true)&&(fix_dose > 0)){
		match_music_t.SPEED = true;
		motor_speed_adjust_accordingto_fp(MODE_DOSAGE);
	}else{
		match_music_t.SPEED = false;
	}
	
}
//	���dosageģʽ���еı�־λ�ͼ���
static void stop_dosage_mode(void)
{
	set_motor_stop();
	fix_dose = 0;
	DOSAGE_FLAG = false;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: run_change_cartridge
* ����˵��: mode�����£�����ҩƿģʽ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_change_cartridge(void)
{
	show_variable_icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	�ٶ�����	*/
//	show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x10);		/*	ʧ�����й���	*/
	show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);	/*	��������	*/
	xSemaphoreGive(sMotor_goOrigin);							/*	֪ͨ�������	*/
	first_run_idle = false;
	run_mode = run_idle_mode;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: run_idle_mode
* ����˵��: mode�����£�idle ģʽ
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_idle_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	if(first_run_idle == true){
		first_run_idle = false;
		clear_match_music();
		if(DOSAGE_FLAG == true)
			write_two_82H_cmd((PAGE_Mode<<8)+PAGE_MODE_SPEED_ID, 0, 0x08);	/*	�ٶ�����, ʧ�����й��ܳ��ˡ�����ע�䡱	*/
		else
			write_two_82H_cmd((PAGE_Mode<<8)+PAGE_MODE_SPEED_ID, 0, 0x00);	/*	�ٶ�����, ʧ�����й���	*/
	}
	
	if(motor_run_direction == MOTOR_TOP){	//	��ǰ��ע����
		if((fp_state_t == FP_CONNECTED)&&((last_fp_state == FP_DELTA_1)||(last_fp_state == FP_DELTA_2)||(last_fp_state == FP_DELTA_3)))
			xSemaphoreGive(sMotor_goOrigin);
		else
			;
		last_fp_state = fp_state_t;
	}else
		return ;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-07-13
* �� �� ��: Mode_Position_Compare
* ����˵��: �жϵ���Ƿ���Ҫ����
*
* ��    ��: 
*			motor_dir_t: ��ǰ����ķ���
      
* �� �� ֵ:
*			MODE_M_CHANGE_DIR_T �����Ҫ�л��ķ���
*********************************************************************************************************
*/
static MODE_M_CHANGE_DIR_T Mode_Position_Compare(MOTOR_DIRECTION_T motor_dir_t)
{
	switch(motor_dir_t){
		case MOTOR_TOP: return MODE_M_NEED_REV;		
		
		case MOTOR_ORIGIN: return MODE_M_NEED_FWD;
		
		case MOTOR_FWD_GOAL: return MODE_M_FWD_GOAL;
		
		default: return MODE_M_NEED_IDLE;
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: motor_speed_adjust_accordingto_fp
* ����˵��: ���ݽ�̤�����ĵ�ѹ���е���ٶȵ���
*
* ��    ��: 
*			page_mode_select ��ǰѡ���ģʽ      
* �� �� ֵ:
*			
*********************************************************************************************************
*/
static void motor_speed_adjust_accordingto_fp(PAGE_MODE_SELECT page_mode_select)
{
	uint8_t index = 0;
	static SET_SPEED_T set_speed_t[3] = {0};
	static PAGE_MODE_SELECT last_page_mode_select = MODE_IDLE;
	
	/*	1. ���ݲ�ͬģʽ���ò�ͬ�ٶ�����	*/
	switch(page_mode_select){
		case MODE_DOSAGE:
		case MODE_NORMAL:	index = 0;	break;
		
		case MODE_FAST:		index = 1;	break;
		
		case MODE_PDL: 		index = 2;	break;
		
		default:	return;
	}
	
	set_speed_t[index].last_speed = set_speed_t[index].now_speed;
	/*	2. ���ݲ�ͬ��̤��ѹ��ȡ���ò�ͬ�ٶȵȼ�	*/
	switch(fp_state_t){
		case FP_DELTA_1:	set_speed_t[index].now_speed = injector_speed_t[index][0];	break;
		
		case FP_DELTA_2:	set_speed_t[index].now_speed = injector_speed_t[index][1];	break;
		
		case FP_DELTA_3:	set_speed_t[index].now_speed = injector_speed_t[index][2];	break;
		
		default: 			set_speed_t[index].now_speed = 0;	break;
	}
	
	/*	3. ���Ƶ����ת	*/
	if(set_speed_t[index].now_speed != set_speed_t[index].last_speed){ // ������������ٶȲ�ͬ�����������õ��ת��
		set_speed_mode_speed(set_speed_t[index].now_speed, (page_mode_select == MODE_DOSAGE ? MOTOR_FWD_TO_GOAL: MOTOR_FWD));
		
		if(set_speed_t[index].now_speed == injector_speed_t[1][0]){
			show_variable_icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);
			MUSIC_F_100MS = 9;
			SPEED_INDEX = LOW_INJECTION_INDEX;
			
		}else if(set_speed_t[index].now_speed == injector_speed_t[1][1]){
			show_variable_icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);
			MUSIC_F_100MS = 7;
			SPEED_INDEX = MIDDLE_INJECTION_INDEX;
			
		}else if(set_speed_t[index].now_speed == injector_speed_t[1][2]){
			show_variable_icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 3);
			MUSIC_F_100MS = 5;
			SPEED_INDEX = HIGH_INJECTION_INDEX;
			
		}else{
			show_variable_icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);
			MUSIC_F_100MS = 0;
			SPEED_INDEX = 0;
		}
	}
	
	/*	4. ��������:����ע���޴˹��ܣ�����ģʽ�л�ʱҲ��ִ�д˹���		*/
	if((page_mode_select != MODE_DOSAGE)&&(last_page_mode_select == page_mode_select))
		motor_asp_response(set_speed_t[index]);

	if(last_page_mode_select != page_mode_select){
		set_speed_t[0].now_speed = 0;
		set_speed_t[1].now_speed = 0;
		set_speed_t[2].now_speed = 0;
	}
		
	
	/*	5. ���浱ǰģʽ	*/
	last_page_mode_select = page_mode_select;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-01-26
* �� �� ��: calu_dis_from_sp_oils
* ����˵��: ���㿪���������ܣ��ֱ�����4mm��Ӧ�ĵ��Ȧ��
*
* ��    ��: 
*			      
* �� �� ֵ:
*			�̶�ֵ˿�˻���4mm����Ӧ�������100Ȧ
*********************************************************************************************************
*/
int16_t calu_dis_from_sp_oils(SET_SPEED_T sp_t)
{ 
	return 100;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-01-26
* �� �� ��: motor_asp_response
* ����˵��: ������� aspirate ��Ӧ
*
* ��    ��: 
*			sp_t ֮ǰ�ٶȺ͵�ǰ�ٶ�      
* �� �� ֵ:
*			
*********************************************************************************************************
*/
static void motor_asp_response(SET_SPEED_T sp_t)
{
	if(is_asp_open() == true)
		if((sp_t.last_speed >0)&&(sp_t.now_speed ==0)){		/*	֮ǰ�ٶȴ���0����ǰ�ٶ�Ϊ0�����ֹͣ��*/
			set_motor_stop();
			osDelay(200);
			asp_cycle = calu_dis_from_sp_oils(sp_t);
			xSemaphoreGive(sMotor_ASP);
			match_music_t.SPEED = false;
		}else
			return ;
	else if(sp_t.now_speed ==0){							/*	δ����asp, ��ǰ�ٶ�Ϊ0�����ֹͣ��*/
		set_motor_stop();
		match_music_t.SPEED = false;
	}else
		return ;
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
		show_variable_icon(PAGE_Mode, PAGE_MODE_DOSE_ID, now_dose);
		if((now_dose == 6)&&(now_dose < last_dose))		//	now_dose =6 �Ҵ���ǰ��������
			match_music_t.REMAIN_LOW = true;
		else if((now_dose == 1)&&(now_dose < last_dose))//	now_dose =1 �Ҵ���ǰ��������
			match_music_t.REMAIN_EMPTY = true;
		else
			;
	}else
		return ;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-03-15
* �� �� ��: show_oil_pressure
* ����˵��: ��ʾҺѹ: ��Һѹ����ʱ�ᷢ����ʾ��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void show_oil_pressure(uint16_t oil_pressure)
{
	static uint8_t last_index, now_index;
	int32_t now_std_g = current_g_from_kPa(oil_pressure);
	
	last_index = now_index;
	if(now_std_g <= 2000)
		now_index = 0;
	else if(now_std_g <= 3000)
		now_index = 1;
	else if(now_std_g <= 4000)
		now_index = 2;
	else if(now_std_g <= 5000)
		now_index = 3;
	else if(now_std_g <= 6000)	//	PDL 1
		now_index = 4;	
	else if(now_std_g <= 7300)	//	PDL 2
		now_index = 5;
	else if(now_std_g <= 8700)
		now_index = 6;
	else if(now_std_g <= 10000)
		now_index = 7;
	else if(now_std_g <= 12500)
		now_index = 8;
	else 						//	ѹ������
		now_index = 8;
	
	if(press_according_to_mode == MODE_PDL){
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index +PRES_PDL_START_INDEX: last_index);
		now_index += 9;
	}else
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index: last_index -PRES_PDL_START_INDEX);
	
	if(last_index != now_index){
		if((now_index - last_index) >= 2)	/*	�����䣬���Դ˴�����	*/
			now_index = last_index;
		else{
			write_two_82H_cmd((PAGE_Mode<<8)+PAGE_MODE_PRES_ID, now_index, oil_pressure);
			if((now_index == 14)||(now_index == 15)){
				match_music_t.PDL = true;
				match_music_t.OVERPRESSURE = false;
			}else if((now_index == 16)||(now_index == 17)){
				match_music_t.OVERPRESSURE = true;
				match_music_t.PDL = false;
			}else{
				match_music_t.PDL = false;
				match_music_t.OVERPRESSURE = false;
			}	
		}		
	}else{
		write_two_82H_cmd((PAGE_Mode<<8)+PAGE_MODE_PRES_ID, now_index, oil_pressure);
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-25
* �� �� ��: play_music_at_same_intervals
* ����˵��: �ȼ����������,�ڵ����� task ��ִ��,������������Ӱ��������
*
* ��    ��: 
*			f_100ms:		����Ƶ��,[20, 10, 5, 4, 2, 1]Hz
*			do_play_music: 	�Ƿ�ִ�д˺���
* �� �� ֵ: 
*********************************************************************************************************
*/
static void play_music_at_same_intervals(uint8_t f_100ms, bool do_play_music)
{
	static uint16_t ms100_cnt = 0;
	static uint8_t last_speed_index = 0;
	static bool first_play_pdl_voice = true;	//	flag of first play pdl voice
	static bool first_play_ovp_voice = true;	//	flag of first play overpressure voice
	
	if((do_play_music == false)||(f_100ms == 0))
		return ;
	else{
		if(++ms100_cnt >= f_100ms){
			ms100_cnt = 0;
			
			switch(music_according_to_mode){
				case MODE_PDL:	/*	(REMAIN_LOW||REMAIN_EMPTY) > PDL_INDEX > ALARM_INDEX > SPEED_INDEX	*/
					if(is_train_mode_open() == true){				//	����ѵ��ģʽ
						if(match_music_t.REMAIN_LOW == true){
							play_now_music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_LOW = false;
							osDelay(1200);
						}else if(match_music_t.REMAIN_EMPTY == true){
							play_now_music(CHANGE_CARTRIDGE_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_EMPTY = false;
							osDelay(1500);
						}else if(match_music_t.PDL == true){
							play_now_music(REACH_PDL_INDEX, system_volume[get_system_vol()]);			//	PDL��ʾ
							osDelay(1000);
						}else if(match_music_t.OVERPRESSURE == true){
							play_now_music(OVER_PRESSURE_INDEX, system_volume[get_system_vol()]);		//	��ѹ��ʾ
							osDelay(800);
						}else if(match_music_t.SPEED == true)
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);				//	��ѹע����
						else
							;
					}else if(is_smart_reminder_open() == true){		//	������������ģʽ
						if(match_music_t.REMAIN_LOW == true){
							play_now_music(REMAIN_LOW_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_LOW = false;
							osDelay(1000);
						}else if(match_music_t.REMAIN_EMPTY == true){
							play_now_music(CARTRIDGE_EMPTY_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_EMPTY = false;
							osDelay(1200);
						}else if(match_music_t.PDL == true){
							if(first_play_pdl_voice == true){
								play_now_music(PDL_INDEX, system_volume[get_system_vol()]);				//	PDL��ʾ
								osDelay(150);
								first_play_pdl_voice = false;
							}else
								play_now_music(MIDDLE_INJECTION_INDEX, system_volume[get_system_vol()]);
							first_play_ovp_voice = true;
						}else if(match_music_t.OVERPRESSURE == true){
							if(first_play_ovp_voice == true){
								play_now_music(OVER_PRESSURE_INDEX, system_volume[get_system_vol()]);	//	��ѹ��ʾ
								osDelay(150);
								first_play_ovp_voice = false;
							}else	
								play_now_music(ALARM_INDEX, system_volume[get_system_vol()]);
							first_play_pdl_voice = true;
						}else if(match_music_t.SPEED == true){
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);				//	��ѹע����
							first_play_pdl_voice = true;
							first_play_ovp_voice = true;
						}else{
							first_play_pdl_voice = true;
							first_play_ovp_voice = true;
						}
					}else{											//	���ڳ���ģʽ
						if(match_music_t.PDL == true){
							play_now_music(MIDDLE_INJECTION_INDEX, system_volume[get_system_vol()]);	//	PDL��ʾ
						}else if(match_music_t.OVERPRESSURE == true){
							play_now_music(ALARM_INDEX, system_volume[get_system_vol()]);				//	��ѹ��ʾ
						}else if(match_music_t.SPEED == true)
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);				//	��ѹע����
						else
							;
					}
				break;
				
				case MODE_NORMAL: case MODE_FAST:	//	(REMAIN_LOW||REMAIN_EMPTY) > SPEED
					if(is_train_mode_open() == true){				//	����ѵ��ģʽ
						if(match_music_t.REMAIN_LOW == true){
							play_now_music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_LOW = false;
							osDelay(1200);
						}else if(match_music_t.REMAIN_EMPTY == true){
							play_now_music(CHANGE_CARTRIDGE_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_EMPTY = false;
							osDelay(1500);
						}else if(match_music_t.SPEED == true){
							if(last_speed_index != SPEED_INDEX){
								switch(SPEED_INDEX){
									case LOW_INJECTION_INDEX:
										play_now_music(LOW_SPEED_INDEX, system_volume[get_system_vol()]);
										osDelay(50);
										break;
									case MIDDLE_INJECTION_INDEX:
										play_now_music(MIDDLE_SPEED_INDEX, system_volume[get_system_vol()]);
										osDelay(150);
										break;
									case HIGH_INJECTION_INDEX:
										play_now_music(HIGH_SPEED_INDEX, system_volume[get_system_vol()]);
										osDelay(250);
										break;
									default: break;
								}
							}else
								play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);
							last_speed_index = SPEED_INDEX;
						}else
						;
					}else if(is_smart_reminder_open() == true){		//	������������ģʽ
						if(match_music_t.REMAIN_LOW == true){
							play_now_music(REMAIN_LOW_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_LOW = false;
							osDelay(1000);
						}else if(match_music_t.REMAIN_EMPTY == true){
							play_now_music(CARTRIDGE_EMPTY_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_EMPTY = false;
							osDelay(1200);
						}else if(match_music_t.SPEED == true){
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);
						}else
						;
					}else{											//	���ڳ���ģʽ
						if(match_music_t.REMAIN_LOW == true){
							play_now_music(REMAIN_INDEX, system_volume[get_system_vol()]);
							osDelay(300);
							match_music_t.REMAIN_LOW = false;
						}else if(match_music_t.REMAIN_EMPTY == true){
							play_now_music(REMAIN_INDEX, system_volume[get_system_vol()]);
							osDelay(300);
							match_music_t.REMAIN_EMPTY = false;
						}else if(match_music_t.SPEED == true){
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);
						}else
						;
					}
					match_music_t.PDL = false;
					match_music_t.OVERPRESSURE = false;
				break;
				
				default: break;
			}
		}
	}	
}

void page_mode_alarm(void)
{
	play_music_at_same_intervals(MUSIC_F_100MS, DO_PLAY_MUSIC);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-27
* �� �� ��: clear_match_music
* ����˵��: ������ֲ��ű�־λ
*
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
static void clear_match_music(void)
{
	music_according_to_mode = MODE_IDLE;	/*	��ǰ����IDLEģʽ	*/
	MUSIC_F_100MS = 0;						/*	�޲���Ƶ��	*/
	match_music_t.PDL 			= false;
	match_music_t.OVERPRESSURE 	= false;
	match_music_t.PRESSURE 		= false;
	match_music_t.SPEED 		= false;
	match_music_t.ASP 			= false;
	match_music_t.REMAIN_LOW 	= false;
	match_music_t.REMAIN_EMPTY 	= false;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-08-01
* �� �� ��: ���ܰ�����������
* ����˵��: 
*
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
static void play_mode_choice_voice(PAGE_MODE_SELECT mode_choice)
{
	if((is_train_mode_open() == true)||(is_smart_reminder_open() == true)){
		switch(mode_choice)
		{
			case MODE_NORMAL:
				play_now_music(NORMAL_MODE_INDEX, system_volume[get_system_vol()]);
			break;
			
			case MODE_FAST:
				play_now_music(FAST_MODE_INDEX, system_volume[get_system_vol()]);
			break;
			
			case MODE_PDL:
				play_now_music(PDL_MODE_INDEX, system_volume[get_system_vol()]);
			break;
			
			case MODE_SETTING:
				
			break;
			
			default: break;
		}
	}else
		return ;	
}
