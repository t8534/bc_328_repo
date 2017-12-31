/*
 * Base Core Atmega328
 *
 * Author : arek
 *
 * The first "old" oled approach:
 * Fonts.c
 * SSD1306.c
 * TWI_Master.c
 *
 * The "new" 171229 oled approach, taken from Arduino:
 *
 * https://electronics.stackexchange.com/questions/156930/stm32-understanding-gpio-settings
 *
 * Display:                                           Arduino Mini Pro
 *
 * OLED_RST    9 ------------------------------------------------- 9    PB1
 * OLED_DC     8 ------------------------------------------------- 8    PB0
 * OLED_CS    10 ------------------------------------------------- 10   PB2 (SS, PCINT2, OC1B)
 * SPI_MOSI   11  -  connect to the DIN pin of OLED - D11 - MOSI - 11   PB3 (MOSI)
 * SPI_SCK    13  -  connect to the CLK pin of OLED - D13 - SCK  - 13   PB5 (SCK) (LED is connected here)
 *
 * 
 * 
 */ 

// CHANGELOG:
//
// 1.[171230]
// Working OLED, however SPI should be CPOL=0 and CPHA=0, different than show in OLED datasheed
// (CPOL=1, CPHA=1).
// 
// Only display bitmap is tested. There is small garbage at the bottom. In arduino it is ok.
// Check.
//


// TODO:
//
// 1.[171231_1107]
// Only display bitmap is tested. There is small garbage at the bottom. In arduino it is ok.
// Clarify it.
//


// NOTES
//
// 1.
// SPI configration
// Arduino default:
// https://arduino.stackexchange.com/questions/16348/how-do-you-use-spi-on-an-arduino
//
// http://www.avrfreaks.net/forum/spi-example-c-code
//
// As I have interfaced a few different SPI devices I can recommend to use an oscilloscope
// to monitor the clock , data and cs lines. You'll have immediate info if the SPI device
// is alive and maybe responding. By the way the most trouble I had with SPI interfaced
// devices was caused by the CS being set/cleared at wrong times for the SPI slave.
// 
//
// Arduino default:
// - Mode 0 (the default) - clock is normally low (CPOL = 0) (SPI_POLARITY_HIGH - stm), and the data is sampled
//   on the transition from low to high (leading edge) (CPHA = 0) (SPI_PHASE_1EDGE - stm)
// - Data order: The default is most-significant bit first,
// - Speed: The default setting for SPI is to use the system clock speed divided by four,
//   that is, one SPI clock pulse every 250 ns, assuming a 16 MHz CPU clock.
//   You can change the clock divider by using setClockDivider like this:
//
//   SPI.setClockDivider (SPI_CLOCK_DIV128);
//
//   The fastest rate is "divide by 2" or one SPI clock pulse every 125 ns, assuming
//   a 16 MHz CPU clock. This would therefore take 8 * 125 ns or 1 탎 to transmit one byte.
//
//   However empirical testing shows that it is necessary to have two clock pulses between
//   bytes, so the maximum rate at which bytes can be clocked out is 1.125 탎 each
//   (with a clock divider of 2).
//   To summarize, each byte can be sent at a maximum rate of one per 1.125 탎
//   (with a 16 MHz clock) giving a theoretical maximum transfer rate of 1/1.125 탎,
//   or 888,888 bytes per second (excluding overhead like setting SS low and so on).
//





//#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

/*
#include "dbg.h"
#include "TWI_Master.h"
#include "Fonts.h"
#include "SSD1306.h"
*/

// oled "new" sh1106.h
#include "sh1106.h"
//test
#include "mcal_gpio.h"

#if 0

#define OLED_RST_PIN        GPIO_PIN_0
#define OLED_DC_PIN         GPIO_PIN_1
#define OLED_CS_PIN         GPIO_PIN_2
#define OLED_GPIO_PORT      GPIOC

#define OLED_RST_SET        HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_RST_PIN, GPIO_PIN_SET)
#define OLED_RST_RESET      HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_RST_PIN, GPIO_PIN_RESET)
#define OLED_DC_SET         HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_DC_PIN, GPIO_PIN_SET)
#define OLED_DC_RESET       HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_DC_PIN, GPIO_PIN_RESET)
#define OLED_CS_SET         HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_CS_PIN, GPIO_PIN_SET)
#define OLED_CS_RESET       HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_CS_PIN, GPIO_PIN_RESET)

