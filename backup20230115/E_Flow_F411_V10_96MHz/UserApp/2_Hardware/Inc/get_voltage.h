#ifndef	__GET_VOLTAGE_H
#define	__GET_VOLTAGE_H
#include <stdint.h>

#define ADC_CONVERTED_DATA_BUFFER_SIZE  3			/*		定义ADC转换后的数据缓区大小		*/
#define VDDA_APPLI   					3300		/*		定义参考电压		*/

/*		全局变量		*/
extern uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];

typedef struct
{
	uint16_t usb_vol;
	uint16_t bat_vol;
	uint16_t footPedal_vol;
}SysVol,*pSysVol;

/*		外部接口函数		*/
void Enable_ADC_DMA(void);
SysVol* Get_Voltage(void);
void Stop_ADC(void);
float Get_Charger_Vol(void);
uint8_t Is_Charger_Adapter_Right(void);
float Get_Battery_Vol(void);
uint16_t Get_Foot_Pedal_Vol(void);

#endif
