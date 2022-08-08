#include "mp6570.h"
#include "main.h"
#include "delay.h"
#include "spi.h"
#include "control.h"
#include "common.h"
#include "macros.h"
#include "MP6570_SPI.h"

uint16_t reg[128] = 
{
	0x6570,	//0x00
	0x8000,	//0x01
	0x0000,	//0x02
	0x0000,	//0x03
	0x0101,	//0x04  necessary
	0xFE00,	//0x05  necessary
	0x205D,	//0x06  necessary
	0x02FB,	//0x07  necessary
	0x00C8,	//0x08
	0x00C8,	//0x09
	0x7FF8,	//0x0A
	0xF833,	//0x0B
	0xF847,	//0x0C
	0x7171,	//0x0D
	0x71b5,	//0x0E
	0x014A,	//0x0F
	
	0x0000,	//0x10
	0x0000,	//0x11
	0x0FA0,	//0x12
	0x1F40,	//0x13
	0x0000,	//0x14
	0x0000,	//0x15
	0x0000,	//0x16
	0x0FA0,	//0x17 necessary
	0x0F50,	//0x18 necessary
	0x0000,	//0x19 necessary
	0x0F90,	//0x1A necessary
	0x5050,	//0x1B necessary
	0x0064,	//0x1C 
	0x0064,	//0x1D
	0x0421,	//0x1E
	0x0421,	//0x1F
		
	0x0BB8,	//0x20
	0x00B0,	//0x21
	0x0001,	//0x22
	0x00C8,	//0x23
	0x00C8,	//0x24
	0x00C6,	//0x25
	0x0000,	//0x26
	0x03FF,	//0x27  ocp necessary
	0x021C,	//0x28  ovp necessary
	0x0130,	//0x29  
	0x0480,	//0x2A
	0x0FB1,	//0x2B
	0x0064,	//0x2C
	0x0000,	//0x2D
	0x0000,	//0x2E
	0x0000,	//0x2F
	
	0x000F,	//0x30  necessary
	0x0000,	//0x31
	0x0040,	//0x32
	0xE003,	//0x33  necessary
	0x015A,	//0x34, baud rate, 40000000/fbps - 1
	0x2000,	//0x35
	0x0000,	//0x36
	0x3FF4,	//0x37
	0xE000,	//0x38
	0x0000,	//0x39
	0x0000,	//0x3A
	0x0000,	//0x3B
	0x0000,	//0x3C
	0x0000,	//0x3D
	0x0000,	//0x3E
	0x0000,	//0x3F
	
	0x0000,	//0x40
	0x0000,	//0x41
	0x0000,	//0x42
	0x0000,	//0x43
	0x0000,	//0x44
	0x0000,	//0x45
	0x0000,	//0x46
	0x0000,	//0x47
	0x0000,	//0x48
	0x0000,	//0x49
	0x0000,	//0x4A
	0x0000,	//0x4B
	0x0000,	//0x4C
	0x0000,	//0x4D
	0x0000,	//0x4E
	0x0000,	//0x4F
	
	0x0000,	//0x50
	0x0000,	//0x51
	0x0000,	//0x52
	0x0000,	//0x53
	0x0000,	//0x54
	0x0000,	//0x55
	0x0000,	//0x56
	0x0000,	//0x57
	0x0000,	//0x58
	0x0000,	//0x59
	0x0000,	//0x5A
	0x0000,	//0x5B
	0x0000,	//0x5C
	0x0000,	//0x5D
	0x0000,	//0x5E
	0x0000,	//0x5F
	
	0x0000,	//0x60
	0x0000,	//0x61
	0x0000,	//0x62
	0x0000,	//0x63
	0x0000,	//0x64
	0x0000,	//0x65
	0x0000,	//0x66
	0x0000,	//0x67
	0x0000,	//0x68
	0x0000,	//0x69
	0x0000,	//0x6A
	0x0000,	//0x6B
	0x0000,	//0x6C
	0x0000,	//0x6D
	0x0000,	//0x6E
	0x0000,	//0x6F
};

