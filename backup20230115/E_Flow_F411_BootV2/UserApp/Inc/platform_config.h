#include "stm32f4xx_hal.h"

/***************Flash划分******************/
//	STM32F401CEU6总共512Kb字节Flash
//	bootloader: 32Kb, 地址：0x08000000--0x08007FFF
//	user app:	224Kb,地址：0x08008000--0x0803FFFF
//	FlashDB:	256Kb,地址：0x08040000--0x0807FFFF
#define	 IN_FLASH_STAR      	0x08008000	/* 内部flash，用户程序起始地址 */
#define	 IN_FLASH_END      		0x0803FFFF	/* 内部flash，用户程序结束地址 */
#define  IN_FLASH_SECTOR		2048		/* 内部flash 2k一个扇区，一次要擦除一个扇区单位*/
#define  FLASH_WAIT_TIMEOUT		100000


typedef  void (*user_app)(void);
