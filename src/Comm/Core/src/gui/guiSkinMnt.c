/*
 ============================================================================
 Name        : xx.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "guiSkinMnt.h"
#include "fatmnt.h"

#pragma location = "AHB_RAM_MEMORY"
WINDOW_BMP gs_winBmpMain;

WINDOW_BMP *pWinSkinBMP = &gs_winBmpMain;

//获取父窗口句柄,即主页面句柄,保存着页面位图的指针
static WM_HWIN WM_GetParent1C(WM_HWIN hChild) {return hChild;}
static WM_HWIN WM_GetParent2C(WM_HWIN hChild) {return WM_GetParent(hChild);}
static WM_HWIN WM_GetParent3C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(hChild));}
static WM_HWIN WM_GetParent4C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(hChild)));}
static WM_HWIN WM_GetParent5C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))));}
static WM_HWIN WM_GetParent6C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))));}
static WM_HWIN WM_GetParent7C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))))));}
static WM_HWIN WM_GetParent8C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))))));}
static WM_HWIN (*WM_GET_PARENT[])(WM_HWIN) = {WM_GetParent1C,WM_GetParent2C,WM_GetParent3C,WM_GetParent4C,WM_GetParent5C,WM_GetParent6C,WM_GetParent7C,WM_GetParent8C};

int SKIN_Button(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
  switch(pDrawItemInfo->Cmd){
    case WIDGET_ITEM_CREATE:
    case WIDGET_ITEM_DRAW_BITMAP:
      break;
 
    case WIDGET_ITEM_DRAW_BACKGROUND:{

        switch(pDrawItemInfo->ItemIndex){
          case BUTTON_SKINFLEX_PI_ENABLED:
          case BUTTON_SKINFLEX_PI_PRESSED:
          case BUTTON_SKINFLEX_PI_DISABLED:
            GUI_SetColor(GUI_LIGHTGREEN|GUI_DARKGRAY);
            GUI_AA_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, 6);
          break;
          
          case BUTTON_SKINFLEX_PI_FOCUSSED:{
            GUI_SetColor(0xA8A851);
            GUI_AA_DrawRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, 6);
          }
          break;
          default:{
            BUTTON_DrawSkinFlex(pDrawItemInfo);
          }
           break;
        }
      }
      break;
      
    case WIDGET_ITEM_DRAW_TEXT:
      return BUTTON_DrawSkinFlex(pDrawItemInfo);
      
    default :
      return BUTTON_DrawSkinFlex(pDrawItemInfo);
  }
  
  return 0;
}

int SKIN_Button2(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
  switch(pDrawItemInfo->Cmd){
    case WIDGET_ITEM_CREATE:
    case WIDGET_ITEM_DRAW_BITMAP:
      break;
 
    case WIDGET_ITEM_DRAW_BACKGROUND:{

        switch(pDrawItemInfo->ItemIndex){
          case BUTTON_SKINFLEX_PI_ENABLED:
          case BUTTON_SKINFLEX_PI_PRESSED:
          case BUTTON_SKINFLEX_PI_DISABLED:
            
          break;
          
          case BUTTON_SKINFLEX_PI_FOCUSSED:{
            GUI_SetColor(0xA8A851);
            GUI_AA_DrawRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, 6);
          }
          break;
          default:{
            BUTTON_DrawSkinFlex(pDrawItemInfo);
          }
           break;
        }
      }
      break;
      
    case WIDGET_ITEM_DRAW_TEXT:
      return BUTTON_DrawSkinFlex(pDrawItemInfo);
      
    default :
      return BUTTON_DrawSkinFlex(pDrawItemInfo);
  }
  
  return 0;
}

static void RADIO_SetUserClip(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	GUI_RECT r;
	r.x0 = pDrawItemInfo->x0; r.y0 = pDrawItemInfo->y0;
	r.x1 = pDrawItemInfo->x1; r.y1 = pDrawItemInfo->y1;
	WM_SetUserClipRect(&r);
}

//Radio控件的自定义绘制函数
static int SKIN_Radio(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BUTTON:
	//case WIDGET_ITEM_DRAW_FOCUS:
	//case WIDGET_ITEM_DRAW_TEXT:
	case WIDGET_ITEM_GET_BUTTONSIZE:
		break;
	default: return RADIO_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BUTTON:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		//获取此控件相对于主页面(比如:WINDOW)位置偏移坐标
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		RADIO_SetUserClip(pDrawItemInfo);//设置1个用户剪切区
		if(RADIO_GetValue(pDrawItemInfo->hWin) != pDrawItemInfo->ItemIndex){
          SKIN_ShowStreamBitMap(pWinSkinBMP->normalpath,  x0, y0);
          //GUI_DrawBitmap(pWinSkinBMP->normal, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
        }else{
          SKIN_ShowStreamBitMap(pWinSkinBMP->maskpath,  x0, y0);
          //GUI_DrawBitmap(pWinSkinBMP->mark, x0, y0);
        }
        WM_SetUserClipRect(0);
		break;
	}
	return 0;
}

//Radio控件的自定义绘制函数,位于第2层(比如:WINDOW->RADIO)
int SKIN_Radio2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Radio(pDrawItemInfo, 2);
}
//Radio控件的自定义绘制函数,位于第3层(比如:FRAMEWIN->CLIENT->RADIO)
int SKIN_Radio3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Radio(pDrawItemInfo, 3);
}
//Radio控件的自定义绘制函数,位于第4层(比如:WINDOW->FRAMEWIN->CLIENT->RADIO)
int SKIN_Radio4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Radio(pDrawItemInfo, 4);
}
//Radio控件的自定义绘制函数,位于第5层(比如:WINDOW->RADIO->CLIENT->WINDOW->RADIO)
int SKIN_Radio5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Radio(pDrawItemInfo, 5);
}

static void MULTIPAGE_SetUserClip(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	GUI_RECT r;
	r.x0 = pDrawItemInfo->x0; r.y0 = pDrawItemInfo->y0;
	r.x1 = pDrawItemInfo->x1; r.y1 = pDrawItemInfo->y1;
	WM_SetUserClipRect(&r);
}

//Multipage控件的自定义绘制函数
int SKIN_Multipage(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BACKGROUND:
	case WIDGET_ITEM_DRAW_FRAME:
	//case WIDGET_ITEM_DRAW_TEXT:
		break;
	default: return MULTIPAGE_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BACKGROUND:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		//获取此控件相对于主页面(比如:WINDOW)位置偏移坐标
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) - WM_GetWindowOrgY(pDrawItemInfo->hWin);
		MULTIPAGE_SetUserClip(pDrawItemInfo);//设置1个用户剪切区
		if(pDrawItemInfo->ItemIndex == ((MULTIPAGE_SKIN_INFO *)pDrawItemInfo->p)->Sel){
          SKIN_ShowStreamBitMap(pWinSkinBMP->maskpath,  x0, y0);
          //GUI_DrawBitmap(pWinSkinBMP->mark, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
        }else{
          //GUI_DrawBitmap(pWinSkinBMP->normal, x0, y0);
          SKIN_ShowStreamBitMap(pWinSkinBMP->normalpath,  x0, y0);
        }
		WM_SetUserClipRect(0);
		break;

	}
	return 0;
}

//Multipage控件的自定义绘制函数,位于第2层(比如:WINDOW->MULTIPAGE)
int SKIN_Multipage2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Multipage(pDrawItemInfo, 2);
}
//Multipage控件的自定义绘制函数,位于第3层(比如:FRAMEWIN->CLIENT->MULTIPAGE)
int SKIN_Multipage3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Multipage(pDrawItemInfo, 3);
}
//Multipage控件的自定义绘制函数,位于第4层(比如:WINDOW->FRAMEWIN->CLIENT->MULTIPAGE)
int SKIN_Multipage4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Multipage(pDrawItemInfo, 4);
}
//Multipage控件的自定义绘制函数,位于第5层(比如:WINDOW->MULTIPAGE->CLIENT->WINDOW->MULTIPAGE)
int SKIN_Multipage5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Multipage(pDrawItemInfo, 5);
}

//Multipage子页面背景重绘函数
static int SKIN_Multipage_Paint(WM_HWIN hPage, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;

	hWin = (WM_GET_PARENT[C-1])(hPage);//位于第几层修改这里(WM_GetParent数=层数-1)
	//获取此控件相对于主页面(比如:WINDOW)位置偏移坐标
	x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(hPage);
	y0 = WM_GetWindowOrgY(hWin) - WM_GetWindowOrgY(hPage);
    SKIN_ShowStreamBitMap(pWinSkinBMP->normalpath,  x0, y0);
	//GUI_DrawBitmap(pWinSkinBMP->normal, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
	return 0;
}

//Multipage子页面背景重绘函数,位于第4层(比如:WINDOW->MULTIPAGE->CLIENT->WINDOW)
int SKIN_Multipage_Paint4C(WM_HWIN hPage)
{
	return SKIN_Multipage_Paint(hPage, 4);
}
//Multipage子页面背景重绘函数,位于第5层(比如:FRAMEWIN->CLIENT->MULTIPAGE->CLIENT->WINDOW)
int SKIN_Multipage_Paint5C(WM_HWIN hPage)
{
	return SKIN_Multipage_Paint(hPage, 5);
}


//Checkbox控件的自定义绘制函数
static int SKIN_Checkbox(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BUTTON:
	//case WIDGET_ITEM_DRAW_BITMAP:
	//case WIDGET_ITEM_DRAW_FOCUS:
	//case WIDGET_ITEM_DRAW_TEXT:
		break;
	default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BUTTON:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		//获取此控件相对于主页面(比如:背景WM_HBKWIN->WINDOW)位置偏移坐标
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
        SKIN_ShowStreamBitMap(pWinSkinBMP->normalpath,  x0, y0);
		//GUI_DrawBitmap(pWinSkinBMP->normal, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
		break;
	case WIDGET_ITEM_DRAW_BITMAP://选择打勾
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
        SKIN_ShowStreamBitMap(pWinSkinBMP->maskpath,  x0, y0);
		//GUI_DrawBitmap(pWinSkinBMP->mark, x0, y0);
		break;
	case WIDGET_ITEM_DRAW_FOCUS:
		CHECKBOX_DrawSkinFlex(pDrawItemInfo);
		break;

	}
	return 0;
}

//Checkbox控件的自定义绘制函数,位于第2层(比如:背景WM_HBKWIN->WINDOW->CHECKBOX)
int SKIN_Checkbox2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Checkbox(pDrawItemInfo, 2);
}
//Checkbox控件的自定义绘制函数,位于第3层(比如:背景WM_HBKWIN->FRAMEWIN->CLIENT->CHECKBOX)
int SKIN_Checkbox3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Checkbox(pDrawItemInfo, 3);
}
//Checkbox控件的自定义绘制函数,位于第4层(比如:背景WM_HBKWIN->WINDOW->FRAMEWIN->CLIENT->CHECKBOX)
int SKIN_Checkbox4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Checkbox(pDrawItemInfo, 4);
}
//Checkbox控件的自定义绘制函数,位于第5层(比如:背景WM_HBKWIN->WINDOW->MULTIPAGE->CLIENT->WINDOW->CHECKBOX)
int SKIN_Checkbox5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	return SKIN_Checkbox(pDrawItemInfo, 5);
}

#if 1
#pragma location = "AHB_RAM_MEMORY"
char acBmpBuffer[1024*4];

static int GetBmpData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off)
{
	static int FileAddress = 0;
	UINT NumBytesRead;
	FIL *PicFile;

	PicFile = (FIL *)p;
	if (NumBytes > sizeof(acBmpBuffer)) {
      NumBytes = sizeof(acBmpBuffer);
	}

	if(Off == 1){
      FileAddress = 0;
    }else{
      FileAddress = Off;
    } 
	f_lseek(PicFile, FileAddress);

	f_read(PicFile, acBmpBuffer, NumBytes, &NumBytesRead);

	*ppData = (const U8 *)acBmpBuffer;

	return NumBytesRead;
}

void SKIN_ShowStreamBitMapEx(const char * pcFilename, int x, int y)
{
  
  g_FatFsMnt.mRes = f_open(&g_FatFsMnt.mFile, pcFilename, FA_READ);
  if(FR_OK != g_FatFsMnt.mRes){
    return ;
  }
  
  GUI_BMP_DrawEx(GetBmpData, &g_FatFsMnt.mFile, x, y);
  
  f_close(&g_FatFsMnt.mFile);

}
#endif

void SKIN_ShowStreamBitMap(const char * pcFilename, int x, int y)
{
  GUI_HMEM hMem;
  GUI_BITMAP BitMap;
  GUI_LOGPALETTE Palette;
  char * pcBuffer;
  g_FatFsMnt.mRes = f_open(&g_FatFsMnt.mFile, pcFilename, FA_OPEN_EXISTING | FA_READ);
  if(FR_OK != g_FatFsMnt.mRes){
    return ;
  }
  
  hMem = GUI_ALLOC_AllocZero(g_FatFsMnt.mFile.fsize);
  pcBuffer = GUI_ALLOC_h2p(hMem);
  g_FatFsMnt.mRes = f_read(&g_FatFsMnt.mFile, pcBuffer, g_FatFsMnt.mFile.fsize, &g_FatFsMnt.br);
  if(FR_OK != g_FatFsMnt.mRes){
    return ;
  }

  GUI_CreateBitmapFromStream(&BitMap, &Palette, pcBuffer);
  GUI_DrawBitmap(&BitMap, x, y);
  
  f_close(&g_FatFsMnt.mFile);
  GUI_ALLOC_Free(hMem);
}



