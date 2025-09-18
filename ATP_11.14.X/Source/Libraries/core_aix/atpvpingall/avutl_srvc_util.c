/****************************************************************************************

   Module: srvc_util.c

   Title: Ascendent Services Manager

   Description: This module contains the utility functions for the Ascendent
                Service Manager.

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Service\srvc_util.c  $
   
      Rev 1.4   Oct 22 1999 11:43:58   MSALEH
   Priority based message retrieval
   
      Rev 1.3   Oct 08 1999 11:17:50   MSALEH
   shutdown XIPC before restarting it
   
      Rev 1.2   Oct 06 1999 12:01:42   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.1   Oct 01 1999 15:36:14   MSALEH
   AIX Modifications
   
      Rev 1.0   Aug 26 1999 15:25:46   MSALEH
   corrected startup/ping bugs
   
      Rev 1.10   Aug 26 1999 11:38:16   MSALEH
   mods to :
   1) shutdown only specific services to force a failover
   2) add single server mode, in which the service
   manager restarts an application if it exceeds
   MaxPingRetries
   
      Rev 1.9   Mar 05 1999 10:39:32   MSALEH
   the startflag entries are included in the tf.ini
   
      Rev 1.8   Mar 02 1999 16:37:00   MSALEH
   When shutting down the system, go through
   the reverse order of starting it.
   
      Rev 1.7   Dec 04 1998 09:53:14   MSALEH
   increased the max_services to 100
   
      Rev 1.6   Nov 19 1998 15:30:48   MSALEH
   added another parameter to GetAscendentFailoverInfo
   to control receive timeout
   
      Rev 1.5   Nov 18 1998 10:12:22   MSALEH
   Move all GetAscendent... calls to pte.lib
   
   
      Rev 1.4   Nov 12 1998 16:34:48   MSALEH
   Split the pteipc_send_receive into
   pteipc_send and pteipc_receive with the timeout
   value the same as the PingInterval from the 
   registry.
   
      Rev 1.3   Sep 09 1998 15:37:24   MSALEH
   added functionality top srvcmgr
   to ping all registered applications
   and to shutdown ascendent if the number
   of failed pings per process reached
   the registry var MaxPingRetries.
   The interval between pings is controlled
   by the registry var FailoverPingInterval, to 
   disable this functionalty, set this var to 0
   
      Rev 1.2   Sep 04 1998 18:26:00   skatuka2
   1. Changed 'Pinnacle' to 'Ascendent'
   
      Rev 1.1   27 May 1998 15:57:34   rmalathk
   Initial Revision with PVCS Header
   
*****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

#include "pteipc.h"
#include "ntutils.h"
#include "srvcmgr.h"
#include "pteipc.h"

// We want the printf's to print information - this is a utility.
#ifdef printf
#undef printf
//#define printf printf
#endif

#define STARTUP_FILE    "startup.ini"
#define STRTFLAG_FILE   "tf.ini"

#ifndef WIN32

#define PROCESS_NOT_FOUND 3
#define ORACLE_HOME       "ORACLE_HOME"
#define START_DIR         "/bin/dbstart"
#define LISTENER_DIR      "/bin/tnslsnr"
#define XIPCROOT          "XIPCROOT"
#define XIPC_INIT         "/bin/xipcinit"
#define XIPC_STOP         "/bin/xipcstop"
#define XIPC_TERM         "/bin/xipcterm"
#define XIPC              "XIPC"

extern int errno;
extern char *sys_errlist[];

/* *************************************************************************** */
/* *************************************************************************** */
INT dependency_start()
{
    CHAR buffer[256];
    pCHAR ora_home;

    printf( "Starting Oracle Enterprise Server....\n" );
    ora_home = getenv( ORACLE_HOME );
    strcpy( buffer, ora_home );
    strcat( buffer, START_DIR );
    system( buffer );
    pteipc_sleep( 10000 );

    printf( "Starting the Oracle TNS Listener....\n" );
    ora_home = getenv( ORACLE_HOME );
    strcpy( buffer, ora_home );
    strcat( buffer, LISTENER_DIR );
    system( buffer );
    pteipc_sleep( 1000 );
}

