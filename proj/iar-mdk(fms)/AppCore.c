#include "main.h"

volatile xFSMTypeDef g_FSM;

#define FSM_TIMEOUT_1S      100
uint16_t g_uiFSMDelay1S = FSM_TIMEOUT_1S;




/*******************TargetInit**************************/
/***初始化芯片内部资源***/
static void InitChipInternal(void){
  vConfigureRTC();
  
  vGPIO_Configure();
  
  SPI1_Configure();
  
  vTIM3_Init();
}

/***初始化外围设备***/
static void InitBoardPeripheral(void){
  
  Flash_Init();
    
  InitParam();
  
  vETH_EmacInit();
  
  startEthernet();
}


static void AppTargetInit(void){
  vStartUpConfigure();
}

void AppTaskInit(void){
  AppTargetInit();
  
  /***初始化芯片内部资源***/
  InitChipInternal();
  
  /***初始化外围设备***/
  InitBoardPeripheral(); 
  
}

/*******************10MSTask**************************/
void vDelay10MSTask(void){
  if(FSM_IsOff(g_FSM.FSM_TASK_DELAY10MS))
    return ;
  FSM_SetOff(g_FSM.FSM_TASK_DELAY10MS);

  if((g_uiFSMDelay1S >0 )&&(--g_uiFSMDelay1S == 0)){
    FSM_SetOn(g_FSM.FSM_TASK_DELAY1S);
    g_uiFSMDelay1S = FSM_TIMEOUT_1S;
  }
  
  if((g_AppCommBfrMnt.mTimeOut > 0) && (--g_AppCommBfrMnt.mTimeOut == 0)){
    g_AppCommBfrMnt.mRxLen = g_AppCommBfrMnt.mCnt;
    g_AppCommBfrMnt.mCnt = 0;
    g_AppCommBfrMnt.mTimeOut = 0;
    switch(g_AppCommBfrMnt.mMsgID){
      case eAppCommMsgID_USART1 :{
        FSM_SetOn(g_FSM.FSM_FLAG_USART1RXED);	
      }
      break;
      
      case eAppCommMsgID_USART3 :{
        
      }
      break;
      
      case eAppCommMsgID_UART4 :{
        FSM_SetOn(g_FSM.FSM_FLAG_UART4RXED);	
      }
      break;
    default:break;
    }
    g_AppCommBfrMnt.mMsgID = eAppCommMsgID_NONE;
  }

}
/*********************1STask****************************/
void vDelay1STask(void){
  if(FSM_IsOff(g_FSM.FSM_TASK_DELAY1S)){
    return ;
  }
  FSM_SetOff(g_FSM.FSM_TASK_DELAY1S);	
	
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("appStart\n");
  #endif 
  
  GPIO_ToggleBits(GPIOC,GPIO_Pin_8);
}

/***************MainTask*************************/

void vMainTask(void){
  
  if(FSM_IsOn(g_FSM.FSM_FLAG_UART4RXED)){
    FSM_SetOff(g_FSM.FSM_FLAG_UART4RXED);	
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("RxBuffer:\n");
    for(int i=0; i<g_AppCommBfrMnt.mRxLen; i++){
      dbgTRACE("%x ",g_AppCommBfrMnt.pRxBfr[i]);
    }
    dbgTRACE("\n");  
    #endif 
  }
  
  LwIP_Periodic_Handle();
}

const pfFSMCallFunctionTYPE pFunCalled = {
  vDelay10MSTask,
  vDelay1STask,
  vMainTask
};


