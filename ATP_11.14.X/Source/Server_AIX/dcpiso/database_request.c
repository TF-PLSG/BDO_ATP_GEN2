/**************************************************************************************************
*  
* MODULE:      Database_Request.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard
*  
* DESCRIPTION: Contains database request routines
*
* APPLICATION: DCPISO
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\new_dcpiso\database_request.c  $
   
      Rev 1.16   May 26 2005 13:47:50   dirby
   Made subtype2 a variable into function insert_tlf01_details.  This
   is to support different actions when inserting into TLF01 - new
   additional action is to support processing of EMV TC Uploads.
   SCR 15371
   
      Rev 1.15   Oct 01 2004 15:08:00   dirby
   Added function to get TLF01 record.
   SCR 12626
   
      Rev 1.14   Jul 13 2004 17:06:08   mbysani
   Added new functionality for AUTO INIT and RELOAD transaction s
   
      Rev 1.13   Mar 12 2004 17:31:16   dirby
   Added function to send request to the data server to see if the
   current transaction is a duplicate.
   SCR 1222
   
      Rev 1.12   Feb 24 2004 11:28:00   dirby
   Modified to set bin type as debit for processing code nn4nnn
   for batch uploads.
   SCR 1351
   
      Rev 1.11   Oct 03 2003 10:23:26   svelaga
   Fixed 2 SCRs:
   1216: Make Card Verify like an Authorization.
   1221: "Quantity" not in host.upl file when batch uploaded
   
      Rev 1.10   Oct 02 2003 09:52:22   lmitchel
   Ver: 4.2.0.6; SCR1220 Implemented New Duplicate Batch Processing Design
   
      Rev 1.9   Mar 24 2003 17:20:34   dirby
   Store settlement_date into product_codes[19].code so it can be
   retrieved for Cirrus reversals.
   SCR 1063
   
      Rev 1.8   Nov 08 2002 11:25:32   dirby
   Added code to not insert record into TLF01 if it has already
   been inserted by the update_all process.
   SCR 845
   
      Rev 1.7   Aug 23 2002 11:46:48   dirby
   Added def_gross_amt and term to bch10 for batch uploads.
   SCR 255
   
      Rev 1.6   Jan 25 2000 10:03:12   sdasgupt
   Bug fixes
   
      Rev 1.5   Jan 21 2000 15:02:50   sdasgupt
   Bug fixes
   
      Rev 1.4   Jan 17 2000 09:05:22   sdasgupt
   Bug fixes
   
      Rev 1.3   Dec 11 1999 18:05:08   sdasgupt
    
   
      Rev 1.2   Dec 08 1999 18:23:38   sdasgupt
    
   
      Rev 1.1   Nov 22 1999 17:51:58   sdasgupt
    
   
      Rev 1.0   Nov 17 1999 13:18:10   sdasgupt
   Initial Release
   
      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "pteipc.h"
#include "global_definitions.h"  
#include "memmnger.h"
#include "database_request.h"
#include "database_response.h"
#include "process_incoming_msg.h"
#include "process_outgoing_msg.h"
#include "service_functions.h"
#include "txutils.h"
#include "ntutils.h"
#include "general_utilities.h"
#include "batch_upload.h"


extern AUTH_TX auth_tx ;


extern CHAR		updateds_que_name[];
extern CHAR		devds_que_name[];
extern CHAR		trands_que_name [] ;
extern CHAR		mrads_que_name[];
extern CHAR     mracon_que_name[MAX_QUE_NAME_SIZE+1];
extern CHAR     dcpiso_que_name[];

extern INT Mracon_Log_Enable_Flag;


BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ; /* defined to be 25000 */

BYTE Get_BIN01_Details (BYTE read_mode)
{
	LONG	nReturnCode = 0L ;
	BOOLEAN bReturnCode = DEVICE_OK ;
	BIN01	sBinDetails ;
	char	strBuffer[30] = {0} ;
	
	BYTE	strCardNumber[21] = {0} ;
	BYTE	strBinType[2] = {0} ;
	BYTE	nAccountType = 0 ; /* determines if the card is a CREDIT, DEBIT, FLEET, 
	                              or LOYALTY card */
	
	memset (&sBinDetails, 0, sizeof sBinDetails) ;
	strcpy (strCardNumber, auth_tx.TLF01_details.card_num) ;

	Get_Bin_Type_From_Processing_Code (&nAccountType) ;
	strBinType[0] = nAccountType ;

	if (read_mode == SHARED_MEMORY_READ)
	{
		nReturnCode = FindMemBinnRange (strCardNumber, strBinType,(BYTE*) &sBinDetails) ;
		if (nReturnCode >= 0L)
		{
			/* have found the bin range in shared memory */
			memcpy (&auth_tx.BIN01_details, &sBinDetails, sizeof sBinDetails) ;
			return RECORD_FOUND_IN_SHARED_MEMORY ; 
		}
		else if (-1L == nReturnCode)
		{
			/* bin range not found in shared memory */
			return RECORD_NOT_FOUND_IN_SHARED_MEMORY ;
		}
		else
		{
			/* problems with the shared memory, so get from database */
			return SYSTEM_PROBLEM_READING_SHARED_MEMORY ;
			
		}
	}
	else if (read_mode == DATABASE_READ)
	{
		memcpy (strBuffer, strCardNumber, strlen (strCardNumber)) ;
		memcpy (strBuffer+20, strBinType, strlen (strBinType)) ;
	
		bReturnCode = Send_Request_To_Service (	trands_que_name,
												MT_DB_REQUEST, 
			                                    ST1_DB_CHECK_BIN_RANGE, 
												0, BIN01_DATA, 
												(pBYTE) strBuffer, 
												sizeof strBuffer) ;
		if (false == bReturnCode) 
			return SYSTEM_ERROR ;
		
		return DEVICE_OK ;
	}
	return DEVICE_OK ;
} /* TxDataServer_Get_BIN01_Details */



