#include "stm32f4xx_hal.h"
#include "eeprom.h"
 

void delay_t(int t);
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
static void I2C_Delay(void)
{
    uint8_t i;
    for(i=0; i<10; i++);
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void I2C_Start(void)
{
    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
    I2C_Delay();
    I2C_SDA_LOW();
    I2C_Delay();
    I2C_SCL_LOW();
    I2C_Delay();

}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void I2C_Stop(void)
{
    I2C_SDA_LOW();
    I2C_SCL_HIGH();
    I2C_Delay();
    I2C_SDA_HIGH();

}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for(i=0; i<8; i++)
    {
        if(Byte&0x80) {
            I2C_SDA_HIGH();
        }
        else {
            I2C_SDA_LOW();
        }
        I2C_Delay();
        I2C_SCL_HIGH();
        I2C_Delay();
        I2C_SCL_LOW();
        if(i==7) {
            I2C_SDA_HIGH();
        }
        Byte<<=1;
        I2C_Delay();

    }
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
uint8_t I2C_ReadByte(void)
{
    uint8_t i,vaule;
    vaule=0;
    for(i=0; i<8; i++) {
        vaule<<=1;
        I2C_SCL_HIGH();
        I2C_Delay();
        if(I2C_SDA_READ()) {
            vaule++;
        }
        I2C_SCL_LOW();
        I2C_Delay();
    }
    return vaule;

}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
uint8_t I2C_WaitAck(void)
{

    uint8_t ErrTime=0;
    I2C_SDA_HIGH();
    I2C_Delay();
    I2C_SCL_HIGH();
    I2C_Delay();
    while(I2C_SDA_READ())
    {
        ErrTime++;
        if(ErrTime>250)
        {
            I2C_Stop();
            return 1;
        }

    }
    I2C_SCL_LOW();
    I2C_Delay();
    return 0;

}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void I2C_Ack()
{
    I2C_SDA_LOW();
    I2C_Delay();
    I2C_SCL_HIGH();
    I2C_Delay();
    I2C_SCL_LOW();
    I2C_Delay();
    I2C_SDA_HIGH();
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void I2C_NAck()
{
    I2C_SDA_HIGH();
    I2C_Delay();
    I2C_SCL_HIGH();
    I2C_Delay();
    I2C_SCL_LOW();
    I2C_Delay();

}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
uint8_t I2C_CheckDev(uint8_t address)
{
    uint8_t ucAck;
    I2C_Start();
    I2C_SendByte(address|I2C_WR);
    ucAck=I2C_WaitAck();
    I2C_Stop();
    return ucAck;

}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
uint8_t EEPROM_Check_Ok()
{
    if(I2C_CheckDev(EEPROM_DEV_ADDR)==0) {
        return 1;
    } else {
        I2C_Stop();
        return 0;
    }
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
uint8_t EEPROM_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp=0;

    I2C_Start();
    if(EE_TYPE>AT24C16)
    {
        I2C_SendByte(0XA0);	   //发送写命令
        if(I2C_WaitAck()==1)temp=0;
        I2C_SendByte(ReadAddr>>8);//发送高地址
    } else
        I2C_SendByte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数?

    if(I2C_WaitAck()==1)temp=0;
    I2C_SendByte((uint8_t)(ReadAddr%256));   //发送低地址

    //  printf("%d\t",c_l);
    if(I2C_WaitAck()==1)temp=0;
    I2C_Start();
    I2C_SendByte(0XA1+((ReadAddr/256)<<1));           //进入接收模式
    if(I2C_WaitAck()==1)temp=0;
    temp=I2C_ReadByte();
    I2C_Stop();//产生一个停止条件
    return temp;
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
//返回 1 成功 返回0 失败
uint8_t EEPROM_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
    uint8_t re=1;

    I2C_Start();
    if(EE_TYPE>AT24C16)
    {
        I2C_SendByte(0XA0);	    //发送写命令
        if(I2C_WaitAck()==1)re=0;


        I2C_SendByte(WriteAddr>>8);//发送高地址
    } else
        I2C_SendByte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据


    if(I2C_WaitAck()==1)re=0;
    I2C_SendByte((uint8_t)(WriteAddr%256));   //发送低地址



    if(I2C_WaitAck()==1)re=0;
    I2C_SendByte(DataToWrite);     //发送字节
    if(I2C_WaitAck()==1)re=0;
    I2C_Stop();//产生一个停止条件

    delay_t(300);
    return re;
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
uint8_t EEPROM_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{
    uint8_t t,re;
    for(t=0; t<Len; t++)
    {
        re=EEPROM_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    }
    return re;
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址
//返回值     :数据
//Len        :要读出数据的长度2,4
uint32_t EEPROM_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{
    uint8_t t;
    uint32_t temp=0;
    for(t=0; t<Len; t++)
    {
        temp<<=8;
        temp+=EEPROM_ReadOneByte(ReadAddr+Len-t-1);
    }
    return temp;
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
uint8_t EEPROM_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{

    while(NumToRead)
    {
        (*pBuffer++)=EEPROM_ReadOneByte(ReadAddr++);
        NumToRead--;
        /********************/
        
    }
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
//返回 1 成功；返回0失败
uint8_t EEPROM_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
    uint8_t re;
    while(NumToWrite--)
    {
        re=EEPROM_WriteOneByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
        /********************/
        
    }
    return re;
}

void delay_t(int t)
{
    int i=0,j=0;
    for(i=0; i<800; i++)
        for(j=0; j<t; j++)
        {
            ;
        }

}