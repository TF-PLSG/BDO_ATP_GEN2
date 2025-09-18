/******************************************************************************
  
        Module: NTUtils.c
  
         Title: PTE Utility Routines for Windows NT
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\NTUTILS.C  $
   
      Rev 1.12   Oct 19 1998 09:14:20   drodden
   Changed init-event-logging to call _tzset() to initialize the runtime 
   library's internal timezone fields so that Greenwich Mean calcs
   work correctly.  This is a windows specific API that will need a 
   platform independent solution.
   
   
      Rev 1.11   Aug 06 1998 16:35:40   drodden
   Changed to keep a static app-name string so repeated calls to 
   GetAppName will return this rather than going to the OS and 
   parsing each time.
   
   
      Rev 1.10   May 28 1998 14:49:22   drodden
   Changed the function, that returns the IPC_TRACE environment variable, to not log
   an error if the variable is not there (OK if it is not).  It will simply return an empty list.
   
   
      Rev 1.9   May 27 1998 13:52:50   drodden
   Added a function to return the IPC_TRACE environment variable.  This is
   a list of service/app names that should start an IPC trace when they load.
   
   
      Rev 1.8   27 May 1998 11:26:54   rmalathk
   minor changes, additions to make ntutils.h
   portable to unix
   
      Rev 1.7   May 18 1998 11:16:22   drodden
   Added a function to return this systems name.  The code
   was taken from tf_os of the logger/reply projects.  Need to 
   consolidate this functionality into one location.
   
   
      Rev 1.6   Apr 09 1998 09:10:44   drodden
   Added registry access routines for: 
     xipc host name, primary machine ID, secondary machine ID.
   
   
      Rev 1.5   Apr 06 1998 16:18:44   drodden
   Changed #include's of all system core headers to be done with double quotes (" ")
   rather than with <> as pre-compiled headers.
   Changed ntutils to declare its event logging handle privately rather than public.
   
   
      Rev 1.4   17 Feb 1998 15:59:32   MSALEH
   1) define a new function to get the logger
   queue name from the registry
   2) use the function when sending refeed/restart
   transactions
   
      Rev 1.3   Feb 13 1998 08:04:22   drodden
   Added a function to return the trace directory.
   
   
      Rev 1.2   10 Feb 1998 14:06:26   rmalathk
   1. Fixed bug in routine to read xipcinstance
   from registry.
   2. modified routines that use registry to log
   events on failure.
   
      Rev 1.1   04 Feb 1998 14:39:18   rmalathk
   replaced hardcoded directory names with
   code to lookup the registry
   
      Rev 1.0   19 Dec 1997 11:19:56   rmalathk
   Initial Revision
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifndef _WINDOWS_
   #include <windows.h>
#endif

#include "NTUtils.h"


PRIVATE HANDLE hEventSource;

PRIVATE CHAR   system_name [MAX_SYSTEM_NAME_SIZE] = {'\0'};
PRIVATE CHAR   app_name    [MAX_APP_NAME_SIZE]    = {'\0'};
PRIVATE pCHAR  key_pinnacle = "SOFTWARE\\Hypercom\\Pinnacle";


//******************************************************************************
//******************************************************************************
void GetAppName( pCHAR AppName )
{
   char tempsrc[ 256 ];
   pCHAR  src;
   DWORD Data;


   if (app_name [0] == '\0')
   {
      src = tempsrc;

      // Get the module name
      for( Data = GetModuleFileName( NULL, src, 256 );
          Data > 0; Data-- )
      {
         if( tempsrc[ Data-1 ] == '\\' )
         {
            src = &tempsrc[ Data ];
            break;
         }
      }

      // strip the '.exe' at the end
      for( Data = strlen( src );
          Data > 0; Data-- )
      {
         if( src[ Data-1 ] == '.' )
         {
            src[ Data-1 ] = 0;
            break;
         }
      }

      strcpy (app_name, src);
   }

   strcpy (AppName, app_name);
}



//******************************************************************************
//******************************************************************************
void InitEventLogging()
{
   LONG Ret;
   HKEY hRegKey;
   DWORD Result;
   DWORD Data;
   BYTE   bData [100];

   DWORD  Type, Size;
   char   ValueName[20];

   char Buffer[256];
   char tempsrc[ 256 ];
   pCHAR  src;
   char MsgFile[256];


   #ifndef _WINDOWS_
      _tzset();
   #endif

   // Get the module name
   src = tempsrc;

   for( Data = GetModuleFileName( NULL, src, 256 );
       Data > 0; Data-- )
   {
      if( tempsrc[ Data-1 ] == '\\' )
      {
         src = &tempsrc[ Data ];
         break;
      }
   }

   // strip the '.exe' at the end
   for( Data = strlen( src );
       Data > 0; Data-- )
   {
      if( src[ Data-1 ] == '.' )
      {
         src[ Data-1 ] = 0;
         break;
      }
   }

   /* get pinnacle home directory and path to message file */
   wsprintf( Buffer, key_pinnacle);
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   wsprintf( ValueName, "PinnacleHome" );
   Size = sizeof( bData );
   Ret = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );
   lstrcpy( MsgFile, (pCHAR)bData );

   lstrcat( MsgFile, MSG_TEMPLATE_FILE );
   RegCloseKey( hRegKey );


   wsprintf( Buffer, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", src );

   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   if( Ret != ERROR_SUCCESS )
   {

      RegCreateKeyEx( HKEY_LOCAL_MACHINE,
                  (LPTSTR)Buffer,
                  0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                  NULL, &hRegKey, &Result );

      wsprintf( Buffer, MsgFile );
      RegSetValueEx( hRegKey,
                  (LPCTSTR)"EventMessageFile",
                  0, REG_EXPAND_SZ,
                  (CONST BYTE *)Buffer,
                  (DWORD)lstrlen( Buffer ) + 1 );

      Data = 7;
      RegSetValueEx( hRegKey,
                  (LPCTSTR)"TypesSupported",
                  0, REG_DWORD,
                  (CONST BYTE * )&Data,
                  sizeof( int ) );
   }

   RegCloseKey( hRegKey );
   hEventSource = RegisterEventSource( NULL, src );
}




