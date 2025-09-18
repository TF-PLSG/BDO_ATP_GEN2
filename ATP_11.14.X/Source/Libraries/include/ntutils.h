/******************************************************************************
  
        Module: NTUtils.h
  
         Title: PTE Utility Routines for Windows NT
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\NTUTILS.H  $
   
      Rev 1.17   Nov 19 1999 10:29:44   MSALEH
   add GetCoreVersion() to display the current version
   of the core
   
      Rev 1.16   Oct 22 1999 11:37:22   MSALEH
   Priority based message retrieval
   
      Rev 1.15   Sep 29 1999 16:30:06   MSALEH
   AIX Modifications
   
      Rev 1.14   Jan 12 1999 13:59:16   MSALEH
   added the following functions:
   GetFirstServerId() , GUI uses this to connect to first server
   GetSecondServerId() , GUI uses this to connect to second server
   GetPrivateField1(), GetPrivateField2(),GetPrivateField2(),
   Private Use Fields, need to document use
   in tf.ini to prevent two apps using one of the fields for two
   different things
   
      Rev 1.13   Nov 19 1998 15:29:22   MSALEH
   added another parameter to GetAscendentFailoverInfo
   to control the receive timeout
   
      Rev 1.12   Nov 18 1998 10:42:00   MSALEH
   change the name of the GetPinnacle... routines to
   GetAscendent..., keep the GetPinnacle... shells
   for backward compatibility
   
      Rev 1.11   Nov 17 1998 12:09:22   MSALEH
   Added new message suntypes under system
   to handle Logger/Reply Primary/Secondary
   designation
   
      Rev 1.10   Nov 09 1998 15:16:46   MSALEH
   Add new API's to get information from both
   systems in a high Availability configuration
   
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

#ifndef WIN32
#include <syslog.h>

#define ERROR_MSG LOG_ERR
#define INFO_MSG  LOG_INFO
#define WARN_MSG  LOG_WARNING
#define ALERT_MSG LOG_ALERT
#else
#define ERROR_MSG 1
#define INFO_MSG  2
#define WARN_MSG  3
#define ALERT_MSG 4
#endif

#define MSG_TEMPLATE_FILE "\\bin\\LogMsgs.Exe"

void InitEventLogging         ();

void ShutDownEventLogging     ();

void LogEvent                 ( pCHAR msg, INT type );

void GetAppName               ( pCHAR AppName );

pCHAR GetThisSystemsName      ();

pCHAR GetOtherSystemsName     ();

INT GetAscendentRootDirectory ( pCHAR AscendentRoot );

INT GetAscendentBinDirectory  ( pCHAR AscendentBin );

INT GetAscendentConfigDirectory( pCHAR AscendentConfig );

INT GetAscendentLogDirectory  ( pCHAR AscendentLog );

INT GetAscendentTraceDirectory( pCHAR AscendentTrace );

INT GetPinnacleRootDirectory  ( pCHAR AscendentRoot );

INT GetPinnacleBinDirectory   ( pCHAR AscendentBin );

INT GetPinnacleConfigDirectory( pCHAR AscendentConfig );

INT GetPinnacleLogDirectory   ( pCHAR AscendentLog );

INT GetPinnacleTraceDirectory ( pCHAR AscendentTrace );

BOOLEAN GetXipcRoot           ( pCHAR XipcRoot );

BOOLEAN GetXipcInstanceName   ( pCHAR InstanceName );

BOOLEAN GetXipcInstanceFile   ( pCHAR InstanceFile );

BOOLEAN GetLoggerQueName      ( pCHAR LoggerName );

BOOLEAN GetOtherLoggerQueName ( pCHAR OtherLoggerName );

BOOLEAN GetLoggerMSQueName    ( pCHAR LoggerMSName );

BOOLEAN GetOtherLoggerMSQueName( pCHAR OtherLoggerMSName );

BOOLEAN GetXipcHostName       ( pCHAR host_name );

BOOLEAN GetThisXipcInstance   ( pCHAR ThisXipcInstance );

BOOLEAN GetOtherXipcInstance  ( pCHAR OtherXipcInstance );

BOOLEAN GetThisMachineId      ( pCHAR ThisMachineId );

BOOLEAN GetThisMachineId_2      ( pCHAR ThisMachineId );
BOOLEAN GetThisMachineId_3      ( pCHAR ThisMachineId );
BOOLEAN GetThisMachineId_4      ( pCHAR ThisMachineId );

BOOLEAN GetHostName_1 ( pCHAR ThisMachineId );
BOOLEAN GetHostName_2 ( pCHAR ThisMachineId );
BOOLEAN GetHostName_3 ( pCHAR ThisMachineId );
BOOLEAN GetHostName_4 ( pCHAR ThisMachineId );

BOOLEAN GetOtherMachineId     ( pCHAR OtherMachineId );

BOOLEAN GetHighAvailableIndicator ( pINT Indicator );

BOOLEAN GetIpcTraceList       ( pCHAR trace_list );

BOOLEAN GetFailoverPingInfo   ( pLONG ping_interval, pINT max_retries, pINT receive_timeout );

BOOLEAN GetFirstServerId      ( pCHAR FirstServerId );

BOOLEAN GetSecondServerId     ( pCHAR SecondServerId );

BOOLEAN GetPrivateField1      ( pCHAR PrivateField1 );
BOOLEAN GetPrivateField2      ( pCHAR PrivateField2 );
BOOLEAN GetPrivateField3      ( pCHAR PrivateField3 );

BOOLEAN GetSystemMessagePriority ( pINT Priority );
BOOLEAN GetQueueDepthThreshold   ( pINT Threshold );

void    GetCoreVersion           ( pCHAR appname );

#ifndef WIN32
/* TF PHANI*/
BOOLEAN SetAppUserId       ( pCHAR app_user_id );
BOOLEAN SetAppServiceName  ( pCHAR app_srvc_name );
BOOLEAN SetAppPassword     ( pCHAR app_password );
BOOLEAN GetAppServiceName  ( pCHAR app_srvc_name );
BOOLEAN GetAppUserId       ( pCHAR app_user_id );
BOOLEAN GetAppPassword     ( pCHAR app_password );

BOOLEAN SetCoreUserId      ( pCHAR core_user_id );
BOOLEAN SetCoreServiceName ( pCHAR core_srvc_name );
BOOLEAN SetCorePassword    ( pCHAR core_password );
BOOLEAN GetCoreServiceName ( pCHAR core_srvc_name );
BOOLEAN GetCoreUserId      ( pCHAR core_user_id );
BOOLEAN GetCorePassword    ( pCHAR core_password );
#endif

/* for non windows systems only */
#ifndef WIN32

   #define FILE_OPEN_ERROR          1
   #define FILE_READ_ERROR          2
   #define SECTION_NOT_FOUND        3
   #define FILE_FORMAT_ERROR        4
   #define BUFFER_SIZE_INSUFFICIENT 5
   #define KEY_NOT_FOUND            6

   void itoa( INT int_val, pCHAR str_val, INT base );
   void ltoa( LONG int_val, pCHAR str_val, INT base );

   INT GetPrivateProfileString( pCHAR section_name,
                                pCHAR key_name,
                                pCHAR default_str,
                                pCHAR dest_buffer,
                                UINT  buffer_size,
                                pCHAR file_name );

   INT WritePrivateProfileString( pCHAR section_name,
                                  pCHAR key_name,
                                  pCHAR value,
                                  pCHAR file_name );


#endif


#endif

