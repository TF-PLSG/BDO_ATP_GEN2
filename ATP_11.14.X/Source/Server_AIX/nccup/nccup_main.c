/******************************************************************************
   *
   * Copyright (c) 2002, Hypercom, Inc.
   * All Rights Reserved.
   *
   * MODULE:	  nccup_main.c
   *  
   * TITLE: 	  CUP Network Controller Database Processor
   *  
   * DESCRIPTION: This module contains functions that handle requests to
   *			  and responses from a data server.
   *
   * APPLICATION: Advanced Transaction Processor (ATP)
   *
   * AUTHOR:	 Abhishek Verma
   
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "pteipc.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "txutils.h"
#include "txtrace.h"
#include "diskio.h"
#include "cup.h"
#include "nccup_prototypes.h"


extern INT  EndProcessSignalled;
extern INT  MainProcessDone;
extern CHAR ServiceName[12];
extern char cup_scheme_timeout_rc [3];
extern char cup_scheme_fai_rc [3];
/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

/* QUEUES */
CHAR  AppName[80];
CHAR  updateds_que_name[]   			= "updatdsA";
CHAR  trands_que_name []    			= "trandsA";
CHAR  netds_que_name []     			= "netdsA";
CHAR  timerds_que_name []   			= "timerdsA";
CHAR  nsp_que_name []       			= "racalA";
CHAR  authorizer_que_name[] 			= "txcntlA";
CHAR  applnk_que_name []    			= "applnkA";
CHAR  oprmon_que_name[]     			= "oprmonI" ;
CHAR  dialog_que_name []    			= "dialogA";
CHAR  nccup_issuing_link_name[]  		= "nccup";
CHAR  pos_handler_module_name[]         = "dcp";
CHAR  voice_handler_que_name[]          = "VOICE";
CHAR  sbatch_handler_que_name[]         = "sbat";
CHAR  cadencie_cup_queue_name[]			= "nccup2";

/* STRUCTURES */
AUTH_TX              Auth_Tx;
NCF01                Ncf01_I;  //Module ncf01_issue table
NCF01                Ncf01_A;  //Module ncf01_acq table
NCF01                Ncf01_Nccup_I; // NCCUP module local structue to keep nccup3 ncf01_issuing table details
NCF01                Ncf01_Nccup3_I; //NCCUP module local structue to send CUP incoming keys details to nccup3 modules
NCF01                Nccup3_Nccup_I; //NCCUP3 module local structue to keep nccup ncf01_issuing table details
NCF30                Ncf30;
CRF01                Crf01;
CHAR    			 tpdu_id[16];

AMEX_ONLINE_UPDATES  Online_Updates;

/* STRUCTURE SIZES */
INT  Auth_Tx_Size;
INT  Ncf01_Size;
INT  Ncf30_Size;
INT  Tlf01_Size;
INT  Crf01_Size;
NCF01    ncf01;

/* Network Type: Issuer, Acquirer, or Both */
INT    NetworkType;
INT    TranFlow;

/* SHARED MEMORY */
BYTE   net_consec_tmouts_ctr[4];
BYTE   active_txns_ctr[4];
BYTE   current_state[2];

/* Counters */
INT    MaxActiveTxns;
INT    MaxConsecTimeouts;

/* Statistics for Monitor */
TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;

/* Originator Information */
BYTE   Orig_Queue[17];
BYTE   Orig_Info[33];

/* Dynamic Key Exchange */
CHAR   KeyChange[CUP_DE48_SIZE+1];

/* Echo Timer Info */
INT    Echo_Timer_Flag;
INT    Echo_Timer_Count;
CHAR   TimerAppData2[MAX_APP_DATA_SIZE];

/* Txn Tracing Info */
CHAR   DirPath[MAX_APP_NAME_SIZE];
CHAR   TraceFile[MAX_APP_NAME_SIZE];
INT    Tracing;
FILE   TraceFile_fp;
FILE   *pTraceFile_fp;
FILE   **ppTraceFile_fp;

/*Flags*/
INT extendedbit = false;
INT tpdu_id_flag = false;
INT Txn_Is_A_Reject = false;


CHAR   NMI_Code[4];
INT    invalid_msg_format;
CHAR   static_Rtdcwk_Key[49] = {0};
CHAR   MAC[17] = {0};
CHAR  nccup_Erro_warning_Filename[256] = {0};
CHAR  nccup_module_error_warning_file_name[256] = {0};
CHAR  nccup_error_warning_file_path[256] ={0};

INT Key_exchange_enable_Flag = 0;
CHAR Last_Tran_Logging_Filename[256];

CHAR   Version[] = "ATP_11.12.0";
							   
