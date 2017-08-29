/*
 ============================================================================
 Name        : TIM3.c
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
 ============================================================================
 */
#include "stm32f4xx.h"
#include "NVIC.h"
#include "TIM3.h"

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
#include "trace.h"
#endif

void vTIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  TIM_TimeBaseInitStructure.TIM_Period = 10000-1; //
  TIM_TimeBaseInitStructure.TIM_Prescaler = 8400 -1; //1M
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
 
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  vNVICConfigure(TIM3_IRQn, 10);
  TIM_Cmd(TIM3, ENABLE);
}


void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE_FUNCTION();
    #endif  
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
}

