/*
 ============================================================================
 Name        : FM17xx.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "RFID\FM17xx.h"

#if defined(RFID_CHIP_FM17xx)

#define BUFMAXRLEN                       18
#define RF_TimeOut		0x7f
/* FM17xx命令码 */
enum eFM17xxCmd_TYPE
{
  eFM17xxCmd_Transceive   = 0x1E,			/* 发送接收命令 */
  eFM17xxCmd_Transmit	  =	0x1a,			/* 发送命令 */
  eFM17xxCmd_ReadE2	      =	0x03,			/* 读FM17xx EEPROM命令 */
  eFM17xxCmd_WriteE2	  =	0x01,			/* 写FM17xx EEPROM命令 */
  eFM17xxCmd_Authent1	  =	0x0c,			/* 验证命令认证过程第1步 */
  eFM17xxCmd_Authent2	  =	0x14,			/* 验证命令认证过程第2步 */
  eFM17xxCmd_LoadKeyE2    = 0x0b,			/* 将密钥从EEPROM复制到KEY缓存 */
  eFM17xxCmd_LoadKey	  = 0x19,			/* 将密钥从FIFO缓存复制到KEY缓存 */
};

//FM17xx Reg
#define Page_Sel				0x00	/* 页写寄存器 */
#define Command					0x01	/* 命令寄存器 */
#define FIFO					0x02	/* 64字节FIFO缓冲的输入输出寄存器 */
#define PrimaryStatus			0x03	/* 发射器接收器及FIFO的状态寄存器1 */
#define FIFO_Length				0x04	/* 当前FIFO内字节数寄存器 */
#define SecondaryStatus			0x05	/* 各种状态寄存器2 */
#define InterruptEn				0x06	/* 中断使能/禁止寄存器 */
#define Int_Req					0x07	/* 中断请求标识寄存器 */
#define Control					0x09	/* 控制寄存器 */
#define ErrorFlag				0x0A	/* 错误状态寄存器 */
#define CollPos					0x0B	/* 冲突检测寄存器 */
#define TimerValue				0x0c	/* 定时器当前值 */
#define Bit_Frame				0x0F	/* 位帧调整寄存器 */
#define TxControl				0x11	/* 发送控制寄存器 */
#define CWConductance			0x12	/* 选择发射脚TX1和TX2发射天线的阻抗 */
#define ModConductance			0x13	/* 定义输出驱动阻抗 */
#define CoderControl			0x14	/* 定义编码模式和时钟频率 */
#define TypeBFraming			0x17	/* 定义ISO14443B帧格式 */
#define DecoderControl			0x1a	/* 解码控制寄存器 */
#define RxControl2				0x1e	/* 解码控制及选择接收源 */
#define RxWait					0x21	/* 选择发射和接收之间的时间间隔 */
#define ChannelRedundancy		0x22	/* RF通道检验模式设置寄存器 */
#define CRCPresetLSB			0x23
#define CRCPresetMSB			0x24
#define MFOUTSelect				0x26	/* mf OUT 选择配置寄存器 */
#define TimerClock				0x2a	/* 定时器周期设置寄存器 */
#define TimerControl			0x2b	/* 定时器控制寄存器 */
#define TimerReload				0x2c	/* 定时器初值寄存器 */
#define TypeSH					0x31	/* 上海标准选择寄存器 */
#define TestDigiSelect			0x3d	/* 测试管脚配置寄存器 */

char FM17xx_BusSel(void){

   for(uint8_t i=0; i<RF_TimeOut; i++){
    if(0x00 == ReadRawRC(Command)){
      WriteRawRC(Page_Sel, 0x80);
      for(uint8_t i=0; i<RF_TimeOut; i++){
        if(0x00 == ReadRawRC(Command)){
          WriteRawRC(Page_Sel, 0x00);
          return MI_OK;
        }
      
      } 
    }
  } 
  return MI_ERR;
}


