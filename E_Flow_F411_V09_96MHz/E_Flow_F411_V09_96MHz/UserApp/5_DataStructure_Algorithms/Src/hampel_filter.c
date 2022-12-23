#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Hampel_Filter.h"

#define BUF_SIZE    (2*MEDIAN_K+1)

//  ����һ�����棬���ڴ洢����Ľ��
hampel_filter_type sorted_data_buf[BUF_SIZE] = {0};
hampel_filter_type raw_data_buf[BUF_SIZE] = {0};
hampel_filter_type sigma_data_buf[BUF_SIZE] = {0};
static uint8_t raw_data_cnt = 0;
static uint8_t raw_data_buf_index = 0;
static uint8_t current_data_index = 0;

//  ��ջ���
void Clear_Midian_Databuf(void)
{
    raw_data_cnt = 0;
    raw_data_buf_index = 0;
    memset(sorted_data_buf, 0, BUF_SIZE*sizeof(hampel_filter_type));
    memset(raw_data_buf, 0, BUF_SIZE*sizeof(hampel_filter_type));
}

//  ð������
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
//  ��ֵ����
static hampel_filter_type median_sort(hampel_filter_type* pData_buf, uint8_t pData_buf_size)
{
    bubble_sort(pData_buf, pData_buf_size);

    // ������ֵ
    if(pData_buf_size%2 == 0)
        return pData_buf[pData_buf_size/2];
    else
        return pData_buf[(pData_buf_size-1)/2];
}



/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-07-27
* �� �� ��: Hampel_Filter
* ����˵��: hampel�˲����������֤���μ�Matlab-- help hampel
*			����ȥ���źŵ��쳣��
*
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
//  Hampel�˲�
hampel_filter_type Hampel_Filter(hampel_filter_type data_in, uint8_t nsigma)
{

    hampel_filter_type median_value = 0;
    hampel_filter_type median_sigma_value = 0;
    hampel_filter_type abs_diff = 0;
    //  1. �������뻺��
    raw_data_buf[raw_data_buf_index] = data_in;
    
    //  2. ����ͳ��
    raw_data_cnt ++;
    if(raw_data_cnt > BUF_SIZE)
        raw_data_cnt = BUF_SIZE;

    //  3. ��ǰ�˲�����������������
    if(raw_data_cnt < (MEDIAN_K +1))
        current_data_index = 0;
    else if(raw_data_cnt < BUF_SIZE)
        current_data_index = raw_data_buf_index - MEDIAN_K;
    else
        current_data_index = (raw_data_buf_index >= MEDIAN_K) ? (raw_data_buf_index - MEDIAN_K) : (raw_data_buf_index + BUF_SIZE - MEDIAN_K);

    //  4. ��������
    raw_data_buf_index ++;
    if(raw_data_buf_index >= BUF_SIZE)
        raw_data_buf_index = 0;
    
    //  5. �������ݲ����򣬻�ȡ��ֵ
    memcpy(sorted_data_buf, raw_data_buf, raw_data_cnt*sizeof(hampel_filter_type)); 
    median_value = median_sort(sorted_data_buf, raw_data_cnt); 

    //  6. ���㷽��
    for(uint8_t i = 0; i< raw_data_cnt; i++){
        sigma_data_buf[i] = (sorted_data_buf[i] >= median_value) ? (sorted_data_buf[i] - median_value) : (median_value - sorted_data_buf[i]);
        sigma_data_buf[i] = sigma_data_buf[i]*1.4826f;
    }
    median_sigma_value = median_sort(sigma_data_buf, raw_data_cnt);

    //  7. �Ƚ����
    abs_diff = (raw_data_buf[current_data_index] >= median_value) ? (raw_data_buf[current_data_index] - median_value) : (median_value - raw_data_buf[current_data_index]);
    if(abs_diff > nsigma*median_sigma_value)
        return median_value;
    else 
        return raw_data_buf[current_data_index];
}

