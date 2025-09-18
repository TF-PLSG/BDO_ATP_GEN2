/**************************************************************************************************
*  
* MODULE:      Database_Response.c

  Copyright (c) 2005, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard
*  
* DESCRIPTION: Contains database response routines
*
* APPLICATION: DCPISO
*
* AUTHOR:  Sanjoy Dasgupta
*
      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "ptemsg.h"
#include "global_definitions.h"  
#include "database_request.h"
#include "database_response.h"
#include "batch_upload.h"
#include "general_utilities.h"
#include "service_functions.h"
#include "process_incoming_msg.h"
#include "process_outgoing_msg.h"
#include "genutil.h"
#include "txutils.h"
#include "ntutils.h"

extern AUTH_TX auth_tx ;
extern INT void_reload_timeout;
extern INT nMra_enable_flag;
DCF01 dcf01;

extern CHAR  percent_overlimit_value_for_VISA[4];
extern CHAR  percent_overlimit_value_for_MC[4];
extern CHAR  percent_overlimit_value_for_JCB[4];
extern CHAR  percent_overlimit_value_for_AMEX[4];
extern CHAR  percent_overlimit_value_for_CUP[4];
extern CHAR  percent_overlimit_value_for_DINERS[4];
extern CHAR error_response_code_for_PREAUTH_COMPLETION_VALIDATION[3];
extern CHAR Incremental_Auth_flag_CUP[1];

#if BDOR_62_PREAUTH_VOID

extern CHAR insufficient_preauth_declined_rc_for_MC[3];
extern CHAR insufficient_preauth_declined_rc_for_VISA[3];
extern CHAR insufficient_preauth_declined_rc_for_AMEX[3];
extern CHAR insufficient_preauth_declined_rc_for_DCI[3];
extern CHAR insufficient_preauth_declined_rc_for_JCB[3];
extern CHAR insufficient_preauth_declined_rc_for_UPI[3];

extern CHAR maximum_validity_of_preauth_in_days_for_MC[3];
extern CHAR maximum_validity_of_preauth_in_days_for_VISA[3];
extern CHAR maximum_validity_of_preauth_in_days_for_AMEX[3];
extern CHAR maximum_validity_of_preauth_in_days_for_DCI[3];
extern CHAR maximum_validity_of_preauth_in_days_for_JCB[3];
extern CHAR maximum_validity_of_preauth_in_days_for_UPI[3];

extern CHAR smcc_maximum_validity_of_preauth_in_days_for_MC[3];
extern CHAR smcc_maximum_validity_of_preauth_in_days_for_VISA[3];
extern CHAR smcc_maximum_validity_of_preauth_in_days_for_AMEX[3];
extern CHAR smcc_maximum_validity_of_preauth_in_days_for_DCI[3];
extern CHAR smcc_maximum_validity_of_preauth_in_days_for_JCB[3];
extern CHAR smcc_maximum_validity_of_preauth_in_days_for_UPI[3];

extern CHAR declined_rc_for_original_txn_not_present_for_MC[3];
extern CHAR declined_rc_for_original_txn_not_present_for_VISA[3];
extern CHAR declined_rc_for_original_txn_not_present_for_AMEX[3];
extern CHAR declined_rc_for_original_txn_not_present_for_DCI[3];
extern CHAR declined_rc_for_original_txn_not_present_for_JCB[3];
extern CHAR declined_rc_for_original_txn_not_present_for_UPI[3];

#endif

BYTE Process_Dataserver_OK_Message (pPTE_MSG p_msg_in)
{
   CHAR           strError[256] = {0} ;
   pBYTE          pCurrent = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           subType1 = 0, subType2 = 0 ;
   BYTE           app_data_type = 0 ;
   BYTE           retCode = 0 ;
   char current_system_date[9]="";
   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in) ;
   pCurrent = ptemsg_get_pte_msg_data_data (p_msg_data) ;

   memset (&auth_tx, 0, sizeof auth_tx) ;
   memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

   app_data_type = ptemsg_get_pte_msg_data_app_data_type (p_msg_data) ;
   subType1 = ptemsg_get_msg_subtype1 (p_msg_in) ;
   subType2 = ptemsg_get_msg_subtype2 (p_msg_in) ;

   if (!If_Transaction_Cannot_Be_Declined ())
   {
      retCode = Perform_Auth_Tx_Checks () ;
      if (retCode != DEVICE_OK)
         return retCode ;
   }



   if (auth_tx.TLF01_details.tx_key < 100)
   {   
      if (subType1 == ST1_DB_SELECT)
      {
         /* If the totals on the transaction match the batch totals, then
          * we have to update the batch header (BCH01) with the real batch
          * number, update the batch details (BCH10) with the real batch number,
          * change batch_status to 'C', add the batch number to the device
          * record, etc.
          */
         if (app_data_type == BIN01_DATA)
         {
            retCode = Process_select_bin01_db_OK (pCurrent) ;
            return retCode ;
         }
         else if (app_data_type == MCT01_DATA)
         {
            retCode = Process_select_mct01_db_OK (pCurrent) ;
            return retCode ;
         }
         else if (app_data_type == DCF01_DATA)
         {
            if ( subType2 == ST2_DB_SELECT_DCF01_AUTO_INIT )
            {
               retCode = Process_select_dcf01_auto_init (pCurrent) ;
            }
            else 
            {
               retCode = Process_select_dcf01_db_OK (pCurrent) ;
            }
            return retCode ;
         }
         else if (app_data_type == MCF01_DATA)
         {
            retCode = Process_select_mcf01_db_OK (pCurrent) ;
            return retCode ;
         }
         else if (app_data_type == BCH01_DATA)
         {
            retCode = Process_select_bch01_db_OK (pCurrent, subType2) ;
            return retCode ;
         }
         else if (app_data_type == BCH10_DATA)
         {
            if ( subType2 == ST2_DB_SELECT_BCH10_DUPLICATE_TXN )
            {
               /* This is a duplicate txn. */
               retCode = process_duplicate_txn();
            }
            else
               retCode = Process_select_bch10_db_OK (pCurrent) ;
            return retCode ;
         }
         else if (app_data_type == BCH20_DATA)
         {

        	 if(subType2 == ST2_DB_SELECT_BCH20_FOR_INCREMENTAL_PREAUTH ||
        		(subType2 == ST2_DB_SELECT_BCH20_FOR_ESTIMATED_AUTHORIZATION ))
			 { /* added seperate subType2-ST2_DB_SELECT_BCH20_FOR_ESTIMATED_AUTHORIZATION for amex AMEX03B_APRIL2024*/
				/*We find the incremental preauth transaction.*/
				retCode = Process_select_bch20_db_OK_incremental_preauth (pCurrent) ;
				return retCode;
			 }
#if BDOR_62_PREAUTH_VOID
        	 else if (subType2 == ST2_DB_SELECT_BCH20_FOR_PREAUTH_VOID)
        	 {
        		 retCode = Process_select_bch20_db_OK_preauth_void(pCurrent);
        		 return (retCode);
        	 }
        	 else if (subType2 == ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT)
        	 {
        		 retCode = Process_select_bch20_db_OK_preauth_total_amount (pCurrent) ;
        		 return (retCode) ;
        	 }
#endif
        	 else
        	 {
				  retCode = Process_select_bch20_db_OK (pCurrent) ;
				  return (retCode) ;
             }
         }
         else if (app_data_type == TLF01_DATA)
         {
            retCode = Process_select_tlf01_db_OK (pCurrent) ;
            return retCode ;
         }
         else
         {
            strcpy (strError, "Invalid incoming message type") ;
            retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR,strError,
                                "Process_Dataserver_OK_Message",
                                 GS_TXN_SYSTEM_ERROR,
                                 INVALID_TRANSACTION) ;
            return retCode ;
         }
      }

      else if (subType1 == ST1_DB_UPDATE)
      {
         if (app_data_type == BCH01_DATA)
         {
            if (subType2 == ST2_DB_UPDATE_BCH01_GOOD_SETTLE)
            {
               /* Totals have matched, the BCH01 record has been updated
                * with batch number, batch_status = 'C', DCF01 has the
                * new batch number, BCH10 records have the new batch number.
                * Set response code = 00 and return BCH01_GOOD_SETTLE 
                */
               retCode = Process_update_bch01_db_OK (BCH01_TOTALS_MATCHED) ;
               return retCode ;
            }
            else if (subType2 == ST2_DB_UPDATE_BCH01_DELETE_BCH10)
            {
               /* Totals have matched, the BCH01 record has been updated
                * with batch number, batch_status = 'C', DCF01 has the
                * new batch number, BCH10 records have the new batch number.
                * Set response code = 00 and return BCH01_GOOD_SETTLE 
                */
               retCode = Process_update_bch01_db_OK (BCH01_TOTALS_NOT_MATCHED);
               return retCode ;
            }
         }
      }
      else if (subType1 == ST1_DB_INSERT)
      {  // This is when we insert a BCH10 record on a BATCH_UPLOAD 
         if (app_data_type == BCH10_DATA)
         {
            if (subType2 == ST2_DB_INSERT_BCH10_SETTLE)
            {
               retCode = Process_insert_bch10_db_OK () ;
               return retCode ;
            }
         }
         else if (app_data_type == TLF01_DATA)
         {
            /* do nothing */
         }
      }
   }
   else
   {  // The incoming db ok message is from a response transaction
      if (subType1 == ST1_DB_UPDATE && app_data_type == ALL_DATA)
      {
         /* Only send response to originator if it is not a credit reversal.
          * A response for these have already been sent.
          */
         if((auth_tx.TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE) ||
            (CREDIT_BIN != Get_Bin_Type(auth_tx.TLF01_details.processing_code)))
         {
            /* Voids are responded to already excpet CUP . For CUP we will send the response */
			if ( (auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE))
			{
				if(true == dcpiso_Check_If_CUP_Transaction())
				{
					return SEND_RESPONSE_TO_ORIGINATOR;
				}
			}
				
            if ( auth_tx.TLF01_details.tx_key != AUTH_VOID_SALE_RESPONSE )
               return SEND_RESPONSE_TO_ORIGINATOR ;
         }
      }
      else if (subType1 == ST1_DB_INSERT && app_data_type == TLF01_DATA)
      {
         /* Do nothing - drop the transaction - recall that at this time
          * the transaction has already been sent to the terminal
          */
      }
   }
   return DEVICE_OK ;
} /* Process_Dataserver_OK_Message */

BYTE Process_Dataserver_NotFound_Message (pPTE_MSG p_msg_in)
{
	pBYTE			pCurrent = 0 ;
	pPTE_MSG_DATA	p_msg_data= 0 ;
	BYTE			app_data_type = 0 ;
	pCHAR			temp_dest_queue = 0 ;
	pCHAR			temp_orig_queue = 0 ;
	CHAR			error_msg[200] = {0} ;
	CHAR			Buffer[240] = {0} ;
	BYTE			subtype1 = 0, subtype2 = 0 ;
	BYTE			retCode = 0 ;

	p_msg_data = ptemsg_get_pte_msg_data(p_msg_in) ;
	pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data) ;

	memset (&auth_tx, 0, sizeof auth_tx) ;
	memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

	app_data_type = ptemsg_get_pte_msg_data_app_data_type (p_msg_data) ;
	temp_dest_queue = ptemsg_get_msg_dest_queue (p_msg_in) ;
	temp_orig_queue = ptemsg_get_msg_orig_queue (p_msg_in) ;

	subtype1 = ptemsg_get_msg_subtype1 (p_msg_in) ;
	subtype2 = ptemsg_get_msg_subtype2 (p_msg_in) ;


	if (!If_Transaction_Cannot_Be_Declined ())
	{
		retCode = Perform_Auth_Tx_Checks () ;
		if (retCode != DEVICE_OK)
			return (retCode) ;
	}

   retCode = DEVICE_OK;
	if (auth_tx.TLF01_details.tx_key < 100)
	{   
		if (subtype1 == ST1_DB_SELECT || subtype1 == ST1_DB_UPDATE)
		{
			if (app_data_type == BIN01_DATA)
				retCode = Process_bin01_db_not_found (pCurrent) ;

			else if (app_data_type == MCT01_DATA)
				retCode = Process_mct01_db_not_found (pCurrent) ;

			else if (app_data_type == MCF01_DATA)
				retCode = Process_mcf01_db_not_found (pCurrent) ;

			else if (app_data_type == DCF01_DATA)
				retCode = Process_dcf01_db_not_found (pCurrent) ;

			else if (app_data_type == BCH01_DATA)
				retCode = Process_bch01_db_not_found (pCurrent, subtype2);

			else if (app_data_type == BCH10_DATA)
			{
            if ( subtype2 == ST2_DB_SELECT_BCH10_DUPLICATE_TXN )
            {
               /* This is not a duplicate txn.  Get device record. */
               retCode = Get_DCF01_Details (ST2_DB_SELECT_DCF01) ;
            }
            else
               retCode = Process_bch10_db_not_found (p_msg_in,pCurrent);
			}
			else if (app_data_type == BCH20_DATA)	
			{
				if(subtype2 == ST2_DB_SELECT_BCH20_FOR_INCREMENTAL_PREAUTH ||
				   subtype2 == 	ST2_DB_SELECT_BCH20_FOR_ESTIMATED_AUTHORIZATION)
				{
					/*We could not find the any preauth transaction.This is the 1st Preauth
					 transaction send transaction to Authorizer for further processing*/
#if AMEX03B_APRIL2024
					if(true == dcpiso_check_If_AMEX_Transaction() && acq_amex_variable_auth_flag == 'Y')
					{
						/* This is to build DE24 in Amex Module*/
						auth_tx.TLF01_details.saf[0]=ESTIMATED_AUTH;
					}
#endif
					retCode = Send_Transaction_To_Authorizer () ;
				}
#if BDOR_62_PREAUTH_VOID
				 else if (subtype2 == ST2_DB_SELECT_BCH20_FOR_PREAUTH_VOID)
				 {
					 retCode = Process_select_bch20_preauth_void_db_not_found();
				 }
				 else if (subtype2 == ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT)
				 {
					 retCode = Process_select_bch20_db_OK_preauth_total_amount_not_found () ;
					 return (retCode) ;
				 }
#endif
				else
				{
				       retCode = Process_bch20_db_not_found (p_msg_in,pCurrent) ;
				}
			}

			else if (app_data_type == TLF01_DATA)
				retCode = Process_tlf01_db_not_found( pCurrent );
		}
	}
	else /* transaction has a response tx_key */
	{   
		retCode = DEVICE_ERROR ;
	}
	return( retCode );
} /* Process_Dataserver_NotFound_Message */

