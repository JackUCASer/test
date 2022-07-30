#ifndef __DC_LCD_IF_H
#define __DC_LCD_IF_H
#include <stdint.h>

/*	定义一个设备接口类型	*/
typedef struct{
	uint8_t* pcRevBuff;		/*	大彩串口屏发送数据到主机的串口接收环形缓冲区首地址		*/
	uint16_t usReadPos;		/*	当前已读接收环形缓冲区的偏移地址	*/
	uint16_t* psRevPos;		/*	当前缓冲区接收数据偏移地址	*/
	uint16_t usMaxPos;		/*	接收环形缓冲区最大索引	*/
	void(*dev_init)(void);
}DC_DEV_T;

/*-------------------------------------指令结构--------------------------------------*/
/*	与界面有关的结构体	*/
typedef struct{
	uint16_t	page_id;				/*	当前屏幕所在界面	*/
	uint8_t		page_wait_read;			/*	当前page指令是否被读取	0：已读取；	1： 有画面切换，但是未读取	*/
}LCD_PAGE_T;
/*	与按钮相关的结构体类型	*/
typedef struct
{
	uint16_t 	page_id;				/*	当前被按下按钮所在的页面	*/
	uint16_t 	touch_id;				/*	当前touch的id	*/
	uint8_t 	touch_state;			/*	当前touch的状态	*/
	uint8_t 	touch_wait_read;		/*	当前touch是否被读取  0：已读取；1：被touch，但是未被读取		*/
}LCD_TOUCH_T;

/*	与进度条相关的结构体类型	*/
typedef struct
{
	uint16_t	page_id;				/*	当前文本控件所在的页面	*/
	uint16_t	text_id;				/*	当前文本控件的id	*/
	uint8_t*	pc_text;				/*	指向文本的指针	*/	
	uint8_t		text_lenth;				/*	文本内容的长度	*/
	uint8_t		text_wait_read;			/*	当前文本是否被读取  0：已读取；1：未被读取	*/
}LCD_TEXT_T;

/*	与进度条相关的结构体类型	*/
typedef struct
{
	uint16_t 	page_id;				/*	当前进度条所在的页面	*/
	uint16_t 	progress_id;			/*	当前progress的id	*/
	uint32_t 	progress_value;			/*	当前progress的值	*/
	uint8_t 	progress_wait_read;		/*	当前progress是否被读取  0：已读取；1：未被读取	*/
}LCD_PROGRESS_T;

/*	与滑块相关的结构体类型	*/
typedef struct
{
	uint16_t 	page_id;				/*	当前滑块所在的页面	*/
	uint16_t 	slider_id;				/*	当前slider的id	*/
	uint32_t 	slider_value;			/*	当前slider的值	*/
	uint8_t 	slider_wait_read;		/*	当前slider是否被读取  0：已读取；1：被slider，但是未被读取	*/
}LCD_SLIDER_T;

/*	与视频相关的结构体类型	*/
typedef struct
{
	uint16_t 	page_id;				/*	当前滑块所在的页面	*/
	uint16_t 	video_id;				/*	当前slider的id	*/
	uint8_t 	video_value;			/*	当前slider的值	*/
	uint8_t 	video_wait_read;		/*	当前slider是否被读取  0：已读取；1：被slider，但是未被读取	*/
}LCD_VIDEO_T;

extern uint8_t unlock_lcd_sys_settings[5];
extern uint8_t lock_lcd_sys_settings[5];
extern uint8_t close_lcd_touch[2];
extern uint8_t open_lcd_touch[2];

extern uint16_t usShould_page_id;
/*	外部接口		*/
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
