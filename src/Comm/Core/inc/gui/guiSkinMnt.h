/*
 * guiSkin.h
 *
 *  Created on: 20xx��x��x��
 *      Author: HeHuiBo
 */

#ifndef GUISKINMNT_H_
#define GUISKINMNT_H_

#include "GUI.h"
#include "DIALOG.h"
#include "guiSkinCfg.h"

typedef struct _tagWINDOWBMP
{
  const char *    normalpath;
  const char *    maskpath;
  const char *    focuspath;
  const char *    disablepath;
  const char *    thumbNpath;
  const char *    thumbMpath;
}WINDOW_BMP;

extern WINDOW_BMP *pWinSkinBMP;
extern WINDOW_BMP g_winBmpMain;

void SKIN_ShowStreamBitMapEx(const char * pcFilename, int x, int y);
void SKIN_ShowStreamBitMap(const char * pcFilename, int x, int y);

//Button
int SKIN_Button(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_Button2(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
//Radio
int SKIN_Radio2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Radio�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:WINDOW->RADIO)

int SKIN_Radio3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Radio�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:FRAMEWIN->CLIENT->RADIO)

int SKIN_Radio4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Radio�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->RADIO)

int SKIN_Radio5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Radio�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:WINDOW->RADIO->CLIENT->WINDOW->RADIO)

//Multipage
int SKIN_Multipage2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Multipage�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:WINDOW->MULTIPAGE)

int SKIN_Multipage3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Multipage�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:FRAMEWIN->CLIENT->MULTIPAGE)

int SKIN_Multipage4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Multipage�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->MULTIPAGE)

int SKIN_Multipage5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Multipage�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:WINDOW->MULTIPAGE->CLIENT->WINDOW->MULTIPAGE)

int SKIN_Multipage_Paint4C(WM_HWIN hPage);//Multipage��ҳ�汳���ػ溯��,λ�ڵ�4��(����:WINDOW->MULTIPAGE->CLIENT->WINDOW)

int SKIN_Multipage_Paint5C(WM_HWIN hPage);////Multipage��ҳ�汳���ػ溯��,λ�ڵ�5��(����:FRAMEWIN->CLIENT->MULTIPAGE->CLIENT->WINDOW)

//Checkbox
int SKIN_Checkbox2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:����WM_HBKWIN->WINDOW->CHECKBOX)

int SKIN_Checkbox3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:����WM_HBKWIN->FRAMEWIN->CLIENT->CHECKBOX)

int SKIN_Checkbox4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:����WM_HBKWIN->WINDOW->FRAMEWIN->CLIENT->CHECKBOX)

int SKIN_Checkbox5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:����WM_HBKWIN->WINDOW->MULTIPAGE->CLIENT->WINDOW->CHECKBOX)

#endif /* GUISKINMNT_H_ */
