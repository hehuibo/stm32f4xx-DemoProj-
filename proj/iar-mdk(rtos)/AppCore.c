/*
 ============================================================================
 Name        : AppCore.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "main.h"

/***初始化芯片内部资源***/
static void InitChipInternal(void){
  
  vConfigureRTC();
  
  SPI1_Configure();
  
  SPI2_Configure();
  
  vFSMC_SRAM_Init();
  
  CAN_Configure();
  
  
}

/***初始化外围设备***/
static void InitBoardPeripheral(void)
{
  extern void setupNetEnv(void);
  extern void startMsgMntTask(void);
  Flash_Init();
  
  InitParam();
  
  RFID_Init();
  
  vETH_EmacInit();
  
  setupNetEnv();
  
  startMsgMntTask();
}

static void appStart(void){
  unsigned int cardTick = 0;
  unsigned int bntTick = 0;

  for(;;){
    unsigned int tick = OSThreadGetTick();
    
    /*卡片轮训*/
    if(tick > cardTick){
      cardTick = tick + TIME_1000ms; 
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("appStart\n");
      #endif 
    }
    
    /*按键扫描*/
    if(tick > bntTick){
      bntTick = tick + TIME_40ms;
    }
    
  }
}



/*********************Init Task****************************/
void InitTask(void *arg){
   /***初始化芯片内部资源***/
  InitChipInternal();
  
  /***初始化外围设备***/
  InitBoardPeripheral();

  appStart();  
}




