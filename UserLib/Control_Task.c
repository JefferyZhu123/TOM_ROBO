#include "Control_Task.h"
#include "Chassis_Control.h"
#include "Lift_Control.h"
#include "Remote_Control.h"
#include "RobotArm_Control.h"
#include "pump.h"

pid_t pid_pos_can2[6];  //CAN2上电机的位置环
pid_t pid_spd_can2[6];  //CAN2上电机的速度环


/*J-Scope显示波形*/
int32_t Jscope_set_speed;  //电机的设定转速
int32_t Jscope_motor_rpm;  //电机的实际转速
int32_t Jscope_spd_out; //速度环PID输出
int32_t Jscope_pos_out; //位置环PID输出
int32_t Jscope_set_angle;
int32_t Jscope_get_angle;
uint8_t clac_sign;

void MotorPIDInit(void)
{
    /*升降2006电机速度环PID初始化*/
    for(int k=0; k<2; k++)
    {
        PID_struct_init(&pid_spd_can2[k], POSITION_PID, 8000, 20000, 1.5f, 0.06f, 0.0f);
    }
    
    //机械臂底座左右移动2006电机速度环PID初始化
    PID_struct_init(&pid_spd_can2[2], POSITION_PID, 8000, 20000, 1.5f, 0.06f, 0.0f);
    
    
    /*机械臂2006电机PID初始化，机械臂前面的2006电机*/
    PID_struct_init(&pid_pos_can2[3], POSITION_PID, 200, 10, 0.005, 0.0f, 0.0f);
    PID_struct_init(&pid_spd_can2[3], POSITION_PID, 8000, 20000, 3.8f, 0.012f, 0.0f);
    /*机械臂后面的2006电机*/
    PID_struct_init(&pid_pos_can2[4], POSITION_PID, 20, 10, 0.005f, 0.0f, 0.0f);
    PID_struct_init(&pid_spd_can2[4], POSITION_PID, 8000, 20000, 5.0f, 0.003f, 0.0f);
    
//    
    /*机械臂底座回收状态6020电机位置环和速度环PID初始化*/
    PID_struct_init(&pid_pos_can2[5], POSITION_PID, 50, 3, 0.045f, 0.0f, 0.0f);
    PID_struct_init(&pid_spd_can2[5], POSITION_PID, 30000, 23000, 300.0f, 0.0f, 0.0f);
//    
////		/*机械臂底座展开状态6020电机位置环和速度环PID初始化*/
//    PID_struct_init(&pid_pos_can2[5], POSITION_PID, 10, 3, 0.045f, 0.0f, 0.0f);
//    PID_struct_init(&pid_spd_can2[5], POSITION_PID, 30000, 23000, 100.0f, 0.0f, 0.0f);
    
}


void AllControlTask(void)
{
    if(remote_control.switch_right == Switch_Middle)  //遥控器开始时要把右上推杆推至中间，用于底盘控制和机械臂平台移动控制
    {
        if(Chassis_Ctrl)
        { 
            Chassis_Ctrl = 0;
            ChassisConttrol();  //底盘控制
					  base_move_set_value_status = 1;
        }
        if(BaseMove_Ctrl)
        {
            BaseMove_Ctrl = 0;
            BaseMoveControl();  //机械臂平台控制，升降和左右平移
            base_move_set_value_status = 1;
        }
				

    }
    else if(remote_control.switch_right == Switch_Up)   //右上推杆推上，用于机械臂控制
    {
        if(RobotArm_Ctrl)
        {
             RobotArm_Ctrl = 0;
					   EnteirRoboArmControl();//机械臂整体控制，机械臂自转和关节运动 
					   pump_task();
					   robot_base_set_value_status = 1;
             robot_arm_set_value_status = 1; 
        }
//        HAL_Delay(15);   
    }
//    else if(remote_control.switch_right == Switch_Down)   //右上推杆推下，用于机械臂控制
//		{
//			pump_task();
////			robot_arm_set_value_status = 1; 
//		}
    Can2MotorDataSend(); //PID计算和CAN发送数据
    
}



void Can2MotorDataSend(void)
{
    
    if((base_move_set_value_status == 1)||(robot_arm_set_value_status == 1)||(robot_base_set_value_status==1))
    {
        //PID计算

        for(int k=0; k<2; k++)
        {
            pid_calc(&pid_spd_can2[k], moto_2006[k].speed_rpm, set_speed_rpm_lift*36);
        }
        pid_calc(&pid_spd_can2[2], moto_2006[2].speed_rpm, set_speed_rpm_horizontal*36);
        
        
        pid_calc(&pid_pos_can2[3], moto_2006[3].total_angle, ref_angle_arm1);  //位置环PID计算
        pid_calc(&pid_spd_can2[3], moto_2006[3].speed_rpm, (pid_pos_can2[3].pos_out)*36);   //速度环PID计算
    
        pid_calc(&pid_pos_can2[4], moto_2006[4].total_angle, ref_angle_arm2);  //位置环PID计算
        pid_calc(&pid_spd_can2[4], moto_2006[4].speed_rpm, (pid_pos_can2[4].pos_out)*36);   //速度环PID计算
				
				pid_calc(&pid_pos_can2[5], moto_6020[0].total_angle, ref_angle_base);
				pid_calc(&pid_spd_can2[5], moto_6020[0].speed_rpm,pid_pos_can2[5].pos_out);
				
	
				/*Jscope波形显示*/
					Jscope_set_speed=set_speed_test;
					Jscope_set_angle=ref_angle_base;
					Jscope_spd_out=pid_spd_can2[5].pos_out;
					Jscope_motor_rpm =  moto_6020[0].speed_rpm;
					Jscope_get_angle = moto_6020[0].total_angle;
					Jscope_pos_out = pid_pos_can2[5].pos_out;
					MotorSetCurrent1(&hcan2, pid_spd_can2[0].pos_out, pid_spd_can2[1].pos_out, pid_spd_can2[2].pos_out, pid_spd_can2[3].pos_out);
		      RobotArmMotorSetCurrent2(&hcan2,pid_spd_can2[5].pos_out,pid_spd_can2[4].pos_out);
					base_move_set_value_status = 0;
					robot_arm_set_value_status = 0;
					robot_base_set_value_status= 0;		
    }
		//CAN数据发送



    		
}

