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
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
extern INT nFalconSendFlag;
extern INT Falcon_RT_Flag ;
DCF01 dcf01;

extern CHAR nciftr_que_name [];
extern CHAR nciftr_module_name [];
extern CHAR pos_response_code[3];
extern CHAR pos_res_txt[40];
extern PRIVATE_CCF03P_STRUCT private_ccf03p_struct ;
extern INT Nmra_send_flag;
INT exception_cardnum_flag;

#define NOT_BLOCKED 0
#define BLOCKED     1
#define EMPTY       2

BYTE Process_Dataserver_OK_Message (pPTE_MSG p_msg_in)
{
   CHAR           strError[256] = {0} ;
   pBYTE          pCurrent = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           subType1 = 0, subType2 = 0 ;
   BYTE           app_data_type = 0 ;
   BYTE           retCode = 0 ;

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
      if(subType1 == ST1_DB_CHECK_BIN_RANGE)
      {
#ifdef HCTS3
		 if(subType2 == ST2_NCIFTR_ONLINE)
		 {
			 BIN01 sBIN01 ;

			 memset (&sBIN01, 0, sizeof sBIN01) ;
			 memcpy (&sBIN01, pCurrent+ sizeof auth_tx, sizeof sBIN01) ;
			 memcpy (&auth_tx.BIN01_details, &sBIN01, sizeof sBIN01) ;

			 /* Determine if transaction is a duplicate. */
			 retCode = Dcpimp_process_when_bin_range_found_online();
			 if(retCode == DEVICE_OK)
			 {
				 retCode = Process_select_mcf01_db_OK (pCurrent, MCF01_READ) ;
			 }
			 else if(retCode == NO_AUTH_ERROR)
			 {
				 retCode = DEVICE_OK;
			 }
			 return retCode ;
		 }
		 else
#endif
		 {
	         if (app_data_type == BIN01_DATA)
	         {
				retCode = Process_select_bin01_db_OK (pCurrent) ;
				return retCode ;
	         }
		 }
      }
      else if (subType1 == ST1_DB_SELECT)
      {
         /* If the totals on the transaction match the batch totals, then
          * we have to update the batch header (BCH01_MP) with the real batch
          * number, update the batch details (BCH10) with the real batch number,
          * change batch_status to 'C', add the batch number to the device
          * record, etc.
          */
         if (app_data_type == BIN01_DATA)
         {
			retCode = Process_select_bin01_db_OK (pCurrent) ;
			return retCode ;
         }
         else if (app_data_type == CAF01_DATA)
         {
#ifdef HCTS3
        	 if(subType2 == ST2_NCIFTR_ONLINE)
        	 {
        		 CAF01 sCAF01 ;

        		 memset (&sCAF01, 0, sizeof sCAF01) ;
        		 memcpy (&sCAF01, pCurrent+ sizeof auth_tx, sizeof sCAF01) ;
        		 memcpy (&auth_tx.CAF01_details, &sCAF01, sizeof sCAF01) ;

        		 /* Determine if transaction is a duplicate. */
        		 retCode = Dcpimp_process_when_caf_found_online();
        		 if(retCode == DEVICE_OK)
        		 {
        			 retCode = Process_select_mcf01_db_OK (pCurrent, MCF01_READ) ;
        		 }
        		 return retCode ;
        	 }
#endif
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
#ifdef HCTS3
			 if ( //Check_Is_Host_Up(nciftr_module_name) == true && //irrespective of host status verify BIN details
				  auth_tx.TLF01_details.tx_key == AUTH_SALE    &&
				  strncmp(auth_tx.TLF01_details.message_type, "0200", 4) == 0)
			 {
				 memcpy (&auth_tx.MCF01_details, pCurrent+ sizeof(auth_tx), sizeof(auth_tx.MCF01_details)) ;
				 retCode = Retrieve_BIN01_Record_Online();
				 if(retCode == DEVICE_OK)
				 {
					 retCode = Process_select_mcf01_db_OK (pCurrent, MCF01_READ) ;
				 }
				 else if(retCode == NO_AUTH_ERROR)
				 {
					 dcpimp_log_message(2, 1, "Database read for BIN01/CAF01, ", "Process_Dataserver_OK_Message", 1);
					 retCode = DEVICE_OK;
				 }
				 return retCode ;
			 }
			 else
#endif
			 {
				 retCode = Process_select_mcf01_db_OK (pCurrent, MCF01_NEW) ;
				 return retCode ;
			 }
         }
		 else if (app_data_type == MGF01_DATA)
         {
            retCode = Process_select_mgf01_db_OK (pCurrent) ;
            return retCode ;
         }
		 else if (app_data_type == CCF02P_DATA)
         {
            retCode = Process_select_ccf02p_db_OK(pCurrent) ;
            return retCode ;
         }
		 else if (app_data_type == CCF02P_DATA_RT)
		 {
			 retCode = Process_select_ccf02p_db_OK_For_RT(pCurrent) ;
			 return retCode ;
		 }
		 else if (app_data_type == ACF01_DATA)
		 {
			retCode= Process_select_ACF01_db_OK(pCurrent);
			return retCode ;
		 }
		 else if (app_data_type == ACF01_DATA_RT)
		 {
			retCode= Process_select_ACF01_db_OK_For_RT(pCurrent);
			return retCode ;
		 }
		 else if (app_data_type == PAF01_DATA)
		 {
			retCode = Process_select_PAF01_db_OK(pCurrent);
		 }
         else if (app_data_type == PAF01_DATA_RT)
         {
        	retCode = Process_select_PAF01_db_OK_For_RT(pCurrent);
         }
		 else if (app_data_type == CCF03P_DATA)
		 {
			retCode= Process_select_CCF03P_db_OK(pCurrent);
			return retCode ;
		 }
		 else if (app_data_type == CR01_DATA)
		 {
			retCode= Process_select_CR01_db_OK(pCurrent);
			return retCode ;
		 }
		 /*else if (app_data_type == MCF03P_DATA)
		 {
		 	retCode= Process_select_MCF03P_db_OK(pCurrent);
		 	return retCode ;
		 }*/
		 else if (app_data_type == MPPARA01_DATA)
         {
            retCode = Process_select_mppara01_db_OK (pCurrent) ;
            return retCode ;
         }
         else if (app_data_type == BCH01_MP_DATA)
         {
            retCode = Process_select_bch01_mp_db_OK (pCurrent, subType2) ;
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
            retCode = Process_select_bch20_db_OK (pCurrent) ;
            return retCode ;
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
         if (app_data_type == BCH01_MP_DATA)
         {
            if (subType2 == ST2_DB_UPDATE_BCH01_GOOD_SETTLE)
            {
               /* Totals have matched, the BCH01_MP record has been updated
                * with batch number, batch_status = 'C', DCF01 has the
                * new batch number, BCH10 records have the new batch number.
                * Set response code = 00 and return BCH01_GOOD_SETTLE 
                */
               retCode = Process_update_bch01_mp_db_OK (BCH01_TOTALS_MATCHED) ;
               return retCode ;
            }
            else if (subType2 == ST2_DB_UPDATE_BCH01_DELETE_BCH10)
            {
               /* Totals have matched, the BCH01_MP record has been updated
                * with batch number, batch_status = 'C', DCF01 has the
                * new batch number, BCH10 records have the new batch number.
                * Set response code = 00 and return BCH01_GOOD_SETTLE 
                */
               retCode = Process_update_bch01_mp_db_OK (BCH01_TOTALS_NOT_MATCHED);
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
           //For MP transaction , we are forwading void tranasction to nciftr and once we receive response 
           // We will forward the same to HOST 
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
			return retCode ;
	}

   retCode = DEVICE_OK;
	if (auth_tx.TLF01_details.tx_key < 100)
	{   
		if (subtype1 == ST1_DB_CHECK_BIN_RANGE || subtype1 == ST1_DB_SELECT || subtype1 == ST1_DB_UPDATE)
		{
			if (app_data_type == BIN01_DATA)
			{
#ifdef HCTS3
	        	if(subtype2 == ST2_NCIFTR_ONLINE)
	        	{
	        		dcpimp_log_message( 2, 3, "BIN01 record not found in Database, ", "Process_Dataserver_NotFound_Message", 1 );
	        		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

	        		retCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, "Invalid card",
	        		                                        "Process_Dataserver_NotFound_Message",
	        												GS_TXN_DROPPED,
	        												INVALID_TRANSACTION) ;
	        	 	return retCode ;
	        	}
	        	else
#endif
	        	{
	        		retCode = Process_bin01_db_not_found (pCurrent) ;
	        	}
			}
			else if (app_data_type == CAF01_DATA)
			{
#ifdef HCTS3
	        	if(subtype2 == ST2_NCIFTR_ONLINE)
	        	{
	        		dcpimp_log_message( 2, 3, "CAF01 record not found in Database, ", "Process_Dataserver_NotFound_Message", 1 );
	        		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

	        		retCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, "Invalid card",
	        		                                        "Process_Dataserver_NotFound_Message",
	        												GS_TXN_DROPPED,
	        												INVALID_TRANSACTION) ;
	        	 	return retCode ;
	        	}
#endif
			}
			else if (app_data_type == MCT01_DATA)
				retCode = Process_mct01_db_not_found (pCurrent) ;

			else if (app_data_type == MCF01_DATA)
				retCode = Process_mcf01_db_not_found (pCurrent) ;
			else if (app_data_type == MGF01_DATA)
				retCode = Process_mgf01_db_not_found (pCurrent) ;
			else if (app_data_type == CCF02P_DATA)
				retCode = Process_ccf02p_db_not_found (pCurrent) ;
			else if (app_data_type == CCF02P_DATA_RT)
				retCode = Process_ccf02p_db_not_found_For_RT (pCurrent) ;
			else if (app_data_type == ACF01_DATA)
				retCode = Process_ACF01_db_not_found(pCurrent);
			else if (app_data_type == ACF01_DATA_RT)
				retCode = Process_ACF01_db_not_found_For_RT(pCurrent);
			else if(app_data_type == PAF01_DATA)
				retCode = Process_PAF01_db_not_found(pCurrent);
			else if(app_data_type == PAF01_DATA_RT)
				retCode = Process_PAF01_db_not_found_For_RT(pCurrent);
			else if (app_data_type == CR01_DATA)
					retCode = Process_CR01_db_not_found(pCurrent);
			else if (app_data_type == CCF03P_DATA)
				retCode = Process_CCF03P_db_not_found(pCurrent);
			/*else if (app_data_type == MCF03P_DATA)
				retCode = Process_MCF03P_db_not_found();*/
			else if (app_data_type == MPPARA01_DATA)
				retCode = Process_mppara01_db_not_found (pCurrent) ;

			else if (app_data_type == DCF01_DATA)
				retCode = Process_dcf01_db_not_found (pCurrent) ;

			else if (app_data_type == BCH01_MP_DATA)
				retCode = Process_bch01_mp_db_not_found (pCurrent, subtype2);

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
				retCode = Process_bch20_db_not_found (p_msg_in,pCurrent) ;

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
   return retCode ;
} /* Process_Dataserver_ERROR_Message */




BYTE Process_select_dcf01_auto_init (pBYTE pCurrent)
{
	memset (&dcf01, 0, sizeof dcf01);
	memcpy (&dcf01, pCurrent + sizeof auth_tx, sizeof dcf01);
	return BCH01_TOTALS_MATCHED ;
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
   // Save DCF01 batch number for logging purpose later.
   memcpy(auth_tx.TLF01_details.voice_auth_text, sDCF01.batch_nbr, sizeof(sDCF01.batch_nbr));

   if (auth_tx.TLF01_details.tx_key == AUTH_BATCH_UPLOAD)
   {
      if (!strlen(auth_tx.TLF01_details.merchant_id)  || 
         (IsFieldBlank (auth_tx.TLF01_details.merchant_id,
                    strlen(auth_tx.TLF01_details.merchant_id))))
      {
         strcpy (auth_tx.TLF01_details.merchant_id, sDCF01.merchant_id) ;
      }
   }

   retCode = Edit_DCF01_Record (sDCF01) ;
   if (retCode != DEVICE_OK)
      return retCode ;

	/*	If the transaction is a SETTLEMENT or a SETTLEMENT_TRAILER
		and if the batch number in the transaction is the same as that found in the
		device, it means that that terminal may have already settled. The BCH01_MP record 
		will be retreived to check the totals against the incoming request totals. If
		the totals match, the terminal batch is a duplicate of a previously settled batch.
		If the totals do not match, the batch will be accepted.
	*/
  

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
			return ISO_PARSE_ERROR ;
			
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
			   
			return SEND_RESPONSE_TO_ORIGINATOR ;
		break ;

		case AUTH_STATISTICS:
			retCode = Process_Terminal_Statistics_Transaction (auth_tx.TLF01_details.tx_key) ;
			return retCode ;
		break;
      
		default:
        break;
    }

	/* For all other transactions, request a MCF01 database read */
	retCode = Get_MCF01_Details () ;
	return retCode ;
	

} /* Process_select_sDCF01_db_reply */

BYTE Process_select_mcf01_db_OK (pBYTE pCurrent, int mcfReadStatus)
{
   INT   len;
   INT   emv_len;
   INT   ret_val;
   MCF01 sMCF01;
   BYTE  retCode = 0;
   CHAR  temp_str[100] = "";
   CHAR  strError[512] = {0};
   BYTE  hex_tag[5];
   double namount = 0.0;
   double nCreditPurchMax = 0.0;
   double nCreditAuthMax = 0.0;

   if(mcfReadStatus == MCF01_READ)
   {
	   memset (&sMCF01, 0, sizeof sMCF01) ;
	   memcpy (&sMCF01, &auth_tx.MCF01_details, sizeof sMCF01) ;
   }
   else
   {
	   memset (&sMCF01, 0, sizeof sMCF01) ;
	   memcpy (&sMCF01, pCurrent+ sizeof auth_tx, sizeof sMCF01) ;
	   memcpy (&auth_tx.MCF01_details, &sMCF01, sizeof sMCF01) ;
   }

	strcpy (auth_tx.TLF01_details.currency_code, sMCF01.currency_code) ;
	memcpy(auth_tx.TLF01_details.product_codes[4].code,sMCF01.process_control_id,6);
	if(strcmp(sMCF01.name02,NULL_STR)!=0)
	{
		strncpy(auth_tx.TLF01_details.card_holder_name,sMCF01.name02,strlen(sMCF01.name02));
	}
	else if(strcmp(sMCF01.name01,NULL_STR)!=0)
	{
		strncpy(auth_tx.TLF01_details.card_holder_name,sMCF01.name01,strlen(sMCF01.name01));
	}
	
   /* For EFT Voice transactions, use MCC that came in the request. */
   if ( auth_tx.TLF01_details.category_code[0] == 0x00 )
   {
      strcpy(auth_tx.TLF01_details.category_code,sMCF01.voice_category_code);
   }
// GET MERCHANT A\name and name01 to tlf01.cardholdername
	/* Validate merchant status and the merchant blocking dates */
	retCode = Edit_MCF01_Record (sMCF01) ;
	if (retCode != DEVICE_OK)
		return retCode ;

	if((auth_tx.TLF01_details.tx_key !=AUTH_SETTLE)&&
		(auth_tx.TLF01_details.tx_key !=AUTH_SETTLE_TRAILER))
	{
		/*Prepare POS DATA for Tranlookup*/
		dcpimp_Prepare_Pos_Data();
	}

	//Conver total amount into double
	namount = (strtod (auth_tx.TLF01_details.total_amount, 0) / 100.0 );
	if(auth_tx.MCF01_details.credit_purch_max[0] != 0)
	{
		nCreditPurchMax = (strtod (auth_tx.MCF01_details.credit_purch_max, 0) / 100.0);
    }
	if(auth_tx.MCF01_details.credit_auth_max[0] != 0)
	{
		nCreditAuthMax = (strtod (auth_tx.MCF01_details.credit_auth_max, 0) / 100.0);
    }

	switch (auth_tx.TLF01_details.tx_key)
	{
		case AUTH_SETTLE:
		case AUTH_SETTLE_TRAILER:

		   if(0 == strncmp(sMCF01.status,"R", 1))
		   {
			   strcpy( auth_tx.TLF01_details.response_code, "05" );
			   strcpy( auth_tx.TLF01_details.response_text, "RESTRICTED Merchant");
			   return DEVICE_ERROR;

		   }
		   else	if( true == Check_Is_Host_Up(nciftr_module_name))
 			{	
				  /*ncitr host is online*/
				 retCode = Send_Request_To_Service 
			            	 (nciftr_que_name, 
							  MT_AUTH_REQUEST, 
							  0, 
							  0,
							  0,
							  0,
							  0) ;
			    if (retCode == false)
			    {
			 	    sprintf(strError,"error in sending data to ncitr host") ;
		     	    dcpimp_log_message( 1, 3, strError, " Process_select_mcf01_db_OK ", 1 );
					return retCode ;
				}
			}
			else
			{

				// We need to store these datils in TFL01_Stip table
				strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
				/*	
					If the transaction is a SETTLEMENT or a SETTLEMENT_TRAILER
					and if the batch number in the transaction is the same as that found in the
					device, it means that that terminal may have already settled. The BCH01_MP record 
					will be retreived to check the totals against the incoming request totals. If
					the totals match, the terminal batch is a duplicate of a previously settled batch.
					If the totals do not match, the batch will be accepted.
				*/
				
			   if (auth_tx.TLF01_details.tx_key == AUTH_SETTLE || 
			       auth_tx.TLF01_details.tx_key == AUTH_SETTLE_TRAILER)
			   	{
				  if (strEqual (auth_tx.TLF01_details.voice_auth_text, auth_tx.TLF01_details.batch_number)) /*LM 09/29/03 SCR1220 - Duplicate Batch Processing*/
			      {
			         /* Yes. Get the previous batch to check batch totals for duplication.*/
			         retCode = Get_BCH01_MP_Details(ST2_DB_SELECT_BCH01_BY_BATCH_NBR) ;
			         return retCode;
				  }
				  else
				  {
				  	strcpy (strError, "Transaction is AUTH_SETTLE/AUTH_SETTLE_TRAILER - Getting BCH01_MP\n") ;
					PRINT (strError) ;
					retCode = Get_BCH01_MP_Details (ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS) ; 
					return retCode ;
				  	
				  }
		   		}
			}
			break;

		case AUTH_BATCH_UPLOAD:
			if( true == Check_Is_Host_Up(nciftr_module_name))
 			{	
				  /*ncitr host is online*/
				 retCode = Send_Request_To_Service 
			            	 (nciftr_que_name, 
							  MT_AUTH_REQUEST, 
							  0, 
							  0,
							  0,
							  0,
							  0) ;
			    if (retCode == false)
			    {
			 	    sprintf(strError,"error in sending data to ncitr host\n") ;
		     	    dcpimp_log_message( 1, 3, strError, " Process_select_mcf01_db_OK ", 1 );
					return retCode ;
		        }
			}
			else
			{
				// We have to update  this in TLF01_Stip table as well.
				strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
				strcpy (auth_tx.TLF01_details.batch_number,	"ACTIVE") ;
				strcpy (strError, "\nThis is a batch upload transaction\n") ;
				PRINT (strError) ;
				retCode = Process_Batch_Upload () ;
				return retCode ;
			}
					   
			break;
			
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
			return retCode ;
		break ;

		case AUTH_OFFLINE_VOID_SALE: /* Void of Offline Sale */
			strcpy (strError, "Transaction is a VOID of OFFLINE SALE\n") ;
		case AUTH_OFFLINE_VOID_REFUND: /* Void of Offline Refund */
				strcpy (strError, "Transaction is a VOID of OFFLINE REFUND\n") ;

				if (strlen (auth_tx.TLF01_details.orig_retrieval_ref_num) > 0)
				{	/* This is the case when the offline sale/refund was piggbacked by a
					on line sale (0220 02) */
					retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_RRN) ;
					return retCode ;
				}
				else
				{
					/* This is the case when the offline sale and the void are collapsed into
					one transaction */
					retCode = Send_Transaction_To_Authorizer () ;
					return retCode ;
				}
		break ;
		
		case AUTH_REVERSAL:

			if( true == Check_Is_Host_Up(nciftr_module_name))
 			{	
				  /*ncitr host is online*/
				 retCode = Send_Request_To_Service 
			            	 (nciftr_que_name, 
							  MT_AUTH_REQUEST, 
							  0, 
							  0,
							  0,
							  0,
							  0) ;
			    if (retCode == false)
			    {
			 	    sprintf(strError,"error in sending data to ncitr host\n") ;
 		     	    dcpimp_log_message( 1, 3, strError, " Process_select_mcf01_db_OK ", 1 );
					return retCode ;
		        }
			}
			else
			{
				// We have to update  this in TLF01_Stip table as well.
				strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
			}
         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( strError, 0x00, sizeof(strError) );
         strcpy( strError,
                 "Reversal from a local device: %s. Card %s, Merchant %s" );

        dcpimp_log_message( 3, 2, strError, " Process_select_mcf01_db_OK ", 1 );
		case AUTH_OFFLINE_SALE:
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

				retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_STAN) ;
				return retCode ;

		break ;
		case AUTH_SALES_COMPLETION:
			/*	First check the BCH10 table by STAN to check if the SALES_COMPLETION
				is a REPEAT 
			*//* TF Phani-  */
			retCode = Get_BCH10_Details (ST2_DB_SELECT_BCH10_BY_STAN) ;
			//retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER_CARD_NBR) ;
			return retCode ;
		break ;
		case AUTH_AUTHORIZATION:
			//Only for MP transactions
			if(0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))
			{
				if (namount > nCreditAuthMax)
				{
					sprintf( strError, "Credit Auth max amount reached");
					dcpimp_log_message( 2, 1, strError, " Process_select_mcf01_db_OK", 1 );
					strcpy( auth_tx.TLF01_details.response_code, pos_response_code);
					strcpy(auth_tx.TLF01_details.response_text, pos_res_txt);
					memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
					return DEVICE_ERROR;
				}

			}

			// Lets check for MID capping
			//retCode = Get_MCF03P_Details();
			/*Check if its a MP tansaction qualifying for Realtime or not*/
			if (true == Falcon_RT_Flag  &&
				AUTH_SALE == auth_tx.TLF01_details.tx_key &&
				true == Dcpimp_check_link_for_NCIFTR() &&
				true == Dcpimp_verify_fraud_rules_for_NCIFTR() &&
				0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))
			{
				/*Lets get the CCF02P details*/
				retCode = Get_CCF02P_Details_For_RT ();
				if (DEVICE_OK == retCode)
				{
					/*Do nothing let it return normally from this function*/
				}
				else
				{
					sprintf( strError, "CCF02P retrieval Failed for FRT transaction");
					dcpimp_log_message( 2, 1, strError, " Process_select_mcf01_db_OK", 1 );
					retCode = Dcpimp_Send_Request_To_RT_Falcon_Service();
					if(DEVICE_ERROR == retCode)
					{
						sprintf( strError, "Failed to send for FRT transaction to Falcon");
						dcpimp_log_message( 2, 1, strError, " Process_select_mcf01_db_OK", 1 );
						retCode = Send_Transaction_To_Authorizer () ;
					}
					else
					{
						return retCode ;
					}
				}
			}
			else
			{
				retCode = Send_Transaction_To_Authorizer () ;
			}
			return retCode ;
			break;
		
		default:
			/* for all other transactions */
			/* only for MP, MID capping */
			if(0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))
			{
				if (namount > nCreditPurchMax)
				{
					sprintf( strError, "Credit Purch max amount reached");
					dcpimp_log_message( 2, 1, strError, " Process_select_mcf01_db_OK", 1 );
					strcpy( auth_tx.TLF01_details.response_code, pos_response_code);
					strcpy(auth_tx.TLF01_details.response_text, pos_res_txt);
					memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
					return DEVICE_ERROR;
				}
			}


			//retCode = Get_MCF03P_Details();
			/*Check if its a MP tansaction qualifying for Realtime or not*/
			if (true == Falcon_RT_Flag  &&
				AUTH_SALE == auth_tx.TLF01_details.tx_key &&
				true == Dcpimp_check_link_for_NCIFTR() &&
				true == Dcpimp_verify_fraud_rules_for_NCIFTR() &&
				 0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity, "1", 1))
			{
				/*Lets get the CCF02P details*/
				retCode = Get_CCF02P_Details_For_RT ();
				if (DEVICE_OK == retCode)
				{
					/*Do nothing let it return normally from this function*/
				}
				else
				{
					sprintf( strError, "CCF02P retrieval Failed for FAI FRT transaction");
					dcpimp_log_message( 2, 1, strError, " Process_select_mcf01_db_OK", 1 );
					retCode = Dcpimp_Send_Request_To_RT_Falcon_Service();
					if(DEVICE_ERROR == retCode)
					{
						sprintf( strError, "Failed to send for FAI FRT transaction to Falcon");
						dcpimp_log_message( 2, 1, strError, " Process_select_mcf01_db_OK", 1 );
						retCode = Send_Transaction_To_Authorizer () ;
					}
					else
					{
						return retCode ;
					}
				}
			}
			else
			{
				retCode = Send_Transaction_To_Authorizer () ;
			}
			return retCode ;
	}

	return(DEVICE_OK) ;
} /* Process_select_mcf01_db_OK */

