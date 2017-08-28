#ifndef _APPCFG_H
#define _APPCFG_H

#if defined (FreeRTOS_Kernel) 
/****Task Param****/
#define TASK_PRIO_MSGMNT	8 /*MsgMnt      Task  */
#define TASK_PRIO_ETH_INPUT	7 /*eth Input   Task  */
#define TASK_PRIO_NETMNT	5 /*NetComm     Task  */
#define TASK_PRIO_GUI		2 /*GUI         Task  */
#define TASK_PRIO_INIT		1 /*Init        Task  */


/****Task Stack Size****/
#define TASK_STACK_INIT         256
#define TASK_STACK_GUI          512
#define TASK_STACK_MSGMNT       256
#define TASK_STACK_NETMNT       256
#define TASK_STACK_ETH_INPUT    256
#endif


#define DBSEVER_PORT    8090
#define TCPSEVER_PORT   9900 
#define UDPPORT_PORT	4008

#define DEFAULT_DBSERVER_IP     "192.168.10.204"

#define DEFAULT_ETH_IP          "192.168.10.29"
#define DEFAULT_ETH_NETMASK     "255.255.255.0"
#define DEFAULT_ETH_GW          "192.168.10.1"
#define DEFAULT_ETH_MAC         "\x21\x23\x24\x25\x0A\x2D" 

struct _tag_SOFTVERTYPE
{
  unsigned int softver;     /*软件版本*/
  unsigned int hardVer;     /*硬件*/
  const char * desc;       
  const char * complitetime;/*编译时间*/
};

extern const struct _tag_SOFTVERTYPE softVersion;

#define ERCOMPLIERVER			5 //
#define DEVICEVERSION   		ERVERSION( 1 , 1, 0)
#define ERVERSION(first,second,third)	(((first&0xff)<<24)|((second&0xff)<<16)|((third&0xff)<<8)|(ERCOMPLIERVER&0xff))


#endif
