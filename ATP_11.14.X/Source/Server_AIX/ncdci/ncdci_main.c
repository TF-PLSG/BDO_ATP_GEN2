/******************************************************************************
   *
   * Copyright (c) 2002, Hypercom, Inc.
   * All Rights Reserved.
   *
   * MODULE:	  ncdci_main.c
   *  
   * TITLE: 	  ncdci Network Controller Database Processor
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
#include "diners.h"
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
#include "ncdci_prototypes.h"



extern INT  EndProcessSignalled;
extern INT  MainProcessDone;
extern CHAR ServiceName[12];
extern char dci_scheme_timeout_rc [4];
extern char dci_scheme_fai_rc [4];
/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

/* QUEUES */
CHAR  AppName[80] = {0};
CHAR  updateds_que_name[]   			= "updatdsA";
CHAR  trands_que_name []    			= "trandsA";
CHAR  netds_que_name []   				= "netdsA";
CHAR  timerds_que_name []   			= "timerdsA";
CHAR  nsp_que_name []       			= "racalA";
CHAR  authorizer_que_name[] 			= "txcntlA";
CHAR  applnk_que_name []    			= "applnkA";
CHAR  oprmon_que_name[]    				= "oprmonI" ;
CHAR  dialog_que_name []    			= "dialogA";
CHAR  ncdci_issuing_link_name[]  		= "ncdci";
CHAR  ncdci_fileupdate_link_que_name[]  = "ncdci3A";
CHAR  pos_handler_module_name[]         = "dcp";
CHAR  voice_handler_que_name[]          = "VOICE";
CHAR  sbatch_handler_que_name[]         = "sbat";
CHAR  cadencie_dci_queue_name[]			= "ncdci2";
CHAR  ncdci2_fileupdate_link_que_name[] = "ncdci2A";

/* STRUCTURES */
AUTH_TX              Auth_Tx;
NCF01                Ncf01_I;
NCF01                Ncf01_I_key_exchange;
NCF01                Ncf01_A;
NCF01                Ncf01_ncdci_I;
NCF30                Ncf30;
NCF01_KEYBLOCK     Ncf01_Keyblock;
NCF01_KEYBLOCK     Ncf01_ncdci_I_keyblock;
NCF01_KEYBLOCK     Ncf01_I_key_exchange_Keyblock;
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
CHAR   static_Rtdcwk_Key[33] = {0};
CHAR   key_check_value[5] = {0};
CHAR   MAC[17] = {0};
CHAR   ncdci_Erro_warning_Filename[256] = {0};
CHAR   ncdci_module_error_warning_file_name[256] = {0};
CHAR   ncdci_error_warning_file_path[256] = {0};
CHAR   static_Rtdcwk_Key_Keyblock[33] = {0};

CHAR   Version[] = "ATP_11.12.1";

INT    keyblock_enable_flag_for_DCI = 0;
CHAR   MAC_Keyblock[17] = {0};
							   
