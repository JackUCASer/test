#include "lcd_common.h"

extern uint16_t vgus_frame_head;
/*	���ڽ��ջ�������ʼ��	*/
uint16_t* data_frame_start = &vgus_frame_head;
uint8_t  ucUsartRevBuff[USART_REV_MAX_BUF_SIZE+1] = {0};
uint16_t usUsartRevCurrentPos;		/*	���ڼ�¼��ǰ���ڽ������ݷ��ڽ��ջ�����������	*/

uint8_t	ucUsartSendBuff[USART_SEND_MAX_BUF_SIZE] = {0};
uint16_t ucUsartSendSize= 0;
uint16_t ucUsartSendCnt = 0;
