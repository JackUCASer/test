#ifndef __LCD_DISPLAY_H
#define __LCD_DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include "vgus_lcd_if.h"

/*	���н���ĵ��ID	*/
#define PAGE_BAT_ID					0x02

void display_battary_state(void);			/*	��ʾ���״̬	*/

#endif
