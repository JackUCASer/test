
////////////////////////////////////////////////////////////////////
/// @file key_Task.c
/// @brief °´¼üÈÎÎñ´¦Àíº¯Êý
/// 
/// ÎÄ¼þÏêÏ¸ÃèÊö£º	
///					
///
///
///
///
/// @author Íõ²ýÊ¢
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>ÐÞ¸ÄÀúÊ·£º--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ¹«Ë¾£º³£ÖÝÈüÀÖÒ½ÁÆ¼¼ÊõÓÐÏÞ¹«Ë¾
////////////////////////////////////////////////////////////////////
#include "delay.h"
#include "key_Task.h"
#include "key.h"
#include "beep.h"
#include "Menu.h"
#include "MenuData.h"
#include "Multiple_Styles.h"
#include "macros.h"
#include "mp6570.h"
#include "control.h"
#include "customer_control.h"
#include "DataStorage.h"

WorkMode_e gTempMode;									//Ä£Ê½¸ü¸ÄÈ«¾Ö±äÁ¿
uint8_t NeedFlash = 0;               					//Ìø×ª½øÈë×Ó²Ëµ¥ Ë¢ÐÂ±êÖ¾  >1Ë¢ÐÂ
//uint8_t language = EN;           						//ÓïÑÔ±êÖ¾
uint8_t KeyFuncIndex = 0;        						//Ë÷Òý
uint8_t LastKeyFuncIndex = 0;        					//ÉÏ¸öË÷Òý
uint8_t LastKeyFuncIndex_sel = 6;        				//ÉÏ¸öË÷Òý
uint8_t BlinkCtrl=0;
uint8_t Charge_ctrl = 0;
extern uint8_t ChargDisp_flag,ChargDisp_cnt;
extern  uint8_t Last_apex_pos;
extern KbdTabStruct KeyTab[];
void (*KeyFuncPtr)(); //°´¼ü¹¦ÄÜÖ¸Õë
uint16_t Changing_disp_cnt = 0;

/**********************************************************************************************************
*	º¯ Êý Ãû: void Jump2Menu(uint8_t Index)
*	¹¦ÄÜËµÃ÷: Ìø×ªµ½Ö¸¶¨²Ëµ¥
*	ÐÎ    ²Î: Index  Ö¸¶¨µÄ²Ëµ¥Ë÷Òý
*	·µ »Ø Öµ: ÎÞ
*	±à ¼­ Õß: 1£ºÍõ²ýÊ¢
*	ÐÞ¶©¼ÇÂ¼: 1
*	±à¼­ÈÕÆÚ: 1: 20210824         
**********************************************************************************************************/
void Jump2Menu(uint8_t Index)
{
    KeyFuncIndex = Index;
}

