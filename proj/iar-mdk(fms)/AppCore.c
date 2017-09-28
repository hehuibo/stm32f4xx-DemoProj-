#include "main.h"

volatile xFSMTypeDef g_FSM;

#define FSM_TIMEOUT_1S      100
uint16_t g_uiFSMDelay1S = FSM_TIMEOUT_1S;

/*******************TargetInit**************************/
/***初始化芯片内部资源***/
USB_OTG_CORE_HANDLE  USB_OTG_dev;
static void InitChipInternal(void){
  //vConfigureRTC();
  
  vGPIO_Configure();
  
  //SPI1_Configure();
  
  //vTIM3_Init();
  
  //vPWM_Init();
  

}

/***初始化外围设备***/
static void InitBoardPeripheral(void){
  
  //Flash_Init();
  
  //FlashEraseChip(TFT_FLASHCS);
  
  //GPIO_SetBits(GPIOF,GPIO_Pin_8);
  GPIO_ResetBits(GPIOF,GPIO_Pin_8);
  LCD19264_Init(); 
  
  
  //InitParam();
  
  //RFID_Init();
  
  //vETH_EmacInit();
  
  //startEthernet();
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
  
  LcdPutStr("显示屏测试", sizeof("显示屏测试"), eLcdWRCtrlLine_One, 1);
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
  
}

