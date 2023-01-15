#ifndef __POWER_CONTROL_H
#define __POWER_CONTROL_H

#include "main.h"

#define PWR_OFF()			HAL_GPIO_WritePin(Sys_KEEP_PWR_GPIO_Port, Sys_KEEP_PWR_Pin, GPIO_PIN_RESET)
#define	PWR_ON()			HAL_GPIO_WritePin(Sys_KEEP_PWR_GPIO_Port, Sys_KEEP_PWR_Pin, GPIO_PIN_SET)

#endif
