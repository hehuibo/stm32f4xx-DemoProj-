/*********************************************************************
*          Portions COPYRIGHT 2015 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.28 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : This file provides emWin Interface with FreeRTOS
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "GUI.h"
  
/* RTOS include files */
#if defined (uCosII_Kernel)|| defined (uCosIII_Kernel)//uCos
#include "includes.h"  

#elif defined (FreeRTOS_Kernel) //FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#endif
/*********************************************************************
*
* Global data
*/
#if defined (uCosII_Kernel)
static  OS_EVENT  *DispSem;
static  OS_EVENT  *EventMbox;

static  OS_EVENT  *KeySem;
static  int        KeyPressed;
static  char       KeyIsInited;

#elif defined (uCosIII_Kernel)  //uCos
static  OS_SEM  *DispSem;
static  OS_SEM  *EventSem;

static  OS_SEM  *KeySem;
static  int        KeyPressed;
static  char       KeyIsInited;

#elif defined (FreeRTOS_Kernel) //FreeRTOS

static xSemaphoreHandle xEmWinMutex;
static xSemaphoreHandle xEmWinSem;

#endif

/*********************************************************************
*
* Timing:
* GUI_X_GetTime()
* GUI_X_Delay(int)

Some timing dependent routines require a GetTime
and delay function. Default time unit (tick), normally is
1 ms.
*/

int GUI_X_GetTime(void)
{
#if defined (FreeRTOS_Kernel)
  return ((int) xTaskGetTickCount());
  
#elif defined (uCosII_Kernel)
  return ((int)OSTimeGet());
  
#elif defined (uCosIII_Kernel)//uCos-III
  OS_ERR err;
  return ((int)OSTimeGet(&err)); 
#endif
}

void GUI_X_Delay(int ms)
{
#if defined (FreeRTOS_Kernel)
  vTaskDelay( ms );
   
#elif defined (uCosII_Kernel)
  INT32U  ticks;   
  ticks = ( ms * 1000) / OS_TICKS_PER_SEC;  
  OSTimeDly((INT16U)ticks); 
  
#elif defined (uCosIII_Kernel)//uCosIII
  OS_ERR err;
  CPU_INT32U ticks;
  ticks = (ms * 1000) / OSCfg_TickRate_Hz;
  OSTimeDly(ticks,OS_OPT_TIME_PERIODIC,&err);
#endif
 
}

/*********************************************************************
*
* GUI_X_Init()
*
* Note:
* GUI_X_Init() is called from GUI_Init is a possibility to init
* some hardware which needs to be up and running before the GUI.
* If not required, leave this routine blank.
*/
#if defined (FreeRTOS_Kernel)
void GUI_X_Init(void) 
{
  
}
#endif

/*********************************************************************
*
* GUI_X_ExecIdle
*
* Note:
* Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) 
{
#if defined (uCosII_Kernel)||(defined uCosIII_Kernel) || defined (FreeRTOS_Kernel)
  GUI_X_Delay(1);
#endif
}

/*********************************************************************
*
* Multitasking:
*
* GUI_X_InitOS()
* GUI_X_GetTaskId()
* GUI_X_Lock()
* GUI_X_Unlock()
*
* Note:
* The following routines are required only if emWin is used in a
* true multi task environment, which means you have more than one
* thread using the emWin API.
* In this case the
* #define GUI_OS 1
* needs to be in GUIConf.h
*/

/* Init OS */
void GUI_X_InitOS(void)
{ 

#if defined (FreeRTOS_Kernel)
  /* Create Mutex lock */
  xEmWinMutex = xSemaphoreCreateMutex();
  configASSERT(xEmWinMutex != NULL)
  
  /* Create Semaphore lock */
  vSemaphoreCreateBinary(xEmWinSem);
  configASSERT(xEmWinSem != NULL);   
  
#elif defined (uCosII_Kernel)
  DispSem   = OSSemCreate(1);
  EventMbox = OSMboxCreate((void *)0);
  
#elif defined (uCosIII_Kernel)//uCosIII
  OS_ERR     err;

  /*  cnt = 1*/
  OSSemCreate((OS_SEM    *)&DispSem,
              (CPU_CHAR  *)"DispSem",
              (OS_SEM_CTR )1,
              (OS_ERR    *)&err);
  /*  cnt = 0*/
  OSSemCreate((OS_SEM    *)&EventSem,
              (CPU_CHAR  *)"EventSem",
              (OS_SEM_CTR )0,
              (OS_ERR    *)&err);
#endif 
}

void GUI_X_Unlock(void)
{ 
#if defined (FreeRTOS_Kernel)
  xSemaphoreGive(xEmWinMutex);
   
#elif defined (uCosII_Kernel)
 OSSemPost(DispSem);
  
#elif defined (uCosIII_Kernel)//uCosIII
  OS_ERR     err;

  OSSemPost((OS_SEM *)&DispSem,
            (OS_OPT  )OS_OPT_POST_1,
            (OS_ERR *)&err);
#endif 
}

