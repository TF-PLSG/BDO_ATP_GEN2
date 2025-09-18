/**************************************************************************************************
*  
* MODULE:      process_incoming_msg.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: Routines when a transactions enters the Device Handler 
*
* APPLICATION: dcpos
*
* AUTHOR:  Abhishek Kumar

      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "pteipc.h"
#include "dcpos_global_definitions.h"
#include "genutil.h"  // Includes all function prototypes for generic utilities
#include "dcpos_iso_utilities.h"
#include "txutils.h"
#include "ntutils.h"
#include "dcpos_process_incoming_msg.h"
#include "dcpos_process_outgoing_msg.h"
#include "dcpos_service_functions.h"
#include "dcpos_general_utilities.h"

/* COMM_BUF_SZE is defined to be 1000 in isoutil.h */
extern BYTE    ReqRsp_Buffer [COMM_BUF_SZE]; 
extern INT     buflen ;                  /* total size of ReqRsp_Buffer */
extern INT     reqidx ; 
extern INT     bHasTimeField ; 

extern CHAR    ServiceName[];
extern struct  iso_msg_rec ISO_MESSAGE;  /* defined in isoutil.c  */
extern AUTH_TX auth_tx;                  /* defined in threqust.c */


extern CHAR		visa1_que_name[] ;
extern CHAR   dcpiso_que_name[];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

extern CHAR response_code_for_DUAL_BRAND_DEBIT_TRANSACTION[];
extern CHAR response_code_for_DEBIT_MAGSTRIPE_TRANSACTION[];
extern CHAR response_code_for_DEBIT_EMV_TRANSACTION[];
extern CHAR response_code_for_DCC_TRANSACTION[];
extern CHAR response_code_for_DINERS_TRANSACTION[];
extern CHAR response_code_for_FLEET_CARD_TRANSACTION[];
extern CHAR response_code_for_CUP_UPI_MAGSTRIPE_TRANSACTION[];
extern CHAR response_code_for_BIN_NOT_FOUND_FOR_BIN_ROUTING[];
extern CHAR response_code_for_AMEX_TRANSACTION_ON_BIN_NII[];

extern CHAR DCC_Termina_Starting_Prefix_1[];
extern CHAR DCC_Termina_Starting_Prefix_2[];
extern CHAR DCC_Termina_Starting_Prefix_3[];
extern CHAR DCC_Termina_Starting_Prefix_4[];
extern CHAR DCC_Termina_Starting_Prefix_5[];
extern CHAR DCC_Termina_Starting_Prefix_6[];
extern CHAR DCC_Termina_Starting_Prefix_7[];
extern CHAR DCC_Termina_Starting_Prefix_8[];
extern CHAR DCC_Termina_Starting_Prefix_9[];
extern CHAR DCC_Termina_Starting_Prefix_10[];

BYTE Process_Incoming_Message_From_Terminal (pPTE_MSG p_msg_in)
{
   pPTE_MSG        p_msg_term = NULL_PTR ;

   CHAR            strError[512] = {0} ;
   LONG            xipcRetCode = 0L ;
   LONG            dataLength = 0L ;
   CHAR            strTemp[512] = {0} ;
   pBYTE           p_data = NULL_PTR ;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR ;
   pCHAR           p_rec = NULL_PTR ;
   BYTE            retCode = 0 ;
   BYTE            transactionType = 0 ;
   CHAR            tran_finish_time[17]={0};
   int   		   ReturnCode = 0 ;
   BYTE  AccountType_credit = 'C'  ;
   BYTE  AccountType_dedit = 'D'  ;

   memset (&auth_tx, 0, sizeof auth_tx) ;
   
   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);
   p_rec      = (pCHAR) p_data;

   /* Copy the data to ReqRsp_Buffer */
   dataLength = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   retCode    = Copy_Message_Data_To_Global_Buffer( p_rec, dataLength );

   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Check result from copy to global buffer. */
   if ( retCode != DEVICE_OK )
   {
      sprintf( strTemp,
              "%s received an abnormal message. Length %d. Dropping it.",
               ServiceName, dataLength );
      dcpos_log_message( 1, 3,strError,"Process_Incoming_Message_From_Terminal",0 );
      return( retCode );
   }

   /*Copy the entire terminal data and send it to host as it for MP cards*/
   if(buflen < COMM_BUF_SZE && buflen>0)
   {
		sprintf(auth_tx.req_data_len,"%04d", buflen);
		memcpy(auth_tx.req_buffer,ReqRsp_Buffer,buflen);
   }
	
   /* Parse the ISO8285 message and move all fields to ISO_MESSAGE (defined in isoutils.c) */
   if (isoutil_parse_terminal_request_message ((pCHAR) p_msg_in) == false)
   {
      dcpos_log_message( 1, 3,"Cannot parse terminal request message",
    		  	  	  	  	  "Process_Incoming_Message_From_Terminal",0 );
      /* Cannot set response_code as auth_tx has not been populated at this time */					
      return (ISO_PARSE_ERROR );
   }

   /* Copy fields from the global variable ISO_MESSAGE to global variable auth_tx */
   retCode = Copy_Message_Fields_From_Global_Structure_To_Auth_Tx () ; 

   if (retCode != DEVICE_OK)
   {
		dcpos_log_message( 2, 3,"Global to Auth data move failed","Process_Incoming_Message_From_Terminal",1 );
		return (retCode) ; /* On error, returns ISO_PARSE_ERROR */

   }

   transactionType = Get_Transaction_Type_From_Global_Structure () ;

   Populate_Sections_Of_Auth_Tx (transactionType, p_msg_in) ;

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, &auth_tx, NA );

   /* We need to send 0220, 0320 , 0400 and 0500 message to dcpios without even bin checking.*/

   if( (0 == strncmp(auth_tx.TLF01_details.message_type,"0320",ISO_MSG_TYPE_LEN))||
	   (0 == strncmp(auth_tx.TLF01_details.message_type,"0400",ISO_MSG_TYPE_LEN))||
	   (0 == strncmp(auth_tx.TLF01_details.message_type,"0420",ISO_MSG_TYPE_LEN))||
	   (0 == strncmp(auth_tx.TLF01_details.message_type,"0500",ISO_MSG_TYPE_LEN)))
	   {
	   	   sprintf( strTemp,
	                 "Sent to dcpiso based on Message type %s",auth_tx.TLF01_details.message_type);
	   	   dcpos_log_message( 2, 1,strTemp,"Process_Incoming_Message_From_Terminal",1 );
	   	   retCode = Send_Request_To_Service ( dcpiso_que_name ,
	   											MT_INCOMING_DCPOS,
	   											0,
	   											0,
	   											0,
	   											0,
	   											0) ;
	   }

   /* For Amex transaction (0220) coming on BIN NII we have to response right back to pos*/
    else if((true == dcpos_check_If_AMEX_Transaction()))
    {
    	retCode = Process_AMEX_TRANSACTION_ON_BIN_NII_Flow();
    }
    else
    {
	   /* We have to now find the bin details*/
	   /* Lets 1st check debit bin*/
	   retCode= Retrieve_BIN01_Record_Dcpos(AccountType_dedit);
	   if (retCode == RECORD_FOUND_IN_SHARED_MEMORY)
	   {
		   retCode = Process_When_Debit_Bin_Range_Found_Dcpos () ;
	   }
	   else if (retCode == RECORD_FOUND_IN_SHARED_MEMORY_CREDIT)
	   {
		   retCode = Process_When_Credit_Bin_Range_Found_Dcpos () ;
	   }
	   else
	   {
		   /* Lets see if we can find in DB */
	   }
   }
   
   return (retCode) ;

}/* Process_Incoming_Message_From_Terminal */


