#ifndef	__USER_DATA_H 
#define	__USER_DATA_H
#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"		/*	ARM��ѧ��������	*/

#include "GL_Config.h"			/*	����myPrintf	*/
#define __DEBUG_USRDATA

#ifdef __DEBUG_USRDATA
#define USRDATA_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define USRDATA_PRINTF(format, ...)
#endif

/*	----------------------------------------�û����ݽṹ-----------------------------------------------	*/
//	0. ϵͳ������ز���
typedef struct
{
	uint32_t ASPIRATE;								/*	����aspirate: 0/1	*/
	
	uint32_t SMART_REMINDER;
	uint32_t TRAIN_MODE;
	uint32_t VOLUME;								/*	����volume: 0-100	*/
	uint32_t LIGHT;									/*	����light: 0-100	*/
	
	uint32_t AGING_CYCLE;							/*	�ϻ�aging cycle: �ϻ�Ȧ��	*/
	uint32_t USE_TIMES;								/*	ʹ�ô�����the times of use	*/
	uint32_t AUTO_BACK;								/*	�����Զ�����	*/
	uint32_t USE_In_CHARGING;						/*	���״̬��ʹ�ñ�־λ	*/
}SYS_PARA_T;

//	1. ��̤��ز�����У�������
typedef struct 
{
	uint32_t MIN;									/*	У������Сֵ	*/
	uint32_t MAX;									/*	У�������ֵ	*/
	uint32_t QUARTER_DELTA;							/*	У����(MAX - MIN)/4 */
}FOOT_PEDAL_T;

//	2. �ֱ�-ע������صĲ�����ORIGIN_LOC��TOP_LOC �� DISTANCE У������£�LAST_LOC�ػ�ʱ���档
typedef struct
{
	uint32_t ORIGIN_LOC;							/*	�ֱ��ײ�λ�ã���У�������	*/
	uint32_t TOP_LOC;								/*	�ֱ�����λ�ã���У�������*/
	uint32_t DISTANCE;								/*	�ֱ�-ע���� ���г̣���У�������	*/
	uint32_t LAST_LOC;								/*	�ϴε��ͣתʱ����λ�ã��ػ�ʱ����	*/
	uint32_t BOTTM2ORIGIN;
}HANDLE_INJECTOR_T;

//	3. ��stm32��floatռ4���ֽ�
typedef union{
	float f_rev_data;
	uint8_t uc_rev_data[4];
}REV_FLOAT_T;
typedef union{
	uint32_t u32_rev_data;
	uint8_t uc_rev_data[4];
}REV_U32_T;

/*	------------------------------------------ȫ�ֺ궨����---------------------------------------------------	*/			
#define F_DIV					2					/*	����ж�ˢ�·�Ƶ����: 1,100us�ж�һ�Σ�2�� 200us�ж�һ��	*/
#define N_TIME_CHECK_ORING		2					/*	�����û�ʹ��N�μ��һ��O��Ȧ	*/

/*	���Ⱥ���������	*/
#define MAX_LIGHT_INDEX			5					/*	�����������	*/
#define MAX_VOLUME_INDEX		5					/*	�����������	*/
/*	------------------------��������------------------------	*/
/*	ͨ��	*/
#define KEY_INDEX					0		/*	������ok	*/
#define ASPIRATE_BACK_INDEX			2		/*	����ʱ����������ok	*/
#define CHANGE_INDEX				3		/*	��ҩƿʱ�����֣���ҩƿ�����ˣ�ok	*/
#define REMAIN_INDEX				7		/*	ʣ��ҩ������	ok*/
#define SMART_REMINDER_ON_INDEX		19		/*	"smart reminder on �������ѿ�"ok	*/
#define TRAIN_MODE_ON_INDEX			20		/*	"train mode on ѵ��ģʽ��"ok	*/
/*	����	*/
#define LOW_INJECTION_INDEX			4		/*	����ע��ʱ������ok	*/
#define MIDDLE_INJECTION_INDEX		5		/*	����ע��ʱ������ok	*/
#define HIGH_INJECTION_INDEX		6		/*	����ע��ʱ������ok	*/
#define ALARM_INDEX					29		/*	ע���ѹ����	ok*/
#define AUTO_INJECTION_INDEX		30		/*	PDLģʽ�Զ�ע����ʾ��	*/
/*	������Smart Reminder����	*/
#define PDL_INDEX					1		/*	"PDL"���򲥱�ok	*/
/*	Smart Reminder��Train Mode����	*/
#define ASPIRATE_ON_INDEX			8		/*	"aspirate on  ������"ok	*/
#define ASPIRATE_OFF_INDEX			9		/*	"aspirate off ������"ok	*/
#define NORMAL_MODE_INDEX			10		/*	"normal mode  ����ģʽ"ok	*/
#define FAST_MODE_INDEX				11		/*	"fast mode	  ����ģʽ"ok	*/
#define PDL_MODE_INDEX				12		/*	"pdl modepdl  ģʽ"ok	*/
#define OVER_PRESSURE_INDEX			18		/*	"over pressure ע��������֯��ѹ"ok	*/
#define CHECK_O_RING_INDEX			21		/*	"check o ring ���O��Ȧ"	ok*/
#define BATTERY_LOW_INDEX			22		/*	"battery low ������"	*/
/*	Smart Reminderר��	*/
#define SPEED_WELL_INDEX			13		/*	"speed well �ٶ�����"	*/
#define SPEED_QUICK_INDEX			14		/*	"speed quick �ٶȿ�"	*/
#define SPEED_HIGH_INDEX			15		/*	"speed high �ٶȸ�"	*/
#define REMAIN_LOW_INDEX			16		/*	"remain low ʣ��ҩ����"ok	*/
#define CARTRIDGE_EMPTY_INDEX		17		/*	"cartridge empty ҩƿ��"ok	*/
/*	Train Modeר��	*/
#define LOW_SPEED_INDEX				23		/*	"low speed ����"ok	*/
#define MIDDLE_SPEED_INDEX			24		/*	"middle speed ����"ok	*/
#define HIGH_SPEED_INDEX			25		/*	"high speed ����"ok	*/
#define CARTRIDGE_REMAIN_LOW_INDEX	26		/*	"cartridge remian low ʣ��ҩ����"ok	*/
#define CHANGE_CARTRIDGE_INDEX		27		/*	"please change the anesthesic cartridge �����ҩƿ"ok	*/
#define REACH_PDL_INDEX				28		/*	"reach pdl ����PDL����"ok	*/


