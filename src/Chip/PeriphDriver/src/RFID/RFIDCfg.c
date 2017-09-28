#include "RFID\RFID.h"

#if defined(RFID_CHIP_RC522)
#include "RFID\MFRC522.h"  
#else
#include "RFID\FM17xx.h"   
#endif

/*M1电子钱包
某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
	4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反

	第8块初始化为数值块，数值为0，用写块命令写入 
0×00 0×00 0×00 0×00 0xff 0xff 0xff 0xff 0×00 0×00 0×00 0×00 0×08 0xf7 0×08 0xf7
*/

/**********************IO口时钟配置*******************************/
#if 1
  #if defined(STM32F40_41xxx)     /*STM32F4xx*/
    #ifdef _MFRC_SOFT_SPI
    #define MFRC_SCK_GPIO_CLK     RCC_AHB1Periph_GPIOB
    #define MFRC_MOSI_GPIO_CLK	RCC_AHB1Periph_GPIOB
    #define MFRC_MISO_GPIO_CLK	RCC_AHB1Periph_GPIOB                 
    #endif

    #define MFRC_CS_GPIO_CLK	    RCC_AHB1Periph_GPIOF
    #define MFRC_RST_GPIO_CLK	    RCC_AHB1Periph_GPIOF

  #else        /*STM32F10x*/
    #ifdef _MFRC_SOFT_SPI
    #define MFRC_SCK_GPIO_CLK	RCC_APB2Periph_GPIOA
    #define MFRC_MOSI_GPIO_CLK	RCC_APB2Periph_GPIOA
    #define MFRC_MISO_GPIO_CLK	RCC_APB2Periph_GPIOA
    #endif

    #define MFRC_CS_GPIO_CLK	RCC_APB2Periph_GPIOC
    #define MFRC_RST_GPIO_CLK	RCC_APB2Periph_GPIOC
  #endif

  /**********************IO口配置*******************************/

  /****************************MFRC_SOFT_SPI*************************************/
  /**
   *  SCK, MISO, MOSI---------->模拟SPI
   */
  #ifdef _MFRC_SOFT_SPI
  #define MFRC_SCK_PIN		GPIO_Pin_13
  #define MFRC_SCK_PORT		GPIOB
  #define Set_MFRC_SCK(x)	x ? GPIO_SetBits(MFRC_SCK_PORT,MFRC_SCK_PIN):GPIO_ResetBits(MFRC_SCK_PORT,MFRC_SCK_PIN);

  #define MFRC_MISO_PIN		GPIO_Pin_14//GPIO_Pin_5//
  #define MFRC_MISO_PORT	GPIOB
  #define MFRC_MISO_STATUS()	GPIO_ReadInputDataBit(MFRC_MISO_PORT,MFRC_MISO_PIN)


  #define MFRC_MOSI_PIN		GPIO_Pin_15
  #define MFRC_MOSI_PORT	GPIOB
  #define Set_MFRC_MOSI(x)	x ? GPIO_SetBits(MFRC_MOSI_PORT,MFRC_MOSI_PIN):GPIO_ResetBits(MFRC_MOSI_PORT,MFRC_MOSI_PIN);
  #endif

  /*****************CS、RST IO***************************/
  #define MFRC_CS_PIN		GPIO_Pin_6
  #define MFRC_CS_PORT	        GPIOF
  #define Set_MFRC_CS(x)	x ? GPIO_SetBits(MFRC_CS_PORT,MFRC_CS_PIN):GPIO_ResetBits(MFRC_CS_PORT,MFRC_CS_PIN);
   
  #define MFRC_RST_PIN		GPIO_Pin_7
  #define MFRC_RST_PORT		GPIOF
  #define Set_MFRC_RST(x)	x ? GPIO_SetBits(MFRC_RST_PORT,MFRC_RST_PIN):GPIO_ResetBits(MFRC_RST_PORT,MFRC_RST_PIN);

