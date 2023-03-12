/*
*********************************************************************************************************
*
*	模块名称 : 按键驱动模块
*	文件名称 : bsp_key.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __KEY_H
#define __KEY_H
#include "stm32f4xx_hal.h"

#define KEY_COUNT    6	   					/* 按键个数, 4个独立建 + 2个组合键 */


/* 根据应用程序的功能重命名按键宏 */
#define KEY_1_DOWN			KEY_ON_DOWN										//开机按键   启动/停止
#define KEY_1_UP			KEY_ON_UP
#define KEY_1_LONG			KEY_ON_LONG

#define KEY_2_DOWN			KEY_SEL_DOWN									//待机模式下，设置按键
#define KEY_2_UP			KEY_SEL_UP
#define KEY_2_LONG			KEY_SEL_LONG

#define KEY_3_DOWN			KEY_DOWN_DOWN									//待机模式下，下翻按键
#define KEY_3_UP			KEY_DOWN_UP
#define KEY_3_LONG			KEY_DOWN_LONG

#define KEY_4_DOWN			KEY_UP_DOWN										//待机模式下，上翻按键
#define KEY_4_UP			KEY_UP_UP
#define KEY_4_LONG			KEY_UP_LONG

#define KEY_5_DOWN			KEY_SET_DOWN									//开机模式下，设置按键，是组合按键“开机+设置”
#define KEY_5_UP			KEY_SET_UP
#define KEY_5_LONG			KEY_SET_LONG

#define KEY_6_DOWN			KEY_FACTORY_DOWN								//开机模式下，工厂模式按键，是组合按键“开机+上翻”
#define KEY_6_UP			KEY_FACTORY_UP
#define KEY_6_LONG			KEY_FACTORY_LONG

/* 按键ID, 主要用于bsp_KeyState()函数的入口参数 */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3,
	KID_K4,
	KID_K5,
	KID_K6,
}KEY_ID_E;

/*
	按键滤波时间50ms, 单位10ms。
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
	即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME   5
#define KEY_LONG_TIME     100			/* 单位10ms， 持续1秒，认为长按事件 */

/*
	每个按键对应1个全局的结构体变量。
*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	uint8_t  Count;				/* 滤波器计数器 */
	uint16_t LongCount;		/* 长按计数器 */
	uint16_t LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	uint8_t  State;				/* 按键当前状态（按下还是弹起） */
	uint8_t  RepeatSpeed;	/* 连续按键周期 */
	uint8_t  RepeatCount;	/* 连续按键计数器 */
}KEY_T;

/*
	定义键值代码, 必须按如下次序定时每个键的按下、弹起和长按事件

	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2) 编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,					/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,					/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,					/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */

	KEY_4_DOWN,				/* 4键按下 */
	KEY_4_UP,					/* 4键弹起 */
	KEY_4_LONG,				/* 4键长按 */
	
	KEY_5_DOWN,				/* 4键按下 */
	KEY_5_UP,					/* 4键弹起 */
	KEY_5_LONG,				/* 4键长按 */
	
	KEY_6_DOWN,				/* 4键按下 */
	KEY_6_UP,					/* 4键弹起 */
	KEY_6_LONG,				/* 4键长按 */

}KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	10
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	uint8_t Read;					/* 缓冲区读指针1 */
	uint8_t Write;					/* 缓冲区写指针 */
	uint8_t Read2;					/* 缓冲区读指针2 */
}KEY_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);

#endif