//******************************************************************************
//******************************************************************************
void ShutDownEventLogging()
{
   (VOID)DeregisterEventSource( hEventSource );
}




//******************************************************************************
//******************************************************************************
void LogEvent( pCHAR msg, int type )
{
    LPTSTR  lpszMsg[1];

   lpszMsg[0] = msg;

   switch( type )
   {
      case ERROR_MSG : 
         if (hEventSource != NULL) 
            ReportEvent(hEventSource,     // handle of event source
               EVENTLOG_ERROR_TYPE,       // event type
                  0,                         // event category
                  1,                         // event ID
                  NULL,                      // current user's SID
                  1,                         // strings in lpszStrings
                  0,                         // no bytes of raw data
                  (const char **)lpszMsg,    // array of error strings
                  NULL);                     // no raw data
         break;
   
      case INFO_MSG :
         if (hEventSource != NULL) 
            ReportEvent(hEventSource,     // handle of event source
               EVENTLOG_INFORMATION_TYPE, // event type
                  0,                         // event category
                  1,                         // event ID
                  NULL,                      // current user's SID
                  1,                         // strings in lpszStrings
                  0,                         // no bytes of raw data
                  (const char **)lpszMsg,    // array of error strings
                  NULL);                     // no raw data
         break;

      case WARN_MSG :
         if (hEventSource != NULL) 
            ReportEvent(hEventSource,     // handle of event source
               EVENTLOG_WARNING_TYPE,     // event type
                  0,                         // event category
                  1,                         // event ID
                  NULL,                      // current user's SID
                  1,                         // strings in lpszStrings
                  0,                         // no bytes of raw data
                  (const char **)lpszMsg,    // array of error strings
                  NULL);                     // no raw data
         break;
   }
}



//******************************************************************************
//******************************************************************************
INT GetPinnacleRootDirectory( pCHAR PinnacleRoot )
{
   LONG Ret;
   HKEY hRegKey;
   BYTE   bData [100];
   DWORD  Type, Size;
   char   ValueName[20];
   char Buffer[256];

   wsprintf( Buffer, key_pinnacle);
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to open Pinnacle key in Registry. Registry may not be \
               initialized for Pinnacle", ERROR_MSG );
      return 0;
   }

   wsprintf( ValueName, "PinnacleHome" );
   Size = sizeof( bData );

   Ret = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );

   RegCloseKey( hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to query PinnacleHome value from Registry. Registry may not be \
               initialized for Pinnacle", ERROR_MSG );
      return 0;
   }

   lstrcpy( PinnacleRoot, (pCHAR)bData );
   lstrcat( PinnacleRoot, "\\" );
   return 1;   
}



