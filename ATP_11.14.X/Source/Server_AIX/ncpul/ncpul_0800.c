/***************************************************************************
 *
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncpul_0800.C
 *  
 * TITLE:       PULSE Network Controller Network Management Processor
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
#include "ncpul_bitfields.h"
#include "ncpul_prototypes.h"
#include "pulse.h"

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
extern NCF01   Ncf01_A;

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
extern CHAR static_Rtdcwk_Key[39];
extern CHAR MAC[17];

/******************************************************************************
 *
 *  NAME:         ncpul_process_monitor_request
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
void ncpul_process_monitor_request( pPTE_MSG p_msg )
{
	INT   nmi_type = 0;
	BYTE  nm_type = 0;
	CHAR  err_buf[100] = {0};
	INT   send_request = true;

	strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
	nm_type = ptemsg_get_msg_subtype1( p_msg );
	if ( nm_type == ST1_LOGON )
	{
		nmi_type = PULSE_LOGON;
	}
	else if ( nm_type == ST1_LOGOFF )
	{
		nmi_type = PULSE_LOGOFF;
	}
	 else if ( nm_type == ST1_ECHOTEST )
	 {
	      nmi_type = PULSE_ECHO;
		  //HCTS1-D
		  ncpul_log_message( 1, 1, "Sending ECHO message to host.", "ncpul_process_monitor_request",0 );
	 }
	else if (nm_type == ST1_PULSE_KEY_REQUEST)
	{
		nmi_type = PULSE_KEY_TEST;
	}
	else if ( nm_type == ST1_NETWORK_QUERY )
	{
		ncpul_process_monitor_query( p_msg );
		send_request = false;
	}
	else
	{
		send_request = false;
		memset( err_buf, 0x00, sizeof(err_buf) );
		sprintf( err_buf,
				"Unsupported request from Monitor. Subtype1 = %c",
				nm_type );
		ncpul_log_message( 1, 2, err_buf, "process_monitor_request",0 );
	}

	if ( send_request == true )
	{
		(void)ncpul_issue_req_08X4( nmi_type );
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncpul_issue_req_08X4
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
void ncpul_issue_req_08X4( INT nm_type )
{
	INT   ret_val = true;
	INT   seconds = 0;
	CHAR  echo_req_timer_key[PULSE_LOOKUP_KEY_SIZE] = {0};
	CHAR  err_buf[100] = {0};

	/* Read shared memory to get current state of host. */
	ncpul_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
								 current_state, err_buf );

	Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
	strncpy( Auth_Tx.TLF01_details.message_type, PULSE_NETWORK_REQ_MESSAGE ,PULSE_MESSAGE_TYPE_SIZE);
	TranFlow = PULSE_SWITCHED_OUT;

	if ( nm_type == PULSE_ECHO )
	{
		strncpy( NMI_Code, PULSE_NMI_SYSTEM_ECHO,PULSE_NETWORK_FUNCTION_CODE_LEN ); // it is hardcoded for acq
	}
	else if ( nm_type == PULSE_LOGON )
	{
		if ( 0 != strcmp( ONLINE, current_state ) )
		{
			strcpy( err_buf, "Sending LOG ON request to host." );
			ncpul_log_message( 1, 1, err_buf, "ncpul_issue_req_08X4",0 );
			strncpy( NMI_Code, PULSE_NMI_LOGON,PULSE_NETWORK_FUNCTION_CODE_LEN );
		}
		else
		{
			/* Already online.  Display this fact, then do not send request. */
		/*	strcpy( err_buf, "Already online.  Log ON request not sent." );
			ncpul_log_message( 1, 1, err_buf, "ncpul_issue_req_08X4",0 );*/
			ret_val = false;
		}
	}
	else if ( nm_type == PULSE_LOGOFF )
	{
		if ( 0 != strcmp( OFFLINE, current_state ) )
		{
			strcpy( err_buf, "Sending LOG OFF request to host." );
			ncpul_log_message( 1, 1, err_buf, "ncpul_issue_req_08X4",0 );
			strncpy( NMI_Code, PULSE_NMI_LOGOFF,PULSE_NETWORK_FUNCTION_CODE_LEN );
		}
		else
		{
			/* Already offline.  Display this fact, then do not send request. */
			strcpy( err_buf, "Already offline.  Log OFF request not sent." );
			ncpul_log_message( 1, 1, err_buf, "ncpul_issue_req_08X4",0 );
			ret_val = false;
		}
	}
	else if ( nm_type == PULSE_KEY_EXCHANGE )
	{
		strcpy( err_buf, "Initiating key exchange . . ." );
		ncpul_log_message( 3, 1, err_buf, "ncpul_issue_req_08X4",0 );
		strncpy( NMI_Code, PULSE_NMI_KEYCHANGE, PULSE_NETWORK_FUNCTION_CODE_LEN );
	}
	else if(nm_type == PULSE_KEY_TEST)
	{
		strcpy( err_buf, "Sending Key Change request. . ." );
		ncpul_log_message( 3, 1, err_buf, "ncpul_issue_req_08X4",0 );
		strncpy( NMI_Code, PULSE_NMI_KEYTEST, PULSE_NETWORK_FUNCTION_CODE_LEN );
	}
	else
	{
		/* Received a network management type that is not supported here. */
		sprintf( err_buf,
				"Received request for unsupported request Type = %d",nm_type );
		ncpul_log_message( 3, 1, err_buf, "ncpul_issue_req_08X4",0 );
		ret_val = false;
	}

	if ( ret_val == true )
	{
		/* Issue the 08X0 request. */

		/* Step 1.  Build the data fields. */
		if ( false == ncpul_build_08X0_msg_request(err_buf , nm_type) )
		{
			ncpul_log_message( 3, 2, err_buf, "ncpul_issue_req_08X4",0 );
		}
		/* Step 2.  Put host message together - build bitmap. */
		else if ( false == ncpul_build_host_msg(err_buf) )
		{
			ncpul_log_message( 3, 2, err_buf, "ncpul_issue_req_08X4",0 );
		}
		/* Step 3.  Send request to host. */
		else if ( false == ncpul_send_18nn_to_host(err_buf) )
		{
			ncpul_log_message( 3, 2, err_buf, "ncpul_issue_req_08X4",0 );
		}
	}
	return ;
}

