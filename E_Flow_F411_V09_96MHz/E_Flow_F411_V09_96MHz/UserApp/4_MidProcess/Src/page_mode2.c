#include "Page_Mode.h"

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
#include "Hampel_Filter.h"

typedef struct{
	uint32_t last_speed;
	uint32_t now_speed;
}SET_SPEED_T;

typedef struct{
	bool PDL;			/*	PDL区域提醒	*/
	bool OVERPRESSURE;	/*	over pressure 注射过压	*/
	bool REMAIN_LOW;	/*	剩余药量低	*/
	bool REMAIN_EMPTY;	/*	剩余药量空	*/
	bool SPEED;			/*	速度更新	*/
}MATCH_MUSIC_T;

typedef void (*DO_SELECTED_MODE)(void);	/*	声明一个指向输入参数为void类型，返回值为void 的函数指针类型	*/
static DO_SELECTED_MODE run_mode = NULL;


//	0. 通用函数原型声明
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol, FOOT_PEDAL_STATE_T* last_fp_state);			//	脚踏扫描
static PAGE_MODE_SELECT touch_btm_scan(LCD_TOUCH_T * lcd_touch_btm, PAGE_MODE_SELECT mode_select);		//	按键扫描
static MODE_M_CHANGE_DIR_T Mode_Position_Compare(MOTOR_DIRECTION_T motor_dir_t);						//	电机换向检测
static void update_pressure_and_remain_dose(void);														//	液压监测
static void clear_music_flag(void);																		//	清除音频标识符（语音）
static void motor_asp_response(int last_speed, int now_speed);											//	回吸响应

//	1. IDLE模式
static void run_idle_mode(void);
static void idle_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* idle_speed);

//	2. Normal常规模式
static void run_normal_mode(void);
static void normal_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* normal_speed);
static void normal_tone_adjust_by_normal_speed(int normal_speed, int* last_normal_speed);

//	3. Fast快速模式
static void run_fast_mode(void);
static void fast_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* fast_speed);
static void fast_tone_adjust_by_fast_speed(int fast_speed, int* last_fast_speed);

//	4. PDL模式
static void run_pdl_mode(void);
static void pdl_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* pdl_speed);
static void pdl_tone_adjust_by_pdl_speed(int pdl_speed, int* last_pdl_speed);

//	5. Dosage定量注射模式
static void run_dosage_mode(void);
static void dosage_mode_adjust_dose_init(void);
static uint16_t compute_end_loc_cycle(uint16_t cycles);
static void dosage_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* dosage_speed);
static void dosage_tone_adjust_by_dosage_speed(int dosage_speed, int* last_dosage_speed);
static void dosage_mode_compute_max_adjust_dose_index(void);
static void dosage_adjust_buttom_show_remain_dose(void);


//	6. 更换药筒模式
static void run_change_cartridge_mode(void);


// 外部变量	
extern SemaphoreHandle_t 	sPage_mode;
extern SemaphoreHandle_t	sMotor_goOrigin2;
extern SemaphoreHandle_t	sMotor_ASP;
extern LCD_TOUCH_T 			lcd_touch_t;

//	全局标志位即变量
bool GO_TOP_FLAG = false;
bool GO_ORIGIN_FLAG = false;
bool ASP_RUNNING_FLAG = false;
int16_t asp_cycle = 0;

//	0. 通用内部变量
static bool FIRST_WORK_FLAG = false;
static MODE_M_CHANGE_DIR_T  mode_m_change_dir_t = MODE_M_NEED_IDLE;	/*	电机状态	*/
static PAGE_MODE_SELECT now_run_mode = MODE_IDLE;					/*	当前运行的模式*/

//	1. 脚踏相关的内部变量
static FOOT_PEDAL_STATE_T fp_state_t = FP_IDLE;						/*	脚踏状态	*/

//	2. 按键扫描相关的内部变量
static PAGE_MODE_SELECT mode_selected = MODE_IDLE;					/*	按键扫描的输出结果	*/

//	3. 液压相关的内部变量


//	4. Idle模式相关的内部变量


//	5. Normal模式相关的内部变量

//	6. Fast模式相关的内部变量

//	7. PDL模式相关的内部变量
static bool PDL_AUTO_INJECTION = false;							/*	PDL模式下，自动注射功能待生效标志符	*/
static bool PDL_AUTO_INJECTION_WORK = false;					/*	PDL模式下，自动注射功能生效标志符	*/
static uint8_t auto_injection_step1_cnt = 0;					//	pdl模式下，自动注射待生效寄存器相关的计数器
static uint8_t auto_injection_step2_cnt = 0;					//	pdl模式下，自动注射生效寄存器相关的计数器

