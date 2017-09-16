/*
 ============================================================================
 Name        : xx.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Kernel.h"
#include "Param.h"
#include "GUI.h"
#include "DIALOG.h"


WM_HWIN CreateDemoWin(void);

extern void FatMntInit(void);
void guiMntTask(void *arg)
{
  FatMntInit();
  GUI_Init();
  
  WM_MULTIBUF_Enable(1);
  WM_SetCreateFlags(WM_CF_MEMDEV);
  
  CreateDemoWin();
  
  for(;;){
    GUI_Exec();           // Do the background work ... Update windows etc.)
    GUI_X_ExecIdle();     // Nothing left to do for the moment ... Idle processing
    GUI_Delay(30);
  }
}

void startGuiMntTask(void)
{
  OSThreadCreateEx(guiMntTask, NULL, eThreadTYPE_GUI);
}
