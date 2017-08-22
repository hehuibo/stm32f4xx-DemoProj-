/*
 * STFLASH.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef STFLASH_H_
#define STFLASH_H_

#include "stm32f4xx.h"

uint32_t STFLASH_ReadWord(uint32_t rAddr);

int STFLASH_ProgramData(uint32_t *pwAddr,uint32_t *pData,uint32_t paramlen);

#endif /* STFLASH_H_ */
