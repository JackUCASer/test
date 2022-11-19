#ifndef _MACROS_H_
#define _MACROS_H_
#include "stm32f4xx_hal.h"
#include "gpio.h"

extern TIM_HandleTypeDef htim2; 
//PID Parameters
/*****************************************/

/**********CIK_motor 1 ****************/

#define KP_Current 0x03E3
#define KI_Current 0x461D

#define KP_Speed 0x42B8
#define KP_Speed_Gain 0x8011

#define KI_Speed 0x35A8
#define KI_Speed_Gain 0x8017

#define KP_Position 0x347E
#define KP_Position_Gain 0x000B

//**********Íù¸´**************

#define KP_Speed1 0x318A
#define KP_Speed_Gain1 0x800D

#define KI_Speed1 0x2FCF
#define KI_Speed_Gain1 0x8015

#define KP_Position1 0x33BB
#define KP_Position_Gain1 0x000B

#define TQ_DISP_GIRD 8

//Motor Parameters
#define m_p 	2						//pole pairs of the motor
#define m_Vin 	6						//(V)DC input voltage
#define m_Rs	1.8				//(ohm)winding resistor(phase-phase)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
#define m_Ld	21					//(uH)winding d axis inductor
#define m_Lq	21					//(uH)winding q axis inductor
#define m_J		2						//(g*cm^2)rotor inertia 
#define m_speed	7200			//(rpm)rated speed of the motor

//Load Parameters
#define m_gear_ratio	6		//gearbox ratio, set to 1 if no gearbox is used.
#define m_TL	0				//(mNm) load torque
#define m_JL	0				//(g*cm^2)load inertia

#define FS	40 					//switching frequency(kHz)
#define DEADTIME	300			//ns
#define PWMMODE		0			//0:ENx+PWMx  1:GLx+GHx(6 gate signals)

// AD Sample
#define AD_MOD		1			//0 current sensing resistors, 1 MP654x series
#define AD_GAIN		7			//7:2x  6:3x 5:4x 4:5x 3:6x 2:7x 1:8x 0:12
#define I_OCP		0x7FF		//OCP register value
#define m_Rpull		680			//(ohm) current sending pull-up resistor when using MP654x power stage(
#define m_Rsense 	50			//(mohm) crrent sensing resistor when using MP653x as power stage

	
#define nSLEEP_SET  		//GPIOB->BSRR = 0x00000001
#define nSLEEP_RESET  		//GPIOB->BSRR = 0x00010000


#define ERROR_RESET  		__NOP //reset
#define ERROR_SET   		__NOP //set;	


#define mp6570_enable()		HAL_GPIO_WritePin(MP6570_EN_GPIO_Port,MP6570_EN_Pin,GPIO_PIN_SET);
#define mp6570_disable()	HAL_GPIO_WritePin(MP6570_EN_GPIO_Port,MP6570_EN_Pin,GPIO_PIN_RESET);

#define PEND_SET			__NOP  
#define PEND_RESET			__NOP 

#define LOOPTIMERINT_ENABLE	__NOP//NVIC_EnableIRQ(SCTM1_IRQn)
#define LOOPTIMER_ENABLE    HAL_TIM_Base_Start_IT(&htim2);  
#define	LOOPTIMER_DISABLE	HAL_TIM_Base_Stop_IT(&htim2);

#endif
