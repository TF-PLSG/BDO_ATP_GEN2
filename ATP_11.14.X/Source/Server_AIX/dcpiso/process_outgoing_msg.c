/**************************************************************************************************
*  
* MODULE:      process_outgoing_msg.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: Routines when a transaction is returned to the terminal  
*
* APPLICATION: DCPISO
*
* AUTHOR:  Sanjoy Dasgupta
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basictyp.h"
#include "pteipc.h"
#include "global_definitions.h"  
#include "genutil.h"  
#include "iso_utilities.h"
#include "txutils.h"
#include "ntutils.h"
#include "process_incoming_msg.h"
#include "process_outgoing_msg.h"
#include "service_functions.h"
#include "database_request.h"
#include "general_utilities.h"

extern BYTE		ReqRsp_Buffer [COMM_BUF_SZE]; 
extern INT		buflen;                  /* total size of ReqRsp_Buffer */
extern INT		reqidx; 
extern INT      Nmra_send_flag;

extern struct iso_msg_rec ISO_MESSAGE ; 
extern AUTH_TX auth_tx ;				

extern CHAR		authorizer_que_name[] ;
extern CHAR     updateds_que_name[] ;
extern CHAR     mrads_que_name[] ;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;
extern INT nMra_enable_flag;
extern DCF01 dcf01;

extern char mc_timeout_rc[3];
extern char visa_timeout_rc[3];
extern char amex_timeout_rc[3];
extern char jcb_timeout_rc [3];
extern char cup_timeout_rc [3];
extern char dci_timeout_rc [3];


