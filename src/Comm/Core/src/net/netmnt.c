/*
 ============================================================================
 Name        : netmnt.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : ÍøÂç
 ============================================================================
 */
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "Param.h"
#include "netio.h"
#include "libs.h"
#include "Kernel.h"
#include "StartUpCfg.h"

#if  defined ( __CC_ARM )
#include "stm32f4xx.h"
#endif
#include "FontMnt.h"


#define NETCOMM_FRAMEBUFFER_SIZE    256

#define TCPCLIENT_NUM   20
struct _tag_TCPCLIENTITEM{
  int mCnFd;
  unsigned int mAckTick;
};

enum _tagITEMNUM{
  eITEMFLAG_UDP = 1,
  eITEMFLAG_DBSEVER,
  eITEMFLAG_SEVER,
  eITEMFLAG_MAX
};

typedef struct _tag_NetMntParamTYPE{
  int mTcpSerFd;
  int mUdpFd;
  
  struct sockaddr_in mRxSockAddr;
  struct _tag_TCPCLIENTITEM mTcpCltItem[TCPCLIENT_NUM];
  uint16_t mTcpCltItemID[TCPCLIENT_NUM];
  int mTcpCltItemCnMap;
  
  int mRxSize, mRxDBSize;
  unsigned char mRxBfr[NETCOMM_FRAMEBUFFER_SIZE];
  unsigned char mTxBfr[NETCOMM_FRAMEBUFFER_SIZE];
  unsigned char mFrameBfr[NETCOMM_FRAMEBUFFER_SIZE];
  
}NetMntTYPE, *pNetMntTYPE;

#pragma location = "AHB_RAM_MEMORY"
static NetMntTYPE NetMntVar;

void NetMntInit(pNetMntTYPE pEnv){
  int port = pDevParam->mUdpPort;
  int mFd = 0;
  struct sockaddr_in sock;
  
  mFd = UdpOpen(&sock, port);
  pEnv->mUdpFd = mFd;
  
  mFd = TcpCreate(pDevParam->mTcpPort);
  listen(mFd, 5);
  pEnv->mTcpSerFd = mFd;
  
  pEnv->mTcpCltItemCnMap = 0;
  
}

/************************NetMntTask*****************************/
void NetMntTask(void *arg)
{
  memset(&NetMntVar, 0, sizeof(NetMntTYPE));

  for(;;){
   
  }
}

static void startNetCommTask(void){
  //sys_thread_new("Net", NetMntTask, NULL, TASK_STACK_NETMNT, TASK_PRIO_NETMNT);
}

void setupNetEnv(void){
  startEthernet();
  
  startNetCommTask();
}


