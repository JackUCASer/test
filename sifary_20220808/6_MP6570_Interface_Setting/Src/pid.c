#include "pid.h"
#include "common.h"

inline void pid_calc(PID *v)
{	 
    v->e = v->pid_ref - v->pid_fdb;           // Compute the error//IQ15   
	  if(v->Kp_gain & 0x8000) //gain decrease
			v->up = mul_32(v->e,v->Kp)>>(v->Kp_gain & 0x7FFF);
		else //gain increase
			v->up = mul_32(v->e,v->Kp)<<v->Kp_gain;
    //v->up = mul_32((v->e<<v->Kp_gain),v->Kp);//(v->Kp*v->Kp_gain * v->e);               // Compute the proportional output//IQ15
    if(v->Ki_gain & 0x8000)
			v->ui = v->ui + (mul_32(v->e,v->Ki)>>(v->Ki_gain & 0x7FFF)) + mul_32(v->saterr,v->Kc);//((v->Ki * v->e)) + ((v->Kc * v->saterr)); // Compute the integral output// IQ15
    else
			v->ui = v->ui + (mul_32(v->e,v->Ki)<<v->Ki_gain) + mul_32(v->saterr,v->Kc);//((v->Ki * v->e)) + ((v->Kc * v->saterr)); // Compute the integral output// IQ15
		//v->ud = mul_32(v->e,v->Kd);//v->Kd * v->e;   // Compute the derivative output//IQ15   
    v->uprsat = ((v->up + v->ui)>>15); //+ v->ud       //Compute the pre-saturated output/IQ15
    if (v->uprsat > v->pid_out_max)						// Saturate the output
      v->pid_out =  v->pid_out_max;
    else if (v->uprsat < v->pid_out_min)
      v->pid_out =  v->pid_out_min;
    else
      v->pid_out = v->uprsat;   
    v->saterr = v->pid_out - v->uprsat;				// Compute the saturate difference
    //v->up1 = v->up;														// Update the previous proportional output
}
