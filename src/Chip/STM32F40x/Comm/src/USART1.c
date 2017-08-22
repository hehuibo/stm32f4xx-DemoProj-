/*
 ============================================================================
 Name        : USART1.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "USART1.h"
#include "TypesCfg.h"
/**************************USART1*******************************/

void vUSART1_Configure(u32 nBaudRate)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  
  //IO ¿ÚÓ³Éä
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
  
  //PA9 TX ,PA10 RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
  //UART 
  USART_InitStructure.USART_BaudRate = nBaudRate;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1,&USART_InitStructure);  
  USART_ITConfig(USART1,USART_IT_RXNE ,ENABLE);
  //USART1 NVIC 
  vNVICConfigure(USART1_IRQn,5);
  
  USART_Cmd(USART1,ENABLE);
}

#if defined (_LOADER)
void USART1_IRQHandler(void){
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
         
        g_AppCommBfrMnt.pRxBfr[g_AppCommBfrMnt.mCnt ++] = USART_ReceiveData(USART1);
        g_AppCommBfrMnt.mTimeOut = 20;
        g_AppCommBfrMnt.mMsgID = eAppCommMsgID_USART1;
        if(g_AppCommBfrMnt.mCnt >= g_AppCommBfrMnt.mRxMax){
            FSM_SetOn(g_FSM.FSM_FLAG_USART1RXED);
        }

    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
#endif


