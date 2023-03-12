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
uint8_t TqStoreCheck[2]; 			//EEPROM数据扭矩标志存储
uint8_t CheckPassword();
void MP6570_CalThetaBias();
uint8_t Speed_limit;
uint8_t Error_CNT = 0;

/**********************************************************************************************************
*	函 数 名: void FactoryTorqueCal()
*	功能说明: 先是进入电机校正，必须校正，不然无法正常启动。
			  出厂扭矩校准，先是空载电流测试，无扭矩条件下，将所有速度扫一遍。接着是固定扭矩扫速度的方式，各个扭矩对应的速度都扫一遍。
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211111         
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
		Save1_Read0_Torque(0);													//读取之前校准的扭矩到内存
		delay_ms(500);
		/********************/
		HAL_IWDG_Refresh(&hiwdg);											//喂狗  //2s会饿*********************************
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
			HAL_IWDG_Refresh(&hiwdg);																//喂狗  //2s会饿*********************************
			Key = Get_KeyVaule();

			switch(Key) 
			{
				case key_on: { 																		//启动键,找出新的菜单状态编号
								Ring.short_ring=1;
//								MX_SPI1_Init();					
								Cal_mode_flag = Cal_Noload_Run_flag ;								// 进入空载电流测试模式
//								delay_ms(200);
//								//配置电机参数
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
								sel.Speed = s50;								      				//设置校准时的速度条件
								motor_settings.mode = 0;
								motor_settings.forward_speed = d_Speed[sel.Speed];
								motor_settings.upper_threshold = 5000; 								//设置一个比较大的值，使不反转
								motor_settings.lower_threshold = 2500;
								update_settings(&motor_settings);
								Round_Cnt = 0;
								start(); 															//启动电机
								
								//进入速度循环
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
									
									//显示电机速度
									Disp_cal();
									delay_ms(50);
									lv_task_handler();
							
									delay_ms(500);

									/********************/
									HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
									for(cnt = 0; cnt < 30; cnt++)
									{
										ave += motor_iq;  											//累计电流
										delay_ms(20);
									}
									
									/********************/	
									HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
									ave = ave / 30;
									 
									NoLoadCurrent[i] = ave;											//空载电流值
									ave = 0;
								}
								
								/**************************停止电机，切换转速********************************/
								stop();
								delay_ms(500);
								HAL_IWDG_Refresh(&hiwdg);											//喂狗  //2s会饿*********************************
								delay_ms(500);
								HAL_IWDG_Refresh(&hiwdg);											//喂狗  //2s会饿*********************************
								sel.Speed = s50;								      				//设置校准时的速度条件
								motor_settings.mode = 0;
								motor_settings.forward_speed = -d_Speed[sel.Speed];
								motor_settings.upper_threshold = 5000; 								//设置一个比较大的值，使不反转
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
									HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
									for(cnt = 0; cnt < 30; cnt++)
									{
										ave += motor_iq;  											//累计电流
										delay_ms(20);
									}
									
									/********************/	
									HAL_IWDG_Refresh(&hiwdg);										//喂狗  //2s会饿*********************************
									ave = ave / 30;
									 
									NoLoadCurrent[i] = ave;											//空载电流值
									ave = 0;
								}
								stop();
								Save1_Read0_NoloadTor(1);											//save
								ctlloop = 0;
								Cal_mode_flag = Cal_Torque_flag;									// 进入转矩测试模式
								delay_ms(500);
								break;
							}
			case key_sel: 	{ 																		//按下“S”键关机
								Ring.short_ring = 1;
								delay_ms(100);
								Ring.short_ring = 1;
								delay_ms(200);
								HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
								break;
							}
			case key_up: 	{ 																// 选择扭矩+
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
								Cal_mode_flag = Cal_Torque_flag;									// 进入转矩测试模式
								delay_ms(500);

								break;
							}
	//---------------------------------------------------------------------------
			default: 																				//按键错误的处理
								IsKey = 0;
								break;
			}
		}
		
		/**************************显示转矩测试模式********************************/
		Disp_cal();
		delay_ms(50);
		lv_task_handler();
		
		/**************************更新空载电流，为转矩测试做准备********************************/
		Save1_Read0_NoloadTor(0);																	// 读取空载电流
		
		delay_ms(200);
		ctlloop = 1;
		while(ctlloop)
		{
			Key_e Key;
			/********************/
			HAL_IWDG_Refresh(&hiwdg);																//喂狗  //2s会饿*********************************
			Key = Get_KeyVaule();

			switch(Key) 
			{
				case key_on: 	{ 																	//启动键,找出新的菜单状态编号
									Ring.short_ring=1;
									ctlloop=0;
									Cal_mode_flag = Cal_Torque_Run_flag;							// 转矩测试模式运行
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
				default: 																			//按键错误的处理

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
				HAL_IWDG_Refresh(&hiwdg);															//喂狗  //2s会饿*********************************
				Key = Get_KeyVaule();
				
				Disp_cal();
				delay_ms(50);
				lv_task_handler();
				
				switch(Key) 
				{
					case key_on: 	{ 																//启动键,找出新的菜单状态编号
										Ring.short_ring = 1;
//										MX_SPI1_Init();
//										reset_parameters1();
										delay_ms(100);
										ctlloop = 0;
//										init_MC();
//										stop();
//										delay_ms(200);
//										LOOPTIMER_ENABLE;
										sel.Speed = s50;											//设置校准时的速度条件
										motor_settings.mode = 0;
										motor_settings.forward_speed = d_Speed[sel.Speed];
										motor_settings.upper_threshold = 5000; 						//设置一个比较大的值，使不反转
										motor_settings.lower_threshold = 2500;
										update_settings(&motor_settings);
										Round_Cnt = 0;
										start();
										Cal_mode_flag = Cal_Torque_Run1_flag;							// 转矩测试模式运行
										delay_ms(200);
										
										break;
									}
					case key_sel: 	{ 																//sel esc
										Ring.short_ring = 1;
										delay_ms(100);
										ctlloop = 0;
										j=14;														//利用循环调节，强制跳出

										break;
									}
					case key_up: 	{ 																// 选择扭矩+
										Ring.short_ring=1;
										j++;
										if(j > 13)
										{
											j = 0;
										}
										Torque_cnt = j;

										break;
									}
					case key_down: 	{ 																// 选择扭矩-
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
					default: 																		//按键错误的处理
									IsKey = 0;
									break;
				}
			}
			
			/**************************根据转矩，确定转速上限********************************/
			if(Torque_cnt <= t10)
			{
				Speed_limit = s1500 + 1;															//扭矩校准，0  ~ 1.0的扭矩，全速度
			}
			else if((Torque_cnt > t10)&&(Torque_cnt <= t20))
			{
				Speed_limit = s1200 + 1;															//扭矩校准，1.0~ 2.0的扭矩，除去1200rpm往后
			}else if((Torque_cnt > t20)&&(Torque_cnt <= t40))
			{
				Speed_limit = s700 + 1;																//扭矩校准，2.0~ 4.0的扭矩，除去700rpm往后
			}else if(Torque_cnt > t40)
			{
				Speed_limit = s400 + 1;																//扭矩校准，	 5.0的扭矩，除去400rpm往后
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
				HAL_IWDG_Refresh(&hiwdg);															//喂狗  //2s会饿*********************************
				delay_ms(300);

				for(cnt=0; cnt < 10; cnt++)
				{		 
					//	ave+=(motor_iq+NoLoadCurrent[i]);
					if((motor_iq > NoLoadCurrent[i])&&(motor_iq < 500))													//只有读取的数据大于空载电流才算有效
					{  
						ave += motor_iq;
					}
					else{
						ave += 0;																	//有一个数据无效，不计入平均值计算
						if(cnt >= 1)
						{
							cnt--;
						}
						dev_ave --;
					}
					delay_ms(10);
				}
				
				if(dev_ave <= 6)																	//丢失数据过多，报错退出
				{
					HAL_IWDG_Refresh(&hiwdg);														//喂狗  //2s会饿*********************************
					stop();
					Ring.long_ring = 1;
					delay_ms(1500);
					HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);				//POWER OFF
				}
				/********************/	
				HAL_IWDG_Refresh(&hiwdg);															//喂狗  //2s会饿*********************************
				ave = ave / 10;
				if(Torque_cnt <= 2)
				{
					S_T_I_COE1[Torque_cnt][i] = ave;												//扭矩校准，0  ~ 1.0的扭矩，全速度
				}
				else if((Torque_cnt > 2)&&(Torque_cnt <= 5))
				{
					S_T_I_COE2[Torque_cnt-3][i] = ave;												//扭矩校准，1.0~ 2.0的扭矩，除去1500rpm
				}else if((Torque_cnt > 5)&&(Torque_cnt <= 11))
				{
					S_T_I_COE3[Torque_cnt-6][i] = ave;												//扭矩校准，2.0~ 4.0的扭矩，除去700rpm往后
				}else if(Torque_cnt > 11)
				{
					S_T_I_COE4[Torque_cnt-12][i] = ave;												//扭矩校准，	 5.0的扭矩，除去400rpm往后
				}
				ave = 0;
				dev_ave = 30;																		//除数恢复为10
			}
			Cal_mode_flag = Cal_Torque_Run_flag;							// 转矩测试模式运行
			
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

		/*************测试根测开路值 ****************/
	//    for(i=0; i<10; i++) {
	//        if(flag.sam_finish==1) {
	//            flag.sam_finish=0;

	//            samH_sum+=samh.VPP;
	//            delay_ms(50);
	//        }
	//    }
		/*	samH_RootOutPos=samH_sum/10;		//计算开路值
			RootOut[0]=samH_RootOutPos>>8;
				RootOut[1]=samH_RootOutPos&0x00ff;
			root_flag[0]=0x1A;
			 EEPROM_Write(ROOTOUTPOS_ADDR,RootOut,2);		// 写入开路值 ,先高位后低位
		   EEPROM_Write(ROOTFLAG_ADDR,root_flag,1);				//写入更新标志
			samH_RootInPos=(samH_RootInPosTemp+samH_RootOutPos)/2;		//重新计算根管进入阀值
			disp.Gram_Clear(BLACK);
			while(ctlloop)
			{
				  Key_e Key;
				 CalculateApex();			//计算根测值
				 DebugApexShowData();		//显示根测数据
				 Key = Get_KeyVaule();
				 if(Key!=none)
				 {
					Ring.short_ring=1;
					 ctlloop=0;		//有按键按下，退出循环
				 }
				 delay_ms(50);
		HAL_IWDG_Refresh(&hiwdg);		//喂狗  //2s会饿
			}*/
	//    disp.Gram_Clear(GRAY);
	//    disp.String(20,20,"Saving...",24,B,BLACK,GRAY );
	//    disp.RefreshGram();
//		LCD_Clear(WHITE);

		/************写入更新标志位*****************/
		TqStoreCheck[0]=0XA8;
		TqStoreCheck[1]=0XA9;
		EEPROM_Write(TQFLAG_ADDR,TqStoreCheck,2);		//写入更新标志
		/*********************************************/
	  //  SaveCalTorque();								//存储校准的扭矩值
		
		Save1_Read0_Torque(1);
		/*************做校验******************/

		Cal_mode_flag = Cal_Fishing;
		Disp_cal();
		delay_ms(50);
		lv_task_handler();		
		/********************/

		/********************/
		delay_ms(500);
		HAL_IWDG_Refresh(&hiwdg);		//喂狗  //2s会饿*********************************
		delay_ms(500);
		HAL_IWDG_Refresh(&hiwdg);		//喂狗  //2s会饿*********************************
		delay_ms(500);
		HAL_IWDG_Refresh(&hiwdg);		//喂狗  //2s会饿*********************************
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
	MP6570_AutoThetaBias(0x00, 150,2000); 								//找初始角度
	Disp_cal();
	lv_task_handler();
	delay_ms(1000);
	HAL_GPIO_WritePin(KEEP_PWR_GPIO_Port,KEEP_PWR_Pin,GPIO_PIN_RESET);	//POWER OFF
}

/**********************************************************************************************************
*	函 数 名: uint8_t CheckPassword()
*	功能说明: 密码判定，返回0，密码输入错误；返回1，开启电机角度校正，进入校准模式。
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211111         
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
        HAL_IWDG_Refresh(&hiwdg);																	//喂狗  //2s会饿*********************************
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
                re = 2;																				// 扭矩调试
				Cal_mode_flag = Cal_Noload_flag;													// 空载电流校正
            }
			else if((pw[0] == 4) && (pw[1] == 3) && (pw[2] == 4) && (pw[3] == 3)) 					// 【<】 【>】 【<】 【>】
			{
				re = 1;																				// 磁编校准
				Cal_mode_flag = AutoThetaBias_flag;													// 电机角度校正
			}
            else 
			{
				re = 0;																				// 密码错误
				Cal_mode_flag = Error_Password_flag;												// 密码错误处理
            }
			Num = 0;
            ctlloop = 0;																			// 输入三次有效数字，不管对错都退出
        }
    }
    return re;
}


