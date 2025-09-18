/**************************************************************************************************
*  
* MODULE:      process_outgoing_msg.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: Routines when a transaction is returned to the terminal  
*
* APPLICATION: dcpos
*
* AUTHOR:  Abhishek Verma
      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basictyp.h"
#include "pteipc.h"
#include "dcpos_global_definitions.h"
#include "genutil.h"  
#include "dcpos_iso_utilities.h"
#include "txutils.h"
#include "ntutils.h"
#include "dcpos_process_incoming_msg.h"
#include "dcpos_process_outgoing_msg.h"
#include "dcpos_service_functions.h"
#include "dcpos_general_utilities.h"

extern BYTE		ReqRsp_Buffer [COMM_BUF_SZE]; 
extern INT		buflen;                  /* total size of ReqRsp_Buffer */
extern INT		reqidx; 

extern struct iso_msg_rec ISO_MESSAGE ; 
extern AUTH_TX auth_tx ;				

extern CHAR     updateds_que_name[] ;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;
extern INT mra_send_falg;
extern DCF01 dcf01;

BYTE Match_Request_To_Response_Transaction_Key (void)
{
switch (auth_tx.TLF01_details.tx_key)
	{
		case AUTH_SALE:
			return AUTH_SALE_RESPONSE ;
		break ;
		case AUTH_REFUND:
			return AUTH_REFUND_RESPONSE ;
		break ;
		case AUTH_CASH_ADVANCE:
			return AUTH_CASH_ADVANCE_RESPONSE ;
		break ;
		case AUTH_PAYMENT:
			return AUTH_PAYMENT_RESPONSE ;
		break ;
		case AUTH_OFFLINE_SALE:
			return AUTH_OFFLINE_SALE_RESPONSE ;
		break ;
		case AUTH_OFFLINE_VOID_SALE:
			return AUTH_OFFLINE_VOID_SALE_RESPONSE ;
		break ;
		case AUTH_OFFLINE_REFUND:
			return AUTH_OFFLINE_REFUND_RESPONSE ;
		break ;
		case AUTH_OFFLINE_VOID_REFUND:
			return AUTH_OFFLINE_VOID_REFUND_RESPONSE ;
		break ;
		case AUTH_SALE_ADJUSTMENT:
			return AUTH_SALE_ADJUSTMENT_RESPONSE ;
		break ;
		case AUTH_REFUND_ADJUSTMENT:
			return AUTH_REFUND_ADJUSTMENT_RESPONSE ;
		break ;
		case AUTH_PAYMENT_ADJUSTMENT:
			return AUTH_PAYMENT_ADJUSTMENT_RESPONSE ;
		break ;
		case AUTH_PRE_AUTHORIZATION:
			return AUTH_PRE_AUTHORIZATION_RESPONSE ;
		break ;
		case AUTH_AUTHORIZATION:
			return AUTH_AUTHORIZATION_RESPONSE ;
		break ;
		case AUTH_BALANCE_INQUIRY:
			return AUTH_BALANCE_INQUIRY_RESPONSE ;
		break ;
		case AUTH_VOID_SALE:
			return AUTH_VOID_SALE_RESPONSE ;
		break ;
		case AUTH_VOID_REFUND:
			return AUTH_VOID_REFUND_RESPONSE ;
		break ;
		case AUTH_SETTLE:
			return AUTH_SETTLE_RESPONSE ;
		break ;
		case AUTH_BATCH_UPLOAD:
			return AUTH_BATCH_UPLOAD_RESPONSE ;
		break ;
		case AUTH_SETTLE_TRAILER:
			return AUTH_SETTLE_TRAILER_RESPONSE ;
		break ;
		case AUTH_STATISTICS:
			return AUTH_STATISTICS_RESPONSE ;
		break ;
		case AUTH_REVERSAL:
			return AUTH_REVERSAL_RESPONSE ;
		break ;
		case AUTH_LOGON:
			return AUTH_LOGON_RESPONSE ;
		break ;
		case AUTH_TEST:
			return AUTH_TEST_RESPONSE ;
		break ;
		case AUTH_PIN_CHANGE:
			return AUTH_PIN_CHANGE_RESPONSE ;
		break ;
		case AUTH_VOID_CASH_ADVANCE:
			return AUTH_VOID_CASH_ADVANCE_RESPONSE ;
		break ;
		case AUTH_SALE_CASH:
			return AUTH_SALE_CASH_RESPONSE ;
		break ;
		case AUTH_CHECK_VERIFICATION:
			return AUTH_CHECK_VERIFICATION_RESPONSE ;
		break ;
		case AUTH_CARD_VERIFICATION:
			return AUTH_CARD_VERIFICATION_RESPONSE ;
		break ;
		case AUTH_VOID_PRE_AUTHORIZATION:
			return AUTH_VOID_PRE_AUTHORIZATION_RESPONSE ;
		break ;
		case AUTH_SALES_COMPLETION:
			return AUTH_SALES_COMPLETION_RESPONSE ;
		break ;
		case AUTH_INITIALIZATION:
			return AUTH_INITIALIZATION_RESPONSE ;
		break ;
		case AUTH_PLEASE_WAIT_ADVICE:
			return AUTH_PLEASE_WAIT_ADVICE_RESPONSE ;
		break ;
		case AUTH_ADMIN_ADVICE:
			return AUTH_ADMIN_ADVICE_RESPONSE ;
		break ;
		case AUTH_VOID_PAYMENT:
			return AUTH_VOID_PAYMENT_RESPONSE ;
		break ;
		case AUTH_REGISTRATION:
			return AUTH_REGISTRATION_RESPONSE ;
		break ;
		case AUTH_QUASI_CASH:
			return AUTH_QUASI_CASH_RESPONSE ;
		break ;
		case AUTH_ADVICE:
			return AUTH_ADVICE_RESPONSE ;
		break ;
		case AUTH_REVERSAL_ADVICE:
			return AUTH_REVERSAL_ADVICE_RESPONSE ;
		break ;
		case AUTH_CARD_UPDATE:
			return AUTH_CARD_UPDATE_RESPONSE ;
		break ;
		case AUTH_RECONCILE_ADVICE:
			return AUTH_RECONCILE_ADVICE_RESPONSE ;
		break ;
		case AUTH_NETWORK_ADVICE:
			return AUTH_NETWORK_ADVICE_RESPONSE ;
		break ;
		case AUTH_ADVICE_REPEAT:
			return AUTH_ADVICE_REPEAT_RESPONSE ;
		break ;
		case AUTH_REVERSAL_REPEAT:
			return AUTH_REVERSAL_REPEAT_RESPONSE ;
		break ;
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
			return AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE ;
		break ;
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
			return AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE ;
		break ;
		case AUTH_AUTHORIZATION_OVERRIDE:
		 return AUTH_AUTHORIZATION_OVERRIDE_RESPONSE ;
		break ;
		case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
		 return AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE ;
		break ;
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
		 return AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE ;
		break ;
		case AUTH_RELOAD:
		 return AUTH_RELOAD_RESPONSE ;
		break ;
		case AUTH_RELOAD_REPEAT:
		 return AUTH_RELOAD_REPEAT_RESPONSE ;
		break ;
		case AUTH_RELOAD_CONFIRM:
		 return AUTH_RELOAD_CONFIRM_RESPONSE ;
		break ;
		case AUTH_VOID_RELOAD:
		 return AUTH_VOID_RELOAD_RESPONSE ;
		break ;
		case AUTH_EMV_ADVICE:
		 return AUTH_EMV_ADVICE_RESPONSE ;
		break ;
	  case  AUTH_EMV_UPLOAD:
		 return AUTH_EMV_UPLOAD_RESPONSE;
	  case AUTH_PRE_AUTHORIZATION_CANCLE:
		  return AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE;
	  break;


		case AUTH_SALE_RESPONSE :
		case AUTH_REFUND_RESPONSE :
		case AUTH_CASH_ADVANCE_RESPONSE :
		case AUTH_PAYMENT_RESPONSE :
		case AUTH_OFFLINE_SALE_RESPONSE :
		case AUTH_OFFLINE_REFUND_RESPONSE :
		case AUTH_SALE_ADJUSTMENT_RESPONSE :
		case AUTH_REFUND_ADJUSTMENT_RESPONSE :
		case AUTH_PAYMENT_ADJUSTMENT_RESPONSE :
		case AUTH_PRE_AUTHORIZATION_RESPONSE :
		case AUTH_AUTHORIZATION_RESPONSE :
		case AUTH_BALANCE_INQUIRY_RESPONSE :
		case AUTH_VOID_SALE_RESPONSE :
		case AUTH_VOID_REFUND_RESPONSE :
		case AUTH_SETTLE_RESPONSE :
		case AUTH_BATCH_UPLOAD_RESPONSE :
		case AUTH_SETTLE_TRAILER_RESPONSE :
		case AUTH_STATISTICS_RESPONSE :
		case AUTH_REVERSAL_RESPONSE :
		case AUTH_LOGON_RESPONSE :
		case AUTH_TEST_RESPONSE :
		case AUTH_PIN_CHANGE_RESPONSE :
		case AUTH_VOID_CASH_ADVANCE_RESPONSE :
		case AUTH_SALE_CASH_RESPONSE :
		case AUTH_CHECK_VERIFICATION_RESPONSE :
		case AUTH_CARD_VERIFICATION_RESPONSE :
		case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE :
		case AUTH_SALES_COMPLETION_RESPONSE :
		case AUTH_INITIALIZATION_RESPONSE :
		case AUTH_PLEASE_WAIT_ADVICE_RESPONSE :
		case AUTH_ADMIN_ADVICE_RESPONSE :
		case AUTH_VOID_PAYMENT_RESPONSE :
		case AUTH_REGISTRATION_RESPONSE :
		case AUTH_QUASI_CASH_RESPONSE :
		case AUTH_ADVICE_RESPONSE :
		case AUTH_REVERSAL_ADVICE_RESPONSE :
		case AUTH_CARD_UPDATE_RESPONSE :
		case AUTH_RECONCILE_ADVICE_RESPONSE :
		case AUTH_NETWORK_ADVICE_RESPONSE :
		case AUTH_ADVICE_REPEAT_RESPONSE :
		case AUTH_REVERSAL_REPEAT_RESPONSE :
		case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE :
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE :
		case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
		case AUTH_OFFLINE_VOID_SALE_RESPONSE :
		case AUTH_OFFLINE_VOID_REFUND_RESPONSE :
		case AUTH_RELOAD_RESPONSE :
		case AUTH_RELOAD_REPEAT_RESPONSE :
		case AUTH_RELOAD_CONFIRM_RESPONSE :
		case AUTH_VOID_RELOAD_RESPONSE :
		case AUTH_EMV_ADVICE_RESPONSE:
		case AUTH_EMV_UPLOAD_RESPONSE	:
		case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE:
			return auth_tx.TLF01_details.tx_key ;
			break ;

		default:
			return AUTH_INVALID ;
		break;
} // switch
} /* Replace_Tx_Key_In_Auth_Tx */

