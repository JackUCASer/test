#ifndef __LP_FILTER_H
#define __LP_FILTER_H
#include "arm_math.h"

extern	uint32_t	my_fblockSize;
extern 	arm_fir_instance_f32 my_fS;		/*	����Ϊȫ�ֱ���	*/
void 	lp_filter_Init(void);

#endif
