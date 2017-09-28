#ifndef _MFRC522_H
#define _MFRC522_H

#include "RFID\RFID.h"

#if defined(RFID_CHIP_RC522)
char PcdHalt(void);
char PcdReset(void);
void MFRC522_Init(void);
char PcdSelect(unsigned char *pSnr);
char PcdAnticoll(unsigned char *pSnr);
signed char M500PcdConfigISOType(unsigned char type);
char PcdRequest(unsigned char req_code,unsigned char *pTagType);          
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);     

char PcdRead(unsigned char addr,unsigned char *pData);     
char PcdWrite(unsigned char addr,unsigned char *pData);    
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);  
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue);            
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);

#if(MFRC_ULTRALPRO_CPU > 0) 
char PcdRATS(uint8_t *pOutBfr, uint8_t *pOutLen);
char PcdComCmdPro(uint8_t mode, uint8_t *pInBfr, uint8_t inLen, uint8_t *pOutBfr, uint8_t *pOutLen);
#endif

#endif

#endif
