/****************************************************************************
 * Project           	      : IoT enabled DEVICE MONITORING APPLICATION
 * Name of the file			  : mini_proj_1.c
 * Brief Description of file  : reads real time calendar (date & time) over i2c interface.
 * Name of Author             : Harish R EE20B04
****************************************************************************/
/**
@file ds3231.c
@brief Contains the driver routines to configure and read DS3231.
@detail I2C based routines to configures and read the RTC information from DS3231.
*/

#include <stdint.h>//Includes the definitions of standard input/output functions//
#include "i2c.h"
#include "log.h"
#include "uart.h"
#include "string.h"
#include "pinmux.h"
#include "utils.h"
#include "xadc_driver.h"



#define UPDATE_TIME 1
#define I2C i2c_instance[1]
#define DS3231_SLAVE_ADDRESS 0XD0
#define DS3231_REG_OFFSET 0
#define DS3231_DEC_TO_HEX(decimal)  ( ( (decimal / 10 ) << 4) | (decimal % 10) )
#define DELAY_VALUE 900
#define PRESCALER_COUNT 0x1F
#define SCLK_COUNT 0x91
#define ESP_UART uart_instance[1]
#define API_KEY "NBBRXKEV0J3J60I9"


/**
 * @fn xadc_onchip_voltage(uint32_t value)
 * @brief Reads the onchip voltage
 * @details Read the voltage in the FPGA chip and returns a value in volts
 * @param unsigned int (32-bit) value
 * @return float
 */
float xadc_onchip_voltage(uint32_t value)
{
        return (((value >> 4)*3.0f)/4096.0f );
}


/**
 * @fn xadc_onchip_temp(uint32_t value)
 * @brief Reads the onchip temperature
 * @details Read the temperature on the FPGA chip and returns a value in
 * celsius.
 * @param unsigned int (32-bit) value
 * @return float
 */
float xadc_onchip_temp(uint32_t value)
{
        return (((value/65536.0f)/0.00198421639f ) - 273.15f);
}



/**
 * @fn uint32_t xadc_read_data(uint32_t *address)
 * @brief A api to read data from a memory mapped address.
 * @param unsigned int (32-bit) address
 * @return unsigned int (32-bit) *address
 */
uint32_t xadc_read_data(uint32_t *address)
{
        return(*address);
}



/** @fn int read_ds3231_registers(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int *readTemp, unsigned char length, unsigned long delay)
 * @brief Reads the date and time field of DS3231 registers.
 * @details Reads the date value (date, month and year) and time (hour, minutes and seconds) 
 * DS3231 registers.
 * @param i2c_struct*
 * @param reg_offset
 * @param *readTemp
 * @param length
 * @param delay
 * @return read status (Zero of success)
 */
int read_ds3231_registers(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int *readTemp, unsigned char length, unsigned long delay)
{
	unsigned char read_buf[4] = {'\0'};
	int i = 0, j = 0,  k = 0, status=0;
	unsigned char temp = 0;

//Writes the slave address for write
		i2c_send_slave_address(i2c_instance, DS3231_SLAVE_ADDRESS, I2C_WRITE, 800);
//Writes the pointer to address that needs to be read
		i2c_write_data(i2c_instance, reg_offset, delay);
//Stops the I2C transaction to start reading the temperature value.
		i2c_instance->control = I2C_STOP;


//Writes the slave address for read
		i2c_send_slave_address(i2c_instance, DS3231_SLAVE_ADDRESS, I2C_READ, 800);


/* Make a dummy read as per spec of the I2C controller */
		i2c_read_data(i2c_instance, &temp, delay);

//Reads the MSB Byte of temperature [D9 - D1]
		for(i = 0; i < length; i++)
		{
			i2c_read_data(i2c_instance, &temp, delay);
			*readTemp = temp;
//			printf("\n Read Value[%02d]: %02x", i, *readTemp);
			if( i == (length - 2) )
				i2c_instance->control = I2C_NACK;
			*readTemp++;
		}
		i2c_instance->control = I2C_STOP;
		return 0;
}

/** @fn int write_ds3231_registers(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int *write_value, unsigned char length, unsigned long delay)
 * @brief configures the DS3231 date and time format.
 * @details Configures the DS3231 Configuration registers with the required 
 * Date and time format over I2C interface.
 * @param i2c instance
 * @param reg_offset
 * @param *write_value
 * @param length
 * @param delay
 * @return write status (Zero on SUCCESS.)
 */
int write_ds3231_registers(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int *write_value, unsigned char length, unsigned long delay)
{
	int i = 0, j = 0,  k = 0, status=0;
	unsigned int temp = 0;
		i2c_send_slave_address(i2c_instance, DS3231_SLAVE_ADDRESS, I2C_WRITE, delay);
		i2c_write_data(i2c_instance, reg_offset, delay);
		for(i = 0; i < length; i++)
	{
		i2c_write_data(i2c_instance,  ( *write_value++  & 0xff) /*LM75_TEMP_REG_OFFSET */, delay);
	}
//Stops the I2C transaction to start reading the temperature value.
		i2c_instance->control = I2C_STOP;
//		delay_loop(800, 800);


			return 0;
}

