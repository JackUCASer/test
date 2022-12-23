#include "Page_Mode.h"

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
#include "Hampel_Filter.h"

typedef struct{
	uint32_t last_speed;
	uint32_t now_speed;
}SET_SPEED_T;

typedef struct{
	bool PDL;			/*	PDL��������	*/
	bool OVERPRESSURE;	/*	over pressure ע���ѹ	*/
	bool REMAIN_LOW;	/*	ʣ��ҩ����	*/
	bool REMAIN_EMPTY;	/*	ʣ��ҩ����	*/
	bool SPEED;			/*	�ٶȸ���	*/
}MATCH_MUSIC_T;

typedef void (*DO_SELECTED_MODE)(void);	/*	����һ��ָ���������Ϊvoid���ͣ�����ֵΪvoid �ĺ���ָ������	*/
static DO_SELECTED_MODE run_mode = NULL;


//	0. ͨ�ú���ԭ������
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol, FOOT_PEDAL_STATE_T* last_fp_state);			//	��̤ɨ��
static PAGE_MODE_SELECT touch_btm_scan(LCD_TOUCH_T * lcd_touch_btm, PAGE_MODE_SELECT mode_select);		//	����ɨ��
static MODE_M_CHANGE_DIR_T Mode_Position_Compare(MOTOR_DIRECTION_T motor_dir_t);						//	���������
static void update_pressure_and_remain_dose(void);														//	Һѹ���
static void clear_music_flag(void);																		//	�����Ƶ��ʶ����������
static void motor_asp_response(int last_speed, int now_speed);											//	������Ӧ

//	1. IDLEģʽ
static void run_idle_mode(void);
static void idle_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* idle_speed);

//	2. Normal����ģʽ
static void run_normal_mode(void);
static void normal_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* normal_speed);
static void normal_tone_adjust_by_normal_speed(int normal_speed, int* last_normal_speed);

//	3. Fast����ģʽ
static void run_fast_mode(void);
static void fast_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* fast_speed);
static void fast_tone_adjust_by_fast_speed(int fast_speed, int* last_fast_speed);

//	4. PDLģʽ
static void run_pdl_mode(void);
static void pdl_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* pdl_speed);
static void pdl_tone_adjust_by_pdl_speed(int pdl_speed, int* last_pdl_speed);

//	5. Dosage����ע��ģʽ
static void run_dosage_mode(void);
static void dosage_mode_adjust_dose_init(void);
static uint16_t compute_end_loc_cycle(uint16_t cycles);
static void dosage_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* dosage_speed);
static void dosage_tone_adjust_by_dosage_speed(int dosage_speed, int* last_dosage_speed);
static void dosage_mode_compute_max_adjust_dose_index(void);
static void dosage_adjust_buttom_show_remain_dose(void);


//	6. ����ҩͲģʽ
static void run_change_cartridge_mode(void);


// �ⲿ����	
extern SemaphoreHandle_t 	sPage_mode;
extern SemaphoreHandle_t	sMotor_goOrigin2;
extern SemaphoreHandle_t	sMotor_ASP;
extern LCD_TOUCH_T 			lcd_touch_t;

//	ȫ�ֱ�־λ������
bool GO_TOP_FLAG = false;
bool GO_ORIGIN_FLAG = false;
bool ASP_RUNNING_FLAG = false;
int16_t asp_cycle = 0;

//	0. ͨ���ڲ�����
static bool FIRST_WORK_FLAG = false;
static MODE_M_CHANGE_DIR_T  mode_m_change_dir_t = MODE_M_NEED_IDLE;	/*	���״̬	*/
static PAGE_MODE_SELECT now_run_mode = MODE_IDLE;					/*	��ǰ���е�ģʽ*/

//	1. ��̤��ص��ڲ�����
static FOOT_PEDAL_STATE_T fp_state_t = FP_IDLE;						/*	��̤״̬	*/

//	2. ����ɨ����ص��ڲ�����
static PAGE_MODE_SELECT mode_selected = MODE_IDLE;					/*	����ɨ���������	*/

//	3. Һѹ��ص��ڲ�����


//	4. Idleģʽ��ص��ڲ�����


//	5. Normalģʽ��ص��ڲ�����

//	6. Fastģʽ��ص��ڲ�����

//	7. PDLģʽ��ص��ڲ�����
static bool PDL_AUTO_INJECTION = false;							/*	PDLģʽ�£��Զ�ע�书�ܴ���Ч��־��	*/
static bool PDL_AUTO_INJECTION_WORK = false;					/*	PDLģʽ�£��Զ�ע�书����Ч��־��	*/
static uint8_t auto_injection_step1_cnt = 0;					//	pdlģʽ�£��Զ�ע�����Ч�Ĵ�����صļ�����
static uint8_t auto_injection_step2_cnt = 0;					//	pdlģʽ�£��Զ�ע����Ч�Ĵ�����صļ�����

//	8. ����ע����ص��ڲ�����
static uint16_t FIX_DOSE = 0;//mL: 0  0.1 0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.0  1.1  1.2  1.3  1.4  1.5  1.6  1.7
static uint16_t ADJUST_DOSE[18] = {0, 63, 126, 189, 252, 315, 378, 441, 504, 567, 630, 693, 756, 819, 882, 945, 1008,1071};	/*	ע��̶�ml�����Ӧ��ת��Ȧ��	*/
static uint8_t MAX_ADJUST_DOSE_INDEX_OF_REMAIN = 17;		/*	����ʣ��ҩҺ������ѡ��������ע��������ʵʱ���£�	*/
static uint16_t FIX_DOSE_END_CYLCE = 0;						/*	����ע�����ʱ���Ӧ��λ��	*/

//	9. ��ҩƿ��ص��ڲ�����

