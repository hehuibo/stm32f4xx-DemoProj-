/*
 ============================================================================
 Name        : IR.c
 Author      : 
 Version     :
 Copyright   : 
 Description : PF7 TIM11 -CH1
 ============================================================================
 */
#include "stm32f4xx.h"
#include "NVIC.h"
#include "IR.h"


void vIR_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
  
  /*GPIO*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_TIM11);
  
  TIM_TimeBaseInitStructure.TIM_Period = 10000-1; //10ms
  TIM_TimeBaseInitStructure.TIM_Prescaler = 84-1;//1us
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseInitStructure);
  
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM11, &TIM_ICInitStructure);
  
  TIM_ITConfig(TIM11, TIM_IT_Update | TIM_IT_CC1, ENABLE);
  vNVICConfigure(TIM1_TRG_COM_TIM11_IRQn, 9);
  
  TIM_Cmd(TIM11, ENABLE);
}


void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM11, TIM_IT_Update) == SET){
  
  }
  
  if(TIM_GetITStatus(TIM11, TIM_IT_CC1) == SET){
  
  }
  
  TIM_ClearITPendingBit(TIM11, TIM_IT_Update | TIM_IT_CC1);
}