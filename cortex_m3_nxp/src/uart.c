/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	uart: Simple UART echo for LPCXpresso 1700
 * File:	uart.c
 * Description:
 * 			LPCXpresso Baseboard uses pins mapped to UART3 for
 * 			its USB-to-UART bridge. This application simply echos
 * 			all characters received.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

/*****************************************************************************
 *   History
 *
 *   2011.12.03  ver 1.10    Rob Jansen: Added FIFOs as receive buffer
 *   2010.07.01  ver 1.01    Added support for UART3, tested on LPCXpresso 1700
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC17xx.h"
#include "string.h"
#include "uart.h"

UARTFifo_t UART0Buffer, UART1Buffer, UART3Buffer;

static uint8_t fifo_put(UARTFifo_t *fifo, uint8_t c)
{
	int next;

	// check if FIFO has room
	next = (fifo->head + 1) % UART_BUFSIZE;
	if (next == fifo->tail) {
		// full
		return 0;
	}

	fifo->buffer[fifo->head] = c;
	fifo->head = next;

	return 1;
}

static uint8_t fifo_get(UARTFifo_t *fifo, uint8_t *pc)
{
	int next;

	// check if FIFO has data
	if (fifo->head == fifo->tail) {
		return 0;
	}

	next = (fifo->tail + 1) % UART_BUFSIZE;

	*pc = fifo->buffer[fifo->tail];
	fifo->tail = next;

	return 1;
}



/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART0_IRQHandler (void)
{
  uint8_t IIRValue;

  IIRValue = LPC_UART0->IIR;

  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
	fifo_put(&UART0Buffer, LPC_UART0->RBR);
  }
}

/*****************************************************************************
** Function name:		UART1_IRQHandler
**
** Descriptions:		UART1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART1_IRQHandler (void)
{
  uint8_t IIRValue;

  IIRValue = LPC_UART1->IIR;

  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
	fifo_put(&UART1Buffer, LPC_UART1->RBR);
  }
}
/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART3_IRQHandler (void)
{
  uint8_t IIRValue;

  IIRValue = LPC_UART3->IIR;

  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
	fifo_put(&UART3Buffer, LPC_UART3->RBR);
  }
}


/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART port, setup pin select,
**						clock, parity, stop bits, FIFO, etc.
**
** parameters:			portNum(0 or 1) and UART baudrate
** Returned value:		true or false, return false only if the
**						interrupt handler can't be installed to the
**						VIC table
**
*****************************************************************************/
uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate )
{
  uint32_t Fdiv;
  uint32_t pclkdiv, pclk;

  if ( PortNum == 0 )
  {
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;  /* RxD0 is P0.3 and TxD0 is P0.2 */
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 6~7 is for UART0 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemCoreClock/4;
		break;
	  case 0x01:
		pclk = SystemCoreClock;
		break;
	  case 0x02:
		pclk = SystemCoreClock/2;
		break;
	  case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

    LPC_UART0->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;
	LPC_UART0->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART0->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART0_IRQn);

    LPC_UART0->IER = IER_RBR;	/* Enable UART0 interrupt */
    return (1);
  }
  else if ( PortNum == 1 )
  {
	LPC_PINCON->PINSEL4 &= ~0x0000000F;
	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */

	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 8,9 are for UART1 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemCoreClock/4;
		break;
	  case 0x01:
		pclk = SystemCoreClock;
		break;
	  case 0x02:
		pclk = SystemCoreClock/2;
		break;
	  case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

    LPC_UART1->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
    LPC_UART1->DLM = Fdiv / 256;
    LPC_UART1->DLL = Fdiv % 256;
	LPC_UART1->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART1->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART1_IRQn);

    LPC_UART1->IER = IER_RBR;	/* Enable UART1 interrupt */
    return (1);
  }
  else if ( PortNum == 3 )
  {
	  LPC_PINCON->PINSEL0 &= ~0x0000000F;
	  LPC_PINCON->PINSEL0 |=  0x0000000A;  /* RxD3 is P0.1 and TxD3 is P0.0 */
	  LPC_SC->PCONP |= 1<<4 | 1<<25; //Enable PCUART1
	  /* By default, the PCLKSELx value is zero, thus, the PCLK for
		all the peripherals is 1/4 of the SystemFrequency. */
	  /* Bit 6~7 is for UART3 */
	  pclkdiv = (LPC_SC->PCLKSEL1 >> 18) & 0x03;
	  switch ( pclkdiv )
	  {
	  case 0x00:
	  default:
		  pclk = SystemCoreClock/4;
		  break;
	  case 0x01:
		  pclk = SystemCoreClock;
		  break;
	  case 0x02:
		  pclk = SystemCoreClock/2;
		  break;
	  case 0x03:
		  pclk = SystemCoreClock/8;
		  break;
	  }
	  LPC_UART3->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	  Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
	  LPC_UART3->DLM = Fdiv / 256;
	  LPC_UART3->DLL = Fdiv % 256;
	  LPC_UART3->LCR = 0x03;		/* DLAB = 0 */
	  LPC_UART3->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

	  NVIC_EnableIRQ(UART3_IRQn);

	  LPC_UART3->IER = IER_RBR;	/* Enable UART3 interrupt */
	  return (1);
  }
  return(0);
}

