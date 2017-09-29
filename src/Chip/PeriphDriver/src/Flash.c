#include "Flash.h"

/***************************************************/
enum _tag_eFlashCmd
{  
  eFlashCmd_WriteStatus         = 0x01,
  eFlashCmd_Write               = 0x02,  /*Page Program*/
  eFlashCmd_Read                = 0x03,
  eFlashCmd_WriteDisable        = 0x04,
  eFlashCmd_ReadStatus          = 0x05,
  eFlashCmd_WriteEnable         = 0x06,
  
  eFlashCmd_Erase4K             = 0x20, /*Sector Erase*/
  eFlashCmd_Erase32K            = 0x52, /* 32k Block Erase*/
  eFlashCmd_Erase64K            = 0xD8, /*64K Block Erase*/
  eFlashCmd_EraseAll            = 0xC7, /*Chip Erase*/
  
  eFlashCmd_DeviceID            = 0x9F,
  
  eFlash_DummyByte              = 0x00,
};
/*********************************************/
#if defined (FLASH_DMA_TXRX)
#define FLASH_RX_LEN 10
uint8_t FLASH_RX_BUFFER[FLASH_RX_LEN];
uint8_t FLASH_TX_BUFFER[FLASH_RX_LEN]; 
#endif

/*********************************************/
static struct _tag_DataFlash FlashChip[FLASH_CS_NUM];

/*********************************************/
#if defined (FLASH_DMA_TXRX)
extern const pfnFlashDMATxRxFUNCTIONAry pfFlashDMATxRxAry;
extern const pfnFlashDMATxRxFUNCTIONAry pfFlashDMARxAry;
extern const pfnFlashDMATxRxFUNCTIONAry pfFlashDMATxAry;
#else
extern const pfFlashTxRxFUNCTION pfFlashTxRxAry;
#endif
extern const xTFlashCSCtrlValTypeDef gxFlashCSCtrlValAry[FLASH_CS_NUM];

extern const struct data_flash_dev data_flash_ids[];

/*********************************************/
/*
 *正在擦除中
 */
volatile bool isErasing;

/*********************************************/
struct _tag_DataFlash* getCurrFlash(enum eFlashCSTYPE FlashCSNum)
{
  return &FlashChip[FlashCSNum];
}

static void Flash_WR_Enable(enum eFlashCSTYPE FlashCSNum)
{
  GPIO_ResetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);
#if defined(FLASH_DMA_TXRX) 
  unsigned char mCnt = 0;
  FLASH_TX_BUFFER[mCnt++] = eFlashCmd_WriteEnable;
  pfFlashDMATxRxAry[FlashCSNum](FLASH_TX_BUFFER, FLASH_RX_BUFFER, mCnt);
#else
  pfFlashTxRxAry[FlashCSNum](eFlashCmd_WriteEnable);
#endif
  
  GPIO_SetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);

}

static unsigned char Flash_RD_SR(enum eFlashCSTYPE FlashCSNum)
{
  unsigned char ret;
  GPIO_ResetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);
#if defined (FLASH_DMA_TXRX)
  unsigned char mCnt = 0;
  FLASH_TX_BUFFER[mCnt++] = eFlashCmd_ReadStatus;
  FLASH_TX_BUFFER[mCnt++] = eFlash_DummyByte;
  pfFlashDMATxAry[FlashCSNum](FLASH_TX_BUFFER, &ret, mCnt);
#else
  pfFlashTxRxAry[FlashCSNum](eFlashCmd_ReadStatus);
  ret = pfFlashTxRxAry[FlashCSNum](eFlash_DummyByte);
#endif
  GPIO_SetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);

  return ret;
}

/*
 *FlashCmd  proc
 */
