
////////////////////////////////////////////////////////////////////
/// @file DataStorage.c
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

#include "DataStorage.h"
#include "MenuData.h"
#include "eeprom.h"

Data2EEprom store;								//待机界面参数存储，即M0、M1、M2....，在初始化或恢复出厂设置时刷一遍
mem2EEprom save;								//修改单个待机界面设置参数时，用到
set2EEprom save_setting;						//修改设置参数时，用到
file2EEprom save_file_seq;

char ReName[RENAME_sum][11] = {
		"New file",
		"New file",
		"New file",
		"New file",
		"New file",		
		"New file",
		"New file",
		"New file",
		"New file",	
		"New file",
};				//用户自定义锉头名时用到

char ReStandyName[RENAME_sum][11];				//用户自定义锉头名时用到
char SEQ_Name[10][3];							//锉名，最多3位字符串

char Versions[] = "S+.1.0";						//当前版本
char Versions1[] = "S+.1.0.0";					//当前版本
extern char* DispEnSetVer[];					//版本号显示
extern IWDG_HandleTypeDef hiwdg;				//看门狗复位
extern uint8_t Outside_file_add_num[1];

/**********************************************************************************************************
*	函 数 名: void Store2EepromInit()
*	功能说明: 初始化EEPROM，新机器或换新EEPROM时，刷入初始数据
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void Store2EepromInit()
{
    //m0
    store.Data[0].m                = 0;
	store.Data[0].taper            = 0;
	store.Data[0].number           = 0;
    store.Data[0].OperaMode        = 4;
    store.Data[0].Speed            = s300;
    store.Data[0].RecSpeed         = 3;
    store.Data[0].AtrTorque        = 3;
    store.Data[0].Torque           = t30;
    store.Data[0].AngleCCW         = d150;
    store.Data[0].AngleCW          = d30;
    store.Data[0].ApicalMode       = 0;
    store.Data[0].AutoStart        = 1;
    store.Data[0].AutoStop         = 0;
    store.Data[0].FlashBarPosition = 0;
    store.Data[0].FileLibrary      = 0;
	store.Data[0].FileSeq          = 0;
	store.Data[0].FileColor        = c_lv;
	store.Data[0].FileSave         = 0;
	store.Data[0].Vaule1           = 0;
	store.Data[0].Vaule2           = 0;
    //m1
    store.Data[1].m                = 1;
	store.Data[1].taper            = 0;
	store.Data[1].number           = 0;
    store.Data[1].OperaMode        = 0;
    store.Data[1].Speed            = s300;
    store.Data[1].RecSpeed         = 3;
    store.Data[1].AtrTorque        = 3;
    store.Data[1].Torque           = t30;
    store.Data[1].AngleCCW         = d150;
    store.Data[1].AngleCW          = d30;
    store.Data[1].ApicalMode       = 0;
    store.Data[1].AutoStart        = 1;
    store.Data[1].AutoStop         = 0;
    store.Data[1].FlashBarPosition = 2;
    store.Data[1].FileLibrary      = 0;
	store.Data[1].FileSeq          = 0;
	store.Data[1].FileColor        = 0;
	store.Data[1].FileSave         = 0;
	store.Data[1].Vaule1           = 0;
	store.Data[1].Vaule2           = 0;
    //m2
    store.Data[2].m                = 2;
	store.Data[2].taper            = 0;
	store.Data[2].number           = 0;
    store.Data[2].OperaMode        = 0;
    store.Data[2].Speed            = s400;
    store.Data[2].RecSpeed         = 3;
    store.Data[2].AtrTorque        = 3;
    store.Data[2].Torque           = t20;
    store.Data[2].AngleCCW         = d180;
    store.Data[2].AngleCW          = d50;
    store.Data[2].ApicalMode       = 0;
    store.Data[2].AutoStart        = 1;
    store.Data[2].AutoStop         = 0;
    store.Data[2].FlashBarPosition = 2;
    store.Data[2].FileLibrary      = 0;
	store.Data[2].FileSeq          = 0;
	store.Data[2].FileColor        = 0;
	store.Data[2].FileSave         = 0;
	store.Data[2].Vaule1           = 0;
	store.Data[2].Vaule2           = 0;
    //m3
    store.Data[3].m                = 3;
	store.Data[3].taper            = 0;
	store.Data[3].number           = 0;
    store.Data[3].OperaMode        = 2;
    store.Data[3].Speed            = s500;
    store.Data[3].RecSpeed         = 4;
    store.Data[3].AtrTorque        = 3;
    store.Data[3].Torque           = t20;
    store.Data[3].AngleCCW         = d150;
    store.Data[3].AngleCW          = d30;
    store.Data[3].ApicalMode       = 0;
    store.Data[3].AutoStart        = 1;
    store.Data[3].AutoStop         = 0;
    store.Data[3].FlashBarPosition = 2;
    store.Data[3].FileLibrary      = 0;
	store.Data[3].FileSeq          = 0;
	store.Data[3].FileColor        = 0;
	store.Data[3].FileSave         = 0;
	store.Data[3].Vaule1           = 0;
	store.Data[3].Vaule2           = 0;
    //m4
    store.Data[4].m                = 4;
	store.Data[4].taper            = 0;
	store.Data[4].number           = 0;
    store.Data[4].OperaMode        = 2;
    store.Data[4].Speed            = 5;
    store.Data[4].RecSpeed         = 4;
    store.Data[4].AtrTorque        = 3;
    store.Data[4].Torque           = t30;
    store.Data[4].AngleCCW         = d160;
    store.Data[4].AngleCW          = d40;
    store.Data[4].ApicalMode       = 0;
    store.Data[4].AutoStart        = 1;
    store.Data[4].AutoStop         = 0;
    store.Data[4].FlashBarPosition = 2;
    store.Data[4].FileLibrary      = 0;
	store.Data[4].FileSeq          = 0;
	store.Data[4].FileColor        = 0;
	store.Data[4].FileSave         = 0;
	store.Data[4].Vaule1           = 0;
	store.Data[4].Vaule2           = 0;
    //m5
    store.Data[5].m                = 5;
	store.Data[5].taper            = 0;
	store.Data[5].number           = 0;
    store.Data[5].OperaMode        = 3;
    store.Data[5].Speed            = s300;
    store.Data[5].RecSpeed         = 4;
    store.Data[5].AtrTorque        = 3;
    store.Data[5].Torque           = t15;
    store.Data[5].AngleCCW         = d50;
    store.Data[5].AngleCW          = d370;
    store.Data[5].ApicalMode       = 0;
    store.Data[5].AutoStart        = 1;
    store.Data[5].AutoStop         = 0;
    store.Data[5].FlashBarPosition = 2;
    store.Data[5].FileLibrary      = 0;
	store.Data[5].FileSeq          = 0;
	store.Data[5].FileColor        = 0;
	store.Data[5].FileSave         = 0;
	store.Data[5].Vaule1           = 0;
	store.Data[5].Vaule2           = 0;
    //m6
    store.Data[6].m                = 6;
	store.Data[6].taper            = 0;
	store.Data[6].number           = 0;
    store.Data[6].OperaMode        = 3;
    store.Data[6].Speed            = s300;
    store.Data[6].RecSpeed         = 3;
    store.Data[6].AtrTorque        = 3;
    store.Data[6].Torque           = t10;
    store.Data[6].AngleCCW         = d50;
    store.Data[6].AngleCW          = d210;
    store.Data[6].ApicalMode       = 0;
    store.Data[6].AutoStart        = 1;
    store.Data[6].AutoStop         = 0;
    store.Data[6].FlashBarPosition = 2;
    store.Data[6].FileLibrary      = 0;
	store.Data[6].FileSeq          = 0;
	store.Data[6].FileColor        = 0;
	store.Data[6].FileSave         = 0;
	store.Data[6].Vaule1           = 0;
	store.Data[6].Vaule2           = 0;
    //m7
    store.Data[7].m                = 7;
	store.Data[7].taper            = 0;
	store.Data[7].number           = 0;
    store.Data[7].OperaMode        = 1;
    store.Data[7].Speed            = s350;
    store.Data[7].RecSpeed         = 3;
    store.Data[7].AtrTorque        = 3;
    store.Data[7].Torque           = t25;
    store.Data[7].AngleCCW         = d60;
    store.Data[7].AngleCW          = d120;
    store.Data[7].ApicalMode       = 0;
    store.Data[7].AutoStart        = 1;
    store.Data[7].AutoStop         = 0;
    store.Data[7].FlashBarPosition = 2;
    store.Data[7].FileLibrary      = 0;
	store.Data[7].FileSeq          = 0;
	store.Data[7].FileColor        = 0;
	store.Data[7].FileSave         = 0;
	store.Data[7].Vaule1           = 0;
	store.Data[7].Vaule2           = 0;
    //m8
    store.Data[8].m                = 8;
	store.Data[8].taper            = 0;
	store.Data[8].number           = 0;
    store.Data[8].OperaMode        = 1;
    store.Data[8].Speed            = s500;
    store.Data[8].RecSpeed         = 3;
    store.Data[8].AtrTorque        = 3;
    store.Data[8].Torque           = t20;
    store.Data[8].AngleCCW         = d180;
    store.Data[8].AngleCW          = d50;
    store.Data[8].ApicalMode			 = 0;
    store.Data[8].AutoStart        = 1;
    store.Data[8].AutoStop         = 0;
    store.Data[8].FlashBarPosition = 2;
    store.Data[8].FileLibrary      = 0;
	store.Data[8].FileSeq          = 0;
	store.Data[8].FileColor        = 0;
	store.Data[8].FileSave         = 0;
	store.Data[8].Vaule1           = 0;
	store.Data[8].Vaule2           = 0;
    //m9
    store.Data[9].m                = 9;
	store.Data[9].taper            = 0;
	store.Data[9].number           = 0;
    store.Data[9].OperaMode        = 0;
    store.Data[9].Speed            = s800;
    store.Data[9].RecSpeed         = 3;
    store.Data[9].AtrTorque        = 3;
    store.Data[9].Torque           = t15;
    store.Data[9].AngleCCW         = d90;
    store.Data[9].AngleCW          = d90;
    store.Data[9].ApicalMode       = 0;
    store.Data[9].AutoStart        = 1;
    store.Data[9].AutoStop         = 0;
    store.Data[9].FlashBarPosition = 2;
    store.Data[9].FileLibrary      = 0;
	store.Data[9].FileSeq          = 0;
	store.Data[9].FileColor        = 0;
	store.Data[9].FileSave         = 0;
	store.Data[9].Vaule1           = 0;
	store.Data[9].Vaule2           = 0;
    //m10
    store.Data[10].m                = 10;
	store.Data[10].taper            = 0;
	store.Data[10].number           = 0;
    store.Data[10].OperaMode        = 0;
    store.Data[10].Speed            = s1000;
    store.Data[10].RecSpeed         = 3;
    store.Data[10].AtrTorque        = 3;
    store.Data[10].Torque           = t10;
    store.Data[10].AngleCCW         = d150;
    store.Data[10].AngleCW          = d30;
    store.Data[10].ApicalMode       = 0;
    store.Data[10].AutoStart        = 1;
    store.Data[10].AutoStop         = 0;
    store.Data[10].FlashBarPosition = 2;
    store.Data[10].FileLibrary      = 0;
	store.Data[10].FileSeq          = 0;
	store.Data[10].FileColor        =c_hei;
	store.Data[10].FileSave         = 0;
	store.Data[10].Vaule1           = 0;
	store.Data[10].Vaule2           = 0;
	
    EEPROM_Write(MEM_ADDR_START,store.WriteEE,sizeof(struct Store_)*NUM);	//将结构体数据写入eeprom
	
    HAL_Delay(10);
	HAL_IWDG_Refresh(&hiwdg);												//喂狗  //2s会饿
	
    //设置 初始化
    save_setting.Data.AutoPowerOffTime		 = 7;							//10分钟
    save_setting.Data.AutoStandyTime		 = 2;							//5秒钟
    save_setting.Data.BeepVol				 = 2;							//1音量
    save_setting.Data.Hand					 = 0;							//右手
    save_setting.Data.ApexSense				 = 1;							//从M1开始
    save_setting.Data.Language				 = 0;							//英文
    save_setting.Data.Calibration			 = 0;							//关闭校准
    save_setting.Data.backlight				 = 3;							//bl
    save_setting.Data.RestoreFactorySettings = 0;							//关闭恢复出厂设置
	save_setting.Data.StartupMemory			 = 0;							//从M1开始
	save_setting.Data.EMC_SCR_PROTECT		 = 1;							//开启屏幕重置程序
    EEPROM_Write(SETTING_ADDR,save_setting.WriteEE,sizeof(struct Set_));	//将结构体数据写入eeprom
	HAL_Delay(10);
	
	Outside_file_add_num[0] = 0;
	Write_File_number(Outside_file_add_num);
    HAL_Delay(10);
}

/**********************************************************************************************************
*	函 数 名: MemorySave(int mem_num)
*	功能说明: 存储设置，将修改的参数存储到EEPROM中，与待机界面有关
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void MemorySave(int mem_num)
{
    uint16_t EEaddr	= 0;
    EEaddr = (mem_num * 20) + MEM_ADDR_START;						//计算地址

    save.Data.m = mem_num;
    //转存当前的配置信息
	save.Data.taper				= sel.taper;					//存储锉号
	save.Data.number			= sel.number;					//存储锉号
    save.Data.OperaMode			= sel.OperaMode;				//存储操作模式
    save.Data.Speed				= sel.Speed;					//存储速度
    save.Data.RecSpeed			= sel.RecSpeed;					//存储往复模式速度
    save.Data.AtrTorque			= sel.AtrTorque;				//存储ATR模式下的转矩上限
    save.Data.Torque			= sel.Torque;					//存储最大转矩
    save.Data.AngleCCW			= sel.AngleCCW;					//存储正转角度
    save.Data.AngleCW			= sel.AngleCW;					//存储反转角度
    save.Data.ApicalMode		= sel.ApicalMode;				//存储根测模式
    save.Data.AutoStart			= sel.AutoStart;				//存储根测自动开始
    save.Data.AutoStop			= sel.AutoStop;					//存储根测自动停止
    save.Data.FlashBarPosition 	= sel.FlashBarPosition;			//存储根测停止位置
    save.Data.FileLibrary		= sel.FileLibrary;				//存储
    save.Data.FileSeq			= sel.FileSeq;					//存储锉号
	save.Data.FileColor			= sel.color;					//存储锉颜色，与界面有关
	save.Data.FileSave          = sel.FileSave;					//程序读取位置
	save.Data.Vaule1			= sel.Vaule1;					//用于记录参数是否变化，即速度扭矩或正角度、负角度
	save.Data.Vaule2			= sel.Vaule2;					//用于记录参数是否变化，即速度扭矩或正角度、负角度
	
    EEPROM_Write( EEaddr, save.WriteEE, sizeof(struct Store_));	//将save.WriteEE中的数据写入EEPROM中
	
    HAL_Delay(50);

}

/**********************************************************************************************************
*	函 数 名: MemoryRead(int mem_num)
*	功能说明: 将EEPROM中的数据读出
*	形    参: mem_num，存储标号
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void MemoryRead(int mem_num)
{
    uint16_t EEaddr = 0;
    EEaddr = mem_num*20+MEM_ADDR_START;							//计算地址
	
    EEPROM_Read(EEaddr,save.WriteEE,sizeof(struct Store_));		//将EEPROM中的数据读到save.WriteEE中
	
	sel.taper			 = save.Data.taper;
	sel.number			 = save.Data.number;
    sel.OperaMode   	 = save.Data.OperaMode;
    sel.Speed			 = save.Data.Speed;
    sel.RecSpeed		 = save.Data.RecSpeed;
    sel.Torque			 = save.Data.Torque;
    sel.AtrTorque		 = save.Data.AtrTorque;
    sel.AngleCCW		 = save.Data.AngleCCW;
    sel.AngleCW			 = save.Data.AngleCW;
    sel.ApicalMode		 = save.Data.ApicalMode;
    sel.AutoStart		 = save.Data.AutoStart;
    sel.AutoStop		 = save.Data.AutoStop;
    sel.FlashBarPosition = save.Data.FlashBarPosition;
    sel.FileLibrary		 = save.Data.FileLibrary;
	sel.FileSeq			 = save.Data.FileSeq;
	sel.color			 = save.Data.FileColor;
	sel.FileSave		 = save.Data.FileSave;
	sel.Vaule1			 = save.Data.Vaule1;
	sel.Vaule2			 = save.Data.Vaule2;
}

/**********************************************************************************************************
*	函 数 名: SettingSave()
*	功能说明: 将Setting（设置参数）保存到EEPROM中
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SettingSave()
{
    save_setting.Data.AutoPowerOffTime       = set.AutoPowerOffTime;
    save_setting.Data.AutoStandyTime	 	 = set.AutoStandyTime;
    save_setting.Data.BeepVol				 = set.BeepVol;
    save_setting.Data.Hand					 = set.Hand;
    save_setting.Data.ApexSense				 = set.ApexSense;
    save_setting.Data.Language				 = set.Language;
    save_setting.Data.Calibration			 = set.Calibration;
    save_setting.Data.backlight				 = set.backlight;
    save_setting.Data.RestoreFactorySettings = set.RestoreFactorySettings;
	save_setting.Data.StartupMemory			 = set.StartupMemory;
	save_setting.Data.EMC_SCR_PROTECT		 = set.EMC_SCR_PROTECT;
	
    EEPROM_Write(SETTING_ADDR,save_setting.WriteEE,sizeof(struct Set_));	//将save_setting.WriteEE中的数据写入EEPROM中
    HAL_Delay(100);

}

/**********************************************************************************************************
*	函 数 名: SettingRead()
*	功能说明: 将EEPROM的数据读取到Setting（设置参数）
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SettingRead()
{
    EEPROM_Read(SETTING_ADDR,save_setting.WriteEE,sizeof(struct Set_));		//将EEPROM中的数据读到save_setting.WriteEE中
	
    HAL_Delay(100);
	
    set.AutoPowerOffTime		= save_setting.Data.AutoPowerOffTime;
    set.AutoStandyTime			= save_setting.Data.AutoStandyTime;
    set.BeepVol					= save_setting.Data.BeepVol;
    set.Hand					= save_setting.Data.Hand;
    set.ApexSense				= save_setting.Data.ApexSense;
    set.Language				= save_setting.Data.Language;
    set.Calibration				= save_setting.Data.Calibration;
    set.backlight				= save_setting.Data.backlight;
    set.RestoreFactorySettings	= save_setting.Data.RestoreFactorySettings;
	set.StartupMemory  			= save_setting.Data.StartupMemory;
	set.EMC_SCR_PROTECT         = save_setting.Data.EMC_SCR_PROTECT;
}

/**********************************************************************************************************
*	函 数 名: SaveLastMem()
*	功能说明: 将上次程序号写入EEPROM中
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SaveLastMem()
{
    uint8_t dat;
    dat	= mem.MemoryNum;
    EEPROM_Write(LAST_MEM,&dat,1);
}

/**********************************************************************************************************
*	函 数 名: ReadLastMem()
*	功能说明: 从EEPROM中读取上次程序号
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void ReadLastMem()
{
    uint8_t dat;
    EEPROM_Read(LAST_MEM,&dat,1);
    mem.MemoryNum	=	dat;
}

///**********************************************************************************************************
//*	函 数 名: ReadReName(uint8_t num)
//*	功能说明: 从EEPROM中读取锉号文件名
//*	形    参: num为文件名序号，目前1~10
//*	返 回 值: 无
//*	编 辑 者: 1：王昌盛		
//*	修订记录: 1:
//*	编辑日期: 1: 20210811 				         
//**********************************************************************************************************/
//void ReadSEQReName(uint8_t num)
//{
//	uint16_t EEaddr	=	0;
//	EEaddr	=	Seq_NAME_ADDR + num * 4;
//	EEPROM_Read( EEaddr, SEQ_Name[num],4);
//}