/***************************************************************************
***************************************************************************/
INT xipc_start( pCHAR xipc_instance )
{
        pCHAR xipc_root, xipc_cfgfile;
        CHAR  buffer[256];
        LONG  ret;

        xipc_root = getenv(XIPCROOT );

        sprintf( buffer, "%s%s", xipc_root, XIPC_STOP);
        printf( "\nStopping XIPC environment....\n" );
        system( buffer );

        pteipc_sleep(7000);

        sprintf( buffer, "%s%s", xipc_root, XIPC_TERM);
        printf( "\nTerminating XIPC environment....\n" );
        system( buffer );
   
        pteipc_sleep(7000);

        sprintf( buffer, "%s%s", xipc_root, XIPC_INIT);
        printf( "\nInitializing xipc environment....\n" );
        system( buffer );
        pteipc_sleep( 3000 );

        printf( "Starting XIPC instance...\n" );
        xipc_cfgfile = getenv( XIPC );
        strcpy( buffer, xipc_cfgfile );

        if( ( ret = XipcStart( buffer, xipc_instance,
              XIPC_START_NETWORK ) ) >= 0  )
        {
           printf( "XIPC instance %s started\n", xipc_instance );
           sprintf( buffer, "XIPC instance %s started", xipc_instance );
           LogEvent( buffer, INFO_MSG );
        }
        else
        {
           printf( "xipc instance %s failed to start\n", xipc_instance );
           sprintf( buffer, "failed to start XIPC instance %s", xipc_instance );
           LogEvent( buffer, ERROR_MSG );
           return( FALSE );
        }

        pteipc_sleep( 3000 );
        return( TRUE );
}

#else

#define DEPENDENCY_KEY "Software\\Hypercom\\Ascendent\\AscendentServiceManager\\Dependencies"

/* *************************************************************************** */
/* *************************************************************************** */
INT dependency_start()
{
	HKEY           hKey;
  	DWORD          Type, Size = 30;
	LONG           retcode;
	BYTE           bData [30];
	HANDLE         hScm, hService;
	char           ValueName[20];
	char           ServiceName[30];
	int            temp = 0;
	char           Buffer[256];
	SERVICE_STATUS ServiceStatus;

    /* get list of dependency services from registry */
	retcode = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				DEPENDENCY_KEY,
				0,
				KEY_QUERY_VALUE,
				&hKey );

	if( retcode != ERROR_SUCCESS )
	{
		printf( "Unable to open Dependency key. Registry not Initialized for service manager\n" );
		LogEvent( "Unable to open Dependency key. Registry not Initialized for service manager", ERROR_MSG );
		return 0;
	}

	/* open the handle to service control manager */
	printf( "\nQuerying Services....\n" );
	hScm = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hScm )
	{
		printf( "Unable to establish connection with the Service Control Manager\n" );
		LogEvent( "Unable to establish connection with the Service Control Manager", ERROR_MSG );
		return 0;
	}

	retcode = ERROR_SUCCESS;
	while( retcode == ERROR_SUCCESS )
	{
		/* verify that all dependency services specified in registry are running */
		temp++;

		wsprintf( ValueName, "Dependency-%d", temp );
		Size = 30;
		retcode = RegQueryValueEx( hKey, ValueName, NULL, &Type, bData, &Size );

		if( retcode == ERROR_SUCCESS )
		{
			/* get name of service to query */
			lstrcpy( ServiceName, (char *)bData );

			/* now check if service is running */
			hService = OpenService( hScm, ServiceName, SERVICE_ALL_ACCESS );
			if( !hService )
			{
				printf( "Unable to open service handle to %s\n", ServiceName );
				sprintf( Buffer, "Unable to open service handle to %s", ServiceName );
				LogEvent( Buffer, ERROR_MSG );
				continue;
			}

			if( !QueryServiceStatus( hService, &ServiceStatus ) )
			{
				printf( "Unable to query service %s\n", ServiceName );
				sprintf( Buffer, "Unable to query service %s", ServiceName );
				LogEvent( Buffer, ERROR_MSG );
				continue;
			}

			if( ServiceStatus.dwCurrentState == SERVICE_RUNNING )
				printf( "Service %s is running\n", ServiceName );
			else
			{
				/* dependency service not running. Try to start it */
				printf( "Service %s is not running\n", ServiceName );
				printf( "Attempting to start Service %s...........\n", ServiceName );
				if( StartService( hService, 0, NULL ) )
				{
                                        Sleep(10000);
					QueryServiceStatus( hService, &ServiceStatus );
					if( ServiceStatus.dwCurrentState == SERVICE_RUNNING )
						printf( "OK\n" );
					else
					{
						/* Dependency service won't run. What do we do??????? */
						/* for now just log an event */
						printf( "Failed\n" );
						sprintf( Buffer, "Unable to start service %s", ServiceName );
						LogEvent( Buffer, ERROR_MSG );
					}
				}
				else
				{
					printf( "Failed\n" );
					sprintf( Buffer, "Unable to start service %s", ServiceName );
					LogEvent( Buffer, ERROR_MSG );
				}
			}

			CloseServiceHandle( hService );
			
		}
	}
	CloseServiceHandle( hScm );
	RegCloseKey( hKey );
    return 1;
}

