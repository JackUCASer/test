#ifndef __OIL_CS1231_H
#define __OIL_CS1231_H
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define IO_ADC_DDAT			HAL_GPIO_ReadPin(HX711_DOUT_GPIO_Port,HX711_DOUT_Pin)
#define IO_ADC_SCLK_SET		HAL_GPIO_WritePin(HX711_SCK_GPIO_Port,HX711_SCK_Pin,GPIO_PIN_SET)
#define IO_ADC_SCLK_RST		HAL_GPIO_WritePin(HX711_SCK_GPIO_Port,HX711_SCK_Pin,GPIO_PIN_RESET)

union LongData
{
	int32_t word;
	uint8_t byte[4];
};

typedef enum{
	No_Initialized = 0x00,		/*	未初始化	*/
	Initialized,				/*	初始化完成	*/
	If_Has_New_Data,
	Wait_Read_Data,
	New_Data_Read_Over,
}CS1231_State;


void Init_adcIC(void);
void StandBy_adcIC(void);
CS1231_State get_oil_pressure(uint32_t* data_addr);
#endif



