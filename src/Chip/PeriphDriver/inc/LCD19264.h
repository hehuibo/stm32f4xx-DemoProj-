/*
 * LCD19264.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef LCD19264_H_
#define LCD19264_H_

#if defined (STM32F40_41xxx)
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif
#include "TypesCfg.h"

enum eTagLcdWRCtrlTYPE
{
  eLcdWRCtrlTYPE_LOW, 
  eLcdWRCtrlTYPE_HIGHT,
  
  eLcdWRCtrlLine_One = 1,
  eLcdWRCtrlLine_Two,
  eLcdWRCtrlLine_Three,
  eLcdWRCtrlLine_Four
};



void LCD19264_Init(void);

void LcdShowNumChar(uint8_t addr, uint8_t i, uint8_t cout,  enum eTagLcdWRCtrlTYPE mType);
void LcdShowChars(uint8_t addr, const uint8_t *pStr, uint8_t cout,  enum eTagLcdWRCtrlTYPE mType);
void LcdPutStr(const uint8_t *pStr, uint8_t len, enum eTagLcdWRCtrlTYPE mLine, uint8_t offset);

#endif /* LCD19264_H_ */
