#ifndef __ARM_PID_H
#define __ARM_PID_H
#include "arm_math.h"

void arm_pid_Init(void);
int32_t arm_pid_Calu(int32_t in);
void arm_pid_DeInit(void);

#endif