/** @fn int dayofweek(int d, int m, int y)
 * @brief Calculates day of the week.
 * @details Calculates the week day based on the passed date.
 * @param d
 * @param m
 * @param y
 * @return Day of the week (0 - 6)
 */
int dayofweek(int d, int m, int y)
{
    static int t[] = { 0, 3, 2, 5, 0, 3,
                       5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    return ( y + y / 4 - y / 100 +
             y / 400 + t[m - 1] + d) % 7;
}

/** @fn unsigned int ds3231_decimal_to_hex(unsigned int decimal)
 * @brief Decimal to hex conversion.
 * @details The configuration value is convertd to hex. which is converted from decimal.
 * @param decimal
 * @return Calculated hex value. 
 */
unsigned int ds3231_decimal_to_hex(unsigned int decimal)
{
	return( ((decimal / 10 ) << 4) | (decimal % 10) );
}


void print_timestamp(int read_buf[])
{
                        read_ds3231_registers(I2C, DS3231_REG_OFFSET, &read_buf[0], 7, 800);

                       	log_info("\n Date: %x-%x-20%x Day: ", read_buf[4], read_buf[5], read_buf[6]);

                       	switch(read_buf[3])
                        {
                                case 1:
                                                log_info( "   Sunday; ");
                                                break;

                                case 2:
                                                log_info( "   Monday; ");
                                                break;

                                case 3:
                                                log_info( "  Tuesday; ");
                                                break;

                               case 4:
                                                log_info( "Wednesday; ");
                                                break;

                                case 5:
                                                log_info( " Thursday; ");
                                                break;

                                case 6:
                                                log_info( "   Friday; ");
                                                break;

                                case 7:
                                                log_info( " Saturday; ");
                                                break;
                        }
                        log_info("Time: %x:%x:%x", read_buf[2], read_buf[1], read_buf[0]);
                        delay_loop(1000, 1000);

}


/**
  * @fn int write_to_esp8266(char *data)
  * @brief sends data to esp8266 using UART
  * @param data data that has to be sent to cloud
*/

int write_to_esp8266(char *data) {
        while (*data != '\0') {
                write_uart_character(ESP_UART, *data);
                data++;
        }
	write_uart_character(ESP_UART,'\r');
        write_uart_character(ESP_UART,'\n');

}

/**
  * @fn int write_enter_to_esp8266()
  * @brief sends carriage return and new line charector to esp8266
  * @detail sends carriage return and new line charector to esp8266
  *         this method is neeed to indicate end to data transmission
*/

int write_enter_to_esp8266(){
        write_uart_character(ESP_UART,'\r');
        write_uart_character(ESP_UART,'\n');
}

/**
  * @fn int read_from_esp8266(char *data)
  * @brief Reads data sent by esp8266
  * @details Reads data sent by esp8266 until one of the key word is found
  * @param data responses read from esp8266, mainly used for logging
*/
int read_from_esp8266(char *data) {
        int ch;
        char *str = data;
        char *test = data;
        for(int i=0;i<198;i++) {
                read_uart_character(ESP_UART,&ch);
                //printf("read from esp %c  \n",ch);
                *str = ch;
                str++; 
                *str = '\0';
               // printf("read  %c\n",ch);
                if(strstr(test,"OK") != NULL)  {
                        printf("read from esp8266 %s\n",test);
                        return;
                }
                if(strstr(test,"ERROR") != NULL) {
                        printf("read from esp8266 %s\n",test);
                        return;
                }
                if(strstr(test,">") != NULL) {
                        printf("read from esp8266 %s\n",test);
                        return;
                }
                if(strstr(test,"RECV") != NULL) {
                        printf("read from esp8266 %s\n",test);
                        return;
		}
                if(strstr(test,"IP") != NULL) {
                        printf("read from esp8266 %s\n",test);
                        return;
                }
        }
        *str = '\0';
        return 1;
}

/**
  * @fn void setup_esp8266 ()
  * @brief setup esp8266
  * @details set up esp8266 to send data
*/
void setup_esp8266(){
        char data[200];
        write_to_esp8266("AT");
        delay(1);
        read_from_esp8266(data);
	//write_to_esp8266("AT+CWLAP");
	//delay(1);
	//read_from_esp8266(data);

        printf(" set esp8266 as AP\n");
        write_to_esp8266("AT+CWMODE=1");
        delay(1);
        read_from_esp8266(data);
        printf(" Connect esp8266 to AP \n");
        write_to_esp8266("AT+CWJAP=\"NIELIT_IITM\",\"vajra64soc\"");
        delay(3);
        read_from_esp8266(data);

  	printf(" sending AT Echo off command to esp\n");
        write_to_esp8266("ATE0");
	delay(1);
        read_from_esp8266(data);
        printf(" write AT+CIPMUX\n");
        write_to_esp8266("AT+CIPMUX=0");
        delay(1);
        read_from_esp8266(data);


}
/**
  * @fn void transmit_data (int temperature )
  * @brief transmit temperature value to thingspeak.com using esp8266
  * @details Open http connection to thingspeak.com, using GET method send data and close the connection
  * @param temperature value that has to eb transmitted.
*/
void transmit_data(int temperature) {
/***************************************************
    AT – response OK
    AT+CWLAP – list nearby available WiFi networks
    AT+CIPMUX=0
    AT+CIPSTART="TCP","api.thingspeak.com",80
    AT+CIPSEND=75
    GET https://api.thingspeak.com/update?api_key=PPI6OVOI1A2LGXMZ&field1=45

**************************************************/


        char data[200];
        char sendData[76];
        sprintf(sendData,"GET https://api.thingspeak.com/update?api_key=%s&field1=%d",API_KEY,temperature);
        printf("\n send data %s\n",sendData);

                // Connect to Wifi using esp8266
		printf(" Open connection to thingspeak.com\n",data);
                flush_uart(ESP_UART);
                write_to_esp8266("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
                read_from_esp8266(data);
                //printf(" data from esp :%s\n",data);

                //delay_loop(2000,2000);
                printf(" write AT+CIPSEND\n");
                flush_uart(ESP_UART);
                write_to_esp8266("AT+CIPSEND=75");
                read_from_esp8266(data);
                //printf(" data from esp :%s\n",data);

                printf(" write Data\n");
                flush_uart(ESP_UART);
                write_to_esp8266(sendData);

                //flush_uart(ESP_UART);
                write_enter_to_esp8266();
                delay_loop(1000,1000);
                read_from_esp8266(data);
                printf(" data from esp :%s\n",data);

                // disconnect from link
                printf(" write AT+CIPCLOSE\n");
                flush_uart(ESP_UART);
                write_to_esp8266("AT+CIPCLOSE");
                read_from_esp8266(data);
                printf(" data from esp :%s\n",data);

}


/** @fn void main()
 * @brief Reads and prints the Date and Time information.
 * @details Configures the DS3231 and reads Date and time information and prints the 
 * in decimal
 * @detail sets the pinmux to select UART1 to communicate with esp8266
 *      Set desired baudrate, get temperature from lm75 
 *      transmit data to thingspeak.com

 */
void main()
{
		int timeout;
		unsigned int tempReadValue = 0;
		unsigned long delay = 1000;
		unsigned int write_buf[7] = {0x00}, read_buf[7] = {0x00};
		unsigned char length;
		unsigned int hour, minute, second, date, month, year;
	
		hour = 12;
		minute = 29;
		second = 30;
	
		date = 04;
		month = 10;
		year = 2021;
	
		write_buf[0] = DS3231_DEC_TO_HEX(second); //Seconds
		write_buf[1] = DS3231_DEC_TO_HEX(minute); //Minutes
		write_buf[2] = DS3231_DEC_TO_HEX(hour); //Hours
	
		write_buf[4] = DS3231_DEC_TO_HEX(date); //Seconds
		write_buf[5] = DS3231_DEC_TO_HEX(month); //Minutes
		write_buf[6] = DS3231_DEC_TO_HEX( (year % 100) ); //Hours
	
		write_buf[3] = dayofweek(date, month, year) + 1;
		length = 7;
	
//	set_baud_rate(uart_instance[0], 115200);
	printf("\nHello Welcome to Shakti");

	i2c_init();

	if(config_i2c(I2C, PRESCALER_COUNT, SCLK_COUNT))
	{
		log_error("\tSomething Wrong In Initialization\n");
		return -1;
	}
	else
		log_info("\tIntilization Happened Fine\n");
#ifdef UPDATE_TIME
	write_ds3231_registers(I2C, 0x00, &write_buf[0], length, delay);
			printf("\n Write complete");
#endif
	for(int x=0; x<5; x++)
	{
		print_timestamp(read_buf);
	}

	int baudrate = 9600;
        int temperature = 24;
        int transmit_count = 0;
        char data[200];

	printf("\n setting PIN MUX config to 2 .... \n");
        *pinmux_config_reg = 0x5;
        set_baud_rate(ESP_UART,baudrate);       
        printf("\n Sending temperature data to cloud server!....\n");
        printf("\n Baud rate set to %d \n",baudrate);
        printf("\n Waiting to ESP8266 to initialize \n");
        delay_loop(1000,1000);
        setup_esp8266();

      
/*
	for(int m=0; m<10; m++)
{
	uint32_t  value = xadc_read_data(0x41200);

        printf("value = %f\n", xadc_onchip_temp(value));

	value = xadc_read_data(0x41204);
       printf("value = %f\n", xadc_onchip_voltage(value));
}
	//transmit_data((int)xadc_onchip_temp(value));
	//transmit_data((int)xadc_onchip_voltage(value));
*/

	for(int m=0; m<10; m++)
	{
		transmit_data(m);
	}


}

