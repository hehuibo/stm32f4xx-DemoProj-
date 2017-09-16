/*
 ============================================================================
 Name        : xx.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "TypesCfg.h"
#include "Flash.h"
#include "fatmnt.h"

struct _tag_FatMntTYPE g_FatFsMnt;


void FatMntInit(void)
{
  g_FatFsMnt.mRes = f_mount(&g_FatFsMnt.mFs, "0:/", 1);
#if 0  
  if(FR_OK == g_FatFsMnt.mRes){
    g_FatFsMnt.mRes = f_open(&g_FatFsMnt.mFile,"0:/LN.dta",FA_OPEN_EXISTING | FA_READ);
    if(FR_OK == g_FatFsMnt.mRes){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      TRACE("f_open OK, filesize= %ld\n", g_FatFsMnt.mFile.fsize);
      #endif 
      f_close(&g_FatFsMnt.mFile);
    }else{
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      TRACE("f_openERR\n");
      #endif
    }
  }
#endif
}
