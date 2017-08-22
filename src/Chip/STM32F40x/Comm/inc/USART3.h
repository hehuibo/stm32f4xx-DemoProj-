/*
 * USART3.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef USART3_H_
#define USART3_H_

#include "stm32f4xx.h"
#include "TypesCfg.h"

void vUSART3_Configure(uint32_t nBaudRate);

void USART3_TxDat(uint8_t *pStr, uint16_t len);

void Serial3_TxPrint(char* fmt, ...);

#endif /* USART3_H_ */
