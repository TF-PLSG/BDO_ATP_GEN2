
/***************************************************************************
*
* Copyright (c) 2004, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      nccup2_bitfields.c
*  
* TITLE:       CUP Network Control Data Field Processor
*  
* DESCRIPTION: This module contains functions
*              to build or parse CUP host data fields.
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
#include "cup.h"
#include "nccup2_prototypes.h"
#include "nccup2_bitfields.h"



/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     TranFlow;
extern CHAR    ServiceName[];
       INT    NetworkType;
extern INT TIDctr=719; 
CHAR Log_Bufr[516]={0};
/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;


/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

extern const struct isobit BITTAB[128 + 1];

/*Flags*/
extern INT extendedbit;


/******************************************************************************
 *
 *  NAME:         build_request_field_2
 *
 *  DESCRIPTION:  This function will move the card number to the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits.  While the card number is
 *                variable length, this field is always the same length.
 *                Need to left justify and zero fill the field.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message -  BDO Cadencie host specific structure (Global)
 *
 *  RETURN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_2( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		len = strlen( Auth_Tx.TLF01_details.card_num );
		if(len > 0)
		{
			sprintf( HOST_MESSAGE.pan_len, "%02d", len  );
			memset( HOST_MESSAGE.pan, '0', CUP_MAX_DE2_SIZE );
			memcpy( HOST_MESSAGE.pan, Auth_Tx.TLF01_details.card_num, len );
		}
	}
	else if ( *result_flag == 0 )
	{
		len = strlen( Auth_Tx.TLF01_details.card_num );
		if ( len == 0 )
		{
			ret_val = false;
			*result_flag = 1;
			strcpy( err_buf,
			"Unable to build request message to host. No card number." );
		}
		else
		{
			if ( isalphanum(Auth_Tx.TLF01_details.card_num) )
			{
				sprintf( HOST_MESSAGE.pan_len, "%02d", len  );
				memset( HOST_MESSAGE.pan, '0', CUP_MAX_DE2_SIZE );
				memcpy( HOST_MESSAGE.pan, Auth_Tx.TLF01_details.card_num, len );
			}
			else
			{
				ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
				"Unable to build request msg to host. Invalid card number: %s",
				Auth_Tx.TLF01_details.card_num );
			}
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_3
 *
 *  DESCRIPTION:  This function will move the processing code to the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Processing code is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf      - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_3( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	BYTE proc_code[7]="200000"; //For void sele going to CUP
	INT proc_code_len = 0;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.processing_code)>0)
		{
			memcpy( HOST_MESSAGE.proccode, 
			Auth_Tx.TLF01_details.processing_code, 
			CUP_DE3_SIZE );
		}
	}
	
	else if ( *result_flag == 0 )
	{
		if( (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE))
		{
			// We need to change the processing code coming from POS and send as 200000.
			memcpy( HOST_MESSAGE.proccode, proc_code,CUP_DE3_SIZE );
		}
		else if( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"02",2))&&
			   ((Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)||
			   (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE)))
		{
			// We need to change the processing code coming from POS and send as 200000 for VOID reversal.
			memcpy( HOST_MESSAGE.proccode, proc_code,CUP_DE3_SIZE );
		}
		else if( Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
				 Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION ||
				 Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION||
				 (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"03",2)))
		{
			if(0 == strncmp(Auth_Tx.TLF01_details.processing_code,CUP_PREAUTH_PROC_CODE,2))
			{
				memcpy( HOST_MESSAGE.proccode,
						Auth_Tx.TLF01_details.processing_code,
						CUP_DE3_SIZE );
			}
			else
			{
				memcpy( HOST_MESSAGE.proccode,CUP_PREAUTH_PROC_CODE,	2 );
				memcpy(HOST_MESSAGE.proccode+2,
						Auth_Tx.TLF01_details.processing_code +2 ,4);
			}

		}
		else if( strncmp(Auth_Tx.TLF01_details.processing_code,"11",2) == 0)
		{
			memcpy( HOST_MESSAGE.proccode,"000000",CUP_DE3_SIZE );
		}
		else if (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		{
			memcpy( HOST_MESSAGE.proccode, Auth_Tx.TLF01_details.processing_code,
																	CUP_DE3_SIZE );
		}
		else if ( isnum(Auth_Tx.TLF01_details.processing_code) )
		{
			memcpy( HOST_MESSAGE.proccode, 
			Auth_Tx.TLF01_details.processing_code, 
			CUP_DE3_SIZE );
		}
		else
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
			"Cannot build request msg for host. Invalid processing code: %s",
			Auth_Tx.TLF01_details.processing_code );
		}
	}
	  /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
	memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,HOST_MESSAGE.proccode,CUP_DE3_SIZE );
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_4
 *
 *  DESCRIPTION:  This function will move the amount to the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek
 *
 ******************************************************************************/
INT build_request_field_4( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE amount[13] = "";

   if ( *result_flag == 0 )
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  || 
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) )
      {
         /*+-----------------------------------------------------------------+
           | For reversals, ATM and POS are treated differently for field 4. |
           |    POS - Field 4 is always the total_amount.                    |
           |    ATM - Field 4 is dispensed amount: zero for full reversals.  |
           |                                                                 |
           |    POS -> Origin = "LO"                                         |
           |    ATM -> Origin = "RO"                                         |
           +-----------------------------------------------------------------+*/

         if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin, "RO") )
         {
            /* ATM - Amount dispensed for Partial or Full Reversal */
			if((0== strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))  )
			{
				if(strlen(Auth_Tx.TLF01_details.sales_amount) > 0)
				{
					memcpy(amount,Auth_Tx.TLF01_details.sales_amount,CUP_DE4_SIZE);
				}
				else
				{
					memcpy( amount, Auth_Tx.TLF01_details.total_amount,CUP_DE4_SIZE );
				}
			}

			else
			{
                memcpy( amount, Auth_Tx.TLF01_details.actual_amount,CUP_DE4_SIZE );
			}
         }
         else
         {
            /* POS - Amount being reversed. */
				memcpy( amount, Auth_Tx.TLF01_details.total_amount,CUP_DE4_SIZE );
		
         }

         memcpy( HOST_MESSAGE.amount, amount, CUP_DE4_SIZE );
      }
      else if ( isnum(Auth_Tx.TLF01_details.total_amount) )
      {
    	 if ( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6) &&
    		  (strlen(Auth_Tx.TLF01_details.nfi_seq_nbr) > 0) &&
       		  0 != strncmp (Auth_Tx.TLF01_details.nfi_seq_nbr, PESO,3) &&
   			  0 != strncmp (Auth_Tx.TLF01_details.nfi_seq_nbr, USD,3))
    	 {
            memcpy( HOST_MESSAGE.amount,
                     Auth_Tx.TLF01_details.refund_amount,
					  CUP_DE4_SIZE );
    	 }
    	 else if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
         {
            memcpy( HOST_MESSAGE.amount,
                    Auth_Tx.TLF01_details.total_amount,
					CUP_DE4_SIZE );
         }
         else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
                               Auth_Tx.TLF01_details.currency_code) )
         {
            memcpy( HOST_MESSAGE.amount,
                    Auth_Tx.TLF01_details.total_amount,
					CUP_DE4_SIZE );
         }
         else
         {
            memcpy(HOST_MESSAGE.amount,Auth_Tx.reconciliation_amount,CUP_DE4_SIZE);
         }
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid amount: %s",
                Auth_Tx.TLF01_details.total_amount );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_5
 *
 *  DESCRIPTION:  This function will move the Settlement amount to the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Settlement Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
INT build_request_field_5( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.settlement_total)> 0)
		{
			memcpy( HOST_MESSAGE.setl_amount,
			Auth_Tx.TLF01_details.settlement_total,
			CUP_DE5_SIZE );
		}
	}
	else  if ( *result_flag == 0 )
	{
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) ||
			0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5)	)
		{
			/* No Need to pulate DE 5 , in case of VOICE/ POS - Req from Kenneth - 23 Sep 2009 - TF PHANI*/
		}
		else
		{	
			memcpy( HOST_MESSAGE.setl_amount,
			Auth_Tx.reconciliation_amount,
			CUP_DE5_SIZE );
		}
	}

	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_6
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing amount to the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Cardholder Billing Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
INT build_request_field_6( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.ch_billing_amt)> 0)
		{
			memcpy( HOST_MESSAGE.ch_bill_amt,Auth_Tx.ch_billing_amt,CUP_DE6_SIZE );
		}
	}
	else  if ( *result_flag == 0 )
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5)
		)
		{
			/* No Need to pulate DE 5 , in case of VOICE/ POS - Req from Kenneth - 23 Sep 2009 - TF PHANI*/
		}
		else
		{
			memcpy( HOST_MESSAGE.ch_bill_amt,Auth_Tx.ch_billing_amt,CUP_DE6_SIZE );
		}
	}

	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_7
 *
 *  DESCRIPTION:  This function will move the transmission date to 
 *		     the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is expressed in local time.  Indicates the time
 *                at the message sender's last node before leaving the sender's
 *                domain.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
INT build_request_field_7( pINT result_flag, pCHAR err_buf )
{
   INT	 ret_val = true;
   CHAR  date_str [ 9] = "";
   CHAR  time_str [ 7] = "";
   CHAR  time_date[25] = "";
  
   if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
   {
	   memcpy( HOST_MESSAGE.gmt_time,
			   Auth_Tx.TLF01_details.transmission_timestamp,
			   CUP_DE7_SIZE );
   }

   else if ( *result_flag == 0 )
   {
	   if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",5) )
	   {
		   memcpy( HOST_MESSAGE.gmt_time,
				   Auth_Tx.TLF01_details.transmission_timestamp,
				   CUP_DE7_SIZE );
	   }
	   else
	   {
		   /* Need to create transmission time stamp. Get local time. */
		   memset( Auth_Tx.TLF01_details.transmission_timestamp, 0x00, CUP_DE7_SIZE );
		   memset( Auth_Tx.TLF01_details.time_hhmmss, 0x00, CUP_DE12_SIZE );
		   ptetime_get_timestamp( time_date );
		   get_date( time_date, date_str );
		   get_time( time_date, time_str );

		   /* Store MMDDhhmms format in transmission_timestamp */
		   /* Store MMDD in transmission_timestamp */
		   memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
				   date_str + CUP_MMDD_START_POS,
				   CUP_MMDD_LEN);
		   /* Store hhmmss in transmission_timestamp after storing DDMM*/
		   memcpy( Auth_Tx.TLF01_details.transmission_timestamp + 4,
				   time_str, CUP_HHMMSS_LEN);
		   /* Lets Populate DE12 as well*/
		   memcpy( Auth_Tx.TLF01_details.time_hhmmss , time_str, CUP_DE12_SIZE);

		   memcpy( HOST_MESSAGE.gmt_time,
				   Auth_Tx.TLF01_details.transmission_timestamp,
				   CUP_DE7_SIZE );
		   return( ret_val );
	  }
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_9
 *
 *  DESCRIPTION:  This function will move the Settlement Conversion Rate
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *               It is the currency conversion rate from the transaction currency 
 *               to settlement currency, which is agreed by UnionPay and the Participant. 
 *               The format is right-justified and no decimal point.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_9( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	BYTE  temp_Conversion_rate[9]="";

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.conversion_rate)> 0)
		{
			memcpy( HOST_MESSAGE.setl_conv_rate,
			Auth_Tx.TLF01_details.conversion_rate,
			CUP_DE9_SIZE );
		}
	}
	else if ( *result_flag == 0 )
	{
		if(strncmp( Auth_Tx.TLF01_details.handler_queue,"sbatchA",4)==0)
		{
			if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"LO")==0 ||
			strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0)
			{
				// Don't pass the values
			}
			else
			{
				if(0==strncmp(Auth_Tx.TLF01_details.conversion_rate,"1",strlen(Auth_Tx.TLF01_details.conversion_rate)))
				{
					// txcntl is making as 1.
					//Do nothing
				}
				else
				{
					sprintf(temp_Conversion_rate,"%08s",Auth_Tx.TLF01_details.conversion_rate);
					memcpy( HOST_MESSAGE.setl_conv_rate,temp_Conversion_rate,CUP_DE9_SIZE );
				}
				
			}
		}
		else
		{
			memcpy( HOST_MESSAGE.setl_conv_rate,
			Auth_Tx.TLF01_details.conversion_rate,
			CUP_DE9_SIZE );
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_10
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing Conversion Rate
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *              It is the conversion rate for the currency conversion from the acquirer�s
 *		   transactioncurrency to the cardholder billing currency. 
 *		  The format is right-justified, no decimalpoint.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_10( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.ch_billing_conv_rate)>0)
		{
			memcpy( HOST_MESSAGE.conv_rate_chb, 
					Auth_Tx.ch_billing_conv_rate,
					CUP_DE10_SIZE );
		}
	}
	else if ( *result_flag == 0 )
	{
		if((strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",4)==0)&&(
		(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"LO")==0))||
		(strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0))
		{
			/* Don't pass the Card holder billing conversion rate TF-Ajay 13-July-09 */
		}
		else
		{
			memcpy( HOST_MESSAGE.conv_rate_chb, 
			Auth_Tx.ch_billing_conv_rate, 
			CUP_DE10_SIZE );
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_11
 *
 *  DESCRIPTION:  This function will move the system trace audit number
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_11( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.sys_trace_audit_num)>0)
		{
			memcpy( HOST_MESSAGE.stan,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					CUP_DE11_SIZE );
		}
	}
	else if ( *result_flag == 0 )
	{
		if ( isnum(Auth_Tx.TLF01_details.sys_trace_audit_num) )
		{
				memcpy( HOST_MESSAGE.stan,
						Auth_Tx.TLF01_details.sys_trace_audit_num, 
						CUP_DE11_SIZE );
		}
		else if ( isnum(Auth_Tx.TLF01_details.orig_sys_trace_num) )
		{
			memcpy( HOST_MESSAGE.stan,
					Auth_Tx.TLF01_details.orig_sys_trace_num, 
					CUP_DE11_SIZE );
			
		}
		else 
		{
				/* For POS reversals create new STAN */
				create_stan( HOST_MESSAGE.stan );
		}				
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_12
 *
 *  DESCRIPTION:  This function will move the transaction Local time
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                The local time of the acquirer location when the transaction occurs
 * Format: HHMMSS
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_12( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	CHAR  time_date[25] = "";
	CHAR  time_str[7];

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  || 
		(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) )
		{

			/* FOR REVERSAL : create new date, time */
			/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
			ptetime_get_timestamp( time_date );

			/* Reformat to get time "hhmmss" */
			get_time( time_date, time_str );

			/* format field 12: hhmmss. */
			memcpy( HOST_MESSAGE.time,	time_str, CUP_DE12_SIZE );
		}
		else if(strlen(Auth_Tx.TLF01_details.time_hhmmss) > 0)
		{
			memcpy( HOST_MESSAGE.time,
			Auth_Tx.TLF01_details.time_hhmmss,
			CUP_DE12_SIZE );
		}
	}

	else if ( *result_flag == 0 )
	{
		if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",5) )
		{
			memcpy( HOST_MESSAGE.time,
					Auth_Tx.TLF01_details.time_hhmmss,
					CUP_DE12_SIZE );
		}
		else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  ||
				  (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) )
		{
			/* FOR REVERSAL : create new date, time */
			/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
			ptetime_get_timestamp( time_date );

			/* Reformat to get time "hhmmss" */
			get_time( time_date, time_str );

			/* format field 12: hhmmss. */
			memcpy( HOST_MESSAGE.time,	time_str, CUP_DE12_SIZE );
		}
		else
		{
			memcpy( HOST_MESSAGE.time,
					Auth_Tx.TLF01_details.time_hhmmss,
					CUP_DE12_SIZE );
		}
	}

	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_13
 *
 *  DESCRIPTION:  This function will move the Local Transaction Date
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                The local date of the acquirer location when the transaction occurs
 *        
 *   Format: MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_13( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	CHAR  time_date[25] = "";
	CHAR err_buf1[100]  ="";

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.date_yyyymmdd)> 0)
		{
			memcpy( HOST_MESSAGE.date, 
			&Auth_Tx.TLF01_details.date_yyyymmdd[CUP_MMDD_START_POS],
			CUP_DE13_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if (0 == strlen(Auth_Tx.TLF01_details.date_yyyymmdd) )
		{
			ptetime_get_timestamp( time_date );
			get_time( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
		}
		memcpy(HOST_MESSAGE.date, 
							&Auth_Tx.TLF01_details.date_yyyymmdd[CUP_MMDD_START_POS],
							CUP_DE13_SIZE);
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_14
 *
 *  DESCRIPTION:  This function will move the expiry date
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the local MMDD at the card aceptors location.
 *
 *  FFORMAT: YYMM
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_14( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.exp_date)>0)
		{
			memcpy( HOST_MESSAGE.date_expd,Auth_Tx.TLF01_details.exp_date,CUP_DE14_SIZE );
		}
	}

	else if ( *result_flag == 0 )
	{
		/*
		 * For Void transaction, ATP need not to send DE14
		 * */
		 if((0 == strncmp(Auth_Tx.TLF01_details.processing_code,"02",2))||
			(AUTH_VOID_SALE == Auth_Tx.TLF01_details.tx_key ))
		 {
			 return( ret_val );
		 }
		 else if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.exp_date ) )
		 {
			memcpy( HOST_MESSAGE.date_expd,	Auth_Tx.TLF01_details.exp_date,	CUP_DE14_SIZE );
		 }
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_15
 *
 *  DESCRIPTION:  This function will move the Settlement date
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the transaction settlement date between the acquirer and the issuer.
 *
 *  FFORMAT: MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_15( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.settlement_date)>0)
		{
			memcpy( HOST_MESSAGE.date_settle,Auth_Tx.TLF01_details.settlement_date,	CUP_DE15_SIZE );
		}
	}

	else if ( *result_flag == 0 )
	{
		if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.settlement_date ) )
		{
		memcpy( HOST_MESSAGE.date_settle,Auth_Tx.TLF01_details.settlement_date,	CUP_DE15_SIZE );
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_16
 *
 *  DESCRIPTION:  This function will move the Conversion date
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *               This field is the effective date of the conversion rate for 
 *		    the currency conversion from the original transaction
 *		    currency to the settlement currency.
 *
 *  FFORMAT: MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_16( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.conv_date)>0)
		{
			memcpy( HOST_MESSAGE.conv_date,Auth_Tx.conv_date,CUP_DE16_SIZE );
		}
	}
	
	else if ( *result_flag == 0 )
	{
		if ( 0 != strcmp( NULL_STR, Auth_Tx.conv_date ) )
		{
			memcpy( HOST_MESSAGE.conv_date,	Auth_Tx.conv_date,CUP_DE16_SIZE );
		}
	}
	return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_18
 *
 *  DESCRIPTION:  This function will move the  Merchant Category Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *               This field indicates Merchant Category Code (MCC).
 *
 *  FFORMAT: n4
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_18( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if (strlen(Auth_Tx.MCF01_details.voice_category_code)> 0 )
		{
			memcpy( HOST_MESSAGE.merch_type, 
			Auth_Tx.MCF01_details.voice_category_code,
			CUP_DE18_SIZE );
		}
		else
		{
			memcpy( HOST_MESSAGE.merch_type,
			Auth_Tx.TLF01_details.category_code,
			CUP_DE18_SIZE );
		}
	}

	else if ( *result_flag == 0 )
	{
		if (strlen(Auth_Tx.MCF01_details.voice_category_code)> 0 )
		{
			memcpy( HOST_MESSAGE.merch_type, 
			Auth_Tx.MCF01_details.voice_category_code,
			CUP_DE18_SIZE );
		}
		else
		{
			memcpy( HOST_MESSAGE.merch_type,
			Auth_Tx.TLF01_details.category_code,
			CUP_DE18_SIZE );
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_19
 *
 *  DESCRIPTION:  This function will move the Merchant Country Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *               It is the country code of the merchant.
 *
 *  FFORMAT: n3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_19( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
  	  if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",6))
   	  {
  		  if(0 == strncmp(Auth_Tx.MCF01_details.currency_code,USD,3))
		  {
   	  		memcpy(HOST_MESSAGE.ctry_code,PESO,CUP_DE19_SIZE);
  		  }
  		  else
  		  {
     	  		memcpy(HOST_MESSAGE.ctry_code,
  				   Auth_Tx.MCF01_details.currency_code,
  				   CUP_DE19_SIZE);
  		  }
   	  }
	  else if( strlen(Auth_Tx.country_code)> 0)		/* Cadencie link changes - ThoughFocus Girija Y */
	  {
		if ( isnum(Auth_Tx.country_code) )
		{
		    memcpy(HOST_MESSAGE.ctry_code,
				   Auth_Tx.country_code,
				   CUP_DE19_SIZE);
		}
		else
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
				   "Cannot build request msg for host. Invalid Merchant country code: %s",
	                Auth_Tx.country_code);
		}
	  }
	  else if(strlen(Ncf01_I.country_code) > 0)
	  {
		if ( isnum(Ncf01_I.country_code) )
		{
			memcpy(HOST_MESSAGE.ctry_code, 
				   Ncf01_I.country_code, 
				   CUP_DE19_SIZE );
		}
		else
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
				   "Cannot build request msg for host. Invalid Merchant country code: %s",
	                Auth_Tx.country_code);
		}
	  }
	  else
	  {
    		ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
				   "Cannot build request msg for host.Merchant country code: missing ");
	  }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_22
 *
 *  DESCRIPTION:  This function will move the POS entry mode
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_22( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	CHAR	 temp_entry_mode[4] = {0};

	memset(HOST_MESSAGE.posentry,0x30,CUP_DE22_SIZE);
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)> 0)
		{
			memcpy(HOST_MESSAGE.posentry,
			Auth_Tx.TLF01_details.pos_entry_mode ,
			CUP_DE22_SIZE);
		}

	}

	else if ( *result_flag == 0 )
	{
		if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",5) )
		{
			if( (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"08",2)))
			{
				memset(HOST_MESSAGE.posentry,0x30,CUP_DE22_SIZE);
				memcpy(HOST_MESSAGE.posentry,"02",2);
				memcpy(HOST_MESSAGE.posentry+2,
				Auth_Tx.TLF01_details.pos_entry_mode +3 ,1);
			}
			else if( (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"8",1)))
			{
				memset(HOST_MESSAGE.posentry,0x30,CUP_DE22_SIZE);
				memcpy(HOST_MESSAGE.posentry,"02",2);
				memcpy(HOST_MESSAGE.posentry+2,
				Auth_Tx.TLF01_details.pos_entry_mode +2 ,1);
			}
			else if(Auth_Tx.TLF01_details.pos_entry_mode[1]=='0')
			{
				memcpy(HOST_MESSAGE.posentry,Auth_Tx.TLF01_details.pos_entry_mode +1,CUP_DE22_SIZE);
			}
			else
			{
				memcpy(HOST_MESSAGE.posentry,Auth_Tx.TLF01_details.pos_entry_mode,CUP_DE22_SIZE);
			}
			
			return(ret_val);

		}
		else if ( 0 == strlen(Auth_Tx.TLF01_details.pos_entry_mode) )
		{
			ret_val = false;
			*result_flag = 1;
			strcpy( err_buf, "POS Entry Mode is missing. Decline txn." );
		}
		else if ( false == isnum(Auth_Tx.TLF01_details.pos_entry_mode) )
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
			"Cannot build request msg for host. Invalid POS Entry Mode: %s",
			Auth_Tx.TLF01_details.pos_entry_mode );
		}