BYTE Copy_Message_Data_To_Global_Buffer (pCHAR indata, LONG msglen)
{
   BYTE  retval = DEVICE_OK;

   memset( ReqRsp_Buffer, 0x00, COMM_BUF_SZE );
   if ( (0 < msglen) && (msglen < COMM_BUF_SZE) )
   {
      buflen = msglen;
      reqidx = 0; 
      memcpy( ReqRsp_Buffer, indata, msglen );
   }
   else
   {
      retval = ISO_PARSE_ERROR;
      if ( msglen >= COMM_BUF_SZE )
      {
         /* Fill the ReqRsp_Buffer with as much of the message as
          * we can so it can be stored in the trace file. End the
          * buffer with three dots (...) so we know it is too large.
          */
         buflen = COMM_BUF_SZE - 1;
         memcpy( ReqRsp_Buffer, indata, buflen - 3 );
         memset( ReqRsp_Buffer+buflen-3, '.', 3 );
      }
      else
      {
         if ( msglen == 0 )
            strcpy( ReqRsp_Buffer, "Zero length message" );

         else /* msglen is negative */
            strcpy( ReqRsp_Buffer, "Negative length message" );

         /* This variable is used when logging the message to a trace file. */
         buflen = strlen( ReqRsp_Buffer );
      }
   }
   return( retval );
} /* Copy_Message_Data_To_Global_Buffer */
/******************************************************************************
 *
 *  NAME:         VALIDATE_DATE_YYYYMMDD
 *
 *  DESCRIPTION:  This function verifies a date to be valid.  The date must
 *                be in format YYYYMMDD.  The following checks are performed:
 *                   1.  Must be numeric
 *                   2.  Year must be in the range of 2000 to 2200
 *                   3.  Month must be from 1 to 12
 *                   4.  Day cannot be more than the maximum for the month,
 *                       leap year is taken into account for February.
 *
 *  INPUTS:       date_str - Date in YYYYMMDD format
 *
 *  OUTPUTS:      err_buf - Explanation of what is wrong with the date
 *
 *  RTRN VALUE:   True if valid date, false if invalid date
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  imonth, iday, iyear;
   CHAR temp_month[3]="";
   CHAR temp_day[3]  ="";
   CHAR temp_year[5] ="";

   /* Validate date is numeric. */
   if ( (true == isnum(date_str)) && (8 == strlen(date_str)) )
   {
      /* Validate month */
      memcpy( temp_month, date_str+4, 2 );
      imonth = atoi(temp_month);
      if ( (1 <= imonth) && (imonth <= 12) )
      {
         /* Month is valid; check the day. */
         memcpy( temp_day, date_str+6, 2 );
         iday = atoi(temp_day);
         if ( (1 <= iday) && (iday <= 31) )
         {
            if ( iday == 31 )
            {
               if ( (imonth == 1) || (imonth == 3) || (imonth == 5) ||
                    (imonth == 7) || (imonth == 8) || (imonth == 10)||
                    (imonth == 12) )
               {
                  ret_val = true;
               }
            }
            else if ( (iday == 30) && (imonth != 2) )
            {
               ret_val = true;
            }
            else if ( (iday == 29) && (imonth == 2) )
            {
               /* This must be a leap year to be valid. */
               memcpy( temp_year, date_str, 4 );
               iyear = atoi(temp_year);
               if ( (iyear%4) == 0 )
                  ret_val = true;
               else
                  sprintf( err_buf, "Too many days for February (%s)",date_str);
            }
            else
               ret_val = true;

            /* Validate the year. */
            memcpy( temp_year, date_str, 4 );
            iyear = atoi(temp_year);
            if ( (iyear < 2000) || (iyear > 2200) )
            {
               ret_val = false;
               sprintf( err_buf, "ERROR: Invalid date (%s). Bad Year",date_str);
            }
         }
         else
         {
            sprintf( err_buf, "ERROR: Invalid date (%s). Bad Day.", date_str );
         }
      }
      else
      {
         sprintf( err_buf, "ERROR: Invalid date (%s). Bad Month.", date_str );
      }
   }
   else
   {
      sprintf( err_buf, "ERROR: Invalid date (%s).", date_str );
   }
   if ( true != ret_val )
      strcat(  err_buf, " Must be in format YYYYMMDD." );

   return( ret_val );
}

