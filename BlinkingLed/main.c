/*
 * BlinkingLed.c
 *
 * Created: 2016-12-07 19:57:25
 * Author : arek
 *
 * 
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
//#include <util/delay.h>
#include "usart_isr.h"



int main(void)
{
	
	
	// USART ISR example - OK
	// Set the baudrate to 9600 bps, external 8MHz oscillator.
	USART1_Init(UBRR_VAL);  
	sei();
	
	DDRB |= (1<<DDB0); // LED
	PORTB |= (1<<PORTB0);
	
	for( ; ; ) {
		// Echo the received character 
		USART1_Transmit(USART1_Receive());
	}	
	
	
	// USART pooling - OK
	// Set the baud rate: 9600 -> UBRRn = 51
	/*
	unsigned char ubrr_v = 51;
	unsigned char data = 66;  // B - 0x42
	UBRR0H = (unsigned char) (ubrr_v>>8);
	UBRR0L = (unsigned char) ubrr_v;
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
	UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));  // 9600 8 N 1 
	while(1)
	{
		PORTB |= (1<<PORTB0);
		_delay_ms(500);
		
		// Wait for empty transmit buffer 
		while (!(UCSR0A & (1<<UDRE0)));
	
		// Start transmission 
		UDR0 = data;
		
		PORTB &= ~(1<<PORTB0);
		_delay_ms(500);
	
	}
	*/
	
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
}
