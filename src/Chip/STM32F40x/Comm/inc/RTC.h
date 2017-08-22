/*
 * RTC.h
 *
 *  Created on: 20xx年x月x日
 *      Author: HeHuiBo
 */
#ifndef _RTC_H
#define _RTC_H

#if defined (STM32F40_41xxx)
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif

//闹钟
struct _tag_AlarmFormat
{
  u8 week;
  u8 hour;
  u8 min;
  u8 sec;
};

//时间参数
struct _tag_RTCTimeParam
{
  RTC_TimeTypeDef Time;
  RTC_DateTypeDef Date;
  struct _tag_AlarmFormat Alarm;
  u32 TconfigFlag;
  
};

void vRTC_GetTimeDate(void);
void vConfigureRTC(void);
ErrorStatus xRTC_SetTime(RTC_TimeTypeDef *pTime);
ErrorStatus xRTC_SetDate(RTC_DateTypeDef *pDate);
int  checkTimeValid(struct _tag_RTCTimeParam *Time);

#endif
