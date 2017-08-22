#ifndef _USER_DEF_UTILS_H
#define _USER_DEF_UTILS_H

#include "TypesCfg.h"

#define TOBCD(h)	((unsigned char)((h)/10)*16+((h)%10))
#define TOHEX(bcd)	((unsigned char)(((bcd&0xf0)>>4)*10)+(bcd&0x0f))

#ifndef htonl
#define htonl(a)                    \
        ((((a) >> 24) & 0x000000ff) |   \
         (((a) >>  8) & 0x0000ff00) |   \
         (((a) <<  8) & 0x00ff0000) |   \
         (((a) << 24) & 0xff000000))
#endif

#ifndef ntohl
#define ntohl(a)    htonl((a))
#endif

#ifndef htons
#define htons(a)                \
        ((((a) >> 8) & 0x00ff) |    \
         (((a) << 8) & 0xff00))
#endif

#ifndef ntohs
	#define ntohs(a)    htons((a))
#endif

unsigned int utils_GetSecondTime(TimeFormatTYPE *time);

void utils_GetTimeFromSecond(unsigned int sec, TimeFormatTYPE *time);

unsigned char utils_CalWeekDay(unsigned char myear, unsigned char mn, unsigned char day);

/*AddParam*/

int utils_AddParam(void *pParam, const void *Data , int len);

inline int utils_AddByte( unsigned char *buf , unsigned char byte ){
  return utils_AddParam( buf , &byte , 1 );
}

inline int utils_AddWord( unsigned char *buf , unsigned short word){
  return utils_AddParam( buf ,&word,2);
}
inline int utils_AddDWord( unsigned char *buf , unsigned int dword){
  return utils_AddParam( buf , &dword , 4 );
}

/*ReadParam*/
int utils_ReadParam( const void *pParam , void *Data , int len ) ;

inline int utils_ReadWord( const unsigned char *buf , unsigned short *word){
  return utils_ReadParam( buf , word , 2 );
}

inline int utils_ReadDword24(const unsigned char *buf , unsigned int *dword){
  *dword = 0 ;
  return utils_ReadParam(buf , dword , 3);
}

inline int utils_ReadDWord( const unsigned char *buf , unsigned int *dword ){
  return utils_ReadParam( buf , dword , 4 );
}

/*******printf-stdarg*********/
int utils_printf(const char *format, ...);
int utils_sprintf(char *out, const char *format, ...);
int utils_snprintf( char *buf, unsigned int count, const char *format, ... );

#endif 