/* *************************************************************************** */
/* *************************************************************************** */
INT xipc_start( pCHAR xipc_instance )
{
   INT   ret;
   CHAR  XipcInstanceFile[100]; 
   CHAR  XipcRoot[100];
   CHAR  Buffer[100];

   
   GetXipcRoot(XipcRoot);
   sprintf(Buffer, "%s\\bin\\xipcstop.exe", XipcRoot);
   printf( "\nStopping XIPC environment....\n" );
   WinExec( Buffer, SW_HIDE );

   pteipc_sleep(7000);

   sprintf(Buffer, "%s\\bin\\xipcterm.exe", XipcRoot);
   printf( "\nTerminating XIPC environment....\n" );
   WinExec( Buffer, SW_HIDE );
   
   pteipc_sleep(7000);

   sprintf(Buffer, "%s\\bin\\xipcinit.exe", XipcRoot);
   printf( "\nInitializing xipc environment....\n" );
   WinExec( Buffer, SW_HIDE );
	
   pteipc_sleep(7000);

   GetXipcInstanceFile(XipcInstanceFile);
			
   printf( "Starting XIPC instance...\n" );
   if( ( ret = XipcStart( XipcInstanceFile, xipc_instance, XIPC_START_NETWORK ) ) >= 0  )
   {
      printf( "XIPC instance %s started\n", xipc_instance );
      sprintf( Buffer, "XIPC instance %s started", xipc_instance );
      LogEvent( Buffer, INFO_MSG );
      return 1;
   }
   else
   {
      printf( "xipc instance %s failed to start\n", xipc_instance );
      sprintf( Buffer, "failed to start XIPC instance %s", xipc_instance );
      LogEvent( Buffer, ERROR_MSG );
      return 0;
   }
}

#endif

/* ************************************************************************** */
/* ************************************************************************** */
void parse_input_line( pCHAR buffer, pSERVICE_INFO service_info )
{
     pCHAR chrptr, curptr;

     chrptr = curptr = buffer;
     
     /*  first get the exe name */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     strncpy( service_info->exe_name, curptr, sizeof(service_info->exe_name) );
     chrptr++;
     curptr = chrptr;

     /*  get the service name */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     strncpy( service_info->service_name, curptr, sizeof(service_info->service_name) );
     chrptr++;
     curptr = chrptr;

     /*  get the display name */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     strncpy( service_info->display_name, curptr, sizeof(service_info->display_name) );
     chrptr++;
     curptr = chrptr;

     /*  get the control que  name */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     strncpy( service_info->que_name, curptr, sizeof(service_info->que_name) );
     chrptr++;
     curptr = chrptr;

     /*  extract the disable flag */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     service_info->disable = *curptr;
     chrptr++;
     curptr = chrptr;

	  /*  extract the running flag */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     service_info->running = *curptr;
     chrptr++;
     curptr = chrptr;

	  /*  if service is running, extract the trace and pid */
	  if( service_info->running == '1' )
	  {
         chrptr = strchr(curptr, ':');
         if (chrptr == NULL_PTR)
            return;
         
         *chrptr = NULL_PTR;
         service_info->trace_on = *curptr;
         chrptr++;
         curptr = chrptr;

         chrptr = strchr(curptr, ':');
         if (chrptr == NULL_PTR)
            return;
         
         *chrptr = NULL_PTR;
         strncpy( service_info->pid, curptr, sizeof(service_info->pid) );
         chrptr++;
         curptr = chrptr;
	  }
	  else
	  {
	      service_info->trace_on = '0';
         chrptr = strchr(curptr, ':');
         if (chrptr == NULL_PTR)
            return;
         
         *chrptr = NULL_PTR;
         chrptr++;
         curptr = chrptr;

	      strcpy( service_info->pid, "xxxx" );
         chrptr = strchr(curptr, ':');
         if (chrptr == NULL_PTR)
            return;
         
         *chrptr = NULL_PTR;
         chrptr++;
         curptr = chrptr;
	  }

     /* Extract the shutdown flag */
     chrptr = strchr(curptr, ':');
     if (chrptr == NULL_PTR)
        return;

     *chrptr = NULL_PTR;
     service_info->shutdown = *curptr;

     return; 
}


