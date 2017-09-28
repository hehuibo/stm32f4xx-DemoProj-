#ifndef _STD_DEBUGdbgTRACE_H
#define _STD_DEBUGdbgTRACE_H


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
#include "USART1.h"
//#include "UART4.h"
#define     dbgTRACEUART   USART1 // UART4 //
#endif

#if defined (UART_TRACE) && defined (JLINK_RTT_TRACE)
#error "JLINK_RTT_TRACE and Serial Debugging can not be used at the same time"
#endif

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  void TRACE_Init(void);

  void TRACE(const char *pstr, ...);
  
  #if defined (IAR_TRACE)
  #include <stdio.h>
  #define dbgTRACE(...)                 printf(__VA_ARGS__)
  #else 
  #define dbgTRACE(...)                 TRACE(__VA_ARGS__)
  #endif
                                  
  #define dbgTRACE_FUNCTION()	        dbgTRACE("%s\n",__FUNCTION__)
  #define dbgTRACE_FILELINE()	        dbgTRACE("File: %s\tLine: %d\n",__FILE__,__LINE__)
  #define dbgTRACE_TIME()	        dbgTRACE("Time:%s\n",__TIME__)
  #define dbgTRACE_FILE()	        dbgTRACE("File: %s\n",__FILE__)
  #define dbgTRACE_LINE()	        dbgTRACE("Line: %d\n",__LINE__)

#else
  #define TRACE(...)
  #define dbgTRACE(...) 
  #define dbgTRACE_FUNCTION()	
  #define dbgTRACE_FILELINE()
  #define dbgTRACE_TIME()	
  #define dbgTRACE_FILE()	
  #define dbgTRACE_LINE()	

#endif


#endif //end debugtrace
