/*
 * xx.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef MSGMNT_H_
#define MSGMNT_H_

#include "Kernel.h"
#include "TypesCfg.h"


#if defined(STM32F40_41xxx)     /*STM32F4xx*/
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif


/*********************************/

enum _tag_eMsgTYPE
{
  eMSG_BUNTTON,       
  eMSG_TIMECHANGED,         
  eMSG_CARD,
  eMSG_NETMNT,
};

struct _tag_MsgItemTYPE{
  char msgType;
  union{
    struct{
      unsigned int param;
    };
    uint8_t ParamBfr[4];//0:cmd, 1: retcode, 2-3: msgBfr Param len
  };
  uint8_t msgBfr[18];
};



#endif /* MSGMNT_H_ */
