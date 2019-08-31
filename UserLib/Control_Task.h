#ifndef __CONTROL_TASK_
#define __CONTROL_TASK_

#include "pid.h"


extern pid_t pid_pos_can2[6];
extern pid_t pid_spd_can2[6];


extern int32_t Jscope_set_speed;
extern int32_t Jscope_motor_rpm;
extern int32_t Jscope_spd_out;
extern int32_t Jscope_pos_out;
extern int32_t Jscope_set_angle;
extern int32_t Jscope_get_angle;


void AllControlTask(void);
void MotorPIDInit(void);
void Can2MotorDataSend(void);


#endif

