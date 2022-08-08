#include "lcd_common.h"

extern uint16_t vgus_frame_head;
/*	串口接收缓冲区初始化	*/
uint16_t* data_frame_start = &vgus_frame_head;
uint8_t  ucUsartRevBuff[USART_REV_MAX_BUF_SIZE+1] = {0};
uint16_t usUsartRevCurrentPos;		/*	用于记录当前串口接收数据放于接收缓冲区的索引	*/

uint8_t	ucUsartSendBuff[USART_SEND_MAX_BUF_SIZE] = {0};
uint16_t ucUsartSendSize= 0;
uint16_t ucUsartSendCnt = 0;
