#ifndef __LIFT_CONTROL
#define __LIFT_CONTROL 


#include "mytype.h"

#define MAX_SPEED_HORZ  200  //��е�۵���ˮƽ�����ƶ����ٶȣ���λ��rpm
#define MAX_SPEED_LIFT  90   //��е��ƽ̨�����ƶ����ٶȣ���λ��rpm

extern uint8_t BaseMove_Ctrl;
extern int32_t set_speed_rpm_lift;
extern int32_t set_speed_rpm_horizontal;
extern uint8_t base_move_set_value_status;


void LiftControl(void);
void HorizontalMovementControl(void);
void BaseMoveControl(void);
void pid_reset_speed(uint32_t kp, uint32_t ki, uint32_t kd);
void pid_reset_position(uint32_t kp, uint32_t ki, uint32_t kd);


#endif

