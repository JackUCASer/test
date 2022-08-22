#include "MP6570_SPI.h"
#include "spi.h"
#include "main.h"
#include "delay.h"

extern SPI_HandleTypeDef hspi1;

uint8_t d_01=0x01,d_00=0x00;	
uint8_t SPI1_ReadWriteByte(uint8_t TxData);

void SPI_ByteWrite(unsigned char addr, unsigned char data1, unsigned char data2)     
{
	uint8_t byte1,byte2,byte3;   
  
	addr = (addr<<1)  & 0xFE;
	                                                                                                 
	byte1 = (data1>>4) & 0x0F;                                                                                         
	byte2 = (data1<<4) + (data2>>4);                                                                                           
	byte3 = ((data2<<4) & 0xF0);   
	NSS_RESET;

	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(addr);
	SPI1_ReadWriteByte(byte1);
	SPI1_ReadWriteByte(byte2);
	SPI1_ReadWriteByte(byte3);
	delay_us(1);
	NSS_SET;
	
}

uint16_t SPI_ByteRead(unsigned char addr)                                                
{
	uint8_t byte_Low,byte_High, byte1,byte2, byte3;	
	uint16_t data;    
  
	addr = (addr<<1)  & 0xFE;
	
	//	addr = (addr<<1)| 0x01;   //100ns
	NSS_RESET;//SPI_NSSInternalSoftwareConfig(SPI_MASTER, SPI_NSSInternalSoft_Reset);
	delay_us(1);
	SPI1->DR = 0x01; 
	
	while((SPI1->SR & 1<<1) == 0);		//wait for tx buf empty
	SPI1->DR = addr; 
	
	while((SPI1->SR&1<<0)==0);   		//等待接收完一个byte  
	byte1= SPI1->DR; 
	
	while((SPI1->SR & 1<<1) == 0);		//wait for tx buf empty
	SPI1->DR = 0x00; 
	
	while((SPI1->SR&1<<0)==0);   		//等待接收完一个byte  
	byte1= SPI1->DR; 
	
	while((SPI1->SR & 1<<1) == 0);		//wait for tx buf empty
	SPI1->DR = 0x00; 
	
	while((SPI1->SR&1<<0)==0);   		//等待接收完一个byte  
	byte1= SPI1->DR; 

	while((SPI1->SR & 1<<1) == 0);		//wait for tx buf empty
	SPI1->DR = 0x00; 

	while((SPI1->SR&1<<0)==0);   		//等待接收完一个byte  
	byte2= SPI1->DR; 
	
	while((SPI1->SR&1<<0)==0);   		//等待接收完一个byte  
	byte3= SPI1->DR; 
	delay_us(1);
	
	NSS_SET;
	byte_Low = (byte2<<4)+(byte3>>4);                             
	byte_High = (byte1<<4)+(byte2>>4);
	data = ((uint16_t)byte_High<<8) +(uint16_t)byte_Low;                                                                                 //75ns
	return data;
}

uint8_t SPI1_ReadWriteByte(uint8_t TxData)
 {        
	uint8_t Rxdata;                
	HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 0xFFFF);       
 	return Rxdata;          		    //返回收到的数据		                  
}