uint8_t FM17xx_ClearFIFO(void){
  
  uint8_t temp;
  
  temp = ReadRawRC(Control);						
  temp = (temp | 0x01);
  WriteRawRC(Control,temp);
  
  for(uint8_t i = 0; i < RF_TimeOut; i++){
    if(ReadRawRC(FIFO_Length) == 0){
      return MI_OK;
    }
  }
  return MI_ERR;
}

void FM17xx_WriteFIFO(uint8_t count,uint8_t *buff){
  for( uint8_t i = 0; i < count; i++){
    WriteRawRC(FIFO,*(buff + i));
  }
}


uint8_t FM17xx_ReadFIFO(uint8_t *buff){
  uint8_t temp;
 
  temp =ReadRawRC(FIFO_Length);
  if (temp == 0){
    return 0;
  }
  
  if (temp >= 24){									       
    //temp=255时,会进入死循环 ,因此增加FIFO_Length越限判断
    temp = 24;						 
  }
  
  for( uint8_t i = 0; i < temp; i++){
    *(buff + i) = ReadRawRC(FIFO);
  }
  
  return temp;
}


uint8_t FM17xx_JudgeReq(uint8_t *buff){
  uint8_t temp1,temp2;

  temp1 = *buff;
  temp2 = *(buff + 1);

  if((temp1 == 0x02) || (temp1 == 0x04) || (temp1 == 0x05) || (temp1 == 0x53) || (temp1 == 0x03)){
    if(temp2 == 0x00){
      return MI_OK;
    }
  }
  return MI_ERR;
}

void FM17xx_SaveUID(uint8_t row, uint8_t col, uint8_t length, uint8_t *pSnr, uint8_t *buf){
  uint8_t	temp;
  uint8_t	temp1;

  if((row == 0x00) && (col == 0x00)){
    for(uint8_t	i = 0; i < length; i++){
      pSnr[i] = buf[i];
    }
  }else{
    temp = buf[0];
    temp1 = pSnr[row - 1];
    switch(col){
      case 0:		temp1 = 0x00; row = row + 1; break;
      case 1:		temp = temp & 0xFE; temp1 = temp1 & 0x01; break;
      case 2:		temp = temp & 0xFC; temp1 = temp1 & 0x03; break;
      case 3:		temp = temp & 0xF8; temp1 = temp1 & 0x07; break;
      case 4:		temp = temp & 0xF0; temp1 = temp1 & 0x0F; break;
      case 5:		temp = temp & 0xE0; temp1 = temp1 & 0x1F; break;
      case 6:		temp = temp & 0xC0; temp1 = temp1 & 0x3F; break;
      case 7:		temp = temp & 0x80; temp1 = temp1 & 0x7F; break;
      default:	break;
    }

    buf[0] = temp;
    pSnr[row - 1] = temp1 | temp;
    for(uint8_t	i = 1; i < length; i++){
      pSnr[row - 1 + i] = buf[i];
    }
  }
}

void FM17xx_SetBitFraming(uint8_t row,uint8_t col, uint8_t* buf)
{
  switch(row){
    case 0:		buf[1] = 0x20; break;
    case 1:		buf[1] = 0x30; break;
    case 2:		buf[1] = 0x40; break;
    case 3:		buf[1] = 0x50; break;
    case 4:		buf[1] = 0x60; break;
    default:	break;
  }

  switch(col){
    case 0:		WriteRawRC(0x0F,0x00);  break;
    case 1:		WriteRawRC(0x0F,0x11); buf[1] = (buf[1] | 0x01); break;
    case 2:		WriteRawRC(0x0F,0x22); buf[1] = (buf[1] | 0x02); break;
    case 3:		WriteRawRC(0x0F,0x33); buf[1] = (buf[1] | 0x03); break;
    case 4:		WriteRawRC(0x0F,0x44); buf[1] = (buf[1] | 0x04); break;
    case 5:		WriteRawRC(0x0F,0x55); buf[1] = (buf[1] | 0x05); break;
    case 6:		WriteRawRC(0x0F,0x66); buf[1] = (buf[1] | 0x06); break;
    case 7:		WriteRawRC(0x0F,0x77); buf[1] = (buf[1] | 0x07); break;
    default:	break;
  }
}

