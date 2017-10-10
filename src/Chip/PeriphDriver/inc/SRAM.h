/*
 ============================================================================
 Name        : SRAM.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#ifndef _SRAM_H
#define _SRAM_H

void vFSMC_SRAM_Init(void);

#define TEST_SRAM  //�ĺ겻����memgr�ļ����ã��������ʾ��ַ�ص�

#if defined (TEST_SRAM)
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	

void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 NumHalfwordToWrite);
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 NumHalfwordToRead);
void vTestSramRDWR(void);

#endif


#endif
