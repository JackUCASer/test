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
	MODE_DOSAGE,	/*	定量注射界面	*/
	MODE_SETTING,
	MODE_CHANGE_CARTRIDGE,
}PAGE_MODE_SELECT;

typedef enum{	/*	Mode过程中，电机需要调整的运转状态	*/
	MODE_M_NEED_IDLE = 0,
	MODE_M_NEED_FWD,
	MODE_M_NEED_REV
}MODE_M_CHANGE_DIR_T;

typedef enum{	/*	脚踏当前状态	*/
	FP_IDLE = 0,
	FP_NO_CONNECT,
	FP_CONNECTED,
	FP_DELTA_1,
	FP_DELTA_2,
	FP_DELTA_3
}FOOT_PEDAL_STATE_T;

#define PAGE_MODE_ASP_ID		0x03		/*	回吸开启开关ID	*/
#define PAGE_MODE_FUNC_ID		0x07		/*	功能按钮ID	*/
#define PAGE_MODE_FIX_DOSE		0x10		/*	定量注射控件	*/
#define PAGE_MODE_ADJECT_DOSE	0x20		/*	调节注射量	*/

#define PAGE_MODE_FP_ID			0x01		/*	脚踏ID	*/
#define PAGE_MODE_SPEED_ID		0x04		/*	电机速度指示ID	*/
#define PAGE_MODE_PRES_ID		0x05		/*	液压曲线控件ID	*/
	#define PRES_PDL_START_INDEX	0x09	/*	PDL显示压力图标起始索引	*/
	#define PRES_OTHER_START_INDEX	0x00	/*	非PDL显示压力图标起始索引 */
#define PAGE_MODE_DOSE_ID		0x06		/*	药液余量ID	*/


extern bool START_ASP_FLAG;
extern int16_t asp_cycle;

void page_mode(void);
void page_mode_alarm(void);

#endif
