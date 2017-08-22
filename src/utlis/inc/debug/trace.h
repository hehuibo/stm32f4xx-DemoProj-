#ifndef _STD_DEBUGdbgTRACE_H
#define _STD_DEBUGdbgTRACE_H

#if defined (UART_TRACE) && defined (JLINK_RTT_TRACE)
#error "JLINK_RTT_TRACE and Serial Debugging can not be used at the same time"
#endif

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)

  void TRACE_Init(void);

  void TRACE(const char *pstr, ...);
  
  #if defined (FreeRTOS_Kernel)
  void TraceLock(void);
  void TraceUnLock(void);
  #define dbgTRACE(...)        do{\
                                    TraceLock(); \
                                    TRACE(__VA_ARGS__);\
                                    TraceUnLock(); \
                                }while(0)
  #else 
  #define dbgTRACE(...)         TRACE(__VA_ARGS__)
  #endif
                                  
  #define dbgTRACE_FUNCTION()	dbgTRACE("%s\n",__FUNCTION__)
  #define dbgTRACE_FILELINE()	dbgTRACE("File: %s\tLine: %d\n",__FILE__,__LINE__)
  #define dbgTRACE_TIME()	    dbgTRACE("Time:%s\n",__TIME__)
  #define dbgTRACE_FILE()	    dbgTRACE("File: %s\n",__FILE__)
  #define dbgTRACE_LINE()	    dbgTRACE("Line: %d\n",__LINE__)

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
