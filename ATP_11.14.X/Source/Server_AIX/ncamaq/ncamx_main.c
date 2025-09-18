/******************************************************************************

   nceqmain.c

   Copyright (c) 2005, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  Amex Host Handler Driver

   This module is the host handler for the Equitable host.  It
   will process all messages destined to, or originated from,
   the Equitable host.

   APPLICATION:  Advanced Transaction Processor (ATP)

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
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"
#include "txutils.h"
#include "txtrace.h"
#include "diskio.h"


extern INT  EndProcessSignalled;
extern INT  MainProcessDone;
extern CHAR ServiceName[12];


/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

/* QUEUES */
CHAR  AppName[80];
CHAR  updateds_que_name[]   = "updatdsA";
CHAR  trands_que_name []    = "trandsA";
CHAR  netds_que_name []     = "netdsA";
CHAR  timerds_que_name []   = "timerdsA";
CHAR  nsp_que_name []       = "racalA";
CHAR  authorizer_que_name[] = "txcntlA";
CHAR  applnk_que_name []    = "applnkA";
CHAR  oprmon_que_name[]     = "oprmonI" ;
CHAR  dialog_que_name []    = "dialogA";
CHAR  Amex_Atm_que_name []  = "ncatmaA";

/* STRUCTURES */
AUTH_TX              Auth_Tx;
NCF01                Ncf01_I;
NCF01                Ncf01_A;
NCF30                Ncf30;
CRF01                Crf01;
BDKKEY01			 Bdkkey01;
AMEX_ONLINE_UPDATES  Online_Updates;

/* STRUCTURE SIZES */
INT  Auth_Tx_Size;
INT  Ncf01_Size;
INT  Ncf30_Size;
INT  Tlf01_Size;
INT  Crf01_Size;
INT  Bdk01_Size;

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
CHAR   KeyChange[DE48_SIZE+1];

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

CHAR   NMI_Code[4];
INT    invalid_msg_format;
CHAR   Version[] = "ATP_11.13.5";

//ramya
CHAR  ncamaq_Erro_warning_Filename[256] = {0};
CHAR  ncamaq_module_error_warning_file_name[256] = {0};
CHAR  ncamaq_error_warning_file_path[256] = {0};
							  
#if AMEX03B_APRIL2024
CHAR acq_amex_variable_auth_flag;
char last_date[9]={0};
#endif
							  

/******************************************************************************
 *
 *  NAME:         ascendent_msg_handler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                host handler.  It will transfer control to the appropriate
 *                module, based on message type.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Ram Malathkar
 *
 ******************************************************************************/
void ascendent_msg_handler( pPTE_MSG p_msg_in )
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

      case MT_INCOMING: 

         /* Message(request/response) from the Equitable host */
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

      case MT_NSP_DECRYPT_TRANSLATE_PIN_RESPONSE:

    	 process_decrypt_pin_response( p_msg_in );
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
                  ncamaq_log_message( 1, 1, buffer, "ascendent_msg_handler",0 );
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Network Controller Service: %s, version %s",
                     ServiceName, Version );
            ncamaq_log_message( 1, 1, buffer, "ascendent_msg_handler" ,0);
         }
      break;

      case MT_SYSTEM_REPLY: 
         PRINT( "MT_SYSTEM_REPLY  \n" );
      break;

      case MT_INCOMING_GUI:
         /* GUI is initiating an 1304 message for Amex: Online File Update. */
         (void)process_file_update_request( p_msg_in );
      break;

      default :
         sprintf( buffer, "Unknown msg type received: %d", (INT)msgtype );
         ncamaq_log_message( 1, 2, buffer, "ascendent_msg_handler",0 );
      break;
   }  /* switch msg_type */

   return;
} /* ascendent_msg_handler */


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
   ncamaq_log_message( 2, 1,buffer, "MainProcessor",0);
   PRINT2( "%s\n", buffer );
   ncamaq_get_error_warning_file_name_path();
   get_variable_auth_config_details();

   if( !pteipc_init_multiple_instance_app( AppName, ServiceName, xipc_instance ))
   {
      ncamaq_log_message( 1, 3, "Failed to create XIPC queues.", "MainProcessor",0 );
      PRINT( "Failed to create XIPC queues\n" );
      pteipc_shutdown_multiple_instance_app();
      MainProcessDone = 1;
   }
   else
   {
      /* Perform any startup initialization. */
      if ( false == startup() )
      {
         pteipc_shutdown_multiple_instance_app();
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
               ascendent_msg_handler( p_msg );
               free( p_msg );
            }
            else if ( QUE_ER_TIMEOUT != ret_code ) 
            {
               pteipc_get_errormsg( ret_code, ErrorMsg );
               sprintf( buffer, "Error on pteipc_receive %s", ErrorMsg );
               ncamaq_log_message( 1, 3, buffer, "MainProcessor",0 );
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
         ncamaq_log_message( 1, 1, Buffer, "startup",0 );
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   ncamaq_log_message( 2, 1,Buffer, "EndProcess",0);
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
   ncamaq_log_message( 1, 1, Buffer, "startup",0 );

   /* Initialize the structure sizes so they
    * do not have to be recalculated every time.
    */
   Auth_Tx_Size = AUTH_TX_SIZE;
   Ncf01_Size   = NCF01_SIZE;
   Ncf30_Size   = NCF30_SIZE;
   Tlf01_Size   = TLF01_SIZE;
   Bdk01_Size   = BDKKEY01_SIZE;

   Echo_Timer_Flag  = ECHO_TIMER_IS_CLEARED;
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
         NetworkType = BOTH;
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
         memset( &Bdkkey01, 0x00, Bdk01_Size );
         memset( Buffer, 0x00, sizeof(Buffer) );
         strcpy(Bdkkey01.card_brand, "AMEX");
         ret_val = get_bdkkey01_with_wait( &Bdkkey01, Buffer);
         if ( false == ret_val )
         {
            strcpy(  err_buf, "Error on select of BDKKEY01 table: " );
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
            ncamaq_log_message( 1, 1, err_buf, "startup",0 );
         }
         else
            ncamaq_log_message( 1, 1, "Tracing is turned ON", "startup",0 );
      }
      else
      {
         Tracing = TRACE_OFF;
         if ( rc == -1 )
         {
            ncamaq_log_message( 1, 1, err_buf, "startup",0 );
         }
         ncamaq_log_message(1, 1, "Tracing is turned off", "startup",0 );
      }

      /* Check the 'Auto Logon' flag. */
      if (0 == strcmp(Ncf01_I.auto_logon,"1"))
      {
         /* Issue a LOGON request to the host. */
         issue_1804( ECHO );
      }
      (void)send_network_status_to_monitor();
   }
   else
   {
      sprintf( Buffer, "Unable to startup %s.  %s.", AppName, err_buf );
      ncamaq_log_message( 1, 3, Buffer, "Startup" ,0);
   }

   return( ret_val );
}

