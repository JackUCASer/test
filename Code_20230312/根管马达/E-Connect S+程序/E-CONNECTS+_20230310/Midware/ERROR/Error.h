#ifndef __ERROR_H
#define __ERROR_H

#include "stm32f4xx_hal.h"
#include "datatype.h"
#include "work_mode.h"

typedef enum
{
    e_NoFault=0,
    e_OverCurrent,
    e_LowPower,
} Fault_Num;                 //´íÎó´úÂë


void CheckLowBattery();
void OverCurentProtect();
WorkMode_e  CheckHardwareFault(WorkMode_e  mode);




#endif