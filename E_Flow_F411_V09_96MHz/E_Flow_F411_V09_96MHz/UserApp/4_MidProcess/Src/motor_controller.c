#include "Motor_Controller.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"

/*	����Ӳ��	*/
#include "motor_misc.h"
#include "vgus_lcd_if.h"

/*	midprocess	*/
#include "Page_Mode.h"

/*	APP&DATA	*/
#include "user_data.h"

static bool check_if_motor_reach_origin(MOTOR_DIRECTION_T motor_dir_t);
static bool check_if_motor_reach_bottom(MOTOR_DIRECTION_T motor_dir_t);
static bool check_if_motor_reach_top(MOTOR_DIRECTION_T motor_dir_t);
static bool check_if_motor_reach_goal(MOTOR_DIRECTION_T motor_dir_t);
static uint16_t compute_end_cycle(int16_t cycles);

SemaphoreHandle_t	sMotor_goOrigin = NULL;		/*	����һ���������ź������������ִ֪ͨ�е��������	*/
SemaphoreHandle_t	sMotor_goOrigin2 = NULL;	/*	����һ���������ź������������ִ֪ͨ�е��������	*/
SemaphoreHandle_t	sMotor_ASP = NULL;			/*	����һ���������ź������������ִ֪ͨ�е������	*/
SemaphoreHandle_t	sMotor_goTop = NULL;
SemaphoreHandle_t	sMotor_goBottom = NULL;


