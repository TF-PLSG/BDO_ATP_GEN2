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

extern INT Mracon_Log_Enable_Flag;

extern AUTH_TX auth_tx ;				

extern CHAR		updateds_que_name[];
extern CHAR		devds_que_name[];
extern CHAR		trands_que_name [] ;
extern CHAR		mrads_que_name[];
extern CHAR     faisnd_que_name [];
#ifdef FALCON_CONTROLLER
extern CHAR		falcntl_que_name[];
#endif
extern CHAR     dcpimp_que_name [];
extern CHAR     mracon_que_name[MAX_QUE_NAME_SIZE+1];


BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ; /* defined to be 25000 */
extern NCF01  ncf01_recs[MAX_NCF01_RECS];

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


BYTE Get_MGF01_Details (void) 
{
	MGF01 sMGF01 ;
	BYTE retCode = 0 ;
	BYTE strTrimmedmerchant_group_id1[10] = {0} ;
	BYTE strTrimmedmerchant_group_id2[10] = {0} ;
	INT  len1 =0;
	INT  len2 =0;
	memset (&sMGF01, 0, sizeof sMGF01) ;

	// Merchant group ID is 15 byte value , 9 byte will be store in MCF01_details.network[5].priority_routing and 
	// 6 byte in MCF01_details.network[6].priority_routing
	
	strTrim (auth_tx.MCF01_details.network[5].priority_routing, strTrimmedmerchant_group_id1) ;
	strTrim (auth_tx.MCF01_details.network[6].priority_routing, strTrimmedmerchant_group_id2) ;

	len1= strlen(strTrimmedmerchant_group_id1);
	strncpy (sMGF01.primary_key.merchant_group_id, strTrimmedmerchant_group_id1, len1);

	len2= strlen(strTrimmedmerchant_group_id2);
	if(len2> 0)
	{
		if(len2> 6)
		{
			strncpy (sMGF01.primary_key.merchant_group_id + len1, strTrimmedmerchant_group_id2, 6);
		}
		else
		{
			strncpy (sMGF01.primary_key.merchant_group_id + len1, strTrimmedmerchant_group_id2, len2);
		}
	}

	retCode = Send_Request_To_Service (devds_que_name,
									   MT_DB_REQUEST, 
									   ST1_DB_SELECT, 
									   0, 
									   MGF01_DATA, 
									   (pBYTE) &sMGF01, 
									   sizeof sMGF01) ;
	return retCode ;

} /* Get_MGF01_Details */

BYTE Get_MPPARA01_Details (char * Product_code) 
{
	MPPARA01 sMPPARA01 ;
	BYTE retCode = 0 ;
	BYTE strTrimmedproduct_code[3] ={0} ;
		
	memset (&sMPPARA01, 0x00, sizeof sMPPARA01) ;
	strTrim (auth_tx.TLF01_details.merchant_id, strTrimmedproduct_code) ;
	//strcpy (sMPPARA01.primary_key.product_code, strTrimmedproduct_code);
	strcpy (sMPPARA01.primary_key.product_code, Product_code);

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST, 
									   ST1_DB_SELECT, 
									   0, 
									   MPPARA01_DATA, 
									   (pBYTE) &sMPPARA01, 
									   sizeof sMPPARA01) ;
	return retCode ;

} /* Get_MPPARA01_Details */


BYTE Get_CCF02P_Details (void) 
{
	CCF02P sCCF02P ;
	BYTE retCode = 0 ;
	BYTE strTrimmedcard_type[2] ={0};
	BYTE strTrimmedcard_nbr[20] ={0};
	BYTE strCardNumber[20]="";
	BYTE nAccountType = 0 ; /* determines if the card is a CREDIT, DEBIT, FLEET,  or LOYALTY card */
	
	strcpy (strCardNumber, auth_tx.TLF01_details.card_num) ;

	Get_Bin_Type_From_Processing_Code (&nAccountType) ;
	strTrimmedcard_type[0] = nAccountType ;
	
	memset (&sCCF02P, 0x00 , sizeof sCCF02P) ;
	
	strTrim (auth_tx.TLF01_details.card_num, strTrimmedcard_nbr) ;
	strncpy (sCCF02P.primary_key.card_type, strTrimmedcard_type, strlen(strTrimmedcard_type));
	strncpy (sCCF02P.primary_key.card_nbr, strTrimmedcard_nbr, strlen(strTrimmedcard_nbr));

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST, 
									   ST1_DB_SELECT, 
									   0, 
									   CCF02P_DATA, 
									   (pBYTE) &sCCF02P, 
									   sizeof sCCF02P) ;
	return retCode ;

} /* Get_CCF02P_Details */

