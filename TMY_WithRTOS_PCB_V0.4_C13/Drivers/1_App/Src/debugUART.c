#include "debugUART.h"

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
	USART2->SR;		/*	复位时TC和TXE被置1，所以先读一下SR寄存器，再写DR寄存器，刚好清除了TC标志位	*/
  LL_USART_TransmitData8(USART2, (uint8_t)ch);
	while(!LL_USART_IsActiveFlag_TC(USART2))
	{}
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  ch = LL_USART_ReceiveData8(USART2);
  return ch;
}

