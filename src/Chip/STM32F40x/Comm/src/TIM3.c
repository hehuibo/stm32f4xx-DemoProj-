/*
 ============================================================================
 Name        : TIM3.c
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