BYTE Process_Response_From_Authorizer (pPTE_MSG p_msg_in)
{
   pBYTE          pCurrent   = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           retCode = 0 ;
   BYTE		  temp_nfi_name[11]={0};
   BYTE		  strError[256]={0};

   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   pCurrent   = ptemsg_get_pte_msg_data_data (p_msg_data);

   memset (&auth_tx, 0, sizeof auth_tx) ;
   memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

   /*We need to store 0220 and partial reversal in TLF01_STIP table */
   if((0 == strncmp(auth_tx.TLF01_details.message_type,"0220",4)) &&
	  (0 == strncmp(auth_tx.TLF01_details.processing_code,"000000",6))&&
	  (0 == strncmp(auth_tx.CAF01_details.authorization_location,"S",1))&&
	  ((0 == strncmp(auth_tx.TLF01_details.acquirer_id,"ncmcr2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"ncvis2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"ncjcb2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"ncdci2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"nposa2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"nccup2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"ncmcr2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"ncvis2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"ncjcb2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"ncdci2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"nposa2",6))||
	   (0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"nccup2",6))))
   {
	   strncpy(auth_tx.TLF01_details.product_codes[1].code,"stndin",6);
   }

   /* All MC, VISA, JCB & DCI partial reversal response should not be sent to POS,
    * AMEX already inserted into TLF01 */
   if (0 == strncmp(auth_tx.TLF01_details.product_codes[3].quantity,"PR",2))
   {
	   if(0 == strncmp(auth_tx.TLF01_details.response_code,"00",2))
	   {
		   Insert_TLF01_Details( ST2_NONE );
		   /* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed */
		   Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		   return( DEVICE_OK );
	   }
	   else
	   {
		   dcpiso_log_message( 1, 3, "Partial Reversal declined","Process_Response_From_Authorizer",0 );
		   Insert_TLF01_Details( ST2_NONE );
		   /* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed */
		   Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		   return( DEVICE_OK );
	   }
   }

   /* Drop timed-out reversals. Reversal response has already been sent
    * to the terminal.  If it timed out, no need to call updatds.
    */
   if ( auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE )
   {
      if ( CREDIT_BIN == Get_Bin_Type(auth_tx.TLF01_details.processing_code) )
      {
         if ( 0 == strcmp(auth_tx.TLF01_details.response_code,REENTER_TRANSACTION))
            return( DEVICE_OK );
      }
   }

	/* Response: Change Authorization back to card verification.
    * Need to do this before updating the database.
    */
   if(auth_tx.mac_key[0] == AUTH_CARD_VERIFICATION)
   {
      auth_tx.TLF01_details.tx_key = AUTH_CARD_VERIFICATION_RESPONSE;
      memcpy( auth_tx.TLF01_details.processing_code, AUTH_CARD_VERIFICATION_RESPONSE_PROC_CODE, PROCESSING_CODE_LEN );
   }

   /* Response: Change Authorization back to Pre Auth .
       * Need to do this before updating the database.
       */
   else if(auth_tx.mac_key[0] == AUTH_PRE_AUTHORIZATION)
   {
	   if ( (true ==  dcpiso_check_If_AMEX_Transaction()))
  	   {
  		   auth_tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_RESPONSE;
  		   memcpy( auth_tx.TLF01_details.processing_code, AMEX_PREAUTH_VERIFICATION_RESPONSE_PROC_CODE, PROCESSING_CODE_LEN );
  	   }
   }

   if ( If_Transaction_Cannot_Be_Declined() )
   {
      /* In the authorizer, pin verification is not done for ADVICES */

      /* We still need to call Update_All_Data () for declined ADVICE
       * transactions as we have to put them in the batch tables
       */
      strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;	
   }
   else
   {
      retCode = Perform_Auth_Tx_Checks () ;
      if (retCode != DEVICE_OK)
         return retCode ;
   }

   /* Ensure that txcntl has changed the tx_key to a RESPONSE type */
   retCode = Check_Transaction_Tx_Key () ;
   if (retCode != DEVICE_OK)
      return retCode ;

   /* SCR 22830 - ThoughtFocus
   /* Map response codes for Hypercom terminals. */

   /*
   (void)Map_Response_Code( auth_tx.TLF01_details.response_code );
   */

   /* SCR 22830 - ThoughtFocus
    * Checking if the request is from a terminal.
    * If true, the response_code is mapped for the terminals
    * else, they are sent as it's. 
    * Currently only e-commerce txns don't get mapped (pos_condition_code = 59),
    * every other transactions get mapped.
    */

	if (IsOriginatorATerminal(auth_tx.TLF01_details.pos_condition_code))
	{
		if(true == dcpiso_Check_If_CUP_Transaction())
		{
			/* Send response code coming as it is from CUP
			Check if it is 99 then send a moniter message to do sign in .*/
			if(0 == strncmp(auth_tx.TLF01_details.response_code,"99",2))
			{
				strcpy(strError,"Error in PIN format , Please re-sign on with CUP/UPI HOST");
				dcpiso_log_message( 1, 3, strError,"Process_Response_From_Authorizer",0 );
			}
		}
		else
		{
			(void)Map_Response_Code( auth_tx.TLF01_details.response_code );
		}
	}



   /* If the transaction's response code is TRANSACTION_APPROVED (00),
    * INCORRECT_PIN (55), PIN_TRIES_EXCEEDED (75), then Update_All_Data ()
    * needs to be called.
    */
   retCode = Check_If_Transaction_Has_Valid_Response_Code () ;
   
   if (retCode != DEVICE_OK)
   {
      memset( auth_tx.TLF01_details.auth_number, 0x00,
              sizeof(auth_tx.TLF01_details.auth_number) );
   }
   else if ( true == txn_is_financial() )
   {
      /* Send request to updateds to update all necessary tables 
       * (BCH10, BCH01, BCH20, ACF01, CCF02P, CCF03, etc).
       */
	   /*
	   For CUP , during strip transaction , we need to make sure that filed 15 (settlement date) have a valid value
	   */
	   if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
	   {
		   if (auth_tx.TLF01_details.saf[0] == INCREMENTAL_AUTH)
		   {
			   strcpy(auth_tx.TLF01_details.response_text, "incremental authorization");
		   }

		   if(true == dcpiso_Check_If_CUP_Transaction())
		   {
				if(0 == strlen(auth_tx.TLF01_details.settlement_date))
				{
					strncpy(auth_tx.TLF01_details.settlement_date,auth_tx.TLF01_details.date_yyyymmdd +4,4);
				}			   
		   }
		}
      retCode = Update_All_Data () ; 
   }
   else
   {
      /* No need to send to updatds, so respond to the terminal. */
      Send_Transaction_To_Originator();
      retCode = DEVICE_OK ;
   }

   if ( auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE )
   {
      /* The purpose of this block of code is for the case when
       * txcntl responds to dcpiso with a declined void. Dcpiso
       * has already responded to the terminal with a void response.
       * If the void is now declined (somehow it is happening),
       * then update_all_data (above) is not called, and dcpiso
       * ends up responding to the terminal again. We can avoid
       * this second response by returning DEVICE_OK. So we are
       * dropping the void here.
       */
      retCode = DEVICE_OK ;
   }
   return retCode ;

} /* Process_Response_From_Authorizer */