BYTE Get_CCF02P_Details_For_RT (void)
{
	CCF02P sCCF02P ;
	BYTE retCode = 0 ;
	BYTE strTrimmedcard_type[2] ={0};
	BYTE strTrimmedcard_nbr[20] ={0};
	BYTE strCardNumber[20]="";
	BYTE nAccountType = 0 ; /* determines if the card is a CREDIT, DEBIT, FLEET,  or LOYALTY card */

	strcpy (strCardNumber, auth_tx.TLF01_details.card_num) ;

	Get_Bin_Type_From_Processing_Code (&nAccountType) ;
	strTrimmedcard_type[0] = nAccountType ;

	memset (&sCCF02P, 0x00 , sizeof sCCF02P) ;

	strTrim (auth_tx.TLF01_details.card_num, strTrimmedcard_nbr) ;
	strncpy (sCCF02P.primary_key.card_type, strTrimmedcard_type, strlen(strTrimmedcard_type));
	strncpy (sCCF02P.primary_key.card_nbr, strTrimmedcard_nbr, strlen(strTrimmedcard_nbr));

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST,
									   ST1_DB_SELECT,
									   0,
									   CCF02P_DATA_RT,
									   (pBYTE) &sCCF02P,
									   sizeof sCCF02P) ;
	return retCode ;

} /* Get_CCF02P_Details_For_RT */

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


BYTE Get_BCH01_MP_Details (BYTE retrieval_type)
{
	/*	The dataserver function  searches the BCH01_MP (terminal_id, batch_nbr = 'ACTIVE')
		If BCH01_MP record does not exist, it is created.
		If a duplicate batch is being verified, BCH01_MP is searched using the batch Number.
	*/
	BCH01_MP sBCH01_MP ;
	BYTE retCode = 0 ;
	memset (&sBCH01_MP, 0, sizeof sBCH01_MP) ;

	strcpy (sBCH01_MP.primary_key.device_id, auth_tx.TLF01_details.terminal_id) ;
	strcpy (sBCH01_MP.merchant_id, auth_tx.TLF01_details.merchant_id) ;

	if (retrieval_type == ST2_DB_SELECT_BCH01_BY_BATCH_NBR) 
		strcpy (sBCH01_MP.primary_key.batch_nbr, auth_tx.TLF01_details.batch_number) ;
	else
		strcpy (sBCH01_MP.primary_key.batch_nbr, "ACTIVE") ;
	
	if (strcmp (auth_tx.TLF01_details.organization_id,"GES") == 0)
		strcpy (sBCH01_MP.visa_merchant_id,"GES") ;
	else
		strcpy(sBCH01_MP.visa_merchant_id,auth_tx.TLF01_details.visa_merchant_id) ;

	retCode = Send_Request_To_Service ( devds_que_name,
										MT_DB_REQUEST, 
										ST1_DB_SELECT, 
										retrieval_type, 
										BCH01_MP_DATA, 
										(pBYTE) &sBCH01_MP, 
										sizeof sBCH01_MP) ;
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
	// Auth number card number
	strcpy (sBCH20.auth_number , auth_tx.TLF01_details.auth_number) ;
	strcpy (sBCH20.card_nbr, auth_tx.TLF01_details.card_num) ;



	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST, 
										ST1_DB_SELECT, 
										retrievalType, 
										BCH20_DATA, 
										(pBYTE) &sBCH20, 
										sizeof sBCH20) ;
	return retCode ;
} /* Get_BCH20_Details */

BYTE Get_BCH10_Details (BYTE retrievalType)
{
	BCH10 sBCH10 ;
	BYTE retCode = 0 ;

	memset (&sBCH10, 0, sizeof sBCH10) ;

	strcpy (sBCH10.primary_key.device_id,			auth_tx.TLF01_details.terminal_id) ;
	strcpy(sBCH10.merchant_id, auth_tx.TLF01_details.merchant_id);
	strcpy (sBCH10.primary_key.batch_nbr,			"ACTIVE") ;
	strcpy (sBCH10.sys_trace_audit_num,				auth_tx.TLF01_details.sys_trace_audit_num) ;
	/* Recall that the RRN carried in the ISO message was placed in the orig_retrieval_ref_num field in auth_tx */
	strcpy (sBCH10.primary_key.retrieval_ref_num,	auth_tx.TLF01_details.orig_retrieval_ref_num) ;


	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST, 
										ST1_DB_SELECT, 
										retrievalType, 
										BCH10_DATA, 
										(pBYTE) &sBCH10, 
										sizeof sBCH10) ;
	return retCode ;
}

