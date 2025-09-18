/****************************************************************************************

   Module: srvcmgr.c

   Title: Ascendent Services Manager

   Description: This module contains the platform independent code for the Ascendent
                Service Manager.

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Service\SRVCMGR.C  $
   
      Rev 1.23   Jan 18 2000 12:09:58   MSALEH
   call pteipc_receive_r() on Unix
   to prevent blocking forever
   
      Rev 1.22   Oct 22 1999 11:43:56   MSALEH
   Priority based message retrieval
   
      Rev 1.21   Oct 08 1999 15:55:22   MSALEH
   if xipc is terminated, prevent srvcmgr from
   logging multiple not connected errors
   
      Rev 1.20   Oct 08 1999 11:17:48   MSALEH
   shutdown XIPC before restarting it
   
      Rev 1.19   Oct 06 1999 12:01:40   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.18   Oct 05 1999 09:54:10   MSALEH
   return a value from ping spy function
   
      Rev 1.17   Oct 01 1999 15:36:14   MSALEH
   AIX Modifications
   
      Rev 1.16   Aug 31 1999 11:09:38   MSALEH
   correct startup options
   
      Rev 1.15   Aug 30 1999 10:58:54   MSALEH
    
   
      Rev 1.14   Aug 27 1999 16:35:18   MSALEH
   1) dd new function to get PIDs from processes
   and add them to startup.ini file
   2) correct instartupmode functionality
   
      Rev 1.13   Aug 26 1999 15:25:36   MSALEH
   corrected startup/ping bugs
   
      Rev 1.12   Aug 26 1999 11:38:12   MSALEH
   mods to :
   1) shutdown only specific services to force a failover
   2) add single server mode, in which the service
   manager restarts an application if it exceeds
   MaxPingRetries
   
      Rev 1.11   Jul 15 1999 15:47:30   MSALEH
   Added enhancement to log a message to the Event Log
   whenever a Queue's current message couns meets or
   exceeds a hard-coded threshold of 15.
   
      Rev 1.10   Jun 18 1999 10:15:58   rmalathk
   When shutting down the services for failover,
   shut down in the reverse order of services.
   
      Rev 1.9   Mar 03 1999 17:15:32   MSALEH
   Correctly logout the pingspy thread
   when srvcmgr is terminated
   
      Rev 1.8   Mar 02 1999 16:36:58   MSALEH
   When shutting down the system, go through
   the reverse order of starting it.
   
      Rev 1.7   Nov 19 1998 15:30:46   MSALEH
   added another parameter to GetAscendentFailoverInfo
   to control receive timeout
   
      Rev 1.6   Nov 18 1998 10:12:18   MSALEH
   Move all GetAscendent... calls to pte.lib
   
   
      Rev 1.5   Nov 12 1998 16:34:46   MSALEH
   Split the pteipc_send_receive into
   pteipc_send and pteipc_receive with the timeout
   value the same as the PingInterval from the 
   registry.
   
      Rev 1.4   Sep 09 1998 15:37:22   MSALEH
   added functionality top srvcmgr
   to ping all registered applications
   and to shutdown ascendent if the number
   of failed pings per process reached
   the registry var MaxPingRetries.
   The interval between pings is controlled
   by the registry var FailoverPingInterval, to 
   disable this functionalty, set this var to 0
   
      Rev 1.3   Sep 04 1998 18:24:42   skatuka2
   1. Changed 'Pinnacle' to 'Ascendent'
   
      Rev 1.2   20 Aug 1998 08:59:00   rmalathk
   minor bug fixes and changes needed for applink.
   
      Rev 1.1   27 May 1998 15:57:36   rmalathk
   Initial Revision with PVCS Header
   
*****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif

#include "ntutils.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "srvcmgr.h"


extern INT EndProcessSignalled;
extern INT MainProcessDone;
extern CHAR ServiceName[];
CHAR srvcmgr_Logging_Filename[256];
SERVICE_INFO services_table[MAX_SERVICES];
SERVICE_INFO services_table_network[MAX_NETWORK_SERVICES];
SERVICE_INFO services_table_non_network[MAX_NON_NETWORK_SERVICES];

#define INFO_ERROR                  "INFO"
#define WARNING_ERROR               "WARNING"
#define FATAL_ERROR                 "FATAL"


INT depstart_flag;
INT xipcstart_flag;
INT pinstart_flag;
INT num_services;
INT num_network_services;
INT num_non_network_services;

INT   volatile TakeAscendentDown = FALSE;
INT   volatile InRestartApplicationMode = FALSE;

LONG  FailoverPingInterval;
INT   MaxPingRetries;
INT   FailoverReceiveTimeout;
INT   SystemMessagePriority;
INT   QueDepthThreshold;
CHAR  	Version[] = "ATP_11.2.1";

void Prepare_services_table_network_nonnetwork_struct()
{
	INT temp;
	INT temp_network = 0;
	INT temp_non_network = 0;
	
	for( temp = 0; temp < num_services; temp++ )
		{
			if( strncmp( services_table[temp].service_name,"n",1 ) == 0 )
			{
				memcpy(&services_table_network[temp_network], 
						&services_table[temp], sizeof( SERVICE_INFO ));
				temp_network = temp_network + 1;
			}
			else
			{
				memcpy(&services_table_non_network[temp_non_network], 
						&services_table[temp], sizeof( SERVICE_INFO ));
				temp_non_network = temp_non_network + 1;
			}
		}
	num_network_services = temp_network;
	num_non_network_services = temp_non_network;


}

/***********************************************************************/
/***********************************************************************/
INT StartSystem()
{
   INT temp;

   /* zero out the services table*/
   memset( services_table, 0, sizeof( services_table ) );
   num_services = 0;

   /* read the service table from disk*/
   if( !get_services_table( services_table, &num_services ) )
      return 0;

	/* now start the Ascendent services, if necessary*/
	if( pinstart_flag )
	{
		for( temp = 0; temp < num_services; temp++ )
		{
			start_service( &services_table [temp] );
         pteipc_sleep(3000);
		}
   }

	/* save the service info, backt to disk*/
	save_services_table( services_table, num_services );
    
	return 1;
}