//******************************************************************************
//******************************************************************************
INT GetPinnacleBinDirectory( pCHAR PinnacleBin )
{
   if( GetPinnacleRootDirectory( PinnacleBin ) )
   {
      lstrcat( PinnacleBin, "bin\\" );
      return 1;
   }
   else
      return 0;
}



//******************************************************************************
//******************************************************************************
INT GetPinnacleConfigDirectory( pCHAR PinnacleConfig )
{

   if( GetPinnacleRootDirectory( PinnacleConfig ) )
   {
      lstrcat( PinnacleConfig, "config\\" );
      return 1;
   }
   else
      return 0;
}



//******************************************************************************
//******************************************************************************
INT GetPinnacleLogDirectory( pCHAR PinnacleLog )
{
   if( GetPinnacleRootDirectory( PinnacleLog ) )
   {
      lstrcat( PinnacleLog, "log\\" );
      return 1;
   }
   else
      return 0;
}




//******************************************************************************
//******************************************************************************
INT GetPinnacleTraceDirectory ( pCHAR PinnacleTrace )
{
   if( GetPinnacleRootDirectory( PinnacleTrace ) )
   {
      lstrcat( PinnacleTrace, "trace\\" );
      return 1;
   }
   else
      return 0;
}




//******************************************************************************
//******************************************************************************
INT GetXipcInstanceName( pCHAR InstanceName )
{
   LONG Ret;
   HKEY hRegKey;
   BYTE   bData [100];
   DWORD  Type, Size;
   char   ValueName[20];
   char Buffer[256];


   wsprintf( Buffer, "SOFTWARE\\Hypercom\\Pinnacle\\PinnacleServiceManager\\XipcConfig" );
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to open XipcConfig key in Registry. Registry may not be \
               initialized for Pinnacle", ERROR_MSG );
      return 0;
   }

   wsprintf( ValueName, "XipcInstanceName" );
   Size = sizeof( bData );

   Ret = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );
   
   RegCloseKey( hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to query XipcInstanceName value from Registry. Registry \
               may not be initialized for Pinnacle", ERROR_MSG );
      return 0;
   }

   lstrcpy( InstanceName, (pCHAR)bData );
   return 1;
}



//******************************************************************************
//******************************************************************************
BOOLEAN GetXipcHostName( pCHAR host_name )
{
   LONG  Ret;
   HKEY  hRegKey;
   BYTE  bData [100];
   DWORD Type, Size;
   char  ValueName[20];
   char  Buffer[256];


   wsprintf (Buffer, "System\\CurrentControlSet\\Control\\Session Manager\\Environment");

   Ret = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                       (LPCTSTR)Buffer,
                       0,
                       KEY_ALL_ACCESS,
                       &hRegKey );

   if (Ret != ERROR_SUCCESS)
   {
      LogEvent ("ntutils.c: Unable to open Environment key in Registry.", ERROR_MSG);
      return (false);
   }

   wsprintf (ValueName, "XIPCHOST");
   Size = sizeof(bData);

   Ret = RegQueryValueEx (hRegKey, ValueName, NULL, &Type, bData, &Size);
   
   RegCloseKey (hRegKey);

   if (Ret != ERROR_SUCCESS)
   {
      LogEvent ("ntutils.c: Unable to query XIPCHOST value from Registry.", ERROR_MSG);
      return (false);
   }

   lstrcpy (host_name, (pCHAR)bData );
   return (true);
}



//******************************************************************************
//******************************************************************************
INT GetLoggerQueName( pCHAR LoggerName )
{
   LONG  Ret;
   HKEY  hRegKey;
   BYTE  bData [100];
   DWORD Type, Size;
   char  ValueName[20];
   char  Buffer[256];

   wsprintf( Buffer, "SOFTWARE\\Hypercom\\Pinnacle\\PinnacleServiceManager\\LoggerQueName" );
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to open LoggerQueName key in Registry. Registry may not be \
               initialized for Pinnacle", ERROR_MSG );
      return 0;
   }

   wsprintf( ValueName, "LoggerQueName" );
   Size = sizeof( bData );

   Ret = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );

   RegCloseKey( hRegKey );
   
   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to query LoggerQueName value from Registry. Registry \
               may not be initialized for Pinnacle", ERROR_MSG );
      return 0;
   }

   lstrcpy( LoggerName, (pCHAR)bData );
   return 1;
}



