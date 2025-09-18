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

extern struct iso_msg_rec ISO_MESSAGE ; 
extern AUTH_TX auth_tx ;				

extern CHAR		authorizer_que_name[] ;
extern CHAR     updateds_que_name[] ;
extern CHAR     mrads_que_name[] ;
extern CHAR    nciftr_module_name [];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;
extern INT nMra_enable_flag;
extern INT nFalconSendFlag;
extern DCF01 dcf01;
extern INT Nmra_send_flag;
extern INT Falcon_RT_Flag;
extern INT Falcon_RT_EXT10_Flag;


extern char mp_rc_for_fai[3]={0};
extern char plfc_rc_for_fai[3]={0};
extern char mp_rc_for_TO[3]={0};
extern char plfc_rc_for_TO[3]={0};

#ifdef HCTS2
extern BYTE card_activation_flag;
extern BYTE card_updating_status [5];
extern BYTE card_current_status [51][5];
extern INT  total_card_current_status;
extern BYTE mp_in_vertexon_codes[101][4];
extern BYTE pl_ft_vertexon_codes[101][4];
BOOLEAN Dcpimp_update_atp_card_status_based_on_vertexon_rc ();
BOOLEAN Dcpimp_verify_onus_online_transaction();
#endif

BYTE Process_Response_From_Authorizer (pPTE_MSG p_msg_in)
{
   pBYTE          pCurrent   = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           retCode = 0 ;
   CHAR  gmt_time[20]= {0};
   CHAR strTemp[100]={0};
   BCH01_MP sBCH01_MP;

   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   pCurrent   = ptemsg_get_pte_msg_data_data (p_msg_data);

   memset (&auth_tx, 0, sizeof auth_tx) ;
   memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

   memset(&sBCH01_MP, 0 ,sizeof(sBCH01_MP));

   if (false == Check_Is_Host_Up(nciftr_module_name) &&
		 true == Falcon_RT_Flag  &&
		 AUTH_SALE_RESPONSE == auth_tx.TLF01_details.tx_key &&
		 true == Dcpimp_check_link_for_NCIFTR() &&
		 true == Dcpimp_verify_fraud_rules_for_NCIFTR() &&
		 (0 == strncmp(auth_tx.TLF01_details.response_code,REFER_TO_CARD_ISSUER, 02)) &&
		 (0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1)))
   {
	   /* Do nothing for Falcon RTmode for ATP-STIP case */
   }
   else
   {
	   /*We need to store feild 63 data so that we can show in tran lookup.*/
		//Store term , factor, montlhy and gross amount
		 //term
		memcpy(auth_tx.TLF01_details.deferred_term_length, auth_tx.future_use_1 + 4, 2);

		//Factore value
		memcpy(auth_tx.TLF01_details.deferred_factor, auth_tx.future_use_1 + 6, 7);

		 //Monthly Amount
		memcpy(auth_tx.TLF01_details.monthly_amort_amt, auth_tx.future_use_1 + 13, 12);

			//Gross Amount
		if(auth_tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE)
		{
			memcpy(auth_tx.TLF01_details.def_gross_amt, auth_tx.future_use_1 + 25, 12);
			if ( 0 == strcmp( NULL_STR,auth_tx.TLF01_details.transmission_timestamp ) )
			{
				pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
				strncpy( auth_tx.TLF01_details.transmission_timestamp, &gmt_time[4], 10 );  /* MMDDhhmmss */

			}
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
      memcpy( auth_tx.TLF01_details.processing_code, "380000", 6 );
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

	if (IsOriginatorATerminal(auth_tx.TLF01_details.pos_condition_code))
	   (void)Map_Response_Code( auth_tx.TLF01_details.response_code );

	  /**Insert in to MRADS * /
	  
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
   else if ( false == Check_Is_Host_Up(nciftr_module_name) &&
		   	 true == Falcon_RT_Flag  &&
		     AUTH_SALE_RESPONSE == auth_tx.TLF01_details.tx_key &&
			 true == Dcpimp_check_link_for_NCIFTR() &&
			 true == Dcpimp_verify_fraud_rules_for_NCIFTR() &&
			 (0 == strncmp(auth_tx.TLF01_details.response_code,REFER_TO_CARD_ISSUER, 02)) &&
			 (0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1)))
   {
	   // Real time standing when nciftr is down
		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
		sprintf(strTemp,"In standin processing for FAI RT transaction") ;
		dcpimp_log_message( 1, 2, strTemp," Process_Response_From_Authorizer ", 1 );

		//Copy last 6 digit of transaction id as Authid
		strncpy(auth_tx.TLF01_details.auth_number, auth_tx.TLF01_details.primary_key.transaction_id +13 , 3);
		strncat(auth_tx.TLF01_details.auth_number,auth_tx.TLF01_details.primary_key.transaction_id +17,3);
		if ( 0 == strcmp( NULL_STR,auth_tx.TLF01_details.transmission_timestamp ) )
		{
			pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
			strncpy( auth_tx.TLF01_details.transmission_timestamp, &gmt_time[4], 10 );  /* MMDDhhmmss */

		}

		auth_tx.TLF01_details.tx_key -= 100;
		retCode = Retrieve_BIN01_Record ();
		return retCode;
   }
   else if ( true == txn_is_financial() )
   {
      /* Send request to updateds to update all necessary tables
       * (BCH10, BCH01_MP, BCH20, ACF01, CCF02P, CCF03, etc).
       */
	   if((0 == strncmp(auth_tx.TLF01_details.card_num, "7",1)))
	   {
		   // No Need to update Fleet card Account details, Txcntl will take care of it.
		   retCode = Update_All_Data () ;
	   }
	   else
	   {
		   //retCode = Update_All_Data () ;
		   auth_tx.TLF01_details.tx_key  = auth_tx.TLF01_details.tx_key  - 100;
		   retCode = Get_CCF02P_Details ();
	   }
   }
   else
   {
   
	  if ((0 == strncmp(auth_tx.TLF01_details.message_type,"0320",4)))
	  {
	  	// Response coming from HOST for 330,we will forward this and update BCH10 details  
			Process_Batch_Upload();
			auth_tx.TLF01_details.tx_key=AUTH_BATCH_UPLOAD_RESPONSE;
     		return BCH10_INSERT_SUCCESSFUL ;
	  }

	  /* No need to send to updatds, so respond to the terminal. */
      Send_Transaction_To_Originator();
	  /*Check if it 500 message respnose coming from Cadencie (online)*/
	  if ((0 == strncmp(auth_tx.TLF01_details.message_type,"0500",4)) &&
	  	  (0 == strncmp(auth_tx.TLF01_details.response_code,"00",2)))
	  {
	  		/*We have received successful settlement message from cadency*/
			retCode= Process_When_Settlement_Success_Online();
			  if (retCode != DEVICE_OK)
     		 return retCode ;
      }
	  else if ((0 == strncmp(auth_tx.TLF01_details.message_type,"0500",4)) &&
	  	  (0 == strncmp(auth_tx.TLF01_details.response_code,"95",2)))
	  {
	  		/*We have received successful settlement message from cadency*/
			retCode= Process_When_Transaction_Total_Does_Not_Equal_Batch_Total (sBCH01_MP) ;
			  if (retCode != DEVICE_OK)
     		 return retCode ;
      }
	  else if ((0 == strncmp(auth_tx.TLF01_details.message_type,"0500",4)))
	  {
	  	//	response code is nither 95 and 00 . Return device error.
     		 return DEVICE_ERROR;
	  }
	  else
	  {
			retCode = DEVICE_OK ;
	  }
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
	BYTE		  temp_nfi_name[11] = {0};
	CHAR          strError[512] = {0};

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
	
	/* Build the response message for the terminal in ReqRsp_Buffer */
	isoutil_build_terminal_response (auth_tx.TLF01_details.tx_key) ;

	/* Send the message to the terminal */
	retCode = Send_Response_To_Terminal () ;

	/*Let send to falcon if it sale/AUTH/Offline sale and flaconsend flag is set to 1 */
	if(((auth_tx.TLF01_details.tx_key == AUTH_SALE)||
		(auth_tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE)||
		(auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION)||
		(auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE)||
		(auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE) ||
		(auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE) ||
		(auth_tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE) ||
		(auth_tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE))&&
		(nFalconSendFlag == 1))
	{
		if (true == Falcon_RT_Flag  &&
			 true == Dcpimp_check_link_for_NCIFTR() &&
			 true == Dcpimp_verify_fraud_rules_for_NCIFTR())
		{
			if(true == Falcon_RT_EXT10_Flag &&
			   false == Dcpimp_check_vertexon_rc_for_ext10_falcnrt())
			{
				if(false == Send_EXT10_Request_To_FalconRT_Service())
				{
					sprintf(strError,"error in sending EXT10 req to falcon") ;
					dcpimp_log_message( 1, 3, strError, " Send_Transaction_To_Originator ", 1 );
				}
			}
		}
		else
		{
			 if(false == Send_Request_To_Falcon_Service())
			 {
				 sprintf(strError,"error in sending data to falcon") ;
				 dcpimp_log_message( 1, 3, strError, " Send_Transaction_To_Originator ", 1 );
			 }
		}
	}

    if((auth_tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
	    auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE ||
	    auth_tx.TLF01_details.tx_key == AUTH_SALE ||
	    auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION) &&
    	nMra_enable_flag ==1)
    {
	    strncpy(temp_nfi_name,auth_tx.TLF01_details.nfi_name,sizeof(auth_tx.TLF01_details.nfi_name));
	    memcpy(auth_tx.TLF01_details.nfi_name,auth_tx.MCF01_details.negative_group_id,sizeof(auth_tx.MCF01_details.negative_group_id));
	    if(Nmra_send_flag == 1)
	    {
	 	   retCode = Insert_MRA_AUTH_Details( ST2_NONE ) ;
	    }
	    else if(Nmra_send_flag == 0)
	    {
		   dcpimp_Send_Request_To_Mracon_Service();
	    }
	    strncpy(auth_tx.TLF01_details.nfi_name,temp_nfi_name,sizeof(auth_tx.TLF01_details.nfi_name));
    }

	/* Insert the TLF01 record - drop the transaction */
	retCode = Insert_TLF01_Details( ST2_NONE ) ; /* Ignore any SYSTEM ERROR */

#ifdef HCTS2
	//pre condition check
	//local, !stndin
	if(Dcpimp_verify_onus_online_transaction() == true)
	{
		Dcpimp_update_atp_card_status_based_on_vertexon_rc();
	}
#endif

	return DEVICE_OK ;
} /* Send_Transaction_To_Originator */