BYTE Process_select_mgf01_db_OK (pBYTE pCurrent)
{
	INT   len;
  	INT	 emv_len;
   	INT	 ret_val;
	INT  i = 0;
	INT  j = 0;
	MGF01 sMGF01;
   	BYTE  retCode = 0;
	double  term = 0.0;
   	CHAR  temp_str[100] = "";
   	CHAR  strError[512] = {0};
	BYTE  term_value[3]="";
	double namount   = 0.0 ;  
	double nTotal_amount_limit1   = 0.0 ;  
	double nTotal_amount_limit2   = 0.0 ;  
	double nTotal_amount_limit3   = 0.0 ;  
	double nTotal_amount_limit4   = 0.0 ; 
	double ntemp_INT_rate = 0.0;
	double ntemp_MFS_rate = 0.0;
	double ntemp_Factor_Rate = 0.0;
	double Factor_Rate = 0.0; 
	double ndividend = 0.0;
	double ndivisor = 0.0;
	double nSubdivisor1 = 0.0;
	double nSubdivisor2 = 0.0;
	double round_factor = 0.0;
	double nmonthlyamount = 0.0;
	double tempnmonthlyamount = 0.0;
	double ngrossamount=0.0;

	char temp_INT_value[8]={0};
	char temp_MSF_value[8]={0};
	char temp_Factor_Rate[13] = {0};
	char str_montly_amt[20]={0};
	char str_INT_amt[20]={0};
	char str_MFS_amt[20]={0};
	char str_Factor_rate[20]={0};
	char str_gross_amt[20]={0};

	CHAR  time_date[25] = "";
	CHAR  date_str[9] = "";

	//Variable for rounding off Monthy Amount
	int temp_index = 0;
	char data[2]={0};
	int a = 0;
	int ninteger_part=0;
	char strinteger_part[13]={0};
	char tempstrinteger_part[13]={0};

	memset (&sMGF01, 0, sizeof sMGF01) ;
	memcpy (&sMGF01, pCurrent+ sizeof auth_tx, sizeof sMGF01) ;

	//Validate date
	ptetime_get_timestamp(time_date);
	get_date(time_date,date_str);
	if(atoi(date_str) < atoi(sMGF01.effective_date))
	{
		//System date is greater than effective  date return 
		sprintf( strError, "Merchant group ID's effective date:%s is less than system date %s ",date_str,sMGF01.effective_date);
		dcpimp_log_message( 2, 3, strError, " Process_select_mgf01_db_OK ", 1 );
		strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
		strcpy(auth_tx.TLF01_details.response_text, "Invalid Merchant groupID effective date");
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}

	strncpy(term_value,auth_tx.TLF01_details.deferred_term_length,2 );
	term = atof(term_value);

	//Conver total amount into double
	namount = (strtod (auth_tx.TLF01_details.total_amount, 0) / 100 );

	//Conver Amount limit received from mgf01 table into double
	if(strlen(sMGF01.merchgrp[0].Amount_limit)> 0)
	{
		nTotal_amount_limit1 = (strtod (sMGF01.merchgrp[0].Amount_limit, 0)  ) ;
	}

	if(strlen(sMGF01.merchgrp[1].Amount_limit)> 0)
	{
		nTotal_amount_limit2 = (strtod (sMGF01.merchgrp[1].Amount_limit, 0) ) ;  
	}

	if(strlen(sMGF01.merchgrp[2].Amount_limit)> 0)
	{
		nTotal_amount_limit3 = (strtod (sMGF01.merchgrp[2].Amount_limit, 0) ) ;  
	}

	if(strlen(sMGF01.merchgrp[3].Amount_limit)> 0)
	{
		nTotal_amount_limit4 = (strtod (sMGF01.merchgrp[3].Amount_limit, 0)  ) ; 
	}

	if (namount <= nTotal_amount_limit1 )
	{
		for (i = 0, j = 0 ; j < 12; j++)
		{
			if(0 == strncmp(sMGF01.merchgrp[i].merchgrepint[j].nbr_of_months, term_value, strlen(term_value)))
			{
				strncpy(temp_INT_value,sMGF01.merchgrp[i].merchgrepint[j].INT_rate, 
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].INT_rate)-1 ));
				strncpy(temp_MSF_value,sMGF01.merchgrp[i].merchgrepint[j].MSF_rate,
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].MSF_rate)-1 ));
				//Break  the loops.
				j = 13;
			}
		}
	}
	else if (namount <= nTotal_amount_limit2)
	{
		for ( i = 1, j = 0 ; j < 12; j++)
		{
			if(0 == strncmp(sMGF01.merchgrp[i].merchgrepint[j].nbr_of_months, term_value, strlen(term_value)))
			{
				strncpy(temp_INT_value,sMGF01.merchgrp[i].merchgrepint[j].INT_rate,
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].INT_rate)-1 ));
				strncpy(temp_MSF_value,sMGF01.merchgrp[i].merchgrepint[j].MSF_rate,
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].MSF_rate)-1 ));
				//Break  the loops.
				j = 13;
			}
		}
	}
	else if (namount <= nTotal_amount_limit3)
	{
		for ( i = 2, j = 0 ; j < 12; j++)
		{
			if(0 == strncmp(sMGF01.merchgrp[i].merchgrepint[j].nbr_of_months, term_value, strlen(term_value)))
			{
				strncpy(temp_INT_value,sMGF01.merchgrp[i].merchgrepint[j].INT_rate, 
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].INT_rate)-1 ));
				strncpy(temp_MSF_value,sMGF01.merchgrp[i].merchgrepint[j].MSF_rate,
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].MSF_rate)-1 ));
				//Break  the loops.
				j = 13;
			}
		}
	}
	else if (namount <= nTotal_amount_limit4)
	{
		for ( i = 3, j = 0 ; j < 12; j++)
		{
			if(0 == strncmp(sMGF01.merchgrp[i].merchgrepint[j].nbr_of_months, term_value, strlen(term_value)))
			{
				strncpy(temp_INT_value,sMGF01.merchgrp[i].merchgrepint[j].INT_rate, 
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].INT_rate)-1 ));
				strncpy(temp_MSF_value,sMGF01.merchgrp[i].merchgrepint[j].MSF_rate, 
						(sizeof(sMGF01.merchgrp[i].merchgrepint[j].MSF_rate)-1 ));
				//Break  the loops.
				j = 13;
			}
		}
	}

	// We have to conver INT and MFS rate in double

	ntemp_INT_rate = (strtod (temp_INT_value, 0) ) ; 
	ntemp_MFS_rate = (strtod (temp_MSF_value, 0) ) ; 
	
	if( ntemp_INT_rate == 99.99)
	{
		//Its an invalid intrest rate  
		sprintf( strError, "Invalid term %f for Merchant: ",ntemp_INT_rate);
		dcpimp_log_message( 2, 3, strError, " Process_select_mgf01_db_OK ", 1 );
		strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
		strcpy(auth_tx.TLF01_details.response_text, "Invalid term ");
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}

	//Store INT rate in TLF01_details.product_codes[11].amount str_INT_amt
	sprintf(str_INT_amt, "%lf", ntemp_INT_rate );
	strncpy(auth_tx.TLF01_details.product_codes[11].amount, str_INT_amt, (sizeof (auth_tx.TLF01_details.product_codes[11].amount) - 1));

	//Store MFS rate in TLF01_details.product_codes[12].amount
	sprintf(str_MFS_amt, "%lf", ntemp_MFS_rate );
	strncpy(auth_tx.TLF01_details.product_codes[12].amount, str_MFS_amt, (sizeof (auth_tx.TLF01_details.product_codes[12].amount) - 1));


	//Check waether we have INT rate and MFS rate data else report error.
	if(ntemp_INT_rate> 0 )
	{
	
		/*Calculate the factore rate :
			Formula to calculate fator rate is 
			Factor Rate  = I / (1 - (1+I) ^ -M)
			where,
			I = monthly interest rate = Int rate/(12*100)
			M = term
			for calucaltion :
			ndividend = Int rate % 12
			nSubdivisor1 = (1+I)
			nSubdivisor2 = (nSubdivisor1)^-M
			double ndivisor =1 -nSubdivisor2
		*/
		ndividend = ntemp_INT_rate/(12*100);

		nSubdivisor1 =  1 + ndividend;
		
		term = term * (-1.0);
		nSubdivisor2 = pow(nSubdivisor1,term);
		
		ndivisor = 1 -nSubdivisor2;
		
		ntemp_Factor_Rate = ndividend/ndivisor;

		round_factor = pow (10.0 , 6);// We have to round till 6 digit.
		Factor_Rate = round ( ntemp_Factor_Rate*round_factor);

		//Store Factor rate str_Factor_rate
		sprintf(str_Factor_rate, "%lf", Factor_Rate );	
		strncpy(auth_tx.TLF01_details.deferred_factor,str_Factor_rate, sizeof(auth_tx.TLF01_details.deferred_factor)-1 );
		Factor_Rate = Factor_Rate / round_factor;

		nmonthlyamount= namount*Factor_Rate;
		tempnmonthlyamount = nmonthlyamount * 100;
		sprintf(str_montly_amt, "%lf", tempnmonthlyamount );

		for( i=0; i < 12; i++)
		{
			if(str_montly_amt[i]=='.')
			{
				temp_index = i + 2;
				strncpy(data,str_montly_amt + temp_index, 1);
				a = atoi(data);
				if(a >= 5)
				{
                	strncpy(data,str_montly_amt + temp_index -1, 1);
                	a = atoi(data);
                	a = a +1;
                }
                else
                {
                	strncpy(data,str_montly_amt + temp_index -1, 1);
                	a = atoi(data);
                }
				if(a > 5)
				{
					strncpy(data,str_montly_amt +( i - 1) , 1);
					if(0 == strncmp(data,"9",1))
					{
						/* We have to add 1 to the integer part of that data
						as adding 1 leads to worn calculation*/
						strncpy(strinteger_part ,str_montly_amt, i );
						ninteger_part = atoi(strinteger_part);
						ninteger_part =ninteger_part + 1;
						sprintf(tempstrinteger_part,"%d",ninteger_part);
						strncpy(str_montly_amt ,tempstrinteger_part , i);
					}
					else
					{
					a = atoi(data) + 1;
					sprintf(data,"%d",a);
					strncpy(str_montly_amt + ( i -1 ),data, 1);
					str_montly_amt[i+1]='0';
				}
				}
				break;
			}
		}
		nmonthlyamount=atoi(str_montly_amt);
		tempnmonthlyamount = nmonthlyamount ;
		sprintf(str_montly_amt, "%lf", tempnmonthlyamount );
		strncpy(auth_tx.TLF01_details.monthly_amort_amt,str_montly_amt, sizeof(auth_tx.TLF01_details.monthly_amort_amt)-1 );
		term = atof(term_value);
		ngrossamount = tempnmonthlyamount*term;
		sprintf(str_gross_amt, "%lf", ngrossamount );
		strncpy(auth_tx.TLF01_details.def_gross_amt, str_gross_amt, (sizeof(auth_tx.TLF01_details.def_gross_amt) -1));
		// Get MP parameter from trands. For that we need the product code from ccf02p table.
		retCode = Get_CCF02P_Details();
		return retCode ;
	}
	else if(ntemp_INT_rate == 0)
	{
		/*Its a valid intrest rate and montly amount to be calculated is by formula
		monthly amount = Gross amount / term 
		*/
		nmonthlyamount = namount /term;
		tempnmonthlyamount = nmonthlyamount * 100;
		sprintf(str_montly_amt, "%lf", tempnmonthlyamount );

		for( i=0; i < 12; i++)
		{
			if(str_montly_amt[i]=='.')
			{
				int temp_index = i + 1;
				char data[2]="";
				strncpy(data,str_montly_amt + temp_index, 1);
				int a = atoi(data);
				if(a >= 5)
				{
					strncpy(data,str_montly_amt +( i - 1) , 1);
					a = atoi(data) + 1;
					sprintf(data,"%d",a);
					strncpy(str_montly_amt + ( i -1 ),data, 1);
					str_montly_amt[i+1]='0';
				}
				break;
			}
		}
		nmonthlyamount=atoi(str_montly_amt);
		//tempnmonthlyamount = nmonthlyamount ;
		sprintf(str_montly_amt, "%lf", nmonthlyamount );
		strncpy(auth_tx.TLF01_details.monthly_amort_amt,str_montly_amt, sizeof(auth_tx.TLF01_details.monthly_amort_amt)-1 );
		term = atof(term_value);
		ngrossamount = tempnmonthlyamount*term;
		sprintf(str_gross_amt, "%lf", ngrossamount );
		strncpy(auth_tx.TLF01_details.def_gross_amt, str_gross_amt, (sizeof(auth_tx.TLF01_details.def_gross_amt) -1));
		// Get MP parameter from trands. For that we need the product code from ccf02p table.
		retCode = Get_CCF02P_Details();
		return retCode ;

	}
	else
	{
		//INT rate and MFS rate both are empty for given number of months
		sprintf (strError, "Could not find INT rate for months:%s and amount:%lf ",term_value , namount) ;
		dcpimp_log_message( 2, 3, strError, " Process_select_mgf01_db_OK ", 1 );
		strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
		strcpy(auth_tx.TLF01_details.response_text, "Could not find INT rate");
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}
}

