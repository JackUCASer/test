#include "get_voltage.h"
#include "adc.h"
#include "dma.h"

uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]=	{0};		/* ADC转换完成后用于存储的数据变量 */

SysVol sysVol = {0,0,0};

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-05-19
* 函 数 名: Activate_DMA
* 功能说明: 使能DMA
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void Activate_DMA(void)
{
  /* 设置DMA传输的源和目标地址 */
  LL_DMA_ConfigAddresses(DMA2,
                         LL_DMA_STREAM_0,
                         LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                         (uint32_t)&aADCxConvertedData,
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetDataLength(DMA2,LL_DMA_STREAM_0,ADC_CONVERTED_DATA_BUFFER_SIZE);		/*		设置DMA传输的大小		*/
  LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_0);										/*		使能DMA传输		*/
}


/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-05-19
* 函 数 名: Activate_ADC
* 功能说明: 使能ADC
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void Activate_ADC(void)
{
	if (LL_ADC_IsEnabled(ADC1) == 0)										/*		判断ADC1是否已经使能		*/
	{
		LL_ADC_Enable(ADC1);												/*		使能ADC		*/
	}
	LL_ADC_REG_StartConversionSWStart(ADC1);								/*		启动转换		*/ 
	LL_ADC_REG_SetDMATransfer(ADC1,LL_ADC_REG_DMA_TRANSFER_UNLIMITED);		/*		设置DMA传输的模式		*/
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-05-19
* 函 数 名: Enable_ADC_DMA
* 功能说明: 使能ADC，DMA
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void Enable_ADC_DMA(void)
{
	Activate_DMA();
	Activate_ADC();
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-05-19
* 函 数 名: Get_Voltage
* 功能说明: 获取电压
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
SysVol* Get_Voltage(void)
{
	
	uint16_t bat_vol,usb_vol,FootPedal;
	
	/* 计算电压，单位：mV */
	usb_vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[0], LL_ADC_RESOLUTION_12B);	/*	单位：mV		*/
	bat_vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[1], LL_ADC_RESOLUTION_12B);	/*	单位：mV		*/
	FootPedal = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[2], LL_ADC_RESOLUTION_12B);/*	单位：mV		*/
	
	sysVol.usb_vol = usb_vol*11;
	sysVol.bat_vol = bat_vol*11;
	sysVol.footPedal_vol = FootPedal;
	
#if Debug_ADC
	myPrintf("%5.1f,	%5.1f,	%7.3f\r\n",usb_vol*11.0f/1000,	bat_vol*11.0f/1000,	FootPedal*1.0f/1000);	/*	打印五位带宽，一位小数		*/
#endif
	return &sysVol;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-05-19
* 函 数 名: Stop_ADC
* 功能说明: 关闭外设ADC及其DMA传输
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
void Stop_ADC(void)
{
	LL_ADC_Disable(ADC1);						/*		关闭ADC		*/
	LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_0);	/*		关闭DMA数据通道0		*/
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-09-24
* 函 数 名: get_charge_voltage
* 功能说明: 获取充电电压
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
float get_charge_voltage(void)
{
	uint16_t Usb_Vol;
	Usb_Vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[0], LL_ADC_RESOLUTION_12B);	/*	单位：mV		*/
	return Usb_Vol*11.0f/1000;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2022-02-14
* 函 数 名: is_charger_adapter_right
* 功能说明: 判断适配器电压是否正确
* 形    参: 
      
* 返 回 值: 
*			0: 适配器电压合适；
*			1: 适配器未接入或者适配器已坏；
*			2：适配器电压偏低；
*			3：适配器电压偏高
*********************************************************************************************************
*/
uint8_t is_charger_adapter_right(void)
{
	float adapter_vol= get_charge_voltage();
	if(adapter_vol < 1.4f)	/*	高于1.4V, 两个PN结导通，无法关闭电源	*/
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
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-09-24
* 函 数 名: get_battery_voltage
* 功能说明: 获取电池电压
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
float get_battery_voltage(void)
{
	uint16_t Bat_Vol;
	Bat_Vol = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[1], LL_ADC_RESOLUTION_12B);	/*	单位：mV		*/
	return Bat_Vol*11.0f/1000;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-09-14
* 函 数 名: get_footpedal_voltage
* 功能说明: 获取脚踏电压
* 形    参: 
      
* 返 回 值: 
*********************************************************************************************************
*/
uint16_t get_footpedal_voltage(void)
{
	uint16_t FootPedal;
	FootPedal = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, aADCxConvertedData[2], LL_ADC_RESOLUTION_12B);	/*	单位：mV		*/
	return FootPedal;
}
