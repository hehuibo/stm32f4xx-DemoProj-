/*
 ============================================================================
 Name        : xx.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "IAP.h"
#include "StartUpCfg.h"

pfAppTYPE JumpToApp;
uint32_t JumpAddress;

enum _tag_eAppCmdTYPE
{
  eAppCmd_UpdateMode     = 0xF4, /*升级模式*/
  eAppCmd_UpdateApp      = 0xF5, /*下载*/
  eAppCmd_UpdateCheckApp = 0xF6, /*App检查*/
  eAppCmd_UpdateDone     = 0xF7, /*升级成功*/
  eAppCmd_Call           = 0xFA,  /*搜机命令*/
  eAppCmd_Conn           = 0xF8,  /*连接*/
};

static uint32_t appProgramAddr = APP_START_ADDRESS;

void vProgramAddrReset(void)
{
  appProgramAddr = APP_START_ADDRESS;
}

uint16_t getFrameCrc(const uint8_t *pData, uint16_t len)
{
  uint16_t crc = 0;
  for(int i=0; i<len; i++){
    crc += pData[i];
  }
  
  return crc;
}

void vProgramData(const uint8_t *pData, uint16_t len)
{
  uint16_t Count = len/4;
  if(len % 4 != 0)
      Count ++;
  FLASH_Unlock();
  STFLASH_ProgramData(&appProgramAddr, (uint32_t *)pData, Count);
  FLASH_Lock();
}

/*len(2)  +  Dev(2) + mCmd(1) + Data(n) + Crc(2) + 0xda*/
eErrTYPE  ProgramFrameMnt(struct _tag_AppCommBufferTYPE *pAppEnv){
  const uint8_t * rbuf = pAppEnv->pRxBfr;
  uint16_t rlen = pAppEnv->mRxLen; 
  eErrTYPE err = eERR_ERROR;

  rlen -= 4;  /*Crc + 0xda*/
  
  memcpy(pAppEnv->pTxBfr, pAppEnv->pRxBfr, 5);
  pAppEnv->mTxLen = 5;
  pAppEnv->pTxBfr[pAppEnv->mTxLen ++ ] = err;
  
  /*Crc 校验*/ 
  uint16_t FrameCrc = 0;
  memcpy(&FrameCrc, &rbuf[rlen], 2);
  if(getFrameCrc(rbuf, rlen) != FrameCrc){
    memcpy(pAppEnv->pTxBfr, &pAppEnv->mTxLen, 2);
    FrameCrc = getFrameCrc(pAppEnv->pTxBfr, pAppEnv->mTxLen);
    memcpy(&pAppEnv->pTxBfr[pAppEnv->mTxLen], &FrameCrc, 2);
    pAppEnv->mTxLen += 2;;
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("crcerr, cmd: %x\r\n", rbuf[4]);
  #endif  
    return err;
  }
  /*Cmd*/
  uint8_t mCmd = rbuf[4];
  rlen -= 5;  /*len + Dev+ Cmd*/
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("cmd: %x\r\n", mCmd);
  #endif  
  switch(mCmd){
    /*搜机命令*/
    case eAppCmd_Call:{
      /*Ip + Port + DevID*/
      err = eERR_OK;
    }
    break;
    
    /*连接*/
    case eAppCmd_Conn:{
      err = eERR_OK;
    }
    break;
    
    /*升级模式*/
    case eAppCmd_UpdateMode :{
      vProgramAddrReset();
      err = eERR_OK;
    }
    break;
    
    /*App下载*/
    case eAppCmd_UpdateApp :{
      vProgramData(&rbuf[5], rlen);
      err = eERR_OK;
    }
    break;
    
    /*App检查*/
    case eAppCmd_UpdateCheckApp: {
      err = eERR_OK;
    }
    break;
    
    /*App升级成功*/
    case eAppCmd_UpdateDone:{
      ReBoot(iCmdReBoot);
    }
    break;
    
    default :{
    }
    break;
  }
  
  pAppEnv->pTxBfr[5] = err;
  memcpy(pAppEnv->pTxBfr, &pAppEnv->mTxLen, 2);
  FrameCrc = getFrameCrc(pAppEnv->pTxBfr, pAppEnv->mTxLen);
  memcpy(&pAppEnv->pTxBfr[pAppEnv->mTxLen], &FrameCrc, 2);
  pAppEnv->mTxLen += 2;;
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("err = %d, TxLen = %d\r\n", err, pAppEnv->mTxLen);
  #endif
  return err;
}