//******************************************************************************
//******************************************************************************
BOOLEAN GetPrimaryMachineId ( pCHAR PrimaryMachineId )
{
   LONG  retcode;
   HKEY  hRegKey;
   DWORD Type, Size;
   BYTE  bData [100];
   char  ValueName[20];
   char  Buffer[256];


   /* get system indicator from registry */
   wsprintf (Buffer, key_pinnacle);

   retcode = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                           (LPCTSTR)Buffer,
                           0,
                           KEY_QUERY_VALUE,
                           &hRegKey);

   if (retcode != ERROR_SUCCESS)
      return (false);

   wsprintf( ValueName, "PrimaryMachineId");
   Size = sizeof( bData );

   retcode = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );
   
   RegCloseKey (hRegKey);

   if (retcode != ERROR_SUCCESS)
      return (false);

   lstrcpy (PrimaryMachineId, (char *)bData );
   return (true);
} //end of GetPrimaryMachineId



//******************************************************************************
//******************************************************************************
BOOLEAN GetSecondaryMachineId ( pCHAR SecondaryMachineId )
{
   LONG  retcode;
   HKEY  hRegKey;   
   DWORD Type, Size;
   BYTE  bData [30];
   char  ValueName[20];
   char  Buffer[256];


   /* get system indicator from registry */
   wsprintf (Buffer, key_pinnacle);

   retcode = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                           (LPCTSTR)Buffer,
                           0,
                           KEY_QUERY_VALUE,
                           &hRegKey);

   if (retcode != ERROR_SUCCESS)
      return(false);

   wsprintf( ValueName, "SecondaryMachineId");
   Size = sizeof( bData );

   retcode = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );
   
   RegCloseKey (hRegKey);

   if (retcode != ERROR_SUCCESS)
      return (false);

   lstrcpy (SecondaryMachineId, (char *)bData );
   return (true);
} // end of GetSecondaryMachineId



/******************************************************************************/
/******************************************************************************/
pCHAR GetThisSystemsName ()
{
   DWORD rc;
   CHAR sectionname [] = "TF_CONFIG";
   CHAR filename    [256];

   
   if (system_name [0] == '\0')
   {
      GetPinnacleConfigDirectory (filename);
      strcat (filename, "\\tf.ini");

      rc = GetPrivateProfileString (
            sectionname,	         // points to section name 
            "TF_THIS_SYSTEM",       // points to key name 
            "S1",        	         // points to default string 
            system_name,            // points to destination buffer 
            sizeof(system_name),    // size of destination buffer 
            filename                // points to initialization filename 
            );
   }

   return (system_name);
}



//******************************************************************************
//******************************************************************************
BOOLEAN GetIpcTraceList ( pCHAR trace_list )
{
   LONG  Ret;
   HKEY  hRegKey;
   BYTE  bData [256];
   DWORD Type, Size;
   char  ValueName[20];
   char  Buffer[256];


   trace_list [0] = '\0';     /* init to empty list */

   wsprintf (Buffer, "System\\CurrentControlSet\\Control\\Session Manager\\Environment");

   Ret = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                       (LPCTSTR)Buffer,
                       0,
                       KEY_ALL_ACCESS,
                       &hRegKey );

   if (Ret != ERROR_SUCCESS)
   {
      LogEvent ("ntutils.c: Unable to open Environment key in Registry.", ERROR_MSG);
      return (false);
   }

   wsprintf (ValueName, "IPC_TRACE");
   Size = sizeof(bData);

   Ret = RegQueryValueEx (hRegKey, ValueName, NULL, &Type, bData, &Size);
   
   RegCloseKey (hRegKey);

   /* if it is there OK, otherwise return an empty list */
   if (Ret == ERROR_SUCCESS)
      lstrcpy (trace_list, (pCHAR)bData);

   return (true);
}