/*****************************************************************************

  Function:   Copy_Message_Fields_From_Global_Structure_To_Auth_Tx

  Description: Convert ISO8583 Fields from their binary fields (ISO_MESSAGE) to 
               their ascii fields (auth_tx).   

  Author: Abhishek Verma
          
  Inputs: None
  
  Outputs: None
  
  Return values: None
******************************************************************************/
BYTE Copy_Message_Fields_From_Global_Structure_To_Auth_Tx (void)
{
	CHAR  datestr1 [21] = {0} ;
	CHAR  datestr2 [21] = {0} ;
	CHAR  temp_amt [25] = {0} ;
	CHAR  temp[17] = {0} ;
	CHAR  tempstr [101] = {0} ;  
	CHAR  templen [6] = {0} ;	 
   CHAR  unique_tran_str_id[20] = "";
   CHAR  temp_track1_card_num[24];
	pCHAR tempptr = NULL_PTR ;
	BYTE  strTranDate[100] = {0} ;
   INT   i,j;
   INT   len;
   INT   temp_len;
	CHAR  tranTime[50] = {0} ;
	CHAR  err_buf[100]="";
   pBYTE stan_ptr = NULL_PTR;
	
   CHAR  ICCdata [256] = {0};
   
	INT   tempint = sizeof (auth_tx.TLF01_details.card_num)-1;

	
	genutil_bcd_to_asc (ISO_MESSAGE.proccode, sizeof(ISO_MESSAGE.proccode),
						&tempstr[0], sizeof tempstr) ;

	memcpy (auth_tx.TLF01_details.processing_code, tempstr, sizeof(auth_tx.TLF01_details.processing_code));

	genutil_bcd_to_asc (ISO_MESSAGE.msgtype_bcd, sizeof(ISO_MESSAGE.msgtype_bcd),
						&tempstr[0], sizeof(tempstr));
	memcpy(auth_tx.TLF01_details.message_type, tempstr, sizeof(auth_tx.TLF01_details.message_type));

	if (ISO_MESSAGE.date_bin[0] != 0x00)
	{
		genutil_bcd_to_asc	(ISO_MESSAGE.date_bin, 
							sizeof ISO_MESSAGE.date_bin,
							&datestr1[0], 
							sizeof datestr1) ;
		if (strlen (datestr1) == 4) 
		{	
			Generate_Transaction_Date_In_YYYYMMDD_format (strTranDate, datestr1) ;
			memcpy (auth_tx.TLF01_details.date_yyyymmdd, 
				    strTranDate, 
					sizeof (auth_tx.TLF01_details.date_yyyymmdd)-1) ;
		}
		else if (validate_date_yyyymmdd(datestr1,err_buf))
		{	memcpy (auth_tx.TLF01_details.date_yyyymmdd, 
			       datestr1, 
				   sizeof (auth_tx.TLF01_details.date_yyyymmdd)-1) ;
		}else
		{	
			genutil_format_date (temp);
			memcpy(auth_tx.TLF01_details.date_yyyymmdd, temp, sizeof(auth_tx.TLF01_details.date_yyyymmdd)-1);
		}
	}
	else /* the ISO message does not have a date */
	{
		genutil_format_date (temp);
		memcpy(auth_tx.TLF01_details.date_yyyymmdd, temp, sizeof(auth_tx.TLF01_details.date_yyyymmdd)-1);
	}

			

	if (bHasTimeField == true)
	{
		genutil_bcd_to_asc (ISO_MESSAGE.time_bin, sizeof(ISO_MESSAGE.time_bin),
				  &datestr2[0], sizeof(datestr2));
		memcpy (auth_tx.TLF01_details.time_hhmmss, datestr2, sizeof(auth_tx.TLF01_details.time_hhmmss)-1);
	}
	else
	{
		PopulateAuthTxTimeField () ;
	}
   
   /* Get system trace audit number (stan). */
   memset( tempstr, 0x00, sizeof(tempstr) );
   genutil_bcd_to_asc (ISO_MESSAGE.stan_bin, sizeof(ISO_MESSAGE.stan_bin),  
                       &tempstr[0],          sizeof(tempstr));

   /* Stan might be all zeroes if a Voice Auth from EFT. */
   if ( 0 == strcmp( tempstr, "000000" ) )
   {
      /* It is all zeroes.  Need to create one. */
      strcpy( auth_tx.TLF01_details.orig_sys_trace_num, "000000" );
      ptetime_get_strtime( unique_tran_str_id );
      stan_ptr = strchr( unique_tran_str_id, '.' );
      if ( stan_ptr == NULL )
      {
         /* There is no decimal. */
         temp_len = strlen( unique_tran_str_id );
         memcpy( auth_tx.TLF01_details.sys_trace_audit_num,
                 &unique_tran_str_id[temp_len-6],
                 6 );
      }
      else
      {
         /* stan_ptr is pointing to a decimal.  Use 6 digits before it. */
         memcpy( auth_tx.TLF01_details.sys_trace_audit_num, stan_ptr-6, 6 );
      }
   }
   else
   {
      memcpy( auth_tx.TLF01_details.sys_trace_audit_num,
              tempstr,
              sizeof(auth_tx.TLF01_details.sys_trace_audit_num)-1);               
   }

	genutil_bcd_to_asc (ISO_MESSAGE.amount_bin, sizeof(ISO_MESSAGE.amount_bin), 
			   &tempstr[0], sizeof(tempstr));

	memcpy(auth_tx.TLF01_details.total_amount, tempstr, sizeof(auth_tx.TLF01_details.total_amount)-1);

   /* Can get Merchant Category Code (field 18) from EFT transactions. */
   if ( ISO_MESSAGE.mcc[0] != 0x00 )
      memcpy( auth_tx.TLF01_details.category_code, ISO_MESSAGE.mcc, 4 );

	genutil_bcd_to_asc (ISO_MESSAGE.posentry, sizeof(ISO_MESSAGE.posentry),  
			   &tempstr[0], sizeof(tempstr));
	memcpy(auth_tx.TLF01_details.pos_entry_mode, tempstr, sizeof(auth_tx.TLF01_details.pos_entry_mode)-1);

   /* Field 23: Card Sequence Number (EMV Txns) */
   if ( ReqRsp_Buffer[4] & 0x02 )
   {
      /* DE23 is present in the bitmap.  Since its value can be all
       * zeroes, we must check it this way.  Because if not present,
       * and we still convert bcd to ascii, the ascii value will be
       * all zeroes.  The Network Controller will not know if it is
       * present or not.
       */
      genutil_bcd_to_asc( ISO_MESSAGE.panseqnum, sizeof(ISO_MESSAGE.panseqnum),
                         &tempstr[0], sizeof(tempstr) );
      memcpy( auth_tx.host2_message_seq_nbr, tempstr,
              sizeof(auth_tx.host2_message_seq_nbr)-1);
   }

	genutil_bcd_to_asc (ISO_MESSAGE.nii, sizeof(ISO_MESSAGE.nii), 
			   &tempstr[0], sizeof(tempstr));
	memcpy(auth_tx.TLF01_details.nii, tempstr, sizeof(auth_tx.TLF01_details.nii)-1);

	genutil_bcd_to_asc (ISO_MESSAGE.pos_condcd, sizeof(ISO_MESSAGE.pos_condcd),  
			   &tempstr[0], sizeof(tempstr));
	memset(Eftpos_pos_condition_code,0x00,sizeof(Eftpos_pos_condition_code));  //Girija GB
	memcpy(auth_tx.TLF01_details.pos_condition_code, tempstr, sizeof(auth_tx.TLF01_details.pos_condition_code)-1);
    strcpy(Eftpos_pos_condition_code,auth_tx.TLF01_details.pos_condition_code);//Girija GB

   /* Some e-commerce transactions have POS Entry Mode 0120 instead of 0012 */
   if ( (0 == strcmp(auth_tx.TLF01_details.pos_condition_code,"59")) &&
        (0 == strcmp(auth_tx.TLF01_details.pos_entry_mode, "0120" ))  )
   {
      strcpy( auth_tx.TLF01_details.pos_entry_mode, "0012" );
   }

  
	if (ISO_MESSAGE.pan_len[0] != 0x00) 
	{
		genutil_bcd_to_asc (ISO_MESSAGE.pan_len, sizeof(ISO_MESSAGE.pan_len),  
				  &tempstr[0], sizeof(tempstr));
		memcpy (auth_tx.TLF01_details.card_num_len, tempstr, sizeof(auth_tx.TLF01_details.card_num_len)-1);
		genutil_bcd_to_asc (ISO_MESSAGE.pan_bin, (atoi(auth_tx.TLF01_details.card_num_len)+1)/2,  
				  &tempstr[0], sizeof(tempstr));
		tempint = sizeof(auth_tx.TLF01_details.card_num) - 1;
		if(tempint > atoi(auth_tx.TLF01_details.card_num_len))
			tempint = atoi(auth_tx.TLF01_details.card_num_len);
		memcpy(auth_tx.TLF01_details.card_num, tempstr, tempint);
	} 

	if (ISO_MESSAGE.date_expd[1] != 0x00)
	{
		genutil_bcd_to_asc (ISO_MESSAGE.date_expd, sizeof(ISO_MESSAGE.date_expd),
				  &tempstr[0], sizeof(tempstr));
		memcpy(auth_tx.TLF01_details.exp_date, tempstr, sizeof(auth_tx.TLF01_details.exp_date)-1);
	}
	  
	if (ISO_MESSAGE.track2_len[0] != 0x00)   
	{
		genutil_bcd_to_asc (ISO_MESSAGE.track2_len, sizeof(ISO_MESSAGE.track2_len),  
				  &templen[0], sizeof(templen));

		genutil_bcd_to_asc (ISO_MESSAGE.track2, (atoi(templen)+1)/2,&tempstr[0], sizeof(tempstr));   

		if (atoi(templen) <= sizeof(auth_tx.TLF01_details.track2)-1)
			memcpy(auth_tx.TLF01_details.track2, tempstr, atoi(templen));
		else
			memcpy(auth_tx.TLF01_details.track2, tempstr, sizeof(auth_tx.TLF01_details.track2)-1);
		memset(Eftpos_service_code,0x00,sizeof(Eftpos_service_code));//Girija GB

		if ((tempptr = strchr(tempstr, '=')) != 0x00)
		{
			memcpy(auth_tx.TLF01_details.exp_date, tempptr+1, sizeof(auth_tx.TLF01_details.exp_date)-1);
			
			memcpy(auth_tx.TLF01_details.service_code, tempptr+5, sizeof(auth_tx.TLF01_details.service_code)-1); 
			strcpy(Eftpos_service_code,auth_tx.TLF01_details.service_code);//Girija GB
			*tempptr = 0x00;
			memcpy(auth_tx.TLF01_details.card_num, tempstr, sizeof(auth_tx.TLF01_details.card_num)-1);
			itoa(strlen(tempstr), auth_tx.TLF01_details.card_num_len, 10);
		}
	}

   /* Track 1 */
   if (ISO_MESSAGE.track1_len[0] != 0x00)     
   {
      genutil_bcd_to_asc( ISO_MESSAGE.track1_len, sizeof(ISO_MESSAGE.track1_len),
                          &templen[0], sizeof(templen));

      memcpy( auth_tx.TLF01_details.track1, ISO_MESSAGE.track1, atoi(templen));

      /* Find 1st field separator; marks end of card number. */
      if ((tempptr = strchr(auth_tx.TLF01_details.track1, '^')) != 0x00)
      {
         len = ( ((int)(tempptr)) - ((int)(auth_tx.TLF01_details.track1)) ) - 1;

         memset( temp_track1_card_num, 0x00, sizeof(temp_track1_card_num) );
         memcpy( temp_track1_card_num,
                 auth_tx.TLF01_details.track1 + 1,
                 len );

         /* Eliminate any spaces (Amex Track1 Card Num has spaces) */
         if ( NULL == strchr(temp_track1_card_num, ' ') )
         {
            /* No spaces */
            memcpy( auth_tx.TLF01_details.card_num,
                    temp_track1_card_num,
                    len );
         }
         else
         {
            /* Found spaces. Remove them for field 2, card num. */
            for( i=0,j=0; i<len; i++ )
            {
               if ( temp_track1_card_num[i] != ' ' )
               {
                  auth_tx.TLF01_details.card_num[j++] = temp_track1_card_num[i];
               }
            }
            len = j;
         }

         itoa( len, auth_tx.TLF01_details.card_num_len, 10 );

         /* Find last field separator; marks start of expiry date. */
         if ((tempptr = strrchr(auth_tx.TLF01_details.track1, '^')) != 0x00)
         {
            memcpy( auth_tx.TLF01_details.exp_date,
                    tempptr+1,
                    sizeof(auth_tx.TLF01_details.exp_date)-1);
            memcpy( auth_tx.TLF01_details.service_code,
                    tempptr+5,
                    sizeof(auth_tx.TLF01_details.service_code)-1);
         }
      }
      tempptr = NULL_PTR;
   }

    memcpy (auth_tx.TLF01_details.orig_retrieval_ref_num, ISO_MESSAGE.rrn, 12) ;
   	memcpy (auth_tx.TLF01_details.auth_number, ISO_MESSAGE.authcode, 6) ;
   	memcpy (auth_tx.TLF01_details.terminal_id, ISO_MESSAGE.termid, 8) ;
   	memcpy (auth_tx.TLF01_details.merchant_id, ISO_MESSAGE.cardaccid, 15) ;
   	memcpy (auth_tx.TLF01_details.visa_merchant_id, ISO_MESSAGE.cardaccid, 15) ;

   /* CVV2 / CVC2 / 4CSC */
   isoutil_parse_adddata_48();

   /* Pin Block can have nulls in it.  So check whole field for data. */
   for ( i=0; i<8; i++ )
   {
      if ( ISO_MESSAGE.pin_block[i] != 0x00 )
      {
         genutil_bin_to_ascii (ISO_MESSAGE.pin_block, sizeof(ISO_MESSAGE.pin_block),
                               &tempstr[0], sizeof(tempstr));
         memcpy( auth_tx.TLF01_details.pin_block, tempstr,
                 sizeof(auth_tx.TLF01_details.pin_block)-1);
         break;
      }
   }

   /* Field 53: Security Related Control Information = Amex 4DBC (not 4CSC) */
   if ( ISO_MESSAGE.sec_ctl[0] != ' ' && ISO_MESSAGE.sec_ctl[1] != ' ')
   {
      /* Format : nnnn000000000000. We need only first 4 characters */
      memset( tempstr, 0x00, sizeof(tempstr) );
      genutil_bcd_to_asc( ISO_MESSAGE.sec_ctl, sizeof(ISO_MESSAGE.sec_ctl),
                         &tempstr[0], sizeof tempstr );

      memcpy( auth_tx.TLF01_details.interest_rate, tempstr, 4 );
   }


	if ((ISO_MESSAGE.addl_len[0] != 0x00) || (ISO_MESSAGE.addl_len[1] != 0x00))
	{
		genutil_bcd_to_asc (ISO_MESSAGE.addl_len, sizeof(ISO_MESSAGE.addl_len),   
				  &templen[0], sizeof(templen));
		memcpy(temp_amt, ISO_MESSAGE.addlamts, atoi(templen));
		sprintf(auth_tx.TLF01_details.add_amounts,"%024d", atol(temp_amt));
		sprintf(auth_tx.TLF01_details.tip_amount,"%012d", atol(temp_amt));
	}


   /* Field 55 EMV Chip data */
   temp_len = genutil_bcd_to_int(ISO_MESSAGE.ICCdata_len, 2);		
   if ( temp_len > 0)
   {
      memset(ICCdata, 0, sizeof(ICCdata));
      memcpy(ICCdata, ISO_MESSAGE.ICCdata, temp_len);

      /* Copy for future emv master card host  along with length*/
      memcpy( auth_tx.EMV_details.emv_block,
              ISO_MESSAGE.ICCdata_len,
              temp_len + sizeof(ISO_MESSAGE.ICCdata_len) );

      genutil_EMV_To_Auth_Tx( ICCdata, temp_len, &auth_tx );
   }

	if ((ISO_MESSAGE.priv60_len[0] != 0x00) || (ISO_MESSAGE.priv60_len[1] != 0x00))
		isoutil_priv60_cvt_ascii() ;

	if ((ISO_MESSAGE.priv61_len[0] != 0x00) || (ISO_MESSAGE.priv61_len[1] != 0x00))
		isoutil_priv61_cvt_ascii() ;

	if ((ISO_MESSAGE.priv62_len[0] != 0x00) || (ISO_MESSAGE.priv62_len[1] != 0x00))
		isoutil_priv62_cvt_ascii() ;

	if ((ISO_MESSAGE.priv63_len[0] != 0x00) || (ISO_MESSAGE.priv63_len[1] != 0x00))
	{
		if (isoutil_priv63_cvt_ascii() == false)
		{
			dcpos_log_message( 1, 3,"Error while parsing private field 63",
			    		  	  	  	 "Copy_Message_Fields_From_Global_Structure_To_Auth_Tx",0 );
			return ISO_PARSE_ERROR ;
		}
	}

   return DEVICE_OK ;
} /* Copy_Message_Fields_From_Global_Buffer_To_Auth_Tx */