BYTE Send_Transaction_To_Originator (void) 
{
	BYTE MatchingResponseKey = 0 ;
	BYTE retCode = 0 ;
	BYTE		  temp_nfi_name[11]={0};

   /* Response: Change Authorization back to card verification */
   if(auth_tx.mac_key[0] == AUTH_CARD_VERIFICATION)
   {
      auth_tx.TLF01_details.tx_key = AUTH_CARD_VERIFICATION;
      memcpy( auth_tx.TLF01_details.processing_code, "380000", 6 );
   }

	MatchingResponseKey = Match_Request_To_Response_Transaction_Key () ;
	
	auth_tx.TLF01_details.tx_key = MatchingResponseKey ;

	retCode = Populate_Response_Fields_In_Auth_Tx () ;

	/* Populate the iso_msg_rec structure with data in auth_tx */
	retCode = Populate_ISO_Structure_From_Auth_Tx () ;
	
	if((true == dcpiso_check_If_AMEX_Transaction()) &&
		   (auth_tx.TLF01_details.saf[0] == PARTIAL_REVERSAL)&&
		   (0 == strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)))
	{
	  /*Its Amex Partial Reversal in ATP STIP case, no need to send response back to POS*/
	}
	else
	{
	/* Build the response message for the terminal in ReqRsp_Buffer */
	isoutil_build_terminal_response (auth_tx.TLF01_details.tx_key) ;

	/* Send the message to the terminal */
	retCode = Send_Response_To_Terminal () ;
	}

	  /**Insert in to MRADS */

	if((auth_tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
		auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE ||
		auth_tx.TLF01_details.tx_key == AUTH_SALE ||
		auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION					  ||
		auth_tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE||
		auth_tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE )  &&
		nMra_enable_flag == 1 )
	{	  /*temp_nfi_name used to store the nfi name tempareroly*/
	   strncpy(temp_nfi_name,auth_tx.TLF01_details.nfi_name,sizeof(auth_tx.TLF01_details.nfi_name));
	   memcpy(auth_tx.TLF01_details.nfi_name,auth_tx.MCF01_details.negative_group_id,sizeof(auth_tx.MCF01_details.negative_group_id));
	   if(Nmra_send_flag == 1)
	   {
		   retCode = Insert_MRA_AUTH_Details( ST2_NONE ) ;
	   }
	   else if(Nmra_send_flag == 0)
	   {
		   dcpiso_Send_Request_To_Mracon_Service();
	   }
	   strncpy(auth_tx.TLF01_details.nfi_name,temp_nfi_name,sizeof(auth_tx.TLF01_details.nfi_name));
	}

	/* Insert the TLF01 record - drop the transaction */
	retCode = Insert_TLF01_Details( ST2_NONE ) ; /* Ignore any SYSTEM ERROR */

	/* We need to check if tranasction is qualifiny for MC 512 i.e Partial reversal*/

	if (true == dcpiso_check_If_Tra_is_Offline_Sale() &&
		0    == strncmp(auth_tx.TLF01_details.response_code,"00",2))
	{
		if(true == dcpiso_check_If_Tra_qualify_MC_512_incremental_Preauth())
		{
			dcpiso_check_If_Tra_qualify_MC_512_handaling();
		}
		else
		{
			return (DEVICE_OK) ;
		}
	}

	return (DEVICE_OK) ;
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
   CHAR resp_code[3]={0};

	len = strlen (auth_tx.TLF01_details.track1) ;
	if (len !=0)
		itoa (len, track1_length, 10) ;
  
	len = strlen(auth_tx.TLF01_details.track2) ;
	if (len !=0)
		itoa(len, track2_length, 10) ;
  

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

   if ( (auth_tx.TLF01_details.tx_key == AUTH_RELOAD_RESPONSE) ||
        (auth_tx.TLF01_details.tx_key == AUTH_RELOAD_CONFIRM_RESPONSE) )
   {
      if ( 0 == strcmp(auth_tx.TLF01_details.response_code,"00") )
      {
         /* Copy balance info from nceqit for terminal. */
         strcpy( auth_tx.TLF01_details.total_amount,
                 auth_tx.TLF01_details.orig_amount );
      }
   }

   if (auth_tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION ||
	   auth_tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE   )
   {
		   if (true == dcpiso_check_If_MC_Transaction()   ||
			   true == dcpiso_check_If_VISA_Transaction())
		   {
			   strcpy( auth_tx.TLF01_details.total_amount,
			                    auth_tx.TLF01_details.total_interest );
		   }
   }

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


    if (genutil_blank_string(track2_length, sizeof(track2_length)) == false)
	{
		genutil_asc_to_bcd (track2_length, 
							sizeof(ISO_MESSAGE.track2_len)*2, 
							ISO_MESSAGE.track2_len) ;
		genutil_asc_to_vbcd (auth_tx.TLF01_details.track2, 
							sizeof(ISO_MESSAGE.track2)*2, 
							ISO_MESSAGE.track2) ;
	}
    
	memcpy (&ISO_MESSAGE.rrn[0],
			auth_tx.TLF01_details.retrieval_ref_num, 
			sizeof(ISO_MESSAGE.rrn)) ;

	if (0 == strncmp(auth_tx.TLF01_details.response_code,
					 SYSTEM_MALFUNCTION,ISO_RESPONSE_CODE_LEN))
	{
		// No need to copy Auth ID when there is system malfunctioning.
	}
	else
	{
		memcpy (&ISO_MESSAGE.authcode[0],
				auth_tx.TLF01_details.auth_number,
				sizeof(ISO_MESSAGE.authcode)) ;
	}

	/*
	 * Use configured value only for timeout scenarios. Use acquirer_id to identify if the response code
	 * is from timeout or from a duplicate transaction( ATPV119-15))
	 */
	if(strncmp(auth_tx.TLF01_details.response_code,"19",2)==0 && auth_tx.TLF01_details.acquirer_id[0]!='\0')
	{
		dcpiso_update_TO_rc_basedon_card_brand(&resp_code);
		if(resp_code[0] != '\0')
		{
			memcpy (&ISO_MESSAGE.responsecd[0],	resp_code,sizeof(ISO_MESSAGE.responsecd)) ;
		}
		else
		{
			memcpy (&ISO_MESSAGE.responsecd[0],
							auth_tx.TLF01_details.response_code,
							sizeof(ISO_MESSAGE.responsecd)) ;
		}
	}
	else
	{
		//For a duplicate transaction
		memcpy (&ISO_MESSAGE.responsecd[0],
			auth_tx.TLF01_details.response_code, 
			sizeof(ISO_MESSAGE.responsecd)) ;
	}


	memcpy (&ISO_MESSAGE.termid[0], 
			auth_tx.TLF01_details.terminal_id, 
			sizeof(ISO_MESSAGE.termid)) ;

	memcpy (&ISO_MESSAGE.cardaccid[0], 
			auth_tx.TLF01_details.merchant_id, 
			sizeof(ISO_MESSAGE.cardaccid)) ;

	if (genutil_blank_string(track1_length, sizeof(track1_length)) == false)
	{
		memcpy (&ISO_MESSAGE.track1_len[0], 
				track1_length, 
				sizeof(ISO_MESSAGE.track1_len)) ;
		memcpy (&ISO_MESSAGE.track1[0], 
				auth_tx.TLF01_details.track1, 
				sizeof(ISO_MESSAGE.track1)) ;
	}
	
   /* Build emv field 55 response if present */
   if ( (auth_tx.EMV_details.emv_block[0] != 0x00) ||
        (auth_tx.EMV_details.emv_block[1] != 0x00)  )
   {
      /* EMV Data from a Network Controller that doesn't parse the EMV block. */

      /* Some network controllers put the response info into the
       * future fields because it is too large for emv_block.
       */
      if ( (auth_tx.EMV_details.future_field1[0] == 0x00) &&
           (auth_tx.EMV_details.future_field1[1] == 0x00)  )
      {
		  //Don't send DE55 if ISSUER has not send any thing in repsonse
        if(strcmp(auth_tx.BIN01_details.card_family, CARD_MASTERCARD) == 0 || strcmp(auth_tx.BIN01_details.card_type ,"MCRD") == 0 )
		{
			//For MC Don't send DE55 if ISSUER has not send any thing in repsonse, req by BDO 3rd OCT
		}
		else
		{ memcpy( ISO_MESSAGE.ICCdata_len, auth_tx.EMV_details.emv_block, 2 );
          len = genutil_bcd_to_int( ISO_MESSAGE.ICCdata_len, 2);

          memcpy( &ISO_MESSAGE.ICCdata, auth_tx.EMV_details.emv_block+2, len );
		}
      }
      else
      {
         /* EMV response info is in the future fields. */
         len = genutil_bcd_to_int( auth_tx.EMV_details.future_field1, 2 );
         if ( len <= 255 )
         {
            memcpy(ISO_MESSAGE.ICCdata_len,auth_tx.EMV_details.future_field1,2);
         }
         else
         {
            len = 255;
            strcpy( temp_str, "0255" );
            genutil_str_asc_to_bcd( temp_str, 4, ISO_MESSAGE.ICCdata_len );			
         }
         memcpy( ISO_MESSAGE.ICCdata,
                 auth_tx.EMV_details.future_field1+2,
                 sizeof(auth_tx.EMV_details.future_field1)-3 );

         if ( len > (sizeof(auth_tx.EMV_details.future_field1)-3) )
         {
            /* Append the EMV data from future field 2. */
            memcpy( ISO_MESSAGE.ICCdata
                    + sizeof(auth_tx.EMV_details.future_field1)-3,
                     auth_tx.EMV_details.future_field2,
                     sizeof(auth_tx.EMV_details.future_field2)-1 );

            if ( len > ((sizeof(auth_tx.EMV_details.future_field1)-3)
                       +(sizeof(auth_tx.EMV_details.future_field2)-1)) )
            {
               /* Append the EMV data from future field 2. */
               memcpy( ISO_MESSAGE.ICCdata
                       + sizeof(auth_tx.EMV_details.future_field1)-3
                       + sizeof(auth_tx.EMV_details.future_field2)-1,
                        auth_tx.EMV_details.future_field3,
                        sizeof(auth_tx.EMV_details.future_field3)-1 );
            }
         }
      }
   }

   if (genutil_blank_string(auth_tx.TLF01_details.pin_block,
                            sizeof(auth_tx.TLF01_details.pin_block)) == false)
   {
      genutil_hex_to_bin (auth_tx.TLF01_details.pin_block, 
                          sizeof(ISO_MESSAGE.pin_block)*2, 
                          ISO_MESSAGE.pin_block) ;
      memcpy (&ISO_MESSAGE.pin_block[0], 
              auth_tx.TLF01_details.pin_block, 
              sizeof(ISO_MESSAGE.pin_block));  
   }

   if (auth_tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION ||
	   auth_tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE   )
   {
	   retCode = Populate_ISO_Private62_Field () ;
   }

   retCode = Populate_ISO_Private63_Field () ;
   return retCode ;
} /* Populate_ISO_Structure_From_Auth_Tx */

