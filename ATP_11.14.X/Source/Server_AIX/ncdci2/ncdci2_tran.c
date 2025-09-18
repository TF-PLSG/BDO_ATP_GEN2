/******************************************************************************
 
 	ncdci2_tran.c
 
    Copyright (c) 2005, ThoughtFocus, Inc.
    All Rights Reserved.
 
    TITLE:  Diners Acquiring Host Handler Driver
 
 	This module is the host handler for the Diners host.  It
    will process all messages destined to, or originated from,
    the Equitable host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
   AUTHOR  : Abhishek Verma
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "diners.h"
#include "ncdci2_prototypes.h"
#include "txutils.h"
#include "txtrace.h"
#include "ntutils.h"

/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

extern CHAR    AppName[];

/* QUEUES */
extern CHAR    updateds_que_name[];
extern CHAR    nsp_que_name[];
extern CHAR    trands_que_name[];
extern CHAR    netds_que_name[];
extern CHAR    devds_que_name[];
extern CHAR    oprmon_que_name[];
extern CHAR    applnk_que_name[];
extern CHAR    authorizer_que_name[];
extern CHAR    timerds_que_name [];
extern CHAR    tpdu_id[16];

extern CHAR ServiceName[12];


/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_I_key_exchange;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern CRF01   Crf01;

extern NCF01_KEYBLOCK     Ncf01_Keyblock;
extern NCF01_KEYBLOCK     Ncf01_I_key_exchange_Keyblock;

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

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Miscellaneous */
extern INT     TranFlow;
extern CHAR    TimerAppData2[];

extern INT    buflen;                  /* total size of buffer */
extern AUTH_TX              Auth_Tx;
extern BYTE   ReqRsp_Buffer [DINERS_COMM_BUF_SZE];
extern NCF01    ncf01;
extern NCF01 Ncf01_Ncdci_A;
extern NCF01 Ncf01_Ncdci_I;

extern CHAR  NMI_Code[4];

/*Flags*/
extern INT Txn_Is_A_Reject;
extern INT keyblock_enable_flag_for_DCI ;

/*Need to feild with correct Diners value*/
CHAR  LOGON_NMI_ECHO_TEST[5] 				= "301";
CHAR  LOGON_NMI_LOGON[5] 					= "001";
CHAR  LOGON_NMI_LOGOFF[5] 					= "002";
CHAR  LOGON_NMI_KEY_RESET_REQUEST[5]        = "101";


CHAR   security_que_name [] = "racalA" ;

INT     si;
extern CHAR   static_Rtdcwk_Key[39];




/***************************************************************************
 *
 *  NAME:         ncdci2_process_host_msg
 *
 *  DESCRIPTION:  Process a message coming in from the host.  Parse it and
 *                populate a host structure with the information.  Then
 *                determine what type of message it is. Call the appropriate
 *                function to deal with it.
 *
 *  INPUTS:       p_msg_in - This is the incoming message from the host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ***************************************************************************/
void ncdci2_process_host_msg( pPTE_MSG p_msg_in )
{
	pPTE_MSG_DATA     p_msg_data = NULL_PTR;
	BYTE    tran_type = 0;
	CHAR    err_buf[256] = {0};
	INT     ret_val = true;

	/* Cycle the echo timer because traffic came over the line. */
	if ( false == ncdci2_reset_echo_timer(err_buf) )
	{
		ncdci2_log_message(2, 2, err_buf, "ncdci2_process_host_msg",0 );
	}

	/* get the p_msg data from p_msg */
	p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);

	/* Copy host message into a character string. */
	ncdci2_copy_to_req_resp_buffer( p_msg_in );
	if(buflen >= DINERS_COMM_BUF_SZE)
	{
		sprintf(err_buf,"Invalid length or Big msg recvd from Host:  %04d", buflen);
		ncdci2_log_message( 1, 3, err_buf, "ncdci2_process_host_msg",0 );
		return;
	}
	/* Log transaction to trace file. */
	ncdci2_write_to_txn_file();

	// We are putting sleep for testing purpose.
	//sleep(6);
	/* Parse message from host. */
	memset( &Auth_Tx, 0x00, Auth_Tx_Size );

	/* Gather originator information */
	(void)ncdci2_get_originator_info( p_msg_in, Auth_Tx.TLF01_details.originator_queue,
									  Auth_Tx.TLF01_details.originator_info );

	if ( !ncdci2_parse_host_msg( err_buf ) )
	{
		ret_val = false;
	}
	else if ( !ncdci2_validate_msgtype( Auth_Tx.TLF01_details.message_type, err_buf ) )
	{
		ret_val = false;
	}
	else
	{
		/* The transaction from host was successfully parsed.
		* Put information from host into auth_tx structure.
		*Copy response msg from cadice to res buffer- TF Abhishek*/
		if(buflen < DINERS_COMM_BUF_SZE && buflen>0)
		{
			sprintf(Auth_Tx.resp_data_len,"%04d", buflen);
			memcpy(Auth_Tx.resp_buffer,ReqRsp_Buffer,buflen);
		}

		/* Determine if switched in request, or response to a switched out txn.*/
		tran_type = ncdci2_get_tran_type_2(Auth_Tx.TLF01_details.tx_key);
		Auth_Tx.TLF01_details.tx_key = tran_type;
		TranFlow = ncdci2_determine_txn_flow( Auth_Tx.TLF01_details.message_type,
											  DINERS_HOST );
		/*This is test code and we have to remove this
		if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"1100", 4))||
		   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"1110", 4)))
			{
				sleep(6);
			}*/
		/* Populate Auth_tx structure with data from host. */
		if ( !ncdci2_populate_auth_tx(err_buf) )
		{
			ret_val = false;
		}

		if ( Txn_Is_A_Reject == true )
		{
			strcpy( Auth_Tx.TLF01_details.response_code, DINERS_FORMAT_ERROR );

			/* Need to convert this request to a response. */
			if ( Auth_Tx.TLF01_details.message_type[1] == '0' )
			{
				Auth_Tx.TLF01_details.message_type[1] = '1';
			}
			else if ( Auth_Tx.TLF01_details.message_type[1] == '2' )
			{
				Auth_Tx.TLF01_details.message_type[1] = '3';
				tran_type = ncdci2_match_request_to_response( tran_type );
				Auth_Tx.TLF01_details.tx_key = tran_type;
				strcpy( Auth_Tx.TLF01_details.response_text,
						"Received Diners Reject Msg on the Request" );
			}
		}
		if ( TranFlow == DINERS_SWITCHED_IN )
		{
			ncdci2_process_request_from_host();
		}
		else
		{
			ncdci2_process_response_from_host();
		}
	}
	
	/* Log any errors. */
	if ( ret_val == false )
	{
		if (0 != strncmp( ReqRsp_Buffer, "0000", 4))
		{
			ncdci2_log_message( 2, 2, err_buf, "ncdci2_process_host_msg",1 );
		}
	}
	return;
}

/**************************************************************************
 *
 *  NAME:         ncdci2_process_request_from_host
 *
 *  DESCRIPTION:  This function is the driver for network requests from 
 *				 the BDO cedaince host
 *                .
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 *
 ***************************************************************************/
