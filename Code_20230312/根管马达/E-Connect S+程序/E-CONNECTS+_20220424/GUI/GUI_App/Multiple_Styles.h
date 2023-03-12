
////////////////////////////////////////////////////////////////////
/// @file Multiple_Styles.h
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

#ifndef __MULTIPLE_STYLES_H
#define __MULTIPLE_STYLES_H
#include "lvgl/lvgl.h"
#include "MenuData.h"
#include "MenuDisp.h"

#define EN 0
#define CN 1

//void lv_Multiple_style(void);

void DispLogo(void);
void DispSetting(void);
void DispFactory(void);

void Disp_cal(void);

void DispBatery_init(uint8_t Sw, int Battery, int Charging);
void DispBatery_charge(uint8_t Sw, int Battery, int Charging);

void DispVol(uint8_t Sw, int Volume);

/************************与待机界面相关****************************************/
void Standy_init(int memstr, FUN_SEL sel);					//待机界面显示
void Apex_init(FUN_SEL sel);								//根测界面，与根测设置相关
void Apex_init1(FUN_SEL sel);
void DispMeasure(void);
void Apex_RUN(int Apex_cs);
/************************与待机设置界面相关****************************************/
void Standy_sel_init(FUN_SEL sel);							//待机界面设置参数
void Standy_ON(int TL_Motor,uint8_t TL_limit_flag);
void Disp_torque_bar_scale(int TL_Motor,uint8_t TL_limit_flag);
void Disp_torque_bar_scale1(int TL_Motor, int TL_Motor_Last, uint8_t TL_limit_flag);
/************************与设置模式相关****************************************/
void SYS_set_init(FUN_SET set);								//系统设置参数

void Filelist_init(Seq_parameter_inside *fname_inside, int Disp_str, int name_str, int Outside_name_add);
void Seq_list_init(Seq_parameter_inside *fSeq_Inside, int name_str, int Seq_Disp_str, int Seq_parameter_str);
void Seq_Outsidelist_init(Seq_parameter_outside fNew_file_parameter, int name_str, int Seq_Disp_str, int Seq_parameter_str);

void New_File_init(int add_file_number, Seq_parameter_outside fNew_file_parameter, int Seq_const_len ,int Seq_str);

void New_File_abckeyname(char* Rename, uint8_t ShiftBig, uint8_t key_str, uint8_t key_Pr);

static void ta_event_cb(lv_event_t * e);
static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa);
void lv_delete_style(uint8_t yes_on);						//删除提示
void lv_memory_full();										//内存已满
void lv_disp_charge_init(int Battary_bar);					//显示电量
void lv_Error_init(uint8_t error_num);						//错误提示
void lv_Finish_init(void);									//校准完成
void lv_Calibrating_init(void);								//正在校准
	
#endif







