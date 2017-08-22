/*
 ============================================================================
 Name        : netio.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "netio.h"
#include "Param.h"

/*ÒÔÌ«Íø ÍøÂç½Ó¿Ú*/
struct netif ethnetif;

#if defined (_LOADER) || defined (_NoRTOSKernel)
__IO uint32_t LocalTime = 0;
uint32_t TCPTimer = 0;
uint32_t ARPTimer = 0;
#endif

struct netif *getEthnetIf(void){
  return &ethnetif;
}

#if defined (FreeRTOS_Kernel)  
int TcpCreate(int listenPort){
  int s;
  int ret;
  
  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(s < 0){
    return -1;
  }
  
  if(listenPort != 0){
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(listenPort);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(s, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in));
    if(ret < 0){
      closesocket(s);
    }
           
  }
  return (ret < 0) ? -1 : s;
}

int UdpOpen(struct sockaddr_in *pSock, int port){
  int opt = 1;
  int s;
  int ret;
  
  memset(pSock, 0, sizeof(struct sockaddr_in));
  pSock->sin_family = AF_INET;
  pSock->sin_port = htons(port);
  pSock->sin_addr.s_addr = htonl(INADDR_ANY);
  
  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(s < 0){
    return -1;
  }
  
  ret = setsockopt(s, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
  if(ret < 0){
    
  }
  
  ret = bind(s, (struct sockaddr*)pSock, sizeof(struct sockaddr_in));
  
  return (ret < 0) ? -1 : s;
}
#endif


extern err_t ethernetif_init(struct netif *netif);
#if defined (FreeRTOS_Kernel)  
static void InitEthernet(void *arg){
  struct netif *netif = (struct netif*)arg;
#else
static void InitEthernet(void)
{
  struct netif *netif = getEthnetIf();
#endif  
  struct ip_addr ipaddr;

  struct ip_addr netmask;
  struct ip_addr gw;
  
  memcpy( netif->hwaddr , pDevParam->mac , NETIF_MAX_HWADDR_LEN );
  ipaddr.addr = htonl(pDevParam->ip);
  netmask.addr = htonl(pDevParam->mask);
  gw.addr = htonl(pDevParam->gw);
  
#if defined (_LOADER) || defined (_NoRTOSKernel)
  /*  Registers the default network interface.*/
  netif_set_default(netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, &ethernet_input));
#else  
  /*  Registers the default network interface.*/
  netif_set_default(netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input));
#endif
  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(netif);
}

void startEthernet(void){
#if defined (FreeRTOS_Kernel)  
  tcpip_init(InitEthernet, &ethnetif);
#else
  extern void vNetEnvMntInit(void);
  InitEthernet();
  
  vNetEnvMntInit();
#endif
}

#if defined (_LOADER) || defined (_NoRTOSKernel)
void LwIP_Periodic_Handle(void){
#if LWIP_TCP
  /* TCP periodic process every 250 ms */
  if (LocalTime - TCPTimer >= TCP_TMR_INTERVAL){
    TCPTimer =  LocalTime;
    tcp_tmr();
  }
#endif
  
  /* ARP periodic process every 5s */
  if ((LocalTime - ARPTimer) >= ARP_TMR_INTERVAL){
    ARPTimer =  LocalTime;
    etharp_tmr();
  }
}
#endif


