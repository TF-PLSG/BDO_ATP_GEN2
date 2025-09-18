/******************************************************************************
  
        Module: ntutils.c
  
         Title: PTE Utility Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\NTUTILS.C  $
   
      Rev 1.24   Jan 14 2000 15:13:18   MSALEH
   upgrade core version to 1.3.2
   
      Rev 1.23   Nov 19 1999 15:13:00   MSALEH
   Correct bug in get_core_version()
   
      Rev 1.22   Nov 19 1999 10:30:20   MSALEH
   add getCoreVersion() to log the current
   version of the core
   
      Rev 1.21   Oct 22 1999 11:36:42   MSALEH
   Priority based message retrieval
   
      Rev 1.20   Oct 15 1999 11:12:20   MSALEH
   Move SSL_SUPPORT to preprocessot definition
   section of project files
   
      Rev 1.19   Sep 29 1999 14:29:22   MSALEH
   AIX Modifications
   
      Rev 1.18   Sep 28 1999 14:40:46   MSALEH
   AIX Modifications
   
      Rev 1.17   Jan 12 1999 13:59:44   MSALEH
   added the following functions:
   GetFirstServerId() , GUI uses this to connect to first server
   GetSecondServerId() , GUI uses this to connect to second server
   GetPrivateField1(), GetPrivateField2(),GetPrivateField2(),
   Private Use Fields, need to document use
   in tf.ini to prevent two apps using one of the fields for two
   different things
   
      Rev 1.16   Nov 19 1998 15:30:20   MSALEH
   added another parameter to GetAscendentFailoverInfo
   to control receive timeout
   
      Rev 1.15   Nov 18 1998 10:42:56   MSALEH
   Change the GetPinnacle... routines to be
   GetAscendent..., keep the GetPinnacle...
   shells for backward compatibility
   
      Rev 1.14   Nov 17 1998 12:10:56   MSALEH
   Added routines to return the names of this system,
   other system, this logger que name, other
   logger que name, etc...
   
      Rev 1.13   Nov 09 1998 15:18:00   MSALEH
   Add new api's to get information from both
   systems in a high availability configuration
   
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

#ifdef WIN32
#ifndef _WINDOWS_
   #include <windows.h>
#endif
#else
#include <syslog.h>
#include <string.h>
#endif

#include "ntutils.h"

#ifdef WIN32
PRIVATE HANDLE hEventSource;
PRIVATE pCHAR  key_ascendent = "SOFTWARE\\Hypercom\\Ascendent";
#else
PRIVATE CHAR   global_err_str[100];
PRIVATE INT    global_err_code;
#endif

PRIVATE CHAR   system_name [MAX_SYSTEM_NAME_SIZE] = {'\0'};
PRIVATE CHAR   app_name    [MAX_APP_NAME_SIZE]    = {'\0'};
CHAR           ExeName     [MAX_APP_NAME_SIZE];
WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};


#ifdef SOLARIS
/******************************************************************************/
/******************************************************************************/
void itoa( INT int_val, pCHAR str_val, INT base )
{
   pCHAR tmp_buf, end_ptr;
   CHAR  temp_data[12];

   memset( temp_data, 0, sizeof( temp_data ) );
   end_ptr = &temp_data[ sizeof( temp_data ) - 1 ];

   tmp_buf = lltostr( int_val, end_ptr );
   strcpy( str_val, tmp_buf );
}

/******************************************************************************/
/******************************************************************************/
void ltoa( LONG int_val, pCHAR str_val, INT base )
{
   pCHAR tmp_buf, end_ptr;
   CHAR  temp_data[12];

   memset( temp_data, 0, sizeof( temp_data ) );
   end_ptr = &temp_data[ sizeof( temp_data ) - 1 ];

   tmp_buf = lltostr( int_val, end_ptr );
   strcpy( str_val, tmp_buf );
}

#else

#ifndef WIN32
/**************************************************************************
 * itoa()	(after Kernighan & Ritchie)
 **************************************************************************/
void itoa(INT n, pCHAR s, INT base)
{
   INT i, j, c;
   INT sign;

   if ((sign = n) < 0)        /* record sign */
      n = -n;		            /* make n positive */
   i = 0;
   do {		                  /* generate digits in reverse order */
      s[i++] = n% 10 + '0';   /* get next digit */
   } while (( n/= 10) > 0);	/* delete it */
   if (sign < 0)
      s[i++] = '-';
   s[i] = '\0';
   /* reverse string */
   for(i = 0, j = strlen(s)-1; i < j; i++, j--)
      c = s[i], s[i] = s[j], s[j] = c;
}

/**************************************************************************
 * ltoa()	(after Kernighan & Ritchie)
 **************************************************************************/