BYTE Process_Dataserver_ERROR_Message (pPTE_MSG p_msg_in)
{
   pBYTE          pCurrent = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           retCode;
   BYTE           app_data_type = 0;
   BYTE           subType2;
   CHAR           strError[512] = {0} ;

   p_msg_data    = ptemsg_get_pte_msg_data (p_msg_in) ;
   pCurrent      = ptemsg_get_pte_msg_data_data (p_msg_data) ;
   app_data_type = ptemsg_get_pte_msg_data_app_data_type (p_msg_data) ;

   memset (&auth_tx, 0, sizeof auth_tx) ;
   memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

   strcpy (strError, pCurrent + sizeof auth_tx) ;
   retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                          "Process_Dataserver_ERROR_Message",
                                           GS_TXN_SYSTEM_ERROR,
                                           SYSTEM_MALFUNCTION) ;
   if (app_data_type == TLF01_DATA)
   {
      subType2 = ptemsg_get_msg_subtype2( p_msg_in );
      if ( subType2 == ST2_DB_SELECT_TRAN_BY_CARD_NUM )
      {
         sprintf( strError,
                 "Unable to get Reload Confirm in TLF01 for Void, Amount: %s, stan: %s",
                  auth_tx.TLF01_details.total_amount,
                  auth_tx.TLF01_details.sys_trace_audit_num );
         retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                                "Process_Dataserver_ERROR_Message",
                                                 GS_TXN_SYSTEM_ERROR,
                                                 SYSTEM_MALFUNCTION );
      }
      else
      {
         /* Drop the transaction - recall that the transaction has already been
          * sent to the terminal - if we do not return DEVICE_OK, then the code
          * in message_handler.c will again try and send to the originator.
          */
         retCode = DEVICE_OK;
      }
   }
   else if( auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE || app_data_type == MRA_AUTH_TBL_DATA)
   {
      /* Drop the transaction.  Already responded to it. */
      retCode = DEVICE_OK;
   }
   else if ( 0 == strcmp("0320",auth_tx.TLF01_details.message_type) )
   {
      /* The tx_key has been changed to reflect the orig txn. */
      auth_tx.TLF01_details.tx_key = AUTH_BATCH_UPLOAD_RESPONSE;

      sprintf( strError,
              "Batch Upload txn failed. Stan = %s  Could cause a batch upload loop.",
               auth_tx.TLF01_details.sys_trace_audit_num );

      retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                             "Process_Dataserver_ERROR_Message",
                                              GS_TXN_REJECTED_BATCH,
                                              INVALID_TRANSACTION );
   }
   else if (app_data_type == BCH20_DATA)
   {
      subType2 = ptemsg_get_msg_subtype2( p_msg_in );
      if ( subType2 == ST2_DB_SELECT_BCH20_FOR_PREAUTH )
      {
        /* Original transaction was not present. No need to respond back */
         retCode = DEVICE_OK;
      }
   }
   return (retCode) ;
} /* Process_Dataserver_ERROR_Message */




BYTE Process_select_dcf01_auto_init (pBYTE pCurrent)
{
	memset (&dcf01, 0, sizeof dcf01);
	memcpy (&dcf01, pCurrent + sizeof auth_tx, sizeof dcf01);
	return (BCH01_TOTALS_MATCHED) ;
} 

BYTE Process_select_dcf01_db_OK (pBYTE pCurrent)
{
   CHAR  strError [512] = {0} ;
   BYTE  retCode = 0 ;
   DCF01 sDCF01 ;

   memset (&sDCF01, 0, sizeof sDCF01);
   memcpy (&sDCF01, pCurrent + sizeof auth_tx, sizeof sDCF01);

   strcpy (auth_tx.TLF01_details.organization_id,   sDCF01.organization_id) ;
   strcpy (auth_tx.TLF01_details.terminal_type,     sDCF01.device_type) ;
   strcpy (auth_tx.industry_code,                   sDCF01.industry_code) ;
   strcpy (auth_tx.TLF01_details.txn_cnt,           sDCF01.txn_cnt) ;
   strcpy (auth_tx.TLF01_details.dcf01_retired_cwk, sDCF01.retired_cwk) ;
   strcpy (auth_tx.TLF01_details.source_key,        sDCF01.cwk) ;
   strcpy (auth_tx.TLF01_details.kek,               sDCF01.kek) ;
   strcpy (auth_tx.TLF01_details.source_pin_format, sDCF01.pin_block_format) ;
   strncpy(auth_tx.EMV_details.chip_condition_code, sDCF01.additional_pos_information,1) ;	

   /*Store EMV capability only for AMEX, CUP and Diners*/
   if(true == dcpiso_check_If_AMEX_Transaction() )
   {

	   strncpy(auth_tx.TLF01_details.terminal_type,
			   sDCF01.current_bus_date,AMEX_DEVICE_EMV_CAPABILITY_LEN) ;
   }
   else if (true == dcpiso_Check_If_CUP_Transaction())
   {
	   strncpy(auth_tx.TLF01_details.terminal_type,
			   sDCF01.current_bus_date + DEVICE_EMV_CAPABILITY_POS_FOR_CUP,
			   CUP_DEVICE_EMV_CAPABILITY_LEN) ;
   }
   else if(true == dcpiso_check_If_Diners_Transaction())
   {
	   strncpy(auth_tx.TLF01_details.terminal_type,
			   sDCF01.current_bus_date + DEVICE_EMV_CAPABILITY_POS_FOR_DINERS,
			   DINERS_DEVICE_EMV_CAPABILITY_LEN) ;
   }
   else
   {
	   strcpy (auth_tx.TLF01_details.terminal_type,     sDCF01.device_type) ;
   }
   /* Save DCF01 batch number for logging purpose later.*/
   memcpy(auth_tx.TLF01_details.voice_auth_text, sDCF01.batch_nbr, sizeof(sDCF01.batch_nbr));

   if (auth_tx.TLF01_details.tx_key == AUTH_BATCH_UPLOAD)
   {
	  if(true == dcpiso_Check_If_CUP_Transaction())
	  {
		  /*Dont update Merchant ID for CUP, it will be CUP accpotor ID */
	  }
     else if (!strlen(auth_tx.TLF01_details.merchant_id)  || 
         (IsFieldBlank (auth_tx.TLF01_details.merchant_id,
                    strlen(auth_tx.TLF01_details.merchant_id))))
      {
         strcpy (auth_tx.TLF01_details.merchant_id, sDCF01.merchant_id) ;
      }
   }

   retCode = Edit_DCF01_Record (sDCF01) ;
   if (retCode != DEVICE_OK)
      return (retCode) ;

	switch (auth_tx.TLF01_details.tx_key)
	{
		case AUTH_INVALID:
			sprintf (strError, 
					"Invalid transaction type %d in the transaction", 
					auth_tx.TLF01_details.tx_key) ;
			retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
											"Process_select_dcf01_db_OK",
											GS_TXN_SYSTEM_ERROR,
											FORMAT_ERROR) ;
			return (ISO_PARSE_ERROR) ;
			
        break ;

		case AUTH_TEST:
		case AUTH_LOGON:
			strcpy (strError, "Transaction is AUTH_TEST/AUTH_LOGON\n") ;
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
			auth_tx.TLF01_details.entry_type = ADMINISTRATIVE ;
			PRINT (strError) ;
			/* For this transaction there is no need to send to the authorizer - returning
			   SEND_RESPONSE_TO_ORIGINATOR would cause the control to insert into tlf01 and
			   sending back a response to the terminal
			*/
			   
			return (SEND_RESPONSE_TO_ORIGINATOR) ;
		break ;

		case AUTH_STATISTICS:
			retCode = Process_Terminal_Statistics_Transaction (auth_tx.TLF01_details.tx_key) ;
			return (retCode) ;
		break;
      
		default:
        break;
    }

	/* For all other transactions, request a MCF01 database read */
	retCode = Get_MCF01_Details () ;
	return (retCode) ;
	

} /* Process_select_sDCF01_db_reply */

BYTE Process_select_mcf01_db_OK (pBYTE pCurrent)
{
   INT   len = 0;
   INT   MGIDlen = 0;
   INT   emv_len = 0;
   INT   ret_val = 0;
   MCF01 sMCF01;
   BYTE  retCode = 0;
   CHAR  temp_str[100] = {0};
   CHAR  strError[512] = {0};
   BYTE  hex_tag[5] = {0};
   BYTE  merchant_group_id[16] = {0};

	memset (&sMCF01, 0, sizeof sMCF01) ;
	memcpy (&sMCF01, pCurrent+ sizeof auth_tx, sizeof sMCF01) ;
	memcpy (&auth_tx.MCF01_details, &sMCF01, sizeof sMCF01) ;

	strcpy (auth_tx.TLF01_details.currency_code, sMCF01.currency_code) ;
	memcpy(auth_tx.TLF01_details.product_codes[4].code,sMCF01.process_control_id,6);

	MGIDlen = strlen(sMCF01.network[5].priority_routing);
	if(MGIDlen == 1)
	{
		if(0 == strncmp(sMCF01.network[5].priority_routing , " ", 1))
		{
			MGIDlen = 0;
		}
	}
	else
	{
		strncpy(merchant_group_id,sMCF01.network[5].priority_routing, (sizeof(sMCF01.network[5].priority_routing) -1));
		strcat(merchant_group_id,sMCF01.network[6].priority_routing);
	}
	if(strcmp(sMCF01.name02,NULL_STR)!=0)
	{
		strncpy(auth_tx.TLF01_details.card_holder_name,sMCF01.name02,strlen(sMCF01.name02));
	}
	
	/*PHani - POS is sending CUP MID, which should be populated as CUP ID and MID should be replaced*/

	strncpy(auth_tx.TLF01_details.visa_merchant_id,sMCF01.primary_key.merchant_id,sizeof(auth_tx.TLF01_details.merchant_id));
	
 	/* For EFT Voice transactions, use MCC that came in the request. */
   	if ( auth_tx.TLF01_details.category_code[0] == 0x00 )
   	{
   	   strcpy(auth_tx.TLF01_details.category_code,sMCF01.voice_category_code);
   	}
	/* GET MERCHANT A\name and name01 to tlf01.cardholdername*/

   	if(strncmp(sMCF01.network[7].priority_routing,'Y',1))
   	{
   		strncpy(auth_tx.TLF01_details.period,sMCF01.network[7].priority_routing,1);
   	}
	
	/* Validate merchant status and the merchant blocking dates */
	retCode = Edit_MCF01_Record (sMCF01) ;
	if (retCode != DEVICE_OK)
		return retCode ;
	
	/*	If the transaction is a SETTLEMENT or a SETTLEMENT_TRAILER
		and if the batch number in the transaction is the same as that found in the
		device, it means that that terminal may have already settled. The BCH01 record 
		will be retreived to check the totals against the incoming request totals. If
		the totals match, the terminal batch is a duplicate of a previously settled batch.
		If the totals do not match, the batch will be accepted.
	*/
   if (auth_tx.TLF01_details.tx_key == AUTH_SETTLE || 
	   auth_tx.TLF01_details.tx_key == AUTH_SETTLE_TRAILER)
   {
	  if (strEqual (auth_tx.TLF01_details.voice_auth_text, auth_tx.TLF01_details.batch_number)) /*LM 09/29/03 SCR1220 - Duplicate Batch Processing*/
	  {
		 /* Yes. Get the previous batch to check batch totals for duplication.
			Check if its a MP transaction settlement request.
			If Merchant is having a valid Merchant group is its MP Settlement transaction
		 */
		if(MGIDlen > 0)
		{
			if(true == isalphanumSpace(sMCF01.network[5].priority_routing,MGIDlen ))
			{
				
                if(0 == strncmp(sMCF01.status,"R", 1))
                {
                	strcpy( auth_tx.TLF01_details.response_code, "05" );
					strcpy(auth_tx.TLF01_details.response_text, "RESTRICTED Merchant");
					return (DEVICE_ERROR);

                }
                else
                {
					 retCode = Send_Request_To_Service (  "dcpimpA",
															MT_INCOMING_DCPISO,
															0,
															0,
															0,
															0,
															0) ;
					return (retCode);
                }
			}
			else
			{
				 memset( strError, 0x00, sizeof(strError) );
				 sprintf( strError,
						 "Invalid Merchant Group Id for Merchant: %s with Merchant Group ID %s",
						 auth_tx.TLF01_details.merchant_id,
						 sMCF01.network[5].priority_routing, 
						 NULL);
				 dcpiso_log_message( 2, 3, strError, "Process_select_mcf01_db_OK",1 );
				 return (DEVICE_ERROR) ;
			}
		}
		else
		{
			retCode = Get_BCH01_Details(ST2_DB_SELECT_BCH01_BY_BATCH_NBR) ;
		 	return (retCode);
		}
	  }
   }

	switch (auth_tx.TLF01_details.tx_key)
	{
		case AUTH_VOID_SALE:

         emv_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
         if ( emv_len > 0 )
         {
            memset( temp_str, 0x00, sizeof(temp_str) );
            hex_tag[0] = 0x9F;
            hex_tag[1] = 0x5B;
            ret_val = genutil_get_emv_tag( hex_tag,
                                           auth_tx.EMV_details.emv_block+2,
                                           emv_len, temp_str, &len );
            if ( ret_val == true )
            {
               /* Save Tag 9F5B so it is not overwritten with orig EMV data. */
               memcpy( auth_tx.BON01_details.response_message, hex_tag, 2 );
               auth_tx.BON01_details.response_message[2] = len;
               memcpy( auth_tx.BON01_details.response_message+3, temp_str, len);
            }
         }

		case AUTH_VOID_REFUND:
		case AUTH_SALE_ADJUSTMENT:
		case AUTH_REFUND_ADJUSTMENT:

			/*	Adjustments carry the RRN of the transaction it is adjusting
				So when a Adjustment is received, we check if we have the original
				transaction is in BCH10 (by the RRN). If the original transaction does
				not exist in BCH10 (by RRN), we cannot proceed with any adjustment and
				we just return. If the original transaction exists in BCH10 (by RRN), we
				check to see if the STANs are the same, we know that the Adjustment is a 
				REPEAT, and we just return, else we send the transaction to the authorizer
			*/

			retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_RRN) ;
			return (retCode) ;
		break ;

		case AUTH_OFFLINE_VOID_SALE: /* Void of Offline Sale */
			strcpy (strError, "Transaction is a VOID of OFFLINE SALE\n") ;
		case AUTH_OFFLINE_VOID_REFUND: /* Void of Offline Refund */
				strcpy (strError, "Transaction is a VOID of OFFLINE REFUND\n") ;

				if (strlen (auth_tx.TLF01_details.orig_retrieval_ref_num) > 0)
				{	/* This is the case when the offline sale/refund was piggbacked by a
					on line sale (0220 02) */
					retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_RRN) ;
					return (retCode) ;
				}
				else
				{
					/* This is the case when the offline sale and the void are collapsed into
					one transaction */
					retCode = Send_Transaction_To_Authorizer () ;
					return (retCode) ;
				}
		break ;
		
		case AUTH_REVERSAL:
         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( strError, 0x00, sizeof(strError) );
         sprintf( strError,
                 "Reversal from a local device: %s.Merchant %s",
                  auth_tx.TLF01_details.terminal_id,
                  auth_tx.TLF01_details.merchant_id );
         dcpiso_log_message( 2, 1, strError, "Process_select_mcf01_db_OK",1 );
		case AUTH_OFFLINE_REFUND:

			/*	For AUTH_REVERSAL, we check if the original transaction exists in 
				BCH10 (the STAN of the original transaction is mirrored in the 
				REVERSAL transaction). If the original transaction exists in BCH10,
				we forward the transaction to Transaction Control. If the original
				transaction is not found in BCH10, then there is nothing to reverse
				and we send the transaction back to the terminal */

			/*	For AUTH_OFFLINE_SALE and AUTH_OFFLINE_REFUND, we check the BCH10 table
				by STAN to verify if the transaction is a REPEAT. If the transaction exists in
				BCH10, the transaction is a REPEAT and we send the transaction back to 
				the terminal. If the transaction does not exist in BCH10, we forward it
				to Transaction Control*/

				/*
					For CUP reversal , we will locate original transaction with the help of
					card number , terminal id, merchant id, and amount with latest one in search
				*/
				if(true == dcpiso_Check_If_CUP_Transaction())
				{
					if((0==strncmp(auth_tx.TLF01_details.message_type,"0400",4))&&
						(0==strncmp(auth_tx.TLF01_details.processing_code,"02",2)))
					{
						retCode = Get_BCH10_Details_Void_CUP (ST2_DB_SELECT_BCH10_FOR_CUP) ;
					}
					else
					{
					retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_FOR_CUP) ;
					}
				}
				else
				{
					retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_STAN) ;
				}
				return (retCode) ;

		break ;

		case AUTH_OFFLINE_SALE:

				/*For AUTH_OFFLINE_SALE we check the BCH10 table
				by STAN to verify if the transaction is a REPEAT. If the transaction exists in
				BCH10, the transaction is a REPEAT and we send the transaction back to
				the terminal. If the transaction does not exist in BCH10, we forward it
				to Transaction Control*/
				retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_STAN) ;
				return (retCode) ;
				break ;

		case AUTH_SALES_COMPLETION:
			/*	First check the BCH10 table by STAN to check if the SALES_COMPLETION
			*//* TF Phani-  */
			retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_STAN) ;
			/*retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER_CARD_NBR) ;*/
			return (retCode) ;
		break ;

		case AUTH_BATCH_UPLOAD:
			/*	Send request to read the BIN01 record 
				if successful, copy the settlement prefix, card_type, and card_family fields from
				the BIN01 record into tlf01
				if failure, then DO NOT RETURN but continue

				Then read the MCT01 table
				if successful copy the settlent_merchant_id from MCT01 into tlf01.settlement_id
				if failure, then DO NOT RETURN but continue

				Call Process_Batch_Upload after returning from the MCT01 database retrieval
			*/

			
			 /* Check if its a MP  Batch Upload transaction .
			 If Merchant is having a valid Merchant group is its MP Settlement request  */
		        
			if(MGIDlen > 0)
			{
				if(true == isalphanumSpace(sMCF01.network[5].priority_routing,MGIDlen ))
				{
					/*Send ALL MP tranasction to dcpimp module . Dcpiso will not receive response message for these transaction*/
					sprintf( strError, "Its A MP merchant with merchant group id %s ",merchant_group_id );
					dcpiso_log_message( 2, 1, strError, "Process_select_mcf01_db_OK",1 );
				    retCode = Send_Request_To_Service (  "dcpimpA",
														MT_INCOMING_DCPISO, 
														0, 
														0, 
														0, 
														0, 
														0) ;
					return (retCode);
				}
				else
				{
					 memset( strError, 0x00, sizeof(strError) );
					 sprintf( strError,
							 "Invalid Merchant Group Id for Merchant: %s with Merchant Group ID %s",
							 auth_tx.TLF01_details.merchant_id,
							 sMCF01.network[5].priority_routing, 
							 NULL);
					 dcpiso_log_message( 2, 3, strError, "Process_select_mcf01_db_OK",1 );
					 return(DEVICE_ERROR) ;
				}
			}
			else
			{
				strcpy (strError, "\nThis is a batch upload transaction\n") ;
				PRINT (strError) ;
				/*retCode = Retrieve_BIN01_Record () ;*/
				retCode = Process_Batch_Upload () ;
				return (retCode) ;
			}
        break;
      
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_CARD_VERIFICATION:
			if( true == dcpiso_check_If_Tra_qualify_for_incremental_Preauth () ||
			    true == dcpiso_Check_If_CUP_Transaction())
			{
				/* Lets check if it qualifies for incremental Preauth.*/
				retCode=Get_BCH20_Details_for_incremental_Preauth(ST2_DB_SELECT_BCH20_FOR_INCREMENTAL_PREAUTH);
				return (retCode) ;
			}
			else
			{
				/* for all other card brand transactions */
				retCode = Send_Transaction_To_Authorizer () ;
				return (retCode) ;
			}
			break;

		case AUTH_AUTHORIZATION:


#if AMEX03B_APRIL2024
			if(true == dcpiso_check_If_AMEX_Transaction() &&
			  (auth_tx.mac_key[0] ==AUTH_PRE_AUTHORIZATION  ||
			   auth_tx.mac_key[0] ==AUTH_CARD_VERIFICATION ))
			{
				/*For amex, the pre-auth and card verification transaction were changed to AUTH_AUTHORIZATION */
				retCode=Get_BCH20_Details_for_incremental_Preauth(ST2_DB_SELECT_BCH20_FOR_ESTIMATED_AUTHORIZATION);
			}
			else
#endif

			{/* for all other card brand transactions */
				retCode = Send_Transaction_To_Authorizer () ;
			}
			return (retCode) ;
			break;

		case AUTH_SETTLE:
		case AUTH_SETTLE_TRAILER:

			 /* Check if its a MP settlement transaction.
			 If Merchant is having a valid Merchant group is its MP Settlement request */
		        
			if(MGIDlen > 0)
			{
				if(true == isalphanumSpace(sMCF01.network[5].priority_routing,MGIDlen ))
				{
					if(0 == strncmp(sMCF01.status,"R", 1))
					   {
						   strcpy( auth_tx.TLF01_details.response_code, "05" );
						   strcpy( auth_tx.TLF01_details.response_text, "RESTRICTED Merchant");
						   return (DEVICE_ERROR);

					   }
					   else
					   {
							retCode = Send_Request_To_Service (  "dcpimpA",
																MT_INCOMING_DCPISO,
																0,
																0,
																0,
																0,
																0) ;
							return (retCode);
					   }
				}
				else
				{
					 memset( strError, 0x00, sizeof(strError) );
					 sprintf( strError,
							 "Invalid Merchant Group Id for Merchant: %s with Merchant Group ID %s",
							 auth_tx.TLF01_details.merchant_id,
							 sMCF01.network[5].priority_routing, 
							 NULL);
					 dcpiso_log_message( 2, 3, strError, "Process_select_mcf01_db_OK",1 );
					 return(DEVICE_ERROR) ;
				}
			}
			else
			{
				strcpy (strError, "Transaction is AUTH_SETTLE/AUTH_SETTLE_TRAILER - Getting BCH01\n") ;
				PRINT (strError) ;
				retCode = Get_BCH01_Details (ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS) ; 
				return (retCode) ;
			}
		break;

#if BDOR_62_PREAUTH_VOID

		case AUTH_VOID_PRE_AUTHORIZATION:

			if( true == dcpiso_Check_If_CUP_Transaction())
			{
				/*For UPI Auth Preauth Cancel is not supported*/
				memset( strError, 0x00, sizeof(strError) );
				memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
				strcpy( strError,"UPI does not support Auth Preauth Void");
				strcpy(auth_tx.TLF01_details.response_text,UPI_AUTH_PREAUT_VOID_ERROR_RESP_TEXT);
				strncpy(auth_tx.TLF01_details.response_code,UPI_PREAUTH_VOID_RESPONSE,ISO_RESPONSE_CODE_LEN);
				dcpiso_log_message( 2, 3, strError, "Process_select_mcf01_db_OK",1 );
				return (UPI_AUTH_PRE_AUTHORIZATION_VOID_ERROR);
			}
			else
			{
				/*We need to find the original Preauth in BCH20 table */
				retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_FOR_PREAUTH_VOID) ;
				return (retCode);
			}
			break;
#endif


		default:
			/* for all other transactions */
			retCode = Send_Transaction_To_Authorizer () ;
			return (retCode) ;
	}

	return(DEVICE_OK) ;
} /* Process_select_mcf01_db_OK */

BYTE Process_select_bin01_db_OK (pBYTE pCurrent)
{
	BYTE retCode = 0 ;

	BIN01 sBIN01 ;

	memset (&sBIN01, 0, sizeof sBIN01) ;
	memcpy (&sBIN01, pCurrent+ sizeof auth_tx, sizeof sBIN01) ;
	memcpy (&auth_tx.BIN01_details, &sBIN01, sizeof sBIN01) ;


	retCode = Process_When_Bin_Range_Found () ;
	return retCode ; /* returns SYSTEM_ERROR for any system related error */

} /* Process_select_bin01_db_OK */


BYTE Process_select_mct01_db_OK (pBYTE pCurrent)
{
	MCT01	sMCT01 ;
	BYTE	retCode = 0 ;
	BYTE	strError[512] = {0} ;
	memset (&sMCT01, 0, sizeof sMCT01) ;
	memcpy (&sMCT01, pCurrent+ sizeof auth_tx, sizeof sMCT01) ;

	strcpy (strError, "Found MCT01 record during batch upload\n") ;
	PRINT (strError) ;

	if (strlen (sMCT01.settlement_id))
	{
		strcpy (auth_tx.TLF01_details.visa_merchant_id, sMCT01.settlement_id) ;
	}

	retCode = Process_Batch_Upload () ; 
	return retCode ;
} /* Process_select_mct01_db_OK */

