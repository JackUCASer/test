#ifndef _MOVINGAVEFILTER_H_
#define _MOVINGAVEFILTER_H_
//#include "Datetype.h"

#define QUE_MAX 20  //������󳤶�  
typedef struct {
	unsigned int s_data[QUE_MAX];
	unsigned int s_front;//����ͷ  
	unsigned int s_size; //���г���  
	unsigned int s_mean;//ƽ��ֵ  
}QUE_def;//ѭ�����нṹ  

void FilterPara_ini(QUE_def* SQue);
unsigned int FILTERcon(QUE_def* SQue,unsigned int d,unsigned int size);

#endif /*filter_con.h*/