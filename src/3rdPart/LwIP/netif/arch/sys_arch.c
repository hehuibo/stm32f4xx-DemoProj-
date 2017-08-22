/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "arch/sys_arch.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX  4

static sys_thread_t gs_LwipTaskId[SYS_THREAD_MAX];   

/*MUTEX */
#if LWIP_COMPAT_MUTEX == 0
/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  *mutex = xSemaphoreCreateMutex();
  if(*mutex == NULL)
  {
#if SYS_STATS
    ++lwip_stats.sys.mutex.err;
#endif /* SYS_STATS */	
    return ERR_MEM;
  }

#if SYS_STATS
  ++lwip_stats.sys.mutex.used;
  if (lwip_stats.sys.mutex.max < lwip_stats.sys.mutex.used) 
  {
    lwip_stats.sys.mutex.max = lwip_stats.sys.mutex.used;
  }
#endif /* SYS_STATS */
  return ERR_OK;  
}
/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
  xSemaphoreTake(*mutex,portMAX_DELAY);
}
/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  xSemaphoreGive(*mutex);
}
/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
#if SYS_STATS
   --lwip_stats.sys.mutex.used;
#endif /* SYS_STATS */
			
  vQueueDelete(*mutex);  
}

/** Check if a mutex is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mutex_valid(sys_mutex_t *mutex)
{
  return (*mutex != NULL );
}

/** Set a mutex invalid so that sys_mutex_valid returns 0 */
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
  *mutex = NULL;
}
#endif


/* Semaphore */

/** Create a new semaphore
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  vSemaphoreCreateBinary(*sem );
  //*sem = xSemaphoreCreateBinary();//TCP客户端连接不上，ping IP 不通
  if(*sem == NULL)
  {
#if SYS_STATS
    ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */	
    return ERR_MEM;
  }
	
   if(count == 0)	// Means it can't be taken
  {
    xSemaphoreTake(*sem,1);
  }

#if SYS_STATS
  ++lwip_stats.sys.sem.used;
  if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
  {
    lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
  }
#endif /* SYS_STATS */
		
  return ERR_OK;  
}
/** Signals a semaphore
 * @param sem the semaphore to signal */
void sys_sem_signal(sys_sem_t *sem)
{
  xSemaphoreGive(*sem);
}
/** Wait for a semaphore for the specified timeout
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return time (in milliseconds) waited for the semaphore
 *         or SYS_ARCH_TIMEOUT on timeout */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  portTickType StartTime, EndTime, Elapsed;

  StartTime = xTaskGetTickCount();

  if(	timeout != 0)
  {
    if( xSemaphoreTake( *sem, timeout / portTICK_RATE_MS ) == pdTRUE )
    {
      EndTime = xTaskGetTickCount();
      Elapsed = (EndTime - StartTime) * portTICK_RATE_MS;
			
      return (Elapsed); // return time blocked TODO test	
    }
    else
    {
      return SYS_ARCH_TIMEOUT;
    }
  }
  else // must block without a timeout
  {
    while( xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE){}
    EndTime = xTaskGetTickCount();
    Elapsed = (EndTime - StartTime) * portTICK_RATE_MS;

    return ( Elapsed ); // return time blocked			
  }  
}
/** Delete a semaphore
 * @param sem semaphore to delete */
void sys_sem_free(sys_sem_t *sem)
{
#if SYS_STATS
      --lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
			
  vQueueDelete(*sem);
}

/** Check if a sempahore is valid/allocated: return 1 for valid, 0 for invalid */
int sys_sem_valid(sys_sem_t *sem)
{
  return (*sem != NULL);
}

/** Set a semaphore invalid so that sys_sem_valid returns 0 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
  *sem = NULL;
}


/* Mailbox functions. */

/** Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (miminum) number of messages in this mbox
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  
  if(size <= 0)
    size = archMESG_QUEUE_LENGTH;
  
  *mbox = xQueueCreate(size, sizeof(void *));
#if SYS_STATS
  ++lwip_stats.sys.mbox.used;
  if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) 
  {
    lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
  }
#endif /* SYS_STATS */
  
 if (*mbox == NULL)
    return ERR_MEM;
 
 return ERR_OK;
}

/** Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  while ( xQueueSendToBack(*mbox, &msg, portMAX_DELAY ) != pdTRUE )
  {
  
  }
}

/** Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  err_t result;

  if ( xQueueSend( *mbox, &msg, 0 ) == pdPASS )
  {
    result = ERR_OK;
  }
  else 
  {
    // could not post, queue must be full
    result = ERR_MEM;
			
#if SYS_STATS
      lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
			
  }

  return result;  
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter! */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  void *dummyptr;
  portTickType StartTime, EndTime, Elapsed;

  StartTime = xTaskGetTickCount();

  if ( msg == NULL )
  {
     msg = &dummyptr;
  }
		
  if ( timeout != 0 )
  {
    if ( pdTRUE == xQueueReceive( *mbox, &(*msg), timeout / portTICK_RATE_MS) )
    {
      EndTime = xTaskGetTickCount();
      Elapsed = (EndTime - StartTime) * portTICK_RATE_MS;
			
      return ( Elapsed );
    }
    else // timed out blocking for message
    {
      *msg = NULL;
			
      return SYS_ARCH_TIMEOUT;
    }
  }
  else // block forever for a message.
  {
   // xQueueReceive( *mbox, &(*msg), portMAX_DELAY ); 
    while( pdTRUE != xQueueReceive( *mbox, &(*msg), portMAX_DELAY ) )
    {
    // time is arbitrary
    } 
    EndTime = xTaskGetTickCount();
    Elapsed = (EndTime - StartTime) * portTICK_RATE_MS;
		
    return ( Elapsed ); // return time blocked TODO test	
  }
}

/* Allow port to override with a macro, e.g. special timout for sys_arch_mbox_fetch() */

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  void *dummyptr;

  if ( msg == NULL )
  {
    msg = &dummyptr;
  }

   if ( pdTRUE == xQueueReceive( *mbox, &(*msg), 0 ) )
   {
      return ERR_OK;
   }
   else
   {
      return SYS_MBOX_EMPTY;
   }
}

/** Delete an mbox
 * @param mbox mbox to delete */
void sys_mbox_free(sys_mbox_t *mbox)
{
  if( uxQueueMessagesWaiting( *mbox ) )
  {
    /* Line for breakpoint.  Should never break here! */
    portNOP();
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
			
    // TODO notify the user of failure.
  }

  vQueueDelete( *mbox );

#if SYS_STATS
     --lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
}

/** Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mbox_valid(sys_mbox_t *mbox)
{
  return (*mbox != NULL);
}

/** Set an mbox invalid so that sys_mbox_valid returns 0 */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  *mbox = NULL;
}


#if SYS_LIGHTWEIGHT_PROT == 1
/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
	vPortEnterCritical();
	return 1;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
	( void ) pval;
	vPortExitCritical();
}

#endif

// TBD

/*-----------------------------------------------------------------------------------*/
/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
static int curr_prio_offset = 0;
sys_thread_t sys_thread_new(const char *name, void (*thread) (void *arg), void *arg, int stacksize, int prio)
{
  sys_thread_t pRet = NULL;

  if( curr_prio_offset < SYS_THREAD_MAX ){
    if(xTaskCreate(thread,
                   (char const*)name,
                   stacksize,arg,
                   prio,
                   &gs_LwipTaskId[curr_prio_offset]) != pdTRUE ){
      return NULL;
    }
    pRet = &gs_LwipTaskId[curr_prio_offset];
    curr_prio_offset++;
  }
	
  return pRet;
}

void sys_init(void)
{
  curr_prio_offset = 0;
}