/****************************************************************************/
/****************************************************************************/
BYTE get_service_info( pCHAR service_name, pSERVICE_INFO service_info,
                       pINT index )
{
	INT temp;

	for( temp = 0; temp < num_services; temp++ )
	{
		if( strcmp( service_name, services_table[temp].service_name ) == 0 )
		{
			memcpy(service_info, &services_table[temp], sizeof( SERVICE_INFO ));
			*index = temp;
			return( PTEMSG_OK );
		}
	}

	return( PTEMSG_SRVC_NOTFOUND );
}

/****************************************************************************/
/****************************************************************************/
pPTE_MSG process_srvc_control( pPTE_MSG p_msg_in, BYTE action )
{
   CHAR		   buffer[100], service_name[12];
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   BYTE           result_code;
   SERVICE_INFO   service_info;
   INT			   index;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   /* retrieve name of service from message*/
   strncpy( service_name, (pCHAR)p_data, data_len );

   /* locate service in services table*/
   if( ( result_code = get_service_info( service_name, &service_info,
						 &index ) ) != PTEMSG_OK )
	   goto build_msg;

   /* now call the OS specific routines to perform */
   /* the required action on the service*/
   switch( action )
	{
		case ST1_SRVC_START:
			result_code = start_service( &services_table [index] );
      	save_services_table( services_table, num_services );
			/*save_service_info( &services_table [index], index );*/
			break;

		case ST1_SRVC_STOP:
			result_code = stop_service( &services_table [index] );
      	save_services_table( services_table, num_services );
			/*save_service_info( &services_table [index], index );*/
			break;

		case ST1_SRVC_INQ:
			result_code = query_service( &service_info );
			break;

		case ST1_SRVC_ENABLE:
		case ST1_SRVC_DISABLE:
			result_code = enable_service( &services_table [index], action );
      	save_services_table( services_table, num_services );
			/*save_service_info( &services_table [index], index );*/
			break;

		case ST1_SRVC_TRACEON:
		case ST1_SRVC_TRACEOFF:
			result_code = trace_service( &services_table [index], action );
      	save_services_table( services_table, num_services );
			/*save_service_info( &services_table [index], index );*/
			break;

		case ST1_SRVC_MRAON:
		case ST1_SRVC_MRAOFF:
			result_code = mra_service( &services_table [index], action );
	      	save_services_table( services_table, num_services );
			break;

		case ST1_SRVC_FALCONON:
		case ST1_SRVC_FALCONOFF:
			result_code = falcon_service( &services_table [index], action );
	      	save_services_table( services_table, num_services );
			break;
	
		case ST1_SRVC_ENCON:
		case ST1_SRVC_ENCOFF:
			result_code = encrypt_trace_service( &services_table [index], action );
	      	save_services_table( services_table, num_services );
			break;

    }

    /* finally build the response message with result code*/
	build_msg:
   p_msg_out = ptemsg_build_msg( MT_SRVC_CONTROL,
								  action,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  NULL_PTR,
								  0,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		sprintf( buffer, "Unable to build response message to service monitor");
		srvcmgr_log_message(3,buffer,"process_srvc_control");
     	//printf( "%s\n", buffer );
		//LogEvent( buffer, ERROR_MSG );
	}
	else
		ptemsg_set_result_code( p_msg_out, result_code );	

   return( p_msg_out );
}




