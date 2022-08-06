#include <string.h>
#include "usart.h"
#include "lcd_common.h"
#include "vgus_lcd_if.h"
#include "user_data.h"
#include "GL_Config.h"

/*	����	*/
#include "page_aging.h"
#include "page_calib.h"
#include "page_mode.h"
#include "page_setting.h"

uint16_t vgus_frame_head = 0xA55A;
uint8_t vgus_frame_head2[2] = {0xA5, 0x5A};

DISPLAY_PAGE_T now_display_page = PAGE_PowerOn;

//	ָ��ͽṹ��
typedef struct{
	uint8_t ucSendSize;
	uint8_t ucSendCmd[ucSendCmdSize];
}SEND_LCD_T;

typedef enum{
	READ_VGUS_IDLE = 0,
	READ_VGUS_FRAME_HEAD,
	READ_VGUS_FRAME_DATA,
	READ_DC_FRAME_END
}GET_VGUS_CMD_STA;

/*-------------------------------------------------------------------------------*/
static LCD_CMD_T lcd_cmd_t = {0};
static SEND_LCD_T send_lcd_t = {0};
/*	��Ϣ����	*/
QueueHandle_t 		qLcd_cmd_handle = NULL;
QueueHandle_t		qLcd_send_handle = NULL;
/*	�ź���	*/
SemaphoreHandle_t 	sPage_mode = NULL;			/*	֪ͨ���빤��ģʽ	*/
SemaphoreHandle_t	sPage_sett = NULL;			/*	֪ͨ��������ģʽ	*/
SemaphoreHandle_t	sPage_calib = NULL;			/*	֪ͨ����У������	*/
SemaphoreHandle_t	sPage_aging = NULL;			/*	֪ͨ�����ϻ�����	*/

LCD_PAGE_T	lcd_page_t = {0};
LCD_TOUCH_T lcd_touch_t = {0};
LCD_VIDEO_T lcd_video_t = {0};

