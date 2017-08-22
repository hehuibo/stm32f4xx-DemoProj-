/*
 ============================================================================
 Name        : StFlash.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "STFLASH.h"

#define STM32_FLASH_STARTADDR       0x08000000 
#define STFLASH_PAGE_SIZE           0x20000      //128K 
#define STFLASH_END_ADDRESS         0x080FFFFF 


#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes 


uint32_t STFLASH_ReadWord(uint32_t rAddr)
{
   return  *(uint32_t*)rAddr;
}

static uint16_t STFLASH_GetFlashSector(uint32_t addr)
{
  if(addr < ADDR_FLASH_SECTOR_1){
    return FLASH_Sector_0;
    
  }else if(addr < ADDR_FLASH_SECTOR_2){
    return FLASH_Sector_1;
    
  }else if(addr < ADDR_FLASH_SECTOR_3){
    return FLASH_Sector_2;
    
  }else if(addr < ADDR_FLASH_SECTOR_4){
    return FLASH_Sector_3;
    
  }else if(addr < ADDR_FLASH_SECTOR_5){
    return FLASH_Sector_4;
    
  } else if(addr < ADDR_FLASH_SECTOR_6){
    return FLASH_Sector_5;
    
  }else if(addr < ADDR_FLASH_SECTOR_7){
    return FLASH_Sector_6;
    
  }else if(addr < ADDR_FLASH_SECTOR_8){
    return FLASH_Sector_7;
    
  }else if(addr < ADDR_FLASH_SECTOR_9){
    return FLASH_Sector_8;
    
  }else if(addr < ADDR_FLASH_SECTOR_10){
    return FLASH_Sector_9;
    
  }else if(addr < ADDR_FLASH_SECTOR_11){
    return FLASH_Sector_10; 
  }else{
	return FLASH_Sector_11;	
  }
}

int STFLASH_ProgramData(uint32_t *pwAddr,uint32_t *pData,uint32_t paramlen)	
{ 
  //扇区首地址
  
  /* 64 Kbyte */
  if(*pwAddr == ADDR_FLASH_SECTOR_4){
    if(FLASH_COMPLETE != FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3)){
      return 1;
    } 
  }
  /* 128 Kbyte */
  if(0 == (*pwAddr % STFLASH_PAGE_SIZE)){
    if(FLASH_COMPLETE != FLASH_EraseSector(STFLASH_GetFlashSector(*pwAddr),VoltageRange_3)){
      return 1;
    }
  }
  
  /*写入数据*/
  for (int i = 0; (i < paramlen) && (*pwAddr <= (STFLASH_END_ADDRESS-4)); i++){
    if (FLASH_ProgramWord(*pwAddr, *(uint32_t*)(pData+i)) == FLASH_COMPLETE)
    {
      if (*(uint32_t*)*pwAddr != *(uint32_t*)(pData+i)){
        return(2);
      }
     
      *pwAddr += 4;
    }else{
      
      return (1);
    }
  }

  return 0;
} 