/******************************************************************************
 *
 *  NAME:         ncdci_ncdci1_PinnacleMsgHandler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into ncdci2
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
void ncdci_PinnacleMsgHandler( pPTE_MSG p_msg_in )
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
    	  ncdci_process_request_for_host( p_msg_in );
         break;

	 case MT_AUTH_RESPONSE: 
     	 /* Response coming in via the authorizer */
	    (void)ncdci_process_incoming_message(p_msg_in);
		 break;

      case MT_INCOMING: 

         /* Message(request/response) from the BDO Cadencie host */
    	  ncdci_process_host_msg( p_msg_in );
         break;

      case MT_DB_REPLY:

         /* Message coming from the data server */
    	  ncdci_process_db_reply( p_msg_in );
         break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE: 

         /* Response from the network security processor */
    	  ncdci_process_translate_pin_response( p_msg_in );
         break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE_KEYBLOCK:

		  /* Response from the network security processor */
		  ncdci_process_translate_pin_response_Keyblock( p_msg_in );
		  break;

	  case MT_NSP_DINERS_GENERATE_KEY_RESPONSE:
	  	 /* Response from the network security processor */
		  ncdci_key_generate_response( p_msg_in );
		  break;

	  case MT_NSP_DINERS_GENERATE_KEY_RESPONSE_KEYBLOCK:
			 /* Response from the network security processor */
			  ncdci_key_generate_response_keyblock( p_msg_in );
			  break;
	
      case MT_TIMER_REPLY:
         subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
         if ( subtype1 == ST1_TIMER_CLEAR )
         {
            /* This is a response from timerds to a clear timer request.
             * Continue processing the transaction.
             */
        	 ncdci_process_clear_timer_reply( p_msg_in );
         }
         break;

      case MT_TIMER_TIMEOUT: 

         /* One of the timers expired. */
    	  ncdci_process_timeout_msg( p_msg_in );
         break;

      case MT_SYS_MONITOR:

         /* Process a request from Monitor. */
    	  ncdci_process_monitor_request( p_msg_in );
         break;  

      case MT_GET_STATS:

         /* Send transaction counts to whoever requested them. */
         (void)ncdci_send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:

         /* Reset transaction counts to zero. */
         (void)ncdci_reset_transaction_statistics( p_msg_in );
         break;
		 
		case MT_NSP_DINERS_KEY_RESPONSE:

			if((0 != strncmp(AppName,"ncdci3",6)))
			 {
				 ncdci_process_key_request_response(p_msg_in);
			 }
			 else
			 {
				 strcpy( buffer, "%s: Invalid module" );
				  ncdci_log_message( 1, 1, buffer, "PingMsgHandler", 0 );
			 }
			break;

		case MT_NSP_DINERS_KEY_RESPONSE_KEYBLOCK:

			 if((0 != strncmp(AppName,"ncdci3",6)))
			 {
				 ncdci_process_key_request_response_keyblock(p_msg_in);
			 }
			 else
			 {
				 strcpy( buffer, "%s: Invalid module" );
				 ncdci_log_message( 1, 1, buffer, "PingMsgHandler", 0 );
			 }
			break;
      case MT_SYSTEM_REQUEST:
         ptesystm_system_msg_handler( p_msg_in );
		 process_encryption_flag( p_msg_in );
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            if ( Tracing == DINERS_TRACE_ON )
            {
               /* Flush the trace file buffer when service is pinged. */
               if ( false == flush_file(ppTraceFile_fp) )
               {
                  Tracing = DINERS_TRACE_OFF;
                  sprintf( buffer,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     AppName );
                  ncdci_log_message( 1, 1, buffer, "PingMsgHandler", 0 );
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Network Controller Service: %s, version %s",
                     ServiceName, Version );
            ncdci_log_message( 1, 1, buffer, "PingMsgHandler", 0 );
         }
		 if(ST1_LOAD_DINERS3DS2_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			ncdci_load_DCI3DS2_records_for_DCI();
		 	send_trace_response(p_msg_in);
		 	sprintf( buffer, "DINERS3DS2 records reloaded for DINERS" );
		 	ncdci_log_message( 3, 1,buffer, "PinnacleMsgHandler", 0 );
		 }
         else if(ST1_LOAD_TO_RC_VALUES == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 ncdci_read_TO_rc_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "dci_scheme_timeout_rc: %s",
					  dci_scheme_timeout_rc);
        	 ncdci_log_message( 1, 1, buffer, "PinnacleMsgHandler" ,0);
         }
         else if(ST1_LOAD_RC_FOR_FAI == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 ncdci_read_fai_rc_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "dci_scheme_fai_rc: %s",
					  dci_scheme_fai_rc);
        	 ncdci_log_message( 1, 1, buffer, "PinnacleMsgHandler" ,0);
         }
         else if(ST1_SYS_KEYBLOCK_ON == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 keyblock_enable_flag_for_DCI = KEYBLOCK_ENABLE;
        	 ncdci_set_keyblock_flag_details("1");
        	 ncdci_get_keyblock_flag_details();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "Keyblock Flag is set to ON");
        	 ncdci_log_message( 3, 1, buffer, "PinnacleMsgHandler" ,0);
         }
         else if(ST1_SYS_KEYBLOCK_OFF == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 keyblock_enable_flag_for_DCI = KEYBLOCK_DISABLE;
        	 ncdci_set_keyblock_flag_details("0");
        	 ncdci_get_keyblock_flag_details();
        	 send_trace_response(p_msg_in);
        	 sprintf( buffer,
        	          "Keyblock Flag is set to OFF");
        	 ncdci_log_message( 3, 1, buffer, "PinnacleMsgHandler" ,0);
         }
      break;

      case MT_SYSTEM_REPLY: 
         PRINT( "MT_SYSTEM_REPLY  \n" );
      break;

      case MT_INCOMING_GUI:
         /* GUI is initiating an 1304 message for ncdci: Online File Update. */
         (void)ncdci_process_file_update_request( p_msg_in );
      break;

      default :
         sprintf( buffer, "Unknown msg type received: %d", (INT)msgtype );
         ncdci_log_message( 1, 2, buffer, "PingMsgHandler", 0 );
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void MainProcessor()
{
   pPTE_MSG  p_msg = NULL_PTR;
   LONG      ret_code = 0;
   CHAR      ErrorMsg[100] = {0};
   CHAR      buffer[256]   = {0};
   CHAR      xipc_instance[30] = {0};

   GetAppName( AppName );
   GetXipcInstanceName( xipc_instance );

   sprintf( buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   ncdci_log_message( 2, 1, buffer,"MainProcessor",0);
   PRINT2( "%s\n", buffer );

   ncdci_get_error_warning_file_name_path();
   if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) ) 
   {
      ncdci_log_message( 2, 3, "Failed to create XIPC queues.", "MainProcessor", 0 );
      PRINT( "Failed to create XIPC queues\n" );

	  pteipc_shutdown_single_instance_app(); 

	  MainProcessDone = 1;
   }
   else
   {
      /* Perform any ncdci_ncdci1_startup initialization. */
      if ( false == ncdci_startup() )
      {
	      pteipc_shutdown_single_instance_app(); 

    	  MainProcessDone = 1;
      }
      else
      {
         /**************
          * MAIN  LOOP *
          **************/
    	 ncdci_load_DCI3DS2_records_for_DCI();
    	 ncdci_read_TO_rc_from_tf_ini();
    	 ncdci_read_fai_rc_from_tf_ini();
    	 ncdci_get_keyblock_flag_details();
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
               ncdci_PinnacleMsgHandler( p_msg );
               free( p_msg );
            }
            else if ( QUE_ER_TIMEOUT != ret_code ) 
            {
               pteipc_get_errormsg( ret_code, ErrorMsg );
               sprintf( buffer, "Error on pteipc_receive %s", ErrorMsg );
               ncdci_log_message( 2, 3, buffer, "MainProcessor", 0 );
            }
         }
      }

      /* Shutting down, need to change state before exiting Xipc. */
      (void)ncdci_set_host_state( DOWN, ErrorMsg );
      ncdci_send_network_status_to_monitor();

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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void EndProcess()
{
   CHAR Buffer[100]  = {0};

   if ( Tracing == DINERS_TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         ncdci_log_message( 2, 2, Buffer, "ncdci_startup", 0 );
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   ncdci_log_message( 2, 1, Buffer,"EndProcess",0);
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}

/******************************************************************************
 *
 *  NAME:         ncdci_ncdci1_startup
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_startup()
{
   INT    ret_val = true;
   INT    rc = 0;
   CHAR   Buffer[200]  = {0};
   CHAR   err_buf[100] = {0};

   sprintf( Buffer, "Network Controller %s was started.", ServiceName );
   ncdci_log_message( 2, 1, Buffer, "ncdci_ncdci1_startup", 0 );

   /* Initialize the structure sizes so they
    * do not have to be recalculated every time.
    */
   Auth_Tx_Size = AUTH_TX_SIZE;
   Ncf01_Size   = NCF01_SIZE;
   Ncf30_Size   = NCF30_SIZE;
   Tlf01_Size   = TLF01_SIZE;

   Echo_Timer_Flag  = DINERS_ECHO_TIMER_IS_CLEARED;
   Echo_Timer_Count = 0;

   memset( TimerAppData2, 0x00, sizeof(TimerAppData2) );   

   /*  Create a table to be used for shared memory between multiple
    *  instances of this Network Control Module.  It contains three 
    *  fields:  # of consecutive timeouts, # of active transactions, 
    *  and the current state of the host.
    */
   ret_val = ncdci_create_shared_mem_table( err_buf );
   if ( true == ret_val )
   {
      /* Get NCF01 record for switched out transactions. */
      memset( &Ncf01_I, 0x00, Ncf01_Size );
      memset( &Ncf01_I_key_exchange, 0x00, Ncf01_Size );
      memset( &Ncf01_I_key_exchange_Keyblock, 0x00, NCF01_KEYBLOCK01_SIZE );
      memset( Buffer, 0x00, sizeof(Buffer) );
      strcpy(Ncf01_I.primary_key.network_id, AppName);
      Ncf01_I.primary_key.network_type[0] = 'I';
      ret_val = ncdci_get_ncf01_with_wait( &Ncf01_I, Buffer );
	  if ( false == ret_val )
      {
         strcpy(  err_buf, "Error on select of NCF01 issuer: " );
         strncat( err_buf,  Buffer, 50 );
      }
      else
      {
         /* Get NCF01 record for switched in transactions. */
         NetworkType = DINERS_BOTH;
         memset( &Ncf01_A, 0x00, Ncf01_Size );
         memset( Buffer, 0x00, sizeof(Buffer) );
         strcpy(Ncf01_A.primary_key.network_id, AppName);
         Ncf01_A.primary_key.network_type[0] = 'A';
         ret_val = ncdci_get_ncf01_with_wait( &Ncf01_A, Buffer);
		 if ( false == ret_val )
         {
            strcpy(  err_buf, "Error on select of NCF01 acquirer: " );
            strncat( err_buf,  Buffer, 50 );
         }
		  memset( &Ncf01_Keyblock, 0x00, NCF01_KEYBLOCK01_SIZE );
		  memset( Buffer, 0x00, sizeof(Buffer) );
		  strcpy(Ncf01_Keyblock.network_id, AppName);
		  Ncf01_A.primary_key.network_type[0] = 'I';
		  ret_val = ncdci_get_ncf01_keyblock_with_wait( &Ncf01_Keyblock, Buffer);
		  if ( false == ret_val )
		  {
			  strcpy(  err_buf, "Error on select of Ncf01_Keyblock Details: " );
			  strncat( err_buf,  Buffer, 50 );
		  }
      }
   }

   if ( true == ret_val )
   {
      /* Perform some initialization */
      ncdci_init_character_code_maps();

      /* Initialize transaction statistics */
      (void)reset_stats( &Txn_Counts );

      /* Initialize configurable counters. */
      MaxActiveTxns     = atoi(Ncf01_I.max_active_txns);
      MaxConsecTimeouts = atoi(Ncf01_I.max_consecutive_timeouts);

      /* Determine if transaction tracing should be turned on. */
      rc = get_trace_ini_info( AppName, DirPath, err_buf );
      if ( rc == 0 )
      {
         Tracing = DINERS_TRACE_ON;

         /* Open the transaction trace file. */
         memset( TraceFile, 0x00, sizeof(TraceFile) );

         pTraceFile_fp  = &TraceFile_fp;
         ppTraceFile_fp = &pTraceFile_fp;

         if (false == open_trace_file(AppName,DirPath,ppTraceFile_fp,TraceFile))
         {
            Tracing = DINERS_TRACE_OFF;
            sprintf( err_buf,
                    "Unable to open trace file %s. Tracing is off",
                     TraceFile );
            ncdci_log_message( 2, 2, err_buf, "ncdci_startup", 0 );
         }
         else
            ncdci_log_message( 2, 1, "Tracing is turned ON", "ncdci_startup", 0 );
      }
      else
      {
         Tracing = DINERS_TRACE_OFF;
         if ( rc == -1 )
         {
            ncdci_log_message( 2, 1, err_buf, "ncdci_startup", 0 );
         }
         ncdci_log_message( 2, 1, "Tracing is turned off", "ncdci_startup", 0 );
      }

      /* Check the 'Auto Logon' flag. */

	  /*

			Need to updated this function
	  */
      if (0 == strcmp(Ncf01_I.auto_logon,"1"))
      {
         /* Issue a LOGON request to the host. */
		 /*We Need TO Write new Signon message for key exchnage */
    	  ncdci_issue_req_08X4( DINERS_ECHO );
      }
      (void)ncdci_send_network_status_to_monitor();
   }
   else
   {
      sprintf( Buffer, "Unable to ncdci_startup %s.  %s.", AppName, err_buf );
      ncdci_log_message( 2, 3, Buffer, "Startup", 0 );
   }

   return( ret_val );
}

void ncdci_get_keyblock_flag_details( void )
{
	DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;
	keyblock_enable_flag_for_DCI = KEYBLOCK_DISABLE;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     /* points to section name       */
							 	  "KEY_BLOCK_FLAG_VALUE_DCI",  /* points to key name           */
								  "0",                        /* points to default string     */
								  ini_info,                   /* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      /* size of destination buffer   */
								  filename                    /* points to ini filename       */
	);

	keyblock_enable_flag_for_DCI = atoi(ini_info);
}

void ncdci_set_keyblock_flag_details( pCHAR flag )
{
	DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);


	rc = WritePrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     /* points to section name       */
							 	  "KEY_BLOCK_FLAG_VALUE_DCI",  	/* points to key name           */
								  flag, 						/* points to default string     */
								  filename                    	/* points to ini filename       */
	);

}
