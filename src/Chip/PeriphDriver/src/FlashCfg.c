/*
 ============================================================================
 Name        : FLASH IDs 
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "flash.h"

/*
 *
 */
const struct data_flash_dev data_flash_ids[]=
{
/*	name-------------id--------mask-------page-----size--------erasesize-opt --- */
/*
 *	SST芯片
 */
	{ "SST25VF080",   0x8E25BF, 0xFFFFFF,  0x1000,  SIZE_K(512), 0x1000,  1	},	
	{ "SST25VF016",   0x4125BF, 0xFFFFFF,  0x1000,  SIZE_M(2), 	0x1000,  1	},
	{ "SST25VF032",   0x4a25BF, 0xFFFFFF,  0x1000,  SIZE_M(4), 	0x1000,  1	},	
/*
 *	Atemal设备
 *	第一字节	0x1F	厂商
 *	第二字节	ID1
 *	第三字节	ID2
 *		0x4		AT26 Serias
 *		4/5/6	Density	
 */
	{ "AT26DF041A",		0x441F,  0xE0FFFF,  0x1000,  SIZE_K(512), 0x1000,  0	},	
	{ "AT26DF081A",		0x451F,  0xE0FFFF,  0x1000,  SIZE_M(1)  , 0x1000,  0	},	
	{ "AT26DF161A", 	0x461F,  0xE0FFFF,  0x1000,  SIZE_M(2)  , 0x1000,  0	},	
	{ "AT26DF321A",		0x471F,  0xE0FFFF,  0x1000,  SIZE_M(4)  , 0x1000,  0	},	

	//4125bf
	{ "AT25VF016A",		0x471F,  0xE0FFFF,  0x1000,  SIZE_M(4)  , 0x1000,  0	},	
	
//	bf4125bf

/*
 *	MX芯片
 */
	{ "MX25L3206E",   0xc22016, 0xFFFFFF,  0x1000,  SIZE_M(4)  , 0x1000,  0	},	
	{ "MX25L6406E",   0x1720c2, 0xFFFFFF,  0x1000,  SIZE_M(8)  , 0x1000,  0	},	
	{ "MX25L3206E",   0x1620c2, 0xFFFFFF,  0x1000,  SIZE_M(4)  , 0x1000,  0	},	
	{ "MX25L12835F",  0x1820c2, 0xFFFFFF,  0x1000,  SIZE_M(16)  , 0x1000,  0	},	
/*
 *	结束
 */
	{0,},
};

//FlashCS
const FlashCSCtrlValTYPE gsFlashCSCtrlValAryTbl[FLASH_CS_NUM] = {
  {FLASH_CS0_PORT, FLASH_CS0_PIN},  /*eFLASH_ID_CS0*/
};

//Flash SPI
const pfFlashTxRxFuncTYPE pfFlashTxRxAryTbl = {
  SPI1_TxRxByte,
};

#if defined (FreeRTOS_Kernel) 
const pfFlashSpiFuncTYPE pfFlashSpiLockFuncAryTbl = {
  SPI1_Lock,
};

const pfFlashSpiFuncTYPE pfFlashSpiUnLockFuncAryTbl = {
  SPI1_Unlock,
};
#endif

void FlashPortInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(FLASH_CSPORT_CLK, ENABLE);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_InitStructure.GPIO_Pin = FLASH_CS0_PIN;
  GPIO_Init(FLASH_CS0_PORT, &GPIO_InitStructure);
}


