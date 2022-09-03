#include "user_data.h"
#include <flashdb.h>		/*	���ݿ�����	*/
#include "lcd_display.h"
#include "motor_misc.h"
#include "eeprom.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"


//	ϵͳ����: ��д�����ݿ�
SYS_PARA_T sys_para_t = {0};

//	��̤: ��д�����ݿ�
FOOT_PEDAL_T	foot_pedal_t = {0};

//	�ֱ�: ��д�����ݿ�: ���ڼ�¼�ֱ�ע���������г�[���������Ȧ�����Ƕ�]
HANDLE_INJECTOR_T handle_injector_t = {0};

//	Һѹ����
float32_t dynamic_pressure_mV = 0;
float32_t dynamic_pressure_kPa = 0;
uint32_t  dynamic_pressure = 0;
uint16_t  dynamic_pressure_kPa_F = 0;		/*	Hampel�˲����Һѹֵ	*/

//	ȫ�ֱ�־λ
bool handle_in_top = false;
bool handle_in_bottom = false;
bool handle_in_origin = false;
bool sys_para_update_flag = false;


//	ϵͳ�еĵ��ת�٣���ǣ��У�������⣬���Բ���д�����ݿ�
/* 	LOW,	MID,	HIGH	*/
const uint32_t injector_speed_t[4][3] = {
	{50*F_DIV,	50*F_DIV,	350*F_DIV},				/*	Normal	�ٶ�	[LOW, LOW, MID]	*/
	{50*F_DIV,	350*F_DIV,	700*F_DIV},				/*	Turbo 	�ٶ�	[LOW, MID, HIGH]*/
	{50*F_DIV,	50*F_DIV,	50*F_DIV},				/*	STA   	�ٶ�	[LOW, LOW, LOW]	*/
	{150*F_DIV,	500*F_DIV,	700*F_DIV},				/*	ASP		����1		����2	*/
	};

//	���������Ⱥ������б�
const uint8_t system_light[MAX_LIGHT_INDEX+1] = {10, 20, 30, 40, 52, 64};
const uint8_t system_volume[MAX_VOLUME_INDEX+1]= {0, 30, 40, 48, 56, 64};

