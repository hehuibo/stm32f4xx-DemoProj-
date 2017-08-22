#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define MAX_QUEUE_ENTRIES 20	//number of mboxs

typedef xSemaphoreHandle sys_sem_t;
typedef xQueueHandle sys_mutex_t;
typedef xQueueHandle sys_mbox_t;
typedef xTaskHandle sys_thread_t;
//typedef unsigned long sys_prot_t;

#define archMESG_QUEUE_LENGTH 6

#endif