/* ************************************************************************** */
/* ************************************************************************** */
void get_startup_flags( pINT pin_flag, pINT xipc_flag, pINT dep_flag )
{
	CHAR temp_str[100];
	CHAR flag_filename[256];

   GetAscendentConfigDirectory( flag_filename );
   strcat( flag_filename, STRTFLAG_FILE );

	GetPrivateProfileString( "STARTUP",
							   "DEPENDENCIES",
							   "0",
							   temp_str,
							   sizeof( temp_str ),
							   flag_filename );
	*dep_flag = atoi( temp_str );

	GetPrivateProfileString( "STARTUP",
							   "IPC",
							   "1",
							   temp_str,
							   sizeof( temp_str ),
							   flag_filename );
	*xipc_flag = atoi( temp_str );

	GetPrivateProfileString( "STARTUP",
							   "SERVICES",
							   "1",
							   temp_str,
							   sizeof( temp_str ),
							   flag_filename );
	*pin_flag = atoi( temp_str );
}




/* ************************************************************************** */
/* ************************************************************************** */
INT get_services_table( SERVICE_INFO serv_table[], pINT num_services )
{
    FILE *srvc_file;
    CHAR  srvc_filename[256]; 
    CHAR  buffer[256], err_msg[256];

    /*  try to open the Ascendent services file.  */
    GetAscendentConfigDirectory( srvc_filename );
    strcat( srvc_filename, STARTUP_FILE );
    srvc_file = fopen( srvc_filename, "r+" );

    if( srvc_file == NULL )
    {
        sprintf( err_msg, "Error opening Ascendent services file: %d-%s",
                          errno, sys_errlist[errno] );
        printf( "%s\n", buffer );
        LogEvent( err_msg, ERROR_MSG );
        return 0;
    } 

    /*  file is open, read in one line at a time....   */
    while( *num_services < MAX_SERVICES )
    {
      fgets( buffer, sizeof( buffer ), srvc_file );
      if( feof( srvc_file ) )
         break;

      if( ferror( srvc_file ) )
      {
         sprintf( err_msg, "Error reading Ascendent startup file: %d-%s",
                          errno, sys_errlist[errno] );
         printf( "%s\n", buffer );
         LogEvent( err_msg, ERROR_MSG );
         return 0;
      }

		/*  ignore all comment lines, lines starting with a # */
		if( buffer[0] == '#' )
		   continue;

      /*  ....and parse the line into a structure... */
      parse_input_line( buffer, &serv_table [*num_services] );

      /*  keep track of the number of services processed. */
      (*num_services)++;
    }

    /*  we have reached end of file. All services have been processed.  */
    fclose( srvc_file );
    return 1;
}



