#ifndef HAMPEL_FILTER_H
#define HAMPEL_FILTER_H

#include <stdint.h>

typedef uint32_t hampel_filter_type;

#define MEDIAN_K     3              /*  single side data num, all_data_num = MEDIAN_K*2 + 1 */

// 外部接口
void clear_median_data_buf(void);
hampel_filter_type hampel_filter(hampel_filter_type data_in, uint8_t nsigma);

// 单元测试
void Hampel_Filter_Unit_Test_by_Simulate_Data(void);
void Hampel_Filter_Unit_Test_by_Collect_Data(void);
#endif