BYTE Generate_Transaction_Date_In_YYYYMMDD_format (BYTE* strTranDate, 
												   BYTE* datestr1)
{
	BYTE strTransactionDate[100] = {0} ;
	BYTE strSystemYYYYMMDD[100] = {0} ;
	BYTE strTransactionYYYYMMDD[100] = {0} ;
	int  SystemYYYYMMDD = 0 ;
	int  TransactionYYYYMMDD = 0 ;

	/* get system date in format yyyymmddminutesseconds ...*/
	genutil_format_date (strTranDate) ; 

	/* get system yyyymmdd into strYYYYmmdd */
	memcpy (strSystemYYYYMMDD, strTranDate, 8) ; 

	/* replace the system mmdd with transaction mmdd */
	memcpy (strTranDate+4, datestr1, 4) ; 

	/* get transaction yyyymmdd */
	memcpy (strTransactionYYYYMMDD, strTranDate, 8) ; 

	SystemYYYYMMDD = atoi (strSystemYYYYMMDD) ;
	TransactionYYYYMMDD = atoi (strTransactionYYYYMMDD) ;

	if (SystemYYYYMMDD != 0 && TransactionYYYYMMDD != 0)
	{
		/* Since the ISO MESSAGE sends only a date in MMDD format
		   and the year is obtained from the ATP server, we have
		   to ensure that if the terminal sends us 1231 and the server
		   year is 2001, we generate the date 20001231 and not 20011231
		*/

		if (TransactionYYYYMMDD > SystemYYYYMMDD)
		{
			TransactionYYYYMMDD -= 1; /*Assuming only one day diffrence, If more than a day diffrence date will be turn to year older */
			if(TransactionYYYYMMDD > SystemYYYYMMDD)
			{	if((TransactionYYYYMMDD - SystemYYYYMMDD) >= 30)
				TransactionYYYYMMDD -= 10000 ;/*If diff in tx date and system date is more than 30, Mark as last year tx to get rejected*/
				else if((TransactionYYYYMMDD - SystemYYYYMMDD) >= 2)
				TransactionYYYYMMDD -= 10000 ;
				else
				TransactionYYYYMMDD = SystemYYYYMMDD ;	
			}
			//TransactionYYYYMMDD -= 10000 ;
		}
			 
		itoa (TransactionYYYYMMDD, strTranDate, 10) ;
	}

	return DEVICE_OK ;
} /* Generate_Transaction_Date_In_YYYYMMDD_format */

