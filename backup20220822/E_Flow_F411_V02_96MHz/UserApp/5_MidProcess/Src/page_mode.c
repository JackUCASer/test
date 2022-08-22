#include "page_mode.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	外设硬件		*/
#include "get_voltage.h"		/*	电压：脚踏、电池、充电器	*/
#include "oil_cs1231.h"			/*	液压传感器	*/
#include "motor_misc.h"			/*	电机控制接口	*/

/*	APP&DATA	*/
#include "user_data.h"			/*	用户和系统数据	*/

/*	数据结构与算法层		*/
#include "lp_filter.h"			/*	低通滤波		*/
#include "movmean.h"			/*	滑动平均		*/
#include <string.h>
#include "hampel_filter.h"

typedef struct{
	uint32_t last_speed;
	uint32_t now_speed;
}SET_SPEED_T;

typedef struct{
	bool PDL;			/*	PDL区域提醒	*/
	bool OVERPRESSURE;	/*	over pressure 注册过压	*/
	bool PRESSURE;		/*	压力提醒，取消	*/
	bool SPEED;			/*	速度提醒	*/
	bool ASP;			/*	回吸提醒	*/
	bool REMAIN_LOW;	/*	剩余药量低	*/
	bool REMAIN_EMPTY;	/*	剩余药量空	*/
}MATCH_MUSIC_T;

typedef void (*DO_SELECTED_MODE)(void);	/*	声明一个指向输入参数为void类型，返回值为void 的函数指针类型	*/

//	函数原型声明
static PAGE_MODE_SELECT touch_btm_scan(LCD_TOUCH_T * lcd_touch_btm);				/*	按键扫描	*/
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol);							/*	脚踏扫描	*/
static void run_normal_mode(void);													/*	运行Normal模式	*/
static void run_fast_mode(void);													/*	运行fast模式	*/
static void run_pdl_mode(void);														/*	运行pdl模式	*/
static void run_dosage_mode(void);
static void stop_dosage_mode(void);
static void run_change_cartridge(void);												/*	运行更换药瓶模式	*/
static void run_idle_mode(void);													/*	空模式	*/
static MODE_M_CHANGE_DIR_T Mode_Position_Compare(MOTOR_DIRECTION_T motor_dir_t);	/*	实时位置监测	*/
static void motor_speed_adjust_accordingto_fp(PAGE_MODE_SELECT page_mode_select);	/*	脚踏实时反馈	*/
static void motor_asp_response(SET_SPEED_T sp_t);									/*	回吸功能	*/
static void show_remain_dose(void);													/*	显示药液余量	*/
static void show_oil_pressure(uint16_t oil_pressure);									/*	显示推力	*/
static void play_music_at_same_intervals(uint8_t f_100ms, bool do_play_music);		/*	等间隔播放音乐	*/
static void clear_match_music(void);												/*	清除音乐标志位	*/
static void play_mode_choice_voice(PAGE_MODE_SELECT mode_choice);

// 外部变量	
extern SemaphoreHandle_t 	sPage_mode;
extern SemaphoreHandle_t	sMotor_goOrigin;
extern SemaphoreHandle_t	sMotor_ASP;
extern LCD_TOUCH_T 			lcd_touch_t;

bool GO_TOP_FLAG = false;
bool GO_ORIGIN_FLAG = false;
bool START_ASP_FLAG = false;
int16_t asp_cycle = 0;
// 内部变量
static DO_SELECTED_MODE run_mode = run_idle_mode;
static MODE_M_CHANGE_DIR_T  mode_m_change_dir_t = MODE_M_NEED_IDLE;
static bool first_run_idle = false; 								/*	首次运行空闲模式	*/
static FOOT_PEDAL_STATE_T fp_state_t = FP_IDLE;						/*	脚踏状态	*/
static PAGE_MODE_SELECT music_according_to_mode = MODE_IDLE;		/*	根据当前模式选择音乐	*/
static PAGE_MODE_SELECT press_according_to_mode = MODE_IDLE;		/*	根据当前模式显示压力	*/