/******************************************************************************
 *
 *  NAME:         ncpul_issue_rsp_08X4
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
INT ncpul_issue_rsp_08X4( INT nm_type )
{
	INT   ret_val = false;
	CHAR  err_buf[100] = {0};

	if ( nm_type == PULSE_LOGON )
	{
		ncpul_log_message( 1, 1, "Sending Log ON response to host.", "issue_rsp_08X0",0 );
		/*they asked to send 061 & 062 for log on log off response , again changed to 071 and 072*/
		//strncpy(Auth_Tx.function_code,PULSE_NMI_LOGON_HOST,3);
	}
	else if ( nm_type == PULSE_LOGOFF )
	{
		ncpul_log_message( 1, 1, "Sending Log OFF response to host.", "issue_rsp_08X0",0 );
		//strncpy(Auth_Tx.function_code,PULSE_NMI_LOGOFF_HOST,3);
	}
	else if ( nm_type == PULSE_ECHO )
	{
		ncpul_log_message( 0, 1, "Sending Echo  response to host.", "issue_rsp_08X0",0 );
			//strncpy(Auth_Tx.function_code,PULSE_NMI_LOGOFF_HOST,3);
	}

	/* Issue the 08X0 response. */
	ncpul_set_tx_key_to_response();

	/* Step 1.  Build the data fields. */
	if ( false == ncpul_build_08X0_msg_response(err_buf,nm_type ) )
	{
		ncpul_log_message( 3, 2, err_buf, "ncpul_issue_rsp_08X4",0 );
	}
	/* Step 2.  Put host message together - build bitmap. */
	else if ( false == ncpul_build_host_msg(err_buf) )
	{
		ncpul_log_message( 3, 2, err_buf, "ncpul_issue_rsp_08X4",0 );
	}
	/* Step 3.  Send request to host. */
	else if ( false == ncpul_send_18nn_to_host(err_buf) )
	{
		ncpul_log_message( 3, 2, err_buf, "ncpul_issue_rsp_08X4",0 );
	}
	else
	{
		ret_val = true;
	}


	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncpul_process_08X4_request
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
void ncpul_process_08X4_request()
{
   INT     ret_val = true;
   INT     type = 0;
   CHAR    buffer[200] = {0};
   CHAR    err_buf[200]  = {0};
   CHAR    new_state[3]  = "99";
   CHAR    same_state[3] = "99";

   /* Read shared memory to get current state of host. */
   ncpul_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   if ( 0 == strcmp(PULSE_NMI_LOGON, NMI_Code) )
   {
      type = PULSE_LOGON;
      ncpul_log_message( 1, 1, "Received Log ON request from host.",
                  "process_08X0_request",0 );
      if ( 0 == strcmp(ONLINE, current_state) )
         ncpul_log_message( 1, 1, "Already online.", "process_08X0_request",0 );
      else
         strcpy( new_state, ONLINE );
   }
   else if ( 0 == strcmp(PULSE_NMI_LOGOFF, NMI_Code) )
   {
      type = PULSE_LOGOFF;
      ncpul_log_message( 1, 1, "Received Log OFF request from host.",
                  "process_08X0_request",0 );
      if ( 0 == strcmp(OFFLINE, current_state) )
         ncpul_log_message( 1, 1, "Not online.", "process_08X0_request",0 );
      else
         strcpy( new_state, OFFLINE );
   }
   else if ( 0 == strcmp(PULSE_NMI_KEYCHANGE_ISS, NMI_Code) ||
		   	 0 == strcmp(PULSE_NMI_KEYCHANGE, NMI_Code) )
   {
      type = PULSE_KEY_EXCHANGE;
      ncpul_log_message( 1, 1, "Received Key change request from host.",
                  "process_08X0_request",0 );
      /* Request to do key translation  */
      memcpy(Auth_Tx.TLF01_details.source_key,Ncf01_I.retired_cwk,
    		 strlen(Ncf01_I.retired_cwk));
      strcpy(Auth_Tx.TLF01_details.dest_key, Ncf01_I.zmk);
      strcpy(Auth_Tx.TLF01_details.nfi_name,Ncf01_I.prev_src_station); // KCV
      strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
      ncpul_key_translate();
      return ;
   }
   else if( 0 == strcmp(PULSE_NMI_SYSTEM_ECHO, NMI_Code) )
   {
		type = PULSE_ECHO;
		/*ncpul_log_message( 1, 1, "Received ECHO TEST request from host.",
				 "process_08X0_request",0 );*/
	  if ( 0 == strcmp(ONLINE, current_state) )
	  {
			// ncpul_log_message( 1, 1, "Already online.", "process_08X0_request",0 );
	  }
	  else
		 strcpy( new_state, ONLINE );

   }
   else if( 0 == strcmp(PULSE_NMI_ISS_ECHO, NMI_Code) )
   {
		type = PULSE_ECHO;
		/*ncpul_log_message( 1, 1, "Received ECHO TEST request from host.",
				 "process_08X0_request",0 );*/
	  if ( 0 == strcmp(ONLINE, current_state) )
	  {
			// ncpul_log_message( 1, 1, "Already online.", "process_08X0_request",0 );
	  }
	  else
		 strcpy( new_state, ONLINE );

   }
   else
   {
      /* If NMI code is anything else, we do not support it. */
      ret_val = false;
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Received unsupported 08X0 request from host. NMI = %s",
                  NMI_Code );
         ncpul_log_message( 3, 1, buffer, "ncpul_process_08X4_request",0 );

   }

   /* Continue to process any 08X0 msg that is supported. */
   if ( ret_val == true )
   {
      /* Send an 0810 response to host. */
      if ( true == ncpul_issue_rsp_08X4( type ) )
      {
         if ( 0 != strcmp(new_state, same_state) )
         {
            /* Host state has changed.  Update shared memory. */
            if ( false == ncpul_set_host_state( new_state, err_buf ) )
            {
               memset(buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "Unable to update host state. " );
               strcat( buffer, err_buf );
               ncpul_log_message( 3, 2, buffer, "ncpul_process_08X4_request",0 );
            }
         }
      }
   }

   return;
}

