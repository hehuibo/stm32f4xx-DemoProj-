/*
 * xx.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef IAP_H_
#define IAP_H_

#include "STFLASH.h"
#include "TypesCfg.h"

#define APP_START_ADDRESS        0x08010000 //0x08010000

typedef  void (*pfAppTYPE)(void);

extern pfAppTYPE JumpToApp;
extern uint32_t JumpAddress;

eErrTYPE  ProgramFrameMnt(struct _tag_AppCommBufferTYPE *pAppEnv);

#endif /* IAP_H_ */