BYTE Process_select_bch10_db_OK (pBYTE pCurrent)
{
   BYTE  retCode = DEVICE_OK ;
   BCH10 sBCH10 ;
   BYTE  strError[512] = {0} ;
   BYTE  bintype;
   CHAR  time_date[25];
   CHAR  date_str[9];
   CHAR  time_str[7];
   CHAR  temp_str[6];
   BYTE  temp_bcd[2];
   INT   len;
   INT   emv_len;

   memset (&sBCH10, 0, sizeof sBCH10) ;
   memcpy (&sBCH10, pCurrent + sizeof auth_tx, sizeof sBCH10) ;

   strcpy (auth_tx.TLF01_details.orig_amount,            sBCH10.tran_amount) ;
   strcpy (auth_tx.TLF01_details.orig_retrieval_ref_num, sBCH10.primary_key.retrieval_ref_num) ;
   strcpy (auth_tx.TLF01_details.auth_number,            sBCH10.auth_number) ;
   strcpy (auth_tx.TLF01_details.orig_message,           sBCH10.message_type) ;
   strcpy (auth_tx.TLF01_details.orig_sys_trace_num,     sBCH10.sys_trace_audit_num) ;

   /* The following are for the network reversals */
   if(true != dcpiso_check_If_Diners_Transaction())
   {
	   strcpy (auth_tx.TLF01_details.transmission_timestamp, sBCH10.transmission_timestamp) ;
   }
   if (true == dcpiso_check_If_Diners_Transaction())
   {
	   strcpy (auth_tx.TLF01_details.product_codes[12].amount,
	  			   sBCH10.product_codes[12].amount) ;
   }
   strcpy (auth_tx.orig_local_time_date,                 sBCH10.transmission_timestamp) ;
   strcpy (auth_tx.TLF01_details.mcard_banknet,          sBCH10.mcard_banknet) ;

   /* For Cirrus */
   strcpy (auth_tx.TLF01_details.settlement_date,        sBCH10.product_codes[19].code) ;

   /* For Amex - YYMMDDhhmmss */
   strcpy( auth_tx.local_date_time, sBCH10.tran_date+2 );
   strcat( auth_tx.local_date_time, sBCH10.tran_time   );
   /* For STAND IN transaction, we need to check whether transaction is apporved stand in or not
		Based on that we need process.. TF PHANI   	*/
   strcpy (auth_tx.TLF01_details.product_codes[1].code,   sBCH10.product_codes[1].code) ;	
   if(strlen(sBCH10.product_codes[6].amount)> 3)
   strncpy (auth_tx.TLF01_details.service_code,   sBCH10.product_codes[6].amount+2,3) ;	
   else
   strncpy (auth_tx.TLF01_details.service_code,   sBCH10.product_codes[6].amount,3) ;	

   if(true == dcpiso_check_If_Diners_Transaction())
   {
	   len = strlen (sBCH10.product_codes[13].amount);
	   if(len > 0 )
	   {
		   memcpy(auth_tx.TLF01_details.resp_source,
				   sBCH10.product_codes[13].code,6);
		   memcpy(auth_tx.TLF01_details.resp_source +6,
				   sBCH10.product_codes[13].amount,len);
	   }
	   else
	   {
		   len = strlen (sBCH10.product_codes[13].code);
		   memcpy(auth_tx.TLF01_details.resp_source,
		   		   sBCH10.product_codes[13].code,len);
	   }
   }

   switch (auth_tx.TLF01_details.tx_key)
   {
      case AUTH_REVERSAL:

		  /* reversal is for reload then just reply and log */
		  if ( sBCH10.tx_key == AUTH_RELOAD_CONFIRM)
		  {
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
			Insert_TLF01_Details( ST2_NONE ); 
			return SEND_RESPONSE_TO_ORIGINATOR ;
		  }

         /* Original transaction for which the REVERSAL is meant for, has been found in BCH10.
          * Set flag so when it comes back from authorizer,it will not get inserted into TLF01.
          * This will happen after exiting this function.*/

         bintype = Get_Bin_Type( auth_tx.TLF01_details.processing_code );

         /* Cash advance->void sale->void reversal scenario handling for processing code*/
         strncpy(auth_tx.TLF01_details.product_codes[11].amount,sBCH10.product_codes[7].amount,6);
#ifdef MC02_APR2024
         if(true == dcpiso_check_If_MC_Transaction() && strlen(sBCH10.product_codes[10].amount) > 0)
         {
        	 memcpy(auth_tx.TLF01_details.product_codes[10].amount,sBCH10.product_codes[10].amount,10);
        	 memcpy(auth_tx.TLF01_details.product_codes[1].amount,sBCH10.product_codes[1].amount,12);
         }
#endif
         if ( bintype == CREDIT_BIN )
            auth_tx.host2_acquirer_cb_code[0] = '*';

         retCode = Send_Transaction_To_Authorizer () ;
         auth_tx.host2_acquirer_cb_code[0] = 0x00;

         /* Send response to terminal in case reversal request times
          * out at the host.  We do not want to lock the terminal.
          * When the reversal response comes back to dcpiso, we will
          * update the database, then drop the transaction.
          */
         if ( bintype == CREDIT_BIN )
         {
            /* Do this only for credit reversals. */
            strcpy( auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED );
            retCode = SEND_RESPONSE_TO_ORIGINATOR ;
         }
      break ;

      case AUTH_SALES_COMPLETION:
         /* At this stage, the SALES_COMPLETION transaction is a REPEAT */
         strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
         retCode = SEND_RESPONSE_TO_ORIGINATOR ;
      break ;

      case AUTH_VOID_SALE:
      case AUTH_VOID_REFUND:

         if ( sBCH10.tx_key == AUTH_RELOAD )
         {
            if ( void_reload_timeout == 0)
            {
               auth_tx.TLF01_details.tx_key = AUTH_VOID_RELOAD;
            }
            else
            {
               BYTE  current_datetime [20]= {0}, reload_trans_datetime [20]={0};
               INT delta =0;

               /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
               memset( time_date, 0x00, sizeof(time_date) );
               memset( date_str,  0x00, sizeof(date_str) );
               memset( time_str,  0x00, sizeof(time_str) );
               ptetime_get_timestamp( time_date );

               /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
               get_date( time_date, date_str );
               get_time( time_date, time_str );

               /* This is the time of the Reload Void transaction. */
               strcpy( current_datetime, date_str );
               strcat( current_datetime, time_str );

               /* This is the time of the original Reload transaction. */
               strcpy(reload_trans_datetime, sBCH10.tran_date) ;
               strcat(reload_trans_datetime, sBCH10.tran_time) ;

               delta = get_timediff( current_datetime, reload_trans_datetime );

               if (delta > 0 && delta < void_reload_timeout)
               {
                  auth_tx.TLF01_details.tx_key = AUTH_VOID_RELOAD;

                  /* We have the orig Reload txn from BCH10.
                   * Now, go get Reload Confirm txn from TLF01.
                   * We need the STAN from it for EB Host (nceqit).
                   */
                  retCode = Get_TLF01_Details( ST2_DB_SELECT_TRAN_BY_CARD_NUM );
                  return retCode ;
               }
               else
               {
                  /* Too much time between Reload and Reload Void. */
                  sprintf( strError,
                          "Late Reload Void1: delta=%d, allowed time=%d",
                           delta, void_reload_timeout );
                  dcpiso_log_message( 1, 1, strError, "Process_select_bch10_db_OK",1 );
                  sprintf( strError,
                          "Late Reload Void2: Void time=%s, Reload time=%s",
                           current_datetime, reload_trans_datetime );
                  dcpiso_log_message( 1, 1, strError, "Process_select_bch10_db_OK",1 );
                  strcpy(auth_tx.TLF01_details.response_code, INVALID_TRANSACTION);
                  Insert_TLF01_Details( ST2_NONE ); 
                  return SEND_RESPONSE_TO_ORIGINATOR ;
               }
            }
         }

         /* You are getting second time the reload approve and respond*/
         else if ( sBCH10.tx_key == AUTH_VOID_RELOAD)
         {
            strcpy( auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED );
            retCode = SEND_RESPONSE_TO_ORIGINATOR ;
            Insert_TLF01_Details( ST2_NONE ); 
            return retCode;			 
         }

         /* Recall that these could be OFFLINE VOID of an ONLINE SALE/REFUND */

         /* Original transaction, for which the VOID
          * is meant for, has been found in BCH10.
          */
         if ( auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE )
         {
            if ( auth_tx.BON01_details.response_message[0] != 0x00 )
            {
               /* In here lies EMV Tag 9f5b for a void.
                * Concatenate it to the end of the EMV field.
                */
               len = auth_tx.BON01_details.response_message[2] + 3;
               emv_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
               memcpy( auth_tx.EMV_details.emv_block+2+emv_len,
                       auth_tx.BON01_details.response_message,
                       len );

               /* Update the field length. */
               emv_len += len;
               memset( temp_str, 0x00, sizeof(temp_str) );
               memset( temp_bcd, 0x00, sizeof(temp_bcd) );
               sprintf( temp_str, "%04d", emv_len );
               genutil_str_asc_to_bcd( temp_str, 4, temp_bcd );
               memcpy( auth_tx.EMV_details.emv_block, temp_bcd, 2 );
            }
            if(true == dcpiso_Check_If_CUP_Transaction())
			{
			   if(0 == strncmp(sBCH10.product_codes[10].quantity,
					   CUP_VOID_RESP_FLAG_VAL,
					   CUP_VOID_RESP_FLAG_VAL_LEN))
			   {
				   // Its duplicate void so we need to respond to terminal
				   // We need to override the Original stand.
				   strncpy (auth_tx.TLF01_details.response_code,
						    TRANSACTION_APPROVED,
							ISO_RESPONSE_CODE_LEN) ;
				   return SEND_RESPONSE_TO_ORIGINATOR;
			   }
			}

			/* For CUP we will wait for the response coming from host and forward the same . 
			For other we will Respond to Voids immediately; they're dropped after update all */
            if(true != dcpiso_Check_If_CUP_Transaction())
			{
				auth_tx.host2_acquirer_cb_code[0] = '*';
			}
#ifdef MC02_APR2024
            if(true == dcpiso_check_If_MC_Transaction() && strlen(sBCH10.product_codes[10].amount) > 0)
            {
           	 memcpy(auth_tx.TLF01_details.product_codes[10].amount,sBCH10.product_codes[10].amount,10);
           	 memcpy(auth_tx.TLF01_details.product_codes[1].amount,sBCH10.product_codes[1].amount,12);
            }
#endif
            retCode = Send_Transaction_To_Authorizer () ;
            auth_tx.host2_acquirer_cb_code[0] = 0x00;

			/* For CUP we will wait for the response coming from host and forward the same . 
			For other we will Respond to Voids immediately; they're dropped after update all */
            if(true == dcpiso_Check_If_CUP_Transaction())
			{
   			   retCode = DEVICE_OK;
			}
			else
			{
            	strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED);
           		retCode = SEND_RESPONSE_TO_ORIGINATOR;
         	}
         }
         else
         {
            retCode = Send_Transaction_To_Authorizer () ;
         }
      break ;

      case AUTH_OFFLINE_SALE:
      case AUTH_OFFLINE_REFUND:

         /* The transaction has been found in BCH10 by
          * STAN - transaction is a REPEAT.
          */
         strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
         retCode = SEND_RESPONSE_TO_ORIGINATOR ;
      break ;

      /* Recall that these are OFFLINE SALES with 0 amount but piggybacked*/
      case AUTH_OFFLINE_VOID_SALE:
      case AUTH_OFFLINE_VOID_REFUND:

      case AUTH_SALE_ADJUSTMENT:
      case AUTH_REFUND_ADJUSTMENT:
         /* The original transaction for which the ADJUSTMENT is meant for,
          * has been found in BCH10 (by RRN). We have to now check if
          * STANs are the same. If the STANs are the same, then the ADJUSTMENT
          * is a REPEAT and we just return. If not we send the transaction to
          * the authorizer.
          */
         if (strEqual (sBCH10.sys_trace_audit_num,
                       auth_tx.TLF01_details.sys_trace_audit_num))
         {
            /* The ADJUSTMENT is a REPEAT */
            strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
            retCode = SEND_RESPONSE_TO_ORIGINATOR ;
         }
         else
         {
            /* The original transaction for which the ADJUSTMENT is meant for
             * has been found in BCH10 (by RRN) and is not a REPEAT, so send
             * transaction to the authorizer.
             */
            retCode = Send_Transaction_To_Authorizer () ;
         }
      break ;
   }

   return (retCode) ;
}/* threqust_process_select_bch10_db_reply */

