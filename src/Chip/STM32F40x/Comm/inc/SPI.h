/*
 ============================================================================
 Name        : SPI.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#ifndef _SPI_H
#define _SPI_H

#if defined (FreeRTOS_Kernel)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

#if defined (STM32F40_41xxx)
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
//#define SPI1_PIN_REAMP
#endif

void SPI1_Configure(void);

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);

uint8_t SPI1_TxRxByte(uint8_t Dat);

void SPI2_Configure(void);

void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler);

uint8_t SPI2_TxRxByte(uint8_t Dat);

#if defined (FreeRTOS_Kernel)
void SPI1_Lock(void);
void SPI1_Unlock(void);
void SPI2_Lock(void);
void SPI2_Unlock(void);
void SPI_Wait(void);
#else
#define	SPI1_Lock()
#define SPI1_Unlock()
#define	SPI2_Lock()
#define SPI2_Unlock()
#define SPI_Wait()
#endif


#endif
