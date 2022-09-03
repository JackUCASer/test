#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "common.h"

extern unsigned short regi[128];
extern unsigned char update_command; //set to 1 to update speed or position command
extern unsigned char errorStatusLatch; 
extern int iq; //motor iq
extern u8 error_status;

void reset_parameters(void);
void reset_ref_and_loop(void);
void control_loop(void);
void clearError(void);
#endif
