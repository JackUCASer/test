#ifndef __LCD_DISPLAY_H
#define __LCD_DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include "vgus_lcd_if.h"

/*	���н���ĵ��ID	*/
#define PAGE_MODE_BAT_ID			0x02
#define PAGE_SETTING_BAT_ID			0x02
#define PAGE_CALIBRATION_BAT_ID		0x04
#define PAGE_AGING_BAT_ID			0x01


void display_battary_state(void);			/*	��ʾ���״̬	*/

#endif
