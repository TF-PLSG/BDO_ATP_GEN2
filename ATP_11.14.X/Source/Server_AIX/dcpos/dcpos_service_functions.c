/**************************************************************************************************
*  
* MODULE:      service_functions.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: General routines
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
#include "txutils.h"
#include "ntutils.h"
#include "genutil.h"
#include "dcpos_process_incoming_msg.h"
#include "dcpos_process_outgoing_msg.h"
#include "dcpos_general_utilities.h"
#include "dcpos_service_functions.h"

extern AUTH_TX  auth_tx;
extern TPOS01   Tpos_details;
extern BYTE		ReqRsp_Buffer[]; 
extern INT		buflen;
extern CHAR  dcpos_Erro_warning_Filename[256] ;
extern CHAR  dcpos_module_error_warning_file_name[256];
extern CHAR  dcpos_error_warning_file_path[256];

UINT Max_File_Size_Defined = 0 ;

BYTE Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   BYTE general_status, BYTE* response_code)
{
	dcpos_log_message( 1, 3,strTemp,function,1 );
	Prepare_AuthTx_For_Forwarding (general_status,
								   response_code) ;	
				
	return DEVICE_ERROR ;
}

void Prepare_AuthTx_For_Forwarding (BYTE general_status, BYTE* strResponseCode) 
{
	strcpy (auth_tx.TLF01_details.response_code, strResponseCode) ; 
	auth_tx.TLF01_details.general_status = general_status ;
    
} /* Prepare_AuthTx_For_Terminal_Handler_Response */

