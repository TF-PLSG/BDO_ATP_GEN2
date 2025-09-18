/*
				M Y M A L L O C  -  A memory allocator
				Copyright 2010

		Written by : NIRBHAY JOSHI

		for XCOM, Thoughtfocus Information technologies

// These are our version of malloc and free function calls.
// These functions will allocate memory from the address pointed to by
// sharedMemAllocStartAddr.

*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#include <time.h>
#include "mymalloc.h"
	// This is required to ensure thread safety.
#define _REENTRANT

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  

#define MYMALLOC_MIN_CHUNK_DATASIZE 64

#define USE_LOGGING 0
#define MYMALLOC_LOG_FILE_NAME "/export/home/nirbhay/mymalloclog/log"

#if PADDING_TEST
char allocPaddingSignature[ALLOC_AREA_PADDING_LEN];
#endif

#if USE_LOGGING
FILE *logfile = NULL;
#endif

// Starting address from where to allocate memory.
// Application utilizing this memory allocation algorithm, must set this pointer
// value before calling myinit(). mymalloc() function call will use memory
// space starting from this address and allocate as requested.
void *sharedMemAllocStartAddr = NULL, *endOfSharedmemory = NULL,
            *sharedMemBaseAddr = NULL;    // Addresses will be stored relative to this base address.
// sharedMemBaseAddr is required because shmat() doesn't get attached to the same address in
// all the processes attaching to the same shared memory. So fifth character into the shared
// memory may have address 0xfbc00005 ( 0xfbc00000 + 5 ) whereas is another process, it
// may be 0xf9900005 ( 0xf9900000 + 5 ). So when data written in the shared memory is an
// address (pointing to a location in the same shared memory, as will be the case when creating
// a link list), it cannot be shared between two different processes. This is obvious as address
// location 0xf9900005 doesn't exist in process 1, where shared memory is starting at
// address 0xfbc00000 !!
//
// Solution:
// But, if the offset (i.e. 5) is stored in place of the absolute address, it can always be converted
// back to 0xfbc00005 in process 1 by adding the base address (0xfbc00000) with 5. Similarly,
// in process 2, in the above example, offset 5 will get converted to 0xf9900005 that also points
// to the fifth character into the shared memory !!
// In the below functions (mymalloc and myfree), handling the links (addresses) of the link list
// is done in the similar way. When an address is written to the shared memory, it is converted
// to an offset by subtracting sharedMemBaseAddr. And when it is read from shared memory, it
// is converted back to an address by adding sharedMemBaseAddr. More information is provided
// alongside the code, in the comments.


//long nodesAllocated = 0;
//char perrorTxt[256];

/*
					USE_DL_PREFIX
	Define the above macro in makefile or compile command, if the
	prefix (of n and not dl) is desired. This is useful if you also want to
	use the default malloc/free functions along with nmalloc/nfree.
*/
#ifndef USE_DL_PREFIX
	//	We do not want these implementations to conflict with regular malloc/free.
	//#define mymalloc malloc
	//#define myfree free
#endif

/******************************************************************************
*  function ptetime_get_timestamp
*
*  This function generates a timestamp for representing the clock time at
*  execution.  The format of the timestamp is ISO standard (???) and has this
*  format:
*     YYYY-MM-DD-HH.II.SS.JJJ
*     where:
*        YYYY   = The year in 4 digits
*        MM     = The month with zero padding, e.g. Feb is 02
*        DD     = The day of the month with zero padding as well
*        HH     = The hour of the day on a 24 hour clock
*        II     = The minute of the hour with zero padding
*        SS     = The seconds after the minute
*        JJJ    = The milliseconds after the second
*
*  Parameters:
*     dest_str.....string to place the timestamp into (should be at least 24 chars).
*
******************************************************************************/
void ptetime_get_timestamp1 ( char *dest_str )
{
	struct   timeb   time_structure;   /* Time for ftime */
	struct   tm       *current_time;    /* Current time structure */

	/* Get the current time from the operating system and convert to local time.*/
	ftime (&time_structure);
	current_time = localtime (&time_structure.time);
	current_time->tm_mon++;    /* month is 0..11; change to 1..12 */

	/*
	Year 2000 work around: The present call does not indicate the century value.
	Therefore, we bias the year for the current century - i.e. adding 1900 or 2000.
	This will change in the future.
	*/
	if (current_time->tm_year >= 70)
		current_time->tm_year += 1900;
	else
		current_time->tm_year += 2000;

	/* Generate the timestamp string */
	if ( dest_str != NULL )
		sprintf (dest_str, "%04d-%02d-%02d-%02d.%02d.%02d.%03d",
	current_time->tm_year,
	current_time->tm_mon,
	current_time->tm_mday,
	current_time->tm_hour,
	current_time->tm_min,
	current_time->tm_sec,
	time_structure.millitm);
}  /* end of ptetime_get_timestamp */


