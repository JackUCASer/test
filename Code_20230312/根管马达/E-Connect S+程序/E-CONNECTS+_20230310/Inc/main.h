/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SAM_5V_Pin GPIO_PIN_15
#define SAM_5V_GPIO_Port GPIOC
#define BAT_SAM_Pin GPIO_PIN_0
#define BAT_SAM_GPIO_Port GPIOA
#define SAM_8KHZ_Pin GPIO_PIN_1
#define SAM_8KHZ_GPIO_Port GPIOA
#define SAM_400HZ_Pin GPIO_PIN_2
#define SAM_400HZ_GPIO_Port GPIOA
#define KEEP_PWR_Pin GPIO_PIN_3
#define KEEP_PWR_GPIO_Port GPIOA
#define KEY_ON_Pin GPIO_PIN_4
#define KEY_ON_GPIO_Port GPIOA
#define NSS_Pin GPIO_PIN_0
#define NSS_GPIO_Port GPIOB
#define MP6570_EN_Pin GPIO_PIN_1
#define MP6570_EN_GPIO_Port GPIOB
#define LCD_RES_Pin GPIO_PIN_2
#define LCD_RES_GPIO_Port GPIOB
#define LCD_A0_Pin GPIO_PIN_10
#define LCD_A0_GPIO_Port GPIOB
#define LCD_SCK_Pin GPIO_PIN_13
#define LCD_SCK_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_14
#define SPI2_NSS_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOB
#define EEPROM_SDA_Pin GPIO_PIN_8
#define EEPROM_SDA_GPIO_Port GPIOA
#define EEPROM_SCL_Pin GPIO_PIN_9
#define EEPROM_SCL_GPIO_Port GPIOA
#define ROOT_PWM_Pin GPIO_PIN_10
#define ROOT_PWM_GPIO_Port GPIOA
#define BL_PWM_Pin GPIO_PIN_15
#define BL_PWM_GPIO_Port GPIOA
#define RELAY_Pin GPIO_PIN_3
#define RELAY_GPIO_Port GPIOB
#define BEEP_PWM_Pin GPIO_PIN_4
#define BEEP_PWM_GPIO_Port GPIOB
#define KEY_SEL_Pin GPIO_PIN_5
#define KEY_SEL_GPIO_Port GPIOB
#define KEY_DOWN_Pin GPIO_PIN_6
#define KEY_DOWN_GPIO_Port GPIOB
#define KEY_UP_Pin GPIO_PIN_7
#define KEY_UP_GPIO_Port GPIOB
#define VM_CTL_Pin GPIO_PIN_9
#define VM_CTL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
