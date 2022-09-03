/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v1.0_Cube
  * @brief          : Memory management layer.
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage_if.h"

/* USER CODE BEGIN INCLUDE */
#include "platform_config.h"		//add usb
#include "flash_if.h"
#include "AES.h"

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  1
#define STORAGE_BLK_NBR                  0x10000
#define STORAGE_BLK_SIZ                  0x200

/* USER CODE BEGIN PRIVATE_DEFINES */
//add usb
//#define RAMDISK_SIZE    STORAGE_BLK_NBR*STORAGE_BLK_SIZ  /* ��FAT��*/
#define RAMDISK_SIZE 	30*1024
#define SECTOR_SIZE     512     /* һ�����ݴ�С��boot����512λ��λ�ְ�*/
uint8_t RAM_DISK[RAMDISK_SIZE]={0};
uint32_t Mass_Memory_Size;
uint32_t Mass_Block_Size;
uint32_t Mass_Block_Count;
__align(4) uint8_t  FLASH_TMP_BUF[IN_FLASH_SECTOR];      //bin�ļ����棬һ������
uint8_t  KEY[32]="Eighteethisthebestindentalworld!";
extern int flag_iap_ok;
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */
//add usb
static const uint8_t FAT_DBR_TABLE[]=
{ //FAT���ĵ�0����������������Ͳ���ÿ�����ӵ��Զ�Ҫ��ʽ��һ�飬��������ķ������ڵ��Ը�ʽ���ɹ���dump������ramdiskȡǰ��512�ֽڼ���
0xEB,0x3C,0x90,0x4D,0x53,0x44,0x4F,0x53,0x35,0x2E,0x30,0x00,0x02,0x01,0x04,0x00,
0x02,0x00,0x02,/**/0xE8,0x03,/**/0xF8,0x02,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x00,0x29,0x9C,0xF8,0x5B,0x50,0x4E,0x4F,0x20,0x4E,0x41,
0x4D,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x31,0x32,0x20,0x20,0x20,0x33,0xC9,
0x8E,0xD1,0xBC,0xF0,0x7B,0x8E,0xD9,0xB8,0x00,0x20,0x8E,0xC0,0xFC,0xBD,0x00,0x7C,
0x38,0x4E,0x24,0x7D,0x24,0x8B,0xC1,0x99,0xE8,0x3C,0x01,0x72,0x1C,0x83,0xEB,0x3A,
0x66,0xA1,0x1C,0x7C,0x26,0x66,0x3B,0x07,0x26,0x8A,0x57,0xFC,0x75,0x06,0x80,0xCA,
0x02,0x88,0x56,0x02,0x80,0xC3,0x10,0x73,0xEB,0x33,0xC9,0x8A,0x46,0x10,0x98,0xF7,
0x66,0x16,0x03,0x46,0x1C,0x13,0x56,0x1E,0x03,0x46,0x0E,0x13,0xD1,0x8B,0x76,0x11,
0x60,0x89,0x46,0xFC,0x89,0x56,0xFE,0xB8,0x20,0x00,0xF7,0xE6,0x8B,0x5E,0x0B,0x03,
0xC3,0x48,0xF7,0xF3,0x01,0x46,0xFC,0x11,0x4E,0xFE,0x61,0xBF,0x00,0x00,0xE8,0xE6,
0x00,0x72,0x39,0x26,0x38,0x2D,0x74,0x17,0x60,0xB1,0x0B,0xBE,0xA1,0x7D,0xF3,0xA6,
0x61,0x74,0x32,0x4E,0x74,0x09,0x83,0xC7,0x20,0x3B,0xFB,0x72,0xE6,0xEB,0xDC,0xA0,
0xFB,0x7D,0xB4,0x7D,0x8B,0xF0,0xAC,0x98,0x40,0x74,0x0C,0x48,0x74,0x13,0xB4,0x0E,
0xBB,0x07,0x00,0xCD,0x10,0xEB,0xEF,0xA0,0xFD,0x7D,0xEB,0xE6,0xA0,0xFC,0x7D,0xEB,
0xE1,0xCD,0x16,0xCD,0x19,0x26,0x8B,0x55,0x1A,0x52,0xB0,0x01,0xBB,0x00,0x00,0xE8,
0x3B,0x00,0x72,0xE8,0x5B,0x8A,0x56,0x24,0xBE,0x0B,0x7C,0x8B,0xFC,0xC7,0x46,0xF0,
0x3D,0x7D,0xC7,0x46,0xF4,0x29,0x7D,0x8C,0xD9,0x89,0x4E,0xF2,0x89,0x4E,0xF6,0xC6,
0x06,0x96,0x7D,0xCB,0xEA,0x03,0x00,0x00,0x20,0x0F,0xB6,0xC8,0x66,0x8B,0x46,0xF8,
0x66,0x03,0x46,0x1C,0x66,0x8B,0xD0,0x66,0xC1,0xEA,0x10,0xEB,0x5E,0x0F,0xB6,0xC8,
0x4A,0x4A,0x8A,0x46,0x0D,0x32,0xE4,0xF7,0xE2,0x03,0x46,0xFC,0x13,0x56,0xFE,0xEB,
0x4A,0x52,0x50,0x06,0x53,0x6A,0x01,0x6A,0x10,0x91,0x8B,0x46,0x18,0x96,0x92,0x33,
0xD2,0xF7,0xF6,0x91,0xF7,0xF6,0x42,0x87,0xCA,0xF7,0x76,0x1A,0x8A,0xF2,0x8A,0xE8,
0xC0,0xCC,0x02,0x0A,0xCC,0xB8,0x01,0x02,0x80,0x7E,0x02,0x0E,0x75,0x04,0xB4,0x42,
0x8B,0xF4,0x8A,0x56,0x24,0xCD,0x13,0x61,0x61,0x72,0x0B,0x40,0x75,0x01,0x42,0x03,
0x5E,0x0B,0x49,0x75,0x06,0xF8,0xC3,0x41,0xBB,0x00,0x00,0x60,0x66,0x6A,0x00,0xEB,
0xB0,0x42,0x4F,0x4F,0x54,0x4D,0x47,0x52,0x20,0x20,0x20,0x20,0x0D,0x0A,0x52,0x65,
0x6D,0x6F,0x76,0x65,0x20,0x64,0x69,0x73,0x6B,0x73,0x20,0x6F,0x72,0x20,0x6F,0x74,
0x68,0x65,0x72,0x20,0x6D,0x65,0x64,0x69,0x61,0x2E,0xFF,0x0D,0x0A,0x44,0x69,0x73,
0x6B,0x20,0x65,0x72,0x72,0x6F,0x72,0xFF,0x0D,0x0A,0x50,0x72,0x65,0x73,0x73,0x20,
0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x20,0x74,0x6F,0x20,0x72,0x65,0x73,0x74,0x61,
0x72,0x74,0x0D,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAC,0xCB,0xD8,0x55,0xAA
//����������FATƫ�ƣ��ֶ���������
//0x800 = F8 FF FF 03 40 00 05 F0 FF
//0xC00 = F8 FF FF 03 40 00 05 F0 FF
};
//�����usbת���ڵ�inf�������ļ������̷���Ϣ
static const uint8_t FAT_FILE_NAME[]={
	0x45, 0x49, 0x47, 0x48, 0x54, 0x45, 0x45, 0x54, 0x48, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x95, 0x94, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x52, 0x00, 0x65, 0x00, 0x61, 0x00, 0x64, 0x00, 0x6D, 0x00, 0x0F, 0x00, 0x73, 0x65, 0x00, 
	0x2E, 0x00, 0x74, 0x00, 0x78, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x52, 0x45, 0x41, 0x44, 0x4D, 0x45, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0xA0, 0xB7, 0x95, 
	0x94, 0x47, 0x94, 0x47, 0x00, 0x00, 0xE4, 0x95, 0x94, 0x47, 0x02, 0x00, 0x95, 0x02, 0x00, 0x00, 
	0x41, 0x4D, 0x00, 0x65, 0x00, 0x73, 0x00, 0x73, 0x00, 0x61, 0x00, 0x0F, 0x00, 0xCB, 0x67, 0x00, 
	0x65, 0x00, 0x2E, 0x00, 0x74, 0x00, 0x78, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
	0x4D, 0x45, 0x53, 0x53, 0x41, 0x47, 0x45, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x61, 0x97, 0xA0, 
	0x94, 0x47, 0x94, 0x47, 0x00, 0x00, 0xA0, 0xA0, 0x94, 0x47, 0x06, 0x00, 0x4B, 0x00, 0x00, 0x00,
};