/****************************************************************************/
/****************************************************************************/
pPTE_MSG process_getall_msg()
{
	pPTE_MSG p_msg_out, p_tmp_msg;
	CHAR     buffer[100], num_services_ascii[12];

	p_msg_out = NULL_PTR;
	p_tmp_msg = NULL_PTR;

	/* simply send the complete services table over*/
	p_tmp_msg = ptemsg_build_msg( MT_SRVC_CONTROL,
								  ST1_SRVC_GETALL,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  (pBYTE)services_table,
								  sizeof( services_table ),
								  0 );
	
	itoa( num_services, num_services_ascii, 10 );
	p_msg_out = ptemsg_chain_data( p_tmp_msg, (pBYTE)num_services_ascii,
								   strlen( num_services_ascii ) + 1, 0 );

	if( p_msg_out == NULL_PTR )
	{
		sprintf( buffer, "Unable to build response message to service monitor");
		srvcmgr_log_message(3,buffer,"process_getall_msg");
     	//printf( "%s\n", buffer );
		//LogEvent( buffer, ERROR_MSG );
	}

	return p_msg_out;
}



/****************************************************************************/

/****************************************************************************/
pPTE_MSG process_getnonnetwork_msg()
{
	pPTE_MSG p_msg_out, p_tmp_msg;
	CHAR     buffer[100], num_services_ascii[12];

	p_msg_out = NULL_PTR;
	p_tmp_msg = NULL_PTR;

	/* simply send the complete services table over*/
	p_tmp_msg = ptemsg_build_msg( MT_SRVC_CONTROL,
								  ST1_SRVC_GETNONNC,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  (pBYTE)services_table_non_network,
								  sizeof( services_table_non_network ),
								  0 );
	
	itoa( num_non_network_services, num_services_ascii, 10 );
	p_msg_out = ptemsg_chain_data( p_tmp_msg, (pBYTE)num_services_ascii,
								   strlen( num_services_ascii ) + 1, 0 );

	if( p_msg_out == NULL_PTR )
	{
		sprintf( buffer, "Unable to build response message to service monitor");
		srvcmgr_log_message(3,buffer,"process_getnonnetwork_msg");
     	//printf( "%s\n", buffer );
		//LogEvent( buffer, ERROR_MSG );
	}

	return p_msg_out;
}




/****************************************************************************/

/****************************************************************************/
pPTE_MSG process_getnetwork_msg()
{
	pPTE_MSG p_msg_out, p_tmp_msg;
	CHAR     buffer[100], num_services_ascii[12];

	p_msg_out = NULL_PTR;
	p_tmp_msg = NULL_PTR;

	/* simply send the complete services table over*/
	p_tmp_msg = ptemsg_build_msg( MT_SRVC_CONTROL,
								  ST1_SRVC_GETNC,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  (pBYTE)services_table_network,
								  sizeof( services_table_network ),
								  0 );
	
	itoa( num_network_services, num_services_ascii, 10 );
	p_msg_out = ptemsg_chain_data( p_tmp_msg, (pBYTE)num_services_ascii,
								   strlen( num_services_ascii ) + 1, 0 );

	if( p_msg_out == NULL_PTR )
	{
		sprintf( buffer, "Unable to build response message to service monitor");
		srvcmgr_log_message(3,buffer,"process_getnetwork_msg");
     	//printf( "%s\n", buffer );
		//LogEvent( buffer, ERROR_MSG );
	}

	return p_msg_out;
}




