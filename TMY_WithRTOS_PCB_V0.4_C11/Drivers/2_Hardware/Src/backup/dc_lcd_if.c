#include <string.h>
#include "lcd_common.h"
#include "dc_lcd_if.h"
/*		Debug			*/
#include "GL_Config.h"
#include "user_data.h"


/*		FreeRTOS		*/
#include "cmsis_os.h"
#include "freertos.h"
#include "queue.h"
#include "semphr.h"

uint8_t  dc_frame_head = 0xEE;
uint32_t dc_frame_end =  0xFFFCFFFF;
uint8_t  dc_frame_end2[4] = {0xFF, 0xFC, 0xFF, 0xFF};
uint8_t unlock_lcd_sys_settings[5] = {0x09, 0xDE, 0xED, 0x13, 0x31};
uint8_t lock_lcd_sys_settings[5] = {0x08, 0xA5, 0x5A, 0x5F, 0xF5};
uint8_t close_lcd_touch[2] = {0x70, 0x70};
uint8_t open_lcd_touch[2] = {0x70, 0x71};

/*	定义一个设备数据提取状态	*/
typedef enum{
	READ_DC_IDLE = 0,
	READ_DC_FRAME_HEAD,
	READ_DC_FRAME_DATA,
	READ_DC_FRAME_END
}GET_DC_CMD_STA;

/*	定义一个指令结构	*/
#define ucRevCmdSize	20		/*	单条接收指令最大长度	*/
#define ucSendCmdSize	30		/*	单条发送指令最大长度	*/
//	指令接收结构体
typedef struct{
	uint8_t ucCmdSize;
	uint8_t ucRevCmd[ucRevCmdSize];
}LCD_CMD_T;
//	指令发送结构体
typedef struct{
	uint8_t ucSendSize;
	uint8_t ucSendCmd[ucSendCmdSize];
}SEND_LCD_T;

/*-------------------------------------------------------------------------------*/
/*	函数原型声明	*/
static void dc_dev_init(void);


/*	初始化设备	*/
DC_DEV_T dc_dev_t ={
	.pcRevBuff 	= &ucUsartRevBuff[0],			/*	数据接收缓冲区首地址	*/
	.usReadPos 	= 0,							/*	当前读取数据的偏移索引	*/
	.psRevPos	= &usUsartRevCurrentPos,		/*	当前接收缓冲区新数据偏移地址	*/
	.usMaxPos	= USART_REV_MAX_BUF_SIZE,		/*	最大可读取数据数	*/
	.dev_init	= dc_dev_init					/*	lcd初始化函数	*/
};

