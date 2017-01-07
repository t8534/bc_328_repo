/*
 * Base Core Atmega328
 *
 * Author : arek
 *
 * 
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "dbg.h"


//#include <ioavr.h>
//#include <inavr.h>
#include "TWI_Master.h"

#define TWI_GEN_CALL         0x00  // The General Call address is 0

// Sample TWI transmission commands
#define TWI_CMD_MASTER_WRITE 0x10
#define TWI_CMD_MASTER_READ  0x20

// Sample TWI transmission states, used in the main application.
#define SEND_DATA             0x01
#define REQUEST_DATA          0x02
#define READ_DATA_FROM_BUFFER 0x03

unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{
                    // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
                    // and take appropriate actions.
                    // Se header file for a list of possible failures messages.
                    
                    // Here is a simple sample, where if received a NACK on the slave address,
                    // then a retransmission will be initiated.
 
  if ( (TWIerrorMsg == TWI_MTX_ADR_NACK) | (TWIerrorMsg == TWI_MRX_ADR_NACK) )
    TWI_Start_Transceiver();
    
  return TWIerrorMsg; 
}


int main( void )
{
  unsigned char messageBuf[4];
  //unsigned char TWI_targetSlaveAddress, temp, TWI_operation=0,
  //              pressedButton, myCounter=0;
  //unsigned char TWI_targetSlaveAddress = 0;


  // MLX90614				
  //double Tf;
  uint16_t Toreg;
  int xh, xl;
  
  // DBG messages
  const char *msg_tr_busy = "Transceiver busy";
  const char *msg_tr_not_ok_1 = "Last trans not ok (1) stopped";
  const char *msg_tr_not_ok_2 = "Last trans not ok (2) stopped";
  const char *msg_tr_not_ok_3 = "Last trans not ok (3) stopped";
  const char *msg_temperat_err = "Temperature related error";
  const char *msg_temperat_toreg = "Temperature Toreg = ";
  const char *msg_crlf= "\r\n";
  uint32_t dbgval = 0;
  
  
  // DBG init
  DBG_PINS_INIT;
  DBG_UartInit()	;


  // TWI init
  TWI_Master_Initialise();
  sei();  //__enable_interrupt();

  PORTB |= (1<<PORTB0);  // LED


  while(1)
  {
      DBG_PIN0_TOGGLE;

      // Get MLX90614 data 
      // Slave Address (SA) can be 0x00 for any MLX90614
      // using Read Word: SA(write) - Command - SA(read) - LSByte - MSByte - PEC
      //TWI_targetSlaveAddress   = 0x00;
      messageBuf[0] = 0x00;  // Slave Address (SA) can be 0x00 for any MLX90614
      messageBuf[1] = 0x07;  // RAM address 0x07, object temperature 1
      TWI_Start_Transceiver_With_Data(messageBuf, 2);  //SA + write to slave
      while (TWI_Transceiver_Busy()) 
	  {
        DBG_UartPrintStr(msg_tr_busy);
        DBG_UartPrintStr(msg_crlf);
      };
      if (!TWI_statusReg.lastTransOK)
      {
        DBG_UartPrintStr(msg_tr_not_ok_1);
        DBG_UartPrintStr(msg_crlf);
	    for(;;);
      }

      messageBuf[0] = 0x01;  // SA + read from slave bit
      TWI_Start_Transceiver_With_Data(messageBuf, 1);  
      while (TWI_Transceiver_Busy()) 
	  {
	    DBG_UartPrintStr(msg_tr_busy);
	    DBG_UartPrintStr(msg_crlf);
      };
      if (!TWI_statusReg.lastTransOK)
      {
	    DBG_UartPrintStr(msg_tr_not_ok_2);
	    DBG_UartPrintStr(msg_crlf);
	    for(;;);
      }

      messageBuf[0] = 0x00;
      messageBuf[1] = 0x00;
      TWI_Get_Data_From_Transceiver(messageBuf, 2);  // REad data: low byte, high byte
      while (TWI_Transceiver_Busy()) 
	  {
	    DBG_UartPrintStr(msg_tr_busy);
	    DBG_UartPrintStr(msg_crlf);
      };
      if (!TWI_statusReg.lastTransOK)
      {
	    DBG_UartPrintStr(msg_tr_not_ok_3);
	    DBG_UartPrintStr(msg_crlf);
	    for(;;);
      }

      xl = messageBuf[0];	//low byte  //todo check order
      xh = messageBuf[1];	//high byte

      Toreg = (xh << 8) | xl; //concatinate MSB and LSB
	
      if(Toreg & 0x8000) //if MSbit is 1 there is an error
      {
        DBG_UartPrintStr(msg_temperat_err);
        DBG_UartPrintStr(msg_crlf);
      }
	
      //Tk = Toreg / 200; //temp Kelvin
      //Tf = (((long)Toreg * (3.6)) - 45967)/100; //temp F

      dbgval = Toreg; 
      DBG_UartPrintStr(msg_temperat_toreg);
      DBG_UartPrintDec(DBG_DecMsgBuff, DBG_DEC_MSG_BUFF_LEN, dbgval);
      DBG_UartPrintStr(msg_crlf);

  }  // end of infinity while()

}





/*
  //LED feedback port - connect port B to the STK500 LEDS
  DDRB  = 0xFF;
  PORTB = myCounter;
  
  //Switch port - connect portD to the STK500 switches
  DDRD  = 0x00;

  TWI_Master_Initialise();
  sei();  //__enable_interrupt();
  
  TWI_targetSlaveAddress   = 0x10;

  // This example is made to work together with the AVR311 TWI Slave application note and stk500.
  // In adition to connecting the TWI pins, also connect PORTB to the LEDS and PORTD to the switches.
  // The code reads the pins to trigger the action you request. There is an example sending a general call,
  // address call with Master Read and Master Write. The first byte in the transmission is used to send
  // commands to the TWI slave.

  // This is a stk500 demo example. The buttons on PORTD are used to control different TWI operations.
  for(;;)
  {  
    pressedButton = ~PIND;
    if (pressedButton)       // Check if any button is pressed
    {
      do{temp = ~PIND;}      // Wait until key released
      while (temp);
      
      switch ( pressedButton ) 
      {
        // Send a Generall Call
        case (1<<PD0):      
          messageBuf[0] = TWI_GEN_CALL;     // The first byte must always consit of General Call code or the TWI slave address.
          messageBuf[1] = 0xAA;             // The command or data to be included in the general call.
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );
          break;

        // Send a Address Call, sending a command and data to the Slave          
        case (1<<PD1):      
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
          messageBuf[1] = TWI_CMD_MASTER_WRITE;             // The first byte is used for commands.
          messageBuf[2] = myCounter;                        // The second byte is used for the data.
          TWI_Start_Transceiver_With_Data( messageBuf, 3 );
          break;

        // Send a Address Call, sending a request, followed by a resceive          
        case (1<<PD2):      
          // Send the request-for-data command to the Slave
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
          messageBuf[1] = TWI_CMD_MASTER_READ;             // The first byte is used for commands.
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );

          TWI_operation = REQUEST_DATA;         // To release resources to other operations while waiting for the TWI to complete,
                                                // we set a operation mode and continue this command sequence in a "followup" 
                                                // section further down in the code.
                    
        // Get status from Transceiver and put it on PORTB
        case (1<<PD5):
          PORTB = TWI_Get_State_Info();
          break;

        // Increment myCounter and put it on PORTB          
        case (1<<PD6):      
          PORTB = ++myCounter;        
          break;
          
        // Reset myCounter and put it on PORTB
        case (1<<PD7):      
          PORTB = myCounter = 0;        
          break;  
      }
    }    

    if ( ! TWI_Transceiver_Busy() )
    {
    // Check if the last operation was successful
      if ( TWI_statusReg.lastTransOK )
      {
        if ( TWI_operation ) // Section for follow-up operations.
        {
        // Determine what action to take now
          if (TWI_operation == REQUEST_DATA)
          { // Request/collect the data from the Slave
            messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
            TWI_Start_Transceiver_With_Data( messageBuf, 2 );       
            TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation        
          }
          else if (TWI_operation == READ_DATA_FROM_BUFFER)
          { // Get the received data from the transceiver buffer
            TWI_Get_Data_From_Transceiver( messageBuf, 2 );
            PORTB = messageBuf[1];        // Store data on PORTB.
            TWI_operation = FALSE;        // Set next operation        
          }
        }
      }
      else // Got an error during the last transmission
      {
        // Use TWI status information to detemine cause of failure and take appropriate actions. 
        TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );
      }
    }

    // Do something else while waiting for TWI operation to complete and/or a switch to be pressed
    __no_operation(); // Put own code here.

  }
}
*/

