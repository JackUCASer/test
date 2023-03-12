#include "datatype.h"
#include "stm32f4xx_hal.h"
#include "work_mode.h"

void SysClock(void);               //
void ClearClock(void);                 //
void ClearMove();
WorkMode_e Clock_Mode(WorkMode_e mode);         //