BYTE Get_DCF01_Details (BYTE subtype2)
{
	DCF01 sDCF01 ;
	BYTE ReturnCode = 0 ;
	
	memset (&sDCF01, 0, sizeof sDCF01) ;
	strcpy(sDCF01.primary_key.device_id, auth_tx.TLF01_details.terminal_id) ;
	strcpy(sDCF01.merchant_id, auth_tx.TLF01_details.merchant_id) ;
		
	ReturnCode = Send_Request_To_Service (devds_que_name,MT_DB_REQUEST, 
										  ST1_DB_SELECT, subtype2, DCF01_DATA, 
										  (pBYTE) &sDCF01, sizeof sDCF01) ;
	return ReturnCode ;

} /* Get_DCF01_Details */

BYTE Get_MCT01_Details () 
{
	MCT01 sMCT01 ;
	BYTE returnCode = 0 ;
	char strTrimmedMerchantId[20] = {0} ;

	memset (&sMCT01, 0, sizeof sMCT01) ;

	strTrim (auth_tx.TLF01_details.merchant_id, strTrimmedMerchantId) ;
	
	strcpy (sMCT01.primary_key.card_type, auth_tx.BIN01_details.card_type) ;
	strcpy (sMCT01.primary_key.merchant_id, strTrimmedMerchantId) ;
	strcpy (sMCT01.primary_key.organization_id, auth_tx.TLF01_details.organization_id) ;
	
	returnCode = Send_Request_To_Service (	trands_que_name,
											MT_DB_REQUEST, 
											ST1_DB_SELECT, 
											0, 
											MCT01_DATA, 
											(pBYTE) &sMCT01, 
											sizeof sMCT01) ;
	return returnCode ;

} /* Get_MCT01_Details */

BYTE Get_MCF01_Details (void) 
{
	MCF01 sMCF01 ;
	BYTE retCode = 0 ;
	BYTE strTrimmedMerchantId[100] = {0} ;
	BYTE strTrimmedOrganizationId[100] = {0} ;
	
	memset (&sMCF01, 0, sizeof sMCF01) ;

	strTrim (auth_tx.TLF01_details.merchant_id, strTrimmedMerchantId) ;
	strTrim (auth_tx.TLF01_details.organization_id, strTrimmedOrganizationId) ;

	strcpy (sMCF01.primary_key.merchant_id, strTrimmedMerchantId);
	strcpy (sMCF01.primary_key.organization_id, strTrimmedOrganizationId);

	retCode = Send_Request_To_Service (devds_que_name,
											MT_DB_REQUEST, 
											ST1_DB_SELECT, 
											0, 
											MCF01_DATA, 
											(pBYTE) &sMCF01, 
											sizeof sMCF01) ;
	return retCode ;

} /* Get_MCF01_Details */


BYTE Get_TLF01_Details( BYTE subtype2 )
{
   TLF01  sTLF01;
   BYTE   ReturnCode;

   memset( &sTLF01, 0x00, sizeof(TLF01) );
 
  /* SCR 13515 - ThoughtFocus
   * Removing this part of the code and copy the mcard_banknet into 
   * the sTLF01 structure for sending it to the data server.
   */
  /*
   strcpy( sTLF01.terminal_id,  auth_tx.TLF01_details.terminal_id  );
   strcpy( sTLF01.card_num,     auth_tx.TLF01_details.card_num     );
   strcpy( sTLF01.total_amount, auth_tx.TLF01_details.total_amount );
   */
   strcpy( sTLF01.mcard_banknet, auth_tx.TLF01_details.mcard_banknet );

   ReturnCode = Send_Request_To_Service( devds_que_name,MT_DB_REQUEST,
                                         ST1_DB_SELECT, subtype2, TLF01_DATA,
                                        (pBYTE) &sTLF01, sizeof(TLF01) );
   return ReturnCode ;

} /* Get_TLF01_Details */


