#include "Menu.h"
#include "key.h"
#include "MenuFunSel.h"
#include "MenuFunSet.h"
#include "MenuFacCal.h"
#include "beep.h"
#include "MenuData.h"
#include "MenuDisp.h"
#include "lcd.h"
#include "_misc.h"
#include "delay.h"
#include "battery.h"
#include "presetting.h"
#include "../lvgl/lvgl.h"

uint8_t	sel_page_flash;																	//sel页面刷新
uint8_t	sel_eeprom_flash;																//与sel页面相关的EEPROM刷新

uint8_t	set_page_flash;																	//set页面刷新
uint8_t	set_eeprom_flash;																//与set页面相关的EEPROM刷新

uint8_t	cal_page_flash;																	//cal页面刷新
uint8_t	cal_eeprom_flash;																//与cal页面相关的EEPROM刷新


const KbdTabStruct KeyTab[]= {
//    |-----------> Index
//    |  ON
//    |  |  Sel
//    |  |  |  Down
//    |  |  |  |    Up longSel  --->功能函数
//    |  |  |  |  |   |   
    { 0, 1, 2, 3, 4,   5,(*DispMenuTop)       },       										// 待机画面
    { 1, 0, 0, 0, 0,   0,(*DispMenuTopON)     },			 								// 启动/停止按键
    { 2, 0, 0, 0, 0,   0,(*DispMenuTopSel)    },		   									// 选择键，进入功能设置
    { 3, 1, 0, 0, 0,   0,(*DispMenuTopDown)   },											// -键，循环递减设置存储号Mx
    { 4, 1, 0, 0, 0,   0,(*DispMenuTopUp)     },		  									// +键，循环递增设置存储号Mx
    { 5, 0, 0, 0, 0,   0,(*DispMenuTopLongSel)},											// 选择键，进入内置锉系统

	// 参数设置
	// “操作模式”菜单
    { 6, 7,8, 9, 10,	 6,(*DispMenuOperationModeInit)	},									// “操作模式”菜单
    { 7, 0, 0, 0, 0,	 0,(*DispMenuOperationModeON)	},									// 保存参数，返回待机界面
    { 8, 0, 0, 0, 0,	 0,(*DispMenuOperationModeSel)	},									// 进入下一个功能设置
    { 9, 0, 0, 0, 0,	 0,(*DispMenuOperationModeDown)	},									// 模式选择，递减
    {10, 0, 0, 0, 0,	 0,(*DispMenuOperationModeUp)	},									// 模式选择，增加
	
	// “速度设置” 菜单
    {11, 7,13,14,15,	11,(*DispMenuSpeedInit)	}, 											// “速度设置” 菜单
    {12, 0, 0, 0, 0,	 0,(*DispMenuSpeedON)	},   										// 保存参数，返回待机界面
    {13, 0, 0, 0, 0,	 0,(*DispMenuSpeedSel)	}, 											// 进入下一个功能设置
    {14, 0, 0, 0, 0,	 0,(*DispMenuSpeedDown)	}, 											// 速度 减小
    {15, 0, 0, 0, 0,	 0,(*DispMenuSpeedUp)	},											// 速度 增加
	
	// “扭矩设置”菜单
    {16, 7,18,19,20,	16,(*DispMenuTorqueInit)}, 											// “扭矩设置”菜单
    {17, 0, 0, 0, 0,	 0,(*DispMenuTorqueON)	},   										// 保存参数，返回待机界面
    {18, 0, 0, 0, 0,	 0,(*DispMenuTorqueSel)	}, 											// 进入下一个功能设置
    {19, 0, 0, 0, 0,	 0,(*DispMenuTorqueDown)}, 											// 扭矩减小
    {20, 0, 0, 0, 0,	 0,(*DispMenuTorqueUp)	},											// 扭矩 增加

	// “角度设置”菜单
    {21, 7,23,24,25,	21,(*DispMenuCWAngleInit)	}, 										// “角度设置”菜单
    {22, 0, 0, 0, 0,	 0,(*DispMenuCWAngleON)		},   									// 保存参数，返回待机界面
    {23, 0, 0, 0, 0,	 0,(*DispMenuCWAngleSel)	}, 										// 进入下一个功能设置
    {24, 0, 0, 0, 0,	 0,(*DispMenuCWAngleDown)	}, 										// 速度 减小
    {25, 0, 0, 0, 0,	 0,(*DispMenuCWAngleUp)		},										// 速度 增加

	// “角度设置”菜单
    {26, 7,28,29,30,	26,(*DispMenuCCWAngleInit)}, 										// “角度设置”菜单
    {27, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleON)	},   									// 保存参数，返回待机界面
    {28, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleSel)	}, 										// 进入下一个功能设置
    {29, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleDown)}, 										// 速度 减小
    {30, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleUp)	},										// 速度 增加

	// "自动启动"设置
    {31, 7,33,34,35,	31,(*DispMenuAutoStartInit)	}, 										// "自动启动"设置
    {32, 0, 0, 0, 0,	 0,(*DispMenuAutoStartON)	},   									// 保存参数，返回待机界面
    {33, 0, 0, 0, 0,	 0,(*DispMenuAutoStartSel)	}, 										// 进入下一个功能设置
    {34, 0, 0, 0, 0,	 0,(*DispMenuAutoStartDown)	}, 										// 模式选择，递减
    {35, 0, 0, 0, 0,	 0,(*DispMenuAutoStartUp)	},										// 模式选择，递增

	// 自动停止
    {36, 7,38,39,40,	36,(*DispMenuAutoStopInit)	}, 										// 自动停止
    {37, 0, 0, 0, 0,	 0,(*DispMenuAutoStopON)	},   									// 保存参数，返回待机界面
    {38, 0, 0, 0, 0,	 0,(*DispMenuAutoStopSel)	}, 										// 进入下一个功能设置
    {39, 0, 0, 0, 0,	 0,(*DispMenuAutoStopDown)	}, 										// 模式选择，递减
    {40, 0, 0, 0, 0,	 0,(*DispMenuAutoStopUp)	},										// 模式选择，递增

	// "根测保护模式"设置
    {41, 7,43,44,45,	41,(*DispMenuApicalModeInit)	}, 									// "根测保护模式"设置
    {42, 0, 0, 0, 0,	 0,(*DispMenuApicalModeON)		},   								// 保存参数，返回待机界面
    {43, 0, 0, 0, 0,	 0,(*DispMenuApicalModeSel)		}, 									// 进入下一个功能设置
    {44, 0, 0, 0, 0,	 0,(*DispMenuApicalModeDown)	}, 									// 模式选择，递减
    {45, 0, 0, 0, 0,	 0,(*DispMenuApicalModeUp)		},									// 模式选择，递增

	// "根测基准点"设置
    {46, 7, 7,49,50,  	46,(*DispMenuFlashBarPositionInit)	}, 								// "根测基准点"设置
    {47, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionON)	},   							// 保存参数，返回待机界面
    {48, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionSel)	}, 								// 进入下一个功能设置
    {49, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionDown)	}, 								// 模式选择，递减
    {50, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionUp)	},								// 模式选择，递增


    /************************************************************************/
	//							功能设置
    // 功能设置“版本显示”
    { 51, 52, 53, 54, 55, 51,(*DispMenuVersionsInit)	}, 									// 功能设置“版本显示”
    { 52,  0,  0,  0,  0,  0,(*DispMenuVersionsON)		},   								// 保存参数，返回待机界面
    { 53,  0,  0,  0,  0,  0,(*DispMenuVersionsSel)		}, 									// 进入下一个功能设置
    { 54,  0,  0,  0,  0,  0,(*DispMenuVersionsDown)	}, 									// 模式选择，递减
    { 55,  0,  0,  0,  0,  0,(*DispMenuVersionsUp)		},									// 模式选择，递增
    
	// 功能设置“自动关机功能设置”
    { 56, 52, 58, 59, 60, 56,(*DispMenuAutoPowerOffInit)}, 									// 功能设置“自动关机功能设置”
    { 57,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffON)	},   								// 保存参数，返回待机界面
    { 58,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffSel)	}, 									// 进入下一个功能设置
    { 59,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffDown)}, 									// 模式选择，递减
    { 60,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffUp)	},									// 模式选择，递增
	
	//功能设置“自动待机时间设置“
    { 61, 52, 63, 64, 65, 61,(*DispMenuAutoStandyInit)	}, 									// 功能设置“自动待机时间设置“
    { 62,  0,  0,  0,  0,  0,(*DispMenuAutoStandyON)	},   								// 保存参数，返回待机界面
    { 63,  0,  0,  0,  0,  0,(*DispMenuAutoStandySel)	}, 									// 进入下一个功能设置
    { 64,  0,  0,  0,  0,  0,(*DispMenuAutoStandyDown)	}, 									// 模式选择，递减
    { 65,  0,  0,  0,  0,  0,(*DispMenuAutoStandyUp)	},									// 模式选择，递增
	
	// 功能设置“音量设置“
    { 66, 52, 68, 69, 70, 66,(*DispMenuBeepVolInit)	}, 										// 功能设置“音量设置“
    { 67,  0,  0,  0,  0,  0,(*DispMenuBeepVolON)	},   									// 保存参数，返回待机界面
    { 68,  0,  0,  0,  0,  0,(*DispMenuBeepVolSel)	}, 										// 进入下一个功能设置
    { 69,  0,  0,  0,  0,  0,(*DispMenuBeepVolDown)	}, 										// 模式选择，递减
    { 70,  0,  0,  0,  0,  0,(*DispMenuBeepVolUp)	},										// 模式选择，递增
	
	// 功能设置“背光设置”
    { 71, 52, 73, 74, 75, 71,(*DispMenuBlInit)	}, 											// 功能设置“背光设置”
    { 72,  0,  0,  0,  0,  0,(*DispMenuBlON)	},   										// 保存参数，返回待机界面
    { 73,  0,  0,  0,  0,  0,(*DispMenuBlSel)	}, 											// 进入下一个功能设置
    { 74,  0,  0,  0,  0,  0,(*DispMenuBlDown)	}, 											// 模式选择，递减
    { 75,  0,  0,  0,  0,  0,(*DispMenuBlUp)	},											// 模式选择，递增
	
	// 功能设置“左右手习惯设置”
    { 76, 52, 78, 79, 80, 76,(*DispMenuHandInit)}, 											// 功能设置“左右手习惯设置”
    { 77,  0,  0,  0,  0,  0,(*DispMenuHandON)	},   										// 保存参数，返回待机界面
    { 78,  0,  0,  0,  0,  0,(*DispMenuHandSel)	}, 											// 进入下一个功能设置
    { 79,  0,  0,  0,  0,  0,(*DispMenuHandDown)}, 											// 模式选择，递减
    { 80,  0,  0,  0,  0,  0,(*DispMenuHandUp)	},											// 模式选择，递增
	
	// 功能设置“根测灵敏度设置”
    { 81, 52, 83, 84, 85, 81,(*DispMenuApexsensitivityInit)	}, 								// 功能设置“根测灵敏度设置”
    { 82,  0,  0,  0,  0,  0,(*DispMenuApexsensitivityON)	},   							// 保存参数，返回待机界面
    { 83,  0,  0,  0,  0,  0,(*DispMenuApexsensitivitySel)	}, 								// 进入下一个功能设置
    { 84,  0,  0,  0,  0,  0,(*DispMenuApexsensitivityDown)	}, 								// 模式选择，递减
    { 85,  0,  0,  0,  0,  0,(*DispMenuApexsensitivityUp)	},								// 模式选择，递增
	
	// 功能设置“语言设置“
    { 86, 52, 88, 89, 90, 86,(*DispMenuLanguageInit)	}, 									//功能设置“语言设置“
    { 87,  0,  0,  0,  0,  0,(*DispMenuLanguageON)		},   								//保存参数，返回待机界面
    { 88,  0,  0,  0,  0,  0,(*DispMenuLanguageSel)		}, 									//进入下一个功能设置
    { 89,  0,  0,  0,  0,  0,(*DispMenuLanguageDown)	}, 									// 模式选择，递减
    { 90,  0,  0,  0,  0,  0,(*DispMenuLanguageUp)		},									//模式选择，递增
	
	// 功能设置“自动校准“
    { 91, 52, 93, 94, 95, 91,(*DispMenuCalibrationInit)	}, 									// 功能设置“自动校准“
    { 92,  0,  0,  0,  0,  0,(*DispMenuCalibrationON)	},   								// 保存参数，返回待机界面
    { 93,  0,  0,  0,  0,  0,(*DispMenuCalibrationSel)	}, 									// 进入下一个功能设置
    { 94,  0,  0,  0,  0,  0,(*DispMenuCalibrationDown)	}, 									// 模式选择，递减
    { 95,  0,  0,  0,  0,  0,(*DispMenuCalibrationUp)	},									// 模式选择，递增

	// 功能设置“恢复出厂功能设置”
    { 96, 52, 98, 99,100, 96,(*DispMenuRestoreFactorySettingsInit)	}, 						// 功能设置“恢复出厂功能设置”
    { 97,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsON)	},   					// 保存参数，返回待机界面
    { 98,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsSel)	}, 						// 进入下一个功能设置
    { 99,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsDown)	}, 						// 模式选择，递减
    {100,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsUp)	},						// 模式选择，递增
	
	// 功能设置“未定义”
    {101, 52,103,104,105,101,(*DispMenuUndefineInit)	}, 									// 功能设置“未定义”
    {102,  0,  0,  0,  0,  0,(*DispMenuUndefineON)		},   								// 保存参数，返回待机界面
    {103,  0,  0,  0,  0,  0,(*DispMenuUndefineSel)		}, 									// 进入下一个功能设置
    {104,  0,  0,  0,  0,  0,(*DispMenuUndefineDown)	}, 									// 模式选择，递减
    {105,  0,  0,  0,  0,  0,(*DispMenuUndefineUp)		},									// 模式选择，递增


	// 内置锉系统
    {106,107,108,109,110,111,(*DispMenuBuiltInFileInit)		}, 								// 内置锉系统
    {107,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileON)		},   							// 保存参数，返回待机界面
    {108,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileSel)		}, 								// 保存参数，返回待机界面
    {109,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileDown)		}, 								// 参数选择，递减
    {110,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileUp)		},								// 参数选择，递增
	{111,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileLongset)	},								// 参数选择，递增

	// 内置锉改名字
    {112,113,114,115,116,117,(*DispMenuRenameFileInit)		}, 								// 内置锉系统
    {113,  0,  0,  0,  0,  0,(*DispMenuRenameFileON)		},   							// 保存参数，返回待机界面
    {114,  0,  0,  0,  0,  0,(*DispMenuRenameFileSel)		}, 								// 保存参数，返回待机界面
    {115,  0,  0,  0,  0,  0,(*DispMenuRenameFileDown)		}, 								// 参数选择，递减
    {116,  0,  0,  0,  0,  0,(*DispMenuRenameFileUp)		},								// 参数选择，递增
    {117,  0,  0,  0,  0,  0,(*DispMenuRenameFileLongSel)	},								// 长按

	// 选锉序列
    {118,119,120,121,122,123,(*DispMenuFilelistInit)	}, 									// 锉序列选择
    {119,  0,  0,  0,  0,  0,(*DispMenuFilelistON)		},   								// 保存参数，返回待机界面
    {120,  0,  0,  0,  0,  0,(*DispMenuFilelistSel)		}, 									// 保存参数，返回待机界面
    {121,  0,  0,  0,  0,  0,(*DispMenuFilelistDown)	}, 									// 参数选择，递减
    {122,  0,  0,  0,  0,  0,(*DispMenuFilelistUp)		},									// 参数选择，递增
	{123,  0,  0,  0,  0,123,(*DispMenuFilelistLongSel)	}, 									// 长按
						 
	// 出厂校准	
	{124,125,126,127,128,124,(*DispMenuFactoryCalInit)	}, 									// 出厂校准
    {125,  0,  0,  0,  0,  0,(*DispMenuFactoryCalON)	},   								// 保存参数，返回待机界面
    {126,  0,  0,  0,  0,  0,(*DispMenuFactoryCalSel)	}, 									// 保存参数，返回待机界面
    {127,  0,  0,  0,  0,  0,(*DispMenuFactoryCalDown)	}, 									// 参数选择，递减
    {128,  0,  0,  0,  0,  0,(*DispMenuFactoryCalUp)	},									// 参数选择，递增

	
	
	{129, 130,131,132,133,184,(*DispFileKeytextInit)	}, 									// 键盘模式设置
    {130,  0,  0,  0,  0,  0,(*DispFileKeytextON)		},   								// 保存参数，返回待机界面
    {131,  0,  0,  0,  0,  0,(*DispFileKeytextSel)		}, 									// 进入下一个功能设置
    {132,  0,  0,  0,  0,  0,(*DispFileKeytextDown)		}, 									// 模式选择，递减
    {133,  0,  0,  0,  0,  0,(*DispFileKeytextUp)		},									// 模式选择，递增
	
	{134, 135,136,137,138,134,(*DispFileSeqnumberInit)	}, 									// 锉个数设置
    {135,  0,  0,  0,  0,  0,(*DispFileSeqnumberON)		},   								// 保存参数，返回待机界面
    {136,  0,  0,  0,  0,  0,(*DispFileSeqnumberSel)	}, 									// 进入下一个功能设置
    {137,  0,  0,  0,  0,  0,(*DispFileSeqnumberDown)	}, 									// 模式选择，递减
    {138,  0,  0,  0,  0,  0,(*DispFileSeqnumberUp)		},									// 模式选择，递增
	
	{139, 135,141,142,143,139,(*DispFileTaperInit)		}, 									// 锥度
    {140,  0,  0,  0,  0,  0,(*DispFileTaperON)			},   								// 保存参数，返回待机界面
    {141,  0,  0,  0,  0,  0,(*DispFileTaperSel)		}, 									// 进入下一个功能设置
    {142,  0,  0,  0,  0,  0,(*DispFileTaperDown)		}, 									// 模式选择，递减
    {143,  0,  0,  0,  0,  0,(*DispFileTaperUp)			},									// 模式选择，递增
	
	{144, 135,146,147,148,144,(*DispFileTapernumberInit)}, 									// 锉号
    {145,  0,  0,  0,  0,  0,(*DispFileTapernumberON)	},   								// 保存参数，返回待机界面
    {146,  0,  0,  0,  0,  0,(*DispFileTapernumberSel)	}, 									// 进入下一个功能设置
    {147,  0,  0,  0,  0,  0,(*DispFileTapernumberDown)	}, 									// 模式选择，递减
    {148,  0,  0,  0,  0,  0,(*DispFileTapernumberUp)	},									// 模式选择，递增
	
	{149, 135,151,152,153,149,(*DispFileOperationInit)	}, 									// 操作模式
    {150,  0,  0,  0,  0,  0,(*DispFileOperationON)		},   								// 保存参数，返回待机界面
    {151,  0,  0,  0,  0,  0,(*DispFileOperationSel)	}, 									// 进入下一个功能设置
    {152,  0,  0,  0,  0,  0,(*DispFileOperationDown)	}, 									// 模式选择，递减
    {153,  0,  0,  0,  0,  0,(*DispFileOperationUp)		},									// 模式选择，递增
	
	{154, 135,156,157,158,154,(*DispFileSpeedInit)		}, 									// 速度
    {155,  0,  0,  0,  0,  0,(*DispFileSpeedON)			},   								// 保存参数，返回待机界面
    {156,  0,  0,  0,  0,  0,(*DispFileSpeedSel)		}, 									// 进入下一个功能设置
    {157,  0,  0,  0,  0,  0,(*DispFileSpeedDown)		}, 									// 模式选择，递减
    {158,  0,  0,  0,  0,  0,(*DispFileSpeedUp)			},									// 模式选择，递增
	
	{159, 135,161,162,163,159,(*DispFileTorqueInit)		}, 									// 转矩
    {160,  0,  0,  0,  0,  0,(*DispFileTorqueON)		},   								// 保存参数，返回待机界面
    {161,  0,  0,  0,  0,  0,(*DispFileTorqueSel)		}, 									// 进入下一个功能设置
    {162,  0,  0,  0,  0,  0,(*DispFileTorqueDown)		}, 									// 模式选择，递减
    {163,  0,  0,  0,  0,  0,(*DispFileTorqueUp)		},									// 模式选择，递增
	
	{164, 135,166,167,168,164,(*DispFileCWAngleInit)	}, 									// 正角度
    {165,  0,  0,  0,  0,  0,(*DispFileCWAngleON)		},   								// 保存参数，返回待机界面
    {166,  0,  0,  0,  0,  0,(*DispFileCWAngleSel)		}, 									// 进入下一个功能设置
    {167,  0,  0,  0,  0,  0,(*DispFileCWAngleDown)		}, 									// 模式选择，递减
    {168,  0,  0,  0,  0,  0,(*DispFileCWAngleUp)		},									// 模式选择，递增
	
	{169, 135,171,172,173,169,(*DispFileCCWAngleInit)	}, 									// 负角度
    {170,  0,  0,  0,  0,  0,(*DispFileCCWAngleON)		},   								// 保存参数，返回待机界面
    {171,  0,  0,  0,  0,  0,(*DispFileCCWAngleSel)		}, 									// 进入下一个功能设置
    {172,  0,  0,  0,  0,  0,(*DispFileCCWAngleDown)	}, 									// 模式选择，递减
    {173,  0,  0,  0,  0,  0,(*DispFileCCWAngleUp)		},									// 模式选择，递增
	
	{174, 135,176,177,178,174,(*DispFileColorInit)		}, 									// 颜色
    {175,  0,  0,  0,  0,  0,(*DispFileColorON)			},   								// 保存参数，返回待机界面
    {176,  0,  0,  0,  0,  0,(*DispFileColorSel)		}, 									// 进入下一个功能设置
    {177,  0,  0,  0,  0,  0,(*DispFileColorDown)		}, 									// 模式选择，递减
    {178,  0,  0,  0,  0,  0,(*DispFileColorUp)			},									// 模式选择，递增
	
	{179, 180,181,182,183,179,(*DispFileDeleteInit)		}, 									// 删除提示
    {180,  0,  0,  0,  0,  0,(*DispFileDeleteON)		},   								// 保存参数，返回待机界面
    {181,  0,  0,  0,  0,  0,(*DispFileDeleteSel)		}, 									// 确认删除
    {182,  0,  0,  0,  0,  0,(*DispFileDeleteDown)		}, 									// 模式选择，递减
    {183,  0,  0,  0,  0,  0,(*DispFileDeleteUp)		},									// 模式选择，递增
	
	{184,  0,  0,  0,  0,  0,(*DispFileKeytextLongSel)	},									// 模式选择，递增
};


//超时等待函数，超过一定时间不操作，退回待机界面
int WaitTimeOut(void)
{
	if(TimeOutCnt > d_StandyTime[set.AutoStandyTime]*10)
	{
		TimeOutCnt=0;
		return -1;
	}
	else return 0;
}