#else
/**********************IO口时钟配置*******************************/
#if defined(STM32F40_41xxx)     /*STM32F4xx*/
  #ifdef _MFRC_SOFT_SPI
  #define MFRC_SCK_GPIO_CLK	    RCC_AHB1Periph_GPIOF
  #define MFRC_MOSI_GPIO_CLK	RCC_AHB1Periph_GPIOF
  #define MFRC_MISO_GPIO_CLK	RCC_AHB1Periph_GPIOF                  
  #endif

  #define MFRC_CS_GPIO_CLK	    RCC_AHB1Periph_GPIOF
  #define MFRC_RST_GPIO_CLK	    RCC_AHB1Periph_GPIOF

#else        /*STM32F10x*/
  #ifdef _MFRC_SOFT_SPI
  #define MFRC_SCK_GPIO_CLK	    RCC_APB2Periph_GPIOA
  #define MFRC_MOSI_GPIO_CLK	RCC_APB2Periph_GPIOA
  #define MFRC_MISO_GPIO_CLK	RCC_APB2Periph_GPIOA
  #endif

  #define MFRC_CS_GPIO_CLK	RCC_APB2Periph_GPIOC
  #define MFRC_RST_GPIO_CLK	RCC_APB2Periph_GPIOC
#endif

/**********************IO口配置*******************************/

/****************************MFRC_SOFT_SPI*************************************/
/**
 *  SCK, MISO, MOSI---------->模拟SPI
 */
#ifdef _MFRC_SOFT_SPI
#define MFRC_SCK_PIN		GPIO_Pin_7
#define MFRC_SCK_PORT		GPIOF
#define Set_MFRC_SCK(x)		x ? GPIO_SetBits(MFRC_SCK_PORT,MFRC_SCK_PIN):GPIO_ResetBits(MFRC_SCK_PORT,MFRC_SCK_PIN);

#define MFRC_MISO_PIN		GPIO_Pin_10//GPIO_Pin_5//
#define MFRC_MISO_PORT		GPIOF
#define MFRC_MISO_STATUS()	GPIO_ReadInputDataBit(MFRC_MISO_PORT,MFRC_MISO_PIN)


#define MFRC_MOSI_PIN		GPIO_Pin_8
#define MFRC_MOSI_PORT		GPIOF
#define Set_MFRC_MOSI(x)	x ? GPIO_SetBits(MFRC_MOSI_PORT,MFRC_MOSI_PIN):GPIO_ResetBits(MFRC_MOSI_PORT,MFRC_MOSI_PIN);
#endif

/*****************CS、RST IO***************************/
#define MFRC_CS_PIN	        GPIO_Pin_6
#define MFRC_CS_PORT	        GPIOF
#define Set_MFRC_CS(x)		x ? GPIO_SetBits(MFRC_CS_PORT,MFRC_CS_PIN):GPIO_ResetBits(MFRC_CS_PORT,MFRC_CS_PIN);
 
#define MFRC_RST_PIN		GPIO_Pin_9
#define MFRC_RST_PORT		GPIOF
#define Set_MFRC_RST(x)		x ? GPIO_SetBits(MFRC_RST_PORT,MFRC_RST_PIN):GPIO_ResetBits(MFRC_RST_PORT,MFRC_RST_PIN);

#endif


void MFRC_Delay(uint16_t mDlyTime){
  for (uint16_t i = 50; i > 0; i--){
    for (uint16_t j = mDlyTime; j > 0; j--);
  }
}


/********************************************/
#if(MFRC_ULTRALPRO_CPU > 0)
uint8_t rfidDes( enum eRFIDCRYMode Mode,uint8_t *MsgIn, uint8_t *Key, uint8_t *MsgOut);
unsigned char rfidMAC(unsigned char *init_data,unsigned char *mac_key,unsigned char data_len,unsigned char *in_data,unsigned char *mac_data);
uint8_t rfidTdes( enum eRFIDCRYMode Mode,uint8_t *MsgIn, uint8_t *Key, uint8_t *MsgOut);
#endif


