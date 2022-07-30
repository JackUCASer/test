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

/*	����һ���豸������ȡ״̬	*/
typedef enum{
	READ_DC_IDLE = 0,
	READ_DC_FRAME_HEAD,
	READ_DC_FRAME_DATA,
	READ_DC_FRAME_END
}GET_DC_CMD_STA;

/*	����һ��ָ��ṹ	*/
#define ucRevCmdSize	20		/*	��������ָ����󳤶�	*/
#define ucSendCmdSize	30		/*	��������ָ����󳤶�	*/
//	ָ����սṹ��
typedef struct{
	uint8_t ucCmdSize;
	uint8_t ucRevCmd[ucRevCmdSize];
}LCD_CMD_T;
//	ָ��ͽṹ��
typedef struct{
	uint8_t ucSendSize;
	uint8_t ucSendCmd[ucSendCmdSize];
}SEND_LCD_T;

/*-------------------------------------------------------------------------------*/
/*	����ԭ������	*/
static void dc_dev_init(void);


/*	��ʼ���豸	*/
DC_DEV_T dc_dev_t ={
	.pcRevBuff 	= &ucUsartRevBuff[0],			/*	���ݽ��ջ������׵�ַ	*/
	.usReadPos 	= 0,							/*	��ǰ��ȡ���ݵ�ƫ������	*/
	.psRevPos	= &usUsartRevCurrentPos,		/*	��ǰ���ջ�����������ƫ�Ƶ�ַ	*/
	.usMaxPos	= USART_REV_MAX_BUF_SIZE,		/*	���ɶ�ȡ������	*/
	.dev_init	= dc_dev_init					/*	lcd��ʼ������	*/
};

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


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-13
* �� �� ��: dc_dev_init
* ����˵��: �豸��ʼ��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void dc_dev_init(void)
{
	// 1. ��������ʼ��
	qLcd_cmd_handle = xQueueCreate(10, sizeof(lcd_cmd_t));		/*	��ʼ��10���洢 LCD_CMD_T ����ָ���������Ϣ����	*/
	qLcd_send_handle = xQueueCreate(10, sizeof(send_lcd_t));	/*	��ʼ��10���洢 SEND_LCD_T ����ָ���������Ϣ����	*/
	
	// 2. ����ź���ʼ��
	sPage_mode = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_mode	*/
	sPage_sett = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_sett	*/
	sPage_calib = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_calib	*/
	sPage_aging = xSemaphoreCreateBinary();						/*	��ʼ����ֵ�ź��� sPage_aging	*/
	
	// 3. �����ʼ��
	
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-13
* �� �� ��: Extract_LcdCmd
* ����˵��: ��ȡLcd ָ��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/

void Extract_LcdCmd(void)
{
	static GET_DC_CMD_STA  get_dc_cmd_sta = READ_DC_IDLE;
	static uint8_t* pcRevCmd = lcd_cmd_t.ucRevCmd;
	static uint32_t uiRevEnd = 0;
	
	switch(get_dc_cmd_sta){
		case READ_DC_IDLE:
			if(dc_dev_t.usReadPos == *(dc_dev_t.psRevPos))					/*	��ǰ��ȡ���������Ƿ���ڽ��ջ���������������	*/
				break;
			else{
				if(dc_dev_t.pcRevBuff[dc_dev_t.usReadPos] == dc_frame_head)	/*	��ǰ�����Ƿ�Ϊ֡ͷ	*/
					get_dc_cmd_sta = READ_DC_FRAME_HEAD;
				else
					get_dc_cmd_sta = READ_DC_IDLE;
				
				if(dc_dev_t.usReadPos == USART_REV_MAX_BUF_SIZE)			/*	��ǰ�����Ƿ��ǻ������������	*/
					dc_dev_t.usReadPos = 0;		/*	ƫ�Ƶ�ַ����	*/
				else				
					dc_dev_t.usReadPos ++;		/*	ƫ�Ƶ�ַ+1	*/
			}	
		break;
		
		case READ_DC_FRAME_HEAD:
			get_dc_cmd_sta = READ_DC_FRAME_DATA;
			memset(lcd_cmd_t.ucRevCmd, 0, ucRevCmdSize);					/*	��ջ�������׼��������ָ��	*/
			lcd_cmd_t.ucCmdSize = 0;
			pcRevCmd = lcd_cmd_t.ucRevCmd;									/*	ָ��洢��������ʼ��ַ	*/
			
		break;
		
		case READ_DC_FRAME_DATA:
			while(1){
				*pcRevCmd =  dc_dev_t.pcRevBuff[dc_dev_t.usReadPos];
				uiRevEnd  = (uiRevEnd << 8) + *pcRevCmd;					/*	��ȡ����֡β		*/
				lcd_cmd_t.ucCmdSize ++;										/*	ָ��ȼ�����	*/
			
				if(dc_dev_t.usReadPos >= USART_REV_MAX_BUF_SIZE)			/*	��ǰ�����Ƿ��ǻ������������	*/
					dc_dev_t.usReadPos = 0;									/*	ƫ�Ƶ�ַ����		*/
				else				
					dc_dev_t.usReadPos ++;									/*	ƫ�Ƶ�ַ+1	*/
				
				if(pcRevCmd == &(lcd_cmd_t.ucRevCmd[ucRevCmdSize-1])){		/*	lcd_cmd_t.ucRevCmd�������	*/
					pcRevCmd = pcRevCmd;
					myPrintf("Log warning %d: Current lcd_cmd_t.ucRevCmd overflow!\r\n", 1);	/*	��ӡ��־	*/
				}else
					pcRevCmd ++;
			
				if(uiRevEnd == dc_frame_end){
					get_dc_cmd_sta = READ_DC_FRAME_END;						/*	�������		*/
					lcd_cmd_t.ucCmdSize -=4;	//	ȥ��������
					break;
				}else
					get_dc_cmd_sta = READ_DC_FRAME_DATA;					/*	��������		*/
			}
		break;
		
		case READ_DC_FRAME_END:
			xQueueSend(qLcd_cmd_handle, &lcd_cmd_t, (TickType_t)10);		/*	������ָ��洢��������	*/		
			get_dc_cmd_sta = READ_DC_IDLE;		/*	�������	*/
		break;
	}
		
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2021-04-21
* �� �� ��: Analysis_LcdCmd
* ����˵��: ����Lcdָ�ִ����ػص�����
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
typedef void (*DO_LCD_CMD)(LCD_CMD_T*);					/*	����һ��ָ���������ΪLCD_CMD_T*���ͣ�����ֵΪvoid �ĺ���ָ������	*/
/*	ԭ������	*/
static void config_ctr_cmd(LCD_CMD_T* lcd_cmd_temp);	/*	��ָ̬���ԭ������		*/
static void config_no_use_cmd(LCD_CMD_T* lcd_cmd_temp);	/*	�ɺ���ָ���ԭ������	*/
void Analysis_LcdCmd(void)
{
	BaseType_t result;
	LCD_CMD_T lcd_cmd_temp = {0};
	static DO_LCD_CMD do_lcd_cmd = NULL;
	/*	1. ָ�����		*/
	result = xQueueReceive(qLcd_cmd_handle,&lcd_cmd_temp, ( TickType_t )10);
	if(result == pdPASS){
		switch(lcd_cmd_temp.ucRevCmd[0]){
			case 0xB1:	/*	��ָ̬��		*/
				do_lcd_cmd = config_ctr_cmd;	/*	ִ����ָ̬��	*/
				break;
			
			/*-------------------------����ͼ��ָ�---------------------------*/
			case 0x01:											/*	����ָ��		*/
			case 0x05:											/*	ͼ��ָ��	*/
			case 0x46:	case 0x47:	case 0x34:					/*	��ͼָ��		*/
			case 0x43:	case 0x45:	case 0x20:	case 0x21:		/*	����ָ��		*/
			case 0x44:	case 0x32:	case 0x80:	case 0x33:		/*	ͼƬ��ʾ		*/
			case 0x51:	case 0x54:	case 0x52:					/*	��ͼ��1	*/
			case 0x58:	case 0x67:								/*	��ͼ��2	*/
			case 0x59:	case 0x76:	case 0x69:					/*	����	*/
			// TO DO 
			do_lcd_cmd = config_no_use_cmd;
			break;
			
			/*---------------------------����ָ�-----------------------------*/
			case 0xB3:											/*	��/ʹ����Ļ����	*/
			case 0x87:	case 0x88:								/*	�洢����д	*/
			case 0x77:	case 0x60:								/*	�Զ��������	*/
			// TO DO 
			do_lcd_cmd = config_no_use_cmd;
			break;
			
			case 0x18:				/*	18������ϵͳ����	*/
			do_lcd_cmd = config_no_use_cmd;
			break;
			
			case 0x17:				/*	17������ϵͳ����	*/
			do_lcd_cmd = config_no_use_cmd;	
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

LCD_PAGE_T	lcd_page_t = {0};				/*	0x01 �����л�	*/
LCD_TOUCH_T lcd_touch_t = {0};				/*	0x10 ����	*/	
LCD_PROGRESS_T lcd_progress_t = {0};		/*	0x12 ������	*/
LCD_SLIDER_T lcd_slider_t = {0};			/*	0x13 ������	*/
LCD_VIDEO_T lcd_video_t = {0};				/*	0x75 ��Ƶ���Ž���	*/
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-03-06
* �� �� ��: config_ctr_cmd
* ����˵��: ������ָ̬��
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
#include "lcd_display.h"
#include "alarm_module.h"
uint16_t usCurrent_page_id = 0;		/*	��ǰ���ڽ���	*/
uint16_t usShould_page_id = 1;		/*	�������ڽ���	*/
static void config_ctr_cmd(LCD_CMD_T* lcd_cmd_temp)
{
	static uint16_t usCtr_id = 0;
	static uint8_t ucCtr_type = 0;
	static bool first_power_on = true;
	switch(lcd_cmd_temp->ucRevCmd[1]){
		case 0x01:	/*	�л������֪ͨ	*/
			usCurrent_page_id = (lcd_cmd_temp->ucRevCmd[2]<<8) + lcd_cmd_temp->ucRevCmd[3];
			lcd_page_t.page_id = usCurrent_page_id;
			lcd_page_t.page_wait_read = 1;
			if(first_power_on)
				first_power_on = false;
			else
				page_track(usShould_page_id,usCurrent_page_id);
			myPrintf("Log Debug: Current we are in page %d\r\n", usCurrent_page_id);
		break;
			
		case 0x75:	/*	��Ƶ���Ž���֪ͨ	*/
			usCurrent_page_id = (lcd_cmd_temp->ucRevCmd[2]<<8) + lcd_cmd_temp->ucRevCmd[3];
			lcd_video_t.page_id = usCurrent_page_id;
			lcd_video_t.video_id = (lcd_cmd_temp->ucRevCmd[4]<<8) + lcd_cmd_temp->ucRevCmd[5];
			lcd_video_t.video_value = lcd_cmd_temp->ucRevCmd[6];
			lcd_video_t.video_wait_read = 1;
		break;
		
		case 0x11:	/*	���ؿؼ���ֵ	*/
			usCurrent_page_id = (lcd_cmd_temp->ucRevCmd[2]<<8) + lcd_cmd_temp->ucRevCmd[3];
			usCtr_id = (lcd_cmd_temp->ucRevCmd[4]<<8) + lcd_cmd_temp->ucRevCmd[5];
			ucCtr_type = lcd_cmd_temp->ucRevCmd[6];
			switch(ucCtr_type){
				case 0x10: //	��ǰΪ��ť������ȡ�����ݴ洢��lcd_touch_t��
					lcd_touch_t.page_id = usCurrent_page_id;
					lcd_touch_t.touch_id = usCtr_id;
					lcd_touch_t.touch_state = lcd_cmd_temp->ucRevCmd[8];
					lcd_touch_t.touch_wait_read = 1;		//	��־λ���ȴ����Ӧ�ö�ȡ
					myPrintf("Log Debug: page %d bottom %d is pressed and its state is %d!\r\n", lcd_touch_t.page_id, lcd_touch_t.touch_id,lcd_touch_t.touch_state);
					break;
				
				case 0x12: //	��ǰΪ������������ȡ�����ݴ洢��lcd_progress_t��
					lcd_progress_t.page_id = usCurrent_page_id;
					lcd_progress_t.progress_id = usCtr_id;
					lcd_progress_t.progress_value = (lcd_cmd_temp->ucRevCmd[7]<<24) + (lcd_cmd_temp->ucRevCmd[8]<<16) \
												  + (lcd_cmd_temp->ucRevCmd[9]<<8) + lcd_cmd_temp->ucRevCmd[10];
					lcd_progress_t.progress_wait_read = 1;	//	��־λ���ȴ����Ӧ�ö�ȡ
					myPrintf("Log Debug: page %d progress %d is pressed and its value is %d!\r\n", lcd_progress_t.page_id, lcd_progress_t.progress_id,lcd_progress_t.progress_value);
					break;
				
				case 0x13: //	��ǰΪ������������ȡ�����ݴ洢��lcd_slider_t��
					lcd_slider_t.page_id = usCurrent_page_id;
					lcd_slider_t.slider_id = usCtr_id;
					lcd_slider_t.slider_value = (lcd_cmd_temp->ucRevCmd[7]<<24) + (lcd_cmd_temp->ucRevCmd[8]<<16) \
											  + (lcd_cmd_temp->ucRevCmd[9]<<8) + lcd_cmd_temp->ucRevCmd[10];
					lcd_slider_t.slider_wait_read = 1;		//	��־λ���ȴ����Ӧ�ö�ȡ
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-13
* �� �� ��: config_no_use_cmd
* ����˵��: ִ�пɺ���ָ��
* ��    ��: 
      
* �� �� ֵ: 
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.1
* ��    �ڣ�2022-03-16
* �� �� ��: page_others
* ����˵��: ��������
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
static void run_check_handle(void);
static void run_check_o_ring(void);
static void run_check_idle(void);
typedef void (*DO_OTHER_PAGE)(void);	/*	����һ��ָ���������ΪLCD_CMD_T*���ͣ�����ֵΪvoid �ĺ���ָ������	*/
DO_OTHER_PAGE do_other_page = run_check_idle;
extern osThreadId FactoryCalibHandle;
extern osThreadId FactoryAgingHandle;
extern osThreadId ModeHandle;
extern osThreadId SettingHandle;
void page_others(void)
{
	if(lcd_page_t.page_wait_read == 1){
		lcd_page_t.page_wait_read = 0;		/*	�����л�ָ���ѱ���ȡ	*/
		switch(lcd_page_t.page_id){
			case PowerOn:			//	��ǰ���ڿ�������
				display_video_start();
				do_other_page = run_check_idle;
			break;
		
			case MODE:			//	��ǰ����Mode����
				xSemaphoreGive(sPage_mode);
				vTaskResume(ModeHandle);			//	�ָ�Mode���̣��ر���������
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_idle;
			break;
			
			case SETTING:		//	��ǰ����SETTING����
				xSemaphoreGive(sPage_sett);		
				vTaskResume(SettingHandle);			//	�ָ�Setting���̣��ر���������					
				vTaskSuspend(ModeHandle);			
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_idle;
			break;
			
			case CALIBRATION:	//	��ǰ����CALIBRATION����
				xSemaphoreGive(sPage_calib);
				vTaskResume(FactoryCalibHandle);	//	�ָ�Calibration���̣��ر���������
				vTaskSuspend(SettingHandle);
				vTaskSuspend(ModeHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_idle;
			break;
			
			case AGING:			//	��ǰ����AGING����
				xSemaphoreGive(sPage_aging);
				vTaskResume(FactoryAgingHandle);	//	�ָ�Aging���̣��ر���������
				vTaskSuspend(ModeHandle);
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				do_other_page = run_check_idle;
			break;
			
			case CheckHandle:	//	��ǰ����ȷ���ֱ�����
				vTaskSuspend(ModeHandle);
				vTaskSuspend(SettingHandle);
				vTaskSuspend(FactoryCalibHandle);
				vTaskSuspend(FactoryAgingHandle);
				do_other_page = run_check_handle;
				break;
			
			case CheckORing:		//	��ǰ����ȷ�ϡ�O����Ȧ����
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-04-24
* �� �� ��: run_check_handle
* ����˵��: �����û�ȷ���ֱ�δ��װ��Ͳ��ҩƿ
* ��    ��: 
      
* �� �� ֵ: 
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
			go_to_page(CALIBRATION);			/*	����У��ģʽ	*/
			key_secret = 0;
		break;
	
		default: break;
	}

}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-04-24
* �� �� ��: run_check_o_ring
* ����˵��: �����û�ȷ���ֱ�δ��װ��Ͳ��ҩƿ
* ��    ��: 
      
* �� �� ֵ: 
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

	if(handle_in_top == false){										/*	�ȴ�˿�˵��ﶥ��	*/
		if(first_this_func == true){
			buf[6] = 0x00;
			load_sendBuf_into_sendQueue(buf, 7);					/*	ʧ�ܿؼ�PAGE_CHECKORING_OK	*/
			first_this_func = false;
		}
	}else{
		if(first_enable_check_o_ring_key == true){
			buf[6] = 0x01;
			load_sendBuf_into_sendQueue(buf, 7);					/*	ʹ�ܿؼ�PAGE_CHECKORING_OK	*/
			first_enable_check_o_ring_key = false;
		}
	}
	
	if((lcd_touch_t.page_id == CheckORing)&&(lcd_touch_t.touch_id == PAGE_CHECKORING_OK)&&(lcd_touch_t.touch_wait_read == 1)){
		lcd_touch_t.touch_wait_read = 0;
		buf[6] = 0x00;
		load_sendBuf_into_sendQueue(buf, 7);						/*	ʧ�ܿؼ�PAGE_CHECKORING_OK	*/
		start_check_bottom = true;
		handle_in_bottom = false;
		xSemaphoreGive(sMotor_goBottom);
									
	}
	if((start_check_bottom == true)&&(handle_in_bottom == true)){	/*	�ȴ�˿�˵���ײ�	*/
		go_to_page(MODE);
		start_check_bottom = false;
	}else
		return ;
}

static void run_check_idle(void)
{
	uint32_t use_times = 0;
	/*	�ж��Ƿ���Ƶ���Ž���	*/
	if((lcd_video_t.video_wait_read ==1)&&(lcd_video_t.page_id == PowerOn)&&(lcd_video_t.video_id == PAGE_POWERON_VIDEO_ID)){
		lcd_video_t.video_wait_read = 0;
		use_times = read_use_times();
		if(use_times % N_TIME_CHECK_ORING != 0){	/*	����O��Ȧ��飬ֱ�ӽ���Mode����	*/
			go_to_page(MODE);
		}else{										/*	����O��Ȧ������	*/
			go_to_page(CheckHandle);
		}
	}else
		return ;
}
#include "usart.h"
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-17
* �� �� ��: send_dc_frame_head
* ����˵��: ��������֡ͷ
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void send_dc_frame_head(void)
{
	HAL_UART_Transmit(&huart2, &dc_frame_head, 1, 0x00FF);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-17
* �� �� ��: send_dc_frame_end
* ����˵��: ��������֡β
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void send_dc_frame_end(void)
{
	HAL_UART_Transmit(&huart2, dc_frame_end2, 4, 0x00FF);
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-17
* �� �� ��: send_data_to_dc_len
* ����˵��: ���͹̶�������������ʴ�����������������֡ͷ��֡β��
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
*********************************************************************************************************
*/
void send_data_to_dc_len(uint8_t* buf, uint8_t buf_len)
{
	HAL_UART_Transmit(&huart2, buf, buf_len, 0x00FF);
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-17
* �� �� ��: send_data_to_dc_len
* ����˵��: ���ͱ䳤��������ʴ�����������������֡ͷ��֡β��
* ��    ��: 
*			buf�� ���������ݵ�ַ
* �� �� ֵ: 
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-12-17
* �� �� ��: send_data_to_dc_len
* ����˵��: ���͹̶�������������ʴ���������������֡ͷ��֡β��
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
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
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��    ����V1.0
* ��    �ڣ�2020-04-20
* �� �� ��: send_data_to_dc_len
* ����˵��: ���͹̶�������������ʴ���������������֡ͷ��֡β��
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
*********************************************************************************************************
*/
void load_sendBuf_into_sendQueue(uint8_t* buf, uint8_t buf_len)
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
* ��	  ����V1.0
* ��    �ڣ�2022-04-14
* �� �� ��: send_data_to_dc_IT
* ����˵��: ���͹̶�������������ʴ���������������֡ͷ��֡β,�жϷ�ʽ����������ʹ��;
			1. �жϻ�Ӱ�����ϵͳ���ȶ���
			2. ���������Ҫ100us�жϣ������ȼ���ߣ���ʹ�ô����жϣ�Ƶ��ͨ�Ų����ķ����жϣ����ϵͳ����ѹ����
* ��    ��: 
*			buf�� ���������ݵ�ַ
*			buf_len�� ���������ݳ���
* �� �� ֵ: 
*********************************************************************************************************
*/
void send_data_to_dc_IT(uint8_t *buf, uint8_t buf_len)
{
	/*	װ�����������ͻ�����	*/
	ucUsartSendBuff[0] = dc_frame_head;
	ucUsartSendSize = 1;
	memcpy(&ucUsartSendBuff[1], buf, buf_len);
	ucUsartSendSize += buf_len;
	memcpy(&ucUsartSendBuff[ucUsartSendSize], dc_frame_end2, 4);
	ucUsartSendSize += 4;
	ucUsartSendCnt = 0;		/*	���ͼ���������	*/
	
	/*	�������ڷ���Ϊ���ж�	*/
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
}

// ��鷢�ͻ������Ƿ���ָ����Ҫ����
void check_sendBuf_and_sendIt(void)
{
	BaseType_t result = xQueueReceive(qLcd_send_handle, &send_lcd_t, ( TickType_t )10);
	if(result == pdPASS){	/*	���ӳɹ�	*/
		send_data_to_dc_withFixLen(send_lcd_t.ucSendCmd, send_lcd_t.ucSendSize);
		memset(&send_lcd_t, 0, sizeof(send_lcd_t));
	}
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-11-09
* �� �� ��: my_itoa
* ����˵��: ������ת��Ϊ�ַ���(10����)��
* ��    ��: 
		num:	��ת�����ַ���������
		str:	����ת������ַ����ĵ�ַ
* �� �� ֵ: 
*********************************************************************************************************
*/
char* my_itoa(int32_t num, char* str)
{
	char index[]="0123456789";	//	������
    uint32_t unum;				//	���Ҫת���������ľ���ֵ,ת�������������Ǹ���
    int i=0,j,k;				//	i����ָʾ�����ַ�����Ӧλ��ת��֮��i��ʵ�����ַ����ĳ��ȣ�
								//	ת����˳��������ģ��������������k����ָʾ����˳��Ŀ�ʼλ��;j����ָʾ����˳��ʱ�Ľ�����
 
    //	��ȡҪת���������ľ���ֵ
    if(num<0)					//	Ҫת����ʮ�����������Ǹ���
    {
        unum= -num;				//	��num�ľ���ֵ����unum
        str[i++]='-';			//	���ַ�����ǰ������Ϊ'-'�ţ�����������1
    }
    else unum = num;			//	����numΪ����ֱ�Ӹ�ֵ��unum
 
    //	ת�����֣�ע��ת�����������
    do
    {
        str[i++] =index[unum%10];//	ȡunum�����һλ��������Ϊstr��Ӧλ��ָʾ������1
        unum /=10;				//	unumȥ�����һλ
 
    }while(unum);				//	ֱ��unumΪ0�˳�ѭ��
 
    str[i]='\0';				//	���ַ���������'\0'�ַ���c�����ַ�����'\0'������
 
    //	��˳���������
    if(str[0]=='-') k=1;		//	����Ǹ��������Ų��õ������ӷ��ź��濪ʼ����
    else k=0;					//	���Ǹ�����ȫ����Ҫ����
 
    char temp;					//	��ʱ��������������ֵʱ�õ�
    for(j=k;j<=(i-1)/2;j++)		//	ͷβһһ�Գƽ�����i��ʵ�����ַ����ĳ��ȣ��������ֵ�ȳ�����1
    {
        temp=str[j];			//	ͷ����ֵ����ʱ����
        str[j]=str[i-1+k-j];	//	β����ֵ��ͷ��
        str[i-1+k-j]=temp;		//	����ʱ������ֵ(��ʵ����֮ǰ��ͷ��ֵ)����β��
    }
    return str;					//	����ת������ַ���
}
