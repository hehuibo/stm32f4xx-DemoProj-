#ifndef _RFIDCONFIG_H
#define _RFIDCONFIG_H

/****************************芯片选择***********************************/
//#define RFID_CHIP_FM17xx        /*FM1702*/

#ifndef RFID_CHIP_FM17xx
#define RFID_CHIP_RC522         /*MFRC522*/
#endif

/****************************通讯方式***********************************/
#define _MFRC_SOFT_SPI  /***模拟SPI***/


/***********************************************************************/
#if defined(STM32F40_41xxx)     /*STM32F4xx*/
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif
#include <string.h>


#ifndef _MFRC_SOFT_SPI
#include "SPI.h"
#define MFRC_TxRxByte SPI1_TxRxByte
#endif

/* 函数错误代码定义 */
#define MI_OK		    0		/* 正确 */
#define MI_NOTAGERR		1		/* 无卡 */
#define MI_CRCERR	    2		/* 卡片CRC校验错误 */
#define MI_EMPTY		3		/* 数值溢出错误 */
#define MI_AUTHERR		4		/* 验证不成功 */
#define MI_PARITYERR	5		/* 卡片奇偶校验错误 */
#define MI_CODEERR		6		/* 通讯错误(BCC校验错) */
#define MI_SERNRERR		8		/* 卡片序列号错误(anti-collision 错误) */
#define MI_SELECTERR	9		/* 卡片数据长度字节错误(SELECT错误) */
#define MI_NOTAUTHERR	10		/* 卡片没有通过验证 */
#define MI_BITCOUNTERR	11		/* 从卡片接收到的位数错误 */
#define MI_BYTECOUNTERR	12		/* 从卡片接收到的字节数错误仅读函数有效 */
#define MI_RESTERR		13		/* 调用restore函数出错 */
#define MI_TRANSERR		14		/* 调用transfer函数出错 */
#define MI_WRITEERR		15		/* 调用write函数出错 */
#define MI_INCRERR		16		/* 调用increment函数出错 */
#define MI_DECRERR		17		/* 调用decrement函数出错 */
#define MI_READERR		18		/* 调用read函数出错 */
#define MI_LOADKEYERR	19		/* 调用LOADKEY函数出错 */
#define MI_FRAMINGERR	20		/* FM1702帧错误 */
#define MI_REQERR		21		/* 调用req函数出错 */
#define MI_SELERR		22		/* 调用sel函数出错 */
#define MI_ANTICOLLERR	23		/* 调用anticoll函数出错 */
#define MI_INTIVALERR	24		/* 调用初始化函数出错 */
#define MI_READVALERR	25		/* 调用高级读块值函数出错 */
#define MI_DESELECTERR	26
#define MI_CMD_ERR		42		/* 命令错误 */
#define MI_ERR          MI_CMD_ERR          


/****Mifare_One卡片命令字****/
enum _tag_ePICC_CMD
{
  PICC_REQIDL    = 0x26,               //寻天线区内未进入休眠状态
  PICC_REQALL    = 0x52,               //寻天线区内全部卡
  PICC_ANTICOLL1 = 0x93,               //防冲撞
  PICC_SELECT    = 0x93,
  PICC_ANTICOLL2 = 0x95,               //防冲撞
  PICC_AUTHENT1A = 0x60,               //验证A密钥
  PICC_AUTHENT1B = 0x61,               //验证B密钥
  PICC_READ      = 0x30,               //读块
  PICC_WRITE     = 0xA0,               //写块
  PICC_DECREMENT = 0xC0,               //扣款
  PICC_INCREMENT = 0xC1,               //充值
  PICC_RESTORE   = 0xC2,               //调块数据到缓冲区
  PICC_TRANSFER  = 0xB0,               //保存缓冲区中数据
  PICC_HALT      = 0x50,               //休眠

};

/*****************卡片操作接口函数********************/
typedef struct tagRfFuncCtrlTYPE{
  char (*pfRequest)(uint8_t, uint8_t*);
  char (*pfAnticoll)(uint8_t *);
  char (*pfSelect)(uint8_t *);
  char (*pfRead)(uint8_t ,uint8_t *);
  char (*pfWrite)(uint8_t ,uint8_t *);
  char (*pfAuthState)(uint8_t ,uint8_t ,uint8_t *,uint8_t *);
  char (*pfHalt)(void);
}sTRfFuncCtrlTYPE;

extern sTRfFuncCtrlTYPE g_rfFuncCtrl;

void MFRC_GPIOConfigure(void);

uint8_t ReadRawRC(uint8_t Address);

void WriteRawRC(uint8_t Address, uint8_t value);

void MFRC_Delay(u16 Delay_Time);

#endif
