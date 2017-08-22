/*
 ============================================================================
 Name        : FMSBUFFER.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "TypesCfg.h"

uint8_t AppRxBuffer[APPDATABUF_MAX_LEN], mAppTxBuffer[APPDATABUF_MAX_LEN];

struct _tag_AppCommBufferTYPE    g_AppCommBfrMnt;

void AppCommBufferInit(void){
  memset(&g_AppCommBfrMnt, 0, sizeof(struct _tag_AppCommBufferTYPE));
  
  g_AppCommBfrMnt.pRxBfr = AppRxBuffer;
  g_AppCommBfrMnt.pTxBfr = mAppTxBuffer;
  g_AppCommBfrMnt.mRxMax = APPDATABUF_MAX_LEN;
  g_AppCommBfrMnt.mMsgID = eAppCommMsgID_NONE;
}

uint16_t getAppCommBufferMaxLen(void)
{
  return APPDATABUF_MAX_LEN;
}


