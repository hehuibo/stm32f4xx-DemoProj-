/*
 ============================================================================
 Name        : PWM.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
  
定时器:
  Tout(溢出时间us) = ((arr+1)*(psc+1))/Tclk(输入时钟MHz)
  arr:自动重装值;
  psc:时钟分频数
 
 每次溢出的时间周期Tp : Tclk,psc 
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
  定时时间计算T: 1S , Tclk = 84HMz, Tp = 100us -->10KHz)
  Tp(每次定时器溢出时间) = Tclk / (psc + 1);
  时钟分频(psc + 1) = Tclk/Tp == 8400  (psc+1 <65535)
  定时时间: T = Tp * (arr -1);  ---> 1S = 100us * (10000-1)

 PWM 输出:
    < 50Hz时会有明显的闪烁
  1)设置定时器确定PWM输出的频率
    Tclk = 84MHz,
    Tp(计数时钟) =  Tclk/(psc + 1) => 84Mhz/84 = 1MHz
    (arr + 1)(计数时长)  ---> 20000   1000000
   fpwm(PWM输出频率) = Tp/(arr+1) = 50Hz
 2)占空比(DutyRadio)
  (TIM_Pluse/TIM_Period)* 100%
    或(CCRx / ARR)100%
  TIM_Pluse:当计数到这个值时电平发生跳变

  输出极性????

  高级定时器--->>TIM_CtrlPWMOutputs
        互补通道？？
        
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
  
  /*PWM频率= 1M/500 = 2KHz*/
  TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler = 84 - 1;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);
  
  /*PWM*/
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                 /*模式*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;     /*比较输出使能*/
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        /*比较输出极性*/
  TIM_OCInitStructure.TIM_Pulse = 1250 -1;
  TIM_OC4Init(TIM8, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);/*使能ARR预装值*/
 
  TIM_Cmd(TIM8, ENABLE);
  
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
}
