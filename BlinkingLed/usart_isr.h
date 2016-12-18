/*
 * usart_isr.h
 *
 * Created: 2016-12-17 14:11:26
 *  Author: arek
 */ 


#ifndef USART_ISR_H_
#define USART_ISR_H_


#include "avr/io.h"
#include "avr/interrupt.h"


#define UBRR_VAL	51
void USART1_Init(unsigned int ubrr_val);
unsigned char USART1_Receive(void);
void USART1_Transmit(unsigned char data);





#endif /* USART_ISR_H_ */