/****************************************************************************/



/****************************************************************************/
pPTE_MSG process_srvc_all( BYTE action, pCHAR orig_queue, pCHAR orig_info )
{
	pPTE_MSG p_msg_out;
	CHAR     buffer[100], temp_str[100];
	BYTE	   rcode;
	LONG	   ipc_rcode;
	INT		temp;

	/* start/stop each service and send confirmation*/
	if( action == ST1_SRVC_STARTALL )
   {
	   for( temp = 0; temp < num_services; temp++ )
	   {
   	   rcode = start_service( &services_table [temp] );

		   p_msg_out = ptemsg_build_msg( MT_SRVC_CONTROL,
								  action,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  (pBYTE)services_table[temp].display_name,
								  strlen(services_table[temp].display_name)+1,
								  0 );

		   if( p_msg_out == NULL_PTR )
		   {
			   sprintf( buffer, "Unable to build response message to service monitor");
			   srvcmgr_log_message(3,buffer,"process_srvc_all");
       		//printf( "%s\n", buffer );
			   //LogEvent( buffer, ERROR_MSG );
		   }
		   else
         {
            ptemsg_set_orig_comms_info( p_msg_out, orig_info );
			   ptemsg_set_result_code( p_msg_out, rcode );	
         }

    	   if( p_msg_out != NULL_PTR )
    	   {
  			   ipc_rcode = pteipc_send( p_msg_out, orig_queue );
     		   if( ipc_rcode < 0 )
			   {
				   /* attempt to send response failed. Log the error*/
				   pteipc_get_errormsg( ipc_rcode, temp_str );
          		sprintf( buffer, "Error sending response to %s - %s", 
							  orig_queue, temp_str );
          		srvcmgr_log_message(3,buffer,"process_srvc_all");
				   //printf( "%s\n", buffer );
				   //LogEvent( buffer, ERROR_MSG );
			   }
            free( p_msg_out );
		   }

		   /* provide an interval between each service*/
         pteipc_sleep(1000);
	   }
   }
	else
   {
      /* Stop them in the reserve order as starting them */
	   for( temp = (num_services - 1); temp >= 0; temp-- )
	   {
		   rcode = stop_service( &services_table [temp] );

		   p_msg_out = ptemsg_build_msg( MT_SRVC_CONTROL,
								  action,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  (pBYTE)services_table[temp].display_name,
								  strlen(services_table[temp].display_name)+1,
								  0 );

		   if( p_msg_out == NULL_PTR )
		   {
			   sprintf( buffer, "Unable to build response message to service monitor");
			   srvcmgr_log_message(3,buffer,"process_srvc_all");
       		//printf( "%s\n", buffer );
			  // LogEvent( buffer, ERROR_MSG );
		   }
		   else
         {
            ptemsg_set_orig_comms_info( p_msg_out, orig_info );
			   ptemsg_set_result_code( p_msg_out, rcode );	
         }

    	   if( p_msg_out != NULL_PTR )
    	   {
  			   ipc_rcode = pteipc_send( p_msg_out, orig_queue );
     		   if( ipc_rcode < 0 )
			   {
				   /* attempt to send response failed. Log the error*/
				   pteipc_get_errormsg( ipc_rcode, temp_str );
          		sprintf( buffer, "Error sending response to %s - %s", 
							  orig_queue, temp_str );
          		srvcmgr_log_message(3,buffer,"process_srvc_all");
				  // printf( "%s\n", buffer );
				   //LogEvent( buffer, ERROR_MSG );
			   }
            free( p_msg_out );
		   }

		   /* provide an interval between each service*/
         pteipc_sleep(1000);
	   }
   }

	/* save the service info, back to disk*/
	save_services_table( services_table, num_services );

	return NULL_PTR;
}