BYTE Process_select_ccf02p_db_OK (pBYTE pCurrent)
{
	INT   len;
	INT  term = 0;
	INT  i = 0;
	INT  j = 0;
   	CCF02P sCCF02P;
   	BYTE  retCode = 0;
   	CHAR  strError[512] = {0};
	CHAR Product_code[21]={0};
	CHAR temp_Product_code[3]={0};
	
	memset (&sCCF02P, 0, sizeof sCCF02P) ;
	memcpy (&sCCF02P, pCurrent+ sizeof auth_tx, sizeof sCCF02P) ;
	memset (&auth_tx.CCF02P_details, 0, sizeof (auth_tx.CCF02P_details)) ;
	memcpy (&auth_tx.CCF02P_details, pCurrent+ sizeof auth_tx, sizeof(auth_tx.CCF02P_details)) ;

	// We have to validate few thing in CCF02 and then get the product code
	/* if (card status is not 01 or 02), the card is invalid */
	if ((strcmp (sCCF02P.status, "01")) && (strcmp (sCCF02P.status, "02"))&&
		 (strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0))
	{
		// We Need to decide response code
		sprintf (strError, "Invalid card (CCF02P) status %s ",sCCF02P.status) ;
		dcpimp_log_message( 2, 3, strError, " Process_select_ccf02p_db_OK ", 1 );
		if( (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
			(auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
			(0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
		{
			// Do nothing we have to ignore this error as Its Void or reversal
		}
		else
		{	//log message in dcmpimp and do interactive db call.
			dcpimp_log_message( 2, 3, "Get PAF01 details through Interactive DB call",
										" Process_select_ccf02p_db_OK ", 1 );
			retCode = Get_PAF01_Details_for_CustrNbr_with_Wait ();

			strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
			strcpy(auth_tx.TLF01_details.response_text, "Invalid card status");
			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
			return DEVICE_ERROR;
		}
	}

    // Validate card experiey date
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
    {
	    retCode = Validate_Expiry_Date(auth_tx.TLF01_details.exp_date);
		if(retCode != NO_AUTH_ERROR)
		{
			sprintf (strError, "Failed to validate Expiry Date %s ",auth_tx.TLF01_details.exp_date) ;
			dcpimp_log_message( 2, 3, strError, " Process_select_ccf02p_db_OK ", 1 );
			if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
			   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
			   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
			{
				//Do nothing we have to ignore this error as Its Void or reversal
			}
			else
			{

				dcpimp_log_message( 2, 3, "Get PAF01 details through Interactive DB call",
											" Process_select_ccf02p_db_OK ", 1 );
				retCode = Get_PAF01_Details_for_CustrNbr_with_Wait ();

				strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
				strcpy(auth_tx.TLF01_details.response_text, "Invalid Expiry Date");
				memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
				return SYSTEM_ERROR ;
			}
		}
		else if(0 != strncmp( auth_tx.CCF02P_details.expire_date,
							  auth_tx.TLF01_details.exp_date,
							  (sizeof(auth_tx.TLF01_details.exp_date )-1)))
			{
				dcpimp_log_message( 2, 3, "Get PAF01 details through Interactive DB call",
										" Process_select_ccf02p_db_OK ", 1 );
				retCode = Get_PAF01_Details_for_CustrNbr_with_Wait ();
				strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
				strcpy(auth_tx.TLF01_details.response_text, "Invalid Expiry Date");
				memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
				return SYSTEM_ERROR ;
			}
		}

	// NO need to validate this if response is coming from BDO host
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
	{
		//We have got the Product code for the card number , now get MPPARA01 details
		// Currently we are supporting product code with 2 byte.
		strncpy(temp_Product_code, auth_tx.CCF02P_details.product_codes, 2);
        //returnNumMP(Product_code);
        for(i=0,j=0;i<2;i++)
        {
        	if(temp_Product_code[i]!=' ')
			{
        		Product_code[j]=temp_Product_code[i];
        		j++;
			}
        }
		if(strlen(Product_code)>0)
		{
			retCode = Get_MPPARA01_Details(&Product_code);
			return retCode ;
		}
		else
		{
			strcpy (strError, "Card number is not linked with Prodcut code ") ;
			dcpimp_log_message( 2, 3, strError, " Process_select_ccf02p_db_OK ", 1 );
		}
		// we need to approved as this merchant/card does not have any limit
		retCode = Get_PAF01_Details ();
		return retCode ; 
	}
	else
	{
		// Response is coming from BDO Host, we need to update ACF01 and CCF03P records
		retCode = Get_PAF01_Details ();
		return retCode ; 
	}
}

BYTE Process_select_ccf02p_db_OK_For_RT (pBYTE pCurrent)
{

   	CCF02P sCCF02P;
   	BYTE  retCode = 0;

	memset (&sCCF02P, 0, sizeof sCCF02P) ;
	memcpy (&sCCF02P, pCurrent+ sizeof auth_tx, sizeof sCCF02P) ;
	memset (&auth_tx.CCF02P_details, 0, sizeof (auth_tx.CCF02P_details)) ;
	memcpy (&auth_tx.CCF02P_details, pCurrent+ sizeof auth_tx, sizeof(auth_tx.CCF02P_details)) ;

	retCode = Get_PAF01_Details_For_RT ();
	return retCode ;
}

BYTE Process_select_ACF01_db_OK (pBYTE pCurrent)
{
	INT   len;
   	INT	 ret_val;
	INT  term = 0;
   	ACF01 sACF01;
   	BYTE  retCode = 0;
   	CHAR  strError[512] = {0};
	char Product_code[21]="";
	double nmonthlyamount = 0.0;
	double ngrossamount=0.0;
	double navailablebalance = 0.0;
	
	memset (&auth_tx.ACF01_details, 0, sizeof (auth_tx.ACF01_details)) ;
	memcpy (&auth_tx.ACF01_details, pCurrent+ sizeof auth_tx, sizeof (auth_tx.ACF01_details)) ;

	memset (&sACF01, 0, sizeof (sACF01)) ;
	memcpy (&sACF01, pCurrent+ sizeof auth_tx, sizeof sACF01) ;

	nmonthlyamount = (strtod (auth_tx.TLF01_details.monthly_amort_amt, 0) /100 ) ;
	ngrossamount    = (strtod (auth_tx.TLF01_details.def_gross_amt, 0)/100);
	navailablebalance = (strtod (sACF01.available, 0)/100 ) ;

	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
	{
		if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
		   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
		   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
		{
			//Do nothing we have to ignore this error as Its Void or reversal
			// Just Prepare CCF03P records and send response back to terminal
			retCode = Get_CCF03P_Details();
			return retCode ;
		}
		else
		{
			if(Dcpimp_Check_TxnType_is_ChipFallBack()==true)
			{
				retCode=Get_CR01_Details();
					return retCode ;
			}
			else if( ngrossamount <=navailablebalance)
			{
				retCode = Get_CCF03P_Details();
				return retCode ;
			}
			else
			{
				sprintf (strError, "Card number is not have available balance:%s :",auth_tx.TLF01_details.monthly_amort_amt) ;
				dcpimp_log_message( 2, 3, strError, " Process_select_ACF01_db_OK ", 1 );
				strcpy( auth_tx.TLF01_details.response_code, NO_SUFFICIENT_FUNDS );
				strcpy(auth_tx.TLF01_details.response_text, "Insufficient available balance");
				memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;

				return DEVICE_ERROR;
			}
		}
	}
	else
	{
		retCode = Get_CCF03P_Details();
		return retCode ;
	}
	
}


BYTE Process_select_ACF01_db_OK_For_RT (pBYTE pCurrent)
{
	INT   len;
   	INT	 ret_val;
	INT  term = 0;
   	ACF01 sACF01;
   	BYTE  retCode = 0;
   	CHAR  strError[512] = {0};
	char Product_code[21]="";
	double nmonthlyamount = 0.0;
	double ngrossamount=0.0;
	double navailablebalance = 0.0;

	memset (&auth_tx.ACF01_details, 0, sizeof (auth_tx.ACF01_details)) ;
	memcpy (&auth_tx.ACF01_details, pCurrent+ sizeof auth_tx, sizeof (auth_tx.ACF01_details)) ;

	memset (&sACF01, 0, sizeof (sACF01)) ;
	memcpy (&sACF01, pCurrent+ sizeof auth_tx, sizeof sACF01) ;

	retCode = Dcpimp_Send_Request_To_RT_Falcon_Service();
	if(DEVICE_ERROR == retCode)
	{
		sprintf( strError, "Failed to send for FAI FRT transaction to Falcon");
		dcpimp_log_message( 2, 1, strError, " Process_select_ACF01_db_OK_For_RT", 1 );
		retCode = Send_Transaction_To_Authorizer () ;
	}
	else
	{
		return retCode ;
	}
}

BYTE Process_select_PAF01_db_OK (pBYTE pCurrent)
{
	INT   len;
   	INT	 ret_val;
   	PAF01 sPAF01;
   	BYTE  retCode = 0;
   	CHAR  strError[512] = {0};

	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	memcpy (&sPAF01, pCurrent+ sizeof auth_tx, sizeof sPAF01) ;

	/* get Custr_nbr to ppoulate in CRTRAN F9, 10 in online mode */
	strncpy(auth_tx.pan_extended, sPAF01.Custr_nbr, sizeof(sPAF01.Custr_nbr));

	retCode = Get_ACF01_Details();
	return retCode ;
}

BYTE Process_select_PAF01_db_OK_For_RT (pBYTE pCurrent)
{
	INT   len;
   	INT	 ret_val;
   	PAF01 sPAF01;
   	BYTE  retCode = 0;
   	CHAR  strError[512] = {0};

	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	memcpy (&sPAF01, pCurrent+ sizeof auth_tx, sizeof sPAF01) ;

	/* get Custr_nbr to ppoulate in CRTRAN F9, 10 in real-time mode */
	strncpy(auth_tx.pan_extended, sPAF01.Custr_nbr, sizeof(sPAF01.Custr_nbr));

	retCode = Get_ACF01_Details_For_RT();
	return retCode;
}
BYTE Process_select_CR01_db_OK (pBYTE pCurrent)
{
	INT   len;
	   	INT	 ret_val;
		INT  term = 0;
	   	ACF01 sACF01;
	   	CR01   sCR01;
	   	BYTE  ReturnCode = 0;
	   	CHAR  strError[512] = {0};
	   	BYTE strTemp[1000] = {0};


		memset (&sCR01, 0, sizeof sCR01) ;
		memcpy (&sCR01, pCurrent+ sizeof auth_tx, sizeof sCR01) ;


		exception_cardnum_flag= Dcpimp_Check_Exception_Card_Num(sCR01);

		if(exception_cardnum_flag== NOT_BLOCKED || exception_cardnum_flag== EMPTY)

		{
			if((strncmp (sCR01.status, "A",1) ==0))
			{
				/* If the value is 0 in this field, then decline it outright.
				* The defined valued in this field, if is smaller than the total amount(DE4), then decline.
				*/
				if(0==strncmp(sCR01.single_amtlimit,"000000000000",12))
				{
							sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
							ReturnCode = Generate_Usage_Error_Message(1,
												strTemp, "Process_select_CR01_db_OK",
												GS_TXN_TRANSACTION_NOT_ALLOWED,
												sCR01.response_code);
							memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
							return ReturnCode;

				}
				else if (0!=strncmp(sCR01.single_amtlimit,"000000000000",12) && sCR01.single_amtlimit[0] != '\0')
				{
					ReturnCode = Dcpimp_Perform_Single_amount_limit(sCR01);
					if (ReturnCode != NO_AUTH_ERROR)
					{
						return ReturnCode;
					}
				}
				else
				{
						//no operation
				}


				//Check the rule to which this should be applied.
				if(strcmp(sCR01.rule_processing,"0") == 0)
				{
					// Installment flow
					memset(sCR01.future_use_1,0x00,sizeof(sCR01.future_use_1));
					strncpy(sCR01.future_use_1,"INSTALLMENT",11);

				}
				else
				{
						//Straight flow
					memset(sCR01.future_use_1,0x00,sizeof(sCR01.future_use_1));
					strncpy(sCR01.future_use_1,"STRAIGHT",8);
				}

				if(auth_tx.TLF01_details.system_date[0] == '\0')
				{
					create_system_date( auth_tx.TLF01_details.system_date );
				}
				if (0==strlen(sCR01.future_use_2))
				{

					strncpy(sCR01.future_use_2,auth_tx.TLF01_details.system_date,8);
					strncpy (sCR01.acc_amtlimit_counter, "000000000000",12) ;
					strncpy (sCR01.acc_transactionlimit_counter, "000000000000",12) ;


				}
				else if(0!=strncmp(sCR01.future_use_2,auth_tx.TLF01_details.system_date,8))
				{

					strncpy(sCR01.future_use_2,auth_tx.TLF01_details.system_date,8);
					strncpy (sCR01.acc_amtlimit_counter, "000000000000",12) ;
					strncpy (sCR01.acc_transactionlimit_counter, "000000000000",12) ;

				}



				ReturnCode = Dcpimp_Perform_Accumulated_Txn_Checks(sCR01);
				if(ReturnCode!=NO_AUTH_ERROR)
				{
							return ReturnCode ;
				}


				ReturnCode=Process_ACF01_Card_Balance_Check_Send_To_CCF03P();
				return ReturnCode;



			}
			else
			{
				ReturnCode=Process_ACF01_Card_Balance_Check_Send_To_CCF03P();
				return ReturnCode;

			}


		}
		else
		{
			ReturnCode=Process_ACF01_Card_Balance_Check_Send_To_CCF03P();
			return ReturnCode;

		}

		return(ReturnCode) ;
}


BYTE Process_select_CCF03P_db_OK (pBYTE pCurrent)
{
	INT   len;
	INT   i=0;
	INT   j=0;
	INT	 ret_val;
	int index = 0 ;
	CCF03 sCCF03P;
	BYTE  retCode = 0;
	CHAR  strError[512] = {0};
	INT   pcmaxTrnPerday = 0; // Max trn per day from Product code
	double pcnmaxAmountperday = 0.0; // Max amount per day from product code
	float authpercentoverlimit = 0.0;
	double nmonthlyamount = 0.0;
	double ngrossamount=0.0;
	CHAR time_date[25]="";
	CHAR date_str[9]="";
	double nACF01_Incremented_Outstanding =0.0;
	double nACF01_Decremented_Available = 0.0 ;
	double nACF01_CreditLimit_Available = 0.0 ;
	double noverlimitamount = 0.0;
	double nmaxoverlimitamount = 0.0;
	double nCurrentUsage = 0.0;
	BYTE strAmount[50] = {0} ;
    BYTE tempstr[101]="";
	BYTE temprspstr[13]="";
	CHAR filler[2]="0";
	BYTE	   temp_nfi_name[11];
	INT     nfactor_len=0;

	memset (&auth_tx.CCF03P_details, 0, sizeof (auth_tx.CCF03P_details)) ;
	memcpy (&auth_tx.CCF03P_details, pCurrent+ sizeof auth_tx, sizeof (auth_tx.CCF03P_details)) ;

	memset (&sCCF03P, 0, sizeof (sCCF03P)) ;
	memcpy (&sCCF03P, pCurrent+ sizeof auth_tx, sizeof sCCF03P) ;

	ptetime_get_timestamp(time_date);
	get_date(time_date,date_str);

	if (!If_Transaction_Occurred_On_Same_Day ())
	{
		strncpy (auth_tx.CCF03P_details.failed_pin_attempts, "0", 1) ;
		strncpy(private_ccf03p_struct.repeat[18].merchant_id,date_str, strlen(date_str));	
	}

	Update_Private_Ccf03p_Structure () ;

	// We are storing date field in repeat[18].merchant_id so storing it back
	strncpy(private_ccf03p_struct.repeat[18].merchant_id,date_str, strlen(date_str));	

	nmonthlyamount = (strtod (auth_tx.TLF01_details.monthly_amort_amt, 0) /100 ) ;
	ngrossamount    = (strtod (auth_tx.TLF01_details.def_gross_amt, 0)/100);
	
	// Check if this card is linked with a product code or not?
	//We have to validate Maximum number of transaction per day ,max transaction amount per day 
	if( strlen(auth_tx.CCF02P_details.product_codes) == 2)
	{
		pcmaxTrnPerday = atoi(auth_tx.TLF01_details.product_codes[10].code);
		pcnmaxAmountperday = strtod (auth_tx.TLF01_details.product_codes[10].amount, 0);

		if(0 ==strncmp(auth_tx.TLF01_details.date_yyyymmdd,private_ccf03p_struct.repeat[18].merchant_id, 
			strlen(auth_tx.TLF01_details.date_yyyymmdd)))
		{
			// Max trn per day check
			if ( private_ccf03p_struct.peso_usage[13].nbr_usage > 0 )
			{
				if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
				   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
				   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
				{
					private_ccf03p_struct.peso_usage[13].nbr_usage = private_ccf03p_struct.peso_usage[13].nbr_usage - 1;
				}
				else if((private_ccf03p_struct.peso_usage[13].nbr_usage > pcmaxTrnPerday )&&
					(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0))
				{
					sprintf (strError, "No of usage per day for Card is more that allowed:%s days",
							  auth_tx.TLF01_details.product_codes[10].code) ;
					dcpimp_log_message( 2, 3, strError, " Process_select_CCF03P_db_OK ", 1 );
					strncpy( auth_tx.TLF01_details.response_code, 
							 auth_tx.TLF01_details.product_codes[8].quantity,
							 (sizeof(auth_tx.TLF01_details.product_codes[8].quantity) -1 ) );
					strcpy(auth_tx.TLF01_details.response_text, "Card usage per day is more that allowed");
					return DEVICE_ERROR;
				}
				else
				{
					private_ccf03p_struct.peso_usage[13].nbr_usage = private_ccf03p_struct.peso_usage[13].nbr_usage + 1;
				}
			}
			else
			{
				private_ccf03p_struct.peso_usage[13].nbr_usage = 1;
			}

			// Max daily amount check 
			if ( private_ccf03p_struct.peso_usage[13].amt_usage > 0 )
			{
				if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
				   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
				   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
				{
					private_ccf03p_struct.peso_usage[13].amt_usage = private_ccf03p_struct.peso_usage[13].amt_usage - ngrossamount;
				}
				else if((private_ccf03p_struct.peso_usage[13].amt_usage > pcnmaxAmountperday)&&
					(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0))
				{
					sprintf (strError, "Transaction amount for a day is more that allowed: %s:",
										auth_tx.TLF01_details.product_codes[10].code) ;
					dcpimp_log_message( 2, 3, strError, " Process_select_CCF03P_db_OK ", 1 );
					strncpy( auth_tx.TLF01_details.response_code, 
							 auth_tx.TLF01_details.product_codes[9].quantity,
							 (sizeof(auth_tx.TLF01_details.product_codes[9].quantity) -1 ) );
					strcpy(auth_tx.TLF01_details.response_text, "Trn amt per day is more that allowed");
					return DEVICE_ERROR;
				}
				else
				{
					private_ccf03p_struct.peso_usage[13].amt_usage = private_ccf03p_struct.peso_usage[13].amt_usage + ngrossamount;
				}
			}
			else
			{
				private_ccf03p_struct.peso_usage[13].amt_usage = ngrossamount;
			}
		}
	}

	Update_CCF03_Structure( true);
	
	authpercentoverlimit = atof(auth_tx.TLF01_details.product_codes[8].amount);
	
	nACF01_Incremented_Outstanding = (strtod (auth_tx.ACF01_details.outstanding, 0) / 100);
	nACF01_Decremented_Available = (strtod (auth_tx.ACF01_details.available, 0) / 100);
	nACF01_CreditLimit_Available = (strtod (auth_tx.ACF01_details.credit_limit, 0) / 100);

	//Current Usage is computed as follows based on the available ATP card balance information: [Credit Limit] - [Available Balance]
	nCurrentUsage = nACF01_CreditLimit_Available - nACF01_Decremented_Available ;
	
	//if the authorzation amount of MP/installment shall fall within the percentage overlimit allowed, the authorization shall proceed (approve)
	noverlimitamount = nCurrentUsage + ngrossamount;
	nmaxoverlimitamount = (nACF01_CreditLimit_Available *(authpercentoverlimit / 100));

	//Let us confirm weather we have MP prodcut code or not, if No mp product code then we will not validate Auth Over limit %
	if((strlen(auth_tx.CCF02P_details.product_codes)== 2)&&
		(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0))
	{
		if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
		   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
		   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
		{
			//Do nothing we have to ignore this error as Its Void or reversal
		}
		else if( noverlimitamount > nmaxoverlimitamount)
		{
			strcpy (strError, "Authorzation amount is beyond the percentage overlimit allowed") ;
			dcpimp_log_message( 2, 3, strError, " Process_select_CCF03P_db_OK ", 1 );
			strncpy( auth_tx.TLF01_details.response_code,
					 auth_tx.TLF01_details.product_codes[10].quantity,
					 (sizeof(auth_tx.TLF01_details.product_codes[10].quantity) -1 ) );
			strcpy(auth_tx.TLF01_details.response_text, "Auth amt is beyond % overlimit allowed");
			return DEVICE_ERROR;
		}
	}

// We need to changes based on the reansaction
// For sale we should deduct the amount
// For void and reversal we shoul add the balance account
	if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
		(auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
		(0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
	{
		nACF01_Incremented_Outstanding = nACF01_Incremented_Outstanding -ngrossamount ;
		nACF01_Decremented_Available = nACF01_Decremented_Available + ngrossamount;
	}
	else
	{
		nACF01_Incremented_Outstanding = nACF01_Incremented_Outstanding + ngrossamount ;
		nACF01_Decremented_Available = nACF01_Decremented_Available - ngrossamount;
	}

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nACF01_Incremented_Outstanding, strAmount) ;
	LeftFill_String_With_Character (sizeof auth_tx.ACF01_details.outstanding-1, strAmount,'0') ;
	strcpy (auth_tx.ACF01_details.outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Available, strAmount) ;
	LeftFill_String_With_Character (sizeof auth_tx.ACF01_details.available-1, strAmount,'0') ;
	strcpy (auth_tx.ACF01_details.available, strAmount) ;

	/* special case - this is because TLF01 contains only <outstanding_balance> */
	strcpy (auth_tx.TLF01_details.outstanding_balance, auth_tx.ACF01_details.available) ;
	/* end special case */

	strcpy (auth_tx.TLF01_details.credit_limit, auth_tx.ACF01_details.credit_limit) ;

	/* Set ACF01_MASK and ccfop in the update_mask field of TLF01 */
	auth_tx.TLF01_details.update_mask[0] |= ACF01_MASK ;
	auth_tx.TLF01_details.update_mask[0] |= CCF03P_MASK ;
	//Update ccf03P records
	auth_tx.ccf03_insert = 0;

	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
	{
		strncpy(auth_tx.TLF01_details.response_code,"00", 2);
		//strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
		strcpy(auth_tx.TLF01_details.response_text, "APPROVED_STAND_IN");

		// Prepare field 63 that we need to send as response
		// 2 byte term , 7 byte factor, 12 byte month EMI and 12 byte gross amount
		// Term value
		strncpy(tempstr,auth_tx.TLF01_details.deferred_term_length,2 );
		//Factore value
		nfactor_len= strlen(auth_tx.TLF01_details.deferred_factor);
		if(nfactor_len > 0)
		{
			for( i=0; i < 7; i++)
			{
				if(auth_tx.TLF01_details.deferred_factor[i]=='.')
				{
					break;
				}
			}
			for ( j=0; j<(7-i);j++ )
			{
				temprspstr[j]=filler[0];
			}
			strncpy(&temprspstr[0]+ j,auth_tx.TLF01_details.deferred_factor,(7-j));
			strncpy(tempstr + 2  ,temprspstr, 7 );
		}
		else
		{
			// Its Zero INT rate
			strncpy(tempstr + 2  ,"0000000", 7 );
		}
		// Monthy EMI
		for( i=0; i < 12; i++)
		{
			if(auth_tx.TLF01_details.monthly_amort_amt[i]=='.')
			{
				break;
			}
		}
		memset(temprspstr, 0,12);
		for ( j =0; j<(12-i);j++ )
		{
			temprspstr[j]=filler[0];
		}
		strncpy(&temprspstr[0]+ j,auth_tx.TLF01_details.monthly_amort_amt,(12-j));
		strncpy(tempstr + 9  ,temprspstr, 12 );
		strncpy(auth_tx.TLF01_details.monthly_amort_amt , temprspstr, 12);
		//strncpy(tempstr + 9  ,auth_tx.TLF01_details.monthly_amort_amt, sizeof(auth_tx.TLF01_details.monthly_amort_amt)-1 );
		//Groos Amount
		for( i=0; i < 12; i++)
		{
			if(auth_tx.TLF01_details.def_gross_amt[i]=='.')
			{
				break;
			}
		}
		memset(temprspstr, 0,12);
		for ( j =0; j<(12-i);j++ )
		{
			temprspstr[j]=filler[0];
		}
		strncpy(&temprspstr[0]+ j,auth_tx.TLF01_details.def_gross_amt,(12-j));
		strncpy(tempstr + 21 ,temprspstr, (sizeof(auth_tx.TLF01_details.def_gross_amt) -1));
		strncpy(auth_tx.TLF01_details.def_gross_amt , temprspstr, 12);

		len = strlen( tempstr );
		/* Now copy the length Id into Auth_Tx and check it. */
		sprintf(auth_tx.future_use_1,"%04d",len);
		memcpy( auth_tx.future_use_1 + 4, tempstr, len);
		retCode = Update_All_Data ();
	}
	else
	{
		auth_tx.TLF01_details.tx_key = auth_tx.TLF01_details.tx_key + 100;
		retCode= Update_All_Data ();
	}
	return(retCode) ;
}

BYTE Process_select_mppara01_db_OK (pBYTE pCurrent)
{
	INT  len = 0;
   	INT	 ret_val = 0;
	INT  term = 0;
   	MPPARA01 sMPPARA01;
   	BYTE  retCode = 0;
   	CHAR  strError[512] = {0};
	CHAR  time_date[25] = {0};
	CHAR  date_str[9] = {0};
	double nmonthlyamount = 0.0;
	double ngrossamount=0.0;
	double namount = 0.0;
	
	memset (&sMPPARA01, 0, sizeof sMPPARA01) ;
	memcpy (&sMPPARA01, pCurrent+ sizeof auth_tx, sizeof sMPPARA01) ;

	//Validate date
	ptetime_get_timestamp(time_date);
	get_date(time_date,date_str);

	if(atoi(date_str) < atoi(sMPPARA01.effective_date))
	{
		if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
		   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
		   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
		{
			//Do nothing we have to ignore this error as Its Void or reversal
		}
		else
		{
			//System date is greater than expiry date return
			sprintf( strError, "system date: %s is less than effective date:%s of product code:%s ",
								date_str,sMPPARA01.effective_date, auth_tx.CCF02P_details.product_codes);
			dcpimp_log_message( 2, 3, strError, " Process_select_mppara01_db_OK ", 1 );
			strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
			strcpy(auth_tx.TLF01_details.response_text, "Invalid MP Product code effective date");
			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
			return DEVICE_ERROR;
		}
	}

    /*Retunr if Max No of Trans per day is zero*/
	if(0 == atoi(sMPPARA01.maxtrnperday))
	{
		sprintf (strError, "No of usage per day for Card is more that allowed:%s days",
				 sMPPARA01.maxtrnperday) ;
		dcpimp_log_message( 2, 3, strError, " Process_select_mppara01_db_OK ", 1 );
		strncpy( auth_tx.TLF01_details.response_code,
				sMPPARA01.authpara[0].response_code,
				(sizeof(auth_tx.TLF01_details.response_code) -1 ) );
		strcpy(auth_tx.TLF01_details.response_text, "Card usage per day is more that allowed");
		return DEVICE_ERROR;
	}
	 /*Retunr if Max Amt per day is zero*/
	if(0 == atoi(sMPPARA01.maxamtperday))
	{
		sprintf (strError, "Transaction amount for a day is more that allowed: %s:",
				 sMPPARA01.maxamtperday) ;
		dcpimp_log_message( 2, 3, strError, " Process_select_mppara01_db_OK ", 1 );
		strncpy( auth_tx.TLF01_details.response_code,
				 sMPPARA01.authpara[1].response_code,
				 (sizeof(auth_tx.TLF01_details.response_code) -1 ) );
		strcpy(auth_tx.TLF01_details.response_text, "Trn amt per day is more that allowed");
		return DEVICE_ERROR;

	}
	//Conver total amount into double
	namount = (strtod (auth_tx.TLF01_details.total_amount, 0) / 100 );
	
	nmonthlyamount = (strtod (auth_tx.TLF01_details.monthly_amort_amt, 0)/ 100 ) ;
	ngrossamount    = (strtod (auth_tx.TLF01_details.def_gross_amt, 0)/100);
	if( ngrossamount < atoi(sMPPARA01.minmajpurchamt))
	{
		if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
		   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
		   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
		{
			//Do nothing we have to ignore this error as Its Void or reversal
		}
		else
		{
			sprintf( strError, "Gross Amount %lf is less than Min Majour Purchage amount %s ",ngrossamount,sMPPARA01.minmajpurchamt);
			dcpimp_log_message( 2, 3, strError, " Process_select_mppara01_db_OK ", 1 );
			strcpy( auth_tx.TLF01_details.response_code, INVALID_AMOUNT );
			strcpy(auth_tx.TLF01_details.response_text, "Invalid Gross Amount");
			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
			return DEVICE_ERROR;
		}
	}

	if( ngrossamount > atoi(sMPPARA01.maxmajpurchamt))
	{
		if((auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
		   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
		   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
		{
			//Do nothing we have to ignore this error as Its Void or reversal
		}
		else
		{
			sprintf( strError, "Gross Amount %lf is greator than Maximum Majour Purchage amount %s ",ngrossamount,sMPPARA01.maxmajpurchamt);
			dcpimp_log_message( 2, 3, strError, " Process_select_mppara01_db_OK ", 1 );
			strcpy( auth_tx.TLF01_details.response_code, INVALID_AMOUNT );
			strcpy(auth_tx.TLF01_details.response_text, "Invalid Gross Amount");
			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
			return DEVICE_ERROR;
		}
	}

	/*
	if( ngrossamount < atoi(sMPPARA01.mininstamt))
	{

		sprintf( strError, "Monthly Amount %lf is less than Min Instalment amount %s ",ngrossamount,sMPPARA01.mininstamt);
		dcpimp_log_message( 1, 3,strError," Process_select_mppara01_db_OK ",1 );
		strcpy( auth_tx.TLF01_details.response_code, INVALID_AMOUNT );
		strcpy(auth_tx.TLF01_details.response_text, "Invalid Monthly Amount");
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}
	
	if( ngrossamount > atoi(sMPPARA01.maxinstamt))
	{
		sprintf( strError, "Monthly Amount %lf is greator than Maximum Instalment amount %s ",ngrossamount,sMPPARA01.minmajpurchamt);
		dcpimp_log_message( 1, 3,strError," Process_select_mppara01_db_OK ",1 );
		strcpy( auth_tx.TLF01_details.response_code, INVALID_AMOUNT );
		strcpy(auth_tx.TLF01_details.response_text, "Invalid Monthly Amount");
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}

	if( ngrossamount > atoi(sMPPARA01.minmajpurchlmt))
	{
		sprintf( strError, "Gross Amount %lf is greator than Maximum Majour Purchage Limit %s ",ngrossamount,sMPPARA01.minmajpurchlmt);
		dcpimp_log_message( 1, 3,strError," Process_select_mppara01_db_OK ",1 );
		strcpy( auth_tx.TLF01_details.response_code, INVALID_AMOUNT );
		strcpy(auth_tx.TLF01_details.response_text, "Invalid Gross Amount");
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}*/

	// We have done all the validation ,
	//We have to store Maximum number of transaction per day ,max transaction amount per day and Auth % over limit
	//product_codes[11].code : Max trans per day
	//product_codes[10].amount: Max Amt per day
	//product_codes[08].amount Auth % Over limit.
	strncpy(auth_tx.TLF01_details.product_codes[10].code,    sMPPARA01.maxtrnperday, (sizeof(sMPPARA01.maxtrnperday) -1 ));
	strncpy(auth_tx.TLF01_details.product_codes[10].amount, sMPPARA01.maxamtperday, (sizeof(auth_tx.TLF01_details.product_codes[10].amount)-1));
	strncpy(auth_tx.TLF01_details.product_codes[8].amount,   sMPPARA01.authoverlimit , (sizeof(sMPPARA01.authoverlimit) -1 ));

	// We Need to Store Response code for "Max No of Trans per day" , " Max Amount per day ", "Auth. % Overlimit"
	//Max No of Transaction per day
	strncpy(auth_tx.TLF01_details.product_codes[8].quantity,  sMPPARA01.authpara[0].response_code, (sizeof(auth_tx.TLF01_details.product_codes[8].quantity) -1 ));
	
	//Max Amount per day 
	strncpy(auth_tx.TLF01_details.product_codes[9].quantity,  sMPPARA01.authpara[1].response_code, (sizeof(auth_tx.TLF01_details.product_codes[9].quantity) -1 ));
	
	//Auth. % Overlimit
	strncpy(auth_tx.TLF01_details.product_codes[10].quantity,  sMPPARA01.authpara[2].response_code, (sizeof(auth_tx.TLF01_details.product_codes[10].quantity) -1 ));
	
	ret_val = Get_ACF01_Details();
	return(ret_val);

}


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
	BYTE	retCode = 0 ;
	retCode = Get_MGF01_Details ();
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
   strcpy (auth_tx.TLF01_details.transmission_timestamp, sBCH10.transmission_timestamp) ;
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
   strncpy (auth_tx.TLF01_details.def_gross_amt,   sBCH10.def_gross_amt,12) ;

		

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

         /* Original transaction for which the REVERSAL
          * is meant for, has been found in BCH10.  Set
          * flag so when it comes back from authorizer,
          * it will not get inserted into TLF01.  This
          * will happen after exiting this function.
          */
         bintype = Get_Bin_Type( auth_tx.TLF01_details.processing_code );

         if ( bintype == CREDIT_BIN )
            auth_tx.host2_acquirer_cb_code[0] = '*';
         // Let us check if its PL void sale
		 if( 0 == strncmp(auth_tx.TLF01_details.card_num, "7",1))
		 {
			retCode = Send_Transaction_To_Authorizer () ;
		 }
		 else if ( true == Check_Is_Host_Up(nciftr_module_name))
	 	 {	
		 		/*ncitr host is online*/
				retCode = Send_Transaction_To_Authorizer () ;
		 }
         else
         {
        	/* Respond to reversal immediately; they're dropped after update all */
			strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
			strcpy(auth_tx.TLF01_details.response_text, "APPROVED_STAND_IN");
			retCode = Get_MGF01_Details ();
         }
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
                  dcpimp_log_message( 1, 1, strError, " Process_select_bch10_db_OK ", 1 );
                  sprintf( strError,
                          "Late Reload Void2: Void time=%s, Reload time=%s",
                           current_datetime, reload_trans_datetime );
                  dcpimp_log_message( 1, 1, strError, " Process_select_bch10_db_OK ", 1 );
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
               /* In here lies EMV Tag 9f5b for a void. Concatenate it to the end of the EMV field. */
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

            // Let us check if its PL void sale
            if( 0 == strncmp(auth_tx.TLF01_details.card_num, "7",1))
            {
            	retCode = Send_Transaction_To_Authorizer () ;
            }
			// We are sending void request to BDO host and wating for their host replay
            else if ( true == Check_Is_Host_Up(nciftr_module_name))
   	 		{	
   		 		/*ncitr host is online*/
				retCode = Send_Transaction_To_Authorizer () ;
			}
  			else
            {
            	/* Process Void Transaction for MP*/
				strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
			   	retCode = Get_MGF01_Details ();
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

   return retCode ;
}/* threqust_process_select_bch10_db_reply */

BYTE Process_select_bch20_db_OK (pBYTE pCurrent)
{
   BYTE retCode = 0 ;
   BCH20 sBCH20 ;

   memset (&sBCH20, 0, sizeof sBCH20) ;
   memcpy (&sBCH20, pCurrent + sizeof auth_tx, sizeof sBCH20) ;

   strcpy (auth_tx.TLF01_details.orig_amount,            sBCH20.tran_amount) ;
   strcpy (auth_tx.TLF01_details.time_hhmmss,            sBCH20.tran_time) ;
   strcpy (auth_tx.TLF01_details.date_yyyymmdd,          sBCH20.tran_date) ;
   strcpy (auth_tx.TLF01_details.orig_retrieval_ref_num, sBCH20.primary_key.retrieval_ref_num) ;
   strcpy (auth_tx.TLF01_details.auth_number,            sBCH20.auth_number) ;
   strcpy (auth_tx.TLF01_details.orig_message,           sBCH20.message_type) ;
   strcpy (auth_tx.TLF01_details.orig_sys_trace_num,     sBCH20.sys_trace_audit_num) ;

   /* The following two are for the network reversals */
   strcpy (auth_tx.TLF01_details.transmission_timestamp, sBCH20.transmission_timestamp) ;
   strcpy (auth_tx.orig_local_time_date,                 sBCH20.transmission_timestamp) ;
   strcpy (auth_tx.TLF01_details.mcard_banknet,          sBCH20.mcard_banknet) ;

   /* For Amex - YYMMDDhhmmss */
   strcpy( auth_tx.local_date_time, sBCH20.tran_date+2 );
   strcat( auth_tx.local_date_time, sBCH20.tran_time   );

   if ( auth_tx.TLF01_details.tx_key == AUTH_REVERSAL )
   {
      auth_tx.host2_acquirer_cb_code[0] = '*';  /* Do not insert into TLF01 */
       // We are sending void request to BDO host and wating for their host replay
       // auth_tx.host2_acquirer_cb_code[0] = '*';

      if( 0 == strncmp(auth_tx.TLF01_details.card_num, "7",1))
      {
    	  retCode = Send_Transaction_To_Authorizer () ;
      }

      else if ( true == Check_Is_Host_Up(nciftr_module_name))
	  {	
			/*ncitr host is online*/
			retCode = Send_Transaction_To_Authorizer () ;
			//auth_tx.host2_acquirer_cb_code[0] = 0x00;
   	  }
      else
      {
        	/* Respond to Voids immediately; they're dropped after update all */
        	strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED);
			strcpy(auth_tx.TLF01_details.response_text, "APPROVED_STAND_IN");
			strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
      }
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
		 // We are sending void request to BDO host and wating for their host replay
		 // auth_tx.host2_acquirer_cb_code[0] = '*';
	     // Let us check if its PL void sale
	     if( 0 == strncmp(auth_tx.TLF01_details.card_num, "7",1))
	     {
	    	 retCode = Send_Transaction_To_Authorizer () ;
	     }

	     else if ( true == Check_Is_Host_Up(nciftr_module_name))
		 {
			/*ncitr host is online*/
			retCode = Send_Transaction_To_Authorizer () ;
			//auth_tx.host2_acquirer_cb_code[0] = 0x00;
		 }
		 else
		 {
			/* Respond to Voids immediately; they're dropped after update all */
			strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED);
			strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
			strcpy(auth_tx.TLF01_details.response_text, "APPROVED_STAND_IN");
			retCode = SEND_RESPONSE_TO_ORIGINATOR;
		 }
   }
   return retCode ;
}/* Process_select_bch20_db_OK */


BYTE Process_select_bch01_mp_db_OK (pBYTE pCurrent, BYTE subType2)
{
    BYTE retCode = 0 ;
	INT  result = 0;
	
	BCH01_MP sBCH01_MP ;

	printf("In Process_select_bch01_mp_db_OK function\n");
	memset (&sBCH01_MP, 0, sizeof sBCH01_MP);
	memcpy (&sBCH01_MP, pCurrent+ sizeof auth_tx, sizeof sBCH01_MP) ;

	/*	BCH01_MP record will always be found, because in the database function, if the
		BCH01_MP record is not found, it is created
	*/
															
    if ( subType2 == ST2_DB_SELECT_BCH01_BY_BATCH_NBR )			/*LM 09/29/03 SCR1220 - Duplicate Batch Processing*/
    {
      /* According to DCF01, this is a duplicate batch.
       * Perform further checks to see if it really is a duplicate.
       */
      result = is_batch_duplicate(sBCH01_MP);

      /* Pass the 'duplicate' result into a function to process duplicates. */
      (void)Process_Duplicate_Batch( result );
    }
    else
    {
      /* Batch is not a duplicate. Continue processing settlement as normal. */
      retCode = Process_Settlement(sBCH01_MP);
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

BYTE Process_update_bch01_mp_db_OK (BYTE type) 
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
      dcpimp_log_message( 1, 1, buffer, " Process_update_bch01_mp_db_OK ", 1 );
   }
   else if (type == BCH01_TOTALS_NOT_MATCHED)
   {
      Add_Response_Fields_To_Auth_Tx( auth_tx.TLF01_details.tx_key,
                                      INITIATE_BATCH_UPLOAD );
      auth_tx.TLF01_details.general_status = GS_TXN_REJECTED_BATCH ;
   }
   return type ;
} /* Process_update_bch01_mp_db_OK */

BYTE Process_insert_bch10_db_OK (void)
{
	BYTE strError[512] = {0} ;

	strcpy (strError, "BCH10 inserted successfully during batch upload - sending 00 response to terminal\n") ;
	PRINT (strError) ;
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
	{	
		auth_tx.TLF01_details.tx_key = AUTH_BATCH_UPLOAD_RESPONSE ;
		strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
		return BCH10_INSERT_SUCCESSFUL ;
	}
	else
	{
		return DEVICE_OK;
	}
} /* Process_insert_bch10_db_OK */

BYTE Process_select_MCF03P_db_OK (pBYTE pCurrent)
{
	CHAR   strError[256] = {0} ;
	MCF03P sMCF03P;
	double cumltv_amount= 0.0;
	double namount = 0.0;
	double nCreditAuthMax = 0.0;
	double nCreditPurchMax = 0.0;
	BYTE retCode = 0 ;

	memset (&sMCF03P, 0, sizeof sMCF03P) ;
	memcpy (&sMCF03P, pCurrent+ sizeof auth_tx, sizeof sMCF03P) ;

	if(auth_tx.MCF01_details.credit_auth_max[0] != 0)
	{
		nCreditAuthMax = (strtod (auth_tx.MCF01_details.credit_auth_max, 0) / 100.0);
	}

	if(auth_tx.MCF01_details.credit_purch_max[0] != 0)
	{
		nCreditPurchMax = (strtod (auth_tx.MCF01_details.credit_purch_max, 0) / 100.0);
	}

	cumltv_amount = atol(auth_tx.TLF01_details.total_amount)+
			  atol(sMCF03P.total_amount);
	namount = (cumltv_amount / 100.0);

	if (namount > nCreditPurchMax)
	{
		sprintf( strError, "Credit Auth max amount reached");
		dcpimp_log_message( 2, 1, strError, " Process_select_MCF03P_db_OK", 1 );
		strcpy( auth_tx.TLF01_details.response_code, pos_response_code);
		strcpy(auth_tx.TLF01_details.response_text, pos_res_txt);
		memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		return DEVICE_ERROR;
	}
	else
	{
		retCode = Send_Transaction_To_Authorizer () ;
		return retCode ;
	}

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

BYTE Process_mgf01_db_not_found (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	
	strcpy (strError, "MGF01 record NOT found during Stip operation for MP transaction ") ;
	dcpimp_log_message( 2, 3, strError, " Process_mgf01_db_not_found ", 1 );
	//We have to decide what to do in this case?
	strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
	strcpy(auth_tx.TLF01_details.response_text, "Invalid Merchant Group ID");
	memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
	return DEVICE_ERROR;

}

BYTE Process_ccf02p_db_not_found (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	BYTE	retCode = 0 ;
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)!=0)
	{	
		retCode= Update_All_Data ();
		return retCode;
	}
	
	strcpy (strError, "CCF02P record NOT found during Stip operation for MP transaction ") ;
	dcpimp_log_message( 2, 3, strError, " Process_ccf02p_db_not_found ", 1 );
	//We have to decide what to do in this case?
	strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
	strcpy(auth_tx.TLF01_details.response_text, "No Card details found");
	memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
	return DEVICE_ERROR ;

}

BYTE Process_ccf02p_db_not_found_For_RT (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	BYTE	retCode = 0 ;

	strcpy (strError, "CCF02P record NOT found for FAI FRT transaction") ;
	dcpimp_log_message( 2, 2, strError, " Process_ccf02p_db_not_found ", 1 );
	retCode = Dcpimp_Send_Request_To_RT_Falcon_Service();
	if(DEVICE_ERROR == retCode)
	{
		sprintf( strError, "Failed to send for FAI FRT transaction to Falcon");
		dcpimp_log_message( 1, 3, strError, " Process_ccf02p_db_not_found_For_RT", 1 );
		retCode = Send_Transaction_To_Authorizer () ;
		return retCode ;
	}
	else
	{
		return retCode ;
	}
}

BYTE Process_ACF01_db_not_found (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	BYTE	retCode = 0 ;
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)!=0)
	{
		retCode= Update_All_Data ();
		return retCode;
	}
	strcpy (strError, "ACF01 record NOT found during Stip operation for MP transaction ") ;
	dcpimp_log_message( 2, 3, strError, " Process_ACF01_db_not_found ", 1 );
	//We have to decide what to do in this case?
	strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
	strcpy(auth_tx.TLF01_details.response_text, "No Balance details found");
	memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
	return DEVICE_ERROR ;

}

BYTE Process_ACF01_db_not_found_For_RT (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	BYTE	retCode = 0 ;

	strcpy (strError, "ACF01 record NOT found for FAI FRT transaction") ;
	dcpimp_log_message( 2, 2, strError, " Process_ACF01_db_not_found_For_RT ", 1 );
	retCode = Dcpimp_Send_Request_To_RT_Falcon_Service();
	if(DEVICE_ERROR == retCode)
	{
		sprintf( strError, "Failed to send for FAI FRT transaction to Falcon");
		dcpimp_log_message( 1, 3, strError, " Process_ACF01_db_not_found_For_RT", 1 );
		retCode = Send_Transaction_To_Authorizer () ;
		return retCode ;
	}
	else
	{
		return retCode ;
	}

}

BYTE Process_PAF01_db_not_found (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	BYTE	retCode = 0 ;
	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)!=0)
	{
		retCode= Update_All_Data ();
		return retCode;
	}
	strcpy (strError, "PAF01 record NOT found during Stip operation for MP transaction ") ;
	dcpimp_log_message( 2, 3, strError, " Process_PAF01_db_not_found ", 1 );

	retCode = Get_ACF01_Details();
	return retCode ;

}
BYTE Process_PAF01_db_not_found_For_RT (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	BYTE	retCode = 0 ;

	strcpy (strError, "PAF01 record NOT found for FAI FRT transaction") ;
	dcpimp_log_message( 2, 2, strError, " Process_PAF01_db_not_found_For_RT ", 1 );

	retCode = Get_ACF01_Details_For_RT();
	return retCode ;
}
BYTE Process_CR01_db_not_found (pBYTE pCurrent)
{

	BYTE	ReturnCode = 0 ;
	ReturnCode=Process_ACF01_Card_Balance_Check_Send_To_CCF03P();
	return ReturnCode;

}
BYTE Process_CCF03P_db_not_found (pBYTE pCurrent)
{
	BYTE	retCode = 0 ;
	BYTE	strError[512] = {0} ;
	CCF03 sCCF03P;
	BYTE strTrimmedcard_type[2] ={0};
	BYTE strCardNumber[20]="";
	BYTE nAccountType = 0 ; 
	CHAR  time_date[25] = "";
	CHAR  date_str[9] = "";
	INT nIndex =0 ;
	CHAR strSystemDateTime[50]="";
	CHAR strTemp[100]="";
	LONG  nTranDateInJulian = 0L;
	CHAR strTranDateInJulian[10]="";
	INT   pcmaxTrnPerday = 0; // Max trn per day from Product code
	double pcnmaxAmountperday = 0.0; // Max amount per day from product code
	float authpercentoverlimit = 0.0;
	double nmonthlyamount = 0.0;
	double ngrossamount=0.0;
	double nACF01_Incremented_Outstanding =0.0;
	double nACF01_Decremented_Available = 0.0 ;
	double nACF01_CreditLimit_Available = 0.0 ;
	double noverlimitamount = 0.0;
	double nmaxoverlimitamount = 0.0;
	double nCurrentUsage = 0.0;
	BYTE strAmount[50] = {0} ;
    BYTE tempstr[101]="";
	int len =0 ;
	int i =0 ;
	int j =0;
	CHAR temprspstr[13]="";
	CHAR filler[2]="0";
    BYTE		  temp_nfi_name[11];
	INT     nfactor_len=0;
	
	strcpy (strCardNumber, auth_tx.TLF01_details.card_num) ;

	Get_Bin_Type_From_Processing_Code (&nAccountType) ;
	strTrimmedcard_type[0] = nAccountType ;
	
	strcpy (strError, "CCF03P record NOT found during Stip operation for MP transaction, Creating CCF03P record ") ;
	dcpimp_log_message( 2, 3, strError, " Process_CCF03P_db_not_found ", 1 );
	//We will crate it.
	strcpy (sCCF03P.primary_key.card_type, strTrimmedcard_type) ;
	strcpy (sCCF03P.primary_key.card_nbr, auth_tx.TLF01_details.card_num) ;
	strcpy (sCCF03P.status, auth_tx.CCF02P_details.status) ;
	strcpy (sCCF03P.primary_acct_nbr, auth_tx.CCF02P_details.primary_acct_nbr) ;
	strcpy (sCCF03P.failed_pin_attempts, "0") ;
	strcpy (sCCF03P.daily_amt_usage, "0") ;
	auth_tx.ccf03_insert = 1 ;

	for (nIndex = 0 ; nIndex <= 13; nIndex++)
	{
		strcpy (sCCF03P.peso_usage[nIndex].nbr_usage, "0") ;
		strcpy (sCCF03P.peso_usage[nIndex].amt_usage, "0") ;

		strcpy (sCCF03P.dollar_usage[nIndex].nbr_usage, "0") ;
		strcpy (sCCF03P.dollar_usage[nIndex].amt_usage, "0") ;
	}

	for (nIndex = 0 ; nIndex <= 19; nIndex++)
	{
		strcpy (sCCF03P.repeat[nIndex].merchant_id, "0") ;
		strcpy (sCCF03P.repeat[nIndex].nbr_uses, "0") ;
	}

	for (nIndex = 0 ; nIndex <= 19; nIndex++)
	{
		strcpy (sCCF03P.mcc[nIndex].category_code, "0") ;
		strcpy (sCCF03P.mcc[nIndex].usage, "0") ;
	}
	
	genutil_format_date (strSystemDateTime) ;
	memset (strTemp, 0, sizeof strTemp) ;

	memcpy (strTemp, strSystemDateTime, 8) ; /* date in yyyymmdd format */
	strcpy (sCCF03P.date_added, strTemp) ;
	nTranDateInJulian = Txutils_Calculate_Julian_Date (strTemp) ;
	ltoa (nTranDateInJulian, strTranDateInJulian, 10) ;
	strcpy (sCCF03P.peso_julian_date, strTranDateInJulian) ;
	strcpy (sCCF03P.dollar_julian_date, strTranDateInJulian) ;

	// We have to process this now.
	ptetime_get_timestamp(time_date);
	get_date(time_date,date_str);
	Update_Private_Ccf03p_Structure () ;
	
	private_ccf03p_struct.peso_julian_date = atol (sCCF03P.peso_julian_date) ;
	private_ccf03p_struct.dollar_julian_date = atol (sCCF03P.dollar_julian_date) ;
	strncpy(private_ccf03p_struct.repeat[18].merchant_id,date_str, strlen(date_str));	

	nmonthlyamount = (strtod (auth_tx.TLF01_details.monthly_amort_amt, 0) / 100 ) ;
	ngrossamount    = (strtod (auth_tx.TLF01_details.def_gross_amt, 0)/100);
	
	// 1st transaction for this card
	private_ccf03p_struct.peso_usage[13].nbr_usage = 1;
	private_ccf03p_struct.peso_usage[13].amt_usage = ngrossamount;

	strcpy (auth_tx.CCF03P_details.primary_key.card_type, strTrimmedcard_type) ;
	strcpy (auth_tx.CCF03P_details.primary_key.card_nbr,  auth_tx.TLF01_details.card_num) ;
	strcpy (auth_tx.CCF03P_details.primary_acct_nbr,  auth_tx.TLF01_details.card_num) ;
	Init_CCF03_Structure();
	Update_CCF03_Structure( true);

	authpercentoverlimit = atof(auth_tx.TLF01_details.product_codes[8].amount);
	
	nACF01_Incremented_Outstanding = (strtod (auth_tx.ACF01_details.outstanding, 0) / 100);
	nACF01_Decremented_Available = (strtod (auth_tx.ACF01_details.available, 0) / 100);
	
	nACF01_CreditLimit_Available = (strtod (auth_tx.ACF01_details.credit_limit, 0) / 100);
	
	//Current Usage is computed as follows based on the available ATP card balance information: [Credit Limit] - [Available Balance]
	nCurrentUsage = nACF01_CreditLimit_Available - nACF01_Decremented_Available ;
	
	//if the authorzation amount of MP/installment shall fall within the percentage overlimit allowed, the authorization shall proceed (approve)
	noverlimitamount = nCurrentUsage + ngrossamount;
	nmaxoverlimitamount = (nACF01_CreditLimit_Available *(authpercentoverlimit / 100));

	//Let us confirm weather we have MP prodcut code or not, if No mp product code then we will not validate Auth Over limit %
	if((strlen(auth_tx.CCF02P_details.product_codes)== 2)&&
		(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0))
	{
		if( noverlimitamount > nmaxoverlimitamount)
		{
			strcpy (strError, "Authorzation amount is beyond the percentage overlimit allowed") ;
			dcpimp_log_message( 2, 3, strError, " Process_CCF03P_db_not_found ", 1 );
			strncpy( auth_tx.TLF01_details.response_code,
					 auth_tx.TLF01_details.product_codes[10].quantity,
					 (sizeof(auth_tx.TLF01_details.product_codes[10].quantity) -1 ) );
			strcpy(auth_tx.TLF01_details.response_text, "Auth amt is beyond % overlimit allowed");
			return retCode;
		}
	}
		
	if( (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE)||
	   (auth_tx.TLF01_details.tx_key==AUTH_VOID_SALE_RESPONSE)||
	   (0==strncmp(auth_tx.TLF01_details.message_type,"04",2)))
	{
		nACF01_Incremented_Outstanding = nACF01_Incremented_Outstanding -ngrossamount ;
		nACF01_Decremented_Available = nACF01_Decremented_Available + ngrossamount;
	}
	else
	{
		nACF01_Incremented_Outstanding = nACF01_Incremented_Outstanding + ngrossamount ;
		nACF01_Decremented_Available = nACF01_Decremented_Available - ngrossamount;
	}

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nACF01_Incremented_Outstanding, strAmount) ;
	LeftFill_String_With_Character (sizeof auth_tx.ACF01_details.outstanding-1, strAmount,'0') ;
	strcpy (auth_tx.ACF01_details.outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Available, strAmount) ;
	LeftFill_String_With_Character (sizeof auth_tx.ACF01_details.available-1, strAmount,'0') ;
	strcpy (auth_tx.ACF01_details.available, strAmount) ;

	/* special case - this is because TLF01 contains only <outstanding_balance> */
	strcpy (auth_tx.TLF01_details.outstanding_balance, auth_tx.ACF01_details.available) ;
	/* end special case */

	strcpy (auth_tx.TLF01_details.credit_limit, auth_tx.ACF01_details.credit_limit) ;

	/* Set ACF01_MASK and ccfop in the update_mask field of TLF01 */
	auth_tx.TLF01_details.update_mask[0] |= ACF01_MASK ;
	auth_tx.TLF01_details.update_mask[0] |= CCF03P_MASK ;

	if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)==0)
	{
		strncpy(auth_tx.TLF01_details.response_code,"00", 2);
		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
		strcpy(auth_tx.TLF01_details.response_text, "APPROVED_STAND_IN");
		// Prepare field 63 that we need to send as response
		// 2 byte term , 7 byte factor, 12 byte month EMI and 12 byte gross amount
		// Term value
		strncpy(tempstr,auth_tx.TLF01_details.deferred_term_length,2 );
		//Factore value
		nfactor_len= strlen(auth_tx.TLF01_details.deferred_factor);
		if(nfactor_len > 0)
		{
			for( i=0; i < 7; i++)
			{
				if(auth_tx.TLF01_details.deferred_factor[i]=='.')
				{
					break;
				}
			}
			for ( j=0; j<(7-i);j++ )
			{
				temprspstr[j]=filler[0];
			}
			strncpy(&temprspstr[0]+ j,auth_tx.TLF01_details.deferred_factor,(7-j));
			strncpy(tempstr + 2  ,temprspstr, 7 );
		}
		else
		{
			// Its Zero INT rate
			strncpy(tempstr + 2  ,"0000000", 7 );
		}
		// Monthy EMI
		for( i=0; i < 12; i++)
		{
			if(auth_tx.TLF01_details.monthly_amort_amt[i]=='.')
			{
				break;
			}
		}
		memset(temprspstr, 0,12);
		for ( j =0; j<(12-i);j++ )
		{
			temprspstr[j]=filler[0];
		}
		strncpy(&temprspstr[0]+ j,auth_tx.TLF01_details.monthly_amort_amt,(12-j));
		strncpy(tempstr + 9  ,temprspstr, 12 );
		strncpy(auth_tx.TLF01_details.monthly_amort_amt , temprspstr, 12);
		//strncpy(tempstr + 9  ,auth_tx.TLF01_details.monthly_amort_amt, sizeof(auth_tx.TLF01_details.monthly_amort_amt)-1 );
		//Groos Amount
		
		for( i=0; i < 12; i++)
		{
			if(auth_tx.TLF01_details.def_gross_amt[i]=='.')
			{
				break;
			}
		}
		memset(temprspstr, 0,12);
		for ( j =0; j<(12-i);j++ )
		{
			temprspstr[j]=filler[0];
		}
		strncpy(&temprspstr[0]+ j,auth_tx.TLF01_details.def_gross_amt,(12-j));
		strncpy(tempstr + 21 ,temprspstr, (sizeof(auth_tx.TLF01_details.def_gross_amt) -1));
	//	strncpy(tempstr + 21 ,auth_tx.TLF01_details.def_gross_amt, (sizeof(auth_tx.TLF01_details.def_gross_amt) -1));
		strncpy(auth_tx.TLF01_details.def_gross_amt , temprspstr, 12);

		len = strlen( tempstr );
		/* Now copy the length Id into Auth_Tx and check it. */
		sprintf(auth_tx.future_use_1,"%04d",len);
		memcpy( auth_tx.future_use_1 + 4, tempstr, len);

		retCode = Update_All_Data ();
	}
	else
	{
		auth_tx.TLF01_details.tx_key = auth_tx.TLF01_details.tx_key + 100;
		retCode= Update_All_Data ();
	}
    return(retCode) ;

}


