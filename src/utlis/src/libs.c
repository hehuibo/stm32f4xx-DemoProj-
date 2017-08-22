/*
 ============================================================================
 Name        : utilslib.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "libs.h"

void StringToIp(const char *cp,unsigned int *ip){
	unsigned long val;
	unsigned char base;
	unsigned long parts[4];
	unsigned long *pp = parts;
	char c = *cp;
	
	for(;;){
		val = 0;
		base = 10;
		if(c == '0'){
			c = *++cp;
			base = 8;
		}
		
		for(;;){
			if((c>='0')&&(c<='9')){
				val= (val * base) + (int)(c-'0');
				c = *++cp;
			}
			else
				break;
		}
		
		if(c == '.'){
			*pp++ = val;
			c = *++cp;
		}
		else
			break;

	}
	val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
     *ip =  val;
}

int constant_fls(int x)
{
  int r = 32;
  
  if(!x)
    return 0;
  
  if(!(x & 0xffff0000u)){
    x <<= 16;
    r -= 16;
  }
  
  if (!(x & 0xff000000u)) {
    x <<= 8;
    r -= 8;
  }
  
  if (!(x & 0xf0000000u)) {
    x <<= 4;
    r -= 4;
  }
  
  if (!(x & 0xc0000000u)) {
    x <<= 2;
    r -= 2;
  }
  
  if (!(x & 0x80000000u)){
    x <<= 1;
    r -= 1;
  }  
  
  return r;
}

int Get2Bits(int x)
{
  if((x&(x-1)) == 0){
    return x;
  }
  
  return 1 << constant_fls(x);
}