BYTE Send_Transaction_To_Originator (void) 
{
	BYTE MatchingResponseKey = 0 ;
	BYTE retCode = 0 ;
	
	//auth_tx.TLF01_details.tx_key = AUTH_SALE_RESPONSE ;
	auth_tx.TLF01_details.tx_key = Match_Request_To_Response_Transaction_Key() ;
	retCode = Populate_Response_Fields_In_Auth_Tx () ;

	/* Populate the iso_msg_rec structure with data in auth_tx */
	retCode = Populate_ISO_Structure_From_Auth_Tx () ;
	
	/* Build the response message for the terminal in ReqRsp_Buffer */
	isoutil_build_terminal_response (auth_tx.TLF01_details.tx_key) ;

	/* Send the message to the terminal */
	retCode = Send_Response_To_Terminal () ;

	/* Insert the TPOS01 record - drop the transaction */
	retCode = Insert_TPOS01_Details( ST2_NONE ) ; /* Ignore any SYSTEM ERROR */
	return DEVICE_OK ;
} /* Send_Transaction_To_Originator */


BYTE Populate_Response_Fields_In_Auth_Tx (void)
{
	CHAR	temp[17] = {0} ;
	CHAR	tran_finish_time[17] = {0} ;
	CHAR	retrievalRefNum [50] = {0} ;

	memset (tran_finish_time, 0, sizeof tran_finish_time) ;
	ptetime_get_strtime (tran_finish_time);
	memcpy (auth_tx.TLF01_details.tran_finish_time, tran_finish_time, sizeof tran_finish_time) ; 

	/* Copy the original retrieval number to the rrn if orig rrn exists */
	if (strlen (auth_tx.TLF01_details.orig_retrieval_ref_num))
		strcpy (auth_tx.TLF01_details.retrieval_ref_num, auth_tx.TLF01_details.orig_retrieval_ref_num) ;

   
	genutil_format_date (temp) ;
	
	if (strlen(auth_tx.TLF01_details.time_hhmmss) == 0)
	{
		if (strlen (temp) != 0)
			memcpy (auth_tx.TLF01_details.time_hhmmss, temp + 8, 6);
	}

	if (strlen(auth_tx.TLF01_details.date_yyyymmdd) == 0)
	{
		if (strlen (temp) != 0)
			memcpy (auth_tx.TLF01_details.date_yyyymmdd, temp, 8);
	}

   /* Reset stan to zeroes for EFT transactions. */
   if ( 0 == strcmp(auth_tx.TLF01_details.orig_sys_trace_num,"000000") )
   {
      strcpy( auth_tx.TLF01_details.sys_trace_audit_num, "000000" );
   }

	return DEVICE_OK ;
} /* Populate_Response_Fields_In_Auth_Tx */