/*******************************************/
xTRfCtrlFUNCTIONTypeDef gfn_rfidCtrl;
void RFID_Init(void)
{
  memset(&gfn_rfidCtrl, 0, sizeof(gfn_rfidCtrl));
  
#if defined(RFID_CHIP_RC522)  
  MFRC522_Init();
  
  gfn_rfidCtrl.pfnHalt = PcdHalt;

  gfn_rfidCtrl.pfnSelect = PcdSelect;

  gfn_rfidCtrl.pfnAnticoll = PcdAnticoll;

  gfn_rfidCtrl.pfnWrite = PcdWrite;

  gfn_rfidCtrl.pfnRead = PcdRead;

  gfn_rfidCtrl.pfnRequest = PcdRequest;

  gfn_rfidCtrl.pfnAuthState = PcdAuthState; 
  
  #if(MFRC_ULTRALPRO_CPU > 0) 
  gfn_rfidCtrl.pfnRATS = PcdRATS;
  
  gfn_rfidCtrl.pfnCmdPro = PcdComCmdPro;
  
  #endif
#elif defined(RFID_CHIP_FM17xx)
  FM17xx_Init();
  
  gfn_rfidCtrl.pfnHalt = FM17xx_Halt;

  gfn_rfidCtrl.pfnSelect = FM17xx_Select;

  gfn_rfidCtrl.pfnAnticoll = FM17xx_AntiColl;

  gfn_rfidCtrl.pfnWrite = FM17xx_Write;

  gfn_rfidCtrl.pfnRead = FM17xx_Read;

  gfn_rfidCtrl.pfnRequest = FM17xx_Request;

  gfn_rfidCtrl.pfnAuthState = FM17xx_AuthState; 
  
  
  #if(MFRC_ULTRALPRO_CPU > 0) 
  gfn_rfidCtrl.pfnRATS = FM17xx_Rats;
  
  gfn_rfidCtrl.pfnCmdPro = FM17xx_ComCmdPro;
  #endif
#endif
  
#if(MFRC_ULTRALPRO_CPU > 0)  
  gfn_rfidCtrl.pfnDES = rfidDes;
  gfn_rfidCtrl.pfnTDES = rfidTdes;
  gfn_rfidCtrl.pfnMAC = rfidMAC;
#endif  
}



/*****************************************/
#ifdef _MFRC_SOFT_SPI
void RFIDWriteByte(uint8_t mData){
  for(uint8_t i=0; i<8; i++){
    MFRC_Delay(5);
    Set_MFRC_MOSI((mData & 0x80) == 0x80);
    Set_MFRC_SCK(1);
    MFRC_Delay(5);
    mData <<= 1;
    Set_MFRC_SCK(0);
  }
}
uint8_t RFIDReadByte(void){
  uint8_t mData;
  for(uint8_t i=0; i<8; i++){
    mData <<= 1;
    mData |= MFRC_MISO_STATUS();
    Set_MFRC_SCK(1);
    MFRC_Delay(5);
    Set_MFRC_SCK(0);
    MFRC_Delay(5);
  }
  return mData;
}
#endif
/*****************************************/
uint8_t ReadRawRC(uint8_t Address){
  uint8_t ucAddr;
  uint8_t ret = 0;

  ucAddr = ((Address << 1) & 0x7E) | 0x80;
  Set_MFRC_CS(0);
#ifdef _MFRC_SOFT_SPI
  RFIDWriteByte(ucAddr);
  ret = RFIDReadByte();
#else
  MFRC_TxRxByte(ucAddr);
  ret = MFRC_TxRxByte(0);
#endif
  Set_MFRC_CS(1);
#if defined(RFID_CHIP_RC522)    
  MFRC_Delay(5);
#endif
  return ret;
}

void WriteRawRC(uint8_t Address, uint8_t value){
  uint8_t ucAddr = 0;

  ucAddr = (Address << 1)& 0x7E;
  Set_MFRC_CS(0);
#ifdef _MFRC_SOFT_SPI
  RFIDWriteByte(ucAddr);
  RFIDWriteByte(value);
#else
  MFRC_TxRxByte(ucAddr);
  MFRC_TxRxByte(value);
#endif
  Set_MFRC_CS(1);

}