BYTE Get_BCH01_Details (BYTE retrieval_type)
{
	/*	The dataserver function  searches the BCH01 (terminal_id, batch_nbr = 'ACTIVE')
		If BCH01 record does not exist, it is created.
		If a duplicate batch is being verified, BCH01 is searched using the batch Number.
	*/
	BCH01 sBCH01 ;
	BYTE retCode = 0 ;
	memset (&sBCH01, 0, sizeof sBCH01) ;

	strcpy (sBCH01.primary_key.device_id, auth_tx.TLF01_details.terminal_id) ;
	strcpy (sBCH01.merchant_id, auth_tx.TLF01_details.merchant_id) ;

	if (retrieval_type == ST2_DB_SELECT_BCH01_BY_BATCH_NBR) 
		strcpy (sBCH01.primary_key.batch_nbr, auth_tx.TLF01_details.batch_number) ;
	else
		strcpy (sBCH01.primary_key.batch_nbr, "ACTIVE") ;
	
	if (strcmp (auth_tx.TLF01_details.organization_id,"GES") == 0)
		strcpy (sBCH01.visa_merchant_id,"GES") ;
	else
		strcpy(sBCH01.visa_merchant_id,auth_tx.TLF01_details.visa_merchant_id) ;

	retCode = Send_Request_To_Service ( devds_que_name,
										MT_DB_REQUEST, 
										ST1_DB_SELECT, 
										retrieval_type, 
										BCH01_DATA, 
										(pBYTE) &sBCH01, 
										sizeof sBCH01) ;
	return retCode ;

} /*     */


BYTE Get_BCH20_Details (BYTE retrievalType)
{
	/* Check if the original authorization transaction exists in bch20 */
	
	BCH20 sBCH20 ;
	BYTE retCode = 0 ;

	memset (&sBCH20, 0, sizeof sBCH20) ;
	strcpy (sBCH20.primary_key.device_id, auth_tx.TLF01_details.terminal_id) ;
	strcpy (sBCH20.merchant_id, auth_tx.TLF01_details.merchant_id) ;
	strcpy (sBCH20.primary_key.batch_nbr, "ACTIVE") ;
	/* Recall that the RRN carried in the ISO message was placed in the orig_retrieval_ref_num field in auth_tx */
	strcpy (sBCH20.primary_key.retrieval_ref_num, auth_tx.TLF01_details.orig_retrieval_ref_num) ;
	strcpy (sBCH20.sys_trace_audit_num, auth_tx.TLF01_details.sys_trace_audit_num) ;
	// Auth number  card number
	strcpy (sBCH20.auth_number , auth_tx.TLF01_details.auth_number) ;
	strcpy (sBCH20.card_nbr, auth_tx.TLF01_details.card_num) ;
	strncpy(sBCH20.tran_date,auth_tx.TLF01_details.date_yyyymmdd,(sizeof(sBCH20.tran_date)-1));
    if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strcpy (sBCH20.tran_amount,		auth_tx.TLF01_details.total_amount) ;
		strcpy (sBCH20.tran_date,		auth_tx.TLF01_details.date_yyyymmdd ) ;
		strncpy (sBCH20.invoice_nbr,	auth_tx.TLF01_details.invoice_number,
										( sizeof (auth_tx.TLF01_details.invoice_number) -1 )) ;
	}
    else if((0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"Y",1)) ||
    		(0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"y",1)) ||
			(retrievalType == ST2_DB_SELECT_BCH20_FOR_PREAUTH) ||
			(retrievalType == ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH ))
    {/*added ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH , for amex we need the offline amount AMEX03B_APRIL2024*/
    	strcpy (sBCH20.tran_amount,		auth_tx.TLF01_details.total_amount) ;
    	strcpy (sBCH20.tran_date,		auth_tx.TLF01_details.date_yyyymmdd ) ;
    	if(true == dcpiso_check_if_mid_is_smcc ())
    	{
    		strncpy(sBCH20.product_codes[18].quantity,"CF",2);
    	}
    }

    if ( ((retrievalType == ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT )||
    	  (retrievalType == ST2_DB_SELECT_BCH20_FOR_PREAUTH) ||
    	  (retrievalType == ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH )) &&
         (true == dcpiso_check_if_mid_is_smcc ()))
    {
		strncpy(sBCH20.product_codes[18].quantity,"CF",2);
    }


	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST, 
										ST1_DB_SELECT, 
										retrievalType, 
										BCH20_DATA, 
										(pBYTE) &sBCH20, 
										sizeof sBCH20) ;
	return retCode ;
} /* Get_BCH20_Details */