//	10. ���ֲ������
static uint8_t MUSIC_F_50MS = 0;
static uint8_t SPEED_INDEX = 0;	
static MATCH_MUSIC_T match_music_t = {false};


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-12-20
* �� �� ��: Page_Mode
* ����˵��: mode�����µĳ���
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Page_Mode2(void)
{
	run_mode = run_idle_mode;
	fp_state_t = FP_IDLE;
	mode_selected = MODE_IDLE;
	if(xSemaphoreTake(sPage_mode, (TickType_t)0) == pdTRUE)	/*	��ȡ�ź����ɹ�	*/
	{
		run_mode = run_idle_mode;		//	����ģʽ����ָ��
		fp_state_t = FP_IDLE;			//	��ǰ��̤״̬
		mode_selected = MODE_IDLE;		//	����ɨ�践�ص�ѡ��ģʽ
		now_run_mode = MODE_IDLE;		//	��ǰ���е�ģʽ
		dosage_mode_adjust_dose_init();	//	����ע��������������ʼ��
		Init_ADC_IC();					//	��ʼ��CS1231Һѹ������
		clear_music_flag();				//	�����Ƶ��־��
		for(;;){
			osDelay(50);
			fp_state_t = FootPedal_Scan(Get_Foot_Pedal_Vol(), &fp_state_t);	//	��̤ɨ��
			mode_selected = touch_btm_scan(&lcd_touch_t, now_run_mode);
			update_pressure_and_remain_dose();
			if(mode_selected == MODE_SETTING){
				run_mode = run_idle_mode;
				break;	/*	����ѭ��	*/
			}else{
				switch(mode_selected){
					case MODE_KEEP: 
						break;
					
					case MODE_NORMAL: 
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_OTHER_START_INDEX);
						FIRST_WORK_FLAG = true;
						run_mode = run_normal_mode;
					break;
					
					case MODE_FAST: 
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_OTHER_START_INDEX);
						FIRST_WORK_FLAG = true;
						run_mode = run_fast_mode;
					break;
					
					case MODE_PDL:
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_PDL_START_INDEX);
						FIRST_WORK_FLAG = true;
						run_mode = run_pdl_mode;
					break;
					
					case MODE_DOSAGE:
						FIRST_WORK_FLAG = true;
						run_mode = run_dosage_mode;
					break;
					
					case MODE_CHANGE_CARTRIDGE:
						FIRST_WORK_FLAG = true;
						run_mode = run_change_cartridge_mode;
					break;
					
					case MODE_IDLE:
						set_motor_stop();
						FIRST_WORK_FLAG = true;
						run_mode = run_idle_mode;
					break;
					
					default:break;
				}
			}
			
			if(!ASP_RUNNING_FLAG) // ��ǰ���Ǵ��ڻ�������
				run_mode();
		}
		set_motor_stop();
		Clear_Lcd_Cmd_SendQueue();
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		Set_VGUS_Page(PAGE_Setting);
		Standby_ADC_IC();				//	ʧ��CS1231Һѹ������
		clear_music_flag();				//	�����Ƶ��־��
	}
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
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol, FOOT_PEDAL_STATE_T* last_fp_state)
{
	FOOT_PEDAL_STATE_T result = FP_IDLE;
	
	fp_vol = (fp_vol > foot_pedal_t.MAX) ? foot_pedal_t.MAX : fp_vol;
	
	if(fp_vol < 100.0f)
		result = FP_NO_CONNECT;											/*	С��100mV����Ϊ��̤δ����	*/
	else if(fp_vol < (foot_pedal_t.MIN + 2*foot_pedal_t.QUARTER_DELTA))
		result = FP_CONNECTED;											/*	С��min+2*Delta,��Ϊ��̤����	*/
	else if(fp_vol < (foot_pedal_t.MIN + 3.2f*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_1;											/*	С��min+3.2*Delta,��Ϊ��̤����1	*/
	else if(fp_vol < (foot_pedal_t.MIN + 3.6f*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_2;											/*	С��min+3.6*Delta,��Ϊ��̤����2	*/
	else
		result = FP_DELTA_3;											/*	����min+3.6*Delta,��Ϊ��̤����3	*/
	
	if(*last_fp_state != result){	//	��̤ͼ�����
		switch(result){
			case FP_NO_CONNECT: 
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FP_ID, 0);
			break;
			
			case FP_CONNECTED: 
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FP_ID, 1);
			break;
			
			case FP_DELTA_1: case FP_DELTA_2: case FP_DELTA_3:
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FP_ID, 2);
			break;
			
			default: break;
		}
	}
	
	*last_fp_state = result;		//	��¼��̤״̬
	return result;					//	���ص�ǰ��̤״̬

}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-12-20
* �� �� ��: touch_btm_scan
* ����˵��: ģʽѡ��
*
* ��    ��: 
*			LCD_TOUCH_T * lcd_touch_btm�� ��ť�ṹ�����
      
* �� �� ֵ: 
*			PAGE_MODE_SELECT �û�ѡ���ģʽ
*********************************************************************************************************
*/
static PAGE_MODE_SELECT touch_btm_scan(LCD_TOUCH_T * lcd_touch_btm, PAGE_MODE_SELECT mode_select)
{
	PAGE_MODE_SELECT now_mode_select = MODE_IDLE;
	PAGE_MODE_SELECT last_mode_select = MODE_IDLE;
	
	last_mode_select = mode_select;
	if((lcd_touch_btm->page_id == PAGE_Mode) && (lcd_touch_btm->touch_wait_read)){
		lcd_touch_btm->touch_wait_read = 0;
		switch(lcd_touch_btm->touch_id){
			case PAGE_MODE_ASP_ID:			//	0x01			/*	���ASP��ť(����)Ϊ����״̬	*/
				vgus_page_objs[PAGE_Mode].obj_value[0] = lcd_touch_btm->touch_state;	//	��¼��ť�ؼ�ֵ��������Ļ��λ�ָ�
				if(lcd_touch_btm->touch_state)
					Open_Asp();
				else
					Close_Asp();
				return MODE_KEEP;
			
			case PAGE_MODE_FUNC_ID:			//	0x07
				vgus_page_objs[PAGE_Mode].obj_value[1] = lcd_touch_btm->touch_state;	//	��¼��ť�ؼ�ֵ��������Ļ��λ�ָ�
				switch(lcd_touch_btm->touch_state){
					case 0x01:		//	����ģʽ��ť
						now_mode_select = MODE_NORMAL;			/*	���Normal��ťΪ����״̬	*/
						if(now_display_page != PAGE_Mode)
							Set_VGUS_Page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x02:		//	����ģʽ��ť
						now_mode_select = MODE_FAST;			/*	���FAST��ťΪ����״̬	*/
						if(now_display_page != PAGE_Mode)
							Set_VGUS_Page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x04:		//	PDLģʽ��ť
						now_mode_select = MODE_PDL;				/*	���PDL��ťΪ����״̬	*/
						if(now_display_page != PAGE_Mode)
							Set_VGUS_Page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x08:		//	����ע�䰴ť
						Clear_Lcd_Cmd_SendQueue();
						now_mode_select = MODE_DOSAGE;			/*	���DOSAGE��ť�����£��Զ���ת������ע�����	*/
						now_display_page = PAGE_Mode2;			//	�ڶ�����ֵ��ǰ��ʾ���棬��Ϊ��������л�����Ļ�Լ��л���
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						FIX_DOSE = 0;
						FIX_DOSE_END_CYLCE = compute_end_loc_cycle(FIX_DOSE);
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);	//	�õ�ǰ������ʾ0
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		//	�ٶ�����	
						return last_mode_select;			
					
					case 0x10:		//	����ҩͲ��ť
						now_mode_select = MODE_CHANGE_CARTRIDGE;/*	����ҩƿ��ť����	*/
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x20:		//	���ð�ť
						last_mode_select = MODE_IDLE;			/*	���Setting��ť������	*/
						return MODE_SETTING;
						
					default: break;
				}
				return MODE_IDLE;

			case PAGE_MODE_ADJECT_DOSE_ID:	//	0x08			/*	����ע��ĵ���ע�䰴ť��ѡ��	*/
				if(lcd_touch_btm->touch_state > MAX_ADJUST_DOSE_INDEX_OF_REMAIN){
					lcd_touch_btm->touch_state = MAX_ADJUST_DOSE_INDEX_OF_REMAIN;
					Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, lcd_touch_btm->touch_state);
				}
				vgus_page_objs[PAGE_Mode].obj_value[2] = lcd_touch_btm->touch_state;	//	��¼��ť�ؼ�ֵ��������Ļ��λ�ָ�
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x08);					//	������ע�䡱���ܰ�ť��ʾ������
				FIX_DOSE = ADJUST_DOSE[lcd_touch_btm->touch_state];
				motor_to_goal_cycle = compute_end_loc_cycle(FIX_DOSE);
				FIX_DOSE_END_CYLCE = motor_to_goal_cycle;
				motor_run_direction = MOTOR_FWD_TO_GOAL;
				return MODE_DOSAGE;
				
			default: return MODE_KEEP;
		}
	}else
		return MODE_KEEP;
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
	current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	now_dose = Compute_Reamin_Dose(current_pos) / 3;	/*	��100 �� 3 �ȼ�໮��	*/
	now_dose = (now_dose > 30 ? 30 : now_dose);
	
	if(last_dose != now_dose){
		Show_Variable_Icon(PAGE_Mode, PAGE_MODE_DOSE_ID, now_dose);
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
	
	if(wait_send_over == 1)
		return ;
	
	int32_t now_std_g = Get_Current_g_From_kPa(oil_pressure);
	
	last_index = now_index;
	if(now_std_g <= 2000)
		now_index = 0;
	else if(now_std_g <= 3000)
		now_index = 1;
	else if(now_std_g <= 4000)
		now_index = 2;
	else if(now_std_g <= 5000)
		now_index = 3;
	else if(now_std_g <= 6000)	
		now_index = 4;	
	else if(now_std_g <= 8000)	//	PDL 1	
		now_index = 5;
	else if(now_std_g <= 10000)	//	PDL 2
		now_index = 6;
	else if(now_std_g <= 12000)	//	PDL 3
		now_index = 7;
	else 						//	ѹ������
		now_index = 8;
	
	
	if(now_run_mode == MODE_PDL){
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index +PRES_PDL_START_INDEX: last_index);
		now_index += 9;
	}else
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index: last_index -PRES_PDL_START_INDEX);
	
	if(last_index != now_index){
		if((now_index - last_index) >= 2)	/*	�����䣬���Դ˴�����	*/
			now_index = last_index;
		else{
			Write_Two_82H_Cmd((PAGE_Mode<<8)+PAGE_MODE_PRES_ID, now_index, oil_pressure);
			if((now_index == 14)||(now_index == 15)||(now_index == 16)){
				match_music_t.PDL = true;
				match_music_t.OVERPRESSURE = false;
			}else if(now_index == 17){
				match_music_t.OVERPRESSURE = true;
				match_music_t.PDL = false;
			}else{
				match_music_t.PDL = false;
				match_music_t.OVERPRESSURE = false;
			}	
		}		
	}else{
		Write_Two_82H_Cmd((PAGE_Mode<<8)+PAGE_MODE_PRES_ID, now_index, oil_pressure);
	}
}
//	����Һѹ��ʣ��ҩ��
static void update_pressure_and_remain_dose(void)
{
	static uint8_t cnt = 0;
	if(Get_Oil_Pressure(&dynamic_pressure) == New_Data_Read_Over){			
		dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
		dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa
		dynamic_pressure_kPa_F = Hampel_Filter((uint16_t)dynamic_pressure_kPa, 3);
#if kPa_DEBUG == 1
		show_oil_pressure(dynamic_pressure_kPa_F);	// ��ʾѹ��
#else
		if((dynamic_pressure_kPa_F < 100) || (handle_in_origin == true))
			show_oil_pressure(100);						// ��ʾ��׼����ѹ��
		else
			show_oil_pressure(dynamic_pressure_kPa_F);	// ��ʾѹ��
#endif
	}
	if(++cnt > 10){
		if((!ASP_RUNNING_FLAG)&&(!GO_ORIGIN_FLAG))		// ���� �� ��ҩƿ ���̣�ҩҺ��������
				show_remain_dose();	// ��ʾʣ��ҩ��
		cnt = 0;
	}
}