#if BDOR_68_REQ_1_UPI
		/*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions With Pin*/
		else if (true == nccup2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   strncpy( HOST_MESSAGE.posentry,
					UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					CUP_DE22_SIZE );
		}
		else if (true == nccup2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
		   strncpy( HOST_MESSAGE.posentry,
					UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					CUP_DE22_SIZE );
		}
		else if(true == nccup2_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   strncpy( HOST_MESSAGE.posentry,
				    UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					CUP_DE22_SIZE );
		}
		/*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions Without Pin*/
		else if(true == nccup2_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
		   strncpy( HOST_MESSAGE.posentry,
				    UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					CUP_DE22_SIZE );
		}
#endif

#if BDOR_68_REQ_3_UPI /* Swiftpass BDO Pay Credit Transactions */
	   else if (true == nccup2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin())
	   {
		   strncpy( HOST_MESSAGE.posentry,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					CUP_DE22_SIZE );
	   }
	   else if (true == nccup2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin())
	   {
		   strncpy( HOST_MESSAGE.posentry,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					CUP_DE22_SIZE );
	   }
	   else if(true == nccup2_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_with_pin())
	   {
		   strncpy( HOST_MESSAGE.posentry,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					CUP_DE22_SIZE );
	   }
	   else if(true == nccup2_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_without_pin())
	   {
		   strncpy( HOST_MESSAGE.posentry,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					CUP_DE22_SIZE );
	   }
#endif
#if BDOR_68_REQ_5_UPI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
	    else if (true == nccup2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx())
	    {
			strncpy( HOST_MESSAGE.posentry,
					UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
					CUP_DE22_SIZE );
		}
		else if(true == nccup2_check_if_trx_is_upi_ecommerce_swiftpass_BDOPay_trx ())
		{
			strncpy( HOST_MESSAGE.posentry,
					UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
					CUP_DE22_SIZE );
		}
#endif
		else
		{
			memcpy(HOST_MESSAGE.posentry,
			Auth_Tx.TLF01_details.pos_entry_mode + 1,
			CUP_DE22_SIZE);
			
			/*	Chnage the POS ENTERY MODE FOR FALLBACK	*/
			if( 0 == strncmp(HOST_MESSAGE.posentry,"80",2))
			{
					memset(HOST_MESSAGE.posentry,0x30,CUP_DE22_SIZE);
					memcpy(HOST_MESSAGE.posentry,"02",2);
					memcpy(HOST_MESSAGE.posentry+2, 
						Auth_Tx.TLF01_details.pos_entry_mode +3 ,1);
			}
			/* For void pin should not be present*/
			if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) )//Coming from POS
			{
				if((0 == strncmp (Auth_Tx.TLF01_details.processing_code,"02",2))||
					(0 == strncmp (Auth_Tx.TLF01_details.processing_code,"20",2)))
					{
						memcpy(HOST_MESSAGE.posentry, "012" ,3);	
					}
			}
			else if(true == pin_block_exists ())			
			{				
				memcpy(HOST_MESSAGE.posentry+2, "1" ,1);			
			}
		}
	}
	memcpy(Auth_Tx.TLF01_details.product_codes[9].code,HOST_MESSAGE.posentry,CUP_DE22_SIZE);
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_23
 *
 *  DESCRIPTION:  This function will move the  Card Sequence Number
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *               The sequence number of IC card
 *
 *  FFORMAT: n3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_23( pINT result_flag, pCHAR err_buf )
{
   INT	 ret_val = true;
   INT	   itemp;
   CHAR    temp_str[4]={0};
   CHAR    temp_str1[4]={0};

   /* We are not suppose to forward DE23 in case of 0420 message and void coming from the pos. Even it is send by POS*/
   /*Client asked to remove these changes as there POS is able to send correct tags for field 55*/
  /*if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) )//Coming from POS
   {
	  if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"04",2))||
		  (0 == strncmp (Auth_Tx.TLF01_details.processing_code,"02",2))||
		  (0 == strncmp (Auth_Tx.TLF01_details.processing_code,"20",2)))
		  {
			  return ret_val; 
		  }
   }*/
   /* For void coming from POS field 23 should not be present*/
	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) )//Coming from POS
	{
		if((0 == strncmp (Auth_Tx.TLF01_details.processing_code,"02",2))||
			(0 == strncmp (Auth_Tx.TLF01_details.processing_code,"20",2)))
			{
				return ret_val;
			}
	}

   if ( *result_flag == 0 )
   {
		
		if (strlen(Auth_Tx.host2_message_seq_nbr) > 0)
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy(temp_str,Auth_Tx.host2_message_seq_nbr+1,CUP_DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOST_MESSAGE.card_seq_num,temp_str1,CUP_DE23_SIZE );
		}else if (strlen(Auth_Tx.EMV_details.pan_sequence_number)> 0)
		{
			  memcpy(temp_str,Auth_Tx.EMV_details.pan_sequence_number,CUP_DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOST_MESSAGE.card_seq_num, temp_str1,CUP_DE23_SIZE );
		}
  }
  return( ret_val );
}

  

/******************************************************************************
 *
 *  NAME:         build_request_field_25
 *
 *  DESCRIPTION:  This function will move the Point of Service Condition Code
 *                to the Host Message structure according to BDO Cadencie hos	
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie hos specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

	if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if ((strlen(Auth_Tx.TLF01_details.pos_condition_code)> 0) && 
		(strlen(Auth_Tx.TLF01_details.pos_condition_code) <= CUP_DE25_SIZE))
		{		
			memcpy(HOST_MESSAGE.pos_condcd, 
			Auth_Tx.TLF01_details.pos_condition_code,
			CUP_DE25_SIZE);
		}
		memcpy(Auth_Tx.TLF01_details.product_codes[4].quantity ,
			   Auth_Tx.TLF01_details.pos_condition_code,CUP_DE25_SIZE);
	}

	else if ( *result_flag == 0 )
	{
		if ((strlen(Auth_Tx.TLF01_details.pos_condition_code)> 0) && 
		(strlen(Auth_Tx.TLF01_details.pos_condition_code) <= CUP_DE25_SIZE))
		{		
			if( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) &&
				 ((Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION) ||
				 (Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION))&&
				 (0 != strncmp(Auth_Tx.TLF01_details.pos_condition_code,"73",3)) )
			{
				memcpy(HOST_MESSAGE.pos_condcd,
					   CUP_PREAUTH_POS_COND_CODE,
					   CUP_DE25_SIZE);
			}
			else
			{
				memcpy(HOST_MESSAGE.pos_condcd,
						Auth_Tx.TLF01_details.pos_condition_code,
						CUP_DE25_SIZE);
			}
			/* Check for MOTO transaction */
			if(true == nccup2_Check_If_CUP_MOTO_TRN())
			{
				memcpy( HOST_MESSAGE.pos_condcd,
						CUP_MOTO_POS_COND_CODE,
						CUP_DE25_SIZE);
			}
			else if ( true == nccup2_Check_If_CUP_RPS_TRN())

			{
				memcpy( HOST_MESSAGE.pos_condcd,
						CUP_UPI_RPS_POS_CONDITION_CODE,
						CUP_DE25_SIZE);
			}
#if BDOR_68_REQ_5_UPI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
			else if (true == nccup2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx())
			{
			   strncpy( HOST_MESSAGE.pos_condcd,
						UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_COND_CODE,
						CUP_DE25_SIZE );
			}
			else if(true == nccup2_check_if_trx_is_upi_ecommerce_swiftpass_BDOPay_trx ())
			{
			   strncpy( HOST_MESSAGE.pos_condcd,
					    UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_COND_CODE,
						CUP_DE25_SIZE );
		   }
