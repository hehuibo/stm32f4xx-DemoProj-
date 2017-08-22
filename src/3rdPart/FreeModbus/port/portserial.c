/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"

#include "stm32f4xx.h"

#define MB_COMM_UART                    USART1
#define MB_COMM_UARTCLK                 RCC_APB2Periph_USART1 
#define MB_COMM_UART_PORTAF             GPIO_AF_USART1
#define MB_COMM_UART_IRQn               USART1_IRQn

#define MB_COMM_UART_PORT               GPIOA
#define MB_COMM_UART_PORTCLK            RCC_AHB1Periph_GPIOC
#define MB_COMM_UART_PORT_TXRXPIN       (GPIO_Pin_10 | GPIO_Pin_0)
#define MB_COMM_UART_PORT_TXRXPINSOURCE (GPIO_PinSource10 | GPIO_PinSource9)

#define MB_COMM_UART_PORT_TXPINSOURCE   GPIO_PinSource10
#define MB_COMM_UART_PORT_RXPINSOURCE   GPIO_PinSource9
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
  /*接收使能*/
  if(xRxEnable){
    USART_ITConfig(MB_COMM_UART, USART_IT_RXNE, ENABLE);
  }else{
    USART_ITConfig(MB_COMM_UART, USART_IT_RXNE, DISABLE);
  }
  
  /*发送使能*/
  if(xTxEnable){
    USART_ITConfig(MB_COMM_UART, USART_IT_TXE, ENABLE);
  }else{
    USART_ITConfig(MB_COMM_UART, USART_IT_TXE, DISABLE);
  }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(MB_COMM_UART_PORTCLK,ENABLE);
  RCC_APB2PeriphClockCmd(MB_COMM_UARTCLK,ENABLE);
  
  (void)ucPORT;
  (void)eParity;
  (void)ucDataBits;
  
  
  //IO 口映射
#if 0
  GPIO_PinAFConfig(MB_COMM_UART_PORT,MB_COMM_UART_PORT_TXRXPINSOURCE, MB_COMM_UART_PORTAF);
#else
  GPIO_PinAFConfig(MB_COMM_UART_PORT,MB_COMM_UART_PORT_RXPINSOURCE, MB_COMM_UART_PORTAF);
  GPIO_PinAFConfig(MB_COMM_UART_PORT,MB_COMM_UART_PORT_TXPINSOURCE, MB_COMM_UART_PORTAF);
#endif 
  GPIO_InitStructure.GPIO_Pin = MB_COMM_UART_PORT_TXRXPIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(MB_COMM_UART_PORT,&GPIO_InitStructure);
  
  //UART 
  USART_InitStructure.USART_BaudRate = ulBaudRate;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(MB_COMM_UART,&USART_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = MB_COMM_UART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_Cmd(MB_COMM_UART, ENABLE);
  
  return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    USART_SendData(MB_COMM_UART, ucByte);
    while(USART_GetFlagStatus(MB_COMM_UART, USART_FLAG_TC) == RESET){
    }
  
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
  
    *pucByte = USART_ReceiveData(MB_COMM_UART);
  
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

void USART1_IRQHandler(void){
  if(USART_GetITStatus(MB_COMM_UART, USART_IT_RXNE) == SET){
    prvvUARTRxISR();
    USART_ClearITPendingBit(MB_COMM_UART, USART_IT_RXNE);
  }
  
  if(USART_GetITStatus(MB_COMM_UART, USART_IT_TXE) == SET){
    prvvUARTTxReadyISR();
    USART_ClearITPendingBit(MB_COMM_UART, USART_IT_TXE);
  } 
}
