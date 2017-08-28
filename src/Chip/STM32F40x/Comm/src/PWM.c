/*
 ============================================================================
 Name        : PWM.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "stm32f4xx.h"
#include "NVIC.h"
#include "PWM.h"

void PWM_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_BDTRInitTypeDef  TIM_BDTRInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
  
  /*GPIO*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
  
  /*TIM8*/
  TIM_TimeBaseInitStructure.TIM_Period = 5000 - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler = 8400 - 1;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);
  
  /*PWM*/
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                 /*模式*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;     /*比较输出使能*/
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;   /*比较互补输出使能*/
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;        /*空闲时通道输出电平*/
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;    /*空闲时互补通道输出电平*/
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;        /*比较输出极性*/
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;      /*比较互补输出极性*/
  TIM_OC4Init(TIM8, &TIM_OCInitStructure);
  
  TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);/*使能ARR预装值*/
  
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 11;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
  
  TIM_Cmd(TIM8, ENABLE);
  
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
}
