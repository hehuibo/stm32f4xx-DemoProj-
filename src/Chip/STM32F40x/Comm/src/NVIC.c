/*
 ============================================================================
 Name        : NVIC.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "NVIC.h"
   
void vNVICConfigure(IRQn_Type Irq, uint8_t nPriority){
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = Irq;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = nPriority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

