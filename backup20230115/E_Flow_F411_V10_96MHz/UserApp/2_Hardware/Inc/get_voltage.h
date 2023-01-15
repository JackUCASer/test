#ifndef	__GET_VOLTAGE_H
#define	__GET_VOLTAGE_H
#include <stdint.h>

#define ADC_CONVERTED_DATA_BUFFER_SIZE  3			/*		����ADCת��������ݻ�����С		*/
#define VDDA_APPLI   					3300		/*		����ο���ѹ		*/

/*		ȫ�ֱ���		*/
extern uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];

typedef struct
{
	uint16_t usb_vol;
	uint16_t bat_vol;
	uint16_t footPedal_vol;
}SysVol,*pSysVol;

/*		�ⲿ�ӿں���		*/
void Enable_ADC_DMA(void);
SysVol* Get_Voltage(void);
void Stop_ADC(void);
float Get_Charger_Vol(void);
uint8_t Is_Charger_Adapter_Right(void);
float Get_Battery_Vol(void);
uint16_t Get_Foot_Pedal_Vol(void);

#endif
