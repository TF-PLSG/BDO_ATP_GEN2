/****************************************************************************************

   Module: srvcmgr.c

   Title: Ascendent Services Manager

   Description: This is the common header file for Service Manager and Service Monitor.

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Common\SRVCINFO.H  $
   
      Rev 1.7   Aug 27 1999 16:35:18   MSALEH
   1) dd new function to get PIDs from processes
   and add them to startup.ini file
   2) correct instartupmode functionality
   
      Rev 1.6   Aug 26 1999 15:25:36   MSALEH
   corrected startup/ping bugs
   
      Rev 1.5   Aug 26 1999 11:38:12   MSALEH
   mods to :
   1) shutdown only specific services to force a failover
   2) add single server mode, in which the service
   manager restarts an application if it exceeds
   MaxPingRetries
   
      Rev 1.4   Dec 04 1998 09:53:10   MSALEH
   increased the max_services to 100
   
      Rev 1.3   Nov 17 1998 12:12:30   MSALEH
   Added new functionality to manager
   logger/reply Primary/secondary designation,
   it works with logger usiong special message
   subtypes under MT_SYSTEM_REQUEST
   
      Rev 1.2   Sep 09 1998 15:37:12   MSALEH
   added functionality top srvcmgr
   to ping all registered applications
   and to shutdown ascendent if the number
   of failed pings per process reached
   the registry var MaxPingRetries.
   The interval between pings is controlled
   by the registry var FailoverPingInterval, to 
   disable this functionalty, set this var to 0
   
      Rev 1.1   Sep 04 1998 13:07:24   skatuka2
   1. changed 'pinnacle' to 'ascendent'
   
      Rev 1.0   27 May 1998 15:59:10   rmalathk
   Initial Revision
   
*****************************************************************************************/
#ifndef __SRVCINFO__
#define __SRVCINFO__

#include "basictyp.h"


#define MAX_SERVICES 150 

typedef struct service_inf
{
    CHAR exe_name[MAX_APP_NAME_SIZE];
    CHAR service_name[MAX_APP_NAME_SIZE];
    CHAR display_name[51];
    CHAR que_name[MAX_QUE_NAME_SIZE];
    BYTE disable;
    BYTE running;
    BYTE trace_on;
    CHAR pid[12];
    BYTE shutdown;
    BYTE ping_count;
	BYTE  encryption_on;
	BYTE  mra_on;
	BYTE  falcon_on;
} SERVICE_INFO, *pSERVICE_INFO;

#endif

