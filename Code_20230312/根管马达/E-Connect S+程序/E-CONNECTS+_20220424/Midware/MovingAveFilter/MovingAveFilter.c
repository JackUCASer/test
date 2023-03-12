
#include "MovingAveFilter.h"



void que_push(QUE_def* SQue,unsigned int d,unsigned int size)
{
	if(SQue->s_size == size) return;// 
	SQue->s_data[(SQue->s_front+SQue->s_size) % size] = d;//
	SQue->s_size++;
}


void que_pop(QUE_def* SQue,unsigned int size)
{
	if(SQue->s_size == 0) return;//
	if(SQue->s_front >= size) SQue->s_front %= size;
	SQue->s_front++;//
	SQue->s_size--; //
}

//
unsigned int que_top(QUE_def* SQue,unsigned int size)
{
	if(SQue->s_size == 0) return 0; //
	return SQue->s_data[SQue->s_front % size]; //
}

//
unsigned int que_fil(QUE_def* SQue,unsigned int size)
{
	unsigned int i;
	unsigned int temp = 0;
	for(i=0; i<SQue->s_size; i++)
	{
		temp += SQue->s_data[(SQue->s_front + i) % size];
	}
	temp /= SQue->s_size;
	return (unsigned int)temp;
}

void FilterPara_ini(QUE_def* SQue)
{
	SQue->s_size = 0;
	SQue->s_front = 0;
	SQue->s_mean	= 0;
}

/**************************************************************
  *Function:   滑动平均滤波
  *PreCondition:  
  *Input:   队列缓存，输入采样数据，滤波数量
  *Output: 
 ***************************************************************/
unsigned int FILTERcon(QUE_def* SQue,unsigned int d,unsigned int size)
{
	que_push(SQue,d,size);// 
	if(SQue->s_size >= size)
	{
		que_pop(SQue,size);//
	}
	SQue->s_mean = que_fil(SQue,size);
	return SQue->s_mean;
}
