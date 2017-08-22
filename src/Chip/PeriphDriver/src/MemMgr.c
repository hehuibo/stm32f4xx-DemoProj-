/*
 ============================================================================
 Name        : MemMgr.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "MemMgr.h"

#if defined (_LOADER) || defined (_NoRTOSKernel)
#include "StartUpCfg.h"
#elif defined(FreeRTOS_Kernel)
#include "FreeRTOS.h"
#include "task.h"
#endif   

#if defined (__CC_ARM) //MDK-ARM	
__align(MEM_HEAP_ALIGNMENT) static uint8_t ucMemHeap[MEM_HEAP_MAXSIZE] __attribute__((at(0x68000000)));									
#elif defined ( __ICCARM__ )//IAR    
#pragma data_alignment = MEM_HEAP_ALIGNMENT											
__no_init static uint8_t ucMemHeap[MEM_HEAP_MAXSIZE] @0x68000000;		   
#endif


#if MEM_HEAP_ALIGNMENT == 32
	#define MEM_HEAP_ALIGNMENT_MASK ( 0x0000001f )
#endif
#if MEM_HEAP_ALIGNMENT == 16
	#define MEM_HEAP_ALIGNMENT_MASK ( 0x0000000f )
#endif
#if MEM_HEAP_ALIGNMENT == 8
	#define MEM_HEAP_ALIGNMENT_MASK ( 0x00000007 )
#endif

    
typedef struct _tag_BlockLinkTYPE
{
  struct _tag_BlockLinkTYPE *pNextFreeBlock;
  size_t xBlockSize;
}BlockLinkTYPE;
    
/* Assumes 8bit bytes! */
#define MemHeapBITS_PER_BYTE		( ( size_t ) 8 )    
    
/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= ( sizeof(BlockLinkTYPE ) + ( ( size_t ) ( MEM_HEAP_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) MEM_HEAP_ALIGNMENT_MASK );

/* Block sizes must not get too small. */
#define MemHeapMINIMUM_BLOCK_SIZE	( ( size_t ) ( xHeapStructSize << 1 ) )
 
/* Create a couple of list links to mark the start and end of the list. */
static BlockLinkTYPE xStart, *pxEnd = NULL;    

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining = 0U;
static size_t xMinimumEverFreeBytesRemaining = 0U;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList(BlockLinkTYPE *pxBlockToInsert )
{
  BlockLinkTYPE *pxIterator;
  uint8_t *puc;

  /* Iterate through the list until a block is found that has a higher address
  than the block being inserted. */
  for( pxIterator = &xStart; pxIterator->pNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pNextFreeBlock )
  {
    /* Nothing to do here, just iterate to the right position. */
  }

  /* Do the block being inserted, and the block it is being inserted after
  make a contiguous block of memory? */
  puc = ( uint8_t * ) pxIterator;
  if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
  {
    pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
    pxBlockToInsert = pxIterator;
  }

  /* Do the block being inserted, and the block it is being inserted before
  make a contiguous block of memory? */
  puc = ( uint8_t * ) pxBlockToInsert;
  if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pNextFreeBlock )
  {
    if( pxIterator->pNextFreeBlock != pxEnd )
    {
      /* Form one big block from the two blocks. */
      pxBlockToInsert->xBlockSize += pxIterator->pNextFreeBlock->xBlockSize;
      pxBlockToInsert->pNextFreeBlock = pxIterator->pNextFreeBlock->pNextFreeBlock;
    }else{
      pxBlockToInsert->pNextFreeBlock = pxEnd;
    }
  }
  else
  {
    pxBlockToInsert->pNextFreeBlock = pxIterator->pNextFreeBlock;
  }

  /* If the block being inserted plugged a gab, so was merged with the block
  before and the block after, then it's pxNextFreeBlock pointer will have
  already been set, and should not be set here as that would make it point
  to itself. */
  if( pxIterator != pxBlockToInsert )
  {
    pxIterator->pNextFreeBlock = pxBlockToInsert;
  }
}

static void MemHeapInit(void)  
{
  BlockLinkTYPE *pFirstFreeBlock;
  uint8_t *pucAlignedHeap;
  size_t uxAddress;
  size_t xTotalHeapSize = MEM_HEAP_MAXSIZE;
  
  /* Ensure the heap starts on a correctly aligned boundary. */
  uxAddress = (size_t)ucMemHeap;
  
  if(0 != (uxAddress & MEM_HEAP_ALIGNMENT_MASK))
  {
    uxAddress += (MEM_HEAP_ALIGNMENT - 1);
    uxAddress &= ~((size_t)MEM_HEAP_ALIGNMENT_MASK);
    xTotalHeapSize -= uxAddress - (size_t)ucMemHeap;
  }
  
  pucAlignedHeap = (uint8_t *)uxAddress;
  
  /* xStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */  
  xStart.pNextFreeBlock  = (void *)pucAlignedHeap;
  xStart.xBlockSize = (size_t ) 0;
  
  /* pxEnd is used to mark the end of the list of free blocks and is inserted
	at the end of the heap space. */ 
  uxAddress = ( ( size_t ) pucAlignedHeap ) + xTotalHeapSize;
  uxAddress -= xHeapStructSize;
  uxAddress &= ~((size_t)MEM_HEAP_ALIGNMENT_MASK);
  pxEnd = (void *)uxAddress;
  pxEnd->xBlockSize = 0;
  pxEnd->pNextFreeBlock = NULL;
  
  /* To start with there is a single free block that is sized to take up the
	entire heap space, minus the space taken by pxEnd. */ 
  pFirstFreeBlock = (void*)pucAlignedHeap;
  pFirstFreeBlock->xBlockSize = uxAddress - (size_t)pFirstFreeBlock;
  pFirstFreeBlock->pNextFreeBlock = pxEnd;

  /* Only one block exists - and it covers the entire usable heap space. */  
  xMinimumEverFreeBytesRemaining = pFirstFreeBlock->xBlockSize;   
  xFreeBytesRemaining = pFirstFreeBlock->xBlockSize;
  
  /* Work out the position of the top bit in a size_t variable. */
  xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * MemHeapBITS_PER_BYTE ) - 1 ); 
}

