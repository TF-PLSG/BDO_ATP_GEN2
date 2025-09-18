
/***************************************************************************
* MODULE:      ncdci_bitfields.c
*  
* TITLE:       PULSE  Network Control Data Field Processor
*  
* DESCRIPTION: This module contains functions
*              to build or parse PULSE host data fields.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      Abhishek

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
#include "nc_dbstruct.h"
#include "diners.h"
#include "ncdci_prototypes.h"
#include "ncdci_bitfields.h"
#include "tx_dbstruct.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     TranFlow;
extern CHAR    ServiceName[];
       INT     NetworkType;
extern INT TIDctr=719; 

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_I_key_exchange;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
DCI3DS2 diners_3ds2;

extern NCF01_KEYBLOCK     Ncf01_Keyblock;
extern NCF01_KEYBLOCK     Ncf01_I_key_exchange_Keyblock;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

extern const struct isobit BITTAB[128+1];

/*Flags*/
extern INT extendedbit;
extern CHAR   MAC[17];
extern CHAR   NMI_Code[4];
extern CHAR   static_Rtdcwk_Key[33];
extern CHAR   static_Rtdcwk_Key_Keyblock[33];
extern CHAR   key_check_value[5];
extern CHAR dci_scheme_timeout_rc[4];
extern INT keyblock_enable_flag_for_DCI;

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_2
 *
 *  DESCRIPTION:  This function will move the Primary Account Number to the Host
 *                 Message structure according to DCI host .
 *
 *  DATA ELEMENT FORMAT :n, 19 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_ncdci_Host_Message-  DCI host specific structure (Global)
 *
 *  RETURN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_2( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	
	memset(ncdci_HOST_MESSAGE.pan, '0', DINERS_MAX_DE2_SIZE );

	if(  true == ncdci_is_issuing_transaction() )
	{
		len = strlen( Auth_Tx.TLF01_details.card_num );
		if( (len > 0) && (len <=DINERS_MAX_DE2_SIZE))
		{
			sprintf(ncdci_HOST_MESSAGE.pan_len, "%02d", len	);
			memcpy( ncdci_HOST_MESSAGE.pan,
					Auth_Tx.TLF01_details.card_num,
					len );
		}
	}

	else if ( ncdci_isnum(Auth_Tx.TLF01_details.card_num) )
	{
		len = strlen( Auth_Tx.TLF01_details.card_num );
		if( (len > 0) && (len <= DINERS_MAX_DE2_SIZE))
		{
			sprintf(ncdci_HOST_MESSAGE.pan_len, "%02d", len  );
			memcpy( ncdci_HOST_MESSAGE.pan,
					Auth_Tx.TLF01_details.card_num,
					len );
		}
		else if(len == 0)
		{
			*result_flag = 1;
			strcpy( err_buf,
					"Unable to build request message to host. No card number." );
		}
		else if(len > DINERS_MAX_DE2_SIZE)
		{
			*result_flag = 1;
			sprintf( err_buf,
			"Unable to build request msg to host card number length is more than 19: %s",
			Auth_Tx.TLF01_details.card_num );
		}
	}
	else
	{
		*result_flag = 1;
		sprintf( err_buf,
		"Unable to build request msg to host. Non numeric card number: %s",
		Auth_Tx.TLF01_details.card_num );
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_3
 *
 *  DESCRIPTION:  This function will move the processing code to the Host
 *                Message structure according to DCI host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf      - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_3( pINT result_flag, pCHAR err_buf )
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.processing_code)>0)
		{
			memcpy( ncdci_HOST_MESSAGE.proccode,
					Auth_Tx.TLF01_details.processing_code,
					DINERS_DE3_SIZE );
		}
	}
	
	else if ( *result_flag == 0 )
	{
		if(true == ncdci_is_reversal())
		{
			memcpy( ncdci_HOST_MESSAGE.proccode,"000000",
					DINERS_DE3_SIZE );
		}
		else if( Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
				 Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION ||
				 Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION)
		{
			memcpy( ncdci_HOST_MESSAGE.proccode,"000000",DINERS_DE3_SIZE );
		}
		else if( strncmp(Auth_Tx.TLF01_details.processing_code,"11",2) == 0)
		{
			memcpy( ncdci_HOST_MESSAGE.proccode,"000000",DINERS_DE3_SIZE);
		}
		else if ( ncdci_isnum(Auth_Tx.TLF01_details.processing_code) )
		{
			memcpy( ncdci_HOST_MESSAGE.proccode,
					Auth_Tx.TLF01_details.processing_code,
					DINERS_DE3_SIZE );
		}
		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_FILE_TRN_MESSAGE_TYPE,
								DINERS_FILE_TRN_MESSAGE_TYPE_LEN))
		{

		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
			"Cannot build request msg for host. Invalid processing code: %s",
			Auth_Tx.TLF01_details.processing_code );
		}
	}
	/* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
	memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,
			ncdci_HOST_MESSAGE.proccode,
			DINERS_DE3_SIZE );
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_4
 *
 *  DESCRIPTION:  This function will move the amount to the Host
 *                Message structure according to DCI host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek
 *
 ******************************************************************************/
void ncdci_build_request_field_4( pINT result_flag, pCHAR err_buf )
{
   if ( *result_flag == 0 )
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  || 
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) )
      {
    	  if(0 == strncmp(Auth_Tx.TLF01_details.product_codes[3].quantity,"PR",2))
    	  {
    		  memcpy( ncdci_HOST_MESSAGE.amount,
    		           			Auth_Tx.TLF01_details.orig_amount,
    		  					DINERS_DE4_SIZE );
    	  }
    	  else if (strlen(Auth_Tx.TLF01_details.total_amount) > 0)
    	  {
         	memcpy( ncdci_HOST_MESSAGE.amount,
         			Auth_Tx.TLF01_details.total_amount,
					DINERS_DE4_SIZE );
    	  }
    	  else
    	  {
    		  *result_flag = 1;
    		  sprintf( err_buf,
				 "Cannot build request msg for host. No amount in reversal transaction");
    	  }
      }

      else if (strlen(Auth_Tx.TLF01_details.total_amount) > 0)
      {
    	  if ( ncdci_isnum(Auth_Tx.TLF01_details.total_amount) )
    	  {
    		  if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
    		  {
    			  memcpy( ncdci_HOST_MESSAGE.amount,
    					  Auth_Tx.TLF01_details.total_amount,
						  DINERS_DE4_SIZE );
    		  }
			  else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
								   Auth_Tx.TLF01_details.currency_code) )
			  {
				  memcpy( ncdci_HOST_MESSAGE.amount,
						  Auth_Tx.TLF01_details.total_amount,
						  DINERS_DE4_SIZE );
			  }
			  else
			  {
				  memcpy( ncdci_HOST_MESSAGE.amount,
						  Auth_Tx.TLF01_details.orig_amount,
						  DINERS_DE4_SIZE);
			  }
    	  }
    	  else
		  {
			   *result_flag = 1;
			   sprintf( err_buf,
					    "Cannot build request msg for host. Non Numeric amount: %s",
					    Auth_Tx.TLF01_details.total_amount );
		 }
      }
      else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
							  DINERS_FILE_TRN_MESSAGE_TYPE,
							  DINERS_FILE_TRN_MESSAGE_TYPE_LEN))
      {

      }
      else if(0 == strlen(Auth_Tx.TLF01_details.total_amount))
      {
    	  //Check if its a valid amount zero transaction
    	 if(false== ncdci_check_if_valid_zero_amount_transaction())
    	 {
			   *result_flag = 1;
			   sprintf( err_buf,
					 "Cannot build request msg for host. Zero amount for invalid tx: %s",
					  Auth_Tx.TLF01_details.total_amount );
    	 }

      }
      else
      {
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid amount: %s",
                Auth_Tx.TLF01_details.total_amount );
      }
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_6
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing amount to the Host
 *                Message structure according to DCI host formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
void ncdci_build_request_field_6( pINT result_flag, pCHAR err_buf )
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.ch_billing_amt)> 0)
		{
			memcpy(ncdci_HOST_MESSAGE.ch_bill_amt,Auth_Tx.ch_billing_amt,DINERS_DE6_SIZE );
		}
	}
	else  if ( *result_flag == 0 )
	{
		// Need to check for offus transaction
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_7
 *
 *  DESCRIPTION:  This function will move the transmission date to 
 *		          the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 * DATA ELEMENT FORMAT :n 10 MMDDhhmmss
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
void ncdci_build_request_field_7( pINT result_flag, pCHAR err_buf )
{
   CHAR  date_str [ 9] = {0};
   CHAR  time_str [ 7] = {0};
   CHAR  time_date[25] = {0};
   CHAR  GMTtime_str_MMDDhhmmss [11] = {0};
   if ( *result_flag == 0 )
   {
	  if( 0  == strncmp(Auth_Tx.TLF01_details.product_codes[3].quantity,"PR",2) &&
			'\0' != Auth_Tx.orig_local_time_date[0]	   )
	  {
			memcpy( ncdci_HOST_MESSAGE.gmt_time,
					Auth_Tx.orig_local_time_date,
					DINERS_DE7_SIZE );
	  }
	  else if ( strlen(Auth_Tx.TLF01_details.transmission_timestamp) > 0 )
      {
    	  memcpy( ncdci_HOST_MESSAGE.gmt_time,
				  Auth_Tx.TLF01_details.transmission_timestamp,
				  DINERS_DE7_SIZE );
      }
      else if ( 0 == strlen(Auth_Tx.TLF01_details.transmission_timestamp) )
	  {
    	  /* Need to create transmission time stamp. Get local time
    	  pintime_get_gmt( time_date );
    	  ncdci_get_date( time_date, date_str );
    	  ncdci_get_time( time_date, time_str );

    	  // transmission_timestamp format MMDDhhmms
    	  // Store MMDD in transmission_timestamp
    	  memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
    			  date_str + DINERS_MMDD_START_POS,
				  DINERS_MMDD_LEN);
    	  // Store hhmmss in transmission_timestamp after storing DDMM
    	  memcpy( Auth_Tx.TLF01_details.transmission_timestamp + 4,
    			  time_str,
				  DINERS_HHMMSS_LEN);
    	  memcpy( ncdci_HOST_MESSAGE.gmt_time,
				  Auth_Tx.TLF01_details.transmission_timestamp,
				  DINERS_DE7_SIZE );*/
    	ncdci_get_GMT_Time_MMDDhhmmss(GMTtime_str_MMDDhhmmss);
        strncpy(Auth_Tx.TLF01_details.transmission_timestamp,
        		GMTtime_str_MMDDhhmmss,DINERS_DE7_SIZE);
    	memcpy( ncdci_HOST_MESSAGE.gmt_time,
    			Auth_Tx.TLF01_details.transmission_timestamp,
				  DINERS_DE7_SIZE );
	  }
      else
      {
    	  memcpy( ncdci_HOST_MESSAGE.gmt_time,
    			  Auth_Tx.TLF01_details.transmission_timestamp,
				  DINERS_DE7_SIZE );
      }
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_11
 *
 *  DESCRIPTION:  This function will move the system trace audit number
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_11( pINT result_flag, pCHAR err_buf )
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.sys_trace_audit_num)>0)
		{
			memcpy(ncdci_HOST_MESSAGE.stan,
			Auth_Tx.TLF01_details.sys_trace_audit_num, 
			DINERS_DE11_SIZE );
		}
	}

	else if ( *result_flag == 0 )
	{
		if(  Auth_Tx.TLF01_details.message_type[1] == '4' )
		{
			// We need to create a seperate STAN for reversal.
			ncdci_create_stan(ncdci_HOST_MESSAGE.stan );
			// We need to store the STAN for key
			memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
					ncdci_HOST_MESSAGE.stan,
					DINERS_DE11_SIZE );
		}
		else if ( ncdci_isnum(Auth_Tx.TLF01_details.sys_trace_audit_num) )
		{
				memcpy( ncdci_HOST_MESSAGE.stan,
						Auth_Tx.TLF01_details.sys_trace_audit_num, 
						DINERS_DE11_SIZE );
		}
		else if ( ncdci_isnum(Auth_Tx.TLF01_details.orig_sys_trace_num) )
		{
			memcpy( ncdci_HOST_MESSAGE.stan,
					Auth_Tx.TLF01_details.orig_sys_trace_num, 
					DINERS_DE11_SIZE );
		}
		else 
		{
				/* create new STAN */
				ncdci_create_stan(ncdci_HOST_MESSAGE.stan );
		}				
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_12
 *
 *  DESCRIPTION:  This function will move the transaction Local time
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 12 YYMMDDhhmmss
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_12( pINT result_flag, pCHAR err_buf )
{
	CHAR  time_date[25] = {0};
	CHAR  time_str[7] = {0};
	CHAR  date_str[9] = {0};
	CHAR  localtime_str[13] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.product_codes[12].amount) > 0)
		{
			memcpy(ncdci_HOST_MESSAGE.time,
				   Auth_Tx.TLF01_details.product_codes[12].amount,
				   DINERS_DE12_SIZE );
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
				DINERS_NETWORK_REQ_MESSAGE,
				DINERS_MESSAGE_TYPE_SIZE))
		{
			 /*if (DINERS_DE7_SIZE == strlen(Auth_Tx.TLF01_details.transmission_timestamp))
			 {
				 // Check if have already prepare this filed while building DE 7
				 ptetime_get_timestamp( time_date );
				 ncdci_get_date( time_date, date_str );
				 // Local Transaction Date And Time fomat  YYMMDDhhmms
				 //Store MMDD in transmission_timestamp
				 memcpy( ncdci_HOST_MESSAGE.time,
						 date_str + DINERS_YY_START_POS,
						 DINERS_YY_LEN);
				  // Store hhmmss in transmission_timestamp after storing DDMM
				 memcpy( ncdci_HOST_MESSAGE.time + DINERS_YY_LEN,
						 Auth_Tx.TLF01_details.transmission_timestamp,
						 DINERS_DE7_SIZE);

			 }
			 else
			 {
				 // Let us prepare Local Transaction Date And Time. We are not storing this in TLF01 as off
				 // now , we will store in some product code if required
				 ptetime_get_timestamp( time_date );
				 ncdci_get_date( time_date, date_str );
				 ncdci_get_time( time_date, time_str );

				  // Local Transaction Date And Time fomat  YYMMDDhhmms
				  // Store MMDD in transmission_timestamp
				  memcpy( ncdci_HOST_MESSAGE.time,
						  date_str + DINERS_YY_START_POS,
						  DINERS_YYMMDD_LEN);
				  // Store hhmmss in transmission_timestamp after storing DDMM
				  memcpy( ncdci_HOST_MESSAGE.time + DINERS_YYMMDD_LEN,
						  time_str,
						  DINERS_HHMMSS_LEN);
			 }*/
			 ncdci_get_local_time(localtime_str);
			 memcpy( ncdci_HOST_MESSAGE.time ,
					 localtime_str,DINERS_DE12_SIZE);
		}
	}

	else if(strlen( Auth_Tx.orig_local_time_date) > 0 &&
			Auth_Tx.TLF01_details.voice_auth[0] == 'Y' &&
			Auth_Tx.TLF01_details.message_type[1] == '4')
	{
		if(strlen(Auth_Tx.TLF01_details.product_codes[12].amount) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.time,
					Auth_Tx.TLF01_details.product_codes[12].amount,
					DINERS_DE12_SIZE );
		}
		else if(strlen(Auth_Tx.orig_local_time_date) < 12 )
		{
			memcpy( ncdci_HOST_MESSAGE.time,&Auth_Tx.TLF01_details.date_yyyymmdd[2],2);
			memcpy( ncdci_HOST_MESSAGE.time+2,
					Auth_Tx.orig_local_time_date,
					10);
			memcpy( Auth_Tx.TLF01_details.product_codes[12].amount ,
					ncdci_HOST_MESSAGE.time,DINERS_DE12_SIZE);
		}
		else
		{
			memcpy( ncdci_HOST_MESSAGE.time,
					Auth_Tx.orig_local_time_date,
					DINERS_DE12_SIZE);
			memcpy( Auth_Tx.TLF01_details.product_codes[12].amount ,
					ncdci_HOST_MESSAGE.time,DINERS_DE12_SIZE);
		}
	}
	else if ( *result_flag == 0 )
	{
		// Let us prepare Local Transaction Date And Time. We are not storing this in TLF01 as off
		// now , we will store in some product code if required
		//ptetime_get_timestamp( time_date );
		//ncdci_get_date( time_date, date_str );
		//ncdci_get_time( time_date, time_str );

		/* Local Transaction Date And Time fomat  YYMMDDhhmms */
		/* Store MMDD in transmission_timestamp */
		//memcpy( ncdci_HOST_MESSAGE.time,
		//date_str + DINERS_YY_START_POS,
		//DINERS_YYMMDD_LEN);
		/* Store hhmmss in transmission_timestamp after storing DDMM*/
		//memcpy( ncdci_HOST_MESSAGE.time + DINERS_YYMMDD_LEN,
		//time_str,
		//DINERS_HHMMSS_LEN);
		 ncdci_get_local_time(localtime_str);
		memcpy( ncdci_HOST_MESSAGE.time,
				 localtime_str,DINERS_DE12_SIZE);

	}
	if(true != ncdci_is_reversal() )
	{
		memcpy( Auth_Tx.TLF01_details.product_codes[12].amount ,
		ncdci_HOST_MESSAGE.time,DINERS_DE12_SIZE);
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_13
 *
 *  DESCRIPTION:  This function will move the Effective Transaction Date
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 YYMM
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_13( pINT result_flag, pCHAR err_buf )
{
	CHAR  time_date[25] = {0};
	CHAR  err_buf1[100]  = {0};

	/*if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.product_codes[15].code)> 0)
		{
			 memcpy( ncdci_HOST_MESSAGE.date,
					 Auth_Tx.TLF01_details.product_codes[15].code,
				     DINERS_DE13_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if (0 == strlen(Auth_Tx.TLF01_details.date_yyyymmdd) )
		{
			ptetime_get_timestamp( time_date );
			ncdci_get_time( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
		}
		memcpy( ncdci_HOST_MESSAGE.date, 
				Auth_Tx.TLF01_details.date_yyyymmdd + DINERS_YY_START_POS,
				DINERS_DE13_SIZE);
	}*/
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_14
 *
 *  DESCRIPTION:  This function will move the expiry date
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 YYMM
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_14( pINT result_flag, pCHAR err_buf )
{
    if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.exp_date)>0)
		{
			memcpy( ncdci_HOST_MESSAGE.date_expd,
					Auth_Tx.TLF01_details.exp_date,
					DINERS_DE14_SIZE );
		}
	}

	else if ( *result_flag == 0 )
	{
		if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.exp_date ) )
		{
			memcpy( ncdci_HOST_MESSAGE.date_expd,
					Auth_Tx.TLF01_details.exp_date,
					DINERS_DE14_SIZE );
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_22
 *
 *  DESCRIPTION:  This function will move the POS entry mode
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :an 12
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_22( pINT result_flag, pCHAR err_buf )
{

	memset (ncdci_HOST_MESSAGE.posentry,0x30,DINERS_DE22_SIZE);

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen(Auth_Tx.TLF01_details.product_codes[16].amount) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.posentry,
					Auth_Tx.TLF01_details.product_codes[16].amount,
					DINERS_DE22_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if ( 0 == strlen(Auth_Tx.TLF01_details.pos_entry_mode) )
		{
			*result_flag = 1;
			strcpy( err_buf, "POS Entry Mode is missing. Decline txn." );
		}
		else if ( false == ncdci_isnum(Auth_Tx.TLF01_details.pos_entry_mode) )
		{
			*result_flag = 1;
			sprintf( err_buf,
			"Cannot build request msg for host. Invalid POS Entry Mode: %s",
			Auth_Tx.TLF01_details.pos_entry_mode );
		}
		else
		{
			ncdci_build_pos_data_from_iso_pos_entry_mode();
			memcpy( ncdci_HOST_MESSAGE.posentry,
					Auth_Tx.TLF01_details.product_codes[16].amount,
					DINERS_DE22_SIZE);
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_23
 *
 *  DESCRIPTION:  This function will move the  Card Sequence Number
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  FFORMAT: n3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_23( pINT result_flag, pCHAR err_buf )
{
   INT	   itemp = 0;
   CHAR    temp_str[4]={0};
   CHAR    temp_str1[4]={0};

   if(  true == ncdci_is_issuing_transaction() )
   {
	   if( strlen(Auth_Tx.EMV_details.pan_sequence_number) > 0)
	   {
		   memcpy( ncdci_HOST_MESSAGE.card_seq_num,
				   Auth_Tx.EMV_details.pan_sequence_number,
				   DINERS_DE23_SIZE );
	   }
   }

   else if ( *result_flag == 0 )
   {
		if (strlen(Auth_Tx.host2_message_seq_nbr) > 0)
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy( temp_str,
					  Auth_Tx.host2_message_seq_nbr + DINERS_DE23_CARD_SEQ_POS_IN_STR,
					  DINERS_DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( ncdci_HOST_MESSAGE.card_seq_num,
					  temp_str1,
					  DINERS_DE23_SIZE );
		}
		else if (strlen(Auth_Tx.EMV_details.pan_sequence_number)> 0)
		{
			  memcpy( temp_str,
					  Auth_Tx.EMV_details.pan_sequence_number,
					  DINERS_DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( ncdci_HOST_MESSAGE.card_seq_num,
					  temp_str1,
					  DINERS_DE23_SIZE );
		}
  }
}
  

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_24
 *
 *  DESCRIPTION:  This function will move the  Function Code
 *                to the Host Message structure according to DCI Standard
 *                formatting specifications.  d
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_24( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[3] = {0};

   if(  true == ncdci_is_issuing_transaction() )
   {
	   if(strlen(Auth_Tx.function_code)>0)
	   {
		   memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
				   Auth_Tx.function_code,
				   DINERS_DE24_SIZE);
	   }
	}

   else if (true == ncdci_is_sbatch_transaction())
   {
	   memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
			   Auth_Tx.TLF01_details.product_codes[0].code,
	  		   DINERS_DE24_SIZE);
   }
   else if ( *result_flag == 0 )
   {
		if(strlen(Auth_Tx.function_code)>0)
		{
			memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
					Auth_Tx.function_code,
					DINERS_DE24_SIZE);
		}
		else if(true == ncdci_Sale_transactio())
		{
			memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
					DINERS_DE24_FUNCION_CODE_MTI_1100_100,
					DINERS_DE24_SIZE);
		}
		else if(true == ncdci_Auth_transactio())
		{
			memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
					DINERS_DE24_FUNCION_CODE_MTI_1100_100,
					DINERS_DE24_SIZE);
		}
		else if(true == ncdci_Auth_Advice_transaction())
		{
			memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
					DINERS_DE24_FUNCION_CODE_MTI_1120_100,
					DINERS_DE24_SIZE);
		}
		else if (true == ncdci_is_reversal())
		{
			if( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,
							 DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL,
							 DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
			{
				// Its void transaction
				memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
						DINERS_DE24_FUNCION_CODE_MTI_1420_400,
						DINERS_DE24_SIZE);
			}
			else if (0 == strncmp(Auth_Tx.TLF01_details.product_codes[3].quantity,"PR",2))
			{
				/* partial reversal */
				memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
						DINERS_DE24_FUNCION_CODE_MTI_1420_400,
						DINERS_DE24_SIZE);
			}
			else
			{
				// Its time out reversal (0400)
				memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
						DINERS_DE24_FUNCION_CODE_MTI_1420_440,
						DINERS_DE24_SIZE);
			}
