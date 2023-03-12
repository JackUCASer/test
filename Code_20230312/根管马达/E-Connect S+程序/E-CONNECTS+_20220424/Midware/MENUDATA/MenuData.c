#include "MenuData.h"
#include "MenuDisp.h"

FUN_SEL sel;
FUN_SET set;
MEM_NUM mem;
SysState sys;

int pageflash = 1;
//**********************//

//电机正转、反转、ATR转速设置
int d_Speed[32]= 
	{
		50,				// 0
		60,				// 1
		70,				// 2
		80,				// 3
		90,				// 4
		100,			// 5
		110,			// 6		
		120,			// 7	
		150,			// 8	
		200,			// 9	
		250,			// 10	
		280,			// 11	
		300,			// 12	
		350,			// 13	
		400,			// 14	
		450,			// 15	
		500,			// 16	
		550,			// 17	
		600,			// 18	
		650,			// 19	
		700,			// 20	
		750,			// 21	
		800,			// 22	
		850,			// 23	
		900,			// 24	
		950,			// 25	
		1000,			// 26	
		1100,			// 27
		1200,			// 28
		1300,			// 29
		1400,			// 30
		1500,			// 31	
	};

//电机往复模式转速设置	
int r_Speed[12]= 
	{
		150,			// 0			
		200,			// 1
		300,			// 2
		400,			// 3
		500,			// 4
		500,			// 5
		500,			// 6
		500,			// 7
		500,			// 8
		500,			// 9
		500,			// 10
		500				// 11
	};					//人为增加了两个元素，防止越界
	//电机往复模式转速设置	
int r_Speed_limit[12]= 
	{
		63,			// 0			
		137,			// 1
		205,			// 2
		273,			// 3
		341,			// 4
		500,			// 5
		500,			// 6
		500,			// 7
		500,			// 8
		500,			// 9
		500,			// 10
		500				// 11
	};	
	
int r_Speed_X[5]= 
	{
		10,			// 0			
		8,			// 1
		5,			// 2
		4,			// 3
		3,			// 4
	};	
	
int r_Speed_Y[5]= 
	{
		100,			// 0			
		121,			// 1
		169,			// 2
		211,			// 3
		248,			// 4
	};
uint8_t r_d_speed[12]=
	{
		s100,			// 0			
		s200,			// 1
		s300,			// 2
		s400,			// 3
		s500,			// 4
		s500,			// 5
		s500,			// 6
		s500,			// 7
		s500,			// 8
		s500,			// 9
		s500,			// 10
		s500				// 11
	};
uint8_t rec2cwSpeed[10]= 
	{
		1,
		2,
		3,
		5,
		6,
		7,
		8,
		9,
		10,
		11
	};					//指针转换，在扭矩条处显示转速时用
// 					 0   1   2   3   4   5   6   7   8   9  10  11  12	13 
int d_Torque[14] = { 5,  8, 10, 15, 18, 20, 22, 25, 30, 32, 35, 40, 45, 50};

int d_StandyTime[15] = {300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,3000,6000};
int d_PowerOffTime[14] = {3,4,5,6,7,8,9,10,11,12,13,14,15};
int d_AngleCW[69] = {
						30,				//0
						35,				//1
						40,				//2
						45,				//3
						50,				//4
						55,				//5
						60,				//6
						65,				//7
						70,				//8
						75,				//9
						80,				//10
						85,				//11
						90,				//12
						95,				//13
						100,			//14
						105,			//15
						110,			//16
						115,			//17
						120,			//18
						125,			//19
						130,			//20
						135,			//21
						140,			//22
						145,			//23
						150,			//24
						155,			//25
						160,			//26
						165,			//27
						170,			//28
						175,			//29
						180,			//30
						185,			//31
						190,			//32
						195,			//33
						200,			//34
						205,			//35
						210,			//36
						215,			//37
						220,			//38
						225,			//39
						230,			//40
						235,			//41
						240,			//42
						245,			//43
						250,			//44
						255,			//45
						260,			//46
						265,			//47
						270,			//48
						275,			//49
						280,			//50
						285,			//51
						290,			//52
						295,			//53
						300,			//54
						305,			//55
						310,			//56
						315,			//57
						320,			//58
						325,			//59
						330,			//60
						335,			//61
						340,			//62
						345,			//63
						350,			//64
						355,			//65
						360,			//66
						365,			//67
						370				//68
					};
