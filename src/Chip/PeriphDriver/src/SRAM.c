/*
 ============================================================================
 Name        : SRAM.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "stm32f4xx.h"
#include "SRAM.h" 

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
#include "trace.h"
#endif

static void vFSMC_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|
                         RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
                         RCC_AHB1Periph_GPIOG, ENABLE);
  
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12

  GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12

  GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FSMC);//PF0,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FSMC);//PF1,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FSMC);//PF2,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FSMC);//PF3,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FSMC);//PF4,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FSMC);//PF5,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FSMC);//PF13,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FSMC);//PF14,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FSMC);//PF15,AF12

  GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource3,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource10,GPIO_AF_FSMC); 
  
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(0XFF<<8);//PD0,1,4,5,8~15 AF OUT
  GPIO_Init(GPIOD,&GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = (3<<0)|(0X1FF<<7);//PE0,1,7~15,AF OUT
  GPIO_Init(GPIOE,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = (0X3F<<0)|(0XF<<12); 	//PF0~5,12~15
  GPIO_Init(GPIOF,&GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = (0X3F<<0)| GPIO_Pin_10;//PG0~5,10
  GPIO_Init(GPIOG,&GPIO_InitStructure);
  

}

void vFSMC_SRAM_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  rwTiming; 
  
  vFSMC_GPIO_Configuration();
  
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);
  
  rwTiming.FSMC_AddressSetupTime = 0x00;
  rwTiming.FSMC_AddressHoldTime = 0x00; 
  rwTiming.FSMC_DataSetupTime = 0x08;
  rwTiming.FSMC_BusTurnAroundDuration = 0x00;
  rwTiming.FSMC_CLKDivision = 0x04;
  rwTiming.FSMC_DataLatency = 0x00;
  rwTiming.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; //SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rwTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &rwTiming;
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

#if 0
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)  
	{										    
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;	  
		WriteAddr++;
		pBuffer++;
	}   
}																			    

void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++;
	}  
} 


void fsmc_sram_test(void)
{  
	u32 i=0;  	  
	u8 temp=0;	   
	u8 sval=0;			   
	for(i=0;i<1024*1024;i+=2048)
	{
		FSMC_SRAM_WriteBuffer(&temp,i,1);
		temp++;
	}		  
 	for(i=0;i<1024*1024;i+=2048) 
	{
  		FSMC_SRAM_ReadBuffer(&temp,i,1);
                
		//if(i==0)sval=temp;
 		//else if(temp<=sval)break;
          #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
          //dbgTRACE("data:%d\n",(temp-sval+1)*4);  
          dbgTRACE("data:%d\n",temp);  
          #endif 
 	}					 
}	
#endif

#if 1
#if defined (TEST_SRAM)

void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
  for(; n!= 0; n--)
  {
    *(vu8 *)(Bank1_SRAM3_ADDR + WriteAddr) = *pBuffer;
  }
}

void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
  for(; n!= 0; n--)
  {
     *pBuffer++ = *(vu8 *)(Bank1_SRAM3_ADDR + ReadAddr);
     ReadAddr ++;
  }
}

#define SRAM_SIZE		(512 * 2 * 1024)
#if defined (__CC_ARM)
unsigned char testsram[SRAM_SIZE] __attribute__((at(0X68000000)));
#elif defined ( __ICCARM__ )
unsigned char testsram[SRAM_SIZE] @0x68000000;
#endif

extern void SysTickDlyMs(uint16_t ms);
void vTestSramRDWR(void)
{
  uint32_t i;
  uint32_t *pwaddr = (uint32_t *) Bank1_SRAM3_ADDR;
  uint32_t *praddr = (uint32_t *) Bank1_SRAM3_ADDR;
  #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)  
  dbgTRACE("SRAM\r\n");
  #endif 

  for(i = 0;i < SRAM_SIZE/4;i++)
  {
    *pwaddr++ = i;
    //testsram[i] = i & 0x00000ff;
  }

  for(i = 0;i < SRAM_SIZE/4;i ++)
  {
    #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)  
    dbgTRACE("%x\ti=%d\t%d\r\n",praddr, i,*praddr);
    praddr ++;
    SysTickDlyMs(100);
    #endif 
  }
#if 0 
  if(i == SRAM_SIZE)
  {
      #if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)  
      dbgTRACE("SRAM OK!\r\n");
      #endif 
  }
  
#endif
  
}

#endif

#endif

