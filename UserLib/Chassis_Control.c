#include "Chassis_Control.h"


int Flag = 0;
int32_t set_spd[2] = {0};
int32_t front_back = 0;
int32_t left_right = 0;
int32_t rotate = 0;
uint8_t Chassis_Ctrl;



/*电机的速度模式测试*/
void WheelHubMotorTestSpeedMode(void)
{
    
    if(remote_control.switch_right == Switch_Middle)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
        
//        set_motor_data_can1(&CAN1_Header,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x0F);  //不使能电机，脱机状态
        set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x30,0x00,0x00,0x00,0x1F);  //急停
        
    }
    else if(remote_control.switch_right == Switch_Up)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
        
        SetMotorSpeed(&hcan1,0x0A,100); //设置速度为100rpm，最高速1000rpm
        HAL_Delay(10);
        
    }
    else if(remote_control.switch_right == Switch_Down)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
        
        SetMotorSpeed(&hcan1,0x0A,50);
        HAL_Delay(10);
        
    }
	
}



/*电机的位置模式测试*/
void WheelHubMotorTestPositionMode(void)
{
    
    if(remote_control.switch_right == Switch_Middle)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
        Flag = 1;
        
    }
    else if((remote_control.switch_right == Switch_Up)&&(Flag == 1))
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
        Flag = 0;
        
        SetMotorPosition(&CAN1_Header,0x0A,10000);
        HAL_Delay(10);
        
    }
    else if((remote_control.switch_right == Switch_Down)&&(Flag == 1))
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
        Flag = 0;
       
        SetMotorPosition(&CAN1_Header,0x0A,-20000);
        HAL_Delay(10);
        
    }
    if(remote_control.switch_left == Switch_Down)
    {
        
        set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x30,0x00,0x00,0x00,0x1F);  //急停
        
    }
    
    
    
}


/*底盘差速控制*/
void ChassisConttrol(void)
{
    
    front_back = remote_control.ch2*300/660;
    left_right = remote_control.ch1*300/660;
//    rotate = remote_control.ch3*300/660;

    if(remote_control.ch1 < 0)
    {
        set_spd[0] = front_back;
        set_spd[1] = left_right - front_back;//2max
    }
    else if(remote_control.ch1 > 0)
    {
        set_spd[0] = left_right + front_back;//2max
        set_spd[1] = -front_back; 
    }
    
//    else if(remote_control.ch3 > 0)
//    {
//        set_spd[0] = rotate;
//        set_spd[1] = rotate;
//        
//    }
//    else if(remote_control.ch3 < 0)
//    {
//        set_spd[0] = rotate;
//        set_spd[1] = rotate;
//    }
    
    else
    {
        set_spd[0] = front_back;
        set_spd[1] = -front_back;
    }
    
    set_speed(set_spd[0], set_spd[1]);
    
}


/**********************************************************************************************************************
 * @brief   速度模式下电机的转速的设置
 * @param   CAN_Header *hcan       ：自定义的CAN收发结构体
 *          uint8_t can_id         ：CAN发送报文的ID，对应电机的ID
 *          int16_t speed_rpm      ：电机速度，单位 rpm
 * @retval  None
**********************************************************************************************************************/
void SetMotorSpeed(CAN_HandleTypeDef *hcan, uint8_t can_id, int16_t speed_rpm)
{
		CAN_TxHeaderTypeDef   tx_header;
	  uint8_t               tx_data[8];
    
    int16_t speed = 0;
    speed = 8192*speed_rpm/3000;
    
  tx_header.StdId = can_id;  //标准标识符
	tx_header.IDE = CAN_ID_STD;  //标准帧
	tx_header.RTR = CAN_RTR_DATA;  //为数据帧
	tx_header.DLC = 0x08;  //消息的数据长度为8个字节
	tx_data[0]=0x00;       //从机组号
  tx_data[1]=0xDA;       //功能码 
	tx_data[2]=0x00;       //寄存器地址
  tx_data[3]=0x11;       //寄存器子地址
	tx_data[4]=0x00;       //数据1高8位
  tx_data[5]=0x00;       //数据1低8位
	tx_data[6]=(speed>>8); //数据2高8位
  tx_data[7]=speed;      //数据2低8位 
    
    HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);
    
}


/**********************************************************************************************************************
 * @brief   位置模式下设置电机的位置
 * @param   CAN_Header *hcan        ：自定义的CAN收发结构体
 *          uint8_t can_id          ：CAN发送报文的ID，对应电机的ID
 *          int32_t position        ：电机编码器对应的数值
 * @retval  None
**********************************************************************************************************************/
void SetMotorPosition(CAN_Header *hcan, uint8_t can_id, int32_t position)
{
    
    int16_t pos_1,pos_2;
    pos_1 = position>>16;  //得到高16位
    pos_2 = position;   //得到低16位
    
    hcan->tx_header.StdId = can_id;  //标准标识符
	hcan->tx_header.IDE = CAN_ID_STD;  //标准帧
	hcan->tx_header.RTR = CAN_RTR_DATA;  //为数据帧
	hcan->tx_header.DLC = 0x08;  //消息的数据长度为8个字节
    hcan->tx_data[0]=0x00;       //从机组号
    hcan->tx_data[1]=0xDA;       //功能码
    hcan->tx_data[2]=0x00;       //寄存器地址
    hcan->tx_data[3]=0x16;       //寄存器子地址
    hcan->tx_data[4]=pos_1>>8;
    hcan->tx_data[5]=pos_1;
    hcan->tx_data[6]=pos_2>>8;
    hcan->tx_data[7]=pos_2;
    
    HAL_CAN_AddTxMessage(&hcan1, &(hcan->tx_header), hcan->tx_data,(uint32_t*)CAN_TX_MAILBOX0);
    
}