BYTE Perform_Auth_Tx_Checks (void)
{
	BYTE retCode = 0 ;
	char strError[1024] = {0} ;

	if (auth_tx.TLF01_details.tx_key == AUTH_INVALID)
	{
		sprintf (strError, "Invalid transaction %d - invalid tx key", auth_tx.TLF01_details.tx_key) ;
	}
	else if (!IsFieldNull (auth_tx.TLF01_details.card_num) && !IsFieldNumeric (auth_tx.TLF01_details.card_num))
	{
			sprintf (strError, "Invalid card number %s - not numeric", auth_tx.TLF01_details.card_num ) ;
	}
	else if (!IsFieldNull (auth_tx.TLF01_details.total_amount) && !IsFieldNumeric (auth_tx.TLF01_details.total_amount))
	{
		sprintf (strError, "Amount field %s in transaction is not numeric", auth_tx.TLF01_details.total_amount) ;
	}
	else if (IsFieldNull (auth_tx.TLF01_details.originator_queue))
	{
		strcpy (strError, "Originator que is not populated in transaction") ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.terminal_id, 
														sizeof auth_tx.TLF01_details.terminal_id))
	{
		sprintf (strError, "Invalid terminal id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.terminal_id,
							strlen (auth_tx.TLF01_details.terminal_id), 
							sizeof auth_tx.TLF01_details.terminal_id) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.card_num,
														sizeof auth_tx.TLF01_details.card_num))
	{
		sprintf (strError, "Invalid card number %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.card_num,
							strlen (auth_tx.TLF01_details.card_num), 
							sizeof auth_tx.TLF01_details.card_num) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.sys_trace_audit_num,
														sizeof auth_tx.TLF01_details.sys_trace_audit_num))
	{
		sprintf (strError, "Invalid sys_trace_audit_num %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.sys_trace_audit_num,				
							strlen (auth_tx.TLF01_details.sys_trace_audit_num), 
							sizeof auth_tx.TLF01_details.sys_trace_audit_num) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.primary_key.transaction_id,
														sizeof auth_tx.TLF01_details.primary_key.transaction_id))
	{
		sprintf (strError, "Invalid transaction_id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.primary_key.transaction_id,
							strlen (auth_tx.TLF01_details.primary_key.transaction_id), 
							sizeof auth_tx.TLF01_details.primary_key.transaction_id) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.retrieval_ref_num,
														sizeof auth_tx.TLF01_details.retrieval_ref_num))
	{
		sprintf (strError, "Invalid calculated retrieval_ref_num %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.retrieval_ref_num,
							strlen (auth_tx.TLF01_details.retrieval_ref_num), 
							sizeof auth_tx.TLF01_details.retrieval_ref_num) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.message_type,
														sizeof auth_tx.TLF01_details.message_type))
	{
		sprintf (strError, "Invalid message_type %s length is %d, datastructure length is %d",
							auth_tx.TLF01_details.message_type,
							strlen (auth_tx.TLF01_details.message_type), 
							sizeof auth_tx.TLF01_details.message_type) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.processing_code,
														sizeof auth_tx.TLF01_details.processing_code))
	{
		sprintf (strError, "Invalid processing_code %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.processing_code,
							strlen (auth_tx.TLF01_details.processing_code), 
							sizeof auth_tx.TLF01_details.processing_code) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.date_yyyymmdd,
														sizeof auth_tx.TLF01_details.date_yyyymmdd))
	{
		sprintf (strError, "Invalid date_yyyymmdd %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.date_yyyymmdd,
							strlen (auth_tx.TLF01_details.date_yyyymmdd), 
							sizeof auth_tx.TLF01_details.date_yyyymmdd) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.total_amount,
														sizeof auth_tx.TLF01_details.total_amount))
	{
		sprintf (strError, "Invalid total_amount %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.total_amount,
							strlen (auth_tx.TLF01_details.total_amount), 
							sizeof auth_tx.TLF01_details.total_amount) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.pos_entry_mode,
														sizeof auth_tx.TLF01_details.pos_entry_mode))
	{
		sprintf (strError, "Invalid pos_entry_mode %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.pos_entry_mode,
							strlen (auth_tx.TLF01_details.pos_entry_mode), 
							sizeof auth_tx.TLF01_details.pos_entry_mode) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.nii,
														sizeof auth_tx.TLF01_details.nii))
	{
		sprintf (strError, "Invalid nii %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.nii,
							strlen (auth_tx.TLF01_details.nii), 
							sizeof auth_tx.TLF01_details.nii) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.card_num_len,
														sizeof auth_tx.TLF01_details.card_num_len))
	{
		sprintf (strError, "Invalid card_num_len %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.card_num_len,
							strlen (auth_tx.TLF01_details.card_num_len), 
							sizeof auth_tx.TLF01_details.card_num_len) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.merchant_id,
														sizeof auth_tx.TLF01_details.merchant_id))
	{
		sprintf (strError, "Invalid merchant_id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.merchant_id,
							strlen (auth_tx.TLF01_details.merchant_id), 
							sizeof auth_tx.TLF01_details.merchant_id) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.handler_queue, 
														sizeof auth_tx.TLF01_details.handler_queue))
	{
		sprintf (strError, "Invalid handler que %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.handler_queue,
							strlen (auth_tx.TLF01_details.handler_queue), 
							sizeof auth_tx.TLF01_details.handler_queue) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.card_num_len,
														sizeof auth_tx.TLF01_details.card_num_len))
	{
		sprintf (strError, "Invalid card num length %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.card_num_len,
							strlen (auth_tx.TLF01_details.card_num_len), 
							sizeof auth_tx.TLF01_details.card_num_len) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.pos_condition_code,
														sizeof auth_tx.TLF01_details.pos_condition_code))
	{
		sprintf (strError, "Invalid pos_condition_code %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.pos_condition_code,
							strlen (auth_tx.TLF01_details.pos_condition_code), 
							sizeof auth_tx.TLF01_details.pos_condition_code) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.exp_date,
														sizeof auth_tx.TLF01_details.exp_date))
	{
		sprintf (strError, "Invalid expiry date %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.exp_date,
							strlen (auth_tx.TLF01_details.exp_date), 
							sizeof auth_tx.TLF01_details.exp_date) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.track1,
														sizeof auth_tx.TLF01_details.track1))
	{
		sprintf (strError, "Invalid track1 %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.track1,
							strlen (auth_tx.TLF01_details.track1), 
							sizeof auth_tx.TLF01_details.track1) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.exp_date,
														sizeof auth_tx.TLF01_details.exp_date))
	{
		sprintf (strError, "Invalid expiry date %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.exp_date,
							strlen (auth_tx.TLF01_details.exp_date), 
							sizeof auth_tx.TLF01_details.exp_date) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.pin_block,
														sizeof auth_tx.TLF01_details.pin_block))
	{
		sprintf (strError, "Invalid pin block %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.pin_block,
							strlen (auth_tx.TLF01_details.pin_block), 
							sizeof auth_tx.TLF01_details.pin_block) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.orig_retrieval_ref_num,
														sizeof auth_tx.TLF01_details.orig_retrieval_ref_num))
	{
		sprintf (strError, "Invalid transaction retrieval reference number %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.orig_retrieval_ref_num,
							strlen (auth_tx.TLF01_details.orig_retrieval_ref_num), 
							sizeof auth_tx.TLF01_details.orig_retrieval_ref_num) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.auth_number,
														sizeof auth_tx.TLF01_details.auth_number))
	{
		sprintf (strError, "Invalid auth_number %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.auth_number,
							strlen (auth_tx.TLF01_details.auth_number), 
							sizeof auth_tx.TLF01_details.auth_number) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.visa_merchant_id,
														sizeof auth_tx.TLF01_details.visa_merchant_id))
	{
		sprintf (strError, "Invalid visa merchant id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.visa_merchant_id,
							strlen (auth_tx.TLF01_details.visa_merchant_id), 
							sizeof auth_tx.TLF01_details.visa_merchant_id) ;
	}

	

	if (!IsFieldNull (strError))
	{
		retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, 
											strError,
											"Perform_Auth_Tx_Checks",
											GS_TXN_SYSTEM_ERROR,
											INVALID_TRANSACTION) ;
		return retCode ;
	}

	return DEVICE_OK ;
} /* Perform_Auth_Tx_Checks */


