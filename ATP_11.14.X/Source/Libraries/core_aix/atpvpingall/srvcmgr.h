/****************************************************************************************

   Module: srvcmgr.c

   Title: Pinnacle Services Manager

   Description:

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Service\SRVCMGR.H  $
   
      Rev 1.6   Oct 22 1999 11:43:58   MSALEH
   Priority based message retrieval
   
      Rev 1.5   Oct 01 1999 15:38:20   MSALEH
   AIX Modifications
   
      Rev 1.4   Nov 19 1998 15:30:48   MSALEH
   added another parameter to GetAscendentFailoverInfo
   to control receive timeout
   
      Rev 1.3   Nov 12 1998 16:34:46   MSALEH
   Split the pteipc_send_receive into
   pteipc_send and pteipc_receive with the timeout
   value the same as the PingInterval from the 
   registry.
   
      Rev 1.2   Sep 09 1998 15:37:24   MSALEH
   added functionality top srvcmgr
   to ping all registered applications
   and to shutdown ascendent if the number
   of failed pings per process reached
   the registry var MaxPingRetries.
   The interval between pings is controlled
   by the registry var FailoverPingInterval, to 
   disable this functionalty, set this var to 0
   
      Rev 1.1   27 May 1998 15:57:36   rmalathk
   Initial Revision with PVCS Header
   
*****************************************************************************************/
#ifndef __SRVCMGR__
#define __SRVCMGR__

#include "basictyp.h"
#include "srvcinfo.h"

extern LONG FailoverPingInterval;
extern INT  FailoverReceiveTimeout;
extern INT  SystemMessagePriority;

/* prototype functions */
INT   xipc_start        ( pCHAR xipc_instance );
INT   dependency_start  ();
BYTE  query_service     ( pSERVICE_INFO service_info );
BYTE  start_service     ( pSERVICE_INFO service_info );
BYTE  stop_service      ( pSERVICE_INFO service_info );
BYTE  ping_service      ( pSERVICE_INFO service_info );
BYTE  enable_service    ( pSERVICE_INFO service_info, BYTE flag );
BYTE  trace_service     ( pSERVICE_INFO service_info, BYTE flag );
void  save_service_info ( pSERVICE_INFO service_info, INT line_num );
INT   get_services_table( SERVICE_INFO serv_table[], pINT num_services );
void  save_services_table( SERVICE_INFO serv_table[], INT num_services );
void  get_startup_flags ( pINT pin_flag, pINT xipc_flag, pINT dep_flag );
BOOLEAN get_failover_ping_interval( pLONG ping_interval, pINT max_retries );
void  *ping_spy         ( void * dummy );
void  ping_thread_start ();
void  ping_all_services ();

#endif
