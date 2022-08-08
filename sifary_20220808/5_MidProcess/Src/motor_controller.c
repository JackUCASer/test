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
static bool check_if_motor_reach_origin(void);
static bool check_if_motor_reach_top(void);
static bool check_if_motor_reach_goal(void);
static uint16_t compute_end_cycle(int16_t cycles);

SemaphoreHandle_t	sMotor_goOrigin = NULL;		/*	定义一个二进制信号量句柄，用于通知执行电机控制器	*/
SemaphoreHandle_t	sMotor_ASP = NULL;			/*	定义一个二进制信号量句柄，用于通知执行电机回吸	*/
SemaphoreHandle_t	sMotor_goTop = NULL;
SemaphoreHandle_t	sMotor_goBottom = NULL;


void motor_controller_alarm(void)
{
	static uint8_t CTR_F_100MS = 0;
	if(++CTR_F_100MS > 4){
		CTR_F_100MS = 0;
		if(START_ASP_FLAG == true)
			play_now_music(ASPIRATE_BACK_INDEX, system_volume[get_system_vol()]);
		else if((GO_TOP_FLAG == true) ||(GO_BOTTOM_FLAG == true))
			play_now_music(CHANGE_INDEX, system_volume[get_system_vol()]);
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
	sMotor_goOrigin = xSemaphoreCreateBinary();	/*	初始化二值信号量 sMotor_goOrigin	*/
	sMotor_ASP = xSemaphoreCreateBinary();
	sMotor_goTop = xSemaphoreCreateBinary();
	sMotor_goBottom = xSemaphoreCreateBinary();;
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
#define REV_SPD injector_speed_t[3][2]		/*	回退速度		*/
#define FWD_SPD	injector_speed_t[3][2]		/*	前进速度		*/
#define ASP_SPD injector_speed_t[3][0]		/*	回吸速度		*/
void motor_controller(void)
{
	static uint16_t cycle_buf = 0;
	uint8_t cnt = 0;
	
	if(xSemaphoreTake(sMotor_goOrigin, (TickType_t)0) == pdTRUE){		//	回到液压原点
		GO_BOTTOM_FLAG = true;
		disable_vgus_touch();	/*	关闭触摸	*/
		set_speed_mode_speed(-REV_SPD);
		while( !check_if_motor_reach_origin() ){	
			osDelay(50);
		}
		handle_in_bottom = true;
		enable_vgus_touch();	/*	开启触摸	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_BOTTOM_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goTop, (TickType_t)0) == pdTRUE){	//	回到丝杆顶点
		GO_TOP_FLAG = true;
		disable_vgus_touch();	/*	关闭触摸	*/
		set_speed_mode_speed(FWD_SPD);
		while( !check_if_motor_reach_top() ){	
			osDelay(50);
		}
		handle_in_top = true;
		enable_vgus_touch();	/*	开启触摸	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_TOP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_ASP, (TickType_t)0) == pdTRUE){		//	回吸程序
		START_ASP_FLAG = true;
		disable_vgus_touch();	/*	关闭触摸	*/
		
		/*	回吸反转阶段	*/
		cycle_buf = (get_current_handle_injector() >> 16);
		motor_to_goal_cycle = compute_end_cycle(-asp_cycle);	//	回吸：回退
		set_speed_mode_speed(-ASP_SPD);
		motor_run_direction = MOTOR_REV_TO_GOAL;
		while(!check_if_motor_reach_goal() )
			osDelay(50);
		osDelay(100);
		
		/*	回吸正转阶段	*/
		motor_to_goal_cycle = cycle_buf;						//	前进：回到之前的位置
		set_speed_mode_speed(ASP_SPD);
		motor_run_direction = MOTOR_FWD_TO_GOAL;
		while(!check_if_motor_reach_goal() )
			osDelay(50);
		
		enable_vgus_touch();	/*	开启触摸	*/
		xSemaphoreTake(sMotor_goTop, (TickType_t)0);
		xSemaphoreTake(sMotor_goOrigin, (TickType_t)0);
		START_ASP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goBottom, (TickType_t)0) == pdTRUE){	//	上电，丝杆回到底部
		disable_vgus_touch();	/*	关闭触摸	*/
		set_speed_mode_speed(-REV_SPD);
		while( !check_if_motor_reach_origin() ){	
			osDelay(50);
			if(++cnt == 10){
				disable_vgus_touch();
				cnt = 0;
			}		
		}
		enable_vgus_touch();	/*	开启触摸	*/
	}else	
		osDelay(10);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-07-13
* 函 数 名: check_if_motor_reach_origin
* 功能说明: 判断电机是否已到达原点
*
* 形    参: 
      
* 返 回 值:
*		true:  电机已到达原点
*		false: 电机未到达原点
*********************************************************************************************************
*/
static bool check_if_motor_reach_origin(void)
{
	CONTROLLER_PRINTF("current cycle is %d, bootom_cycle is %d!\r\n", (get_current_handle_injector() >>16), (handle_injector_t.BOTTOM_LOC >>16));
	if(motor_run_direction == MOTOR_ORIGIN)
		return true;
	else	
		return false;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-07-13
* 函 数 名: check_if_motor_reach_top
* 功能说明: 判断电机是否已到达顶部
*
* 形    参: 
      
* 返 回 值:
*		true:  电机已到达顶部
*		false: 电机未到达顶部
*********************************************************************************************************
*/
static bool check_if_motor_reach_top(void)
{
	CONTROLLER_PRINTF("current cycle is %d, top_cycle is %d!\r\n", (get_current_handle_injector() >>16), (handle_injector_t.TOP_LOC >>16));
	if(motor_run_direction == MOTOR_TOP)
		return true;
	else	
		return false;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.2
* 日    期：2022-07-13
* 函 数 名: check_if_motor_reach_goal
* 功能说明: 判断电机是否已到达目标位置或底部
*
* 形    参: 

* 返 回 值:
*		true:  电机已到达目标位置
*		false: 电机未到达目标位置
*********************************************************************************************************
*/
static bool check_if_motor_reach_goal(void)
{
	CONTROLLER_PRINTF("current_cycle = %d, goal_cycle = %d\r\n", (get_current_handle_injector() >> 16), motor_to_goal_cycle);
	switch(motor_run_direction){
		case MOTOR_FWD_GOAL: return true;
		
		case MOTOR_REV_GOAL: return true;
		
		default: return false;
	}
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.2
* 日    期：2022-07-13
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
	uint16_t buf2 = (uint16_t)(get_current_handle_injector() >> 16);	/*	当前电机所在位置， 单位：圈数	*/
	uint16_t top_cycle = (handle_injector_t.TOP_LOC >>16);
	uint16_t bottom_cycle = (handle_injector_t.BOTTOM_LOC >>16);
	
	end_cycle = (uint16_t)(buf2 + cycles);
	CONTROLLER_PRINTF("current= %d, need= %d, end= %d\r\n", buf2, cycles, end_cycle);
	if(cycles < 0){		// 回退
		if((end_cycle >= bottom_cycle)&&(end_cycle < top_cycle))	
			return end_cycle;
		else
			return bottom_cycle;
	}else{				// 前进
		if((end_cycle <= top_cycle)&&(end_cycle > bottom_cycle))
			return end_cycle;
		else
			return top_cycle;
	}
}
