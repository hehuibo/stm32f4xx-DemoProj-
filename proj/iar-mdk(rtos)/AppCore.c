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

/***��ʼ��оƬ�ڲ���Դ***/
static void InitChipInternal(void){
  
  vConfigureRTC();
  
  SPI1_Configure();
  
  SPI2_Configure();
  
  vFSMC_SRAM_Init();
  
  CAN_Configure();
  
  
}

/***��ʼ����Χ�豸***/
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
    
    /*��Ƭ��ѵ*/
    if(tick > cardTick){
      cardTick = tick + TIME_1000ms; 
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("appStart\n");
      #endif 
    }
    
    /*����ɨ��*/
    if(tick > bntTick){
      bntTick = tick + TIME_40ms;
    }
    
  }
}



/*********************Init Task****************************/
void InitTask(void *arg){
   /***��ʼ��оƬ�ڲ���Դ***/
  InitChipInternal();
  
  /***��ʼ����Χ�豸***/
  InitBoardPeripheral();

  appStart();  
}




