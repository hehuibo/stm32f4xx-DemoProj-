/*
 ============================================================================
 Name        : PWM.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
  
��ʱ��:
  Tout(���ʱ��us) = ((arr+1)*(psc+1))/Tclk(����ʱ��MHz)
  arr:�Զ���װֵ;
  psc:ʱ�ӷ�Ƶ��
 
 ÿ�������ʱ������Tp : Tclk,psc 
 Tclk = 84MHz, psc = (8400 -1)   -----> f = 10KHz
  84*10^6 /(84 * 10^2) = 10^4 == 10KHz
 T = 1/f  = 1/(10 * 1000) 10-4 S  100uS

  note --->>psc: 0-65535
  Tp = 1ms = 1000us ---> 1KHz
  1/(1000) -->10^-3  
  10^3 = Tclk/psc  ---> psc+1 = Tclk/Tp ----> 84*10^6 / 10^3    84*10^3 -->84000

  84 ---> Tp = 10^6 = 1000KHz
  T = 10^-6 = 1us;

  note:
  ��ʱʱ�����T: 1S , Tclk = 84HMz, Tp = 100us -->10KHz)
  Tp(ÿ�ζ�ʱ�����ʱ��) = Tclk / (psc + 1);
  ʱ�ӷ�Ƶ(psc + 1) = Tclk/Tp == 8400  (psc+1 <65535)
  ��ʱʱ��: T = Tp * (arr -1);  ---> 1S = 100us * (10000-1)

 PWM ���:
    < 50Hzʱ�������Ե���˸
  1)���ö�ʱ��ȷ��PWM�����Ƶ��
    Tclk = 84MHz,
    Tp(����ʱ��) =  Tclk/(psc + 1) => 84Mhz/84 = 1MHz
    (arr + 1)(����ʱ��)  ---> 20000   1000000
   fpwm(PWM���Ƶ��) = Tp/(arr+1) = 50Hz
 2)ռ�ձ�(DutyRadio)
  (TIM_Pluse/TIM_Period)* 100%
    ��(CCRx / ARR)100%
  TIM_Pluse:�����������ֵʱ��ƽ��������

  �������????

  �߼���ʱ��--->>TIM_CtrlPWMOutputs
        ����ͨ������
        
 ============================================================================
 */
#include "stm32f4xx.h"
#include "NVIC.h"
#include "PWM.h"

void vPWM_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
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
  
  /*PWMƵ��= 1M/500 = 2KHz*/
  TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler = 84 - 1;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);
  
  /*PWM*/
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                 /*ģʽ*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;     /*�Ƚ����ʹ��*/
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        /*�Ƚ��������*/
  TIM_OCInitStructure.TIM_Pulse = 1250 -1;
  TIM_OC4Init(TIM8, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);/*ʹ��ARRԤװֵ*/
 
  TIM_Cmd(TIM8, ENABLE);
  
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
}
