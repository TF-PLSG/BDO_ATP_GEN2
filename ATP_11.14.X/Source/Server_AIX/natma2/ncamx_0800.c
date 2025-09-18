/***************************************************************************
 *
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCAMX_1804.C
 *  
 * TITLE:       Amex Network Controller Network Management Processor
 *  
 * DESCRIPTION: This module contains functions that handle network
 *              management requests and responses, and
 *              switched out.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Sree Velaga
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\ncjcb\ncamx_0800.c  $
   
      Rev 1.4   Sep 23 2003 17:38:30   svelaga
   1814 responses to Amex host - set response code to 800 instead of 000.
   
      Rev 1.3   Aug 28 2003 11:18:36   svelaga
   Timerkey is structure changed.
   For refunds, reversals - merchant_id ( first 8 char) are used as
   part of key.
   for rest of txns - terminal_id is used as part of key.
   
      Rev 1.2   Aug 27 2003 14:42:02   svelaga
   Pin traslation error fix.
   
      Rev 1.1   Aug 27 2003 09:05:02   svelaga
   Fixed issues with 1804 messages coming from host.
   Also added support for POS Sale, voids.
   
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "ptetime.h"
#include "equitdb.h"
#include "memmnger.h"
#include "nc_dbstruct.h"
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"
#include "ncamx_bitfields.h"
#include "txutils.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     buflen;
extern BYTE    ReqRsp_Buffer[];
extern INT     TranFlow;

/* QUEUES */
extern CHAR    dialog_que_name[];
extern CHAR    applnk_que_name[];

/* Database structures */
extern AUTH_TX    Auth_Tx;
extern NCF01      Ncf01_I;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Echo Timer Info */
extern INT    Echo_Timer_Flag;
extern INT    Echo_Timer_Count;

extern CHAR    TimerAppData2[];
extern CHAR  authorizer_que_name[];