/****************************************************************************/
/****************************************************************************/
void ping_all_services()
{
	BYTE	   pcode, scode;
	INT      temp, temp2;
	CHAR		tmpQueName[MAX_QUE_NAME_SIZE], buffer[100];
	LONG		tmpQueId, RetCode = 0;	/* rs-070799 */
	QUEINFOQUE	QueData;

	/* start/stop each service and send confirmation*/
	for( temp = 0; temp < num_services; temp++ )
	{
      if (EndProcessSignalled)
         break;
      
      /* if the service is stopped or disabled, fake a positive response*/
      /* these two cases will not cause the ping_count to be incremented*/
      if( (services_table [temp].running != '1') || (services_table [temp].disable != '0') )
      {
         pteipc_sleep(1000);
	      continue;    
      }
      
      if (EndProcessSignalled)
         break;

      pcode = ping_service( &services_table [temp] );

      if (EndProcessSignalled)
         break;

      /* process the response from service manager*/
      if (pcode == PTEMSG_OK)
      {
         services_table[temp].ping_count = 0;
      }
      else
      {
         /* 
            If one of the services failed to respond to the ping
            then shutdown the whole system, including dialog manager
            to force a NAC level failover, this piece of code was 
            copied from the OnStopAll() function and could be made
            into a function itself that both functions can call
         */

         services_table[temp].ping_count++;

         if (services_table[temp].ping_count == MaxPingRetries)
         {
            if (InRestartApplicationMode)
            {
               sprintf(buffer, "MaxPingRetries Exceeded, Ascendent Service %s on This Machine will be Restarted", services_table[temp].service_name);
               srvcmgr_log_message(1,buffer,"ping_all_services");
	           // LogEvent( buffer, WARN_MSG );
               scode = stop_service( &services_table [temp] );
               /* force reset all parameters */
               services_table[temp].running = '0';
               strcpy(services_table[temp].pid, "xxxx");
               save_services_table( services_table, num_services );
	            for( temp2 = 0; temp2 < num_services; temp2++ )
               pteipc_sleep(2000);
  	       
 	            /* Cleanup the queues */
	            strcpy(tmpQueName, services_table[temp].service_name);
               sprintf(buffer, "%sC", tmpQueName);
               pteipc_quedestroy(tmpQueName);
               sprintf(buffer, "%sI", tmpQueName);
               pteipc_quedestroy(tmpQueName);

   	         scode = start_service( &services_table [temp] );
               save_services_table( services_table, num_services );
               services_table[temp].ping_count = 0;
            }
            else
            {
               TakeAscendentDown = TRUE;

	            for( temp2 = 0; temp2 < num_services; temp2++ )
	            {
                  if (services_table[temp2].shutdown == '1')
                  {
                     sprintf(buffer, "MaxPingRetries Exceeded, Ascendent Service %s will be Shutdown, CALL THE ASCENDENT SYSTEM ADMINISTRATOR IMMEDIATELY", 
                                          services_table [temp2].service_name );
                     srvcmgr_log_message(1,buffer,"ping_all_services");
                     //LogEvent(buffer, WARN_MSG);
                     scode = stop_service( &services_table [temp2] );
                  }
               }
            }
         }
      }

      /* Check if we have to exit the for loop */
      if (TakeAscendentDown)
         break;

	   /* 
		   Obtain the Shared Que Id and check it's Current Message Count rs-070799
 	   */
	   strcpy( tmpQueName, services_table[temp].exe_name );
	   strcat( tmpQueName, "A" );
	   tmpQueId = pteipc_queaccess ( tmpQueName );
	   if ( tmpQueId > 0 ) 
	   {
		   memset( (char *)&QueData, 0x00, sizeof(QueData) );
		   RetCode = QueInfoQue( tmpQueId, &QueData );
		   if ( QueData.CountMessages >= QueDepthThreshold )
		   {
			   sprintf( buffer, "Current Message Count of [%d] for Queue [%s] exceeds threshold of [%d]",
			   QueData.CountMessages,tmpQueName, QueDepthThreshold );
			   srvcmgr_log_message(3,buffer,"ping_all_services");
       		//printf( "%s\n", buffer );
			  // LogEvent( buffer, ERROR_MSG );
 		   }		 
	   }
	
		pteipc_sleep( FailoverPingInterval*1000 );

   } /* Endof for() */

	/* save the service info, backt to disk*/
	save_services_table( services_table, num_services );
}

/*************************************************************
 ping_spy is run as a thread, it pings the registered apps 
 every FailoverPingInterval seconds 
 *************************************************************/
void *ping_spy( void *dummy )
{
  LONG login_result;
  CHAR xipcinstance_name[256];

  if (!num_services) 
     return(NULL);

  GetThisXipcInstance( xipcinstance_name );
  login_result = pteipc_xipclogin(xipcinstance_name, "pingspy");

  if (login_result > 0)
  {
      while (!EndProcessSignalled && !TakeAscendentDown)
      {
         ping_all_services();
      }
  }

  pteipc_xipclogout();
  return(NULL);
}

