//#include "lcd_common.h"
#include "vgus_lcd_if.h"
#include "app_main.h"

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
LCD_CMD_T lcd_cmd_t = {0};
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
	.pcRevBuff 	= NULL,
	.usReadPos 	= 0,
	.psRevPos	= NULL,		
	.usMaxPos	= 0,		
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
	
	// 3. ʹ�ܴ��ڽ���
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);				//	ʹ�ܴ���idle�ж�
	HAL_UART_Receive_DMA(&huart2, RX_BUF, RX_BUFFER_SIZE);		//	ʹ��DMA����
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
				do_lcd_cmd = config_no_use_cmd;
				break;
			
			default: 
				do_lcd_cmd = config_no_use_cmd;	
				break;
		}
		do_lcd_cmd(&lcd_cmd_temp);	/*	ִ�о������	*/
	}else{
		return ;
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
					handle_in_top = false; 
					xSemaphoreGive(sMotor_goTop); 
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
					handle_in_bottom = false; 
					xSemaphoreGive(sMotor_goOrigin); 
					set_now_page(PAGE_Mode);
					break;
				
				default: break;
			}
			break;
		
		case PAGE_Mode:
			switch(wedget_id){
				case 0x01:
				case 0x02: 
				case 0x03: 
				case 0x04: 
				case 0x05: 
				case 0x06:
				case 0x07:
				case 0x08:
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
				case 0x0A:
				case 0x0B:
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
	return ;
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
	return ;
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-08-11
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
	HAL_UART_Transmit_DMA(&huart2, buf, buf_len);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-08-11
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
	static uint8_t buf2[30] = {0};
	buf2[0] = 0xa5;
	buf2[1] = 0x5a;
	memcpy(&buf2[2], buf, buf_len);
	send_data_to_vgus_len(buf2, buf_len+2);
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
static void initial_page_mode(void)
{
	show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0x00);
	show_variable_icon(PAGE_Mode, PAGE_MODE_ADJECT_DOSE_ID, 0);
}
static void initial_page_setting(void)
{
	show_variable_icon(PAGE_Setting, PAGE_CHECK_O_RING_ID, 0);
//	if(is_autoback_off()== true)
//		show_variable_icon(0x00, PAGE_AUTOBACK_ID, 1);
//	else
//		show_variable_icon(0x00, PAGE_AUTOBACK_ID, 0);
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

extern osThreadId FactoryCalibHandle;
extern osThreadId FactoryAgingHandle;
extern osThreadId ModeHandle;
extern osThreadId SettingHandle;
extern osThreadId MotorHandle;
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
	static uint8_t buf[6] = {0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
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

/*	13.1 multi_write 82H command,�������¶��	*/
void write_two_82H_cmd(uint16_t start_addr, uint16_t var1, uint16_t var2)
{
	static uint8_t buf[8] = {0x07, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	buf[2] = (start_addr >> 8);
	buf[3] =  (start_addr &0x00FF);
	
	buf[4] = (var1 >> 8);
	buf[5] =  (var1 &0x00FF);
	
	buf[6] = (var2 >> 8);
	buf[7] =  (var2 &0x00FF);
	load_sendBuf_into_sendQueue(buf, 8);
}

void write_three_82H_cmd(uint16_t start_addr, uint16_t var1, uint16_t var2, uint16_t var3)
{
	static uint8_t buf[10] = {0x09, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	buf[2] = (start_addr >> 8);
	buf[3] =  (start_addr &0x00FF);
	
	buf[4] = (var1 >> 8);
	buf[5] =  (var1 &0x00FF);
	
	buf[6] = (var2 >> 8);
	buf[7] =  (var2 &0x00FF);
	
	buf[8] = (var3 >> 8);
	buf[9] =  (var3 &0x00FF);
	load_sendBuf_into_sendQueue(buf, 10);
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
//	play_now_video(0x0001, 0x1F);
	
	//	MODE����
	if(is_asp_open() == true)
		show_variable_icon(PAGE_Mode, PAGE_MODE_ASP_ID, 1);
	else
		show_variable_icon(PAGE_Mode, PAGE_MODE_ASP_ID, 0);	
	show_variable_icon(PAGE_Mode, PAGE_MODE_FUNC_ID, 0);
	
	//	Setting����
	show_variable_icon(PAGE_Setting, PAGE_SETT_VOL_ID, get_system_vol());
	show_variable_icon(PAGE_Setting, PAGE_SETT_LIGHT_ID, get_system_light());
	if(is_smart_reminder_open() == true)
		show_variable_icon(PAGE_Setting, PAGE_SETT_SMART_REMD, 1);
	else 
		show_variable_icon(PAGE_Setting, PAGE_SETT_SMART_REMD, 0);
	if(is_train_mode_open() == true)
		show_variable_icon(PAGE_Setting, PAGE_SETT_TRAIN_MODE, 1);
	else 
		show_variable_icon(PAGE_Setting, PAGE_SETT_TRAIN_MODE, 0);
}