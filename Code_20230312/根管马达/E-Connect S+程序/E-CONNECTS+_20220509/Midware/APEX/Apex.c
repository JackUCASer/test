#include "apex.h"
#include "main.h"
#include "datatype.h"
#include "presetting.h"
#include "menudata.h"
#include "MovingAveFilter.h"
#include "work_mode.h"

SAM_DAT samh,saml;
Str_FL flag;
uint32_t SamH[20];
uint32_t SamL[20];
uint32_t Ratio_mul;
QUE_def samH_Que,samL_Que;

uint32_t	CAL_APEX_CAL_3K6_100nF;
uint32_t 	samH_RootInPosTemp = 480;
uint32_t  	samH_RootInPos = 550;			//判断进入根管的标记 根据高频信号
uint32_t  	samH_RootOutPos = 630;
uint8_t 	InRoot = 0;
uint8_t 	RunWithApex_InRootFlag = 0;
uint8_t 	Last_apex_pos = 24;				//根测显示的位置控制，上一个位置点记录
unsigned int Ratio;

const unsigned int Init_APEX_3K6_100nF =755;
float E_rate;       						//误差变动率
ApexNum apex_cnt= {0,0};
ApexTab apex_pos= {
    610,		//-2
    630,		//-1
    670,		//00			0 apex
    730,		//01
    775,		//02			0.5
    798,		//03
    820,		//04	 		1.0
    840,		//05
    860,		//06			1.5
    870,		//07
    885,		//08			2.0
    905,		//09
    915,		//10			2.5
    922,		//11
    930,	  	//12			3.0
    940,		//13
    950,		//14
    960,		//15
    970,		//16
    980,		//17
};



