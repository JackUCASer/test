
////////////////////////////////////////////////////////////////////
/// @file Multiple_Styles.c
/// @brief 用户自定义的GUI界面程序
/// 
/// 文件详细描述：	
///					
///
///
///
///
/// @author 王昌盛
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>修改历史：--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// 公司：常州赛乐医疗技术有限公司
////////////////////////////////////////////////////////////////////

#include "Multiple_Styles.h"
#include "lvgl/lvgl.h"
#include "DataStorage.h"
#include "lcd.h"
#include "Menu.h"
#include "Beep.h"
#include "MenuData.h"
#include "MenuFunc.h"
#include "work_mode.h"
#include "Error.h"
#include "apex.h"

lv_obj_t *	sel_top;
extern WorkMode_e gTempMode;
extern uint8_t KeyFuncIndex;

uint8_t Changing_cnt=0;
/**********************************************************************************************************
*	函 数 名: void DispLogo()
*	功能说明: LOGO显示
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void DispLogo(void)
{

    /*Create an object with the new style*/
    lv_obj_t * Logo = lv_img_create(lv_scr_act());
	
    LV_IMG_DECLARE(eighteeth);
    lv_img_set_src(Logo, &eighteeth);
	lv_img_set_zoom(Logo,LV_IMG_ZOOM_NONE);										//图片放大、缩小设置。256位1倍。128缩小50%。281放大10%。
	lv_img_set_angle(Logo,0);													//图片旋转。458为45.8°。角度精度为0.1°。
	lv_obj_center(Logo);
}

/**********************************************************************************************************
*	函 数 名: void DispSetting(void)
*	功能说明: LOGO显示
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void DispSetting(void)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	lv_obj_t* textFac=lv_label_create(sel_top);
	if(set.Language==EN){
		lv_label_set_text(textFac,"Setting");
	}
	else if(set.Language==CN){
		lv_label_set_text(textFac,CN_GAO CN_JI2 CN_SHE CN_ZHI);
	}
	lv_obj_align(textFac,LV_ALIGN_CENTER,0,0);				//设置文字位置	
}

/**********************************************************************************************************
*	函 数 名: void DispFactory(void)
*	功能说明: 显示“Factory Mode”标签
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void DispFactory(void)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	lv_obj_t* textFac=lv_label_create(sel_top);
	if(set.Language==EN){
		lv_label_set_text(textFac,"Factory");
	}
	else if(set.Language==CN){
		lv_label_set_text(textFac,CN_GONG CN_CHANG CN_SHE CN_ZHI);
	}
	lv_obj_align(textFac,LV_ALIGN_CENTER,0,0);				//设置文字位置
	
}
extern uint8_t Num;
extern uint8_t pw[4];
extern uint8_t speed_cnt;
extern uint8_t Torque_cnt;
/**********************************************************************************************************
*	函 数 名: void DispPassword(void)
*	功能说明: 显示密码
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void Disp_cal(void)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_Cal_theme;
	lv_style_reset(&style_Cal_theme);
	lv_style_init(&style_Cal_theme);
	lv_style_set_radius(&style_Cal_theme, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_Cal_theme, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE,3));											//设置上背景颜色
	lv_style_set_bg_grad_color(&style_Cal_theme, lv_palette_darken(LV_PALETTE_BLUE,3));
	lv_style_set_bg_grad_dir(&style_Cal_theme, LV_GRAD_DIR_HOR);
	
	lv_style_set_border_width(&style_Cal_theme, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_Cal_theme, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_Cal_theme, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_Cal_theme, 160);																				//设置样式宽度
	lv_style_set_height(&style_Cal_theme, 23);																				//设置样式高度
	
	static lv_style_t 	style_Cal_data;
	lv_style_reset(&style_Cal_data);
	lv_style_init(&style_Cal_data);
	lv_style_set_radius(&style_Cal_data, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_Cal_data, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_Cal_data, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_Cal_data, lv_color_black());																//设置样式中文字颜色
	lv_style_set_text_font(&style_Cal_data, &lv_font_yahei_16_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_Cal_data, 160);																				//设置样式宽度
	lv_style_set_height(&style_Cal_data, 57);	
	
	//设置一个主题显示按钮
	lv_obj_t * text_Cal_theme  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_Cal_theme , &style_Cal_theme, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_Cal_theme ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_Cal_theme , LV_ALIGN_TOP_MID, 0, 0);																	//设置目标位置	
	
	lv_obj_t * text_Cal_data  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_Cal_data , &style_Cal_data, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_Cal_data ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_Cal_data , LV_ALIGN_BOTTOM_MID, 0, 0);																//设置目标位置
	
	lv_obj_t * theme_label = lv_label_create(text_Cal_theme);																//创建标签   theme_label
	lv_obj_align(theme_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置	
	
	lv_obj_t * data_label = lv_label_create(text_Cal_data);																	//创建标签   data_label
	lv_obj_align(data_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置
	
	if(Cal_mode_flag == Input_Password_flag)
	{
		/**************主题名称：输入密码********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Password:");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_QING CN_SHU2 CN_RU CN_MI CN_MA);
		}
		
		/**************数据内容，输入密码**********************************/
		lv_label_set_text(data_label," ");
		if(Num > 0)
		{
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispPassword[pw[0]-1]);	
		}
		if(Num > 1)
		{
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST," ");
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispPassword[pw[1]-1]);
		}
		if(Num > 2)
		{
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST," ");
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispPassword[pw[2]-1]);
		}
		if(Num > 3)
		{
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST," ");
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispPassword[pw[3]-1]);
		}		
	}else if(Cal_mode_flag == Error_Password_flag)
	{
		/**************主题名称：密码错误********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Password:");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_QING CN_SHU2 CN_RU CN_MI CN_MA);
		}
		/**************数据内容，密码错误**********************************/
		if(set.Language == EN)
		{
			lv_label_set_text(data_label,"Error Password");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(data_label,CN_MI CN_MA CN_CUO CN_WU);
		}
	}else if(Cal_mode_flag == AutoThetaBias_flag)
	{
		/**************主题名称：电机磁编校正********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"AutoThetaBias");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_DIAN1 CN_JI1 CN_JIAO CN_DU1 CN_XIAO CN_ZHUN);
		}
		
		/**************数据内容，电机磁编校正**********************************/
		if(set.Language == EN)
		{
			lv_label_set_text(data_label,"Running");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(data_label,CN_JIAO CN_DU1 CN_XIAO CN_ZHUN CN_ZHONG "...");
		}
		
	}else if(Cal_mode_flag == ThetaBiasFish_flag)
	{
		/**************主题名称：电机磁编校正进行中********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"AutoThetaBias");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_DIAN1 CN_JI1 CN_JIAO CN_DU1 CN_XIAO CN_ZHUN);
		}
		
		/**************数据内容，电机磁编校正进行中**********************************/
		if(set.Language == EN)
		{
			lv_label_set_text(data_label,"Finish!");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(data_label,CN_WAN CN_CHENG1 CN_GANTAN);
		}
	}else if(Cal_mode_flag == Cal_Noload_flag)	
	{
		/**************主题名称：空载电流测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"No load current Test");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_KONG CN_ZAI2 CN_DIAN1 CN_LIU CN_CE CN_SHI4);
		}
		
		/**************数据内容：空载电流测试**********************************/
		if(set.Language == EN)
		{
			lv_label_set_text(data_label,"Press " CN_DIAN " to start");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(data_label,CN_AN " " CN_DIAN " " CN_KAI CN_SHI1);
		}			
	}else if(Cal_mode_flag == Cal_Noload_Run_flag)	
	{
		/**************主题名称：空载电流测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Speed Test");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_ZHUAN CN_SU CN_CE CN_SHI4);
		}
		
		/**************数据内容：空载电流测试**********************************/
		if(speed_cnt < 32)
		{
			lv_label_set_text(data_label,DispEnSpeedData[speed_cnt]);
		}else
		{
			lv_label_set_text(data_label,"-");
			lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispEnSpeedData[speed_cnt-32]);
		}
		lv_label_ins_text(data_label,LV_LABEL_POS_LAST,"rpm");	
	}else if(Cal_mode_flag == Cal_Torque_flag)	
	{
		/**************主题名称：扭矩测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Torque Test");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_NIU CN_JU CN_CE CN_SHI4);
		}
		
		/**************数据内容：扭矩测试**********************************/
		if(set.Language == EN)
		{
			lv_label_set_text(data_label,"Press " CN_DIAN " to start");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(data_label,CN_AN " " CN_DIAN " " CN_KAI CN_SHI1);
		}
	}else if(Cal_mode_flag == Cal_Torque_Run_flag)	
	{
		/**************主题名称：扭矩测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Torque Test");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_NIU CN_JU CN_CE CN_SHI4);
		}
		/**************数据内容：空载电流测试**********************************/
		lv_label_set_text(data_label,DispEnTorqueData[Torque_cnt]);
		lv_label_ins_text(data_label,LV_LABEL_POS_LAST,"N"CN_DIAN"cm");	
	}else if(Cal_mode_flag == Cal_Torque_Run1_flag)
	{
				/**************主题名称：扭矩测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Torque Test");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_NIU CN_JU CN_CE CN_SHI4);
		}
		/**************数据内容：空载电流测试**********************************/
		lv_label_set_text(data_label,DispEnTorqueData[Torque_cnt]);
		lv_label_ins_text(data_label,LV_LABEL_POS_LAST,"N"CN_DIAN"cm");
		lv_label_ins_text(data_label,LV_LABEL_POS_LAST,"\n");
		lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispEnSpeedData[speed_cnt]);
		lv_label_ins_text(data_label,LV_LABEL_POS_LAST,"rpm");	
	}else if(Cal_mode_flag == Cal_Saving)
	{
				/**************主题名称：扭矩测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Torque Test");
			lv_label_set_text(data_label,"Saving ...");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_NIU CN_JU CN_CE CN_SHI4);
			lv_label_set_text(data_label,CN_CUN CN_CHU3 CN_ZHONG "...");
		}	
	}if(Cal_mode_flag == Cal_Fishing)
	{
				/**************主题名称：扭矩测试********************************************/
		if(set.Language == EN)
		{
			lv_label_set_text(theme_label,"Torque Test");
			lv_label_set_text(data_label,"Completed");
		}
		else if(set.Language == CN)
		{
			lv_label_set_text(theme_label,CN_NIU CN_JU CN_CE CN_SHI4);
			lv_label_set_text(data_label,CN_XIAO CN_ZHUN CN_WAN CN_CHENG1);
		}	
	}
}

/********************************与电量相关的全局变量*******************************************/
//电池，bat....
lv_style_t 	style_bat;											//bat的样式
lv_obj_t *	text_bat;											//设置一个bat的文件按钮
lv_obj_t * 	bat_label;											//bat的文件按钮上显示的文字
/**********************************************************************************************************
*	函 数 名: void DispBatery_init(uint8_t Sw, int Battery)
*	功能说明: 显示“电量”标签
*	形    参: uint8_t Sw, int Battery  Sw为开启或关闭电量显示，Battery为电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void DispBatery_init(uint8_t Sw, int Battery, int Charging)
{
	if(Sw == 1)
	{
		lv_style_reset(&style_bat);
		lv_style_init(&style_bat);
		lv_style_set_bg_opa(&style_bat, LV_OPA_0);																		//设置背景透明度
		lv_style_set_shadow_opa(&style_bat, LV_OPA_0);																	//设置阴影透明度
		lv_style_set_border_opa(&style_bat, LV_OPA_0);																	//设置边界透明度
		if((Charging == 0) && (Battery == 0))
		{
			lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_RED));										//设置样式中文字颜色	
		}
		else  if((Charging == 0) && ((Battery > 0)&&(Battery <= 1)))
		{	
			lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_YELLOW));
		}else  if((Charging == 0) && (Battery > 1))
		{	
			lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_GREEN));
		}
		else if(Charging == 1)
		{
			lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_GREEN));
		}
//		lv_style_set_text_font(&style_bat, &lv_font_montserrat_20);														//设置样式中文字大小
		lv_style_set_text_font(&style_bat, &lv_font_yahei_16_bpp4);														//设置样式中文字大小
		lv_style_set_width(&style_bat, 36);																				//设置样式宽度
		lv_style_set_height(&style_bat, 25);																			//设置样式高度
		
		text_bat  = lv_obj_create(lv_scr_act());																		//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(text_bat , &style_bat, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_bat ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
		lv_obj_align(text_bat,LV_ALIGN_TOP_RIGHT,0,0);																	//设置符号位置,显示符号

		bat_label = lv_label_create(text_bat);										//创建标签   bat_label
		
		if(Charging == 1)
		{
			if(Battery > 0)
			{
				Battery --;
			}

			if((Changing_cnt + Battery) > 4)
			{
				Changing_cnt = 0;
			}else
			{
				Battery += Changing_cnt;
				Changing_cnt++;
			}
		}else
		{
			Changing_cnt = 0;
		}
		
		switch(Battery)
		{
			case 0:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_EMPTY);		break;	
			case 1:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_1);			break;
			case 2:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_2);			break;
			case 3:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_3);			break;
			case 4:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_FULL);		break;
			default: break;		
		}
		lv_obj_center(bat_label);																//设置文字位置
	}
}

/**********************************************************************************************************
*	函 数 名: void DispBatery_charge(uint8_t Sw, int Battery, int Charging)
*	功能说明: 显示“电量”标签,当电量改变时，调用此函数
*	形    参: uint8_t Sw, int Battery  Sw为开启或关闭电量显示，Battery为电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void DispBatery_charge(uint8_t Sw, int Battery, int Charging)
{
	lv_obj_del(text_bat);
	if(Sw == 1)
	{
		if(Charging == 0)
		{
			if(Battery == 0)
			{
				lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_RED));
			}
			else if( Battery == 1 )
			{
				lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_YELLOW));
			}else if(( Battery > 1 ) && (Battery <= 4))
			{
				lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_GREEN));
			}
		}else 
		{			
			lv_style_set_text_color(&style_bat, lv_palette_main(LV_PALETTE_GREEN));
		}
			
		text_bat  = lv_obj_create(lv_scr_act());																		//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(text_bat , &style_bat, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_bat ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
		lv_obj_align(text_bat,LV_ALIGN_TOP_RIGHT,0,0);																	//设置符号位置,显示符号

		bat_label = lv_label_create(text_bat);																			//创建标签   bat_label
		
		if(Charging == 1)
		{
			if(sys.FullCharge == 0)																						//未充满电，显示充电状态
			{
				if(Battery > 0)
				{
					Battery --;
				}

				if((Changing_cnt + Battery) > 4)
				{
					Changing_cnt = 0;
				}else
				{
					Battery += Changing_cnt;
					Changing_cnt++;
				}
			}else
			{
				Battery = 4;	
			}
		}else
		{
			Changing_cnt = 0;
		}
	
		switch(Battery)
		{
			case 0:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_EMPTY);		break;	
			case 1:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_1);			break;
			case 2:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_2);			break;
			case 3:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_3);			break;
			case 4:	lv_label_set_text(bat_label,LV_SYMBOL_BATTERY_FULL);		break;
			default: break;		
		}
		lv_obj_center(bat_label);																//设置文字位置
	}
}

/**********************************************************************************************************
*	函 数 名: void DispVol(uint8_t Sw, int Volume)
*	功能说明: 显示“音量”标签
*	形    参: uint8_t Sw, int Volume  Sw为开启或关闭电量显示，Battery为电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void DispVol(uint8_t Sw, int Volume)
{
	if(Sw == 1)
	{
		static lv_style_t style_vol;
		lv_style_reset(&style_vol);
		lv_style_init(&style_vol);
		lv_style_set_text_color(&style_vol, lv_palette_lighten(LV_PALETTE_GREEN,1));
		lv_style_set_text_font(&style_vol, &lv_font_yahei_16_bpp4);								//设置样式中文字大小
		
		lv_obj_t * vol = lv_label_create(lv_scr_act());
		lv_obj_add_style(vol, &style_vol, 0);
		
		switch(Volume)
		{
			case 0:	lv_label_set_text(vol,LV_SYMBOL_MUTE);					break;	
			case 1:	lv_label_set_text(vol,LV_SYMBOL_VOLUME_MID);			break;
			case 2:	lv_label_set_text(vol,LV_SYMBOL_VOLUME_MAX);			break;
			case 3:	lv_label_set_text(vol,LV_SYMBOL_VOLUME_FILL);			break;
			default: break;		
		}
		lv_obj_align(vol,LV_ALIGN_TOP_RIGHT,-39,-2);				//设置符号位置,显示符号
	}
}


/********************************与待机界面相关的全局变量**********************************/


/**********************************************************************************************************
*	函 数 名: void Standy_init(void)
*	功能说明: 显示主菜单,待机模式调用
*	形    参: int memstr, FUN_SEL sel		memstr文件存储指针   sel设置缓存
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void Standy_init(int memstr, FUN_SEL sel)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_seqn;																							//设置锉号背景颜色
	lv_style_reset(&style_seqn);																							//必须复位，不然会出现内存泄漏
	lv_style_init(&style_seqn);
	lv_style_set_border_width(&style_seqn, 0);																				//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_seqn, 0);																				//设置阴影边框宽度   宽度为0，无边框
	if(sel.color == c_white)																								//如果锉的颜色为白色，则用黑色字体
	{
		lv_style_set_text_color(&style_seqn, lv_color_black());																//设置样式中文字颜色
	}	
	else
	{
		lv_style_set_text_color(&style_seqn, lv_color_white());																//设置样式中文字颜色
	}		
	if(sel.FileSave == 0)
	{
		lv_style_set_text_font(&style_seqn, &lv_font_yahei_16_bpp4);														//设置样式中文字大小
	}else if(sel.FileSave == 1)
	{
		lv_style_set_text_font(&style_seqn, &lv_font_yahei_12_bpp4);	
	}else if(sel.FileSave == 2)
	{
		if(Seq_Inside[sel.FileLibrary].fseq[sel.FileSeq].seqName == 0)
		{
			lv_style_set_text_font(&style_seqn, &lv_font_yahei_12_bpp4);
		}else
		{
			lv_style_set_text_font(&style_seqn, &lv_font_yahei_16_bpp4);
		}	
	}
	lv_style_set_width(&style_seqn, 56);																					//设置样式宽度
	lv_style_set_height(&style_seqn, 58);																					//设置样式高度
	lv_style_set_radius(&style_seqn, 0);																					//设置边框圆角
//	Color_select(sel.color, style_seqn);																					//设置颜色
	switch(sel.color)
	{
		case c_white:	{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_GREY,5));						//设置白色
							break;
						}
		case c_hong:	{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_RED,1));						//设置红色
							break;
						}
		case c_zong:	{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_BROWN,1));						//设置棕色
							break;
						}
		case c_huang:	{
							lv_style_set_bg_color(&style_seqn, lv_palette_darken(LV_PALETTE_YELLOW,1));						//设置黄色
							break;
						}
		case c_lv:		{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_GREEN,1));						//设置绿色
							break;
						}
		case c_fen:		{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_PINK,1));						//设置粉红
							break;
						}
		case c_lan:		{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_BLUE,1));						//设置蓝色
							break;
						}
		case c_zi:		{
							lv_style_set_bg_color(&style_seqn, lv_palette_lighten(LV_PALETTE_PURPLE,1));					//设置紫色
							break;
						}
		case c_hei:		{
							lv_style_set_bg_color(&style_seqn, lv_color_black());											//设置黑色
							break;
						}
		case c_hui:		{
							lv_style_set_bg_color(&style_seqn, lv_palette_darken(LV_PALETTE_GREY,1));						//设置灰色
							break;
						}
		default: 		break;		
	}
	//设置背景块
	lv_obj_t * bg_seqn = lv_obj_create(lv_scr_act());																		//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(bg_seqn , &style_seqn, 0);																				//加载一种样式
	lv_obj_set_scrollbar_mode(bg_seqn ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	lv_obj_align(bg_seqn , LV_ALIGN_BOTTOM_LEFT, 0, 0);																		//设置目标位置
	
/*******************************锉号，seqn....****************************************************/	
	lv_obj_t * seqn_label = lv_label_create(bg_seqn);																		//创建标签   seqn_label
	if(sel.FileSave == 0)
	{
		lv_label_set_text(seqn_label, DispEnMem[mem.MemoryNum]);																//设置动态文本(字符串形式)
		lv_obj_align(seqn_label , LV_ALIGN_TOP_RIGHT, 6, -12);																	//设置目标位置
	}else if(sel.FileSave == 1)
	{
		char buf[8];
		lv_snprintf(buf, sizeof(buf), "%d%", sel.taper);
		if(sel.taper < 10)
		{
			lv_label_set_text(seqn_label,"0");
			lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, buf);
		}else
		{
			lv_label_set_text(seqn_label,buf);
		}
		lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, "/");																	//设置动态文本(字符串形式)	
		lv_snprintf(buf, sizeof(buf), "%d%", sel.number);
		if(sel.number < 10)
		{
			lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, "0");																//设置动态文本(字符串形式)
			lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)
		}else
		{
			lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)
		}	
		lv_obj_align(seqn_label , LV_ALIGN_TOP_RIGHT, 6, -8);																	//设置目标位置
	}else if(sel.FileSave == 2)
	{
		if(Seq_Inside[sel.FileLibrary].fseq[sel.FileSeq].seqName == 0)
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", sel.taper);
			if(sel.taper < 10)
			{
				lv_label_set_text(seqn_label,"0");
				lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, buf);
			}else
			{
				lv_label_set_text(seqn_label,buf);
			}
			lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, "/");																	//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", sel.number);
			if(sel.number < 10)
			{
				lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, "0");																//设置动态文本(字符串形式)
				lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)
			}else
			{
				lv_label_ins_text(seqn_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)
			}
			lv_obj_align(seqn_label , LV_ALIGN_TOP_RIGHT, 6, -8);																	//设置目标位置
		}else
		{
			lv_label_set_text(seqn_label,Seq_Inside[sel.FileLibrary].fseq[sel.FileSeq].seqName);
			lv_obj_align(seqn_label , LV_ALIGN_TOP_RIGHT, 6, -12);																	//设置目标位置
		}
	}
	
