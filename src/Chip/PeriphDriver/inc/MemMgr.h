/*
 * MemMgr.h
 *
 *  Created on: 20xxƒÍx‘¬x»’
 *      Author: HeHuiBo
 */

#ifndef MEMMGR_H_
#define MEMMGR_H_

#include "SRAM.h"
#include <stdlib.h>
#include <stdint.h>

#define MEM_HEAP_ALIGNMENT  (32) 
#define MEM_HEAP_MAXSIZE    (( size_t) ( 950 * 1024))

void *MemMgrMalloc(size_t xWantedSize);
void MemMgrFree(void *pv);

#endif /* MEMMGR_H_ */
