/**************************************************************************************************
*  
* MODULE:      Batch_Upload.c

  Copyright (c) 2004, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard
*  
* DESCRIPTION: Contains batch upload routines
*
* APPLICATION: DCPISO
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*
* $Log:   N:\PVCS\PTE\Equitable\new_dcpiso\batch_upload.c  $
   
      Rev 1.8   Sep 21 2004 16:55:10   dirby
   Modified to allow AUTH_RELOAD's during a batch upload.
   SCR 12506
   
      Rev 1.7   Jul 23 2004 10:17:04   mbysani
   for batch uploads added reload confirm message type
   
      Rev 1.6   Oct 02 2003 09:52:28   lmitchel
   Ver: 4.2.0.6; SCR1220 Implemented New Duplicate Batch Processing Design
   
      Rev 1.5   Jan 17 2000 09:05:22   sdasgupt
   Bug fixes
   
      Rev 1.4   Dec 11 1999 18:05:08   sdasgupt
    
   
      Rev 1.3   Dec 08 1999 18:23:36   sdasgupt
    
   
      Rev 1.2   Dec 02 1999 17:16:46   sdasgupt
   Updates
   
      Rev 1.1   Nov 22 1999 17:51:56   sdasgupt
    
   
      Rev 1.0   Nov 17 1999 13:18:08   sdasgupt
   Initial Release
   
      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "global_definitions.h"  
#include "batch_upload.h"
#include "process_incoming_msg.h"
#include "service_functions.h"
#include "database_request.h"
#include "general_utilities.h"
#include "service_functions.h"
#include "process_incoming_msg.h"

extern AUTH_TX auth_tx ;
extern CHAR devds_que_name[] ;

BYTE Process_Batch_Upload (void)
{
	BYTE	retCode = 0 ;
	BYTE	temp [20] = {0} ;
	BYTE	retrievalRefNum[50] = {0} ;
	BYTE	strError[512] = {0} ;

	strcpy (strError, "Inside Process Batch Upload\n") ;
	PRINT (strError) ;

	/* Get the original transaction type from the 0320 */
	auth_tx.TLF01_details.tx_key = Get_Transaction_Type_For_Original_Transaction () ;

	/* If the original transaction type cannot be determined */
	if (auth_tx.TLF01_details.tx_key == AUTH_INVALID)
	{
		//	??
	}

	auth_tx.TLF01_details.entry_type = FINANCIAL;

	if (IsFieldBlank (auth_tx.TLF01_details.response_code, 
					strlen(auth_tx.TLF01_details.response_code)))
	{
		 strcpy (auth_tx.TLF01_details.response_code, TRANSACTION_APPROVED) ;
	}

	retCode = Insert_BCH10_Details_For_Batch_Upload () ;
	return retCode ;

} /* Process_Batch_Upload */

BYTE Get_Transaction_Type_For_Original_Transaction (void)
{
	CHAR processingCode [10] = {0} ;
	CHAR messageType [10] = {0} ;
   
	memcpy (processingCode, auth_tx.TLF01_details.processing_code, 2);
   
	if (!strcmp(processingCode, "40"))
		return AUTH_BRANCH_PMT ;
   
	/* Copy the original message type from the 0320, if present */
	memcpy (messageType, auth_tx.TLF01_details.orig_message, 4) ;

	
	if (!strcmp (messageType, "0200"))
	{
		if (!strcmp (processingCode, "00"))
			return AUTH_SALE ;
		else if (!strcmp (processingCode, "20"))
			return AUTH_REFUND ;
		else if (!strcmp (processingCode, "02"))
			return AUTH_VOID_SALE ;
		else if (!strcmp (processingCode, "22"))
			return AUTH_VOID_REFUND ;
		else if (!strcmp (processingCode, "01"))
			return AUTH_CASH_ADVANCE ;
        else if (!strcmp (processingCode, "21"))
			return AUTH_RELOAD;
	    else if (!strcmp (processingCode, "11"))
			return AUTH_QUASI_CASH;
		
	}
	else if (!strcmp (messageType, "0202"))
	{
		if (!strcmp (processingCode, "21"))
			return AUTH_RELOAD_CONFIRM ;
	}
	else if (!strcmp(messageType, "0220"))
	{
		if (!strcmp (processingCode, "00"))
			return AUTH_OFFLINE_SALE ;
		else if (!strcmp (processingCode, "20"))
			return AUTH_OFFLINE_REFUND ;
		else if (!strcmp (processingCode, "02"))
			return AUTH_SALE_ADJUSTMENT ;
		else if (!strcmp (processingCode, "22"))
			return AUTH_REFUND_ADJUSTMENT ;
		
	}
		
	/*	Take a best guess shot at what the original transaction was
		based on the processing code along

		If the original transaction is unknown, we return AUTH_INVALID
	*/
	if (!strcmp (processingCode, "00"))
		return AUTH_SALE ;
	else if (!strcmp (processingCode, "20"))
		return AUTH_REFUND ;
	else if (!strcmp (processingCode, "02"))
		return AUTH_VOID_SALE ;
	else if (!strcmp (processingCode, "22"))
		return AUTH_VOID_REFUND ;
	else 
		return AUTH_INVALID ;
	   
} /* Get_Transaction_Type_For_Original_Transaction */


