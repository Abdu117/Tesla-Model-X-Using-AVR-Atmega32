 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(u32 baud_rate)
{
	u16 ubrr_value = 0;

	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/ 
	UCSRB = (1<<RXEN) | (1<<TXEN);
	
	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = 00 Disable parity bit
	 * USBS    = 0 One stop bit
	 * UCSZ1:0 = 11 For 8-bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/ 	
	UCSRC = (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1); 
	
	/* Calculate the UBRR register value */
	ubrr_value = (u16)(((F_CPU / (baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const u8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
u8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
    return UDR;		
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const u8 *Str)
{
	u8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(u8 *Str)
{
	unsigned char i=0,J=0;

	do

	{

	*(Str+i)= UART_recieveByte();

	J = *(Str+i);

	i++;

	}

	while((J!='d') && (J!='p'));

	i++;

	*(Str+i) = '\0';

	/*u8 i = 0;

	Str[i] = UART_recieveByte();


	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	Str[i] = '\0';*/
}
