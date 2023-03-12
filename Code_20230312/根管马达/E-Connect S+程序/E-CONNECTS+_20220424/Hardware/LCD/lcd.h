
#ifndef __LCD_H
#define __LCD_H		

#include "stm32f4xx_hal.h"
#include "main.h"
#include "stdlib.h"	  

#define USE_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 80
#endif


#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long

typedef struct COLOER_
{
    uint8_t fore;
    uint8_t back;
    uint8_t Blink_on;		//0 不闪烁；1 闪烁
    uint8_t Blink_flag;
    uint8_t Blink_cnt;
    uint8_t ApexBlink_flag;
    uint8_t ApexBlink_cnt;

} COLOR;
extern COLOR color;
    			
//定义一个oled的驱动函数接口
typedef struct OLED_
{
    uint8_t fore;
    uint8_t back;
    void (*String)(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*StringBlink)(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*Num)(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*NumBlink)(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*Img)(uint8_t x,uint8_t y,uint8_t Pic_XSIZE,uint8_t Pic_YSIZE,const uint8_t *Pic,unsigned char forecolor,unsigned char background);
    void (*ImgBlink)(uint8_t x,uint8_t y,uint8_t Pic_XSIZE,uint8_t Pic_YSIZE,const uint8_t *Pic,unsigned char forecolor,unsigned char background);
    void (*MemFib)(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*Init)(void);
    void (*RefreshGram)(void);
    void (*RefreshGramBlink)(void);
    void (*Refresh_Run_torqueBar)(void);
    void (*RefreshGramMov)(void);
    void (*Gram_Clear)(uint8_t mode);
    void (*Gram_ClearBlink)(uint8_t mode);
    void (*ImgMov)(uint8_t x,uint8_t y,uint8_t Pic_XSIZE,uint8_t Pic_YSIZE,const uint8_t *Pic,unsigned char forecolor,unsigned char background);
    void (*StringMov)(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*MemFibMov)(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size,uint8_t b_n,unsigned char forecolor,unsigned char background);
    void (*Logo)(void);
} OLEDDriver;

extern OLEDDriver disp;

#define B 0X01
#define N 0
//-----------------测试LED端口定义---------------- 



//-----------------OLED端口定义---------------- 

//#define OLED_SCLK_Clr() HAL_GPIO_WritePin(LCD_SCK_GPIO_Port,LCD_SCK_Pin,GPIO_PIN_RESET)
//#define OLED_SCLK_Set() HAL_GPIO_WritePin(LCD_SCK_GPIO_Port,LCD_SCK_Pin,GPIO_PIN_SET)

//#define OLED_SDIN_Clr() HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_RESET)//DIN
//#define OLED_SDIN_Set() HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_SET)

#define LCD_SCLK_Set()  LCD_SCK_GPIO_Port->BSRR = 	LCD_SCK_Pin
#define LCD_SCLK_Clr()  LCD_SCK_GPIO_Port->BSRR = (uint32_t)LCD_SCK_Pin << 16U

#define LCD_MOSI_Set() 	LCD_MOSI_GPIO_Port->BSRR = LCD_MOSI_Pin
#define LCD_MOSI_Clr() 	LCD_MOSI_GPIO_Port->BSRR = (uint32_t)LCD_MOSI_Pin << 16U

#define OLED_RST_Clr() 	HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_RESET)//RES
#define OLED_RST_Set() 	HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_SET)

#define OLED_DC_Clr() 	HAL_GPIO_WritePin(LCD_A0_GPIO_Port,LCD_A0_Pin,GPIO_PIN_RESET)//DC
#define OLED_DC_Set() 	HAL_GPIO_WritePin(LCD_A0_GPIO_Port,LCD_A0_Pin,GPIO_PIN_SET)
 		     
#define OLED_CS_Clr()  	HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port,SPI2_NSS_Pin,GPIO_PIN_RESET)//CS
#define OLED_CS_Set()  	HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port,SPI2_NSS_Pin,GPIO_PIN_SET)

#define OLED_BLK_Clr()  
#define OLED_BLK_Set() 

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
u8 read_id_lcd(u8 reg);
u8 LCD_Read_Bus(u8 size);

extern  u16 BACK_COLOR;   //背景色

void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);
void Lcd_Init(void);
void LCD_Clear(u16 Color);
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void LCD_DrawPoint_big(u16 x,u16 y,u16 color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color);
u32 mypow(u8 m,u8 n);
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color);
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color);
void LCD_ShowPicture(u16 x1,u16 y1,u16 x2,u16 y2);
void SetBL(int bl);
void lcd_res(void);
void Lcd_Show_Img(uint8_t x,uint8_t y,uint8_t Pic_XSIZE,uint8_t Pic_YSIZE,const uint8_t *Pic,unsigned int forecolor,unsigned int background);
//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define VIOLET  		 0x881F	 	//紫
#define FEN  		 	 0xa3da	 	//紫
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


					  		 
#endif  
	 
	 



