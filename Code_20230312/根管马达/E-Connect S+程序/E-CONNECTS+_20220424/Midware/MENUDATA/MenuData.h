#ifndef __MENUDATA_H
#define __MENUDATA_H

#include "main.h"
//#include "DataType.h"

#define 	ON 				1
#define 	OFF 			0
#define 	AUTO_START_POS 	0

/*�����ٶ� ָ�� ��Ӧ*/
#define 	s50				0
#define 	s60				1
#define 	s70				2
#define 	s80				3
#define 	s90				4
#define 	s100			5
#define 	s110			6
#define 	s120			7
#define 	s150 			8
#define 	s200 			9
#define 	s250 			10
#define 	s280 			11
#define 	s300 			12
#define 	s350 			13
#define 	s400 			14
#define 	s450 			15
#define 	s500 			16
#define 	s550 			17
#define 	s600 			18
#define 	s650 			19
#define 	s700 			20
#define 	s750 			21
#define 	s800 			22
#define 	s850 			23
#define 	s900 			24
#define 	s950 			25
#define 	s1000 			26
#define 	s1100 			27
#define 	s1200 			28
#define 	s1300 			29
#define 	s1400 			30
#define 	s1500 			31

/* �����ٶ� */
#define 	rs100 			0
#define 	rs200 			1
#define 	rs300 			2
#define 	rs400 			3
#define 	rs500 			4


/* ����Ť�� ָ�� ��Ӧ*/ //��Ť��ǿ���
#define 	t05 			0
#define 	t08 			1
#define 	t10 			2
#define 	t15 			3
#define 	t18 			4
#define 	t20 			5
#define 	t22 			6
#define 	t25 			7
#define 	t30 			8
#define 	t32 			9
#define 	t35 			10
#define 	t40 			11
#define 	t45 			12
#define 	t50 			13


/* ����Ƕ� ָ�� ��Ӧ*/
#define d30				0
#define d35				1
#define d40				2
#define d45				3
#define d50				4
#define d55				5
#define d60				6
#define d65				7
#define d70				8
#define d75				9
#define d80				10
#define d85				11
#define d90				12
#define d95				13
#define d100			14
#define d105			15
#define d110			16
#define d115			17
#define d120			18
#define d125			19
#define d130			20
#define d135			21
#define d140			22
#define d145			23
#define d150			24
#define d155			25
#define d160			26
#define d165			27
#define d170			28
#define d175			29
#define d180			30
#define d185			31
#define d190			32
#define d195			33
#define d200			34
#define d205			35
#define d210			36
#define d215			37
#define d220			38
#define d225			39
#define d230			40
#define d235			41
#define d240			42
#define d245			43
#define d250			44
#define d255			45
#define d260			46
#define d265			47
#define d270			48
#define d275			49
#define d280			50
#define d285			51
#define d290			52
#define d295			53
#define d300			54
#define d305			55
#define d310			56
#define d315			57
#define d320			58
#define d325			59
#define d330			60
#define d335			61
#define d340			62
#define d345			63
#define d350			64
#define d355			65
#define d360			66
#define d365			67
#define d370			68

/*************�������ɫ*******************/
#define 	c_white 		0  				//��ɫ
#define 	c_hong 			1  				//��ɫ
#define 	c_zong	 		2  				//��ɫ
#define 	c_huang 		3  				//��ɫ
#define 	c_lv 			4  				//��ɫ
#define 	c_fen 			5  				//��ɫ
#define 	c_lan 			6  				//��ɫ
#define 	c_zi	 		7  				//��ɫ
#define 	c_hei 			8  				//��ɫ
#define 	c_hui 			9  				//��ɫ



//ָ�롪���������ݡ���������ʾ
/**************����********************************/
extern uint8_t r_d_speed[12];
extern int d_ApexSense[3];
extern int d_Speed[32];
extern int r_Speed[12];
extern int r_Speed_limit[12];
extern int r_Speed_X[5];
extern int r_Speed_Y[5];
extern uint8_t rec2cwSpeed[10];
extern int d_Torque[14];
extern int d_StandyTime[15];
extern int d_PowerOffTime[14];
extern int d_AngleCW[69];
extern int d_AngleCCW[69];
extern int AngleCW_T[69];
extern int d_Beep[4];
extern int d_backlight[5];
extern int S_T_I_COE1[3][32];							//Ť��У׼��0  ~ 1.0��Ť�أ�ȫ�ٶ�
extern int S_T_I_COE2[3][29];							//Ť��У׼��1.0~ 2.0��Ť�أ���ȥ1300,1400,1500rpm
extern int S_T_I_COE3[6][21];							//Ť��У׼��2.0~ 4.0��Ť�أ���ȥ700rpm����
extern int S_T_I_COE4[2][15];							//Ť��У׼��	 5.0��Ť�أ���ȥ400rpm����

extern int NoLoadCurrent[64];
extern uint8_t CurentDevTorque;			//=cur/tor
extern const char* ver[];

/******************/
//ѡ��������,ָ��
typedef struct Sel_
{
	int taper;
	int number;
    int OperaMode;
    int OperaMode_len;
    int Speed;
    int Speed_len;
    int RecSpeed;
    int RecSpeed_len;
    int Torque;
    int Torque_len;
    int AtrTorque;
    int AtrTorque_len;
    int AngleCW;
    int AngleCW_len;
    int AngleCCW;
    int AngleCCW_len;
    int ApicalMode;
    int ApicalMode_len;
    int AutoStart;
    int AutoStart_len;
    int AutoStop;
    int AutoStop_len;
    int FlashBarPosition;
    int FlashBarPosition_len;
    int FileLibrary;
    int FileLibrary_len;
    int FileSeq;
    int FileSeq_len;		
	int color;
	int color_len;
	int FileSave;						//�洢λ��  =0��Ĭ�ϣ�=1���û��Զ���洢��=2����Ƭ���ڲ��洢
	int Vaule1;							//���ڼ�¼�����Ƿ�仯�����ٶ�Ť�ػ����Ƕȡ����Ƕ�
	int Vaule2;							//���ڼ�¼�����Ƿ�仯�����ٶ�Ť�ػ����Ƕȡ����Ƕ�

} FUN_SEL;

extern FUN_SEL sel;
/*************************/
//���ù������ݣ�ָ��
typedef struct Setfun_
{
    int AutoPowerOffTime;
    int AutoStandyTime;
    int BeepVol;
    int Hand;
    int ApexSense;
    int Language;
    int backlight;
    int Calibration;
    int RestoreFactorySettings;
	int EMC_SCR_PROTECT;
	int StartupMemory;

    int AutoPowerOffTime_len;
    int AutoStandyTime_len;
    int BeepVol_len;
    int Hand_len;
    int ApexSense_len;
    int Language_len;
    int backlight_len;
    int Calibration_len;
    int RestoreFactorySettings_len;
	int StartupMemory_len;
} FUN_SET;
extern FUN_SET set;
/***************************/
//�洢���������ָ��
typedef struct Mem_
{
    int MemoryNum;
    int MemoryNum_len;

} MEM_NUM;
extern MEM_NUM mem;

//ϵͳ״̬
typedef struct Sys_
{
	int BatteryLevel; 								//��ص�������
	int TorqueBar;									//Ť�ظ���
	int apex;										//�������

	uint8_t FlagThetaBias;							//�Ƕ�У��
	uint8_t FlagRunWithApex;						//�߿��߲��־
	uint8_t RunEALflag;								//���������б�־
	uint8_t mma8452_get_flag;						//�ƶ�����־
	uint8_t AutoSleep_cnt;							//�Զ���ֹ���߼���
	uint8_t InSleep;								//��ֹ����
	uint8_t Charging;								//���״̬��־
	uint8_t FullCharge;								//�������־λ
	uint8_t DispBattFlag;     						//֪ͨ�����ʾ��־
	uint8_t ActiveShutRunning;						//�ڱ����߲��������ر�����ģʽ���ص�����ģʽ
	uint8_t ApicalSlowing;							//������ٱ�־
	uint8_t InRoot;
	uint8_t InApicalOff;							//�ڸ�����ֹͣ��־
	uint8_t error;									//���������־
	uint8_t error_num;								//�������
	uint8_t ProgramMotorStop;						//�������ֹͣ��־
	uint8_t Incal;									//��ǰִ��У׼
	uint8_t over_apex_pos_flag;						//��������ο����־λ
	uint8_t reach_flag;
	uint8_t Disp_scr_id;
	uint8_t Read_Torque;
} SysState;
extern SysState sys;