///**********************************************************************************************************
//*	函 数 名: SaveReName(uint8_t num)
//*	功能说明: 将锉号文件名写入EEPROM中
//*	形    参: num为文件名序号，目前1~11
//*	返 回 值: 无
//*	编 辑 者: 1：王昌盛		
//*	修订记录: 1:
//*	编辑日期: 1: 20210811 				         
//**********************************************************************************************************/
//void SaveSEQReName(uint8_t num)
//{
//	uint16_t EEaddr	=	0;
//	EEaddr	=	Seq_NAME_ADDR + num * 4;

//	EEPROM_Write(EEaddr,SEQ_Name[num],4);
//}

/**********************************************************************************************************
*	函 数 名: ReadReName(uint8_t num)
*	功能说明: 从EEPROM中读取锉号文件名
*	形    参: num为文件名序号，目前1~10
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void ReadStandyReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	STANDYRENAME_ADDR + (num-1) * 11;
	EEPROM_Read( EEaddr, ReStandyName[num-1],11);
}

/**********************************************************************************************************
*	函 数 名: SaveReName(uint8_t num)
*	功能说明: 将锉号文件名写入EEPROM中
*	形    参: num为文件名序号，目前1~11
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SaveStandyReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	STANDYRENAME_ADDR + (num-1) * 11;

	EEPROM_Write(EEaddr,ReStandyName[num-1],11);
}

/**********************************************************************************************************
*	函 数 名: ReadReName(uint8_t num)
*	功能说明: 从EEPROM中读取锉号文件名
*	形    参: num为文件名序号，目前1~10
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void ReadReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	RENAME_ADDR + num * 11;
	EEPROM_Read( EEaddr, ReName[num],11);
}

/**********************************************************************************************************
*	函 数 名: SaveReName(uint8_t num)
*	功能说明: 将锉号文件名写入EEPROM中
*	形    参: num为文件名序号，目前1~11
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SaveReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	RENAME_ADDR + num * 11;

	EEPROM_Write(EEaddr,ReName[num],11);
}

/**********************************************************************************************************
*	函 数 名: InitReName()
*	功能说明: 将锉号文件名初始化。并写入EEPROM中
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void InitReName()
{
	uint16_t EEaddr = 0;
	int i;
//	ReName[RENAME_sum][11] =
//	{
//		"file name",
//		"file name",
//		"file name",
//		"file name",
//		"file name",		
//		"file name",
//		"file name",
//		"file name",
//		"file name",	
//		"file name",
//	};
	for(i = 0; i < RENAME_sum; i++)
	{
		EEaddr = RENAME_ADDR + i*11;
		EEPROM_Write(EEaddr,ReName[i],11);			//将str[i]写入EEPROM
	}
}

/**********************************************************************************************************
*	函 数 名: Save1_Read0_Torque(uint8_t yn)
*	功能说明: 校准13种转矩值，并写入EEPROM中
*	形    参: uint8_t yn，校准模式，yn = 1，将转矩写入，其他模式读出
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/

void Save1_Read0_Torque(uint8_t yn)
{
	uint8_t Torque8[678];
	uint8_t i,k;
	
	uint16_t t16 = 0, h8, l8, j;
	if(yn == 1) 
	{
		j = 0;
		for(i = 0; i < 3; i++)												//扭矩校准，0  ~ 1.0的扭矩，全速度
		{
			for(k = 0; k < 32; k++)
			{
				t16 = S_T_I_COE1[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE1[i][k] & 0x00ff;			//低8位
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE1[i][k] >> 8); 				//高8位
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 3; i++)												//扭矩校准，1.0~ 2.0的扭矩，除去1500rpm
		{
			for(k = 0; k < 29; k++)
			{
				t16 = S_T_I_COE2[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE2[i][k] & 0x00ff;			//低8位
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE2[i][k] >> 8); 				//高8位
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 6; i++)												//扭矩校准，2.0~ 4.0的扭矩，除去700rpm往后
		{
			for(k = 0; k < 21; k++)
			{
				t16 = S_T_I_COE3[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE3[i][k] & 0x00ff;			//低8位
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE3[i][k] >> 8); 				//高8位
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 2; i++)												//扭矩校准，	 5.0的扭矩，除去400rpm往后
		{
			for(k = 0; k < 15; k++)
			{
				t16 = S_T_I_COE4[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE4[i][k] & 0x00ff;			//低8位
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE4[i][k] >> 8); 				//高8位
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR,Torque8,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR+100,Torque8+100,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR+200,Torque8+200,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR+300,Torque8+300,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR+400,Torque8+400,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR+500,Torque8+500,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Write(TORQUE_ADDR+600,Torque8+600,78);
		HAL_IWDG_Refresh(&hiwdg);
	}
	else if(yn == 0)
	{
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR,Torque8,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR+100,Torque8+100,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR+200,Torque8+200,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR+300,Torque8+300,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR+400,Torque8+400,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR+500,Torque8+500,100);
		HAL_IWDG_Refresh(&hiwdg);
		EEPROM_Read(TORQUE_ADDR+600,Torque8+600,78);
		HAL_IWDG_Refresh(&hiwdg);
		j = 0;
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 3; i++)										//扭矩校准，0  ~ 1.0的扭矩，全速度
		{
			for(k = 0; k < 32; k++)
			{
				l8 = Torque8[j];
				j++;
				h8 = Torque8[j];
				S_T_I_COE1[i][k] = (h8 << 8) | l8;
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 3; i++)										//扭矩校准，1.0~ 2.0的扭矩，除去1500rpm
		{
			for(k = 0; k < 29; k++)
			{
				l8 = Torque8[j];
				j++;
				h8 = Torque8[j];
				S_T_I_COE2[i][k] = (h8 << 8) | l8;
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 6; i++)										//扭矩校准，2.0~ 4.0的扭矩，除去700rpm往后
		{
			for(k = 0; k < 21; k++)
			{
				l8 = Torque8[j];
				j++;
				h8 = Torque8[j];
				S_T_I_COE3[i][k] = (h8 << 8) | l8;
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 2; i++)										//扭矩校准，	 5.0的扭矩，除去400rpm往后
		{
			for(k = 0; k < 15; k++)
			{
				l8 = Torque8[j];
				j++;
				h8 = Torque8[j];
				S_T_I_COE4[i][k] = (h8 << 8) | l8;
				j++;
			}
		}
	}
	 HAL_IWDG_Refresh(&hiwdg);
}


/**********************************************************************************************************
*	函 数 名: Save1_Read0_NoloadTor(uint8_t yn)
*	功能说明: 校准空载转矩值，并写入EEPROM中
*	形    参: uint8_t yn，校准模式，yn = 1，将转矩写入，其他模式读出
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void Save1_Read0_NoloadTor(uint8_t yn)
{
    uint8_t tor8[64];
    uint8_t i,j;
    uint16_t t16=0,h8,l8;
    if(yn==1) 
	{
        for(i=0; i<64; i++)
        {
			NoLoadCurrent[i] += 128;								// 把负数转换为正数，方便存储
            tor8[i]=(uint8_t)NoLoadCurrent[i];						// 低8位
        }
        EEPROM_Write(NoLoadTOR_ADDR,tor8,64);
    }
    else if(yn==0)
    {
        EEPROM_Read(NoLoadTOR_ADDR,tor8,64);
        for(i=0; i<64; i++)
        {
            NoLoadCurrent[i] = tor8[i];
			NoLoadCurrent[i] -= 128;								// 还原
        }
    }
}

/**********************************************************************************************************
*	函 数 名: SaveVersions()
*	功能说明: 保存软件版本号，写入EEPROM中
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SaveVersions()
{
    char Ver[9]= {0};
    uint8_t i=0;
    for(i=0; i<9; i++)
    {
        Ver[i] = Versions[i];		//将字符串数值转换成无符号数据
    }

    EEPROM_Write(VER_ADDR,Ver,9);
}

/**********************************************************************************************************
*	函 数 名: ReadVersions()
*	功能说明: 从EEPROM中读取软件版本号
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void ReadVersions()
{
    char Ver[9] = {0};
    uint8_t i = 0;
    EEPROM_Read( VER_ADDR, Ver, 9);
    printf("%s\n",Ver);
    for(i=0; i<9; i++)
    {
        Versions[i] = Ver[i];
    }
    printf("%s\n",Versions);
}

/**********************************************************************************************************
*	函 数 名: void SaveOutsideseq(mem_num)
*	功能说明: 将用户自定义参数保存到EEPROM
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void SaveOutsideseq(int mem_num)
{
	uint8_t i;
	uint16_t EEaddr = 0;
    EEaddr = mem_num*37 + Seq_ADDR;													//计算地址
	
	save_file_seq.Data.Seq_num = New_file_parameter.seq_Len;
	for(i = 0; i < New_file_parameter.seq_Len; i++)
	{
		save_file_seq.Data.newfile[i].taper = New_file_parameter.fseq[i].taper;
		save_file_seq.Data.newfile[i].number = New_file_parameter.fseq[i].number;
		save_file_seq.Data.newfile[i].OperaMode = New_file_parameter.fseq[i].OperaMode;
		save_file_seq.Data.newfile[i].color = New_file_parameter.fseq[i].color;
		if((New_file_parameter.fseq[i].OperaMode == 0) || (New_file_parameter.fseq[i].OperaMode == 1))
		{
			save_file_seq.Data.newfile[i].data1 = New_file_parameter.fseq[i].speed;
			save_file_seq.Data.newfile[i].data2 = New_file_parameter.fseq[i].torque;
		}else 
		{
			save_file_seq.Data.newfile[i].data1 = New_file_parameter.fseq[i].AngleCW;
			save_file_seq.Data.newfile[i].data2 = New_file_parameter.fseq[i].AngleCCW;
		}
	}
	
	EEPROM_Write(EEaddr,save_file_seq.WriteEE,sizeof(struct saveseq_));				//将save_file_seq.WriteEE中的数据写入EEPROM中
	
    HAL_Delay(100);		
}

/**********************************************************************************************************
*	函 数 名: void ReadOutsideseq(mem_num)
*	功能说明: 从EEPROM中读取用户自定义锉参数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void ReadOutsideseq(int mem_num)
{
	uint8_t i;
	uint16_t EEaddr = 0;
    EEaddr = mem_num*37 + Seq_ADDR;													//计算地址
	
    EEPROM_Read(EEaddr,save_file_seq.WriteEE,sizeof(struct saveseq_));				//将EEPROM中的数据读到save.WriteEE中
	
	New_file_parameter.seq_Len = save_file_seq.Data.Seq_num;
	for(i = 0; i < New_file_parameter.seq_Len; i++)
	{
		New_file_parameter.fseq[i].taper = save_file_seq.Data.newfile[i].taper;
		New_file_parameter.fseq[i].number = save_file_seq.Data.newfile[i].number;
		New_file_parameter.fseq[i].OperaMode = save_file_seq.Data.newfile[i].OperaMode;
		New_file_parameter.fseq[i].color = save_file_seq.Data.newfile[i].color;
		if((New_file_parameter.fseq[i].OperaMode == 0) || (New_file_parameter.fseq[i].OperaMode == 1))
		{
			New_file_parameter.fseq[i].speed = save_file_seq.Data.newfile[i].data1;
			New_file_parameter.fseq[i].torque = save_file_seq.Data.newfile[i].data2;
		}else 
		{
			New_file_parameter.fseq[i].AngleCW = save_file_seq.Data.newfile[i].data1;
			New_file_parameter.fseq[i].AngleCCW = save_file_seq.Data.newfile[i].data2;
		}
	}
	
    HAL_Delay(100);		
}

/**********************************************************************************************************
*	函 数 名: void ReadOutsideseq(mem_num)
*	功能说明: 从EEPROM中读取用户自定义锉文件夹的总数
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void Read_File_number(uint8_t *number)
{
//	uint8_t dat;
//    EEPROM_Read(Filenumber_ADDR,&dat,1);
	*number = EEPROM_ReadOneByte(Filenumber_ADDR);
//	EEPROM_Read(Filenumber_ADDR,number,1);
//	number = EEPROM_ReadOneByte(Filenumber_ADDR);
}

/**********************************************************************************************************
*	函 数 名: void SaveOutsideseq(mem_num)
*	功能说明: 将用户自定义锉文件夹的总数保存到EEPROM
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void Write_File_number(uint8_t *number)
{
	EEPROM_Write(Filenumber_ADDR,number,1);
//	EEPROM_WriteOneByte(Filenumber_ADDR, number);
}
/**********************************************************************************************************
*	函 数 名: void HardWareIDRead(void)
*	功能说明: 读取屏幕ID
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void HardWareIDRead(void)
{
	uint8_t scr_id_rdd;
	EEPROM_Read(SCR_EMC_PROTECT,&scr_id_rdd,1);
	sys.Disp_scr_id = scr_id_rdd;
}
/**********************************************************************************************************
*	函 数 名: void HardWareIDRead(void)
*	功能说明: 将屏幕ID保存到EEPROM
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛		
*	修订记录: 1:
*	编辑日期: 1: 20210811 				         
**********************************************************************************************************/
void HardWareIDWrite(uint8_t *number)
{
	EEPROM_Write(SCR_EMC_PROTECT,number,1);
}




