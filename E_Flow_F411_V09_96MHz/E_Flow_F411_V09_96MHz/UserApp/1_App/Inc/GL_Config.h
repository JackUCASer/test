#ifndef	__GL_CONFIG_H 
#define	__GL_CONFIG_H
#include <stdio.h>
#include "usbd_cdc_if.h"

/*	����һЩ������йص���Ϣ			*/

#define __USB_DEBUG_

#ifdef __USB_DEBUG_
#define myPrintf(format, ...)	usb_printf(format, ##__VA_ARGS__)
//#define myPrintf(format, ...)	printf(format, ##__VA_ARGS__)
#else	
#define myPrintf(format, ...)
#endif

#define USE_BOOTLOADER	1			// system_stm32f4xx.c���ж�����ƫ�Ƶ�ַ��Ӧ�����޸�

#define kPa_DEBUG	0				//	Page Mode������Һѹ���Կ��أ�1�� ������ 0�� �ر�

#endif
