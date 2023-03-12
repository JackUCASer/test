#include "beep.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "menudata.h"


BEEP Ring;
uint8_t BEEP_IT_Counter;
extern uint8_t Torque_reach_ring;
/**************************************************************
 * 函数名：ENDO_BEEP()
 * 描述  ：声音处理函数,在10ms定时器内运行
 * 输入  :
 * 输出  ：
 ***************************************************************/
void ENDO_BEEP() 
{
	static uint8_t last_fre=0;
	static uint8_t ring_cnt=0;
	uint16_t volume;
	volume = d_Beep[set.BeepVol];
	
	if(Ring.apex_beep_on == 1) 
	{
		if(last_fre != Ring.fre)
		{
			Ring.beep_cnt = 0;
		}
		
		Ring.beep_cnt ++;
		
		if(Ring.beep_cnt <= 5)
		{
			// EPWM3_LoadDutyValue(volume);           //PWM输出，驱动蜂鸣器发声
			TIM3->CCR1 = volume;
		}
		if(Ring.beep_cnt > 5)
		{
		//EPWM3_LoadDutyValue(0);
			TIM3->CCR1 = 0;
		}
		if(Ring.beep_cnt == Ring.fre)
		{
			Ring.beep_cnt = 0;
		}

		last_fre = Ring.fre;
	}
	else if (Ring.short_ring == 1) 
	{
				
		ring_cnt++;
		//   EPWM3_LoadDutyValue(volume);           //PWM输出，驱动蜂鸣器发声
		TIM3->CCR1 = volume;

		if (ring_cnt == 6) 
		{          
			//设定一个响铃时长
			Ring.short_ring = 0;
			ring_cnt = 0;
		}
	}
	else if (Ring.long_ring==1) 
	{		 
		ring_cnt++;
		TIM3->CCR1 = volume;

		if (ring_cnt == 60) 
		{          
			//设定一个响铃时长
			Ring.long_ring = 0;
			ring_cnt = 0;
			Torque_reach_ring = 1;
		}
	}
	else
	{
		// EPWM3_LoadDutyValue(0);
		TIM3->CCR1 = 0;
	}
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void ApexRing(uint8_t fre,uint8_t on)
{
    Ring.fre = fre;
    Ring.apex_beep_on = on;
//	Ring.beep_cnt=0;
}

void EndoRing(uint8_t fre,uint8_t on)
{
    Ring.fre = fre;
    Ring.apex_beep_on = on;
//	Ring.beep_cnt=0;
}









