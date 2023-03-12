#ifndef __MOVMEAN_H
#define __MOVMEAN_H
#include <stdint.h>

/*	定义滑动平均的因子	*/
#define MOVMEAN_FACTOR		5
#define MOVMEAN_CNT_LIMIT	2*MOVMEAN_FACTOR


int16_t Movmean_Filter(int16_t input_data);
float Movmean_Filter3(float input_data, float* data_buf, uint8_t data_buf_size);
int32_t Movmean_Filter2(int32_t input_data, int32_t* data_buf, uint8_t data_buf_size);
#endif