/*
	Initialize the available memory into one large chunk of free memory.
	Call myinit() immediately after setting the address value in
	sharedMemAllocStartAddr. Pass the size of the total allocated shared
	memory.
*/
int myinit(size_t totalMemory)
{
#ifdef TEST_WITH_SYSTEM_MALLOC
	return (0);	// Nothing to initialize
#else
	pthread_mutex_t	*Mem_Lock = (pthread_mutex_t*)sharedMemAllocStartAddr;
	struct CHUNK_HEADER *ptr	= (struct CHUNK_HEADER *)(((char*)sharedMemAllocStartAddr) + sizeof(pthread_mutex_t));

	if ( sharedMemAllocStartAddr == NULL || totalMemory < sizeof(pthread_mutex_t) )	// Idiot check allocation pointer and arguments
		return -1;

        // points to the immediate next address from the last byte of shared memory
	endOfSharedmemory = sharedMemAllocStartAddr + totalMemory;

	// Addresses will be stored relative to this base address.
	sharedMemBaseAddr = sharedMemAllocStartAddr;

	PRINT ("sharedMemAllocStartAddr is 0x%x.\n", sharedMemAllocStartAddr);

	//nodesAllocated = 0;

	//memset(Mem_Lock, 0, sizeof(pthread_mutex_t));	// Initialize the mutex lock
	//pthread_mutex_init(Mem_Lock, USYNC_PROCESS|LOCK_ROBUST, NULL);
	pthread_mutex_init(Mem_Lock, NULL);

	// Initialize the remaining memory as a single big free chunk.
	ptr->lock				= 1;	// lock it.
	ptr->chunkSize		= totalMemory - sizeof(pthread_mutex_t) -
							sizeof(struct CHUNK_HEADER);	// Total available memory - mutex - header size
	ptr->chunkStatus		= 0;	// free
	ptr->nextChunk		= NULL;	// Non circular doubly link list.
	ptr->prevChunk		= NULL;
	ptr->lock				= 0;	// unlock it.
	return 0;
#endif
}

