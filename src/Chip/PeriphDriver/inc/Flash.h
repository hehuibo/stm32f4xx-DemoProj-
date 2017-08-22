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
   
  iFlashCmd_WriteStatus         = 0x01,
  iFlashCmd_Write               = 0x02,  /*Page Program*/
  iFlashCmd_Read                = 0x03,
  iFlashCmd_WriteDisable        = 0x04,
  iFlashCmd_ReadStatus          = 0x05,
  iFlashCmd_WriteEnable         = 0x06,
  
  iFlashCmd_Erase4K             = 0x20, /*Sector Erase*/
  iFlashCmd_Erase32K            = 0x52, /* 32k Block Erase*/
  iFlashCmd_Erase64K            = 0xD8, /*64K Block Erase*/
  iFlashCmd_EraseAll            = 0xC7, /*Chip Erase*/
  
  iFlashCmd_DeviceID            = 0x9F,
  
  /**/
  eFlash_DummyByte              = 0x00,
};

#if defined(FreeRTOS_Kernel)
#define FlashDelayms(x)  vTaskDelay(x)   
#else
#define FlashDelayms(x)
#endif

typedef struct _tag_FlashCSCtrlVal
{
  GPIO_TypeDef* mGPIOx;
  unsigned int mBasePin;
}FlashCSCtrlValTYPE;

typedef uint8_t(*pfFlashTxRxFuncTYPE[])(uint8_t);
typedef void (*pfFlashSpiFuncTYPE[])(void);

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
#define GUIFONT_FLASHCS     eFLASH_ID_CS2

#define FLASH_CS0_PIN		GPIO_Pin_4
#define FLASH_CS0_PORT		GPIOE
#define FLASH_CS0_GPIOCLK	RCC_AHB1Periph_GPIOE


#define FLASH_CSPORT_CLK    (FLASH_CS0_GPIOCLK)

#define Flash_TxRxByte	SPI1_TxRxByte
#define Flash_SetSpeed	SPI1_SetSpeed


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

extern const struct data_flash_dev data_flash_ids[];

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