BYTE Process_mppara01_db_not_found (pBYTE pCurrent)
{
	BYTE	strError[512] = {0} ;
	CHAR 	product_code[3]={0};

	strncpy(product_code,auth_tx.CCF02P_details.product_codes,2);
	sprintf (strError, "MPPARA01 record  %s NOT found during Stip operation for MP transaction", product_code) ;
	dcpimp_log_message( 2, 3, strError, " Process_mppara01_db_not_found ", 1 );
	strcpy(auth_tx.TLF01_details.response_text, "Invalid MP Product code");
	strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
	memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
	return DEVICE_ERROR ;

}


BYTE Process_bin01_db_not_found (pBYTE pCurrent)
{
	BYTE retCode = 0 ;

	retCode = Process_When_Bin_Range_Not_Found () ;
	return retCode ;
} /* Process_bin01_db_not_found */

BYTE Process_mct01_db_not_found (pBYTE pCurrent)
{
	BYTE	retCode = 0 ;
	BYTE	strError[512] = {0} ;

 	strcpy (strError, "MCT01 record NOT found during ATP STIP") ;
 	dcpimp_log_message( 2, 3, strError, " Process_mct01_db_not_found ", 1 );
 	strcpy(auth_tx.TLF01_details.response_text, "Card Plan not allowed on MID");
 	strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
 	memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
 	return DEVICE_ERROR ;

} /* Process_mct01_db_not_found */