BYTE IsFieldLengthGreaterThanDatabaseLength (unsigned char field[], unsigned int field_datastructure_length)
{
	/* We subtract 1 from <field_datastructure_length> because the datastructure field is always defined to
	   be 1 more than the actual field length (to accomodate the NULL character at the end 
	*/
	if (strlen (field) > (field_datastructure_length - 1))
	{
		return 1 ;
	}
	return 0 ;
		
} /* IsFieldLengthGreaterThanDatabaseLength */

BYTE PopulateAuthTxTimeField (void)
{
	CHAR temp[50] = {0} ;
	genutil_format_date (temp) ;
	/* create time of transaction and store it in the auth_tx table */
	memcpy(auth_tx.TLF01_details.time_hhmmss, temp + 8, 6) ;

	return 1 ;
} /* CalculateTransactionTimeField */  

BYTE PopulateAuthTxDateField (void)
{
	CHAR temp[50] = {0} ;
	genutil_format_date (temp) ;
	/* create date of transaction and store it in the auth_tx table */
	memcpy (auth_tx.TLF01_details.date_yyyymmdd, temp, 8) ;

	return 1 ;
} /* CalculateTransactionDateField */  

BOOLEAN Is_EMV_Transaction(void)
{
	if ( (auth_tx.EMV_details.emv_block[0] == 0x00) &&
		 (auth_tx.EMV_details.emv_block[1] == 0x00)  )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Prepare_TPOS01_details(void)
{
	CHAR  gmt_time[20] = {0};
	CHAR  temp[17] = {0};

	memset (&Tpos_details, 0, sizeof Tpos_details) ;

	strncpy(Tpos_details.primary_key.transaction_id,
			auth_tx.TLF01_details.primary_key.transaction_id,
			strlen(auth_tx.TLF01_details.primary_key.transaction_id));

	Tpos_details.tx_key= auth_tx.TLF01_details.tx_key;

	if(SYSTEM_DATE_LEN == strlen(auth_tx.TLF01_details.system_date))
	{
		strncpy(Tpos_details.primary_key.system_date,
				auth_tx.TLF01_details.system_date,
				strlen(auth_tx.TLF01_details.system_date));
	}
	else
	{
		pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
		strncpy( auth_tx.TLF01_details.system_date, &gmt_time[0], 8 );
		strncpy(Tpos_details.primary_key.system_date,
				auth_tx.TLF01_details.system_date,
				strlen(auth_tx.TLF01_details.system_date));
	}

	strncpy(Tpos_details.message_type,
			auth_tx.TLF01_details.message_type,
			strlen(auth_tx.TLF01_details.message_type));

	strncpy(Tpos_details.card_num,
			auth_tx.TLF01_details.card_num,
			strlen(auth_tx.TLF01_details.card_num));

	//Masking the card Number.
	Tpos_details.card_num[6]  = 'X';
	Tpos_details.card_num[7]  = 'X';
	Tpos_details.card_num[8]  = 'X';
	Tpos_details.card_num[9]  = 'X';
	Tpos_details.card_num[10] = 'X';
	Tpos_details.card_num[11] = 'X';

	strncpy(Tpos_details.processing_code,
			auth_tx.TLF01_details.processing_code,
			strlen(auth_tx.TLF01_details.processing_code));

	if(strlen(auth_tx.TLF01_details.sys_trace_audit_num)>0)
	{
		strncpy(Tpos_details.sys_trace_audit_num,
				auth_tx.TLF01_details.sys_trace_audit_num,
				strlen(auth_tx.TLF01_details.sys_trace_audit_num));
	}
	else
	{
		create_stan_dcpos(auth_tx.TLF01_details.sys_trace_audit_num);
		strncpy(Tpos_details.sys_trace_audit_num,
				auth_tx.TLF01_details.sys_trace_audit_num,
				strlen(auth_tx.TLF01_details.sys_trace_audit_num));
	}

	strncpy(Tpos_details.time_HHMMSS,
			auth_tx.TLF01_details.time_hhmmss,
			strlen(auth_tx.TLF01_details.time_hhmmss));

	strncpy(Tpos_details.date_YYYYMMDD,
			auth_tx.TLF01_details.date_yyyymmdd,
			strlen(auth_tx.TLF01_details.date_yyyymmdd));

	strncpy(Tpos_details.pos_entry_mode,
			auth_tx.TLF01_details.pos_entry_mode,
			strlen(auth_tx.TLF01_details.pos_entry_mode));

	strncpy(Tpos_details.nii,
			auth_tx.TLF01_details.nii,
			strlen(auth_tx.TLF01_details.nii));

	strncpy(Tpos_details.pos_condition_code,
			auth_tx.TLF01_details.pos_condition_code,
			strlen(auth_tx.TLF01_details.pos_condition_code));

	if(strlen(auth_tx.TLF01_details.retrieval_ref_num)>0)
	{
		strncpy(Tpos_details.retrieval_ref_num,
				auth_tx.TLF01_details.retrieval_ref_num,
				strlen(auth_tx.TLF01_details.retrieval_ref_num));
	}
	else
	{
		/* create RRN */
		genutil_format_date(temp);
		strcpy(auth_tx.TLF01_details.retrieval_ref_num, temp + 2);
		strncpy(Tpos_details.retrieval_ref_num,
				auth_tx.TLF01_details.retrieval_ref_num,
				strlen(auth_tx.TLF01_details.retrieval_ref_num));
	}

	strncpy(Tpos_details.response_code,
			auth_tx.TLF01_details.response_code,
			strlen(auth_tx.TLF01_details.response_code));

	strncpy(Tpos_details.terminal_id,
			auth_tx.TLF01_details.terminal_id,
			strlen(auth_tx.TLF01_details.terminal_id));

	strncpy(Tpos_details.merchant_id,
			auth_tx.TLF01_details.merchant_id,
			strlen(auth_tx.TLF01_details.merchant_id));

	strncpy(Tpos_details.bin_low,
			auth_tx.BIN01_details.primary_key.low,
			strlen(auth_tx.BIN01_details.primary_key.low));

	strncpy(Tpos_details.bin_high,
			auth_tx.BIN01_details.high,
			strlen(auth_tx.BIN01_details.high));

	strncpy(Tpos_details.bin_type,
			auth_tx.BIN01_details.primary_key.type,
			strlen(auth_tx.BIN01_details.primary_key.type));

	strncpy(Tpos_details.card_family,
			auth_tx.BIN01_details.card_family,
			strlen(auth_tx.BIN01_details.card_family));

	strncpy(Tpos_details.bin_description,
			auth_tx.BIN01_details.description,
			strlen(auth_tx.BIN01_details.description));

	strncpy(Tpos_details.response_text,
			auth_tx.TLF01_details.response_text,
			strlen(auth_tx.TLF01_details.response_text));

	   /* Unix implements unsigned chars greater than 127 by wrapping around zero.
	    * That causes the tx_key in the database to not be accurate.  So for tx_keys
	    * above 127, subtract 100.  This puts the request tx_key, not the response
	    * tx_key, into the database.
	    */
	   if ( Tpos_details.tx_key > 127 )
	   {
		   Tpos_details.tx_key -= 100;
	   }
}

/******************************************************************************
 *
 *  NAME:         CREATE_STAN_DCPOS
 *
 *  DESCRIPTION:  This function creates a unique 6-digit system trace audit
 *                number (stan).  It uses the last 6 digits of the time -
 *                number of seconds since 1-1-1970.  This is unique for a
 *                cycle of 11 days.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      stan - Six digit stan
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void create_stan_dcpos( pBYTE stan )
{
   INT   len;
   BYTE  time_in_seconds[30]="";

   ptetime_get_strtime( time_in_seconds );
   len = strlen( time_in_seconds );
   if ( len < 10 )
      len = 10;

   /* Time is in format nnnnnnnnn.nnn
    * I want the last 6 n's before the decimal point.
    */
   memcpy( stan, &time_in_seconds[len-10], 6 );
   return;
}