void Motor_Controller_Alarm(void)
{
	static uint8_t CTR_F_100MS = 0;
	if(++CTR_F_100MS > 4){
		CTR_F_100MS = 0;
		if(ASP_RUNNING_FLAG == true)
			Play_Now_Music(ASPIRATE_BACK_INDEX, system_volume[Get_System_Vol()]);
		else if((GO_TOP_FLAG == true)||(GO_ORIGIN_FLAG == true))
			Play_Now_Music(CHANGE_INDEX, system_volume[Get_System_Vol()]);
		else
			return;
	}
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-03-02
* �� �� ��: Motor_Controller_Init
* ����˵��: �����������ʼ��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Motor_Controller_Init(void)
{
	sMotor_goOrigin = xSemaphoreCreateBinary();	/*	��ʼ����ֵ�ź��� sMotor_goOrigin	*/
	sMotor_goOrigin2 = xSemaphoreCreateBinary();	/*	��ʼ����ֵ�ź��� sMotor_goOrigin	*/
	sMotor_ASP = xSemaphoreCreateBinary();
	sMotor_goTop = xSemaphoreCreateBinary();
	sMotor_goBottom = xSemaphoreCreateBinary();;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-03-09
* �� �� ��: Motor_Controller
* ����˵��: ���������
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern osThreadId ModeHandle;
#define CHG_REV_SPD_TIME	20
#define REV_SPD_LOW			injector_speed_t[2][0]		/*	�����ٶ�LOW		*/
#define REV_SPD 			injector_speed_t[3][2]		/*	�����ٶ�		*/
#define FWD_SPD				injector_speed_t[3][2]		/*	ǰ���ٶ�		*/
#define ASP_SPD 			injector_speed_t[3][0]		/*	�����ٶ�		*/
void Motor_Controller(void)
{
	static uint16_t cycle_buf = 0;
	static uint8_t cnt, cnt2 = 0;
	
	if(xSemaphoreTake(sMotor_goOrigin, (TickType_t)0) == pdTRUE){		//	�ص�Һѹԭ��
		GO_ORIGIN_FLAG = true;
		Disable_VGUS_Touch();	/*	�رմ���	*/
		cnt2 = 0;
		set_speed_mode_speed(-REV_SPD_LOW, MOTOR_REV_TO_ORIGIN);
		while( !check_if_motor_reach_origin(motor_run_direction) ){	
			osDelay(50);
			if(++cnt == 10){
				Disable_VGUS_Touch();
				cnt = 0;
			}
			if(cnt2 == CHG_REV_SPD_TIME){
				set_speed_mode_speed(-REV_SPD, MOTOR_REV);		//	����CHG_REV_SPD_TIME*50ms��ı����ٶ�
				cnt2 ++;
			}else if(cnt2 > CHG_REV_SPD_TIME)
				cnt2 = CHG_REV_SPD_TIME+1;
			else
				cnt2 ++;
		}
		Cycle_Reset_To_Origin();
		Enable_VGUS_Touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_ORIGIN_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goOrigin2, (TickType_t)0) == pdTRUE){		//	�ص�Һѹԭ��
		GO_ORIGIN_FLAG = true;
		Disable_VGUS_Touch();	/*	�رմ���	*/
		cnt2 = 0;
		set_speed_mode_speed(-REV_SPD_LOW, MOTOR_REV);
		while( !check_if_motor_reach_bottom(motor_run_direction) ){	
			osDelay(50);
			if(++cnt == 10){
				Disable_VGUS_Touch();
				cnt = 0;
			}
			if(cnt2 == CHG_REV_SPD_TIME){
				set_speed_mode_speed(-REV_SPD, MOTOR_REV);		//	����CHG_REV_SPD_TIME*50ms��ı����ٶ�
				cnt2 ++;
			}else if(cnt2 > CHG_REV_SPD_TIME)
				cnt2 = CHG_REV_SPD_TIME+1;
			else
				cnt2 ++;
		}
		motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
		set_speed_mode_speed(FWD_SPD, MOTOR_FWD_TO_ORIGIN);
		while( !check_if_motor_reach_origin(motor_run_direction) ){	
			osDelay(50);
			if(++cnt == 10){
				Disable_VGUS_Touch();
				cnt = 0;
			}		
		}
		Cycle_Reset_To_Origin();
		Enable_VGUS_Touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_ORIGIN_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goTop, (TickType_t)0) == pdTRUE){	//	�ص�˿�˶���
		GO_TOP_FLAG = true;
		Disable_VGUS_Touch();	/*	�رմ���	*/
		set_speed_mode_speed(FWD_SPD, MOTOR_FWD);
		while( !check_if_motor_reach_top(motor_run_direction) ){	
			osDelay(50);
		}
		handle_in_top = true;
		Enable_VGUS_Touch();	/*	��������	*/
		xSemaphoreTake(sMotor_ASP, (TickType_t)0);
		GO_TOP_FLAG = false;
	}else if(xSemaphoreTake(sMotor_ASP, (TickType_t)0) == pdTRUE){		//	��������
		ASP_RUNNING_FLAG = true;
		Disable_VGUS_Touch();	/*	�رմ���	*/
		
		/*	������ת�׶�	*/
		cycle_buf = (Get_Current_Handle_Injector_Pos() >> 16);
		motor_to_goal_cycle = compute_end_cycle(-asp_cycle);	//	����������
		set_speed_mode_speed(-ASP_SPD, MOTOR_REV_TO_GOAL);
		while(!check_if_motor_reach_goal(motor_run_direction) )
			osDelay(50);
		osDelay(100);
		
		/*	������ת�׶�	*/
		motor_to_goal_cycle = cycle_buf;						//	ǰ�����ص�֮ǰ��λ��
		set_speed_mode_speed(ASP_SPD, MOTOR_FWD_TO_GOAL);
		while(!check_if_motor_reach_goal(motor_run_direction) )
			osDelay(50);
		
		Enable_VGUS_Touch();	/*	��������	*/
		xSemaphoreTake(sMotor_goTop, (TickType_t)0);
		xSemaphoreTake(sMotor_goOrigin, (TickType_t)0);
		ASP_RUNNING_FLAG = false;
	}else if(xSemaphoreTake(sMotor_goBottom, (TickType_t)0) == pdTRUE){	//	�ϵ磬˿�˻ص��ײ�
		Disable_VGUS_Touch();	/*	�رմ���	*/
		set_speed_mode_speed(-REV_SPD, MOTOR_REV);
		while( !check_if_motor_reach_bottom(motor_run_direction) ){	
			osDelay(50);
			if(++cnt == 10){
				Disable_VGUS_Touch();
				cnt = 0;
			}
		}
		motor_to_origin_cycle = (((motor_cycle <<16) + motor_angle + handle_injector_t.BOTTM2ORIGIN) >>16);
		set_speed_mode_speed(FWD_SPD, MOTOR_FWD_TO_ORIGIN);
		while( !check_if_motor_reach_origin(motor_run_direction) ){	
			osDelay(50);
			if(++cnt == 10){
				Disable_VGUS_Touch();
				cnt = 0;
			}		
		}
		Cycle_Reset_To_Origin();
		Enable_VGUS_Touch();	/*	��������	*/
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
static bool check_if_motor_reach_origin(MOTOR_DIRECTION_T motor_dir_t)
{
	CONTROLLER_PRINTF("current_cycle = %d, origin_cycle = %d!\r\n",(Get_Current_Handle_Injector_Pos() >>16), (handle_injector_t.ORIGIN_LOC >>16));
	if(motor_dir_t == MOTOR_ORIGIN)
		return true;
	else	
		return false;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-08-11
* �� �� ��: check_if_motor_reach_bottom
* ����˵��: �жϵ���Ƿ��ѵ���׶�
*
* ��    ��: 
      
* �� �� ֵ:
*		true:  ����ѵ���׶�
*		false: ���δ����׶�
*********************************************************************************************************
*/
static bool check_if_motor_reach_bottom(MOTOR_DIRECTION_T motor_dir_t)
{
	CONTROLLER_PRINTF("current_cycle = %d, origin_cycle = %d!\r\n",(Get_Current_Handle_Injector_Pos() >>16), (handle_injector_t.ORIGIN_LOC >>16));
	if(motor_dir_t == MOTOR_BOTTOM)
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
static bool check_if_motor_reach_top(MOTOR_DIRECTION_T motor_dir_t)
{
	CONTROLLER_PRINTF("current_cycle = %d, top_cycle = %d!\r\n",(Get_Current_Handle_Injector_Pos() >>16), (handle_injector_t.TOP_LOC >>16));
	if(motor_dir_t == MOTOR_TOP)
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
static bool check_if_motor_reach_goal(MOTOR_DIRECTION_T motor_dir_t)
{
	CONTROLLER_PRINTF("current_cycle = %d, goal_cycle = %d\r\n", (Get_Current_Handle_Injector_Pos() >> 16), motor_to_goal_cycle);
	switch(motor_dir_t){
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
	uint16_t buf2 = (uint16_t)(Get_Current_Handle_Injector_Pos() >> 16);	/*	��ǰ�������λ�ã� ��λ��Ȧ��	*/
	uint16_t top_cycle = (handle_injector_t.TOP_LOC >>16);
	uint16_t origin_cycle = (handle_injector_t.ORIGIN_LOC >>16);
	
	end_cycle = (uint16_t)(buf2 + cycles);
	CONTROLLER_PRINTF("top_cycle= %d, origin_cycle= %d, current= %d, need= %d, end= %d\r\n", top_cycle, origin_cycle, buf2, cycles, end_cycle);
	if(cycles < 0){		// ����
		if((end_cycle >= origin_cycle)&&(end_cycle < top_cycle))	
			return end_cycle;
		else
			return origin_cycle;
	}else{				// ǰ��
		if((end_cycle <= top_cycle)&&(end_cycle > origin_cycle))
			return end_cycle;
		else
			return top_cycle;
	}
}
