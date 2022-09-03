#ifndef __LCD_DISPLAY_H
#define __LCD_DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include "vgus_lcd_if.h"

/*	所有界面的电池ID	*/
#define PAGE_BAT_ID					0x02
#define PAGE_AUTOBACK_ID			0x03

void display_battary_state(void);			/*	显示电池状态	*/

#endif
