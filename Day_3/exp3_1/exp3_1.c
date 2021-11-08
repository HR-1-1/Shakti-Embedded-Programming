/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp3_1.c
* Description : Embedded C Program to run the motor connected to Pins GPIO2-4 
		in Clockwise direction
* Author      : Harish R EE20B044
***************************************************/

#include "platform.h"
#include "gpio.h"
#include "utils.h"


void main()
{
	write_word(GPIO_DIRECTION_CNTRL_REG, 0x1C);
	write_word(GPIO_DATA_REG, 0x04);
	while(1)
	{
		write_word(GPIO_DATA_REG, (0x04)^(0x08));
		delay_loop(3000, 5000);
	}
}