/***********************************************************************/
/***********************************************************************/
void ping_thread_start()
{
#ifndef WIN32
   pthread_t lthread_id;

   pthread_create(&lthread_id, NULL, ping_spy, NULL);
#else
   _beginthread((void *)ping_spy, 0, NULL);
#endif
}

/***********************************************************************/
/***********************************************************************/
void update_services_file_with_pid(pPTE_MSG p_msg_in)
{
    INT           entry;
    pCHAR         tmpptr, tmpstr;
    BYTE          rcode, sub_type1;
    CHAR          orig_comms_info[MAX_ORIG_INFO_SIZE];
    SERVICE_INFO  service_info;

    sub_type1 = ptemsg_get_msg_subtype1( p_msg_in );
	 switch( sub_type1 )
	 {
	 	case ST1_SYS_APP_CHECKIN:
         tmpstr = ptemsg_get_msg_orig_comms_info ( p_msg_in );
         strcpy(orig_comms_info, tmpstr);
         tmpptr = strchr(orig_comms_info, ',');
         if (tmpptr != NULL_PTR)
         {
            *tmpptr++ = NULL_PTR;

            rcode = get_service_info( orig_comms_info, &service_info, &entry );
            if (rcode == PTEMSG_OK)
            {
               strcpy(services_table[entry].pid, tmpptr);
	            /* save the service info, back to disk*/
	            save_services_table( services_table, num_services );
            }
         }
    }
}

/****************************************************************************/
/****************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{
    pCHAR    	orig_queue, orig_info;
    CHAR     	buffer[256], temp_str[100];
    LONG     	ipc_rcode;
    pPTE_MSG	p_msg_out;
    BYTE		   sub_type1;

    orig_queue = ptemsg_get_msg_orig_queue( p_msg_in );
    orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );
    ptestats_accumulate_msg_stats( p_msg_in );

    switch( ptemsg_get_msg_type( p_msg_in ) )
    {
        case MT_SRVC_CONTROL:

			/* request from service monitor. process message*/
         sub_type1 = ptemsg_get_msg_subtype1( p_msg_in );
			switch( sub_type1 )
			{
				case ST1_SRVC_INQ:
				case ST1_SRVC_START:
				case ST1_SRVC_STOP:
				case ST1_SRVC_ENABLE:
				case ST1_SRVC_DISABLE:
				case ST1_SRVC_TRACEON:
				case ST1_SRVC_TRACEOFF:
				case ST1_SRVC_ENCON:
				case ST1_SRVC_ENCOFF:
				case ST1_SRVC_MRAON:
				case ST1_SRVC_MRAOFF:
				case ST1_SRVC_FALCONON:
				case ST1_SRVC_FALCONOFF:
					p_msg_out = process_srvc_control( p_msg_in, sub_type1 );
					break;

				case ST1_SRVC_GETALL:
					p_msg_out = process_getall_msg();
					break;
				case ST1_SRVC_GETNONNC:
					p_msg_out = process_getnonnetwork_msg();
					break;
				case ST1_SRVC_GETNC:
					p_msg_out = process_getnetwork_msg();
					break;


				case ST1_SRVC_STARTALL:
				case ST1_SRVC_STOPALL:
					p_msg_out = process_srvc_all( sub_type1, orig_queue, orig_info );
					break;

        		default:
					//printf( "Ascendent: unknown subtype1 from %s\n", orig_queue );
					sprintf( buffer, "Ascendent: Service Manager: Unknown subtype1 from %s\n", orig_queue );
					srvcmgr_log_message(3,buffer,"PinnacleMsgHandler");
					//LogEvent( buffer, ERROR_MSG );
			}
	    	break;

        case MT_SYSTEM_REQUEST:
	    	ptesystm_system_msg_handler( p_msg_in );
         break;

        case MT_SYSTEM_REPLY:
         p_msg_out = NULL_PTR;
         update_services_file_with_pid(p_msg_in);
         break;

        default:
			//printf( "unknown msgtype from %s\n", orig_queue );
			sprintf( buffer, "Ascendent: Service Manager: Unknown msgtype from %s\n", orig_queue );
			srvcmgr_log_message(3,buffer,"PinnacleMsgHandler");
			//LogEvent( buffer, ERROR_MSG );
	}

    /* try to send the reply to service monitor, if necessary*/
    if( p_msg_out != NULL_PTR )
    {
      ptemsg_set_orig_comms_info( p_msg_out, orig_info );
  		ipc_rcode = pteipc_send( p_msg_out, orig_queue );
     	if( ipc_rcode < 0 )
		{
			/* attempt to send response failed. Log the error*/
			pteipc_get_errormsg( ipc_rcode, temp_str );
         sprintf( buffer, "Ascendent: Service Manager: Error sending response to %s - %s", orig_queue, temp_str );
         srvcmgr_log_message(3,buffer,"PinnacleMsgHandler");
			//printf( "%s\n", buffer );
			//LogEvent( buffer, ERROR_MSG );
		}
      if( p_msg_out != NULL_PTR )
         free( p_msg_out );
	}
}


