#include "Error.h"
#include "datatype.h"
#include "MenuData.h"
#include "beep.h"
#include "battery.h"
#include "work_mode.h"


uint16_t HwFault_cnt=0;

/**********************************************************************************************************
*	函 数 名: void CheckLowBattery()
*	功能说明: 运行过程中电量检测
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211224         
**********************************************************************************************************/
void CheckLowBattery()
{
    static uint8_t lowp_cnt = 0;
    uint16_t BatVol_mv;
    BatVol_mv = (ADC_ConvertedValue[0]*Ref_v)>>11;		//计算电池两端电压，mv
    if(BatVol_mv < 2500)
    {
		if(ADC_ConvertedValue[0] > 100)					//出现电池电压采样异常，不累计
		{
			lowp_cnt++;
		}
        if(lowp_cnt > 50)			//累积次数
        {
            lowp_cnt = 0;
            sys.error = 1;
            sys.error_num = e_LowPower;
        }
    }
    else 
	{
        lowp_cnt = 0;
    }
}
/**********************************************************************************************************
*	函 数 名: void CheckLowBattery()
*	功能说明: 运行过程中电量检测
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211224         
**********************************************************************************************************/
void OverCurentProtect()
{
    static	uint8_t over_cnt = 0,low_cnt = 0;
    /* 过流保护*/
    if(motor_iq >= 280 || motor_iq <= -280)
	{											//超过电流阀值
        over_cnt++;
		if(over_cnt >= 50)
		{
			over_cnt = 50;
			low_cnt = 0;
			sys.error = 1;
			sys.error_num = e_OverCurrent;
		}
    }
    else if((motor_iq <= 200) && (motor_iq >= -300))
	{
		low_cnt++;
		if(low_cnt >= 50)
		{
			over_cnt = 0;
			low_cnt = 50;
		}
	}
}

/**********************************************************************************************************
*	函 数 名: void CheckLowBattery()
*	功能说明: 运行过程中电量检测
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211224         
**********************************************************************************************************/
WorkMode_e  CheckHardwareFault(WorkMode_e  mode)
{
    CheckLowBattery();							//检测运行中电池电压是否过低
    OverCurentProtect();						//检测电流是否过大

    if(sys.error==1)
	{											//发生错误，进入故障模式
        mode = Fault_mode;
        ApexRing(0,0);
        Ring.long_ring = 1;
    }
    return mode;
}





























