/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCEQ0800.C
 *  
 * TITLE:       Equitable Network Controller Network Management Processor
 *  
 * DESCRIPTION: This module contains functions that handle network
 *              management requests and responses, switched in and
 *              switched out.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq0800.c  $
   
      Rev 1.10   Mar 01 2004 08:28:58   dirby
   Modified to use new definitions that differentiate between nceqit and ncmlnk.
   SCR 1352
   
      Rev 1.9   Feb 17 2003 15:02:20   dirby
   Modified to support single, double, and triple length keys in
   DE48 for dynamic key change commands.
   SCR 955
   
      Rev 1.8   Sep 17 2002 15:34:50   dirby
   Moved include of memmnger.h below equitdb.h to allow for a
   clean compile.
   SCR 877
   
      Rev 1.7   Aug 02 2002 15:35:22   dirby
   Removed code so time outs of echo requests do not increment
    the timeout counter.  This is so we do not go offline due to too
   many echo request time outs.  Only increment the timeout
   counter for timeouts of financial transactions.
   SCR 594
   
      Rev 1.6   Jul 26 2002 13:25:54   dirby
   Modified to allow 0800 echo requests to go out when host
   state is Offline due to Maximum Time Outs.  Will go back Online
   when a response to the echo request is received.
   SCR 594
   
      Rev 1.5   Jul 07 2002 02:38:34   dirby
   Reimplemented going offline when max time outs occur.
   SCR 594
   
      Rev 1.4   Jan 14 2002 18:14:42   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.3   Dec 14 2001 14:53:06   dirby
   Added some nceqit_log_message calls for some error messages
   that were being formed, but not logged.
   SCR 87
   
      Rev 1.2   Dec 10 2001 14:08:12   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.1   Aug 17 2001 12:49:30   dirby
   Added line of code to only issue Key Changes is 'PAD_CHAR' 
   in the network definition issuing record is enabled.
   
      Rev 1.0   Aug 06 2001 09:47:52   dirby
   Initial revision.
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
#include "nceq_constants.h"
#include "nceq_prototypes.h"
#include "nceq_bitfields.h"
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
extern INT  Atm01_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

/* Network:  nceqit or ncmlnk */
extern INT  Network;

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