#endif
			/* POS condtion code sent to HOST will be stored to dipsplay it on
			 * Tranlookup page */
			memcpy(Auth_Tx.TLF01_details.product_codes[4].quantity ,
				   HOST_MESSAGE.pos_condcd,CUP_DE25_SIZE);
		}

		else
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
			"Cannot build request msg for host.Point of Service Condition Code is : Invalid");
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_26
 *
 *  DESCRIPTION:  This function will move thePoint of Service PIN Capture Code
 *                to the Host Message structure according to BDO Cadencie hos	
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_26( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if ((strlen(Auth_Tx.TLF01_details.product_codes[17].code) > 0) && 
			(strlen(Auth_Tx.TLF01_details.product_codes[17].code) <= CUP_DE26_SIZE))
		{
			strncpy( HOST_MESSAGE.pos_pincode, 
			Auth_Tx.TLF01_details.product_codes[17].code, 
			CUP_DE26_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if ((strlen(Auth_Tx.TLF01_details.product_codes[17].code) > 0) && 
		(strlen(Auth_Tx.TLF01_details.product_codes[17].code) <= CUP_DE26_SIZE))
		{
			strncpy( HOST_MESSAGE.pos_pincode, 
			Auth_Tx.TLF01_details.product_codes[17].code, 
			CUP_DE26_SIZE);
		}
		else if( (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE))
		{
			//Pin code for viod is 00
			strncpy( HOST_MESSAGE.pos_pincode,"00" ,CUP_DE26_SIZE);
		}
		else
		{
			//The maximum number of PIN characters accepted by point service device
			strncpy( HOST_MESSAGE.pos_pincode,"12" ,CUP_DE26_SIZE);
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_28
 *
 *  DESCRIPTION:  This function will move the Transaction Fee Amount
 *                to the Host Message structure according to BDO Cadencie hos	
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie hos specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_28( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if ((strlen(Auth_Tx.TLF01_details.tran_fee_amount) > 0) && 
			(strlen(Auth_Tx.TLF01_details.tran_fee_amount) <= CUP_DE28_SIZE))
		{
			strncpy(HOST_MESSAGE.tran_fee_amt,"D",1);
			memcpy(HOST_MESSAGE.tran_fee_amt +1 ,
			Auth_Tx.TLF01_details.tran_fee_amount,
			CUP_DE28_SIZE);
		}
	}
	else if ( *result_flag == 0 )
	{
		if ((strlen(Auth_Tx.TLF01_details.tran_fee_amount) > 0) && 
		(strlen(Auth_Tx.TLF01_details.tran_fee_amount) <= CUP_DE28_SIZE))
		{
			strncpy(HOST_MESSAGE.tran_fee_amt,"D",1);
			memcpy(HOST_MESSAGE.tran_fee_amt +1 ,
			Auth_Tx.TLF01_details.tran_fee_amount,
			CUP_DE28_SIZE);
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_32
 *
 *  DESCRIPTION:  This function will move the acquiring institution ID
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is  numeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_32( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len = 0;
	CHAR temp_len[3] = {0};


	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{
		if( strlen(Auth_Tx.acq_inst_id_cd)> 0 )
		{
			len = strlen( Auth_Tx.acq_inst_id_cd );
			memcpy(HOST_MESSAGE.acq_inst_id,Auth_Tx.acq_inst_id_cd,len);
		}
		else
		{
			len = 0;
			len = strlen( Ncf01_I.acquirer_inst_id_code );
			if(len > 0)
			{
				memcpy( HOST_MESSAGE.acq_inst_id, Ncf01_I.acquirer_inst_id_code, len );
			}
		}
	}
	else if ( *result_flag == 0 )
	{
		len = strlen( Ncf01_I.acquirer_inst_id_code );
		memcpy( HOST_MESSAGE.acq_inst_id, Ncf01_I.acquirer_inst_id_code, len );
	}

	/* Get the length. */
	if(len > 0)
	{
		sprintf( temp_len, "%02d", len );
		memcpy( HOST_MESSAGE.acq_inst_len, temp_len, CUP_ACQ_INST_ID_CODE_LEN );

		/* Store value in TLF01 if there is not already a value. */
		if ( Auth_Tx.TLF01_details.acquiring_id[0] == 0x00 )
		{
		memcpy( Auth_Tx.TLF01_details.acquiring_id,HOST_MESSAGE.acq_inst_id,len );
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_33
 *
 *  DESCRIPTION:  This function will move the forwarding institution ID
 *                to the Host Message structure according to  BDO Cadencie host
 *                formatting specifications.  It is  numeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message -  BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_33( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len;
	CHAR temp_len[3] = {0};
	CHAR  error_msg[256] = {0};

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if ( strlen(Auth_Tx.forwarding_inst_id_cd) > 0 )
		{
			len = strlen( Auth_Tx.forwarding_inst_id_cd );
			memcpy( HOST_MESSAGE.fwd_inst_id, Auth_Tx.forwarding_inst_id_cd, len );

			sprintf( temp_len, "%02d", len );
			memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, CUP_FORW_INST_ID_CODE_LEN );
		}
		else 
		{
			len = strlen( Ncf01_I.forwarding_inst_id_code );
			memcpy( HOST_MESSAGE.fwd_inst_id, Ncf01_I.forwarding_inst_id_code, len );
            
			sprintf( temp_len, "%02d", len );
			memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, CUP_FORW_INST_ID_CODE_LEN );
		}
	}

	else if ( *result_flag == 0 )
	{
		if ( strlen(Auth_Tx.forwarding_inst_id_cd) > 0 )
		{
			len = strlen( Auth_Tx.forwarding_inst_id_cd );
			memcpy( HOST_MESSAGE.fwd_inst_id, Auth_Tx.forwarding_inst_id_cd, len );

			sprintf( temp_len, "%02d", len );
			memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, CUP_FORW_INST_ID_CODE_LEN );
		}
		else 
		{
			len = strlen( Ncf01_I.forwarding_inst_id_code );
			memcpy( HOST_MESSAGE.fwd_inst_id, Ncf01_I.forwarding_inst_id_code, len );
			if(len >0)
			{
				sprintf( temp_len, "%02d", len );
				memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, CUP_FORW_INST_ID_CODE_LEN );
			}
			else
			{
				sprintf( error_msg,
	            		 "No Forward Intitution ID (field 33) : %s,",
	              		 Ncf01_I.forwarding_inst_id_code );
					nccup2_log_message( 1, 2, error_msg, "build_request_field_33",1 );
			}
		}
	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_35
 *
 *  DESCRIPTION:  This function will move the track2 data
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *                This function will create track2 data from track1 data if
 *                there is no track2, but track1 does exist.  If there is no
 *                track1, then track2 is created from card number and expiry
 *                date.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_35( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR temp_len[3]="";

	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{
		if(strlen(Auth_Tx.TLF01_details.track2) > 0 )
		{
			len = strlen( Auth_Tx.TLF01_details.track2 );
			sprintf( temp_len, "%02d", len );
			memcpy( HOST_MESSAGE.track2_len, temp_len, CUP_TRACK_2_DATA_LEN);
			memcpy( HOST_MESSAGE.track2, Auth_Tx.TLF01_details.track2 ,len);
		}
	}
	else if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.track2[0] != 0x00)
		{
			len = strlen( Auth_Tx.TLF01_details.track2 );
			memcpy( HOST_MESSAGE.track2, Auth_Tx.TLF01_details.track2, len );
			sprintf( temp_len, "%02d", len );
			memcpy( HOST_MESSAGE.track2_len, temp_len, CUP_TRACK_2_DATA_LEN);
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_37
 *
 *  DESCRIPTION:  This function will move the Acquirer reference number
 *                to the Host Message structure according to BDO Cadencie host 
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host  specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_37( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.retrieval_ref_num)>0)
		{
			memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,CUP_DE37_SIZE);
		}
	}

	else if ( *result_flag == 0 )
	{
		if ((strlen(Auth_Tx.TLF01_details.retrieval_ref_num) > 0) && 
		(strlen(Auth_Tx.TLF01_details.retrieval_ref_num) <= CUP_DE37_SIZE))
		{
			memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,CUP_DE37_SIZE);
		}
		else
		{
			memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.orig_retrieval_ref_num,CUP_DE37_SIZE);
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_38
 *
 *  DESCRIPTION:  This function will move the authorization number
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:     Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_38( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if ( strlen(Auth_Tx.TLF01_details.auth_number)> 0 )
		{

			memcpy( HOST_MESSAGE.authcode,
					Auth_Tx.TLF01_details.auth_number,
					CUP_DE38_SIZE );
		}
	}
	else if ( *result_flag == 0 )
	{
	 // NO need to validate this for CUP
	  if ( strlen(Auth_Tx.TLF01_details.auth_number)> 0 )
	  {
	     
	     memcpy( HOST_MESSAGE.authcode,
	             Auth_Tx.TLF01_details.auth_number,
				 CUP_DE38_SIZE );
	  }
	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_39
 *
 *  DESCRIPTION:  This function will move the response code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_39( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch", 6))
   	{

   		if (0 == strncmp(Auth_Tx.TLF01_details.response_code, "19", 2)) {
   			/*Lets log this transaction in ewlog for MC Duplicate PROD issue*/

   			memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
   			sprintf(Log_Bufr,
   					" Timeout Advice_Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
   					Auth_Tx.TLF01_details.message_type,
   					Auth_Tx.TLF01_details.sys_trace_audit_num,
   					Auth_Tx.TLF01_details.terminal_id,
   					Auth_Tx.TLF01_details.merchant_id,
   					Auth_Tx.TLF01_details.retrieval_ref_num);

   			nccup2_log_message(2, 1, Log_Bufr, "build_host_structure",
   					1);
   		} else {

   			/*Lets log this transaction in ewlog for MC Duplicate PROD issue*/

   			memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
   			sprintf(Log_Bufr,
   					" Advice_Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
   					Auth_Tx.TLF01_details.message_type,
   					Auth_Tx.TLF01_details.sys_trace_audit_num,
   					Auth_Tx.TLF01_details.terminal_id,
   					Auth_Tx.TLF01_details.merchant_id,
   					Auth_Tx.TLF01_details.retrieval_ref_num);

   			nccup2_log_message(2, 1, Log_Bufr, "build_host_structure",
   					1);

   		}
   	}

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{	
		if(strlen( Auth_Tx.TLF01_details.response_code)> 0)
		{
			memcpy( HOST_MESSAGE.responsecd, Auth_Tx.TLF01_details.response_code, CUP_DE39_SIZE );
		}
		else
		{
			memcpy( HOST_MESSAGE.responsecd, "00", CUP_DE39_SIZE );
		}
	}
	else if ( *result_flag == 0 )
	{
		if ( 0 == strlen(Auth_Tx.TLF01_details.response_code) )
		{
			memcpy( HOST_MESSAGE.responsecd, "00", CUP_DE39_SIZE );
		}
		else
		{
			memcpy( HOST_MESSAGE.responsecd, Auth_Tx.TLF01_details.response_code, CUP_DE39_SIZE );
		}
	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_41
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric specail with spaces,
 *                fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_41( pINT result_flag, pCHAR err_buf )
{
	INT  i,ret_val = true;
	CHAR temp_str[8]="";

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.terminal_id)>0)
		{
			memcpy(HOST_MESSAGE.termid,Auth_Tx.TLF01_details.terminal_id, CUP_DE41_SIZE);
		}
	}
	
	else if ( *result_flag == 0 )
	{
		if ((strlen(Auth_Tx.TLF01_details.terminal_id) > 0) && 
		   (strlen(Auth_Tx.TLF01_details.terminal_id) <= CUP_DE41_SIZE))
			{
		 		strcpy(temp_str, Auth_Tx.TLF01_details.terminal_id);
				/* If Device number less than 8, intead of NULL fills with spaces*/
				for (i = strlen(Auth_Tx.TLF01_details.terminal_id); i <8 ; i++)
				{
				 	temp_str[i] = ' ';
				}
				memcpy(HOST_MESSAGE.termid,temp_str, CUP_DE41_SIZE);
			}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_42
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Identification Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric specail, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_42( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   //CUP Acceptor IS is stored in MCF01.network[3].priority_routing and MCF01.network[4].priority_routing fields 
   INT cup_acp_id1=0;
   INT cup_acp_id2=0;

   if ( *result_flag == 0 )
   {
   		cup_acp_id1 = strlen(Auth_Tx.MCF01_details.network[3].priority_routing);
   		
	    if ((strlen(Auth_Tx.TLF01_details.merchant_id) > 0) && 
	        (strlen(Auth_Tx.TLF01_details.merchant_id) <= CUP_DE42_SIZE))
	    {
    	 	memset(HOST_MESSAGE.cardaccid,0x20,CUP_DE42_SIZE);
		 	memcpy(HOST_MESSAGE.cardaccid,Auth_Tx.TLF01_details.merchant_id,CUP_DE42_SIZE);
	    }else if (cup_acp_id1 > 0)
	    {
    	 	memset(HOST_MESSAGE.cardaccid,0x20,CUP_DE42_SIZE);
		 	memcpy(HOST_MESSAGE.cardaccid,Auth_Tx.MCF01_details.network[3].priority_routing,cup_acp_id1);

			cup_acp_id2=strlen(Auth_Tx.MCF01_details.network[4].priority_routing);
			if ( cup_acp_id2 > 0)
			{
				memcpy(HOST_MESSAGE.cardaccid + cup_acp_id1,Auth_Tx.MCF01_details.network[4].priority_routing,cup_acp_id2);
			}
	    }
    }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_43
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location
 *                to the Host Message structure according to  BDO Cadencie host
 *                formatting specifications.  It is alphanumeric specail, fixed length.
 *
 * 		Store DE43 here so it gets logged to TLF01:
 *   		Merchant Name         --> TLF01.card_holder_name
 *   		City and Country Code --> TLF01.ncf01_retired_cwk
 *
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Card Acceptor Name/Location specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_43( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  f43_Len = 0;


    /*
        	 Sub-field    Position    Length                      Feild details
	              1             1            25               Merchant name or ATM location (padding with blanks)
	              2            26           12               City�s name (padding with blanks)
	              3            38            3                Country/region Code, adopting the coding
	 */


	 if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5)) 
	 {
		 memset(HOST_MESSAGE.cardaccname,' ',CUP_DE43_SIZE);
		 
		 memcpy( HOST_MESSAGE.cardaccname,
		 		 Auth_Tx.TLF01_details.card_holder_name,
				 CUP_MER_NAME_LEN_FEILF_43 );// Max 25 Byte
		 
		 memcpy( HOST_MESSAGE.cardaccname+25,
		 		 Auth_Tx.TLF01_details.ncf01_retired_cwk,
				 CUP_CITY_LEN_FEILD_43 );// Max 12 Byte
		 
         memcpy( HOST_MESSAGE.cardaccname+37,
		 	     Auth_Tx.TLF01_details.ncf01_retired_cwk+13,
				 CUP_COUNTY_CODE_LEN_FEILD_43 );// Max 3 Byte
	 }
	 else  if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",6))
	 {
		 /* Store the Merchant name */
	     memset(HOST_MESSAGE.cardaccname,' ',CUP_DE43_SIZE);
		 memcpy( HOST_MESSAGE.cardaccname,Auth_Tx.TLF01_details.card_holder_name,
				 strlen(Auth_Tx.TLF01_details.card_holder_name) );
		 memcpy( HOST_MESSAGE.cardaccname+CUP_MER_NAME_LEN_FEILF_43,Auth_Tx.TLF01_details.ncf01_retired_cwk,
				 strlen(Auth_Tx.TLF01_details.ncf01_retired_cwk) );
		 memcpy( HOST_MESSAGE.cardaccname+(CUP_MER_NAME_LEN_FEILF_43+CUP_CITY_LEN_FEILD_43),Auth_Tx.TLF01_details.ncf01_retired_cwk+13,
		         		 CUP_COUNTY_CODE_LEN_FEILD_43 );
		 return( ret_val );
	 }

	 else   if ( *result_flag == 0 )
	 {
	   	 memset(HOST_MESSAGE.cardaccname,0x20,CUP_DE43_SIZE);
	 
  		 if( strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0)
		 {
			 /* Transaction is ATM. */
			 memcpy( HOST_MESSAGE.cardaccname, 
			 		 Auth_Tx.MCF01_details.address01,
					 CUP_ATM_LOC_LEN_FEILD_43);// Max 25 Byte
		 }
		 else
		 {
			 /* Transaction is a POS transaction. */
			if(strcmp(Auth_Tx.MCF01_details.name02,"")!=0)
			{
				memcpy( HOST_MESSAGE.cardaccname, 
						Auth_Tx.MCF01_details.name02, 
						CUP_MER_NAME_LEN_FEILF_43);	// Max 25 Byte
			}
		 }
      
		 memcpy( HOST_MESSAGE.cardaccname+25, 
		 	     Auth_Tx.MCF01_details.city, 
				 CUP_CITY_LEN_FEILD_43);// Max 12 Byte
		 
		 memcpy( HOST_MESSAGE.cardaccname+37,
				 CUP_COUNTRY_CODE_FEILD_43,
				 CUP_COUNTY_CODE_LEN_FEILD_43);// Max 3 Byte
	 }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_44
 *
 *  DESCRIPTION:  This function will move the Additional Response Data
 *                to the Host Message structure according to  BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message -  BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_44( pINT result_flag, pCHAR err_buf )
{
	INT len = 0;
	INT ret_val = true;
	BYTE temp_len[3] = {0};
	
	/*Based on value set in nccup/dcpiso we have to update this function*/	
	/* data present in Auth_Tx.acct_id_1 is LLVAR*/

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{	
		if((strlen(Auth_Tx.acct_id_1)> 0 ))
		{
			/* Copy length from Auth_Tx.acct_id_1 */
			strncpy(temp_len,Auth_Tx.acct_id_1,CUP_DE44_LEN_SIZE);
			
			len=atoi(temp_len);// len is having noew data length
			
			/* Store additional response data into host message */
			memcpy(HOST_MESSAGE.addlrspdata,Auth_Tx.acct_id_1+2,len);// Store data after length
			
			/* Store length of additional response data into host message*/
			memcpy(HOST_MESSAGE.addl_rsp_len,temp_len,CUP_DE44_LEN_SIZE);
		}
	}
	
	else if ( *result_flag == 0 )
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6) )
		{
			//Incase any garbage value has been copied. NULL it.
			memcpy(HOST_MESSAGE.addl_rsp_len,0x00,sizeof(HOST_MESSAGE.addl_rsp_len));
			memcpy(HOST_MESSAGE.addlrspdata,0x00,sizeof(HOST_MESSAGE.addlrspdata));
		}

		if((strlen(Auth_Tx.acct_id_1)> 0 ))
		{
			/* Copy length from Auth_Tx.acct_id_1 */
			strncpy(temp_len,Auth_Tx.acct_id_1,CUP_DE44_LEN_SIZE);
			
			len=atoi(temp_len);// len is having noew data length
			
			/* Store additional response data into host message */
			memcpy(HOST_MESSAGE.addlrspdata,Auth_Tx.acct_id_1+2,len);// Store data after length
			
			/* Store length of additional response data into host message*/
			memcpy(HOST_MESSAGE.addl_rsp_len,temp_len,CUP_DE44_LEN_SIZE);
		}
	}
	return(ret_val);
}
	
/******************************************************************************
 *
 *  NAME:         build_request_field_45
 *
 *  DESCRIPTION:  This function will move the track1
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  Code set of the track 2 and 3 on magnetic
 *		    stripe cards as defined in ISO 4909 and ISO 7813, variable length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_45( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR  temp_len[3]="";

    if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.track1)>0)
		{
			len = strlen( Auth_Tx.TLF01_details.track1 );
			sprintf(temp_len,"%2d",len);
			memcpy(HOST_MESSAGE.track1_len,temp_len ,CUP_DE45_LEN_SIZE);
			memcpy( HOST_MESSAGE.track1, Auth_Tx.TLF01_details.track1  , len  );
		}
	}

	else if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.track1[0] != 0x00)
		{
			len = strlen( Auth_Tx.TLF01_details.track1 );
			sprintf(temp_len,"%2d",len);
			memcpy(HOST_MESSAGE.track1_len,temp_len ,CUP_DE45_LEN_SIZE);
			memcpy( HOST_MESSAGE.track1, Auth_Tx.TLF01_details.track1, len );
		}
	}
   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_48
 *
 *  DESCRIPTION:  This function will move the additional Data-Private
 *                to the Host Message structure according to  BDO Cadencie host
 *                formatting specifications.  It is alphanumeric binary and specail, variable length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message -  BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:         Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_48( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  temp_len[3] = {0};
	CHAR temp_data[100] = {0};


	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{
		if(strlen(Auth_Tx.TLF01_details.voice_auth_text)>0)
		{
			memcpy( temp_len,Auth_Tx.TLF01_details.voice_auth_text, CUP_DE48_LEN_SIZE);
			memcpy( HOST_MESSAGE.addl_data_len, temp_len, CUP_DE48_LEN_SIZE);
			len = strlen(Auth_Tx.TLF01_details.voice_auth_text);
			memcpy( HOST_MESSAGE.addl_data,
					Auth_Tx.TLF01_details.voice_auth_text +
					CUP_DE48_LEN_SIZE,(len - CUP_DE48_LEN_SIZE));
		}
	}
	
	else if( *result_flag == 0 )
	{
		//Check if transaction is Preauth
		if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))
		{
			if( Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
				Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION ||
				Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION ||
				(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"03",2)))
				{
					strncpy(temp_data,CUP_ACQ_PREAUTH_ADD_INFO_FUNC_CODE,CUP_ACQ_ADD_INFO_FUNC_CODE_LEN);
				}
				else
				{
					strncpy(temp_data,CUP_ACQ_ADD_INFO_FUNC_CODE,CUP_ACQ_ADD_INFO_FUNC_CODE_LEN);
				}
				len = strlen(temp_data);
				memcpy( HOST_MESSAGE.addl_data,temp_data,len);

				sprintf( temp_len, "%03d", len );
				memcpy( HOST_MESSAGE.addl_data_len, temp_len, CUP_DE48_LEN_SIZE);
		}
		else
		{
			strncpy(temp_data,CUP_ACQ_ADD_INFO_FUNC_CODE,CUP_ACQ_ADD_INFO_FUNC_CODE_LEN);
			len = strlen(temp_data);
			memcpy( HOST_MESSAGE.addl_data,temp_data,len);

			sprintf( temp_len, "%03d", len );
			memcpy( HOST_MESSAGE.addl_data_len, temp_len, CUP_DE48_LEN_SIZE);
		}

	}
	return(true);
	}

/******************************************************************************
 *
 *  NAME:         build_request_field_49
 *
 *  DESCRIPTION:  This function will move the Transaction Currency Code, 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_49( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   
	if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
	   if (strlen(Auth_Tx.TLF01_details.currency_code)> 0 )
	   {
		   
		   memcpy( HOST_MESSAGE.tran_curr_cd,
		   	       Auth_Tx.TLF01_details.currency_code,
				   CUP_DE49_SIZE);
	   }
	}
	else if ( *result_flag == 0 )
	{
		if(strlen(Auth_Tx.TLF01_details.currency_code)> 0)
		{
		   	if ( false == isalphanum(Auth_Tx.TLF01_details.currency_code) )
			{
				 ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code in TLF01 (%s)",
					  Auth_Tx.TLF01_details.currency_code );
			}
	    	  //supporting new currency
		   	else if ( (strlen(Auth_Tx.TLF01_details.nfi_seq_nbr) > 0) &&
		   			  0!=strncmp (Auth_Tx.TLF01_details.nfi_seq_nbr, PESO ,3) &&
	    			  0!=strncmp (Auth_Tx.TLF01_details.nfi_seq_nbr, USD,3) &&
	    			  0==strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6))
	    	{
	              memcpy( HOST_MESSAGE.tran_curr_cd,
	                      Auth_Tx.TLF01_details.nfi_seq_nbr,
						  CUP_DE49_SIZE );
	    	}
			else
			{
				memcpy( HOST_MESSAGE.tran_curr_cd,
					    Auth_Tx.TLF01_details.currency_code,
						CUP_DE49_SIZE );
			}
		}
		else
		{  
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code (%s)",
					  Auth_Tx.TLF01_details.currency_code );
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_50
 *
 *  DESCRIPTION:  This function will move the reconciliation Settlement currency code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_50 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if( strlen(Auth_Tx.settlement_curr_code)> 0)
	   	{
	   		memcpy( HOST_MESSAGE.settle_cur_cd,
				    Auth_Tx.settlement_curr_code,
					CUP_DE50_SIZE );
		}
	}

   else if ( *result_flag == 0 )
   {
	   if( strlen(Auth_Tx.settlement_curr_code)> 0)
	   {
			if ( false == isalphanum(Auth_Tx.settlement_curr_code) )
			{
				 ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code (%s)",
					  Auth_Tx.settlement_curr_code );
			}
			else
			{
				memcpy( HOST_MESSAGE.settle_cur_cd,
					    Auth_Tx.settlement_curr_code,
						CUP_DE50_SIZE );
			}
	   }
	}
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_51
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing currency code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_51 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE temp_str[5]="";
   if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if (Auth_Tx.ch_billing_curr_code [0] != '\0')
		{
		   memcpy (HOST_MESSAGE.card_curr_cd, Auth_Tx.ch_billing_curr_code, CUP_DE51_SIZE);
		}  
	}
   
   else if ( *result_flag == 0 )
   {
   	  /* if the txn comes from DCPSIO and voice while sending 0120 msg 
           no need to pass card holder billing currecny code */
      if((strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",4)==0)&&(
	      (strcmp(Auth_Tx.TLF01_details.dispensation.origin,"LO")==0))||
          (strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0))
	   {
		   // Don't pass the values 
	   }
	   
	   else if (Auth_Tx.ch_billing_curr_code [0] != '\0')
	   {
			memcpy (HOST_MESSAGE.card_curr_cd, Auth_Tx.ch_billing_curr_code, CUP_DE51_SIZE);
	   }  
	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_52
 *
 *  DESCRIPTION:  This function will move the pin block
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_52( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR tempstr[20];


   if ( *result_flag == 0 )
   {
      /* Field is mandatory. Zero fill it if it does not exist. */
      if ( Auth_Tx.TLF01_details.pin_block[0] == 0x00 )
      {
         memset( HOST_MESSAGE.pinblock, 0x00, CUP_DE52_SIZE );
      }
      else
      {
      	  if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
      	  {
  	         memset( HOST_MESSAGE.pinblock, 0x00, CUP_DE52_SIZE );
			 memcpy(HOST_MESSAGE.pinblock,Auth_Tx.TLF01_details.pin_block,
			 		strlen(Auth_Tx.TLF01_details.pin_block));
		  }
		  else
		  {
        	 memset( HOST_MESSAGE.pinblock, 0x00, CUP_DE52_SIZE );
        	 memset( tempstr, 0x00, sizeof(tempstr) );

         	/* Convert to Hex */
        	 ascii_to_bin_with_length( Auth_Tx.TLF01_details.pin_block,
            	           strlen(Auth_Tx.TLF01_details.pin_block),
                	       &tempstr[0], sizeof(tempstr) );

	         memcpy(HOST_MESSAGE.pinblock, tempstr, sizeof(HOST_MESSAGE.pinblock) );
		  }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_53
 *
 *  DESCRIPTION:  This function will move the Security Related Control Information
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_53 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR temp_len[3]="";

   if(	0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
   {
   		if ( strlen (Auth_Tx.mac_key ) > 0)
		{
			 memcpy(HOST_MESSAGE.sec_cntl,Auth_Tx.mac_key ,CUP_DE53_SIZE);
		}
   }

   /*We will store  Security Info in mac_key of Auth_tx. feild */

   //mac_key
   else if ( *result_flag == 0 )
   {
		if ( strlen (Auth_Tx.mac_key ) > 0)
		{
			 memcpy(HOST_MESSAGE.sec_cntl,Auth_Tx.mac_key ,CUP_DE53_SIZE);
		}
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_54
 *
 *  DESCRIPTION:  This function will move theAdditional Amounts
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_54 ( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len;
	INT  position_count=0;
	BYTE tem_str[41]="";
	BYTE temp_len[4]="";

	BYTE acc_type[3]="";
	BYTE acc_type_saving[3]="10";
	BYTE acc_type_debit[3]="20";
	BYTE acc_type_credit[3]="30";

	BYTE balance_type_ledger[3]="01";
	BYTE balance_type_available[3]="02";

	BYTE currency_code[4]="";

	BYTE balance_symbol_credit[2]="C";
	BYTE balance_symbol[2]="";

	BYTE balance_ledger[13]="";
	BYTE balance_aviable[13]="";


    if((Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY )||
		(Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE))
    {
		if ( 0 == strncmp(Auth_Tx.TLF01_details.response_code, "00", 2) )
		{
			/*Prepare 1st 20 BYTE*/

			/*Prepare Account type*/
			if( 0 == strncmp(Auth_Tx.TLF01_details.processing_code+2, "10",2))
			{
				memcpy( acc_type, acc_type_saving, 2 );
			}
			else if( 0 == strncmp(Auth_Tx.TLF01_details.processing_code+2, "20",2))
			{
				memcpy( acc_type, acc_type_debit, 2 );
			}
			else 
			{	
				memcpy( acc_type, acc_type_credit, 2 );
			}
			memcpy( tem_str + position_count, acc_type, 2 );
			position_count = position_count + 2;

			/*Prepare Balanc TYPE*/
			memcpy( tem_str + position_count, balance_type_available, 2 );
			position_count = position_count + 2;

			/*Prepare Currency Code */
			if ( 0 == strncmp(Auth_Tx.ACF01_details.primary_key.account_type,CUP_PESO_ACCOUNT_TYPE,1))
			{
				strcpy( currency_code, PESO );
			}
			else
			{
				strcpy( currency_code, USD );
			}
			memcpy( tem_str + position_count, currency_code, 3 );
			position_count = position_count + 3;

			/*Prepare Balance Symbol*/
			memcpy(balance_symbol , balance_symbol_credit , 1);
			memcpy( tem_str + position_count, balance_symbol, 2 );
			position_count = position_count + 1;
			/*Prepare Balance*/
			memcpy(tem_str + position_count, Auth_Tx.ACF01_details.cash_available, 12);
			position_count = position_count + 12;

			/*Prepare next 20 BYTE*/
			memcpy( tem_str + position_count, acc_type, 2 );
			position_count = position_count + 2;

			memcpy( tem_str + position_count, balance_type_available, 2 );
			position_count = position_count + 2;

			memcpy( tem_str + position_count, currency_code, 3 );
			position_count = position_count + 3;

			memcpy( tem_str + position_count, balance_symbol, 2 );
			position_count = position_count + 1;

			memset(tem_str + position_count, "0",12);
			position_count = position_count + 12;

			sprintf( temp_len, "%03d", position_count );
			memcpy(HOST_MESSAGE.AddlAmts_len,temp_len, CUP_DE54_LEN_SIZE);
			memcpy(HOST_MESSAGE.AddlAmts,tem_str,position_count);
		}
	}

	return( ret_val );
	}

/******************************************************************************
 *
 *  NAME:         build_request_field_55
 *
 *  DESCRIPTION:  This function will move IC Card Data
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani Kumar
 *
 ******************************************************************************/

INT build_request_field_55 ( pINT result_flag, pCHAR err_buf )
{
		INT  ret_val = true;
		INT  len,i;
		CHAR temp_bcd_len[5] = {0};
		CHAR temp_ascii_len[5] = {0};
		CHAR temp_len[5] = {0};
		BYTE De55Buffer[269] = {0};
		BYTE Bin_De55Buffer[135] = {0};
		BYTE HeaderVer[5]="0001";
		BYTE Bin_hdr_ver[3]={0};
		INT de55_len=0,IADlen=0;
		INT asciiTagLen=0;
		INT binTagLen=0;
		INT emv_len=0;
		BYTE tag_95[128]={0};
		BYTE tag_9F10[128]={0};
		BYTE tag_9F36[128]={0};
		BYTE strtag_95_len[3]={0};
		BYTE strtag_9F10_len[3]={0};
		BYTE strtag_9F36_len[3]={0};
		BYTE ascci_emv_data[255]={0};
		BYTE binary_emv_data[255]={0};
		INT rev_tag_len=0;
		INT tag_95_len = 0;
		INT tag_9F10_len = 0;
		INT tag_9F36_len = 0;
		INT emv_rev_len =0;

		if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
		(Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
		{
			return true;
		}
        
		/* For void coming from POS field 55 should not be present*/
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) )//Coming from POS
		{
			if((0 == strncmp (Auth_Tx.TLF01_details.processing_code,"02",2))||
				(0 == strncmp (Auth_Tx.TLF01_details.processing_code,"20",2)))
				{
					return ret_val;
				}
		}
		/* We are not suppose to forward DE55 in case of 0420 message and void coming from the pos. Even it is send by POS*/
		/*Client asked to remove these changes as there POS is able to send correct tags for field 55*/
		/*
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) )//Coming from POS
		 {
			if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"04",2))||
				(0 == strncmp (Auth_Tx.TLF01_details.processing_code,"02",2))||
				(0 == strncmp (Auth_Tx.TLF01_details.processing_code,"20",2)))
				{
					return ret_val;	
				}
		 }*/
		
		//HOST_MESSAGE.IccData_len
		 if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )// Issuing tx, Coming from CUP		 
		 {

			
			memcpy(temp_ascii_len,Auth_Tx.EMV_details.emv_block,3); // First 3 bytes length of CUP EMV data
			memcpy(HOST_MESSAGE.IccData_len,temp_ascii_len,3);
			len = atoi(temp_ascii_len);
			memcpy(HOST_MESSAGE.IccData,Auth_Tx.EMV_details.emv_block+3,len);
			
			return ret_val;			
		 }

		if ((0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "nceqit", 6)  ||
			 0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "neqit2", 6)) &&
				Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		{
			emv_len = Auth_Tx.EMV_details.emv_block[0];
			sprintf(HOST_MESSAGE.IccData_len,"%03d",emv_len);

			memcpy(HOST_MESSAGE.IccData, &Auth_Tx.EMV_details.emv_block[1], emv_len);
			return( ret_val );
		}

		 // Len
		emv_len = genutil_bcd_to_int( Auth_Tx.EMV_details.emv_block, 2 );
		if(emv_len == 0)
		return true;
	
		if( (0 == strncmp(Auth_Tx.TLF01_details.message_type,"04",02)))
		{
			//build only 3 tags 95 , 9F10 and 9F36 	
			strcpy(tag_95,"95"); // Tag
			tag_95_len = strlen(Auth_Tx.EMV_details.tvr);
			sprintf(strtag_95_len,"%02x",tag_95_len/2);
			strncat(tag_95,strtag_95_len,2); // len
			strncat(tag_95,Auth_Tx.EMV_details.tvr,tag_95_len); //data
			
			strcpy(tag_9F10,"9F10");//Tag
			tag_9F10_len = strlen(Auth_Tx.EMV_details.iad);
			sprintf(strtag_9F10_len,"%02x",tag_9F10_len/2);
			strncat(tag_9F10,strtag_9F10_len,2); // len
			strncat(tag_9F10,Auth_Tx.EMV_details.iad,tag_9F10_len);//data
			
			strcpy(tag_9F36,"9F36");
			tag_9F36_len = strlen(Auth_Tx.EMV_details.atc);
			sprintf(strtag_9F36_len,"%02x",tag_9F36_len/2);
			strncat(tag_9F36,strtag_9F36_len,2); // len
			strncat(tag_9F36,Auth_Tx.EMV_details.atc,tag_9F36_len);
			
			 emv_rev_len = strlen(tag_95)+strlen(tag_9F10)+strlen(tag_9F36);
			 strcat(ascci_emv_data,tag_95);
			 strcat(ascci_emv_data,tag_9F10);
			 strcat(ascci_emv_data,tag_9F36);
			 ascii_to_bin(ascci_emv_data,binary_emv_data,emv_rev_len,emv_rev_len/2);
			 sprintf(HOST_MESSAGE.IccData_len,"%03d",emv_rev_len/2);
			 memcpy(HOST_MESSAGE.IccData,binary_emv_data,emv_rev_len/2);
			 return(ret_val);
		}
		sprintf(HOST_MESSAGE.IccData_len,"%03d",emv_len);
		
		 // EMV data
		memcpy(HOST_MESSAGE.IccData,Auth_Tx.EMV_details.emv_block+2,emv_len);
	


		return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_57
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_57 ( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len = 0;
	BYTE temp_len[3] = {0};
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.future_use_1)> 0)
	   	{
			 // We will store length in TLF01_details.product_codes[5].quantity
		 	 memcpy(temp_len, Auth_Tx.TLF01_details.product_codes[5].quantity, CUP_DE57_LEN_SIZE);
		 	 len=atoi(temp_len);
		 	 sprintf( HOST_MESSAGE.add_data_pvt_len, "%03d", len );
		 
		 	 memcpy( HOST_MESSAGE.add_data_pvt, Auth_Tx.future_use_1, len );
		}
	}
   
	else if ( *result_flag == 0 )
	{
	   if(strlen(Auth_Tx.future_use_1)> 0)
	   	{
			 // We will store length in TLF01_details.product_codes[5].quantity
		 	 memcpy(temp_len, Auth_Tx.TLF01_details.product_codes[5].quantity, CUP_DE57_LEN_SIZE);
		 	 len=atoi(temp_len);
		 	 sprintf( HOST_MESSAGE.add_data_pvt_len, "%03d", len );
		 
		 	 memcpy( HOST_MESSAGE.add_data_pvt, Auth_Tx.future_use_1, len );
		}
	}

  
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         build_request_field_59
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_59 ( pINT result_flag, pCHAR err_buf )
{
	INT ret_val= true;
	return ret_val;

}


/******************************************************************************
 *
 *  NAME:         build_request_field_60
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_60 ( pINT result_flag, pCHAR err_buf )
{

	INT  ret_val = true;
	INT  len = 0;
	INT  len_60_3 =0;
	CHAR temp_len[4] = {0};
	BYTE temp_pos[4] = {0};
	INT  temp_posentry=0;
	BYTE transaction_medium[3] = {0};
	CHAR team_field_23[4] = {0};
	INT  temp_len_field_23 =0;

	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{
		if(strlen(Auth_Tx.future_use_4)>0)
		{
			memcpy(temp_len, Auth_Tx.future_use_4,CUP_DE60_LEN_SIZE);
			len = atoi(temp_len);

			if(len <= CUP_MAX_DE60_SIZE)
			{
				memcpy( HOST_MESSAGE.self_def_feild_len,temp_len,CUP_DE60_LEN_SIZE);

				memcpy( HOST_MESSAGE.self_def_feild, 
				Auth_Tx.future_use_4 + CUP_DE60_LEN_SIZE, len) ;
			}
		}				 
	}
	else if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",5) )
	{
		/*Prepare feild 60.1*/
		memcpy(HOST_MESSAGE.self_def_feild,CUP_MESSAGE_REPONSE_CODE_FEILD_60_1,
				CUP_MESSAGE_REPONSE_CODE_FEILD_60_1_LEN);
		memcpy(HOST_MESSAGE.self_def_feild +4 ,Auth_Tx.TLF01_details.product_codes[16].amount,11);
		/*memcpy(HOST_MESSAGE.self_def_feild+4,"0",1); */
		len = strlen(HOST_MESSAGE.self_def_feild);
		/*Total length of feild 60 temp_len*/
		sprintf(temp_len ,"%03d", len);
		memcpy(HOST_MESSAGE.self_def_feild_len ,temp_len, sizeof(HOST_MESSAGE.self_def_feild_len));
	}
	else if ( *result_flag == 0 )
	{
		/*Prepare feild 60.1*/
		memcpy( HOST_MESSAGE.self_def_feild,
				CUP_MESSAGE_REPONSE_CODE_FEILD_60_1,
				CUP_MESSAGE_REPONSE_CODE_FEILD_60_1_LEN);

		/*Prepare feild 60.2*/

		/*Prepare feild 60.2.1 Reserved filled in with 9 as we are not aware of it*/
		if((true == nccup2_Check_If_CUP_MOTO_TRN())||
		   (true == nccup2_Check_If_CUP_RPS_TRN()))
		{
			memcpy(HOST_MESSAGE.self_def_feild + 4,"9",1);
		}
		else
		{
			memcpy(HOST_MESSAGE.self_def_feild + 4,"0",1);
		}

		/*Prepare feild 60.2.2	Terminal Entry Capability
		Value					  POS entry mode
		0							Unknown

		2							Magnetic stripe read capability (02 , 90)

		5							Chip-capable terminal. If the first and second position of
		field 22 is 05 or 95 or 97, this  field must be 5.

		6							Contactless IC card read capability. If the first and second position of
		field 22 is 07 or 91 or 96 or 98, this field must be 6.
		*/

		/*Prepare feild 60.2.3	Chip Condition Code
		This valoue 0 indicate successful chip data read by terminal
		1 or 2 indicate its magnatic read. 
		*/
		strncpy(temp_pos,HOST_MESSAGE.posentry,2);
		temp_posentry=atoi(temp_pos);

		switch(temp_posentry)
		{
			case 02:
			case 90:
			case 06:
			/*Prepare feild 60.2.2*/
			memcpy(HOST_MESSAGE.self_def_feild + 5,"5",1);
			/*Prepare feild 60.2.3*/
			memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
			memcpy(transaction_medium,"1",1);
			break;

			case 05:
			case 95:
			case 97:
			case 80:
			if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
			{
				/*Prepare field 60.2.2*/
				memcpy(HOST_MESSAGE.self_def_feild + 5,"0",1);
				/*Prepare field 60.2.3 */
				memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
			}
			else
			{
				/*Prepare field 60.2.2 */
				memcpy(HOST_MESSAGE.self_def_feild + 5,"5",1);
				/*Prepare field 60.2.3 */
				memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
			}
			memcpy(transaction_medium,"2",1);
			break;

			case 07:
			case 91:
			case 96:
			case 98:
			case 83:
			/*Prepare field 60.2.2 */
			memcpy(HOST_MESSAGE.self_def_feild + 5,"6",1);
			/*Prepare field 60.2.3 */
			memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
			memcpy(transaction_medium,"2",1);
			break;

			case 01:
				/*Prepare field 60.2.2 */
				if((0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)) ||
				   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2)))
			    {
					memcpy(HOST_MESSAGE.self_def_feild + 5,"0",1);
			    }
				else if(true == nccup2_Check_If_CUP_MOTO_TRN())
				{
						if(true == nccup2_is_voice_transaction())
						{
							memcpy(HOST_MESSAGE.self_def_feild + 5,"0",1);
						}
						else
						{
							memcpy(HOST_MESSAGE.self_def_feild + 5,"5",1);
						}
				}
				else if (true == nccup2_Check_If_CUP_RPS_TRN()||
						(true == nccup2_Check_If_CUP_BDO_PAY()))
				{
					memcpy(HOST_MESSAGE.self_def_feild + 5,"0",1);
				}
				else
				{
					if (true == nccup2_is_voice_transaction())
					{
						memcpy(HOST_MESSAGE.self_def_feild + 5,"0",1);
					}
					else
					{
						memcpy(HOST_MESSAGE.self_def_feild + 5,"5",1);
					}

				}

				/*Prepare field 60.2.3 */
				memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
				break;

			default:
			/*Prepare field 60.2.2 */
			memcpy(HOST_MESSAGE.self_def_feild + 5,"0",1);
			/*Prepare field 60.2.3 */
			memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
			memcpy(transaction_medium,"0",1);
			break;
		}

		/*Prepare field 60.2.4 Reserved filled in with 0*/
		memcpy(HOST_MESSAGE.self_def_feild + 7,"0",1);

		/*Prepare field 60.2.2 for Void and Reversal for Magstripe Transaction */
		if( temp_posentry == 05 || 
			 temp_posentry == 12 ||
			 temp_posentry == 01 ||
			 temp_posentry == 80 || 
			 temp_posentry == 95 || 
			 temp_posentry == 97 ||
			 temp_posentry == 07 || 
			 temp_posentry == 91 ||
			 temp_posentry == 96 ||
			 temp_posentry == 98)
 		{
			/* Do nothing Its EMV Trasaction */
 		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE
			)
		{
				/* Its Magstrip Transaction */
			  memcpy(HOST_MESSAGE.self_def_feild + 5,"2",1);
		}
		
	 /* Prepare field 60.2.3 for Reversal and Fallback reversal */
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE )
		{
			if(Auth_Tx.TLF01_details.pos_entry_mode[1]== '8')
			{
				if( temp_posentry == 02 || 	 temp_posentry == 90 )
				{
					memcpy(HOST_MESSAGE.self_def_feild + 5,"5",1);
					memcpy(HOST_MESSAGE.self_def_feild + 6,"2",1);
					memcpy(transaction_medium,"3",1);
				}
				else
				{ 
					/* Normal  Reversal */
					memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
				}
			}
			else
			{ 
				/* Normal  Reversal */
				memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
			}
			
		}
		/*Prepare field 60.2.3 for POS EMV fallback transaction */
		else if(Auth_Tx.TLF01_details.pos_entry_mode[1]== '8')
		{
				if( ( temp_posentry == 02 ||
					  temp_posentry == 90 || 
					  temp_posentry == 01 ))
				{
 					memcpy(HOST_MESSAGE.self_def_feild + 5,"5",1);
 					memcpy(HOST_MESSAGE.self_def_feild + 6,"2",1);
					memcpy(transaction_medium,"3",1);
 				}
				else
				{
					memcpy(HOST_MESSAGE.self_def_feild + 6,"0",1);
				}
 		}

		/*Prepare field 60.2.5 Transaction Channel Type*/
		if( true == nccup2_is_voice_transaction ())
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				/*Mobile phone Voice transaction */
				memcpy(HOST_MESSAGE.self_def_feild + 8,"08", 2);
			}
			else if(true == nccup2_Check_If_CUP_MOTO_TRN())
			{
				/*MOTO Voice transaction */
				memcpy(HOST_MESSAGE.self_def_feild + 8,"00", 2);
			}
			else
			{
				/*PC Voice transaction */
				memcpy(HOST_MESSAGE.self_def_feild + 8,"08", 2);
			}
			
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(HOST_MESSAGE.self_def_feild + 14,"0", 1);
		}
		else if (true == nccup2_Check_If_CUP_RPS_TRN())
		{
			memcpy(HOST_MESSAGE.self_def_feild + 8,"14", 2);
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(HOST_MESSAGE.self_def_feild + 14,"0", 1);
		}
		else if (true == nccup2_Check_If_CUP_BDO_PAY())
		{
			memcpy(HOST_MESSAGE.self_def_feild + 8,"08", 2);
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(HOST_MESSAGE.self_def_feild + 14,"0", 1);
		}
		else
		{
			/*POS-Point Of Sale 03 */
			memcpy(HOST_MESSAGE.self_def_feild + 8,"03", 2);
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(HOST_MESSAGE.self_def_feild + 14,"0", 1);
		}

		/*Prepare feild 60.2.6 Signature-based Credit Card Network Identifier*/
		if(true == nccup2_Check_If_CUP_MOTO_TRN())
		{
			memcpy(HOST_MESSAGE.self_def_feild + 10,"1", 1);
		}
		else
		{
			memcpy(HOST_MESSAGE.self_def_feild + 10,"0", 1);
		}
		/*Prepare feild 60.2.7 Chip Card Authentication Reliability Indicato*/
		memcpy(HOST_MESSAGE.self_def_feild + 11,"0", 1);
		/*Prepare feild 60.2.8	Electronic Commerce Identification*/
		memcpy(HOST_MESSAGE.self_def_feild + 12,"00", 2);
		