BYTE Populate_ISO_Private63_Field (void)
{
   INT     resp_text_flag;
   CHAR    tempstr[61];
   LONG    amount;
   double  def_gross_amt;
   double  amort_amt;
   BOOLEAN auto_init_enabled = false;
   BOOLEAN approved;

   INT field63_len = 0;
   CHAR response_text1[41] = "GB      ACCEPTED                        " ;
   CHAR response_text2[41] = "QD      DUPLICATE   BATCH               " ;
   CHAR response_text3[41] = "RB      DIFFERENT COUNTS OR AMOUNTS     " ;


   memset( &ISO_MESSAGE.priv63, 0x00, sizeof(ISO_MESSAGE.priv63) );

   if ( 0 == strcmp(auth_tx.TLF01_details.response_code,TRANSACTION_APPROVED) )
      approved = true;
   else
      approved = false;

   if (AUTH_SETTLE_RESPONSE         == auth_tx.TLF01_details.tx_key ||
       AUTH_SETTLE_TRAILER_RESPONSE == auth_tx.TLF01_details.tx_key  )
   {
      /*  Check if we need to send the auto init information */
      if (dcf01.auto_init_activation[0]          == '1' ||
          dcf01.auto_program_load_activation[0]  == '1' || 
          dcf01.auto_icepac_activation[0]        == '1'  )
      {
         auto_init_enabled = true;
      }
		
      /* Send the auto init parameters in table id A3 for
       * settlement trailer and response code 00
       */
      if ((approved) && (auto_init_enabled == true))
      {
         /* 2 byte length for the table  A3*/
         field63_len = 2;

         strncpy(&ISO_MESSAGE.priv63[field63_len], "A3",2);
         field63_len += 2;

         /* Copy this fields from dcf01 record assumption: no conversion needed*/
         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_init_activation, 1);
         ++field63_len;

         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_init_time, 4);
         field63_len += 4;

         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_init_ref_number, 6);
         field63_len += 6;

         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_program_load_activation, 1);
         ++field63_len;

         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_program_load_time, 4);
         field63_len += 4;

         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_program_load_ref_number, 6);
         field63_len += 6;

         strncpy(&ISO_MESSAGE.priv63[field63_len], dcf01.auto_icepac_activation, 1);
         ++field63_len;

         sprintf( tempstr, "%04d", field63_len );
         genutil_str_asc_to_bcd(tempstr, 4, ISO_MESSAGE.priv63);	
      }

      /* Copy the length*/
      sprintf(tempstr, "%04d", field63_len + 64 );
      genutil_str_asc_to_bcd(tempstr, 4, ISO_MESSAGE.priv63_len);			

      if (GS_TXN_GOOD_BATCH == auth_tx.TLF01_details.general_status )
      {
         strncpy(response_text1 + 2, auth_tx.TLF01_details.batch_number + 1, 5) ;
         strncpy(&ISO_MESSAGE.priv63[field63_len], response_text1, 40) ;
      }
      if (GS_TXN_DUPLICATE_BATCH == auth_tx.TLF01_details.general_status )
      {
         strncpy(response_text2 + 2, auth_tx.TLF01_details.batch_number + 1, 5) ;
         strncpy(&ISO_MESSAGE.priv63[field63_len], response_text2, 40);
      }
      if (GS_TXN_REJECTED_BATCH == auth_tx.TLF01_details.general_status )
      {
         strncpy(response_text3 + 2, auth_tx.TLF01_details.batch_number + 1, 5) ;
         strncpy(&ISO_MESSAGE.priv63[field63_len], response_text3, 40);
      }
   }
   else
   {
      if ( approved )
      {
         field63_len = 0;

         if(auth_tx.TLF01_details.product_codes[7].quantity[0] == 'C' ||
            auth_tx.TLF01_details.product_codes[7].quantity[0] == 'R')
         {
        	 /*  Do not build DE63 for P3 transactions*/
#if 0
             /* Total length of Table 71 */
             ISO_MESSAGE.priv63[field63_len+0] = 0x00;
             ISO_MESSAGE.priv63[field63_len+1] = 0x22;

             /* Table ID */
             memcpy(ISO_MESSAGE.priv63+field63_len+2, "71", 2);

             memcpy(ISO_MESSAGE.priv63+field63_len+4, auth_tx.TLF01_details.visa_tran_id, 15);
             memcpy(ISO_MESSAGE.priv63+field63_len+4+15, "     ", 5);
             field63_len += 24;

             /* Total length of Field 63 */
             ISO_MESSAGE.priv63_len[0] = 0x00;
             (void)genutil_binary_to_bcd( (LONG)field63_len,&ISO_MESSAGE.priv63_len[1] );
#endif
         }
         else if ( 0 == strcmp(auth_tx.TLF01_details.type_of_data,RSB_CARD) )
         {
            /* This is an RSB card. Copy the Deferred Gross amount
             * and the Monthly Amortization amount into DE63 so as
             * to display on the device.
             */
            def_gross_amt = strtod(auth_tx.TLF01_details.def_gross_amt,0)/100.0;
            amort_amt = strtod(auth_tx.TLF01_details.monthly_amort_amt,0)/100.0;

            memset(  tempstr, 0x00, sizeof(tempstr) );
            sprintf( tempstr, "GROSS AMT %9.2f SEMI-MONTHLY %7.2f",
                     def_gross_amt, amort_amt );

            memcpy( ISO_MESSAGE.priv63, tempstr, 40 );
            ISO_MESSAGE.priv63_len[0] = 0x00;
            ISO_MESSAGE.priv63_len[1] = 0x40;
         }
         else if ( auth_tx.TLF01_details.deferred_term_length[0] != 0x00 )
         {
            if ( (auth_tx.TLF01_details.tx_key != AUTH_BATCH_UPLOAD_RESPONSE) &&
                 (auth_tx.TLF01_details.tx_key != AUTH_VOID_SALE_RESPONSE   ) &&
                 (auth_tx.TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE    )  )
            {
               /* Total length of Table 58 */
               ISO_MESSAGE.priv63[field63_len+0] = 0x00;
               ISO_MESSAGE.priv63[field63_len+1] = 0x26;

               /* Table ID */
               memcpy(ISO_MESSAGE.priv63+field63_len+2, "58", 2);

               /* Table Data */

               /* Monthly Amortization Amount */
               amount = atol(auth_tx.TLF01_details.monthly_amort_amt);
               sprintf( tempstr, "%012ld", amount );
               memcpy(ISO_MESSAGE.priv63+field63_len+4, tempstr, 12);

               /* Deferred Gross Amount */
               amount = atol(auth_tx.TLF01_details.def_gross_amt);
               sprintf( tempstr, "%012ld", amount );
               memcpy(ISO_MESSAGE.priv63+field63_len+16, tempstr, 12);

               field63_len += 28;

               /* Total length of Field 63 */
               ISO_MESSAGE.priv63_len[0] = 0x00;
               (void)genutil_binary_to_bcd( (LONG)field63_len,
                                            &ISO_MESSAGE.priv63_len[1] );
            }
         }

         /* Cash Bonus and other response texts. */
         /* Check if it is not RPS, F22=08, F25=012 */
         if( 0 != strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2) &&
			 0 != strncmp(auth_tx.TLF01_details.pos_entry_mode,"0012",4))
         {
        	if (auth_tx.TLF01_details.tx_key != AUTH_BATCH_UPLOAD_RESPONSE)
        	{
    			if ( auth_tx.TLF01_details.response_text[0] != 0x00 )
    			{
    			 /* Do not send this in a table. */
    			 memcpy( ISO_MESSAGE.priv63+field63_len,
    					 auth_tx.TLF01_details.response_text,
    					 40 );

    			 field63_len += 40;

    			 /* Total length of Field 63 */
    			 ISO_MESSAGE.priv63_len[0] = 0x00;
    			 (void)genutil_binary_to_bcd( (LONG)field63_len,
    										  &ISO_MESSAGE.priv63_len[1] );
    			}
        	}
         }

      }
      else
      {
         /* Not approved.  Send response text if present. */
         resp_text_flag = false;
         memset( tempstr, 0x00, sizeof(tempstr) );

         if ( 0 == strcmp(auth_tx.TLF01_details.response_code, TRANSACTION_NOT_PERMITTED))
         {
            /* Some terminals do not handle this RC, so need to supply text. */
        	if (false == dcpiso_check_If_AMEX_Transaction())
        	{
                resp_text_flag = true;
                strcpy( tempstr, "Transaction Not Permitted" );
        	}

         }
         else if ( auth_tx.TLF01_details.response_text[0] != 0x00 )
         {
            /* But only send it if it is from CSF01 (set in txcntl). */
            if (( auth_tx.FAF01_details.velocity[9].to_time[0] == '1' ) &&
                ( 0 != strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2) &&
          		  0 != strncmp(auth_tx.TLF01_details.pos_entry_mode,"0012",4) &&
				( auth_tx.TLF01_details.product_codes[7].quantity[0] == 'C' ||
				  auth_tx.TLF01_details.product_codes[7].quantity[0] == 'R')))
            {
               resp_text_flag = true;
               strcpy( tempstr, auth_tx.TLF01_details.response_text );
            }
         }

         if (0 != strncmp(auth_tx.TLF01_details.message_type, "0330", 4))
         {
             /* Put the text into the ISO Message structure. */
             if ( resp_text_flag == true )
             {
                /* Total length of Field 63 */
                ISO_MESSAGE.priv63_len[0] = 0x00;
                ISO_MESSAGE.priv63_len[1] = 0x44;

                /* Length of Table 22 - BCD */
                ISO_MESSAGE.priv63[0] = 0x00;
                ISO_MESSAGE.priv63[1] = 0x42;

                /* Table ID */
                memcpy( ISO_MESSAGE.priv63+2, "22", 2 );

                /* Table Data - Response Text */
                memcpy( ISO_MESSAGE.priv63+4, tempstr, 40 );
             }
         }

      }
   }
   return DEVICE_OK ;
} /* Populate_ISO_Private63_Field */

