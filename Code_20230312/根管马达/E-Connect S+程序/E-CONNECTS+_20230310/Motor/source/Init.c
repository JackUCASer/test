#include "init.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
void CKCU_Configuration(void)
{
  RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | GPIO_EN_GPIO_CLK |GPIO_nSLEEP_GPIO_CLK|
                         RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//initialize EN & nSLEEP pin
	GPIO_InitStructure.GPIO_Pin = GPIO_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_EN_GPIO, &GPIO_InitStructure);
	
	//initialize nSLEEP pin
  GPIO_InitStructure.GPIO_Pin = GPIO_nSLEEP_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_nSLEEP_GPIO, &GPIO_InitStructure);	
	
	//initialize key1, key2 pin
	GPIO_InitStructure.GPIO_Pin = GPIO_KEY1_PIN | GPIO_KEY2_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_KEY1_GPIO, &GPIO_InitStructure);	
	
	//initialize led1
	GPIO_InitStructure.GPIO_Pin = GPIO_LED1_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED1_GPIO, &GPIO_InitStructure);	
}

void SPI1_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//sck, mosi pin config
	GPIO_InitStructure.GPIO_Pin = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);
	
	//nss pin config
	GPIO_InitStructure.GPIO_Pin = SPI_MASTER_PIN_NSS ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);
	
	//miso config
	GPIO_InitStructure.GPIO_Pin = SPI_MASTER_PIN_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);	
	
	SPI_InitTypeDef SPI_InitStructure;
	  /* SPI_MASTER configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI_MASTER, &SPI_InitStructure);
	SPI_SSOutputCmd(SPI_MASTER, ENABLE);
	/* Enable SPI_MASTER */
  SPI_Cmd(SPI_MASTER, ENABLE);
	
}

void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 99; //1us 
	TIM_TimeBaseStructure.TIM_Prescaler = 71; //1M clock
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);
	/* TIM IT enable */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM2->CNT = 0;
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //enable overflow interrupt
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void init_peripheral()
{
	SystemInit();
	NVIC_Configuration();
	CKCU_Configuration();
	
	TIM2_Configuration();
	GPIO_Configuration();
	SPI1_Configuration();
	NSS_SET;  //set nss (chip select pin to high)
}