// 音乐播放相关
static bool DO_PLAY_MUSIC = false;
static float MUSIC_F_100MS = 0.0f;
static MATCH_MUSIC_T match_music_t = {false};
static uint8_t SPEED_INDEX = 0;

//	与定量注射相关
static bool DOSAGE_FLAG = false;
static uint16_t fix_dose = 0;
// ----------------------------mL: 0  0.1 0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.0  1.1  1.2  1.3  1.4  1.5  1.6  1.7
static uint16_t ADJUST_DOSE[18] = {0, 60, 120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720, 780, 840, 900, 960, 1020};

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: page_mode
* 功能说明: mode界面下的程序
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void page_mode(void)
{	
	static PAGE_MODE_SELECT mode_selected = MODE_IDLE;
	static uint16_t footPedal_vol = 0;
	static uint16_t f_footPedal_vol = 0;
	static uint32_t fre_cnt = 0;
	if(xSemaphoreTake(sPage_mode, (TickType_t)0) == pdTRUE)	/*	获取信号量成功	*/
	{
		memset(&lcd_touch_t, 0, sizeof(lcd_touch_t));		/*	清除按键历史	*/
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
				break;	/*	跳出循环	*/
			}else{
				switch(mode_selected){
					case MODE_NORMAL:
						run_mode = run_normal_mode;			/*	进入Normal工作模式	*/
						music_according_to_mode = MODE_NORMAL;
						press_according_to_mode = MODE_NORMAL;
						show_variable_icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_OTHER_START_INDEX);
						break;
					
					case MODE_FAST:
						run_mode = run_fast_mode;			/*	进入Fast工作模式	*/
						music_according_to_mode = MODE_FAST;
						press_according_to_mode = MODE_FAST;
						show_variable_icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_OTHER_START_INDEX);
						break;
					
					case MODE_PDL:					
						run_mode = run_pdl_mode;			/*	进入Pdl工作模式	*/
						music_according_to_mode = MODE_PDL;
						press_according_to_mode = MODE_PDL;
						show_variable_icon(PAGE_Mode, PAGE_MODE_PRES_ID, PRES_PDL_START_INDEX);
						break;
					
					case MODE_DOSAGE:						
						run_mode = run_dosage_mode;			/*	进入定量注射界面	*/
						music_according_to_mode = MODE_FAST;
						press_according_to_mode = MODE_FAST;
						break;
					
					case MODE_CHANGE_CARTRIDGE:				/*	进入更换药瓶模式	*/
						run_mode = run_change_cartridge;
						music_according_to_mode = MODE_IDLE;
						press_according_to_mode = MODE_IDLE;
						break;
					
					case MODE_IDLE:
						first_run_idle = true;
						run_mode = run_idle_mode;			/*	进入idle工作模式	*/
						music_according_to_mode = MODE_IDLE;
						press_according_to_mode = MODE_IDLE;
						break;
					
					case MODE_KEEP: 						/*	保持当前模式，电机运转方向不变	*/
						break;
					
					default:break;
				}
			}
			fp_state_t = FootPedal_Scan(f_footPedal_vol);	/*	脚踏扫描	*/
			if(!START_ASP_FLAG) // 当前不是处于回吸过程
				run_mode();
			
			if(get_oil_pressure(&dynamic_pressure) == New_Data_Read_Over){			
				dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
				dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa
				footPedal_vol = get_footpedal_voltage();
				f_footPedal_vol = movmean_filter(footPedal_vol);
				dynamic_pressure_kPa_F = hampel_filter((uint16_t)dynamic_pressure_kPa, 3);
				if((!START_ASP_FLAG)&&(!GO_ORIGIN_FLAG))		// 回吸和换药瓶过程，药液余量不变
					show_remain_dose();	// 显示剩余药量
				show_oil_pressure(dynamic_pressure_kPa_F);	// 显示压力
//				MODE_PRINTF("footPedal_vol = %d, fp_state_t = %d\r\n", f_footPedal_vol, fp_state_t);	//	调试脚踏的感觉
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-08-07
* 函 数 名: compute_end_cycle
* 功能说明: 根据电机端（转子）前进的圈数，计算终点圈数
*
* 形    参: 
*			cycles: 电机正向运转的圈数
* 返 回 值:
*		电机端（转子）终点所在的圈数
*********************************************************************************************************
*/
static uint16_t compute_end_loc_cycle(uint16_t cycles)
{
	uint16_t end_cycle = 0;
	uint16_t buf2 = (uint16_t)(get_current_handle_injector() >> 16);	/*	当前电机所在位置， 单位：圈数	*/
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-21
* 函 数 名: touch_btm_scan
* 功能说明: 模式选择
*
* 形    参: 
*			LCD_TOUCH_T * lcd_touch_btm： 按钮结构体参数
      
* 返 回 值: 
*			PAGE_MODE_SELECT 用户选择的模式
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
						now_mode_select = MODE_NORMAL;			/*	如果Normal按钮为按下状态	*/
						if(now_display_page != PAGE_Mode)
							set_now_page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x02:
						stop_dosage_mode();
						now_mode_select = MODE_FAST;			/*	如果FAST按钮为按下状态	*/
						if(now_display_page != PAGE_Mode)
							set_now_page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x04:
						stop_dosage_mode();
						now_mode_select = MODE_PDL;				/*	如果PDL按钮为按下状态	*/
						if(now_display_page != PAGE_Mode)
							set_now_page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x08:
						stop_dosage_mode();
						now_mode_select = MODE_DOSAGE;			/*	如果DOSAGE按钮被按下，自动跳转至定量注射界面	*/
						now_display_page = PAGE_Mode2;
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						DOSAGE_FLAG = (last_mode_select == MODE_DOSAGE ? true: false);
						show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
						return last_mode_select;			
					
					case 0x10:
						stop_dosage_mode();
						now_mode_select = MODE_CHANGE_CARTRIDGE;/*	更换药瓶按钮按下	*/
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x20:
						stop_dosage_mode();
						last_mode_select = MODE_IDLE;			/*	如果Setting按钮被按下	*/
						return MODE_SETTING;
						
					default: break;
				}
				return MODE_IDLE;
					
			case PAGE_MODE_ASP_ID:								/*	如果ASP按钮(回吸)为按下状态	*/
				if(lcd_touch_btm->touch_state)
					open_asp();
				else
					close_asp();
				return MODE_KEEP;
			
			case PAGE_MODE_ADJECT_DOSE_ID:						/*	调节注射按钮被选中	*/
				show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x08);
				DOSAGE_FLAG = true;
				fix_dose = ADJUST_DOSE[lcd_touch_btm->touch_state];
				motor_to_goal_cycle = compute_end_loc_cycle(fix_dose);
				motor_run_direction = MOTOR_FWD_TO_GOAL;		/*	清除MOTOR_FWD_GOAL标志	*/
				return MODE_DOSAGE;
				
			default: return MODE_KEEP;
		}
	}else
		return MODE_KEEP;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-05-24
