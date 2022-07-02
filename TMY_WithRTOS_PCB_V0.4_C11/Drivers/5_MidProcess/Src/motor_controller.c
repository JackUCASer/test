#include "motor_controller.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	����Ӳ��	*/
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

SemaphoreHandle_t	sMotor_goBottom = NULL;		/*	����һ���������ź������������ִ֪ͨ�е��������	*/
SemaphoreHandle_t	sMotor_ASP = NULL;			/*	����һ���������ź������������ִ֪ͨ�е������	*/
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-03-02
* �� �� ��: motor_controller_init
* ����˵��: �����������ʼ��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void motor_controller_init(void)
{
	sMotor_goBottom = xSemaphoreCreateBinary();	/*	��ʼ����ֵ�ź��� sMotor_goBottom	*/
	sMotor_ASP = xSemaphoreCreateBinary();		/*	��ʼ����ֵ�ź��� sMotor_ASP	*/
	sMotor_goTop = xSemaphoreCreateBinary();	/*	��ʼ����ֵ�ź��� sMotor_ASP	*/
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-03-09
* �� �� ��: motor_controller
* ����˵��: ���������
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern osThreadId ModeHandle;
#define REV_SPD injector_speed_t[3][0]		/*	�����ٶ�		*/
#define FWD_SPD	injector_speed_t[1][1]		/*	ǰ���ٶ�		*/
#define ASP_SPD injector_speed_t[1][0]		/*	�����ٶ�		*/
void motor_controller(void)
{
	static uint16_t end_cycle = 0;
	static uint16_t now_cycle = 0;
	if(xSemaphoreTake(sMotor_goBottom, (TickType_t)0) == pdTRUE){
		GO_BOTTOM_FLAG = true;
		disable_vgus_touch();	/*	�رմ���	*/
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
		enable_vgus_touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_BOTTOM_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goTop, (TickType_t)0) == pdTRUE){
		GO_TOP_FLAG = true;
		disable_vgus_touch();	/*	�رմ���	*/
		set_speed_mode_speed(FWD_SPD);
		while( !check_if_motor_reach_top() ){	
			osDelay(50);
			if(iq > 60)
				break;
		}
		set_motor_stop();
		handle_in_top = true;
		enable_vgus_touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_TOP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_ASP, (TickType_t)0) == pdTRUE){
		START_ASP_FLAG = true;
		disable_vgus_touch();	/*	�رմ���	*/
		/*	������ת�׶�	*/
		end_cycle = compute_end_cycle(-asp_cycle);
		set_speed_mode_speed(-ASP_SPD);
		while(!check_if_motor_reach_goal(end_cycle))
			osDelay(50);
		set_motor_stop();
		osDelay(100);
		/*	������ת�׶�	*/
		end_cycle = compute_end_cycle(asp_cycle);
		set_speed_mode_speed(ASP_SPD);
		while(!check_if_motor_reach_goal(end_cycle))
			osDelay(50);
		set_motor_stop();
		
		enable_vgus_touch();	/*	��������	*/
		xSemaphoreTake(sMotor_goTop, (TickType_t)0);
		xSemaphoreTake(sMotor_goBottom, (TickType_t)0);
		START_ASP_FLAG = false;
	}else
		osDelay(10);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-20
* �� �� ��: Check_if_Motor_Reach_Bottom
* ����˵��: �жϵ���Ƿ��ѵ���ײ�
*
* ��    ��: 
      
* �� �� ֵ:
*		true: ����ѵ���ײ�
*		false: ���δ����ײ�
*********************************************************************************************************
*/
static bool check_if_motor_reach_bottom(void)
{
	static uint16_t current_pos = 0;
	static uint16_t bottom_pos = 0;
	current_pos = (get_current_handle_injector() >> 16);
	bottom_pos = (handle_injector_t.BOTTOM_LOC >> 16);
	if(bottom_pos < 5)					/*	��ֹend_cycle = 0ʱ������δ��ʱ�жϣ���ɳ������	*/
		bottom_pos = 65535;
	
	if((current_pos&0x8000)==(bottom_pos&0x8000))
		if(current_pos <= bottom_pos)
			return true;												
	return false;
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-04-24
* �� �� ��: check_if_motor_reach_top
* ����˵��: �жϵ���Ƿ��ѵ��ﶥ��
*
* ��    ��: 
      
* �� �� ֵ:
*		true: ����ѵ���ײ�
*		false: ���δ����ײ�
*********************************************************************************************************
*/
static bool check_if_motor_reach_top(void)
{
	static uint16_t current_pos = 0;
	static uint16_t top_pos = 0;
	current_pos = (get_current_handle_injector() >> 16);
	top_pos = (handle_injector_t.TOP_LOC >> 16);
	if(top_pos >65530)					/*	��ֹend_cycle = 0ʱ������δ��ʱ�жϣ���ɳ������	*/
		top_pos = 0;
	
	if((current_pos&0x8000)==(top_pos&0x8000))
		if(current_pos >= top_pos)
			return true;												
	return false;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-04-24
* �� �� ��: check_if_motor_reach_goal
* ����˵��: �жϵ���Ƿ��ѵ���Ŀ��λ�û�ײ�
*
* ��    ��: 
			start_cycle: ��ʼʱ�������Ȧ��
			end_cycle: ����ʱ�������Ȧ��
* �� �� ֵ:
*		true: ����ѵ���Ŀ��λ�û�ײ�
*		false: ���δ����Ŀ��λ�û�ײ�
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
		if( (current_cycle >= end_cycle) && (current_cycle- end_cycle <= 5))	/*	��Ŀ��λ��+5��Χ��	*/
			return true;
		else if( (current_cycle < end_cycle) && (end_cycle- current_cycle <= 5))/*	��Ŀ��λ��-5��Χ��	*/
			return true;
	}
		
	if( (current_cycle&0x8000)==(bottom_cycle&0x8000) ){
		if(current_cycle <= bottom_cycle)				/*	��ǰλ���ѵ������λ��	*/
			return true;
	}
		
	if( (current_cycle&0x8000)==(top_cycle&0x8000) ){
		if(current_cycle >= top_cycle)					/*	��ǰλ���Ѹ������λ��	*/
			return true;
	}
	
	return false;										/*	δ�ﵽĿ��λ��	*/
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-04-22
* �� �� ��: compute_end_cycle
* ����˵��: ���ݵ���ˣ�ת�ӣ����˵�Ȧ���������յ�Ȧ��
*
* ��    ��: 
*			cycles: �����ת��Ȧ����������ʾ��ת��������ʾ��ת
* �� �� ֵ:
*		����ˣ�ת�ӣ����˵�Ȧ��
*********************************************************************************************************
*/
static uint16_t compute_end_cycle(int16_t cycles)
{
	uint16_t end_cycle = 0;
	uint16_t buf1 = cycles;
	uint16_t buf2 = (uint16_t)(get_current_handle_injector() >> 16);
	end_cycle = buf2 + buf1;
	if(end_cycle < 5)			/*	��ֹend_cycle = 0ʱ������δ��ʱ�жϣ���ɳ������	*/
		end_cycle = 65535;
	return end_cycle;
}
