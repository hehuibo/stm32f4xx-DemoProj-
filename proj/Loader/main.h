#ifndef _MAIN_H
#define _MAIN_H

#include "TypesCfg.h"

#include "GPIO.h"
#include "IAP.h"
#include "StartUpCfg.h"
#include "netio.h"

#include "GPIO.h"
#include "SPI.h"
#include "RTC.h"
#include "ETH_EMAC.h"
#include "Flash.h"
#include "Param.h"

#define MAX_FUNCITEM 3

typedef void(*pfFSMCallFunctionTYPE[])(void);
extern const pfFSMCallFunctionTYPE pFunCalled;

/******Task******/
void AppTaskInit(void);


#endif
