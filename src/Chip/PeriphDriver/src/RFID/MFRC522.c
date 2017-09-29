/*
 ============================================================================
 Name        : MFRC522.c 
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "RFID\MFRC522.h"
#if defined(RFID_CHIP_RC522)

#if(MFRC_ULTRALPRO_CPU > 0)    
extern uint8_t gcRfidPcdPCB;
void PcdSwitchPCB(void);
#endif

/*****************************************************************/
extern void MFRC_GPIOConfigure(void);
extern uint8_t ReadRawRC(uint8_t Address);
extern void WriteRawRC(uint8_t Address, uint8_t value);
extern void MFRC_Delay(uint16_t mDlyTime);
/*****************************************************************/

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       RFID_FIFO_MAXLENGTH                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  0x3F

//MF522命令字
/////////////////////////////////////////////////////////////////////
enum eMF522_CMD_TYPE
{
  PCD_IDLE       = 0x00,               //取消当前命令
  PCD_AUTHENT    = 0x0E,               //验证密钥
  PCD_RECEIVE    = 0x08,               //接收数据
  PCD_TRANSMIT   = 0x04,               //发送数据
  PCD_TRANSCEIVE = 0x0C,               //发送并接收数据
  PCD_RESETPHASE = 0x0F,               //复位
  PCD_CALCCRC    = 0x03,               //CRC计算
};


/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t reg, uint8_t mask){
  uint8_t tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t reg, uint8_t mask){
  uint8_t tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp | mask);  // set bit mask
}


//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
void PcdAntennaOn(void){
  uint8_t i;
  i = ReadRawRC(TxControlReg);
  if (!(i & 0x03)){
    SetBitMask(TxControlReg, 0x03);
  }
}

//关闭天线
void PcdAntennaOff(void)
{
  ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
#define MAXRLEN                       18
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
  char status = MI_ERR;
  unsigned char irqEn   = 0x00;
  unsigned char waitFor = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int i;

  switch (Command){
  case PCD_AUTHENT:
    irqEn = 0x12;
    waitFor = 0x10;
    break;
  case PCD_TRANSCEIVE:
    irqEn = 0x77;
    waitFor = 0x30;
    break;
  default:
    break;
  }

  WriteRawRC(ComIEnReg, irqEn | 0x80);
  ClearBitMask(ComIrqReg, 0x80);
  WriteRawRC(CommandReg, PCD_IDLE);
  SetBitMask(FIFOLevelReg, 0x80);

  for (i = 0; i < InLenByte; i++){
    WriteRawRC(FIFODataReg, pInData[i]);
  }
  WriteRawRC(CommandReg, Command);

  if (Command == PCD_TRANSCEIVE){
    SetBitMask(BitFramingReg, 0x80);
  }

  i = 800; 
  do{
    n = ReadRawRC(ComIrqReg);
    MFRC_Delay(10);
    i--;
  } while ((i != 0) && !(n & 0x01) && !(n & waitFor));
  ClearBitMask(BitFramingReg, 0x80);

  if (i != 0){ 
    if (!(ReadRawRC(ErrorReg) & 0x1B)){
      status = MI_OK;
      if (n & irqEn & 0x01){
        status = MI_NOTAGERR;
      }
      if (Command == PCD_TRANSCEIVE){
        n = ReadRawRC(FIFOLevelReg);
        lastBits = ReadRawRC(ControlReg) & 0x07;
        if (lastBits){
          *pOutLenBit = (n - 1) * 8 + lastBits;
        }else{
          *pOutLenBit = n * 8; //OK
        }
        
        if (n == 0){
          n = 1;
        }
        
        if (n > MAXRLEN){
          n = MAXRLEN;
        }
        for (i = 0; i < n; i++){
          pOutData[i] = ReadRawRC(FIFODataReg); //oK
        }
      }
    }else{
      status = MI_ERR;
    }
  }
  
  SetBitMask(ControlReg, 0x80);           // stop timer now
  WriteRawRC(CommandReg, PCD_IDLE);
  return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{   
  MFRC_Delay(500);  
  WriteRawRC(CommandReg, PCD_RESETPHASE);
  MFRC_Delay(2000);  

  WriteRawRC(ModeReg, 0x3D);            
  WriteRawRC(TReloadRegL, 30);             
  WriteRawRC(TReloadRegH, 0);
  WriteRawRC(TModeReg, 0x8D);
  WriteRawRC(TPrescalerReg, 0x3E);
  WriteRawRC(TxAutoReg, 0x40);

  ClearBitMask(TestPinEnReg, 0x80);         //off MX and DTRQ out
 // WriteRawRC(TxAutoReg, 0x40);
  MFRC_Delay(2000);  
  return MI_OK;
}	

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
  unsigned char i,n;

  ClearBitMask(DivIrqReg,0x04);
  WriteRawRC(CommandReg,PCD_IDLE);
  SetBitMask(FIFOLevelReg,0x80);
  
  for (i=0; i<len; i++){   
    WriteRawRC(FIFODataReg, *(pIndata+i));  
  }
  
  WriteRawRC(CommandReg, PCD_CALCCRC);
  i = 0xFF;
  do {
    n = ReadRawRC(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));
  pOutData[0] = ReadRawRC(CRCResultRegL);
  pOutData[1] = ReadRawRC(CRCResultRegM);
}


