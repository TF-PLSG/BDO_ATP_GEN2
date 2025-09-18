/******************************************************************************
 
 	ncdci_tran.c
 
    Copyright (c) 2005, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  ncdci Acquiring Host Handler Driver
 
 	This module is the host handler for the ncdci host.  It
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
#include "txutils.h"
#include "txtrace.h"
#include "ntutils.h"
#include "diners.h"
#include "ncdci_prototypes.h"

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
extern AUTH_TX 				Auth_Tx;
extern NCF01   				Ncf01_I;
extern NCF01   				Ncf01_I_key_exchange;
extern NCF01_KEYBLOCK 		Ncf01_Keyblock;
extern NCF01_KEYBLOCK  		Ncf01_I_key_exchange_Keyblock;
extern NCF01   				Ncf01_A;
extern NCF30   				Ncf30;
extern CRF01   				Crf01;


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

extern CHAR  NMI_Code[4];

/*Flags*/
extern INT Txn_Is_A_Reject;

DCI3DS2 diners_3ds2;

CHAR   security_que_name [] = "racalA" ;

INT     si;

extern CHAR static_Rtdcwk_Key[33];
extern CHAR static_Rtdcwk_Key_Keyblock[33];
extern CHAR MAC[17];
extern INT keyblock_enable_flag_for_DCI;

/***************************************************************************
 *
 *  NAME:         ncdci_process_host_msg
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
void ncdci_process_host_msg( pPTE_MSG p_msg_in )
{
	CHAR    err_buf[256] = {0};
	INT     ret_val = true;
	BYTE    tran_type = 0;
	pPTE_MSG_DATA     p_msg_data = NULL_PTR;

	/* Cycle the echo timer because traffic came over the line. */
	if ( false == ncdci_reset_echo_timer(err_buf) )
	{
		ncdci_log_message(2, 2, err_buf, "ncdci_process_host_msg",0 );
	}

	/* get the p_msg data from p_msg */
	p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);

	/* Copy host message into a character string. */
	ncdci_copy_to_req_resp_buffer( p_msg_in );
	if(buflen >= DINERS_COMM_BUF_SZE)
	{
		sprintf(err_buf,"Invalid length or Big msg recvd from Host:  %04d", buflen);
		ncdci_log_message( 1, 3, err_buf, "ncdci_process_host_msg", 0 );
		return;
	}
	/* Log transaction to trace file. */
	ncdci_write_to_txn_file();

	/* Parse message from host. */
	memset( &Auth_Tx, 0x00, Auth_Tx_Size );

	/* Gather originator information */
	(void)ncdci_get_originator_info( p_msg_in, Auth_Tx.TLF01_details.originator_queue,
									 Auth_Tx.TLF01_details.originator_info );

	if ( !ncdci_parse_host_msg( err_buf ) )
	{
		ret_val = false;
	}

	/* Get message type. */
	else if ( !ncdci_validate_msgtype( Auth_Tx.TLF01_details.message_type, err_buf ) )
	{
		ret_val = false;
	}

	else
	{
		if(buflen < DINERS_COMM_BUF_SZE && buflen>0)
		{
			sprintf(Auth_Tx.req_data_len,"%04d", buflen);
			memcpy(Auth_Tx.req_buffer,ReqRsp_Buffer,buflen);
		}

		/* Determine if switched in request, or switched out response txn.*/

		tran_type = ncdci_get_tran_type_2(Auth_Tx.TLF01_details.tx_key);
		Auth_Tx.TLF01_details.tx_key = tran_type;

		TranFlow = ncdci_determine_txn_flow( Auth_Tx.TLF01_details.message_type, DINERS_HOST );

		/* Populate Auth_tx structure with data from host. */
		if ( !ncdci_populate_auth_tx(err_buf) )
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
			}

			tran_type = ncdci_match_request_to_response( tran_type );
			Auth_Tx.TLF01_details.tx_key = tran_type;

			strcpy( Auth_Tx.TLF01_details.response_text,
					"Received ncdci Reject Msg on the Request" );
		}
		if ( TranFlow == DINERS_SWITCHED_IN )
		{
			ncdci_process_request_from_host();
		}
		else
		{
			ncdci_process_response_from_host();
		}
	}

	/* Log any errors. */
	if ( ret_val == false )
	{
			ncdci_log_message( 3, 3, err_buf, "ncdci_process_host_msg", 1 );
	}
	return;
}

