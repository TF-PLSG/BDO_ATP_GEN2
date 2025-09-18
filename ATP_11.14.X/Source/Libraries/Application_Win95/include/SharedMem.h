
#ifndef _SHAREDMEM
#define _SHAREDMEM

#include "Lock.h"


struct ATPsharedMem
{
	short idle_status;
	
	CTime shm_time;
};

class CSharedMem
{
public :


	ATPsharedMem *atpshmmem;
	HANDLE atpMap;

	short CreateSharedMemorySegment();
	short DestroySharedMemorySegment();
	short MapSharedMemorySegment();
	short SetIdleStatus(short flag);
	short GetIdleStatus();
	short CreateMutexLock();
	short DeleteMutexLock();
	short SetLastUpdateTime(CTime val);
	CTime GetLastUpdateTime();

	
};

#endif