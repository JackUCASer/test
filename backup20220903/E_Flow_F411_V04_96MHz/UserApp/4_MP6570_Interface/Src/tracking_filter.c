#include "tracking_filter.h"
#include "common.h"
void tracking_calc(TRACKING_TypeDef *v)
{
	v->e = v->angle - v->position; //in - out
	v->temp1= v->e * v->beta;// v->beta * track_error; 
	v->Un = v->temp1 + v->Un;
	v->temp2 = v->e * v->alpha;// track_error*v->alpha;
	v->velocity =  (v->temp2 + v->Un);
	v->position = v->position + (v->velocity>>16);
	v->velocity =  (v->temp2 + v->Un) - 0x8000;
}
