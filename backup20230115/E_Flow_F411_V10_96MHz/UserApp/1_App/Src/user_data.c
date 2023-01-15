#include "user_data.h"
#include "lcd_display.h"
#include "motor_misc.h"
#include "eeprom.h"
#include "power_control.h"
#include "get_voltage.h"

/*	FreeRTOS	*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"


//	ϵͳ����: ��д�����ݿ�
SYS_PARA_T sys_para_t = {0};
uint32_t IDLE_TIME_CNT = 0;			//	����ϵͳ���м��������������5�����޲�����ϵͳ�Զ��ػ�
CHARGE_STATE_E CHARGE_STATE = RIGHT_CHARGE;

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
bool handle_in_top = false;				/*	˿�˴��ڶ���	*/
bool handle_in_bottom = false;			/*	˿�˴��ڵ׶�	*/
bool handle_in_origin = false;			/*	˿�˴���ԭ��	*/
bool sys_para_update_flag = false;		/*	����������־	*/
bool sys_bat_too_low = false;			/*	��ص�������	*/

//	����汾
const uint8_t EB_VERSION[] = "1.1.1.001";

//	ϵͳ�еĵ��ת�٣���ǣ��У�������⣬���Բ���д�����ݿ�
/* 	LOW,	MID,	HIGH	*/
const uint32_t injector_speed_t[4][3] = {
	{31.5*F_DIV,	31.5*F_DIV,		181*F_DIV},				/*	Normal	�ٶ�	[LOW, LOW, MID]	*/
	{181*F_DIV,		181*F_DIV,		380*F_DIV},				/*	Turbo 	�ٶ�	[MID, MID, HIGH]*/
	{31.5*F_DIV,	31.5*F_DIV,		31.5*F_DIV},			/*	STA   	�ٶ�	[LOW, LOW, LOW]	*/
	{200*F_DIV,		500*F_DIV,		700*F_DIV},				/*	ASP		����1		����2	*/
	};

//	���������Ⱥ������б�
const uint8_t system_light[MAX_LIGHT_INDEX+1] = {10, 20, 30, 40, 52, 63};
const uint8_t system_volume[MAX_VOLUME_INDEX+1]= {0, 30, 40, 48, 56, 63};

/**************************���¹������ݿ�����************************/
/*	1. Aspirate��������	*/
bool Open_Asp(void)
{
	sys_para_t.ASPIRATE = 1;
	if(Is_Train_Mode_Open() == true)
		Play_Now_Music(ASPIRATE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Mode].obj_value[0] = sys_para_t.ASPIRATE;
	return true;
}

bool Close_Asp(void)
{
	sys_para_t.ASPIRATE = 0;
	if(Is_Train_Mode_Open() == true)
		Play_Now_Music(ASPIRATE_OFF_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Mode].obj_value[0] = sys_para_t.ASPIRATE;
	return false;
}
bool Is_Asp_Open(void)
{
	vgus_page_objs[PAGE_Mode].obj_value[0] = sys_para_t.ASPIRATE;
	if(sys_para_t.ASPIRATE == 1)
		return true;
	else
		return false;
}