/**************************���¹������ݿ�����************************/
/*	1. Aspirate��������	*/
bool open_asp(void)
{
	sys_para_t.ASPIRATE = 1;
	if((is_train_mode_open() == true)||(is_smart_reminder_open() == true))
		play_now_music(ASPIRATE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

bool close_asp(void)
{
	sys_para_t.ASPIRATE = 0;
	if((is_train_mode_open() == true)||(is_smart_reminder_open() == true))
		play_now_music(ASPIRATE_OFF_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return false;
}
bool is_asp_open(void)
{
	if(sys_para_t.ASPIRATE == 1)
		return true;
	else
		return false;
}

/*	2. smart_reminder��������	*/
bool open_smart_reminder(void)
{
	sys_para_t.SMART_REMINDER =1;
	play_now_music(SMART_REMINDER_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

bool close_smart_reminder(void)
{
	sys_para_t.SMART_REMINDER = 0;
	sys_para_update_flag = true;
	return false;
}

bool is_smart_reminder_open(void)
{
	if(sys_para_t.SMART_REMINDER == 1)
		return true;
	else
		return false;
}

/*	3. train modeѵ��ģʽ */
bool open_train_mode(void)
{
	sys_para_t.TRAIN_MODE =1;
	play_now_music(TRAIN_MODE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

bool close_train_mode(void)
{
	sys_para_t.TRAIN_MODE = 0;
	sys_para_update_flag = true;
	return false;
}

bool is_train_mode_open(void)
{
	if(sys_para_t.TRAIN_MODE == 1)
		return true;
	else
		return false;
}

/*	4. ����:0x00--0x40	*/
//	volumeȡֵ��Χ��0-5
bool set_system_vol(uint8_t volume)
{
	if(volume > MAX_VOLUME_INDEX)
		volume = MAX_VOLUME_INDEX;
	sys_para_t.VOLUME = volume;
	play_now_music(0, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	return true;
}

uint8_t get_system_vol(void)	/*	������������	*/
{
	return (sys_para_t.VOLUME&0x000000FF);
}

/*	5. ����:0x00--0x40  [0, 15, 30, 40, 52, 64]	*/
//	lightȡֵ��Χ��0-5
bool set_system_light(uint8_t light)
{
	if(light > MAX_LIGHT_INDEX)
		light = MAX_LIGHT_INDEX;
	sys_para_t.LIGHT = light;
	set_now_brightness(system_light[sys_para_t.LIGHT]);
	sys_para_update_flag = true;
	return true;
}
uint8_t get_system_light(void)
{
	return sys_para_t.LIGHT;
}

/*	6. �û�ʹ�ô���	*/
void set_use_times(void)
{
	sys_para_t.USE_TIMES ++;
}
uint32_t read_use_times(void)
{
	return sys_para_t.USE_TIMES;
}

/*	7. �����Զ�����	*/
bool open_autoback(void)
{
	sys_para_t.AUTO_BACK = 1;
	return true;
}
bool close_autoback(void)
{
	sys_para_t.AUTO_BACK = 0x10;
	return true;
}
bool is_autoback_off(void)
{
	if(sys_para_t.AUTO_BACK == 0x10)
		return true;
	else
		return false;
}

/* --------------------------------------------	�����ṩ���ݶ�д�ӿ�	-------------------------------------------------	*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-8-16
* �� �� ��: tmy_database_init
* ����˵��: �����ǵ�Ƭ���ڲ�Flash��ʼ���������Ϊ�ⲿEEPROM�洢
*
* ��    ��: 
*
* �� �� ֵ:
*			-1�� ��ʼ��ʧ��
*			0 �� ��ʼ���ɹ�
*********************************************************************************************************
*/
int8_t tmy_database_init(void)
{
	int8_t read_result = -1;

	read_result = read_sys_para_t_data();			/*	��ȡϵͳ����		*/
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data sys_para_t failed = %d!\r\n", read_result);
	else{
		USRDATA_PRINTF("sys_para_t.VOLUME == %d \r\n",sys_para_t.VOLUME);
		USRDATA_PRINTF("sys_para_t.LIGHT == %d \r\n",sys_para_t.LIGHT);
		USRDATA_PRINTF("sys_para_t.ASPIRATE == %d \r\n",sys_para_t.ASPIRATE);
		USRDATA_PRINTF("sys_para_t.AGING_CYCLE == %d \r\n",sys_para_t.AGING_CYCLE);
		USRDATA_PRINTF("sys_para_t.USE_TIMES == %d \r\n",sys_para_t.USE_TIMES);
	}
	
	read_result = read_handle_injector_data();		/*	�ֱ�ע��������		*/
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data handle_injector_t failed = %d!\r\n", read_result);
	else{
		USRDATA_PRINTF("handle_injector_t.ORIGIN_LOC    = 0x%08x\r\n",handle_injector_t.ORIGIN_LOC);
		USRDATA_PRINTF("handle_injector_t.TOP_LOC       = 0x%08x\r\n",handle_injector_t.TOP_LOC);
		USRDATA_PRINTF("handle_injector_t.DISTANCE      = %d\r\n",(handle_injector_t.DISTANCE>>16));
		USRDATA_PRINTF("hhandle_injector_t.LAST_LOC     = %d\r\n",(handle_injector_t.LAST_LOC>>16));
	}
	
	read_result = read_foot_pedal_t_data();			/*	��̤����		*/;
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data foot_pedal_t failed!\r\n");
	else{
		USRDATA_PRINTF("foot_pedal_t.MAX           = %d\r\n", foot_pedal_t.MAX);
		USRDATA_PRINTF("foot_pedal_t.MIN           = %d\r\n", foot_pedal_t.MIN);
		USRDATA_PRINTF("foot_pedal_t.QUARTER_DELTA = %d\r\n", foot_pedal_t.QUARTER_DELTA);
	}
	
	return 0;
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: read_handle_injector_data
* ����˵��: �����ݿ�� �ֱ�ע����handle_injector��Ϣ
*	handle_injector:
*		ORIGIN_LOC ���λ�ã�
*		TOP_LOC ���λ�ã�
*		DISTANCE ���������Ȧ��
*		LAST_LOC �ϴε��ͣתʱ����λ��
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t read_handle_injector_data(void)
{
	//	return tmy_read_user_data("handle_injector_t", &handle_injector_t, sizeof(handle_injector_t));
	static uint8_t databuf[20] = {0};
	REV_U32_T rev_u32_t;
	
	taskENTER_CRITICAL();
	EEPROM_Read(0x60, databuf,20);
	taskEXIT_CRITICAL();
	
	rev_u32_t.uc_rev_data[0] = databuf[0];
	rev_u32_t.uc_rev_data[1] = databuf[1];
	rev_u32_t.uc_rev_data[2] = databuf[2];
	rev_u32_t.uc_rev_data[3] = databuf[3];
	handle_injector_t.ORIGIN_LOC = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[4];
	rev_u32_t.uc_rev_data[1] = databuf[5];
	rev_u32_t.uc_rev_data[2] = databuf[6];
	rev_u32_t.uc_rev_data[3] = databuf[7];
	handle_injector_t.TOP_LOC = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[8];
	rev_u32_t.uc_rev_data[1] = databuf[9];
	rev_u32_t.uc_rev_data[2] = databuf[10];
	rev_u32_t.uc_rev_data[3] = databuf[11];
	handle_injector_t.DISTANCE = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[12];
	rev_u32_t.uc_rev_data[1] = databuf[13];
	rev_u32_t.uc_rev_data[2] = databuf[14];
	rev_u32_t.uc_rev_data[3] = databuf[15];
	handle_injector_t.LAST_LOC = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[16];
	rev_u32_t.uc_rev_data[1] = databuf[17];
	rev_u32_t.uc_rev_data[2] = databuf[18];
	rev_u32_t.uc_rev_data[3] = databuf[19];
	handle_injector_t.BOTTM2ORIGIN = rev_u32_t.u32_rev_data;
	if(handle_injector_t.BOTTM2ORIGIN > (MAX_BOTTOM_ORIGIN<<16))	
		handle_injector_t.BOTTM2ORIGIN = (handle_injector_t.BOTTM2ORIGIN & 0x0000FFFF) + (200<<16);
	
	return 0;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: write_handle_injector_data
* ����˵��: �����ݿ�д �ֱ�ע����handle_injector��Ϣ
*	handle_injector:
*		ORIGIN_LOC ���λ�ã�
*		TOP_LOC ���λ�ã�
*		DISTANCE ���������Ȧ��
*		LAST_LOC �ϴε��ͣתʱ����λ��
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t write_handle_injector_data(void)
{
//	return tmy_write_user_data("handle_injector_t", &handle_injector_t, sizeof(handle_injector_t));
	static uint8_t databuf[20] = {0};
	REV_U32_T rev_u32_t;
	
	rev_u32_t.u32_rev_data = handle_injector_t.ORIGIN_LOC;
	databuf[0] = rev_u32_t.uc_rev_data[0];
	databuf[1] = rev_u32_t.uc_rev_data[1];
	databuf[2] = rev_u32_t.uc_rev_data[2];
	databuf[3] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.TOP_LOC;
	databuf[4] = rev_u32_t.uc_rev_data[0];
	databuf[5] = rev_u32_t.uc_rev_data[1];
	databuf[6] = rev_u32_t.uc_rev_data[2];
	databuf[7] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.DISTANCE;
	databuf[8] = rev_u32_t.uc_rev_data[0];
	databuf[9] = rev_u32_t.uc_rev_data[1];
	databuf[10] = rev_u32_t.uc_rev_data[2];
	databuf[11] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.LAST_LOC;
	databuf[12] = rev_u32_t.uc_rev_data[0];
	databuf[13] = rev_u32_t.uc_rev_data[1];
	databuf[14] = rev_u32_t.uc_rev_data[2];
	databuf[15] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = handle_injector_t.BOTTM2ORIGIN;
	databuf[16] = rev_u32_t.uc_rev_data[0];
	databuf[17] = rev_u32_t.uc_rev_data[1];
	databuf[18] = rev_u32_t.uc_rev_data[2];
	databuf[19] = rev_u32_t.uc_rev_data[3];
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x60, databuf, 20);
	taskEXIT_CRITICAL();
	
	return 0;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: read_foot_pedal_t_data
* ����˵��: �����ݿ�� ��̤foot_pedal��Ϣ
*	handle_injector:
*		��Ϣʱ��ѹֵMIN��
*		������ײ���ѹֵMAX��
*		�仯��4�ȷ�ֵQUARTER_DELTA
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t read_foot_pedal_t_data(void)
{
//	return tmy_read_user_data("foot_pedal_t", &foot_pedal_t, sizeof(foot_pedal_t));
	static uint8_t databuf[12] = {0};
	REV_U32_T rev_u32_t;
	taskENTER_CRITICAL();
	EEPROM_Read(0x40, databuf,12);
	taskEXIT_CRITICAL();
	
	rev_u32_t.uc_rev_data[0] = databuf[0];
	rev_u32_t.uc_rev_data[1] = databuf[1];
	rev_u32_t.uc_rev_data[2] = databuf[2];
	rev_u32_t.uc_rev_data[3] = databuf[3];
	foot_pedal_t.MIN = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[4];
	rev_u32_t.uc_rev_data[1] = databuf[5];
	rev_u32_t.uc_rev_data[2] = databuf[6];
	rev_u32_t.uc_rev_data[3] = databuf[7];
	foot_pedal_t.MAX = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[8];
	rev_u32_t.uc_rev_data[1] = databuf[9];
	rev_u32_t.uc_rev_data[2] = databuf[10];
	rev_u32_t.uc_rev_data[3] = databuf[11];
	foot_pedal_t.QUARTER_DELTA = rev_u32_t.u32_rev_data;
	
	return 0;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: write_foot_pedal_t_data
* ����˵��: �����ݿ�д ��̤foot_pedal��Ϣ��
*	handle_injector:
*		��Ϣʱ��ѹֵMIN��
*		������ײ���ѹֵMAX��
*		�仯��4�ȷ�ֵQUARTER_DELTA
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t write_foot_pedal_t_data(void)
{
//	return tmy_write_user_data("foot_pedal_t", &foot_pedal_t, sizeof(foot_pedal_t));
	static uint8_t databuf[12] = {0};
	REV_U32_T rev_u32_t;
	
	rev_u32_t.u32_rev_data = foot_pedal_t.MIN;
	databuf[0] = rev_u32_t.uc_rev_data[0];
	databuf[1] = rev_u32_t.uc_rev_data[1];
	databuf[2] = rev_u32_t.uc_rev_data[2];
	databuf[3] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = foot_pedal_t.MAX;
	databuf[4] = rev_u32_t.uc_rev_data[0];
	databuf[5] = rev_u32_t.uc_rev_data[1];
	databuf[6] = rev_u32_t.uc_rev_data[2];
	databuf[7] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = foot_pedal_t.QUARTER_DELTA;
	databuf[8] = rev_u32_t.uc_rev_data[0];
	databuf[9] = rev_u32_t.uc_rev_data[1];
	databuf[10] = rev_u32_t.uc_rev_data[2];
	databuf[11] = rev_u32_t.uc_rev_data[3];
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x40, databuf, 12);
	taskEXIT_CRITICAL();
	
	return 0;
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: read_sys_para_t_data
* ����˵��: �����ݿ�� sys_para_t��Ϣ
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t read_sys_para_t_data(void)
{
//	return tmy_read_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
	static uint8_t databuf[14] = {0};
	REV_U32_T rev_u32_t;
	
	taskENTER_CRITICAL();
	EEPROM_Read(0x20, databuf,14);
	taskEXIT_CRITICAL();
	
	sys_para_t.ASPIRATE = databuf[0];
	sys_para_t.SMART_REMINDER = databuf[1];
	sys_para_t.TRAIN_MODE = databuf[2];
	sys_para_t.VOLUME = databuf[3];
	sys_para_t.LIGHT = databuf[4];
	
	rev_u32_t.uc_rev_data[0] = databuf[5];
	rev_u32_t.uc_rev_data[1] = databuf[6];
	rev_u32_t.uc_rev_data[2] = databuf[7];
	rev_u32_t.uc_rev_data[3] = databuf[8];
	sys_para_t.AGING_CYCLE = rev_u32_t.u32_rev_data;
	
	rev_u32_t.uc_rev_data[0] = databuf[9];
	rev_u32_t.uc_rev_data[1] = databuf[10];
	rev_u32_t.uc_rev_data[2] = databuf[11];
	rev_u32_t.uc_rev_data[3] = databuf[12];
	sys_para_t.USE_TIMES = rev_u32_t.u32_rev_data;
	
	sys_para_t.AUTO_BACK = databuf[13];
	
	return true;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: read_sys_para_t_data
* ����˵��: �����ݿ�д�� sys_para_t ��Ϣ
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t write_sys_para_t_data(void)
{
//	return tmy_write_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
	static uint8_t databuf[14] = {0};
	REV_U32_T rev_u32_t;
	
	databuf[0] = sys_para_t.ASPIRATE;
	databuf[1] = sys_para_t.SMART_REMINDER;
	databuf[2] = sys_para_t.TRAIN_MODE;
	databuf[3] = sys_para_t.VOLUME;
	databuf[4] = sys_para_t.LIGHT;
	
	rev_u32_t.u32_rev_data = sys_para_t.AGING_CYCLE;
	databuf[5] = rev_u32_t.uc_rev_data[0];
	databuf[6] = rev_u32_t.uc_rev_data[1];
	databuf[7] = rev_u32_t.uc_rev_data[2];
	databuf[8] = rev_u32_t.uc_rev_data[3];
	
	rev_u32_t.u32_rev_data = sys_para_t.USE_TIMES;
	databuf[9] = rev_u32_t.uc_rev_data[0];
	databuf[10] = rev_u32_t.uc_rev_data[1];
	databuf[11] = rev_u32_t.uc_rev_data[2];
	databuf[12] = rev_u32_t.uc_rev_data[3];
	
	databuf[13] = sys_para_t.AUTO_BACK;
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x20, databuf, 14);
	taskEXIT_CRITICAL();
	
	sys_para_update_flag = false;
	return 0;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-01-21
* �� �� ��: get_current_handle_injector
* ����˵��: ��ȡ��ǰ˿������λ��
*
* ��    ��: 
*
* �� �� ֵ:
*			��ǰ˿������λ��
*********************************************************************************************************
*/
uint32_t get_current_handle_injector(void)
{
	uint32_t loc = (uint32_t)(motor_cycle << 16) + motor_angle;
	return (loc + handle_injector_t.LAST_LOC);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-07-13
* �� �� ��: get_current_handle_run_distance
* ����˵��: ��ȡ��ǰ˿�������е��г̣���λ��Ȧ��
*
* ��    ��: 
*
* �� �� ֵ:
*			��ǰ˿������Ȧ������λ��Ȧ��
*********************************************************************************************************
*/
uint16_t get_current_handle_run_distance(uint16_t current_pos)
{
	uint16_t ret = 0;
	uint16_t currnt_loc_cycle = current_pos;
	uint16_t bottom_loc_cycle = (handle_injector_t.ORIGIN_LOC >> 16);
	uint16_t distance_cycle = (handle_injector_t.DISTANCE >> 16);
	
	if(currnt_loc_cycle >= bottom_loc_cycle){
		ret = currnt_loc_cycle - bottom_loc_cycle;
		if(ret > distance_cycle)
			return distance_cycle;
		else
			return ret;
	}else{
		ret = 0xFFFF - bottom_loc_cycle + 1 + currnt_loc_cycle;
		if(ret > distance_cycle)
			return 0;
		else
			return ret;
	}	
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-03-15
* �� �� ��: compute_remain_dose
* ����˵��: ����������ʣ��ҩ��
*
* ��    ��: 
*
* �� �� ֵ:
*			��ʾʣ��ҩ���İٷֱȣ�10-->10%, 20-->20%,...,100-->100%
*********************************************************************************************************
*/
uint8_t compute_remain_dose(uint16_t current_pos)
{
	uint16_t all_dis = (handle_injector_t.DISTANCE >> 16);
	uint16_t now_dis = get_current_handle_run_distance(current_pos);
	uint16_t remain_dose = (all_dis - now_dis)*100 / all_dis;
	return remain_dose;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-01-21
* �� �� ��: save_bf_pwroff
* ����˵��: �ػ�ǰ����������Ϣ
*
* ��    ��: 
*
* �� �� ֵ:
*			-1�� ��ʼ��ʧ��
*			0 �� ��ʼ���ɹ�
*********************************************************************************************************
*/
static int8_t save_bf_pwroff(void)
{
	int8_t result = 0;
	handle_injector_t.LAST_LOC = get_current_handle_injector();			/*	У��ʱ���㣬�ػ�ʱ���棬�����Ǹ���*/
	result = write_handle_injector_data();	
	if(!result)
		USRDATA_PRINTF("handle_injector_t.LAST_LOC = %d\r\n",(handle_injector_t.LAST_LOC>>16));
	
	result = write_sys_para_t_data();
	if(!result)
		USRDATA_PRINTF("sys_para_t.AGING_CYCLE = %d\r\n",sys_para_t.AGING_CYCLE);
	
	return result;
}

#include "power_control.h"
#include "get_voltage.h"
typedef enum
{
	POWER_CHARGE_IDLE = 0,
	OFF1_NO_CHG,			/*	1:	�ػ�����Ч��������δ����	*/
	OFF2_LOW_CHG,			/*	2:	�ػ�����Ч�������������ѹ�� */
	OFF3_HIGH_CHG,			/*	3:	�ػ�����Ч�������������ѹ��	*/
	ON1_CHG,				/*	4:	�ػ�����/��Ч����ȷ������������	*/
	ON2_NO_CHG,				/*	5:	�ػ�����Ч��������δ����	*/
	ON3_LOW_CHG,			/*	6:	�ػ�����Ч�������������ѹ��	*/
	ON4_HIGH_CHG			/*	7:	�ػ�����Ч�������������ѹ��	*/
}POWER_CHARGER_T;	/*	���ػ������״̬		*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.2
* ��    �ڣ�2022-08-03
* �� �� ��: power_on_off_detect
* ����˵��: ���ػ����
*
* ��    ��: 
*
* �� �� ֵ:
*			
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sMotor_goOrigin;
void power_on_off_detect(void)
{
	static POWER_CHARGER_T power_charger_t = POWER_CHARGE_IDLE;
	static bool already_pwr_on_flase = false;
	/*	1. ״̬���	*/
	if( isPWR_OFF() ){
		switch( is_charger_adapter_right() ){
			case 1:	power_charger_t = OFF1_NO_CHG; break;			/*	1:	�ػ�����Ч��������δ���룬�ɹػ�	*/
			case 2: power_charger_t = OFF2_LOW_CHG; break;			/*	2:	�ػ�����Ч�������������ѹ�ͣ����ǵ�ǰ�޷��ػ�	 */
			case 3: power_charger_t = OFF3_HIGH_CHG; break;			/*	3:	�ػ�����Ч�������������ѹ�ߣ����ǵ�ǰ�޷��ػ�		*/
			default: power_charger_t = ON1_CHG; break;				/*	4:	�ػ�����Ч����ȷ�����������룬���ǵ�ǰ�޷��ػ�		*/
		}
	}else{
		switch( is_charger_adapter_right() ){
			case 1: power_charger_t = ON2_NO_CHG; break;			/*	5:	�ػ�����Ч��������δ����	*/
			case 2: power_charger_t = ON3_LOW_CHG; break;			/*	6:	�ػ�����Ч�������������ѹ��	*/
			case 3: power_charger_t = ON4_HIGH_CHG; break;			/*	7:	�ػ�����Ч�������������ѹ��	*/
			default: power_charger_t = ON1_CHG; break;				/*	4:	�ػ�����Ч����ȷ������������	*/
		}
	}
	/*	2. ״ִ̬��	*/
	switch(power_charger_t){
		case POWER_CHARGE_IDLE: break;
		
		case OFF1_NO_CHG:	/*	�ػ�����Ч�������ػ�	*/
			set_now_page(PAGE_PowerOff);
			PowerOFF();
		break;
		
		case OFF2_LOW_CHG:
		case OFF3_HIGH_CHG:
		case ON1_CHG:
		case ON2_NO_CHG:
		case ON3_LOW_CHG:
		case ON4_HIGH_CHG:
			if(already_pwr_on_flase == false){
				PWR_ON();
				already_pwr_on_flase = true;
			}else
				;
		break;
	}
	
	/*	3. ��ص�ѹ���	*/
	display_battary_state();
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: get_Calibration_Oil_index
* ����˵��: ��eeprom�м���У���õ�Һѹϵ�� Oil_K, Oil_b
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/

float Oil_K = 71.32f;
float Oil_b	= 3.602f;
uint8_t get_fitting_oil_index(void)
{
	static REV_FLOAT_T Oil_K_data;
	static REV_FLOAT_T Oil_b_data;
	static uint8_t temp[4] = {0};
	float temp_kPa = 0;
	uint8_t ret = 0;
	taskENTER_CRITICAL();
	ret = EEPROM_Read(0x01, temp, 4);		/*	��ȡOil_K	*/
	taskEXIT_CRITICAL();
	Oil_K_data.uc_rev_data[0] = temp[0];
	Oil_K_data.uc_rev_data[1] = temp[1];
	Oil_K_data.uc_rev_data[2] = temp[2];
	Oil_K_data.uc_rev_data[3] = temp[3];
	
	taskENTER_CRITICAL();
	ret = EEPROM_Read(0x05, temp, 4);		/*	��ȡOil_b	*/
	taskEXIT_CRITICAL();
	Oil_b_data.uc_rev_data[0] = temp[0];
	Oil_b_data.uc_rev_data[1] = temp[1];
	Oil_b_data.uc_rev_data[2] = temp[2];
	Oil_b_data.uc_rev_data[3] = temp[3];
	
	/*	��������һ����֤����ֹ���ϵ����ʵ��ƫ��ϴ�	*/
	temp_kPa = Oil_K_data.f_rev_data*1.62f + Oil_b_data.f_rev_data;
	if( (temp_kPa <120) && (temp_kPa >80) ){
		Oil_K = Oil_K_data.f_rev_data;
		Oil_b = Oil_b_data.f_rev_data;
		ret = 1;
	}else{
		Oil_K = 71.32f;
		Oil_b = 3.602f;
		ret = 0;
	}
	USRDATA_PRINTF("System read oil calibration data over, Oil_K= %3.2f, Oil_b= %3.2f!\r\n", Oil_K, Oil_b);
	return ret;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-04-14
* �� �� ��: current_g_from_kPa
* ����˵��: ���ݶ�ȡ��Һѹת��Ϊʵ�ʵ�����
* ��    ��: 
      ����ֵ��λ�� ��g
* �� �� ֵ: 
*********************************************************************************************************
*/
float Power_K = 14.77;
float Power_b = -3596;
int32_t current_g_from_kPa(float kPa)
{
	static float ret = 0.0f;
	ret = Power_K * kPa + Power_b;
	
	return (int32_t)ret;
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-08-11
* �� �� ��: cycle_reset_to_origin
* ����˵��: ��λ�����Һѹԭ��
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void cycle_reset_to_origin(void)
{
	uint16_t distance_cycle = (handle_injector_t.DISTANCE >> 16);
	handle_injector_t.LAST_LOC = 0;
	if((distance_cycle < 1000)||(distance_cycle > 1300))
		handle_injector_t.DISTANCE = (MAX_DIS_CYCLES << 16);
	handle_injector_t.ORIGIN_LOC = (0x0000FFFF&(handle_injector_t.ORIGIN_LOC)) + (CYCLE_BIAS << 16);
	handle_injector_t.TOP_LOC = handle_injector_t.DISTANCE + handle_injector_t.ORIGIN_LOC;
	reset_cycle_to_bias();
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: PowerOFF
* ����˵��: �ػ�
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void PowerOFF(void)
{
	static int8_t i8_result = 0;
	set_motor_stop();
	set_use_times();
	USRDATA_PRINTF("\r\nUser has use %d times\r\n", read_use_times());
	i8_result = save_bf_pwroff();	//	F401ִ�и�ָ���޷������ػ�
	if(!i8_result)	USRDATA_PRINTF("\r\nSave data success!\r\n");
	else			USRDATA_PRINTF("\r\nSave data failed!\r\n");

	osDelay(50);
	PWR_OFF();
}