/**********************************************************************************************************************
 * @brief   两个底盘电机速度模式控制初始化
 * @param   uint8_t speed_up_time   ：加速时间，单位 ms
 *          uint8_t speed_cut_time  ：减速时间，单位 ms
 * @retval  None
**********************************************************************************************************************/
void MotorSpeedModeInit(uint8_t speed_up_time, uint8_t speed_cut_time)
{
    
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F); //速度模式
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F); //速度模式
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x13,0x00,0x00,speed_up_time,speed_cut_time); //加减速为10*100ms，11*100ms
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x13,0x00,0x00,speed_up_time,speed_cut_time);
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x11,0x00,0x00,0x00,0x00); //设置速度为0，最高速1000rpm
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x11,0x00,0x00,0x00,0x00);
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);  //使能电机
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);
    HAL_Delay(10);
    
}


/**********************************************************************************************************************
 * @brief   电机位置模式控制初始化
 * @param   uint8_t speed_up_time   ：加速时间，单位 ms
 *          uint8_t speed_cut_time  ：减速时间，单位 ms
 *          uint16_t max_speed_rpm  ：最大速度，单位 rpm
 *          uint8_t mode            ：模式选择，分相对位置运行和绝对位置运行两种模式
 * @retval  None
**********************************************************************************************************************/
void MotorPositionModeInit(uint8_t speed_up_time, uint8_t speed_cut_time, uint16_t max_speed_rpm, uint8_t mode)
{
    
    uint8_t max_speed_1, max_speed_2;
    uint16_t max_speed;
    
    max_speed = 8192*max_speed_rpm/3000;
    max_speed_1 = max_speed>>8;
    max_speed_2 = max_speed;
    
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x3F); //位置模式
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x12,0x00,0x00,speed_up_time,speed_cut_time); //设置加减速时间，加减速时间都为10*100ms
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x14,0x00,0x00,max_speed_1,max_speed_2);  //设置电机运行的最高速,最高速度限制为1000rpm
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x17,0x00,0x00,0x00,mode);  //位置模式控制字,设为相对位置运行
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);  //使能电机
    HAL_Delay(10);
    
}


/**********************************************************************************************************************
 * @brief   速度模式控制下，底盘两个电机的速度设置
 * @param   int16_t speed_rpm_l    ：左边电机的速度设置，单位 rpm
 *          int16_t speed_rpm_r    ：右边电机的速度设置，单位 rpm
 * @retval  None
**********************************************************************************************************************/
void set_speed(int16_t speed_rpm_l, int16_t speed_rpm_r)
{
    
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F); //速度模式
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F);
    HAL_Delay(5);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x13,0x00,0x00,0x0A,0x0A); //加减速为10*100ms，11*100ms
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x13,0x00,0x00,0x0A,0x0A);
    HAL_Delay(5);
    SetMotorSpeed(&hcan1,0x0A,speed_rpm_l);//设置速度
    SetMotorSpeed(&hcan1,0x0B,speed_rpm_r);
    HAL_Delay(5);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);  //使能电机
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);
//    HAL_Delay(5);
    
}


/**********************************************************************************************************************
 * @brief   底盘电机的参数设置
 * @param   CAN_Header *hcan   ：自定义的CAN收发结构体
 *          uint8_t can_id     ：电机的ID
 *          uint8_t Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8     ：电机参数设置的相关数据
 * @retval  None
**********************************************************************************************************************/
void set_motor_data_can1(CAN_HandleTypeDef *hcan, uint8_t can_id, uint8_t Data1,uint8_t Data2,uint8_t Data3,uint8_t Data4,uint8_t Data5,uint8_t Data6,uint8_t Data7,uint8_t Data8)
{
	CAN_TxHeaderTypeDef   tx_header;
	uint8_t               tx_data[8];
	tx_header.StdId = can_id;  //标准标识符
	tx_header.IDE = CAN_ID_STD;  //标准帧
	tx_header.RTR = CAN_RTR_DATA;  //为数据帧
	tx_header.DLC = 0x08;  //消息的数据长度为8个字节
	tx_data[0]=Data1; //第一个字节数据
  tx_data[1]=Data2; //第二个字节数据 
	tx_data[2]=Data3; //第三个字节数据
  tx_data[3]=Data4; //第四个字节数据
	tx_data[4]=Data5; //第五个字节数据
  tx_data[5]=Data6; //第六个字节数据 
	tx_data[6]=Data7; //第七个字节数据
  tx_data[7]=Data8; //第八个字节数据 
	
	HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);
	
}
