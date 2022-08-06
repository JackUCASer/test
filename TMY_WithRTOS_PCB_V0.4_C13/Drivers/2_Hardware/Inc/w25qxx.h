#ifndef	__W25QXX_H 
#define	__W25QXX_H

#include "gpio.h"

#define SF_CS_0()	HAL_GPIO_WritePin(W25_SPI_CS_GPIO_Port, W25_SPI_CS_Pin, GPIO_PIN_RESET)
#define SF_CS_1()	HAL_GPIO_WritePin(W25_SPI_CS_GPIO_Port, W25_SPI_CS_Pin, GPIO_PIN_SET)

#define SF_MAX_PAGE_SIZE	(4 * 1024)

/* 定义串行Flash ID */
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
	W25Q32_ID    = 0xEF4016,
	W25Q128_ID   = 0xEF4018
};

typedef struct
{
	uint32_t ChipID;			/* 芯片ID */
	char ChipName[16];		/* 芯片型号字符串，主要用于显示 */
	uint32_t TotalSize;		/* 总容量 */
	uint16_t PageSize;		/* 页面大小 */
}SFLASH_T;

void bsp_InitSFlash(void);
uint32_t sf_ReadID(void);
void sf_EraseChip(void);
void sf_EraseSector(uint32_t _uiSectorAddr);
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
void sf_ReadInfo(void);

extern SFLASH_T g_tSF;

#endif