void GUI_X_Lock(void)
{
#if defined (FreeRTOS_Kernel)
  if(xEmWinMutex == NULL){
    GUI_X_InitOS();
  }
  xSemaphoreTake(xEmWinMutex,portMAX_DELAY);
  
#elif defined (uCosII_Kernel)
  INT8U  err;
  OSSemPend(DispSem, 0, &err);
  
#elif defined (uCosIII_Kernel)//uCosIII
  OS_ERR     err;
	   
  OSSemPend((OS_SEM *)&DispSem,
            (OS_TICK )0,
	    (OS_OPT  )OS_OPT_PEND_BLOCKING,
	    (CPU_TS  )0,
	    (OS_ERR *)&err);
#endif 
}

/* Get Task handle */
U32 GUI_X_GetTaskId(void) { 
#if defined (FreeRTOS_Kernel)
  return ((U32) xTaskGetCurrentTaskHandle()); 
  
#elif defined (uCosII_Kernel)
  return ((U32)(OSTCBCur->OSTCBPrio));
  
#elif defined (uCosIII_Kernel)//uCosIII
  
  return ((U32)(OSTCBCurPtr->Prio));
#endif 

}

/*
*********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
*********************************************************************************************************
*/

void GUI_X_WaitEvent (void) 
{
#if defined (FreeRTOS_Kernel)
  while(xSemaphoreTake(xEmWinSem, portMAX_DELAY) != pdTRUE);
   
#elif defined (uCosII_Kernel)
  INT8U  err;
  OSMboxPend(EventMbox, 0, &err);
  
#elif defined (uCosIII_Kernel)//uCosIII
  OS_ERR err;
  OSSemPend(EventSem,0,OS_OPT_PEND_BLOCKING,0,&err); 
#endif 
}

void GUI_X_SignalEvent (void) 
{
#if defined (FreeRTOS_Kernel)
   xSemaphoreGive(xEmWinSem);
   
#elif defined (uCosII_Kernel)
  OSMboxPost(EventMbox, (void *)1);
  
#elif defined (uCosIII_Kernel)//uCosIII

#endif 
}

/*
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed	in the keyboard buffer. 
*          This is a neat feature which allows you to operate your target system without having to use or 
*          even to have a keyboard connected to it. (useful for demos !)
*********************************************************************************************************
*/
#if defined (uCosII_Kernel)||(defined uCosIII_Kernel)//uCos
static  void  CheckInit (void) 
{
#if defined (uCosII_Kernel)  
  if (KeyIsInited == OS_FALSE) {
        KeyIsInited = OS_FALSE;
        
#elif defined (uCosIII_Kernel)//uCosIII  
  if (KeyIsInited == DEF_FALSE) {
        KeyIsInited = DEF_TRUE;
#endif

        GUI_X_Init();
    }
}


void GUI_X_Init (void) 
{
#if defined (uCosII_Kernel)  
  KeySem = OSSemCreate(0);
  
#elif defined (uCosIII_Kernel)//uCosIII  
  OS_ERR err;
	
  OSSemCreate((OS_SEM*)&KeySem,
              (CPU_CHAR *)"KeySem",
              (OS_SEM_CTR)0,
              (OS_ERR *)&err);
#endif  
    
}


int  GUI_X_GetKey (void) 
{
  int r;
  
  r = KeyPressed;
  CheckInit();
  KeyPressed = 0;
  
  return (r);
}


int  GUI_X_WaitKey (void) 
{
  int    r;
#if defined (uCosII_Kernel) 
  INT8U  err;
#elif defined (uCosIII_Kernel)//uCosIII  
  OS_ERR err;
#endif
  
  CheckInit();
  if (KeyPressed == 0) 
  {
#if defined (uCosII_Kernel)      
    OSSemPend(KeySem, 0, &err);
#elif defined (uCosIII_Kernel)//uCosIII   
    OSSemPend((OS_SEM *)&KeySem,
              (OS_TICK )0,
              (OS_OPT  )OS_OPT_PEND_BLOCKING,
              (CPU_TS  )0,
              (OS_ERR *)&err);
#endif
  }
  
  r = KeyPressed;
  KeyPressed = 0;
  
  return (r);
}


void  GUI_X_StoreKey (int k) 
{
  KeyPressed = k;
#if defined (uCosIII_Kernel)      
  OS_ERR err;
  OSSemPost(KeySem,OS_OPT_POST_1,&err);
#elif defined (uCosII_Kernel)//uCosII   
	OSSemPost(KeySem);  
#endif
      
}

#endif

/*********************************************************************
*
* Logging: OS dependent

Note:
Logging is used in higher debug levels only. The typical target
build does not use logging and does therefor not require any of
the logging routines below. For a release build without logging
the routines below may be eliminated to save some space.
(If the linker is not function aware and eliminates unreferenced
functions automatically)

*/

void GUI_X_Log (const char *s) { }
void GUI_X_Warn (const char *s) { }
void GUI_X_ErrorOut(const char *s) { }

/*************************** End of file ****************************/
