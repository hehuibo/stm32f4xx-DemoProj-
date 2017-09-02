/*
 ============================================================================
 Name        : SPI.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "SPI.h"

/**SPI1**/
#if defined (FreeRTOS_Kernel)
static xSemaphoreHandle gs_xSpi1Mutex, gs_xSpi2Mutex;
#endif

/*
  DMA2 
  Stream2 -- Channel 3  --->SPI1->Rx
  Stream3 --  Chanel 3  --->SPI1->Tx
*/
void SPI1_DMAConfigure(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
 
  //Tx
  DMA_DeInit(DMA2_Stream3);
  DMA_InitStructure.DMA_Channel = DMA_Channel_3;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI1->DR);/*Peripheral*/
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;/*Memory*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_Init(DMA2_Stream3, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream3, ENABLE);
  //Rx
  DMA_DeInit(DMA2_Stream2);
  DMA_InitStructure.DMA_Channel = DMA_Channel_3;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_Init(DMA2_Stream2, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream2, ENABLE);
}

static uint8_t DummyByte = 0xFF;

void SPI_DMATxData(void *buffer, int len)
{
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Rx, ENABLE);
   
  /*接收通道*/
  DMA2_Stream2->M0AR = (uint32_t)&DummyByte;
  DMA_SetCurrDataCounter(DMA2_Stream2, len);
  
  /*发送通道*/
  DMA2_Stream3->M0AR = (uint32_t)buffer;
  DMA_SetCurrDataCounter(DMA2_Stream3, len);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Rx, ENABLE);
  while(DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3) == RESET);
  
  DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Tx, DISABLE);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Rx, DISABLE);
}

SPI_InitTypeDef  SPI_InitStructure;
void SPI_DMARxData(void *buffer, int len)
{

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;
  SPI_Init(SPI1, &SPI_InitStructure);  
  /*发送通道*/
  DMA2_Stream3->M0AR = (uint32_t)&DummyByte;
  DMA_SetCurrDataCounter(DMA2_Stream3, len);
  
  /*接收通道*/ 
  DMA2_Stream2->M0AR = (uint32_t)buffer;
  DMA_SetCurrDataCounter(DMA2_Stream2, len);
  //SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Rx, ENABLE);
  while(DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == RESET);
  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);
  DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
  //SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Tx, DISABLE);
  SPI_I2S_DMACmd(SPI1, SPI_DMAReq_Rx, DISABLE);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_Init(SPI1, &SPI_InitStructure); 
}

void SPI1_Configure(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

#if defined (STM32F40_41xxx)
  SPI_DeInit(SPI1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 , ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource4, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5, GPIO_AF_SPI1); 
  GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
#else
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI1,  ENABLE );  
  #if defined (SPI1_PIN_REAMP)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  #else
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  #endif
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  #if defined (SPI1_PIN_REAMP)
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
  #else
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
  #endif
 
#endif  

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
  SPI_InitStructure.SPI_CRCPolynomial = 7;	
  SPI_Init(SPI1, &SPI_InitStructure);  
  SPI_Cmd(SPI1, ENABLE); 
	
#if defined (FreeRTOS_Kernel)
  gs_xSpi1Mutex = xSemaphoreCreateMutex();
#endif
    
}

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  SPI1->CR1&=0XFFC7;
  SPI1->CR1|=SPI_BaudRatePrescaler;	
  SPI_Cmd(SPI1,ENABLE); 
}

uint8_t SPI1_TxRxByte(uint8_t Dat)
{
  uint8_t t=0;				 	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){
    if(++t>250)return 0;
  }			  
  SPI_I2S_SendData(SPI1, Dat); 
  
  t=0;
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
    if(++t>250)return 0;
  }	  						    
  return SPI_I2S_ReceiveData(SPI1); 			    
}

#if defined (FreeRTOS_Kernel)
void SPI1_Lock(void){
  xSemaphoreTake(gs_xSpi1Mutex, portMAX_DELAY);
}
void SPI1_Unlock(void){
  xSemaphoreGive(gs_xSpi1Mutex);
}

void SPI2_Lock(void){
  xSemaphoreTake(gs_xSpi2Mutex, portMAX_DELAY);
}
void SPI2_Unlock(void){
  xSemaphoreGive(gs_xSpi2Mutex);
}

void SPI_Wait(void){
  vTaskDelay(1);
}	

#endif

/**SPI2**/
void SPI2_Configure(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;

  SPI_DeInit(SPI2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15, GPIO_AF_SPI2); 
  GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);


  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
  SPI_InitStructure.SPI_CRCPolynomial = 7;	  
  SPI_Init(SPI2, &SPI_InitStructure);  
  SPI_Cmd(SPI2, ENABLE); 
  
#if defined (FreeRTOS_Kernel)
	gs_xSpi2Mutex = xSemaphoreCreateMutex();
#endif  
}

void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  SPI2->CR1&=0XFFC7;
  SPI2->CR1|=SPI_BaudRatePrescaler;	
  SPI_Cmd(SPI2,ENABLE); 
}

uint8_t SPI2_TxRxByte(uint8_t Dat)
{
  uint8_t t=0;				 	
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){
    if(++t>250)
      return 0;
  }			  
  SPI_I2S_SendData(SPI2, Dat); 
  
  t=0;
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {
    if(++t>250)
      return 0;
  }	  						    
  return SPI_I2S_ReceiveData(SPI2); 			    
}

