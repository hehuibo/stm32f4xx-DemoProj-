#include "main.h"

volatile xFSMTypeDef g_FSM;

#define FSM_TIMEOUT_1S      100
uint16_t g_uiFSMDelay1S = FSM_TIMEOUT_1S;



/*******************TargetInit**************************/
/***��ʼ��оƬ�ڲ���Դ***/
USB_OTG_CORE_HANDLE  USB_OTG_dev;
static void InitChipInternal(void){
  vConfigureRTC();
  
  vGPIO_Configure();
  
  SPI1_Configure();
  
  //vTIM3_Init();
  
  //vPWM_Init();
  

}

/***��ʼ����Χ�豸***/
static void InitBoardPeripheral(void){
  
  Flash_Init();
  
  //FlashEraseChip(TFT_FLASHCS);
    
  InitParam();
  
  vETH_EmacInit();
  
  startEthernet();
}


static void AppTargetInit(void){
  vStartUpConfigure();
}

void AppTaskInit(void){
  uint8_t wbuffer[32];
  
  AppTargetInit();
  
  /***��ʼ��оƬ�ڲ���Դ***/
  InitChipInternal();
  
  /***��ʼ����Χ�豸***/
  InitBoardPeripheral(); 
  
  for(int i=0; i< 32;i ++){
    wbuffer[i] = i;
  }
#if 0
  int wlen = FlashWrite(0x1000, wbuffer, 32, eFLASH_ID_CS0);

  dbgTRACE("\nFlashReadEnd: %d\r\n", wlen);
   dbgTRACE("\nFlashReadEnd: %d\r\n", wlen);
    dbgTRACE("\nFlashReadEnd: %d\r\n", wlen);
     dbgTRACE("\nFlashReadEnd: %d\r\n", wlen);
  dbgTRACE("\nFlashReadEnd\r\n");
  dbgTRACE("\nFlashReadEnd\r\n");
  dbgTRACE("\nFlashReadEnd\r\n");
  dbgTRACE("\nFlashReadEnd\r\n");
#endif  
  
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
      case eAppCommMsgID_USART1 :{//USART1
        FSM_SetOn(g_FSM.FSM_FLAG_USART1RXED);	
      }
      break;
      
      case eAppCommMsgID_USART3 :{//USART3
        
      }
      break;
      
      case eAppCommMsgID_UART4 :{//UART4
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
  #if 1
    uint8_t buffer[32];
    int rlen  =  FlashRead(0x1000, buffer, 32, eFLASH_ID_CS0);
    dbgTRACE("\nFlashReadEnd: rlen %d\r\n", rlen);
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    for(int i= 0 ;i< rlen;i++)
    dbgTRACE("FlashRead:%x\n", buffer[i]);
    #endif 
  #endif
  GPIO_ToggleBits(GPIOE,GPIO_Pin_2);
}

/***************MainTask*************************/

void vMainTask(void)
{
  
  if(FSM_IsOn(g_FSM.FSM_FLAG_USART1RXED)){
    FSM_SetOff(g_FSM.FSM_FLAG_USART1RXED);	
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("USART1 Recved\n");  
    #endif 
  }
  
  LwIP_PeriodicHandle();
}

const pfnFSM_AryFUNCTION pfnFSMTaskAry = {
  vDelay10MSTask,
  vDelay1STask,
  vMainTask
};


