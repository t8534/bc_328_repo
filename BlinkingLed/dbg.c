/*
 * dbg.c
 *
 *  Author: arek
 *
 *
 * TODO:
 * 1.
 * Predefined string messages in a flash.
 *
 */ 

#include <avr/io.h>
#include "dbg.h"


static void PutChar(const char *s);


void DBG_UartInit()
{
	// USART pooling
	// Set the baud rate: 9600 -> UBRRn = 51, ext 8MHz clock.
	uint8_t ubrr_v = 51;
	UBRR0H = (uint8_t) (ubrr_v>>8);
	UBRR0L = (uint8_t) ubrr_v;
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
	UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));  // 9600 8 N 1 	
}

void DBG_UartPrintStr(const char *msg)
{
  const char *str = msg;
  while (*str)
  {
	  PutChar(str);
	  str++;
  }	
	
}


void DBG_UartPrintDec(uint8_t* str, uint8_t len, uint32_t val)
{
	uint8_t i;

	for (i = 1; i <= len; i++)
	{
		str[len-i] = (uint8_t) ((val % 10UL) + '0');
		val/=10;
	}

	str[i-1] = '\0';
	
	DBG_UartPrintStr((const char *)str);
}


static void PutChar(const char *s)
{
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = *s;	
}


	// Blinky Led example
	/*
	DDRB |= (1<<DDB0); //Set the 6th bit on PORTB (i.e. PB5) to 1 => output
	while(1)
	{
		PORTB |= (1<<PORTB0);    //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
		_delay_ms(1000);         //Delay for 1000ms => 1 sec
		PORTB &= ~(1<<PORTB0);   //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
		_delay_ms(1000);         //Delay for 1000ms => 1 sec
	}
	*/