/*
  // This example code runs forever; sends a byte to the slave, then requests a byte
  // from the slave and stores it on PORTB, and starts over again. Since it is interupt
  // driven one can do other operations while waiting for the transceiver to complete.
  
  // Send initial data to slave
  messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
  messageBuf[1] = 0x00;
  TWI_Start_Transceiver_With_Data( messageBuf, 2 );

  TWI_operation = REQUEST_DATA; // Set the next operation

  for (;;)
  {
    // Check if the TWI Transceiver has completed an operation.
    if ( ! TWI_Transceiver_Busy() )                              
    {
    // Check if the last operation was successful
      if ( TWI_statusReg.lastTransOK )
      {
      // Determine what action to take now
        if (TWI_operation == SEND_DATA)
        { // Send data to slave
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );
          
          TWI_operation = REQUEST_DATA; // Set next operation
        }
        else if (TWI_operation == REQUEST_DATA)
        { // Request data from slave
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );
          
          TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation        
        }
        else if (TWI_operation == READ_DATA_FROM_BUFFER)
        { // Get the received data from the transceiver buffer
          TWI_Get_Data_From_Transceiver( messageBuf, 2 );
          PORTB = messageBuf[1];        // Store data on PORTB.
          
          TWI_operation = SEND_DATA;    // Set next operation        
        }
      }
      else // Got an error during the last transmission
      {
        // Use TWI status information to detemine cause of failure and take appropriate actions. 
        TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );
      }
    }
    // Do something else while waiting for the TWI Transceiver to complete the current operation
    __no_operation(); // Put own code here.
  }
}
*/






/*
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
*/