BYTE Process_select_bch20_db_OK (pBYTE pCurrent)
{
   BYTE retCode = 0 ;
   INT len = 0;
   BCH20 sBCH20 ;
   BYTE temp_offline_auth_amount[13] = {0};
   BYTE temp_total_Preauth_auth_amount[13] = {0};
   CHAR  gmt_time[20] = {0};
   double offline_auth_amount = 0.0;
   double overlimit_auth_amount = 0.0;
   double total_Preauth_auth_amount = 0.0;
   int percnet_overlimit_value = 0;
   CHAR strError[256] = {0};

   memset (&sBCH20, 0, sizeof sBCH20) ;
   memcpy (&sBCH20, pCurrent + sizeof auth_tx, sizeof sBCH20) ;

   strcpy (auth_tx.TLF01_details.orig_amount,            sBCH20.tran_amount) ;
   strcpy (auth_tx.TLF01_details.orig_retrieval_ref_num, sBCH20.primary_key.retrieval_ref_num) ;
   strcpy (auth_tx.TLF01_details.auth_number,            sBCH20.auth_number) ;
   strcpy (auth_tx.TLF01_details.orig_message,           sBCH20.message_type) ;
   strcpy (auth_tx.TLF01_details.orig_sys_trace_num,     sBCH20.sys_trace_audit_num) ;
   strcpy (auth_tx.host2_message_seq_nbr,                sBCH20.product_codes[6].code);
   strncpy(auth_tx.TLF01_details.product_codes[10].code, sBCH20.pos_entry_mode, 4);

   /* The following two are for the network reversals */

#if BDOR_62_PREAUTH_VOID
   if(0 == strncmp(sBCH20.product_codes[4].code,USED, TRAN_STATUS_LEN) &&
	  true == Check_If_Transaction_qualifies_for_Preauth_validation())
   {
	    Process_get_response_text_for_preauth_void(&strError);
   		strncpy(auth_tx.TLF01_details.response_text,strError,
   				(sizeof(auth_tx.TLF01_details.response_text)-1));
   		dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK",1 );
   		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
   		prepare_authtx_for_forwarding_preauth_void_original_processed ();
   		return (DEVICE_ERROR) ;
   	}

#endif

	if(true != dcpiso_check_If_Diners_Transaction())
	{
		strcpy (auth_tx.TLF01_details.transmission_timestamp, sBCH20.transmission_timestamp) ;
	}
	if (true == dcpiso_check_If_Diners_Transaction())
    {
		strcpy (auth_tx.TLF01_details.product_codes[12].amount,
				   sBCH20.product_codes[12].amount) ;

		if ((0 == strlen(auth_tx.host2_message_seq_nbr) &&
			(0 == strlen(auth_tx.EMV_details.pan_sequence_number))))
		{
			strcpy (auth_tx.TLF01_details.product_codes[11].code,
							   sBCH20.product_codes[11].code) ;
		}

		strncpy (auth_tx.TLF01_details.product_codes[14].code, auth_tx.TLF01_details.pos_entry_mode, 4);
		strcpy (auth_tx.TLF01_details.pos_entry_mode,sBCH20.pos_entry_mode);

    }

	if (true == dcpiso_check_If_AMEX_Transaction())
    {
		strncpy (auth_tx.TLF01_details.product_codes[14].code, auth_tx.TLF01_details.pos_entry_mode, 4);
		strcpy (auth_tx.TLF01_details.pos_entry_mode,sBCH20.pos_entry_mode);
    }

   strcpy (auth_tx.orig_local_time_date,                 sBCH20.transmission_timestamp) ;
   strcpy (auth_tx.TLF01_details.mcard_banknet,          sBCH20.mcard_banknet) ;
   /* Settlement date from Host*/
   if(strlen(sBCH20.product_codes[19].code) > 0)
   {
	   strncpy(auth_tx.TLF01_details.settlement_date,       sBCH20.product_codes[19].code , (sizeof(auth_tx.TLF01_details.settlement_date)-1) );
   }
   /* For Amex - YYMMDDhhmmss */
   strcpy( auth_tx.local_date_time, sBCH20.tran_date+2 );
   strcat( auth_tx.local_date_time, sBCH20.tran_time   );

   /*Let us store the pos data*/
   strncpy(auth_tx.TLF01_details.product_codes[16].amount,
		   sBCH20.product_codes[16].amount,
		   strlen(sBCH20.product_codes[16].amount));

   if(0 == strncmp(auth_tx.TLF01_details.message_type,"0220",4))
   {
	   strncpy (auth_tx.TLF01_details.service_code,
					   sBCH20.product_codes[6].amount+2,
					   (sizeof(auth_tx.TLF01_details.service_code)-1)) ;
	   if(auth_tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE)
	   {
		   strcpy (auth_tx.TLF01_details.retrieval_ref_num,
				   	   sBCH20.primary_key.retrieval_ref_num) ;
	   }

	   if(true == dcpiso_check_If_VISA_Transaction())
	   {
		   auth_tx.TLF01_details.resp_source[0] =sBCH20.product_codes[0].code[0];
	   }
   }


	if (true == dcpiso_check_If_VISA_Transaction() &&
			0 == strncmp(sBCH20.product_codes[5].code, "OFFUS", 5))
  {

		strncpy(auth_tx.TLF01_details.product_codes[1].amount,sBCH20.product_codes[1].amount, 10);
		strncpy (auth_tx.TLF01_details.product_codes[5].quantity, sBCH20.product_codes[5].quantity, 2);
		strncpy(auth_tx.TLF01_details.product_codes[7].code, sBCH20.product_codes[8].code, 3);
		strncpy (auth_tx.TLF01_details.add_amounts, sBCH20.down_payment, 9);
		strncpy(auth_tx.TLF01_details.visa_validation_code,sBCH20.interest_rate, 4);
		strncpy (auth_tx.TLF01_details.currency_code, sBCH20.term, 3);


   }

	if(true == dcpiso_check_If_Diners_Transaction())
	{
		len = strlen (sBCH20.product_codes[13].amount);
		if(len > 0 )
		{
			memcpy(auth_tx.TLF01_details.resp_source,
					sBCH20.product_codes[13].code,6);
			memcpy(auth_tx.TLF01_details.resp_source +6,
					sBCH20.product_codes[13].amount,len);
		}
		else
		{
			len = strlen (sBCH20.product_codes[13].code);
			memcpy(auth_tx.TLF01_details.resp_source,
					sBCH20.product_codes[13].code,len);
		}
	}

   if (true == dcpiso_check_If_MC_Transaction()) 
   {
	   strcpy(auth_tx.TLF01_details.product_codes[5].code,sBCH20.product_codes[5].code);
#ifdef MC02_APR2024
	   if(strlen(sBCH20.product_codes[10].amount) > 0)
	   {
		   memcpy(auth_tx.TLF01_details.product_codes[10].amount, sBCH20.product_codes[10].amount, 10);
		   memcpy(auth_tx.TLF01_details.product_codes[1].amount, sBCH20.product_codes[1].amount, 12);
	   }
#endif
   }

   if ( auth_tx.TLF01_details.tx_key == AUTH_REVERSAL )
   {
	  if(0==strncmp(sBCH20.product_codes[7].code,INITIAL_PREAUTH_TRA,PREAUTH_TRA_LEN))
	  {
		  auth_tx.TLF01_details.saf[0] = PRIMARY_AUTH;
	  }
	  else
	  {
		  auth_tx.TLF01_details.saf[0] = INCREMENTAL_AUTH;
	  }
      auth_tx.host2_acquirer_cb_code[0] = '*';  /* Do not insert into TLF01 */
      retCode = Send_Transaction_To_Authorizer () ;
      auth_tx.host2_acquirer_cb_code[0] = 0x00; /* Do insert into TLF01 */

      /* Send response to terminal in case reversal request times
       * out at the host.  We do not want to lock the terminal.
       * When the reversal response comes back to dcpiso, we will
       * update the database, then drop the transaction.
       */
      if ( CREDIT_BIN == Get_Bin_Type(auth_tx.TLF01_details.processing_code))
      {
         /* Do this only for credit reversals. */
         strcpy( auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED );
         retCode = SEND_RESPONSE_TO_ORIGINATOR ;
      }
   }
   else
   {

	   /* We have to validate this before responding to it.*/
	   strncpy(temp_offline_auth_amount,
			   auth_tx.TLF01_details.total_amount,
			   (sizeof(temp_offline_auth_amount)-1));
	   offline_auth_amount= strtod (temp_offline_auth_amount, 0) / 100;

	   strncpy(temp_total_Preauth_auth_amount,
			   sBCH20.product_codes[4].amount,
	   		   (sizeof(temp_total_Preauth_auth_amount)-1));
	   total_Preauth_auth_amount= strtod (temp_total_Preauth_auth_amount, 0) / 100;

	   strncpy(auth_tx.TLF01_details.product_codes[19].amount,
			   sBCH20.product_codes[4].amount,12);

	   /*Let us check if Merchant is configure for Pre-Auth completion validation */

	   if( true == Check_If_Transaction_qualifies_for_Preauth_validation())
	   {
		   if(total_Preauth_auth_amount>=offline_auth_amount)
		   {
			   /* No need to verify the Pre Auth Over limited range Approve the transaction.*/
			   retCode = Send_Transaction_To_Authorizer () ;
			   return (retCode) ;
		   }
		   else
		   {
			   /* Lets check if OFFLINE Auth Mount is beyond the Pre Auth over limit range.*/
			   if(true == dcpiso_check_If_VISA_Transaction())
			   {
				   percnet_overlimit_value = atoi(percent_overlimit_value_for_VISA);
				   overlimit_auth_amount = (total_Preauth_auth_amount * percnet_overlimit_value)/100;
			   }
			   else if(true == dcpiso_check_If_MC_Transaction())
			   {
				   percnet_overlimit_value = atoi(percent_overlimit_value_for_MC);
				   overlimit_auth_amount = (total_Preauth_auth_amount * percnet_overlimit_value)/100;
			   }
#if AMEX03B_APRIL2024
			   else if(true == dcpiso_check_If_AMEX_Transaction())
			   {
					   /*Perform Preauth Validation login to generate the Over limit amount*/
					   percnet_overlimit_value = atoi(percent_overlimit_value_for_AMEX);
					   overlimit_auth_amount = (total_Preauth_auth_amount * percnet_overlimit_value)/100;
					   if(overlimit_auth_amount>=offline_auth_amount)
					   {
						   /* Lets forward to host*/
						   retCode = Send_Transaction_To_Authorizer () ;
						   return (retCode) ;
					   }
					   else
					   {
						   /* Reject transaction.*/
					   strncpy (auth_tx.TLF01_details.response_code,
							   insufficient_preauth_declined_rc_for_AMEX,
							   ISO_RESPONSE_CODE_LEN) ;

					   auth_tx.TLF01_details.general_status = GS_INSUFFICIENT_PREAUTH_AMOUNT ;
					   strncpy(auth_tx.TLF01_details.response_text, "Offline amount is greater than pre-auth",39);
						   return (DEVICE_ERROR);
					   }
				   }
#endif
			   else if(true == dcpiso_check_If_JCB_Transaction())
			   {
				   percnet_overlimit_value = atoi(percent_overlimit_value_for_JCB);
				   overlimit_auth_amount = (total_Preauth_auth_amount * percnet_overlimit_value)/100;
			   }
			   else if(true == dcpiso_Check_If_CUP_Transaction())
			   {
				   if (0==strncmp(Incremental_Auth_flag_CUP,"1",1))
				   {
				   percnet_overlimit_value = atoi(percent_overlimit_value_for_CUP);
				   overlimit_auth_amount = (total_Preauth_auth_amount * percnet_overlimit_value)/100;

			   if(overlimit_auth_amount>=offline_auth_amount)
			   {
				   /* Lets forward to host*/
				   retCode = Send_Transaction_To_Authorizer () ;
				   return (retCode) ;
			   }
			   else
			   {
					   /* Reject transaction.*/
						   strcpy( auth_tx.TLF01_details.response_code, insufficient_preauth_declined_rc_for_UPI );
						   strcpy(auth_tx.TLF01_details.response_text, "Beyond Over Limit");
						   /* No need to update the BCH20 status for declined case */
						   /* Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH); */
					   return (DEVICE_ERROR);
				   }
				   }
				   else
				   {
						retCode = Send_Transaction_To_Authorizer();
						return (retCode);
				   }
			   }
			   else if(true == dcpiso_check_If_Diners_Transaction())
			   {
				   percnet_overlimit_value = atoi(percent_overlimit_value_for_DINERS);
				   overlimit_auth_amount = (total_Preauth_auth_amount * percnet_overlimit_value)/100;
		   }

			   if(overlimit_auth_amount>=offline_auth_amount)
	   {
				   /* Lets forward to host*/

			   retCode = Send_Transaction_To_Authorizer () ;
			   return (retCode) ;
		   }
		   else
		   {
				   if (true == dcpiso_check_If_Tra_qualify_MC_512_incremental_Preauth())
				   {

			    /* Reject transaction.*/
			    if(true == dcpiso_check_If_MC_Transaction())
				{
					strncpy (auth_tx.TLF01_details.response_code,
							 insufficient_preauth_declined_rc_for_MC,
							 ISO_RESPONSE_CODE_LEN) ;
				}
				else if(true == dcpiso_check_If_VISA_Transaction())
				{
					strncpy (auth_tx.TLF01_details.response_code,
							 insufficient_preauth_declined_rc_for_VISA,
							 ISO_RESPONSE_CODE_LEN) ;
				}

				else if(true == dcpiso_check_If_Diners_Transaction())
				{
					strncpy (auth_tx.TLF01_details.response_code,
							 insufficient_preauth_declined_rc_for_DCI,
							 ISO_RESPONSE_CODE_LEN) ;
				}
				else if(true == dcpiso_check_If_JCB_Transaction())
				{
					strncpy (auth_tx.TLF01_details.response_code,
							 insufficient_preauth_declined_rc_for_JCB,
							 ISO_RESPONSE_CODE_LEN) ;
				}
			    auth_tx.TLF01_details.general_status = GS_INSUFFICIENT_PREAUTH_AMOUNT ;
			    strncpy(auth_tx.TLF01_details.response_text, "Offline amount is greater than pre-auth",39);
			   /*Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH_REVERSE);*/
			   return (DEVICE_ERROR);
		   }
			else
			{
					   /* Reject transaction.*/
						if(true == dcpiso_check_If_MC_Transaction())
						{
							strncpy (auth_tx.TLF01_details.response_code,
									 insufficient_preauth_declined_rc_for_MC,
									 ISO_RESPONSE_CODE_LEN) ;
						}
						else if(true == dcpiso_check_If_VISA_Transaction())
						{
							strncpy (auth_tx.TLF01_details.response_code,
									 insufficient_preauth_declined_rc_for_VISA,
									 ISO_RESPONSE_CODE_LEN) ;
						}

						else if(true == dcpiso_check_If_Diners_Transaction())
						{
							strncpy (auth_tx.TLF01_details.response_code,
									 insufficient_preauth_declined_rc_for_DCI,
									 ISO_RESPONSE_CODE_LEN) ;
						}
						else if(true == dcpiso_check_If_JCB_Transaction())
						{
							strncpy (auth_tx.TLF01_details.response_code,
									 insufficient_preauth_declined_rc_for_JCB,
									 ISO_RESPONSE_CODE_LEN) ;
						}
					   strcpy(auth_tx.TLF01_details.response_text, "Beyond Over Limit");
					   Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
				return (DEVICE_ERROR);
			}
		}
		   }
	   }
	   else
	   {
		   /*Merchant is not configure for Pre-Auth completion validation */
		   retCode = Send_Transaction_To_Authorizer () ;
	   }
   }
   return (retCode) ;
}/* Process_select_bch20_db_OK */

BYTE Process_select_bch20_db_OK_incremental_preauth (pBYTE pCurrent)
{
	BYTE retCode = 0 ;
	BCH20 sBCH20 ;
	INT visa_trn_id_len = 0;
	memset (&sBCH20, 0, sizeof sBCH20) ;
	memcpy (&sBCH20, pCurrent + sizeof auth_tx, sizeof sBCH20) ;

	if(true == dcpiso_check_If_VISA_Transaction())
	{
		visa_trn_id_len = strlen (sBCH20.product_codes[13].amount);
		if(visa_trn_id_len > 0 )
		{
		   memcpy(auth_tx.TLF01_details.visa_tran_id,
				   sBCH20.product_codes[13].code,6);
		   memcpy(auth_tx.TLF01_details.visa_tran_id +6,
				   sBCH20.product_codes[13].amount,visa_trn_id_len );
		}
		else
		{
		   visa_trn_id_len = strlen (sBCH20.product_codes[13].code);
		   memcpy(auth_tx.TLF01_details.visa_tran_id,
				   sBCH20.product_codes[13].code,visa_trn_id_len);
		}
	}
	else if (true == dcpiso_check_If_MC_Transaction())
	{
		strcpy(auth_tx.TLF01_details.mcard_banknet, sBCH20.mcard_banknet);

		memcpy(auth_tx.TLF01_details.settlement_date,
			   sBCH20.product_codes[19].code,
			   (sizeof(auth_tx.TLF01_details.settlement_date)-1));
#ifdef MC02_APR2024
		if(strlen(sBCH20.product_codes[10].amount) > 0)
		{
			memcpy(auth_tx.TLF01_details.product_codes[10].amount, sBCH20.product_codes[10].amount, 10);
			memcpy(auth_tx.TLF01_details.product_codes[1].amount, sBCH20.product_codes[1].amount, 12);

		}
#endif
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strcpy(auth_tx.TLF01_details.auth_number, sBCH20.auth_number);
		strcpy(auth_tx.TLF01_details.merchant_id, sBCH20.merchant_id);
		strcpy(auth_tx.TLF01_details.orig_message, sBCH20.message_type);
		strcpy(auth_tx.TLF01_details.orig_sys_trace_num, sBCH20.sys_trace_audit_num);
		strcpy(auth_tx.orig_local_time_date, sBCH20.transmission_timestamp);
	}
	else if (true == dcpiso_check_If_Diners_Transaction())
	{
		if (true == Check_Validation_DCI_Incremental_Pre_auth_trxn( sBCH20 ))
			auth_tx.TLF01_details.product_codes[18].quantity[0] = 'I';
	}
#if AMEX03B_APRIL2024
	else if(true == dcpiso_check_If_AMEX_Transaction() )
	{
		/* Copy the original TID for incremental auth*/
		strncpy(auth_tx.TLF01_details.visa_tran_id, sBCH20.mcard_banknet,15);
	}
#endif

	strcpy(auth_tx.TLF01_details.response_text,"incremental authorization");
	auth_tx.TLF01_details.saf[0]=INCREMENTAL_AUTH; /*Incremental authorization*/
	strncpy( auth_tx.TLF01_details.product_codes[3].quantity,sBCH20.product_codes[4].quantity,
			 PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG );
	retCode = Send_Transaction_To_Authorizer () ;
	return (retCode) ;

}/* Process_select_bch20_db_OK_incremental_preauth */