//	�����Ƶ��־��(��������)
static void clear_music_flag(void)
{	
	match_music_t.PDL = false;
	match_music_t.OVERPRESSURE = false;
	match_music_t.REMAIN_LOW = false;
	match_music_t.REMAIN_EMPTY = false;
	match_music_t.SPEED = false;
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
static int16_t calu_dis_from_sp_oils(int last_speed, int now_speed)
{ 
	return 100;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-12-22
* �� �� ��: motor_asp_response
* ����˵��: ������� aspirate ��Ӧ
*
* ��    ��: 
*			sp_t ֮ǰ�ٶȺ͵�ǰ�ٶ�      
* �� �� ֵ:
*			
*********************************************************************************************************
*/
static void motor_asp_response(int last_speed, int now_speed)
{
	if(Is_Asp_Open() == true){
		if((last_speed >0)&&(now_speed ==0)){		/*	֮ǰ�ٶȴ���0����ǰ�ٶ�Ϊ0�����ֹͣ��*/
			asp_cycle = calu_dis_from_sp_oils(last_speed, now_speed);
			xSemaphoreGive(sMotor_ASP);
		}else
			return ;
	}else
		return ;
}
/*-------------------------------------------------------��������ģʽ---------------------------------------------------------------*/
//----------------------------------------------------------------IDLEģʽ��صĺ����������ʵ��----------------------------------------------------------
static void run_idle_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int idle_speed = 0;
	static int last_idle_speed = 0;
	
	//	1. ʧ�����й���
	if(FIRST_WORK_FLAG == true){
		Write_Two_82H_Cmd((PAGE_Mode<<8)+PAGE_MODE_SPEED_ID, 0, 0x00);	/*	�ٶ�����, ʧ�����й���	*/
		FIRST_WORK_FLAG = false;
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		FIX_DOSE = 0;
		
		idle_speed = 0;
		last_idle_speed = 0;
	}
	//	2. �ж��Ƿ���Ҫִ�и���ҩƿ����ǰ��ҩƿ��ע����
	if(motor_run_direction == MOTOR_TOP){						//	ҩҺע����
		idle_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &idle_speed);
		if((idle_speed == 0)&&(last_idle_speed > 0)){							
			xSemaphoreGive(sMotor_goOrigin2);	/*	֪ͨ�������	*/
		}
		last_idle_speed = idle_speed;
	}
	//	3. ���õ�ǰ����ģʽΪ��MODE_IDLE
	now_run_mode = MODE_IDLE;
}
//	Normalģʽ�µ��ٶȵ���:��Ҫ���ڸ���ҩƿ�ж�
static void idle_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* idle_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*idle_speed = 0;
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:
			case FP_DELTA_3:
				*idle_speed = injector_speed_t[0][1];	//	���⸳һ����0ֵ
			break;
		}
	}else
		return ;
}
//----------------------------------------------------------------Normalģʽ��صĺ����������ʵ��--------------------------------------------------------