int AngleCW_T[69] = {
						0,				//0
						0,				//1
						0,				//2
						0,				//3
						0,				//4
						0,				//5
						20,				//6
						21,				//7
						22,				//8
						23,				//9
						24,				//10
						25,				//11
						26,				//12
						27,				//13
						28,			//14
						29,			//15
						30,			//16
						31,			//17
						32,			//18
						32,			//19
						32,			//20
						32,			//21
						33,			//22
						33,			//23
						33,			//24
						34,			//25
						34,			//26
						34,			//27
						35,			//28
						35,			//29
						35,			//30
						35,			//31
						35,			//32
						36,			//33
						36,			//34
						36,			//35
						36,			//36
						36,			//37
						36,			//38
						36,			//39
						36,			//40
						37,			//41
						37,			//42
						37,			//43
						37,			//44
						37,			//45
						37,			//46
						38,			//47
						38,			//48
						38,			//49
						39,			//50
						39,			//51
						39,			//52
						39,			//53
						40,			//54
						40,			//55
						40,			//56
						40,			//57
						41,			//58
						41,			//59
						41,			//60
						41,			//61
						41,			//62
						41,			//63
						41,			//64
						42,			//65
						42,			//66
						42,			//67
						42				//68
					};
int d_AngleCCW[69] = {
						-30,			//0
						-35,			//1
						-40,			//2
						-45,			//3
						-50,			//4
						-55,			//5
						-60,			//6
						-65,			//7
						-70,			//8
						-75,			//9
						-80,			//10
						-85,			//11
						-90,			//12
						-95,			//13
						-100,			//14
						-105,			//15
						-110,			//16
						-115,			//17
						-120,			//18
						-125,			//19
						-130,			//20
						-135,			//21
						-140,			//22
						-145,			//23
						-150,			//24
						-155,			//25
						-160,			//26
						-165,			//27
						-170,			//28
						-175,			//29
						-180,			//30
						-185,			//31
						-190,			//32
						-195,			//33
						-200,			//34
						-205,			//35
						-210,			//36
						-215,			//37
						-220,			//38
						-225,			//39
						-230,			//40
						-235,			//41
						-240,			//42
						-245,			//43
						-250,			//44
						-255,			//45
						-260,			//46
						-265,			//47
						-270,			//48
						-275,			//49
						-280,			//50
						-285,			//51
						-290,			//52
						-295,			//53
						-300,			//54
						-305,			//55
						-310,			//56
						-315,			//57
						-320,			//58
						-325,			//59
						-330,			//60
						-335,			//61
						-340,			//62
						-345,			//63
						-350,			//64
						-355,			//65
						-360,			//66
						-365,			//67
						-370			//68
					};
//int d_AngleCCW[69] = {-30,-40,-50,-60,-70,-80,-90,-100,-120,-150,-160,-180,-200,-210,-230,-250,-260,-280,-300,-320,-340,-360,-370};
int d_Beep[4] = {0,20,62,125};
int d_backlight[5] = {180,360,540,720,900};

int d_ApexSense[3] = {3,2,1};

int S_T_I_COE1[3][32];							//扭矩校准，0  ~ 1.0的扭矩，全速度
int S_T_I_COE2[3][29];							//扭矩校准，1.0~ 2.0的扭矩，除去1500rpm
int S_T_I_COE3[6][21];							//扭矩校准，2.0~ 4.0的扭矩，除去700rpm往后
int S_T_I_COE4[2][15];							//扭矩校准，	 5.0的扭矩，除去400rpm往后