/**********************************************************************************************************
*	º¯ Êý Ãû: uint8_t Get_Index(void)
*	¹¦ÄÜËµÃ÷: »ñÈ¡µ±Ç°µÄ²Ëµ¥Ë÷Òý
*	ÐÎ    ²Î: ÎÞ
*	·µ »Ø Öµ: ÎÞ
*	±à ¼­ Õß: 1£ºÍõ²ýÊ¢
*	ÐÞ¶©¼ÇÂ¼: 1
*	±à¼­ÈÕÆÚ: 1: 20210824         
**********************************************************************************************************/
uint8_t Get_Index(void)
{
   return KeyFuncIndex;
}
uint8_t Versions_Flag = 0;
/**********************************************************************************************************
*	º¯ Êý Ãû: WorkMode_e CheckKey_StandbyMode(WorkMode_e mode)
*	¹¦ÄÜËµÃ÷: ´ý»úÄ£Ê½ÏÂµÄ°´¼üÉ¨Ãè
*	ÐÎ    ²Î: WorkMode_e mode	ÏµÍ³ÔËÐÐÄ£Ê½
*	·µ »Ø Öµ: ÎÞ
*	±à ¼­ Õß: 1£ºÍõ²ýÊ¢
*	ÐÞ¶©¼ÇÂ¼: 1
*	±à¼­ÈÕÆÚ: 1: 20210824         
**********************************************************************************************************/
WorkMode_e CheckKey_StandbyMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; 											/* °´¼ü´úÂë */
	gTempMode = mode;
	IsKey = 1;
	ucKeyCode = Get_KeyVaule();
    if((ucKeyCode!=none) || (sys.Charging==1))
	{
		Time_cnt = 0;							//ÓÐ°´¼ü°´ÏÂ£¬×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
	}
    if(-1==WaitTimeOut() && KeyFuncIndex<=50&& KeyFuncIndex>0 /*&& sys.DispBattFlag!=1*/ )		
	{			//ÅÐ¶Ï³¬Ê±Ìõ¼þ£ºÔÚ²ÎÊýÉèÖÃÄ£Ê½£¨ÏµÍ³ÉèÖÃÎÞ³¬Ê±£©,´ý»úÄ£Ê½ÏÂ
			        /***********¸ß×ªËÙÅ¤¾ØÏÞÖÆ**************/
        if(sel.Speed>s700 && sel.Torque>t20)		//750rpm 2.0Ncm
        {
           sel.Torque=t20;
        }
        MemorySave(mem.MemoryNum);		//±£´æ²ÎÊý
        Jump2Menu(TOP_MENU_INDEX);
        NeedFlash=1;
		sel_page_flash  = 1;
    }

	switch(ucKeyCode) 
	{
		/*******************************KEY_ON°´¼ü´¦Àí*****************************************************************************************/
		case key_on: 		{ 	//Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
									Ring.short_ring = 1;
									TimeOutCnt=0;
									if((KeyFuncIndex >= 6) && (KeyFuncIndex <= 46))
									{
										LastKeyFuncIndex_sel = KeyFuncIndex;													//±£´æ²Ù×÷Ç°µÄË÷Òý,ÓÃÓÚselÉèÖÃ¼ÇÒäÇ°´Î²Ù×÷Î»ÖÃ
									}
									KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
									KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																			

									break;
								}
		
		/*******************************KEY_SEL°´¼ü´¦Àí*****************************************************************************************/						
		case key_sel: 			{ 	//Ñ¡Ôñ¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
									Ring.short_ring = 1;
									TimeOutCnt=0;
//									if((mem.MemoryNum == 0) && (sel.OperaMode == 4) && (KeyFuncIndex == 0)) 
//									{
//										
//									}
//									else 
									{
										KeyFuncIndex = KeyTab[KeyFuncIndex].KeySelState;
										KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
										(*KeyFuncPtr)();//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
									}
									
									break;
								}
		
		/*******************************KEY_UP°´¼ü´¦Àí*****************************************************************************************/
		case key_up: 		{ 	//ÏòÉÏ¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
									Ring.short_ring = 1;
									TimeOutCnt=0;
									LastKeyFuncIndex = KeyFuncIndex;												//±£´æ²Ù×÷Ç°µÄË÷Òý
									KeyFuncIndex     = KeyTab[KeyFuncIndex].KeyUpState;
									KeyFuncPtr       = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																//Ö8´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
									Jump2Menu(LastKeyFuncIndex);
			
									break;
								}
		
		/*******************************KEY_DOWN°´¼ü´¦Àí*****************************************************************************************/
		case key_down: 	{ 	//ÏòÏÂ¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
									Ring.short_ring = 1;
									TimeOutCnt=0;
									LastKeyFuncIndex = KeyFuncIndex;													//±£´æ²Ù×÷Ç°µÄË÷Òý
									KeyFuncIndex     = KeyTab[KeyFuncIndex].KeyDnState;
									KeyFuncPtr       = KeyTab[KeyFuncIndex].CurrentOperate;
									(*KeyFuncPtr)();																	//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
									Jump2Menu(LastKeyFuncIndex);														//»Ö¸´²Ù×÷Ç°Ë÷Òý
									break;
								}

		/*******************************KEY_SET×éºÏ°´¼ü´¦Àí*****************************************************************************************/
		case key_sel_long: 		{
									Ring.short_ring = 1;
									TimeOutCnt=0;
									sys.FlagThetaBias = 1;
									
									break;
								}
		case key_sel_long_on: 	{
									gTempMode = PowerOff_mode;
									break;
								}

	//---------------------------------------------------------------------------
		default: 				//°´¼ü´íÎóµÄ´¦Àí
								IsKey = 0;
								break;
	}

	if(NeedFlash) 																					//Ë¢ÐÂÒ³Ãæ
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
		NeedFlash = 0;
	}

	
	if((sys.Charging == 1)&& (KeyFuncIndex == 0))
	{
		if(Changing_disp_cnt > 500)
		{
			DispBatery_charge(1, sys.BatteryLevel, sys.Charging);										//ÏÔÊ¾µç³ØµçÁ¿
			Changing_disp_cnt = 0;
		}	
	}else if((sys.Charging == 0) && (sys.DispBattFlag == 1) && (KeyFuncIndex == 0))
	{
		sys.DispBattFlag = 0;
		DispBatery_charge(1, sys.BatteryLevel, sys.Charging);										//ÏÔÊ¾µç³ØµçÁ¿
	}
	
	if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==0 && Charge_ctrl == 1)
	{
		Charge_ctrl = 0;
	}
    else if(HAL_GPIO_ReadPin(SAM_5V_GPIO_Port,SAM_5V_Pin)==1 && Charge_ctrl == 0)
    {
			Charge_ctrl = 1;
			EndoRing(0,0);	
			sys.Charging = 1;		//¼ì²âÍâ½ÓµçÔ´Òý½Å£¬¸ßµçÆ½±íÊ¾ÕýÔÚ³äµç
			Ring.short_ring = 1;
			ChargDisp_flag = 0;			//ÇåÁã³äµçÏÔÊ¾±êÖ¾
			ChargDisp_cnt = 0;				//ÇåÁã¼ÆÊý
			gTempMode = Charging_mode;
    }

	mode = gTempMode;	
   
  return mode;
}
/**************************************************************
  *Function:   ÔÚÔËÐÐÄ£Ê½ÏÂµÄ°´¼üÉ¨Ãè
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
WorkMode_e CheckKey_RunMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* °´¼ü´úÂë */

	IsKey = 1;
	Time_cnt = 0;							//×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
	ucKeyCode = Get_KeyVaule();
 //   if(Key!=none)ClearClock();			//ÓÐ°´¼ü°´ÏÂ£¬×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
	switch(ucKeyCode) 
	{
		case key_on: 	{ //Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
							Ring.short_ring = 1;
							TimeOutCnt = 0;
							KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
							KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
							Last_apex_pos = 24;			//³õÊ¼»¯¸ù²âÎ»ÖÃ
							stop();
							delay_ms(1);
							LOOPTIMER_DISABLE;

							NeedFlash = 1;
							sel_page_flash = 1;
							(*KeyFuncPtr)();//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
							mode = Standby_mode;
							break;
						}
//---------------------------------------------------------------------------
		default: //°´¼ü´íÎóµÄ´¦Àí
									IsKey = 0;
									break;
	}
	if(NeedFlash) 																					//Ë¢ÐÂÒ³Ãæ
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
		NeedFlash = 0;
	}
  return mode;
}

