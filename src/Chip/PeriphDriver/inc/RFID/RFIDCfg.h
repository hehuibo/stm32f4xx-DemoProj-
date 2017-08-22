#ifndef _RFIDCONFIG_H
#define _RFIDCONFIG_H

/****************************оƬѡ��***********************************/
//#define RFID_CHIP_FM17xx        /*FM1702*/

#ifndef RFID_CHIP_FM17xx
#define RFID_CHIP_RC522         /*MFRC522*/
#endif

/****************************ͨѶ��ʽ***********************************/
#define _MFRC_SOFT_SPI  /***ģ��SPI***/


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

/* ����������붨�� */
#define MI_OK		    0		/* ��ȷ */
#define MI_NOTAGERR		1		/* �޿� */
#define MI_CRCERR	    2		/* ��ƬCRCУ����� */
#define MI_EMPTY		3		/* ��ֵ������� */
#define MI_AUTHERR		4		/* ��֤���ɹ� */
#define MI_PARITYERR	5		/* ��Ƭ��żУ����� */
#define MI_CODEERR		6		/* ͨѶ����(BCCУ���) */
#define MI_SERNRERR		8		/* ��Ƭ���кŴ���(anti-collision ����) */
#define MI_SELECTERR	9		/* ��Ƭ���ݳ����ֽڴ���(SELECT����) */
#define MI_NOTAUTHERR	10		/* ��Ƭû��ͨ����֤ */
#define MI_BITCOUNTERR	11		/* �ӿ�Ƭ���յ���λ������ */
#define MI_BYTECOUNTERR	12		/* �ӿ�Ƭ���յ����ֽ����������������Ч */
#define MI_RESTERR		13		/* ����restore�������� */
#define MI_TRANSERR		14		/* ����transfer�������� */
#define MI_WRITEERR		15		/* ����write�������� */
#define MI_INCRERR		16		/* ����increment�������� */
#define MI_DECRERR		17		/* ����decrement�������� */
#define MI_READERR		18		/* ����read�������� */
#define MI_LOADKEYERR	19		/* ����LOADKEY�������� */
#define MI_FRAMINGERR	20		/* FM1702֡���� */
#define MI_REQERR		21		/* ����req�������� */
#define MI_SELERR		22		/* ����sel�������� */
#define MI_ANTICOLLERR	23		/* ����anticoll�������� */
#define MI_INTIVALERR	24		/* ���ó�ʼ���������� */
#define MI_READVALERR	25		/* ���ø߼�����ֵ�������� */
#define MI_DESELECTERR	26
#define MI_CMD_ERR		42		/* ������� */
#define MI_ERR          MI_CMD_ERR          


/****Mifare_One��Ƭ������****/
enum _tag_ePICC_CMD
{
  PICC_REQIDL    = 0x26,               //Ѱ��������δ��������״̬
  PICC_REQALL    = 0x52,               //Ѱ��������ȫ����
  PICC_ANTICOLL1 = 0x93,               //����ײ
  PICC_SELECT    = 0x93,
  PICC_ANTICOLL2 = 0x95,               //����ײ
  PICC_AUTHENT1A = 0x60,               //��֤A��Կ
  PICC_AUTHENT1B = 0x61,               //��֤B��Կ
  PICC_READ      = 0x30,               //����
  PICC_WRITE     = 0xA0,               //д��
  PICC_DECREMENT = 0xC0,               //�ۿ�
  PICC_INCREMENT = 0xC1,               //��ֵ
  PICC_RESTORE   = 0xC2,               //�������ݵ�������
  PICC_TRANSFER  = 0xB0,               //���滺����������
  PICC_HALT      = 0x50,               //����

};

/*****************��Ƭ�����ӿں���********************/
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
