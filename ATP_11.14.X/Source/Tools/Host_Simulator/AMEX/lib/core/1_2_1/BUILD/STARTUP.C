/******************************************************************************
  
        Module: startup.c
  
         Title: start/stop routines for console/service based apps
  
   Description: 

   $Log:   N:\PVCS\PTE\SYSTEM\STARTUP.C  $
   
      Rev 1.1   Apr 06 1998 16:18:56   drodden
   Changed #include's of all system core headers to be done with double quotes (" ")
   rather than with <> as pre-compiled headers.
   Changed ntutils to declare its event logging handle privately rather than public.
   
   
      Rev 1.0   19 Dec 1997 11:37:54   rmalathk
   Initial Revision
   
******************************************************************************/
#include <windows.h>
#include <stdio.h>

#include "NTUtils.h"

/****************************************************************************/
/*					G L O B A L   V A R I A B L E S                         */
/*****************************************************************************/
DWORD ThreadId;
HANDLE hThread, hProcess;
int	EndProcessSignalled;
int MainProcessDone;

extern void MainProcessor();
extern void EndProcess();


/*****************************************************************************/
/*****************************************************************************/
int Start()
{
	EndProcessSignalled = 0;
	MainProcessDone = 0;
	InitEventLogging();

	/* Create main thread for the process */
	hThread = CreateThread( NULL,
						    0,
						    ( LPTHREAD_START_ROUTINE )MainProcessor,
						    NULL,
						    0,
						    &ThreadId );
	if( hThread == NULL )
	{
		printf(  "Unable to create MainProcessor thread" );
		LogEvent( "Unable to create MainProcessor thread", ERROR_MSG );
		return 0;
	}

	return 1;
}



/*****************************************************************************/
/*****************************************************************************/
void Stop()
{
	EndProcessSignalled = 1;
	EndProcess();

	while( !MainProcessDone )
		Sleep( 100 );

	ShutDownEventLogging();

	if( hThread )
		TerminateThread( hThread, 0 );
	Sleep( 500 );
}




