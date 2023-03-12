#include "MenuDisp.h"
#include "MenuData.h"


/*****改动锉系统时，须改此处*******************/

const char*DispEnMem[]=
{
	{"M0"},
	{"M1"},
	{"M2"},
	{"M3"},
	{"M4"},
	{"M5"},
	{"M6"},
	{"M7"},
	{"M8"},
	{"M9"},
	{"M10"},
};

const char* DispENFileName[]=
{
    {"APEX"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
    {"Myfile"},
};
const char* DispCNFileName[]=
{
    {CN_GEN CN_CE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
    {CN_WO CN_DE CN_MO CN_SHI2},
};



const char* DispEnSpeedData[]=			//CW/CCW转速
{
	{"50"},						//0
	{"60"},						//1
	{"70"},						//2
	{"80"},						//3
	{"90"},						//4
	{"100"},					//5
	{"110"},					//6
	{"120"},					//7
	{"150"},					//8
	{"200"},					//9
	{"250"},					//10
	{"280"},					//11
	{"300"},					//12
	{"350"},					//13
	{"400"},					//14
	{"450"},					//15
	{"500"},					//16
	{"550"},					//17
	{"600"},					//18
	{"650"},					//19
	{"700"},					//20
	{"750"},					//21
	{"800"},					//22
	{"850"},					//23
	{"900"},					//24
	{"950"},					//25
	{"1000"},					//26
	{"1100"},					//27
	{"1200"},					//28
	{"1300"},					//29
	{"1400"},					//30
	{"1500"},					//31
};

const char* DispEnRecSpeedData[]=		//REC/ATR转速
{
	{"100"},
	{"200"},
	{"300"},
	{"400"},
	{"500"},
};
const char* DispCNRecSpeedData[]=		//REC/ATR转速
{
	{"Lv.1"},
	{"Lv.2"},
	{"Lv.3"},
	{"Lv.4"},
	{"Lv.5"},
};

// 					 	 0   1   2   3   4   5   6   7   8   9  10  11  12	13  14RL
int d_TqBlinkGrid[15]= {15,	24,	30,	45,	54,	60,	66,	75,	90,	96,105,120,135,150,162};						//与扭矩数量强相关

const char* DispEnTorqueData[]=		//CW/CCW扭矩
{
	{"0.5"},						// 0
	{"0.8"},						// 1
	{"1.0"},						// 2
	{"1.5"},						// 3
	{"1.8"},						// 4
	{"2.0"},						// 5
	{"2.2"},						// 6
	{"2.5"},						// 7
	{"3.0"},						// 8
	{"3.2"},						// 9
	{"3.5"},						// 10
	{"4.0"},						// 11
	{"4.5"},						// 12
	{"5.0"},						// 13
	{"R"CN_DIAN"L"},						// 14

};

const char* DispEnAtrTorqueData[]=			//ATR扭矩
{
	{"0.5"},
	{"0.8"},
	{"1.0"},
	{"1.5"},
	{"1.8"},
	{"2.0"},
	{"2.2"},
	{"2.5"},
	{"3.0"},

};
const char* DispEnAngleCWData[]=		//角度
{
	{" 30"},		//0
	{" 35"},		//1
	{" 40"},		//2
	{" 45"},		//3
	{" 50"},		//4
	{" 55"},		//5
	{" 60"},		//6
	{" 65"},		//7
	{" 70"},		//8
	{" 75"},		//9
	{" 80"},		//10
	{" 85"},		//11
	{" 90"},		//12
	{" 95"},		//13
	{"100"},		//14
	{"105"},		//15
	{"110"},		//16
	{"115"},		//17
	{"120"},		//18
	{"125"},		//19
	{"130"},		//20
	{"135"},		//21
	{"140"},		//22
	{"145"},		//23
	{"150"},		//24
	{"155"},		//25
	{"160"},		//26
	{"165"},		//27
	{"170"},		//28
	{"175"},		//29
	{"180"},		//30
	{"185"},		//31
	{"190"},		//32
	{"195"},		//33
	{"200"},		//34
	{"205"},		//35
	{"210"},		//36
	{"215"},		//37
	{"220"},		//38
	{"225"},		//39
	{"230"},		//40
	{"235"},		//41
	{"240"},		//42
	{"245"},		//43
	{"250"},		//44
	{"255"},		//45
	{"260"},		//46
	{"265"},		//47
	{"270"},		//48
	{"275"},		//49
	{"280"},		//50
	{"285"},		//51
	{"290"},		//52
	{"295"},		//53
	{"300"},		//54
	{"305"},		//55
	{"310"},		//56
	{"315"},		//57
	{"320"},		//58
	{"325"},		//59
	{"330"},		//60
	{"335"},		//61
	{"340"},		//62
	{"345"},		//63
	{"350"},		//64
	{"355"},		//65
	{"360"},		//66
	{"365"},		//67
	{"370"},		//68
};
const char* DispEnAngleCCWData[]=
{
	{" 30"},		//0
	{" 35"},		//1
	{" 40"},		//2
	{" 45"},		//3
	{" 50"},		//4
	{" 55"},		//5
	{" 60"},		//6
	{" 65"},		//7
	{" 70"},		//8
	{" 75"},		//9
	{" 80"},		//10
	{" 85"},		//11
	{" 90"},		//12
	{" 95"},		//13
	{"100"},		//14
	{"105"},		//15
	{"110"},		//16
	{"115"},		//17
	{"120"},		//18
	{"125"},		//19
	{"130"},		//20
	{"135"},		//21
	{"140"},		//22
	{"145"},		//23
	{"150"},		//24
	{"155"},		//25
	{"160"},		//26
	{"165"},		//27
	{"170"},		//28
	{"175"},		//29
	{"180"},		//30
	{"185"},		//31
	{"190"},		//32
	{"195"},		//33
	{"200"},		//34
	{"205"},		//35
	{"210"},		//36
	{"215"},		//37
	{"220"},		//38
	{"225"},		//39
	{"230"},		//40
	{"235"},		//41
	{"240"},		//42
	{"245"},		//43
	{"250"},		//44
	{"255"},		//45
	{"260"},		//46
	{"265"},		//47
	{"270"},		//48
	{"275"},		//49
	{"280"},		//50
	{"285"},		//51
	{"290"},		//52
	{"295"},		//53
	{"300"},		//54
	{"305"},		//55
	{"310"},		//56
	{"315"},		//57
	{"320"},		//58
	{"325"},		//59
	{"330"},		//60
	{"335"},		//61
	{"340"},		//62
	{"345"},		//63
	{"350"},		//64
	{"355"},		//65
	{"360"},		//66
	{"365"},		//67
	{"370"},		//68
};

const char* DispENSelectTitle[]=
{
    {"Operation Mode"},
    {"Speed Set"},
    {"Speed Level"},
    {"Torque Limit"},
    {"FWD Angle"},
    {"REV Angle"},
	{"Auto Start"},   //20210513  增加5个字段  //6
    {"Auto Stop"},    //7
    {"Apical Action"}, //8
	{"Reference Point"},  //9
};

const char* DispCNSelectTitle[]=
{
	{CN_CAO CN_ZUO1 CN_MO CN_SHI2},
	(CN_ZHUAN CN_SU CN_SHE CN_ZHI),
	(CN_WANG CN_FU CN_SU CN_LV),
	{CN_NIU CN_JU CN_SHE CN_ZHI},
	{CN_ZHENG CN_ZHUAN CN_JIAO CN_DU1},
	{CN_FAN1 CN_ZHUAN CN_JIAO CN_DU1},
	
	{CN_ZI CN_DONG CN_QI CN_DONG},   //20210526
	{CN_ZI CN_DONG CN_TING CN_ZHI1},
	{CN_GEN CN_JIAN4 CN_DONG CN_ZUO1},
	{CN_GEN CN_JIAN4 CN_CAN CN_KAO CN_WEI CN_ZHI},	
};

const char* DispENNEWSelectTitle[] = 
{
	{"File name"},
    {"Seq number"},
    {"Taper"},
    {"Size"},
    {"Operation Mode"},
    {"Speed Set"},
	{"Torque Limit"},   //20210513  增加5个字段  //6
    {"FWD Angle"},    //7
    {"REV Angle"}, //8
	{"Taper Indicator"},  //9
};

const char* DispCNNEWSelectTitle[] = 
{
	{CN_CUO1 CN_MING1 CN_SHE CN_ZHI},
    {CN_CUO1 },
    {CN_ZHUI CN_DU1},
    {CN_CUO1 CN_HAO},
    {CN_CAO CN_ZUO1 CN_MO CN_SHI2},
    {CN_SU CN_DU1 CN_SHE CN_ZHI},
	{CN_NIU CN_JU CN_SHE CN_ZHI},   //20210513  增加5个字段  //6
    {CN_ZHENG CN_ZHUAN CN_JIAO CN_DU1},    //7
    {CN_FAN1 CN_ZHUAN CN_JIAO CN_DU1}, //8
	{CN_SE CN_HUAN},  //9
};


const char* DispEnTitleFwdRev[]=
{
	{"Operation Mode"},
	{"Speed"},
	{"Torque Limit"},
	{"FWD Angle"},
	{"REV Angle"},
	{"Apical Action"},
	{"Auto Start"},
	{"Auto Stop"},
	{"Reference point"},
};

const char* DispEnTitleReciprocate[]=
{
	{"Operation Mode"},
	{"Speed"},
	{"Torque Limit"},
	{"Angle"},
	{"Apical Action"},
	{"Auto Start"},
	{"Auto Stop"},
	{"Reference point"},
};
const char* DispEnTitleEMR[]=
{
	{"Operation Mode"},
	{"Reference point"},
};
/****************************************/
//子选项显示，英文  //模式、位置 与 DispMenuOperationModeDown()  DispMenuOperationModeUp() 函数强相关
//const char* DispEnSelOperationMode[]=
//{
//	{"Fwd"},			//0
//	{"Rev"},			//1
//	{"REC"},	    	//2
//	{"ATC"},			//3
//	{"EAL"},			//4
//};
const char* DispEnSelOperationMode[]=
{
	{FWD},			//0
	{REV},			//1
	{REC},	    	//2
	{ATC},			//3
	{"EAL"},		//4
};

const char* DispCNSelOperationMode[]=
{
    {CN_ZHENG CN_ZHUAN},			  //0
    {CN_FAN1 CN_ZHUAN},			//1
    {CN_WANG CN_FU},	    //2
    {CN_ZI CN_SHI5 CN_YING},			//3
    {"EAL"},			//4
};

const char* DispEnSelRpm[]=
{
	{"RPM"},

};

const char* DispEnSelNcm[]=
{
	{"N*cm"},
};

const char* DispEnSelDeg[]=
{
	{"deg"},
	{"/"}, 			 	//   °/
	{"'"}, 				//   °
	{"|"}, 		 		//    /
};
const char* DispEnSelApical[]=
{
	{"Reverse"},
	{"SlowDown"},
	{"Stop"},
	{"OFF"},
};

const char* DispCNSelApical[]=   //20210528
{
    {CN_FAN1 CN_ZHUAN},
    {CN_JIAN5 CN_SU},
    {CN_TING CN_ZHI1},
    {CN_GUAN CN_BI},
};

const char* DispEnSelAuto[]=
{
	{"OFF"},
	{"ON"},
};


/***************************************/
//设置功能显示标题，英文

const char* DispEnSetTitle[]=
{
    {"Versions"},											// 0 本机版本
    {"Auto Power Off"},										// 1 自动关机时间
    {"Auto Return"},										// 2 自动返回时间
    {"Beeper Volume"},										// 3 音量设置
	{"Back light"},											// 4 背光设置
    {"Habit Hand"},											// 5 左右手设置
    {"ApexSensitivity"},									// 6 根测灵敏度
    {"Language"},											// 7 语言设置
    {"Calibration"},										// 8 自动校准
    {"Restore Settings"},									// 9 恢复出厂设置
	{"Startup Memory"},										// 10启动模式
};

const char* DispCNSetTitle[]=
{
	{CN_BEN CN_JI1 CN_BAN CN_BEN},							// 0 本机版本
    {CN_ZI CN_DONG CN_GUAN CN_JI1 CN_SHI3 CN_JIAN2},		// 1 自动关机时间
    {CN_ZI CN_DONG CN_FAN2 CN_HUI1 CN_SHI3 CN_JIAN2},		// 2 自动返回时间
    {CN_YIN CN_LIANG CN_SHE CN_ZHI},						// 3 音量设置
	{CN_BEI CN_GUANG CN_LIANG1 CN_DU1 },					// 4 背光设置
    {CN_ZUO CN_YOU CN_SHOU CN_SHE CN_ZHI},					// 5 左右手设置
    {CN_GEN CN_CE CN_LING CN_MIN CN_DU1},					// 6 根测灵敏度
    {CN_YU CN_YAN CN_SHE CN_ZHI},							// 7 语言设置
    {CN_ZI CN_DONG CN_XIAO CN_ZHUN},						// 8 自动校准
    {CN_HUI2 CN_FU CN_CHU1 CN_CHANG CN_SHE CN_ZHI},			// 9 恢复出厂设置
	{CN_QI CN_DONG CN_MO CN_SHI2},							// 10启动模式
};

/*****************************/
//设置功能的子选项，英文
const char* DispEnSetAPOtime[]=
{
	{"3"},
	{"4"},
	{"5"},
	{"6"},
	{"7"},
	{"8"},
	{"9"},
	{"10"},
	{"11"},
	{"12"},
	{"13"},
	{"14"},
	{"15"},
};
const char* DispEnSetAPO[]=
{
  {"Min"},
};
const char* DispEnSetAstTime[]=
{
	{"3"},
	{"4"},
	{"5"},
	{"6"},
	{"7"},
	{"8"},
	{"9"},
	{"10"},
	{"11"},
	{"12"},
	{"13"},
	{"14"},
	{"15"},
	{"30"},
	{"60"},
};
const char* DispEnSetAst[]=
{
    {"Sec"},
};
const char* DispEnSetVolLevel[]=
{
    {"0"},
    {"1"},
    {"2"},
    {"3"},
//{"4"},
//{"5"},
};
const char* DispEnSetbl[]=
{
    {"1"},
    {"2"},
    {"3"},
    {"4"},
    {"5"},
};
const char* DispEnSetVol[]=
{
    {"Vol."},
};
const char* DispEnSethand[]=
{
    {"Right Hand"},
    {"Left Hand"},
};

const char* DispCNSethand[]=
{
    {CN_YOU CN_SHOU},
    {CN_ZUO CN_SHOU},
};

const char* DispEnSetMem[]=
{
    {"M1"},
    {"Last"},
};

const char* DispCNSetMem[]=
{
    {"M1"},
    {CN_SHANG CN_CI},
};

const char* DispEnSetApexSen[]=
{
	{"Low"},
    {"Mid"},
	{"High"},
};

const char* DispCNSetApexSen[]=
{
	{CN_DI},
    {CN_ZHONG},
	{CN_GAO},
};

const char* DispEnSetLan[]=
{
    {"English"},
	{CN_JIAN1 CN_TI CN_ZHONG CN_WEN},
};

/* char* DispEnSetVer[]=			//
{
{"E.1.0.121"},

};  */
const char* DispEnSetCal[]=
{
    {"OFF"},
    {"ON"},
};


const char* DispCN_ON_OFF[]=
{
    {CN_GUAN CN_BI},
	{CN_KAI CN_QI},
};

const char* DispEnSetRfs[]=
{
    {"OFF"},
    {"ON"},
};

const char* DispEnFacCalTitle[]=
{
    {"Factory Cal"}
};
const char* DispEnFacCal[]=
{
    {"Speed"},
    {"Torque"},
};


const char* DispPassword[]=
{
	{CN_DIAN},
	{"S"},
	{">"},
	{"<"},
};
/**************************************************************
  *Function:   显示内容的初始化，计算数组元素个数,以便循环显示
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void MenuDispInit()
{
    sel.OperaMode_len				=	sizeof(DispEnSelOperationMode) / sizeof(DispEnSelOperationMode[0]) - 1;
    sel.Speed_len					=	sizeof(DispEnSpeedData)		   / sizeof(DispEnSpeedData[0])		   - 1;
    sel.RecSpeed_len				=	sizeof(DispEnRecSpeedData)	   / sizeof(DispEnRecSpeedData[0])     - 1;
    sel.Torque_len					=	sizeof(DispEnTorqueData)	   / sizeof(DispEnTorqueData[0])       - 1;
    sel.AtrTorque_len				=	sizeof(DispEnAtrTorqueData)	   / sizeof(DispEnAtrTorqueData[0])    - 1;
    sel.AngleCW_len					=	sizeof(DispEnAngleCWData)	   / sizeof(DispEnAngleCWData[0])      - 1;
    sel.AngleCCW_len				=	sizeof(DispEnAngleCCWData)	   / sizeof(DispEnAngleCCWData[0])     - 1;
    sel.ApicalMode_len				=	sizeof(DispEnSelApical)		   / sizeof(DispEnSelApical[0])        - 1;
    sel.AutoStart_len				=	sizeof(DispEnSelAuto)		   / sizeof(DispEnSelAuto[0])          - 1;
    sel.AutoStop_len				=	sizeof(DispEnSelAuto)		   / sizeof(DispEnSelAuto[0])          - 1;

    set.AutoPowerOffTime_len		=	sizeof(DispEnSetAPOtime)	   / sizeof(DispEnSetAPOtime[0])       - 1;
    set.AutoStandyTime_len			=	sizeof(DispEnSetAstTime)	   / sizeof(DispEnSetAstTime[0])-1;
    set.BeepVol_len					=	sizeof(DispEnSetVolLevel)	   / sizeof(DispEnSetVolLevel[0])-1;
    set.Hand_len					=	sizeof(DispEnSethand)		   / sizeof(DispEnSethand[0])-1;
    set.Language_len				=	sizeof(DispEnSetLan)		   / sizeof(DispEnSetLan[0])-1;
    set.RestoreFactorySettings_len	=	sizeof(DispEnSetRfs)		   / sizeof(DispEnSetRfs[0])-1;
    set.ApexSense_len				=	sizeof(DispEnSetApexSen)	   / sizeof(DispEnSetApexSen[0])-1;
    set.Calibration_len				=	sizeof(DispEnSetCal)		   / sizeof(DispEnSetCal[0])-1;
  	set.backlight_len				=	sizeof(DispEnSetbl)			   / sizeof(DispEnSetbl[0])-1;
    mem.MemoryNum_len				=	sizeof(DispEnMem)			   / sizeof(DispEnMem[0])-1;
	set.StartupMemory_len			=	sizeof(DispEnSetMem)/sizeof(DispEnSetMem[0])-1;
}









