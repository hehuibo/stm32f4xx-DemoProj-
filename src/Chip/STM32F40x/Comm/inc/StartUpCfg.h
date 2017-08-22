/*
 ============================================================================
 Name        : StartUpCfg.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#ifndef _STM_STARTUPCFG_H
#define _STM_STARTUPCFG_H

#include "stm32f4xx.h"

enum BOOTMODECMD{
  iCmdReBoot = 0,               /*直接复位*/
  iCmdIAPUpGradeMode = 1,       /*进入Boot升级模式*/
  
};

#define RESETMAGIC	0x55AA7799

struct BootMessage{
  unsigned int magic;
  enum BOOTMODECMD bootcmd;
};

void vStartUpConfigure(void);

void ReBoot(enum BOOTMODECMD Mode);

#if defined (_LOADER) || defined (_NoRTOSKernel)

uint32_t xGetTickCount(void);

void SysTickDlyMs(uint16_t ms);

#endif


#endif
