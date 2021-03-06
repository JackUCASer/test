#ifndef __EEPROM_H
#define __EEPROM_H

//#include "datatype.h"
#include "main.h"

#define I2C_WR 				0
#define I2C_RD 				1
#define I2C_GPIO_CLK_EN

/*	BEEP 引脚高电平模拟开关:
		Y+  <---> D+	
		Y-  <---> D-
	此时，Y+(HX711_DOUT)连接EEPROM SDA引脚
		  Y-(HX711_SCK)连接EEPROM SCL引脚
*/

#define I2C_SCL_HIGH() 	HAL_GPIO_WritePin(HX711_SCK_GPIO_Port,HX711_SCK_Pin,GPIO_PIN_SET)
#define I2C_SCL_LOW() 	HAL_GPIO_WritePin(HX711_SCK_GPIO_Port,HX711_SCK_Pin,GPIO_PIN_RESET)

#define I2C_SDA_HIGH() 	HAL_GPIO_WritePin(HX711_DOUT_GPIO_Port,HX711_DOUT_Pin,GPIO_PIN_SET)
#define I2C_SDA_LOW() 	HAL_GPIO_WritePin(HX711_DOUT_GPIO_Port,HX711_DOUT_Pin,GPIO_PIN_RESET)
#define I2C_SDA_READ() 	HAL_GPIO_ReadPin(HX711_DOUT_GPIO_Port,HX711_DOUT_Pin)

#define EEPROM_DEV_ADDR 	0XA0
#define EEPROM_PAGE_SIZE 	8
#define EEPROM_SIZE 		1024

#define AT24C01 			127
#define AT24C02 			255
#define AT24C04 			511
#define AT24C08 			1023
#define AT24C16 			2047
#define AT24C32 			4095
#define AT24C64 			8191
#define AT24C128 			16383
#define AT24C256 			32767

//定义EE_TYPE为AT24C32
#define EE_TYPE AT24C32


uint8_t EEPROM_Check_Ok(void);
uint8_t I2C_CheckDev(uint8_t address);
uint8_t EEPROM_ReadOneByte(uint16_t ReadAddr);
uint8_t EEPROM_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);

uint8_t EEPROM_ReadBytes(uint8_t *ReadBuf, uint16_t Address, uint16_t Size); 			//old
uint8_t EEPROM_WriteBytes(uint8_t *WriteBuf, uint16_t Address, uint16_t Size); 			//old

uint8_t EEPROM_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite); 			//写入缓存区
uint8_t EEPROM_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead); 			//读到缓存区
uint32_t EEPROM_ReadLenByte(uint16_t ReadAddr,uint8_t Len); 							//多字节读取，单字节合并成多字节
uint8_t EEPROM_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len); 		//多字节写入，多字节分解成单字节

#endif 
 