/******************************************************************************
 *
 *  NAME:         PinnacleMsgHandler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into nccup2
 *                host handler.  It will transfer control to the appropriate
 *                module, based on message type.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{
   BYTE            subtype1;
   BYTE            msgtype;
   CHAR            buffer[256];      
   pBYTE           p_data = NULL_PTR;

   ptestats_accumulate_msg_stats(p_msg_in);

   /* Determine message type so we know which module to send it to. */
   msgtype = ptemsg_get_msg_type(p_msg_in);
   switch( msgtype )
   {
      case MT_AUTH_REQUEST:

         /* Authorization request from txcntl */
         process_request_for_host( p_msg_in );
         break;

	 case MT_AUTH_RESPONSE: 
     	 /* Response coming in via the authorizer */
	    (void)process_incoming_message(p_msg_in);
		 break;

      case MT_INCOMING: 

         /* Message(request/response) from the BDO Cadencie host */
         process_host_msg( p_msg_in );
         break;

      case MT_DB_REPLY:

         /* Message coming from the data server */
         process_db_reply( p_msg_in );
         break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE: 

         /* Response from the network security processor */
         process_translate_pin_response( p_msg_in );
         break;

	  case MT_NSP_TRANSLATE_ZMK_LMK_RESPONSE:
	  	 /* Response from the network security processor */
         process_key_translate_response( p_msg_in );
         break;
	
      case MT_TIMER_REPLY:
         subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
         if ( subtype1 == ST1_TIMER_CLEAR )
         {
            /* This is a response from timerds to a clear timer request.
             * Continue processing the transaction.
             */
            process_clear_timer_reply( p_msg_in );
         }
         break;

      case MT_TIMER_TIMEOUT: 

         /* One of the timers expired. */
         process_timeout_msg( p_msg_in );
         break;

      case MT_SYS_MONITOR:

         /* Process a request from Monitor. */
         process_monitor_request( p_msg_in );
         break;  

      case MT_GET_STATS:

         /* Send transaction counts to whoever requested them. */
         (void)send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:

         /* Reset transaction counts to zero. */
         (void)reset_transaction_statistics( p_msg_in );
         break;
		 
		case MT_NSP_CUP_KEY_RESPONSE:
		    // This should be done by only ACQ link
			 if((0 != strncmp(AppName,"nccup3",6)))
			 {
				process_key_request_response(p_msg_in);
			 }
			break;
		case MT_NSP_CUP_KEY_UPDATE_REQUEST:
			/* Due to round robin design of CUP , New Key is received by nccup module.
			// nccup module send newly received key to nccup3 module.
			//nccup3 should contine as it has received new key from CUP host*/
			 if((0 == strncmp(AppName,"nccup3",6)))
			 {
				 nccup_process_key_update_request_from_Acq(p_msg_in);
			 }
			break;
		case MT_NSP_CUP_KEY_TRANSLATE_RESPONSE:
			/* Key translation is successful, nccup module is online,
			let us make nccup3 module also online after successful key transalation
			else make state OFFLINE*/
			if((0 == strncmp(AppName,"nccup3",6)))
			{
				subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
				if ( subtype1 == ST1_CUP_KEY_TRANSLATION_SUCCESS )
				{
					//process_logon_Reset_Key_exchange_disable_flag();
					process_logon_Reset_Key_exchange_disable_flag_KEY_TRANSLATION_SUCCESS();
					nccup_process_logon_after_key_transalation();
				}
				else if ( subtype1 == ST1_CUP_KEY_TRANSLATION_FAILED )
				{
					//process_logon_Reset_Key_exchange_disable_flag();
					process_logon_Reset_Key_exchange_disable_flag_KEY_TRANSLATION_FAILED();
					nccup_process_logoff_after_key_transalation();
				}
				else if(subtype1 == ST1_CUP_ACTIVE_ACTIVE_TRIGGER_FAILED)
				{
					// Active Active trigger failed for nccup or nccup2 module go OFFLINE.
					//Go offline as Active Active failed.
					nccup_make_nccup3_offline_after_active_active_triger_failed();
					//Send message to nccup and nccup2 to go offline as active triger is failed
					nccup_send_logof_to_nccup_after_active_artive_triger_failed_from_nccup3();
					nccup_send_logof_to_nccup2_after_active_artive_triger_failed_from_nccup3();
				}

			}
			else
			{
				subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
				if(subtype1 == ST1_CUP_ACTIVE_ACTIVE_TRIGGER)
				{
					// This is Active active trigger coming from nccup3 module to nccup module.
					//Read translated key from DB.
					nccup_activ_active_trigger_handler_for_nccup();
				}
				else if(subtype1 == ST1_CUP_ACTIVE_ACTIVE_TRIGGER_FAILED)
				{
					// This is Active active trigger coming from nccup3 module and its failed.
					//Make nccup module status as offline.
					nccup_make_nccup_offline_after_active_active_triger_failed();
				}
			}

			break;

		case MT_SEND_LOG_OFF:

			/*One CUP link is offline , make other link offline as well*/
			nccup_process_logoff_after_logoff_message();
			break;

      case MT_SYSTEM_REQUEST:
         ptesystm_system_msg_handler( p_msg_in );
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
                     AppName );
                  nccup_log_message( 2, 1, buffer, "PingMsgHandler",0 );
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Network Controller Service: %s, version %s",
                     ServiceName, Version );
            nccup_log_message( 1, 1, buffer, "PingMsgHandler",0 );
         }
         else if(ST1_LOAD_TO_RC_VALUES == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 nccup_read_TO_RC_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "cup_scheme_timeout_rc: %s",
					  cup_scheme_timeout_rc);
        	 nccup_log_message( 1, 1, buffer, "ascendent_msg_handler" ,0);
         }
         else if(ST1_LOAD_RC_FOR_FAI == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 nccup_read_fai_RC_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "cup_scheme_fai_rc: %s",
					  cup_scheme_fai_rc);
        	 nccup_log_message( 1, 1, buffer, "ascendent_msg_handler" ,0);
         }

      break;

      case MT_SYSTEM_REPLY: 
         PRINT( "MT_SYSTEM_REPLY  \n" );
      break;

      case MT_INCOMING_GUI:
         /* GUI is initiating an 1304 message for CUP: Online File Update. */
         (void)process_file_update_request( p_msg_in );
      break;

      default :
         sprintf( buffer, "Unknown msg type received: %d", (INT)msgtype );
         nccup_log_message( 1, 2, buffer, "PingMsgHandler",0 );
      break;
   }  /* switch msg_type */

   return;
} /* PingMsgHandler */