BYTE Get_BCH20_Details_for_incremental_Preauth (BYTE retrievalType)
{
	/* Check if the original authorization transaction exists in bch20 */

	BCH20 sBCH20 ;
	BYTE retCode = 0 ;

	memset (&sBCH20, 0, sizeof sBCH20) ;
	strcpy (sBCH20.merchant_id, auth_tx.TLF01_details.merchant_id) ;
	strcpy (sBCH20.card_nbr, auth_tx.TLF01_details.card_num) ;
	if(auth_tx.mac_key[0] == AUTH_CARD_VERIFICATION)
	{
		strcpy (sBCH20.processing_code,"380000") ;
	}
	else
	{
		strcpy (sBCH20.processing_code, auth_tx.TLF01_details.processing_code) ;
	}
	strncpy(sBCH20.tran_date,auth_tx.TLF01_details.date_yyyymmdd,(sizeof(sBCH20.tran_date)-1));

	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST,
										ST1_DB_SELECT,
										retrievalType,
										BCH20_DATA,
										(pBYTE) &sBCH20,
										sizeof sBCH20) ;
	return retCode ;
} /* Get_BCH20_Details_for_incremental_Preauth */

BYTE Get_BCH10_Details (BYTE retrievalType)
{
	BCH10 sBCH10 ;
	BYTE retCode = 0 ;

	memset (&sBCH10, 0, sizeof sBCH10) ;

	strcpy (sBCH10.primary_key.device_id,			auth_tx.TLF01_details.terminal_id) ;
	strcpy (sBCH10.merchant_id, auth_tx.TLF01_details.merchant_id);
	strcpy (sBCH10.primary_key.batch_nbr,			"ACTIVE") ;
	strcpy (sBCH10.sys_trace_audit_num,				auth_tx.TLF01_details.sys_trace_audit_num) ;
	/* Recall that the RRN carried in the ISO message was placed in the orig_retrieval_ref_num field in auth_tx */
	strcpy (sBCH10.primary_key.retrieval_ref_num,	auth_tx.TLF01_details.orig_retrieval_ref_num) ;

	if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strcpy (sBCH10.card_nbr,			auth_tx.TLF01_details.card_num) ;
		strcpy (sBCH10.tran_amount,			auth_tx.TLF01_details.total_amount) ;
		strcpy (sBCH10.tran_date,			auth_tx.TLF01_details.date_yyyymmdd ) ;
		strcpy (sBCH10.invoice_nbr,     	auth_tx.TLF01_details.invoice_number);
	}


	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST, 
										ST1_DB_SELECT, 
										retrievalType, 
										BCH10_DATA, 
										(pBYTE) &sBCH10, 
										sizeof sBCH10) ;
	return retCode ;
}
BYTE Get_BCH10_Details_Void_CUP (BYTE retrievalType)
{
	BCH10 sBCH10 ;
	BYTE retCode = 0 ;

	memset (&sBCH10, 0, sizeof sBCH10) ;

	strcpy (sBCH10.primary_key.device_id,			auth_tx.TLF01_details.terminal_id) ;
	strcpy (sBCH10.merchant_id, auth_tx.TLF01_details.merchant_id);
	strcpy (sBCH10.primary_key.batch_nbr,			"ACTIVE") ;
	strcpy (sBCH10.sys_trace_audit_num,				auth_tx.TLF01_details.sys_trace_audit_num) ;
	/* Recall that the RRN carried in the ISO message was placed in the orig_retrieval_ref_num field in auth_tx */
	strcpy (sBCH10.primary_key.retrieval_ref_num,	auth_tx.TLF01_details.orig_retrieval_ref_num) ;

	if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strcpy (sBCH10.card_nbr,			auth_tx.TLF01_details.card_num) ;
		strcpy (sBCH10.tran_amount,			auth_tx.TLF01_details.total_amount) ;
		strcpy (sBCH10.tran_date,			auth_tx.TLF01_details.date_yyyymmdd ) ;
		strcpy (sBCH10.invoice_nbr,     	auth_tx.TLF01_details.invoice_number);
		strcpy (sBCH10.processing_code,     	auth_tx.TLF01_details.processing_code);
		strcpy (sBCH10.message_type,     	auth_tx.TLF01_details.message_type);
	}


	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST,
										ST1_DB_SELECT,
										retrievalType,
										BCH10_DATA,
										(pBYTE) &sBCH10,
										sizeof sBCH10) ;
	return retCode ;
}

BYTE look_for_duplicate_txn( void )
{
   BCH10  sBCH10;
   BYTE   retCode;

   memset( &sBCH10, 0x00, sizeof(sBCH10) );
   strcpy( sBCH10.primary_key.device_id, auth_tx.TLF01_details.terminal_id );
   strcpy( sBCH10.merchant_id,           auth_tx.TLF01_details.merchant_id);
   strcpy( sBCH10.sys_trace_audit_num,   auth_tx.TLF01_details.sys_trace_audit_num);
   strcpy( sBCH10.message_type,          auth_tx.TLF01_details.message_type );


   retCode = Send_Request_To_Service( devds_que_name,
                                      MT_DB_REQUEST,
                                      ST1_DB_SELECT,
                                      ST2_DB_SELECT_BCH10_DUPLICATE_TXN,
                                      BCH10_DATA,
                                      (pBYTE) &sBCH10,
                                      sizeof(sBCH10) );
   return retCode ;

} /* look_for_duplicate_txn */