#if BDOR_68_REQ_1_UPI /*MobilePOS NFC Tap2Phone Trx*/
		if (true == nccup2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   memcpy( HOST_MESSAGE.self_def_feild + 4,
				   UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
				   UPI_POS_DATA_LEN);
		}
		else if (true == nccup2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
			memcpy( HOST_MESSAGE.self_def_feild + 4,
					UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					UPI_POS_DATA_LEN);
		}
		if(true == nccup2_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   memcpy( HOST_MESSAGE.self_def_feild + 4,
				   UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
				   UPI_POS_DATA_LEN);
		}
		else if (true == nccup2_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
			memcpy( HOST_MESSAGE.self_def_feild + 4,
					UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					UPI_POS_DATA_LEN);
		}
#endif

#if BDOR_68_REQ_3_UPI /* Swiftpass BDO Pay Credit Transactions */
		else if (true == nccup2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
			   memcpy(HOST_MESSAGE.self_def_feild + 4,
					  UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					  UPI_POS_DATA_LEN);
		}
		else if (true == nccup2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
			   memcpy(HOST_MESSAGE.self_def_feild + 4,
					  UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					  UPI_POS_DATA_LEN);
		}
		else if(true == nccup2_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_with_pin())
	    {
		   memcpy(HOST_MESSAGE.self_def_feild + 4,
				  UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				  UPI_POS_DATA_LEN);
	    }
		else if (true == nccup2_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
		   memcpy(HOST_MESSAGE.self_def_feild + 4,
				  UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				  UPI_POS_DATA_LEN);
		}
