#include "pressure_origin_search.h"


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-07-27
* �� �� ��: pressure_origin_search
* ����˵��: Һѹԭ��Ѱ��
*
* ��    ��: 
*			current_pressure: ��ǰҺѹֵ
*			left_distance: ʣ���г�
      
* �� �� ֵ:
*			true:  ����Һѹԭ��
*			false: δ����Һѹԭ��
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
