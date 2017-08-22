/*
 * param.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef PARAM_H_
#define PARAM_H_

#include "TypesCfg.h"
#include "ParamCfg.h"

#define PARAM_FLASH_ID  eFLASH_ID_CS0

/*Device Param*/
typedef  struct tag_DevParamTYPE
{
  uint16_t magic;
  uint16_t paramlen;
	
  uint16_t  mDevId;
  uint16_t  mDevType;

   /*Net*/
  uint8_t   bAutoIP;
  uint8_t   mac[6];               /*MAC*/
  uint16_t  mUdpPort;
  uint16_t  mDBTcpPort;
  uint16_t  mTcpPort;
  uint32_t  DBip;
  uint32_t  ip;                  /*IP*/
  uint32_t  gw;                  /*gw*/
  uint32_t  mask;                /*mask*/
  
  /*Font Addr*/
  uint32_t mFontGBK;
  
  /*RS485*/
  uint16_t  mRS485Addr;
  uint32_t  mRS485BaundRate;
  
  uint8_t mResEnd[2];
}xTDevParamTypeDef;


extern xTDevParamTypeDef *pDevParam;

void InitParam(void);

#endif /* PARAM_H_ */
