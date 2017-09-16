/*
 * TFT.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef TFT_H_
#define TFT_H_

#include "TypesCfg.h"
#include "stm32f4xx.h"

#define TFT_XSIZE_PHYS  480
#define TFT_YSIZE_PHYS  272

/*
RS: A16 
CmdAddr: 0x6x000000 + 1<<(n+1)
*/
#define TFT_ADDR_BASE    ((uint32_t)0x6C000000)
//#define TFT_REG          *(__IO uint16_t *)(TFT_ADDR_BASE +  (1 << (16 + 1)))	
#define TFT_REG          *(__IO uint16_t *)(TFT_ADDR_BASE +  (1 << (15 + 1)))	
#define TFT_RAM          *(__IO uint16_t *)(TFT_ADDR_BASE)

void TFT_WaitBusy(void);

#define TFT_WRCmd(mCmd)         {TFT_WaitBusy();TFT_REG = mCmd;}
#define TFT_WRData(mData)       (TFT_RAM  = mData)

void TFT_Init(void);

#endif /* TFT_H_ */