BYTE Process_bch10_db_not_found (pPTE_MSG p_msg_in, pBYTE pCurrent)
{
    CHAR strError[200] = {0} ;
	BYTE retCode = 0 ;
    
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

			retCode = Get_BCH20_Details (ST2_DB_SELECT_BCH20_BY_STAN) ;
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
		case AUTH_OFFLINE_REFUND:
					
			/*	These are not REPEATS, so now fetch the merchant record, if merchant is
				found, then send the transaction to the authorizer 
			*/
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
			 dcpimp_log_message( 2, 2, Buff, " Process_bch20_db_not_found ", 1 );
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
         dcpimp_log_message( 2, 2, Buff, " Process_bch20_db_not_found ", 1 );
         /* Send an approved response. */
		strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
		/* Set general status if original txn not found.
		   We changed while supporting void and reversal txn for MasterCard
		   Let be it set for all brands. because this not make much changes.
		   and also easy to identify (09-76) 11 Nov,2006 TF-Ajay */
		auth_tx.TLF01_details.general_status= GS_ORIGINAL_TXN_NOT_FOUND_FOR_VOID_SALE ;
		return SEND_RESPONSE_TO_ORIGINATOR ;
		break ;
	}

	retCode = Get_MCF01_Details () ;
	return retCode ;
} /* Process_bch20_db_not_found */



