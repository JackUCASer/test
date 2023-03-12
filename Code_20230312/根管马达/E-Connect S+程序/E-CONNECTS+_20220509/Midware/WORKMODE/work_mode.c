
////////////////////////////////////////////////////////////////////
/// @file work_mode.c
/// @brief ����ģʽ����Ҫ�д���ģʽ����ʾLOGOģʽ������ģʽ�ȵ�
/// 
/// �ļ���ϸ������	WorkMode_e Rotory(WorkMode_e mode);					// �����˶�ģʽ��������ת����ת
/// 				WorkMode_e Reciproc(WorkMode_e mode);				// �����˶�ģʽ
/// 				WorkMode_e ATC_RUN(WorkMode_e mode);				// ATC�˶�ģʽ
/// 				WorkMode_e Measure(WorkMode_e mode);				// ����ģʽ
/// 				WorkMode_e RunWithApex(WorkMode_e mode);			// ����+����ģʽ
/// 				WorkMode_e RecWithApex(WorkMode_e mode);			// ����+����ģʽ
/// 				WorkMode_e ATCwithApex(WorkMode_e mode);			// ATC+����ģʽ
/// 				WorkMode_e Standby(WorkMode_e mode);				// ����ģʽ	
/// 				WorkMode_e Charging(WorkMode_e mode);				// ���ģʽ
/// 				WorkMode_e Fault(WorkMode_e mode);					// ���д���ģʽ
/// 				WorkMode_e PowerOff(WorkMode_e mode);				// �ػ�Ԥ��ģʽ
/// 				WorkMode_e DispOff(WorkMode_e mode);				// �ر���ʾģʽ
/// 				WorkMode_e Displaylogo(WorkMode_e mode);			// ��ʾLOGOģʽ
///
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////

#include "work_mode.h"
#include "datatype.h"
#include "menuUI.h"
#include "menu.h"
#include "menudisp.h"
#include "menudata.h"
#include "lcd.h"
#include "beep.h"
#include "apex.h"
#include "../lvgl/lvgl.h"
#include "delay.h"
#include "mp6570.h"
#include "control.h"
#include "customer_control.h"
#include "macros.h"
#include "presetting.h"
#include "_misc.h"
#include "battery.h"
#include "key.h"
#include "key_Task.h"
#include "Multiple_Styles.h"
#include "DataStorage.h"
#include "main.h"
#include "spi.h"
#include "adc.h"
#include "dma.h"
#include "Error.h"
#include "MP6570_SPI.h"

WorkMode_e WorkMode;

int 		APEX_cnt = 0;
int 		iq_motor = 0;
int 		iq_sum = 0;
uint8_t 	Disp_TL_cnt;
uint8_t 	Disp_TL_limit_flag = 0;
uint16_t	Disp_TL_limit_cnt;
uint8_t 	flag_running=0,rev_first=1;
uint8_t 	ap_flag=0;
uint8_t 	ChargDisp_cnt=0,ChargDisp_flag=0,ChargDisp_cnt_100=0;
int 		Torque_limit;
float 		Torque_p;
int 		Disp_Torque;
extern uint16_t IQ_Cnt; 													//����������������ֹ����ٵ���ת����
extern u16 reach_upper_times;
extern u16 reach_lower_times;

extern WorkMode_e gTempMode;
extern uint8_t KeyFuncIndex;
extern uint8_t Last_apex_pos;						//������ʾ��λ�ÿ��ƣ���һ��λ�õ��¼
extern uint8_t LVGL_task_Flag;
extern unsigned int reciprocate_sw;					// ����ȫ�ֱ���
extern unsigned char LVGL_TL_Flag;

extern TIM_HandleTypeDef htim10;
extern IWDG_HandleTypeDef hiwdg;					// ϵͳ���Ź�
extern ADC_HandleTypeDef 	hadc1;					// ADת��

