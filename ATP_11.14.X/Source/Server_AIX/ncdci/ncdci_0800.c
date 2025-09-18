/***************************************************************************
 *
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci_0800.C
 *  
 * TITLE:       DINERS Network Controller Network Management Processor
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
#include "txutils.h"
#include "ncdci_bitfields.h"
#include "ncdci_prototypes.h"
#include "diners.h"
#include "tx_dbstruct.h"

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
extern AUTH_TX   		 	Auth_Tx;
extern NCF01      			Ncf01_I;
extern NCF01      			Ncf01_I_key_exchange;
extern NCF01   	  			Ncf01_A;
extern NCF01_KEYBLOCK     	Ncf01_I_key_exchange_Keyblock;
extern NCF01_KEYBLOCK 		Ncf01_Keyblock;

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

extern INT keyblock_enable_flag_for_DCI;

extern CHAR  LOGON_NMI_KEY_RESET_REQUEST[5];
extern CHAR static_Rtdcwk_Key[39];
extern CHAR MAC[17];
extern CHAR static_Rtdcwk_Key_Keyblock[33];



/******************************************************************************
 *
 *  NAME:         ncdci_process_monitor_request
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_process_monitor_request( pPTE_MSG p_msg )
{
	INT   nmi_type = 0;
	BYTE  nm_type = 0;
	CHAR  err_buf[100] = {0};
	INT   send_request = true;

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );
	strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
	nm_type = ptemsg_get_msg_subtype1( p_msg );
	if ( nm_type == ST1_LOGON )
	{
		nmi_type = DINERS_LOGON;
	}
	else if ( nm_type == ST1_LOGOFF )
	{
		nmi_type = DINERS_LOGOFF;
	}
	else if ( nm_type == ST1_ECHOTEST )
	{
	    nmi_type = DINERS_ECHO;
		ncdci_log_message( 1, 1, "Sending ECHO TEST request to host", "ncdci_process_monitor_request",0 );
	}
	else if (nm_type == ST1_DINERS_KEY_TEST_REQUEST)
	{
		nmi_type = DINERS_KEY_TEST;

		 if( 0 == strncmp(AppName,"ncdci3",6))
		 {
			 if (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
			 {
				 ncdci_key_generate_req_keyblock();
			 }
			 else
			 {
				 ncdci_key_generate_req();
			 }
		 }
		return;
	}
	else if ( nm_type = ST1_NETWORK_QUERY )
	{
		ncdci_process_monitor_query( p_msg );
		send_request = false;
	}
	else
	{
		send_request = false;
		memset( err_buf, 0x00, sizeof(err_buf) );
		sprintf( err_buf,
				"Unsupported request from Monitor. Subtype1 = %c",
				nm_type );
		ncdci_log_message( 1, 2, err_buf, "process_monitor_request", 0 );
	}

	if ( send_request == true )
	{
		(void)ncdci_issue_req_08X4( nmi_type );
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_issue_req_08X4
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_issue_req_08X4( INT nm_type )
{
	INT   ret_val = true;
	INT   seconds = 0;
	CHAR  echo_req_timer_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  err_buf[100] = {0};

	/* Read shared memory to get current state of host. */
	ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
								 current_state, err_buf );

	Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
	strncpy( Auth_Tx.TLF01_details.message_type, DINERS_NETWORK_REQ_MESSAGE ,DINERS_MESSAGE_TYPE_SIZE);
	TranFlow = DINERS_SWITCHED_OUT;

	if ( nm_type == DINERS_ECHO )
	{
		//strcpy( err_buf, "Sending ECHO message to host." );
		//ncdci_log_message( 1, 1, err_buf, "ncdci_issue_req_08X4",0 );
		strncpy( NMI_Code, DINERS_NMI_SYSTEM_ECHO,DINERS_NETWORK_FUNCTION_CODE_LEN );

		//HCTS1-A
	    /* Set Timer for Echoes. */
	    seconds = atoi(Ncf01_I.request_timer);
	    strcpy( TimerAppData2, DINERS_TIMER_ECHO );
	    ncdci_create_echo_req_timer_key( echo_req_timer_key );
	    ncdci_set_timer( echo_req_timer_key, seconds, DINERS_ECHO_TIMER );
	}
	else if ( nm_type == DINERS_LOGON )
	{
		if ( 0 != strcmp( ONLINE, current_state ) )
		{
			strcpy( err_buf, "Sending LOG ON request to host." );
			ncdci_log_message( 1, 1, err_buf, "ncdci_issue_req_08X4", 0 );
			strncpy( NMI_Code, DINERS_NMI_LOGON,DINERS_NETWORK_FUNCTION_CODE_LEN );
		}
		else
		{
			/* Already online.  Display this fact, then do not send request. */
			strcpy( err_buf, "Already online.  Log ON request not sent." );
			ncdci_log_message( 1, 1, err_buf, "ncdci_issue_req_08X4", 0 );
			ret_val = false;
		}
	}
	else if ( nm_type == DINERS_LOGOFF )
	{
		if ( 0 != strcmp( OFFLINE, current_state ) )
		{
			strcpy( err_buf, "Sending LOG OFF request to host." );
			ncdci_log_message( 1, 1, err_buf, "ncdci_issue_req_08X4" , 0);
			strncpy( NMI_Code, DINERS_NMI_LOGOFF,DINERS_NETWORK_FUNCTION_CODE_LEN );
		}
		else
		{
			/* Already offline.  Display this fact, then do not send request. */
			strcpy( err_buf, "Already offline.  Log OFF request not sent." );
			ncdci_log_message( 1, 1, err_buf, "ncdci_issue_req_08X4", 0 );
			ret_val = false;
		}
	}
	else if ( nm_type == DINERS_KEY_EXCHANGE )
	{
		strcpy( err_buf, "Initiating key exchange . . ." );
		ncdci_log_message( 1, 1, err_buf, "ncdci_issue_req_08X4", 0 );
		strncpy( NMI_Code, DINERS_NMI_KEYCHANGE, DINERS_NETWORK_FUNCTION_CODE_LEN );
	}
	else
	{
		/* Received a network management type that is not supported here. */
		sprintf( err_buf,
				"Received request for unsupported request Type = %d",nm_type );
		ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		ret_val = false;
	}

	if ( ret_val == true )
	{
		/* Issue the 08X0 request. */

		/* Step 1.  Build the data fields. */
		if ( false == ncdci_build_08X0_msg_request(err_buf , nm_type) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
		/* Step 2.  Put host message together - build bitmap. */
		else if ( false == ncdci_build_host_msg(err_buf) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
		/* Step 3.  Send request to host. */
		else if ( false == ncdci_send_18nn_to_host(err_buf) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_issue_keytest_req_08X4
 *
 *  DESCRIPTION:  This function sends a KEY TEST REQUEST to the host.
 *
 *  INPUTS:       nm_type - Type of 08X0 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_issue_keytest_req_08X4( INT nm_type )
{
	INT   ret_val = true;
	INT   seconds = 0;
	CHAR  echo_req_timer_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  err_buf[100] = {0};

	/* Read shared memory to get current state of host. */
	ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
								 current_state, err_buf );

	Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE;
	strncpy( Auth_Tx.TLF01_details.message_type, DINERS_NETWORK_REQ_MESSAGE ,DINERS_MESSAGE_TYPE_SIZE);
	TranFlow = DINERS_SWITCHED_OUT;
	strncpy( NMI_Code, DINERS_NMI_KEYTEST ,DINERS_NETWORK_FUNCTION_CODE_LEN);

	if ( ret_val == true )
	{
		/* Issue the 08X0 request. */

		/* Step 1.  Build the data fields. */
		if ( false == ncdci_build_08X0_msg_request(err_buf , nm_type) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
		/* Step 2.  Put host message together - build bitmap. */
		else if ( false == ncdci_build_host_msg(err_buf) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
		/* Step 3.  Send request to host. */
		else if ( false == ncdci_send_18nn_to_host(err_buf) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_issue_changetest_req_08X4
 *
 *  DESCRIPTION:  This function sends a KEY CHANGE REQUEST to the host.
 *
 *  INPUTS:       nm_type - Type of 08X0 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_issue_changetest_req_08X4( INT nm_type )
{
	INT   ret_val = true;
	INT   seconds = 0;
	CHAR  echo_req_timer_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  err_buf[100] = {0};

	/* Read shared memory to get current state of host. */
	ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
								 current_state, err_buf );

	Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE;
	strncpy( Auth_Tx.TLF01_details.message_type, DINERS_NETWORK_REQ_MESSAGE ,DINERS_MESSAGE_TYPE_SIZE);
	TranFlow = DINERS_SWITCHED_OUT;
	strncpy( NMI_Code, DINERS_NMI_KEYCHANGE ,DINERS_NETWORK_FUNCTION_CODE_LEN);

	if ( ret_val == true )
	{
		/* Issue the 08X0 request. */

		/* Step 1.  Build the data fields. */
		if ( false == ncdci_build_08X0_msg_request(err_buf , nm_type) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
		/* Step 2.  Put host message together - build bitmap. */
		else if ( false == ncdci_build_host_msg(err_buf) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
		/* Step 3.  Send request to host. */
		else if ( false == ncdci_send_18nn_to_host(err_buf) )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_issue_req_08X4", 0 );
		}
	}
	return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_issue_rsp_08X4
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
INT ncdci_issue_rsp_08X4( INT nm_type )
{
	INT   ret_val = false;
	CHAR  err_buf[100] = {0};

	if ( nm_type == DINERS_LOGON )
	{
		ncdci_log_message( 1, 1, "Sending Log ON response to host.",
								 "issue_rsp_08X0", 0 );
	}
	else if ( nm_type == DINERS_LOGOFF )
	{
		ncdci_log_message( 1, 1, "Sending Log OFF response to host.",
								 "issue_rsp_08X0", 0 );
	}

	/* Issue the 08X0 response. */
	ncdci_set_tx_key_to_response();

	/* Step 1.  Build the data fields. */
	if ( false == ncdci_build_08X0_msg_response(err_buf,nm_type ) )
	{
		ncdci_log_message( 1, 2, err_buf, "ncdci_issue_rsp_08X4", 0 );
	}
	/* Step 2.  Put host message together - build bitmap. */
	else if ( false == ncdci_build_host_msg(err_buf) )
	{
		ncdci_log_message( 1, 2, err_buf, "ncdci_issue_rsp_08X4", 0 );
	}

	/* Step 3.  Send request to host. */
	else if ( false == ncdci_send_18nn_to_host(err_buf) )
	{
		ncdci_log_message( 1, 2, err_buf, "ncdci_issue_rsp_08X4", 0 );
	}
	else
	{
		ret_val = true;
	}

	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_08X4_request
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_process_08X4_request()
{
   INT     ret_val = true;
   INT     type = 0;
   CHAR    buffer[200] = {0};
   CHAR    err_buf[200]  = {0};
   CHAR    new_state[3]  = "99";
   CHAR    same_state[3] = "99";

   /* Read shared memory to get current state of host. */
   ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   if ( 0 == strcmp(DINERS_NMI_LOGON, NMI_Code) )
   {
      type = DINERS_LOGON;
      ncdci_log_message( 1, 1, "Received Log ON request from host.",
                  "ncdci_process_08X4_request", 0 );
      if ( 0 == strcmp(ONLINE, current_state) )
         ncdci_log_message( 1, 1, "Already online.",
        		 "ncdci_process_08X4_request", 0 );
      else
         strcpy( new_state, ONLINE );
   }
   else if ( 0 == strcmp(DINERS_NMI_LOGOFF, NMI_Code) )
   {
      type = DINERS_LOGOFF;
      ncdci_log_message( 1, 1, "Received Log OFF request from host.",
                  	  	  	   "ncdci_process_08X4_request", 0 );
      if ( 0 == strcmp(OFFLINE, current_state) )
         ncdci_log_message( 1, 1, "Not online.",
        		 	 	 	 	  "ncdci_process_08X4_request", 0 );
      else
         strcpy( new_state, OFFLINE );
   }
    else
   {
      /* If NMI code is anything else, we do not support it. */
      ret_val = false;
      if ( 0 == strcmp(DINERS_NMI_KEYCHANGE, NMI_Code) )
         ncdci_log_message( 1, 2, "Key Change request from host not supported.",
                     "process_08X0_request", 0 );
      else
      {
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Received unsupported 08X0 request from host. NMI = %s",
                  NMI_Code );
         ncdci_log_message( 1, 2, buffer, "ncdci_process_08X4_request", 0 );
      }
   }

   /* Continue to process any 08X0 msg that is supported. */
   if ( ret_val == true )
   {
      /* Send an 1814 response to host. */
      if ( true == ncdci_issue_rsp_08X4( type ) )
      {
         if ( 0 != strcmp(new_state, same_state) )
         {
            /* Host state has changed.  Update shared memory. */
            if ( false == ncdci_set_host_state( new_state, err_buf ) )
            {
               memset(buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "Unable to update host state. " );
               strcat( buffer, err_buf );
               ncdci_log_message( 1, 3, buffer, "ncdci_process_08X4_request", 0 );
            }
         }
      }
   }
   // We have to make other module offlien if we have received offlien request from host.
   if ( 0 == strcmp(new_state, OFFLINE) )
   {
	   if((0 == strncmp(AppName,"ncdci3",6)))
	   {
		   ncdci_send_logoff_message_to_ncdci();
	   }
	   else
	   {
		   ncdci_send_logoff_message_to_ncdci3();
	   }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_08X4_response
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_process_08X4_response()
{
	INT     ret_val = true;
	CHAR    buffer[200]  = {0};
	CHAR    err_buf[200] = {0};
	CHAR    echo_req_timer_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	pPTE_MSG    p_msg_ins   = NULL_PTR;
	INT         Key_reset_request_flag = 0;

	/* Populate this field for statistical purposes. */
	strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);

	if ( 0 == strncmp(DINERS_NMI_SYSTEM_ECHO, NMI_Code,DINERS_NETWORK_FUNCTION_CODE_LEN) )
	{
		/* Read shared memory to get current state of host. */
		ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
									 current_state, err_buf );

		ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		if ( ret_val < 0 )
		{
			pteipc_get_errormsg( ret_val, err_buf );
			sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
			ncdci_log_message(1, 1,buffer,"ncdci_process_08X4_response", 0);
		}

		ncdci_log_message( 1, 1, "Received Echo message reply. Setting state online.",
								"ncdci_process_08X4_response", 0 );

		if( 0 == strncmp(Auth_Tx.TLF01_details.response_code,DINERS_ISO_APPROVED,DINERS_ISO_RESP_LEN))
		{
			if ( 0 == strcmp(OFFLINE, current_state) )
			{
				if ( false == ncdci_set_host_state( ONLINE, err_buf ) )
				{

					strcpy( buffer, "Unable to update host state to online. " );
					strcat( buffer, err_buf );
					ncdci_log_message( 1, 3, buffer, "ncdci_process_08X4_response", 0 );
				}
			}
			else
			{
				strcpy( buffer, "Host is already online. " );
				strcat( buffer, err_buf );
				ncdci_log_message( 1, 1, buffer, "ncdci_process_08X4_response", 0 );
			}
		}
	}

	else if ( 0 == strncmp(DINERS_NMI_LOGON, NMI_Code,DINERS_NETWORK_FUNCTION_CODE_LEN) )
	{
		/* Read shared memory to get current state of host. */
		ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
		current_state, err_buf );

		ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		if ( ret_val < 0 )
		{
			pteipc_get_errormsg( ret_val, err_buf );
			sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
			ncdci_log_message(1, 1,buffer,"ncdci_process_08X4_response", 0);
		}

		ncdci_log_message( 1, 1, "Received Log ON response. Setting state online.",
								"ncdci_process_08X4_response", 0 );

		if ( 0 == strcmp(OFFLINE, current_state) )
		{
			if ( false == ncdci_set_host_state( ONLINE, err_buf ) )
			{

				strcpy( buffer, "Unable to update host state to online. " );
				strcat( buffer, err_buf );
				ncdci_log_message( 1, 3, buffer, "ncdci_process_08X4_response", 0 );
			}
		}
		else
		{
			strcpy( buffer, "Host is already online. " );
			strcat( buffer, err_buf );
			ncdci_log_message( 1, 1, buffer, "ncdci_process_08X4_response", 0 );
		}
	}

	else if ( 0 == strcmp(DINERS_NMI_LOGOFF, NMI_Code) )
	{
		/* Read shared memory to get current state of host. */
		ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
		current_state, err_buf );

		if ( 0 != strcmp(OFFLINE, current_state) )
		{
			ncdci_log_message( 1, 1, "Received Log OFF response. Setting state offline.",
					"ncdci_process_08X4_response", 0 );

			if ( false == ncdci_set_host_state( OFFLINE, err_buf ) )
			{
				strcpy( buffer, "Unable to update host state to offline. " );
				strcat( buffer, err_buf );
				ncdci_log_message( 1, 3, buffer, "ncdci_process_08X4_response", 0 );
			}
		}
	}

	else if ( 0 == strcmp(DINERS_NMI_KEYTEST, NMI_Code) )
	{
		if( 0 == strncmp(AppName,"ncdci3",6))
		{
			if( 0 == strncmp(Auth_Tx.TLF01_details.response_code,
							 DINERS_ISO_APPROVED,
							 DINERS_ISO_RESP_LEN))
			{
				ncdci_log_message( 1, 1,"Received response for key test tranasction ,Sending key Test transaction",
									"ncdci_process_08X4_response",1 );
				// Let send key change request tansaction to DCI DINERS_NMI_KEYCHANGE only by ncdci3(issuing module)
				ncdci_issue_changetest_req_08X4(DINERS_NMI_KEYCHANGE);
			}
			else
			{
				ncdci_log_message( 1, 1,"Key generation request declined by DCI",
									"ncdci_process_08X4_response",1 );
			}
		}
	}

	else if ( 0 == strcmp(DINERS_NMI_KEYCHANGE, NMI_Code) )
	{

		if( 0 == strncmp(AppName,"ncdci3",6))
		{
			if( 0 == strncmp(Auth_Tx.TLF01_details.response_code,
							 DINERS_ISO_APPROVED,
							 DINERS_ISO_RESP_LEN))
			{
				ncdci_log_message( 1, 1,
									"Received response to key change request. Going to update key.",
									"ncdci_process_08X4_response", 0 );

				if (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
				{
					ncdci_process_logon_Key_Reset_Transaction_keyblock();
				}
				else
				{
					ncdci_process_logon_Key_Reset_Transaction();
				}
			}
			else
			{
				ncdci_log_message( 1, 1,"Key generation request declined by DCI",
									"ncdci_process_08X4_response",1 );
			}
		}
	}
	else
	{
		strcpy( Auth_Tx.TLF01_details.response_code, DINERS_INVALID_TXN );
		sprintf( buffer,
				"Received response to unsupported network management message. NMI=%s,STAN=%s",
				NMI_Code, Auth_Tx.TLF01_details.sys_trace_audit_num );

		ncdci_log_message( 1, 2, buffer, "ncdci_process_08X4_response", 0 );
	}

	/* Update transaction statistics. */
	update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_net_management_from_host
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
void ncdci_process_net_management_from_host()
{
	CHAR  buffer[100] = {0};

	if ( Auth_Tx.TLF01_details.message_type[2] == '0' )
	{
		ncdci_process_08X4_request();
	}

	else if ( Auth_Tx.TLF01_details.message_type[2] == '1' )
	{
		ncdci_process_08X4_response();
	}

	else
	{
		memset( buffer, 0x00, sizeof(buffer) );
		sprintf( buffer,
				"Unsupported net management msg from host. Msgtype = %s",
				Auth_Tx.TLF01_details.message_type );
		ncdci_log_message( 1, 2, buffer, "ncdci_process_net_management_from_host", 0 );
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_08X0_msg_request
 *
 *  DESCRIPTION:  This function will populate thencdci_HOST_MESSAGE structure for
 *                an 08X0 request.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information, network Cmd
 *
 *  OUTPUTS:     ncdci_HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_build_08X0_msg_request( pCHAR err_buf , int cmd)
{
	INT		ret_val = true;
	CHAR	time_date[25] = {0};
	CHAR	date_str[9] = {0};
	CHAR	time_str[7] = {0};
	INT		len = 0;
	CHAR	temp_len[3] = {0};
	INT     result_code = 0;  /* 0 = no error, 1 = error */
	CHAR    buffer[100] = {0};

	memset( &ncdci_HOST_MESSAGE, 0x00, sizeof (ncdci_HOST_MESSAGE) );

	if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					DINERS_NETWORK_REQ_MESSAGE,
					DINERS_MESSAGE_TYPE_SIZE))
	{
		ncdci_build_request_field_7( &result_code, err_buf );
		if(0 == strncmp(NMI_Code,DINERS_NMI_KEYCHANGE,
						DINERS_NETWORK_FUNCTION_CODE_LEN))
		{
			sleep( 3 );
		}
		ncdci_create_stan(ncdci_HOST_MESSAGE.stan );
		ncdci_build_request_field_12( &result_code, err_buf );
		memcpy(Auth_Tx.function_code,NMI_Code,DINERS_DE24_SIZE);
		ncdci_build_request_field_24( &result_code, err_buf );
		ncdci_build_request_field_33( &result_code, err_buf );
		ncdci_build_request_field_44( &result_code, err_buf );
		ncdci_build_request_field_64( &result_code, err_buf );
		ncdci_build_request_field_93( &result_code, err_buf );
		ncdci_build_request_field_94( &result_code, err_buf );
		ncdci_build_request_field_96( &result_code, err_buf );
		ncdci_build_request_field_100( &result_code, err_buf );
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_08X0_msg_response
 *
 *  DESCRIPTION:  This function will populate thencdci_HOST_MESSAGE structure for
 *                an 08X0 response.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information , Network Cmd
 *
 *  OUTPUTS:     ncdci_HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_build_08X0_msg_response( pCHAR err_buf, int nm_type )
{
   INT      ret_val = true;
   INT		len = 0;
   CHAR		temp_len[3] = {0};
   INT	    result_code = 0;  /* 0 = no error, 1 = error */


   memset( &ncdci_HOST_MESSAGE, 0x00, sizeof (ncdci_HOST_MESSAGE) );

   if((nm_type == DINERS_LOGON)|| (nm_type == DINERS_LOGOFF)|| (nm_type == DINERS_ECHO))
	{
		  // Build 7
		 (void)ncdci_build_request_field_7( &result_code, err_buf );
  		  // Build 11
		 (void)ncdci_build_request_field_11( &result_code, err_buf );
		  // Build 39
		 (void)ncdci_build_request_field_33( &result_code, err_buf );
  		 // Build 100
		 (void)ncdci_build_request_field_100( &result_code, err_buf );
	}
	else if((nm_type == DINERS_KEY_EXCHANGE))
	{
		// Build 7
		(void)ncdci_build_request_field_7( &result_code, err_buf );
		// Build 11
		(void)ncdci_build_request_field_11( &result_code, err_buf );
		// Build 39
		(void)ncdci_build_request_field_39( &result_code, err_buf );
		// Build 100
		(void)ncdci_build_request_field_100( &result_code, err_buf );

	}
	//strcpy(ncdci_HOST_MESSAGE.responsecd, DINERS_NETMGMT_APPROVED );


   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_send_18nn_to_host
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_send_18nn_to_host( pCHAR err_buf )
{
	INT   ret_val;
	CHAR  buffer[100] = {0};

	ret_val = ncdci_send_message_to_host( err_buf,true );//praneeth added true

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
 *  NAME:         ncdci_process_monitor_query
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
void ncdci_process_monitor_query( pPTE_MSG p_msg )
{
   pPTE_MSG    p_msg_query = NULL_PTR;
   NCF01       ncf01;
   CHAR        err_buf[200] = {0};
   CHAR        Buff[256] = {0};
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0};
   XINT        ReturnCode = 0 ;
   pBYTE       pCurrent = NULL_PTR;

   /* Request from Monitor to send status information */
   ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
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
      ncdci_log_message( 2, 3, "Unable to build PTE message for Monitor query",
                   	   	   	   "ncdci_process_monitor_query", 0 );
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         ptemsg_set_orig_comms_info( p_msg_query, ConnectionId) ;
         if ( false == ncdci_send_msg( p_msg_query, applnk_que_name, err_buf ))
            ncdci_log_message( 2, 3, err_buf, "ncdci_process_monitor_query", 0 );
      }
      free( p_msg_query );
   }
   return;
}

void ncdci_process_key_request_response(pPTE_MSG p_msg_in)
{
	 pPTE_MSG_DATA	 p_msg_data	= NULL_PTR;
	 pBYTE			 p_data = NULL_PTR;;
	 LONG			 data_len = 0;
	 CHAR			 ErrorMsg[100] = {0};
	 CHAR   		 Buffer[100] = {0};

	 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
	 data_len	= ptemsg_get_pte_msg_data_data_length( p_msg_data );
	 p_data 	= ptemsg_get_pte_msg_data_data( p_msg_data );
	 
	ncdci_Get_Key_from_ncf01_table();
	if (strlen(static_Rtdcwk_Key)> 0)
	{
		strcpy(ErrorMsg,"ncdci updating the key");
		ncdci_log_message( 1, 1, ErrorMsg, "ncdci_process_key_request_response", 0 );
		memcpy( Ncf01_I.retired_cwk,Ncf01_I_key_exchange.retired_cwk,
				DINERS_DE96_KEY_LEN);
		memcpy( Ncf01_I.cwk,Ncf01_I_key_exchange.cwk,
				DINERS_DE96_KEY_LEN);
		memcpy( Ncf01_I.prev_src_station,Ncf01_I_key_exchange.prev_src_station,
				DINERS_DE96_KEY_CHECK_LEN);
		ncdci_update_ncf01(&Auth_Tx);
	}
	else
	{
		strcpy(ErrorMsg,"Key update failed ");
		ncdci_log_message( 1, 3, ErrorMsg, "ncdci_process_key_request_response", 0 );
	}
	free( p_msg_data );
	free( p_data );
}

void ncdci_process_key_request_response_keyblock(pPTE_MSG p_msg_in)
{
	 pPTE_MSG_DATA	 p_msg_data	= NULL_PTR;
	 pBYTE			 p_data = NULL_PTR;;
	 LONG			 data_len = 0;
	 CHAR			 ErrorMsg[100] = {0};
	 CHAR   		 Buffer[100] = {0};

	 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
	 data_len	= ptemsg_get_pte_msg_data_data_length( p_msg_data );
	 p_data 	= ptemsg_get_pte_msg_data_data( p_msg_data );

	 ncdci_Get_Key_from_ncf01_keyblock_table();
	 if (strlen(static_Rtdcwk_Key_Keyblock)>0)
	 {
		 strcpy(ErrorMsg,"ncdci updating the key");
		 ncdci_log_message( 1, 1, ErrorMsg, "ncdci_process_key_request_response_keyblock", 0 );

		 memcpy( Ncf01_Keyblock.iss_rtdcwk_keyblock,
				 Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock,
				 DINERS_DE96_KEY_LEN);

		 memcpy( Ncf01_Keyblock.iss_cwk_keyblock,
				 Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock,
				 DINERS_DE96_KEY_LEN);

		 memcpy( Ncf01_Keyblock.iss_prev_src_station,
				 Ncf01_I_key_exchange_Keyblock.iss_prev_src_station,
				 DINERS_DE96_KEY_CHECK_LEN);

		 ncdci_update_ncf01_keyblock(&Auth_Tx);
	 }
	 else
	 {
		strcpy(ErrorMsg,"Key update failed ");
		ncdci_log_message( 1, 3, ErrorMsg, "ncdci_process_key_request_response", 0 );
	 }
	 free( p_msg_data );
	 free( p_data );
}


