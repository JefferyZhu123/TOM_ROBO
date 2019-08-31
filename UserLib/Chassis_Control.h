#ifndef __CHASSIS_CONTROL
#define __CHASSIS_CONTROL


#include "Remote_Control.h"
#include "main.h"
#include "bsp_can.h"
#include "stdio.h"


extern uint8_t Chassis_Ctrl;


void WheelHubMotorTestSpeedMode(void);
void WheelHubMotorTestPositionMode(void);

/*底盘差速控制*/
void ChassisConttrol(void);
/*速度模式下电机的转速的设置*/
void SetMotorSpeed(CAN_HandleTypeDef *hcan, uint8_t can_id, int16_t speed_rpm);
/*位置模式下设置电机的位置*/
void SetMotorPosition(CAN_Header *hcan, uint8_t can_id, int32_t position);
/*两个底盘电机速度模式控制初始化*/
void MotorSpeedModeInit(uint8_t speed_up_time, uint8_t speed_cut_time);
/*电机位置模式控制初始化*/
void MotorPositionModeInit(uint8_t speed_up_time, uint8_t speed_cut_time, uint16_t max_speed_rpm, uint8_t mode);
/*速度模式控制下，底盘两个电机的速度设置*/
void set_speed(int16_t speed_rpm_l, int16_t speed_rpm_r);
/*底盘电机的参数设置*/
void set_motor_data_can1(CAN_HandleTypeDef *hcan, uint8_t can_id, uint8_t Data1,uint8_t Data2,uint8_t Data3,uint8_t Data4,\
    uint8_t Data5,uint8_t Data6,uint8_t Data7,uint8_t Data8);

#endif