/*****************************************************************************
** Function name:		UARTBaudrate
**
** Descriptions:		Set the baud rate for the given port.
**                      This function needs to be called after changing
**                      the SystemCoreClock.
**
** parameters:			portNum(0 or 1) and UART baudrate
** Returned value:		none
**
*****************************************************************************/
void UARTBaudrate( uint32_t PortNum, uint32_t baudrate )
{
  uint32_t Fdiv;
  uint32_t pclkdiv, pclk;

  if ( PortNum == 0 )
  {
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemCoreClock/4;
		break;
	  case 0x01:
		pclk = SystemCoreClock;
		break;
	  case 0x02:
		pclk = SystemCoreClock/2;
		break;
	  case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;
  }
  else if ( PortNum == 1 )
  {
	pclkdiv = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemCoreClock/4;
		break;
	  case 0x01:
		pclk = SystemCoreClock;
		break;
	  case 0x02:
		pclk = SystemCoreClock/2;
		break;
	  case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

 	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
    LPC_UART1->DLM = Fdiv / 256;
    LPC_UART1->DLL = Fdiv % 256;
  }
  else if ( PortNum == 3 )
  {
	  pclkdiv = (LPC_SC->PCLKSEL1 >> 18) & 0x03;
	  switch ( pclkdiv )
	  {
	  case 0x00:
	  default:
		  pclk = SystemCoreClock/4;
		  break;
	  case 0x01:
		  pclk = SystemCoreClock;
		  break;
	  case 0x02:
		  pclk = SystemCoreClock/2;
		  break;
	  case 0x03:
		  pclk = SystemCoreClock/8;
		  break;
	  }
	  Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
	  LPC_UART3->DLM = Fdiv / 256;
	  LPC_UART3->DLL = Fdiv % 256;
  }
}

/*****************************************************************************
** Function name:		UARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**						on the data length
**
** parameters:			portNum, buffer pointer, and data length
** Returned value:		None
**
*****************************************************************************/
void UARTSend( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
  if ( portNum == 0 )
  {
    while ( Length != 0 )
    {
	  while (!(LPC_UART0->LSR & LSR_THRE));
	  LPC_UART0->THR = *BufferPtr;
	  BufferPtr++;
	  Length--;
	}
  }
  else if (portNum == 1)
  {
	while ( Length != 0 )
    {
	  while (!(LPC_UART1->LSR & LSR_THRE));
	  LPC_UART1->THR = *BufferPtr;
	  BufferPtr++;
	  Length--;
    }
  }
  else if ( portNum == 3 )
  {
    while ( Length != 0 )
    {
  	  while (!(LPC_UART3->LSR & LSR_THRE));
	  LPC_UART3->THR = *BufferPtr;
	  BufferPtr++;
	  Length--;
	}
  }
  return;
}

/*****************************************************************************
** Function name:		UARTPutStr
**
** Descriptions:		Send a block of data to the UART 0 port based
**						on the data length
**
** parameters:			portNum, buffer pointer, and data length
** Returned value:		None
**
*****************************************************************************/
void UARTSendStr( uint32_t portNum, char *BufferPtr )
{
	uint32_t Length;

	Length = strlen(BufferPtr);
	  if ( portNum == 0 )
	  {
	    while ( Length != 0 )
	    {
	  	  while (!(LPC_UART0->LSR & LSR_THRE));
		  if(*BufferPtr == '\n') LPC_UART0->THR = '\r';
		  LPC_UART0->THR = *BufferPtr;
		  BufferPtr++;
		  Length--;
		}
	  }
	  else if (portNum == 1)
	  {
		while ( Length != 0 )
	    {
		  while (!(LPC_UART1->LSR & LSR_THRE));
		  if(*BufferPtr == '\n') LPC_UART1->THR = '\r';
		  LPC_UART1->THR = *BufferPtr;
		  BufferPtr++;
		  Length--;
	    }
	  }
	  else if ( portNum == 3 )
	  {
	    while ( Length != 0 )
	    {
	  	  while (!(LPC_UART3->LSR & LSR_THRE));
		  if(*BufferPtr == '\n') LPC_UART3->THR = '\r';
		  LPC_UART3->THR = *BufferPtr;
		  BufferPtr++;
		  Length--;
		}
	  }
	  return;

}

/*
 * UARTPutChar(uint32_t portNum, char c)
 */

void UARTPutChar(uint32_t portNum, char c)
{
  if ( portNum == 0 )
  {
	  while (!(LPC_UART0->LSR & LSR_THRE));
	  LPC_UART0->THR = c;
  }
  else if (portNum == 1)
  {
	  while (!(LPC_UART1->LSR & LSR_THRE));
	  LPC_UART1->THR = c;
  }
  else if ( portNum == 3 )
  {
  	  while (!(LPC_UART3->LSR & LSR_THRE));
	  LPC_UART3->THR = c;
  }
  return;
}

/*****************************************************************************
** Function name:		UARTGetChar
**
** Descriptions:		Wait for a character from the given port.
**                      This functions blocks, waiting for the character
**
** parameters:			portNum
** Returned value:		Character received
**
*****************************************************************************/
char UARTGetChar( uint32_t portNum)
{
	UARTFifo_t *fifo;
	uint8_t c = '\0';

	switch(portNum)
	{
	case 0:
		fifo = &UART0Buffer;
		break;
	case 1:
		fifo = &UART1Buffer;
		break;
	case 3:
		fifo = &UART3Buffer;
		break;
	default:
		fifo = NULL;
		break;
	}
	if(fifo)
		while(!fifo_get(fifo, &c));

	return c;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
