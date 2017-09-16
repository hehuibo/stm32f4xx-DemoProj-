/*
 ============================================================================
 Name        : KernelAL.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Kernel.h"
#include "StartUpCfg.h"

const sThreadAttr_t ThreadAttrAry[eThreadTYPE_MAX] = 
{
  /*--mStackBase---------mStackSize--------------mPrio---------------mName----*/
  {   NULL,        TASK_STACK_INIT,          TASK_PRIO_INIT,	    "Init"  },		/*	InitTask     	*/
  {   NULL,        TASK_STACK_MSGMNT,	     TASK_PRIO_MSGMNT,	    "Msg"   },		/*	MsgMnt          */
  {   NULL,        TASK_STACK_GUI,	         TASK_PRIO_GUI,	        "GUI"   },		/*	GuiMnt          */
};


void OSThreadInit(void){
  vStartUpConfigure();
}

void OSThreadCreate(void){
  extern void InitTask(void *arg);
  
  OSThreadCreateEx(InitTask, NULL, eThreadTYPE_INIT);
}

void OSThreadStart(void){
  vTaskStartScheduler();
}

eErrTYPE OSThreadCreateEx(void (*pThreadCode)(void *), void *param, enum eThreadTYPE eType)
{
  signed portBASE_TYPE  ret ;
  const sThreadAttr_t *attr ;
  
  if(eType > eThreadTYPE_MAX){
    return eERR_EINVAL ;
  }
	
  attr = &ThreadAttrAry[eType];

  ret = xTaskCreate( pThreadCode, 
                    (char const*)attr->mName, 
                    attr->mStackSize, 
                    param, 
                    attr->mPrio, 
                    NULL);
 
  if( ret == pdFALSE ){
    return eERR_NOMEM ;
  }
	
  return eERR_OK;
}
