/*
 ============================================================================
 Name        : 
 Author      : 
 Version     :
 Copyright   : 
 Description : 
 ============================================================================
 */
#include "AppCfg.h"
   
#define DEVNAME "≤‚ ‘∞Ê"

const struct _tag_SOFTVERTYPE softVersion = {
  DEVICEVERSION,
  ERVERSION(1, 0, 0),
  DEVNAME,
  __DATE__" "__TIME__ ,
};

unsigned int getSoftVer(void)
{
	return softVersion.softver;
}