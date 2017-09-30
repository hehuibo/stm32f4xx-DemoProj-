/*
 ============================================================================
 Name        : LCD19264.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "LCD19264.h"
#include "TimDly.h"

extern void SysTickDlyMs(uint16_t ms);
#define LcdDly(x) SysTickDlyMs(x)//TimDlyMs(x)

#define LCD_RST_PIN          GPIO_Pin_1
#define LCD_RST_PORT         GPIOB
#define LCD_RST_PORTCLK      RCC_AHB1Periph_GPIOB
#define SET_LCD_RST(x)       x ? GPIO_SetBits(LCD_RST_PORT,LCD_RST_PIN):GPIO_ResetBits(LCD_RST_PORT,LCD_RST_PIN);

#define LCD_CS_PIN           GPIO_Pin_2
#define LCD_CS_PORT          GPIOB
#define LCD_CS_PORTCLK       RCC_AHB1Periph_GPIOB
#define SET_LCD_CS(x)	     x ? GPIO_SetBits(LCD_CS_PORT,LCD_CS_PIN):GPIO_ResetBits(LCD_CS_PORT,LCD_CS_PIN);

#define LCD_SID_PIN         GPIO_Pin_0  //PB0
#define LCD_SID_PORT        GPIOB
#define LCD_SID_PORTCLK     RCC_AHB1Periph_GPIOB
#define SET_LCD_SID(x)      x ? GPIO_SetBits(LCD_SID_PORT,LCD_SID_PIN):GPIO_ResetBits(LCD_SID_PORT,LCD_SID_PIN);

#define LCD_SCK1_PIN        GPIO_Pin_7
#define LCD_SCK1_PORT       GPIOB
#define LCD_SCK1_PORTCLK    RCC_AHB1Periph_GPIOB
#define SET_LCD_SCK1(x)     x ? GPIO_SetBits(LCD_SCK1_PORT,LCD_SCK1_PIN):GPIO_ResetBits(LCD_SCK1_PORT,LCD_SCK1_PIN);

#define LCD_SCK2_PIN        GPIO_Pin_6
#define LCD_SCK2_PORT       GPIOB
#define LCD_SCK2_PORTCLK    RCC_AHB1Periph_GPIOB
#define SET_LCD_SCK2(x)	    x ? GPIO_SetBits(LCD_SCK2_PORT,LCD_SCK2_PIN):GPIO_ResetBits(LCD_SCK2_PORT,LCD_SCK2_PIN);

static void LCD19264_GPIOConfigure(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  RCC_AHB1PeriphClockCmd(LCD_RST_PORTCLK| LCD_CS_PORTCLK | LCD_SID_PORTCLK |
                         LCD_SCK1_PORTCLK| LCD_SCK2_PORTCLK, ENABLE);  
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  
  /*CS*/
  GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
  GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
  
  /*RST*/
  GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
  GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);
  
  /*SID*/
  GPIO_InitStructure.GPIO_Pin = LCD_SID_PIN;
  GPIO_Init(LCD_SID_PORT, &GPIO_InitStructure);
  
  /*SCK1*/
  GPIO_InitStructure.GPIO_Pin = LCD_SCK1_PIN;
  GPIO_Init(LCD_SCK1_PORT, &GPIO_InitStructure);
  
  /*SCK2*/
  GPIO_InitStructure.GPIO_Pin = LCD_SCK2_PIN;
  GPIO_Init(LCD_SCK2_PORT, &GPIO_InitStructure);
}

void LcdSendByte(uint8_t mData, enum eTagLcdWRCtrlTYPE mType)
{
  for(uint8_t i=0; i<8; i++){
    
    SET_LCD_SID(((mData<<i)&0x80) == 0x80);
    switch(mType){
    case eLcdWRCtrlTYPE_HIGHT: 
      SET_LCD_SCK1(false);
      LcdDly(1);
      SET_LCD_SCK1(true);
      break;
      
    case eLcdWRCtrlTYPE_LOW :
      SET_LCD_SCK2(false);
      LcdDly(1);
      SET_LCD_SCK2(true);
      break;
    
    default :
      
      break;
    }
    LcdDly(1);
  }
}

