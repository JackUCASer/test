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


void	DispFileKeytextInit();									// ����ģʽ����
void	DispFileKeytextON();   									// ������������ش�������
void	DispFileKeytextSel(); 									// ������һ����������
void	DispFileKeytextDown(); 									// ģʽѡ�񣬵ݼ�
void	DispFileKeytextUp();									// ģʽѡ�񣬵���
void	DispFileKeytextLongSel();								// ��������

void	DispFileSeqnumberInit(); 								// ﱸ�������
void	DispFileSeqnumberON();   								// ������������ش�������
void	DispFileSeqnumberSel(); 								// ������һ����������
void	DispFileSeqnumberDown(); 								// ģʽѡ�񣬵ݼ�
void	DispFileSeqnumberUp();									// ģʽѡ�񣬵���
	
void	DispFileTaperInit(); 									// ׶��
void	DispFileTaperON();   									// ������������ش�������
void	DispFileTaperSel();										// ������һ����������
void	DispFileTaperDown(); 									// ģʽѡ�񣬵ݼ�
void	DispFileTaperUp();										// ģʽѡ�񣬵���
	
void	DispFileTapernumberInit(); 								// ﱺ�
void	DispFileTapernumberON();   								// ������������ش�������
void	DispFileTapernumberSel(); 								// ������һ����������
void	DispFileTapernumberDown();								// ģʽѡ�񣬵ݼ�
void	DispFileTapernumberUp();								// ģʽѡ�񣬵���
	
void	DispFileOperationInit(); 								// ����ģʽ
void	DispFileOperationON();   								// ������������ش�������
void	DispFileOperationSel(); 								// ������һ����������
void	DispFileOperationDown(); 								// ģʽѡ�񣬵ݼ�
void	DispFileOperationUp();									// ģʽѡ�񣬵���
	
void	DispFileSpeedInit(); 									// �ٶ�
void	DispFileSpeedON();   									// ������������ش�������
void	DispFileSpeedSel(); 									// ������һ����������
void	DispFileSpeedDown(); 									// ģʽѡ�񣬵ݼ�
void	DispFileSpeedUp();										// ģʽѡ�񣬵���
	
void	DispFileTorqueInit();									// ת��
void	DispFileTorqueON();   									// ������������ش�������
void	DispFileTorqueSel(); 									// ������һ����������
void	DispFileTorqueDown(); 									// ģʽѡ�񣬵ݼ�
void	DispFileTorqueUp();										// ģʽѡ�񣬵���
	
void	DispFileCWAngleInit(); 									// ���Ƕ�
void	DispFileCWAngleON();   									// ������������ش�������
void	DispFileCWAngleSel(); 									// ������һ����������
void	DispFileCWAngleDown(); 									// ģʽѡ�񣬵ݼ�
void	DispFileCWAngleUp();									// ģʽѡ�񣬵���
	
void	DispFileCCWAngleInit(); 								// ���Ƕ�
void	DispFileCCWAngleON();   								// ������������ش�������
void	DispFileCCWAngleSel(); 									// ������һ����������
void	DispFileCCWAngleDown(); 								// ģʽѡ�񣬵ݼ�
void	DispFileCCWAngleUp();									// ģʽѡ�񣬵���
	
void	DispFileColorInit(); 									// ��ɫ
void	DispFileColorON();   									// ������������ش�������
void	DispFileColorSel();										// ������һ����������
void	DispFileColorDown(); 									// ģʽѡ�񣬵ݼ�
void	DispFileColorUp();										// ģʽѡ�񣬵���






#endif