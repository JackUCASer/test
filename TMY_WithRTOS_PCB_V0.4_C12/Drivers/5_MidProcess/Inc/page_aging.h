#ifndef __PAGE_AGING_H
#define __PAGE_AGING_H
#include <stdint.h>

#include "GL_Config.h"			/*	依赖myPrintf	*/

#define __DEBUG_AGING

#ifdef __DEBUG_AGING
#define AGING_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define AGING_PRINTF(format, ...)
#endif

typedef enum	/*	老化状态	*/
{
	AGING_IDLE = 0,
	AGING_START,
	AGING_STOP,
	AGING_RUNING,
	AGING_FINISH,
	EXIT_AGING
}AGING_MODE;


typedef enum	/*	老化过程中，电机需要需要调整的运转状态	*/
{
	AGING_M_NEED_IDLE = 0,
	AGING_M_NEED_FWD,
	AGING_M_NEED_REV
}AGING_M_CHANGE_DIR_T;

#define PAGE_AGING_START_ID		0x03			//	开始按钮  ID
#define PAGE_AGING_FINISH_ID	0x04			//	完成按钮  ID

#define PAGE_AGING_DOSE_ID		0x02			//	药液余量控件ID
#define PAGE_AGING_COMM_ID		0x0805			//	通信测试  ID
#define PAGE_AGING_CYCLE_ID		0x0807			//	老化圈数  ID
#define PAGE_AGING_kPa_ID		0x0809			//	液压具体值
#define PAGE_AGING_mV_ID		0x080B			//	脚踏具体电压值

void page_aging(void);							

#endif
