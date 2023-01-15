#ifndef	__GL_CONFIG_H 
#define	__GL_CONFIG_H
#include <stdio.h>
#include "usbd_cdc_if.h"

/*	定义一些与调试有关的信息			*/

#define __USB_DEBUG_

#ifdef __USB_DEBUG_
#define myPrintf(format, ...)	usb_printf(format, ##__VA_ARGS__)
//#define myPrintf(format, ...)	printf(format, ##__VA_ARGS__)
#else	
#define myPrintf(format, ...)
#endif

#define USE_BOOTLOADER	1			// system_stm32f4xx.c中中断向量偏移地址对应做出修改

#define kPa_DEBUG	0				//	Page Mode界面下液压调试开关：1， 开启； 0， 关闭

#endif