static void vgus_dev_init(void);
VGUS_DEV_T vgus_dev_t ={
	.pcRevBuff 	= &ucUsartRevBuff[0],
	.usReadPos 	= 0,
	.psRevPos	= &usUsartRevCurrentPos,		
	.usMaxPos	= USART_REV_MAX_BUF_SIZE,		
	.dev_init	= vgus_dev_init					
};
	
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: dc_dev_init
* ����˵��: �豸��ʼ��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void vgus_dev_init(void)
{
	// 1. ��������ʼ��
	qLcd_cmd_handle = xQueueCreate(10, sizeof(lcd_cmd_t));		/*	��ʼ��10���洢 LCD_CMD_T ����ָ���������Ϣ����	*/
	qLcd_send_handle = xQueueCreate(10, sizeof(send_lcd_t));	/*	��ʼ��10���洢 SEND_LCD_T ����ָ���������Ϣ����	*/
	
	// 2. ����ź���ʼ��
	sPage_mode = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_mode	*/
	sPage_sett = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_sett	*/
	sPage_calib = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_calib	*/
	sPage_aging = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_aging	*/
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-05-11
* �� �� ��: Extract_LcdCmd
* ����˵��: ��ȡLCD������ָ��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Extract_LcdCmd(void)
{
	static GET_VGUS_CMD_STA  get_vgus_cmd_sta = READ_VGUS_IDLE;
	static uint16_t read2bytes = 0;
	
	switch(get_vgus_cmd_sta){
		case READ_VGUS_IDLE:
			if(vgus_dev_t.usReadPos == *(vgus_dev_t.psRevPos))
				break;
			else{
				read2bytes = (read2bytes <<8) + vgus_dev_t.pcRevBuff[vgus_dev_t.usReadPos];
				if(read2bytes == vgus_frame_head){
					get_vgus_cmd_sta = READ_VGUS_FRAME_HEAD;
					vgus_dev_t.pcRevBuff[vgus_dev_t.usReadPos] = 0;
				}else
					get_vgus_cmd_sta = READ_VGUS_IDLE;
				
				vgus_dev_t.usReadPos ++;
				vgus_dev_t.usReadPos &= USART_REV_MAX_BUF_SIZE;	/*	��֤�����	*/
			}	
		break;
			
		case READ_VGUS_FRAME_HEAD:
			lcd_cmd_t.ucCmdSize = vgus_dev_t.pcRevBuff[vgus_dev_t.usReadPos];
			vgus_dev_t.pcRevBuff[vgus_dev_t.usReadPos] = 0;
			vgus_dev_t.usReadPos ++;
			vgus_dev_t.usReadPos &= USART_REV_MAX_BUF_SIZE;		/*	��֤�����	*/
			if(lcd_cmd_t.ucCmdSize > 0)
				get_vgus_cmd_sta = READ_VGUS_FRAME_DATA;
			else	/*	δ���յ���Ч���ݣ��ص�IDLE״̬	*/
				get_vgus_cmd_sta = READ_VGUS_IDLE;
			
		break;
			
		case READ_VGUS_FRAME_DATA:
			memset(lcd_cmd_t.ucRevCmd, 0, ucRevCmdSize);
			for(uint16_t cnt=0; cnt <lcd_cmd_t.ucCmdSize; cnt++){
				lcd_cmd_t.ucRevCmd[cnt] = vgus_dev_t.pcRevBuff[vgus_dev_t.usReadPos];
				vgus_dev_t.pcRevBuff[vgus_dev_t.usReadPos] = 0;
				vgus_dev_t.usReadPos ++;
				vgus_dev_t.usReadPos &= USART_REV_MAX_BUF_SIZE;		/*	��֤�����	*/
			}
//			get_vgus_cmd_sta = READ_DC_FRAME_END;
			xQueueSend(qLcd_cmd_handle, &lcd_cmd_t, (TickType_t)10);
			get_vgus_cmd_sta = READ_VGUS_IDLE;
			break;
			
		case READ_DC_FRAME_END:
//			xQueueSend(qLcd_cmd_handle, &lcd_cmd_t, (TickType_t)10);
//			get_vgus_cmd_sta = READ_VGUS_IDLE;
		break;
		
		default: break;
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: Analysis_LcdCmd
* ����˵��: ����Lcdָ�ִ����ػص�����
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
typedef void (*DO_LCD_CMD)(LCD_CMD_T*);	
/*	ԭ������	*/
static void config_ctr_cmd80H(LCD_CMD_T* lcd_cmd_temp);
static void config_ctr_cmd81H(LCD_CMD_T* lcd_cmd_temp);
static void config_ctr_cmd83H(LCD_CMD_T* lcd_cmd_temp);
static void config_no_use_cmd(LCD_CMD_T* lcd_cmd_temp);
void Analysis_LcdCmd(void)
{
	BaseType_t result;
	LCD_CMD_T lcd_cmd_temp = {0};
	static DO_LCD_CMD do_lcd_cmd = NULL;
	/*	1. ָ�����		*/
	result = xQueueReceive(qLcd_cmd_handle,&lcd_cmd_temp, ( TickType_t )10);
	if(result == pdPASS){
		switch(lcd_cmd_temp.ucRevCmd[0]){
			case 0x80:	/*	�Ĵ��� д ָ�����ֵ8λ�������ֽ�	*/
				do_lcd_cmd = config_ctr_cmd80H;
				break;
			
			case 0x81:	/*	�Ĵ��� �� ָ�����ֵ8λ�������ֽ�	*/
				do_lcd_cmd = config_ctr_cmd81H;
				break;
			
			case 0x82:	/*	�����洢�Ĵ��� д ָ�����ֵ16λ����˫�ֽ�	*/
				do_lcd_cmd = config_ctr_cmd80H;
				break;
			
			case 0x83:	/*	�����洢�Ĵ��� �� ָ�����ֵ16λ����˫�ֽ�	*/
				do_lcd_cmd = config_ctr_cmd83H;
				break;
			
			case 0x85:	/*	��չָ��	*/
				
				break;
			
			default: 
				do_lcd_cmd = config_no_use_cmd;	
			break;
		}
		do_lcd_cmd(&lcd_cmd_temp);								/*	ִ�о������	*/
	}else{
		do_lcd_cmd = NULL;
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	����V1.0
* ��    �ڣ�2022-05-16
* �� �� ��: config_ctr_cmd83H
* ����˵��: �����пؼ������£�ִ����Ӧָ��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
extern SemaphoreHandle_t	sMotor_goTop;
extern SemaphoreHandle_t	sMotor_goOrigin;
static void config_ctr_cmd83H(LCD_CMD_T* lcd_cmd_temp)
{
	static uint8_t page_id = 0;		/*	ҳ��id	*/
	static uint8_t wedget_id = 0;	/*	�ؼ�id	*/
//	static uint8_t val_length = 0;	/*	���ݳ���	*/
	static uint16_t wedget_val = 0;	/*	�ؼ�ֵ	*/
	
	static uint8_t key_secret = 0;	/*	����	*/
	
	page_id = lcd_cmd_temp->ucRevCmd[1];
	wedget_id = lcd_cmd_temp->ucRevCmd[2];
//	val_length = lcd_cmd_temp->ucRevCmd[3];
	wedget_val = (lcd_cmd_temp->ucRevCmd[4] << 8) + lcd_cmd_temp->ucRevCmd[5];
	VGUS_LCD("Cmd size= %d, Page_id= %d, Wedget_id= %d, wedget_val= %d\r\n", lcd_cmd_temp->ucCmdSize, page_id, wedget_id, wedget_val);
	
	switch(page_id){
		case PAGE_PowerOn: 
			break;
		
		case PAGE_CheckHandle:
			switch(wedget_id){
				case 0x02: 
					key_secret = 0; 
					handle_in_top = false; xSemaphoreGive(sMotor_goTop); 
					set_now_page(PAGE_CheckOring); 
					break;
				case 0x03: 
					key_secret = (key_secret << 1); 
					key_secret += 1; 
					break;
				case 0x04: 
					key_secret = (key_secret << 1); 
					key_secret += 0; 
					break;
				default: break;
			}
			if(key_secret == 0xAC){
				set_now_page(PAGE_Calibration);
				key_secret = 0;
			}
			break;
		
		case PAGE_CheckOring:
			switch(wedget_id){
				case 0x02:
					handle_in_bottom = false; xSemaphoreGive(sMotor_goOrigin); 
					set_now_page(PAGE_Mode);
					break;
				
				default: break;
			}
			break;
		
		case PAGE_Mode:
			switch(wedget_id){
				case 0x03: 
				case 0x07: 
				case 0x08: 
				case 0x09: 
				case 0x0A:
					lcd_touch_t.page_id = PAGE_Mode;
					lcd_touch_t.touch_id = wedget_id;
					lcd_touch_t.touch_state = wedget_val;
					lcd_touch_t.touch_wait_read = 1;
					break;
				
				default: break;
			}
			break;
		
		case PAGE_Setting:
			switch(wedget_id){
				case 0x01:
				case 0x03:
				case 0x04:
				case 0x05:
				case 0x06:
				case 0x07:
				case 0x08:
				case 0x09:
					lcd_touch_t.page_id = PAGE_Setting;
					lcd_touch_t.touch_id = wedget_id;
					lcd_touch_t.touch_state = wedget_val;
					lcd_touch_t.touch_wait_read = 1;
					break;
				
				default: break;
			}
			break;
		
		case PAGE_Calibration:
			switch(wedget_id){
				case 0x01:
				case 0x02:
				case 0x03:
					lcd_touch_t.page_id = PAGE_Calibration;
					lcd_touch_t.touch_id = wedget_id;
					lcd_touch_t.touch_state = wedget_val;
					lcd_touch_t.touch_wait_read = 1;
					break;
				
				default: break;
			}
			break;
		
		case PAGE_Aging:
			switch(wedget_id){
				case 0x03:
				case 0x04:
					lcd_touch_t.page_id = PAGE_Aging;
					lcd_touch_t.touch_id = wedget_id;
					lcd_touch_t.touch_state = wedget_val;
					lcd_touch_t.touch_wait_read = 1;
					break;
				
				default: break;
			}
			break;
		
		case PAGE_PowerOff:
		
			break;
		
		default: break;
	}
}

static void config_ctr_cmd80H(LCD_CMD_T* lcd_cmd_temp)
{
	;
}

static void config_ctr_cmd81H(LCD_CMD_T* lcd_cmd_temp)
{
	now_display_page = lcd_cmd_temp->ucRevCmd[4];
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: config_no_use_cmd
* ����˵��: ִ�пɺ���ָ��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void config_no_use_cmd(LCD_CMD_T* lcd_cmd_temp)
{
	switch(lcd_cmd_temp->ucRevCmd[0]){
		
		default: break;
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: send_vgus_frame_head
* ����˵��: ��������֡ͷ
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void send_vgus_frame_head(void)
{
	HAL_UART_Transmit(&huart2, vgus_frame_head2, 2, 0x00FF);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: send_data_to_vgus_len
* ����˵��: ���͹̶�������������ʴ�����������������֡ͷ��֡β��
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
*********************************************************************************************************
*/
static void send_data_to_vgus_len(uint8_t* buf, uint8_t buf_len)
{
	HAL_UART_Transmit(&huart2, buf, buf_len, 0x00FF);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: send_data_to_vgus_len
* ����˵��: ���͹̶�������������ʴ���������������֡ͷ��֡β��
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
*********************************************************************************************************
*/
static void send_data_to_vgus_withFixLen(uint8_t* buf, uint8_t buf_len)
{
	send_vgus_frame_head();
	send_data_to_vgus_len(buf, buf_len);
}

// ��鷢�ͻ������Ƿ���ָ����Ҫ����
void check_sendBuf_and_sendBuf(void)
{
	BaseType_t result = xQueueReceive(qLcd_send_handle, &send_lcd_t, ( TickType_t )10);
	if(result == pdPASS){	/*	���ӳɹ�	*/
		send_data_to_vgus_withFixLen(send_lcd_t.ucSendCmd, send_lcd_t.ucSendSize);
		memset(&send_lcd_t, 0, sizeof(send_lcd_t));
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��    ����V1.0
* ��    �ڣ�2022-05-11
* �� �� ��: load_sendBuf_into_sendQueue
* ����˵��: ���͹̶��������������Դ�����
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
*********************************************************************************************************
*/
static void load_sendBuf_into_sendQueue(uint8_t* buf, uint8_t buf_len)
{
	static SEND_LCD_T send_data_to_lcd = {0};
	if(buf_len > ucSendCmdSize)
		return ;
	memcpy(send_data_to_lcd.ucSendCmd, buf, buf_len);
	send_data_to_lcd.ucSendSize = buf_len;
	xQueueSend(qLcd_send_handle, &send_data_to_lcd, (TickType_t)10);		/*	������ָ��ָ��洢��������	*/	
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��    ����V1.0
* ��    �ڣ�2022-05-19
* �� �� ��: 
* ����˵��: Mode, Setting, Calib, Aging��ʼ����ʾ
* ��    ��: 
*		
* �� �� ֵ: 
*********************************************************************************************************
*/
extern osThreadId FactoryCalibHandle;
extern osThreadId FactoryAgingHandle;
extern osThreadId ModeHandle;
extern osThreadId SettingHandle;
extern osThreadId MotorHandle;
static void initial_page_mode(void)
{
	show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x00);
}
static void initial_page_setting(void)
{
	show_variable_icon(PAGE_Setting, PAGE_CHECK_O_RING_ID, 0);
}
static void initial_page_calibration(void)
{
	show_variable_icon(PAGE_Calibration, PAGE_CALIB_PEDAL_ID, 0);
	show_variable_icon(PAGE_Calibration, PAGE_CALIB_FINISH_ID, 0);
}
static void initial_page_aging(void)
{
	show_variable_icon(PAGE_Aging, PAGE_AGING_START_ID, 0);
	show_variable_icon(PAGE_Aging, PAGE_AGING_FINISH_ID, 0);
}

/*------------------------------------���Դ�����API--------------------------------------------*/
/*	1. ���ñ������ȣ��Ĵ�����ַ0x01	*/
void set_now_brightness(uint8_t light_level)
{
	static uint8_t buf[4] = {0x03, 0x80, 0x01, 0x00};
	if(light_level > 0x40)
		light_level = 0x40;
	buf[3] = light_level;
	load_sendBuf_into_sendQueue(buf, 4);
}

/*	2. ��ȡ�������ȣ��Ĵ�����ַ0x01	*/
void get_now_brightness(void)
{
	static uint8_t buf[4] = {0x03, 0x81, 0x01, 0x01};
	load_sendBuf_into_sendQueue(buf, 4);
}

/*	3. ���õ�ǰ���棬�Ĵ�����ַ0x03	*/
void set_now_page(uint8_t page_id)
{
	static uint8_t last_page_id = 0;
	static uint8_t buf[5] = {0x04, 0x80, 0x03, 0x00, 0x00};
	buf[4] = page_id;
	load_sendBuf_into_sendQueue(buf, 5);
	now_display_page = page_id;
	
	if(last_page_id != page_id){	//	�ָ���һ������ϵͳ�����޹ؿؼ���״̬
		switch(last_page_id){
			case PAGE_Mode:
				initial_page_mode();
			break;
			
			case PAGE_Setting:
				initial_page_setting();
			break;
			
			case PAGE_Calibration:
				initial_page_calibration();
			break;
			
			case PAGE_Aging:
				initial_page_aging();
			break;
			
			default: break;
		}
	}
	last_page_id = page_id;
//	VGUS_LCD("page_id= %d, now_display_page = %d\r\n", page_id, now_display_page);
	
	switch(page_id){
		case PAGE_PowerOn:
			xSemaphoreGive(sPage_mode);
		break;
		
		case PAGE_CheckHandle:
			vTaskSuspend(ModeHandle);
			vTaskSuspend(SettingHandle);
			vTaskSuspend(FactoryCalibHandle);
			vTaskSuspend(FactoryAgingHandle);
		break;
		
		case PAGE_CheckOring:
			vTaskSuspend(ModeHandle);
			vTaskSuspend(SettingHandle);
			vTaskSuspend(FactoryCalibHandle);
			vTaskSuspend(FactoryAgingHandle);
		break;
		
		case PAGE_Mode:
			xSemaphoreGive(sPage_mode);
			vTaskResume(ModeHandle);			//	�ָ�Mode���̣��ر���������
			vTaskSuspend(SettingHandle);
			vTaskSuspend(FactoryCalibHandle);
			vTaskSuspend(FactoryAgingHandle);
		break;
		
		case PAGE_Setting:
			xSemaphoreGive(sPage_sett);		
			vTaskResume(SettingHandle);			//	�ָ�Setting���̣��ر���������	
			vTaskSuspend(ModeHandle);			
			vTaskSuspend(FactoryCalibHandle);
			vTaskSuspend(FactoryAgingHandle);
		break;
		
		case PAGE_Calibration:
			xSemaphoreGive(sPage_calib);
			vTaskResume(FactoryCalibHandle);	//	�ָ�Calibration���̣��ر���������
			vTaskSuspend(SettingHandle);
			vTaskSuspend(ModeHandle);
			vTaskSuspend(FactoryAgingHandle);
		break;
		
		case PAGE_Aging:
			xSemaphoreGive(sPage_aging);
			vTaskResume(FactoryAgingHandle);	//	�ָ�Aging���̣��ر���������
			vTaskSuspend(ModeHandle);
			vTaskSuspend(SettingHandle);
			vTaskSuspend(FactoryCalibHandle);
		break;
		
		default: break;
	}
}

/*	4. ��ȡ��ǰ���棬�Ĵ�����ַ0x03	*/
void get_now_page(void)
{
	static uint8_t buf[4] = {0x03, 0x81, 0x03, 0x02};
	load_sendBuf_into_sendQueue(buf, 4);
}

/*	5. ���ŵ�ǰ����	*/
void play_now_music(uint16_t music_num, uint8_t music_vol)
{
	static uint8_t buf[8] = {0x07, 0x80, 0x50, 0x5B, 0x00, 0x00, 0x5A, 0x00};
	buf[4] = (music_num >> 8);
	buf[5] = (music_num & 0x00FF);
	if(music_vol > 0x40)
		music_vol = 0x40;
	buf[7] = music_vol;
	load_sendBuf_into_sendQueue(buf, 8);
}

/*	6. ֹͣ��ǰ����	*/
void stop_now_music(uint16_t music_num)
{
	static uint8_t buf[6] = {0x05, 0x80, 0x50, 0x5C, 0x00, 0x00};
	buf[4] = (music_num >> 8);
	buf[5] = (music_num & 0x00FF);
	load_sendBuf_into_sendQueue(buf, 6);
}

/*	7. ���ŵ�ǰ��Ƶ	*/
void play_now_video(uint16_t video_num, uint8_t video_vol)
{
	/*					ע�ͣ�								  avi_type, XH,	  XL,	YH,	  YL,	video_num*/
	static uint8_t buf_avi_set[11] = {0x0A, 0x80, 0x60, 0x5A, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static uint8_t buf_vol[5] = {0x04, 0x80, 0x68, 0x5A, 0x00};
	
	buf_avi_set[9] = (video_num >> 8);
	buf_avi_set[10] =(video_num & 0x00FF);
	load_sendBuf_into_sendQueue(buf_avi_set, 11);
	
	if(video_vol > 0x3F)
		video_vol = 0x3F;
	buf_vol[4] = video_vol;
	load_sendBuf_into_sendQueue(buf_vol, 5);
}

/*	8. ֹͣ��ǰ��Ƶ	*/
void stop_now_video(void)
{
	static uint8_t buf[4] = {0x03, 0x80, 0x6B, 0x5A};
	load_sendBuf_into_sendQueue(buf, 4);
}

/*	9. ��Ƶ�ؼ�����	*/
//	addr: ��Ƶ�ؼ��ĵ�ַ
//	val: 1 ��ʼ���ţ�0 ֹͣ����
void set_wedget_video(uint16_t addr, uint16_t val)
{
	static uint8_t buf[6] = {0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
	buf[2] = (addr >> 8);
	buf[3] =  (addr &0x00FF);
	
	buf[4] = (val >> 8);
	buf[5] =  (val &0x00FF);
	load_sendBuf_into_sendQueue(buf, 6);
}

/*	10. ����ͨ��������������	*/
void add_single_line_data(uint8_t ch, uint16_t ch_data)
{
	static uint8_t buf[5] = {0x04, 0x84, 0x00, 0x00, 0x00};
	if(ch > 7)
		ch = 0;
	buf[2] = (0x01 << ch);
	buf[3] = (ch_data >> 8);
	buf[4] = (ch_data &0x00FF);
	load_sendBuf_into_sendQueue(buf, 5);
}

/*	11. ��˫ͨ��������������	*/
void add_two_line_data(uint8_t ch1, uint16_t ch1_data, uint8_t ch2, uint16_t ch2_data)
{
	static uint8_t buf[7] = {0x06, 0x84, 0x00, 0x00, 0x00,0x00, 0x00};
	buf[2] = (0x01 << ch1) + (0x01 << ch2);
	buf[3] = (ch1_data >> 8);
	buf[4] = (ch1_data &0x00FF);
	buf[5] = (ch2_data >> 8);
	buf[6] = (ch2_data &0x00FF);
	load_sendBuf_into_sendQueue(buf, 7);
}

/*	12. ��ʾ4�ֽ��޷������α���	*/
void show_uint32_t_data(uint16_t addr, uint32_t n_cycle)
{
	static uint8_t buf[8] = {0x07, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	buf[2] = (addr >> 8);
	buf[3] = (addr & 0x00FF);
	buf[4] = (n_cycle >> 24);
	buf[5] = ((n_cycle&0x00FF0000)>>16);
	buf[6] = ((n_cycle&0x0000FF00)>>8);
	buf[7] = (n_cycle&0x000000FF);
	load_sendBuf_into_sendQueue(buf, 8);
}

void show_uint16_t_data(uint16_t addr, uint16_t n_cycle)
{
	static uint8_t buf[6] = {0x07, 0x82, 0x00, 0x00, 0x00, 0x00};
	buf[2] = (addr >> 8);
	buf[3] = (addr & 0x00FF);
	buf[4] = (n_cycle >> 8);
	buf[5] = (n_cycle&0x00FF);
	load_sendBuf_into_sendQueue(buf, 6);
}

/*	13. ��ʾ����ͼ��,ʵ���ϱ�����ַ = 	(page << 8 + id)	*/
void show_variable_icon(uint8_t page, uint8_t id, uint16_t n_frame)
{
	static uint8_t buf[6] = {0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
	buf[2] = page;
	buf[3] =  id;
	
	buf[4] = (n_frame >> 8);
	buf[5] =  (n_frame &0x00FF);
	load_sendBuf_into_sendQueue(buf, 6);
}

/*	14. ʹ�ܴ���	*/
void enable_vgus_touch(void)
{
	static uint8_t buf[4] = {0x03, 0x80, 0x0B, 0xFF};
	load_sendBuf_into_sendQueue(buf, 4);
}

/*	15. �رմ���	*/
void disable_vgus_touch(void)
{
	static uint8_t buf[4] = {0x03, 0x80, 0x0B, 0x00};
	load_sendBuf_into_sendQueue(buf, 4);
}

/*	16. ������Ƶ����	*/
void set_vgus_music_volume(uint8_t volume)
{
	static uint8_t buf[5] = {0x04, 0x80, 0x53, 0x5A, 0x40};
	buf[4] = volume;
	load_sendBuf_into_sendQueue(buf, 5);
}

/*	17. ������Ƶ����	*/
void set_vgus_video_volume(uint8_t volume)
{
	static uint8_t buf[5] = {0x04, 0x80, 0x68, 0x5A, 0x3F};
	buf[4] = volume;
	load_sendBuf_into_sendQueue(buf, 5);
}

/*	����������	*/
void vgus_lcd_reboot(void)
{
	set_vgus_music_volume(system_volume[get_system_vol()]);
	set_system_light(get_system_light());
	
	set_now_page(PAGE_PowerOn);
	set_wedget_video(1, 0x0001);
	
	show_variable_icon(PAGE_Setting, PAGE_SETT_VOL_ID, get_system_vol());
	
	show_variable_icon(PAGE_Setting, PAGE_SETT_LIGHT_ID, get_system_light());
	
	if(is_asp_open() == true)
		show_variable_icon(PAGE_Mode, PAGE_MODE_ASP_ID, 1);
	else
		show_variable_icon(PAGE_Mode, PAGE_MODE_ASP_ID, 0);	
	if(is_smart_reminder_open() == true)
		show_variable_icon(PAGE_Setting, PAGE_SETT_SMART_REMD, 1);
	else 
		show_variable_icon(PAGE_Setting, PAGE_SETT_SMART_REMD, 0);
	
	if(is_train_mode_open() == true)
		show_variable_icon(PAGE_Setting, PAGE_SETT_TRAIN_MODE, 1);
	else 
		show_variable_icon(PAGE_Setting, PAGE_SETT_TRAIN_MODE, 0);
}
