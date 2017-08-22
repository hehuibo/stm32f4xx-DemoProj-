/*
 ============================================================================
 Name        : xx.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

/**************Tcp*****************/
static void vTcpServerCloseConn(struct tcp_pcb *pcb)
{
  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, NULL);
  tcp_close(pcb);
}

err_t TcpServerRecv(void *arg, struct tcp_pcb *pcb,struct pbuf *p, err_t err)
{
  if((ERR_OK == err) && (p != NULL)){
    tcp_recved(pcb, p->tot_len);
    
    memcpy(g_AppCommBfrMnt.pRxBfr, p->payload,  p->len);
    g_AppCommBfrMnt.mRxLen = p->len;
    ProgramFrameMnt(&g_AppCommBfrMnt);
    memcpy(p->payload, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen);
    tcp_write(pcb, g_AppCommBfrMnt.pTxBfr, g_AppCommBfrMnt.mTxLen, 0);
    
  }else if (err == ERR_OK && p == NULL){
    vTcpServerCloseConn(pcb);
  }
  
  pbuf_free(p);
  
  return ERR_OK;
}

err_t TcpServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  tcp_recv(pcb, TcpServerRecv);
  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  
  return ERR_OK;
}
void vTcpCommEnvMntInit(void){
   struct tcp_pcb *tpcb;
  err_t err;
  
  tpcb = tcp_new();
  if(tpcb){
    err = tcp_bind(tpcb, IP_ADDR_ANY, pDevParam->mTcpPort);
    if(ERR_OK == err){
      tpcb = tcp_listen(tpcb);
      tcp_accept(tpcb, TcpServerAccept);
    }else{
      tcp_close(tpcb);
    }
  }
  
}

void vNetEnvMntInit(void)
{
  vTcpCommEnvMntInit();
  
}


