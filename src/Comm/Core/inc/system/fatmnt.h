/*
 * fatmnt.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef FATMNT_H_
#define FATMNT_H_

#include "ff.h"

struct _tag_FatMntTYPE
{
  FATFS mFs;
  FIL   mFile;
  FRESULT mRes;
  UINT br;
};

extern struct _tag_FatMntTYPE g_FatFsMnt;

void FatMntInit(void);

#endif /* FATMNT_H_ */