/*	------------------------------------------ȫ�ֱ���������-------------------------------------------------	*/
extern const uint8_t EB_VERSION[];
extern SYS_PARA_T sys_para_t;						/*	ϵͳ����	*/
extern FOOT_PEDAL_T	foot_pedal_t;					/*	��̤����	*/
extern HANDLE_INJECTOR_T handle_injector_t;			/*	ע��������	*/

extern const uint32_t injector_speed_t[4][3];		/*	�����ٶ��б�	*/
extern const uint8_t system_light[6];				/*	�����б�	*/
extern const uint8_t system_volume[6];				/*	�����б�	*/

extern float Oil_K;									/*	Һѹϵ�� k	*/
extern float Oil_b;									/*	Һѹϵ�� b	*/

extern float32_t dynamic_pressure_mV;				/*	AD��ȡҺѹ�������ĵ�ѹֵ����λ��mV	*/
extern float32_t dynamic_pressure_kPa;				/*	dynamic_pressure_mVת����Һѹֵ����λkPa, �����ȸ�����	*/
extern uint32_t  dynamic_pressure;					/*	dynamic_pressure_mVת����Һѹֵ����λkPa, �޷�������	*/
extern uint16_t  dynamic_pressure_kPa_F;			/*	Hampel�˲����Һѹ����λ��kPa, �޷�������		*/

extern bool handle_in_top;							/*	˿�˴��ڶ��˱�־λ	*/
extern bool handle_in_bottom;						/*	˿�˴��ڵ׶˱�־λ	*/
extern bool handle_in_origin;						/*	˿�˴���Һѹԭ��λ��	*/
extern bool sys_para_update_flag;					/*	ϵͳ�������±���	*/
extern bool sys_bat_too_low;						/*	��ص������ͣ�ϵͳӦ��ȡ������ʩ	*/

/*	------------------------------------------HMI�ӿ�-------------------------------------------------	*/
bool Open_Asp(void);								
bool Close_Asp(void);
bool Is_Asp_Open(void);								/*	��������		*/

bool Open_Smart_Reminder(void);
bool Close_Smart_Reminder(void);
bool Is_Smart_Reminder_Open(void);					/*	�������ѹ���	*/

bool Open_Train_Mode(void);
bool Close_Train_Mode(void);
bool Is_Train_Mode_Open(void);						/*	ѵ��ģʽ		*/

bool Set_System_Vol(uint8_t volume);
uint8_t Get_System_Vol(void);						/*	ϵͳ����		*/

bool Set_System_Light(uint8_t light);
uint8_t Get_System_Light(void);						/*	ϵͳ����		*/

void Set_Use_Times(void);
uint32_t Read_Use_Times(void);						/*	��ȡ�û�ʹ�ô���	*/

bool Open_Autoback(void);							/*	�����Զ�����	*/
bool Close_AutoBack(void);
bool Is_Autoback_Off(void);

bool Open_UseInCharge(void);
bool Close_UseInCharge(void);
bool Is_UseInCharge(void);


/*	--------------------------------------���ݿ�����ӿ�-----------------------------------------------	*/
int8_t TMY_DB_Init(void);											/*	���ݿ��ʼ������	*/
int8_t Read_Handle_Injector_Data(void);								/*	��ȡ�ֱ�ע������ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t Write_Handle_Injector_Data(void);							/*	д���ֱ�ע������ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t Read_Foot_Pedal_Data(void);									/*	��ȡ��̤��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t Write_Foot_Pedal_Data(void);									/*	д���̤��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t Read_Sys_Para_Data(void);									/*	��ȡϵͳ��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t Write_Sys_Para_Data(void);									/*	д��ϵͳ��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/


/*	----------------------------------------�����ӿ�---------------------------------------------------	*/
void PWR_On_Off_Detect(void);										/*	���ػ��������	*/
void PWR_On_Off_Detect2(void);
uint8_t Get_Fitting_Oil_Index(void);								/*	�ϵ��ȡҺѹ���������ϵ��	*/

uint32_t Get_Current_Handle_Injector_Pos(void);						/*	��ȡ��ǰ˿��λ��	*/
uint16_t Get_Current_Handle_Run_Distance(uint16_t current_pos);		/*	��ȡ��ǰ˿�����ߵ�Ȧ��	*/
uint8_t Compute_Reamin_Dose(uint16_t current_pos);					/*	��ȡʣ��ҩҺ����	*/
int32_t Get_Current_g_From_kPa(float kPa);							/*	ͨ��Һѹ��������	*/

void Cycle_Reset_To_Origin(void);									/*	��λ�����Һѹԭ��	*/
void PWR_Off_Save(void);

#endif
