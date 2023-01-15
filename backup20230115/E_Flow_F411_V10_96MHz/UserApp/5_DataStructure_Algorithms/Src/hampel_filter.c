#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Hampel_Filter.h"

#define BUF_SIZE    (2*MEDIAN_K+1)

//  开辟一个缓存，用于存储排序的结果
hampel_filter_type sorted_data_buf[BUF_SIZE] = {0};
hampel_filter_type raw_data_buf[BUF_SIZE] = {0};
hampel_filter_type sigma_data_buf[BUF_SIZE] = {0};
static uint8_t raw_data_cnt = 0;
static uint8_t raw_data_buf_index = 0;
static uint8_t current_data_index = 0;

//  清空缓存
void Clear_Midian_Databuf(void)
{
    raw_data_cnt = 0;
    raw_data_buf_index = 0;
    memset(sorted_data_buf, 0, BUF_SIZE*sizeof(hampel_filter_type));
    memset(raw_data_buf, 0, BUF_SIZE*sizeof(hampel_filter_type));
}

//  冒泡排序
static void bubble_sort(hampel_filter_type* pData_buf, uint8_t pData_buf_size)
{
    uint8_t i,j;
    hampel_filter_type tmp;
    for(i = 0; i< pData_buf_size -1; i++){
        for(j =0; j< pData_buf_size -1 -i; j++){
            if (pData_buf[j] > pData_buf[j+1]) {  
                tmp = pData_buf[j];  
                pData_buf[j] = pData_buf[j+1];  
                pData_buf[j+1] = tmp;  
            }  
        }
    }
}
//  中值排序
static hampel_filter_type median_sort(hampel_filter_type* pData_buf, uint8_t pData_buf_size)
{
    bubble_sort(pData_buf, pData_buf_size);

    // 返回中值
    if(pData_buf_size%2 == 0)
        return pData_buf[pData_buf_size/2];
    else
        return pData_buf[(pData_buf_size-1)/2];
}



/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-07-27
* 函 数 名: Hampel_Filter
* 功能说明: hampel滤波，相关理论证明参见Matlab-- help hampel
*			用于去除信号的异常点
*
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
//  Hampel滤波
hampel_filter_type Hampel_Filter(hampel_filter_type data_in, uint8_t nsigma)
{

    hampel_filter_type median_value = 0;
    hampel_filter_type median_sigma_value = 0;
    hampel_filter_type abs_diff = 0;
    //  1. 数据载入缓存
    raw_data_buf[raw_data_buf_index] = data_in;
    
    //  2. 数据统计
    raw_data_cnt ++;
    if(raw_data_cnt > BUF_SIZE)
        raw_data_cnt = BUF_SIZE;

    //  3. 当前滤波对象数据所处索引
    if(raw_data_cnt < (MEDIAN_K +1))
        current_data_index = 0;
    else if(raw_data_cnt < BUF_SIZE)
        current_data_index = raw_data_buf_index - MEDIAN_K;
    else
        current_data_index = (raw_data_buf_index >= MEDIAN_K) ? (raw_data_buf_index - MEDIAN_K) : (raw_data_buf_index + BUF_SIZE - MEDIAN_K);

    //  4. 索引更新
    raw_data_buf_index ++;
    if(raw_data_buf_index >= BUF_SIZE)
        raw_data_buf_index = 0;
    
    //  5. 拷贝数据并排序，获取中值
    memcpy(sorted_data_buf, raw_data_buf, raw_data_cnt*sizeof(hampel_filter_type)); 
    median_value = median_sort(sorted_data_buf, raw_data_cnt); 

    //  6. 计算方差
    for(uint8_t i = 0; i< raw_data_cnt; i++){
        sigma_data_buf[i] = (sorted_data_buf[i] >= median_value) ? (sorted_data_buf[i] - median_value) : (median_value - sorted_data_buf[i]);
        sigma_data_buf[i] = sigma_data_buf[i]*1.4826f;
    }
    median_sigma_value = median_sort(sigma_data_buf, raw_data_cnt);

    //  7. 比较输出
    abs_diff = (raw_data_buf[current_data_index] >= median_value) ? (raw_data_buf[current_data_index] - median_value) : (median_value - raw_data_buf[current_data_index]);
    if(abs_diff > nsigma*median_sigma_value)
        return median_value;
    else 
        return raw_data_buf[current_data_index];
}