/* ************************************************************************** */
/* ************************************************************************** */
void save_services_table( SERVICE_INFO serv_table[], INT num_services )
{
   FILE  *srvc_file;
   CHAR  srvc_filename[256]; 
   CHAR  buffer[256], err_msg[256];
	INT   temp;

    /*  try to open the Ascendent services file.  */
    GetAscendentConfigDirectory( srvc_filename );
    strcat( srvc_filename, STARTUP_FILE );
    srvc_file = fopen( srvc_filename, "w+" );

    if( srvc_file == NULL )
    {
        sprintf( err_msg, "Error opening Ascendent services file: %d-%s",
                          errno, sys_errlist[errno] );
        printf( "%s\n", buffer );
        LogEvent( err_msg, ERROR_MSG );
        return;
    } 

	for( temp = 0; temp < num_services; temp++ )
	{
		/*  write each service info to the file */
		sprintf( buffer, "%s:%s:%s:%s:%c:%c:%c:%s:%c:    \n", 
				 serv_table[temp].exe_name,
				 serv_table[temp].service_name,
				 serv_table[temp].display_name,
				 serv_table[temp].que_name,
				 serv_table[temp].disable,
				 serv_table[temp].running,
				 serv_table[temp].trace_on,
			    serv_table[temp].pid,
             serv_table[temp].shutdown);

		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), srvc_file );
	}

	/*  close the services file and return */
	fclose( srvc_file );
	return;
}



/* ************************************************************************** */
/* ************************************************************************** */
void save_service_info( pSERVICE_INFO service_info, INT line_num )
{
    FILE *srvc_file;
    CHAR  srvc_filename[256]; 
    CHAR  buffer[256], err_msg[256];
	INT   temp, num_bytes;

    /*  try to open the Ascendent services file.  */
    GetAscendentConfigDirectory( srvc_filename );
    strcat( srvc_filename, STARTUP_FILE );
    srvc_file = fopen( srvc_filename, "r+" );

    if( srvc_file == NULL )
    {
        sprintf( err_msg, "Error opening Ascendent services file: %d-%s",
                          errno, sys_errlist[errno] );
        printf( "%s\n", buffer );
        LogEvent( err_msg, ERROR_MSG );
        return;
    } 

	/*  advance to the line containing the required service */
	temp = 0;  num_bytes = 0;
	while( temp < line_num )
	{
		memset( buffer, 0, sizeof( buffer ) );
        fgets( buffer, sizeof( buffer ), srvc_file );
		temp++; 
		num_bytes = num_bytes + strlen( buffer );
	}
	fseek( srvc_file, num_bytes, SEEK_SET );

	/*  modify the line with the new data */
	memset( buffer, 0, sizeof( buffer ) );
	sprintf( buffer, "%s:%s:%s:%s:%c:%c:%c:%s:%c:    ", 
				 service_info->exe_name,
				 service_info->service_name,
				 service_info->display_name,
				 service_info->que_name,
				 service_info->disable,
				 service_info->running,
				 service_info->trace_on,
			    service_info->pid,
             service_info->shutdown);

	fwrite( buffer, sizeof( CHAR ), strlen( buffer ), srvc_file );

	/*  close the file and return */
	fclose( srvc_file );
	return;
}

#ifdef WIN32
/* ************************************************************************** */
/* ************************************************************************** */
BYTE start_service( pSERVICE_INFO service_info )
{
   CHAR buffer[256];
	BYTE rcode;
   DWORD err_code;
	HANDLE hScm, hService;
	SERVICE_STATUS ServiceStatus;

   /*  if the service is disabled, simply return */
   if( service_info->disable == '1' )
       return( PTEMSG_SRVC_DISABLED );

   /*  if the service is already running, simply return */
   if( service_info->running == '1' )
       return( PTEMSG_SRVC_RUNNING );

   /* open the handle to service control manager */
	hScm = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hScm )
	{
      err_code = GetLastError();
      sprintf( buffer, "Error opening NT Service Control Database - %d", err_code );
		printf( "%s\n", buffer );
		LogEvent( buffer, ERROR_MSG );
		return( PTEMSG_SRVC_STARTFAILED );
	}

	/*  open handle to service */
	hService = OpenService( hScm, service_info->service_name, SERVICE_ALL_ACCESS );
	if( hService == NULL )
	{
      err_code = GetLastError();
      sprintf( buffer, "Unable to open service handle to %s - %d",
               service_info->service_name, err_code );
		printf( "%s\n", buffer );
		LogEvent( buffer, ERROR_MSG );
	   CloseServiceHandle( hScm );
		return( PTEMSG_SRVC_STARTFAILED );
	}

   /*  try to start service */
   printf( "Attempting to start %s.......\n", service_info->service_name );
   if( StartService( hService, 0, NULL ) )
	{
      pteipc_sleep(5000);

		QueryServiceStatus( hService, &ServiceStatus );
		if( ServiceStatus.dwCurrentState == SERVICE_RUNNING )
      {
	    service_info->running = '1';
       service_info->ping_count = 0;
		 printf( "OK\n" );
       rcode = PTEMSG_OK;
      }
		else
		{
		 printf( "Failed\n" );
		 sprintf( buffer, "Unable to verify that service %s started",
                  service_info->service_name );
		 LogEvent( buffer, ERROR_MSG );
		 rcode = PTEMSG_SRVC_STARTFAILED;
		}
	}
	else
	{
      err_code = GetLastError();
		printf( "Failed\n" );
		sprintf( buffer, "Error starting service %s - %d",
                 service_info->service_name, err_code );
		LogEvent( buffer, ERROR_MSG );
		rcode = PTEMSG_SRVC_STARTFAILED;
	}

	CloseServiceHandle( hService );			
	CloseServiceHandle( hScm );
   return( rcode );
} 


