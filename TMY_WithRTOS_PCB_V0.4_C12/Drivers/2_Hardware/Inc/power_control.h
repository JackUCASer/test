#ifndef __POWER_CONTROL_H
#define __POWER_CONTROL_H

#include "main.h"

#define LED1_ON()	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,GPIO_PIN_RESET)
#define LED1_OFF()	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,GPIO_PIN_SET)
#define LED1_SW()	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)
#define PWR_OFF()	HAL_GPIO_WritePin(Sys_KEEP_PWR_GPIO_Port, Sys_KEEP_PWR_Pin, GPIO_PIN_RESET)
#define	PWR_ON()	HAL_GPIO_WritePin(Sys_KEEP_PWR_GPIO_Port, Sys_KEEP_PWR_Pin, GPIO_PIN_SET)
#define isPWR_OFF()	HAL_GPIO_ReadPin(Sys_KEY_ON_GPIO_Port, Sys_KEY_ON_Pin)	/*	1: PwrOff; 0: PwrOn	*/


#endif
