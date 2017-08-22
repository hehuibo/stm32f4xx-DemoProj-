/*
 ============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */

#include "main.h"

int main(void){
  uint8_t i = 0;
  AppTaskInit();
  while(1){
    for(i=0; i< MAX_FUNCITEM; i++){
      pFunCalled[i]();
    }
    i = 0;			
  }
}

