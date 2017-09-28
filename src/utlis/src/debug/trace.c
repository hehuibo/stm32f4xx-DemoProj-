/*
 ============================================================================
 Name        : trace.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */


#if defined (JLINK_RTT_TRACE)
#include "SEGGER_RTT.h"
#endif

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
#include "trace.h"
#include <stdio.h>
#include <stdarg.h>

void TRACE_Init(void)
{
#if defined (UART_TRACE)
  vUSART1_Configure(115200);
  //vUART4_Configure(115200);
#endif
}


void TRACE0(const char ch)
{
#if defined (JLINK_RTT_TRACE)
  SEGGER_RTT_Write(0,&ch, 1);
#elif defined (UART_TRACE)
  while(USART_GetFlagStatus(dbgTRACEUART, USART_FLAG_TC) == RESET){
  }
  USART_SendData(dbgTRACEUART,(char) ch);
#endif
}

static char trace_buffer[TRACE_BUFFER_SIZE]={0};
void TRACE(const char *pstr, ...)
{
  int i=0;
  va_list ptr;
  if(NULL == pstr)
  {
    return ;
  }
  trace_buffer[0] = 0;
  
  va_start(ptr, pstr);
  if(NULL != pstr)
  {
    vsprintf(trace_buffer, pstr, ptr);
    va_end(ptr);
    for(i=0; (0!=trace_buffer[i])&&(i<TRACE_BUFFER_SIZE);++i)
    {
      TRACE0(trace_buffer[i]);
    }
  }
}

#endif

