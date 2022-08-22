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
/*	Setting����ؼ�ID	*/
#define PAGE_SETT_BACK_ID		0x01
#define PAGE_SETT_SMART_REMD	0x03
#define PAGE_SETT_TRAIN_MODE	0x04
#define PAGE_SETT_LIGHT_ID		0x05		/*	���ȵ��ڻ������ؼ�	*/
#define PAGE_SETT_VOL_ID		0x06		/*	�������ڻ������ؼ�	*/
#define PAGE_SECRET_KEY1_ID		0x07		/*	���ΰ�ťKEY_1	*/
#define PAGE_SECRET_KEY2_ID		0x08		/*	���ΰ�ťKEY_2	*/
#define PAGE_CHECK_O_RING_ID	0x09		/*	���O��Ȧ��ť	*/
#define PAGE_SETT_BACK_OFF_ID	0x0A		/*	�����Զ����˰�ť��Ĭ�Ͽ�����������ιرտ����Զ�����	*/
#define PAGE_SETT_UPGEADE_ID	0x0B		/*	������ť: ��������Ĵν�����������	*/

#define PAGE_UPGRADE_ID		0x0C01			/*	��������ͼ����ʼID	*/


void page_setting(void);

#endif