#define 	Inside_file_num 			35  		//�ڲ�ﱸ���
/****************************�ڲ��������ã����ɸ���*******************************/
typedef struct Parameter_inside
{    
	char *seqName;									//ﱺ�
	uint8_t taper;									//׶��
	uint8_t number;									//����
	uint8_t OperaMode;								//ģʽ
    uint8_t speed;									//ת��
    uint8_t RecSpeed;								//ATC��ת��
    uint8_t torque;									//ת��
    uint8_t AngleCW;								//���Ƕ�
    uint8_t AngleCCW;								//���Ƕ�
	uint8_t color;									//��ɫ
} Parameter_inside;

extern Parameter_inside  fseq_inside_E_FLEX_EDGE[1];
extern Parameter_inside  fseq_inside_E_FLEX_GOLD[1];
extern Parameter_inside  fseq_inside_E_FLEX_BLUE[1];
extern Parameter_inside  fseq_inside_E_FLEX[1];
extern Parameter_inside  fseq_inside_E_FLEX_S[1];
extern Parameter_inside  fseq_inside_E_FLEX_REC[1];
extern Parameter_inside  fseq_inside_E_FLEX_ONE[1];
extern Parameter_inside  fseq_inside_E_FLEX_MINI[1];
extern Parameter_inside  fseq_inside_E_FLEX_RT[1];
extern Parameter_inside  fseq_inside_E_FLEX_OPEN[1];
extern Parameter_inside  fseq_inside_Protaper[6];
extern Parameter_inside  fseq_inside_Hyflex[3];
extern Parameter_inside  fseq_inside_Mtwo[14];
extern Parameter_inside  fseq_inside_FlexMaster[18];
extern Parameter_inside  fseq_inside_Reciproc[1];
extern Parameter_inside  fseq_inside_R_Pilot[1];
extern Parameter_inside  fseq_inside_K3XF[1];
extern Parameter_inside  fseq_inside_TF[1];
extern Parameter_inside  fseq_inside_GATES[3];
extern Parameter_inside  fseq_inside_PATHFILE[1];
extern Parameter_inside  fseq_inside_Pro_Glider[1];
extern Parameter_inside  fseq_inside_WaveOne[1];
extern Parameter_inside  fseq_inside_OneG[1];
extern Parameter_inside  fseq_inside_OneShape[1];
extern Parameter_inside  fseq_inside_OneFlare[1];
extern Parameter_inside  fseq_inside_OneCurve[1];
extern Parameter_inside  fseq_inside_One_RECI[1];
extern Parameter_inside  fseq_inside_2Shape[1];
extern Parameter_inside  fseq_inside_Xpendo_Shaper[1];
extern Parameter_inside  fseq_inside_Xpendo_Finisher[1];
extern Parameter_inside  fseq_inside_iRace[1];
extern Parameter_inside  fseq_inside_BT_Race[1];
extern Parameter_inside  fseq_inside_BioRace[1];
extern Parameter_inside  fseq_inside_Edge_platinum[1];


typedef struct Seq_parameter_inside
{
	const char *Name;
    uint8_t seq_Len;								//ﱸ���
	uint8_t seq_array;								//�����
	Parameter_inside *fseq;							//ﱲ���
} Seq_parameter_inside;

extern Seq_parameter_inside 	Seq_Inside[Inside_file_num];

#define 	Outside_file_num 			15  		//�ⲿ����Ӳ�������
/****************************�ⲿ�������ã��ɸ���*******************************/
typedef struct Parameter_outside
{
	int taper;										//׶��
	int number;										//����
	int OperaMode;									//ģʽ
    int speed;										//ת��
    int RecSpeed;									//ATC��ת��
    int torque;										//ת��
    int AngleCW;									//���Ƕ�
    int AngleCCW;									//���Ƕ�
	int color;										//��ɫ
} Parameter_outside;

typedef struct Seq_parameter_outside
{
	int seq_Len;									//ﱸ���
	int seq_new_Len;								//ﱸ���
	Parameter_inside fseq[8];						//ﱲ���
} Seq_parameter_outside;
extern Seq_parameter_outside 	New_file_parameter;

extern void MenuDataInit();


#endif