BYTE Process_bch01_mp_db_not_found (pBYTE pCurrent, BYTE subtype2)
{
   CHAR strError[200] = {0} ;
   BYTE retCode = 0 ;
   BYTE MatchingResponseKey = 0 ;

   printf("In Process_bch01_mp_db_not_found function\n");

   /*LM 09/29/03 SCR1220 - Duplicate Batch Processing Design*/

    /* Check subtype 2 to determine if we are dealing with duplicate batch. */
    if ( subtype2 == ST2_DB_SELECT_BCH01_BY_BATCH_NBR )
    {
      /* We are dealing with a duplicate batch situation, however, we were
       * unable to find the duplicate in BCH01_MP, so continue normal processing
       * of settlement transaction.  Start by getting the Active batch.
       */
      retCode = Get_BCH01_MP_Details( ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS );
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
                                       "Process_bch01_mp_db_not_found",
                                        GS_TXN_SYSTEM_ERROR,
                                        INVALID_TERMINAL_ID );
   }

   return( true );
}/* Process_bch01_mp_db_not_found */


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
	BYTE	strError[512] = {0} ;

	strcpy (strError, "BIN01 record NOT found during ATP STIP") ;
	dcpimp_log_message( 2, 3, strError, " Process_When_Bin_Range_Not_Found ", 1 );

	strcpy(auth_tx.TLF01_details.response_text, "BIN details Not Found");
	strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
	memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
	return DEVICE_ERROR ;
} /* Process_When_Bin_Range_Not_Found */

