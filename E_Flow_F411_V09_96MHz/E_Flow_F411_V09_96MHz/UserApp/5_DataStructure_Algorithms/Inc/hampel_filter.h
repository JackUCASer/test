#ifndef __HAMPEL_FILTER_H
#define __HAMPEL_FILTER_H
#include <stdint.h>



#define MEDIAN_K     3   				//  单边样本点数量(最大值 = (255-1)/2)，总样本点数量 = MEDIAN_K*2 + 1,
typedef uint16_t hampel_filter_type;	//	hampel滤波器中的数据类型


// 外部接口
void Clear_Midian_Databuf(void);
hampel_filter_type Hampel_Filter(hampel_filter_type data_in, uint8_t nsigma);

#endif