BYTE Process_select_bch20_db_OK_preauth_void (pBYTE pCurrent)
{
	BYTE retCode = 0 ;
	INT len = 0;
	BCH20 sBCH20 ;
	CHAR strError[256] = {0};

	memset (&sBCH20, 0, sizeof sBCH20) ;
	memcpy (&sBCH20, pCurrent + sizeof auth_tx, sizeof sBCH20) ;

	strcpy (auth_tx.TLF01_details.orig_amount,            sBCH20.tran_amount) ;
	strcpy (auth_tx.TLF01_details.orig_retrieval_ref_num, sBCH20.primary_key.retrieval_ref_num) ;
	strcpy (auth_tx.TLF01_details.auth_number,            sBCH20.auth_number) ;
	strcpy (auth_tx.TLF01_details.orig_message,           sBCH20.message_type) ;
	strcpy (auth_tx.TLF01_details.orig_sys_trace_num,     sBCH20.sys_trace_audit_num) ;
	strcpy (auth_tx.host2_message_seq_nbr,                sBCH20.product_codes[6].code);

	/* Retrieve the DE62 from original if not received with Void Pre-auth */
	strcpy (auth_tx.TLF01_details.invoice_number,   sBCH20.invoice_nbr);

	/* Retrieve the DE4 from original if not received with Void Pre-auth */
		strcpy (auth_tx.TLF01_details.total_amount,	sBCH20.tran_amount) ;


	/*We have found the records lets check if if the original preauth is
	 * "procsd" or "voided" or "revers" or "posted" or "expird" or its unprocessed */

	if(0 == strncmp(sBCH20.product_codes[4].code,PROCESSED, TRAN_STATUS_LEN))
	{
		strcpy (strError,"Original PreAuth already Processed");
		strncpy(auth_tx.TLF01_details.response_text,strError,
				(sizeof(auth_tx.TLF01_details.response_text)-1));
		dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK_preauth_void",1 );
		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
		prepare_authtx_for_forwarding_preauth_void_original_processed ();
		return (DEVICE_ERROR) ;
	}
	else if(0 == strncmp(sBCH20.product_codes[4].code,USED, TRAN_STATUS_LEN))
	{
		Process_get_response_text_for_preauth_void(&strError);
		strncpy(auth_tx.TLF01_details.response_text,strError,
				(sizeof(auth_tx.TLF01_details.response_text)-1));
		dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK_preauth_void",1 );
		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
		prepare_authtx_for_forwarding_preauth_void_original_posted();
		return (DEVICE_ERROR) ;

	}
	else if(0 == strncmp(sBCH20.product_codes[4].code,EXPIRED, TRAN_STATUS_LEN))
	{
		strcpy (strError,"Original PreAuth already Expired");
		strncpy(auth_tx.TLF01_details.response_text,strError,
				(sizeof(auth_tx.TLF01_details.response_text)-1));
		dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK_preauth_void",1 );
		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
		prepare_authtx_for_forwarding_preauth_void_original_expired();
		return (DEVICE_ERROR) ;

	}
	else if(0 == strncmp(sBCH20.product_codes[4].code,VOIDED, TRAN_STATUS_LEN))
	{
		strcpy (strError,"Original PreAuth already Voided");
		strncpy(auth_tx.TLF01_details.response_text,strError,
				(sizeof(auth_tx.TLF01_details.response_text)-1));
		dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK_preauth_void",1 );
		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
		prepare_authtx_for_forwarding_preauth_void_original_voided();
		return (DEVICE_ERROR) ;
	}
	else if(0 == strncmp(sBCH20.product_codes[4].code,REVERSED, TRAN_STATUS_LEN))
	{
		strcpy (strError,"Original PreAuth already Reversed");
		strncpy(auth_tx.TLF01_details.response_text,strError,
				(sizeof(auth_tx.TLF01_details.response_text)-1));
		dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK_preauth_void",1 );
		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
		prepare_authtx_for_forwarding_preauth_void_original_reversed();
		return (DEVICE_ERROR) ;
	}


	/* The following two are for the network reversals */
	if(true != dcpiso_check_If_Diners_Transaction())
	{
		strcpy (auth_tx.TLF01_details.transmission_timestamp, sBCH20.transmission_timestamp) ;
	}

	else if (true == dcpiso_check_If_Diners_Transaction())
	{
		strcpy (auth_tx.TLF01_details.product_codes[12].amount,
			   sBCH20.product_codes[12].amount) ;


		if ((0 == strlen(auth_tx.host2_message_seq_nbr) &&
				(0 == strlen(auth_tx.EMV_details.pan_sequence_number))))
		{
			strcpy (auth_tx.TLF01_details.product_codes[11].code,
						   sBCH20.product_codes[11].code) ;
		}

		strcpy (auth_tx.TLF01_details.pos_entry_mode,sBCH20.pos_entry_mode);
	}

	if (true == dcpiso_check_If_AMEX_Transaction())
	{
		strcpy (auth_tx.TLF01_details.pos_entry_mode,sBCH20.pos_entry_mode);
	}

	strcpy (auth_tx.orig_local_time_date,                 sBCH20.transmission_timestamp) ;
	strcpy (auth_tx.TLF01_details.mcard_banknet,          sBCH20.mcard_banknet) ;

	// Settlement date from Host
	if(strlen(sBCH20.product_codes[19].code) > 0)
	{
		strncpy(auth_tx.TLF01_details.settlement_date,       sBCH20.product_codes[19].code , (sizeof(auth_tx.TLF01_details.settlement_date)-1) );
	}
	/* For Amex - YYMMDDhhmmss */
	strcpy( auth_tx.local_date_time, sBCH20.tran_date+2 );
	strcat( auth_tx.local_date_time, sBCH20.tran_time   );

	/*Let us store the pos data*/
	strncpy(auth_tx.TLF01_details.product_codes[16].amount,
			sBCH20.product_codes[16].amount,
			strlen(sBCH20.product_codes[16].amount));

	if(true == dcpiso_check_If_Diners_Transaction())
	{
		len = strlen (sBCH20.product_codes[13].amount);
		if(len > 0 )
		{
			memcpy(auth_tx.TLF01_details.resp_source,
					sBCH20.product_codes[13].code,6);
			memcpy(auth_tx.TLF01_details.resp_source +6,
					sBCH20.product_codes[13].amount,len);
		}
		else
		{
			len = strlen (sBCH20.product_codes[13].code);
			memcpy(auth_tx.TLF01_details.resp_source,
					sBCH20.product_codes[13].code,len);
		}
	}

	if (true == dcpiso_check_If_MC_Transaction())
	{
		strcpy(auth_tx.TLF01_details.product_codes[5].code,sBCH20.product_codes[5].code);
		#ifdef MC02_APR2024
		if(strlen(sBCH20.product_codes[10].amount) > 0)
		{
			memcpy(auth_tx.TLF01_details.product_codes[10].amount, sBCH20.product_codes[10].amount, 10);
			memcpy(auth_tx.TLF01_details.product_codes[1].amount, sBCH20.product_codes[1].amount, 12);
		}
		#endif
	}

	if (true == dcpiso_check_If_AMEX_Transaction() ||
		true == dcpiso_check_If_Diners_Transaction())
	{
			strcpy (auth_tx.TLF01_details.total_amount,	sBCH20.tran_amount) ;
	}

	/* Retrieve DE4 if not received in the Void pre-auth request,
	 * use it to build terminal response */
	if (true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.total_amount, auth_tx.TLF01_details.orig_amount, 12);
	}

	/*Check if this Void Reversal for Original Preauth with No Incremental */
	if((0==strncmp(sBCH20.product_codes[7].code,INITIAL_PREAUTH_TRA,PREAUTH_TRA_LEN))&&
	   (0==strncmp(sBCH20.product_codes[4].quantity,INITIAL_PREAUTH_TRA_WITHOUT_INCREMENTAL,PREAUTH_TRA_LEN)))
	{
		auth_tx.TLF01_details.saf[0] = PRIMARY_AUTH;
		strncpy (auth_tx.TLF01_details.total_interest, auth_tx.TLF01_details.orig_amount, 12);
		retCode = Send_Transaction_To_Authorizer () ;
	}
	/*Check if this Void Reversal for Original Preauth with Incremental */
	else if((0==strncmp(sBCH20.product_codes[7].code,INITIAL_PREAUTH_TRA,PREAUTH_TRA_LEN))&&
			(0==strncmp(sBCH20.product_codes[4].quantity,INITIAL_PREAUTH_TRA_WITH_INCREMENTAL,PREAUTH_TRA_LEN)))
	{
		auth_tx.TLF01_details.saf[0] = PRIMARY_AUTH;
		if (true == dcpiso_check_If_MC_Transaction() ||
			true == dcpiso_check_If_VISA_Transaction () ||
			true == dcpiso_check_If_JCB_Transaction ())
		{
			retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT) ;
		}
		else
		{
			retCode = Send_Transaction_To_Authorizer ();
		}
	}
	else if((0==strncmp(sBCH20.product_codes[7].code,INCREMENTAL_PREAUTH_TRA,PREAUTH_TRA_LEN))&&
		    (0==strncmp(sBCH20.product_codes[4].quantity,INITIAL_PREAUTH_TRA_WITH_INCREMENTAL,PREAUTH_TRA_LEN)))
	{
		auth_tx.TLF01_details.saf[0] = INCREMENTAL_AUTH;
		/*Its Incrmental Preauth. Lets get total Preauth Amounr and send it along with void reversal amount*/
		retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT) ;
	}
	return (retCode) ;
}

void Process_get_response_text_for_preauth_void(char *responseText)
{
	if (true == dcpiso_check_if_mid_is_smcc() )
	{
		if(true == dcpiso_check_If_MC_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					smcc_maximum_validity_of_preauth_in_days_for_MC);
		}
		else if( true == dcpiso_check_If_VISA_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					smcc_maximum_validity_of_preauth_in_days_for_VISA);
		}
		else if(true == dcpiso_check_If_AMEX_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					smcc_maximum_validity_of_preauth_in_days_for_AMEX);
		}
		else if(true == dcpiso_check_If_JCB_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					smcc_maximum_validity_of_preauth_in_days_for_JCB);
		}
		else if(true == dcpiso_check_If_Diners_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					smcc_maximum_validity_of_preauth_in_days_for_DCI);
		}
		else if(true == dcpiso_Check_If_CUP_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					smcc_maximum_validity_of_preauth_in_days_for_UPI);
		}
	}
	else
	{
		if(true == dcpiso_check_If_MC_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					maximum_validity_of_preauth_in_days_for_MC);
		}
		else if( true == dcpiso_check_If_VISA_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					maximum_validity_of_preauth_in_days_for_VISA);
		}
		else if(true == dcpiso_check_If_AMEX_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					maximum_validity_of_preauth_in_days_for_AMEX);
		}
		else if(true == dcpiso_check_If_JCB_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					maximum_validity_of_preauth_in_days_for_JCB);
		}
		else if(true == dcpiso_check_If_Diners_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					maximum_validity_of_preauth_in_days_for_DCI);
		}
		else if(true == dcpiso_Check_If_CUP_Transaction())
		{
			sprintf(responseText,
					"Pre-Auth Reversed >%s days",
					maximum_validity_of_preauth_in_days_for_UPI);
		}
	}
}

BYTE Process_select_bch20_db_OK_preauth_total_amount (pBYTE pCurrent)
{
	BYTE retCode = 0 ;
	CHAR strInfo[200] = {0} ;
	CHAR total_preauth_amt[13] = {0};
	BCH20 sBCH20 ;

	memset (&sBCH20, 0, sizeof sBCH20) ;
	memcpy (&sBCH20, pCurrent + sizeof auth_tx, sizeof sBCH20) ;

	strncpy(total_preauth_amt,sBCH20.product_codes[4].amount,12);

	/* We need to send Total Amount to network link for sending Void for incremental transaction*/
	if(	0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		memset( strInfo, 0x00, sizeof(strInfo) );
		memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
		strcpy( strInfo,"UPI does not support Auth Preauth Void");
		strcpy(auth_tx.TLF01_details.response_text,UPI_AUTH_PREAUT_VOID_ERROR_RESP_TEXT);
		strncpy(auth_tx.TLF01_details.response_code,UPI_PREAUTH_VOID_RESPONSE,ISO_RESPONSE_CODE_LEN);
		dcpiso_log_message( 2, 3, strInfo, "Process_select_bch20_db_OK_preauth_total_amount",1 );
		return (DEVICE_ERROR) ;
	}
	else
	{
		sprintf(strInfo,"Total Pre-auth Amount is: %s",total_preauth_amt);
		dcpiso_log_message( 2, 1, strInfo,"Process_select_bch20_db_OK_preauth_total_amount",1 );
		strncpy(auth_tx.TLF01_details.total_interest ,total_preauth_amt,12);
	}

	if (true == dcpiso_check_If_AMEX_Transaction() ||
		true == dcpiso_check_If_Diners_Transaction())
	{
		strcpy (auth_tx.TLF01_details.total_amount,	sBCH20.tran_amount) ;
	}

	/* Retrieve DE4 if not received in the Void pre-auth request,
	 * use it to build terminal response */
	if (true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.total_amount,
						auth_tx.TLF01_details.total_interest, 12);
	}

	if((true == dcpiso_check_If_MC_Transaction()))
	{
		retCode = Send_Transaction_To_Authorizer () ;
	}
	else  if((true == dcpiso_check_If_VISA_Transaction()))
	{
		retCode = Send_Transaction_To_Authorizer () ;
	}
	else if((true == dcpiso_check_If_AMEX_Transaction()))
	{
		retCode = Send_Transaction_To_Authorizer () ;
	}
	else if((true == dcpiso_check_If_Diners_Transaction()))
	{
		retCode = Send_Transaction_To_Authorizer () ;
	}
	else if((true == dcpiso_check_If_JCB_Transaction()))
	{
		retCode = Send_Transaction_To_Authorizer () ;
	}
	return (retCode) ;
}

BYTE Process_select_bch01_db_OK (pBYTE pCurrent, BYTE subType2)
{
    BYTE retCode = 0 ;
	INT  result = 0;
	
	BCH01 sBCH01 ;

	memset (&sBCH01, 0, sizeof sBCH01);
	memcpy (&sBCH01, pCurrent+ sizeof auth_tx, sizeof sBCH01) ;

	/*	BCH01 record will always be found, because in the database function, if the
		BCH01 record is not found, it is created
	*/
															
    if ( subType2 == ST2_DB_SELECT_BCH01_BY_BATCH_NBR )			/*LM 09/29/03 SCR1220 - Duplicate Batch Processing*/
    {
      /* According to DCF01, this is a duplicate batch.
       * Perform further checks to see if it really is a duplicate.
       */
      result = is_batch_duplicate(sBCH01);

      /* Pass the 'duplicate' result into a function to process duplicates. */
      (void)Process_Duplicate_Batch( result );
    }
    else
    {
      /* Batch is not a duplicate. Continue processing settlement as normal. */
      retCode = Process_Settlement(sBCH01);
    }	

	return DEVICE_OK;
} /* threqust_process_select_bch01_db_reply */

BYTE Process_select_tlf01_db_OK (pBYTE pCurrent)
{
   BYTE  retCode = DEVICE_OK ;
   TLF01 sTLF01;
   BYTE  strError[512] = {0} ;

   memset( &sTLF01, 0, sizeof(TLF01) );
   memcpy( &sTLF01, pCurrent + sizeof auth_tx, sizeof(TLF01) );

   strcpy(auth_tx.TLF01_details.orig_sys_trace_num, sTLF01.sys_trace_audit_num);

   retCode = Send_Transaction_To_Authorizer();

   return retCode ;
}/* Process_select_tlf01_db_reply */

BYTE Process_update_bch01_db_OK (BYTE type) 
{
   BYTE buffer[100];
	CHAR date_time[50];
   CHAR date_str[9];
   CHAR time_str[7];

   if (type == BCH01_TOTALS_MATCHED)
   {
      Add_Response_Fields_To_Auth_Tx( auth_tx.TLF01_details.tx_key,
                                      TRANSACTION_APPROVED );
      auth_tx.TLF01_details.general_status = GS_TXN_GOOD_BATCH;

      /* At Equitable's request (SCR 1181), log a terminal's settlement. */
      memset( buffer,    0x00, sizeof(buffer)    );
      memset( date_time, 0x00, sizeof(date_time) );
      memset( date_str,  0x00, sizeof(date_str)  );
      memset( time_str,  0x00, sizeof(time_str)  );

      genutil_format_date( date_time ) ;
      memcpy( date_str, date_time,   8 );
      memcpy( time_str, date_time+8, 6 );

      if (auth_tx.TLF01_details.tx_key == AUTH_SETTLE )
      {
  		char old_batch_num[7];
  		memcpy (old_batch_num, auth_tx.TLF01_details.voice_auth_text, sizeof(old_batch_num)); old_batch_num[6] = '\0';
         sprintf( buffer,
                 "Terminal %s completed settlement at %s, %s. Old Batch: %s, New Batch: %s",
                  auth_tx.TLF01_details.terminal_id, date_str, time_str, old_batch_num, auth_tx.TLF01_details.batch_number);
		 type = Get_DCF01_Details (ST2_DB_SELECT_DCF01_AUTO_INIT) ;
      }
      else
      {
         /* Settlement Trailer - Batch upload just completed. */
		 char old_batch_num[7];
		 memcpy (old_batch_num, auth_tx.TLF01_details.voice_auth_text, sizeof(old_batch_num)); old_batch_num[6] = '\0';
         sprintf( buffer,
                 "Terminal %s completed batch upload at %s, %s. Old Batch: %s, New Batch: %s",
                  auth_tx.TLF01_details.terminal_id, date_str, time_str, old_batch_num, auth_tx.TLF01_details.batch_number );
		 type = Get_DCF01_Details (ST2_DB_SELECT_DCF01_AUTO_INIT) ;

      }
      dcpiso_log_message( 2, 1, buffer, "Process_update_bch01_db_OK",1 );
   }
   else if (type == BCH01_TOTALS_NOT_MATCHED)
   {
      Add_Response_Fields_To_Auth_Tx( auth_tx.TLF01_details.tx_key,
                                      INITIATE_BATCH_UPLOAD );
      auth_tx.TLF01_details.general_status = GS_TXN_REJECTED_BATCH ;
   }
   return type ;
} /* Process_update_bch01_db_OK */

