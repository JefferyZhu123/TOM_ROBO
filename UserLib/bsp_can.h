#ifndef __BSP_CAN
#define __BSP_CAN

#include "can.h"
#include "mytype.h"


#define FILTER_BUF_LEN		5


/*定义CAN发送或是接收的ID*/
typedef enum
{

	CAN_2006Moto1_ID = 0x201,
	CAN_2006Moto2_ID = 0x202,
	CAN_2006Moto3_ID = 0x203,
	CAN_2006Moto4_ID = 0x204,
  CAN_6020Moto6_ID = 0x205,
  CAN_2006Moto6_ID = 0x206,
	
}CAN_Message_ID;



typedef struct{
	
    CAN_TxHeaderTypeDef   tx_header;
    CAN_RxHeaderTypeDef   rx_header;
    uint8_t               tx_data[8];
    uint8_t               rx_data[8];
	
}CAN_Header;


typedef struct{
	int16_t	 	speed_rpm;
    float  	    real_current;
    int16_t  	given_current;
    uint8_t  	hall;
	uint16_t 	angle;		//abs angle range:[0,8191]
	uint16_t 	last_angle;	  //abs angle range:[0,8191]
	uint16_t	offset_angle;
	int32_t		round_cnt;
	int32_t		total_angle;
	u8			buf_idx;
	u16			angle_buf[FILTER_BUF_LEN];
	u16			fited_angle;
	u32			msg_cnt;
}moto_measure_t;


extern moto_measure_t moto_2006[];
extern moto_measure_t moto_6020[];
extern CAN_Header CAN1_Header;
extern CAN_Header CAN2_Header;


void can_filter_init(void);
void get_moto_measure(moto_measure_t *ptr, uint8_t can_rx_data[]);
void get_moto_offset(moto_measure_t *ptr, uint8_t can_rx_data[]);
void RobotArmMotorSetCurrent2(CAN_HandleTypeDef *hcan,s16 iq1,s16 iq2);
void MotorSetCurrent1(CAN_HandleTypeDef *hcan,s16 iq1, s16 iq2, s16 iq3, s16 iq4);
void BaseRotateMotorSetVoltage(CAN_HandleTypeDef *hcan,s16 iq2);


#endif

