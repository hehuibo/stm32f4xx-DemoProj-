/*
 ============================================================================
 Name        : Flash.c 
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "Flash.h"

//#pragma location = "AHB_RAM_MEMORY"
static struct _tag_DataFlash FlashChip[FLASH_CS_NUM];

extern const pfFlashTxRxFuncTYPE pfFlashTxRxAryTbl;
extern const FlashCSCtrlValTYPE gsFlashCSCtrlValAryTbl[FLASH_CS_NUM];

#if defined (FreeRTOS_Kernel) 
extern const pfFlashSpiFuncTYPE pfFlashSpiLockFuncAryTbl;
extern const pfFlashSpiFuncTYPE pfFlashSpiUnLockFuncAryTbl;
#endif

/*********************************************/
/*
 *ÕýÔÚ²Á³ýÖÐ
 */
volatile bool isErasing;

struct _tag_DataFlash* getCurrFlash(enum eFlashCSTYPE FlashCSNum)
{
  return &FlashChip[FlashCSNum];
}

static unsigned char Flash_RD_SR(enum eFlashCSTYPE FlashCSNum)
{
  unsigned char ret;

  GPIO_ResetBits(gsFlashCSCtrlValAryTbl[FlashCSNum].mGPIOx, gsFlashCSCtrlValAryTbl[FlashCSNum].mBasePin);
  pfFlashTxRxAryTbl[FlashCSNum](iFlashCmd_ReadStatus);
  ret = pfFlashTxRxAryTbl[FlashCSNum](eFlash_DummyByte);
  GPIO_SetBits(gsFlashCSCtrlValAryTbl[FlashCSNum].mGPIOx, gsFlashCSCtrlValAryTbl[FlashCSNum].mBasePin);

  return ret;
}

static void Flash_WR_Enable(enum eFlashCSTYPE FlashCSNum)
{
  GPIO_ResetBits(gsFlashCSCtrlValAryTbl[FlashCSNum].mGPIOx, gsFlashCSCtrlValAryTbl[FlashCSNum].mBasePin);
  pfFlashTxRxAryTbl[FlashCSNum](iFlashCmd_WriteEnable);
  GPIO_SetBits(gsFlashCSCtrlValAryTbl[FlashCSNum].mGPIOx, gsFlashCSCtrlValAryTbl[FlashCSNum].mBasePin);

}

/*
 *FlashCmd  proc
 */
signed char FlashCommand(unsigned char cmd, int addr, const void *indat, void *outdat, uint32_t len,enum eFlashCSTYPE FlashCSNum)
{
  unsigned char *inbuf = (unsigned char*)indat;
  unsigned char *outbuf = (unsigned char*)outdat;
	
  while(1){
#if defined (FreeRTOS_Kernel) 
    pfFlashSpiLockFuncAryTbl[FlashCSNum]();
#endif    
    if(!isErasing){
      /*Idle*/
      if(cmd == iFlashCmd_Erase4K  ||
         cmd == iFlashCmd_Erase32K ||
         cmd == iFlashCmd_Erase64K ){
          isErasing = true;
        }
       break;
     }
     else if(cmd == iFlashCmd_WriteEnable ||
             cmd == iFlashCmd_WriteDisable||
             cmd == iFlashCmd_ReadStatus){
        break;
      }
    if(eFLASH_ID_CS2 == FlashCSNum){
    
    }else{
#if defined (FreeRTOS_Kernel) 
     pfFlashSpiUnLockFuncAryTbl[FlashCSNum]();
     SPI_Wait();
#endif
    }		

  }/*end while*/
	
  switch(cmd){
    case iFlashCmd_Erase32K :
    case iFlashCmd_Erase4K :
    case iFlashCmd_Erase64K :
    case iFlashCmd_EraseAll :
    case iFlashCmd_Write :
      do{
        Flash_WR_Enable(FlashCSNum);

        /*delay*/
       for(int i=0; i<100; i++){
          __ASM("nop");
       }
        unsigned char stat;
        do{		
          stat = Flash_RD_SR(FlashCSNum);
        }while((stat & 2)== 0);

      }while(false);
      
      break;
  }/*switch*/
  
  GPIO_ResetBits(gsFlashCSCtrlValAryTbl[FlashCSNum].mGPIOx, gsFlashCSCtrlValAryTbl[FlashCSNum].mBasePin);
  //Flash_SetSpeed();
  pfFlashTxRxAryTbl[FlashCSNum](cmd);
  if(addr != -1){
    pfFlashTxRxAryTbl[FlashCSNum]((addr>>16)&0xFF);
    pfFlashTxRxAryTbl[FlashCSNum]((addr>>8)&0xFF);
    pfFlashTxRxAryTbl[FlashCSNum](addr&0xFF);
  }

  while(len-- > 0){
    unsigned char tmp;
    if(inbuf){
      tmp = *inbuf++;
    }else{
      tmp = 0;
    }

    tmp = pfFlashTxRxAryTbl[FlashCSNum](tmp);
    if(outbuf){
      *outbuf++ = tmp;
    }
  }/*while(len-- > 0)*/

  GPIO_SetBits(gsFlashCSCtrlValAryTbl[FlashCSNum].mGPIOx, gsFlashCSCtrlValAryTbl[FlashCSNum].mBasePin);
 
  switch(cmd){
    case iFlashCmd_Write :{
      for(int i=0; i<100; i++){
        __ASM("nop");
      }

      unsigned char stat;
      do{		
        stat = Flash_RD_SR(FlashCSNum);
      }while((stat & 1));

      for(int i=0; i<100; i++){
        __ASM("nop");
      }
    }
    break;

    default:
      break;
  }
  //SET_SPI_SPEED_HIGH();
#if defined (FreeRTOS_Kernel) 
  pfFlashSpiUnLockFuncAryTbl[FlashCSNum]();
#endif
  
  return 0;
}

