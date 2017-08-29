#ifndef _MAIN_H
#define _MAIN_H

#include "TypesCfg.h"
#include "IAP.h"
#include "StartUpCfg.h"
#include "netio.h"

#include "GPIO.h"
#include "TIM3.h"
#include "PWM.h"
#include "SPI.h"
#include "RTC.h"
#include "ETH_EMAC.h"
#include "Flash.h"
#include "Param.h"

#define MAX_FUNCITEM 3

typedef void(*pFSM_AryFUNCTION[])(void);
extern const pFSM_AryFUNCTION pfn_FSM;

/******Task******/
void AppTaskInit(void);


#endif
