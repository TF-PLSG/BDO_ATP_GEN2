
/***************************************************************************
* MODULE:      ncpul_bitfields.c
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
#include "ncpul_prototypes.h"
#include "ncpul_bitfields.h"
#include "pulse.h"
#include "genutil.h"  /* Includes all function prototypes for generic utilities */

/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     TranFlow;
extern CHAR    ServiceName[];
       INT     NetworkType;
extern INT 	   TIDctr=719;

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;


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
extern CHAR   key_check_value[5];




/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_2
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
 *                ncpul_ncpul_Host_Message-  PULSE host specific structure (Global)
 *
 *  RETURN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_2( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	
	memset(ncpul_HOST_MESSAGE.pan, '0', PULSE_MAX_DE2_SIZE );

	if ( ncpul_isnum(Auth_Tx.TLF01_details.card_num) )
	{
		len = strlen( Auth_Tx.TLF01_details.card_num );
		if( (len > 0) && (len <= PULSE_MAX_DE2_SIZE))
		{
			sprintf(ncpul_HOST_MESSAGE.pan_len, "%02d", len  );
			memcpy( ncpul_HOST_MESSAGE.pan,
					Auth_Tx.TLF01_details.card_num,
					len );
		}
		else if(len == 0)
		{
			*result_flag = 1;
			strcpy( err_buf,
					"Unable to build request message to host. No card number." );
		}
		else if(len > PULSE_MAX_DE2_SIZE)
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
 *  NAME:         ncpul_build_request_field_3
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
 *                ncpul_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_3( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		if( AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key  )
		{
			if(  true == ncpul_is_IST_transaction() )
			{
				memcpy( ncpul_HOST_MESSAGE.proccode,
						PULSE_DE3_PROC_CODE_BALANCE_INQ,
						PULSE_DE3_PROC_CODE_FIRST_2_DIGIT_LEN );

				memcpy( ncpul_HOST_MESSAGE.proccode + PULSE_DE3_PROC_CODE_FIRST_2_DIGIT_LEN,
					    Auth_Tx.TLF01_details.processing_code + PULSE_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ,
					    (PULSE_DE3_SIZE -PULSE_DE3_PROC_CODE_FIRST_2_DIGIT_LEN));
			}
			else
			{
				memcpy( ncpul_HOST_MESSAGE.proccode,
							Auth_Tx.TLF01_details.processing_code,
							PULSE_DE3_SIZE );
			}
		}
		else if ( ncpul_isnum(Auth_Tx.TLF01_details.processing_code) )
		{
			memcpy( ncpul_HOST_MESSAGE.proccode,
					Auth_Tx.TLF01_details.processing_code,
					PULSE_DE3_SIZE );
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
	memcpy(Auth_Tx.TLF01_details.product_codes[17].amount,ncpul_HOST_MESSAGE.proccode,PULSE_DE3_SIZE);
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_4
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
 *                ncpul_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_4( pINT result_flag, pCHAR err_buf )
{
	  if (strlen(Auth_Tx.TLF01_details.total_amount) > 0)
      {
    	  if ( ncpul_isnum(Auth_Tx.TLF01_details.total_amount) )
    	  {
    		  if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
    		  {
    			  memcpy( ncpul_HOST_MESSAGE.amount,
    					  Auth_Tx.TLF01_details.total_amount,
						  PULSE_DE4_SIZE );
    		  }
			  else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
								   Auth_Tx.TLF01_details.currency_code) )
			  {
				  memcpy( ncpul_HOST_MESSAGE.amount,
						  Auth_Tx.TLF01_details.total_amount,
						  PULSE_DE4_SIZE );
			  }
			  else
			  {
				  memcpy( ncpul_HOST_MESSAGE.amount,
						  Auth_Tx.TLF01_details.orig_amount,
						  PULSE_DE4_SIZE);
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
      else if(0 == strlen(Auth_Tx.TLF01_details.total_amount))
      {
    	  //Check if its a valid amount zero transaction
    	 if(false== ncpul_check_if_valid_zero_amount_transaction())
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


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_5
 *
 *  DESCRIPTION:  This function will move the Settlement_amount to the Host
 *                Message structure according to PULSE host formatting
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_5( pINT result_flag, pCHAR err_buf )
{
	/* It is the copy of DE4 need to know from which field of auth this as to be filled */
   if ( *result_flag == 0 )
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) )
      {
    	  if (strlen(Auth_Tx.TLF01_details.total_amount) > 0)
    	  {
         	memcpy( ncpul_HOST_MESSAGE.Settlement_amount,
         			Auth_Tx.TLF01_details.total_amount,
					PULSE_DE5_SIZE );
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
    	  if ( ncpul_isnum(Auth_Tx.TLF01_details.total_amount) )
    	  {
    		  if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
    		  {
    			  memcpy( ncpul_HOST_MESSAGE.Settlement_amount,
    					  Auth_Tx.TLF01_details.total_amount,
						  PULSE_DE5_SIZE );
    		  }
			  else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
								   Auth_Tx.TLF01_details.currency_code) )
			  {
				  memcpy( ncpul_HOST_MESSAGE.Settlement_amount,
						  Auth_Tx.TLF01_details.total_amount,
						  PULSE_DE5_SIZE );
			  }
			  else
			  {
				  memcpy( ncpul_HOST_MESSAGE.Settlement_amount,
						  Auth_Tx.TLF01_details.orig_amount,
						  PULSE_DE5_SIZE);
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
      else if(0 == strlen(Auth_Tx.TLF01_details.total_amount))
      {
    	  //Check if its a valid amount zero transaction
    	 if(false== ncpul_check_if_valid_zero_amount_transaction())
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
 *  NAME:         ncpul_build_request_field_6
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
 *                ncpul_Host_Message- DCI host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_6( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		// Need to check for offus transaction
	}
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_7
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
 *                ncpul_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_7( pINT result_flag, pCHAR err_buf )
{
   CHAR  date_str [ 9] = {0};
   CHAR  time_str [ 7] = {0};
   CHAR  time_date[25] = {0};
  
   if ( *result_flag == 0 )
   {
      if ( ncpul_is_reversal() )
      {
         /* It is a reversal.  Original time stamp must exist. */
         if ( 0 != strlen(Auth_Tx.orig_local_time_date) )
         {
         	 memcpy(ncpul_HOST_MESSAGE.gmt_time, Auth_Tx.orig_local_time_date, PULSE_DE7_SIZE );
         }
         else  if ( 0 == strlen(Auth_Tx.TLF01_details.transmission_timestamp) )
         {
               /* Original time stamp does not exist. Error! */
               *result_flag = 1;
               sprintf( err_buf,
                     	"Cannot build request msg (%s) for host. Missing a time stamp.",
                     	 Auth_Tx.TLF01_details.message_type );
         }
         else
         {
            /* Need to move timestamp to 'original' field. */
            memcpy( Auth_Tx.orig_local_time_date,
                    Auth_Tx.TLF01_details.transmission_timestamp,
                    PULSE_DE7_SIZE );

		    memcpy( ncpul_HOST_MESSAGE.gmt_time,
		    		Auth_Tx.orig_local_time_date,
					PULSE_DE7_SIZE );
         }
         
      }

      else if ( 0 == strlen(Auth_Tx.TLF01_details.transmission_timestamp) )
	  {
    	  /* Need to create transmission time stamp. Get local time. */
    	  ptetime_get_timestamp( time_date );
    	  ncpul_get_date( time_date, date_str );
    	  ncpul_get_time( time_date, time_str );

    	  /* transmission_timestamp format MMDDhhmms */
    	  /* Store MMDD in transmission_timestamp */
    	  memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
    			  date_str + PULSE_MMDD_START_POS,
				  PULSE_MMDD_LEN);
    	  /* Store hhmmss in transmission_timestamp after storing MMDD */
    	  memcpy( Auth_Tx.TLF01_details.transmission_timestamp + 4,
    			  time_str,
				  PULSE_HHMMSS_LEN);
    	  memcpy(ncpul_HOST_MESSAGE.gmt_time, Auth_Tx.TLF01_details.transmission_timestamp, PULSE_DE7_SIZE );
	  }
      else
      {
    	  memcpy( ncpul_HOST_MESSAGE.gmt_time,
    			  Auth_Tx.TLF01_details.transmission_timestamp,
				  PULSE_DE7_SIZE );
      }
   }
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_9
 *
 *  DESCRIPTION:  This function will move the Settlement Conversion Rate
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
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_9( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if ( *result_flag == 0 )
	{
			memcpy( ncpul_HOST_MESSAGE.convRateSettlement,
			Auth_Tx.TLF01_details.conversion_rate,
			PULSE_DE9_SIZE );
	}

}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_10
 *
 *  DESCRIPTION:  This function will move the Card holder Billing Conversion Rate
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *              It is the conversion rate for the currency conversion from the acquirer‘s
 *		   transaction currency to the card holder billing currency.
 *		  The format is right-justified, no decimal point.
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
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_10( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if ( *result_flag == 0 )
	{

			memcpy( ncpul_HOST_MESSAGE.convRateCardHolder,
			Auth_Tx.ch_billing_conv_rate,
			PULSE_DE10_SIZE );

	}

}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_11
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
 *                ncpul_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_11( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		if ( ncpul_isnum(Auth_Tx.TLF01_details.sys_trace_audit_num) )
		{
				memcpy( ncpul_HOST_MESSAGE.stan,
						Auth_Tx.TLF01_details.sys_trace_audit_num, 
						PULSE_DE11_SIZE );
		}
		else if ( ncpul_isnum(Auth_Tx.TLF01_details.orig_sys_trace_num) )
		{
			memcpy( ncpul_HOST_MESSAGE.stan,
					Auth_Tx.TLF01_details.orig_sys_trace_num, 
					PULSE_DE11_SIZE );
		}
		else 
		{
				/* For POS reversals create new STAN */
				ncpul_create_stan(ncpul_HOST_MESSAGE.stan );
		}				
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_12
 *
 *  DESCRIPTION:  This function will move the transaction Local time
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 6 hhmmss
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_12( pINT result_flag, pCHAR err_buf )
{
	CHAR  time_date[25] = {0};
	CHAR  time_str[7] = {0};
	CHAR  date_str[9] = {0};

	if ( *result_flag == 0 )
	{
		// Let us prepare Local Transaction Date And Time. We are not storing this in TLF01 as off
		// now , we will store in some product code if required


		 if ( 0x00 != Auth_Tx.TLF01_details.time_hhmmss[0] )
		 {
			 memcpy( ncpul_HOST_MESSAGE.time,
							 Auth_Tx.TLF01_details.time_hhmmss,
							 PULSE_DE12_SIZE );

		}else
		{
			ptetime_get_timestamp( time_date );
			ncpul_get_time( time_date, time_str );

					  /* Local Transaction Date And Time format  hhmmss */
					  /* Store hhmmss in transmission_timestamp*/
					  memcpy( ncpul_HOST_MESSAGE.time ,
							  time_str,
							  PULSE_DE12_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_13
 *
 *  DESCRIPTION:  This function will move the Effective Transaction Date
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_13( pINT result_flag, pCHAR err_buf )
{
	CHAR  time_date[25] = {0};
	CHAR err_buf1[100]  = {0};

	if ( *result_flag == 0 )
	{

		memcpy( ncpul_HOST_MESSAGE.date,
				&Auth_Tx.TLF01_details.date_yyyymmdd[PULSE_MMDD_START_POS],
				PULSE_DE13_SIZE);
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_14
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
 *                ncpul_Host_Message- DCI  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_14( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.exp_date ) )
		{
			memcpy( ncpul_HOST_MESSAGE.date_expd,
					Auth_Tx.TLF01_details.exp_date,
					PULSE_DE14_SIZE );
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_15
 *
 *  DESCRIPTION:  This function will move the settlement date
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_15( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.exp_date ) )
		{
			memcpy( ncpul_HOST_MESSAGE.SettleDate,
					Auth_Tx.TLF01_details.settlement_date,
					PULSE_DE15_SIZE );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_16
 *
 *  DESCRIPTION: This function will move the conversionDate date
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *                The month and day the conversion rate is effective to
 *                convert DE 004 (Amount, Transaction) and DE 028 (Amount, Transaction Fee)
 *                to the settlement currencies.
 *
 *  DATA ELEMENT FORMAT :n 4 MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_16( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;

	if ( *result_flag == 0 )
	{
		if ( 0 != strcmp( NULL_STR, Auth_Tx.conv_date ) )
		{
			memcpy( ncpul_HOST_MESSAGE.conversionDate,	Auth_Tx.conv_date,PULSE_DE16_SIZE );
		}
	}

}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_17
 *
 *  DESCRIPTION:  This function will move the Effective Capture Date
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *                The month and day the transaction was processed by the
 *                acquirer processor, as determined by the acquirer processor’s calendar day.
 *
 *  DATA ELEMENT FORMAT :n 4 MMDD
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_17( pINT result_flag, pCHAR err_buf )
{
	CHAR  time_date[25] = {0};
	CHAR err_buf1[100]  = {0};

	/* Optional field
	 *  need to know from where we need to copy to host */
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_18
 *
 *  DESCRIPTION:  This function will move the  Merchant Category Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *               This field indicates Merchant Category Code (MCC).
 *
 *  FFORMAT: n4  Merchant Type
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
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_18( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	/*copied form nccup as both spec explain the same */
	if ( *result_flag == 0 )
	{
		if (strlen(Auth_Tx.MCF01_details.voice_category_code)> 0 )
		{
			memcpy( ncpul_HOST_MESSAGE.Merch_type,
			Auth_Tx.MCF01_details.voice_category_code,
			PULSE_DE18_SIZE );
		}
		else
		{
			memcpy( ncpul_HOST_MESSAGE.Merch_type,
			Auth_Tx.TLF01_details.category_code,
			PULSE_DE18_SIZE );
		}
	}

}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_19
 *
 *  DESCRIPTION:  This function will move the Acquirer Country Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *				The code of the country (ISO 3166) where the acquiring institution’s is located
 *
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
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_19( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   if(strlen(Ncf01_I.country_code) > 0)
	  {
			if ( ncpul_isnum(Ncf01_I.country_code) )
			{
				memcpy(ncpul_HOST_MESSAGE.Acq_country_code,
					   Ncf01_I.country_code,
					   PULSE_DE19_SIZE );
			}
			else
			{
				memcpy(ncpul_HOST_MESSAGE.Acq_country_code,
								   "608",
								   PULSE_DE19_SIZE );
			}

	  }
	  else
	  {
	   			memcpy(ncpul_HOST_MESSAGE.Acq_country_code,
	   							   "608",
	   							   PULSE_DE19_SIZE );
	  	}
   }

}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_22
 *
 *  DESCRIPTION:  This function will move the POS entry mode
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT : n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_22( pINT result_flag, pCHAR err_buf )
{

	memset (ncpul_HOST_MESSAGE.posentry,0x30,PULSE_DE22_SIZE);

	if ( 0 == strlen(Auth_Tx.TLF01_details.pos_entry_mode) )
	{
		*result_flag = 1;
		strcpy( err_buf, "POS Entry Mode is missing. Decline txn." );
	}
	else
	{
		strncpy(ncpul_HOST_MESSAGE.posentry,Auth_Tx.TLF01_details.pos_entry_mode+1,PULSE_DE22_SIZE);
	}
	memcpy(Auth_Tx.TLF01_details.product_codes[9].code,Auth_Tx.TLF01_details.pos_entry_mode,sizeof(Auth_Tx.TLF01_details.pos_entry_mode)-1);
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_23
 *
 *  DESCRIPTION:  This function will move the  Card Sequence Number
 *                to the Host Message structure according to PULSE  host
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_23( pINT result_flag, pCHAR err_buf )
{
   INT	   itemp = 0;
   CHAR    temp_str[4]={0};
   CHAR    temp_str1[4]={0};

   if ( *result_flag == 0 )
   {
		if (ncpul_blank_or_null_string(Auth_Tx.host2_message_seq_nbr,PULSE_DE23_SIZE) != true)
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy( temp_str,
					  Auth_Tx.host2_message_seq_nbr + PULSE_DE23_CARD_SEQ_POS_IN_STR,
					  PULSE_DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( ncpul_HOST_MESSAGE.card_seq_num,
					  temp_str1,
					  PULSE_DE23_SIZE );
		}
		else if (ncpul_blank_or_null_string(Auth_Tx.EMV_details.pan_sequence_number,PULSE_DE23_SIZE) != true )
		{
			  memcpy( temp_str,
					  Auth_Tx.EMV_details.pan_sequence_number,
					  PULSE_DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( ncpul_HOST_MESSAGE.card_seq_num,
					  temp_str1,
					  PULSE_DE23_SIZE );
		}
		else if (strncmp(ncpul_HOST_MESSAGE.posentry,"051",3) == 0)
		{
			  memcpy( ncpul_HOST_MESSAGE.card_seq_num,
					  "000",
					  PULSE_DE23_SIZE );
		}
  }
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_26
 *
 *  DESCRIPTION:  This function will move the Point-of-Service PIN Capture Code
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *                The maximum number of PIN characters accepted by the point-of-service device
 *
 *  DATA ELEMENT FORMAT :n 2
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  hos specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:     Ragahvendra
 *
 ******************************************************************************/
void ncpul_build_request_field_26( pINT result_flag, pCHAR err_buf )
{
	/* last two bytes of pos enrty mode is copied to
	 * pos pin capture code
	 *  */

	if ( *result_flag == 0 )
	{
		/*if (0 == strlen(Auth_Tx.TLF01_details.pos_entry_mode))
		{
			strncpy( ncpul_HOST_MESSAGE.Pos_pin_capture_code, Auth_Tx.TLF01_details.pos_entry_mode+3 ,PULSE_DE26_SIZE);
		}
		else
		{

			strncpy( ncpul_HOST_MESSAGE.Pos_pin_capture_code,"12" ,PULSE_DE26_SIZE);
		}*/
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_28
 *
 *  DESCRIPTION:  This function will move the Transaction Fee Amount
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
 *  AUTHOR:     	Raghavevndra
 *
 ******************************************************************************/
void ncpul_build_request_field_28( pINT result_flag, pCHAR err_buf )
{
   /* copied form cup,need to know how to assign C or D
    * */
	if ( *result_flag == 0 )
	{
		if ((strlen(Auth_Tx.TLF01_details.tran_fee_amount) > 0) &&
		(strlen(Auth_Tx.TLF01_details.tran_fee_amount) <= PULSE_DE28_SIZE))
		{
			strncpy(ncpul_HOST_MESSAGE.Txn_Fee_Amount,"D",1);
			memcpy(ncpul_HOST_MESSAGE.Txn_Fee_Amount +1 ,
			Auth_Tx.TLF01_details.tran_fee_amount,
			PULSE_DE28_SIZE);
		}
	}

}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_29
 *
 *  DESCRIPTION:  This function will move the Settlement Fee Amount
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
 *  AUTHOR:     Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_29( pINT result_flag, pCHAR err_buf )
{

	// conditional and required if DE5 is present

}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_32
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:     Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_32( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	CHAR temp_len[3] = {0};
   /* copy of ncdci need to verify */
	if ( *result_flag == 0 )
	{
		len = strlen( Ncf01_I.acquirer_inst_id_code );
		memcpy( ncpul_HOST_MESSAGE.acq_inst_id,
				Ncf01_I.acquirer_inst_id_code,
				len );
	}

	/* Get the length. */
	if(len > 0)
	{
		sprintf( temp_len, "%02d", len );
		memcpy( ncpul_HOST_MESSAGE.acq_inst_len,
				temp_len,
				PULSE_DE32_LEN_SIZE );

		/* Store value in TLF01 if there is not already a value. */
		if ( Auth_Tx.TLF01_details.acquiring_id[0] == 0x00 )
		{
			memcpy( Auth_Tx.TLF01_details.acquiring_id,
					ncpul_HOST_MESSAGE.acq_inst_id,
					len );
		}
	}
	else
	{
		result_flag = 1;

	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_33
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
 *                ncpul_Host_Message-  PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_33( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	CHAR temp_len[3] = {0};

/*	if ( *result_flag == 0 )
	{
		if ( strlen(Auth_Tx.forwarding_inst_id_cd) > 0 )
		{
			len = strlen( Auth_Tx.forwarding_inst_id_cd );
			sprintf( temp_len, "%02d", len );
			memcpy( ncpul_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					PULSE_DE33_LEN_SIZE );

			memcpy( ncpul_HOST_MESSAGE.fwd_inst_id,
					Auth_Tx.forwarding_inst_id_cd,
					len );
		}
		else 
		{
			len = strlen( Ncf01_I.forwarding_inst_id_code );
			sprintf( temp_len, "%02d", len );
			memcpy( ncpul_HOST_MESSAGE.fwd_inst_len,
					temp_len,
					PULSE_DE33_LEN_SIZE );

			memcpy( ncpul_HOST_MESSAGE.fwd_inst_id,
					Ncf01_I.forwarding_inst_id_code,
					len );
		}
	} */
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_35
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_35( pINT result_flag, pCHAR err_buf )
{
	INT   len = 0;
	CHAR temp_len[3] = {0};
	//  copy of dci need to verify
	if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.track2[0] != 0x00)
		{
			len = strlen( Auth_Tx.TLF01_details.track2 );
			sprintf( temp_len, "%02d", len );
			memcpy( ncpul_HOST_MESSAGE.track2_len,
					temp_len,
					PULSE_DE35_LEN_SIZE);

			memcpy( ncpul_HOST_MESSAGE.track2,
					Auth_Tx.TLF01_details.track2,
					len );
			/*make sure if Tk2 is exists DE2 should be dropped*/
			if(len >0)
			{
				memset( ncpul_HOST_MESSAGE.pan,
									0x00,
									sizeof(ncpul_HOST_MESSAGE.pan));
				memset( ncpul_HOST_MESSAGE.pan_len,
									0x00,
									sizeof(ncpul_HOST_MESSAGE.pan_len));

			}
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_37
 *
 *  DESCRIPTION:  This function will move the Acquirer reference number
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.  I
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host  specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_37( pINT result_flag, pCHAR err_buf )
{
	//  copy of dci need to verify
	if( Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE )
	{
		if (strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) > 0)
		{
			memcpy( ncpul_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.orig_retrieval_ref_num,
					PULSE_DE37_SIZE);
		}
		else
		{
			memcpy( ncpul_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.retrieval_ref_num,
					PULSE_DE37_SIZE);
		}
	}
	else
	{
		if (strlen(Auth_Tx.TLF01_details.retrieval_ref_num) > 0)
		{
			memcpy( ncpul_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.retrieval_ref_num,
					PULSE_DE37_SIZE);
		}
		else if (strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) > 0)
		{
			memcpy( ncpul_HOST_MESSAGE.rrn,
					Auth_Tx.TLF01_details.orig_retrieval_ref_num,
					PULSE_DE37_SIZE);
		}
		else
		{
			ncpul_create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_38
 *
 *  DESCRIPTION:  This function will move the authorization number
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :	ans 6
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_38( pINT result_flag, pCHAR err_buf )
{
	//  copy of dci need to verify
	if ( *result_flag == 0 )
	{
		if ( strlen(Auth_Tx.TLF01_details.auth_number)> 0 )
		{
			memcpy(	ncpul_HOST_MESSAGE.authcode,
					Auth_Tx.TLF01_details.auth_number,
					PULSE_DE38_SIZE );
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_39
 *
 *  DESCRIPTION:  This function will move the response code
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *   DATA ELEMENT FORMAT : an 2
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_39( pINT result_flag, pCHAR err_buf )
{
   CHAR dci_response_code[4] = {0};
   //  copy of dci need to verify
   if(  *result_flag == 0)
	{	
		if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
							 PULSE_NETWORK_RESP_MESSAGE,
							 PULSE_MESSAGE_TYPE_SIZE))
		{
			memcpy(	ncpul_HOST_MESSAGE.responsecd,
					PULSE_NETMGMT_APPROVED,
					PULSE_DE39_SIZE );
		}
		else if(strlen( Auth_Tx.TLF01_details.response_code)> 0)
		{
			memcpy(	ncpul_HOST_MESSAGE.responsecd,
					Auth_Tx.TLF01_details.response_code,
					PULSE_DE39_SIZE );
			if(0 == strncmp(Auth_Tx.TLF01_details.message_type,
								PULSE_REVERSAL_REQ_MESSAGE,
										 PULSE_MESSAGE_TYPE_SIZE)||
			   0 == strncmp(Auth_Tx.TLF01_details.message_type,
					   PULSE_ISO_REVESAL_REQ_MESSAGE_TYPE,
															 PULSE_MESSAGE_TYPE_SIZE)	)
			{
				memcpy(	ncpul_HOST_MESSAGE.responsecd,
						PULSE_APPROVED,
									PULSE_DE39_SIZE );
			}

		}

		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
		   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE	)
		{
			memcpy(	ncpul_HOST_MESSAGE.responsecd,
									PULSE_APPROVED,
												PULSE_DE39_SIZE );
		}	
	}


}


/*****************PULSE_APPROVED*************************************************************
 *
 *  NAME:         ncpul_build_request_field_40
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to PULSE Standard
 *                formatting specifications.  I
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_40( pINT result_flag, pCHAR err_buf )
{
	CHAR temp_str[8] = {0};
	// copy of dci need to check
	if ( *result_flag == 0 )
	{
		if (strlen(Auth_Tx.TLF01_details.service_code) > 0)
		{
			memcpy( ncpul_HOST_MESSAGE.service_code,Auth_Tx.TLF01_details.service_code, PULSE_DE40_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_41
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *   DATA ELEMENT FORMAT :an 8
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_41( pINT result_flag, pCHAR err_buf )
{
	INT  i = 0;
	INT  j=0;
	CHAR temp_str[9] = {0};
	// copy of dci need to check
	if ( *result_flag == 0 )
	{
		if (strlen(Auth_Tx.TLF01_details.terminal_id) > 0)
		{
			strcpy(temp_str, Auth_Tx.TLF01_details.terminal_id);
			/* If Device number less than 8, instead of NULL fill with spaces*/
			j = strlen(Auth_Tx.TLF01_details.terminal_id);
			if(j>8 || j <0)
			{
				result_flag = 1 ;
			}
			for (i = j; i <8 ; i++)
			{
				temp_str[i] = ' ';
			}
			memcpy( ncpul_HOST_MESSAGE.termid,temp_str, PULSE_DE41_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_42
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Identification Code
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :an 15
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_42( pINT result_flag, pCHAR err_buf )
{
	// copy of dci need to check
	if ( *result_flag == 0 )
   {
	   	if (strlen(Auth_Tx.TLF01_details.merchant_id) > 0)
		{
			memset (ncpul_HOST_MESSAGE.cardaccid,0x20,PULSE_DE42_SIZE);
			memcpy( ncpul_HOST_MESSAGE.cardaccid,Auth_Tx.TLF01_details.merchant_id,PULSE_DE42_SIZE);
		}
    }
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_43
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location
 *                to the Host Message structure according to  PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 40
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- Card Acceptor Name/Location specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_43( pINT result_flag, pCHAR err_buf )
{
	   INT  ret_val = true;
	   INT  f43_Len = 0;
	   // copy of dci need to check
	    /*
	        Sub-field    Position    Length                      Feild details
		      1             1       23               Merchant name or ATM location (padding with blanks)
		      2            24       12               City‘s name (padding with blanks)
		      3            37        2               State
		      4			   39		 2				 Country/region Code, adopting the coding
		 */

	   if ( *result_flag == 0 )
	   {
		   	 memset( ncpul_HOST_MESSAGE.cardaccname,0x20,PULSE_DE43_SIZE);

	  		 if( strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0)
			 {
				 /* Transaction is ATM. */
				 memcpy( ncpul_HOST_MESSAGE.cardaccname,
				 		 Auth_Tx.MCF01_details.address01,
						 PULSE_ATM_LOC_LEN_FEILD_43);// Max 23 Byte
			 }
			 else
			 {
				 /* Transaction is a POS transaction. */
				if(strcmp(Auth_Tx.MCF01_details.name02,"")!=0)
				{
					memcpy( ncpul_HOST_MESSAGE.cardaccname,
							Auth_Tx.MCF01_details.name02,
							PULSE_MER_NAME_LEN_FEILD_43);	// Max 23 Byte
				}
			}

			 memcpy( ncpul_HOST_MESSAGE.cardaccname+23,
			 	     Auth_Tx.MCF01_details.city,
					 PULSE_CITY_LEN_FEILD_43);// Max 12 Byte one byte will over right on Country Name

			 memcpy( ncpul_HOST_MESSAGE.cardaccname+36,
					 "  ",
					PULSE_STATE_CODE_LEN_FEILD_43 );// Max 2 Byte

			 memcpy( ncpul_HOST_MESSAGE.cardaccname+38,
					 PULSE_COUNTRY_CODE_FEILD_43,
					 PULSE_COUNTY_CODE_LEN_FEILD_43);// Max 2 Byte
		 }




}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_44
 *
 *  DESCRIPTION:  This function will move the Additional Response Data
 *                to the Host Message structure according to  PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 25 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message-  PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_44( pINT result_flag, pCHAR err_buf )
{
	INT len = 0;
	BYTE temp_len[3] = {0};

	/* conditional but not required
	 * need to know */
}
	

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_46
 *
 *  DESCRIPTION:  This function will move the Surcharge Fee/ Additional fee
 *                to the Host Message structure according to  PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 88 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message-  PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:        Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_46( pINT result_flag, pCHAR err_buf )
{
	INT   len = 0;
	CHAR  temp_len[4] = {0};
	// copy of dci need to verify
	if( *result_flag == 0 )
	{
		// Need to check if we have to send this filed for ACQ transaction
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_47
 *
 *  DESCRIPTION:  This function will move the additional Data-Private
 *                to the Host Message structure according to  PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 100 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message-  PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:        Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_47( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  temp_len[4] = {0};

	/*conditional check later*/
	if( *result_flag == 0 )
	{
		// Need to check if we have to send this filed for ACQ transaction
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_48
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location
 *                to the Host Message structure according to  PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 25 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- Card Acceptor Name/Location specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_48( pINT result_flag, pCHAR err_buf )
{
	   INT  ret_val = true;
	   INT  f43_Len = 0;
	   INT   len = 0;
	   CHAR  temp_len[4] = {0};

	    /*
	       Sub-field    Position    Length                      Feild details
		    1             1            25               Merchant name or ATM location (padding with blanks)

		 */

	   if ( *result_flag == 0 )
	   {
			if( strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0)
			{
				len = strlen(Auth_Tx.MCF01_details.address01);
				sprintf(temp_len, "%03d", len);

				memcpy( ncpul_HOST_MESSAGE.Merch_name_len,
					temp_len,
					PULSE_DE48_LEN_SIZE);
			 /* Transaction is ATM. */
				memcpy( ncpul_HOST_MESSAGE.Merch_name,
					 Auth_Tx.MCF01_details.address01,
					 len);// Max 25 Byte
			}
			else
			{
			 /* Transaction is a POS transaction. */
				if(strcmp(Auth_Tx.MCF01_details.name02,"")!=0)
				{
					len = strlen(Auth_Tx.MCF01_details.name02);
					sprintf(temp_len, "%03d", len);

					memcpy( ncpul_HOST_MESSAGE.Merch_name_len,
								temp_len,
							PULSE_DE48_LEN_SIZE);
					memcpy( ncpul_HOST_MESSAGE.Merch_name,
							Auth_Tx.MCF01_details.name02,
							len);	// Max 25 Byte
				}
			}

	   }
}



/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_49
 *
 *  DESCRIPTION:  This function will move the Transaction Currency Code, 
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_49( pINT result_flag, pCHAR err_buf )
{
	// copy of dci need to verify
	if ( *result_flag == 0 )
	{
		if(strlen(Auth_Tx.TLF01_details.currency_code)> 0)
		{
		   	if ( false == ncpul_isalphanum(Auth_Tx.TLF01_details.currency_code) )
			{
				 *result_flag = 1;
				 sprintf( err_buf,
						 "Unable to build request msg to host. Invalid currency code in TLF01 (%s)",
						 Auth_Tx.TLF01_details.currency_code );
			}
			else
			{
				memcpy( ncpul_HOST_MESSAGE.tran_curr_cd,
					    Auth_Tx.TLF01_details.currency_code,
					    PULSE_DE49_SIZE );
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
 *  NAME:         ncpul_build_request_field_50
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
 *  AUTHOR:     Raghavendra
 *
 ******************************************************************************/
INT ncpul_build_request_field_50 ( pINT result_flag, pCHAR err_buf )
{
   /*copied from cup */
   if ( *result_flag == 0 )
   {
	   if( strlen(Auth_Tx.settlement_curr_code)> 0)
	   {
			if ( false == ncpul_isalphanum(Auth_Tx.settlement_curr_code) )
			{

				*result_flag = 1;
				sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code (%s)",
					  Auth_Tx.settlement_curr_code );
			}
			else
			{
				memcpy( ncpul_HOST_MESSAGE.settlement_curr_cd,
					    Auth_Tx.settlement_curr_code,
						PULSE_DE50_SIZE );
			}
	   }
	}

}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_51
 *
 *  DESCRIPTION:  This function will move the Card holder Billing currency code
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_51 ( pINT result_flag, pCHAR err_buf )
{
	// copy of dci need to verify
	if ( *result_flag == 0 )
   {
		if (Auth_Tx.ch_billing_curr_code [0] != '\0')
		{
			memcpy( ncpul_HOST_MESSAGE.card_curr_cd,
					Auth_Tx.ch_billing_curr_code,
					PULSE_DE51_SIZE);
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_52
 *
 *  DESCRIPTION:  This function will move the pin block
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :hexa 8
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_52( pINT result_flag, pCHAR err_buf )
{
   INT  len = 0;
   CHAR tempstr[20] = {0};
   // copy of dci need to check
   if ( *result_flag == 0 )
   {
		if ( true == ncpul_pin_block_exists() )
		{
			memset(ncpul_HOST_MESSAGE.pinblock, 0x00, PULSE_DE52_SIZE );

			memcpy( ncpul_HOST_MESSAGE.pinblock,
								Auth_Tx.TLF01_details.pin_block,
								PULSE_DE52_SIZE);
		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_54
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_54 ( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	BYTE temp_len[4] = {0};
	BYTE temp_addamt[51] = {0};

	if( *result_flag == 0)
	{
		/*Account Type (same as DE3 positions 3–4)*/
		strncpy( temp_addamt,
				 Auth_Tx.TLF01_details.processing_code + PULSE_DE54_ACCOUNT_TYPE_STARTING_POS,
				 PULSE_DE54_ACCOUNT_TYPE_LEN);

		/*Amount Type
		 * 91 -*/
		strncat( temp_addamt,
				 PULSE_DE54_AMOUNT_TYPE,
				 PULSE_DE54_AMOUNT_TYPE_LEN);

		/*digit numeric ISO Currency Code*/
		strncat( temp_addamt,
				 Auth_Tx.TLF01_details.currency_code,
				 PULSE_DE49_SIZE);

		/*Amount Signing positive(C)/negative(D)*/
		// Need to know how to Identify positive(C)/negative(D)available balance
		strncat( temp_addamt,
				 PULSE_DE54_POSITIVE_AVAILABLE_BALANCE,
				 PULSE_DE54_AVAILABLLE_BALANCE_LEN);

		/*Amount*/
		strncat( temp_addamt,
				 Auth_Tx.TLF01_details.total_amount,
				 PULSE_DE4_SIZE);

		len = strlen(temp_addamt);
		sprintf(temp_len, "%03d", len);

		memcpy( ncpul_HOST_MESSAGE.AddlAmts_len,
				temp_len,
				PULSE_DE54_LEN_SIZE);

		memcpy( ncpul_HOST_MESSAGE.AddlAmts,
				temp_addamt ,
				len);
	}
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_55
 *
 *  DESCRIPTION:  This function will move IC Card Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans-b…255 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Phani Kumar
 *
 ******************************************************************************/

void ncpul_build_request_field_55 ( pINT result_flag, pCHAR err_buf )
{
		INT  emv_len = 0;
		INT  eqt_len = 0;
		CHAR temp_len[5] = {0};

		if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
			 (Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
		{
			return;
		}
		eqt_len = Auth_Tx.EMV_details.emv_block[0];
		emv_len =  Auth_Tx.EMV_details.emv_block[3];
		/* Checking the header length and the actual data length to be sent to Pulse.
		 * sample header length and value 64010061<actuall DE55 data>
		 * 64 (hex): Total len
		 * 0100    : Header
		 * 61 (hex): DE55 len
		 * Hence subtracting the total len with 3 to check the actual data len.
		 * */
		if (emv_len != (eqt_len - 3) ) // these two values should be same.
		{
			ncpul_log_message( 1, 1, "DE55, Header length and actual data length mis match", "ncpul_build_request_field_55",0 );
		}
		if(emv_len == 0)
		{
			return ;
		}
		sprintf(temp_len, "%03d", emv_len);
		memcpy( ncpul_HOST_MESSAGE.IccData_len,temp_len,PULSE_DE55_LEN_SIZE);
		memcpy( ncpul_HOST_MESSAGE.IccData,&Auth_Tx.EMV_details.emv_block[4],emv_len);

}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_56
 *
 *  DESCRIPTION:  This function will move the Original Data Elements
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 88 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_56 ( pINT result_flag, pCHAR err_buf )
{
	INT  len = 0;
	BYTE temp_len[3] = {0};
	BYTE orig_data[34] = {0};

	// need to check for subfileds 4 and 5
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_57
 *
 *  DESCRIPTION:  This function will move the Original Data Elements
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :n 03 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_57 ( pINT result_flag, pCHAR err_buf )
{


	// need to check know the logic to fill
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_58
 *
 *  DESCRIPTION:  This function will move the National Point-Of-Service Condition Code
 *  			   to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :an 11
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_58( pINT result_flag, pCHAR err_buf )
{
   INT  len = 0;
   CHAR tempstr[20] = {0};
   /*Need to modify for testing purpose a sample data is copied */
   //ncpul_build_pos_data_from_iso_pos_entry_mode();01000000012

	len = 11;
	sprintf(ncpul_HOST_MESSAGE.National_pos_cc_len, "%03d",len);
	strncpy(ncpul_HOST_MESSAGE.National_pos_cc,"00000000025",len);
	if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021")== 0 )
	{
		ncpul_HOST_MESSAGE.National_pos_cc[10]='2';
	}
	if(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0420",4))
	{
		strncpy(ncpul_HOST_MESSAGE.National_pos_cc,"00000000020",11);
	}

}



/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_59
 *
 *  DESCRIPTION:  This function will move the National Point-Of-Service Geographic Data
 *                to the Host Message structure according to PULSE  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_59( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	BYTE  temp_len[4] = {0};
    CHAR  tempstr[20] = {0};

    if ( *result_flag == 0 )
   {
	   strncpy(ncpul_HOST_MESSAGE.Nationalposgeogrphic_datalen, "017",3);
	   strcpy(ncpul_HOST_MESSAGE.Nationalposgeographic_data,"00608         608");
   }

}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_60
 *
 *  DESCRIPTION:  This function will move the Advice/Reversal Reason Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:      Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_60 ( pINT result_flag, pCHAR err_buf )
{
	/* the below message is for pinless txn
	 *  length= 6 bytes
	 *  2 bytes are byte map
	 *	sub fld 1 :
 	 *  sub fld 2 :
	 * Added dummy data need to modify
	 * */
   strncpy( ncpul_HOST_MESSAGE.Advice_reason_code_len, "006", PULSE_DE60_LEN_SIZE );
   strncpy(  ncpul_HOST_MESSAGE.Advice_reason_code,"C0",2);
   strncpy(  ncpul_HOST_MESSAGE.Advice_reason_code+2,"2300",4);
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_61
 *
 *  DESCRIPTION:  This function will move the Acquirer Transport Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *  DATA ELEMENT FORMAT :ans 100 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_61 ( pINT result_flag, pCHAR err_buf )
{
	// need to check from where  to fill into host struct
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_62
 *
 *  DESCRIPTION:  This function will move the Additional Authorization Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *
 *	DATA ELEMENT FORMAT :ans-b…999 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_62 ( pINT result_flag, pCHAR err_buf )
{	
	BYTE temp_len[4] = {0};
	INT len = 0;
	len = strlen(Auth_Tx.TLF01_details.invoice_number);
	if(  true == ncpul_is_issuing_transaction() )
	{
	}
	else if ( *result_flag == 0 )
	{
		// Optional filed check for later

	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_63
 *
 *  DESCRIPTION:  This function will move the Issuer Transport Data
 *                to the Host Message structure according to DCI  host
 *                formatting specifications.
 *
 *
 *	DATA ELEMENT FORMAT :ans-b… 50 LLLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_63 ( pINT result_flag, pCHAR err_buf )
{
	BYTE temp_len[4] = {0};
	INT len = 0;
  // add additional comments for the field formation
	/* pulse assigned terminal id
	of 6 bytes is assigned as 000000 for temporary */
	if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE	)
	{
	/*	strncpy(ncpul_HOST_MESSAGE.Pulse_data_len,"017",3);
		strncpy(ncpul_HOST_MESSAGE.Pulse_data,"D0076100PULPRCD44",17);*/
		strncpy(temp_len,Auth_Tx.TLF01_details.ncf01_retired_cwk,PULSE_DE63_LEN_SIZE);
		len = atoi(temp_len);
		if(len > 25)
		{
			strncpy(ncpul_HOST_MESSAGE.Pulse_data_len,"017",3);
			strncpy(ncpul_HOST_MESSAGE.Pulse_data,"D0076100PULPRCD44",17);
		}
		else
		{
			strncpy(ncpul_HOST_MESSAGE.Pulse_data_len,Auth_Tx.TLF01_details.ncf01_retired_cwk,PULSE_DE63_LEN_SIZE);
			strncpy(ncpul_HOST_MESSAGE.Pulse_data,Auth_Tx.TLF01_details.ncf01_retired_cwk+PULSE_DE63_LEN_SIZE,len);
		}

	}
	else
	{
			strncpy(ncpul_HOST_MESSAGE.Pulse_data_len,"011",3);
		// Bit map
		strncpy(ncpul_HOST_MESSAGE.Pulse_data,"C0",2);
		// Pesudo terminal
		strncat(ncpul_HOST_MESSAGE.Pulse_data,"076100",6);
		//Acq Network ID
		strncat(ncpul_HOST_MESSAGE.Pulse_data,"PUL",3);
		// Processor ID
		//strncpy(ncpul_HOST_MESSAGE.Pulse_data,"82076100  0  ",13);	
		}

}



/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_70
 *
 *  DESCRIPTION:  This function will move the Network Management Information Code
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
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_70 ( pINT result_flag, pCHAR err_buf )
{

   INT  ret_val = true;
   INT  len;
   BYTE temp_len[3]="";

   /* 061 -  Sign on as issuer
      * 071 -  Sign on as acquirer
      * Still many are there need to know the logic to fill them all */
   /*we are assuming ncpul as ACQ only*/
   if(strncmp(Auth_Tx.TLF01_details.message_type+1,"80",2)==0)
   {
	   memcpy( ncpul_HOST_MESSAGE.Network_Management_info_code, Auth_Tx.function_code, PULSE_DE70_SIZE);
	   extendedbit= true;
   }


}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_90
 *
 *  DESCRIPTION:  This function will move the Original Data Elements
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
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void ncpul_build_request_field_90 ( pINT result_flag, pCHAR err_buf )
{
	BYTE orig_data[43] = {0};
	BYTE temp_data1[12] = {0};
	BYTE temp_data2[12] = {0};
	INT count = 0;
	INT  ret_val = true;

// copy of dci DE56 as same fileds are filled
	/* copied from cup as both spec explain the same.
	+-------------------------------------------------+
	Feild 90 details
	| BYTES  1- 4:  Original Message Type    |
	| BYTES  5-10:  STAN of Original message          |
	| BYTES 11-20:  Trans date & time of original msg |
	| BYTES 21-31:  Acquiring Institution Code        |
	| BYTES 32-42:  Forwarding Institution Code = '0' |
	+-------------------------------------------------+
	*/
	if ( *result_flag == 0 )
	{
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT||
		Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE)
		{
			//ECHO FLD 90 Data
			memset (orig_data, 0, sizeof(orig_data));

			strcat (orig_data, "0200");

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
				strncpy(temp_data1, Auth_Tx.orig_acq_inst_id, PULSE_MAX_DE32_SIZE);
			}
			else
			{
				strncpy(temp_data1, Ncf01_I.acquirer_inst_id_code, PULSE_MAX_DE32_SIZE);
			}
			ncpul_Rj_with_lead_zeros(temp_data1,PULSE_MAX_DE32_SIZE);
			strcat (orig_data, temp_data1);

			strncpy(temp_data2,Ncf01_I.forwarding_inst_id_code , PULSE_MAX_DE33_SIZE);
			ncpul_Rj_with_lead_zeros(temp_data2,PULSE_MAX_DE33_SIZE);

			strcat (orig_data, temp_data2);
			extendedbit= true;

			memcpy (ncpul_HOST_MESSAGE.Original_data,orig_data, sizeof(ncpul_HOST_MESSAGE.Original_data));

		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE  ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE)
		{
			//ECHO FLD 90 Data
			memset (orig_data, 0, sizeof(orig_data));
			strcat (orig_data, Auth_Tx.TLF01_details.orig_message);

			strcat (orig_data, Auth_Tx.TLF01_details.orig_sys_trace_num);

			strcat (orig_data, Auth_Tx.orig_local_time_date);

			strncpy(temp_data1, Auth_Tx.orig_acq_inst_id, PULSE_MAX_DE32_SIZE);
			ncpul_Rj_with_lead_zeros(temp_data1,PULSE_MAX_DE32_SIZE);
			strcat (orig_data, temp_data1);

			strncpy(temp_data2,Ncf01_I.forwarding_inst_id_code , PULSE_MAX_DE33_SIZE);
			ncpul_Rj_with_lead_zeros(temp_data2,PULSE_MAX_DE33_SIZE);
			strcat (orig_data, temp_data2);
			memcpy (ncpul_HOST_MESSAGE.Original_data,orig_data, sizeof(ncpul_HOST_MESSAGE.Original_data));
			extendedbit= true;
		}
		else if ( strlen(Auth_Tx.TLF01_details.orig_message)> 0)
		{
			memcpy (orig_data , ncpul_HOST_MESSAGE.Original_data, sizeof(ncpul_HOST_MESSAGE.Original_data));
			if((orig_data[1] == '0') && (Auth_Tx.TLF01_details.message_type[1]=='4'))
			{
				sprintf( err_buf, "Invalid data for original data elements %s %s %s ",
				Auth_Tx.TLF01_details.terminal_id,
				Auth_Tx.TLF01_details.card_num,
				Auth_Tx.TLF01_details.merchant_id);
				ncpul_log_message( 2, 3, err_buf, "build_request_field_90",1);

				return ;
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
					Auth_Tx.TLF01_details.sys_trace_audit_num, PULSE_DE11_SIZE);
				}
				memcpy(orig_data+4,Auth_Tx.TLF01_details.orig_sys_trace_num,  PULSE_DE11_SIZE);
				memcpy(orig_data+10 ,Auth_Tx.orig_local_time_date, PULSE_DE7_SIZE);
				memcpy(orig_data+20, Auth_Tx.orig_acq_inst_id,PULSE_MAX_DE32_SIZE);

				/* Actually forwarding institution id code */
				memcpy(orig_data+31, Auth_Tx.amex_pos_entry_mode,PULSE_MAX_DE33_SIZE);
				extendedbit= true;
				memcpy (ncpul_HOST_MESSAGE.Original_data,orig_data, sizeof(ncpul_HOST_MESSAGE.Original_data));
			}
		}
	}

}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_91
 *
 *  DESCRIPTION:  This function will move the File Update Code
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_91 ( pINT result_flag, pCHAR err_buf )
{
	// copied from mcrd and need to know logic or the field to decide what value to fill in pulse

	   /* mandatory  for 0302 and 0312
	   switch(Auth_Tx->TLF01_details.nii[0])
	   {
         case CHOICE_ADD:     ncpul_HOST_MESSAGE.file_code[0] = '1';  break;
         case CHOICE_UPDATE:  ncpul_HOST_MESSAGE.file_code[0] = '2';  break;
         case CHOICE_DELETE:  ncpul_HOST_MESSAGE.file_code[0] = '3';  break;
         default:              break;
	   } */

}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_95
 *
 *  DESCRIPTION:  This function will move the	Replacement Amounts
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_95 ( pINT result_flag, pCHAR err_buf )
{
	CHAR tmp_rplcmnt_amt[43] = {0} ;

	/*
	 * sud fld 1. Actual ammount, transaction - 12 bytes
	 * sub fld 2. Actual ammount, settlement  - 12 bytes
	 * sub fld 3. Actual Amount, Transaction Fee - x + 8 bytes
	 * sub fld 3. Actual Amount, Settlement Fee - x + 8 bytes
	 * */
	strncpy(tmp_rplcmnt_amt,Auth_Tx.TLF01_details.total_amount,12);
	strncat(tmp_rplcmnt_amt,Auth_Tx.TLF01_details.total_amount,12);
	strncat(tmp_rplcmnt_amt,"D",1);
	strncat(tmp_rplcmnt_amt,Auth_Tx.TLF01_details.tran_fee_amount,8);
	strncat(tmp_rplcmnt_amt,"D",1);
	//strncat(tmp_rplcmnt_amt,"",8);/* if 29 done this can be filled */
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_96
 *
 *  DESCRIPTION:  This function will move the Message Security Code
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_96 ( pINT result_flag, pCHAR err_buf )
{
	memcpy(ncpul_HOST_MESSAGE.Msg_security_code, "76576   ", PULSE_DE96_SIZE);
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_98
 *
 *  DESCRIPTION:  This function will move the Payee to the Host Message structure
 *  			  according to PULSE  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_98 ( pINT result_flag, pCHAR err_buf )
{
	/* condtional and required for 020,021,022,042*/
 }

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_101
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_101 ( pINT result_flag, pCHAR err_buf )
{
	   /* required only for 0302 , hence check for later*/
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_102
 *
 *  DESCRIPTION:  This function will move the	Account Identification 1
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_102 ( pINT result_flag, pCHAR err_buf )
{
   if ( Auth_Tx.acct_id_1[0] != 0x00 )
   {
      if ( Auth_Tx.TLF01_details.tx_key > AUTH_INVALID )
      {
         /* Response */
         if ( Auth_Tx.FAF01_details.velocity[0].amount[0] == '1' )
         {
            /* This flag indicates that this field came in the request. */
        	 ncpul_HOST_MESSAGE.Account_identificaton1_len[0] = strlen(Auth_Tx.acct_id_1 );
             memcpy( ncpul_HOST_MESSAGE.Account_identificaton1,
            		Auth_Tx.acct_id_1,
					ncpul_HOST_MESSAGE.Account_identificaton1_len[0] );
         }
      }
      else
      {
    	  ncpul_HOST_MESSAGE.Account_identificaton1_len[0] = strlen(Auth_Tx.acct_id_1 );
          memcpy( ncpul_HOST_MESSAGE.Account_identificaton1,
        		 Auth_Tx.acct_id_1,
				 ncpul_HOST_MESSAGE.Account_identificaton1_len[0] );
      }
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_103
 *
 *  DESCRIPTION:  This function will move the	Account Identification 2
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_103 ( pINT result_flag, pCHAR err_buf )
{
   if ( Auth_Tx.acct_id_2[0] != 0x00 )
   {
      if ( Auth_Tx.TLF01_details.tx_key > AUTH_INVALID )
      {
         /* Response */
         if ( Auth_Tx.FAF01_details.velocity[0].amount[0] == '1' )
         {
            /* This flag indicates that this field came in the request. */
        	 ncpul_HOST_MESSAGE.Account_identificaton2_len[0] = strlen(Auth_Tx.acct_id_2 );
             memcpy( ncpul_HOST_MESSAGE.Account_identificaton2,
            		Auth_Tx.acct_id_2,
					ncpul_HOST_MESSAGE.Account_identificaton2_len[0] );
         }
      }
      else
      {
    	  ncpul_HOST_MESSAGE.Account_identificaton2_len[0] = strlen(Auth_Tx.acct_id_2 );
          memcpy( ncpul_HOST_MESSAGE.Account_identificaton2,
        		 Auth_Tx.acct_id_2,
				 ncpul_HOST_MESSAGE.Account_identificaton2_len[0] );
      }
   }
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_104
 *
 *  DESCRIPTION:  This function will move the Transaction Description
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_104 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_105
 *
 *  DESCRIPTION:  This function will move the Large Private Data 1
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_105 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_109
 *
 *  DESCRIPTION:  This function will move the Sender Data
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_109 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_110
 *
 *  DESCRIPTION:  This function will move the Receiver Data
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_110 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_111
 *
 *  DESCRIPTION:  This function will move the Additional Data, Private Acquirer
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_111 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_113
 *
 *  DESCRIPTION:  This function will move the Authorizing Agent Institution ID Code (ANSI-defined)
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_113 ( pINT result_flag, pCHAR err_buf )
{
	INT itemp = 0 ;

   if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.agent_id ) )
   {
      itemp = strlen( Auth_Tx.TLF01_details.agent_id );
      sprintf( ncpul_HOST_MESSAGE.Auth_agent_inst_id_code_len, "%03d", itemp );
      strcpy( ncpul_HOST_MESSAGE.Auth_agent_inst_id_code, Auth_Tx.TLF01_details.agent_id );
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_114
 *
 *  DESCRIPTION:  This function will move the	Country Code, Authorizing Agent (ANSI-defined)
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - BDO Cadencie host specific structure (Global)
 *
 *  RTRN VALUE:
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_114 ( pINT result_flag, pCHAR err_buf )
{
   memcpy( ncpul_HOST_MESSAGE.Country_code, Auth_Tx.country_code, PULSE_DE114_LEN_SIZE );
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_120
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_120 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_121
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_121 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_122
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_122 ( pINT result_flag, pCHAR err_buf )
{
	/* need to knwo from where to fill into host message*/
}

/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_123
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_123 ( pINT result_flag, pCHAR err_buf )
{
	/* need to knwo from where to fill into host message*/
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_124
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_124 ( pINT result_flag, pCHAR err_buf )
{
	/* need to know from where to fill into host message*/
}


/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_125
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_125 ( pINT result_flag, pCHAR err_buf )
{
	/* need to know from where to fill into host message*/
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_126
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_126 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later*/
}
/******************************************************************************
 *
 *  NAME:         ncpul_build_request_field_127
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
 *                ncpul_Host_Message- PULSE  host specific structure (Global)
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_build_request_field_127 ( pINT result_flag, pCHAR err_buf )
{
	   /*   check for later  */
}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_2
 *
 *  DESCRIPTION:  This function will move the card number from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n, 19 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_move_field_2( pINT result_flag, pCHAR err_buf )
{
	INT   len = 0;
	BYTE  tem_len[3] = {0};

	/* First get the length of the card number. */
	memcpy( Auth_Tx.TLF01_details.card_num_len,	ncpul_HOST_MESSAGE.pan_len,
			PULSE_DE2_LEN_SIZE );

	memcpy( tem_len,Auth_Tx.TLF01_details.card_num_len,
			PULSE_DE2_LEN_SIZE);

	len = atoi(tem_len);
	if ( len <= PULSE_MAX_DE2_SIZE )
	{
		memcpy( Auth_Tx.TLF01_details.card_num,ncpul_HOST_MESSAGE.pan, len );
		if (false == ncpul_isnum(Auth_Tx.TLF01_details.card_num))
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
 *  NAME:         ncpul_move_field_3
 *
 *  DESCRIPTION:  This function will move the processing code from the Host
 *                Message structure according to DCI Standard host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncpul_move_field_3( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		if(ncpul_HOST_MESSAGE.proccode[0] != 0x00)
		{
			memcpy( Auth_Tx.TLF01_details.processing_code,
					ncpul_HOST_MESSAGE.proccode,
					PULSE_DE3_SIZE );
      		if( strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 ||
      			strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",3) == 0	)
      		{
      			// do nothing as you copied it in while sendig the request
      		}
      		else
      		{
    			/* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
    			memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,
    					ncpul_HOST_MESSAGE.proccode,
    					PULSE_DE3_SIZE );
      		}

			if (false == ncpul_isnum(Auth_Tx.TLF01_details.processing_code))
			{
				sprintf( err_buf,
						 "Non-numeric processing code (field 3) %s: for card number %s,",
						  Auth_Tx.TLF01_details.processing_code,
						  Auth_Tx.TLF01_details.card_num );
						 *result_flag = 1;
			}
		}
		else
		{
			sprintf( err_buf, "processing code is not present (field 3) for card number %s:",
					 Auth_Tx.TLF01_details.card_num);
			*result_flag = 1;
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_4
 *
 *  DESCRIPTION:  This function will move the amount from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_4( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.amount[0] != 0x00)
   		{
   		    memcpy( Auth_Tx.TLF01_details.total_amount,
        	       ncpul_HOST_MESSAGE.amount,
            	    PULSE_DE4_SIZE );
			  
		  	if (false ==(ret_val = ncpul_isnum(Auth_Tx.TLF01_details.total_amount)))
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
 *  NAME:         ncpul_move_field_5
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_5( pINT result_flag, pCHAR err_buf )
{
	//Need to find out a field that we will using to store this data
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_6
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing amount from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_6( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0 ;

	if ( *result_flag == 0 )
	{
		if(ncpul_HOST_MESSAGE.ch_bill_amt[0] != 0x00)
		{
			memcpy( Auth_Tx.ch_billing_amt,
					ncpul_HOST_MESSAGE.ch_bill_amt,
					PULSE_DE6_SIZE );

			 if (false == ncpul_isnum(Auth_Tx.ch_billing_amt))
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
 *  NAME:         ncpul_move_field_7
 *
 *  DESCRIPTION:  This function will move the transmission time from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 * DATA ELEMENT FORMAT :n 10 MMDDhhmmss
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20] = {0};
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.gmt_time[0] != 0x00 )
   		{
	       	 memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
        	    	 ncpul_HOST_MESSAGE.gmt_time,
            		 PULSE_DE7_SIZE );
			 
			 ret_val = ncpul_isnum(Auth_Tx.TLF01_details.transmission_timestamp);
         	 if ( ret_val == false )
         	 {
            	*result_flag = 1;
           		 sprintf( err_buf,
           				  "Non-numeric Transmission Timestamp (field 7): %s,",
						  Auth_Tx.TLF01_details.transmission_timestamp );
         	}
         	else if ( false == ncpul_validate_date_time(Auth_Tx.TLF01_details.transmission_timestamp))
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
 *  NAME:         ncpul_move_field_9
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
void ncpul_move_field_9( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20];
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(ncpul_HOST_MESSAGE.convRateSettlement[0] != 0x00)
   	  {
      		memcpy( Auth_Tx.TLF01_details.conversion_rate,
      				ncpul_HOST_MESSAGE.convRateSettlement,
					PULSE_DE9_SIZE );
			ret_val = ncpul_isnum(Auth_Tx.TLF01_details.conversion_rate);
			if ( ret_val == false )
        	{
           		 *result_flag = 1;
           		  sprintf( err_buf,
               		  	   "Non-numeric Settlement Conversion Rate (field 9): %s,",
                   		   Auth_Tx.TLF01_details.conversion_rate );
         	}
   	  	}
     }


}



/******************************************************************************
 *
 *  NAME:         ncpul_move_field_10
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
void ncpul_move_field_10( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20];
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.convRateCardHolder[0] != 0x00)
   		{
	   		memcpy( Auth_Tx.ch_billing_conv_rate,
	   				ncpul_HOST_MESSAGE.convRateCardHolder,
	 		 		PULSE_DE10_SIZE );
			ret_val = ncpul_isnum(Auth_Tx.ch_billing_conv_rate);
			if ( ret_val == false )
         	{
           		 *result_flag = 1;
           		 sprintf( err_buf,
               	 	   	 "Non-numeric Cardholder Billing  Conversion Rate (field 10): %s,",
                   		  Auth_Tx.ch_billing_conv_rate );
         	}
   		}
     }


}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_11
 *
 *  DESCRIPTION:  This function will move the system trace number from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_11( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.stan[0] != 0x00)
   		{
   			memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
            	    ncpul_HOST_MESSAGE.stan,
            	    PULSE_DE11_SIZE );
			ret_val = ncpul_isnum(Auth_Tx.TLF01_details.sys_trace_audit_num);
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
 *  NAME:         ncpul_move_field_12
 *
 *  DESCRIPTION:  This function will move the transaction time from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12 YYMMDDhhmmss
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  time[13] = {0};
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(ncpul_HOST_MESSAGE.time[0] != 0x00)
   	  {
		  /* store Local Transaction  Time */
	  		memcpy( Auth_Tx.TLF01_details.product_codes[12].amount,
        		   ncpul_HOST_MESSAGE.time, PULSE_DE12_SIZE);
		  
  			memcpy(time,Auth_Tx.TLF01_details.product_codes[12].amount,PULSE_DE12_SIZE);

   	  		ret_val = ncpul_isnum(time);
      		if ( ret_val == false )
   			{
       			*result_flag = 1;
       			sprintf( err_buf,
               			 "Non-numeric transaction time (field 12): %s,",
               			 time );
   			}
			else if ( false == ncpul_validate_time(time))
    		{
       	 		ret_val = false;
	    	    *result_flag = 1;
	        	 sprintf( err_buf,
       	    		     "Invalid transaction  time (field 12): %s,",
           	    		  time );
      		}
			
   	  	}
   	}
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_13
 *
 *  DESCRIPTION:  This function will move the transaction date from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 YYMM
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_13( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  date[5] = {0};
   INT   len     = 0;
   BYTE time_date[25] = {0};
   

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.date[0] != 0x00)
   		{
   			ptetime_get_timestamp( time_date );
			/*Store system date year YYYY */
			memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd, 
	    	      	 "0000", PULSE_YYYY_LEN);
   			/* Date  MMDD Format*/
			  memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd[PULSE_MMDD_START_POS],
	    	      	 ncpul_HOST_MESSAGE.date, PULSE_MMDD_LEN);
	  
	 		  memcpy( date,Auth_Tx.TLF01_details.date_yyyymmdd + 
	  	     		  PULSE_MMDD_START_POS, PULSE_MMDD_LEN);
			  
              ret_val = ncpul_isnum(date);
		      if ( ret_val == false )
      		  {
			      *result_flag = 1;
         		  sprintf( err_buf,
              		 	  "Non-numeric transaction date (field 13): %s,",
                  		   date );
		      }
			  else if ( false == ncpul_validate_date_mmdd(date))
			  {
        		 *result_flag = 1;
				  sprintf( err_buf,
               			 "Invalid transaction date (field 13): %s,",
			              date );
      		}
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_14
 *
 *  DESCRIPTION:  This function will move the Expire date from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 4 YYMM
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_14( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
	  if(ncpul_HOST_MESSAGE.date_expd[0] != 0x00)
	  {
		  memcpy( Auth_Tx.TLF01_details.exp_date,
				  ncpul_HOST_MESSAGE.date_expd,
				  PULSE_DE14_SIZE);

		   ret_val = ncpul_isnum(Auth_Tx.TLF01_details.exp_date);
		   if ( ret_val == false )
      	   {
		        *result_flag = 1;
        		 sprintf( err_buf,
                		  "Non-numeric Expire date (field 14): %s,",
                  		  Auth_Tx.TLF01_details.exp_date );
      		}
	  	}
	  else
	  {

		 // no expiry date code here.
	  }
   	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_15
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
void ncpul_move_field_15( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(ncpul_HOST_MESSAGE.SettleDate[0] != 0x00)
   	  {
	   	   	  /* Settlement Date */
		      memcpy( Auth_Tx.TLF01_details.settlement_date,
		    		  ncpul_HOST_MESSAGE.SettleDate,
				  	  PULSE_DE15_SIZE );
			  ret_val = ncpul_isnum(Auth_Tx.TLF01_details.settlement_date);
	      	  if ( ret_val == false )
	      	  {
			     *result_flag = 1;
	        	 sprintf( err_buf,
	              		 "Non-numeric Settlement date (field 15): %s,",
	               	 	  Auth_Tx.TLF01_details.settlement_date);
	      	  }
		}
   }

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_16
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
void ncpul_move_field_16( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if (ncpul_HOST_MESSAGE.conversionDate[0] != 0x00)
   	  {
   		  /* Settlement Date */
	      memcpy( Auth_Tx.conv_date,
	    		  ncpul_HOST_MESSAGE.conversionDate,
			  	  PULSE_DE16_SIZE );
		  ret_val = ncpul_isnum(Auth_Tx.conv_date);
      	  if ( ret_val == false )
      	  {
		    *result_flag = 1;
        	 sprintf( err_buf,
               		 "Non-numeric Conversion date (field 16): %s,",
               	 	  Auth_Tx.conv_date);
      	  }
   	  }
   }

}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_18
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
void ncpul_move_field_18( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(ncpul_HOST_MESSAGE.Merch_type[0] != 0x00)
   	   {
   	   		if ( Auth_Tx.MCF01_details.voice_category_code[0] != 0x00 )
   	   		{
   				memcpy( Auth_Tx.MCF01_details.voice_category_code,
   						ncpul_HOST_MESSAGE.Merch_type,
	    	    		PULSE_DE18_SIZE );

				ret_val = ncpul_isnum(Auth_Tx.MCF01_details.voice_category_code);
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
						ncpul_HOST_MESSAGE.Merch_type,
		            	PULSE_DE18_SIZE );
				ret_val =ncpul_isnum(Auth_Tx.TLF01_details.category_code);
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

}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_19
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
void ncpul_move_field_19( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(ncpul_HOST_MESSAGE.Acq_country_code[0] != 0x00)
   	   {
	   		memcpy( Auth_Tx.country_code,
	   				ncpul_HOST_MESSAGE.Acq_country_code,
            	    PULSE_DE19_SIZE );

			ret_val = ncpul_isnum(Auth_Tx.country_code);
      		if ( ret_val == false )
      		{
		      	*result_flag = 1;
        		 sprintf( err_buf,
               			 "Non-numeric Merchant Country Code (field 19): %s,",
               		 	  Auth_Tx.conv_date);
      		}
		}
   }

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_22
 *
 *  DESCRIPTION:  This function will move the POS Entry Mode from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :an 12
 *
 *                Prepend '0' to put it into Hypercom ISO format.
 *
 *  INPUTS:       ncpul_Host_Message- DCI  host specific structure (Global)
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
void ncpul_move_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.posentry[0] != 0x00)
   		{
   			memcpy (Auth_Tx.TLF01_details.pos_entry_mode,
   					ncpul_HOST_MESSAGE.posentry, PULSE_DE22_SIZE);
      		if( strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 ||
      			strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",3) == 0	)
      		{
      			// do nothing as you copied it in while sendig the request
      		}
      		else
      		{
       			memcpy (Auth_Tx.TLF01_details.product_codes[9].code,
       					Auth_Tx.TLF01_details.pos_entry_mode,PULSE_DE22_SIZE);
      		}

			ret_val = ncpul_isalphanum(Auth_Tx.TLF01_details.pos_entry_mode);
	      	if ( ret_val == false )
    		{
			     *result_flag = 1;
			      sprintf( err_buf,
                	 	  "Invalid POS Entry Mode (field 22): %s,",
						  ncpul_HOST_MESSAGE.posentry );
      		}
	      	else
	      	{
	      		ncpul_populate_iso_pos_entry_mode();
	      	}
		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_23
 *
 *  DESCRIPTION:  This function will move the Card Sequence Number from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host  specific structure (Global)
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
void ncpul_move_field_23( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   	    if(ncpul_HOST_MESSAGE.card_seq_num[0] != 0x00)
   	    {
   	    	memcpy( Auth_Tx.EMV_details.pan_sequence_number , 
				    ncpul_HOST_MESSAGE.card_seq_num,
				    PULSE_DE23_SIZE );
				
			ret_val = ncpul_isnum(ncpul_HOST_MESSAGE.card_seq_num);
		    if ( ret_val == false )
	    	{
			      *result_flag = 1;
			      sprintf( err_buf,
	               	 	  "Non-numeric Card Sequence Number (field 23): %s,",
						   ncpul_HOST_MESSAGE.card_seq_num );
	      	}
   	    }
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_26
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Business Code from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :n 4
 *
 *  INPUTS:       ncpul_Host_Message- Equitable specific structure (Global)
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
void ncpul_move_field_26( pINT result_flag, pCHAR err_buf )
{
	   INT   ret_val = true;
	   INT   len     = 0;
// copied from cup as both spec explains the same
	   if ( *result_flag == 0 )
	   {
	   	   if(ncpul_HOST_MESSAGE.Pos_pin_capture_code[0] != 0x00)
	   	   {
		   	   memcpy( Auth_Tx.TLF01_details.product_codes[17].code,
		   			ncpul_HOST_MESSAGE.Pos_pin_capture_code,
					  PULSE_DE26_SIZE );

				ret_val = ncpul_isnum(Auth_Tx.TLF01_details.product_codes[17].code);
				if ( ret_val == false )
				{
					*result_flag = 1;
					sprintf( err_buf,
						 	"Non-numeric Point of Service PIN Capture Code (field 26): %s,",
							 Auth_Tx.TLF01_details.product_codes[17].code );
				}
	   	   	}
	     }

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_28
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
void ncpul_move_field_28( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	   if(ncpul_HOST_MESSAGE.Txn_Fee_Amount[0] != 0x00)
   	   {
	   	   memcpy( Auth_Tx.TLF01_details.tran_fee_amount,
	   			ncpul_HOST_MESSAGE.Txn_Fee_Amount + 1,
				   PULSE_DE28_SIZE );

			ret_val = ncpul_isalphanum(Auth_Tx.TLF01_details.tran_fee_amount);
			if ( ret_val == false )
			{
				*result_flag = 1;
				sprintf( err_buf,
					 	"Non- alpha numeric Transaction Fee Amount (field 28): %s,",
						 Auth_Tx.TLF01_details.tran_fee_amount );
			}
   	   	}
     }

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_29
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_29( pINT result_flag, pCHAR err_buf )
{
	//Need to find out a field that we will using to store this data
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_32
 *
 *  DESCRIPTION:  This function will move the Acquiring Inst ID from the Host
 *                Message structure according to Transaction Fee Amount formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message- Transaction Fee Amount specific structure (Global)
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
void ncpul_move_field_32( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  len_str[3] = {0};

	if ( *result_flag == 0 )
	{
		if(ncpul_HOST_MESSAGE.acq_inst_len[0] != 0x00)
		{
			/* First get the length. */
			memcpy( len_str,ncpul_HOST_MESSAGE.acq_inst_len, PULSE_DE32_LEN_SIZE);
				
			len = atoi( len_str );
			if ( len <= PULSE_MAX_DE32_SIZE)
			{
				/* Now copy the Id into Auth_Tx and check it. */
				memcpy( Auth_Tx.acq_inst_id_len,ncpul_HOST_MESSAGE.acq_inst_len, PULSE_DE32_LEN_SIZE);
				memcpy( Auth_Tx.acq_inst_id_cd,ncpul_HOST_MESSAGE.acq_inst_id, len );

				if (false == ncpul_isnum(Auth_Tx.acq_inst_id_cd))
				{
					*result_flag = 1;
					sprintf( err_buf,
							 "Non-numeric acquiring inst. Id (field 32): %s,",
							 ncpul_HOST_MESSAGE.acq_inst_id );
				}
				else
				{
					/* Copy it into TLF01 for database reference. */
					memcpy( Auth_Tx.TLF01_details.acquiring_id,
							ncpul_HOST_MESSAGE.acq_inst_id,
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
 *  NAME:         ncpul_move_field_33
 *
 *  DESCRIPTION:  This function will move the Forwarding Inst ID from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_33( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0;
   CHAR  len_str[3] = {0};

   if ( *result_flag == 0 )
   {
   	  if(ncpul_HOST_MESSAGE.fwd_inst_len[0] != 0x00)
   	  {
	      /* First get the length. */
	      memcpy( len_str,ncpul_HOST_MESSAGE.fwd_inst_len, PULSE_DE33_LEN_SIZE);
	      len = atoi( len_str );
	      if ( len <= PULSE_MAX_DE33_SIZE )
	      {
	         /* Now copy the Id into Auth_Tx and check it. */
			 memcpy( Auth_Tx.forwarding_inst_id_len,ncpul_HOST_MESSAGE.fwd_inst_len,PULSE_DE33_LEN_SIZE);
	         memcpy( Auth_Tx.forwarding_inst_id_cd,ncpul_HOST_MESSAGE.fwd_inst_id, len );
	         if (false ==(ret_val = ncpul_isnum(ncpul_HOST_MESSAGE.fwd_inst_id)))
	         {
	            *result_flag = 1;
	            sprintf( err_buf,
	                    "Non-alphanumeric acquiring inst. Id (field 33): %s,",
						ncpul_HOST_MESSAGE.fwd_inst_id );
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
 *  NAME:         ncpul_move_field_35
 *
 *  DESCRIPTION:  This function will move the track2 from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 * DATA ELEMENT FORMAT :z 37 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_35( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len = 0;
	CHAR  len_str[3] = {0};
	CHAR  service_code[4] = {0};
	CHAR  error_msg[256] = {0};

	if ( *result_flag == 0 )
	{
		if(ncpul_HOST_MESSAGE.track2[0] != 0x00)
		{
			/* First get the length. */
			memcpy( len_str,ncpul_HOST_MESSAGE.track2_len, PULSE_DE35_LEN_SIZE);
			len = atoi( len_str );
			if ( len <= PULSE_MAX_DE35_SIZE )
			{
				memcpy( Auth_Tx.TLF01_details.track2 ,ncpul_HOST_MESSAGE.track2, len);
				if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
				{
					// Let us get the service code.
					if((true == ncpul_get_service_code(  service_code,  error_msg )))
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
 *  NAME:         ncpul_move_field_37
 *
 *  DESCRIPTION:  This function will move the retrieval reference number from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 12
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host  specific structure (Global)
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
void ncpul_move_field_37( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   	  if(ncpul_HOST_MESSAGE.rrn[0] != 0x00)
   	  {
		  memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
			 	  ncpul_HOST_MESSAGE.rrn, PULSE_DE37_SIZE );
			  
		  ret_val = ncpul_isalphanum_space(Auth_Tx.TLF01_details.retrieval_ref_num);
		  for( len = 0; len <PULSE_DE37_SIZE; len++)
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
						 ncpul_HOST_MESSAGE.rrn );
		  }
      }
   }
}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_38
 *
 *  DESCRIPTION:  This function will move the Authorization Number from Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 6
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host  specific structure (Global)
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
void ncpul_move_field_38( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   CHAR error_msg[256] = {0};

   if ( *result_flag == 0 )
   {
      if(ncpul_HOST_MESSAGE.authcode[0] != 0x00)
      {
	      	memcpy( Auth_Tx.TLF01_details.auth_number,
	        		ncpul_HOST_MESSAGE.authcode, PULSE_DE38_SIZE );
			
	  		ret_val = ncpul_isalphanum(Auth_Tx.TLF01_details.auth_number);
			if ( ret_val == false )
	  		{
	  			/* If RC is not " 00" , still proceed with tx*/
				if(strcmp (ncpul_HOST_MESSAGE.responsecd,PULSE_APPROVED) != 0)
				{
     				sprintf( error_msg,
     						 "Non-alphanumeric Auth Number (field 38) : %s,",
							 ncpul_HOST_MESSAGE.authcode );
					ncpul_log_message( 3, 2, error_msg, "ncpul_move_field_38",1 );
				}
				memset( Auth_Tx.TLF01_details.auth_number,0x00, PULSE_DE38_SIZE );
   			}
      	}
      else
      {

    	 // Send the blank;
      }
   	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_39
 *
 *  DESCRIPTION:  This function will move the Response Code from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *   DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host  specific structure (Global)
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
void ncpul_move_field_39( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   len     = 0;
	CHAR dci_response_code[4] = {0};

	if ( *result_flag == 0 )
	{
		if( ncpul_HOST_MESSAGE.responsecd[0] != 0x00 )
		{
			memcpy( Auth_Tx.TLF01_details.response_code,ncpul_HOST_MESSAGE.responsecd, PULSE_DE39_SIZE );
			ret_val = ncpul_isnum(Auth_Tx.TLF01_details.response_code);
			if ( ret_val == false )
			{
				   sprintf( err_buf,
							"Non-numeric response code (field 39) : %s,",
							 ncpul_HOST_MESSAGE.responsecd);

			}
		}
	}
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_40
 *
 *  DESCRIPTION:  This function will move the Service Code from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host  specific structure (Global)
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
void ncpul_move_field_40( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if( ncpul_HOST_MESSAGE.service_code[0] != 0x00 )
   		{
   			// Let us understand DE 35 service code is more important or this
   			// we are keeping DE 35 data more important
   			if(0 == strlen(Auth_Tx.TLF01_details.service_code))
   			{
   				ret_val = ncpul_isnum(ncpul_HOST_MESSAGE.service_code);
   				memcpy( Auth_Tx.TLF01_details.service_code,
   						ncpul_HOST_MESSAGE.service_code,
						PULSE_DE40_SIZE );

				if ( ret_val == false )
				{
					   sprintf( err_buf,
								"Non-numeric service code (field 40) : %s,",
								ncpul_HOST_MESSAGE.service_code);
				}
   			}
 		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_41
 *
 *  DESCRIPTION:  This function will move the terminal ID from the Host
 *                Message structure according to  DCI  host  formatting
 *                specifications.
 *
 *   DATA ELEMENT FORMAT :ans 8
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host  specific structure (Global)
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
void ncpul_move_field_41( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if(ncpul_HOST_MESSAGE.termid[0] != 0x00 )
   		{
   		    memcpy( Auth_Tx.TLF01_details.terminal_id,
              		ncpul_HOST_MESSAGE.termid, PULSE_DE41_SIZE );
 		}
   }
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_42
 *
 *  DESCRIPTION:  This function will move the Merchant ID from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 15
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_42( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if( ncpul_HOST_MESSAGE.cardaccid[0] != 0x00 )
   		{
	   		memcpy( Auth_Tx.TLF01_details.merchant_id,
             		ncpul_HOST_MESSAGE.cardaccid, PULSE_DE42_SIZE );
		}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_43
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 99 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_43( pINT result_flag, pCHAR err_buf )
{
	   INT   len     = 0;

	   if ( *result_flag == 0 )
	   {
	   		if( ncpul_HOST_MESSAGE.cardaccname[0] != 0x00 )
	   		{

			   /*
	        	   Sub-field    Position    Length                      Feild details
		              1             0            23               Merchant name or ATM location (padding with blanks)
		              2            23           12                City‘s name (padding with blanks)
		              3            36            2                State code
		              4            38            2				  Country code */

	   			memcpy( Auth_Tx.TLF01_details.card_holder_name,
	      				ncpul_HOST_MESSAGE.cardaccname,
				 		 PULSE_MER_NAME_LEN_FEILD_43 );// Max 23 Byte

			 	memcpy( Auth_Tx.TLF01_details.ticket_nbr,
			 			ncpul_HOST_MESSAGE.cardaccname+23,
				 		PULSE_CITY_LEN_FEILD_43 );// Max 12 Byte	1 byte we will over write on country name

				memcpy( Auth_Tx.TLF01_details.ticket_nbr+12," ",1);

	         	memcpy( Auth_Tx.TLF01_details.ticket_nbr+13,
	         			ncpul_HOST_MESSAGE.cardaccname+36,
						PULSE_STATE_CODE_LEN_FEILD_43 );// Max 2 Byte

	         	memcpy( Auth_Tx.TLF01_details.ticket_nbr+15,
	         			ncpul_HOST_MESSAGE.cardaccname+38,
						PULSE_STATE_CODE_LEN_FEILD_43 );// Max 2 Byte
	   		}
	   	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_44
 *
 *  DESCRIPTION:  This function will move the Additional Response Data from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 22 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-PULSE  host specific structure (Global)
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
void ncpul_move_field_44( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   {  
		if( ncpul_HOST_MESSAGE.addlrspdata[0] != 0x00 )
		{
	    	/* 1st find the length */
			memcpy( temp_len,
					ncpul_HOST_MESSAGE.addl_rsp_len,
					PULSE_DE44_LEN_SIZE);
			len = atoi (temp_len);
			
			if(len <= PULSE_MAX_DE44_SIZE) // validate data length
			{
				/* Store additional response data into host message */
				memcpy( Auth_Tx.acct_id_1 ,
						ncpul_HOST_MESSAGE.addlrspdata,
						len);// Store data after length
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
 *  NAME:         ncpul_move_field_46
 *
 *  DESCRIPTION:  This function will move the Surcharge Fee from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 * DATA ELEMENT FORMAT :an 11 LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_46( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
		if( ncpul_HOST_MESSAGE.Additional_Fees_len[0] != 0x00 )
		{
			/* 1st find the length */
			memcpy(temp_len,ncpul_HOST_MESSAGE.Additional_Fees_len,PULSE_DE46_LEN_SIZE);
			len = atoi(temp_len);

			if( len <= PULSE_MAX_DE48_SIZE) // validate data length
			{
				memcpy( Auth_Tx.TLF01_details.tax_amount ,
											ncpul_HOST_MESSAGE.Additional_Fees,
											len);
				ret_val = ncpul_isalphanum(Auth_Tx.TLF01_details.tax_amount);
				if ( ret_val == false )
				{
					  *result_flag = 1;
					  sprintf( err_buf,
							   "Non-ASCII numeric Additional Fee (field 46): %s,",
							   ncpul_HOST_MESSAGE.Additional_Fees );
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
 *  NAME:         ncpul_move_field_47
 *
 *  DESCRIPTION:  This function will move the Additional Data-Private from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_47( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

   if ( *result_flag == 0 )
   {
	   if( ncpul_HOST_MESSAGE.addl_data_len[0] != 0x00 )
	   {
			/* 1st find the length */
			memcpy( temp_len,
					ncpul_HOST_MESSAGE.addl_data_len,
					PULSE_DE48_LEN_SIZE);
			len = atoi(temp_len);
			
			if( len <= PULSE_MAX_DE48_SIZE) // validate data length
			{
				if(strlen(ncpul_HOST_MESSAGE.addl_data) < ( sizeof(Auth_Tx.TLF01_details.voice_auth_text) -1 ))
				{
					/* Store additional response data into host message */
					memcpy( Auth_Tx.TLF01_details.voice_auth_text,
							ncpul_HOST_MESSAGE.addl_data,len);
				}
				else
				{
					/* Store additional response data into host message */
					memcpy( Auth_Tx.TLF01_details.voice_auth_text ,
							ncpul_HOST_MESSAGE.addl_data,
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
 *  NAME:         ncpul_move_field_48
 *
 *  DESCRIPTION:  This function will move the Card Acceptor Name/Location from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 99 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_48( pINT result_flag, pCHAR err_buf )
{
	   INT   len     = 0;

	   // already moved from 43
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_49
 *
 *  DESCRIPTION:  This function will move the Transaction currency code from the Host
 *                Message structure according to DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_49( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0 ;

   if ( *result_flag == 0 )
   {
   		if( ncpul_HOST_MESSAGE.tran_curr_cd[0] != 0x00 )
   		{
	   		memcpy( Auth_Tx.TLF01_details.currency_code,
            	    ncpul_HOST_MESSAGE.tran_curr_cd,
            	    PULSE_DE49_SIZE );
      	}
   	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_50
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
void ncpul_move_field_50( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if( ncpul_HOST_MESSAGE.settlement_curr_cd[0] != 0x00 )
   		{
	   		memcpy( Auth_Tx.settlement_curr_code,
	   				ncpul_HOST_MESSAGE.settlement_curr_cd,
            	    PULSE_DE50_SIZE );
		    ret_val = ncpul_isalphanum(Auth_Tx.settlement_curr_code);
		    if ( ret_val == false )
		    {
		       *result_flag = 1;
		       sprintf( err_buf,
		               "Non-alphanumeric Settlement Currency Code (field 50) : %s,",
		                Auth_Tx.settlement_curr_code );
		   }
   		}
   }

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_51
 *
 *  DESCRIPTION:  This function will move the Cardholder Billing currency code from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans 3
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_51( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if( ncpul_HOST_MESSAGE.card_curr_cd[0] != 0x00 )
   		{
   			memcpy( Auth_Tx.ch_billing_curr_code, 
					ncpul_HOST_MESSAGE.card_curr_cd,
				    PULSE_DE51_SIZE);
   		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_52
 *
 *  DESCRIPTION:  This function will move the PIN Data from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :b 8
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_52( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;

   if ( *result_flag == 0 )
   {
   		if( ncpul_HOST_MESSAGE.pinblock[0] != 0x00 )
   		{
			memcpy( Auth_Tx.TLF01_details.pin_block, 
					ncpul_HOST_MESSAGE.pinblock,
				    PULSE_DE52_SIZE);
   		}
   }
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_54
 *
 *  DESCRIPTION:  This function will move the Additional Amount Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 120 LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_54( pINT result_flag, pCHAR err_buf )
{
   INT   		len     			= 0;
   BYTE 		temp_len[4] 		= {0};
   CHAR  		tempstr [512]		= {0};
   INT        	addl_amts_size			;
   int   		i				    = 0;
   CHAR  		templen [6]			= {0} ;

   if ( *result_flag == 0 )
   {
		memcpy(temp_len,ncpul_HOST_MESSAGE.AddlAmts_len,PULSE_DE54_LEN_SIZE);
		len = atoi(temp_len);

		if( len <= PULSE_MAX_DE54_SIZE)
		{
			if(strlen(ncpul_HOST_MESSAGE.AddlAmts) < (sizeof(Auth_Tx.TLF01_details.mcard_banknet) -1))
			{
				memcpy( Auth_Tx.TLF01_details.mcard_banknet ,
						ncpul_HOST_MESSAGE.AddlAmts,len);// Store data after length
			}
			else
			{
				memcpy( Auth_Tx.TLF01_details.mcard_banknet,
						ncpul_HOST_MESSAGE.AddlAmts,
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
 *  NAME:         ncpul_move_field_55
 *
 *  DESCRIPTION:  This function will move the IC Card Data from the Host
 *                Message structure according to Equitable formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :ans-b…255 LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message- Equitable specific structure (Global)
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

void ncpul_move_field_55( pINT result_flag, pCHAR err_buf )
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
	   ascii_len[0] = '0';
	   memcpy(ascii_len+1,ncpul_HOST_MESSAGE.IccData_len, 3);
	   emvLen = atoi(ascii_len);
	   genutil_asc_to_bcd( ascii_len, 4, bcd_len );

	   if(emvLen > 0 && emvLen < sizeof (ncpul_HOST_MESSAGE.IccData))
       {
		   memcpy(Auth_Tx.EMV_details.emv_block,bcd_len,2);
		   memcpy(Auth_Tx.EMV_details.emv_block+2,ncpul_HOST_MESSAGE.IccData,emvLen);
	   }
   }
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_56
 *
 *  DESCRIPTION:  This function will move the  Original Data Elements from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :n 33 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_56( pINT result_flag, pCHAR err_buf )
{
	/* need to know where to move */
}

/******************************************************************************
*
*  NAME:         ncpul_move_field_58
*
*  DESCRIPTION:  This function will move the Authorizing Agent Institution Identification
 *  			  Code (IIC) to the Host Message structure according to  DCI  host formatting
*                specifications.
*
*  DATA ELEMENT FORMAT :n 11
*
*  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_58( pINT result_flag, pCHAR err_buf )
{
  INT   ret_val = true;
  INT   len     = 0;
  CHAR temp_len[4]= {0};

  if(ncpul_HOST_MESSAGE.National_pos_cc[0] != 0x00 )
  {
	  strncpy(temp_len,ncpul_HOST_MESSAGE.National_pos_cc_len,3);
	  len = atoi(temp_len);
	 /* memcpy( Auth_Tx.TLF01_details.product_codes[5].code,
			  ncpul_HOST_MESSAGE.National_pos_cc,
			    len);

	    ret_val = ncpul_isnum(Auth_Tx.TLF01_details.product_codes[5].code);
		if ( ret_val == false )
  		{
		   *result_flag = 1;
		    sprintf( err_buf,
        		     "Non-numeric Message Reason Code (field 25): %s,",
					  ncdci_HOST_MESSAGE.pos_condcd );
  		}*/
 }

}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_59
 *
 *  DESCRIPTION:  This function will move the  Network ID Transport Data
 *                to the Host Message structure according to DCI  host formatting
 *                specifications.  
 *
 *  DATA ELEMENT FORMAT :ans 999 LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_59( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[4] = {0};

// need to know where to copy in auth
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_60
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_60( pINT result_flag, pCHAR err_buf )
{
	//Need to find out a field that we will using to store this data
}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_161
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_61( pINT result_flag, pCHAR err_buf )
{
	//Need to find out a field that we will using to store this data
}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_62
 *
 *  DESCRIPTION:  This function will move the the Additional Authorization Data
 *                to the Host Message structure according to DCI  host formatting
 *                specifications.  
 *
 *	DATA ELEMENT FORMAT :ans-b…999 LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_62( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[4] = {0};
   //Need to find out a field that we will using to store this data

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_63
 *
 *  DESCRIPTION:  This function will move the Message Authentication Code from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 * 	DATA ELEMENT FORMAT :b 8
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_63( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   CHAR  temp_len[4] = {0} ;

	
   strncpy(temp_len,ncpul_HOST_MESSAGE.Pulse_data_len,PULSE_DE63_LEN_SIZE);
   len = atoi (temp_len);
   memcpy(Auth_Tx.TLF01_details.ncf01_retired_cwk,ncpul_HOST_MESSAGE.Pulse_data_len,PULSE_DE63_LEN_SIZE);
   memcpy(Auth_Tx.TLF01_details.ncf01_retired_cwk+PULSE_DE63_LEN_SIZE,ncpul_HOST_MESSAGE.Pulse_data,len);


}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_70
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
void ncpul_move_field_70( pINT result_flag, pCHAR err_buf )
{

   INT	ret_val = true;
   INT	len;
   BYTE temp_len[3]="";

   memcpy(Auth_Tx.function_code,ncpul_HOST_MESSAGE.Network_Management_info_code, PULSE_DE70_SIZE);
   strncpy(NMI_Code,Auth_Tx.function_code,PULSE_DE70_SIZE);
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_90
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
void ncpul_move_field_90( pINT result_flag, pCHAR err_buf )
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
   		 if ( !ncpul_blank_or_null_string(ncpul_HOST_MESSAGE.Original_data,5) )
   		{
   		   	memcpy (tempstr ,ncpul_HOST_MESSAGE.Original_data,   sizeof(ncpul_HOST_MESSAGE.Original_data));
		 	if((tempstr[1] == '0') && (Auth_Tx.TLF01_details.message_type[1]=='4'))
      		{
         		sprintf( err_buf, "Invalid data for original data elements %s %s ",
					     Auth_Tx.TLF01_details.terminal_id,
		                 Auth_Tx.TLF01_details.merchant_id);
         		ncpul_log_message( 2, 3, err_buf, "build_request_field_90",1);
         		return ;
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

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_91
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_91( pINT result_flag, pCHAR err_buf )
{
	/* FIELD 91: FILE UPDATE CODE */
	if ( ncpul_HOST_MESSAGE.File_update_code[0] != 0x00 )
	{
		switch( ncpul_HOST_MESSAGE.File_update_code[0] )
		{
			case '1':  Auth_Tx.TLF01_details.nii[0] = CHOICE_ADD;      break;
			case '2':  Auth_Tx.TLF01_details.nii[0] = CHOICE_UPDATE;   break;
			case '3':  Auth_Tx.TLF01_details.nii[0] = CHOICE_DELETE;   break;
			case '5':  Auth_Tx.TLF01_details.nii[0] = CHOICE_INQUIRE;  break;
			default:   Auth_Tx.TLF01_details.nii[0] = ncpul_HOST_MESSAGE.File_update_code[0];  break;
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_95
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_95( pINT result_flag, pCHAR err_buf )
{
	INT    int_total_amount;
	double double_total_amount;
	// need to know where to move in auth
}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_96
 *
 *  DESCRIPTION:  This function will move the Key Management Data
 *  			  from the Host  Message structure according to
 *  			  DCI  host formatting  specifications.
 *
 *	DATA ELEMENT FORMAT : LLLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_96( pINT result_flag, pCHAR err_buf )
{
	   INT   ret_val = true;
	   INT   len     = 0;
	   CHAR temp_len[4] = {0};
	   if ( *result_flag == 0 )
	   {
			if (ncpul_HOST_MESSAGE.Msg_security_code[0] != 0x00 )
			{
				memcpy(Auth_Tx.amex_pos_entry_mode,ncpul_HOST_MESSAGE.Msg_security_code,PULSE_DE96_SIZE);

			}
	   	}

}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_98
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_98( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_101
 *
 *  DESCRIPTION:  This function will move the File Name 1 from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 *	DATA ELEMENT FORMAT :ans 17 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_101( pINT result_flag, pCHAR err_buf )
{
   INT   len     = 0;
   BYTE  temp_len[3] = {0};

	// used only for 0302
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_102
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_102( pINT result_flag, pCHAR err_buf )
{
	if ( *result_flag == 0 )
	{
		/* FIELD 102 ACCOUNT ID #1 */
		if ( ncpul_HOST_MESSAGE.Account_identificaton1[0] != 0x00 )
		{
			memcpy( Auth_Tx.acct_id_1, ncpul_HOST_MESSAGE.Account_identificaton1,
              sizeof(ncpul_HOST_MESSAGE.Account_identificaton1));

			/* Set flag indicating this field came in a request.
			 * At this point, we don't know if this is a request
			 * or response, but that is ok.  Use FAF01 because
			 * EB is not using the Fleet functionality at all.
			 */
			Auth_Tx.FAF01_details.velocity[0].amount[0] = '1';
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_103
 *
 *  DESCRIPTION:  This function will move the Transaction Originator Institution country code
 *				  from the Host. Message structure according to DCI  host
 *				  formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 3
 *
 *  INPUTS:       ncpul_Host_Message- PULSE  host specific structure (Global)
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
void ncpul_move_field_103( pINT result_flag, pCHAR err_buf )
{
   if ( *result_flag == 0 )
	{
		   /* FIELD 103 ACCOUNT ID #2 */
		if ( ncpul_HOST_MESSAGE.Account_identificaton2[0] != 0x00 )
		{
			memcpy( Auth_Tx.acct_id_2, ncpul_HOST_MESSAGE.Account_identificaton2,
              sizeof(ncpul_HOST_MESSAGE.Account_identificaton2));

			/* Set flag indicating this field came in a request.
			* At this point, we don't know if this is a request
			* or response, but that is ok.  Use FAF01 because
			* EB is not using the Fleet functionality at all.
			*/
			Auth_Tx.FAF01_details.velocity[0].amount[1] = '1';
		}
	}
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_104
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_104( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_105
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_105( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_109
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_109( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_110
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_110( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_111
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_111( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_113
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_113( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_114
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_114( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_120
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_120( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}
/******************************************************************************
 *
 *  NAME:         ncpul_move_field_121
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_121( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_122
 *
 *  DESCRIPTION:  This function will move the Receiving Institution
 *  			  Identification Code from the Host Message structure
 *  			  according to  DCI  host formatting specifications.
 *
 *	DATA ELEMENT FORMAT :n 11 LLVAR
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_122( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE  temp_len[3] = {0};
	// need to know where to move in auth

}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_123
 *
 *  DESCRIPTION:  This function will move the Network Reference Id (NRID) from the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *	DATA ELEMENT FORMAT :LLLVAR n 15
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_123( pINT result_flag, pCHAR err_buf )
{
    INT   len     = 0;
    CHAR  temp_len[4] = {0};

	// need to know where to move in auth
}

/******************************************************************************
 *
 *  NAME:         ncpul_move_field_124
 *
 *  DESCRIPTION:  This function will move theIssuer Transaction Qualifier the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.  
 *
 * 	DATA ELEMENT FORMAT :LLLVAR ans 999
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_124( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	CHAR  temp_len[4] = {0};

	// need to know where to move in auth
}


/******************************************************************************
 *
 *  NAME:         ncpul_move_field_125
 *
 *  DESCRIPTION:  This function will move the Replacement PIN  the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :LLLVAR b 8
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_125( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	CHAR  temp_len[4] = {0};

	#define MAX_KEY_LENGTH 32


   if ( *result_flag == 0 )
   {
		if( ncpul_HOST_MESSAGE.Network_Mngment_info[0] != 0x00 )
		{
			strncpy(temp_len,ncpul_HOST_MESSAGE.Network_Mngment_info_len,3);
			len = atoi(temp_len);
			memset(Ncf01_I.retired_cwk,0x00,sizeof(Ncf01_I.retired_cwk));
			if(len >= MAX_KEY_LENGTH )
			{
				memset(Ncf01_I.kek,0x00,sizeof(Ncf01_I.kek));
				memcpy(Ncf01_I.retired_cwk,ncpul_HOST_MESSAGE.Network_Mngment_info,MAX_KEY_LENGTH);
				memcpy(Ncf01_I.kek,ncpul_HOST_MESSAGE.Network_Mngment_info+MAX_KEY_LENGTH,len-MAX_KEY_LENGTH);
			}
			else
			{
				memcpy(Ncf01_I.retired_cwk,ncpul_HOST_MESSAGE.Network_Mngment_info,len);
			}

		}
	}

}



/******************************************************************************
 *
 *  NAME:         ncpul_move_field_126
 *
 *  DESCRIPTION:  This function will move the Replacement PIN  the Host
 *                Message structure according to  DCI  host formatting
 *                specifications.
 *
 *  DATA ELEMENT FORMAT :LLLVAR b 8
 *
 *  INPUTS:       ncpul_Host_Message-  PULSE  host specific structure (Global)
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
void ncpul_move_field_126( pINT result_flag, pCHAR err_buf )
{
	INT   len     = 0;
	CHAR  temp_len[4] = {0};
	// need to know where to move in auth
}