#endif
#if BDOR_68_REQ_5_UPI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
	else if(true == nccup2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx())
	{
		 memcpy(HOST_MESSAGE.self_def_feild + 4,
				 UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
				 UPI_POS_DATA_LEN);
    }
	else if(true == nccup2_check_if_trx_is_upi_ecommerce_swiftpass_BDOPay_trx ())
	{
		 memcpy(HOST_MESSAGE.self_def_feild + 4,
				 UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
				 UPI_POS_DATA_LEN);
    }
#endif

		/*Prepare feild 60.3*/

		/* We need to keep 00 for length */
		/* Currently F60.3.1 and F60.3.2 shall be filled with 0.*/
		/*Prepare feild 60.3.1 Special Pricing Type*/
		memcpy(HOST_MESSAGE.self_def_feild + 15,"00", 2);
		/*Prepare feild 60.3.2 Special Pricing Level */
		memcpy(HOST_MESSAGE.self_def_feild + 17,"0", 1);
		/*Prepare feild 60.3.3 Minor Unit of Transaction Currency */
		memcpy(HOST_MESSAGE.self_def_feild + 18,"000", 3);
		/*Prepare feild 60.3.4 Partial approval and balance return supported tag */
		memcpy(HOST_MESSAGE.self_def_feild + 21,"0", 1);
		/*Prepare feild 60.3.5 Transaction initiation method  */	
		if((true == nccup2_Check_If_CUP_MOTO_TRN())||
		   (true == nccup2_Check_If_CUP_RPS_TRN()))
		{
			/*3 indicate agent, indicates transactions initiated by merchant on
			cardholder�s behalf who is not present, like Recurring, MOTO.*/
			memcpy(HOST_MESSAGE.self_def_feild + 22,CUP_MOTO_TRN_INITIATION_MODE, 1);
		}
		else
		{
			memcpy(HOST_MESSAGE.self_def_feild + 22,"0", 1);//Attended( POS transction
		}
		/*Prepare feild 60.3.6 Transaction medium  */
		if((true == nccup2_Check_If_CUP_MOTO_TRN())||
		   (true == nccup2_Check_If_CUP_RPS_TRN()))
		{
			memcpy(HOST_MESSAGE.self_def_feild + 23,CUP_CARD_NOT_PRESENT_TRAN,1);
		}
		else
		{
			memcpy(HOST_MESSAGE.self_def_feild + 23,transaction_medium,1);
		}
		/*Prepare feild 60.3.7 IC card application type */
		if((true == nccup2_Check_If_CUP_MOTO_TRN()||
		   (true == nccup2_Check_If_CUP_RPS_TRN())))
		{
			memcpy(HOST_MESSAGE.self_def_feild + 24,"0",1);
		}
		else
		{
			memcpy(HOST_MESSAGE.self_def_feild + 24,"1",1);
		}
		/*Prepare feild 60.3.8 Account Type */
		if((true == nccup2_Check_If_CUP_MOTO_TRN()||
		   (true == nccup2_Check_If_CUP_RPS_TRN())))
		{
			memcpy(HOST_MESSAGE.self_def_feild + 25,"00",2);
		}
		else if (Auth_Tx.TLF01_details.processing_code[2] == '0' ||
		Auth_Tx.TLF01_details.processing_code[2] == '3' ||
		Auth_Tx.TLF01_details.processing_code[2] == '4')
		{
			memcpy(HOST_MESSAGE.self_def_feild + 25,"02",2);
		}
		else
		{
			memcpy(HOST_MESSAGE.self_def_feild + 25,"01",2);

		}

		/*Prepare feild 60.3.9 Card Type */
		memcpy(HOST_MESSAGE.self_def_feild + 27,"0",1);

		/*Prepare feild 60.3.10 Card Category */
		memcpy(HOST_MESSAGE.self_def_feild + 28,"00",2);

		/*Store Len*/
		len = strlen(HOST_MESSAGE.self_def_feild);
		len_60_3 = len - (CUP_DE60_1_SIZE + CUP_DE61_2_SIZE);
		sprintf(temp_len ,"%03d", len);
		memcpy(HOST_MESSAGE.self_def_feild_len ,temp_len, sizeof(HOST_MESSAGE.self_def_feild_len));
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,HOST_MESSAGE.self_def_feild	+ 4 ,11);
		  
	}
	
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_61
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_61 ( pINT result_flag, pCHAR err_buf )
{

	INT  ret_val = true;
	INT  len = 0;
	BYTE temp_len[4] = {0};
	BYTE temp_str[201] = {0};
	BYTE CVN_check_result[2] = {0};
	BYTE PVV_check_result[2] = {0};
	BYTE card_not_present_value[8] = {0};
	BYTE ID_number[23] = {0};
	BYTE ARCQ_auth_result[2] = {0};
	BYTE security_check_value[169] = {0};
	BYTE switch_center_identifier[4]="CUP";
	BYTE card_not_present_check_value[4] = {0};
	BYTE card_not_present_check_result[2] = {0};
	BYTE ARQC_Authentication_result_value[2] = {0};
	BYTE AM_USAGE_BIT_MAP[17]= {0};
	INT resp_source_len=0;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.future_use_2)> 0)
		{
			memcpy(temp_len, Auth_Tx.TLF01_details.product_codes[13].code, CUP_DE61_LEN_SIZE);
			len=atoi(temp_len);
			sprintf( HOST_MESSAGE.card_holder_auth_info_len, "%03d", len );

			if(len > 100)
			{
				memcpy( HOST_MESSAGE.card_holder_auth_info, Auth_Tx.future_use_2, 100 );
				memcpy( HOST_MESSAGE.card_holder_auth_info + 100, Auth_Tx.future_use_3, (len - 100));
			}
			else
			{
				memcpy( HOST_MESSAGE.card_holder_auth_info, Auth_Tx.future_use_2, len );
			}
		}
	}

	else if ( *result_flag == 0 )
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6) &&
				Auth_Tx.TLF01_details.resp_source[0]!='\0')
		{
			/*strncpy(temp_str,Auth_Tx.TLF01_details.product_codes[1].amount,12);
			strncpy(temp_str+12,Auth_Tx.TLF01_details.product_codes[2].amount,12);
			strncpy(temp_str+24,Auth_Tx.TLF01_details.product_codes[8].amount,12);
			strncpy(temp_str+36,Auth_Tx.TLF01_details.product_codes[0].code,4);*/
			resp_source_len=strlen(Auth_Tx.TLF01_details.resp_source);
			strncpy(temp_str,Auth_Tx.TLF01_details.resp_source,resp_source_len);


			len = strlen(temp_str);
			sprintf( temp_len, "%03d", len );
			memcpy(HOST_MESSAGE.card_holder_auth_info_len,temp_len, CUP_DE61_LEN_SIZE);
			memcpy(HOST_MESSAGE.card_holder_auth_info,temp_str,len);

		}
		// Check for MOTO transaction and build Field 61
		if(true == nccup2_Check_If_CUP_MOTO_TRN())
		{
			// Its MOTO transaction coming from POS
			memset(ID_number,0x20,22);
			memset(CVN_check_result,0x20,1);
			memset(PVV_check_result, 0x20, 1);
			memset(ARQC_Authentication_result_value,0x20,1);
			memcpy(AM_USAGE_BIT_MAP,"0000000000000000",16);
			memcpy(card_not_present_check_value,
				   Auth_Tx.TLF01_details.cvc_data,
				   strlen(Auth_Tx.TLF01_details.cvc_data));
			if(strlen(Auth_Tx.TLF01_details.exp_date)> 0)
			{
				// We have received the expiry date.
				memset(&AM_USAGE_BIT_MAP[1],0x31,1);
			}
			if(strlen(card_not_present_check_value)> 0)
			{
				// We have received the CVV, set the bitmap
				memset(&AM_USAGE_BIT_MAP[5],0x31,1);
			}
			else
			{
				// We have not received CVN data, set 3 byte spaces
				memset(card_not_present_check_value,0x20, 3);
			}


			memset(card_not_present_check_result, 0x20, 1);

			memcpy(temp_str,ID_number,22);			//61.1
			strncat(temp_str,CVN_check_result,1);//61.2
			strncat(temp_str,PVV_check_result,1);//61.3
			strncat(temp_str,switch_center_identifier,3);//61.4
			strncat(temp_str,card_not_present_check_value, 3);
			strncat(temp_str,card_not_present_check_result,1);
			strncat(temp_str,ARQC_Authentication_result_value,1);//61.5
			strncat(temp_str,switch_center_identifier,3);//61.6
			strncat(temp_str,CUP_DE61_AM_USAGE_TAG,CUP_DE61_AM_USAGE_TAG_LEN);
			strncat(temp_str,AM_USAGE_BIT_MAP,CUP_DE_61_AM_USAGE_BITMAP_LEN);
			len = strlen(temp_str);
			sprintf( temp_len, "%03d", len );
			memcpy(HOST_MESSAGE.card_holder_auth_info_len,temp_len, CUP_DE61_LEN_SIZE);
			memcpy(HOST_MESSAGE.card_holder_auth_info,temp_str,len);
		}
		else if( (Auth_Tx.TLF01_details.tx_key == AUTH_SALE))
		{
			return( ret_val );

		}

		else if(strlen(Auth_Tx.TLF01_details.cvc_data)> 0)
		{
			/*
			ID number, CVN_check_result and PVV_check_result are kept as spaces.
			*/ 
			memset(ID_number,0x20,22);
			memset(CVN_check_result,0x20,1);
			memset(PVV_check_result, 0x20, 1);
			memset(ARQC_Authentication_result_value,0x20,1);
			memcpy(AM_USAGE_BIT_MAP,"0000000000000000",16);
			memcpy(card_not_present_check_value,
				   Auth_Tx.TLF01_details.cvc_data,
				   strlen(Auth_Tx.TLF01_details.cvc_data));
			if(strlen(Auth_Tx.TLF01_details.exp_date)> 0)
			{
				// We have received the expiry date.
				memset(&AM_USAGE_BIT_MAP[1],0x31,1);
			}
			if(strlen(card_not_present_check_value)> 0)
			{
				// We have received the CVV, set the bitmap
				memset(&AM_USAGE_BIT_MAP[5],0x31,1);
			}
			else
			{
				// We have not received CVN data, set 3 byte spaces
				memset(card_not_present_check_value,0x20, 3);
			}

			memset(card_not_present_check_result, 0x20, 1);
			memcpy(temp_str,ID_number,22);
			strncat(temp_str,CVN_check_result,1);
			strncat(temp_str,PVV_check_result,1);
			strncat(temp_str,switch_center_identifier,3);
			strncat(temp_str,card_not_present_check_value, 3);
			strncat(temp_str,card_not_present_check_result,1);
			strncat(temp_str,ARQC_Authentication_result_value,1);//61.5
			strncat(temp_str,switch_center_identifier,3);//61.6
			strncat(temp_str,CUP_DE61_AM_USAGE_TAG,CUP_DE61_AM_USAGE_TAG_LEN);
			strncat(temp_str,AM_USAGE_BIT_MAP,CUP_DE_61_AM_USAGE_BITMAP_LEN);
			len = strlen(temp_str);
			sprintf( temp_len, "%03d", len );
			memcpy(HOST_MESSAGE.card_holder_auth_info_len,temp_len, CUP_DE61_LEN_SIZE);
			memcpy(HOST_MESSAGE.card_holder_auth_info,temp_str,len);
		}
	}
	
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_62
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_62 ( pINT result_flag, pCHAR err_buf )
{
	INT ret_val= true;
    BYTE temp_len[4]="";
	INT len =0;
	len = strlen(Auth_Tx.TLF01_details.invoice_number);

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{

	}
	else if ( *result_flag == 0 )
	{
		if(len > 0)
		{
			sprintf( temp_len, "%03d", len );
			memcpy ( HOST_MESSAGE.switching_data_len,temp_len, CUP_DE62_LEN_SIZE);
			memcpy ( HOST_MESSAGE.switching_data, Auth_Tx.TLF01_details.invoice_number,len);
		}
	}
	return ret_val;

}