BYTE Get_ACF01_Details (void) 
{
	ACF01 sACF01 ;
	BYTE retCode = 0 ;
	INT nIndex =0;
	BYTE strTrimmedcard_type[3] ={0};
	BYTE strTrimmedcard_nbr[20] ={0};
	char strRelatedAccountType[50] = {0} ;
	char strTemp[100]="";
	BOOLEAN bTypeFound = false ;
	BYTE MessageSubType2 = 0;

	memset (&sACF01, 0, sizeof (sACF01)) ;

	if (strEqual (auth_tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
	{
			strncpy (strRelatedAccountType, DOLLAR_ACCOUNT_TYPE, 2) ;
	}
	else if (strEqual (auth_tx.TLF01_details.currency_code, PESO_CURRENCY))
	{
		strncpy (strRelatedAccountType, PESO_ACCOUNT_TYPE, 2) ;
	}

	if (strlen(strRelatedAccountType) == 0)
	{
		/******/
		strcpy (strTemp, "Related Acc cannot be determined from Tx") ;
		dcpimp_log_message( 2, 3, strTemp, " Get_ACF01_Details ",1 );
		if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)!=0)
		{
			retCode= Update_All_Data ();
			return retCode;
		}
		else
		{
			return DEVICE_ERROR;
		}
		/******/
	}

	for (nIndex = 0 ; nIndex < 9 && !bTypeFound; nIndex++)
	{
		if (strEqual(auth_tx.CCF02P_details.related_acct[nIndex].type, strRelatedAccountType))
		{
			bTypeFound = true ;
		}
	}

	if (bTypeFound == TRUE)
	{
		MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_OTHER ;

		strncpy(strTrimmedcard_type, auth_tx.CCF02P_details.related_acct[nIndex-1].type,
				sizeof(strTrimmedcard_type));
		strncpy(strTrimmedcard_nbr,  auth_tx.CCF02P_details.related_acct[nIndex-1].nbr,
				sizeof(strTrimmedcard_nbr));
		
		strncpy(sACF01.primary_key.account_type, strTrimmedcard_type,sizeof(sACF01.primary_key.account_type));
		strncpy(sACF01.primary_key.account_nbr,  strTrimmedcard_nbr,sizeof(sACF01.primary_key.account_nbr));

		
		retCode = Send_Request_To_Service (trands_que_name,
										   MT_DB_REQUEST, 
										   ST1_DB_SELECT, 
										   MessageSubType2, 
										   ACF01_DATA, (pBYTE) &sACF01, 
										   sizeof(sACF01)) ;
		return retCode ;
	}
	else
	{
		sprintf (strTemp, "RelatedAcc cannot is not present for card type %s",strRelatedAccountType ) ;
		dcpimp_log_message( 2, 3, strTemp, " Get_ACF01_Details ",1 );
		if(strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)!=0)
		{
			retCode= Update_All_Data ();
			return retCode;
		}
		else
		{
			strcpy( auth_tx.TLF01_details.response_code, DO_NOT_HONOR );
			return DEVICE_ERROR;
		}
	}

}

BYTE Get_ACF01_Details_For_RT (void)
{
	ACF01 sACF01 ;
	BYTE retCode = 0 ;
	INT nIndex =0;
	BYTE strTrimmedcard_type[3] ={0};
	BYTE strTrimmedcard_nbr[20] ={0};
	char strRelatedAccountType[50] = {0} ;
	char strTemp[100]="";
	CHAR strError[512] = {0} ;
	BOOLEAN bTypeFound = false ;
	BYTE MessageSubType2 = 0;

	memset (&sACF01, 0, sizeof (sACF01)) ;

	if (strEqual (auth_tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
	{
			strncpy (strRelatedAccountType, DOLLAR_ACCOUNT_TYPE, 2) ;
	}
	else if (strEqual (auth_tx.TLF01_details.currency_code, PESO_CURRENCY))
	{
		strncpy (strRelatedAccountType, PESO_ACCOUNT_TYPE, 2) ;
	}

	for (nIndex = 0 ; nIndex < 9 && !bTypeFound; nIndex++)
	{
		if (strEqual(auth_tx.CCF02P_details.related_acct[nIndex].type, strRelatedAccountType))
		{
			bTypeFound = true ;
		}
	}

	if (bTypeFound == TRUE)
	{
		MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_OTHER ;

		strncpy(strTrimmedcard_type, auth_tx.CCF02P_details.related_acct[nIndex-1].type,
				sizeof(strTrimmedcard_type));
		strncpy(strTrimmedcard_nbr,  auth_tx.CCF02P_details.related_acct[nIndex-1].nbr,
				sizeof(strTrimmedcard_nbr));

		strncpy(sACF01.primary_key.account_type, strTrimmedcard_type,sizeof(sACF01.primary_key.account_type));
		strncpy(sACF01.primary_key.account_nbr,  strTrimmedcard_nbr,sizeof(sACF01.primary_key.account_nbr));


		retCode = Send_Request_To_Service (trands_que_name,
										   MT_DB_REQUEST,
										   ST1_DB_SELECT,
										   MessageSubType2,
										   ACF01_DATA_RT, (pBYTE) &sACF01,
										   sizeof(sACF01)) ;
		return retCode ;
	}
	else
	{
		sprintf( strError, "RelatedAcc cannot is not present for FAI FRT transaction");
		dcpimp_log_message( 2, 1, strError, " Get_ACF01_Details_For_RT", 1 );
		retCode = Dcpimp_Send_Request_To_RT_Falcon_Service();
		if(DEVICE_ERROR == retCode)
		{
			sprintf( strError, "Failed to send for FAI FRT transaction to Falcon");
			dcpimp_log_message( 2, 1, strError, " Get_ACF01_Details_For_RT", 1 );
			retCode = Send_Transaction_To_Authorizer () ;
		}
		else
		{
			return retCode ;
		}

	}

}


BYTE Get_CCF03P_Details (void)
{
	CCF03 sCCF03P ;
	BYTE retCode = 0 ;
	BYTE strTrimmedcard_type[2] ={0};
	BYTE strTrimmedcard_nbr[20] ={0};
	BYTE strCardNumber[20]="";
	BYTE nAccountType = 0 ; /* determines if the card is a CREDIT, DEBIT, FLEET,  or LOYALTY card */
	
	strcpy (strCardNumber, auth_tx.TLF01_details.card_num) ;

	Get_Bin_Type_From_Processing_Code (&nAccountType) ;
	strTrimmedcard_type[0] = nAccountType ;


	memset (&sCCF03P, 0, sizeof (sCCF03P)) ;

	strcpy (sCCF03P.primary_key.card_type, strTrimmedcard_type) ;
	strcpy (sCCF03P.primary_key.card_nbr,  auth_tx.TLF01_details.card_num) ;

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST, 
									   ST1_DB_SELECT, 
									   0, 
									   CCF03P_DATA, 
									   (pBYTE) &sCCF03P,
									   sizeof(sCCF03P)) ;
	return retCode ;
}

BYTE Get_PAF01_Details (void)
{
	PAF01 sPAF01 ;
	BYTE retCode = 0 ;

	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	strcpy (sPAF01.primary_key.primary_acct_nbr, auth_tx.CCF02P_details.primary_acct_nbr) ;

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST,
									   ST1_DB_SELECT,
									   0,
									   PAF01_DATA,
									   (pBYTE) &sPAF01,
									   sizeof(sPAF01)) ;
	return retCode ;
}