uint8_t FM17xx_CheckUID(uint8_t *pSnr)
{
  uint8_t	temp = 0;
  for(uint8_t   i= 0; i < 5; i++){
    temp = temp ^ pSnr[i];
  }
  if(temp == 0){
    return MI_OK;
  }
  return MI_ERR;
}

char FM17xx_M500HostCodeKey(uint8_t *uncoded, uint8_t *coded){
  uint8_t ln  = 0;     
  uint8_t hn  = 0;      

  for (uint8_t cnt = 0; cnt < 6; cnt++){
    ln = uncoded[cnt] & 0x0F;
    hn = uncoded[cnt] >> 4;
    coded[cnt * 2 + 1] = (~ln << 4) | ln;
    coded[cnt * 2 ] = (~hn << 4) | hn;
  }
  return MI_OK;
}


uint8_t FM17xx_CommandSend(uint8_t count, uint8_t * buff, uint8_t Comm_Set){
  WriteRawRC(Command,0x00);
  FM17xx_ClearFIFO();
  
  if (count != 0){
    FM17xx_WriteFIFO(count,buff);
  }	 
  
  ReadRawRC(FIFO_Length);
  WriteRawRC(Command,Comm_Set);	

  for(uint16_t j = 0; j< RF_TimeOut;j++){
    if(0x00 == ReadRawRC(Command)){
     return MI_OK;
    }
  }
  return MI_NOTAGERR;	
}

char FM17xx_Request(uint8_t req_code,uint8_t *pTagType){
  uint8_t  temp;
  uint8_t ucComBuf[BUFMAXRLEN];

  WriteRawRC(TxControl,0x58);
  MFRC_Delay(10);
  WriteRawRC(TxControl,0x5b);	
  WriteRawRC(CWConductance,0x3f);

  memset(ucComBuf, 0, BUFMAXRLEN);
  ucComBuf[0] = req_code;					             //Request模式选择
  WriteRawRC(Bit_Frame,0x07);			       //发送7bit
  WriteRawRC(ChannelRedundancy,0x03);	   //关闭CRC
  WriteRawRC(TxControl,0x5b);
  WriteRawRC(Control,0x01);

  if(MI_OK == FM17xx_CommandSend(1, ucComBuf,  eFM17xxCmd_Transceive)){
    if(FM17xx_ReadFIFO(ucComBuf)){
      temp = FM17xx_JudgeReq(ucComBuf);			        
      if ((temp == MI_OK) &&(4 == ucComBuf[0])){
       pTagType[0] = ucComBuf[0];
       pTagType[1] = ucComBuf[1];
        return MI_OK;
      }
      return MI_REQERR; 
    }				             
  }

  return MI_NOTAGERR;
}

char FM17xx_AntiColl(uint8_t *pSnr){
  uint8_t	row, col;
  uint8_t	pre_row;
  uint8_t	temp;
  uint8_t ucComBuf[BUFMAXRLEN];

  row = 0;
  col = 0;
  pre_row = 0;
  
  WriteRawRC(CRCPresetLSB,0x63);
  WriteRawRC(CWConductance,0x3f);
  WriteRawRC(ModConductance,0x3f);
  WriteRawRC(ChannelRedundancy,0x03);	                   // 关闭CRC,打开奇偶校验
  ucComBuf[0] = PICC_ANTICOLL1;
  ucComBuf[1] = 0x20;
  temp = FM17xx_CommandSend(2, ucComBuf, eFM17xxCmd_Transceive);
  
  while(1){
    if(temp == MI_NOTAGERR){
     return(MI_NOTAGERR);
    }

    temp = ReadRawRC(0x04);
    if(temp == 0){
     return MI_BYTECOUNTERR;
    }

    FM17xx_ReadFIFO(ucComBuf);
    FM17xx_SaveUID(row, col, temp, pSnr, ucComBuf);	// 将收到的UID放入UID数组中

    temp = ReadRawRC(0x0A);				        // 判断接収数据是否出错
    temp = temp & 0x01;
    if(temp == 0x00){	            
      if(FM17xx_CheckUID(pSnr)== MI_ERR){// 校验收到的UID
        return(MI_SERNRERR);
      }
      return(MI_OK);
    }else{
      temp = ReadRawRC(0x0B);             // 读取冲突检测寄存器 
      row = temp / 8;
      col = temp % 8;
      ucComBuf[0] = PICC_ANTICOLL1;
      FM17xx_SetBitFraming(row + pre_row, col, ucComBuf);	// 设置待发送数据的字节数 
      pre_row = pre_row + row;
      for(uint8_t i = 0; i < pre_row + 1; i++){
        ucComBuf[i + 2] = pSnr[i];
      }

      if(col != 0x00){
        row = pre_row + 1;
      }else{
        row = pre_row;
      }
      temp = FM17xx_CommandSend(row + 2, ucComBuf, eFM17xxCmd_Transceive);
    }
  }
}

