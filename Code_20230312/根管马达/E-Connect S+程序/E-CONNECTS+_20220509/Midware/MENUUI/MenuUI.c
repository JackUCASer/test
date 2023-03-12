
#include "lcd.h"
#include "datatype.h"
#include "../lvgl/lvgl.h"
#include "MenuDisp.h"
#include "MenuData.h"
#include "customer_control.h"
#include "beep.h"
#include "macros.h"


extern const unsigned char gImage_RCM_NUM[21][135];
extern const unsigned char gImage_apex_bg_num[4][60];
extern uint8_t rev_first;

//待机界面设计

void DrawGird(uint8_t x,uint8_t y,uint16_t c)
{
	LCD_Fill(x,y,x+3,y+35,c);
};


uint8_t last_x=150,last_apex_x=30;  //全局变量
void ResLast_x()
{
	last_x = 150;
}

void ResLast_apex_x()
{
	last_apex_x = 30;
}

void DispTorqueBar(uint8_t tq ,uint8_t running)
{
	uint8_t x,y,z;
	static uint8_t sw = 0;

//	if(running==0)last_x=150;
	z = d_Torque[sel.Torque]/2-1;
	if(tq > z) 
	{
		tq = z;	
	}
	
	x = 150 - tq * 5; 
	
	if(blink_flag == 1)
	{
		if(sw == 0)
		{
			blink_flag = 0;
			sw = 1;
			DrawGird(150-z*5,4,RED);
		}
		else if(sw == 1)
		{
			blink_flag = 0;
			sw = 0;			
			DrawGird(150-z*5,4,WHITE);
		}	
	}
	 
	
	if(x < last_x)
	{
		while(x < last_x)
		{
			DrawGird(last_x,4,0x0400);
//		if(tq<12)DrawGird(last_x,4,0x0400);
//		else if(tq>=12 && tq<18)DrawGird(last_x,4,0xfd20);
//		else if(tq>=18)DrawGird(last_x,4,RED);
			last_x -= 5;
		}
	}
	else if(x > last_x)
	{
		while(x > last_x)
		{
			DrawGird(last_x,4,WHITE);
			last_x += 5;
		}	
	}
 	else if((x == last_x) && (x == 150))
	{
		DrawGird(150,4,WHITE);
 	}		
	last_x = x;
}

 
void ApexDispMode_1(uint8_t num)
{
  int i;
  LCD_Fill(28,22,78,26,WHITE);
  HAL_Delay(33);
	
	for(i = 0; i < 10; i++)
	{  
		if(i == 5) 
		{
			LCD_Fill(28,29+i*7,78,33+i*7,WHITE);
		}
		else 
		{
			LCD_Fill(28,29+i*7,78,33+i*7,WHITE);
		}
		HAL_Delay(30);
	}
	LCD_Fill(28,99,78,103,GREEN);
  HAL_Delay(30);
  for(i=0;i<5;i++)
  {
		if(i==2) 
		{
			LCD_Fill(28,106+i*7,78,110+i*7,GREEN);
		}
		else 
		{
			LCD_Fill(28,106+i*7,78,110+i*7,GREEN);
		}

		HAL_Delay(30);
  }
	
	LCD_Fill(28,141,78,145,GREEN);
	for(i = 0; i < 2; i++)
	{
		LCD_Fill(28,148+i*7,78,152+i*7,RED);
		HAL_Delay(30);
	}
}
   /**************************************************************
    *Function:   画根测格子条
    *PreCondition:
    *Input:
    *Output:
   ***************************************************************/
void DrawApexGird(uint8_t x,uint8_t y,uint16_t c)
{
	LCD_Fill(x,y,x+4,y+40,c);
};
  /**************************************************************
    *Function:   画根测闪烁格子条
    *PreCondition:
    *Input:
    *Output:
   ***************************************************************/
void DrawFlashGird(uint8_t x,uint8_t y,uint16_t c)
{
	LCD_DrawRectangle(x,y,x+5,y+40,c);
};
  /**************************************************************
    *Function:   根测背景刻度
    *PreCondition:
    *Input:
    *Output:
   ***************************************************************/
void ApexDispBG()
{
	
}

  /**************************************************************
    *Function:   根测显示条刷屏
    *PreCondition:
    *Input:
    *Output:
   ***************************************************************/
void DispApexBar(uint8_t num )
{

}
/****************************************************/
 /**************************************************************
    *Function:   扭矩条显示和音量提示
    *PreCondition:
    *Input:
    *Output:
   ***************************************************************/

void RunDisp()
{

}