BYTE Get_PAF01_Details_For_RT (void)
{
	PAF01 sPAF01 ;
	BYTE retCode = 0 ;

	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	strcpy (sPAF01.primary_key.primary_acct_nbr, auth_tx.CCF02P_details.primary_acct_nbr) ;

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST,
									   ST1_DB_SELECT,
									   0,
									   PAF01_DATA_RT,
									   (pBYTE) &sPAF01,
									   sizeof(sPAF01)) ;
	return retCode ;
}

BYTE Get_PAF01_Details_for_CustrNbr_with_Wait (void)
{
	PAF01 sPAF01 ;
	BYTE retCode = 0 ;

	pPTE_MSG	     p_msg_out;
	pPTE_MSG	     p_msg_in;
	pBYTE         p_data;
	pPTE_MSG_DATA p_msg_data;
	BYTE          rcode;
	BYTE          buffer[sizeof(AUTH_TX) + sizeof(PAF01)];
	char 		 error_msg[100];

	memset( buffer,0, sizeof(buffer) );
	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	strcpy (sPAF01.primary_key.primary_acct_nbr, auth_tx.CCF02P_details.primary_acct_nbr) ;

	memcpy( buffer, &auth_tx, sizeof(AUTH_TX) );
	memcpy( buffer+sizeof(AUTH_TX), &sPAF01, sizeof(PAF01));

	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								 ST1_DB_SELECT,
								 0,
								 NULL_PTR,
								 interactive_que_name,
								 (pBYTE) buffer,
								 sizeof( buffer),
								 PAF01_DATA );
	if( NULL_PTR == p_msg_out )
	{
	  strcpy( error_msg, "Insufficient memory to build request message." );
	  retCode = false;
	}
	else
	{
	  rcode = dcpimp_Send_Interactive_DB_Request(p_msg_out,
													  &p_msg_in,
													  trands_que_name,
													  error_msg );
	  if ( rcode == PTEMSG_OK )
	  {
		 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
		 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

		 memcpy( &sPAF01, p_data+sizeof(AUTH_TX), sizeof(PAF01) );
		 strcpy(auth_tx.pan_extended, sPAF01.Custr_nbr);
	  }

	  if ( PTEMSG_IPC_ERROR != rcode )
		 free( p_msg_in );
	  else
		  retCode = false;
	}
	return retCode ;
}

BOOLEAN Get_CR01_Details(void)
{
	CR01 sCR01 ;
	BOOLEAN bReturnCode = true ;

	memset (&sCR01, 0, sizeof (sCR01)) ;
	strcpy (sCR01.rule_bin_low, auth_tx.BIN01_details.primary_key.low) ;
	strcpy (sCR01.rule_bin_high, auth_tx.BIN01_details.high) ;

	bReturnCode = Send_Request_To_Service (trands_que_name,MT_DB_REQUEST,
																	ST1_DB_SELECT,ST2_DB_SELECT_BIN_RANGE_CFB_TXN, CR01_DATA,
																	(pBYTE) &sCR01, sizeof(sCR01)) ;
	return bReturnCode ;

}

BOOLEAN Update_CR01_Details(CR01 sCR01)
{
	//CR01 sCR01 ;
	BOOLEAN bReturnCode = true ;


	bReturnCode = Send_Request_To_Service (trands_que_name,MT_DB_REQUEST,
													ST1_DB_UPDATE,0,
													CR01_DATA,
													(pBYTE) &sCR01,
													sizeof(sCR01)) ;
	return bReturnCode ;

}