* 函 数 名: FootPedal_Scan
* 功能说明: 脚踏扫描
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	FOOT_PEDAL_STATE_T result = FP_IDLE;
	
	fp_vol = (fp_vol > foot_pedal_t.MAX) ? foot_pedal_t.MAX : fp_vol;
	
	if(fp_vol < 100.0f)
		result = FP_NO_CONNECT;											/*	小于x00mV，认为脚踏未连接	*/
	else if(fp_vol < (foot_pedal_t.MIN + foot_pedal_t.QUARTER_DELTA))
		result = FP_CONNECTED;											/*	小于min+1*Delta,认为脚踏连接	*/
	else if(fp_vol < (foot_pedal_t.MIN + 2*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_1;											/*	小于min+2*Delta,认为脚踏踩下1	*/
	else if(fp_vol < (foot_pedal_t.MIN + 3*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_2;											/*	小于min+3*Delta,认为脚踏踩下2	*/
	else
		result = FP_DELTA_3;											/*	大于min+3*Delta,认为脚踏踩下3	*/
	
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
/*-----------------------------------------------------------具体运行模式----------------------------------------------------------------*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: run_normal_mode
* 功能说明: mode界面下：normal 模式
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_normal_mode(void)
{
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}
	
	/*	2. 根据电压进行调速	*/
	match_music_t.SPEED = true;
	motor_speed_adjust_accordingto_fp(MODE_NORMAL);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: run_fast_mode
* 功能说明: mode界面下：fast 模式
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_fast_mode(void)
{
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}

	/*	2. 根据电压进行调速	*/
	match_music_t.SPEED = true;
	motor_speed_adjust_accordingto_fp(MODE_FAST);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: run_pdl_mode
* 功能说明: mode界面下：pdl 模式
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_pdl_mode(void)
{
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}
	
	/*	2. 压力提醒		*/
	match_music_t.SPEED = true;
	motor_speed_adjust_accordingto_fp(MODE_PDL);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-08-06
* 函 数 名: run_dose_mode
* 功能说明: mode界面下：dosage 模式
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_dosage_mode(void)
{
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if((mode_m_change_dir_t == MODE_M_FWD_GOAL)||(mode_m_change_dir_t == MODE_M_NEED_REV)){
		fix_dose = 0;
		show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
		first_run_idle = true;
		run_mode = run_idle_mode;
		return ;
	}

	/*	2. 根据电压进行调速	*/
	if((DOSAGE_FLAG == true)&&(fix_dose > 0)){
		match_music_t.SPEED = true;
		motor_speed_adjust_accordingto_fp(MODE_DOSAGE);
	}else{
		match_music_t.SPEED = false;
	}
	
}
//	清除dosage模式运行的标志位和剂量
static void stop_dosage_mode(void)
{
	set_motor_stop();
	fix_dose = 0;
	DOSAGE_FLAG = false;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-08-16
* 函 数 名: run_change_cartridge
* 功能说明: mode界面下：更换药瓶模式
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_change_cartridge(void)
{
	show_variable_icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	速度清零	*/
//	show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x10);		/*	失能所有功能	*/
	show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);	/*	剂量清零	*/
	xSemaphoreGive(sMotor_goOrigin);							/*	通知电机回退	*/
	first_run_idle = false;
	run_mode = run_idle_mode;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: run_idle_mode
* 功能说明: mode界面下：idle 模式
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_idle_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	if(first_run_idle == true){
		first_run_idle = false;
		clear_match_music();
		if(DOSAGE_FLAG == true)
			write_two_82H_cmd((PAGE_Mode<<8)+PAGE_MODE_SPEED_ID, 0, 0x08);	/*	速度清零, 失能所有功能除了“定量注射”	*/
		else
			write_two_82H_cmd((PAGE_Mode<<8)+PAGE_MODE_SPEED_ID, 0, 0x00);	/*	速度清零, 失能所有功能	*/
	}
	
	if(motor_run_direction == MOTOR_TOP){	//	当前已注射完
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-07-13
* 函 数 名: Mode_Position_Compare
* 功能说明: 判断电机是否需要换向
*
* 形    参: 
*			motor_dir_t: 当前电机的方向
      
* 返 回 值:
*			MODE_M_CHANGE_DIR_T 电机需要切换的方向
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: motor_speed_adjust_accordingto_fp
* 功能说明: 根据脚踏反馈的电压进行电机速度调整
*
* 形    参: 
*			page_mode_select 当前选择的模式      
* 返 回 值:
*			
*********************************************************************************************************
*/
static void motor_speed_adjust_accordingto_fp(PAGE_MODE_SELECT page_mode_select)
{
	uint8_t index = 0;
	static SET_SPEED_T set_speed_t[3] = {0};
	static PAGE_MODE_SELECT last_page_mode_select = MODE_IDLE;
	
	/*	1. 根据不同模式设置不同速度索引	*/
	switch(page_mode_select){
		case MODE_DOSAGE:
		case MODE_NORMAL:	index = 0;	break;
		
		case MODE_FAST:		index = 1;	break;
		
		case MODE_PDL: 		index = 2;	break;
		
		default:	return;
	}
	
	set_speed_t[index].last_speed = set_speed_t[index].now_speed;
	/*	2. 根据不同脚踏电压获取设置不同速度等级	*/
	switch(fp_state_t){
		case FP_DELTA_1:	set_speed_t[index].now_speed = injector_speed_t[index][0];	break;
		
		case FP_DELTA_2:	set_speed_t[index].now_speed = injector_speed_t[index][1];	break;
		
		case FP_DELTA_3:	set_speed_t[index].now_speed = injector_speed_t[index][2];	break;
		
		default: 			set_speed_t[index].now_speed = 0;	break;
	}
	
	/*	3. 控制电机运转	*/
	if(set_speed_t[index].now_speed != set_speed_t[index].last_speed){ // 如果相邻两次速度不同，则重新设置电机转速
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
	
	/*	4. 回吸配置:定量注射无此功能；功能模式切换时也不执行此功能		*/
	if((page_mode_select != MODE_DOSAGE)&&(last_page_mode_select == page_mode_select))
		motor_asp_response(set_speed_t[index]);

	if(last_page_mode_select != page_mode_select){
		set_speed_t[0].now_speed = 0;
		set_speed_t[1].now_speed = 0;
		set_speed_t[2].now_speed = 0;
	}
		
	
	/*	5. 保存当前模式	*/
	last_page_mode_select = page_mode_select;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-01-26
* 函 数 名: calu_dis_from_sp_oils
* 功能说明: 计算开启回吸功能，手柄回退4mm对应的电机圈数
*
* 形    参: 
*			      
* 返 回 值:
*			固定值丝杆回退4mm，对应电机回退100圈
*********************************************************************************************************
*/
int16_t calu_dis_from_sp_oils(SET_SPEED_T sp_t)
{ 
	return 100;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-01-26
* 函 数 名: motor_asp_response
* 功能说明: 电机回吸 aspirate 响应
*
* 形    参: 
*			sp_t 之前速度和当前速度      
* 返 回 值:
*			
*********************************************************************************************************
*/
static void motor_asp_response(SET_SPEED_T sp_t)
{
	if(is_asp_open() == true)
		if((sp_t.last_speed >0)&&(sp_t.now_speed ==0)){		/*	之前速度大于0，当前速度为0（电机停止）*/
			set_motor_stop();
			osDelay(200);
			asp_cycle = calu_dis_from_sp_oils(sp_t);
			xSemaphoreGive(sMotor_ASP);
			match_music_t.SPEED = false;
		}else
			return ;
	else if(sp_t.now_speed ==0){							/*	未开启asp, 当前速度为0（电机停止）*/
		set_motor_stop();
		match_music_t.SPEED = false;
	}else
		return ;
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
	
	last_dose = now_dose; 
	current_pos = (get_current_handle_injector() >> 16);
	now_dose = compute_remain_dose(current_pos) / 3;	/*	将100 以 3 等间距划分	*/
	now_dose = (now_dose > 30 ? 30 : now_dose);
	
	if(last_dose != now_dose){
		show_variable_icon(PAGE_Mode, PAGE_MODE_DOSE_ID, now_dose);
		if((now_dose == 6)&&(now_dose < last_dose))		//	now_dose =6 且处于前进过程中
			match_music_t.REMAIN_LOW = true;
		else if((now_dose == 1)&&(now_dose < last_dose))//	now_dose =1 且处于前进过程中
			match_music_t.REMAIN_EMPTY = true;
		else
			;
	}else
		return ;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-03-15
* 函 数 名: show_oil_pressure
* 功能说明: 显示液压: 仅液压增加时会发出提示音
*
* 形    参: 
      
* 返 回 值: 
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
	else 						//	压力报警
		now_index = 8;
	
	if(press_according_to_mode == MODE_PDL){
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index +PRES_PDL_START_INDEX: last_index);
		now_index += 9;
	}else
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index: last_index -PRES_PDL_START_INDEX);
	
	if(last_index != now_index){
		if((now_index - last_index) >= 2)	/*	有跳变，忽略此次数据	*/
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-05-25
* 函 数 名: play_music_at_same_intervals
* 功能说明: 等间隔播放音乐,在单独的 task 中执行,避免语音播放影响主任务
*
* 形    参: 
*			f_100ms:		播放频率,[20, 10, 5, 4, 2, 1]Hz
*			do_play_music: 	是否执行此函数
* 返 回 值: 
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
					if(is_train_mode_open() == true){				//	处于训练模式
						if(match_music_t.REMAIN_LOW == true){
							play_now_music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_LOW = false;
							osDelay(1200);
						}else if(match_music_t.REMAIN_EMPTY == true){
							play_now_music(CHANGE_CARTRIDGE_INDEX, system_volume[get_system_vol()]);
							match_music_t.REMAIN_EMPTY = false;
							osDelay(1500);
						}else if(match_music_t.PDL == true){
							play_now_music(REACH_PDL_INDEX, system_volume[get_system_vol()]);			//	PDL提示
							osDelay(1000);
						}else if(match_music_t.OVERPRESSURE == true){
							play_now_music(OVER_PRESSURE_INDEX, system_volume[get_system_vol()]);		//	过压提示
							osDelay(800);
						}else if(match_music_t.SPEED == true)
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);				//	低压注射中
						else
							;
					}else if(is_smart_reminder_open() == true){		//	处于智能提醒模式
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
								play_now_music(PDL_INDEX, system_volume[get_system_vol()]);				//	PDL提示
								osDelay(150);
								first_play_pdl_voice = false;
							}else
								play_now_music(MIDDLE_INJECTION_INDEX, system_volume[get_system_vol()]);
							first_play_ovp_voice = true;
						}else if(match_music_t.OVERPRESSURE == true){
							if(first_play_ovp_voice == true){
								play_now_music(OVER_PRESSURE_INDEX, system_volume[get_system_vol()]);	//	过压提示
								osDelay(150);
								first_play_ovp_voice = false;
							}else	
								play_now_music(ALARM_INDEX, system_volume[get_system_vol()]);
							first_play_pdl_voice = true;
						}else if(match_music_t.SPEED == true){
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);				//	低压注射中
							first_play_pdl_voice = true;
							first_play_ovp_voice = true;
						}else{
							first_play_pdl_voice = true;
							first_play_ovp_voice = true;
						}
					}else{											//	处于常规模式
						if(match_music_t.PDL == true){
							play_now_music(MIDDLE_INJECTION_INDEX, system_volume[get_system_vol()]);	//	PDL提示
						}else if(match_music_t.OVERPRESSURE == true){
							play_now_music(ALARM_INDEX, system_volume[get_system_vol()]);				//	过压提示
						}else if(match_music_t.SPEED == true)
							play_now_music(SPEED_INDEX, system_volume[get_system_vol()]);				//	低压注射中
						else
							;
					}
				break;
				
				case MODE_NORMAL: case MODE_FAST:	//	(REMAIN_LOW||REMAIN_EMPTY) > SPEED
					if(is_train_mode_open() == true){				//	处于训练模式
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
					}else if(is_smart_reminder_open() == true){		//	处于智能提醒模式
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
					}else{											//	处于常规模式
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-05-27
* 函 数 名: clear_match_music
* 功能说明: 清除音乐播放标志位
*
* 形    参: 
* 返 回 值: 
*********************************************************************************************************
*/
static void clear_match_music(void)
{
	music_according_to_mode = MODE_IDLE;	/*	当前处于IDLE模式	*/
	MUSIC_F_100MS = 0;						/*	无播放频率	*/
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-08-01
* 函 数 名: 功能按键语音播放
* 功能说明: 
*
* 形    参: 
* 返 回 值: 
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
