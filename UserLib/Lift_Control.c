#include "Lift_Control.h"
#include "Remote_Control.h"
#include "bsp_can.h"
#include "Control_Task.h"


uint8_t BaseMove_Ctrl;
int32_t set_speed_rpm_lift;
int32_t set_speed_rpm_horizontal;
uint8_t base_move_set_value_status;


//ˮƽ�ƶ�����
void HorizontalMovementControl(void)
{
    
    if(remote_control.ch3 >= 50)
    {
        set_speed_rpm_horizontal = -MAX_SPEED_HORZ;
    }
    else if(remote_control.ch3 <= -50)
    {
        set_speed_rpm_horizontal = MAX_SPEED_HORZ;
    }
    else
    {
        set_speed_rpm_horizontal = 0;
    }
    
//    Jscope_set_speed = set_speed_rpm_horizontal;
    //PID����
//    pid_calc(&pid_spd_can2[2], moto_2006[2].speed_rpm, set_speed_rpm_horizontal*36);
    
}


//��������
void LiftControl(void)
{
    
    if(remote_control.ch4 >= 50)
    {
        set_speed_rpm_lift = MAX_SPEED_LIFT;
    }
    else if(remote_control.ch4 <= -50)
    {
        set_speed_rpm_lift = -MAX_SPEED_LIFT;
    }
    else
    {
        set_speed_rpm_lift = 0;
    }
    
    //PID����
//    for(int k=0; k<2; k++)
//    {
//        pid_calc(&pid_spd_can2[k], moto_2006[k].speed_rpm, set_speed_rpm_lift*36);
//    }
    
    Jscope_set_speed = set_speed_rpm_lift;
    Jscope_motor_rpm = moto_2006[0].speed_rpm/36;
    
}



void BaseMoveControl(void)
{
    LiftControl();  //��е�۵�����������
    HorizontalMovementControl();  //��е�۵���ˮƽ�ƶ�����
    
    
    //CAN����2006�������ֵ
//    MobilePlatformMotorSetCurrent(&hcan2,pid_spd_can2[0].pos_out,pid_spd_can2[1].pos_out,pid_spd_can2[2].pos_out);
    
}