uint8_t PassKeyA[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xff,0xff};
uint8_t PassKeyB[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t PurchaseKey[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t PurchaseKeyA[16] = {0x3E,0x01,0x3E,0x01 ,0x3E ,0x01, 0x3E ,0x01 ,0x3E ,0x01, 0x3E ,0x01 ,0x3E ,0x01,0x3E,0xFF};
uint8_t PassKeyMac[8];

/*发卡*/
void CardIssure(void){
  char ret = MI_ERR;
  uint8_t msgIn[32];
  uint8_t msgOut[16];
  uint8_t msgKey[16];
  
  uint8_t Buffer[32];
  do{
    ret = gfn_rfidCtrl.pfnRequest(PICC_REQIDL, Buffer);
    if(ret != MI_OK){
      ret = gfn_rfidCtrl.pfnRequest(PICC_REQIDL, Buffer);
    } 
    if(ret != MI_OK){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("Request Err\n");
      #endif 
      break;
    }

    ret = gfn_rfidCtrl.pfnAnticoll(Buffer);
    if(ret != MI_OK){
      break;
    }
    ret = gfn_rfidCtrl.pfnSelect(Buffer);
    if(ret != MI_OK){
      break;
    }
    ret = gfn_rfidCtrl.pfnRATS(g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(ret != MI_OK){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("RATS Err\n");
      #endif 
      break;
    }
    
    /************************************************************/
    /*选择主目录*/
#if 0
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xA4\x00\x00\x00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
#else
    g_AppCommBfrMnt.mTxLen = 7;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xA4\x00\x00\x02\x3f\x00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
#endif 
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("选择主目录\n");
    #endif 

    /*取随机数*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x84\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("取随机数\n");
    #endif
    
    
    /*DES加密运行*/
    memcpy(msgIn, &g_AppCommBfrMnt.pRxBfr[2], 8);
    memcpy(msgKey, PassKeyA,8);
    gfn_rfidCtrl.pfnDES(eRFID_ENCRY, msgIn, msgKey, msgOut);//DES加密
    
    /*主目录外部认证*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x82\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    memcpy(&g_AppCommBfrMnt.pTxBfr[2+g_AppCommBfrMnt.mTxLen], msgOut, 8);
    g_AppCommBfrMnt.mTxLen += 8;
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("主目录外部认证\n");
    #endif 
    
     /*擦除目录下所有文件*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x0e\x00\x00\x00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("擦除目录下所有文件\n");
    #endif 
    
    /* 主目录密钥文件建立 */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x00\x07\x3f\x00\xb0\x01\xf0\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("主目录密钥文件建立\n");
    #endif
    
    /*  添加文件线路保护密钥*/
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x36\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加文件线路保护密钥\n");
    #endif 
    
    /*  添加外部认证密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x39\xf0\xf0\xAA\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加外部认证密钥\n");
    #endif 
    
    /* 添加解锁口令密钥*/
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x37\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加解锁口令密钥\n");
    #endif 
    
    /* 添加重装密钥密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x38\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加重装密钥密钥\n");
    #endif 
    
    /* 添加口令密钥密钥*/
    g_AppCommBfrMnt.mTxLen = 13;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x08\x3a\xf0\xef\x55\x55\x12\x34\x56", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加口令密钥密钥\n");
    #endif 
    
    /* 创建PBOC应用目录*/
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x3f\x01\x0d\x38\x05\x20\xf0\xf0\x95\xff\xff\x44\x44\x46\x30\x31", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("创建PBOC应用目录\n");
    #endif 

    /* 选择PBOC应用目录*/
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xa4\x00\x00\x02\x3f\01\00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("选择PBOC应用目录\n");
    #endif 
   
    
    /* 创建PBOC目录下密钥文件*/
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x00\x07\x3f\x01\x8f\x95\xf0\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("创建PBOC目录下密钥文件\n");
    #endif 
    
    
    /* 添加外部认证密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x39\xf0\xf0\xAA\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加外部认证密钥\n");
    #endif 
    
    
    /* 添加线路保护密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x36\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加线路保护密钥\n");
    #endif 
    
    
    /* 添加解锁口令密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x37\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加解锁口令密钥\n");
    #endif 
    
    
    /* 添加TAC密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x34\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加TAC密钥\n");
    #endif 
    
    
    /* 添加重装密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x0d\x38\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加重装密钥\n");
    #endif 
    
    
    /* 添加口令密钥 */
    g_AppCommBfrMnt.mTxLen = 13;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x08\x3a\xf0\xef\x55\x55\x12\x34\x56", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加口令密钥\n");
    #endif 
    
    
    /* 添加圈存密钥0 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x3f\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加圈存密钥0 \n");
    #endif 
    
    
    /* 添加圈存密钥1 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x0d\x3f\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加圈存密钥1\n");
    #endif 
    
    
    /* 添加消费密钥1 */
    g_AppCommBfrMnt.mTxLen = 10;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x15\x3e\xf0\xf0\x00\x00", g_AppCommBfrMnt.mTxLen);
    memcpy(&g_AppCommBfrMnt.pTxBfr[2+g_AppCommBfrMnt.mTxLen],PurchaseKeyA, 16);
    g_AppCommBfrMnt.mTxLen += 16;
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加消费密钥1\n");
    #endif 
    
    
    /* 添加消费密钥2 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x02\x0d\x3e\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加消费密钥2\n");
    #endif 
    
    
    
    /* 添加圈提密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x3d\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加圈提密钥\n");
    #endif 
    
    
    /* 添加透支密钥 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x3c\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加透支密钥\n");
    #endif 
    
    
    /* 添加TAC密 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x0d\x34\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("添加TAC密\n");
    #endif 
    
    /* 创建钱包持卡人基本数据文件  */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x16\x07\x28\x00\x40\xf0\xf0\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("创建钱包持卡人基本数据文件\n");
    #endif  
    
    /* 创建交易明细文件  */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x18\x07\x2e\x0a\x17\xf0\xef\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE(" 创建交易明细文件\n");
    #endif 
     
    
    /* 创建公共电子钱包文件 */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x02\x07\x2f\x02\x08\xf0\x00\xff\x18", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("创建公共电子钱包文件\n");
    #endif 
     
  }while(false);

  gfn_rfidCtrl.pfnHalt();
}


/*圈存*/
void CardTransference(void){
  char ret = MI_ERR;
  uint8_t msgIn[32];
  uint8_t msgOut[16];
  uint8_t msgKey[16];
  
  uint8_t Buffer[32];
  do{
    ret = gfn_rfidCtrl.pfnRequest(PICC_REQIDL, Buffer);
    if(ret != MI_OK){
      ret = gfn_rfidCtrl.pfnRequest(PICC_REQIDL, Buffer);
    } 
    if(ret != MI_OK){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("Request Err\n");
      #endif 
      break;
    }

    ret = gfn_rfidCtrl.pfnAnticoll(Buffer);
    if(ret != MI_OK){
      break;
    }
    ret = gfn_rfidCtrl.pfnSelect(Buffer);
    if(ret != MI_OK){
      break;
    }
    ret = gfn_rfidCtrl.pfnRATS(g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(ret != MI_OK){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("RATS Err\n");
      #endif 
      break;
    }
    
    /************************************************************/
    /*选择电子钱包*/
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xa4\x00\x00\x02\x3f\01\00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);

    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("选择电子钱包\n");
    #endif 

    /*电子钱包PIN码认证 */
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x20\x00\x00\x03\x12\x34\x56", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("电子钱包PIN码认证\n");
    #endif
    
    
    
    /*电子钱包圈存初始化*/
    g_AppCommBfrMnt.mTxLen = 17;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x50\x00\x02\x0b\x00\x00\x00\x12\x34\x01\x02\x03\x04\x05\x06\x10", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("电子钱包圈存初始化\n");
    #endif 
    
    /* 过程密钥计算   MAC1 MAC2计算 */
     
  }while(false);

  gfn_rfidCtrl.pfnHalt();
}


/*消费操作  */
void CardDec(void){
  char ret = MI_ERR;
  uint8_t msgIn[32];
  uint8_t msgOut[16];
  uint8_t msgKey[16];
  
  uint8_t Buffer[32];
  do{
    ret = gfn_rfidCtrl.pfnRequest(PICC_REQIDL, Buffer);
    if(ret != MI_OK){
      ret = gfn_rfidCtrl.pfnRequest(PICC_REQIDL, Buffer);
    } 
    if(ret != MI_OK){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("Request Err\n");
      #endif 
      break;
    }

    ret = gfn_rfidCtrl.pfnAnticoll(Buffer);
    if(ret != MI_OK){
      break;
    }
    ret = gfn_rfidCtrl.pfnSelect(Buffer);
    if(ret != MI_OK){
      break;
    }
    ret = gfn_rfidCtrl.pfnRATS(g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(ret != MI_OK){
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("RATS Err\n");
      #endif 
      break;
    }
    
    /************************************************************/
    /*选择电子钱包*/
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xa4\x00\x00\x02\x3f\01\00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);

    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("选择电子钱包\n");
    #endif 

    /*获取8字节随机数 */
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x84\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("获取8字节随机数\n");
    #endif
    
    /*DES加密运行*/
    memcpy(msgIn, &g_AppCommBfrMnt.pRxBfr[2], 8);
    memcpy(msgKey, PassKeyA,8);
    gfn_rfidCtrl.pfnDES(eRFID_ENCRY, msgIn, msgKey, msgOut);//DES加密
    
    /*电子钱包外部认证*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x82\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    memcpy(&g_AppCommBfrMnt.pTxBfr[2+g_AppCommBfrMnt.mTxLen], msgOut, 8);
    g_AppCommBfrMnt.mTxLen += 8;
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("电子钱包外部认证\n");
    #endif 
    
    /*电子钱包消费初始化*/
    g_AppCommBfrMnt.mTxLen = 17;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x50\x01\x02\x0b\x01\x00\x00\x00\x34\x01\x02\x03\x04\x05\x06\x0f", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("电子钱包消费初始化\n");
    #endif 
    
    /* 过程密钥计算   MAC1 MAC2计算 */
#if 0
    memcpy(msgIn,&TPDU_Rece[12],4);                              /* 过程密钥计算   MAC1 MAC2计算 */
    memcpy(&msgIn[4],&TPDU_Rece[5],2);                                 
    memcpy(&msgIn[6],"\x00\x00",2);
    statues = Tdes( msgMode,msgIn,PurchaseKeyA,PassKeyMac);
    memcpy(mac,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
    memcpy(msgIn,"\x00\x00\x00\x34",4);                          /* 校验金额00001234 */
    memcpy(&msgIn[4],"\x06",1);                                  /* 交易类型识别吗   */
    memcpy(&msgIn[5],"\x01\x02\x03\x04\x05\x06",6);              /* 交易终端识别码  */
    memcpy(&msgIn[11],"\x20\x16\x05\x05\x03\x04\x06",7);         /* 交易时间  */
    MAC(mac,PassKeyMac,18,msgIn,mac1);
    memcpy(APDU.send_buff,"\x80\x54\x01\x00\x0f\x00\x00\x00\x00\x20\x16\x05\x05\x03\x04\x06",16); /* 消费指令               */
    memcpy(&APDU.send_buff[16],mac1,4);                         /*  消费指令               */
    memcpy(&APDU.send_buff[20],"\x08",1);   
#endif    
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("电子钱包消费交易\n");
    #endif 
    
    /*电子钱包余额*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x5C\x00\x02\x04", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("电子钱包余额\n");
    #endif 
     
  }while(false);

  gfn_rfidCtrl.pfnHalt();
}



/*********************1STask****************************/
void vDelay1STask(void){
  if(FSM_IsOff(g_FSM.FSM_TASK_DELAY1S)){
    return ;
  }
  FSM_SetOff(g_FSM.FSM_TASK_DELAY1S);	
	
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("appStart\n");//
  #endif 
  
 // getCardSnrPoll();

  LcdPutStr("Lcd TestDemo", sizeof("Lcd TestDemo"), eLcdWRCtrlLine_Two, 2);
  LcdPutStr("Lcd TestDemo", sizeof("Lcd TestDemo"), eLcdWRCtrlLine_Three, 2);
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
  
  //LwIP_PeriodicHandle();
}

const pfnFSM_AryFUNCTION pfnFSMTaskAry = {
  vDelay10MSTask,
  vDelay1STask,
  vMainTask
};


