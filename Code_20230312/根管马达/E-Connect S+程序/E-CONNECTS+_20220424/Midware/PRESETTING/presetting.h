
////////////////////////////////////////////////////////////////////
/// @file preSetting.h
/// @brief ϵͳ����������ǰ��Ԥ�����������ݳ�ʼ���ȵ�
/// 
/// �ļ���ϸ������	void power_on()����������⣬��ͬ������ϣ����벻ͬ��ģʽ
///					
///
///
///
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

#ifndef __PRESETTING_H_
#define __PRESETTING_H_



void power_on(void);	
void Power_mode_select(void);							// �������ݲ�ͬ������ϣ����벻ͬ����ģʽ
void data_init(void);									// ���ݳ�ʼ������EEPROM�����ó�ʼģʽ���˵���ʾ��ʼ����
void Start_Adc(void);									// ADC����1��
void init_MC(void);										// ��ʼ���������

#endif