BYTE Populate_ISO_Structure_From_Auth_Tx (void)
{
   CHAR  temp_str[100] = {0} ;
   BYTE  track2_length[3] = {0} ;
   BYTE  track1_length[3] = {0} ;
   BYTE  retCode = 0 ;
   INT   len = 0 ;

   memset (&ISO_MESSAGE, 0x00, sizeof(ISO_MESSAGE)) ;

    if ((genutil_blank_string (auth_tx.TLF01_details.card_num_len, 
							  sizeof(auth_tx.TLF01_details.card_num_len)) == false) &&
	   (genutil_blank_string (track2_length, sizeof(track2_length))) &&
	   (genutil_blank_string (track1_length, sizeof(track1_length))) )  
	{
		genutil_asc_to_bcd (auth_tx.TLF01_details.exp_date, 
							sizeof(ISO_MESSAGE.date_expd)*2,
							ISO_MESSAGE.date_expd);
		genutil_asc_to_bcd (auth_tx.TLF01_details.card_num_len, 
							sizeof(ISO_MESSAGE.pan_len)*2,
							ISO_MESSAGE.pan_len);
		genutil_asc_to_bcd (auth_tx.TLF01_details.card_num, 
							sizeof(ISO_MESSAGE.pan_bin)*2,
							ISO_MESSAGE.pan_bin);
	}

    genutil_asc_to_bcd (auth_tx.TLF01_details.processing_code, 
						sizeof(ISO_MESSAGE.proccode)*2,
						ISO_MESSAGE.proccode) ;  

   genutil_asc_to_bcd (auth_tx.TLF01_details.total_amount, 
                       sizeof(ISO_MESSAGE.amount_bin)*2,
                       ISO_MESSAGE.amount_bin) ;

    genutil_asc_to_bcd (auth_tx.TLF01_details.sys_trace_audit_num, 
						sizeof(ISO_MESSAGE.stan_bin)*2,
						ISO_MESSAGE.stan_bin);

	genutil_asc_to_bcd (auth_tx.TLF01_details.time_hhmmss, 
						sizeof(ISO_MESSAGE.time_bin)*2,
						ISO_MESSAGE.time_bin) ;


    /* we need to use a temporary string for the date field because we only
	   want to copy the day and month without the year */
    memset (temp_str, 0, sizeof(temp_str)) ;
    memcpy (temp_str, auth_tx.TLF01_details.date_yyyymmdd, 8) ;  

	genutil_str_asc_to_bcd (&temp_str[4], 
							sizeof(ISO_MESSAGE.date_bin)*2, 
							ISO_MESSAGE.date_bin);
   
    if (genutil_blank_string (auth_tx.TLF01_details.pos_entry_mode, 
								sizeof(auth_tx.TLF01_details.pos_entry_mode)) == false)
		genutil_asc_to_bcd (auth_tx.TLF01_details.pos_entry_mode, 
							sizeof(ISO_MESSAGE.posentry)*2, 
							ISO_MESSAGE.posentry) ;

	genutil_asc_to_bcd (auth_tx.TLF01_details.nii, 
						sizeof(ISO_MESSAGE.nii)*2, 
						ISO_MESSAGE.nii) ;

    genutil_asc_to_bcd (auth_tx.TLF01_details.pos_condition_code, 
						sizeof(ISO_MESSAGE.pos_condcd)*2, 
						ISO_MESSAGE.pos_condcd) ;

    memcpy (&ISO_MESSAGE.rrn[0],
			auth_tx.TLF01_details.retrieval_ref_num, 
			sizeof(ISO_MESSAGE.rrn)) ;

	memcpy (&ISO_MESSAGE.authcode[0], 
			auth_tx.TLF01_details.auth_number, 
			sizeof(ISO_MESSAGE.authcode)) ;
			
	memcpy (&ISO_MESSAGE.responsecd[0], 
			auth_tx.TLF01_details.response_code, 
			sizeof(ISO_MESSAGE.responsecd)) ;

	memcpy (&ISO_MESSAGE.termid[0], 
			auth_tx.TLF01_details.terminal_id, 
			sizeof(ISO_MESSAGE.termid)) ;

	memcpy (&ISO_MESSAGE.cardaccid[0], 
			auth_tx.TLF01_details.merchant_id, 
			sizeof(ISO_MESSAGE.cardaccid)) ;

   return retCode ;
} /* Populate_ISO_Structure_From_Auth_Tx */

