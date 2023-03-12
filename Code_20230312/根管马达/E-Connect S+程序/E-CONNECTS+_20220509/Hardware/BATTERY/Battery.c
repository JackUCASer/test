
////////////////////////////////////////////////////////////////////
/// @file battery.c
/// @brief 系统电压检测程序等
/// 
/// 文件详细描述：	void ResBatSam(void)复位电池电压检测
///					void SamBattery(void)电压检测函数
///
///
///
///
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210820
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
////////////////////////////////////////////////////////////////////


#include "battery.h"
#include "presetting.h"
#include "beep.h"
#include "menu.h"
#include "datatype.h"
#include "preSetting.h"
#include "key_Task.h"

extern uint32_t ADC_ConvertedValue[ADC_NUM_CH];
uint16_t	Disp_bat_cnt = 0 ;
uint8_t 	Battery_COUNT_IT = 0;

/**********************************************************************************************************
*	函 数 名: void ResBatSam(void)
*	功能说明: 复位电池电压检测
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210820         
**********************************************************************************************************/
void ResBatSam(void)
{
	Disp_bat_cnt=0;
	Battery_COUNT_IT = 0;
}

/**********************************************************************************************************
*	函 数 名: void Battery_init(void)
*	功能说明: 电池初始化，开机就检测
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210820         
**********************************************************************************************************/
void Battery_init(void)
{
	uint16_t BatVol_mv;
 	
	do
	{
		Start_Adc();																//启动ADC检测电池电压
		BatVol_mv = (ADC_ConvertedValue[0] * Ref_v) >> 11;							//计算电池两端电压，mv
		
		//电量检测
		if(BatVol_mv < 3400)														//电量划分，为显示做准备
		{
			sys.BatteryLevel = 0;
		}
		else if((BatVol_mv >= 3400) && (BatVol_mv < 3600))
		{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
			sys.BatteryLevel = 1;
		}
		else if((BatVol_mv >= 3600) && (BatVol_mv < 3900))
		{
			sys.BatteryLevel = 2;
		}
		else if((BatVol_mv >= 3900) && (BatVol_mv < 4100))
		{
			sys.BatteryLevel = 3;
		}
	
		if(BatVol_mv > 4100)
		{
			sys.BatteryLevel = 4;
			sys.FullCharge   = 1;
		}
		else 
		{
			sys.FullCharge = 0;
		}
		
		sys.DispBattFlag = 1;														//更新电池显示标志位
		
		if(sys.Charging != 1)
		{																			//不在充电状态下显示

		}
	}while(0);
	
}
/**********************************************************************************************************
*	函 数 名: void SamBattery(void)
*	功能说明: 启动电压检测，2s检测1次电池电压，10ms检测是否充电等等
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1:
*	编辑日期: 1: 20210820         
**********************************************************************************************************/
void SamBattery(void)
{
	uint16_t BatVol_mv; 
	uint8_t Index_state = 1;
 	
	static uint8_t ring_sw = 0;
	// if(Get_Index()==0||Get_Index()==3 || Get_Index()==4)Index_state=0;
	if(Battery_COUNT_IT >= 10)
	{
		Battery_COUNT_IT = 0;
		Disp_bat_cnt++;
		
		if((Disp_bat_cnt > 200) &&(0 == Get_Index()))
		{	//2s 刷新一次
			Disp_bat_cnt = 0;															//计数清零
			Start_Adc();																//启动ADC检测电池电压
			BatVol_mv = (ADC_ConvertedValue[0] * Ref_v) >> 11;							//计算电池两端电压，mv
			
			//电量检测
			if(BatVol_mv < 3400)														//电量划分，为显示做准备
			{
				sys.BatteryLevel = 0;
			}
			else if((BatVol_mv >= 3400) && (BatVol_mv < 3600))
			{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
				sys.BatteryLevel = 1;
			}
			else if((BatVol_mv >= 3600) && (BatVol_mv < 3900))
			{
				sys.BatteryLevel = 2;
			}
			else if((BatVol_mv >= 3900) && (BatVol_mv < 4100))
			{
				sys.BatteryLevel = 3;
			}
		
			if(BatVol_mv > 4100)
			{
				sys.BatteryLevel = 4;
				sys.FullCharge   = 1;
			}
			else 
			{
				sys.FullCharge = 0;
			}
			
			sys.DispBattFlag = 1;														//更新电池显示标志位
			
			if(sys.Charging != 1)
			{																			//不在充电状态下显示

			}
		}
		
		if((HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin) == 1)	&& (sys.Charging == 0))						//在 Menu.c 里面跳转到充电界面
		{
			if(ring_sw == 1)
			{
				ring_sw = 0;
				Ring.short_ring = 1;
			}
			sys.Charging = 1;															//检测外接电源引脚，高电平表示正在充电
		}
		else if((HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin) == 0))
		{
			ring_sw = 1;
			sys.Charging = 0;
		}
	}
}








