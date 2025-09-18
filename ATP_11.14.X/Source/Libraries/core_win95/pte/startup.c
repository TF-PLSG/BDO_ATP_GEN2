/******************************************************************************
  
        Module: startup.c
  
         Title: start/stop routines for console/service based apps
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\STARTUP.C  $
   
      Rev 1.5   Oct 15 1999 11:12:26   MSALEH
   Move SSL_SUPPORT to preprocessot definition
   section of project files
   
      Rev 1.4   Oct 06 1999 09:40:36   MSALEH
   add #include "pte_ipc.h"
   
      Rev 1.3   Oct 06 1999 09:35:14   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.2   Sep 28 1999 14:40:52   MSALEH
   AIX Modifications
   
      Rev 1.1   Apr 06 1998 16:18:56   drodden
   Changed #include's of all system core headers to be done with double quotes (" ")
   rather than with <> as pre-compiled headers.
   Changed ntutils to declare its event logging handle privately rather than public.
   
   
      Rev 1.0   19 Dec 1997 11:37:54   rmalathk
   Initial Revision
   
******************************************************************************/
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <stdio.h>

#include "ntutils.h"
#include "pteipc.h"

/****************************************************************************/
/*					G L O B A L   V A R I A B L E S                         */
/*****************************************************************************/
#ifdef WIN32
DWORD  ThreadId;
HANDLE hThread, hProcess;
#else
pthread_t mainThread;
#endif

int    EndProcessSignalled;
int    MainProcessDone;

extern void MainProcessor();
extern void EndProcess();


/*****************************************************************************/
/*****************************************************************************/
int Start()
{
   #ifndef WIN32
   int retcode;
   #endif

   EndProcessSignalled = 0;
   MainProcessDone = 0;
   InitEventLogging();

   #ifdef WIN32
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
   #else

   retcode = pthread_create( &mainThread, NULL, (void*(*)(void*))MainProcessor, NULL);
   if (retcode != 0)
   {
      LogEvent("Unable to create MainProcessor thread", ERROR_MSG);
      printf(  "Unable to create MainProcessor thread" );
      return(0);
   }

   #endif

   return (1);
}



/*****************************************************************************/
/*****************************************************************************/
void Stop()
{
	EndProcessSignalled = 1;
	EndProcess();

	while( !MainProcessDone )
      pteipc_sleep(100);
	
   ShutDownEventLogging();

   #ifdef WIN32
	if( hThread )
	   TerminateThread( hThread, 0 );
   #else
   if ( mainThread )
      pthread_cancel( mainThread );
   #endif

   pteipc_sleep( 500 );
}