/*
BYTE Get_MCF03P_Details (void)
{
	MCF03P sMCF03P ;
	BYTE retCode = 0 ;

	memset (&sMCF03P, 0, sizeof (sMCF03P)) ;

	strncpy (sMCF03P.merch_id, auth_tx.TLF01_details.merchant_id,15) ;
	if(auth_tx.TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx.TLF01_details.system_date );
	}
	strncpy (sMCF03P.system_date,  auth_tx.TLF01_details.system_date,8) ;

	retCode = Send_Request_To_Service (trands_que_name,
									   MT_DB_REQUEST,
									   ST1_DB_SELECT,
									   0,
									   MCF03P_DATA,
									   (pBYTE) &sMCF03P,
									   sizeof(sMCF03P)) ;
	return retCode ;
}*/


BYTE look_for_duplicate_txn( void )
{
   BCH10  sBCH10;
   BYTE   retCode;

   memset( &sBCH10, 0x00, sizeof(sBCH10) );
   strcpy( sBCH10.primary_key.device_id, auth_tx.TLF01_details.terminal_id );
   strcpy( sBCH10.merchant_id, 			 auth_tx.TLF01_details.merchant_id);
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



BYTE Update_BCH01_MP_Details (BCH01_MP sBCH01_MP, BYTE subType1, BYTE subType2) 
{
	BYTE retCode = 0 ;

	retCode = Send_Request_To_Service (	devds_que_name,
										MT_DB_REQUEST,
										subType1,
										subType2,
										BCH01_MP_DATA, 
										(pBYTE) &sBCH01_MP, 
										sizeof sBCH01_MP) ;
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
	
		   retCode = Send_Request_To_Service (	mrads_que_name,
	                                          MT_DB_REQUEST,
	                                          ST1_DB_INSERT,
	                                          subtype2,
	                                          MRA_AUTH_TBL_DATA,
	                                          (pBYTE) &auth_tx.TLF01_details,
	                                          sizeof auth_tx.TLF01_details);

 
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
                                          sizeof (auth_tx.TLF01_details)) ;
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

BOOLEAN dcpimp_Send_Request_To_Mracon_Service ( void )
{
   BOOLEAN  retval = true;
   INT       nBufferSize;
   LONG     lReturnCode  = 0L;
   BYTE     ReturnCode   = 0;
   pPTE_MSG p_msg_out    = 0;
   CHAR     strTemp[256] = {0};

   /*For MP transactions Accquring ID wil be NCIFTR   */
   strcpy( auth_tx.TLF01_details.acquiring_id,"NCIFTR");

   memset( p_buffer, 0x00,     sizeof(p_buffer) );
   memcpy( p_buffer, &auth_tx, sizeof(AUTH_TX ) );
   nBufferSize = sizeof( AUTH_TX );

   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
                                 mracon_que_name,
                                 dcpimp_que_name,
								 (pBYTE) p_buffer,
                                 nBufferSize,
                                 0 );
   if (p_msg_out == NULL)
   {
	   if(Mracon_Log_Enable_Flag == 1)
	  {
		  sprintf( strTemp,"Mem allocate Err build msg for %s", mracon_que_name );
		  dcpimp_log_message( 1, 3,strTemp,"dcpimp_Send_Request_To_Mracon_Service", 1 );
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
			 dcpimp_log_message( 1, 3,strTemp,"dcpimp_Send_Request_To_Mracon_Service", 1 );
		  }
      }
      free( p_msg_out );
   }
   return( retval );
}


/*************************************************************************************
 * NAME:		Send_Request_To_Falcon_Service
 * DESCRIPTION:	Send AUTH_TX structure to Falcon service
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Abhishek
 * MODIFICATIONS:	
 ************************************************************************************/
BOOLEAN Send_Request_To_Falcon_Service ( void )
{
	BOOLEAN  retval = true;
	INT       nBufferSize;
	LONG     lReturnCode  = 0L;
	BYTE     ReturnCode   = 0;
	pPTE_MSG p_msg_out    = 0;
	CHAR     strTemp[256] = {0};

	/* First, need to make sure Acquirer Id (DE32) is populated.
	* It will not be populated it txn failed velocity checks.
	* In that case, need to get it from NCF01.
	*/

	/*For MP transactions Accquring ID wil be NCIFTR   */
	strcpy( auth_tx.TLF01_details.acquiring_id,"NCIFTR");

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &auth_tx, sizeof(AUTH_TX ) );
	nBufferSize = sizeof( AUTH_TX );

	//as of now only Falcon RT transactions are handled in Falcon Controller(falcntl).
/*#ifdef FALCON_CONTROLLER
	p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  falcntl_que_name,
								  dcpimp_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", falcntl_que_name );
		dcpimp_log_message( 1, 3, strTemp, " Send_Request_To_Falcon_Service ", 1 );
	}
	else
	{
		lReturnCode = pteipc_send (p_msg_out, falcntl_que_name) ;
		if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,falcntl_que_name);
			dcpimp_log_message( 1, 3, strTemp, " Send_Request_To_Falcon_Service ", 1 );
		}
		free( p_msg_out );
	}
#else*/
	p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  faisnd_que_name,
								  dcpimp_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", faisnd_que_name );
		dcpimp_log_message( 1, 3, strTemp, " Send_Request_To_Falcon_Service ", 1 );
	}
	else
	{
		lReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
		if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,faisnd_que_name);
			dcpimp_log_message( 1, 3, strTemp, " Send_Request_To_Falcon_Service ", 1 );
		}
		free( p_msg_out );
	}