/******************************************************************************
 *
 *  NAME:         build_request_field_70
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_70 ( pINT result_flag, pCHAR err_buf )
{

   INT  ret_val = true;
   INT  len;
   BYTE temp_len[3]="";
   
   if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
	   if(strlen(Auth_Tx.function_code)>0)
	   {
		   memcpy(HOST_MESSAGE.net_manag_info_code, Auth_Tx.function_code, CUP_DE70_SIZE);
		   extendedbit= true;
	   }
	}
   
   //Auth_Tx.function_code
   else if ( *result_flag == 0 )
   {
   		if(strlen(Auth_Tx.function_code)>0)
		{
			memcpy(HOST_MESSAGE.net_manag_info_code, Auth_Tx.function_code, CUP_DE70_SIZE);
			extendedbit= true;
		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_90
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_90 ( pINT result_flag, pCHAR err_buf )
{
	BYTE orig_data[43] = {0};
	BYTE temp_data1[12] = {0};
	BYTE temp_data2[12] = {0};
	INT count = 0;
	INT  ret_val = true;


	/*
	+-------------------------------------------------+
	Feild 90 details
	| BYTES  1- 4:	Original Message Type	 |
	| BYTES  5-10:	STAN of Original message		  |
	| BYTES 11-20:	Trans date & time of original msg |
	| BYTES 21-31:	Acquiring Institution Code		  |
	| BYTES 32-42:	Forwarding Institution Code = '0' |
	+-------------------------------------------------+
	*/
	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{
		if ( strlen(Auth_Tx.TLF01_details.orig_message)> 0)
		{
		//ECHO FLD 90 Data
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, Auth_Tx.TLF01_details.orig_message); 

			strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num);

			strcat (orig_data, Auth_Tx.orig_local_time_date); 

			strncpy(temp_data1, Auth_Tx.orig_acq_inst_id, CUP_MAX_DE32_SIZE);
			Rj_with_lead_zeros(temp_data1,CUP_MAX_DE32_SIZE);
			strcat (orig_data, temp_data1);  

			strncpy(temp_data2,Auth_Tx.amex_pos_entry_mode , CUP_MAX_DE33_SIZE);
			Rj_with_lead_zeros(temp_data2,CUP_MAX_DE33_SIZE);
			strcat (orig_data, temp_data2); 
			memcpy (HOST_MESSAGE.orig_data,orig_data, sizeof(HOST_MESSAGE.orig_data));
			extendedbit= true;
		}
	}

	else if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE || 
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE || 
		Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE)
		{
			//ECHO FLD 90 Data
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, Auth_Tx.TLF01_details.orig_message); 

			strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num);

			strcat (orig_data, Auth_Tx.orig_local_time_date); 

			strncpy(temp_data1, Auth_Tx.orig_acq_inst_id, CUP_MAX_DE32_SIZE);
			Rj_with_lead_zeros(temp_data1,CUP_MAX_DE32_SIZE);
			strcat (orig_data, temp_data1);  

			strncpy(temp_data2,Ncf01_I.forwarding_inst_id_code , CUP_MAX_DE33_SIZE);
			Rj_with_lead_zeros(temp_data2,CUP_MAX_DE33_SIZE);
			strcat (orig_data, temp_data2); 
			memcpy (HOST_MESSAGE.orig_data,orig_data, sizeof(HOST_MESSAGE.orig_data));
			extendedbit= true;
		}

		else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL || 
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT||
		Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE)
		{
			//ECHO FLD 90 Data
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, "0100");

			if(strlen(Auth_Tx.TLF01_details.orig_sys_trace_num) > 0 )
			{
				strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num); 
			}
			else
			{
				strcat (orig_data, Auth_Tx.TLF01_details.sys_trace_audit_num); 
			}

			if(strlen(Auth_Tx.orig_local_time_date) > 0 )
			{
				strcat (orig_data, Auth_Tx.orig_local_time_date); 
			}
			else
			{
				strcat (orig_data, Auth_Tx.TLF01_details.date_yyyymmdd + 4); 
				strcat (orig_data, Auth_Tx.TLF01_details.time_hhmmss); 
			}

			if(strlen(Auth_Tx.orig_acq_inst_id) > 0 )
			{
				strncpy(temp_data1, Auth_Tx.orig_acq_inst_id, CUP_MAX_DE32_SIZE);
			}
			else
			{
				strncpy(temp_data1, Ncf01_I.acquirer_inst_id_code, CUP_MAX_DE32_SIZE);
			}
			Rj_with_lead_zeros(temp_data1,CUP_MAX_DE32_SIZE);
			strcat (orig_data, temp_data1);  

			strncpy(temp_data2,Ncf01_I.forwarding_inst_id_code , CUP_MAX_DE33_SIZE);
			Rj_with_lead_zeros(temp_data2,CUP_MAX_DE33_SIZE);

			strcat (orig_data, temp_data2); 
			extendedbit= true;

			memcpy (HOST_MESSAGE.orig_data,orig_data, sizeof(HOST_MESSAGE.orig_data));

		}

		else if( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))&&
   	 		  (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE) ||
			  (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"73",2) ))
		{
			//ECHO FLD 90 Data
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, "0100");	
		
			if(strlen(Auth_Tx.TLF01_details.orig_sys_trace_num) > 0 )
			{
				strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num); 
			}
			else
			{
				strcat (orig_data, Auth_Tx.TLF01_details.sys_trace_audit_num); 
			}

			if(strlen(Auth_Tx.orig_local_time_date) > 0 )
			{
				strcat (orig_data, Auth_Tx.orig_local_time_date); 
			}
			else
			{
				strcat (orig_data, Auth_Tx.TLF01_details.date_yyyymmdd + 4); 
				strcat (orig_data, Auth_Tx.TLF01_details.time_hhmmss); 
			}

			if(strlen(Auth_Tx.orig_acq_inst_id) > 0 )
			{
				strncpy(temp_data1, Auth_Tx.orig_acq_inst_id, CUP_MAX_DE32_SIZE);
			}
			else
			{
				strncpy(temp_data1, Ncf01_I.acquirer_inst_id_code, CUP_MAX_DE32_SIZE);
			}
			Rj_with_lead_zeros(temp_data1,CUP_MAX_DE32_SIZE);
			strcat (orig_data, temp_data1);  

			strncpy(temp_data2,Ncf01_I.forwarding_inst_id_code , CUP_MAX_DE33_SIZE);
			Rj_with_lead_zeros(temp_data2,CUP_MAX_DE33_SIZE);

			strcat (orig_data, temp_data2); 
			extendedbit= true;

			memcpy (HOST_MESSAGE.orig_data,orig_data, sizeof(HOST_MESSAGE.orig_data));
		}

		else if ( strlen(Auth_Tx.TLF01_details.orig_message)> 0)
		{
			memcpy (orig_data , HOST_MESSAGE.orig_data, sizeof(HOST_MESSAGE.orig_data));
			if((orig_data[1] == '0') && (Auth_Tx.TLF01_details.message_type[1]=='4'))
			{
				strcpy( err_buf, "Invalid data for original data elements ");
				nccup2_log_message( 1, 3, err_buf, "build_request_field_90",1 );

				return false;
			}
			else
			{
				memcpy(orig_data, Auth_Tx.TLF01_details.orig_message, 4);

				/* Since all values after message type in DE90 can be zero,
				* check the stan.  If zero, will need to use DE11 for reversals
				* to look up original transaction.
				*/
				if ( 0 == strcmp(Auth_Tx.TLF01_details.orig_sys_trace_num,"000000"))
				{
					memcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
					Auth_Tx.TLF01_details.sys_trace_audit_num, CUP_DE11_SIZE);
				}
				memcpy(orig_data+4,Auth_Tx.TLF01_details.orig_sys_trace_num,  CUP_DE11_SIZE);
				memcpy(orig_data+10 ,Auth_Tx.orig_local_time_date, CUP_DE7_SIZE);
				memcpy(orig_data+20, Auth_Tx.orig_acq_inst_id,CUP_MAX_DE32_SIZE);

				/* Actually forwarding institution id code */
				memcpy(orig_data+31, Auth_Tx.amex_pos_entry_mode,CUP_MAX_DE33_SIZE);
				extendedbit= true;
			}
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_96
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_96 ( pINT result_flag, pCHAR err_buf )
{

	INT  ret_val = true;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.amex_pos_entry_mode)>0)
		{
			memcpy(HOST_MESSAGE.mess_sec_code, Auth_Tx.amex_pos_entry_mode, CUP_DE96_SIZE);
			extendedbit= true;
		}
	}
	else if ( *result_flag == 0 )
	{
		if(strlen(Auth_Tx.amex_pos_entry_mode)>0)
		{
			memcpy(HOST_MESSAGE.mess_sec_code, Auth_Tx.amex_pos_entry_mode, CUP_DE96_SIZE);
			extendedbit= true;
		}
	}
	return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         build_request_field_100
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_100 ( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len = 0 ;
	BYTE temp_len[3] = {0};
		
	
	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{	
		/* We need to store this data in  Auth_Tx.TLF01_details.product_codes[3].amount
		in nccup module as this data is filled by CUP.
		*/

		/*Find the length*/
		if(strlen(Auth_Tx.TLF01_details.product_codes[3].amount)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.product_codes[3].amount);
			sprintf(temp_len, "%02d", len);
			memcpy(HOST_MESSAGE.recv_ins_id_code_len, temp_len, CUP_DE100_SIZE_LEN);

			/*Store data*/
			memcpy( HOST_MESSAGE.recv_ins_id_code,
			Auth_Tx.TLF01_details.product_codes[3].amount, len);
			extendedbit=true;
		}
	}
	else if( *result_flag == 0 )
	{
		if( Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
			Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION ||
			Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION||
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"03",2)))
		{
			return ret_val;
		}
		else if(strlen(Ncf01_I.acquirer_inst_id_code)> 0)
		{
			memcpy( HOST_MESSAGE.recv_ins_id_code,
					Ncf01_I.acquirer_inst_id_code, strlen(Ncf01_I.acquirer_inst_id_code));
			len = strlen(Ncf01_I.acquirer_inst_id_code);
			sprintf(temp_len, "%02d", len);
			memcpy(HOST_MESSAGE.recv_ins_id_code_len, temp_len, CUP_DE100_SIZE_LEN);
			extendedbit=true;
		}
		else
		{
			  strcpy( err_buf, "Invalid Receiving Institution Identification Code is Empty");
			
		}
	}

	return ret_val;
 }






/******************************************************************************
 *
 *  NAME:         build_request_field_102
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_102 ( pINT result_flag, pCHAR err_buf )
{
	INT ret_val= true;
	return ret_val;
}

/******************************************************************************
 *
 *  NAME:         build_request_field_117
 *
 *  DESCRIPTION:  This function will build the Payment Facilitator from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_117( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  byte_len[4] = {0};
   BYTE CUP_PF_data [16] = {0};

   if ( *result_flag == 0 )
   {
		if(( strlen(Auth_Tx.TLF01_details.product_codes[7].quantity)> 0 ) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",6)))
		{
			// DE 117 LENGTH
			memcpy(HOST_MESSAGE.cup_payFacl_len,
				   Auth_Tx.TLF01_details.product_codes[7].quantity,CUP_DE117_LEN);

			// DE117 sub filed Tag
			memcpy(HOST_MESSAGE.cup_payFacl,
				   Auth_Tx.TLF01_details.product_codes[7].code,
				   CUP_DE117_SUBFIELD_TAG_LEN);

			// DE117 sub filed Length
			strncpy ( CUP_PF_data,
					  Auth_Tx.TLF01_details.product_codes[7].amount ,
					 (sizeof (Auth_Tx.TLF01_details.product_codes[7].amount)- 1));
			len = atoi(CUP_PF_data);
			sprintf( byte_len, "%03d", len );
			memcpy(HOST_MESSAGE.cup_payFacl + CUP_DE117_SUBFIELD_BYTE_LEN_POS,
					byte_len , CUP_DE117_SUBFIELD_BYTE_LEN);

			memcpy( HOST_MESSAGE.cup_payFacl + CUP_DE117_PAYMENT_FACILTATOR_POS,
					Auth_Tx.TLF01_details.product_codes[7].amount,len);
		}

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_121
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_121 ( pINT result_flag, pCHAR err_buf )
{
	INT  ret_val = true;
	INT  len = 0;
	BYTE temp_len[4] = {0};

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
	//	if(true == isalphanum(Auth_Tx.future_use_5)>0)
		if(strlen(Auth_Tx.iss_inst_id_len)>0)
		{
			/*store the length*/
			memcpy(temp_len,Auth_Tx.iss_inst_id_len,CUP_DE121_SIZE_LEN);
			len = atoi(temp_len);
			memcpy(HOST_MESSAGE.cup_resv_len,temp_len,CUP_DE121_SIZE_LEN);

			strncpy(HOST_MESSAGE.cup_resv, Auth_Tx.future_use_5, len);
			extendedbit = true;
		}
	}
		
	else if ( *result_flag == 0 )
	{
		/* These two feild of Auth tx will be used to store these data.
		forwarding_inst_id_len --> Store the variable length
		future_use_5				 ---> store the data   
		These feild value should be feild in nccup module 
		when message is coming fron nccup   */
		
	  	if(strlen(Auth_Tx.iss_inst_id_len)>0)
	  	{
	  		/*store the length*/
			memcpy(temp_len,Auth_Tx.iss_inst_id_len,CUP_DE121_SIZE_LEN);
			len = atoi(temp_len);
			memcpy(HOST_MESSAGE.cup_resv_len,temp_len,CUP_DE121_SIZE_LEN);

			memcpy(HOST_MESSAGE.cup_resv, Auth_Tx.future_use_5, len);
			extendedbit = true;
  	 	}
    }
	return ret_val;
}

/******************************************************************************
 *
 *  NAME:         build_request_field_122
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_122 ( pINT result_flag, pCHAR err_buf )
{
	INT ret_val = true;
	return ret_val;


}






/******************************************************************************
 *
 *  NAME:         build_request_field_123
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_123 ( pINT result_flag, pCHAR err_buf )
{

    INT len = 0;
	INT ret_val = true;
	BYTE temp_len[2] = {0};

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.int_net_facility_data)>0)
		{
			memcpy( HOST_MESSAGE.issu_inst_resv_len,Auth_Tx.int_net_facility_data, 
					CUP_DE123_SIZE_LEN);
				memcpy(temp_len,Auth_Tx.int_net_facility_data, CUP_DE123_SIZE_LEN);
				len= atoi(temp_len);
				memcpy( HOST_MESSAGE.issu_inst_resv, Auth_Tx.int_net_facility_data +CUP_DE123_SIZE_LEN, len);
				extendedbit = true;
		}
	}
	
	else if ( *result_flag == 0 )
	{
		if(strlen(Auth_Tx.int_net_facility_data)>0)
		{
			memcpy( HOST_MESSAGE.issu_inst_resv_len,Auth_Tx.int_net_facility_data, 
					CUP_DE123_SIZE_LEN);
			memcpy(temp_len,Auth_Tx.int_net_facility_data, CUP_DE123_SIZE_LEN);
			len= atoi(temp_len);
			memcpy( HOST_MESSAGE.issu_inst_resv, Auth_Tx.int_net_facility_data +CUP_DE123_SIZE_LEN, len);
			extendedbit = true;
		}
	}
	return ret_val;
}


INT build_request_field_125 ( pINT result_flag, pCHAR err_buf )
{

    char len[3] = {0};
	INT ret_val = true;
	BYTE temp_len[2] = {0};
	int DE125_len=0;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if( Auth_Tx.TLF01_details.product_codes[13].amount[0]!= '\0' &&
			Auth_Tx.TLF01_details.product_codes[18].amount[0]!='\0'  &&
			Auth_Tx.TLF01_details.product_codes[19].amount[0]!='\0')
		{

			strncpy(HOST_MESSAGE.ds_transactionID,Auth_Tx.TLF01_details.product_codes[12].amount,7);
			strncat(HOST_MESSAGE.ds_transactionID,Auth_Tx.TLF01_details.product_codes[13].amount,12);
			strncat(HOST_MESSAGE.ds_transactionID,Auth_Tx.TLF01_details.product_codes[18].amount,12);
			strncat(HOST_MESSAGE.ds_transactionID,Auth_Tx.TLF01_details.product_codes[19].amount,12);

			DE125_len=strlen(HOST_MESSAGE.ds_transactionID);
			sprintf(len, "%03d", DE125_len );
			strncpy(HOST_MESSAGE.ds_transactionID_len,len,3);
		}
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6)&&
			Auth_Tx.TLF01_details.product_codes[15].code[0]!= '\0' )
	{
		strncpy(HOST_MESSAGE.ds_transactionID,DE125_USAGEIP,9); //USAGE IP
		strncat(HOST_MESSAGE.ds_transactionID+9,Auth_Tx.TLF01_details.product_codes[15].code,3);

		DE125_len=strlen(HOST_MESSAGE.ds_transactionID);
		sprintf(len, "%03d", DE125_len );
		strncpy(HOST_MESSAGE.ds_transactionID_len,len,3);
	}


	return ret_val;
}

/******************************************************************************
 *
 *  NAME:         build_request_field_128
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT build_request_field_128 ( pINT result_flag, pCHAR err_buf )
{

	INT   ret_val = true;
	INT   len = 0;

	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
	{
		if(strlen(Auth_Tx.TLF01_details.product_codes[0].amount)>0)
		{
			memcpy(HOST_MESSAGE.mess_auth_code,
				   Auth_Tx.TLF01_details.product_codes[0].amount,
				   CUP_DE128_SIZE);
			extendedbit=true;
		}
	}
	
    else if ( *result_flag == 0 )
    {
    	if(Auth_Tx.TLF01_details.product_codes[0].amount[0]!=0x00)
    	{
    		memcpy(HOST_MESSAGE.mess_auth_code,
				   Auth_Tx.TLF01_details.product_codes[0].amount,
				   CUP_DE128_SIZE);
			extendedbit=true;
		}
    }
	return ret_val;
}


/******************************************************************************
 *
 *  NAME:         build_response_field_2
 *
 *  DESCRIPTION:  This function will move the card number to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits. This is an echo from the request.
 *                Therefore, there is no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_2( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;

   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.TLF01_details.card_num );
      if ( len == 0 )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf,
                "Unable to build response message to host. No card number." );
      }
      else
      {
         sprintf( HOST_MESSAGE.pan_len, "%02d", len );
         memset( HOST_MESSAGE.pan, '0', CUP_MAX_DE2_SIZE );
         memcpy( HOST_MESSAGE.pan, Auth_Tx.TLF01_details.card_num, len );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_2
 *
 *  DESCRIPTION:  This function will move the card number from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_2( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0;
   BYTE tem_len[3] = {0};

   if ( *result_flag == 0 )
   {
      /* First get the length of the card number. */
      memcpy( Auth_Tx.TLF01_details.card_num_len,
              HOST_MESSAGE.pan_len, 
			  CUP_DE2_LEN_SIZE );
	  
	  memcpy( tem_len,Auth_Tx.TLF01_details.card_num_len,
			  CUP_DE2_LEN_SIZE);
	  
      len = atoi(tem_len);

      if ( len <= CUP_MAX_DE2_SIZE )
      {
         memcpy( Auth_Tx.TLF01_details.card_num, HOST_MESSAGE.pan, len );
         if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.card_num)))
         {
            *result_flag = 1;
            sprintf( err_buf, "Non-numeric pan (field 2): %s,",
                     Auth_Tx.TLF01_details.card_num );
         }
      }
      else
      {
         sprintf( err_buf, "Invalid length of card number (field 2): %d,", len );
         ret_val = false;
         *result_flag = 1;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_3
 *
 *  DESCRIPTION:  This function will move the processing code from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Processing code is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_3( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		 if(strlen(HOST_MESSAGE.proccode)> 0)
   		 {
			memcpy( Auth_Tx.TLF01_details.processing_code,
       	 		  	HOST_MESSAGE.proccode, 
					CUP_DE3_SIZE );
			 if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.processing_code)))
			 {
	   			*result_flag = 1;
	   			sprintf( err_buf,
	           			 "Non-numeric processing code (field 3): %s,",
	           			 Auth_Tx.TLF01_details.processing_code );
			 }
		 }
 	  }
    return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_4
 *
 *  DESCRIPTION:  This function will move the amount from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_4( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.amount) > 0)
   		{
   		    memcpy( Auth_Tx.TLF01_details.total_amount,
        	        HOST_MESSAGE.amount, 
					CUP_DE4_SIZE );
			  
		  	if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.total_amount)))
      	  	{
         			*result_flag = 1;
         		  	sprintf( err_buf,
                 	         "Non-numeric amount (field 4): %s,",
		                      Auth_Tx.TLF01_details.total_amount );
	      	}

   		}
   	}
   
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         move_field_5
 *
 *  DESCRIPTION:  This function will move the reconciliation amount from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_5( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.setl_amount) > 0)
      {
      	 	memcpy( Auth_Tx.TLF01_details.settlement_total,
      	 			HOST_MESSAGE.setl_amount,
					CUP_DE5_SIZE );

			 if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.settlement_total)))
			 	 {
					*result_flag = 1;
		 			sprintf( err_buf,
		  					 "Non-numeric amount (field 5): %s,",
		   				 	 Auth_Tx.TLF01_details.settlement_total );
			 	 }
		
	     }
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_6
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing amount from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_6( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.ch_bill_amt) > 0)
      {
      	 	memcpy( Auth_Tx.ch_billing_amt,
           		  	HOST_MESSAGE.ch_bill_amt, 
					CUP_DE6_SIZE );
	
			 if (false ==(ret_val = isnum(Auth_Tx.ch_billing_amt)))
      	 	 {
        		*result_flag = 1;
         		sprintf( err_buf,
          				 "Non-numeric Cardholder Billing (field 6): %s,",
           				  Auth_Tx.reconciliation_amount );
      	 	 }
      }
    }
   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_7
 *
 *  DESCRIPTION:  This function will move the transmission time from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Transmission time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20];
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.gmt_time) > 0 )
   		{
	       	 memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
        	    	 HOST_MESSAGE.gmt_time, 
					 CUP_DE7_SIZE );
			 
			 ret_val = isnum(Auth_Tx.TLF01_details.transmission_timestamp);
         	 if ( ret_val == false )
         	 {
            	*result_flag = 1;
           		 sprintf( err_buf,
               		    "Non-numeric Transmission Timestamp (field 7): %s,",
                   		 Auth_Tx.TLF01_details.transmission_timestamp );
         	}
         	else if ( false == validate_date_time(Auth_Tx.TLF01_details.transmission_timestamp))
         	{
            	ret_val = false;
            	*result_flag = 1;
            	sprintf( err_buf,
               		    "Invalid Transmission Timestamp (field 7): %s,",
                   		 Auth_Tx.TLF01_details.transmission_timestamp );
         	}
		}
   }
    
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_9
 *
 *  DESCRIPTION:  This function will move the Settlement Conversion Rate from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Transmission time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_9( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20];
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.setl_conv_rate) > 0)
   	  {
      		memcpy( Auth_Tx.TLF01_details.conversion_rate,
					HOST_MESSAGE.setl_conv_rate,
					CUP_DE9_SIZE );
			ret_val = isnum(Auth_Tx.TLF01_details.conversion_rate);
			if ( ret_val == false )
        	{
           		 *result_flag = 1;
           		  sprintf( err_buf,
               		  	   "Non-numeric Settlement Conversion Rate (field 9): %s,",
                   		   Auth_Tx.TLF01_details.conversion_rate );
         	}
   	  	}
     }
   
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_10	
 *
 *  DESCRIPTION:  This function will move the  Cardholder Billing Conversion Rate from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Transmission time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_10( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20];
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.conv_rate_chb) > 0)
   		{
	   		memcpy( Auth_Tx.ch_billing_conv_rate, 
					HOST_MESSAGE.conv_rate_chb,
					CUP_DE10_SIZE );
			ret_val = isnum(Auth_Tx.ch_billing_conv_rate);
			if ( ret_val == false )
         	{
           		 *result_flag = 1;
           		 sprintf( err_buf,
               	 	   	 "Non-numeric Cardholder Billing  Conversion Rate (field 10): %s,",
                   		  Auth_Tx.ch_billing_conv_rate );
         	}
   		}
     }
   
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_11
 *
 *  DESCRIPTION:  This function will move the system trace number from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_11( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.stan) > 0)
   		{
   			memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
            	    HOST_MESSAGE.stan, 
					CUP_DE11_SIZE );
			ret_val = isnum(Auth_Tx.TLF01_details.sys_trace_audit_num);
      		if ( ret_val == false )
      		{
        		*result_flag = 1;
         	 	sprintf( err_buf,
               			 "Non-numeric System Trace Audit Number (field 11): %s,",
               			 Auth_Tx.TLF01_details.sys_trace_audit_num );
      		}
		}
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_12
 *
 *  DESCRIPTION:  This function will move the transaction time from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  Transaction time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT move_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  time[7] = "";
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.time)>0 )
   	  {
		  /* Time */
	  		memcpy( Auth_Tx.TLF01_details.time_hhmmss,
        		    HOST_MESSAGE.time, CUP_HHMMSS_LEN);
		  
  			memcpy(time,Auth_Tx.TLF01_details.time_hhmmss,CUP_HHMMSS_LEN);

   	  		ret_val = isnum(time);
      		if ( ret_val == false )
   			{
       			*result_flag = 1;
       			sprintf( err_buf,
               			 "Non-numeric transaction time (field 12): %s,",
               			 time );
   			}
			else if ( false == validate_time(time))
    		{
       	 		ret_val = false;
	    	    *result_flag = 1;
	        	 sprintf( err_buf,
       	    		     "Invalid transaction  time (field 12): %s,",
           	    		  time );
      		}
			
   	  	}
   	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_13
 *
 *  DESCRIPTION:  This function will move the transaction date from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Transaction time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT move_field_13( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  date[5] = "";
   INT   len     = 0;
   BYTE time_date[25]="";
   

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.date) > 0)
   		{
   			ptetime_get_timestamp( time_date );
			/*Store system date year YYYY */
			memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd, 
	    	      	  		time_date, CUP_YYYY_LEN);
   			/* Date  MMDD Format*/
			  memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd[CUP_MMDD_START_POS],
	    	      	  HOST_MESSAGE.date, CUP_MMDD_LEN);
	  
	 		  memcpy( date,Auth_Tx.TLF01_details.date_yyyymmdd + 
	 				  CUP_MMDD_START_POS,
					  CUP_MMDD_LEN);
			  
              ret_val = isnum(date);
		      if ( ret_val == false )
      		  {
			      *result_flag = 1;
         		  sprintf( err_buf,
              		 	  "Non-numeric transaction date (field 13): %s,",
                  		   date );
		      }
			  else if ( false == validate_date_mmdd(date))
			  {
			      ret_val = false;
        		 *result_flag = 1;
				  sprintf( err_buf,
               			 "Invalid transaction date (field 13): %s,",
			              date );
      		}
		}
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_14
 *
 *  DESCRIPTION:  This function will move the Expire date from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Expire date is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma 
 *
 ******************************************************************************/