uint32_t getFlashID(enum eFlashCSTYPE FlashCSNum)
{
  uint32_t id = 0;
  
  FlashCommand(iFlashCmd_DeviceID, -1, NULL, &id, 4, FlashCSNum);
  
  return id;
}

static void FlashSendCmd(unsigned char cmd, enum eFlashCSTYPE FlashCSNum){
  FlashCommand(cmd, -1, NULL, NULL, 0, FlashCSNum);
}

unsigned char FlashReadStatus(enum eFlashCSTYPE FlashCSNum){
  unsigned char stat;
  
  FlashCommand(iFlashCmd_ReadStatus, -1, NULL, &stat, 1, FlashCSNum);
  return stat;
}

static bool FlashWriteEnable(enum eFlashCSTYPE FlashCSNum){
  for(int i=0; i<4; i++){
    FlashSendCmd(iFlashCmd_WriteEnable, FlashCSNum);
    if(FlashReadStatus(FlashCSNum) & 0x02){
      return true;
    }
  }
  return false;
}

#define FlashWriteDisable(x)	    FlashSendCmd(iFlashCmd_WriteDisable, x)

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
  FlashCommand(iFlashCmd_WriteStatus, -1, &stat, NULL, 1, FlashCSNum);
  WaitFlashReady(true, FlashCSNum);
  FlashWriteDisable(FlashCSNum);
}


/*Read Flash*/
int FlashRead(unsigned int addr, void *buffer, int len, enum eFlashCSTYPE FlashCSNum){
  if(addr + len > FlashChip[FlashCSNum].chipsize)
    return 0;
  
  FlashCommand(iFlashCmd_Read, addr, NULL, buffer, len, FlashCSNum);
  
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
      FlashCommand(iFlashCmd_Write, addr+i, &pDat[i], NULL, 1, FlashCSNum);
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
    
    FlashCommand(iFlashCmd_Write, addr, pDat, NULL, slen, FlashCSNum);
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

  FlashEraseMemory( address , iFlashCmd_Erase4K , 0, FlashCSNum) ;
  
  return true;
}

/*
 *	32K
 */
bool FlashErase32K( unsigned int address, enum eFlashCSTYPE FlashCSNum){
  if(address & FLASHSECTOR32KMASK){
    return false ;
  }

  FlashEraseMemory( address , iFlashCmd_Erase32K , 0, FlashCSNum);
  
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

  FlashEraseMemory( address , iFlashCmd_Erase64K, 0, FlashCSNum) ;
  
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
  FlashCommand(iFlashCmd_WriteStatus , -1 , &status , NULL , 1, FlashCSNum);
}

static void FlashRepair(enum eFlashCSTYPE FlashCSNum)
{
  DisableAllProtection(FlashCSNum);
}

uint32_t FlashScan(enum eFlashCSTYPE FlashCSNum)
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



extern void FlashPortInit(void);
void Flash_Init(void){
  unsigned char i = 3;
  FlashPortInit(); 
  
  memset(&FlashChip, 0, FLASH_CS_NUM * sizeof(FlashChip));
 
  do{
    if(!FlashChip[eFLASH_ID_CS0].id){
      FlashScan(eFLASH_ID_CS0);
    }
    
    if((FlashChip[eFLASH_ID_CS0].id)){
      break;
    }
    
  }while(--i);
  
}
