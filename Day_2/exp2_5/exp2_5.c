/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp2_5.c
* Description : Embedded C Program to turn ON the lamp connected via RELAY(GPIO2)
* Author      : Harish R EE20B044
***************************************************/
#include "platform.h"
#include "utils.h"
#include "gpio.h"

void main()
{
	write_word(GPIO_DIRECTION_CNTRL_REG, 0x00);
	write_word(GPIO_DATA_REG, 0x00);
	while(1);
}