//	8. 定量注射相关的内部变量
static uint16_t FIX_DOSE = 0;//mL: 0  0.1 0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.0  1.1  1.2  1.3  1.4  1.5  1.6  1.7
static uint16_t ADJUST_DOSE[18] = {0, 63, 126, 189, 252, 315, 378, 441, 504, 567, 630, 693, 756, 819, 882, 945, 1008,1071};	/*	注射固定ml电机对应的转动圈数	*/
static uint8_t MAX_ADJUST_DOSE_INDEX_OF_REMAIN = 17;		/*	根据剩余药液余量可选择的最大定量注射索引（实时更新）	*/
static uint16_t FIX_DOSE_END_CYLCE = 0;						/*	定量注射完成时电机应在位置	*/

//	9. 换药瓶相关的内部变量

//	10. 音乐播放相关
static uint8_t MUSIC_F_50MS = 0;
static uint8_t SPEED_INDEX = 0;	
static MATCH_MUSIC_T match_music_t = {false};


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-12-20
* 函 数 名: Page_Mode
* 功能说明: mode界面下的程序
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void Page_Mode2(void)
{
	run_mode = run_idle_mode;
	fp_state_t = FP_IDLE;
	mode_selected = MODE_IDLE;
	if(xSemaphoreTake(sPage_mode, (TickType_t)0) == pdTRUE)	/*	获取信号量成功	*/
	{
		run_mode = run_idle_mode;		//	运行模式函数指针
		fp_state_t = FP_IDLE;			//	当前脚踏状态
		mode_selected = MODE_IDLE;		//	按键扫描返回的选中模式
		now_run_mode = MODE_IDLE;		//	当前运行的模式
		dosage_mode_adjust_dose_init();	//	定量注射剂量划分数组初始化
		Init_ADC_IC();					//	初始化CS1231液压传感器
		clear_music_flag();				//	清除音频标志符
		for(;;){
			osDelay(50);
			fp_state_t = FootPedal_Scan(Get_Foot_Pedal_Vol(), &fp_state_t);	//	脚踏扫描
			mode_selected = touch_btm_scan(&lcd_touch_t, now_run_mode);
			update_pressure_and_remain_dose();
			if(mode_selected == MODE_SETTING){
				run_mode = run_idle_mode;
				break;	/*	跳出循环	*/
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
			
			if(!ASP_RUNNING_FLAG) // 当前不是处于回吸过程
				run_mode();
		}
		set_motor_stop();
		Clear_Lcd_Cmd_SendQueue();
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		Set_VGUS_Page(PAGE_Setting);
		Standby_ADC_IC();				//	失能CS1231液压传感器
		clear_music_flag();				//	清除音频标志符
	}
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
static FOOT_PEDAL_STATE_T FootPedal_Scan(uint16_t fp_vol, FOOT_PEDAL_STATE_T* last_fp_state)
{
	FOOT_PEDAL_STATE_T result = FP_IDLE;
	
	fp_vol = (fp_vol > foot_pedal_t.MAX) ? foot_pedal_t.MAX : fp_vol;
	
	if(fp_vol < 100.0f)
		result = FP_NO_CONNECT;											/*	小于100mV，认为脚踏未连接	*/
	else if(fp_vol < (foot_pedal_t.MIN + 2*foot_pedal_t.QUARTER_DELTA))
		result = FP_CONNECTED;											/*	小于min+2*Delta,认为脚踏连接	*/
	else if(fp_vol < (foot_pedal_t.MIN + 3.2f*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_1;											/*	小于min+3.2*Delta,认为脚踏踩下1	*/
	else if(fp_vol < (foot_pedal_t.MIN + 3.6f*foot_pedal_t.QUARTER_DELTA))
		result = FP_DELTA_2;											/*	小于min+3.6*Delta,认为脚踏踩下2	*/
	else
		result = FP_DELTA_3;											/*	大于min+3.6*Delta,认为脚踏踩下3	*/
	
	if(*last_fp_state != result){	//	脚踏图标更新
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
	
	*last_fp_state = result;		//	记录脚踏状态
	return result;					//	返回当前脚踏状态

}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-12-20
* 函 数 名: touch_btm_scan
* 功能说明: 模式选择
*
* 形    参: 
*			LCD_TOUCH_T * lcd_touch_btm： 按钮结构体参数
      
* 返 回 值: 
*			PAGE_MODE_SELECT 用户选择的模式
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
			case PAGE_MODE_ASP_ID:			//	0x01			/*	如果ASP按钮(回吸)为按下状态	*/
				vgus_page_objs[PAGE_Mode].obj_value[0] = lcd_touch_btm->touch_state;	//	记录按钮控件值，用于屏幕复位恢复
				if(lcd_touch_btm->touch_state)
					Open_Asp();
				else
					Close_Asp();
				return MODE_KEEP;
			
			case PAGE_MODE_FUNC_ID:			//	0x07
				vgus_page_objs[PAGE_Mode].obj_value[1] = lcd_touch_btm->touch_state;	//	记录按钮控件值，用于屏幕复位恢复
				switch(lcd_touch_btm->touch_state){
					case 0x01:		//	常规模式按钮
						now_mode_select = MODE_NORMAL;			/*	如果Normal按钮为按下状态	*/
						if(now_display_page != PAGE_Mode)
							Set_VGUS_Page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x02:		//	快速模式按钮
						now_mode_select = MODE_FAST;			/*	如果FAST按钮为按下状态	*/
						if(now_display_page != PAGE_Mode)
							Set_VGUS_Page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x04:		//	PDL模式按钮
						now_mode_select = MODE_PDL;				/*	如果PDL按钮为按下状态	*/
						if(now_display_page != PAGE_Mode)
							Set_VGUS_Page(PAGE_Mode);
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x08:		//	定量注射按钮
						Clear_Lcd_Cmd_SendQueue();
						now_mode_select = MODE_DOSAGE;			/*	如果DOSAGE按钮被按下，自动跳转至定量注射界面	*/
						now_display_page = PAGE_Mode2;			//	第二处赋值当前显示界面，因为这个界面切换是屏幕自己切换的
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						FIX_DOSE = 0;
						FIX_DOSE_END_CYLCE = compute_end_loc_cycle(FIX_DOSE);
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);	//	让当前剂量显示0
						Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		//	速度清零	
						return last_mode_select;			
					
					case 0x10:		//	更换药筒按钮
						now_mode_select = MODE_CHANGE_CARTRIDGE;/*	更换药瓶按钮按下	*/
						last_mode_select = (last_mode_select == now_mode_select ? MODE_IDLE: now_mode_select);
						return last_mode_select;
					
					case 0x20:		//	设置按钮
						last_mode_select = MODE_IDLE;			/*	如果Setting按钮被按下	*/
						return MODE_SETTING;
						
					default: break;
				}
				return MODE_IDLE;

			case PAGE_MODE_ADJECT_DOSE_ID:	//	0x08			/*	定量注射的调节注射按钮被选中	*/
				if(lcd_touch_btm->touch_state > MAX_ADJUST_DOSE_INDEX_OF_REMAIN){
					lcd_touch_btm->touch_state = MAX_ADJUST_DOSE_INDEX_OF_REMAIN;
					Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, lcd_touch_btm->touch_state);
				}
				vgus_page_objs[PAGE_Mode].obj_value[2] = lcd_touch_btm->touch_state;	//	记录按钮控件值，用于屏幕复位恢复
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x08);					//	“定量注射”功能按钮显示被按下
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
	current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	now_dose = Compute_Reamin_Dose(current_pos) / 3;	/*	将100 以 3 等间距划分	*/
	now_dose = (now_dose > 30 ? 30 : now_dose);
	
	if(last_dose != now_dose){
		Show_Variable_Icon(PAGE_Mode, PAGE_MODE_DOSE_ID, now_dose);
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
	else 						//	压力报警
		now_index = 8;
	
	
	if(now_run_mode == MODE_PDL){
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index +PRES_PDL_START_INDEX: last_index);
		now_index += 9;
	}else
		last_index = (last_index < PRES_PDL_START_INDEX ? last_index: last_index -PRES_PDL_START_INDEX);
	
	if(last_index != now_index){
		if((now_index - last_index) >= 2)	/*	有跳变，忽略此次数据	*/
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
//	更新液压和剩余药量
static void update_pressure_and_remain_dose(void)
{
	static uint8_t cnt = 0;
	if(Get_Oil_Pressure(&dynamic_pressure) == New_Data_Read_Over){			
		dynamic_pressure_mV = 25.78125f*((double)dynamic_pressure/0x7FFFFF);	// mV
		dynamic_pressure_kPa = Oil_K*dynamic_pressure_mV + Oil_b;				// kPa
		dynamic_pressure_kPa_F = Hampel_Filter((uint16_t)dynamic_pressure_kPa, 3);
#if kPa_DEBUG == 1
		show_oil_pressure(dynamic_pressure_kPa_F);	// 显示压力
#else
		if((dynamic_pressure_kPa_F < 100) || (handle_in_origin == true))
			show_oil_pressure(100);						// 显示标准大气压力
		else
			show_oil_pressure(dynamic_pressure_kPa_F);	// 显示压力
#endif
	}
	if(++cnt > 10){
		if((!ASP_RUNNING_FLAG)&&(!GO_ORIGIN_FLAG))		// 回吸 和 换药瓶 过程，药液余量不变
				show_remain_dose();	// 显示剩余药量
		cnt = 0;
	}
}

//	清除音频标志符(语音播报)
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
static int16_t calu_dis_from_sp_oils(int last_speed, int now_speed)
{ 
	return 100;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-12-22
* 函 数 名: motor_asp_response
* 功能说明: 电机回吸 aspirate 响应
*
* 形    参: 
*			sp_t 之前速度和当前速度      
* 返 回 值:
*			
*********************************************************************************************************
*/
static void motor_asp_response(int last_speed, int now_speed)
{
	if(Is_Asp_Open() == true){
		if((last_speed >0)&&(now_speed ==0)){		/*	之前速度大于0，当前速度为0（电机停止）*/
			asp_cycle = calu_dis_from_sp_oils(last_speed, now_speed);
			xSemaphoreGive(sMotor_ASP);
		}else
			return ;
	}else
		return ;
}
/*-------------------------------------------------------具体运行模式---------------------------------------------------------------*/
//----------------------------------------------------------------IDLE模式相关的函数及其具体实现----------------------------------------------------------
static void run_idle_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int idle_speed = 0;
	static int last_idle_speed = 0;
	
	//	1. 失能所有功能
	if(FIRST_WORK_FLAG == true){
		Write_Two_82H_Cmd((PAGE_Mode<<8)+PAGE_MODE_SPEED_ID, 0, 0x00);	/*	速度清零, 失能所有功能	*/
		FIRST_WORK_FLAG = false;
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		FIX_DOSE = 0;
		
		idle_speed = 0;
		last_idle_speed = 0;
	}
	//	2. 判断是否需要执行更换药瓶程序：前提药瓶已注射完
	if(motor_run_direction == MOTOR_TOP){						//	药液注射完
		idle_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &idle_speed);
		if((idle_speed == 0)&&(last_idle_speed > 0)){							
			xSemaphoreGive(sMotor_goOrigin2);	/*	通知电机回退	*/
		}
		last_idle_speed = idle_speed;
	}
	//	3. 设置当前工作模式为：MODE_IDLE
	now_run_mode = MODE_IDLE;
}
//	Normal模式下的速度调节:主要用于更换药瓶判断
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
				*idle_speed = injector_speed_t[0][1];	//	任意赋一个非0值
			break;
		}
	}else
		return ;
}
//----------------------------------------------------------------Normal模式相关的函数及其具体实现--------------------------------------------------------