BYTE Process_When_Bin_Range_Found (void) 
{
	BYTE retCode = 0 ;

	//copy bin low to display in tranlookup
	strncpy(auth_tx.TLF01_details.product_codes[2].code,auth_tx.BIN01_details.primary_key.low,6);
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
	strncpy(auth_tx.TLF01_details.product_codes[2].code,auth_tx.BIN01_details.primary_key.low,6);
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
         if (!strEqual (strDeviceMerchant, strTransactionMerchant))
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

	/* 
	if (!IsFieldNull (sMCF01.block_start_date) &&
		!IsFieldNull (sMCF01.block_end_date))
	{
		BlockStartDate = atol (sMCF01.block_start_date) ;
		BlockEndDate = atol (sMCF01.block_end_date) ;

		
		if (BlockStartDate > 0 && BlockEndDate > 0 && 
			BlockEndDate >= BlockStartDate)
		{
			genutil_format_date (strSystemDateTime) ;
			memcpy (strTemp, strSystemDateTime, 8) ; 
			TransactionDate = Txutils_Calculate_Julian_Date (strTemp) ;

			if (TransactionDate >= BlockStartDate &&
				TransactionDate <= BlockEndDate)
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
	}
	*/
	return DEVICE_OK ;
} /* Edit_MCF01_Record */

