/*
 * Kernel.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef OSKERNEL_H_
#define OSKERNEL_H_

#include "TypesCfg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "appcfg.h"

typedef struct _tag_ThreadAttr{
  void * mStackBase;
  unsigned short mStackSize;
  unsigned char mPrio;
  const char * mName;
}sThreadAttr_t;

enum eThreadTYPE
{
  eThreadTYPE_INIT,
  eThreadTYPE_MSGMNT,
  eThreadTYPE_GUI,
  eThreadTYPE_MAX
};
#define KERNEL_ENTER_CRITICAL() portENTER_CRITICAL()
#define KERNEL_EXIT_CRITICAL()  portEXIT_CRITICAL()

#define OSThreadGetTick xTaskGetTickCount

void OSThreadInit(void);

void OSThreadCreate(void);

void OSThreadStart(void);

eErrTYPE OSThreadCreateEx(void (*pThreadCode)(void *), void *param, enum eThreadTYPE eType);

#endif /* OSTHREADAL_H_ */