BYTE Process_insert_bch10_db_OK (void)
{
	BYTE strError[512] = {0} ;

	strcpy (strError, "BCH10 inserted successfully during batch upload - sending 00 response to terminal\n") ;
	PRINT (strError) ;
	auth_tx.TLF01_details.tx_key = AUTH_BATCH_UPLOAD_RESPONSE ;
	strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
    return BCH10_INSERT_SUCCESSFUL ;
} /* Process_insert_bch10_db_OK */


BYTE process_duplicate_txn( void )
{
   CHAR strError[512] = {0} ;
   BYTE retCode = 0 ;

   /* This is a duplicate transaction.  Log it and return to originator. */
   sprintf( strError,
           "Duplicate txn. TID: %s, STAN: %s, Msg Type: %s",
            auth_tx.TLF01_details.terminal_id,
            auth_tx.TLF01_details.sys_trace_audit_num,
            auth_tx.TLF01_details.message_type );

   retCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, strError,
                                          "process_duplicate_txn",
                                           GS_TXN_SYSTEM_ERROR,
                                           REENTER_TRANSACTION) ;
	return retCode ;
} /* process_duplicate_txn */


BYTE Process_dcf01_db_not_found (pBYTE pCurrent)
{
   CHAR strError[512] = {0} ;
   BYTE retCode = 0 ;

   /* Either the device is not ours, or somehow it
    * was not uploaded to the database.
    */
   sprintf (strError,
           "Terminal %s not found in the database. ",
            auth_tx.TLF01_details.terminal_id) ;

   retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                          "Process_select_dcf01_db_not_found",
                                           GS_TXN_SYSTEM_ERROR,
                                           INVALID_TERMINAL_ID) ;
	return retCode ;
} /* Process_dcf01_db_not_found */

BYTE Process_mcf01_db_not_found (pBYTE pCurrent)
{
   CHAR strError[512] = {0} ;
   BYTE retCode = 0 ;

   /* Either the merchant is not ours, or somehow it
    * was not uploaded to the database.
    */
   sprintf (strError,
           "Merchant %s not found in the database\n",
            auth_tx.TLF01_details.merchant_id) ;
   retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                          "Process_select_mcf01_db_not_found",
                                           GS_TXN_SYSTEM_ERROR,
                                           INVALID_MERCHANT) ;
	return retCode ;
} /* Process_mcf01_db_not_found */

BYTE Process_bin01_db_not_found (pBYTE pCurrent)
{
	BYTE retCode = 0 ;
	CHAR strError[512] = {0} ;

	strcpy (strError, "BIN01 record NOT found during batch upload\n") ;
	PRINT (strError) ;
	retCode = Process_When_Bin_Range_Not_Found () ;
	return retCode ;
} /* Process_bin01_db_not_found */


BYTE Process_mct01_db_not_found (pBYTE pCurrent)
{
	BYTE	retCode = 0 ;
	BYTE	strError[512] = {0} ;

	strcpy (strError, "MCT01 record NOT found during batch upload\n") ;
	PRINT (strError) ;
	retCode = Process_Batch_Upload () ;
	return retCode ;
} /* Process_mct01_db_not_found */

BYTE Process_bch10_db_not_found (pPTE_MSG p_msg_in, pBYTE pCurrent)
{
    CHAR strError[200] = {0} ;
	BYTE retCode = 0 ;
    CHAR  gmt_time[20]={0};
	
	switch (auth_tx.TLF01_details.tx_key)
	{
		case AUTH_SALES_COMPLETION:
			/*	AUTH_SALES_COMPLETION is not a REPEAT. 
				Now we need to check if the original authorization for which the 
				SALES_COMPLETION was intended exists in BCH20
			*/
			retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_BY_RRN) ;
			return retCode ;

		break ;
		case AUTH_REVERSAL:

			/*	if the original transaction is in BCH10 then we have to reverse the 
				transaction	
			
				if the original transaction was not found in BCH10, check if the 
			    original transaction is in BCH20 (if the original transaction was a AUTH)

				
				if the original transaction is in BCH20 then we have to reverse the 
				AUTHORIZATION, 
				
				if not then there is nothing to reverse 
			*/
			/*
				For CUP reversal , we will locate original transaction with the help of
				card number , terminal id, merchant id, and amount with latest one in search
			*/
			if(true == dcpiso_Check_If_CUP_Transaction())
			{
				retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_FOR_CUP) ;
			}
#if BDOR_62_PREAUTH_VOID
			else if(strncmp(auth_tx.TLF01_details.processing_code, "02", 2) == 0)
			{
				retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_BY_RRN) ;
			}
#endif
			else
			{
				retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_BY_STAN) ;
			}
			return retCode ;
			/*
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
			return SEND_RESPONSE_TO_ORIGINATOR ;
			*/
		break ;

      case AUTH_VOID_SALE:
      case AUTH_VOID_REFUND:

         /* The original transaction was not found in BCH10.
          * If it is a Void of a Deferred, then need to look in BCH20.
          */
         if ( auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE )
         {
            retCode = Get_BCH20_Details( ST2_DB_SELECT_BCH20_BY_RRN );
         }
         else
         {
            strcpy(strError,
                  "Original Sale or Refund not found in active batch for the Online/Offline VOID") ;
            retCode = Generate_Usage_Error_Message (1,strError, 
                                                   "Process_bch10_db_not_found",
                                                    GS_TXN_BCH10_NOT_FOUND, 
                                                    REFER_TO_CARD_ISSUER) ;
         }
         return retCode ;
      break ;
		case AUTH_OFFLINE_VOID_SALE:
		case AUTH_OFFLINE_VOID_REFUND:

			/*	If the original transaction was not found in BCH10, it is an error
			*/
			strcpy (strError, "Current tx is VOID - UPLOAD OFFLINE SALE - Original OFFLINE Sale or Refund not found in active batch\n") ;
			retCode = Generate_Usage_Error_Message (1,strError, 
													"Process_bch10_db_not_found",
													GS_TXN_BCH10_NOT_FOUND, 
													REFER_TO_CARD_ISSUER) ;
			return retCode ;

		break ;
		
		case AUTH_OFFLINE_SALE:
					
			/*	These are not REPEATS, so now fetch the merchant record, if merchant is
				found, then send the transaction to the authorizer 
			*/
			/* Current Greenwich Mean Time for the transmission-to-host time. */
			if ( 0 == strcmp( NULL_STR,
							auth_tx.TLF01_details.transmission_timestamp ) )
			{
				 pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
				 strncpy( auth_tx.TLF01_details.transmission_timestamp,
						  &gmt_time[4], 10 );  /* MMDDhhmmss */
			}
			strcpy (strError, "Sending the OFFLINE/VOID OF SALE/REFUND to the authorizer\n") ;
			PRINT (strError) ;

			/*Let us check if Merchant is configure for Pre-Auth completion validation */
#if AMEX03B_APRIL2024
            if(true == dcpiso_check_If_AMEX_Transaction() &&
              (0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"Y",1)||
               0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"y",1)))
            {
            	retCode = Get_BCH20_Details(ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH);
            }
            /*if this block enabled make below if as else if */
            else
#endif
            if(((0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"Y",1)) ||
                 (0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"y",1))) ||
                (true == dcpiso_check_If_Tra_qualify_MC_512_incremental_Preauth ()))
            {
            	retCode = Get_BCH20_Details(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
            }
            else
            {
            	/*Merchant is not configure for Pre-Auth completion validation */
            	//retCode = Get_BCH20_Details(ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER_CARD_NBR);
            	retCode = Get_BCH20_Details(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
            }
			return retCode ;
		break ;

		case AUTH_OFFLINE_REFUND:

			/*	These are not REPEATS, so now fetch the merchant record, if merchant is
				found, then send the transaction to the authorizer
			*/
			/* Current Greenwich Mean Time for the transmission-to-host time. */
			if ( 0 == strcmp( NULL_STR,
							auth_tx.TLF01_details.transmission_timestamp ) )
			{
				 pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
				 strncpy( auth_tx.TLF01_details.transmission_timestamp,
						  &gmt_time[4], 10 );  /* MMDDhhmmss */
			}
			strcpy (strError, "Sending the OFFLINE/VOID OF SALE/REFUND to the authorizer\n") ;
			PRINT (strError) ;
			retCode = Send_Transaction_To_Authorizer () ;
			return retCode ;
		break ;

		case AUTH_SALE_ADJUSTMENT:
		case AUTH_REFUND_ADJUSTMENT:
			/*	The original transaction for which the adjustment was meant for was
				not found in BCH10, so we have nothing to adjust (this is an error 
				situation)
			*/
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
			return SEND_RESPONSE_TO_ORIGINATOR ;
		break ;
		default:
		
		
			/* the transaction was not found in bch10, so it needs to be
		       forwarded to txcntl 
		   
		       txcntl does no update any balances for AUTH_OFFLINE_SALE and
			   AUTH_OFFLINE_REFUND but it populates the transaction with 
			   card_family etc etc
		   
		   */
			retCode = Send_Transaction_To_Authorizer () ;
			return retCode ;
			
		break;
	}

	return DEVICE_OK ;
}/* Process_bch10_db_not_found */

BYTE Process_bch20_db_not_found (pPTE_MSG p_msg_in, pBYTE pCurrent)
{
	BYTE retCode = 0 ;
   CHAR Buff[100];
	
	switch (auth_tx.TLF01_details.tx_key)
	{
		case AUTH_REVERSAL:  /* BCH20 was retrieved by STAN */

         /* For a REVERSAL transaction, if the original transaction was an
          * AUTHORIZATION and was not found in BCH20, then it implies that
          * we never saw the transaction in either BCH10 or BCH20. We return
          * the transaction to the terminal with a 00 response code.
          */
         memset( Buff, 0x00, sizeof(Buff) );
         sprintf( Buff,
                 "Original txn for a reversal not found in database. Stan: %s",
                  auth_tx.TLF01_details.sys_trace_audit_num );
         dcpiso_log_message( 2, 1, Buff, "Process_bch20_db_not_found",1 );
         /* Send an approved response. */
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
			/* Set general status if original txn not found.
			   We changed while supporting void and reversal txn for MasterCard 
			   Let be it set for all brands. because this not make much changes.
			   and also easy to identify (09-76) 11 Nov,2006 TF-Ajay */
			auth_tx.TLF01_details.general_status= GS_ORIGINAL_TXN_NOT_FOUND_FOR_REVERSAL ;
			return SEND_RESPONSE_TO_ORIGINATOR ;
		break ;
		case AUTH_SALES_COMPLETION: /* BCH20 was retrieved by RRN */
			/* The original AUTH/PRE_AUTH was not found in BCH20, so we change the SALES
			COMPLETION transaction to a AUTH_OFFLINE_SALE and process it like a AUTH_
			OFFLINE_SALE 
			*/
			auth_tx.TLF01_details.tx_key = AUTH_OFFLINE_SALE ;
			retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_STAN) ;
				return retCode ;
		break ;

		case AUTH_VOID_SALE:  /* BCH20 was retrieved by RRN */

         /* For a VOID transaction, if the original transaction was a
          * DEFERRED and was not found in BCH20, then it implies that
          * we never saw the transaction in either BCH10 or BCH20. We return
          * the transaction to the terminal with a 00 response code.
          */
         memset( Buff, 0x00, sizeof(Buff) );
         sprintf( Buff,
                 "Original txn for a void not found in database. Stan: %s",
                  auth_tx.TLF01_details.sys_trace_audit_num );
         dcpiso_log_message( 2, 1, Buff, "Process_bch20_db_not_found",1 );

         /* Send an approved response. */
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
			/* Set general status if original txn not found.
			   We changed while supporting void and reversal txn for MasterCard 
			   Let be it set for all brands. because this not make much changes.
			   and also easy to identify (09-76) 11 Nov,2006 TF-Ajay */
			auth_tx.TLF01_details.general_status= GS_ORIGINAL_TXN_NOT_FOUND_FOR_VOID_SALE ;
			return SEND_RESPONSE_TO_ORIGINATOR ;
		break ;

		case AUTH_OFFLINE_SALE:
			if( (0 == strlen(auth_tx.TLF01_details.settlement_date)))
			{
				   strncpy(auth_tx.TLF01_details.settlement_date,       auth_tx.TLF01_details.date_yyyymmdd + 4, (sizeof(auth_tx.TLF01_details.settlement_date)-1) );
			}

	        if((0 == strlen(auth_tx.TLF01_details.service_code)))
			{
				strncpy(auth_tx.TLF01_details.service_code,"000", 3 );
			}

	        if (true == dcpiso_check_If_Diners_Transaction()||
	        	true == dcpiso_check_If_AMEX_Transaction())
	        {
	        	strncpy (auth_tx.TLF01_details.product_codes[14].code, auth_tx.TLF01_details.pos_entry_mode, 4);
	        }

			/*Let us check if Merchant is configure for Pre-Auth completion validation */
	        if( true == Check_If_Transaction_qualifies_for_Preauth_validation())
			{
				// Original transaction details are not present. Reject this transaction.
	        	if(true == dcpiso_check_If_MC_Transaction())
	        	{
	        		strncpy (auth_tx.TLF01_details.response_code,
	        				 declined_rc_for_original_txn_not_present_for_MC,
	        				 ISO_RESPONSE_CODE_LEN) ;
	        		strcpy(auth_tx.TLF01_details.response_text, "Original Tx not present");
	        		return DEVICE_ERROR;
	        	}
	        	else if(true == dcpiso_check_If_VISA_Transaction())
	        	{
	        		strncpy (auth_tx.TLF01_details.response_code,
	        				 declined_rc_for_original_txn_not_present_for_VISA,
	        				 ISO_RESPONSE_CODE_LEN) ;
	        		strcpy(auth_tx.TLF01_details.response_text, "Original Tx not present");
	        		return DEVICE_ERROR;
	        	}
	        	else if(true == dcpiso_check_If_AMEX_Transaction())
	        	{
	        		strncpy (auth_tx.TLF01_details.response_code,
	        				 declined_rc_for_original_txn_not_present_for_AMEX,
	        				 ISO_RESPONSE_CODE_LEN) ;
	        		strcpy(auth_tx.TLF01_details.response_text, "Original Tx not present");
	        		return DEVICE_ERROR;
	        	}
	        	else if(true == dcpiso_check_If_Diners_Transaction())
	        	{
	        		strncpy (auth_tx.TLF01_details.response_code,
	        				 declined_rc_for_original_txn_not_present_for_DCI,
	        				 ISO_RESPONSE_CODE_LEN) ;
	        		strcpy(auth_tx.TLF01_details.response_text, "Original Tx not present");
	        		return DEVICE_ERROR;
	        	}
	        	else if(true == dcpiso_check_If_JCB_Transaction())
	        	{
	        		strncpy (auth_tx.TLF01_details.response_code,
	        				 declined_rc_for_original_txn_not_present_for_JCB,
	        				 ISO_RESPONSE_CODE_LEN) ;
	        		strcpy(auth_tx.TLF01_details.response_text, "Original Tx not present");
	        		return DEVICE_ERROR;
	        	}
				else if(true==dcpiso_Check_If_CUP_Transaction())
	        	{
	        		strcpy( auth_tx.TLF01_details.response_code, declined_rc_for_original_txn_not_present_for_UPI );
	        		strcpy(auth_tx.TLF01_details.response_text, "No pre-authorization found");
	        		return DEVICE_ERROR;
	        	}
			}
			else
			{
                retCode = Send_Transaction_To_Authorizer () ;
            }
		    return retCode ;
}


	retCode = Get_MCF01_Details () ;
	return retCode ;
} /* Process_bch20_db_not_found */


