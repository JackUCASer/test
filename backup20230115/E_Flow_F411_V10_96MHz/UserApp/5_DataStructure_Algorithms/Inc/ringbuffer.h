/******************************************************************************
 * @brief    ���λ���������(�ο�linux/kfifo)
 *
 * Copyright (c) 2016~2021
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date                   Notes 
 * 2016-05-30             Initial version.
 * 2021-02-05             ���ӿ��пռ��ȡ�ӿ�.
 ******************************************************************************/

#ifndef _RING_BUF_H_
#define _RING_BUF_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*���λ�����������*/
typedef struct {
     unsigned char *buf;    /*���λ�����        */
     unsigned int size;     /*���λ�����        */
     unsigned int front;    /*ͷָ��            */
     unsigned int rear;     /*βָ��            */
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
