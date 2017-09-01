/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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
#include "usbd_usr.h" 
#include "usb_dcd_int.h"


#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
#include "trace.h"
#endif
/** @addtogroup USBD_USER
  * @{
  */

/** @addtogroup USBD_MSC_DEMO_USER_CALLBACKS
  * @{
  */
  
/** @defgroup USBD_USR 
  * @brief    This file includes the user application layer
  * @{
  */ 

/** @defgroup USBD_USR_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 
extern USB_OTG_CORE_HANDLE  USB_OTG_dev;


void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}


/** @defgroup USBD_USR_Private_Variables
  * @{
  */ 
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

/**
  * @}
  */

/** @defgroup USBD_USR_Private_Constants
  * @{
  */ 

#define USER_INFORMATION1  (uint8_t*)"INFO : Single Lun configuration"
#define USER_INFORMATION2  (uint8_t*)"INFO : microSD is used"
/**
  * @}
  */


/** @defgroup USBD_USR_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Functions
  * @{
  */ 

/**
* @brief  Displays the message on LCD on device lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{
#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("> USBD_USR_Init \r\n" ); 
#endif
}

/**
* @brief  Displays the message on LCD on device reset event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset (uint8_t speed)
{
 switch (speed)
 {
   case USB_OTG_SPEED_HIGH: 
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
    dbgTRACE(" USB Device Library v1.2.0  [HS]\r\n" ); 
    #endif
     break;

  case USB_OTG_SPEED_FULL: 
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE ("     USB Device Library v1.2.0  [FS]\r\n" );
      #endif
     break;
 default:
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
      dbgTRACE("     USB Device Library v1.2.0  [??]\r\n" );
      #endif
     break;
 }
}


/**
* @brief  Displays the message on LCD on device config event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured (void)
{
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("> MSC Interface started.\r\n");
  #endif
}
/**
* @brief  Displays the message on LCD on device suspend event 
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("> Device In suspend mode.\r\n");
  #endif 
}


/**
* @brief  Displays the message on LCD on device resume event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("> Device Resumed.\r\n");
  #endif 
  
}

/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected (void)
{
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("> USB Device Connected.\r\n");
  #endif 
  
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected (void)
{
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE("> USB Device Disconnected.\n");
  #endif 
  
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
