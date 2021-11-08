/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp2_3.c
* Description : Embedded C Program to turn ON leds 1-4 (GPIO 16-19)
* Author      : Harish R EE20B044
***************************************************/

#include "platform.h"
#include "gpio.h"
#include "utils.h"

void main()
{
	write_word(GPIO_DIRECTION_CNTRL_REG, 0xF0000);
	write_word(GPIO_DATA_REG, 0xF0000);
	while(1);
}