void Apex_cal()
{
    uint8_t i=0;
    uint32_t E_rate1000, cal_3k6_100nF_sum = 0, samH_sum = 0;
    HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_SET);					//RELAY ON 切换到基准
    HAL_Delay(150);
	flag_cal = 1;
    while(i < 10) 
	{
		if(flag.sam_finish == 1) 
		{
			flag.sam_finish = 0;
			Ratio_mul = 1000 * samh.VPP;

			if(saml.VPP != 0)
			{
				Ratio = Ratio_mul / saml.VPP;
			}	
			else 
			{
				Ratio = 0;
			}
			cal_3k6_100nF_sum += Ratio;
			samH_sum += samh.VPP;
			HAL_Delay(50);
			i++;
		}
    }
    HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_RESET);				//RELAY OFF 切换到外部回路
	flag_cal = 0;
    CAL_APEX_CAL_3K6_100nF = cal_3k6_100nF_sum / 10;  							//平均滤波
    samH_RootInPosTemp = (samH_sum / 10) * 2.5;									//作为进入根管的判断依据  实验数据得出的数值，须校准 开路时的数值
    samH_RootInPos = (samH_RootInPosTemp + samH_RootOutPos) / 2;				//取中间值
    samH_sum = 0;
    E_rate = (float)Init_APEX_3K6_100nF / (float)CAL_APEX_CAL_3K6_100nF;     	//计算变动率

    E_rate1000 = E_rate * 1000;
    Ratio = apex_pos.Pos_17 + 50;
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void CalculateApex()
{
	static uint8_t  stable_cnt = 0;
	static uint8_t 	Loss_cnt = 0;
    static unsigned int Last_Apex_num = 0;
	/****  未进入根管，检测信号是否满足********/
	if(samh.VPP <= samH_RootInPos && InRoot == 0)			//满足进入根管的数值  & 之前未进入根管
	{
		stable_cnt++;										//数据稳定，延时2ms1次
	}
	else	
	{
		stable_cnt = 0;
	}
	/******* 信号满足 并且延时稳定后进行计算 *******/
	if(stable_cnt > 5 && InRoot==0) 
	{		
		stable_cnt = 0;
		InRoot = 1;
		if(samh.VPP > 50)
		{													//信号正常
			Ratio_mul = 1000 * samh.VPP;
			Ratio = Ratio_mul / saml.VPP;					//计算比值 [1000倍实际值]
			Ratio = Ratio * E_rate;
		}
//		/*------------应急措施----------------*/	
//									if(samh.VPP<50 && saml.VPP>50)			    //h信号不可用，抛弃h信号
//									{
//								Ratio=CalculateSamL(saml.VPP);
//									}
//							  	else	if(samh.VPP>50 && saml.VPP<50)        //l信号不可用，抛弃l信号
//									{
//								Ratio=CalculateSamH(samh.VPP);	
//									}
//										
//		/*----------------------------------*/	
	}
                /******* 进入根管后，不延时直接计算 *************/
	if(InRoot==1)
	{
		/*************** 短接 ***************/
		if(samh.VPP < 20 && saml.VPP < 20)
		{
			Ratio=0;				//判断为短接后，防止数值计算错误，强制归零
		}
		if(samh.VPP > 50)
		{
			Ratio_mul =  1000 * samh.VPP;
			Ratio = Ratio_mul / saml.VPP;		//计算比值 [1000倍实际值]
			Ratio = Ratio * E_rate;
		}
				/*------------应急措施----------------*/	
//									if(samh.VPP<50 && saml.VPP>50)					//h信号不可用，抛弃；使用L信号
//									{
//									Ratio=CalculateSamL(saml.VPP);					//查l信号的表格，直接给出比值
//									}
//									else if(samh.VPP>50 && saml.VPP<50)        //l信号不可用，抛弃l信号，使用h信号
//									{
//							   	Ratio=CalculateSamH(samh.VPP);								//查h信号的表格，直接给出比值
//									}

	}
				/*----------------------------------*/				
	/******** 离开根管 检测信号是否满足 ****************/
	if(samh.VPP > samH_RootInPos)
	{
		Loss_cnt++;
	}
	else 
	{
		Loss_cnt=0;
	}
	if(Loss_cnt>3)			//1S
	{
		Loss_cnt = 0;
		InRoot = 0;
		Ratio = apex_pos.Pos_17 + 50;
	}		

    if(flag.sam_finish == 1) 
	{
		flag.sam_finish=0;		

//                    Ratio_mul=1000*samh.VPP;
//                    Ratio=Ratio_mul/saml.VPP;		//计算比值 [1000倍实际值]
//                    Ratio=Ratio*E_rate;
//			
        if(Ratio < apex_pos.Pos__2)
        {
            if(apex_cnt.num==20)
            {
                apex_cnt.cnt++;
            }
            else 
			{
                apex_cnt.cnt = 0;
                apex_cnt.num=20;
            }
        }
        else if(Ratio>apex_pos.Pos__2 && Ratio<=apex_pos.Pos__1)
        {
            if(apex_cnt.num==19)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=19;
            }
        }
        else if(Ratio>apex_pos.Pos__1 && Ratio<=apex_pos.Pos_00)
        {
            if(apex_cnt.num==18)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=18;
            }
        }
        else if(Ratio>apex_pos.Pos_00 && Ratio<=apex_pos.Pos_01)
        {
            if(apex_cnt.num==17)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=17;
            }
        }

        else if(Ratio>apex_pos.Pos_01 && Ratio<=apex_pos.Pos_02)
        {
            if(apex_cnt.num==16)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=16;
            }
        }
        else if(Ratio>apex_pos.Pos_02 && Ratio<=apex_pos.Pos_03)
        {
            if(apex_cnt.num==15)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=15;
            }
        }
        else if(Ratio>apex_pos.Pos_03 && Ratio<=apex_pos.Pos_04)
        {
            if(apex_cnt.num==14)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=14;
            }
        }
        else if(Ratio>apex_pos.Pos_04 && Ratio<=apex_pos.Pos_05)
        {
            if(apex_cnt.num==13)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=13;
            }
        }
        else if(Ratio>apex_pos.Pos_05 && Ratio<=apex_pos.Pos_06)
        {
            if(apex_cnt.num==12)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=12;
            }
        }
        else if(Ratio>apex_pos.Pos_06 && Ratio<=apex_pos.Pos_07)
        {
            if(apex_cnt.num==11)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=11;
            }
        }
        else if(Ratio>apex_pos.Pos_07 && Ratio<=apex_pos.Pos_08)
        {
            if(apex_cnt.num==10)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=10;
            }
        }

        else if(Ratio>apex_pos.Pos_08 && Ratio<=apex_pos.Pos_09)
        {
            if(apex_cnt.num==9)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=9;
            }
        }

        else if(Ratio>apex_pos.Pos_09 && Ratio<=apex_pos.Pos_10)
        {
            if(apex_cnt.num==8)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=8;
            }
        }

        else if(Ratio>apex_pos.Pos_10 && Ratio<=apex_pos.Pos_11)
        {
            if(apex_cnt.num==7)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=7;
            }
        }

        else if(Ratio>apex_pos.Pos_11 && Ratio<=apex_pos.Pos_12)
        {
            if(apex_cnt.num==6)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=6;
            }
        }

        else if(Ratio>apex_pos.Pos_12 && Ratio<=apex_pos.Pos_13)
        {
            if(apex_cnt.num==5)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=5;
            }
        }

        else if(Ratio>apex_pos.Pos_13 && Ratio<=apex_pos.Pos_14)
        {
            if(apex_cnt.num==4)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=4;
            }
        }

        else if(Ratio>apex_pos.Pos_14 && Ratio<=apex_pos.Pos_15)
        {
            if(apex_cnt.num==3)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=3;
            }
        }

        else if(Ratio>apex_pos.Pos_15 && Ratio<=apex_pos.Pos_16)
        {
            if(apex_cnt.num==2)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=2;
            }
        }

        else if((Ratio>apex_pos.Pos_16 && Ratio<=apex_pos.Pos_17)||(InRoot==1))
        {
            if(apex_cnt.num==1)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=1;
            }
        }

        else if(Ratio>apex_pos.Pos_17 )
        {
            if(apex_cnt.num==0)
            {
                apex_cnt.cnt++;
            }
            else {
                apex_cnt.cnt=0;
                apex_cnt.num=0;
            }
        }
    }
    if(Last_Apex_num==0) 
	{
        HAL_Delay(2);
        if(apex_cnt.cnt>DELAY_CNT &&apex_cnt.cnt < 20 && apex_cnt.num!=20)			//初次进入根管 && 测量不是-2
        {
            sys.apex = apex_cnt.num;
            Last_Apex_num = sys.apex;
        }
		else if(apex_cnt.cnt > 20)
		{
			sys.apex = apex_cnt.num;
            Last_Apex_num = sys.apex;
		}
    }
    else if(Last_Apex_num!=0)
    {

        if(apex_cnt.cnt>DELAY_CNT)
        {
            sys.apex=apex_cnt.num;
            Last_Apex_num=sys.apex;
        }

    }
}

