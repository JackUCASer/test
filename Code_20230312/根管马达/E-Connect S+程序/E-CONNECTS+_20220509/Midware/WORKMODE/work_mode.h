
////////////////////////////////////////////////////////////////////
/// @file work_mode.h
/// @brief ����ģʽ����Ҫ�д���ģʽ����ʾLOGOģʽ������ģʽ�ȵ�
/// 
/// �ļ���ϸ������	WorkMode_e Rotory(WorkMode_e mode);					// �����˶�ģʽ��������ת����ת
/// 				WorkMode_e Reciproc(WorkMode_e mode);				// �����˶�ģʽ
/// 				WorkMode_e ATC_RUN(WorkMode_e mode);				// ATC�˶�ģʽ
/// 				WorkMode_e Measure(WorkMode_e mode);				// ����ģʽ
/// 				WorkMode_e RunWithApex(WorkMode_e mode);			// ����+����ģʽ
/// 				WorkMode_e RecWithApex(WorkMode_e mode);			// ����+����ģʽ
/// 				WorkMode_e ATCwithApex(WorkMode_e mode);			// ATC+����ģʽ
/// 				WorkMode_e Standby(WorkMode_e mode);				// ����ģʽ	
/// 				WorkMode_e Charging(WorkMode_e mode);				// ���ģʽ
/// 				WorkMode_e Fault(WorkMode_e mode);					// ���д���ģʽ
/// 				WorkMode_e PowerOff(WorkMode_e mode);				// �ػ�Ԥ��ģʽ
/// 				WorkMode_e DispOff(WorkMode_e mode);				// �ر���ʾģʽ
/// 				WorkMode_e Displaylogo(WorkMode_e mode);			// ��ʾLOGOģʽ
///
/// @author ����ʢ
/// @version 1.1.1.0
/// @date 20210813
/// 
/// <b>�޸���ʷ��--
/// - 1. <date> <author> <modification>
/// - 2. <date> <author> <modification>
/// 
/// ��˾����������ҽ�Ƽ������޹�˾
////////////////////////////////////////////////////////////////////

#ifndef __WORK_MODE_H_
#define __WORK_MODE_H_
#include "datatype.h"

typedef enum
{
    Rotory_mode = 0,								// �����˶�ģʽ
    Reciproc_mode,									// �����˶�ģʽ
    Atc_mode,										// ATC�˶�ģʽ
    Measure_mode,									// ����ģʽ
    RunWithApex_mode,								// ����+����ģʽ
    RecWithApex_mode,								// ����+����ģʽ
    ATRwithApex_mode,								// ATC+����ģʽ
	Standby_mode,									// ����ģʽ
    Charging_mode,									// ���ģʽ
    Fault_mode,										// ����ģʽ
    PowerOff_mode,									// �Զ��ػ�ģʽ
    DispOff_mode,									// �ر���ʾģʽ
	DisplayLogo_mode,								// ��ʾLOGOģʽ
} WorkMode_e;                 						// ����ģʽö��
extern WorkMode_e WorkMode;

WorkMode_e Rotory(WorkMode_e mode);					// �����˶�ģʽ��������ת����ת
WorkMode_e Reciproc(WorkMode_e mode);				// �����˶�ģʽ
WorkMode_e ATC_RUN(WorkMode_e mode);				// ATC�˶�ģʽ
WorkMode_e Measure(WorkMode_e mode);				// ����ģʽ
WorkMode_e RunWithApex(WorkMode_e mode);			// ����+����ģʽ
WorkMode_e RecWithApex(WorkMode_e mode);			// ����+����ģʽ
WorkMode_e ATCwithApex(WorkMode_e mode);			// ATC+����ģʽ
WorkMode_e Standby(WorkMode_e mode);				// ����ģʽ	
WorkMode_e Charging(WorkMode_e mode);				// ���ģʽ
WorkMode_e Fault(WorkMode_e mode);					// ���д���ģʽ
WorkMode_e PowerOff(WorkMode_e mode);				// �ػ�Ԥ��ģʽ
WorkMode_e DispOff(WorkMode_e mode);				// �ر���ʾģʽ
WorkMode_e Displaylogo(WorkMode_e mode);			// ��ʾLOGOģʽ
WorkMode_e RunningTurn2Apex(WorkMode_e mode);		// ����������ģʽ��⵽�����źţ�ת�������߲�ģʽ
WorkMode_e MonitorApexStart(WorkMode_e mode);		// �����߲���ӳ���,������ڰ���ɨ�躯������
WorkMode_e MonitorApexStop(WorkMode_e mode);		// �����߲���ӳ���,�뿪���ܣ��Զ�ֹͣ
void MonitorApexAction(WorkMode_e mode);			// �����߲���ӳ���,�쵽�趨�����ʱ��ѡ����ε������״̬
void Turn2Standby(WorkMode_e mode);					// ����Standbyǰ�Բ�������
void Motor2Reverse();
void Motor2Forward();
void Motor2INREC();
void Motor2OUTREC();


#endif