/******************************************************************************
 *
 *  NAME:         MainProcessor
 *
 *  DESCRIPTION:  This is the driver function for the Equitable host handler.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void MainProcessor()
{
   pPTE_MSG  p_msg;
   LONG      ret_code;
   CHAR      ErrorMsg[100] = "";
   CHAR      buffer[256]   = "";
   CHAR      xipc_instance[30];

   GetAppName( AppName );
   GetXipcInstanceName( xipc_instance );

   sprintf( buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   nccup_log_message( 2, 1, buffer,"MainProcessor",0);
   PRINT2( "%s\n", buffer );

   nccup_get_error_warning_file_name_path();

   strcpy( Last_Tran_Logging_Filename, nccup_error_warning_file_path );
   strcat( Last_Tran_Logging_Filename, ServiceName );
   strcat( Last_Tran_Logging_Filename, "last_tran.log" );

   if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) ) 
   {
      nccup_log_message( 2, 3, "Failed to create XIPC queues.", "MainProcessor",0 );
      PRINT( "Failed to create XIPC queues\n" );

	  pteipc_shutdown_single_instance_app(); 

	  MainProcessDone = 1;
   }
   else
   {
      /* Perform any startup initialization. */
      if ( false == startup() )
      {
	      pteipc_shutdown_single_instance_app(); 

    	  MainProcessDone = 1;
      }
      else
      {
         /**************
          * MAIN  LOOP *
          **************/

         while( !EndProcessSignalled )
         {
            /* You are blocked here waiting for a message on either app queue
             * or control queue. If there is no message on either queue for 5
             * seconds, the blocking call returns.
             *
             * The following line will be used to get messages from the queue.
             */
            p_msg = pteipc_receive( application_que_name,
                                    control_que_name,
                                    5, &ret_code ); 

            if ( NULL_PTR != p_msg )
            {
               PinnacleMsgHandler( p_msg );
               free( p_msg );
            }
            else if ( QUE_ER_TIMEOUT != ret_code ) 
            {
               pteipc_get_errormsg( ret_code, ErrorMsg );
               sprintf( buffer, "Error on pteipc_receive %s", ErrorMsg );
               nccup_log_message( 2, 3, buffer, "MainProcessor",0 );
            }
         }
      }

      /* Shutting down, need to change state before exiting Xipc. */
      (void)set_host_state( DOWN, ErrorMsg );
      send_network_status_to_monitor();

      /* pteipc_shutdown_single_instance_app(); */
      pteipc_shutdown_multiple_instance_app();
      MainProcessDone = 1;
   }
}


