/*
 ============================================================================
 Name        : xx.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "USART3.h"
#include "stdio.h"
#include <stdarg.h>

/**************************USART1*******************************/

void vUSART3_Configure(uint32_t nBaudRate)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  
  //IO ¿ÚÓ³Éä
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
  
  //PA9 TX ,PA10 RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  //USART3 
  USART_DeInit(USART3);
  
  USART_InitStructure.USART_BaudRate = nBaudRate;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART3,&USART_InitStructure); 
  
  
  USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
  //USART1 NVIC 
  vNVICConfigure(USART3_IRQn, 5);
  
  USART_Cmd(USART3,ENABLE);
}


void USART3_IRQHandler(void)
{
 
  #if defined (_DEBUGTRACE)
  //dbgTRACE_FUNCTION();
  #endif 
  if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
  {
      g_AppCommBfrMnt.pRxBfr[g_AppCommBfrMnt.mCnt ++] = USART_ReceiveData(USART3);
      g_AppCommBfrMnt.mTimeOut = 2; 
      g_AppCommBfrMnt.mMsgID = eAppCommMsgID_USART3;
      if(g_AppCommBfrMnt.mCnt >= g_AppCommBfrMnt.mRxMax){
        FMS_SetOn(FMS.FMS_FLAG_ESPWIFI_RXED);
      #if defined (_DEBUGTRACE)
      dbgTRACE("\nRecvData;%s\n", g_AppCommBfrMnt.pRxBfr);
      #endif 
      }
  }
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}

void USART3_TxDat(unsigned char *pStr, unsigned short len)
{
  while(len--){
    while(RESET == USART_GetFlagStatus(USART3,USART_FLAG_TXE));
    USART_SendData(USART3,*pStr++);
  }
}

void Serial3_TxPrint(char* fmt, ...)
{
  va_list ap;
  
  va_start(ap, fmt);
  vsprintf((char*)g_AppCommBfrMnt.pTxBfr, fmt, ap);
  va_end(ap);
  
  for(int i=0; i<strlen((char const *)g_AppCommBfrMnt.pTxBfr); i++){
    while(RESET == USART_GetFlagStatus(USART3,USART_FLAG_TXE));
    USART_SendData(USART3,g_AppCommBfrMnt.pRxBfr[i]);
  }
}

