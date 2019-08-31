#include "Chassis_Control.h"


int Flag = 0;
int32_t set_spd[2] = {0};
int32_t front_back = 0;
int32_t left_right = 0;
int32_t rotate = 0;
uint8_t Chassis_Ctrl;



/*������ٶ�ģʽ����*/
void WheelHubMotorTestSpeedMode(void)
{
    
    if(remote_control.switch_right == Switch_Middle)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
        
//        set_motor_data_can1(&CAN1_Header,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x0F);  //��ʹ�ܵ�����ѻ�״̬
        set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x30,0x00,0x00,0x00,0x1F);  //��ͣ
        
    }
    else if(remote_control.switch_right == Switch_Up)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
        
        SetMotorSpeed(&hcan1,0x0A,100); //�����ٶ�Ϊ100rpm�������1000rpm
        HAL_Delay(10);
        
    }
    else if(remote_control.switch_right == Switch_Down)
    {
        
        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
        
        SetMotorSpeed(&hcan1,0x0A,50);
        HAL_Delay(10);
        
    }
	
}



/*�����λ��ģʽ����*/
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
        
        set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x30,0x00,0x00,0x00,0x1F);  //��ͣ
        
    }
    
    
    
}


/*���̲��ٿ���*/
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
 * @brief   �ٶ�ģʽ�µ����ת�ٵ�����
 * @param   CAN_Header *hcan       ���Զ����CAN�շ��ṹ��
 *          uint8_t can_id         ��CAN���ͱ��ĵ�ID����Ӧ�����ID
 *          int16_t speed_rpm      ������ٶȣ���λ rpm
 * @retval  None
**********************************************************************************************************************/
void SetMotorSpeed(CAN_HandleTypeDef *hcan, uint8_t can_id, int16_t speed_rpm)
{
		CAN_TxHeaderTypeDef   tx_header;
	  uint8_t               tx_data[8];
    
    int16_t speed = 0;
    speed = 8192*speed_rpm/3000;
    
  tx_header.StdId = can_id;  //��׼��ʶ��
	tx_header.IDE = CAN_ID_STD;  //��׼֡
	tx_header.RTR = CAN_RTR_DATA;  //Ϊ����֡
	tx_header.DLC = 0x08;  //��Ϣ�����ݳ���Ϊ8���ֽ�
	tx_data[0]=0x00;       //�ӻ����
  tx_data[1]=0xDA;       //������ 
	tx_data[2]=0x00;       //�Ĵ�����ַ
  tx_data[3]=0x11;       //�Ĵ����ӵ�ַ
	tx_data[4]=0x00;       //����1��8λ
  tx_data[5]=0x00;       //����1��8λ
	tx_data[6]=(speed>>8); //����2��8λ
  tx_data[7]=speed;      //����2��8λ 
    
    HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);
    
}


/**********************************************************************************************************************
 * @brief   λ��ģʽ�����õ����λ��
 * @param   CAN_Header *hcan        ���Զ����CAN�շ��ṹ��
 *          uint8_t can_id          ��CAN���ͱ��ĵ�ID����Ӧ�����ID
 *          int32_t position        �������������Ӧ����ֵ
 * @retval  None
**********************************************************************************************************************/
void SetMotorPosition(CAN_Header *hcan, uint8_t can_id, int32_t position)
{
    
    int16_t pos_1,pos_2;
    pos_1 = position>>16;  //�õ���16λ
    pos_2 = position;   //�õ���16λ
    
    hcan->tx_header.StdId = can_id;  //��׼��ʶ��
	hcan->tx_header.IDE = CAN_ID_STD;  //��׼֡
	hcan->tx_header.RTR = CAN_RTR_DATA;  //Ϊ����֡
	hcan->tx_header.DLC = 0x08;  //��Ϣ�����ݳ���Ϊ8���ֽ�
    hcan->tx_data[0]=0x00;       //�ӻ����
    hcan->tx_data[1]=0xDA;       //������
    hcan->tx_data[2]=0x00;       //�Ĵ�����ַ
    hcan->tx_data[3]=0x16;       //�Ĵ����ӵ�ַ
    hcan->tx_data[4]=pos_1>>8;
    hcan->tx_data[5]=pos_1;
    hcan->tx_data[6]=pos_2>>8;
    hcan->tx_data[7]=pos_2;
    
    HAL_CAN_AddTxMessage(&hcan1, &(hcan->tx_header), hcan->tx_data,(uint32_t*)CAN_TX_MAILBOX0);
    
}


