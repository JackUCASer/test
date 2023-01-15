#include "rc_filter.h"
#include "common.h"
void rc_calc(RC_TypeDef *v)
{
	v->e = v->in - v->out; //e(n) = x(n)-y(n)
	
	v->out = (mul_32(v->e,v->alpha)>>16) + v->out; //y(n+1)=y(n)+alpha*(x(n)-y(n))
}
