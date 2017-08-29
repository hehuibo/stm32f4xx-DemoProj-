/*
 ============================================================================
 Name        : TIMIC.c
 Author      : 
 Version     :
 Copyright   : 
 Description : TIM5, PA0
  ���벶��
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
  
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;/*ͨ��*/
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;/*������:������*/
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;/*ӳ��*/
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;/*����Ƶ-->1,2,4,8  ����ÿ����������Ϊ1��Ƶ*/
  TIM_ICInitStructure.TIM_ICFilter = 0;/*���˲�:0-0x0F, һ�㲻�˲�*/
  TIM_ICInit(TIM5, &TIM_ICInitStructure);
  
  TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE);
  
  vNVICConfigure(TIM5_IRQn, 10);
  
  TIM_Cmd(TIM5, ENABLE);
}

typedef struct _tag_TIMCaptureCtrlValue
{
  bool bCaptureLeveled;  /*���񵽵�ƽ��־*/
  bool bCaptureEnd;      /*�������*/
  uint32_t mOVCnt;      /*�������*/
  uint32_t mCapturVal;   /*����ɹ�ʱ�Ĵ�����ֵ*/
}xTIMCaptureCtrlValueTypeDef;

xTIMCaptureCtrlValueTypeDef ICCtrValue;

void TIM5_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET){
    if(ICCtrValue.bCaptureLeveled){ /*�Ѿ�����ߵ�ƽ*/
      /*��¼�ߵ�ƽʱ��*/
      ICCtrValue.mOVCnt ++;
    }
  }
  
  /*���񵽵�ƽ*/
  if(TIM_GetITStatus(TIM5, TIM_IT_CC1) == SET){
    
    if(ICCtrValue.bCaptureLeveled){/*�Ѿ�����ߵ�ƽ,���񵽵͵�ƽ*/
      ICCtrValue.bCaptureEnd = true;/*�ɹ�����һ�θߵ�ƽ*/
      ICCtrValue.mCapturVal = TIM_GetCapture1(TIM5);
      TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);
    }else{
      /*��һ������*/
      memset(&ICCtrValue, 0, sizeof(xTIMCaptureCtrlValueTypeDef));
      ICCtrValue.bCaptureLeveled = true;
      
      TIM_Cmd(TIM5, DISABLE);
      TIM_SetCounter(TIM5, 0);/*��ռ���*/
      TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);/*����Ϊ�½��ز���*/
      TIM_Cmd(TIM5, ENABLE);
    }
  }
  
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update | TIM_IT_CC1);
}

