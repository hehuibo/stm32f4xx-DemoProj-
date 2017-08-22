/*
 ============================================================================
 Name        : RTC.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "RTC.h"
#include "TimDly.h"
#include "utils.h"

//RTC Clock Source
#define RTC_CLOCK_SOURCE_LSE       /* LSE */
//#define RTC_CLOCK_SOURCE_LSI     /* LSI */ 

#pragma location = "AHB_RAM_MEMORY"
static struct _tag_RTCTimeParam sysTimeDate;

ErrorStatus xRTC_SetTime(RTC_TimeTypeDef *pTime){
  RTC_TimeTypeDef RTC_TimeTypeInitStructure;
  
  RTC_TimeTypeInitStructure.RTC_Hours = pTime->RTC_Hours;
  RTC_TimeTypeInitStructure.RTC_Minutes = pTime->RTC_Minutes;
  RTC_TimeTypeInitStructure.RTC_Seconds = pTime->RTC_Seconds;
  RTC_TimeTypeInitStructure.RTC_H12 = pTime->RTC_H12;
  
  return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
}

ErrorStatus xRTC_SetDate(RTC_DateTypeDef *pDate)
{
  RTC_DateTypeDef RTC_DateTypeInitStructure;
  
  RTC_DateTypeInitStructure.RTC_Date = pDate->RTC_Date;
  RTC_DateTypeInitStructure.RTC_Month = pDate->RTC_Month;
  RTC_DateTypeInitStructure.RTC_WeekDay = pDate->RTC_WeekDay;
  RTC_DateTypeInitStructure.RTC_Year = pDate->RTC_Year;  
  return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

int  checkTimeValid(struct _tag_RTCTimeParam *Time){
  //if((Time->Date.RTC_Year < 0) || (Time->Date.RTC_Year > 99))
  if(Time->Date.RTC_Year > 99)
     return 0;
      
  if((Time->Date.RTC_Month == 0) || (Time->Date.RTC_Month > 12))
     return 0;

  if(Time->Date.RTC_Date > 31)
     return 0;
   
  if(Time->Time.RTC_H12 == RTC_H12_AM) {
    if(Time->Time.RTC_Hours >= 24) 
       return 0;
  }else{
    if(Time->Time.RTC_Hours >= 12) 
         return 0;
  }
  
  if((Time->Time.RTC_Minutes >= 60) ||
     (Time->Time.RTC_Seconds >= 60))
     return 0;
      
   return 1;
  
}

static void vRTC_Config(void){
  RTC_InitTypeDef RTC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
  PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  //LSI 
  RCC_LSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){
    TimDlyMs(100);
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("RCC LSI is Not Ready!\n");
    #endif 
  }
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#elif defined (RTC_CLOCK_SOURCE_LSE)  //LSE 
  RCC_LSEConfig(RCC_LSE_ON);
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
    TimDlyMs(100);
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("RCC LSE is Not Ready!\n");
    #endif 
  }
  
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#else
#error Please select the RTC Clock source
#endif 
  
  RCC_RTCCLKCmd(ENABLE);
  RTC_WaitForSynchro();
  
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);
  
 
  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != sysTimeDate.TconfigFlag){
    xRTC_SetTime(&sysTimeDate.Time);
    xRTC_SetDate(&sysTimeDate.Date);
    RTC_WriteBackupRegister(RTC_BKP_DR0,sysTimeDate.TconfigFlag);
  }
  
  
  if(!checkTimeValid(&sysTimeDate)){
    sysTimeDate.Date.RTC_Year = 16;
    sysTimeDate.Date.RTC_Month= 3;
    sysTimeDate.Date.RTC_Date  = 24;
    sysTimeDate.Date.RTC_WeekDay = utils_CalWeekDay(sysTimeDate.Date.RTC_Year,sysTimeDate.Date.RTC_Month,sysTimeDate.Date.RTC_Date);
    
    sysTimeDate.Time.RTC_Hours = 17;
    sysTimeDate.Time.RTC_Minutes = 50;
    sysTimeDate.Time.RTC_Seconds = 50;
    sysTimeDate.Time.RTC_H12 = RTC_H12_AM;
    xRTC_SetTime(&sysTimeDate.Time);
    xRTC_SetDate(&sysTimeDate.Date);
    RTC_WriteBackupRegister(RTC_BKP_DR0,sysTimeDate.TconfigFlag);
  }
}

static void vRTC_SetWakeUp(void){
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  
  RTC_WakeUpCmd(DISABLE);
  
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(0);
  
  RTC_ClearITPendingBit(RTC_IT_WUT);
  EXTI_ClearITPendingBit(EXTI_Line22);
  
  RTC_ITConfig(RTC_IT_WUT,ENABLE);
  RTC_WakeUpCmd(ENABLE);
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void vConfigureRTC(void){
  sysTimeDate.Date.RTC_Year = 17;
  sysTimeDate.Date.RTC_Month= 7;
  sysTimeDate.Date.RTC_Date  = 19;
  sysTimeDate.Date.RTC_WeekDay = utils_CalWeekDay(sysTimeDate.Date.RTC_Year,sysTimeDate.Date.RTC_Month,sysTimeDate.Date.RTC_Date);
  
  sysTimeDate.Time.RTC_Hours = 10;
  sysTimeDate.Time.RTC_Minutes = 21;
  sysTimeDate.Time.RTC_Seconds = 50;
  sysTimeDate.Time.RTC_H12 = RTC_H12_AM;
  
  sysTimeDate.TconfigFlag = 0x5959;
  
  vRTC_Config();
  
  vRTC_SetWakeUp();
  
  RTC_GetTime(RTC_Format_BIN,&sysTimeDate.Time);
  RTC_GetDate(RTC_Format_BIN,&sysTimeDate.Date);
}

void vRTC_SetAlarm(struct _tag_AlarmFormat *pAlarm){ 
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
  RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
  RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
  RTC_TimeTypeInitStructure.RTC_Hours = pAlarm->hour;
  RTC_TimeTypeInitStructure.RTC_Minutes = pAlarm->min;
  RTC_TimeTypeInitStructure.RTC_Seconds = pAlarm->sec;
  RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;
  
  RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = pAlarm->week;
  RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;
  RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
  RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
  RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	
  RTC_ClearITPendingBit(RTC_IT_ALRA);
  EXTI_ClearITPendingBit(EXTI_Line17);
	
  RTC_ITConfig(RTC_IT_ALRA,ENABLE);
  RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void RTC_Alarm_IRQHandler(void){    
  if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET){
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    //TRACE("ALARM A!\r\n");
  }   
  EXTI_ClearITPendingBit(EXTI_Line17);	 
}

void RTC_WKUP_IRQHandler(void){
  if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET){
    RTC_ClearFlag(RTC_FLAG_WUTF);
    RTC_GetTime(RTC_Format_BIN,&sysTimeDate.Time);
    RTC_GetDate(RTC_Format_BIN,&sysTimeDate.Date);
  #if defined (FreeRTOS_Kernel)   
  #endif
  }   
  EXTI_ClearITPendingBit(EXTI_Line22);
}

