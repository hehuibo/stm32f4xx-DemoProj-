/*
 ============================================================================
 Name        : GPIO.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "stm32f4xx.h"
#include "GPIO.h"

void vGPIO_Configure(void){
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG,ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
  GPIO_Init(GPIOE,&GPIO_InitStructure);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
  GPIO_Init(GPIOG,&GPIO_InitStructure);
  
  GPIO_SetBits(GPIOE, GPIO_Pin_2);
  GPIO_SetBits(GPIOG, GPIO_Pin_12);
}


