#ifndef __PAGE_CALIB_H
#define __PAGE_CALIB_H
#include <stdint.h>


#include "GL_Config.h"			/*	����myPrintf	*/
#define __DEBUG_CALIB

#ifdef __DEBUG_CALIB
#define CALIB_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define CALIB_PRINTF(format, ...)
#endif

typedef enum
{
	CALIB_READY = 0,
	CALIB_FWD_1st,		//	��һ����ת
	CALIB_REV_2nd,		//	�ڶ��η�ת
	CALIB_FWD_3rd,		//	��������ת
	CALIB_FINISH		//	У�����
}OIL_PRESS_CALIB_STATE_T;		/*	����ҺѹУ�����̸���״̬��	*/

typedef enum
{
	KEEP_RUNNING = 0,	//	������ת
	REACH_TOP,			//	�ﵽ����
	REACH_BOTTOM,		//	���ﶥ��
	REACH_ORIGIN,		//	����ԭ��
}REACH_ENDPOINT_T;				/*	�˵��ⷵ��״̬	*/

typedef struct
{
	uint32_t REV_1st_LOC;	//	�������״η�ת������λ��:��16λ��ʾȦ������16λ��ʾ�Ƕ�
	uint32_t FWD_1st_LOC;	//	�������״���ת������λ��:��16λ��ʾȦ������16λ��ʾ�Ƕ�
	uint32_t REV_2nd_LOC;	//	�����ɶ��η�ת������λ��:��16λ��ʾȦ������16λ��ʾ�Ƕ�
}OIL_PRESS_CALIB_MOTOR_LOC_T;

#define PAGE_CALIB_PEDAL_ID		0x01
#define PAGE_CALIB_OILPS_ID		0x02
#define PAGE_CALIB_FINISH_ID	0x03

void page_calibration(void);

#endif	
