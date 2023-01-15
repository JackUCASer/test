#include "stm32f4xx_hal.h"

/***************Flash����******************/
//	STM32F401CEU6�ܹ�512Kb�ֽ�Flash
//	bootloader: 32Kb, ��ַ��0x08000000--0x08007FFF
//	user app:	224Kb,��ַ��0x08008000--0x0803FFFF
//	FlashDB:	256Kb,��ַ��0x08040000--0x0807FFFF
#define	 IN_FLASH_STAR      	0x08008000	/* �ڲ�flash���û�������ʼ��ַ */
#define	 IN_FLASH_END      		0x0803FFFF	/* �ڲ�flash���û����������ַ */
#define  IN_FLASH_SECTOR		2048		/* �ڲ�flash 2kһ��������һ��Ҫ����һ��������λ*/
#define  FLASH_WAIT_TIMEOUT		100000


typedef  void (*user_app)(void);
