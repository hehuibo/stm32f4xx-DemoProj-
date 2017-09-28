#include "main.h"

volatile xFSMTypeDef g_FSM;

#define FSM_TIMEOUT_1S      100
uint16_t g_uiFSMDelay1S = FSM_TIMEOUT_1S;

/*******************TargetInit**************************/
/***��ʼ��оƬ�ڲ���Դ***/
USB_OTG_CORE_HANDLE  USB_OTG_dev;
static void InitChipInternal(void){
  //vConfigureRTC();
  
  vGPIO_Configure();
  
  //SPI1_Configure();
  
  //vTIM3_Init();
  
  //vPWM_Init();
  

}

/***��ʼ����Χ�豸***/
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
  
  /***��ʼ��оƬ�ڲ���Դ***/
  InitChipInternal();
  
  /***��ʼ����Χ�豸***/
  InitBoardPeripheral(); 
  
  LcdPutStr("��ʾ������", sizeof("��ʾ������"), eLcdWRCtrlLine_One, 1);
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

/*����*/
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
    /*ѡ����Ŀ¼*/
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
    dbgTRACE("ѡ����Ŀ¼\n");
    #endif 

    /*ȡ�����*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x84\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("ȡ�����\n");
    #endif
    
    
    /*DES��������*/
    memcpy(msgIn, &g_AppCommBfrMnt.pRxBfr[2], 8);
    memcpy(msgKey, PassKeyA,8);
    gfn_rfidCtrl.pfnDES(eRFID_ENCRY, msgIn, msgKey, msgOut);//DES����
    
    /*��Ŀ¼�ⲿ��֤*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x82\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    memcpy(&g_AppCommBfrMnt.pTxBfr[2+g_AppCommBfrMnt.mTxLen], msgOut, 8);
    g_AppCommBfrMnt.mTxLen += 8;
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��Ŀ¼�ⲿ��֤\n");
    #endif 
    
     /*����Ŀ¼�������ļ�*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x0e\x00\x00\x00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ŀ¼�������ļ�\n");
    #endif 
    
    /* ��Ŀ¼��Կ�ļ����� */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x00\x07\x3f\x00\xb0\x01\xf0\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��Ŀ¼��Կ�ļ�����\n");
    #endif
    
    /*  ����ļ���·������Կ*/
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x36\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����ļ���·������Կ\n");
    #endif 
    
    /*  ����ⲿ��֤��Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x39\xf0\xf0\xAA\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����ⲿ��֤��Կ\n");
    #endif 
    
    /* ��ӽ���������Կ*/
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x37\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��ӽ���������Կ\n");
    #endif 
    
    /* �����װ��Կ��Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x38\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("�����װ��Կ��Կ\n");
    #endif 
    
    /* ��ӿ�����Կ��Կ*/
    g_AppCommBfrMnt.mTxLen = 13;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x08\x3a\xf0\xef\x55\x55\x12\x34\x56", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��ӿ�����Կ��Կ\n");
    #endif 
    
    /* ����PBOCӦ��Ŀ¼*/
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x3f\x01\x0d\x38\x05\x20\xf0\xf0\x95\xff\xff\x44\x44\x46\x30\x31", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����PBOCӦ��Ŀ¼\n");
    #endif 

    /* ѡ��PBOCӦ��Ŀ¼*/
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xa4\x00\x00\x02\x3f\01\00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("ѡ��PBOCӦ��Ŀ¼\n");
    #endif 
   
    
    /* ����PBOCĿ¼����Կ�ļ�*/
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x00\x07\x3f\x01\x8f\x95\xf0\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����PBOCĿ¼����Կ�ļ�\n");
    #endif 
    
    
    /* ����ⲿ��֤��Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x39\xf0\xf0\xAA\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����ⲿ��֤��Կ\n");
    #endif 
    
    
    /* �����·������Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x36\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("�����·������Կ\n");
    #endif 
    
    
    /* ��ӽ���������Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x37\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��ӽ���������Կ\n");
    #endif 
    
    
    /* ���TAC��Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x34\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���TAC��Կ\n");
    #endif 
    
    
    /* �����װ��Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x0d\x38\xf0\xf0\xff\x55\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("�����װ��Կ\n");
    #endif 
    
    
    /* ��ӿ�����Կ */
    g_AppCommBfrMnt.mTxLen = 13;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x08\x3a\xf0\xef\x55\x55\x12\x34\x56", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��ӿ�����Կ\n");
    #endif 
    
    
    /* ���Ȧ����Կ0 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x3f\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���Ȧ����Կ0 \n");
    #endif 
    
    
    /* ���Ȧ����Կ1 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x0d\x3f\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���Ȧ����Կ1\n");
    #endif 
    
    
    /* ���������Կ1 */
    g_AppCommBfrMnt.mTxLen = 10;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x15\x3e\xf0\xf0\x00\x00", g_AppCommBfrMnt.mTxLen);
    memcpy(&g_AppCommBfrMnt.pTxBfr[2+g_AppCommBfrMnt.mTxLen],PurchaseKeyA, 16);
    g_AppCommBfrMnt.mTxLen += 16;
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���������Կ1\n");
    #endif 
    
    
    /* ���������Կ2 */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x02\x0d\x3e\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���������Կ2\n");
    #endif 
    
    
    
    /* ���Ȧ����Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x3d\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���Ȧ����Կ\n");
    #endif 
    
    
    /* ���͸֧��Կ */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x00\x0d\x3c\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���͸֧��Կ\n");
    #endif 
    
    
    /* ���TAC�� */
    g_AppCommBfrMnt.mTxLen = 18;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xd4\x01\x01\x0d\x34\xf0\xf0\x00\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("���TAC��\n");
    #endif 
    
    /* ����Ǯ���ֿ��˻��������ļ�  */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x16\x07\x28\x00\x40\xf0\xf0\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ���ֿ��˻��������ļ�\n");
    #endif  
    
    /* ����������ϸ�ļ�  */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x18\x07\x2e\x0a\x17\xf0\xef\xff\xff", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE(" ����������ϸ�ļ�\n");
    #endif 
     
    
    /* ������������Ǯ���ļ� */
    g_AppCommBfrMnt.mTxLen = 12;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\xe0\x00\x02\x07\x2f\x02\x08\xf0\x00\xff\x18", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("������������Ǯ���ļ�\n");
    #endif 
     
  }while(false);

  gfn_rfidCtrl.pfnHalt();
}


