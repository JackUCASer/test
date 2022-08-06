#include "inner_flash.h"
#include "stm32f4xx_hal.h"

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-14
* �� �� ��: rd_Inner_Flash
* ����˵��: ��ȡ�ڲ�flash����
* ��    ��: 
*			src_addr:	flash�еĵ�ַ
*			size:		��ȡ���ݵ���Ŀ����λ4�ֽ�Ϊһ����λ
*			target_addr:��ȡ������ݴ洢��ַ
* ˵    ����
			
* �� �� ֵ: 
*			0: ʧ��
*			x��ʵ�ʶ�ȡ����Ŀ
*********************************************************************************************************
*/
uint32_t rd_Inner_Flash(uint32_t src_addr, uint32_t size, uint32_t* target_addr)
{
	uint32_t i = 0;
	uint32_t max_could_read_size = (PARA_END_ADDR +1 -src_addr) >>2;
	
	/*	1. �жϱ߽�	*/
	if(src_addr < PARA_START_ADDR)	
		return 0;
	if(src_addr >PARA_END_ADDR)
		return 0;
	/*	2. ���ɶ�ȡ��Ŀ	*/
	max_could_read_size = max_could_read_size > size ? size:max_could_read_size;
	/*	3. ��ȡ����	*/
	for(i =0; i< max_could_read_size; i++)
	{
		*target_addr = *((__IO uint32_t *) src_addr);	/*	��ȡflash�е�����	*/
		src_addr += 4;									/*	flash�еĵ�ַ+4	*/
		target_addr ++;									/*	Ŀ���ַ����	*/
	}
	return max_could_read_size;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2021-10-14
* �� �� ��: wr_Inner_Flash
* ����˵��: ��ȡ�ڲ�flash����
* ��    ��: 
*			target_addr:flash�еĵ�ַ
*			size:		д�����ݵ���Ŀ����λ4�ֽ�Ϊһ����λ
*			src_addr:	����Դ�ĵ�ַ
* ˵    ����
			
* �� �� ֵ: 
*			0: ʧ��
*			x��ʵ��д�����Ŀ
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
	
	/*	1. �жϱ߽�	*/
	if(target_addr < PARA_START_ADDR)	
		return 0;
	if(target_addr >PARA_END_ADDR)
		return 0;
	
	/*	2. ���д����Ŀ	*/
	max_could_wr_size = max_could_wr_size > size ? size:max_could_wr_size;
	
	HAL_FLASH_Unlock();				/*	����	*/
	__HAL_FLASH_CLEAR_FLAG(	FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |\
							FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR);	/*	�����־		*/
	
	/*	3. д������	*/
	while(wr_times --)
	{
		/*	3.1 ��������		*/
		FLASH_Erase_Sector(FLASH_SECTOR_7, VOLTAGE_RANGE_3);
		/*	3.2 д������		*/
		for(i =0; i< max_could_wr_size; i++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, t_addr, *s_addr);
			t_addr += 4;
			s_addr ++;
		}
		/*	3.3 У������		*/
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