signed char FlashCommand(unsigned char cmd, int addr, const void *indat, void *outdat, unsigned int len, enum eFlashCSTYPE FlashCSNum)
{
  unsigned char *inbuf = (unsigned char*)indat;
  unsigned char *outbuf = (unsigned char*)outdat;
  unsigned char mCnt = 0;
  /*发送命令*/
  
  while(1){
    #if defined (FreeRTOS_Kernel) 
    pfFlashSpiLockAry[FlashCSNum]();
    #endif    
    if(!isErasing){
      /*Idle*/
      if(cmd == eFlashCmd_Erase4K  ||
         cmd == eFlashCmd_Erase32K ||
         cmd == eFlashCmd_Erase64K ){
          isErasing = true;
        }
       break;
     }
     else if(cmd == eFlashCmd_WriteEnable ||
             cmd == eFlashCmd_WriteDisable||
             cmd == eFlashCmd_ReadStatus){
        break;
      }
     #if defined (FreeRTOS_Kernel) 
     pfFlashSpiUnLockAry[FlashCSNum]();
     SPI_Wait();
     #endif
	
  }/*end while*/
  
  switch(cmd){
    case eFlashCmd_Erase32K :
    case eFlashCmd_Erase4K :
    case eFlashCmd_Erase64K :
    case eFlashCmd_EraseAll :
    case eFlashCmd_Write :{
      do{
        Flash_WR_Enable(FlashCSNum);
        /*delay*/
         for(int i=0; i<100; i++){
            __ASM("nop");
         }
        unsigned char stat;
        do{		
          stat = Flash_RD_SR(FlashCSNum);
          //dbgprint("%x\t %x\r\n",stat, stat & 2);
          #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
          dbgTRACE("%x\t %x\r\n",stat, stat & 2);
          #endif 
        }while((stat & 2)== 0);

      }while(false);
     }
      break;
  }/*end switch*/
  
  
  /*发送命令*/
#if defined (FLASH_DMA_TXRX)
  mCnt = 0;
  FLASH_TX_BUFFER[mCnt++] = cmd;
  GPIO_ResetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);
  if(addr != -1){ //读写数据
    FLASH_TX_BUFFER[mCnt++] = (uint8_t)((addr>>16)&0xFF);
    FLASH_TX_BUFFER[mCnt++] = (uint8_t)((addr>>8)&0xFF);
    FLASH_TX_BUFFER[mCnt++] = (uint8_t)(addr&0xFF);
    //GPIO_ResetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);
    pfFlashDMATxRxAry[FlashCSNum](FLASH_TX_BUFFER, FLASH_RX_BUFFER, mCnt);
    if(inbuf){/*写数据*/
      pfFlashDMATxAry[FlashCSNum](inbuf,NULL,len);
    }
    if(outbuf){/*读数据*/
      pfFlashDMARxAry[FlashCSNum](NULL, outbuf,len);
    }
  }else{
    for(int i= mCnt; i< len; i++){
      FLASH_TX_BUFFER[i] = eFlash_DummyByte;
    }
    mCnt += len;
    //GPIO_ResetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);
    pfFlashDMATxRxAry[FlashCSNum](FLASH_TX_BUFFER, FLASH_RX_BUFFER, mCnt);
    if(len){
      memcpy(outbuf, &FLASH_RX_BUFFER[mCnt - len], len);
    } 
  }
#else
  
  
#endif
  
  GPIO_SetBits(gxFlashCSCtrlValAry[FlashCSNum].mGPIOx, gxFlashCSCtrlValAry[FlashCSNum].mBasePin);
  
  switch(cmd){
    case eFlashCmd_Write :{
      for(int i=0; i<100; i++){
        __ASM("nop");
      }
      unsigned char stat;
      do{		
          stat = Flash_RD_SR(FlashCSNum);
          #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
          dbgTRACE("%x\t %x\r\n",stat, stat & 1);
          #endif
      }while((stat & 1));

      for(int i=0; i<100; i++){
        __ASM("nop");
      }     
    }
    break;

    default:
      break;
  }
  #if defined (FreeRTOS_Kernel) 
  pfFlashSpiUnLockAry[FlashCSNum]();
  #endif
  return 0;
}

