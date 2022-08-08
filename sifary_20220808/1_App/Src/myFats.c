#include "myFats.h"
#include "GL_Config.h"
#include "fatfs.h"

static void Fats_print(char* functionStr, uint8_t errCode);

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-01
* 函 数 名: Fats_print
* 功能说明: 打印调试信息
* 形    参: 
      
* 返 回 值: 
       
*********************************************************************************************************
*/
static void Fats_print(char* functionStr, uint8_t errCode)
{
	switch(errCode)
	{
		case 	FR_OK:									myPrintf("%s FR_OK!\r\n",functionStr);									break;
		case 	FR_DISK_ERR:						myPrintf("%s FR_DISK_ERR!\r\n",functionStr);						break;
		case	FR_INT_ERR:							myPrintf("%s FR_INT_ERR!\r\n",functionStr);							break;
		case	FR_NOT_READY:						myPrintf("%s FR_NOT_READY!\r\n",functionStr);						break;
		case	FR_NO_FILE:							myPrintf("%s FR_NO_FILE!\r\n",functionStr);							break;
		case	FR_NO_PATH:							myPrintf("%s FR_NO_PATH!\r\n",functionStr);							break;
		case	FR_INVALID_NAME:				myPrintf("%s FR_INVALID_NAME!\r\n",functionStr);				break;
		case	FR_DENIED:							myPrintf("%s FR_DENIED!\r\n",functionStr);							break;
		case	FR_EXIST:								myPrintf("%s FR_EXIST!\r\n",functionStr);								break;
		case	FR_INVALID_OBJECT:			myPrintf("%s FR_INVALID_OBJECT!\r\n",functionStr);			break;
		case	FR_WRITE_PROTECTED:			myPrintf("%s FR_WRITE_PROTECTED!\r\n",functionStr);			break;
		case	FR_INVALID_DRIVE:				myPrintf("%s FR_INVALID_DRIVE!\r\n",functionStr);				break;
		case	FR_NOT_ENABLED:					myPrintf("%s FR_NOT_ENABLED!\r\n",functionStr);					break;
		case	FR_NO_FILESYSTEM:				myPrintf("%s FR_NO_FILESYSTEM!\r\n",functionStr);				break;
		case	FR_MKFS_ABORTED:				myPrintf("%s FR_MKFS_ABORTED!\r\n",functionStr);				break;
		case	FR_TIMEOUT:							myPrintf("%s FR_TIMEOUT!\r\n",functionStr);							break;
		case	FR_LOCKED:							myPrintf("%s FR_LOCKED!\r\n",functionStr);							break;
		case	FR_NOT_ENOUGH_CORE:			myPrintf("%s FR_NOT_ENOUGH_CORE!\r\n",functionStr);			break;
		case	FR_TOO_MANY_OPEN_FILES:	myPrintf("%s FR_TOO_MANY_OPEN_FILES!\r\n",functionStr);	break;
		case	FR_INVALID_PARAMETER:		myPrintf("%s FR_INVALID_PARAMETER!\r\n",functionStr);		break;
		default:break;
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-02
* 函 数 名: SPI_Flash_Format
* 功能说明: 格式化Flash,该函数仅在恢复出厂设置时生效
* 形    参: 
      
* 返 回 值: 

*********************************************************************************************************
*/
void SPI_Flash_Format(void)
{
	FRESULT res = FR_OK;
	BYTE work[_MAX_SS]; 		/* Work area (larger is better for processing time) */
	
	if(FATFS_LinkDriver(&USER_Driver, USERPath) == FR_OK)
		myPrintf("SPI_Flash_Format:FATFS_LinkDriver Sucess! USERPath = %s!\r\n",USERPath);
	else{
		myPrintf("SPI_Flash_Format:FATFS_LinkDriver Failure!\r\n");
		return ;
	}
	
	/*	1. 注册工作区		*/
	res = f_mount(&USERFatFS, (TCHAR const*)USERPath, 0);
	if(res != FR_OK)
	{
		Fats_print("f_mount",res);
		return;
	}
	
	/*	2. 格式化Flash		*/
	res = f_mkfs(USERPath, FM_FAT, 0, work, _MAX_SS);
	if(res != FR_OK)
	{
		Fats_print("f_mkfs",res);
		return;
	}
	
	/*	3. 在根目录创建文件夹 UPGRADE，如果返回已存在，继续下一步		*/
	res = f_mkdir("Upgrade");
	if((res != FR_OK) && (res != FR_EXIST))
	{
		Fats_print("f_mkdir",res);
		return;
	}
	
	/*	4. 在根目录创建文件夹 DATA，如果返回已存在，继续下一步		*/
	res = f_mkdir("Data");
	if((res != FR_OK) && (res != FR_EXIST))
	{
		Fats_print("f_mkdir",res);
		return;
	}
	
	/*	5. 注销工作区		*/
	f_mount(NULL, (TCHAR const*)USERPath, 0);
	
	/*	6. 解除驱动链接		*/
	FATFS_UnLinkDriver(USERPath);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-02
* 函 数 名: SPI_FLASH_Bf_ReadWrite
* 功能说明: 在向SPI_Flash读写数据之前需要的配置
* 形    参: 
      
* 返 回 值: 
       
*********************************************************************************************************
*/
void SPI_FLASH_Bf_ReadWrite(void)
{
	if(FATFS_LinkDriver(&USER_Driver, USERPath) == FR_OK)
		myPrintf("SPI_FLASH_Bf_ReadWrite:FATFS_LinkDriver Sucess! USERPath = %s!\r\n",USERPath);
	else{
		myPrintf("SPI_FLASH_Bf_ReadWrite:FATFS_LinkDriver Failure!\r\n");
		return ;
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-02
* 函 数 名: SPI_FLASH_Af_ReadWrite
* 功能说明: 在向SPI_Flash读写数据之后需要的配置
* 形    参: 
      
* 返 回 值: 
       
*********************************************************************************************************
*/
void SPI_FLASH_Af_ReadWrite(void)
{
	FATFS_UnLinkDriver(USERPath);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-02
* 函 数 名: SPI_FLASH_Write
* 功能说明: 向SPI FLASH写数据
* 形    参: 
					path:	路径（绝对路径）
					buff:	写入数据的缓冲区
					btr：	需要写的字节数目，范围：0	~ (2^32)-1	
					br:		记录文件中已写的字节数目;
								正常情况下 *br=btr，即已写的字节数等于需要写的字节数；
								如果 *br<btr,说明卷已写满了，无法再写。
      
* 返 回 值: 
       
*********************************************************************************************************
*/
FRESULT SPI_FLASH_Write(const TCHAR* path, void* buff, UINT btr, UINT* br)
{
	FRESULT res = FR_OK;
	
	res = f_mount(&USERFatFS, (TCHAR const*)USERPath, 0);
	if(res != FR_OK)
	{
		Fats_print("f_mount",res);
		return res;
	}
	
	res = f_open(&USERFile, path, FA_OPEN_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		Fats_print("f_open",res);
		return res;
	}
	
	res = f_write(&USERFile, buff, btr, br);
	if(res != FR_OK)
	{
		Fats_print("f_write",res);
		return res;
	}
	
	f_close(&USERFile);
	f_mount(NULL, (TCHAR const*)USERPath, 0);
	return FR_OK;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-02
* 函 数 名: SPI_FLASH_Read
* 功能说明: 从SPI FLASH读数据
* 形    参: 
					path:	路径（绝对路径）
					buff:	接收数据的缓冲区
					btr：	需要读的字节数目，范围：0	~ (2^32)-1	
					br:		记录本次操作中读到的字节数，
								正常情况下 *br=btr，即读到的字节数等于需要读的字节数；
								如果 *br<btr,说明文件已读完。 
      
* 返 回 值: 
					无
       
*********************************************************************************************************
*/
FRESULT SPI_FLASH_Read(const TCHAR* path, void* buff, UINT btr, UINT* br)
{
	FRESULT res = FR_OK;
	
	res = f_mount(&USERFatFS, (TCHAR const*)USERPath, 0);
	if(res != FR_OK)
	{
		Fats_print("f_mount",res);
		return res;
	}
	res = f_open(&USERFile, path, FA_OPEN_ALWAYS | FA_READ);
	if(res != FR_OK)
	{
		Fats_print("f_open",res);
		return res;
	}

	res = f_read(&USERFile, buff, btr, br);
	if(res != FR_OK)
	{
		Fats_print("f_read",res);
		return res;
	}
	f_close(&USERFile);
	f_mount(NULL, (TCHAR const*)USERPath, 0);
	
	return FR_OK;
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-06-02
* 函 数 名: test_SPI_Flash_Write_Read
* 功能说明: 测试SPI_FLASH_Write和SPI_FLASH_Read函数
* 形    参:  
      
* 返 回 值: 
       
*********************************************************************************************************
*/
void test_SPI_Flash_Write_Read(void)
{
	FRESULT res = FR_OK;
	char wbuff[] = "this is just for Flash write test in folder upgrade!";
	UINT wCnt = 0;
	
	char wbuff2[] = "this is just for Flash write test in folder data!";
	UINT wCnt2 = 0;
	
	char rbuff[100]={0};
	UINT rCnt = 0;
	
	char rbuff2[100]={0};
	UINT rCnt2 = 0;
	
	/*	1. 测试写		*/
	SPI_FLASH_Bf_ReadWrite();
	res = SPI_FLASH_Write("0:/UPGRADE/TEST.TXT", wbuff, sizeof(wbuff),&wCnt);
	if(res == FR_OK)
		myPrintf("the write para: %d,%d\r\n", sizeof(wbuff),wCnt);
	
	res = SPI_FLASH_Write("0:/DATA/TEST.TXT", wbuff2, sizeof(wbuff2),&wCnt2);
	if(res == FR_OK)
		myPrintf("the write para: %d,%d\r\n", sizeof(wbuff2),wCnt2);
	SPI_FLASH_Af_ReadWrite();
	
	/*	2. 测试读		*/
	SPI_FLASH_Bf_ReadWrite();
	res = SPI_FLASH_Read("0:/UPGRADE/TEST.TXT", rbuff, sizeof(wbuff),&rCnt);
	if(res == FR_OK)
		myPrintf("the read para: %s, %d,%d\r\n", rbuff, sizeof(wbuff),rCnt);
	
	res = SPI_FLASH_Read("0:/DATA/TEST.TXT", rbuff2, sizeof(wbuff2),&rCnt2);
	if(res == FR_OK)
		myPrintf("the read para: %s, %d,%d\r\n", rbuff2, sizeof(wbuff2),rCnt2);
	SPI_FLASH_Af_ReadWrite();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2021-06-01
* 函 数 名: SPI_Flash_Test
* 功能说明: 测试U盘
* 形    参: 
      
* 返 回 值: 
       
*********************************************************************************************************
*/
void SPI_Flash_Test(void)
{
	uint32_t wbytes; 																									/* 写入文件的字节计数 */
	uint8_t wtext[] = "TMY test Text, Develop by WangYuan, Sifary!"; 	/* 写入文件的缓冲 */
	uint8_t rtext[100] = {0};
	BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */
	
	FRESULT res = FR_OK;
	/*	1. */
	if(FATFS_LinkDriver(&USER_Driver, USERPath) == FR_OK)
		myPrintf("FATFS_LinkDriver Sucess! USERPath = %s!\r\n",USERPath);
	else{
		myPrintf("FATFS_LinkDriver Failure!\r\n");
		return ;
	}
	
	/*	2. */
	res = f_mount(&USERFatFS, (TCHAR const*)USERPath, 0);
	if(res != FR_OK)
	{
		Fats_print("f_mount",res);
		return;
	}
	res = f_mkfs(USERPath, FM_FAT, 0, work, _MAX_SS);
	if(res != FR_OK)
	{
		Fats_print("f_mkfs",res);
		return;
	}
	
	/*	3. 创建文件夹 Upgrade，如果返回已存在，继续下一步		*/
	res = f_mkdir("Upgrade");
	if((res != FR_OK) && (res != FR_EXIST))
	{
		Fats_print("f_mkdir",res);
		return;
	}
		
	/*	4. 进入文件夹 Upgrade*/
	res = f_chdir("Upgrade");
	if(res != FR_OK)
	{
		Fats_print("f_chdir",res);
		return;
	}
	
	/*	5.1 打开文件		*/
	res = f_open(&USERFile, "0:/Upgrade/TEST.TXT", FA_OPEN_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		Fats_print("f_open",res);
		return;
	}
	/*	5.2 写入数据		*/
	res = f_write(&USERFile, wtext, sizeof(wtext), (void *)&wbytes);
	if(res != FR_OK)
	{
		Fats_print("f_write",res);
		return;
	}
	/*	5.3 关闭文件		*/
	f_close(&USERFile);			/*		关闭文件		*/
	
	/*	6.1 打开文件		*/
	res = f_open(&USERFile, "0:/Upgrade/TEST.TXT", FA_OPEN_ALWAYS | FA_READ);
	if(res != FR_OK)
	{
		Fats_print("f_open",res);
		return;
	}
	/*	6.2 读取文件		*/
	res = f_read(&USERFile, rtext, sizeof(wtext), (void *)&wbytes);
	if(res != FR_OK)
	{
		Fats_print("f_read",res);
		return;
	}
	myPrintf("the read data: %s,%d,%d\r\n", rtext, sizeof(wtext),wbytes);
	
	res = f_read(&USERFile, rtext, 7, (void *)&wbytes);
	if(res != FR_OK)
	{
		Fats_print("f_read",res);
		return;
	}
	myPrintf("the read data: %s,%d,%d\r\n", rtext, sizeof(wtext),wbytes);
	/*	6.3 关闭文件*/
	f_close(&USERFile);			/*		关闭文件		*/
	
	/*	7. end		*/
	FATFS_UnLinkDriver(USERPath);
}
