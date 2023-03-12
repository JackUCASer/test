#ifndef _MP6570_SPI_H_
#define _MP6570_SPI_H_

#include "stm32f4xx_hal.h"
#include "gpio.h"

#define NSS_SET HAL_GPIO_WritePin(NSS_GPIO_Port,NSS_Pin,GPIO_PIN_SET)
#define NSS_RESET HAL_GPIO_WritePin(NSS_GPIO_Port,NSS_Pin,GPIO_PIN_RESET)

void SPI_ByteWrite(unsigned char addr, unsigned char data1, unsigned char data2);
unsigned short SPI_ByteRead(unsigned char addr);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);
#endif