BYTE Process_select_bch20_preauth_void_db_not_found ()
{
	CHAR strError[512] = {0} ;
	BYTE retCode = 0 ;

	/* Either the merchant is not ours, or somehow it
	* was not uploaded to the database.
	*/
	strcpy (strError,"Original PreAuth transaction not Found");
    strncpy(auth_tx.TLF01_details.response_text,strError,
	    		(sizeof(auth_tx.TLF01_details.response_text)-1));
	dcpiso_log_message( 2, 1, strError,"Process_select_bch20_preauth_void_db_not_found",1 );
	memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
	prepare_authtx_for_forwarding_preauth_void_original_not_present ();
	return (DEVICE_ERROR) ;
}

BYTE Process_select_bch20_db_OK_preauth_total_amount_not_found (void )
{
	CHAR strError[512] = {0} ;
	BYTE retCode = 0 ;

	/* Either the merchant is not ours, or somehow it was not uploaded to the database.*/
	strcpy (strError,"Not able to retrieve total amount");
    strncpy(auth_tx.TLF01_details.response_text,strError,
	    		(sizeof(auth_tx.TLF01_details.response_text)-1));
	dcpiso_log_message( 2, 1, strError,"Process_select_bch20_db_OK_preauth_total_amount_not_found",1 );
	memset(auth_tx.TLF01_details.auth_number, 0x00, sizeof(auth_tx.TLF01_details.auth_number));
	prepare_authtx_for_forwarding_preauth_void_original_not_present ();
	return (DEVICE_ERROR) ;
}


BYTE Process_bch01_db_not_found (pBYTE pCurrent, BYTE subtype2)
{
   CHAR strError[200] = {0} ;
   BYTE retCode = 0 ;
   BYTE MatchingResponseKey = 0 ;

   /*LM 09/29/03 SCR1220 - Duplicate Batch Processing Design*/

    /* Check subtype 2 to determine if we are dealing with duplicate batch. */
    if ( subtype2 == ST2_DB_SELECT_BCH01_BY_BATCH_NBR )
    {
      /* We are dealing with a duplicate batch situation, however, we were
       * unable to find the duplicate in BCH01, so continue normal processing
       * of settlement transaction.  Start by getting the Active batch.
       */
      retCode = Get_BCH01_Details( ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS );
   }
   else
   {
      /* We are not dealing with a duplicate batch.
       * We were unable to find the active batch, so error out.
       */
      strcpy( auth_tx.TLF01_details.response_code, RC_INVALID_TXN );
      MatchingResponseKey = Match_Request_To_Response_Transaction_Key () ;
      auth_tx.TLF01_details.tx_key = MatchingResponseKey;

      if ( auth_tx.TLF01_details.tx_key > AUTH_INVALID )
      {
         Send_Transaction_To_Originator();
      }

      sprintf( strError,
              "Batch header not found in the database for %s",
               auth_tx.TLF01_details.terminal_id );

      /* strcpy (error_msg, pCurrent + sizeof auth_tx) ; */
      retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                       "Process_select_bch01_db_not_found",
                                        GS_TXN_SYSTEM_ERROR,
                                        INVALID_TERMINAL_ID );
   }

   return( true );
}/* Process_bch01_db_not_found */


BYTE Process_tlf01_db_not_found( pBYTE pCurrent )
{
   CHAR  strError[200] = {0} ;
   BYTE  retCode;

   strcpy( auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED );

   sprintf( strError,
           "Unable to find Reload Confirm in TLF01 for Void, Amount: %s, stan: %s",
            auth_tx.TLF01_details.total_amount,
            auth_tx.TLF01_details.sys_trace_audit_num );
   retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                          "Process_tlf01_db_not_found",
                                           GS_TXN_SYSTEM_ERROR,
                                           SYSTEM_MALFUNCTION );

   return SEND_RESPONSE_TO_ORIGINATOR ;
}/* Process_tlf01_db_not_found */

BYTE Process_When_Bin_Range_Not_Found (void)
{
	BYTE	retCode = 0 ;
	BYTE	strError[512] = {0} ;
	/*	Control gets here only on a AUTH_BATCH_UPLOAD
		Even if the BIN record is not found, we cannot treat DEVICE_OK as an error,
		we have to continue by getting the MCT01 record
	*/

	strcpy (strError, "BIN01 record NOT found during batch upload\n") ;
	PRINT (strError) ;
	retCode = Get_MCT01_Details () ;
	return retCode ;

} /* Process_When_Bin_Range_Not_Found */

BYTE Process_When_Bin_Range_Found (void) 
{
	
	BYTE retCode = 0 ;
    
	BYTE TransactionOrigin = 0 ;
	char strError[512] = {0} ;

	strcpy (strError, "Found BIN01 record during batch upload\n") ;
	PRINT (strError) ;
	
	/* If the transaction is locally acquired,before switching out 
	   do the merchant velocity checks, if any check fails,
	   the function returns ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR,
	   else NO_AUTH_ERROR */

	/* Copy the bin's settlement file prefix into TLF01's settlement file prefix */
	/*
	if (strlen (auth_tx.BIN01_details.settle_file_prefix) != 0)
		strcpy (auth_tx.TLF01_details.settle_file_prefix, auth_tx.BIN01_details.settle_file_prefix) ;

	if (strlen (auth_tx.BIN01_details.card_family) != 0)
		strcpy (auth_tx.TLF01_details.card_family, auth_tx.BIN01_details.card_family) ;

	if (strlen (auth_tx.BIN01_details.card_type) != 0)
		strcpy (auth_tx.TLF01_details.card_type, auth_tx.BIN01_details.card_type) ;
	*/

	retCode = Get_MCT01_Details () ;
	return retCode ;

	
} /* Process_When_Bin_Range_Found */

BYTE Edit_DCF01_Record (DCF01 sDCF01)
{
	BYTE strError[512] = {0} ;
	BYTE retCode = 0 ;
	CHAR	strDeviceMerchant[50] = {0} ; 
	CHAR	strTransactionMerchant[50] = {0} ;

	strTrim (sDCF01.merchant_id,				strDeviceMerchant) ;
	strTrim (auth_tx.TLF01_details.merchant_id, strTransactionMerchant) ;

	/* Ignore the following checks if the transaction cannot be declined */
	if (!If_Transaction_Cannot_Be_Declined () &&
		auth_tx.TLF01_details.tx_key != AUTH_SETTLE &&
		auth_tx.TLF01_details.tx_key != AUTH_SETTLE_TRAILER &&
		auth_tx.TLF01_details.tx_key != AUTH_BATCH_UPLOAD)
	{
      /*+----------------------------------------------------------------------+
        | This 'if' statement is temporary for SCR 449. It handles EFT Master  |
        | Voice transactions during Phase 1.  After Phase 1, ATP will not be   |
        | receiving EFT transactions.  An EFT Voice transaction is defined as  |
        | POS Entry Mode = 0000, and Terminal ID = 00000000.  ATP requires a   |
        | device record, but this code here bypasses the check that ensures    |
        | the merchant id in the device record matches the merchant id in the  |
        | transaction.        9/25/01                                          |
        |                                                                      |
        | Apparently, non-voice txns from EFT will have POS Entry Mode, but    |
        | terminal Id will still be all zeroes.    10/10/01                    |
        +----------------------------------------------------------------------+
      */

      if ( 0 == strcmp(auth_tx.TLF01_details.terminal_id,"00000000") )
      {
         strcpy( auth_tx.TLF01_details.organization_id, "0000" );
      }
      else
      {
    	  if(true == dcpiso_Check_If_CUP_Transaction())
			{	/*For Cup MID and TID  combination may not valid. CUP MID is diffretnt, it is in CUP accespotr ID*/
			}
			 else if (!strEqual (strDeviceMerchant, strTransactionMerchant))
	         {
	            sprintf (strError, "Invalid merchant in the transaction - not matching the device merchant: %s\n", auth_tx.TLF01_details.merchant_id) ;
	         }
      }

		if (!strEqual (sDCF01.status, "A"))
		{
			sprintf (strError, "Invalid status %s for the device %s\n", sDCF01.status, auth_tx.TLF01_details.terminal_id);
		}
		else if (auth_tx.TLF01_details.tx_key < 41 && /* this is because valid_transactions is an array of 40 and tx_key 
			                                             starts with 1 */
			     sDCF01.valid_transactions[auth_tx.TLF01_details.tx_key-1] != '1') 
		{
			sprintf (strError, "Transaction not permitted for this terminal: %s\n", auth_tx.TLF01_details.terminal_id);
		}

		if (strError[0] != 0)
		{
			retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
												"Process_select_dcf01_db_OK",
												GS_TXN_SYSTEM_ERROR,
												INVALID_TERMINAL_ID) ;
			return retCode ;
		}
	}
	return DEVICE_OK ;
}

BYTE Edit_MCF01_Record (MCF01 sMCF01) 
{
	BYTE	strError [512] = {0} ;
	CHAR	strSystemDateTime[50] = {0} ;
	CHAR	strTemp [512] = {0} ;
	LONG	TransactionDate = 0L ;
	LONG	BlockStartDate = 0L ;
	LONG	BlockEndDate = 0L ;
	BYTE	retCode = 0 ;

	/* Please note that all dates are in Julian format */

	if (!If_Transaction_Cannot_Be_Declined () &&
		auth_tx.TLF01_details.tx_key != AUTH_SETTLE &&
		auth_tx.TLF01_details.tx_key != AUTH_SETTLE_TRAILER &&
		auth_tx.TLF01_details.tx_key != AUTH_BATCH_UPLOAD)
	{
		if (strcmp (sMCF01.status, "A") != 0) 
		{
			strcpy (strError, "Invalid merchant status") ;
			retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
												"Process_select_mcf01_db_OK",
												GS_TXN_SYSTEM_ERROR,
												INVALID_MERCHANT) ;
			return retCode ;
		}
	}

	/*	Check if the system date is between the block start date and the block end date
		for the merchant - if yes, then flag error and return 
	*/

	BlockStartDate = atol(sMCF01.block_start_date);
	BlockEndDate = atol(sMCF01.block_end_date);
	genutil_format_date (strSystemDateTime);
	memcpy(strTemp, strSystemDateTime, 8); 
	TransactionDate = Txutils_Calculate_Julian_Date (strTemp);
	if ((BlockStartDate > 0) && (BlockEndDate > 0))
	{
		if (TransactionDate >= BlockStartDate && TransactionDate <= BlockEndDate)
		{
			sprintf (strError, "Merchant %s is blocked during this period", 
					 auth_tx.TLF01_details.merchant_id) ;
			retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
												    "Edit_MCF01_Record",
												    GS_TXN_SYSTEM_ERROR,
												    INVALID_MERCHANT) ;
			return retCode ; 
		}
	}
	else if ((BlockStartDate > 0) && (BlockEndDate == 0))
	{
		if (TransactionDate >= BlockStartDate)
		{
			sprintf (strError, "Merchant %s is blocked during this period", 
					 auth_tx.TLF01_details.merchant_id) ;
			retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
												"Edit_MCF01_Record",
												GS_TXN_SYSTEM_ERROR,
												INVALID_MERCHANT) ;
			return retCode ;
		}
	}
	else if ((BlockStartDate == 0) && (BlockEndDate > 0))
	{
		if (TransactionDate <= BlockEndDate)
		{
			sprintf (strError, "Merchant %s is blocked during this period", 
					 auth_tx.TLF01_details.merchant_id) ;
			retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
												    "Edit_MCF01_Record",
												    GS_TXN_SYSTEM_ERROR,
												    INVALID_MERCHANT) ;
			return retCode ;
		}
	}

	return DEVICE_OK ;
} /* Edit_MCF01_Record */

INT Check_Validation_OfflineSale_Pre_auth_trxn(BCH20 sBCH20)
{


	INT return_code=false;
	LONG juldate_preauth_txn = 0;
	LONG juldate_offline_sale_txn = 0;
	CHAR strSystemDate[9] = {0};
	CHAR strSystemDateTime[50] = {0};

	genutil_format_date (strSystemDateTime) ;

	strncpy(strSystemDate,strSystemDateTime,8);

	juldate_preauth_txn = Txutils_Calculate_Julian_Date(sBCH20.open_date);
	juldate_offline_sale_txn	 = Txutils_Calculate_Julian_Date(strSystemDate);
	if((juldate_offline_sale_txn - juldate_preauth_txn) > 30)
	{
		return_code=true;
	}

	return return_code;

}
/*INT CUP_Check_If_Transaction_qualifies_for_Preauth_validation()
{
	if(((0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"Y",1)) ||
			(0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"y",1))) &&
			(0 == strncmp(auth_tx.TLF01_details.processing_code,validate_preauth_validation_processing_code,
					PROCESSING_CODE_LEN)))
		{
			return true ;
		}
		else
		{
			return false;
		}

}*/

INT Check_Validation_DCI_Incremental_Pre_auth_trxn(BCH20 sBCH20)
{


	INT return_code=false;
	LONG juldate_preauth_txn = 0;
	LONG juldate_incr_preauth_txn = 0;
	CHAR strSystemDate[9] = {0};
	CHAR strSystemDateTime[50] = {0};
	char year[5]={0};
	char month[3]={0};
	char day[3]={0};
	int iyear=0,yyyy=0,mm=0,dd=0,JulianDate=0;

	genutil_format_date (strSystemDateTime) ;

	strncpy(strSystemDate,strSystemDateTime,8);

	strncpy(year,strSystemDate,4);
	strncpy(month,strSystemDate+4,2);
	strncpy(day,strSystemDate+6,2);
	yyyy=atoi(year);
	mm=atoi(month);
	dd=atoi(day);

	juldate_preauth_txn 		 = Txutils_Calculate_Julian_Date(sBCH20.open_date);
	juldate_incr_preauth_txn	 = Txutils_Calculate_Julian_Date(strSystemDate);
	if((juldate_incr_preauth_txn - juldate_preauth_txn) < 30)
	{
		return_code=true;
	}

	return return_code;

}