void ncdci2_process_request_from_host()
{
	INT   ret_val;
	INT   len =0;
	pPTE_MSG_DATA     p_msg_data = NULL_PTR;
	pPTE_MSG          p_msg_chain = NULL_PTR;
	pPTE_MSG          p_msg_timer_out = NULL_PTR;
	CHAR              tid[9] = {0};
	CHAR              Buffer[256] = {0};
	CHAR              ErrorMsg[100] = {0};
	CHAR              nmi_code[4]= {0};
	CHAR			  lookup_key[22] = {0};
	CHAR              temp_eff_date[7] = {0};
	BYTE              buffer[sizeof(AUTH_TX)] = {0};
	BYTE              tran_type = 0;

	if(AUTH_INVALID == Auth_Tx.TLF01_details.tx_key)
	{
		/*+-------------------------+
		| Send out error message. |
		+-------------------------+*/
		strcpy( Buffer, "Unsupported transaction from Diners. Message type = ");
		strcat( Buffer,  Auth_Tx.TLF01_details.message_type );
		strcat( Buffer, ", Processing code = " );
		strcat( Buffer,  Auth_Tx.TLF01_details.processing_code );
		ncdci2_log_message( 1, 2, Buffer, "ncdci2_process_request_from_host",1 );
		return(true);
	}

	/*Network Request message type*/
	if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					DINERS_NETWORK_REQ_MESSAGE,
					DINERS_MESSAGE_TYPE_SIZE))
	{
		strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
		if(strncmp(NMI_Code,DINERS_NMI_SYSTEM_ECHO, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci2_perform_host_logon_request(DINERS_NMI_SYSTEM_ECHO,SOURCE_IS_HOST,&Auth_Tx);
			ncdci2_send_network_status_to_monitor ();
		}
		else if(strncmp(NMI_Code,DINERS_NMI_LOGON, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci2_perform_host_logon_request(DINERS_NMI_LOGON,SOURCE_IS_HOST,&Auth_Tx);
			ncdci2_send_network_status_to_monitor ();
		}
		else if (strncmp(NMI_Code,DINERS_NMI_LOGOFF, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci2_perform_host_logon_request(DINERS_NMI_LOGOFF,SOURCE_IS_HOST,&Auth_Tx);
			ncdci2_send_network_status_to_monitor ();
		}
		else if(strncmp(NMI_Code,DINERS_NMI_KEYTEST, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci2_key_translate();
		}
		else if(strncmp(NMI_Code,DINERS_NMI_KEYCHANGE, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			 ncdci2_log_message( 3, 1, "Received key change message request from Host",
				  		         "ncdci2_process_key_translate_response ", 0 );

			 if (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
			 {
				 len = strlen(Ncf01_Keyblock.iss_cwk_keyblock);
				 memcpy( Ncf01_Keyblock.iss_cwk_keyblock,
						 Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock, len);

				 memcpy( Ncf01_Keyblock.iss_prev_src_station,
						 Ncf01_I_key_exchange_Keyblock.iss_prev_src_station,
						 DINERS_KEY_CHECK_LEN);

				 len = strlen(Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock);
				 memcpy( Ncf01_Keyblock.iss_rtdcwk_keyblock,
						 Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock,
						 (len - 4));
				 Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
				 ncdci2_update_ncf01_keyblock(&Auth_Tx);
			 }
			 else
			 {
				 len = strlen(Ncf01_I_key_exchange.cwk);
				 memcpy( Ncf01_I.cwk, Ncf01_I_key_exchange.cwk, len);
				 memcpy( Ncf01_I.prev_src_station,
						 Ncf01_I_key_exchange.prev_src_station,
						 DINERS_KEY_CHECK_LEN);
				 len = strlen(Ncf01_I_key_exchange.retired_cwk);
				 memcpy( Ncf01_I.retired_cwk,
						 Ncf01_I_key_exchange.retired_cwk,
						 (len - 4));
				 Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
				 ncdci2_update_ncf01(&Auth_Tx);
			 }
		}

		return ;

	}

	else if((0 == strcmp(Auth_Tx.TLF01_details.message_type, DINERS_NETWORK_RESP_MESSAGE)))
	{
		strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
		ncdci2_process_logon_response(NMI_Code,&Auth_Tx);
		return ;
	}

	/*Network Response message type*/
	else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
						 DINERS_NETWORK_RESP_MESSAGE,
						 DINERS_MESSAGE_TYPE_SIZE))
	{
		strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
		ncdci2_process_logon_response(NMI_Code,&Auth_Tx);
		return ;
	}
	else if ( 0 ==strncmp(Auth_Tx.TLF01_details.message_type,
						  DINERS_FILE_UPDATE_REQ_MESSAGE_TYPE,
						  DINERS_MESSAGE_TYPE_SIZE ))
	{
		ncdci2_process_file_update_request( );
		return;
	}

	(void)ncdci2_init_txn( &Auth_Tx );
	strncpy(Auth_Tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
	strncpy(tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

	/* Send switched in request to the authorizer. */
	if((Auth_Tx.TLF01_details.tx_key >= AUTH_SALE) &&
	   (Auth_Tx.TLF01_details.tx_key <= AUTH_INVALID))
	{
		strcpy(Auth_Tx.TLF01_details.issuer_id, AppName);
		strcpy(Auth_Tx.TLF01_details.source_key, ncf01.cwk);
		strcpy(Auth_Tx.TLF01_details.source_pin_format, ncf01.pin_type);

		/* Are we blocking this transaction? */
		if ( true == ncdci2_txn_is_blocked(&Auth_Tx,Buffer) )
		{
			/* Txn is blocked.  Log message and respond to host. */
			ncdci2_log_message( 2, 2, Buffer, "ncdci2_process_request_from_host",1 );
			ncdci2_perform_error_processing( &Auth_Tx );
			strcpy(Auth_Tx.TLF01_details.response_code,"57");
			Auth_Tx.TLF01_details.tx_key =
					ncdci2_match_request_to_response(Auth_Tx.TLF01_details.tx_key);
			ncdci2_incoming_other_host_response(&Auth_Tx);
			return(false);
		}
		else if(false == ncdci2_hhutil_process_host_request(&Auth_Tx))
		{
			ncdci2_perform_error_processing( &Auth_Tx );
			strcpy(Auth_Tx.TLF01_details.response_code,"91");
			Auth_Tx.TLF01_details.tx_key =
					ncdci2_match_request_to_response(Auth_Tx.TLF01_details.tx_key);
			ncdci2_incoming_other_host_response(&Auth_Tx);
			return(false);
		}
	return(true);
	}

	if ( ret_val == false )
	{
		/* Unable to continue processing transaction.
		 * Need to send a declined response to the host.*/
		strcpy( Auth_Tx.TLF01_details.response_code, DINERS_INVALID_TXN );
		ncdci2_send_response_to_host( DINERS_SEND_UNCONDITIONALLY );
	}
	return;
}



/****************************************************************************
 *
 *  NAME:         ncdci2_process_request_for_host
 *
 *  DESCRIPTION:  This function handles requests from the authorizer; it
 *                begins the process of sending a request to the host.
 *
 *  INPUTS:       p_msg_in - PTE Message from authorizer containing txn info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ****************************************************************************/
void ncdci2_process_request_for_host( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data = NULL_PTR;
   CHAR			   err_buf[100] = {0};
   CHAR			   Buffer[200] = {0};
   CHAR  		   temp_source_pin_format[2] = "1"; // its  1 values
   CHAR  		   temp_dest_pin_format[2] = "1";	// its  1 values
   LONG            data_len = 0;
   LONG            auth_len = 0;

   /* Copy transaction record into auth_tx structure. */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   auth_len   = data_len;
   if ( Auth_Tx_Size < data_len )
      auth_len = Auth_Tx_Size;
   p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &Auth_Tx, p_data, auth_len );

   /* Store originator_info coming from Applnk for sending back response to stpmgr - ThoughtFocus Girija Y*/
   if( 0 == strncmp(Auth_Tx.TLF01_details.originator_queue,"stpmgr",6))
   {
	   strcpy( Auth_Tx.TLF01_details.originator_info,
              ptemsg_get_msg_orig_comms_info (p_msg_in)) ;
   }

   /* Set Auth_Tx values. */
   strcpy(Auth_Tx.TLF01_details.tpdu_id,      Ncf01_I.tpdu_id );
   strcpy( Auth_Tx.TLF01_details.acquirer_id, AppName         );
   Auth_Tx.TLF01_details.general_status = GS_TXN_NEW;
   TranFlow = DINERS_SWITCHED_OUT;

   if( (true == ncdci2_is_POS_Acq_transaction()) ||
	   (true == ncdci2_is_voice_transaction()))
   {
	   (void)ncdci2_process_request_for_host_after_security();
   }

   else if ( (!ncdci2_is_reversal()) && ncdci2_pin_block_exists() && true == ncdci2_verify_host_status(err_buf))
   {
		if ( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE )&&
		     (false == ncdci2_is_IST_transaction()))
		{
			memcpy(Auth_Tx.future_use_2,Ncf01_Keyblock.iss_cwk_keyblock,99);
			memcpy(Auth_Tx.future_use_3,Ncf01_Keyblock.iss_cwk_keyblock +99,29);
			memcpy(Auth_Tx.TLF01_details.source_pin_format, temp_source_pin_format,1)  ;/* Source pin format */
			memcpy(Auth_Tx.TLF01_details.dest_pin_format, temp_dest_pin_format,1) ;/* Destination pin format */
			(void)ncdci2_translate_pin_keyblock();
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.dest_key,Ncf01_I.cwk ) ;/* Storing Destination key */
			memcpy(Auth_Tx.TLF01_details.source_pin_format, temp_source_pin_format,1)  ;/* Source pin format */
			memcpy(Auth_Tx.TLF01_details.dest_pin_format, temp_dest_pin_format,1) ;/* Destination pin format */
			(void)ncdci2_translate_pin();
		}
   }
   else
   {
		(void)ncdci2_process_request_for_host_after_security();
   }
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci2_process_request_for_host_after_security
 *
 *  DESCRIPTION:  Continue processing a request for the host. Check to see
 *                if we need to perform currency conversion.
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
void ncdci2_process_request_for_host_after_security()
{
	BYTE   tx_key = 0;
	CHAR   err_buf[100] = {0};
	CHAR   Buff[100] = {0};
	INT    ret_val = true;
	DWORD  txn_amt = 0;
	DWORD  converted_amt = 0;

	/* Process based on transaction type. */
	tx_key = Auth_Tx.TLF01_details.tx_key;
	switch ( tx_key )
	{
		case AUTH_REVERSAL:
		case AUTH_REVERSAL_ADVICE:
			memset( Buff, 0x00, sizeof(Buff) );
			strcpy( Buff,"Reversal going to Diners:");
			ncdci2_log_message( 3, 2, Buff,
								"ncdci2_process_request_for_host_after_security",1 );
		case AUTH_VOID_PRE_AUTHORIZATION:
	         /* Display Pre-auth canceln on Monitor.
	          * This message is used to isolate problems to selected bins
	          * or to certain merchants or to networks.
	          */
			if ( Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
			{
			memset( Buff, 0x00, sizeof(Buff) );
				strcpy( Buff,"Void preauth going to ncdci2:");
			ncdci2_log_message( 3, 2, Buff,
								"ncdci2_process_request_for_host_after_security",1 );
			}
		case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
		case AUTH_AUTHORIZATION_OVERRIDE:

			if ( Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE)
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION;
				/*TF Praneeth Modified to support tx_key revert back while stip transaction*/
				Auth_Tx.TLF01_details.voice_auth_text[0] = AUTH_AUTHORIZATION_OVERRIDE;
			}
			else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION )/*TF Phani- Added CA AUTH*/
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
			}
			else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE )
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
				/*TF Praneeth Modified to support tx_key revert back while stip transaction*/
				Auth_Tx.TLF01_details.voice_auth_text[0] = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE;
			}
			/* Do not break out of switch.  Fall thru to online processing. */
		case AUTH_SALE_CASH:
		case AUTH_SALE:
		case AUTH_AUTHORIZATION:
		case AUTH_CASH_ADVANCE:
		case AUTH_ADVICE:
		case AUTH_ADVICE_REPEAT:
		case AUTH_REFUND:
		case AUTH_VOID_CASH_ADVANCE:
		case AUTH_VOID_REFUND:
		case AUTH_VOID_SALE:
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_PRE_AUTHORIZATION_CANCLE:
		case AUTH_CARD_VERIFICATION:
		case AUTH_BALANCE_INQUIRY:
			break;

		case AUTH_OFFLINE_SALE:
			ret_val = ncdci2_perform_standin_processing( DINERS_ISO_APPROVED,
														 GS_TXN_COMPLETED_OFF,
														 err_buf );
			if ( false == ret_val )
			{
				ncdci2_log_message( 2,2,err_buf,
									"ncdci2_process_request_for_host_after_security",1);
			}
			/* Make sure transaction processing does not continue. */
			ret_val = false;
			break;

		default:
			/* Unknown transaction, reject it. Send response to authorizer. */
			memset( err_buf, 0x00, sizeof(err_buf) );
			sprintf( err_buf,"Invalid txn. Cannot process. TX_KEY = %d",tx_key );
			ncdci2_log_message( 2, 3, err_buf,
								"ncdci2_process_request_for_host_after_security",1 );
			/*TF Praneeth revert back tx_key  while stip transaction */
			if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
			{
				Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.voice_auth_text[0];
				memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
			}

			ret_val = ncdci2_perform_standin_processing( DINERS_INVALID_TXN,
														 GS_TXN_TRANSACTION_NOT_ALLOWED,
														 err_buf );
			if ( false == ret_val )
			{
				ncdci2_log_message(2,2,err_buf,
									"ncdci2_process_request_for_host_after_security",1);
			}

			/* Make sure transaction processing does not continue. */
			ret_val = false;
			break;
	}

	/* Continue processing the request for the host. */
	if ( false != ret_val )
	{
		(void)ncdci2_process_request_for_host_after_currency();
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci2_process_request_for_host_after_currency
 *
 *  DESCRIPTION:  Continue processing a request for the host.  At this point,
 *                we have all the data we need from the transaction and from
 *                the data server.  So all that is left to do is to verify the
 *                host is up, then put the message together and send it to the
 *                host.
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
void ncdci2_process_request_for_host_after_currency()
{
	CHAR err_buf[100] = {0};
	INT  ret_val = true;

	/* Check the status of the host.  Are we online? */
	ret_val = ncdci2_verify_host_status( err_buf );

	/*we are hard coing the status of host for testing we have to remove this*/
	//////////////////////////////////////
	//ret_val = true;
	/////////////////////////////////////

	if ( false == ret_val )
	{
		ncdci2_log_message( 1, 3, err_buf,
							"ncdci2_process_request_for_host_after_currency",1 );

		/*TF Praneeth revert back tx_key  while stip transaction */
		if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) ||
			(Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
		{
			Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.voice_auth_text[0];
			memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
		}
		ret_val = ncdci2_perform_standin_processing( "01",GS_TXN_DROPPED,err_buf );
		if ( false == ret_val )
		{
			ncdci2_log_message(2,2,err_buf,
									"ncdci2_process_request_for_host_after_currency",1);
		}
	}
	else
	{
		/*TF Praneeth clearing buffer variable used to store tx_key  while stip transaction */
		if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) ||
			(Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
		{
			memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
		}
		(void)ncdci2_send_request_to_host( DINERS_TIMER_FINANCIAL );
	}

	return;
}

/**************************************************************************
 *
 *  NAME:         ncdci2_process_response_from_host
 *
 *  DESCRIPTION:  This function is the driver for responses from the host.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ***************************************************************************/
void ncdci2_process_response_from_host()
{
	BYTE  tran_type = 0;
	CHAR  err_buf[100] = {0};
	CHAR  lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};

	/* Get host finish time, for statistic purposes. */
	ptetime_get_strtime( Auth_Tx.TLF01_details.host_finish_time );

	/* Decrement number of txns that are outstanding with the host. */
	if ( false == ncdci2_decrement_active_count( err_buf ) )
	{
		ncdci2_log_message(2, 2, err_buf, "ncdci2_process_response_from_host",0 );
	}

	/* Reset number of consecutive timeouts to zero. */
	if ( false == ncdci2_reset_timeout_count( err_buf ) )
	{
		ncdci2_log_message(2, 2, err_buf, "ncdci2_process_response_from_host",0 );
	}

	/* Get tx_key, making sure it is valid. */
	tran_type = ncdci2_get_tran_type_2(Auth_Tx.TLF01_details.tx_key);
	Auth_Tx.TLF01_details.tx_key = tran_type;
	if(Auth_Tx.TLF01_details.tx_key != AUTH_INVALID)
	{
		/* Handle Network Management transactions separately. */
		if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
		{
			update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, MGT );//praneeth added line with MGT
			ncdci2_process_net_management_from_host();
		}
		else
		{
			/* Update transaction statistics. */
			update_txn_stats( &Txn_Counts, &Auth_Tx, RECEIVED );//praneeth modified NA as RECEIVED

			/* Need to clear the timer; so create the lookup key. */
			ncdci2_create_request_timer_key( lookup_key );

			/* Now clear the request timer;
			* exit code while waiting for timer's response.
			* Timer's response will contain original auth_tx.
			*/
			if ( false == ncdci2_clear_request_timer(lookup_key) )
			{
				sprintf( err_buf,
						"Unable to clear request on response from host. Msg type: %s, DE7: %s",
						Auth_Tx.TLF01_details.message_type,
						Auth_Tx.TLF01_details.transmission_timestamp );
				ncdci2_log_message(2, 2, err_buf, "ncdci2_process_response_from_host",1 );
			}
		}
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci2_process_clear_timer_reply
 *
 *  DESCRIPTION:  Process a reply from the timer.  This function continues
 *                the processing started by 'ncdci2_process_response_from_host'.
 *                It will complete the processing of a response that was
 *                received from a host.
 *
 *  INPUTS:       p_msg_in - This is the message from the timer.  It contains
 *                           two auth_tx buffers: the original auth_tx from
 *                           the acquiring application (dcpiso, ...) and an
 *                           auth_tx containing the host response information.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_process_clear_timer_reply( pPTE_MSG p_msg_in )
{
   pBYTE          p_data            = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
   pPTE_MSG       p_msg_out         = NULL_PTR;
   pPTE_MSG       p_echo_msg        = NULL_PTR;
   CHAR           app_data1[32] = {0};
   CHAR           app_data2[32] = {0};
   CHAR           lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
   CHAR           echo_key[DINERS_LOOKUP_KEY_SIZE]  = {0};
   CHAR           echo_req_key[DINERS_LOOKUP_KEY_SIZE]  = {0};
   CHAR           err_buf[100] = {0};
   LONG           timer_reply_size  = 0;
   LONG           data_len = 0;
   INT			  len = 0;
   AUTH_TX        host_auth_tx;

   
   /* First, extract timer reply from incoming message.
    * This also gets the original Auth_Tx containing all
    * transaction request information.
    */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
                                                app_data2, (pBYTE)&Auth_Tx,
                                                Auth_Tx_Size );

   /* Do not do anything if this is a response to a clear command
    * for the echo timer or for an 0800 echo request.
    */
   ncdci2_create_echo_timer_key( echo_key );
   ncdci2_create_echo_req_timer_key( echo_req_key );
   if ( (0 != strcmp(echo_key,     lookup_key)) &&
        (0 != strcmp(echo_req_key, lookup_key))  )
   {
      /* Verify that timerds was able to successfully
       * retrieve timer and our original auth_tx data.
       * This is the auth_tx we sent along with the
       * 'start timer' request prior to going to the host.
       */
      if( ptemsg_get_result_code( p_msg_in ) == PTEMSG_NOT_FOUND )
      {
         /* Timerds was unable to retrieve our
          * data for the lookup key we provided.
          */
         sprintf( err_buf,
                 "Unable to retrieve timer data for lookup key %s",
                  lookup_key );
         ncdci2_log_message( 1, 3, err_buf, "ncdci2_process_clear_timer_reply",1 );
      }

      /* Verify that the data size returned matches auth_tx size */
      else if ( timer_reply_size != Auth_Tx_Size )
      {
         sprintf( err_buf,
                 "Did not receive auth_tx data with timer reply. Key = %s",
                  lookup_key );
         ncdci2_log_message( 2, 2, err_buf, "ncdci2_process_clear_timer_reply",1 );
      }
      else
      {
         /* Get auth_tx record that was chained to the 'clear timer' message.
          * This is the auth_tx record built from response data from the
          * host.
          */
         p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
         p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
         p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
         data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

         if ( data_len != Auth_Tx_Size )
         {
            sprintf( err_buf,
                 "Did not receive chained host response data with timer reply. Key = %s",
                  lookup_key );
            ncdci2_log_message( 2, 2, err_buf, "ncdci2_process_clear_timer_reply",0 );
         }
         else if(0 == strcmp(DINERS_TIMER_REVERSAL, app_data2))
         {
            /* Log the reversal caused due to timeout into TLF01.
             * No need to send response to originator; a timeout response 
             * should already have been sent back. Need to change transaction id
             * to avoid unique constraint violation with the timed out transaction.
             * Increase the last digit by 1.
             */
            len = strlen( Auth_Tx.TLF01_details.primary_key.transaction_id);

            if(Auth_Tx.TLF01_details.primary_key.transaction_id[len - 1] == '9')
               Auth_Tx.TLF01_details.primary_key.transaction_id[len - 1]  = '0';
            else
               Auth_Tx.TLF01_details.primary_key.transaction_id[len - 1]  += 1;

            (void)ncdci2_insert_db_record( TLF01_DATA );
         }
         else
         {
            /* Get auth_tx record that was built from host response data. */
            memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
            memcpy( &host_auth_tx, p_data, Auth_Tx_Size );

            /* Move response info from host into Auth_Tx. */
            (void)ncdci2_copy_resp_into_request( &host_auth_tx );

            /* Restore original amount, if currency conversion took place. */
            if ( Ncf01_I.currency_code_cash[0] != 0x00 )
            {
               if ( Auth_Tx.reconciliation_cur_code[0] != 0x00 )
                  memcpy( Auth_Tx.TLF01_details.currency_code,
                          Auth_Tx.reconciliation_cur_code,
                          sizeof(Auth_Tx.TLF01_details.currency_code));

               if ( Auth_Tx.reconciliation_amount[0] != 0x00 )
                  memcpy( Auth_Tx.TLF01_details.total_amount,
                          Auth_Tx.reconciliation_amount,
                          sizeof(Auth_Tx.TLF01_details.total_amount));
            }

            /* Indicate a Remote Authorization, not local. */
            strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

            /* Set tx_key to a 'response' value. */
            (void)ncdci2_set_tx_key_to_response();

            /* Update the general status. */
            Auth_Tx.TLF01_details.general_status = GS_TXN_COMPLETED;

            if ( Auth_Tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE ||
            	 Auth_Tx.TLF01_details.tx_key == AUTH_CARD_UPDATE)
            (void)ncdci2_issue_1304_response( Auth_Tx.TLF01_details.response_code, err_buf );

            /* Forward the response to the authorizer. */
             if ( false == ncdci2_send_msg_to_authorizer(MT_AUTH_RESPONSE) )
            {
               sprintf( err_buf,
                    "Unable to complete processing of response msg. MsgType: %s",
                     Auth_Tx.TLF01_details.message_type);
               ncdci2_log_message( 3, 3, err_buf, "ncdci2_process_clear_timer_reply",1 );
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci2_copy_resp_into_request
 *
 *  DESCRIPTION:  This procedure copies information that came from the host in
 *                a response into the Auth_Tx buffer.
 *
 *  INPUTS:       host_auth_tx - Transaction response record from host
 *
 *  OUTPUTS:      Auth_Tx - (Global) Txn record is updated with response info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_copy_resp_into_request( pAUTH_TX host_auth_tx )
{
	BYTE temp_future_field1[4] = {0};
	BYTE asci_emv_len[5] 		= {0};
	INT emv_len = 0;
   if ( AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key )
   {
      strcpy( Auth_Tx.TLF01_details.total_amount,
              host_auth_tx->TLF01_details.total_amount);
      if(  true == ncdci2_is_IST_transaction())
      {
    	  strncpy( Auth_Tx.TLF01_details.processing_code,
    	           host_auth_tx->TLF01_details.processing_code,
				   DINERS_DE3_SIZE);
      }
   }

   /* Copy balance info for Balance Inquiries and Cash Txns. */
   strcpy( Auth_Tx.current_balance,   host_auth_tx->current_balance   );
   strcpy( Auth_Tx.available_balance, host_auth_tx->available_balance );

   Auth_Tx.current_balance_sign   = host_auth_tx->current_balance_sign;
   Auth_Tx.available_balance_sign = host_auth_tx->available_balance_sign;

   /* Copy balances into these fields; ncvisa looks for them here. */
   strcpy( Auth_Tx.ACF01_details.cash_available, Auth_Tx.current_balance );
   strcpy( Auth_Tx.ACF01_details.cash_limit, Auth_Tx.available_balance );

   /* Copy balances into these fields for Equitable's proprietary reports. */
   strcpy( Auth_Tx.TLF01_details.outstanding_balance,
           host_auth_tx->TLF01_details.outstanding_balance );
   strcpy( Auth_Tx.TLF01_details.credit_limit,
           host_auth_tx->TLF01_details.credit_limit );

   /* Copy balance currency code. */
   strcpy( Auth_Tx.host2_src_fi_nbr, host_auth_tx->host2_src_fi_nbr );

   if ( host_auth_tx->TLF01_details.auth_number[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.auth_number,
              host_auth_tx->TLF01_details.auth_number );
   }

   if (strlen(host_auth_tx->TLF01_details.response_code) > 0)
   {
	   strcpy( Auth_Tx.TLF01_details.response_code,
			   host_auth_tx->TLF01_details.response_code );
   }

   // if cadencie did not send RC, send it as "00" in 130 response
   if (0 == strncmp(host_auth_tx->TLF01_details.message_type,DINERS_ADVICE_RESP_MESSAGE, 4) &&
		   strlen(host_auth_tx->TLF01_details.response_code) == 0)
   {
	   strcpy( Auth_Tx.TLF01_details.response_code,
			   DINERS_ISO_APPROVED);
   }

   if ( 0 != strcmp(Auth_Tx.TLF01_details.response_code,DINERS_ISO_APPROVED) )
   {
      strcpy( Auth_Tx.TLF01_details.response_text,
              host_auth_tx->TLF01_details.response_text );

      if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y' )
      {
         /* Txn is a Voice Auth. Copy response text to voice text field.*/
         strcpy( Auth_Tx.TLF01_details.voice_auth_text,
                 host_auth_tx->TLF01_details.voice_auth_text );
      }
   }

   strcpy( Auth_Tx.TLF01_details.host_finish_time,
           host_auth_tx->TLF01_details.host_finish_time );

   /* Copy VISA CPS information */
   if ( Auth_Tx.TLF01_details.visa_char_indicator != 0x00 )
   {
      Auth_Tx.TLF01_details.visa_char_indicator =
                                host_auth_tx->TLF01_details.visa_char_indicator;
   }
   
      strcpy( Auth_Tx.TLF01_details.visa_tran_id,
              host_auth_tx->TLF01_details.visa_tran_id );
   
   if ( Auth_Tx.TLF01_details.visa_validation_code[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.visa_validation_code,
              host_auth_tx->TLF01_details.visa_validation_code );
   }

   /* Copy Account Identification information. */
   strcpy( Auth_Tx.acct_id_1, host_auth_tx->acct_id_1 );
   strcpy( Auth_Tx.acct_id_2, host_auth_tx->acct_id_2 );

   /* Copy field 44 - ThoughtFocus Girija Y, Phani */
   memcpy(Auth_Tx.TLF01_details.product_codes[19].code,
	      host_auth_tx->TLF01_details.product_codes[19].code,
		  sizeof(host_auth_tx->TLF01_details.product_codes[19].code));

   /* Diners Response Code - used for Online Updates (1304/1314) */
   strcpy( Auth_Tx.accptor_bus_code, host_auth_tx->accptor_bus_code );
   /* TF Phani - For VOICE CA- Trasnctions should go as AUTH-
	During request we have changed to CA and changes back to CA_AUTH		
   */
    if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y'  && Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;

	if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y'  && Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE)
	   Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL;

	/* Copy DE22 back to AUTH_TX amex_pos_entry_mode*/
	//if(strlen(Auth_Tx.amex_pos_entry_mode)== 0)
	//strcpy(Auth_Tx.amex_pos_entry_mode,host_auth_tx->amex_pos_entry_mode);
	/*Copy Resp buffer into AUTH TX*/
	/*Copy resp buffer and length into outgoing message*/
	if(atoi(host_auth_tx->resp_data_len) > 0 && atoi(host_auth_tx->resp_data_len) <  DINERS_COMM_BUF_SZE )
	{	
		memcpy(Auth_Tx.resp_data_len,host_auth_tx->resp_data_len,sizeof(host_auth_tx->resp_data_len));
		memcpy(Auth_Tx.resp_buffer,host_auth_tx->resp_buffer,sizeof(host_auth_tx->resp_buffer));
	}

	// Store Settlement Date coming from HOST
	memcpy(Auth_Tx.TLF01_details.settlement_date,host_auth_tx->TLF01_details.settlement_date,sizeof(Auth_Tx.TLF01_details.settlement_date));

	// Store transmission time coming from HOST.
	if(strlen(host_auth_tx->TLF01_details.transmission_timestamp)>0)
	{
		memcpy(Auth_Tx.TLF01_details.transmission_timestamp,host_auth_tx->TLF01_details.transmission_timestamp,sizeof(Auth_Tx.TLF01_details.transmission_timestamp));
	}
	printf("Transmission timestamp for STAN%s is %s \n",Auth_Tx.TLF01_details.sys_trace_audit_num,
															Auth_Tx.TLF01_details.transmission_timestamp);

	// Copy EMV fields
	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncdci",5))
	{
		emv_len = genutil_bcd_to_int(host_auth_tx->EMV_details.future_field1,2);
		sprintf(asci_emv_len,"%03d",emv_len );
		strncpy(Auth_Tx.EMV_details.future_field1,asci_emv_len,3);
		memcpy(&Auth_Tx.EMV_details.future_field1[3],&host_auth_tx->EMV_details.future_field1[2],sizeof(Auth_Tx.EMV_details.future_field1)-3);

	}
	else
	{
	memcpy(Auth_Tx.EMV_details.future_field1,host_auth_tx->EMV_details.future_field1,
	sizeof(Auth_Tx.EMV_details.future_field1));

	memcpy(Auth_Tx.EMV_details.future_field2, host_auth_tx->EMV_details.future_field2,
			sizeof(Auth_Tx.EMV_details.future_field2));
	}


	if(strlen(host_auth_tx->TLF01_details.resp_source)>0)
	{
		memcpy(Auth_Tx.TLF01_details.resp_source,host_auth_tx->TLF01_details.resp_source,
			(sizeof(Auth_Tx.TLF01_details.resp_source)-1));
	}

	//Copy action code.
	if (strlen(host_auth_tx->TLF01_details.product_codes[12].code)> 0)
	{
		memcpy(Auth_Tx.TLF01_details.product_codes[12].code,
		       host_auth_tx->TLF01_details.product_codes[12].code,
			   sizeof(host_auth_tx->TLF01_details.product_codes[12].code));
	}

   return;
}
  
/******************************************************************************
 *
 *  NAME:         ncdci2_process_1304_from_host
 *
 *  DESCRIPTION:  This procedure prepare the buffer to send the file update 
 *                (1304) request to Diners Host.
 *
 *  INPUTS:       host_auth_tx - Transaction request record from host
 *
 *  OUTPUTS:      Auth_Tx - (Global) Txn record is updated with request info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Girija Y - ThoughtFocus 
 *
 ******************************************************************************/
void ncdci2_process_1304_from_host()
{
	BYTE     buffer[sizeof(AUTH_TX) + sizeof(AMEX_ONLINE_UPDATES)] = {0};
	AMEX_ONLINE_UPDATES FileUpdate;
	INT         ret_val;
    CHAR        err_msg[100] = {0};
	memset( buffer, 0, sizeof(buffer) );
	memset( &FileUpdate, 0, sizeof(AMEX_ONLINE_UPDATES));

	memcpy( FileUpdate.card_nbr,Auth_Tx.TLF01_details.card_num,
		    sizeof(FileUpdate.card_nbr));

	memcpy( FileUpdate.exp_date,Auth_Tx.TLF01_details.exp_date,
		    sizeof(FileUpdate.exp_date));

	memcpy( FileUpdate.function_code,Auth_Tx.function_code,
		    sizeof(FileUpdate.function_code));

	memcpy( FileUpdate.reason_code,Auth_Tx.TLF01_details.nii,
		    sizeof(FileUpdate.reason_code));

	memcpy( FileUpdate.date_time,Auth_Tx.TLF01_details.date_yyyymmdd,
		    sizeof(Auth_Tx.TLF01_details.date_yyyymmdd));

	memcpy( &FileUpdate.date_time[8],Auth_Tx.TLF01_details.time_hhmmss,
		    sizeof(Auth_Tx.TLF01_details.time_hhmmss));

	memcpy( FileUpdate.fwd_inst_code,Auth_Tx.forwarding_inst_id_cd,
		    sizeof(FileUpdate.fwd_inst_code));

	memcpy( FileUpdate.stan,Auth_Tx.TLF01_details.sys_trace_audit_num,6);


	memcpy( buffer + sizeof(AUTH_TX), &FileUpdate,sizeof(AMEX_ONLINE_UPDATES));

	/*Need to modified based on Diners*/
/*
	ret_val = ncdci2_build_and_send_request_message( MT_INCOMING_GUI, 
                                      ST1_NONE,
                                      ST2_NONE,
                                      pos_amex_que_name,  // , Not applicable for ATM
                                      (pBYTE)&buffer,
                                      sizeof( buffer ),
                                      0,
                                      NULL_STR,
                                      err_msg );


	if ( false == ret_val )
   {
   	   	   ncdci2_log_message( 1, 1, err_msg",
								  "ncdci2_process_1304_from_host",0 );
   }*/

} /* ncdci2_process_1304_from_host */


/******************************************************************************
*
*  NAME:         ncdci2_delete_orig_ncf30
*
*  DESCRIPTION:  This function sends a message to the data server to delete
*                a particular NCF30 record.  This will be used to delete 
*                transactions that are being reversed.
*
*  INPUTS:       p_auth_tx - Transaction record 
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred 
*
*  AUTHOR:       Abhishek Verma
*
*  MODIFICATIONS:

********************************************************************** ********/
INT ncdci2_delete_orig_ncf30( NCF30 ncf30, pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)] = {0};
   CHAR            Buff[256] = {0};
   CHAR            err_mesg[200] = {0};
   LONG            ret_code =  false;
 
   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, sizeof(AUTH_TX) );
   memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_DELETE, 
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF30_DATA );
   if ( NULL_PTR == p_msg  )
   {
	   ncdci2_log_message( 1, 3,  "Insufficient Memory to build NCF30 message",
			   	   	   	   	   	   "ncdci2_delete_orig_ncf30",1 );
       return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s", netds_que_name, err_mesg );
      ncdci2_log_message( 1, 3, Buff,"ncdci2_delete_orig_ncf30",1 );
      return( false );
   }
   return(true);
 }

 
 /*****************************************************************************
 
   Function:	ncdci2_process_logon_response
 
   Description: This function will zero out all counters used in the host handler
				for determining the host's status.	(These counters are in shared
				memory).
   Author: 
	   Abhishek Verma
   Inputs:
	   None
   Outputs:
	   None
   Return values:
	   true  - always returns true
   Modified by:
******************************************************************************/
 BOOLEAN ncdci2_process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx)
 {
	pPTE_MSG  p_msg_host_app = NULL_PTR;
	pPTE_MSG   p_msg_ins = NULL_PTR;
	BYTE	   buffer1[sizeof(AUTH_TX)+sizeof(TLF01)] = {0};
	BYTE	  net_consec_tmouts_ctr[4] = {0};
	BYTE	  active_txns_ctr[4] = {0};
	BYTE	  current_state[2] = {0};
	CHAR	  unique_tran_str_id[17] = {0};
	CHAR      table_name[15] = {0};
	CHAR	  ErrorMsg[200] = {0};
	INT 	  update_host_status = false;
	INT 	   insert_flag = 0;
	LONG	   ret_code = false;

	memset( table_name, 0x00, sizeof(table_name) );
    strcpy( table_name, AppName );
    strcat( table_name, "Table" );
 
	/* Get host state and counters */
	ReadGenericTable( table_name, net_consec_tmouts_ctr, 
					  active_txns_ctr, current_state );
	strcpy(net_consec_tmouts_ctr, "0");
 
	if (strcmp(LOGON_NMI_LOGON ,p_auth_tx->function_code) == 0)
	{
	   if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
	   {
		  strcpy(current_state, "0");
		  update_host_status = true;
		  ncdci2_log_message( 1, 1, "DINERS_LOGON Request successful.",
				  	  	  	  	  "ncdci2_process_logon_response",0 );
	   }
	   else
	   {
		   ncdci2_log_message( 1, 1, "DINERS_LOGON Request DENIED.",
		   				  	  	  	  "ncdci2_process_logon_response",0 );
	   }
	}
	
	else if (strcmp(LOGON_NMI_LOGOFF,p_auth_tx->function_code) == 0)
	{
	   if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
	   {
		  strcpy(current_state, "1");
		  update_host_status = true;
		  ncdci2_log_message( 1, 1, "DINERS_LOGOFF Request successful.",
		 		   				  	 "ncdci2_process_logon_response",0 );
	   }
	   else
	   {
		  ncdci2_log_message( 1, 1, "DINERS_LOGOFF Request successful.",
		  		 		   			"ncdci2_process_logon_response",0 );
	   }
	}
	 
	else if (strcmp(LOGON_NMI_ECHO_TEST,p_auth_tx->function_code) == 0)
	{
	   if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
	   {
		  strcpy(current_state, "1");
		  update_host_status = true;
		  ncdci2_log_message( 1, 1, "ECHO Request successful.",
		  		  		 		   	"ncdci2_process_logon_response",0 );
	   }
	   else
	   {
		   ncdci2_log_message( 1, 1, "ECHO Request DENIED.",
		   		  		  		 	 "ncdci2_process_logon_response",0 );
		}
	}
	else if (strcmp(LOGON_NMI_KEY_RESET_REQUEST,p_auth_tx->function_code) == 0)
	{
	   if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
	   {
		   ncdci2_log_message( 1, 1, "Key Exchange Success successful.",
		  		   		  		  	"ncdci2_process_logon_response",0 );
	   }
	   else
	   {
		  ncdci2_log_message( 1, 1, "Key Exchange Request DENIED.",
		   		  		   		  	"ncdci2_process_logon_response",0 );
	   }
	   return true;
	}

	/* Transaction is completed.  Insert it into the database. */
	
    memcpy(p_auth_tx->TLF01_details.message_type,ReqRsp_Buffer,4);
	ptetime_get_strtime(unique_tran_str_id);
	memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
	memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id, p_auth_tx->TLF01_details.terminal_id, sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
	strncat(p_auth_tx->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);
 
	memset(buffer1, 0, sizeof(buffer1) );
	memcpy(buffer1+sizeof(AUTH_TX),&p_auth_tx->TLF01_details,sizeof(TLF01));
 
	p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
								 ST1_DB_INSERT, 
								 ST2_NONE,
								 updateds_que_name, 
								 application_que_name,
								 (pBYTE)&buffer1,
								 sizeof(buffer1), 
								 TLF01_DATA); 
	if ( NULL_PTR == p_msg_ins )
	{
		ncdci2_log_message( 1, 3, "Insufficient Memory to build insert TLF01 message.",
				   		  		   "ncdci2_process_logon_response",1 );
	   return(false);
	}
	else
	{
	   ret_code = pteipc_send( p_msg_ins, updateds_que_name );
	   free( p_msg_ins );
	}
 
	if ( update_host_status == true )
	{
	   /* Update host state and counters */
	   WriteGenericTable( table_name, net_consec_tmouts_ctr, 
						  active_txns_ctr, current_state );
	   (void)ncdci2_send_network_status_to_monitor();
	}
	return(true);
 }
 
 /*****************************************************************************
 
   Function:	ncdci2_process_incoming_message
 
   Description: This function will process an incoming message from the authorizer,
				it may be a request or a response.
   Author: 
	   Abhishek Verma
   Inputs:
	   pmsg - a pointer to the incoming message that contains the auth_tx
   Outputs:
	   None
   Return values:
	   true  - indicates successful processing.
	   false - indicates no successful processing.
   Modified by:
	   Abhishek Verma	   
 ******************************************************************************/
 INT ncdci2_process_incoming_message(pPTE_MSG p_msg)
 {
	pPTE_MSG_DATA	p_msg_data = NULL_PTR;
	pBYTE			p_data_data = NULL_PTR;
	LONG			data_len;
	LONG			auth_len;
	AUTH_TX 		auth_tx;
	CHAR           errbuf[256] = {0};

	memset(&Auth_Tx, 0x00, sizeof(AUTH_TX));
 
	p_msg_data = ptemsg_get_pte_msg_data(p_msg);
	data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
	auth_len = data_len;
	if(sizeof(Auth_Tx) < data_len)
	{
	   auth_len = sizeof(Auth_Tx);
	}
	p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
	memcpy(&Auth_Tx, p_data_data, auth_len);
	p_data_data = p_data_data + sizeof(Auth_Tx);
 
	/* No need to continue for authorization advices.
	 * They have already been answered.
	 */
	/*if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE )
	{
	 //Send a msg to updateds to restore the balances.
	 ncdci2_update_database(&auth_tx );
	 return( true );
	}*/
	/* TF PHANI..If it is ATM_CONFIRM it is reversal response */
	if(Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE)
	{
	   Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
	}
 
	strncpy(Auth_Tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
 
	if(!(Auth_Tx.TLF01_details.tx_key == AUTH_LOGON_RESPONSE))
	{
	   if((Auth_Tx.TLF01_details.tx_key < AUTH_SALE) ||
		  (Auth_Tx.TLF01_details.tx_key > AUTH_INVALID))  /* a response */
	   {
		  Auth_Tx.TLF01_details.tx_key =
						 ncdci2_match_request_to_response(Auth_Tx.TLF01_details.tx_key);
 
		  /* Restore the conversion rate. */
		  if ( Auth_Tx.settlement_conv_rate[0] != 0x00 )
		  {
			 strcpy( Auth_Tx.TLF01_details.conversion_rate,
					 Auth_Tx.settlement_conv_rate );
		  }
		 
		  if((0 == strcmp(Auth_Tx.TLF01_details.response_code, "00")) || (0 == strcmp(Auth_Tx.TLF01_details.response_code, "55")))
		  {
			  if(true == ncdci2_check_for_cadencieacq_transaction())
			  {
				  if(true != ncdci2_verify_host_status(errbuf))
				  {
					  // timeout
					  strcpy( Auth_Tx.TLF01_details.response_code, DINERS_TIMEOUT );
				  }
			  }
		  	  if ( false == ncdci2_update_database( ))
			  {

		  		ncdci2_log_message( 1, 3, "Unable to update database tables.",
		  						   		  "ncdci2_process_incoming_message",1 );
				/* Unable to complete a transaction.  Inform host; log to TLF01. */
				 if(false == ncdci2_incoming_other_host_response_db_reply( ) )
				 {
					 ncdci2_log_message( 1, 3, "Unable to send response to host.",
					 		  					"ncdci2_process_incoming_message",1 );
				 }
			  }
			  
		  }
		  else
		  {
			  /* 
				  since the transaction control responded with response code other than 00, 
				  Database is not updated. Sending back the response to Host. 
				  Insert into TLF01.
			  */
			  if(false == ncdci2_incoming_other_host_response_db_reply())
			  {
				  ncdci2_log_message( 1, 3, "Unable to send response to host.",
				  					 	 	 "ncdci2_process_incoming_message",1 );
			  }
		  }
		  return(true);
	   }
	}
 
	return( true );
 }	

 
 
 
 /*****************************************************************************
 
   Function:	ncdci2_send_it_back_to_terminal
 
   Description: This function will send the auth_tx buffer back to the authorizer.
   Author: 
	   Abhishek Verma
   Inputs:
	   resp_code - The response code to be sent to the terminal
	   p_auth_tx - A pointer to the auth_tx buffer to be sent to the authorizer.
   Outputs:
	   None
   Return values:
	   true  - indicates that the message was sent successfully.
	   false - indicates that the message could not be sent successfully.
   Modified by:
	   Abhishek Verma	   
 ******************************************************************************/
 BOOLEAN ncdci2_send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx )
 {
	pPTE_MSG	p_msg_term_handler_out = NULL_PTR;
	CHAR	   Buffer[256] = {0};
	CHAR	   ErrorMsg[200] = {0};
	CHAR		sender_que_name[17] = {0};
	LONG		ret_code = false;
 
	strcpy(p_auth_tx->TLF01_details.response_code, resp_code);
	p_auth_tx->TLF01_details.tx_key = ncdci2_match_request_to_response(p_auth_tx->TLF01_details.tx_key);
	strcpy(sender_que_name, authorizer_que_name);
	p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
											   ST1_NONE,
											   ST2_NONE,
											   sender_que_name,
													   application_que_name,
											   (pBYTE)p_auth_tx,
											   sizeof(AUTH_TX),
											   0 );
	if ( NULL_PTR == p_msg_term_handler_out )
	 {
		ncdci2_log_message( 1, 3, "Insufficient Memory to build response to txcntl.",
				  				   "ncdci2_send_it_back_to_terminal",1 );
	  return(false);
	 }
	ret_code = pteipc_send (p_msg_term_handler_out, sender_que_name );
	if ( ret_code < 0 )
	{
		 pteipc_get_errormsg( ret_code, ErrorMsg );
	   sprintf( Buffer, "error on pteipc_send to que %s: %s", sender_que_name, ErrorMsg );
	   ncdci2_log_message( 1, 3, Buffer,"ncdci2_send_it_back_to_terminal",1 );
	   free(p_msg_term_handler_out);
	   return(false);
	}
	else
	{
		ncdci2_log_message( 2, 3, "Response sent successfully to terminal handler",
								"ncdci2_send_it_back_to_terminal",1 );
	}
 
	free(p_msg_term_handler_out);
	return(true);
 }

 /*****************************************************************************
 
   Function:	ncdci2_hhutil_verify_pin_block
 
   Description: This function will determine whether or not there is a pin block
				in the auth_tx
   Author: 
	   Abhishek Verma
   Inputs:
	   p_auth_tx - the auth_tx structure
   Outputs:
	   None
   Return values:
	   true  - pin block
	   false - no pin block 
   Modified by:
 ******************************************************************************/
 BOOLEAN ncdci2_hhutil_verify_pin_block(pAUTH_TX p_auth_tx)
 {
	if(0 == strlen(p_auth_tx->TLF01_details.pin_block))
	   return(false);
	if(ncdci2_blank_string(p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block)))
	   return(false);
	return(true);
 }
 
 
 /*****************************************************************************
 
   Function:	ncdci2_hhutil_process_debit_transaction
 
   Description: This function will request pin translation.  (The pin translation
				will convert the pin that is encrypted using the terminal key to 
				a pin encrypted using the host key).
   Author: 
	   Abhishek Verma
   Inputs:
	   p_auth_tx - pointer to the auth_tx structure 
   Outputs:
	   None
   Return values:
	   true  - successful 
	   false - an error occurred when trying to send message to security function.
   Modified by:
 ******************************************************************************/
 BOOLEAN ncdci2_hhutil_process_debit_transaction(pAUTH_TX p_auth_tx)
 {
	pPTE_MSG  p_msg = NULL_PTR;
	CHAR	 Buffer[256] = {0};
	CHAR	 ErrorMsg[100] = {0};
	LONG	  ret_code = false;
  
	p_msg = ptemsg_build_msg(MT_NSP_XLATETOANSI, 
							 ST1_NONE, 
							 ST2_NONE, 
							 security_que_name, 
							 application_que_name, 
							 (pBYTE)p_auth_tx, 
							 sizeof(AUTH_TX), 
							 0);
 
 
	
	if ( NULL_PTR == p_msg )
	 {
		ncdci2_log_message( 1, 3, "Insufficient Memory to build pin translation message",
								  "ncdci2_hhutil_process_debit_transaction",1 );
	   return(false);
	 }
	 ret_code = pteipc_send(p_msg, security_que_name);
	 if( ret_code < 0 )
	 {
		pteipc_get_errormsg( ret_code, ErrorMsg );
	   sprintf( Buffer, "error on pteipc_send to que %s: %s", security_que_name, ErrorMsg );
	   ncdci2_log_message( 1, 3, Buffer,
	   							"ncdci2_hhutil_process_debit_transaction",1 );
	   free( p_msg );
	   return(false);
	 }
	 
	free( p_msg );
	return(true);
 }
 
 /******************************************************************************
  *
  *  NAME:		   ncdci2_ncdci2_process_incoming_message_continued
  *
  *  DESCRIPTION:  This function continues the processing of an incoming
  * 			   message.  It is used as a common stepping stone to get
  * 			   to 'ncdci2_incoming_terminal_request'.	It is called after NCF01
  * 			   data is obtained, and also after PIN verification has
  * 			   been performed.
  *
  *  INPUTS:	   p_auth_tx - Transaction record
  *
  *  OUTPUTS:	   None
  *
  *  RTRN VALUE:   True if successful, false is a problem occurred
  *
  *  AUTHOR:	   Abhishek Verma
  *
  *  MODIFICATIONS:
  ******************************************************************************/
 INT ncdci2_ncdci2_process_incoming_message_continued( pAUTH_TX p_auth_tx )
 {
	if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
	{
	   if(ncdci2_incoming_terminal_request(p_auth_tx))
		  return(true);
	   return(false);
	}
	return(false);
	
 }
 
 
 
 /*****************************************************************************
 
   Function:	ncdci2_incoming_terminal_request
 
   Description: This function will process an incoming message from the terminal 
				(via the authorizer)
   Author: 
	   Abhishek Verma
   Inputs:
	   p_auth_tx - a pointer to the incoming auth_tx
   Outputs:
	   None
   Return values:
	   true  - indicates that the message processed successfully.
	   false - indicates that the message could not be processed successfully.
   Modified by:
	   Abhishek Verma	   
	   DI - 04/14/99 - Added offline cases for override transactions.
 ******************************************************************************/
 INT ncdci2_incoming_terminal_request(pAUTH_TX p_auth_tx)
 {
	pPTE_MSG_DATA		 p_msg_data = NULL_PTR;
	BYTE				 temp_tx_key = 0;
	BYTE			     current_state[2] = {0};


	if ((p_auth_tx->TLF01_details.general_status > GS_NSP_KEYCHANGE_FAILED) && ( p_auth_tx->TLF01_details.general_status <= GS_NSP_OUTOFSERVICE))
	{
	   if(ncdci2_send_it_back_to_terminal("58", p_auth_tx ))
		  return(true);
	   return(false);
	}
	 
	/* update the general_status for auth_tx */
	strcpy(p_auth_tx->TLF01_details.acquirer_id, AppName);
	p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
 
 
	/*****************************************************************************************
	/
	 / Check if we do standin processing or if we need to go online
	/ to the host for this transaction 
	/
	/*****************************************************************************************/
	temp_tx_key = p_auth_tx->TLF01_details.tx_key;
	 switch ( temp_tx_key)
	{
	   case AUTH_OFFLINE_SALE: 
		 {	 /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			return(true); 
		 }
	   case AUTH_REFUND: 
		 {	 /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
	   case AUTH_OFFLINE_REFUND: 
		 {	 /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
	   case AUTH_VOID_SALE:
	   { /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
	   case AUTH_VOID_REFUND:
	   { /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
		 case AUTH_SALE_ADJUSTMENT: 
		 {	 /* Delete the original transaction from the auth_tx_table if it is there */
		  /* create transaction type string and store it in the auth_tx_block	   */
		 
			 p_auth_tx->TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			return(true);	
		 }
	   case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE: 
	   {
		  p_auth_tx->TLF01_details.tx_key =
					  AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE;
		  ncdci2_perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
		  return( true );
	   }
	   case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE: 
	   {
		  p_auth_tx->TLF01_details.tx_key =
					  AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE;
		  ncdci2_perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
		  return( true );
	   }
	   case AUTH_AUTHORIZATION_OVERRIDE: 
	   {
		  p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_OVERRIDE_RESPONSE;
		  ncdci2_perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
		  return( true );
	   }
		case AUTH_REVERSAL: 
		case AUTH_REVERSAL_ADVICE: 
	   { /* Create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_PERF_REQ, p_auth_tx);
		  p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
		  if (0 != atoi(p_auth_tx->TLF01_details.reversal_amount))
		  {
			 strcpy(p_auth_tx->TLF01_details.response_code, "96");
		  }
			 break;
		 }
		 case AUTH_REFUND_ADJUSTMENT: 
		 {	 /* Delete the original transaction from the auth_tx_table if it is there */
		  /* create transaction type string and store it in the auth_tx_block */
 
			/* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_ADJUSTMENT_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			return(true);	 
		 }
	   case AUTH_VOID_CASH_ADVANCE: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_VOID_CASH_ADVANCE_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
	   case AUTH_SALES_COMPLETION: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_SALES_COMPLETION_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
	   case AUTH_OFFLINE_VOID_SALE: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_SALE_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
	   case AUTH_OFFLINE_VOID_REFUND: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_REFUND_RESPONSE;
			 ncdci2_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
		 default:  /* online transaction to host */
		 {
		 }
	 } /* end switch statement */ 
	
	/*************************************************************************************/
	
	/* confirm host is available */
	if (false == ncdci2_verify_host_app_status(p_auth_tx))
	{
	   /* build response_for_terminal */
		ncdci2_log_message( 1, 3, "Host Inactive",
			   					"ncdci2_incoming_terminal_request",1 );
		p_auth_tx->TLF01_details.tx_key = ncdci2_match_request_to_response(p_auth_tx->TLF01_details.tx_key);
 
		if (0 == strcmp(current_state,"2"))  /* check if we are error state */
	    {
			ncdci2_log_message( 1, 2, "Host Inactive: current_state!=0",
						   			  "ncdci2_incoming_terminal_request",1 );
		    /* set up a logon request to the host in temp_auth_tx, increment host_app */
		    ncdci2_perform_host_logon_request(DINERS_NMI_LOGON, SOURCE_IS_FSVISA, p_auth_tx);
		   /* build PTEMSG's to be sent */
			 ncdci2_send_request_to_host(p_auth_tx);
	   }
 
	   ncdci2_perform_standin_processing("58", GS_TXN_DROPPED, p_auth_tx);
		 return(true);
	}
 
	/* build PTEMSG's to be sent */
	 ncdci2_send_request_to_host(p_auth_tx);
	
	 return(true);
 } /* ncdci2_incoming_terminal_request */

 
 
 /*****************************************************************************
 
   Function:	ncdci2_perform_host_logon_request
 
   Description: This function will send a logon request to the host to ensure
				that the host is up and running.
   Author: 
	   Abhishek Verma
   Inputs:
	   request_type - the type of request
	   source - indicates the source of the logon request
	   p_auth_tx - the auth_tx structure
   Outputs:
	   None
   Return values:
	   true  - indicates successful processing.
	   false - indicates no successful processing.
   Modified by:
	   Abhishek Verma	   
 ******************************************************************************/
 INT ncdci2_perform_host_logon_request(char request_type[], int source,
								pAUTH_TX p_auth_tx)
 {
	BYTE        tempstr[10] = {0};
	BYTE		net_consec_tmouts_ctr[4] = {0};
	BYTE		active_txns_ctr[4] = {0};
	BYTE		current_state[2] = {0};
	CHAR        table_name[15] = {0};

    memset( table_name, 0x00, sizeof(table_name) );
    strcpy( table_name, AppName );
    strcat( table_name, "Table" );
 
	/* Get host state and counters */
	ReadGenericTable( table_name, net_consec_tmouts_ctr, 
					  active_txns_ctr, current_state );
 
	switch (source)
	 {
		 case SOURCE_IS_FSVISA:
		 {
			 if (2 == atoi(current_state))
			 {
				 ncdci2_log_message( 1, 1, "Sending logon request to host:",
				 						   	"ncdci2_perform_host_logon_request",0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;

			 }
			 else
				 return (true);
			 break;
		 } /* end case SOURCE_IS_FSVISA */
 
		 case SOURCE_IS_STATE:
		 {
			 if (0 == atoi(current_state))
			 {
				 ncdci2_log_message( 1, 1, "Sending logon ECHO request to host:",
				 				 			"ncdci2_perform_host_logon_request",0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
				 memcpy( Auth_Tx.function_code,request_type,DINERS_DE24_SIZE);
				 ncdci2_send_request_to_host(p_auth_tx);
			 }
			 break;
		 } /* end case SOURCE_IS_STATE */
 
	   case SOURCE_IS_OPERATOR:
	   {
		  if (false == ncdci2_perform_op_net_mngmnt_request(request_type, p_auth_tx))
			 return(false);
		  else
			 return(true);
	   }
		 
		 case SOURCE_IS_HOST:
		 {

			 if (strcmp(DINERS_NMI_LOGON , request_type)==0)
			 {
				 if (1 == atoi(current_state))
				 {
					 ncdci2_log_message( 1, 1, "Received logon request from host:",
					 				 			"ncdci2_perform_host_logon_request",0 );
					 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					 memcpy( Auth_Tx.function_code,request_type,DINERS_DE24_SIZE);
					 strncpy(current_state,"0",1);
					 WriteGenericTable( table_name, net_consec_tmouts_ctr,
							 	 	 	 active_txns_ctr, current_state );
					 ncdci2_perform_op_net_mngmnt_response(request_type, p_auth_tx);
				 }
				 else
				 {
					 ncdci2_log_message( 1, 1, "Received logon request from host:",
					 					 		"ncdci2_perform_host_logon_request",0 );
					 ncdci2_log_message( 1, 1, "Already Online:",
					 					 		"ncdci2_perform_host_logon_request",0 );
				 	 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					 ncdci2_perform_op_net_mngmnt_response(request_type, p_auth_tx);
				 }
					 
			 }
			 else if (strcmp(DINERS_NMI_LOGOFF , request_type)==0)
			 {
				 	ncdci2_log_message( 1, 1, "Received logoff request from host:",
				 					 			"ncdci2_perform_host_logon_request",0 );
			  		p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					strncpy(current_state,"1",1);
					WriteGenericTable( table_name, net_consec_tmouts_ctr, 
                            active_txns_ctr, current_state );
				  	ncdci2_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }
		     //LOGON_NMI_KEY_EXCHANGE
		     else if (strcmp(DINERS_NMI_KEYTEST , request_type)==0)
			 {
		    	  if(AUTH_PIN_CHANGE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		    	  {
		    		  // No message , below function will send message to monitor.
		    	  }
		    	  else
		    	  {
		    		  ncdci2_log_message( 1, 1, "Received Key request reset:",
		    		  		    	 		"ncdci2_perform_host_logon_request",0 );
		    	  }
		    	  p_auth_tx->TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
		    	  ncdci2_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }
			 else if (strcmp(DINERS_NMI_KEYCHANGE , request_type)==0)
			 {
				  if(AUTH_PIN_CHANGE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
				  {
					  // No message , below function will send message to monitor.
				  }
				  else
				  {
					 ncdci2_log_message( 1, 1, "Received Key change request:",
												"ncdci2_perform_host_logon_request",0 );
				  }
					 p_auth_tx->TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
					 ncdci2_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }
			 else
			 {
				 if (0 == atoi(current_state))
				 {
					// ncdci2_log_message( 1, 1, "Received network request from host:",
					// 		    	 			"ncdci2_perform_host_logon_request",0 );
					 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					 ncdci2_perform_op_net_mngmnt_response(request_type, p_auth_tx);
				 }
				 else
					 return(false);
			 }
			 break;
		 } /* end case SOURCE_IS_HOST */
 
	} /* end switch statement */
 
	 return (true);
 
 };  /* ncdci2_perform_host_logon_request */


 
 
 /*****************************************************************************
 
   Function:	ncdci2_verify_host_app_status
 
   Description: This function will check the active transaction counter
				and the host state (both stored in shared memory) to 
				determine whether the host is "up"
   Author: 
	   Abhishek Verma
   Inputs:
	   p_auth_tx - used to determine whether this transaction is a reversal
   Outputs:
	   None
   Return values:
	   true  - indicates successful processing.
	   false - indicates no successful processing.
	Modified by:
 ******************************************************************************/
 INT  ncdci2_verify_host_app_status(pAUTH_TX p_auth_tx)
 {

	BYTE		net_consec_tmouts_ctr[4] = {0};
	BYTE		active_txns_ctr[4] = {0};
	BYTE		current_state[2] = {0};
	CHAR  		tempstr[14] = {0};
	CHAR     	table_name[15] = {0};
	INT   		tempint = 0;

    memset( table_name, 0x00, sizeof(table_name) );
    strcpy( table_name, AppName );
    strcat( table_name, "Table" );
 
	/* Get host state and counters */
	ReadGenericTable( table_name, net_consec_tmouts_ctr, 
					  active_txns_ctr, current_state );
 
	if (strcmp(current_state,"0") != 0) 
	{
		ncdci2_log_message( 1, 2, "Host app not in state 0, cannot process tran:",
							 	  "ncdci2_verify_host_app_status",1 );
	   return(false);
	}
 
	if (2 == atoi(current_state))  
	{  
		ncdci2_log_message( 1, 3, "Maximum consecutive timeouts reached:",
									"ncdci2_verify_host_app_status",1 );
	    strcpy(current_state,"2");
		return(false);
	}
	if (atoi(active_txns_ctr) >= atoi(ncf01.max_active_txns))
	{
		ncdci2_log_message( 1, 3, "Maximum consecutive timeouts reached:",
									"ncdci2_verify_host_app_status",0 );
	    return(false);
	}
 
	/* increment the number of active transactions for this host */
	 if (p_auth_tx->TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE)
	 {
		 tempint = atoi(active_txns_ctr) + 1;
		 memset(&tempstr, 0x00, sizeof(tempstr));
		 itoa(tempint,tempstr,10);
		 strcpy(active_txns_ctr, tempstr ); 
	 }
 
	 return(true);
 }; /* ncdci2_verify_host_app_status */

 
 /******************************************************************************
  *
  *  NAME:		   ncdci2_perform_op_net_mngmnt_request
  *
  *  DESCRIPTION:  This procedure prepares a log on or log off request.
  *
  *  INPUTS:	   request_type - Transaction type of request transaction
  * 			   p_auth_tx	- Transaction record
  *
  *  OUTPUTS:	   None
  *
  *  RTRN VALUE:   True if successful
  * 			   False if a problem occurred
  *
  *  AUTHOR:	   Abhishek Verma
  *
  *  MODIFICATIONS:
  ******************************************************************************/
 INT ncdci2_perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx )
 {
	CHAR	 gmt_time[15] = {0};
	CHAR	 diners_response_code[4] = {0};
	CHAR	 temp[17] = {0};
	AUTH_TX  auth_tx;

	memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	if (DINERS_NMI_LOGON == request_type)
	   strncpy( auth_tx.function_code, LOGON_NMI_LOGON, 3);  // 801 = sign on
	else if (DINERS_NMI_LOGOFF == request_type)
	strncpy( auth_tx.function_code, DINERS_NMI_LOGOFF, 3);  // 802 = sign off
	else 
	strncpy( auth_tx.function_code, LOGON_NMI_ECHO_TEST, 3);  // 801 = echo test
 
	strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
	strncpy(tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
	if( strcmp(ncf01.logon_bin[0].identifier,"SI") == 0)
	{
		si=1;
	}
	else
		si=0;
	auth_tx.TLF01_details.tx_key = AUTH_LOGON;
 
	/* Current Time for the transmission-to-host time  */
	ptetime_get_strtime(auth_tx.TLF01_details.host_start_time);
	
	/* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
	if ( 0 == strcmp( NULL_STR, auth_tx.TLF01_details.retrieval_ref_num))
	{
	   ncdci2_create_rrn( auth_tx.TLF01_details.retrieval_ref_num );
	}
 
	/* Use the time hhmmss for sys_trace_audit_num */
	memset(gmt_time, 0, sizeof(gmt_time));
	ptetime_get_strtime (gmt_time);
	ncdci2_Rj_with_lead_zeros(gmt_time, sizeof(gmt_time)-1);
	memcpy (auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 7, 3); 
	strncat(auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 11,3);
 
	strncpy( diners_response_code, "000", 3);
	strcpy ( auth_tx.forwarding_inst_id_cd, ncf01.forwarding_inst_id_code);
	 strcpy ( auth_tx.TLF01_details.processing_code, "000000");
	 strcpy ( auth_tx.host_processing_code, "000000");
	genutil_format_date (temp);
	memcpy (auth_tx.TLF01_details.date_yyyymmdd, temp, 8);	
	memcpy (auth_tx.TLF01_details.time_hhmmss, temp+8, 6);
 
	if (0 == strcmp(request_type, DINERS_NMI_LOGON))
	{
		ncdci2_log_message( 1, 1, "Sending DINERS_LOGON Request to host.",
								   "ncdci2_perform_op_net_mngmnt_request",0 );
	}
	else if (0 == strcmp(request_type, DINERS_NMI_LOGOFF))
	{
		ncdci2_log_message( 1, 1, "Sending DINERS_LOGOFF Request to host.",
								  "ncdci2_perform_op_net_mngmnt_request",0 );
	}

	memcpy( Auth_Tx.function_code,request_type,DINERS_DE24_SIZE);
	if (false == ncdci2_hhutil_send_net_mngmnt_request(&auth_tx, diners_response_code))
	   return(false);
	else
	   return(true);
 
 } /* ncdci2_perform_op_net_mngmnt_request */

 
 
 /*****************************************************************************
 
   Function:	ncdci2_perform_op_net_mngmnt_response
 
   Description: This function will 
   Author: 
	   Jimmy Hapitas
   Inputs:
	   nmi_code  - the nmi code
	   p_auth_tx - the auth_tx structure
   Outputs:
	   None
   Return values:
	   true  - indicates successful processing.
	   false - indicates no successful processing.
   Modified by:
	   Abhishek Verma	   
 ******************************************************************************/
 int ncdci2_perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx)
 {
 
	if(0 == strncmp(p_auth_tx->TLF01_details.message_type,DINERS_NETWORK_REQ_MESSAGE,DINERS_MESSAGE_TYPE_SIZE))
	{
		strncpy(p_auth_tx->TLF01_details.message_type,DINERS_NETWORK_RESP_MESSAGE,DINERS_MESSAGE_TYPE_SIZE);
	}
	strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id);
   
	ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);
	strcpy(p_auth_tx->TLF01_details.sys_trace_audit_num, p_auth_tx->TLF01_details.sys_trace_audit_num); 
	strcpy(p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx->TLF01_details.retrieval_ref_num);
 
	if (0 == strcmp(nmi_code, DINERS_NMI_LOGON))
	{
		ncdci2_log_message( 1, 1, "Sending DINERS_LOGON Response to host.",
								   "ncdci2_perform_op_net_mngmnt_response",0 );
		p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
	}
	else if (0 == strcmp(nmi_code, DINERS_NMI_LOGOFF))
	{
		ncdci2_log_message( 1, 1, "Sending DINERS_LOGOFF Response to host.",
								  "ncdci2_perform_op_net_mngmnt_response",0 );
		p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
	}
	else if (0 == strcmp(nmi_code,DINERS_NMI_KEYCHANGE))
	{
		ncdci2_log_message( 1, 1, "Sending Key Reset request Response to host.",
								   "ncdci2_perform_op_net_mngmnt_response",0 );
		p_auth_tx->TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
	}
	else if (0 == strcmp(nmi_code,DINERS_NMI_KEYTEST))
	{
		ncdci2_log_message( 1, 1, "Sending Key Test Response to host.",
								  "ncdci2_perform_op_net_mngmnt_response",0 );
		p_auth_tx->TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
	}
	else 
	{
		ncdci2_log_message( 1, 1, "Sending ECHO Response to host.",
								  "ncdci2_perform_op_net_mngmnt_response",0 );
	}
 
	memcpy( Auth_Tx.function_code,nmi_code,DINERS_DE24_SIZE);
 
	if (false == ncdci2_hhutil_send_net_mngmnt_response(p_auth_tx))
	   return(false);
	else
	   return(true);
 
 } /* ncdci2_perform_op_net_mngmnt_response */

 
 
 /******************************************************************************
  *
  *  NAME:		   ncdci2_set_originator_info
  *
  *  DESCRIPTION:  This function set originator communications information
  * 			   into a PTE message.	For responses, tx_key > 100, the info
  * 			   stored in auth_tx is used.  For requests, the tpdu_id is
  * 			   used.
  *
  *  INPUTS:	   p_auth_tx  - Transaction information
  *
  *  OUTPUTS:	   p_msg_out  - PTE message going to dialog manager
  *
  *  RTRN VALUE:   True if p_msg_out is not null, else false
  *
  *  AUTHOR:	   Abhishek Verma
  *
  ******************************************************************************/
 INT ncdci2_set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out )
 {
	INT  ret_val = true;
 
	if ( p_msg_out != NULL )
	{
	   if ( p_auth_tx->TLF01_details.tx_key > 100 )
	   {
		  /* This is a response. */
		  (void)ptemsg_set_orig_comms_info( p_msg_out,
								 p_auth_tx->TLF01_details.originator_info );
	   }
	   else
	   {
		  /* This is a request. */
		  (void)ptemsg_set_orig_comms_info( p_msg_out,
											p_auth_tx->TLF01_details.tpdu_id);
	   }
	}
	else
	{
	   ret_val = false;
	}
	return( ret_val );
 }

 /******************************************************************************
  *
  *  NAME:		   ncdci2_process_logon_Key_Reset_Transaction
  *
  *  DESCRIPTION:  This function store the PIK coming from Diners
  *
  *  INPUTS:	   None
  *
  *  OUTPUTS:	   None
  *
  *  RTRN VALUE:   True if p_msg_out is not null, else false
  *
  *  AUTHOR:	   Abhishek Verma
  *
  ******************************************************************************/
  void  ncdci2_process_logon_Key_Reset_Transaction()
 {
 	char temp_len[4] = {0};
	char temp_data[256] = {0};
	char buffer[256] = {0};
	char Feild_ident[3] = {0};
	int len=0;
	
 	if (strlen(Auth_Tx.TLF01_details.voice_auth_text) > 0)
 	{
 	
 		//Data format is <VLength(3)> <field identifier (2)> <VDATA>
 		memcpy(temp_len,Auth_Tx.TLF01_details.voice_auth_text, DINERS_DE48_LEN_SIZE);
		len = atoi(temp_len);
		memcpy(Feild_ident,Auth_Tx.TLF01_details.voice_auth_text+ DINERS_DE48_LEN_SIZE, 2);
		memcpy(temp_data,Auth_Tx.TLF01_details.voice_auth_text+ DINERS_DE48_LEN_SIZE + 2, len -2);
		
		bin_to_ascii (temp_data, buffer, strlen(temp_data));
		if(Feild_ident[0]=='N'&& Feild_ident[1]=='K')
		{ // We have got new key , we will store this in ZMK feild of ncf01 tables
			memcpy(Ncf01_I.zmk,buffer,strlen(buffer));
			ncdci2_update_ncf01(&Auth_Tx);
			Auth_Tx.TLF01_details.tx_key=AUTH_PIN_CHANGE_RESPONSE;
			ncdci2_hhutil_send_net_mngmnt_response(&Auth_Tx);
		}
 	}
 }

 BOOLEAN ncdci2_check_for_cadencieacq_transaction()
 {
	   if( (0 == strncmp(Auth_Tx.TLF01_details.message_type,"1100",4)	||
			0 == strncmp(Auth_Tx.TLF01_details.message_type,"1110",4)   ||
		    0 == strncmp(Auth_Tx.TLF01_details.message_type,"1420",4)	||
			0 == strncmp(Auth_Tx.TLF01_details.message_type,"1430",4))  	&&
		    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncdci2",6))
	   {
		   return true;
	   }
  	   return false;
  }

 BOOLEAN ncdci2_transfer_response_msg_to_dci()
  {

 		BOOLEAN   retval              = true;
 		pPTE_MSG  p_msg_out           = NULL_PTR;
 		CHAR	  dci_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncdciA" ;
 		CHAR      buffer[sizeof(AUTH_TX)+1]= {0};
 		BYTE      originator_info[33] = {0};
 		LONG      retCode             = 0L;
 		CHAR      strError[512]       = {0};

 	    memcpy(buffer , &Auth_Tx, sizeof(AUTH_TX) );

 	    p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
 	                                  ST1_NONE,
 	                                  ST2_NONE,
									  dci_host_que_name,
 	                                  application_que_name,
 	                                  (pBYTE) buffer,
 									  sizeof(AUTH_TX),
 	                                  0 );

 	    if ( p_msg_out == NULL_PTR )
 	    {
 	       sprintf( strError,
 	               "Insufficient Memory to build DCI request message. Size: %d",
 	                buflen );
 	       ncdci2_log_message( 3, 3, strError , "ncdci2_transfer_response_msg_to_dci2" , 0);
 	       retval = false;
 	    }
 	    else
 	    {
 	       ptemsg_set_orig_comms_info( p_msg_out, originator_info );
 	       retCode = pteipc_send( p_msg_out, dci_host_que_name );
 	       free (p_msg_out);

 	       if ( retCode < 0 )
 	       {
 	          pteipc_get_errormsg (retCode, strError);
 	          sprintf( strError,
 	                  "Error on pteipc_send to que %s: %s",
					  dci_host_que_name, strError );

 	          ncdci2_log_message( 3, 3, strError , "ncdci2_transfer_response_msg_to_dci2" , 0);
 	          retval = false;
 	       }
 	    }

 		return retval;
  }