//#endif

	return( retval );
} /* Send_Request_To_Falcon_Service */


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

#ifdef HCTS3
BYTE Retrieve_BIN01_Record_Online (void)
{
	BYTE  AccountType = 0 ; // determines if the card is a CREDIT, DEBIT, FLEET, or LOYALTY
	BYTE ReturnCode = 0 ;
	CHAR strError[256] = {0};
	INT  AuthIndicator = 0 ;

	Get_Bin_Type_From_Processing_Code (&AccountType) ;
	/* Set the bin_type in auth_tx - used later in grouping txns in the batch */
	auth_tx.bin_type = AccountType ;

	/* Get the BIN record for the card */
	/* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
	ReturnCode = Get_BIN01_Details_Online (SHARED_MEMORY_READ) ;

	if (ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY)
	{
	    // bin range is not found in shared memory
		sprintf( strError,
		         "BIN01 record not found for Msg Type: %s",
		         auth_tx.TLF01_details.message_type );
		dcpimp_log_message( 2, 3, strError, "Retrieve_BIN01_Record_Online", 1 );
		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

		ReturnCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, "Invalid card",
		                                           "Retrieve_BIN01_Record_Online",
												   GS_TXN_DROPPED,
												   INVALID_TRANSACTION) ;
	    return ReturnCode ;
	}
	else if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY)
	{
		ReturnCode = Dcpimp_process_when_bin_range_found_online();
		return ReturnCode;
	}
	else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
	{
		// if problems with reading shared memory, read the
		// bin table from the database
		ReturnCode = Get_BIN01_Details_Online (DATABASE_READ) ;
		return ReturnCode ;
	}
	return DEVICE_OK ;
} /* Retrieve_BIN01_Record_Online */

BYTE Get_BIN01_Details_Online (BYTE read_mode)
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
												ST2_NCIFTR_ONLINE,
												BIN01_DATA,
												(pBYTE) strBuffer,
												sizeof strBuffer) ;
		if (bReturnCode == DEVICE_OK)
		{
			return NO_AUTH_ERROR;
		}

		return bReturnCode ;
	}
	return DEVICE_OK ;
} /* Get_BIN01_Details_Online */

BYTE Dcpimp_process_when_bin_range_found_online()
{
	BYTE ReturnCode = 0 ;
	CHAR strError[256] = {0};
	INT  AuthIndicator = 0 ;

    if(auth_tx.BIN01_details.auth_ind[0] == 0 ||
	   (auth_tx.BIN01_details.auth_ind[0] != '1' &&
		auth_tx.BIN01_details.auth_ind[0] != '0')
	  )
    {
    	sprintf(strError, "Invalid auth_indicator value %s in BIN ",
    			auth_tx.BIN01_details.auth_ind);
    	dcpimp_log_message( 2, 3, strError, "Retrieve_BIN01_Record_Online", 1 );
    	strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

		ReturnCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, strError,
		                                           "Retrieve_BIN01_Record_Online",
												   GS_INVALID_AUTH_INDICATOR,
												   REFER_TO_CARD_ISSUER) ;
	    return ReturnCode ;
    }

    /* Switch out to the network if the auth_ind is 0 */
    AuthIndicator = atoi (auth_tx.BIN01_details.auth_ind) ;
    if (AuthIndicator == REMOTE_PROCESSING_INDICATOR) /* 0 */
    {
    	if(strncmp(auth_tx.BIN01_details.destination_name,     iftr_module_name, 6) == 0 ||
    	   strncmp(auth_tx.BIN01_details.alt_destination_name, iftr_module_name, 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.destination_name,     "ncmcr2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.alt_destination_name, "ncmcr2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.destination_name, 	   "ncvis2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.alt_destination_name, "ncvis2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.destination_name,     "ncjcb2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.alt_destination_name, "ncjcb2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.destination_name, 	   "ncdci2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.alt_destination_name, "ncdci2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.destination_name,     "nposa2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.alt_destination_name, "nposa2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.destination_name, 	   "nccup2", 6) == 0 ||
		   strncmp(auth_tx.BIN01_details.alt_destination_name, "nccup2", 6) == 0)
    	{
    		/* BDO change request to route the ONUS transactions to nciftr
    		 * if configured as any Vertexon network links*/
    		return DEVICE_OK;
    	}
    	else
    	{
    		sprintf( strError,
    		         "BIN01 remote dest_que is not IFTR nor any Vertexon link, dest_que:%s",
					 auth_tx.BIN01_details.destination_name );
    		dcpimp_log_message( 2, 3, strError, "Retrieve_BIN01_Record_Online", 1 );
    		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

    		ReturnCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, "Invalid card",
    		                                           "Retrieve_BIN01_Record_Online",
    												   GS_TXN_DROPPED,
    												   INVALID_TRANSACTION) ;
    	    return ReturnCode ;
    	}
    }
    else if (AuthIndicator == LOCAL_PROCESSING_INDICATOR) /* 1 */
    {
    	if (auth_tx.BIN01_details.caf_profile_id[0] != 0)
    	{
			ReturnCode = Dcpimp_Retrieve_CAF_Profile_Online(auth_tx.BIN01_details.caf_profile_id,
															auth_tx.BIN01_details.primary_key.type) ;
			return ReturnCode ;
    	}
    	else
    	{
	    	sprintf(strError, "caf_profile_id cannot be NULL at bin level",
	    			auth_tx.BIN01_details.auth_ind);
	    	dcpimp_log_message( 2, 3, strError, "Retrieve_BIN01_Record_Online", 1 );
	    	strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

			ReturnCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, strError,
			                                           "Retrieve_BIN01_Record_Online",
													   GS_INVALID_AUTH_INDICATOR,
													   REFER_TO_CARD_ISSUER) ;
			return ReturnCode ;
    	}
    }

    return ReturnCode;
}