void LcdWR(uint8_t mData, uint8_t mWRS, enum eTagLcdWRCtrlTYPE mType)
{
  LcdSendByte(0xF8+(mWRS << 1), mType);
  LcdSendByte(mData & 0xF0, mType);
  LcdSendByte((mData << 4)& 0xF0, mType);
}

/*
*/
void LcdWRCmdDat(bool bData, uint8_t mCmdDat, enum eTagLcdWRCtrlTYPE mType)
{
  SET_LCD_CS(false);
  LcdDly(1);
  SET_LCD_CS(true);
  LcdWR(mCmdDat, bData, mType);//1,data, 0: cmd
  LcdDly(1);
  //delay
}

void LCD19264_Init(void)
{
  LCD19264_GPIOConfigure();
    
  LcdDly(5);
  SET_LCD_RST(true);
  SET_LCD_RST(false);
  LcdDly(5);
  SET_LCD_RST(true);
  
  LcdWRCmdDat(false, 0x30, eLcdWRCtrlTYPE_HIGHT);
  LcdWRCmdDat(false, 0x30, eLcdWRCtrlTYPE_LOW);
  
  LcdWRCmdDat(false, 0x06, eLcdWRCtrlTYPE_HIGHT);
  LcdWRCmdDat(false, 0x06, eLcdWRCtrlTYPE_LOW);
  
  /*�����ʾ: 0x01*/
  LcdWRCmdDat(false, 0x01, eLcdWRCtrlTYPE_HIGHT);
  LcdWRCmdDat(false, 0x01, eLcdWRCtrlTYPE_LOW);
  
  LcdWRCmdDat(false, 0x0C, eLcdWRCtrlTYPE_HIGHT);
  LcdWRCmdDat(false, 0x0C, eLcdWRCtrlTYPE_LOW);
  
  /*��ַ����: 0x02*/
  LcdWRCmdDat(false, 0x02, eLcdWRCtrlTYPE_HIGHT);
  LcdWRCmdDat(false, 0x02, eLcdWRCtrlTYPE_LOW);
}

void LcdClrScreen(void)
{
  LcdWRCmdDat(false, 0x01, eLcdWRCtrlTYPE_HIGHT);
  LcdWRCmdDat(false, 0x01, eLcdWRCtrlTYPE_LOW);
}

void LcdShowChars(uint8_t addr, const uint8_t *pStr, uint8_t cout,  enum eTagLcdWRCtrlTYPE mType)
{
  
  //LcdWRCmdDat(false, 0x30, mType);
  //LcdWRCmdDat(false, 0x0C, eLcdWRCtrlTYPE_LOW);
  
  LcdWRCmdDat(false, addr, mType);

#if 0
  for(uint8_t i=0; i<cout; i++){
   LcdWRCmdDat(true, pStr[i*2], mType); 
   LcdWRCmdDat(true, pStr[i*2+1], mType);
  }
#else
  while(*pStr){
    LcdWRCmdDat(true, *pStr, mType);
    pStr++;
  }
#endif
  //LcdWRCmdDat(false, 0x34, mType);
  //LcdWRCmdDat(false, 0x34, eLcdWRCtrlTYPE_HIGHT);
}

void LcdPutStr(const uint8_t *pStr, uint8_t len, enum eTagLcdWRCtrlTYPE mLine, uint8_t offset)
{
  uint8_t addr = 0x80;
  enum eTagLcdWRCtrlTYPE mType = eLcdWRCtrlTYPE_HIGHT;
  
  if((mLine == eLcdWRCtrlLine_Two) || (mLine == eLcdWRCtrlLine_Four)){
    addr = 0x90;
  }
  
  addr += offset;
 
  if((mLine == eLcdWRCtrlLine_Three) || (mLine == eLcdWRCtrlLine_Four)){
    mType = eLcdWRCtrlTYPE_LOW;
  }
 
  if(len % 2){
    len ++;
  }
  
  LcdShowChars(addr, pStr, len/2,  mType);
}