static void run_normal_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int normal_speed = 0;
	static int last_normal_speed = 0;
	
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. ���ν���Normalģʽ
	if(FIRST_WORK_FLAG == true){
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		normal_speed = 0;
		last_normal_speed = 0;
		
		MUSIC_F_50MS = 0;
		SPEED_INDEX = 0;
		
		FIX_DOSE = 0;
		
		FIRST_WORK_FLAG = false;
	}
	
	//	3. �ٶ�/����/��������
	now_run_mode = MODE_NORMAL;
	normal_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &normal_speed);
	motor_asp_response(last_normal_speed, normal_speed);
	normal_tone_adjust_by_normal_speed(normal_speed, &last_normal_speed);
}

//	Normalģʽ�µ��ٶȵ���
static void normal_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* normal_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*normal_speed = 0;
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	�ٶ�����	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:	
				*normal_speed = injector_speed_t[0][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);		/*	�ٶ�����	*/
			break;
			
			case FP_DELTA_3:	
				*normal_speed = injector_speed_t[0][2];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);		/*	�ٶ�����	*/
			break;
		}
		set_speed_mode_speed(*normal_speed, MOTOR_FWD);
	}else
		return ;
}

//	Normalģʽ�µ���������
static void normal_tone_adjust_by_normal_speed(int normal_speed, int* last_normal_speed)
{
	static uint16_t cnt = 0;
	//	1. �����ٶ�ȷ����Ƶ������Ƶ��
	if(normal_speed != *last_normal_speed){			//	�������β�ѯ�ٶȲ�ͬ
		if(normal_speed == injector_speed_t[0][1]){
			MUSIC_F_50MS = 30;	//	1.5s���
			SPEED_INDEX = LOW_INJECTION_INDEX;
		}else if(normal_speed == injector_speed_t[0][2]){
			MUSIC_F_50MS = 20;	//	1s���
			SPEED_INDEX = MIDDLE_INJECTION_INDEX;
		}else{
			MUSIC_F_50MS = 0;
			SPEED_INDEX = 0;
			clear_music_flag();
		}
		*last_normal_speed = normal_speed;
		match_music_t.SPEED = true;	//	�ٶȸ���
		cnt = 0;
	}

	//	2. ������Ƶ
	if(MUSIC_F_50MS != 0){
		if(cnt % MUSIC_F_50MS == 4){
			if(Is_Train_Mode_Open() == true){								//	����ѵ��ģʽ
				if(match_music_t.REMAIN_LOW == true){
					Play_Now_Music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
				}else if(match_music_t.REMAIN_EMPTY == true){
					Play_Now_Music(CHANGE_CARTRIDGE_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
				}else if(match_music_t.SPEED == true){		//	�ٶ���������
					switch(SPEED_INDEX){
						case LOW_INJECTION_INDEX:
							Play_Now_Music(LOW_SPEED_INDEX, system_volume[Get_System_Vol()]);
							break;
						case MIDDLE_INJECTION_INDEX:
							Play_Now_Music(MIDDLE_SPEED_INDEX, system_volume[Get_System_Vol()]);
							break;
						default: break;
					}
					match_music_t.SPEED = false;
				}else										//	�ٶ���������
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
			}
			else if(Is_Smart_Reminder_Open() == true){						//	������������ģʽ
				if(match_music_t.REMAIN_LOW == true){
					Play_Now_Music(REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
				}else if(match_music_t.REMAIN_EMPTY == true){
					Play_Now_Music(CARTRIDGE_EMPTY_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
				}else
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
				match_music_t.SPEED = false;
			}
			else{															//	���ڳ���ģʽ		
				if(match_music_t.REMAIN_EMPTY == true)
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
				else if(match_music_t.REMAIN_LOW == true)
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
				else
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
				match_music_t.SPEED = false;
			}
		}
		cnt++;
	}else
		cnt = 0;
	
}
//----------------------------------------------------------------Fastģʽ��صĺ����������ʵ��----------------------------------------------------------

static void run_fast_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int fast_speed = 0;
	static int last_fast_speed = 0;
	
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. ���ν���Fastģʽ
	if(FIRST_WORK_FLAG == true){
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		fast_speed = 0;
		last_fast_speed = 0;
		
		MUSIC_F_50MS = 0;
		SPEED_INDEX = 0;
		
		FIX_DOSE = 0;
		
		FIRST_WORK_FLAG = false;
	}
	
	//	3. �ٶȺ���������
	now_run_mode = MODE_FAST;
	fast_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &fast_speed);
	motor_asp_response(last_fast_speed, fast_speed);
	fast_tone_adjust_by_fast_speed(fast_speed, &last_fast_speed);
}

//	Fastģʽ�µ��ٶȵ���
static void fast_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* fast_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*fast_speed = 0;
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	�ٶ�����	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:	
				*fast_speed = injector_speed_t[1][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);		/*	�ٶ�����	*/
			break;
			
			case FP_DELTA_3:	
				*fast_speed = injector_speed_t[1][2];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 3);		/*	�ٶ�����	*/
			break;
		}
		set_speed_mode_speed(*fast_speed, MOTOR_FWD);
	}else
		return ;
}