void mp6570_init(unsigned char slave_addr)
{
	MP6570_WriteRegister(slave_addr, 0x62, 0x0000);//Turn on MP6570 internal Clock
	delay_ms(2);												//wait for internal Clock ON
	MP6570_WriteRegister(slave_addr, 0x04, m_p); //pole pair	
	MP6570_WriteRegister(slave_addr, 0x10, 0x0000); //ID_REF
	MP6570_WriteRegister(slave_addr, 0x11, 0x0001); //IQ_REF
	MP6570_WriteRegister(slave_addr, 0x12, regi[0x2C]); //CURRENT_KP
	MP6570_WriteRegister(slave_addr, 0x13, regi[0x2B]); //CURRENT_KI
	
	int PERIOD = 80000/FS;
	uint16_t MaxOnTime = 0;
	if(PERIOD >= 32768) //fix low side on time to 1us
		MaxOnTime = PERIOD - 10;
	else if(PERIOD >=16384)
		MaxOnTime = PERIOD - 20;
	else if(PERIOD >=8192)
		MaxOnTime = PERIOD - 40;
	else if(PERIOD >=4096)
		MaxOnTime = PERIOD - 80;
	else
		MaxOnTime = PERIOD - 160;
	uint16_t deadtime = DEADTIME/25;
	
	MP6570_WriteRegister(slave_addr, 0x17, PERIOD); 						//PERIOD
	MP6570_WriteRegister(slave_addr, 0x18, MaxOnTime); 						//MAX_PERIOD
	MP6570_WriteRegister(slave_addr, 0x19, deadtime); 						//{SH_DELAY, DTPWM}
	MP6570_WriteRegister(slave_addr, 0x1A, PERIOD-0); 						//SH_CMP
	//Íù¸´Ð±ÂÊ
//	MP6570_WriteRegister(slave_addr, 0x1C, 0x0A); 						//SH_CMP
//	MP6570_WriteRegister(slave_addr, 0x1D, 0x0A); 						//SH_CMP
//	MP6570_WriteRegister(slave_addr, 0x1F, 0x3FF); 						//SH_CMP
//	MP6570_WriteRegister(slave_addr, 0x1E, 0x3E0); 						//SH_CMP
	
	MP6570_WriteRegister(slave_addr, 0x27, regi[0x5A]); 					//I_OCP
	MP6570_WriteRegister(slave_addr, 0x28, 0x021C); 						//FAULT
	MP6570_WriteRegister(slave_addr, 0x29, regi[0x56]); 					//OVP
	MP6570_WriteRegister(slave_addr, 0x30, (AD_MOD<<3) + AD_GAIN); 			//{AD_MOD, AD_GAIN}
	MP6570_WriteRegister(slave_addr, 0x33, 0xA003|(PWMMODE<<7)); 			//MODE....
	MP6570_WriteRegister(slave_addr, 0x37, 0x3FF4); 						//KALMAN_N
	MP6570_WriteRegister(slave_addr, 0x38, 0xE000); 						//sensor inital time
	MP6570_WriteRegister(slave_addr, 0x39, 0x0000); 						//sensor control bits	
	MP6570_WriteRegister(slave_addr, 0x50, 0x0000); 						//{THETA_GEN_TIME[7:0], THETA_GEN_EN, mem_section[1:0]}		
	MP6570_WriteRegister(slave_addr, 0x51, 0x0000); 						//{OPEN_LOOP_DBG, UD_SET_DBG[11:0]}				
	MP6570_WriteRegister(slave_addr, 0x52, 0x0000); 						//{UQ_SET_DBG[11:0], THETA_BIAS_EN}	
}




/*********************************************************************************************************//**
  * @brief  Initial theta bias auto detect 
	* @param  None 
  * @retval None
  ***********************************************************************************************************/
