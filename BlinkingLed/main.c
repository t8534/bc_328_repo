/*
 * Base Core Atmega328
 *
 * DHT22 driver.
 * Humidity/temperature sensor.
 *
 * Author : arek
 *
 * The Data Pin for DHT22: PD2
 * 
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "dbg.h"
#include "dht22isr.h"



int main(void)
{
	const char *msg_crlf= "\r\n";	
	const char *msg_dht22_checksum = "DHT22 checksum error";
	const char *msg_dht22_noresp = "DHT22 not respond";
	//uint32_t val = 0;

	DHT22_STATE_t state;
	DHT22_DATA_t sensor_data;


	DBG_PINS_INIT;
	DBG_UartInit();
	
	DHT22_Init();
	sei();
	
    while (1)
	{
		DBG_PIN0_TOGGLE;
		state = DHT22_StartReading();
        while (DHT_DATA_READY != state) 
        {
			if (DHT_ERROR_CHECKSUM == state)
			{
				DBG_UartPrintStr(msg_dht22_checksum);
				DBG_UartPrintStr(msg_crlf);
			}
			else if (DHT_ERROR_NOT_RESPOND == state)
			{
				DBG_UartPrintStr(msg_dht22_noresp);
				DBG_UartPrintStr(msg_crlf);
			}
			state = DHT22_CheckStatus(&sensor_data);
        } 

        // Do something with the data.
        // sensor_data.temperature_integral
        // sensor_data.temperature_decimal
        // sensor_data.humidity_integral
        // sensor_data.humidity_decimal
		
		_delay_ms(1000);
	}
	
	/*
	while(1)
	{
		PORTB |= (1<<PORTB0);  // LED
		DBG_PIN0_TOGGLE;
		DBG_PIN1_TOGGLE;
		DBG_PIN2_TOGGLE;
		
		_delay_ms(500);
		
		DBG_UartPrintStr(msg1);
		DBG_UartPrintDec(DBG_DecMsgBuff, DBG_DEC_MSG_BUFF_LEN, val);
		DBG_UartPrintStr(msg_crlf);
		
		PORTB &= ~(1<<PORTB0);
		_delay_ms(500);
		
		if (val < 99999999) val++;
	}
	*/
}
