#include "MenuFunc.h"
#include "Menu.h"
#include "MenuData.h"
#include "MenuDisp.h"
#include "datatype.h"
#include "lcd.h"
#include "DataStorage.h"
#include "key.h"
#include "beep.h"
#include "pid.h"
#include "eeprom.h"
#include "control.h"
#include "mp6570.h"
#include "customer_control.h"
#include "macros.h"
#include "key_Task.h"
#include "lvgl/lvgl.h"
#include "presetting.h"
#include "Multiple_Styles.h"
#include "spi.h"
#include "delay.h"
#include "MP6570_SPI.h"

uint8_t speed_cnt;
uint8_t Torque_cnt;
CAL_MODE_e Cal_mode_flag = Input_Password_flag;
extern uint8_t Motor_Error_flag;
extern uint16_t Round_Cnt;
extern uint32_t samH_RootOutPos;
extern uint32_t samH_RootInPosTemp;
extern IWDG_HandleTypeDef hiwdg;
uint8_t TqStoreCheck[2]; 			//EEPROM����Ť�ر�־�洢
uint8_t CheckPassword();
void MP6570_CalThetaBias();
uint8_t Speed_limit;
uint8_t Error_CNT = 0;

/**********************************************************************************************************
*	�� �� ��: void FactoryTorqueCal()
*	����˵��: ���ǽ�����У��������У������Ȼ�޷�����������
			  ����Ť��У׼�����ǿ��ص������ԣ���Ť�������£��������ٶ�ɨһ�顣�����ǹ̶�Ť��ɨ�ٶȵķ�ʽ������Ť�ض�Ӧ���ٶȶ�ɨһ�顣
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20211111         
**********************************************************************************************************/
void FactoryTorqueCal()
{
	int i = 0, j = 0, cnt = 0, init_j = 0, k;
	int cur[10] = {0};
	long ave = 0;
	uint8_t RootOut[2] = {0};
	uint8_t ctlloop = 1;
	uint32_t samH_sum = 0;
	uint8_t root_flag[1] = {0x1A};
	uint8_t thing = 0, dev_ave = 30, fistst = 0;
	
	Cal_mode_flag = Input_Password_flag;
	
	HAL_GPIO_WritePin(VM_CTL_GPIO_Port,VM_CTL_Pin,GPIO_PIN_SET);
	
	Disp_cal();
	delay_ms(50);
	lv_task_handler();
	
	thing = CheckPassword();	
	if(	thing == 0)
	{
		delay_ms(50);
		Disp_cal();
		lv_task_handler();
		delay_ms(800);
		HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
	}
	else if(thing == 1)
	{
		MP6570_CalThetaBias();	
//		delay_ms(300);
//		HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
	}
	else if(thing == 2)
	{
		Save1_Read0_Torque(0);													//��ȡ֮ǰУ׼��Ť�ص��ڴ�
		delay_ms(500);
		/********************/
		HAL_IWDG_Refresh(&hiwdg);											//ι��  //2s���*********************************
		delay_ms(500);
		Disp_cal();
		delay_ms(50);
		lv_task_handler();
		ctlloop = 1;
		while(ctlloop)
		{
			uint8_t IsKey;
			Key_e Key;
			IsKey = 1;
			/********************/
			HAL_IWDG_Refresh(&hiwdg);																//ι��  //2s���*********************************
			Key = Get_KeyVaule();

			switch(Key) 
			{
				case key_on: { 																		//������,�ҳ��µĲ˵�״̬���
								Ring.short_ring=1;
//								MX_SPI1_Init();					
								Cal_mode_flag = Cal_Noload_Run_flag ;								// ������ص�������ģʽ
//								delay_ms(200);
//								//���õ������
								init_MC();
								regi[0x24] =  	KI_Speed2;//800;//0x0E30;			//Speed loop Ki
								regi[0x25] =  	KI_Speed_Gain2;//0x8014;			//Speed loop Ki gain
							  
								regi[0x22] =  	KP_Speed2;//800;//0x08D2;			//Speed loop Kp
								regi[0x23] =  	KP_Speed_Gain1;//0x800C;			//Speed loop Kp gain
								
								regi[0x1A] = 	KP_Position2;//1500;//0x04F3;			//position loop Kp
								regi[0x1B] =	KP_Position_Gain2;//0x000E;			//position loop Kp gain
//								stop();
								delay_ms(200);
								LOOPTIMER_ENABLE;
								sel.Speed = s50;								      				//����У׼ʱ���ٶ�����
								motor_settings.mode = 0;
								motor_settings.forward_speed = d_Speed[sel.Speed];
								motor_settings.upper_threshold = 5000; 								//����һ���Ƚϴ��ֵ��ʹ����ת
								motor_settings.lower_threshold = 2500;
								update_settings(&motor_settings);
								Round_Cnt = 0;
								start(); 															//�������
								
								//�����ٶ�ѭ��
								for(i = 0; i < 32; i++)
								{
									speed_cnt = i;
									if(Motor_Error_flag == 1)
									{
										i = 0;
										Round_Cnt = 0;
										start();
										Motor_Error_flag = 0;
									}
									motor_settings.forward_speed = d_Speed[i];
									update_settings(&motor_settings);
									
									//��ʾ����ٶ�
									Disp_cal();
									delay_ms(50);
									lv_task_handler();
							
									delay_ms(500);

									/********************/
									HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
									for(cnt = 0; cnt < 30; cnt++)
									{
										ave += motor_iq;  											//�ۼƵ���
										delay_ms(20);
									}
									
									/********************/	
									HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
									ave = ave / 30;
									 
									NoLoadCurrent[i] = ave;											//���ص���ֵ
									ave = 0;
								}
								
								/**************************ֹͣ������л�ת��********************************/
								stop();
								delay_ms(500);
								HAL_IWDG_Refresh(&hiwdg);											//ι��  //2s���*********************************
								delay_ms(500);
								HAL_IWDG_Refresh(&hiwdg);											//ι��  //2s���*********************************
								sel.Speed = s50;								      				//����У׼ʱ���ٶ�����
								motor_settings.mode = 0;
								motor_settings.forward_speed = -d_Speed[sel.Speed];
								motor_settings.upper_threshold = 5000; 								//����һ���Ƚϴ��ֵ��ʹ����ת
								motor_settings.lower_threshold = 2500;
								update_settings(&motor_settings);
								Round_Cnt = 0;
								start(); 
								
								for(i = 32; i < 64; i++)
								{
									if(Motor_Error_flag == 1)
									{
										i = 32;
										Round_Cnt = 0;
										start();
										Motor_Error_flag = 0;
									}
									speed_cnt = i;
									motor_settings.forward_speed = -d_Speed[i-32];
									update_settings(&motor_settings);
									
									Disp_cal();
									delay_ms(50);
									lv_task_handler();
							
									delay_ms(500);

									/********************/
									HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
									for(cnt = 0; cnt < 30; cnt++)
									{
										ave += motor_iq;  											//�ۼƵ���
										delay_ms(20);
									}
									
									/********************/	
									HAL_IWDG_Refresh(&hiwdg);										//ι��  //2s���*********************************
									ave = ave / 30;
									 
									NoLoadCurrent[i] = ave;											//���ص���ֵ
									ave = 0;
								}
								stop();
								Save1_Read0_NoloadTor(1);											//save
								ctlloop = 0;
								Cal_mode_flag = Cal_Torque_flag;									// ����ת�ز���ģʽ
								delay_ms(500);
								break;
							}
			case key_sel: 	{ 																		//���¡�S�����ػ�
								Ring.short_ring = 1;
								delay_ms(100);
								Ring.short_ring = 1;
								delay_ms(200);
								HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
								break;
							}
			case key_up: 	{ 																// ѡ��Ť��+
								Ring.short_ring=1;
								stop();
								init_MC();
								regi[0x24] =  	KI_Speed2;//800;//0x0E30;			//Speed loop Ki
								regi[0x25] =  	KI_Speed_Gain2;//0x8014;			//Speed loop Ki gain
							  
								regi[0x22] =  	KP_Speed2;//800;//0x08D2;			//Speed loop Kp
								regi[0x23] =  	KP_Speed_Gain1;//0x800C;			//Speed loop Kp gain
								
								regi[0x1A] = 	KP_Position2;//1500;//0x04F3;			//position loop Kp
								regi[0x1B] =	KP_Position_Gain2;//0x000E;			//position loop Kp gain
//								stop();
								delay_ms(200);
								LOOPTIMER_ENABLE;
								ctlloop = 0;
								Cal_mode_flag = Cal_Torque_flag;									// ����ת�ز���ģʽ
								delay_ms(500);

								break;
							}
	//---------------------------------------------------------------------------
			default: 																				//��������Ĵ���
								IsKey = 0;
								break;
			}
		}
		
		/**************************��ʾת�ز���ģʽ********************************/
		Disp_cal();
		delay_ms(50);
		lv_task_handler();
		
		/**************************���¿��ص�����Ϊת�ز�����׼��********************************/
		Save1_Read0_NoloadTor(0);																	// ��ȡ���ص���
		
		delay_ms(200);
		ctlloop = 1;
		while(ctlloop)
		{
			Key_e Key;
			/********************/
			HAL_IWDG_Refresh(&hiwdg);																//ι��  //2s���*********************************
			Key = Get_KeyVaule();

			switch(Key) 
			{
				case key_on: 	{ 																	//������,�ҳ��µĲ˵�״̬���
									Ring.short_ring=1;
									ctlloop=0;
									Cal_mode_flag = Cal_Torque_Run_flag;							// ת�ز���ģʽ����
									//  start();
									break;
								}
				case key_sel: 	{ 																	//sel esc
									Ring.short_ring = 1;
									delay_ms(100);
									Ring.short_ring = 1;
									delay_ms(200);
									HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
									break;
								}
	//---------------------------------------------------------------------------
				default: 																			//��������Ĵ���

								break;
			}
		}
		ctlloop = 1;
		for(j = 0; j <= 13; j++)
		{
			Torque_cnt = j;
			
			Disp_cal();
			delay_ms(50);
			lv_task_handler();
			while(ctlloop)
			{
				uint8_t IsKey;
				Key_e Key;
				IsKey = 1;
				/********************/
				HAL_IWDG_Refresh(&hiwdg);															//ι��  //2s���*********************************
				Key = Get_KeyVaule();
				
				Disp_cal();
				delay_ms(50);
				lv_task_handler();
				
				switch(Key) 
				{
					case key_on: 	{ 																//������,�ҳ��µĲ˵�״̬���
										Ring.short_ring = 1;
//										MX_SPI1_Init();
//										reset_parameters1();
										delay_ms(100);
										ctlloop = 0;
//										init_MC();
//										stop();
//										delay_ms(200);
//										LOOPTIMER_ENABLE;
										sel.Speed = s50;											//����У׼ʱ���ٶ�����
										motor_settings.mode = 0;
										motor_settings.forward_speed = d_Speed[sel.Speed];
										motor_settings.upper_threshold = 5000; 						//����һ���Ƚϴ��ֵ��ʹ����ת
										motor_settings.lower_threshold = 2500;
										update_settings(&motor_settings);
										Round_Cnt = 0;
										start();
										Cal_mode_flag = Cal_Torque_Run1_flag;							// ת�ز���ģʽ����
										delay_ms(200);
										
										break;
									}
					case key_sel: 	{ 																//sel esc
										Ring.short_ring = 1;
										delay_ms(100);
										ctlloop = 0;
										j=14;														//����ѭ�����ڣ�ǿ������

										break;
									}
					case key_up: 	{ 																// ѡ��Ť��+
										Ring.short_ring=1;
										j++;
										if(j > 13)
										{
											j = 0;
										}
										Torque_cnt = j;

										break;
									}
					case key_down: 	{ 																// ѡ��Ť��-
										Ring.short_ring = 1;
										j--;
										if(j < 0)
										{
											j = 13;
										}
										Torque_cnt = j;
					
										break;
									}
		//---------------------------------------------------------------------------
					default: 																		//��������Ĵ���
									IsKey = 0;
									break;
				}
			}
			
			/**************************����ת�أ�ȷ��ת������********************************/
			if(Torque_cnt <= t10)
			{
				Speed_limit = s1500 + 1;															//Ť��У׼��0  ~ 1.0��Ť�أ�ȫ�ٶ�
			}
			else if((Torque_cnt > t10)&&(Torque_cnt <= t20))
			{
				Speed_limit = s1200 + 1;															//Ť��У׼��1.0~ 2.0��Ť�أ���ȥ1200rpm����
			}else if((Torque_cnt > t20)&&(Torque_cnt <= t40))
			{
				Speed_limit = s700 + 1;																//Ť��У׼��2.0~ 4.0��Ť�أ���ȥ700rpm����
			}else if(Torque_cnt > t40)
			{
				Speed_limit = s400 + 1;																//Ť��У׼��	 5.0��Ť�أ���ȥ400rpm����
			}
			
			for(i = 0; i < Speed_limit; i++)
			{
				if(Motor_Error_flag == 1)
				{
					i = 0;
					stop();
					HAL_IWDG_Refresh(&hiwdg);
					delay_ms(300);
					motor_settings.forward_speed = d_Speed[i];
					update_settings(&motor_settings);
					Round_Cnt = 0;
					start();
					Motor_Error_flag = 0;
				}			
				speed_cnt = i;
				Disp_cal();
				delay_ms(50);
				lv_task_handler();
				HAL_IWDG_Refresh(&hiwdg);
				delay_ms(300);
				motor_settings.forward_speed = d_Speed[i];
				update_settings(&motor_settings);
					
				/********************/
				HAL_IWDG_Refresh(&hiwdg);															//ι��  //2s���*********************************
				delay_ms(300);

				for(cnt=0; cnt < 10; cnt++)
				{		 
					//	ave+=(motor_iq+NoLoadCurrent[i]);
					if((motor_iq > NoLoadCurrent[i])&&(motor_iq < 500))													//ֻ�ж�ȡ�����ݴ��ڿ��ص���������Ч
					{  
						ave += motor_iq;
					}
					else{
						ave += 0;																	//��һ��������Ч��������ƽ��ֵ����
						if(cnt >= 1)
						{
							cnt--;
						}
						dev_ave --;
					}
					delay_ms(10);
				}
				
				if(dev_ave <= 6)																	//��ʧ���ݹ��࣬�����˳�
				{
					HAL_IWDG_Refresh(&hiwdg);														//ι��  //2s���*********************************
					stop();
					Ring.long_ring = 1;
					delay_ms(1500);
					HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);				//POWER OFF
				}
				/********************/	
				HAL_IWDG_Refresh(&hiwdg);															//ι��  //2s���*********************************
				ave = ave / 10;
				if(Torque_cnt <= 2)
				{
					S_T_I_COE1[Torque_cnt][i] = ave;												//Ť��У׼��0  ~ 1.0��Ť�أ�ȫ�ٶ�
				}
				else if((Torque_cnt > 2)&&(Torque_cnt <= 5))
				{
					S_T_I_COE2[Torque_cnt-3][i] = ave;												//Ť��У׼��1.0~ 2.0��Ť�أ���ȥ1500rpm
				}else if((Torque_cnt > 5)&&(Torque_cnt <= 11))
				{
					S_T_I_COE3[Torque_cnt-6][i] = ave;												//Ť��У׼��2.0~ 4.0��Ť�أ���ȥ700rpm����
				}else if(Torque_cnt > 11)
				{
					S_T_I_COE4[Torque_cnt-12][i] = ave;												//Ť��У׼��	 5.0��Ť�أ���ȥ400rpm����
				}
				ave = 0;
				dev_ave = 30;																		//�����ָ�Ϊ10
			}
			Cal_mode_flag = Cal_Torque_Run_flag;							// ת�ز���ģʽ����
			
			Disp_cal();
			delay_ms(50);
			lv_task_handler();	
			
			ave = 0;

			stop();
			delay_ms(100);
//			LOOPTIMER_DISABLE;
			ctlloop = 1;
		}
		stop();
		Cal_mode_flag = Cal_Saving;
		Disp_cal();
		delay_ms(50);
		LOOPTIMER_DISABLE;
		lv_task_handler();	

		/*************���Ը��⿪·ֵ ****************/
	//    for(i=0; i<10; i++) {
	//        if(flag.sam_finish==1) {
	//            flag.sam_finish=0;

	//            samH_sum+=samh.VPP;
	//            delay_ms(50);
	//        }
	//    }
		/*	samH_RootOutPos=samH_sum/10;		//���㿪·ֵ
			RootOut[0]=samH_RootOutPos>>8;
				RootOut[1]=samH_RootOutPos&0x00ff;
			root_flag[0]=0x1A;
			 EEPROM_Write(ROOTOUTPOS_ADDR,RootOut,2);		// д�뿪·ֵ ,�ȸ�λ���λ
		   EEPROM_Write(ROOTFLAG_ADDR,root_flag,1);				//д����±�־
			samH_RootInPos=(samH_RootInPosTemp+samH_RootOutPos)/2;		//���¼�����ܽ��뷧ֵ
			disp.Gram_Clear(BLACK);
			while(ctlloop)
			{
				  Key_e Key;
				 CalculateApex();			//�������ֵ
				 DebugApexShowData();		//��ʾ��������
				 Key = Get_KeyVaule();
				 if(Key!=none)
				 {
					Ring.short_ring=1;
					 ctlloop=0;		//�а������£��˳�ѭ��
				 }
				 delay_ms(50);
		HAL_IWDG_Refresh(&hiwdg);		//ι��  //2s���
			}*/
	//    disp.Gram_Clear(GRAY);
	//    disp.String(20,20,"Saving...",24,B,BLACK,GRAY );
	//    disp.RefreshGram();
//		LCD_Clear(WHITE);

		/************д����±�־λ*****************/
		TqStoreCheck[0]=0XA8;
		TqStoreCheck[1]=0XA9;
		EEPROM_Write(TQFLAG_ADDR,TqStoreCheck,2);		//д����±�־
		/*********************************************/
	  //  SaveCalTorque();								//�洢У׼��Ť��ֵ
		
		Save1_Read0_Torque(1);
		/*************��У��******************/

		Cal_mode_flag = Cal_Fishing;
		Disp_cal();
		delay_ms(50);
		lv_task_handler();		
		/********************/

		/********************/
		delay_ms(500);
		HAL_IWDG_Refresh(&hiwdg);		//ι��  //2s���*********************************
		delay_ms(500);
		HAL_IWDG_Refresh(&hiwdg);		//ι��  //2s���*********************************
		delay_ms(500);
		HAL_IWDG_Refresh(&hiwdg);		//ι��  //2s���*********************************
		Ring.short_ring=1;
		delay_ms(100);
		Ring.short_ring=1;
		delay_ms(500);
		HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
		
	}
}
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void DispMenuFactoryCalInit(void)
{

    FactoryTorqueCal();
};
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void DispMenuFactoryCalON(void)
{};
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/

