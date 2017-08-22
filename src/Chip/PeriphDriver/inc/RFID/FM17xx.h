/*
 * FM17xx.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef FM17xx_H_
#define FM17xx_H_

#include "RFID\RFIDCfg.h"

#if defined(RFID_CHIP_FM17xx)
   
void FM17xx_Init(void);

char FM17xx_Halt(void);

char FM17xx_Select(uint8_t *pSnr);

char FM17xx_AntiColl(unsigned char *pSnr);

char FM17xx_LoadkeyE2_CPY(uint8_t *uncoded_keys);

char FM17xx_Write(uint8_t Block_Adr, uint8_t *buff);

char FM17xx_Read( uint8_t Block_Adr, uint8_t *buff);

char FM17xx_Value(uint8_t mOpmode,uint8_t addr,uint8_t *pValue);

char FM17xx_Request(unsigned char req_code,unsigned char *pTagType);

char FM17xx_AuthState(uint8_t auth_mode, uint8_t addr, uint8_t *pKey, uint8_t *pSnr);
#endif

#endif /* FM17xx_H_ */
