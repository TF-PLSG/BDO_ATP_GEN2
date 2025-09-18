/*
				N M A L L O C  -  A memory allocator
				Copyright 2010

		Written by : NIRBHAY JOSHI

		for XCOM, Thoughtfocus Information technologies
*/

// Standard header file to be included when using this library.

#ifndef __MYMALLOC_H__
#define __MYMALLOC_H__

#include <thread.h>
#include <synch.h>
#include <syslog.h>

#define PADDING_TEST 1

#if PADDING_TEST
// Safeguarding the dynamic allocation area from an overflow.
// A padding of this many bytes will be put between SHR_MEM_LAYOUT and
// allocation start area.
#define ALLOC_AREA_PADDING_LEN 128
#endif

struct CHUNK_HEADER
{
    // All structures must be 8 byte aligned for
    // SPARC64 architecture, else we might get BUS error (SIGBUS).
    long chunkSize;
    struct CHUNK_HEADER *nextChunk;
    struct CHUNK_HEADER *prevChunk;
    char lock;
    char chunkStatus; // 0 = free, 1 = allocated
    char padding[sizeof(int) - 2 * sizeof(char)];    // 4 byte aligned (for 32 bit build) or 8 bytes aligned (for 64 bit build)
};	// As the CHUNK_HEADER will be allocated dynamically, size of this structure must be properly aligned (requirement for SPARC systems).

// Currently not used
struct CHUNK_FOOTER
{
	long chunkSize;
};

int myinit();
void* mymalloc(size_t size);
int myfree();
void displayVisualMemStatus();
int getRoot(char *AscendentRoot);
void ptetime_get_timestamp1 ( char *dest_str );
#endif


