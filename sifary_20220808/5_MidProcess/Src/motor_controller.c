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
static bool check_if_motor_reach_origin(void);
static bool check_if_motor_reach_top(void);
static bool check_if_motor_reach_goal(void);
static uint16_t compute_end_cycle(int16_t cycles);

SemaphoreHandle_t	sMotor_goOrigin = NULL;		/*	����һ���������ź������������ִ֪ͨ�е��������	*/
SemaphoreHandle_t	sMotor_ASP = NULL;			/*	����һ���������ź������������ִ֪ͨ�е������	*/
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
	sMotor_goOrigin = xSemaphoreCreateBinary();	/*	��ʼ����ֵ�ź��� sMotor_goOrigin	*/
	sMotor_ASP = xSemaphoreCreateBinary();
	sMotor_goTop = xSemaphoreCreateBinary();
	sMotor_goBottom = xSemaphoreCreateBinary();;
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
#define REV_SPD injector_speed_t[3][2]		/*	�����ٶ�		*/
#define FWD_SPD	injector_speed_t[3][2]		/*	ǰ���ٶ�		*/
#define ASP_SPD injector_speed_t[3][0]		/*	�����ٶ�		*/
void motor_controller(void)
{
	static uint16_t cycle_buf = 0;
	uint8_t cnt = 0;
	
	if(xSemaphoreTake(sMotor_goOrigin, (TickType_t)0) == pdTRUE){		//	�ص�Һѹԭ��
		GO_BOTTOM_FLAG = true;
		disable_vgus_touch();	/*	�رմ���	*/
		set_speed_mode_speed(-REV_SPD);
		while( !check_if_motor_reach_origin() ){	
			osDelay(50);
		}
		handle_in_bottom = true;
		enable_vgus_touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_BOTTOM_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goTop, (TickType_t)0) == pdTRUE){	//	�ص�˿�˶���
		GO_TOP_FLAG = true;
		disable_vgus_touch();	/*	�رմ���	*/
		set_speed_mode_speed(FWD_SPD);
		while( !check_if_motor_reach_top() ){	
			osDelay(50);
		}
		handle_in_top = true;
		enable_vgus_touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_TOP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_ASP, (TickType_t)0) == pdTRUE){		//	��������
		START_ASP_FLAG = true;
		disable_vgus_touch();	/*	�رմ���	*/
		
		/*	������ת�׶�	*/
		cycle_buf = (get_current_handle_injector() >> 16);
		motor_to_goal_cycle = compute_end_cycle(-asp_cycle);	//	����������
		set_speed_mode_speed(-ASP_SPD);
		motor_run_direction = MOTOR_REV_TO_GOAL;
		while(!check_if_motor_reach_goal() )
			osDelay(50);
		osDelay(100);
		
		/*	������ת�׶�	*/
		motor_to_goal_cycle = cycle_buf;						//	ǰ�����ص�֮ǰ��λ��
		set_speed_mode_speed(ASP_SPD);
		motor_run_direction = MOTOR_FWD_TO_GOAL;
		while(!check_if_motor_reach_goal() )
			osDelay(50);
		
		enable_vgus_touch();	/*	��������	*/
		xSemaphoreTake(sMotor_goTop, (TickType_t)0);
		xSemaphoreTake(sMotor_goOrigin, (TickType_t)0);
		START_ASP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goBottom, (TickType_t)0) == pdTRUE){	//	�ϵ磬˿�˻ص��ײ�
		disable_vgus_touch();	/*	�رմ���	*/
		set_speed_mode_speed(-REV_SPD);
		while( !check_if_motor_reach_origin() ){	
			osDelay(50);
			if(++cnt == 10){
				disable_vgus_touch();
				cnt = 0;
			}		
		}
		enable_vgus_touch();	/*	��������	*/
	}else	
		osDelay(10);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-07-13
* �� �� ��: check_if_motor_reach_origin
* ����˵��: �жϵ���Ƿ��ѵ���ԭ��
*
* ��    ��: 
      
* �� �� ֵ:
*		true:  ����ѵ���ԭ��
*		false: ���δ����ԭ��
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-07-13
* �� �� ��: check_if_motor_reach_top
* ����˵��: �жϵ���Ƿ��ѵ��ﶥ��
*
* ��    ��: 
      
* �� �� ֵ:
*		true:  ����ѵ��ﶥ��
*		false: ���δ���ﶥ��
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.2
* ��    �ڣ�2022-07-13
* �� �� ��: check_if_motor_reach_goal
* ����˵��: �жϵ���Ƿ��ѵ���Ŀ��λ�û�ײ�
*
* ��    ��: 

* �� �� ֵ:
*		true:  ����ѵ���Ŀ��λ��
*		false: ���δ����Ŀ��λ��
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.2
* ��    �ڣ�2022-07-13
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
	uint16_t buf2 = (uint16_t)(get_current_handle_injector() >> 16);	/*	��ǰ�������λ�ã� ��λ��Ȧ��	*/
	uint16_t top_cycle = (handle_injector_t.TOP_LOC >>16);
	uint16_t bottom_cycle = (handle_injector_t.BOTTOM_LOC >>16);
	
	end_cycle = (uint16_t)(buf2 + cycles);
	CONTROLLER_PRINTF("current= %d, need= %d, end= %d\r\n", buf2, cycles, end_cycle);
	if(cycles < 0){		// ����
		if((end_cycle >= bottom_cycle)&&(end_cycle < top_cycle))	
			return end_cycle;
		else
			return bottom_cycle;
	}else{				// ǰ��
		if((end_cycle <= top_cycle)&&(end_cycle > bottom_cycle))
			return end_cycle;
		else
			return top_cycle;
	}
}