/**************************************************************************
 *
 *  NAME:         ncdci_process_request_from_host
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
INT ncdci_process_request_from_host()
{
	INT   ret_val = true;

	///////////////////////////////////////////////////////////////////////////////

	pPTE_MSG_DATA     p_msg_data = NULL_PTR;
	pPTE_MSG          p_msg_chain = NULL_PTR;
	pPTE_MSG          p_msg_timer_out = NULL_PTR;
	CHAR              Buffer[256] = {0};
	CHAR              ErrorMsg[100] = {0};
	BYTE              tran_type = 0;
	CHAR              nmi_code[4] = {0};
	BYTE              buffer[sizeof(AUTH_TX)] = {0};
	CHAR			  lookup_key[22] = {0};
	CHAR              temp_eff_date[7] = {0};
	INT               idate = 0;
	INT               pan_len = 0;
	CHAR              tid[9] = {0};

	if(AUTH_INVALID == Auth_Tx.TLF01_details.tx_key)
	{

		strcpy( Buffer, "Unsupported transaction from ncdci. Message type = ");
		strcat( Buffer,  Auth_Tx.TLF01_details.message_type );
		strcat( Buffer, ", Processing code = " );
		strcat( Buffer,  Auth_Tx.TLF01_details.processing_code );
		ncdci_log_message( 3, 3, Buffer, "ncdci_process_request_from_host",1 );
		return(true);
	}

	/* Update transaction statistics. */
	update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

	/*Network Request message type*/
	if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					DINERS_NETWORK_REQ_MESSAGE,
					DINERS_MESSAGE_TYPE_SIZE))
	{
		strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
		if(strncmp(NMI_Code,DINERS_NMI_SYSTEM_ECHO, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci_perform_host_logon_request(DINERS_NMI_SYSTEM_ECHO,SOURCE_IS_HOST,&Auth_Tx);
			ncdci_send_network_status_to_monitor ();
		}
		else if(strncmp(NMI_Code,DINERS_NMI_LOGON, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci_perform_host_logon_request(DINERS_NMI_LOGON,SOURCE_IS_HOST,&Auth_Tx);
			ncdci_send_network_status_to_monitor ();
		}
		else if (strncmp(NMI_Code,DINERS_NMI_LOGOFF, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci_perform_host_logon_request(DINERS_NMI_LOGOFF,SOURCE_IS_HOST,&Auth_Tx);
			ncdci_send_network_status_to_monitor ();
		}
		else if(strncmp(NMI_Code,DINERS_NMI_KEYTEST, DINERS_NETWORK_FUNCTION_CODE_LEN)==0)
		{
			ncdci_perform_host_logon_request(DINERS_NMI_KEYTEST,SOURCE_IS_HOST,&Auth_Tx);

		}

		return(true);

	}

	/*Network Response message type*/
	else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
						 DINERS_NETWORK_RESP_MESSAGE,
						 DINERS_MESSAGE_TYPE_SIZE))
	{
		strcpy( Auth_Tx.TLF01_details.handler_queue,application_que_name);
		ncdci_process_logon_response(NMI_Code,&Auth_Tx);
		return(true);
	}
	/*Negative file 1324 Response message type*/
	else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
						 DINERS_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE,
						 DINERS_MESSAGE_TYPE_SIZE))
	{
		ncdci_process_file_update_request_1324();
		return true;
	}

	(void)ncdci_init_txn( &Auth_Tx );
	strncpy(Auth_Tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
	strncpy(tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

	/* Send switched in request to the authorizer. */
	if((Auth_Tx.TLF01_details.tx_key >= AUTH_SALE) &&
	   (Auth_Tx.TLF01_details.tx_key <= AUTH_INVALID))
	{
		strcpy(Auth_Tx.TLF01_details.issuer_id, AppName);
		strcpy(Auth_Tx.TLF01_details.source_key, ncf01.cwk);
		strcpy(Auth_Tx.TLF01_details.source_pin_format, ncf01.pin_type);
		ncdci_populate_iso_pos_entry_mode();
		if( AUTH_CASH_ADVANCE == Auth_Tx.TLF01_details.tx_key ||
		    ncdci_pin_block_exists())
		{
			/* Its PIN transactionn, we have to store source key */

			memcpy( Auth_Tx.TLF01_details.source_key,
					Ncf01_I.retired_cwk,DINERS_KEY_LEN);

		}
		/* Are we blocking this transaction? */
		if ( true == ncdci_txn_is_blocked(&Auth_Tx,Buffer) )
		{
		/* Txn is blocked.  Log message and respond to host. */
			ncdci_log_message( 1, 1, Buffer, "ncdci_process_request_from_host",1 );
			ncdci_perform_error_processing( &Auth_Tx );
			strcpy(Auth_Tx.TLF01_details.response_code,"57");
			ncdci_match_request_to_response(Auth_Tx.TLF01_details.tx_key);
			ncdci_incoming_other_host_response(&Auth_Tx);
			return(false);
		}
		else if(false == ncdci_hhutil_process_host_request(&Auth_Tx))
		{
			ncdci_perform_error_processing( &Auth_Tx );
			strcpy(Auth_Tx.TLF01_details.response_code,"91");
			Auth_Tx.TLF01_details.tx_key =
			ncdci_match_request_to_response(Auth_Tx.TLF01_details.tx_key);
			ncdci_incoming_other_host_response(&Auth_Tx);
			return(false);
		}
		return(true);
	}

	if ( ret_val == false )
	{
		/* Unable to continue processing transaction.
		 * Need to send a declined response to the host.
		 */
		strcpy( Auth_Tx.TLF01_details.response_code, DINERS_INVALID_TXN );
		ncdci_send_response_to_host( DINERS_SEND_UNCONDITIONALLY );
	}
	return ret_val;
}



/****************************************************************************
 *
 *  NAME:         ncdci_process_request_for_host
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
void ncdci_process_request_for_host( pPTE_MSG p_msg_in )
{
	pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
	pBYTE           p_data = NULL_PTR;;
	LONG            data_len = 0;
	LONG            auth_len = 0;
	CHAR			   err_buf[100] = {0};
	CHAR			   Buffer[200] = {0};
	CHAR            temp_source_pin_format[2] = "1";
	CHAR            temp_dest_pin_format[2] = "1";


	/* Copy transaction record into auth_tx structure. */
	memset( &Auth_Tx, 0x00, Auth_Tx_Size );
	p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
	auth_len   = data_len;
	if ( Auth_Tx_Size < data_len )
	{
	  auth_len = Auth_Tx_Size;
	}
	p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
	memcpy( &Auth_Tx, p_data, auth_len );

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

	if( true == ncdci_is_POS_Acq_transaction ())
	{
		if ( (!ncdci_is_reversal()) && ncdci_pin_block_exists() && true == ncdci_verify_host_status(err_buf))
		{
			strcpy(Auth_Tx.TLF01_details.source_key, Ncf01_A.cwk ) ;// Storing Source key
			strcpy(Auth_Tx.TLF01_details.dest_key, Ncf01_I.retired_cwk ) ;// Storing Destination key
			memcpy(Auth_Tx.TLF01_details.source_pin_format, temp_source_pin_format,1)  ;// Source pin format
			memcpy(Auth_Tx.TLF01_details.dest_pin_format, temp_dest_pin_format,1) ;// Destination pin format
			if (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
			{
				(void)ncdci_translate_pin_keyblock();
			}
			else
			{
				(void)ncdci_translate_pin();
			}
		}
		else
		{
			(void)ncdci_process_request_for_host_after_security();
		}
	}

	else
	{
		(void)ncdci_process_request_for_host_after_security();
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_request_for_host_after_security
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
void ncdci_process_request_for_host_after_security()
{
   BYTE   tx_key=0;
   INT    ret_val = true;
   CHAR   err_buf[100] = {0};
   CHAR   Buff[100] = {0};
   DWORD  txn_amt = 0;
   DWORD  converted_amt = 0;

   /* Process based on transaction type. */
   tx_key = Auth_Tx.TLF01_details.tx_key;
   switch ( tx_key )
   {
      case AUTH_REVERSAL:
      case AUTH_REVERSAL_ADVICE:

         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff,"Reversal going to ncdci: " );
         ncdci_log_message( 3, 2, Buff, "ncdci_process_request_for_host_after_security", 1 );

      case AUTH_VOID_PRE_AUTHORIZATION:
          /* Display Pre-auth canceln message on Monitor.
           * This message is used to isolate problems to selected bins
           * or to certain merchants or to networks.
           */
    	  if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
    	  {
          memset( Buff, 0x00, sizeof(Buff) );
          strcpy( Buff,"Void preauth going to ncdci: " );
              ncdci_log_message( 3, 2, Buff,
									  "ncdci_process_request_for_host_after_security", 1 );
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
               Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
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
      case AUTH_REFUND:
      case AUTH_VOID_CASH_ADVANCE:
      case AUTH_VOID_REFUND:
      case AUTH_VOID_SALE:
	  case AUTH_PRE_AUTHORIZATION:
	  case AUTH_QUASI_CASH:
	  case AUTH_CARD_VERIFICATION:
	        break;

	  case AUTH_OFFLINE_SALE:
		  ret_val = ncdci_perform_standin_processing(DINERS_ISO_APPROVED,
												 	  GS_TXN_COMPLETED_OFF,
													  err_buf );
		  if ( false == ret_val )
		  {
			  ncdci_log_message( 2,2,err_buf,
								"ncdci_perform_standin_processing",1);
		  }
		  /* Make sure transaction processing does not continue. */
		  ret_val = false;
	break;

      default:

         /* Unknown transaction, reject it. Send response to authorizer. */
         memset( err_buf, 0x00, sizeof(err_buf) );
         sprintf( err_buf,
                 "Invalid txn. Cannot process. TX_KEY = %d",
                  tx_key );
         ncdci_log_message( 1, 2, err_buf,
                     "ncdci_process_request_for_host_after_security",1 );
		 /*TF Praneeth revert back tx_key  while stip transaction */
		 if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
		 {
			 Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.voice_auth_text[0];
			 memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
		 }

         ret_val = ncdci_perform_standin_processing( DINERS_INVALID_TXN,
                                               GS_TXN_TRANSACTION_NOT_ALLOWED,
                                               err_buf );
         if ( false == ret_val )
        	 ncdci_log_message(2,2,err_buf,
        			 	 	 	 	 "ncdci_process_request_for_host_after_security", 1);

         /* Make sure transaction processing does not continue. */
         ret_val = false;
         break;
   }

   /* Continue processing the request for the host. */
   if ( false != ret_val )
      (void)ncdci_process_request_for_host_after_currency();

   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_request_for_host_after_currency
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
void ncdci_process_request_for_host_after_currency()
{
   INT  ret_val = true;
   CHAR err_buf[100] = {0};

   /* Check the status of the host.  Are we online? */
   ret_val = ncdci_verify_host_status( err_buf );

   if ( false == ret_val )
   {
	   ncdci_log_message( 1, 2, err_buf,
			   	   	   	   	   "ncdci_process_request_for_host_after_currency", 0 );
	  /*TF Praneeth revert back tx_key  while stip transaction */
	  if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
		  Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.voice_auth_text[0];
		  memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
	  }

      ret_val = ncdci_perform_standin_processing( "01",
                                            GS_TXN_DROPPED,
                                            err_buf );
      if ( false == ret_val )
    	  ncdci_log_message(2,2,err_buf,
    			  	  "ncdci_process_request_for_host_after_currency", 1);
   }
   else
   {
	  /*TF Praneeth clearing buffer variable used to store tx_key  while stip transaction */
	  if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
	    memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
	  }
      (void)ncdci_send_request_to_host( DINERS_TIMER_FINANCIAL );
   }

   return;
}

/**************************************************************************
 *
 *  NAME:         ncdci_process_response_from_host
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
void ncdci_process_response_from_host()
{
	CHAR  err_buf[100] = {0};
	CHAR  lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	BYTE  tran_type = 0;

	/* Get host finish time, for statistic purposes. */
	ptetime_get_strtime( Auth_Tx.TLF01_details.host_finish_time );

	sprintf(err_buf, "RECEIVED DIALOG MESSAGE: %s response from Host", Auth_Tx.TLF01_details.message_type);
	ncdci_log_message(2, 1, err_buf, "ncdci_process_response_from_host", 0 );

	/* Decrement number of txns that are outstanding with the host. */
	if ( false == ncdci_decrement_active_count( err_buf ) )
	{
		ncdci_log_message(2, 1, err_buf, "ncdci_process_response_from_host", 0 );
	}

	/* Reset number of consecutive timeouts to zero. */
	if ( false == ncdci_reset_timeout_count( err_buf ) )
	{
		ncdci_log_message(2, 1, err_buf, "ncdci_process_response_from_host", 0 );
	}

	/* Get tx_key, making sure it is valid. */
	tran_type = ncdci_get_tran_type_2(Auth_Tx.TLF01_details.tx_key);
	Auth_Tx.TLF01_details.tx_key = tran_type;
	if(Auth_Tx.TLF01_details.tx_key != AUTH_INVALID)
	{
		/* Handle Network Management transactions separately. */
		if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
		{
			update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, MGT );
			ncdci_process_net_management_from_host();
		}
		else
		{
			/* Update transaction statistics. */
			update_txn_stats( &Txn_Counts, &Auth_Tx, RECEIVED );

			/* Need to clear the timer; so create the lookup key. */
			ncdci_create_request_timer_key( lookup_key );

			/* Now clear the request timer;
			* exit code while waiting for timer's response.
			* Timer's response will contain original auth_tx.
			*/
			if ( false == ncdci_clear_request_timer(lookup_key) )
			{
				sprintf( err_buf,
						 "Unable to clear request on response from host. Msg type: %s, DE7: %s",
						 Auth_Tx.TLF01_details.message_type,
						 Auth_Tx.TLF01_details.transmission_timestamp );
				ncdci_log_message(2, 2, err_buf, "ncdci_process_response_from_host",1 );
			}
		}
	}
	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_clear_timer_reply
 *
 *  DESCRIPTION:  Process a reply from the timer.  This function continues
 *                the processing started by 'process_response_from_host'.
 *                It will complete the processing of a response that was
 *                received from a host.
 *
 *  INPUTS:       p_msg_in - This is the message from the timer.  It contains
 *                           two auth_tx buffers: the original auth_tx from
 *                           the acquiring application (POS handler, ...) and an
 *                           auth_tx containing the host response information.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_process_clear_timer_reply( pPTE_MSG p_msg_in )
{
   pBYTE          p_data            = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
   pPTE_MSG       p_msg_out         = NULL_PTR;
   pPTE_MSG       p_echo_msg        = NULL_PTR;
   LONG           timer_reply_size  = 0;
   LONG           data_len = 0;
   CHAR           app_data1[32] = {0};
   CHAR           app_data2[32] = {0};
   CHAR           lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
   CHAR           echo_key[DINERS_LOOKUP_KEY_SIZE]  = {0};
   CHAR           echo_req_key[DINERS_LOOKUP_KEY_SIZE]  = {0};
   CHAR           err_buf[100] = {0};
   AUTH_TX        host_auth_tx;
   INT			  len = 0;
   
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
   ncdci_create_echo_timer_key( echo_key );
   ncdci_create_echo_req_timer_key( echo_req_key );
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
         ncdci_log_message( 2, 1, err_buf, "ncdci_process_clear_timer_reply", 1 );
      }

      /* Verify that the data size returned matches auth_tx size */
      else if ( timer_reply_size != Auth_Tx_Size )
      {
         sprintf( err_buf,
                 "Did not receive auth_tx data with timer reply. Key = %s",
                  lookup_key );
         ncdci_log_message( 2, 2, err_buf, "ncdci_process_clear_timer_reply", 1 );
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
            ncdci_log_message( 2, 2, err_buf, "ncdci_process_clear_timer_reply" , 0);
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

            (void)ncdci_insert_db_record( TLF01_DATA );
         }
         else
         {
            /* Get auth_tx record that was built from host response data. */
            memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
            memcpy( &host_auth_tx, p_data, Auth_Tx_Size );

            /* Move response info from host into Auth_Tx. */
            (void)ncdci_copy_resp_into_request( &host_auth_tx );

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
            (void)ncdci_set_tx_key_to_response();

            /* Update the general status. */
            Auth_Tx.TLF01_details.general_status = GS_TXN_COMPLETED;

            if ( Auth_Tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
               (void)ncdci_process_file_update_response();

            if ( true == ncdci_check_for_cadencieacq_transaction())
            {
            	/* send repsonse directly to ncdci2
            	 * */
         	   memcpy(Auth_Tx.req_data_len,host_auth_tx.req_data_len,sizeof(host_auth_tx.req_data_len));
         	   memcpy(Auth_Tx.req_buffer,host_auth_tx.req_buffer,sizeof(host_auth_tx.req_buffer));
         	   ncdci_transfer_response_msg_to_dci2();
         	   return;
            }
            /* Forward the response to the authorizer. */
            else if ( false == ncdci_send_msg_to_authorizer(MT_AUTH_RESPONSE) )
            {
               sprintf( err_buf,
                    "Unable to complete processing of response msg. MsgType: %s, processing code: %s",
                     Auth_Tx.TLF01_details.message_type,
                     Auth_Tx.TLF01_details.processing_code);
               ncdci_log_message( 1, 2, err_buf, "ncdci_process_clear_timer_reply", 1 );
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_copy_resp_into_request
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
void ncdci_copy_resp_into_request( pAUTH_TX host_auth_tx )
{
	int buflen_loc = 0;
   if ( AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key )
   {
      strcpy( Auth_Tx.TLF01_details.total_amount,
              host_auth_tx->TLF01_details.total_amount);
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
   }else
   {
	  if(Auth_Tx.TLF01_details.auth_number[0] != 0x00  &&
		 Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE &&
		 Auth_Tx.TLF01_details.tx_key != AUTH_VOID_PRE_AUTHORIZATION)
	  memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
   }

   strcpy( Auth_Tx.TLF01_details.response_code,
           host_auth_tx->TLF01_details.response_code );

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

      if(0 == strcmp(Auth_Tx.TLF01_details.message_type,"1100") &&
    	true== ncdci_check_for_cadencieacq_transaction())
      {
          strcpy( Auth_Tx.TLF01_details.message_type,
             host_auth_tx->TLF01_details.message_type );

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
		  sizeof(Auth_Tx.TLF01_details.product_codes[19].code));

   /* ncdci Response Code - used for Online Updates (1304/1314) */
   strcpy( Auth_Tx.accptor_bus_code, host_auth_tx->accptor_bus_code );
   /* TF Phani - For VOICE CA- Trasnctions should go as AUTH-
	During request we have changed to CA and changes back to CA_AUTH		
   */
    if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y'  && Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;

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

	// Copy EMV fields
	memcpy(Auth_Tx.EMV_details.future_field1,host_auth_tx->EMV_details.future_field1,
			sizeof(Auth_Tx.EMV_details.future_field1));

	memcpy(Auth_Tx.EMV_details.future_field2,host_auth_tx->EMV_details.future_field2,
			sizeof(Auth_Tx.EMV_details.future_field2));

	if(strlen(host_auth_tx->TLF01_details.resp_source)>0)
	{
		memcpy(Auth_Tx.TLF01_details.resp_source,host_auth_tx->TLF01_details.resp_source,
			(sizeof(Auth_Tx.TLF01_details.resp_source)-1));
	}
    else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
						  	DINERS_FILE_TRN_MESSAGE_TYPE,
							DINERS_FILE_TRN_MESSAGE_TYPE_LEN))
  {
    	memcpy(Auth_Tx.req_data_len,host_auth_tx->req_data_len,
    		sizeof(Auth_Tx.req_data_len));
    	buflen_loc = atoi(Auth_Tx.req_data_len);
    	memcpy(Auth_Tx.req_buffer, host_auth_tx->req_buffer,
    			buflen_loc);
  }

	//Copy action code.
    memcpy(Auth_Tx.TLF01_details.product_codes[12].code,
		      host_auth_tx->TLF01_details.product_codes[12].code,
			  sizeof(host_auth_tx->TLF01_details.product_codes[12].code));

   return;
}
  
/******************************************************************************
 *
 *  NAME:         ncdci_process_1304_from_host
 *
 *  DESCRIPTION:  This procedure prepare the buffer to send the file update 
 *                (1304) request to ncdci Host.
 *
 *  INPUTS:       host_auth_tx - Transaction request record from host
 *
 *  OUTPUTS:      Auth_Tx - (Global) Txn record is updated with request info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_process_1304_from_host()
{
	BYTE     buffer[sizeof(AUTH_TX) + sizeof(AMEX_ONLINE_UPDATES)] = {0};
	AMEX_ONLINE_UPDATES FileUpdate;
	INT         ret_val;
    CHAR        err_msg[100] =  {0};
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

	/*Need to modified based on ncdci*/
/*
	ret_val = ncdci_build_and_send_request_message( MT_INCOMING_GUI, 
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
   	   ncdci_log_message( 3, 1, err_msg, "ncdci_process_1304_from_host" );
     }*/

} /* process_1304_from_host */


/******************************************************************************
*
*  NAME:         ncdci_delete_orig_ncf30
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
INT ncdci_delete_orig_ncf30( NCF30 ncf30, pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)] ={0};
   LONG            ret_code = false;
   CHAR            Buff[256] = {0};
   CHAR            err_mesg[200] = {0};
 
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
	   ncdci_log_message( 1, 1, "Insufficient Memory to build NCF30 message",
			   	   	   	   	   "ncdci_delete_orig_ncf30", 0 );
       return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s", netds_que_name, err_mesg );
      ncdci_log_message( 1, 1,Buff , "ncdci_delete_orig_ncf30", 1 );
      return( false );
   }
   return(true);
 }

 
 /*****************************************************************************
 
   Function:	ncdci_process_logon_response
 
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
 BOOLEAN ncdci_process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx)
 {
	pPTE_MSG  p_msg_host_app = NULL_PTR;
	CHAR	  ErrorMsg[200] = {0};
	BYTE	  net_consec_tmouts_ctr[4] = {0};
	BYTE	  active_txns_ctr[4] = {0};
	BYTE	  current_state[2] = {0};
	CHAR	  unique_tran_str_id[17] = {0};
	INT 	  update_host_status = false;
 
	INT 	   insert_flag = 0;
	BYTE	   buffer1[sizeof(AUTH_TX)+sizeof(TLF01)] = {0};
	pPTE_MSG   p_msg_ins = NULL_PTR;
	LONG	   ret_code = false;
	INT 	   i = 0;
	CHAR     table_name[15] = {0};

    memset( table_name, 0x00, sizeof(table_name) );
    strcpy( table_name, AppName );
    strcat( table_name, "Table" );
 
	/* Get host state and counters */
	ReadGenericTable( table_name, net_consec_tmouts_ctr, 
					  active_txns_ctr, current_state );
	strcpy(net_consec_tmouts_ctr, "0");
 
	if (strcmp(DINERS_NMI_LOGON ,p_auth_tx->function_code) == 0)
	{
	   if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
	   {
		  strcpy(current_state, "0");
		  update_host_status = true;
		  ncdci_log_message( 1, 1, "LOGON Request successful.",
				  	  	  	  	  "ncdci_process_logon_response", 0 );
	   }
	   else
	   {
		   ncdci_log_message( 1, 1, "LOGON Request DENIED.",
				   	   	   	   "ncdci_process_logon_response", 0 );
	   }
	}
	
	else if (strcmp(DINERS_NMI_LOGOFF,p_auth_tx->function_code) == 0)
	{
	   if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
	   {
		  strcpy(current_state, "1");
		  update_host_status = true;
		  ncdci_log_message( 1, 1, "LOGOFF Request successful.",
				  	  	  	  	  "ncdci_process_logon_response", 0 );
	   }
	   else
	   {
		   ncdci_log_message( 1, 1, "LOGOFF Request DENIED.",
				   	   	   	   	   "ncdci_process_logon_response", 0 );
	   }
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
		ncdci_log_message( 1, 3, "Insufficient Memory to build insert TLF01 message",
								"ncdci_process_logon_response", 1 );
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
	   (void)ncdci_send_network_status_to_monitor();
	}
	return(true);
 }
 
 /*****************************************************************************
 
   Function:	ncdci_process_incoming_message
 
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
 ******************************************************************************/
 INT ncdci_process_incoming_message(pPTE_MSG p_msg)
 {
	pPTE_MSG_DATA	p_msg_data = NULL_PTR;
	LONG			data_len = 0;
	LONG			auth_len = 0;
	pBYTE			p_data_data = NULL_PTR;
	AUTH_TX 		auth_tx;
	CHAR dci_response_code[4] = {0};

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

	/* TF PHANI..If it is ATM_CONFIRM it is reversal response */
	if(Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE)
	{
		Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
	}

	strncpy(Auth_Tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

	if((Auth_Tx.TLF01_details.tx_key < AUTH_SALE) ||
	   (Auth_Tx.TLF01_details.tx_key > AUTH_INVALID))  /* a response */
	{
		Auth_Tx.TLF01_details.tx_key =	ncdci_match_request_to_response(Auth_Tx.TLF01_details.tx_key);

		/* Restore the conversion rate. */
		if ( Auth_Tx.settlement_conv_rate[0] != 0x00 )
		{
			strcpy( Auth_Tx.TLF01_details.conversion_rate,
					Auth_Tx.settlement_conv_rate );
		}

		/*Let us store the original amount in TLF01_details.total_amount field if we have
		* change the total_amount during to currency conversion
		*/
		if(Auth_Tx.TLF01_details.tx_key > 100 )
		{
			if ( ((strlen(Auth_Tx.reconciliation_cur_code) > 0 ) &&
			(ncdci_isnum(Auth_Tx.reconciliation_cur_code))))
			{
				if(strlen(Auth_Tx.TLF01_details.orig_amount))
				{
					strncpy(Auth_Tx.TLF01_details.product_codes[9].amount,
							Auth_Tx.TLF01_details.total_amount,
							DINERS_DE4_SIZE);
					memcpy( Auth_Tx.TLF01_details.total_amount,
							Auth_Tx.TLF01_details.orig_amount,
							DINERS_DE4_SIZE);
				}
				else
				{
					memcpy( Auth_Tx.TLF01_details.total_amount,
							Auth_Tx.TLF01_details.total_amount,
							DINERS_DE4_SIZE );
				}

				if( 0 == strncmp(Auth_Tx.settlement_curr_code, DINERS_PESO_CURRENCY_CODE,3) ||
					0 == strncmp(Auth_Tx.ch_billing_curr_code, DINERS_PESO_CURRENCY_CODE,3))
				{
					strcpy( Auth_Tx.TLF01_details.host_start_time, "5.000");
				}
				else if( 0 == strncmp(Auth_Tx.settlement_curr_code, DINERS_USA_CURRENCY_CODE,3) ||
						 0 == strncmp(Auth_Tx.ch_billing_curr_code, DINERS_USA_CURRENCY_CODE,3))
				{
					strcpy( Auth_Tx.TLF01_details.host_start_time, "7.000");
				}

				memcpy( Auth_Tx.TLF01_details.currency_code,Auth_Tx.reconciliation_cur_code,
						DINERS_DE49_SIZE);
			}
		}
		// Only for STIP transaction we shall copy the Action code here
	    if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))
	    {
	    	//DCI 3DS2
			if ( true == ncdci_is_issuing_transaction() &&
				  0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+0, DINERS_DE122_AUTH_DATA_TYPE_PRODUCTBUY_VALUE, 1) &&
			      0 != strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, "00", 2))
			{
				if (Auth_Tx.TLF01_details.product_codes[12].code[0] == '\0')
				{
					match_ISO_to_Diners_response_code(dci_response_code);
					memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
							dci_response_code,
							sizeof(dci_response_code));
					//Store for tranlookup
					if (0 == strncmp(Auth_Tx.TLF01_details.profile, "5", 1))
						memcpy(Auth_Tx.TLF01_details.operator_id, diners_3ds2.self_validation_pass_cavv_rc, 2);
				}
			}
			else
			{
				match_ISO_to_Diners_response_code(dci_response_code);
				if (0 == strlen(Auth_Tx.TLF01_details.product_codes[12].code))
				{
					memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
							dci_response_code,
							sizeof(dci_response_code));
				}
			}

			//OCT MAND 2021 BR
			if (0== strncmp(Auth_Tx.function_code,"108",2) &&
				0==strncmp(Auth_Tx.TLF01_details.processing_code,"18",2)) //card acount verification
				{
					//Further process based on the presense of Cvv2 data( field 40)
					if(0==strlen(Auth_Tx.TLF01_details.cvc_data))
					{
						if(0==strncmp(Auth_Tx.CCF02P_details.status,"02",2))
						{
							memcpy(	Auth_Tx.TLF01_details.product_codes[12].code,
									DINERS_CARD_ACCT_VERIFICATION_SUCCESS,
									DINERS_DE39_SIZE );
						}
						else
						{
							memcpy(	Auth_Tx.TLF01_details.product_codes[12].code,
									DINERS_CARD_ACCT_VERIFICATION_FAILURE,
									DINERS_DE39_SIZE );
						}

					}
					else
					{
						//Cvv2 validation unsuccessful.
						if(0==strncmp(Auth_Tx.TLF01_details.security_response_code, "02",2))
						{
							memcpy(	Auth_Tx.TLF01_details.product_codes[12].code,
												DINERS_CARD_ACCT_VERIFICATION_FAILURE,
												DINERS_DE39_SIZE );

						}
						//cvv2 validation successful.
						if(0==strncmp(Auth_Tx.TLF01_details.security_response_code, "03",2))
						{
							if(0==strncmp(Auth_Tx.CCF02P_details.status,"02",2))
							{
								memcpy(	Auth_Tx.TLF01_details.product_codes[12].code,
										DINERS_CARD_ACCT_VERIFICATION_SUCCESS,
										DINERS_DE39_SIZE );
							}
							else
							{
								memcpy(	Auth_Tx.TLF01_details.product_codes[12].code,
										DINERS_CARD_ACCT_VERIFICATION_FAILURE,
										DINERS_DE39_SIZE );

							}

						}

						if(Auth_Tx.CCF02P_details.status[0] == 'I' )
						{
							memcpy(	Auth_Tx.TLF01_details.product_codes[12].code,
																DINERS_CARD_ACCT_VERIFICATION_FAILURE,
																DINERS_DE39_SIZE );

						}


					}

			    }
	    }
	    else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
						   DINERS_ADVICE_REQ_MESSAGE,
						   DINERS_MESSAGE_TYPE_SIZE))
		{
			if(strlen(Auth_Tx.TLF01_details.orig_auth_number) > 0)
			{
				memcpy( Auth_Tx.TLF01_details.auth_number,
						Auth_Tx.TLF01_details.orig_auth_number,
						DINERS_DE38_SIZE );
			}
			if(strlen( Auth_Tx.TLF01_details.product_codes[12].code)> 0)
			{
				memcpy(	dci_response_code,
						Auth_Tx.TLF01_details.product_codes[12].code,
						DINERS_DE39_SIZE );
				match_DCI_to_ISO_response_code(dci_response_code);
			}
		}
		else if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
							  DINERS_REVERSAL_REQ_MESSAGE,DINERS_MESSAGE_TYPE_SIZE))||
				(0 == strncmp(Auth_Tx.TLF01_details.message_type,
						DINERS_ISO_REVESAL_ADVICE_REQ_MESSAGE_TYPE,DINERS_MESSAGE_TYPE_SIZE)))
		{
			if(0 == strlen(Auth_Tx.TLF01_details.auth_number))
			{
				if(strlen(Auth_Tx.TLF01_details.orig_auth_number) > 0)
				{
				memcpy( Auth_Tx.TLF01_details.auth_number,
						Auth_Tx.TLF01_details.orig_auth_number,
						DINERS_DE38_SIZE );
				}
			}
			if(0 == strlen(Auth_Tx.TLF01_details.response_code))
			{
				if(strlen( Auth_Tx.TLF01_details.product_codes[12].code)> 0)
				{
					memcpy(	dci_response_code,
							Auth_Tx.TLF01_details.product_codes[12].code,
							DINERS_DE39_SIZE );
					match_DCI_to_ISO_response_code(dci_response_code);
				}
			}
		}

		if((0 == strcmp(Auth_Tx.TLF01_details.response_code, "00")) ||
		   (0 == strcmp(Auth_Tx.TLF01_details.response_code, "55")))
		{
			if ( false == ncdci_update_database( ))
			{
				ncdci_log_message( 1, 3, "Unable to update database tables",
										"ncdci_process_incoming_message",1 );
				/* Unable to complete a transaction.  Inform host; log to TLF01. */
				if(false == ncdci_incoming_other_host_response_db_reply( ) )
				{
					ncdci_log_message( 1, 3, "Unable to send response to host",
											"ncdci_process_incoming_message",1 );
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
			if(false == ncdci_incoming_other_host_response_db_reply())
			{
				ncdci_log_message( 1, 3, "Unable to send response to host",
									"ncdci_process_incoming_message",1 );
			}
		}
		return(true);
	}
}


 
 
 /*****************************************************************************
 
   Function:	ncdci_send_it_back_to_terminal
 
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

 ******************************************************************************/
 BOOLEAN ncdci_send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx )
 {
	pPTE_MSG	p_msg_term_handler_out = NULL_PTR;
	LONG		ret_code = false;
	CHAR	   Buffer[256] = {0};
	CHAR	   ErrorMsg[200] = {0};
	CHAR		sender_que_name[17] = {0};
 
	strcpy(p_auth_tx->TLF01_details.response_code, resp_code);
	p_auth_tx->TLF01_details.tx_key = ncdci_match_request_to_response(p_auth_tx->TLF01_details.tx_key);
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
		ncdci_log_message( 1, 3, "Insufficient Memory to build response to txcntl",
								"ncdci_send_it_back_to_terminal",1 );
		return(false);
	}
	ret_code = pteipc_send (p_msg_term_handler_out, sender_que_name );
	if ( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
	   sprintf( Buffer, "error on pteipc_send to que %s: %s", sender_que_name, ErrorMsg );
	   ncdci_log_message( 1, 3, Buffer, "ncdci_send_it_back_to_terminal", 1 );
	   free(p_msg_term_handler_out);
	   return(false);
	}
	else
	{
	   ncdci_log_message( 2, 1, "Response sent successfully to terminal handler",
			   	   	   	   	   "ncdci_send_it_back_to_terminal",1 );
	}
 
	free(p_msg_term_handler_out);
	return(true);
 }

 /*****************************************************************************
 
   Function:	ncdci_hhutil_verify_pin_block
 
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
 BOOLEAN ncdci_hhutil_verify_pin_block(pAUTH_TX p_auth_tx)
 {
	if(0 == strlen(p_auth_tx->TLF01_details.pin_block))
	   return(false);
	if(ncdci_blank_string(p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block)))
	   return(false);
	return(true);
 }
 
 
 /*****************************************************************************
 
   Function:	ncdci_hhutil_process_debit_transaction
 
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
 BOOLEAN ncdci_hhutil_process_debit_transaction(pAUTH_TX p_auth_tx)
 {
	pPTE_MSG  p_msg = NULL_PTR;
	LONG	  ret_code = false;
	CHAR	  Buffer[256] = {0};
	CHAR	  ErrorMsg[100] = {0};
  
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
			ncdci_log_message( 1, 3, "Insufficient Memory to build pin translation message",
									"ncdci_hhutil_process_debit_transaction",0 );
	    return(false);
	 }
	 ret_code = pteipc_send(p_msg, security_que_name);
	 if( ret_code < 0 )
	 {
		 pteipc_get_errormsg( ret_code, ErrorMsg );
		 sprintf( Buffer, "error on pteipc_send to que %s: %s", security_que_name, ErrorMsg );
		 ncdci_log_message( 1, 3, Buffer, "ncdci_hhutil_process_debit_transaction",0 );
		 free( p_msg );
		 return(false);
	 }
	 
	free( p_msg );
	return(true);
 }
 
 /******************************************************************************
  *
  *  NAME:		   ncdci_process_incoming_message_continued
  *
  *  DESCRIPTION:  This function continues the processing of an incoming
  * 			   message.  It is used as a common stepping stone to get
  * 			   to 'ncdci_incoming_terminal_request'.	It is called after NCF01
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
 INT ncdci_process_incoming_message_continued( pAUTH_TX p_auth_tx )
 {
	if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
	{
	   if(ncdci_incoming_terminal_request(p_auth_tx))
		  return(true);
	   return(false);
	}
	return(false);
	
 }
 
 
 
 /*****************************************************************************
 
   Function:	ncdci_incoming_terminal_request
 
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
******************************************************************************/
 INT ncdci_incoming_terminal_request(pAUTH_TX p_auth_tx)
 {
	pPTE_MSG_DATA		p_msg_data = NULL_PTR;
	BYTE				temp_tx_key = 0;
	BYTE			    current_state[2] = {0};

	if ((p_auth_tx->TLF01_details.general_status > GS_NSP_KEYCHANGE_FAILED) && ( p_auth_tx->TLF01_details.general_status <= GS_NSP_OUTOFSERVICE))
	{
	   if(ncdci_send_it_back_to_terminal("58", p_auth_tx ))
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
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			return(true); 
		 }
	   case AUTH_REFUND: 
		 {	 /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
	   case AUTH_OFFLINE_REFUND: 
		 {	 /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
	   case AUTH_VOID_SALE:
	   { /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
	   case AUTH_VOID_REFUND:
	   { /* create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx); 	  
			return(true);	 
		 }
		 case AUTH_SALE_ADJUSTMENT: 
		 {	 /* Delete the original transaction from the auth_tx_table if it is there */
		  /* create transaction type string and store it in the auth_tx_block	   */
		 
			 p_auth_tx->TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			return(true);	
		 }
	   case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE: 
	   {
		  p_auth_tx->TLF01_details.tx_key =
					  AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE;
		  ncdci_perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
		  return( true );
	   }
	   case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE: 
	   {
		  p_auth_tx->TLF01_details.tx_key =
					  AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE;
		  ncdci_perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
		  return( true );
	   }
	   case AUTH_AUTHORIZATION_OVERRIDE: 
	   {
		  p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_OVERRIDE_RESPONSE;
		  ncdci_perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
		  return( true );
	   }
		case AUTH_REVERSAL: 
		case AUTH_REVERSAL_ADVICE: 
	   { /* Create transaction type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_PERF_REQ, p_auth_tx);
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
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			return(true);	 
		 }
	   case AUTH_VOID_CASH_ADVANCE: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_VOID_CASH_ADVANCE_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
	   case AUTH_SALES_COMPLETION: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_SALES_COMPLETION_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
	   case AUTH_OFFLINE_VOID_SALE: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_SALE_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
	   case AUTH_OFFLINE_VOID_REFUND: 
		 {	 /* create tran type string and store it in the auth_tx_block */
			 p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_REFUND_RESPONSE;
			 ncdci_perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
			 return(true);	  
		 }
		 default:  /* online transaction to host */
		 {
		 }
	 } /* end switch statement */ 
	
	/*************************************************************************************/
	
	/* confirm host is available */
	if (false == ncdci_verify_host_app_status(p_auth_tx))
	{
	   /* build response_for_terminal */
		ncdci_log_message( 1, 1, "Host Inactive",
							"ncdci_incoming_terminal_request",0 );
		p_auth_tx->TLF01_details.tx_key = ncdci_match_request_to_response(p_auth_tx->TLF01_details.tx_key);
 
		if (0 == strcmp(current_state,"2"))  /* check if we are error state */
	   {
			ncdci_log_message( 1, 1, "Host Inactive: current_state!=0",
								"ncdci_incoming_terminal_request",0 );
			/* set up a logon request to the host in temp_auth_tx, increment host_app */
			ncdci_perform_host_logon_request(DINERS_NMI_LOGON, SOURCE_IS_FSVISA, p_auth_tx);
			/* build PTEMSG's to be sent */
			ncdci_send_request_to_host(p_auth_tx);
	   }
 
	   ncdci_perform_standin_processing("58", GS_TXN_DROPPED, p_auth_tx);
		 return(true);
	}
 
	
	/* build PTEMSG's to be sent */
	 ncdci_send_request_to_host(p_auth_tx);
	
	 return(true);
 } /* ncdci_incoming_terminal_request */

 
 
 /*****************************************************************************
 
   Function:	ncdci_perform_host_logon_request
 
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
 ******************************************************************************/
 INT ncdci_perform_host_logon_request(char request_type[], int source,
								pAUTH_TX p_auth_tx)
 {
	char 	tempstr[10] = {0};
	BYTE	net_consec_tmouts_ctr[4] = {0};
	BYTE	active_txns_ctr[4] = {0};
	BYTE	current_state[2] = {0};
	CHAR    table_name[15] = {0};

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
				 ncdci_log_message( 1, 1, "Sending logon request to host",
						 	 	 	 	 "ncdci_perform_host_logon_request",0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
			 }
			 else
				 return (true);
			 break;
		 } /* end case SOURCE_IS_FSVISA */

	   case SOURCE_IS_OPERATOR:
	   {
		  if (false == ncdci_perform_op_net_mngmnt_request(request_type, p_auth_tx))
			 return(false);
		  else
			 return(true);
	   }
		 
		 case SOURCE_IS_HOST:
		 {
			 if (strcmp(DINERS_NMI_SYSTEM_ECHO , request_type)==0)
			 {
				 //ncdci_log_message( 1, 1, "Received Echo request from host:",
						 //	 	 	 	 "ncdci_perform_host_logon_request", 0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				 ncdci_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }

			 else if (strcmp(DINERS_NMI_LOGON , request_type)==0)
			 {
				 if (1 == atoi(current_state))
				 {
					 ncdci_log_message( 1, 1, "Received logon request from host:",
							 	 	 	 	 	 "ncdci_perform_host_logon_request",0 );
					 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					 strncpy(current_state,"0",1);
					 WriteGenericTable( table_name, net_consec_tmouts_ctr,
							 	 	 	active_txns_ctr, current_state );
					 ncdci_perform_op_net_mngmnt_response(request_type, p_auth_tx);
				 }
				 else
				 {
					 ncdci_log_message( 1, 1, "Received logon request from host:",
							 	 	 	 	 "ncdci_perform_host_logon_request",0 );
					 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					 ncdci_perform_op_net_mngmnt_response(request_type, p_auth_tx);
				 }
					 
			 }
			 else if (strcmp(DINERS_NMI_LOGOFF , request_type)==0)
			 {
				 ncdci_log_message( 1, 1, "Received logoff request from host:",
						 	 	 	 	 "ncdci_perform_host_logon_request",0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				 strncpy(current_state,"1",1);
				 WriteGenericTable( table_name, net_consec_tmouts_ctr,
									active_txns_ctr, current_state );
				 ncdci_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }


			 else if (strcmp(DINERS_NMI_KEYTEST , request_type)==0)
			 {
				 ncdci_log_message( 1, 1, "Received Keytest request from host:",
						 	 	 	 	 "ncdci_perform_host_logon_request",0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				 ncdci_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }
		
			 else if (strcmp(DINERS_NMI_KEYCHANGE , request_type)==0)
			 {
				 ncdci_log_message( 1, 1, "Received Keytest request from host:",
						 	 	 	 	 "ncdci_perform_host_logon_request",0 );
				 p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				 ncdci_perform_op_net_mngmnt_response(request_type, p_auth_tx);
			 }

			 break;
		 } /* end case SOURCE_IS_HOST */
 
	} /* end switch statement */
 
	 return (true);
 
 };  /* ncdci_perform_host_logon_request */


 
 
 /*****************************************************************************
 
   Function:	ncdci_verify_host_app_status
 
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
 INT  ncdci_verify_host_app_status(pAUTH_TX p_auth_tx)
 {
	INT   	tempint = 0 ;
	CHAR  	tempstr[14] = {0};
	BYTE	net_consec_tmouts_ctr[4] = {0};
	BYTE	active_txns_ctr[4] = {0};
	BYTE	current_state[2] = {0};
	CHAR     table_name[15] = {0};

    memset( table_name, 0x00, sizeof(table_name) );
    strcpy( table_name, AppName );
    strcat( table_name, "Table" );
 
	/* Get host state and counters */
	ReadGenericTable( table_name, net_consec_tmouts_ctr, 
					  active_txns_ctr, current_state );
 
	if (strcmp(current_state,"0") != 0) 
	{
		ncdci_log_message( 1, 1, "Host app not in state 0, cannot process tran",
								"ncdci_verify_host_app_status",0 );
		return(false);
	}
 
	if (2 == atoi(current_state))  
	{  
		ncdci_log_message( 1, 2, "Maximum consecutive timeouts reached",
								"ncdci_verify_host_app_status",0 );
		strcpy(current_state,"2"); 
		return(false);
	}
	if (atoi(active_txns_ctr) >= atoi(ncf01.max_active_txns))
	{
		ncdci_log_message( 1, 2, "Maximum active transactions reached",
								"ncdci_verify_host_app_status",0 );
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
 }; /* ncdci_verify_host_app_status */

 
 /******************************************************************************
  *
  *  NAME:		   ncdci_perform_op_net_mngmnt_request
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
INT ncdci_perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx )
{
	CHAR	 gmt_time[15] = {0};
	AUTH_TX  auth_tx;
	CHAR	 ncdci_response_code[4] = {0};
	CHAR	 temp[17] = {0};

	memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	auth_tx.TLF01_details.tx_key = AUTH_LOGON;

	strncpy( auth_tx.function_code, request_type,DINERS_NETWORK_FUNCTION_CODE_LEN);
	strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
	strncpy(tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

	if( strcmp(ncf01.logon_bin[0].identifier,"SI") == 0)
	{
		si=1;
	}
	else
	{
		si=0;
	}

	/* Current Time for the transmission-to-host time  */
	ptetime_get_strtime(auth_tx.TLF01_details.host_start_time);

	/* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
	if ( 0 == strcmp( NULL_STR, auth_tx.TLF01_details.retrieval_ref_num))
	{
		ncdci_create_rrn( auth_tx.TLF01_details.retrieval_ref_num );
	}

	/* Use the time hhmmss for sys_trace_audit_num */
	memset(gmt_time, 0, sizeof(gmt_time));
	ptetime_get_strtime (gmt_time);
	ncdci_Rj_with_lead_zeros(gmt_time, sizeof(gmt_time)-1);
	memcpy (auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 7, 3);
	strncat(auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 11,3);

	strncpy( ncdci_response_code, "000", 3);
	strcpy ( auth_tx.forwarding_inst_id_cd, ncf01.forwarding_inst_id_code);
	strcpy ( auth_tx.TLF01_details.processing_code, "000000");
	strcpy ( auth_tx.host_processing_code, "000000");
	genutil_format_date (temp);
	memcpy (auth_tx.TLF01_details.date_yyyymmdd, temp, 8);
	memcpy (auth_tx.TLF01_details.time_hhmmss, temp+8, 6);

	if (0 == strcmp(request_type, DINERS_NMI_SYSTEM_ECHO))
	{
		ncdci_log_message( 1, 1, "Sending Echo Request to host.",
								"ncdci_perform_op_net_mngmnt_request",0 );
	}

	else if (0 == strcmp(request_type, DINERS_NMI_LOGON))
	{
		ncdci_log_message( 1, 1, "Sending LOGON Request to host.",
								"ncdci_perform_op_net_mngmnt_request",0 );
	}
	else if (0 == strcmp(request_type, DINERS_NMI_LOGOFF))
	{
		ncdci_log_message( 1, 1, "Sending LOGOFF Request to host.",
								"ncdci_perform_op_net_mngmnt_request",0);
	}
	else if (0 == strcmp(request_type, DINERS_NMI_KEYCHANGE))
	{
		ncdci_log_message( 1, 1, "Sending Key Exchange Request to host.",
								"ncdci_perform_op_net_mngmnt_request",0 );
	}

	else if (0 == strcmp(request_type, DINERS_NMI_KEYTEST))
	{
		ncdci_log_message( 1, 1, "Sending LOGOFF Request to host.",
								"ncdci_perform_op_net_mngmnt_request",0 );
	}
	memcpy( Auth_Tx.function_code,request_type,DINERS_DE24_SIZE);
	if (false == ncdci_hhutil_send_net_mngmnt_request(&auth_tx, ncdci_response_code))
	{
		return(false);
	}
	else
	{
		return(true);
	}

} /* ncdci_perform_op_net_mngmnt_request */

 
 
 /*****************************************************************************
 
   Function:	ncdci_perform_op_net_mngmnt_response
 
   Description: This function will 
   Author: 
	   Abhishek Verma
   Inputs:
	   nmi_code  - the nmi code
	   p_auth_tx - the auth_tx structure
   Outputs:
	   None
   Return values:
	   true  - indicates successful processing.
	   false - indicates no successful processing.
   Modified by:

 ******************************************************************************/
 int ncdci_perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx)
 {
 
	p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
	if(0 == strncmp( p_auth_tx->TLF01_details.message_type,
					 DINERS_NETWORK_REQ_MESSAGE,
					 DINERS_MESSAGE_TYPE_SIZE))
	{
		strncpy( p_auth_tx->TLF01_details.message_type,
				 DINERS_NETWORK_RESP_MESSAGE,
				 DINERS_MESSAGE_TYPE_SIZE);
	}

	strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id);
   
	ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);
	strcpy(p_auth_tx->TLF01_details.sys_trace_audit_num, p_auth_tx->TLF01_details.sys_trace_audit_num); 
	strcpy(p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx->TLF01_details.retrieval_ref_num);

	if (0 == strcmp(nmi_code, DINERS_NMI_SYSTEM_ECHO))
	{
		ncdci_log_message( 2, 1, "Sending Echo message Response to host.",
								"ncdci_perform_op_net_mngmnt_response",0 );
	}
	else if (0 == strcmp(nmi_code, DINERS_NMI_LOGON))
	{
		ncdci_log_message( 1, 1, "Sending LOGON Response to host.",
								"ncdci_perform_op_net_mngmnt_response",0 );
	}
	else if (0 == strcmp(nmi_code, DINERS_NMI_LOGOFF))
	{
		ncdci_log_message( 1, 1, "Sending LOGOFF Response to host.",
								"ncdci_perform_op_net_mngmnt_response",0 );
	}
	else if (0 == strcmp(nmi_code,DINERS_NMI_KEYCHANGE))
	{
		ncdci_log_message( 1, 1, "Sending Key Reset Response to host.",
								"ncdci_perform_op_net_mngmnt_response",0 );
	}
	else if (0 == strcmp(nmi_code,DINERS_NMI_KEYTEST))
	{
		ncdci_log_message( 1, 1, "Sending Key test Response to host.",
								"ncdci_perform_op_net_mngmnt_response",0 );
	}
	else 
	{
		ncdci_log_message( 1, 1, "Sending ECHO Response to host.",
							"ncdci_perform_op_net_mngmnt_response",0 );
	}
 
	memcpy( Auth_Tx.function_code,nmi_code,DINERS_DE24_SIZE);
	if (false == ncdci_hhutil_send_net_mngmnt_response(p_auth_tx))
	   return(false);
	else
	   return(true);
 
 } /* ncdci_perform_op_net_mngmnt_response */

 
 
 /******************************************************************************
  *
  *  NAME:		   ncdci_set_originator_info
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
 INT ncdci_set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out )
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
  *  NAME:		   ncdci_process_logon_Key_Reset_Transaction
  *
  *  DESCRIPTION:  This function store the PIK coming from ncdci
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
  void ncdci_process_logon_Key_Reset_Transaction()
 {
	Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE;

	memcpy( Ncf01_I.retired_cwk,
			Ncf01_I_key_exchange.retired_cwk,
			strlen(Ncf01_I_key_exchange.retired_cwk));
	memcpy( Ncf01_I.cwk,
			Ncf01_I_key_exchange.cwk,
			strlen(Ncf01_I_key_exchange.cwk));
	memcpy( Ncf01_I.prev_src_station,
			Ncf01_I_key_exchange.prev_src_station,
			strlen(Ncf01_I_key_exchange.prev_src_station));
	ncdci_update_ncf01(&Auth_Tx);
}

  /******************************************************************************
   *
   *  NAME:		   ncdci_process_logon_Key_Reset_Transaction_keyblock
   *
   *  DESCRIPTION:  This function store the PIK coming from ncdci
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
   void ncdci_process_logon_Key_Reset_Transaction_keyblock()
  {
		Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE;

		memcpy( Ncf01_Keyblock.iss_rtdcwk_keyblock,
				Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock,
				strlen(Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock));
		memcpy( Ncf01_Keyblock.iss_cwk_keyblock,
				Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock,
				strlen(Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock));

		memcpy( Ncf01_Keyblock.iss_prev_src_station,
				Ncf01_I_key_exchange_Keyblock.iss_prev_src_station,
				strlen(Ncf01_I_key_exchange_Keyblock.iss_prev_src_station));
		ncdci_update_ncf01_keyblock(&Auth_Tx);
 }
 
 /*****************************************************************************
 
   Function:	ncdci_process_incoming_message_ncf30_reply
 
   Description: This function will process an incoming message from the data
				server request for a select of ncf30.  
   Author: 
	   Abhishek Verma
   Inputs:
	   pCurrent - a pointer to the reply message from the data server
   Outputs:
	   None
   Return values:
	   true  - indicates successful processing.
	   false - indicates no successful processing.
   Modified by:
 ******************************************************************************/
 INT ncdci_process_incoming_message_ncf30_reply(pBYTE pCurrent)
 {
	AUTH_TX   Auth_Tx;
	pPTE_MSG  p_msg_ins = NULL_PTR;
	CHAR	  err_mesg[200] = {0};
	NCF30	  ncf30;
 
 
	memset(&ncf30,	 0, sizeof(NCF30));
	memset(&Auth_Tx, 0, sizeof(AUTH_TX));
	memcpy(&ncf30,	 pCurrent+(sizeof(Auth_Tx)), sizeof(NCF30));
	memcpy(&Auth_Tx, pCurrent, sizeof(AUTH_TX));
	//Check if field 49 is DINERS_PESO or Doller
	if((0 != strcmp(Auth_Tx.TLF01_details.currency_code,DINERS_PESO))&&
		(0 != strcmp(Auth_Tx.TLF01_details.currency_code,DINERS_USD)))
	{
		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code);
		strcpy(Auth_Tx.reconciliation_amount, Auth_Tx.TLF01_details.total_amount);
		strcpy(Auth_Tx.TLF01_details.orig_amount, Auth_Tx.TLF01_details.total_amount);
		strcpy(Auth_Tx.TLF01_details.currency_code,ncf30.currency_code);
	    strcpy(Auth_Tx.TLF01_details.total_amount,ncf30.tran_amount);
	    strcpy(Auth_Tx.TLF01_details.reversal_amount, Auth_Tx.TLF01_details.total_amount);
	}
	strncpy(Auth_Tx.TLF01_details.orig_auth_number,ncf30.auth_number,DINERS_DE38_SIZE);
	strcpy(Auth_Tx.TLF01_details.reversal_amount, Auth_Tx.TLF01_details.total_amount);
    (void)ncdci_hhutil_transfer_ctl_to_auth_app(&Auth_Tx);
	return(true);
 }	
 BOOLEAN ncdci_check_for_cadencieacq_transaction()
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
BOOLEAN ncdci_transfer_response_msg_to_dci2()
 {

		BOOLEAN   retval              = true;
		pPTE_MSG  p_msg_out           = NULL_PTR;
		CHAR	  dci2_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncdci2A" ;
		CHAR      buffer[sizeof(AUTH_TX)+1]= {0};
		BYTE      originator_info[33] = {0};
		LONG      retCode             = 0L;
		CHAR      strError[512]       = {0};

	    memcpy(buffer , &Auth_Tx, sizeof(AUTH_TX) );

	    p_msg_out = ptemsg_build_msg( MT_AUTH_RESPONSE,
	                                  ST1_NONE,
	                                  ST2_NONE,
									  dci2_host_que_name,
	                                  application_que_name,
	                                  (pBYTE) buffer,
									  sizeof(AUTH_TX),
	                                  0 );

	    if ( p_msg_out == NULL_PTR )
	    {
	       sprintf( strError,
	               "Insufficient Memory to build DCI2 request message. Size: %d",
	                buflen );
	       ncdci_log_message( 3, 3, strError , "ncdci_transfer_response_msg_to_dci2" , 0);
	       retval = false;
	    }
	    else
	    {
	       ptemsg_set_orig_comms_info( p_msg_out, originator_info );
	       retCode = pteipc_send( p_msg_out, dci2_host_que_name );
	       free (p_msg_out);

	       if ( retCode < 0 )
	       {
	          pteipc_get_errormsg (retCode, strError);
	          sprintf( strError,
	                  "Error on pteipc_send to que %s: %s",
					  dci2_host_que_name, strError );

	          ncdci_log_message( 3, 3, strError , "ncdci_transfer_response_msg_to_dci2" , 0);
	          retval = false;
	       }
	       ncdci_log_message( 2, 1, "ncdci transfer response_msg to ncdci2" , "ncdci_transfer_response_msg_to_dci2" , 0);
	    }

		return retval;
 }
