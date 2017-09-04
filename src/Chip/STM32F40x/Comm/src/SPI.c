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

#if defined (FreeRTOS_Kernel)
static xSemaphoreHandle gs_xSpi1Mutex, gs_xSpi2Mutex;
#endif

void SPI1_Configure(void){
  SPI_InitTypeDef  SPI_InitStructure;
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

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler){
  SPI1->CR1&=0XFFC7;
  SPI1->CR1|=SPI_BaudRatePrescaler;	
  SPI_Cmd(SPI1,ENABLE); 
}

uint8_t SPI1_TxRxByte(uint8_t Dat){
  uint8_t t=0;				 	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){
    t++;
    if(t>250)return 0;
  }			  
  SPI_I2S_SendData(SPI1, Dat); 
  
  t=0;
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
    t++;
    if(t>250)return 0;
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

void SPI2_Configure(void){
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

void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler){
  SPI2->CR1&=0XFFC7;
  SPI2->CR1|=SPI_BaudRatePrescaler;	
  SPI_Cmd(SPI2,ENABLE); 
}

uint8_t SPI2_TxRxByte(uint8_t Dat){
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