void Get_Set_ApexFre()
{
	static unsigned char change_pwm_cnt = 0;			//频率变换计数器
    static uint8_t i = 0, k = 0, stable_cnt = 0;
	static uint8_t Loss_cnt = 0;
	static unsigned int last_8KHz_H, last_8KHz_L, last_400Hz_H, last_400Hz_L;
	uint8_t j;
	uint16_t samH = 0,samL = 0;
	unsigned int tmpH, tmpL = 0, tmp;
	change_pwm_cnt++;
	if(change_pwm_cnt == 6)										//14
	{
		flag.samL_Rise_begin = 1;

	}
	if(change_pwm_cnt == 22)										//46
	{
		flag.samH_Rise_begin = 1;
	}
	if(change_pwm_cnt <= 1)
	{
		TIM1->ARR = ROOT_400HZ_ARR;									//设置400HZ频率
		TIM1->CCR3 = ROOT_400HZ_CCR;
	}
	if(change_pwm_cnt == 15)										//32
	{
		TIM1->CNT = 0;
		TIM1->ARR = ROOT_8KHZ_ARR;									//设置8KHZ频率
		TIM1->CCR3 = ROOT_8KHZ_CCR;
	}
	if(change_pwm_cnt > 31)											//61
	{
		change_pwm_cnt = 0;
	}

	if(flag.samL_Rise_begin == 1)
	{
		Start_Adc();
				 
		SamH[i] = ADC_ConvertedValue[1]>>2;
		SamL[i] = ADC_ConvertedValue[2]>>2;
		
		i++;
		if(i >= 2)
		{
			i = 0;
			samh.sum = 0;
			saml.sum = 0;
			for(j = 1; j < 2; j++) 									//抛弃第一个数
			{
				samh.sum += SamH[j];
				saml.sum += SamL[j];
			}
			samh.min = samh.sum / 1;								//获取高频谷值，低频峰值
			saml.max = saml.sum / 1;
			
			//---采样偏差控制----低频峰值-----------------
			if(saml.max > last_400Hz_H)
			{
				tmp = saml.max - last_400Hz_H;
				if( tmp < FILTER_N )                 				//偏差范围内使用前一次数据
				{
					saml.max = last_400Hz_H;
				}
			}
			else
			{
				tmp = last_400Hz_H - saml.max;
				if( tmp < FILTER_N )                 				//偏差范围内使用前一次数据
				{
					saml.max = last_400Hz_H;
				}
			} 
			last_400Hz_H = saml.max;
           //---采样偏差控制----高频谷值-----------------
			if(samh.min > last_8KHz_L)
			{
				tmp = samh.min -last_8KHz_L;
				if( tmp < FILTER_N )                 				//偏差范围内使用前一次数据
				{
					samh.min = last_8KHz_L;
				}
			}
			else
			{
				tmp = last_8KHz_L - samh.min;
				if( tmp < FILTER_N )                 				//偏差范围内使用前一次数据
				{
					samh.min = last_8KHz_L;
				}
			}
                
			last_8KHz_L = samh.min;
                
			//----------------------

			flag.samL_Rise_begin = 0;
		}
	}

	if(flag.samH_Rise_begin == 1)
	{
		Start_Adc();
			 
		SamH[k]=ADC_ConvertedValue[1] >> 2;
		SamL[k]=ADC_ConvertedValue[2] >> 2;
		k++;

		if(k>=2)
		{
			k = 0;
			samh.sum = 0;

			saml.sum = 0;
			for(j = 1; j < 2; j++)  								//抛弃第一个数
			{
				samh.sum += SamH[j];
				saml.sum += SamL[j];
			}
			samh.max = samh.sum / 1;      							//获取高频峰值，低频谷值
			saml.min = saml.sum / 1;
           //---采样偏差控制--------高频峰值------------
			if(samh.max > last_8KHz_H)
			{
				tmp = samh.max  - last_8KHz_H;
				if( tmp < FILTER_N )                 				//偏差范围内使用前一次数据
				{
					samh.max = last_8KHz_H;
				}
			}
			else
			{
				tmp = last_8KHz_H - samh.max;
				if( tmp < FILTER_N )                 	//偏差范围内使用前一次数据
				{
					samh.max = last_8KHz_H;
				}
			}
                
			last_8KHz_H = samh.max;
			//---采样偏差控制-------低频谷值--------------
			if(saml.min > last_400Hz_L)
			{
				tmp = saml.min  - last_400Hz_L;
				if( tmp < FILTER_N )                 //偏差范围内使用前一次数据
				{
					saml.min = last_400Hz_L;
				}
			}
			else
			{
			   tmp = last_400Hz_L - saml.min;
				if( tmp < FILTER_N )                 //偏差范围内使用前一次数据
				{
					saml.min = last_400Hz_L;
				}
			}
                
			last_400Hz_L = saml.min;
                  
              //----------------------------------------------
			flag.samH_Rise_begin = 0;
#if 0
			if(samh.max>samh.min)
			{
				samh.VPP=samh.max-samh.min;      //计算高频峰峰值
			}
			else 
			{
				samh.VPP=0;
			}
			if(saml.max>saml.min)
			{
				saml.VPP=saml.max-saml.min;			//计算低频峰峰值
			}
			else 
			{
				saml.VPP=0;
			}
			flag.sam_finish=1;                          // 数据已更新
#endif		
	
#if 1	
                  /********************滑动平均*******************/

			if(flag_cal == 0)  
			{        																	//未在校准状态
				
				if((WorkMode == Standby_mode)||(WorkMode == Measure_mode))
				{
					if(samh.max > samh.min)
					{
					   samH = samh.max - samh.min;      									//计算高频峰峰值 2021-8-3
					   // samh.VPP=samh.max-samh.min;      									//计算高频峰峰值
					}
					else 
					{
					   samH = 0;
					}
					if(saml.max > saml.min)
					{
						samL = saml.max - saml.min;											//计算低频峰峰值 2021-8-3
						//saml.VPP=saml.max-saml.min;									    //计算低频峰峰值
					}
					else 
					{
					   samL = 0;
					}
					samh.VPP = FILTERcon(&samH_Que,samH,d_ApexSense[set.ApexSense]) ;   	//滑动平均计算H值
					saml.VPP = FILTERcon(&samL_Que,samL,d_ApexSense[set.ApexSense]) ; 		//滑动平均计算L值
					if((samH < SHORT_CIRCUIT_THRESHOLD)&&(samL < SHORT_CIRCUIT_THRESHOLD)) 	//加快短接判断2021-8-5
					{
						samh.VPP=0;
						
					}
				}else
				{
					if(samh.max>samh.min)
					{
						samh.VPP=samh.max-samh.min;      //计算高频峰峰值
					}
					else 
					{
						samh.VPP=0;
					}
					if(saml.max>saml.min)
					{
						saml.VPP=saml.max-saml.min;			//计算低频峰峰值
					}
					else 
					{
						saml.VPP=0;
					}
				}
				flag.sam_finish=1;                          // 数据已更新
			}
			else if(flag_cal==1)
			{
/*****************************原算法**********************************/
		
				if(samh.max>samh.min)
				{
					samh.VPP=samh.max-samh.min;      //计算高频峰峰值
				}
				else 
				{
					samh.VPP=0;
				}
				if(saml.max>saml.min)
				{
					saml.VPP=saml.max-saml.min;			//计算低频峰峰值
				}
				else 
				{
					saml.VPP=0;
				}
				flag.sam_finish=1;                          // 数据已更新
			}	
#endif		
		}				
	}
}




