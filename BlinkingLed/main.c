/*
 * BlinkingLed.c
 *
 * Created: 2016-12-07 19:57:25
 * Author : arek
 */ 

//#define F_CPU 16000000UL
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
	DDRB |= (1<<DDB0); //Set the 6th bit on PORTB (i.e. PB5) to 1 => output
	while(1)
	{
		PORTB |= (1<<PORTB0);    //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
		_delay_ms(1000);        //Delay for 1000ms => 1 sec
		PORTB &= ~(1<<PORTB0);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
		_delay_ms(1000);        //Delay for 1000ms => 1 sec
	}
}
