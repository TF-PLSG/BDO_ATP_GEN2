/******************************************************************************
  
        Module: service.c
  
         Title: Main routine for service based apps
  
   Description: 

   $Log:   N:\PVCS\PTE\SYSTEM\Service.c  $
   
      Rev 1.0   19 Dec 1997 11:37:54   rmalathk
   Initial Revision
   
******************************************************************************/
///////////////////////////////////////////////////////

#ifndef _WINDOWS_
#include <windows.h>
#endif

// TODO: Put your server Name here 
#define SERVER_NAME "TestApp"

// this event is signalled when the
// worker thread ends
//
HANDLE                  hServDoneEvent = NULL;
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwGlobalErr;
int						ConsoleTest = 0; // Will always be false in this file.
CHAR					ServiceName[12];

VOID    service_main(DWORD dwArgc, LPTSTR *lpszArgv);
VOID    service_ctrl(DWORD dwCtrlCode);

BOOL    ReportStatusToSCMgr(DWORD dwCurrentState,
                            DWORD dwWin32ExitCode,
                            DWORD dwCheckPoint,
                            DWORD dwWaitHint);

VOID    StopService( LPTSTR lpszMsg );
int 	Started;

extern int Start();
extern void Stop();


//  main() --
//      all main does is call StartServiceCtrlDispatcher
//      to register the main service thread.  When the
//      API returns, the service has stopped, so exit.
//


main()
{

    SERVICE_TABLE_ENTRY dispatchTable[] = 
    {
        { TEXT( SERVER_NAME ), (LPSERVICE_MAIN_FUNCTION)service_main },
        { NULL, NULL }
    };


    if (!StartServiceCtrlDispatcher(dispatchTable))
	{
		if( GetLastError () != 1 )
    		StopService( "StartServiceCtrlDispatcher error" );
	}
}



//  service_main() --
//      this function takes care of actually starting the service,
//      informing the service controller at each step along the way.
//      After launching the worker thread, it waits on the event
//      that the worker thread will signal at its termination.
//
VOID
service_main( DWORD dwArgc, LPTSTR *lpszArgv )
{
    DWORD dwWait;

	lstrcpy( ServiceName, lpszArgv[0] );
    // register our service control handler:
    //
    sshStatusHandle = RegisterServiceCtrlHandler(
                                    TEXT( SERVER_NAME ),
                                    (LPHANDLER_FUNCTION) service_ctrl);

    if (!sshStatusHandle)
        goto cleanup;

    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;

    // report the status to Service Control Manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        1,                     // checkpoint
        6000))                 // wait hint
        goto cleanup;

	Started =  Start();	// Start all threads

	MessageBeep( (unsigned int)-1 );


    // create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    //
    hServDoneEvent = CreateEvent(
        NULL,    // no security attributes
        TRUE,    // manual reset event
        FALSE,   // not-signalled
        NULL);   // no name

    if (hServDoneEvent == (HANDLE)NULL)
        goto cleanup;

    // report the status to the service control manager.
    
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        2,                     // checkpoint
        3000))                 // wait hint
        goto cleanup;

	// Start the service threads

	if( !Started )
		goto cleanup;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_RUNNING, // service state
        NO_ERROR,        // exit code
        0,               // checkpoint
        0))              // wait hint
        goto cleanup;

    // wait indefinitely until hServDoneEvent is signaled.
    //
    dwWait = WaitForSingleObject(
        hServDoneEvent,  // event object
        INFINITE);       // wait indefinitely

cleanup:

    if (hServDoneEvent != NULL)
        CloseHandle(hServDoneEvent);


    // try to report the stopped status to the service control manager.
    //
    if (sshStatusHandle != 0)
        (VOID)ReportStatusToSCMgr(
                            SERVICE_STOPPED,
                            dwGlobalErr,
                            0,
                            0);

    // When SERVICE MAIN FUNCTION returns in a single service
    // process, the StartServiceCtrlDispatcher function in
    // the main thread returns, terminating the process.
    //
    return;
}

//  service_ctrl() --
//      this function is called by the Service Controller whenever
//      someone calls ControlService in reference to our service.
//
VOID
service_ctrl(DWORD dwCtrlCode)
{
    DWORD  dwState = SERVICE_RUNNING;

    // Handle the requested control code.
    //
    switch(dwCtrlCode) {

        // Pause the service if it is running.
        //
        case SERVICE_CONTROL_PAUSE:

            if (ssStatus.dwCurrentState == SERVICE_RUNNING) {				
                dwState = SERVICE_PAUSED;
            }
            break;

        // Resume the paused service.
        //
        case SERVICE_CONTROL_CONTINUE:

            if (ssStatus.dwCurrentState == SERVICE_PAUSED) {
                dwState = SERVICE_RUNNING;
            }
            break;

        // Stop the service.
        //
        case SERVICE_CONTROL_STOP:

            dwState = SERVICE_STOP_PENDING;

            // Report the status, specifying the checkpoint and waithint,
            //  before setting the termination event.
            //
            ReportStatusToSCMgr(
                    SERVICE_STOP_PENDING, // current state
                    NO_ERROR,             // exit code
                    1,                    // checkpoint
                    3000);                // waithint

			Stop();
            SetEvent( hServDoneEvent );
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    // send a status response.
    //
    ReportStatusToSCMgr(dwState, NO_ERROR, 0, 0);
}

// utility functions...

// ReportStatusToSCMgr() --
//      This function is called by the ServMainFunc() and
//      ServCtrlHandler() functions to update the service's status
//      to the service control manager.
//
BOOL
ReportStatusToSCMgr(DWORD dwCurrentState,
                    DWORD dwWin32ExitCode,
                    DWORD dwCheckPoint,
                    DWORD dwWaitHint)
{
    BOOL fResult;

    // Disable control requests until the service is started.
    //
    if (dwCurrentState == SERVICE_START_PENDING)
        ssStatus.dwControlsAccepted = 0;
    else
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
            SERVICE_ACCEPT_PAUSE_CONTINUE;

    // These SERVICE_STATUS members are set from parameters.
    //
    ssStatus.dwCurrentState = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwCheckPoint = dwCheckPoint;

    ssStatus.dwWaitHint = dwWaitHint;

    // Report the status of the service to the service control manager.
    //
    if (!(fResult = SetServiceStatus(
                sshStatusHandle,    // service reference handle
                &ssStatus))) {      // SERVICE_STATUS structure

        // If an error occurs, stop the service.
        //
        StopService("SetServiceStatus ERROR");
    }
    return fResult;
}

// The StopService function can be used by any thread to report an
//  error, or stop the service.
//
VOID
StopService( LPTSTR lpszMsg )
{
    CHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[ 1 ];

    dwGlobalErr = GetLastError();

    // Use event logging to log the error.
    //
    hEventSource = RegisterEventSource(NULL,
                            TEXT( SERVER_NAME ));

    wsprintf(chMsg, "%s error: %d, %s", SERVER_NAME, dwGlobalErr, lpszMsg);
    lpszStrings[0] = chMsg;

    if (hEventSource != NULL) {
        ReportEvent(hEventSource, // handle of event source
            EVENTLOG_ERROR_TYPE,  // event type
            0,                    // event category
            1,                    // event ID
            NULL,                 // current user's SID
            1,                    // strings in lpszStrings
            0,                    // no bytes of raw data
            lpszStrings,          // array of error strings
            NULL);                // no raw data

        (VOID) DeregisterEventSource(hEventSource);
    }

    // Set a termination event to stop SERVICE MAIN FUNCTION.
    //
    SetEvent(hServDoneEvent);
}

