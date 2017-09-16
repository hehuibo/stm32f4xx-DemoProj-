/*
 ============================================================================
 Name        : TFT.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "TFT.h"
#include "TimDly.h"

#define	cSetD0		 0x01
#define	cSetD1		 0x02
#define	cSetD2		 0x04
#define	cSetD3		 0x08
#define	cSetD4		 0x10
#define	cSetD5		 0x20
#define	cSetD6		 0x40
#define	cSetD7		 0x80

#define	cClrD0		 0xfe
#define	cClrD1		 0xfd
#define	cClrD2		 0xfb
#define	cClrD3		 0xf7
#define	cClrD4		 0xef
#define	cClrD5		 0xdf
#define	cClrD6		 0xbf
#define	cClrD7		 0x7f

#define cClrD1D0     0xfc
#define cClrD1SetD0  0x01
#define cSetD1ClrD0  0x02
#define cSetD1D0     0x03
#define cClrD3D2     0xf3
#define cClrD3SetD2  0x04
#define cSetD3ClrD2  0x08
#define cSetD3D2     0x0c

#define TFT_DlyMs   TimDlyMs //
#define TFT_DlyUs   TimDlyUs //vTaskDelay 
static void TFT_GPIOConfigure(void){
  
  GPIO_InitTypeDef  GPIO_InitStructure;
  
#if defined(STemWin_GUI)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);   
#endif 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD |
                         RCC_AHB1Periph_GPIOE | 
                         RCC_AHB1Periph_GPIOF |
                         RCC_AHB1Periph_GPIOG, ENABLE);  
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_14 |
                                GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 |
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//A16
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12
  
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12
 
  //NE4
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);//A16

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static void TFT_FSMCConfigure(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  rdTiming,wrTiming; 
  
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);

  rdTiming.FSMC_AddressSetupTime = 4;
  rdTiming.FSMC_AddressHoldTime = 0; 
  rdTiming.FSMC_DataSetupTime = 6;
  rdTiming.FSMC_BusTurnAroundDuration = 1;
  rdTiming.FSMC_CLKDivision = 0;
  rdTiming.FSMC_DataLatency = 0;
  rdTiming.FSMC_AccessMode = FSMC_AccessMode_A;
  
  wrTiming.FSMC_AddressSetupTime = 4;
  wrTiming.FSMC_AddressHoldTime = 0; 
  wrTiming.FSMC_DataSetupTime = 8;
  wrTiming.FSMC_BusTurnAroundDuration = 1;
  wrTiming.FSMC_CLKDivision = 0;
  wrTiming.FSMC_DataLatency = 0;
  wrTiming.FSMC_AccessMode = FSMC_AccessMode_A;


  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rdTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &wrTiming;
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

uint8_t TFT_RDStatus(void){
  return   TFT_REG;
}


void TFT_WaitBusy(void){   
  while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == RESET){};

}

uint8_t TFT_RDData(void){
  return TFT_RAM;
}

uint16_t TFT_RDData16(void){
  uint16_t mValue;
  
  mValue = TFT_RAM;
  return mValue;
}


uint8_t TFT_RDReg(uint8_t mReg){
  //TFT_WaitBusy();
  TFT_WRCmd(mReg);
  return TFT_RDData();
}
void TFT_WRReg(uint8_t mCmd, uint8_t mData){
  //TFT_WaitBusy();
  TFT_REG = mCmd;
  TFT_RAM = mData;

}

void RA8875_SoftwareReset(void){
  TFT_WRCmd(0x01);
  TFT_WRData(0x01);
  TFT_WRData(0x00);
  
  TFT_DlyMs(1);
}

void RA8875_PLL_Init(void){
  /*480x272*/
  TFT_WRReg(0x88, 0x0A);
  TFT_DlyMs(5);
  TFT_WRReg(0x89, 0x02); 
  TFT_DlyMs(5);
  
  RA8875_SoftwareReset();
  TFT_DlyMs(5);
}

void RA8875_PWM1_Enable(void){  
  TFT_WRCmd(0x8a);
  TFT_WRData(TFT_RDData() | cSetD7 );
}

void RA8875_PWM1_Disable(void){
  TFT_WRCmd(0x8a);
  TFT_WRData(TFT_RDData() & cSetD7 );  
}

void RA8875_PWM1_FunctionSel(void){
  TFT_WRCmd(0x8a);
  TFT_WRData( TFT_RDData() & cClrD4);
}

void RA8875_PWM1_ClockRatio(uint8_t setx){
  uint8_t tmp,tmp1;
  
  tmp1 = setx & 0x0F;
  TFT_WRCmd(0x8a);
  tmp = TFT_RDData() & 0xF0;
  tmp |= tmp1;
  TFT_WRData(tmp);  
}

void RA8875_PWM1_DutyCycle(uint8_t setx){
  TFT_WRCmd(0x8b);
  TFT_WRData(setx);
}

void RA8875_GPOData(uint8_t setx){
  TFT_WRCmd(0x13);
  TFT_WRData(setx);
}

