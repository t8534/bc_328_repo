/*
 * Base Core Atmega328
 *
 * DHT22 driver.
 * Humidity/temperature sensor.
 *
 * Author : arek
 *
 * 
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "dbg.h"



int main(void)
{
	const char *msg1 = "The value of val = ";	
	const char *msg_crlf= "\r\n";	
	uint32_t val = 0;

	DBG_PINS_INIT;
	DBG_UartInit()	;
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
	
}
