/*
 * dbg.h
 *
 *  Author: arek
 */ 


#ifndef DBG_H_
#define DBG_H_

// Debug pins C0, C1, C2, C3, C4, C5
#define DBG_PINS_INIT    \
do { \
	DDRC = 0b00000111; \
	PORTC = 0b00000000; \
} while (0)

#define DBG_PIN0_TOGGLE  PORTC ^=0b00000001
#define DBG_PIN1_TOGGLE  PORTC ^=0b00000010
#define DBG_PIN2_TOGGLE  PORTC ^=0b00000100
#define DBG_PIN3_TOGGLE  PORTC ^=0b00001000
#define DBG_PIN4_TOGGLE  PORTC ^=0b00010000
#define DBG_PIN5_TOGGLE  PORTC ^=0b00100000




#define DBG_DEC_MSG_BUFF_LEN    10UL
uint8_t DBG_DecMsgBuff[DBG_DEC_MSG_BUFF_LEN];

void DBG_UartInit();
void DBG_UartPrintStr(const char *str);
void DBG_UartPrintDec(uint8_t* str, uint8_t len, uint32_t val);


#endif /* DBG_H_ */