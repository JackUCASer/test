#include "get_voltage.h"
#include "adc.h"
#include "dma.h"

uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]=	{0};		/* ADCת����ɺ����ڴ洢�����ݱ��� */

SysVol sysVol = {0,0,0};

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-05-19
* �� �� ��: Activate_DMA
* ����˵��: ʹ��DMA
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Activate_DMA(void)
{
  /* ����DMA�����Դ��Ŀ���ַ */
  LL_DMA_ConfigAddresses(DMA2,
                         LL_DMA_STREAM_0,
                         LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                         (uint32_t)&aADCxConvertedData,
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetDataLength(DMA2,LL_DMA_STREAM_0,ADC_CONVERTED_DATA_BUFFER_SIZE);		/*		����DMA����Ĵ�С		*/
  LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_0);										/*		ʹ��DMA����		*/
}


/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-05-19
* �� �� ��: Activate_ADC
* ����˵��: ʹ��ADC
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Activate_ADC(void)
{
	if (LL_ADC_IsEnabled(ADC1) == 0)										/*		�ж�ADC1�Ƿ��Ѿ�ʹ��		*/
	{
		LL_ADC_Enable(ADC1);												/*		ʹ��ADC		*/
	}
	LL_ADC_REG_StartConversionSWStart(ADC1);								/*		����ת��		*/ 
	LL_ADC_REG_SetDMATransfer(ADC1,LL_ADC_REG_DMA_TRANSFER_UNLIMITED);		/*		����DMA�����ģʽ		*/
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-05-19
* �� �� ��: Enable_ADC_DMA
* ����˵��: ʹ��ADC��DMA
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Enable_ADC_DMA(void)
{
	Activate_DMA();
	Activate_ADC();
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-05-19
* �� �� ��: Get_Voltage
* ����˵��: ��ȡ��ѹ
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
SysVol* Get_Voltage(void)
{
	
	uint16_t bat_vol,usb_vol,FootPedal;
	
	/* �����ѹ����λ��mV */
	usb_vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[0], LL_ADC_RESOLUTION_12B);	/*	��λ��mV		*/
	bat_vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[1], LL_ADC_RESOLUTION_12B);	/*	��λ��mV		*/
	FootPedal = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[2], LL_ADC_RESOLUTION_12B);/*	��λ��mV		*/
	
	sysVol.usb_vol = usb_vol*11;
	sysVol.bat_vol = bat_vol*11;
	sysVol.footPedal_vol = FootPedal;
	
#if Debug_ADC
	myPrintf("%5.1f,	%5.1f,	%7.3f\r\n",usb_vol*11.0f/1000,	bat_vol*11.0f/1000,	FootPedal*1.0f/1000);	/*	��ӡ��λ����һλС��		*/
#endif
	return &sysVol;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-05-19
* �� �� ��: Stop_ADC
* ����˵��: �ر�����ADC����DMA����
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
void Stop_ADC(void)
{
	LL_ADC_Disable(ADC1);						/*		�ر�ADC		*/
	LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_0);	/*		�ر�DMA����ͨ��0		*/
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-09-24
* �� �� ��: get_charge_voltage
* ����˵��: ��ȡ����ѹ
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
float get_charge_voltage(void)
{
	uint16_t Usb_Vol;
	Usb_Vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[0], LL_ADC_RESOLUTION_12B);	/*	��λ��mV		*/
	return Usb_Vol*11.0f/1000;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2022-02-14
* �� �� ��: is_charger_adapter_right
* ����˵��: �ж���������ѹ�Ƿ���ȷ
* ��    ��: 
      
* �� �� ֵ: 
*			0: ��������ѹ���ʣ�
*			1: ������δ��������������ѻ���
*			2����������ѹƫ�ͣ�
*			3����������ѹƫ��
*********************************************************************************************************
*/
uint8_t is_charger_adapter_right(void)
{
	float adapter_vol= get_charge_voltage();
	if(adapter_vol < 1.4f)	/*	����1.4V, ����PN�ᵼͨ���޷��رյ�Դ	*/
		return 1;
	else if(adapter_vol < 18.0f)
		return 2;
	else if(adapter_vol > 24.0f)
		return 3;
	else
		return 0;
}
/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-09-24
* �� �� ��: get_battery_voltage
* ����˵��: ��ȡ��ص�ѹ
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
float get_battery_voltage(void)
{
	uint16_t Bat_Vol;
	Bat_Vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[1], LL_ADC_RESOLUTION_12B);	/*	��λ��mV		*/
	return Bat_Vol*11.0f/1000;
}

/*
*********************************************************************************************************
* �� �� �ߣ���Զ����������ҽ�Ƽ������޹�˾
* ��	  ����V1.0
* ��    �ڣ�2021-09-14
* �� �� ��: get_footpedal_voltage
* ����˵��: ��ȡ��̤��ѹ
* ��    ��: 
      
* �� �� ֵ: 
*********************************************************************************************************
*/
uint16_t get_footpedal_voltage(void)
{
	uint16_t FootPedal;
	FootPedal = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[2], LL_ADC_RESOLUTION_12B);	/*	��λ��mV		*/
	return FootPedal;
}