BYTE Populate_ISO_Private62_Field (void)
{
	BYTE	templen[5];
	int		len62;

	if (strlen(auth_tx.TLF01_details.invoice_number) > 0)
	{
		memcpy(ISO_MESSAGE.priv62, auth_tx.TLF01_details.invoice_number,
						strlen(auth_tx.TLF01_details.invoice_number));

		len62 = strlen(auth_tx.TLF01_details.invoice_number);
	}

	/* Build Purchase ID type and Purchase ID */
	/*if (strlen(auth_tx.TLF01_details.product_codes[18].amount) > 0)
	{
		memcpy(ISO_MESSAGE.priv62 + INVOICE_NUMBER_LEN , auth_tx.TLF01_details.product_codes[17].code,
			   PURCHASE_ID_TYPE_LEN);
		memcpy(ISO_MESSAGE.priv62 + PURCHASE_ID_DATA_POS , auth_tx.TLF01_details.product_codes[17].amount,
			   12);
		memcpy(ISO_MESSAGE.priv62 + PURCHASE_ID_LEFTOVER_DATA_POS_1 , auth_tx.TLF01_details.product_codes[18].amount,
			   12);
		memcpy(ISO_MESSAGE.priv62 + PURCHASE_ID_LEFTOVER_DATA_POS_2 , auth_tx.TLF01_details.product_codes[18].code,
			   1);
		len62 = len62 + PURCHASE_ID_TYPE_LEN + PURCHASE_ID_DATA_LEN;
	}*/

	if (len62 !=0)
		sprintf(templen, "%04X", len62);

	genutil_asc_to_bcd (templen, (sizeof(ISO_MESSAGE.priv62_len))*2, ISO_MESSAGE.priv62_len);

	return DEVICE_OK ;
} /* Populate_ISO_Private62_Field */

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
		dcpiso_log_message( 1, 3, strError,"Send_Response_To_Terminal",1 );

		return DEVICE_ERROR ;
	}
	// debug ifo

	//printf("ORG-QUEUE : %s \n",auth_tx.TLF01_details.originator_queue);

	//printf("ORG-INFO : %s \n",auth_tx.TLF01_details.originator_info);
	
	ptemsg_set_orig_comms_info (p_msg_out, auth_tx.TLF01_details.originator_info) ;
	retCode = pteipc_send (p_msg_out, auth_tx.TLF01_details.originator_queue) ;
	
	free (p_msg_out);

	if	(retCode < 0)
	{
		pteipc_get_errormsg (retCode, strError) ;
		sprintf (strError, "Error on pteipc_send to que %s: %s", 
				auth_tx.TLF01_details.originator_queue, 
				strError) ;
		dcpiso_log_message( 1, 3, strError,"Send_Response_To_Terminal",1 );
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
			return auth_tx.TLF01_details.tx_key ;
			break ;
		
		default:
			return AUTH_INVALID ;
		break;
	} // switch
} /* Replace_Tx_Key_In_Auth_Tx */


BYTE Add_Response_Fields_To_Auth_Tx (BYTE tx_type, pCHAR resp_code)
{
   CHAR temp[17] = "";

   auth_tx.TLF01_details.tx_key = tx_type ;
   strcpy(auth_tx.TLF01_details.response_code, resp_code) ;
   /* create RRN and store it in the auth_tx block */
   genutil_format_date(temp);
   strcpy(auth_tx.TLF01_details.retrieval_ref_num, temp + 2);
  
   /* change status to completed and store it in the auth_tx block */
   auth_tx.TLF01_details.general_status = GS_TXN_COMPLETED_OFF;

   /* create date of transaction and store it in the auth_tx table */
   memcpy(auth_tx.TLF01_details.date_yyyymmdd, temp, 8);

   /* create time of transaction and store it in the auth_tx table */
   memcpy(auth_tx.TLF01_details.time_hhmmss, temp + 8, 6);

   /* make entry_type field equal to ADMINISTRATIVE to seperate 
      tran from FINANCIAL trans */
   auth_tx.TLF01_details.entry_type = ADMINISTRATIVE;

   return 1 ;
     
} /* threqust_add_response_fields_to_auth_tx */  

