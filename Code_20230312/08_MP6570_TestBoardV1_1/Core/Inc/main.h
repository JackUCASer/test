/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

#include "stm32f4xx_ll_exti.h"

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
#define PWM_A_Pin GPIO_PIN_0
#define PWM_A_GPIO_Port GPIOA
#define PWM_B_Pin GPIO_PIN_1
#define PWM_B_GPIO_Port GPIOA
#define PWM_C_Pin GPIO_PIN_2
#define PWM_C_GPIO_Port GPIOA
#define CS1_Pin GPIO_PIN_3
#define CS1_GPIO_Port GPIOA
#define CS2_Pin GPIO_PIN_4
#define CS2_GPIO_Port GPIOA
#define CS3_Pin GPIO_PIN_5
#define CS3_GPIO_Port GPIOA
#define BAT_VOL_Pin GPIO_PIN_6
#define BAT_VOL_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_7
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_0
#define KEY2_GPIO_Port GPIOB
#define KEY3_Pin GPIO_PIN_1
#define KEY3_GPIO_Port GPIOB
#define KEY4_Pin GPIO_PIN_2
#define KEY4_GPIO_Port GPIOB
#define MP6570_CLK_Pin GPIO_PIN_10
#define MP6570_CLK_GPIO_Port GPIOB
#define MP6570_CS_Pin GPIO_PIN_12
#define MP6570_CS_GPIO_Port GPIOB
#define MP6570_EN_Pin GPIO_PIN_13
#define MP6570_EN_GPIO_Port GPIOB
#define MP6570_MISO_Pin GPIO_PIN_14
#define MP6570_MISO_GPIO_Port GPIOB
#define MP6570_MOSI_Pin GPIO_PIN_15
#define MP6570_MOSI_GPIO_Port GPIOB
#define M043_DIN_Pin GPIO_PIN_9
#define M043_DIN_GPIO_Port GPIOA
#define M043_DOUT_Pin GPIO_PIN_10
#define M043_DOUT_GPIO_Port GPIOA
#define SENSOR_SCL_Pin GPIO_PIN_6
#define SENSOR_SCL_GPIO_Port GPIOB
#define SENSOR_SDA_Pin GPIO_PIN_7
#define SENSOR_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