BYTE If_Transaction_Total_Equals_Batch_Total (BCH01 sBCH01)
{
	if	((atol(auth_tx.TLF01_details.num_sales) ==		atol(sBCH01.total_credit_cnt)) &&
		(atol(auth_tx.TLF01_details.sales_amount) ==	atol(sBCH01.total_credit_amt)) &&
		(atol(auth_tx.TLF01_details.num_refunds) ==		atol(sBCH01.total_credit_refund_cnt)) &&
		(atol(auth_tx.TLF01_details.refund_amount) ==	atol(sBCH01.total_credit_refund_amt)) &&
		(atol(auth_tx.TLF01_details.num_dbsales) ==		atol(sBCH01.total_debit_cnt)) &&
		(atol(auth_tx.TLF01_details.dbsales_amount) ==	atol(sBCH01.total_debit_amt)) &&
		(atol(auth_tx.TLF01_details.num_dbrefunds) ==	atol(sBCH01.total_debit_refund_cnt)) &&
		(atol(auth_tx.TLF01_details.dbrefund_amount) == atol(sBCH01.total_debit_refund_amt)))
	{
		return 1 ;
	}
	return 0 ;

}

BYTE Process_When_Transaction_Total_Equals_Batch_Total (BCH01 sBCH01) 
{
	BYTE retCode = 0 ;
	BYTE strError[512] = {0} ;
	/*	If the totals on the transaction match the batch totals, then
		we have to update the batch header (BCH01) with the real batch number,
		update the batch details (BCH10) with the real batch number,
		change batch_status to 'C', add the batch number to the device record etc 
	*/

	sprintf (strError, "Transaction totals match batch totals - response date is %s response time is %s\n",
						auth_tx.TLF01_details.date_yyyymmdd, auth_tx.TLF01_details.time_hhmmss) ;
	PRINT (strError) ;
	/* sBCH01.primary_key.device_id is already populated */
	strcpy (sBCH01.primary_key.batch_nbr,	auth_tx.TLF01_details.batch_number) ;
	strcpy (sBCH01.release_date,			auth_tx.TLF01_details.date_yyyymmdd) ;
	strcpy (sBCH01.release_time,			auth_tx.TLF01_details.time_hhmmss) ;
	strcpy (sBCH01.batch_status,			"C");

	if (strcmp (auth_tx.TLF01_details.organization_id,"GES") == 0)
		strcpy (sBCH01.visa_merchant_id,"GES") ;
	else
		strcpy (sBCH01.visa_merchant_id, auth_tx.TLF01_details.visa_merchant_id) ;
	
	retCode = Update_BCH01_Details (sBCH01, ST1_DB_UPDATE, ST2_DB_UPDATE_BCH01_GOOD_SETTLE) ;
	return retCode ;
}

BYTE Process_When_Transaction_Total_Does_Not_Equal_Batch_Total (BCH01 sBCH01) 
{
	/*	If the totals on the transaction DO NOT the batch totals, then
			we have to update the batch header (BCH01) with zero totals,
			update the batch details (BCH10) with 'ACTIVE' batch, delete all
			BCH10 details etc ...and if the database reply is OK, we send
			a '95' to the device to initiate a batch upload
	*/

	BYTE retCode = 0 ;
	BYTE strError[512] = {0} ;
   
	strcpy (sBCH01.primary_key.device_id,	auth_tx.TLF01_details.terminal_id) ;
	strcpy (sBCH01.merchant_id,			auth_tx.TLF01_details.merchant_id) ;
	strcpy (sBCH01.primary_key.batch_nbr,	"ACTIVE") ;

	strcpy (strError, "Totals Do Not Match - sending ST2_DB_UPDATE_BCH01_DELETE_BCH10 to devds\n") ;
	PRINT (strError) ;
	
	strcpy (sBCH01.total_credit_cnt,				"0") ;
	strcpy (sBCH01.total_credit_amt,				"0") ;
	strcpy (sBCH01.total_credit_refund_cnt,			"0") ;
	strcpy (sBCH01.total_credit_refund_amt,			"0") ;
	strcpy (sBCH01.total_debit_cnt,					"0") ;
	strcpy (sBCH01.total_debit_amt,					"0") ;
	strcpy (sBCH01.total_debit_refund_cnt,			"0") ;
	strcpy (sBCH01.total_debit_refund_amt,			"0") ;

	strcpy (sBCH01.total_void_credit_cnt,			"0") ;
	strcpy (sBCH01.total_void_credit_amt,			"0") ;
	strcpy (sBCH01.total_void_credit_refund_cnt,	"0") ;
	strcpy (sBCH01.total_void_credit_refund_amt,	"0") ;

	retCode = Update_BCH01_Details (sBCH01, ST1_DB_UPDATE, ST2_DB_UPDATE_BCH01_DELETE_BCH10) ;
	return retCode ;
}
