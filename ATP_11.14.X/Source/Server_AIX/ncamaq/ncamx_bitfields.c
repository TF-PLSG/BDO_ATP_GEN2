/***************************************************************************
*
* Copyright (c) 2004, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      NCAMX_BITFIELDS.C
*  
* TITLE:       Amex Network Control Data Field Processor
*  
* DESCRIPTION: This module contains functions
*              to build or parse Amex host data fields.
*
* APPLICATION: Advanced Transaction Processor (ATP)
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
#include "nc_dbstruct.h"
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"
#include "ncamx_bitfields.h"



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

extern const struct isobit BITTAB[128+1];
extern CHAR acq_amex_variable_auth_flag ;

/******************************************************************************
 *
 *  NAME:         build_request_field_2
 *
 *  DESCRIPTION:  This function will move the card number to the Host
 *                Message structure according to Equitable formatting
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
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_2( pINT result_flag, pCHAR err_buf )
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
                "Unable to build request message to host. No card number." );
      }
      else
      {
         if ( isalphanum(Auth_Tx.TLF01_details.card_num) )
         {
            sprintf( HOST_MESSAGE.pan_len, "%02d", len  );
            memset( HOST_MESSAGE.pan, '0', MAX_DE2_SIZE );
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
 *                Message structure according to Equitable formatting
 *                specifications.  Processing code is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf      - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_3( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE proc_code[7]="";

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.TLF01_details.processing_code) )
      {

		  if((Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION) 		 ||
			 (Auth_Tx.TLF01_details.tx_key == AUTH_SALE) 		  		 ||
			  strncmp(Auth_Tx.TLF01_details.processing_code,"11",2) == 0 ||
			 (Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH) )
		  { /* O00000 to 004000 for quasi cash transaction, requested by marvin. 13th July 2018*/
				memcpy( proc_code, "004000", 6 );
		  }
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		  {
			  if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6))
			  {
				  memcpy( proc_code, "014008", 6 );
			  }
			  else
			  {
				  memcpy( proc_code, "014000", 6 );
			  }
		  }
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE)
				memcpy( proc_code, "000000", 6 );
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_REFUND)
				memcpy( proc_code, "204000", 6 );
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_SALE_CASH)
				memcpy( proc_code, "014000", 6 );
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
			  memcpy( proc_code, Auth_Tx.TLF01_details.processing_code, 6 );
		  else if((Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL) || (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE))
		  {
			  if(strncmp(Auth_Tx.TLF01_details.processing_code,"00",2) == 0)
					memcpy( proc_code, "004000", 6 );
			  else if(strncmp(Auth_Tx.TLF01_details.processing_code,"01",2) == 0)
					memcpy( proc_code, "014000", 6 );
			  else if(strncmp(Auth_Tx.TLF01_details.processing_code,"02",2) == 0)/* Fix for DE3- TF Phani*/
					memcpy( proc_code, "024000", 6 );
			  else
					memcpy( proc_code, "004000", 6 );
		  }
#if BDOR_62_PREAUTH_VOID
		  else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
		  {
			  if(strncmp(Auth_Tx.TLF01_details.processing_code,"02",2) == 0)
					memcpy( proc_code, "004000", 6 );
		  }
