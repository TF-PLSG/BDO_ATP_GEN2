/*******************************************************************
*
* Copyright (c) 2006, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      amexmain.c
*  
* TITLE:       Main function for the amex network handler      
*  
* DESCRIPTION: This module will handle all incoming messages from
*               the terminal and the host.
*
* APPLICATION: 
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "amexmain.h"
#include "amexutil.h"
#include "amextran.h"
#include "memmnger.h"
#include "txutils.h"
#include "txtrace.h"
#include "diskio.h"


extern CHAR  ServiceName[12];
extern CHAR  tpdu_id[];

extern INT  volatile EndProcessSignalled;
extern INT  volatile MainProcessDone;

extern CHAR  LOGON_REQUEST[];    
extern CHAR  LOGOFF_REQUEST[];  
extern CHAR  ECHO_REQUEST[]; 
extern char amex_scheme_timeout_rc [4];
extern char amex_scheme_fai_rc [4];
/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

NCF01    ncf01;

CHAR  cur_app_name    [MAX_APP_NAME_SIZE];
CHAR  oprmon_que_name []           = "oprmonI";
CHAR  dialog_que_name []           = "dialogA";
CHAR  authorizer_que_name[]        = "txcntlA";
CHAR  updateds_que_name[]          = "updatdsA";
CHAR  SHARED_MEMORY_TBL[12];

CHAR   AppName[8] = "";
CHAR   Version[] = "ATP_11.12.0";
							  

/* Online Updates */
AMEX_ONLINE_UPDATES  Online_Updates;

/* Statistics for Monitor */
TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
CHAR   DirPath[MAX_APP_NAME_SIZE];
CHAR   TraceFile[MAX_APP_NAME_SIZE];
INT    Tracing;
FILE   TraceFile_fp;
FILE   *pTraceFile_fp;
FILE   **ppTraceFile_fp;

