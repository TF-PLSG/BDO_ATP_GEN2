/******************************************************************************
  
        Module: NTUtils.h
  
         Title: PTE Utility Routines for Windows NT
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\NTUTILS.H  $
   
      Rev 1.9   May 27 1998 13:52:50   drodden
   Added a function to return the IPC_TRACE environment variable.  This is
   a list of service/app names that should start an IPC trace when they load.
   
   
      Rev 1.8   27 May 1998 11:26:56   rmalathk
    
   
      Rev 1.7   May 18 1998 16:23:50   doxtoby
   Add definitionof ALERT_MSG
   
      Rev 1.6   May 18 1998 11:16:22   drodden
   Added a function to return this systems name.  The code
   was taken from tf_os of the logger/reply projects.  Need to 
   consolidate this functionality into one location.
   
   
      Rev 1.5   Apr 09 1998 09:10:46   drodden
   Added registry access routines for: 
     xipc host name, primary machine ID, secondary machine ID.
   
   
      Rev 1.4   Apr 06 1998 16:18:46   drodden
   Changed #include's of all system core headers to be done with double quotes (" ")
   rather than with <> as pre-compiled headers.
   Changed ntutils to declare its event logging handle privately rather than public.
   
   
      Rev 1.3   17 Feb 1998 15:59:34   MSALEH
   1) define a new function to get the logger
   queue name from the registry
   2) use the function when sending refeed/restart
   transactions
   
      Rev 1.2   Feb 13 1998 08:04:24   drodden
   Added a function to return the trace directory.
   
   
      Rev 1.1   10 Feb 1998 14:06:26   rmalathk
   1. Fixed bug in routine to read xipcinstance
   from registry.
   2. modified routines that use registry to log
   events on failure.
   
      Rev 1.0   19 Dec 1997 11:19:56   rmalathk
   Initial Revision
   
******************************************************************************/

#ifndef __NTUTILS__
#define __NTUTILS__


#include "basictyp.h"


#define ERROR_MSG 1
#define INFO_MSG  2
#define WARN_MSG  3
#define ALERT_MSG 4

#define MSG_TEMPLATE_FILE "\\bin\\LogMsgs.Exe"


void InitEventLogging();

void ShutDownEventLogging();

void LogEvent( pCHAR msg, INT type );

void GetAppName( pCHAR AppName );

pCHAR GetThisSystemsName ();

INT GetPinnacleRootDirectory( pCHAR PinnacleRoot );

INT GetPinnacleBinDirectory( pCHAR PinnacleBin );

INT GetPinnacleConfigDirectory( pCHAR PinnacleConfig );

INT GetPinnacleLogDirectory( pCHAR PinnacleLog );

INT GetPinnacleTraceDirectory ( pCHAR PinnacleTrace );

INT GetXipcInstanceName( pCHAR InstanceName );

INT GetLoggerQueName( pCHAR LoggerName );

BOOLEAN GetXipcHostName        ( pCHAR host_name );

BOOLEAN GetPrimaryMachineId    ( pCHAR PrimaryMachineId );

BOOLEAN GetSecondaryMachineId  ( pCHAR SecondaryMachineId );

BOOLEAN GetIpcTraceList        ( pCHAR trace_list );


/* for non windows systems only */
#ifndef WIN32

   #define FILE_OPEN_ERROR          1
   #define FILE_READ_ERROR          2
   #define SECTION_NOT_FOUND        3
   #define FILE_FORMAT_ERROR        4
   #define BUFFER_SIZE_INSUFFICIENT    5
   #define KEY_NOT_FOUND            6

   void itoa( INT int_val, pCHAR str_val, INT base );
   void ltoa( LONG int_val, pCHAR str_val, INT base );

   INT GetPrivateProfileString( pCHAR section_name,
                                pCHAR key_name,
                                pCHAR default_str,
                                pCHAR dest_buffer,
                                UINT  buffer_size,
                                pCHAR file_name );

#endif



#endif

