/**
 * \file
 *
 * \brief USART Interrupt example
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "usart_isr.h"



/* USART Buffer Defines */
#define USART_RX_BUFFER_SIZE 8     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_SIZE 8     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

#if (USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif
#if (USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif

/* Static Variables */
static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile unsigned char USART_RxHead;
static volatile unsigned char USART_RxTail;
static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile unsigned char USART_TxHead;
static volatile unsigned char USART_TxTail;

/*
int main(void)
{
	// Set the baudrate to 9600 bps using 8MHz internal RC oscillator 
	USART1_Init(UBRR_VAL);   

	#if __GNUC__
		sei();
	#else
		_SEI();
	#endif

	for( ; ; ) {
		// Echo the received character 
		USART1_Transmit(USART1_Receive());
	}
	
	return 0;
}
*/

/* Initialize USART */
void USART1_Init(unsigned int ubrr_val)
{
	unsigned char x;

	// Set the baud rate 
	// 9600 -> UBRRn = 51
	UBRR0H = (unsigned char) (ubrr_val>>8);
	UBRR0L = (unsigned char) ubrr_val;

	
	// Enable USART receiver and transmitter 
	//UCSR1B = ((1 << RXCIE) | (1 << RXEN) | (1 << TXEN)); 
	
	//todo: UDRE IRQ is not enabled ? But we use ISR !
	//      OK, this is enabled and disabled futher below, check itt.
	
	// RXCIE0
	// Writing this bit to one enables interrupt on the RXC0 Flag.
	// A USART Receive Complete interrupt will be generated only if the RXCIE0 bit is written
	// to one, the Global Interrupt Flag in SREG is written to one and the RXC0 bit in UCSR0A is set.
	//
    // UDRIE0: USART Data Register Empty Interrupt Enable 0
    // Writing this bit to one enables interrupt on the UDRE0 Flag. A Data Register Empty interrupt
	// will be generated only if the UDRIE0 bit is written to one, the Global Interrupt Flag in SREG
	// is written to one and the UDRE0 bit in UCSR0A is set.
	UCSR0B = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)); 
	
	
	/* For devices in which UBRRH/UCSRC shares the same location
	* eg; ATmega16, URSEL should be written to 1 when writing UCSRC
	* 
	*/
	/* Set frame format: 8 data 2stop */
	//UCSR1C = (1 << USBS) | (1 << UCSZ1) | (1 << UCSZ0);
	// 9600 8 N 1
	// UMSEL0[1:0] - 00 - asynchro
	// UPM0n: USART Parity Mode 0 n [n = 1:0] = 00 - Disabled
	// USBS0 stop bit = 0 - 1 bit
	//
	// UCSZ01 / UDORD0: USART Character Size / Data Order
	// UCSZ0[1:0]: USART Modes: The UCSZ0[1:0] bits combined with the UCSZ02 bit in UCSR0B sets the
	// number of data bits (Character Size) in a frame the Receiver and Transmitter use.
	//
	// UCSZ02 - 0
	// UCSZ01 - 1
	// UCSZ00 - 1
	//
	// todo: finally set all bits to 0 or 1 explicity 
	//
	//UCSR0C=0x00;  // improve, delete this and use AND below
	//UCSR0C=(0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)| (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
	//UCSR0B|=(0<<UCSZ02);  // todo: should be AND
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 9600 8 N 1 
	
	
	
	/* Flush receive buffer */
	x = 0; 			    

	USART_RxTail = x;
	USART_RxHead = x;
	USART_TxTail = x;
	USART_TxHead = x;
}


//todo: this vector flag irq is not disable and than enabled at the end of ?

ISR(USART_RX_vect)
{
	unsigned char data;
	unsigned char tmphead;

	// Read the received data 
	data = UDR0;                 
	// Calculate buffer index 
	tmphead = (USART_RxHead + 1) & USART_RX_BUFFER_MASK;
	// Store new index 
	USART_RxHead = tmphead;

	if (tmphead == USART_RxTail) {
		// ERROR! Receive buffer overflow 
	}
	// Store received data in buffer 
	USART_RxBuf[tmphead] = data; 
}



ISR(USART_UDRE_vect)
{
	unsigned char tmptail;

	// Check if all data is transmitted 
	if (USART_TxHead != USART_TxTail) {
		// Calculate buffer index 
		tmptail = (USART_TxTail + 1) & USART_TX_BUFFER_MASK;
		// Store new index 
		USART_TxTail = tmptail;      
		// Start transmission 
		UDR0 = USART_TxBuf[tmptail];  
	} else {       
		// Disable UDRE interrupt 
		UCSR0B &= ~(1<<UDRIE0);         
	}
}


unsigned char USART1_Receive(void)
{
	unsigned char tmptail;
	
	/* Wait for incoming data */
	while (USART_RxHead == USART_RxTail);
	/* Calculate buffer index */
	tmptail = (USART_RxTail + 1) & USART_RX_BUFFER_MASK;
	/* Store new index */
	USART_RxTail = tmptail;                
	/* Return data */
	return USART_RxBuf[tmptail];          
}

void USART1_Transmit(unsigned char data)
{
	unsigned char tmphead;
	
	/* Calculate buffer index */
	tmphead = (USART_TxHead + 1) & USART_TX_BUFFER_MASK;
	/* Wait for free space in buffer */
	while (tmphead == USART_TxTail);
	/* Store data in buffer */
	USART_TxBuf[tmphead] = data;
	/* Store new index */
	USART_TxHead = tmphead;               
	/* Enable UDRE interrupt */
	UCSR0B |= (1<<UDRIE0);                    
}


/*
void uart_put_slow(unsigned char *msg)
{
	register unsigned char c = 0;
	
	while(c = *s++)
	{
		uart_put(c);
		_delay_ms(20);
	}
}
*/