//	Fastģʽ�µ���������
static void fast_tone_adjust_by_fast_speed(int fast_speed, int* last_fast_speed)
{
	static uint16_t cnt = 0;
	if(fast_speed != *last_fast_speed){			//	�������β�ѯ�ٶȲ�ͬ
		if(fast_speed == injector_speed_t[1][1]){
			MUSIC_F_50MS = 20;
			SPEED_INDEX = MIDDLE_INJECTION_INDEX;
		}else if(fast_speed == injector_speed_t[1][2]){
			MUSIC_F_50MS = 15;
			SPEED_INDEX = HIGH_INJECTION_INDEX;
		}else{
			MUSIC_F_50MS = 0;
			SPEED_INDEX = 0;
			clear_music_flag();
		}
		*last_fast_speed = fast_speed;
		match_music_t.SPEED = true;	//	�ٶȸ���
		cnt = 0;
	}

	if(MUSIC_F_50MS != 0){
		if(cnt % MUSIC_F_50MS == 4){
			if(Is_Train_Mode_Open() == true){								//	����ѵ��ģʽ
				if(match_music_t.REMAIN_LOW == true){
					Play_Now_Music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
				}else if(match_music_t.REMAIN_EMPTY == true){
					Play_Now_Music(CHANGE_CARTRIDGE_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
				}else if(match_music_t.SPEED == true){		//	�ٶ���������
					switch(SPEED_INDEX){
						case HIGH_INJECTION_INDEX:
							Play_Now_Music(HIGH_SPEED_INDEX, system_volume[Get_System_Vol()]);
							break;
						case MIDDLE_INJECTION_INDEX:
							Play_Now_Music(MIDDLE_SPEED_INDEX, system_volume[Get_System_Vol()]);
							break;
						default: break;
					}
					match_music_t.SPEED = false;
				}else										//	�ٶ���������
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
			}
			else if(Is_Smart_Reminder_Open() == true){						//	������������ģʽ
				if(match_music_t.REMAIN_LOW == true){
					Play_Now_Music(REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
				}else if(match_music_t.REMAIN_EMPTY == true){
					Play_Now_Music(CARTRIDGE_EMPTY_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
				}else
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
				match_music_t.SPEED = false;
			}
			else{															//	���ڳ���ģʽ		
				if(match_music_t.REMAIN_EMPTY == true)
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
				else if(match_music_t.REMAIN_LOW == true)
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
				else
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
				match_music_t.SPEED = false;
			}
		}
		cnt++;
	}else
		cnt = 0;
	
}

//----------------------------------------------------------------PDLģʽ��صĺ����������ʵ��----------------------------------------------------------

static void run_pdl_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int pdl_speed = 0;
	static int last_pdl_speed = 0;
	
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		PDL_AUTO_INJECTION_WORK = false;
		PDL_AUTO_INJECTION = false;
		auto_injection_step1_cnt = 0;
		auto_injection_step2_cnt = 0;
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. ���ν���PDLģʽ
	if(FIRST_WORK_FLAG == true){
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		pdl_speed = 0;
		last_pdl_speed = 0;
		
		MUSIC_F_50MS = 0;
		SPEED_INDEX = 0;
		
		FIX_DOSE = 0;
		
		FIRST_WORK_FLAG = false;
		
		PDL_AUTO_INJECTION_WORK = false;
		PDL_AUTO_INJECTION = false;
		auto_injection_step1_cnt = 0;
		auto_injection_step2_cnt = 0;
		clear_music_flag();
	}
	
	//	3. �ٶȺ���������
	now_run_mode = MODE_PDL; 
	pdl_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &pdl_speed);
	if(PDL_AUTO_INJECTION_WORK == false)
		motor_asp_response(last_pdl_speed, pdl_speed);
	pdl_tone_adjust_by_pdl_speed(pdl_speed, &last_pdl_speed);
//	MODE_PRINTF("PDL_AUTO_INJECTION = %d, PDL_AUTO_INJECTION_WORK = %d\r\n", PDL_AUTO_INJECTION, PDL_AUTO_INJECTION_WORK);
}
//	PDLģʽ�µ��ٶȵ���
static void pdl_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* pdl_speed)
{	
	if((auto_injection_step2_cnt > 4)&&(PDL_AUTO_INJECTION_WORK == false)){
		PDL_AUTO_INJECTION = false;
		auto_injection_step1_cnt = 0;
	}
	
	if(fp_state_t != *last_fp_state){
		//	�Զ�ע�����ЧΪ�� && ��̤�ɿ�
		if((PDL_AUTO_INJECTION == true)&&((*last_fp_state == FP_DELTA_1)||(*last_fp_state == FP_DELTA_2)||(*last_fp_state == FP_DELTA_2))&&(fp_state_t == FP_CONNECTED)){
			//	�����ǰ����δ�Զ�ע���У����Զ�ע����Ч�������Զ�ע��ʧЧ,�����ؼĴ��������Ӧ������
			if(PDL_AUTO_INJECTION_WORK == false)	
				PDL_AUTO_INJECTION_WORK = true;
			else{
				PDL_AUTO_INJECTION_WORK = false;
				PDL_AUTO_INJECTION = false;
				auto_injection_step1_cnt = 0;
				auto_injection_step2_cnt = 0;
			}
		}
		
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*pdl_speed = 0;
				if(PDL_AUTO_INJECTION_WORK == false)
					Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);	/*	�ٶ�����	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:
			case FP_DELTA_3:
				*pdl_speed = injector_speed_t[2][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);		/*	��ʾ����	*/
			break;
		}
		if(PDL_AUTO_INJECTION_WORK == false)
			set_speed_mode_speed(*pdl_speed, MOTOR_FWD);
	}
	
}