/*****************************************************************************

  Function:  Get_Transaction_Type_From_Global_Structure

  Description: DEVICE_OK function gets the transaction type from the request message.
  Inputs: None
  Outputs: None
  Return values: BYTE - contains the transaction type
  Author: Abhishek Verma
******************************************************************************/
BYTE Get_Transaction_Type_From_Global_Structure (void)
{  
  /* Check if the transaction is a financial request */
	if ((ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
	{
		/* it is, now verify the procesing code to know what kind */
		switch(ISO_MESSAGE.proccode[0])
		{
			case 0x00:
            return AUTH_SALE;
			break;
			case 0x20:
				return AUTH_REFUND ;
			break;
			case 0x02:
				return AUTH_VOID_SALE ;    
			break;
			case 0x21:
				return AUTH_RELOAD ;  
			break;
			case 0x22:
				return AUTH_VOID_REFUND ;  
			break;
			case 0x01:
				return AUTH_CASH_ADVANCE ;
			break;
			case 0x04:
				return AUTH_CHECK_VERIFICATION ;
			break;
			case 0x09:
				return AUTH_SALE_CASH ;
			break;
			case 0x11:
            return AUTH_QUASI_CASH ;
            break;
			default:
				return AUTH_INVALID;
			break;
		} /*  end switch */
	} /* end if */
  	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x01) )
	{
		/* it is, now verify the procesing code to know what kind */
		switch(ISO_MESSAGE.proccode[0])
		{
			case 0x21:
				return AUTH_RELOAD_REPEAT ;  
			break;
			default:
				return AUTH_INVALID;
			break;
		} /*  end switch */
	} /* end if */
	/* or  an advise request */   
  	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x02) )
	{
		/* it is, now verify the procesing code to know what kind */
		switch(ISO_MESSAGE.proccode[0])
		{
			case 0x21:
				return AUTH_RELOAD_CONFIRM ;  
			break;
			default:
				return AUTH_INVALID;
			break;
		} /*  end switch */
	} /* end if */
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x20) )
	{
		switch(ISO_MESSAGE.proccode[0])
		{
			case 0x00:

				if (atoi(auth_tx.TLF01_details.total_amount) == 0)
					return AUTH_OFFLINE_VOID_SALE ; /* Void of a OFFLINE SALE */
				
				return AUTH_OFFLINE_SALE ;
			break;
			case 0x20:
				
				if (atoi(auth_tx.TLF01_details.total_amount) == 0)
					return AUTH_OFFLINE_VOID_REFUND ; /* Void of a OFFLINE REFUND */
				
				return AUTH_OFFLINE_REFUND ;
			break;
			case 0x02:
				if (atoi(auth_tx.TLF01_details.total_amount) == 0)
					return AUTH_OFFLINE_VOID_SALE ; /* Void of an OFFLINE SALE */

				return AUTH_SALE_ADJUSTMENT ;
			break;
			case 0x22:
				if (atoi(auth_tx.TLF01_details.total_amount) == 0)
					return AUTH_OFFLINE_VOID_REFUND ;

				return AUTH_REFUND_ADJUSTMENT ;
			break;
			case 0x42:
				return AUTH_PAYMENT_ADJUSTMENT ;
			break;

			case 0x90:
				return AUTH_GUARANTEED_LATE_ARRIVAL ;
			break;
			default:
				return AUTH_INVALID ;
			break;
		} /* end switch */
	} /* end if */

	/* or an Authorization request */  
	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x01) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
	{
		switch (ISO_MESSAGE.proccode[0])
		{  
		   case 0x00:
			 return AUTH_AUTHORIZATION ;
			  break;

		  case 0x30:
			 return AUTH_PRE_AUTHORIZATION ;
			  break;

		   case 0x31:
			 return AUTH_BALANCE_INQUIRY ;
			  break;

		  case 0x38:
			 return AUTH_CARD_VERIFICATION ;
			  break;

		  case 0x04:
			 return AUTH_CHECK_VERIFICATION ;
			  break;

		  case 0x02:
			 return AUTH_VOID_PRE_AUTHORIZATION ;
			  break;

         /* Look for these processing codes because EFT Master is
          * sending financial requests to ATP as 0100 transactions.
          */
         case 0x01:
            return AUTH_CASH_ADVANCE ;
            break;
         case 0x09:
            return AUTH_SALE_CASH ;
            break;
         case 0x11:
            return AUTH_QUASI_CASH ;
            break;

		  default:
			 return AUTH_INVALID ;
			  break;
		} 
	}

	/* or a Reversal request */  
	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x04) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
	   return AUTH_REVERSAL ;
		

   /* or a batch upload */ 
   else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x20) )
   {
      if ( ISO_MESSAGE.proccode[0] == 90 )
         return AUTH_SIGNATURE_DATA;
      else if ( ISO_MESSAGE.proccode[0] == 0x94 )
         return AUTH_EMV_UPLOAD;
      return AUTH_BATCH_UPLOAD ;
   }

	/* or a EMV ADVICE */ 
	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x40) )
	{
		return AUTH_EMV_ADVICE ;
	}
	/* or a settlement transaction */
	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x05) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
	{
		switch (ISO_MESSAGE.proccode[0])
		{
			case 0x92:
				return AUTH_SETTLE ;
			break;

			case 0x96:
				return AUTH_SETTLE_TRAILER ;
			break;

			case 0x95:
				return AUTH_BATCH_DOWN_LINE_LOAD ;
			break;

			default:
				return AUTH_INVALID ;
			break;
		}
	}
  
	/* or a statistics transaction */
	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x08) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00))
	{
		switch (ISO_MESSAGE.proccode[0])
		{
			case 0x90:
				return AUTH_PIN_CHANGE ;
			break;
			case 0x91:
				return AUTH_STATISTICS ;
			break;
			case 0x92:
				return AUTH_LOGON ;
			break;
			case 0x93:
				return AUTH_INITIALIZATION ;
			break;
			case 0x99:
				return AUTH_TEST ;
			break;
			default:
				return AUTH_INVALID ;
			break;
		}
	}
  
	/* or a Please Wait Advice */  
	else if ((ISO_MESSAGE.msgtype_bcd [0] == 0x08) && (ISO_MESSAGE.msgtype_bcd[1] == 0x30) )
	   return AUTH_PLEASE_WAIT_ADVICE ;
		
	/* or a transaction that the financial server does not understand */
	else
		return AUTH_INVALID ;
} /* Get_Transaction_Type_From_Global_Structure */

