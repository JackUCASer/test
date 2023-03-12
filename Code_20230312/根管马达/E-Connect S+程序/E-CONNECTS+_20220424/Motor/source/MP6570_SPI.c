#include "MP6570_SPI.h"
#include "spi.h"
#include "main.h"
#include "delay.h"

#define u8 uint8_t
#define u16 uint16_t

extern SPI_HandleTypeDef hspi1;
uint8_t d_01=0x01,d_00=0x00;	
u8 SPI1_ReadWriteByte(u8 TxData);

void SPI_ByteWrite(unsigned char addr, unsigned char data1, unsigned char data2)     
{
	u8 byte1,byte2,byte3;   
  
	addr = (addr<<1)  & 0xFE;
	                                                                                                 
	byte1 = (data1>>4) & 0x0F;                                                                                         
	byte2 = (data1<<4) + (data2>>4);                                                                                           
	byte3 = ((data2<<4) & 0xF0);   
	NSS_RESET;

 //HAL_SPI_Transmit(&hspi1,senddata,5,1000);//SPI_I2S_SendData(SPI_MASTER,0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(addr);
	SPI1_ReadWriteByte(byte1);
	SPI1_ReadWriteByte(byte2);
	SPI1_ReadWriteByte(byte3);
	delay_us(1);
	NSS_SET;
	
}

u16 SPI_ByteRead(unsigned char addr)                                                
{
	u8 byte_Low,byte_High, byte1,byte2, byte3;	
	u16 data;    
  
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
	data = ((u16)byte_High<<8) +(u16)byte_Low;                                                                                 //75ns
	return data;
}

u8 SPI1_ReadWriteByte(u8 TxData)
 {        
	uint8_t Rxdata;                
	HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 0xFFFF);       
 	return Rxdata;          		    //返回收到的数据		                  
}
