#include "bsp_can.h"
#include "printf_info.h"


uint16_t can_cnt;

moto_measure_t moto_2006[5];
moto_measure_t moto_6020[1];
CAN_Header CAN1_Header;
CAN_Header CAN2_Header;


void can_filter_init(void)
{
	
	CAN_FilterTypeDef  CAN_FilterConfigStructure;

	//filter config for can1
	CAN_FilterConfigStructure.FilterBank = 1;                      // filter 1
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;  //ָ��������Ϊ��ʶ������λģʽ
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT; //������λ��Ϊ32λ
	CAN_FilterConfigStructure.FilterIdHigh = 0x0000;  //��������ʶ���ĸ�16λֵ
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;   //��������ʶ���ĵ�16λֵ
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;  //���������α�ʶ���ĸ�16λֵ
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;   //���������α�ʶ���ĵ�16λֵ
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_RX_FIFO0; //�趨��ָ���������FIFOΪ0
	CAN_FilterConfigStructure.FilterActivation = ENABLE;           //ʹ�ܹ�����
	CAN_FilterConfigStructure.SlaveStartFilterBank = 14;           //only meaningful in dual can mode
	
	HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterConfigStructure);	//init can filter
    HAL_CAN_Start(&hcan1); //start can1
    HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING); // enable can1 rx interrupt
    
    CAN_FilterConfigStructure.FilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterConfigStructure);
	HAL_CAN_Start(&hcan2); 
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
	
}



/*CAN�жϻص�����*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *_hcan)
{
    
    CAN_RxHeaderTypeDef   rx_header;
    uint8_t               rx_data[8];
    
    HAL_CAN_GetRxMessage(_hcan, CAN_RX_FIFO0, &rx_header, rx_data);  /*recive can data*/
    
//	if(_hcan->Instance == CAN1)
//	{
//		HAL_CAN_GetRxMessage(_hcan,CAN_RX_FIFO0,&(CAN1_Header.rx_header),CAN1_Header.rx_data); /*recive can data*/
//		
////        PrintfInfo();
////		can_cnt++;
//	}
    
    if(_hcan->Instance == CAN2)
    {
        
        switch(rx_header.StdId)
        {
            case CAN_2006Moto1_ID:
            case CAN_2006Moto2_ID:
            case CAN_2006Moto3_ID:
            case CAN_2006Moto4_ID:
            
            {
                static u8 i;
                i = rx_header.StdId - CAN_2006Moto1_ID;
                
                moto_2006[i].msg_cnt++ <= 50 ? get_moto_offset(&moto_2006[i], rx_data) : get_moto_measure(&moto_2006[i], rx_data);
                
                can_cnt++;
            }
            break;
						case CAN_2006Moto6_ID:
						{
                moto_2006[4].msg_cnt++ <= 50 ? get_moto_offset(&moto_2006[4], rx_data) : get_moto_measure(&moto_2006[4], rx_data);
            }
						break;
            case CAN_6020Moto6_ID:
            {
                moto_6020[0].msg_cnt++ <= 50 ? get_moto_offset(&moto_6020[0], rx_data) : get_moto_measure(&moto_6020[0], rx_data);
                
//                can_cnt++;
            }
            break;
         }
			 if(can_cnt == 500)
			{
				can_cnt = 0;
				HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
			}  
        
    }		
//	 if(_hcan->Instance == CAN1)
//	 {
//			 switch(rx_header.StdId)
//			 {
//				case CAN_6020Moto6_ID:
//				{
//						moto_6020[0].msg_cnt++ <= 50 ? get_moto_offset(&moto_6020[0], rx_data) : get_moto_measure(&moto_6020[0], rx_data);
//						
//						can_cnt++;
//				}
//				break;

//			  }
//			  if(can_cnt == 500)
//				{
//					can_cnt = 0;
//					HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
//				}   
//		}						
}



/**********************************************************************************************************************
 * @brief    ����3508���ͨ��CAN����������Ϣ��2006���Ҳ���ã�����2006���û���¶�ֵ����
 * @param	 moto_measure_t *ptr����������ṹ��
 *           uint8_t can_rx_data[]��CAN�������ݻ�����
 * @petval	 None
**********************************************************************************************************************/
void get_moto_measure(moto_measure_t *ptr, uint8_t can_rx_data[])
{

	ptr->last_angle = ptr->angle;
	ptr->angle = (uint16_t)(can_rx_data[0]<<8 | can_rx_data[1]);
	ptr->speed_rpm  = (int16_t)(can_rx_data[2]<<8 | can_rx_data[3]);
	ptr->real_current = (can_rx_data[4]<<8 | can_rx_data[5])*5.f/16384.f;

	ptr->hall = can_rx_data[6];
	
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
    
}


void get_moto_offset(moto_measure_t *ptr, uint8_t can_rx_data[])        
{
	ptr->angle = (uint16_t)(can_rx_data[0]<<8 | can_rx_data[1]) ;
	ptr->offset_angle = ptr->angle;
}


/**********************************************************************************************************************
 * @brief  send motor control message through can bus
 * @param  CAN_HandleTypeDef *hcan             CAN handle Structure
 *         s16 iq1, s16 iq2, s16 iq3, s16 iq4  �����ĸ�����ĵ���ֵ
 * @retval None
 * @others ��ʶ��0x200��Ӧǰ�ĸ�ID�ĵ������ʶ��0x1FF��Ӧ���ĸ�ID�ĵ����һ·CAN�����ԽӰ˸������
 *         �˺����Ƕ�Ӧǰ4��ID�ĵ��
**********************************************************************************************************************/
void MotorSetCurrent1(CAN_HandleTypeDef *hcan,s16 iq1, s16 iq2, s16 iq3, s16 iq4)
{
    
    CAN_TxHeaderTypeDef   tx_header;
    uint8_t               tx_data[8];

	tx_header.StdId = 0x200;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 0x08;
	tx_data[0] = (iq1 >> 8);
	tx_data[1] = iq1;
	tx_data[2] = (iq2 >> 8);
	tx_data[3] = iq2;
	tx_data[4] = (iq3 >> 8);
	tx_data[5] = iq3;
	tx_data[6] = (iq4 >> 8);
	tx_data[7] = iq4;
	
    HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);

}


//��е�۵�����ת�����6020��������õ�ѹֵ�����IDΪ6
void BaseRotateMotorSetVoltage(CAN_HandleTypeDef *hcan,s16 iq2)
{
    
    CAN_TxHeaderTypeDef   tx_header;
    uint8_t               tx_data[8];

	tx_header.StdId = 0x2FF;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 0x08;
  tx_data[0] = 0;
	tx_data[1] = 0;
	tx_data[2] = (iq2 >> 8);
	tx_data[3] = iq2;
  tx_data[4] = 0;
	tx_data[5] = 0;
	tx_data[6] = 0;
	tx_data[7] = 0;
	
    HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);
    
}


//��е�ۺ���2006��������IDΪ5
void RobotArmMotorSetCurrent2(CAN_HandleTypeDef *hcan,s16 iq1,s16 iq2)
{
    
    CAN_TxHeaderTypeDef   tx_header;
    uint8_t               tx_data[8];

	tx_header.StdId = 0x1FF;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 0x08;
	tx_data[0] = (iq1 >> 8);
	tx_data[1] = iq1;
	tx_data[2] = (iq2 >> 8);
	tx_data[1] = iq2;
	tx_data[4] = 0;
	tx_data[5] = 0;
	tx_data[6] = 0;
	tx_data[7] = 0;
	
    HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0);
    
}

