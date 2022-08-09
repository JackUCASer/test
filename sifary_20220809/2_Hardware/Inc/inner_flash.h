#ifndef __INNER_FLASH_H
#define __INNER_FLASH_H
#include <stdint.h>

#define PARA_START_ADDR		((uint32_t)0x08060000)		/*	内部Flash使用起始地址	*/
#define PARA_END_ADDR		((uint32_t)0x0807FFFF)		/*	内部Flash使用结束地址	*/
#define PARA_PAGE_SIZE		((uint32_t)0x00020000)		/*	内部Flash扇区大小：128Kbytes	*/

#define PARA_FLAG	0x1000	//	参数标记

#define MAX_FLASH_WR_TIMES	3							/*	单次写入数据最多写入次数	*/

/*	flash存储时会自动4字节对齐，尽量保证PARASAVED_T的大小是4的整数倍，避免存入读取后数据错位的麻烦	*/
typedef	struct
{
	uint8_t 	para[64];
	uint32_t	flag;
}PARASAVED_T;


/*	外部函数接口		*/
uint32_t rd_Inner_Flash(uint32_t src_addr, uint32_t size, uint32_t* target_addr);	/*	读数据	*/
uint32_t wr_Inner_Flash(uint32_t target_addr, uint32_t size, uint32_t* src_addr);	/*	写数据	*/

#endif