char FM17xx_Select(uint8_t *pSnr){
  uint8_t   temp;
  uint8_t ucComBuf[BUFMAXRLEN];

  WriteRawRC(CRCPresetLSB,0x63);
  WriteRawRC(CWConductance,0x3f);
  memset(ucComBuf, 0, BUFMAXRLEN);
  ucComBuf[0] = PICC_SELECT;
  ucComBuf[1] = 0x70;
 
  for(uint8_t i = 0; i < 5; i++){
    ucComBuf[i + 2] = pSnr[i];
  }

  WriteRawRC(ChannelRedundancy,0x0f);	                       // 开启CRC,奇偶校验校验 

  if(FM17xx_CommandSend(7, ucComBuf, eFM17xxCmd_Transceive) == MI_OK){
    temp = ReadRawRC(0x0A);
    if((temp & 0x02) == 0x02){
      return(MI_PARITYERR);
    }
    if((temp & 0x04) == 0x04){
      return(MI_FRAMINGERR);
    }  
    if((temp & 0x08) == 0x08) {
      return(MI_CRCERR);
    }
    
    temp = ReadRawRC(0x04);
    if(temp != 1){
      return(MI_BYTECOUNTERR);
    } 

    FM17xx_ReadFIFO(ucComBuf);	                      // 从FIFO中读取应答信息 
    temp = *ucComBuf;
    //判断应答信号是否正确 
    if((temp == 0x08) || (temp == 0x88) || (temp == 0x53) ||(temp == 0x18)||(temp == 0x28)) //S70 temp = 0x18	
      return(MI_OK);
    else
      return(MI_SELERR);
       
  }else{
    return(MI_NOTAGERR);
  }
}

char FM17xx_LoadkeyE2_CPY(uint8_t *uncoded_keys){
  uint8_t coded_keys[13];

  FM17xx_M500HostCodeKey(uncoded_keys, coded_keys);
  FM17xx_CommandSend(12, coded_keys, eFM17xxCmd_LoadKey);

  if ((ReadRawRC(0x0A) & 0x40) == 0x40){
    return MI_ERR;
  }
  return MI_OK;
}


