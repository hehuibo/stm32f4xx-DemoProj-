/*
 ============================================================================
 Name        : MsgMnt.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "MsgMnt.h"

#define MSG_ITEM_CMD        0
#define MSG_ITEM_RETCODE    1

struct _tag_MsgMntTYPE{
  xQueueHandle msgQueue;
}msgMnt;

void MsgMnt_Init(void)
{
  memset(&msgMnt, 0, sizeof(struct _tag_MsgMntTYPE));
	
  msgMnt.msgQueue = xQueueCreate(8, sizeof(struct _tag_MsgItemTYPE));
}

/*============================================================================*/
/*Wait Message*/
signed portBASE_TYPE waitCommMsg(struct _tag_MsgMntTYPE *pMnt, struct _tag_MsgItemTYPE *pMsg){
  signed portBASE_TYPE ret = pdFALSE;
  
  ret = xQueueReceive(pMnt->msgQueue, pMsg, portMAX_DELAY);
  
  return ret;
}

/*============================================================================*/
/*============================================================================*/
void vMsgMntProc(struct _tag_MsgItemTYPE *pMsg){

  switch(pMsg->msgType){
  case eMSG_BUNTTON: {
      
    }
    break;

  case eMSG_TIMECHANGED:{
      
    }
    break; 

  case eMSG_CARD:{
     
    }
    break;
    
    default : break;
  }

}


/*MsgMntTask*/
void MsgMntTask(void *arg){
  MsgMnt_Init();
  while(1){
    struct _tag_MsgItemTYPE sMsg;
    
    waitCommMsg(&msgMnt, &sMsg);
    
    vMsgMntProc(&sMsg);
  }
}

void startMsgMntTask(void){
  
  OSThreadCreateEx(MsgMntTask,  NULL, eThreadTYPE_MSGMNT);
  
}


