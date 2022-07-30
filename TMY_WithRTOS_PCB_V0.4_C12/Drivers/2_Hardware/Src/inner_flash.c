#include "inner_flash.h"
#include "stm32f4xx_hal.h"

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-14
* 函 数 名: rd_Inner_Flash
* 功能说明: 读取内部flash数据
* 形    参: 
*			src_addr:	flash中的地址
*			size:		读取数据的数目，单位4字节为一个单位
*			target_addr:读取后的数据存储地址
* 说    明：
			
* 返 回 值: 
*			0: 失败
*			x：实际读取的数目
*********************************************************************************************************
*/
uint32_t rd_Inner_Flash(uint32_t src_addr, uint32_t size, uint32_t* target_addr)
{
	uint32_t i = 0;
	uint32_t max_could_read_size = (PARA_END_ADDR +1 -src_addr) >>2;
	
	/*	1. 判断边界	*/
	if(src_addr < PARA_START_ADDR)	
		return 0;
	if(src_addr >PARA_END_ADDR)
		return 0;
	/*	2. 最大可读取数目	*/
	max_could_read_size = max_could_read_size > size ? size:max_could_read_size;
	/*	3. 读取数据	*/
	for(i =0; i< max_could_read_size; i++)
	{
		*target_addr = *((__IO uint32_t *) src_addr);	/*	读取flash中的数据	*/
		src_addr += 4;									/*	flash中的地址+4	*/
		target_addr ++;									/*	目标地址自增	*/
	}
	return max_could_read_size;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	本：V1.0
* 日    期：2021-10-14
* 函 数 名: wr_Inner_Flash
* 功能说明: 读取内部flash数据
* 形    参: 
*			target_addr:flash中的地址
*			size:		写入数据的数目，单位4字节为一个单位
*			src_addr:	数据源的地址
* 说    明：
			
* 返 回 值: 
*			0: 失败
*			x：实际写入的数目
*********************************************************************************************************
*/
uint32_t wr_Inner_Flash(uint32_t target_addr, uint32_t size, uint32_t* src_addr)
{
	uint32_t i = 0;
	uint32_t max_could_wr_size = (PARA_END_ADDR +1 -target_addr) >>2;
	uint32_t  t_addr = target_addr;
	uint32_t* s_addr = src_addr;
	uint8_t wr_times = MAX_FLASH_WR_TIMES;
	uint8_t isRight = 0;
	
	/*	1. 判断边界	*/
	if(target_addr < PARA_START_ADDR)	
		return 0;
	if(target_addr >PARA_END_ADDR)
		return 0;
	
	/*	2. 最大写入数目	*/
	max_could_wr_size = max_could_wr_size > size ? size:max_could_wr_size;
	
	HAL_FLASH_Unlock();				/*	解锁	*/
	__HAL_FLASH_CLEAR_FLAG(	FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |\
							FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR);	/*	清除标志		*/
	
	/*	3. 写入数据	*/
	while(wr_times --)
	{
		/*	3.1 擦除扇区		*/
		FLASH_Erase_Sector(FLASH_SECTOR_7, VOLTAGE_RANGE_3);
		/*	3.2 写入数据		*/
		for(i =0; i< max_could_wr_size; i++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, t_addr, *s_addr);
			t_addr += 4;
			s_addr ++;
		}
		/*	3.3 校验数据		*/
		isRight = 1;
		t_addr = target_addr;
		s_addr = src_addr;
		for(i =0; i< max_could_wr_size; i++)
		{
			if((*(__IO uint32_t*)t_addr) != *s_addr)
				isRight = 0;
			t_addr += 4;
			s_addr ++;
		}
		
		if(isRight)
			return max_could_wr_size;	
	}
	
	return 0;
}
