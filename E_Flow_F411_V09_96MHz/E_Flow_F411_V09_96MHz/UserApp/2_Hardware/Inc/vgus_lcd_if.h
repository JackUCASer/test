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

/*	定义一个设备接口类型	*/
typedef struct{
	uint8_t* pcRevBuff;		/*	大彩串口屏发送数据到主机的串口接收环形缓冲区首地址		*/
	uint16_t usReadPos;		/*	当前已读接收环形缓冲区的偏移地址	*/
	uint16_t* psRevPos;		/*	当前缓冲区接收数据偏移地址	*/
	uint16_t usMaxPos;		/*	接收环形缓冲区最大索引	*/
	void(*dev_init)(void);
}VGUS_DEV_T;

/*	定义一个指令结构	*/
#define ucRevCmdSize	20		/*	单条接收指令最大长度	*/
#define ucSendCmdSize	30		/*	单条发送指令最大长度	*/

//	指令接收结构体
typedef struct{
	uint8_t ucCmdSize;
	uint8_t ucRevCmd[ucRevCmdSize];
}LCD_CMD_T;

extern uint8_t wait_send_over;
extern LCD_CMD_T lcd_cmd_t;
extern QueueHandle_t 	qLcd_cmd_handle;
extern VGUS_DEV_T 		vgus_dev_t;

extern SemaphoreHandle_t 	sPage_mode;			/*	通知进入工作模式	*/
extern SemaphoreHandle_t	sPage_sett;			/*	通知进入设置模式	*/
extern SemaphoreHandle_t	sPage_calib;			/*	通知进入校正流程	*/
extern SemaphoreHandle_t	sPage_aging;			/*	通知进入老化流程	*/

void Analysis_Lcd_Cmd(void);
void Check_Senfbuf_And_Send_Buf(void);


/*-------------------------------------指令结构--------------------------------------*/
//	VGUS中辅助地址均以F开头
//	所有界面
typedef enum{
	PAGE_PowerOn 		= 0x00,
	PAGE_CheckHandle 	= 0x01,
	PAGE_CheckHandle2	= 0x02,
	PAGE_CheckOring 	= 0x03,
	PAGE_CheckOring2	= 0x04,
	PAGE_Mode 			= 0x05,
	PAGE_Mode2			= 0x06,
	PAGE_Setting 		= 0x07,
	PAGE_MotorCalib		= 0x08,
	PAGE_Aging			= 0x09,
	PAGE_PowerOff		= 0x0A,
	PAGE_PowerOff_End	= 0x0B,
	PAGE_UPGRADE		= 0x0C,
	PAGE_FP_CALIB		= 0x0D,
}DISPLAY_PAGE_T;


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
	uint16_t 	touch_state;			/*	当前touch的状态	*/
	uint8_t 	touch_wait_read;		/*	当前touch是否被读取  0：已读取；1：被touch，但是未被读取		*/
}LCD_TOUCH_T;

/*	与视频相关的结构体类型	*/
typedef struct
{
	uint16_t 	page_id;				/*	当前滑块所在的页面	*/
	uint16_t 	video_id;				/*	当前slider的id	*/
	uint8_t 	video_value;			/*	当前slider的值	*/
	uint8_t 	video_wait_read;		/*	当前slider是否被读取  0：已读取；1：被slider，但是未被读取	*/
}LCD_VIDEO_T;



extern DISPLAY_PAGE_T now_display_page;
/*	屏幕操作对话接口	*/
extern LCD_TOUCH_T lcd_touch_t;

void Set_VGUS_Brightness(uint8_t light_level);
void Get_VGUS_Brightness(void);
void Set_VGUS_Page(uint8_t page_id);
void Set_Now_Page_When_VGUS_Reboot(uint8_t page_id);
void Set_Now_Page_When_VGUS_DisError(uint8_t page_id);
void Get_VGUS_Page(void);
void Play_Now_Music(uint16_t music_num, uint8_t music_vol);
void Stop_Now_Music(uint16_t music_num);
void Play_Now_Video(uint16_t video_num, uint8_t video_vol);
void Stop_Now_Video(void);
void Set_Wedget_Video(uint16_t addr, uint16_t val);
void Add_Single_Line_Data(uint8_t ch, uint16_t ch_data);
void Add_Two_Line_Data(uint8_t ch1, uint16_t ch1_data, uint8_t ch2, uint16_t ch2_data);
void Show_uint32_t_Data(uint16_t addr, uint32_t n_cycle);
void Show_uint16_t_Data(uint16_t addr, uint16_t n_cycle);
void Show_Variable_Icon(uint8_t page, uint8_t id, uint16_t n_frame);
void Write_Two_82H_Cmd(uint16_t start_addr, uint16_t var1, uint16_t var2);
void Write_Three_82H_Cmd(uint16_t start_addr, uint16_t var1, uint16_t var2, uint16_t var3);
void Enable_VGUS_Touch(void);
void Disable_VGUS_Touch(void);
void Set_VGUS_Music_Volume(uint8_t volume);
void Set_VGUS_Video_Volume(uint8_t volume);
void Set_VGUS_Display_String(uint16_t addr, uint8_t strLen, uint8_t* str);
void VGUS_LCD_Reboot(void);
void Close_VGUS_BACKLIGHT(void);
void Open_VGUS_BACKLIGHT(void);
//	清空发送缓冲区
void Clear_Lcd_Cmd_SendQueue(void);
#endif
