#include "printf_info.h"



void PrintfInfo(void)
{
    /*打印电机驱动返回的报文信息*/
    printf("\n\r %d, %d, %d, %d, %d, %d, %d, %d \n\r",CAN1_Header.rx_data [0],CAN1_Header.rx_data [1],CAN1_Header.rx_data [2],\
    CAN1_Header.rx_data [3],CAN1_Header.rx_data [4],CAN1_Header.rx_data [5],CAN1_Header.rx_data [6],CAN1_Header.rx_data [7]);
    
    
    
}