/* ************************************************************************** */
/* ************************************************************************** */
BYTE stop_service( pSERVICE_INFO service_info )
{
   CHAR buffer[256];
	BYTE rcode;
   DWORD err_code;
	HANDLE hScm, hService;
	SERVICE_STATUS ServiceStatus;

    /*  if the service is already stopped, simply return */
    if( service_info->running == '0' )
        return( PTEMSG_SRVC_STOPPED );

    /* open the handle to service control manager */
	hScm = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hScm )
	{
      err_code = GetLastError();
      sprintf( buffer, "Error opening NT Service Control Database - %d", err_code );
		printf( "%s\n", buffer );
		LogEvent( buffer, ERROR_MSG );
		return( PTEMSG_SRVC_STOPFAILED );
	}

	/*  open handle to service */
	hService = OpenService( hScm, service_info->service_name, SERVICE_ALL_ACCESS );
	if( hService == NULL )
	{
      err_code = GetLastError();
      sprintf( buffer, "Unable to open service handle to %s - %d",
                 service_info->service_name, err_code );
		printf( "%s\n", buffer );
		LogEvent( buffer, ERROR_MSG );
	   CloseServiceHandle( hScm );
		return( PTEMSG_SRVC_STOPFAILED );
	}

   printf( "Attempting to stop %s........\n", service_info->service_name );
	if( !ControlService( hService, SERVICE_CONTROL_STOP, &ServiceStatus ) )
	{
      err_code = GetLastError();
      if( err_code == ERROR_SERVICE_NOT_ACTIVE )
		{
			/*  the process must have terminated unexpectedly */
         printf( "service already stopped\n" );
			service_info->running = '0';
			service_info->trace_on = '0';
			strcpy( service_info->pid, "xxxx" );
         service_info->ping_count = 0;
			rcode = PTEMSG_OK;
		}
      else
      {
         printf( "Failed\n" );
         sprintf( buffer, "Error stopping service %s - %d",
                     service_info->service_name, err_code );
		   printf( "%s\n", buffer );
		   LogEvent( buffer, ERROR_MSG );
		   rcode = PTEMSG_SRVC_STOPFAILED;
      }
	}
   else
   {
      printf( "OK\n" );
		service_info->running = '0';
		service_info->trace_on = '0';
		strcpy( service_info->pid, "xxxx" );
      service_info->ping_count = 0;
      rcode = PTEMSG_OK;
   }

	CloseServiceHandle( hService );			
	CloseServiceHandle( hScm );
   return( rcode );
}

#else