extern uint8_t Outside_file_add_num[1];
extern uint8_t  Theta_flag ;
/**********************************************************************************************************
*	�� �� ��: WorkMode_e Rotory(WorkMode_e mode)
*	����˵��: �����˶�ģʽ��������ת����תģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
uint8_t Torque_reach_ring = 0;
WorkMode_e Rotory(WorkMode_e mode)
{
	uint8_t i;
	
	//������ʼ��
	if((flag_running == 0) && (LVGL_TL_Flag == 1))
	{
		iq_sum = 0;
		Iq_cnt = 0;	
		
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		
		//����Ť�غ��ٶȣ����õ���˵�ѹ
		if((sel.Torque <t30 )&&(sel.Speed<s600))
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_RESET);			//���õ����ѹΪ6V
		}else
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);			//���õ����ѹΪ9V
		}
		
		//Ѱ��Ť������
		if(sel.Torque <=2)
		{
			Torque_limit = S_T_I_COE1[sel.Torque][sel.Speed];
		}else if((sel.Torque>2) &&(sel.Torque <=5))
		{
			Torque_limit = S_T_I_COE2[sel.Torque-3][sel.Speed];	
		}else if((sel.Torque>5) &&(sel.Torque <=11))
		{
			Torque_limit = S_T_I_COE3[sel.Torque-6][sel.Speed];	
		}else if(sel.Torque>11)
		{
			Torque_limit = S_T_I_COE4[sel.Torque-12][sel.Speed];
		}
		
		LOOPTIMER_ENABLE;
		LVGL_task_Flag = 1;	
		flag_running = 1;
		stop();
		
		delay_ms(200);
		motor_settings.mode = 0;
		
		//���õ��ת����Ť������
		if(sel.OperaMode == 0)																									//��ת
		{
			motor_settings.forward_speed = -d_Speed[sel.Speed];
			motor_settings.reverse_speed =  d_Speed[sel.Speed];	
			motor_settings.upper_threshold =  Torque_limit - NoLoadCurrent[sel.Speed] - NoLoadCurrent[sel.Speed+28];			//ת�ر�����������
//			motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[sel.Speed])/2 - NoLoadCurrent[sel.Speed+28];			//ת��ȡ��������������	
			motor_settings.lower_threshold =  (Torque_limit- NoLoadCurrent[sel.Speed]) /2 + NoLoadCurrent[sel.Speed];			//ת��ȡ��������������	
		}
		else if(sel.OperaMode == 1)																								//��ת
		{
			motor_settings.forward_speed = d_Speed[sel.Speed];
			motor_settings.reverse_speed = -d_Speed[sel.Speed];
			motor_settings.upper_threshold =  Torque_limit;																		//ת�ر�����������
//			motor_settings.lower_threshold =  (Torque_limit- NoLoadCurrent[sel.Speed]) /2 + NoLoadCurrent[sel.Speed];			//ת��ȡ��������������	
			motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[sel.Speed])/2 - NoLoadCurrent[sel.Speed+28];			//ת��ȡ��������������	
			
					
		}
		Torque_p = ((float)(Torque_limit - NoLoadCurrent[sel.Speed])) / d_TqBlinkGrid[sel.Torque];
		update_settings(&motor_settings);
		start(); 
	}
	
	//���ϼ��
	if((status_mp6570 != 0x04))														//��⵽MP6570���ڹ��ϲ����ڹ���״̬
	{
		stop();
		delay_ms(10);
		LOOPTIMER_DISABLE;
		mp6570_disable();  																	//disable mp6570
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		delay_ms(100);
		mp6570_enable();
		flag_running = 0;
		LVGL_TL_Flag = 0;
	}else
	{	
		CalculateApex();	
	}
	
	if(ADC_ConvertedValue[0] < 100)
	{
		HAL_TIM_Base_Stop_IT(&htim10); 							// 2ms
		HAL_ADC_Stop_DMA(&hadc1);
		MX_DMA_Init();
		MX_ADC1_Init();
		HAL_TIM_Base_Start_IT(&htim10); 							// 2ms
	}
	
	//��ֵ�˲�
	iq_sum += motor_iq;
	Iq_cnt++;
	if(Iq_cnt >= Iq_num)
	{
		iq_motor = iq_sum / Iq_cnt;
		Iq_cnt = 0;
		iq_sum = iq_motor * Iq_num / 2;
		Iq_cnt = Iq_num / 2;
		if(sel.OperaMode == 0)
		{
			if(motor_status.status == Status_FORWARD)
			{
				
				iq_motor -= NoLoadCurrent[sel.Speed+28];
			}else 
			{
				iq_motor -= NoLoadCurrent[sel.Speed];
			}
		}
		else if(sel.OperaMode == 1)
		{	
			if(motor_status.status == Status_FORWARD)
			{
				iq_motor -= NoLoadCurrent[sel.Speed];
			}else 
			{
				iq_motor -= NoLoadCurrent[sel.Speed+28];
			}
		}
		iq_motor = abs(iq_motor);
		Disp_Torque = (int)(((float)iq_motor)/Torque_p);
		Read_ig_flag = 1;
	}
	
	//��������
	if(sys.error == 1)
	{
		mode = Fault_mode;  																	//�������״̬
	}
	
	//��������
	if(sel.OperaMode == 0)
	{
		if(		(Disp_Torque > (d_TqBlinkGrid[sel.Torque]/2))
			&& 	(Disp_Torque < (d_TqBlinkGrid[sel.Torque])) 
			&& 	(motor_status.status == Status_FORWARD) 
			&& 	(motor_status.reach_torque != 1))
		{
			EndoRing(50,1);	
			Torque_reach_ring = 0;
		}else if(motor_status.status == Status_REVERSE)
		{
			if(Torque_reach_ring == 0)
			{
				EndoRing(50,0);
				Ring.long_ring = 1;
			}else 
			{
				EndoRing(100,1);	
			}	
		}else if((Disp_Torque <= (d_TqBlinkGrid[sel.Torque]/2))
			&&	(motor_status.status == Status_FORWARD) 
			&& 	(motor_status.reach_torque != 1))
		{
			EndoRing(50,0);																		//��
			Torque_reach_ring = 0;	
		}	
	}else if(sel.OperaMode == 1)
	{
		if(motor_status.status == Status_REVERSE)
		{
			if(Torque_reach_ring == 0)
			{
				EndoRing(50,0);
				Ring.long_ring = 1;
			}else 
			{
				EndoRing(100,1);	
			}
		}else
		{
			EndoRing(100,1);
			Torque_reach_ring = 0;			
		}	
	}
	
	//Ť�ع����˸����
	if(Disp_TL_limit_cnt > 1000)
	{
		if(Disp_TL_limit_flag)
		{	
			Disp_TL_limit_flag = 0;
		}
		else
		{
			Disp_TL_limit_flag = 1;
		}
		Disp_TL_limit_cnt = 0;
	}
	//Ť������ʾ
	if(Read_ig_flag == 1)
	{
		Disp_torque_bar_scale(Disp_Torque,Disp_TL_limit_flag);
		Disp_TL_cnt = 0;
		Read_ig_flag = 0;
	}
	mode = CheckKey_RunMode(mode);					//������ģʽ�µİ���ɨ��
	mode = CheckHardwareFault(mode);
	mode=RunningTurn2Apex(mode);					//������������Ƿ��и����ź�
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
	
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e Reciproc(WorkMode_e mode)
*	����˵��: ��������ģʽ��������ת�Ƕȡ���ת�Ƕȡ�ת�����ơ������ٶȵ�
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e Reciproc(WorkMode_e mode)
{
	if((flag_running == 0) && (LVGL_TL_Flag == 1))
	{
		MX_SPI1_Init1();
		delay_ms(20);
		init_MC();

		regi[0x24] =  	KI_Speed1;//800;//0x0E30;			//Speed loop Ki
		regi[0x25] =  	KI_Speed_Gain1;//0x8014;			//Speed loop Ki gain
	  
	  	regi[0x22] =  	KP_Speed1;//800;//0x08D2;			//Speed loop Kp
		regi[0x23] =  	KP_Speed_Gain1;//0x800C;			//Speed loop Kp gain
		
		regi[0x1A] = 	KP_Position1;//1500;//0x04F3;		//position loop Kp
		regi[0x1B] =	KP_Position_Gain1;//0x000E;			//position loop Kp gain
	
		
		//����Ť�غ��ٶȣ����õ���˵�ѹ
		if((sel.Torque < t30 )&&(r_d_speed[sel.RecSpeed] < s600))
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_RESET);			//���õ����ѹΪ6V
		}else
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);			//���õ����ѹΪ9V
		}
		//Ѱ��Ť������
		if(sel.Torque <=2)
		{
			Torque_limit = S_T_I_COE1[sel.Torque][r_d_speed[sel.RecSpeed]];
		}else if((sel.Torque>2) &&(sel.Torque <=5))
		{
			Torque_limit = S_T_I_COE2[sel.Torque-3][r_d_speed[sel.RecSpeed]];	
		}else if((sel.Torque>5) &&(sel.Torque <=11))
		{
			Torque_limit = S_T_I_COE3[sel.Torque-6][r_d_speed[sel.RecSpeed]];	
		}else if(sel.Torque>11)
		{
			Torque_limit = S_T_I_COE4[sel.Torque-12][r_d_speed[sel.RecSpeed]];
		}
			
		LOOPTIMER_ENABLE;
		flag_running = 1;

		stop();
		delay_ms(200);
		motor_settings.mode = 1; 																															//����ģʽ
		motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed];			  																				//����ģʽ�ٶ�����
		motor_settings.forward_position = d_AngleCCW[sel.AngleCW];																						//��ǰת���Ƕ�  ǰתΪ������תΪ��
		motor_settings.reverse_position = d_AngleCW[sel.AngleCCW];																						//���ת���Ƕ�
		
		if( -motor_settings.forward_position >= motor_settings.reverse_position )
		{	
			motor_settings.upper_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28]);			//ת�ر�����������
			motor_settings.lower_threshold = ((Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]]);			//ת��ȡ��������������	
		}else
		{
			motor_settings.upper_threshold = Torque_limit;																									//ת�ر�����������
			motor_settings.lower_threshold = ((Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28]);		//ת��ȡ��������������
		}
		update_settings(&motor_settings);
		start();
	}
	
	if(reciprocate_sw == 1)
	{
		delay_ms(100);
		motor_settings.mode = 0;																		//�ٶ�ģʽ
		if( -motor_settings.forward_position >= motor_settings.reverse_position )
		{
			motor_settings.forward_speed = 350;																//��ת�ٶ� -350rpm
			motor_settings.reverse_speed = -350;																//��ת�ٶ� 350rpm
		}else
		{
			motor_settings.forward_speed =-350;																//��ת�ٶ� -350rpm
			motor_settings.reverse_speed = 350;																//��ת�ٶ� 350rpm
		}
		
		motor_settings.upper_threshold = 1000;															//ת�ر�����������
		motor_settings.lower_threshold = 500;															//ת��ȡ��������������
		update_settings(&motor_settings);
		start(); 
		reciprocate_sw = 0;
	}
	
	//���ϼ��
	if((status_mp6570 != 0x04))														//��⵽MP6570���ڹ��ϲ����ڹ���״̬
	{
		stop();
		delay_ms(10);
		LOOPTIMER_DISABLE;
		mp6570_disable();  																	//disable mp6570
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		regi[0x24] =  KI_Speed1;//800;//0x0E30;			//Speed loop Ki
		regi[0x25] =  KI_Speed_Gain1;//0x8014;			//Speed loop Ki gain
	  
	  	regi[0x22] =  KP_Speed1;//800;//0x08D2;			//Speed loop Kp
		regi[0x23] =  KP_Speed_Gain1;//0x800C;			//Speed loop Kp gain
		delay_ms(100);
		mp6570_enable();
		flag_running = 0;
		LVGL_TL_Flag = 0;
	}else
	{	
		CalculateApex();	
	}
	
	mode = CheckKey_RunMode(mode);					//������ģʽ�µİ���ɨ��
	mode = CheckHardwareFault(mode);
	mode=RunningTurn2Apex(mode);					//������������Ƿ��и����ź�
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
 
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e ATC_RUN(WorkMode_e mode)
*	����˵��: ATC����ģʽ�������ٶȡ�ת�����Ƶ�
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e ATC_RUN(WorkMode_e mode)
{
	uint8_t i;
	
	if((flag_running == 0) && (LVGL_TL_Flag == 1))
	{
		iq_sum = 0;
		Iq_cnt = 0;
		
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		
		//����Ť�غ��ٶȣ����õ���˵�ѹ
		if((sel.AtrTorque < t30 )&&(r_d_speed[sel.RecSpeed] < s600))
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_RESET);			//���õ����ѹΪ6V
		}else
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);			//���õ����ѹΪ9V
		}
		//Ѱ��Ť������
		if(sel.AtrTorque <=2)
		{
			Torque_limit = S_T_I_COE1[sel.AtrTorque][r_d_speed[sel.RecSpeed]];
		}else if((sel.AtrTorque > 2) &&(sel.AtrTorque <= 5))
		{
			Torque_limit = S_T_I_COE2[sel.AtrTorque-3][r_d_speed[sel.RecSpeed]];	
		}else if((sel.AtrTorque > 5) &&(sel.AtrTorque <= 11))
		{
			Torque_limit = S_T_I_COE3[sel.AtrTorque-6][r_d_speed[sel.RecSpeed]];	
		}else if(sel.AtrTorque > 11)
		{
			Torque_limit = S_T_I_COE4[sel.AtrTorque-12][r_d_speed[sel.RecSpeed]];
		}
		
		LOOPTIMER_ENABLE;
		LVGL_task_Flag = 1;
		flag_running = 1;	 

		stop();
		delay_ms(100);
		motor_settings.mode = 0;																								//ATC

		motor_settings.forward_speed =  -r_Speed[sel.RecSpeed];																	//��ת�ٶ�
		motor_settings.reverse_speed =  r_Speed[sel.RecSpeed];																	//��ת�ٶ�

		motor_settings.upper_threshold = Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];			//ת�ر�����������
//		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������
		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]];			//ת��ȡ��������������			
		update_settings(&motor_settings);		
		start(); 	
		Torque_p = ((float)(Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])) / d_TqBlinkGrid[sel.AtrTorque];
	}

		//���ϼ��
	if((status_mp6570 != 0x04))														//��⵽MP6570���ڹ��ϲ����ڹ���״̬
	{
		stop();
		delay_ms(10);
		LOOPTIMER_DISABLE;
		mp6570_disable();  																	//disable mp6570
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		delay_ms(100);
		mp6570_enable();
		flag_running = 0;
		LVGL_TL_Flag = 0;
	}else
	{	
		CalculateApex();	
	}
	
	//��ֵ�˲�
	iq_sum += motor_iq;
	Iq_cnt++;
	if(Iq_cnt >= Iq_num)
	{
		iq_motor = iq_sum / Iq_cnt;
		Iq_cnt = 0;
		iq_sum = iq_motor * Iq_num / 2;
		Iq_cnt = Iq_num / 2;
		if(motor_status.mode == 0)
		{
			if(motor_status.status == Status_FORWARD)
			{
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]];
			}else 
			{
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];
			}
		}
		else if(motor_status.mode == 1)
		{	
			if(motor_status.status == Status_FORWARD)
			{
				
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];
			}else 
			{
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]];
			}
		}
		iq_motor = abs(iq_motor);
		Disp_Torque = (int)(((float)iq_motor)/Torque_p);
		Read_ig_flag = 1;
	}
	
	//Ť�ع����˸����
	if(Disp_TL_limit_cnt > 1000)
	{
		if(Disp_TL_limit_flag)
		{	
			Disp_TL_limit_flag = 0;
		}
		else
		{
			Disp_TL_limit_flag = 1;
		}
		Disp_TL_limit_cnt = 0;
	}
	//Ť������ʾ
	if(Read_ig_flag == 1)
	{
		Disp_torque_bar_scale(Disp_Torque,Disp_TL_limit_flag);
		Disp_TL_cnt = 0;
		Read_ig_flag = 0;
	}
//	if(sel_page_flash ==1)
//	{
//		Standy_ON(motor_iq,Disp_TL_limit_flag);
//		sel_page_flash = 0;
//	}
	
	if(reciprocate_sw == 2)		
	{
		stop();
		delay_ms(100);
		motor_settings.mode = 1;
		motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed];	
		
		motor_settings.forward_position = d_AngleCW[sel.AngleCW];
		motor_settings.reverse_position = d_AngleCCW[sel.AngleCCW];	
		motor_settings.upper_threshold = Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];			//ת�ر�����������																					//ת�ر�����������
		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������		
		update_settings(&motor_settings);
		start();   
		reciprocate_sw = 0;
	}	
	
	if(reciprocate_sw == 3)		
	{
		motor_settings.mode = 0;																								//ATC
		motor_settings.forward_speed =  r_Speed[sel.RecSpeed];																	//��ת�ٶ�
		motor_settings.reverse_speed = -r_Speed[sel.RecSpeed];																	//��ת�ٶ�
		motor_settings.upper_threshold = Torque_limit;																			//ת�ر�����������
		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������		 
		update_settings(&motor_settings);																											//��ת״̬
		start(); 	 
		reciprocate_sw = 0;
	}


	mode = CheckKey_RunMode(mode);					//������ģʽ�µİ���ɨ��
	mode = CheckHardwareFault(mode);
	mode=RunningTurn2Apex(mode);					//������������Ƿ��и����ź�
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e Measure(WorkMode_e mode)
*	����˵��: ���⹤��ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e Measure(WorkMode_e mode)
{
	static int cnt = 20, sw = 0, num = 20, fisrt_run = 1;

	// HAL_Delay(10);
	CalculateApex();							//�������
//	DispApexBar(20-sys.apex);					//��ʾ������	
	if(Disp_TL_limit_cnt > 100)
	{
		Apex_RUN(18-sys.apex);
		Disp_TL_limit_cnt = 0;
			//��������
		switch(sys.apex)
			{
			case 10: {
				ApexRing(40,1);
				break;
			}
			case 11: {
				ApexRing(35,1);
				break;
			}
			case 12: {
				ApexRing(30,1);
				break;
			}
			case 13: {
				ApexRing(25,1);
				break;
			}
			case 14: {
				ApexRing(20,1);
				break;
			}
			case 15: {
				ApexRing(16,1);
				break;
			}
			case 16: {
				ApexRing(13,1);
				break;
			}
			case 17: {
				ApexRing(10,1);
				break;
			}
			case 18: {
				ApexRing(5,1);
				break;
			}
			case 19: {
				ApexRing(7,1);
				break;
			}
			case 20: {
				ApexRing(6,1);
				break;
			}
			default:
				break;
			}
			if(sys.apex<10) ApexRing(0,0);
	}
	
//	Apex_RUN(20-sys.apex);

	
	mode = CheckKey_RunMode(mode);
	mode = CheckHardwareFault(mode);
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
	
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e RunWithApex(WorkMode_e mode)
*	����˵��: ����+����ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e RunWithApex(WorkMode_e mode)
{

	uint8_t i;
	if(Disp_TL_limit_cnt > 100)
	{
		Apex_RUN(18-sys.apex);
		Disp_TL_limit_cnt = 0;
		switch(sys.apex)
			{
			case 10: {
				ApexRing(40,1);
				break;
			}
			case 11: {
				ApexRing(35,1);
				break;
			}
			case 12: {
				ApexRing(30,1);
				break;
			}
			case 13: {
				ApexRing(25,1);
				break;
			}
			case 14: {
				ApexRing(20,1);
				break;
			}
			case 15: {
				ApexRing(16,1);
				break;
			}
			case 16: {
				ApexRing(13,1);
				break;
			}
			case 17: {
				ApexRing(10,1);
				break;
			}
			case 18: {
				ApexRing(5,1);
				break;
			}
			case 19: {
				ApexRing(7,1);
				break;
			}
			case 20: {
				ApexRing(6,1);
				break;
			}
			default:
				break;
			}
			if(sys.apex<10) ApexRing(0,0);

	}
	//������ʼ��
	if((flag_running == 0) && (LVGL_TL_Flag == 1))
	{
		iq_sum = 0;
		Iq_cnt = 0;	
		
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		
		//����Ť�غ��ٶȣ����õ���˵�ѹ
		if((sel.Torque <t30 )&&(sel.Speed<s600))
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_RESET);			//���õ����ѹΪ6V
		}else
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);			//���õ����ѹΪ9V
		}
		
		//Ѱ��Ť������
		if(sel.Torque <=2)
		{
			Torque_limit = S_T_I_COE1[sel.Torque][sel.Speed];
		}else if((sel.Torque>2) &&(sel.Torque <=5))
		{
			Torque_limit = S_T_I_COE2[sel.Torque-3][sel.Speed];	
		}else if((sel.Torque>5) &&(sel.Torque <=11))
		{
			Torque_limit = S_T_I_COE3[sel.Torque-6][sel.Speed];	
		}else if(sel.Torque>11)
		{
			Torque_limit = S_T_I_COE4[sel.Torque-12][sel.Speed];
		}
		LVGL_task_Flag = 1;	
		flag_running = 1;
		delay_ms(200);
		stop();
		
		LOOPTIMER_ENABLE;
			
		delay_ms(100);
		motor_settings.mode = 0;
		
		//���õ��ת����Ť������
		if(sel.OperaMode == 0)																									//��ת
		{
			motor_settings.forward_speed = -d_Speed[sel.Speed];
			motor_settings.reverse_speed =  d_Speed[sel.Speed];	
			motor_settings.upper_threshold =  Torque_limit - NoLoadCurrent[sel.Speed] - NoLoadCurrent[sel.Speed+28];			//ת�ر�����������
//			motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[sel.Speed])/2 - NoLoadCurrent[sel.Speed+28];			//ת��ȡ��������������	
			motor_settings.lower_threshold =  (Torque_limit- NoLoadCurrent[sel.Speed]) /2 + NoLoadCurrent[sel.Speed];			//ת��ȡ��������������	
		}
		else if(sel.OperaMode == 1)																								//��ת
		{
			motor_settings.forward_speed = d_Speed[sel.Speed];
			motor_settings.reverse_speed = -d_Speed[sel.Speed];
			motor_settings.upper_threshold =  Torque_limit;																		//ת�ر�����������
//			motor_settings.lower_threshold =  (Torque_limit- NoLoadCurrent[sel.Speed]) /2 + NoLoadCurrent[sel.Speed];			//ת��ȡ��������������	
			motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[sel.Speed])/2 - NoLoadCurrent[sel.Speed+28];			//ת��ȡ��������������	
		}
		Torque_p = ((float)(Torque_limit - NoLoadCurrent[sel.Speed])) / d_TqBlinkGrid[sel.Torque];
		update_settings(&motor_settings);
		start(); 
	}
	
	//���ϼ��
	if((status_mp6570 != 0x04))														//��⵽MP6570���ڹ��ϲ����ڹ���״̬
	{
		stop();
		delay_ms(10);
		LOOPTIMER_DISABLE;
		mp6570_disable();  																	//disable mp6570
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		delay_ms(100);
		mp6570_enable();
		flag_running = 0;
		LVGL_TL_Flag = 0;
	}else
	{	
		CalculateApex();	
	}
	
	//��ֵ�˲�
	iq_sum += motor_iq;
	Iq_cnt++;
	if(Iq_cnt >= Iq_num)
	{
		iq_motor = iq_sum / Iq_cnt;
		Iq_cnt = 0;
		iq_sum = iq_motor * Iq_num / 2;
		Iq_cnt = Iq_num / 2;
		if(sel.OperaMode == 0)
		{
			if(motor_status.status == Status_FORWARD)
			{
				iq_motor -= NoLoadCurrent[sel.Speed];
			}else 
			{
				iq_motor -= NoLoadCurrent[sel.Speed+28];
			}
		}
		else if(sel.OperaMode == 1)
		{	
			if(motor_status.status == Status_FORWARD)
			{
				
				iq_motor -= NoLoadCurrent[sel.Speed+28];
			}else 
			{
				iq_motor -= NoLoadCurrent[sel.Speed];
			}
		}
		iq_motor = abs(iq_motor);
		Disp_Torque = (int)(((float)iq_motor)/Torque_p);
		Read_ig_flag = 1;
	}
	
	//��������
	if(sys.error == 1)
	{
		mode = Fault_mode;  																	//�������״̬
	}
	
	
//	//Ť�ع����˸����
//	if(Disp_TL_limit_cnt > 1000)
//	{
//		if(Disp_TL_limit_flag)
//		{	
//			Disp_TL_limit_flag = 0;
//		}
//		else
//		{
//			Disp_TL_limit_flag = 1;
//		}
//		Disp_TL_limit_cnt = 0;
//	}
//	//Ť������ʾ
//	if(Read_ig_flag == 1)
//	{
//		Disp_torque_bar_scale(Disp_Torque,Disp_TL_limit_flag);
//		Disp_TL_cnt = 0;
//		Read_ig_flag = 0;
//	}

	MonitorApexAction(mode);
	mode = CheckKey_RunWithApexMode(mode);			//������ģʽ�µİ���ɨ��
	mode = MonitorApexStop(mode);
	mode = CheckHardwareFault(mode);
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
	
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e RecWithApex(WorkMode_e mode)
*	����˵��: ����+����ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e RecWithApex(WorkMode_e mode)
{
	//������ں�����ǰ�棬��ִ�и�����ʾ
	if(Disp_TL_limit_cnt > 100)
	{
		Apex_RUN(18-sys.apex);
		Disp_TL_limit_cnt = 0;
		switch(sys.apex)
			{
			case 10: {
				ApexRing(40,1);
				break;
			}
			case 11: {
				ApexRing(35,1);
				break;
			}
			case 12: {
				ApexRing(30,1);
				break;
			}
			case 13: {
				ApexRing(25,1);
				break;
			}
			case 14: {
				ApexRing(20,1);
				break;
			}
			case 15: {
				ApexRing(16,1);
				break;
			}
			case 16: {
				ApexRing(13,1);
				break;
			}
			case 17: {
				ApexRing(10,1);
				break;
			}
			case 18: {
				ApexRing(5,1);
				break;
			}
			case 19: {
				ApexRing(7,1);
				break;
			}
			case 20: {
				ApexRing(6,1);
				break;
			}
			default:
				break;
			}
			if(sys.apex<10) ApexRing(0,0);
	}
 	if((flag_running == 0) && (LVGL_TL_Flag == 1))
	{	
		MX_SPI1_Init1();
		delay_ms(20);	
		init_MC();

		regi[0x24] =  	KI_Speed1;//800;//0x0E30;			//Speed loop Ki
		regi[0x25] =  	KI_Speed_Gain1;//0x8014;			//Speed loop Ki gain
	  
	  	regi[0x22] =  	KP_Speed1;//800;//0x08D2;			//Speed loop Kp
		regi[0x23] =  	KP_Speed_Gain1;//0x800C;			//Speed loop Kp gain
		
		regi[0x1A] = 	KP_Position1;//1500;//0x04F3;			//position loop Kp
		regi[0x1B] =	KP_Position_Gain1;//0x000E;			//position loop Kp gain
		
		//����Ť�غ��ٶȣ����õ���˵�ѹ
		if((sel.Torque < t30 )&&(r_d_speed[sel.RecSpeed] < s600))
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_RESET);			//���õ����ѹΪ6V
		}else
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);			//���õ����ѹΪ9V
		}
		//Ѱ��Ť������
		if(sel.Torque <=2)
		{
			Torque_limit = S_T_I_COE1[sel.Torque][r_d_speed[sel.RecSpeed]];
		}else if((sel.Torque>2) &&(sel.Torque <=5))
		{
			Torque_limit = S_T_I_COE2[sel.Torque-3][r_d_speed[sel.RecSpeed]];	
		}else if((sel.Torque>5) &&(sel.Torque <=11))
		{
			Torque_limit = S_T_I_COE3[sel.Torque-6][r_d_speed[sel.RecSpeed]];	
		}else if(sel.Torque>11)
		{
			Torque_limit = S_T_I_COE4[sel.Torque-12][r_d_speed[sel.RecSpeed]];
		}
		
		LOOPTIMER_ENABLE;
		flag_running = 1;

		stop();
		delay_ms(200);
		motor_settings.mode = 1; 																						//����ģʽ
		motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed];			  											//����ģʽ�ٶ�����

		motor_settings.forward_position = d_AngleCCW[sel.AngleCW];																						//��ǰת���Ƕ�  ǰתΪ������תΪ��
		motor_settings.reverse_position = d_AngleCW[sel.AngleCCW];																						//���ת���Ƕ�
		
		if( -motor_settings.forward_position >= motor_settings.reverse_position )
		{	
			motor_settings.upper_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28]);			//ת�ر�����������
			motor_settings.lower_threshold = ((Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]]);			//ת��ȡ��������������	
		}else
		{
			motor_settings.upper_threshold = Torque_limit;																									//ת�ر�����������
			motor_settings.lower_threshold = ((Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28]);		//ת��ȡ��������������
		}													//���ת���Ƕ�
		
		update_settings(&motor_settings);
		start();
	}
	
	if(reciprocate_sw == 1)
	{
		delay_ms(100);
		motor_settings.mode = 0;																		//�ٶ�ģʽ
		if( -motor_settings.forward_position >= motor_settings.reverse_position )
		{
			motor_settings.forward_speed = 350;																//��ת�ٶ� -350rpm
			motor_settings.reverse_speed = -350;																//��ת�ٶ� 350rpm
		}else
		{
			motor_settings.forward_speed =-350;																//��ת�ٶ� -350rpm
			motor_settings.reverse_speed = 350;																//��ת�ٶ� 350rpm
		}
		motor_settings.upper_threshold = 1000;															//ת�ر�����������
		motor_settings.lower_threshold = 500;															//ת��ȡ��������������
		update_settings(&motor_settings);
		start(); 
		reciprocate_sw = 0;
	}
	
		//���ϼ��
	if((status_mp6570 != 0x04))														//��⵽MP6570���ڹ��ϲ����ڹ���״̬
	{
		stop();
		delay_ms(10);
		LOOPTIMER_DISABLE;
		mp6570_disable();  																	//disable mp6570
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		regi[0x24] =  KI_Speed1;//800;//0x0E30;			//Speed loop Ki
		regi[0x25] =  KI_Speed_Gain1;//0x8014;			//Speed loop Ki gain
	  
	  	regi[0x22] =  KP_Speed1;//800;//0x08D2;			//Speed loop Kp
		regi[0x23] =  KP_Speed_Gain1;//0x800C;			//Speed loop Kp gain
		
		delay_ms(100);
		mp6570_enable();
		flag_running = 0;
		LVGL_TL_Flag = 0;
	}else
	{	
		CalculateApex();	
	}
	
	MonitorApexAction(mode);
	mode = CheckKey_RunWithApexMode(mode);					//������ģʽ�µİ���ɨ��
	mode = MonitorApexStop(mode);
	mode = CheckHardwareFault(mode);
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e ATCwithApex(WorkMode_e mode)
*	����˵��: ATC+����ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e ATCwithApex(WorkMode_e mode)
{
 
	uint8_t i;
	
	//������ں�����ǰ�棬��ִ�и�����ʾ
	if(Disp_TL_limit_cnt > 100)
	{
		Apex_RUN(18-sys.apex);
		Disp_TL_limit_cnt = 0;
		switch(sys.apex)
			{
			case 10: {
				ApexRing(40,1);
				break;
			}
			case 11: {
				ApexRing(35,1);
				break;
			}
			case 12: {
				ApexRing(30,1);
				break;
			}
			case 13: {
				ApexRing(25,1);
				break;
			}
			case 14: {
				ApexRing(20,1);
				break;
			}
			case 15: {
				ApexRing(16,1);
				break;
			}
			case 16: {
				ApexRing(13,1);
				break;
			}
			case 17: {
				ApexRing(10,1);
				break;
			}
			case 18: {
				ApexRing(5,1);
				break;
			}
			case 19: {
				ApexRing(7,1);
				break;
			}
			case 20: {
				ApexRing(6,1);
				break;
			}
			default:
				break;
			}
			if(sys.apex<10) ApexRing(0,0);
	}
	
	if((flag_running == 0) && (LVGL_TL_Flag == 1))
	{
		iq_sum = 0;
		Iq_cnt = 0;
		
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		
		//����Ť�غ��ٶȣ����õ���˵�ѹ
		if((sel.AtrTorque < t30 )&&(r_d_speed[sel.RecSpeed] < s600))
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_RESET);			//���õ����ѹΪ6V
		}else
		{
			HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);			//���õ����ѹΪ9V
		}
		//Ѱ��Ť������
		if(sel.AtrTorque <=2)
		{
			Torque_limit = S_T_I_COE1[sel.AtrTorque][r_d_speed[sel.RecSpeed]];
		}else if((sel.AtrTorque > 2) &&(sel.AtrTorque <= 5))
		{
			Torque_limit = S_T_I_COE2[sel.AtrTorque-3][r_d_speed[sel.RecSpeed]];	
		}else if((sel.AtrTorque > 5) &&(sel.AtrTorque <= 11))
		{
			Torque_limit = S_T_I_COE3[sel.AtrTorque-6][r_d_speed[sel.RecSpeed]];	
		}else if(sel.AtrTorque > 11)
		{
			Torque_limit = S_T_I_COE4[sel.AtrTorque-12][r_d_speed[sel.RecSpeed]];
		}
		
		LOOPTIMER_ENABLE;
		LVGL_task_Flag = 1;
		flag_running = 1;	 

		stop();
		delay_ms(100);
		motor_settings.mode = 0;																								//ATC

		motor_settings.forward_speed = -r_Speed[sel.RecSpeed];																	//��ת�ٶ�
		motor_settings.reverse_speed = r_Speed[sel.RecSpeed];																	//��ת�ٶ�

		motor_settings.upper_threshold = Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];			//ת�ر�����������
//		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������
		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]];			//ת��ȡ��������������		
		update_settings(&motor_settings);		
		start(); 	
		Torque_p = ((float)(Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])) / d_TqBlinkGrid[sel.AtrTorque];
	}

	//���ϼ��
	if((status_mp6570 != 0x04))														//��⵽MP6570���ڹ��ϲ����ڹ���״̬
	{
		stop();
		delay_ms(10);
		LOOPTIMER_DISABLE;
		mp6570_disable();  																	//disable mp6570
		MX_SPI1_Init();
		delay_ms(20);
		init_MC();
		delay_ms(100);
		mp6570_enable();
		flag_running = 0;
		LVGL_TL_Flag = 0;
	}else
	{	
		CalculateApex();	
	}
	
	//��ֵ�˲�
	iq_sum += motor_iq;
	Iq_cnt++;
	if(Iq_cnt >= Iq_num)
	{
		iq_motor = iq_sum / Iq_cnt;
		Iq_cnt = 0;
		iq_sum = iq_motor * Iq_num / 2;
		Iq_cnt = Iq_num / 2;
		if(motor_status.mode == 0)
		{
			if(motor_status.status == Status_FORWARD)
			{
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]];
			}else 
			{
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];
			}
		}
		else if(motor_status.mode == 1)
		{	
			if(motor_status.status == Status_FORWARD)
			{
				
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];
			}else 
			{
				iq_motor -= NoLoadCurrent[r_d_speed[sel.RecSpeed]];
			}
		}
		iq_motor = abs(iq_motor);
		Disp_Torque = (int)(((float)iq_motor)/Torque_p);
		Read_ig_flag = 1;
	}
	
	//Ť�ع����˸����
//	if(Disp_TL_limit_cnt > 1000)
//	{
//		if(Disp_TL_limit_flag)
//		{	
//			Disp_TL_limit_flag = 0;
//		}
//		else
//		{
//			Disp_TL_limit_flag = 1;
//		}
//		Disp_TL_limit_cnt = 0;
//	}
//	//Ť������ʾ
//	if(Read_ig_flag == 1)
//	{
//		Disp_torque_bar_scale(Disp_Torque,Disp_TL_limit_flag);
//		Disp_TL_cnt = 0;
//		Read_ig_flag = 0;
//	}
//	if(sel_page_flash ==1)
//	{
//		Standy_ON(motor_iq,Disp_TL_limit_flag);
//		sel_page_flash = 0;
//	}
	
	if(reciprocate_sw == 2)		
	{
		stop();
		delay_ms(100);
		motor_settings.mode = 1;
		motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed];	
		
		motor_settings.forward_position = d_AngleCW[sel.AngleCW];
		motor_settings.reverse_position = d_AngleCCW[sel.AngleCCW];	
		motor_settings.upper_threshold = Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];			//ת�ر�����������																					//ת�ر�����������
		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������		
		update_settings(&motor_settings);
		start();   
		reciprocate_sw = 0;
	}	
	
	if(reciprocate_sw == 3)		
	{
		motor_settings.mode = 0;																								//ATC
		motor_settings.forward_speed =  r_Speed[sel.RecSpeed];																	//��ת�ٶ�
		motor_settings.reverse_speed = -r_Speed[sel.RecSpeed];																	//��ת�ٶ�
		motor_settings.upper_threshold = Torque_limit;																			//ת�ر�����������
		motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������		 
		update_settings(&motor_settings);																											//��ת״̬
		start(); 	 
		reciprocate_sw = 0;
	}

	MonitorApexAction(mode);
	mode = CheckKey_RunWithApexMode(mode);					//������ģʽ�µİ���ɨ��
	mode =MonitorApexStop(mode);
	mode = CheckHardwareFault(mode);
	Turn2Standby(mode);								//����Standbyǰ�Բ�������
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e Standby(WorkMode_e mode)
*	����˵��: ����ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e Standby(WorkMode_e mode)
{
	static uint32_t Apex_Start = 0;
	if(sys.FlagThetaBias == 1)
	{
		sys.FlagThetaBias = 0;
		stop();
		delay_ms(100);
		MP6570_AutoThetaBias(0x00, 100,2000); //�ҳ�ʼ�Ƕ�
		delay_ms(100);
		LOOPTIMER_DISABLE;
	}
	stop();
	delay_ms(10);
	flag_running = 0;
	LVGL_TL_Flag = 0;
	LOOPTIMER_DISABLE;
	IQ_Cnt = 0;

	regi[0x24] =  KI_Speed;							//800;//0x0E30;			//Speed loop Ki
	regi[0x25] =  KI_Speed_Gain;					//0x8014;			//Speed loop Ki gain
	regi[0x22] =  KP_Speed;							//800;//0x08D2;			//Speed loop Kp
	regi[0x23] =  KP_Speed_Gain;					//0x800C;			//Speed loop Kp gain	
	regi[0x1A] =  KP_Position;						//1500;//0x04F3;		//position loop Kp
	regi[0x1B] =  KP_Position_Gain;					//0x000E;		//position loop Kp gain
//	ResLast_apex_x();
//	ResLast_x();
	CalculateApex();								//ʵʱ��ؼ������
	if(Apex_Start > 300)							//������ʱ��Ԥ������������
	{
		mode = MonitorApexStart(mode);
	}else
	{
		Apex_Start++;
	}
	mode = CheckKey_StandbyMode(mode);
 
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e Charging(WorkMode_e mode)
*	����˵��: ���ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e Charging(WorkMode_e mode)
{
	static int not_charge_cnt = 0, bg_off_cnt = 0, charge_bar_cover_len = 0, full_charge_cnt = 0;
    uint8_t i = 0;
    uint8_t IsKey;
    Key_e Key;
    IsKey = 1;
	
//    ClearClock();
	Time_cnt = 0;
    
//		if(ChargDisp_cnt > 100){
//			ChargDisp_cnt = 0;
//			ChargDisp_cnt_100++;
//		}
	if(ChargDisp_cnt > 10)
	{
		ChargDisp_cnt = 0;
		
		//��������
		if(bg_off_cnt < 100)
		{
			bg_off_cnt++;
		}
		else
		{
			TIM2->CCR1 = 30;
		}
		
		//����������
		if(sys.FullCharge == 1)
		{
			full_charge_cnt++;
		}
		else
		{
			full_charge_cnt = 0;
		}
		
		//���������
		if(full_charge_cnt > 1)
		{
			charge_bar_cover_len = 52;
		}
		else
		{
			if(charge_bar_cover_len <= 52)
			{
				charge_bar_cover_len++;
			}
			else
			{
				charge_bar_cover_len = 13 * sys.BatteryLevel;
			}
		}
		lv_disp_charge_init(charge_bar_cover_len);
	}
	
	if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==0)
	{
		not_charge_cnt++;
	}
	else
	{ 
		not_charge_cnt=0;
	}
		
    Key = Get_KeyVaule();
    if(Key != none)
	{
		Ring.short_ring = 1;
		not_charge_cnt = 2;
	}
		
	if(not_charge_cnt > 1)
	{
		TIM2->CCR1 = d_backlight[set.backlight];
		not_charge_cnt = 0;
		mode = Standby_mode;
		NeedFlash = 1;					//֪ͨ�˵����������ʾ����
		sel_page_flash = 1;
		bg_off_cnt = 0;
		Turn2Standby(mode);
    }
 
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e Fault(WorkMode_e mode)
*	����˵��: ���д���ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e Fault(WorkMode_e mode)
{
	if(sys.error==1) 
	{
		stop();
		delay_ms(10);
		flag_running = 0;
		LVGL_TL_Flag = 0;
		LOOPTIMER_DISABLE;
		
		lv_Error_init(sys.error_num);
		sys.error=0;				//���ϱ�־ ���
        sys.error_num=0;			//���ϴ����־ ���
	}
	mode=CheckKey_RunMode(mode);
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e PowerOff(WorkMode_e mode)
*	����˵��: �ػ�Ԥ��ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e PowerOff(WorkMode_e mode)
{
	int i = 0;
	ApexRing(0,0);	
	Ring.short_ring = 1;
	HAL_Delay(100);
	Ring.short_ring = 1;
//	flag_bl_off = 1;
	 SaveLastMem();		//�����ϴεĳ����
	//save to eeprom
	//stop motor
	// beep
	for(i = 0; i < d_backlight[set.backlight]; i++)
	{
		TIM2->CCR1--;
		delay_ms(1);
	}

	HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
	HAL_Delay(500);
	HAL_NVIC_SystemReset();
 
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e DispOff(WorkMode_e mode)
*	����˵��: �ر���ʾģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: WorkMode_e ����ģʽ
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e DispOff(WorkMode_e mode)
{
 
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e Displaylogo(WorkMode_e mode)
*	����˵��: ��ʾLOGOģʽ��Ȼ����������ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20210813         
**********************************************************************************************************/
WorkMode_e Displaylogo(WorkMode_e mode)
{
	static uint16_t cnt = 0;
	cnt++;
	delay_ms(1);
	Save1_Read0_Torque(0);
	HAL_Delay(50);
	Save1_Read0_NoloadTor(0);
	HAL_Delay(50);
	HAL_IWDG_Refresh(&hiwdg);				//ι�� 2s���	
	Get_KeyVaule();							//�ն��������ֵ
	
//	if(cnt > 10)
//	{
//		cnt = 0;
		mode = Standby_mode;
		NeedFlash = 1;		
//	}	
	return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e RunningTurn2Apex(WorkMode_e mode)
*	����˵��: ����������ģʽ��⵽�����źţ�ת�������߲�ģʽ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211112         
**********************************************************************************************************/
WorkMode_e RunningTurn2Apex(WorkMode_e mode)
{
    if(sys.apex > AUTO_START_POS && sel.ApicalMode<3)		//��⵽��������⣬���������߲�ģʽ, & �����⶯�����ر�	��
    {
        switch(sel.OperaMode)
        {
        case 0: {
            mode=RunWithApex_mode;
            break;
        }
        case 1: {
            mode=RunWithApex_mode;
            break;
        }
        case 2: {
            mode=RecWithApex_mode;
            break;
        }
        case 3: {
            mode=ATRwithApex_mode;
            break;
        }
        default :
            break;
        }
        Last_apex_pos=24;				//��ʼ��������λ��Ϊ0
        sys.FlagRunWithApex=1;
    }
    return mode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e MonitorApexStart(WorkMode_e mode)
*	����˵��: �����߲���ӳ���,������ڰ���ɨ�躯������
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
WorkMode_e MonitorApexStart(WorkMode_e mode)
{
    gTempMode = mode;
    if(sel.AutoStart==1 && KeyFuncIndex==0)					//�����Զ������������������п����� ��ǰ���ڴ�������
    {
        if(		(sys.apex > AUTO_START_POS) 				//�������
			&& 	(sys.FlagRunWithApex == 0) 					//&&֮ǰû����
			&& 	(sys.ActiveShutRunning == 0))				//�����ػ�״̬���
        {
            sys.FlagRunWithApex = 1;						//֪ͨϵͳĿǰ����ApexRunģʽ����
            Last_apex_pos = 24;								//��ʼ��������λ��Ϊ0
            if(sel.OperaMode == 0)		
			{
				gTempMode = RunWithApex_mode;
			}
            else if(sel.OperaMode == 1)	
			{
				gTempMode = RunWithApex_mode;
			}
            else if(sel.OperaMode == 2)  	
			{
				gTempMode = RecWithApex_mode;
			}
            else if(sel.OperaMode == 3) 
			{
				gTempMode = ATRwithApex_mode;
            }
            else if(sel.OperaMode == 4)
			{
				gTempMode = Measure_mode;
			}
            Ring.short_ring = 1;
			Jump2Menu(1);
			Disp_TL_limit_cnt = 110;				//�˴�����100��ʱΪ�˱����߲⣬�����ִ�и�����ʾ
//			NextOperate(1);
        }
        if(sys.apex <= AUTO_START_POS)		//����״̬ && ������С��������
        {
            sys.ActiveShutRunning = 0;
        }
    }

    return gTempMode;
}

/**********************************************************************************************************
*	�� �� ��: WorkMode_e MonitorApexStop(WorkMode_e mode)
*	����˵��: �����߲���ӳ���,�뿪���ܣ��Զ�ֹͣ
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
WorkMode_e MonitorApexStop(WorkMode_e mode)
{
    static uint8_t Autostop_cnt = 0;
    gTempMode = mode;
    if(sel.AutoStop==1)
    {
        if(		(sys.apex <= AUTO_START_POS) 	// �뿪����
			&& 	(sys.FlagRunWithApex == 1))		// &&֮ǰ��������
        {
            Autostop_cnt++;						// delay
            if(Autostop_cnt > 3) 
			{
                Autostop_cnt = 0;
                sys.FlagRunWithApex = 0;
                gTempMode = Standby_mode;			//�ص�����ģʽ,����ģʽ�л�ֹͣ���ת��
                NeedFlash = 1;
				sel_page_flash = 1;
				Jump2Menu(0);
            }
        }
    }

    return gTempMode;
}

/**********************************************************************************************************
*	�� �� ��: void MonitorApexAction(WorkMode_e mode)
*	����˵��: �����߲���ӳ���,�쵽�趨�����ʱ��ѡ����ε������״̬
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
void MonitorApexAction(WorkMode_e mode)
{
    gTempMode=mode;

    switch(sel.ApicalMode)
    {
    case 0: { //Reverse
        if(sys.apex>=(18-sel.FlashBarPosition) && ap_flag==0)
        {
            ap_flag=1;
			sys.over_apex_pos_flag = 1;
            if(sel.OperaMode==0)
			{
				Motor2Reverse();  				//���ݲ���ģʽ��ѡ����
			}
            if(sel.OperaMode==1)
			{
				Motor2Forward();
			}
            if(sel.OperaMode==2) 	
			{
				Motor2INREC();   		//���ݲ���ģʽ��ѡ����
			}
            if(sel.OperaMode==3) 
			{
				Motor2Reverse();
			}
            HAL_Delay(500);
        }
        if(ap_flag==1 && sys.apex<(16-sel.FlashBarPosition))
        {
            if(sel.OperaMode==0)
			{
				Motor2Forward();					//���ݲ���ģʽ��ѡ����
			}
            if(sel.OperaMode==1)
			{
				Motor2Reverse();
			}
            if(sel.OperaMode==2) 	
			{
				Motor2OUTREC();  					//���ݲ���ģʽ��ѡ����
			}
            if(sel.OperaMode==3)
			{
				Motor2Forward();
			}
            ap_flag = 0;
			sys.over_apex_pos_flag = 0;
        }

        break;
    }
    case 1: {														//slowdown
        if(sys.apex>=(17-sel.FlashBarPosition) && ap_flag==0)		//�ڸ���λ������ֵ��ǰһ��λ�ã�����
        {
            ap_flag=1;
            sys.ApicalSlowing=1;									//֪ͨPID ��������
			if(sel.OperaMode==0)
			{
				motor_settings.forward_speed = d_Speed[sel.Speed]/2;
				motor_settings.reverse_speed = -d_Speed[sel.Speed]/2;
			}
			if(sel.OperaMode==1)
			{
				motor_settings.forward_speed = -d_Speed[sel.Speed]/2;
				motor_settings.reverse_speed = d_Speed[sel.Speed]/2;	
			}
			if(sel.OperaMode==2)
			{
				motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed]/2;			//����ģʽ�ٶ�����
			}
			if(sel.OperaMode==4)
			{
				motor_settings.forward_speed =  r_Speed[sel.RecSpeed]/2;																	//��ת�ٶ�
				motor_settings.reverse_speed = -r_Speed[sel.RecSpeed]/2;																	//��ת�ٶ�
			}
			update_settings(&motor_settings);			
        }
        if(ap_flag==1 && sys.apex<(16-sel.FlashBarPosition))
        {

            ap_flag=0;
            sys.ApicalSlowing=0;									//֪ͨpid���������ģʽ���ָ��ٶ�
			if(sel.OperaMode==0)
			{
				motor_settings.forward_speed = d_Speed[sel.Speed];
				motor_settings.reverse_speed = -d_Speed[sel.Speed];
			}
			if(sel.OperaMode==1)
			{
				motor_settings.forward_speed = -d_Speed[sel.Speed];
				motor_settings.reverse_speed = d_Speed[sel.Speed];	
			}
			if(sel.OperaMode==2)
			{
				motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed];			//����ģʽ�ٶ�����
			}
			if(sel.OperaMode==4)
			{
				motor_settings.forward_speed =  r_Speed[sel.RecSpeed];																	//��ת�ٶ�
				motor_settings.reverse_speed = -r_Speed[sel.RecSpeed];																	//��ת�ٶ�
			}
			update_settings(&motor_settings);
        }

        break;
    }
    case 2: { //stop

        if(sys.apex>(18-sel.FlashBarPosition) && ap_flag==0)
        {
            ap_flag=1;
			stop();
			sys.ProgramMotorStop=1;										//����ֹͣ�����λ
            sys.InApicalOff = 1;											//֪ͨ�����򣬲��ٸ����������״̬
            HAL_Delay(100);
        }
        if(ap_flag==1 && sys.apex<(16-sel.FlashBarPosition))
        {
            sys.InApicalOff = 0;												//���
            ap_flag=0;
			start(); 
        }

        break;
    }
    case 3: {
        //do noting
        break;
    }		//off
    default:
        break;
    }

}

/**********************************************************************************************************
*	�� �� ��: void Turn2Standby(WorkMode_e mode)
*	����˵��: ����Standbyǰ�Բ�������
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
void Turn2Standby(WorkMode_e mode)
{
    if(mode==Standby_mode)
    {
        ApexRing(0,0);							//���ش�������󣬹رշ�ת��ʾ����
//		ClearMove();							//����ƶ�����־
//        ClearClock();
		
		
		stop();
		delay_ms(10);
//		LOOPTIMER_DISABLE;
//		if(sel.OperaMode == 2)
//		{
//			mp6570_disable();  																	//disable mp6570
//			MX_SPI1_Init();
//			delay_ms(20);
//			init_MC();
//			delay_ms(100);
//			mp6570_enable();
//		}
		flag_running = 0;
		LVGL_TL_Flag = 0;
		
		motor_status.reach_torque = 0;
		reach_upper_times = 0;
		reach_lower_times = 0;
		
		ap_flag=0;
		sys.over_apex_pos_flag = 0;
        sys.FlagRunWithApex = 0;       
        sys.ApicalSlowing = 0;					//�رռ���
		sys.InApicalOff = 0;					//����Զ�ֹͣ
		sys.RunEALflag = 0;		
		LVGL_task_Flag = 0;
		IQ_Cnt = 0;
    }		
}
/**********************************************************************************************************
*	�� �� ��: Motor2Reverse()
*	����˵��: ����Standbyǰ�Բ�������
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
void Motor2Reverse()
{
	stop();
	delay_ms(100);
	motor_settings.mode = 0;
	motor_settings.forward_speed = d_Speed[sel.Speed];
	motor_settings.reverse_speed = -d_Speed[sel.Speed];
	update_settings(&motor_settings);
	start(); 
}

/**********************************************************************************************************
*	�� �� ��: void Motor2Forward()
*	����˵��: ����Standbyǰ�Բ�������
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
void Motor2Forward()
{
	stop();
	delay_ms(100);
	motor_settings.mode = 0;
	motor_settings.forward_speed = -d_Speed[sel.Speed];
	motor_settings.reverse_speed = d_Speed[sel.Speed];
	update_settings(&motor_settings);
	start(); 
}

/**********************************************************************************************************
*	�� �� ��: void Motor2Forward()
*	����˵��: ����Standbyǰ�Բ�������
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
void Motor2INREC()
{
	stop();
	delay_ms(100);
	if(sel.AngleCW >= sel.AngleCCW)								
	{
		motor_settings.forward_speed = 350;									//��ת�ٶ� -350rpm
		motor_settings.reverse_speed = -350;									//��ת�ٶ� 350rpm
	}
	else if(sel.AngleCW < sel.AngleCCW)
	{
		motor_settings.forward_speed = -350;									//��ת�ٶ� -350rpm
		motor_settings.reverse_speed = 350;									//��ת�ٶ� 350rpm
	}
	motor_settings.mode = 0;
	update_settings(&motor_settings);
	start(); 
}

/**********************************************************************************************************
*	�� �� ��: void Motor2Forward()
*	����˵��: ����Standbyǰ�Բ�������
*	��    ��: WorkMode_e mode����ģʽ
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1��
*	�༭����: 1: 20211119         
**********************************************************************************************************/
void Motor2OUTREC()
{
	stop();
	delay_ms(200);
	motor_settings.mode = 1; 																						//����ģʽ
	motor_settings.toggle_mode_speed = r_Speed[sel.RecSpeed];			  											//����ģʽ�ٶ�����

	motor_settings.upper_threshold = Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]] - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];			//ת�ر�����������
	motor_settings.lower_threshold = (Torque_limit - NoLoadCurrent[r_d_speed[sel.RecSpeed]])/2 - NoLoadCurrent[r_d_speed[sel.RecSpeed]+28];		//ת��ȡ��������������		

	motor_settings.forward_position = d_AngleCW[sel.AngleCW];														//��ǰת���Ƕ�
	motor_settings.reverse_position = d_AngleCCW[sel.AngleCCW];														//���ת���Ƕ�
	
	update_settings(&motor_settings);
	start();
}




















