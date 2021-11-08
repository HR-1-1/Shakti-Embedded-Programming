/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp2_6.c
* Description : Embedded C Program to read the status digital mic connected to GPIO3 and display on Serial debug port
* Author      : Harish R EE20B044
***************************************************/

#include "platform.h"
#include "gpio.h"
#include "utils.h"


void main()
{
	unsigned long io;
	write_word(GPIO_DIRECTION_CNTRL_REG, 0x00);
	while(1)
	{
		io = read_word(GPIO_DATA_REG) & GPIO3;
		log_info("\n Reading gpio pins :%x", io);
		delay_loop(3000, 5000);
	}
}

