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
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       RFID_FIFO_MAXLENGTH                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
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

//MF522������
/////////////////////////////////////////////////////////////////////
enum eMF522_CMD_TYPE
{
  PCD_IDLE       = 0x00,               //ȡ����ǰ����
  PCD_AUTHENT    = 0x0E,               //��֤��Կ
  PCD_RECEIVE    = 0x08,               //��������
  PCD_TRANSMIT   = 0x04,               //��������
  PCD_TRANSCEIVE = 0x0C,               //���Ͳ���������
  PCD_RESETPHASE = 0x0F,               //��λ
  PCD_CALCCRC    = 0x03,               //CRC����
};


/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t reg, uint8_t mask){
  uint8_t tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t reg, uint8_t mask){
  uint8_t tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp | mask);  // set bit mask
}


//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
void PcdAntennaOn(void){
  uint8_t i;
  i = ReadRawRC(TxControlReg);
  if (!(i & 0x03)){
    SetBitMask(TxControlReg, 0x03);
  }
}

//�ر�����
void PcdAntennaOff(void)
{
  ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
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
//��MF522����CRC16����
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
//����RC522�Ĺ�����ʽ 
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
��ʼ��RC522
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
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)CPU��
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�RATS
/////////////////////////////////////////////////////////////////////
char PcdRATS(uint8_t *pOutBfr, uint8_t *pOutLen)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ClearBitMask(Status2Reg,0x08);	// ���У��ɹ���־,���MFCrypto1Onλ

  memset(ucComMF522Buf, 0, MAXRLEN);

  ucComMF522Buf[0] = PICC_RATS;		
  ucComMF522Buf[1] = 0x51;				

  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�

  if(pOutBfr){
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,pOutBfr,&unLen);// ���յ��Ŀ�Ƭ���ͺű���
  }else{
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);// ���յ��Ŀ�Ƭ���ͺű���
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
			 
  return status;								//���ؽ��
}

char PcdComCmdPro(uint8_t mode, uint8_t *pInBfr, uint8_t inLen, uint8_t *pOutBfr, uint8_t *pOutLen)
{
  char status;
  unsigned int  unLen = inLen;
 // ClearBitMask(Status2Reg,0x08);	// ���У��ɹ���־,���MFCrypto1Onλ
  PcdSwitchPCB();
  pInBfr[0] = gcRfidPcdPCB;		
  pInBfr[1] = 0x01;
  unLen += 2;
  CalulateCRC(pInBfr, unLen,&pInBfr[unLen]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE("%d \r\n", unLen);
    for(int i=0; i<(unLen); i++){
      dbgTRACE("%0.2x ", pInBfr[i]);
    }
    dbgTRACE("\r\n");
  #endif
  
  status = PcdComMF522(PCD_TRANSCEIVE, pInBfr, unLen + 2, pOutBfr, &unLen);// ���յ��Ŀ�Ƭ���ͺű���
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

