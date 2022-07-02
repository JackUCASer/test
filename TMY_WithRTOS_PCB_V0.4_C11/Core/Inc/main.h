/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define Sys_KEY_ON_Pin GPIO_PIN_14
#define Sys_KEY_ON_GPIO_Port GPIOC
#define Sys_KEEP_PWR_Pin GPIO_PIN_15
#define Sys_KEEP_PWR_GPIO_Port GPIOC
#define USB_P_IN_Pin GPIO_PIN_0
#define USB_P_IN_GPIO_Port GPIOA
#define BATVOL_Pin GPIO_PIN_1
#define BATVOL_GPIO_Port GPIOA
#define M043_DIN_Pin GPIO_PIN_2
#define M043_DIN_GPIO_Port GPIOA
#define M043_DOUT_Pin GPIO_PIN_3
#define M043_DOUT_GPIO_Port GPIOA
#define Hall_LIN_Pin GPIO_PIN_4
#define Hall_LIN_GPIO_Port GPIOA
#define HX711_DOUT_Pin GPIO_PIN_5
#define HX711_DOUT_GPIO_Port GPIOA
#define HX711_SCK_Pin GPIO_PIN_6
#define HX711_SCK_GPIO_Port GPIOA
#define BEEP_Pin GPIO_PIN_7
#define BEEP_GPIO_Port GPIOA
#define W25_SPI_CLK_Pin GPIO_PIN_13
#define W25_SPI_CLK_GPIO_Port GPIOB
#define W25_SPI_MISO_Pin GPIO_PIN_14
#define W25_SPI_MISO_GPIO_Port GPIOB
#define W25_SPI_MOSI_Pin GPIO_PIN_15
#define W25_SPI_MOSI_GPIO_Port GPIOB
#define W25_SPI_CS_Pin GPIO_PIN_8
#define W25_SPI_CS_GPIO_Port GPIOA
#define MP6570_CS_Pin GPIO_PIN_15
#define MP6570_CS_GPIO_Port GPIOA
#define MP6570_SCLK_Pin GPIO_PIN_3
#define MP6570_SCLK_GPIO_Port GPIOB
#define MP6570_MISO_Pin GPIO_PIN_4
#define MP6570_MISO_GPIO_Port GPIOB
#define MP6570_MOSI_Pin GPIO_PIN_5
#define MP6570_MOSI_GPIO_Port GPIOB
#define MP6570_EN_Pin GPIO_PIN_6
#define MP6570_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
