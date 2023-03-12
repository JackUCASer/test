
////////////////////////////////////////////////////////////////////
/// @file DataStorage.c
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

#include "DataStorage.h"
#include "MenuData.h"
#include "eeprom.h"

Data2EEprom store;								//������������洢����M0��M1��M2....���ڳ�ʼ����ָ���������ʱˢһ��
mem2EEprom save;								//�޸ĵ��������������ò���ʱ���õ�
set2EEprom save_setting;						//�޸����ò���ʱ���õ�
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
};				//�û��Զ����ͷ��ʱ�õ�

char ReStandyName[RENAME_sum][11];				//�û��Զ����ͷ��ʱ�õ�
char SEQ_Name[10][3];							//��������3λ�ַ���

char Versions[] = "S+.1.0";						//��ǰ�汾
char Versions1[] = "S+.1.0.0";					//��ǰ�汾
extern char* DispEnSetVer[];					//�汾����ʾ
extern IWDG_HandleTypeDef hiwdg;				//���Ź���λ
extern uint8_t Outside_file_add_num[1];

/**********************************************************************************************************
*	�� �� ��: void Store2EepromInit()
*	����˵��: ��ʼ��EEPROM���»�������EEPROMʱ��ˢ���ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
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
	
    EEPROM_Write(MEM_ADDR_START,store.WriteEE,sizeof(struct Store_)*NUM);	//���ṹ������д��eeprom
	
    HAL_Delay(10);
	HAL_IWDG_Refresh(&hiwdg);												//ι��  //2s���
	
    //���� ��ʼ��
    save_setting.Data.AutoPowerOffTime		 = 7;							//10����
    save_setting.Data.AutoStandyTime		 = 2;							//5����
    save_setting.Data.BeepVol				 = 2;							//1����
    save_setting.Data.Hand					 = 0;							//����
    save_setting.Data.ApexSense				 = 1;							//��M1��ʼ
    save_setting.Data.Language				 = 0;							//Ӣ��
    save_setting.Data.Calibration			 = 0;							//�ر�У׼
    save_setting.Data.backlight				 = 3;							//bl
    save_setting.Data.RestoreFactorySettings = 0;							//�رջָ���������
	save_setting.Data.StartupMemory			 = 0;							//��M1��ʼ
	save_setting.Data.EMC_SCR_PROTECT		 = 1;							//������Ļ���ó���
    EEPROM_Write(SETTING_ADDR,save_setting.WriteEE,sizeof(struct Set_));	//���ṹ������д��eeprom
	HAL_Delay(10);
	
	Outside_file_add_num[0] = 0;
	Write_File_number(Outside_file_add_num);
    HAL_Delay(10);
}

/**********************************************************************************************************
*	�� �� ��: MemorySave(int mem_num)
*	����˵��: �洢���ã����޸ĵĲ����洢��EEPROM�У�����������й�
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void MemorySave(int mem_num)
{
    uint16_t EEaddr	= 0;
    EEaddr = (mem_num * 20) + MEM_ADDR_START;						//�����ַ

    save.Data.m = mem_num;
    //ת�浱ǰ��������Ϣ
	save.Data.taper				= sel.taper;					//�洢ﱺ�
	save.Data.number			= sel.number;					//�洢ﱺ�
    save.Data.OperaMode			= sel.OperaMode;				//�洢����ģʽ
    save.Data.Speed				= sel.Speed;					//�洢�ٶ�
    save.Data.RecSpeed			= sel.RecSpeed;					//�洢����ģʽ�ٶ�
    save.Data.AtrTorque			= sel.AtrTorque;				//�洢ATRģʽ�µ�ת������
    save.Data.Torque			= sel.Torque;					//�洢���ת��
    save.Data.AngleCCW			= sel.AngleCCW;					//�洢��ת�Ƕ�
    save.Data.AngleCW			= sel.AngleCW;					//�洢��ת�Ƕ�
    save.Data.ApicalMode		= sel.ApicalMode;				//�洢����ģʽ
    save.Data.AutoStart			= sel.AutoStart;				//�洢�����Զ���ʼ
    save.Data.AutoStop			= sel.AutoStop;					//�洢�����Զ�ֹͣ
    save.Data.FlashBarPosition 	= sel.FlashBarPosition;			//�洢����ֹͣλ��
    save.Data.FileLibrary		= sel.FileLibrary;				//�洢
    save.Data.FileSeq			= sel.FileSeq;					//�洢ﱺ�
	save.Data.FileColor			= sel.color;					//�洢���ɫ��������й�
	save.Data.FileSave          = sel.FileSave;					//�����ȡλ��
	save.Data.Vaule1			= sel.Vaule1;					//���ڼ�¼�����Ƿ�仯�����ٶ�Ť�ػ����Ƕȡ����Ƕ�
	save.Data.Vaule2			= sel.Vaule2;					//���ڼ�¼�����Ƿ�仯�����ٶ�Ť�ػ����Ƕȡ����Ƕ�
	
    EEPROM_Write( EEaddr, save.WriteEE, sizeof(struct Store_));	//��save.WriteEE�е�����д��EEPROM��
	
    HAL_Delay(50);

}

/**********************************************************************************************************
*	�� �� ��: MemoryRead(int mem_num)
*	����˵��: ��EEPROM�е����ݶ���
*	��    ��: mem_num���洢���
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void MemoryRead(int mem_num)
{
    uint16_t EEaddr = 0;
    EEaddr = mem_num*20+MEM_ADDR_START;							//�����ַ
	
    EEPROM_Read(EEaddr,save.WriteEE,sizeof(struct Store_));		//��EEPROM�е����ݶ���save.WriteEE��
	
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
*	�� �� ��: SettingSave()
*	����˵��: ��Setting�����ò��������浽EEPROM��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
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
	
    EEPROM_Write(SETTING_ADDR,save_setting.WriteEE,sizeof(struct Set_));	//��save_setting.WriteEE�е�����д��EEPROM��
    HAL_Delay(100);

}

/**********************************************************************************************************
*	�� �� ��: SettingRead()
*	����˵��: ��EEPROM�����ݶ�ȡ��Setting�����ò�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void SettingRead()
{
    EEPROM_Read(SETTING_ADDR,save_setting.WriteEE,sizeof(struct Set_));		//��EEPROM�е����ݶ���save_setting.WriteEE��
	
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
*	�� �� ��: SaveLastMem()
*	����˵��: ���ϴγ����д��EEPROM��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void SaveLastMem()
{
    uint8_t dat;
    dat	= mem.MemoryNum;
    EEPROM_Write(LAST_MEM,&dat,1);
}

/**********************************************************************************************************
*	�� �� ��: ReadLastMem()
*	����˵��: ��EEPROM�ж�ȡ�ϴγ����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void ReadLastMem()
{
    uint8_t dat;
    EEPROM_Read(LAST_MEM,&dat,1);
    mem.MemoryNum	=	dat;
}

///**********************************************************************************************************
//*	�� �� ��: ReadReName(uint8_t num)
//*	����˵��: ��EEPROM�ж�ȡﱺ��ļ���
//*	��    ��: numΪ�ļ�����ţ�Ŀǰ1~10
//*	�� �� ֵ: ��
//*	�� �� ��: 1������ʢ		
//*	�޶���¼: 1:
//*	�༭����: 1: 20210811 				         
//**********************************************************************************************************/
//void ReadSEQReName(uint8_t num)
//{
//	uint16_t EEaddr	=	0;
//	EEaddr	=	Seq_NAME_ADDR + num * 4;
//	EEPROM_Read( EEaddr, SEQ_Name[num],4);
//}

