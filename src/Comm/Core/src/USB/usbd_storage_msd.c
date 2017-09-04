/**
  ******************************************************************************
  * @file    usbd_storage_msd.c
  * @author  MCD application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file provides the disk operations functions.
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
#include "usbd_msc_mem.h"
#include "usb_conf.h"
#include "SDIO.h"
#include "Flash.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

#define LUN_SD		    0
#define LUN_SPIFLASH	1


/** @defgroup STORAGE 
  * @brief media storage application module
  * @{
  */ 

/** @defgroup STORAGE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Defines
  * @{
  */ 

#define STORAGE_LUN_NBR                  1 
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Variables
  * @{
  */ 
/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', /* Product      : 16 Bytes */
  'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
  '1', '.', '0' ,'0',                     /* Version      : 4 Bytes */
}; 

/**
  * @}
  */ 


/** @defgroup STORAGE_Private_FunctionPrototypes
  * @{
  */ 
int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun, 
                           uint32_t *block_num, 
                           uint32_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);


USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata,
};

USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;
#ifndef USE_STM3210C_EVAL   
extern SD_CardInfo SDCardInfo;
#endif
__IO uint32_t count = 0;
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Functions
  * @{
  */ 


/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */

int8_t STORAGE_Init (uint8_t lun)
{

#if 0
  if( SD_Init() != SD_OK)
  {
    return (-1); 
  } 
#ekse
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE_FUNCTION();
  #endif 
  Flash_Init();
  struct _tag_DataFlash* pDataFlash = getCurrFlash(TFT_FLASHCS);
  if(pDataFlash->id != getFlashID(TFT_FLASHCS)){
    return -1;
  }
#endif
  return (0);
  
}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{

#if 0
  if(SD_GetStatus() != 0 )
  {
    return (-1); 
  }   
  
  *block_size =  512;  
  *block_num =  SDCardInfo.CardCapacity / 512;  
#else 
  *block_size =  4096;  
  *block_num =  2048;//1536; //8M:2048, 16M 4096
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE_FUNCTION();
  #endif 
#endif  
  return (0);
  
}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{
#if 0
  if( SD_Init() != 0)
  {
    return (-1);
  }  
#endif
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE_FUNCTION();
  #endif 
  return (0);
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE_FUNCTION();
  #endif 
  return  0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Read (uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_addr,                       
                 uint16_t blk_len)
{
#if 0 
  if( SD_ReadMultiBlocks (buf, 
                          blk_addr * 512, 
                          512,
                          blk_len) != 0)
  {
    return -1;
  }
  SD_WaitReadOperation();
  while (SD_GetStatus() != SD_TRANSFER_OK);
#else
  //blk_addr += 512;
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE_FUNCTION();
  #endif 
  FlashRead(blk_addr << 12, buf, blk_len<<12, TFT_FLASHCS);
  
#endif  
  return 0;
}
/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
#if 0  
  if( SD_WriteMultiBlocks (buf, 
                           blk_addr * 512, 
                           512,
                           blk_len) != 0)
  {
    return -1;
  }
  SD_WaitWriteOperation();
  while (SD_GetStatus() != SD_TRANSFER_OK);  
#else
  //blk_addr += 512;
  FlashErase4K(blk_addr << 12, TFT_FLASHCS);
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
  dbgTRACE_FUNCTION();
  #endif 
  FlashWrite(blk_addr << 12, buf, blk_len<<12, TFT_FLASHCS);
#endif
  
  return (0);
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun (void)
{
  return (STORAGE_LUN_NBR - 1);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