/****************************************************************************/
/****************************************************************************/
void MainProcessor()
{
	INT success;
   CHAR error_msg[256], buffer[256];
   CHAR xipcinstance_name[64];
   LONG ipc_rcode;
   pPTE_MSG p_msg;
	CHAR  time_date[25] = "";
	CHAR  current_mmdd[5]={0};
	INT   len=0;
	FILE  *fp;

	/*Log event indicating service was started */
   sprintf( buffer, "Ascendent ServiceManager was started\n" );
   srvcmgr_log_message(1,buffer,"MainProcessor");
	//printf( "Ascendent ServiceManager was started\n" );
	//LogEvent( "Ascendent ServiceManager was started", INFO_MSG );

  Create_srvcmgr_Error_Filename();
	get_startup_flags( &pinstart_flag, &xipcstart_flag, &depstart_flag );

    /* start all the services that Ascendent is dependent on */
    if( depstart_flag )
    	success = dependency_start();
    else
        success = TRUE;

	/* next, fire up the XIPC instance */
    GetXipcInstanceName( xipcinstance_name );
    if( xipcstart_flag )
      success = xipc_start( xipcinstance_name );
    else
      success = TRUE;

    if (success)
    {
      /* initialize the XIPC stuff for the application */
      if( !pteipc_init_single_instance_app( ServiceName, xipcinstance_name ))
      {
    	   sprintf( buffer, "Failed to login to XIPC\n" );
    	   srvcmgr_log_message(1,buffer,"MainProcessor");
	     //printf( "Failed to login to XIPC\n" );
        // LogEvent( "Failed to login to XIPC", ERROR_MSG );
         pteipc_shutdown_single_instance_app();
      }
      else
      {
    	  pteipc_sleep(3000);
	     printf( "pteipc init successful\n" );

         GetSystemMessagePriority(&SystemMessagePriority);
         GetQueueDepthThreshold(&QueDepthThreshold);
      
         /* next startup all the Ascendent services */
         StartSystem();

		 /*PrePare services_table_network and non_network struncture seperatyl
		 We are doing so that we can send data more than 65535*/
		 Prepare_services_table_network_nonnetwork_struct();
		
         /* Mods to add automatic NAC level failoever to system */
         /* Get the FailoverPingInterval from tf.ini, if not found, then default to 10 Sec */
  	      success = GetFailoverPingInfo( &FailoverPingInterval, &MaxPingRetries, &FailoverReceiveTimeout );
         if (success)
         {
           printf("GetFailoverPingInterval succeeded\n");
           sprintf(buffer, "FailoverPingInterval=%0ld, MaxPingRetries=%0d, FailoverReceiveTimeout=%0d",
                                         FailoverPingInterval, 
                                         MaxPingRetries, 
                                         FailoverReceiveTimeout);
           srvcmgr_log_message(1,buffer,"MainProcessor");
           //LogEvent( buffer, INFO_MSG );
         }
         else
        	 sprintf(buffer, "Failed to Retrieve Failover Parameters, Check the TF.INI file\n");
        	  srvcmgr_log_message(3,buffer,"MainProcessor");
           //LogEvent( "Failed to Retrieve Failover Parameters, Check the TF.INI file\n", ERROR_MSG );
         
         /* User can enter 0 to disable functionality */
         if (FailoverPingInterval)
         {
            if (FailoverPingInterval > 100)
            {
               FailoverPingInterval -= 100;
               InRestartApplicationMode = TRUE;
            }
         
            ping_thread_start();
         }
         
         while( !EndProcessSignalled )
         {

     		ptetime_get_timestamp( time_date );

     	    memcpy( current_mmdd,   time_date+5, 2 );
     	    memcpy( current_mmdd+2, time_date+8, 2 );
     	    len = strlen(srvcmgr_Logging_Filename);

     	    if ( 0 != strncmp(&srvcmgr_Logging_Filename[len-8], current_mmdd,4) )
     	    {

     	    	Create_srvcmgr_Error_Filename();

     	    }
      		if((fp = fopen(srvcmgr_Logging_Filename,"a+b"))==NULL)
      		{
                sprintf( buffer, "Error on opening file %s", srvcmgr_Logging_Filename );
                srvcmgr_log_message(1,buffer,"MainProcessor");
      		}
      		fclose(fp);
            p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ipc_rcode );

            if( p_msg != NULL_PTR )
            {
	      	   PinnacleMsgHandler( p_msg );
               free( p_msg );
            }
            else if( ipc_rcode != QUE_ER_TIMEOUT )
            {
	      	   pteipc_get_errormsg( ipc_rcode, error_msg );
               sprintf( buffer, "Error on pteipc_receive %s", error_msg );
               srvcmgr_log_message(1,buffer,"MainProcessor");
               //printf( "%s\n", buffer );
               //LogEvent( buffer, WARN_MSG );
               pteipc_sleep(1000);
               if (ipc_rcode == QUE_ER_NOTLOGGEDIN)
                  break;
            }
         }
      }
    }

    pteipc_shutdown_single_instance_app();
    MainProcessDone = 1;
}

