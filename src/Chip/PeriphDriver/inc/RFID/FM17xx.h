/*
 * FM17xx.h
 *
 *  Created on: 20xxÄêxÔÂxÈÕ
 *      Author: HeHuiBo
 */

#ifndef FM17xx_H_
#define FM17xx_H_

#include "RFID\RFID.h"

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

#if (MFRC_ULTRALPRO_CPU > 0) 

char FM17xx_PPS(void);

char FM17xx_Rats(uint8_t *pOutBfr, uint8_t *pOutLen);

char FM17xx_ComCmdPro(uint8_t *pInBfr, uint8_t InLen, uint8_t *pOutBfr, uint8_t *pOutLen);
#endif

#endif

#endif /* FM17xx_H_ */