//	lv_label_set_text(seqn_label, "04/20");																					//设置动态文本(字符串形式)
	
	lv_obj_t *	standy_seqn = lv_img_create(sel_top);																		//设置锉号显示底图
	LV_IMG_DECLARE(my_standy_hei);
	lv_img_set_src(standy_seqn, &my_standy_hei);
	lv_obj_align(standy_seqn , LV_ALIGN_TOP_LEFT, 0, 23);																	//设置目标位置


/*******************************文件名，myfile....****************************************************/	
	static lv_style_t 	style_file;
	lv_style_reset(&style_file);
	lv_style_init(&style_file);
	lv_style_set_radius(&style_file, 0);																					//设置边框圆角
	lv_style_set_bg_color(&style_file, lv_palette_lighten(LV_PALETTE_GREY,3));												//设置上背景颜色
	lv_style_set_border_width(&style_file, 0);																				//设置边框宽度
	lv_style_set_text_color(&style_file, lv_color_black());																	//设置样式中文字颜色		
	lv_style_set_text_font(&style_file, &lv_font_yahei_12_bpp4);															//设置样式中文字大小
	lv_style_set_width(&style_file, 160);																					//设置样式宽度
	lv_style_set_height(&style_file, 23);																					//设置样式高度
	
	//设置一个file的文件按钮
	lv_obj_t * text_file  = lv_obj_create(lv_scr_act());																	//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_file , &style_file, 0);																			//加载一种样式
	lv_obj_set_scrollbar_mode(text_file ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
	lv_obj_align(text_file , LV_ALIGN_TOP_LEFT, 0, 0);																		//设置目标位置	
	
	//file的文件按钮上显示的文字
	lv_obj_t * file_label = lv_label_create(text_file);																		//创建标签   file_label
	lv_label_set_long_mode(file_label,LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(file_label,80);
	if(sel.FileSave == 0)
	{
		if(set.Language == EN)
		{
			lv_label_set_text(file_label, DispENFileName[mem.MemoryNum]);															//设置动态文本(字符串形式)
		}else
		{
			lv_label_set_text(file_label, DispCNFileName[mem.MemoryNum]);															//设置动态文本(字符串形式)
		}
	}else if(sel.FileSave == 1)
	{
		lv_label_set_text(file_label, ReStandyName[mem.MemoryNum-1]);															//设置动态文本(字符串形式)
	}else if(sel.FileSave == 2)
	{
		lv_label_set_text(file_label, Seq_Inside[sel.FileLibrary].Name);	
	}
	lv_obj_align(file_label , LV_ALIGN_LEFT_MID, -2, -2);																	//设置目标位置
	if(sel.OperaMode != 4 )	
	{
	/*******************************参数显示，sel....****************************************************/	
		static lv_style_t 	style_sel;
		lv_style_reset(&style_sel);
		lv_style_init(&style_sel);
		lv_style_set_radius(&style_sel, 0);																					//设置边框圆角
		lv_style_set_bg_color(&style_sel, lv_color_white());																//设置上背景颜色
		lv_style_set_border_width(&style_sel, 0);																			//设置边框宽度
		lv_style_set_text_color(&style_sel, lv_color_black());																//设置样式中文字颜色		
		lv_style_set_text_font(&style_sel, &lv_font_yahei_16_bpp4);															//设置样式中文字大小
		lv_style_set_width(&style_sel, 104);																				//设置样式宽度
		lv_style_set_height(&style_sel, 57);																				//设置样式高度

		//设置一个参数显示的文件按钮
		lv_obj_t * text_sel  = lv_obj_create(lv_scr_act());																	//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(text_sel , &style_sel, 0);																			//加载一种样式
		lv_obj_set_scrollbar_mode(text_sel ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_sel , LV_ALIGN_BOTTOM_RIGHT, 0, 0);																//设置目标位置

	/*******************************转速，rpm....****************************************************/
		//rpm的文件按钮上显示的单位文字
		lv_obj_t *rpm_label = lv_label_create(text_sel);																	//创建标签   rpm_DW_label

		if((sel.OperaMode == 0) || (sel.OperaMode == 1))
		{
			lv_label_set_text(rpm_label, DispEnSpeedData[sel.Speed]);														//设置动态文本(字符串形式)
			lv_label_ins_text(rpm_label, LV_LABEL_POS_LAST, " rpm");															//设置动态文本(字符串形式)
			lv_obj_align(rpm_label , LV_ALIGN_TOP_RIGHT, 8, -13);																//设置目标位置
		}
		else if(sel.OperaMode == 2)
		{
			if(set.Language == EN)
			{
				lv_label_set_text(rpm_label, DispEnRecSpeedData[sel.RecSpeed]);												//设置动态文本(字符串形式)
				lv_label_ins_text(rpm_label, LV_LABEL_POS_LAST, " rpm");													//设置动态文本(字符串形式)
				lv_obj_align(rpm_label , LV_ALIGN_TOP_RIGHT, 8, -13);																//设置目标位置
			}else
			{
				lv_label_set_text(rpm_label, DispCNRecSpeedData[sel.RecSpeed]);												//设置动态文本(字符串形式)
				lv_obj_align(rpm_label , LV_ALIGN_TOP_RIGHT, -15, -13);																//设置目标位置
			}
		}else if(sel.OperaMode == 3)
		{
			lv_label_set_text(rpm_label, DispEnRecSpeedData[sel.RecSpeed]);												//设置动态文本(字符串形式)
			lv_label_ins_text(rpm_label, LV_LABEL_POS_LAST, " rpm");													//设置动态文本(字符串形式)
			lv_obj_align(rpm_label , LV_ALIGN_TOP_RIGHT, 8, -13);																//设置目标位置
		}
		
		
		//画边框		
		static lv_style_t style;
		lv_style_reset(&style);
		lv_style_init(&style);
		lv_style_set_line_color(&style, lv_palette_darken(LV_PALETTE_GREY,1));
//		lv_style_set_line_opa(&style,100);
		lv_style_set_line_width(&style, 1);
		lv_style_set_line_rounded(&style, true);
		
		static lv_point_t p1[] = {{0, 0}, {0, 5}, {5, 5}};
		static lv_point_t p2[] = {{0, 5}, {0, 0}, {5, 0}};
		static lv_point_t p3[] = {{0, 0}, {5, 0}, {5, 5}};
		static lv_point_t p4[] = {{0, 5}, {5, 5}, {5, 0}};
		
		if(sel.FileSave != 0)
		{
			if((sel.OperaMode == 0) || (sel.OperaMode == 1))
			{	
				if(sel.Speed != sel.Vaule1)	
				{					

					
					lv_obj_t * Line1 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line1, &style, 0);
					lv_line_set_points(Line1, p1, 3);
						
					lv_obj_t * Line2 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line2, &style, 0);
					lv_line_set_points(Line2, p2, 3);
					
					if(sel.Speed <= 5)
					{
						lv_obj_align(Line1 , LV_ALIGN_CENTER, 4, 7);
						lv_obj_align(Line2 , LV_ALIGN_CENTER, 4, -12);
					}else if((sel.Speed <= 25) && (sel.Speed >5))
					{
						lv_obj_align(Line1 , LV_ALIGN_CENTER, -8, 7);
						lv_obj_align(Line2 , LV_ALIGN_CENTER, -8, -12);
					}else
					{
						lv_obj_align(Line1 , LV_ALIGN_CENTER, -18, 7);
						lv_obj_align(Line2 , LV_ALIGN_CENTER, -18, -12);
					}
						
					lv_obj_t * Line3 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line3, &style, 0);
					lv_line_set_points(Line3, p3, 3);
					
					lv_obj_align(Line3 , LV_ALIGN_CENTER, 74, -12);
					
					lv_obj_t * Line4 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line4, &style, 0);
					lv_line_set_points(Line4, p4, 3);
					
					lv_obj_align(Line4 , LV_ALIGN_CENTER, 74, 7);
				}
			}
		}


	/*******************************转矩或角度，tq....****************************************************/	
		//tq的文件按钮上显示的单位文字
		lv_obj_t * tq_cw_label = lv_label_create(text_sel);																	//创建标签   tq_label
		if((sel.OperaMode == 0) || (sel.OperaMode == 1))
		{
			lv_label_set_text(tq_cw_label, DispEnTorqueData[sel.Torque]);													//设置动态文本(字符串形式)
			lv_label_ins_text(tq_cw_label,LV_LABEL_POS_LAST," N"CN_DIAN"cm");
		}
		else if((sel.OperaMode == 2) || (sel.OperaMode == 3))
		{		
			lv_label_set_text(tq_cw_label, DispEnAngleCWData[sel.AngleCW]);													//正角度
			lv_label_ins_text(tq_cw_label,LV_LABEL_POS_LAST,CN_DU"/");
			lv_label_ins_text(tq_cw_label,LV_LABEL_POS_LAST,DispEnAngleCWData[sel.AngleCCW]);								//负角度
			lv_label_ins_text(tq_cw_label,LV_LABEL_POS_LAST,CN_DU);
		}
		lv_obj_align(tq_cw_label, LV_ALIGN_BOTTOM_RIGHT, 8, 13);															//设置目标位置
		
		if(sel.FileSave != 0)
		{
			if((sel.OperaMode == 0) || (sel.OperaMode == 1))
			{	
				if(sel.Torque != sel.Vaule2)	
				{					
					lv_obj_t * Line5 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line5, &style, 0);
					lv_line_set_points(Line5, p1, 3);
					lv_obj_align(Line5 , LV_ALIGN_CENTER, -14, 36);
						
					lv_obj_t * Line6 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line6, &style, 0);
					lv_line_set_points(Line6, p2, 3);
					lv_obj_align(Line6 , LV_ALIGN_CENTER, -14, 17);
						
					lv_obj_t * Line7 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line7, &style, 0);
					lv_line_set_points(Line7, p3, 3);
					
					lv_obj_align(Line7 , LV_ALIGN_CENTER, 74, 17);
					
					lv_obj_t * Line8 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line8, &style, 0);
					lv_line_set_points(Line8, p4, 3);
					
					lv_obj_align(Line8 , LV_ALIGN_CENTER, 74, 36);
				}
			}else if((sel.OperaMode == 2) || (sel.OperaMode == 3))
			{
				if((sel.AngleCW != sel.Vaule1) || (sel.AngleCCW != sel.Vaule2))
				{
					lv_obj_t * Line5 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line5, &style, 0);
					lv_line_set_points(Line5, p1, 3);
						
					lv_obj_t * Line6 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line6, &style, 0);
					lv_line_set_points(Line6, p2, 3);
					if((sel.AngleCW <= 13)&&(sel.AngleCCW <= 13))
					{
						lv_obj_align(Line5 , LV_ALIGN_CENTER, -5, 36);
						lv_obj_align(Line6 , LV_ALIGN_CENTER, -5, 17);		
					}else if((sel.AngleCW > 13)&&(sel.AngleCCW > 13))
					{
						lv_obj_align(Line5 , LV_ALIGN_CENTER, -25, 36);
						lv_obj_align(Line6 , LV_ALIGN_CENTER, -25, 17);	
						
					}else
					{
						lv_obj_align(Line5 , LV_ALIGN_CENTER, -14, 36);
						lv_obj_align(Line6 , LV_ALIGN_CENTER, -14, 17);		
					}
	
					lv_obj_t * Line7 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line7, &style, 0);
					lv_line_set_points(Line7, p3, 3);
					
					lv_obj_align(Line7 , LV_ALIGN_CENTER, 74, 17);
					
					lv_obj_t * Line8 = lv_line_create(lv_scr_act());
					lv_obj_add_style(Line8, &style, 0);
					lv_line_set_points(Line8, p4, 3);
					
					lv_obj_align(Line8 , LV_ALIGN_CENTER, 74, 36);					
				}		
			}
		}
		
	}else
	{
		Apex_init(sel);		
	}