static void FlashSendCmd(unsigned char cmd, enum eFlashCSTYPE FlashCSNum){
  FlashCommand(cmd, -1, NULL, NULL, 0, FlashCSNum);
}

static unsigned char FlashReadStatus(enum eFlashCSTYPE FlashCSNum){
  unsigned char stat;
  
  FlashCommand(eFlashCmd_ReadStatus, -1, NULL, &stat, 1, FlashCSNum);
  return stat;
}

static bool FlashWriteEnable(enum eFlashCSTYPE FlashCSNum){
  for(int i=0; i<4; i++){
    FlashSendCmd(eFlashCmd_WriteEnable, FlashCSNum);
    if(FlashReadStatus(FlashCSNum) & 0x02){
      return true;
    }
  }
  return false;
}

#define FlashWriteDisable(x)	    FlashSendCmd(eFlashCmd_WriteDisable, x)

static void WaitFlashReady(bool fast, enum eFlashCSTYPE FlashCSNum)
{
  int ulTimeOutCnt = 0 ;
  unsigned int ulMaxTimeOutCnt = 0;

  ulMaxTimeOutCnt = 120;	// wait max time 120*20ms

  while (FlashReadStatus(FlashCSNum) & 0x01){
    FlashDelayms(20) ;
    if (++ulTimeOutCnt > ulMaxTimeOutCnt)
      break;				// max is 10ms or 2400ms
  }
}

static void FlashWriteStatus(unsigned char stat, enum eFlashCSTYPE FlashCSNum)
{
  FlashWriteEnable(FlashCSNum);
  FlashCommand(eFlashCmd_WriteStatus, -1, &stat, NULL, 1, FlashCSNum);
  WaitFlashReady(true, FlashCSNum);
  FlashWriteDisable(FlashCSNum);
}



static void DisableAllProtection(enum eFlashCSTYPE FlashCSNum)
{
  unsigned char status = FlashReadStatus(FlashCSNum);
  
  if((FlashChip[FlashCSNum].id & 0xFF) == 0x1F){
    /*	Atemal*/
    if((status& 0x0C) == 0x0C){
      if(status& 0x80){
        FlashWriteStatus(0x00, FlashCSNum);
      }
    }
    
    FlashWriteStatus(0x00, FlashCSNum);
    return;
  }
  
  if(!FlashWriteEnable(FlashCSNum)){
    return;
  }
  
  status = 0x40;
  FlashCommand(eFlashCmd_WriteStatus , -1 , &status , NULL , 1, FlashCSNum);
}

static void FlashRepair(enum eFlashCSTYPE FlashCSNum)
{
  DisableAllProtection(FlashCSNum);
}

static uint32_t FlashScan(enum eFlashCSTYPE FlashCSNum)
{
  uint32_t id;
  
  id = getFlashID(FlashCSNum);
  
  if( id == 0xffffffff || id == 0 ){
    id = getFlashID(FlashCSNum);
  }
  
  for(int i=0; data_flash_ids[i].name != NULL ; i++){
    
    if( ( id&data_flash_ids[i].idmask ) != data_flash_ids[i].id ){
      continue;
    }
    
    FlashChip[FlashCSNum].id = id;
    FlashChip[FlashCSNum].chipsize = data_flash_ids[i].chipsize;
    FlashChip[FlashCSNum].opt = data_flash_ids[i].options;
    
    FlashRepair(FlashCSNum);
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("%s\t %s\t%d\r\n",__FUNCTION__,data_flash_ids[i].name,FlashCSNum);
    #endif   
   
    break;
  } 

  return id;
}

/******************getFlashID*********************/
uint32_t getFlashID(enum eFlashCSTYPE FlashCSNum)
{
  uint32_t id = 0;
  
  FlashCommand(eFlashCmd_DeviceID, -1, NULL, &id, 4, FlashCSNum);
  
  return id;
}

