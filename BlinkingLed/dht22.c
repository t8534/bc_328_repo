/*
 * dht22.c
 * 
 * No isr version
 *
 * Created: 2016-12-29 22:38:40
 *  Author: arek
 */ 


#if 0

#include "dht22.h"



    /* 
     * DHT22.c
     * clock is at 8 MHz
     * sv1eed
     */


    #define set_low(pin)		PORTD &= ~(1<<pin)
    #define set_output(pin)		DDRD |= (1<<pin)
    #define set_input(pin)		DDRD &= ~(1<<pin)
    #define is_high(pin)		(PIND & (1<<pin))


    uint8_t DHT22_read(uint8_t sensor, _DHT22_data* data)
    {
    	uint8_t inbyte;
    	uint8_t buf[5];
    	uint8_t to_cnt;

    	const uint8_t pins[3] = {5, 6, 7};	// SENSORS	// WARNING: PORTD pins only !!!

    	if (sensor > 2)
    		return 1;		// bad cfg or mem corruption
    		
    	uint8_t pin = pins[sensor];
    	
    	data->status = 0;	
    	
    	// the line should be high (open collector with pullup)
    // 	if (!is_high(pin))
    // 		return 2;
    	
    	set_low(pin);		// set low before going output (open collector line safety)
    	
    	set_output(pin);	// pull low
    	_delay_us(600);		// datasheet: at least 500us
    	set_input(pin);		// release
    		
    	_delay_us(70);		// go to the middle of the expected low (datasheet: 20-40 high, 80 low, ara read on 30+80/2)
    	if(is_high(pin))
    		return 2;		// no sensor
    	
    	_delay_us(80);		// go to the middle of the expected high
    	if(!is_high(pin))
    		return 3;		// bad sensor or noise?
    	
    	for (uint8_t b = 0; b < 5; b++)		// 5 bytes
    	{
    		inbyte = 0;

    		for (uint8_t i = 0; i < 8; i++)	// 8 bits
    		{
    			to_cnt = 0;
    			while(is_high(pin))
    			{
    				_delay_us(2);
    				if (to_cnt++ > 25)
    					return 4;
    			}
    			_delay_us(5);				// falling edge should be fast enough, but just to be safe

    			to_cnt = 0;
    			while(!is_high(pin))
    			{
    				_delay_us(2);
    				if (to_cnt++ > 28)
    					return 5;
    			}

    			// this is the problematic rising edge
    			// datasheet: duration defines bit: 26-28us for 0, 70 for 1, so read at 30 + (70-30)/2 = 50
    			_delay_us(50);
    				
    			inbyte <<= 1;
    			if(is_high(pin))			// read bit
    				inbyte |= 1;
    		}
    		buf[b] = inbyte;
    	}
    		
    	uint8_t sum = buf[0] + buf[1] + buf[2] + buf[3];
    	if(buf[4] != sum)
    	{
    		debug_print("checksum error\n");
    			return 6;						// checksum error
    	}

    	uint16_t H = (buf[0] << 8) | buf[1];	// misleading datasheet terminology, no integral/decimal, just high and low bytes
    	data->H = H / 10;

    	int16_t T = ((buf[2] & 0b01111111) << 8) | buf[3];	// remove the sign bit and combine
    	data->T = T / 10;
    	data->Tdecimal = T % 10;

    	// check negative on the temperature high byte
    	if(buf[2] & 0b10000000)
    		data->T *= -1;
    				
    	data->status = 1;
    	
    	return 0;
    }

#endif