/*******************************工作模式，motion....****************************************************/
	static lv_style_t 	style_motion;
	lv_style_reset(&style_motion);
	lv_style_init(&style_motion);
	lv_style_set_radius(&style_motion, 0);																					//设置边框圆角
	lv_style_set_border_width(&style_motion, 0);																			//设置边框宽度
	lv_style_set_bg_opa(&style_motion, LV_OPA_0);																			//设置背景透明度
	lv_style_set_shadow_opa(&style_motion, LV_OPA_0);																		//设置阴影透明度
	lv_style_set_border_opa(&style_motion, LV_OPA_0);																		//设置边界透明度
	lv_style_set_text_color(&style_motion, lv_color_black());																//设置样式中文字颜色
	if((sel.OperaMode == 4)||(set.Language == CN))
	{
		if(sel.OperaMode == 4)
		{
			lv_style_set_text_font(&style_motion, &lv_font_yahei_16_bpp4);	
		}else
		{
			lv_style_set_text_font(&style_motion, &lv_font_yahei_12_bpp4);	
		}
	}else
	{
		lv_style_set_text_font(&style_motion, &my_font_standy_mode);
	}
	lv_style_set_width(&style_motion, 48);																					//设置样式宽度
	lv_style_set_height(&style_motion, 30);																					//设置样式高度

	//设置一个seqn的文件按钮
	lv_obj_t * text_motion  = lv_obj_create(lv_scr_act());																	//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_motion , &style_motion, 0);																		//加载一种样式
	lv_obj_set_scrollbar_mode(text_motion ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
	lv_obj_align(text_motion , LV_ALIGN_BOTTOM_LEFT, 5, 0);																	//设置目标位置
	
	lv_obj_t * motion_label = lv_label_create(text_motion);																	//创建标签   seqn_label
	if(set.Language == EN)
	{
		lv_label_set_text(motion_label, DispEnSelOperationMode[sel.OperaMode]);													//设置动态文本(字符串形式)
	}else
	{
		lv_label_set_text(motion_label, DispCNSelOperationMode[sel.OperaMode]);													//设置动态文本(字符串形式)
	}
	lv_obj_align(motion_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置	
}

/**********************************************************************************************************
*	函 数 名: void Standy_sel_init(FUN_SEL sel)
*	功能说明: 待机模式的设置参数样式初始化
*	形    参: 无	 
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void Standy_sel_init(FUN_SEL sel)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_sel_theme;
	lv_style_reset(&style_sel_theme);
	lv_style_init(&style_sel_theme);
	lv_style_set_radius(&style_sel_theme, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_sel_theme, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE,3));											//设置上背景颜色
	lv_style_set_bg_grad_color(&style_sel_theme, lv_palette_darken(LV_PALETTE_BLUE,3));
    lv_style_set_bg_grad_dir(&style_sel_theme, LV_GRAD_DIR_HOR);
	
	lv_style_set_border_width(&style_sel_theme, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_sel_theme, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_sel_theme, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_sel_theme, 160);																				//设置样式宽度
	lv_style_set_height(&style_sel_theme, 23);																				//设置样式高度
	
	//设置一个主题显示按钮
	lv_obj_t * text_sel_theme  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_sel_theme , &style_sel_theme, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_sel_theme ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_sel_theme , LV_ALIGN_TOP_MID, 0, 0);																	//设置目标位置	
	
	static lv_style_t 	style_sel_data;
	lv_style_reset(&style_sel_data);
	lv_style_init(&style_sel_data);
	lv_style_set_radius(&style_sel_data, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_sel_data, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_sel_data, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_sel_data, lv_color_black());																//设置样式中文字颜色
	if((sel_mode_flag == OperationMode_flag) && (set.Language == EN))	
	{
		lv_style_set_text_font(&style_sel_data, &my_font_standy_mode);														//设置样式中文字大小
	}
	else
	{
		lv_style_set_text_font(&style_sel_data, &lv_font_yahei_16_bpp4);													//设置样式中文字大小
	}
	lv_style_set_width(&style_sel_data, 160);																				//设置样式宽度
	lv_style_set_height(&style_sel_data, 57);
	
	//设置一个数据显示按钮
	lv_obj_t * text_sel_data  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_sel_data , &style_sel_data, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_sel_data ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_sel_data , LV_ALIGN_BOTTOM_MID, 0, 0);																//设置目标位置
	
	lv_obj_t * theme_label = lv_label_create(text_sel_theme);																//创建标签   seqn_label
	lv_obj_align(theme_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置	
	
	lv_obj_t * data_label = lv_label_create(text_sel_data);																	//创建标签   seqn_label
	lv_obj_align(data_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置
	
	switch(sel_mode_flag)
	{
		case OperationMode_flag:	{																						//运动模式设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[0]);							//设置动态文本(字符串形式)	
											lv_label_set_text(data_label , DispEnSelOperationMode[sel.OperaMode]);	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[0]);							//设置动态文本(字符串形式)	
											lv_label_set_text(data_label , DispCNSelOperationMode[sel.OperaMode]);	
										}	
										
										break;
									}
		case Speed_flag:			{																						//速度参数设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[1]);							//设置动态文本(字符串形式)
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[1]);							//设置动态文本(字符串形式)		
										}	
										if((sel.OperaMode == 0) || (sel.OperaMode == 1))
										{
											lv_label_set_text(data_label,DispEnSpeedData[sel.Speed]);						//设置具体参数标签文字
											lv_label_ins_text(data_label, LV_LABEL_POS_LAST, " rpm");							//设置动态文本(字符串形式)
										}
										else if(sel.OperaMode == 2)
										{
											if(set.Language == EN)
											{
												lv_label_set_text(data_label,DispEnRecSpeedData[sel.RecSpeed]);					//设置具体参数标签文字
												lv_label_ins_text(data_label, LV_LABEL_POS_LAST, " rpm");							//设置动态文本(字符串形式)
											}else
											{
												lv_label_set_text(data_label,DispCNRecSpeedData[sel.RecSpeed]);					//设置具体参数标签文字
											}
										}else if(sel.OperaMode == 3)
										{
											lv_label_set_text(data_label,DispEnRecSpeedData[sel.RecSpeed]);					//设置具体参数标签文字
											lv_label_ins_text(data_label, LV_LABEL_POS_LAST, " rpm");							//设置动态文本(字符串形式
										}
										
										break;
									}
		case Torque_flag:			{																						//转矩参数设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[3]);							//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[3]);							//设置动态文本(字符串形式)	
										}	
										if((sel.OperaMode == 0) || (sel.OperaMode == 1) || (sel.OperaMode == 2))
										{
											lv_label_set_text(data_label,DispEnTorqueData[sel.Torque]);						//设置具体参数标签文字
										}
										else if(sel.OperaMode == 3)
										{
											lv_label_set_text(data_label,DispEnAtrTorqueData[sel.AtrTorque]);				//设置具体参数标签文字
										}
										lv_label_ins_text(data_label, LV_LABEL_POS_LAST, " N"CN_DIAN"cm");					//设置动态文本(字符串形式)
										
										break;
									}
		case CWAngle_flag:			{																						//正角度设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[4]);							//设置动态文本(字符串形式)		
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[4]);							//设置动态文本(字符串形式)	
										}	
										if((sel.OperaMode == 2) || (sel.OperaMode == 3))
										{
											lv_label_set_text(data_label, DispEnAngleCWData[sel.AngleCW]);					//设置具体参数标签文字
										}
										lv_label_ins_text(data_label, LV_LABEL_POS_LAST, " "CN_DU);							//设置动态文本(字符串形式)
										
										break;
									}
		case CCWAngle_flag:			{																						//负角度设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[5]);							//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[5]);							//设置动态文本(字符串形式)	
										}
										if((sel.OperaMode == 2) || (sel.OperaMode == 3))
										{
											lv_label_set_text(data_label, DispEnAngleCCWData[sel.AngleCCW]);				//设置具体参数标签文字
										}
										lv_label_ins_text(data_label, LV_LABEL_POS_LAST, " "CN_DU);							//设置动态文本(字符串形式)
										
										break;
									}
		case AutoStart_flag:		{																						//自动启动设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[6]);							//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[6]);							//设置动态文本(字符串形式)	
										}	
										if(set.Language == EN)
										{
											lv_label_set_text(data_label,DispEnSelAuto[sel.AutoStart]);
										}
										else if(set.Language == CN)
										{
											lv_label_set_text(data_label,DispCN_ON_OFF[sel.AutoStart]);
										}
										
										break;
									}
		case AutoStop_flag:			{																						//自动关闭设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[7]);							//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[7]);							//设置动态文本(字符串形式)	
										}	
										if(set.Language == EN)
										{
											lv_label_set_text(data_label,DispEnSelAuto[sel.AutoStop]);
										}
										else if(set.Language == CN)
										{
											lv_label_set_text(data_label,DispCN_ON_OFF[sel.AutoStop]);
										}
										
										break;
									}
		case ApicalMode_flag:		{																						//根测模式设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[8]);							//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[8]);							//设置动态文本(字符串形式)	
										}	
										if(set.Language == EN)
										{
											lv_label_set_text(data_label, DispEnSelApical[sel.ApicalMode]);
										}
										else if(set.Language == CN)
										{
											lv_label_set_text(data_label, DispCNSelApical[sel.ApicalMode]);
										}
										
										break;
									}
		case BarPosition_flag:		{																						//根测参考点设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispENSelectTitle[9]);							//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSelectTitle[9]);							//设置动态文本(字符串形式)	
										}
										Apex_init1(sel);	
										
										char buf[8];
										lv_snprintf(buf, sizeof(buf), "%d%", sel.FlashBarPosition);
										
										lv_label_set_text(data_label,"0");
										lv_label_ins_text(data_label, LV_LABEL_POS_LAST, buf);								//设置动态文本(字符串形式)
										lv_obj_align(data_label , LV_ALIGN_CENTER, -50, 0);									//设置目标位置
										
										break;
									}
		default: 					break;		
	}
}
/**********************************************************************************************************
*	函 数 名: void Apex_init(FUN_SEL sel)
*	功能说明: 设置模式下，显示版本号
*	形    参: FUN_SEL sel	  sel设置缓存
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210930         
**********************************************************************************************************/
uint16_t  APEX_disp_cnt = 0;
void Apex_RUN(int Apex_cs)								//根测界面，与根测设置相关
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_apex1;
	lv_style_reset(&style_apex1);
	lv_style_init(&style_apex1);
	lv_style_set_bg_color(&style_apex1, lv_palette_main(LV_PALETTE_LIGHT_BLUE));			//蓝色，13~17
    lv_style_set_border_color(&style_apex1, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
	lv_style_set_border_width(&style_apex1, 1);												//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex1, 4);												//设置阴影边框宽度   宽度为0，无边框
    lv_style_set_shadow_ofs_y(&style_apex1, 2);
    lv_style_set_shadow_opa(&style_apex1, LV_OPA_50);
	lv_style_set_width(&style_apex1, 7);													//设置样式宽度
	lv_style_set_height(&style_apex1, 62);													//设置样式高度
	lv_style_set_radius(&style_apex1, 0);													//设置边框圆角
	if(Apex_cs <= 17)
	{
		lv_obj_t * bg_apex17 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex17 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex17 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex17  , LV_ALIGN_BOTTOM_RIGHT, -11, 2);							//设置目标位置
	}
	if(Apex_cs <= 16)
	{
		lv_obj_t * bg_apex16 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex16 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex16 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex16  , LV_ALIGN_BOTTOM_RIGHT, -19, 2);							//设置目标位置
	}
	if(Apex_cs <= 15)
	{
		lv_obj_t * bg_apex15 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex15 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex15 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex15  , LV_ALIGN_BOTTOM_RIGHT, -27, 2);							//设置目标位置
	}
	if(Apex_cs <= 14)
	{
		lv_obj_t * bg_apex14 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex14 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex14 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex14  , LV_ALIGN_BOTTOM_RIGHT, -35, 2);							//设置目标位置
	}
	if(Apex_cs <= 13)
	{
		lv_obj_t * bg_apex13 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex13 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex13 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex13  , LV_ALIGN_BOTTOM_RIGHT, -43, 2);							//设置目标位置
	}
	
	static lv_style_t 	style_apex2;
	lv_style_reset(&style_apex2);
	lv_style_init(&style_apex2);
	lv_style_set_bg_color(&style_apex2, lv_palette_main(LV_PALETTE_LIGHT_GREEN));			//蓝色，13~17
    lv_style_set_border_color(&style_apex2, lv_palette_darken(LV_PALETTE_LIGHT_GREEN, 3));
	lv_style_set_border_width(&style_apex2, 1);												//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex2, 4);												//设置阴影边框宽度   宽度为0，无边框
    lv_style_set_shadow_ofs_y(&style_apex2, 2);
    lv_style_set_shadow_opa(&style_apex2, LV_OPA_50);
	lv_style_set_width(&style_apex2, 7);													//设置样式宽度
	lv_style_set_height(&style_apex2, 62);													//设置样式高度
	lv_style_set_radius(&style_apex2, 0);													//设置边框圆角
	
	if(Apex_cs <= 12)
	{
		lv_obj_t * bg_apex12 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex12 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex12 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex12  , LV_ALIGN_BOTTOM_RIGHT, -51, 2);							//设置目标位置
	}
	
	if(Apex_cs <= 11)
	{
		lv_obj_t * bg_apex11 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex11 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex11 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex11  , LV_ALIGN_BOTTOM_RIGHT, -59, 2);							//设置目标位置
	}
	
	if(Apex_cs <= 10)
	{
		lv_obj_t * bg_apex10 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex10 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex10 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex10  , LV_ALIGN_BOTTOM_RIGHT, -67, 2);							//设置目标位置
	}	
	
	if(Apex_cs <= 9)
	{
		lv_obj_t * bg_apex9 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex9 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex9 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex9  , LV_ALIGN_BOTTOM_RIGHT, -75, 2);							//设置目标位置
	}
	
	static lv_style_t 	style_apex3;
	lv_style_reset(&style_apex3);
	lv_style_init(&style_apex3);
	lv_style_set_bg_color(&style_apex3, lv_palette_main(LV_PALETTE_YELLOW));			//蓝色，13~17
    lv_style_set_border_color(&style_apex3, lv_palette_darken(LV_PALETTE_YELLOW, 3));
	lv_style_set_border_width(&style_apex3, 1);												//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex3, 4);												//设置阴影边框宽度   宽度为0，无边框
    lv_style_set_shadow_ofs_y(&style_apex3, 2);
    lv_style_set_shadow_opa(&style_apex3, LV_OPA_50);
	lv_style_set_width(&style_apex3, 7);													//设置样式宽度
	lv_style_set_height(&style_apex3, 62);													//设置样式高度
	lv_style_set_radius(&style_apex3, 0);													//设置边框圆角
	
	if(Apex_cs <= 8)
	{
		lv_obj_t * bg_apex8 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex8 , &style_apex3, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex8 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex8  , LV_ALIGN_BOTTOM_RIGHT, -83, 2);							//设置目标位置
	}	
	
	if(Apex_cs <= 7)
	{
		lv_obj_t * bg_apex7 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex7 , &style_apex3, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex7 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex7 , LV_ALIGN_BOTTOM_RIGHT, -91, 2);							//设置目标位置
	}	
	
	if(Apex_cs <= 6)
	{
		lv_obj_t * bg_apex6 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex6 , &style_apex3, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex6 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex6  , LV_ALIGN_BOTTOM_RIGHT, -99, 2);							//设置目标位置
	}	
	
	if(Apex_cs <= 5)
	{
		lv_obj_t * bg_apex5 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex5 , &style_apex3, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex5 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex5  , LV_ALIGN_BOTTOM_RIGHT, -107, 2);							//设置目标位置
	}
	
	static lv_style_t 	style_apex4;
	lv_style_reset(&style_apex4);
	lv_style_init(&style_apex4);
	lv_style_set_bg_color(&style_apex4, lv_palette_main(LV_PALETTE_ORANGE));			//蓝色，13~17
    lv_style_set_border_color(&style_apex4, lv_palette_darken(LV_PALETTE_ORANGE, 3));
	lv_style_set_border_width(&style_apex4, 1);												//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex4, 4);												//设置阴影边框宽度   宽度为0，无边框
    lv_style_set_shadow_ofs_y(&style_apex4, 2);
    lv_style_set_shadow_opa(&style_apex4, LV_OPA_50);
	lv_style_set_width(&style_apex4, 7);													//设置样式宽度
	lv_style_set_height(&style_apex4, 62);													//设置样式高度
	lv_style_set_radius(&style_apex4, 0);													//设置边框圆角
	
	if(Apex_cs <= 4)
	{
		lv_obj_t * bg_apex4 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex4 , &style_apex4, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex4 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex4  , LV_ALIGN_BOTTOM_RIGHT, -115, 2);							//设置目标位置
	}
	
	if(Apex_cs <= 3)
	{
		lv_obj_t * bg_apex3 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex3 , &style_apex4, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex3 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex3  , LV_ALIGN_BOTTOM_RIGHT, -123, 2);							//设置目标位置
	}
	
	if(Apex_cs <= 2)
	{
		lv_obj_t * bg_apex2 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex2 , &style_apex4, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex2 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex2  , LV_ALIGN_BOTTOM_RIGHT, -131, 2);							//设置目标位置
	}
	
	if(Apex_cs <= 1)
	{
		lv_obj_t * bg_apex1 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex1 , &style_apex4, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex1 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex1  , LV_ALIGN_BOTTOM_RIGHT, -139, 2);							//设置目标位置
	}
	
	static lv_style_t 	style_apex5;
	lv_style_reset(&style_apex5);
	lv_style_init(&style_apex5);
	lv_style_set_bg_color(&style_apex5, lv_palette_main(LV_PALETTE_RED));					//蓝色，13~17
    lv_style_set_border_color(&style_apex5, lv_palette_darken(LV_PALETTE_RED, 3));
	lv_style_set_border_width(&style_apex5, 1);												//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex5, 4);												//设置阴影边框宽度   宽度为0，无边框
    lv_style_set_shadow_ofs_y(&style_apex5, 2);
    lv_style_set_shadow_opa(&style_apex5, LV_OPA_50);
	lv_style_set_width(&style_apex5, 7);													//设置样式宽度
	lv_style_set_height(&style_apex5, 62);													//设置样式高度
	lv_style_set_radius(&style_apex5, 0);													//设置边框圆角
	
	if(Apex_cs <= 0)
	{
		lv_obj_t * bg_apex0 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex0 , &style_apex5, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex0 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex0  , LV_ALIGN_BOTTOM_RIGHT, -147, 2);							//设置目标位置
	}
	
	/*******************************根测，Apex....****************************************************/	
	lv_obj_t *	standy_apex = lv_img_create(lv_scr_act());								//设置锉号显示底图
	LV_IMG_DECLARE(RUN_APEX1);
	lv_img_set_src(standy_apex, &RUN_APEX1);
	lv_obj_align(standy_apex , LV_ALIGN_CENTER, 0, 0);									//设置目标位置	
	
	static lv_style_t 	style_data;
	lv_style_reset(&style_data);
	lv_style_init(&style_data);
	lv_style_set_radius(&style_data, 0);												//设置边框圆角
	lv_style_set_bg_color(&style_data, lv_color_white());								//设置上背景颜色
	lv_style_set_border_width(&style_data, 0);											//设置边框宽度
	lv_style_set_text_color(&style_data, lv_color_black());								//设置样式中文字颜色
	lv_style_set_text_font(&style_data, &lv_font_yahei_16_bpp4);						//设置样式中文字大小
	lv_style_set_height(&style_data, 32);												//设置样式高度
	lv_style_set_width(&style_data, 32);												//设置样式宽度
	
	lv_obj_t * data_bnt = lv_obj_create(lv_scr_act());
	lv_obj_add_style(data_bnt , &style_data, 0);										//加载一种样式
	lv_obj_set_scrollbar_mode(data_bnt ,LV_SCROLLBAR_MODE_OFF);							//不显示滚动条
	lv_obj_align(data_bnt , LV_ALIGN_TOP_RIGHT, -10, -5);								//设置目标位置	
	
	if(sys.apex>=(18-sel.FlashBarPosition))
	{
		if(APEX_disp_cnt > 600)
		{
			lv_obj_t * theme_label = lv_label_create(data_bnt);									//创建标签   seqn_label
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", Apex_cs);
			
			if(Apex_cs>17)
			{
				lv_label_set_text(theme_label,"--");
			}
			else if((Apex_cs >= 0)&&(Apex_cs < 10))
			{
				lv_label_set_text(theme_label,"0");
				lv_label_ins_text(theme_label, LV_LABEL_POS_LAST, buf);							//设置动态文本(字符串形式)
			}else
			{
				lv_label_set_text(theme_label,buf);
			}
			lv_obj_align(theme_label , LV_ALIGN_CENTER, 0, 0);									//设置目标位置
			
			APEX_disp_cnt = 0;
		}
	}else
	{
		lv_obj_t * theme_label = lv_label_create(data_bnt);									//创建标签   seqn_label
		char buf[8];
		lv_snprintf(buf, sizeof(buf), "%d%", Apex_cs);
		
		if(Apex_cs>17)
		{
			lv_label_set_text(theme_label,"--");
		}
		else if((Apex_cs >= 0)&&(Apex_cs < 10))
		{
			lv_label_set_text(theme_label,"0");
			lv_label_ins_text(theme_label, LV_LABEL_POS_LAST, buf);							//设置动态文本(字符串形式)
		}else
		{
			lv_label_set_text(theme_label,buf);
		}
		lv_obj_align(theme_label , LV_ALIGN_CENTER, 0, 0);									//设置目标位置
	}
	
	
	
	lv_obj_t * bg_apex_bar = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象	
	if(Apex_cs >= 13)
	{
		lv_obj_add_style(bg_apex_bar , &style_apex1, 0);									//加载一种样式
	}else if((Apex_cs <= 12)&&(Apex_cs >= 9))
	{
		lv_obj_add_style(bg_apex_bar , &style_apex2, 0);									//加载一种样式
		
	}else if((Apex_cs <= 8)&&(Apex_cs >= 5))
	{
		lv_obj_add_style(bg_apex_bar , &style_apex3, 0);									//加载一种样式
	}else if((Apex_cs <= 4)&&(Apex_cs >= 1))
	{
		lv_obj_add_style(bg_apex_bar , &style_apex4, 0);									//加载一种样式
	}else 
	{
		lv_obj_add_style(bg_apex_bar , &style_apex5, 0);									//加载一种样式
	}
	lv_obj_set_size(bg_apex_bar, 32,5);
	lv_obj_set_scrollbar_mode(bg_apex_bar ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
	lv_obj_align(bg_apex_bar , LV_ALIGN_TOP_RIGHT, -10, 22);							//设置目标位置
}

/**********************************************************************************************************
*	函 数 名: void DispMeasure(void)
*	功能说明: 显示“Factory Mode”标签
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
extern unsigned int Ratio;
extern uint32_t	samH_RootInPos;
extern SAM_DAT samh,saml;
void DispMeasure(void)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	lv_obj_t* textFac=lv_label_create(sel_top);
	char buf[8];
	lv_snprintf(buf, sizeof(buf), "%d%", Ratio);
	lv_label_set_text(textFac,"D: ");
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST, buf);					//设置动态文本(字符串形式)
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST," N: ");
	lv_snprintf(buf, sizeof(buf), "%d%", samH_RootInPos);
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST, buf);					//设置动态文本(字符串形式)
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST, "\n ");					//设置动态文本(字符串形式)
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST,"H: ");
	lv_snprintf(buf, sizeof(buf), "%d%", samh.VPP);
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST, buf);					//设置动态文本(字符串形式)
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST," L: ");
	lv_snprintf(buf, sizeof(buf), "%d%", saml.VPP);
	lv_label_ins_text(textFac, LV_LABEL_POS_LAST, buf);					//设置动态文本(字符串形式)
	
	lv_obj_align(textFac,LV_ALIGN_CENTER,0,0);				//设置文字位置
	
}
/**********************************************************************************************************
*	函 数 名: void Apex_init(FUN_SEL sel)
*	功能说明: 设置模式下，显示版本号
*	形    参: FUN_SEL sel	  sel设置缓存
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210911         
**********************************************************************************************************/
void Apex_init(FUN_SEL sel)								//根测界面，与根测设置相关
{
	static lv_style_t 	style_apex0;
	lv_style_reset(&style_apex0);
	lv_style_init(&style_apex0);
	lv_style_set_border_width(&style_apex0, 0);											//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex0, 0);											//设置阴影边框宽度   宽度为0，无边框
	lv_style_set_width(&style_apex0, 5);												//设置样式宽度
	lv_style_set_height(&style_apex0, 50);												//设置样式高度
	lv_style_set_radius(&style_apex0, 0);												//设置边框圆角
	lv_style_set_bg_color(&style_apex0, lv_palette_main(LV_PALETTE_RED));
	
	lv_obj_t * bg_apex0 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(bg_apex0 , &style_apex0, 0);									//加载一种样式
	lv_obj_set_scrollbar_mode(bg_apex0 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
	lv_obj_align(bg_apex0  , LV_ALIGN_BOTTOM_MID, -13, 0);							//设置目标位置
	
	static lv_style_t 	style_apex1;
	lv_style_reset(&style_apex1);
	lv_style_init(&style_apex1);
	lv_style_set_border_width(&style_apex1, 0);											//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex1, 0);											//设置阴影边框宽度   宽度为0，无边框
	lv_style_set_width(&style_apex1, 5);												//设置样式宽度
	lv_style_set_height(&style_apex1, 50);												//设置样式高度
	lv_style_set_radius(&style_apex1, 0);												//设置边框圆角
	lv_style_set_bg_color(&style_apex1, lv_palette_main(LV_PALETTE_ORANGE));		
	
	if(sel.FlashBarPosition >= 1)
	{			
		lv_obj_t * bg_apex1 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex1 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex1 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex1  , LV_ALIGN_BOTTOM_MID, -6, 0);							//设置目标位置
	}
	
	if(sel.FlashBarPosition >= 2)
	{
		lv_obj_t * bg_apex2 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex2 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex2 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex2  , LV_ALIGN_BOTTOM_MID, 1, 0);							//设置目标位置
	}
	
	if(sel.FlashBarPosition >= 3)
	{
		lv_obj_t * bg_apex3 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex3 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex3 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex3  , LV_ALIGN_BOTTOM_MID, 8, 0);							//设置目标位置
	}
	
	if(sel.FlashBarPosition >= 4)
	{
		lv_obj_t * bg_apex4 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex4 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex4 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex4 , LV_ALIGN_BOTTOM_MID, 15, 0);							//设置目标位置	
	}
	
	static lv_style_t 	style_apex2;
	lv_style_reset(&style_apex2);
	lv_style_init(&style_apex2);
	lv_style_set_border_width(&style_apex2, 0);											//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex2, 0);											//设置阴影边框宽度   宽度为0，无边框
	lv_style_set_width(&style_apex2, 5);												//设置样式宽度
	lv_style_set_height(&style_apex2, 50);												//设置样式高度
	lv_style_set_radius(&style_apex2, 0);												//设置边框圆角	
	lv_style_set_bg_color(&style_apex2, lv_palette_main(LV_PALETTE_YELLOW));	
	
	if(sel.FlashBarPosition >= 5)
	{
		lv_obj_t * bg_apex5 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex5 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex5 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex5 , LV_ALIGN_BOTTOM_MID, 22, 0);							//设置目标位置	
	}
	
	if(sel.FlashBarPosition >= 6)
	{
		lv_obj_t * bg_apex6 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex6 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex6 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex6 , LV_ALIGN_BOTTOM_MID, 30, 0);							//设置目标位置	
	}
	
	if(sel.FlashBarPosition >= 7)
	{
		lv_obj_t * bg_apex7 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex7 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex7 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex7 , LV_ALIGN_BOTTOM_MID, 37, 0);							//设置目标位置	
	}
	
	if(sel.FlashBarPosition >= 8)
	{
		lv_obj_t * bg_apex8 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex8 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex8 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex8 , LV_ALIGN_BOTTOM_MID, 44, 0);							//设置目标位置	
	}
	
	
	/*******************************根测，Apex....****************************************************/	

		lv_obj_t *	standy_apex = lv_img_create(lv_scr_act());								//设置锉号显示底图
		LV_IMG_DECLARE(Standy_APEX);
		lv_img_set_src(standy_apex, &Standy_APEX);
		lv_obj_align(standy_apex , LV_ALIGN_BOTTOM_RIGHT, 0, 0);							//设置目标位置	
}