void RA8875_ClearActiveWindow(void){
  uint8_t tmp;
  
  TFT_WRCmd(0x8e);
  tmp = TFT_RDData() | cSetD6;
  TFT_WRData(tmp); 
  TFT_WaitBusy();
}

void RA8875_SetCursor(uint16_t x0, uint16_t y0){
  TFT_WRReg(0x46, x0);
  TFT_WRReg(0x47, x0>>8);  
  TFT_WRReg(0x48, y0);
  TFT_WRReg(0x49, y0>>8);
}

void RA8875_ActiveWindow(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB){
  //setting active window X
  TFT_WRReg(0x30, XL);
  TFT_WRReg(0x31, XL>>8);
  
  TFT_WRReg(0x34, XR);
  TFT_WRReg(0x35, XR>>8);
  
  TFT_WRReg(0x32, YT);
  TFT_WRReg(0x33, YT>>8);
 
  
  TFT_WRReg(0x36, YB);
  TFT_WRReg(0x37, YB>>8);
  TFT_WaitBusy();
}

void RA8875_MPU16BitInterface(void){
  uint8_t tmp;
  
  TFT_WRCmd(0x10);
  tmp = TFT_RDData() | cSetD1ClrD0;
  TFT_WRData(tmp);  
}

void RA8875_MemoryClear(void){
  uint8_t tmp;
  
  TFT_WRCmd(0x8e);
  tmp = TFT_RDData() | cSetD7;;
  TFT_WRData(tmp);  
  TFT_WaitBusy();
}

void RA8875_GraphicMode(void){
   uint8_t tmp;
  
  TFT_WRCmd(0x40);
  tmp = TFT_RDData() & cClrD7;
  TFT_WRData(tmp);   
}

void RA8875_SetForegroundColor(uint16_t nColor){
  TFT_WRReg(0x63, (nColor & 0xF800) >> 11);
  TFT_WRReg(0x64, (nColor & 0x07E0) >> 5);
  TFT_WRReg(0x65, nColor & 0x001F);
  TFT_WaitBusy();
}

void RA8875_SetBackgroundColor(uint16_t nColor){
  TFT_WRReg(0x60, (nColor & 0xF800) >> 11);
  TFT_WRReg(0x61, (nColor & 0x07E0) >> 5);
  TFT_WRReg(0x62, nColor & 0x001F);
  
  TFT_WaitBusy();
}

void GeometricCoordinate(uint16_t SX,uint16_t SY,uint16_t EX,uint16_t EY){
  TFT_WRReg(0x91, SX);
  TFT_WRReg(0x92, SX >> 8);
  
  TFT_WRReg(0x95, EX);
  TFT_WRReg(0x96, EX >> 8);
  
  TFT_WRReg(0x93, SY);
  TFT_WRReg(0x94, SY >> 8);
  
  TFT_WRReg(0x97, EY);
  TFT_WRReg(0x98, EY >> 8);
  //wait
  TFT_WaitBusy();
}

void RA8875_TextMode(void){
  TFT_WRReg(0x40,TFT_RDData() | cSetD7 );  
}

void TFT_Init(void){
  TFT_GPIOConfigure();
  
  TFT_FSMCConfigure();
  
  TFT_DlyMs(100);
  /*Config PLL*/
  RA8875_PLL_Init();
  /*480x272*/
  TFT_WRReg(0x10,0x0F);
  TFT_DlyMs(1);
  
  /*480x272*/
  TFT_WRReg(0x04,0x82);
  TFT_DlyMs(1);
  
  TFT_WRReg(0x14, 0x3B);
  if(TFT_RDReg(0x14) != 0x3B){/*????*/
    TFT_WRReg(0x14, 0x3B);
  }
  TFT_WRReg(0x15, 0x00);
  TFT_WRReg(0x16, 0x01);
  TFT_WRReg(0x17, 0x00);
  TFT_WRReg(0x18, 0x05);//05
  
  TFT_WRReg(0x19, 0x0F);
  TFT_WRReg(0x1A, 0x01);
  TFT_WRReg(0x1B, 0x02);//02
  TFT_WRReg(0x1C, 0x00);
  TFT_WRReg(0x1D, 0x07);
  TFT_WRReg(0x1E, 0x00);
  TFT_WRReg(0x1F, 0x09);
  
  TFT_WRReg(0x30, 0x00);
  TFT_WRReg(0x31, 0x00);
  TFT_WRReg(0x34, 0xDF);
  TFT_WRReg(0x35, 0x01);
  
  TFT_WRReg(0x32, 0x00);
  TFT_WRReg(0x33, 0x00);
  TFT_WRReg(0x36, 0x0F);
  TFT_WRReg(0x37, 0x01);
  TFT_WRReg(0xC7 , 0x01);  //DISP 1
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("0x14 = %x\tID = %x\r\n", TFT_RDReg(0x14), TFT_RDReg(0x00));
  #endif 
  
  TFT_WRReg(0x01, 0x80);
  
  RA8875_ActiveWindow(0, TFT_XSIZE_PHYS, 0 ,TFT_YSIZE_PHYS);
  
  RA8875_MPU16BitInterface();
  
  RA8875_MemoryClear();
  
  RA8875_GraphicMode();
}
