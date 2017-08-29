/*
 ============================================================================
 Name        : TIMIC.c
 Author      : 
 Version     :
 Copyright   : 
 Description : TIM5, PA0
  输入捕获
 ============================================================================
 */
#include "stm32f4xx.h"
#include <stdbool.h>
#include <string.h>
#include "NVIC.h"
#include "TIMIC.h"

void vTIMIC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  
  /*GPIO*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
  
  /*
   f =  84MHz/84 = 1MHz
   T = 1/f --->10^6   1us  
  */
  
  TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1; /*10ms*/
  TIM_TimeBaseInitStructure.TIM_Prescaler = 84-1;/*1us*/
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);
  
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;/*通道*/
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;/*捕获极性:上升沿*/
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;/*映射*/
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;/*不分频-->1,2,4,8  捕获每个边沿设置为1分频*/
  TIM_ICInitStructure.TIM_ICFilter = 0;/*不滤波:0-0x0F, 一般不滤波*/
  TIM_ICInit(TIM5, &TIM_ICInitStructure);
  
  TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE);
  
  vNVICConfigure(TIM5_IRQn, 10);
  
  TIM_Cmd(TIM5, ENABLE);
}

typedef struct _tag_TIMCaptureCtrlValue
{
  bool bCaptureLeveled;  /*捕获到电平标志*/
  bool bCaptureEnd;      /*捕获完成*/
  uint32_t mOVCnt;      /*溢出计数*/
  uint32_t mCapturVal;   /*捕获成功时寄存器的值*/
}xTIMCaptureCtrlValueTypeDef;

xTIMCaptureCtrlValueTypeDef ICCtrValue;

void TIM5_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET){
    if(ICCtrValue.bCaptureLeveled){ /*已经捕获高电平*/
      /*记录高电平时间*/
      ICCtrValue.mOVCnt ++;
    }
  }
  
  /*捕获到电平*/
  if(TIM_GetITStatus(TIM5, TIM_IT_CC1) == SET){
    
    if(ICCtrValue.bCaptureLeveled){/*已经捕获高电平,捕获到低电平*/
      ICCtrValue.bCaptureEnd = true;/*成功捕获一次高电平*/
      ICCtrValue.mCapturVal = TIM_GetCapture1(TIM5);
      TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);
    }else{
      /*第一个捕获到*/
      memset(&ICCtrValue, 0, sizeof(xTIMCaptureCtrlValueTypeDef));
      ICCtrValue.bCaptureLeveled = true;
      
      TIM_Cmd(TIM5, DISABLE);
      TIM_SetCounter(TIM5, 0);/*清空计数*/
      TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);/*设置为下降沿捕获*/
      TIM_Cmd(TIM5, ENABLE);
    }
  }
  
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update | TIM_IT_CC1);
}