void Apex_init1(FUN_SEL sel)								//根测界面，与根测设置相关
{
	static lv_style_t 	style_apex0;
	lv_style_reset(&style_apex0);
	lv_style_init(&style_apex0);
	lv_style_set_border_width(&style_apex0, 0);											//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex0, 0);											//设置阴影边框宽度   宽度为0，无边框
	lv_style_set_width(&style_apex0, 5);												//设置样式宽度
	lv_style_set_height(&style_apex0, 50);												//设置样式高度
	lv_style_set_radius(&style_apex0, 0);												//设置边框圆角
	lv_style_set_bg_color(&style_apex0, lv_palette_main(LV_PALETTE_RED));
	
	lv_obj_t * bg_apex0 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(bg_apex0 , &style_apex0, 0);									//加载一种样式
	lv_obj_set_scrollbar_mode(bg_apex0 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
	lv_obj_align(bg_apex0  , LV_ALIGN_BOTTOM_MID, -13, 0);							//设置目标位置
	
	static lv_style_t 	style_apex1;
	lv_style_reset(&style_apex1);
	lv_style_init(&style_apex1);
	lv_style_set_border_width(&style_apex1, 0);											//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex1, 0);											//设置阴影边框宽度   宽度为0，无边框
	lv_style_set_width(&style_apex1, 5);												//设置样式宽度
	lv_style_set_height(&style_apex1, 50);												//设置样式高度
	lv_style_set_radius(&style_apex1, 0);												//设置边框圆角
	lv_style_set_bg_color(&style_apex1, lv_palette_main(LV_PALETTE_ORANGE));		
	
	if(sel.FlashBarPosition >= 1)
	{			
		lv_obj_t * bg_apex1 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex1 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex1 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex1  , LV_ALIGN_BOTTOM_MID, -6, 0);							//设置目标位置
	}
	
	if(sel.FlashBarPosition >= 2)
	{
		lv_obj_t * bg_apex2 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex2 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex2 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex2  , LV_ALIGN_BOTTOM_MID, 1, 0);							//设置目标位置
	}
	
	if(sel.FlashBarPosition >= 3)
	{
		lv_obj_t * bg_apex3 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex3 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex3 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex3  , LV_ALIGN_BOTTOM_MID, 8, 0);							//设置目标位置
	}
	
	if(sel.FlashBarPosition >= 4)
	{
		lv_obj_t * bg_apex4 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex4 , &style_apex1, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex4 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex4 , LV_ALIGN_BOTTOM_MID, 15, 0);							//设置目标位置	
	}
	
	static lv_style_t 	style_apex2;
	lv_style_reset(&style_apex2);
	lv_style_init(&style_apex2);
	lv_style_set_border_width(&style_apex2, 0);											//设置边框宽度   宽度为0，无边框
	lv_style_set_shadow_width(&style_apex2, 0);											//设置阴影边框宽度   宽度为0，无边框
	lv_style_set_width(&style_apex2, 5);												//设置样式宽度
	lv_style_set_height(&style_apex2, 50);												//设置样式高度
	lv_style_set_radius(&style_apex2, 0);												//设置边框圆角	
	lv_style_set_bg_color(&style_apex2, lv_palette_main(LV_PALETTE_YELLOW));	
	
	if(sel.FlashBarPosition >= 5)
	{
		lv_obj_t * bg_apex5 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex5 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex5 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex5 , LV_ALIGN_BOTTOM_MID, 22, 0);							//设置目标位置	
	}
	
	if(sel.FlashBarPosition >= 6)
	{
		lv_obj_t * bg_apex6 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex6 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex6 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex6 , LV_ALIGN_BOTTOM_MID, 30, 0);							//设置目标位置	
	}
	
	if(sel.FlashBarPosition >= 7)
	{
		lv_obj_t * bg_apex7 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex7 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex7 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex7 , LV_ALIGN_BOTTOM_MID, 37, 0);							//设置目标位置	
	}
	
	if(sel.FlashBarPosition >= 8)
	{
		lv_obj_t * bg_apex8 = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(bg_apex8 , &style_apex2, 0);									//加载一种样式
		lv_obj_set_scrollbar_mode(bg_apex8 ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex8 , LV_ALIGN_BOTTOM_MID, 44, 0);							//设置目标位置	
	}
	
	
	/*******************************根测，Apex....****************************************************/	

		lv_obj_t *	standy_apex = lv_img_create(lv_scr_act());								//设置锉号显示底图
		LV_IMG_DECLARE(Standy_APEX);
		lv_img_set_src(standy_apex, &Standy_APEX);
		lv_obj_align(standy_apex , LV_ALIGN_BOTTOM_RIGHT, 0, 0);							//设置目标位置	
		
	if(sel_mode_flag == BarPosition_flag)
	{
		lv_obj_t * bg_apex_bar = lv_obj_create(lv_scr_act());								//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象	
		if(sel.FlashBarPosition >= 5)
		{
			lv_obj_add_style(bg_apex_bar , &style_apex2, 0);									//加载一种样式
		}else if((sel.FlashBarPosition <= 5)&&(sel.FlashBarPosition >= 1))
		{
			lv_obj_add_style(bg_apex_bar , &style_apex1, 0);									//加载一种样式
			
		}else
		{
			lv_obj_add_style(bg_apex_bar , &style_apex0, 0);									//加载一种样式
		}
		lv_obj_set_size(bg_apex_bar, 32,5);
		lv_obj_set_scrollbar_mode(bg_apex_bar ,LV_SCROLLBAR_MODE_OFF);						//不显示滚动条
		lv_obj_align(bg_apex_bar , LV_ALIGN_CENTER, -50, 27);							//设置目标位置
	}
}


