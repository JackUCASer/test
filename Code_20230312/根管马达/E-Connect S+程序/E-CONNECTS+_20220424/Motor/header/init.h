#ifndef _INIT_H
#define _INIT_H

#include "macros.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"





//spi pin definition
#define SPI_MASTER                   	SPI1
#define SPI_MASTER_CLK               	RCC_APB2Periph_SPI1
#define SPI_MASTER_GPIO              	GPIOA
#define SPI_MASTER_GPIO_CLK          	RCC_APB2Periph_GPIOA  
#define SPI_MASTER_PIN_NSS           	GPIO_Pin_4
#define SPI_MASTER_PIN_SCK           	GPIO_Pin_5
#define SPI_MASTER_PIN_MISO          	GPIO_Pin_6
#define SPI_MASTER_PIN_MOSI          	GPIO_Pin_7

//gpio pin definition
#define GPIO_EN_GPIO              		GPIOB
#define GPIO_EN_GPIO_CLK          		RCC_APB2Periph_GPIOB  
#define GPIO_EN_PIN			          		GPIO_Pin_1

#define GPIO_nSLEEP_GPIO              GPIOB
#define GPIO_nSLEEP_GPIO_CLK          RCC_APB2Periph_GPIOB  
#define GPIO_nSLEEP_PIN			          GPIO_Pin_0

//key1
#define GPIO_KEY1_GPIO              	GPIOA
#define GPIO_KEY1_GPIO_CLK          	RCC_APB2Periph_GPIOA  
#define GPIO_KEY1_PIN			          	GPIO_Pin_0

//key2
#define GPIO_KEY2_GPIO              	GPIOA
#define GPIO_KEY2_GPIO_CLK          	RCC_APB2Periph_GPIOA  
#define GPIO_KEY2_PIN			          	GPIO_Pin_2

//led1
#define GPIO_LED1_GPIO              	GPIOB
#define GPIO_LED1_GPIO_CLK          	RCC_APB2Periph_GPIOB  
#define GPIO_LED1_PIN			          	GPIO_Pin_10

void CKCU_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void SPI1_Configuration(void);
void TIM2_Configuration(void);
void init_peripheral(void);


#endif
