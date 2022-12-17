#ifndef _PID_H_
#define _PID_H_

typedef struct {  
					int  pid_ref;   								/* Input: Reference input  参考设定输入*/
					int  pid_fdb;   								/* Input: Feedback input   反馈值*/
					int  e;			      							/* Variable: Error 		   偏差值*/
					unsigned short  Kp_gain;
					unsigned short  Kp;			    				/* Parameter: Proportional gain, increase 2^15 gain  */
					long long int  up;			    				/* Variable: Proportional output  */
					long long int  ui;			    				/* Variable: Integral output    */
					long long int  ud;			    				/* Variable: Derivative output    */	
					int  uprsat; 	    							/* Variable: Pre-saturated output  */
					int  pid_out_max; 								/* Parameter: Maximum output    */
					int  pid_out_min;								/* Parameter: Minimum output    */
					int  pid_out;   								/* Output: PID output    */
					int  saterr;		  							/* Variable: Saturated difference 饱和 */
					unsigned short  Ki;			    				/* Parameter: Integral gain   increase 2^15 gain */
					unsigned short  Ki_gain;
					unsigned short  Kc;			    				/* Parameter: Integral correction gain  */
					unsigned short  Kd; 		      				/* Parameter: Derivative gain  导数 */
					int  up1;		      							/* History: Previous proportional output  */
					void  (*calc)();								/* Pointer to calculation function */ 
				 } PID;	            

typedef PID *PID_handle;			 
#define PID_DEFAULTS { 0, \
                       0, \
                       0, \
											 1, \
                       0, \
                       0, \
                       0, \
                       0, \
                       0, \
                       0x000003FF, \
                       0xFFFFFC00, \
                       0, \
                       0, \
                       0, \
                       0, \
											 0, \
                       0, \
                       0, \
              		   (void (*)(long))pid_calc}

/*------------------------------------------------------------------------------
Prototypes for the functions in PID.C
------------------------------------------------------------------------------*/
void pid_calc(PID_handle);										 
#endif
