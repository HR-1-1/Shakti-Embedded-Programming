/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp2_4.c
* Description : Embedded C Program to toggle leds1-4 with a delay
* Author      : Harish R EE20B044
***************************************************/

#include "platform.h"
#include "gpio.h"
#include "utils.h"


void main()
{
	write_word(GPIO_DIRECTION_CNTRL_REG, 0xF0000);
	while(1)
	{
		write_word(GPIO_DATA_REG, 0xF0000);
		delay_loop(1000, 5000);
		write_word(GPIO_DATA_REG, 0x00);
		delay_loop(100, 5000);
	}

}
