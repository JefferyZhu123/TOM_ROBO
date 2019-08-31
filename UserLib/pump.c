#include "pump.h"

void pump_task()
{
	    if(remote_control.switch_left == Switch_Up)//ÎüÆø
		{

			HAL_GPIO_WritePin(Breather_in_GPIO_Port,Breather_in_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Breather_out_GPIO_Port,Breather_out_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(PUMP_GPIO_Port,PUMP_Pin,GPIO_PIN_SET);
		}
		else	if(remote_control.switch_left == Switch_Middle)//Æô¶¯
		{
			HAL_GPIO_WritePin(PUMP_GPIO_Port,PUMP_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Breather_in_GPIO_Port,Breather_in_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Breather_out_GPIO_Port,Breather_out_Pin,GPIO_PIN_RESET);
		}
		else	if(remote_control.switch_left == Switch_Down)//³äÆø
		{
			HAL_GPIO_WritePin(Breather_in_GPIO_Port,Breather_in_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Breather_out_GPIO_Port,Breather_out_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PUMP_GPIO_Port,PUMP_Pin,GPIO_PIN_SET);
		}
}
