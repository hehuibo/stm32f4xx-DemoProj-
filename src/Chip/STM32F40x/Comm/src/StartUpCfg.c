/*
 ============================================================================
 Name        : StartUpCfg.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "StartUpCfg.h"
#include "IAP.h"


void ReBoot(enum BOOTMODECMD Mode){
  struct BootMessage *bootmsg = ( struct BootMessage  *)0x10000000 ;
  {
    bootmsg->magic = RESETMAGIC ;
    bootmsg->bootcmd = Mode ;		
  }
  __set_FAULTMASK(1);
  NVIC_SystemReset();
}

#if defined (_LOADER) 
void JumpToApplication(void)
{
  __set_PRIMASK(1);
  if (((*(__IO uint32_t*)APP_START_ADDRESS) & 0x1FFE0000 ) == 0x10000000){
    __ASM("CPSID  I");
    JumpAddress = *(__IO uint32_t*) (APP_START_ADDRESS + 4);
    JumpToApp = (pfAppTYPE)JumpAddress;

    __set_MSP(*(__IO uint32_t*) APP_START_ADDRESS);
    __set_PSP(*(__IO uint32_t*) APP_START_ADDRESS);
    __set_CONTROL(0);

    /* ensure what we have done could take effect */
    __ISB();

    JumpToApp();
  }
  __set_FAULTMASK(0);
  __ASM("CPSIE  I"); 
}
#endif


void vStartUpConfigure(void)
{
  /* Set the Vector Table base location */
#ifdef _LOADER
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
#else
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000);
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
  __ASM("CPSIE  I");
#endif

#ifdef _LOADER  
  struct BootMessage *bootmsg = ( struct BootMessage  *)0x10000000;
  if(bootmsg->magic == RESETMAGIC){
    switch(bootmsg->bootcmd){
      case   iCmdReBoot:{
        JumpToApplication();
      }
      break;
      
      case iCmdIAPUpGradeMode:{
        __set_FAULTMASK(0);
        __ASM("CPSIE  I"); 
      }
      break;
    }
  }else{
    JumpToApplication();
  }
#endif
  
  /* Set the NVIC PriorityGroup */
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
   
   AppCommBufferInit();
   
   
#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    TRACE_Init();
#endif

  /* Set the SysTick */
#if defined (_LOADER) || defined (_NoRTOSKernel)
	RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks); 

    SysTick_Config(rcc_clocks.SYSCLK_Frequency / 1000); /* 1Ms  */
#endif
          
}

#if defined (_LOADER) || defined (_NoRTOSKernel)

volatile static uint32_t SysTickCnt = 0;
extern __IO uint32_t LocalTime;
void SysTick_Handler(void){
  static int tickCnt_10MS = 0;
  
  SysTickCnt ++;
  if(++tickCnt_10MS >= 10){
    FSM_SetOn(g_FSM.FSM_TASK_DELAY10MS);
    tickCnt_10MS = 0;//10MS
    LocalTime += 10;
  }  
}


uint32_t xGetTickCount(void){
  return SysTickCnt;
}


void SysTickDlyMs(uint16_t ms){
  uint16_t t1,t;
	
  t1 = xGetTickCount();
  while(1){
    t = xGetTickCount() - t1;
    if(t >= ms){
      break;
    }
  }
	
}
#endif