INT dcpos_check_If_CUP_Transaction (void)
{
	if ((0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx.TLF01_details.card_num, CUP_CARD_NUM,1)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT dcpos_check_If_VISA_Transaction (void)
{
	if (auth_tx.TLF01_details.card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT dcpos_check_If_MC_Transaction (void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,auth_tx.TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	auth_tx.TLF01_details.card_num[0]=='5')
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT dcpos_check_If_JCB_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   (auth_tx.TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT dcpos_check_If_AMEX_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   ((auth_tx.TLF01_details.card_num[1] == '4') ||
		(auth_tx.TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT dcpos_check_If_Diners_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   ((auth_tx.TLF01_details.card_num[1] == '0') ||
		(auth_tx.TLF01_details.card_num[1] == '6') ||
		(auth_tx.TLF01_details.card_num[1] == '8') ||
		(auth_tx.TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((auth_tx.TLF01_details.card_num[0]=='6') &&
			 ((auth_tx.TLF01_details.card_num[1] == '0') ||
			 (auth_tx.TLF01_details.card_num[1] == '4') ||
			 (auth_tx.TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void dcpos_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  dcpos_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   GetAppName (appname) ;

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   dcpos_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(dcpos_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  dcpos_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(dcpos_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
   								  "ERROR_WARNING",             /* points to section name         */
   								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
   								  "500",  					/* Default string                 */
								  dcpos_error_warning_file_size,              	/* points to destination buffer   */
   								  sizeof(dcpos_error_warning_file_size)-1,   	 /* size of destination buffer     */
   								  filename                  /* points to ini filename         */
   								);

   	/* File size size conversion */
   	Max_File_Size_Defined = atoi(dcpos_error_warning_file_size);
   	if(Max_File_Size_Defined <= 0)
   	{
   		Max_File_Size_Defined = 500 ;
   	}
   	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(dcpos_error_warning_file_path) > 0) &&
	  (strlen(dcpos_module_error_warning_file_name)> 0))
   {
	   dcpos_create_Error_Filename();
   }
}

INT dcpos_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;
	CHAR  appname[512] = {0};

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(dcpos_Erro_warning_Filename);
    path_len = strlen(dcpos_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&dcpos_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	dcpos_create_Error_Filename();
    }
	if((fp = fopen(dcpos_Erro_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	if(detail == 1)
	{
		if (strlen(auth_tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( auth_tx.TLF01_details.card_num );
			strncpy( tempcard, auth_tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,
					auth_tx.TLF01_details.sys_trace_audit_num,
					auth_tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,
					auth_tx.TLF01_details.sys_trace_audit_num,
					auth_tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);
	if(Max_File_Size_Defined <= file_size)
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size provided, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, WARNING_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void dcpos_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( dcpos_Erro_warning_Filename, dcpos_error_warning_file_path );
    strcat( dcpos_Erro_warning_Filename, dcpos_module_error_warning_file_name );
	strcat( dcpos_Erro_warning_Filename, ".EW." );
    strncat(dcpos_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(dcpos_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(dcpos_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/******************************************************************************
 *
 *  NAME:         dcpos_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void dcpos_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
   INT  Appname_len = 0;
   CHAR  appname[512] = {0};

   GetAppName (appname) ;
   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",appname);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   dcpos_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   if(details == 1)
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
			   	   	   	   	   	   	 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 auth_tx.TLF01_details.terminal_id,
										 auth_tx.TLF01_details.card_num,
										 auth_tx.TLF01_details.merchant_id );
	   }
	   else
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
   }

   return;
}

INT dcpos_check_If_preauth_void_transaction (void)
{
	if ((0 == strncmp(auth_tx.TLF01_details.message_type,PREAUTH_TRAN_MSG_TYPE,
					  ISO_MSG_TYPE_LEN))&&
		(0 == strncmp(auth_tx.TLF01_details.processing_code,ISO_VOID_PROCESSING_CODE,
					  ISO_VOID_PROCESSING_CODE_LEN)))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