BYTE Dcpimp_Retrieve_CAF_Profile_Online(BYTE* caf_profile_id, BYTE* type)
{
	BYTE ReturnCode = 0 ;

	/* Set the first parameter to SHARED_MEMORY_READ */
	ReturnCode = Dcpimp_Get_CAF01_Details_Online ( SHARED_MEMORY_READ,
												   caf_profile_id,
												   type) ;

	if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY)
	{
		ReturnCode = Dcpimp_process_when_caf_found_online();
	}
	else
	{
		ReturnCode = Dcpimp_Get_CAF01_Details_Online ( DATABASE_READ,
													   caf_profile_id,
													   type) ;
	}
	return ReturnCode ;

}

BYTE Dcpimp_Get_CAF01_Details_Online (BYTE read_mode, BYTE* caf_profile_id, BYTE* type)
{
	CAF01 sCAF01 ;
	BOOLEAN bReturnCode = true ;
	LONG lReturnCode = 0L ;

	memset (&sCAF01, 0, sizeof sCAF01) ;

	if (read_mode == SHARED_MEMORY_READ)
	{
		strcpy (sCAF01.primary_key.caf_profile_id, caf_profile_id) ;
		strcpy (sCAF01.primary_key.type, type) ;
		lReturnCode = FindMemRecord ((pBYTE)&sCAF01, sizeof (sCAF01.primary_key),
			                         sizeof (sCAF01), "Caf01Table") ;

		if (lReturnCode >= 0)
		{
			/* Store the CAF01 record in auth_tx from BIN01, */
			memcpy (&auth_tx.CAF01_details, &sCAF01, sizeof(sCAF01)) ;
			return RECORD_FOUND_IN_SHARED_MEMORY ;
		}
		else if (lReturnCode == -1L)
		{
			return RECORD_NOT_FOUND_IN_SHARED_MEMORY ;
		}
		else
		{
			return SYSTEM_PROBLEM_READING_SHARED_MEMORY ;
		}
	}
	else if (read_mode == DATABASE_READ)
	{
		memset (&sCAF01, 0, sizeof (sCAF01)) ;
		strcpy (sCAF01.primary_key.caf_profile_id, caf_profile_id) ;
		strcpy (sCAF01.primary_key.type, type) ;

		bReturnCode = Send_Request_To_Service (	trands_que_name,
												MT_DB_REQUEST,
												ST1_DB_SELECT,
												ST2_NCIFTR_ONLINE,
												CAF01_DATA,
												(pBYTE) &sCAF01,
												sizeof(sCAF01)) ;

		if (bReturnCode == DEVICE_OK)
		{
			return NO_AUTH_ERROR;
		}

		return bReturnCode  ;
	}

	return DEVICE_OK ;
}

