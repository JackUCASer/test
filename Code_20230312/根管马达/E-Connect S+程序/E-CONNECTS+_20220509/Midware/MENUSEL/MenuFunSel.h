#ifndef __MENUFUNSEL_H
#define __MENUFUNSEL_H

#include "datatype.h"

//MenuTop.C
void DispMenuTop(void);
void DispMenuTopUp(void);
void DispMenuTopDown(void);
void DispMenuTopSel(void);
void DispMenuTopON(void);
void DispMenuTopLongSel(void);
void DispMenuOperationModeInit(void);
void DispMenuOperationModeON(void);
void DispMenuOperationModeSel(void);
void DispMenuOperationModeUp(void);
void DispMenuOperationModeDown(void);

//MenuFunc.C
void DispMenuSpeedInit(void);
void DispMenuSpeedON(void);
void DispMenuSpeedSel(void);
void DispMenuSpeedUp(void);
void DispMenuSpeedDown(void);
//MenuPara.C
void DispMenuTorqueInit(void);
void DispMenuTorqueON(void);
void DispMenuTorqueSel(void);
void DispMenuTorqueUp(void);
void DispMenuTorqueDown(void);
//MenuLanguage.C
void DispMenuCWAngleInit(void);
void DispMenuCWAngleON(void);
void DispMenuCWAngleSel(void);
void DispMenuCWAngleUp(void);
void DispMenuCWAngleDown(void);

void DispMenuCCWAngleInit(void);
void DispMenuCCWAngleON(void);
void DispMenuCCWAngleSel(void);
void DispMenuCCWAngleUp(void);
void DispMenuCCWAngleDown(void);
//MenuMeasure.C
void DispMenuApicalModeInit(void);
void DispMenuApicalModeON(void);
void DispMenuApicalModeSel(void);
void DispMenuApicalModeUp(void);
void DispMenuApicalModeDown(void);
void DispMenuAutoPowerOffInit(void);
void DispMenuAutoPowerOffON(void);
void DispMenuAutoStartInit(void);
void DispMenuAutoStartON(void);
void DispMenuAutoStartSel(void);
void DispMenuAutoStartUp(void);
void DispMenuAutoStartDown(void);
void DispMenuAutoStopInit(void);

void DispMenuAutoStopON(void);
void DispMenuAutoStopSel(void);
void DispMenuAutoStopUp(void);

void DispMenuAutoStopDown(void);
void DispMenuFlashBarPositionInit(void);
void DispMenuFlashBarPositionON(void);
void DispMenuFlashBarPositionSel(void);
void DispMenuFlashBarPositionUp(void);
void DispMenuFlashBarPositionDown(void);

void DispMenuNoDefineInit(void);

void DispMenuNoDefineON(void);

void DispMenuNoDefineSel(void);

void DispMenuNoDefineUp(void);

void DispMenuNoDefineDown(void);


void	DispFileKeytextInit();									// 键盘模式设置
void	DispFileKeytextON();   									// 保存参数，返回待机界面
void	DispFileKeytextSel(); 									// 进入下一个功能设置
void	DispFileKeytextDown(); 									// 模式选择，递减
void	DispFileKeytextUp();									// 模式选择，递增
void	DispFileKeytextLongSel();								// 长按保存

void	DispFileSeqnumberInit(); 								// 锉个数设置
void	DispFileSeqnumberON();   								// 保存参数，返回待机界面
void	DispFileSeqnumberSel(); 								// 进入下一个功能设置
void	DispFileSeqnumberDown(); 								// 模式选择，递减
void	DispFileSeqnumberUp();									// 模式选择，递增
	
void	DispFileTaperInit(); 									// 锥度
void	DispFileTaperON();   									// 保存参数，返回待机界面
void	DispFileTaperSel();										// 进入下一个功能设置
void	DispFileTaperDown(); 									// 模式选择，递减
void	DispFileTaperUp();										// 模式选择，递增
	
void	DispFileTapernumberInit(); 								// 锉号
void	DispFileTapernumberON();   								// 保存参数，返回待机界面
void	DispFileTapernumberSel(); 								// 进入下一个功能设置
void	DispFileTapernumberDown();								// 模式选择，递减
void	DispFileTapernumberUp();								// 模式选择，递增
	
void	DispFileOperationInit(); 								// 操作模式
void	DispFileOperationON();   								// 保存参数，返回待机界面
void	DispFileOperationSel(); 								// 进入下一个功能设置
void	DispFileOperationDown(); 								// 模式选择，递减
void	DispFileOperationUp();									// 模式选择，递增
	
void	DispFileSpeedInit(); 									// 速度
void	DispFileSpeedON();   									// 保存参数，返回待机界面
void	DispFileSpeedSel(); 									// 进入下一个功能设置
void	DispFileSpeedDown(); 									// 模式选择，递减
void	DispFileSpeedUp();										// 模式选择，递增
	
void	DispFileTorqueInit();									// 转矩
void	DispFileTorqueON();   									// 保存参数，返回待机界面
void	DispFileTorqueSel(); 									// 进入下一个功能设置
void	DispFileTorqueDown(); 									// 模式选择，递减
void	DispFileTorqueUp();										// 模式选择，递增
	
void	DispFileCWAngleInit(); 									// 正角度
void	DispFileCWAngleON();   									// 保存参数，返回待机界面
void	DispFileCWAngleSel(); 									// 进入下一个功能设置
void	DispFileCWAngleDown(); 									// 模式选择，递减
void	DispFileCWAngleUp();									// 模式选择，递增
	
void	DispFileCCWAngleInit(); 								// 负角度
void	DispFileCCWAngleON();   								// 保存参数，返回待机界面
void	DispFileCCWAngleSel(); 									// 进入下一个功能设置
void	DispFileCCWAngleDown(); 								// 模式选择，递减
void	DispFileCCWAngleUp();									// 模式选择，递增
	
void	DispFileColorInit(); 									// 颜色
void	DispFileColorON();   									// 保存参数，返回待机界面
void	DispFileColorSel();										// 进入下一个功能设置
void	DispFileColorDown(); 									// 模式选择，递减
void	DispFileColorUp();										// 模式选择，递增






#endif