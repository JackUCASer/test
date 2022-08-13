#ifndef __INNER_FLASH_H
#define __INNER_FLASH_H
#include <stdint.h>

#define PARA_START_ADDR		((uint32_t)0x08060000)		/*	�ڲ�Flashʹ����ʼ��ַ	*/
#define PARA_END_ADDR		((uint32_t)0x0807FFFF)		/*	�ڲ�Flashʹ�ý�����ַ	*/
#define PARA_PAGE_SIZE		((uint32_t)0x00020000)		/*	�ڲ�Flash������С��128Kbytes	*/

#define PARA_FLAG	0x1000	//	�������

#define MAX_FLASH_WR_TIMES	3							/*	����д���������д�����	*/

/*	flash�洢ʱ���Զ�4�ֽڶ��룬������֤PARASAVED_T�Ĵ�С��4������������������ȡ�����ݴ�λ���鷳	*/
typedef	struct
{
	uint8_t 	para[64];
	uint32_t	flag;
}PARASAVED_T;


/*	�ⲿ�����ӿ�		*/
uint32_t rd_Inner_Flash(uint32_t src_addr, uint32_t size, uint32_t* target_addr);	/*	������	*/
uint32_t wr_Inner_Flash(uint32_t target_addr, uint32_t size, uint32_t* src_addr);	/*	д����	*/

#endif
