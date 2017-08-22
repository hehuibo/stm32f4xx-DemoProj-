/*
 * NVIC.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef NVIC_H_
#define NVIC_H_

#if defined (STM32F40_41xxx)
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif

void vNVICConfigure(IRQn_Type Irq, uint8_t nPriority);

#endif /* NVIC_H_ */