#if BDOR_62_PREAUTH_VOID
			if ( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,
							  DINERS_DE3_PROC_CODE_ISO_VOID_PRE_AUTH_VAL,
							  DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
			{
				if (Auth_Tx.TLF01_details.saf[0] == INCREMENTAL_AUTH)
				{
					memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
							DINERS_DE24_FUNCION_CODE_MTI_1420_401,
							DINERS_DE24_SIZE);
				}
				else if (Auth_Tx.TLF01_details.saf[0] == PRIMARY_AUTH)
				{
					memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
							DINERS_DE24_FUNCION_CODE_MTI_1420_400,
							DINERS_DE24_SIZE);
				}
			}
			if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7) )
			{
				/* Full reversal  - DE24 : 400 */
				memcpy( ncdci_HOST_MESSAGE.net_manag_info_code,
						DINERS_DE24_FUNCION_CODE_MTI_1420_400,
						DINERS_DE24_SIZE);
			}
#endif
		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_25
 *
 *  DESCRIPTION:  This function will move the Message Reason Code
 *                to the Host Message structure according to DCI  hos
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  hos specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_25( pINT result_flag, pCHAR err_buf )
{
    if(  true == ncdci_is_issuing_transaction() )
	{
		if ( strlen(Auth_Tx.TLF01_details.product_codes[5].code) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.pos_condcd,
					Auth_Tx.TLF01_details.product_codes[5].code,
					DINERS_DE25_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if ( strlen(Auth_Tx.TLF01_details.pos_condition_code) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.pos_condcd,
					Auth_Tx.TLF01_details.pos_condition_code,
					DINERS_DE25_SIZE);
		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
					 "Cannot build request msg for host missing Message Reason Code");
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_26
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Business Code
 *                to the Host Message structure according to DCI  hos
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  hos specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_26( pINT result_flag, pCHAR err_buf )
{
   if(  true == ncdci_is_issuing_transaction() )
	{
		if ( strlen(Auth_Tx.TLF01_details.category_code) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.accpt_bus_cd,
			Auth_Tx.TLF01_details.category_code,
			DINERS_DE26_SIZE);
		}
	}

   else if ( *result_flag == 0 )
	{
		if ( strlen(Auth_Tx.TLF01_details.category_code) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.accpt_bus_cd,
			Auth_Tx.TLF01_details.category_code,
			DINERS_DE26_SIZE);
		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
					 "Can not build host message ,missing Card Acceptor Business Code");
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_30
 *
 *  DESCRIPTION:  This function will move thePoint of Original Amount
 *                to the Host Message structure according to DCI  hos
 *                formatting specifications.
 *
 * DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_30( pINT result_flag, pCHAR err_buf )
{
   LONG   est_amt = 0.0;
   BYTE   estmatd_amount[13] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.mcard_banknet) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.Original_amount,
					Auth_Tx.TLF01_details.mcard_banknet,
					DINERS_DE30_SIZE );
		}
	}
#if BDOR_62_PREAUTH_VOID
	else if (AUTH_VOID_PRE_AUTHORIZATION == Auth_Tx.TLF01_details.tx_key)
	{
		if (Auth_Tx.TLF01_details.saf[0] == INCREMENTAL_AUTH )
		{
			est_amt = atol(Auth_Tx.TLF01_details.total_interest) -
							atol(Auth_Tx.TLF01_details.orig_amount);
			sprintf(estmatd_amount, "%012ld", est_amt);

			memcpy(ncdci_HOST_MESSAGE.Original_amount,
					estmatd_amount,
					DINERS_DE30_SIZE);
		}
		else if (Auth_Tx.TLF01_details.saf[0] == PRIMARY_AUTH )
		{
			/* Not applicable for full reversal  or canceln of
			 * estimated/original preauth transaction
			 */
		}
	}
#endif
	else if ( *result_flag == 0 )
	{
		if( strlen(Auth_Tx.TLF01_details.total_amount) > 0)
		{
			// Need to calculate for OFF US transaction/stip
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_32
 *
 *  DESCRIPTION:  This function will move the acquiring institution ID
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_32( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	CHAR temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen(Auth_Tx.acq_inst_id_cd)> 0 )
		{
			len = strlen( Auth_Tx.acq_inst_id_cd );
			memcpy( ncdci_HOST_MESSAGE.acq_inst_id,
					Auth_Tx.acq_inst_id_cd,
					len);
		}
	}

	else if ( *result_flag == 0 )
	{
		len = strlen( Ncf01_I.acquirer_inst_id_code );
		memcpy( ncdci_HOST_MESSAGE.acq_inst_id,
				Ncf01_I.acquirer_inst_id_code,
				len );
	}

	/* Get the length. */
	if(len > 0)
	{
		sprintf( temp_len, "%02d", len );
		memcpy( ncdci_HOST_MESSAGE.acq_inst_len,
				temp_len,
				DINERS_DE32_LEN_SIZE );

		/* Store value in TLF01 if there is not already a value. */
		if ( Auth_Tx.TLF01_details.acquiring_id[0] == 0x00 )
		{
			memcpy( Auth_Tx.TLF01_details.acquiring_id,
					ncdci_HOST_MESSAGE.acq_inst_id,
					len );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_33
 *
 *  DESCRIPTION:  This function will move the forwarding institution ID
 *                to the Host Message structure according to  DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message-  DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_33( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	CHAR temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(AUTH_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key ||
				AUTH_ADVICE == Auth_Tx.TLF01_details.tx_key)
		{
			len = strlen( Ncf01_I.forwarding_inst_id_code );
			sprintf( temp_len, "%02d", len );
			memcpy(ncdci_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					DINERS_DE33_LEN_SIZE );

			memcpy(ncdci_HOST_MESSAGE.fwd_inst_id,
					Ncf01_I.forwarding_inst_id_code,
					len );
		}
		else if(strlen(Auth_Tx.TLF01_details.product_codes[3].amount)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.product_codes[3].amount);
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.fwd_inst_len, temp_len, DINERS_DE100_SIZE_LEN);

			/*Store data*/
			memcpy(ncdci_HOST_MESSAGE.fwd_inst_id,
			Auth_Tx.TLF01_details.product_codes[3].amount, len);
		}
		else if ( strlen(Auth_Tx.forwarding_inst_id_cd) > 0 )
		{
			len = strlen( Auth_Tx.forwarding_inst_id_cd );
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					DINERS_DE33_LEN_SIZE );

			memcpy( ncdci_HOST_MESSAGE.fwd_inst_id,
					Auth_Tx.forwarding_inst_id_cd,
					len );
		}
		else 
		{
			len = strlen( Ncf01_I.forwarding_inst_id_code );
			sprintf( temp_len, "%02d", len );
			memcpy(ncdci_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					DINERS_DE33_LEN_SIZE );

			memcpy(ncdci_HOST_MESSAGE.fwd_inst_id,
					Ncf01_I.forwarding_inst_id_code,
					len );
		}
	}

	else if ( *result_flag == 0 )
	{
		if ( strlen(Auth_Tx.forwarding_inst_id_cd) > 0 )
		{
			len = strlen( Auth_Tx.forwarding_inst_id_cd );
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					DINERS_DE33_LEN_SIZE );

			memcpy( ncdci_HOST_MESSAGE.fwd_inst_id,
					Auth_Tx.forwarding_inst_id_cd,
					len );
		}
		else 
		{
			len = strlen( Ncf01_I.forwarding_inst_id_code );
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					DINERS_DE33_LEN_SIZE );

			memcpy( ncdci_HOST_MESSAGE.fwd_inst_id,
					Ncf01_I.forwarding_inst_id_code,
					len );
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_35
 *
 *  DESCRIPTION:  This function will move the track2 data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :z 37 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_35( pINT result_flag, pCHAR err_buf )
{
	INT   len = 0;
	CHAR temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.track2) > 0 )
		{
			len = strlen( Auth_Tx.TLF01_details.track2 );
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.track2_len,
					temp_len,
					DINERS_DE35_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.track2,
					Auth_Tx.TLF01_details.track2 ,
					len);
		}
	}
	else if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.track2[0] != 0x00)
		{
			len = strlen( Auth_Tx.TLF01_details.track2 );
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.track2_len,
					temp_len,
					DINERS_DE35_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.track2,
					Auth_Tx.TLF01_details.track2,
					len );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_37
 *
 *  DESCRIPTION:  This function will move the Acquirer reference number
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.  I
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host  specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_37( pINT result_flag, pCHAR err_buf )
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION )
	{
		if (strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.orig_retrieval_ref_num,
					DINERS_DE37_SIZE);
		}
		else
		{
			memcpy( ncdci_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.retrieval_ref_num,
					DINERS_DE37_SIZE);
		}
	}
	else
	{
		if (strlen(Auth_Tx.TLF01_details.retrieval_ref_num) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.retrieval_ref_num,
					DINERS_DE37_SIZE);
		}
		else if (strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.orig_retrieval_ref_num,
					DINERS_DE37_SIZE);
		}
		else
		{
			ncdci_create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_38
 *
 *  DESCRIPTION:  This function will move the authorization number
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_38( pINT result_flag, pCHAR err_buf )
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					    DINERS_ADVICE_REQ_MESSAGE,
					    DINERS_MESSAGE_TYPE_SIZE))
		{
			if(strlen(Auth_Tx.TLF01_details.orig_auth_number) > 0)
			{
				memcpy( ncdci_HOST_MESSAGE.authcode,
						Auth_Tx.TLF01_details.orig_auth_number,
						DINERS_DE38_SIZE );
			}
			else
			{
				memcpy(	ncdci_HOST_MESSAGE.authcode,
						Auth_Tx.TLF01_details.auth_number,
						DINERS_DE38_SIZE );
			}
		}
		else if ( strlen(Auth_Tx.TLF01_details.auth_number)> 0 )
		{
			memcpy(	ncdci_HOST_MESSAGE.authcode,
					Auth_Tx.TLF01_details.auth_number,
					DINERS_DE38_SIZE );
		}
	}
	else if ( *result_flag == 0 )
	{
		if (true == ncdci_is_reversal())
		{
			if( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,
							 DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL,
							 DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
			{
				// Its void transaction
				memcpy(	ncdci_HOST_MESSAGE.authcode,
						Auth_Tx.TLF01_details.auth_number,
						DINERS_DE38_SIZE );
			}
			else if(true == ncdci_is_voice_transaction())
			{
				if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
								DINERS_ISO_REVESAL_REQ_MESSAGE_TYPE,
								DINERS_MESSAGE_TYPE_SIZE))
				{
					// Its void transaction coming from Voice
					memcpy(	ncdci_HOST_MESSAGE.authcode,
							Auth_Tx.TLF01_details.auth_number,
							DINERS_DE38_SIZE );
				}
			}
			else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
			{
				memcpy(	ncdci_HOST_MESSAGE.authcode,
						Auth_Tx.TLF01_details.auth_number,
						DINERS_DE38_SIZE );
			}
			else if (strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
			{
				memcpy(	ncdci_HOST_MESSAGE.authcode,
						Auth_Tx.TLF01_details.auth_number,
						DINERS_DE38_SIZE );
			}
			else
			{
				// Its time out reversal (0400)
				// No Need to send DE38.
			}
		}
		else if ( strlen(Auth_Tx.TLF01_details.auth_number)> 0 )
		{
			memcpy(	ncdci_HOST_MESSAGE.authcode,
					Auth_Tx.TLF01_details.auth_number,
					DINERS_DE38_SIZE );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_39
 *
 *  DESCRIPTION:  This function will move the response code
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *   DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_39( pINT result_flag, pCHAR err_buf )
{
   CHAR dci_response_code[4] = {0};

   if(  true == ncdci_is_issuing_transaction() )
	{	
		if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					   DINERS_ADVICE_REQ_MESSAGE,
					   DINERS_MESSAGE_TYPE_SIZE))
		{
		   memcpy( ncdci_HOST_MESSAGE.responsecd,
				   Auth_Tx.TLF01_details.product_codes[12].code,
				   DINERS_DE39_SIZE );
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
							 DINERS_NETWORK_RESP_MESSAGE,
						     DINERS_MESSAGE_TYPE_SIZE))
		{
			memcpy(	ncdci_HOST_MESSAGE.responsecd,
					DINERS_NETMGMT_APPROVED,
					DINERS_DE39_SIZE );
		}
	    else if(strlen( Auth_Tx.TLF01_details.response_code)> 0)
		{
			match_ISO_to_Diners_response_code(dci_response_code);
			memcpy(	ncdci_HOST_MESSAGE.responsecd,
					dci_response_code,
					DINERS_DE39_SIZE );
			//DCI 3DS2
			if (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+0, DINERS_DE122_AUTH_DATA_TYPE_PRODUCTBUY_VALUE, 1) &&
					 0 != strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, "00", 2))
			{
				if (strlen(Auth_Tx.TLF01_details.product_codes[12].code)> 0)
				{
				memcpy(	ncdci_HOST_MESSAGE.responsecd,
						Auth_Tx.TLF01_details.product_codes[12].code,
						DINERS_DE39_SIZE );
			}
		}
		}
	    else if(0 == strncmp(Auth_Tx.TLF01_details.response_text,"LR",2))
	    {
			memcpy(	ncdci_HOST_MESSAGE.responsecd,
					dci_scheme_timeout_rc,
							DINERS_DE39_SIZE );
	    }
		else
		{
			memcpy(	ncdci_HOST_MESSAGE.responsecd,
					DINERS_APPROVED,
					DINERS_DE39_SIZE );
		}

		//OCT MAND 2021
		if (0== strncmp(Auth_Tx.function_code,"108",2) &&
			0==strncmp(Auth_Tx.TLF01_details.processing_code,"18",2) &&
			0==strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN)) //card acount verification
		{

			if(0==strncmp(Auth_Tx.TLF01_details.product_codes[12].code,"086",3))
			{
				memcpy(	ncdci_HOST_MESSAGE.responsecd,DINERS_CARD_ACCT_VERIFICATION_SUCCESS,DINERS_DE39_SIZE );
			}
			else if(0==strncmp(Auth_Tx.TLF01_details.product_codes[12].code,"197",3))
			{
				memcpy(	ncdci_HOST_MESSAGE.responsecd,DINERS_CARD_ACCT_VERIFICATION_FAILURE,DINERS_DE39_SIZE );
			}
		}
	}

	else if ( *result_flag == 0 )
	{
		if(strlen( Auth_Tx.TLF01_details.response_code)> 0)
		{
			match_ISO_to_Diners_response_code(dci_response_code);
			memcpy(	ncdci_HOST_MESSAGE.responsecd,
					dci_response_code,
					DINERS_DE39_SIZE );
		}
		else
		{
			memcpy(	ncdci_HOST_MESSAGE.responsecd,
					DINERS_APPROVED,
					DINERS_DE39_SIZE );
		}
	}
}