int NoLoadCurrent[64]= 
	{
		5,
		7,
		8,
		8,
		10,
		10,
		11,
		13,
		14,
		15,
		16,
		17,
		17,
		18,
		19,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
		20,
	};
//120 150 200 250 280 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000
// 增加或减少扭矩时，须改对应的EEPROM存储参数

										//seq_Len  	seq_array	
Seq_parameter_inside Seq_Inside[Inside_file_num] = { 	{	"Myfile",			0,			0,			0},						//0   Protaper
														{	"E-FLEX EDGE",		1,			0,	fseq_inside_E_FLEX_EDGE},		//1   E-FLEX EDGE
														{	"E-FLEX GOLD",		1,			0,	fseq_inside_E_FLEX_GOLD},		//2   E-FLEX GOLD
														{	"E-FLEX BLUE",		1,			0,	fseq_inside_E_FLEX_BLUE},		//3   E-FLEX BLUE
														{	"E-FLEX",			1,			0,	fseq_inside_E_FLEX},			//4   E-FLEX
														{	"E-FLEX S",			1,			0,	fseq_inside_E_FLEX_S},			//5   E-FLEX S
														{	"E-FLEX REC",		1,			0,	fseq_inside_E_FLEX_REC},		//6   E-FLEX REC
														{	"E-FLEX ONE",		1,			0,	fseq_inside_E_FLEX_ONE},		//7   E-FLEX ONE
														{	"E-FLEX MINI",		1,			0,	fseq_inside_E_FLEX_MINI},		//8   E-FLEX MINI
														{	"E-FLEX RT",		1,			0,	fseq_inside_E_FLEX_RT},			//9   E-FLEX RT
														{	"E-FLEX OPEN",		1,			0,	fseq_inside_E_FLEX_OPEN},		//10  E-FLEX OPEN
														{	"Protaper",			6,			0,	fseq_inside_Protaper},			//11  Protaper
														{	"Hyflex",			3,			0,	fseq_inside_Hyflex},			//12  Hyflex	
														{	"Mtwo",				14,			0,	fseq_inside_Mtwo},				//13  Mtwo	
														{	"Flex.Master",		18,			0,	fseq_inside_FlexMaster},		//14  FlexMaster
														{	"Reciproc",			1,			0,	fseq_inside_Reciproc},			//15  Reciproc
														{	"R-Pilot",			1,			0,	fseq_inside_R_Pilot},			//16  R-Pilot
														{	"K3XF",				1,			0,	fseq_inside_K3XF},				//17  K3XF
														{	"TF",				1,			0,	fseq_inside_TF},				//18  TF
														{	"GATES",			3,			0,	fseq_inside_GATES},				//19  GATES
														{	"PATHFILE",			1,			0,	fseq_inside_PATHFILE},			//20  PATHFILE
														{	"Pro.Glider",		1,			0,	fseq_inside_Pro_Glider},		//21  Pro.Glider
														{	"WaveOne",			1,			0,	fseq_inside_WaveOne},			//22  WaveOne
														{	"OneG",				1,			0,	fseq_inside_OneG},				//23  OneG
														{	"OneShape",			1,			0,	fseq_inside_OneShape},			//24  OneShape
														{	"OneFlare",			1,			0,	fseq_inside_OneFlare},			//25  OneFlare
														{	"OneCurve",			1,			0,	fseq_inside_OneCurve},			//26  OneCurve
														{	"One RECI",			1,			0,	fseq_inside_One_RECI},			//27  One RECI
														{	"2Shape",			1,			0,	fseq_inside_2Shape},			//28  2Shape
														{	"Xpendo.Shaper",	1,			0,	fseq_inside_Xpendo_Shaper},		//29  Xpendo.Shaper
														{	"Xpendo,Finisher",	1,			0,	fseq_inside_Xpendo_Finisher},	//30  Xpendo,Finisher
														{	"iRace",			1,			0,	fseq_inside_iRace},				//31  iRace
														{	"BT-Race",			1,			0,	fseq_inside_BT_Race},			//32  BT-Race
														{	"BioRace",			1,			0,	fseq_inside_BioRace},			//33  BioRace
														{	"Edge.platinum",	1,			0,	fseq_inside_Edge_platinum},		//34  Edge.platinum
													};													

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_EDGE[1] =	{{	"TY",	 0,	   	0,		  0, s400,		  0,	  7,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_GOLD[1] =	{{	"TY",	 0,	   	0,		  0, s350,		  0,	  7,	   0,		 0,		c_white}};

Parameter_inside  fseq_inside_E_FLEX_BLUE[1] =	{{	"TY",	 0,	   	0,		  0, s350,		  0,	  7,	   0,		 0,		c_white}};

Parameter_inside  fseq_inside_E_FLEX[1] =		{{	"TY",	 0,	   	0,		  0, s350,		  0,	  7,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_S[1] =		{{	"TY",	 0,	   	0,		  0, s350,		  0,	  5,	   0,		 0,		c_white}};												 
												 
												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_REC[1] =	{{	"TY",	 0,	   	0,		  2,	0,		  0,	  0,	 d30,	  d150,		c_white}};												 
												 
												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_ONE[1] =	{{	"TY",	 0,	   	0,		  0, s400,		  0,	  7,	   0,		 0,		c_white}};													 

												//seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_MINI[1] =	{{	"TY",	 0,	   	0,		  0, s350,		  0,	  7,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_RT[1] =	{{	"TY",	 0,	   	0,		  0, s350,		  0,	  5,	   0,		 0,		c_white}};	

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_E_FLEX_OPEN[1] =	{{		0,	 8,	   	17,		  0, s300,		  0,	  8,	   0,		 0,		c_white}};	
													
												//seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Protaper[6] = 	{{	"SX",	 0,	    0,		  0, s300,		  0,	  7,	   0,		 0,		c_white},
												 {	"S1",	 0,	    0,		  0, s300,		  0,	  7,	   0,		 0,		c_lan},
												 {	"S2",	 0,	    0,		  0, s300,		  0,	  3,	   0,		 0,		c_hong},
												 {	"F1",	 0,	    0,		  0, s300,		  0,	  3,	   0,		 0,		c_huang},
												 {	"F2",	 0,	    0,		  0, s300,		  0,	  7,	   0,		 0,		c_white},
												 {	"F3",	 0,	    0,		  0, s300,		  0,	  7,	   0,		 0,		c_zi}};

												//seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Hyflex[3] = 		{{	"CM",	 0,	    0,		  0, s500,		  0,	  7,	   0,		 0,		c_white},
												 {	"EDM",	 0,	    0,		  0, s500,		  0,	  7,	   0,		 0,		c_white},
												 {	"NT",	 0,	    0,		  0, s300,		  0,	  7,	   0,		 0,		c_white}};

												//seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Mtwo[14]	=		{{		0,	 4,	   10,		  0, s280,		  0,	  2,	   0,		 0,		c_zi},
												 {		0,	 5,	   15,		  0, s280,		  0,	  3,	   0,		 0,		c_white},
												 {		0,	 6,	   20,		  0, s280,		  0,	  5,	   0,		 0,		c_huang},
												 {		0,	 6,	   25,		  0, s280,		  0,	  6,	   0,		 0,		c_hong},
												 {		0,	 5,	   30,		  0, s280,		  0,	  2,	   0,		 0,		c_lan},
												 {		0,	 4,	   35,		  0, s280,		  0,	  2,	   0,		 0,		c_lv},
												 {		0,	 4,	   40,		  0, s280,		  0,	  3,	   0,		 0,		c_hei},
												 {		0,	 4,	   45,		  0, s280,		  0,	  3,	   0,		 0,		c_white},
												 {		0,	 4,	   50,		  0, s280,		  0,	  5,	   0,		 0,		c_huang},
												 {		0,	 4,	   60,		  0, s280,		  0,	  8,	   0,		 0,		c_lan},
												 {		0,	 7,	   25,		  0, s280,		  0,	  5,	   0,		 0,		c_hong},
												 {		0,	 6,	   30,		  0, s280,		  0,	  2,	   0,		 0,		c_lan},
												 {		0,	 6,	   35,		  0, s280,		  0,	  2,	   0,		 0,		c_lv},
												 {		0,	 6,	   40,		  0, s280,		  0,	  4,	   0,		 0,		c_hei}};

												//seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_FlexMaster[18]=	{{		0,	 2,	   25,		  0, s280,		  0,	  0,	   0,		 0,		c_hong},
												 {		0,	 2,	   30,		  0, s280,		  0,	  0,	   0,		 0,		c_lan},
												 {		0,	 2,	   35,		  0, s280,		  0,	  1,	   0,		 0,		c_lv},
												 {		0,	 2,	   40,		  0, s280,		  0,	  2,	   0,		 0,		c_hei},
												 {		0,	 2,	   45,		  0, s280,		  0,	  3,	   0,		 0,		c_white},
												 {		0,	 2,	   50,		  0, s280,		  0,	  5,	   0,		 0,		c_huang},
												 {		0,	 2,	   60,		  0, s280,		  0,	  5,	   0,		 0,		c_lan},
												 {		0,	 2,	   70,		  0, s280,		  0,	 11,	   0,		 0,		c_lv},
												 {		0,	 4,	   25,		  0, s280,		  0,	  0,	   0,		 0,		c_hong},
												 {		0,	 4,	   30,		  0, s280,		  0,	  2,	   0,		 0,		c_lan},
												 {		0,	 4,	   35,		  0, s280,		  0,	  2,	   0,		 0,		c_lv},
												 {		0,	 4,	   40,		  0, s280,		  0,	  3,	   0,		 0,		c_hei},
												 {		0,	 6,	   15,		  0, s280,		  0,	  0,	   0,		 0,		c_white},
												 {		0,	 6,	   20,		  0, s280,		  0,	  0,	   0,		 0,		c_huang},
												 {		0,	 6,	   25,		  0, s280,		  0,	  3,	   0,		 0,		c_hong},
												 {		0,	 6,	   30,		  0, s280,		  0,	  3,	   0,		 0,		c_lan},
												 {		0,	 6,	   35,		  0, s280,		  0,	  4,	   0,		 0,		c_lv},
												 {		0,	 6,	   40,		  0, s280,		  0,	  5,	   0,		 0,		c_hei}};	
							
												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Reciproc[1] =		{{	"TY",	 0,	   	0,		  2,	0,		  0,	  0,	 d30,	  d150,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_R_Pilot[1] =		{{		0,	 4,	  	12,		  2,	0,		  0,	  0,	 d30,	  d150,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_K3XF[1] =			{{	"TY",	 0,	   	0,		  0, s400,		  0,	  5,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_TF[1] =			{{	"TY",	 0,	   	0,		  0, s600,		  0,	  11,	   0,		 0,		c_white}};

												//seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_GATES[3]=			{{	"#1",	 0,	    0,		  0, s800,		  0,	  2,	   0,		 0,		c_white},
												 {	"#2",	 0,	    0,		  0, s800,		  0,	  3,	   0,		 0,		c_white},
												 {	"#3-6",	 0,	    0,		  0, s800,		  0,	  8,	   0,		 0,		c_white}};
										
												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_PATHFILE[1] =		{{	"TY",	 0,	   	0,		  0, s300,		  0,	  2,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Pro_Glider[1] =	{{		0,	 2,	   	16,		  0, s300,		  0,	  11,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_WaveOne[1] =		{{	"TY",	 0,	   	0,		  2,	0,		  0,	  0,	 d30,	  d150,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_OneG[1] =			{{		0,	 3,	   	14,		  0, s300,		  0,	  2,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_OneShape[1] =		{{		0,	 6,	   	25,		  0, s300,		  0,	  7,	   0,		 0,		c_hong}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_OneFlare[1] =		{{		0,	 9,	   	25,		  0, s300,		  0,	  8,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_OneCurve[1] =		{{		0,	 6,	   	25,		  0, s300,		  0,	  7,	   0,		 0,		c_hong}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_One_RECI[1] =		{{	"TY",	 0,	   	0,		  2,	0,		  0,	  0,	 d60,	  d180,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_2Shape[1] =		{{	"TY",	 0,	   	0,		  0, s350,		  0,	  5,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Xpendo_Shaper[1] ={{	"TY",	 0,	   	0,		  0, s900,		  0,	  2,	   0,		 0,		c_lan}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Xpendo_Finisher[1] ={{	0,	 0,	   	25,		  0, s900,		  0,	  2,	   0,		 0,		c_hong}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_iRace[1] =		{{	"TY",	 0,	   	0,		  0, s500,		  0,	  3,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_BT_Race[1] =		{{	"TY",	 0,	   	0,		  0, s700,		  0,	  3,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_BioRace[1] =		{{	"TY",	 0,	   	0,		  0, s600,		  0,	  3,	   0,		 0,		c_white}};

												 //seqName taper number OperaMode speed  RecSpeed  torque  AngleCW  AngleCCW 	color
Parameter_inside  fseq_inside_Edge_platinum[1] ={{	"TY",	 0,	   	0,		  0, s350,		  0,	  11,	   0,		 0,		c_white}};


Seq_parameter_outside 	New_file_parameter;


uint8_t CurentDevTorque;			//=cur/tor
const char* ver[]=
{
    {"Ver.1.1.2.3"}, 				//版本显示
};

void MenuDataInit()
{

    mem.MemoryNum   = 0;
    sel.OperaMode   = 0;
    sel.Speed       = 0;
    sel.RecSpeed    = 0;
    sel.Torque      = 0;
    sel.AngleCW     = d30;
    sel.AngleCCW    = d150;
    sel.ApicalMode  = 0;
    sel.AutoStart   = 0;
    sel.AutoStop    = 0;
    sel.FileLibrary = 0;
	sel.FileSeq     = 0;
	sel.color       = 0;

    set.AutoPowerOffTime = 0;
    set.AutoStandyTime   = 3;
    set.BeepVol          = 2;
    set.Calibration      = OFF;
    set.Hand             = 0;				//right hand
    set.Language         = 0;				//english
    set.RestoreFactorySettings = OFF;
	set.StartupMemory    = 0;		//m0
    set.ApexSense        = 1;				//m0
    set.backlight        = 3;  				//back light

    sys.BatteryLevel     = 3;
    sys.TorqueBar        = 0;
    
    sys.FlagRunWithApex  = 0;
    sys.RunEALflag       = 0;
    sys.mma8452_get_flag = 0;
    sys.AutoSleep_cnt    = 30;
    sys.InSleep          = 0;
    sys.Charging         = 0;
    sys.FullCharge       = 0;
    sys.DispBattFlag     = 0;
    sys.ActiveShutRunning = 0;
    sys.ApicalSlowing    = 0;
    sys.InRoot           = 0;
    sys.InApicalOff      = 0;
    sys.error            = 0;
    sys.error_num        = 0;
    sys.ProgramMotorStop = 0;
	sys.Incal            = 0;
	sys.over_apex_pos_flag = 0;
	sys.reach_flag       = 0;
	MenuDispInit();
}
/**********************************/