static uint8_t FAT_FILE_DATA[800]=
{
	//offect 0x5000
	"1,Please copy firmware file (Exxxxx.edo ) to the disk.\r\n"
	"\r\n"
	"2,Do not disconnect the upgrade process.\r\n"
	"\r\n"
	"3,A few seconds will upgrade success. \r\n"
	"\r\n"
	"4,When handpiece display [Success],you can pull out the usb cable.\r\n"
};

static uint8_t FAT_FILE_DATA2[]={"1,Please copy firmware file (Exxxxx.edo ) to the disk.\r\n"};		 


union 	                                        /* ��4��u8�ַ��ϲ���һ��u32 */
{
   uint8_t  char_4[4];
   uint32_t long_1;	
} char2long;

uint16_t MAL_Init(uint8_t lun)
{
	uint16_t i=0;

	Mass_Block_Count = 64*1024/SECTOR_SIZE;		//	128kb
	Mass_Block_Size =  SECTOR_SIZE;				//	512byte
	Mass_Memory_Size = 64*1024;					//	64kb
	for(i=0;i<sizeof(FAT_DBR_TABLE);i++){//�ؽ�FAT��
		RAM_DISK[i]=FAT_DBR_TABLE[i];
	}
	for(i=0;i<511;i++){
		RAM_DISK[i+0x1000]=0;
		RAM_DISK[i+0x5000]=0;
		RAM_DISK[i+0x5800]=0; 
	}
	for(i=0;i<sizeof(FAT_FILE_NAME);i++){
		RAM_DISK[i+0x1000]=FAT_FILE_NAME[i];
	}
	for(i=0;i<sizeof(FAT_FILE_DATA);i++){
		RAM_DISK[i+0x5000]=FAT_FILE_DATA[i];
	} 
	for(i=0;i<sizeof(FAT_FILE_DATA2);i++){
		RAM_DISK[i+0x5800]=FAT_FILE_DATA2[i];
	}  
	RAM_DISK[0x800]=0xF8;
	RAM_DISK[0x801]=0xFF;
	RAM_DISK[0x802]=0xFF;
	RAM_DISK[0x803]=0x03;
	RAM_DISK[0x804]=0x40;
	RAM_DISK[0x805]=0x00;
	RAM_DISK[0x806]=0x05;
	RAM_DISK[0x807]=0xF0;
	RAM_DISK[0x808]=0xFF;
	RAM_DISK[0x809]=0xFF;
	RAM_DISK[0x80A]=0x0F;

	RAM_DISK[0xC00]=0xF8;
	RAM_DISK[0xC01]=0xFF;
	RAM_DISK[0xC02]=0xFF;
	RAM_DISK[0xC03]=0x03;
	RAM_DISK[0xC04]=0x40;
	RAM_DISK[0xC05]=0x00;
	RAM_DISK[0xC06]=0x05;
	RAM_DISK[0xC07]=0xF0;
	RAM_DISK[0xC08]=0xFF;  
	RAM_DISK[0xC09]=0xFF;  
	RAM_DISK[0xC0A]=0x0F;  

	return lun == 0 ? 0 : 1;
}
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA_FS */
/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata_FS[] = {/* 36 */
  
  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
}; 
/* USER CODE END INQUIRY_DATA_FS */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *)STORAGE_Inquirydata_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes over USB FS IP
  * @param  lun:
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init_FS(uint8_t lun)
{
  /* USER CODE BEGIN 2 */
  return (USBD_OK);
  /* USER CODE END 2 */
}

