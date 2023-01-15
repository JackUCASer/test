#include "movmean.h"
#include "stdint.h"
#include "GL_Config.h"			/*	����myPrintf	*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-09-14
* �� �� ��: Movmean_Filter
* ����˵��: ����ƽ���˲���
* ��    ��:
			input_data�� ��������
      
* �� �� ֵ: 16λ�з�����
			��������� �� MOVMEAN_FACTORʱ�������˲����ԭʼ���ݣ���������
			��������� �� MOVMEAN_FACTORʱ���������ƽ��ֵ
			
*********************************************************************************************************
*/
static int16_t movmean_buf[MOVMEAN_FACTOR] = {0};
int16_t Movmean_Filter(int16_t input_data)
{
	static uint8_t movmean_cnt = 0;					/*	���ݼ�����	*/
	static int32_t movmean_sum = 0;					/*	���ݺ�	*/
	uint8_t index = 0;
	if(movmean_cnt < MOVMEAN_FACTOR)
	{
		movmean_buf[movmean_cnt] = input_data;		/*	������������	*/
		movmean_sum += input_data;					/*	�������ݺ�	*/
		movmean_cnt ++;								/*	���ݼ�����+1	*/
		if(movmean_cnt == MOVMEAN_FACTOR)			/*	������ݸ����ﵽMOVMEAN_FACTOR	*/
			return movmean_sum/MOVMEAN_FACTOR;		/*	���ؼ����ֵ	*/
		return input_data;							/*	���򷵻���������*/
	}
	index = movmean_cnt%MOVMEAN_FACTOR;				/*	ϵ��ȡ��	*/
	movmean_sum -= movmean_buf[index];				/*	movmean_sum��ȥ�������е�index��	*/
	movmean_buf[index] = input_data;
	movmean_sum += input_data;
	movmean_cnt ++;
	if(movmean_cnt >= MOVMEAN_CNT_LIMIT)	/*	�����ݼ����������趨�����ޣ����ݼ�����= ���ݼ�����ȡ�ࣨ�����MOVMEAN_FACTOR�� + MOVMEAN_FACTOR	*/
		movmean_cnt = movmean_cnt%MOVMEAN_FACTOR + MOVMEAN_FACTOR;
	return movmean_sum/MOVMEAN_FACTOR;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-04-25
* �� �� ��: Movmean_Filter2
* ����˵��: ����ƽ���˲���
* ��    ��:
			input_data�� ��������
      
* �� �� ֵ: 32λ�з�����
			
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-09-14
* �� �� ��: Movmean_Filter3
* ����˵��: ����ƽ���˲���
* ��    ��:
*			input_data�� float
*			data_buf: ���ڼ��㻬��ƽ���Ļ�����
*			data_buf_size�� ��������С��Ҳ�ǻ���ƽ������

* �� �� ֵ: 
			��������� �� data_buf_sizeʱ�������˲������ǰ���������ݵľ�ֵ��
			��������� �� data_buf_sizeʱ������̶������Ļ���ƽ��ֵ
			
*********************************************************************************************************
*/
float Movmean_Filter3(float input_data, float* data_buf, uint8_t data_buf_size)
{
	static uint8_t movmean_cnt = 0;					/*	���ݼ�����	*/
	static float movmean_sum = 0;					/*	���ݺ�	*/
	uint8_t index = 0;
	if(movmean_cnt < data_buf_size)
	{
		data_buf[movmean_cnt] = input_data;			/*	������������	*/
		movmean_sum += input_data;					/*	�������ݺ�	*/
		movmean_cnt ++;								/*	���ݼ�����+1	*/
		return movmean_sum/movmean_cnt;				/*	���ؼ����ֵ	*/
	}
	index = movmean_cnt%data_buf_size;				/*	ϵ��ȡ��	*/
	movmean_sum -= data_buf[index];					/*	movmean_sum��ȥ�������е�index��	*/
	
	data_buf[index] = input_data;
	movmean_sum += input_data;
	movmean_cnt ++;
	/*	�����ݼ����������趨�����ޣ����ݼ�����= ���ݼ�����ȡ�ࣨ�����MOVMEAN_FACTOR�� + MOVMEAN_FACTOR	*/
	if(movmean_cnt >= 2*data_buf_size)	
		movmean_cnt = movmean_cnt%data_buf_size + data_buf_size;
	return movmean_sum/data_buf_size;
}
