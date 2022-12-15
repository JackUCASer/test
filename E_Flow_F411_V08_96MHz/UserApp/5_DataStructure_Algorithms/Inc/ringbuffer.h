/******************************************************************************
 * @brief    环形缓冲区管理(参考linux/kfifo)
 *
 * Copyright (c) 2016~2021
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date                   Notes 
 * 2016-05-30             Initial version.
 * 2021-02-05             增加空闲空间获取接口.
 ******************************************************************************/

#ifndef _RING_BUF_H_
#define _RING_BUF_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*环形缓冲区管理器*/
typedef struct {
     unsigned char *buf;    /*环形缓冲区        */
     unsigned int size;     /*环形缓冲区        */
     unsigned int front;    /*头指针            */
     unsigned int rear;     /*尾指针            */
}ring_buf_t;

bool Ring_Buf_Init(ring_buf_t *r,unsigned char *buf,unsigned int size);

void Ring_Buf_Clr(ring_buf_t *r);

unsigned int Ring_Buf_Len(ring_buf_t *r);

unsigned int Ring_Buf_Put(ring_buf_t *r,unsigned char *buf,unsigned int len);

unsigned int Ring_Buf_Get(ring_buf_t *r,unsigned char *buf,unsigned int len);

unsigned int Ring_Buf_Free_Space(ring_buf_t *r);

#ifdef __cplusplus
}
#endif

#endif