BYTE Populate_Sections_Of_Auth_Tx (BYTE tx_type, pPTE_MSG p_msg_in)
{
   CHAR  unique_tran_str_id[50] = {0} ;
   CHAR  temp [17] = {0} ;
   BYTE  accountType = 0 ;
   BYTE  retrievalRefNum [50] = {0} ;
   CHAR  tran_start_time[50] = {0} ;
   CHAR   date_str [ 9] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT     tid_len = 0;
   INT     unique_tran_str_id_len=0;

   ptetime_get_strtime (tran_start_time);

   if ( auth_tx.TLF01_details.deferred_term_length[0] != 0x00 )
   {
      if ( (tx_type == AUTH_BATCH_UPLOAD) ||
           (tx_type == AUTH_EMV_UPLOAD  ) ||
           (tx_type == AUTH_VOID_SALE   ) ||
           (tx_type == AUTH_REVERSAL    )  )
         auth_tx.TLF01_details.tx_key = tx_type;
	  // Check its MP transaction or deferred purchange transaction
      else if(0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))
	  {
	  		// Its an MP transaction 
         	auth_tx.TLF01_details.tx_key = tx_type ;
	  }
	   else
		 	auth_tx.TLF01_details.tx_key = AUTH_DEFERRED_PURCHASE_AUTHORIZATION ;
   }
   else
      auth_tx.TLF01_details.tx_key = tx_type ;

   if (tx_type == AUTH_CARD_VERIFICATION)
   {
      /* Treat card verification transaction as Authorization */
      auth_tx.TLF01_details.tx_key = AUTH_AUTHORIZATION;

      /* Save the original transaction key in mac_key, so that we
       * can find it when it is going back.
       */
      auth_tx.mac_key[0] = tx_type;

      /* Change the processing code */
      strncpy(auth_tx.TLF01_details.processing_code,"000000",6);
   }

   ptetime_get_strtime (unique_tran_str_id) ;

   memset (auth_tx.TLF01_details.primary_key.transaction_id, 0, 
           sizeof (auth_tx.TLF01_details.primary_key.transaction_id)) ;
   tid_len= strlen(auth_tx.TLF01_details.terminal_id);
   if(tid_len == 8)
   {
   		memcpy (auth_tx.TLF01_details.primary_key.transaction_id, 
        	   auth_tx.TLF01_details.terminal_id, sizeof(auth_tx.TLF01_details.terminal_id) - 1) ;

   		strncat (auth_tx.TLF01_details.primary_key.transaction_id, unique_tran_str_id+2,12) ;
   }
   else
   {
   	   /* We have to now prepare 20 digit unique Transaction ID.
   	   	 unique_tran_str_id will be having 14 digit length
   	   	 now we will make it as 20 by adding YYMMDD in the beginning of it.
   	   	 20 byte will look like
   	   	 YYMMDD"unique_tran_str_id"
   	   	 for example if unique_tran_str_id is "1444752896.334" then it will become
   	   	 YYMMDD1444752896.334
		 Based on ther terminal length we will copy data from new unique_tran_str_id
	   */
	   strcpy(temp_unique_tran,unique_tran_str_id);
	   memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));
	   ptetime_get_timestamp( time_date );
       get_date( time_date, date_str );
	   strncpy(unique_tran_str_id, date_str+2, 6); //YYMMDD
	   strcat (unique_tran_str_id,temp_unique_tran);
	   
   	   unique_tran_str_id_len= strlen(unique_tran_str_id);
	   memcpy(auth_tx.TLF01_details.primary_key.transaction_id,
			   auth_tx.TLF01_details.terminal_id, tid_len );
	   strncat( auth_tx.TLF01_details.primary_key.transaction_id,
			    unique_tran_str_id+tid_len, 
			    (unique_tran_str_id_len-tid_len) );

   }

   strcpy (auth_tx.TLF01_details.originator_queue, ptemsg_get_msg_orig_queue (p_msg_in)) ;
   strcpy (auth_tx.TLF01_details.originator_info, ptemsg_get_msg_orig_comms_info (p_msg_in)) ;

   auth_tx.TLF01_details.general_status = GS_TXN_NEW ;
   auth_tx.TLF01_details.entry_type = FINANCIAL ;

   strcpy (auth_tx.TLF01_details.tran_start_time, tran_start_time) ;
   strcpy (auth_tx.TLF01_details.handler_queue, application_que_name) ; 
   strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ; 

   if (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL)
   {
      strcpy(auth_tx.TLF01_details.actual_amount, "0") ;
      strcpy(auth_tx.TLF01_details.reversal_amount, auth_tx.TLF01_details.total_amount) ;
   }

   if ( (auth_tx.TLF01_details.track2[0] != 0x00 )          ||
        (auth_tx.TLF01_details.track2[0] != 0x00 )          ||
        (auth_tx.TLF01_details.verify_mag_stripe[0] != 'N')  )
   {
      strcpy (auth_tx.TLF01_details.verify_mag_stripe, "Y") ;
   }
   strcpy (auth_tx.TLF01_details.dispensation.origin,"LO") ;

   /* Recall that if the transaction had a rrn when it came in, it is in orig_retrieval_ref_num */
   /* if the incoming transaction already has a rrn (ie auth_tx.TLF01_orig_retrieval_ref_num != NULL), */
   /* then mirror the same rrn in the response (auth_tx.TLF01.retrieval_ref_num)*/
   /* if the incoming transaction does not have a rrn (ie auth_tx.TLF01_orig_retrieval_ref_num == NULL)*/
   /* create a new rrn and copy it in the response (auth_tx.TLF01.retrieval_ref_num)*/ 

   /* Create a retrieval reference number for the transaction */

   if (strlen (auth_tx.TLF01_details.orig_retrieval_ref_num) == 0)
   {
      create_rrn (retrievalRefNum) ;
      memcpy (auth_tx.TLF01_details.retrieval_ref_num, 
              retrievalRefNum, 
              sizeof auth_tx.TLF01_details.retrieval_ref_num);
   }
   else
   {
      /* retain the original rrn in the response (auth_tx.TLF01.retrieval_ref_num) */
      strcpy (auth_tx.TLF01_details.retrieval_ref_num, auth_tx.TLF01_details.orig_retrieval_ref_num) ;
   }

   /* Some terminals send in an authorization number when they shouldn't.
    * So far, it has always been spaces only.
    */
   if ( auth_tx.TLF01_details.auth_number[0] != 0x00 )
   {
      if ( (auth_tx.TLF01_details.tx_key == AUTH_SALE           ) ||
           (auth_tx.TLF01_details.tx_key == AUTH_REFUND         ) ||
           (auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION  ) ||
           (auth_tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)  )
      {
         memset( auth_tx.TLF01_details.auth_number, 0x00,
                 sizeof(auth_tx.TLF01_details.auth_number) );
      }
   }

   return DEVICE_OK ;

} /* Populate_Sections_Of_Auth_Tx */