//	PDLģʽ�µ���������
//	�Զ�ע��ԭ��
//	1. ����3���ٶ���ʾ�����豸�������Զ�ע��/Auto Injection��;
//	2. �ڽ�������3���ٶ���ʾ�����ɿ���̤����Ϊ�Զ�ע����Ч������3���ٶ���ʾ�����Զ�ע����Ч��
//	3. �Զ�ע����Ч�󣬲��½�̤���ɿ����Զ�ע��ʧЧ��
//	4. �Զ�ע��ʧЧ�����²��½�̤���ظ�����1��2�ٴ���Ч��
static void pdl_tone_adjust_by_pdl_speed(int pdl_speed, int* last_pdl_speed)
{
	static uint16_t cnt = 0;
	static bool first_play_pdl_voice = true;		//	��־�����״β��š�PDL������
	static bool first_play_ovp_voice = true;		//	��־�����״β��š���ѹ������
	
	static bool auto_inject_of_first_pdl = true;	//	���Զ�ע����صı�־�����״ν���"PDL"����
	static bool auto_inject_of_first_idle = true;	//	���Զ�ע����صı�־�����״ν���"̽��"��������ע�䣬����δ��PDL�͹�ѹ����
	
	if(pdl_speed != *last_pdl_speed){			//	�������β�ѯ�ٶȲ�ͬ
		if(pdl_speed == injector_speed_t[2][1]){
			MUSIC_F_50MS = 30;
			SPEED_INDEX = LOW_INJECTION_INDEX;
		}else{
			MUSIC_F_50MS = 0;
			SPEED_INDEX = 0;
			
		}
		*last_pdl_speed = pdl_speed;
		cnt = 0;
	}
	if(PDL_AUTO_INJECTION_WORK == true){
		MUSIC_F_50MS = 30;
		SPEED_INDEX = LOW_INJECTION_INDEX;
	}

	if(MUSIC_F_50MS != 0){
		if(cnt % MUSIC_F_50MS == 4){
			if(Is_Train_Mode_Open() == true){								//	����ѵ��ģʽ
				if(match_music_t.REMAIN_EMPTY == true){			//	ҩͲ��
					Play_Now_Music(CHANGE_CARTRIDGE_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					osDelay(1000);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.REMAIN_LOW == true){		//	������
					Play_Now_Music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					osDelay(1000);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.OVERPRESSURE == true){	//	��ѹ	
					Play_Now_Music(OVER_PRESSURE_INDEX, system_volume[Get_System_Vol()]);	//	��ѹ��������
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					osDelay(150);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.PDL == true){			//	PDL����
					//	1. �״ν���PDL�������Զ�ע��ʧЧ״̬: �����ؼĴ������������
					if((auto_inject_of_first_pdl == true)&&(PDL_AUTO_INJECTION_WORK == false)){	
						auto_inject_of_first_pdl = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. ���������1 ����3�������Զ�ע����ʾ�������򲥷�PDL�����ٶȣ���ʾ��
					if(auto_injection_step1_cnt == 3){	
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	�����Զ�ע����ʾ��
						PDL_AUTO_INJECTION = true;
					}else{
						Play_Now_Music(REACH_PDL_INDEX, system_volume[Get_System_Vol()]);	//	PDL��ʾ
						osDelay(150);
					}
					//	3. ����PDL_AUTO_INJECTION״̬���¼�����1��ֵ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. ����PDL_AUTO_INJECTION״̬���¼�����2��ֵ
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_idle= true;
				}else{											//	����ע�䣨̽�������У���Ѱ��PDL����
					//	1. �״ν���̽���������Զ�ע��ʧЧ״̬: �����ؼĴ������������
					if((auto_inject_of_first_idle == true)&&(PDL_AUTO_INJECTION_WORK == false)){
						auto_inject_of_first_idle = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. ���������1 ����3�������Զ�ע����ʾ�������򲥷�̽���׶�(�ٶ�)��ʾ��
					if(auto_injection_step1_cnt == 3){
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	�����Զ�ע����ʾ��
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
					//	3. ����PDL_AUTO_INJECTION״̬���¼�����1��ֵ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. ����PDL_AUTO_INJECTION״̬���¼�����2��ֵ
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_pdl = true;
				}
				match_music_t.SPEED = false;
				first_play_ovp_voice = true;
			}
			else if(Is_Smart_Reminder_Open() == true){						//	������������ģʽ
				if(match_music_t.REMAIN_EMPTY == true){			//	ҩͲ��
					Play_Now_Music(CARTRIDGE_EMPTY_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.REMAIN_LOW == true){		//	������
					Play_Now_Music(REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.OVERPRESSURE == true){	//	��ѹ
					first_play_pdl_voice = true;
					if(first_play_ovp_voice == true){
						Play_Now_Music(OVER_PRESSURE_INDEX, system_volume[Get_System_Vol()]);	//	��ѹ��ʾ
						osDelay(150);
						first_play_ovp_voice = false;
					}else	
						Play_Now_Music(ALARM_INDEX, system_volume[Get_System_Vol()]);
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.PDL == true){			//	PDL����
					first_play_ovp_voice = true;
					//	1. �״ν���PDL�������Զ�ע��ʧЧ״̬: �����ؼĴ������������
					if((auto_inject_of_first_pdl == true)&&(PDL_AUTO_INJECTION_WORK == false)){	
						auto_inject_of_first_pdl = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. ���������1 ����3�������Զ�ע����ʾ�������򲥷�PDL�����ٶȣ���ʾ��
					if(auto_injection_step1_cnt == 3){	
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	�����Զ�ע����ʾ��
						PDL_AUTO_INJECTION = true;
						osDelay(150);
					}else if(first_play_pdl_voice == true){
						Play_Now_Music(PDL_INDEX, system_volume[Get_System_Vol()]);	//	PDL��ʾ
						first_play_pdl_voice = false;
						osDelay(150);
					}else
						Play_Now_Music(MIDDLE_INJECTION_INDEX, system_volume[Get_System_Vol()]);	//	PDL��ʾ
					//	3. ����PDL_AUTO_INJECTION״̬���¼�����1��ֵ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. ����PDL_AUTO_INJECTION״̬���¼�����2��ֵ
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_idle= true;
				}else{											//	����ע�䣨̽����
					first_play_pdl_voice = true;
					first_play_ovp_voice = true;
					//	1. �״ν���̽���������Զ�ע��ʧЧ״̬: �����ؼĴ������������
					if((auto_inject_of_first_idle == true)&&(PDL_AUTO_INJECTION_WORK == false)){
						auto_inject_of_first_idle = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. ���������1 ����3�������Զ�ע����ʾ�������򲥷�̽���׶�(�ٶ�)��ʾ��
					if(auto_injection_step1_cnt == 3){
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	�����Զ�ע����ʾ��
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
					//	3. ����PDL_AUTO_INJECTION״̬���¼�����1��ֵ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. ����PDL_AUTO_INJECTION״̬���¼�����2��ֵ
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_pdl = true;
				}
				match_music_t.SPEED = false;
			}
			else{															//	���ڳ���ģʽ		
				if(match_music_t.REMAIN_EMPTY == true){			//	ҩͲ��
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.REMAIN_LOW == true){		//	������
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.OVERPRESSURE == true){	//	��ѹ
					Play_Now_Music(ALARM_INDEX, system_volume[Get_System_Vol()]);				//	��ѹ��ʾ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.PDL == true){			//	PDL����	
					//	1. �״ν���PDL�������Զ�ע��ʧЧ״̬: �����ؼĴ������������
					if((auto_inject_of_first_pdl == true)&&(PDL_AUTO_INJECTION_WORK == false)){	
						auto_inject_of_first_pdl = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. ���������1 ����3�������Զ�ע����ʾ�������򲥷�PDL�����ٶȣ���ʾ��
					if(auto_injection_step1_cnt == 3){	
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	�����Զ�ע����ʾ��
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(MIDDLE_INJECTION_INDEX, system_volume[Get_System_Vol()]);	//	PDL��ʾ
					//	3. ����PDL_AUTO_INJECTION״̬���¼�����1��ֵ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. ����PDL_AUTO_INJECTION״̬���¼�����2��ֵ
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_idle= true;
				}else{											//	����ע�䣨̽�������У���Ѱ��PDL����
					//	1. �״ν���̽���������Զ�ע��ʧЧ״̬: �����ؼĴ������������
					if((auto_inject_of_first_idle == true)&&(PDL_AUTO_INJECTION_WORK == false)){
						auto_inject_of_first_idle = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. ���������1 ����3�������Զ�ע����ʾ�������򲥷�̽���׶�(�ٶ�)��ʾ��
					if(auto_injection_step1_cnt == 3){
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	�����Զ�ע����ʾ��
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
					//	3. ����PDL_AUTO_INJECTION״̬���¼�����1��ֵ
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. ����PDL_AUTO_INJECTION״̬���¼�����2��ֵ
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_pdl = true;
				}
				match_music_t.SPEED = false;
				first_play_ovp_voice = true;
			}
		}
		cnt++;
	}else
		cnt = 0;
	
}
//----------------------------------------------------------------Dosageģʽ��صĺ����������ʵ��--------------------------------------------------------

static void run_dosage_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int dosage_speed = 0;
	static int last_dosage_speed = 0;
	
	/*	1. λ�û�ȡ	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if((mode_m_change_dir_t == MODE_M_FWD_GOAL)||(mode_m_change_dir_t == MODE_M_NEED_REV)){
		Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
		FIX_DOSE = 0;
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. ���ν���Dosageģʽ
	if(FIRST_WORK_FLAG == true){
		set_motor_stop();		//	���ֹͣ���������趨ע������󷽿��������
		
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		dosage_speed = 0;
		last_dosage_speed = 0;
		
		MUSIC_F_50MS = 0;
		SPEED_INDEX = 0;
		
		FIRST_WORK_FLAG = false;
	}
	
	//	3. �ٶȺ���������
	now_run_mode = MODE_DOSAGE;
	if(FIX_DOSE > 0){
		dosage_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &dosage_speed);
		dosage_tone_adjust_by_dosage_speed(dosage_speed, &last_dosage_speed);
		dosage_adjust_buttom_show_remain_dose();
	}else{
		last_fp_state = FP_IDLE;
		
		dosage_speed = 0;
		last_dosage_speed = 0;
		
		MUSIC_F_50MS = 0;
		SPEED_INDEX = 0;
	}
	dosage_mode_compute_max_adjust_dose_index();
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-8-31
* �� �� ��: adjust_dose_init
* ����˵��: ��ʼ������ע��Ĵ���
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void dosage_mode_adjust_dose_init(void)
{
	float per_dose = (handle_injector_t.DISTANCE >> 16)/17.0f;
	for(uint8_t index = 1; index < 17; index ++){
		ADJUST_DOSE[index] = (uint16_t)(per_dose * index);
	}
	ADJUST_DOSE[17] = (handle_injector_t.DISTANCE >> 16);
}

//	Dosageģʽ�µ��ٶȵ���
static void dosage_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* dosage_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*dosage_speed = 0;
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	�ٶ�����	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:	
				*dosage_speed = injector_speed_t[0][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);		/*	����	*/
			break;
			
			case FP_DELTA_3:	
				*dosage_speed = injector_speed_t[0][2];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);		/*	����	*/
			break;
		}
		set_speed_mode_speed(*dosage_speed, MOTOR_FWD_TO_GOAL);
	}else
		return ;
}

//	Dosageģʽ�µ���������
static void dosage_tone_adjust_by_dosage_speed(int dosage_speed, int* last_dosage_speed)
{
	static uint8_t cnt = 0;
	if(dosage_speed != *last_dosage_speed){			//	�������β�ѯ�ٶȲ�ͬ
		if(dosage_speed == injector_speed_t[0][1]){
			MUSIC_F_50MS = 30;
			SPEED_INDEX = LOW_INJECTION_INDEX;
		}else if(dosage_speed == injector_speed_t[0][2]){
			MUSIC_F_50MS = 20;
			SPEED_INDEX = MIDDLE_INJECTION_INDEX;
		}else{
			MUSIC_F_50MS = 0;
			SPEED_INDEX = 0;
		}
		*last_dosage_speed = dosage_speed;
		cnt = 0;
	}

	if(MUSIC_F_50MS != 0){
		if(cnt % MUSIC_F_50MS == 4){
			Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
			cnt = 4;
		}
		cnt++;
	}else
		cnt = 0;
	
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-12-01
* �� �� ��: compute_max_adjust_dose_index
* ����˵��: ����ע��ģʽ�и���ʣ��ҩ����������ע����
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void dosage_mode_compute_max_adjust_dose_index(void)
{
	uint16_t current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	uint16_t all_dis = (handle_injector_t.DISTANCE >> 16);
	uint16_t now_dis = Get_Current_Handle_Run_Distance(current_pos);
	uint16_t remain_dose = all_dis - now_dis;
	
	//	1. ����߽�
	if(remain_dose >= ADJUST_DOSE[17]){
		MAX_ADJUST_DOSE_INDEX_OF_REMAIN = 17;			//	��������
		return ;
	}else{
		for(uint8_t i=0; i<17; i++){
			if((remain_dose >= ADJUST_DOSE[i])&&(remain_dose < ADJUST_DOSE[i+1])){
				MAX_ADJUST_DOSE_INDEX_OF_REMAIN = i;	//	��������
				//MODE_PRINTF("all_dis= %d, now_dis= %d, and remain_dose= %d, remain_index= %d\r\n", all_dis, now_dis, remain_dose, MAX_ADJUST_DOSE_INDEX_OF_REMAIN);
				return ;
			}else
				continue;	//	������һ��ѭ��
		}
	}
}

static void dosage_adjust_buttom_show_remain_dose(void)
{
	static uint8_t last_index = 0;
	uint16_t current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	int16_t remain_dose = FIX_DOSE_END_CYLCE - current_pos;
	uint8_t index = 0;
	if(remain_dose <= 0)
		index = 0;
	else{
		for(uint8_t i=1; i<= 17; i++){
			if((remain_dose <= ADJUST_DOSE[i])&&(remain_dose > ADJUST_DOSE[i-1])){
				index = i;
				break;
			}else
				continue;
		}
	}
	
	if(index != last_index){
		Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, index);
		last_index = index;
	}else
		return ;
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
	uint16_t start_cycle = (uint16_t)(Get_Current_Handle_Injector_Pos() >> 16);	/*	��ǰ�������λ�ã� ��λ��Ȧ��	*/
	uint16_t top_cycle = (handle_injector_t.TOP_LOC >>16);
	uint16_t origin_cycle = (handle_injector_t.ORIGIN_LOC >>16);
		
	end_cycle = (uint16_t)(start_cycle + cycles);
	if((end_cycle <= top_cycle)&&(end_cycle > origin_cycle))
		return end_cycle;
	else
		return top_cycle;
}

//----------------------------------------------------------------����ҩͲģʽ��صĺ����������ʵ��------------------------------------------------------

static void run_change_cartridge_mode(void)
{
	Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	�ٶ�����	*/
	Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x10);		/*	����ҩƿ��ť��ʾ������	*/
	Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);	/*	��������	*/
	xSemaphoreGive(sMotor_goOrigin2);							/*	֪ͨ�������	*/
	FIRST_WORK_FLAG = true;
	mode_selected = MODE_IDLE;
	run_mode = run_idle_mode;
}