BYTE Update_BCH01_Details (BCH01 sBCH01, BYTE subType1, BYTE subType2) 
{
	BYTE retCode = 0 ;

	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST,
										subType1,
										subType2,
										BCH01_DATA, 
										(pBYTE) &sBCH01, 
										sizeof sBCH01) ;
	return retCode ;

}

BYTE Update_BCH20_Details_for_OFFLINE_Sale ( BYTE subType2)
{
	BCH20 sBCH20 ;
	BYTE retCode = 0 ;

	memset (&sBCH20, 0, sizeof sBCH20) ;
	strncpy (sBCH20.merchant_id, auth_tx.TLF01_details.merchant_id, (sizeof(sBCH20.merchant_id)-1)) ;
	strncpy (sBCH20.primary_key.batch_nbr, "ACTIVE", 6) ;
	strncpy (sBCH20.card_nbr, auth_tx.TLF01_details.card_num, (sizeof(sBCH20.card_nbr)-1)) ;


	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST,
										ST1_DB_UPDATE,
										subType2,
										BCH20_DATA,
										(pBYTE) &sBCH20,
										sizeof sBCH20) ;
	return retCode ;

}

BYTE Update_All_Data (void)
{
	BYTE retCode = 0 ;
	BYTE MatchingResponseKey = 0 ;
     BCH10  sBCH10;
	/* The following ensures that only responses go the updateds */

	MatchingResponseKey = Match_Request_To_Response_Transaction_Key () ;
	auth_tx.TLF01_details.tx_key = MatchingResponseKey ;
	//Girija GB test code for EFTPOS phase 2
	 strcpy(sBCH10.product_codes[6].amount,auth_tx.TLF01_details.pos_condition_code);
	 strcat(sBCH10.product_codes[6].amount,auth_tx.TLF01_details.service_code);


	retCode = Send_Request_To_Service (	updateds_que_name,
										MT_DB_REQUEST,
										ST1_DB_UPDATE,
										0,
										ALL_DATA, 
										0, 
										0) ;
	return retCode ;
} /* Update_All_Data */
BYTE Insert_MRA_AUTH_Details( BYTE subtype2 )
{
	BYTE retCode = 0 ;

     {
	   retCode = Send_Request_To_Service (	mrads_que_name,
                                          MT_DB_REQUEST,
                                          ST1_DB_INSERT,
                                          subtype2,
                                          MRA_AUTH_TBL_DATA, 
                                          (pBYTE) &auth_tx.TLF01_details,
                                          sizeof auth_tx.TLF01_details);
   	}
 
	return DEVICE_OK ;
} /* Insert_MRA_details */

BYTE Insert_TLF01_Details( BYTE subtype2 )
{
	BYTE retCode = 0 ;

   if ( auth_tx.host2_acquirer_cb_code[0] == '*' )
   {
      /* TLF01 has already been inserted. */
      retCode = DEVICE_OK;
   }
   else
   {
      auth_tx.host2_acquirer_cb_code[0] = '*';
      retCode = Send_Request_To_Service (	updateds_que_name,
                                          MT_DB_REQUEST,
                                          ST1_DB_INSERT,
                                          subtype2,
                                          TLF01_DATA, 
                                          (pBYTE) &auth_tx.TLF01_details,
                                          sizeof auth_tx.TLF01_details) ;
   }
	return retCode ;
} /* Insert_TLF01_details */

