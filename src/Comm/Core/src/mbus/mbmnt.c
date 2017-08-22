/*
 ============================================================================
 Name        : mbmnt.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Modbus管理
 ============================================================================
 */
#include "TypesCfg.h"
#include "port.h"
#include "mb.h"
#include "mbutils.h"
#include "mbport.h"

/******************************************************************************
                               定义Modbus相关参数
  起始地址,寄存器数目
******************************************************************************/

/*输入寄存器*/
#define REG_INPUT_START                          (USHORT)0x0000 
#define REG_INPUT_NREGS                          (USHORT)16  
uint16_t usRegInputStart = REG_INPUT_START;

/*输入寄存器内容*/
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007};

/*保持寄存器*/
#define REG_HOLDING_START                        (USHORT)0x0000 
#define REG_HOLDING_NREGS                        (USHORT)8  
uint16_t usRegHoldingStart = REG_HOLDING_START;

/*保存寄存器内容*/
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007};


/*线圈寄存器*/
#define REG_COILS_START                         (USHORT)0x0000
#define REG_COILS_SIZE                          (USHORT)16 
//线圈状态，上位机可更改
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x02};  //是否由上位机直接赋值

/*开关寄存器*/
#define REG_DISCRETE_START    0x0000
#define REG_DISCRETE_SIZE     16
//开关输入状态，供上位机读取
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x01,0x02};





/**
  *****************************************************************************
  * @Name   : 操作输入寄存器
  *
  *
  * @Input  : *pucRegBuffer 数据缓冲区,响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       寄存器个数
  *
  * @Output : none
  *
  * @Return : 
  *****************************************************************************
**/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( (int16_t)usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
       //获得操作偏移量，本次操作起始地址-输入寄存器的初始地址
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作保持寄存器
  *
  * @Brief  : 对应功能码0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode err = MB_ENOERR;
    int          iRegIndex = 0;
    
    if(((int16_t)usAddress >= REG_HOLDING_START)
       &&((usAddress+usNRegs)<=(REG_HOLDING_START+REG_HOLDING_NREGS))){
      
      //计算偏移量 = 要访问的地址—基地址   
      iRegIndex = (int)(usAddress - usRegHoldingStart);
      
      switch(eMode){
        case MB_REG_READ :{
            while(usNRegs > 0){
              *pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);  /*H*/
              *pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF); /*L*/
              iRegIndex++;
              usNRegs--;
            }
          }
          break;
          
        case MB_REG_WRITE :{
            while(usNRegs > 0){
              usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;  /*H*/
              usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;  /*L*/
              iRegIndex++;
              usNRegs--;
            }
          }
          break;
      }
    }else{
      err = MB_ENOREG;
    }
    
    return err;
}

/**
  * @brief  线圈寄存器处理函数，线圈寄存器可读，可读可写
  * @param  pucRegBuffer  读操作---返回数据指针，写操作--返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode err = MB_ENOERR;
    int16_t      iNCoils = ( int16_t )usNCoils;
    int16_t         usBitOffset;
    
    if(((int16_t)usAddress >= REG_HOLDING_START)
       &&((usAddress+usNCoils)<=(REG_HOLDING_START+REG_HOLDING_NREGS))){
         
      //计算寄存器偏移量
      usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
      
      switch(eMode){
        case MB_REG_READ :{
            while(usNCoils > 0){
              *pucRegBuffer++ = xMBUtilGetBits(ucRegCoilsBuf, usBitOffset,
                                               ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
              iNCoils -= 8;
              usBitOffset += 8;
            }
          }
          break;
          
        case MB_REG_WRITE :{
            while(usNCoils > 0){
              xMBUtilSetBits(ucRegCoilsBuf, usBitOffset,
                             ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                             *pucRegBuffer++);
              iNCoils -= 8;
              usBitOffset += 8;
            }
          }
          break;
      }
    }else{
      err = MB_ENOREG;
    }
    
    return err;
}

/*
读离散线圈回函数。* pucRegBuffer为要添加到协议中的数据，
usAddress为线圈地址，usNDiscrete为要访问线圈的个数。
用户应根据要访问的线圈地址usAddress将相应线圈的值按顺序添加到pucRegBuffer中。
*/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //操作寄存器个数
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //偏移量
  uint16_t        usBitOffset;

  //判断寄存器时候再制定范围内
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //获得偏移量
    usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );
    
    while( iNDiscrete > 0 )
    {
      *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                      ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
      iNDiscrete -= 8;
      usBitOffset += 8;
    }

  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

/**
  *****************************************************************************
  *****************************************************************************
**/

void ModbusMntInit(void)
{
  eMBInit(MB_RTU, 1, 1, 9600, MB_PAR_NONE);
  
  eMBEnable();
  
  for(int i=0; i<REG_INPUT_NREGS; i++){
   usRegInputBuf[i] = 255 + i;  
  }
}

void vModbusPoll(void)
{
  /*FreeModBus*/
  eMBPoll();
}

void TestModBus(void)
{
  


		
}

