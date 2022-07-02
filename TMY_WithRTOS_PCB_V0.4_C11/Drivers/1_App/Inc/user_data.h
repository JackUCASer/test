#ifndef	__USER_DATA_H 
#define	__USER_DATA_H
#include <stdint.h>
#include <flashdb.h>		/*	���ݿ�����	*/

/*	----------------------------------------�û����ݽṹ-----------------------------------------------	*/
//	0. ϵͳ������ز���
typedef struct
{
	uint32_t ASPIRATE;	/*	����aspirate: 0/1	*/
	
	uint32_t SMART_REMINDER;
	uint32_t TRAIN_MODE;
	uint32_t VOLUME;	/*	����volume: 0-100	*/
	uint32_t LIGHT;	/*	����light: 0-100	*/
	
	uint32_t AGING_CYCLE;	/*	�ϻ�aging cycle: �ϻ�Ȧ��	*/
	uint32_t USE_TIMES;	/*	ʹ�ô�����the times of use	*/	
}SYS_PARA_T;

//	1. ��̤��ز�����У�������
typedef struct 
{
	uint32_t MIN;			/*	У������Сֵ	*/
	uint32_t MAX;			/*	У�������ֵ	*/
	uint32_t QUARTER_DELTA;	/*	У����(MAX - MIN)/4 */
}FOOT_PEDAL_T;

//	2. �ֱ�-ע������صĲ�����BOTTOM_LOC��TOP_LOC �� DISTANCE У������£�LAST_LOC�ػ�ʱ���档
typedef struct
{
	uint32_t BOTTOM_LOC;			/*	�ֱ��ײ�λ�ã���У�������	*/
	uint32_t TOP_LOC;				/*	�ֱ�����λ�ã���У�������*/
	uint32_t DISTANCE;				/*	�ֱ�-ע���� ���г̣���У�������	*/
	uint32_t LAST_LOC;				/*	�ϴε��ͣתʱ����λ�ã��ػ�ʱ����	*/
}HANDLE_INJECTOR_T;

//	3. ��stm32��floatռ4���ֽ�
typedef union{
	float f_rev_data;
	uint8_t uc_rev_data[4];
}REV_FLOAT_T;

/*	---------------------------------------ȫ�ֱ�����-----------------------------------------------	*/
#define F_DIV	2								/*	����ж�ˢ�·�Ƶ����: 1,100us�ж�һ�Σ�2�� 200us�ж�һ��	*/
#define N_TIME_CHECK_ORING		2				/*	�����û�ʹ��N�μ��һ��O��Ȧ	*/
#define MAX_LIGHT_INDEX			5				/*	�����������	*/
#define MAX_VOLUME_INDEX		5				/*	�����������	*/
/*	------------------------��������------------------------	*/
#define KEY_INDEX			0
#define PDL_INDEX			1
#define PRESS_INDEX			2
#define ASP_INDEX			3
#define TOP_BOTTOM_INDEX	4
#define LOW_INDEX			5
#define MID_INDEX			6
#define HIGH_INDEX			7


extern SYS_PARA_T sys_para_t;					/*	ϵͳ����	*/
extern FOOT_PEDAL_T	foot_pedal_t;				/*	��̤����	*/
extern HANDLE_INJECTOR_T handle_injector_t;		/*	ע��������	*/
extern const uint32_t injector_speed_t[4][3];	/*	�����ٶ��б�	*/
extern const uint8_t system_light[6];			/*	�����б�	*/
extern const uint8_t system_volume[6];			/*	�����б�	*/
extern float Oil_K;
extern float Oil_b;
extern bool handle_in_top;
extern bool handle_in_bottom;


bool open_asp(void);
bool close_asp(void);
bool is_asp_open(void);

bool open_smart_reminder(void);
bool close_smart_reminder(void);
bool is_smart_reminder_open(void);

bool open_train_mode(void);
bool close_train_mode(void);
bool is_train_mode_open(void);

bool set_system_vol(uint8_t volume);
uint8_t get_system_vol(void);

bool set_system_light(uint8_t light);
uint8_t get_system_light(void);

void set_use_times(void);
uint32_t read_use_times(void);				/*	��ȡ�û�ʹ�ô���	*/



uint8_t compute_remain_dose(uint16_t current_pos);
/*	--------------------------------------���ݿ�����ӿ�-----------------------------------------------	*/
int8_t read_handle_injector_data(void);		/*	��ȡ�ֱ�ע������ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t write_handle_injector_data(void);	/*	д���ֱ�ע������ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t read_foot_pedal_t_data(void);		/*	��ȡ��̤��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t write_foot_pedal_t_data(void);		/*	д���̤��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t read_sys_para_t_data(void);			/*	��ȡϵͳ��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/
int8_t write_sys_para_t_data(void);			/*	д��ϵͳ��ز����� -1 ʧ�ܣ� 0 �ɹ�	*/

uint32_t get_current_handle_injector(void);	/*	��ȡ��ǰ˿��λ��	*/
void power_on_off_detect(void);				/*	���ػ��������	*/
uint8_t get_fitting_oil_index(void);		/*	�ϵ��ȡҺѹ���������ϵ��	*/
int32_t current_g_from_kPa(float kPa);		/*	ͨ��Һѹ��������	*/

#endif