void DispMenuFactoryCalSel(void)
{};
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void DispMenuFactoryCalUp(void)
{};

void DispMenuFactoryCalDown(void)
{};
	
	/**************************************************************
    *Function:
    *PreCondition:
    *Input:
    *Output:
   ***************************************************************/
void MP6570_CalThetaBias()
{
	Disp_cal();
	lv_task_handler();
	delay_ms(500);
	MP6570_AutoThetaBias(0x00, 150,2000); 								//�ҳ�ʼ�Ƕ�
	Disp_cal();
	lv_task_handler();
	delay_ms(1000);
	HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
}

/**********************************************************************************************************
*	�� �� ��: uint8_t CheckPassword()
*	����˵��: �����ж�������0������������󣻷���1����������Ƕ�У��������У׼ģʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*	�� �� ��: 1������ʢ
*	�޶���¼: 1
*	�༭����: 1: 20211111         
**********************************************************************************************************/
uint8_t Num = 0;
uint8_t pw[4] = {0};
uint8_t CheckPassword()
{
	uint8_t ctlloop = 1;
	uint8_t temp_pw = 0;
	uint8_t re = 0;
    pw[0] = 0;
	pw[1] = 0;
	pw[2] = 0;
	pw[3] = 0;
	
	Num = 0;
	
	Disp_cal();
	lv_task_handler();
    while(ctlloop)
    {
        HAL_IWDG_Refresh(&hiwdg);																	//ι��  //2s���*********************************
        temp_pw = CheckKey_Password(temp_pw);
        if(temp_pw != 0)
        {
			pw[Num] = temp_pw;
			Num++;
			Disp_cal();
			lv_task_handler();
            temp_pw = 0;
        }
        if(Num >= 4)
        {
            if((pw[0] == 4) && (pw[1] == 2) && (pw[2] == 3) && (pw[3] == 4))						// [<] [S] [>] [<]
            {
                re = 2;																				// Ť�ص���
				Cal_mode_flag = Cal_Noload_flag;													// ���ص���У��
            }
			else if((pw[0] == 4) && (pw[1] == 3) && (pw[2] == 4) && (pw[3] == 3)) 					// ��<�� ��>�� ��<�� ��>��
			{
				re = 1;																				// �ű�У׼
				Cal_mode_flag = AutoThetaBias_flag;													// ����Ƕ�У��
			}
            else 
			{
				re = 0;																				// �������
				Cal_mode_flag = Error_Password_flag;												// ���������
            }
			Num = 0;
            ctlloop = 0;																			// ����������Ч���֣����ܶԴ��˳�
        }
    }
    return re;
}


