/*
 *RFID.h
 *
 *  Created on: 20xx年x月x日
 *      Author: HeHuiBo
 */

#ifndef RFID_H_
#define RFID_H_

#include "RFID\RFIDCfg.h"

/*
   卡片操作管理函数接口
  g_rfFuncCtrl.pfHalt(void);

  g_rfFuncCtrl.pfSelect(uint8_t *);

  g_rfFuncCtrl.pfAnticoll(uint8_t *);

  g_rfFuncCtrl.pfWrite(uint8_t ,uint8_t *);

  g_rfFuncCtrl.pfRead(uint8_t ,uint8_t *);

  g_rfFuncCtrl.pfRequest(uint8_t, uint8_t*);

  g_rfFuncCtrl.pfAuthState(uint8_t ,uint8_t ,uint8_t *,uint8_t *); 
 */ 
//extern sTRfidFuncCtrlTYPE g_rfFuncCtrl; //RFIDCfg.h

void RFID_Init(void);

#endif /* RFID_H_ */
