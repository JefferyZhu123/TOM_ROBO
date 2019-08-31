#ifndef __CHASSIS_CONTROL
#define __CHASSIS_CONTROL


#include "Remote_Control.h"
#include "main.h"
#include "bsp_can.h"
#include "stdio.h"


extern uint8_t Chassis_Ctrl;


void WheelHubMotorTestSpeedMode(void);
void WheelHubMotorTestPositionMode(void);

/*���̲��ٿ���*/
void ChassisConttrol(void);
/*�ٶ�ģʽ�µ����ת�ٵ�����*/
void SetMotorSpeed(CAN_HandleTypeDef *hcan, uint8_t can_id, int16_t speed_rpm);
/*λ��ģʽ�����õ����λ��*/
void SetMotorPosition(CAN_Header *hcan, uint8_t can_id, int32_t position);
/*�������̵���ٶ�ģʽ���Ƴ�ʼ��*/
void MotorSpeedModeInit(uint8_t speed_up_time, uint8_t speed_cut_time);
/*���λ��ģʽ���Ƴ�ʼ��*/
void MotorPositionModeInit(uint8_t speed_up_time, uint8_t speed_cut_time, uint16_t max_speed_rpm, uint8_t mode);
/*�ٶ�ģʽ�����£���������������ٶ�����*/
void set_speed(int16_t speed_rpm_l, int16_t speed_rpm_r);
/*���̵���Ĳ�������*/
void set_motor_data_can1(CAN_HandleTypeDef *hcan, uint8_t can_id, uint8_t Data1,uint8_t Data2,uint8_t Data3,uint8_t Data4,\
    uint8_t Data5,uint8_t Data6,uint8_t Data7,uint8_t Data8);

#endif
