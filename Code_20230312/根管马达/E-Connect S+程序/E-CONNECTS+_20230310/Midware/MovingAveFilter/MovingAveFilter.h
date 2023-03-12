#ifndef _MOVINGAVEFILTER_H_
#define _MOVINGAVEFILTER_H_
//#include "Datetype.h"

#define QUE_MAX 20  //队列最大长度  
typedef struct {
	unsigned int s_data[QUE_MAX];
	unsigned int s_front;//队列头  
	unsigned int s_size; //队列长度  
	unsigned int s_mean;//平均值  
}QUE_def;//循环队列结构  

void FilterPara_ini(QUE_def* SQue);
unsigned int FILTERcon(QUE_def* SQue,unsigned int d,unsigned int size);

#endif /*filter_con.h*/