uint8_t MP6570_AutoThetaBias(unsigned char slave_addr, uint16_t Ihold_mA , uint16_t Thold_ms)//current(mA)?interval(ms)
{
	uint8_t INL_STATUS = 0;	
	uint16_t i55,i55_2;
	int i07_temp;
	uint16_t CMPA_SET,x_theta_dir;
	LOOPTIMER_DISABLE;											//Turn off SCTM1
	HAL_Delay(100);
	nSLEEP_SET;
	HAL_Delay(100);
	MP6570_WriteRegister(slave_addr, 0x62, 0);					//turn internal clock on
	uint16_t i54 = MP6570_ReadRegister(slave_addr, 0x54);			//read register 0x54 to determine if MP6570 is at ON or OFF state
	uint16_t i33 = MP6570_ReadRegister(slave_addr, 0x33);			//read register 0x33 to determine standby or not-standby mode
	if (i54 != 0x00)
	{
		if ((i33 & 0x8000) == 0x8000)							//set MP6570 to OFF state
    	MP6570_WriteRegister(slave_addr, 0x61, 0);
		else
     	MP6570_WriteRegister(slave_addr, 0x60, 0);
	}
	
	uint16_t i05 = MP6570_ReadRegister(slave_addr, 0x05);			//read register 0x05
	reg[0x05] = i05 & 0x7fff; 
	MP6570_WriteRegister(slave_addr, 0x05, reg[0x05]);			//set THETA_DIR = 0
	i05 = MP6570_ReadRegister(slave_addr, 0x05);
	MP6570_WriteRegister(slave_addr, 0x20 , 100);	      		//set SPEED_CMD !=0
	MP6570_WriteRegister(slave_addr, 0x11,1); 					//set iq_ref !=0
	
	CMPA_SET =Ihold_mA * regi[0x01]*2/1000 * 40 / 100 / regi[0x06];
	MP6570_WriteRegister(slave_addr, 0x51, CMPA_SET);    		//write CMPA_SET to register 51H Bit0~Bit11
	uint16_t THETA_BIAS_TIME=(uint16_t)(Thold_ms*0.1);	     			// calculate THETA_BIAS_TIME value
	MP6570_WriteRegister(slave_addr, 0x52,(THETA_BIAS_TIME<<1) + 1); 	//write THETA_BIAS_TIME to register 0x52 and set the THETA_BIAS_EN bit
	
	if ((i33 & 0x8000) == 0x8000)								// set MP6570 to ON state
		MP6570_WriteRegister(slave_addr, 0x60,0);
	else
		MP6570_WriteRegister(slave_addr, 0x61,0);
	
	HAL_Delay( Thold_ms * 0.6);									//delay 0.8 x Thold_ms until the motor is steady at position 1 
	i55 = MP6570_ReadRegister(slave_addr, 0x55);				//read the angle at position 1
	HAL_Delay( Thold_ms * 1);									//delay Thold_ms until the motor is steady at position 2
	i55_2 = MP6570_ReadRegister(slave_addr, 0x55);				//read the angle at postion 2
	HAL_Delay( Thold_ms * 0.2 + 100);							//delay (Thold_ms * 0.2 +100) to let the auto-theta-bias procedure finish.
	//set MP6570 to OFF state
	if ((i33 & 0x8000) == 0x8000) 				
		MP6570_WriteRegister(slave_addr, 0x61, 0);
	else
		MP6570_WriteRegister(slave_addr, 0x60, 0);

	MP6570_WriteRegister(slave_addr, 0x52, 0);					//exit auto-theta-bias procedure
																//calculate the THETA_DIR and THETA_BIAS
	if (((i55 > i55_2) && ((i55 - i55_2) < 0X3FFF)))  			// decrease and not over a turn (the zero deg transition)
	{
		x_theta_dir = 1;
		i07_temp = 65536 -((i55 + i55_2)>>1);
	}
	else if ((i55 < i55_2) && (i55_2 - i55 > 0X3FFF)) 			// decrease and over a turn (the zero deg transition)
	{
		x_theta_dir = 1;
		i07_temp = (i55 + 65536 + i55_2)>>1;
		if (i07_temp > 65536)
		{
			i07_temp = i07_temp - 65535;
		}
		i07_temp = 65536- i07_temp;
	}
	else if ((i55 > i55_2) && (i55 - i55_2 > 0X3FFF))			//increase and over a turn (the zero deg transition)
	{
		x_theta_dir = 0;
		i07_temp = (i55 + 65536 + i55_2)>>1;
		if (i07_temp > 65535)
		{
			i07_temp = i07_temp - 65535;
		}
	}
	else														// increasing and not over a turn (the zero deg transition)
	{
		x_theta_dir = 0;
		i07_temp = (i55 + i55_2)>>1;
	}
	
	
	reg[0x07] = i07_temp>>5;
	reg[0x05] = (reg[0x05] | (x_theta_dir<<15));
	MP6570_WriteRegister(0x00,0x05, reg[0x05]);
	MP6570_WriteRegister(0x00,0x07, reg[0x07]);
	MP6570_WriteRegister(0x00,0x64, 0x0000);
	HAL_Delay(2000);
	SPI_ByteWrite(0x60,0x00,0x00); //Turn on MP6570  
	HAL_Delay(100);
	LOOPTIMER_ENABLE;
	reset_parameters();
	INL_STATUS = 1;
	return INL_STATUS;
}

void MP6570_WriteRegister(unsigned char slave_addr, unsigned char reg_addr,uint16_t data)
{
	uint8_t data1, data2;
	reg[reg_addr] = data;
	data2 = (uint8_t)data;
	data1 = (uint8_t)(data>>8);
	SPI_ByteWrite(reg_addr, data1, data2);
}

uint16_t MP6570_ReadRegister(unsigned char slave_addr, unsigned char reg_addr)
{
	uint16_t temp;
	temp = SPI_ByteRead(reg_addr);
	reg[reg_addr] = temp;
	return temp;
}
