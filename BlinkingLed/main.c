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
	const char *msg_temp_integ = "Temperature integral:";
	const char *msg_temp_deci = "Temperature decimal:";
	const char *msg_humi_integ = "Temperature integral:";
	const char *msg_humi_deci = "Temperature decimal:";
	
	
	uint32_t val = 0;

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

        DBG_UartPrintStr(msg_temp_integ);
		val = sensor_data.temperature_integral;
        DBG_UartPrintDec(DBG_DecMsgBuff, DBG_DEC_MSG_BUFF_LEN, val);
        DBG_UartPrintStr(msg_crlf);

        DBG_UartPrintStr(msg_temp_deci);
		val = sensor_data.temperature_decimal;
        DBG_UartPrintDec(DBG_DecMsgBuff, DBG_DEC_MSG_BUFF_LEN, val);
        DBG_UartPrintStr(msg_crlf);

        DBG_UartPrintStr(msg_humi_integ);
        val = sensor_data.humidity_integral;
		DBG_UartPrintDec(DBG_DecMsgBuff, DBG_DEC_MSG_BUFF_LEN, val);
        DBG_UartPrintStr(msg_crlf);

        DBG_UartPrintStr(msg_humi_deci);
		val = sensor_data.humidity_decimal;
        DBG_UartPrintDec(DBG_DecMsgBuff, DBG_DEC_MSG_BUFF_LEN, val);
        DBG_UartPrintStr(msg_crlf);

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
