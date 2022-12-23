#ifndef __PAGE_MODE_H
#define __PAGE_MODE_H

#include "lcd_display.h"
#include <stdbool.h>

#include "GL_Config.h"

#define __DEBUG_MODE

#ifdef __DEBUG_MODE
#define MODE_PRINTF(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define MODE_PRINTF(format, ...)
#endif


typedef enum{	
	MODE_IDLE 	= 0x00,
	MODE_KEEP,
	MODE_NORMAL,
	MODE_FAST,
	MODE_PDL,
	MODE_DOSAGE,/*	����ע��ģʽ	*/
	MODE_SETTING,
	MODE_CHANGE_CARTRIDGE,
}PAGE_MODE_SELECT;

typedef enum{	/*	Mode�����У������Ҫ��������ת״̬	*/
	MODE_M_NEED_IDLE = 0,
	MODE_M_NEED_FWD,
	MODE_M_NEED_REV,
	MODE_M_FWD_GOAL
}MODE_M_CHANGE_DIR_T;

typedef enum{	/*	��̤��ǰ״̬	*/
	FP_IDLE = 0,
	FP_NO_CONNECT,
	FP_CONNECTED,
	FP_DELTA_1,
	FP_DELTA_2,
	FP_DELTA_3
}FOOT_PEDAL_STATE_T;

/*	����ؼ�ID	*/
#define PAGE_MODE_ASP_ID			0x01		/*	������������ID	*/
#define PAGE_MODE_DOSE_ID			0x02		/*	ҩҺ����ID	*/
#define PAGE_MODE_PRES_ID			0x03		/*	Һѹͼ��ؼ�ID	*/
#define PAGE_MODE_kPa_ID			0x04		/*	����Һѹֵ	*/
#define PAGE_MODE_FP_ID				0x05		/*	��̤ID	*/
#define PAGE_MODE_SPEED_ID			0x06		/*	����ٶ�ָʾID	*/
#define PAGE_MODE_FUNC_ID			0x07		/*	���ܰ�ťID	*/
#define PAGE_MODE_ADJECT_DOSE_ID	0x08		/*	����ע����	*/

#define PRES_PDL_START_INDEX	0x09	/*	PDL��ʾѹ��ͼ����ʼ����	*/
#define PRES_OTHER_START_INDEX	0x00	/*	��PDL��ʾѹ��ͼ����ʼ���� */

extern bool GO_ORIGIN_FLAG;
extern bool GO_TOP_FLAG;
extern bool START_ASP_FLAG;
extern int16_t asp_cycle;

void Page_Mode(void);
void Page_Mode_Alarm(void);

#endif