extern CHAR   TimerAppData2[];

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
      nceqit_log_message( 1, 2, err_buf, "process_monitor_request",0 );
   }

   if ( send_request == true )
   {
      (void)issue_0800( nmi_type );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ISSUE_0800
 *
 *  DESCRIPTION:  This function sends a network management request to the host.
 *                It does whatever it needs to for each particular type of
 *                network management request.  This includes displaying an
 *                informational message on Monitor.
 *
 *  INPUTS:       nm_type - Type of 0800 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_0800( INT nm_type )
{
   INT   ret_val = true;
   INT   seconds;
   CHAR  echo_req_timer_key[LOOKUP_KEY_SIZE]="";

   CHAR  err_buf[100]="";

   /* Read shared memory to get current state of host. */
   ret_val=  read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   if( true == ret_val)
   {
   Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
   strcpy( Auth_Tx.TLF01_details.message_type, "0800" );
   TranFlow = SWITCHED_OUT;

		if (nm_type == ECHO) {
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
      strcpy( TimerAppData2, TIMER_0800 );
      (void)create_echo_req_timer_key( echo_req_timer_key );
      (void)set_timer( echo_req_timer_key, seconds, REQUEST_TIMER );
		} else if (nm_type == LOGON) {
			if (0 != strcmp( ONLINE, current_state)) {
         strcpy( err_buf, "Sending LOG ON request to host." );
         nceqit_log_message( 1, 1, err_buf, "issue_0800",0 );
         strcpy( NMI_Code, NMI_LOGON );
			} else {
         /* Already online.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already online.  Log ON request not sent." );
         nceqit_log_message( 1, 1, err_buf, "issue_0800" ,0);
         ret_val = false;
      }
		} else if (nm_type == LOGOFF) {
			if (0 != strcmp( OFFLINE, current_state)) {
         strcpy( err_buf, "Sending LOG OFF request to host." );
         nceqit_log_message( 1, 1, err_buf, "issue_0800",0 );
         strcpy( NMI_Code, NMI_LOGOFF );
			} else {
         /* Already offline.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already offline.  Log OFF request not sent." );
         nceqit_log_message( 1, 1, err_buf, "issue_0800",0 );
         ret_val = false;
      }
		} else if (nm_type == KEY_EXCHANGE) {
      /* Perform a change of Pin Key. */
			if (Ncf01_I.cwk[0] == 0x00) {
         /* We do not have a pin key.  Cannot continue with request. */
         strcpy( err_buf, "Unable to issue a key change.  CWK is empty." );
         nceqit_log_message( 1, 1, err_buf, "issue_0800",0 );
         ret_val = false;
			} else if (0 != strcmp( ONLINE, current_state)) {
         /* We are not online.  Cannot continue with key change request. */
				strcpy(err_buf,
						"Unable to issue a key change. Host not online.");
         nceqit_log_message( 1, 1, err_buf, "issue_0800",0 );
         ret_val = false;
			} else {
         /* Everything is ok.  Continue with key change request. */
         strcpy( err_buf, "Initiating key change . . ." );
         nceqit_log_message( 1, 1, err_buf, "issue_0800" ,1);
         strcpy( NMI_Code, NMI_KEYCHANGE );
      }
		} else {
      /* Received a network management type that is not supported here. */
      sprintf( err_buf,
              "Received request for unsupported 0800 request. Type = %d",
               nm_type );
      nceqit_log_message( 1, 2, err_buf, "issue_0800" ,0);
      ret_val = false;
   }

		if (ret_val == true) {
      /* Issue the 0800 request. */

      /* Step 1.  Build the data fields. */
      if ( false == build_0800_msg(err_buf) )
         nceqit_log_message( 1, 2, err_buf, "issue_0800",1 );

      /* Step 2.  Put host message together - build bitmap. */
      else if ( false == build_host_msg(err_buf) )
         nceqit_log_message( 1, 2, err_buf, "issue_0800",1 );

      /* Step 3.  Send request to host. */
      else if ( false == send_08nn_to_host(err_buf) )
         nceqit_log_message( 1, 2, err_buf, "issue_0800",1 );
   }
	}
   else
   {
	   sprintf(err_buf,"Unable to read from shared memory.Shutting down the service");
	   	nceqit_log_message(1, 2, err_buf, "issue_0800", 0);
	   	shut_down();
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ISSUE_0810
 *
 *  DESCRIPTION:  This function sends a network management response to the
 *                host. It displays a message on Monitor (except for echoes),
 *                then sends the response out.
 *
 *  INPUTS:       nm_type - Type of 0810 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT issue_0810( INT nm_type )
{
   INT   ret_val = false;
   CHAR  err_buf[100]="";

   if ( nm_type == LOGON )
   {
      nceqit_log_message( 1, 1, "Sending Log ON response to host.", "issue_0810",0 );
   }
   else if ( nm_type == LOGOFF )
   {
      nceqit_log_message( 1, 1, "Sending Log OFF response to host.", "issue_0810",0 );
   }

   /* Issue the 0810 response. */
   set_tx_key_to_response();

   /* Step 1.  Build the data fields. */
   if ( false == build_0810_msg(err_buf) )
      nceqit_log_message( 1, 2, err_buf, "issue_0810",0 );

   /* Step 2.  Put host message together - build bitmap. */
   else if ( false == build_host_msg(err_buf) )
      nceqit_log_message( 1, 2, err_buf, "issue_0810",0 );

   /* Step 3.  Send request to host. */
   else if ( false == send_08nn_to_host(err_buf) )
      nceqit_log_message( 1, 2, err_buf, "issue_0810",0 );

   else
      ret_val = true;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_0800_REQUEST
 *
 *  DESCRIPTION:  This function handles an 0800 request from the host.
 *                It will update shared memory to reflect a change in host
 *                state; it will send an 0810 response to the host.
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
void process_0800_request()
{
   INT     ret_val = true;
   INT     type;
   CHAR    buffer[200];
   CHAR    err_buf[200]  = "";
   CHAR    new_state[3]  = "99";
   CHAR    same_state[3] = "99";

   /* Read shared memory to get current state of host. */
   ret_val=read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );
   if(true==ret_val)
   {
		if (0 == strcmp(NMI_LOGON, NMI_Code)) {
      type = LOGON;
      nceqit_log_message( 1, 1, "Received Log ON request from host.",
                  "process_0800_request",1 );
      if ( 0 == strcmp(ONLINE, current_state) )
				nceqit_log_message(1, 1, "Already online.",
						"process_0800_request", 1);
      else
         strcpy( new_state, ONLINE );
		} else if (0 == strcmp(NMI_LOGOFF, NMI_Code)) {
      type = LOGOFF;
      nceqit_log_message( 1, 1, "Received Log OFF request from host.",
                  "process_0800_request" ,1);
			if (0 == strcmp(OFFLINE, current_state)) {
				nceqit_log_message(1, 1, "Not online.", "process_0800_request",
						1);
         ret_val = false;
			} else
         strcpy( new_state, OFFLINE );
		} else if (0 == strcmp(NMI_ECHO, NMI_Code)) {
      /* Only do echoes if online. */
      type = ECHO;
      if ( 0 != strcmp(ONLINE, current_state) )
         ret_val = false;
		} else {
      /* If NMI code is anything else, we do not support it. */
      ret_val = false;
      if ( 0 == strcmp(NMI_KEYCHANGE, NMI_Code) )
				nceqit_log_message(1, 1,
						"Key Change request from host not supported.",
                     "process_0800_request" ,1);

      else if ( 0 == strcmp(NMI_CUTOFF, NMI_Code) )
				nceqit_log_message(1, 1,
						"Cutoff (0800) request from host not supported.",
                     "process_0800_request",1 );

      else if ( 0 == strcmp(NMI_SAF, NMI_Code) )
				nceqit_log_message(1, 1,
						"SAF (0800) request from host not supported.",
                     "process_0800_request",1 );

			else {
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Received unsupported 0800 request from host. NMI = %s",
                  NMI_Code );
         nceqit_log_message( 1, 1, buffer, "process_0800_request" ,1);
      }
   }

   /* Continue to process any 0800 msg that is supported. */
		if (ret_val == true) {
      /* Send an 0810 response to host. */
			if ( true == issue_0810(type)) {
				if (0 != strcmp(new_state, same_state)) {
            /* Host state has changed.  Update shared memory. */
					if ( false == set_host_state(new_state, err_buf)) {
               memset(buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "Unable to update host state. " );
               strcat( buffer, err_buf );
						nceqit_log_message(1, 2, buffer, "process_0800_request",
								1);
					}
				}
            }
         }
      }
   else
   {
	   sprintf(err_buf,"Unable to process 0800 request. Problem with reading shared memory.Shutting down the service");
	   	nceqit_log_message(1, 2, err_buf, "process_0800_request", 0);
	   	shut_down();


   }
   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_0810_RESPONSE
 *
 *  DESCRIPTION:  This function handles an 0810 response from the host.
 *                It updates shared memory if a change is warranted. If
 *                bringing the host online, then a keychange is initiated
 *                (this is only a requirement for nceqit - EB's host).
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
void process_0810_response()
{
   INT     ret_val = true;
   CHAR    buffer[200]  = "";
   CHAR    err_buf[200] = "";
   CHAR    echo_req_timer_key[LOOKUP_KEY_SIZE]="";

   /* Populate this field for statistical purposes. */
   strcpy( Auth_Tx.TLF01_details.response_code, APPROVED );

   if ( 0 == strcmp(NMI_LOGON, NMI_Code) )
   {
      /* Read shared memory to get current state of host. */
	   ret_val=  read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );
	   if(true==ret_val)
      {
			if (0 != strcmp(ONLINE, current_state)) {
				nceqit_log_message(1, 1,
						"Received Log ON response. Setting state online.",
                     "process_0810_response",1 );

				if ( true == set_host_state( ONLINE, err_buf)) {
            /* We just went online.  If nceqit, issue key change request. */
					if (Network == NCEQIT) {
               /* Only issue key change if it is enabled. */
						if (Ncf01_I.pad_char[0] == KEY_CHANGE_ENABLED) {
							nceqit_log_message(1, 1,
									"Issuing 0800 Key Change message.",
                              "process_0810_response",1 );
                  issue_0800( KEY_EXCHANGE );
               }
            }
				} else {
            strcpy( buffer, "Unable to update host state to online. " );
            strcat( buffer, err_buf );
					nceqit_log_message(1, 2, buffer, "process_0810_response",
							1);
         }
      }

      /* This field may have been overwritten on the Key Change.
       * Need for updating txn statistics.
       */
      Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   }
	   else
	   {
		   sprintf(err_buf,"Unable to process NMI_LOGON. Problem with reading shared memory.Shutting down the service");
		   	nceqit_log_message(1, 2, err_buf, "process_0810_response", 0);
		   	shut_down();

	   }
   }
   else if ( 0 == strcmp(NMI_LOGOFF, NMI_Code) )
   {
      /* Read shared memory to get current state of host. */
	   ret_val=  read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

	   if(true==ret_val)
      {
			if (0 != strcmp(OFFLINE, current_state)) {
				nceqit_log_message(1, 1,
						"Received Log OFF response. Setting state offline.",
                     "process_0810_response",1 );

				if ( false == set_host_state( OFFLINE, err_buf)) {
            strcpy( buffer, "Unable to update host state to offline. " );
            strcat( buffer, err_buf );
					nceqit_log_message(1, 2, buffer, "process_0810_response",
							1);
         }
      }

		}
	   else
	   {

		   sprintf(err_buf,"Unable to process NMI_LOGOFF. Problem with reading shared memory.Shutting down the service");
		   	nceqit_log_message(1, 2, err_buf, "process_0810_response", 0);
		   	shut_down();
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
         nceqit_log_message(1, 2, err_buf, "process_0810_response",1 );
      }

      /* If OFFLINE, set state to ONLINE. */
      /* -------------------------------- */

      /* Read shared memory to get current state of host. */
      ret_val=  read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );
      if(true==ret_val)
      {
			if (0 == strcmp(OFFLINE, current_state)) {
				nceqit_log_message(1, 1,
						"Received response to echo. Going back Online",
                     "process_0810_response",1 );

				if ( false == set_host_state( ONLINE, err_buf)) {
            strcpy( buffer, "Unable to update host state to online. " );
            strcat( buffer, err_buf );
					nceqit_log_message(1, 2, buffer, "process_0810_response",
							1);
				}
			}
         }
      else
      {
		   sprintf(err_buf,"Unable to process NMI_ECHO. Problem with reading shared memory.Shutting down the service");
		   	nceqit_log_message(1, 2, err_buf, "process_0810_response", 0);
		   	shut_down();

      }

   }
   else if ( 0 == strcmp(NMI_KEYCHANGE, NMI_Code) )
   {
      nceqit_log_message( 1, 1,
                  "Received response to key change.  ATP is in sync with host.",
                  "process_0810_response" ,1);
   }
   else
   {
      strcpy( Auth_Tx.TLF01_details.response_code, INVALID_TXN );
      sprintf( buffer,
              "Received response to unsupported network management message. NMI=%s,STAN=%s",
               NMI_Code, Auth_Tx.TLF01_details.sys_trace_audit_num );

      nceqit_log_message( 1, 2, buffer, "process_0810_response" , 1);
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
      process_0800_request();

   else if ( Auth_Tx.TLF01_details.message_type[2] == '1' )
      process_0810_response();

   else
   {
      memset( buffer, 0x00, sizeof(buffer) );
      sprintf( buffer,
              "Unsupported net management msg from host. Msgtype = %s",
               Auth_Tx.TLF01_details.message_type );
      nceqit_log_message( 1, 2, buffer, "process_net_management_from_host",1 );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         BUILD_0800_MSG
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 0800 request.
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
INT build_0800_msg( pCHAR err_buf )
{
   INT     ret_val = true;
   INT     DE48_len;
   INT     key_len;
   CHAR    temp[4];
   CHAR    time_date[25];
   CHAR    date_str[9];
   CHAR    time_str[7];

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   /* FIELD 7 - TRANSMISSION DATE AND TIME (LOCAL TIME) */

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Now format field 7: MMDDhhmmss. */
   memcpy( HOST_MESSAGE.gmt_time, date_str+4, 4 );
   memcpy( HOST_MESSAGE.gmt_time+4, time_str, 6 );


   /* FIELD 11 - SYSTEM TRACE AUDIT NUMBER */
   create_stan( HOST_MESSAGE.stan );


   /* FIELD 15 - SETTLEMENT DATE */

   /* This field is used only for cutoff, which we do not support.  */
   /* memcpy( HOST_MESSAGE.date_settle, "0000", 4 ); */


   /* FIELD 48 - Key Change Information */
   if ( 0 == strcmp( NMI_KEYCHANGE, NMI_Code ) )
   {
      /*+-------------------------------------------------------------+
        | This is a key change.  Here is the format.                  |
        | ------------------------------------------------------------|
        | Length field of 3 bytes.  Value should be 30.               |
        | Length Receiving Institution: Pos 1->4: Not used            |
        | Length Sending Institution:   Pos 5->8: Not used            |
        | Key index:                    Pos 9:   value 1 or not used  |
        | Key index:                    Pos 10:  value 1 or not used  |
        | New Comm Key:                 Pos 11->nn: computed by switch|
        | Comm Key Check digits:        Pos nn+1->nn+4: not used      |
        +-------------------------------------------------------------+*/

      key_len = strlen( Ncf01_I.cwk );
      switch( key_len )
      {
         case 32:  DE48_len = 46;               break;
         case 48:  DE48_len = 62;               break;
         default:  DE48_len = 30; key_len = 16; break;
      }

      memset( temp, 0x00, sizeof(temp) );
      sprintf( temp, "%03d", DE48_len );
      memcpy( HOST_MESSAGE.addl_data_len, temp, 3 );
      memset( HOST_MESSAGE.addl_data+8,  '1',   2 );
      memcpy( HOST_MESSAGE.addl_data+10,  Ncf01_I.cwk, key_len );
   }


   /* FIELD 70 - NETWORK MANAGERMENT INFORMATION CODE */
   memcpy( HOST_MESSAGE.nmi_code, NMI_Code, 3 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         BUILD_0810_MSG
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 0810 response.
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
INT build_0810_msg( pCHAR err_buf )
{
   INT     ret_val = true;

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   /* FIELD 7 - TRANSMISSION DATE AND TIME (LOCAL TIME) */
   memcpy( HOST_MESSAGE.gmt_time,
           Auth_Tx.TLF01_details.transmission_timestamp,
           10 );


   /* FIELD 11 - SYSTEM TRACE AUDIT NUMBER */
   memcpy( HOST_MESSAGE.stan,
           Auth_Tx.TLF01_details.sys_trace_audit_num,
           6 );

   /* FIELD 39 - RESPONSE CODE */

   /* Populate this field for logging and statistical purposes only.
    * It does not get sent to the host.
    */
   strcpy( Auth_Tx.TLF01_details.response_code, APPROVED );

   /* FIELD 70 - NETWORK MANAGERMENT INFORMATION CODE */
   memcpy( HOST_MESSAGE.nmi_code, NMI_Code, 3 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SEND_08NN_TO_HOST
 *
 *  DESCRIPTION:  This function sends an 0800/0810 message to the host.
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
INT send_08nn_to_host( pCHAR err_buf )
{
   INT   ret_val;
   CHAR  buffer[100]="";

   ret_val = send_message_to_host( err_buf,true );//praneeth added true as parameter

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
   INT   ret_val = true;
   /* Request from Monitor to send status information */
   ret_val= read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );
  if(true==ret_val)
  {
   pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg));
   memset( &ncf01, 0,        Ncf01_Size );
   memcpy( &ncf01, pCurrent, Ncf01_Size );
   strcpy( ncf01.status,     current_state );
 
   p_msg_query = ptemsg_build_msg( MT_SYS_MONITOR,
                                   ST1_NETWORK_UPDATE, 
		ST2_NONE, applnk_que_name, application_que_name, (pBYTE) &ncf01,
				sizeof(ncf01), 0);
		if ( NULL_PTR == p_msg_query) {
			nceqit_log_message(1, 3,
					"Unable to build PTE message for Monitor query",
                   "process_monitor_query" ,0);
		} else {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
			if (ConnectionId[0] != 0) {
         ptemsg_set_orig_comms_info( p_msg_query, ConnectionId) ;
         if ( false == send_msg( p_msg_query, applnk_que_name, err_buf ))
					nceqit_log_message(1, 3, err_buf, "process_monitor_query",
							0);
      }
      free( p_msg_query );
   }
	}
  else
  {
	   sprintf(err_buf,"Unable to process monitor query. Problem with reading shared memory");
	   	nceqit_log_message(1, 2, err_buf, "process_monitor_query", 0);
	   	shut_down();
  }
   return;
}