//////////////////////////////////////////////////////////////////////
//设置RC522的工作方式 
//////////////////////////////////////////////////////////////////////
signed char M500PcdConfigISOType(unsigned char type)
{
  if('A' == type){
    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(ModeReg,0x3D);	
    WriteRawRC(RxSelReg,0x86);
    WriteRawRC(RFCfgReg,0x7F);   
    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    MFRC_Delay(10000);
    PcdAntennaOn();
    MFRC_Delay(100);  
  }else{
    return -1;
  }
	
  return MI_OK;
}

/***
初始化RC522
*/

void MFRC522_Init(void)
{
  MFRC_GPIOConfigure();

  PcdReset();
  PcdAntennaOff();
  MFRC_Delay(2000);
  //PcdAntennaOn();
  M500PcdConfigISOType('A');
}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)CPU卡
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(uint8_t req_code, uint8_t *pTagType)
{
  char status;  
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ClearBitMask(Status2Reg, 0x08);
  WriteRawRC(BitFramingReg, 0x07);
  SetBitMask(TxControlReg, 0x03);

  ucComMF522Buf[0] = req_code;

  status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);

  if ((status == MI_OK) && (unLen == 0x10)){
    *pTagType = ucComMF522Buf[0];
    *(pTagType + 1) = ucComMF522Buf[1];
  }else{
    status = MI_ERR;
  }

  return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
  char status;
  unsigned char i,snr_check=0;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ClearBitMask(Status2Reg,0x08);
  WriteRawRC(BitFramingReg,0x00);
  ClearBitMask(CollReg,0x80);

  ucComMF522Buf[0] = PICC_ANTICOLL1;
  ucComMF522Buf[1] = 0x20;

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

  if (status == MI_OK){
    for (i=0; i<4; i++){   
      *(pSnr+i)  = ucComMF522Buf[i];
      snr_check ^= ucComMF522Buf[i];
   }
   
   if (snr_check != ucComMF522Buf[i]){ 
     status = MI_ERR;   
   }
  }

  SetBitMask(CollReg,0x80);
  return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr){
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_SELECT;
  ucComMF522Buf[1] = 0x70;
  ucComMF522Buf[6] = 0;
  for (int i=0; i<4; i++){
    ucComMF522Buf[i+2] = *(pSnr+i);
    ucComMF522Buf[6]  ^= *(pSnr+i);
  }
  CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

  ClearBitMask(Status2Reg,0x08);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

  if ((status == MI_OK) && (unLen == 0x18)){  
    status = MI_OK;  
  }else{  
    status = MI_ERR;    
  }

  return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void){
  //    int status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_HALT;
  ucComMF522Buf[1] = 0;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

  //status =
  PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr){
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = auth_mode;
  ucComMF522Buf[1] = addr;
  
  for (int i=0; i<6; i++){    
    ucComMF522Buf[i+2] = *(pKey+i);  
  }
  
  for (int i=0; i<6; i++){    
    ucComMF522Buf[i+8] = *(pSnr+i);   
  }
  //memcpy(&ucComMF522Buf[2], pKey, 6); 
  //memcpy(&ucComMF522Buf[8], pSnr, 4); 

  status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
  if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08))){  
    status = MI_ERR;   
  }

  return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData){
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_READ;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  if ((status == MI_OK) && (unLen == 0x90)){   
    memcpy(pData, ucComMF522Buf, 16);   
  }else{   
    status = MI_ERR;  
  }

  return status;
 }

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr,unsigned char *pData){
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_WRITE;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)){  
    status = MI_ERR;   
  }

  if (status == MI_OK){
    //memset(ucComMF522Buf, 0, 16);
    memcpy(ucComMF522Buf, pData, 16);

    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)){   
      status = MI_ERR;  
    }
  }

  return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue){
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = dd_mode;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)){   
    status = MI_ERR;  
  }

  if (status == MI_OK){
    memcpy(ucComMF522Buf, pValue, 4);

    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
    unLen = 0;
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {    
      status = MI_OK;    
    }
  }

  if (status == MI_OK){
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)){   
      status = MI_ERR;  
    }
  }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_RESTORE;
  ucComMF522Buf[1] = sourceaddr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)){   
    status = MI_ERR;   
  }

  if (status == MI_OK){
	memset(ucComMF522Buf, 0, 4);
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR){
      status = MI_OK;   
    }
  }

  if (status != MI_OK){
    return MI_ERR;   
  }

  ucComMF522Buf[0] = PICC_TRANSFER;
  ucComMF522Buf[1] = goaladdr;

  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   
    status = MI_ERR;  
  }

  return status;
}