/******************************************************************************
 *
 *  NAME:         ncpul_process_08X4_response
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
void ncpul_process_08X4_response()
{
	INT     ret_val = true;
	CHAR    buffer[200]  = {0};
	CHAR    err_buf[200] = {0};
	CHAR    echo_req_timer_key[PULSE_LOOKUP_KEY_SIZE] = {0};
	pPTE_MSG    p_msg_ins   = NULL_PTR;
	INT         Key_reset_request_flag = 0;

	/* Populate this field for statistical purposes. */
	if(Txn_Is_A_Reject == false)
	{
		strcpy( Auth_Tx.TLF01_details.response_code, PULSE_ISO_APPROVED );
	}

	strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);

	if ( 0 == strncmp(PULSE_NMI_SYSTEM_ECHO, NMI_Code,PULSE_NETWORK_FUNCTION_CODE_LEN) )
	{
		/* Read shared memory to get current state of host. */
		ncpul_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
									 current_state, err_buf );

		ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		if ( ret_val < 0 )
		{
			pteipc_get_errormsg( ret_val, err_buf );
			sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
			ncpul_log_message(1, 1,buffer,"ncpul_process_08X4_response",0);
		}

		ncpul_log_message( 1, 1, "Received Echo message reply. Setting state online.",
								"ncpul_process_08X4_response",0 );

		if( 0 == strncmp(Auth_Tx.TLF01_details.response_code,PULSE_ISO_APPROVED,PULSE_ISO_RESP_LEN))
		{
			if ( true == ncpul_set_host_state( ONLINE, err_buf ) )
			{
				if((0 == strncmp(AppName,"ncpul3",6)))
				{
					/* We just went online. Issue key change request. */
					ncpul_log_message( 1, 1, "Issuing Key Change message.",
					"process_08X0_response",0 );
					ncpul_issue_req_08X4( PULSE_KEY_EXCHANGE );
					Key_reset_request_flag = 1;
				}
			}
			else
			{
				strcpy( buffer, "Unable to update host state to online. " );
				strcat( buffer, err_buf );
				ncpul_log_message( 3, 2, buffer, "ncpul_process_08X4_response",0 );
			}
		}
		else
		{
			strcpy( buffer, "Echo message response failed. " );
			strcat( buffer, err_buf );
			ncpul_log_message( 3, 2, buffer, "ncpul_process_08X4_response",0 );
		}
	}

	else if ( 0 == strncmp(PULSE_NMI_LOGON, NMI_Code,PULSE_NETWORK_FUNCTION_CODE_LEN) )
	{
		/* Read shared memory to get current state of host. */
		ncpul_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
		current_state, err_buf );

		ret_val = pteipc_send(p_msg_ins, authorizer_que_name);
		if ( ret_val < 0 )
		{
			pteipc_get_errormsg( ret_val, err_buf );
			sprintf(buffer,"Failed transmit to que: %s. %s",applnk_que_name,err_buf);
			ncpul_log_message(1, 1,buffer,"ncpul_process_08X4_response",0);
		}

		ncpul_log_message( 1, 1, "Received Log ON response. Setting state online.",
								"ncpul_process_08X4_response",0 );

		if ( false == ncpul_set_host_state( ONLINE, err_buf ) )
		{
			strcpy( buffer, "Unable to update host state to offline. " );
			strcat( buffer, err_buf );
			ncpul_log_message( 3, 2, buffer, "process_08X0_response",0 );
		}
		return;
	}

	/* This field may have been overwritten on the Key Change.
	* Need for updating txn statistics.
	*/
	Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;

	if ( 0 == strcmp(PULSE_NMI_LOGOFF, NMI_Code) )
	{
		/* Read shared memory to get current state of host. */
		ncpul_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
		current_state, err_buf );

		if ( 0 != strcmp(OFFLINE, current_state) )
		{
			ncpul_log_message( 1, 1, "Received Log OFF response. Setting state offline.",
					"process_08X0_response",0 );

			if ( false == ncpul_set_host_state( OFFLINE, err_buf ) )
			{
				strcpy( buffer, "Unable to update host state to offline. " );
				strcat( buffer, err_buf );
				ncpul_log_message( 3, 2, buffer, "process_08X0_response",0 );
			}
		}
	}
	else if ( 0 == strcmp(PULSE_NMI_KEYCHANGE, NMI_Code) )
	{
		ncpul_log_message( 1, 1,"Received response to key change request",
							"process_08X0_response",0 );
		ncpul_process_logon_Key_Reset_Transaction();
	}
	else if ( 0 == strcmp(PULSE_NMI_KEYTEST, NMI_Code) )
	{
		ncpul_log_message( 1, 1,
					"Received response to key change request.  ATP is in now waiting for the key from Host.",
					"process_08X0_response",0 );
	}


	/* Update transaction statistics. */
	update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

	return;
}

