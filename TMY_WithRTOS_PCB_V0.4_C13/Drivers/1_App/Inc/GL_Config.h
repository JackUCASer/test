#ifndef	__GL_CONFIG_H 
#define	__GL_CONFIG_H

#include "usbd_cdc_if.h"

/*	����һЩ������йص���Ϣ			*/

/*	usb_printf:����USB���⴮�ڣ�	printf:	���ô���2	*/
#define myPrintf		usb_printf
//#define myPrintf		none_pritntf

/*
		0: ȡ��
		1����Ч
*/
#define	Debug_SPI_Flash		1	

#define Debug_Bootloader	0

#define Debug_ADC			1

#define Debug_Fats			1

#define	Debug_USB_MSC_CDC	1

/*		����ʹ��USB CDC��Ϊ����ʹ��		*/
#if Debug_USB_MSC_CDC
	#define Debug_USB_CDC	1
#endif


#endif
