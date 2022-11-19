#ifndef __PAGE_SETTING_H
#define __PAGE_SETTING_H

#include "lcd_display.h"

typedef enum{
	SETTING_IDLE = 0x00,
	SETTING_KEEP, 
	SETTING_EXIT,
	SETTING_2_CALIBRATION, 
	SETTING_2_AGING,
	SETTING_2_CHECKHANDLE,
	SETTING_2_UPGRADE,
}PAGE_SETTING_SELECT;
/*	Setting界面控件ID	*/
#define PAGE_SETT_BACK_ID		0x01
#define PAGE_SETT_SMART_REMD	0x03
#define PAGE_SETT_TRAIN_MODE	0x04
#define PAGE_SETT_LIGHT_ID		0x05		/*	亮度调节滑动条控件	*/
#define PAGE_SETT_VOL_ID		0x06		/*	音量调节滑动条控件	*/
#define PAGE_SECRET_KEY1_ID		0x07		/*	隐蔽按钮KEY_1	*/
#define PAGE_SECRET_KEY2_ID		0x08		/*	隐蔽按钮KEY_2	*/
#define PAGE_CHECK_O_RING_ID	0x09		/*	检查O型圈按钮	*/
#define PAGE_SETT_BACK_OFF_ID	0x0A		/*	开机自动回退按钮：默认开启，连续五次关闭开机自动回退	*/
#define PAGE_SETT_UPGEADE_ID	0x0B		/*	升级按钮: 连续点击四次进入升级界面	*/

#define PAGE_UPGRADE_ID		0x0C01			/*	升级界面图标起始ID	*/


void page_setting(void);

#endif
