#ifndef __VGUS_LCD_IF_H
#define __VGUS_LCD_IF_H
#include <stdint.h>
#include "GL_Config.h"
/*		FreeRTOS		*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"
#include "semphr.h"

#define __DEBUG_VGUS

#ifdef __DEBUG_VGUS
#define VGUS_LCD(format, ...)	myPrintf(format, ##__VA_ARGS__)
#else	
#define VGUS_LCD(format, ...)
#endif

/*	����һ���豸�ӿ�����	*/
typedef struct{
	uint8_t* pcRevBuff;		/*	��ʴ������������ݵ������Ĵ��ڽ��ջ��λ������׵�ַ		*/
	uint16_t usReadPos;		/*	��ǰ�Ѷ����ջ��λ�������ƫ�Ƶ�ַ	*/
	uint16_t* psRevPos;		/*	��ǰ��������������ƫ�Ƶ�ַ	*/
	uint16_t usMaxPos;		/*	���ջ��λ������������	*/
	void(*dev_init)(void);
}VGUS_DEV_T;

/*	����һ��ָ��ṹ	*/
#define ucRevCmdSize	20		/*	��������ָ����󳤶�	*/
#define ucSendCmdSize	30		/*	��������ָ����󳤶�	*/

//	ָ����սṹ��
typedef struct{
	uint8_t ucCmdSize;
	uint8_t ucRevCmd[ucRevCmdSize];
}LCD_CMD_T;

extern LCD_CMD_T lcd_cmd_t;
extern QueueHandle_t 	qLcd_cmd_handle;
extern VGUS_DEV_T 		vgus_dev_t;

extern SemaphoreHandle_t 	sPage_mode;			/*	֪ͨ���빤��ģʽ	*/
extern SemaphoreHandle_t	sPage_sett;			/*	֪ͨ��������ģʽ	*/
extern SemaphoreHandle_t	sPage_calib;			/*	֪ͨ����У������	*/
extern SemaphoreHandle_t	sPage_aging;			/*	֪ͨ�����ϻ�����	*/

void Extract_LcdCmd(void);
void Analysis_LcdCmd(void);
void check_sendBuf_and_sendBuf(void);


/*-------------------------------------ָ��ṹ--------------------------------------*/
//	VGUS�и�����ַ����F��ͷ
//	���н���
typedef enum{
	PAGE_PowerOn 		= 0x00,
	PAGE_CheckHandle 	= 0x01,
	PAGE_CheckOring 	= 0x03,
	PAGE_Mode 			= 0x05,
	PAGE_Mode2			= 0x06,
	PAGE_Setting 		= 0x07,
	PAGE_Calibration	= 0x08,
	PAGE_Aging			= 0x09,
	PAGE_PowerOff		= 0x0A,
	PAGE_PowerOff_End	= 0x0B
}DISPLAY_PAGE_T;


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
	uint16_t 	touch_state;			/*	��ǰtouch��״̬	*/
	uint8_t 	touch_wait_read;		/*	��ǰtouch�Ƿ񱻶�ȡ  0���Ѷ�ȡ��1����touch������δ����ȡ		*/
}LCD_TOUCH_T;

/*	����Ƶ��صĽṹ������	*/
typedef struct
{
	uint16_t 	page_id;				/*	��ǰ�������ڵ�ҳ��	*/
	uint16_t 	video_id;				/*	��ǰslider��id	*/
	uint8_t 	video_value;			/*	��ǰslider��ֵ	*/
	uint8_t 	video_wait_read;		/*	��ǰslider�Ƿ񱻶�ȡ  0���Ѷ�ȡ��1����slider������δ����ȡ	*/
}LCD_VIDEO_T;



extern DISPLAY_PAGE_T now_display_page;
/*	��Ļ�����Ի��ӿ�	*/
extern LCD_TOUCH_T lcd_touch_t;

void set_now_brightness(uint8_t light_level);
void get_now_brightness(void);
void set_now_page(uint8_t page_id);
void get_now_page(void);
void play_now_music(uint16_t music_num, uint8_t music_vol);
void stop_now_music(uint16_t music_num);
void play_now_video(uint16_t video_num, uint8_t video_vol);
void stop_now_video(void);
void set_wedget_video(uint16_t addr, uint16_t val);
void add_single_line_data(uint8_t ch, uint16_t ch_data);
void add_two_line_data(uint8_t ch1, uint16_t ch1_data, uint8_t ch2, uint16_t ch2_data);
void show_uint32_t_data(uint16_t addr, uint32_t n_cycle);
void show_uint16_t_data(uint16_t addr, uint16_t n_cycle);
void show_variable_icon(uint8_t page, uint8_t id, uint16_t n_frame);
void write_two_82H_cmd(uint16_t start_addr, uint16_t var1, uint16_t var2);
void write_three_82H_cmd(uint16_t start_addr, uint16_t var1, uint16_t var2, uint16_t var3);
void enable_vgus_touch(void);
void disable_vgus_touch(void);
void set_vgus_music_volume(uint8_t volume);
void set_vgus_video_volume(uint8_t volume);
void vgus_lcd_reboot(void);
#endif