BYTE Process_MCF03P_db_not_found (void)
{
	BYTE	retCode = 0 ;
	BYTE	strError[512] = {0} ;

 	sprintf (strError, "MCF03P record NOT found,1st transaction for MID %s",
 				auth_tx.TLF01_details.merchant_id) ;
 	dcpimp_log_message( 2, 1, strError, " Process_MCf03P_db_not_found ", 1 );

 	retCode = Send_Transaction_To_Authorizer () ;
 	return retCode ;

} /* Process_mct01_db_not_found */

BYTE Process_ACF01_Card_Balance_Check_Send_To_CCF03P(void)
{
	double nmonthlyamount = 0.0;
	double ngrossamount=0.0;
	double navailablebalance = 0.0;
	BYTE  retCode = 0;
	CHAR  strError[512] = {0};

	nmonthlyamount = (strtod (auth_tx.TLF01_details.monthly_amort_amt, 0) /100 ) ;
	ngrossamount    = (strtod (auth_tx.TLF01_details.def_gross_amt, 0)/100);
	navailablebalance = (strtod (auth_tx.ACF01_details.available, 0)/100 ) ;
	if( ngrossamount <=navailablebalance)
	{
		retCode = Get_CCF03P_Details();
		return retCode ;
	}
	else
	{
			sprintf (strError, "Card number is not have available balance:%s :",auth_tx.TLF01_details.monthly_amort_amt) ;
			dcpimp_log_message( 2, 3, strError, " Process_select_ACF01_db_OK ", 1 );
			strcpy( auth_tx.TLF01_details.response_code, NO_SUFFICIENT_FUNDS );
			strcpy(auth_tx.TLF01_details.response_text, "Insufficient available balance");
			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;

			return DEVICE_ERROR;
	}
}

INT Dcpimp_Check_Exception_Card_Num(CR01 sCR01)
{
   CHAR  strTemp[256] = {0} ;
   BYTE  ReturnCode = 0 ;
   INT   i=0;
   INT   ret_val = EMPTY;

   if ( 0x00 != sCR01.cardnum_blocking[i].exception_cardnum[0] )
   {

            ret_val = NOT_BLOCKED;
            for ( i = 0; i <= 50; i++ )
            {
               if(( strcmp(sCR01.cardnum_blocking[i].exception_cardnum, auth_tx.TLF01_details.card_num) == 0))
               {
                  ret_val = BLOCKED ;
                  break;
               }
            }


   }
   return( ret_val );
}

BYTE Dcpimp_Perform_Single_amount_limit (CR01 sCR01)
{
	double nSingle_AMTLimit = 0.0 ;
	double nTotalAmount =0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;


	nTotalAmount = (strtod (auth_tx.TLF01_details.total_amount, 0) / 100.0) ;
		nSingle_AMTLimit = (strtod (sCR01.single_amtlimit, 0) / 100.0) ;

		if (nTotalAmount > nSingle_AMTLimit)
		{
			sprintf (strTemp,"Chip fallback decline rule %s",sCR01.rule_id) ;

			ReturnCode = Generate_Usage_Error_Message( 1,
	                                                      strTemp,
	                                                     "TxDataServer_Perform_Single_amount_limit",
	                                                      GS_TXN_TRANSACTION_NOT_ALLOWED,
														  sCR01.response_code) ;
			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
			return ReturnCode ;
		}
	return NO_AUTH_ERROR ;
}

BYTE Dcpimp_Perform_Accumulated_Txn_Checks(CR01 sCR01)
{

	int CR01_Txn_Counter_Value;
	double CR01_Amt_Counter_Value=0.0;
	double CR01_Amt_Counter_Incremented=0.0;
	int CR01_Txn_Acc_limit_Value;
	double CR01_Amt_Acc_Limit_Value=0.0;
	BYTE  strAmount[50] = {0} ;
	BYTE  strAmount_limit[50] = {0} ;
	BYTE		strTemp[256] = {0} ;
   	double CF_nTotalAmount=0.0;
	double nConversionRate =0;
	double nConvertedTxAmount=0.0;
	BYTE ReturnCode = 0 ;

	int	CurrencyType = 0 ;

	CF_nTotalAmount = (strtod (auth_tx.TLF01_details.total_amount, 0) / 100.0) ;


	if (strEqual (auth_tx.TLF01_details.currency_code, PESO_CURRENCY))
		CurrencyType = PESO ;
	else if (strEqual (auth_tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
		CurrencyType = DOLLAR ;

	if (CurrencyType == PESO)
		{
			if(sCR01.acc_amtlimit[0]!='\0')
        {
        	if(Dcpimp_Check_TxnType_is_ChipFallBack()==true)
        	{

        		if(0!=strncmp(sCR01.acc_amtlimit,"000000000000",12) || sCR01.acc_transactionlimit!="0")
        		{
        			//transaction counter increment
        			CR01_Txn_Counter_Value = atoi(sCR01.acc_transactionlimit_counter);
        			CR01_Txn_Acc_limit_Value = atoi(sCR01.acc_transactionlimit);
        			CR01_Txn_Counter_Value += 1;

        			memset(strTemp, 0, sizeof(strTemp));
        			itoa(CR01_Txn_Counter_Value, strTemp, 10);
        			strcpy(sCR01.acc_transactionlimit_counter, strTemp);

        			// Accumulated amount counter increment
        			 CR01_Amt_Counter_Value = (strtod(sCR01.acc_amtlimit_counter, 0)/ 100.0);
        			 CR01_Amt_Acc_Limit_Value = (strtod(sCR01.acc_amtlimit, 0) / 100.0);

        			 CR01_Amt_Counter_Incremented =CR01_Amt_Counter_Value+ CF_nTotalAmount;

        			 //The accumulated amount field should always be greater than the counter value.
        			if (CR01_Amt_Counter_Incremented > CR01_Amt_Acc_Limit_Value)
        			{
        					sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
        					ReturnCode = Generate_Usage_Error_Message(
        												1, strTemp, "Dcpimp_Perform_Accumulated_Txn_Checks",
        												GS_TXN_TRANSACTION_NOT_ALLOWED,
        												sCR01.response_code);
        					memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
        					return ReturnCode;
        			}
        			//The accumulated txn field should always be greater than the counter value.
        			if (CR01_Txn_Counter_Value > CR01_Txn_Acc_limit_Value)
        			{
        					sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
        					ReturnCode = Generate_Usage_Error_Message(
        												1, strTemp, "Dcpimp_Perform_Accumulated_Txn_Checks",
        												GS_TXN_TRANSACTION_NOT_ALLOWED,
        												sCR01.response_code);
        					memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
        										return ReturnCode;
        			}

        			//convert the amount counter value into 12 characters.
        			Convert_Amt_To_Auth_Tx_String_Format(CR01_Amt_Counter_Incremented,strAmount);
            	    LeftFill_String_With_Character (sizeof sCR01.acc_amtlimit_counter-1, strAmount,'0') ;
            		strcpy (sCR01.acc_amtlimit_counter, strAmount) ;

            		//Insert the updated values into the db
					ReturnCode=Update_CR01_Details(sCR01);
					memset(&sCR01,0x00,sizeof(sCR01));
        		}
        		else
        		{
        			sprintf (strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id) ;
        			ReturnCode = Generate_Usage_Error_Message( 1,
		                                                      strTemp,
		                                                     "Dcpimp_Perform_Accumulated_Txn_Checks",
		                                                      GS_TXN_TRANSACTION_NOT_ALLOWED,
															  sCR01.response_code) ;
        			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
        			return ReturnCode;

        		}
        	}
        }
		}

	if (CurrencyType == DOLLAR)
			{
		if(sCR01.acc_amtlimit[0]!='\0')
		        {
		             if(Dcpimp_Check_TxnType_is_ChipFallBack()==true)
		             {

		            	 if(0!=strncmp(sCR01.acc_amtlimit,"000000000000",12) || sCR01.acc_transactionlimit!="0")
		                	{
		            		 	 //transaction counter increment
		            		    CR01_Txn_Counter_Value = atoi(sCR01.acc_transactionlimit_counter);
		            		    CR01_Txn_Acc_limit_Value = atoi(sCR01.acc_transactionlimit);
		            		    CR01_Txn_Counter_Value += 1;

		            		    memset(strTemp, 0, sizeof(strTemp));
		            		    itoa(CR01_Txn_Counter_Value, strTemp, 10);
		            		    strcpy(sCR01.acc_transactionlimit_counter, strTemp);

		            		    // Accumulated amount counter increment
		            		     CR01_Amt_Counter_Value = (strtod(sCR01.acc_amtlimit_counter, 0)/ 100.0);
		            		     CR01_Amt_Acc_Limit_Value = (strtod(sCR01.acc_amtlimit, 0) / 100.0);

		            		     CR01_Amt_Counter_Incremented =CR01_Amt_Counter_Value+ CF_nTotalAmount;

		            		      //The accumulated amount field should always be greater than the counter value.
		            		       if (CR01_Amt_Counter_Incremented > CR01_Amt_Acc_Limit_Value)
		            		       {
		            		               sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
		            		                ReturnCode = Generate_Usage_Error_Message(
		            		                 								1, strTemp, "Dcpimp_Perform_Accumulated_Txn_Checks",
		            		                 							    GS_TXN_TRANSACTION_NOT_ALLOWED,
		            		                 							   sCR01.response_code);
		            		                memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		            		                 return ReturnCode;
		            		        }
		            		        //The accumulated txn field should always be greater than the counter value.
		            		       if (CR01_Txn_Counter_Value > CR01_Txn_Acc_limit_Value)
		            		        {
		            		                 sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
		            		                 ReturnCode = Generate_Usage_Error_Message(
		            		                 						1, strTemp, "Dcpimp_Perform_Accumulated_Txn_Checks",
		            		                 						GS_TXN_TRANSACTION_NOT_ALLOWED,
		            		                 						sCR01.response_code);
		            		                 memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		            		                 return ReturnCode;
		            		       }

		            		        //convert the amount counter value into 12 characters.
		            		        Convert_Amt_To_Auth_Tx_String_Format(CR01_Amt_Counter_Incremented,strAmount);
		            		        LeftFill_String_With_Character (sizeof sCR01.acc_amtlimit_counter-1, strAmount,'0') ;
		            		        strcpy (sCR01.acc_amtlimit_counter, strAmount) ;

		            		          //Insert the updated values into the db
		            		         ReturnCode=Update_CR01_Details(sCR01);
		            		         memset(&sCR01,0x00,sizeof(sCR01));
							}
		                	else
		                	{
		                			sprintf(strTemp, "ChipFallback rule decline %s \n",
										sCR01.rule_id);
		                			ReturnCode = Generate_Usage_Error_Message(1,
		                					strTemp, "Dcpimp_Perform_Accumulated_Txn_Checks",
											GS_TXN_TRANSACTION_NOT_ALLOWED,
											sCR01.response_code);
		                			memset(auth_tx.TLF01_details.auth_number, 0, sizeof(auth_tx.TLF01_details.auth_number)) ;
		                			return ReturnCode;

		                	}


		            }
		        }
			}
	return NO_AUTH_ERROR ;
}


INT Dcpimp_Check_TxnType_is_ChipFallBack()
{

	if(strncmp(auth_tx.TLF01_details.handler_queue,"dcpimp",6) == 0)
	{
		if((strncmp(auth_tx.TLF01_details.message_type,"0200",4) == 0 || strncmp(auth_tx.TLF01_details.message_type,"0100",4) == 0) &&
		strncmp(auth_tx.TLF01_details.pos_entry_mode,"08",2) == 0 )
		{
			return true;
		}
		else
		{
		 return false;
		}

	}




}
