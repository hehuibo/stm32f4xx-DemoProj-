/*
 ============================================================================
 Name        : trace.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */

/*根据芯片类型设置缓存的大小*/
#if defined (COTEX_CORE_M3) ||defined (COTEX_CORE_M4)
#define TRACE_BUFFER_SIZE	(1048)
#elif defined (COTEX_CORE_M0) || defined (COTEX_CORE_M0PLUS)
#define TRACE_BUFFER_SIZE	 256
#else
#define TRACE_BUFFER_SIZE	 512
#endif
   
/*串口打印配置*/
#if defined (UART_TRACE)
#include "stm32f4xx.h"
//#include "USART1.h"
#include "UART4.h"
#define     dbgTRACEUART    UART4 //USART1 //
#endif

#if defined (JLINK_RTT_TRACE)
#include "SEGGER_RTT.h"
#endif

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
#include "trace.h"
#include <stdio.h>
#include <stdarg.h>

#if defined (FreeRTOS_Kernel)
#include "Kernel.h"
xSemaphoreHandle xTraceMutex;

void TraceLock(void)
{
  xSemaphoreTake(xTraceMutex, portMAX_DELAY);
}

void TraceUnLock(void)
{
  xSemaphoreGive(xTraceMutex);
}
#endif

void TRACE_Init(void)
{
#if defined (FreeRTOS_Kernel)
  xTraceMutex = xSemaphoreCreateMutex();
#endif
  
#if defined (UART_TRACE)
  //vUSART1_Configure(115200);
  vUART4_Configure(115200);
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