BYTE Dcpimp_process_when_caf_found_online()
{
	BYTE ReturnCode = 0 ;
	CHAR strError[256] = {0};

	if(strncmp(auth_tx.CAF01_details.destination_name, iftr_module_name, 6) == 0 ||
	   strncmp(auth_tx.CAF01_details.destination_name, "ncmcr2", 6) == 0 ||
	   strncmp(auth_tx.CAF01_details.destination_name, "ncvis2", 6) == 0 ||
	   strncmp(auth_tx.CAF01_details.destination_name, "ncjcb2", 6) == 0 ||
	   strncmp(auth_tx.CAF01_details.destination_name, "ncdci2", 6) == 0 ||
	   strncmp(auth_tx.CAF01_details.destination_name, "nposa2", 6) == 0 ||
	   strncmp(auth_tx.CAF01_details.destination_name, "nccup2", 6) == 0 )
	{
		/* BDO change request to route the ONUS transactions to nciftr
		 * if configured as any Vertexon network links*/
	}
	else
	{
		//is it req to check Auth Location - Onus or Offus??
		sprintf( strError,
		         "Auth profile dest_que is not IFTR nor any Vertexon link, dest_que:%s",
				 auth_tx.CAF01_details.destination_name );
		dcpimp_log_message( 2, 3, strError, "Dcpimp_process_when_caf_found_online", 1 );
		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);

		ReturnCode = Generate_Usage_Error_Message( SET_SYSTEM_MONITOR, "Invalid card",
		                                           "Retrieve_BIN01_Record_Online",
												   GS_TXN_DROPPED,
												   INVALID_TRANSACTION) ;
	   return ReturnCode ;
	}

	return DEVICE_OK;
}
#endif

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
INT Process_Settlement(BCH01_MP sBCH01_MP)
{
   INT   retCode = true;

    printf("In Process_Settlement\n");
   	if (If_Transaction_Total_Equals_Batch_Total (sBCH01_MP))
	{
		/*	If the totals on the transaction match the batch totals, then
			we have to update the batch header (BCH01_MP) with the real batch number,
			update the batch details (BCH10) with the real batch number,
			change batch_status to 'C', add the batch number to the device record etc 
		*/
		retCode = Process_When_Transaction_Total_Equals_Batch_Total (sBCH01_MP) ;
		return retCode ;
	}
	else
	{
		/*	If the totals on the transaction DO NOT the batch totals, then
			we have to update the batch header (BCH01_MP) with zero totals,
			update the batch details (BCH10) with 'ACTIVE' batch, delete all
			BCH10 details etc ...and if the database reply is OK, we send
			a '95' to the device to initiate a batch upload
		*/
		retCode = Process_When_Transaction_Total_Does_Not_Equal_Batch_Total (sBCH01_MP) ;
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
INT is_batch_duplicate(BCH01_MP sBCH01_MP)
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
   bch01_cnt   = atoi(sBCH01_MP.total_credit_cnt);
   if ( auth_tx_cnt == bch01_cnt )
   {
      /* Check credit amount */
      auth_tx_amt = atol(auth_tx.TLF01_details.sales_amount);
      bch01_amt   = atol(sBCH01_MP.total_credit_amt);
      if ( auth_tx_amt == bch01_amt )
      {
         /* Check credit refund count */
         auth_tx_cnt = atoi(auth_tx.TLF01_details.num_refunds);
         bch01_cnt   = atoi(sBCH01_MP.total_credit_refund_cnt);
         if ( auth_tx_cnt == bch01_cnt )
         {
            /* Check credit refund amount */
            auth_tx_amt = atol(auth_tx.TLF01_details.refund_amount);
            bch01_amt   = atol(sBCH01_MP.total_credit_refund_amt);
            if ( auth_tx_amt == bch01_amt )
            {
               /* Check debit count */
               auth_tx_cnt = atoi(auth_tx.TLF01_details.num_dbsales);
               bch01_cnt   = atoi(sBCH01_MP.total_debit_cnt);
               if ( auth_tx_cnt == bch01_cnt )
               {
                  /* Check debit amount */
                  auth_tx_amt = atol(auth_tx.TLF01_details.dbsales_amount);
                  bch01_amt   = atol(sBCH01_MP.total_debit_amt);
                  if ( auth_tx_amt == bch01_amt )
                  {
                     /* Check debit refund count */
                     auth_tx_cnt = atoi(auth_tx.TLF01_details.num_dbrefunds);
                     bch01_cnt   = atoi(sBCH01_MP.total_debit_refund_cnt);
                     if ( auth_tx_cnt == bch01_cnt )
                     {
                        /* Check debit amount */
                        auth_tx_amt=atol(auth_tx.TLF01_details.dbrefund_amount);
                        bch01_amt  =atol(sBCH01_MP.total_debit_refund_amt);
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

    printf("In Process_Duplicate_Batch Function\n ");
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
   dcpimp_log_message( 2, 3, buffer, " Process_Duplicate_Batch ",1 );

   Send_Transaction_To_Originator();

   return;
}

/*************************************************************************************
 * NAME:		Send_EXT10_Request_To_FalconRT_Service
 * DESCRIPTION:	Send EXT10 structure to FAIsnd, a FalconRT service
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Abhishek
 * MODIFICATIONS:
 ************************************************************************************/
BOOLEAN Send_EXT10_Request_To_FalconRT_Service ( void )
{
	BOOLEAN  retval = true;
	INT       nBufferSize;
	LONG     lReturnCode  = 0L;
	BYTE     ReturnCode   = 0;
	pPTE_MSG p_msg_out    = 0;
	CHAR     strTemp[256] = {0};

	/* First, need to make sure Acquirer Id (DE32) is populated.
	* It will not be populated it txn failed velocity checks.
	* In that case, need to get it from NCF01.
	*/

	/*For MP transactions Accquring ID wil be NCIFTR   */
	strcpy( auth_tx.TLF01_details.acquiring_id,"NCIFTR");

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &auth_tx, sizeof(AUTH_TX ) );
	nBufferSize = sizeof( AUTH_TX );

	p_msg_out = ptemsg_build_msg( MT_AUTH_EXT10_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  faisnd_que_name,
								  dcpimp_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", faisnd_que_name );
		dcpimp_log_message( 1, 3, strTemp, " Send_EXT10_Request_To_FalconRT_Service ", 1 );
	}
	else
	{
		lReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
		if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,faisnd_que_name);
			dcpimp_log_message( 1, 3, strTemp, " Send_EXT10_Request_To_FalconRT_Service ", 1 );
		}
		free( p_msg_out );
	}


	return( retval );
} /* Send_EXT10_Request_To_FalconRT_Service */
