#include "pressure_origin_search.h"


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-07-27
* 函 数 名: pressure_origin_search
* 功能说明: 液压原点寻找
*
* 形    参: 
*			current_pressure: 当前液压值
*			left_distance: 剩余行程
      
* 返 回 值:
*			true:  到达液压原点
*			false: 未到达液压原点
*********************************************************************************************************
*/
bool pressure_origin_search(uint16_t current_pressure, int16_t left_distance)
{
	static uint16_t last_pressure = 0;
	int16_t diff_pressure = 0;
	
	if(left_distance > 100){
		last_pressure = current_pressure;
		return false;
	}
	diff_pressure = current_pressure - last_pressure;
	last_pressure = current_pressure;
	
	if(diff_pressure <= DIFF_KPA)
		return true;
	else
		return false;
}
