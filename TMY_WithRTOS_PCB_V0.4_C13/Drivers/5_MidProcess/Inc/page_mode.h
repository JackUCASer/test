#ifndef __PAGE_MODE_H
#define __PAGE_MODE_H

#include "lcd_display.h"
#include <stdbool.h>

#include "GL_Config.h"

//#define __DEBUG_MODE

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
	MODE_DOSAGE,	/*	����ע�����	*/
	MODE_SETTING,
	MODE_CHANGE_CARTRIDGE,
}PAGE_MODE_SELECT;

typedef enum{	/*	Mode�����У������Ҫ��������ת״̬	*/
	MODE_M_NEED_IDLE = 0,
	MODE_M_NEED_FWD,
	MODE_M_NEED_REV
}MODE_M_CHANGE_DIR_T;

typedef enum{	/*	��̤��ǰ״̬	*/
	FP_IDLE = 0,
	FP_NO_CONNECT,
	FP_CONNECTED,
	FP_DELTA_1,
	FP_DELTA_2,
	FP_DELTA_3
}FOOT_PEDAL_STATE_T;

#define PAGE_MODE_ASP_ID		0x03		/*	������������ID	*/
#define PAGE_MODE_FUNC_ID		0x07		/*	���ܰ�ťID	*/
#define PAGE_MODE_FIX_DOSE		0x10		/*	����ע��ؼ�	*/
#define PAGE_MODE_ADJECT_DOSE	0x20		/*	����ע����	*/

#define PAGE_MODE_FP_ID			0x01		/*	��̤ID	*/
#define PAGE_MODE_SPEED_ID		0x04		/*	����ٶ�ָʾID	*/
#define PAGE_MODE_PRES_ID		0x05		/*	Һѹ���߿ؼ�ID	*/
	#define PRES_PDL_START_INDEX	0x09	/*	PDL��ʾѹ��ͼ����ʼ����	*/
	#define PRES_OTHER_START_INDEX	0x00	/*	��PDL��ʾѹ��ͼ����ʼ���� */
#define PAGE_MODE_DOSE_ID		0x06		/*	ҩҺ����ID	*/


extern bool START_ASP_FLAG;
extern int16_t asp_cycle;

void page_mode(void);
void page_mode_alarm(void);

#endif