void *MemMgrMalloc(size_t xWantedSize)
{
  BlockLinkTYPE *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
  void *pvReturn = NULL;  

  //Enter
  #if defined (_LOADER) || defined (_NoRTOSKernel)
  NVIC_DISABLE_INTERRUPTS();
  #elif defined(FreeRTOS_Kernel)
  portENTER_CRITICAL();
  #endif
  {
    /* If this is the first call to malloc then the heap will require
    initialisation to setup the list of free blocks. */
    if(pxEnd == NULL){
      MemHeapInit();
    }

    /* Check the requested block size is not so large that the top bit is
    set.  The top bit of the block size member of the BlockLink_t structure
    is used to determine who owns the block - the application or the
    kernel, so it must be free. */    
    if(0 == ( xWantedSize & xBlockAllocatedBit ))
    {
      /* The wanted size is increased so it can contain a BlockLink_t
      structure in addition to the requested amount of bytes. */
      if(xWantedSize > 0)
      {
        xWantedSize += xHeapStructSize;

        /* Ensure that blocks are always aligned to the required number
        of bytes. */
        if(( xWantedSize & MEM_HEAP_ALIGNMENT_MASK ) != 0x00 )
        {
          /* Byte alignment required. */
          xWantedSize += (MEM_HEAP_ALIGNMENT - (xWantedSize & MEM_HEAP_ALIGNMENT_MASK));
        } 
      }

      if((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining))
      {
        /* Traverse the list from the start	(lowest address) block until
        one	of adequate size is found. */
        pxPreviousBlock = &xStart;
        pxBlock = xStart.pNextFreeBlock;
        while((pxBlock->xBlockSize < xWantedSize ) && (pxBlock->pNextFreeBlock != NULL) )
        {
          pxPreviousBlock = pxBlock;
          pxBlock = pxBlock->pNextFreeBlock;
        }

        /* If the end marker was reached then a block of adequate size
        was	not found. */
        if( pxBlock != pxEnd )
        {
          /* Return the memory space pointed to - jumping over the
          BlockLink_t structure at its start. */
          pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pNextFreeBlock ) + xHeapStructSize );

          /* This block is being returned for use so must be taken out
          of the list of free blocks. */
          pxPreviousBlock->pNextFreeBlock = pxBlock->pNextFreeBlock;

          /* If the block is larger than required it can be split into
          two. */
          if( ( pxBlock->xBlockSize - xWantedSize ) > MemHeapMINIMUM_BLOCK_SIZE )
          {
            /* This block is to be split into two.  Create a new
            block following the number of bytes requested. The void
            cast is used to prevent byte alignment warnings from the
            compiler. */
            pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );

            /* Calculate the sizes of two blocks split from the
            single block. */
            pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
            pxBlock->xBlockSize = xWantedSize;

            /* Insert the new block into the list of free blocks. */
            prvInsertBlockIntoFreeList( pxNewBlockLink );
          }

          xFreeBytesRemaining -= pxBlock->xBlockSize;

          if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining )
          {
            xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
          }

          /* The block is being returned - it is allocated and owned
          by the application and has no "next" block. */
          pxBlock->xBlockSize |= xBlockAllocatedBit;
          pxBlock->pNextFreeBlock = NULL;
        }

      }
    }
  }
  //Exit
  #if defined (_LOADER) || defined (_NoRTOSKernel)
  NVIC_ENABLE_INTERRUPTS();
  #elif defined(FreeRTOS_Kernel)  
  portEXIT_CRITICAL(); 
  #endif

  return pvReturn;
}

/*-----------------------------------------------------------*/

void MemMgrFree(void *pv)
{
  uint8_t *puc = ( uint8_t * ) pv;
  BlockLinkTYPE *pxLink;
	
  if( pv != NULL )
  {
    /* The memory being freed will have an BlockLink_t structure immediately
    before it. */
    puc -= xHeapStructSize;

    /* This casting is to keep the compiler from issuing warnings. */
    pxLink = ( void * ) puc;

    /* Check the block is actually allocated. */
    if(( pxLink->xBlockSize & xBlockAllocatedBit ) != 0)
    {
      if(pxLink->pNextFreeBlock == NULL)
      {
        /* The block is being returned to the heap - it is no longer
        allocated. */
        pxLink->xBlockSize &= ~xBlockAllocatedBit;

        #if defined (_LOADER) || defined (_NoRTOSKernel)
        NVIC_DISABLE_INTERRUPTS();
        #elif defined(FreeRTOS_Kernel)
		portENTER_CRITICAL();
		#endif
        {
          /* Add this block to the list of free blocks. */
          xFreeBytesRemaining += pxLink->xBlockSize;
          prvInsertBlockIntoFreeList( ((BlockLinkTYPE *) pxLink ));
        }
        #if defined (_LOADER) || defined (_NoRTOSKernel)
        NVIC_ENABLE_INTERRUPTS();
        #elif defined(FreeRTOS_Kernel)  
        portEXIT_CRITICAL(); 
		#endif
      }//end if(pxLink->pxNextFreeBlock == NULL)
    }
  }
}

