/******************************************************************************
 * @brief    ���λ���������(�ο�linux/kfifo)
 *
 * Copyright (c) 2016~2021, 
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date                   Notes 
 * 2016-05-30             Initial version
 * 2021-02-05             ���ӿ��пռ��ȡ�ӿ�.
 ******************************************************************************/
#include "ringbuffer.h"
#include <string.h>
#include <stddef.h>

#define min(a,b) ( (a) < (b) )? (a):(b)     
     
/*
 *@brief      ����һ���ջ��λ�����
 *@param[in]  r    - ���λ�����������
 *@param[in]  buf  - ���ݻ�����
 *@param[in]  len  - buf����(������2��N����)
 *@retval     bool
 */
bool Ring_Buf_Init(ring_buf_t *r,unsigned char *buf, unsigned int len)
{
    r->buf    = buf;
    r->size   = len;
    r->front  = r->rear = 0;
    return buf != NULL && (len & len -1) == 0;
}

/*
 *@brief      ��ջ��λ����� 
 *@param[in]  r - ����յĻ��λ�����
 *@retval     none
 */
void Ring_Buf_Clr(ring_buf_t *r)
{
    r->front = r->rear = 0;
}

/*
 *@brief      ��ȡ���λ��������ݳ���
 *@retval     ���λ���������Ч�ֽ��� 
 */
unsigned int Ring_Buf_Len(ring_buf_t *r)
{
    return r->rear - r->front;
}

/*
 *@brief      ��ȡ���λ�����пռ�
 *@retval     ���пռ�
 */
unsigned int Ring_Buf_Free_Space(ring_buf_t *r)
{
    return r->size - (unsigned int)(r->rear - r->front);
}

/*
 *@brief       ��ָ�����ȵ����ݷŵ����λ������� 
 *@param[in]   buf - ���ݻ�����
 *             len - ���������� 
 *@retval      ʵ�ʷŵ��е����� 
 */
unsigned int Ring_Buf_Put(ring_buf_t *r,unsigned char *buf,unsigned int len)
{
    unsigned int i;
    unsigned int left;
    left = r->size + r->front - r->rear;
    len  = min(len , left);
    i    = min(len, r->size - (r->rear & r->size - 1));   
    memcpy(r->buf + (r->rear & r->size - 1), buf, i); 
    memcpy(r->buf, buf + i, len - i);
    r->rear += len;     
    return len;
    
}

/*
 *@brief       �ӻ��λ������ж�ȡָ�����ȵ����� 
 *@param[in]   len - ��ȡ���� 
 *@param[out]  buf - ������ݻ�����
 *@retval      ʵ�ʶ�ȡ���� 
 */
unsigned int Ring_Buf_Get(ring_buf_t *r,unsigned char *buf,unsigned int len)
{
    unsigned int i;
    unsigned int left;    
    left = r->rear - r->front;
    len  = min(len , left);                                
    i    = min(len, r->size - (r->front & r->size - 1));
    memcpy(buf, r->buf + (r->front & r->size - 1), i);    
    memcpy(buf + i, r->buf, len - i);   
    r->front += len;
    return len;
}