INT move_field_14( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	  if(strlen(HOST_MESSAGE.date_expd) > 0)
	  {
		  memcpy( Auth_Tx.TLF01_details.exp_date,
				  HOST_MESSAGE.date_expd, 
				  CUP_DE14_SIZE);
		   ret_val = isnum(Auth_Tx.TLF01_details.exp_date);
		   if ( ret_val == false )
      	   {
		        *result_flag = 1;
        		 sprintf( err_buf,
                		 "Non-numeric Expire date (field 14): %s,",
                  		  Auth_Tx.TLF01_details.exp_date );
      		}
	  	}
   	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_15
 *
 *  DESCRIPTION:  This function will move the Settlement date from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Expire date is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma 
 *
 ******************************************************************************/
INT move_field_15( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.date_settle) > 0)
   	  {
	   	   	  /* Settlement Date */
		      memcpy( Auth_Tx.TLF01_details.settlement_date,
				      HOST_MESSAGE.date_settle,
					  CUP_DE15_SIZE );
			  ret_val = isnum(Auth_Tx.TLF01_details.settlement_date);
	      	  if ( ret_val == false )
	      	  {
			     *result_flag = 1;
	        	 sprintf( err_buf,
	              		 "Non-numeric Settlement date (field 15): %s,",
	               	 	  Auth_Tx.TLF01_details.settlement_date);
	      	  }
		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_16
 *
 *  DESCRIPTION:  This function will move the Conversion date from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Expire date is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma 
 *
 ******************************************************************************/
INT move_field_16( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if (strlen(HOST_MESSAGE.conv_date)>0)
   	  {
	   	  
	  	  /* Settlement Date */
	      memcpy( Auth_Tx.conv_date,
			      HOST_MESSAGE.conv_date,
				  CUP_DE16_SIZE );
		  ret_val = isnum(Auth_Tx.conv_date);
      	  if ( ret_val == false )
      	  {
		    *result_flag = 1;
        	 sprintf( err_buf,
               		 "Non-numeric Conversion date (field 16): %s,",
               	 	  Auth_Tx.conv_date);
      	  }
   	  	}
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_18
 *
 *  DESCRIPTION:  This function will move the Merchnat Type  from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_18( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(strlen(HOST_MESSAGE.merch_type)> 0)
   	   {
   	   		if ( Auth_Tx.MCF01_details.voice_category_code[0] != 0x00 )
   	   		{
   				memcpy( Auth_Tx.MCF01_details.voice_category_code,
	    		   		HOST_MESSAGE.merch_type, 
						CUP_DE18_SIZE );
				
				ret_val = isnum(Auth_Tx.MCF01_details.voice_category_code);
  		  		if ( ret_val == false )
  		  		{
	       	  		*result_flag = 1;
    			 	sprintf( err_buf,
            				 "Non-numeric Merchant TypeV (field 18) : %s,",
             		 		  Auth_Tx.MCF01_details.voice_category_code);
  		  		}
			}
				
			else
			{
				memcpy( Auth_Tx.TLF01_details.category_code,
      				  	HOST_MESSAGE.merch_type, 
						CUP_DE18_SIZE );
				ret_val =isnum(Auth_Tx.TLF01_details.category_code);
  		  		if ( ret_val == false )
  		  		{
	       	  		*result_flag = 1;
    			 	sprintf( err_buf,
            				 "Non-numeric Merchant Type (field 18): %s,",
             		 		  Auth_Tx.TLF01_details.category_code);
  		  		}
			}
			
   	   	}

	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_19
 *
 *  DESCRIPTION:  This function will move the Acquirer institution country code 
 *				  from the Host. Message structure according to BDO Cadencie host 
 *				  formatting specifications.  System trace number is numeric, fixed  length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_19( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(strlen(HOST_MESSAGE.ctry_code) > 0)
   	   {
	   		memcpy( Auth_Tx.country_code,
            	    HOST_MESSAGE.ctry_code, 
					CUP_DE19_SIZE );
			
			ret_val = isnum(Auth_Tx.country_code);
      		if ( ret_val == false )
      		{
		      	*result_flag = 1;
        		 sprintf( err_buf,
               			 "Non-numeric Merchant Country Code (field 19): %s,",
               		 	  Auth_Tx.conv_date);
      		}
		}
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_22
 *
 *  DESCRIPTION:  This function will move the POS Entry Mode from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  POS Entry Mode is numeric, fixed
 *                length.  For BDO Cadencie host host, it will always be the
 *                same value.
 *
 *                Prepend '0' to put it into Hypercom ISO format.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.posentry) > 0)
   		{
      		memcpy( Auth_Tx.TLF01_details.pos_entry_mode, 
				    HOST_MESSAGE.posentry, 
					CUP_DE22_SIZE );
			ret_val = isnum(Auth_Tx.TLF01_details.pos_entry_mode);
	      	if ( ret_val == false )
    		{
			     *result_flag = 1;
			      sprintf( err_buf,
                	 	  "Non-numeric POS Entry Mode (field 22): %s,",
               	  		  Auth_Tx.TLF01_details.pos_entry_mode );
      		}
			
		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_23
 *
 *  DESCRIPTION:  This function will move the Card Sequence Number from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications. Card Sequence Number is numeric, fixed
 *                length.  For BDO Cadencie host host, it will always be the
 *                same value.
 *
 *                Prepend '0' to put it into Hypercom ISO format.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Kumar Verma
 *
 ******************************************************************************/
INT move_field_23( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   	    if(strlen(HOST_MESSAGE.card_seq_num)> 0)
   	    {
   	    	memcpy( Auth_Tx.EMV_details.pan_sequence_number , 
				    HOST_MESSAGE.card_seq_num, 
					CUP_DE23_SIZE );
				
			ret_val = isnum(Auth_Tx.EMV_details.pan_sequence_number);
		    if ( ret_val == false )
	    	{
			      *result_flag = 1;
			      sprintf( err_buf,
	               	 	  "Non-numeric Card Sequence Number (field 23): %s,",
	              	  		  Auth_Tx.EMV_details.pan_sequence_number );
	      	}
   	    }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_25
 *
 *  DESCRIPTION: This function will move the Point of Service Condition Code
 *				from the Host. Message structure according to Equitable formatting
 *                         specifications. Point of Service Condition Code is numeric, fixed
 *                        length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host  specific specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.pos_condcd) > 0)
   	  {
			memcpy( Auth_Tx.TLF01_details.pos_condition_code,
				    HOST_MESSAGE.pos_condcd, 
					CUP_DE25_SIZE);

		    ret_val = isnum(Auth_Tx.TLF01_details.pos_condition_code);
			if ( ret_val == false )
      		{
			   *result_flag = 1;
			    sprintf( err_buf,
            		     "Non-numeric POS Condition Code (field 25): %s,",
			              Auth_Tx.TLF01_details.pos_condition_code );
      		}
		}
   	}
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         move_field_26
 *
 *  DESCRIPTION:  This function will move the Point of Service PIN Capture Code from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_26( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(strlen(HOST_MESSAGE.pos_pincode)> 0)
   	   {
	   	   memcpy( Auth_Tx.TLF01_details.product_codes[17].code,
				   HOST_MESSAGE.pos_pincode,
				   CUP_DE26_SIZE );
				
			ret_val = isnum(Auth_Tx.TLF01_details.product_codes[17].code);
			if ( ret_val == false )
			{
				*result_flag = 1;
				sprintf( err_buf,
					 	"Non-numeric Point of Service PIN Capture Code (field 26): %s,",
						 Auth_Tx.TLF01_details.product_codes[17].code );
			}
   	   	}
     }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_28
 *
 *  DESCRIPTION:  This function will move the Transaction Fee Amount from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_28( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(strlen(HOST_MESSAGE.tran_fee_amt) > 0)
   	   {
	   	   memcpy( Auth_Tx.TLF01_details.tran_fee_amount,
				   HOST_MESSAGE.tran_fee_amt +1, 
				   CUP_DE28_SIZE );
			
			ret_val = isalphanum(Auth_Tx.TLF01_details.tran_fee_amount);
			if ( ret_val == false )
			{
				*result_flag = 1;
				sprintf( err_buf,
					 	"Non- alpha numeric Transaction Fee Amount (field 28): %s,",
						 Auth_Tx.TLF01_details.tran_fee_amount );
			}
   	   	}
     }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_32
 *
 *  DESCRIPTION:  This function will move the Acquiring Inst ID from the Host
 *                Message structure according to Transaction Fee Amount formatting
 *                specifications.  Acquiring Inst ID is numeric, variable
 *                length up to 2 digits. .
 *
 *  INPUTS:       Host_Message - Transaction Fee Amount specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_32( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0;
   CHAR  len_str[3] = {0};

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.acq_inst_len) > 0)
      {
	   	  /* First get the length. */
	      memcpy( len_str, HOST_MESSAGE.acq_inst_len, CUP_DE32_SIZE);
				
		  len = atoi( len_str );
	      if ( len <= CUP_MAX_DE32_SIZE)
	      {
	      	 	/* Now copy the Id into Auth_Tx and check it. */
				memcpy(Auth_Tx.acq_inst_id_len, HOST_MESSAGE.acq_inst_len, CUP_DE32_SIZE);
		        memcpy( Auth_Tx.acq_inst_id_cd, HOST_MESSAGE.acq_inst_id, len );
	         
	            if (false ==(ret_val = isalphanum(Auth_Tx.acq_inst_id_cd)))
	            {
	        		*result_flag = 1;
			        sprintf( err_buf,
	        	            "Non-numeric acquiring inst. Id (field 32): %s,",
	               		     Auth_Tx.acq_inst_id_cd );
	            }

	    	   /* Copy it into TLF01 for database reference. */
	    	   memcpy( Auth_Tx.TLF01_details.acquiring_id,
	             	   HOST_MESSAGE.acq_inst_id,
	             	   len );
	  		}
	 		else
	  		{
	     		*result_flag = 1;
				ret_val = false;
	     		sprintf( err_buf, "Invalid length of acquiring inst. Id: %c %c,",
	            	  	HOST_MESSAGE.acq_inst_len[0],
	              		HOST_MESSAGE.acq_inst_len[1] );
	     	}
      	}
   	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_33
 *
 *  DESCRIPTION:  This function will move the Forwarding Inst ID from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Acquiring Inst ID is numeric, variable
 *                length up to 2 digits.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_33( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0 ;
   CHAR  len_str[3] = {0};

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.fwd_inst_len)> 0)
   	  {
	      /* First get the length. */
	      memcpy( len_str, HOST_MESSAGE.fwd_inst_len, CUP_DE33_SIZE);
	      len = atoi( len_str );
	      if ( len <= CUP_MAX_DE33_SIZE )
	      {
	         /* Now copy the Id into Auth_Tx and check it. */
			 memcpy( Auth_Tx.forwarding_inst_id_len, HOST_MESSAGE.fwd_inst_len,CUP_DE33_SIZE);
	         memcpy( Auth_Tx.forwarding_inst_id_cd, HOST_MESSAGE.fwd_inst_id, len );
	         if (false ==(ret_val = isalphanum(Auth_Tx.forwarding_inst_id_cd)))
	         {
	            *result_flag = 1;
	            sprintf( err_buf,
	                    "Non-alphanumeric acquiring inst. Id (field 33): %s,",
	                     Auth_Tx.forwarding_inst_id_cd );
	         }
	      }
		  else
		  {
			  *result_flag = 1;
		       sprintf( err_buf,
					  "Invalid acquiring inst. Id length (field 33): %c %c,",
					   HOST_MESSAGE.fwd_inst_len[0],
					   HOST_MESSAGE.fwd_inst_len[1]);
		  }
   	  	}
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_35
 *
 *  DESCRIPTION:  This function will move the Forwarding Inst ID from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Acquiring Inst ID is numeric, variable
 *                length up to 2 digits.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_35( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0;
   CHAR  len_str[3] = {0};

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.track2)> 0)
   	  {
	      /* First get the length. */
	      memcpy( len_str, HOST_MESSAGE.track2_len, CUP_DE35_LEN_SIZE);
	      len = atoi( len_str );
	      if ( len <= CUP_MAX_DE35_SIZE )
	      {
	         /* Now copy the length Id into Auth_Tx and check it. */
 	         memcpy( Auth_Tx.TLF01_details.track2  , HOST_MESSAGE.track2, len);
	      }
		  else
		  {
			  *result_flag = 1;
		       sprintf( err_buf,
					  "Invalid tracke 2 length (field 35): %c %c,",
					   HOST_MESSAGE.track2_len[0],
					   HOST_MESSAGE.track2_len[1]);
		  }
   	  	}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_37
 *
 *  DESCRIPTION:  This function will move the retrieval reference number from the Host
 *                Message structure according to  BDO Cadencie host  formatting
 *                specifications.  System trace number is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_37( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(strlen(HOST_MESSAGE.rrn)> 0)
   	  {
		  memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
			 	   HOST_MESSAGE.rrn, CUP_DE37_SIZE );
			  
		  ret_val = isalphanum_space(Auth_Tx.TLF01_details.retrieval_ref_num);
		  for( len = 0; len <CUP_DE37_SIZE; len++)
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
						Auth_Tx.TLF01_details.retrieval_ref_num );
		  }
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         move_field_38
 *
 *  DESCRIPTION:  This function will move the Authorization Number from Host
 *                Message structure according to  BDO Cadencie host  formatting
 *                specifications.  Auth Number is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT move_field_38( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   CHAR error_msg[256]="";

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.authcode)> 0 )
      {
	      	memcpy( Auth_Tx.TLF01_details.auth_number,
	        		HOST_MESSAGE.authcode, CUP_DE38_SIZE );
			
	  		ret_val = isalphanum(Auth_Tx.TLF01_details.auth_number);
	  		if ( ret_val == false )
	  		{

				/*TF Phani : If RC is not " 00" , still proceed with tx*/
				if(strcmp(HOST_MESSAGE.responsecd,"00") != 0)
				{	
					sprintf( error_msg,
	            		 "Non-alphanumeric Auth Number (field 38) : %s,",
	              		 Auth_Tx.TLF01_details.auth_number );
					nccup2_log_message( 1, 2, error_msg, "move_field_38",1 );
				}
				memset( Auth_Tx.TLF01_details.auth_number,0x00, CUP_DE38_SIZE );
				ret_val= true;
	  		}
   		}
   	}
  return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_39
 *
 *  DESCRIPTION:  This function will move the Response Code from the Host
 *                Message structure according to  BDO Cadencie host  formatting
 *                specifications.  Response Code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_39( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0; 
   CHAR error_msg[256]="";

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.responsecd)> 0)
      {
   	      memcpy( Auth_Tx.TLF01_details.response_code, HOST_MESSAGE.responsecd, CUP_DE39_SIZE );
     	  ret_val = isalphanum(Auth_Tx.TLF01_details.response_code);

      	  if ( ret_val == false )
      	  {
		       sprintf( error_msg,
		                "Non-alphanumeric response code (field 39) : %s,",
		                 Auth_Tx.TLF01_details.response_code);
				   nccup2_log_message( 1, 2, error_msg, "move_field_39",1 );
		  }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_41
 *
 *  DESCRIPTION:  This function will move the terminal ID from the Host
 *                Message structure according to  BDO Cadencie host  formatting
 *                specifications.  Terminal ID is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host  specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT move_field_41( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.termid) >0 )
   		{
   		    memcpy( Auth_Tx.TLF01_details.terminal_id,
              		HOST_MESSAGE.termid, CUP_DE41_SIZE );
 		}
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_42
 *
 *  DESCRIPTION:  This function will move the Merchant ID from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Merchant ID is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_42( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.cardaccid)> 0)
   		{
	   		memcpy( Auth_Tx.TLF01_details.merchant_id,
             		HOST_MESSAGE.cardaccid, CUP_DE42_SIZE );
		}
   	}
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_43
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Card Acceptor Name/Location is alpha numrical specail, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_43( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.cardaccname) > 0)
   		{
			
		   /*
        	   Sub-field    Position    Length                      Feild details
	              1             1            25               Merchant name or ATM location (padding with blanks)
	              2            26           12               City�s name (padding with blanks)
	              3            38            3                Country/region Code, adopting the coding   */
      		memcpy(  Auth_Tx.TLF01_details.card_holder_name,
			 		 HOST_MESSAGE.cardaccname,
					 CUP_MER_NAME_LEN_FEILF_43 );// Max 25 Byte
		 
		 	memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk,
				    HOST_MESSAGE.cardaccname+25,
					CUP_CITY_LEN_FEILD_43 );// Max 12 Byte
		 
         	memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk+13,
					HOST_MESSAGE.cardaccname+37,
					CUP_COUNTY_CODE_LEN_FEILD_43 );// Max 3 Byte
   		}
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_44
 *
 *  DESCRIPTION:  This function will move the Additional Response Data from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_44( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {  
		if(strlen(HOST_MESSAGE.addlrspdata) > 0)
		{
	    	/*Based on value set in nccup/dcpiso we have to update this function*/	
			/* data present in Auth_Tx.acct_id_1 is LLVAR*/

			/* 1st find the length */
			memcpy(temp_len,HOST_MESSAGE.addl_rsp_len,CUP_DE44_LEN_SIZE);
			len = atoi (temp_len);
			
			if(len <= CUP_MAX_DE44_SIZE) // validate data length
			{
				/* Copy length into  Auth_Tx.acct_id_1 */
				memcpy(Auth_Tx.acct_id_1,HOST_MESSAGE.addl_rsp_len,CUP_DE44_LEN_SIZE);
				
				/* Store additional response data into host message */
				memcpy(Auth_Tx.acct_id_1 + CUP_DE44_LEN_SIZE ,HOST_MESSAGE.addlrspdata,len);// Store data after length
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Additional Response Data length (field 44) : %c %c,",
						 HOST_MESSAGE.addl_rsp_len[0],
						 HOST_MESSAGE.addl_rsp_len[1]);

			}
		}
   	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_45
 *
 *  DESCRIPTION:  This function will move the Track 1 Data from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_45( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};


   if ( *result_flag == 0 )
   {  
		if(strlen(HOST_MESSAGE.track1) > 0)
		{
	    	/* 1st find the length */
			memcpy(temp_len,HOST_MESSAGE.track1_len,CUP_DE45_LEN_SIZE);
			len = atoi (temp_len);
			
			if(len <= CUP_MAX_DE45_SIZE) // validate data length
			{
				memcpy( Auth_Tx.TLF01_details.track1 ,HOST_MESSAGE.track1,len);
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Track data 1 length (field 45) : %c %c,",
						 HOST_MESSAGE.track1_len[0],
						 HOST_MESSAGE.track1_len[1]);

			}
		}
   	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_48
 *
 *  DESCRIPTION:  This function will move the Additional Data-Private from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_48( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {

		if(strlen(HOST_MESSAGE.addl_data_len)> 0)
		{
	    	/*Based on value set in nccup/dcpiso we have to update this function*/	
			
			/* 1st find the length */
			memcpy(temp_len,HOST_MESSAGE.addl_data_len,CUP_DE48_LEN_SIZE);
			len = atoi(temp_len);
			
			if( len <= CUP_MAX_DE48_SIZE) // validate data length
			{
				/* Store length */
				memcpy(Auth_Tx.TLF01_details.voice_auth_text,
						HOST_MESSAGE.addl_data_len,
						CUP_DE48_LEN_SIZE);
				
				/* Store additional response data into host message */
				memcpy(Auth_Tx.TLF01_details.voice_auth_text + CUP_DE48_LEN_SIZE ,HOST_MESSAGE.addl_data,len);// Store data after length
			}
			else
			{
				*result_flag = 1;
				sprintf( err_buf,
						 "Invalid Additional Data-Private length (field 44) : %c %c %c,",
						 HOST_MESSAGE.addl_rsp_len[0],
						 HOST_MESSAGE.addl_rsp_len[1],
	 					 HOST_MESSAGE.addl_rsp_len[2]);
			}
		}
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_49
 *
 *  DESCRIPTION:  This function will move the Transaction currency code from the Host
 *                Message structure according to BDO Cadencie host formatting
 *                specifications.  Currency code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_49( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.tran_curr_cd)> 0)
   		{
	   		memcpy( Auth_Tx.TLF01_details.currency_code,
            	    HOST_MESSAGE.tran_curr_cd, 
					CUP_DE49_SIZE );
      		ret_val = isalphanum(Auth_Tx.TLF01_details.currency_code);

	        if ( ret_val == false )
	        {
	           *result_flag = 1;
	           sprintf( err_buf,
	                   "Non-alphanumeric Transaction Currency Code (field 49) : %s,",
	                    Auth_Tx.TLF01_details.currency_code );
	        }
		}
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_50
 *
 *  DESCRIPTION:  This function will move the Settlement currency code from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  Settlement Currency code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_50( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.settle_cur_cd)>0)
   		{
	   		memcpy( Auth_Tx.settlement_curr_code,
            	    HOST_MESSAGE.settle_cur_cd, 
					CUP_DE50_SIZE );
		    ret_val = isalphanum(Auth_Tx.settlement_curr_code);
		    if ( ret_val == false )
		    {
		       *result_flag = 1;
		       sprintf( err_buf,
		               "Non-alphanumeric Settlement Currency Code (field 50) : %s,",
		                Auth_Tx.settlement_curr_code );
		   }
   		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_51
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing currency code from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  Cardholder Billing Currency code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_51( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.card_curr_cd)>0)
   		{
   			memcpy( Auth_Tx.ch_billing_curr_code, 
					HOST_MESSAGE.card_curr_cd, 
					CUP_DE51_SIZE);
			
		    ret_val = isalphanum(Auth_Tx.ch_billing_curr_code);
		    if ( ret_val == false )
		    {
		       *result_flag = 1;
		       sprintf( err_buf,
		               "Non-alphanumeric Cardholder Billing Currency Code (field 51) : %s,",
		                Auth_Tx.ch_billing_curr_code );
		   }
			
   		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_52
 *
 *  DESCRIPTION:  This function will move the PIN Data from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  Cardholder Billing Currency code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_52( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.pinblock) > 0)
   		{
			memcpy( Auth_Tx.TLF01_details.pin_block, 
					HOST_MESSAGE.pinblock, 
					CUP_DE52_SIZE);

   		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_53
 *
 *  DESCRIPTION:  This function will move the Security Related Control Information from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_53( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
  		if(strlen(HOST_MESSAGE.sec_cntl)> 0)
   		{
	   		memcpy( Auth_Tx.mac_key, 
					HOST_MESSAGE.sec_cntl, 
					CUP_DE53_SIZE);
			
		    ret_val = isnum(Auth_Tx.mac_key);
		    if ( ret_val == false )
		    {
		       *result_flag = 1;
		       sprintf( err_buf,
		               "Non-numeric Security Related Control Information (field 53) : %s,",
		                Auth_Tx.mac_key );
		   }

   		}
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_54
 *
 *  DESCRIPTION:  This function will move the Additional Amount Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_54( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_55
 *
 *  DESCRIPTION:  This function will move the reconciliaton currency code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  reconciliation Currency code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/

INT move_field_55( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   movelen = 0;
   INT   temp_int = 0 ;
   INT   tag_91_len = 0;
   INT   emvLen = 0;
   BYTE  temp_string_len[5] = {0};
   BYTE  temp_emv_len[5] = {0};
   BYTE  temp_tag_len[3] = {0};
   BYTE  ICCdata[264] = {0};
   BYTE  ascii_len[5] = {0};
   BYTE  bcd_len[3] = {0};
   if ( *result_flag == 0 )
   {
      
     	strncpy(temp_emv_len,HOST_MESSAGE.IccData_len,3);
   		emvLen = atoi(temp_emv_len);
      if(emvLen > 0 && emvLen < sizeof(HOST_MESSAGE.IccData))
      	{
			// Request Msg

			if(Auth_Tx.TLF01_details.tx_key < 100)
			{
				//Copy raw data into EMV RAW
				memcpy(Auth_Tx.EMV_details.emv_block,HOST_MESSAGE.IccData_len,emvLen+3); // First 3 byte ACii len + EMV data in TLV format
				memset(ICCdata, 0, sizeof(ICCdata));
				memcpy(ICCdata, HOST_MESSAGE.IccData, emvLen);
				genutil_EMV_To_Auth_Tx(ICCdata,emvLen ,&Auth_Tx );
			}// resposne MSG
			else 
			{
				// Parse response TAGs only
				if(emvLen >0 && emvLen < 80)
				{
					sprintf(ascii_len,"%04d",emvLen);
					asc_to_bcd (ascii_len, 4, bcd_len);
					memcpy(Auth_Tx.EMV_details.future_field1,bcd_len,2); // First 3 byte ACii len + EMV data in TLV format
					// TAG len includes First byte length and follwed by IAD daata 8 byte ARPC and 2 byte RC 
					memcpy(&Auth_Tx.EMV_details.future_field1[2],&HOST_MESSAGE.IccData,emvLen);

				}
			}
      	}
     
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_57
 *
 *  DESCRIPTION:  This function will move the Additional Data Private from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_57( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		
   }
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         move_field_59
 *
 *  DESCRIPTION:  This function will move the Detailed Inquiry Data1 from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_59( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_60
 *
 *  DESCRIPTION:  This function will move the Self-Defined Field from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_60( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
   		if ( strlen(HOST_MESSAGE.self_def_feild)> 0)
   		{
   			memcpy(temp_len,HOST_MESSAGE.self_def_feild_len, CUP_DE60_LEN_SIZE);
   			len = atoi(temp_len);
			if(len <= CUP_MAX_DE60_SIZE)
			{
   				memcpy( Auth_Tx.future_use_4,
   						HOST_MESSAGE.self_def_feild_len,
						CUP_DE60_LEN_SIZE);
				
				memcpy( Auth_Tx.future_use_4 + CUP_DE60_LEN_SIZE,
						HOST_MESSAGE.self_def_feild,len) ;
   			}
   		}
   	}
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         move_field_61
 *
 *  DESCRIPTION:  This function will move the Cardholder Authentication Information from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_61( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
   		if( strlen(HOST_MESSAGE.card_holder_auth_info)> 0)
		{
			memcpy( temp_len,HOST_MESSAGE.card_holder_auth_info_len, 
					CUP_DE61_LEN_SIZE);
			memcpy(Auth_Tx.TLF01_details.product_codes[13].code,temp_len, CUP_DE61_LEN_SIZE);
			len=atoi(temp_len);
			if(len > 100)
		 	{
				memcpy( Auth_Tx.future_use_2 ,HOST_MESSAGE.card_holder_auth_info,100);
				memcpy( Auth_Tx.future_use_3 ,HOST_MESSAGE.card_holder_auth_info + 100, (len - 100));
		 	}
			else
			{
				memcpy( Auth_Tx.future_use_2 ,HOST_MESSAGE.card_holder_auth_info,len);
			}
		}
   	}
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_70
 *
 *  DESCRIPTION:  This function will move the Network Management Information Code from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_70( pINT result_flag, pCHAR err_buf )
{

   INT	ret_val = true;
   INT	len;
   BYTE temp_len[3]="";
   
   //Auth_Tx.function_code
   if ( *result_flag == 0 )
   {
		if(strlen(HOST_MESSAGE.net_manag_info_code)>0)
		{
			memcpy( Auth_Tx.function_code,HOST_MESSAGE.net_manag_info_code,CUP_DE70_SIZE);
		}
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_90
 *
 *  DESCRIPTION:  This function will move the  Original Data from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_90( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE tempstr[43] = {0};

   if ( *result_flag == 0 )
   {
   
   		/*
		 +-------------------------------------------------+
		      Feild 90 details
		 | BYTES  1- 4:  Original Message Type    |
		 | BYTES  5-10:  STAN of Original message          |
		 | BYTES 11-20:  Trans date & time of original msg |
		 | BYTES 21-31:  Acquiring Institution Code        |
		 | BYTES 32-42:  Forwarding Institution Code = '0' |
		 +-------------------------------------------------+
		 */
   		if ( strlen(HOST_MESSAGE.orig_data)> 0)
   		{
   		   	memcpy (tempstr ,HOST_MESSAGE.orig_data,   sizeof(HOST_MESSAGE.orig_data));
		 	if((tempstr[1] == '0') && (Auth_Tx.TLF01_details.message_type[1]=='4'))
      		{
         		 strcpy( err_buf, "Invalid data for original data elements");
				 nccup2_log_message( 1, 3, err_buf, "build_request_field_90",1 );
        	 return false;
		 	}
	      	else
	      	{
	        	memcpy(Auth_Tx.TLF01_details.orig_message, tempstr, 4);
	        	memcpy(Auth_Tx.TLF01_details.orig_sys_trace_num, tempstr+4, 6);
	        	memcpy(Auth_Tx.orig_local_time_date, tempstr+10,10);
	         	memcpy(Auth_Tx.orig_acq_inst_id,tempstr+20,11);

		         /* Actually forwarding institution id code */
		         memcpy(Auth_Tx.amex_pos_entry_mode,tempstr+31,11);

		         /* Since all values after message type in DE90 can be zero,
		          	* check the stan.  If zero, will need to use DE11 for reversals
		          	* to look up original transaction.
		          	*/
		         if ( 0 == strcmp(Auth_Tx.TLF01_details.orig_sys_trace_num,"000000"))
		         {
		            memcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
		                    Auth_Tx.TLF01_details.sys_trace_audit_num, 6 );
		         }
	      	}
   		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_96
 *
 *  DESCRIPTION:  This function will move the Message Security Code from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_96( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen( HOST_MESSAGE.mess_sec_code)>0)
   		{
   			memcpy(Auth_Tx.amex_pos_entry_mode,HOST_MESSAGE.mess_sec_code,CUP_DE96_SIZE);
		}
   	}
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_100
 *
 *  DESCRIPTION:  This function will move the Receiving Institution Identification Code from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_100( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.recv_ins_id_code)>0)
   		{
   			memcpy(temp_len,HOST_MESSAGE.recv_ins_id_code_len,CUP_DE100_SIZE_LEN);
   			len=atoi(temp_len);
			if(len <=CUP_MAX_DE100_SIZE)
			{

 				 /*Store data in  Auth_Tx.TLF01_details.product_codes[3].amount */
	  			 memcpy( Auth_Tx.TLF01_details.product_codes[3].amount,
				 		 HOST_MESSAGE.recv_ins_id_code,
				 		 len);
				ret_val = isalphanum(HOST_MESSAGE.recv_ins_id_code);
			    if ( ret_val == false )
			    {
			       *result_flag = 1;
			       sprintf( err_buf,
			               "Non-numeric Receiving Institution Identification Code (field 100) : %s,",
			               HOST_MESSAGE.recv_ins_id_code );
			   }
			}
			else
			{
			   *result_flag = 1;
			   sprintf( err_buf,
					   "Invalid Receiving Institution Identification Code length (field 100) : %s,",
						HOST_MESSAGE.recv_ins_id_code );
			}
   		}
   	}
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         move_field_102
 *
 *  DESCRIPTION:  This function will move the Account Identification 1 from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_102( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {

		if(strlen(HOST_MESSAGE.acc_id)>0)
		{


		}
		
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_104
 *
 *  DESCRIPTION:  This function will move the Field 104 Transaction and Industry Application Information from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_104( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
		if(strlen(HOST_MESSAGE.tranindappinfo_data_len)>0)
		{
			// As of now we are not storing this in TLF01 as we don't have data
		}
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_121
 *
 *  DESCRIPTION:  This function will move the CUPS Reserved from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_121( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   CHAR  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.cup_resv)>0)
   		{
   			memcpy(	Auth_Tx.iss_inst_id_len,
					HOST_MESSAGE.cup_resv_len,
					CUP_DE121_SIZE_LEN);
			strncpy(temp_len,HOST_MESSAGE.cup_resv_len,CUP_DE121_SIZE_LEN);

			len = atoi(temp_len);
			
   		   	memcpy(	Auth_Tx.iss_inst_id_len,
					HOST_MESSAGE.cup_resv_len,
					CUP_DE121_SIZE_LEN);
			strncpy( Auth_Tx.future_use_5,
					HOST_MESSAGE.cup_resv,len);
   		}
   	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_122
 *
 *  DESCRIPTION:  This function will move the Acquirer Institution Reserved from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_122( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		
   	}
   return( ret_val );
}









/******************************************************************************
 *
 *  NAME:         move_field_123
 *
 *  DESCRIPTION:  This function will move theIssuer Institution Reservedfrom the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_123( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[4]="";

   if ( *result_flag == 0 )
   {

		if(strlen(HOST_MESSAGE.issu_inst_resv)>0)
		{
		    len= strlen(HOST_MESSAGE.issu_inst_resv);
			sprintf( temp_len, "%03d", len );
			memcpy(Auth_Tx.int_net_facility_data,temp_len, CUP_DE123_SIZE_LEN);
			strcat(Auth_Tx.int_net_facility_data +CUP_DE123_SIZE_LEN ,HOST_MESSAGE.issu_inst_resv);
		}
   }
   return ret_val;
}

/******************************************************************************
 *
 *  NAME:         move_field_128
 *
 *  DESCRIPTION:  This function will move the Message Authentication Code from the Host
 *                Message structure according to  BDO Cadencie host formatting
 *                specifications.  
 *
 *  INPUTS:       Host_Message -  BDO Cadencie host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT move_field_128( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.mess_auth_code)> 0)
   		{
			memcpy( Auth_Tx.TLF01_details.product_codes[0].amount,
					HOST_MESSAGE.mess_auth_code,
					CUP_DE128_SIZE);
		}
   	}
   return( ret_val );
}











