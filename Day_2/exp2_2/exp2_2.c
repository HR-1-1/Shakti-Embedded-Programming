/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp2_1.c
* Description : Embedded C Program to print using UART0
* Author      : Harish R EE20B044
***************************************************/

#include "platform.h"
#include "gpio.h"
#include "utils.h"

void main()
{
	write_word(GPIO_DIRECTION_CNTRL_REG,0x10000);
	write_word(GPIO_DATA_REG,0x10000);
	while(1);

}
