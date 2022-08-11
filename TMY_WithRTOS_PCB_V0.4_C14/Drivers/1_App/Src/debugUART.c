#include "debugUART.h"

/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
	USART2->SR;		/*	��λʱTC��TXE����1�������ȶ�һ��SR�Ĵ�������дDR�Ĵ������պ������TC��־λ	*/
  LL_USART_TransmitData8(USART2, (uint8_t)ch);
	while(!LL_USART_IsActiveFlag_TC(USART2))
	{}
  return ch;
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  ch = LL_USART_ReceiveData8(USART2);
  return ch;
}

