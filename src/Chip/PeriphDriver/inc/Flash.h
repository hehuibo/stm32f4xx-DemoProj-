/*
 ============================================================================
 Name        : flash.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#ifndef _FLASH_H
#define _FLASH_H

#if defined (STM32F40_41xxx)
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif

#include "TypesCfg.h"
#include "SPI.h"

#define FLASHSECTORSIZE			( 0x1000 )
#define FLASHSECTOR32KSIZE		( 32 * 1024 )
#define FLASHSECTOR64KSIZE		( 64 * 1024 )
#define FLASHSECTORMASK			( ( 0x1000 ) - 1 )
#define FLASHSECTOR32KMASK		( ( 32 * 1024 ) - 1 )
#define FLASHSECTOR64KMASK		( ( 64 * 1024 ) -  1 )
#define FLASHSECTORSIZE	        ( 0x1000 )

#define LISTBLKSIZE	             4096
#define FLASHSECT(Addr)			((Addr)>>12)
#define FLASHSECTADDR( addr ) 	(( addr )&(~(LISTBLKSIZE-1) )) /*该地址所在扇区的首地址*/

enum _tag_eFlashCmd
{
   
  eFlashCmd_WriteStatus         = 0x01,
  eFlashCmd_Write               = 0x02,  /*Page Program*/
  eFlashCmd_Read                = 0x03,
  eFlashCmd_WriteDisable        = 0x04,
  eFlashCmd_ReadStatus          = 0x05,
  eFlashCmd_WriteEnable         = 0x06,
  
  eFlashCmd_Erase4K             = 0x20, /*Sector Erase*/
  eFlashCmd_Erase32K            = 0x52, /* 32k Block Erase*/
  eFlashCmd_Erase64K            = 0xD8, /*64K Block Erase*/
  eFlashCmd_EraseAll            = 0xC7, /*Chip Erase*/
  
  eFlashCmd_DeviceID            = 0x9F,
  
  /**/
  eFlash_DummyByte              = 0xFF,
};

#if defined(FreeRTOS_Kernel)
#define FlashDelayms(x)  vTaskDelay(x)   
#else
extern void SysTickDlyMs(uint16_t ms);
#define FlashDelayms(x)  //SysTickDlyMs(x)
#endif

#if defined(SPI1_DMA_TXRX) || defined(SPI2_DMA_TXRX)
#define FLASH_DMA_TXRX
#endif


typedef struct _tag_FlashCSCtrlValTYPE
{
  GPIO_TypeDef* mGPIOx;
  unsigned int mBasePin;
}xTFlashCSCtrlValTypeDef;

#if defined (FLASH_DMA_TXRX)
typedef void (*pfnFlashDMATxRxFUNCTIONAry[])(void*, void* , int);
#else
typedef uint8_t(*pfFlashTxRxFUNCTION[])(uint8_t);
#endif
typedef void (*pfFlashSpiFUNCTION[])(void);

/*********Config**********/
#define FLASH_CS_NUM    1

enum eFlashCSTYPE{
  eFLASH_ID_CS0 = 0,
  eFLASH_ID_CS1 = 1,
  eFLASH_ID_CS2 = 2,
  eFLASH_ID_CS3 = 3,
  eFLASH_ID_CS4 = 4
};

//字体存取
#define GUIFONT_FLASHCS     eFLASH_ID_CS0
#define TFT_FLASHCS         GUIFONT_FLASHCS

#define FLASH_CS0_PIN		GPIO_Pin_4
#define FLASH_CS0_PORT		GPIOE //GPIOA//
#define FLASH_CS0_GPIOCLK	RCC_AHB1Periph_GPIOE

#define FLASH_CSPORT_CLK    FLASH_CS0_GPIOCLK

/**
 * struct data_flash_dev - DATA Flash Device ID Structure
 *
 * @name:	Identify the device type
 * @id:		device ID code
 * @pagesize:	
 * @erasesize:	Size of an erase block in the flash device.
 * @chipsize:	Total chipsize in Mega Bytes
 * @options:	Bitfield to store chip relevant options
 */
struct data_flash_dev {
  char *name;
  unsigned int id;
  unsigned int idmask;
  unsigned long pagesize;
  unsigned long chipsize;
  unsigned long erasesize;
  unsigned long options;
};

#define SIZE_K(n)	( (n) *1024 )
#define SIZE_M(n)	( (n) *1024 * 1024 )

struct _tag_DataFlash
{
  unsigned int id;
  unsigned int chipsize;
  unsigned int opt;
};

void Flash_Init(void);

uint32_t getFlashID(enum eFlashCSTYPE FlashCSNum);

struct _tag_DataFlash* getCurrFlash(enum eFlashCSTYPE FlashCSNum);

bool FlashErase4K( unsigned int address, enum eFlashCSTYPE FlashCSNum);

bool FlashErase32K( unsigned int address, enum eFlashCSTYPE FlashCSNum);

bool FlashErase64K( unsigned int address, enum eFlashCSTYPE FlashCSNum);

bool FlashEraseChip(enum eFlashCSTYPE FlashCSNum);

int FlashRead(unsigned int addr, void *buffer, int len, enum eFlashCSTYPE FlashCSNum);

int FlashWrite(unsigned int addr, void *data, int len, enum eFlashCSTYPE FlashCSNum);



#endif