BYTE Insert_BCH10_Details_For_Batch_Upload (void)
{
   BCH10 sBCH10 ;
   BYTE retCode = 0 ;

   memset (&sBCH10, 0, sizeof sBCH10) ;

   strcpy (sBCH10.primary_key.device_id,         auth_tx.TLF01_details.terminal_id) ;
   strcpy (sBCH10.merchant_id,         auth_tx.TLF01_details.merchant_id) ;
   strcpy (sBCH10.primary_key.batch_nbr,        "ACTIVE") ;
   strcpy (sBCH10.primary_key.retrieval_ref_num, auth_tx.TLF01_details.orig_retrieval_ref_num) ;

  /* Open_date and open_time are populated in the dataserver function from the bch01 record */


   strcpy (sBCH10.processing_code,     auth_tx.TLF01_details.processing_code) ;
   strcpy (sBCH10.tran_amount,         auth_tx.TLF01_details.total_amount) ;
   strcpy (sBCH10.response_code,       auth_tx.TLF01_details.response_code) ;
   strcpy (sBCH10.auth_number,         auth_tx.TLF01_details.auth_number) ;
   strcpy (sBCH10.tran_date,           auth_tx.TLF01_details.date_yyyymmdd) ;
   strcpy (sBCH10.tran_time,           auth_tx.TLF01_details.time_hhmmss) ;
   strcpy (sBCH10.transaction_id,      auth_tx.TLF01_details.primary_key.transaction_id) ;
   strcpy (sBCH10.message_type,        auth_tx.TLF01_details.orig_message) ;
   strcpy (sBCH10.card_nbr,            auth_tx.TLF01_details.card_num) ;
   strcpy (sBCH10.invoice_nbr,         auth_tx.TLF01_details.invoice_number) ;
   strcpy (sBCH10.settle_file_prefix,  auth_tx.TLF01_details.settle_file_prefix) ;
   strcpy (sBCH10.product_code,        auth_tx.TLF01_details.product_code) ;
   strcpy (sBCH10.odometer,            auth_tx.TLF01_details.odometer) ;
   strcpy (sBCH10.sys_trace_audit_num, auth_tx.TLF01_details.sys_trace_audit_num) ;
   strcpy (sBCH10.def_gross_amt,       auth_tx.TLF01_details.def_gross_amt) ;
   strcpy (sBCH10.term,                auth_tx.TLF01_details.deferred_term_length) ;
   strncpy (sBCH10.product_codes[6].code, auth_tx.host2_message_seq_nbr +1 , 3) ;
   sBCH10.tx_key = auth_tx.TLF01_details.tx_key ;

   /* This line is put here for the Cirrus network controller.
    * There is no field in BCH10 for settlement date, which Cirrus network
    * controller must send to Cirrus in reversals.  So to avoid adding a
    * new field to BCH10, used the 20th product.code field.
    */

//Girija GB test code 
//strcpy(sBCH10.product_codes[6].amount,Eftpos_pos_condition_code );
//strcat(sBCH10.product_codes[6].amount,Eftpos_service_code);
   /* In inconsistencies in the logging on BCH10- TF Phani*/
   strcpy(sBCH10.product_codes[6].amount,auth_tx.TLF01_details.pos_condition_code);
   strcat(sBCH10.product_codes[6].amount,auth_tx.TLF01_details.service_code);



   strcpy(sBCH10.product_codes[19].code, auth_tx.TLF01_details.settlement_date);

   if (auth_tx.TLF01_details.processing_code[2] == '0' || 
       auth_tx.TLF01_details.processing_code[2] == '3' ||
       auth_tx.TLF01_details.processing_code[2] == '4')
      sBCH10.bin_type = 'C' ;
   else
      sBCH10.bin_type = 'D' ;

   strcpy (sBCH10.pos_entry_mode, auth_tx.TLF01_details.pos_entry_mode) ;
   strcpy (sBCH10.exp_date,       auth_tx.TLF01_details.exp_date) ;

   /* Ticket number needed for settlement file and log file */
   strcpy(sBCH10.ticket_nbr,		auth_tx.TLF01_details.ticket_nbr);
	/* TF PHANI - Add Prodcut 15 amount- Dt- 01 mrah*/
   strcpy(sBCH10.product_codes[14].amount,auth_tx.TLF01_details.product_codes[14].amount);

   /* The following line is commented out for COMFORT */
   /* strcpy (sBCH10.settlement_id,			auth_tx.TLF01_details.visa_merchant_id) ;  */
   /**/

   strcpy(sBCH10.profile, auth_tx.TLF01_details.profile) ;

   retCode = Send_Request_To_Service (	devds_que_name,
                                       MT_DB_REQUEST, 
                                       ST1_DB_INSERT, 
                                       ST2_DB_INSERT_BCH10_SETTLE, 
                                       BCH10_DATA, 
                                       (pBYTE) &sBCH10, 
                                       sizeof sBCH10) ;
   return retCode ;
}


BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
							  BYTE nSubType2, BYTE nAppDataType, 
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	INT nBufferSize = 0 ;
	char strError[512] = {0} ;
    CHAR logger_que_name[256] = {0} ;
	CHAR orig_comms_info [MAX_ORIG_INFO_SIZE] = {0} ; 

	memset (p_buffer, 0, sizeof p_buffer) ;
	memcpy (p_buffer, &auth_tx, sizeof auth_tx) ;
	
	if (p_data != 0)
		memcpy (p_buffer + sizeof auth_tx, p_data, nDataSize) ;
		
	nBufferSize = sizeof auth_tx + nDataSize ;

	p_msg_out = ptemsg_build_msg (  nType, 
								    nSubType1, 
								    nSubType2, 
								    que_name, 
								    application_que_name, 
								    (pBYTE) p_buffer, 
								    nBufferSize, 
								    nAppDataType) ;

	if (p_msg_out == NULL_PTR)
	{	if(strcmp(que_name,mrads_que_name) == 0)
			return DEVICE_OK;
		strcpy (strError,"Insufficient Memory for building terminal message") ;
		
		Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
									"Send_Request_To_Service",
									GS_TXN_SYSTEM_ERROR,
									SYSTEM_MALFUNCTION) ;
		return SYSTEM_ERROR ;
 	} 

	retCode = pteipc_send (p_msg_out, que_name) ;
	free (p_msg_out) ;

	if (retCode != 0)
	{	if(strcmp(que_name,mrads_que_name) == 0)
			return DEVICE_OK;
		sprintf (strError,"Error %ld has occurred sending message to %s\n", retCode, que_name) ;
        
		Generate_Usage_Error_Message (	SET_SYSTEM_MONITOR, strError,
										"Send_Request_To_Service",
										GS_TXN_SYSTEM_ERROR,
										SYSTEM_MALFUNCTION) ;
		return SYSTEM_ERROR ;
	}

	return DEVICE_OK ;
} /* Send_Request_To_Service */