/*	2. smart_reminder��������	*/
bool Open_Smart_Reminder(void)
{
	sys_para_t.SMART_REMINDER =1;
	Play_Now_Music(SMART_REMINDER_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[0] = sys_para_t.SMART_REMINDER;
	return true;
}

bool Close_Smart_Reminder(void)
{
	sys_para_t.SMART_REMINDER = 0;
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[0] = sys_para_t.SMART_REMINDER;
	return false;
}

bool Is_Smart_Reminder_Open(void)
{
	vgus_page_objs[PAGE_Setting].obj_value[0] = sys_para_t.SMART_REMINDER;
	if(sys_para_t.SMART_REMINDER == 1)
		return true;
	else
		return false;
}

/*	3. train modeѵ��ģʽ */
bool Open_Train_Mode(void)
{
	sys_para_t.TRAIN_MODE =1;
	Play_Now_Music(TRAIN_MODE_ON_INDEX, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[1] = sys_para_t.TRAIN_MODE;
	return true;
}

bool Close_Train_Mode(void)
{
	sys_para_t.TRAIN_MODE = 0;
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[1] = sys_para_t.TRAIN_MODE;
	return false;
}

bool Is_Train_Mode_Open(void)
{
	vgus_page_objs[PAGE_Setting].obj_value[1] = sys_para_t.TRAIN_MODE;
	if(sys_para_t.TRAIN_MODE == 1)
		return true;
	else
		return false;
}

/*	4. ����:0x00--0x40	*/
//	volumeȡֵ��Χ��0-5
bool Set_System_Vol(uint8_t volume)
{
	if(volume > MAX_VOLUME_INDEX)
		volume = MAX_VOLUME_INDEX;
	sys_para_t.VOLUME = volume;
	Play_Now_Music(0, system_volume[sys_para_t.VOLUME]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[3] = sys_para_t.VOLUME;		//	������������
	return true;
}

uint8_t Get_System_Vol(void)	/*	������������	*/
{
	vgus_page_objs[PAGE_Setting].obj_value[3] = sys_para_t.VOLUME;		//	������������
	return (sys_para_t.VOLUME&0x000000FF);
}

/*	5. ����:0x00--0x40  [0, 15, 30, 40, 52, 64]	*/
//	lightȡֵ��Χ��0-5
bool Set_System_Light(uint8_t light)
{
	if(light > MAX_LIGHT_INDEX)
		light = MAX_LIGHT_INDEX;
	sys_para_t.LIGHT = light;
	Set_VGUS_Brightness(system_light[sys_para_t.LIGHT]);
	sys_para_update_flag = true;
	vgus_page_objs[PAGE_Setting].obj_value[2] = sys_para_t.LIGHT;		//	������������
	return true;
}
uint8_t Get_System_Light(void)
{
	if(sys_para_t.LIGHT > MAX_LIGHT_INDEX)
		sys_para_t.LIGHT = MAX_LIGHT_INDEX;
	vgus_page_objs[PAGE_Setting].obj_value[2] = sys_para_t.LIGHT;		//	������������
	return sys_para_t.LIGHT;
}

/*	6. �û�ʹ�ô���	*/
void Set_Use_Times(void)
{
	sys_para_t.USE_TIMES ++;
}
uint32_t Read_Use_Times(void)
{
	return sys_para_t.USE_TIMES;
}

/*	7. �����Զ�����	*/
bool Open_Autoback(void)
{
	sys_para_t.AUTO_BACK = 1;
	return true;
}
bool Close_AutoBack(void)
{
	sys_para_t.AUTO_BACK = 0x10;
	return true;
}
bool Is_Autoback_Off(void)
{
	if(sys_para_t.AUTO_BACK == 0x10)
		return true;
	else
		return false;
}

/*	8. ���ʱʹ��	*/
bool Open_UseInCharge(void)
{
	sys_para_t.USE_In_CHARGING = 0x10;
	return true;
}
bool Close_UseInCharge(void)
{
	sys_para_t.USE_In_CHARGING = 1;
	return true;
}
bool Is_UseInCharge(void)
{
	if(sys_para_t.USE_In_CHARGING == 0x10)
		return true;
	else
		return false;
}

/*	9. ϵͳ����	*/
bool Start_SysUpdate(void)
{
	uint8_t databuf = 1;
	taskENTER_CRITICAL();
	EEPROM_Write(0x2F, &databuf, 1);
	taskEXIT_CRITICAL();
}

bool Stop_SysUpdate(void)
{
	uint8_t databuf = 0;
	taskENTER_CRITICAL();
	EEPROM_Write(0x2F, &databuf, 1);
	taskEXIT_CRITICAL();
}

/* --------------------------------------------	�����ṩ���ݶ�д�ӿ�	-------------------------------------------------	*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-8-16
* �� �� ��: TMY_DB_Init
* ����˵��: �����ǵ�Ƭ���ڲ�Flash��ʼ���������Ϊ�ⲿEEPROM�洢
*
* ��    ��: 
*
* �� �� ֵ:
*			-1�� ��ʼ��ʧ��
*			0 �� ��ʼ���ɹ�
*********************************************************************************************************
*/
int8_t TMY_DB_Init(void)
{
	int8_t read_result = -1;

	read_result = Read_Sys_Para_Data();			/*	��ȡϵͳ����		*/
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data sys_para_t failed = %d!\r\n", read_result);
	else{
//		USRDATA_PRINTF("ASPIRATE= %d\r\n SMART_REMINDER= %d\r\n TRAIN_MODE= %d\r\n VOLUME= %d\r\n LIGHT= %d\r\n AGING_CYCLE= %d\r\n USE_TIMES=%d\r\n AUTO_BACK= %d\r\n",
//		sys_para_t.ASPIRATE, sys_para_t.SMART_REMINDER, sys_para_t.TRAIN_MODE, sys_para_t.VOLUME, sys_para_t.LIGHT, sys_para_t.AGING_CYCLE, sys_para_t.USE_TIMES, sys_para_t.AUTO_BACK);
	}
	
	read_result = Read_Handle_Injector_Data();		/*	�ֱ�ע��������		*/
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data handle_injector_t failed = %d!\r\n", read_result);
	else{
//		USRDATA_PRINTF("ORIGIN_LOC= 0x%08x\r\n TOP_LOC= 0x%08x\r\n DISTANCE= %d\r\n LAST_LOC= %d\r\n",
//		handle_injector_t.ORIGIN_LOC, handle_injector_t.TOP_LOC, (handle_injector_t.DISTANCE>>16), (handle_injector_t.LAST_LOC>>16));
	}
	
	read_result = Read_Foot_Pedal_Data();			/*	��̤����		*/;
	if(read_result)
		USRDATA_PRINTF("tmy_read_user_data foot_pedal_t failed!\r\n");
	else{
//		USRDATA_PRINTF("foot_pedal_t.MAX= %d\r\n MIN= %d\r\n QUARTER_DELTA= %d\r\n", foot_pedal_t.MAX,foot_pedal_t.MIN,foot_pedal_t.QUARTER_DELTA);
	}
	
	return 0;
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: Read_Handle_Injector_Data
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
int8_t Read_Handle_Injector_Data(void)
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
* �� �� ��: Write_Handle_Injector_Data
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
int8_t Write_Handle_Injector_Data(void)
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
* �� �� ��: Read_Foot_Pedal_Data
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
int8_t Read_Foot_Pedal_Data(void)
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
* �� �� ��: Write_Foot_Pedal_Data
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
int8_t Write_Foot_Pedal_Data(void)
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
* �� �� ��: Read_Sys_Para_Data
* ����˵��: �����ݿ�� sys_para_t��Ϣ
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t Read_Sys_Para_Data(void)
{
//	return tmy_read_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
	static uint8_t databuf[16] = {0};
	REV_U32_T rev_u32_t;
	
	taskENTER_CRITICAL();
	EEPROM_Read(0x20, databuf,16);
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
	sys_para_t.USE_In_CHARGING = databuf[14];
	sys_para_t.UPDATE_FLAG = databuf[15];
	
	return true;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.1
* ��    �ڣ�2022-08-16
* �� �� ��: Read_Sys_Para_Data
* ����˵��: �����ݿ�д�� sys_para_t ��Ϣ
* �� �� ֵ:
*			-1�� ��������ʧ��
*			0 �� �������ݳɹ�
*********************************************************************************************************
*/
int8_t Write_Sys_Para_Data(void)
{
//	return tmy_write_user_data("sys_para_t", &sys_para_t, sizeof(sys_para_t));
	static uint8_t databuf[16] = {0};
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
	databuf[14] = sys_para_t.USE_In_CHARGING;
	databuf[15] = sys_para_t.UPDATE_FLAG;
	
	taskENTER_CRITICAL();
	EEPROM_Write(0x20, databuf, 16);
	taskEXIT_CRITICAL();
	
	sys_para_update_flag = false;
	return 0;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-01-21
* �� �� ��: Get_Current_Handle_Injector_Pos
* ����˵��: ��ȡ��ǰ˿������λ��
*
* ��    ��: 
*
* �� �� ֵ:
*			��ǰ˿������λ��
*********************************************************************************************************
*/
uint32_t Get_Current_Handle_Injector_Pos(void)
{
	uint32_t loc = (uint32_t)(motor_cycle << 16) + motor_angle;
	return (loc + handle_injector_t.LAST_LOC);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-07-13
* �� �� ��: Get_Current_Handle_Run_Distance
* ����˵��: ��ȡ��ǰ˿�������е��г̣���λ��Ȧ��
*
* ��    ��: 
*
* �� �� ֵ:
*			��ǰ˿������Ȧ������λ��Ȧ��
*********************************************************************************************************
*/
uint16_t Get_Current_Handle_Run_Distance(uint16_t current_pos)
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
* �� �� ��: Compute_Reamin_Dose
* ����˵��: ����������ʣ��ҩ��
*
* ��    ��: 
*
* �� �� ֵ:
*			��ʾʣ��ҩ���İٷֱȣ�10-->10%, 20-->20%,...,100-->100%
*********************************************************************************************************
*/
uint8_t Compute_Reamin_Dose(uint16_t current_pos)
{
	uint16_t all_dis = (handle_injector_t.DISTANCE >> 16);
	uint16_t now_dis = Get_Current_Handle_Run_Distance(current_pos);
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
	handle_injector_t.LAST_LOC = Get_Current_Handle_Injector_Pos();			/*	У��ʱ���㣬�ػ�ʱ���棬�����Ǹ���*/
	result = Write_Handle_Injector_Data();	
	if(!result)
		USRDATA_PRINTF("handle_injector_t.LAST_LOC = %d\r\n",(handle_injector_t.LAST_LOC>>16));
	
	result = Write_Sys_Para_Data();
	if(!result)
		USRDATA_PRINTF("sys_para_t.AGING_CYCLE = %d\r\n",sys_para_t.AGING_CYCLE);
	
	return result;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.2
* ��    �ڣ�2022-08-03
* �� �� ��: PWR_On_Off_Detect
* ����˵��: ���ػ����
*
* ��    ��: 
*
* �� �� ֵ:
*			
*********************************************************************************************************
*/
void PWR_On_Off_Detect2(void)
{
	static bool already_pwr_on_flag = false;
	static bool disable_vgus_touch_false = false;
	static uint8_t time_cnt = 0;
	
	
	//	0,���ʵ����������룻1��������δ��������������ѻ���2����������ѹƫ�ͣ�3����������ѹƫ��
	CHARGE_STATE = Is_Charger_Adapter_Right();		
	/*	1. �ֶ��ϵ�	*/
	if(already_pwr_on_flag == false){				//	A. �ػ�״̬-->�ϵ翪��״̬
		PWR_ON();
		already_pwr_on_flag = true;
		osDelay(2000);
		if(isPWR_KEY_INVALID() == 1){	//	�ȴ��û�����
			while(1){					
				osDelay(200);
				if(isPWR_KEY_INVALID()==0)
					break;
			}
		}
	}else{											//	B. ����״̬��
		if((isPWR_KEY_INVALID() == 1)&&(CHARGE_STATE != RIGHT_CHARGE)){
			osDelay(2000);
			if(isPWR_KEY_INVALID() == 1){	//	2s֮�󿪹ػ�������Ȼ�����£����ʹ�ܹػ�
				PWR_Off_Save();
				Clear_Lcd_Cmd_SendQueue();
				Set_VGUS_Page(PAGE_PowerOff, true);
				osDelay(3000);
				Close_VGUS_BACKLIGHT();
				while(1){				//	�ȴ��û�����
					if(isPWR_KEY_INVALID()==0)
						PWR_OFF();
					osDelay(300);
				}
			}	
		}
	}
	
	if(++time_cnt > 35){	//	�տ�ʼ�ϵ�ʱ�����г��͵�ؼ��
		/*	2. ���ʹ�ù����Ƿ���	*/
		if(CHARGE_STATE != NO_CHARGE){
			if(Is_UseInCharge() == false){
				Disable_VGUS_Touch();
				disable_vgus_touch_false = true;
			}
		}else if(disable_vgus_touch_false == true){
			Clear_Lcd_Cmd_SendQueue();
			Enable_VGUS_Touch();
			disable_vgus_touch_false = false;
		}
			
		/*	3. ��ص�ѹ���	*/
		Display_Battery();
		
		time_cnt = 35;
	}

	/*	4. ϵͳ���м��	*/
	IDLE_TIME_CNT ++;
	if(IDLE_TIME_CNT > MAX_IDLE_TIME_CNT){
		PWR_Off_Save();
		Clear_Lcd_Cmd_SendQueue();
		Set_VGUS_Page(PAGE_PowerOff, true);
		osDelay(3000);
		Close_VGUS_BACKLIGHT();
		osDelay(300);
		PWR_OFF();
	}
		
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
uint8_t Get_Fitting_Oil_Index(void)
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
* �� �� ��: Get_Current_g_From_kPa
* ����˵��: ���ݶ�ȡ��Һѹת��Ϊʵ�ʵ�����
* ��    ��: 
      ����ֵ��λ�� ��g
* �� �� ֵ: 
*********************************************************************************************************
*/
float Power_K = 14.77;
float Power_b = -3596;
int32_t Get_Current_g_From_kPa(float kPa)
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
* �� �� ��: Cycle_Reset_To_Origin
* ����˵��: ��λ�����Һѹԭ��
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void Cycle_Reset_To_Origin(void)
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
* �� �� ��: PWR_Off_Save
* ����˵��: �ػ�
* ��    ��: 
* �� �� ֵ: 
*********************************************************************************************************
*/
void PWR_Off_Save(void)
{
	static int8_t i8_result = 0;
	set_motor_stop();
	Set_Use_Times();
	USRDATA_PRINTF("\r\nUser has use %d times\r\n", Read_Use_Times());
	i8_result = save_bf_pwroff();	//	F401ִ�и�ָ���޷������ػ�
	if(!i8_result)	USRDATA_PRINTF("\r\nSave data success!\r\n");
	else			USRDATA_PRINTF("\r\nSave data failed!\r\n");
}