/* ************************************************************************** */
/* ************************************************************************** */
BYTE start_service( pSERVICE_INFO service_info )
{
    CHAR buffer[256], err_msg[256];
    INT tmp_pid;

    /* if the service is disabled, simply return */
    if( service_info->disable == '1' )
      return( PTEMSG_SRVC_DISABLED ) ;

    /* if the service is already running, simply return */
    if( service_info->running == '1' )
      return( PTEMSG_SRVC_RUNNING ) ;

    printf( "Attempting to start %s\n", service_info->service_name );

    /* first try forking a child process, store the process ID */
    tmp_pid = fork();
    if( tmp_pid == -1 )
    {
       sprintf( err_msg, "Error forking Ascendent process %s: %d-%s",
                 service_info->service_name, errno, sys_errlist[errno] );
       printf( "%s\n", buffer );
       LogEvent( err_msg, ERROR_MSG );
       return( PTEMSG_SRVC_STARTFAILED );
    }

    /* fork succeeded, now determine if parent or child */
    if( tmp_pid == 0 )
    {
       /* child process, exec the desired application */
       GetAscendentBinDirectory( buffer );
       strcat( buffer, service_info->exe_name );

       execl( buffer, buffer, service_info->service_name,
               (char *)0 );
       /* If we get here, the exec failed! */
       sprintf( err_msg, "Error exec'ing Ascendent process %s: %d-%s",
                 service_info->service_name, errno, sys_errlist[errno] );
       printf( "%s\n", buffer );
       LogEvent( err_msg, ERROR_MSG );
       exit(-1);
    }
    else
    {
       /* parent process, store the pid */
       itoa( tmp_pid, service_info->pid, 10 );
       service_info->running = '1';
       return( PTEMSG_OK );
    }
}

/************************************************************************
 * extremePrejudice():
 * A thread-starting function that hangs-out for 10 seconds
 * and then sends a SIGKILL.  Just to make sure it's dead.
 * Should be passed a pointer to int.  The int should be the
 * process id you want KILLed.
 ************************************************************************/
void *extremePrejudice(void *arg)
{
   int pid;

   pid = *(int*)arg;
   pteipc_sleep(10000);
   kill(pid, SIGKILL);
   pthread_exit(NULL);
}

/* ************************************************************************** */
/* ************************************************************************** */
BYTE stop_service( pSERVICE_INFO service_info )
{
    pthread_t killthread;

    CHAR buffer[256], err_msg[256];
    INT rcode, pid;

    /* if the service is already stopped, simply return */
    if( service_info->running == '0' )
        return( PTEMSG_SRVC_STOPPED ) ;

    printf( "Attempting to stop %s\n", service_info->service_name );

    /* now try to stop the service. */
    pid = atoi( service_info->pid );
    rcode = kill( pid, SIGTERM );

    /* Make sure it dies: */
    /*pthread_create(killthread, NULL, extremePrejudice,  (void*)&pid);*/

    if( rcode == -1 )
    {
        sprintf( err_msg, "error stopping Ascendent process %s: %d-%s",
                 service_info->service_name, errno, sys_errlist[errno] );
        printf( "%s\n", buffer );
        LogEvent( err_msg, ERROR_MSG );
        if( errno == PROCESS_NOT_FOUND )
        {
                /* the process must have terminated unexpectedly */
                service_info->running = '0';
                service_info->trace_on = '0';
                strcpy( service_info->pid, "xxxx" );
                return( PTEMSG_OK );
        }
        else
        return( PTEMSG_SRVC_STOPFAILED );
    }
    else
    {
        service_info->running = '0';
        service_info->trace_on = '0';
        strcpy( service_info->pid, "xxxx" );
        return( PTEMSG_OK );
    }
}

#endif

