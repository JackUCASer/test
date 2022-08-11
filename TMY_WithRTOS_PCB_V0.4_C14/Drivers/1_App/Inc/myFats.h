#ifndef __MYFATS_H
#define __MYFATS_H

#include "fatfs.h"

/* 
Notice: 
  对于一个新的SPI FLASH，先挂载f_mount，再格式化文件系统f_mkfs，
  之后就可以做各种新建文件、读写操作；所以在整个产品周期中，仅在
  第一次开机和恢复出厂设置中调用函数SPI_Flash_Format()。
 */
void SPI_Flash_Format(void);

/* 外部接口  */
void SPI_FLASH_Bf_ReadWrite(void);
FRESULT SPI_FLASH_Write(const TCHAR* path, void* buff, UINT btr, UINT* br);
FRESULT SPI_FLASH_Read(const TCHAR* path, void* buff, UINT btr, UINT* br);
void SPI_FLASH_Af_ReadWrite(void);

/*单元测试*/
void SPI_Flash_Test(void);
void test_SPI_Flash_Write_Read(void);



#endif
