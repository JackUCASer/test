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

uint8_t	sel_page_flash;																	//selҳ��ˢ��
uint8_t	sel_eeprom_flash;																//��selҳ����ص�EEPROMˢ��

uint8_t	set_page_flash;																	//setҳ��ˢ��
uint8_t	set_eeprom_flash;																//��setҳ����ص�EEPROMˢ��

uint8_t	cal_page_flash;																	//calҳ��ˢ��
uint8_t	cal_eeprom_flash;																//��calҳ����ص�EEPROMˢ��


const KbdTabStruct KeyTab[]= {
//    |-----------> Index
//    |  ON
//    |  |  Sel
//    |  |  |  Down
//    |  |  |  |    Up longSel  --->���ܺ���
//    |  |  |  |  |   |   
    { 0, 1, 2, 3, 4,   5,(*DispMenuTop)       },       										// ��������
    { 1, 0, 0, 0, 0,   0,(*DispMenuTopON)     },			 								// ����/ֹͣ����
    { 2, 0, 0, 0, 0,   0,(*DispMenuTopSel)    },		   									// ѡ��������빦������
    { 3, 1, 0, 0, 0,   0,(*DispMenuTopDown)   },											// -����ѭ���ݼ����ô洢��Mx
    { 4, 1, 0, 0, 0,   0,(*DispMenuTopUp)     },		  									// +����ѭ���������ô洢��Mx
    { 5, 0, 0, 0, 0,   0,(*DispMenuTopLongSel)},											// ѡ��������������ϵͳ

	// ��������
	// ������ģʽ���˵�
    { 6, 7,8, 9, 10,	 6,(*DispMenuOperationModeInit)	},									// ������ģʽ���˵�
    { 7, 0, 0, 0, 0,	 0,(*DispMenuOperationModeON)	},									// ������������ش�������
    { 8, 0, 0, 0, 0,	 0,(*DispMenuOperationModeSel)	},									// ������һ����������
    { 9, 0, 0, 0, 0,	 0,(*DispMenuOperationModeDown)	},									// ģʽѡ�񣬵ݼ�
    {10, 0, 0, 0, 0,	 0,(*DispMenuOperationModeUp)	},									// ģʽѡ������
	
	// ���ٶ����á� �˵�
    {11, 7,13,14,15,	11,(*DispMenuSpeedInit)	}, 											// ���ٶ����á� �˵�
    {12, 0, 0, 0, 0,	 0,(*DispMenuSpeedON)	},   										// ������������ش�������
    {13, 0, 0, 0, 0,	 0,(*DispMenuSpeedSel)	}, 											// ������һ����������
    {14, 0, 0, 0, 0,	 0,(*DispMenuSpeedDown)	}, 											// �ٶ� ��С
    {15, 0, 0, 0, 0,	 0,(*DispMenuSpeedUp)	},											// �ٶ� ����
	
	// ��Ť�����á��˵�
    {16, 7,18,19,20,	16,(*DispMenuTorqueInit)}, 											// ��Ť�����á��˵�
    {17, 0, 0, 0, 0,	 0,(*DispMenuTorqueON)	},   										// ������������ش�������
    {18, 0, 0, 0, 0,	 0,(*DispMenuTorqueSel)	}, 											// ������һ����������
    {19, 0, 0, 0, 0,	 0,(*DispMenuTorqueDown)}, 											// Ť�ؼ�С
    {20, 0, 0, 0, 0,	 0,(*DispMenuTorqueUp)	},											// Ť�� ����

	// ���Ƕ����á��˵�
    {21, 7,23,24,25,	21,(*DispMenuCWAngleInit)	}, 										// ���Ƕ����á��˵�
    {22, 0, 0, 0, 0,	 0,(*DispMenuCWAngleON)		},   									// ������������ش�������
    {23, 0, 0, 0, 0,	 0,(*DispMenuCWAngleSel)	}, 										// ������һ����������
    {24, 0, 0, 0, 0,	 0,(*DispMenuCWAngleDown)	}, 										// �ٶ� ��С
    {25, 0, 0, 0, 0,	 0,(*DispMenuCWAngleUp)		},										// �ٶ� ����

	// ���Ƕ����á��˵�
    {26, 7,28,29,30,	26,(*DispMenuCCWAngleInit)}, 										// ���Ƕ����á��˵�
    {27, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleON)	},   									// ������������ش�������
    {28, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleSel)	}, 										// ������һ����������
    {29, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleDown)}, 										// �ٶ� ��С
    {30, 0, 0, 0, 0,	 0,(*DispMenuCCWAngleUp)	},										// �ٶ� ����

	// "�Զ�����"����
    {31, 7,33,34,35,	31,(*DispMenuAutoStartInit)	}, 										// "�Զ�����"����
    {32, 0, 0, 0, 0,	 0,(*DispMenuAutoStartON)	},   									// ������������ش�������
    {33, 0, 0, 0, 0,	 0,(*DispMenuAutoStartSel)	}, 										// ������һ����������
    {34, 0, 0, 0, 0,	 0,(*DispMenuAutoStartDown)	}, 										// ģʽѡ�񣬵ݼ�
    {35, 0, 0, 0, 0,	 0,(*DispMenuAutoStartUp)	},										// ģʽѡ�񣬵���

	// �Զ�ֹͣ
    {36, 7,38,39,40,	36,(*DispMenuAutoStopInit)	}, 										// �Զ�ֹͣ
    {37, 0, 0, 0, 0,	 0,(*DispMenuAutoStopON)	},   									// ������������ش�������
    {38, 0, 0, 0, 0,	 0,(*DispMenuAutoStopSel)	}, 										// ������һ����������
    {39, 0, 0, 0, 0,	 0,(*DispMenuAutoStopDown)	}, 										// ģʽѡ�񣬵ݼ�
    {40, 0, 0, 0, 0,	 0,(*DispMenuAutoStopUp)	},										// ģʽѡ�񣬵���

	// "���Ᵽ��ģʽ"����
    {41, 7,43,44,45,	41,(*DispMenuApicalModeInit)	}, 									// "���Ᵽ��ģʽ"����
    {42, 0, 0, 0, 0,	 0,(*DispMenuApicalModeON)		},   								// ������������ش�������
    {43, 0, 0, 0, 0,	 0,(*DispMenuApicalModeSel)		}, 									// ������һ����������
    {44, 0, 0, 0, 0,	 0,(*DispMenuApicalModeDown)	}, 									// ģʽѡ�񣬵ݼ�
    {45, 0, 0, 0, 0,	 0,(*DispMenuApicalModeUp)		},									// ģʽѡ�񣬵���

	// "�����׼��"����
    {46, 7, 7,49,50,  	46,(*DispMenuFlashBarPositionInit)	}, 								// "�����׼��"����
    {47, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionON)	},   							// ������������ش�������
    {48, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionSel)	}, 								// ������һ����������
    {49, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionDown)	}, 								// ģʽѡ�񣬵ݼ�
    {50, 0, 0, 0, 0,	 0,(*DispMenuFlashBarPositionUp)	},								// ģʽѡ�񣬵���


    /************************************************************************/
	//							��������
    // �������á��汾��ʾ��
    { 51, 52, 53, 54, 55, 51,(*DispMenuVersionsInit)	}, 									// �������á��汾��ʾ��
    { 52,  0,  0,  0,  0,  0,(*DispMenuVersionsON)		},   								// ������������ش�������
    { 53,  0,  0,  0,  0,  0,(*DispMenuVersionsSel)		}, 									// ������һ����������
    { 54,  0,  0,  0,  0,  0,(*DispMenuVersionsDown)	}, 									// ģʽѡ�񣬵ݼ�
    { 55,  0,  0,  0,  0,  0,(*DispMenuVersionsUp)		},									// ģʽѡ�񣬵���
    
	// �������á��Զ��ػ��������á�
    { 56, 52, 58, 59, 60, 56,(*DispMenuAutoPowerOffInit)}, 									// �������á��Զ��ػ��������á�
    { 57,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffON)	},   								// ������������ش�������
    { 58,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffSel)	}, 									// ������һ����������
    { 59,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffDown)}, 									// ģʽѡ�񣬵ݼ�
    { 60,  0,  0,  0,  0,  0,(*DispMenuAutoPowerOffUp)	},									// ģʽѡ�񣬵���
	
	//�������á��Զ�����ʱ�����á�
    { 61, 52, 63, 64, 65, 61,(*DispMenuAutoStandyInit)	}, 									// �������á��Զ�����ʱ�����á�
    { 62,  0,  0,  0,  0,  0,(*DispMenuAutoStandyON)	},   								// ������������ش�������
    { 63,  0,  0,  0,  0,  0,(*DispMenuAutoStandySel)	}, 									// ������һ����������
    { 64,  0,  0,  0,  0,  0,(*DispMenuAutoStandyDown)	}, 									// ģʽѡ�񣬵ݼ�
    { 65,  0,  0,  0,  0,  0,(*DispMenuAutoStandyUp)	},									// ģʽѡ�񣬵���
	
	// �������á��������á�
    { 66, 52, 68, 69, 70, 66,(*DispMenuBeepVolInit)	}, 										// �������á��������á�
    { 67,  0,  0,  0,  0,  0,(*DispMenuBeepVolON)	},   									// ������������ش�������
    { 68,  0,  0,  0,  0,  0,(*DispMenuBeepVolSel)	}, 										// ������һ����������
    { 69,  0,  0,  0,  0,  0,(*DispMenuBeepVolDown)	}, 										// ģʽѡ�񣬵ݼ�
    { 70,  0,  0,  0,  0,  0,(*DispMenuBeepVolUp)	},										// ģʽѡ�񣬵���
	
	// �������á��������á�
    { 71, 52, 73, 74, 75, 71,(*DispMenuBlInit)	}, 											// �������á��������á�
    { 72,  0,  0,  0,  0,  0,(*DispMenuBlON)	},   										// ������������ش�������
    { 73,  0,  0,  0,  0,  0,(*DispMenuBlSel)	}, 											// ������һ����������
    { 74,  0,  0,  0,  0,  0,(*DispMenuBlDown)	}, 											// ģʽѡ�񣬵ݼ�
    { 75,  0,  0,  0,  0,  0,(*DispMenuBlUp)	},											// ģʽѡ�񣬵���
	
	// �������á�������ϰ�����á�
    { 76, 52, 78, 79, 80, 76,(*DispMenuHandInit)}, 											// �������á�������ϰ�����á�
    { 77,  0,  0,  0,  0,  0,(*DispMenuHandON)	},   										// ������������ش�������
    { 78,  0,  0,  0,  0,  0,(*DispMenuHandSel)	}, 											// ������һ����������
    { 79,  0,  0,  0,  0,  0,(*DispMenuHandDown)}, 											// ģʽѡ�񣬵ݼ�
    { 80,  0,  0,  0,  0,  0,(*DispMenuHandUp)	},											// ģʽѡ�񣬵���
	
	// �������á��������������á�
    { 81, 52, 83, 84, 85, 81,(*DispMenuApexsensitivityInit)	}, 								// �������á��������������á�
    { 82,  0,  0,  0,  0,  0,(*DispMenuApexsensitivityON)	},   							// ������������ش�������
    { 83,  0,  0,  0,  0,  0,(*DispMenuApexsensitivitySel)	}, 								// ������һ����������
    { 84,  0,  0,  0,  0,  0,(*DispMenuApexsensitivityDown)	}, 								// ģʽѡ�񣬵ݼ�
    { 85,  0,  0,  0,  0,  0,(*DispMenuApexsensitivityUp)	},								// ģʽѡ�񣬵���
	
	// �������á��������á�
    { 86, 52, 88, 89, 90, 86,(*DispMenuLanguageInit)	}, 									//�������á��������á�
    { 87,  0,  0,  0,  0,  0,(*DispMenuLanguageON)		},   								//������������ش�������
    { 88,  0,  0,  0,  0,  0,(*DispMenuLanguageSel)		}, 									//������һ����������
    { 89,  0,  0,  0,  0,  0,(*DispMenuLanguageDown)	}, 									// ģʽѡ�񣬵ݼ�
    { 90,  0,  0,  0,  0,  0,(*DispMenuLanguageUp)		},									//ģʽѡ�񣬵���
	
	// �������á��Զ�У׼��
    { 91, 52, 93, 94, 95, 91,(*DispMenuCalibrationInit)	}, 									// �������á��Զ�У׼��
    { 92,  0,  0,  0,  0,  0,(*DispMenuCalibrationON)	},   								// ������������ش�������
    { 93,  0,  0,  0,  0,  0,(*DispMenuCalibrationSel)	}, 									// ������һ����������
    { 94,  0,  0,  0,  0,  0,(*DispMenuCalibrationDown)	}, 									// ģʽѡ�񣬵ݼ�
    { 95,  0,  0,  0,  0,  0,(*DispMenuCalibrationUp)	},									// ģʽѡ�񣬵���

	// �������á��ָ������������á�
    { 96, 52, 98, 99,100, 96,(*DispMenuRestoreFactorySettingsInit)	}, 						// �������á��ָ������������á�
    { 97,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsON)	},   					// ������������ش�������
    { 98,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsSel)	}, 						// ������һ����������
    { 99,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsDown)	}, 						// ģʽѡ�񣬵ݼ�
    {100,  0,  0,  0,  0,  0,(*DispMenuRestoreFactorySettingsUp)	},						// ģʽѡ�񣬵���
	
	// �������á�δ���塱
    {101, 52,103,104,105,101,(*DispMenuUndefineInit)	}, 									// �������á�δ���塱
    {102,  0,  0,  0,  0,  0,(*DispMenuUndefineON)		},   								// ������������ش�������
    {103,  0,  0,  0,  0,  0,(*DispMenuUndefineSel)		}, 									// ������һ����������
    {104,  0,  0,  0,  0,  0,(*DispMenuUndefineDown)	}, 									// ģʽѡ�񣬵ݼ�
    {105,  0,  0,  0,  0,  0,(*DispMenuUndefineUp)		},									// ģʽѡ�񣬵���


	// �����ϵͳ
    {106,107,108,109,110,111,(*DispMenuBuiltInFileInit)		}, 								// �����ϵͳ
    {107,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileON)		},   							// ������������ش�������
    {108,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileSel)		}, 								// ������������ش�������
    {109,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileDown)		}, 								// ����ѡ�񣬵ݼ�
    {110,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileUp)		},								// ����ѡ�񣬵���
	{111,  0,  0,  0,  0,  0,(*DispMenuBuiltInFileLongset)	},								// ����ѡ�񣬵���

	// ����ﱸ�����
    {112,113,114,115,116,117,(*DispMenuRenameFileInit)		}, 								// �����ϵͳ
    {113,  0,  0,  0,  0,  0,(*DispMenuRenameFileON)		},   							// ������������ش�������
    {114,  0,  0,  0,  0,  0,(*DispMenuRenameFileSel)		}, 								// ������������ش�������
    {115,  0,  0,  0,  0,  0,(*DispMenuRenameFileDown)		}, 								// ����ѡ�񣬵ݼ�
    {116,  0,  0,  0,  0,  0,(*DispMenuRenameFileUp)		},								// ����ѡ�񣬵���
    {117,  0,  0,  0,  0,  0,(*DispMenuRenameFileLongSel)	},								// ����

	// ѡ�����
    {118,119,120,121,122,123,(*DispMenuFilelistInit)	}, 									// �����ѡ��
    {119,  0,  0,  0,  0,  0,(*DispMenuFilelistON)		},   								// ������������ش�������
    {120,  0,  0,  0,  0,  0,(*DispMenuFilelistSel)		}, 									// ������������ش�������
    {121,  0,  0,  0,  0,  0,(*DispMenuFilelistDown)	}, 									// ����ѡ�񣬵ݼ�
    {122,  0,  0,  0,  0,  0,(*DispMenuFilelistUp)		},									// ����ѡ�񣬵���
	{123,  0,  0,  0,  0,123,(*DispMenuFilelistLongSel)	}, 									// ����
						 
	// ����У׼	
	{124,125,126,127,128,124,(*DispMenuFactoryCalInit)	}, 									// ����У׼
    {125,  0,  0,  0,  0,  0,(*DispMenuFactoryCalON)	},   								// ������������ش�������
    {126,  0,  0,  0,  0,  0,(*DispMenuFactoryCalSel)	}, 									// ������������ش�������
    {127,  0,  0,  0,  0,  0,(*DispMenuFactoryCalDown)	}, 									// ����ѡ�񣬵ݼ�
    {128,  0,  0,  0,  0,  0,(*DispMenuFactoryCalUp)	},									// ����ѡ�񣬵���

	
	
	{129, 130,131,132,133,184,(*DispFileKeytextInit)	}, 									// ����ģʽ����
    {130,  0,  0,  0,  0,  0,(*DispFileKeytextON)		},   								// ������������ش�������
    {131,  0,  0,  0,  0,  0,(*DispFileKeytextSel)		}, 									// ������һ����������
    {132,  0,  0,  0,  0,  0,(*DispFileKeytextDown)		}, 									// ģʽѡ�񣬵ݼ�
    {133,  0,  0,  0,  0,  0,(*DispFileKeytextUp)		},									// ģʽѡ�񣬵���
	
	{134, 135,136,137,138,134,(*DispFileSeqnumberInit)	}, 									// ﱸ�������
    {135,  0,  0,  0,  0,  0,(*DispFileSeqnumberON)		},   								// ������������ش�������
    {136,  0,  0,  0,  0,  0,(*DispFileSeqnumberSel)	}, 									// ������һ����������
    {137,  0,  0,  0,  0,  0,(*DispFileSeqnumberDown)	}, 									// ģʽѡ�񣬵ݼ�
    {138,  0,  0,  0,  0,  0,(*DispFileSeqnumberUp)		},									// ģʽѡ�񣬵���
	
	{139, 135,141,142,143,139,(*DispFileTaperInit)		}, 									// ׶��
    {140,  0,  0,  0,  0,  0,(*DispFileTaperON)			},   								// ������������ش�������
    {141,  0,  0,  0,  0,  0,(*DispFileTaperSel)		}, 									// ������һ����������
    {142,  0,  0,  0,  0,  0,(*DispFileTaperDown)		}, 									// ģʽѡ�񣬵ݼ�
    {143,  0,  0,  0,  0,  0,(*DispFileTaperUp)			},									// ģʽѡ�񣬵���
	
	{144, 135,146,147,148,144,(*DispFileTapernumberInit)}, 									// ﱺ�
    {145,  0,  0,  0,  0,  0,(*DispFileTapernumberON)	},   								// ������������ش�������
    {146,  0,  0,  0,  0,  0,(*DispFileTapernumberSel)	}, 									// ������һ����������
    {147,  0,  0,  0,  0,  0,(*DispFileTapernumberDown)	}, 									// ģʽѡ�񣬵ݼ�
    {148,  0,  0,  0,  0,  0,(*DispFileTapernumberUp)	},									// ģʽѡ�񣬵���
	
	{149, 135,151,152,153,149,(*DispFileOperationInit)	}, 									// ����ģʽ
    {150,  0,  0,  0,  0,  0,(*DispFileOperationON)		},   								// ������������ش�������
    {151,  0,  0,  0,  0,  0,(*DispFileOperationSel)	}, 									// ������һ����������
    {152,  0,  0,  0,  0,  0,(*DispFileOperationDown)	}, 									// ģʽѡ�񣬵ݼ�
    {153,  0,  0,  0,  0,  0,(*DispFileOperationUp)		},									// ģʽѡ�񣬵���
	
	{154, 135,156,157,158,154,(*DispFileSpeedInit)		}, 									// �ٶ�
    {155,  0,  0,  0,  0,  0,(*DispFileSpeedON)			},   								// ������������ش�������
    {156,  0,  0,  0,  0,  0,(*DispFileSpeedSel)		}, 									// ������һ����������
    {157,  0,  0,  0,  0,  0,(*DispFileSpeedDown)		}, 									// ģʽѡ�񣬵ݼ�
    {158,  0,  0,  0,  0,  0,(*DispFileSpeedUp)			},									// ģʽѡ�񣬵���
	
	{159, 135,161,162,163,159,(*DispFileTorqueInit)		}, 									// ת��
    {160,  0,  0,  0,  0,  0,(*DispFileTorqueON)		},   								// ������������ش�������
    {161,  0,  0,  0,  0,  0,(*DispFileTorqueSel)		}, 									// ������һ����������
    {162,  0,  0,  0,  0,  0,(*DispFileTorqueDown)		}, 									// ģʽѡ�񣬵ݼ�
    {163,  0,  0,  0,  0,  0,(*DispFileTorqueUp)		},									// ģʽѡ�񣬵���
	
	{164, 135,166,167,168,164,(*DispFileCWAngleInit)	}, 									// ���Ƕ�
    {165,  0,  0,  0,  0,  0,(*DispFileCWAngleON)		},   								// ������������ش�������
    {166,  0,  0,  0,  0,  0,(*DispFileCWAngleSel)		}, 									// ������һ����������
    {167,  0,  0,  0,  0,  0,(*DispFileCWAngleDown)		}, 									// ģʽѡ�񣬵ݼ�
    {168,  0,  0,  0,  0,  0,(*DispFileCWAngleUp)		},									// ģʽѡ�񣬵���
	
	{169, 135,171,172,173,169,(*DispFileCCWAngleInit)	}, 									// ���Ƕ�
    {170,  0,  0,  0,  0,  0,(*DispFileCCWAngleON)		},   								// ������������ش�������
    {171,  0,  0,  0,  0,  0,(*DispFileCCWAngleSel)		}, 									// ������һ����������
    {172,  0,  0,  0,  0,  0,(*DispFileCCWAngleDown)	}, 									// ģʽѡ�񣬵ݼ�
    {173,  0,  0,  0,  0,  0,(*DispFileCCWAngleUp)		},									// ģʽѡ�񣬵���
	
	{174, 135,176,177,178,174,(*DispFileColorInit)		}, 									// ��ɫ
    {175,  0,  0,  0,  0,  0,(*DispFileColorON)			},   								// ������������ش�������
    {176,  0,  0,  0,  0,  0,(*DispFileColorSel)		}, 									// ������һ����������
    {177,  0,  0,  0,  0,  0,(*DispFileColorDown)		}, 									// ģʽѡ�񣬵ݼ�
    {178,  0,  0,  0,  0,  0,(*DispFileColorUp)			},									// ģʽѡ�񣬵���
	
	{179, 180,181,182,183,179,(*DispFileDeleteInit)		}, 									// ɾ����ʾ
    {180,  0,  0,  0,  0,  0,(*DispFileDeleteON)		},   								// ������������ش�������
    {181,  0,  0,  0,  0,  0,(*DispFileDeleteSel)		}, 									// ȷ��ɾ��
    {182,  0,  0,  0,  0,  0,(*DispFileDeleteDown)		}, 									// ģʽѡ�񣬵ݼ�
    {183,  0,  0,  0,  0,  0,(*DispFileDeleteUp)		},									// ģʽѡ�񣬵���
	
	{184,  0,  0,  0,  0,  0,(*DispFileKeytextLongSel)	},									// ģʽѡ�񣬵���
};


//��ʱ�ȴ�����������һ��ʱ�䲻�������˻ش�������
int WaitTimeOut(void)
{
	if(TimeOutCnt > d_StandyTime[set.AutoStandyTime]*10)
	{
		TimeOutCnt=0;
		return -1;
	}
	else return 0;
}