void ltoa(LONG n, pCHAR s, INT base)
{
   INT i, j, c;
   LONG sign;

   if ((sign = n) < 0)        /* record sign */
      n = -n;		            /* make n positive */
   i = 0;
   do {		                  /* generate digits in reverse order */
      s[i++] = n% 10 + '0';   /* get next digit */
   } while (( n/= 10) > 0);	/* delete it */
   if (sign < 0)
      s[i++] = '-';
   s[i] = '\0';
   /* reverse string */
   for(i = 0, j = strlen(s)-1; i < j; i++, j--)
      c = s[i], s[i] = s[j], s[j] = c;
}
#endif

#endif

/* ****************************************************************************** */
/* ****************************************************************************** */
void GetAppName( pCHAR AppName )
{
#ifdef WIN32

   char tempsrc[ 256 ];
   pCHAR  src;
   DWORD Data;


   if (app_name [0] == '\0')
   {
      src = tempsrc;

      /*  Get the module name */
      for( Data = GetModuleFileName( NULL, src, 256 );
          Data > 0; Data-- )
      {
         if( tempsrc[ Data-1 ] == '\\' )
         {
            src = &tempsrc[ Data ];
            break;
         }
      }

      /*  strip the '.exe' at the end */
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

#else

   INT counter, i;
 
   counter = strlen( ExeName ) - 1;
   for( i = counter; i >= 0; i-- )
   {
       if( ExeName[i] == '/' )
           break;

   }

   strcpy( AppName, &ExeName[i+1] );
   return;

#endif
}

/* ****************************************************************************** */
/* ****************************************************************************** */
void InitEventLogging()
{
#ifdef WIN32

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

   /*  Get the module name */
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

   /*  strip the '.exe' at the end */
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
   wsprintf( Buffer, key_ascendent);
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   wsprintf( ValueName, "AscendentHome" );
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

#else

   char  *logIda = NULL;
   int   logId;
   int   logFlag = LOG_LOCAL1;
   int   badVarValue = 0;
   char  logMsg[256];

   logIda = getenv("ASCENDENT_LOG_ID");
   if(logIda != NULL)
   {
      logId = atoi(logIda);
      switch(logId)
      {
         case 0: logFlag = LOG_LOCAL0;  break;
         case 1: logFlag = LOG_LOCAL1;  break;
         case 2: logFlag = LOG_LOCAL2;  break;
         case 3: logFlag = LOG_LOCAL3;  break;
         case 4: logFlag = LOG_LOCAL4;  break;
         case 5: logFlag = LOG_LOCAL5;  break;
         case 6: logFlag = LOG_LOCAL6;  break;
         case 7: logFlag = LOG_LOCAL7;  break;
         default:
         {
            badVarValue = 1;
         }
      }
   }
   openlog("ASCENDENT", LOG_CONS|LOG_NDELAY|LOG_PID, logFlag);

   if(badVarValue)
   {
      sprintf(logMsg,
        "Invalid value (%d) of ASCENDENT_LOG_ID; defaulting to LOG_LOCAL1",
        logId);
      LogEvent(logMsg, WARN_MSG);
   }

#endif
}

/* ****************************************************************************** */
/* ****************************************************************************** */
void ShutDownEventLogging()
{
#ifdef WIN32
   (VOID)DeregisterEventSource( hEventSource );
#else
   closelog();
#endif
}

/* ****************************************************************************** */
/* ****************************************************************************** */
void LogEvent( pCHAR msg, int type )
{
#ifdef WIN32

    LPTSTR  lpszMsg[1];

   lpszMsg[0] = msg;

   switch( type )
   {
      case ERROR_MSG : 
         if (hEventSource != NULL) 
            ReportEvent(hEventSource,        /*  handle of event source */
               EVENTLOG_ERROR_TYPE,          /*  event type */
                  0,                         /*  event category */
                  1,                         /*  event ID */
                  NULL,                      /*  current user's SID */
                  1,                         /*  strings in lpszStrings */
                  0,                         /*  no bytes of raw data */
                  (const char **)lpszMsg,    /*  array of error strings */
                  NULL);                     /*  no raw data */
         break;
   
      case INFO_MSG :
         if (hEventSource != NULL) 
            ReportEvent(hEventSource,        /*  handle of event source */
               EVENTLOG_INFORMATION_TYPE,    /*  event type */
                  0,                         /*  event category */
                  1,                         /*  event ID */
                  NULL,                      /*  current user's SID */
                  1,                         /*  strings in lpszStrings */
                  0,                         /*  no bytes of raw data */
                  (const char **)lpszMsg,    /*  array of error strings */
                  NULL);                     /*  no raw data */
         break;

      case WARN_MSG :
         if (hEventSource != NULL) 
            ReportEvent(hEventSource,        /*  handle of event source */
               EVENTLOG_WARNING_TYPE,        /*  event type */
                  0,                         /*  event category */
                  1,                         /*  event ID */
                  NULL,                      /*  current user's SID */
                  1,                         /*  strings in lpszStrings */
                  0,                         /*  no bytes of raw data */
                  (const char **)lpszMsg,    /*  array of error strings */
                  NULL);                     /*  no raw data */
         break;
   }
#else
   syslog( type, msg );
#endif

}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetAscendentRootDirectory( pCHAR AscendentRoot )
{
#ifdef WIN32

   LONG  Ret;
   HKEY  hRegKey;
   BYTE  bData [100];
   DWORD Type, Size;
   CHAR  ValueName[20];
   CHAR  Buffer[256];

   wsprintf( Buffer, key_ascendent);
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to open Ascendent key in Registry. Registry may not be \
               initialized for Ascendent", ERROR_MSG );
      return 0;
   }

   wsprintf( ValueName, "AscendentHome" );
   Size = sizeof( bData );

   Ret = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );

   RegCloseKey( hRegKey );

   if( Ret != ERROR_SUCCESS )
   {
      LogEvent( "ntutils.c: Unable to query AscendentHome value from Registry. Registry may not be \
               initialized for Ascendent", ERROR_MSG );
      return 0;
   }

   lstrcpy( AscendentRoot, (pCHAR)bData );
   lstrcat( AscendentRoot, "\\" );
   return 1;   