BOOLEAN dcpiso_Send_Request_To_Mracon_Service ( void )
{
   BOOLEAN  retval = true;
   INT       nBufferSize;
   LONG     lReturnCode  = 0L;
   BYTE     ReturnCode   = 0;
   pPTE_MSG p_msg_out    = 0;
   CHAR     strTemp[256] = {0};

   memset( p_buffer, 0x00,     sizeof(p_buffer) );
   memcpy( p_buffer, &auth_tx, sizeof(AUTH_TX ) );
   nBufferSize = sizeof( AUTH_TX );

   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
                                 mracon_que_name,
                                 dcpiso_que_name,
								 (pBYTE) p_buffer,
                                 nBufferSize,
                                 0 );
   if (p_msg_out == NULL)
   {
	   if(Mracon_Log_Enable_Flag == 1)
	   {
		   sprintf( strTemp,"Mem allocate Err build msg for %s", mracon_que_name );
		   dcpiso_log_message( 1, 3, strTemp, "dcpiso_Send_Request_To_Mracon_Service",1 );
	   }
   }
   else
   {
      lReturnCode = pteipc_send (p_msg_out, mracon_que_name) ;
      if (lReturnCode != 0)
      {
    	  if(Mracon_Log_Enable_Flag == 1)
    	  {
			 sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,mracon_que_name);
			 dcpiso_log_message( 1, 3, strTemp, "dcpiso_Send_Request_To_Mracon_Service",1 );
    	  }
      }
      free( p_msg_out );
   }
   return( retval );
}




