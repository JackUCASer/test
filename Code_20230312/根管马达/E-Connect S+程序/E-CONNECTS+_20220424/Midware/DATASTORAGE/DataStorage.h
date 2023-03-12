
////////////////////////////////////////////////////////////////////
/// @file DataStorage.h
/// @brief 掉电存储EEPROM
/// 
/// 文件详细描述：	Store2EepromInit()初始化EEPROM，当换新芯片时，刷入初始数据
///					
/// 
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210811
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
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

#define RECOVERY_ADDR 		0						//0~1	0xAB,0XCD		// 重写校验位
#define TQFLAG_ADDR 		4						//4~5	//扭矩标志位
#define ROOTOUTPOS_ADDR 	6						//6~7 根测在开口处的值
#define ROOTFLAG_ADDR 		8  						//根测校准更新标志
#define LAST_MEM  			9						//上次程序号
#define MEM_ADDR_START 		10						//10~230  常用参数存储

#define SETTING_ADDR 		340   					//340~350设置参数存储 
#define SCR_EMC_PROTECT		351						//355~356	静电屏幕死机重置标志位
#define RENAME_ADDR 		355  					//355~465用户锉名字保存位置
#define Filenumber_ADDR		470  					//470	用户新增文件夹个数

#define Seq_ADDR			475  					//490~855	存储起始地址

#define STANDYRENAME_ADDR	860						//860~970

#define VER_ADDR			975						//975~985	E.1.1.008
#define UPDATA_ADDR			990						//990-991 APP更新校验位

#define NoLoadTOR_ADDR 		1000					//1000~1056 空载扭矩电流
#define TORQUE_ADDR 		1100					//1200~1828 扭矩校准  728


#define RENAME_sum 			10  					//用户锉名数组大小
extern char ReStandyName[RENAME_sum][11];
extern char ReName[RENAME_sum][11];
extern char SEQ_Name[10][3];						//锉名，最多3位字符串
/**********运行参数*******/
typedef struct Store_
{
	uint8_t m;
	uint8_t taper;									//锥度
	uint8_t number;									//号数
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
	uint8_t FileSave;								//存储位置  =0，默认；=1，用户自定义存储；=2，单片机内部存储
	uint8_t Vaule1;									//用于记录参数是否变化，即速度扭矩或正角度、负角度
	uint8_t Vaule2;									//用于记录参数是否变化，即速度扭矩或正角度、负角度
} DataStore;

//联合体，将结构体的元素转存成普通数组，以便按eeprom格式存储
typedef union cf_page
{
	DataStore Data[NUM];
	uint8_t WriteEE[sizeof(struct Store_)*NUM];			//初始化和恢复出厂设置时用
} Data2EEprom;
extern Data2EEprom store;;


typedef union mem_page
{
	DataStore Data;
	uint8_t WriteEE[sizeof(struct Store_)];				//每次设置保存用
} mem2EEprom;
extern mem2EEprom save;

/******************************************************/
/********系统设置参数********/
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
	uint8_t WriteEE[sizeof(struct Set_)];				//每次设置保存用
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
	uint8_t WriteEE[sizeof(struct saveseq_)];				//每次设置保存用
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