WORD  echo_timer_length = 0;
/*****************************************************************************

  Function:    PinnacleMsgHandler  

  Description: This function will bring in a PTE message and determine where
               it should be routed.
  Author: 
      unknown
  Inputs:
      p_msg_in - the incoming PTE message
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{

   pPTE_MSG_DATA   	p_msg_data = NULL_PTR;
   BYTE        		subtype1;
   AUTH_TX     		auth_tx;
   CHAR        		buffer[512] = {0};
   pPTE_MSG 		p_cur_msg;
   pPTE_MSG 		p_msg_out;
   pBYTE    		p_data_data = NULL_PTR;


   p_cur_msg = p_msg_in;
   p_msg_out = NULL_PTR;

   ptestats_accumulate_msg_stats(p_msg_in);

	switch( ptemsg_get_msg_type(p_cur_msg) )
	{
		case MT_AUTH_RESPONSE: 
          /* Response coming in via the authorizer */
		    (void)process_incoming_message(p_msg_in);
			 break;

      case MT_INCOMING: 
			 /* Reply from the external host */
	       (void)incoming_host_message(p_msg_in);
          break;

      case MT_TIMER_TIMEOUT: 
          /* Reply about timeout of ptemsg */
          (void)process_timeout_msg(p_msg_in);
          break;

      case MT_SYS_MONITOR:
         
         switch( ptemsg_get_msg_subtype1(p_cur_msg) )
         {
            case ST1_LOGON:
                // Request from Monitor to send LogOn 
                memset (&auth_tx, 0, sizeof(AUTH_TX));
                if (false == perform_host_logon_request(LOGON_REQUEST,
                                                        SOURCE_IS_OPERATOR,
                                                        &auth_tx))
                {
                   ncamex_log_message( 3, 1, "Error executing host LogOn request" , "PinnacleMsgHandler" , 0 ,"");
                }
               break;

            case ST1_LOGOFF:
                // Request from Monitor to send LogOff 
               memset (&auth_tx, 0, sizeof(AUTH_TX));
               if (false == perform_host_logon_request(LOGOFF_REQUEST,
                                                       SOURCE_IS_OPERATOR,
                                                       &auth_tx))
                {
                   ncamex_log_message( 3, 1, "Error executing host LogOff request" , "PinnacleMsgHandler" , 0 ,"");
                }
                break;

            case ST1_ECHOTEST:
                // Request from Monitor to send LogOn 
                memset (&auth_tx, 0, sizeof(AUTH_TX));
              	//HCTS1-D
              	ncamex_log_message( 1, 1, "Sending ECHO TEST request to host." , "PinnacleMsgHandler" , 0 ,"");
                if (false == perform_host_logon_request(ECHO_REQUEST,
                                                        SOURCE_IS_OPERATOR,
                                                        &auth_tx))
                {
                   ncamex_log_message( 3, 1, "Error executing host Echo Test" , "PinnacleMsgHandler" , 0 ,"");
                }
                break;
  
            case ST1_NETWORK_QUERY:
                /* Request from Monitor to send status information */
                if (false == send_network_status_to_monitor())
                {
                   ncamex_log_message( 2, 1, "Error sending network status to monitor" , "PinnacleMsgHandler" , 0 ,"");
                }
                break;
  
            default:
                break;
         }
            break;
        

		case MT_SYSTEM_REQUEST: 
         PRINT("MT_SYSTEM_REQUEST  \n" );
         ptesystm_system_msg_handler(p_msg_in);
		 process_encryption_flag( p_msg_in );
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            if ( Tracing == TRACE_ON )
            {
               /* Flush the trace file buffer when service is pinged. */
               if ( false == flush_file(ppTraceFile_fp) )
               {
                  Tracing = TRACE_OFF;
                  sprintf( buffer,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     ServiceName );
                  ncamex_log_message( 3, 1, buffer , "PinnacleMsgHandler" , 0 ,"");
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Network Controller Service: %s, version %s",
                     ServiceName, Version );
            ncamex_log_message( 3, 1, buffer , "PinnacleMsgHandler" , 0 ,"");
         }
         else if(ST1_LOAD_TO_RC_VALUES == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 ncamex_read_TO_RC_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "amex_scheme_timeout_rc: %s",
					  amex_scheme_timeout_rc);
        	 ncamex_log_message( 1, 1, buffer, "ascendent_msg_handler" ,0,"");
         }
         else if(ST1_LOAD_RC_FOR_FAI == ptemsg_get_msg_subtype1(p_msg_in))
          {
         	 ncamex_read_fai_RC_from_tf_ini();
         	 send_trace_response(p_msg_in);
         	 sprintf( buffer,
         	          "amex_scheme_fai_rc: %s",
 					  amex_scheme_fai_rc);
         	 ncamex_log_message( 1, 1, buffer, "ascendent_msg_handler" ,0,"");
          }
         break;

      case MT_SYSTEM_REPLY: 
         PRINT("MT_SYSTEM_REPLY  \n" );
         break;

      case MT_DB_REPLY:
            if (PTEMSG_OK == ptemsg_get_result_code(p_msg_in))
            {
               hhutil_parse_db_ok(p_msg_in);
            }
            else
            {
               hhutil_parse_db_error(p_msg_in);
            }
          break;

      case MT_TIMER_REPLY:
         PRINT("MT_TIMER_REPLY  \n" );
         if (PTEMSG_OK == ptemsg_get_result_code(p_msg_in))
         {
            subtype1 = ptemsg_get_msg_subtype1(p_msg_in);

            if (ST1_TIMER_READ == subtype1)
            {
               (void)incoming_host_message_timer_reply(p_msg_in);
            }
            else if (ST1_TIMER_CLEAR == subtype1)
			{
			   (void)incoming_host_message_timer_clear(p_msg_in);
			}
            else if (ST1_TIMER_START == subtype1)
            {
               send_request_to_host_timer_reply(p_msg_in);
            }

         }
         break;


      case MT_GET_STATS:
         (void)send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:
         (void)reset_transaction_statistics( p_msg_in );
         break;

      case MT_INCOMING_GUI:
         /* GUI is initiating an 1304 message for Amex: Online File Update. */

		  (void)process_file_update_request( p_msg_in );
      break;

      case MT_NSP_XLATETOANSI_RESPONSE:

         // Response from a translate key
	      p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
	      memcpy(&auth_tx, p_data_data, sizeof(AUTH_TX));
         if ( false == process_incoming_message_continued( &auth_tx ) )  
         {
            ncamex_log_message( 3, 2, "Error processing incoming security request" , "PinnacleMsgHandler" , 0 ,"");
         }
      break;


      default :
         ncamex_log_message( 3, 2, "Unknown message type received" , "PinnacleMsgHandler" , 0 ,"");
			break;
	}  /* switch msg_type */

} /* PinnacleMsgHandler */

