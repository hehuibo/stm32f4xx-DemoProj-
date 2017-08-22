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
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "stm32f4xx.h"

#define MB_COMM_TIMER                    TIM2
#define MB_COMM_TIMERCLK                 RCC_APB1Periph_TIM2 
#define MB_COMM_TIMER_IRQn               TIM2_IRQn

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(MB_COMM_TIMERCLK, ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = usTim1Timerout50us;
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)((SystemCoreClock / 20000) - 1);
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(MB_COMM_TIMER, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(MB_COMM_TIMER, ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = MB_COMM_TIMER_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ClearITPendingBit(MB_COMM_TIMER, TIM_IT_Update);
  TIM_ITConfig(MB_COMM_TIMER, TIM_IT_Update, DISABLE);
  TIM_Cmd(MB_COMM_TIMER, DISABLE);
  
  return TRUE;
}


void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    TIM_ClearITPendingBit(MB_COMM_TIMER, TIM_IT_Update);
	TIM_ITConfig(MB_COMM_TIMER, TIM_IT_Update, ENABLE);
	TIM_SetCounter(MB_COMM_TIMER, 0);
	TIM_Cmd(MB_COMM_TIMER, ENABLE);
}

void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
    TIM_ClearITPendingBit(MB_COMM_TIMER, TIM_IT_Update);
	TIM_ITConfig(MB_COMM_TIMER, TIM_IT_Update, DISABLE);
	TIM_SetCounter(MB_COMM_TIMER, 0);
	TIM_Cmd(MB_COMM_TIMER, DISABLE);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(MB_COMM_TIMER, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(MB_COMM_TIMER, TIM_IT_Update);
		prvvTIMERExpiredISR();
	}
}

