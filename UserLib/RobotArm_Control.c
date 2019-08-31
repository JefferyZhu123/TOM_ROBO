#include "RobotArm_Control.h"
#include "bsp_can.h"
#include "Remote_Control.h"
#include "Control_Task.h"


uint8_t RobotArm_Ctrl;
uint8_t RobotBase_Ctrl;
uint8_t robot_arm_set_value_status;
uint8_t robot_base_set_value_status;
int32_t ref_angle_base;  //机械臂底座旋转角度值
int32_t ref_angle_arm1;  //机械臂前面2006电机角度值
int32_t ref_angle_arm2;  //机械臂后面2006电机角度值

//测试
int32_t set_speed_test;
int32_t set_angle_test;

//机械臂底座旋转控制
void BaseRotateControl(void)
{
    
      ref_angle_base -= (remote_control.ch3)*0.01;
}

void RobobaseTest(void)
{
	 
		if(remote_control.switch_left == Switch_Up)
	{
		ref_angle_base = 8192;  //底座6020电机
	
	}
	if(remote_control.switch_left == Switch_Middle)
	{
		ref_angle_base =0;  //底座6020电机
	}		
}
void RobobaseControl(void)
{  
	  
	/*角度环测试*/	
	  ref_angle_base += remote_control.ch3*0.002;
	
}
void RobotArmControl(void)
{

/*位置环测试*/    
      ref_angle_arm1 -= (remote_control.ch1)*36*0.002;  //机械臂前面2006电机
      ref_angle_arm2 += (remote_control.ch2)*36*0.002;  //机械臂后面2006电机
    
}



void EnteirRoboArmControl(void)
{
    
      RobobaseControl();
      RobotArmControl();
    
}
void pid_reset_speed(uint32_t kp, uint32_t ki, uint32_t kd)
{
    pid_spd_can2[5].p = kp/1000.0f;
    pid_spd_can2[5].i = ki/1000.0f;
    pid_spd_can2[5].d = kd/1000.0f;
}


void pid_reset_position(uint32_t kp, uint32_t ki, uint32_t kd)
{
    pid_pos_can2[5].p = kp/1000.0f;
    pid_pos_can2[5].i = ki/1000.0f;
    pid_pos_can2[5].d = kd/1000.0f; 
}