/*Ȧ��*/
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
    /*ѡ�����Ǯ��*/
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xa4\x00\x00\x02\x3f\01\00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);

    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("ѡ�����Ǯ��\n");
    #endif 

    /*����Ǯ��PIN����֤ */
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x20\x00\x00\x03\x12\x34\x56", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ��PIN����֤\n");
    #endif
    
    
    
    /*����Ǯ��Ȧ���ʼ��*/
    g_AppCommBfrMnt.mTxLen = 17;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x50\x00\x02\x0b\x00\x00\x00\x12\x34\x01\x02\x03\x04\x05\x06\x10", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ��Ȧ���ʼ��\n");
    #endif 
    
    /* ������Կ����   MAC1 MAC2���� */
     
  }while(false);

  gfn_rfidCtrl.pfnHalt();
}


/*���Ѳ���  */
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
    /*ѡ�����Ǯ��*/
    g_AppCommBfrMnt.mTxLen = 8;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\xa4\x00\x00\x02\x3f\01\00", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);

    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("ѡ�����Ǯ��\n");
    #endif 

    /*��ȡ8�ֽ������ */
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x84\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("��ȡ8�ֽ������\n");
    #endif
    
    /*DES��������*/
    memcpy(msgIn, &g_AppCommBfrMnt.pRxBfr[2], 8);
    memcpy(msgKey, PassKeyA,8);
    gfn_rfidCtrl.pfnDES(eRFID_ENCRY, msgIn, msgKey, msgOut);//DES����
    
    /*����Ǯ���ⲿ��֤*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x00\x82\x00\x00\x08", g_AppCommBfrMnt.mTxLen);
    memcpy(&g_AppCommBfrMnt.pTxBfr[2+g_AppCommBfrMnt.mTxLen], msgOut, 8);
    g_AppCommBfrMnt.mTxLen += 8;
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ���ⲿ��֤\n");
    #endif 
    
    /*����Ǯ�����ѳ�ʼ��*/
    g_AppCommBfrMnt.mTxLen = 17;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x50\x01\x02\x0b\x01\x00\x00\x00\x34\x01\x02\x03\x04\x05\x06\x0f", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ�����ѳ�ʼ��\n");
    #endif 
    
    /* ������Կ����   MAC1 MAC2���� */
#if 0
    memcpy(msgIn,&TPDU_Rece[12],4);                              /* ������Կ����   MAC1 MAC2���� */
    memcpy(&msgIn[4],&TPDU_Rece[5],2);                                 
    memcpy(&msgIn[6],"\x00\x00",2);
    statues = Tdes( msgMode,msgIn,PurchaseKeyA,PassKeyMac);
    memcpy(mac,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
    memcpy(msgIn,"\x00\x00\x00\x34",4);                          /* У����00001234 */
    memcpy(&msgIn[4],"\x06",1);                                  /* ��������ʶ����   */
    memcpy(&msgIn[5],"\x01\x02\x03\x04\x05\x06",6);              /* �����ն�ʶ����  */
    memcpy(&msgIn[11],"\x20\x16\x05\x05\x03\x04\x06",7);         /* ����ʱ��  */
    MAC(mac,PassKeyMac,18,msgIn,mac1);
    memcpy(APDU.send_buff,"\x80\x54\x01\x00\x0f\x00\x00\x00\x00\x20\x16\x05\x05\x03\x04\x06",16); /* ����ָ��               */
    memcpy(&APDU.send_buff[16],mac1,4);                         /*  ����ָ��               */
    memcpy(&APDU.send_buff[20],"\x08",1);   
#endif    
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ�����ѽ���\n");
    #endif 
    
    /*����Ǯ�����*/
    g_AppCommBfrMnt.mTxLen = 5;
    memcpy(&g_AppCommBfrMnt.pTxBfr[2],"\x80\x5C\x00\x02\x04", g_AppCommBfrMnt.mTxLen);
    ret = gfn_rfidCtrl.pfnCmdPro(1, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, g_AppCommBfrMnt.pRxBfr, (uint8_t *)&g_AppCommBfrMnt.mRxLen);
    if(MI_OK != ret){
      break;
    }
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("����Ǯ�����\n");
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


