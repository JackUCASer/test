#ifndef __APEX_H
#define __APEX_H

#include "stm32f4xx_hal.h"

#define ROOT_400HZ_ARR 2500									// ��Ƶ����������2500us
#define ROOT_400HZ_CCR 1250									// ��Ƶ������ռ�ձ�1250us

#define SHORT_CIRCUIT_THRESHOLD  50							// �̽��ж�ʱ��

#define ROOT_8KHZ_ARR 125									// ��Ƶ�񵴵�����125us
#define ROOT_8KHZ_CCR 62									// ��Ƶ�񵴵�ռ�ձ�62us

#define ROOT_7KHZ_ARR 116									

#define IN_DELAY_CNT  6
#define DELAY_CNT 1

#define FILTER_N  (4)                   					// ����ƫ������˲�����ֵƫ�ƶ��ٸ���λ���ڣ���Ϊͬһֵ
typedef struct FL
{
    unsigned char samL_Rise_begin;
    unsigned char samH_Rise_begin;
    unsigned char sam_finish;
	unsigned char sam_apex_begin;
} Str_FL;     //ȫ�ֱ�־λ

typedef struct SAM {
    uint32_t ave;
    uint32_t sum;
    uint32_t max;
    uint32_t min;

    uint32_t VPP;

} SAM_DAT;
extern SAM_DAT samh,saml;									// ��¼��Ƶ�źš���Ƶ�źŲ����������ݵ�ƽ��ֵ���ܺ͡����ֵ����Сֵ�ͷ��ֵ
extern uint32_t SamH[20];									// ��Ƶ�źŲ������ݻ���
extern uint32_t SamL[20];									// ��Ƶ�źŲ������ݻ���
extern uint32_t Ratio_mul;									// ����
extern unsigned int Ratio;									// ����

extern float E_rate;
extern	uint32_t	CAL_APEX_CAL_3K6_100nF;					// ÿ�ο���ʱ����ҪУ��һ�£���Init_APEX_3K6_100nF��ȣ��ó��䶯�ʣ���Ҫ�������ϻ���������
extern	const unsigned int Init_APEX_3K6_100nF ;			// ����ʱ�Ķ�ֵ�����ɸ��ĵ�
extern uint32_t  samH_RootInPos;							// ���ݸ�Ƶ�źŵı仯��ȷ������Ƿ�������						
typedef struct  Apex_t {
    uint16_t Pos__2;
    uint16_t Pos__1;
    uint16_t Pos_00;			//0
    uint16_t Pos_01;
    uint16_t Pos_02;			//0.5
    uint16_t Pos_03;
    uint16_t Pos_04;			//1
    uint16_t Pos_05;
    uint16_t Pos_06;			//1.5
    uint16_t Pos_07;
    uint16_t Pos_08;			//2
    uint16_t Pos_09;
    uint16_t Pos_10;			//2.5
    uint16_t Pos_11;
    uint16_t Pos_12;	  		//3
    uint16_t Pos_13;
    uint16_t Pos_14;
    uint16_t Pos_15;
    uint16_t Pos_16;
    uint16_t Pos_17;


} ApexTab;
typedef struct AT {
    unsigned char cnt;
    unsigned char num;
} ApexNum;
void Get_Set_ApexFre();
void CalculateApex();
void Apex_cal();
#endif