char FM17xx_AuthState(uint8_t auth_mode, uint8_t addr, uint8_t *pKey, uint8_t *pSnr){
  uint8_t	temp, temp1;
  uint8_t ucComBuf[BUFMAXRLEN];
  
  if(FM17xx_LoadkeyE2_CPY(pKey) == MI_OK){
    WriteRawRC(CRCPresetLSB,0x63);
    WriteRawRC(CWConductance,0x3f);
    WriteRawRC(ModConductance,0x3f);
    temp1 = ReadRawRC(Control);
    temp1 = temp1 & 0xf7;
    WriteRawRC(Control,temp1);  
    
    ucComBuf[0] = PICC_AUTHENT1A;
    if(auth_mode == PICC_AUTHENT1B){
      ucComBuf[0] = PICC_AUTHENT1B;
    }			            
    //ucComBuf[1] = addr * 4 + 3;
    ucComBuf[1] = (addr/4) * 4 + 3;
    memcpy(ucComBuf + 2, pSnr, 4);

    WriteRawRC(ChannelRedundancy,0x0f);	                    

    if(FM17xx_CommandSend(6, ucComBuf,  eFM17xxCmd_Authent1) != MI_OK){
      return MI_NOTAGERR;
    }

    temp = ReadRawRC(ErrorFlag);
    if((temp & 0x02) == 0x02){
      return MI_PARITYERR;
    } 
    if((temp & 0x04) == 0x04){
      return MI_FRAMINGERR;
    } 
    if((temp & 0x08) == 0x08){
      return MI_CRCERR;
    } 
     
    if(FM17xx_CommandSend(0, ucComBuf,  eFM17xxCmd_Authent2) != MI_OK){
      return MI_NOTAGERR;
    }

    temp = ReadRawRC(ErrorFlag);
    if((temp & 0x02) == 0x02){
      return MI_PARITYERR;
    }
    if((temp & 0x04) == 0x04){
      return MI_FRAMINGERR;
    } 
    if((temp & 0x08) == 0x08){
       return MI_CRCERR;
    }
    temp1 = ReadRawRC(Control);
    temp1 = temp1 & 0x08;	                    
    if(temp1 == 0x08){
      return MI_OK;
    }
    
  }
  return MI_AUTHERR;
}

char FM17xx_Read(uint8_t Block_Adr, uint8_t *buff)
{
  uint8_t	temp;
  
  WriteRawRC(CRCPresetLSB,0x63);
  WriteRawRC(CWConductance,0x3f);
  WriteRawRC(ModConductance,0x3f);
  WriteRawRC(ChannelRedundancy,0x0f);

  buff[0] = PICC_READ;
  buff[1] = Block_Adr;

  if(FM17xx_CommandSend(2, buff, eFM17xxCmd_Transceive) != MI_OK){
    return MI_NOTAGERR;
  }

  temp = ReadRawRC(ErrorFlag);
  if((temp & 0x02) == 0x02){
    return MI_PARITYERR;
  }
  if((temp & 0x04) == 0x04){
    return MI_FRAMINGERR;
  } 
  if((temp & 0x08) == 0x08){
    return MI_CRCERR;
  }
  
  switch(ReadRawRC(FIFO_Length)){
  case 0x10 :
  case 0x04 :
    FM17xx_ReadFIFO(buff);
    break;
  default :
    return MI_BYTECOUNTERR;
  }
  
  
  return MI_OK;
}

char FM17xx_Write(uint8_t Block_Adr, uint8_t *buff)
{
  uint8_t	temp;
  uint8_t ucComBuf[2];

  WriteRawRC(CRCPresetLSB,0x63);
  WriteRawRC(CWConductance,0x3f);
  WriteRawRC(ChannelRedundancy,0x07);/* Note: this line is for 1702, different from RC500*/
  ucComBuf[0] = PICC_WRITE;
  ucComBuf[1] = Block_Adr;
  
  if(FM17xx_CommandSend(2, ucComBuf, eFM17xxCmd_Transceive) != MI_OK){
    return(MI_NOTAGERR);
  }

  if(ReadRawRC(FIFO_Length) == 0){
    return(MI_BYTECOUNTERR);
  }

  FM17xx_ReadFIFO(ucComBuf);
  switch(ucComBuf[0]) {
    case 0x00:	return(MI_NOTAUTHERR);	     
    case 0x04:	return(MI_EMPTY);
    case 0x0a:	break;
    case 0x01:	return(MI_CRCERR);
    case 0x05:	return(MI_PARITYERR);
    default:	return(MI_WRITEERR);
  }

  if(FM17xx_CommandSend(16, buff, eFM17xxCmd_Transceive) != MI_OK){
    temp = ReadRawRC(ErrorFlag);
    if((temp & 0x02) == 0x02){
      return(MI_PARITYERR);
    }else if((temp & 0x04) == 0x04){
      return(MI_FRAMINGERR);
    }else if((temp & 0x08) == 0x08){
      return(MI_CRCERR);
    }else{
      return(MI_WRITEERR);
    }
    
  }else{
    return(MI_OK);
  }
}

