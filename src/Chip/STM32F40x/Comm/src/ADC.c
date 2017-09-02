/*
 ============================================================================
 Name        : ADC.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
  APB2 ---> 84MHz
  ADC 最大f  36MHz
 ============================================================================
 */
#include "stm32f4xx.h"
#include "ADC.h"

void vADC_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_InitTypeDef   ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /*ADC*/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;/*采样延迟时钟*/
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//84/4 = 21Mhz
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;/*持续转换*/
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//触发检测
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_Cmd(ADC1, ENABLE);
  
  /*指定ADC的规则通道,一个序列,采样时间*/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);
  ADC_SoftwareStartConv(ADC1);
#if 0 
  while(RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
  ADC_GetConversionValue(ADC1);
#endif
}