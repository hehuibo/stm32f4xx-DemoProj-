/*
 * netio.h
 *
 *  Created on: 20xx.x.x
 *      Author: HeHuiBo
 */

#ifndef NETIO_H_
#define NETIO_H_

#if defined (FreeRTOS_Kernel) 
#include "lwip/tcpip.h"
#include "lwip/tcp_impl.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "TypesCfg.h"
#else
#include "stm32f4xx.h"
#include "netio.h"
#include "Param.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/tcp_impl.h"
#endif

struct netif *getEthnetIf(void);

void startEthernet(void);

#if defined (FreeRTOS_Kernel) 
int TcpCreate(int listenPort);
int UdpOpen(struct sockaddr_in *pSock, int port);
#else
void LwIP_Periodic_Handle(void);

void NetEnvInit(void);
#endif

#endif /* NETIO_H_ */
