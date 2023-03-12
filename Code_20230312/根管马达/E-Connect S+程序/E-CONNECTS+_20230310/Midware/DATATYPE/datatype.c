#include "DataType.h"


uint32_t ADC_ConvertedValue[ADC_NUM_CH];


int motor_iq = 0;
uint16_t 	Iq_cnt = 0;
uint8_t 	Read_ig_flag = 0;
int			Iq_buf[Iq_num] = {0};

uint8_t flag_cal = 0;

uint32_t  Time_cnt = 0;
uint16_t  TimeOutCnt = 0;			