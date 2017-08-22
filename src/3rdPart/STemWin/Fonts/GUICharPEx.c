/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              μC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUICharP.C
Purpose     : Implementation of Proportional fonts
---------------------------END-OF-HEADER------------------------------
*/
#include "GUI.h"
#include "GUI_Private.h"
#include "Flash.h"
#include "Param.h"

#define BYTES_PER_FONT	512//1024	//32*32=1024
U8 GUI_FontDataBuf[BYTES_PER_FONT];

//#define GUIFONT_FLASHCS eFLASH_ID_CS0

/*
  0x00000000 // ASCII12.DZK
  0x00000C00 // ASCII16.DZK
  0x00001C00 // ASCII24.DZK
  0x00004C00 // ASCII32.DZK
  0x00008C00 // STGBK12.DZK
  0x00095060 // STGBK16.DZK
  0x001500E0 // STGBK24.DZK
  0x002F4E00 // HWLSGBK32.DZK
 */
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c){
  U16 BytesPerFont;
  U32 offset = 0, FontBaseAddr;
  U8  code1, code2;
  
  char *pFont = (char *)pProp->paCharInfo->pData;
  
  /* 每个字模的数据字节数 */
  BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine;
  if(BytesPerFont > BYTES_PER_FONT){
    BytesPerFont = BYTES_PER_FONT;
  }
  
  
  if(c < 0x80){/* 英文字符地址偏移计算 */ 
    if(strncmp("A12", pFont, 3) == 0){/* 6*12 ASCII字符:ASCII12 */
      FontBaseAddr = 0x00000000;
    }else if(strncmp("A16", pFont, 3) == 0){/* 8*16 ASCII字符:ASCII16 */
      FontBaseAddr = 0x00000C00;
    }else if(strncmp("A24", pFont, 3) == 0){/* 12*24 ASCII字符:ASCII24 */
      FontBaseAddr = 0x00001C00;
    }else if(strncmp("A32", pFont, 3) == 0){/* 24*48 ASCII字符:ASCII32*/
      FontBaseAddr = 0x00004C00;
    }

   offset = c* BytesPerFont + FontBaseAddr; 
  }else{/* 汉字和全角字符的偏移地址计算 */
    if(strncmp("H12", pFont, 3) == 0){/* 12*12 字符 */
      FontBaseAddr = 0x00008C00;
    }else if(strncmp("H16", pFont, 3) == 0){/* 16*16 字符 */
      FontBaseAddr = 0x00095060;
    }else if(strncmp("H24", pFont, 3) == 0){/* 24*24 字符 */
      FontBaseAddr = 0x001500E0;
    }else if(strncmp("H32", pFont, 3) == 0){/* 32*32 字符 */
      FontBaseAddr = 0x002F4E00;
    }
    
    /* 根据汉字内码的计算公式锁定起始地址 */
    code2 = c >> 8;
    code1 = c & 0xFF;
    
    offset = ((code1 - 0x81) * 190 + (code2- 0x40) - (code2 / 128))* BytesPerFont + FontBaseAddr;
  }
  
  FlashRead(offset + pDevParam->mFontGBK, (void*)GUI_FontDataBuf , BytesPerFont, GUIFONT_FLASHCS);
}
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIPROP_DispChar
*
* Purpose:
*   This is the routine that displays a character. It is used by all
*   other routines which display characters as a subroutine.
*/
void GUIPROP_X_DispChar(U16P c) 
{	
  int BytesPerLine;
  GUI_DRAWMODE DrawMode = GUI_pContext->TextMode;
  const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp;

  for (; pProp; pProp = pProp->pNext)                                         {
    if ((c >= pProp->First) && (c <= pProp->Last))
      break;
  }
  if (pProp){
    GUI_DRAWMODE OldDrawMode;
    const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
    
    GUI_GetDataFromMemory(pProp, c);
    BytesPerLine = pCharInfo->BytesPerLine;                
    OldDrawMode  = LCD_SetDrawMode(DrawMode);
    LCD_DrawBitmap(GUI_pContext->DispPosX, GUI_pContext->DispPosY,
         pCharInfo->XSize, GUI_pContext->pAFont->YSize,
         GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
         1,     /* Bits per Pixel */
         BytesPerLine,
         &GUI_FontDataBuf[0],
         &LCD_BKCOLORINDEX
         );
    /* Fill empty pixel lines */
    if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) {
      int YMag = GUI_pContext->pAFont->YMag;
      int YDist = GUI_pContext->pAFont->YDist * YMag;
      int YSize = GUI_pContext->pAFont->YSize * YMag;
      if (DrawMode != LCD_DRAWMODE_TRANS) {
        LCD_COLOR OldColor = GUI_GetColor();
        GUI_SetColor(GUI_GetBkColor());
        LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize, 
                         GUI_pContext->DispPosX + pCharInfo->XSize, 
                         GUI_pContext->DispPosY + YDist);
        GUI_SetColor(OldColor);
      }
    }
    LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
    GUI_pContext->DispPosX += pCharInfo->XDist * GUI_pContext->pAFont->XMag;
  }
}

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
int GUIPROP_X_GetCharDistX(U16P c) 
{
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;  
  for (; pProp; pProp = pProp->pNext){
    if ((c >= pProp->First) && (c <= pProp->Last))
      break;
  }
  return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}

