/*
 * Version.h
 *
 *  Created on: 20xx��x��x��
 *      Author: HeHuiBo
 */

#ifndef VERSION_H_
#define VERSION_H_

#include "TypesCfg.h"

struct TagSoftVerTYPE{
  unsigned int mSoftVer;        /*����汾*/
  unsigned int mHardwareVer;    /*Ӳ���汾*/
  char *pDevName;
};

#define ERCOMPLIERVER			0//
#define DEVICEVERSION   		ERVERSION( 2 , 0 , 0 )
#define ERVERSION(first,second,third)	(((first&0xff)<<24)|((second&0xff)<<16)|((third&0xff)<<8)|(ERCOMPLIERVER&0xff))

unsigned int getSoftVerion(void);


#endif /* VERSION_H_ */
