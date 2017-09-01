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

FATFS fs;
FIL  fil;

UINT    fnum;
char fpath[100];
char rbuffer[512];
BYTE work[_MAX_SS];
void FatFsTestDemo(void)
{
  FRESULT fres;
  FATFS *pfs;
  DWORD fre_clust, fre_sect, tot_sect;
  //fres = f_mkfs("0:", FM_FAT, 0, work, sizeof(work));
  if((fres = f_mount(&fs, "0:", 1)) == FR_NO_FILESYSTEM){
    fres = f_mkfs("0:", FM_FAT, 0, work, sizeof(work));
    if(FR_OK == fres){
      f_mount(NULL, "0:", 1);
      fres = f_mount(&fs, "0:", 1);
    }
  }
  
  if(FR_OK == fres){
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("Fs Mount OK\n");
    #endif 
  }else{
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("Fs Mount ERR,res = %d\n",fres);
    #endif
  }
  
  /*��ȡ�豸��Ϣ*/
  fres = f_getfree("0:", &fre_clust, &pfs);
  tot_sect = (pfs->n_fatent - 2)*pfs->csize;/*��������*/
  fre_sect = fre_clust * pfs->csize;
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE(">>�豸�ܿռ�: %10lu KB\n>>���ÿռ�: %10lu KB", tot_sect*4, fre_sect*4);
  #endif
}

void AppTaskInit(void){
  AppTargetInit();
  
  /***��ʼ��оƬ�ڲ���Դ***/
  InitChipInternal();
  
  /***��ʼ����Χ�豸***/
  InitBoardPeripheral(); 
  
  //FatFsTestDemo();
  
#if 1
  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_MSC_cb,
            &USR_cb);
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
  
  GPIO_ToggleBits(GPIOE,GPIO_Pin_2);
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
  
  LwIP_PeriodicHandle();
}

const pFSM_AryFUNCTION pfn_FSM = {
  vDelay10MSTask,
  vDelay1STask,
  vMainTask
};