/* ************************************************************************** */
/* ************************************************************************** */
BYTE ping_service( pSERVICE_INFO service_info )
{
   pPTE_MSG  p_msg_out, p_msg_in;
	LONG      rcode, Qid;
   CHAR      buffer[256], tmpstr[256];

   printf( "Attempting to ping %s........\n", service_info->service_name );

   p_msg_out = NULL_PTR;
   p_msg_in  = NULL_PTR;

   /*  build the outgoing message */
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
			         ST1_SYS_PING,
			         0,
			         service_info->que_name,
				      interactive_que_name,
				      NULL_PTR,
				      0,
				      0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( buffer, "Insufficient memory to build request message" );
		LogEvent( buffer, ERROR_MSG );
                printf("Failed\n");
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   /* Make sure there are no left over messages from a previous ping */
   Qid = pteipc_queaccess(interactive_que_name);
   if (Qid >= 0)
      pteipc_quepurge(Qid);

   rcode = pteipc_send_hp( p_msg_out, service_info->que_name, SystemMessagePriority );
   free( p_msg_out );

   if( rcode >= 0 )
   {
      /*  Send Succeeded */
      /*  try to receive a response */
#ifdef WIN32
      p_msg_in = pteipc_receive( interactive_que_name, NULL_PTR, (SHORT)FailoverReceiveTimeout, &rcode );
#else
      /* in multi threaded applications, on unix, XIPC does not support receive with timeouts */
      pteipc_sleep(FailoverReceiveTimeout*1000);
      p_msg_in = pteipc_receive( interactive_que_name, NULL_PTR, (SHORT)PTEIPC_NOWAIT, &rcode);
#endif

      if( p_msg_in != NULL_PTR )
      {     
         free( p_msg_in );
         printf("OK\n");
	      return PTEMSG_OK;    
      }
      else
      {
         sprintf( buffer, "Communication Error during request to %s", 
                                 service_info->service_name );
	      LogEvent( buffer, ERROR_MSG );
         pteipc_get_errormsg(rcode, buffer);
         LogEvent(buffer, ERROR_MSG);
         printf("Failed\n");
         return PTEMSG_IPC_ERROR;
      }
   }
   else
   {  
      /*  attempt to send response failed. Log the error */
      pteipc_get_errormsg( rcode, tmpstr );
      sprintf( buffer, "Error sending response to %s - %s", service_info->que_name, tmpstr );
      printf( "%s\n", buffer );
      LogEvent( buffer, ERROR_MSG );
      printf("Failed\n");
      return PTEMSG_IPC_ERROR;
   }
}

/* ************************************************************************** */
/* ************************************************************************** */
BYTE query_service( pSERVICE_INFO service_info )
{
	return( PTEMSG_OK );
}



/* ************************************************************************** */
/* ************************************************************************** */
BYTE enable_service( pSERVICE_INFO service_info, BYTE flag )
{

	/*  if service is running, do nothing */
	if( service_info->running == '1' )
        return( PTEMSG_SRVC_RUNNING );

	/*  check the flag and enable/disable as required */
	if( flag == ST1_SRVC_ENABLE )
		service_info->disable = '0';
	else
		service_info->disable = '1';

	return( PTEMSG_OK );
}


/* ************************************************************************** */
/* ************************************************************************** */
BYTE trace_service( pSERVICE_INFO service_info, BYTE flag )
{
	pPTE_MSG p_msg_out, p_msg_in;
	BYTE trace_flag, result;
	LONG rcode;
	CHAR err_msg[100], buffer[256];
	CHAR trace_on;

	/*  build a trace message to the desired service */
	if( flag == ST1_SRVC_TRACEON )
	{   trace_on = '1';
		trace_flag = ST1_SYS_IPC_TRACE_ON;
	}
	else
	{
		trace_on = '0';
		trace_flag = ST1_SYS_IPC_TRACE_OFF;
	}

        p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
				  trace_flag,
				  0,
				  NULL_PTR,
				  interactive_que_name,
				  NULL_PTR,
				  0,
				  0 );

	if( p_msg_out == NULL_PTR )
	{
	   sprintf( buffer, "Unable to build trace message to %s", 
				 service_info->service_name );
       	   printf( "%s\n", buffer );
		LogEvent( buffer, ERROR_MSG );
	   return PTEMSG_INSUFFICIENT_MEMORY;
	}

	/*  try to send the msg and get a response */
	p_msg_in = pteipc_send_receive( p_msg_out, service_info->que_name,
						interactive_que_name, &rcode );
	if( p_msg_in == NULL_PTR )
	{
	   pteipc_get_errormsg( rcode, err_msg );
	   sprintf( buffer, "Communication error during trace request to %s:%d-%s:", service_info->service_name, rcode, err_msg );
	   return PTEMSG_IPC_ERROR;
	}

	/*  get the result code from the response */
	result = ptemsg_get_result_code( p_msg_in );

	if( result == PTEMSG_OK )
	   service_info->trace_on = trace_on;

	free( p_msg_in );
	return result;
}