#if(MFRC_ULTRALPRO_CPU > 0) 
uint8_t gcRfidPcdPCB;
void PcdSwitchPCB(void)
{
  switch(gcRfidPcdPCB)
  {
    case 0x00:
      gcRfidPcdPCB=0x0A;
    break;
    case 0x0A:
      gcRfidPcdPCB=0x0B;
    break;
    case 0x0B:
      gcRfidPcdPCB=0x0A;
    break;
  }
}
#endif


void MFRC_GPIOConfigure(void){
  GPIO_InitTypeDef GPIO_InitStructure;

#if defined(STM32F40_41xxx)     /*STM32F4xx*/
  
  #ifdef _MFRC_SOFT_SPI
  RCC_AHB1PeriphClockCmd(MFRC_SCK_GPIO_CLK| MFRC_MISO_GPIO_CLK | MFRC_MOSI_GPIO_CLK |
                         MFRC_CS_GPIO_CLK | MFRC_RST_GPIO_CLK, ENABLE);
  #else
  RCC_AHB1PeriphClockCmd(MFRC_CS_GPIO_CLK| MFRC_RST_GPIO_CLK, ENABLE);
  #endif

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  #ifdef _MFRC_SOFT_SPI
  GPIO_InitStructure.GPIO_Pin = MFRC_SCK_PIN;
  GPIO_Init(MFRC_SCK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MFRC_MOSI_PIN;
  GPIO_Init(MFRC_MOSI_PORT, &GPIO_InitStructure);
  #endif
  
  GPIO_InitStructure.GPIO_Pin = MFRC_CS_PIN;
  GPIO_Init(MFRC_CS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MFRC_RST_PIN;
  GPIO_Init(MFRC_RST_PORT, &GPIO_InitStructure);

  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  
  #ifdef _MFRC_SOFT_SPI
  GPIO_InitStructure.GPIO_Pin = MFRC_MISO_PIN;
  GPIO_Init(MFRC_MISO_PORT, &GPIO_InitStructure);  
  #endif  
  
#else   /*STM32F10x*/
  
  #ifdef _MFRC_SOFT_SPI
  RCC_APB2PeriphClockCmd(MFRC_SCK_GPIO_CLK| MFRC_MISO_GPIO_CLK | MFRC_MOSI_GPIO_CLK |
                          MFRC_CS_GPIO_CLK| MFRC_RST_GPIO_CLK, ENABLE);
  #else
  RCC_APB2PeriphClockCmd(MFRC_CS_GPIO_CLK| MFRC_RST_GPIO_CLK, ENABLE);
  #endif


  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  #ifdef _MFRC_SOFT_SPI
  GPIO_InitStructure.GPIO_Pin = MFRC_SCK_PIN;
  GPIO_Init(MFRC_SCK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MFRC_MOSI_PIN;
  GPIO_Init(MFRC_MOSI_PORT, &GPIO_InitStructure);
  #endif
  
  GPIO_InitStructure.GPIO_Pin = MFRC_CS_PIN;
  GPIO_Init(MFRC_CS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MFRC_RST_PIN;
  GPIO_Init(MFRC_RST_PORT, &GPIO_InitStructure);
  
  #ifdef _MFRC_SOFT_SPI
  GPIO_InitStructure.GPIO_Pin = MFRC_MISO_PIN;
  GPIO_Init(MFRC_MISO_PORT, &GPIO_InitStructure);  
  #endif
#endif //end /*STM32F10x*/

#ifdef _MFRC_SOFT_SPI
  Set_MFRC_SCK(0);
  Set_MFRC_MOSI(0);
#endif
  
  Set_MFRC_CS(1);
  
  Set_MFRC_RST(1);     
  MFRC_Delay(1000);  
  Set_MFRC_RST(0);   
  MFRC_Delay(5000); 
  
  #if defined(RFID_CHIP_RC522)  
  Set_MFRC_RST(1); 
  #endif
}