/******************************************************************************
 *
 *  NAME:         PROCESS_MONITOR_REQUEST
 *
 *  DESCRIPTION:  This function is called when a network management request
 *                is made on the ATP Monitor: Log ON/OFF/Echo, etc.  It will
 *                call the function that issues the request to the host,
 *                passing it the appropriate network management type.
 *
 *  INPUTS:       p_msg - PTE message from Monitor
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_monitor_request( pPTE_MSG p_msg )
{
   INT   nmi_type;
   BYTE  nm_type;
   CHAR  err_buf[100];
   INT   send_request = true;

   nm_type = ptemsg_get_msg_subtype1( p_msg );
   if ( nm_type == ST1_LOGON )
      nmi_type = LOGON;
   else if ( nm_type == ST1_LOGOFF )
      nmi_type = LOGOFF;
   else if ( nm_type == ST1_ECHOTEST )
      nmi_type = ECHO;
   else if ( nm_type = ST1_NETWORK_QUERY )
   {
      process_monitor_query( p_msg );
      send_request = false;
   }
   else
   {
      send_request = false;
      memset( err_buf, 0x00, sizeof(err_buf) );
      sprintf( err_buf,
              "Unsupported request from Monitor. Subtype1 = %c",
               nm_type );
      natma2_log_message( 1, 2, err_buf, "process_monitor_request",0 );
   }

   if ( send_request == true )
   {
      (void)issue_1804( nmi_type );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ISSUE_1804
 *
 *  DESCRIPTION:  This function sends a network management request to the host.
 *                It does whatever it needs to for each particular type of
 *                network management request.  This includes displaying an
 *                informational message on Monitor.
 *
 *  INPUTS:       nm_type - Type of 1804 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_1804( INT nm_type )
{
   INT   ret_val = true;
   INT   seconds;
   CHAR  echo_req_timer_key[LOOKUP_KEY_SIZE]="";

   CHAR  err_buf[100]="";

   /* Read shared memory to get current state of host. */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
   strcpy( Auth_Tx.TLF01_details.message_type, "1804" );
   TranFlow = SWITCHED_OUT;

   if ( nm_type == ECHO )
   {
      /* Before sending Echo request, make sure
       * a bunch of them have not timed out.
       */
      Echo_Timer_Count++;

      /* Don't let this value get too high. */
      if ( Echo_Timer_Count > 999 )
         Echo_Timer_Count = 1;

      /* Set the NMI code in field 70 for building the transaction. */
      strcpy( NMI_Code, NMI_ECHO );

      /* Set Timer for Echoes. */
      seconds = atoi(Ncf01_I.request_timer);
      strcpy( TimerAppData2, TIMER_1804 );
      (void)create_echo_req_timer_key( echo_req_timer_key );
      (void)set_timer( echo_req_timer_key, seconds, REQUEST_TIMER );
   }
   else if ( nm_type == LOGON )
   {
      if ( 0 != strcmp( ONLINE, current_state ) )
      {
         strcpy( err_buf, "Sending LOG ON request to host." );
         natma2_log_message( 1, 1, err_buf, "issue_1804",0 );
         strcpy( NMI_Code, NMI_LOGON );
      }
      else
      {
         /* Already online.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already online.  Log ON request not sent." );
         natma2_log_message( 1, 1, err_buf, "issue_1804" ,0);
         ret_val = false;
      }
   }
   else if ( nm_type == LOGOFF )
   {
      if ( 0 != strcmp( OFFLINE, current_state ) )
      {
         strcpy( err_buf, "Sending LOG OFF request to host." );
         natma2_log_message( 1, 1, err_buf, "issue_1804",0 );
         strcpy( NMI_Code, NMI_LOGOFF );
      }
      else
      {
         /* Already offline.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already offline.  Log OFF request not sent." );
         natma2_log_message( 1, 1, err_buf, "issue_1804",0 );
         ret_val = false;
      }
   }
   else if ( nm_type == KEY_EXCHANGE )
   {
      /* Perform a change of Pin Key. */
      if ( Ncf01_I.cwk[0] == 0x00 )
      {
         /* We do not have a pin key.  Cannot continue with request. */
         strcpy( err_buf, "Unable to issue a key change.  CWK is empty." );
         natma2_log_message( 1, 1, err_buf, "issue_1804",0 );
         ret_val = false;
      }
      else if ( 0 != strcmp( ONLINE, current_state ) )
      {
         /* We are not online.  Cannot continue with key change request. */
         strcpy( err_buf, "Unable to issue a key change. Host not online." );
         natma2_log_message( 1, 1, err_buf, "issue_1804",0 );
         ret_val = false;
      }
      else
      {
         /* Everything is ok.  Continue with key change request. */
         strcpy( err_buf, "Initiating key change . . ." );
         natma2_log_message( 1, 1, err_buf, "issue_1804" ,0);
         strcpy( NMI_Code, NMI_KEYCHANGE );
      }
   }
   else
   {
      /* Received a network management type that is not supported here. */
      sprintf( err_buf,
              "Received request for unsupported 1804 request. Type = %d",
               nm_type );
      natma2_log_message( 1, 1, err_buf, "issue_1804",0 );
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Issue the 1804 request. */

      /* Step 1.  Build the data fields. */
      if ( false == build_1804_msg(err_buf) )
         natma2_log_message( 1, 2, err_buf, "issue_1804",0 );

      /* Step 2.  Put host message together - build bitmap. */
      else if ( false == build_host_msg(err_buf) )
         natma2_log_message( 1, 2, err_buf, "issue_1804",0 );

      /* Step 3.  Send request to host. */
      else if ( false == send_18nn_to_host(err_buf) )
         natma2_log_message( 1, 2, err_buf, "issue_1804" ,0);
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         issue_1814
 *
 *  DESCRIPTION:  This function sends a network management response to the
 *                host. It displays a message on Monitor (except for echoes),
 *                then sends the response out.
 *
 *  INPUTS:       nm_type - Type of 1814 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT issue_1814( INT nm_type )
{
   INT   ret_val = false;
   CHAR  err_buf[100]="";

   if ( nm_type == LOGON )
   {
      natma2_log_message( 1, 1, "Sending Log ON response to host.", "issue_1814",0 );
   }
   else if ( nm_type == LOGOFF )
   {
      natma2_log_message( 1, 1, "Sending Log OFF response to host.", "issue_1814",0 );
   }

   /* Issue the 1814 response. */
   set_tx_key_to_response();

   /* Step 1.  Build the data fields. */
   if ( false == build_1814_msg(err_buf) )
      natma2_log_message( 1, 2, err_buf, "issue_1814" ,0);

   /* Step 2.  Put host message together - build bitmap. */
   else if ( false == build_host_msg(err_buf) )
      natma2_log_message( 1, 2, err_buf, "issue_1814",0 );

   /* Step 3.  Send request to host. */
   else if ( false == send_18nn_to_host(err_buf) )
      natma2_log_message( 1, 2, err_buf, "issue_1814",0 );

   else
      ret_val = true;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_1804_REQUEST
 *
 *  DESCRIPTION:  This function handles an 1804 request from the host.
 *                It will update shared memory to reflect a change in host
 *                state; it will send an 1814 response to the host.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction info containing NMI Code
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_1804_request()
{
   INT     ret_val = true;
   INT     type;
   CHAR    buffer[200];
   CHAR    err_buf[200]  = "";
   CHAR    new_state[3]  = "99";
   CHAR    same_state[3] = "99";

   /* Read shared memory to get current state of host. */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   if ( 0 == strcmp(NMI_LOGON, NMI_Code) )
   {
      type = LOGON;
      natma2_log_message( 1, 1, "Received Log ON request from host.",
                  "process_1804_request" ,0);
      if ( 0 == strcmp(ONLINE, current_state) )
         natma2_log_message( 1, 1, "Already online.", "process_1804_request",0 );
      else
         strcpy( new_state, ONLINE );
   }
   else if ( 0 == strcmp(NMI_LOGOFF, NMI_Code) )
   {
      type = LOGOFF;
      natma2_log_message( 1, 1, "Received Log OFF request from host.",
                  "process_1804_request",0 );
      if ( 0 == strcmp(OFFLINE, current_state) )
         natma2_log_message( 1, 1, "Not online.", "process_1804_request",0 );
      else
         strcpy( new_state, OFFLINE );
   }
   else if ( 0 == strcmp(NMI_ECHO, NMI_Code) )
   {
      /* Only do echoes if online. */
      type = ECHO;
      if ( 0 != strcmp(ONLINE, current_state) )
         ret_val = false;
   }
   else
   {
      /* If NMI code is anything else, we do not support it. */
      ret_val = false;
      if ( 0 == strcmp(NMI_KEYCHANGE, NMI_Code) )
         natma2_log_message( 1, 1, "Key Change request from host not supported.",
                     "process_1804_request",0 );

      else if ( 0 == strcmp(NMI_CUTOFF, NMI_Code) )
         natma2_log_message( 1, 1, "Cutoff (1804) request from host not supported.",
                     "process_1804_request" ,0);

      else if ( 0 == strcmp(NMI_SAF, NMI_Code) )
         natma2_log_message( 1, 1, "SAF (1804) request from host not supported.",
                     "process_1804_request",0 );

      else
      {
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Received unsupported 1804 request from host. NMI = %s",
                  NMI_Code );
         natma2_log_message( 1, 1, buffer, "process_1804_request",0 );
      }
   }

   /* Continue to process any 1804 msg that is supported. */
   if ( ret_val == true )
   {
      /* Send an 1814 response to host. */
      if ( true == issue_1814( type ) )
      {
         if ( 0 != strcmp(new_state, same_state) )
         {
            /* Host state has changed.  Update shared memory. */
            if ( false == set_host_state( new_state, err_buf ) )
            {
               memset(buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "Unable to update host state. " );
               strcat( buffer, err_buf );
               natma2_log_message( 1, 2, buffer, "process_1804_request" ,0);
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         process_1814_request
 *
 *  DESCRIPTION:  This function handles an 1814 response from the host.
 *                It updates shared memory if a change is warranted. If
 *                bringing the host online, then a keychange is initiated
  *
 *  INPUTS:       Auth_Tx - (Global)Transaction info containing NMI Code
 *                AppName - (Global)Name of application
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_1814_request()
{
   INT     ret_val = true;
   CHAR    buffer[200]  = "";
   CHAR    err_buf[200] = "";
   CHAR    echo_req_timer_key[LOOKUP_KEY_SIZE]="";
   pPTE_MSG    p_msg_ins   = NULL_PTR;

   /* Populate this field for statistical purposes. */
   strcpy( Auth_Tx.TLF01_details.response_code, ISO_APPROVED );

   strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);
   

   if ( 0 == strcmp(NMI_LOGON, NMI_Code) )
   {
      /* Read shared memory to get current state of host. */
      read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

      if ( 0 != strcmp(ONLINE, current_state) )
      {
		  p_msg_ins = ptemsg_build_msg( MT_LOGON_RESPONSE,
                                              ST1_NONE, 
                                              ST2_NONE,
                                              application_que_name, 
                                              authorizer_que_name,
                                              (pBYTE)&Auth_Tx,
                                              sizeof(AUTH_TX), 
                                              0 );
    if ( NULL_PTR == p_msg_ins )
            {
				natma2_log_message(1, 1,"Error sending network status to Txcntl",
                                 "process_logon_response",0);
            }
		   ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		   if ( ret_val < 0 )
		   {
			   pteipc_get_errormsg( ret_val, err_buf );
               sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
               natma2_log_message(1, 1,buffer,"process_logon_response",0);
		   }

         natma2_log_message( 1, 1, "Received Log ON response. Setting state online.",
                     "process_1814_request" ,0);

         if ( true == set_host_state( ONLINE, err_buf ) )
         {
            /* We just went online.  If nceqit, issue key change request. */
            if ( 0 == strcmp( AppName, "nceqit" ) )
            {
               /* Only issue key change if it is enabled. */
               if ( Ncf01_I.pad_char[0] == KEY_CHANGE_ENABLED )
               {
                  natma2_log_message( 1, 1, "Issuing 1804 Key Change message.",
                              "process_1814_request",0 );
                  issue_1804( KEY_EXCHANGE );
               }
            }
         }
         else
         {
            strcpy( buffer, "Unable to update host state to online. " );
            strcat( buffer, err_buf );
            natma2_log_message( 1, 2, buffer, "process_1814_request" ,0);
         }
      }

      /* This field may have been overwritten on the Key Change.
       * Need for updating txn statistics.
       */
      Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   }
   else if ( 0 == strcmp(NMI_LOGOFF, NMI_Code) )
   {
      /* Read shared memory to get current state of host. */
      read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

      if ( 0 != strcmp(OFFLINE, current_state) )
      {
         natma2_log_message( 1, 1, "Received Log OFF response. Setting state offline.",
                     "process_1814_request" ,0);

         if ( false == set_host_state( OFFLINE, err_buf ) )
         {
            strcpy( buffer, "Unable to update host state to offline. " );
            strcat( buffer, err_buf );
            natma2_log_message( 1, 2, buffer, "process_1814_request",0 );
         }
      }
   }
   else if ( 0 == strcmp(NMI_ECHO, NMI_Code) )
   {
      Echo_Timer_Count = 0;

      /* Clear the request timer. */
      create_echo_req_timer_key( echo_req_timer_key );
      if ( false == clear_request_timer(echo_req_timer_key) )
      {
         strcpy(err_buf, "Unable to clear echo request on response from host.");
         natma2_log_message(1, 2, err_buf, "process_1814_request",0 );
      }

      /* If OFFLINE, set state to ONLINE. */
      /* -------------------------------- */

      /* Read shared memory to get current state of host. */
      read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

      if ( 0 == strcmp(OFFLINE, current_state) )
      {
		  p_msg_ins = ptemsg_build_msg( MT_LOGON_RESPONSE,
                                              ST1_NONE, 
                                              ST2_NONE,
                                              application_que_name, 
                                              authorizer_que_name,
                                              (pBYTE)&Auth_Tx,
                                              sizeof(AUTH_TX), 
                                              0 );
    if ( NULL_PTR == p_msg_ins )
            {
				natma2_log_message(1, 1,"Error sending network status to Txcntl",
                                 "process_logon_response",0);
            }
		   ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		   if ( ret_val < 0 )
		   {
			   pteipc_get_errormsg( ret_val, err_buf );
               sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
               natma2_log_message(1, 1,buffer,"process_logon_response",0);
		   }

         natma2_log_message( 1, 1, "Received response to echo. Going back Online",
                     "process_1814_request",1 );

         if ( false == set_host_state( ONLINE, err_buf ) )
         {
            strcpy( buffer, "Unable to update host state to online. " );
            strcat( buffer, err_buf );
            natma2_log_message( 1, 2, buffer, "process_1814_request" ,1);
         }
      }

   }
   else if ( 0 == strcmp(NMI_KEYCHANGE, NMI_Code) )
   {
      natma2_log_message( 1, 1,
                  "Received response to key change.  ATP is in sync with host.",
                  "process_1814_request",1 );
   }
   else
   {
      strcpy( Auth_Tx.TLF01_details.response_code, INVALID_TXN );
      sprintf( buffer,
              "Received response to unsupported network management message. NMI=%s,STAN=%s",
               NMI_Code, Auth_Tx.TLF01_details.sys_trace_audit_num );

      natma2_log_message( 1, 2, buffer, "process_1814_request",1 );
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_NET_MANAGEMENT_FROM_HOST
 *
 *  DESCRIPTION:  This function handles network management (08nn) messages
 *                from the host.  These can either be requests or responses.
 *                This function decides which, and calls the appropriate
 *                handler.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction info containing message type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_net_management_from_host()
{
   CHAR  buffer[100];

   if ( Auth_Tx.TLF01_details.message_type[2] == '0' )
      process_1804_request();

   else if ( Auth_Tx.TLF01_details.message_type[2] == '1' )
      process_1814_request();

   else
   {
      memset( buffer, 0x00, sizeof(buffer) );
      sprintf( buffer,
              "Unsupported net management msg from host. Msgtype = %s",
               Auth_Tx.TLF01_details.message_type );
      natma2_log_message( 1, 2, buffer, "process_net_management_from_host" ,1);
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         BUILD_1804_MSG
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 1804 request.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_1804_msg( pCHAR err_buf )
{
   INT		ret_val = true;
   CHAR		time_date[25];
   CHAR		date_str[9];
   CHAR		time_str[7];
   INT		len = 0;
   CHAR		temp_len[3]="";

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   /* FIELD 3 - PROCESSING CODE : 000000 */
		memcpy(HOST_MESSAGE.proccode,"000000",DE3_SIZE);

   /* FIELD 11 - SYSTEM TRACE AUDIT NUMBER */
		create_stan( HOST_MESSAGE.stan );

   /* FIELD 12 - DATE AND TIME (LOCAL TIME) */

		/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
		ptetime_get_timestamp( time_date );

		/* Reformat to get date "YYYYMMDD" and time "hhmmss" */
		get_date( time_date, date_str );
		get_time( time_date, time_str );

		/* Now format field 12: YYMMDDhhmmss. */
		memcpy( HOST_MESSAGE.datetime, date_str+2, 6 );
		memcpy( HOST_MESSAGE.datetime+6, time_str, 6 );

	/* FIELD 24 - FUNCTION CODE*/
		memcpy(HOST_MESSAGE.func_cd,NMI_Code,DE24_SIZE);

	/* FIELD 33 - FORWARDING INSTITUTION IDENTIFICATION CODE */
	
	/* FIELD 33 - FORWARDING INSTITUTION IDENTIFICATION CODE */
	
		len = 11;
		sprintf( temp_len, "%02d", len );
		memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, 2 );

		/*
		memcpy(HOST_MESSAGE.fwd_inst_id,"000000",6);

		memcpy(HOST_MESSAGE.fwd_inst_id,Auth_Tx.TLF01_details.forwarding_institution_id,5);
		*/

		if(Auth_Tx.forwarding_inst_id_cd[0] != 0x00)
			memcpy(HOST_MESSAGE.fwd_inst_id,Auth_Tx.forwarding_inst_id_cd,MAX_DE33_SIZE);
		else
			memcpy(HOST_MESSAGE.fwd_inst_id,Ncf01_I.forwarding_inst_id_code,MAX_DE33_SIZE);

   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         BUILD_1814_MSG
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 1814 response.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_1814_msg( pCHAR err_buf )
{
   INT     ret_val = true;
   INT		len = 0;
   CHAR		temp_len[3]="";


   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   /* FIELD 3 - PROCESSING CODE : 000000 */
		memcpy(HOST_MESSAGE.proccode,"000000",DE3_SIZE);

   /* FIELD 11 - SYSTEM TRACE AUDIT NUMBER */
		memcpy( HOST_MESSAGE.stan,
						 Auth_Tx.TLF01_details.sys_trace_audit_num,
						DE11_SIZE );

   /* FIELD 12 - DATE AND TIME (LOCAL TIME) */
	  
      memcpy( HOST_MESSAGE.datetime,
               &Auth_Tx.TLF01_details.date_yyyymmdd[2],
              6);
	  memcpy( &HOST_MESSAGE.datetime[6],
              Auth_Tx.TLF01_details.time_hhmmss,
              6);

	
	/* FIELD 24 - FUNCTION CODE*/
		memcpy(HOST_MESSAGE.func_cd,NMI_Code,DE24_SIZE);

	/* FIELD 33 - FORWARDING INSTITUTION IDENTIFICATION CODE */
	
		len = MAX_DE33_SIZE;
		sprintf( temp_len, "%02d", len );
		memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, 2 );

	/*	memcpy(HOST_MESSAGE.fwd_inst_id,"000000",6);*/

	
		memcpy(HOST_MESSAGE.fwd_inst_id,Auth_Tx.forwarding_inst_id_cd,MAX_DE33_SIZE);


   /* FIELD 39 - RESPONSE CODE */

   /* Populate this field for logging and statistical purposes only.
    * It does not get sent to the host.
    */
	strcpy( HOST_MESSAGE.responsecd, NETMGMT_APPROVED );


   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SEND_08NN_TO_HOST
 *
 *  DESCRIPTION:  This function sends an 1804/1814 message to the host.
 *                It is the same as sending any message to the host, but
 *                the error message is modified a little bit to include
 *                transmission timestamp.  This is because other identifying
 *                information like card number, TID & MID, are not present
 *                in an 08nn message.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message if an error occurs
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_18nn_to_host( pCHAR err_buf )
{
   INT   ret_val;
   CHAR  buffer[100]="";

   ret_val = send_message_to_host( err_buf,true );//praneeth added true

   if ( ret_val == false )
   {
      sprintf( buffer,
              "%s  DE7 = %s.",
               err_buf,
               Auth_Tx.TLF01_details.transmission_timestamp );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         PROCESS_MONITOR_QUERY
 *
 *  DESCRIPTION:  This function answers the call of the Monitor by sending
 *                its host state (online, offline) back to the Monitor.
 *
 *  INPUTS:       p_msg - PTE message from Monitor
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_monitor_query( pPTE_MSG p_msg )
{
   pPTE_MSG    p_msg_query = NULL_PTR;
   NCF01       ncf01;
   CHAR        err_buf[200]="";
   CHAR        Buff[256]="";
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0};
   XINT        ReturnCode = 0 ;
   pBYTE       pCurrent;

   /* Request from Monitor to send status information */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );
   pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg));
   memset( &ncf01, 0,        Ncf01_Size );
   memcpy( &ncf01, pCurrent, Ncf01_Size );
   strcpy( ncf01.status,     current_state );
 
   p_msg_query = ptemsg_build_msg( MT_SYS_MONITOR,
                                   ST1_NETWORK_UPDATE, 
                                   ST2_NONE,
                                   applnk_que_name, 
                                   application_que_name,
                                   (pBYTE)&ncf01,
                                   sizeof(ncf01), 
                                   0 );
   if ( NULL_PTR == p_msg_query )
   {
      natma2_log_message( 1, 3, "Unable to build PTE message for Monitor query",
                   "process_monitor_query",0 );
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         ptemsg_set_orig_comms_info( p_msg_query, ConnectionId) ;
         if ( false == send_msg( p_msg_query, applnk_que_name, err_buf ))
            natma2_log_message( 1, 3, err_buf, "process_monitor_query",0 );
      }
      free( p_msg_query );
   }
   return;
}