/*****************DINERS_APPROVED*************************************************************
 *
 *  NAME:         ncdci_build_request_field_40
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to DCI Standard
 *                formatting specifications.  I
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_40( pINT result_flag, pCHAR err_buf )
{
	CHAR temp_str[8] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen(Auth_Tx.TLF01_details.cvc_data) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.service_code,
					Auth_Tx.TLF01_details.cvc_data,
					DINERS_DE40_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if (strlen(Auth_Tx.TLF01_details.cvc_data) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.service_code,
					Auth_Tx.TLF01_details.cvc_data,
					DINERS_DE40_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_41
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *   DATA ELEMENT FORMAT :ans 8
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_41( pINT result_flag, pCHAR err_buf )
{
	INT  i = 0;
	CHAR temp_str[9] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.terminal_id)>0)
		{
			memcpy( ncdci_HOST_MESSAGE.termid,
					Auth_Tx.TLF01_details.terminal_id,
					DINERS_DE41_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if (strlen(Auth_Tx.TLF01_details.terminal_id) > 0)
		{
			strcpy(temp_str, Auth_Tx.TLF01_details.terminal_id);
			/* If Device number less than 8, intead of NULL fills with spaces*/
			for (i = strlen(Auth_Tx.TLF01_details.terminal_id); i <8 ; i++)
			{
				temp_str[i] = ' ';
			}
			memcpy( ncdci_HOST_MESSAGE.termid,temp_str, DINERS_DE41_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_42
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Identification Code
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 15
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_42( pINT result_flag, pCHAR err_buf )
{
   if(  true == ncdci_is_issuing_transaction() )
   {
   		if(strlen(Auth_Tx.TLF01_details.merchant_id)>0)
   		{
   			memcpy( ncdci_HOST_MESSAGE.cardaccid,
   					Auth_Tx.TLF01_details.merchant_id,
					DINERS_DE42_SIZE);
   		}
   }

   else if ( *result_flag == 0 )
   {
	   	if (strlen(Auth_Tx.TLF01_details.merchant_id) > 0)
		{
			memset (ncdci_HOST_MESSAGE.cardaccid,0x20,DINERS_DE42_SIZE);
			memcpy( ncdci_HOST_MESSAGE.cardaccid,Auth_Tx.TLF01_details.merchant_id,DINERS_DE42_SIZE);
		}
    }
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_43
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location
 *                to the Host Message structure according to  DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 99 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- Card Acceptor Name/Location specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_43( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len = 0;
   INT  F43_len = 0;
   INT  city_last_position = 0;
   BYTE temp_len[3] = {0};

   /*
		Subfield Name       format  length		length type
		name\street\city\ 	ans 	83 			variable length
		postal code			ans 	10 			fixed length
		region 				ans 	3 			fixed length
		country code 		n 		3 			fixed length
	*/

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen (Auth_Tx.int_net_facility_data) > 0)
		{
			memset (ncdci_HOST_MESSAGE.cardaccname,0x20,DINERS_MAX_DE43_SIZE);

			len = strlen( Auth_Tx.int_net_facility_data);
			sprintf(temp_len, "%02d", len);
			memset ( ncdci_HOST_MESSAGE.cardaccname_len,
					 temp_len,
					 DINERS_DE43_LEN_SIZE);

			memset ( ncdci_HOST_MESSAGE.cardaccname,
					 Auth_Tx.int_net_facility_data,
					 len);
		}
	}
#if BDOR_62_PREAUTH_VOID
	else if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,parvbh_que_name,7) )
	{
		return ;
	}
#endif
	else if( *result_flag == 0)
	{
		memset (ncdci_HOST_MESSAGE.cardaccname,0x20,DINERS_MAX_DE43_SIZE);

		/*Prepare Merchant Name */
		if(0 != strcmp(Auth_Tx.MCF01_details.name02,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.name02);
			memcpy( ncdci_HOST_MESSAGE.cardaccname,
					Auth_Tx.MCF01_details.name02,
					len);
		}
		else if( 0 != strcmp(Auth_Tx.MCF01_details.name01,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.name01);
			memcpy( ncdci_HOST_MESSAGE.cardaccname,
					Auth_Tx.MCF01_details.name01,
					len);
		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
					 "Invalid Merchant, without Merchant name (field 43): %s,",
					 Auth_Tx.TLF01_details.merchant_id );
			return ;
		}
		F43_len = len +1 ;

		/*Add / after merchant name*/
		//memset( ncdci_HOST_MESSAGE.cardaccname + F43_len,0x5c,1);
		ncdci_HOST_MESSAGE.cardaccname[F43_len]=0x5c;

		F43_len = F43_len + 1;

		/*Prepare street Name */
		if(0 != strcmp(Auth_Tx.MCF01_details.address01,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.address01);
			memcpy( ncdci_HOST_MESSAGE.cardaccname + F43_len,
					Auth_Tx.MCF01_details.address01,
					len);
			F43_len = F43_len + len;
		}
		/*if( 0 != strcmp(Auth_Tx.MCF01_details.address02,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.address02);
			memcpy( ncdci_HOST_MESSAGE.cardaccname + F43_len,
					Auth_Tx.MCF01_details.address02,
					len);
			F43_len = F43_len + len;
		}*/
		/*Add / after street name*/

		F43_len = F43_len + 1;
		//memset( ncdci_HOST_MESSAGE.cardaccname + F43_len,0x5c,1);
		ncdci_HOST_MESSAGE.cardaccname[F43_len]=0x5c;

		F43_len = F43_len + 1;

		/*Prepare city Name */
		if(0 != strcmp(Auth_Tx.MCF01_details.city,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.city);
			memcpy( ncdci_HOST_MESSAGE.cardaccname + F43_len,
					Auth_Tx.MCF01_details.city,
					len);
		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
					 "Invalid Merchant, without city name (field 43): %s,",
					 Auth_Tx.TLF01_details.merchant_id );
			return ;
		}
		city_last_position = DINERS_DE43_POSTAL_CODE_POSTION - 1;

		/*Add / after street name at 82nd position*/
		//memset( ncdci_HOST_MESSAGE.cardaccname + city_last_position,0x5c,1);
		ncdci_HOST_MESSAGE.cardaccname[city_last_position]=0x5c;

		/*Add POSTAL CODE*/
		if(0 != strcmp(Auth_Tx.MCF01_details.zip_code,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.zip_code);
			memcpy( ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_POSTAL_CODE_POSTION,
					Auth_Tx.MCF01_details.zip_code,
					len);
		}
		else
		{
			memcpy( ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_POSTAL_CODE_POSTION,
					"10062",
					5);
		}

		/*Add Region CODE*/
		if(0 != strcmp(Auth_Tx.MCF01_details.state,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.state);
			memcpy( ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_REGION_POSTION,
					Auth_Tx.MCF01_details.state,
					len);
		}
		else
		{
			memcpy( ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_REGION_POSTION,
					"001",
					3);
		}

		/*Add country code CODE*/
		memcpy( ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_COUNTRY_CODE_POSTION,
			    "608",DINERS_DE43_COUNTRY_CODE_LEN);

		strncpy(temp_len, DINERS_DE43_MAX_SIZE, DINERS_DE43_LEN_SIZE);
		memcpy ( ncdci_HOST_MESSAGE.cardaccname_len,
				 temp_len,
				 DINERS_DE43_LEN_SIZE);
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_44
 *
 *  DESCRIPTION:  This function will move the Additional Response Data
 *                to the Host Message structure according to  DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 22 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message-  DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_44( pINT result_flag, pCHAR err_buf )
{
	INT len = 0;
	BYTE temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if((strlen(Auth_Tx.acct_id_1)> 0 ))
		{
			memcpy( temp_len,Auth_Tx.acct_id_1,DINERS_DE44_LEN_SIZE);
			len=atoi(temp_len);
			memcpy( ncdci_HOST_MESSAGE.addl_rsp_len,
					temp_len,DINERS_DE44_LEN_SIZE);
			memcpy( ncdci_HOST_MESSAGE.addlrspdata,
					Auth_Tx.acct_id_1 + DINERS_DE44_LEN_SIZE,len);


			if (true == ncdci_is_issuing_3DS2_transaction())
			{

				if (0 == strncmp(Auth_Tx.TLF01_details.product_codes[12].code, "081", 3))
				{
					if(0 == strncmp(Auth_Tx.TLF01_details.profile, "5", 1))
						memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, diners_3ds2.self_validation_pass_cavv_rc, 2);
						//memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, "01", 2);
				}
				else
				{
					if (0 == strncmp(Auth_Tx.TLF01_details.profile, "5", 1))
					{

						if (0 == strncmp(Auth_Tx.TLF01_details.response_code, "51", 2))
						{
							//DCI 3DS2 RC for insufficient funds
							memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, "01", 2);
						}
						else if (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DINERS_DE122_3D_SEC_AUTH_NOT_PERFORMED_VALUE, 2) ||
								 0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DINERS_DE122_3D_SEC_FAILED_VALUE, 2))
						{
							memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, diners_3ds2.authentication_result_code_cavv_rc, 2);
							//memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, "02", 2);
						}
						else if(0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DINERS_DE122_3D_SEC_ACQ_ATTEMPTED_VALUE, 2) ||
								0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DINERS_DE122_3D_SEC_ACQ_ATTEMPTED_ISS_ACS_NOT_AVALABLE_VALUE, 2))
						{
							memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, "06", 2);
						}
						else
						{
							memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, diners_3ds2.self_validation_cavv_rc, 2);
							//memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, "02", 2);
						}

					}
					else if (0 == strncmp(Auth_Tx.TLF01_details.profile, "6", 1))
					{
						memcpy(ncdci_HOST_MESSAGE.addlrspdata+5, "06", 2);
					}

				}

				//Store DE44 in TLF01 to retrieve CAVV result code
				memset(Auth_Tx.TLF01_details.operator_id, 0x00, sizeof(Auth_Tx.TLF01_details.operator_id));
				memcpy(Auth_Tx.TLF01_details.operator_id, &ncdci_HOST_MESSAGE.addlrspdata[5], 2);
			}
		}
		else if( true == ncdci_is_Magstrip_pos_transaction() )
		{
			len = 5;
			if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00001",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.security_response_code,"01",2)==0 ||
					strncmp(Auth_Tx.TLF01_details.security_response_code,"02",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00002",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.response_code,"96",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00018",len);
			}
			else
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   ",len);
			}
			ncdci_HOST_MESSAGE.addlrspdata[0]=0x20;
			ncdci_HOST_MESSAGE.addlrspdata[1]=0x20;
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.addl_rsp_len,temp_len,DINERS_DE44_LEN_SIZE);
		}
		else if(true == ncdci_is_ECOM_transaction() &&
				true == ncdci_is_CAVV_Key_Indicator_set_for_BDO_CAVV())
		{
			len = 7;
			if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   01",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.security_response_code,"01",2)==0 ||
					strncmp(Auth_Tx.TLF01_details.security_response_code,"02",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   02",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.response_code,"96",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   06",len);
			}
			else
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00     ",len);
			}
			ncdci_HOST_MESSAGE.addlrspdata[0]=0x20;
			ncdci_HOST_MESSAGE.addlrspdata[1]=0x20;
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.addl_rsp_len,temp_len,DINERS_DE44_LEN_SIZE);
		}
		else if(true == ncdci_is_ECOM_transaction() &&
				0 != strlen(Auth_Tx.TLF01_details.cvc_data))
		{
			len = 5;
			if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00006",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.security_response_code,"01",2)==0 ||
					strncmp(Auth_Tx.TLF01_details.security_response_code,"02",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00011",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.response_code,"96",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   ",len);
			}
			else
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   ",len);
			}
			ncdci_HOST_MESSAGE.addlrspdata[0]=0x20;
			ncdci_HOST_MESSAGE.addlrspdata[1]=0x20;
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.addl_rsp_len,temp_len,DINERS_DE44_LEN_SIZE);
		}
		else if(true == ncdci_check_Asiapay_Ecommerce_transaction() &&
				0 != strlen(Auth_Tx.TLF01_details.cvc_data))
		{
			len = 5;
			if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00006",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.security_response_code,"01",2)==0 ||
					strncmp(Auth_Tx.TLF01_details.security_response_code,"02",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00007",len);
			}
			else if(strncmp(Auth_Tx.TLF01_details.response_code,"96",2)==0)
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00018",len);
			}
			else
			{
				memcpy( ncdci_HOST_MESSAGE.addlrspdata,"00   ",len);
			}
			ncdci_HOST_MESSAGE.addlrspdata[0]=0x20;
			ncdci_HOST_MESSAGE.addlrspdata[1]=0x20;
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.addl_rsp_len,temp_len,DINERS_DE44_LEN_SIZE);
		}
	}
	else if ( *result_flag == 0 )
	{
		if((strlen(Auth_Tx.acct_id_1)> 0 ))
		{
			len=atoi(Auth_Tx.acct_id_1);
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.addl_rsp_len,temp_len,DINERS_DE44_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.addlrspdata,Auth_Tx.acct_id_1,len);
		}
	}
}
	
