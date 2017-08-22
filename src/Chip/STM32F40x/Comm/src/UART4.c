/*
 ============================================================================
 Name        : UART4.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "UART4.h"
#include "TypesCfg.h"
/**************************USART1*******************************/

void vUART4_Configure(u32 nBaudRate)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
  
  //IO ¿ÚÓ³Éä
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
  
  //PA9 TX ,PA10 RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
  
  //UART 
  USART_InitStructure.USART_BaudRate = nBaudRate;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(UART4,&USART_InitStructure);  
  USART_ITConfig(UART4,USART_IT_RXNE ,ENABLE);
  //USART1 NVIC 
  vNVICConfigure(UART4_IRQn,5);
  
  USART_Cmd(UART4,ENABLE);
}

#if defined (_LOADER)
void UART4_IRQHandler(void){
    if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET){
         
        g_AppCommBfrMnt.pRxBfr[g_AppCommBfrMnt.mCnt ++] = USART_ReceiveData(UART4);
        g_AppCommBfrMnt.mTimeOut = 20;
        g_AppCommBfrMnt.mMsgID = eAppCommMsgID_UART4;
        if(g_AppCommBfrMnt.mCnt >= g_AppCommBfrMnt.mRxMax){
            FSM_SetOn(g_FSM.FSM_FLAG_UART4RXED);
        }

    }
    USART_ClearITPendingBit(UART4, USART_IT_RXNE);
}
#endif


