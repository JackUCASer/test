#ifndef __KEY_TASK_H
#define __KEY_TASK_H

#include "work_mode.h"
#include "stm32f4xx_hal.h"

#define EN 0
#define CN 1

WorkMode_e CheckKey_StandbyMode(WorkMode_e mode);
WorkMode_e CheckKey_RunMode(WorkMode_e mode);
WorkMode_e CheckKey_MeaMode(WorkMode_e mode);
WorkMode_e CheckKey_RunWithApexMode(WorkMode_e mode);
WorkMode_e CheckKey_DispOffMode(WorkMode_e mode);

uint8_t CheckKey_Password(uint8_t pw);
void Jump2Menu(uint8_t Index);
uint8_t Get_Index();
void NextOperate(uint8_t Index);
void  InitLastIndex();

#endif