static LCD_CMD_T lcd_cmd_t = {0};
static SEND_LCD_T send_lcd_t = {0};
/*	消息队列	*/
QueueHandle_t 		qLcd_cmd_handle = NULL;
QueueHandle_t		qLcd_send_handle = NULL;
/*	信号量	*/
SemaphoreHandle_t 	sPage_mode = NULL;			/*	通知进入工作模式	*/
SemaphoreHandle_t	sPage_sett = NULL;			/*	通知进入设置模式	*/
SemaphoreHandle_t	sPage_calib = NULL;			/*	通知进入校正流程	*/
SemaphoreHandle_t	sPage_aging = NULL;			/*	通知进入老化流程	*/


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-13
* 函 数 名: dc_dev_init
* 功能说明: 设备初始化
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void dc_dev_init(void)
{
	// 1. 缓冲区初始化
	qLcd_cmd_handle = xQueueCreate(10, sizeof(lcd_cmd_t));		/*	初始化10个存储 LCD_CMD_T 类型指针变量的消息队列	*/
	qLcd_send_handle = xQueueCreate(10, sizeof(send_lcd_t));	/*	初始化10个存储 SEND_LCD_T 类型指针变量的消息队列	*/
	
	// 2. 相关信号量始化
	sPage_mode = xSemaphoreCreateBinary();						/*	初始化二值信号量 sPage_mode	*/
	sPage_sett = xSemaphoreCreateBinary();						/*	初始化二值信号量 sPage_sett	*/
	sPage_calib = xSemaphoreCreateBinary();						/*	初始化二值信号量 sPage_calib	*/
	sPage_aging = xSemaphoreCreateBinary();						/*	初始化二值信号量 sPage_aging	*/
	
	// 3. 外设初始化
	
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-13
* 函 数 名: Extract_LcdCmd
* 功能说明: 提取Lcd 指令
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/

void Extract_LcdCmd(void)
{
	static GET_DC_CMD_STA  get_dc_cmd_sta = READ_DC_IDLE;
	static uint8_t* pcRevCmd = lcd_cmd_t.ucRevCmd;
	static uint32_t uiRevEnd = 0;
	
	switch(get_dc_cmd_sta){
		case READ_DC_IDLE:
			if(dc_dev_t.usReadPos == *(dc_dev_t.psRevPos))					/*	当前读取数据索引是否等于接收缓冲区新数据索引	*/
				break;
			else{
				if(dc_dev_t.pcRevBuff[dc_dev_t.usReadPos] == dc_frame_head)	/*	当前数据是否为帧头	*/
					get_dc_cmd_sta = READ_DC_FRAME_HEAD;
				else
					get_dc_cmd_sta = READ_DC_IDLE;
				
				if(dc_dev_t.usReadPos == USART_REV_MAX_BUF_SIZE)			/*	当前索引是否是缓冲区最大索引	*/
					dc_dev_t.usReadPos = 0;		/*	偏移地址归零	*/
				else				
					dc_dev_t.usReadPos ++;		/*	偏移地址+1	*/
			}	
		break;
		
		case READ_DC_FRAME_HEAD:
			get_dc_cmd_sta = READ_DC_FRAME_DATA;
			memset(lcd_cmd_t.ucRevCmd, 0, ucRevCmdSize);					/*	清空缓冲区，准备接收新指令	*/
			lcd_cmd_t.ucCmdSize = 0;
			pcRevCmd = lcd_cmd_t.ucRevCmd;									/*	指令存储缓冲区起始地址	*/
			
		break;
		
		case READ_DC_FRAME_DATA:
			while(1){
				*pcRevCmd =  dc_dev_t.pcRevBuff[dc_dev_t.usReadPos];
				uiRevEnd  = (uiRevEnd << 8) + *pcRevCmd;					/*	获取数据帧尾		*/
				lcd_cmd_t.ucCmdSize ++;										/*	指令长度计数器	*/
			
				if(dc_dev_t.usReadPos >= USART_REV_MAX_BUF_SIZE)			/*	当前索引是否是缓冲区最大索引	*/
					dc_dev_t.usReadPos = 0;									/*	偏移地址归零		*/
				else				
					dc_dev_t.usReadPos ++;									/*	偏移地址+1	*/
				
				if(pcRevCmd == &(lcd_cmd_t.ucRevCmd[ucRevCmdSize-1])){		/*	lcd_cmd_t.ucRevCmd数据溢出	*/
					pcRevCmd = pcRevCmd;
					myPrintf("Log warning %d: Current lcd_cmd_t.ucRevCmd overflow!\r\n", 1);	/*	打印日志	*/
				}else
					pcRevCmd ++;
			
				if(uiRevEnd == dc_frame_end){
					get_dc_cmd_sta = READ_DC_FRAME_END;						/*	接收完成		*/
					lcd_cmd_t.ucCmdSize -=4;	//	去除结束符
					break;
				}else
					get_dc_cmd_sta = READ_DC_FRAME_DATA;					/*	继续接收		*/
			}
		break;
		
		case READ_DC_FRAME_END:
			xQueueSend(qLcd_cmd_handle, &lcd_cmd_t, (TickType_t)10);		/*	解析的指令存储至队列中	*/		
			get_dc_cmd_sta = READ_DC_IDLE;		/*	接收完成	*/
		break;
	}
		
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2021-04-21
* 函 数 名: Analysis_LcdCmd
* 功能说明: 解析Lcd指令并执行相关回调函数
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
typedef void (*DO_LCD_CMD)(LCD_CMD_T*);					/*	声明一个指向输入参数为LCD_CMD_T*类型，返回值为void 的函数指针类型	*/
/*	原型声明	*/
static void config_ctr_cmd(LCD_CMD_T* lcd_cmd_temp);	/*	组态指令处理原型声明		*/
static void config_no_use_cmd(LCD_CMD_T* lcd_cmd_temp);	/*	可忽略指令处理原型声明	*/
void Analysis_LcdCmd(void)
{
	BaseType_t result;
	LCD_CMD_T lcd_cmd_temp = {0};
	static DO_LCD_CMD do_lcd_cmd = NULL;
	/*	1. 指令出队		*/
	result = xQueueReceive(qLcd_cmd_handle,&lcd_cmd_temp, ( TickType_t )10);
	if(result == pdPASS){
		switch(lcd_cmd_temp.ucRevCmd[0]){
			case 0xB1:	/*	组态指令		*/
				do_lcd_cmd = config_ctr_cmd;	/*	执行组态指令	*/
				break;
			
			/*-------------------------基本图形指令集---------------------------*/
			case 0x01:											/*	清屏指令		*/
			case 0x05:											/*	图呈指令	*/
			case 0x46:	case 0x47:	case 0x34:					/*	截图指令		*/
			case 0x43:	case 0x45:	case 0x20:	case 0x21:		/*	文字指令		*/
			case 0x44:	case 0x32:	case 0x80:	case 0x33:		/*	图片显示		*/
			case 0x51:	case 0x54:	case 0x52:					/*	简单图形1	*/
			case 0x58:	case 0x67:								/*	简单图形2	*/
			case 0x59:	case 0x76:	case 0x69:					/*	曲线	*/
			// TO DO 
			do_lcd_cmd = config_no_use_cmd;
			break;
			
			/*---------------------------基本指令集-----------------------------*/
			case 0xB3:											/*	禁/使能屏幕更新	*/
			case 0x87:	case 0x88:								/*	存储器读写	*/
			case 0x77:	case 0x60:								/*	自动背光调节	*/
			// TO DO 
			do_lcd_cmd = config_no_use_cmd;
			break;
			
			case 0x18:				/*	18：解锁系统配置	*/
			do_lcd_cmd = config_no_use_cmd;
			break;
			
			case 0x17:				/*	17：锁定系统配置	*/
			do_lcd_cmd = config_no_use_cmd;	
			break;
			
			default: 
			do_lcd_cmd = config_no_use_cmd;	
			break;
		}
		do_lcd_cmd(&lcd_cmd_temp);								/*	执行具体操作	*/
	}else{
		do_lcd_cmd = NULL;
	}
}

LCD_PAGE_T	lcd_page_t = {0};				/*	0x01 画面切换	*/
LCD_TOUCH_T lcd_touch_t = {0};				/*	0x10 按键	*/	
LCD_PROGRESS_T lcd_progress_t = {0};		/*	0x12 进度条	*/
LCD_SLIDER_T lcd_slider_t = {0};			/*	0x13 滑动条	*/
LCD_VIDEO_T lcd_video_t = {0};				/*	0x75 视频播放结束	*/
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-03-06
* 函 数 名: config_ctr_cmd
* 功能说明: 处理组态指令
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
#include "lcd_display.h"
#include "alarm_module.h"
uint16_t usCurrent_page_id = 0;		/*	当前所在界面	*/
uint16_t usShould_page_id = 1;		/*	理论所在界面	*/
static void config_ctr_cmd(LCD_CMD_T* lcd_cmd_temp)
{
	static uint16_t usCtr_id = 0;
	static uint8_t ucCtr_type = 0;
	static bool first_power_on = true;
	switch(lcd_cmd_temp->ucRevCmd[1]){
		case 0x01:	/*	切换画面的通知	*/
			usCurrent_page_id = (lcd_cmd_temp->ucRevCmd[2]<<8) + lcd_cmd_temp->ucRevCmd[3];
			lcd_page_t.page_id = usCurrent_page_id;
			lcd_page_t.page_wait_read = 1;
			if(first_power_on)
				first_power_on = false;
			else
				page_track(usShould_page_id,usCurrent_page_id);
			myPrintf("Log Debug: Current we are in page %d\r\n", usCurrent_page_id);
		break;
			
		case 0x75:	/*	视频播放结束通知	*/
			usCurrent_page_id = (lcd_cmd_temp->ucRevCmd[2]<<8) + lcd_cmd_temp->ucRevCmd[3];
			lcd_video_t.page_id = usCurrent_page_id;
			lcd_video_t.video_id = (lcd_cmd_temp->ucRevCmd[4]<<8) + lcd_cmd_temp->ucRevCmd[5];
			lcd_video_t.video_value = lcd_cmd_temp->ucRevCmd[6];
			lcd_video_t.video_wait_read = 1;
		break;
		
		case 0x11:	/*	返回控件的值	*/
			usCurrent_page_id = (lcd_cmd_temp->ucRevCmd[2]<<8) + lcd_cmd_temp->ucRevCmd[3];
			usCtr_id = (lcd_cmd_temp->ucRevCmd[4]<<8) + lcd_cmd_temp->ucRevCmd[5];
			ucCtr_type = lcd_cmd_temp->ucRevCmd[6];
			switch(ucCtr_type){
				case 0x10: //	当前为按钮，将读取的数据存储在lcd_touch_t中
					lcd_touch_t.page_id = usCurrent_page_id;
					lcd_touch_t.touch_id = usCtr_id;
					lcd_touch_t.touch_state = lcd_cmd_temp->ucRevCmd[8];
					lcd_touch_t.touch_wait_read = 1;		//	标志位，等待相关应用读取
					myPrintf("Log Debug: page %d bottom %d is pressed and its state is %d!\r\n", lcd_touch_t.page_id, lcd_touch_t.touch_id,lcd_touch_t.touch_state);
					break;
				
				case 0x12: //	当前为进度条，将读取的数据存储在lcd_progress_t中
					lcd_progress_t.page_id = usCurrent_page_id;
					lcd_progress_t.progress_id = usCtr_id;
					lcd_progress_t.progress_value = (lcd_cmd_temp->ucRevCmd[7]<<24) + (lcd_cmd_temp->ucRevCmd[8]<<16) \
												  + (lcd_cmd_temp->ucRevCmd[9]<<8) + lcd_cmd_temp->ucRevCmd[10];
					lcd_progress_t.progress_wait_read = 1;	//	标志位，等待相关应用读取
					myPrintf("Log Debug: page %d progress %d is pressed and its value is %d!\r\n", lcd_progress_t.page_id, lcd_progress_t.progress_id,lcd_progress_t.progress_value);
					break;
				
				case 0x13: //	当前为滑动条，将读取的数据存储在lcd_slider_t中
					lcd_slider_t.page_id = usCurrent_page_id;
					lcd_slider_t.slider_id = usCtr_id;
					lcd_slider_t.slider_value = (lcd_cmd_temp->ucRevCmd[7]<<24) + (lcd_cmd_temp->ucRevCmd[8]<<16) \
											  + (lcd_cmd_temp->ucRevCmd[9]<<8) + lcd_cmd_temp->ucRevCmd[10];
					lcd_slider_t.slider_wait_read = 1;		//	标志位，等待相关应用读取
					myPrintf("Log Debug: page %d slider %d is pressed and its value is %d!\r\n", lcd_slider_t.page_id, lcd_slider_t.slider_id,lcd_slider_t.slider_value);
					break;
				
				default: break;
				
			}
		break;
			
		default: break;
	}	
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-13
* 函 数 名: config_no_use_cmd
* 功能说明: 执行可忽略指令
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void config_no_use_cmd(LCD_CMD_T* lcd_cmd_temp)
{
	switch(lcd_cmd_temp->ucRevCmd[0]){
		
		case 0x07:
			myPrintf("Log notice %d: LCD reboot OK!\r\n", 0x07);
		break;
		
		case 0x17:
			myPrintf("Log notice %d: Lock screen touch!\r\n", 0x17);	
		break;
		
		case 0x18:
			myPrintf("Log notice %d: Unlock screen touch!\r\n", 0x18);	
		break;

		default: break;
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.1
* 日    期：2022-03-16
* 函 数 名: page_others
* 功能说明: 界面中枢
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
static void run_check_handle(void);
static void run_check_o_ring(void);
static void run_check_idle(void);
typedef void (*DO_OTHER_PAGE)(void);	/*	声明一个指向输入参数为LCD_CMD_T*类型，返回值为void 的函数指针类型	*/
DO_OTHER_PAGE do_other_page = run_check_idle;
extern osThreadId FactoryCalibHandle;
extern osThreadId FactoryAgingHandle;
extern osThreadId ModeHandle;
extern osThreadId SettingHandle;
void page_others(void)
{
	if(lcd_page_t.page_wait_read == 1){
		lcd_page_t.page_wait_read = 0;		/*	界面切换指令已被读取	*/
		switch(lcd_page_t.page_id){
			case PowerOn:			//	当前处于开机界面
				display_video_start();
				do_other_page = run_check_idle;
			break;
		
			case MODE:			//	当前处于Mode界面
				xSemaphoreGive(sPage_mode);
				vTaskResume(ModeHandle);			//	恢复Mode进程，关闭其他进程
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_idle;
			break;
			
			case SETTING:		//	当前处于SETTING界面
				xSemaphoreGive(sPage_sett);		
				vTaskResume(SettingHandle);			//	恢复Setting进程，关闭其他进程					
				vTaskSuspend(ModeHandle);			
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_idle;
			break;
			
			case CALIBRATION:	//	当前处于CALIBRATION界面
				xSemaphoreGive(sPage_calib);
				vTaskResume(FactoryCalibHandle);	//	恢复Calibration进程，关闭其他进程
				vTaskSuspend(SettingHandle);
				vTaskSuspend(ModeHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_idle;
			break;
			
			case AGING:			//	当前处于AGING界面
				xSemaphoreGive(sPage_aging);
				vTaskResume(FactoryAgingHandle);	//	恢复Aging进程，关闭其他进程
				vTaskSuspend(ModeHandle);
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				do_other_page = run_check_idle;
			break;
			
			case CheckHandle:	//	当前处于确认手柄界面
				vTaskSuspend(ModeHandle);
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_handle;
				break;
			
			case CheckORing:		//	当前处于确认“O”型圈界面
				vTaskSuspend(ModeHandle);
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_o_ring;
				break;
			
			default: break;
		}
	}
	
	do_other_page();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-24
* 函 数 名: run_check_handle
* 功能说明: 提醒用户确认手柄未安装套筒和药瓶
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sMotor_goTop;
extern bool handle_in_top;
static void run_check_handle(void)
{
	static uint8_t key_secret = 0; 
	
	if((lcd_touch_t.page_id == CheckHandle)&&(lcd_touch_t.touch_id == PAGE_CHECKHANDLE_OK)&&(lcd_touch_t.touch_wait_read == 1)){
		lcd_touch_t.touch_wait_read = 0;
		go_to_page(CheckORing);
		handle_in_top = false;
		xSemaphoreGive(sMotor_goTop);
		key_secret = 0;
		
	}else if((lcd_touch_t.page_id == CheckHandle)&&(lcd_touch_t.touch_id == PAGE_CHECKHANDLE_KEY1_ID)&&(lcd_touch_t.touch_wait_read == 1)){
		lcd_touch_t.touch_wait_read = 0;
		key_secret = (key_secret << 1);
		key_secret += 1;
		myPrintf("key_secret = %d\r\n", key_secret);
	}else if((lcd_touch_t.page_id == CheckHandle)&&(lcd_touch_t.touch_id == PAGE_CHECKHANDLE_KEY2_ID)&&(lcd_touch_t.touch_wait_read == 1)){
		lcd_touch_t.touch_wait_read = 0;
		key_secret = (key_secret << 1);
		key_secret += 0;
		myPrintf("key_secret = %d\r\n", key_secret);
	}
	
	switch(key_secret){
		case 0xAC:
			go_to_page(CALIBRATION);			/*	进入校正模式	*/
			key_secret = 0;
		break;
	
		default: break;
	}

}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-24
* 函 数 名: run_check_o_ring
* 功能说明: 提醒用户确认手柄未安装套筒和药瓶
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sMotor_goBottom;
extern bool handle_in_bottom;
static void run_check_o_ring(void)
{
	static bool first_this_func = true;
	static bool first_enable_check_o_ring_key = true;
	static uint8_t buf[7] = {0xB1, 0x04, 0x00, CheckORing, 0x00, PAGE_CHECKORING_OK, 0x00};
	
	static bool start_check_bottom = false;

	if(handle_in_top == false){										/*	等待丝杆到达顶部	*/
		if(first_this_func == true){
			buf[6] = 0x00;
			load_sendBuf_into_sendQueue(buf, 7);					/*	失能控件PAGE_CHECKORING_OK	*/
			first_this_func = false;
		}
	}else{
		if(first_enable_check_o_ring_key == true){
			buf[6] = 0x01;
			load_sendBuf_into_sendQueue(buf, 7);					/*	使能控件PAGE_CHECKORING_OK	*/
			first_enable_check_o_ring_key = false;
		}
	}
	
	if((lcd_touch_t.page_id == CheckORing)&&(lcd_touch_t.touch_id == PAGE_CHECKORING_OK)&&(lcd_touch_t.touch_wait_read == 1)){
		lcd_touch_t.touch_wait_read = 0;
		buf[6] = 0x00;
		load_sendBuf_into_sendQueue(buf, 7);						/*	失能控件PAGE_CHECKORING_OK	*/
		start_check_bottom = true;
		handle_in_bottom = false;
		xSemaphoreGive(sMotor_goBottom);
									
	}
	if((start_check_bottom == true)&&(handle_in_bottom == true)){	/*	等待丝杆到达底部	*/
		go_to_page(MODE);
		start_check_bottom = false;
	}else
		return ;
}

static void run_check_idle(void)
{
	uint32_t use_times = 0;
	/*	判断是否视频播放结束	*/
	if((lcd_video_t.video_wait_read ==1)&&(lcd_video_t.page_id == PowerOn)&&(lcd_video_t.video_id == PAGE_POWERON_VIDEO_ID)){
		lcd_video_t.video_wait_read = 0;
		use_times = read_use_times();
		if(use_times % N_TIME_CHECK_ORING != 0){	/*	跳过O型圈检查，直接进入Mode界面	*/
			go_to_page(MODE);
		}else{										/*	进入O型圈检查界面	*/
			go_to_page(CheckHandle);
		}
	}else
		return ;
}
#include "usart.h"
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-17
* 函 数 名: send_dc_frame_head
* 功能说明: 发送数据帧头
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void send_dc_frame_head(void)
{
	HAL_UART_Transmit(&huart2, &dc_frame_head, 1, 0x00FF);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-17
* 函 数 名: send_dc_frame_end
* 功能说明: 发送数据帧尾
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void send_dc_frame_end(void)
{
	HAL_UART_Transmit(&huart2, dc_frame_end2, 4, 0x00FF);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-17
* 函 数 名: send_data_to_dc_len
* 功能说明: 发送固定长度数据至大彩串口屏（不包含数据帧头和帧尾）
* 形    参: 
*			buf： 待发送数据地址
*			buf_len： 待发送数据长度
* 返 回 值: 
*********************************************************************************************************
*/
void send_data_to_dc_len(uint8_t* buf, uint8_t buf_len)
{
	HAL_UART_Transmit(&huart2, buf, buf_len, 0x00FF);
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-17
* 函 数 名: send_data_to_dc_len
* 功能说明: 发送变长数据至大彩串口屏（不包含数据帧头和帧尾）
* 形    参: 
*			buf： 待发送数据地址
* 返 回 值: 
*********************************************************************************************************
*/
void send_data_to_dc(uint8_t* buf)
{
	uint8_t i = 0;
	while(1){
		if(buf[i]!=0){
			HAL_UART_Transmit(&huart2, &buf[i], 1, 0x00FF);
			i++;
		}else
			return ;
	}
}
/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-12-17
* 函 数 名: send_data_to_dc_len
* 功能说明: 发送固定长度数据至大彩串口屏（包含数据帧头和帧尾）
* 形    参: 
*			buf： 待发送数据地址
*			buf_len： 待发送数据长度
* 返 回 值: 
*********************************************************************************************************
*/
void send_data_to_dc_withFixLen(uint8_t* buf, uint8_t buf_len)
{
	send_dc_frame_head();
	send_data_to_dc_len(buf, buf_len);
	send_dc_frame_end();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版    本：V1.0
* 日    期：2020-04-20
* 函 数 名: send_data_to_dc_len
* 功能说明: 发送固定长度数据至大彩串口屏（包含数据帧头和帧尾）
* 形    参: 
*			buf： 待发送数据地址
*			buf_len： 待发送数据长度
* 返 回 值: 
*********************************************************************************************************
*/
void load_sendBuf_into_sendQueue(uint8_t* buf, uint8_t buf_len)
{
	static SEND_LCD_T send_data_to_lcd = {0};
	if(buf_len > ucSendCmdSize)
		return ;
	memcpy(send_data_to_lcd.ucSendCmd, buf, buf_len);
	send_data_to_lcd.ucSendSize = buf_len;
	xQueueSend(qLcd_send_handle, &send_data_to_lcd, (TickType_t)10);		/*	将发送指令指令存储至队列中	*/	
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-04-14
* 函 数 名: send_data_to_dc_IT
* 功能说明: 发送固定长度数据至大彩串口屏（包含数据帧头和帧尾,中断方式），不建议使用;
			1. 中断会影响操作系统的稳定性
			2. 电机控制需要100us中断，且优先级最高；若使用串口中断，频繁通信产生的发射中断，会给系统带来压力；
* 形    参: 
*			buf： 待发送数据地址
*			buf_len： 待发送数据长度
* 返 回 值: 
*********************************************************************************************************
*/
void send_data_to_dc_IT(uint8_t *buf, uint8_t buf_len)
{
	/*	装在数据至发送缓冲区	*/
	ucUsartSendBuff[0] = dc_frame_head;
	ucUsartSendSize = 1;
	memcpy(&ucUsartSendBuff[1], buf, buf_len);
	ucUsartSendSize += buf_len;
	memcpy(&ucUsartSendBuff[ucUsartSendSize], dc_frame_end2, 4);
	ucUsartSendSize += 4;
	ucUsartSendCnt = 0;		/*	发送计数器清零	*/
	
	/*	开启串口发送为空中断	*/
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
}

// 检查发送缓冲区是否有指令需要发送
void check_sendBuf_and_sendIt(void)
{
	BaseType_t result = xQueueReceive(qLcd_send_handle, &send_lcd_t, ( TickType_t )10);
	if(result == pdPASS){	/*	出队成功	*/
		send_data_to_dc_withFixLen(send_lcd_t.ucSendCmd, send_lcd_t.ucSendSize);
		memset(&send_lcd_t, 0, sizeof(send_lcd_t));
	}
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-11-09
* 函 数 名: my_itoa
* 功能说明: 将整形转换为字符串(10进制)，
* 形    参: 
		num:	待转换成字符串的数字
		str:	保存转换后的字符串的地址
* 返 回 值: 
*********************************************************************************************************
*/
char* my_itoa(int32_t num, char* str)
{
	char index[]="0123456789";	//	索引表
    uint32_t unum;				//	存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;				//	i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；
								//	转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
    //	获取要转换的整数的绝对值
    if(num<0)					//	要转换成十进制数并且是负数
    {
        unum= -num;				//	将num的绝对值赋给unum
        str[i++]='-';			//	在字符串最前面设置为'-'号，并且索引加1
    }
    else unum = num;			//	若是num为正，直接赋值给unum
 
    //	转换部分，注意转换后是逆序的
    do
    {
        str[i++] =index[unum%10];//	取unum的最后一位，并设置为str对应位，指示索引加1
        unum /=10;				//	unum去掉最后一位
 
    }while(unum);				//	直至unum为0退出循环
 
    str[i]='\0';				//	在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
    //	将顺序调整过来
    if(str[0]=='-') k=1;		//	如果是负数，符号不用调整，从符号后面开始调整
    else k=0;					//	不是负数，全部都要调整
 
    char temp;					//	临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)		//	头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];			//	头部赋值给临时变量
        str[j]=str[i-1+k-j];	//	尾部赋值给头部
        str[i-1+k-j]=temp;		//	将临时变量的值(其实就是之前的头部值)赋给尾部
    }
    return str;					//	返回转换后的字符串
}