/*****************************************************************************

  Function:    MainProcessor

  Description: This function will start up and shutdown ncamex, as well as
               check for and bring in new PTE messages.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void MainProcessor()
{
	pPTE_MSG	p_msg;
	LONG		ret_code;
   INT      rc;
	CHAR		ErrorMsg[512] = {0};
	CHAR		Buffer[512] = {0};
   CHAR     err_msg[200];
   AUTH_TX  auth_tx;
   NCF01    ncf01_i;
    
	GetAppName( AppName );

   GetAppName (cur_app_name);
   GetXipcInstanceName( Buffer );
   sprintf( Buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   ncamex_get_error_warning_file_name_path();
   ncamex_log_message( 2, 1, Buffer , "MainProcessor" , 0 ,"");
   printf( "%s\n", Buffer );

#ifdef _DEBUG   
   if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) ) 
#else
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName, Buffer ))
#endif
	{
	   ncamex_log_message( 3, 3, "Failed to create XIPC queues" , "MainProcessor" , 0 ,"");
		/* pteipc_shutdown_single_instance_app(); */
	   pteipc_shutdown_multiple_instance_app();
	   MainProcessDone = 1;
	   return;
	}

   if (strlen(AppName) > (sizeof(ncf01.primary_key.network_id)-2) )
	{
      sprintf(Buffer,
             "Error - the name of the application: %s  must be %d characters or less",
              AppName,(sizeof(ncf01.primary_key.network_id)-2));
      ncamex_log_message( 3, 2, Buffer , "MainProcessor" , 0 ,"");
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
		MainProcessDone = 1;
		return;
	}

   /*  Create a table to be used for sahred memory between multiple
       instances of this Network Control Module.  It contains three 
       fields:  # of consecutive timeouts, # of active transactions, 
       and the current state of the host.
   */
   memset(SHARED_MEMORY_TBL, 0, sizeof(SHARED_MEMORY_TBL));
   strcpy(SHARED_MEMORY_TBL,cur_app_name);
   strcat(SHARED_MEMORY_TBL,"Table");
   ret_code = create_generic_table( SHARED_MEMORY_TBL);
   if ( MEMACCESSERROR == ret_code )
   {
      // Table already exists.
      PRINT ("Shared memory table already exists. \n");
   }
   else if ( 0 == ret_code )
   {
      // Table was successfully created.  Initialize host counters and state. 
      ret_code = WriteGenericTable( SHARED_MEMORY_TBL, "0", "0", ONLINE );
      if ( 0 != ret_code)
      {
         /* Error creating the table.  It must already exist. */
         ncamex_log_message( 3, 2, "Unable to write to shared memory" , "MainProcessor" , 0 ,"");
      }
   }
   else
   {
         /* Error creating the table.   */
         ncamex_log_message( 3, 2, "Unable to create to shared memory" , "MainProcessor" , 0 ,"");
   }
  
   /* Initialize transaction statistics */
   (void)reset_stats( &Txn_Counts );

   /* Determine if transaction tracing should be turned on. */
   rc = get_trace_ini_info( AppName, DirPath, ErrorMsg );
   if ( rc == 0 )
   {
      Tracing = TRACE_ON;

      /* Open the transaction trace file. */
      memset( TraceFile, 0x00, sizeof(TraceFile) );

      pTraceFile_fp  = &TraceFile_fp;
      ppTraceFile_fp = &pTraceFile_fp;

      if (false == open_trace_file(AppName,DirPath,ppTraceFile_fp,TraceFile))
      {
         Tracing = TRACE_OFF;
         sprintf( ErrorMsg,
                 "Unable to open trace file %s. Tracing is off",
                  TraceFile );
         ncamex_log_message( 3, 1,ErrorMsg , "MainProcessor" , 0 ,"");
      }
      else
      {
    	  ncamex_log_message( 3, 1,"Tracing is turned ON" , "MainProcessor" , 0 ,"");
      }
   }
   else
   {
      Tracing = TRACE_OFF;
      if ( rc == -1 )
      {
    	  ncamex_log_message( 3, 1,ErrorMsg , "MainProcessor" , 0 ,"");
      }
      ncamex_log_message( 3, 1,"Tracing is turned off" , "MainProcessor" , 0 ,"");
   }

   /* Get Acquiring network record. */
   memset(&ncf01, 0, sizeof(NCF01));
   strcpy(ncf01.primary_key.network_id, AppName );
   ncf01.primary_key.network_type[0] = 'A';
   if (hhutil_get_ncf01_cfg( &ncf01, err_msg) != PTEMSG_OK)
   {
      sprintf(Buffer, "Error on select of NCF01: %s", err_msg);
      ncamex_log_message( 3, 2,Buffer , "MainProcessor" , 0 ,"");
   }

   /* Get Issuing network record */
   memset(&ncf01_i, 0, sizeof(NCF01));
   strcpy(ncf01_i.primary_key.network_id, AppName );
   ncf01_i.primary_key.network_type[0] = 'I';
   if (hhutil_get_ncf01_cfg( &ncf01_i, err_msg) == PTEMSG_OK)
   {
      /* Copy some information from Issuer record into Acquirer record. */
      strcpy( ncf01.forwarding_inst_id_code,  ncf01_i.forwarding_inst_id_code  );
      strcpy( ncf01.max_consecutive_timeouts, ncf01_i.max_consecutive_timeouts );
      strcpy( ncf01.request_timer,            ncf01_i.request_timer            );
      strcpy( ncf01.max_active_txns,          ncf01_i.max_active_txns          );
      strcpy( ncf01.acquirer_inst_id_code,    ncf01_i.acquirer_inst_id_code    );
   }
   //HCTS1-A
   echo_timer_length = atoi(ncf01.echo_interval);
   if (0 == strcmp(ncf01.auto_logon,"1"))
   {
      // set up a logon request to the host in auth_tx 
      memset(&auth_tx, 0, sizeof(AUTH_TX));
      perform_host_logon_request(LOGON_REQUEST, SOURCE_IS_OPERATOR, &auth_tx);
   }

   if (false == send_network_status_to_monitor())
   {
      ncamex_log_message(2, 1,"Error sending network status to monitor" , "MainProcessor" , 0 ,"");
   }

   ncamex_read_TO_RC_from_tf_ini();
   ncamex_read_fai_RC_from_tf_ini();
   while( !EndProcessSignalled )
	{

		/* You are blocked here waiting for a message on either app queue or control que  */
		/* if there is no message on either que for 5 seconds, the blocking call returns   */

		/* The following line will be used to get messages from the queue. For now the
		   message is automatically created here, but this will be removed */
	   p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ret_code ); 

			                 
		if( p_msg != NULL_PTR )
		{
			PinnacleMsgHandler( p_msg );
			free (p_msg); 
		}
		else if( ret_code != QUE_ER_TIMEOUT ) 
		{
			pteipc_get_errormsg( ret_code, ErrorMsg );
			sprintf( Buffer, "Error on pteipc_receive %s", ErrorMsg );
			ncamex_log_message(3, 3,Buffer , "MainProcessor" , 0 ,"");
		}
	}

   WriteGenericTable( SHARED_MEMORY_TBL, "0", "0", DOWN );
   (void)send_network_status_to_monitor();

#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
	MainProcessDone = 1;
}



/*****************************************************************************

  Function:    EndProcess   

  Description: This function will output a message to notify the user that ncamex
               is being shut down.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void EndProcess()
{
   CHAR Buffer[256] = {0};

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         ncamex_log_message(3, 1,Buffer , "EndProcess" , 0 ,"");
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   ncamex_log_message(2, 1,Buffer , "EndProcess" , 0 ,"");
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}


