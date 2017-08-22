/*
 ============================================================================
 Name        : CAN.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "CAN.h"
#include "NVIC.h"

static void CAN_GPIOConfigure(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
}

static void CAN_ModeFilterConfigure(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  CAN_InitStructure.CAN_TTCM = DISABLE; //时间触发通信模式
  CAN_InitStructure.CAN_ABOM = ENABLE;  //自动离线
  CAN_InitStructure.CAN_AWUM = ENABLE;  //自动唤醒
  CAN_InitStructure.CAN_NART = DISABLE; //报文自动重传
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE; //发送FIFO
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW  = CAN_SJW_2tq;
  CAN_InitStructure.CAN_BS1  = CAN_BS1_5tq;
  CAN_InitStructure.CAN_BS2  = CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler = 5;
  CAN_Init(CAN1, &CAN_InitStructure);
  
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//掩码模式
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = ((((u32)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;
  CAN_FilterInitStructure.CAN_FilterIdLow  = (((u32)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation   = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void CAN_Configure(void)
{
  CAN_GPIOConfigure();
  
  CAN_ModeFilterConfigure();
  
  vNVICConfigure(CAN1_RX1_IRQn, 11);
}

#if 0
void CAN_SetMsg(CanTxMsg *TxMessage)
{	  
	uint8_t ubCounter = 0;

  //TxMessage.StdId=0x00;						 
  TxMessage->ExtId=0x1314;					
  TxMessage->IDE=CAN_ID_EXT;				
  TxMessage->RTR=CAN_RTR_DATA;				
  TxMessage->DLC=8;							 
	
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    TxMessage->Data[ubCounter] = ubCounter;
  }
  CAN_Transmit(CAN1, &TxMessage);
}
#else
void CAN_SetMsg(void)
{	  
  uint8_t ubCounter = 0;
  CanTxMsg TxMessage;
  //TxMessage.StdId=0x00;						 
  TxMessage.ExtId=0x1314;					
  TxMessage.IDE=CAN_ID_EXT;				
  TxMessage.RTR=CAN_RTR_DATA;				
  TxMessage.DLC=8;							 
	
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    TxMessage.Data[ubCounter] = ubCounter;
  }
  
  CAN_Transmit(CAN1, &TxMessage);
}
#endif

void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMsg;
  CAN_Receive(CAN1, CAN_FIFO0, &RxMsg);
}
