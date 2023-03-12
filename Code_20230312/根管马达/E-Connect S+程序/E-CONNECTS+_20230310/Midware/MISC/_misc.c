#include "_misc.h"
#include "MenuData.h"
//  led_cnt ????? ??
uint8_t MoveSec=0;

static uint8_t cnt = 0,sec = 0, min = 0, sleep_cnt = 0, mflag = 0,scan_time = 0;
static uint16_t led_cnt = 0;
#define SCAN_PERIOD  80                   // ?80*10ms????????
#define OFF_CNT 20

void SysClock(void) 		//
{
  cnt++;

	if(cnt >= 100) 				// 100*10 ms =1s
	{
		cnt = 0;
		sec ++;
		if(sec >= 60) 				// 60s
		{
			sec = 0;
			min++;
			if(min >= d_PowerOffTime[set.AutoPowerOffTime]) 
			{
				 min = OFF_CNT ;
			}
		}
	}
}

void ClearClock(void) 
{
	cnt = 0;
	sec = 0;
	min = 0;

	MoveSec = 0;
}

void ClearMove()
{
	MoveSec = 0;
	sys.AutoSleep_cnt = 0;
	sys.mma8452_get_flag = 0;
}

WorkMode_e Clock_Mode(WorkMode_e mode) 
{
	if(	 	 (mode == Rotory_mode) 
			|| (mode == Reciproc_mode)
			|| (mode == Atr_mode)
			|| (mode == RunWithApex_mode)
			|| (mode == RecWithApex_mode)
			|| (mode == ATRwithApex_mode)
			|| (mode == Charging_mode)
			|| (sys.Charging == 1) )						//工作在连续旋转、往复、根测 模式中，禁止自动关机
	{
		ClearClock();
	}

	if (OFF_CNT == min) //?????
	{
		//  mode = Sleep_mode;
		mode = PowerOff_mode;          //????,???? edit 2016-11-7
		//  OLED_Clear();
		ClearClock();
	}
	return mode;
}