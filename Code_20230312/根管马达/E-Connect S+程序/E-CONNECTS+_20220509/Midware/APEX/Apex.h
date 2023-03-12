#ifndef __APEX_H
#define __APEX_H

#include "stm32f4xx_hal.h"

#define ROOT_400HZ_ARR 2500
#define ROOT_400HZ_CCR 1250

#define SHORT_CIRCUIT_THRESHOLD  50

#define ROOT_8KHZ_ARR 125
#define ROOT_8KHZ_CCR 62

#define ROOT_7KHZ_ARR 116

#define IN_DELAY_CNT  6
#define DELAY_CNT 1

#define FILTER_N  (4)                   // 采样偏差控制滤波，均值偏移多少个单位以内，视为同一值
typedef struct FL
{

    unsigned char samL_Rise_begin;
    unsigned char samH_Rise_begin;
    unsigned char sam_finish;



} Str_FL;     //全局标志位

typedef struct SAM {
    uint32_t ave;
    uint32_t sum;
    uint32_t max;
    uint32_t min;

    uint32_t VPP;

} SAM_DAT;
extern SAM_DAT samh,saml;
extern uint32_t SamH[20];
extern uint32_t SamL[20];
extern uint32_t Ratio_mul;
extern unsigned int Ratio;

extern float E_rate;
extern	uint32_t	CAL_APEX_CAL_3K6_100nF;
extern	const unsigned int Init_APEX_3K6_100nF ;
extern uint32_t  samH_RootInPos;
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
    uint16_t Pos_12;	  	//3
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