#include "arm_pid.h"

arm_pid_instance_f32 S;

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-10-13
* 函 数 名: arm_pid_Init
* 功能说明: PID参数初始化
* 形    参: 

* 返 回 值: 
*********************************************************************************************************
*/
void arm_pid_Init(void)
{
	S.Kd = 1;					//	比例因子
	S.Ki = 1;					//	积分因子
	S.Kp = 0;					//	微分因子
	arm_pid_init_f32(&S, 1);
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-10-13
* 函 数 名: arm_pid_Calu
* 功能说明: PID控制
* 形    参: 

* 返 回 值: 
*********************************************************************************************************
*/
int32_t arm_pid_Calu(int32_t in)
{
	float32_t out;
	out = arm_pid_f32(&S,(float)in);
	return (int32_t)out;
}

/*
*********************************************************************************************************
* 设 计 者：王远，常州赛乐医疗技术有限公司
* 版	  本：V1.0
* 日    期：2021-10-13
* 函 数 名: arm_pid_Calu
* 功能说明: PID重置相关变量为0
* 形    参: 

* 返 回 值: 
*********************************************************************************************************
*/
void arm_pid_DeInit(void)
{
	arm_pid_reset_f32(&S);
}