/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_45
 *
 *  DESCRIPTION:  This function will move the track1
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 76 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_45( pINT result_flag, pCHAR err_buf )
{
   INT   len = 0;
   CHAR  temp_len[3] = {0};

   if(  true == ncdci_is_issuing_transaction() )
   {
		if(strlen(Auth_Tx.TLF01_details.track1)>0)
		{
			len = strlen( Auth_Tx.TLF01_details.track1 );
			sprintf(temp_len,"%2d",len);
			memcpy( ncdci_HOST_MESSAGE.track1_len,
					temp_len ,
					DINERS_DE45_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.track1,
					Auth_Tx.TLF01_details.track1  ,
					len  );
		}
	}

	else if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.track1[0] != 0x00)
		{
			len = strlen( Auth_Tx.TLF01_details.track1 );
			sprintf(temp_len,"%2d",len);
			memcpy( ncdci_HOST_MESSAGE.track1_len,
					temp_len ,
					DINERS_DE45_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.track1,
					Auth_Tx.TLF01_details.track1,
					len );
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_46
 *
 *  DESCRIPTION:  This function will move the Surcharge Fee
 *                to the Host Message structure according to  DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :an 11 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message-  DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:         Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_46( pINT result_flag, pCHAR err_buf )
{
	INT   len = 0;
	CHAR  temp_len[4] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen(Auth_Tx.TLF01_details.tax_amount ) > 0)
		{
			len = strlen(Auth_Tx.TLF01_details.tax_amount );
			sprintf(temp_len, "%03d", len);

			memcpy( ncdci_HOST_MESSAGE.surcharge_fee_len,
					temp_len,
					DINERS_DE46_LEN_SIZE);

			memcpy(	ncdci_HOST_MESSAGE.surcharge_fee,
					Auth_Tx.TLF01_details.tax_amount,
					len);
		}
	}

	else if( *result_flag == 0 )
	{
		// Need to check if we have to send this filed for ACQ transaction
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_48
 *
 *  DESCRIPTION:  This function will move the additional Data-Private
 *                to the Host Message structure according to  DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message-  DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:         Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_48( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  temp_len[4] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen(Auth_Tx.TLF01_details.voice_auth_text) > 0)
		{
			len = strlen(Auth_Tx.TLF01_details.voice_auth_text);
			sprintf(temp_len, "%03d", len);

			memcpy( ncdci_HOST_MESSAGE.addl_data_len,
					temp_len,
					DINERS_DE48_LEN_SIZE);

			memcpy(	ncdci_HOST_MESSAGE.addl_data,
					Auth_Tx.TLF01_details.voice_auth_text,
					len);
		}
	}

	else if( *result_flag == 0 )
	{
		// Need to check if we have to send this filed for ACQ transaction
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_49
 *
 *  DESCRIPTION:  This function will move the Transaction Currency Code, 
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_49( pINT result_flag, pCHAR err_buf )
{

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.reconciliation_cur_code) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.tran_curr_cd,
					Auth_Tx.reconciliation_cur_code,
					DINERS_DE49_SIZE);

		}
		else if (strlen(Auth_Tx.TLF01_details.currency_code)> 0 )
		{

			memcpy( ncdci_HOST_MESSAGE.tran_curr_cd,
					Auth_Tx.TLF01_details.currency_code,
					DINERS_DE49_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if(strlen(Auth_Tx.TLF01_details.currency_code)> 0)
		{
		   	if ( false == ncdci_isalphanum(Auth_Tx.TLF01_details.currency_code) )
			{
				 *result_flag = 1;
				 sprintf( err_buf,
						 "Unable to build request msg to host. Invalid currency code in TLF01 (%s)",
						 Auth_Tx.TLF01_details.currency_code );
			}
			else
			{
				memcpy( ncdci_HOST_MESSAGE.tran_curr_cd,
					    Auth_Tx.TLF01_details.currency_code,
					    DINERS_DE49_SIZE );
			}
		}
		else
		{  
			*result_flag = 1;
			sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code (field 49) (%s)",
					  Auth_Tx.TLF01_details.currency_code );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_51
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing currency code
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_51 ( pINT result_flag, pCHAR err_buf )
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if (Auth_Tx.ch_billing_curr_code [0] != '\0')
		{
			memcpy( ncdci_HOST_MESSAGE.card_curr_cd,
					Auth_Tx.ch_billing_curr_code,
					DINERS_DE51_SIZE);
		}
	}

   else if ( *result_flag == 0 )
   {
		if (Auth_Tx.ch_billing_curr_code [0] != '\0')
		{
			memcpy( ncdci_HOST_MESSAGE.card_curr_cd,
					Auth_Tx.ch_billing_curr_code,
					DINERS_DE51_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_52
 *
 *  DESCRIPTION:  This function will move the pin block
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :b 8
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_52( pINT result_flag, pCHAR err_buf )
{
   INT  len = 0;
   CHAR tempstr[20] = {0};

   if(  true == ncdci_is_issuing_transaction() )
   {
	   if ( true == ncdci_pin_block_exists() )
	   {
		    memset( ncdci_HOST_MESSAGE.pinblock, 0x00, DINERS_DE52_SIZE );
			memcpy( ncdci_HOST_MESSAGE.pinblock,
					Auth_Tx.TLF01_details.pin_block,
					DINERS_DE52_SIZE);
	   }
   }

   else if ( *result_flag == 0 )
   {
		if ( true == ncdci_pin_block_exists() )
		{
			memset(ncdci_HOST_MESSAGE.pinblock, 0x00, DINERS_DE52_SIZE );
			memset( tempstr, 0x00, sizeof(tempstr) );

			/* Convert to Hex */
			ncdci_ascii_to_bin_with_length( Auth_Tx.TLF01_details.pin_block,
											strlen(Auth_Tx.TLF01_details.pin_block),
											&tempstr[0], sizeof(tempstr) );

		memcpy( ncdci_HOST_MESSAGE.pinblock,
				tempstr,
				DINERS_DE52_SIZE );
		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_54
 *
 *  DESCRIPTION:  This function will move the Additional Amounts
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 120 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_54 ( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	BYTE temp_len[4] = {0};
	BYTE temp_addamt[51] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		len = strlen(Auth_Tx.TLF01_details.mcard_banknet);
		if(len > 0)
		{
			sprintf(temp_len, "%03d", len);

			memcpy( ncdci_HOST_MESSAGE.AddlAmts_len,
					temp_len,
					DINERS_DE54_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.AddlAmts,
					Auth_Tx.TLF01_details.mcard_banknet ,
					len);
		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE)
		{
			/*Account Type (same as DE3 positions 3�4)*/
			strncpy( temp_addamt,
					 Auth_Tx.TLF01_details.processing_code + DINERS_DE54_ACCOUNT_TYPE_STARTING_POS,
					 DINERS_DE54_ACCOUNT_TYPE_LEN);

			/*Amount Type = 02*/
			strncat( temp_addamt,
					 DINERS_DE54_AMOUNT_TYPE,
					 DINERS_DE54_AMOUNT_TYPE_LEN);

			/*digit numeric ISO Currency Code (DE 51)*/
			strncat( temp_addamt,
					 Auth_Tx.ch_billing_curr_code,
					 DINERS_DE51_SIZE);

			/*Amount Signing positive(C)/negative(D)*/
			// Need to know how to Indentify positive(C)/negative(D)available balance
			strncat( temp_addamt,
					 DINERS_DE54_POSITIVE_AVAILABLE_BALANCE,
					 DINERS_DE54_AVAILABLLE_BALANCE_LEN);

			/*Amount*/
			strncat( temp_addamt,
					 Auth_Tx.ACF01_details.available,
					 DINERS_DE4_SIZE);

			len = strlen(temp_addamt);
			sprintf(temp_len, "%03d", len);

			memcpy( ncdci_HOST_MESSAGE.AddlAmts_len,
					temp_len,
					DINERS_DE54_LEN_SIZE);

			memcpy( ncdci_HOST_MESSAGE.AddlAmts,
					temp_addamt ,
					len);
		}
	}

	else if( *result_flag == 0)
	{
		/*Account Type (same as DE3 positions 3�4)*/
		strncpy( temp_addamt,
				 Auth_Tx.TLF01_details.processing_code + DINERS_DE54_ACCOUNT_TYPE_STARTING_POS,
				 DINERS_DE54_ACCOUNT_TYPE_LEN);

		/*Amount Type = 02*/
		strncat( temp_addamt,
				 DINERS_DE54_AMOUNT_TYPE,
				 DINERS_DE54_AMOUNT_TYPE_LEN);

		/*digit numeric ISO Currency Code*/
		strncat( temp_addamt,
				 Auth_Tx.TLF01_details.currency_code,
				 DINERS_DE49_SIZE);

		/*Amount Signing positive(C)/negative(D)*/
		// Need to know how to Indentify positive(C)/negative(D)available balance
		strncat( temp_addamt,
				 DINERS_DE54_POSITIVE_AVAILABLE_BALANCE,
				 DINERS_DE54_AVAILABLLE_BALANCE_LEN);

		/*Amount*/
		strncat( temp_addamt,
				 Auth_Tx.TLF01_details.total_amount,
				 DINERS_DE4_SIZE);

		len = strlen(temp_addamt);
		sprintf(temp_len, "%03d", len);

		memcpy( ncdci_HOST_MESSAGE.AddlAmts_len,
				temp_len,
				DINERS_DE54_LEN_SIZE);

		memcpy( ncdci_HOST_MESSAGE.AddlAmts,
				temp_addamt ,
				len);
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_55
 *
 *  DESCRIPTION:  This function will move IC Card Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans-b�255 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Phani Kumar
 *
 ******************************************************************************/

void ncdci_build_request_field_55 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	INT  i = 0;
	INT  emv_len = 0;
	CHAR temp_bcd_len[5] = {0};
	CHAR temp_ascii_len[5] = {0};
	CHAR emvlen[5] = {0};

	if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
		 (Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
	{
		return;
	}
	//ncdci_HOST_MESSAGE.IccData_len
	if(  true == ncdci_is_issuing_transaction() )// Issuing tx, Coming from ncdci
	{
		// Response TAG should populate
		if(Auth_Tx.EMV_details.future_field1[1] != 0x00 ||
		   Auth_Tx.EMV_details.future_field1[2] != 0x00)
		{
			memcpy( ncdci_HOST_MESSAGE.IccData_len,Auth_Tx.EMV_details.future_field1,3);
			strncpy(temp_ascii_len,ncdci_HOST_MESSAGE.IccData_len,3);
			len = atoi(temp_ascii_len);
			memcpy( ncdci_HOST_MESSAGE.IccData,Auth_Tx.EMV_details.future_field1+3,len);
		}
		else if (0 == strncmp(Auth_Tx.TLF01_details.response_code,
							  DINERS_ISO_APPROVED,DINERS_ISO_RESP_LEN))
		{
			strncpy(emvlen,Auth_Tx.EMV_details.future_field1,3);
			sprintf(err_buf,"Unable to build DE55 in response message, emv len : %s ",emvlen);
			ncdci_log_message( 2, 2, err_buf, "ncdci_build_request_field_55",1 );
		}
	}
	else
	{
		// Len
		emv_len = genutil_bcd_to_int( Auth_Tx.EMV_details.emv_block, 2 );
		if(emv_len == 0)
		{
			return ;
		}
		else
		{
			sprintf (ncdci_HOST_MESSAGE.IccData_len,"%03d",emv_len);
			// EMV data
			memcpy( ncdci_HOST_MESSAGE.IccData,Auth_Tx.EMV_details.emv_block+2,emv_len);
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_56
 *
 *  DESCRIPTION:  This function will move the Original Data Elements
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 33 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_56 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[3] = {0};
	BYTE orig_data[34] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if ( strlen(Auth_Tx.TLF01_details.orig_message)> 0)
		{
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, Auth_Tx.TLF01_details.orig_message);
			strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num);
			strcat (orig_data, Auth_Tx.TLF01_details.product_codes[12].amount);
			strcat (orig_data, Auth_Tx.orig_acq_inst_id);

			len = strlen(orig_data);
			sprintf(temp_len, "%02d", len);

			memcpy( ncdci_HOST_MESSAGE.orig_data_len,
					temp_len,
					DINERS_DE56_LEN_SIZE);

			memcpy (ncdci_HOST_MESSAGE.orig_data,
					orig_data,
					len);
		}
	}

	else if ( *result_flag == 0 )
	{
		if(true == ncdci_is_reversal())
		{
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, "1100");
			strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num);
			if(0    == strncmp(Auth_Tx.TLF01_details.product_codes[3].quantity,"PR",2) &&
			  '\0' != Auth_Tx.orig_local_time_date[0])
		    {
				strncat( orig_data, Auth_Tx.orig_local_time_date, 10);
		    }
			else
			{
				strcat (orig_data, Auth_Tx.TLF01_details.product_codes[12].amount);
			}
			strcat (orig_data, Ncf01_I.acquirer_inst_id_code);

			len = strlen(orig_data);
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.orig_data_len,
					temp_len,
					DINERS_DE56_LEN_SIZE);

			memcpy (ncdci_HOST_MESSAGE.orig_data,
					orig_data,
					len);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_58
 *
 *  DESCRIPTION:  This function will move the Authorizing Agent Institution Identification
 *  			  Code (IIC) to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 11
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_58( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	CHAR temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		len = strlen( Auth_Tx.TLF01_details.product_codes[6].amount );
		if( len > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.Auth_agent_IIC,
					Auth_Tx.TLF01_details.product_codes[6].amount,
					DINERS_DE58_SIZE);
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.Auth_agent_IIC_len,
					temp_len,
					DINERS_DE32_LEN_SIZE );
		}
	}
}



/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_59
 *
 *  DESCRIPTION:  This function will move the Network ID Transport Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_59( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	BYTE  temp_len[4] = {0};
    CHAR  tempstr[20] = {0};

   if(  true == ncdci_is_issuing_transaction() )
   	{
	   /* Need to see sample data and where we are storing in TLF01/AUTHTX
	   if( strlen() > 0)
	   {
		   len = strlen();
		   memcpy(ncdci_HOST_MESSAGE.NetworkID_Transport_data,
				   tempstr,
				   DINERS_DE59_LEN_SIZE);
		   len = strlen(ncdci_HOST_MESSAGE.NetworkID_Transport_data);
		   sprintf(temp_len, "%03d", len);
		   memcpy(ncdci_HOST_MESSAGE.NetworkID_Transport_data_len,
				   tempstr,
		   		   DINERS_DE59_LEN_SIZE);

	   }*/
   	}

   else if ( *result_flag == 0 )
   {
	  //Need to check with DCI where and what value theu are keeping it for ATP.
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_62
 *
 *  DESCRIPTION:  This function will move the Additional Authorization Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *
 *	DATA ELEMENT FORMAT :ans-b�999 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_62 ( pINT result_flag, pCHAR err_buf )
{	
	BYTE temp_len[4] = {0};
	INT len = 0;
	/*len = strlen(Auth_Tx.TLF01_details.invoice_number);*/
	len = strlen(Auth_Tx.TLF01_details.product_codes[19].amount);
	if(  true == ncdci_is_issuing_transaction() )
	{
		if (len > 0)
		{
			sprintf( temp_len, "%03d", len );
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data_len,temp_len, DINERS_DE62_LEN_SIZE);
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data, Auth_Tx.TLF01_details.product_codes[19].amount,len);

		}

	}
	else if ( Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
			  Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION )
	{
		sprintf( temp_len, "%03d", DINERS_DE62_TAG05_VALUE_FOR_RPS_LEN );
		memcpy (ncdci_HOST_MESSAGE.addition_auth_data_len,temp_len, DINERS_DE62_LEN_SIZE);
		if(Auth_Tx.TLF01_details.product_codes[18].quantity[0] == 'I')
		{
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data, DINERS_DE62_TAG05_VALUE_FOR_INC_PRE_AUTH, 6);
		}
		else
		{
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data, DINERS_DE62_TAG05_VALUE_FOR_PRE_AUTH, 6);
			/* Do not display text for 1st pre-auth or succeeding pre-auth after 30 days */
			memset (Auth_Tx.TLF01_details.response_text, 0x00, sizeof(Auth_Tx.TLF01_details.response_text));
		}

	}
#if BDOR_68_REQ_1_DCI
	/*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions With Pin*/
	else if(true == ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_with_pin())
	{
		sprintf( temp_len, "%03d", DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR_LEN );
		memcpy (ncdci_HOST_MESSAGE.addition_auth_data_len,temp_len, DINERS_DE62_LEN_SIZE);
		memcpy (ncdci_HOST_MESSAGE.addition_auth_data, DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR,
				DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR_LEN);
	}
	/*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions Without Pin*/
	else if(true == ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_without_pin())
	{
		sprintf( temp_len, "%03d", DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR_LEN );
		memcpy (ncdci_HOST_MESSAGE.addition_auth_data_len,temp_len, DINERS_DE62_LEN_SIZE);
		memcpy (ncdci_HOST_MESSAGE.addition_auth_data, DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR,
				DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR_LEN);
	}
#endif
	/*else if(true == ncdci_is_POS_Acq_transaction())
	{
		if(true == ncdci_check_RPS_Transaction())
		{
			sprintf( temp_len, "%03d", DINERS_DE62_TAG05_VALUE_FOR_RPS_LEN );
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data_len,temp_len, DINERS_DE62_LEN_SIZE);
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data, DINERS_DE62_TAG05_VALUE_FOR_RPS,DINERS_DE62_TAG05_VALUE_FOR_RPS_LEN);

		}
	}*/

	/*(else if ( *result_flag == 0 )
	{
		if(len > 0)
		{
			sprintf( temp_len, "%03d", len );
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data_len,temp_len, DINERS_DE62_LEN_SIZE);
			memcpy (ncdci_HOST_MESSAGE.addition_auth_data, Auth_Tx.TLF01_details.invoice_number,len);
		}
	}*/
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_64
 *
 *  DESCRIPTION:  This function will move the Message Authentication Code
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :b 8
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_64 ( pINT result_flag, pCHAR err_buf )
{
	BYTE temp_len[4] = {0};
	INT len = 0;

	if(  true == ncdci_is_issuing_transaction() )
	{
		if( strlen(Auth_Tx.TLF01_details.product_codes[0].amount) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.mess_auth_code,
					Auth_Tx.TLF01_details.product_codes[0].amount,
					DINERS_DE64_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		// We need to find out how to handle this.
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_72
 *
 *  DESCRIPTION:  This function will move the Data record
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :ans 99 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_72 ( pINT result_flag, pCHAR err_buf )
{
	BYTE temp_len[4] = {0};
	INT len = 0;

	if(  true == ncdci_is_issuing_transaction() )
	{
		// Need to know we have to handle file handling message or note
	}

	else if ( *result_flag == 0 )
	{
		// We need to find out how to handle this.
	}
}

/******************************************************************************
 *
 *  NAME:         build_request_field_92
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution
 *  			  Country Code to the Host Message structure according to DCI
 *  			  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_92( pINT result_flag, pCHAR err_buf )
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		/*Find the length*/
		if( strlen(Auth_Tx.country_code) > 0)
		{
			memcpy( ncdci_HOST_MESSAGE.ctry_code,
					Auth_Tx.country_code,
					DINERS_DE92_SIZE);
			extendedbit=true;
		}
	}

	else if ( *result_flag == 0 )
	{
		if(  true == ncdci_is_POS_Acq_transaction() )
		{
			memcpy(ncdci_HOST_MESSAGE.ctry_code,
				   Auth_Tx.MCF01_details.currency_code,
				   DINERS_DE92_SIZE);

			extendedbit=true;
		}
		else if(strlen(Ncf01_I.country_code) > 0)
		{
			if ( ncdci_isnum(Ncf01_I.country_code) )
			{
				memcpy( ncdci_HOST_MESSAGE.ctry_code,
						Ncf01_I.country_code,
						DINERS_DE92_SIZE );
				extendedbit=true;
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
					   "Cannot build request msg for host. Invalid Merchant country code: %s",
						Auth_Tx.country_code);
			}
		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
				   "Cannot build request msg for host.Merchant country code: missing ");
		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_93
 *
 *  DESCRIPTION:  This function will move the Transaction Destination Institution
 *  			  Identification Code (IIC) to the Host Message structure
 *  			  according to DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_93 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		/*Find the length*/
		if(strlen(Auth_Tx.TLF01_details.product_codes[5].amount)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.product_codes[5].amount);
			sprintf(temp_len, "%02d", len);

			memcpy( ncdci_HOST_MESSAGE.TranDestination_ICC_len,
					temp_len,
					DINERS_DE94_LEN_SIZE);

			/*Store data*/
			memcpy( ncdci_HOST_MESSAGE.TranDestination_ICC,
					Auth_Tx.TLF01_details.product_codes[5].amount,
					len);
			extendedbit=true;
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
				DINERS_NETWORK_REQ_MESSAGE,
				DINERS_MESSAGE_TYPE_SIZE))
		{
			if(strlen(Ncf01_I.receiving_inst_id_code)> 0)
			{
				memcpy( ncdci_HOST_MESSAGE.TranDestination_ICC,
						Ncf01_I.receiving_inst_id_code,
						strlen(Ncf01_I.receiving_inst_id_code));

				len = strlen(Ncf01_I.receiving_inst_id_code);
				sprintf(temp_len, "%02d", len);
				memcpy( ncdci_HOST_MESSAGE.TranDestination_ICC_len,
						temp_len,
						DINERS_DE94_LEN_SIZE);

				extendedbit=true;
			}
		}
	}

	else if( *result_flag == 0 )
	{
		if(strlen(Ncf01_I.receiving_inst_id_code)> 0)
		{
			memcpy( ncdci_HOST_MESSAGE.TranDestination_ICC,
					Ncf01_I.receiving_inst_id_code,
					strlen(Ncf01_I.receiving_inst_id_code));

			len = strlen(Ncf01_I.receiving_inst_id_code);
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.TranDestination_ICC_len,
					temp_len,
					DINERS_DE94_LEN_SIZE);

			extendedbit=true;
		}
		else
		{
			strcpy( err_buf, "Transaction Destination Institution Identification Code is Empty (field 94)");
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_94
 *
 *  DESCRIPTION:  This function will move the Transaction Originator
 *  			  Institution Identification Code (IIC) to the Host
 *  			  Message structure according to DCI host formatting
 *  			  specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_94 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		/*Find the length*/
		if(strlen(Auth_Tx.TLF01_details.product_codes[4].amount)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.product_codes[4].amount);
			sprintf(temp_len, "%02d", len);

			memcpy( ncdci_HOST_MESSAGE.TranOriginator_ICC_len,
					temp_len,
					DINERS_DE94_LEN_SIZE);

			/*Store data*/
			memcpy( ncdci_HOST_MESSAGE.TranOriginator_ICC,
					Auth_Tx.TLF01_details.product_codes[4].amount,
					len);
			extendedbit=true;
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
						DINERS_NETWORK_REQ_MESSAGE,
						DINERS_MESSAGE_TYPE_SIZE))
		{
			if(strlen(Ncf01_I.forwarding_inst_id_code)> 0)
			{
				memcpy( ncdci_HOST_MESSAGE.TranOriginator_ICC,
						Ncf01_I.forwarding_inst_id_code,
						strlen(Ncf01_I.forwarding_inst_id_code));

				len = strlen(Ncf01_I.forwarding_inst_id_code);
				sprintf(temp_len, "%02d", len);
				memcpy( ncdci_HOST_MESSAGE.TranOriginator_ICC_len,
						temp_len,
						DINERS_DE94_LEN_SIZE);

				extendedbit=true;
			}
		}
	}

	else if( *result_flag == 0 )
	{
		if(strlen(Ncf01_I.forwarding_inst_id_code)> 0)
		{
			memcpy( ncdci_HOST_MESSAGE.TranOriginator_ICC,
					Ncf01_I.forwarding_inst_id_code,
					strlen(Ncf01_I.forwarding_inst_id_code));

			len = strlen(Ncf01_I.forwarding_inst_id_code);
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.TranOriginator_ICC_len,
					temp_len,
					DINERS_DE94_LEN_SIZE);

			extendedbit=true;
		}
		else
		{
			strcpy( err_buf, "Transaction Originator Institution Identification Code is Empty (field 94)");
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_96
 *
 *  DESCRIPTION:  This function will move the Key Management Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT : LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_96 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[4] = {0};
	BYTE tem_key_data[37] = {0};
	
	if( *result_flag == 0 )
	{
		 if (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
		 {
			 if(0 == strncmp(Auth_Tx.function_code,
						DINERS_NMI_KEYTEST,
						DINERS_NETWORK_FUNCTION_CODE_LEN))
			 {
				len = strlen(Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock);
				memcpy( tem_key_data,Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock,
						strlen(Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock));

				memcpy( tem_key_data +len,Ncf01_I_key_exchange_Keyblock.iss_prev_src_station, 4);
				len = strlen(tem_key_data);
				memcpy( ncdci_HOST_MESSAGE.Key_Managment_data, tem_key_data, len);
				sprintf(temp_len, "%03d", len);
				memcpy( ncdci_HOST_MESSAGE.Key_Managment_data_len, temp_len, DINERS_DE96_LEN_SIZE);
				extendedbit=true;
			 }
		}
		 else
		 {
			 if(0 == strncmp(Auth_Tx.function_code,
							DINERS_NMI_KEYTEST,
							DINERS_NETWORK_FUNCTION_CODE_LEN))
			 {
				len = strlen(Ncf01_I_key_exchange.cwk);
				memcpy( tem_key_data,Ncf01_I_key_exchange.cwk, strlen(Ncf01_I_key_exchange.cwk));
				memcpy( tem_key_data +len,Ncf01_I_key_exchange.prev_src_station, 4);
				len = strlen(tem_key_data);
				memcpy( ncdci_HOST_MESSAGE.Key_Managment_data, tem_key_data, len);
				sprintf(temp_len, "%03d", len);
				memcpy( ncdci_HOST_MESSAGE.Key_Managment_data_len, temp_len, DINERS_DE96_LEN_SIZE);
				extendedbit=true;
			 }
		 }
	}
 }

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_100
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code (IIC) to the Host Message structure
 *  			  according to DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_100 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[3] = {0};

	if( true == ncdci_is_voice_transaction() ||
		true == ncdci_is_POS_Acq_transaction())
	{

	}

	else if(  true == ncdci_is_issuing_transaction() )
	{
		if ( strlen(Auth_Tx.forwarding_inst_id_cd) > 0 )
		{
			len = strlen( Auth_Tx.forwarding_inst_id_cd );
			sprintf( temp_len, "%02d", len );
			memcpy( ncdci_HOST_MESSAGE.recv_ins_id_code_len,
					temp_len,
					DINERS_DE33_LEN_SIZE );

			memcpy( ncdci_HOST_MESSAGE.recv_ins_id_code,
					Auth_Tx.forwarding_inst_id_cd,
					len );
			extendedbit=true;
		}
		/*Find the length*/
		else if(strlen(Auth_Tx.TLF01_details.product_codes[3].amount)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.product_codes[3].amount);
			sprintf(temp_len, "%02d", len);
			memcpy( ncdci_HOST_MESSAGE.recv_ins_id_code_len, temp_len, DINERS_DE100_SIZE_LEN);

			/*Store data*/
			memcpy(ncdci_HOST_MESSAGE.recv_ins_id_code,
			Auth_Tx.TLF01_details.product_codes[3].amount, len);
			extendedbit=true;
		}
	}

 }

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_101
 *
 *  DESCRIPTION:  This function will move the File Name
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :ans 17 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_101 ( pINT result_flag, pCHAR err_buf )
{
    INT len = 0;
	BYTE temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{

	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_106
 *
 *  DESCRIPTION:  This function will move the Transactional data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :ans-b...999,LLLVAR,BER-TLV
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_106 ( pINT result_flag, pCHAR err_buf )
{
    INT len = 0;
	BYTE temp_len[3] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.product_codes[13].amount)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.product_codes[13].amount);
			sprintf(temp_len, "%03d", len);
			memcpy( ncdci_HOST_MESSAGE.tran_data_len, temp_len, DINERS_DE106_SIZE_LEN);

					/*Store data*/
			memcpy(ncdci_HOST_MESSAGE.tran_data,
			Auth_Tx.TLF01_details.product_codes[13].amount, len);

		}

	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_122
 *
 *  DESCRIPTION:  This function will move the Additional Authentication Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :LLLVAR ans 999
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_122 ( pINT result_flag, pCHAR err_buf )
{
    INT len = 0;
	BYTE temp_len[4] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_123
 *
 *  DESCRIPTION:  This function will move the Network Reference Id (NRID)
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :LLLVAR n 15
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_123 ( pINT result_flag, pCHAR err_buf )
{
    INT len = 0;
	BYTE temp_len[4] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		len = strlen(Auth_Tx.TLF01_details.resp_source);
		if( len > 0)
		{
			/*Lets prepare for the length*/
			sprintf(temp_len, "%03d", len);
			memcpy( ncdci_HOST_MESSAGE.networ_ref_id_len,
					temp_len,
					DINERS_DE123_SIZE_LEN);

			memcpy( ncdci_HOST_MESSAGE.networ_ref_id,
					Auth_Tx.TLF01_details.resp_source,
					len);

			extendedbit = true;
		}
	}
	else if(ncdci_is_reversal())
	{
		if( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,
						 DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL,
						 DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN) ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[3].quantity, "PR", 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7))
		{
			len = strlen(Auth_Tx.TLF01_details.resp_source);
			if(len > 0)
			{
				sprintf(temp_len, "%03d", len);
				memcpy( ncdci_HOST_MESSAGE.networ_ref_id_len,
						temp_len,
						DINERS_DE123_SIZE_LEN);

				memcpy( ncdci_HOST_MESSAGE.networ_ref_id,
						Auth_Tx.TLF01_details.resp_source,
						len);
				extendedbit = true;
			}
		}
		else if( (Auth_Tx.TLF01_details.voice_auth[0] == 'Y') &&
				 (strlen(Auth_Tx.TLF01_details.auth_number) > 0))
		{
			len = strlen(Auth_Tx.TLF01_details.resp_source);
			if(len > 0)
			{
				sprintf(temp_len, "%03d", len);
				memcpy( ncdci_HOST_MESSAGE.networ_ref_id_len,
						temp_len,
						DINERS_DE123_SIZE_LEN);

				memcpy( ncdci_HOST_MESSAGE.networ_ref_id,
						Auth_Tx.TLF01_details.resp_source,
						len);
				extendedbit = true;
			}
		}
		else
		{
			// Its time out reversal (0400)
			// No Need to send DE123.
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_124
 *
 *  DESCRIPTION:  This function will move the Transaction Qualifier
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :LLLVAR ans 999
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_124 ( pINT result_flag, pCHAR err_buf )
{
    INT len = 0;
	CHAR  temp_len[4] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen (Auth_Tx.future_use_5) > 0)
		{
			/*Lets prepare for the length*/
			len = atoi(Auth_Tx.future_use_5);
			sprintf(temp_len, "%03d", len);
			memcpy( ncdci_HOST_MESSAGE.transaction_qualifer_len,
					temp_len,
					DINERS_DE125_SIZE_LEN);

			memcpy( ncdci_HOST_MESSAGE.transaction_qualifer,
					Auth_Tx.future_use_5,
					len);
			extendedbit = true;
		}
	}

	else if ( *result_flag == 0 )
	{
		// We need to check how Cadecnie response is coming from host for authorization
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_build_request_field_125
 *
 *  DESCRIPTION:  This function will move the Replacement PIN
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *	DATA ELEMENT FORMAT :LLLVAR b 8
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncdci_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_build_request_field_125 ( pINT result_flag, pCHAR err_buf )
{
    INT len = 0;
	CHAR  temp_len[4] = {0};

	if(  true == ncdci_is_issuing_transaction() )
	{
		if(strlen(Auth_Tx.TLF01_details.vehicle_number)>0)
		{
			/*Lets prepare for the length*/
			len = atoi(Auth_Tx.TLF01_details.vehicle_number);
			sprintf(temp_len, "%03d", len);
			memcpy( ncdci_HOST_MESSAGE.replacement_pin_len,
					temp_len,
					DINERS_DE125_SIZE_LEN);

			memcpy( ncdci_HOST_MESSAGE.replacement_pin,
					Auth_Tx.TLF01_details.vehicle_number,
					len);

			extendedbit = true;
		}
	}

	else if ( *result_flag == 0 )
	{
		// We need to check if DCI POS support this for OFFUS cards
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_2
 *
 *  DESCRIPTION:  This function will move the card number from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n, 19 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_2( pINT result_flag, pCHAR err_buf )
{
	INT   len = 0;
	BYTE  tem_len[3] = {0};

	/* First get the length of the card number. */
	memcpy( Auth_Tx.TLF01_details.card_num_len,	ncdci_HOST_MESSAGE.pan_len,
			DINERS_DE2_LEN_SIZE );

	memcpy( tem_len,Auth_Tx.TLF01_details.card_num_len,
			DINERS_DE2_LEN_SIZE);

	len = atoi(tem_len);
	if ( len <= DINERS_MAX_DE2_SIZE )
	{
		memcpy( Auth_Tx.TLF01_details.card_num,ncdci_HOST_MESSAGE.pan, len );
		if (false == ncdci_isnum(Auth_Tx.TLF01_details.card_num))
		{
			sprintf( err_buf, "Non-numeric pan (field 2): %s,",
					 Auth_Tx.TLF01_details.card_num );
			*result_flag = 1;
		}
	}
	else
	{
		sprintf( err_buf, "Invalid length of card number (field 2): %d,", len );
		*result_flag = 1;
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_3
 *
 *  DESCRIPTION:  This function will move the processing code from the Host
 *                Message structure according to DCI Standard host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_3( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.proccode[0] != 0x00 )
		{
			memcpy( Auth_Tx.TLF01_details.processing_code,
					ncdci_HOST_MESSAGE.proccode,
					DINERS_DE3_SIZE );

			if (false == ncdci_isnum(Auth_Tx.TLF01_details.processing_code))
			{
				sprintf( err_buf,
						 "Non-numeric processing code (field 3) %s: for card number %s,",
						  Auth_Tx.TLF01_details.processing_code,
						  Auth_Tx.TLF01_details.card_num );
						 *result_flag = 1;
			}
		}
		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_FILE_TRN_MESSAGE_TYPE,
								DINERS_FILE_TRN_MESSAGE_TYPE_LEN))
		{

		}
		else
		{
			sprintf( err_buf, "processing code is not present (field 3) for card number %s:",
					 Auth_Tx.TLF01_details.card_num);
			*result_flag = 1;
		}
	}
	if( strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 ||
		strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",3) == 0	)
	{
		// do nothing as you copied it in while sendig the request
	}
	else
	{
	/* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
		memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,ncdci_HOST_MESSAGE.proccode,DINERS_DE3_SIZE );
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_4
 *
 *  DESCRIPTION:  This function will move the amount from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_4( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.amount[0] != 0x00 )
   		{
   		    memcpy( Auth_Tx.TLF01_details.total_amount,
        	       ncdci_HOST_MESSAGE.amount, 
            	    DINERS_DE4_SIZE );
			  
		  	if (false ==(ret_val = ncdci_isnum(Auth_Tx.TLF01_details.total_amount)))
      	  	{
         			*result_flag = 1;
         		  	sprintf( err_buf,
                 	         "Non-numeric amount (field 4): %s, for card number %s",
							  Auth_Tx.TLF01_details.total_amount,
							  Auth_Tx.TLF01_details.card_num );
	      	}

   		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_6
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing amount from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_6( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0 ;

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.ch_bill_amt[0] != 0x00 )
		{
			memcpy( Auth_Tx.ch_billing_amt,
					ncdci_HOST_MESSAGE.ch_bill_amt,
					DINERS_DE6_SIZE );

			 if (false == ncdci_isnum(Auth_Tx.ch_billing_amt))
			 {
				*result_flag = 1;
				sprintf( err_buf,
						 "Non-numeric Cardholder Billing (field 6): %s,",
						  Auth_Tx.reconciliation_amount );
			 }
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_7
 *
 *  DESCRIPTION:  This function will move the transmission time from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 * DATA ELEMENT FORMAT :n 10 MMDDhhmmss
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20] = {0};
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.gmt_time[0] != 0x00 )
   		{
	       	 memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
        	    	 ncdci_HOST_MESSAGE.gmt_time,
            		 DINERS_DE7_SIZE );
			 
			 ret_val = ncdci_isnum(Auth_Tx.TLF01_details.transmission_timestamp);
         	 if ( ret_val == false )
         	 {
            	*result_flag = 1;
           		 sprintf( err_buf,
           				  "Non-numeric Transmission Timestamp (field 7): %s,",
						  Auth_Tx.TLF01_details.transmission_timestamp );
         	}
         	else if ( false == ncdci_validate_date_time(Auth_Tx.TLF01_details.transmission_timestamp))
         	{
            	ret_val = false;
            	*result_flag = 1;
            	sprintf( err_buf,
               		     "Invalid Transmission Timestamp (field 7): %s,",
                   		 Auth_Tx.TLF01_details.transmission_timestamp );
         	}
		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_11
 *
 *  DESCRIPTION:  This function will move the system trace number from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_11( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.stan[0] != 0x00 )
   		{
   			memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
            	    ncdci_HOST_MESSAGE.stan,
            	    DINERS_DE11_SIZE );
			ret_val = ncdci_isnum(Auth_Tx.TLF01_details.sys_trace_audit_num);
      		if ( ret_val == false )
      		{
        		*result_flag = 1;
         	 	sprintf( err_buf,
               			 "Non-numeric System Trace Audit Number (field 11): %s,",
               			 Auth_Tx.TLF01_details.sys_trace_audit_num );
      		}
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_12
 *
 *  DESCRIPTION:  This function will move the transaction time from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12 YYMMDDhhmmss
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  time[13] = {0};
   INT   len     = 0;
   CHAR  time_date[25] = {0};
   CHAR  date_str[9] = {0};

   if ( *result_flag == 0 )
   {
	  if(ncdci_HOST_MESSAGE.time[0] != 0x00 )
   	  {
		  /* store Local Transaction Date And Time */
	  		memcpy( Auth_Tx.TLF01_details.product_codes[12].amount,
        		    ncdci_HOST_MESSAGE.time, DINERS_DE12_SIZE);

	  		ptetime_get_timestamp( time_date );
	  		ncdci_get_date( time_date, date_str );

  			memcpy( time,Auth_Tx.TLF01_details.product_codes[12].amount,DINERS_DE12_SIZE);
  			//Store the Century details
  			memcpy( Auth_Tx.TLF01_details.date_yyyymmdd,date_str,DINERS_YY_LEN);
  			memcpy( Auth_Tx.TLF01_details.date_yyyymmdd +DINERS_YY_LEN ,
  					time,DINERS_YYMMDD_LEN);
  			memcpy( Auth_Tx.TLF01_details.time_hhmmss,
  			      	time + DINERS_HHMMSS_START_POS,DINERS_HHMMSS_LEN);
   	  		ret_val = ncdci_isnum(time);
      		if ( ret_val == false )
   			{
       			*result_flag = 1;
       			sprintf( err_buf,
               			 "Non-numeric transaction time (field 12): %s,",
               			 time );
   			}
		}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_13
 *
 *  DESCRIPTION:  This function will move the transaction date from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 YYMM
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_13( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.date[0] != 0x00 )
   		{
	    	  memcpy( Auth_Tx.TLF01_details.product_codes[15].code,
	    			  ncdci_HOST_MESSAGE.date,DINERS_DE13_SIZE);
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_14
 *
 *  DESCRIPTION:  This function will move the Expire date from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 YYMM
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma 
 *
 ******************************************************************************/
void ncdci_move_field_14( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	  if(ncdci_HOST_MESSAGE.date_expd[0] != 0x00 )
	  {
		  memcpy( Auth_Tx.TLF01_details.exp_date,
				  ncdci_HOST_MESSAGE.date_expd,
				  DINERS_DE14_SIZE);

		   ret_val = ncdci_isnum(Auth_Tx.TLF01_details.exp_date);
		   if ( ret_val == false )
      	   {
		        *result_flag = 1;
        		 sprintf( err_buf,
                		  "Non-numeric Expire date (field 14): %s,",
                  		  Auth_Tx.TLF01_details.exp_date );
      		}
	  	}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_22
 *
 *  DESCRIPTION:  This function will move the POS Entry Mode from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :an 12
 *
 *                Prepend '0' to put it into Hypercom ISO format.
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;
   CHAR  temp_posentry[13] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.posentry[0] != 0x00 )
   		{
   			memcpy (Auth_Tx.TLF01_details.product_codes[16].amount,
   					ncdci_HOST_MESSAGE.posentry, DINERS_DE22_SIZE);
   			memcpy (temp_posentry,ncdci_HOST_MESSAGE.posentry,
   					DINERS_DE22_SIZE);
			ret_val = ncdci_isalphanum(temp_posentry);
	      	if ( ret_val == false )
    		{
			     *result_flag = 1;
			      sprintf( err_buf,
                	 	  "Invalid POS Entry Mode (field 22): %s,",
						  ncdci_HOST_MESSAGE.posentry );
    		}
		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_23
 *
 *  DESCRIPTION:  This function will move the Card Sequence Number from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Kumar Verma
 *
 ******************************************************************************/
void ncdci_move_field_23( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.card_seq_num[0] != 0x00 )
   	    {
   	    	memcpy( Auth_Tx.EMV_details.pan_sequence_number , 
				    ncdci_HOST_MESSAGE.card_seq_num,
				    DINERS_DE23_SIZE );
				
			ret_val = ncdci_isnum( Auth_Tx.EMV_details.pan_sequence_number);
		    if ( ret_val == false )
	    	{
			      *result_flag = 1;
			      sprintf( err_buf,
	               	 	  "Non-numeric Card Sequence Number (field 23): %s,",
						   ncdci_HOST_MESSAGE.card_seq_num );
	      	}
   	    }
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_24
 *
 *  DESCRIPTION:  This function will move the Function Code from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_24( pINT result_flag, pCHAR err_buf )
{

   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.net_manag_info_code[0] != 0x00 )
		{
			memcpy( Auth_Tx.function_code,
					ncdci_HOST_MESSAGE.net_manag_info_code,
					DINERS_DE24_SIZE);
			strncpy(NMI_Code,Auth_Tx.function_code,DINERS_DE24_SIZE);
			memcpy (Auth_Tx.TLF01_details.product_codes[0].code,
					ncdci_HOST_MESSAGE.net_manag_info_code,
					DINERS_DE24_SIZE);
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_25
 *
 *  DESCRIPTION: This function will move the Message Reason Code
 *				from the Host.
 *
 *  DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host  specific specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	  if(ncdci_HOST_MESSAGE.pos_condcd[0] != 0x00 )
   	  {
			memcpy( Auth_Tx.TLF01_details.product_codes[5].code,
				    ncdci_HOST_MESSAGE.pos_condcd,
				    DINERS_DE25_SIZE);

		    ret_val = ncdci_isnum(Auth_Tx.TLF01_details.product_codes[5].code);
			if ( ret_val == false )
      		{
			   *result_flag = 1;
			    sprintf( err_buf,
            		     "Non-numeric Message Reason Code (field 25): %s,",
						  ncdci_HOST_MESSAGE.pos_condcd );
      		}
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_26
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Business Code from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       ncdci_Host_Message- Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_26( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.accpt_bus_cd[0] != 0x00 )
   	   {
	   	   memcpy( Auth_Tx.TLF01_details.category_code,
				   ncdci_HOST_MESSAGE.accpt_bus_cd,
				   DINERS_DE26_SIZE );
				
			ret_val = ncdci_isnum(Auth_Tx.TLF01_details.category_code);
			if ( ret_val == false )
			{
				*result_flag = 1;
				sprintf( err_buf,
					 	"Non-numeric Card Acceptor Business Code (field 26): %s,",
						 ncdci_HOST_MESSAGE.accpt_bus_cd );
			}
   	   	}
     }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_30
 *
 *  DESCRIPTION:  This function will move the Original Amount from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 * * DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_30( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.Original_amount[0] != 0x00 )
   	   {
   		   // We need to find a place where to store De 30 in TLF01
	   	   memcpy( Auth_Tx.TLF01_details.mcard_banknet,
				   ncdci_HOST_MESSAGE.Original_amount ,
				   DINERS_DE30_SIZE );

			ret_val = ncdci_isalphanum(Auth_Tx.TLF01_details.mcard_banknet);
			if ( ret_val == false )
			{
				*result_flag = 1;
				sprintf( err_buf,
					 	"Non- alpha numeric TOriginal Amount (field 30): %s,",
						 ncdci_HOST_MESSAGE.Original_amount );
			}
   	   	}
     }
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_32
 *
 *  DESCRIPTION:  This function will move the Acquiring Inst ID from the Host
 *                Message structure according to Transaction Fee Amount formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message- Transaction Fee Amount specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_32( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  len_str[3] = {0};

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.acq_inst_len[0] != 0x00 )
		{
			/* First get the length. */
			memcpy( len_str,ncdci_HOST_MESSAGE.acq_inst_len, DINERS_DE32_LEN_SIZE);
				
			len = atoi( len_str );
			if ( len <= DINERS_MAX_DE32_SIZE)
			{
				/* Now copy the Id into Auth_Tx and check it. */
				memcpy( Auth_Tx.acq_inst_id_len,ncdci_HOST_MESSAGE.acq_inst_len, DINERS_DE32_LEN_SIZE);
				memcpy( Auth_Tx.acq_inst_id_cd,ncdci_HOST_MESSAGE.acq_inst_id, len );

				if (false == ncdci_isnum(Auth_Tx.acq_inst_id_cd))
				{
					*result_flag = 1;
					sprintf( err_buf,
							 "Non-numeric acquiring inst. Id (field 32): %s,",
							 ncdci_HOST_MESSAGE.acq_inst_id );
				}
				else
				{
					/* Copy it into TLF01 for database reference. */
					memcpy( Auth_Tx.TLF01_details.acquiring_id,
							ncdci_HOST_MESSAGE.acq_inst_id,
							len );
				}
			}
			else
			{
				*result_flag = 1;
				ret_val = false;
				sprintf( err_buf, "Invalid length of acquiring inst. Id: %s,",
						len_str );
			}
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_33
 *
 *  DESCRIPTION:  This function will move the Forwarding Inst ID from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_33( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0;
   CHAR  len_str[3] = {0};

   if ( *result_flag == 0 )
   {
	  if(ncdci_HOST_MESSAGE.fwd_inst_len[0] != 0x00 )
   	  {
	      /* First get the length. */
	      memcpy( len_str,ncdci_HOST_MESSAGE.fwd_inst_len, DINERS_DE33_LEN_SIZE);
	      len = atoi( len_str );
	      if ( len <= DINERS_MAX_DE33_SIZE )
	      {
	         /* Now copy the Id into Auth_Tx and check it. */
			 memcpy( Auth_Tx.forwarding_inst_id_len,ncdci_HOST_MESSAGE.fwd_inst_len,DINERS_DE33_LEN_SIZE);
	         memcpy( Auth_Tx.forwarding_inst_id_cd,ncdci_HOST_MESSAGE.fwd_inst_id, len );
	         if (false == ncdci_isnum(Auth_Tx.forwarding_inst_id_cd))
	         {
	            *result_flag = 1;
	            sprintf( err_buf,
	                    "Non-alphanumeric acquiring inst. Id (field 33): %s,",
						ncdci_HOST_MESSAGE.fwd_inst_id );
	         }
	      }
		  else
		  {
			  *result_flag = 1;
		       sprintf( err_buf,
					  "Invalid acquiring inst. Id length (field 33): %s,",
					  len_str);
		  }
   	  	}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_35
 *
 *  DESCRIPTION:  This function will move the track2 from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 * DATA ELEMENT FORMAT :z 37 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_35( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  len_str[3] = {0};
	CHAR  service_code[4] = {0};
	CHAR  error_msg[256] = {0};

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.track2[0] != 0x00 )
		{
			/* First get the length. */
			memcpy( len_str,ncdci_HOST_MESSAGE.track2_len, DINERS_DE35_LEN_SIZE);
			len = atoi( len_str );
			if ( len <= DINERS_MAX_DE35_SIZE )
			{
				memcpy( Auth_Tx.TLF01_details.track2 ,ncdci_HOST_MESSAGE.track2, len);
				if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
				{
					// Let us get the service code.
					if((true == ncdci_get_service_code(  service_code,  error_msg )))
					{
						strncpy(Auth_Tx.TLF01_details.service_code,service_code,
								(sizeof(service_code) -1));
					}
				}
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid tracke 2 length (field 35): %s",
						 len_str);
			}
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_37
 *
 *  DESCRIPTION:  This function will move the retrieval reference number from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_37( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	  if(ncdci_HOST_MESSAGE.rrn[0] != 0x00 )
   	  {
		  memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
			 	  ncdci_HOST_MESSAGE.rrn, DINERS_DE37_SIZE );
			  
		  ret_val = ncdci_isalphanum_space(Auth_Tx.TLF01_details.retrieval_ref_num);
		  for( len = 0; len <DINERS_DE37_SIZE; len++)
		  {
				if( Auth_Tx.TLF01_details.retrieval_ref_num[len]== ' ' )
				{
					// Fill space with 0
					Auth_Tx.TLF01_details.retrieval_ref_num[len]= '0';
				}
		  }	
		  if ( ret_val == false )
		  {
			   *result_flag = 1;
		   	   sprintf( err_buf,
						"Non-alphanumeric Retrieval Refence Number (field 37): %s,",
						 ncdci_HOST_MESSAGE.rrn );
		  }
      }
   }
}
/******************************************************************************
 *
 *  NAME:         ncdci_move_field_38
 *
 *  DESCRIPTION:  This function will move the Authorization Number from Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_38( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   CHAR error_msg[256] = {0};
   CHAR temp_auth_id[7] = {0};

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.authcode[0] != 0x00 )
       {
    	  memcpy( temp_auth_id,ncdci_HOST_MESSAGE.authcode,
				  DINERS_DE38_SIZE );
			
	  		ret_val = ncdci_isalphanum(temp_auth_id);
	  		if ( ret_val == true )
			{
				memcpy( Auth_Tx.TLF01_details.auth_number,
						ncdci_HOST_MESSAGE.authcode, DINERS_DE38_SIZE );
				// Save Auth_ID coming from DCI so that we can echo the same for advice transaction
				memcpy( Auth_Tx.TLF01_details.orig_auth_number,
						ncdci_HOST_MESSAGE.authcode, DINERS_DE38_SIZE );
			}
      	}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_39
 *
 *  DESCRIPTION:  This function will move the Response Code from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *   DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_39( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len     = 0;
	CHAR dci_response_code[4] = {0};

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.responsecd[0] != 0x00 )
		{
			memcpy( dci_response_code,ncdci_HOST_MESSAGE.responsecd, DINERS_DE39_SIZE );
			// Save Action code coming from DCI so that we can echo the same for advice transaction
			memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
					ncdci_HOST_MESSAGE.responsecd, DINERS_DE39_SIZE );
			ret_val = ncdci_isnum(dci_response_code);
			if ( ret_val == false )
			{
				   sprintf( err_buf,
							"Non-numeric response code (field 39) : %s,",
							 ncdci_HOST_MESSAGE.responsecd);

			}
			match_DCI_to_ISO_response_code(dci_response_code);

		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_40
 *
 *  DESCRIPTION:  This function will move the Service Code from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_40( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.service_code[0] != 0x00 )
   		{
   			if(0 == strlen(Auth_Tx.TLF01_details.cvc_data))
   			{
   				memcpy( Auth_Tx.TLF01_details.cvc_data,
   						ncdci_HOST_MESSAGE.service_code,
						DINERS_DE40_SIZE );
   				ret_val = ncdci_isnum(Auth_Tx.TLF01_details.cvc_data);
				if ( ret_val == false )
				{
					   sprintf( err_buf,
								"Non-numeric CVV2 (field 40) : %s,",
								ncdci_HOST_MESSAGE.service_code);
				}
				else
				{
					Auth_Tx.TLF01_details.verify_mag_stripe[0] = 'Y';
				}
   			}
 		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_41
 *
 *  DESCRIPTION:  This function will move the terminal ID from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *   DATA ELEMENT FORMAT :ans 8
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_41( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.termid[0] != 0x00 )
   		{
   		    memcpy( Auth_Tx.TLF01_details.terminal_id,
              		ncdci_HOST_MESSAGE.termid, DINERS_DE41_SIZE );
 		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_42
 *
 *  DESCRIPTION:  This function will move the Merchant ID from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 15
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_42( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.cardaccid[0] != 0x00 )
   		{
	   		memcpy( Auth_Tx.TLF01_details.merchant_id,
             		ncdci_HOST_MESSAGE.cardaccid, DINERS_DE42_SIZE );
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_43
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 99 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_43( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   CHAR  temp_cardaccname[101] = {0};
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.cardaccname_len[0] != 0x00 )
   		{
			
		   /*
		        Subfield Name       format  length		length type
        		name\street\city\ 	ans 	83 			variable length
				postal code			ans 	10 			fixed length
				region 				ans 	3 			fixed length
				country code 		n 		3 			fixed length
			*/

            /* Store Name/ street /city*/
   			memcpy( temp_len,
   					ncdci_HOST_MESSAGE.cardaccname_len,
					DINERS_DE43_LEN_SIZE);

   			memcpy( temp_cardaccname,
   					ncdci_HOST_MESSAGE.cardaccname,
   					DINERS_MAX_DE43_SIZE);

			memcpy( Auth_Tx.int_net_facility_data,
					ncdci_HOST_MESSAGE.cardaccname,
					DINERS_MAX_DE43_SIZE);

   			ncdci_populate_DE43_name_street_city( &temp_cardaccname);
         	/*Store postal code*/
         	memcpy( Auth_Tx.TLF01_details.product_codes[14].amount,
					ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_POSTAL_CODE_POSTION,
					DINERS_DE43_POSTAL_CODE_LEN );

         	/*store region*/
         	memcpy( Auth_Tx.TLF01_details.product_codes[14].code,
				    ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_REGION_POSTION,
					DINERS_DE43_REGION_LEN );

         	/*store country code*/
            memcpy( Auth_Tx.country_code,
					ncdci_HOST_MESSAGE.cardaccname + DINERS_DE43_COUNTRY_CODE_POSTION,
					DINERS_DE43_COUNTRY_CODE_LEN );

   		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_44
 *
 *  DESCRIPTION:  This function will move the Additional Response Data from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 22 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_44( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {  
	    if(ncdci_HOST_MESSAGE.addlrspdata[0] != 0x00 )
		{
	    	/* 1st find the length */
			memcpy( temp_len,
					ncdci_HOST_MESSAGE.addl_rsp_len,
					DINERS_DE44_LEN_SIZE);
			len = atoi (temp_len);
			
			if(len <= DINERS_MAX_DE44_SIZE) // validate data length
			{
				/* Store Length in 1st 2 byte and additional response data into host message */
				memcpy( Auth_Tx.acct_id_1 ,
						ncdci_HOST_MESSAGE.addl_rsp_len,
						DINERS_DE44_LEN_SIZE);
				memcpy( Auth_Tx.acct_id_1 +2 ,
						ncdci_HOST_MESSAGE.addlrspdata,
						len);// Store data after length
				//Store for Tranlookup & sbatch
				if(len>12)
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[18].amount, ncdci_HOST_MESSAGE.addlrspdata, 12);
					memcpy(Auth_Tx.TLF01_details.product_codes[1].amount, ncdci_HOST_MESSAGE.addlrspdata+12,len-12);
				}
				else
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[18].amount, ncdci_HOST_MESSAGE.addlrspdata, 12);
				}
				strncpy(Auth_Tx.TLF01_details.operator_id, &ncdci_HOST_MESSAGE.addlrspdata[5], 2);

			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Additional Response Data length (field 44) : %s",
						 temp_len);
			}
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_45
 *
 *  DESCRIPTION:  This function will move the Track 1 Data from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 76 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_45( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};
   CHAR service_code[4] = {0};
   CHAR error_msg[256] = {0};


   if ( *result_flag == 0 )
   {  
	    if(ncdci_HOST_MESSAGE.track1[0] != 0x00 )
		{
	    	/* 1st find the length */
			memcpy(temp_len,ncdci_HOST_MESSAGE.track1_len,DINERS_DE45_LEN_SIZE);
			len = atoi (temp_len);
			
			if(len <= DINERS_MAX_DE45_SIZE) // validate data length
			{
				memcpy( Auth_Tx.TLF01_details.track1 ,ncdci_HOST_MESSAGE.track1,len);
				if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
				{
					// Let us get the service code.
					if((true == ncdci_get_service_code(  service_code,  error_msg )))
					{
						strncpy(Auth_Tx.TLF01_details.service_code,service_code,(sizeof(service_code) -1));
					}
				}
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Track data 1 length (field 45) : %s,",
						 temp_len);
			}
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_46
 *
 *  DESCRIPTION:  This function will move the Surcharge Fee from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 * DATA ELEMENT FORMAT :an 11 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_46( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.surcharge_fee_len[0] != 0x00 )
		{
			/* 1st find the length */
			memcpy(temp_len,ncdci_HOST_MESSAGE.surcharge_fee_len,DINERS_DE46_LEN_SIZE);
			len = atoi(temp_len);

			if( len <= DINERS_MAX_DE48_SIZE) // validate data length
			{
				memcpy( Auth_Tx.TLF01_details.tax_amount ,
						ncdci_HOST_MESSAGE.surcharge_fee,
						len);
				ret_val = ncdci_isalphanum(Auth_Tx.TLF01_details.tax_amount);
				if ( ret_val == false )
				{
					  *result_flag = 1;
					  sprintf( err_buf,
							   "Non-ASCII numeric Surcharge Fee (field 46): %s,",
							   ncdci_HOST_MESSAGE.surcharge_fee );
				}
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Additional Data-Private length (field 46) : %s",
						 temp_len);
			}
		}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_48
 *
 *  DESCRIPTION:  This function will move the Additional Data-Private from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_48( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.addl_data_len[0] != 0x00 )
	   {
			/* 1st find the length */
			memcpy( temp_len,
					ncdci_HOST_MESSAGE.addl_data_len,
					DINERS_DE48_LEN_SIZE);
			len = atoi(temp_len);
			
			if( len <= DINERS_MAX_DE48_SIZE) // validate data length
			{
				if(strlen(ncdci_HOST_MESSAGE.addl_data) < (sizeof(Auth_Tx.TLF01_details.voice_auth_text) -1))
				{
					/* Store additional response data into host message */
					memcpy( Auth_Tx.TLF01_details.voice_auth_text,
							ncdci_HOST_MESSAGE.addl_data,len);
				}
				else
				{
					/* Store additional response data into host message */
					memcpy( Auth_Tx.TLF01_details.voice_auth_text ,
							ncdci_HOST_MESSAGE.addl_data,
							(sizeof(Auth_Tx.TLF01_details.voice_auth_text) -1));
				}
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Additional Data-Private length (field 44) : %s",
						 temp_len);
			}
		}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_49
 *
 *  DESCRIPTION:  This function will move the Transaction currency code from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_49( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.tran_curr_cd[0] != 0x00 )
   		{
	   		memcpy( Auth_Tx.TLF01_details.currency_code,
            	    ncdci_HOST_MESSAGE.tran_curr_cd,
            	    DINERS_DE49_SIZE );

	   		/* DCI req 2 Oct Mndt 2022 */
			if(0 != strncmp(Auth_Tx.TLF01_details.currency_code, DINERS_USA_CURRENCY_CODE, DINERS_DE49_SIZE) &&
			   0 != strncmp(Auth_Tx.TLF01_details.currency_code, DINERS_PESO_CURRENCY_CODE, DINERS_DE49_SIZE))
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[13].code, Auth_Tx.TLF01_details.currency_code, DINERS_DE49_SIZE);
			}
      	}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_51
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing currency code from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_51( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.card_curr_cd[0] != 0x00 )
   		{
   			memcpy( Auth_Tx.ch_billing_curr_code, 
					ncdci_HOST_MESSAGE.card_curr_cd, 
				    DINERS_DE51_SIZE);

   			if(0 != strncmp(Auth_Tx.TLF01_details.currency_code, DINERS_USA_CURRENCY_CODE, DINERS_DE49_SIZE) &&
			   0 != strncmp(Auth_Tx.TLF01_details.currency_code, DINERS_PESO_CURRENCY_CODE, DINERS_DE49_SIZE))
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[7].code, Auth_Tx.ch_billing_curr_code, DINERS_DE51_SIZE);
				strncpy(Auth_Tx.TLF01_details.product_codes[7].amount, Auth_Tx.ch_billing_amt, DINERS_DE6_SIZE);
			}
   		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_52
 *
 *  DESCRIPTION:  This function will move the PIN Data from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :b 8
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_52( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.pinblock[0] != 0x00 && ncdci_HOST_MESSAGE.pinblock[1] != 0x00)
   		{
			memcpy( Auth_Tx.TLF01_details.pin_block, 
					ncdci_HOST_MESSAGE.pinblock, 
				    DINERS_DE52_SIZE);
   		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_54
 *
 *  DESCRIPTION:  This function will move the Additional Amount Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 120 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_54( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
		memcpy(temp_len,ncdci_HOST_MESSAGE.AddlAmts_len,DINERS_DE54_LEN_SIZE);
		len = atoi(temp_len);

		if( len <= DINERS_MAX_DE54_SIZE)
		{
			if(strlen(ncdci_HOST_MESSAGE.AddlAmts) < (sizeof(Auth_Tx.TLF01_details.mcard_banknet) -1))
			{
				memcpy( Auth_Tx.TLF01_details.mcard_banknet ,
						ncdci_HOST_MESSAGE.AddlAmts,len);// Store data after length
			}
			else
			{
				memcpy( Auth_Tx.TLF01_details.mcard_banknet  ,
						ncdci_HOST_MESSAGE.AddlAmts,
						(sizeof(Auth_Tx.TLF01_details.mcard_banknet) -1));
			}
		}
		else
		{
			*result_flag = 1;
			sprintf( err_buf,
					 "Invalid Additional Amount length (field 54) : %s",
					 temp_len);
		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_55
 *
 *  DESCRIPTION:  This function will move the IC Card Data from the Host
 *                Message structure according to Equitable formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans-b�255 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message- Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/

void ncdci_move_field_55( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   emvLen = 0;
	INT   temp_int = 0;
	INT   tag_91_len = 0;
	INT   movelen = 0;
	BYTE  temp_string_len[5] = {0};
	BYTE  temp_emv_len[5] = {0};
	BYTE  temp_tag_len[3] = {0};
	BYTE  ICCdata[265] = {0};
	BYTE  ascii_len[5] = {0};
	BYTE  bcd_len[3] = {0};

	if ( *result_flag == 0 )
	{
		strncpy(temp_emv_len,ncdci_HOST_MESSAGE.IccData_len,3);
		emvLen = atoi(temp_emv_len);
		if(emvLen > 0 && emvLen < sizeof (ncdci_HOST_MESSAGE.IccData))
		{
			// Request Msg
			if(Auth_Tx.TLF01_details.tx_key < 100)
			{
				//Copy raw data into EMV RAW
				memcpy(Auth_Tx.EMV_details.emv_block,ncdci_HOST_MESSAGE.IccData_len,emvLen+3); // First 3 byte ACii len + EMV data in TLV format

				memset(ICCdata, 0, sizeof(ICCdata));
				memcpy(ICCdata,ncdci_HOST_MESSAGE.IccData, emvLen);
				genutil_EMV_To_Auth_Tx(ICCdata,emvLen ,&Auth_Tx );
				//Check if we have received TAG 72 or 9F5B
				ncdci_genutil_EMV_To_Auth_Tx_ncdci_specific(ICCdata,emvLen ,&Auth_Tx);

			}
			// resposne MSG
			else
			{
				// Parse response TAGs only
				if(emvLen >0 && emvLen < 80)
				{
					sprintf(ascii_len,"%04d",emvLen);
					ncdci_asc_to_bcd (ascii_len, 4, bcd_len);
					memcpy(Auth_Tx.EMV_details.future_field1,bcd_len,2); // First 3 byte ACii len + EMV data in TLV format
					// TAG len includes First byte length and follwed by IAD daata 8 byte ARPC and 2 byte RC
					memcpy(&Auth_Tx.EMV_details.future_field1[2],&ncdci_HOST_MESSAGE.IccData,emvLen);

					if ( emvLen > (sizeof(Auth_Tx.EMV_details.future_field1)-3) )
					{
						/* Append the EMV data to future field 2. */
						memcpy( Auth_Tx.EMV_details.future_field2,
								ncdci_HOST_MESSAGE.IccData +
								(sizeof(Auth_Tx.EMV_details.future_field1)-3),
								 sizeof(Auth_Tx.EMV_details.future_field2)-1 );
					}
				}
			}
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_56
 *
 *  DESCRIPTION:  This function will move the  Original Data Elements from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :n 33 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_56( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE tempstr[33] = {0};
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
   		/*
		 +-------------------------------------------------+
		 |     Feild 56 details							   |
		 | BYTES  1- 4:  Original Message Type             |
		 | BYTES  5-10:  STAN of Original message          |
		 | BYTES 11-20:  Original date time DE12           |
		 | BYTES 21-31:  Acquiring Institution Code        |
		 +-------------------------------------------------+
		 */
	     if(ncdci_HOST_MESSAGE.orig_data_len[0] != 0x00 )
   		 {
   			len = memcpy( temp_len ,ncdci_HOST_MESSAGE.orig_data_len,
   						  DINERS_DE56_LEN_SIZE);
   			len = atoi(temp_len);
   		    memcpy (tempstr ,ncdci_HOST_MESSAGE.orig_data,len);
			memcpy( Auth_Tx.TLF01_details.orig_message,
					tempstr,
					DINERS_MESSAGE_TYPE_SIZE);

			memcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
					tempstr + DINERS_MESSAGE_TYPE_SIZE,
					DINERS_DE11_SIZE);

			memcpy( Auth_Tx.TLF01_details.product_codes[12].amount,
					tempstr + DINERS_MESSAGE_TYPE_SIZE + DINERS_DE11_SIZE,
					DINERS_DE12_SIZE);

			memcpy( Auth_Tx.orig_acq_inst_id,
					tempstr+ DINERS_MESSAGE_TYPE_SIZE + DINERS_DE11_SIZE + DINERS_DE12_SIZE,
					DINERS_MAX_DE32_SIZE);
		}
   }
}

/******************************************************************************
*
*  NAME:         ncdci_move_field_58
*
*  DESCRIPTION:  This function will move the Authorizing Agent Institution Identification
 *  			  Code (IIC) to the Host Message structure according to  DCI  host formatting
*                specifications.
*
*  DATA ELEMENT FORMAT :n 11
*
*  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
*                result_flag  - If 1, an error occurred else where
*
*  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
*                Auth_Tx     - (Global)Structure populated from Host Message
*                result_flag - Set to 1 if an error occurs
*
*  RTRN VALUE:   NONE
*
*  AUTHOR:       Abhishek Verma
*
******************************************************************************/
void ncdci_move_field_58( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len     = 0;
	BYTE  temp_len[3] = {0};

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.Auth_agent_IIC_len[0] != 0x00 )
		{
			memcpy( temp_len,ncdci_HOST_MESSAGE.Auth_agent_IIC_len,
					DINERS_DE58_SIZE_LEN);
			len=atoi(temp_len);
			if(len <=DINERS_DE58_MAX_SIZE_LEN)
			{
				memcpy( Auth_Tx.TLF01_details.product_codes[6].amount,
						ncdci_HOST_MESSAGE.Auth_agent_IIC,
						DINERS_DE58_SIZE);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						"Invalid AUTHORISING AGENT IIC length (field 58) : %s,",
						temp_len);
			}
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_59
 *
 *  DESCRIPTION:  This function will move the  Network ID Transport Data
 *                to the Host Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_59( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.NetworkID_Transport_data_len[0] != 0x00 )
	   {
		   // Need to see a sample data
		   memcpy(temp_len,
				   ncdci_HOST_MESSAGE.NetworkID_Transport_data_len,
				   DINERS_DE59_LEN_SIZE);
		   len = atoi(temp_len);
		   if(len <=DINERS_MAX_DE59_SIZE)
		   {
			   //Need to find out a field that we will using to store this data
			   //memcpy(temp_len,ncdci_HOST_MESSAGE.NetworkID_Transport_data,len);
		   }
		   else
		   {
			   *result_flag = 1;
				sprintf( err_buf,
						 "Invalid Network ID Transport Data (field 59) : %s",
						 temp_len);
		   }
	   }
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_62
 *
 *  DESCRIPTION:  This function will move the the Additional Authorization Data
 *                to the Host Message structure according to DCI  host formatting
 *                specifications.  
 *
 *	DATA ELEMENT FORMAT :ans-b�999 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_62( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[4] = {0};
   BYTE  temp_62_data[100] = {0};
   char   buf[100] = {0};

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.addition_auth_data_len[0] != 0x00 )
	   {
		   memcpy(temp_len,ncdci_HOST_MESSAGE.addition_auth_data_len,DINERS_DE62_LEN_SIZE);
		   len = atoi(temp_len);

		   if(len <=DINERS_MAX_DE62_SIZE)
		   {

			   //Need to find out a field that we will using to store this data
			   //Store DF05 here
			   memcpy(Auth_Tx.TLF01_details.product_codes[19].amount, ncdci_HOST_MESSAGE.addition_auth_data, DINERS_DE62_TAG05_VALUE_FOR_RPS_LEN);

			   //Max len for 62 is 18 BYTES
			   //memcpy(Auth_Tx.TLF01_details.product_codes[13].code, ncdci_HOST_MESSAGE.addition_auth_data + 13, (len - 13));

			   /* strncpy(temp_62_data, ncdci_HOST_MESSAGE.addition_auth_data, len);
			   genutil_bin_to_ascii(temp_62_data, len, buf, (len*2)); */

		   }
		   else
		   {
			   *result_flag = 1;
				sprintf( err_buf,
						 "Invalid Network ID Transport Data (field 62) : %s",
						 temp_len);
		   }
	   }
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_64
 *
 *  DESCRIPTION:  This function will move the Message Authentication Code from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 * 	DATA ELEMENT FORMAT :b 8
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_64( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.mess_auth_code[0] != 0x00 )
   		{
			memcpy( Auth_Tx.TLF01_details.product_codes[0].amount,
					ncdci_HOST_MESSAGE.mess_auth_code,
					DINERS_DE64_SIZE);
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_72
 *
 *  DESCRIPTION:  This function will move the Data Record the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 * 	DATA ELEMENT FORMAT :ans 99 LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_72( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.data_record_len[0] != 0x00 )
	   {
		   memcpy(temp_len,ncdci_HOST_MESSAGE.data_record_len,DINERS_DE72_LEN_SIZE);
		   len = atoi(temp_len);
		   if(len <=DINERS_MAX_DE72_SIZE)
		   {
			   //Need to find out a field that we will using to store this data

		   }
		   else
		   {
			   *result_flag = 1;
				sprintf( err_buf,
						 "Invalid data record length (field 72) : %c %c %c,",
						 temp_len);
		   }
	   }
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_92
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncdci_Host_Message- DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_92( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	   if(ncdci_HOST_MESSAGE.ctry_code[0] != 0x00 )
   	   {
	   		memcpy( Auth_Tx.country_code,
	   				ncdci_HOST_MESSAGE.ctry_code,
            	    DINERS_DE92_SIZE );

			ret_val = ncdci_isnum(Auth_Tx.country_code);
      		if ( ret_val == false )
      		{
		      	*result_flag = 1;
        		 sprintf( err_buf,
               			  "Non-numeric Transaction Originator Institution country code (field 92): %s,",
               		 	  Auth_Tx.country_code);
      		}
		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_93
 *
 *  DESCRIPTION:  This function will move the Transaction Destination Institution
 *  			  Identification Code from the Host  Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_93( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.TranDestination_ICC_len[0] != 0x00 )
   		{
   			memcpy(temp_len,ncdci_HOST_MESSAGE.TranDestination_ICC_len,DINERS_DE93_LEN_SIZE);
   			len = atoi(temp_len);
   			if(len <=DINERS_MAX_DE93_SIZE)
   			{
   				/*Store data in  Auth_Tx.TLF01_details.product_codes[5].amount */
				 memcpy( Auth_Tx.TLF01_details.product_codes[5].amount,
						 ncdci_HOST_MESSAGE.TranDestination_ICC,
						 len);
			}
   			else
   			{
				 *result_flag = 1;
				 sprintf( err_buf,
						  "Invalid Transaction Destination IIC length (field 93) : %s",
						  temp_len);
   			}
		}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_94
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution
 *  			  Identification Code from the Host Message structure according
 *  			   to  DCI  host formatting specifications.
 *
 *DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_94( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.TranOriginator_ICC_len[0] != 0x00 )
   		{
   			memcpy(temp_len,ncdci_HOST_MESSAGE.TranOriginator_ICC_len,DINERS_DE94_LEN_SIZE);
   			len = atoi(temp_len);
   			if(len <=DINERS_MAX_DE94_SIZE)
   			{
   				/*Store data in  Auth_Tx.TLF01_details.product_codes[4].amount */
				 memcpy( Auth_Tx.TLF01_details.product_codes[4].amount,
						 ncdci_HOST_MESSAGE.TranOriginator_ICC,
						 len);
			}
   			else
   			{
				 *result_flag = 1;
				 sprintf( err_buf,
						 "Invalid Transaction Destination IIC length (field 93) : %s",
						 temp_len);
   			}
		}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_96
 *
 *  DESCRIPTION:  This function will move the Key Management Data
 *  			  from the Host  Message structure according to
 *  			  DCI  host formatting  specifications.
 *
 *	DATA ELEMENT FORMAT : LLLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_96( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[4] = {0};
   char buffer[37] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.Key_Managment_data_len[0] != 0x00 )
   		{
	    	if ( keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
	    	{
	    		memcpy(temp_len,ncdci_HOST_MESSAGE.Key_Managment_data_len,DINERS_DE96_LEN_SIZE);
				len = atoi(temp_len);
				if(len <=DINERS_MAX_DE96_SIZE)
				{
					memcpy(	buffer,ncdci_HOST_MESSAGE.Key_Managment_data,len);
					memcpy(	Ncf01_Keyblock.iss_rtdcwk_keyblock,
							buffer,DINERS_DE96_KEY_LEN);

					memcpy(	Ncf01_Keyblock.iss_rtdcwk_keyblock,
							buffer+DINERS_DE96_KEY_LEN ,
							DINERS_DE96_KEY_CHECK_LEN);

					memcpy(	static_Rtdcwk_Key_Keyblock,buffer,DINERS_DE96_KEY_LEN);
					memcpy( key_check_value,
							buffer+DINERS_DE96_KEY_LEN ,
							DINERS_DE96_KEY_CHECK_LEN);
				}
				else
				{
					*result_flag = 1;
					sprintf( err_buf,
							"Invalid Key Management Data length (field 96) : %s",
							temp_len);
				}
	    	}

	    	else
	    	{
	    		memcpy(temp_len,ncdci_HOST_MESSAGE.Key_Managment_data_len,DINERS_DE96_LEN_SIZE);
	    		len = atoi(temp_len);
	    		if(len <=DINERS_MAX_DE96_SIZE)
	    		{
	    			memcpy(	buffer,ncdci_HOST_MESSAGE.Key_Managment_data,len);
	    			memcpy(	Ncf01_I.retired_cwk,buffer,DINERS_DE96_KEY_LEN);
	    			memcpy(	Ncf01_I.retired_cwk,
	    					buffer+DINERS_DE96_KEY_LEN ,
							DINERS_DE96_KEY_CHECK_LEN);

	    			memcpy(	static_Rtdcwk_Key,buffer,DINERS_DE96_KEY_LEN);
	    			memcpy( key_check_value,
	    					buffer+DINERS_DE96_KEY_LEN ,
							DINERS_DE96_KEY_CHECK_LEN);
	    		}
	    		else
	    		{
	    			*result_flag = 1;
	    			sprintf( err_buf,
	    					"Invalid Key Management Data length (field 96) : %s",
							temp_len);
	    		}
	    	}
		}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_100
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_100( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.recv_ins_id_code_len[0] != 0x00 )
   		{
   			memcpy(temp_len,ncdci_HOST_MESSAGE.recv_ins_id_code_len,DINERS_DE100_SIZE_LEN);
   			len=atoi(temp_len);
			if(len <=DINERS_MAX_DE100_SIZE)
			{
				 /*Store data in  Auth_Tx.TLF01_details.product_codes[3].amount */
	  			 memcpy( Auth_Tx.TLF01_details.product_codes[3].amount,
				 		ncdci_HOST_MESSAGE.recv_ins_id_code,
				 		 len);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						"Invalid Receiving Institution Identification Code length (field 100) : %s,",
						temp_len);
			}
   		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_101
 *
 *  DESCRIPTION:  This function will move the File Name 1 from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *	DATA ELEMENT FORMAT :ans 17 LLVAR
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_101( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.file_name_len[0] != 0x00 )
		{
			memcpy(temp_len,ncdci_HOST_MESSAGE.file_name_len,DINERS_DE101_SIZE_LEN);
			len=atoi(temp_len);
			if(len <=DINERS_MAX_DE101_SIZE)
			{

				 /* Need to identify where to store this data */
				// memcpy( Auth_Tx.TLF01_details.product_codes[3].amount,ncdci_HOST_MESSAGE.file_name_len,len);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid file name length (field 101) : %s",
						 temp_len);
			}
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_106
 *
 *  DESCRIPTION:  This function will move the Transaction data from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *	DATA ELEMENT FORMAT :ans-b...999,LLLVAR,BER-TLV
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_106( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
	    if(ncdci_HOST_MESSAGE.tran_data_len[0] != 0x00 )
		{
			// We are not stroing in TLF01 , we need to prepare seperate table to store them

			memcpy(temp_len,ncdci_HOST_MESSAGE.tran_data_len,DINERS_DE106_SIZE_LEN);
			len=atoi(temp_len);
			if(len <=DINERS_MAX_DE106_SIZE)
			{
				 /* Need to identify where to store this data */
				 //memcpy( Auth_Tx.TLF01_details.product_codes[14].quantity,ncdci_HOST_MESSAGE.tran_data_len,DINERS_DE106_SIZE_LEN);
				 //Store Tag 67 here
	  			 memcpy( Auth_Tx.TLF01_details.product_codes[13].amount,
				 		ncdci_HOST_MESSAGE.tran_data,
				 		 len);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid tran data length (field 106) : %s",
						 temp_len);
			}

		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_122
 *
 *  DESCRIPTION:  This function will move the Additional Authentication Data the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *	DATA ELEMENT FORMAT :LLLVAR ans 999
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_122( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   CHAR  temp_len[4] = {0};

   if ( *result_flag == 0 )
     {
	    if(ncdci_HOST_MESSAGE.Add_authentication_data_len[0] != 0x00 )
  		{
  			memcpy(temp_len,ncdci_HOST_MESSAGE.Add_authentication_data_len,DINERS_DE122_SIZE_LEN);
  			len=atoi(temp_len);
  			if(len <DINERS_MAX_DE122_SIZE)
  			{

  				memcpy( Auth_Tx.future_use_4,
  						ncdci_HOST_MESSAGE.Add_authentication_data,
						len);
  	  			//Store ECI value in profile
  				memset (Auth_Tx.TLF01_details.profile, 0x00, sizeof(Auth_Tx.TLF01_details.profile));
  				memcpy(Auth_Tx.TLF01_details.profile, ncdci_HOST_MESSAGE.Add_authentication_data+1, 1);

  	  			//Store all of DE122 value in below to show in tranlookup
  	  			memset(Auth_Tx.TLF01_details.dcf01_retired_cwk, 0x00, sizeof(Auth_Tx.TLF01_details.dcf01_retired_cwk));
  	  			//memset(Auth_Tx.TLF01_details.operator_id, 0x00, sizeof(Auth_Tx.TLF01_details.operator_id));
  	  			//memset(Auth_Tx.TLF01_details.workstation, 0x00, sizeof(Auth_Tx.TLF01_details.workstation));

  	  			memcpy(Auth_Tx.TLF01_details.dcf01_retired_cwk, ncdci_HOST_MESSAGE.Add_authentication_data, len);
  	  			//memcpy(Auth_Tx.TLF01_details.operator_id, ncdci_HOST_MESSAGE.Add_authentication_data+48, 20);
  	  			//memcpy(Auth_Tx.TLF01_details.workstation, ncdci_HOST_MESSAGE.Add_authentication_data+68, 16);
  			}
  			else
  			{
  				*result_flag = 1;
  				sprintf( err_buf,
  						 "Invalid Additional Authentication Data length (field 122) : %s",
						 temp_len);
  			}
  		}
  	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_123
 *
 *  DESCRIPTION:  This function will move the Network Reference Id (NRID) from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *	DATA ELEMENT FORMAT :LLLVAR n 15
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_123( pINT result_flag, pCHAR err_buf )
{
    INT   len     = 0;
    CHAR  temp_len[4] = {0};

    if ( *result_flag == 0 )
	{
    	if(ncdci_HOST_MESSAGE.networ_ref_id_len[0] != 0x00 )
		{
			memcpy(temp_len,ncdci_HOST_MESSAGE.networ_ref_id_len,DINERS_DE123_SIZE_LEN);
			len=atoi(temp_len);
			if(len <=DINERS_MAX_DE123_SIZE)
			{
				memcpy( Auth_Tx.TLF01_details.resp_source,
						ncdci_HOST_MESSAGE.networ_ref_id,len);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Network Reference Id length (field 123) : %s",
						 temp_len);
			}
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncdci_move_field_124
 *
 *  DESCRIPTION:  This function will move theIssuer Transaction Qualifier the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 * 	DATA ELEMENT FORMAT :LLLVAR ans 999
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_124( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	CHAR  temp_len[4] = {0};

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.transaction_qualifer_len[0] != 0x00 )
		{
			memcpy(temp_len,ncdci_HOST_MESSAGE.transaction_qualifer_len,DINERS_DE124_SIZE_LEN);
			len=atoi(temp_len);
			if(len <=DINERS_MAX_DE124_SIZE)
			{
				memcpy(	Auth_Tx.future_use_5,
						ncdci_HOST_MESSAGE.transaction_qualifer,
						(sizeof(Auth_Tx.future_use_5)-1));
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Transaction Qualifier length (field 124) : %s",
						 temp_len);
			}
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncdci_move_field_125
 *
 *  DESCRIPTION:  This function will move the Replacement PIN  the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :LLLVAR b 8
 *
 *  INPUTS:       ncdci_Host_Message-  DCI  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_move_field_125( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	CHAR  temp_len[4] = {0};

	if ( *result_flag == 0 )
	{
		if(ncdci_HOST_MESSAGE.replacement_pin_len[0] != 0x00 )
		{
			memcpy(temp_len,ncdci_HOST_MESSAGE.replacement_pin_len,DINERS_DE125_SIZE_LEN);
			len=atoi(temp_len);
			if(len <=DINERS_MAX_DE125_SIZE)
			{
				/* Store additional response data into host message ,
				 *  In Build function we will calculate the length from this filed*/
				 memcpy( Auth_Tx.TLF01_details.vehicle_number,ncdci_HOST_MESSAGE.replacement_pin,len);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Replacement PIN data length (field 125) : %s",
						 temp_len);
			}
		}
	}
}
