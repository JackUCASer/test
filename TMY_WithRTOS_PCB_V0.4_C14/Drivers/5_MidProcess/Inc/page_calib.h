#ifndef __PAGE_CALIB_H
#define __PAGE_CALIB_H
#include <stdint.h>


#include "GL_Config.h"			/*	依赖myPrintf	*/
#define __DEBUG_CALIB

#ifdef __DEBUG_CALIB
#define CALIB_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define CALIB_PRINTF(format, ...)
#endif

typedef enum
{
	CALIB_READY = 0,
	CALIB_FWD_1st,		//	第一次正转
	CALIB_REV_2nd,		//	第二次反转
	CALIB_FWD_3rd,		//	第三次正转
	CALIB_FINISH		//	校正完成
}OIL_PRESS_CALIB_STATE_T;		/*	定义液压校正流程各个状态机	*/

typedef enum
{
	KEEP_RUNNING = 0,	//	继续运转
	REACH_TOP,			//	达到顶端
	REACH_BOTTOM,		//	到达顶部
	REACH_ORIGIN,		//	到达原点
}REACH_ENDPOINT_T;				/*	端点检测返回状态	*/

typedef struct
{
	uint32_t REV_1st_LOC;	//	电机完成首次反转所处的位置:高16位表示圈数，低16位表示角度
	uint32_t FWD_1st_LOC;	//	电机完成首次正转所处的位置:高16位表示圈数，低16位表示角度
	uint32_t REV_2nd_LOC;	//	电机完成二次反转所处的位置:高16位表示圈数，低16位表示角度
}OIL_PRESS_CALIB_MOTOR_LOC_T;

#define PAGE_CALIB_PEDAL_ID		0x01
#define PAGE_CALIB_OILPS_ID		0x02
#define PAGE_CALIB_FINISH_ID	0x03

void page_calibration(void);

#endif	