BYTE Process_Terminal_Statistics_Transaction (BYTE t_auth_key)
{
	strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
	auth_tx.TLF01_details.general_status = GS_TXN_COMPLETED_OFF ;
	
	auth_tx.TLF01_details.entry_type = ADMINISTRATIVE ;

	return TERMINAL_STATISTICS_SUCCESSFUL ;
} /* Process_Terminal_Statistics_Transaction */

BYTE Process_When_Debit_Bin_Range_Found_Dcpos (void)
{
	BYTE retCode;

	if((true == Is_EMV_Transaction()))
	{
		retCode = Process_Debit_EMV_Transaction_Flow();

	}
	else
	{
		retCode = Process_Debit_Magstripe_Transaction_Flow();
	}
	return retCode;

}

BYTE Process_When_Credit_Bin_Range_Found_Dcpos(void)
{
	BYTE retCode;
	INT  nCard_family_type = 0;
	BYTE strCard_family_type[3] = {0};
	CHAR buffer[256] = {0};

	strncpy(strCard_family_type,auth_tx.BIN01_details.card_family,2);
	nCard_family_type=atoi(strCard_family_type);

	switch(nCard_family_type)
	{
		case CARD_CUP:
			if((true == Is_EMV_Transaction()))
			{
				/*Its CUP EMV transaction , forward to dcpiso*/
				// We have to send to dcpiso
				dcpos_log_message( 2, 1,"CUP EMV transaction sent to dcpiso",
										  "Process_When_Credit_Bin_Range_Found_Dcpos",1 );
				retCode = Send_Request_To_Service ( dcpiso_que_name ,
													MT_INCOMING_DCPOS,
													0,
													0,
													0,
													0,
													0) ;

			}
			else
			{
				retCode = Process_CUP_Magstripe_Transaction_Flow();
			}
			break;

		case CARD_DCC:

			/* Let check Terminal ID
			* if terminal ID starts with "60" then its DCC terminal.
			*/
			if( true == dcpos_Is_DCC_terminal())
			{
				retCode = Process_DCC_Sale_Transaction_Flow();
			}
			else
			{
				/* Credit Bin is found We will send it to dcpiso*/
				// We have to send to dcpiso
				dcpos_log_message( 2, 1,"DCC transaction failed, routing to dcpsio",
										  "Process_When_Credit_Bin_Range_Found_Dcpos",1 );
				retCode = Send_Request_To_Service ( dcpiso_que_name ,
													MT_INCOMING_DCPOS,
													0,
													0,
													0,
													0,
													0) ;
			}
			break;

		case CARD_DINERS:
			retCode = Process_Dinner_Transaction_Flow();
			break;

		case CARD_DUAL:
			retCode = Process_Dual_Branded_Debit_Transaction_Flow();
			break;

		case CARD_FLEET:
			retCode = Process_Fleet_Transaction_Flow();
			break;

		default:
			/* Credit Bin is found We will send it to dcpiso*/
			dcpos_log_message( 2, 1,"Transaction fallen into default case, routing to dcpsio",
									  "Process_When_Credit_Bin_Range_Found_Dcpos",1 );
			retCode = Send_Request_To_Service ( dcpiso_que_name ,
												MT_INCOMING_DCPOS,
												0,
												0,
												0,
												0,
												0) ;
			break;
	}
		return (retCode);
}


