/*
 ============================================================================
 Name        : mbmnt.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Modbus����
 ============================================================================
 */
#include "TypesCfg.h"
#include "port.h"
#include "mb.h"
#include "mbutils.h"
#include "mbport.h"

/******************************************************************************
                               ����Modbus��ز���
  ��ʼ��ַ,�Ĵ�����Ŀ
******************************************************************************/

/*����Ĵ���*/
#define REG_INPUT_START                          (USHORT)0x0000 
#define REG_INPUT_NREGS                          (USHORT)16  
uint16_t usRegInputStart = REG_INPUT_START;

/*����Ĵ�������*/
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007};

/*���ּĴ���*/
#define REG_HOLDING_START                        (USHORT)0x0000 
#define REG_HOLDING_NREGS                        (USHORT)8  
uint16_t usRegHoldingStart = REG_HOLDING_START;

/*����Ĵ�������*/
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007};


/*��Ȧ�Ĵ���*/
#define REG_COILS_START                         (USHORT)0x0000
#define REG_COILS_SIZE                          (USHORT)16 
//��Ȧ״̬����λ���ɸ���
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x02};  //�Ƿ�����λ��ֱ�Ӹ�ֵ

/*���ؼĴ���*/
#define REG_DISCRETE_START    0x0000
#define REG_DISCRETE_SIZE     16
//��������״̬������λ����ȡ
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x01,0x02};





/**
  *****************************************************************************
  * @Name   : ��������Ĵ���
  *
  *
  * @Input  : *pucRegBuffer ���ݻ�����,��Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �Ĵ�������
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
       //��ò���ƫ���������β�����ʼ��ַ-����Ĵ����ĳ�ʼ��ַ
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
  * @Name   : �������ּĴ���
  *
  * @Brief  : ��Ӧ������0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
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
      
      //����ƫ���� = Ҫ���ʵĵ�ַ������ַ   
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
  * @brief  ��Ȧ�Ĵ�������������Ȧ�Ĵ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������---��������ָ�룬д����--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
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
         
      //����Ĵ���ƫ����
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
����ɢ��Ȧ�غ�����* pucRegBufferΪҪ��ӵ�Э���е����ݣ�
usAddressΪ��Ȧ��ַ��usNDiscreteΪҪ������Ȧ�ĸ�����
�û�Ӧ����Ҫ���ʵ���Ȧ��ַusAddress����Ӧ��Ȧ��ֵ��˳����ӵ�pucRegBuffer�С�
*/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�����Ĵ�������
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //ƫ����
  uint16_t        usBitOffset;

  //�жϼĴ���ʱ�����ƶ���Χ��
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //���ƫ����
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

