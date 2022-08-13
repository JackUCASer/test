#ifndef __DC_LCD_IF_H
#define __DC_LCD_IF_H
#include <stdint.h>

/*	����һ���豸�ӿ�����	*/
typedef struct{
	uint8_t* pcRevBuff;		/*	��ʴ������������ݵ������Ĵ��ڽ��ջ��λ������׵�ַ		*/
	uint16_t usReadPos;		/*	��ǰ�Ѷ����ջ��λ�������ƫ�Ƶ�ַ	*/
	uint16_t* psRevPos;		/*	��ǰ��������������ƫ�Ƶ�ַ	*/
	uint16_t usMaxPos;		/*	���ջ��λ������������	*/
	void(*dev_init)(void);
}DC_DEV_T;

/*-------------------------------------ָ��ṹ--------------------------------------*/
/*	������йصĽṹ��	*/
typedef struct{
	uint16_t	page_id;				/*	��ǰ��Ļ���ڽ���	*/
	uint8_t		page_wait_read;			/*	��ǰpageָ���Ƿ񱻶�ȡ	0���Ѷ�ȡ��	1�� �л����л�������δ��ȡ	*/
}LCD_PAGE_T;
/*	�밴ť��صĽṹ������	*/
typedef struct
{
	uint16_t 	page_id;				/*	��ǰ�����°�ť���ڵ�ҳ��	*/
	uint16_t 	touch_id;				/*	��ǰtouch��id	*/
	uint8_t 	touch_state;			/*	��ǰtouch��״̬	*/
	uint8_t 	touch_wait_read;		/*	��ǰtouch�Ƿ񱻶�ȡ  0���Ѷ�ȡ��1����touch������δ����ȡ		*/
}LCD_TOUCH_T;

/*	���������صĽṹ������	*/
typedef struct
{
	uint16_t	page_id;				/*	��ǰ�ı��ؼ����ڵ�ҳ��	*/
	uint16_t	text_id;				/*	��ǰ�ı��ؼ���id	*/
	uint8_t*	pc_text;				/*	ָ���ı���ָ��	*/	
	uint8_t		text_lenth;				/*	�ı����ݵĳ���	*/
	uint8_t		text_wait_read;			/*	��ǰ�ı��Ƿ񱻶�ȡ  0���Ѷ�ȡ��1��δ����ȡ	*/
}LCD_TEXT_T;

/*	���������صĽṹ������	*/
typedef struct
{
	uint16_t 	page_id;				/*	��ǰ���������ڵ�ҳ��	*/
	uint16_t 	progress_id;			/*	��ǰprogress��id	*/
	uint32_t 	progress_value;			/*	��ǰprogress��ֵ	*/
	uint8_t 	progress_wait_read;		/*	��ǰprogress�Ƿ񱻶�ȡ  0���Ѷ�ȡ��1��δ����ȡ	*/
}LCD_PROGRESS_T;

/*	�뻬����صĽṹ������	*/
typedef struct
{
	uint16_t 	page_id;				/*	��ǰ�������ڵ�ҳ��	*/
	uint16_t 	slider_id;				/*	��ǰslider��id	*/
	uint32_t 	slider_value;			/*	��ǰslider��ֵ	*/
	uint8_t 	slider_wait_read;		/*	��ǰslider�Ƿ񱻶�ȡ  0���Ѷ�ȡ��1����slider������δ����ȡ	*/
}LCD_SLIDER_T;

/*	����Ƶ��صĽṹ������	*/
typedef struct
{
	uint16_t 	page_id;				/*	��ǰ�������ڵ�ҳ��	*/
	uint16_t 	video_id;				/*	��ǰslider��id	*/
	uint8_t 	video_value;			/*	��ǰslider��ֵ	*/
	uint8_t 	video_wait_read;		/*	��ǰslider�Ƿ񱻶�ȡ  0���Ѷ�ȡ��1����slider������δ����ȡ	*/
}LCD_VIDEO_T;

extern uint8_t unlock_lcd_sys_settings[5];
extern uint8_t lock_lcd_sys_settings[5];
extern uint8_t close_lcd_touch[2];
extern uint8_t open_lcd_touch[2];

extern uint16_t usShould_page_id;
/*	�ⲿ�ӿ�		*/
extern DC_DEV_T dc_dev_t;
void Extract_LcdCmd(void);
void Analysis_LcdCmd(void);
void check_sendBuf_and_sendIt(void);
void page_others(void);

void send_dc_frame_head(void);
void send_dc_frame_end(void);
void send_data_to_dc_len(uint8_t* buf, uint8_t buf_len);
void send_data_to_dc(uint8_t* buf);
void send_data_to_dc_withFixLen(uint8_t* buf, uint8_t buf_len);
void load_sendBuf_into_sendQueue(uint8_t* buf, uint8_t buf_len);
void send_data_to_dc_IT(uint8_t *buf, uint8_t buf_len);
char* my_itoa(int32_t num, char* str);
#endif
