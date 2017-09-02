/*
 ============================================================================
 Name        : Param.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "Param.h"
#include "libs.h"
#include "appcfg.h"
#include "Flash.h"

#if defined (FreeRTOS_Kernel) 
#include "Kernel.h"
#else
#define   KERNEL_ENTER_CRITICAL()
#define   KERNEL_EXIT_CRITICAL()
#endif


static uint32_t prvDevParamAddr;	

//#pragma location = "AHB_RAM_MEMORY"
xTDevParamTypeDef  gs_DevParam;

struct _tag_ParamAddrLimit{
  unsigned int addrbegin;
  unsigned int addrend;
  unsigned short size;
  unsigned short magic;
};


/**DeviceParamAddrLimit**/
const struct _tag_ParamAddrLimit DeviceParamAddrLimit = {
  DEVICEPARAM_STARD_ADDR,
  DEVICEPARAM_END_ADDR,
  DEVICEPARAM_MAX_SIZE,
  DEVICEPARAM_MAGIC
};


xTDevParamTypeDef *pDevParam =  &gs_DevParam;

/*ParamDefaultInit*/
void DefaultDevParamInit(void)
{	
  //net param
  memcpy(gs_DevParam.mac, DEFAULT_ETH_MAC, sizeof(gs_DevParam.mac));
  gs_DevParam.bAutoIP = false;
  gs_DevParam.mTcpPort = TCPSEVER_PORT;
  gs_DevParam.mUdpPort = UDPPORT_PORT;
  gs_DevParam.mDBTcpPort = DBSEVER_PORT;
  StringToIp(DEFAULT_DBSERVER_IP, &gs_DevParam.DBip);
  StringToIp(DEFAULT_ETH_IP, &gs_DevParam.ip);
  StringToIp(DEFAULT_ETH_NETMASK ,&gs_DevParam.mask) ;
  StringToIp(DEFAULT_ETH_GW ,&gs_DevParam.gw);
   
  /*RS485*/
  //gs_DevParam.mRS485Addr = 0x00;
  gs_DevParam.mRS485BaundRate = 9600;
;
  
  /*Font Addr*/
  gs_DevParam.mFontGBK = GBKFONT_STARD_ADDR;
  
  //Dev
  gs_DevParam.mDevId = 0x00;
  
  gs_DevParam.magic = DEVICEPARAM_MAGIC;
  gs_DevParam.paramlen = offsetof(xTDevParamTypeDef , mResEnd) + sizeof(gs_DevParam.mResEnd);
}

/****ParamInit****/
uint32_t DevParamInit(void){
  
  DefaultDevParamInit();
	
  return prvDevParamAddr;
}


/****************************************************/
void InitParam(void)
{
  DevParamInit();
}






