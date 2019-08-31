#ifndef __TOBOTARM_CONTROL_
#define __TOBOTARM_CONTROL_


#include "mytype.h"

extern uint8_t RobotArm_Ctrl;
extern uint8_t RobotBase_Ctrl;
extern uint8_t robot_arm_set_value_status;
extern uint8_t robot_base_set_value_status;
extern int32_t ref_angle_base; 
extern int32_t ref_angle_arm1;
extern int32_t ref_angle_arm2;

//≤‚ ‘
extern int32_t set_speed_test;
extern int32_t set_angle_test;

void RobobaseTest(void);
void BaseRotateControl(void);
void RobotArmControl(void);
void EnteirRoboArmControl(void);
void RobobaseControl(void);
#endif