#endif
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE)
			  		memcpy( proc_code, "004000", 6 );
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE)
			  		memcpy( proc_code, "014000", 6 );
		  else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND)
			  		memcpy( proc_code, "204000", 6 );
		  else 
			  	memcpy( proc_code, "000000", 6 );

	
		 
         memcpy( HOST_MESSAGE.proccode, proc_code, DE3_SIZE );
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
   memcpy(Auth_Tx.TLF01_details.product_codes[17].amount,proc_code,DE3_SIZE);


   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_4
 *
 *  DESCRIPTION:  This function will move the amount to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
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
INT build_request_field_4( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE amount[13] = "";

   if ( *result_flag == 0 )
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  || 
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION))
      {
         /*+-----------------------------------------------------------------+
           | For reversals, ATM and POS are treated differently for field 4. |
           |    POS - Field 4 is always the total_amount.                    |
           |    ATM - Field 4 is dispensed amount: zero for full reversals.  |
           |                                                                 |
           |    POS -> Origin = "LO"                                         |
           |    ATM -> Origin = "RO"                                         |
           +-----------------------------------------------------------------+*/

         if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin, "RO") &&
        		 Auth_Tx.TLF01_details.actual_amount[0] != 0x00 )
         {
            /* ATM - Amount dispensed for Partial or Full Reversal */
            strcpy( amount, Auth_Tx.TLF01_details.actual_amount );
         }
         else
         {
            /* POS - Amount being reversed. */ /* void preauth amount for VOID PRE AUTH */
            strcpy( amount, Auth_Tx.TLF01_details.total_amount );
         }

         memcpy( HOST_MESSAGE.amount, amount, DE4_SIZE );
      }
      else if ( isnum(Auth_Tx.TLF01_details.total_amount) )
      {
         if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
         {
            memcpy( HOST_MESSAGE.amount,
                    Auth_Tx.TLF01_details.total_amount,
                    DE4_SIZE );
         }
         else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
                               Auth_Tx.TLF01_details.currency_code) )
         {
            memcpy( HOST_MESSAGE.amount,
                    Auth_Tx.TLF01_details.total_amount,
                    DE4_SIZE );
         }
         else
         {
            memcpy(HOST_MESSAGE.amount,Auth_Tx.reconciliation_amount,DE4_SIZE);
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
 *  DESCRIPTION:  This function will move the reconciliation amount to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Reconciliation Amount is numeric, fixed length.
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
INT build_request_field_5( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.setl_amount,
                    Auth_Tx.reconciliation_amount,
                    DE5_SIZE );
      
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_7
 *
 *  DESCRIPTION:  This function will move the transmission date and time
 *                to the Host Message structure according to Equitable
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
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  date_str [ 9] = "";
   CHAR  time_str [ 7] = "";
   CHAR  time_date[25] = "";

   if ( *result_flag == 0 )
   {
      /* If this is a reversal, need to make sure we have the original
       * timestamp.  It should exist in 'orig_local_time_date', but
       * it might exist in 'transmission_timestamp'.
       *
       * If this is not a reversal, value may or may not exist.
       */
      if ( is_reversal() )
      {
         /* It is a reversal.  Original timestamp must exist. */
         if ( 0x00 == Auth_Tx.orig_local_time_date[0] )
         {
            if ( 0x00 == Auth_Tx.TLF01_details.transmission_timestamp[0] )
            {
               /* Original timestamp does not exist. Error! */
               ret_val = false;
               *result_flag = 1;
               sprintf( err_buf,
                     "Cannot build request msg (%s) for host. Missing a timestamp.",
                      Auth_Tx.TLF01_details.message_type );
            }
            else
            {
               /* Need to move timestamp to 'original' field. */
               memcpy( Auth_Tx.orig_local_time_date,
                       Auth_Tx.TLF01_details.transmission_timestamp,
                       DE7_SIZE );

               memset( Auth_Tx.TLF01_details.transmission_timestamp,
                       0x00,
                       sizeof(Auth_Tx.TLF01_details.transmission_timestamp));
            }
         }
      }

      /* Create timestamp if it does not exist. */
      if ( true == ret_val )
      {
         if ( 0x00 == Auth_Tx.TLF01_details.transmission_timestamp[0] )
         {
            /* Need to create transmission time stamp. Get local time. */
            ptetime_get_timestamp( time_date );
            get_date( time_date, date_str );
            get_time( time_date, time_str );

            /* Put it in MMDDhhmms format */
            memcpy( Auth_Tx.TLF01_details.transmission_timestamp, date_str+4,4);
            memcpy( Auth_Tx.TLF01_details.transmission_timestamp+4, time_str,6);

         }

         memcpy( HOST_MESSAGE.gmt_time,
                 Auth_Tx.TLF01_details.transmission_timestamp,
                 DE7_SIZE );
      }
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_11
 *
 *  DESCRIPTION:  This function will move the system trace audit number
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_11( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  validate_stan = true;

   if ( *result_flag == 0 )
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  || 
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin,"LO") )
         {
            /* For POS reversals create new STAN */
            create_stan( HOST_MESSAGE.stan );
            validate_stan = false;
         }
      }

      if ( validate_stan == true )
      {
         if ( isnum(Auth_Tx.TLF01_details.sys_trace_audit_num) )
         {
            if ( Auth_Tx.TLF01_details.sys_trace_audit_num[0] != 0x00)
               memcpy( HOST_MESSAGE.stan,
                       Auth_Tx.TLF01_details.sys_trace_audit_num,
                       DE11_SIZE );
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
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_12
 *
 *  DESCRIPTION:  This function will move the transaction time
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the local time at the card aceptors location.
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
INT build_request_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";
   CHAR  date_str[9];
   CHAR  time_str[7];

   if ( *result_flag == 0 )
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)  || 
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) )
      {
         /* FOR REVERSAL : create new date, time */

         /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
         ptetime_get_timestamp( time_date );

         /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
         get_date( time_date, date_str );
         get_time( time_date, time_str );

         /* Now format field 12: YYMMDDhhmmss. */
         memcpy( HOST_MESSAGE.datetime, date_str+2, 6 );
         memcpy( HOST_MESSAGE.datetime+6, time_str, 6 );
      }
      else
      {
         if ( (0x00 == Auth_Tx.TLF01_details.time_hhmmss[0] ) ||
              (0x00 == Auth_Tx.TLF01_details.date_yyyymmdd[0]) )
         {
            /* Need to create transaction time. Get local time. */
            ptetime_get_timestamp( time_date );
            get_time( time_date, Auth_Tx.TLF01_details.time_hhmmss );
            get_date( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
         }

         memcpy( HOST_MESSAGE.datetime,
                &Auth_Tx.TLF01_details.date_yyyymmdd[2],
                 6 );
         memcpy( &HOST_MESSAGE.datetime[6],
                 Auth_Tx.TLF01_details.time_hhmmss,
                 6 );
      }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_13
 *
 *  DESCRIPTION:  This function will move the effective date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the local MMDD at the card aceptors location.
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
INT build_request_field_13( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";
   CHAR err_buf1[100]  ="";



   if ( *result_flag == 0 )
   {
	if(Auth_Tx.effective_date[0] != 0x00)
	{
		 if (!validate_date_yymm(Auth_Tx.effective_date,err_buf1))
		 {
			 ret_val = false;
			 *result_flag = 1;
			 sprintf( err_buf,
				 "Cannot build request msg for host. Invalid effective date: %s",
					Auth_Tx.TLF01_details.exp_date);
		  }
		  else
		  {
			memcpy( HOST_MESSAGE.eff_date,
					 Auth_Tx.effective_date,
					 DE13_SIZE );
		  }
	}
   }

  
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_14
 *
 *  DESCRIPTION:  This function will move the expiry date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the local MMDD at the card aceptors location.
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
INT build_request_field_14( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";
   CHAR  err_buf1[100]  ="";

   if ( *result_flag == 0 )
   {
      if(Auth_Tx.TLF01_details.exp_date[0] != 0x00)
      {
         if (!validate_date_yymm(Auth_Tx.TLF01_details.exp_date,err_buf1) )
         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
               "Cannot build request msg for host. Invalid expiry date: %s. %s",
                Auth_Tx.TLF01_details.exp_date, err_buf1 );
        }
        else
        {
           memcpy( HOST_MESSAGE.date_expd,
                   Auth_Tx.TLF01_details.exp_date,
                   DE14_SIZE );
        }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_19
 *
 *  DESCRIPTION:  This function will move the Acquiring institution country code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_19( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	  if(Ncf01_I.country_code[0] != 0x00)
	  {
		if ( isnum(Ncf01_I.country_code) )
		{
			 memcpy(HOST_MESSAGE.ctry_code, Ncf01_I.country_code, DE19_SIZE );
		}
		else
		{
			ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
				   "Cannot build request msg for host. Invalid Acquiring institution country code: %s",
	                Auth_Tx.country_code);
		}
	  }
	  else
	  {
    		ret_val = false;
			*result_flag = 1;
			sprintf( err_buf,
				   "Cannot build request msg for host.Acquiring institution country code: missing ");
	  }
   }
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         build_request_field_22
 *
 *  DESCRIPTION:  This function will move the POS entry mode
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR	 temp_entry_mode[4] = {0};

   if ( *result_flag == 0 )
   {
      if ( 0x00 == Auth_Tx.TLF01_details.pos_entry_mode[0] )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf, "POS Entry Mode is missing. Decline txn." );
      }
      else if ( false == isalphanum(Auth_Tx.TLF01_details.pos_entry_mode) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid POS Entry Mode: %s",
                Auth_Tx.TLF01_details.pos_entry_mode );
      }

      else
      {
         /* -----------------------------------
          * Determine format of POS Entry Mode.
          * -----------------------------------
          *
          * Equitable sent the following mapping from pos entry to Amx Bit 22.
          *
          * Descriptions               Entry Mode from POS      Bit22 to AMEX
          * ----------------------     -------------------      -------------
          * swipe without 4dbc         02X                      260101254140
          * swipe with 4dbc            06X                      260101W54140
          * manual key in without 4dbc 01X                      660101654140
          * manual key in with 4dbc    07X                      660101S54140
          * RPS transactions           012 pos cond code of 08  160420064100
          * Voice Transactions         000                      160001S54130
          * ATM transactions           021                      210111210141
          * EMV Tx 				 	   051  			        500U01501001
          * EMV contat less            071                      500U0X501001
          * MAg Contact less	 
          */

         memset(HOST_MESSAGE.posentry,0x30,DE22_SIZE);

         /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */
         if(strlen(Auth_Tx.TLF01_details.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode);

         if(strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",5)== 0 ||
        		   strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit",4)== 0)
        {
        	 if(strncmp(temp_entry_mode,"05",2) == 0)
        	 {
        		 memcpy(HOST_MESSAGE.posentry,"51140151304C",DE22_SIZE);
        	 }
        	 else if(strncmp(temp_entry_mode,"80",2) == 0)
        	 {
        		 memcpy(HOST_MESSAGE.posentry,"51140191304C",DE22_SIZE);
        	 }
        	 else {
        		 memcpy(HOST_MESSAGE.posentry,"21140121304C",DE22_SIZE);
        	 }

          }
       else
       {
#if BDOR_68_REQ_1_AMEX	   /* MobilePOS-NFC Terminal for Tap2Phone Transactions */
		   if(true == ncamaq_check_if_trx_is_amex_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		   {
			   strncpy( HOST_MESSAGE.posentry,
						AMEX_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,
						DE22_SIZE );
		   }
		   else if(true == ncamaq_check_if_trx_is_amex_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		   {
			   strncpy( HOST_MESSAGE.posentry,
						AMEX_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,
						DE22_SIZE );
		   }
#endif
#if BDOR_68_REQ_3_AMEX /* Swiftpass BDO Pay Credit Transactions */
		   else if(true == ncamaq_check_if_trx_is_amex_Swiftpass_BDO_Pay_credit_trx_with_pin())
		   {
			   strncpy( HOST_MESSAGE.posentry,
						AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN,
						DE22_SIZE );
		   }
		   else if(true == ncamaq_check_if_trx_is_amex_Swiftpass_BDO_Pay_credit_trx_without_pin())
		   {
			   strncpy( HOST_MESSAGE.posentry,
						AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN,
						DE22_SIZE );
		   }
#endif
#if BDOR_68_REQ_5_AMEX /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

			else if(true == ncamaq_check_if_trx_is_amex_ecommerce_swiftpass_BDOPay_trx ())
			{
			   strncpy( HOST_MESSAGE.posentry,
					    AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						DE22_SIZE );
		   }
#endif
	   else if(strncmp(temp_entry_mode,"02",2) == 0 ||
		   strncmp(temp_entry_mode,"06",2) == 0 ||
		   strncmp(temp_entry_mode,"08",2) == 0 ||
		   strncmp(temp_entry_mode,"90",2) == 0
		  )
		{	/*Mag stripe*/

			if(temp_entry_mode[2] == '1')
			{
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_MAGSTRIP_POS_DATA_WITH_PIN,DE22_SIZE);
			}
			else
			{
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN,DE22_SIZE);
			}
		}

		else if( strncmp(temp_entry_mode,"05",2) == 0 )
		{
			/* EMV - Contact*/
			if(temp_entry_mode[2] == '1')
			{
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_CHIP_POS_DATA_WITH_PIN,DE22_SIZE);
			}
			else
			{
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_CHIP_POS_DATA_WITHOUT_PIN,DE22_SIZE);
			}

			if (true == ncamaq_is_Online_PIN_transction())
			{
				/* Set Bit 22 pos 8 to '1' and pos 9 to '3'
			 	 * if txn validates online PIN
			 	 */
				HOST_MESSAGE.posentry[7] = '1';
				HOST_MESSAGE.posentry[8] = '3';
			}
		}

		else if( strncmp(temp_entry_mode,"80",2) == 0 )
		{	/* POS 80X */
			if(temp_entry_mode[2] == '1')
			{
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_FALLBACK_POS_DATA_WITH_PIN,DE22_SIZE);
			}
			else
			{
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_FALLBACK_POS_DATA_WITHOUT_PIN,DE22_SIZE);
			}

			if (true == ncamaq_is_Online_PIN_transction())
			{
				/* Set Bit 22 pos 8 to '1' and pos 9 to '3'
			 	 * if txn validates online PIN
			 	 */
				HOST_MESSAGE.posentry[7] = '1';
				HOST_MESSAGE.posentry[8] = '3';
			}
		}

		else if( strncmp(temp_entry_mode,"01",2) == 0 )
		{	/*Manual - VOICE- ECOM  - Recurring - RPS*/
				/*VOICE*/
			if(Auth_Tx.TLF01_details.voice_auth[0] == 'Y')
			{
				memcpy(HOST_MESSAGE.posentry,"660430600130",DE22_SIZE);

				/* For position seven, need to know if 4DBC/4CSC was entered.
				* Security control information is coming in
				* auth_Tx.tlf01_details.interest_rate.
				*/
				if ( Auth_Tx.TLF01_details.interest_rate[0] == 0x00 )
				{
					/* It was not entered, so change 'S' to '6'. */
					HOST_MESSAGE.posentry[6] = '6';
				}
			}

			else if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2) == 0)
			{
				/* For E-commerce tx*/
				memcpy(HOST_MESSAGE.posentry,
					   AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA,DE22_SIZE);
				/*if(Auth_Tx.TLF01_details.interest_rate[0] != 0x00 )
				{
					memcpy(HOST_MESSAGE.posentry,"100SS0S00100",DE22_SIZE);
				}*/
			}
			else if(0 == strcmp(Auth_Tx.industry_code,"800") &&
					(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0)
					)
			{
				/*RPS*/
				 memcpy(HOST_MESSAGE.posentry,
						AMEX_RPS_ASIAPAY_RPS_POS_DATA,DE22_SIZE);
			}
			else if ( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2))
			{
				//BDO PAY
				memcpy(HOST_MESSAGE.posentry,
						AMEX_BDO_PAY_POS_DATA,DE22_SIZE);
			}
			else
			{
				/*Manual entry*/
				if(temp_entry_mode[2] == '1')
				{
					memcpy(HOST_MESSAGE.posentry,
						   AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN,DE22_SIZE);
				}
				else
				{
					memcpy(HOST_MESSAGE.posentry,
						   AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN,DE22_SIZE);
				}
			}
		}

		else if( strncmp(temp_entry_mode,"07",2) == 0)
		{
			 /*  07X with out POS, Special case */
			if(Auth_Tx.TLF01_details.tx_key == AUTH_REFUND)
			{
			   memcpy(HOST_MESSAGE.posentry,"660101654140",DE22_SIZE);
			}
			else if((strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0)
					&& (0 == strcmp(Auth_Tx.industry_code,"800")) ) 			{
			   memcpy(HOST_MESSAGE.posentry,"600090S04100",DE22_SIZE);
			}
			else
			{
				if(temp_entry_mode[2] == '1')
				{
					memcpy(HOST_MESSAGE.posentry,
						   AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,DE22_SIZE);
				}
				else
				{
					memcpy(HOST_MESSAGE.posentry,
						   AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN,DE22_SIZE);
				}

				if (true == ncamaq_is_Online_PIN_transction())
				{
					/* Set Bit 22 pos 8 to '1' and pos 9 to '3'
				 	 * if txn validates online PIN
				 	 */
					HOST_MESSAGE.posentry[7] = '1';
					HOST_MESSAGE.posentry[8] = '3';
				}
			}
		}
		else if(strncmp(temp_entry_mode,"91",2) == 0)
		{
			/*Special case for COntact less */
			memcpy(HOST_MESSAGE.posentry,
				   AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN,DE22_SIZE);
		}
		else
		{	 /* POS 00X */
			memcpy(HOST_MESSAGE.posentry,"010110054140",DE22_SIZE);
		}


      }	// End of Else case of POS DATA build

      } // End of POS entry building

   } // End of Function
   // Store POS entry mode 
   strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,HOST_MESSAGE.posentry,DE22_SIZE);
     if(strlen(Auth_Tx.amex_pos_entry_mode)== 0)
   	strncpy(Auth_Tx.amex_pos_entry_mode,HOST_MESSAGE.posentry,DE22_SIZE); 
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_24
 *
 *  DESCRIPTION:  This function will move the Function code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_24( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE   tx_key;

   if ( *result_flag == 0 )
   {
      /* CASH ADVANCE with origin as "RO" (from ATM ) treated as 1200 message. 
       * CASH ADVANCE with origin as "LO" (from POS ) treated as 1100 message.
       */
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE) && 
           (strncmp(Auth_Tx.TLF01_details.dispensation.origin,"RO",2) == 0))
      {
         tx_key = AUTH_SALE_CASH;
      }

      /*  BALANCE INQUIRY with origin as "RO" (from ATM ) treated as 1200 message.
		* BALANCE INQUIRY with origin as "LO" (from POS ) treated as 1100 message.
		*/
      else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY) &&
              (strncmp(Auth_Tx.TLF01_details.dispensation.origin,"RO",2) == 0))
      {
    	  tx_key = AUTH_SALE_CASH;
      }
      else
         tx_key = Auth_Tx.TLF01_details.tx_key;

      switch ( tx_key )
      {
         case AUTH_SALE_CASH: 
            memcpy( HOST_MESSAGE.func_cd, CASHSALE_FUNC_CD, DE24_SIZE );
            break;

         case AUTH_AUTHORIZATION: 
         case AUTH_SALE:
         case AUTH_CASH_ADVANCE: 
         case AUTH_ADVICE:
         case AUTH_REFUND:
#if AMEX03B_APRIL2024
        	 if(Auth_Tx.TLF01_details.saf[0] == ESTIMATED_AUTH)
        	 {
        		 memcpy( HOST_MESSAGE.func_cd, ESTIMATED_AUTHORIZATION, DE24_SIZE );
        	 }
        	 else if(Auth_Tx.TLF01_details.saf[0] == INCREMENTAL_AUTH)
			 {
        		 if(acq_amex_variable_auth_flag =='Y')
        		 {
        		 memcpy( HOST_MESSAGE.func_cd, INCREMENTAL_AUTHORIZATION, DE24_SIZE );
        		 }
        		 else
        		 {
        			 memcpy( HOST_MESSAGE.func_cd, AUTHORIZATION_FUNC_CD, DE24_SIZE );
        		 }
			 }/* add else below once this block is enabled*/
        	 else
#endif

        	 {
        		 memcpy( HOST_MESSAGE.func_cd, AUTHORIZATION_FUNC_CD, DE24_SIZE );
        	 }

            break;

         case AUTH_REVERSAL:
         case AUTH_REVERSAL_ADVICE:
#if AMEX03B_APRIL2024
        	 if(Auth_Tx.TLF01_details.saf[0] == PARTIAL_REVERSAL)
             {
            	 memcpy(HOST_MESSAGE.func_cd, PARTIALREVERSAL_FUNC_CD, DE24_SIZE);
             }
			 /*if AMEX03B_APRIL2024 enabled the make the below if as else if*/
        	 else
#endif
        	 if ( 0 == atol(Auth_Tx.TLF01_details.actual_amount) )
            {
               /* Full reversal */
               memcpy( HOST_MESSAGE.func_cd, REVERSAL_FUNC_CD, DE24_SIZE );
            }
            else
            {
               /* Partial reversal */
               memcpy(HOST_MESSAGE.func_cd, PARTIALREVERSAL_FUNC_CD, DE24_SIZE);
            }
            break;

#if BDOR_62_PREAUTH_VOID
         case AUTH_VOID_PRE_AUTHORIZATION:
             if ( Auth_Tx.TLF01_details.saf[0] == PRIMARY_AUTH )
             {
                /* Full reversal for PRIMARY_AUTH */
                memcpy( HOST_MESSAGE.func_cd, REVERSAL_FUNC_CD, DE24_SIZE );
             }
             else
             {
                /* Partial reversal  for INCREMENTAL_AUTH */
                memcpy(HOST_MESSAGE.func_cd, PARTIALREVERSAL_FUNC_CD, DE24_SIZE);
             }
             break;
#endif
         case AUTH_ATM_CONFIRM:
         case AUTH_VOID_CASH_ADVANCE:
         case AUTH_VOID_REFUND:
         case AUTH_VOID_SALE:
            memcpy( HOST_MESSAGE.func_cd, REVERSAL_FUNC_CD, DE24_SIZE );
            break;

         case AUTH_CARD_UPDATE:
            memcpy( HOST_MESSAGE.func_cd, "30", 2 );
            HOST_MESSAGE.func_cd[2] = Auth_Tx.TLF01_details.processing_code[0];
            break;

         default:
            ret_val = false;
            break;
      }
   }
   return( ret_val );
}

  