/******************************************************************************
 *
 *  NAME:         EndProcess
 *
 *  DESCRIPTION:  This is the function used to clean up when the app is
 *                being exited.  All clean up code goes in here.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void EndProcess()
{
   CHAR Buffer[100]  = "";

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         nccup_log_message( 2, 1, Buffer, "startup",0 );
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   nccup_log_message( 2, 1, Buffer,"EndProcess",0);
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}

/******************************************************************************
 *
 *  NAME:         STARTUP
 *
 *  DESCRIPTION:  This function performs required initialization when the
 *                service starts up.  It creates the shared memory table, gets
 *                NCF01, and may perform auto logon.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT startup()
{
   INT    ret_val = true;
   INT    rc;
   CHAR   Buffer[200]  = "";
   CHAR   err_buf[100] = "";

   sprintf( Buffer, "Network Controller %s was started.", ServiceName );
   nccup_log_message( 2, 1, Buffer, "startup",0 );

   /* Initialize the structure sizes so they
    * do not have to be recalculated every time.
    */
   Auth_Tx_Size = AUTH_TX_SIZE;
   Ncf01_Size   = NCF01_SIZE;
   Ncf30_Size   = NCF30_SIZE;
   Tlf01_Size   = TLF01_SIZE;

   Echo_Timer_Flag  = CUP_ECHO_TIMER_IS_CLEARED;
   Echo_Timer_Count = 0;

   memset( TimerAppData2, 0x00, sizeof(TimerAppData2) );   

   /*  Create a table to be used for shared memory between multiple
    *  instances of this Network Control Module.  It contains three 
    *  fields:  # of consecutive timeouts, # of active transactions, 
    *  and the current state of the host.
    */
   ret_val = create_shared_mem_table( err_buf );
   if ( true == ret_val )
   {
      /* Get NCF01 record for switched out transactions. */
      memset( &Ncf01_I, 0x00, Ncf01_Size );
      memset( Buffer, 0x00, sizeof(Buffer) );
      strcpy(Ncf01_I.primary_key.network_id, AppName);
      Ncf01_I.primary_key.network_type[0] = 'I';
      ret_val = get_ncf01_with_wait( &Ncf01_I, Buffer );
	  if ( false == ret_val )
      {
         strcpy(  err_buf, "Error on select of NCF01 issuer: " );
         strncat( err_buf,  Buffer, 50 );
      }
      else
      {
         /* Get NCF01 record for switched in transactions. */
         NetworkType = CUP_BOTH;
         memset( &Ncf01_A, 0x00, Ncf01_Size );
         memset( Buffer, 0x00, sizeof(Buffer) );
         strcpy(Ncf01_A.primary_key.network_id, AppName);
         Ncf01_A.primary_key.network_type[0] = 'A';
         ret_val = get_ncf01_with_wait( &Ncf01_A, Buffer);
		 if ( false == ret_val )
         {
            strcpy(  err_buf, "Error on select of NCF01 acquirer: " );
            strncat( err_buf,  Buffer, 50 );
         }
      }
   }

   if ( true == ret_val )
   {
      /* Perform some initialization */
      init_character_code_maps();

      /* Initialize transaction statistics */
      (void)reset_stats( &Txn_Counts );

      /* Initialize configurable counters. */
      MaxActiveTxns     = atoi(Ncf01_I.max_active_txns);
      MaxConsecTimeouts = atoi(Ncf01_I.max_consecutive_timeouts);

      /* Determine if transaction tracing should be turned on. */
      rc = get_trace_ini_info( AppName, DirPath, err_buf );
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
            sprintf( err_buf,
                    "Unable to open trace file %s. Tracing is off",
                     TraceFile );
            nccup_log_message( 2, 1, err_buf, "startup",0 );
         }
         else
            nccup_log_message( 2, 1, "Tracing is turned ON", "startup",0 );
      }
      else
      {
         Tracing = TRACE_OFF;
         if ( rc == -1 )
         {
            nccup_log_message( 2, 1, err_buf, "startup",0 );
         }
         nccup_log_message( 2, 1, "Tracing is turned off", "startup",0 );
      }

      /* Check the 'Auto Logon' flag. */

	  /*

			Need to updated this function
	  */
      if (0 == strcmp(Ncf01_I.auto_logon,"1"))
      {
         /* Issue a LOGON request to the host. */
		 /*We Need TO Write new Signon message for key exchnage */
         issue_req_08X0( CUP_ECHO );
      }
      (void)send_network_status_to_monitor();
   }
   else
   {
      sprintf( Buffer, "Unable to startup %s.  %s.", AppName, err_buf );
      nccup_log_message( 2, 3, Buffer, "Startup",0 );
   }

   nccup_read_TO_RC_from_tf_ini();
   nccup_read_fai_RC_from_tf_ini();
   return( ret_val );
}




