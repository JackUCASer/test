#ifndef __MENU_H
#define __MENU_H
#include "stm32f4xx_hal.h"

//#include "datatype.h"
#define TOP_MENU_INDEX 0

extern uint8_t	sel_page_flash;																	//selҳ��ˢ��
extern uint8_t	sel_eeprom_flash;																//��selҳ����ص�EEPROMˢ��

extern uint8_t	set_page_flash;																	//setҳ��ˢ��
extern uint8_t	set_eeprom_flash;																//��setҳ����ص�EEPROMˢ��

extern uint8_t	cal_page_flash;																	//calҳ��ˢ��
extern uint8_t	cal_eeprom_flash;																//��calҳ����ص�EEPROMˢ��

extern uint8_t 	NeedFlash ;

typedef enum
{
    Input_Password_flag = 1,							// ������������
    Error_Password_flag,								// �������
	AutoThetaBias_flag,									// ����Ƕ�У��
	ThetaBiasFish_flag,									// ����Ƕ�У�����
    Cal_Noload_flag,									// ���ص���
	Cal_Noload_Run_flag,								// ���ص������Խ�����
    Cal_Torque_flag,									// Ť�ز���
	Cal_Torque_Run_flag,								// Ť�ز��Խ�����
	Cal_Torque_Run1_flag,								// Ť�ز��Խ�����
	Cal_Saving,											// ���ݱ�����
	Cal_Fishing,										// Ť�ز��Խ�����
} CAL_MODE_e;                 							// �������ñ�־λ
extern CAL_MODE_e Cal_mode_flag; 

typedef enum
{
    OperationMode_flag = 1,								// ����ģʽѡ������
    Speed_flag,											// �ٶ�����
    Torque_flag,										// ת������
    CWAngle_flag,										// ���Ƕ�����
    CCWAngle_flag,										// ���Ƕ�����
    AutoStart_flag,										// �Զ���ʼ����
    AutoStop_flag,										// �Զ�ֹͣ����
	ApicalMode_flag,									// ���Ᵽ������
    BarPosition_flag,									// ����ο�������
} SEL_MODE_e;                 							// �������ñ�־λ
extern SEL_MODE_e sel_mode_flag; 


typedef enum
{
    Versions_flag = 1,									// �汾����ʾ
    AutoPowerOff_flag,									// �Զ��ػ���������
    AutoStandy_flag,									// ���ش�������ʱ������
    BeepVol_flag,										// ����������
    Bl_flag,											// ��������
    Hand_flag,											// ����������
    Apexsensitivity_flag,								// ��������������
	Language_flag,										// ��������
    Calibration_flag,									// У׼����
	RestoreFactory_flag,								// �ָ���������
	MenuStartup_flag,
} SET_MODE_e;                 							// �������ñ�־λ
extern SET_MODE_e set_mode_flag; 


typedef enum
{
    File_name_flag = 1,									// �ļ�������
    Seq_name_flag,										// ﱸ���
    Taper_flag,											// ׶��
    Number_flag,										// ﱺ�
    OperaMode_flag,										// ����ģʽ
    RecSpeed_flag,										// �ٶ�
    torque_flag,										// ת��
	AngleCW_flag,										// ���Ƕ�
    AngleCCW_flag,										// ���Ƕ�
	color_flag,											// ��ɫ
} NEW_FILE_e;                 							// �������ñ�־λ
extern NEW_FILE_e new_file_flag; 

typedef struct 
{
    uint8_t KeyStateIndex; 				//��ǰ״̬������
    uint8_t KeyONState; 				//����"����"��ʱת���״̬������
    uint8_t KeySelState; 				//���¡�ѡ�񡱼�ʱת���״̬������
    uint8_t KeyDnState;  				//����"���¡���ʱת���״̬������
    uint8_t KeyUpState;  				//����"����"��ʱת���״̬������
    uint8_t KeyLongSelState; 			//������ѡ�񡱼�ʱת���״̬������
    void (*CurrentOperate)(); 			//��ǰ״̬Ӧ��ִ�еĹ��ܲ���
} KbdTabStruct;
int WaitTimeOut(void);
#endif















