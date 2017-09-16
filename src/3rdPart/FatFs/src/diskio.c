/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "Flash.h"
/* Definitions of physical drive number for each drive */
#define DEV_FLASH		0	    //SPI FLASH 
#define DEV_MMC		    1	    //SD 	

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
      case DEV_FLASH :{
        struct _tag_DataFlash* pDataFlash = getCurrFlash(TFT_FLASHCS);
        if(pDataFlash->id == getFlashID(TFT_FLASHCS)){
          stat &= ~STA_NOINIT;
        }
      }
      break;
      default :
        break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
      case DEV_FLASH :{
        stat = disk_status(pdrv);
      }
      break;
          
      default:break;

	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	switch (pdrv) {
      case DEV_FLASH :{
        sector += 2048;/*扇区偏移8M,外部Flash文件系统存放在后8M*/
        FlashRead(sector << 12, buff, count << 12, TFT_FLASHCS);
        return RES_OK;
      }
      break;
      
    default :
      break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{

    if(!count){
      return RES_PARERR;
    }
    
	switch (pdrv) {
      case DEV_FLASH :{
        sector += 2048;/*扇区偏移8M,外部Flash文件系统存放在后8M*/
        FlashErase4K(sector << 12, TFT_FLASHCS);
        FlashWrite(sector << 12, (void *)buff, count << 12, TFT_FLASHCS);
        return RES_OK;
      }
      
    default :
      break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{

	switch (pdrv) {
      case DEV_FLASH :{
        switch(cmd){
          
          case GET_SECTOR_COUNT:/* 扇区数量: 2048*4096/1024/1024=8(MB) */
            *(DWORD * )buff = 2048;//4096;		
            return  RES_OK;
            
          
          case GET_SECTOR_SIZE :/* 扇区大小 */
            *(WORD * )buff = 4096;
            return  RES_OK;
            
          /*同时擦除扇区个数*/
          case CTRL_SYNC : 
          case CTRL_TRIM :
          case GET_BLOCK_SIZE :
            return  RES_OK;
                 
        }
       
      }
      break;
      
    default:break;
	}

	return RES_PARERR;
}

DWORD get_fattime(void) {
	/* 返回当前时间戳*/
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

