#ifndef __PAGE_SETTING_H
#define __PAGE_SETTING_H

#include "lcd_display.h"

typedef enum{
	SETTING_IDLE = 0x00,
	SETTING_KEEP, 
	SETTING_EXIT,
	SETTING_2_CALIBRATION, 
	SETTING_2_AGING
}PAGE_SETTING_SELECT;
/*	Setting界面控件ID	*/
#define PAGE_SETT_BACK_ID		0x01
#define PAGE_SETT_SMART_REMD	0x03
#define PAGE_SETT_TRAIN_MODE	0x04
#define PAGE_SETT_LIGHT_ID		0x05		/*	亮度调节滑动条控件	*/
#define PAGE_SETT_VOL_ID		0x06		/*	音量调节滑动条控件	*/
#define PAGE_SECRET_KEY1_ID		0x07		/*	隐蔽按钮KEY_1	*/
#define PAGE_SECRET_KEY2_ID		0x08		/*	隐蔽按钮KEY_2	*/


void page_setting(void);

#endif