///**********************************************************************************************************
//*	�� �� ��: SaveReName(uint8_t num)
//*	����˵��: ��ﱺ��ļ���д��EEPROM��
//*	��    ��: numΪ�ļ�����ţ�Ŀǰ1~11
//*	�� �� ֵ: ��
//*	�� �� ��: 1������ʢ		
//*	�޶���¼: 1:
//*	�༭����: 1: 20210811 				         
//**********************************************************************************************************/
//void SaveSEQReName(uint8_t num)
//{
//	uint16_t EEaddr	=	0;
//	EEaddr	=	Seq_NAME_ADDR + num * 4;

//	EEPROM_Write(EEaddr,SEQ_Name[num],4);
//}

/**********************************************************************************************************
*	�� �� ��: ReadReName(uint8_t num)
*	����˵��: ��EEPROM�ж�ȡﱺ��ļ���
*	��    ��: numΪ�ļ�����ţ�Ŀǰ1~10
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void ReadStandyReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	STANDYRENAME_ADDR + (num-1) * 11;
	EEPROM_Read( EEaddr, ReStandyName[num-1],11);
}

/**********************************************************************************************************
*	�� �� ��: SaveReName(uint8_t num)
*	����˵��: ��ﱺ��ļ���д��EEPROM��
*	��    ��: numΪ�ļ�����ţ�Ŀǰ1~11
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void SaveStandyReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	STANDYRENAME_ADDR + (num-1) * 11;

	EEPROM_Write(EEaddr,ReStandyName[num-1],11);
}

/**********************************************************************************************************
*	�� �� ��: ReadReName(uint8_t num)
*	����˵��: ��EEPROM�ж�ȡﱺ��ļ���
*	��    ��: numΪ�ļ�����ţ�Ŀǰ1~10
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void ReadReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	RENAME_ADDR + num * 11;
	EEPROM_Read( EEaddr, ReName[num],11);
}

/**********************************************************************************************************
*	�� �� ��: SaveReName(uint8_t num)
*	����˵��: ��ﱺ��ļ���д��EEPROM��
*	��    ��: numΪ�ļ�����ţ�Ŀǰ1~11
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void SaveReName(uint8_t num)
{
	uint16_t EEaddr	=	0;
	EEaddr	=	RENAME_ADDR + num * 11;

	EEPROM_Write(EEaddr,ReName[num],11);
}

/**********************************************************************************************************
*	�� �� ��: InitReName()
*	����˵��: ��ﱺ��ļ�����ʼ������д��EEPROM��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
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
		EEPROM_Write(EEaddr,ReName[i],11);			//��str[i]д��EEPROM
	}
}

/**********************************************************************************************************
*	�� �� ��: Save1_Read0_Torque(uint8_t yn)
*	����˵��: У׼13��ת��ֵ����д��EEPROM��
*	��    ��: uint8_t yn��У׼ģʽ��yn = 1����ת��д�룬����ģʽ����
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/

void Save1_Read0_Torque(uint8_t yn)
{
	uint8_t Torque8[678];
	uint8_t i,k;
	
	uint16_t t16 = 0, h8, l8, j;
	if(yn == 1) 
	{
		j = 0;
		for(i = 0; i < 3; i++)												//Ť��У׼��0  ~ 1.0��Ť�أ�ȫ�ٶ�
		{
			for(k = 0; k < 32; k++)
			{
				t16 = S_T_I_COE1[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE1[i][k] & 0x00ff;			//��8λ
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE1[i][k] >> 8); 				//��8λ
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 3; i++)												//Ť��У׼��1.0~ 2.0��Ť�أ���ȥ1500rpm
		{
			for(k = 0; k < 29; k++)
			{
				t16 = S_T_I_COE2[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE2[i][k] & 0x00ff;			//��8λ
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE2[i][k] >> 8); 				//��8λ
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 6; i++)												//Ť��У׼��2.0~ 4.0��Ť�أ���ȥ700rpm����
		{
			for(k = 0; k < 21; k++)
			{
				t16 = S_T_I_COE3[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE3[i][k] & 0x00ff;			//��8λ
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE3[i][k] >> 8); 				//��8λ
				j++;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
		for(i = 0; i < 2; i++)												//Ť��У׼��	 5.0��Ť�أ���ȥ400rpm����
		{
			for(k = 0; k < 15; k++)
			{
				t16 = S_T_I_COE4[i][k];
				Torque8[j] = (uint8_t)S_T_I_COE4[i][k] & 0x00ff;			//��8λ
				j++;
				Torque8[j] = (uint8_t)(S_T_I_COE4[i][k] >> 8); 				//��8λ
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
		for(i = 0; i < 3; i++)										//Ť��У׼��0  ~ 1.0��Ť�أ�ȫ�ٶ�
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
		for(i = 0; i < 3; i++)										//Ť��У׼��1.0~ 2.0��Ť�أ���ȥ1500rpm
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
		for(i = 0; i < 6; i++)										//Ť��У׼��2.0~ 4.0��Ť�أ���ȥ700rpm����
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
		for(i = 0; i < 2; i++)										//Ť��У׼��	 5.0��Ť�أ���ȥ400rpm����
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
*	�� �� ��: Save1_Read0_NoloadTor(uint8_t yn)
*	����˵��: У׼����ת��ֵ����д��EEPROM��
*	��    ��: uint8_t yn��У׼ģʽ��yn = 1����ת��д�룬����ģʽ����
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
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
			NoLoadCurrent[i] += 128;								// �Ѹ���ת��Ϊ����������洢
            tor8[i]=(uint8_t)NoLoadCurrent[i];						// ��8λ
        }
        EEPROM_Write(NoLoadTOR_ADDR,tor8,64);
    }
    else if(yn==0)
    {
        EEPROM_Read(NoLoadTOR_ADDR,tor8,64);
        for(i=0; i<64; i++)
        {
            NoLoadCurrent[i] = tor8[i];
			NoLoadCurrent[i] -= 128;								// ��ԭ
        }
    }
}

/**********************************************************************************************************
*	�� �� ��: SaveVersions()
*	����˵��: ��������汾�ţ�д��EEPROM��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void SaveVersions()
{
    char Ver[9]= {0};
    uint8_t i=0;
    for(i=0; i<9; i++)
    {
        Ver[i] = Versions[i];		//���ַ�����ֵת�����޷�������
    }

    EEPROM_Write(VER_ADDR,Ver,9);
}

/**********************************************************************************************************
*	�� �� ��: ReadVersions()
*	����˵��: ��EEPROM�ж�ȡ����汾��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
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
*	�� �� ��: void SaveOutsideseq(mem_num)
*	����˵��: ���û��Զ���������浽EEPROM
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void SaveOutsideseq(int mem_num)
{
	uint8_t i;
	uint16_t EEaddr = 0;
    EEaddr = mem_num*37 + Seq_ADDR;													//�����ַ
	
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
	
	EEPROM_Write(EEaddr,save_file_seq.WriteEE,sizeof(struct saveseq_));				//��save_file_seq.WriteEE�е�����д��EEPROM��
	
    HAL_Delay(100);		
}

/**********************************************************************************************************
*	�� �� ��: void ReadOutsideseq(mem_num)
*	����˵��: ��EEPROM�ж�ȡ�û��Զ���ﱲ���
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void ReadOutsideseq(int mem_num)
{
	uint8_t i;
	uint16_t EEaddr = 0;
    EEaddr = mem_num*37 + Seq_ADDR;													//�����ַ
	
    EEPROM_Read(EEaddr,save_file_seq.WriteEE,sizeof(struct saveseq_));				//��EEPROM�е����ݶ���save.WriteEE��
	
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
*	�� �� ��: void ReadOutsideseq(mem_num)
*	����˵��: ��EEPROM�ж�ȡ�û��Զ�����ļ��е�����
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
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
*	�� �� ��: void SaveOutsideseq(mem_num)
*	����˵��: ���û��Զ�����ļ��е��������浽EEPROM
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void Write_File_number(uint8_t *number)
{
	EEPROM_Write(Filenumber_ADDR,number,1);
//	EEPROM_WriteOneByte(Filenumber_ADDR, number);
}
/**********************************************************************************************************
*	�� �� ��: void HardWareIDRead(void)
*	����˵��: ��ȡ��ĻID
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void HardWareIDRead(void)
{
	uint8_t scr_id_rdd;
	EEPROM_Read(SCR_EMC_PROTECT,&scr_id_rdd,1);
	sys.Disp_scr_id = scr_id_rdd;
}
/**********************************************************************************************************
*	�� �� ��: void HardWareIDRead(void)
*	����˵��: ����ĻID���浽EEPROM
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ		
*	�޶���¼: 1:
*	�༭����: 1: 20210811 				         
**********************************************************************************************************/
void HardWareIDWrite(uint8_t *number)
{
	EEPROM_Write(SCR_EMC_PROTECT,number,1);
}




