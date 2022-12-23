#include "movmean.h"
#include "stdint.h"
#include "GL_Config.h"			/*	依赖myPrintf	*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-09-14
* 函 数 名: Movmean_Filter
* 功能说明: 滑动平均滤波器
* 形    参:
			input_data： 输入数据
      
* 返 回 值: 16位有符号数
			当输入个数 ＜ MOVMEAN_FACTOR时，滑动滤波输出原始数据，不做处理；
			当输入个数 ≥ MOVMEAN_FACTOR时，输出滑动平均值
			
*********************************************************************************************************
*/
static int16_t movmean_buf[MOVMEAN_FACTOR] = {0};
int16_t Movmean_Filter(int16_t input_data)
{
	static uint8_t movmean_cnt = 0;					/*	数据计数器	*/
	static int32_t movmean_sum = 0;					/*	数据和	*/
	uint8_t index = 0;
	if(movmean_cnt < MOVMEAN_FACTOR)
	{
		movmean_buf[movmean_cnt] = input_data;		/*	保存输入数据	*/
		movmean_sum += input_data;					/*	计算数据和	*/
		movmean_cnt ++;								/*	数据计数器+1	*/
		if(movmean_cnt == MOVMEAN_FACTOR)			/*	如果数据个数达到MOVMEAN_FACTOR	*/
			return movmean_sum/MOVMEAN_FACTOR;		/*	返回计算均值	*/
		return input_data;							/*	否则返回输入数据*/
	}
	index = movmean_cnt%MOVMEAN_FACTOR;				/*	系数取余	*/
	movmean_sum -= movmean_buf[index];				/*	movmean_sum减去缓冲区中第index数	*/
	movmean_buf[index] = input_data;
	movmean_sum += input_data;
	movmean_cnt ++;
	if(movmean_cnt >= MOVMEAN_CNT_LIMIT)	/*	当数据计数器超过设定的上限，数据计数器= 数据计数器取余（相对于MOVMEAN_FACTOR） + MOVMEAN_FACTOR	*/
		movmean_cnt = movmean_cnt%MOVMEAN_FACTOR + MOVMEAN_FACTOR;
	return movmean_sum/MOVMEAN_FACTOR;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-25
* 函 数 名: Movmean_Filter2
* 功能说明: 滑动平均滤波器
* 形    参:
			input_data： 输入数据
      
* 返 回 值: 32位有符号数
			
*********************************************************************************************************
*/
int32_t Movmean_Filter2(int32_t input_data, int32_t* data_buf, uint8_t data_buf_size)
{
	static uint8_t index = 0;
	int32_t ret = 0;
	data_buf[index] = input_data;
	index ++;
	if(index == data_buf_size)
		index = 0;
	for(uint8_t i =0; i< data_buf_size; i++){
		ret += data_buf[i];
	}
	return ret/data_buf_size;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-09-14
* 函 数 名: Movmean_Filter3
* 功能说明: 滑动平均滤波器
* 形    参:
*			input_data： float
*			data_buf: 用于计算滑动平均的缓冲区
*			data_buf_size： 缓冲区大小，也是滑动平均因子

* 返 回 值: 
			当输入个数 ＜ data_buf_size时，滑动滤波输出当前个数的数据的均值；
			当输入个数 ≥ data_buf_size时，输出固定个数的滑动平均值
			
*********************************************************************************************************
*/
float Movmean_Filter3(float input_data, float* data_buf, uint8_t data_buf_size)
{
	static uint8_t movmean_cnt = 0;					/*	数据计数器	*/
	static float movmean_sum = 0;					/*	数据和	*/
	uint8_t index = 0;
	if(movmean_cnt < data_buf_size)
	{
		data_buf[movmean_cnt] = input_data;			/*	保存输入数据	*/
		movmean_sum += input_data;					/*	计算数据和	*/
		movmean_cnt ++;								/*	数据计数器+1	*/
		return movmean_sum/movmean_cnt;				/*	返回计算均值	*/
	}
	index = movmean_cnt%data_buf_size;				/*	系数取余	*/
	movmean_sum -= data_buf[index];					/*	movmean_sum减去缓冲区中第index数	*/
	
	data_buf[index] = input_data;
	movmean_sum += input_data;
	movmean_cnt ++;
	/*	当数据计数器超过设定的上限，数据计数器= 数据计数器取余（相对于MOVMEAN_FACTOR） + MOVMEAN_FACTOR	*/
	if(movmean_cnt >= 2*data_buf_size)	
		movmean_cnt = movmean_cnt%data_buf_size + data_buf_size;
	return movmean_sum/data_buf_size;
}