#else

   CHAR  buffer[256];
   pCHAR tmp_str;

   tmp_str = getenv( "ASCENDENTROOT" );
   if( tmp_str == NULL )
   {
      sprintf( buffer, "Unable to get value of AscendentRoot: Environment \
		 not initialized for Ascendent" );
      printf( "%s\n", buffer );
      LogEvent( buffer, ERROR_MSG );
      return 0;
   }
   else
   {
      sprintf( AscendentRoot, "%s/", tmp_str );
      return 1;	
   }

#endif
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetPinnacleRootDirectory( pCHAR AscendentRoot )
{
   return(GetAscendentRootDirectory(AscendentRoot));
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetAscendentBinDirectory( pCHAR AscendentBin )
{
   if( GetAscendentRootDirectory( AscendentBin ) )
   {
#ifdef WIN32
      lstrcat( AscendentBin, "bin\\" );
#else
      strcat( AscendentBin, "bin/" );
#endif

      return 1;
   }
   else
      return 0;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetPinnacleBinDirectory( pCHAR AscendentBin )
{
   return(GetAscendentBinDirectory(AscendentBin));
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetAscendentConfigDirectory( pCHAR AscendentConfig )
{
   if( GetAscendentRootDirectory( AscendentConfig ) )
   {
#ifdef WIN32
      lstrcat( AscendentConfig, "config\\" );
#else
      strcat( AscendentConfig, "config/" );
#endif

      return 1;
   }
   else
      return 0;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetPinnacleConfigDirectory( pCHAR AscendentBin )
{
   return(GetAscendentConfigDirectory(AscendentBin));
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetAscendentLogDirectory( pCHAR AscendentLog )
{
   if( GetAscendentRootDirectory( AscendentLog ) )
   {
#ifdef WIN32
      lstrcat( AscendentLog, "log\\" );
#else
      strcat( AscendentLog, "log/" );
#endif

      return 1;
   }
   else
      return 0;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetPinnacleLogDirectory( pCHAR AscendentBin )
{
   return(GetAscendentLogDirectory(AscendentBin));
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetAscendentTraceDirectory ( pCHAR AscendentTrace )
{
   if( GetAscendentRootDirectory( AscendentTrace ) )
   {
#ifdef WIN32
      lstrcat( AscendentTrace, "trace\\" );
#else
      strcat( AscendentTrace, "trace/" );
#endif

      return 1;
   }
   else
      return 0;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
INT GetPinnacleTraceDirectory( pCHAR AscendentBin )
{
   return(GetAscendentTraceDirectory(AscendentBin));
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetXipcRoot ( pCHAR XipcRoot )
{
   DWORD rc;
   CHAR  sectionname  [] = "XIPC";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
            
#ifdef WIN32
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "XIPC_ROOT",                    /*  points to key name  */
    "c:\\xipc",                     /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );
#else
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "XIPC_ROOT",                    /*  points to key name  */
    "//xipc",                       /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );
#endif

   strcpy(XipcRoot, tmpstr);

   return 1;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetXipcInstanceFile( pCHAR InstanceFile )
{
   DWORD rc;
   CHAR  sectionname  [] = "XIPC";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
#ifdef WIN32 
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "XIPC_INSTANCE_FILE",           /*  points to key name  */
    "c:\\xipc\\pte_ipc",            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );
#else
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "XIPC_INSTANCE_FILE",           /*  points to key name  */
    "// xipc//pte_ipc",             /* points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );
#endif

   strcpy(InstanceFile, tmpstr);

   return 1;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetXipcInstanceName( pCHAR InstanceName )
{
   DWORD rc;
   CHAR  sectionname  [] = "XIPC";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "XIPC_INSTANCE_NAME",           /*  points to key name  */
    "pte_ipc",                      /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(InstanceName, tmpstr);

   return 1;
}


/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetXipcHostName( pCHAR host_name )
{
   DWORD rc;
   CHAR  sectionname  [] = "XIPC";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "XIPC_HOST",                    /*  points to key name  */
    "local",     	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy (host_name, tmpstr);

   return 1;
}



/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetLoggerQueName( pCHAR LoggerName )
{
   DWORD rc;
   CHAR  sectionname  [] = "TF_CONFIG";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "TF_THIS_SYSTEM",               /*  points to key name  */
    "S1",        	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   sprintf(LoggerName, "L_AP_%s", tmpstr);

   return 1;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetOtherLoggerQueName( pCHAR OtherLoggerName )
{
   DWORD rc;
   CHAR  sectionname  [] = "TF_CONFIG";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "TF_OTHER_SYSTEM",              /*  points to key name  */
    "S2",        	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   sprintf(OtherLoggerName, "L_AP_%s", tmpstr);

   return 1;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetLoggerMSQueName( pCHAR LoggerMSName )
{
   DWORD rc;
   CHAR  sectionname  [] = "TF_CONFIG";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "TF_THIS_SYSTEM",               /*  points to key name  */
    "S1",        	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   sprintf(LoggerMSName, "L_MS_%s", tmpstr);

   return 1;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetOtherLoggerMSQueName( pCHAR OtherLoggerMSName )
{
   DWORD rc;
   CHAR  sectionname  [] = "TF_CONFIG";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "TF_OTHER_SYSTEM",              /*  points to key name  */
    "S2",        	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   sprintf(OtherLoggerMSName, "L_MS_%s", tmpstr);

   return 1;
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetThisXipcInstance ( pCHAR ThisXipcInstance )
{
   DWORD rc;
   CHAR  sectionname  [] = "TF_CONFIG";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "TF_PRIMARY_INSTANCE",          /*  points to key name  */
    "@pte_ipc", 	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(ThisXipcInstance, tmpstr);

   return 1;
} /* end of GetThisXipcInstance */



/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetOtherXipcInstance ( pCHAR OtherXipcInstance )
{
   DWORD rc;
   CHAR  sectionname  [] = "TF_CONFIG";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "TF_SECONDARY_INSTANCE",        /*  points to key name  */
    "@pte_ipc", 	            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(OtherXipcInstance, tmpstr);

   return 1;
} /*  end of GetOtherMachineId */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetThisMachineId ( pCHAR ThisMachineId )
{
   DWORD rc;
   CHAR  sectionname  [] = "SRVCMON";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "THIS_MACHINE_ID",              /*  points to key name  */
    "ASCENDENT_SVR1",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(ThisMachineId, tmpstr);

   return 1;
} /* end of GetThisMachineId */


/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetOtherMachineId ( pCHAR OtherMachineId )
{
   DWORD rc;
   CHAR  sectionname  [] = "SRVCMON";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "OTHER_MACHINE_ID",             /*  points to key name  */
    "ASCENDENT_SVR2",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(OtherMachineId, tmpstr);

   return 1;
} /* end of GetOtherMachineId */

/******************************************************************************/
/******************************************************************************/
pCHAR GetThisSystemsName ()
{
   DWORD rc;
   CHAR  sectionname [] = "TF_CONFIG";
   CHAR  filename    [256];
   CHAR  tmpstr      [256];

   if (system_name [0] == '\0')
   {
      GetAscendentConfigDirectory(tmpstr);
      sprintf(filename, "%stf.ini", tmpstr);

      rc = GetPrivateProfileString (
            sectionname,	    /*  points to section name  */
            "TF_THIS_SYSTEM",       /*  points to key name  */
            "S1",        	    /*  points to default string  */
            system_name,            /*  points to destination buffer  */
            sizeof(system_name),    /*  size of destination buffer  */
            filename                /*  points to initialization filename  */
            );
   }

   return (system_name);
}

/******************************************************************************/
/******************************************************************************/
pCHAR GetOtherSystemsName ()
{
   DWORD rc;
   CHAR sectionname [] = "TF_CONFIG";
   CHAR filename    [256];
   CHAR tmpstr      [256];

   if (system_name [0] == '\0')
   {
      GetAscendentConfigDirectory(tmpstr);
      sprintf(filename, "%stf.ini", tmpstr);

      rc = GetPrivateProfileString (
            sectionname,	    /*  points to section name  */
            "TF_OTHER_SYSTEM",      /*  points to key name  */
            "S2",        	    /*  points to default string  */
            system_name,            /*  points to destination buffer  */
            sizeof(system_name),    /*  size of destination buffer  */
            filename                /*  points to initialization filename  */
            );
   }

   return (system_name);
}

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetIpcTraceList ( pCHAR trace_list )
{
   DWORD rc;
   CHAR  sectionname  [] = "SRVCMON";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "IPC_TRACE",                    /*  points to key name  */
    "",                             /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(trace_list, tmpstr);

   return 1;
}

/******************************************************************************/
/******************************************************************************/
BOOLEAN GetFailoverPingInfo( pLONG ping_interval, pINT max_retries, pINT receive_timeout )
{
   DWORD rc;
   CHAR  sectionname  [] = "SRVCMON";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "FAILOVER_PING_INTERVAL",       /*  points to key name  */
    "10",                           /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   *ping_interval = atol(tmpstr);

   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "FAILOVER_MAX_RETRIES",         /*  points to key name  */
    "5",                            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   *max_retries = atoi(tmpstr);

   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "FAILOVER_RECEIVE_TIMEOUT",     /*  points to key name  */
    "5",                            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   *receive_timeout = atoi(tmpstr);

   return 1;
} /* end of GetFailoeverPingInfo */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetHighAvailableIndicator( pINT Indicator )
{
   DWORD rc;
   CHAR  sectionname  [] = "SRVCMON";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "HIGH_AVAILABILITY_INDICATOR",  /*  points to key name  */
    "0",                            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   *Indicator = atoi(tmpstr);

   return 1;
} 

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetFirstServerId ( pCHAR FirstServerId )
{
   DWORD rc;
   CHAR  sectionname  [] = "GUI";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "FIRST_SERVER_ID",              /*  points to key name  */
    "ASCENDENT_SVR1",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(FirstServerId, tmpstr);

   return 1;
} /* end of GetFirstServerId */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetSecondServerId ( pCHAR SecondServerId )
{
   DWORD rc;
   CHAR  sectionname  [] = "GUI";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "SECOND_SERVER_ID",             /*  points to key name  */
    "ASCENDENT_SVR2",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(SecondServerId, tmpstr);

   return 1;
} /* end of GetSecondServerId */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetPrivateField1 ( pCHAR PrivateField1 )
{
   DWORD rc;
   CHAR  sectionname  [] = "GUI";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "PRIVATE_FIELD1",               /*  points to key name  */
    "",                             /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(PrivateField1, tmpstr);

   return 1;
} /* end of GetPrivateField1 */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetPrivateField2 ( pCHAR PrivateField2 )
{
   DWORD rc;
   CHAR  sectionname  [] = "GUI";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "PRIVATE_FIELD2",               /*  points to key name  */
    "",                             /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(PrivateField2, tmpstr);

   return 1;
} /* end of GetPrivateField2 */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetPrivateField3 ( pCHAR PrivateField3 )
{
   DWORD rc;
   CHAR  sectionname  [] = "GUI";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "PRIVATE_FIELD3",               /*  points to key name  */
    "",                             /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(PrivateField3, tmpstr);

   return 1;
} /* end of GetPrivateField3 */

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetSystemMessagePriority( pINT Priority )
{
   DWORD rc;
   CHAR  sectionname  [] = "STARTUP";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	                  /*  points to section name  */
    "SYSTEM_MESSAGE_PRIORITY",      /*  points to key name  */
    "2",                            /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   *Priority = atoi(tmpstr);

   return 1;
} 

/* ****************************************************************************** */
/* ****************************************************************************** */
BOOLEAN GetQueueDepthThreshold( pINT Threshold )
{
   DWORD rc;
   CHAR  sectionname  [] = "XIPC";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	                  /*  points to section name  */
    "QUEUE_DEPTH_THRESHOLD",        /*  points to key name  */
    "15",                           /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   *Threshold = atoi(tmpstr);

   return 1;
} 

/* ****************************************************************************** */
/* ****************************************************************************** */
void GetCoreVersion( pCHAR appname )
{
   CHAR buffer[256];

   sprintf(buffer, "Ascendent Application %s is using Core Version 1.3.2", appname);
   LogEvent(buffer, INFO_MSG);
} 

/* TF Phani- Srikanth - DB Password changes*/
/* ****************************************************************************** */
BOOLEAN GetCoreUserId ( pCHAR core_user_id )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "CORE_USER_ID",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(core_user_id, tmpstr);

   return 1;
} /* end of GetCoreUserId */

/* ****************************************************************************** */
BOOLEAN GetCorePassword ( pCHAR core_password )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "CORE_PASSWORD",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   DecryptPassword(tmpstr, core_password);


   return 1;
} /* end of GetCorePassword */

/* ****************************************************************************** */
BOOLEAN GetCoreServiceName ( pCHAR core_srvc_name )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "CORE_SERVICE_NAME",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(core_srvc_name, tmpstr);

   return 1;
} /* end of GetCoreServiceName */

/* ****************************************************************************** */
BOOLEAN GetAppUserId ( pCHAR app_user_id )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "APP_USER_ID",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(app_user_id, tmpstr);

   return 1;
} /* end of GetAppUserId */

/* ****************************************************************************** */
BOOLEAN GetAppPassword ( pCHAR app_password )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "APP_PASSWORD",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   DecryptPassword(tmpstr, app_password);

   return 1;
} /* end of GetAppPassword */
 
/* ****************************************************************************** */
BOOLEAN GetAppServiceName ( pCHAR app_srvc_name )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "APP_SERVICE_NAME",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(app_srvc_name, tmpstr);

   return 1;
} /* end of GetAppServiceName */

/* ****************************************************************************** */
/* Set methods */

BOOLEAN SetAppPassword ( pCHAR app_password )
{
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
   
   WritePrivateProfileString(
       "DATA_SOURCE",  /* pointer to section name*/
        "APP_PASSWORD",  /* pointer to key name*/
        app_password ,   /* pointer to string to add*/
        filename  /* pointer to initialization filename*/
   );

   return 1;
} /* end of SetAppPassword */

/* ****************************************************************************** */
BOOLEAN SetCorePassword ( pCHAR core_password )
{
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   
   WritePrivateProfileString(
        "DATA_SOURCE",    /* pointer to section name*/
        "CORE_PASSWORD",  /* pointer to key name*/
        core_password ,   /* pointer to string to add*/
        filename          /* pointer to initialization filename*/
   );

   return 1;
} /* end of SetCorePassword */

/* ****************************************************************************** */
BOOLEAN SetAppUserId ( pCHAR app_user_id )
{
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   WritePrivateProfileString(
       "DATA_SOURCE",  /* pointer to section name*/
        "APP_USER_ID",  /* pointer to key name*/
        app_user_id ,   /* pointer to string to add*/
        filename  /* pointer to initialization filename*/
   );

   return 1;
} /* end of SetAppUserId */

/* ****************************************************************************** */
BOOLEAN SetCoreUserId ( pCHAR core_user_id )
{
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   WritePrivateProfileString(
       "DATA_SOURCE",  /* pointer to section name*/
        "CORE_USER_ID",  /* pointer to key name*/
        core_user_id ,   /* pointer to string to add*/
        filename  /* pointer to initialization filename*/
   );

   return 1;
} /* end of SetCoreUserId */

/* ****************************************************************************** */
BOOLEAN SetAppServiceName ( pCHAR app_srvc_name )
{
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   WritePrivateProfileString(
       "DATA_SOURCE",        /* pointer to section name*/
        "APP_SERVICE_NAME",  /* pointer to key name*/
        app_srvc_name ,   /* pointer to string to add*/
        filename             /* pointer to initialization filename*/
   );

   return 1;
} /* end of SetAppServiceName */

/* ****************************************************************************** */
BOOLEAN SetCoreServiceName ( pCHAR core_user_id )
{
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   WritePrivateProfileString(
       "DATA_SOURCE",  /* pointer to section name*/
        "CORE_SERVICE_NAME",  /* pointer to key name*/
        core_user_id ,   /* pointer to string to add*/
        filename  /* pointer to initialization filename*/
   );

   return 1;
} /* end of SetCoreServiceName */

/* End of set functions*/
#ifndef WIN32
/******************************************************************************/
/******************************************************************************/
INT GetPrivateProfileString( pCHAR section_name,
                             pCHAR key_name,
                             pCHAR default_str,
                             pCHAR dest_buffer,
                             UINT  buffer_size,
                             pCHAR file_name )
{
    FILE *src_file;
    CHAR buffer[256], full_section_name[256];
    INT  found_sect, found_eq;
    UINT temp;

    /* copy the default string to the destination buffer */
    strcpy( dest_buffer, default_str );
    sprintf( full_section_name, "[%s]", section_name);

    /* try to open the source file */
    src_file = fopen( file_name, "r+" );
    if( src_file == NULL )
    {
        sprintf( global_err_str, "Unable to open file - %s", file_name  );
        global_err_code = FILE_OPEN_ERROR;
        return 0;
    }

    /* file is open, go through each line of the file..... */
    found_sect = 0;
    while( !feof( src_file ) )
    {
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            global_err_code = FILE_READ_ERROR;
            fclose( src_file );
            return 0;
        }

        /* ....and look for the specified section */
        if( strstr( buffer, full_section_name ) != NULL )
        {
            /* found the section, break out */
            found_sect = 1;
            break;
        }
    }
    
    /* if end of file was reached and section was not found, return */
    if( !found_sect )
    {
        sprintf( global_err_str, "section not found" );
        global_err_code = SECTION_NOT_FOUND;
        fclose( src_file );
        return 0;
    }

    /* section has been located, now look for requested key_name */
    while( !feof( src_file ) )
    {
        /* get each line in the section.... */
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            global_err_code = FILE_READ_ERROR;
            fclose( src_file );
            return 0;
        }

        /* ....and check if it contains the required key_name */
        if( strncmp( key_name, buffer, strlen( key_name ) ) == 0 )
        {
            /* found the key_name, retrieve the value */
            found_eq = 0;
            for( temp = strlen( key_name ); temp < strlen( buffer ); temp++ )
            {
                if( buffer[temp] == '=' )
                {
                    found_eq = 1;
                    break;
                }
            }

            if( !found_eq )
            {
                sprintf( global_err_str, "File format error - %s", file_name  );
                global_err_code = FILE_FORMAT_ERROR;
                fclose( src_file );
                return 0;
            }

            while( ++temp < strlen( buffer ) && buffer[temp] == ' ' );
            if( strlen( &buffer[temp] ) > buffer_size )
            {
                sprintf( global_err_str, "buffer size insufficient - %s", file_name  );
                global_err_code = BUFFER_SIZE_INSUFFICIENT;
                fclose( src_file );
                return 0;
            }

            strcpy( dest_buffer, &buffer[temp] );
            fclose( src_file );
            return 1;
        }
        else if( buffer[0] == '[' )
        {
            /* reached end of section, key_name was not found */
            sprintf( global_err_str, "key not found" );
            global_err_code = KEY_NOT_FOUND;
            fclose( src_file );
            return 0;
        }
    }

    /* reached end of file, key_name was not found */
    sprintf( global_err_str, "key not found" );
    global_err_code = KEY_NOT_FOUND;
    fclose( src_file );
    return 0;
}

INT GetPrivateProfileStringSpaces( pCHAR section_name,
                             pCHAR key_name,
                             pCHAR default_str,
                             pCHAR dest_buffer,
                             UINT  buffer_size,
                             pCHAR file_name )
{
    FILE *src_file;
    CHAR buffer[256], full_section_name[256];
    INT  found_sect, found_eq;
    UINT temp;

    /* copy the default string to the destination buffer */
    strcpy( dest_buffer, default_str );
    sprintf( full_section_name, "[%s]", section_name);

    /* try to open the source file */
    src_file = fopen( file_name, "r+" );
    if( src_file == NULL )
    {
        sprintf( global_err_str, "Unable to open file - %s", file_name  );
        global_err_code = FILE_OPEN_ERROR;
        return 0;
    }

    /* file is open, go through each line of the file..... */
    found_sect = 0;
    while( !feof( src_file ) )
    {
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            global_err_code = FILE_READ_ERROR;
            fclose( src_file );
            return 0;
        }

        /* ....and look for the specified section */
        if( strstr( buffer, full_section_name ) != NULL )
        {
            /* found the section, break out */
            found_sect = 1;
            break;
        }
    }

    /* if end of file was reached and section was not found, return */
    if( !found_sect )
    {
        sprintf( global_err_str, "section not found" );
        global_err_code = SECTION_NOT_FOUND;
        fclose( src_file );
        return 0;
    }

    /* section has been located, now look for requested key_name */
    while( !feof( src_file ) )
    {
        /* get each line in the section.... */
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            global_err_code = FILE_READ_ERROR;
            fclose( src_file );
            return 0;
        }

        /* ....and check if it contains the required key_name */
        if( strncmp( key_name, buffer, strlen( key_name ) ) == 0 )
        {
            /* found the key_name, retrieve the value */
            found_eq = 0;
            for( temp = strlen( key_name ); temp < strlen( buffer ); temp++ )
            {
                if( buffer[temp] == '=' )
                {
                    found_eq = 1;
                    break;
                }
            }

            if( !found_eq )
            {
                sprintf( global_err_str, "File format error - %s", file_name  );
                global_err_code = FILE_FORMAT_ERROR;
                fclose( src_file );
                return 0;
            }

            //while( ++temp < strlen( buffer ) && buffer[temp] == ' ' );
            ++temp;
            if( strlen( &buffer[temp] ) > buffer_size )
            {
                sprintf( global_err_str, "buffer size insufficient - %s", file_name  );
                global_err_code = BUFFER_SIZE_INSUFFICIENT;
                fclose( src_file );
                return 0;
            }

            strcpy( dest_buffer, &buffer[temp] );
            fclose( src_file );
            return 1;
        }
        else if( buffer[0] == '[' )
        {
            /* reached end of section, key_name was not found */
            sprintf( global_err_str, "key not found" );
            global_err_code = KEY_NOT_FOUND;
            fclose( src_file );
            return 0;
        }
    }

    /* reached end of file, key_name was not found */
    sprintf( global_err_str, "key not found" );
    global_err_code = KEY_NOT_FOUND;
    fclose( src_file );
    return 0;
}
/****************************************************************************
 * A Unix implementation of WIN32's utility for editing Microsoft's
 * ini file format.
 ***************************************************************************/
INT WritePrivateProfileString( pCHAR section_name,
                               pCHAR key_name,
                               pCHAR value,
                               pCHAR file_name )
{
   FILE *src_file;
   FILE *tmp_file;
   CHAR tmpFilename[256];
   CHAR buffer[256];
   CHAR newBuffer[256];
   INT found_sect, found_key, found_eq;
   UINT temp;
   INT key_length;
   CHAR section_buf[256];

   /* Initialization */
   key_length = strlen(key_name);
   sprintf(section_buf, "[%s]", section_name);

   /* try to open the source file */
   src_file = fopen( file_name, "r" );
   if( src_file == NULL )
   {
      sprintf( global_err_str, "Unable to open file - %s", file_name  );
      global_err_code = FILE_OPEN_ERROR;
      return 0;
   }

   /* try to open the temporary file */
   sprintf(tmpFilename, "%s.tmp", file_name);
   tmp_file = fopen( tmpFilename, "w" );
   if( tmp_file == NULL )
   {
      sprintf( global_err_str, "Unable to open file - %s", tmpFilename  );
      global_err_code = FILE_OPEN_ERROR;
      fclose( src_file );
      return 0;
   }

   /* file is open, go through each line of the file..... */
   found_sect = 0;
   while( !feof( src_file ) )
   {
      fgets( buffer, sizeof( buffer ), src_file );

      /* ....and look for the specified section */
      if ( strstr( buffer, section_buf) != NULL )
      {
         /* found the section */
         found_sect = 1;

         /* Copy the section line to the temp file */
         fputs(buffer, tmp_file);

         found_key = 0;
         /* section has been located, now look for requested key_name */
         while( !feof( src_file ) )
         {
            /* get each line in the section.... */
            fgets( buffer, sizeof( buffer ), src_file );

            /* ....and check if it contains the required key_name */
            if(
                ( strncmp( key_name, buffer, key_length ) == 0 )
                &&
                (
                   (buffer[key_length] == '=')
                   ||
                   (buffer[key_length] == ' ')
                   ||
                   (buffer[key_length] == '\t')
                )
              )
            {
               /* found the key_name */

               /* Replace the key and value */
               sprintf(newBuffer, "%s=%s\n", key_name, value);
               fputs(newBuffer, tmp_file);

               /* from here, we'll just loop thru until we've copied the
                  rest of the file */
               break;  /* section while loop */
            }
            else if( buffer[0] == '[' )  /* Hit next section */
            {
               /* Add the key */
               sprintf(newBuffer, "%s=%s\n", key_name, value);
               fputs(newBuffer, tmp_file);

               /* Copy new section line */
               fputs(buffer, tmp_file);

               /* Break out of this section's while-loop */
               break;   /* section while loop */
            }
            else /* Haven't found key yet; still in section. */
            {
               /* Copy line to temp file */
               fputs( buffer, tmp_file );
            }
         }

         /* end of file was reached and key not found in section */
         if(feof(src_file))
         {
            /* Add key and value */
            sprintf(newBuffer, "%s=%s\n", key_name, value);
            fputs(newBuffer, tmp_file);
         }
      }
      else /* Uninteresting line */
      {
         if( ! feof(src_file) )
         {
            /* Copy the line to the temp file */
            fputs(buffer, tmp_file);
         }
      }
   }

   /* end of file was reached and section was not found */
   if( !found_sect )
   {
      /* Add section */
      sprintf(newBuffer, "[%s]\n", section_name);
      fputs(newBuffer, tmp_file);

      /* Add key and value */
      sprintf(newBuffer, "%s=%s\n", key_name, value);
      fputs(newBuffer, tmp_file);
   }

   fclose( src_file );
   fclose( tmp_file );
   rename( tmpFilename, file_name );
   return 1;
}
#endif