#ifdef HCTS2
BOOLEAN Dcpimp_verify_onus_online_transaction()
{
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code, STANDIN) != 0)
	{
		return true;
	}

	return false;
}

BOOLEAN Dcpimp_update_atp_card_status_based_on_vertexon_rc ()
{
	int i=0, codesCount = 0;
	BOOLEAN cardUpdateFlag = false;
	BOOLEAN bReturnCode = true ;
	CCF02P ccf02P_details;
	PAF01 sPAF01;
	char tempStatus[51] = {0};
	//remove after UT
	char buffer[100] = {0};

	if(card_activation_flag == 'Y')
	{
		//validate the response/action codes from Vertexon
		if(strcmp(auth_tx.TLF01_details.acquirer_id, "nciftr") == 0 &&
		   ((strncmp(auth_tx.TLF01_details.message_type, "0210", 4) == 0) ||
			 auth_tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE)
		   )
		{
			/*sprintf(buffer, "nciftr checking for configured RC with response RC = [%s]", auth_tx.TLF01_details.response_code);
			dcpimp_log_message(2, 1, buffer, "Dcpimp_update_atp_card_status_based_on_vertexon_rc", 1);*/

			if(strncmp(auth_tx.TLF01_details.card_num, "7", 1) == 0)
			{
				codesCount = atoi(pl_ft_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(pl_ft_vertexon_codes[i], auth_tx.TLF01_details.response_code, 2) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			else
			{
				codesCount = atoi(mp_in_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(mp_in_vertexon_codes[i], auth_tx.TLF01_details.response_code, 2) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}

			if(cardUpdateFlag == true)
			{
				sprintf(buffer, "Qualified for card status update with  RC = [%s], sending request for trands", auth_tx.TLF01_details.response_code);
				dcpimp_log_message(2, 1, buffer, "Dcpimp_update_atp_card_status_based_on_vertexon_rc", 1);

				memset(&ccf02P_details, 0x00, sizeof(ccf02P_details));
				strcpy(ccf02P_details.primary_key.card_nbr, auth_tx.TLF01_details.card_num);
				strcpy(ccf02P_details.primary_key.card_type, "C");

				//storing STAN to display in Change History data
				memset(ccf02P_details.changed_by, 0, sizeof(ccf02P_details.changed_by));
				strcpy(ccf02P_details.changed_by, "STAN:");
				strcat(ccf02P_details.changed_by, auth_tx.TLF01_details.sys_trace_audit_num);

				//db call to update card status for both CCF02P and PAF01
				bReturnCode = Send_Request_To_Service ("trandsA",
													   MT_DB_REQUEST,
													   ST1_DB_UPDATE, 0,
													   ATP_CARD_STATUS_UPDATE_BY_BDO_HOST,
													   (pBYTE)&ccf02P_details,
													   sizeof(ccf02P_details));
			}
		}
	}
	else
	{
		dcpimp_log_message(2, 2, "Vertexon Card Activation flag is disabled", "Dcpimp_update_atp_card_status_based_on_vertexon_rc", 1);
	}

	return bReturnCode ;
}
#endif

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
			
	if(0 == strncmp(auth_tx.TLF01_details.response_code,"19",2))
	{
		if(auth_tx.TLF01_details.product_codes[2].quantity[0] == 1 &&
		   mp_rc_for_TO[0] != '\0'	)
		{
			memcpy (&ISO_MESSAGE.responsecd[0],
					mp_rc_for_TO,
					sizeof(ISO_MESSAGE.responsecd)) ;
		}
		else if(plfc_rc_for_TO[0] != '\0')
		{
			memcpy (&ISO_MESSAGE.responsecd[0],
					plfc_rc_for_TO,
					sizeof(ISO_MESSAGE.responsecd)) ;
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
		memcpy (&ISO_MESSAGE.responsecd[0],
				auth_tx.TLF01_details.response_code,
				sizeof(ISO_MESSAGE.responsecd)) ;
	}
	memcpy (&ISO_MESSAGE.responsecd[0], 
			auth_tx.TLF01_details.response_code, 
			sizeof(ISO_MESSAGE.responsecd)) ;

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
   CHAR    tmp_len_63[5]="";


   INT field63_len = 0;
   CHAR response_text1[41] = "GB      ACCEPTED                        " ;
   CHAR response_text2[41] = "QD      DUPLICATE   BATCH               " ;
   CHAR response_text3[41] = "RB      DIFFERENT COUNTS OR AMOUNTS     " ;
   CHAR	strError[512] = {0};

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

        /*Check if its a MP or fllet card tansaction */
		if((0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))||
	   	   (0 == strncmp(auth_tx.TLF01_details.card_num, "7",1)))
		{
				/*We have stored response coming from Host in auth_tx.featire_1 feild 
				  or we have prepared this during stip and stored in  */
				 memcpy(tmp_len_63,auth_tx.future_use_1, 4); 
				 field63_len=atoi(tmp_len_63);
				 genutil_asc_to_bcd (tmp_len_63, 
							sizeof(ISO_MESSAGE.priv63_len)*2, 
							ISO_MESSAGE.priv63_len) ;
				 memcpy(ISO_MESSAGE.priv63,auth_tx.future_use_1 + 4,field63_len);
				 sprintf (strError, "Response field 63: %s ",ISO_MESSAGE.priv63) ;
		  		 dcpimp_log_message( 2, 1, strError," Populate_ISO_Private63_Field ", 1 );
				 return DEVICE_OK ;
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

		if (auth_tx.TLF01_details.tx_key != AUTH_BATCH_UPLOAD_RESPONSE)
		{
	         /* Cash Bonus and other response texts. */
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
      else
      {
         /* Not approved.  Send response text if present. */
         resp_text_flag = false;
         memset( tempstr, 0x00, sizeof(tempstr) );

         if ( 0 == strcmp(auth_tx.TLF01_details.response_code, TRANSACTION_NOT_PERMITTED))
         {
            /* Some terminals do not handle this RC, so need to supply text. */
            resp_text_flag = true;
            strcpy( tempstr, "Transaction Not Permitted" );
         }
         else if ( auth_tx.TLF01_details.response_text[0] != 0x00 )
         {
            /* But only send it if it is from CSF01 (set in txcntl). */
            if ( auth_tx.FAF01_details.velocity[9].to_time[0] == '1' )
            {
               resp_text_flag = true;
               strcpy( tempstr, auth_tx.TLF01_details.response_text );
            }
         }

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
   return DEVICE_OK ;
} /* Populate_ISO_Private63_Field */

BYTE Send_Response_To_Terminal (void)
{
	pPTE_MSG	p_msg_out = NULL_PTR ;
	pPTE_MSG	p_msg_auth = NULL_PTR ;
	LONG		retCode = 0L ;
	CHAR		strError[512] = {0};
	INT         buf_temp_len=0;

	// IF response is coming for Cadencie send it as it to terminal
	/*Check if its a MP or fllet card tansaction */
	if((0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))||
	   (0 == strncmp(auth_tx.TLF01_details.card_num, "7",1)))
	{
		buf_temp_len = atoi(auth_tx.resp_data_len);
		if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
		{ 
			buflen = buf_temp_len;
			memcpy(ReqRsp_Buffer,auth_tx.resp_buffer,buflen);
			sprintf (strError, "Sending Cadencie response to Terminal for message type :%s", 
					auth_tx.TLF01_details.message_type) ;
			dcpimp_log_message( 2, 1, strError," Send_Response_To_Terminal ", 1 );
		}

	}
	
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
		dcpimp_log_message( 1, 3, strError," Send_Response_To_Terminal", 1 );
		return DEVICE_ERROR ;
	}

	ptemsg_set_orig_comms_info (p_msg_out, auth_tx.TLF01_details.originator_info) ;
	retCode = pteipc_send (p_msg_out, auth_tx.TLF01_details.originator_queue) ;
	
	free (p_msg_out);

	if	(retCode < 0)
	{
		pteipc_get_errormsg (retCode, strError) ;
		sprintf (strError, "Error on pteipc_send to que %s: %s", 
				auth_tx.TLF01_details.originator_queue, 
				strError) ;
		dcpimp_log_message( 1, 3, strError," Send_Response_To_Terminal ", 1 );
		return DEVICE_ERROR ;
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, &auth_tx, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
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

BOOLEAN Dcpimp_check_vertexon_rc_for_ext10_falcnrt ()
{
	int i=0, codesCount = 0;
	BOOLEAN bReturnCode = false ;
	char tempStatus[51] = {0};
	//remove after UT
	char buffer[100] = {0};

		//validate the response/action codes from Vertexon
		if(strcmp(auth_tx.TLF01_details.acquirer_id, "nciftr") == 0)
		{
			sprintf(buffer, "nciftr checking for configured RC with response RC = [%s]", auth_tx.TLF01_details.response_code);
			dcpimp_log_message(2, 1, buffer, "Dcpimp_check_vertexon_rc_for_ext10_falcnrt", 0);

			if((strncmp(auth_tx.TLF01_details.card_num, "7", 1) != 1) ||
			   (strncmp(auth_tx.TLF01_details.product_codes[2].quantity, "1", 1) != 1))
			{
				codesCount = atoi(mp_in_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(mp_in_vertexon_codes[i], auth_tx.TLF01_details.response_code, 2) == 0)
					{
						bReturnCode = true;
						break;
					}
				}
			}
			else
			{
				codesCount = atoi(pl_ft_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(pl_ft_vertexon_codes[i], auth_tx.TLF01_details.response_code, 2) == 0)
					{
						bReturnCode = true;
						break;
					}
				}
			}
		}

	return bReturnCode ;
}