/**************************************************************
  *Function:   ÔÚapexÔËÐÐÄ£Ê½ÏÂµÄ°´¼üÉ¨Ãè
  *PreCondition:
  *Input:
  *Output:
 ***************************************************************/
WorkMode_e CheckKey_RunWithApexMode(WorkMode_e mode)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* °´¼ü´úÂë */

	IsKey = 1;
	Time_cnt = 0;							//×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
	ucKeyCode = Get_KeyVaule();
 //   if(Key!=none)ClearClock();			//ÓÐ°´¼ü°´ÏÂ£¬×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
	switch(ucKeyCode) 
	{
    case key_on: { 	//Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
					Ring.short_ring = 1;
					TimeOutCnt = 0;
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyONState;
					KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
					Last_apex_pos = 24;								//³õÊ¼»¯¸ù²âÎ»ÖÃ
					stop();
					delay_ms(1);
					LOOPTIMER_DISABLE;

					NeedFlash = 1;
					sel_page_flash = 1;
					(*KeyFuncPtr)();								//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
					mode = Standby_mode;
					sys.FlagRunWithApex = 0;
					sys.ActiveShutRunning = 1;						//Ö÷¶¯°´ÏÂON¼ü£¬ÏÂ´Î²»×Ô¶¯¸ù²âÆô¶¯£¬Ö±µ½½â³ý
					break;
				}
//---------------------------------------------------------------------------
		default: //°´¼ü´íÎóµÄ´¦Àí
					IsKey = 0;
					break;
	}	
	if(NeedFlash) 																					//Ë¢ÐÂÒ³Ãæ
	{ 
		KeyFuncPtr = KeyTab[KeyFuncIndex].CurrentOperate;
		(*KeyFuncPtr)();																			//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
		NeedFlash = 0;
	}
	return mode;
}


uint8_t CheckKey_Password(uint8_t pw)
{
	uint8_t IsKey;
	uint8_t ucKeyCode; /* °´¼ü´úÂë */

	IsKey = 1;
	ucKeyCode = Get_KeyVaule();
	if(ucKeyCode != none)
	{
		Time_cnt = 0;							//×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
//		ClearClock();							//ÓÐ°´¼ü°´ÏÂ£¬×Ô¶¯¹Ø»ú¼ÆÊ±ÇåÁã
	}
	
	switch(ucKeyCode) 
	{
		case key_on: { //Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 1;
										break;
									}
		case key_sel: { //Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 2;
										break;
									}
		case key_up: { //Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
										Ring.short_ring = 1;
										TimeOutCnt = 0;

										pw = 3;
										break;
									}
		case key_down: { //Æô¶¯¼ü,ÕÒ³öÐÂµÄ²Ëµ¥×´Ì¬±àºÅ
										Ring.short_ring = 1;
										TimeOutCnt = 0;
										pw = 4;
										break;
									}


//---------------------------------------------------------------------------
    default: //°´¼ü´íÎóµÄ´¦Àí
									IsKey = 0;
									break;
	}
  return pw;
}

void NextOperate(uint8_t Index)			//½öÊÊÓÃÓÚ²Ëµ¥²éÕÒ
{
	KeyFuncIndex = Index;
	KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
	(*KeyFuncPtr)();//Ö´ÐÐµ±Ç°°´¼üµÄ²Ù×÷
}


void InitLastIndex()
{
	LastKeyFuncIndex_sel=6;		// ³õÊ¼»¯
}














