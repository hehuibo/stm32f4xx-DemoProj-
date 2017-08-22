/*
 ============================================================================
 Name        : utils.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdarg.h>
#include "utils.h"

#if defined (UART_TRACE)
#include "trace.h"
#endif

uint8_t MonthAry[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

/*获取当前时间的时间戳*/
uint32_t utils_GetSecondTime(TimeFormatTYPE *time)
{
  uint32_t secCnt = 0;
  uint16_t yearCnt = 0;
  uint16_t year = time->mYear + 2000;

  for(int i=1970; i< year; i++){
    if(((i%4 == 0)&&(i%100 != 0)) ||
      (i%400 == 0)){
      yearCnt ++;
    }
  }
  secCnt = yearCnt*366 + (year - 1970 - yearCnt)*365;
  
  for(int i=1; i < time->mMonth; i++){
    if((i== 2)&&(((i%4 == 0)&&(i%100 != 0)) ||(i%400 == 0))){
      secCnt += 29;
    }else{
      secCnt += MonthAry[i-1];
    }
  }
  
  secCnt += (time->mDay -1);
  
  secCnt = secCnt*86400 + time->mHour * 3600 + time->mMinute*60 + time->mSecond;
  return secCnt;
}


void utils_GetTimeFromSecond(uint32_t sec, TimeFormatTYPE *time)
{
  uint32_t day, iday;
  
  day = sec / 86400;
  sec = sec % 86400;

  uint16_t i = 1970;
  while(day > 365){
    if(((i%4 == 0)&&(i%100 != 0)) ||
      (i%400 == 0)){
      day -= 366;
      }else{
        day -= 365;
      }
    i++;
  }
  
  if((day == 365) && !(((i%4 == 0)&&(i%100 != 0)) ||(i%400 == 0))){
    day -= 365;
  }
  time->mYear = i - 2000;
  
  uint16_t j = 0;
  for(j=0; j<12; j++){
    if((j== 1)&&(((i%4 == 0)&&(i%100 != 0)) ||(i%400 == 0))){
      iday = 29;
    }else{
      iday = MonthAry[j];
    }
    
    if(day >= iday){
      day -= iday;
    }else{
      break;
    }
  }
  
  time->mMonth = j+ 1;
  time->mDay = day + 1;
  time->mHour = ((sec/3600))%24;//((sec/3600) + 8)%24;
  time->mMinute = sec % 3600 / 60;
  time->mSecond = sec % 3600 % 60;
}

/*计算星期*/
unsigned char utils_CalWeekDay(unsigned char myear, unsigned char mn, unsigned char day){
  int days = 0, sum;
  unsigned char k;
  unsigned short year = myear + 2000;
  unsigned char mont[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if (year % 4 == 0 || year % 100 == 0 || year % 400 == 0)
    mont[2] = 29;
  else
    mont[2] = 28;

  for (int i = 0; i < mn; i++)
    days += mont[i];

  days += day;
  sum = year - 1 + (int) ((year - 1) / 4) - (int) ((year - 1) / 100) + (int) ((year - 1) / 400) + days;
  k = sum % 7;
  return k;
}


int utils_AddParam(void *pParam, const void *Data , int len)
{
  memcpy(pParam, Data, len);
  return len;
}

int utils_ReadParam(const void *pParam, void *Data, int len)
{
  memcpy(Data, pParam,len );
  return len;
}

/***********printf-stdarg******************/
static void printchar(char **str, int c)
{
	if (str) {
		**str = (char)c;
		++(*str);
	}
	else
	{
	 /* Output char here. */
#if 0 
      while(USART_GetFlagStatus(dbgTRACEUART, USART_FLAG_TC) == RESET){
      }
      USART_SendData(dbgTRACEUART,(char) c);
#endif
	}
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = (unsigned int)i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = (unsigned int)-i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = (unsigned int)u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = (char)(t + '0');
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' || *format == 'i' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int utils_printf(const char *format, ...)
{
	va_list args;

	va_start( args, format );
	return print( 0, format, args );
}

int utils_sprintf(char *out, const char *format, ...)
{
	va_list args;

	va_start( args, format );
	return print( &out, format, args );
}

int utils_snprintf( char *buf, unsigned int count, const char *format, ... )
{
	va_list args;

	( void ) count;

	va_start( args, format );
	return print( &buf, format, args );
}
