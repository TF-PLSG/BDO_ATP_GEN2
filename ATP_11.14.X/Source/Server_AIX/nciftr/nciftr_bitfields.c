
/***************************************************************************
*
* Copyright (c) 2004, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      nciftr_bitfields.c
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
#include "nciftr_constants.h"
#include "nciftr_prototypes.h"
#include "nciftr_bitfields.h"



/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     TranFlow;
extern CHAR    ServiceName[];
       INT    NetworkType;
extern INT TIDctr=719; 

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;


/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

extern CHAR  nciftr_sbatmp_void_reversal_procode[7];
extern CHAR  nciftr_sbatmp_void_reversal_amt_flag[2];

extern const struct isobit BITTAB[128+1];

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
   INT  len = 0;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
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
        	 len = strlen( Auth_Tx.TLF01_details.card_num );
			 sprintf( temp_len, "%02d", len );
			 genutil_asc_to_vbcd (temp_len,
								  sizeof(HOST_MESSAGE.pan_len)*2,
							 HOST_MESSAGE.pan_len);
		 genutil_asc_to_vbcd (Auth_Tx.TLF01_details.card_num, 
							  strlen(Auth_Tx.TLF01_details.card_num),
							 HOST_MESSAGE.pan);

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
   BYTE proc_code[7] = {0};
   INT proc_code_len = 0;
   //BYTE sbatmp_proc_code_for_reversal[7] = "000000";

   if ( *result_flag == 0 )
   {
	  if(0 == strncmp( Auth_Tx.TLF01_details.handler_queue,"sbatmpA",4))
	  { 	
			/* We are doing as Cadencie is now sure how to handle Duplicate Earmaking of MP trxns in
			Cadencie* The Handling can be changed if requiremnt changes in Cadencie*/
		   if( (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0320",4))||
			   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0500",4)))
			   {
				   /* We are doing as Cadencie is now sure how to handle Duplicate Earmaking of MP trxns in
					Cadencie* The Handling can be changed if requiremnt changes in Cadencie*/
				    if ( isnum(Auth_Tx.TLF01_details.processing_code) )
					{
						genutil_asc_to_vbcd (Auth_Tx.TLF01_details.processing_code, 
											sizeof(HOST_MESSAGE.proccode)*2,
											HOST_MESSAGE.proccode) ; 
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
		   else if( (0 == strncmp(Auth_Tx.TLF01_details.message_type,"04",2))||
				    (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE))
		   {
			   genutil_asc_to_vbcd (nciftr_sbatmp_void_reversal_procode,
									sizeof(HOST_MESSAGE.proccode)*2,
									HOST_MESSAGE.proccode) ;
		   }
			   else
			   {	
					/* We are doing as Cadencie is now sure how to handle Duplicate Earmaking of MP trxns in
					Cadencie* The Handling can be changed if requiremnt changes in Cadencie*/
					if ( isnum(Auth_Tx.TLF01_details.processing_code) )
					{
						genutil_asc_to_vbcd (Auth_Tx.TLF01_details.processing_code, 
											sizeof(HOST_MESSAGE.proccode)*2,
											HOST_MESSAGE.proccode) ; 
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
	  }	
	  else
	  {
		if ( isnum(Auth_Tx.TLF01_details.processing_code) )
		{
			genutil_asc_to_vbcd (Auth_Tx.TLF01_details.processing_code, 
								sizeof(HOST_MESSAGE.proccode)*2,
								HOST_MESSAGE.proccode) ; 
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
   }
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
   BYTE amount[13] = {0};
   INT  void_rev_flag = 0;
   BYTE Void_rev_amt[13] = "000000000000";

   if ( *result_flag == 0 )
   {
	   if(0 == strncmp( Auth_Tx.TLF01_details.handler_queue,"sbatmpA",4))
	   {
		   if((Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
			  (0 == strncmp(Auth_Tx.TLF01_details.message_type,"04",2)))
		   {
			   void_rev_flag = atoi(nciftr_sbatmp_void_reversal_amt_flag);
			   if(1 == void_rev_flag)
			   {
				   genutil_asc_to_vbcd (Void_rev_amt,
										 sizeof(HOST_MESSAGE.amount)*2,
										 HOST_MESSAGE.amount) ;
			   }
			   else
			   {
				   genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount,
										 sizeof(HOST_MESSAGE.amount)*2,
										 HOST_MESSAGE.amount) ;
			   }
		   }
		   else if ( isnum(Auth_Tx.TLF01_details.total_amount) )
		   {
				if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
				{
					genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount,
										 sizeof(HOST_MESSAGE.amount)*2,
										 HOST_MESSAGE.amount) ;
				}
				else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
						Auth_Tx.TLF01_details.currency_code) )
				{
					genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount,
									 	 sizeof(HOST_MESSAGE.amount)*2,
										 HOST_MESSAGE.amount) ;
				}
				else
				{
					genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount,
									 	 sizeof(HOST_MESSAGE.amount)*2,
										 HOST_MESSAGE.amount) ;
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
	  else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  ||
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

         if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin, "LO") )
         {
          
            /* POS - Amount being reversed. */
		  	genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount, 
                       			sizeof(HOST_MESSAGE.amount)*2,
		                        HOST_MESSAGE.amount) ;
         }
      }
      else if ( isnum(Auth_Tx.TLF01_details.total_amount) )
      {
         if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
         {
            genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount, 
                       			sizeof(HOST_MESSAGE.amount)*2,
		                        HOST_MESSAGE.amount) ;
         }
         else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
                               Auth_Tx.TLF01_details.currency_code) )
         {
            genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount, 
                       			sizeof(HOST_MESSAGE.amount)*2,
		                        HOST_MESSAGE.amount) ;
         }
         else
         {
            genutil_asc_to_vbcd (Auth_Tx.TLF01_details.total_amount, 
                       			sizeof(HOST_MESSAGE.amount)*2,
		                        HOST_MESSAGE.amount) ;
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
 *  NAME:         build_request_field_7
 *
 *  DESCRIPTION:  This function will move the transmission date to 
 *		         the Host Message structure according to BDO Cadencie host
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
	CHAR  date_str [ 9] = {0};
	CHAR  time_str [ 7] = {0};
	CHAR  time_date[25] = {0};
	BYTE  temp_timestamp[11] = {0};
	if ( *result_flag == 0 )
	{
		if ( 0 == strlen(Auth_Tx.TLF01_details.transmission_timestamp) )
		{
			/* Need to create transmission time stamp. Get local time. */
			ptetime_get_timestamp( time_date );
			get_date( time_date, date_str );
			get_time( time_date, time_str );

			/* Store MMDDhhmms format in transmission_timestamp */
			/* Store MMDD in transmission_timestamp */
			memcpy( Auth_Tx.TLF01_details.transmission_timestamp, 
					date_str + MMDD_START_POS,
					MMDD_LEN);
			/* Store hhmmss in transmission_timestamp after storing DDMM*/
			memcpy( Auth_Tx.TLF01_details.transmission_timestamp + 4, 
					time_str, HHMMSS_LEN);
		}	

		memcpy( temp_timestamp,Auth_Tx.TLF01_details.transmission_timestamp,10 );//MMDDHHMMSS
		genutil_asc_to_vbcd (temp_timestamp,sizeof(HOST_MESSAGE.gmt_time)*2,
								HOST_MESSAGE.gmt_time) ;
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

	if ( *result_flag == 0 )
	{
		if(0 == strncmp( Auth_Tx.TLF01_details.handler_queue,"sbatmpA",4))
		{
			// For MP stip void, we should send original STAN as DE11 in 0420 message
			if( ((0 == strncmp(Auth_Tx.TLF01_details.message_type,"04",2))||
				 (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE)) &&
				(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"02",2)))
			{
				genutil_asc_to_vbcd (Auth_Tx.TLF01_details.orig_sys_trace_num,
									 sizeof(HOST_MESSAGE.stan)*2,
									 HOST_MESSAGE.stan) ;
			}
			else if ( isnum(Auth_Tx.TLF01_details.sys_trace_audit_num) )
			{
				if ( Auth_Tx.TLF01_details.sys_trace_audit_num[0] != 0x00)
				{
					genutil_asc_to_vbcd (Auth_Tx.TLF01_details.sys_trace_audit_num,
										 sizeof(HOST_MESSAGE.stan)*2,
										 HOST_MESSAGE.stan) ;
				}
				else
				{
					ret_val = false;
					*result_flag = 1;
					sprintf( err_buf,
							"Cannot build request msg for host. STAN missing");
				}
			}
			else
			{
				ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
						"Cannot build request msg for host. Invalid STAN: %s",
						Auth_Tx.TLF01_details.sys_trace_audit_num );
			}
		}
		else if ( isnum(Auth_Tx.TLF01_details.sys_trace_audit_num) )
		{
			if ( Auth_Tx.TLF01_details.sys_trace_audit_num[0] != 0x00)
			{
					genutil_asc_to_vbcd (Auth_Tx.TLF01_details.sys_trace_audit_num,
										 sizeof(HOST_MESSAGE.stan)*2,
										 HOST_MESSAGE.stan) ;
			}
			else
			{
				ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
						"Cannot build request msg for host. STAN missing");
			}
		}
		else
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
					"Cannot build request msg for host. Invalid STAN: %s",
					Auth_Tx.TLF01_details.sys_trace_audit_num );
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
   CHAR  time_date[25] = {0};
   CHAR  time_str[7] = {0};

   if ( *result_flag == 0 )
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
		genutil_asc_to_vbcd (time_str,sizeof(HOST_MESSAGE.time)*2,
							HOST_MESSAGE.time) ;
	
	  }
      else
      {
         if ( (0x00 == Auth_Tx.TLF01_details.time_hhmmss[0] ))
         {
            /* Need to create transaction time. Get local time. */
            ptetime_get_timestamp( time_date );
            get_time( time_date, Auth_Tx.TLF01_details.time_hhmmss );
         }
		 genutil_asc_to_vbcd (Auth_Tx.TLF01_details.time_hhmmss,
		 					 sizeof(HOST_MESSAGE.time)*2,HOST_MESSAGE.time) ;
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
   CHAR  time_date[25] = {0};
   CHAR err_buf1[100]  = {0};

   if ( *result_flag == 0 )
   {
	 	if (Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
		 {
			ptetime_get_timestamp( time_date );
			get_time( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
	 	 }
		 genutil_asc_to_vbcd (&Auth_Tx.TLF01_details.date_yyyymmdd[MMDD_START_POS],
		 					 sizeof(HOST_MESSAGE.date)*2,HOST_MESSAGE.date) ;
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

   if ( *result_flag == 0 )
   {
    	if ( 0 != strcmp( NULL_STR, Auth_Tx.TLF01_details.exp_date ) )
    	{
			   genutil_asc_to_vbcd (Auth_Tx.TLF01_details.exp_date,
		 					 sizeof(HOST_MESSAGE.date_expd)*2,HOST_MESSAGE.date_expd) ;
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
   CHAR  mobilePOS_nfc_tap2phone_pos_data[5]={0};
   CHAR  swiftpass_bdopay_credit_pos_data[5]={0};
   CHAR  ecommerce_swiftpass_BDOPay_pos_data[5] ={0};

   if ( *result_flag == 0 )
   {
	   if ( 0x00 == Auth_Tx.TLF01_details.pos_entry_mode[0] )
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
#if BDOR_68_REQ_1_MP_FLEET    /* MobilePOS-NFC Terminal for Tap2Phone Transactions */
		else if(true == nciftr_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
			memcpy (mobilePOS_nfc_tap2phone_pos_data,
					MP_FLEET_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN,
					MP_FLEET_DE22_PEM_LEN);
			genutil_asc_to_vbcd ( mobilePOS_nfc_tap2phone_pos_data,
								  sizeof(HOST_MESSAGE.posentry)*2,HOST_MESSAGE.posentry) ;
		}
		else if(true == nciftr_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{

			memcpy (mobilePOS_nfc_tap2phone_pos_data,
					MP_FLEET_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN,
					MP_FLEET_DE22_PEM_LEN);
			genutil_asc_to_vbcd ( mobilePOS_nfc_tap2phone_pos_data,
					 			 sizeof(HOST_MESSAGE.posentry)*2,HOST_MESSAGE.posentry) ;
		}
#endif
#if BDOR_68_REQ_3_MP_FLEET   /*MobilePOS_NFC Tap2Phone Transactions */
		else if(true == nciftr_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
			memcpy (swiftpass_bdopay_credit_pos_data,
					MP_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					MP_FLEET_DE22_PEM_LEN);
			genutil_asc_to_vbcd ( swiftpass_bdopay_credit_pos_data,
								  sizeof(HOST_MESSAGE.posentry)*2,HOST_MESSAGE.posentry) ;
		}
		else if(true == nciftr_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{

			memcpy (swiftpass_bdopay_credit_pos_data,
					MP_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					MP_FLEET_DE22_PEM_LEN);
			genutil_asc_to_vbcd ( swiftpass_bdopay_credit_pos_data,
					 			 sizeof(HOST_MESSAGE.posentry)*2,HOST_MESSAGE.posentry) ;
		}
#endif
#if BDOR_68_REQ_5_MP_FLEET /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

	else if(true == nciftr_check_if_trx_is_ecommerce_swiftpass_BDOPay_trx ())
	{
		memcpy (ecommerce_swiftpass_BDOPay_pos_data,
				MP_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM,
				MP_FLEET_DE22_PEM_LEN);
				genutil_asc_to_vbcd ( ecommerce_swiftpass_BDOPay_pos_data,
						 			 sizeof(HOST_MESSAGE.posentry)*2,HOST_MESSAGE.posentry) ;
   }
#endif
	   else
	   {
         memset(HOST_MESSAGE.posentry,0x30,DE22_SIZE);
		 
		 genutil_asc_to_vbcd ( Auth_Tx.TLF01_details.pos_entry_mode,
		 					 sizeof(HOST_MESSAGE.posentry)*2,HOST_MESSAGE.posentry) ;
		
	   }
   }
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
   INT   ret_val = true;
   INT     itemp = 0;
   CHAR    temp_str[4] = {0};
   CHAR    temp_str1[4] = {0};

   if ( *result_flag == 0 )
   {
    	if (Auth_Tx.EMV_details.pan_sequence_number[0] != 0x00)
		{
			  memcpy(temp_str,Auth_Tx.EMV_details.pan_sequence_number,DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );			  
		 	  genutil_asc_to_vbcd (Auth_Tx.EMV_details.pan_sequence_number, 
								  sizeof(HOST_MESSAGE.panseqnum)*2, 
								  HOST_MESSAGE.panseqnum) ;

		}
		else if ( Auth_Tx.host2_message_seq_nbr[0] != 0x00 )
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy(temp_str,Auth_Tx.host2_message_seq_nbr,DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
		 	  genutil_asc_to_vbcd (Auth_Tx.host2_message_seq_nbr, 
							      sizeof(HOST_MESSAGE.panseqnum)*2, 
							      HOST_MESSAGE.panseqnum) ;
		}
  }
  return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_24
 *
 *  DESCRIPTION:  This function will move the  Network International identifier
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
 *         *
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
INT build_request_field_24( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT     itemp = 0;
   CHAR    temp_str[4] = {0};
   CHAR    temp_str1[4] = {0};

   if ( *result_flag == 0 )
   {
   		if(strlen(Auth_Tx.TLF01_details.nii)>0)
   		{
   			genutil_asc_to_vbcd (Auth_Tx.TLF01_details.nii, 
								sizeof(HOST_MESSAGE.nii)*2, 
								HOST_MESSAGE.nii) ;
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
   char ecom_pos_condition_code[3] ={0};

   if ( *result_flag == 0 )
   {

#if BDOR_68_REQ_5_MP_FLEET /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

		if(true == nciftr_check_if_trx_is_ecommerce_swiftpass_BDOPay_trx ())
		{
			memcpy (ecom_pos_condition_code,
					POS_ECOM_CONDITION_CODE,
					DE25_SIZE);
			genutil_asc_to_vbcd (ecom_pos_condition_code,
								 sizeof(HOST_MESSAGE.pos_condcd)*2,
								 HOST_MESSAGE.pos_condcd) ;
		}
#endif
		else if ((strlen(Auth_Tx.TLF01_details.pos_condition_code)> 0) &&
	  	  (strlen(Auth_Tx.TLF01_details.pos_condition_code) <= DE25_SIZE))
      	{
			genutil_asc_to_vbcd (Auth_Tx.TLF01_details.pos_condition_code, 
								sizeof(HOST_MESSAGE.pos_condcd)*2, 
								HOST_MESSAGE.pos_condcd) ;
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
   CHAR temp_len[3] = {0};
    
   if ( *result_flag == 0 )
   {
      if(Auth_Tx.TLF01_details.track2[0] != 0x00)
	  {
         len = strlen( Auth_Tx.TLF01_details.track2 );
         //memcpy( HOST_MESSAGE.track2, Auth_Tx.TLF01_details.track2, len );

	     sprintf( temp_len, "%02d", len );
	 	 //memcpy( HOST_MESSAGE.track2_len, temp_len, TRACK_2_DATA_LEN);

		 genutil_asc_to_vbcd (temp_len, 
							sizeof(HOST_MESSAGE.track2_len)*2, 
							HOST_MESSAGE.track2_len) ;
		 genutil_asc_to_vbcd (Auth_Tx.TLF01_details.track2, 
							strlen(Auth_Tx.TLF01_details.track2),
							HOST_MESSAGE.track2) ;

		 // We need to Memset the card number and make sure DE2 is not posted.
		 memset(HOST_MESSAGE.pan_len, 0x00,1);
		 memset(HOST_MESSAGE.pan, 0x00,10);
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

   if ( *result_flag == 0 )
   {
	   if ((strlen(Auth_Tx.TLF01_details.retrieval_ref_num) > 0) && 
	       (strlen(Auth_Tx.TLF01_details.retrieval_ref_num) <= DE37_SIZE))
    	{
       		memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,DE37_SIZE);
   		}
		else if ((strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) > 0) &&
			     (strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) <= DE37_SIZE))
		{
			memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.orig_retrieval_ref_num,DE37_SIZE);
		}
		else
		{
			memset(HOST_MESSAGE.rrn, 0x00, DE37_SIZE );
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

   if ( *result_flag == 0 )
   {
      if ( false == isalphanum(Auth_Tx.TLF01_details.auth_number) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Unable to build request msg to host. Auth num invalid (%s)",
                  Auth_Tx.TLF01_details.auth_number );
      }
      else if ( Auth_Tx.TLF01_details.auth_number[0] != 0x00 )
      {
         
         memcpy( HOST_MESSAGE.authcode,
                 Auth_Tx.TLF01_details.auth_number,
                 DE38_SIZE );
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
 
   if ( *result_flag == 0 )
   {
      if ( Auth_Tx.TLF01_details.response_code[0] == 0x00 )
      {
			memcpy( HOST_MESSAGE.responsecd, "00", DE39_SIZE );
      }
      else
      {
			memcpy( HOST_MESSAGE.responsecd, Auth_Tx.TLF01_details.response_code, DE39_SIZE );
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
   CHAR temp_str[8] = {0};

   if ( *result_flag == 0 )
   {
	   if ((strlen(Auth_Tx.TLF01_details.terminal_id) > 0) && 
	       (strlen(Auth_Tx.TLF01_details.terminal_id) <= DE41_SIZE))
	   	{
	     	strcpy(temp_str, Auth_Tx.TLF01_details.terminal_id);
	  		/* If Device number less than 8, intead of NULL fills with spaces*/
      		for (i = strlen(Auth_Tx.TLF01_details.terminal_id); i <8 ; i++)
      		{
        		 temp_str[i] = ' ';
      		}
      		memcpy(HOST_MESSAGE.termid,temp_str, DE41_SIZE);
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

   if ( *result_flag == 0 )
   {
	    if ((strlen(Auth_Tx.TLF01_details.merchant_id) > 0) && 
	        (strlen(Auth_Tx.TLF01_details.merchant_id) <= DE42_SIZE))
	    {
    	 	memset(HOST_MESSAGE.cardaccid,0x20,DE42_SIZE);
		 	memcpy(HOST_MESSAGE.cardaccid,Auth_Tx.TLF01_details.merchant_id,DE42_SIZE);
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
   BYTE temp_cardaccname[41] = {0};

    /*
        	 Sub-field    Position    Length                      Feild details
	              1             1            25               Merchant name or ATM location (padding with blanks)
	              2            26           12               City�s name (padding with blanks)
	              3            38            3                Country/region Code, adopting the coding
	 */

   if ( *result_flag == 0 )
   {
	   	memset(HOST_MESSAGE.cardaccname,0x20,DE43_SIZE);
	 
		/* Transaction is a POS transaction. */
		if(strcmp(Auth_Tx.MCF01_details.name02,"")!=0)
		{
			memcpy( HOST_MESSAGE.cardaccname, 
					Auth_Tx.MCF01_details.name02, 
					MER_NAME_LEN_FEILF_43);	// Max 25 Byte
		}
		    
		memcpy( HOST_MESSAGE.cardaccname + 25, 
		 	    Auth_Tx.MCF01_details.city, 
		 	    CITY_LEN_FEILD_43);// Max 12 Byte
		 
		memcpy( HOST_MESSAGE.cardaccname +37,
		 	    COUNTRY_CODE_FEILD_43,
		 	    COUNTY_CODE_LEN_FEILD_43);// Max 3 Byte	
	 }


   return( ret_val );
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
   INT   len = 0;
   BYTE  temp_len[3] = {0};

   if ( *result_flag == 0 )
   {
      if(Auth_Tx.TLF01_details.track1[0] != 0x00)
      {
         /* Track 1 is present. Do not send track 2. */
         memset(HOST_MESSAGE.track2_len, 0x00, sizeof(HOST_MESSAGE.track2_len));
		 len = strlen( Auth_Tx.TLF01_details.track1 );
    	 sprintf( HOST_MESSAGE.track1_len, "%02d", len );
		 memcpy( HOST_MESSAGE.track1, Auth_Tx.TLF01_details.track1, len );

		 // We need to Memset the card number and make sure DE2 is not posted.
		 memset(HOST_MESSAGE.pan_len, 0x00,1);
		 memset(HOST_MESSAGE.pan, 0x00,10);
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
	CHAR  temp_len[5] = {0};

	if( *result_flag == 0 )
	{
		if( strlen(Auth_Tx.TLF01_details.cvc_data)> 0 )
		{
			len = strlen(Auth_Tx.TLF01_details.cvc_data);
			sprintf( temp_len, "%03d", len );
		    memcpy( HOST_MESSAGE.addl_data_len, temp_len, DE48_LEN_SIZE);
			memcpy( HOST_MESSAGE.addl_data,Auth_Tx.TLF01_details.cvc_data,len);
		}
		else if(strlen(Auth_Tx.TLF01_details.interest_rate)> 0)
		{
			len = strlen(Auth_Tx.TLF01_details.interest_rate);
			sprintf( temp_len, "%03d", len );
			memcpy( HOST_MESSAGE.addl_data_len, temp_len, DE48_LEN_SIZE);
			memcpy( HOST_MESSAGE.addl_data,Auth_Tx.TLF01_details.interest_rate,len);
		}
	}
	return(true);
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
   INT  len = 0;
   CHAR tempstr[20] = {0};


   if ( *result_flag == 0 )
   {
      /* Field is mandatory. Zero fill it if it does not exist. */
      if ( Auth_Tx.TLF01_details.pin_block[0] == 0x00 )
      {
         memset( HOST_MESSAGE.pinblock, 0x00, DE52_SIZE );
      }
      else
      {
         memset( HOST_MESSAGE.pinblock, 0x00, DE52_SIZE );
         memset( tempstr, 0x00, sizeof(tempstr) );

         /* Convert to Hex */
         ascii_to_bin_with_length( Auth_Tx.TLF01_details.pin_block,
                       strlen(Auth_Tx.TLF01_details.pin_block),
                       &tempstr[0], sizeof(tempstr) );
         len = strlen( tempstr );
         if ( len > DE52_SIZE )
         {
            ret_val = false;
            *result_flag = 1;
            strcpy( err_buf, "Unable to build request msg for host. PIN too long." );
         }
         else
         {
            memcpy( HOST_MESSAGE.pinblock, tempstr, strlen(tempstr) );
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
   INT  len = 0;
   CHAR temp_len[3] = {0};

   /*We will store  Security Info in mac_key of Auth_tx. feild */

   //mac_key
   if ( *result_flag == 0 )
   {
		if ( strlen (Auth_Tx.mac_key ) > 0)
		{
			 genutil_asc_to_vbcd (Auth_Tx.mac_key, 
								sizeof(HOST_MESSAGE.sec_cntl)*2, 
								HOST_MESSAGE.sec_cntl) ;
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
   INT  len = 0;
   
   if ( *result_flag == 0 )
   {

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
	BYTE temp_str[100] = {0};
	INT len = 0;
	INT  ret_val = true;
		
	if ( (Auth_Tx.EMV_details.emv_block[0] != 0x00) ||
			(Auth_Tx.EMV_details.emv_block[1] != 0x00)	)
	    { 
			  memcpy( HOST_MESSAGE.IccData_len, Auth_Tx.EMV_details.emv_block, 2 );
			  len = genutil_bcd_to_int( HOST_MESSAGE.IccData_len, 2);
	
			  memcpy( &HOST_MESSAGE.IccData, Auth_Tx.EMV_details.emv_block+2, len );
	    }
		return( ret_val );
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
   CHAR temp_len[4] = {0};

   if ( *result_flag == 0 )
   	{
	      if(0 == strncmp( Auth_Tx.TLF01_details.message_type, "0500", 4))
		  {
			 /* It is a settlement or settlement trailer, msg types 0500 */
				len= strlen(Auth_Tx.TLF01_details.batch_number);
			 sprintf(temp_len, "%04d", len);
			 genutil_asc_to_vbcd (temp_len,
								  sizeof(HOST_MESSAGE.priv60_len)*2,
								  HOST_MESSAGE.priv60_len);
			 memcpy ( HOST_MESSAGE.priv60, Auth_Tx.TLF01_details.batch_number,len);
		  }

	   	  else if((0 == strncmp( Auth_Tx.TLF01_details.message_type, "0200", 4)) ||
	   			  (0 == strncmp( Auth_Tx.TLF01_details.message_type, "0220", 4)))
		  {
			 /* It is an adjustment, msg type 0220 or a void, msg type 0200 */
             len= strlen(Auth_Tx.TLF01_details.orig_amount);
			 sprintf(temp_len, "%04d", len);

			 genutil_asc_to_vbcd (temp_len, 
			 				 	  sizeof(HOST_MESSAGE.priv60_len)*2,
								  HOST_MESSAGE.priv60_len);
		 	 memcpy (HOST_MESSAGE.priv60, Auth_Tx.TLF01_details.orig_amount,len);
		  }

		  else if(0 == strncmp( Auth_Tx.TLF01_details.message_type, "0320", 4))
		  {
			  /* It is a batch upload, msg type 0320. The first 4 bytes are the original message id,
			  * next 6 are the original stan.*/
			 memcpy( HOST_MESSAGE.priv60, Auth_Tx.TLF01_details.orig_message,4);		
			 memcpy(HOST_MESSAGE.priv60 + 4,Auth_Tx.TLF01_details.orig_sys_trace_num,6);
		  }

	      else if(0 == strncmp( Auth_Tx.TLF01_details.handler_queue,"sbatmpA",4))
	   	  {
	 		 len= strlen(Auth_Tx.TLF01_details.product_codes[16].amount);
	 	 	 sprintf(temp_len, "%04d", len);
	 	 	 genutil_asc_to_vbcd (temp_len,
  		 					 	  sizeof(HOST_MESSAGE.priv60_len)*2,
   		 						  HOST_MESSAGE.priv60_len);
	 	 	 memcpy ( HOST_MESSAGE.priv60, Auth_Tx.TLF01_details.product_codes[16].amount,len);

	   	  }

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
   BYTE temp_len[5] = {0};
   BYTE dummy_DE61_for_SM_e_card[03] = "01";

   if ( *result_flag == 0 )
   {
		if( 0 == (strncmp(Auth_Tx.TLF01_details.card_num,"7",1)))
		{
			if(strlen(Auth_Tx.TLF01_details.product_code)>0)
			{
				len= strlen(Auth_Tx.TLF01_details.product_code);
				sprintf(temp_len, "%04d", len);
				genutil_asc_to_vbcd (temp_len, sizeof(HOST_MESSAGE.priv61_len)*2,
									 HOST_MESSAGE.priv61_len);
				memcpy(HOST_MESSAGE.priv61 , Auth_Tx.TLF01_details.product_code,len);
			}
			else
			{
				/*Pos terminal did not send DE61 that is why control had come here.
				We have to send DE 61 as 01 so that Cadencie can recognize SM e-card 0220
				transaction and process the earmarking of transaction.*/
				len= strlen(dummy_DE61_for_SM_e_card);
				sprintf(temp_len, "%04d", len);
				genutil_asc_to_vbcd (temp_len,sizeof(HOST_MESSAGE.priv61_len)*2,
									 HOST_MESSAGE.priv61_len);
				memcpy(HOST_MESSAGE.priv61 , dummy_DE61_for_SM_e_card,len);
			}
		}
		else if (strlen(Auth_Tx.future_use_3) > 0)
		{
			 // Only for DE48 transaction contaning PSTN
			len= strlen(Auth_Tx.TLF01_details.product_code);
			sprintf(temp_len, "%04d", len);
			genutil_asc_to_vbcd (temp_len,sizeof(HOST_MESSAGE.priv61_len)*2,
								 HOST_MESSAGE.priv61_len);
			memcpy(HOST_MESSAGE.priv61 , Auth_Tx.TLF01_details.product_code,len);
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
INT build_request_field_62 ( pINT result_flag, pCHAR err_buf )
{

   INT  ret_val = true;
   INT  len = 0;
   BYTE temp_len[5] = {0};
   INT  len_PI_PT = 37; // 6 byte Invoice number, 6 byte purchase type and 25 byte purchase ID

   if ( *result_flag == 0 )
   {
   		if(strlen(Auth_Tx.TLF01_details.invoice_number)>0)
   		{
		  /* it is message type 0100 or 0200 or 0220 or 0320 */
	      len= strlen(Auth_Tx.TLF01_details.invoice_number);
		  sprintf(temp_len, "%04d", len);
		  genutil_asc_to_vbcd (temp_len, 
		 				 	   sizeof(HOST_MESSAGE.priv62_len)*2,
							   HOST_MESSAGE.priv62_len);

		  memcpy(HOST_MESSAGE.priv62,Auth_Tx.TLF01_details.invoice_number, len);
   		}
   		if(strlen(Auth_Tx.TLF01_details.product_codes[17].code) > 0)
		{
			// Then we have received Purchase ID type and Purchase ID
   			memcpy( HOST_MESSAGE.priv62 +INVOICE_NUMBER_LEN,
   					Auth_Tx.TLF01_details.product_codes[17].code,
					PURCHASE_ID_TYPE_LEN);
   			memcpy( HOST_MESSAGE.priv62 + PURCHASE_ID_DATA_POS,
   					Auth_Tx.TLF01_details.product_codes[17].amount,
					12);
   			memcpy( HOST_MESSAGE.priv62 + PURCHASE_ID_LEFTOVER_DATA_POS_1,
   					Auth_Tx.TLF01_details.product_codes[18].amount,
					12);
   			memcpy( HOST_MESSAGE.priv62 + PURCHASE_ID_LEFTOVER_DATA_POS_2,
					Auth_Tx.TLF01_details.product_codes[18].code,
					1);

   			memset(HOST_MESSAGE.priv62_len,0x00,sizeof(HOST_MESSAGE.priv62_len));
   			memset(temp_len,0x00,sizeof(temp_len));

   			sprintf(temp_len, "%04d", len_PI_PT);
   			genutil_asc_to_vbcd (temp_len,
							   	 sizeof(HOST_MESSAGE.priv62_len)*2,
							     HOST_MESSAGE.priv62_len);
		}
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_63
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
INT build_request_field_63 ( pINT result_flag, pCHAR err_buf )
{

   INT  ret_val = true;
   INT  len = 0;
   BYTE temp_len[4] = {0};
   CHAR INT_ZERO[8] = "0000000";
   CHAR factor_rate[8] = {0};
   CHAR strtand_id_len[3] = {0};
   CHAR strfiled_63_len[5] = {0};
   CHAR strfield_63_data_len[5] = {0};
   CHAR strfield_63_data_len_bcd[3] = {0};
   CHAR tem_filed_63_raw_data[355] = {0};
   INT Factor = 0;
   INT table_id_len = 2;
   INT table_id_len_value = 12;
   INT move_len_TID12 = 0;
   INT field_63_data_len = 0;
   INT filed_63_len = 0;
   
   if ( *result_flag == 0 )
	{
		if(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"sbat",4) )
		{
			if(0 == strncmp( Auth_Tx.TLF01_details.message_type, "0500", 4))
			{
				if(strlen(Auth_Tx.TLF01_details.num_sales)>0)
				{
					memcpy(HOST_MESSAGE.priv63,      Auth_Tx.TLF01_details.num_sales,	   	3);
					memcpy(HOST_MESSAGE.priv63 +  3, Auth_Tx.TLF01_details.sales_amount,    12);
					memcpy(HOST_MESSAGE.priv63 + 15, Auth_Tx.TLF01_details.num_refunds,	    3);
					memcpy(HOST_MESSAGE.priv63 + 18, Auth_Tx.TLF01_details.refund_amount,   12);
					memcpy(HOST_MESSAGE.priv63 + 30, Auth_Tx.TLF01_details.num_dbsales,	    3);
					memcpy(HOST_MESSAGE.priv63 + 33, Auth_Tx.TLF01_details.dbsales_amount,  12);
					memcpy(HOST_MESSAGE.priv63 + 45, Auth_Tx.TLF01_details.num_dbrefunds,   3);
					memcpy(HOST_MESSAGE.priv63 + 48, Auth_Tx.TLF01_details.dbrefund_amount, 12);

					len= strlen(HOST_MESSAGE.priv63);
					sprintf(temp_len, "%04d", len);
					genutil_asc_to_vbcd (temp_len, 
										 sizeof(HOST_MESSAGE.priv63_len)*2,
										 HOST_MESSAGE.priv63_len);
				}
			}
			else if(0 == strncmp( Auth_Tx.TLF01_details.message_type, "0320", 4))
			{
				if(0 == strncmp( Auth_Tx.TLF01_details.card_num, "7", 1))
				{
					// Get the length of 
					// Field 63 format
					// 2 byte lenth of entire data, 2 byte length of filed 63 data len, 2 byte TID len and  n byte data
					move_len_TID12 = strlen(Auth_Tx.TLF01_details.ticket_nbr);
				
					field_63_data_len=move_len_TID12 + table_id_len; // Filed 63 data and 2 byte data len
				
					filed_63_len = field_63_data_len +2 ; // Field 63 raw data len
					sprintf(strfiled_63_len,"%04d",filed_63_len);
								
					sprintf(strfield_63_data_len, "%04d",field_63_data_len);
					genutil_asc_to_vbcd (strfield_63_data_len, 
										 sizeof(strfield_63_data_len_bcd)*2,
										 strfield_63_data_len_bcd);
					memcpy(tem_filed_63_raw_data,strfield_63_data_len_bcd,2);
				
					sprintf(strtand_id_len, "%02d",table_id_len_value);
					memcpy(tem_filed_63_raw_data + 2,strtand_id_len,2);
					memcpy(tem_filed_63_raw_data + 4,Auth_Tx.TLF01_details.ticket_nbr,move_len_TID12);
					genutil_asc_to_vbcd (strfiled_63_len, 
										 sizeof(HOST_MESSAGE.priv63_len)*2,
										 HOST_MESSAGE.priv63_len);
					/* Total length to be copy is 2 byte data length + 2 byte table len + total ticket_nbr len*/
					memcpy(HOST_MESSAGE.priv63 ,tem_filed_63_raw_data , move_len_TID12 +4); 
				}
				else
				{
					//term
					memcpy(HOST_MESSAGE.priv63,		Auth_Tx.TLF01_details.deferred_term_length, 2);
					//Factore value
					Factor=atoi(Auth_Tx.TLF01_details.deferred_factor);
					sprintf(factor_rate,"%07d",Factor);
					if(strlen(Auth_Tx.TLF01_details.deferred_factor)>0)
					{
						memcpy(HOST_MESSAGE.priv63 + 2,factor_rate, 		7);
					}
					else
					{
						memcpy(HOST_MESSAGE.priv63 + 2, INT_ZERO, 		7);
					}
					//Monthly Amount
					memcpy(HOST_MESSAGE.priv63 + 9, Auth_Tx.TLF01_details.monthly_amort_amt,    12);
					//Groos Amount
					memcpy(HOST_MESSAGE.priv63 + 21,Auth_Tx.TLF01_details.def_gross_amt, 		12);
					len= strlen(HOST_MESSAGE.priv63);
					if(len > 0)
					{
						sprintf(temp_len, "%04d", len);
						genutil_asc_to_vbcd (temp_len, sizeof(HOST_MESSAGE.priv63_len)*2,
										HOST_MESSAGE.priv63_len);
					}
				}
			}
			else if(0 == strncmp( Auth_Tx.TLF01_details.card_num, "7", 1))
			{
				// Get the length of 
				// Field 63 format
				// 2 byte lenth of entire data, 2 byte length of filed 63 data len, 2 byte TID len and  n byte data
				move_len_TID12 = strlen(Auth_Tx.TLF01_details.ticket_nbr);
				
				field_63_data_len=move_len_TID12 + table_id_len; // Filed 63 data and 2 byte data len
				
				filed_63_len = field_63_data_len +2 ; // Field 63 raw data len
				sprintf(strfiled_63_len,"%04d",filed_63_len);
								
				sprintf(strfield_63_data_len, "%04d",field_63_data_len);
				genutil_asc_to_vbcd (strfield_63_data_len, 
										 sizeof(strfield_63_data_len_bcd)*2,
										 strfield_63_data_len_bcd);
				memcpy(tem_filed_63_raw_data,strfield_63_data_len_bcd,2);
				
				sprintf(strtand_id_len, "%02d",table_id_len_value);
				memcpy(tem_filed_63_raw_data + 2,strtand_id_len,2);
				memcpy(tem_filed_63_raw_data + 4,Auth_Tx.TLF01_details.ticket_nbr,move_len_TID12);
				genutil_asc_to_vbcd (strfiled_63_len, 
										 sizeof(HOST_MESSAGE.priv63_len)*2,
										 HOST_MESSAGE.priv63_len);
				/* Total length to be copy is 2 byte data length + 2 byte table len + total ticket_nbr len*/
				memcpy(HOST_MESSAGE.priv63 ,tem_filed_63_raw_data , move_len_TID12 +4); 
			}
			else
			{
				//term
				memcpy(HOST_MESSAGE.priv63,		Auth_Tx.TLF01_details.deferred_term_length, 2);
				//Factore value
				Factor=atoi(Auth_Tx.TLF01_details.deferred_factor);
				sprintf(factor_rate,"%07d",Factor);
				if(strlen(Auth_Tx.TLF01_details.deferred_factor)>0)
				{
					memcpy(HOST_MESSAGE.priv63 + 2,factor_rate, 		7);
				}
				else
				{
					memcpy(HOST_MESSAGE.priv63 + 2, INT_ZERO, 		7);
				}
				//Monthly Amount
				memcpy(HOST_MESSAGE.priv63 + 9, Auth_Tx.TLF01_details.monthly_amort_amt,    12);
				//Groos Amount
				memcpy(HOST_MESSAGE.priv63 + 21,Auth_Tx.TLF01_details.def_gross_amt, 		12);
				len= strlen(HOST_MESSAGE.priv63);
				if(len > 0)
				{
					sprintf(temp_len, "%04d", len);
					genutil_asc_to_vbcd (temp_len, sizeof(HOST_MESSAGE.priv63_len)*2,
									HOST_MESSAGE.priv63_len);
				}
			}
		}
				
   	   else if(0 == strncmp( Auth_Tx.TLF01_details.message_type, "0500", 4))
   	   {
			 if(strlen(Auth_Tx.TLF01_details.num_sales)>0)
			 {
				memcpy(HOST_MESSAGE.priv63,      Auth_Tx.TLF01_details.num_sales,	   	3);
				memcpy(HOST_MESSAGE.priv63 +  3, Auth_Tx.TLF01_details.sales_amount,    12);
				memcpy(HOST_MESSAGE.priv63 + 15, Auth_Tx.TLF01_details.num_refunds,	    3);
				memcpy(HOST_MESSAGE.priv63 + 18, Auth_Tx.TLF01_details.refund_amount,   12);
				memcpy(HOST_MESSAGE.priv63 + 30, Auth_Tx.TLF01_details.num_dbsales,	    3);
				memcpy(HOST_MESSAGE.priv63 + 33, Auth_Tx.TLF01_details.dbsales_amount,  12);
				memcpy(HOST_MESSAGE.priv63 + 45, Auth_Tx.TLF01_details.num_dbrefunds,   3);
				memcpy(HOST_MESSAGE.priv63 + 48, Auth_Tx.TLF01_details.dbrefund_amount, 12);

				len= strlen(HOST_MESSAGE.priv63);
				sprintf(temp_len, "%04d", len);

				genutil_asc_to_vbcd (temp_len,
									 sizeof(HOST_MESSAGE.priv63_len)*2,
									 HOST_MESSAGE.priv63_len);
			 }
   		}
   	   else if (strlen(Auth_Tx.future_use_3) > 0)
	   {
   		   // Only for DE48 transaction contaning PSTN
   		   memcpy(HOST_MESSAGE.priv63,		Auth_Tx.TLF01_details.deferred_term_length, 2);//term
   		   len= strlen(HOST_MESSAGE.priv63);
   		   if(len > 0)
		   {
				sprintf(temp_len, "%04d", len);
				genutil_asc_to_vbcd (temp_len, sizeof(HOST_MESSAGE.priv63_len)*2,
								HOST_MESSAGE.priv63_len);//DE 63 Length
		  }
	   }
		
	 }

   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_64
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
INT build_request_field_64 ( pINT result_flag, pCHAR err_buf )
{

   INT  ret_val = true;
   INT  len = 0 ;
   BYTE temp_len[3] = {0};

   if ( *result_flag == 0 )
   	{}

   //Rj_with_lead_zeros();
   return( ret_val );
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
         memset( HOST_MESSAGE.pan, '0', MAX_DE2_SIZE );
         memcpy( HOST_MESSAGE.pan, Auth_Tx.TLF01_details.card_num, len );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_3
 *
 *  DESCRIPTION:  This function will move the processing code to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Processing code is numeric, fixed length.
 *                This is an echo from the request. Therefore, there is
 *                no reason to validate the data here.
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
INT build_response_field_3( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.proccode,
              Auth_Tx.TLF01_details.processing_code,
              DE3_SIZE );
   }
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         build_response_field_11
 *
 *  DESCRIPTION:  This function will move the system trace audit number
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
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
INT build_response_field_11( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.stan,
              Auth_Tx.TLF01_details.sys_trace_audit_num,
              DE11_SIZE );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_response_field_12
 *
 *  DESCRIPTION:  This function will move the date and time
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
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
INT build_response_field_12( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

  if ( *result_flag == 0 )
   {
	   memcpy( HOST_MESSAGE.date,
               &Auth_Tx.TLF01_details.date_yyyymmdd[2],
              4);
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_14
 *
 *  DESCRIPTION:  This function will move the expire date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *            
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
 *  AUTHOR:       Girija Y - ThoughtFocus 
 *
 ******************************************************************************/
INT build_response_field_14( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

  if ( *result_flag == 0 )
   {
	  if( Auth_Tx.TLF01_details.exp_date[0]  != 0x00 )
	  {
	   memcpy( HOST_MESSAGE.date_expd,
               &Auth_Tx.TLF01_details.exp_date,sizeof(HOST_MESSAGE.date_expd));
	  }

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_25
 *
 *  DESCRIPTION:  This function will move the reason code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *            
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
 *  AUTHOR:       Girija Y - ThoughtFocus 
 *
 ******************************************************************************/
INT build_response_field_25( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

  if ( *result_flag == 0 )
  {
//	 if( Auth_Tx.TLF01_details.product_codes[13].code[0] !=0x00 )
	 if( Auth_Tx.TLF01_details.nii[0] != 0x00 )
	  {
		memcpy( HOST_MESSAGE.pos_condcd,Auth_Tx.TLF01_details.nii,
			    sizeof(HOST_MESSAGE.pos_condcd));
	  }
  }

   return( ret_val );
}






/******************************************************************************
 *
 *  NAME:         build_response_field_39
 *
 *  DESCRIPTION:  This function will move the response code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
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
INT build_response_field_39( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
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
   INT   len;
   BYTE tem_len[3]={0};
   CHAR  tempstr [101] = {0} ;
   INT	 tempint = 0;

   if ( *result_flag == 0 )
   {
      /* First get the length of the card number. */
      /*memcpy( Auth_Tx.TLF01_details.card_num_len,
              HOST_MESSAGE.pan_len, 
              DE2_LEN_SIZE );
	  
	  memcpy( tem_len,Auth_Tx.TLF01_details.card_num_len,
	  		  DE2_LEN_SIZE);*/
  	  genutil_bcd_to_asc (HOST_MESSAGE.pan_len, sizeof(HOST_MESSAGE.pan_len),  
							  &tempstr[0], sizeof(tempstr));
	  memcpy (Auth_Tx.TLF01_details.card_num_len, tempstr, 
	  	      sizeof(Auth_Tx.TLF01_details.card_num_len)-1);
	
	  genutil_bcd_to_asc (HOST_MESSAGE.pan, (atoi(Auth_Tx.TLF01_details.card_num_len)+1)/2,  
				  		  &tempstr[0], sizeof(tempstr));

      tempint= sizeof(Auth_Tx.TLF01_details.card_num) - 1;
	  if(tempint > atoi(Auth_Tx.TLF01_details.card_num_len))
  	  {
			tempint = atoi(Auth_Tx.TLF01_details.card_num_len);
  	  }
 	  memcpy(Auth_Tx.TLF01_details.card_num, tempstr, tempint);
  

      if ( tempint <= MAX_DE2_SIZE )
      {
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
   CHAR  tempstr [101] ={0} ;

   if ( *result_flag == 0 )
   {
   		
			/*memcpy( Auth_Tx.TLF01_details.processing_code,
       	 		  	HOST_MESSAGE.proccode, 
       	  	  		DE3_SIZE );*/
       	  	  		
			genutil_bcd_to_asc (HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode),
								&tempstr[0], sizeof (tempstr) );
			memcpy (Auth_Tx.TLF01_details.processing_code, tempstr, 
					sizeof(Auth_Tx.TLF01_details.processing_code));
	  	
			 if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.processing_code)))
			 {
	   			*result_flag = 1;
	   			sprintf( err_buf,
	           			 "Non-numeric processing code (field 3): %s,",
	           			 Auth_Tx.TLF01_details.processing_code );
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
   CHAR  tempstr [101] = {0} ;

   if ( *result_flag == 0 )
   {

	    /*memcpy( Auth_Tx.TLF01_details.total_amount,
	        HOST_MESSAGE.amount, 
    	   	 DE4_SIZE );*/
		genutil_bcd_to_asc (HOST_MESSAGE.amount, sizeof(HOST_MESSAGE.amount), 
	   					&tempstr[0], sizeof(tempstr));

		memcpy(Auth_Tx.TLF01_details.total_amount, tempstr, 
			   sizeof(Auth_Tx.TLF01_details.total_amount)-1);
	  
  		if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.total_amount)))
	  	{
 			*result_flag = 1;
 		  	sprintf( err_buf,
         	         "Non-numeric amount (field 4): %s,",
                      Auth_Tx.TLF01_details.total_amount );
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
   CHAR  tempstr [101] ={0} ;

   if ( *result_flag == 0 )
   {
   		 genutil_bcd_to_asc (HOST_MESSAGE.stan, sizeof(HOST_MESSAGE.stan),  
                       		 &tempstr[0],sizeof(tempstr));
		 memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
           	    tempstr,DE11_SIZE );
		ret_val = isnum(Auth_Tx.TLF01_details.sys_trace_audit_num);
      	if ( ret_val == false )
      	{
        	*result_flag = 1;
          	sprintf( err_buf,
           			 "Non-numeric System Trace Audit Number (field 11): %s,",
           			 Auth_Tx.TLF01_details.sys_trace_audit_num );
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
   BYTE  time[7] = {0};
   CHAR  tempstr [101] = {0} ;

   if ( *result_flag == 0 )
   {

		  /* Time */
		  genutil_bcd_to_asc (HOST_MESSAGE.time, sizeof(HOST_MESSAGE.time),
				  &tempstr[0], sizeof(tempstr));
		  memcpy(Auth_Tx.TLF01_details.time_hhmmss,tempstr, HHMMSS_LEN);
		  memcpy(time,Auth_Tx.TLF01_details.time_hhmmss,HHMMSS_LEN);

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
   BYTE  date[5] = {0};
   INT   len     = 0;
   BYTE time_date[25]={0};
   CHAR  tempstr [101] = {0};

   if ( *result_flag == 0 )
   {
   	 			ptetime_get_timestamp( time_date );
			/*Store system date year YYYY */
			memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd, 
	    	      	  		time_date, YYYY_LEN);
   			/* Date  MMDD Format*/
			genutil_bcd_to_asc	(HOST_MESSAGE.date,sizeof HOST_MESSAGE.date,
								 &tempstr[0], sizeof tempstr) ;
			memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd[MMDD_START_POS], 
	    	      	 tempstr, MMDD_LEN);
	  
	 		memcpy( date,Auth_Tx.TLF01_details.date_yyyymmdd + MMDD_START_POS,MMDD_LEN);
			  
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
   BYTE tempstr[101]={0};

   if ( *result_flag == 0 )
   {
	
	  		genutil_bcd_to_asc (HOST_MESSAGE.date_expd, sizeof(HOST_MESSAGE.date_expd),
				  				&tempstr[0], sizeof(tempstr));
			memcpy(Auth_Tx.TLF01_details.exp_date,
				   HOST_MESSAGE.date_expd,DE15_SIZE);
			
		   ret_val = isnum(Auth_Tx.TLF01_details.exp_date);
		   if ( ret_val == false )
      	   {
		        *result_flag = 1;
        		 sprintf( err_buf,
                		 "Non-numeric Expire date (field 14): %s,",
                  		  Auth_Tx.TLF01_details.exp_date );
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
   CHAR tempstr[101]={0};

   if ( *result_flag == 0 )
   {

			genutil_bcd_to_asc (HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry),  
			   				&tempstr[0], sizeof(tempstr));

			memcpy( Auth_Tx.TLF01_details.pos_entry_mode,tempstr,DE22_SIZE );

			ret_val = isnum(Auth_Tx.TLF01_details.pos_entry_mode);
			if ( ret_val == false )
			{
			 	*result_flag = 1;
			  	sprintf( err_buf,
				 		 "Non-numeric POS Entry Mode (field 22): %s,",
				  		 Auth_Tx.TLF01_details.pos_entry_mode );
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
   BYTE  tempstr[101]={0};

   if ( *result_flag == 0 )
   	{

			  genutil_bcd_to_asc (HOST_MESSAGE.card_seq_num, sizeof(HOST_MESSAGE.card_seq_num), 
						  		  &tempstr[0], sizeof(tempstr));
		   	  memcpy(Auth_Tx.EMV_details.pan_sequence_number, 
			  		 tempstr, sizeof(Auth_Tx.EMV_details.pan_sequence_number)-1);
		}
		else if ( Auth_Tx.host2_message_seq_nbr[0] != 0x00 )
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  genutil_bcd_to_asc (HOST_MESSAGE.card_seq_num, sizeof(HOST_MESSAGE.card_seq_num), 
						  		  &tempstr[0], sizeof(tempstr));
		   	  memcpy(Auth_Tx.host2_message_seq_nbr, tempstr, sizeof(Auth_Tx.TLF01_details.nii)-1);
		}
   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_24
 *
 DESCRIPTION:  This function will move the  Network International identifier
 *                to the Host Message structure according to BDO Cadencie host
 *                formatting specifications.  It is numeric, fixed length.
 *
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

INT move_field_24( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0 ;
   BYTE  tempstr[101]={0};

   if ( *result_flag == 0 )
   	{
  
		   genutil_bcd_to_asc (HOST_MESSAGE.nii, sizeof(HOST_MESSAGE.nii), 
						  		&tempstr[0], sizeof(tempstr));
		   memcpy(Auth_Tx.TLF01_details.nii, tempstr, sizeof(Auth_Tx.TLF01_details.nii)-1);
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
   BYTE  tempstr[101]={0};

   if ( *result_flag == 0 )
   {
   	 		genutil_bcd_to_asc (HOST_MESSAGE.pos_condcd, sizeof(HOST_MESSAGE.pos_condcd),  
			  					&tempstr[0], sizeof(tempstr));
			memcpy( Auth_Tx.TLF01_details.pos_condition_code,
				    tempstr, DE25_SIZE);

		    ret_val = isnum(Auth_Tx.TLF01_details.pos_condition_code);
			if ( ret_val == false )
      		{
			   *result_flag = 1;
			    sprintf( err_buf,
            		     "Non-numeric POS Condition Code (field 25): %s,",
			              Auth_Tx.TLF01_details.pos_condition_code );
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
   INT   len;
   CHAR  len_str[3]={0};
   BYTE templen[6]={0};
   BYTE tempstr[101]={0};

   if ( *result_flag == 0 )
   {
   	
	     /* First get the length. */
		 genutil_bcd_to_asc (HOST_MESSAGE.track2_len, sizeof(HOST_MESSAGE.track2_len),  
				  			  &templen[0], sizeof(templen));

		 genutil_bcd_to_asc (HOST_MESSAGE.track2, (atoi(templen)+1)/2,&tempstr[0], sizeof(tempstr));   
		 
    	 memcpy( len_str, templen, DE35_LEN_SIZE);
	     len = atoi( len_str );
	     if ( len <= MAX_DE35_SIZE )
	     {
	         /* Now copy the length Id into Auth_Tx and check it. */
	         memcpy( Auth_Tx.TLF01_details.track2, templen, DE35_LEN_SIZE);
 	         memcpy( Auth_Tx.TLF01_details.track2 + DE35_LEN_SIZE, tempstr, len);
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
			 	   HOST_MESSAGE.rrn, DE37_SIZE );
			  
		  ret_val = isalphanum(Auth_Tx.TLF01_details.retrieval_ref_num); 
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

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.authcode)> 0 )
      {
	      	memcpy( Auth_Tx.TLF01_details.auth_number,
	        		HOST_MESSAGE.authcode, DE38_SIZE );
			
	  		ret_val = isalphanum(Auth_Tx.TLF01_details.auth_number);
	  		if ( ret_val == false )
	  		{
	     		*result_flag = 1;
	     		sprintf( err_buf,
	            		 "Non-alphanumeric Auth Number (field 38) : %s,",
	              		 Auth_Tx.TLF01_details.auth_number );
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

   if ( *result_flag == 0 )
   {
      if(strlen(HOST_MESSAGE.responsecd)> 0)
      {
   	      memcpy( Auth_Tx.TLF01_details.response_code, HOST_MESSAGE.responsecd, DE39_SIZE );
     	  ret_val = isalphanum(Auth_Tx.TLF01_details.response_code);

      	  if ( ret_val == false )
      	  {
		       *result_flag = 1;
		        sprintf( err_buf,
		                "Non-alphanumeric response code (field 39) : %s,",
		                 Auth_Tx.TLF01_details.response_code);
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
   		    memcpy( Auth_Tx.TLF01_details.terminal_id,
              		HOST_MESSAGE.termid, DE41_SIZE );
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
             		HOST_MESSAGE.cardaccid, DE42_SIZE );
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
      		memcpy( Auth_Tx.TLF01_details.card_holder_name,
			 		 HOST_MESSAGE.cardaccname,
			 		 MER_NAME_LEN_FEILF_43 );// Max 25 Byte	
		 
		 	memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk,
				    HOST_MESSAGE.cardaccname+25,
			 		CITY_LEN_FEILD_43 );// Max 12 Byte	
		 
         	memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk+14,
					HOST_MESSAGE.cardaccname+37,
		 	     	COUNTY_CODE_LEN_FEILD_43 );// Max 3 Byte
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
   BYTE temp_len[3]={0};


   if ( *result_flag == 0 )
   {  
		if(strlen(HOST_MESSAGE.track1) > 0)
		{
	    	/* 1st find the length */
			memcpy(temp_len,HOST_MESSAGE.track1_len,DE45_LEN_SIZE);
			len = atoi (temp_len);
			
			if(len <= MAX_DE45_SIZE) // validate data length
			{
				/* Copy length into  Auth_Tx.acct_id_1 */
				memcpy(Auth_Tx.TLF01_details.track1,HOST_MESSAGE.track1_len,DE44_LEN_SIZE);

				/* Store additional response data into host message */
				memcpy(Auth_Tx.TLF01_details.track1+ DE45_LEN_SIZE ,HOST_MESSAGE.track1,len);
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
   BYTE  temp_len[4]={0};

   if ( *result_flag == 0 )
   {

		if(strlen(HOST_MESSAGE.addl_data_len)> 0)
		{
	    	/*Based on value set in nciftr/dcpiso we have to update this function*/	
			
			/* 1st find the length */
			memcpy(temp_len,HOST_MESSAGE.addl_data_len,DE48_LEN_SIZE);
			len = atoi(temp_len);
			
			if( len <= MAX_DE48_SIZE) // validate data length
			{
				/* Store length */
				memcpy(Auth_Tx.TLF01_details.voice_auth_text,HOST_MESSAGE.addl_data_len,DE48_LEN_SIZE);
				
				/* Store additional response data into host message */
				memcpy(Auth_Tx.TLF01_details.voice_auth_text + DE48_LEN_SIZE ,HOST_MESSAGE.addl_data,len);// Store data after length
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
				    DE52_SIZE);

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
   BYTE tempstr[101]={0};

   if ( *result_flag == 0 )
   {
  		if(strlen(HOST_MESSAGE.sec_cntl)> 0)
   		{
   		
			genutil_bcd_to_asc( HOST_MESSAGE.sec_cntl, sizeof(HOST_MESSAGE.sec_cntl),
								 &tempstr[0], sizeof tempstr );
	   		memcpy( Auth_Tx.mac_key,tempstr, DE53_SIZE);
			
		    ret_val = isnum(Auth_Tx.mac_key);
		    if ( ret_val == false )
		    {
		       *result_flag = 1;
		       sprintf( err_buf,
		               "Non-numeric Security Related Control Information (field 53) : %s,",
		                Auth_Tx.reconciliation_cur_code );
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
   BYTE  templen[101]={0};
   BYTE temp_amt[25]={0};

   if ( *result_flag == 0 )
   {
   		if(strlen(HOST_MESSAGE.addlamts)>0)
   		{
   			genutil_bcd_to_asc (HOST_MESSAGE.addl_len, sizeof(HOST_MESSAGE.addl_len),   
						 	    &templen[0], sizeof(templen));
			memcpy(temp_amt, HOST_MESSAGE.addlamts, atoi(templen));
			sprintf(Auth_Tx.TLF01_details.add_amounts,"%024d", atol(temp_amt));
   		}
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
 	INT temp_len =0;
	CHAR IccData[256]={0};
	INT  ret_val = true;
    if ( *result_flag == 0 )
    {
      	temp_len = genutil_bcd_to_int(HOST_MESSAGE.IccData_len, 2);		
		if ( temp_len > 0)
		{
		  memset(IccData, 0, sizeof(IccData));
		  memcpy(IccData, HOST_MESSAGE.IccData, temp_len);

		  /* Copy for future emv master card host  along with length*/
		  memcpy( Auth_Tx.EMV_details.emv_block,
		          HOST_MESSAGE.IccData_len,
		          temp_len + sizeof(HOST_MESSAGE.IccData_len) );

		  genutil_EMV_To_Auth_Tx( IccData, temp_len, &Auth_Tx );
		}
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
   	{}
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
   	{}
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         move_field_62
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
INT move_field_62( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[4] = {0};

   if ( *result_flag == 0 )
   	{}
   return( ret_val );
}








/******************************************************************************
 *
 *  NAME:         move_field_63
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
INT move_field_63( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   BYTE templen[6]={0};
   BYTE tempstr[101]={0};

   if ( *result_flag == 0 )
   	{
   			// Only Length in BCD data is in Ascii
   			genutil_bcd_to_asc (HOST_MESSAGE.priv63_len, sizeof(HOST_MESSAGE.priv63_len),  
								 &templen[0], sizeof(templen));
   			len = atoi( templen );
			/* Now copy the length Id into Auth_Tx and check it. */
			sprintf(Auth_Tx.future_use_1,"%04d",len);
			memcpy( Auth_Tx.future_use_1 + 4, HOST_MESSAGE.priv63, len);// We are storing data into ascii formate
			
   }
   return( ret_val );
}








/******************************************************************************
 *
 *  NAME:         move_field_64
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
INT move_field_64( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len     = 0;
   BYTE temp_len[4] ={0};

   if ( *result_flag == 0 )
   	{}
   return( ret_val );
}