/***********************************************************************/
/***********************************************************************/
void EndProcess()
{

	CHAR     	Buffer[256];
  // printf( "Shutting down the Ascendent Service Manager\n" );
   // LogEvent( "Shutting down the Ascendent Service Manager", INFO_MSG );
    memset( Buffer, 0x00, sizeof(Buffer) );
    sprintf( Buffer,"Shutting down the Ascendent Service Manager\n");
    srvcmgr_log_message(3,Buffer,"EndProcess");
}

/******************************************************************************
 *
 *  NAME:         srvcmgr_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void srvcmgr_log_message(INT sev, pCHAR msg, pCHAR func)
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};


   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   memcpy( text_message, msg, sizeof(text_message)-1 );
   srvcmgr_Log_error_warning_to_File(text_message,sev,func);
   /* Call function to post the message. */

   return;
}
void Create_srvcmgr_Error_Filename( )
{
	CHAR  system_date[25] = "";
	 CHAR  filename   [80] = {0};
	 CHAR  tmpstr     [80] = {0};

	  memset( tmpstr, 0x00, sizeof(tmpstr) );
	  GetPinnacleBinDirectory(tmpstr);
	  sprintf(filename, "%s", tmpstr );

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

	strcpy(srvcmgr_Logging_Filename,filename);
	strcat( srvcmgr_Logging_Filename, "srvcmgr" );
	strcat( srvcmgr_Logging_Filename, "_" );
	strncat(srvcmgr_Logging_Filename, system_date,   4 );  /* YYYY */
	strncat(srvcmgr_Logging_Filename, system_date+5, 2 );  /* MM   */
	strncat(srvcmgr_Logging_Filename, system_date+8, 2 );  /* DD   */
	strcat(srvcmgr_Logging_Filename, ".out");  /* DD   */

    return;
}

INT srvcmgr_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR  card_tid_mid_details [256] = {0};
	INT   cardlen = 0;
	INT   nIndex = 0;
	CHAR  tempcard[21] = {0};

	FILE *fp;

	#define NUM_SIZE_WRITES  1

	// Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj"
	ptetime_get_timestamp( time_date );

	// Pick out the month and day to compare to filename extenstion.
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	// Compare filename extension to current date.
    len = strlen(srvcmgr_Logging_Filename);
    if ( 0 != strcmp(&srvcmgr_Logging_Filename[len-4], current_mmdd) )
    {
		// Now create the new filename.
    	Create_srvcmgr_Error_Filename();
    }
	if((fp = fopen(srvcmgr_Logging_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	strcat(Buffer, func);
	strcat(Buffer,"\n");

	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}
	fclose(fp);
	return  0;
}