/**********************************************************************************************************************
 * @brief   �������̵���ٶ�ģʽ���Ƴ�ʼ��
 * @param   uint8_t speed_up_time   ������ʱ�䣬��λ ms
 *          uint8_t speed_cut_time  ������ʱ�䣬��λ ms
 * @retval  None
**********************************************************************************************************************/
void MotorSpeedModeInit(uint8_t speed_up_time, uint8_t speed_cut_time)
{
    
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F); //�ٶ�ģʽ
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F); //�ٶ�ģʽ
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x13,0x00,0x00,speed_up_time,speed_cut_time); //�Ӽ���Ϊ10*100ms��11*100ms
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x13,0x00,0x00,speed_up_time,speed_cut_time);
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x11,0x00,0x00,0x00,0x00); //�����ٶ�Ϊ0�������1000rpm
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x11,0x00,0x00,0x00,0x00);
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);  //ʹ�ܵ��
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);
    HAL_Delay(10);
    
}


/**********************************************************************************************************************
 * @brief   ���λ��ģʽ���Ƴ�ʼ��
 * @param   uint8_t speed_up_time   ������ʱ�䣬��λ ms
 *          uint8_t speed_cut_time  ������ʱ�䣬��λ ms
 *          uint16_t max_speed_rpm  ������ٶȣ���λ rpm
 *          uint8_t mode            ��ģʽѡ�񣬷����λ�����к;���λ����������ģʽ
 * @retval  None
**********************************************************************************************************************/
void MotorPositionModeInit(uint8_t speed_up_time, uint8_t speed_cut_time, uint16_t max_speed_rpm, uint8_t mode)
{
    
    uint8_t max_speed_1, max_speed_2;
    uint16_t max_speed;
    
    max_speed = 8192*max_speed_rpm/3000;
    max_speed_1 = max_speed>>8;
    max_speed_2 = max_speed;
    
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x3F); //λ��ģʽ
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x12,0x00,0x00,speed_up_time,speed_cut_time); //���üӼ���ʱ�䣬�Ӽ���ʱ�䶼Ϊ10*100ms
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x14,0x00,0x00,max_speed_1,max_speed_2);  //���õ�����е������,����ٶ�����Ϊ1000rpm
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x17,0x00,0x00,0x00,mode);  //λ��ģʽ������,��Ϊ���λ������
    HAL_Delay(10);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);  //ʹ�ܵ��
    HAL_Delay(10);
    
}


/**********************************************************************************************************************
 * @brief   �ٶ�ģʽ�����£���������������ٶ�����
 * @param   int16_t speed_rpm_l    ����ߵ�����ٶ����ã���λ rpm
 *          int16_t speed_rpm_r    ���ұߵ�����ٶ����ã���λ rpm
 * @retval  None
**********************************************************************************************************************/
void set_speed(int16_t speed_rpm_l, int16_t speed_rpm_r)
{
    
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F); //�ٶ�ģʽ
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x19,0x00,0x00,0x00,0x2F);
    HAL_Delay(5);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x13,0x00,0x00,0x0A,0x0A); //�Ӽ���Ϊ10*100ms��11*100ms
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x13,0x00,0x00,0x0A,0x0A);
    HAL_Delay(5);
    SetMotorSpeed(&hcan1,0x0A,speed_rpm_l);//�����ٶ�
    SetMotorSpeed(&hcan1,0x0B,speed_rpm_r);
    HAL_Delay(5);
    set_motor_data_can1(&hcan1,0x0A,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);  //ʹ�ܵ��
    set_motor_data_can1(&hcan1,0x0B,0x00,0xDA,0x00,0x10,0x00,0x00,0x00,0x1F);
//    HAL_Delay(5);
    
}


/**********************************************************************************************************************
 * @brief   ���̵���Ĳ�������
 * @param   CAN_Header *hcan   ���Զ����CAN�շ��ṹ��
 *          uint8_t can_id     �������ID
 *          uint8_t Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8     ������������õ��������
 * @retval  None
**********************************************************************************************************************/
void set_motor_data_can1(CAN_HandleTypeDef *hcan, uint8_t can_id, uint8_t Data1,uint8_t Data2,uint8_t Data3,uint8_t Data4,uint8_t Data5,uint8_t Data6,uint8_t Data7,uint8_t Data8)
{
	CAN_TxHeaderTypeDef   tx_header;
	uint8_t               tx_data[8];
	tx_header.StdId = can_id;  //��׼��ʶ��
	tx_header.IDE = CAN_ID_STD;  //��׼֡
	tx_header.RTR = CAN_RTR_DATA;  //Ϊ����֡
	tx_header.DLC = 0x08;  //��Ϣ�����ݳ���Ϊ8���ֽ�
	tx_data[0]=Data1; //��һ���ֽ�����
  tx_data[1]=Data2; //�ڶ����ֽ����� 
	tx_data[2]=Data3; //�������ֽ�����
  tx_data[3]=Data4; //���ĸ��ֽ�����
	tx_data[4]=Data5; //������ֽ�����
  tx_data[5]=Data6; //�������ֽ����� 
	tx_data[6]=Data7; //���߸��ֽ�����
  tx_data[7]=Data8; //�ڰ˸��ֽ����� 
	
	HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);
	
}
