#ifndef __LCD_DISPLAY_H
#define __LCD_DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include "vgus_lcd_if.h"

#define N_PAGE						10		//	UI�ܽ������
#define N_UPDATE_BAT				8		//	N_UPDATE_BAT*150ms����һ�ε�ѹͼ��

/*	���н���ĵ��ID	*/
#define PAGE_BAT_ID					0x02
#define PAGE_AUTOBACK_ID			0x03

/*	����������Խṹ��	*/
typedef struct{
	uint8_t 	obj;				//	����ֵ
	uint8_t 	obj_num;			//	�����ж���ĸ���
	uint8_t 	obj_id[10];			//	�����ж����id
	uint8_t 	obj_value[10];		//	�����ж����ֵ
}VGUS_PAGE_S;


extern VGUS_PAGE_S *vgus_page_objs;
void Init_Page_Objs(void);
void Display_Battery(void);

#endif