#if(MFRC_ULTRALPRO_CPU > 0) 
/////////////////////////////////////////////////////////////////////
//功    能：RATS
/////////////////////////////////////////////////////////////////////
char PcdRATS(uint8_t *pOutBfr, uint8_t *pOutLen)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ClearBitMask(Status2Reg,0x08);	// 清空校验成功标志,清除MFCrypto1On位

  memset(ucComMF522Buf, 0, MAXRLEN);

  ucComMF522Buf[0] = PICC_RATS;		
  ucComMF522Buf[1] = 0x51;				

  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);	// 生成发送内容的CRC校验,保存到最后两个字节

  if(pOutBfr){
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,pOutBfr,&unLen);// 将收到的卡片类型号保存
  }else{
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);// 将收到的卡片类型号保存
  }
  
  if (status == MI_OK){
      if(pOutLen){
        *pOutLen = unLen/8  -2;
      }
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("%d \r\n", unLen);
      for(int i=0; i<*pOutLen; i++){
        if(pOutBfr){
          dbgTRACE("%0.2x ", pOutBfr[i]);
        }else{
          dbgTRACE("%0.2x ", ucComMF522Buf[i]);
        }
      }
      dbgTRACE("\r\n");
      #endif

    status = MI_OK;
  }	 
  else{
    status = MI_ERR;
  }
			 
  return status;								//返回结果
}

char PcdComCmdPro(uint8_t mode, uint8_t *pInBfr, uint8_t inLen, uint8_t *pOutBfr, uint8_t *pOutLen)
{
  char status;
  unsigned int  unLen = inLen;
 // ClearBitMask(Status2Reg,0x08);	// 清空校验成功标志,清除MFCrypto1On位
  PcdSwitchPCB();
  pInBfr[0] = gcRfidPcdPCB;		
  pInBfr[1] = 0x01;
  unLen += 2;
  CalulateCRC(pInBfr, unLen,&pInBfr[unLen]);	// 生成发送内容的CRC校验,保存到最后两个字节
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("%d \r\n", unLen);
    for(int i=0; i<(unLen); i++){
      dbgTRACE("%0.2x ", pInBfr[i]);
    }
    dbgTRACE("\r\n");
  #endif
  
  status = PcdComMF522(PCD_TRANSCEIVE, pInBfr, unLen + 2, pOutBfr, &unLen);// 将收到的卡片类型号保存
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("%d \r\n", unLen);
    for(int i=0; i<(unLen); i++){
      dbgTRACE("%0.2x ", pOutBfr[i]);
    }
    dbgTRACE("\r\n");
  #endif
  if (status == MI_OK){
    *pOutLen = unLen;
    status = MI_OK;
  }	 
  else{
    status = MI_ERR;
  }
	
  return status;
}

#endif

#endif

