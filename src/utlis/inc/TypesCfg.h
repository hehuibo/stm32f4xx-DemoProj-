#ifndef _STD_TYPESCFG_H
#define _STD_TYPESCFG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "FSM.h"

typedef struct _tag_TimeFormat{
  uint8_t	mYear;
  uint8_t	mMonth;				// 1-12
  uint8_t	mDay;			    // 1-28, 1-29, 1-30, 1-31
  
  uint8_t	mHour;				// 0-23
  uint8_t	mMinute;			// 0-59 
  uint8_t	mSecond;   			// 0-59
  
  uint8_t	mWeek;			// 0-6
}TimeFormatTYPE;

#if defined (UART_TRACE) || defined (JLINK_RTT_TRACE)
#include "trace.h"
#endif

#define ARRAYLEN(array)				(sizeof(array) / sizeof(array[0]))




/*¥ÌŒÛ¿‡–Õ√∂æŸ*/
#if 0
typedef enum _tag_eERRTYPE
{
  eERR_EINPARAM = -2,	/*Illegal Param*/
  eERR_ERANGE = -1,		/*out of range*/
  eERR_OK = 0,			/*No Err*/
  eERR_CRC ,			/*Crc Err*/
  eERR_NOMEM,   		/* Not enough memory */
  eERR_NOBUF,			/* No buffer space available */
  eERR_TIMEOUT,			/*Timeout*/
  eERR_EINVAL,  		/*Illegal value*/
  
  eERR_ERROR,   		/*  */
}eErrTYPE;
#else

#define     eERR_EINPARAM   -2    	/*Illegal Param*/
#define     eERR_ERANGE     -1		/*out of range*/
#define     eERR_OK          0	    /*No Err*/
#define     eERR_CRC         1	    /*Crc Err*/
#define     eERR_NOMEM       2      /* Not enough memory */
#define     eERR_NOBUF       3	    /* No buffer space available */
#define     eERR_TIMEOUT     4	    /*Timeout*/
#define     eERR_EINVAL      5     /*Illegal value*/
#define     eERR_ERROR       127   /*  */

typedef char eErrTYPE;
#endif

#endif
