#include "flash_if.h"
#include "usbd_storage_if.h"

/** 
  * @brief  MEM_If_Init_FS 
  *         Memory initialization routine. 
  * @param  None 
  * @retval 0 if operation is successful, MAL_FAIL else. 
  */  
uint16_t MEM_If_Init_FS(void)  
{    
    HAL_FLASH_Unlock();  
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |  
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);  
  return (USBD_OK);  
} 

/** 
  * @brief  MEM_If_DeInit_FS 
  *         De-Initializes Memory. 
  * @param  None 
  * @retval 0 if operation is successful, MAL_FAIL else. 
  */  
uint16_t MEM_If_DeInit_FS(void)  
{   
    HAL_FLASH_Lock();  
    return (USBD_OK);  
}

static uint32_t GetSector(uint32_t Address)  
{  
    uint32_t sector = 0;  
    if((Address < ADDR_FLASH_SECTOR_1)&&(Address >= ADDR_FLASH_SECTOR_0))  
    {  
        sector = FLASH_SECTOR_0;  
    }  
    else if((Address < ADDR_FLASH_SECTOR_2)&&(Address >= ADDR_FLASH_SECTOR_1))  
    {  
        sector = FLASH_SECTOR_1;  
    }  
    else if((Address < ADDR_FLASH_SECTOR_3)&&(Address >= ADDR_FLASH_SECTOR_2))  
    {  
        sector = FLASH_SECTOR_2;  
    }  
    else if((Address < ADDR_FLASH_SECTOR_4)&&(Address >= ADDR_FLASH_SECTOR_3))  
    {  
        sector = FLASH_SECTOR_3;  
    }  
    else if((Address < ADDR_FLASH_SECTOR_5)&&(Address >= ADDR_FLASH_SECTOR_4))  
    {  
        sector = FLASH_SECTOR_4;  
    }	
    else if((Address < ADDR_FLASH_SECTOR_6)&&(Address >= ADDR_FLASH_SECTOR_5)) 
    {  
        sector = FLASH_SECTOR_5;  
    }  
	else if((Address < ADDR_FLASH_SECTOR_7)&&(Address >= ADDR_FLASH_SECTOR_6)) 
    {  
        sector = FLASH_SECTOR_6;  
    }
	else  
    {  
        sector = FLASH_SECTOR_7;  
    }
    return sector;  
}  


/** 
  * @brief  MEM_If_Erase_FS 
  *         Erase sector. 
  * @param  Add: Address of sector to be erased. 
  * @retval 0 if operation is successful, MAL_FAIL else. 
  */  
uint16_t MEM_If_Erase_FS(uint32_t Add)  
{  
    uint32_t UserStartSector;  
    uint32_t SectorError;  
    FLASH_EraseInitTypeDef pEraseInit;  
    MEM_If_Init_FS();  
    UserStartSector = GetSector(Add);  
    pEraseInit.TypeErase = TYPEERASE_SECTORS;  
    pEraseInit.Sector = UserStartSector;  
    pEraseInit.NbSectors = 2; 		//	擦除连续的N个扇区 
    pEraseInit.VoltageRange = VOLTAGE_RANGE_3;  
    if(HAL_FLASHEx_Erase(&pEraseInit,&SectorError)!=HAL_OK)  
    {  
        return (USBD_FAIL);  
    }     
    return (USBD_OK);  
} 

/** 
  * @brief  MEM_If_Write_FS 
  *         Memory write routine. 
  * @param  src: Pointer to the source buffer. Address to be written to. 
  * @param  dest: Pointer to the destination buffer. 
  * @param  Len: Number of data to be written (in bytes). 
  * @retval 0 if operation is successful, MAL_FAIL else. 
  */  
uint16_t MEM_If_Write_FS(uint8_t *src, uint8_t *dest, uint32_t Len)  
{  
    uint32_t i = 0;   
    for(i = 0; i < Len; i = i + 4)  
    {  
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)(dest + i),*(uint32_t *)(src + i)) == HAL_OK)  
        {  
            if(*(uint32_t *)(src + i) != *(uint32_t *)(dest + i))  
            {  
                return 2;  
            }  
        }  
        else  
        {  
            return 1;  
        }  
    }     
  return (USBD_OK);  
}

/** 
  * @brief  MEM_If_Read_FS 
  *         Memory read routine. 
  * @param  src: Pointer to the source buffer. Address to be written to. 
  * @param  dest: Pointer to the destination buffer. 
  * @param  Len: Number of data to be read (in bytes). 
  * @retval Pointer to the physical address where data should be read. 
  */  
uint8_t *MEM_If_Read_FS (uint8_t *src, uint8_t *dest, uint32_t Len)  
{  
    uint32_t i = 0;  
    uint8_t *psrc = src;  
    for( i = 0; i < Len ; i++ )  
    {  
        dest[i] = *psrc++;  
    }  
    return (uint8_t *)(dest);  
}  

/** 
  * @brief  Flash_If_GetStatus_FS 
  *         Get status routine. 
  * @param  Add: Address to be read from. 
  * @param  Cmd: Number of data to be read (in bytes). 
  * @param  buffer: used for returning the time necessary for a program or an erase operation 
  * @retval 0 if operation is successful 
  */  
uint16_t MEM_If_GetStatus_FS (uint32_t Add, uint8_t Cmd, uint8_t *buffer)  
{  
	/*
    uint16_t FLASH_PROGRAM_TIME = 50;  
    uint16_t FLASH_ERASE_TIME = 50;  
    switch (Cmd)  
    {  
        case DFU_MEDIA_PROGRAM:  
            buffer[1] = (uint8_t)FLASH_PROGRAM_TIME;  
            buffer[2] = (uint8_t)(FLASH_PROGRAM_TIME << 8);  
            buffer[3] = 0;  
        break;  
        case DFU_MEDIA_ERASE:  
        default:  
            buffer[1] = (uint8_t)FLASH_ERASE_TIME;  
            buffer[2] = (uint8_t)(FLASH_ERASE_TIME << 8);  
            buffer[3] = 0;  
    break;  
    }   
	*/                            
  return  (USBD_OK);  
} 
