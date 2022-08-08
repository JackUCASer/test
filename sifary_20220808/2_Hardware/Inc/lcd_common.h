#ifndef __LCD_COMMON_H
#define __LCD_COMMON_H
#include <stdint.h>

#define USART_REV_MAX_BUF_SIZE		1023
#define USART_SEND_MAX_BUF_SIZE		32

/*	外部接口	*/
extern uint32_t* data_frame_end;
extern uint16_t* data_frame_start;
extern uint8_t  ucUsartRevBuff[USART_REV_MAX_BUF_SIZE+1];
extern uint16_t usUsartRevCurrentPos;

extern uint8_t	ucUsartSendBuff[USART_SEND_MAX_BUF_SIZE];		/*	发送缓冲区	*/
extern uint16_t ucUsartSendSize;								/*	待发送缓冲区大小	*/
extern uint16_t ucUsartSendCnt;									/*	发送字节计数器	*/

#endif
