#ifndef __PAGE_AGING_H
#define __PAGE_AGING_H
#include <stdint.h>

#include "GL_Config.h"			/*	����myPrintf	*/

#define __DEBUG_AGING

#ifdef __DEBUG_AGING
#define AGING_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define AGING_PRINTF(format, ...)
#endif

typedef enum	/*	�ϻ�״̬	*/
{
	AGING_IDLE = 0,
	AGING_START,
	AGING_STOP,
	AGING_RUNING,
	AGING_FINISH,
	EXIT_AGING
}AGING_MODE;


typedef enum	/*	�ϻ������У������Ҫ��Ҫ��������ת״̬	*/
{
	AGING_M_NEED_IDLE = 0,
	AGING_M_NEED_FWD,
	AGING_M_NEED_REV
}AGING_M_CHANGE_DIR_T;

#define PAGE_AGING_START_ID		0x03			//	��ʼ��ť  ID
#define PAGE_AGING_FINISH_ID	0x04			//	��ɰ�ť  ID

#define PAGE_AGING_DOSE_ID		0x02			//	ҩҺ�����ؼ�ID
#define PAGE_AGING_COMM_ID		0x0805			//	ͨ�Ų���  ID
#define PAGE_AGING_CYCLE_ID		0x0807			//	�ϻ�Ȧ��  ID
#define PAGE_AGING_kPa_ID		0x0809			//	Һѹ����ֵ
#define PAGE_AGING_mV_ID		0x080B			//	��̤�����ѹֵ

void page_aging(void);							

#endif