/******************************************************************************
 *
 *  NAME:         ncpul_process_net_management_from_host
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
void ncpul_process_net_management_from_host()
{
	CHAR  buffer[100] = {0};

	if ( Auth_Tx.TLF01_details.message_type[2] == '0' )
	{
		ncpul_process_08X4_request();
	}

	else if ( Auth_Tx.TLF01_details.message_type[2] == '1' )
	{
		ncpul_process_08X4_response();
	}

	else
	{
		memset( buffer, 0x00, sizeof(buffer) );
		sprintf( buffer,
				"Unsupported net management msg from host. Msgtype = %s",
				Auth_Tx.TLF01_details.message_type );
		ncpul_log_message( 3, 2, buffer, "ncpul_process_net_management_from_host",0 );
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_08X0_msg_request
 *
 *  DESCRIPTION:  This function will populate thencpul_HOST_MESSAGE structure for
 *                an 08X0 request.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information, network Cmd
 *
 *  OUTPUTS:     ncpul_HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncpul_build_08X0_msg_request( pCHAR err_buf , int cmd)
{
	INT		ret_val = true;
	CHAR	time_date[25] = {0};
	CHAR	date_str[9] = {0};
	CHAR	time_str[7] = {0};
	INT		len = 0;
	CHAR	temp_len[3] = {0};
	INT     result_code = 0;  /* 0 = no error, 1 = error */
	CHAR    buffer[100] = {0};

	/*NMI_Code is stored in Auth_Tx.function_code because it is similar to DE24 in ncdci*/
	memset( &ncpul_HOST_MESSAGE, 0x00, sizeof (ncpul_HOST_MESSAGE) );

	if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					PULSE_NETWORK_REQ_MESSAGE,
					PULSE_MESSAGE_TYPE_SIZE))
	{
		(void)ncpul_build_request_field_7( &result_code, err_buf );
		ncpul_create_stan(ncpul_HOST_MESSAGE.stan );
		memcpy(Auth_Tx.function_code,NMI_Code,PULSE_DE70_SIZE);
		ncpul_build_request_field_70( &result_code, err_buf );
		ncpul_build_request_field_96( &result_code, err_buf );
		if(result_code == 1)
		{
			ret_val = false ;
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_08X0_msg_response
 *
 *  DESCRIPTION:  This function will populate thencpul_HOST_MESSAGE structure for
 *                an 08X0 response.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information , Network Cmd
 *
 *  OUTPUTS:     ncpul_HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncpul_build_08X0_msg_response( pCHAR err_buf, int nm_type )
{
   INT      ret_val = true;
   INT		len = 0;
   CHAR		temp_len[3] = {0};
   INT	    result_code = 0;  /* 0 = no error, 1 = error */


   memset( &ncpul_HOST_MESSAGE, 0x00, sizeof (ncpul_HOST_MESSAGE) );

   if((nm_type == PULSE_LOGON)||
	  (nm_type == PULSE_LOGOFF)||
	  (nm_type == PULSE_ECHO)||
      (nm_type == PULSE_KEY_EXCHANGE))
	{
		  // Build 7
		 (void)ncpul_build_request_field_7( &result_code, err_buf );
  		  // Build 11
		 (void)ncpul_build_request_field_11( &result_code, err_buf );
		  // Build 39
		 //(void)ncpul_build_request_field_39( &result_code, err_buf );
		 strncpy(ncpul_HOST_MESSAGE.responsecd, PULSE_NETMGMT_APPROVED,2 );
  		 // Build 70
		 (void)ncpul_build_request_field_70( &result_code, err_buf );
	}



   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncpul_send_18nn_to_host
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
INT ncpul_send_18nn_to_host( pCHAR err_buf )
{
	INT   ret_val;
	CHAR  buffer[100] = {0};

	ret_val = ncpul_send_message_to_host( err_buf,true );//praneeth added true

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
 *  NAME:         ncpul_process_monitor_query
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
void ncpul_process_monitor_query( pPTE_MSG p_msg )
{
   pPTE_MSG    p_msg_query = NULL_PTR;
   NCF01       ncf01;
   CHAR        err_buf[200] = {0};
   CHAR        Buff[256] = {0};
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0};
   XINT        ReturnCode = 0 ;
   pBYTE       pCurrent = NULL_PTR;

   /* Request from Monitor to send status information */
   ncpul_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
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
      ncpul_log_message( 2, 3, "Unable to build PTE message for Monitor query",
                   "ncpul_process_monitor_query",0 );
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         ptemsg_set_orig_comms_info( p_msg_query, ConnectionId) ;
         if ( false == ncpul_send_msg( p_msg_query, applnk_que_name, err_buf ))
            ncpul_log_message( 2, 3, err_buf, "ncpul_process_monitor_query",0 );
      }
      free( p_msg_query );
   }
   return;
}


