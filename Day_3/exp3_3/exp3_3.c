/*************************************************
* Project     : Shakti Embedded Programming Workshop
* Title       : Exp3_3.c
* Description : Embedded C Program to RUN the motor clockwise with  GPIO3 configured as PWM
* Author      : Harish R EE20B044
***************************************************/

#include "pwm_driver.h"
#include "platform.h"
#include "gpio.h"
#include "utils.h"
#include "pinmux.h"

void main()
{
	*pinmux_config_reg = 0x80;
	pwm_configure(PWM_0, 0xffff, 0xff, 0x0f, false);
	pwm_start(PWM_0,0);

	write_word(GPIO_DIRECTION_CNTRL_REG, 0x1C);
        write_word(GPIO_DATA_REG, 0x04);
 
	while(1);
}
