#include "stdafx.h"
#include "SharedMem.h"


CDMLock *shmLock;

short CSharedMem::CreateMutexLock()
{
	shmLock = new CDMLock("SharedMemLock");
	return 1;
}

short CSharedMem::DeleteMutexLock()
{
	delete shmLock;
	shmLock = NULL;

	return 1;
}

short CSharedMem::CreateSharedMemorySegment()
{
	/* Create a file map for sharing the memory. This is for sharing the memory between same processes. */
	atpMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,
							 sizeof(ATPsharedMem),"ATPsharedMem");
	
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		atpMap = ::OpenFileMapping(FILE_MAP_WRITE,FALSE,"ATPsharedMem");
	}
	
	return 0;
}

short CSharedMem::DestroySharedMemorySegment()
{
	::UnmapViewOfFile(atpshmmem);
	::CloseHandle(atpMap);

	return 0;
}


short CSharedMem::MapSharedMemorySegment()
{	
	atpshmmem = (ATPsharedMem*)::MapViewOfFile(atpMap,FILE_MAP_WRITE,0,0,sizeof(ATPsharedMem));
	return 0;
}

short CSharedMem::SetIdleStatus(short status)
{
	if(shmLock == NULL)
		return -1;

	if(shmLock->Lock())
	{
		atpshmmem->idle_status = status;
	}
	shmLock->Unlock();

	return status;
}

short CSharedMem::GetIdleStatus()
{
	short ret;

	ret = atpshmmem->idle_status;

	return ret;
}


CTime CSharedMem::GetLastUpdateTime()
{	
	CTime tmp_time;

	tmp_time = atpshmmem->shm_time;
	
	shmLock->Unlock();
	return tmp_time;	
}

short CSharedMem::SetLastUpdateTime(CTime timeval)
{
	if(shmLock == NULL)
		return -1;

	if(shmLock->Lock())
	{
		atpshmmem->shm_time = timeval;
	}
	shmLock->Unlock();
	return 1;
}