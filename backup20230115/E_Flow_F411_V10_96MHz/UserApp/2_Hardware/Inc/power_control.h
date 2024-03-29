#ifndef __POWER_CONTROL_H
#define __POWER_CONTROL_H

#include "main.h"

#define PWR_OFF()			HAL_GPIO_WritePin(Sys_KEEP_PWR_GPIO_Port, Sys_KEEP_PWR_Pin, GPIO_PIN_RESET)
#define	PWR_ON()			HAL_GPIO_WritePin(Sys_KEEP_PWR_GPIO_Port, Sys_KEEP_PWR_Pin, GPIO_PIN_SET)
#define isPWR_KEY_INVALID()	HAL_GPIO_ReadPin(Sys_KEY_ON_GPIO_Port, Sys_KEY_ON_Pin)	/*	1: ����; 0: δ����	*/	
#define ENABLE_CHARGE()		HAL_GPIO_WritePin(CHARGE_CONTROL_GPIO_Port, CHARGE_CONTROL_Pin, GPIO_PIN_SET)	/*	Enable battery charge	*/
#define DISABLE_CHARGE()	HAL_GPIO_WritePin(CHARGE_CONTROL_GPIO_Port, CHARGE_CONTROL_Pin, GPIO_PIN_RESET)	/*	Disable battery charge	*/

#endif
