#ifndef __LCD_DISPLAY_H
#define __LCD_DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include "vgus_lcd_if.h"

#define N_PAGE						10		//	UI总界面个数
#define N_UPDATE_BAT				8		//	N_UPDATE_BAT*150ms更新一次电压图标

/*	所有界面的电池ID	*/
#define PAGE_BAT_ID					0x02
#define PAGE_AUTOBACK_ID			0x03

/*	界面对象属性结构体	*/
typedef struct{
	uint8_t 	obj;				//	界面值
	uint8_t 	obj_num;			//	界面中对象的个数
	uint8_t 	obj_id[10];			//	界面中对象的id
	uint8_t 	obj_value[10];		//	界面中对象的值
}VGUS_PAGE_S;


extern VGUS_PAGE_S *vgus_page_objs;
void Init_Page_Objs(void);
void Display_Battery(void);

#endif