//lv_obj_t * TL_motor_bar;
lv_obj_t * TL_motor_limit;
lv_obj_t * TL_motor_bar1[32];
/**********************************************************************************************************
*	函 数 名: void Standy_ON(void)
*	功能说明: 电机运行时调用
*	形    参: int TL_Motor电机转矩	uint8_t TL_limit_flag转矩显示标志	
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
extern uint8_t Aging_Flag;
extern uint8_t Aging_Flag1;
void Standy_ON(int TL_Motor,uint8_t TL_limit_flag)
{
	unsigned int i;
	 
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_work_bg;
	lv_style_reset(&style_work_bg);
	lv_style_init(&style_work_bg);
	lv_style_set_radius(&style_work_bg, 0);																					//设置边框圆角
	lv_style_set_bg_color(&style_work_bg, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_work_bg, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_work_bg, lv_color_black());																//设置样式中文字颜色		
	lv_style_set_text_font(&style_work_bg, &lv_font_yahei_12_bpp4);															//设置样式中文字大小
	lv_style_set_width(&style_work_bg, 160);																				//设置样式宽度
	lv_style_set_height(&style_work_bg, 80);																				//设置样式高
	
	lv_obj_t * Running_disp_main = lv_obj_create(sel_top);
	lv_obj_add_style(Running_disp_main, &style_work_bg, 0);
	lv_obj_set_scrollbar_mode(Running_disp_main ,LV_SCROLLBAR_MODE_OFF);
	lv_obj_align(Running_disp_main , LV_ALIGN_CENTER, 0, 0);																//设置目标位置
	
	
	lv_obj_t * run_img = lv_img_create(Running_disp_main);
	lv_obj_t * Running_Text = lv_label_create(Running_disp_main);
	lv_label_set_text(Running_Text," ");
//	lv_obj_set_size(Running_Text,96,12);
	
	if((sel.OperaMode ==0) || (sel.OperaMode ==1) || (sel.OperaMode==3))
	{
		static int torque_limit_flag = 0;
		if(sel.OperaMode == 0 || sel.OperaMode == 1)
		{
			torque_limit_flag = d_TqBlinkGrid[sel.Torque];
		}
		else if(sel.OperaMode == 3)
		{
			torque_limit_flag = d_TqBlinkGrid[sel.AtrTorque];
		}
		static lv_style_t 	Disp_torque_limit_style;
		lv_style_reset(&Disp_torque_limit_style);
		lv_style_init(&Disp_torque_limit_style);
		lv_style_set_radius(&Disp_torque_limit_style, 0);														//设置圆角0的边框
		lv_style_set_border_opa(&Disp_torque_limit_style, LV_OPA_0);											//设置边框透明
		lv_style_set_bg_color(&Disp_torque_limit_style, lv_palette_main(LV_PALETTE_RED));						//设置背景色为LV_COLOR_WHITE
		lv_style_set_width(&Disp_torque_limit_style, 3);														//设置样式宽度
		lv_style_set_height(&Disp_torque_limit_style, 25);														//设置样式高度
		
		TL_motor_limit = lv_obj_create(lv_scr_act());															//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
		lv_obj_add_style(TL_motor_limit , &Disp_torque_limit_style, 0);											//加载一种样式
		lv_obj_set_scrollbar_mode(TL_motor_limit ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
		lv_obj_align(TL_motor_limit , LV_ALIGN_BOTTOM_LEFT, torque_limit_flag, -23);							//设置目标位置
		
		static lv_style_t 	Disp_torque_style;
		lv_style_reset(&Disp_torque_style);
		lv_style_init(&Disp_torque_style);
		lv_style_set_radius(&Disp_torque_style, 0);																//设置圆角0的边框
		lv_style_set_border_opa(&Disp_torque_style, LV_OPA_0);													//设置边框透明
		lv_style_set_bg_color(&Disp_torque_style, lv_palette_main(LV_PALETTE_RED));								//设置背景色为LV_COLOR_WHITE
		lv_style_set_radius(&Disp_torque_style, 0);																//设置圆角0的边框
		lv_style_set_width(&Disp_torque_style, 4);																//设置样式宽度
		lv_style_set_height(&Disp_torque_style, 25);															//设置样式高度															//设置边框圆角	
		
		for(i = 0; i < torque_limit_flag/5; i++)
		{
			TL_motor_bar1[i] = lv_obj_create(lv_scr_act());
			lv_obj_add_style(TL_motor_bar1[i], &Disp_torque_style, 0);											//加载一种样式
			lv_obj_set_scrollbar_mode(TL_motor_bar1[i] ,LV_SCROLLBAR_MODE_OFF);									//不显示滚动条
			lv_obj_align(TL_motor_bar1[i] , LV_ALIGN_BOTTOM_LEFT, i*5, -23);									//设置目标位置
			lv_obj_move_background(TL_motor_bar1[i]);
		}
		
		if(TL_Motor < torque_limit_flag)
		{
			for(i = 0; i < (TL_Motor/5); i++)
			{
				lv_obj_move_foreground(TL_motor_bar1[i]);
			}
		}else
		{
			for(i = 0; i < (torque_limit_flag/5); i++)
			{
				lv_obj_move_foreground(TL_motor_bar1[i]);
			}
		}
			
		lv_obj_t * bar_img = lv_img_create(Running_disp_main);
		LV_IMG_DECLARE(Torque_Bar_scale);
		lv_img_set_src(bar_img, &Torque_Bar_scale);
		lv_obj_align(bar_img,LV_ALIGN_BOTTOM_MID,0,10);
		
		if(sel.OperaMode == 3)
		{
			LV_IMG_DECLARE(ROT_atr_NONE_small);
			lv_img_set_src(run_img, &ROT_atr_NONE_small);
			
			lv_label_set_text(Running_Text," ");
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnRecSpeedData[sel.RecSpeed]);
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"rpm");
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"");
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnAngleCWData[sel.AngleCW]);
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,CN_DU);
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"/");
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnAngleCCWData[sel.AngleCCW]);
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,CN_DU);
			
			lv_obj_align(Running_Text,LV_ALIGN_TOP_RIGHT,10,-13);
			gTempMode = Atc_mode;
		}else
		{
			if(sel.OperaMode ==0)
			{
				LV_IMG_DECLARE(ROT_fwd_NONE_small);
				lv_img_set_src(run_img, &ROT_fwd_NONE_small);		
			}else if(sel.OperaMode == 1)
			{
				LV_IMG_DECLARE(ROT_rev_NONE_small);
				lv_img_set_src(run_img, &ROT_rev_NONE_small);		
			}
			if((Aging_Flag == 0)&&(Aging_Flag1 == 0))
			{		
				lv_label_set_text(Running_Text," ");
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnSpeedData[sel.Speed]);
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"rpm");
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST," ");
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnTorqueData[sel.Torque]);
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"N"CN_DIAN"cm");
			}else
			{
				lv_label_set_text(Running_Text," ");
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnSpeedData[sel.Speed]);
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"rpm");
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST," ");
				if(Aging_Flag==1)
				{
					lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"Aging");
				}else
				{
					lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"T_Aging");
				}
				lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST," ");
			}
		
			lv_obj_align(Running_Text,LV_ALIGN_TOP_RIGHT,10,-13);
			gTempMode = Rotory_mode;
		}
		lv_obj_align(run_img,LV_ALIGN_TOP_LEFT,-7,-10);		
	}else if(sel.OperaMode== 2)
	{
		if(sel.AngleCW < sel.AngleCCW)
		{
			LV_IMG_DECLARE(REC_r_f_NONE);
			lv_img_set_src(run_img, &REC_r_f_NONE);
		}else if(sel.AngleCW > sel.AngleCCW)
		{
			LV_IMG_DECLARE(REC_f_r_NONE);
			lv_img_set_src(run_img, &REC_f_r_NONE);	
		}else if(sel.AngleCW == sel.AngleCCW)
		{
			LV_IMG_DECLARE(REC_fr_NONE);
			lv_img_set_src(run_img, &REC_fr_NONE);	
		}
		lv_obj_align(run_img, LV_ALIGN_LEFT_MID, 4, 0);
		
		lv_label_set_text(Running_Text," ");
		if(set.Language == EN)
		{
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnRecSpeedData[sel.RecSpeed]);
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"rpm ");
		}else
		{
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispCNRecSpeedData[sel.RecSpeed]);
		}
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"\n ");
		if(set.Language==EN)
		{
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"FWD");
		}
		else if(set.Language==CN)
		{
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,CN_ZHENG CN_ZHUAN CN_JIAO);
		}
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,": ");
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnAngleCWData[sel.AngleCW]);
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,CN_DU);
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"\n ");
		
		if(set.Language==EN)
		{
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,"REV");
		}
		else if(set.Language==CN)
		{
			lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,CN_FAN1 CN_ZHUAN CN_JIAO);
		}
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,": ");
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,DispEnAngleCCWData[sel.AngleCCW]);
		lv_label_ins_text(Running_Text,LV_LABEL_POS_LAST,CN_DU);
		
		lv_obj_align(Running_Text, LV_ALIGN_CENTER, 26, -2);
		gTempMode = Reciproc_mode;
		
	}else if(sel.OperaMode==4)
	{
		HAL_Delay(50);
		gTempMode = Measure_mode;
		Apex_RUN(18);						//初始化此处大于17
//		ApexDispBG();	
	}
	
}
void Disp_torque_bar_scale(int TL_Motor,uint8_t TL_limit_flag)
{
	unsigned int i;
	
	static int torque_limit_flag = 0;
	if(sel.OperaMode == 0 || sel.OperaMode == 1)
	{
		torque_limit_flag = d_TqBlinkGrid[sel.Torque];
	}
	else if(sel.OperaMode == 3)
	{
		torque_limit_flag = d_TqBlinkGrid[sel.AtrTorque];
	}
	
	for(i = 0; i < torque_limit_flag/5; i++)
	{
		lv_obj_move_background(TL_motor_bar1[i]);
	}
	if(TL_Motor < torque_limit_flag)
	{
		for(i = 0; i < (TL_Motor/5); i++)
		{
			lv_obj_move_foreground(TL_motor_bar1[i]);
		}
	}else
	{
		for(i = 0; i < torque_limit_flag/5; i++)
		{
			lv_obj_move_foreground(TL_motor_bar1[i]);
		}
	}

	if(TL_limit_flag)
	{
		lv_obj_move_foreground(TL_motor_limit);
	}else
	{
		lv_obj_move_background(TL_motor_limit);
	}
}
extern int iq;
extern u16 regi[128];
extern u16 angle_raw;
void Disp_torque_bar_scale1(int TL_Motor, int TL_Motor_Last, uint8_t TL_limit_flag)
{
	unsigned int i;
	static int torque_limit_flag = 0;
	if(sel.OperaMode == 0 || sel.OperaMode == 1)
	{
		torque_limit_flag = d_TqBlinkGrid[sel.Torque];
	}
	else if(sel.OperaMode == 3)
	{
		torque_limit_flag = d_TqBlinkGrid[sel.AtrTorque];
	}
	if(TL_Motor > torque_limit_flag)
	{
		TL_Motor = torque_limit_flag;
	}
	TL_Motor = TL_Motor / 3;
	TL_Motor_Last = TL_Motor_Last /3;
	if(TL_Motor > TL_Motor_Last)
	{
		for(i = TL_Motor_Last; i < TL_Motor; i++)
		{
//			LCD_Fill(i*3,31,i*3+2,56,RED);
		}
	}else if(TL_Motor < TL_Motor_Last)
	{
		for(i = TL_Motor; i < TL_Motor_Last; i++)
		{
//			LCD_Fill(i*3,31,i*3+2,56,WHITE);
		}
	}
	LCD_ShowNum(30,30,iq,5,RED);
	LCD_ShowNum(30,60,angle_raw,5,RED);
	if(TL_limit_flag == 1)
	{
		LCD_Fill(torque_limit_flag,31,torque_limit_flag+2,56,RED);
	}else
	{
		LCD_Fill(torque_limit_flag,31,torque_limit_flag+2,56,WHITE);
	}
}
/*****************************************************与Setting相关的界面**********************************************/
/*****************************************此界面与待机界面的设置界面相同，故用一个样式*********************************/
extern char Versions[];
extern char Versions1[];
extern uint8_t Versions_Flag;
/**********************************************************************************************************
*	函 数 名: void SYS_set_init(FUN_SET set)
*	功能说明: 待机模式的设置参数样式初始化
*	形    参: 无	 
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void SYS_set_init(FUN_SET set)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_set_theme;
	lv_style_reset(&style_set_theme);
	lv_style_init(&style_set_theme);
	lv_style_set_radius(&style_set_theme, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_set_theme, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE,3));											//设置上背景颜色
	lv_style_set_bg_grad_color(&style_set_theme, lv_palette_darken(LV_PALETTE_BLUE,3));
	lv_style_set_bg_grad_dir(&style_set_theme, LV_GRAD_DIR_HOR);
	
	lv_style_set_border_width(&style_set_theme, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_set_theme, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_set_theme, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_set_theme, 160);																				//设置样式宽度
	lv_style_set_height(&style_set_theme, 23);																				//设置样式高度
	
	//设置一个主题显示按钮
	lv_obj_t * text_set_theme  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_set_theme , &style_set_theme, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_set_theme ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_set_theme , LV_ALIGN_TOP_MID, 0, 0);																	//设置目标位置	
	
	static lv_style_t 	style_set_data;
	lv_style_reset(&style_set_data);
	lv_style_init(&style_set_data);
	lv_style_set_radius(&style_set_data, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_set_data, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_set_data, 0);																			//设置边框宽度
	if(set_mode_flag == BeepVol_flag)
	{
		lv_style_set_text_color(&style_set_data, lv_palette_lighten(LV_PALETTE_GREEN,1));									//设置样式中文字颜色
	}else
	{
		lv_style_set_text_color(&style_set_data, lv_color_black());															//设置样式中文字颜色
	}
	if(set_mode_flag == BeepVol_flag)																						
	{
		lv_style_set_text_font(&style_set_data, &lv_font_yahei_16_bpp4);													//判断是否显示音量
	}else
	{
		lv_style_set_text_font(&style_set_data, &lv_font_yahei_16_bpp4);													//设置样式中文字大小
	}
	lv_style_set_width(&style_set_data, 160);																				//设置样式宽度
	lv_style_set_height(&style_set_data, 57);
	
	//设置一个数据显示按钮
	lv_obj_t * text_set_data  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_set_data , &style_set_data, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_set_data ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_set_data , LV_ALIGN_BOTTOM_MID, 0, 0);																//设置目标位置
	
	lv_obj_t * theme_label = lv_label_create(text_set_theme);																//创建标签   seqn_label
	lv_obj_align(theme_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置	
	
	lv_obj_t * data_label = lv_label_create(text_set_data);																	//创建标签   seqn_label
	lv_obj_align(data_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置
	
	switch(set_mode_flag)
	{
		case Versions_flag:			{																						//版本号显示
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[0]);								//设置动态文本(字符串形式)	
										}
										else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[0]);								//设置动态文本(字符串形式)	
										}
										if(Versions_Flag == 0)
										{
											lv_label_set_text(data_label , Versions);											//显示版本号
										}else
										{
											lv_label_set_text(data_label , Versions1);											//显示版本号
										}
										
										break;
									}
		case AutoPowerOff_flag:		{																						//自动关机时间设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[1]);								//设置动态文本(字符串形式)
										}
										else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[1]);								//设置动态文本(字符串形式)		
										}	
										
										lv_label_set_text(data_label,DispEnSetAPOtime[set.AutoPowerOffTime]);				// 设置具体参数标签文字
										
										if(set.Language == EN)																// 添加单位
										{
											lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispEnSetAPO[0]);
										}
										else if(set.Language == CN)
										{
											lv_label_ins_text(data_label,LV_LABEL_POS_LAST,CN_FEN CN_ZHONG1);
										}
										
										break;
									}
		case AutoStandy_flag:		{																						// 自动返回待机时间设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[2]);								// 设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[2]);								// 设置动态文本(字符串形式)	
										}	
										
										lv_label_set_text(data_label,DispEnSetAstTime[set.AutoStandyTime]);					// 设置具体参数标签文字
										
										if(set.Language == EN)																// 添加单位
										{
											lv_label_ins_text(data_label,LV_LABEL_POS_LAST,DispEnSetAst[0]);
										}
										else if(set.Language == CN)
										{
											lv_label_ins_text(data_label, LV_LABEL_POS_LAST, CN_MIAO CN_ZHONG1);
										}
										
										break;
									}
		case BeepVol_flag:			{																						// 音量设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[3]);								// 设置动态文本(字符串形式)		
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[3]);								// 设置动态文本(字符串形式)	
										}	
										
										if(set.BeepVol == 0)
										{
											lv_label_set_text(data_label,LV_SYMBOL_MUTE);									// 设置 音量符号
										}
										else if(set.BeepVol == 1)
										{
											lv_label_set_text(data_label,LV_SYMBOL_VOLUME_MID);								// 设置 音量符号
										}
										else if(set.BeepVol == 2)
										{
											lv_label_set_text(data_label,LV_SYMBOL_VOLUME_MAX);								// 设置 音量符号
										}else if(set.BeepVol == 3)		
										{	
											lv_label_set_text(data_label,LV_SYMBOL_VOLUME_FILL);								// 设置 音量符号
										}
										
										break;
									}
		case Bl_flag:				{																						// 背光设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[4]);								// 设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[4]);								// 设置动态文本(字符串形式)	
										}
										lv_label_set_text(data_label,DispEnSetbl[set.backlight]);							// 设置标签文字	
										
										break;
									}
		case Hand_flag:				{																						// 左右手操作设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[5]);								// 设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[5]);								// 设置动态文本(字符串形式)	
										}	
										if(set.Language==EN)
										{
											lv_label_set_text(data_label,DispEnSethand[set.Hand]);							// 设置具体参数标签文字
										}
										else if(set.Language==CN)
										{
											lv_label_set_text(data_label,DispCNSethand[set.Hand]);							// 设置具体参数标签文字
										}
										
										break;
									}
		case Apexsensitivity_flag:	{																						// 根测灵敏度设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[6]);								// 设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[6]);								// 设置动态文本(字符串形式)	
										}	
										if(set.Language==EN)
										{
											lv_label_set_text(data_label,DispEnSetApexSen[set.ApexSense]);					// 设置具体参数标签文字
										}
										else if(set.Language==CN)
										{
											lv_label_set_text(data_label,DispCNSetApexSen[set.ApexSense]);					// 设置具体参数标签文字
										}
										
										break;
									}
		case Language_flag:			{																						// 语言设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[7]);								// 设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[7]);								// 设置动态文本(字符串形式)	
										}	
										if(set.Language==EN)
										{
											lv_label_set_text(data_label,DispEnSetLan[0]);									// 设置具体参数标签文字
										}
										else if(set.Language==CN)
										{
											lv_label_set_text(data_label,DispEnSetLan[1]);									// 设置具体参数标签文字
										}
										
										break;
									}
		case Calibration_flag:		{																						// 校准设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[8]);								// 设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[8]);								// 设置动态文本(字符串形式)	
										}
										if(set.Language==EN)
										{
											lv_label_set_text(data_label,DispEnSetCal[set.Calibration]);					// 设置具体参数标签文字
										}
										else if(set.Language==CN)
										{
											lv_label_set_text(data_label,DispCN_ON_OFF[set.Calibration]);					// 设置具体参数标签文字
										}
										
										break;
									}
		case RestoreFactory_flag:	{																						//恢复出厂设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[9]);								//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[9]);								//设置动态文本(字符串形式)	
										}
										if(set.Language==EN)
										{
											lv_label_set_text(data_label, DispEnSetRfs[set.RestoreFactorySettings]);		// 设置具体参数标签文字
										}
										else if(set.Language==CN)
										{
											lv_label_set_text(data_label, DispCN_ON_OFF[set.RestoreFactorySettings]);		// 设置具体参数标签文字
										}
																			
										break;
									}
		case MenuStartup_flag:		{																						//恢复出厂设置
										if(set.Language == EN)
										{
											lv_label_set_text(theme_label, DispEnSetTitle[10]);								//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(theme_label, DispCNSetTitle[10]);								//设置动态文本(字符串形式)	
										}
										if(set.Language==EN)
										{
											lv_label_set_text(data_label, DispEnSetMem[set.StartupMemory]);		// 设置具体参数标签文字
										}
										else if(set.Language==CN)
										{
											lv_label_set_text(data_label, DispCNSetMem[set.StartupMemory]);		// 设置具体参数标签文字
										}
																			
										break;
									}
						
		default: 					break;		
	}
}

/**********************************************************************************************************
*	函 数 名: void Filelist_init(SeqLib *fileseq)
*	功能说明: 内置错列表
*	形    参: Seq_name_inside *name_inside(文件夹名称), int Disp_str(显示指针), int name_str(文件名指针),int Outside_name_add(外部增加文件夹数)
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210914         
**********************************************************************************************************/
void Filelist_init(Seq_parameter_inside *fname_inside, int Disp_str, int name_str, int Outside_name_add)
{
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_inside_list;
	lv_style_reset(&style_inside_list);
	lv_style_init(&style_inside_list);
	lv_style_set_radius(&style_inside_list, 2);																				//设置边框圆角	
	lv_style_set_bg_color(&style_inside_list, lv_palette_lighten(LV_PALETTE_GREY, 2));										//设置上背景颜色
	lv_style_set_border_color(&style_inside_list, lv_palette_lighten(LV_PALETTE_GREY, 1));
	lv_style_set_shadow_width(&style_inside_list, 2);
    lv_style_set_shadow_ofs_y(&style_inside_list, 1);
    lv_style_set_shadow_opa(&style_inside_list, LV_OPA_50);
	lv_style_set_border_width(&style_inside_list, 1);																			//设置边框宽度
	lv_style_set_text_color(&style_inside_list, lv_color_black());																//设置样式中文字颜色		
	lv_style_set_text_font(&style_inside_list, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_inside_list, 156);																				//设置样式宽度
	lv_style_set_height(&style_inside_list, 18);																				//设置样式高度
	
	static lv_style_t 	style_outside_list;
	lv_style_reset(&style_outside_list);
	lv_style_init(&style_outside_list);
	lv_style_set_radius(&style_outside_list, 2);																				//设置边框圆角	
	lv_style_set_bg_color(&style_outside_list, lv_color_white());																//设置上背景颜色
	lv_style_set_border_color(&style_outside_list, lv_palette_lighten(LV_PALETTE_GREY, 1));
	lv_style_set_shadow_width(&style_outside_list, 2);
    lv_style_set_shadow_ofs_y(&style_outside_list, 1);
    lv_style_set_shadow_opa(&style_outside_list, LV_OPA_50);
	lv_style_set_border_width(&style_outside_list, 1);																			//设置边框宽度
	lv_style_set_text_color(&style_outside_list, lv_color_black());																//设置样式中文字颜色		
	lv_style_set_text_font(&style_outside_list, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_outside_list, 156);																				//设置样式宽度
	lv_style_set_height(&style_outside_list, 18);																				//设置样式高度
	
	static lv_style_t 	style_set_list_pr;																					//按压状态
	lv_style_reset(&style_set_list_pr);
	lv_style_init(&style_set_list_pr);
	lv_style_set_radius(&style_set_list_pr, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_set_list_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE,1));									//设置上背景颜色
	lv_style_set_border_color(&style_set_list_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
	lv_style_set_shadow_width(&style_set_list_pr, 2);
    lv_style_set_shadow_ofs_y(&style_set_list_pr, 1);
    lv_style_set_shadow_opa(&style_set_list_pr, LV_OPA_50);
	lv_style_set_border_width(&style_set_list_pr, 1);	
	lv_style_set_border_width(&style_set_list_pr, 1);																		//设置边框宽度
	lv_style_set_text_color(&style_set_list_pr, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_set_list_pr, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_set_list_pr, 156);																			//设置样式宽度
	lv_style_set_height(&style_set_list_pr, 18);																			//设置样式高度
	
	
	lv_obj_t * text_name1 = lv_obj_create(lv_scr_act());	
	if((name_str -Disp_str) >= Outside_name_add)
	{
		lv_obj_add_style(text_name1 , &style_inside_list, 0);																//加载一种样式
	}else
	{	
		lv_obj_add_style(text_name1 , &style_outside_list, 0);																//加载一种样式
	}
	lv_obj_set_scrollbar_mode(text_name1 ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_name1 , LV_ALIGN_TOP_MID, 0, 1);																	//设置目标位置
	
	
	lv_obj_t * text_name2 = lv_obj_create(lv_scr_act());
	if((name_str -Disp_str + 1) >= Outside_name_add)
	{
		lv_obj_add_style(text_name2 , &style_inside_list, 0);																//加载一种样式
	}else
	{
		lv_obj_add_style(text_name2 , &style_outside_list, 0);																//加载一种样式
	}
	lv_obj_set_scrollbar_mode(text_name2 ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_name2 , LV_ALIGN_TOP_MID, 0, 21);																	//设置目标位置
	
	
	lv_obj_t * text_name3 = lv_obj_create(lv_scr_act());	
	if((name_str -Disp_str + 2) >= Outside_name_add)
	{
		lv_obj_add_style(text_name3 , &style_inside_list, 0);																//加载一种样式
	}else
	{
		lv_obj_add_style(text_name3 , &style_outside_list, 0);																//加载一种样式
	}
	lv_obj_set_scrollbar_mode(text_name3 ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_name3 , LV_ALIGN_TOP_MID, 0, 41);																	//设置目标位置

	
	lv_obj_t * text_name4 = lv_obj_create(lv_scr_act());	
	if((name_str -Disp_str +3) >= Outside_name_add)
	{
		lv_obj_add_style(text_name4 , &style_inside_list, 0);																//加载一种样式
	}else
	{
		lv_obj_add_style(text_name4 , &style_outside_list, 0);																//加载一种样式
	}
	lv_obj_set_scrollbar_mode(text_name4 ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_name4 , LV_ALIGN_TOP_MID, 0, 61);																	//设置目标位置
	
	switch(Disp_str)
	{
		case 0:		{
						lv_obj_add_style(text_name1 , &style_set_list_pr, 0);												//加载一种样式
						lv_obj_set_scrollbar_mode(text_name1 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
						break;
					}
		case 1:		{
						lv_obj_add_style(text_name2 , &style_set_list_pr, 0);												//加载一种样式
						lv_obj_set_scrollbar_mode(text_name2 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
						break;
					}
		case 2:		{
						lv_obj_add_style(text_name3 , &style_set_list_pr, 0);												//加载一种样式
						lv_obj_set_scrollbar_mode(text_name3 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
						break;
					}
		case 3:		{
						lv_obj_add_style(text_name4 , &style_set_list_pr, 0);												//加载一种样式
						lv_obj_set_scrollbar_mode(text_name4 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
						break;
					}
		default: 		break;	
	}
	
	if((name_str -Disp_str) == -1)
	{
		lv_obj_t * name1_label = lv_label_create(text_name1);																//创建标签   name1_label
		lv_label_set_text(name1_label, NEWFILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "New File");														//设置动态文本(字符串形式)	
		lv_obj_align(name1_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}
	else 
	{
		if((name_str -Disp_str) >= Outside_name_add )
		{
			lv_obj_t * name1_label = lv_label_create(text_name1);															//创建标签   name1_label
			lv_label_set_text(name1_label, FILE" ");																		//设置动态文本(字符串形式)
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, fname_inside[(name_str - Disp_str - Outside_name_add)].Name);	//设置动态文本(字符串形式)	
			lv_obj_align(name1_label , LV_ALIGN_LEFT_MID, -5, -1);															//设置目标位置
		}else
		{
			lv_obj_t * name1_label = lv_label_create(text_name1);															//创建标签   name1_label
			lv_label_set_text(name1_label, FILE" ");																		//设置动态文本(字符串形式)
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, ReName[(name_str -Disp_str)]);								//设置动态文本(字符串形式)	
			lv_obj_align(name1_label , LV_ALIGN_LEFT_MID, -5, -1);															//设置目标位置
		}	
	}
	
	if(((name_str -Disp_str)+1) >= Outside_name_add )
	{
		lv_obj_t * name2_label = lv_label_create(text_name2);																//创建标签   name1_label
		lv_label_set_text(name2_label, FILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, fname_inside[(name_str - Disp_str - Outside_name_add+1)].Name);	//设置动态文本(字符串形式)	
		lv_obj_align(name2_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}else
	{
		lv_obj_t * name2_label = lv_label_create(text_name2);																//创建标签   name1_label
		lv_label_set_text(name2_label, FILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, ReName[(name_str -Disp_str + 1)]);								//设置动态文本(字符串形式)	
		lv_obj_align(name2_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}	
	
	if(((name_str -Disp_str)+2) >= Outside_name_add )
	{
		lv_obj_t * name3_label = lv_label_create(text_name3);																//创建标签   name1_label
		lv_label_set_text(name3_label, FILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name3_label, LV_LABEL_POS_LAST, fname_inside[(name_str - Disp_str - Outside_name_add+2)].Name);	//设置动态文本(字符串形式)	
		lv_obj_align(name3_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}else
	{
		lv_obj_t * name3_label = lv_label_create(text_name3);																//创建标签   name1_label
		lv_label_set_text(name3_label, FILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name3_label, LV_LABEL_POS_LAST, ReName[(name_str -Disp_str + 2)]);								//设置动态文本(字符串形式)	
		lv_obj_align(name3_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}	
	

	if(((name_str -Disp_str)+3) >= Outside_name_add )
	{
		lv_obj_t * name4_label = lv_label_create(text_name4);																//创建标签   name1_label
		lv_label_set_text(name4_label, FILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name4_label, LV_LABEL_POS_LAST, fname_inside[(name_str - Disp_str - Outside_name_add+3)].Name);	//设置动态文本(字符串形式)	
		lv_obj_align(name4_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}else
	{
		lv_obj_t * name4_label = lv_label_create(text_name4);																//创建标签   name1_label
		lv_label_set_text(name4_label, FILE" ");																			//设置动态文本(字符串形式)
		lv_label_ins_text(name4_label, LV_LABEL_POS_LAST, ReName[(name_str -Disp_str + 3)]);								//设置动态文本(字符串形式)	
		lv_obj_align(name4_label , LV_ALIGN_LEFT_MID, -5, -1);																//设置目标位置
	}	
}

/**********************************************************************************************************
*	函 数 名: void Seq_list_init(Seq_parameter_inside *fSeq_Inside, int name_str, int Seq_Disp_str, int Seq_parameter_str)
*	功能说明: 内置错列表
*	形    参: Seq_parameter_inside *fSeq_Inside(显示的参数), int Seq_Disp_str(滚动的指针), int Seq_parameter_str(第几行参数)
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210914         
**********************************************************************************************************/
void Seq_list_init(Seq_parameter_inside *fSeq_Inside, int name_str, int Seq_Disp_str, int Seq_parameter_str)
{
	uint8_t color_select;
	uint8_t seq_len;
	
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_list_theme;
	lv_style_reset(&style_list_theme);
	lv_style_init(&style_list_theme);
	lv_style_set_radius(&style_list_theme, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_list_theme, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE,3));											//设置上背景颜色
	lv_style_set_bg_grad_color(&style_list_theme, lv_palette_darken(LV_PALETTE_BLUE,3));
	lv_style_set_bg_grad_dir(&style_list_theme, LV_GRAD_DIR_HOR);
	
	lv_style_set_border_width(&style_list_theme, 0);																		//设置边框宽度
	lv_style_set_text_color(&style_list_theme, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_list_theme, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_list_theme, 160);																				//设置样式宽度
	lv_style_set_height(&style_list_theme, 23);																				//设置样式高度
		
	//设置一个主题显示按钮
	lv_obj_t * text_list_theme  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_list_theme , &style_list_theme, 0);																//加载一种样式
	lv_obj_set_scrollbar_mode(text_list_theme ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_list_theme , LV_ALIGN_TOP_MID, 0, 0);																	//设置目标位置
	lv_obj_t * name_list_label = lv_label_create(text_list_theme);
	lv_label_set_text(name_list_label, fSeq_Inside[name_str].Name);														//设置动态文本(字符串形式)
	lv_obj_align(name_list_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
	
	static lv_style_t 	style_list_data;
	lv_style_reset(&style_list_data);
	lv_style_init(&style_list_data);
	lv_style_set_radius(&style_list_data, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_list_data, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_list_data, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_list_data, lv_color_black());															//设置样式中文字颜色
	lv_style_set_text_font(&style_list_data, &lv_font_yahei_16_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_list_data, 160);																				//设置样式宽度
	lv_style_set_height(&style_list_data, 57);
	
	static lv_style_t 	style_list_dw;
	lv_style_reset(&style_list_dw);
	lv_style_init(&style_list_dw);
	lv_style_set_bg_opa(&style_list_dw, LV_OPA_0);																			//设置背景透明度
	lv_style_set_shadow_opa(&style_list_dw, LV_OPA_0);																		//设置阴影透明度
	lv_style_set_border_opa(&style_list_dw, LV_OPA_0);																		//设置边界透明度
	lv_style_set_text_color(&style_list_dw, lv_color_black());																//设置样式中文字颜色
	lv_style_set_text_font(&style_list_dw, &lv_font_yahei_12_bpp4);															//设置样式中文字大小
	lv_style_set_width(&style_list_dw, 160);																				//设置样式宽度
	lv_style_set_height(&style_list_dw, 57);
	
	//设置一个数据显示按钮
	lv_obj_t * text_list_data  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_list_data , &style_list_data, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_list_data ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_list_data , LV_ALIGN_BOTTOM_MID, 0, 0);																//设置目标位置

	//设置一个数据显示按钮
	lv_obj_t * text_list_dw  = lv_obj_create(lv_scr_act());																	//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_list_dw , &style_list_dw, 0);																		//加载一种样式
	lv_obj_set_scrollbar_mode(text_list_dw ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
	lv_obj_align(text_list_dw , LV_ALIGN_BOTTOM_MID, 0, 0);																	//设置目标位置

	/*******************************转速，rpm....****************************************************/
	//rpm的文件按钮上显示的单位文字
																//创建标签   rpm_DW_label
	if(fSeq_Inside[name_str].fseq[Seq_parameter_str].OperaMode == 0)
	{	
		lv_obj_t *data1_label = lv_label_create(text_list_data);	
		lv_label_set_text(data1_label, DispEnSpeedData[fSeq_Inside[name_str].fseq[Seq_parameter_str].speed]);				//设置动态文本(字符串形式)
		lv_obj_align(data1_label , LV_ALIGN_RIGHT_MID, -100, -10);															//设置目标位置
		
		lv_obj_t *data2_label = lv_label_create(text_list_data);	
		lv_label_set_text(data2_label, DispEnTorqueData[fSeq_Inside[name_str].fseq[Seq_parameter_str].torque]);				//设置动态文本(字符串形式)
		lv_obj_align(data2_label , LV_ALIGN_RIGHT_MID, -30, -10);															//设置目标位置
		
		lv_obj_t *dw1_label = lv_label_create(text_list_dw);	
		lv_label_set_text(dw1_label, "rpm");																				//设置动态文本(字符串形式)
		lv_obj_align(dw1_label , LV_ALIGN_RIGHT_MID, -67, -10);																//设置目标位置
		
		lv_obj_t *dw2_label = lv_label_create(text_list_dw);	
		lv_label_set_text(dw2_label, "N"CN_DIAN"cm");																		//设置动态文本(字符串形式)
		lv_obj_align(dw2_label , LV_ALIGN_RIGHT_MID, 12, -10);																//设置目标位置
		
	}else if(fSeq_Inside[name_str].fseq[Seq_parameter_str].OperaMode == 2)
	{
		lv_obj_t *data1_label = lv_label_create(text_list_data);	
		lv_label_set_text(data1_label, DispEnAngleCWData[fSeq_Inside[name_str].fseq[Seq_parameter_str].AngleCW]);			//设置动态文本(字符串形式)
//		lv_label_set_text(data1_label, "360");			//设置动态文本(字符串形式)
		lv_label_ins_text(data1_label, LV_LABEL_POS_LAST, CN_DU);															//设置动态文本(字符串形式)
		lv_obj_align(data1_label , LV_ALIGN_LEFT_MID, 23, -10);																//设置目标位置
		
		lv_obj_t *data2_label = lv_label_create(text_list_data);	
		lv_label_set_text(data2_label, DispEnAngleCWData[fSeq_Inside[name_str].fseq[Seq_parameter_str].AngleCCW]);			//设置动态文本(字符串形式)
//		lv_label_set_text(data2_label, "360");			//设置动态文本(字符串形式)
		lv_label_ins_text(data2_label, LV_LABEL_POS_LAST, CN_DU);															//设置动态文本(字符串形式)
		lv_obj_align(data2_label , LV_ALIGN_LEFT_MID, 104, -10);																//设置目标位置	

		if( set.Language == EN)
		{
			lv_obj_t *dw1_label = lv_label_create(text_list_dw);
			lv_label_set_text(dw1_label, "Fwd:");																					//设置动态文本(字符串形式)
			lv_obj_align(dw1_label , LV_ALIGN_LEFT_MID, -15, -10);																//设置目标位置
			
			lv_obj_t *dw2_label = lv_label_create(text_list_dw);	
			lv_label_set_text(dw2_label, "Rev:");																				//设置动态文本(字符串形式)
			lv_obj_align(dw2_label , LV_ALIGN_LEFT_MID, 65, -10);																//设置目标位置	
		}else
		{
			lv_obj_t *dw1_label = lv_label_create(text_list_dw);
			lv_label_set_text(dw1_label, CN_ZHENG CN_ZHUAN":");																					//设置动态文本(字符串形式)
			lv_obj_align(dw1_label , LV_ALIGN_LEFT_MID, -15, -10);																//设置目标位置
			
			lv_obj_t *dw2_label = lv_label_create(text_list_dw);	
			lv_label_set_text(dw2_label, CN_FAN1 CN_ZHUAN ":");																				//设置动态文本(字符串形式)
			lv_obj_align(dw2_label , LV_ALIGN_LEFT_MID, 65, -10);																//设置目标位置	
		}
	}

	//创建按钮
	static lv_style_t 	style_set_seq;
	lv_style_reset(&style_set_seq);
	lv_style_init(&style_set_seq);
	lv_style_set_radius(&style_set_seq, 5);																					//设置边框圆角
	lv_style_set_bg_color(&style_set_seq, lv_color_white());																//设置上背景颜色
	lv_style_set_border_color(&style_set_seq, lv_palette_lighten(LV_PALETTE_GREY, 1));
	lv_style_set_shadow_width(&style_set_seq, 3);
    lv_style_set_shadow_ofs_y(&style_set_seq, 1);
    lv_style_set_shadow_opa(&style_set_seq, LV_OPA_50);
	lv_style_set_border_width(&style_set_seq, 2);																			//设置边框宽度
	lv_style_set_text_color(&style_set_seq, lv_color_black());																//设置样式中文字颜色		
	lv_style_set_text_font(&style_set_seq, &lv_font_yahei_12_bpp4);															//设置样式中文字大小
	lv_style_set_width(&style_set_seq, 50);																					//设置样式宽度
	lv_style_set_height(&style_set_seq, 22);																				//设置样式高度
	
	static lv_style_t 	style_set_seq_pr;																					//按压状态
	lv_style_reset(&style_set_seq_pr);
	lv_style_init(&style_set_seq_pr);
	lv_style_set_radius(&style_set_seq_pr, 5);																				//设置边框圆角
	switch(fSeq_Inside[name_str].fseq[Seq_parameter_str].color)
	{
		case c_white:	{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_GREY,5));				//设置白色
							break;
						}
		case c_hong:	{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_RED,1));					//设置红色
							break;
						}
		case c_zong:	{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_BROWN,1));				//设置棕色
							break;
						}
		case c_huang:	{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_YELLOW,1));				//设置黄色
							break;
						}
		case c_lv:		{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_GREEN,1));				//设置绿色
							break;
						}
		case c_fen:		{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_PINK,1));				//设置粉红
							break;
						}
		case c_lan:		{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_BLUE,1));				//设置蓝色
							break;
						}
		case c_zi:		{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_PURPLE,1));				//设置紫色
							break;
						}
		case c_hei:		{
							lv_style_set_bg_color(&style_set_seq_pr, lv_color_black());										//设置黑色
							break;
						}
		case c_hui:		{
							lv_style_set_bg_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_GREY,1));					//设置灰色
							break;
						}
		default: 		break;		
	}
	lv_style_set_border_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
	lv_style_set_shadow_width(&style_set_seq_pr, 3);
    lv_style_set_shadow_ofs_y(&style_set_seq_pr, 1);
    lv_style_set_shadow_opa(&style_set_seq_pr, LV_OPA_50);
	lv_style_set_border_width(&style_set_seq_pr, 2);																			//设置边框宽度
	if(fSeq_Inside[name_str].fseq[Seq_parameter_str].color == c_white)
	{
		lv_style_set_text_color(&style_set_seq_pr, lv_color_black());														//设置样式中文字颜色	
	}else
	{
		lv_style_set_text_color(&style_set_seq_pr, lv_color_white());														//设置样式中文字颜色
	}		
	lv_style_set_text_font(&style_set_seq_pr, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_set_seq_pr, 50);																				//设置样式宽度
	lv_style_set_height(&style_set_seq_pr, 22);																				//设置样式高度
	
	if(fSeq_Inside[name_str].seq_Len == 1)
	{
		lv_obj_t * text_seq1 = lv_obj_create(lv_scr_act());	
		lv_obj_add_style(text_seq1 , &style_set_seq_pr, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq1 , LV_ALIGN_BOTTOM_MID, 0, 0);																	//设置目标位置
		
		lv_obj_t * name1_label = lv_label_create(text_seq1);																	//创建标签   name1_label
		if(fSeq_Inside[name_str].fseq[Seq_parameter_str].seqName != 0)
		{
			lv_label_set_text(name1_label, fSeq_Inside[name_str].fseq[0].seqName);	
		}else
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[0].taper);
			lv_label_set_text(name1_label,buf);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[0].number);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
		}
		lv_obj_align(name1_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
	}else if(fSeq_Inside[name_str].seq_Len == 2)
	{
		lv_obj_t * text_seq1 = lv_obj_create(lv_scr_act());	
		lv_obj_add_style(text_seq1 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq1 , LV_ALIGN_BOTTOM_MID, -26, 0);																	//设置目标位置
		
		lv_obj_t * text_seq2 = lv_obj_create(lv_scr_act());
		lv_obj_add_style(text_seq2 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq2 , LV_ALIGN_BOTTOM_MID, 26, 0);																	//设置目标位置
		
		switch(Seq_parameter_str)
		{
			case 0:		{
							lv_obj_add_style(text_seq1 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			case 1:		{
							lv_obj_add_style(text_seq2 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			default: 		break;	
		}
		
		lv_obj_t * name1_label = lv_label_create(text_seq1);																	//创建标签   name1_label
		if(fSeq_Inside[name_str].fseq[0].seqName != 0)
		{
			lv_label_set_text(name1_label, fSeq_Inside[name_str].fseq[0].seqName);	
		}else
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[0].taper);
			lv_label_set_text(name1_label,buf);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[0].number);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
		}
		lv_obj_align(name1_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
		
		lv_obj_t * name2_label = lv_label_create(text_seq2);																	//创建标签   name1_label
		if(fSeq_Inside[name_str].fseq[1].seqName != 0)
		{
			lv_label_set_text(name2_label, fSeq_Inside[name_str].fseq[1].seqName);	
		}else
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[1].taper);
			lv_label_set_text(name2_label,buf);
			lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[1].number);
			lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)			
		}
		lv_obj_align(name2_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
	}else if(fSeq_Inside[name_str].seq_Len >= 3)
	{
		lv_obj_t * text_seq1 = lv_obj_create(lv_scr_act());	
		lv_obj_add_style(text_seq1 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq1 , LV_ALIGN_BOTTOM_MID, -52, 0);																	//设置目标位置
		
		lv_obj_t * text_seq2 = lv_obj_create(lv_scr_act());
		lv_obj_add_style(text_seq2 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq2 , LV_ALIGN_BOTTOM_MID, 0, 0);																	//设置目标位置
		
		lv_obj_t * text_seq3 = lv_obj_create(lv_scr_act());
		lv_obj_add_style(text_seq3 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq3 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq3 , LV_ALIGN_BOTTOM_MID, 52, 0);																	//设置目标位置
		
		switch(Seq_Disp_str)
		{
			case 0:		{
							lv_obj_add_style(text_seq1 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			case 1:		{
							lv_obj_add_style(text_seq2 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			case 2:		{
							lv_obj_add_style(text_seq3 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq3 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			default: 		break;	
		}
		
		lv_obj_t * name1_label = lv_label_create(text_seq1);																	//创建标签   name1_label
		if(fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str].seqName != 0)
		{
			lv_label_set_text(name1_label, fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str].seqName);	
		}else
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str].taper);
			lv_label_set_text(name1_label,buf);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str].number);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
		}
		lv_obj_align(name1_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
		
		lv_obj_t * name2_label = lv_label_create(text_seq2);																	//创建标签   name1_label
		if(fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +1].seqName != 0)
		{
			lv_label_set_text(name2_label, fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +1].seqName);	
		}else
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +1].taper);
			lv_label_set_text(name2_label,buf);
			lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +1].number);
			lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)			
		}
		lv_obj_align(name2_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
		
		lv_obj_t * name3_label = lv_label_create(text_seq3);																	//创建标签   name1_label
		if(fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +2].seqName != 0)
		{
			lv_label_set_text(name3_label, fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +2].seqName);	
		}else
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +2].taper);
			lv_label_set_text(name3_label,buf);
			lv_label_ins_text(name3_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fSeq_Inside[name_str].fseq[Seq_parameter_str - Seq_Disp_str +2].number);
			lv_label_ins_text(name3_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
		}
		lv_obj_align(name3_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
	}	
}
/**********************************************************************************************************
*	函 数 名: void Seq_Outsidelist_init(Seq_parameter_outside fNew_file_parameter, int name_str, int Seq_Disp_str, int Seq_parameter_str)
*	功能说明: 内置错列表
*	形    参: Seq_parameter_outside fNew_file_parameter, int name_str, int Seq_Disp_str, int Seq_parameter_str
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210918         
**********************************************************************************************************/
void Seq_Outsidelist_init(Seq_parameter_outside fNew_file_parameter, int name_str, int Seq_Disp_str, int Seq_parameter_str)
{
	uint8_t color_select;
	uint8_t seq_len;
	
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_list_theme;
	lv_style_reset(&style_list_theme);
	lv_style_init(&style_list_theme);
	lv_style_set_radius(&style_list_theme, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_list_theme, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE,3));											//设置上背景颜色
	lv_style_set_bg_grad_color(&style_list_theme, lv_palette_darken(LV_PALETTE_BLUE,3));
	lv_style_set_bg_grad_dir(&style_list_theme, LV_GRAD_DIR_HOR);
	
	lv_style_set_border_width(&style_list_theme, 0);																		//设置边框宽度
	lv_style_set_text_color(&style_list_theme, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_list_theme, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_list_theme, 160);																				//设置样式宽度
	lv_style_set_height(&style_list_theme, 23);																				//设置样式高度
		
	//设置一个主题显示按钮
	lv_obj_t * text_list_theme  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_list_theme , &style_list_theme, 0);																//加载一种样式
	lv_obj_set_scrollbar_mode(text_list_theme ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_list_theme , LV_ALIGN_TOP_MID, 0, 0);																	//设置目标位置
	
	lv_obj_t * name_list_label = lv_label_create(text_list_theme);
	lv_label_set_text(name_list_label, ReName[name_str]);																	//设置动态文本(字符串形式)
	lv_obj_align(name_list_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
	
	static lv_style_t 	style_list_data;
	lv_style_reset(&style_list_data);
	lv_style_init(&style_list_data);
	lv_style_set_radius(&style_list_data, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_list_data, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_list_data, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_list_data, lv_color_black());															//设置样式中文字颜色
	lv_style_set_text_font(&style_list_data, &lv_font_yahei_16_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_list_data, 160);																				//设置样式宽度
	lv_style_set_height(&style_list_data, 57);
	
	static lv_style_t 	style_list_dw;
	lv_style_reset(&style_list_dw);
	lv_style_init(&style_list_dw);
	lv_style_set_bg_opa(&style_list_dw, LV_OPA_0);																			//设置背景透明度
	lv_style_set_shadow_opa(&style_list_dw, LV_OPA_0);																		//设置阴影透明度
	lv_style_set_border_opa(&style_list_dw, LV_OPA_0);																		//设置边界透明度
	lv_style_set_text_color(&style_list_dw, lv_color_black());																//设置样式中文字颜色
	lv_style_set_text_font(&style_list_dw, &lv_font_yahei_12_bpp4);															//设置样式中文字大小
	lv_style_set_width(&style_list_dw, 160);																				//设置样式宽度
	lv_style_set_height(&style_list_dw, 57);
	
	//设置一个数据显示按钮
	lv_obj_t * text_list_data  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_list_data , &style_list_data, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_list_data ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_list_data , LV_ALIGN_BOTTOM_MID, 0, 0);																//设置目标位置

	//设置一个数据显示按钮
	lv_obj_t * text_list_dw  = lv_obj_create(lv_scr_act());																	//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_list_dw , &style_list_dw, 0);																		//加载一种样式
	lv_obj_set_scrollbar_mode(text_list_dw ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
	lv_obj_align(text_list_dw , LV_ALIGN_BOTTOM_MID, 0, 0);																	//设置目标位置

	/*******************************转速，rpm....****************************************************/
	//rpm的文件按钮上显示的单位文字
	if(Seq_parameter_str >= fNew_file_parameter.seq_Len)
	{
		lv_obj_t *data1_label = lv_label_create(text_list_data);
		if(set.Language == EN)
		{
			lv_label_set_text(data1_label, "Add a bit");					//设置动态文本(字符串形式)
		}else
		{
			lv_label_set_text(data1_label, CN_ZENG CN_JIA1 CN_YI1 CN_GE CN_CUO1);					//设置动态文本(字符串形式)
		}
		lv_obj_align(data1_label , LV_ALIGN_CENTER, 0, -10);															//设置目标位置
	}
	else
	{
		if((fNew_file_parameter.fseq[Seq_parameter_str].OperaMode == 0)||(fNew_file_parameter.fseq[Seq_parameter_str].OperaMode == 1))
		{	
			lv_obj_t *data1_label = lv_label_create(text_list_data);	
			lv_label_set_text(data1_label, DispEnSpeedData[fNew_file_parameter.fseq[Seq_parameter_str].speed]);					//设置动态文本(字符串形式)
			lv_obj_align(data1_label , LV_ALIGN_RIGHT_MID, -100, -10);															//设置目标位置
			
			lv_obj_t *data2_label = lv_label_create(text_list_data);	
			lv_label_set_text(data2_label, DispEnTorqueData[fNew_file_parameter.fseq[Seq_parameter_str].torque]);				//设置动态文本(字符串形式)
			lv_obj_align(data2_label , LV_ALIGN_RIGHT_MID, -30, -10);															//设置目标位置
			
			lv_obj_t *dw1_label = lv_label_create(text_list_dw);	
			lv_label_set_text(dw1_label, "rpm");																				//设置动态文本(字符串形式)
			lv_obj_align(dw1_label , LV_ALIGN_RIGHT_MID, -67, -10);																//设置目标位置
			
			lv_obj_t *dw2_label = lv_label_create(text_list_dw);	
			lv_label_set_text(dw2_label, "N"CN_DIAN"cm");																		//设置动态文本(字符串形式)
			lv_obj_align(dw2_label , LV_ALIGN_RIGHT_MID, 12, -10);																//设置目标位置
			
		}else if((fNew_file_parameter.fseq[Seq_parameter_str].OperaMode == 2)||(fNew_file_parameter.fseq[Seq_parameter_str].OperaMode == 3))
		{
			lv_obj_t *data1_label = lv_label_create(text_list_data);	
			lv_label_set_text(data1_label, DispEnAngleCWData[fNew_file_parameter.fseq[Seq_parameter_str].AngleCW]);			//设置动态文本(字符串形式)
			lv_label_ins_text(data1_label, LV_LABEL_POS_LAST, CN_DU);															//设置动态文本(字符串形式)
			lv_obj_align(data1_label , LV_ALIGN_LEFT_MID, 22, -10);																//设置目标位置
			
			lv_obj_t *data2_label = lv_label_create(text_list_data);	
			lv_label_set_text(data2_label, DispEnAngleCWData[fNew_file_parameter.fseq[Seq_parameter_str].AngleCCW]);			//设置动态文本(字符串形式)
			lv_label_ins_text(data2_label, LV_LABEL_POS_LAST, CN_DU);															//设置动态文本(字符串形式)
			lv_obj_align(data2_label , LV_ALIGN_LEFT_MID, 100, -10);																//设置目标位置	
			
			if( set.Language == EN)
			{
				lv_obj_t *dw1_label = lv_label_create(text_list_dw);
				lv_label_set_text(dw1_label, "Fwd:");																					//设置动态文本(字符串形式)
				lv_obj_align(dw1_label , LV_ALIGN_LEFT_MID, -15, -10);																//设置目标位置
				
				lv_obj_t *dw2_label = lv_label_create(text_list_dw);	
				lv_label_set_text(dw2_label, "Rev:");																				//设置动态文本(字符串形式)
				lv_obj_align(dw2_label , LV_ALIGN_LEFT_MID, 65, -10);																//设置目标位置	
			}else
			{
				lv_obj_t *dw1_label = lv_label_create(text_list_dw);
				lv_label_set_text(dw1_label, CN_ZHENG CN_ZHUAN":");																					//设置动态文本(字符串形式)
				lv_obj_align(dw1_label , LV_ALIGN_LEFT_MID, -15, -10);																//设置目标位置
				
				lv_obj_t *dw2_label = lv_label_create(text_list_dw);	
				lv_label_set_text(dw2_label, CN_FAN1 CN_ZHUAN ":");																				//设置动态文本(字符串形式)
				lv_obj_align(dw2_label , LV_ALIGN_LEFT_MID, 65, -10);																//设置目标位置	
			}	
		}
	}

	//创建按钮
	static lv_style_t 	style_set_seq;
	lv_style_reset(&style_set_seq);
	lv_style_init(&style_set_seq);
	lv_style_set_radius(&style_set_seq, 5);																					//设置边框圆角
	lv_style_set_bg_color(&style_set_seq, lv_color_white());																//设置上背景颜色
	lv_style_set_border_color(&style_set_seq, lv_palette_lighten(LV_PALETTE_GREY, 1));
	lv_style_set_shadow_width(&style_set_seq, 3);
    lv_style_set_shadow_ofs_y(&style_set_seq, 1);
    lv_style_set_shadow_opa(&style_set_seq, LV_OPA_50);
	lv_style_set_border_width(&style_set_seq, 2);																			//设置边框宽度
	lv_style_set_text_color(&style_set_seq, lv_color_black());																//设置样式中文字颜色		
	lv_style_set_text_font(&style_set_seq, &lv_font_yahei_12_bpp4);															//设置样式中文字大小
	lv_style_set_width(&style_set_seq, 50);																					//设置样式宽度
	lv_style_set_height(&style_set_seq, 22);																				//设置样式高度
	
	static lv_style_t 	style_set_seq_pr;																					//按压状态
	lv_style_reset(&style_set_seq_pr);
	lv_style_init(&style_set_seq_pr);
	lv_style_set_radius(&style_set_seq_pr, 5);																				//设置边框圆角
	
	if(Seq_parameter_str >= fNew_file_parameter.seq_Len)
	{
		lv_style_set_bg_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE,1));									//设置上背景颜色
		lv_style_set_text_color(&style_set_seq_pr, lv_color_white());															//设置样式中文字颜色
	}else
	{
		switch(fNew_file_parameter.fseq[Seq_parameter_str].color)
		{
			case c_white:	{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_GREY,5));				//设置白色
								break;
							}
			case c_hong:	{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_RED,1));					//设置红色
								break;
							}
			case c_zong:	{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_BROWN,1));				//设置棕色
								break;
							}
			case c_huang:	{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_YELLOW,1));				//设置黄色
								break;
							}
			case c_lv:		{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_GREEN,1));				//设置绿色
								break;
							}
			case c_fen:		{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_PINK,1));				//设置粉红
								break;
							}
			case c_lan:		{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_BLUE,1));				//设置蓝色
								break;
							}
			case c_zi:		{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_lighten(LV_PALETTE_PURPLE,1));				//设置紫色
								break;
							}
			case c_hei:		{
								lv_style_set_bg_color(&style_set_seq_pr, lv_color_black());										//设置黑色
								break;
							}
			case c_hui:		{
								lv_style_set_bg_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_GREY,1));					//设置灰色
								break;
							}
			default: 		break;		
		}
		if(fNew_file_parameter.fseq[Seq_parameter_str].color == c_white)
		{
			lv_style_set_text_color(&style_set_seq_pr, lv_color_black());														//设置样式中文字颜色	
		}else
		{
			lv_style_set_text_color(&style_set_seq_pr, lv_color_white());														//设置样式中文字颜色
		}	
	}
	lv_style_set_border_color(&style_set_seq_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
	lv_style_set_shadow_width(&style_set_seq_pr, 3);
    lv_style_set_shadow_ofs_y(&style_set_seq_pr, 1);
    lv_style_set_shadow_opa(&style_set_seq_pr, LV_OPA_50);
	lv_style_set_border_width(&style_set_seq_pr, 2);																			//设置边框宽度
	
	lv_style_set_text_font(&style_set_seq_pr, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_set_seq_pr, 50);																				//设置样式宽度
	lv_style_set_height(&style_set_seq_pr, 22);																				//设置样式高度
	
//	if(fNew_file_parameter.seq_Len == 1)
//	{
//		lv_obj_t * text_seq1 = lv_obj_create(lv_scr_act());	
//		lv_obj_add_style(text_seq1 , &style_set_seq_pr, 0);																		//加载一种样式
//		lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
//		lv_obj_align(text_seq1 , LV_ALIGN_BOTTOM_MID, 0, 1);																	//设置目标位置
//		
//		lv_obj_t * name1_label = lv_label_create(text_seq1);																	//创建标签   name1_label
//		{
//			char buf[8];
//			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str].taper);
//			lv_label_set_text(name1_label,buf);
//			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
//			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str].number);
//			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
//		}
//		lv_obj_align(name1_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
//		
//	}else 
	if(fNew_file_parameter.seq_Len == 1)
	{
		lv_obj_t * text_seq1 = lv_obj_create(lv_scr_act());	
		lv_obj_add_style(text_seq1 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq1 , LV_ALIGN_BOTTOM_MID, -26, 0);																	//设置目标位置
		
		lv_obj_t * text_seq2 = lv_obj_create(lv_scr_act());
		lv_obj_add_style(text_seq2 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq2 , LV_ALIGN_BOTTOM_MID, 26, 0);																	//设置目标位置
		
		switch(Seq_parameter_str)
		{
			case 0:		{
							lv_obj_add_style(text_seq1 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			case 1:		{
							lv_obj_add_style(text_seq2 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			default: 		break;	
		}
		
		lv_obj_t * name1_label = lv_label_create(text_seq1);																	//创建标签   name1_label
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[0].taper);
			lv_label_set_text(name1_label,buf);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[0].number);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
		}
		lv_obj_align(name1_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置
		
		
		lv_obj_t * name2_label = lv_label_create(text_seq2);																	//创建标签   name1_label
		{
			lv_label_set_text(name2_label,"+");			
		}
		lv_obj_align(name2_label , LV_ALIGN_CENTER, 0, 0);																		//设置目标位置
		
	}else if(fNew_file_parameter.seq_Len >= 2)
	{
		lv_obj_t * text_seq1 = lv_obj_create(lv_scr_act());	
		lv_obj_add_style(text_seq1 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq1 , LV_ALIGN_BOTTOM_MID, -52, 0);																	//设置目标位置
		
		lv_obj_t * text_seq2 = lv_obj_create(lv_scr_act());
		lv_obj_add_style(text_seq2 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq2 , LV_ALIGN_BOTTOM_MID, 0, 0);																	//设置目标位置
		
		lv_obj_t * text_seq3 = lv_obj_create(lv_scr_act());
		lv_obj_add_style(text_seq3 , &style_set_seq, 0);																		//加载一种样式
		lv_obj_set_scrollbar_mode(text_seq3 ,LV_SCROLLBAR_MODE_OFF);															//不显示滚动条
		lv_obj_align(text_seq3 , LV_ALIGN_BOTTOM_MID, 52, 0);																	//设置目标位置
		
		switch(Seq_Disp_str)
		{
			case 0:		{
							lv_obj_add_style(text_seq1 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq1 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			case 1:		{
							lv_obj_add_style(text_seq2 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq2 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			case 2:		{
							lv_obj_add_style(text_seq3 , &style_set_seq_pr, 0);													//加载一种样式
							lv_obj_set_scrollbar_mode(text_seq3 ,LV_SCROLLBAR_MODE_OFF);										//不显示滚动条
							break;
						}
			default: 		break;	
		}
		
		lv_obj_t * name1_label = lv_label_create(text_seq1);																	//创建标签   name1_label
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str - Seq_Disp_str].taper);
			lv_label_set_text(name1_label,buf);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str - Seq_Disp_str].number);
			lv_label_ins_text(name1_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)				
		}
		lv_obj_align(name1_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
		
		lv_obj_t * name2_label = lv_label_create(text_seq2);																	//创建标签   name1_label
		{
			char buf[8];
			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str - Seq_Disp_str +1].taper);
			lv_label_set_text(name2_label,buf);
			lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
			lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str - Seq_Disp_str +1].number);
			lv_label_ins_text(name2_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)			
		}
		lv_obj_align(name2_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
		
		
		lv_obj_t * name3_label = lv_label_create(text_seq3);																	//创建标签   name1_label
		{
			if((Seq_parameter_str - Seq_Disp_str +2) >= fNew_file_parameter.seq_Len)
			{
				lv_label_set_text(name3_label,"+");
			}else
			{
				char buf[8];
				lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str - Seq_Disp_str +2].taper);
				lv_label_set_text(name3_label,buf);
				lv_label_ins_text(name3_label, LV_LABEL_POS_LAST, "/");																//设置动态文本(字符串形式)	
				lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_parameter_str - Seq_Disp_str +2].number);
				lv_label_ins_text(name3_label, LV_LABEL_POS_LAST, buf);																//设置动态文本(字符串形式)	
			}				
		}
		lv_obj_align(name3_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
	}	
	
}

