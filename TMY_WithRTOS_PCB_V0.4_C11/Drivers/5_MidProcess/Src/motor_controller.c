#include "motor_controller.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	外设硬件	*/
#include "motor_misc.h"
#include "vgus_lcd_if.h"

/*	midprocess	*/
#include "page_mode.h"

/*	APP&DATA	*/
#include "user_data.h"


static bool GO_TOP_FLAG = false;
static bool GO_BOTTOM_FLAG = false;
static bool check_if_motor_reach_bottom(void);
static bool check_if_motor_reach_top(void);
static bool check_if_motor_reach_goal(uint16_t end_cycle);
static uint16_t compute_end_cycle(int16_t cycles);

SemaphoreHandle_t	sMotor_goBottom = NULL;		/*	定义一个二进制信号量句柄，用于通知执行电机控制器	*/
SemaphoreHandle_t	sMotor_ASP = NULL;			/*	定义一个二进制信号量句柄，用于通知执行电机回吸	*/
SemaphoreHandle_t	sMotor_goTop = NULL;


void motor_controller_alarm(void)
{
	static uint8_t CTR_F_100MS = 0;
	if(++CTR_F_100MS > 4){
		CTR_F_100MS = 0;
		if(START_ASP_FLAG == true)
			play_now_music(ASP_INDEX, system_volume[get_system_vol()]);
		else if((GO_TOP_FLAG == true) ||(GO_BOTTOM_FLAG == true))
			play_now_music(TOP_BOTTOM_INDEX, system_volume[get_system_vol()]);
		else
			return;
	}
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-03-02
* 函 数 名: motor_controller_init
* 功能说明: 电机控制器初始化
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void motor_controller_init(void)
{
	sMotor_goBottom = xSemaphoreCreateBinary();	/*	初始化二值信号量 sMotor_goBottom	*/
	sMotor_ASP = xSemaphoreCreateBinary();		/*	初始化二值信号量 sMotor_ASP	*/
	sMotor_goTop = xSemaphoreCreateBinary();	/*	初始化二值信号量 sMotor_ASP	*/
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-03-09
* 函 数 名: motor_controller
* 功能说明: 电机控制器
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern osThreadId ModeHandle;
#define REV_SPD injector_speed_t[3][0]		/*	回退速度		*/
#define FWD_SPD	injector_speed_t[1][1]		/*	前进速度		*/
#define ASP_SPD injector_speed_t[1][0]		/*	回吸速度		*/
void motor_controller(void)
{
	static uint16_t end_cycle = 0;
	static uint16_t now_cycle = 0;
	if(xSemaphoreTake(sMotor_goBottom, (TickType_t)0) == pdTRUE){
		GO_BOTTOM_FLAG = true;
		disable_vgus_touch();	/*	关闭触摸	*/
		set_speed_mode_speed(-REV_SPD);
		while( !check_if_motor_reach_bottom() ){	
			osDelay(50);
			now_cycle = (get_current_handle_injector() >> 16);
			CONTROLLER_PRINTF("regi[0x5F] is %d, current cycle is %d, bootom_cycle is %d!\r\n", regi[0x5F], now_cycle, (handle_injector_t.BOTTOM_LOC >> 16));
			if(iq < -60)
				break;
		}
		set_motor_stop();
		handle_in_bottom = true;
		enable_vgus_touch();	/*	开启触摸	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_BOTTOM_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goTop, (TickType_t)0) == pdTRUE){
		GO_TOP_FLAG = true;
		disable_vgus_touch();	/*	关闭触摸	*/
		set_speed_mode_speed(FWD_SPD);
		while( !check_if_motor_reach_top() ){	
			osDelay(50);
			if(iq > 60)
				break;
		}
		set_motor_stop();
		handle_in_top = true;
		enable_vgus_touch();	/*	开启触摸	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_TOP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_ASP, (TickType_t)0) == pdTRUE){
		START_ASP_FLAG = true;
		disable_vgus_touch();	/*	关闭触摸	*/
		/*	回吸反转阶段	*/
		end_cycle = compute_end_cycle(-asp_cycle);
		set_speed_mode_speed(-ASP_SPD);
		while(!check_if_motor_reach_goal(end_cycle))
			osDelay(50);
		set_motor_stop();
		osDelay(100);
		/*	回吸正转阶段	*/
		end_cycle = compute_end_cycle(asp_cycle);
		set_speed_mode_speed(ASP_SPD);
		while(!check_if_motor_reach_goal(end_cycle))
			osDelay(50);
		set_motor_stop();
		
		enable_vgus_touch();	/*	开启触摸	*/
		xSemaphoreTake(sMotor_goTop, (TickType_t)0);
		xSemaphoreTake(sMotor_goBottom, (TickType_t)0);
		START_ASP_FLAG = false;
	}else
		osDelay(10);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-20
* 函 数 名: Check_if_Motor_Reach_Bottom
* 功能说明: 判断电机是否已到达底部
*
* 形    参: 
      
* 返 回 值:
*		true: 电机已到达底部
*		false: 电机未到达底部
*********************************************************************************************************
*/
static bool check_if_motor_reach_bottom(void)
{
	static uint16_t current_pos = 0;
	static uint16_t bottom_pos = 0;
	current_pos = (get_current_handle_injector() >> 16);
	bottom_pos = (handle_injector_t.BOTTOM_LOC >> 16);
	if(bottom_pos < 5)					/*	防止end_cycle = 0时，程序未及时判断，造成超距回退	*/
		bottom_pos = 65535;
	
	if((current_pos&0x8000)==(bottom_pos&0x8000))
		if(current_pos <= bottom_pos)
			return true;												
	return false;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-24
* 函 数 名: check_if_motor_reach_top
* 功能说明: 判断电机是否已到达顶部
*
* 形    参: 
      
* 返 回 值:
*		true: 电机已到达底部
*		false: 电机未到达底部
*********************************************************************************************************
*/
static bool check_if_motor_reach_top(void)
{
	static uint16_t current_pos = 0;
	static uint16_t top_pos = 0;
	current_pos = (get_current_handle_injector() >> 16);
	top_pos = (handle_injector_t.TOP_LOC >> 16);
	if(top_pos >65530)					/*	防止end_cycle = 0时，程序未及时判断，造成超距回退	*/
		top_pos = 0;
	
	if((current_pos&0x8000)==(top_pos&0x8000))
		if(current_pos >= top_pos)
			return true;												
	return false;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-04-24
* 函 数 名: check_if_motor_reach_goal
* 功能说明: 判断电机是否已到达目标位置或底部
*
* 形    参: 
			start_cycle: 起始时电机所在圈数
			end_cycle: 结束时电机所在圈数
* 返 回 值:
*		true: 电机已到达目标位置或底部
*		false: 电机未到达目标位置或底部
*********************************************************************************************************
*/
static bool check_if_motor_reach_goal(uint16_t end_cycle)
{
	static uint16_t current_cycle = 0;
	static uint16_t bottom_cycle = 0;
	static uint16_t top_cycle = 0;
	current_cycle = (get_current_handle_injector() >> 16);
	bottom_cycle = (handle_injector_t.BOTTOM_LOC >> 16);
	top_cycle = (handle_injector_t.TOP_LOC >> 16);
	if(bottom_cycle < 5)
		bottom_cycle = 65535;
	if(top_cycle > 65530)
		top_cycle = 0;
	
	if( (current_cycle&0x8000) == (end_cycle&0x8000) ){
		if( (current_cycle >= end_cycle) && (current_cycle- end_cycle <= 5))	/*	在目标位置+5范围内	*/
			return true;
		else if( (current_cycle < end_cycle) && (end_cycle- current_cycle <= 5))/*	在目标位置-5范围内	*/
			return true;
	}
		
	if( (current_cycle&0x8000)==(bottom_cycle&0x8000) ){
		if(current_cycle <= bottom_cycle)				/*	当前位置已低于最低位置	*/
			return true;
	}
		
	if( (current_cycle&0x8000)==(top_cycle&0x8000) ){
		if(current_cycle >= top_cycle)					/*	当前位置已高于最高位置	*/
			return true;
	}
	
	return false;										/*	未达到目标位置	*/
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-04-22
* 函 数 名: compute_end_cycle
* 功能说明: 根据电机端（转子）回退的圈数，计算终点圈数
*
* 形    参: 
*			cycles: 电机运转的圈数，整数表示正转，负数表示反转
* 返 回 值:
*		电机端（转子）回退的圈数
*********************************************************************************************************
*/
static uint16_t compute_end_cycle(int16_t cycles)
{
	uint16_t end_cycle = 0;
	uint16_t buf1 = cycles;
	uint16_t buf2 = (uint16_t)(get_current_handle_injector() >> 16);
	end_cycle = buf2 + buf1;
	if(end_cycle < 5)			/*	防止end_cycle = 0时，程序未及时判断，造成超距回退	*/
		end_cycle = 65535;
	return end_cycle;
}