BYTE Send_Response_To_Terminal (void)
{
	pPTE_MSG	p_msg_out = NULL_PTR ;
	pPTE_MSG	p_msg_auth = NULL_PTR ;
	LONG		retCode = 0L ;
	CHAR		strError[512] = {0};
	
	p_msg_out = ptemsg_build_msg(MT_OUTGOING,
	         					ST1_NONE,
								ST2_NONE,
								auth_tx.TLF01_details.originator_queue,
								application_que_name,
								ReqRsp_Buffer,
								buflen,
								0) ; 

	if (p_msg_out == NULL_PTR)
	{
		strcpy (strError, "Insufficient Memory to build terminal response message") ;
		dcpos_log_message( 1, 3,strError,"Send_Response_To_Terminal",1 );

		return DEVICE_ERROR ;
	}
	// debug ifo

	printf("ORG-QUEUE : %s \n",auth_tx.TLF01_details.originator_queue);
	printf("ORG-INFO : %s \n",auth_tx.TLF01_details.originator_info);
	
	ptemsg_set_orig_comms_info (p_msg_out, auth_tx.TLF01_details.originator_info) ;
	retCode = pteipc_send (p_msg_out, auth_tx.TLF01_details.originator_queue) ;
	
	free (p_msg_out);

	if	(retCode < 0)
	{
		pteipc_get_errormsg (retCode, strError) ;
		sprintf (strError, "Error on pteipc_send to que %s: %s", 
				auth_tx.TLF01_details.originator_queue, 
				strError) ;
		dcpos_log_message( 1, 3,strError,"Send_Response_To_Terminal",1 );
		return DEVICE_ERROR ;
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, &auth_tx, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
   }

	PRINT ("\n***Sending response back to the dialog manager\n\n") ;
	
	return DEVICE_OK ;
} /* Send_Response_To_Terminal */

