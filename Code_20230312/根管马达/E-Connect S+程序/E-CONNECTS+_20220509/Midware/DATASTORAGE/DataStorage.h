
////////////////////////////////////////////////////////////////////
/// @file DataStorage.h
/// @brief ����洢EEPROM
/// 
/// �ļ���ϸ������	Store2EepromInit()��ʼ��EEPROM��������оƬʱ��ˢ���ʼ����
///					
/// 
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210811
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////

#ifndef __DATASTORAGE_H
#define __DATASTORAGE_H

#include "stm32f4xx_hal.h"
#include "datatype.h"

#define NUM 11

#define TORQUE_1 	28
#define TORQUE_2 	27
#define TORQUE_4 	21
#define TORQUE_5 	15

#define RECOVERY_ADDR 		0						//0~1	0xAB,0XCD		// ��дУ��λ
#define TQFLAG_ADDR 		4						//4~5	//Ť�ر�־λ
#define ROOTOUTPOS_ADDR 	6						//6~7 �����ڿ��ڴ���ֵ
#define ROOTFLAG_ADDR 		8  						//����У׼���±�־
#define LAST_MEM  			9						//�ϴγ����
#define MEM_ADDR_START 		10						//10~230  ���ò����洢

#define SETTING_ADDR 		340   					//340~350���ò����洢 
#define SCR_EMC_PROTECT		351						//355~356	������Ļ�������ñ�־λ
#define RENAME_ADDR 		355  					//355~465�û�����ֱ���λ��
#define Filenumber_ADDR		470  					//470	�û������ļ��и���

#define Seq_ADDR			475  					//490~855	�洢��ʼ��ַ

#define STANDYRENAME_ADDR	860						//860~970

#define VER_ADDR			975						//975~985	E.1.1.008
#define UPDATA_ADDR			990						//990-991 APP����У��λ

#define NoLoadTOR_ADDR 		1000					//1000~1056 ����Ť�ص���
#define TORQUE_ADDR 		1100					//1200~1828 Ť��У׼  728


#define RENAME_sum 			10  					//�û���������С
extern char ReStandyName[RENAME_sum][11];
extern char ReName[RENAME_sum][11];
extern char SEQ_Name[10][3];						//��������3λ�ַ���
/**********���в���*******/
typedef struct Store_
{
	uint8_t m;
	uint8_t taper;									//׶��
	uint8_t number;									//����
	uint8_t OperaMode;								
	uint8_t Speed;
	uint8_t RecSpeed;
	uint8_t Torque;
	uint8_t AtrTorque;
	uint8_t AngleCW;
	uint8_t AngleCCW;
	uint8_t ApicalMode;
	uint8_t AutoStart;
	uint8_t AutoStop;
	uint8_t FlashBarPosition;
	uint8_t FileLibrary;
	uint8_t FileSeq;
	uint8_t FileColor;
	uint8_t FileSave;								//�洢λ��  =0��Ĭ�ϣ�=1���û��Զ���洢��=2����Ƭ���ڲ��洢
	uint8_t Vaule1;									//���ڼ�¼�����Ƿ�仯�����ٶ�Ť�ػ����Ƕȡ����Ƕ�
	uint8_t Vaule2;									//���ڼ�¼�����Ƿ�仯�����ٶ�Ť�ػ����Ƕȡ����Ƕ�
} DataStore;

//�����壬���ṹ���Ԫ��ת�����ͨ���飬�Ա㰴eeprom��ʽ�洢
typedef union cf_page
{
	DataStore Data[NUM];
	uint8_t WriteEE[sizeof(struct Store_)*NUM];			//��ʼ���ͻָ���������ʱ��
} Data2EEprom;
extern Data2EEprom store;;


typedef union mem_page
{
	DataStore Data;
	uint8_t WriteEE[sizeof(struct Store_)];				//ÿ�����ñ�����
} mem2EEprom;
extern mem2EEprom save;

/******************************************************/
/********ϵͳ���ò���********/
typedef struct Set_
{
    uint8_t AutoPowerOffTime;
    uint8_t AutoStandyTime;
    uint8_t BeepVol;
    uint8_t Calibration;
    uint8_t Hand;
    uint8_t Language;
    uint8_t RestoreFactorySettings;
    uint8_t ApexSense;
    uint8_t backlight;
	uint8_t StartupMemory;
	uint8_t EMC_SCR_PROTECT;
} SetStore;

typedef union set_page
{
	SetStore Data;
	uint8_t WriteEE[sizeof(struct Set_)];				//ÿ�����ñ�����
} set2EEprom;
extern set2EEprom save_setting;


typedef struct savefile_
{
    uint8_t taper;
    uint8_t number;
    uint8_t OperaMode;
    uint8_t data1;
    uint8_t data2;
    uint8_t color;
} savefile;

typedef struct saveseq_
{
    uint8_t Seq_num;
    savefile newfile[8];
} saveseq;

typedef union set_file
{
	saveseq Data;
	uint8_t WriteEE[sizeof(struct saveseq_)];				//ÿ�����ñ�����
} file2EEprom;
extern file2EEprom save_file_seq;




void MemorySave(int mem_num);
void Store2EepromInit();
void MemoryRead(int mem_num);
void SettingSave();
void SettingRead();
void SaveLastMem();
void ReadLastMem();
void Save1_Read0_Torque(uint8_t yn);
void Save1_Read0_NoloadTor(uint8_t yn);
void SaveVersions();
void ReadVersions();

//void ReadSEQReName(uint8_t num);
//void SaveSEQReName(uint8_t num);

void SaveStandyReName(uint8_t num);
void ReadStandyReName(uint8_t num);

void ReadReName(uint8_t num);
void SaveReName(uint8_t num);
void InitReName();

void ReadOutsideseq(int mem_num);
void SaveOutsideseq(int mem_num);
void Read_File_number(uint8_t *number);
void Write_File_number(uint8_t *number);
void HardWareIDRead(void);
void HardWareIDWrite(uint8_t *number);
#endif





