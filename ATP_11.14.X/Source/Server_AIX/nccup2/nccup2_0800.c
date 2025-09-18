/***************************************************************************
 *
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup2_0800.C
 *  
 * TITLE:       CUP Network Controller Network Management Processor
 *  
 * DESCRIPTION: This module contains functions that handle network
 *              management requests and responses, and
 *              switched out.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Abhishek Verma
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
#include "cup.h"
#include "nccup2_prototypes.h"
#include "nccup2_bitfields.h"
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

/*Flags*/
extern INT Txn_Is_A_Reject;
extern INT extendedbit;

extern CHAR  LOGON_NMI_KEY_RESET_REQUEST[5];
extern CHAR  static_Rtdcwk_Key[39];
extern CHAR  security_que_name[];
CHAR   Verify_command_code[17]="";
extern CHAR    MAC[17];

char masked_Key_value[39]={0};

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

   strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
   nm_type = ptemsg_get_msg_subtype1( p_msg );
   if ( nm_type == ST1_LOGON )
      nmi_type = CUP_LOGON;
   else if ( nm_type == ST1_LOGOFF )
      nmi_type = CUP_LOGOFF;
   else if ( nm_type == ST1_ECHOTEST )
      nmi_type = CUP_ECHO;
   
   else if ( nm_type == ST1_CUP_KEY_SEND)
	 {
		  nmi_type = CUP_KEY_EXCHANGE;
	 }
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
      nccup2_log_message( 1, 2, err_buf, "process_monitor_request",0 );
   }

   if ( send_request == true )
   {
      (void)issue_req_08X0( nmi_type );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         issue_req_08X0
 *
 *  DESCRIPTION:  This function sends a network management request to the host.
 *                It does whatever it needs to for each particular type of
 *                network management request.  This includes displaying an
 *                informational message on Monitor.
 *
 *  INPUTS:       nm_type - Type of 08X0 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_req_08X0( INT nm_type )
{
   INT   ret_val = true;
   INT   seconds;
   CHAR  echo_req_timer_key[LOOKUP_KEY_SIZE]="";

   CHAR  err_buf[100]="";

   /* Read shared memory to get current state of host. */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
   strncpy( Auth_Tx.TLF01_details.message_type, "0820" ,4);
   TranFlow = SWITCHED_OUT;

   if ( nm_type == CUP_ECHO )
   {
      /* Before sending Echo request, make sure a bunch of them have not timed out. */
      Echo_Timer_Count++;

      /* Don't let this value get too high. */
      if ( Echo_Timer_Count > 999 )
         Echo_Timer_Count = 1;

      /* Set the NMI code in field 70 for building the transaction. */
      strcpy( NMI_Code, CUP_NMI_ECHO );

      /* Set Timer for Echoes. */
      seconds = atoi(Ncf01_I.request_timer);
      strcpy( TimerAppData2, TIMER_08X0 );
      (void)create_echo_req_timer_key( echo_req_timer_key );
      (void)set_timer( echo_req_timer_key, seconds, REQUEST_TIMER );
   }
   else if ( nm_type == CUP_LOGON )
   {
      if ( 0 != strcmp( ONLINE, current_state ) )
      {
         strcpy( err_buf, "Sending LOG ON request to host." );
         nccup2_log_message( 1, 1, err_buf, "issue_req_0820",0 );
         strcpy( NMI_Code, CUP_NMI_LOGON );
      }
      else
      {
         /* Already online.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already online.  Log ON request not sent." );
         nccup2_log_message( 1, 1, err_buf, "issue_req_0820",0 );
         ret_val = false;
      }
   }
   else if ( nm_type == CUP_LOGOFF )
   {
      if ( 0 != strcmp( OFFLINE, current_state ) )
      {
         strcpy( err_buf, "Sending LOG OFF request to host." );
         nccup2_log_message( 1, 1, err_buf, "issue_req_0820",0 );
         strcpy( NMI_Code, CUP_NMI_LOGOFF );
      }
      else
      {
         /* Already offline.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already offline.  Log OFF request not sent." );
         nccup2_log_message( 1, 1, err_buf, "issue_req_0820",0 );
         ret_val = false;
      }
   }
   else if ( nm_type == CUP_KEY_EXCHANGE )
   {
	  strcpy( Auth_Tx.TLF01_details.message_type, "0800" );
	  /* Send  Key. */
	  strcpy( NMI_Code, CUP_NMI_KEYCHANGE );
	  Get_Key_from_ncf01_table();
	  if(0!=strlen(static_Rtdcwk_Key))
	  {
		  perform_host_key_send();
		  nccup2_log_message( 1, 1, "Sending Key to Host", "issue_req_0800",0 );
		  return true;
	  }
	  else
	  {
		nccup2_log_message( 1, 3, "Cound not able to retrive Key from database",
							"issue_req_0800",0 );
	  }
   }
   else
   {
      /* Received a network management type that is not supported here. */
      sprintf( err_buf,
              "Received request for unsupported request. Type = %d",
               nm_type );
      nccup2_log_message( 1, 2, err_buf, "issue_req_08X0",0 );
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Issue the 08X0 request. */

      /* Step 1.  Build the data fields. */
      if ( false == build_08X0_msg_request(err_buf , nm_type) )
         nccup2_log_message( 1, 2, err_buf, "issue_req_08X0",0 );

      /* Step 2.  Put host message together - build bitmap. */
      else if ( false == build_host_msg(err_buf) )
         nccup2_log_message( 1, 2, err_buf, "issue_req_08X0",0 );

      /* Step 3.  Send request to host. */
      else if ( false == send_18nn_to_host(err_buf) )
         nccup2_log_message( 1, 2, err_buf, "issue_req_08X0",0 );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         issue_rsp_08X0
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
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
INT issue_rsp_08X0( INT nm_type )
{
   INT   ret_val = false;
   CHAR  err_buf[100]="";

   if ( nm_type == CUP_LOGON )
   {
      nccup2_log_message( 1, 1, "Sending Log ON response to host.",
    		  	  	  	  	  "issue_rsp_0820",0 );
   }
   else if ( nm_type == CUP_LOGOFF )
   {
      nccup2_log_message( 1, 1, "Sending Log OFF response to host.",
    		  	  	  	  	  "issue_rsp_0820",0 );
   }

   /* Issue the 08X0 response. */
   set_tx_key_to_response();

   /* Step 1.  Build the data fields. */
   if ( false == build_08X0_msg_response(err_buf,nm_type ) )
      nccup2_log_message( 1, 2, err_buf, "issue_rsp_08X0",0 );

   /* Step 2.  Put host message together - build bitmap. */
   else if ( false == build_host_msg(err_buf) )
      nccup2_log_message( 1, 2, err_buf, "issue_rsp_08X0",0 );

   /* Step 3.  Send request to host. */
   else if ( false == send_18nn_to_host(err_buf) )
      nccup2_log_message( 1, 2, err_buf, "issue_rsp_08X0",0 );

   else
      ret_val = true;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         process_08X0_request
 *
 *  DESCRIPTION:  This function handles an 08X0 request from the host.
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
void process_08X0_request()
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

   if ( 0 == strcmp(CUP_NMI_LOGON, NMI_Code) )
   {
      type = CUP_LOGON;
      nccup2_log_message( 1, 1, "Received Log ON request from host.",
                  	  	  	  "process_0820_request",0 );
      if ( 0 == strcmp(ONLINE, current_state) )
         nccup2_log_message( 1, 1, "Already online.",
        		 	 	 	 	 "process_0820_request",0 );
      else
         strcpy( new_state, ONLINE );
   }
   else if ( 0 == strcmp(CUP_NMI_LOGOFF, NMI_Code) )
   {
      type = CUP_LOGOFF;
      nccup2_log_message( 1, 1, "Received Log OFF request from host.",
                  	  	  	  	  "process_0820_request",0 );
      if ( 0 == strcmp(OFFLINE, current_state) )
         nccup2_log_message( 1, 1, "Not online.", "process_0820_request",0 );
      else
         strcpy( new_state, OFFLINE );
   }
   else if ( 0 == strcmp(CUP_NMI_ECHO, NMI_Code) )
   {
      /* Only do echoes if online. */
      type = CUP_ECHO;
      if ( 0 != strcmp(ONLINE, current_state) )
         ret_val = false;
   }
   else
   {
      /* If NMI code is anything else, we do not support it. */
      ret_val = false;
      if ( 0 == strcmp(CUP_NMI_KEYCHANGE, NMI_Code) )
         nccup2_log_message( 1, 2, "Key Change request from host not supported.",
                     	 	 	 "process_08X0_request",0 );

      else if ( 0 == strcmp(CUP_NMI_STR_DATE_SWITCH , NMI_Code) )
         nccup2_log_message( 1, 1, "Unionpay starts date switch (0820) request from host not supported.",
                     	 	 	 "process_08X0_request",0 );

      else if ( 0 == strcmp(CUP_NMI_END_DATE_SWITCH , NMI_Code) )
         nccup2_log_message( 1, 1, "Unionpay end date switch (0820) request from host not supported.",
        		 	 	 	 	 "process_08X0_request",0 );
	  
	  else if ( 0 == strcmp(CUP_STND_IN_AUTH_ECHO , NMI_Code) )
			   nccup2_log_message( 1, 1, "Stand in Echo test (0820) request from host not supported.",
						   	   	   "process_08X0_request",0 );
	   else if ( 0 == strcmp(CUP_STND_IN_AUTH_TRA_ACT  , NMI_Code) )
			   nccup2_log_message( 1, 1, "Stand in Authorization Tra Transmission active (0820) request from host not supported.",
						   	   	   "process_08X0_request",0 );
	   else if ( 0 == strcmp(CUP_STND_IN_PARA_UPD , NMI_Code) )
			   nccup2_log_message( 1, 1, "Stand in Parameters (0820) request from host not supported.",
						   	   	   "process_08X0_request",0 );
      else
      {
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Received unsupported request from host. NMI = %s",
                  NMI_Code );
         nccup2_log_message( 1, 2, buffer, "process_08X0_request",0 );
      }
   }

   /* Continue to process any 08X0 msg that is supported. */
   if ( ret_val == true )
   {
      /* Send an 0820 response to host. */
      if ( true == issue_rsp_08X0( type ) )
      {
         if ( 0 != strcmp(new_state, same_state) )
         {
            /* Host state has changed.  Update shared memory. */
            if ( false == set_host_state( new_state, err_buf ) )
            {
               memset(buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "Unable to update host state. " );
               strcat( buffer, err_buf );
               nccup2_log_message( 1, 3, buffer, "process_0820_request",0 );
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         process_08X0_response
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
void process_08X0_response()
{
   INT     ret_val = true;
   CHAR    buffer[200]  = {0};
   CHAR    err_buf[200] = {0};
   CHAR    echo_req_timer_key[LOOKUP_KEY_SIZE] = {0};
   pPTE_MSG    p_msg_ins   = NULL_PTR;

   /* Populate this field for statistical purposes. */
   if(Txn_Is_A_Reject == false)
   {
   		strcpy( Auth_Tx.TLF01_details.response_code, CUP_ISO_APPROVED );
   }

   strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);
   

   if ( 0 == strcmp(CUP_NMI_LOGON, NMI_Code) )
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
					nccup2_log_message(1, 3,"Error sending network status to Txcntl",
									 	 "process_0820_response",0);
				}
			   ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
			   if ( ret_val < 0 )
			   {
				   pteipc_get_errormsg( ret_val, err_buf );
				   sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
				   nccup2_log_message(1, 3,buffer,"process_0820_response",0);
			   }

			 nccup2_log_message( 1, 1, "Received Log ON response. Waiting for Key reset transaction",
						 	 	 "process_0820_response",0 );
      }

      /* This field may have been overwritten on the Key Change.
       * Need for updating txn statistics.
       */
      Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   }
   else if ( 0 == strcmp(CUP_NMI_LOGOFF, NMI_Code) )
   {
      /* Read shared memory to get current state of host. */
      read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

      if ( 0 != strcmp(OFFLINE, current_state) )
      {
         nccup2_log_message( 1, 1, "Received Log OFF response. Setting state offline.",
                     	 	 "process_0820_response",0 );

         if ( false == set_host_state( OFFLINE, err_buf ) )
         {
            strcpy( buffer, "Unable to update host state to offline. " );
            strcat( buffer, err_buf );
            nccup2_log_message( 1, 3, buffer, "process_0820_response",0 );
         }
      }
   }
   else if ( 0 == strcmp(CUP_NMI_ECHO, NMI_Code) )
   {
      Echo_Timer_Count = 0;

      /* Clear the request timer. */
      create_echo_req_timer_key( echo_req_timer_key );
      if ( false == clear_request_timer(echo_req_timer_key) )
      {
         strcpy(err_buf, "Unable to clear echo request on response from host.");
         nccup2_log_message(2, 2, err_buf, "process_0820_response",0 );
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
				nccup2_log_message(1, 3,"Error sending network status to Txcntl",
                                 	 "process_0820_response",0);
            }
		   ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		   if ( ret_val < 0 )
		   {
			   pteipc_get_errormsg( ret_val, err_buf );
               sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
               nccup2_log_message(1, 3,buffer,"process_0820_response",1);
		   }

         nccup2_log_message( 1, 1, "Received response to echo",
                     	 	 "process_0820_response",0 );

         /* For CUP link to go online, Key exchange sequence is must.
         if ( false == set_host_state( ONLINE, err_buf ) )
         {
            strcpy( buffer, "Unable to update host state to online. " );
            strcat( buffer, err_buf );
            nccup2_log_message( 3, 2, buffer, "process_0820_response" );
         }*/
      }

   }
   else if ( 0 == strcmp(CUP_NMI_KEYCHANGE, NMI_Code) )
   {
      nccup2_log_message( 1, 1, "Received response to key change request",
                  	  	  	  	  "process_0820_response",0 );
      if(0 == strncmp(Auth_Tx.TLF01_details.response_code,"00",2))
      {
    	  if ( false == set_host_state( ONLINE, err_buf ) )
    	  {
    		  strcpy( buffer, "Key exchange successful, unable to update host state to ONLINE. " );
    		  strcat( buffer, err_buf );
    		  nccup2_log_message( 1, 3, buffer, "process_0820_response",0 );
    	  }
    	  else
    	  {
    		  strcpy( buffer, "Key exchange successful, host state to ONLINE. " );
    		  strcat( buffer, err_buf );
    		  nccup2_log_message( 1, 1, buffer, "process_0820_response",0 );
    	  }
      }
      else
      {
    	  if ( false == set_host_state( OFFLINE, err_buf ) )
		  {
			  strcpy( buffer, "Key exchange failed. Unable to update host state to OFFLINE. " );
			  strcat( buffer, err_buf );
			  nccup2_log_message( 1, 3, buffer, "process_0820_response",0 );
		  }
		  else
		  {
			  strcpy( buffer, "Key exchange failed, host state to ONLINE. " );
			  strcat( buffer, err_buf );
			  nccup2_log_message( 1, 3, buffer, "process_0820_response",0 );
		  }
      }
   }
   else
   {
      strcpy( Auth_Tx.TLF01_details.response_code, CUP_INVALID_TXN );
      sprintf( buffer,
              "Received response to unsupported network management message. NMI=%s,STAN=%s",
               NMI_Code, Auth_Tx.TLF01_details.sys_trace_audit_num );

      nccup2_log_message( 1, 2, buffer, "process_08X0_response",0 );
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
 *  AUTHOR:      Abhishek
 *
 ******************************************************************************/
void process_net_management_from_host()
{
   CHAR  buffer[100];

   if ( Auth_Tx.TLF01_details.message_type[2] == '0' || Auth_Tx.TLF01_details.message_type[2] == '2')
      process_08X0_request();

   else if ( Auth_Tx.TLF01_details.message_type[2] == '1' || Auth_Tx.TLF01_details.message_type[2] == '3' )
      process_08X0_response();

   else
   {
      memset( buffer, 0x00, sizeof(buffer) );
      sprintf( buffer,
              "Unsupported net management msg from host. Msgtype = %s",
               Auth_Tx.TLF01_details.message_type );
      nccup2_log_message( 1, 2, buffer, "process_net_management_from_host",0 );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         BUILD_080X_MSG
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 08X0 request.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information, network Cmd
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_08X0_msg_request( pCHAR err_buf , int cmd)
{
   INT		ret_val = true;
   CHAR		time_date[25];
   CHAR		date_str[9];
   CHAR		time_str[7];
   INT		len = 0;
   CHAR		temp_len[3]="";
   INT     result_code = 0;  /* 0 = no error, 1 = error */

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   if(0 == strcmp(Auth_Tx.TLF01_details.message_type, "0820"))
	{
		  // Build 7
		 (void)build_request_field_7( &result_code, err_buf );
  		  // Build 11
		 create_stan( HOST_MESSAGE.stan );
		  // Build 33
		 (void)build_request_field_33( &result_code, err_buf );
		 //Build 53
  		 if(0 == strncmp(NMI_Code,"101", 3))
		 {
			memcpy(HOST_MESSAGE.sec_cntl,"1600000000000000" ,CUP_DE53_SIZE);
		 }
		 else
		 {
	 		memcpy(HOST_MESSAGE.sec_cntl,"0000000000000000" ,CUP_DE53_SIZE);
		 }
  		  // Build 70
		 memcpy( HOST_MESSAGE.net_manag_info_code,NMI_Code,3);
	}
  return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_08X0_msg_response
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 08X0 response.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information , Network Cmd
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_08X0_msg_response( pCHAR err_buf, int nm_type )
{
   INT     ret_val = true;
   INT		len = 0;
   CHAR		temp_len[3]="";
   INT	   result_code = 0;  /* 0 = no error, 1 = error */


   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   if((nm_type == CUP_LOGON)|| (nm_type == CUP_LOGOFF)|| (nm_type == CUP_ECHO))
	{
		  // Build 7
		 (void)build_request_field_7( &result_code, err_buf );
  		  // Build 11
		 (void)build_request_field_11( &result_code, err_buf );
		  // Build 39
		 (void)build_request_field_33( &result_code, err_buf );
  		  // Build 70
		 (void)build_request_field_70( &result_code, err_buf );
		  // Build 100
		 (void)build_request_field_100( &result_code, err_buf );
	}
	else if((nm_type == CUP_KEY_EXCHANGE))
	{
		// Build 7
		(void)build_request_field_7( &result_code, err_buf );
		// Build 11
		(void)build_request_field_11( &result_code, err_buf );
		// Build 39
		(void)build_request_field_39( &result_code, err_buf );
		// Build 53
		(void)build_request_field_53( &result_code, err_buf );
		// Build 70
		(void)build_request_field_70( &result_code, err_buf );
		// Build 100
		(void)build_request_field_100( &result_code, err_buf );
		// Build 128
		(void)build_request_field_128( &result_code, err_buf );
	}
	//strcpy( HOST_MESSAGE.responsecd, NETMGMT_APPROVED );


   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SEND_08NN_TO_HOST
 *
 *  DESCRIPTION:  This function sends an 08X0/1814 message to the host.
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
 *  AUTHOR:       Abhishek Verma
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
      nccup2_log_message( 2, 3, "Unable to build PTE message for Monitor query",
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
            nccup2_log_message( 2, 3, err_buf, "process_monitor_query",0 );
      }
      free( p_msg_query );
   }
   return;
}

void perform_host_key_send()
{
	CHAR	 err_buf[100] = {0};
	CHAR     Receiving_ID[100] = {0};
    INT     len = 0;
	INT 	result_code = 0; 
	INT     i=0;
	CHAR    temp_mess_sec_code[17]="0000000000000000";
	CHAR    key_binary_val[17] = {0};
	CHAR    key_msg[256] = {0};

	strncpy(Auth_Tx.TLF01_details.message_type, "0800",4);
	Auth_Tx.TLF01_details.tx_key=AUTH_PIN_CHANGE;
	strncpy(Receiving_ID,"27240608",8);

	// Build 7
	(void)build_request_field_7( &result_code, err_buf );
  	// Build 11
	create_stan( HOST_MESSAGE.stan );
	//Build 48
	len=18;// 2 byte for NK and 16 byte for key
	sprintf(HOST_MESSAGE.addl_data_len, "%03d", len);
	strncpy( HOST_MESSAGE.addl_data,"NK",2);
	ascii_to_bin_with_length (static_Rtdcwk_Key,32,key_binary_val , 16);
	Mask_Keys(static_Rtdcwk_Key);
	sprintf(key_msg,"Key sent to Cadencie is %s",masked_Key_value);
	nccup2_log_message( 2, 2, key_msg, "perform_host_key_send",0 );
	strncpy( HOST_MESSAGE.addl_data + 2 ,key_binary_val, (len -2)); // NK is already added

	//Build 53
	memcpy(HOST_MESSAGE.sec_cntl,"1600000000000000" ,CUP_DE53_SIZE);
	// Build 70
	strncpy( HOST_MESSAGE.net_manag_info_code,LOGON_NMI_KEY_RESET_REQUEST,3);

	//Build 100
	len = strlen(Receiving_ID);
	sprintf(HOST_MESSAGE.recv_ins_id_code_len, "%02d", len);
	strncpy( HOST_MESSAGE.recv_ins_id_code,Receiving_ID , len);
	//Build 128
	//We are not sedding it as off now
	ascii_to_bin_with_length (MAC,16, HOST_MESSAGE.mess_auth_code, 8);
	extendedbit=true;

	if ( false == build_host_msg(err_buf) )
         nccup2_log_message( 1, 3, err_buf, "perform_host_key_send",0 );
	if ( false == send_18nn_to_host(err_buf) )
         nccup2_log_message( 1, 3, err_buf, "perform_host_key_send",0 );
}


void process_key_request_response(pPTE_MSG p_msg_in)
{
	 pPTE_MSG_DATA	 p_msg_data	= NULL_PTR;
	 pBYTE			 p_data;
	 LONG			 data_len;
	 CHAR			 ErrorMsg[100]="";
	 CHAR   		 Buffer[100]="";
	 pPTE_MSG		 p_msg_out = NULL_PTR;

	 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
	 data_len	= ptemsg_get_pte_msg_data_data_length( p_msg_data );
	 p_data 	= ptemsg_get_pte_msg_data_data( p_msg_data );

	// empty privious key and Mac Value
	memset(static_Rtdcwk_Key,0x00,32);
	memset(MAC,0x00,16);
	Get_Key_from_ncf01_table();
	if ((strlen(static_Rtdcwk_Key)> 0) && (strlen(MAC)> 0))
	{
		strcpy(ErrorMsg,"Sending New key to Cadecncie");
		nccup2_log_message( 1, 1, ErrorMsg, "process_key_request_response",0 );
		perform_host_key_send();
		memcpy(Ncf01_I.retired_cwk,static_Rtdcwk_Key, strlen(static_Rtdcwk_Key));
		memcpy(Ncf01_I.kek,MAC, strlen(MAC));
		update_ncf01(&Auth_Tx);
	}
	else
	{
		strcpy(ErrorMsg,"Key update failed ");
		nccup2_log_message( 1, 3, ErrorMsg, "process_key_request_response",0 );
	}
	 
	free( p_msg_data );
	free( p_data );	
	 
}
void nccup2_process_logoff_after_key_transalation()
{
	INT     ret_val = true;
	CHAR    buffer[200]  = {0};
	CHAR    err_buf[200] = {0};

	sprintf(buffer,"Key exchange sequence is failed :%s Send log off message",AppName);
	nccup2_log_message( 1, 3, buffer,
						"nccup2_process_logoff_after_key_transalation",0 );

	issue_req_08X0(CUP_LOGOFF);
}

void Mask_Keys(char * key_value)
{
	int key_valuelen = 0;
	int nIndex 	= 0;
	memset(masked_Key_value ,0x00, sizeof(masked_Key_value));
	key_valuelen = strlen( key_value );
	if (key_valuelen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_Key_value, key_value, key_valuelen);
		for ( nIndex = key_valuelen-5; nIndex >= key_valuelen-11; nIndex-- )
		{
			masked_Key_value[nIndex] = '*';
		}
	}
}