BYTE Retrieve_BIN01_Record (void) 
{
	BYTE  AccountType = 0 ; // determines if the card is a CREDIT, DEBIT, FLEET, or LOYALTY 
	BYTE ReturnCode = 0 ;
	Get_Bin_Type_From_Processing_Code (&AccountType) ;
	/* Set the bin_type in auth_tx - used later in grouping txns in the batch */
	auth_tx.bin_type = AccountType ;
	
	/* Get the BIN record for the card */
	/* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
	ReturnCode = Get_BIN01_Details (SHARED_MEMORY_READ) ;
	
   
	if (ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY)
	{

	   // bin range is not found in shared memory 
	   // if bin type is CREDIT, return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR
	   ReturnCode = Process_When_Bin_Range_Not_Found () ;
	   return ReturnCode ;
	}
	else if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY) // found the bin range in memory
	{
		// network, return NO_AUTH_ERROR
		
		// if system error return SYSTEM_ERROR
		ReturnCode = Process_When_Bin_Range_Found () ;
		return ReturnCode ;
	}
	else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
	{
		// if problems with reading shared memory, read the 
		// bin table from the database
		ReturnCode = Get_BIN01_Details (DATABASE_READ) ;
		return ReturnCode ;
	}
	return DEVICE_OK ;
} /* Retrieve_BIN01_Record */


/******************************************************************************
 *
 *  NAME:         PROCESS_SETTLEMENT
 *
 *  DESCRIPTION:  This function coninues processing of  an 0500 settlement transaction.
 *				  The batch has been determined not to be a duplicate and now processing
 *				  will continue to determine if a batch upload is required or if settlement
 *                can be approved.
 *
 *  INPUTS:       bch01   - (Global) Active batch from the database
 *                auth_tx - (Global) 0500 transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false if unable to process
 *
 *  AUTHOR:       Dennis Irby
 *				  9/29/03 LM - SCR1220 - Added for duplicate batch processing design	
 ******************************************************************************/
INT Process_Settlement(BCH01 sBCH01)
{
   INT   retCode = true;


   	if (If_Transaction_Total_Equals_Batch_Total (sBCH01))
	{
		/*	If the totals on the transaction match the batch totals, then
			we have to update the batch header (BCH01) with the real batch number,
			update the batch details (BCH10) with the real batch number,
			change batch_status to 'C', add the batch number to the device record etc 
		*/
		retCode = Process_When_Transaction_Total_Equals_Batch_Total (sBCH01) ;
		return retCode ;
	}
	else
	{
		/*	If the totals on the transaction DO NOT the batch totals, then
			we have to update the batch header (BCH01) with zero totals,
			update the batch details (BCH10) with 'ACTIVE' batch, delete all
			BCH10 details etc ...and if the database reply is OK, we send
			a '95' to the device to initiate a batch upload
		*/
		retCode = Process_When_Transaction_Total_Does_Not_Equal_Batch_Total (sBCH01) ;
		return retCode ;
	}
}

/******************************************************************************
 *
 *  NAME:         IS_BATCH_DUPLICATE
 *
 *  DESCRIPTION:  This function compares the current batch totals in auth_tx
 *                to a bch01 record that contains the same batch number.
 *
 *  INPUTS:       auth_tx - (Global) 0500 transaction record
 *                bch01   - (Global) Batch record from database
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   TRUE_DUPLICATE_BATCH = Totals & counts same in auth_tx & bch01
 *                FALSE_DUPLICATE_BATCH= Totals & counts not the same, but dcf01
 *                                       indicates batch is a duplicate
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_batch_duplicate(BCH01 sBCH01)
{
   INT   ret_val = FALSE_DUPLICATE_BATCH;
   INT   auth_tx_cnt;
   INT   bch01_cnt;
   LONG  auth_tx_amt;
   LONG  bch01_amt;

   /* Convert counts and amounts to numeric in case
    * leading zeroes are different in auth_tx and bch01.
    */

   /* Check credit count */
   auth_tx_cnt = atoi(auth_tx.TLF01_details.num_sales);
   bch01_cnt   = atoi(sBCH01.total_credit_cnt);
   if ( auth_tx_cnt == bch01_cnt )
   {
      /* Check credit amount */
      auth_tx_amt = atol(auth_tx.TLF01_details.sales_amount);
      bch01_amt   = atol(sBCH01.total_credit_amt);
      if ( auth_tx_amt == bch01_amt )
      {
         /* Check credit refund count */
         auth_tx_cnt = atoi(auth_tx.TLF01_details.num_refunds);
         bch01_cnt   = atoi(sBCH01.total_credit_refund_cnt);
         if ( auth_tx_cnt == bch01_cnt )
         {
            /* Check credit refund amount */
            auth_tx_amt = atol(auth_tx.TLF01_details.refund_amount);
            bch01_amt   = atol(sBCH01.total_credit_refund_amt);
            if ( auth_tx_amt == bch01_amt )
            {
               /* Check debit count */
               auth_tx_cnt = atoi(auth_tx.TLF01_details.num_dbsales);
               bch01_cnt   = atoi(sBCH01.total_debit_cnt);
               if ( auth_tx_cnt == bch01_cnt )
               {
                  /* Check debit amount */
                  auth_tx_amt = atol(auth_tx.TLF01_details.dbsales_amount);
                  bch01_amt   = atol(sBCH01.total_debit_amt);
                  if ( auth_tx_amt == bch01_amt )
                  {
                     /* Check debit refund count */
                     auth_tx_cnt = atoi(auth_tx.TLF01_details.num_dbrefunds);
                     bch01_cnt   = atoi(sBCH01.total_debit_refund_cnt);
                     if ( auth_tx_cnt == bch01_cnt )
                     {
                        /* Check debit amount */
                        auth_tx_amt=atol(auth_tx.TLF01_details.dbrefund_amount);
                        bch01_amt  =atol(sBCH01.total_debit_refund_amt);
                        if ( auth_tx_amt == bch01_amt )
                        {
                           /* Total credit & debit counts & amounts all match.
                            * Consider this a duplicate batch.
                            */
                           ret_val = TRUE_DUPLICATE_BATCH;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_DUPLICATE_BATCH
 *
 *  DESCRIPTION:  This function handles processing when an 0500 transaction
 *                is determined to be a duplicate.  It logs a message to the
 *                system log and approves or declines the settlement.  No
 *                other processing is required.  Response code is set
 *                according to whether or not the batch is a true duplicate
 *                or a false duplicate.
 *
 *  INPUTS:       duplicate - TRUE_DUPLICATE_BATCH or FALSE_DUPLICATE_BATCH
 *                auth_tx   - (Global) 0500 transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void Process_Duplicate_Batch( INT duplicate )
{
   CHAR   buffer[100]="";
   BYTE	  MatchingResponseKey = 0;

	MatchingResponseKey = Match_Request_To_Response_Transaction_Key () ;
	auth_tx.TLF01_details.tx_key = MatchingResponseKey ;

   if ( duplicate == TRUE_DUPLICATE_BATCH )
   {
      strcpy( auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED );
      strcpy( auth_tx.TLF01_details.response_text, RC_DUPLICATE_BATCH_APPROVED);
      sprintf( buffer,
              "Duplicate batch - Approved: TID = %s, Batch = %s",
               auth_tx.TLF01_details.terminal_id,
               auth_tx.TLF01_details.batch_number );
   }
   else
   {
      strcpy( auth_tx.TLF01_details.response_code, RC_DUPLICATE_BATCH );
      strcpy( auth_tx.TLF01_details.response_text, RC_DUPLICATE_BATCH_DECLINED);
      sprintf( buffer,
              "Duplicate batch - Declined: TID = %s, Batch = %s",
               auth_tx.TLF01_details.terminal_id,
               auth_tx.TLF01_details.batch_number );
   }
   auth_tx.TLF01_details.general_status = GS_TXN_DUPLICATE_BATCH;
   

   /* Log to system log */
   dcpiso_log_message( 2, 1, buffer, "Process_Duplicate_Batch",1 );
   Send_Transaction_To_Originator();

   return;
}
