#ifndef __BEEP_H
#define	__BEEP_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "stm32f4xx_hal.h"


typedef struct BEE
{
		uint8_t short_ring;
		uint8_t long_ring;
		uint8_t ring_freq;
		uint8_t apex_beep_on;
		uint8_t fre;
		uint8_t beep_cnt;

} BEEP;

extern BEEP Ring;
void ENDO_BEEP();													//声音驱动，在定时器运行
void ApexRing(uint8_t fre,uint8_t on);		//根测声音频率
void EndoRing(uint8_t fre,uint8_t on);		//马达声音
#ifdef	__cplusplus
}
#endif

#endif	/* BEEP_H */