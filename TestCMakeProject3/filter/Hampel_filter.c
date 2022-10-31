#include "Hampel_filter.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BUF_SIZE    (2*MEDIAN_K+1)
//  开辟一个缓存，用于存储排序的结果
hampel_filter_type sorted_data_buf[BUF_SIZE] = {0};
hampel_filter_type raw_data_buf[BUF_SIZE] = {0};
hampel_filter_type sigma_data_buf[BUF_SIZE] = {0};
static uint8_t raw_data_cnt = 0;
static uint8_t raw_data_buf_index = 0;
static uint8_t current_data_index = 0;

//  清空缓存
void clear_median_data_buf(void)
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

//  Hampel滤波
hampel_filter_type hampel_filter(hampel_filter_type data_in, uint8_t nsigma)
{
    uint8_t ret = 0;

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
    // ret = current_data_index;

    //  4. 索引更新
    // ret = raw_data_buf_index;
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
    // return ret;
}




/*  单元测试1： 使用虚拟生成的数据进行滤波    */
void Hampel_Filter_Unit_Test_by_Simulate_Data(void)
{
    uint16_t cnt = 0;
    hampel_filter_type ret = 0;
    for(cnt = 0; cnt < 100; cnt ++)
    {
        if(cnt %30 == 0)
            ret = hampel_filter(cnt*10, 3);
        else
            ret = hampel_filter(cnt, 3);
        
        printf("No. %d || ", cnt);
        for(uint8_t i = 0; i < raw_data_cnt; i++)
            printf("%d ", raw_data_buf[i]);
        printf("|| ");
        for(uint8_t i = 0; i < raw_data_cnt; i++)
            printf("%d ", sorted_data_buf[i]);
        printf("|| ");
        for(uint8_t i = 0; i < raw_data_cnt; i++)
            printf("%d ", sigma_data_buf[i]);
        printf("|| ");
        printf("%d", ret);
        printf("\r\n");
    }
}

/*  单元测试2： 使用真实采集的数据进行滤波    */
//  定位行位置
static int SetPositionByLine(FILE *fp, int nLine)
{
    int i = 0;
    char buffer[100];
    fpos_t pos;
    rewind(fp);
    for (; i < nLine; i++)
        fgets(buffer, 100, fp);
    fgetpos(fp, &pos);
    return 0;
}

//  读取行位置数据
static int ReadByLine(char *buffer, int frame_idx)
{
    FILE *fp;
    // fp = fopen("G:/0_Algorithms/Multi_Folder/Data/SaveWindows2022_7_29_10-57-25.TXT", "r");
    fp = fopen("./data/SaveWindows2022_7_29_10-57-25.TXT", "r");    //  相对于${workspaceFolder}
    SetPositionByLine(fp, frame_idx);
    fgets(buffer, 100, fp);
    fclose(fp);
    return 0;
}

void Hampel_Filter_Unit_Test_by_Collect_Data(void)
{
    char buffer[100];
    int data[8] = {0};
    int ALL_LINE = 3000;
    hampel_filter_type ret = 0;

    for(uint16_t i = 0; i<=340; i++){
        //  1. 读取第i行数据
        ReadByLine(buffer, i);
        //  2. 将第i行的数据按照逗号隔开，并且把每个值都变成int型变量；
        // sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d", &data[0], &data[1],&data[2],&data[3],&data[4],&data[5],&data[6],&data[7]);
        sscanf(buffer, "%d,%d", &data[4], &data[7]);
        //  3. data[4]当前采集的液压数据， data[7]是单片机Hampel滤波后的数据
        ret = hampel_filter(data[4], 3);
        //  4. 打印观察
        // printf("Line %4d,%d,%d,%d\r\n", i, data[4], data[7], ret);
        printf("Line. %4d || ", i);                     //  当前数据所在行
        printf("%3d, %3d", data[4], ret);               //  原始数据，Hampel滤波后的数据
        printf("|| ");
        for(uint8_t i = 0; i < raw_data_cnt; i++)       //  查看缓存中的原始数据
            printf("%3d ", raw_data_buf[i]);
        printf("|| ");
        for(uint8_t i = 0; i < raw_data_cnt; i++)       //  查看排序后的原始数据
            printf("%3d ", sorted_data_buf[i]);
        printf("|| ");
        for(uint8_t i = 0; i < raw_data_cnt; i++)       //  查看排序后的方差数据：方差符合3 sigma原则
            printf("%d ", sigma_data_buf[i]);
        printf("\r\n");
    }
}