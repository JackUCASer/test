#ifndef	__GL_CONFIG_H 
#define	__GL_CONFIG_H

#include "usbd_cdc_if.h"

/*	定义一些与调试有关的信息			*/

/*	usb_printf:采用USB虚拟串口；	printf:	采用串口2	*/
#define myPrintf		usb_printf
//#define myPrintf		none_pritntf

/*
		0: 取消
		1：有效
*/
#define	Debug_SPI_Flash		1	

#define Debug_Bootloader	0

#define Debug_ADC			1

#define Debug_Fats			1

#define	Debug_USB_MSC_CDC	1

/*		配置使用USB CDC作为串口使用		*/
#if Debug_USB_MSC_CDC
	#define Debug_USB_CDC	1
#endif


#endif