/*
	Initialize the available memory into one large chunk of free memory.
*/
void* mymalloc(size_t size)
{
#ifdef TEST_WITH_SYSTEM_MALLOC
	return(malloc (size));
#else
	pthread_mutex_t	*Mem_Lock = (pthread_mutex_t*)sharedMemAllocStartAddr;
	struct CHUNK_HEADER *ptr	= (struct CHUNK_HEADER *)(((char*)sharedMemAllocStartAddr) + sizeof(pthread_mutex_t));
	struct CHUNK_HEADER *nChunk;
	void *return_value;
	int pthread_mutex_ret = 0;
#if PADDING_TEST
	int paddingCtr;
	int sizeBackup = size;
#endif

	if ( sharedMemAllocStartAddr == NULL || size <= 0 )	// Idiot check allocation pointer and parameters
		return NULL;

	if ( (size % sizeof(int)) != 0 )
	{
		// Add extra bytes to make 'size' fall on 4 or 8 byte boundary.
		// You may get Bus error (SIGBUS), if this is not done
		size = ((size/sizeof(int)) + 1 ) * sizeof(int);
	}

#if PADDING_TEST
	// Adding a padding area to safeguard against any overflows.
	size += ALLOC_AREA_PADDING_LEN;
	if ( sizeBackup == 60 && size != 188 )
		syslog (LOG_ERR, "MyMalloc ERROR: size wrong at start of mymalloc.");
#endif

#if USE_LOGGING
	if ( logfile == NULL )
	{
		char filename[1024], timestmp[50];
		strcpy (filename, MYMALLOC_LOG_FILE_NAME);
		ptetime_get_timestamp1 (timestmp);
		strcat (filename, timestmp);
		logfile = fopen(MYMALLOC_LOG_FILE_NAME, "a+");
	}
	if ( logfile != NULL )
	{
		fprintf(logfile, "%d\n", size);
	}
#endif

	if ( (pthread_mutex_ret = pthread_mutex_lock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Cannot acquire lock. Should return with error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_lock returns with %d in mymalloc().", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
		//if ( pthread_mutex_ret == EOWNERDEAD )
		if ( pthread_mutex_ret != 0 )
		{
			//pthread_mutex_consistent();	// When EOWNERDEAD is returned by pthread_mutex_lock(), the mutex is acquired by this
								// process and should be unlocked as normal (as if 0 was returned).
								// But the previous process that had acquired the lock, didn't unlock the
								// mutex, but crashed/died. So the state of mymalloc link list might
								// be inconsistent and it might be
								// in the middle of rearranging some pointers and it is the responsibility
								// of this process, to make the state of mymalloc consistent !!
								// There is no way to do that (depends on where the last process crashed), so
								// we are just pretending everything is fine and continuing.
								// TODO: design an UNDO process to ensure a crash doesn't make
								// TODO: mymalloc inconsistent !!
		}
		else
			return NULL;
	}

		// Critical section

	// First fit Search for a chunk greater than or equal to the desired size.
	while ( ptr != NULL && ( !( ptr->chunkStatus == 0 && ptr->chunkSize >= size) ))
	{
		// make an absolute address from address offset that gets stored in memory.
		if ( ptr->nextChunk != NULL )
			ptr = (struct CHUNK_HEADER *)(((char*)sharedMemBaseAddr) + (unsigned long)(ptr->nextChunk));
		else
			ptr = NULL;
	}

#if PADDING_TEST
	if ( sizeBackup == 60 && size != 188 )
		syslog (LOG_ERR, "MyMalloc ERROR: size wrong at middle of mymalloc.");
#endif

	if ( ptr != NULL )
	{
		if ( (ptr->chunkSize) > (size + MYMALLOC_MIN_CHUNK_DATASIZE) )	// chunk is bigger than (required + smallest chunk possible)...
		{
			// Divide this chunk into two parts, allocate first and initialize second
			// chunk as free chunk.
			ptr->lock = 1;	// lock the first part that is to be allocated

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at IF-start of mymalloc.");
#endif

			if(ptr->nextChunk != NULL)
			{
				// Save the next node address (convert from offset to absolute address)
				nChunk = (struct CHUNK_HEADER *)(((char*)sharedMemBaseAddr) + (unsigned long)(ptr->nextChunk));
			}
			else
			{
				nChunk = NULL;
			}
			// Define the new free chunk boundry
			(ptr->nextChunk) = (struct CHUNK_HEADER*)(((char*)ptr) + sizeof(struct CHUNK_HEADER) + size);

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at IF-middle of mymalloc.");
#endif

			// Setup the newly created free chunk (second part of original big chunk)
			(ptr->nextChunk)->lock = 1;	// lock it
			(ptr->nextChunk)->nextChunk = nChunk;
			(ptr->nextChunk)->prevChunk = ptr;
			(ptr->nextChunk)->chunkSize = (ptr->chunkSize) - sizeof(struct CHUNK_HEADER) - size;
			(ptr->nextChunk)->chunkStatus = 0;	// free
			(ptr->nextChunk)->lock = 0;	// unlock it

			// previous pointer in next to next chunk should now point
			// to the newly created free chunk.
			if ( nChunk != NULL )
			{
				nChunk->lock = 1;
				nChunk->prevChunk = (ptr->nextChunk);
				nChunk->lock = 0;
			}

			// Setup the rest of values in the allocated first part
			ptr->chunkSize = size;	// new size of this chunk
			ptr->chunkStatus = 1;	// It is now allocated
			//ptr->prevChunk	= ;	previous chunk remains as it is, so no changes

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at IF-middle 1 of mymalloc.");
#endif

			// -- IMPORTANT --
			// Convert absolute addresses to offsets, relative to base address,
			// if the addresses are not NULL. This is required as base address
			// of the shared memory after shmat() call can be different in
			// different modules even though they are pointing to the same
			// shared memory segment.
			//
			// Four possible pointers are set above : ptr->nextChunk,
			// ptr->nextChunk->nextChunk, ptr->nextChunk->prevChunk and
			// ptr->nextChunk->nextChunk->prevChunk
			// Convert them starting from the last one - order is important.
			//
			
			if ( (ptr->nextChunk->nextChunk) != NULL )
			{
				if ( (ptr->nextChunk->nextChunk->prevChunk) != NULL )
				{
					ptr->nextChunk->nextChunk->prevChunk = (struct CHUNK_HEADER*)
						( (char*)(ptr->nextChunk->nextChunk->prevChunk) - (char*)sharedMemBaseAddr );
				}

				ptr->nextChunk->nextChunk = (struct CHUNK_HEADER*)
									( (char*)(ptr->nextChunk->nextChunk) - (char*)sharedMemBaseAddr );
			}
			if ( (ptr->nextChunk->prevChunk) != NULL )
			{
				ptr->nextChunk->prevChunk = (struct CHUNK_HEADER*)
									( (char*)(ptr->nextChunk->prevChunk) - (char*)sharedMemBaseAddr );
			}
			if ( (ptr->nextChunk) != NULL )
			{
				ptr->nextChunk = (struct CHUNK_HEADER*)( (char*)(ptr->nextChunk) -
				    (char*)sharedMemBaseAddr );
			}

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at IF-middle 2 of mymalloc.");
#endif

			ptr->lock = 0;	// unlock it

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at IF-end of mymalloc.");
#endif
		}
		else
		{

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at else 1 of mymalloc.");
#endif
			// Chunk is of same size. Allocate it as it is.
			ptr->lock				=	1;	// lock it.
			ptr->chunkStatus		=	1;	// now it is allocated
			ptr->lock				=	0;	// unlock it.

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong at else 2 of mymalloc.");
#endif
		}

#if PADDING_TEST
		// Initialize the padding area with a signature to check for any overflows.
		for ( paddingCtr = 1 ; paddingCtr <= ALLOC_AREA_PADDING_LEN ; paddingCtr++ )
		{
			(((char*)ptr) + sizeof(struct CHUNK_HEADER) + (ptr->chunkSize) - ALLOC_AREA_PADDING_LEN )[paddingCtr-1] =
								allocPaddingSignature[paddingCtr-1] = (char)paddingCtr;
		}
#endif

		// point to the start of data bytes in the allocated chunk
		ptr	=	(struct CHUNK_HEADER*)(((char*)ptr) + sizeof(struct CHUNK_HEADER));
	}

#if PADDING_TEST
	if ( sizeBackup == 60 && size != 188 )
		syslog (LOG_ERR, "MyMalloc ERROR: size wrong at end of mymalloc.");
#endif

	if ( ( pthread_mutex_ret = pthread_mutex_unlock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Couldn't unlock. Should atleast log this error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_unlock returns with %d", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
	}

#if PADDING_TEST
			if ( sizeBackup == 60 && size != 188 )
				syslog (LOG_ERR, "MyMalloc ERROR: size wrong just before returning from mymalloc.");
#endif

	return ((void*)ptr);
#endif
}

/*
	Initialize the available memory into one large chunk of free memory.
*/
int myfree(void *freeptr)
{
#if PADDING_TEST
	int paddingCtr;
#endif

#ifdef TEST_WITH_SYSTEM_MALLOC
	return(free(ptr));
#else
	pthread_mutex_t	*Mem_Lock = (pthread_mutex_t*)sharedMemAllocStartAddr;
	struct CHUNK_HEADER *nChunk = NULL, *pChunk = NULL;
	int pthread_mutex_ret = 0;
	// point to the header for this chunk.
	struct CHUNK_HEADER *ptr = (struct CHUNK_HEADER*)(((char*)freeptr) - sizeof(struct CHUNK_HEADER));

	if ( sharedMemAllocStartAddr == NULL || endOfSharedmemory == NULL )	// Idiot check allocation pointer
		return -1;

	if ( freeptr <  (sharedMemAllocStartAddr + sizeof(pthread_mutex_t)
			+ sizeof(struct CHUNK_HEADER)) ||
				( freeptr >= endOfSharedmemory )  )
	{
		char errorBuf[256];
		// Cannot acquire lock. Should return with error.
		sprintf (errorBuf, "MyMalloc ERROR: myfree() received an invalid address = 0x%x from PID = %d.", freeptr, getpid());
		syslog( LOG_ERR, errorBuf );
		return (-2);
	}

// We do NOT need to be in the critical section for this check.
#if PADDING_TEST
	
		for ( paddingCtr = 1 ; paddingCtr <= ALLOC_AREA_PADDING_LEN ; paddingCtr++ )
		{
			allocPaddingSignature[paddingCtr-1] = (char)paddingCtr;
		}
		// Initialize the padding area with a signature to check for any overflows.
		if ( memcmp( (((char*)freeptr) + ptr->chunkSize - ALLOC_AREA_PADDING_LEN ),
					allocPaddingSignature, ALLOC_AREA_PADDING_LEN) != 0 )
			{	// Overflow detected.
				char errorBuf[256];
				sprintf (errorBuf, "MyMalloc ERROR: myfree() detected memory OVERFLOW at address = 0x%x from PID = %d.", freeptr, getpid());
				syslog( LOG_ERR, errorBuf );
			}
#endif

	if ( (pthread_mutex_ret = pthread_mutex_lock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Cannot acquire lock. Should return with error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_lock returns with %d in myfree().", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
		return -3;
	}
		// Critical section

	//nodesAllocated--;
	//sprintf(perrorTxt, "nodesFreed = %ld", nodesAllocated);
	//perror(perrorTxt);

	ptr->lock = 1;	// lock it
	ptr->chunkStatus = 0;	// Now this chunk is free

	// Convert offset to absolute address.
	if ( ptr->nextChunk != NULL )
		nChunk = (struct CHUNK_HEADER *)(((char*)sharedMemBaseAddr) + (unsigned long)(ptr->nextChunk));
	else
		nChunk = NULL;
	// if next and/or prev chunks are already free, those should be
	// combined into this chunk.
	if ( nChunk != NULL && ( nChunk->chunkStatus) == 0 )
	{	// combine current with next chunk
	    ptr->chunkSize		+=	(nChunk->chunkSize) + sizeof(struct CHUNK_HEADER);
	    if ( nChunk->nextChunk != NULL )    // Convert offset to absolute address
	        ptr->nextChunk = (struct CHUNK_HEADER *)(((char*)sharedMemBaseAddr) + (unsigned long)(nChunk->nextChunk));
	    else
	        ptr->nextChunk = NULL;

	    if ( ptr->nextChunk != NULL )
	        (ptr->nextChunk)->prevChunk = ptr;

	    // new nextChunk, required below while merging with previous.
	    nChunk = ptr->nextChunk; // Store absolute address before conversion

	    // Convert absolute addresses to offsets.
	    if ( (nChunk != NULL) && (nChunk->prevChunk != NULL) )
	        nChunk->prevChunk = (struct CHUNK_HEADER*)
	                ( (char*)(nChunk->prevChunk) - (char*)sharedMemBaseAddr);
	    if ( ptr->nextChunk != NULL )
	        ptr->nextChunk = (struct CHUNK_HEADER*)
	                ( (char*)(ptr->nextChunk) - (char*)sharedMemBaseAddr);
	    // Note: nChunk is still absolute value.
	}

	// Convert offset to absolute address.
	// prevChunk remains untouched in the operations above. So it is still an offset.
	if ( ptr->prevChunk != NULL )
	    pChunk = (struct CHUNK_HEADER *)(((char*)sharedMemBaseAddr) + (unsigned long)(ptr->prevChunk));
	else
	    pChunk = NULL;

	// Check if previous chunk is free... if yes combine it.
	if ( pChunk != NULL && ( pChunk->chunkStatus) == 0 )
	{	// combine current with previous chunk
	    pChunk->lock = 1;	// lock previous chunk
	    pChunk->chunkSize		+=	(ptr->chunkSize) + sizeof(struct CHUNK_HEADER);
	    pChunk->nextChunk		=	nChunk;
	    if  ( nChunk != NULL )
	        nChunk->prevChunk = pChunk;

	    // Convert absolute addresses to offsets.
	    if ( pChunk->nextChunk != NULL )
	        pChunk->nextChunk = (struct CHUNK_HEADER*)
	                ( (char*)(pChunk->nextChunk) - (char*)sharedMemBaseAddr);
	    if ( (nChunk != NULL) && (nChunk->prevChunk != NULL) )
	        nChunk->prevChunk = (struct CHUNK_HEADER*)
	                ( (char*)(nChunk->prevChunk) - (char*)sharedMemBaseAddr);

	    pChunk->lock = 0;	// unlock
	}

	ptr->lock = 0;	// unlock it

	if ( ( pthread_mutex_ret = pthread_mutex_unlock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Couldn't unlock. Should atleast log this error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_unlock returns with %d", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
	}
	return 0;
#endif
}

#if PADDING_TEST			// DO NOT REMOVE
/*
	Testing purpose.

		Pass the allocated pointer to know the allocated chunk size. This function
	is written to track the bug where chunk size is getting changed between
	mymalloc() call and myfree().
*/
int getAllocatedChunkSize(void *freeptr)
{
	pthread_mutex_t	*Mem_Lock = (pthread_mutex_t*)sharedMemAllocStartAddr;
	int pthread_mutex_ret = 0, allocatedSize = 0;
	// point to the header for this chunk.
	struct CHUNK_HEADER *ptr = (struct CHUNK_HEADER*)(((char*)freeptr) - sizeof(struct CHUNK_HEADER));

	if ( sharedMemAllocStartAddr == NULL || endOfSharedmemory == NULL )	// Idiot check allocation pointer
		return -1;

	if ( freeptr <  (sharedMemAllocStartAddr + sizeof(pthread_mutex_t)
			+ sizeof(struct CHUNK_HEADER)) ||
				( freeptr >= endOfSharedmemory )  )
	{
		char errorBuf[256];
		// Cannot acquire lock. Should return with error.
		sprintf (errorBuf, "MyMalloc ERROR: getAllocatedChunkSize() received an invalid address = 0x%x from PID = %d.", freeptr, getpid());
		syslog( LOG_ERR, errorBuf );
		return (-2);
	}

	if ( (pthread_mutex_ret = pthread_mutex_lock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Cannot acquire lock. Should return with error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_lock returns with %d in getAllocatedChunkSize().", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
		return -3;
	}
		// Critical section
		allocatedSize = ptr->chunkSize;

	if ( ( pthread_mutex_ret = pthread_mutex_unlock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Couldn't unlock. Should atleast log this error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_unlock returns with %d", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
	}
	return allocatedSize;
}

#endif			// DO NOT REMOVE

int max_nodes = 0;  // Statistics purpose

void displayVisualMemStatus()
{
	pthread_mutex_t	*Mem_Lock = (pthread_mutex_t*)sharedMemAllocStartAddr;
	struct CHUNK_HEADER *ptr	= (struct CHUNK_HEADER *)(((char*)sharedMemAllocStartAddr) + sizeof(pthread_mutex_t));
	int i = 0, j = 0, allocatedSize = 0, freeSize = 0;
	int pthread_mutex_ret = 0;

	if ( (pthread_mutex_ret = pthread_mutex_lock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Cannot acquire lock. Should return with error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_lock returns with %d in displayVisualMemStatus().", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
		return;
	}
	        // Critical section

	printf("\nList Status: ");
	while ( ptr != NULL )
	{
		// Display this node
		// [F/A][size] = free or allocated

		// WARNING: DO NOT CHANGE THIS printf CALL TO PRINT MACRO
		printf("%c/%d==>>", "FA0"[ptr->chunkStatus], ptr->chunkSize );
		if ( ptr->nextChunk != NULL )
		{
			ptr = (struct CHUNK_HEADER*)( (unsigned long)(ptr->nextChunk) +
			                            ((char*)sharedMemBaseAddr));
			if ( ptr->chunkStatus == 0 )
			{
				freeSize += ptr->chunkSize;
				j++;
			}
			else
			{
				allocatedSize += ptr->chunkSize;
				i++;
			}

		}
		else
			ptr = NULL;
	}

	if ( ( pthread_mutex_ret = pthread_mutex_unlock(Mem_Lock) ) != 0 )
	{
		char errorBuf[256];
		// Couldn't unlock. Should atleast log this error.
		sprintf (errorBuf, "MyMalloc ERROR: pthread_mutex_unlock returns with %d", pthread_mutex_ret);
		syslog( LOG_ERR, errorBuf );
	}
	printf ("NULL\n");

	if ( (i+j) > max_nodes )
	{
		max_nodes = ( i + j ) ;  // New maximum.
		printf("Maximum Nodes reached = %d\nAllocated nodes = %d, Free nodes = %d\n", max_nodes, i, j);
		printf("Allocated data = %d, Free data = %d\n", allocatedSize, freeSize );
	}
}

int getRoot(char *AscendentRoot)
{
	char buffer[256];
	char *tmp_str;

	tmp_str = getenv( "ASCENDENTROOT" );
	if( tmp_str == NULL )
	{
		sprintf( buffer, "XCOM ERROR getRoot: Unable to get value of AscendentRoot: Environment \
		not initialized for Ascendent" );
		PRINT( "%s\n", buffer );
		syslog( LOG_ERR, buffer);
		return 0;
	}
	else
	{
		sprintf( AscendentRoot, "%s/", tmp_str );
		return 1;	
	}
}