/**
  * @brief  .
  * @param  lun: .
  * @param  block_num: .
  * @param  block_size: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */
  *block_num  = STORAGE_BLK_NBR;
  *block_size = STORAGE_BLK_SIZ;
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsReady_FS(uint8_t lun)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */
	uint32_t n,i=0;	//add usb
	for(n=0;n<blk_len;n++){
		for(i=0;i<STORAGE_BLK_SIZ;i++){
			buf[n*STORAGE_BLK_SIZ+i]=RAM_DISK[(blk_addr+n)*STORAGE_BLK_SIZ+i];
		}
	}
	
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */
	//add usb
	uint32_t i;
	uint8_t *p=(uint8_t *)buf;
	static uint8_t is_updata_file=0;
	static uint32_t write_cnt=0;
	static uint32_t Bytes=0;
	static uint32_t Write_Bytes=0;
	if(lun==0){//ENDOMOTORWITHAPEXedo
		if(buf[0]==0x45  && buf[1]==0x4E  && buf[2]==0x44  && buf[3]==0x4F\
			&& buf[4]==0x4D && buf[5]==0x4F && buf[6]==0x54 && buf[7]==0x4F\
			&&buf[8]==0x52  && buf[9]==0x57  && buf[10]==0x49  && buf[11]==0x54\
			&& buf[12]==0x48  && buf[13]==0x41  && buf[14]==0x50  &&buf[15]==0x45\
			&& buf[16]==0x58  && buf[17]==0x65  && buf[18]==0x64  && buf[19]==0x6F){
			is_updata_file=1;
			write_cnt=0;
			for(i=0;i<SECTOR_SIZE;i++){								
				FLASH_TMP_BUF[i]=p[i];								
			}

			Bytes=(uint32_t)(FLASH_TMP_BUF[32]<<24)|(uint32_t)(FLASH_TMP_BUF[33]<<16)|(uint32_t)(FLASH_TMP_BUF[34]<<8)|(uint32_t)(FLASH_TMP_BUF[35]);//�ֽ���

			MEM_If_Erase_FS(IN_FLASH_STAR);				//	����app

		}else if(is_updata_file==1){
			for(i=0;i<SECTOR_SIZE;i++){
				FLASH_TMP_BUF[i]=p[i];
			}
			AES_DECODE(KEY,FLASH_TMP_BUF,SECTOR_SIZE);  //	����

			for(i=0;i<SECTOR_SIZE/4;i++){
				char2long.char_4[3] =  FLASH_TMP_BUF[i*4+3];
				char2long.char_4[2] =  FLASH_TMP_BUF[i*4+2];
				char2long.char_4[1] =  FLASH_TMP_BUF[i*4+1];
				char2long.char_4[0] =  FLASH_TMP_BUF[i*4];

				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,IN_FLASH_STAR + write_cnt,char2long.long_1);		//һ��д��4�ֽ�
				write_cnt += 4;
			} 

			Write_Bytes++;
			if(Write_Bytes>(Bytes/SECTOR_SIZE)){
				is_updata_file=0;
				flag_iap_ok=1;
			}
		}
	}
  return (USBD_OK);
  /* USER CODE END 7 */
}

/**
  * @brief  .
  * @param  None
  * @retval .
  */
int8_t STORAGE_GetMaxLun_FS(void)
{
  /* USER CODE BEGIN 8 */
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/