static void run_normal_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int normal_speed = 0;
	static int last_normal_speed = 0;
	
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. 初次进入Normal模式
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
	
	//	3. 速度/音调/回吸控制
	now_run_mode = MODE_NORMAL;
	normal_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &normal_speed);
	motor_asp_response(last_normal_speed, normal_speed);
	normal_tone_adjust_by_normal_speed(normal_speed, &last_normal_speed);
}

//	Normal模式下的速度调节
static void normal_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* normal_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*normal_speed = 0;
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	速度清零	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:	
				*normal_speed = injector_speed_t[0][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);		/*	速度清零	*/
			break;
			
			case FP_DELTA_3:	
				*normal_speed = injector_speed_t[0][2];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);		/*	速度清零	*/
			break;
		}
		set_speed_mode_speed(*normal_speed, MOTOR_FWD);
	}else
		return ;
}

//	Normal模式下的音调调节
static void normal_tone_adjust_by_normal_speed(int normal_speed, int* last_normal_speed)
{
	static uint16_t cnt = 0;
	//	1. 根据速度确定音频索引和频率
	if(normal_speed != *last_normal_speed){			//	相邻两次查询速度不同
		if(normal_speed == injector_speed_t[0][1]){
			MUSIC_F_50MS = 30;	//	1.5s间隔
			SPEED_INDEX = LOW_INJECTION_INDEX;
		}else if(normal_speed == injector_speed_t[0][2]){
			MUSIC_F_50MS = 20;	//	1s间隔
			SPEED_INDEX = MIDDLE_INJECTION_INDEX;
		}else{
			MUSIC_F_50MS = 0;
			SPEED_INDEX = 0;
			clear_music_flag();
		}
		*last_normal_speed = normal_speed;
		match_music_t.SPEED = true;	//	速度更新
		cnt = 0;
	}

	//	2. 播放音频
	if(MUSIC_F_50MS != 0){
		if(cnt % MUSIC_F_50MS == 4){
			if(Is_Train_Mode_Open() == true){								//	处于训练模式
				if(match_music_t.REMAIN_LOW == true){
					Play_Now_Music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
				}else if(match_music_t.REMAIN_EMPTY == true){
					Play_Now_Music(CHANGE_CARTRIDGE_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
				}else if(match_music_t.SPEED == true){		//	速度语音提醒
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
				}else										//	速度音调提醒
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
			}
			else if(Is_Smart_Reminder_Open() == true){						//	处于智能提醒模式
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
			else{															//	处于常规模式		
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
//----------------------------------------------------------------Fast模式相关的函数及其具体实现----------------------------------------------------------

static void run_fast_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int fast_speed = 0;
	static int last_fast_speed = 0;
	
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if(mode_m_change_dir_t == MODE_M_NEED_REV){
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. 初次进入Fast模式
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
	
	//	3. 速度和音调控制
	now_run_mode = MODE_FAST;
	fast_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &fast_speed);
	motor_asp_response(last_fast_speed, fast_speed);
	fast_tone_adjust_by_fast_speed(fast_speed, &last_fast_speed);
}

//	Fast模式下的速度调节
static void fast_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* fast_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*fast_speed = 0;
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	速度清零	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:	
				*fast_speed = injector_speed_t[1][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);		/*	速度清零	*/
			break;
			
			case FP_DELTA_3:	
				*fast_speed = injector_speed_t[1][2];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 3);		/*	速度清零	*/
			break;
		}
		set_speed_mode_speed(*fast_speed, MOTOR_FWD);
	}else
		return ;
}

//	Fast模式下的音调调节
static void fast_tone_adjust_by_fast_speed(int fast_speed, int* last_fast_speed)
{
	static uint16_t cnt = 0;
	if(fast_speed != *last_fast_speed){			//	相邻两次查询速度不同
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
		match_music_t.SPEED = true;	//	速度更新
		cnt = 0;
	}

	if(MUSIC_F_50MS != 0){
		if(cnt % MUSIC_F_50MS == 4){
			if(Is_Train_Mode_Open() == true){								//	处于训练模式
				if(match_music_t.REMAIN_LOW == true){
					Play_Now_Music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
				}else if(match_music_t.REMAIN_EMPTY == true){
					Play_Now_Music(CHANGE_CARTRIDGE_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
				}else if(match_music_t.SPEED == true){		//	速度语音提醒
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
				}else										//	速度音调提醒
					Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
			}
			else if(Is_Smart_Reminder_Open() == true){						//	处于智能提醒模式
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
			else{															//	处于常规模式		
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

//----------------------------------------------------------------PDL模式相关的函数及其具体实现----------------------------------------------------------

static void run_pdl_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int pdl_speed = 0;
	static int last_pdl_speed = 0;
	
	/*	1. 位置获取	*/
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
	
	//	2. 初次进入PDL模式
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
	
	//	3. 速度和音调控制
	now_run_mode = MODE_PDL; 
	pdl_speed_adjust_by_fp_state(fp_state_t, &last_fp_state, &pdl_speed);
	if(PDL_AUTO_INJECTION_WORK == false)
		motor_asp_response(last_pdl_speed, pdl_speed);
	pdl_tone_adjust_by_pdl_speed(pdl_speed, &last_pdl_speed);
//	MODE_PRINTF("PDL_AUTO_INJECTION = %d, PDL_AUTO_INJECTION_WORK = %d\r\n", PDL_AUTO_INJECTION, PDL_AUTO_INJECTION_WORK);
}
//	PDL模式下的速度调节
static void pdl_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* pdl_speed)
{	
	if((auto_injection_step2_cnt > 4)&&(PDL_AUTO_INJECTION_WORK == false)){
		PDL_AUTO_INJECTION = false;
		auto_injection_step1_cnt = 0;
	}
	
	if(fp_state_t != *last_fp_state){
		//	自动注射待生效为真 && 脚踏松开
		if((PDL_AUTO_INJECTION == true)&&((*last_fp_state == FP_DELTA_1)||(*last_fp_state == FP_DELTA_2)||(*last_fp_state == FP_DELTA_2))&&(fp_state_t == FP_CONNECTED)){
			//	如果当前处于未自动注射中，则自动注射生效；否则，自动注射失效,清除相关寄存器及其对应计数器
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
					Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);	/*	速度清零	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:
			case FP_DELTA_3:
				*pdl_speed = injector_speed_t[2][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);		/*	显示低速	*/
			break;
		}
		if(PDL_AUTO_INJECTION_WORK == false)
			set_speed_mode_speed(*pdl_speed, MOTOR_FWD);
	}
	
}

//	PDL模式下的音调调节
//	自动注射原理：
//	1. 连续3声速度提示音后，设备发出“自动注射/Auto Injection”;
//	2. 在接下来的3声速度提示音内松开脚踏，认为自动注射有效；超过3声速度提示音，自动注射无效；
//	3. 自动注射生效后，踩下脚踏再松开，自动注射失效；
//	4. 自动注射失效后，重新踩下脚踏，重复步骤1、2再次生效。
static void pdl_tone_adjust_by_pdl_speed(int pdl_speed, int* last_pdl_speed)
{
	static uint16_t cnt = 0;
	static bool first_play_pdl_voice = true;		//	标志符：首次播放“PDL”语音
	static bool first_play_ovp_voice = true;		//	标志符：首次播放“过压”语音
	
	static bool auto_inject_of_first_pdl = true;	//	与自动注射相关的标志符：首次进入"PDL"区域
	static bool auto_inject_of_first_idle = true;	//	与自动注射相关的标志符：首次进入"探索"区域（正在注射，但是未在PDL和过压区域）
	
	if(pdl_speed != *last_pdl_speed){			//	相邻两次查询速度不同
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
			if(Is_Train_Mode_Open() == true){								//	处于训练模式
				if(match_music_t.REMAIN_EMPTY == true){			//	药筒空
					Play_Now_Music(CHANGE_CARTRIDGE_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					osDelay(1000);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.REMAIN_LOW == true){		//	余量低
					Play_Now_Music(CARTRIDGE_REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					osDelay(1000);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.OVERPRESSURE == true){	//	过压	
					Play_Now_Music(OVER_PRESSURE_INDEX, system_volume[Get_System_Vol()]);	//	过压语音提醒
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					osDelay(150);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.PDL == true){			//	PDL区域
					//	1. 首次进入PDL区域且自动注射失效状态: 清空相关寄存器及其计数器
					if((auto_inject_of_first_pdl == true)&&(PDL_AUTO_INJECTION_WORK == false)){	
						auto_inject_of_first_pdl = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. 如果计数器1 到达3，播放自动注射提示音；否则播放PDL区域（速度）提示音
					if(auto_injection_step1_cnt == 3){	
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	播放自动注射提示音
						PDL_AUTO_INJECTION = true;
					}else{
						Play_Now_Music(REACH_PDL_INDEX, system_volume[Get_System_Vol()]);	//	PDL提示
						osDelay(150);
					}
					//	3. 根据PDL_AUTO_INJECTION状态更新计数器1的值
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. 根据PDL_AUTO_INJECTION状态更新计数器2的值
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_idle= true;
				}else{											//	正常注射（探索过程中，即寻找PDL区域）
					//	1. 首次进入探索区域且自动注射失效状态: 清空相关寄存器及其计数器
					if((auto_inject_of_first_idle == true)&&(PDL_AUTO_INJECTION_WORK == false)){
						auto_inject_of_first_idle = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. 如果计数器1 到达3，播放自动注射提示音；否则播放探索阶段(速度)提示音
					if(auto_injection_step1_cnt == 3){
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	播放自动注射提示音
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
					//	3. 根据PDL_AUTO_INJECTION状态更新计数器1的值
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. 根据PDL_AUTO_INJECTION状态更新计数器2的值
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_pdl = true;
				}
				match_music_t.SPEED = false;
				first_play_ovp_voice = true;
			}
			else if(Is_Smart_Reminder_Open() == true){						//	处于智能提醒模式
				if(match_music_t.REMAIN_EMPTY == true){			//	药筒空
					Play_Now_Music(CARTRIDGE_EMPTY_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					osDelay(1000);
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.REMAIN_LOW == true){		//	余量低
					Play_Now_Music(REMAIN_LOW_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					osDelay(1000);
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.OVERPRESSURE == true){	//	过压
					first_play_pdl_voice = true;
					if(first_play_ovp_voice == true){
						Play_Now_Music(OVER_PRESSURE_INDEX, system_volume[Get_System_Vol()]);	//	过压提示
						osDelay(150);
						first_play_ovp_voice = false;
					}else	
						Play_Now_Music(ALARM_INDEX, system_volume[Get_System_Vol()]);
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.PDL == true){			//	PDL区域
					first_play_ovp_voice = true;
					//	1. 首次进入PDL区域且自动注射失效状态: 清空相关寄存器及其计数器
					if((auto_inject_of_first_pdl == true)&&(PDL_AUTO_INJECTION_WORK == false)){	
						auto_inject_of_first_pdl = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. 如果计数器1 到达3，播放自动注射提示音；否则播放PDL区域（速度）提示音
					if(auto_injection_step1_cnt == 3){	
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	播放自动注射提示音
						PDL_AUTO_INJECTION = true;
						osDelay(150);
					}else if(first_play_pdl_voice == true){
						Play_Now_Music(PDL_INDEX, system_volume[Get_System_Vol()]);	//	PDL提示
						first_play_pdl_voice = false;
						osDelay(150);
					}else
						Play_Now_Music(MIDDLE_INJECTION_INDEX, system_volume[Get_System_Vol()]);	//	PDL提示
					//	3. 根据PDL_AUTO_INJECTION状态更新计数器1的值
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. 根据PDL_AUTO_INJECTION状态更新计数器2的值
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_idle= true;
				}else{											//	正常注射（探索）
					first_play_pdl_voice = true;
					first_play_ovp_voice = true;
					//	1. 首次进入探索区域且自动注射失效状态: 清空相关寄存器及其计数器
					if((auto_inject_of_first_idle == true)&&(PDL_AUTO_INJECTION_WORK == false)){
						auto_inject_of_first_idle = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. 如果计数器1 到达3，播放自动注射提示音；否则播放探索阶段(速度)提示音
					if(auto_injection_step1_cnt == 3){
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	播放自动注射提示音
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
					//	3. 根据PDL_AUTO_INJECTION状态更新计数器1的值
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. 根据PDL_AUTO_INJECTION状态更新计数器2的值
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_pdl = true;
				}
				match_music_t.SPEED = false;
			}
			else{															//	处于常规模式		
				if(match_music_t.REMAIN_EMPTY == true){			//	药筒空
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_EMPTY = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.REMAIN_LOW == true){		//	余量低
					Play_Now_Music(REMAIN_INDEX, system_volume[Get_System_Vol()]);
					match_music_t.REMAIN_LOW = false;
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.OVERPRESSURE == true){	//	过压
					Play_Now_Music(ALARM_INDEX, system_volume[Get_System_Vol()]);				//	过压提示
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? 0 : auto_injection_step1_cnt);
					auto_inject_of_first_pdl = true;
					auto_inject_of_first_idle= true;
				}else if(match_music_t.PDL == true){			//	PDL区域	
					//	1. 首次进入PDL区域且自动注射失效状态: 清空相关寄存器及其计数器
					if((auto_inject_of_first_pdl == true)&&(PDL_AUTO_INJECTION_WORK == false)){	
						auto_inject_of_first_pdl = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. 如果计数器1 到达3，播放自动注射提示音；否则播放PDL区域（速度）提示音
					if(auto_injection_step1_cnt == 3){	
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	播放自动注射提示音
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(MIDDLE_INJECTION_INDEX, system_volume[Get_System_Vol()]);	//	PDL提示
					//	3. 根据PDL_AUTO_INJECTION状态更新计数器1的值
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. 根据PDL_AUTO_INJECTION状态更新计数器2的值
					auto_injection_step2_cnt = ((PDL_AUTO_INJECTION == true) ? (auto_injection_step2_cnt +1) : 0);
					auto_inject_of_first_idle= true;
				}else{											//	正常注射（探索过程中，即寻找PDL区域）
					//	1. 首次进入探索区域且自动注射失效状态: 清空相关寄存器及其计数器
					if((auto_inject_of_first_idle == true)&&(PDL_AUTO_INJECTION_WORK == false)){
						auto_inject_of_first_idle = false;
						PDL_AUTO_INJECTION = false;
						auto_injection_step1_cnt = 0;
						auto_injection_step2_cnt = 0;
					}
					//	2. 如果计数器1 到达3，播放自动注射提示音；否则播放探索阶段(速度)提示音
					if(auto_injection_step1_cnt == 3){
						Play_Now_Music(AUTO_INJECTION_INDEX, system_volume[Get_System_Vol()]);		//	播放自动注射提示音
						PDL_AUTO_INJECTION = true;
					}else
						Play_Now_Music(SPEED_INDEX, system_volume[Get_System_Vol()]);
					//	3. 根据PDL_AUTO_INJECTION状态更新计数器1的值
					auto_injection_step1_cnt = ((PDL_AUTO_INJECTION == false) ? (auto_injection_step1_cnt +1) : 4);
					//	4. 根据PDL_AUTO_INJECTION状态更新计数器2的值
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
//----------------------------------------------------------------Dosage模式相关的函数及其具体实现--------------------------------------------------------

static void run_dosage_mode(void)
{
	static FOOT_PEDAL_STATE_T last_fp_state = FP_IDLE;
	static int dosage_speed = 0;
	static int last_dosage_speed = 0;
	
	/*	1. 位置获取	*/
	mode_m_change_dir_t = Mode_Position_Compare(motor_run_direction);
	if((mode_m_change_dir_t == MODE_M_FWD_GOAL)||(mode_m_change_dir_t == MODE_M_NEED_REV)){
		Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
		FIX_DOSE = 0;
		FIRST_WORK_FLAG = true;
		mode_selected = MODE_IDLE;
		run_mode = run_idle_mode;
		return ;
	}
	
	//	2. 初次进入Dosage模式
	if(FIRST_WORK_FLAG == true){
		set_motor_stop();		//	电机停止，必须在设定注射剂量后方可启动电机
		
		fp_state_t = FP_IDLE;
		last_fp_state = FP_IDLE;
		
		dosage_speed = 0;
		last_dosage_speed = 0;
		
		MUSIC_F_50MS = 0;
		SPEED_INDEX = 0;
		
		FIRST_WORK_FLAG = false;
	}
	
	//	3. 速度和音调控制
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-8-31
* 函 数 名: adjust_dose_init
* 功能说明: 初始化定量注射寄存器
*
* 形    参: 
      
* 返 回 值: 
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

//	Dosage模式下的速度调节
static void dosage_speed_adjust_by_fp_state(FOOT_PEDAL_STATE_T fp_state_t, FOOT_PEDAL_STATE_T* last_fp_state, int* dosage_speed)
{
	if(fp_state_t != *last_fp_state){
		*last_fp_state = fp_state_t;
		switch(fp_state_t){
			case FP_CONNECTED:
			case FP_NO_CONNECT:
			case FP_IDLE:		
				*dosage_speed = 0;
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	速度清零	*/
			break;
			
			case FP_DELTA_1:
			case FP_DELTA_2:	
				*dosage_speed = injector_speed_t[0][1];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 1);		/*	低速	*/
			break;
			
			case FP_DELTA_3:	
				*dosage_speed = injector_speed_t[0][2];	
				Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 2);		/*	中速	*/
			break;
		}
		set_speed_mode_speed(*dosage_speed, MOTOR_FWD_TO_GOAL);
	}else
		return ;
}

//	Dosage模式下的音调调节
static void dosage_tone_adjust_by_dosage_speed(int dosage_speed, int* last_dosage_speed)
{
	static uint8_t cnt = 0;
	if(dosage_speed != *last_dosage_speed){			//	相邻两次查询速度不同
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-12-01
* 函 数 名: compute_max_adjust_dose_index
* 功能说明: 定量注射模式中根据剩余药量计算最大可注射量
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void dosage_mode_compute_max_adjust_dose_index(void)
{
	uint16_t current_pos = (Get_Current_Handle_Injector_Pos() >> 16);
	uint16_t all_dis = (handle_injector_t.DISTANCE >> 16);
	uint16_t now_dis = Get_Current_Handle_Run_Distance(current_pos);
	uint16_t remain_dose = all_dis - now_dis;
	
	//	1. 计算边界
	if(remain_dose >= ADJUST_DOSE[17]){
		MAX_ADJUST_DOSE_INDEX_OF_REMAIN = 17;			//	更新索引
		return ;
	}else{
		for(uint8_t i=0; i<17; i++){
			if((remain_dose >= ADJUST_DOSE[i])&&(remain_dose < ADJUST_DOSE[i+1])){
				MAX_ADJUST_DOSE_INDEX_OF_REMAIN = i;	//	更新索引
				//MODE_PRINTF("all_dis= %d, now_dis= %d, and remain_dose= %d, remain_index= %d\r\n", all_dis, now_dis, remain_dose, MAX_ADJUST_DOSE_INDEX_OF_REMAIN);
				return ;
			}else
				continue;	//	继续下一轮循环
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
	uint16_t start_cycle = (uint16_t)(Get_Current_Handle_Injector_Pos() >> 16);	/*	当前电机所在位置， 单位：圈数	*/
	uint16_t top_cycle = (handle_injector_t.TOP_LOC >>16);
	uint16_t origin_cycle = (handle_injector_t.ORIGIN_LOC >>16);
		
	end_cycle = (uint16_t)(start_cycle + cycles);
	if((end_cycle <= top_cycle)&&(end_cycle > origin_cycle))
		return end_cycle;
	else
		return top_cycle;
}

//----------------------------------------------------------------更换药筒模式相关的函数及其具体实现------------------------------------------------------

static void run_change_cartridge_mode(void)
{
	Show_Variable_Icon(PAGE_Mode, PAGE_MODE_SPEED_ID, 0);		/*	速度清零	*/
	Show_Variable_Icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x10);		/*	更换药瓶按钮显示被按下	*/
	Show_Variable_Icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);	/*	剂量清零	*/
	xSemaphoreGive(sMotor_goOrigin2);							/*	通知电机回退	*/
	FIRST_WORK_FLAG = true;
	mode_selected = MODE_IDLE;
	run_mode = run_idle_mode;
}