/*Read Flash*/
int FlashRead(unsigned int addr, void *buffer, int len, enum eFlashCSTYPE FlashCSNum)
{
  if(addr + len > FlashChip[FlashCSNum].chipsize)
    return 0;
  
  FlashCommand(eFlashCmd_Read, addr, NULL, buffer, len, FlashCSNum);
  
  return len;
}

/*Write Flash*/
int FlashWrite(unsigned int addr, void *data, int len, enum eFlashCSTYPE FlashCSNum)
{
  
  int slen;
  int rlen = len;
  int off;
  unsigned char *pDat = (unsigned char *)data;
  
  if(addr + len > FlashChip[FlashCSNum].chipsize)
    return 0;
  
  if(FlashChip[FlashCSNum].opt){
    for(int i=0; i<len; i++){
      if(!FlashWriteEnable(FlashCSNum))
        return 0;
      FlashCommand(eFlashCmd_Write, addr+i, &pDat[i], NULL, 1, FlashCSNum);
    }
    
   return len;
  }
  
  while(len){
    off = addr %256;
    if((off + len) > 256){
      slen = 256 - off;
    }else{
      slen = len;
    }
    
    FlashCommand(eFlashCmd_Write, addr, pDat, NULL, slen, FlashCSNum);
    addr += slen;
    len -= slen;
    pDat += slen;
  }
  
  return rlen;
}

bool FlashEraseMemory(unsigned int address , unsigned char command , unsigned int waitms, enum eFlashCSTYPE FlashCSNum)
{
   FlashCommand(command, address, NULL, NULL, 0, FlashCSNum);
   
   if(waitms != 0){
      FlashDelayms(waitms);
   }
   
   while(FlashReadStatus(FlashCSNum) & 0x01){
    FlashDelayms(1);
   }
   
   for(int i=0; i<100; i++){
     __ASM("nop");
   }
   
   
   isErasing = false;
   
   return true;
}

/*
 *	4K
 */
bool FlashErase4K( unsigned int address, enum eFlashCSTYPE FlashCSNum){
  if(address & FLASHSECTORMASK){
    return false ;
  }

  FlashEraseMemory( address , eFlashCmd_Erase4K , 0, FlashCSNum) ;
  
  return true;
}

/*
 *	32K
 */
bool FlashErase32K( unsigned int address, enum eFlashCSTYPE FlashCSNum){
  if(address & FLASHSECTOR32KMASK){
    return false ;
  }

  FlashEraseMemory( address , eFlashCmd_Erase32K , 0, FlashCSNum);
  
  return true;
}

/*
 *	64K
 */
bool FlashErase64K( unsigned int address, enum eFlashCSTYPE FlashCSNum)
{
  if(address & FLASHSECTOR64KMASK){
    return false ;
  }

  FlashEraseMemory( address , eFlashCmd_Erase64K, 0, FlashCSNum) ;
  
  return true;
}

bool FlashEraseChip(enum eFlashCSTYPE FlashCSNum)
{
  unsigned int addr = 0;

  while( addr < FlashChip[FlashCSNum].chipsize ){
    if ( !FlashErase64K(addr, FlashCSNum) )
      return false;
    addr += FLASHSECTOR64KSIZE ;
  }
	
  return true ;
}


void Flash_Init(void)
{
  extern void FlashPortInit(void);
  uint8_t validCnt = 0;
  FlashPortInit(); 
  
  memset(&FlashChip, 0, FLASH_CS_NUM * sizeof(FlashChip));
  do{
    for(enum eFlashCSTYPE eFlashID = eFLASH_ID_CS0; eFlashID < FLASH_CS_NUM; eFlashID++){

      if(0 == FlashChip[eFlashID].id){ /*未初始化*/
        FlashScan(eFlashID);
        FlashDelayms(2);
      }
      
      if(0 == FlashChip[eFlashID].id){ 
        /*未初始化成功,重新初始化*/
        eFlashID = eFLASH_ID_CS0;
        validCnt = eFlashID;
      }else{
        validCnt ++;
      }    
    }
  }while(validCnt < FLASH_CS_NUM);
  
}
