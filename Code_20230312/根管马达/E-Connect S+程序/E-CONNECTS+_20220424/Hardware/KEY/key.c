#include "key.h"
#include "main.h"
#include "stm32f4xx_hal.h"


Key_e KeyVaule=none;
uint8_t KEY_COUNT_IT;
/**************************************************************
  *Function:
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
void Key_Scan(void)
{
    static Key_e LastKey = none;       // key vaule
    static uint8_t ValidCnt = 0, InValidCnt = 0, PowerLongCnt = 0, MemoLongCnt = 0, MotorLongCnt = 0, PedalLongCnt = 0;            // key keep time
    Key_e KeyTemp = none;
    static uint8_t state = 0;
	if(KEY_COUNT_IT >= 10)
	{
		KEY_COUNT_IT = 0;
		
		if((KEY_TRUE == KEY_ON_PORT)&&(KEY_FAULT == KEY_SEL_PORT)&&(KEY_FAULT == KEY_UP_PORT)&&(KEY_FAULT == KEY_DOWN_PORT))
			KeyTemp = key_on;    																									//启动键按下
		if((KEY_TRUE == KEY_SEL_PORT)&&(KEY_FAULT == KEY_ON_PORT)&&(KEY_FAULT == KEY_UP_PORT)&&(KEY_FAULT == KEY_DOWN_PORT))
			KeyTemp = key_sel;   																									//选择键按下
		if((KEY_TRUE == KEY_UP_PORT)&&(KEY_FAULT == KEY_SEL_PORT)&&(KEY_FAULT == KEY_ON_PORT)&&(KEY_FAULT == KEY_DOWN_PORT))
			KeyTemp = key_up;    																									//上键按下
		if((KEY_TRUE == KEY_DOWN_PORT)&&(KEY_FAULT == KEY_SEL_PORT)&&(KEY_FAULT == KEY_ON_PORT)&&(KEY_FAULT == KEY_UP_PORT))
			KeyTemp = key_down;																										//下键按下


		//===================scan KeyPad end=========================================================//
		if(( LastKey == KeyTemp)&&(none != KeyTemp)&&(0 == state))       	// key pad is in state 0(no key pressed)
		{
			ValidCnt ++;
			InValidCnt = 0;
			if(KEY_DELAY == ValidCnt)
			{
				ValidCnt = 0;
				KeyVaule = KeyTemp;       									//保存键值
				state = 1;                                         			// key pad turn in state 1 (key pressed).
			}
		}
		else if(none == KeyTemp)
		{

			InValidCnt ++;
			if((KEY_DELAY / 3) <= InValidCnt)
			{
				InValidCnt = 0;
				state = 0;                                         			// key pad turn in state 0(no key pressed)
			}
		}
		else
		{
			ValidCnt = 0;
			InValidCnt = 0;
		}
		//=====================  long key press=========================//
		if(( LastKey == KeyTemp)&&(key_sel == KeyTemp)&&(1 == state))        // motor key long_press
		{
			MotorLongCnt ++;
			KeyVaule = none;
			if(KEY_LONG < MotorLongCnt)
			{
				MotorLongCnt = 0;
				KeyVaule = key_sel_long;
				state = 2;
			}
		}	/*****************2017-8-24 add **********************/
		else if(KEY_TRUE == KEY_ON_PORT && MotorLongCnt > 0 )				//按下sel后，又按下on
		{
			MotorLongCnt = 0;
			KeyVaule = key_sel_long_on;
			state = 2;
		}
		else if(MotorLongCnt)                                   			// short
		{
			MotorLongCnt = 0;
			KeyVaule = key_sel;
		}
		//============================================================================//
		LastKey = KeyTemp;
	}
}

/**************************************************************
 * 函数名：  Get_KeyVaule
 * 描述  ：  获取键值
 * 输入  :   无
 * 输出  ： 键值
 ***************************************************************/
Key_e Get_KeyVaule() 
{
	Key_e KeyTemp=none;
	KeyTemp = KeyVaule;
	KeyVaule = none;
	return KeyTemp;
}