/**********************************************************************************************************
*	函 数 名: void New_File_init(int add_file_number, Seq_parameter_outside fNew_file_parameter, int Seq_const_len ,int Seq_str)
*	功能说明: 新建用户自定义锉列表
*	形    参: int add_file_number增加锉个数, Seq_parameter_outside fNew_file_parameter锉参数, int Seq_const_len 此锉文件原有的锉个数,int Seq_str新增的锉个数
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210914         
**********************************************************************************************************/
void New_File_init(int add_file_number, Seq_parameter_outside fNew_file_parameter, int Seq_const_len ,int Seq_str)
{
	char buf[8];
	lv_obj_del(sel_top);
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	style_new_theme;
	lv_style_reset(&style_new_theme);
	lv_style_init(&style_new_theme);
	lv_style_set_radius(&style_new_theme, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_new_theme, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE,3));											//设置上背景颜色
	lv_style_set_bg_grad_color(&style_new_theme, lv_palette_darken(LV_PALETTE_BLUE,3));
	lv_style_set_bg_grad_dir(&style_new_theme, LV_GRAD_DIR_HOR);
	
	lv_style_set_border_width(&style_new_theme, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_new_theme, lv_color_white());															//设置样式中文字颜色		
	lv_style_set_text_font(&style_new_theme, &lv_font_yahei_12_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_new_theme, 160);																				//设置样式宽度
	lv_style_set_height(&style_new_theme, 23);																				//设置样式高度
	
	//设置一个主题显示按钮
	lv_obj_t * text_new_theme  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_new_theme , &style_new_theme, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_new_theme ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_new_theme , LV_ALIGN_TOP_MID, 0, 0);																	//设置目标位置
	
	static lv_style_t 	style_new_data;
	lv_style_reset(&style_new_data);
	lv_style_init(&style_new_data);
	lv_style_set_radius(&style_new_data, 0);																				//设置边框圆角
	lv_style_set_bg_color(&style_new_data, lv_color_white());																//设置上背景颜色
	lv_style_set_border_width(&style_new_data, 0);																			//设置边框宽度
	lv_style_set_text_color(&style_new_data, lv_color_black());																//设置样式中文字颜色
	lv_style_set_text_font(&style_new_data, &lv_font_yahei_16_bpp4);														//设置样式中文字大小
	lv_style_set_width(&style_new_data, 160);																				//设置样式宽度
	lv_style_set_height(&style_new_data, 57);
	
	//设置一个数据显示按钮
	lv_obj_t * text_new_data  = lv_obj_create(lv_scr_act());																//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(text_new_data , &style_new_data, 0);																	//加载一种样式
	lv_obj_set_scrollbar_mode(text_new_data ,LV_SCROLLBAR_MODE_OFF);														//不显示滚动条
	lv_obj_align(text_new_data , LV_ALIGN_BOTTOM_MID, 0, 0);																//设置目标位置
	
	lv_obj_t * name_new_label = lv_label_create(text_new_theme);
	lv_obj_align(name_new_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
	
	lv_obj_t * data_new_label = lv_label_create(text_new_data);	
	lv_obj_align(data_new_label , LV_ALIGN_CENTER, 0, 0);																	//设置目标位置
	
	switch(new_file_flag)
	{
		case File_name_flag:		{																						//运动模式设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[0]);									//设置动态文本(字符串形式)		
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[0]);						//设置动态文本(字符串形式)		
										}	
										lv_label_set_text(data_new_label , ReName[add_file_number]);
										
										break;
									}
		case Seq_name_flag:			{																						//锉个数设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[1]);								//设置动态文本(字符串形式)
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[1]);						//设置动态文本(字符串形式)		
										}
										lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.seq_new_Len);
										lv_label_set_text(data_new_label , buf);
										
										break;
									}
		case Taper_flag:			{																						//锥度设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[2]);										//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[2]);						//设置动态文本(字符串形式)	
										}	
										lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_const_len+Seq_str].taper);
										lv_label_set_text(data_new_label , buf);
										
										break;
									}
		case Number_flag:			{																							//号数设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[3]);							//设置动态文本(字符串形式)		
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[3]);							//设置动态文本(字符串形式)	
										}	
										lv_snprintf(buf, sizeof(buf), "%d%", fNew_file_parameter.fseq[Seq_const_len+Seq_str].number);
										lv_label_set_text(data_new_label , buf);
										
										break;
									}
		case OperaMode_flag:		{																												//操作模式设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[4]);												//设置动态文本(字符串形式)	
											if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 0)
											{
												lv_label_set_text(data_new_label, "FWD");
											}else if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 1)
											{
												lv_label_set_text(data_new_label, "REV");
											}else if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 2)
											{
												lv_label_set_text(data_new_label, "REC");
											}else if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 3)
											{
												lv_label_set_text(data_new_label, "ATC");
											}	
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[4]);												//设置动态文本(字符串形式)	
											if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 0)
											{
												lv_label_set_text(data_new_label, DispCNSelOperationMode[0]);
											}else if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 1)
											{
												lv_label_set_text(data_new_label, DispCNSelOperationMode[1]);
											}else if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 2)
											{
												lv_label_set_text(data_new_label, DispCNSelOperationMode[2]);
											}else if(fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 3)
											{
												lv_label_set_text(data_new_label, DispCNSelOperationMode[3]);
											}	
										}
										
										break;
									}
		case RecSpeed_flag:			{																												//速度设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[5]);												//设置动态文本(字符串形式)
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[5]);												//设置动态文本(字符串形式)		
										}	
										if((fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 0) || (fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 1))
										{
											lv_label_set_text(data_new_label,DispEnSpeedData[fNew_file_parameter.fseq[Seq_const_len+Seq_str].speed]);						//设置具体参数标签文字
										}
										else if((fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 2) || (fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 3))
										{
											lv_label_set_text(data_new_label,DispEnRecSpeedData[fNew_file_parameter.fseq[Seq_const_len+Seq_str].RecSpeed]);				//设置具体参数标签文字
										}
										lv_label_ins_text(data_new_label, LV_LABEL_POS_LAST, " rpm");												//设置动态文本(字符串形式)
										
										break;
									}
		case torque_flag:			{																												//转矩设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[6]);												//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[6]);												//设置动态文本(字符串形式)	
										}	
										if((fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 0) || (fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 1))
										{
											lv_label_set_text(data_new_label,DispEnTorqueData[fNew_file_parameter.fseq[Seq_const_len+Seq_str].torque]);					//设置具体参数标签文字
										}
										lv_label_ins_text(data_new_label, LV_LABEL_POS_LAST, " N"CN_DIAN"cm");										//设置动态文本(字符串形式)
										
										
										break;
									}
		case AngleCW_flag:			{																												//正角度设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[7]);												//设置动态文本(字符串形式)		
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[7]);												//设置动态文本(字符串形式)	
										}	
										if((fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 2) || (fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 3))
										{
											lv_label_set_text(data_new_label, DispEnAngleCWData[fNew_file_parameter.fseq[Seq_const_len+Seq_str].AngleCW]);				//设置具体参数标签文字
										}
										lv_label_ins_text(data_new_label, LV_LABEL_POS_LAST, " "CN_DU);												//设置动态文本(字符串形式)
										break;
									}
		case AngleCCW_flag:			{																												//根测参考点设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[8]);												//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[8]);												//设置动态文本(字符串形式)	
										}
										if((fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 2) || (fNew_file_parameter.fseq[Seq_const_len+Seq_str].OperaMode == 3))
										{
											lv_label_set_text(data_new_label, DispEnAngleCCWData[fNew_file_parameter.fseq[Seq_const_len+Seq_str].AngleCCW]);				//设置具体参数标签文字
										}
										lv_label_ins_text(data_new_label, LV_LABEL_POS_LAST, " "CN_DU);												//设置动态文本(字符串形式)
										
										break;
									}
		case color_flag:			{																												//根测参考点设置
										if(set.Language == EN)
										{
											lv_label_set_text(name_new_label, DispENNEWSelectTitle[9]);														//设置动态文本(字符串形式)	
										}else
										{
											lv_label_set_text(name_new_label, DispCNNEWSelectTitle[9]);												//设置动态文本(字符串形式)	
										}
										static lv_style_t 	style_color;																			//设置锉号背景颜色
										lv_style_reset(&style_color);																				//必须复位，不然会出现内存泄漏
										lv_style_init(&style_color);
										lv_style_set_border_width(&style_color, 0);																	//设置边框宽度   宽度为0，无边框
										lv_style_set_shadow_width(&style_color, 0);																	//设置阴影边框宽度   宽度为0，无边框	
										lv_style_set_width(&style_color, 50);																		//设置样式宽度
										lv_style_set_height(&style_color, 22);																		//设置样式高度
										lv_style_set_radius(&style_color, 0);																		//设置边框圆角
										switch(fNew_file_parameter.fseq[Seq_const_len+Seq_str].color)
										{
											case c_white:	{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_GREY,5));			//设置白色
																break;
															}
											case c_hong:	{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_RED,1));			//设置红色
																break;
															}
											case c_zong:	{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_BROWN,1));		//设置棕色
																break;
															}
											case c_huang:	{
																lv_style_set_bg_color(&style_color, lv_palette_darken(LV_PALETTE_YELLOW,1));		//设置黄色
																break;
															}
											case c_lv:		{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_GREEN,1));		//设置绿色
																break;
															}
											case c_fen:		{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_PINK,1));			//设置粉红
																break;
															}
											case c_lan:		{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_BLUE,1));			//设置蓝色
																break;
															}
											case c_zi:		{
																lv_style_set_bg_color(&style_color, lv_palette_lighten(LV_PALETTE_PURPLE,1));		//设置紫色
																break;
															}
											case c_hei:		{
																lv_style_set_bg_color(&style_color, lv_color_black());								//设置黑色
																break;
															}
											case c_hui:		{
																lv_style_set_bg_color(&style_color, lv_palette_darken(LV_PALETTE_GREY,1));			//设置灰色
																break;
															}
											default: 		break;		
										}
										//设置背景块
										lv_obj_t * bg_seqn = lv_obj_create(text_new_data);															//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
										lv_obj_add_style(bg_seqn , &style_color, 0);																//加载一种样式
										lv_obj_set_scrollbar_mode(bg_seqn ,LV_SCROLLBAR_MODE_OFF);													//不显示滚动条
										lv_obj_align(bg_seqn , LV_ALIGN_CENTER, 0, 0);																//设置目标位置
										
										break;
									}
		default: 					break;			
	}
	
}

								//0     1   2   3   4   5   6   7  8   9    10           11         12    13 14  15  16  17  18  19  20  21  22  23       24          25  26  27  28  29  30  31  32     33