#endif


int main(void)
{

/* Old, SSD1306.c, TWI_Master.c based.	
  // Init I2C	
  TWI_Master_Initialise();
  sei();	
	
  ssd1306_init();
	
	
    ssd1306_clear_screen(0xFF);
	_delay_ms(1000);
	ssd1306_clear_screen(0x00);
	ssd1306_display_string(18, 0, "1.3inch OLED", 16, 1);
	ssd1306_display_string(0, 16, "This is a demo for SSD1306/1106 OLED moudle!", 16, 1);
	ssd1306_refresh_gram();
	_delay_ms(1000);
	ssd1306_clear_screen(0x00);

	ssd1306_draw_bitmap(0, 2, &c_chSingal816[0], 16, 8);
	ssd1306_draw_bitmap(24, 2, &c_chBluetooth88[0], 8, 8);
	ssd1306_draw_bitmap(40, 2, &c_chMsg816[0], 16, 8);
	ssd1306_draw_bitmap(64, 2, &c_chGPRS88[0], 8, 8);
	ssd1306_draw_bitmap(90, 2, &c_chAlarm88[0], 8, 8);
	ssd1306_draw_bitmap(112, 2, &c_chBat816[0], 16, 8);

	ssd1306_draw_3216char(0,16, '2');
	ssd1306_draw_3216char(16,16, '3');
	ssd1306_draw_3216char(32,16, ':');
	ssd1306_draw_3216char(48,16, '5');
	ssd1306_draw_3216char(64,16, '6');
	ssd1306_draw_1616char(80,32, ':');
	ssd1306_draw_1616char(96,32, '4');
	ssd1306_draw_1616char(112,32, '7');
	ssd1306_draw_bitmap(87, 16, &c_chBmp4016[0], 40, 16);  
	
	ssd1306_display_string(0, 52, "MUSIC", 12, 0);
	ssd1306_display_string(52, 52, "MENU", 12, 0);
	ssd1306_display_string(98, 52, "PHONE", 12, 0);
	
	ssd1306_refresh_gram();
	while (1) {};	
*/	
	
	
/*	const char *msg1 = "The value of val = ";	
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
*/


    // oled "new"
	//todo move to better place
	uint8_t oled_buf[WIDTH * HEIGHT / 8];
	
	// display an image of bitmap matrix
	SH1106_begin();
	
	SH1106_clear(oled_buf);
	SH1106_bitmap(0, 0, Waveshare12864, 128, 64, oled_buf);
	SH1106_display(oled_buf);
	_delay_ms(2000);
	SH1106_clear(oled_buf);

#if 0
	//SH1106_clear(oled_buf);
	SH1106_char3216(0, 16, '1', oled_buf);
	SH1106_char3216(16, 16, '2', oled_buf);
	SH1106_char3216(32, 16, ':', oled_buf);
	SH1106_char3216(48, 16, '3', oled_buf);
	SH1106_char3216(64, 16, '4', oled_buf);
	SH1106_char3216(80, 16, ':', oled_buf);
	SH1106_char3216(96, 16, '5', oled_buf);
	SH1106_char3216(112, 16, '6', oled_buf);

	SH1106_display(oled_buf);
#endif

    // Test.
	// PD2 (arduino D2) is used as a LED indicator.
	DDRD = (1<<DDD2);
	while(1)
	{
		PORTD |= (1<<PORTD2);  
		_delay_ms(500);
		PORTD &= ~(1<<PORTD2);
		_delay_ms(500);
	}


	// Port PB5 isused by SPI and LED.
	/*
    DDRB = 0xFF; //Nakes PORTC as Output
	while(1)
	{
		//PORTB &= ~(1<<PORTB1);

		//PORTB = 0xFF; //Turns ON All LEDs
		PORTB |= (1<<PORTB5);  // On board Pro Mini LED
		_delay_ms(1000);
		//PORTB = 0x00;
		PORTB &= ~(1<<PORTB5);
		_delay_ms(1000);
		
	}
	*/
	
}