#if 0
char FM17xx_Restore(uint8_t Block_Adr){
	uint8_t	temp, i;
	uint8_t ucComBuf[BUFMAXRLEN];
    
    
	WriteRawRC(CRCPresetLSB,0x63);
	WriteRawRC(CWConductance,0x3f);
	WriteRawRC(ChannelRedundancy,0x07);
	*ucComBuf = PICC_RESTORE;
	*(ucComBuf + 1) = Block_Adr;
	if(MI_OK != FM17xx_CommandSend(2, ucComBuf, eFM17xxCmd_Transceive)){
		return MI_NOTAGERR;
	}

	temp = SPIRead(FIFO_Length);
	if(temp == 0)
	{
		return MI_BYTECOUNTERR;
	}

	FM17xx_ReadFIFO(ucComBuf);
	switch(ucComBuf[0])
	{
      case 0x00:	/* break; */return(MI_NOTAUTHERR);	
      case 0x04:	return(MI_EMPTY);
      case 0x0a:	break;
      case 0x01:	return(MI_CRCERR);
      case 0x05:	return(MI_PARITYERR);
      default:	return(MI_RESTERR);
	}

    memset(ucComBuf, 0, 4);
    if(MI_OK != FM17xx_CommandSend(4, ucComBuf, eFM17xxCmd_Transmit)){
    return MI_RESTERR;
	}

	return MI_OK;
}
#endif

char FM17xx_Value(uint8_t mOpmode,uint8_t addr,uint8_t *pValue){
	uint8_t		temp;
	uint8_t ucComBuf[BUFMAXRLEN];;
	
	WriteRawRC(CRCPresetLSB,0x63);
	WriteRawRC(CWConductance,0x3f);

	ucComBuf[0] = mOpmode;
	ucComBuf[1] = addr;
	WriteRawRC(ChannelRedundancy,0x07);

	if(FM17xx_CommandSend(16, ucComBuf, eFM17xxCmd_Transceive) != MI_OK){
		return MI_NOTAGERR;
	}

	if(ReadRawRC(FIFO_Length) == 0){
		return MI_BYTECOUNTERR;
	}

	FM17xx_ReadFIFO(ucComBuf);
	temp = *ucComBuf;
	switch(temp)
	{
	case 0x00:	/* break; */return(MI_NOTAUTHERR);	
	case 0x04:	return(MI_EMPTY);
	case 0x0a:	break;
	case 0x01:	return(MI_CRCERR);
	case 0x05:	return(MI_PARITYERR);
	default:	return(MI_DECRERR);
	}

    memcpy(ucComBuf, pValue, 4);
	if(FM17xx_CommandSend(4, ucComBuf, eFM17xxCmd_Transceive) != MI_OK){
		return MI_NOTAGERR;
	}
    
    ucComBuf[0] = PICC_TRANSFER;
	ucComBuf[1] = addr;
	
    if(FM17xx_CommandSend(4, ucComBuf, eFM17xxCmd_Transceive) != MI_OK){
		return MI_NOTAGERR;
	}
    
	
    if(ReadRawRC(FIFO_Length)){
      FM17xx_ReadFIFO(ucComBuf);
      if(ucComBuf[0] == 0x0A){
        return MI_OK;
      }
	}
    
    return MI_ERR;
	
}

char FM17xx_Halt(void){
  uint8_t ucComBuf[2];
  
  WriteRawRC(ChannelRedundancy, 0x07);			//
  ucComBuf[0] = PICC_HALT;
  ucComBuf[1] = 0x00;
  
  return FM17xx_CommandSend(2, ucComBuf, eFM17xxCmd_Transmit);
}


void FM17xx_Init(void){
  MFRC_GPIOConfigure();
  
  FM17xx_BusSel();
}

#endif