const char* keynameTab[34]=		{"123","q","w","e","r","t","y","u","i","o","p",LV_SYMBOL_BACKSPACE,"ABC","a","s","d","f","g","h","j","k","l","_","-",LV_SYMBOL_CLOSE,"z","x","c","v","b","n","m",",",LV_SYMBOL_OK};
const char* keynameTabBig[34]=	{"123","Q","W","E","R","T","Y","U","I","O","P",LV_SYMBOL_BACKSPACE,"abc","A","S","D","F","G","H","J","K","L","_","-",LV_SYMBOL_CLOSE,"Z","X","C","V","B","N","M",",",LV_SYMBOL_OK};

const char* key123nameTab[11]=	{"abc","0","1","2","3","4","5","6","7","8","9"};
	
/**********************************************************************************************************
*	函 数 名: void lv_keyboard_1(void)
*	功能说明: 显示键盘
*	形    参: uint8_t ShiftBig大小写切换
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void New_File_abckeyname(char* Rename, uint8_t ShiftBig, uint8_t key_str, uint8_t key_Pr)
{
	int i=0;  //循环计数

	
	lv_obj_t* rn;
	
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条	
		
	//创建按键样式
	static lv_style_t k_rel;
	lv_style_reset(&k_rel);
	lv_style_init(&k_rel);
	lv_style_set_radius(&k_rel, 1);
	lv_style_set_bg_color(&k_rel, lv_color_white());										//设置上背景颜色
	lv_style_set_border_color(&k_rel, lv_palette_lighten(LV_PALETTE_GREY, 1));
	lv_style_set_shadow_width(&k_rel, 1);
    lv_style_set_shadow_ofs_y(&k_rel, 1);
    lv_style_set_shadow_opa(&k_rel, LV_OPA_50);
	lv_style_set_border_width(&k_rel, 1);																			//设置边框宽度
	lv_style_set_text_color(&k_rel, lv_color_black());																//设置样式中文字颜色	
	lv_style_set_text_color(&k_rel, lv_color_black());
	if(key_Pr == 0)
	{
		lv_style_set_text_font(&k_rel, &lv_font_montserrat_8);
	}else
	{
		lv_style_set_text_font(&k_rel, &lv_font_yahei_12_bpp4);
	}

	static lv_style_t k_pr;
	lv_style_reset(&k_pr);
	lv_style_init(&k_pr);
	lv_style_set_radius(&k_pr, 1);
	lv_style_set_bg_color(&k_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE,1));																//设置上背景颜色
	lv_style_set_border_color(&k_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
	lv_style_set_shadow_width(&k_pr, 1);
    lv_style_set_shadow_ofs_y(&k_pr, 1);
    lv_style_set_shadow_opa(&k_pr, LV_OPA_50);
	lv_style_set_border_width(&k_pr, 1);																			//设置边框宽度
	lv_style_set_text_color(&k_pr, lv_color_white());
	if(key_Pr == 0)
	{
		lv_style_set_text_font(&k_pr, &lv_font_montserrat_8);
	}else
	{
		lv_style_set_text_font(&k_pr, &lv_font_yahei_12_bpp4);
	}
	
	if(key_Pr == 0)
	{
		//创建单个按键
		lv_obj_t *keyboard[34];
		for(i = 0; i < 34; i++)
		{
			keyboard[i] = lv_obj_create(lv_scr_act());
			lv_obj_set_scrollbar_mode(keyboard[i] ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
		}
		
		//创建按键标签
		lv_obj_t *keyname[34];
		for(i = 0; i < 34; i++)
		{
			keyname[i] = lv_label_create(keyboard[i]);
			lv_obj_set_scrollbar_mode(keyname[i] ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
			lv_obj_center(keyname[i]);
		}
		
		//设置标签文字
		for(i = 0; i < 34; i++)
		{
			if(ShiftBig==0)
			{
				lv_label_set_text(keyname[i],keynameTab[i]);
			}
			else
			{			 
				lv_label_set_text(keyname[i],keynameTabBig[i]);
			}
		}
		//设置按键大小
		do
		{
			lv_obj_set_size(keyboard[0],20,15);															//"123"
			lv_obj_set_size(keyboard[1],10,15);															//"q"
			lv_obj_set_size(keyboard[2],10,15);															//"w"
			lv_obj_set_size(keyboard[3],10,15);															//"e"
			lv_obj_set_size(keyboard[4],10,15);															//"r"
			lv_obj_set_size(keyboard[5],10,15);															//"t"
			lv_obj_set_size(keyboard[6],10,15);															//"y"
			lv_obj_set_size(keyboard[7],10,15);															//"u"
			lv_obj_set_size(keyboard[8],10,15);															//"i"
			lv_obj_set_size(keyboard[9],10,15);															//"o"
			lv_obj_set_size(keyboard[10],10,15);														//"p"
			lv_obj_set_size(keyboard[11],20,15);														//"BKSP"
			
			lv_obj_set_size(keyboard[12],25,15);														//"ABC"
			lv_obj_set_size(keyboard[13],10,15);														//"a"
			lv_obj_set_size(keyboard[14],10,15);														//"s"
			lv_obj_set_size(keyboard[15],10,15);														//"d"
			lv_obj_set_size(keyboard[16],10,15);														//"f"
			lv_obj_set_size(keyboard[17],10,15);														//"g"
			lv_obj_set_size(keyboard[18],10,15);														//"h"
			lv_obj_set_size(keyboard[19],10,15);														//"j"
			lv_obj_set_size(keyboard[20],10,15);														//"k"
			lv_obj_set_size(keyboard[21],10,15);														//"l"
			lv_obj_set_size(keyboard[22],10,15);														//"_"
			lv_obj_set_size(keyboard[23],15,15);														//"-"
			
			lv_obj_set_size(keyboard[24],30,15);														//LV_SYMBOL_CLOSE
			lv_obj_set_size(keyboard[25],10,15);														//"z"
			lv_obj_set_size(keyboard[26],10,15);														//"x"
			lv_obj_set_size(keyboard[27],10,15);														//"c"
			lv_obj_set_size(keyboard[28],10,15);														//"v"
			lv_obj_set_size(keyboard[29],10,15);														//"b"
			lv_obj_set_size(keyboard[30],10,15);														//"n"
			lv_obj_set_size(keyboard[31],10,15);														//"m"
			lv_obj_set_size(keyboard[32],10,15);														//","
			lv_obj_set_size(keyboard[33],32,15);														//LV_SYMBOL_OK
				
		}while(0);

		//设置按键排布位置
		do
		{
			lv_obj_set_pos(keyboard[0],	  0 +  3,31);											//"123"
			lv_obj_set_pos(keyboard[1],	 20 +  4,31);											//"q"
			lv_obj_set_pos(keyboard[2],	 30 +  5,31);											//"w"
			lv_obj_set_pos(keyboard[3],	 40 +  6,31);											//"e"
			lv_obj_set_pos(keyboard[4],	 50 +  7,31);											//"r"
			lv_obj_set_pos(keyboard[5],	 60 +  8,31);											//"t"
			lv_obj_set_pos(keyboard[6],	 70 +  9,31);											//"y"
			lv_obj_set_pos(keyboard[7],	 80 + 10,31);											//"u"
			lv_obj_set_pos(keyboard[8],	 90 + 11,31);											//"i"
			lv_obj_set_pos(keyboard[9],	100 + 12,31);											//"o"
			lv_obj_set_pos(keyboard[10],110 + 13,31);											//"p"
			lv_obj_set_pos(keyboard[11],120 + 14,31);											//"BKSP"
			
			lv_obj_set_pos(keyboard[12],  0 +  3,47);											//"ABC"				
			lv_obj_set_pos(keyboard[13], 25 +  4,47);											//"a"	
			lv_obj_set_pos(keyboard[14], 35 +  5,47);											//"s"	
			lv_obj_set_pos(keyboard[15], 45 +  6,47);											//"d"	
			lv_obj_set_pos(keyboard[16], 55 +  7,47);											//"f"	
			lv_obj_set_pos(keyboard[17], 65 +  8,47);											//"g"	
			lv_obj_set_pos(keyboard[18], 75 +  9,47);											//"h"	
			lv_obj_set_pos(keyboard[19], 85 + 10,47);											//"j"	
			lv_obj_set_pos(keyboard[20], 95 + 11,47);											//"k"	
			lv_obj_set_pos(keyboard[21],105 + 12,47);											//"l"	
			lv_obj_set_pos(keyboard[22],115 + 13,47);											//"_"	
			lv_obj_set_pos(keyboard[23],125 + 14,47);											//"-"	
			
			lv_obj_set_pos(keyboard[24],  0 +  3,63);											//LV_SYMBOL_CLOSE
			lv_obj_set_pos(keyboard[25], 30 +  4,63);											//"z"
			lv_obj_set_pos(keyboard[26], 40 +  5,63);											//"x"
			lv_obj_set_pos(keyboard[27], 50 +  6,63);											//"c"
			lv_obj_set_pos(keyboard[28], 60 +  7,63);											//"v"
			lv_obj_set_pos(keyboard[29], 70 +  8,63);											//"b"
			lv_obj_set_pos(keyboard[30], 80 +  9,63);											//"n"
			lv_obj_set_pos(keyboard[31], 90 + 10,63);											//"m"
			lv_obj_set_pos(keyboard[32],100 + 11,63);											//","
			lv_obj_set_pos(keyboard[33],110 + 12,63);											//LV_SYMBOL_OK

		}while(0);
		
		//设置按键样式
		for(i = 0; i < 34; i++)
		{
			if(i == key_str)																	//按键指针
			{
				lv_obj_add_style(keyboard[i],&k_pr,0);
			}else
			{
				lv_obj_add_style(keyboard[i],&k_rel,0);	
			}	
		}
	}else
	{
		lv_obj_t *keyboard[11];
		for(i = 0; i < 11; i++)
		{
			keyboard[i] = lv_obj_create(lv_scr_act());
			lv_obj_set_scrollbar_mode(keyboard[i] ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
		}
		
		//创建按键标签
		lv_obj_t *keyname[11];
		for(i = 0; i < 11; i++)
		{
			keyname[i] = lv_label_create(keyboard[i]);
			lv_obj_set_scrollbar_mode(keyname[i] ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
			lv_obj_center(keyname[i]);
		}
		
		for(i = 0; i < 11; i++)
		{
			lv_label_set_text(keyname[i],key123nameTab[i]);
		}
		//设置按键大小
		do
		{
			lv_obj_set_size(keyboard[0],49,20);															//"abc"
			lv_obj_set_size(keyboard[1],24,20);															//"0"
			lv_obj_set_size(keyboard[2],24,20);															//"1"
			lv_obj_set_size(keyboard[3],24,20);															//"2"
			lv_obj_set_size(keyboard[4],24,20);															//"3"
			lv_obj_set_size(keyboard[5],24,20);															//"4"
			lv_obj_set_size(keyboard[6],24,20);															//"5"
			lv_obj_set_size(keyboard[7],24,20);															//"6"
			lv_obj_set_size(keyboard[8],24,20);															//"7"
			lv_obj_set_size(keyboard[9],24,20);															//"8"
			lv_obj_set_size(keyboard[10],24,20);														//"9"			
		}while(0);
		
		//设置按键排布位置
		do
		{
			lv_obj_set_pos(keyboard[0],	  0 +  7,33);													//"abc"
			lv_obj_set_pos(keyboard[1],	 40 + 17,33);													//"0"
			lv_obj_set_pos(keyboard[2],	 60 + 22,33);													//"1"
			lv_obj_set_pos(keyboard[3],	 80 + 27,33);													//"2"
			lv_obj_set_pos(keyboard[4],	100 + 32,33);													//"3"
			
			lv_obj_set_pos(keyboard[5],	  0 +  7,56);													//"4"			
			lv_obj_set_pos(keyboard[6],	 20 + 12,56);													//"5"
			lv_obj_set_pos(keyboard[7],	 40 + 17,56);													//"6"
			lv_obj_set_pos(keyboard[8],	 60 + 22,56);													//"7"
			lv_obj_set_pos(keyboard[9],	 80 + 27,56);													//"8"
			lv_obj_set_pos(keyboard[10],100 + 32,56);													//"9"
		}while(0);
		
		//设置按键样式
		for(i = 0; i < 11; i++)
		{
			if(i == key_str)																	//按键指针
			{
				lv_obj_add_style(keyboard[i],&k_pr,0);
			}else
			{
				lv_obj_add_style(keyboard[i],&k_rel,0);	
			}	
		}	
	}
	lv_obj_t *Textbnt = lv_obj_create(lv_scr_act());
	lv_obj_add_style(Textbnt,&k_rel,0);	
	lv_obj_set_size(Textbnt,152,26);
	lv_obj_set_scrollbar_mode(Textbnt ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	lv_obj_align(Textbnt, LV_ALIGN_TOP_LEFT, 3, 2);
	
	static lv_style_t s_ta;
	lv_style_reset(&s_ta);
	lv_style_init(&s_ta);
	lv_style_set_bg_opa(&s_ta, LV_OPA_0);
	lv_style_set_border_opa(&s_ta, LV_OPA_0);																				//设置边框透明
	lv_style_set_text_color(&s_ta, lv_color_black());
	lv_style_set_text_font(&s_ta, &lv_font_yahei_12_bpp4);
	lv_style_set_text_align(&s_ta,LV_TEXT_ALIGN_CENTER);
	lv_style_set_border_width(&s_ta, 1);
	lv_style_set_radius(&s_ta, 1);

	
	//创建文本框
	lv_obj_t *TextArea = lv_textarea_create(lv_scr_act());
	lv_obj_set_scrollbar_mode(TextArea ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	lv_obj_align(TextArea, LV_ALIGN_TOP_LEFT, 3, -4);
	lv_textarea_set_one_line(TextArea,true);
	lv_textarea_set_align(TextArea,LV_TEXT_ALIGN_LEFT);
	lv_textarea_set_text(TextArea, Rename);
	lv_obj_add_state(TextArea,LV_STATE_FOCUSED);
//	lv_label_set_long_mode(TextArea,LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(TextArea,150);
//	lv_obj_set_size(TextArea,152,28);	
	lv_obj_add_style(TextArea,&s_ta,0);	
}
/**********************************************************************************************************
*	函 数 名: void lv_memory_full(uint8_t yes_on)
*	功能说明: 删除提示
*	形    参: uint8_t yes_on    yes或no选择
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20210812         
**********************************************************************************************************/
void lv_delete_style(uint8_t yes_on)
{
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
    /*A base style*/
    static lv_style_t style_delete;
	lv_style_reset(&style_delete);
    lv_style_init(&style_delete);
    lv_style_set_bg_color(&style_delete, lv_color_white());
    lv_style_set_border_color(&style_delete, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_delete, 2);
    lv_style_set_radius(&style_delete, 10);
    lv_style_set_shadow_width(&style_delete, 10);
    lv_style_set_shadow_ofs_y(&style_delete, 5);
    lv_style_set_shadow_opa(&style_delete, LV_OPA_50);
    lv_style_set_text_color(&style_delete, lv_color_black());
	lv_style_set_text_font(&style_delete, &lv_font_yahei_12_bpp4);
    lv_style_set_width(&style_delete, 140);
    lv_style_set_height(&style_delete, 70);
	
	lv_obj_t * obj_delete = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_delete, &style_delete, 0);
	lv_obj_set_scrollbar_mode(obj_delete ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
    lv_obj_center(obj_delete);
	
	lv_obj_t * delete_label = lv_label_create(obj_delete);
	if(set.Language == EN)
	{
		lv_label_set_text(delete_label, " REMOVE ");
		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"\n ");
		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"SETTING ?");
		lv_obj_align(delete_label, LV_ALIGN_TOP_MID, 0, -15);
	}else
	{
		lv_label_set_text(delete_label, CN_SHI CN_FOU CN_SHAN CN_CHU CN_CI1 CN_XIANG);
//		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"\n ");
		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"?");
		lv_obj_align(delete_label, LV_ALIGN_TOP_MID, 0, -3);
	}
	
	static lv_style_t style_YN;
	lv_style_reset(&style_YN);
    lv_style_init(&style_YN);
	lv_style_set_radius(&style_YN, 2);	
    lv_style_set_bg_color(&style_YN, lv_color_white());
	lv_style_set_border_color(&style_delete, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 1));
	lv_style_set_shadow_width(&style_YN, 4);
    lv_style_set_shadow_ofs_y(&style_YN, 2);
	lv_style_set_shadow_opa(&style_YN, LV_OPA_50);
	lv_style_set_border_width(&style_YN, 2);
    lv_style_set_text_color(&style_YN, lv_color_black());
	lv_style_set_text_font(&style_YN, &lv_font_yahei_12_bpp4);
    lv_style_set_width(&style_YN, 50);
    lv_style_set_height(&style_YN, 22);
	
	static lv_style_t style_YN_pr;
	lv_style_reset(&style_YN_pr);
    lv_style_init(&style_YN_pr);
	lv_style_set_radius(&style_YN_pr, 2);
    lv_style_set_bg_color(&style_YN_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE,1));
	lv_style_set_border_color(&style_YN_pr, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
	lv_style_set_shadow_width(&style_YN_pr, 4);
    lv_style_set_shadow_ofs_y(&style_YN_pr, 2);
    lv_style_set_shadow_opa(&style_YN_pr, LV_OPA_50);
	lv_style_set_border_width(&style_YN_pr, 2);
    lv_style_set_text_color(&style_YN_pr, lv_color_white());
	lv_style_set_text_font(&style_YN_pr, &lv_font_yahei_12_bpp4);
    lv_style_set_width(&style_YN_pr, 50);
    lv_style_set_height(&style_YN_pr, 22);
	
	if(yes_on)
	{
		lv_obj_t * yes_btn = lv_obj_create(obj_delete);
		lv_obj_add_style(yes_btn, &style_YN_pr, 0);
		lv_obj_set_scrollbar_mode(yes_btn ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
		lv_obj_align(yes_btn, LV_ALIGN_BOTTOM_MID, -30, 8);
		
		lv_obj_t * yes_label = lv_label_create(yes_btn);
		if(set.Language == EN)
		{
			lv_label_set_text(yes_label, "Yes");
		}else
		{
			lv_label_set_text(yes_label, CN_SHI);
		}
		lv_obj_center(yes_label);
		
		lv_obj_t * no_btn = lv_obj_create(obj_delete);
		lv_obj_add_style(no_btn, &style_YN, 0);
		lv_obj_set_scrollbar_mode(no_btn ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
		lv_obj_align(no_btn, LV_ALIGN_BOTTOM_MID, 30, 8);
		
		lv_obj_t * no_label = lv_label_create(no_btn);
		if(set.Language == EN)
		{
			lv_label_set_text(no_label, "No");
		}else
		{
			lv_label_set_text(no_label, CN_FOU);
		}
		lv_obj_center(no_label);
	}else
	{
		lv_obj_t * yes_btn = lv_obj_create(obj_delete);
		lv_obj_add_style(yes_btn, &style_YN, 0);
		lv_obj_set_scrollbar_mode(yes_btn ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
		lv_obj_align(yes_btn, LV_ALIGN_BOTTOM_MID, -30, 8);
		
		lv_obj_t * yes_label = lv_label_create(yes_btn);
		if(set.Language == EN)
		{
			lv_label_set_text(yes_label, "Yes");
		}else
		{
			lv_label_set_text(yes_label, CN_SHI);
		}
		lv_obj_center(yes_label);
		
		lv_obj_t * no_btn = lv_obj_create(obj_delete);
		lv_obj_add_style(no_btn, &style_YN_pr, 0);
		lv_obj_set_scrollbar_mode(no_btn ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
		lv_obj_align(no_btn, LV_ALIGN_BOTTOM_MID, 30, 8);
		
		lv_obj_t * no_label = lv_label_create(no_btn);
		if(set.Language == EN)
		{
			lv_label_set_text(no_label, "No");
		}else
		{
			lv_label_set_text(no_label, CN_FOU);
		}
		lv_obj_center(no_label);
	}
}


/**********************************************************************************************************
*	函 数 名: void lv_memory_full()
*	功能说明: 内存已满提示
*	形    参: 无
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211031         
**********************************************************************************************************/
void lv_memory_full()
{
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条	
	
	/*A base style*/
    static lv_style_t style_delete;
	lv_style_reset(&style_delete);
    lv_style_init(&style_delete);
    lv_style_set_bg_color(&style_delete, lv_color_white());
    lv_style_set_border_color(&style_delete, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_delete, 2);
    lv_style_set_radius(&style_delete, 10);
    lv_style_set_shadow_width(&style_delete, 10);
    lv_style_set_shadow_ofs_y(&style_delete, 5);
    lv_style_set_shadow_opa(&style_delete, LV_OPA_50);
    lv_style_set_text_color(&style_delete, lv_color_black());
	lv_style_set_text_font(&style_delete, &lv_font_yahei_12_bpp4);
    lv_style_set_width(&style_delete, 140);
    lv_style_set_height(&style_delete, 70);
	
	lv_obj_t * obj_delete = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_delete, &style_delete, 0);
	lv_obj_set_scrollbar_mode(obj_delete ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
    lv_obj_center(obj_delete);
	
	lv_obj_t * delete_label = lv_label_create(obj_delete);

	if(set.Language == EN)
	{
		lv_label_set_text(delete_label, "MEMORY");
		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"\n ");
		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"  FULL !");
	}else
	{
		lv_label_set_text(delete_label, CN_CUN CN_CHU3 CN_YI CN_MAN);
//		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST,"\n ");
		lv_label_ins_text(delete_label,LV_LABEL_POS_LAST," !");
	}
    lv_obj_align(delete_label, LV_ALIGN_CENTER, 0, 0);	
}

/**********************************************************************************************************
*	函 数 名: void lv_disp_charge_init(int Battary_bar)
*	功能说明: 充电页面初始化
*	形    参: int Battary_bar 电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211031         
**********************************************************************************************************/
void lv_disp_charge_init(int Battary_bar)
{
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
	
	static lv_style_t 	Disp_Charging_bar_style;
	lv_style_reset(&Disp_Charging_bar_style);
	lv_style_init(&Disp_Charging_bar_style);
	lv_style_set_radius(&Disp_Charging_bar_style, 0);														//设置圆角0的边框
	lv_style_set_border_opa(&Disp_Charging_bar_style, LV_OPA_0);											//设置边框透明
	lv_style_set_bg_color(&Disp_Charging_bar_style, lv_palette_lighten(LV_PALETTE_GREEN,3));					//设置背景色为LV_COLOR_WHITE
	lv_style_set_bg_grad_color(&Disp_Charging_bar_style, lv_palette_main(LV_PALETTE_GREEN));
	lv_style_set_bg_grad_dir(&Disp_Charging_bar_style, LV_GRAD_DIR_VER);									//从左到右渐变
	lv_style_set_width(&Disp_Charging_bar_style, Battary_bar);														//设置样式宽度
	lv_style_set_height(&Disp_Charging_bar_style, 25);														//设置样式高度
	
	lv_obj_t * Charging_bar = lv_obj_create(lv_scr_act());													//创建目标   lv_scr_act()接口来获取当前活跃的屏幕对象
	lv_obj_add_style(Charging_bar , &Disp_Charging_bar_style, 0);											//加载一种样式
	lv_obj_set_scrollbar_mode(Charging_bar ,LV_SCROLLBAR_MODE_OFF);											//不显示滚动条
	lv_obj_align(Charging_bar , LV_ALIGN_RIGHT_MID, -54, 0);													//设置目标位置
	
	lv_obj_t * charge_img = lv_img_create(lv_scr_act());
	LV_IMG_DECLARE(Charging_Gray_Chroma);
	lv_img_set_src(charge_img, &Charging_Gray_Chroma);
	lv_obj_align(charge_img , LV_ALIGN_RIGHT_MID, -52, 0);													//设置目标位置
}

/**********************************************************************************************************
*	函 数 名: void lv_disp_charge_init(int Battary_bar)
*	功能说明: 充电页面初始化
*	形    参: int Battary_bar 电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211031         
**********************************************************************************************************/
void lv_Error_init(uint8_t error_num)
{
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条	
	
	/*A base style*/
    static lv_style_t style_Error;
	lv_style_reset(&style_Error);
    lv_style_init(&style_Error);
    lv_style_set_bg_color(&style_Error, lv_color_white());
    lv_style_set_border_color(&style_Error, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_Error, 2);
    lv_style_set_radius(&style_Error, 10);
    lv_style_set_shadow_width(&style_Error, 10);
    lv_style_set_shadow_ofs_y(&style_Error, 5);
    lv_style_set_shadow_opa(&style_Error, LV_OPA_50);
    lv_style_set_text_color(&style_Error, lv_color_black());
	lv_style_set_text_font(&style_Error, &lv_font_yahei_12_bpp4);
    lv_style_set_width(&style_Error, 140);
    lv_style_set_height(&style_Error, 70);
	
	lv_obj_t * obj_Error = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_Error, &style_Error, 0);
	lv_obj_set_scrollbar_mode(obj_Error ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条
    lv_obj_center(obj_Error);
	
	lv_obj_t *warning_img = lv_img_create(obj_Error);
	LV_IMG_DECLARE(warning_yellow);
	lv_img_set_src(warning_img, &warning_yellow);
	lv_obj_align(warning_img, LV_ALIGN_LEFT_MID, -5, -15);
	
	lv_obj_t * delete_label = lv_label_create(obj_Error);
	lv_obj_t * delete_label1 = lv_label_create(obj_Error);
	
	switch(error_num)
	{
		case e_NoFault:		{
								break;
							}
		case e_OverCurrent:	{
								if(set.Language==EN)
								{
									lv_label_set_text(delete_label,"Overload");
									lv_label_set_text(delete_label1,"Restart Motor");
								}
								else if(set.Language==CN)
								{
									lv_label_set_text(delete_label,CN_DIAN1 CN_JI1 CN_GUO CN_ZAI2);
									lv_label_set_text(delete_label1,CN_QING CN_CHONG2 CN_QI CN_DIAN1 CN_JI1);
								}
								break;
							}
		case e_LowPower:	{
								if(set.Language==EN)
								{
									lv_label_set_text(delete_label,"Low Power");
									lv_label_set_text(delete_label1,"Please Charge");
								}
								else if(set.Language==CN)
								{
									lv_label_set_text(delete_label,CN_DIAN1 CN_LIANG CN_GUO CN_DI);
									lv_label_set_text(delete_label1,CN_QING CN_CHONG1 CN_DIAN1);
								}
								break;
							}
		default:
								break;
	}
	lv_obj_align(delete_label, LV_ALIGN_LEFT_MID, 35, -15);
	lv_obj_align(delete_label1, LV_ALIGN_BOTTOM_MID, 0, 5);
}

/**********************************************************************************************************
*	函 数 名: void lv_disp_charge_init(int Battary_bar)
*	功能说明: 充电页面初始化
*	形    参: int Battary_bar 电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211031         
**********************************************************************************************************/
void lv_Calibrating_init(void)
{
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条	
	
	lv_obj_t* textFac=lv_label_create(sel_top);
	if(set.Language==EN){
		lv_label_set_text(textFac,"Calibrating...");
	}
	else if(set.Language==CN){
		lv_label_set_text(textFac,CN_ZHENG CN_ZAI1 CN_XIAO CN_ZHUN CN_SHENGLUE);
	}
	lv_obj_align(textFac,LV_ALIGN_CENTER,0,0);				//设置文字位置	
}

/**********************************************************************************************************
*	函 数 名: void lv_disp_charge_init(int Battary_bar)
*	功能说明: 充电页面初始化
*	形    参: int Battary_bar 电量
*	返 回 值: 无
*	编 辑 者: 1：王昌盛
*	修订记录: 1
*	编辑日期: 1: 20211031         
**********************************************************************************************************/
void lv_Finish_init(void)
{
	lv_obj_del(sel_top);		
	sel_top = lv_obj_create(NULL);																							//为显示器创建一个 lv_obj 类型的基础对象
	lv_scr_load(sel_top);
	lv_obj_set_scrollbar_mode(sel_top ,LV_SCROLLBAR_MODE_OFF);																//不显示滚动条	
	
	lv_obj_t* textFac=lv_label_create(sel_top);
	if(set.Language==EN){
		lv_label_set_text(textFac,"Finish");
	}
	else if(set.Language==CN){
		lv_label_set_text(textFac,CN_XIAO CN_ZHUN CN_WAN CN_CHENG1);
	}
	lv_obj_align(textFac,LV_ALIGN_CENTER,0,0);				//设置文字位置	
}