/******************************************************************************
 *
 *  NAME:         build_request_field_25
 *
 *  DESCRIPTION:  This function will move the Reason code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      /* Temporarily assigned 4021 reason code, which is Card Acceptor device
       * unable to complete transaction.
       * More formal method to assign reason code is required.
       */
      if( (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE ) ||
          (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND       ) ||
          (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE         )  )
      {
         memcpy( HOST_MESSAGE.pos_condcd,"4000",DE25_SIZE );
      }

      else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL) ||
                (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
				(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) )
      {
         /*+--------------------------------------------------------+
           | For reversals, ATM and POS are treated differently     |
           | for field 25:                                          |
           |                                                        |
           |    POS -> Origin = "LO"                                |
           |    ATM -> Origin = "RO"                                |
           |                                                        |
           | The following codes are used:                          |
           |                                                        |
           |    4006 - POS: Response received too late = timeout    |
           |    4007 - ATM: Partial dispense                        |
           |    4018 - ATM: No cash dispensed (full reversal)       |
           |    4021 - POS: Device unable to complete transaction.  |
           |                                                        |
           +--------------------------------------------------------+*/

         if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin, "RO") )
         {
            /* ATM */
            if ( 0 == atol(Auth_Tx.TLF01_details.actual_amount) )
               memcpy( HOST_MESSAGE.pos_condcd,"4018",DE25_SIZE );
            else
               memcpy( HOST_MESSAGE.pos_condcd,"4007",DE25_SIZE );
         }
         else
         {
            if ( 0 == strcmp(Auth_Tx.TLF01_details.response_code,AMEX_TIMEOUT) )
               memcpy( HOST_MESSAGE.pos_condcd,"4006",DE25_SIZE );
            else
               memcpy( HOST_MESSAGE.pos_condcd,"4021",DE25_SIZE );
         }
      }

      else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CARD_UPDATE )
      {
         /* Reason Code for Online Updates */
         memcpy( HOST_MESSAGE.pos_condcd, Auth_Tx.TLF01_details.nii, 4 );
      }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_26
 *
 *  DESCRIPTION:  This function will move the Card Acceptor business code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_26( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   /* No need to build DE26 when Primary Preauth is Voided*/

#if BDOR_62_PREAUTH_VOID
       if ((Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) &&
    	   (Auth_Tx.TLF01_details.saf[0] == PRIMARY_AUTH))
       {
	   return( ret_val );
       }
#endif

   else if ( *result_flag == 0 )
   {
       memcpy( HOST_MESSAGE.accpt_bus_cd,Auth_Tx.TLF01_details.category_code,DE26_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_30
 *
 *  DESCRIPTION:  This function will move the Amounts original
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
INT build_request_field_30( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   LONG	 est_amt = 0;
   LONG	 total_amt = 0;
   LONG	 trn_amt = 0;
   CHAR  estmatd_amount[13] = {0};

   /* No need to build DE30 when Primary Preauth is Voided*/

#if BDOR_62_PREAUTH_VOID
       if ((Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) &&
    	   (Auth_Tx.TLF01_details.saf[0] == PRIMARY_AUTH))
       {
    	   return( ret_val );
       }
#endif

#ifdef AMEX03B_APRIL2024
       else if ((0 == strncmp(Auth_Tx.TLF01_details.response_code,AMEX_TIMEOUT,2))&&
       			 (0 == strlen(Auth_Tx.TLF01_details.auth_number))&&
       			 (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ) &&
       			 (0 == strncmp(Auth_Tx.TLF01_details.message_type, "0420", 4)) &&
       			 (0 == strncmp(Auth_Tx.TLF01_details.acquirer_id,"ncposa",6)))
       	{
       		 return( ret_val );
       	}
#endif

	memset(HOST_MESSAGE.orig_amt,0x30,DE30_SIZE);

	if(Auth_Tx.TLF01_details.orig_amount[0] == 0x00)
	{
		/*ret_val = false;
		*result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid Original amount missing Field 30: %s",
                Auth_Tx.TLF01_details.orig_amount);*/
		memcpy(HOST_MESSAGE.orig_amt,Auth_Tx.TLF01_details.total_amount,12);
	}
	else
	{
		total_amt = atol(Auth_Tx.TLF01_details.total_interest);
		trn_amt = atol(Auth_Tx.TLF01_details.orig_amount);

		if (total_amt > trn_amt )
		{
			est_amt = total_amt - trn_amt;
		}
		else
		{
			est_amt = trn_amt;
		}

		sprintf(estmatd_amount, "%012d", est_amt);
		memcpy(HOST_MESSAGE.orig_amt, estmatd_amount, 12);


		/* According to spec no need to send Reconciliation amount, since AEGN is handling
		currency conversion */
	}
	strncat(HOST_MESSAGE.orig_amt,"000000000000",12); // Make sure last 12 should be '0'

   return( ret_val );
}
/******************************************************************************
*
*  NAME:         CREATE_TID
*
*  DESCRIPTION:  This procedure creates a retrieval reference number 
*                in the
*                format of YDDDhmmssjjj, that is:
*                     Y   = last digit of the year
*                     DDD = Julian day
*                     h   = last digit of the 24 hour clock (current time)
*                     mm  = current system minutes
*                     ss  = current system seconds
*                     jjj = current system milliseconds
					  ccc = counter from 1-999
*
*  INPUTS:       None
*
*  OUTPUTS:      TID - Formatted as mentioned above in the description
*                      TID should be a 15 character string.
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Phani
*
*  MODIFICATIONS:
******************************************************************************/

void create_tid(pCHAR tid)
{
	CHAR    time_date[25];
    CHAR    date_str[9];
    CHAR    time_str[7];
    CHAR    julian_day[4];

    /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
    ptetime_get_timestamp( time_date );

    /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
    get_date( time_date, date_str );
    get_time( time_date, time_str );

    /* Change date to julian format - DDD */
    calc_julian_day( date_str, julian_day );

    /* Format TID: "YDDDhmmssjjjCCC" */
    sprintf( tid, "%01.1s%03.3s%05.5s%03.3s%03.3d",time_date+3, julian_day,
                 time_str+1, time_date+20,TIDctr );
	  if ( TIDctr > 999 )
      TIDctr = 1;


}
/******************************************************************************
 *
 *  NAME:         build_request_field_31
 *
 *  DESCRIPTION:  This function will move the transaction ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
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
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
INT build_request_field_31( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE	 tid[40]={0};  INT  len = 0;
   CHAR temp_len[3]="";
	
   memset(HOST_MESSAGE.tranid_len,0x00,sizeof(HOST_MESSAGE.tranid_len));
   memset(HOST_MESSAGE.tranid,0x00,sizeof(HOST_MESSAGE.tranid));

   /* TF Phani*/
   if(strlen(Auth_Tx.TLF01_details.mcard_banknet) != 0)
   {
	 memcpy(Auth_Tx.TLF01_details.visa_tran_id,Auth_Tx.TLF01_details.mcard_banknet,sizeof(Auth_Tx.TLF01_details.visa_tran_id));
   }
   
   /*if(strlen(Auth_Tx.TLF01_details.visa_tran_id) == 0)
   {
	   create_tid(tid);
	   memcpy(Auth_Tx.TLF01_details.visa_tran_id,tid,sizeof(Auth_Tx.TLF01_details.visa_tran_id));
   }*/
      /* Get the length. */
   len = strlen(Auth_Tx.TLF01_details.visa_tran_id);   
   if(len > 0 && len < 40 )
   {	sprintf( temp_len, "%02d", len );
	   memcpy( HOST_MESSAGE.tranid_len, temp_len, 2 );
	   memcpy(HOST_MESSAGE.tranid,Auth_Tx.TLF01_details.visa_tran_id,len);
   }
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         build_request_field_32
 *
 *  DESCRIPTION:  This function will move the acquiring institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
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
INT build_request_field_32( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len = 0;
   CHAR temp_len[3]="";

   if ( *result_flag == 0 )
   {
      if ( Ncf01_I.acquirer_inst_id_code[0] == 0x00 )
      {
         /* An Acquirer ID is not configured.  Base it on origin. */
         len = 2;
         if(0== strcmp(REMOTE_ORIGIN,Auth_Tx.TLF01_details.dispensation.origin))
            memcpy( HOST_MESSAGE.acq_inst_id, INTL_ATM_POS, DE32_SIZE );
         else
            memcpy( HOST_MESSAGE.acq_inst_id, LOCAL_POS,    DE32_SIZE );
      }
      else
      {
         len = strlen( Ncf01_I.acquirer_inst_id_code );
         memcpy( HOST_MESSAGE.acq_inst_id, Ncf01_I.acquirer_inst_id_code, len );
      }

      /* Get the length. */
      sprintf( temp_len, "%02d", len );
      memcpy( HOST_MESSAGE.acq_inst_len, temp_len, 2 );

      /* Store value in TLF01 if there is not already a value. */
      if ( Auth_Tx.TLF01_details.acquiring_id[0] == 0x00 )
      {
         memcpy( Auth_Tx.TLF01_details.acquiring_id,
                 HOST_MESSAGE.acq_inst_id,
                 len );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_33
 *
 *  DESCRIPTION:  This function will move the forwarding institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
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
INT build_request_field_33( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR temp_len[3]="";

   if ( *result_flag == 0 )
   {
      if ( Auth_Tx.forwarding_inst_id_cd[0] != 0x00 )
      {
         len = strlen( Auth_Tx.forwarding_inst_id_cd );
         memcpy( HOST_MESSAGE.fwd_inst_id, Auth_Tx.forwarding_inst_id_cd, len );

         sprintf( temp_len, "%02d", len );
         memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, 2 );
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_35
 *
 *  DESCRIPTION:  This function will move the track2 data
 *                to the Host Message structure according to Equitable
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
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_35( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len = 0;
   CHAR temp_len[3]="";
   
   
   if ( *result_flag == 0 )
   {
   
      if(Auth_Tx.TLF01_details.track2[0] != 0x00)
	  {
         len = strlen( Auth_Tx.TLF01_details.track2 );
         memcpy( HOST_MESSAGE.track2, Auth_Tx.TLF01_details.track2, len );

	     sprintf( temp_len, "%02d", len );
	 	 memcpy( HOST_MESSAGE.track2_len, temp_len, 2 );


	  }
   }

   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_37
 *
 *  DESCRIPTION:  This function will move the Acquirer reference number
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
INT build_request_field_37( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
       memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,DE37_SIZE);
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_request_field_38
 *
 *  DESCRIPTION:  This function will move the authorization number
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
INT build_request_field_38( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( false == isalphanum_space(Auth_Tx.TLF01_details.auth_number) )
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
INT build_request_field_39( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   CHAR amex_action_code[4] = {0};

   if(!map_amex_action_code_to_iso_repsonse_code(amex_action_code))
   {
	 	 *result_flag = 1;
		 ret_val = false;
		 sprintf( err_buf,
				 "Cannot convert between transform iso response code %s to Amex action code",Auth_Tx.TLF01_details.response_code );
   }
   if ( *result_flag == 0 )
   {
      if ( amex_action_code[0] == 0x00 )
      {
			memcpy( HOST_MESSAGE.responsecd, "000", DE39_SIZE );
      }
      else
      {
			memcpy( HOST_MESSAGE.responsecd,
                 amex_action_code,
                 DE39_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_41
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric with spaces,
 *                fixed length.
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
INT build_request_field_41( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.termid,
              Auth_Tx.TLF01_details.terminal_id,
              DE41_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_42
 *
 *  DESCRIPTION:  This function will move the merchant Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
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
INT build_request_field_42( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   /* According to Equitable:  They want
			<6th to 15th of MerchantID> pad with spaces (5) at the end */

	   memset(HOST_MESSAGE.cardaccid,0x20,DE42_SIZE);

	   memcpy( HOST_MESSAGE.cardaccid,
              &Auth_Tx.TLF01_details.merchant_id[5],
              10 );

   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_43
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
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
INT build_request_field_43( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   CHAR temp_len[3]="",*temp,*temp1;
   INT  f43_Len = 0,fieldLen=0,tempInt=0;
/*   INT postal_code_pos = 83; */
   INT postal_code_len = 10;
/*   INT region_code_pos = 93; */
   INT region_code_len = 3;
/*   INT country_code_pos = 96; */
   INT country_code_len = 3;
   INT len = 0;

/*  According to the new mandate DE43 should be in below format - TF Phani
*	Merchant~One\\Anytown\85000~~~~~AZ~840
*/
   if ( *result_flag == 0 )
   {
	   //memset(HOST_MESSAGE.cardaccname,0x20,MAX_DE43_SIZE);
		memset(HOST_MESSAGE.cardaccname,'~',MAX_DE43_SIZE);
	 
  	 if( strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0)
      {
         /* Transaction is ATM. */
         //Name
         memcpy( HOST_MESSAGE.cardaccname, Auth_Tx.MCF01_details.address01,
                 strlen(Auth_Tx.MCF01_details.address01));	
		 fieldLen = strlen(Auth_Tx.MCF01_details.address01);

      }
      else
      {
         /* Transaction is a POS transaction. */
         /* Merchant Name*/

    	 len = ncamaq_find_data_length_without_last_space(Auth_Tx.MCF01_details.name01);
    	 if (len > 0)
    	 {
             memcpy( HOST_MESSAGE.cardaccname, Auth_Tx.MCF01_details.name01, len);
    	 }
		 fieldLen = len;
		 memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
		 fieldLen +=1;

		 /*Adress Line 1*/
		 len = ncamaq_find_data_length_without_last_space(Auth_Tx.MCF01_details.address01);
		 if (len > 0)
		 {
			 memcpy( HOST_MESSAGE.cardaccname +fieldLen, Auth_Tx.MCF01_details.address01, len);
		 }
		 fieldLen = fieldLen + len;
		 memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
		 fieldLen +=1;

		 /*Adress Line 2*/
		 /*memcpy( HOST_MESSAGE.cardaccname +fieldLen, Auth_Tx.MCF01_details.address02,
				 strlen(Auth_Tx.MCF01_details.address02));

		 fieldLen = fieldLen + strlen(Auth_Tx.MCF01_details.address02);*/
	}

  	 if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",5) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit",4)))
	 {
		 memcpy( HOST_MESSAGE.cardaccname+25,Auth_Tx.MCF01_details.city,strlen(Auth_Tx.MCF01_details.city));
		 fieldLen = fieldLen + strlen(Auth_Tx.MCF01_details.city);
		 memcpy( HOST_MESSAGE.cardaccname+ 38, "PH", 2);// Starting Point of Country location
		 fieldLen +=2;

		 if(fieldLen>40)
		 {
			 fieldLen = 40;
		 }

	 }
  	 else
  	 {
		 /*memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
		 fieldLen +=1;
      
		 memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);	
		 fieldLen +=1;*/

		/*City */
  		len = ncamaq_find_data_length_without_last_space(Auth_Tx.MCF01_details.city);
  		if (len > 0)
  		{
  		 	memcpy( HOST_MESSAGE.cardaccname+ fieldLen,Auth_Tx.MCF01_details.city,len);
  		}
		fieldLen = fieldLen + len;

		memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
		fieldLen +=1;

		/*Postal code */
	/*  memset(HOST_MESSAGE.cardaccname + postal_code_pos ,'~',postal_code_len); */
		memset(HOST_MESSAGE.cardaccname + fieldLen ,'~',postal_code_len);
		len = ncamaq_find_data_length_without_last_space(Auth_Tx.MCF01_details.zip_code);
	/*  memcpy( HOST_MESSAGE.cardaccname+ postal_code_pos,Auth_Tx.MCF01_details.zip_code,len); */
		if (len > 0)
		{
			memcpy( HOST_MESSAGE.cardaccname+ fieldLen,Auth_Tx.MCF01_details.zip_code,len);
			fieldLen = fieldLen + postal_code_len;
		}
		else
		{
			 memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
		     fieldLen = fieldLen + 1;
		}

		/* Region code */
		len = ncamaq_find_data_length_without_last_space(Auth_Tx.MCF01_details.intl_code);
	/*	memcpy( HOST_MESSAGE.cardaccname+region_code_pos, Auth_Tx.MCF01_details.intl_code, len); */
		if (len > 0)
		{
			memcpy( HOST_MESSAGE.cardaccname+fieldLen, Auth_Tx.MCF01_details.intl_code, len);
			fieldLen = fieldLen + region_code_len;
		}
		else
		{
			 memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
				fieldLen = fieldLen + 1;
		}

	    /* Country code */
		/* memcpy( HOST_MESSAGE.cardaccname+fieldLen,Auth_Tx.MCF01_details.intl_code, len);*/
		if (len > 0)
		{
			memcpy( HOST_MESSAGE.cardaccname+fieldLen, Auth_Tx.MCF01_details.intl_code, len);
			fieldLen = fieldLen + country_code_len;
		}
		else
		{
			 memset( HOST_MESSAGE.cardaccname+fieldLen,0x5c,1);
			fieldLen = fieldLen + 1;
		}


	/*	fieldLen = 99;*/

  	 }
		temp = &HOST_MESSAGE.cardaccname[0];
		tempInt = fieldLen;
		  while(temp != NULL)
		  {
			  temp1 = memchr(temp,0x20,tempInt);
			  if(temp1 != NULL)
			  {
				  *temp1 = '~';
				  tempInt -= (temp1- temp);
	
			  }

			  temp =temp1;
		  }
		
	    /* Get the length. */
	    f43_Len = fieldLen;
		sprintf( temp_len, "%02d", f43_Len );
		memcpy( HOST_MESSAGE.cardaccnamelen, temp_len, 2 );
		/*HOST_MESSAGE.cardaccname[f43_Len] ='\0';*/

	   /* Store DE43 here so it gets logged to TLF01:
       *   Merchant Name         --> TLF01.card_holder_name
       *   City and Country Code --> TLF01.ncf01_retired_cwk
       */

		memcpy( Auth_Tx.TLF01_details.card_holder_name,
              HOST_MESSAGE.cardaccname,
              25 );
		memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk,
              HOST_MESSAGE.cardaccname+25,
              15 );
		
   }

    
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_45
 *
 *  DESCRIPTION:  This function will move the track1
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, variable length.
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
INT build_request_field_45( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR temp_len[3]="";

   if ( *result_flag == 0 )
   {
      if(Auth_Tx.TLF01_details.track1[0] != 0x00)
      {
         /* Track 1 is present. Do not send track 2. */
         memset(HOST_MESSAGE.track2_len, 0x00, sizeof(HOST_MESSAGE.track2_len));

         len = strlen( Auth_Tx.TLF01_details.track1 );
         memcpy( HOST_MESSAGE.track1, Auth_Tx.TLF01_details.track1, len );

         sprintf( temp_len, "%02d", len );
         memcpy( HOST_MESSAGE.track1_len, temp_len, 2 );
      }
   }
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         build_request_field_49
 *
 *  DESCRIPTION:  This function will move the currency code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
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
INT build_request_field_49( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   if(Auth_Tx.MCF01_details.currency_code[0] != 0x00)
	   {
			if ( false == isalphanum(Auth_Tx.MCF01_details.currency_code) )
			{
				 ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code (%s)",
					  Auth_Tx.MCF01_details.currency_code );
			}
			else
			{
					memcpy( HOST_MESSAGE.tran_curr_cd,
					Auth_Tx.MCF01_details.currency_code,
					DE49_SIZE );

					/* Copy this currency code to TLF01.currency_code for correct logging */
					memcpy( Auth_Tx.TLF01_details.currency_code,
					 Auth_Tx.MCF01_details.currency_code,
					 DE49_SIZE );

			}
	   }
	   else  if(Auth_Tx.TLF01_details.currency_code[0] != 0x00)
	   {
		   	if ( false == isalphanum(Auth_Tx.TLF01_details.currency_code) )
			{
				 ret_val = false;
				*result_flag = 1;
				sprintf( err_buf,
					 "Unable to build request msg to host. Invalid currency code (%s)",
					  Auth_Tx.TLF01_details.currency_code );
			}
			else
					memcpy( HOST_MESSAGE.tran_curr_cd,
					Auth_Tx.TLF01_details.currency_code,
					DE49_SIZE );
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
 *  DESCRIPTION:  This function will move the currency code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
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
INT build_request_field_50 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_52
 *
 *  DESCRIPTION:  This function will move the pin block
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
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
         memset( HOST_MESSAGE.pinblock, 0x00, DE52_SIZE );
      }
      else
      {
         memset( HOST_MESSAGE.pinblock, 0x00, DE52_SIZE );
         memset( tempstr, 0x00, sizeof(tempstr) );

         /* Convert to Hex */
         ascii_to_bin( Auth_Tx.TLF01_details.pin_block,
                       strlen(Auth_Tx.TLF01_details.pin_block),
                       &tempstr[0], sizeof(tempstr) );

         memcpy(HOST_MESSAGE.pinblock, tempstr, sizeof(HOST_MESSAGE.pinblock) );

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
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
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
 *  AUTHOR:       Sreerama Velaga
 *
 ******************************************************************************/
INT build_request_field_53 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len = 0;
   CHAR temp_len[3] = {0};

   if(Auth_Tx.TLF01_details.pos_entry_mode[0] == 0x00)
   {
      ret_val = false;
   }
   else
   {
      if( (HOST_MESSAGE.posentry[6] == 'W') ||
          (HOST_MESSAGE.posentry[6] == 'S') ||
  		  ((HOST_MESSAGE.posentry[6] == '1') &&
		   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2))))
      {
    	 if(Auth_Tx.TLF01_details.interest_rate[0] != 0x00 )
    	 {
    		 /* Get the length. */
    		 len = strlen(Auth_Tx.TLF01_details.interest_rate);
    		 sprintf( temp_len, "%02d", len );
    		 memcpy( HOST_MESSAGE.sec_cntl_len, temp_len, 2 );

    		 /* 4DBC or 4CSC */

    		 /* Security control information is coming in
    		  * auth_Tx.tlf01_details.interest_rate.
    		  */
    		 memcpy(HOST_MESSAGE.sec_cntl,Auth_Tx.TLF01_details.interest_rate,len);
    	 }
      }
   }
   return( ret_val );
}
/******************************************************************************
/******************************************************************************/

INT build_request_field_55 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR temp_len[3]="";
   BYTE msg_hdr[5]="AGNS";
   BYTE De55Buffer[269]={0};
   BYTE Bin_De55Buffer[135]={0};
   BYTE HeaderVer[5]="0001";
   BYTE Bin_hdr_ver[3]={0};
   INT de55_len=0,IADlen=0;
   INT asciiTagLen=0,binTagLen=0,emv_len=0; 
  
//HOST_MESSAGE.IccData_len
   if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
        (Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
   	{
   	 return true;
   	}else if( strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",5)== 0 ||
	   strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit",4)== 0)
   	{
   	   // IST transaction coming from NCEQIT
   		emv_len = Auth_Tx.EMV_details.emv_block[0];
	  /* if(emv_len > 0 && emv_len < 255 )
	   {   memcpy(HOST_MESSAGE.IccData_len,Auth_Tx.EMV_details.emv_block,emv_len+1);
		   memcpy(HOST_MESSAGE.IccData,Auth_Tx.EMV_details.emv_block+1,emv_len);
	   }
	   return true; */
   	}
	else
   	{
		emv_len = genutil_bcd_to_int( Auth_Tx.EMV_details.emv_block, 2 );
		if(emv_len == 0)
		return true;
   	}
   
   convert_to_ebcdic(msg_hdr,HOST_MESSAGE.IccData,DE55_HDR_LEN);
   de55_len += DE55_HDR_LEN;
   ascii_to_bin(HeaderVer,strlen(HeaderVer),&Bin_hdr_ver[0],2);
   memcpy(&HOST_MESSAGE.IccData[de55_len],Bin_hdr_ver,DE55_HDR_VER);
   de55_len += DE55_HDR_VER;
 
	//ICC header  EBCDIC

	//ICC header version

// AC
	// prepare ICC data.
	strncpy(De55Buffer,Auth_Tx.EMV_details.app_crypto, 16);
	asciiTagLen = 16;
	ascii_to_bin(De55Buffer,asciiTagLen,&Bin_De55Buffer[0],asciiTagLen/2);
	memcpy(&HOST_MESSAGE.IccData[de55_len],Bin_De55Buffer,8);
	 de55_len += 8;
	memset(De55Buffer,0x00,sizeof(De55Buffer));
	memset(Bin_De55Buffer,0x00,sizeof(Bin_De55Buffer));
	
	//IAD
	IADlen = strlen(Auth_Tx.EMV_details.iad);
	sprintf(De55Buffer,"%02d",IADlen/2);
	strncpy(&De55Buffer[2],Auth_Tx.EMV_details.iad, IADlen);
	IADlen +=2;
	ascii_to_bin(De55Buffer,IADlen,&Bin_De55Buffer[0],IADlen/2);
	memcpy(&HOST_MESSAGE.IccData[de55_len],Bin_De55Buffer,IADlen/2);
	 de55_len += IADlen/2;
	memset(De55Buffer,0x00,sizeof(De55Buffer));
	memset(Bin_De55Buffer,0x00,sizeof(Bin_De55Buffer));
	//UN
	strncpy(De55Buffer,Auth_Tx.EMV_details.unpredictable_number, EMV_AMEX_UNPREDICTABLE_NBR_LEN*2);
	//ATC
	strncat(De55Buffer,Auth_Tx.EMV_details.atc, EMV_AMEX_ATC_LEN*2);
	//TVR
	strncat(De55Buffer,Auth_Tx.EMV_details.tvr, EMV_AMEX_TVR_LEN*2);
	//T D
	strncat(De55Buffer,Auth_Tx.EMV_details.tran_date, EMV_AMEX_TX_DATE_LEN*2);
	//T T
	strncat(De55Buffer,Auth_Tx.EMV_details.tran_type, EMV_AMEX_TX_TYPE_LEN*2);
	//A A
	strncat(De55Buffer,Auth_Tx.EMV_details.tran_amount, EMV_AMEX_TX_AMOUNT_LEN*2);
	//TCC
	strncat(De55Buffer,Auth_Tx.EMV_details.currency_code, EMV_AMEX_TX_CURRENCY_CODE_LEN*2);
	//Terminal Country codce
	//Buffer for TX country code
	strncat(De55Buffer,"0", 1);
	strncat(De55Buffer,Auth_Tx.EMV_details.country_code, 3);
	//A I P
	strncat(De55Buffer,Auth_Tx.EMV_details.interchange_profile, EMV_AMEX_AIP_LEN*2);
	// A O
	strncat(De55Buffer,Auth_Tx.EMV_details.other_amount,EMV_AMEX_AMT_OTHER_LEN*2);
	// A P N
	if(strlen(Auth_Tx.EMV_details.pan_sequence_number) != 0)
	strncat(De55Buffer,Auth_Tx.EMV_details.pan_sequence_number, EMV_AMEX_PAN_SEQ_NBR_LEN*2);
	else
	strncat(De55Buffer,Auth_Tx.host2_message_seq_nbr, EMV_AMEX_PAN_SEQ_NBR_LEN*2);	
	//C I D
	strncat(De55Buffer,Auth_Tx.EMV_details.crypto_info, EMV_AMEX_CRYPTO_INFO_LEN*2);
	asciiTagLen = strlen(De55Buffer);
	ascii_to_bin(De55Buffer,asciiTagLen,&Bin_De55Buffer[0],asciiTagLen/2);
    memcpy(&HOST_MESSAGE.IccData[de55_len],Bin_De55Buffer,asciiTagLen/2);
    de55_len += asciiTagLen/2;

	sprintf(temp_len,"%03d",de55_len);
	memcpy(HOST_MESSAGE.IccData_len,temp_len,3);

   return( ret_val );
}
/******************************************************************************/


/******************************************************************************
 *
 *  NAME:         build_request_field_56
 *
 *  DESCRIPTION:  This function will move the 
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
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
INT build_request_field_56 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   INT  ATM;
   CHAR temp_len[3]="";

   memset( HOST_MESSAGE.orig_data, 0x30, 22 );

   if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin, "RO") )
   {
      memcpy(HOST_MESSAGE.orig_data, "1200", 4);
      ATM = true;
   }
   else
   {
      memcpy(HOST_MESSAGE.orig_data, "1100", 4);
      ATM = false;
   }

   if ( Auth_Tx.TLF01_details.orig_sys_trace_num[0] != 0x00 )
   {
      memcpy( HOST_MESSAGE.orig_data+4,
              Auth_Tx.TLF01_details.orig_sys_trace_num,
              6 );
   }

   /* Get value from bit 12 of original 1100 or 1200. */
   if ( Auth_Tx.local_date_time[0] != 0x00 )
   {
      if ( 10 == strlen(Auth_Tx.local_date_time) )
      {
         /* Get original year */
         memcpy( HOST_MESSAGE.orig_data+10, HOST_MESSAGE.datetime, 2 );
         memcpy( HOST_MESSAGE.orig_data+12, Auth_Tx.local_date_time, 10 );
      }
      else
      {
         memcpy( HOST_MESSAGE.orig_data+10, Auth_Tx.local_date_time, 12 );
   }
   }
   else if (Auth_Tx.orig_local_time_date[0] != 0x00)
   {
	      if ( 10 == strlen(Auth_Tx.orig_local_time_date) )
	      {
	         /* Get original year */
	         memcpy( HOST_MESSAGE.orig_data+10, HOST_MESSAGE.datetime, 2 );
	         memcpy( HOST_MESSAGE.orig_data+12, Auth_Tx.orig_local_time_date, 10 );
	      }
	      else
	      {
	         memcpy( HOST_MESSAGE.orig_data+10, Auth_Tx.orig_local_time_date, 12 );
	      }
   }

#if BDOR_62_PREAUTH_VOID
   if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
   {
	   if (strlen(Auth_Tx.local_date_time) > 0)
	   {
		   memcpy( HOST_MESSAGE.orig_data+10, Auth_Tx.local_date_time, 12 );
	   }
   }
#endif

   if( ATM == true )
   {
      /* For ATM, use value from original message. */
      len = strlen( Ncf01_I.acquirer_inst_id_code );
      sprintf( temp_len, "%02d", len );
      memcpy( HOST_MESSAGE.orig_data+22, temp_len, 2 );
      memcpy( HOST_MESSAGE.orig_data+24, Ncf01_I.acquirer_inst_id_code, len );
      len += 24;
      memset( temp_len, 0x00,sizeof(temp_len) );
   }
   else
   {
      /* Original acquiring institution ID code is hard coded for POS. */
      memcpy( HOST_MESSAGE.orig_data+22, ACQ_INSTN_ID, 8 );
      len = 30;
   }

   sprintf( temp_len, "%02d", len );
   memcpy( HOST_MESSAGE.orig_data_len, temp_len, 2 );

   return( ret_val );
}

#if AMEX03B_APRIL2024
INT build_request_field_60(pINT result_flag, pCHAR err_buf)
{
	INT  ret_val = true;
	CHAR temp_len[6]="";
	BYTE temp_str[5]="";
	INT  len=0,ind=0, ind1;
	char bit_60_id[6]="AXAAD";
	char bit_60_7_bitmap[9]="02000000"; /* this bit map enables bit 60.7*/
	char bit_60_8_bitmap[9]="01000000";  /* this bit map enables bit 60.8*/
	char bitmap[9]={0};
	char bit60_len[4]={0};
	char terminal_classification_code_with_pin [3]={"CS"};
	char terminal_classification_code_without_pin [3]={"CC"};

	if(Auth_Tx.TLF01_details.visa_tran_id[0] != '\0' && 0 == strncmp(HOST_MESSAGE.func_cd,"106",3))
	{
			/*sample transaction shared By BDO UAT AMEX simulator
			 * F0F2F4C1E7C1C1C402000000F1F0F0F9F9F2F9F2F6F7F7F5F3F1F1
			 * */
			strncpy(bit60_len, "024",3);
			for(ind=0;ind<strlen(bit_60_id);ind++)
			{
				HOST_MESSAGE.priv60[ind]=ascii_ebcdic_conv (bit_60_id[ind], A2E);
			}
			genutil_str_asc_to_bcd(&bit_60_7_bitmap,8,&bitmap);

			memcpy(HOST_MESSAGE.priv60+5,bitmap,4);

			for(ind1=0,ind=9; ind1 < 15/*original tid len*/;ind1++,ind++)
			{
				HOST_MESSAGE.priv60[ind]=ascii_ebcdic_conv (Auth_Tx.TLF01_details.visa_tran_id[ind1], A2E);
			}

			for( ind=0; ind<3; ind++ )
		   {
			   HOST_MESSAGE.priv60_len[ind] = ascii_ebcdic_conv (bit60_len[ind], A2E);
		   }
	}
#if BDOR_68_REQ_1_AMEX
	   /*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions With Pin*/
	   if(true == ncamaq_check_if_trx_is_amex_MobilePOS_NFC_Tap2Phone_trx_with_pin(Auth_Tx))
	   {
		   strncpy(bit60_len, "011",3);
		   for(ind=0;ind<strlen(bit_60_id);ind++)
		   {
				HOST_MESSAGE.priv60[ind]=ascii_ebcdic_conv (bit_60_id[ind], A2E);
		   }

		   genutil_str_asc_to_bcd(&bit_60_8_bitmap,8,&bitmap);
		   memcpy(HOST_MESSAGE.priv60+5,bitmap,4);

		   for(ind1=0,ind=9; ind1 < 2;ind1++,ind++)
		   {
				HOST_MESSAGE.priv60[ind]=ascii_ebcdic_conv (terminal_classification_code_with_pin[ind1], A2E);
		   }
		   for( ind=0; ind<3; ind++ )
		   {
			   HOST_MESSAGE.priv60_len[ind] = ascii_ebcdic_conv (bit60_len[ind], A2E);
		   }
	   }
	   /*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions Without Pin*/
	   else if(true == ncamaq_check_if_trx_is_amex_MobilePOS_NFC_Tap2Phone_trx_without_pin(Auth_Tx))
	   {
		   strncpy(bit60_len, "011",3);
		   for(ind=0;ind<strlen(bit_60_id);ind++)
		   {
				HOST_MESSAGE.priv60[ind]=ascii_ebcdic_conv (bit_60_id[ind], A2E);
		   }
		   genutil_str_asc_to_bcd(&bit_60_8_bitmap,8,&bitmap);
		   memcpy(HOST_MESSAGE.priv60+5,bitmap,4);

		   for(ind1=0,ind=9; ind1 < 2;ind1++,ind++)
		   {
				HOST_MESSAGE.priv60[ind]=ascii_ebcdic_conv (terminal_classification_code_without_pin[ind1], A2E);
		   }
		   for( ind=0; ind<3; ind++ )
		   {
			   HOST_MESSAGE.priv60_len[ind] = ascii_ebcdic_conv (bit60_len[ind], A2E);
		   }
	   }
#endif
	return (ret_val);
}
#endif
/******************************************************************************
 *
 *  NAME:         build_request_field_61
 *
 *  DESCRIPTION:  This function will build 61 Safe key datat
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
 *  AUTHOR:      Phani
 *
 ******************************************************************************/

INT build_request_field_61 ( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len=0;
   INT 	i = 0;
   INT  data_len=0;
   CHAR temp_len[6]={0};
   BYTE temp_str[5]={0};
   BYTE bcd_eci[5]={0};
   BYTE ascii_eci[5]={0};

   BYTE Ecommerce_swiftpass_bdopay_ECI[3]={0};

#if BDOR_68_REQ_5_AMEX /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

   if(true == ncamaq_check_if_trx_is_amex_ecommerce_swiftpass_BDOPay_trx ())
	{
	   strncpy(Ecommerce_swiftpass_bdopay_ECI, DE61_ECI_VALUE_NOT_AUTHENTICATED, 2);

	   data_len = strlen(Ecommerce_swiftpass_bdopay_ECI);
	   sprintf(temp_str,"%03d",data_len);
	   memcpy(HOST_MESSAGE.priv61_len,temp_str,3);
	   /*memcpy(HOST_MESSAGE.priv61,Ecommerce_swiftpass_bdopay_ECI,data_len);*/
	   for ( i = 0; i < 2; i++)
	   {
		   HOST_MESSAGE.priv61[i] = ascii_ebcdic_conv (Ecommerce_swiftpass_bdopay_ECI[i], A2E);
	   }

	   /*Copy ECI value*/
		memcpy( bcd_eci,Ecommerce_swiftpass_bdopay_ECI,2);
		convert_to_ascii(bcd_eci,ascii_eci,2);
		memcpy( Auth_Tx.TLF01_details.profile,ascii_eci,2);
   }
#endif
   else
   {
	   strncpy(temp_len,Auth_Tx.int_net_facility_len,3);
	
	   len = atoi(temp_len);
	   data_len = len-2;

		if(data_len>0 && data_len < 100)
		{
			sprintf(temp_str,"%03d",data_len);
			memcpy(HOST_MESSAGE.priv61_len,temp_str,3);
			memcpy(HOST_MESSAGE.priv61,&Auth_Tx.int_net_facility_data[2],data_len);
			/*Copy ECI value*/
			memcpy( bcd_eci,&Auth_Tx.int_net_facility_data[7],2);
			convert_to_ascii(bcd_eci,ascii_eci,2);
			memcpy( Auth_Tx.TLF01_details.profile,ascii_eci,2);
		}
   }
  
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
	   memcpy( HOST_MESSAGE.datetime,
               &Auth_Tx.TLF01_details.date_yyyymmdd[2],
              6);
	   memcpy( &HOST_MESSAGE.datetime[6],
              Auth_Tx.TLF01_details.time_hhmmss,
              6);
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_24
 *
 *  DESCRIPTION:  This function will move the Function code
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
INT build_response_field_24( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
       memcpy( HOST_MESSAGE.func_cd,
			  Auth_Tx.function_code,
			  DE24_SIZE );
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_response_field_33
 *
 *  DESCRIPTION:  This function will move the Forwarding institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length. This is an echo from the request.  Therefore, there
 *                is no reason to validate the data here.
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
INT build_response_field_33( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;


   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.forwarding_inst_id_cd );
      if ( len == 0 )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf,
                "Unable to build response message to host. No acquiring ID." );
      }
      else
      {
         sprintf( HOST_MESSAGE.fwd_inst_len, "%02d", len );
         strcpy( HOST_MESSAGE.fwd_inst_id, Auth_Tx.forwarding_inst_id_cd );
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
   CHAR amex_action_code[4] = {0};

   if(!map_amex_action_code_to_iso_repsonse_code(amex_action_code))
   {
	 	 *result_flag = 1;
		 ret_val = false;
		 sprintf( err_buf,
				 "Cannot convert between transform iso response code %s to Amex action code",Auth_Tx.TLF01_details.response_code );
   }
   if ( *result_flag == 0 )
   {
      if ( amex_action_code[0] == 0x00 )
      {
         memcpy( HOST_MESSAGE.responsecd, amex_action_code, DE39_SIZE );
      }
      else
      {
         memcpy( HOST_MESSAGE.responsecd,
                 amex_action_code,
                 DE39_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_2
 *
 *  DESCRIPTION:  This function will move the card number from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits.
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_2( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;

   if ( *result_flag == 0 )
   {
      /* First get the length of the card number. */
      memcpy( Auth_Tx.TLF01_details.card_num_len,
              HOST_MESSAGE.pan_len, DE2_LEN_SIZE );
      len = atoi(Auth_Tx.TLF01_details.card_num_len);

      if ( len <= MAX_DE2_SIZE )
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
         sprintf( err_buf, "Invalid length of card number: %d,", len );
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
 *                Message structure according to Equitable formatting
 *                specifications.  Processing code is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_3( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.processing_code,
              HOST_MESSAGE.proccode, DE3_SIZE );
  	if( strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 ||
  		strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",3) == 0	)
  	{
  		// do nothing as you copied it in while sendig the request
  	}
  	else
  	{
        /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
        memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,
                      HOST_MESSAGE.proccode, DE3_SIZE );
  	}

      if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.processing_code)))
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric processing code: %s,",
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
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_4( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.total_amount,
              HOST_MESSAGE.amount, DE4_SIZE );
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
 *  NAME:         move_field_5
 *
 *  DESCRIPTION:  This function will move the reconciliation amount from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
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
 *  AUTHOR:       Sree Velaga
 *
 ******************************************************************************/
INT move_field_5( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.reconciliation_amount,
              HOST_MESSAGE.setl_amount, DE5_SIZE );
      if (false ==(ret_val = isnum(Auth_Tx.reconciliation_amount)))
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric amount (field 5): %s,",
                  Auth_Tx.reconciliation_amount );
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         move_field_7
 *
 *  DESCRIPTION:  This function will move the transmission time from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Transmission time is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  gmt_timedate[20];

   if ( *result_flag == 0 )
   {
      /* Only do this for a request from the host.
       * Ignore for responses, as it may not be there.
       */
      if ( TranFlow == SWITCHED_IN )
      {
         memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
                 HOST_MESSAGE.gmt_time, DE7_SIZE );
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
         else
         {
            /* This date-time is expressed in local (Manila) time.
             * Save it for the response, then convert it to GMT
             * for Visa, JCB, and Cirrus.
             */
            memcpy( Auth_Tx.TLF01_details.product_codes[19].amount,
                    Auth_Tx.TLF01_details.transmission_timestamp,
                    DE7_SIZE );

            memset( gmt_timedate, 0x00, sizeof(gmt_timedate) );
            pintime_get_gmt( gmt_timedate );                /* YYYYMMDDhhmmss */

            memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
                    gmt_timedate+4, 10 );                   /*     MMDDhhmmss */
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
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_11( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
              HOST_MESSAGE.stan, DE11_SIZE );
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
 *                Message structure according to Equitable formatting
 *                specifications.  Transaction time is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  datetime[15] = "";

   if ( *result_flag == 0 )
   {
	  /* Date */
	  memcpy( Auth_Tx.TLF01_details.date_yyyymmdd,
              "20", 2 );
      memcpy( &Auth_Tx.TLF01_details.date_yyyymmdd[2],
              HOST_MESSAGE.datetime, 6 );
	  /* Time */
	  memcpy( Auth_Tx.TLF01_details.time_hhmmss,
              &HOST_MESSAGE.datetime[6], 6);

	  memcpy(datetime,Auth_Tx.TLF01_details.date_yyyymmdd,8);
	  memcpy(&datetime[8],Auth_Tx.TLF01_details.time_hhmmss,6);

      ret_val = isnum(datetime);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric transaction time (field 12): %s,",
                  datetime );
      }
      else if ( false == validate_date_time(datetime))
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Invalid transaction date time (field 12): %s,",
                  datetime );
      }
   }
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         move_field_19
 *
 *  DESCRIPTION:  This function will move the Acquirer institution country code 
 *				  from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_19( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.country_code,
              HOST_MESSAGE.ctry_code, DE19_SIZE );
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_22
 *
 *  DESCRIPTION:  This function will move the POS Entry Mode from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  POS Entry Mode is numeric, fixed
 *                length.  For Equitable host, it will always be the
 *                same value.
 *
 *                Prepend '0' to put it into Hypercom ISO format.
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy(Auth_Tx.amex_pos_entry_mode, HOST_MESSAGE.posentry, DE22_SIZE );
      ret_val = isalphanum(Auth_Tx.amex_pos_entry_mode);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric POS Entry Mode (field 22): %s,",
                  Auth_Tx.amex_pos_entry_mode );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_24
 *
 *  DESCRIPTION:  This function will move the Function Code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Function Code is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_24( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.function_code,
              HOST_MESSAGE.func_cd, DE24_SIZE );
      ret_val = isnum(Auth_Tx.function_code);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric POS Condition Code (field 24): %s,",
                  Auth_Tx.function_code );
      }
   }
   return( ret_val );
}





/******************************************************************************
 *
 *  NAME:         move_field_26
 *
 *  DESCRIPTION:  This function will move the Card acceptor business code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is also called Merchant category code.
 *				  Card acceptor business code is numeric, fixed
 *                length.  For Equitable host, it will always one of two
 *                values.
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
 *  AUTHOR:       Sree Velaga
 *
 ******************************************************************************/
INT move_field_26( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	   if(HOST_MESSAGE.accpt_bus_cd[0] == 0x00)
	   {
			memcpy( Auth_Tx.TLF01_details.category_code,
					  HOST_MESSAGE.accpt_bus_cd, DE26_SIZE );
			ret_val = isnum(Auth_Tx.TLF01_details.category_code);
			if ( ret_val == false )
			{
				*result_flag = 1;
				sprintf( err_buf,
					 "Non-numeric card acceptor business Code (field 26): %s,",
					Auth_Tx.TLF01_details.category_code );
			}
      }
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_31
 *
 *  DESCRIPTION:  This function will move the Transaction Identifier from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Transaction ID is alphanumeric, variable
 *                length up to 2 digits. (From the requirements, it will
 *                always be 2 alphanumeric characters).
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
INT move_field_31( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true,int_length;
   CHAR   tempstr[101];  

   if ( *result_flag == 0 )
   {
	   memset (tempstr, 0, sizeof(tempstr));
	   memcpy (tempstr, HOST_MESSAGE.tranid_len, 2);
	   int_length = atoi(tempstr);
	   if(int_length > 15)
	   {
		memcpy(Auth_Tx.TLF01_details.visa_tran_id,HOST_MESSAGE.tranid,15);
		memcpy(Auth_Tx.TLF01_details.visa_validation_code,&HOST_MESSAGE.tranid[15],int_length-15);
	   }else
	   memcpy(Auth_Tx.TLF01_details.visa_tran_id,HOST_MESSAGE.tranid,int_length);

   }

      
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_32
 *
 *  DESCRIPTION:  This function will move the Acquiring Inst ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Acquiring Inst ID is alphanumeric, variable
 *                length up to 2 digits. (From the requirements, it will
 *                always be 2 alphanumeric characters).
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_32( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR  len_str[3]="";

   if ( *result_flag == 0 )
   {
      /* First get the length. */
      memcpy( len_str, HOST_MESSAGE.acq_inst_len, 2 );
      len = atoi( len_str );
      if ( len > 0 )
      {
         /* Now copy the Id into Auth_Tx and check it. */
         memcpy( Auth_Tx.acq_inst_id_cd, HOST_MESSAGE.acq_inst_id, len );
         if (false ==(ret_val = isalphanum(Auth_Tx.acq_inst_id_cd)))
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Non-alphanumeric acquiring inst. Id (field 32): %s,",
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
         sprintf( err_buf, "Invalid length of acquiring inst. Id: %c%c,",
                  HOST_MESSAGE.acq_inst_len[0],
                  HOST_MESSAGE.acq_inst_len[1] );
         ret_val = false;
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_33
 *
 *  DESCRIPTION:  This function will move the Forwarding Inst ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Acquiring Inst ID is alphanumeric, variable
 *                length up to 2 digits. (From the requirements, it will
 *                always be 2 alphanumeric characters).
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_33( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR  len_str[3]="";

   if ( *result_flag == 0 )
   {
      /* First get the length. */
      memcpy( len_str, HOST_MESSAGE.fwd_inst_len, 2 );
      len = atoi( len_str );
      if ( len > 0 )
      {
         /* Now copy the Id into Auth_Tx and check it. */
         memcpy( Auth_Tx.forwarding_inst_id_cd, HOST_MESSAGE.fwd_inst_id, len );
         if (false ==(ret_val = isalphanum(Auth_Tx.forwarding_inst_id_cd)))
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Non-alphanumeric acquiring inst. Id (field 33): %s,",
                     Auth_Tx.forwarding_inst_id_cd );
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
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_37( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	  if(Auth_Tx.TLF01_details.retrieval_ref_num[0] == 0x00)
	  {
		  memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
				  HOST_MESSAGE.rrn, DE11_SIZE );
		  ret_val = isalphanum(Auth_Tx.TLF01_details.retrieval_ref_num);  // ThoughtFocus Girija Y: Changed from isnum to isalphanumeric
		  if ( ret_val == false )
		  {
			 *result_flag = 1;
			 sprintf( err_buf,
					 "Non-numeric Retrieval Refence Number (field 37): %s,",
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
 *                Message structure according to Equitable formatting
 *                specifications.  Auth Number is alphanumeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_38( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.auth_number,
              HOST_MESSAGE.authcode, DE38_SIZE );
      ret_val = isalphanum_space(Auth_Tx.TLF01_details.auth_number);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in Auth Number (field 38) not allowed: %s,",
                  Auth_Tx.TLF01_details.auth_number );
      }
      else
      {
          if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "nceqit", 6)) ||
         	 (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "neqit2", 6)))
          {
        	  trim_trailing_spaces( Auth_Tx.TLF01_details.auth_number );
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
 *                Message structure according to Equitable formatting
 *                specifications.  Response Code is alphanumeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_39( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  amex_action_code[4] = {0};

   if ( *result_flag == 0 )
   {
      memcpy( amex_action_code, HOST_MESSAGE.responsecd, DE39_SIZE );
      //Store Action code so that we can show in transaction Lookup
      memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
         	  HOST_MESSAGE.responsecd, DE39_SIZE );
      ret_val = isalphanum(amex_action_code);

      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in response code (field 39) not allowed: %s,",
                  amex_action_code);
      }
      else if( !map_amex_action_code_to_iso_repsonse_code(amex_action_code) )
      {
         *result_flag = 1;
         ret_val = false;
         sprintf( err_buf,
           "Cannot convert between transfor amex action code %s to iso response code",
            amex_action_code );
      }
      else
      {
         /* This is used for Online Updates (1304/1314) */
         strcpy( Auth_Tx.accptor_bus_code, amex_action_code );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_41
 *
 *  DESCRIPTION:  This function will move the terminal ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Terminal ID is alphanumeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_41( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

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
 *                Message structure according to Equitable formatting
 *                specifications.  Merchant ID is numeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_42( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.merchant_id,
              HOST_MESSAGE.cardaccid, DE42_SIZE );
   }
   return( ret_val );
}







/******************************************************************************
 *
 *  NAME:         move_field_49
 *
 *  DESCRIPTION:  This function will move the currency code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Currency code is alphanumeric, fixed
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_49( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.currency_code,
              HOST_MESSAGE.tran_curr_cd, DE49_SIZE );
      ret_val = isalphanum(Auth_Tx.TLF01_details.currency_code);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in Currency Code (field 49) not allowed: %s,",
                  Auth_Tx.TLF01_details.currency_code );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_50
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_50( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.reconciliation_cur_code,
              HOST_MESSAGE.recn_curr_cd, DE50_SIZE );
      ret_val = isalphanum(Auth_Tx.reconciliation_cur_code);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in Reconciliation Currency Code (field 50) not allowed: %s,",
                  Auth_Tx.reconciliation_cur_code );
      }
   }
   return( ret_val );
}
/******************************************************************************/

/******************************************************************************/
INT move_field_55( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true,movelen=0,temp_int,tag_91_len=0;
   BYTE temp_string_len[5]={0},temp_emv_len[5]={0},temp_tag_len[3]={0};
   if ( *result_flag == 0 )
   {
      
      convert_to_ascii( &HOST_MESSAGE.IccData_len[0], temp_string_len, 3 );
      movelen = atoi(temp_string_len);
	  
	 /* if(movelen >0 && movelen < 80)
	  {
	    tag_91_len = movelen-6; // Header and version 4 + 2 
		  sprintf(temp_emv_len,"%04d",tag_91_len+1);
	    genutil_asc_to_bin(temp_emv_len,Auth_Tx.EMV_details.future_field1,&temp_int);
		  Auth_Tx.EMV_details.future_field1[2]= 0x91;
	   // TAG len includes First byte length and follwed by IAD daata 8 byte ARPC and 2 byte RC 
		  memcpy(&Auth_Tx.EMV_details.future_field1[3],&HOST_MESSAGE.IccData[6],tag_91_len);
	
	  } */

		if(movelen >0 && movelen < 80)
		{
			tag_91_len = movelen-6; /* Header and version 4 + 2 */

			if (tag_91_len == 37)
			{
				/*Tag 91 Data */
				/*In this case we are droping 19 which is present after Tag 91 Deta
				* so no need to increase the length even if we add the 91 Tag in the message*/
				sprintf(temp_emv_len,"%04d",tag_91_len);
				genutil_asc_to_bin(temp_emv_len,Auth_Tx.EMV_details.future_field1,&temp_int);

				Auth_Tx.EMV_details.future_field1[2]= 0x91;

				/*Copy Tag 91 Len and Data . Total Length of this data is 1+ 10 */
				memcpy(&Auth_Tx.EMV_details.future_field1[3],&HOST_MESSAGE.IccData[6],11);

				/* * Total Length of DE55 is 37 out of we which 11 byte data is for tag 91 .
				 * We need to avoid lenth for Tag 72*
				 *
				 * Sample DE 55 Data
				 * DE55_1110: F0F4F3C1C7D5E200010A7B6B5FE69F250BF330301972179F180400000000860E04DA9F580903E417C1AB8BFB9D9B
				 *
				 *F0F4F3
				 *C1C7D5E2
				 *0001
				 *Tag 91 DATA  -0A 7B6B5FE69F250BF33030 ( "Len  DATA" Tag 91 wont be present in
				 Transaction data it will only have data. we have hardcoded Tag91 in the above section
				 19 - This we need to avoid when we are responding to POS.
				 Tag 72 --72 17 9F180400000000860E04DA9F580903E417C1AB8BFB9D9B ("Tag Len Data" We need to
				 Respond to POS as it is. Total length of Tag 72 data is 25. */

				memcpy(&Auth_Tx.EMV_details.future_field1[14],&HOST_MESSAGE.IccData[18],
						(tag_91_len - 12)); /*1 byte 91 Tag, 1 byte 91 tag len, 10 byte 91 tag and 1 byte total tag 72 data*/



			}
			else
			{
				sprintf(temp_emv_len,"%04d",tag_91_len+1);
				genutil_asc_to_bin(temp_emv_len,Auth_Tx.EMV_details.future_field1,&temp_int);

				/*Tag 91 Data */
				Auth_Tx.EMV_details.future_field1[2]= 0x91;
				/* TAG len includes First byte length and follwed by IAD daata 8 byte ARPC and 2 byte RC */
				memcpy(&Auth_Tx.EMV_details.future_field1[3],&HOST_MESSAGE.IccData[6],tag_91_len);
			}
		}
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_56
 *
 *  DESCRIPTION:  This function will move the Original data elements the Host
 *                Message structure according to Equitable formatting
 *                specifications.  PIN Block is alphanumeric, fixed
 *                length.  This is an encrypted pin, could be padded
 *                with "F"s, left justified.
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_56( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  temporigdata[MAX_DE56_SIZE+1];  

   if ( *result_flag == 0 )
   {
	   if(HOST_MESSAGE.orig_data[0] != 0x00)
	   {
			memset (temporigdata, 0x00, sizeof(temporigdata));
			memcpy(temporigdata, HOST_MESSAGE.orig_data,  MAX_DE56_SIZE);
			memcpy(Auth_Tx.TLF01_details.orig_message, temporigdata, 4);
			memcpy(Auth_Tx.TLF01_details.orig_sys_trace_num, temporigdata+4, 6);
			memcpy(Auth_Tx.orig_local_time_date, temporigdata+10, 12);
			memcpy(Auth_Tx.orig_acq_inst_id, temporigdata+22, 13);
	   }

	}

   return( ret_val );
}

#if AMEX03B_APRIL2024
INT move_field_60( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT len_field60=0;
	INT     BitMap_Len = 8;
	BYTE    BitMap[8] ={0};
	INT     cps_bit = 0;
	INT     cps_byte = 0;
	CHAR bit60_data[356]={0};
	INT bit60_flag=0;
	CHAR SF1_len[4]={0};
	INT intsf1_len=0;
	INT bit_60len=0;
	CHAR sbit_60len[4]={0};
	INT i,k=0;


	for( i=0; i<3; i++ )
	{
		sbit_60len[i] = ascii_ebcdic_conv (HOST_MESSAGE.priv60_len[i], E2A);
	}
	len_field60= atoi(sbit_60len);
	if(len_field60 > 0)
	{
		   /*parse to get the bit60.7*/
		   for( i=0; i<5; i++)
		   {
			   bit60_data[i] = ascii_ebcdic_conv (HOST_MESSAGE.priv60[i], E2A);
		   }
		   if(0 == strncmp(bit60_data,BIT60_ADITIONAL_DATA_ID,5))
		   {
			   memset( BitMap, 0x00, sizeof(BitMap) );

			    for( k=0,i=5; k<BitMap_Len; i++ ,k++)
			    	BitMap[k] = HOST_MESSAGE.priv60[i];

			   for(i=9,k=5;i<len_field60;i++,k++)
			   {
				   bit60_data[k] = ascii_ebcdic_conv (HOST_MESSAGE.priv60[i], E2A);
			   }

			   bit_60len+=5;
			   for(cps_byte=1;cps_byte<=3;cps_byte++)
			   {
				   for(cps_bit=1; cps_bit<=8; cps_bit++)
				   {
					   if ( BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
					   {
						   switch(cps_byte)
						   {
						   	   case 1:

								   switch(cps_bit)
								   {
								   	   case 1:
								   		strncpy(SF1_len,&bit60_data[5],3);
								   		bit_60len+=3;
								   		bit_60len+=atoi(SF1_len);

								   		   break;
								   	   case 2:
								   		   bit_60len+=20;
								   		   break;
								   	   case 3:
								   		   bit_60len+=42;
										   break;
									   case 4:
										   bit_60len+=20;
										   break;
								   	   case 5:
								   		   bit_60len+=11;
										   break;
									   case 6:
										   bit_60len+=1;
										   break;
									   case 7:
										   bit60_flag=1;
										   strncpy(Auth_Tx.TLF01_details.visa_merchant_id,&bit60_data[bit_60len],15);
										   printf("bit 60 SF7");
									   break;
								   }

							  break;
						   }
					  }
					   if(bit60_flag == 1)
					   {
						   /*found the SF7 no need to parse other sub fields*/
						   break;

					   }
				   }
				   if(bit60_flag == 1)
				   {
					   /*found the SF7 no need to parse other sub fields*/
					   break;

				   }
			   }
		   }
		   /* copy only the 60.7 if present else not required*/

	   }

	return ret_val;
}
#endif
/******************************************************************************
 *
 *  NAME:         move_field_61
 *
 *  DESCRIPTION:  This function will move the Original data elements the Host
 *                Message structure according to Equitable formatting
 *                specifications.  PIN Block is alphanumeric, fixed
 *                length.  This is an encrypted pin, could be padded
 *                with "F"s, left justified.
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
INT move_field_61( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   
   return( ret_val );
}