BYTE  Process_Dual_Branded_Debit_Transaction_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_DUAL_BRAND_DEBIT_TRANSACTION,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			DUAL_BRAND_DEBIT_TRANSACTION_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}


BYTE  Process_Debit_Magstripe_Transaction_Flow(void)
{
	BYTE retCode;
	if(strncmp(auth_tx.BIN01_details.card_family,"07",2) == 0)
	{
		Process_Dual_Branded_Debit_Transaction_Flow();
	}
	else
	{
		strncpy(auth_tx.TLF01_details.response_code,
				response_code_for_DEBIT_MAGSTRIPE_TRANSACTION,
				RESPONSE_CODE_LEN);
		strcpy(auth_tx.TLF01_details.response_text,
				DEBIT_MAGSTRIPE_TRANSACTION_RESPONSE_TEXT);
		retCode= Send_Transaction_To_Originator();
	}


	return DEVICE_OK;
}


BYTE  Process_Debit_EMV_Transaction_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_DEBIT_EMV_TRANSACTION,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			DEBIT_EMV_TRANSACTION_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	dcpos_log_message( 2, 1,"Responsed back to Pos ","Process_Debit_EMV_Transaction_Flow",1 );
	return DEVICE_OK;
}


BYTE  Process_DCC_Sale_Transaction_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_DCC_TRANSACTION,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			DCC_TRANSACTION_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}


BYTE  Process_Dinner_Transaction_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_DINERS_TRANSACTION,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			DINERS_TRANSACTION_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}


BYTE  Process_Fleet_Transaction_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_FLEET_CARD_TRANSACTION,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			FLEET_CARD_TRANSACTION_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}


BYTE  Process_CUP_Magstripe_Transaction_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_CUP_UPI_MAGSTRIPE_TRANSACTION,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			CUP_UPI_MAGSTRIPE_TRANSACTION_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}

BYTE  Process_AMEX_TRANSACTION_ON_BIN_NII_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_AMEX_TRANSACTION_ON_BIN_NII,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			AMEX_TRANSACTION_ON_BIN_NII_RESPONSE_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}

BYTE  Process_No_Bin_Found_Flow(void)
{
	BYTE retCode;
	strncpy(auth_tx.TLF01_details.response_code,
			response_code_for_BIN_NOT_FOUND_FOR_BIN_ROUTING,
			RESPONSE_CODE_LEN);
	strcpy(auth_tx.TLF01_details.response_text,
			NO_BIN_FOUND_RESPONSE_ERROR_TEXT);
	retCode= Send_Transaction_To_Originator();
	return DEVICE_OK;
}


BYTE dcpos_Is_DCC_terminal (void)
{
	if((0==strncmp(auth_tx.TLF01_details.terminal_id,
				   DCC_Termina_Starting_Prefix_1,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_2,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_3,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_4,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_5,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_6,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_7,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_8,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_9,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN))||
	   (0==strncmp(auth_tx.TLF01_details.terminal_id,
			   	   DCC_Termina_Starting_Prefix_10,
				   DCC_TERMINAL_STSRTING_PREFIX_LEN)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
