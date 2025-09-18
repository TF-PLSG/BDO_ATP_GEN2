/************************************************************************
*  
* MODULE:      TxDataServer.c
*  
* TITLE:       Equitable DataServer Functions 
*  
* DESCRIPTION: This routines handles all dataserver requests and responses
*
* APPLICATION: TXCNTL
*
****************************************************************************/
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "pteipc.h"
#include "ptesystm.h"
#include "ntutils.h"
#include "txcntl.h"
#include "txservice.h"
#include "txdataserver.h"
#include "txcntl_constants.h"
#include "genutil.h"
#include "txfleet.h"
#include "txauthorize.h"
#include "txutils.h"
#include "memmnger.h"
#include "myutils.h"
#include "diners.h"
#define  OMC01_MASK			 0x10/*praneeth Code*/

#define VISA7 1

extern INT No_of_MCC_codes;
extern MCC_CHECK mcc_check_list[MCC_MAX_CODES];

extern AUTH_TX Auth_Tx ;
extern MCC_LIMITS  Mcc_Limits;
extern PRIVATE_CCF03P_STRUCT private_ccf03p_struct ;
extern COUNTRY_AMT_LIMITS Country_Amt_Limits ;
#ifdef VISA7
extern MERCH_INT_PARAMS MERCH_INT_PARAM;
#endif
extern CHAR tran_dataserver_que_name [] ;
extern CHAR cirrus_host_que_name[] ;
extern CHAR plus_host_que_name[] ;
extern CHAR device_dataserver_que_name[] ;
extern CHAR network_dataserver_que_name[] ;
extern CHAR fconds_que_name			[MAX_QUE_NAME_SIZE+1];
extern char cavv_verification_rc[3];
extern char Authentication_attempt_result_code[3];
extern char cav2_validation_flag;
extern char authentication_attempt_verify_flag;

extern char pos_rc[3];
extern char pos_res_txt[40];

extern CRF01 crf01;
extern CR01 sCR01;
extern NCF01 ncf01_recs[];

extern CHAR ServiceName[];   /* defined in startup.c */
extern CHAR cadencie_host_que_name[] ; // nirbhay ThoughtFocus
extern INT isalphanum(pCHAR string);//TF PHANI
extern CHAR EB_Host_Issuer_ID[];   /* GirijaY, TF */

extern OBS01_GUI_LIST obs01_list;
extern VISA3DS2_GUI_LIST visa3ds2_list;
extern JCB3DS2_GUI_LIST jcb3ds2_list;
extern AMEX3DS2 amex_3ds2;
extern DCI3DS2 diners_3ds2;
extern CUP3DS2 cup_3ds2;
extern JCB3DS2 jcb_3ds2;
extern KEY3DS01 key3ds01;
/* Indicator for Authorization Number for ON-US txns.
 * When it changes, UAG01 in the database is updated.
 */
extern char partial_match_rc[3];
extern char no_match_rc[3];
extern char name_validation_flag;
extern INT Falcon_RT_Flag;
extern INT Falcon_RT_Ext10_Flag;

#ifdef HCTS2
extern BYTE card_activation_flag;
extern BYTE card_updating_status[5];
extern BYTE card_current_status[51][5];
extern INT  total_card_current_status;
extern BYTE visa_vertexon_codes [101][4];
extern BYTE mc_vertexon_codes   [101][4];
extern BYTE jcb_vertexon_codes  [101][4];
extern BYTE amex_vertexon_codes [101][4];
extern BYTE dci_vertexon_codes  [101][4];
extern BYTE upi_vertexon_codes  [101][4];
extern BYTE mp_in_vertexon_codes[101][4];
extern BYTE pl_ft_vertexon_codes[101][4];
#endif

#define NOT_BLOCKED 0
#define BLOCKED     1
#define EMPTY       2

extern INT  AuthNumberChanged;
extern INT nFalconSendFlag;
extern CHAR key[];

extern OBS01 obs01_data;
extern char mc_3ds2_delcine_rc[3];
extern char mc_default_rc_for_digital_payment_data[3];

FRULE01_GUI_LIST amex_frual01;
FRULE01_GUI_LIST dci_frual01;
FRULE01_GUI_LIST cup_frual01;
FRULE01_GUI_LIST jcb_frual01;
FRULE01_GUI_LIST mc_frual01;
FRULE01_GUI_LIST visa_frual01;
FRULE01_GUI_LIST pl_frual01;

FCLF01 amex_fclf01;
FCLF01 dci_fclf01;
FCLF01 cup_fclf01;
FCLF01 jcb_fclf01;
FCLF01 mc_fclf01;
FCLF01 visa_fclf01;
FCLF01 iftr_fclf01;

BYTE TxDataServer_Get_OtherAccount ()
{
	BOOLEAN bTypeFound = false;
	char strRelatedAccountType[50] = "";
	BYTE strTemp[1000] = {0};
	int nIndex = 0 ;
	BOOLEAN bReturnCode = true ;

	if(strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)) 
	{
		strcpy(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY);
		strcpy(Auth_Tx.TLF01_details.host_start_time, PESO_COMP);
		strcpy(Auth_Tx.TLF01_details.host_finish_time, USD_COMP);
		strcpy (strRelatedAccountType, PESO_ACCOUNT_TYPE) ;
	}
	else if(strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
	{
		strcpy(Auth_Tx.TLF01_details.currency_code,USDOLLAR_CURRENCY);
		strcpy(Auth_Tx.TLF01_details.host_start_time, USD_COMP);
		strcpy(Auth_Tx.TLF01_details.host_finish_time, PESO_COMP);
		strcpy (strRelatedAccountType, DOLLAR_ACCOUNT_TYPE) ;
	}
	if (strlen(strRelatedAccountType) == 0)
	{
		/******/
		strcpy (strTemp, "RelatedAcc cannot be determined from Tx") ;
		return txservice_Generate_Usage_Error_Message (2,strTemp,
			"TxDataServer_Get_OtherAccount",
			GS_TXN_ACF01_NOT_FOUND,
			INVALID_ACCOUNT_NUMBER) ;
		/******/
	}
	/* Search the related_acct array for the card holder to check if the 
	the specific related account is present. if not, then it is an error */
	
	for (nIndex = 0 ; nIndex < 9 && !bTypeFound; nIndex++)
	{
		if (strEqual(Auth_Tx.CCF02P_details.related_acct[nIndex].type, strRelatedAccountType))
			bTypeFound = true ;
	}
	if(bTypeFound)
	{
		strcpy(Auth_Tx.CCF02P_details.related_acct[9].type, Auth_Tx.CCF02P_details.related_acct[nIndex-1].type);
		strcpy(Auth_Tx.CCF02P_details.related_acct[9].nbr,  Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr);
		
		bReturnCode = TxDataServer_Get_ACF01_Details (Auth_Tx.CCF02P_details.related_acct[9].type,
			Auth_Tx.CCF02P_details.related_acct[9].nbr,
			BALANCE_ACCOUNT_TYPE_IS_OTHER) ;
		if (bReturnCode == true)
		{
			return NO_AUTH_ERROR;
		}
		else 
		{
			sprintf(strTemp,"unable to get acf01 record nbr=%stype=%s",Auth_Tx.CCF02P_details.related_acct[9].nbr,Auth_Tx.CCF02P_details.related_acct[9].type);
			return txservice_Generate_Usage_Error_Message (2,strTemp,
				"TxDataServer_Get_OtherAccount",
				GS_TXN_ACF01_NOT_FOUND,
				INVALID_ACCOUNT_NUMBER) ;
		}
		
	}
	else
	{
		return txservice_Generate_Usage_Error_Message (2,"Account not found in Related Accounts",
			"TxDataServer_Get_OtherAccount",
			GS_TXN_ACF01_NOT_FOUND,
			INVALID_ACCOUNT_NUMBER) ;
	}
						
}
/*************************************************************************************
NAME:			TxDataServer_Get_BIN01_Details
DESCRIPTION:	Sends request to retrieve the record from the BIN01 table	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BYTE TxDataServer_Get_BIN01_Details (BYTE read_mode)
{
	LONG	nReturnCode = 0L ;
	BOOLEAN bReturnCode = true ;
	BIN01	sBinDetails ;
	char strBuffer[30] = {0} ;
	
	BYTE  strCardNumber[21] = {0} ;
	BYTE	strBinType[2] = {0} ;
	
	memset (&sBinDetails, 0, sizeof (sBinDetails)) ;
	strcpy (strCardNumber, Auth_Tx.TLF01_details.card_num) ;

	//strBinType[0] = Auth_Tx.bin_type;
	/*TF Phani : As per BIN ROUTING logic. ATP should consider only CREDIT TYPE*/
	strBinType[0] = 'C';
	if (read_mode == SHARED_MEMORY_READ)
	{
		nReturnCode = FindMemBinnRange (strCardNumber, strBinType,(BYTE*) &sBinDetails) ;
		if (nReturnCode >= 0L)
		{
			/* have found the bin range in shared memory */
			memcpy (&Auth_Tx.BIN01_details, &sBinDetails, sizeof (sBinDetails)) ;
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
	
		bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
														MT_DB_REQUEST, 
			                                 ST1_DB_CHECK_BIN_RANGE, 
														0, BIN01_DATA, 
														(pBYTE) strBuffer, 
														sizeof strBuffer) ;
		if (false == bReturnCode) 
			return SYSTEM_ERROR ;
		
		return NO_AUTH_ERROR ;
	}
	return NO_AUTH_ERROR ;
} /* TxDataServer_Get_BIN01_Details */

BYTE TxDataServer_Get_BIN02_Details ()
{
	BYTE  strCardNumber[21] = {0} ;
	BOOLEAN bReturnCode = 0 ;
			
	strncpy (strCardNumber, Auth_Tx.TLF01_details.card_num, 
		      sizeof Auth_Tx.TLF01_details.card_num) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
													MT_DB_REQUEST, 
			                                        ST1_DB_CHECK_BIN02_RANGE, 0, 
													BIN02_DATA, 
													(pBYTE) strCardNumber, 
													sizeof strCardNumber) ;
	if (false == bReturnCode) 
		return SYSTEM_ERROR ;
	
	return NO_AUTH_ERROR ;
		
} /* TxDataServer_Get_BIN02_Details */

BYTE TxDataServer_Get_MID_Blockling_Lisgt_Assoisated_with_Bin( void)
{
	BYTE  strCardNumber[21] = {0} ;
	BOOLEAN bReturnCode = 0 ;

	// Lets send Auth Tx. Its having Bin details
	bReturnCode = txservice_Send_Request_To_Service ( tran_dataserver_que_name,
													  MT_DB_REQUEST,
													  ST1_DB_SELECT, 0,
													  MIDBINBLK01_DATA,
													  (pBYTE) &Auth_Tx,
													  sizeof Auth_Tx) ;
	if (false == bReturnCode)
		return SYSTEM_ERROR ;

	return NO_AUTH_ERROR ;

} /* TxDataServer_Get_BIN02_Details */

#ifdef MC07_OCT2023

BOOLEAN TxDataServer_Get_CCF01_Details (void)
{
	BOOLEAN bReturnCode = true ;

	CCF01 sCCF01 = {0} ;

	strcpy (sCCF01.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
	strcpy (sCCF01.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;


	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name, MT_DB_REQUEST,
																	ST1_DB_SELECT,
																	0, CCF01_DATA, (pBYTE) &sCCF01,
																	sizeof(sCCF01)) ;
	return bReturnCode ;
} /* TxDataServer_Get_CCF04_Details */

#endif
/*************************************************************************************
NAME:			TxDataServer_Get_CAF01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BYTE TxDataServer_Get_CAF01_Details (BYTE read_mode, 
												 BYTE* caf_profile_id, 
												 BYTE* type, 
												 BYTE MessageSubType2)
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
			/* Store the CAF01 record in Auth_Tx from BIN01, */
			memcpy (&Auth_Tx.CAF01_details, &sCAF01, sizeof(sCAF01)) ;
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

		bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																		MT_DB_REQUEST, ST1_DB_SELECT, 
																		MessageSubType2, 
																		CAF01_DATA, (pBYTE) &sCAF01, 
																		sizeof(sCAF01)) ;
		if (false == bReturnCode)
			return SYSTEM_ERROR ;
		
		return NO_AUTH_ERROR  ;
	}
	return NO_AUTH_ERROR ;	
	
} /* TxDataServer_Get_CAF01_Details */


/*************************************************************************************
NAME:			TxDataServer_Get_MCF01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_MCF01_Details (BYTE* organization_id, BYTE* merchant_id)
{
	
	MCF01 sMCF01 ;
	BOOLEAN bReturnCode = true ;

	memset (&sMCF01, 0, sizeof (sMCF01)) ;
	strcpy (sMCF01.primary_key.organization_id, organization_id) ;
	strcpy (sMCF01.primary_key.merchant_id, merchant_id) ;
	
	bReturnCode = txservice_Send_Request_To_Service (device_dataserver_que_name,
																	MT_DB_REQUEST, 
																	ST1_DB_SELECT, 0, MCF01_DATA, 
																	(pBYTE) &sMCF01, 
																	sizeof(sMCF01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_MCF01_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_MCT01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_MCT01_Details () 
{
	MCT01 sMCT01 ;
	BOOLEAN bReturnCode = true ;
	char strTrimmedMerchantId[20] = {0} ;

	if(true == txservice_Check_If_CUP_Transaction())
	{
		if(strlen(Auth_Tx.TLF01_details.visa_merchant_id)> 0)
		{
			strTrim (Auth_Tx.TLF01_details.visa_merchant_id, strTrimmedMerchantId) ;
		}
		else
		{
			strTrim (Auth_Tx.TLF01_details.merchant_id, strTrimmedMerchantId) ;
		}
	}
	else
	{
		strTrim (Auth_Tx.TLF01_details.merchant_id, strTrimmedMerchantId) ;
	}
	memset (&sMCT01, 0, sizeof (sMCT01)) ;
	strcpy (sMCT01.primary_key.card_type, Auth_Tx.BIN01_details.card_type) ;
	strcpy (sMCT01.primary_key.merchant_id, strTrimmedMerchantId) ;
	strcpy (sMCT01.primary_key.organization_id, Auth_Tx.TLF01_details.organization_id) ;
	
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																	MT_DB_REQUEST, 
																	ST1_DB_SELECT, 0, MCT01_DATA, 
																	(pBYTE) &sMCT01, sizeof(sMCT01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_MCT01_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_CCF02_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/				
BYTE TxDataServer_Get_CCF02_Details (void)
{
	CCF02P sCCF02P ;
	BOOLEAN bReturnCode = true ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;

    memset (&sCCF02P, 0, sizeof (sCCF02P)) ;

	strcpy (sCCF02P.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
	strcpy (sCCF02P.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;

	
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																		MT_DB_REQUEST, 
																		ST1_DB_SELECT, 0, CCF02P_DATA, 
																		(pBYTE) &sCCF02P, sizeof(sCCF02P)) ;
    if (bReturnCode == false)
		return SYSTEM_ERROR ;
	return NO_AUTH_ERROR ;
} /* TxCntl_Get_CCF02_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_CCF02_Details_For_RT
DESCRIPTION:	Contains logic for authorizing various transactions
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Abhishek Verma
MODIFIED BY:
*************************************************************************************/
BYTE TxDataServer_Get_CCF02_Details_For_RT (void)
{
	CCF02P sCCF02P ;
	BOOLEAN bReturnCode = true ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;

    memset (&sCCF02P, 0, sizeof (sCCF02P)) ;

	strcpy (sCCF02P.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
	strcpy (sCCF02P.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;


	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																		MT_DB_REQUEST,
																		ST1_DB_SELECT, 0, CCF02P_DATA_RT,
																		(pBYTE) &sCCF02P, sizeof(sCCF02P)) ;
    if (bReturnCode == false)
		return SYSTEM_ERROR ;
	return NO_AUTH_ERROR ;
} /* TxDataServer_Get_CCF02_Details_For_RT */


/*************************************************************************************
NAME:			TxDataServer_Get_PAF01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_PAF01_Details(void)
{
	PAF01 sPAF01 ;
	BOOLEAN bReturnCode = true ;

	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	/* using TLF01.card_num to get customer number FLCN -1 */
	if (Auth_Tx.CCF02P_details.primary_acct_nbr[0] != 0)
	{
		strcpy (sPAF01.primary_key.primary_acct_nbr, Auth_Tx.CCF02P_details.primary_acct_nbr) ;
	}
	else
	{
		strcpy (sPAF01.primary_key.primary_acct_nbr, Auth_Tx.TLF01_details.card_num) ;
	}

		
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST, 
																	ST1_DB_SELECT, 0, PAF01_DATA, 
																	(pBYTE) &sPAF01, sizeof(sPAF01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_PAF01_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_PAF01_Details_For_RT
DESCRIPTION:	Retreive the Primary Account Details for Falcon Real-Time transaction.
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Abhishek Verma
MODIFIED BY:	Divya Teja
*************************************************************************************/
BOOLEAN TxDataServer_Get_PAF01_Details_For_RT(void)
{
	PAF01 sPAF01 ;
	BOOLEAN bReturnCode = true ;

	memset (&sPAF01, 0, sizeof (sPAF01)) ;
	strcpy (sPAF01.primary_key.primary_acct_nbr, Auth_Tx.CCF02P_details.primary_acct_nbr) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST,
																	ST1_DB_SELECT, 0, PAF01_DATA_RT,
																	(pBYTE) &sPAF01, sizeof(sPAF01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_PAF01_Details_For_RT */
/*************************************************************************************
NAME:			TxDataServer_Get_ACF01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_ACF01_Details (BYTE* account_type, BYTE* account_nbr, 
													 BYTE MessageSubType2) 
{
	ACF01 sACF01 ;
	BOOLEAN bReturnCode = true ;

	memset (&sACF01, 0, sizeof (sACF01)) ;
	strcpy (sACF01.primary_key.account_type, account_type) ;
	strcpy (sACF01.primary_key.account_nbr, account_nbr) ;

	
	
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST, 
																	ST1_DB_SELECT, MessageSubType2, 
																	ACF01_DATA, (pBYTE) &sACF01, 
																	sizeof(sACF01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_ACF01_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_ACF01_Details_For_RT
DESCRIPTION:	Contains logic for authorizing various transactions
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Abhishek Verma
MODIFIED BY:
*************************************************************************************/
BOOLEAN TxDataServer_Get_ACF01_Details_For_RT (void)
{
	ACF01 sACF01 ;
	BOOLEAN bReturnCode = true ;
	INT nIndex =0;
	BYTE strTrimmedcard_type[3] ={0};
	BYTE strTrimmedcard_nbr[20] ={0};
	char strRelatedAccountType[50] = {0} ;
	char strTemp[100]={0};
	BOOLEAN bTypeFound = false ;
	BYTE MessageSubType2 = 0;

	memset (&sACF01, 0, sizeof (sACF01)) ;

	if (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
	{
			strncpy (strRelatedAccountType, DOLLAR_ACCOUNT_TYPE, 2) ;
	}
	else if (strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
	{
		strncpy (strRelatedAccountType, PESO_ACCOUNT_TYPE, 2) ;
	}

	if (strlen(strRelatedAccountType) == 0)
	{
		strcpy (strTemp, "Related Acc cannot be determined from FTR Tx") ;
		txcntl_log_message( 2, 2, strTemp, " TxDataServer_Get_ACF01_Details_For_RT ",1 );
		return false;
	}

	for (nIndex = 0 ; nIndex < 9 && !bTypeFound; nIndex++)
	{
		if (strEqual(Auth_Tx.CCF02P_details.related_acct[nIndex].type, strRelatedAccountType))
		{
			bTypeFound = true ;
		}
	}

	if (bTypeFound == true)
	{
		MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_OTHER ;

		strncpy(strTrimmedcard_type, Auth_Tx.CCF02P_details.related_acct[nIndex-1].type,
				sizeof(strTrimmedcard_type));
		strncpy(strTrimmedcard_nbr,  Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr,
				sizeof(strTrimmedcard_nbr));

		strncpy(sACF01.primary_key.account_type, strTrimmedcard_type,sizeof(sACF01.primary_key.account_type));
		strncpy(sACF01.primary_key.account_nbr,  strTrimmedcard_nbr,sizeof(sACF01.primary_key.account_nbr));


		bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST,
																	ST1_DB_SELECT, MessageSubType2,
																	ACF01_DATA_RT, (pBYTE) &sACF01,
																	sizeof(sACF01)) ;
		return bReturnCode;
	}

} /* TxDataServer_Get_ACF01_Details_For_RT */

/*************************************************************************************
NAME:			TxDataServer_Get_CCF03_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_CCF03_Details (void)
{
	CCF03 sCCF03P ;
	
	BOOLEAN bReturnCode = true ;

	memset (&sCCF03P, 0, sizeof (sCCF03P)) ;

	strcpy (sCCF03P.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
	strcpy (sCCF03P.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																	MT_DB_REQUEST, ST1_DB_SELECT, 
																	0, CCF03P_DATA, (pBYTE) &sCCF03P, 
																	sizeof(sCCF03P)) ;
	return bReturnCode ;

} /* TxDataServer_Get_CCF03_Details */
/*************************************************************************************
NAME:			TxDataServer_Get_OMC01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			messagetype2 ONUS or OFFUS
OUTPUTS:		None
RETURNS:       
               
AUTHOR:        TF
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_OMC01_Details (BYTE subtype2)
{
	OMC01 sOmc01 ;
	
	BOOLEAN bReturnCode = true ;

	memset (&sOmc01, 0, sizeof (sOmc01)) ;

	strcpy (sOmc01.primary_key.merchant_id, Auth_Tx.TLF01_details.merchant_id) ;
	strcpy (sOmc01.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																	MT_DB_REQUEST, ST1_DB_SELECT, 
																	subtype2, OMC01_DATA, (pBYTE) &sOmc01, 
																	sizeof(sOmc01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_OMC01_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_CCF03_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_Offus_Velocity_Details (void)
{
	Offus_velocity sOffus_velocity ;
	
	BOOLEAN bReturnCode = true ;

	memset (&sOffus_velocity, 0, sizeof (sOffus_velocity)) ;

	strcpy (sOffus_velocity.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
	strcpy (sOffus_velocity.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																	MT_DB_REQUEST, ST1_DB_SELECT, 
																	0, OFFUS_VELOCITY_DATA, (pBYTE) &sOffus_velocity, 
																	sizeof(sOffus_velocity)) ;
	return bReturnCode ;

} /* TxDataServer_Get_CCF03_Details */

/******************************************************************************
   NAME:        TxDataServer_Get_MCC_Limit_Details
   DESCRIPTION: Contains logic for authorizing various transactions	
   INPUTS:      subtype2 - OFFUS or ONUS
   OUTPUTS:     None
   RETURNS:     false  if request was not sent to the data server
                true   if transaction was sent to the data server
   AUTHOR:      Dennis Irby
*******************************************************************************/
BOOLEAN TxDataServer_Get_MCC_Limit_Details( BYTE subtype2 )
{
   BOOLEAN bReturnCode;

   memset( &Mcc_Limits, 0x00, sizeof(MCC_LIMITS) );

   strcpy( Mcc_Limits.primary_key.mcc,  Auth_Tx.TLF01_details.category_code );
   strcpy( Mcc_Limits.primary_key.type, Auth_Tx.CAF01_details.primary_key.type);
   strcpy( Mcc_Limits.primary_key.caf_profile_id,
           Auth_Tx.CAF01_details.primary_key.caf_profile_id );

   bReturnCode = txservice_Send_Request_To_Service( tran_dataserver_que_name,
                                                    MT_DB_REQUEST,
                                                    ST1_DB_SELECT, 
                                                    subtype2,
                                                    MCC_LIMITS_DATA,
                                                    (pBYTE) &Mcc_Limits, 
                                                    sizeof(MCC_LIMITS) );

   //ATP116V-23
   memset( &Mcc_Limits, 0x00, sizeof(MCC_LIMITS) );

   return bReturnCode ;

} /* TxDataServer_Get_MCC_Limit_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_FAF01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_FAF01_Details (BYTE* caf_profile_id) 
{
	FAF01 sFAF01 ;

	BOOLEAN bReturnCode = true ;
	memset (&sFAF01, 0, sizeof (sFAF01)) ;

	strcpy (sFAF01.primary_key.caf_profile_id, caf_profile_id) ;
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST, 
																	ST1_DB_SELECT, 0, FAF01_DATA, 
																	(pBYTE) &sFAF01, sizeof(sFAF01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_FAF01_Details */

/*************************************************************************************
NAME:			TxDataServer_Get_CRF01_Details
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_CRF01_Details (BYTE* merchant_currency_code) 
{
	CRF01 sCRF01 ;

	BOOLEAN bReturnCode = true ;
	memset (&sCRF01, 0, sizeof (sCRF01)) ;

	strcpy (sCRF01.primary_key.currency_code, merchant_currency_code) ;
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST, 
																	ST1_DB_SELECT, 0, CRF01_DATA, 
																	(pBYTE) &sCRF01, sizeof(sCRF01)) ;
	return bReturnCode ;

} /* TxDataServer_Get_CRF01_Details */




/*************************************************************************************
NAME:			TxDataServer_Get_CSF01_Details
DESCRIPTION:	Retrieves the CSF01 record for the primary key 'status'	
INPUTS:			BYTE* card_status
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN TxDataServer_Get_CSF01_Details (BYTE* card_status)
{
	BOOLEAN bReturnCode = true ;
	CSF01 sCSF01 = {0} ;
	
	strcpy (sCSF01.primary_key.status, card_status) ;
	
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name, MT_DB_REQUEST, 
																	ST1_DB_SELECT, 0, CSF01_DATA, 
																	(pBYTE) &sCSF01, sizeof(sCSF01)) ;
	return bReturnCode ;
} /* TxDataServer_Get_CSF01_Details */

BOOLEAN TxDataServer_Get_DEF01_Details (void)
{
	BOOLEAN bReturnCode = true ;
	
	char strTrimmedMerchantId[20] = {0} ;
	DEF01 sDEF01 = {0} ;

	strTrim (Auth_Tx.MCF01_details.primary_key.merchant_id, strTrimmedMerchantId) ;
	
	strcpy (sDEF01.primary_key.organization_id, Auth_Tx.TLF01_details.organization_id) ;
	strcpy (sDEF01.primary_key.merchant_id, strTrimmedMerchantId) ;
	strcpy (sDEF01.primary_key.term_length, Auth_Tx.TLF01_details.deferred_term_length) ;
	
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name, MT_DB_REQUEST, 
																	ST1_DB_SELECT, 
																	0, DEF01_DATA, 
																	(pBYTE) &sDEF01, 
																	sizeof(sDEF01)) ;
	return bReturnCode ;
} /* TxDataServer_Get_DEF01_Details */

BOOLEAN TxDataServer_Get_UAG01_Details (void)
{
	BOOLEAN bReturnCode = true ;
	
	UAG01 sUAG01 = {0} ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name, MT_DB_REQUEST, 
																	ST1_DB_SELECT_AUTH_ID, 
																	0, UAG01_DATA, (pBYTE) &sUAG01, 
																	sizeof(sUAG01)) ;
	return bReturnCode ;
} /* TxDataServer_Get_UAG01_Details */

BOOLEAN TxDataServer_Get_MCF03P_Details (void )
{
	MCF03P sMCF03P ;
	BOOLEAN bReturnCode = true ;

	memset (&sMCF03P, 0, sizeof (sMCF03P)) ;
	strncpy (sMCF03P.merch_id, Auth_Tx.TLF01_details.merchant_id,15) ;
	if(Auth_Tx.TLF01_details.system_date[0] == '\0')
	{
		txservice_create_system_date( Auth_Tx.TLF01_details.system_date );
	}
	strncpy (sMCF03P.system_date,  Auth_Tx.TLF01_details.system_date,8) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
													 MT_DB_REQUEST,
													 ST1_DB_SELECT, 0,
													 MCF03P_DATA,
													 (pBYTE) &sMCF03P,
													 sizeof(sMCF03P)) ;
	return bReturnCode ;

} /* TxDataServer_Get_MCF01_Details */

#if(defined VISA7 || defined MC12_OCT2023 || defined MC13_OCT2023)
BOOLEAN TxDataServer_Get_Merchant_Integrity_Param_Details (char *category_cd)
{
	MERCH_INT_PARAMS sMIP;
	BOOLEAN bReturnCode = true ;

	memset (&sMIP, 0, sizeof (sMIP)) ;
	strncpy (sMIP.primary_key.merchant_id, Auth_Tx.TLF01_details.merchant_id, 15) ;
	strncpy (sMIP.primary_key.card_nbr,	   Auth_Tx.TLF01_details.card_num, 	  strlen(Auth_Tx.TLF01_details.card_num)) ;
	strncpy (sMIP.primary_key.category_cd, category_cd, 					  4) ;

	if((true == txservice_Check_If_MC_Transaction()) &&
	   (0 == strncmp(category_cd, MAC_CATEGORY_CODE, 4))
	  )
	{
		bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
													 	 MT_DB_REQUEST,
														 ST1_DB_SELECT, 0,
														 MERCH_INT_PARAM_MC_MAC_DATA,
														 (pBYTE) &sMIP,
														 sizeof(sMIP)) ;
	}
	else
	{
		bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
													 	 MT_DB_REQUEST,
														 ST1_DB_SELECT, 0,
														 MERCH_INT_PARAM_DATA,
														 (pBYTE) &sMIP,
														 sizeof(sMIP)) ;
	}

	return bReturnCode ;

} /* TxDataServer_Get_Merchant_Integrity_Param_Details */
#endif

#ifdef HCTS2
BOOLEAN TxDataserver_update_atp_card_status_based_on_vertexon_rc ()
{
	int i=0, codesCount = 0;
	BOOLEAN cardUpdateFlag = false;
	BOOLEAN bReturnCode = true ;
	PAF01 sPAF01;
	char tempStatus[51] = {0};
	//remove after UT
	char buffer[100] = {0};

	if(card_activation_flag == 'Y')
	{
		if(Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE ||
		   Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE )
		{
			//validate the response/action codes from Vertexon
			if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncmcr2") == 0)
			{
				/*sprintf(buffer, "ncmcr2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);*/

				codesCount = atoi(mc_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(mc_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncvis2") == 0)
			{
				/*sprintf(buffer, "ncvis2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);*/

				codesCount = atoi(visa_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(visa_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			else if((strcmp(Auth_Tx.TLF01_details.acquirer_id, "nposa2") == 0) ||
					(strcmp(Auth_Tx.TLF01_details.acquirer_id, "natma2") == 0))
			{
				/*sprintf(buffer, "nposa2/natma2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);*/

				codesCount = atoi(amex_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(amex_vertexon_codes[i], Auth_Tx.TLF01_details.product_codes[12].code, 3) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncjcb2") == 0)
			{
				/*sprintf(buffer, "ncjcb2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);*/

				codesCount = atoi(jcb_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(jcb_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncdci2") == 0)
			{
				/*sprintf(buffer, "ncdci2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);*/

				codesCount = atoi(dci_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(dci_vertexon_codes[i], Auth_Tx.TLF01_details.product_codes[12].code, 3) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "nccup2") == 0)
			{
				/*sprintf(buffer, "nccup2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);*/

				codesCount = atoi(upi_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(upi_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
					{
						cardUpdateFlag = true;
						break;
					}
				}
			}
			//also handled in dcpimp module
			/*else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "nciftr") == 0)
			{
				//sprintf(buffer, "nciftr checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
				//txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);

				if(strncmp(Auth_Tx.TLF01_details.card_num, "7", 1) == 0)
				{
					codesCount = atoi(pl_ft_vertexon_codes[0]);
					for(i=1; i<=codesCount; i++)
					{
						if(strncmp(pl_ft_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
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
						if(strncmp(mp_in_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
						{
							cardUpdateFlag = true;
							break;
						}
					}
				}
			}*/

			if(cardUpdateFlag == true)
			{
				sprintf(buffer, "Qualified for card status update with RC = [%s], requesting to trands", Auth_Tx.TLF01_details.response_code);
				txcntl_log_message(2, 1, buffer, "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);

				if(strlen(Auth_Tx.CCF02P_details.primary_key.card_nbr) == 0)
					strcpy(Auth_Tx.CCF02P_details.primary_key.card_nbr, Auth_Tx.TLF01_details.card_num);

				if(strlen(Auth_Tx.CCF02P_details.primary_key.card_type) == 0)
					strcpy(Auth_Tx.CCF02P_details.primary_key.card_type, "C");

				//storing STAN to display in Change History data
				memset(Auth_Tx.CCF02P_details.changed_by, 0, sizeof(Auth_Tx.CCF02P_details.changed_by));
				strcpy(Auth_Tx.CCF02P_details.changed_by, "STAN:");
				strcat(Auth_Tx.CCF02P_details.changed_by, Auth_Tx.TLF01_details.sys_trace_audit_num);

				//db call to update card status for both CCF02P and PAF01
				bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,
																 MT_DB_REQUEST,
																 ST1_DB_UPDATE, 0,
																 ATP_CARD_STATUS_UPDATE_BY_BDO_HOST,
																 (pBYTE)&Auth_Tx.CCF02P_details,
																 sizeof(Auth_Tx.CCF02P_details));
			}
			else
			{
				//txcntl_log_message(2, 1, "Configured RC not matched with Vertexon response code", "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 0);
			}
		}
	}
	else
	{
		txcntl_log_message(2, 2, "Vertexon Card Activation flag is disabled", "TxDataserver_update_atp_card_status_based_on_vertexon_rc", 1);
	}

	return bReturnCode ;
}
#endif

BOOLEAN TxDataServer_Update_UAG01( pCHAR AuthNumber )
{
   BOOLEAN bReturnCode;

   UAG01 sUAG01 = {0};
   strcpy( sUAG01.auth_id, AuthNumber );

   bReturnCode = txservice_Send_Request_To_Service( tran_dataserver_que_name,
                                                    MT_DB_REQUEST, 
                                                    ST1_DB_UPDATE,
                                                    ST2_NONE,
                                                    UAG01_DATA,
                                           (pBYTE) &sUAG01, 
                                                    sizeof(UAG01) );
   return bReturnCode;
}

BOOLEAN TxDataServer_Get_CR01_Details(void)
{
	CR01 sCR01 ;
	BOOLEAN bReturnCode = true ;

	memset (&sCR01, 0, sizeof (sCR01)) ;

	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST,
																	ST1_DB_SELECT,ST2_DB_SELECT_BIN_RANGE_CFB_TXN, CR01_DATA,
																	(pBYTE) &sCR01, sizeof(sCR01)) ;
	return bReturnCode ;

}

BOOLEAN TxDataServer_Update_CR01_Details(CR01 sCR01)
{

	BOOLEAN bReturnCode = true ;




	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name,MT_DB_REQUEST,
													ST1_DB_UPDATE,0,
													CR01_DATA,
													(pBYTE) &sCR01,
													sizeof(sCR01)) ;
	return bReturnCode ;

}

BOOLEAN TxDataServer_Get_CCF04_Details (void)
{
	BOOLEAN bReturnCode = true ;
	
	CCF04 sCCF04 = {0} ;

	strcpy (sCCF04.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
	strcpy (sCCF04.primary_key.card_nbr,  Auth_Tx.TLF01_details.card_num) ;
	strcpy (sCCF04.mcc_lo_range,          Auth_Tx.TLF01_details.category_code) ;
	
	bReturnCode = txservice_Send_Request_To_Service (tran_dataserver_que_name, MT_DB_REQUEST, 
																	ST1_DB_SELECT, 
																	0, CCF04_DATA, (pBYTE) &sCCF04, 
																	sizeof(sCCF04)) ;
	return bReturnCode ;
} /* TxDataServer_Get_CCF04_Details */


/*************************************************************************************
NAME:			TxCntl_Process_Db_Reply
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BYTE TxDataServer_Process_Db_Reply (pPTE_MSG p_msg_in)
{
	LONG lReturnCode = 0L ;
	BYTE ReturnCode = NO_AUTH_ERROR ;
	INT  MGF03P_Result_code = true;
	BYTE RetCode = NO_AUTH_ERROR ;
	BOOLEAN bReturnCode = true ;
	BYTE MessageSubType2 = 0 ;
	BYTE MessageSubType1 = 0 ;
	BYTE AppDataType = 0 ;
	BYTE ResultCode = 0 ;
	BYTE Buffer[512]; /* Girija Y, Phani */
	char ErrorMsg[1024];//praneteh code
	double dMerchant_Currency_Code_In_US = 0.0,nAuthTxTotalAmount=0.0 ;
	double dCurrency_Conversion_Rate = 0.0 ;

	LONG nNumberOfRecordsReturned = 0L ;
    LONG  nTotalAmount = 0;
    double CF_nTotalAmount=0.0;
    LONG  nProfileLimit = 0;



    INT exception_cardnum_flag;
	LONG nTranDateInJulian = 0L ;
	BYTE strTemp[1000] = {0};
	BYTE strSystemDateTime[50] = {0} ;
	BYTE strTranDateInJulian[10] = {0} ;
	int nAuthIndicator = 0 ;
	char strTrimmedMerchantId[20] = {0} ;
	char strResponseCode[256] = {0} ;

	BYTE strTxType[7] = {0} ;

	int nIndex = 0 ;

	pBYTE			pATEMessageDataData = 0 ;
	pPTE_MSG_DATA	pATEMessageData = 0 ;

	CHAR       date_str [ 9] = "";
	CHAR       time_date[25] = "";
	int	CurrencyType = 0 ;
	double ConvertedTxAmount = 0;
	double nConversionRate =0;
	double nConvertedTxAmount=0.0;
	BYTE UpdateACF01Struct = 0;
	BYTE UpdateCashBonusStruct = 0;
	BYTE UpdateCCF03PStruct = 0;
	BYTE tempamount [13]="";
	CHAR   strError[256] = {0} ;

	double cumltv_amount= 0.0;
	double namount = 0.0;
	double nCreditAuthMax = 0.0;
	double nCreditPurchMax = 0.0;

	char *peso_amt_limit;
	long int_peso_amt_limit;
	char *usd_amt_limit;
	long int_usd_amt_limit;
	char CCF01_name[151]={0};
	char Txnfirst_name[51]={0},Txnmiddle_name[2]={0},Txnlast_name[51]={0};
	char Dbfirst_name[51]={0},Dbmiddle_name[2]={0},Dblast_name[51]={0};
	char *lastnameptr=NULL;
	int firstName_validation_Result=0;
	int middleName_validation_Result=0;
	int lastName_validation_Result=0;

	CCF02P sCCF02P = {0} ;
	/*DCF01 sDCF01 = {0} ;*/
	CAF01 sCAF01 = {0} ;
	/*MCF01 sMCF01 = {0} ;*/
	PAF01 sPAF01 = {0} ;
	ACF01 sACF01 = {0} ;
	CRF01 sCRF01 = {0} ;
	CCF03 sCCF03P = {0} ;
	FAF01 sFAF01 = {0} ;
	BIN01 sBIN01 = {0} ;
	TLF01 sTLF01 = {0} ;
	CSF01 sCSF01 = {0} ;
	DEF01 sDEF01 = {0} ;
	BON01 sBON01 = {0} ;
	UAG01 sUAG01 = {0} ;
	CCF04 sCCF04 = {0} ;
	CCF01 sCCF01 ={0};
	BIN02 sBIN02 = {0} ;
    NEG01 sNEG01 = {0} ;
	OMC01 sOMC01 = {0};
	CR01 sCR01 = {0};
	MCF03P sMCF03P;
    Offus_velocity sOffus_velocity = {0} ;
    COUNTRY_AMT_LIMITS sCOUNTRY_AMT_LIMITS = {0};
    #if (defined VISA7 || defined MC12_OCT2023 || defined MC13_OCT2023)
    MERCH_INT_PARAMS sMIP = {0};
    #endif

	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memset (&sOffus_velocity, 0, sizeof sOffus_velocity) ;
	memset (&sCCF02P, 0, sizeof sCCF02P) ;
	/*memset (&sDCF01,  0, sizeof sDCF01) ;*/
	memset (&sCAF01,  0, sizeof sCAF01) ;
	/*memset (&sMCF01,  0, sizeof sMCF01) ;*/
	memset (&sCCF02P, 0, sizeof sCCF02P) ;
	memset (&sPAF01,  0, sizeof sPAF01) ;
	memset (&sACF01,  0, sizeof sACF01) ;
	memset (&sCRF01,  0, sizeof sCRF01) ;
	memset (&sCCF03P, 0, sizeof sCCF03P) ;
	memset (&sFAF01,  0, sizeof sFAF01) ;
	memset (&sBIN01,  0, sizeof sBIN01) ;
	memset (&sTLF01,  0, sizeof sTLF01) ;
	memset (&sCSF01,  0, sizeof sCSF01) ;
	memset (&sDEF01,  0, sizeof sDEF01) ;
	memset (&sBON01,  0, sizeof sBON01) ;
	memset (&sUAG01,  0, sizeof sUAG01) ;
	memset (&sCCF04,  0, sizeof sCCF04) ;
	memset (&sBIN02,  0, sizeof sBIN02) ;
	memset (&sCR01,  0, sizeof sCR01) ;
	memset (&sMCF03P, 0, sizeof sMCF03P) ;
		
	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	MessageSubType1         = ptemsg_get_msg_subtype1 (p_msg_in) ;
	MessageSubType2			= ptemsg_get_msg_subtype2 (p_msg_in) ;
	AppDataType             = ptemsg_get_pte_msg_data_app_data_type (pATEMessageData) ;
	ResultCode				= ptemsg_get_result_code (p_msg_in) ;


   /**************************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof(AUTH_TX)) ;
   /**************************************************************************/

   if (ResultCode != PTEMSG_OK && ResultCode != PTEMSG_NOT_FOUND)
   {
      switch( AppDataType )
      {
         case UAG01_DATA:
                  if ( ResultCode == 5 )
                  {
                     strcpy( strTemp,
                            "Err: Too many rows, database table UAG01" );
                  }
                  else
                  {
                     sprintf( strTemp,
                             "Err: Result code %d database table UAG01",
                              ResultCode );
                  }
				  /* Ignore ERROR responses to update UAG01. */
				  
				  strcat (strTemp, pATEMessageDataData+sizeof(AUTH_TX)) ;
				  ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
														 "txservice_Process_Db_Reply",
														  GS_TXN_SYSTEM_ERROR, 
														  SYSTEM_MALFUNCTION) ;
			      return NO_AUTH_ERROR;
                 // break;

         default:
                  sprintf (strTemp,
                          "DB Table Err AppData: %d ResultCode: %d",
                           AppDataType, ResultCode) ;
      }

      strcat (strTemp, pATEMessageDataData+sizeof(AUTH_TX)) ;
      ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                             "txservice_Process_Db_Reply",
                                              GS_TXN_SYSTEM_ERROR, 
                                              SYSTEM_MALFUNCTION) ;
		
		return SYSTEM_ERROR ;
	}
   else if ( (ResultCode      == PTEMSG_OK)   &&
             (AppDataType     == UAG01_DATA)  &&
             (MessageSubType1 == ST1_DB_UPDATE) )
   {
      /* Ignore responses to update UAG01. */
      return NO_AUTH_ERROR;
   }

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

    /* Perform semantic checks on the transaction */
	ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
	if (ReturnCode != NO_AUTH_ERROR)
		return ReturnCode ;
		
	if(ST1_DB_UPDATE == MessageSubType1)
	{
		if((Auth_Tx.TLF01_details.tx_key > 100) && 
		((atoi(Auth_Tx.BIN01_details.auth_ind)) == LOCAL_PROCESSING_INDICATOR) &&
		 (!strEqual (Auth_Tx.CAF01_details.authorization_location, LOCAL_AUTHORIZATION)))
		{
			//The balance has been successfully updated, continue with transaction. 
			//Send the message back to the originator
			txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
    														     MT_AUTH_RESPONSE, 
    														     0, 
    														     0);
			return RECORD_FOUND_IN_SHARED_MEMORY;
		}//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
	}

	switch (MessageSubType1) 
	{
	/*	case ST1_DB_UPDATE:
			switch (AppDataType)
			{
				case ALL_DATA: 
					if (ResultCode != PTEMSG_OK) 
					{
						
						if (AUTH_REVERSAL_RESPONSE == Auth_Tx.TLF01_details.tx_key && 
							 (!strcmp (Auth_Tx.TLF01_details.response_code, "00")))
						{
							strcpy (strTemp, "Overiding db error with a 00 reversal response") ;
							txservice_Generate_Usage_Error_Message (1,strTemp, 
																	"Process_Db_Reply",
																	GS_TXN_INVALID_AUTHORIZATION_METHOD, 
																	TRANSACTION_APPROVED) ;
							return NO_UPDATER_ERROR ;
						}
						else
						{
							ReturnCode = txservice_Generate_Usage_Error_Message (1,
																"Problems encountered with the Update processor", 
																"txservice_Process_Db_Reply",
																 GS_TXN_SYSTEM_ERROR, 
																 SYSTEM_MALFUNCTION) ;
							return UPDATER_ERROR ;
						}
					}
					else 
					{
																
						return NO_UPDATER_ERROR ;
					}
										
				default:
                    strcpy (strTemp,"Unknown app data type from data server (ST1_DB_UPDATE)") ; 
                    if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                        strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
					txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_SYSTEM_ERROR,SYSTEM_MALFUNCTION) ;
					return UPDATER_ERROR ;

				break ;

			}
		break ;*/

      case ST1_DB_SELECT_AUTH_ID:
         switch (AppDataType)
         {
            case UAG01_DATA:
               if (PTEMSG_NOT_FOUND == ResultCode)
               {
                  strcpy (strTemp, "Authorization Id(UAG01)record not found") ;
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                         "Process_Db_Reply",
                                                          GS_TXN_UAG_NOT_FOUND,
                                                          REFER_TO_CARD_ISSUER);
                  return ReturnCode ;
               }
               else /* PTEMSG_OK */
               {
                  /* Assign an authorization number for the transaction.
                   * This gets cleared later if transaction is declined.
                   */
                  memcpy (&sUAG01, pATEMessageDataData+sizeof(AUTH_TX) ,sizeof (UAG01));
                  if ( strlen (Auth_Tx.TLF01_details.auth_number) == 0 )
                  {
                     strcpy (Auth_Tx.TLF01_details.auth_number, sUAG01.auth_id) ;

                     /* RSB and deferred transactions must have
                      * an auth number format of 'Knnnnn'.
                      */
                     if ( (Auth_Tx.BIN01_details.credit_subtype[0] == RSB_CARD) ||
                          (txservice_If_Tx_Is_A_Deferred_Purchase_Type())        )
                     {
                        Auth_Tx.TLF01_details.auth_number[0] = 'K';
                     }
                  }

                  return NO_AUTH_ERROR;  /* Sends back to originator. */
               }
            break ; /* case UAG01_DATA */

				default:

					strcpy (strTemp, "Invalid AppDataType from Data Server") ;
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		"Process_Db_Reply",
																		GS_TXN_SYSTEM_ERROR, 
																		SYSTEM_MALFUNCTION) ;
					return SYSTEM_ERROR ; 
				break ;
			}
		break ; /* case ST1_DB_SELECT_AUTH_ID */

      /* This is the return point after a call
       * to TxDataServer_Get_BIN02_Details().
       */
      case ST1_DB_CHECK_BIN02_RANGE:
         switch (AppDataType)
         {
            case BIN02_DATA:

               if (PTEMSG_NOT_FOUND == ResultCode) 
               {
                  /* Card number was not found in BIN02.
                   * It might need to go to Cirrus (this is for Equitable).
                   */
                  ReturnCode = txservice_Switch_to_Cirrus() ;
                  return ReturnCode ;
               }
               else /* PTEMSG_OK */
               {
                  /* Send to the PLUS network - this network
                   * name should be in destination_name.
                   */
                  memcpy( &sBIN02,
                          pATEMessageDataData+sizeof(AUTH_TX),
                          sizeof(sBIN02));
				  /* EQIT Interface Changes, Jan 02 2009, Girija Y, TF */
                  /* Added code to pass network Id to eqit */

                  txservice_Eqit_POS_Field63(&Auth_Tx,sBIN02.destination_name);

                  ReturnCode = txservice_Process_Bin_Range_Switch_Out(BIN02_DATA,
                                                        sBIN02.destination_name,
                                                        "");
                  return ReturnCode ;
               }
            break ; /* case BIN02_DATA */

            default:
               strcpy (strTemp, "Invalid AppDataType from Data Server") ;
               ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                                           "Process_Db_Reply",
                                                            GS_TXN_SYSTEM_ERROR,
                                                            SYSTEM_MALFUNCTION);
               return SYSTEM_ERROR ;
            break ;
      }
      break ;

		/* This is the return point after a call to TxDataServer_Get_BIN01_Details() */
		case ST1_DB_CHECK_BIN_RANGE:
			switch (AppDataType)
			{
				case BIN01_DATA:	/* Response from TxDataServer_Get_BIN01_Details (caf_profile_id)*/

					if (PTEMSG_NOT_FOUND == ResultCode) 
					{
						ReturnCode = txservice_Process_When_Bin_Range_Not_Found () ;
						// Check if we have converted the Currency code during Local Processing
					    if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
							(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
							((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
							 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
						{
						   strncpy( Auth_Tx.TLF01_details.currency_code,
								    USDOLLAR_CURRENCY,
								    CURRENCY_LEN);
						}
						return ReturnCode ;
					}
					else /* PTEMSG_OK */
					{	
						memcpy (&sBIN01, pATEMessageDataData+sizeof(AUTH_TX),sizeof(sBIN01)) ;
						/* Store the BIN01 record in Auth_Tx */
						memcpy (&Auth_Tx.BIN01_details, &sBIN01, sizeof(sBIN01)) ;
						/* will need to add code snippet to handle the standin processing request*/

						if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN) == 0)
						{
							/* This code is executed only when the BIN01 is not present 
							in shared memory when the standin processing is checking for
							the BIN range of the card which has to be approved*/
							txservice_perform_standin_processing();
							return RECORD_FOUND_IN_SHARED_MEMORY;
						}
						/* EQIT Interface Changes, Jan 02 2009, Girija Y, TF */
                        /* Added code to pass network Id to eqit */

                        txservice_Eqit_POS_Field63(&Auth_Tx,Auth_Tx.BIN01_details.destination_name);

					#ifdef MC13_OCT2023
                        if( Auth_Tx.MCF01_details.division_nbr[0] == 'C'     &&
                        	txcntl_check_if_txn_falls_under_mac_validation() &&
							(true == txservice_Check_If_MC_Transaction()) 	 &&
							txcntl_check_if_cnp_txn_flag_is_enabled_with_config_pos_codes()
                          )
                        {
                        	ReturnCode = TxDataServer_Get_Merchant_Integrity_Param_Details(MAC_CATEGORY_CODE);
                        }
                        else
					#endif
					#ifdef VISA7
                        if( Auth_Tx.MCF01_details.division_nbr[0] == 'C' &&
                        	txcntl_check_if_txn_falls_under_merchant_ingerity_validation()&&
							(true == txservice_Check_If_VISA_Transaction() ||true == txservice_Check_If_MC_Transaction()))
                        {
                        	if( true == txservice_Check_If_VISA_Transaction() &&
                        	   (cat1_MID_integrity_validation_flag == 'Y'     ||
                        	    cat2_MID_integrity_validation_flag == 'Y'     ||
								cat3_MID_integrity_validation_flag == 'Y'     ||
								cat4_MID_integrity_validation_flag == 'Y'    ))
                        	{
                        		ReturnCode = TxDataServer_Get_Merchant_Integrity_Param_Details(VISA_CATEGORY_CODE);
                        	}
						#ifdef MC12_OCT2023
                        	else if(true == txservice_Check_If_MC_Transaction() &&  excessive_declines_flag == 'Y')
                        	{
                        		ReturnCode = TxDataServer_Get_Merchant_Integrity_Param_Details(MC_CATEGORY_CODE);
                        	}
                        	else
                        	{
                        				ReturnCode = txservice_Process_When_Bin_Range_Found () ;
                        	}
						#endif
                        }
					#endif
                        else
                        {
                        	ReturnCode = txservice_Process_When_Bin_Range_Found () ;
                        }


						return ReturnCode ;
						
					}
				break ; /* case BIN01_DATA */
					
				default:
					strcpy (strTemp, "Invalid AppDataType from Data Server") ;
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		"Process_Db_Reply",
																		 GS_TXN_SYSTEM_ERROR, 
																		 SYSTEM_MALFUNCTION) ;
					return SYSTEM_ERROR ;
					
				break ;

			} /* switch */
		break ;

      case ST1_DB_SELECT:
         switch (AppDataType)
         {
#if (defined VISA7 || defined MC12_OCT2023 || defined MC13_OCT2023)
         	 case MERCH_INT_PARAM_DATA:
         	 case MERCH_INT_PARAM_MC_MAC_DATA:
         		 if(ResultCode == PTEMSG_NOT_FOUND || ResultCode != PTEMSG_OK)
         		 {
         			 ReturnCode = txservice_Process_When_Bin_Range_Found () ;

         			 return ReturnCode;
         		 }
         		 else
         		 {
         			 memcpy(&sMIP, pATEMessageDataData+sizeof(AUTH_TX), sizeof(MERCH_INT_PARAM));
         			 memcpy(&MERCH_INT_PARAM, &sMIP, sizeof(MERCH_INT_PARAM));
         			 if(true == txservice_Check_If_VISA_Transaction())
         			 {
         				 ReturnCode = txservice_Merchant_Integrity_Param_Check();
         			 }
         			 else if(true == txservice_Check_If_MC_Transaction())
         			 {
         				if(strncmp(MERCH_INT_PARAM.primary_key.category_cd, MAC_CATEGORY_CODE, 4) == 0)
         				{
         					ReturnCode = txservice_Merchant_Integrity_Param_Check_for_MAC();
         				}
         				else
         				    ReturnCode = txservice_Merchant_Integrity_Param_Check_for_MC();
         			 }
         			 return ReturnCode ;
         		 }
        	 break;
#endif

            case NEG01_DATA:

               if (PTEMSG_NOT_FOUND == ResultCode)
               {
                  if ( strEqual(Auth_Tx.CAF01_details.authorization_method,
                                M_NEGATIVE_OFFUS_NEGATIVE_FRAUD) )
                  {
                     /* Check if card is on the Negative Fraud file. */
                     bReturnCode = TxDataServer_Get_Neg_Fraud_Details();
                     if (bReturnCode == false) 
                        return SYSTEM_ERROR ;
                     return NO_AUTH_ERROR ;
                  }
                  else if ( strEqual(Auth_Tx.CAF01_details.authorization_method,
                                NEGATIVE_WITH_OFFUS_VELOCITY)
                     &&
                       txservice_If_Tx_Qualifies_For_Card_Velocity_Checking() )
                  {
                     bReturnCode = TxDataServer_Get_Offus_Velocity_Details () ;
                     if (bReturnCode == false) 
                        return SYSTEM_ERROR ;
                     return NO_AUTH_ERROR ;
                  }
                  else
                  {
                     ReturnCode = TxDataServer_Continue_With_Local_Processing();
                     return ReturnCode ;
                  }
               }
               else /* PTEMSG_OK */
               {
                  /* Error situation because card is on negative file list */
                  strcpy (strTemp, "Tx cannot be processed - Hot Card") ;
                  if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
                  {
                     ReturnCode = txservice_Generate_Usage_Error_Message( 2,
                                                 strTemp,
                                                "Process_Db_Reply",
                                                 GS_TXN_TRANSACTION_NOT_ALLOWED,
                                                 DO_NOT_HONOR) ;
                  }
                  else
                  {
                     /* Get the status code from the negative file NEG01. */
                     memset( &sNEG01, 0x00, sizeof(NEG01) );
                     memcpy( &sNEG01,
                             pATEMessageDataData+sizeof(AUTH_TX),
                             sizeof(NEG01) );

                     if ( sNEG01.card_status[0] == '5' )
                     {
                        /* Use response code 05 when status is 5. SCR 623 */
                        strcat( strTemp, " (5)" );
                        ReturnCode = txservice_Generate_Usage_Error_Message( 2,
                                                 strTemp,
                                                "Process_Db_Reply",
                                                 GS_TXN_TRANSACTION_NOT_ALLOWED,
                                                 DO_NOT_HONOR) ;
                     }
                     else
                     {
                        /* Use response code 41 for statuses other than 05.
                         * Typically, the only other status will be 06.
                         */
                        sprintf( strTxType, " (%c)", sNEG01.card_status[0] );
                        strcat( strTemp, strTxType );
                        ReturnCode = txservice_Generate_Usage_Error_Message( 2,
                                                 strTemp,
                                                "Process_Db_Reply",
                                                 GS_TXN_TRANSACTION_NOT_ALLOWED,
                                                 LOST_CARD) ;
                     }
                  }
                  return ReturnCode ;
               }
            break ;


            case NEGATIVE_FRAUD_DATA:

               if (PTEMSG_NOT_FOUND == ResultCode)
               {
                  /* Card is not in Negative Fraud bin.
                   * Good!  Continue processing; get off-us velocity
                   * information because there is only one config
                   * parameter for Neg Fraud and that includes doing
                   * off-us velocity.
                   */
                  if ( txservice_If_Tx_Qualifies_For_Card_Velocity_Checking() )
                  {
                     bReturnCode = TxDataServer_Get_Offus_Velocity_Details () ;
                     if (bReturnCode == false) 
                        return SYSTEM_ERROR ;
                     return NO_AUTH_ERROR ;
                  }
                  else
                  {
                     ReturnCode = TxDataServer_Continue_With_Local_Processing();
                     return ReturnCode ;
                  }
               }
               else /* PTEMSG_OK */
               {
                  /* Error situation because card is on negative fraud list */
                  strcpy (strTemp, "Txn is in Negative Fraud list") ;
                  ReturnCode = txservice_Generate_Usage_Error_Message( 2,
                                                 strTemp,
                                                "Process_Db_Reply",
                                                 GS_TXN_TRANSACTION_NOT_ALLOWED,
                                                 TXN_NOT_PERMITTED_FOR_CARD) ;
                  return ReturnCode ;
               }
            break;


				case CAF01_DATA:	/* Response from TxDataServer_Get_CAF01_Details (caf_profile_id) */
					
					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						ReturnCode = TxDataServer_Process_CAF_Not_Found (MessageSubType2) ;
						// Check if we have converted the Currency code during Local Processing
							if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
								(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
								((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
								 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
							{
							   strncpy( Auth_Tx.TLF01_details.currency_code,
										USDOLLAR_CURRENCY,
										CURRENCY_LEN);
							}
						return ReturnCode ;
					}
					else /* PTEMSG_OK */
					{
						memcpy (&sCAF01, pATEMessageDataData+sizeof(AUTH_TX), sizeof (CAF01)) ;
						memcpy (&Auth_Tx.CAF01_details, &sCAF01, sizeof(sCAF01)) ;

					   if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN) == 0)
					   {
							/* This code is executed only when the BIN01 is not present 
							in shared memory when the standin processing is checking for
							the BIN range of the card which has to be approved*/
							txservice_perform_standin_processing();
							return RECORD_FOUND_IN_SHARED_MEMORY;
						}

						ReturnCode = TxDataServer_Process_CAF_Found (MessageSubType2) ;
						return ReturnCode ;
					}
				break ; /* case CAF01_DATA */
				
				case MCF01_DATA:	
				break ; /* case MCF01_DATA */

				case MCT01_DATA:
					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						strTrim (Auth_Tx.TLF01_details.merchant_id, strTrimmedMerchantId) ;
						/* The merchant is not authorized to process the card_type */
						sprintf (strTemp, 
							"Mid is not authorized for cardtype %s\n",
							  Auth_Tx.BIN01_details.card_type) ;

						/*******/
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		  "Process_Db_Reply",
																		  GS_TXN_MERCHANT_BIN_NOT_FOUND, 
																		  REFER_TO_CARD_ISSUER) ;
						// Check if we have converted the Currency code during Local Processing
						if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
							(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
							((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
							 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
						{
						   strncpy( Auth_Tx.TLF01_details.currency_code,
									USDOLLAR_CURRENCY,
									CURRENCY_LEN);
						}
						return ReturnCode ;
						/*******/
					}
                    else
                    {
                        ReturnCode = txservice_Continue_Processing_When_Bin_Range_Found () ;
                        return ReturnCode ;
                    }

				break ; /* case MCT01_DATA */
				
				case CCF02P_DATA:
															
					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						if((Auth_Tx.TLF01_details.tx_key > 100) && 
							((atoi(Auth_Tx.BIN01_details.auth_ind)) == LOCAL_PROCESSING_INDICATOR) &&
							(!strEqual (Auth_Tx.CAF01_details.authorization_location, LOCAL_AUTHORIZATION))							)
						{
							/* TF PHANI */
							memset(&Buffer, 0, sizeof(Buffer));
						    sprintf( Buffer,"Transaction Approved by HOST,Unable to do Balance_Update: The CCF02P record does not exists for the  transaction id: %s transaction amount: %s, STAN: %s,",
    											Auth_Tx.TLF01_details.primary_key.transaction_id,
    											Auth_Tx.TLF01_details.total_amount, Auth_Tx.TLF01_details.sys_trace_audit_num);
    										LogEvent( Buffer, INFO_MSG );
							printf( "%s\n", Buffer );
							txcntl_log_message( 2, 3,Buffer, "Process_Db_Reply", 1 );
    						txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
    																						 MT_AUTH_RESPONSE, 
    																						 0, 
    																						 0);
							return RECORD_FOUND_IN_SHARED_MEMORY;

						}//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16

						sprintf (strTemp, 
							"Card%s notfound,type %s\n",
							Auth_Tx.TLF01_details.card_num,Auth_Tx.BIN01_details.primary_key.type) ;
						/******/
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			  "Process_Db_Reply",
																			  GS_TXN_CCF02P_NOT_FOUND, 
																			  INVALID_ACCOUNT_NUMBER) ;
						// Check if we have converted the Currency code during Local Processing
						if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
							(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
							((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
							 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
						{
						   strncpy( Auth_Tx.TLF01_details.currency_code,
									USDOLLAR_CURRENCY,
									CURRENCY_LEN);
						}
						return ReturnCode ;
						/******/
						
					}
					else /* PTEMSG_OK */
					{
						memcpy (&sCCF02P, pATEMessageDataData+sizeof(AUTH_TX) ,sizeof(sCCF02P)) ;
						/* Store the CCF02 record in Auth_Tx */
						memcpy (&Auth_Tx.CCF02P_details, &sCCF02P, sizeof(sCCF02P)) ;						

					#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
						if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncpos", 5)) 		&&
						   (0 == strncmp(Auth_Tx.host_processing_code, "334000", 6))       		&&
						   (0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code, STANDIN)) 	&&
						   (Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION)
						   )
						{
							//when DE4 amount zeros
							if(atoi(Auth_Tx.TLF01_details.total_amount) == 0)
							{
								//case 1
								if(sCCF02P.status[0] == ' ' 			 ||
								   strncmp(sCCF02P.status, "  ", 2) == 0 ||
								   strncmp(sCCF02P.status, "02", 2) == 0)
								{
									txcntl_log_message( 2, 1, "Approved ZVAV txn with DE4 zeros and account status is ACTIVE", "txservice_Process_Db_Reply", 1 );
									if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
									{
										/* No. Generate a numeric authorization id */
										(void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
									}
									txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_COMPLETED,	TRANSACTION_APPROVED);
								}
								//case 2
								else
								{
									txcntl_log_message( 2, 1, "Declined ZVAV txn with DE4 zeros and account status is NOT ACTIVE", "txservice_Process_Db_Reply", 1 );
									txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CCF02P_STATUS, REFER_TO_CARD_ISSUER);
								}
								return NO_UPDATER_ERROR;
							}
							//when DE4 amount non-zeros
							else
							{
								//case 3
								if(sCCF02P.status[0] == ' ' 			 ||
								   strncmp(sCCF02P.status, "  ", 2) == 0 ||
								   strncmp(sCCF02P.status, "02", 2) == 0)
								{
									txcntl_log_message( 2, 1, "Approved ZVAV txn with DE4 non-zeros and account status is ACTIVE", "txservice_Process_Db_Reply", 1 );
									if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
									{
										/* No. Generate a numeric authorization id */
										(void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
									}
									txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_COMPLETED,	TRANSACTION_APPROVED);
								}
								//case 4
								else
								{
									txcntl_log_message( 2, 1, "Declined ZVAV txn with DE4 non-zeros and account status is NOT ACTIVE", "txservice_Process_Db_Reply", 1 );
									txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CCF02P_STATUS, REFER_TO_CARD_ISSUER);
								}

								return NO_UPDATER_ERROR;
							}
						}
					#endif

						//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						if((Auth_Tx.TLF01_details.tx_key > 100) && 
							(atoi(Auth_Tx.BIN01_details.auth_ind) == LOCAL_PROCESSING_INDICATOR) &&
							(!strEqual (Auth_Tx.CAF01_details.authorization_location, LOCAL_AUTHORIZATION)))
						{
							// The CCF02P data is requested for cadience balance update requirement. 
							//if the data has been obtained, continue with balance_update

							ReturnCode =txserviceCadience_Continue_With_CCF02P_For_Balance_Update();
								if(ReturnCode == NO_AUTH_ERROR)
									return RECORD_FOUND_IN_SHARED_MEMORY;
								else
									return ReturnCode ;
						

						}//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						
					    
					  if ( (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE)   &&
						   (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_REFUND) &&
						   (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_CASH_ADVANCE) &&
						   (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_RELOAD) &&
						   (Auth_Tx.TLF01_details.tx_key != AUTH_REFUND) &&
							(false == txservice_check_for_AMEX_refund_txn()))
					  {/*AMEX02*/
						 if (!txservice_If_Tx_Cannot_Be_Declined())
						 {
								/* if (card status is not 01 or 02), the card is invalid */

							if(Auth_Tx.TLF01_details.product_codes[6].quantity[0] == 'R')
							{
								if(sCCF02P.status[0] == ' '              ||
								   strncmp (sCCF02P.status, "02",2) == 0 || strncmp (sCCF02P.status, "01", 2) == 0 )
								{
									// Do Nothing
								}
								else
								{
									sprintf (strTemp, "Invalid card (CCF02P) status %s\n",sCCF02P.status) ;
									 if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
										strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
									 txcntl_log_message( 2, 3,Buffer, "txservice_Process_Db_Reply", 1 );
									 if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == CARD_NOT_PRESENT_MC_DE61_SE5)
									 {
										 Auth_Tx.current_balance_sign='1';
									 }
									 ReturnCode = TxDataServer_Get_CSF01_Details (sCCF02P.status) ;
									/* to retrieve the user entered ISO response code and text */
									 if (false == ReturnCode) /* XIPC error */
										 return SYSTEM_ERROR ;
									 break ;
								}
							}
							else if(true == txservice_check_for_DCI_refund_txn() ||true == txservice_check_for_AMEX_refund_txn())
							{
								if(sCCF02P.status[0] == 'I' 			 || sCCF02P.status[0] == ' '              ||
								   strncmp (sCCF02P.status, "02",2) == 0 || strncmp (sCCF02P.status, "01", 2) == 0 ||
								   strncmp (sCCF02P.status, "03",2) == 0 ||  strncmp (sCCF02P.status, "23",2) == 0)
									{
										// Do Nothing
									}
									else
									{
										ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																							  "txservice_perform_standin_processing",
																							  GS_TXN_TRANSACTION_NOT_ALLOWED,
																							  DO_NOT_HONOR) ;
										 return ReturnCode;
									}
							}
							else if(Auth_Tx.TLF01_details.product_codes[6].quantity[0] == 'S')
							{
									if( (0==strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6)) &&
										(0==strncmp(Auth_Tx.TLF01_details.message_type,"200",6)) &&
										(0==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)))
										{
												if(sCCF02P.status[0] == 'I')
												{
														ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																														  "txservice_perform_standin_processing",
																														  GS_TXN_TRANSACTION_NOT_ALLOWED,
																														  DO_NOT_HONOR) ;
														return ReturnCode;

												}

										}
										else if(true==txservice_Check_If_Transaction_Is_Incoming_RPS_STIP())
										{

												if(sCCF02P.status[0] == 'I')
												{
														ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																														  "txservice_perform_standin_processing",
																														  GS_TXN_TRANSACTION_NOT_ALLOWED,
																														  DO_NOT_HONOR) ;
														return ReturnCode;

												}

										}


							}
							else if((strcmp (sCCF02P.status, "01")) && (strcmp (sCCF02P.status, "02")) &&
									(strcmp (sCCF02P.status, "  ")))
							{
								 sprintf (strTemp, "Invalid card (CCF02P) status %s\n",sCCF02P.status) ;
								 if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
									strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
								 txcntl_log_message( 2, 3,Buffer, "txservice_Process_Db_Reply", 1 );
								 if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == CARD_NOT_PRESENT_MC_DE61_SE5)
								 {
									 Auth_Tx.current_balance_sign='1';
								 }
								 bReturnCode = TxDataServer_Get_CSF01_Details (sCCF02P.status) ;
								/* to retrieve the user entered ISO response code and text */
								 if (false == bReturnCode) /* XIPC error */
									 return SYSTEM_ERROR ;
								 break ;
							}

						 }
					  }

					}

					if((true == txservice_Check_If_CUP_Transaction()) &&
					   (0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))&&
					   (true == txservice_Check_If_CUP_ECOM ())&&
					   (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)&&
					   ((0 != strncmp( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY, CURRENCY_LEN ))&&
						 (0 != strncmp ( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))))
					  {
						ReturnCode = txservice_Validate_CUP_ECOM_TRAN();
						return ReturnCode;
					  }
					/*if((true == txservice_Check_If_Validate_CVN2_FOR_CUP_Non_ECOM_TRAN()) &&
					   (true == txservice_Check_If_CUP_Transaction()) &&
					   (0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN)))
					{
						ReturnCode = TxAuthorize_Process_Security (NSP_CUP_ECOM_CVN2) ;
						return ReturnCode ;
					}*/
					if (Auth_Tx.CCF02P_details.visa_purchase_card[0] == 'Y' &&
						 Auth_Tx.TLF01_details.category_code[0] != 0 )
					{
						bReturnCode = TxDataServer_Get_CCF04_Details() ; 
						/* details about the Visa Purchase card */
						if (false == bReturnCode) /* XIPC error */
							return SYSTEM_ERROR ;
						break ;
					}

#ifdef MC07_OCT2023
					/*
					 * Should be ASI, DE108 SE07 SF1 should be "10" or "11", only then do the Name Validation.
					 */
			if (name_validation_flag== 'Y' &&
						 ( MC_ASI_TXN_IDENTIFIER_VALUE == Auth_Tx.TLF01_details.product_codes[16].amount[6])&&
					     strncmp(Auth_Tx.TLF01_details.processing_code ,"000000",6)==0 &&
					     strncmp(Auth_Tx.TLF01_details.total_amount ,"000000000000",12)==0 &&
						 (0==strncmp(Auth_Tx.MCF01_details.credit_purch_max,"10",2) ||
						 0==strncmp(Auth_Tx.MCF01_details.credit_purch_max,"11",2)) )
					{
						bReturnCode = TxDataServer_Get_CCF01_Details() ;
						// Get the name field for the name validation
						if (false == bReturnCode) /* XIPC error */
						return SYSTEM_ERROR ;
						break;

					}
#endif
					/* Retrieve PAF01 details only if the primary acct nbr is present
					   at the CCF02P level for the card */
					if (Auth_Tx.CCF02P_details.primary_acct_nbr[0] != 0)
					{
						bReturnCode = TxDataServer_Get_PAF01_Details() ; 
						/* details about the Primary Account number */
						if (false == bReturnCode) /* XIPC error */
							return SYSTEM_ERROR ;
					}
					else
					{
					   ReturnCode = TxDataServer_Continue_With_CCF02P_Processing(ST2_DB_SELECT_CAF01_PAF01);

					   return ReturnCode ;
					}
					
				break ; /* case CCF02P_DATA */

			case CCF02P_DATA_RT:
				if (PTEMSG_NOT_FOUND == ResultCode)
				{
					txcntl_log_message( 2, 3, "CCF02P is not found for FRT transaction", "txservice_Process_Db_Reply", 1 );
					ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
					if(false == ReturnCode)
					{
					   txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "txservice_Process_Db_Reply", 1 );
					   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
													  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
													   MT_AUTH_REQUEST,
													   0,
													   0) ;
				   }
				   else
				   {
					   return ReturnCode ;
				   }
				}
				else /* PTEMSG_OK */
				{
					memcpy (&sCCF02P, pATEMessageDataData+sizeof(AUTH_TX) ,sizeof(sCCF02P)) ;
					/* Store the CCF02 record in Auth_Tx */
					memcpy (&Auth_Tx.CCF02P_details, &sCCF02P, sizeof(sCCF02P)) ;

					ReturnCode = TxDataServer_Get_PAF01_Details_For_RT();
					if (ReturnCode == true)
				    {
						return NO_AUTH_ERROR;
				    }
					else
					{
					   ReturnCode = TxDataServer_Get_ACF01_Details_For_RT () ;
					   if (ReturnCode == true)
					   {
							return NO_AUTH_ERROR;
					   }
					   else
					   {
						   txcntl_log_message( 2, 3, "Failed to retrieve ACF details for FRT transaction", "txservice_Process_Db_Reply", 1 );
						   ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
						   if(false == ReturnCode)
						   {
							   txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "txservice_Process_Db_Reply", 1 );
							   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
														  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
														   MT_AUTH_REQUEST,
														   0,
														   0) ;
						   }
						   else
						   {
							   return ReturnCode ;
						   }
					   }

					}


				}

				break;/* case CCF02P_DATA_RT */

            case CCF04_DATA:
               if (PTEMSG_OK == ResultCode)  /* Purchase Card category code was found within a valid range for the card */
               {							 /* Next Check the tlf01.total_amount does not exceed ccf02p per transaction limit*/

                   memcpy (&sCCF04, pATEMessageDataData+sizeof(AUTH_TX) , sizeof(sCCF04)) ;
                  /*ReturnCode = TxDataServer_Validate_Category_Code_Range (sCCF04) ;
                  if (ReturnCode != NO_AUTH_ERROR)
                     return ReturnCode ;*/

                  ReturnCode = TxDataServer_Validate_Tran_Limit () ;
                  if (ReturnCode != NO_AUTH_ERROR)
                     return ReturnCode ;


                 /* Retrieve PAF01 details only if the primary acct nbr is present
                     at the CCF02P level for the card */
                  if (Auth_Tx.CCF02P_details.primary_acct_nbr[0] != 0)
                  {
                     bReturnCode = TxDataServer_Get_PAF01_Details() ; 
                     /* details about the Primary Account number */
                     if (false == bReturnCode) /* XIPC error */
                        return SYSTEM_ERROR ;
                  }
                  else
                  {
                     ReturnCode = TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_PAF01) ;
                     return ReturnCode ;
                  }
               }
               else  /*ptemsg not ok*/
               {
                  strcpy( strTemp, "Visa Purchase record not found." );
                  ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                           "Validate_Category_Code_Range",
                                            GS_TXN_PURCHASE_CARD_CHECK_FAILED, 
                                            TRANSACTION_NOT_PERMITTED );
                  return( ReturnCode );
               }
            break ;

#ifdef MC07_OCT2023
            case CCF01_DATA:

                if (PTEMSG_OK == ResultCode)
                {
					memcpy (&sCCF01, pATEMessageDataData+sizeof(AUTH_TX) , sizeof(sCCF01)) ;

					memcpy(CCF01_name,sCCF01.name_dba,strlen(sCCF01.name_dba));

					if(Auth_Tx.MCF01_details.name01[0]!= '\0' &&Auth_Tx.MCF01_details.name02[0]!= '\0'  )
					{
						strncpy(Txnfirst_name,Auth_Tx.MCF01_details.name01,strlen(Auth_Tx.MCF01_details.name01));
						strncat(Txnfirst_name,Auth_Tx.MCF01_details.name02,strlen(Auth_Tx.MCF01_details.name02));
					}
					else if(Auth_Tx.MCF01_details.name01[0]!= '\0')
					{
						strncpy(Txnfirst_name,Auth_Tx.MCF01_details.name01,strlen(Auth_Tx.MCF01_details.name01));
					}

					if(Auth_Tx.MCF01_details.address01[0]!= '\0' &&Auth_Tx.MCF01_details.address02[0]!= '\0'  )
					{
						strncpy(Txnlast_name,Auth_Tx.MCF01_details.address01,strlen(Auth_Tx.MCF01_details.address01));
						strncat(Txnlast_name,Auth_Tx.MCF01_details.address02,strlen(Auth_Tx.MCF01_details.address02));
					}
					else if(Auth_Tx.MCF01_details.address01[0]!= '\0')
					{
						strncpy(Txnlast_name,Auth_Tx.MCF01_details.address01,strlen(Auth_Tx.MCF01_details.address01));
					}

					if(Auth_Tx.MCF01_details.state[0] != '\0')
					{
						strncpy(Txnmiddle_name,Auth_Tx.MCF01_details.state,1);
					}

					if(CCF01_name[0] != '\0')
					{
						if(NULL != strstr(CCF01_name,Txnfirst_name))
						{
							strncpy(Dbfirst_name,CCF01_name,strlen(Txnfirst_name));
							firstName_validation_Result=1;
						}

						if(NULL != (lastnameptr= strstr(CCF01_name,Txnlast_name)))
						{
							strncpy(Dblast_name,lastnameptr,strlen(lastnameptr));
							lastName_validation_Result=1;
						}

						if(strlen(CCF01_name) > strlen(Dbfirst_name) && Dbfirst_name[0] != '\0')
						{
							Dbmiddle_name[0] =  CCF01_name[strlen(Dbfirst_name)+1];
						}


						if(Dbmiddle_name[0]==Txnmiddle_name[0])
						{
							middleName_validation_Result=1;
						}

						if(firstName_validation_Result == 1 && middleName_validation_Result == 1 &&
							  lastName_validation_Result ==1)
						{
							strncpy(Auth_Tx.MCF01_details.status,"A",1);
						}
						else if(firstName_validation_Result == 0 && middleName_validation_Result == 0 &&
							  lastName_validation_Result == 0)
						{
							  strncpy(Auth_Tx.MCF01_details.status,"C",1);
#ifdef BNDL1_APR2024
							  /*Enhancements on AN 7402 Account Status Inquiry to Enable Name Validation*/
							  if(0 == strncmp(no_match_rc,"00",2))
							  {
								  /* configured rc is 00 so continue with BAU
								   * */
								  txcntl_log_message( 2, 1, "No match configured rc is 00 so continue with BAU", "txservice_Process_Db_Reply", 1 );
							  }
							  else
#endif
							  {
							  strcpy(Auth_Tx.TLF01_details.response_code,no_match_rc);
							  ReturnCode = txservice_Generate_Usage_Error_Message (2, "Name validation: No match",
																	"TxDataServer_Process_Db_Reply",
																	GS_TXN_TRANSACTION_NOT_ALLOWED,
																	Auth_Tx.TLF01_details.response_code) ;
							  return( ReturnCode );
						}
						}
						else
						{
							  strncpy(Auth_Tx.MCF01_details.status,"B",1);

							  if(0 == strncmp(partial_match_rc,"00",2))
							  {
								  /* configured rc is 00 so continue with BAU
								   * */
							  }
							  else
							  {
								  strcpy(Auth_Tx.TLF01_details.response_code,partial_match_rc);
								  ReturnCode = txservice_Generate_Usage_Error_Message (2, "Name validation: Partial match",
																					"TxDataServer_Process_Db_Reply",
																					GS_TXN_TRANSACTION_NOT_ALLOWED,
																					Auth_Tx.TLF01_details.response_code) ;
								  return( ReturnCode );
							  }

						}
					}
					else
					{
						strcpy( strTemp, "For ASI transactions CCF01 record found but name is empty,The first Name and Last name was not verifed." );
						txcntl_log_message( 2, 1,strTemp, "txservice_Process_Db_Reply", 1 );
						//Continue with the BAU or decine the transaction
						strncpy(Auth_Tx.MCF01_details.status,"U",1);
					}

					/* Retrieve PAF01 details only if the primary acct nbr is present
					  at the CCF02P level for the card */
					if (Auth_Tx.CCF02P_details.primary_acct_nbr[0] != 0)
					{
					  bReturnCode = TxDataServer_Get_PAF01_Details() ;
					  /* details about the Primary Account number */
					  if (false == bReturnCode) /* XIPC error */
						 return SYSTEM_ERROR ;
					}
					else
					{
					  ReturnCode = TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_PAF01) ;
					  return ReturnCode ;
					}

                }
                else  /*ptemsg not ok*/
                {
					strcpy( strTemp, "For ASI transactions CCF01 record not found,The first Name and Last name was not verifed." );
					txcntl_log_message( 2, 3,strTemp, "txservice_Process_Db_Reply", 1 );
					//Continue with the BAU or decine the transaction
					strncpy(Auth_Tx.MCF01_details.status,"U",1);

					if (Auth_Tx.CCF02P_details.primary_acct_nbr[0] != 0)
					{
					  bReturnCode = TxDataServer_Get_PAF01_Details() ;
					  /* details about the Primary Account number */
					  if (false == bReturnCode) /* XIPC error */
						 return SYSTEM_ERROR ;
					}
					else
					{
					  ReturnCode = TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_PAF01) ;
					  return ReturnCode ;
					}
					return( ReturnCode );
                }
            	break;
#endif


				case PAF01_DATA: /* from TxDataServer_Get_PAF01_Details */

					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						sprintf (strTemp, 
							"PAN %s notfound,PAF01",
							Auth_Tx.CCF02P_details.primary_acct_nbr) ; 

						if (Auth_Tx.TLF01_details.product_codes[9].quantity[0] == 1)
						{
							sprintf(strTemp," PAN %s ,Customer number not found in PAF01", Auth_Tx.CCF02P_details.primary_acct_nbr);
							txcntl_log_message( 2, 3,strTemp, "txservice_Process_Db_Reply", 1 );
							Auth_Tx.TLF01_details.product_codes[9].quantity[0] = 0;
							return SYSTEM_ERROR;
						}
						/******/
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			  "Process_Db_Reply",
																			  GS_TXN_PAF_NOT_FOUND, 
																			  INVALID_ACCOUNT_NUMBER) ;
						// Check if we have converted the Currency code during Local Processing
						if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
							(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
							((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
							 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
						{
						   strncpy( Auth_Tx.TLF01_details.currency_code,
									USDOLLAR_CURRENCY,
									CURRENCY_LEN);
						}
						return ReturnCode ;
						/******/
					}
					else if (PTEMSG_OK == ResultCode)
					{
						memcpy (&sPAF01, pATEMessageDataData+sizeof(AUTH_TX) , 
								  sizeof(sPAF01)) ;
						/* Copy FALCON fields from PAF01
						Falcon-CRTRANS24,customerIdFromHeader,Customer Number TEXT 20 CUST_NBR (12 A/N) 
						Falcon-CRTRANS24,userData02,User defined information TEXT 10 APP_SOURCE 
						Falcon-CRTRANS24,userData05,User defined information TEXT 40 COMP_NAME (30 A/N) */
						/*if (0 != strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "ncvis2", 6) &&
							0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "ncvis3", 6))
						{
						strncpy(Auth_Tx.pan_extended,sPAF01.Custr_nbr,sizeof(sPAF01.Custr_nbr));
						}*/
						strncpy(Auth_Tx.BON01_details.minimum_amount,sPAF01.APP_Source,sizeof(sPAF01.APP_Source));
						strncpy(Auth_Tx.BON01_details.response_message,sPAF01.COMP_Name,sizeof(Auth_Tx.BON01_details.response_message));

						if (Auth_Tx.TLF01_details.product_codes[9].quantity[0] == 1)
						{
							strncpy(Auth_Tx.pan_extended,sPAF01.Custr_nbr,sizeof(sPAF01.Custr_nbr));
							Auth_Tx.TLF01_details.product_codes[9].quantity[0] = 0;

							if ((Auth_Tx.CCF02P_details.status[0] == ' ')||
								(strncmp(Auth_Tx.CCF02P_details.status, "02", 2) == 0) || (strncmp(Auth_Tx.CCF02P_details.status, "01", 2) == 0))
							{
								return SYSTEM_ERROR;
							}
							else
							{
								return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
							}

						}
						strncpy(Auth_Tx.pan_extended,sPAF01.Custr_nbr,sizeof(sPAF01.Custr_nbr));

                  if ( (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE)   &&
                       (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_REFUND) &&
                       (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_CASH_ADVANCE) &&
					   (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_RELOAD)&&
					   (false == txservice_check_for_AMEX_refund_txn()))
                  {/*AMEX02*/
  					    if (!txservice_If_Tx_Cannot_Be_Declined())
                   {
						    /* if (PAF is not 01 or 02), the card is invalid */

							if(true == txservice_check_for_DCI_refund_txn() ||
							   true == txservice_check_for_AMEX_refund_txn()	)
							{
								if(sPAF01.status[0] == 'I' 			 || sPAF01.status[0] == ' '              ||
								   strncmp (sPAF01.status, "02",2) == 0 || strncmp (sPAF01.status, "01", 2) == 0 ||
								   strncmp (sPAF01.status, "03",2) == 0 ||  strncmp (sPAF01.status, "23",2) == 0)
									{
										// Do Nothing
									}
									else
									{
										ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																							  "txservice_perform_standin_processing",
																							  GS_TXN_TRANSACTION_NOT_ALLOWED,
																							  DO_NOT_HONOR) ;
										 return ReturnCode;
									}
							}
							else if ((strcmp (sPAF01.status, "01") != 0) && (strcmp (sPAF01.status, "02") != 0) &&
									 (strcmp (sPAF01.status, "  ") != 0))

						    {
							    sprintf (strTemp, 
										    "Invalid primary acct(PAF01)status:%s\n", 
										    sPAF01.status) ;
                                if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                                    strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
							    txcntl_log_message( 2, 3,Buffer, "txservice_Process_Db_Reply", 1 );
							    bReturnCode = TxDataServer_Get_CSF01_Details (sPAF01.status) ; 
							    /* to retrieve the user entered ISO response code and text */
							    if (false == bReturnCode) /* XIPC error */
								    return SYSTEM_ERROR ;
							    break ;
						    }
                    }
                  }


						/* if the caf_profile_id is present at the PAF02 record */
						if (sPAF01.caf_profile_id[0] != 0 &&
								sPAF01.caf_profile_id[0] != ' '	)
						{
							ReturnCode = TxDataServer_Retrieve_CAF_Profile (sPAF01.caf_profile_id, 
																			Auth_Tx.BIN01_details.primary_key.type,
																			ST2_DB_SELECT_CAF01_PAF01) ;
							return ReturnCode ;
						}
						else 
						{
							ReturnCode = 
								TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_PAF01) ;
							return ReturnCode ;
						}
					}
				break ; /* case PAF01_DATA */

				case PAF01_DATA_RT:
					if(PTEMSG_NOT_FOUND == ResultCode)
					{
						txcntl_log_message( 2, 3, "Failed to retrieve PAF01 details for FRT transaction", "TxDataServer_Process_Db_Reply", 1 );
						ReturnCode = TxDataServer_Get_ACF01_Details_For_RT();
						if (false == ReturnCode)
						{
							ReturnCode = txservice_Send_Request_To_RT_Falcon_Service () ;
							if(false == ReturnCode)
							{
							   txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "TxDataServer_Process_Db_Reply", 1 );
							   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
														  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
														   MT_AUTH_REQUEST,
														   0,
														   0) ;
							}
							else
							{
							   return ReturnCode ;
							}
						}

					}
					else if (PTEMSG_OK == ResultCode)
					{
						memcpy (&sPAF01, pATEMessageDataData+sizeof(AUTH_TX), sizeof(sPAF01)) ;
						strncpy(Auth_Tx.pan_extended,sPAF01.Custr_nbr,sizeof(sPAF01.Custr_nbr));

						ReturnCode = TxDataServer_Get_ACF01_Details_For_RT () ;
						if (ReturnCode == true)
						{
							return NO_AUTH_ERROR;
						}
						else
						{
						   txcntl_log_message( 2, 3, "Failed to retrieve ACF details for FRT transaction", "txservice_Process_Db_Reply", 1 );
						   ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
						   if(false == ReturnCode)
						   {
							   txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "txservice_Process_Db_Reply", 1 );
							   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
														  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
														   MT_AUTH_REQUEST,
														   0,
														   0) ;
						   }
						   else
						   {
							   return ReturnCode ;
						   }
						}
					}

					break;

				case ACF01_DATA:
					
					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						if((Auth_Tx.TLF01_details.tx_key > 100) && 
							((atoi(Auth_Tx.BIN01_details.auth_ind)) == LOCAL_PROCESSING_INDICATOR) &&
							(!strEqual (Auth_Tx.CAF01_details.authorization_location, LOCAL_AUTHORIZATION)))
						{
							// The CCF02P data is requested for cadience balance update requirement. 
							/* TF PHANI */
						   memset(&Buffer, 0, sizeof(Buffer));
						   sprintf( Buffer,"Transaction Approved by HOST,Unable to do Balance_Update: The CCF02P record does not exists for the transaction id: %s transaction amount: %s, STAN: %s,",
    											 Auth_Tx.TLF01_details.primary_key.transaction_id,
    											Auth_Tx.TLF01_details.total_amount, Auth_Tx.TLF01_details.sys_trace_audit_num);
						   txcntl_log_message( 2, 3,Buffer, "Process_Db_Reply", 1 );
    					   txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue,
    																						 MT_AUTH_RESPONSE, 
    																						 0, 
    																						 0);							
							
							return RECORD_FOUND_IN_SHARED_MEMORY;
						}//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						else
						{
							
							//TF Praneeth New logic for checking other account
							if(atoi(Auth_Tx.CCF02P_details.pin_verification_override) == 0 )
							{
								strcpy(Auth_Tx.CCF02P_details.pin_verification_override,"1");
								return TxDataServer_Get_OtherAccount();	
							}
							else
							{
								
							/******/
								sprintf (strTemp,
										"Balance in ACF01 notfound for type %s",
										Auth_Tx.BIN01_details.primary_key.type) ;
								ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																					  "Process_Db_Reply",
																					  GS_TXN_ACF01_NOT_FOUND,
																					  INVALID_ACCOUNT_NUMBER) ;
								// Check if we have converted the Currency code during Local Processing
								if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
									(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
									((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
									 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
								{
								   strncpy( Auth_Tx.TLF01_details.currency_code,
											USDOLLAR_CURRENCY,
											CURRENCY_LEN);
								}
								return ReturnCode ;
							/******/
							}
						}
						
					}
					if (PTEMSG_OK == ResultCode) /* PTEMSG_OK */
					{
						memcpy (&sACF01, pATEMessageDataData+sizeof(AUTH_TX),sizeof(sACF01)) ;
						
						/* Store the ACF01 record in Auth_Tx */
						memcpy (&Auth_Tx.ACF01_details, &sACF01, sizeof(sACF01)) ;

						//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						if((Auth_Tx.TLF01_details.tx_key > 100) && 
							((atoi(Auth_Tx.BIN01_details.auth_ind)) == LOCAL_PROCESSING_INDICATOR) &&
							(!strEqual (Auth_Tx.CAF01_details.authorization_location, LOCAL_AUTHORIZATION)))
						{
							// The CCF02P data is requested for cadience balance update requirement. 
							//if the data has been obtained, continue with balance_update
								ReturnCode= txserviceCadience_Continue_With_ACF01_For_Balance_Update();
								if(ReturnCode == NO_AUTH_ERROR)
									return RECORD_FOUND_IN_SHARED_MEMORY;
								else
									return ReturnCode ;
								//return RECORD_FOUND_IN_SHARED_MEMORY;
						}//Ravikumar Nageshaiah - ThoughtFocus for rel 08-16
						
	                  if ( (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE)   &&
	                       (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_REFUND) &&
	                       (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_CASH_ADVANCE) &&
						   (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_RELOAD) &&
						   (false == txservice_check_for_AMEX_refund_txn()))
		                  {/*AMEX02*/
		                     if (!txservice_If_Tx_Cannot_Be_Declined())
		                     {
						            /* if (status is not 01 or 02), the card is invalid */

		                        /* if ((strcmp (sACF01.status, "01")) && (strcmp (sACF01.status, "02"))) */
		                        if (!strEqual (sACF01.status, "00"))
		                        {
									//TF Praneeth New logic for checking other account
									if(atoi(Auth_Tx.CCF02P_details.pin_verification_override) == 0 )
									{
										strcpy(Auth_Tx.CCF02P_details.pin_verification_override,"1");
										if(strEqual (sACF01.status, "05"))
										{
											return TxDataServer_Get_OtherAccount();	
											break;
										}
									}
									else
									{
										if ( strEqual(Auth_Tx.TLF01_details.host_finish_time, USD_COMP) )
										{
											strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
										}
										else if ( strEqual(Auth_Tx.TLF01_details.host_finish_time, PESO_COMP) )
										{
											strcpy( Auth_Tx.TLF01_details.currency_code,   PESO_CURRENCY );
										}
										strcpy( Auth_Tx.TLF01_details.host_start_time,"" );
										strcpy( Auth_Tx.TLF01_details.host_finish_time,"");
										strcpy(Auth_Tx.CCF02P_details.pin_verification_override,"");										
		                           sprintf (strTemp, "Invalid balance acct(ACF01)status:%s\n", 
												    sACF01.status) ;
		                           if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
		                                    strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
		                           txcntl_log_message( 2, 3,Buffer, "txservice_Process_Db_Reply", 1 );
		                           bReturnCode = TxDataServer_Get_CSF01_Details (sACF01.status) ; 

		                           /* to retrieve the user entered ISO response code and text */
		                          if (false == bReturnCode) /* XIPC error */
										{
											txcntl_log_message( 1, 3,"xipc error", "txservice_Process_Db_Reply", 1 );
										    return SYSTEM_ERROR ;
										}
										break ;
									}
									      break ;
		                        }
		                     }
		                  }
						

                  /* special case - this is because TLF01 contains only <outstanding_balance> */
                  strcpy (Auth_Tx.TLF01_details.outstanding_balance, Auth_Tx.ACF01_details.available) ;
                  /* end special case */

                  strcpy (Auth_Tx.TLF01_details.credit_limit, Auth_Tx.ACF01_details.credit_limit) ;

					}

					if (MessageSubType2 == BALANCE_ACCOUNT_TYPE_IS_DEFERRED)
					{
						/* Account type is deferred, get deferred details */
						bReturnCode = TxDataServer_Get_DEF01_Details () ;
						if (bReturnCode == false)
							return SYSTEM_ERROR ;
						break ;
					}

					ReturnCode = TxDataServer_Continue_With_ACF01_Processing (MessageSubType2) ;
					if (ReturnCode != NO_AUTH_ERROR)
						return ReturnCode ;

				break ; /* ACF01 */

				case ACF01_DATA_RT:

				if (PTEMSG_NOT_FOUND == ResultCode)
				{
					txcntl_log_message( 2, 3, "Failed to retrieve ACF01 details for FRT transaction", "txservice_Process_Db_Reply", 1 );
					ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
					if(false == ReturnCode)
					{
						txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "txservice_Process_Db_Reply", 1 );
						ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
												  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
												   MT_AUTH_REQUEST,
												   0,
												   0) ;
					}
					else
					{
						return ReturnCode ;
					}
				}
				if (PTEMSG_OK == ResultCode) /* PTEMSG_OK */
				{
					memcpy (&sACF01, pATEMessageDataData+sizeof(AUTH_TX),sizeof(sACF01)) ;

					/* Store the ACF01 record in Auth_Tx */
					memcpy (&Auth_Tx.ACF01_details, &sACF01, sizeof(sACF01)) ;
					ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
					if(false == ReturnCode)
					{
						txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "txservice_Process_Db_Reply", 1 );
						ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
												  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
												   MT_AUTH_REQUEST,
												   0,
												   0) ;
					}
					else
					{
						return ReturnCode ;
					}
				}


				break ; /* ACF01_DATA_RT */
				case DEF01_DATA:
															
					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						sprintf (strTemp, 
							"Defered rec notfound,Mid%s\n", 
							Auth_Tx.TLF01_details.merchant_id) ;

						/******/
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			  "Process_Db_Reply",
																			  GS_TXN_DEF01_NOT_FOUND, 
																			  REFER_TO_CARD_ISSUER) ;
						return ReturnCode ;
						/******/

					}
					else /* PTEMSG_OK */
					{
						memcpy (&sDEF01, pATEMessageDataData+sizeof(AUTH_TX),sizeof(sDEF01)) ;
						strcpy (Auth_Tx.TLF01_details.deferred_factor, sDEF01.factor) ;

						if (strlen(sDEF01.factor) == 0)
						{
							sprintf (strTemp, "Invalid def factor Mid %s\n",
								    Auth_Tx.TLF01_details.merchant_id) ;
							
							/******/
							ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																				"Process_Db_Reply",
																				GS_TXN_DEF01_NOT_FOUND, 
																				REFER_TO_CARD_ISSUER) ;
							return ReturnCode ;
							/******/
							
						}
                          /* Validate deferred amounts only if the transaction is a DEFERRED_PURCHASE_AUTHORIZATION */
                          /* Not a REVERSAL or a OVERRIDE of a DEFERRED purchase */
                          if ((!txservice_If_Tx_Is_Treated_Like_A_Reversal ()) &&
                             (!txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override ()))
                          {
						           ReturnCode = TxAuthorize_Validate_Deferred_Amounts (sDEF01.deferred_min_amount,
																					   sDEF01.deferred_max_amount) ;
						           if (ReturnCode != NO_AUTH_ERROR)
							           return ReturnCode ;
                          }
						
						ReturnCode = TxDataServer_Continue_With_ACF01_Processing(BALANCE_ACCOUNT_TYPE_IS_DEFERRED);
						if (ReturnCode != NO_AUTH_ERROR)
							return ReturnCode ;
					}
				break ;
				case CRF01_DATA:

					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						sprintf (strTemp, 
									"Currency code %s notfound in CRF01", 
									Auth_Tx.MCF01_details.currency_code) ; 
						/******/
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			  "Process_Db_Reply",
																			  GS_TXN_CRF_NOT_FOUND, 
																			  REFER_TO_CARD_ISSUER) ;
						// Check if we have converted the Currency code during Local Processing
						if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
							(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
							((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
							 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
						{
						   strncpy( Auth_Tx.TLF01_details.currency_code,
									USDOLLAR_CURRENCY,
									CURRENCY_LEN);
						}
						return ReturnCode ;
						/******/
					}
					
					else /* PTEMSG_OK */
					{
						memcpy (&sCRF01, pATEMessageDataData+sizeof (AUTH_TX), sizeof(sCRF01)) ;
						dCurrency_Conversion_Rate = strtod (sCRF01.conversion_rate, 0) ;
						if (dCurrency_Conversion_Rate == 0)
							dCurrency_Conversion_Rate = 1.0 ;
						dCurrency_Conversion_Rate = 1 / dCurrency_Conversion_Rate ;
						
						gcvt (dCurrency_Conversion_Rate, 7, Auth_Tx.TLF01_details.conversion_rate) ;
						
						bReturnCode = TxDataServer_Get_CCF03_Details() ; 
						/* details about the CCF03 table */
						if (false == bReturnCode) /* XIPC Error */
							return SYSTEM_ERROR ;
					}
				break ; /* CRF01_DATA */
				
            case CSF01_DATA:

               if (PTEMSG_NOT_FOUND == ResultCode)
               {
                  strcpy (strTemp, "CSF01 record not found ");
                  ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                          "Process_Db_Reply",
                                           GS_TXN_INVALID_CCF02P_STATUS,
                                           REFER_TO_CARD_ISSUER);
                  // Check if we have converted the Currency code during Local Processing
				   if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
					    (strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
					    ((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
					     (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
					 {
					   strncpy(Auth_Tx.TLF01_details.currency_code,
							   USDOLLAR_CURRENCY,
							   CURRENCY_LEN);
					 }
                  return ReturnCode ;
               }

               else /* PTEMSG_OK */
               {
                  /* Copy response info from DB into data structure. */
                  memcpy( &sCSF01,
                          pATEMessageDataData+sizeof(AUTH_TX),
                          sizeof(sCSF01) );

                  /* Indicate where response text came from. */
                  Auth_Tx.FAF01_details.velocity[9].to_time[0] = '1';

                  /* Copy response code and response text into auth_tx. */
                  if (sCSF01.iso_resp_code[0] != NULL_PTR)
				  {
						  /*
						   * MC03_OCT2023
						   * */
						 /* Response text field might already be populate
						  * if txn is an RPS transaction. Do not overwrite.
						  */
						 if ( Auth_Tx.TLF01_details.response_text[0] == 0x00 )
						 {
							/* Response text is not populated. Use info from DB. */
							if ( sCSF01.resp_text_msg[0] != NULL_PTR )
							{
							   strcpy( Auth_Tx.TLF01_details.response_text,
									   sCSF01.resp_text_msg);
								if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
								strcpy (Auth_Tx.TLF01_details.voice_auth_text,  Auth_Tx.TLF01_details.response_text) ;
							}
							else
							{
							   strcpy( Auth_Tx.TLF01_details.response_text,
									  "NO ERROR TEXT" );
							}

						 }

						 if( 'R' == Auth_Tx.TLF01_details.product_codes[6].quantity[0])
						 {
							 /* April Mandt changes 2023: ENH1
								send response code for card status other than active*/
							 if (0 != strncmp(sCSF01.primary_key.status, "02", 2) &&
								 0 != strncmp(sCSF01.primary_key.status, "01", 2) &&
								 0 != strncmp(sCSF01.primary_key.status, " ", 1))
							 {
								 txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CCF02P_STATUS,
																 sCSF01.iso_resp_code);
							 }
							 else
							 {
							 txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CCF02P_STATUS,
									 DO_NOT_HONOR);
							 }

						 }
						 else
						 {
							 /*check response code and authid,
							  * generate authid for other card status*/
							if ((0 == strncmp(sCSF01.iso_resp_code, "00", 2) &&
								0 ==  strlen ( Auth_Tx.TLF01_details.auth_number )))
							{
								(void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
							}

							txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CCF02P_STATUS,
																  sCSF01.iso_resp_code);
						 }

#ifdef MC03_OCT2023
						 /* to check wether CSF is read for CCf02p only, added a indicator in Auth_Tx.current_balance_sign
						  * if its 1, CSF01 data is read for CCF02P
						  * * */

						if( Auth_Tx.current_balance_sign =='1' &&
						    Auth_Tx.TLF01_details.product_codes[16].amount[4] == CARD_NOT_PRESENT_MC_DE61_SE5 )
						{
							/*validation for mac value presence, space , length, alpha numeric */
							if( sCSF01.mac_value[0] != '\0' && 2 == strlen(sCSF01.mac_value) &&
								0 != strncmp(sCSF01.mac_value,"  ",2) &&( 0 != isalnum(sCSF01.mac_value[0]) && 0 != isalnum(sCSF01.mac_value[1]) ))
							{
								strncat(Auth_Tx.TLF01_details.response_text," DE48 SE84:",11);
								strncat( Auth_Tx.TLF01_details.response_text,sCSF01.mac_value,2 );
							}
							Auth_Tx.current_balance_sign = '\0';
						}

#endif
						 if (Auth_Tx.pan_extended[0] == '\0')
						 {
							 Auth_Tx.TLF01_details.product_codes[9].quantity[0] = 1;
							 ReturnCode = TxDataServer_Get_PAF01_Details();
							 return ERROR_BUT_DO_NOT_FORWARD_TO_UPDATE_PROCESSOR;
						 }
				  }
                  else
                  {
                     txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CCF02P_STATUS,
                                                              INVALID_TRANSACTION);
                  }


                  return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
               }
				break ; /* CSF01_DATA */

				case CCF03P_DATA:
															
					if (PTEMSG_NOT_FOUND == ResultCode)
					{
						/* if the record is not found, create it  ----------------
						memset (strTemp, 0, sizeof (strTemp)) ;
						sprintf (strTemp, 
							"Velocity (CCF03P) record not found for type %s and card %s Creating new record\n",
							Auth_Tx.BIN01_details.primary_key.type, 
							Auth_Tx.TLF01_details.card_num) ;

                  -----------------------------------------------------------*/

						strcpy (sCCF03P.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
						strcpy (sCCF03P.primary_key.card_nbr, Auth_Tx.TLF01_details.card_num) ;
						strcpy (sCCF03P.status, Auth_Tx.CCF02P_details.status) ;
						strcpy (sCCF03P.primary_acct_nbr, Auth_Tx.CCF02P_details.primary_acct_nbr) ;
						strcpy (sCCF03P.failed_pin_attempts, "0") ;
						strcpy (sCCF03P.daily_amt_usage, "0") ;
						Auth_Tx.ccf03_insert = 1 ;

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
					}
					
					else /* PTEMSG_OK */
					{
						memcpy (&sCCF03P, pATEMessageDataData+sizeof(AUTH_TX), sizeof(sCCF03P)) ;
					    
						Auth_Tx.ccf03_insert = 0 ;
					}

					/* Store the CCF03 record in Auth_Tx */
					memcpy (&Auth_Tx.CCF03P_details, &sCCF03P, sizeof sCCF03P) ;

                    /* Clear the card's failed_pin_attempts field to 0 on a new transaction day */
                    /* This will also happen if the card is being used for the very first time */
                    if (!TxAuthorize_If_Transaction_Occurred_On_Same_Day ())
                    {
                        strcpy (Auth_Tx.CCF03P_details.failed_pin_attempts, "0") ;


                    }

                    /* Populate the private_ccf03p_structure starting with memsetting it to 0 */
                    TxAuthorize_Update_Private_Ccf03p_Structure () ;
					
                    if(txservice_If_Tx_qualify_for_EMV_validation() ==  false)
                    {
                    	/*For all advice , reverssal and void transaction no need for PIN validation.
                    	 * Approve all advice txn at this point.
                    	 *
                    	 */
                        /* Proceed to authorize the ONUS transaction. */
                        ReturnCode = TxDataServer_Continue_With_Authorizing_Transaction();
                        return ReturnCode ;
                    }
					/* determine if txn requires PIN validation */

					if (txservice_If_Tx_Requires_Pin_Validation())
					{
			            if (strlen(Auth_Tx.TLF01_details.pin_block) > 0 ) /* pin is present */
						{
				         	if (!txservice_If_Tx_Cannot_Be_Declined())
							{
							    ReturnCode = TxAuthorize_Process_Security (PIN_VERIFICATION) ;
							    /* ReturnCode will be NO_AUTH_ERROR when successfully 
							    sent to the crypto processor
							    else will be SYSTEM_ERROR or ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR */ 
							    return ReturnCode ;
							}
						}
					}
					
					if(TxAuthorize_Check_TxnType_is_ChipFallBack()==true &&
							(0 != strncmp(Auth_Tx.TLF01_details.product_codes[2].quantity,"1",1)) &&
							(0 != strncmp(Auth_Tx.TLF01_details.card_num, "7",1)))
					{
						ReturnCode=TxDataServer_Get_CR01_Details();
						return ReturnCode ;
					}

					ReturnCode = TxDataServer_Continue_With_Pin_Processing () ;
					return ReturnCode ;
					
				break ;	/* case CCF03P_DATA */

				case CR01_DATA:

							if (PTEMSG_NOT_FOUND == ResultCode)
							{
								//Continue from the CCF03P flow.
								ReturnCode = TxDataServer_Continue_With_Pin_Processing () ;
								return ReturnCode ;

							}
							else
							{
								memcpy (&sCR01, pATEMessageDataData+sizeof (AUTH_TX), sizeof(sCR01)) ;
								exception_cardnum_flag= TxDataServer_Check_Exception_Card_Num(sCR01);

								if(exception_cardnum_flag== NOT_BLOCKED || exception_cardnum_flag== EMPTY)

								{
									if((strncmp (sCR01.status, "A",1) ==0))
									{
										/* If the value is 0 in this field, then decline it outright.
										* The defined valued in this field, if is smaller than the total amount(DE4), then decline.
										*/
										if (0==strncmp(sCR01.single_amtlimit,"000000000000",12))
										{
													sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
													ReturnCode = txservice_Generate_Usage_Error_Message(2,
																		strTemp, "Process_Db_Reply",
																		GS_TXN_TRANSACTION_NOT_ALLOWED,
																		sCR01.response_code);
													return ReturnCode;

										}
										else if (0!=strncmp(sCR01.single_amtlimit,"000000000000",12) && sCR01.single_amtlimit[0] != '\0')
										{
											ReturnCode = TxDataServer_Perform_Single_amount_limit(sCR01);
											if (ReturnCode != NO_AUTH_ERROR)
											{
												return ReturnCode;
											}
										}
										else
										{
												//no operation
										}



										/*The date is added here because, on the new day, the counter will reset and for that one day, the transactions
										are validated*/
										if(Auth_Tx.TLF01_details.system_date[0] == '\0')
										{
											txservice_create_system_date( Auth_Tx.TLF01_details.system_date );
										}
										if (0==strlen(sCR01.future_use_2))
										{

											strncpy(sCR01.future_use_2,Auth_Tx.TLF01_details.system_date,8);
											strncpy (sCR01.acc_amtlimit_counter, "000000000000",12) ;
											strncpy (sCR01.acc_transactionlimit_counter, "000000000000",12) ;


										}
										else if(0!=strncmp(sCR01.future_use_2,Auth_Tx.TLF01_details.system_date,8))
										{

											strncpy(sCR01.future_use_2,Auth_Tx.TLF01_details.system_date,8);
											strncpy (sCR01.acc_amtlimit_counter, "000000000000",12) ;
											strncpy (sCR01.acc_transactionlimit_counter, "000000000000",12) ;

										}

										CF_nTotalAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

										if (!strEqual (Auth_Tx.TLF01_details.conversion_rate, "1"))
										{
												nConversionRate = strtod (Auth_Tx.TLF01_details.conversion_rate, 0) ;
												nConvertedTxAmount = CF_nTotalAmount * nConversionRate ;
										}
										else
												nConvertedTxAmount = CF_nTotalAmount ;

										ReturnCode = TxAuthorize_Perform_Card_Velocity_Checks(nConvertedTxAmount, sCR01);
										if(ReturnCode!=NO_AUTH_ERROR)
										{
												return ReturnCode ;
										}

										ReturnCode = TxDataServer_Continue_With_Pin_Processing () ;
										return ReturnCode ;


									}
									else
									{
										ReturnCode = TxDataServer_Continue_With_Pin_Processing () ;
										return ReturnCode ;
									}


							}
							else
							{
									ReturnCode = TxDataServer_Continue_With_Pin_Processing () ;
									return ReturnCode ;

							}
						}
							break;
			case  OMC01_DATA:
				
				if (PTEMSG_NOT_FOUND == ResultCode)
				{
				  /* strcpy (strTemp, "Merchant Ovrride  record not found\n");
				   ReturnCode = txservice_Generate_Usage_Error_Message (1,Auth_Tx.TLF01_details.response_text,
										   "Process_Db_Reply",
											GS_TXN_CATEGORY_USAGE_CHECK_FAILED,
											REFER_TO_CARD_ISSUER); */
					// Check if we have converted the Currency code during Local Processing
					if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
						(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
						((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
						 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
					{
					   strncpy( Auth_Tx.TLF01_details.currency_code,
								USDOLLAR_CURRENCY,
								CURRENCY_LEN);
					}
				   return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
				}else
				{
					memcpy( &sOMC01,
                    	      pATEMessageDataData+sizeof(AUTH_TX),
                        	  sizeof(sOMC01) );

					/*
					BYTE			Feild Name					Flag Value		Meaning
					BYPASS[0]		No Expiry							1/0			Enable/Not eanle
					BYPASS[1]		Global Parameter capping			1/0			Enable/Not eanle
					BYPASS[2]		MCC								1/0			Enable/Not eanle
					BYPASS[3]		MID								1/0			Enable/Not eanle
					// For more information  refer BR SSD-143-13 mcc ATP Override Merchant Denial Enhancements - v1.1
					*/
					
					//Check if No expiry option is enable or not TF-- Abhishek
					// For more information  refer BR SSD-143-13 mcc ATP Override Merchant Denial Enhancements - v1.1
					if('0' == sOMC01.bypass[0])
					{
						// No expiry option is not enable, Comapre system date with expiry date
						ptetime_get_timestamp(time_date);
						get_date(time_date,date_str);
						if(atoi(date_str) > atoi(sOMC01.primary_key.expiry_date))
						{
							//System date is greater than expiry date return 
							sprintf( strTemp, " system date (%s)  is more than expire date(%s) ",date_str,sOMC01.primary_key.expiry_date);
							// Clearing voice authorisation test as error is overriden 
							strcpy(Auth_Tx.TLF01_details.voice_auth_text,"");
							strcpy(Auth_Tx.TLF01_details.voice_auth_text,strTemp);
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}
						else
						{
							//System date is not greater than expiry date return 
							// Check the date of exp date
							//praneeth code
							if(atoi(Auth_Tx.TLF01_details.date_yyyymmdd) > atoi(sOMC01.primary_key.expiry_date))
							{
								return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
							}
							//end of praneeth code
						}
					}

					//If card number is present validate it TF--Abhishek
					// For more information  refer BR SSD-143-13 mcc ATP Override Merchant Denial Enhancements - v1.1
					if (strlen(sOMC01.primary_key.card_nbr)  != 0)
					{
						if (!FieldIsNumeric (sOMC01.primary_key.card_nbr))
						{
							strcpy (strTemp, "Invalid non numeric card number for OMD transaction") ;
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}
					
						if (strlen (sOMC01.primary_key.card_nbr)  > 19)
						{
							strcpy (strTemp, "Invalid card number length of the card is more than 19 for OMD transaction") ;
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}

						if( !ValidateCardNumber())
						{
							sprintf (strTemp, "Invalid card number as Card number (%s) failed Mod 10 check for OMD transaction",
	                  						Auth_Tx.TLF01_details.card_num) ;
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}
						
						if(0 != strcmp( Auth_Tx.TLF01_details.card_num , sOMC01.primary_key.card_nbr))
						{
							strcpy (strTemp, "Invalid card number for OMD transaction") ;
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}
					}

								
					// check the no of allowances
					//praneeth code				
					if((atoi(sOMC01.primary_key.repeated))+1 > atoi(sOMC01.primary_key.max_repeat))
					{
						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
					}
					if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) )
					{
						/* When a user enters the OMC PESO amount limit/USD amount limit as "9999999999"(10 digits)
						 * It exceeds the limit that the int data type can store i.e.,-2,147,483,647 to 2,147,483,647.
						 * Hence convert the str into long with strtol.
						 */
						int_peso_amt_limit = strtol(sOMC01.primary_key.peso_amt_limit, &peso_amt_limit, 10);
						if(atoi(Auth_Tx.TLF01_details.total_amount) > (int_peso_amt_limit*100))
						{
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}
					}
					else  if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY) )
					{
						int_usd_amt_limit = strtol(sOMC01.primary_key.usd_amt_limit, &usd_amt_limit, 10);
						if(atoi(Auth_Tx.TLF01_details.total_amount) > (int_usd_amt_limit*100))
						{
							return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
						}
					}
					else
					{
						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
					}

					// clear nbr_of_prod_codes 
					strcpy(Auth_Tx.TLF01_details.nbr_of_prod_codes,"");
					
					// clear OMD flag 
					strcpy(Auth_Tx.TLF01_details.product_codes[18].code,"");
					// clear OMD category flag - new changes Amith k
					strcpy(Auth_Tx.TLF01_details.vehicle_number,"");
					
					//sprintf(Auth_Tx.TLF01_details.nbr_of_prod_codes,"%d",atoi(sOMC01.primary_key.repeated)+1);
					//Auth_Tx.TLF01_details.update_mask[0] = OMC01_MASK ;

					/*tlf01.nbr_of_prod_codes 2bytes field is used to update the repeated counts in OMC01
					  if repeated > 99 updatds throws error while inserting into TLF01 table
					  New Changes:- Restrict the repeated count if repeated is 99
					  if threshold limit is set beyond 99 - OMC01 repeated count will never update after 99
					  and txn will not reach threshold limit and keeps approving the txns without any limit.
					  This constraint needs to be addressed in Transact.
					*/
					if(atoi(sOMC01.primary_key.repeated) == 99)
					{
						memcpy(sOMC01.primary_key.repeated, "98", 2);
						txcntl_log_message(3, 2, "OMD repeated count reached 99, ATP will restrict to update the count beyond 99,", "TxDataServer_Process_Db_Reply", 1);
					}

					/*
						GS_TXN_EXCEEDED_MAX_TRANSACTION_LIMIT_FOR_MERCHANT --Max_Credit_Amt
						GS_TXN_EXCEEDED_MAX_REFUND_LIMIT_FOR_MERCHANT -- Max_CashAdvance_Amt & Max_Return_Amt  
					*/
					// Check MID Capping Bypass flag enable or not?
					if('1' == sOMC01.bypass[3])
					{
						// MID Capping Bypass flag enable
						// Check Genral status to see if its MID check 
						if( Auth_Tx.TLF01_details.general_status == GS_TXN_EXCEEDED_MAX_TRANSACTION_LIMIT_FOR_MERCHANT ||
							Auth_Tx.TLF01_details.general_status == GS_TXN_EXCEEDED_MAX_REFUND_LIMIT_FOR_MERCHANT) 
						{

							// Its MID check 
							// Clearing voice authorisation test as error is overriden 
							strcpy(Auth_Tx.TLF01_details.voice_auth_text,"");

							sprintf(Auth_Tx.TLF01_details.nbr_of_prod_codes,"%d",atoi(sOMC01.primary_key.repeated)+1);
							// Set OMD flag 
							strcpy(Auth_Tx.TLF01_details.product_codes[18].code,"OMD");
							// Set OMD category flag
							strcpy(Auth_Tx.TLF01_details.vehicle_number,"MID_BYPS");
							if ( false == TxDataServer_Get_MCT01_Details() )
							{
                  				ReturnCode = SYSTEM_ERROR ;
							}
							else
							{
								
								return NO_AUTH_ERROR;
							}
						}
					}
		
					/*	GS_TXN_VELOCITY_NBR_USAGE_CHECK_FAILED -- nbr_usage velocity
						GS_TXN_VELOCITY_AMT_USAGE_CHECK_FAILED -- amt_usage velocity

					*/

					// Check Global Parameter Capping Bypass flag enable or not?
					if('1' == sOMC01.bypass[1])
					{
						// Global Parameter Capping Bypass flag enable
						//Check Genral status to see if its Global Parameter Capping check 
						if (Auth_Tx.TLF01_details.general_status == GS_TXN_VELOCITY_NBR_USAGE_CHECK_FAILED||
							Auth_Tx.TLF01_details.general_status == GS_TXN_VELOCITY_AMT_USAGE_CHECK_FAILED)
						{
							//Its Global Parameter Capping check
							sprintf(Auth_Tx.TLF01_details.nbr_of_prod_codes,"%d",atoi(sOMC01.primary_key.repeated)+1);
							// Set OMD flag 
							strcpy(Auth_Tx.TLF01_details.product_codes[18].code,"OMD");
							// Set OMD category flag
							if (Auth_Tx.TLF01_details.general_status == GS_TXN_VELOCITY_NBR_USAGE_CHECK_FAILED)
								strcpy(Auth_Tx.TLF01_details.vehicle_number,"NBR_BYPS");
							else if(Auth_Tx.TLF01_details.general_status == GS_TXN_VELOCITY_AMT_USAGE_CHECK_FAILED)
								strcpy(Auth_Tx.TLF01_details.vehicle_number,"AMT_BYPS");
							ReturnCode=txservice_Perform_OMD_GlobalParamterBypass(MessageSubType2);
							if(NO_AUTH_ERROR == ReturnCode)
							{
								
								return NO_AUTH_ERROR;
							}
							else
							{
								return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
							}
					
						}
					}
													
					
					/*
						GS_TXN_CATEGORY_USAGE_CHECK_FAILED MCC category check
					*/
					// Check MCC Capping Bypass flag enable or not?
					if('1' == sOMC01.bypass[2])
					{
						//MCC Capping Bypass flag enable
						// Check Genral status to see if its MCC Capping check
						if (Auth_Tx.TLF01_details.general_status == GS_TXN_CATEGORY_USAGE_CHECK_FAILED)
						{
							sprintf(Auth_Tx.TLF01_details.nbr_of_prod_codes,"%d",atoi(sOMC01.primary_key.repeated)+1);
							// Set OMD flag 
							strcpy(Auth_Tx.TLF01_details.product_codes[18].code,"OMD");
							// Set OMD category flag
							strcpy(Auth_Tx.TLF01_details.vehicle_number,"MCC_BYPS");
							////Its MCC Capping check
							ReturnCode=txservice_Perform_OMD_MCCBypass();
							if(NO_AUTH_ERROR == ReturnCode)
							{
								
								return NO_AUTH_ERROR;
							}
							else
							{
								return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
							}
						}
					}
					// We came here means NO Bypass falg is enable  
					return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
				}
				
				break ;	
            case OFFUS_VELOCITY_DATA:

               if (PTEMSG_NOT_FOUND == ResultCode)
               {
                  /* if the record is not found, create it --------------------
                  memset (strTemp, 0, sizeof (strTemp)) ;
                  sprintf (strTemp, 
                    "Velocity (OFFUS_VELOCITY) record not found for type %s and card %s Creating new record\n",
                     Auth_Tx.BIN01_details.primary_key.type, 
                     Auth_Tx.TLF01_details.card_num) ;

                  ---------------------------------------------------------------*/

                  strcpy (sOffus_velocity.primary_key.card_type, Auth_Tx.BIN01_details.primary_key.type) ;
                  strcpy (sOffus_velocity.primary_key.card_nbr, Auth_Tx.TLF01_details.card_num) ;
                  strcpy (sOffus_velocity.status, Auth_Tx.CCF02P_details.status) ;
                  strcpy (sOffus_velocity.primary_acct_nbr, Auth_Tx.CCF02P_details.primary_acct_nbr) ;
                  strcpy (sOffus_velocity.failed_pin_attempts, "0") ;
                  strcpy (sOffus_velocity.daily_amt_usage, "0") ;
                  Auth_Tx.offus_velocity_insert = 1 ;
				  Auth_Tx.TLF01_details.update_mask[0] |= OFFUS_VELOCITY_MASK ;	
                  for (nIndex = 0 ; nIndex <= 13; nIndex++)
                  {
                     strcpy (sOffus_velocity.peso_usage[nIndex].nbr_usage, "0") ;
                     strcpy (sOffus_velocity.peso_usage[nIndex].amt_usage, "0") ;

                     strcpy (sOffus_velocity.dollar_usage[nIndex].nbr_usage, "0") ;
                     strcpy (sOffus_velocity.dollar_usage[nIndex].amt_usage, "0") ;
                  }

                  for (nIndex = 0 ; nIndex <= 19; nIndex++)
                  {
                     strcpy (sOffus_velocity.repeat[nIndex].merchant_id, "0") ;
                     strcpy (sOffus_velocity.repeat[nIndex].nbr_uses, "0") ;
                  }

                  for (nIndex = 0 ; nIndex <= 19; nIndex++)
                  {
                     strcpy (sOffus_velocity.mcc[nIndex].category_code, "0") ;
                     strcpy (sOffus_velocity.mcc[nIndex].usage, "0") ;
                  }

                  genutil_format_date (strSystemDateTime) ;
                  memset (strTemp, 0, sizeof strTemp) ;

                  memcpy (strTemp, strSystemDateTime, 8) ; /* date in yyyymmdd format */
                  strcpy (sOffus_velocity.date_added, strTemp) ;
                  nTranDateInJulian = Txutils_Calculate_Julian_Date (strTemp) ;
                  ltoa (nTranDateInJulian, strTranDateInJulian, 10) ;
                  strcpy (sOffus_velocity.peso_julian_date, strTranDateInJulian) ;
                  strcpy (sOffus_velocity.dollar_julian_date, strTranDateInJulian) ;
               }

               else /* PTEMSG_OK */
               {
                  memcpy (&sOffus_velocity, pATEMessageDataData+sizeof(AUTH_TX), sizeof(sOffus_velocity)) ;
                  Auth_Tx.offus_velocity_insert = 0 ;
               }

               /* Store the CCF03 record in Auth_Tx */
               memcpy (&Auth_Tx.offus_velocity_details, &sOffus_velocity, sizeof sOffus_velocity) ;

               /* Clear the card's failed_pin_attempts field to 0 on a new transaction day */
               /* This will also happen if the card is being used for the very first time */
               if (!TxAuthorize_If_Transaction_Occurred_On_Same_Day ())
               {
                  strcpy (Auth_Tx.offus_velocity_details.failed_pin_attempts, "0") ;
               }

               /* Populate the private_ccf03p_structure starting with memsetting it to 0 */
               TxAuthorize_Update_Private_Offus_Velocity_Structure () ;

               /* Get the MCC Limits, then perform Off-Us velocity checks. */
               ReturnCode = TxDataServer_Get_MCC_Limit_Details( OFFUS );
               return ReturnCode ;

				break ;	/* case OFFUS_VELOCITY_DATA */


            case MCC_LIMITS_DATA:

               memset( &Mcc_Limits, 0x00, sizeof(MCC_LIMITS) );
               if (PTEMSG_OK == ResultCode)
               {
                  memcpy( &Mcc_Limits,
                          pATEMessageDataData+sizeof(AUTH_TX),
                          sizeof(MCC_LIMITS) );
               }

               /* Repopulate the private ccf03p structure. */
               TxAuthorize_Update_Private_Ccf03p_Structure () ;

               if ( MessageSubType2 == OFFUS )
               {
                  /* Perform Off-Us Velocity Checks. */
                  ReturnCode = TxAuthorize_Perform_Offus_Card_Velocity_Checks();
               }
               else
               {
                  /* Proceed to authorize the ONUS transaction. */
                  ReturnCode = TxDataServer_Continue_With_Authorizing_Transaction();
               }
               return ReturnCode ;
            break;


				default: /* error situation */
					/******/
					ReturnCode = txservice_Generate_Usage_Error_Message (2,"Unknown app_datatype from data server (ST1_DB_SELECT)",
																		  "Process_Db_Reply",
																		  GS_TXN_SYSTEM_ERROR, 
																		  SYSTEM_MALFUNCTION) ;
					// Check if we have converted the Currency code during Local Processing
					if ( (strEqual(Auth_Tx.TLF01_details.host_start_time,"0.000")) &&
						(strEqual(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY)) &&
						((strEqual(Auth_Tx.BIN01_details.card_family,CARD_JCB))||
						 (strEqual(Auth_Tx.BIN01_details.card_family,CARD_AMEX))))
					{
					   strncpy( Auth_Tx.TLF01_details.currency_code,
								USDOLLAR_CURRENCY,
								CURRENCY_LEN);
					}
					return SYSTEM_ERROR ;
					/******/
				break ;

            case COUNTRY_AMT_LIMITS_DATA:

               if (PTEMSG_OK == ResultCode)               
               {
                  /* Amount limit from country_amt_limits table */
                  memcpy( &sCOUNTRY_AMT_LIMITS,
                          pATEMessageDataData+sizeof(AUTH_TX),
                          sizeof(sCOUNTRY_AMT_LIMITS) );

                  nProfileLimit = atol( sCOUNTRY_AMT_LIMITS.amount_limit );

                  /* Check if country currency code is in tlf01.nfi_seq_nbr
                   * for remotely originated transaction.
                   */
                  if (strlen(Auth_Tx.TLF01_details.nfi_seq_nbr) > 0)
                  {
                     /* Check the country transaction amount in USD.
                      * The USD amount might be in settlement_total (MC & Amex)
                      * or ch_billing_amt (Visa) if from a remote network.
                      * Check this first.
                      */
                     if ( 0x00 != Auth_Tx.TLF01_details.settlement_total[0] )
                     {
                        /* Use settlement_total from MC or Amex. */
                        nTotalAmount = atol( Auth_Tx.TLF01_details.settlement_total );
                     }
                     else if ( 0x00 != Auth_Tx.ch_billing_amt[0] )
                     {
                        /* Use Card Holder Billing amount from Visa. */
                        nTotalAmount = atol( Auth_Tx.ch_billing_amt );
                     }
                     else
                     {
                        /* The above amounts will be empty if from USA. */
                        nTotalAmount = atol( Auth_Tx.TLF01_details.total_amount );
                     }
                  }
                  else
                     nTotalAmount = atol( Auth_Tx.TLF01_details.total_amount );

                  if (nTotalAmount > nProfileLimit)
                  {
                     memset( &strTemp, 0x00, sizeof(strTemp) );
                     sprintf( strTemp, "Amt %0.2f over %0.2f lmt, Country_code %s.",
                              nTotalAmount/100.0, nProfileLimit/100.0,
                              sCOUNTRY_AMT_LIMITS.country_code );

                     ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                                  "Validate_Country Code Limits",
                                                   GS_TXN_CRF_NOT_FOUND, 
                                                   REFER_TO_CARD_ISSUER );
                     return( ReturnCode );
                  }
               }

               /* Amount is either within country code amount
                * limits or there were no limits to check.
                */

               /* Retrieve the ACF01 record using this type and nbr and
                * store it in Auth_Tx.ACF01_details. Prior to retrieving
                * the country amt details, the acct type and nbr to get
                * acf01_details were stored in auth_tx_ccf02p detail
                * position 10 of the related accounts. MessageSubType2
                * was passed as a parameter to
                * TxDataServer_Get_Cntry_Limits_Details
                */
               bReturnCode = 
                  TxDataServer_Get_ACF01_Details (Auth_Tx.CCF02P_details.related_acct[9].type,
                                                  Auth_Tx.CCF02P_details.related_acct[9].nbr,

                                                  MessageSubType2) ;

               if (false == bReturnCode) /* XIPC error */
                  return SYSTEM_ERROR ;

            break ;/*COUNTRY_AMT_LIMITS_DATA*/

            case MIDBINBLK01_DATA:
            	if(ResultCode == PTEMSG_OK)
            	{
            		// This MID is listed in Bin Blocking
            		// Have to stop processing and send response to schmens
            		sprintf( strTemp,
							  "Tx blocked , MID listed in BIN blocking %s",
							   Auth_Tx.TLF01_details.merchant_id);
            		if(true == txservice_Check_If_MC_Transaction())
					{
            			ReturnCode = txservice_Generate_Usage_Error_Message ( 2,strTemp,
            										  	  	  	  	  	  	  "TxCntl_Process_Db_Reply",
            																   GS_TXN_TRANSACTION_NOT_ALLOWED,
																			   INVALID_MERCHANT) ;
            		}
					else
					{
					ReturnCode = txservice_Generate_Usage_Error_Message ( 2,strTemp,
							  	  	  	  	  	  	  	  	  	  	  	  "TxCntl_Process_Db_Reply",
																		  GS_TXN_TRANSACTION_NOT_ALLOWED,
																		  TRANSACTION_NOT_PERMITTED) ;
					}
					/*Even for rejected MID, still send to falcon- Phani*/
					if( nFalconSendFlag == 1)//IF tx is BDO card sent it to FAI...TF Phani
					{
						if (true == Falcon_RT_Flag  &&
							true == txservice_If_Tx_Qualifies_For_RealTime() &&
							false == txservice_Check_If_IST_Message())
						{
							if (true == Falcon_RT_Ext10_Flag  &&
								false == txservice_check_auth_rc_for_falcon_ext10())
							{
								txservice_Send_EXT10_Request_To_RT_Falcon_Service();
							}

						}
						else
						{
							txservice_Send_Request_To_Falcon_Service();
						}

					}
					return ReturnCode;
            	}
            	else
            	{

            			// Continue processing the transaction
            		ReturnCode=txservice_Start_Local_Processing();
            		//If card status is restricted, let it decline with returncode from the fucntion.
            		if (strcmp (Auth_Tx.CAF01_details.status, CAFPROFILE_ACTIVE_STATUS) != 0) //
            		{
            			return ReturnCode;
            		}
            		else
            		{
            			return MID_BIN_BLK_OK;
            		}

            	}
            	break;
            case MCF03P_DATA:
				if(ResultCode == PTEMSG_OK)
				{

					memcpy( &sMCF03P,pATEMessageDataData+sizeof(AUTH_TX),
					                 sizeof(sMCF03P) );

					if(Auth_Tx.MCF01_details.credit_auth_max[0] != 0)
					{
						nCreditAuthMax = (strtod (Auth_Tx.MCF01_details.credit_auth_max, 0) / 100.0);
					}

					if(Auth_Tx.MCF01_details.credit_purch_max[0] != 0)
					{
						nCreditPurchMax = (strtod (Auth_Tx.MCF01_details.credit_purch_max, 0) / 100.0);
					}

					cumltv_amount = atol(Auth_Tx.TLF01_details.total_amount)+
							  	    atol(sMCF03P.total_amount);
					namount = (cumltv_amount / 100.0);

					if(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0200",4))
					{
						if (namount > nCreditPurchMax)
						{
							sprintf( strError, "Credit Auth max amount reached");
							txcntl_log_message( 2, 1, strError, " TxDataServer_Process_Db_Reply", 1 );
							strcpy( Auth_Tx.TLF01_details.response_code, pos_rc);
							strcpy(Auth_Tx.TLF01_details.response_text, pos_res_txt);
							memset(Auth_Tx.TLF01_details.auth_number, 0, sizeof(Auth_Tx.TLF01_details.auth_number)) ;
							MGF03P_Result_code = false;
						}
					}
					else if (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))
					{
						if (namount > nCreditAuthMax)
						{
							sprintf( strError, "Credit Auth max amount reached");
							txcntl_log_message( 2, 1, strError, " TxDataServer_Process_Db_Reply", 1 );
							strcpy( Auth_Tx.TLF01_details.response_code, pos_rc);
							strcpy(Auth_Tx.TLF01_details.response_text, pos_res_txt);
							memset(Auth_Tx.TLF01_details.auth_number, 0, sizeof(Auth_Tx.TLF01_details.auth_number)) ;
							MGF03P_Result_code = false;
						}
					}

					/*Even for Transaction is Failed  still send to falcon- Phani*/
					if( 1 == nFalconSendFlag &&
						false == MGF03P_Result_code )//IF tx is BDO card sent it to FAI...TF Phani
					{
						/* Falcon requirement to support sending EXT10 data to falcon
						 * for declined responses from Vertexon/STIP */
						if (true == Falcon_RT_Flag  &&
							true == txservice_If_Tx_Qualifies_For_RealTime() &&
							false == txservice_Check_If_IST_Message())
						{
							if (true == Falcon_RT_Ext10_Flag  &&
								false == txservice_check_auth_rc_for_falcon_ext10())
							{
								txservice_Send_EXT10_Request_To_RT_Falcon_Service();
							}

						}
						else
						{
							ReturnCode = txservice_Send_Request_To_Falcon_Service();
						}
						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
					}
					else if(true == MGF03P_Result_code)
					{
						ReturnCode = txservice_Start_Local_Processing();
						return ReturnCode;
					}
					else
					{
						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
					}
				}
				else
				{
					ReturnCode = txservice_Start_Local_Processing();
					return ReturnCode;
				}
				break;


      } /* end switch (AppDataType) */	
      break ;
   } /* end switch */
   return NO_AUTH_ERROR ;
} /* TxCntl_Process_Db_Reply */

/*************************************************************************************
NAME:			TxDataServer_Continue_With_CCF02P_Processing
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BYTE TxDataServer_Continue_With_CCF02P_Processing (BYTE MessageSubType2) 
{
	BYTE ReturnCode = 0 ;
	BOOLEAN bTypeFound = false ; 
	BOOLEAN bReturnCode = true ;
	char strRelatedAccountType[50] = {0} ;
   char strProcessingCode[20] = {0} ;
	int nIndex = 0 ;
	int nIndex1 = 0 ;
	char strTemp[256] = {0} ;

   if (txservice_If_Tx_Qualifies_For_Expiry_Date_Checking ())
	    {
		    bReturnCode = TxAuthorize_Check_Expiry_Date () ;
		    if (bReturnCode != NO_AUTH_ERROR)
			    return SYSTEM_ERROR ;
	}

	if (MessageSubType2 == ST2_DB_SELECT_CAF01_PAF01)
	{
		if (Auth_Tx.CCF02P_details.caf_profile_id[0] != 0)
		{
			ReturnCode = TxDataServer_Retrieve_CAF_Profile 
										(Auth_Tx.CCF02P_details.caf_profile_id, 
										 Auth_Tx.BIN01_details.primary_key.type,
										 ST2_DB_SELECT_CAF01_CCF02P) ;
			return ReturnCode ;
		}
		else 
		{
			ReturnCode = TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_CCF02P) ;
			return ReturnCode ;
		}
	}

	else if (ST2_DB_SELECT_CAF01_CCF02P == MessageSubType2)
	{
		memset (strRelatedAccountType, 0, sizeof strRelatedAccountType) ;

      strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;

      /* If Deferred, or void/reversal/override of a deferred... */
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION) ||
           (strEqual(strProcessingCode, "70"))                                    ||
           (Auth_Tx.TLF01_details.deferred_term_length[0] != 0x00)                 )
      {
         strcpy (strRelatedAccountType, DEFERRED_ACCOUNT_TYPE) ;
      }
		
		else if (Auth_Tx.MCF01_details.house_card_merchant[0] == 'Y')
			strcpy (strRelatedAccountType, COUNTRY_CLUB_ACCOUNT_TYPE) ;
		
		else if (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
			strcpy (strRelatedAccountType, DOLLAR_ACCOUNT_TYPE) ;
		
		else if (strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
			strcpy (strRelatedAccountType, PESO_ACCOUNT_TYPE) ;

		
		if (strlen(strRelatedAccountType) == 0)
		{
			/******/
			strcpy (strTemp, "RelatedAcc cannot be determined from Tx") ;
			ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																"Continue_With_CCF02P_Processing",
																GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																REFER_TO_CARD_ISSUER) ;
			return ReturnCode ;
			/******/
		}

		/* Search the related_acct array for the card holder to check if the 
		   the specific related account is present. if not, then it is an error */

		for (nIndex = 0 ; nIndex < 9 && !bTypeFound; nIndex++)
			if (strEqual(Auth_Tx.CCF02P_details.related_acct[nIndex].type, strRelatedAccountType))
				bTypeFound = true ;

		
	
		/* if the type wasn't found or the nbr was NULL,  return */
		if (!bTypeFound) 
		{
			/*****/
         if ( cPESO_ACCT_TYPE == strRelatedAccountType[1] )
            strcpy( strTemp, "Peso " );

         else if ( cDOLLAR_ACCT_TYPE == strRelatedAccountType[1] )
            strcpy( strTemp, "Dollar " );

         else if ( cDEFERRED_ACCT_TYPE == strRelatedAccountType[1] )
            strcpy( strTemp, "Deferred " );

         else
            strcpy( strTemp, "House " );
		 /* The txn currency is USD. if Dollar account not found, convert into peso. debit from the peso account 
		   OR The txn currency is Peso. If Peso account not found debit from the Dollar account if found. 
		   Otherwise log error message. 08-Sep-2009*/
			 if((strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN)==0 || strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0 )&&
				(!strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP)) &&
				(!strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP)) 
				)
				{
					if(strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)) 
					{
						strcpy(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY);
						strcpy(Auth_Tx.TLF01_details.host_start_time, PESO_COMP);
					}
					else if(strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
					{
						strcpy(Auth_Tx.TLF01_details.currency_code,USDOLLAR_CURRENCY);
						strcpy(Auth_Tx.TLF01_details.host_start_time, USD_COMP);
					}
					ReturnCode = TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_CCF02P) ;
						return ReturnCode ;
		 				
				}
				else
				{   
					if(strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP) ||
					   strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP)	
					  )
					  strcpy( strTemp, "Peso and Dollar " );	

					strcat( strTemp, "account not found in card record" );
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		"Continue_With_CCF02P_Processing",
																		GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																		REFER_TO_CARD_ISSUER) ;
					return ReturnCode ;
				}
		}
		if (strlen(Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr) == 0)
		{
			/*****/
			strcpy (strTemp, "RelatedAccNo invalid for relatedacctype") ;
			ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																  "Continue_With_CCF02P_Processing",
																  GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																  REFER_TO_CARD_ISSUER) ;
			return ReturnCode ;
			/*****/
		}

		if (strEqual (strRelatedAccountType, DEFERRED_ACCOUNT_TYPE))
		{
			MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_DEFERRED ;
			bTypeFound = false ;

			for (nIndex1 = 0 ; nIndex1 < 9 && !bTypeFound; nIndex1++)
				if (strEqual(Auth_Tx.CCF02P_details.related_acct[nIndex1].type, PESO_ACCOUNT_TYPE))
					bTypeFound = true ;

			if (!bTypeFound) 
			{
				/* If a card holder has a deferred account type, then
					the card holder also has to have a Peso account */
				/*****/
				strcpy (strTemp, "PesoAcc notfound in related for defered") ;
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																      "Continue_With_CCF02P_Processing",
																      GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																      REFER_TO_CARD_ISSUER) ;
				return ReturnCode ;
				/*****/
			
			}
		}
		else
			MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_OTHER ;

      /* If the country amt limits flag in enabled, retrieve the
       * country_amt_limits record to verify the dollar amount
       * does not exceed currency/country code limit - LM SCR13929
       */
      if ( (Auth_Tx.CAF01_details.country_limits_flag[0] == 'Y') &&
           (!txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override() ) &&
		   (Auth_Tx.TLF01_details.pin_block[0] == 0x00 ) )
      {
         /* Temporarily store the card related account type and number
          * in the ccf02 related account position 10 to recover this data
          * to pass to get_acf01_details after the country amt limits
          * record is handled.
          */

         strcpy(Auth_Tx.CCF02P_details.related_acct[9].type, Auth_Tx.CCF02P_details.related_acct[nIndex-1].type);
         strcpy(Auth_Tx.CCF02P_details.related_acct[9].nbr,  Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr);

         bReturnCode = TxDataServer_Get_Cntry_Limits_Details(MessageSubType2);

         if(false == bReturnCode) /* XIPC error */
            return SYSTEM_ERROR;
      }
      else
      {
         /* Retrieve the ACF01 record using this type and nbr
          * and store it in Auth_Tx.ACF01_details
          */
		 //TF PHANI store store the card related account type and number
		 strcpy(Auth_Tx.CCF02P_details.related_acct[9].type, Auth_Tx.CCF02P_details.related_acct[nIndex-1].type);
                 strcpy(Auth_Tx.CCF02P_details.related_acct[9].nbr,  Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr);

         bReturnCode = 
            TxDataServer_Get_ACF01_Details (Auth_Tx.CCF02P_details.related_acct[nIndex-1].type, 
                                            Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr,
                                            MessageSubType2 );

         if (false == bReturnCode) /* XIPC error */
            return SYSTEM_ERROR;
      }
   }
   return NO_AUTH_ERROR ;
} /* TxDataServer_Continue_With_CCF02P_Processing */



BYTE TxDataServer_Process_CAF_Not_Found (BYTE caf_context)
{
	char strTemp[1000] = {0} ;
	BYTE ReturnCode = 0 ;

	if (caf_context == ST2_DB_SELECT_CAF01_BIN01)
	{
		sprintf (strTemp,
					"Profile %s in BIN notfound in CAF01\n", 
					Auth_Tx.BIN01_details.caf_profile_id) ; 
	}
	else if (caf_context == ST2_DB_SELECT_CAF01_PAF01)
	{
		strcpy (strTemp,"Profile at Primary Acct level not found") ; 
	}
	else if (caf_context == ST2_DB_SELECT_CAF01_CCF02P)
	{
		sprintf (strTemp,
					"Profile %s  in Card level notfound", 
					Auth_Tx.CCF02P_details.caf_profile_id) ; 
	}

	/*****/
	ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
														  "Process_CAF_Not_Found",
														  GS_TXN_CAF_NOT_FOUND, 
														  REFER_TO_CARD_ISSUER) ;
	return ReturnCode ;
	/*****/
	
} /* TxDataServer_Process_CAF_Not_Found */

BYTE TxDataServer_Process_CAF_Found (BYTE caf_context)
{
   BYTE    ReturnCode = 0 ;
   CHAR    strTemp[256] = {0} ;
   BOOLEAN bReturnCode = true ;
   char strResponseCode[256] = {0} ;


   if (strcmp (Auth_Tx.CAF01_details.status, CAFPROFILE_ACTIVE_STATUS) != 0)
   {
      /* CAF Profile status is not ACTIVE. It is an BDO business
       * rule to set response code to 96 instead of 01 in this case.
       */
      strcpy (strTemp, "Invalid status for profile\n") ;
      ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                                    "Process_CAF_Found",
                                                     GS_TXN_INVALID_CAF_STATUS,
                                                     DO_NOT_HONOR);
	  /* Response changed to 05 from 96 - as per BDO req*/
      return ReturnCode ;
   }
   /**************************************************************/
   /* EQIT Interface Changes, Jan 02 2009, Girija Y, TF */
   /* Added code to pass network Id to eqit */

    txservice_Eqit_POS_Field63(&Auth_Tx,Auth_Tx.CAF01_details.destination_name);
   /* TF Phani - Copy BIN and PROFILE ID*/
	strncpy(Auth_Tx.TLF01_details.product_codes[2].code,Auth_Tx.BIN01_details.primary_key.low,6);// BIN LOW
	strcpy(Auth_Tx.TLF01_details.product_codes[3].code,Auth_Tx.CAF01_details.primary_key.caf_profile_id);//PROFILE ID
	if(0 != strcmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA"))
	strcpy(Auth_Tx.TLF01_details.product_codes[4].code,"ISSUE");//INCOMING  tx
	else
	strcpy(Auth_Tx.TLF01_details.product_codes[4].code,"ACQUR");// BDO merchant Acq tx

	// Store transaction type is ONUS or OFFUS card
	if(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0)
	{
		// ONUS card
		strcpy(Auth_Tx.TLF01_details.product_codes[6].code,"ONUS"); 
	}
	else
	{
		//OFFUS card
		strcpy(Auth_Tx.TLF01_details.product_codes[6].code,"OFFUS");
	}
		
   
	if (caf_context == ST2_DB_SELECT_CAF01_BIN01)
	{
		/*	CAF found at the bin level */
		
		/*	if the authorization_method is NEGATIVE, 
				check the negative table 
				if card is found in the negative table, 
					then decline the transaction
					return ERROR BUT FORWARD TO UPDATE PROCESSOR to Txcntl.c
		*/
		/*  for authorization_methods NEGATIVE, POSITIVE, POSITIVE_WITH_BALANCE
			get floor limit on the MCT01 table, if NULL or 0, 
			get the floor limit on the profile)
					
			if the transaction amount is less than or equal to the floor limit 
				approve the transaction
				return NO_AUTH_ERROR
						

			if the transaction amount is greater than the floor limit
				if the authorization_method is NEGATIVE
					if authorization location is not LOCAL
						get the <CAF01.destination_name>	
						switch out to the remote host
						return NO_AUTH_ERROR
					else if authorization_method is LOCAL
						decline the transaction
						return ERROR BUT FORWARD TO UPDATE PROCESSOR to Txcntl.c
				if the authorization_method is POSITIVE or POSITIVE_WITH_BALANCE
						regardless of authorization_location 
						proceed with getting CCF02P
							
				
		*/

      /* If transaction is to be switched out, do not perform any more
       * local processing unless transaction qualifies for velocity checking.
       * However, velocity checking is only for local origin; this is the
       * only case where we have the merchant record.
       */
      if ( 0 != strcmp(Auth_Tx.CAF01_details.destination_name, NULL_STR) )
      {
         /* The currency may have been changed from USD to Peso for Amex.
          * But this should only be done for ON-US.  Since this is not
          * ON-US, change the currency code back to USD.  The amount was
          * not converted.
          */
         if ( strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )
         {
            if ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000") )
               strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
         }

         if ( (REMOTE_ORIGIN == txservice_GetTransactionOrigin       () ) ||
              (!txservice_If_Tx_Qualifies_For_Card_Velocity_Checking () )  )
         {
			 /* In case of STAND IN,We will do balnace checking before it goes to HOST
				Keep all accounts ready ,once transaction approved we update balances.  
			 
			if(strEqual (Auth_Tx.CAF01_details.authorization_method, M_POSITIVE_WITH_BALANCE_FILE) &&
				strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0
				)
			{
				 STAND IN porcessing Do not forward to Net work controller now..we willdo
				Local processing and get balances updated then we can forward to NC..TF PHANI
			}*/
			
            ReturnCode = 
               txservice_Process_Bin_Range_Switch_Out (BIN01_DATA, 
                                   Auth_Tx.CAF01_details.destination_name,
                                   Auth_Tx.BIN01_details.alt_destination_name) ;
			
            return ReturnCode ;
			

         }
      }

      /* Do not check negative status for VOIDS. */
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE) || (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) || (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD))
		{
         ReturnCode = TxDataServer_Continue_With_Local_Processing () ;
            return ReturnCode ;
      }

		if ( strEqual (Auth_Tx.CAF01_details.authorization_method,NEGATIVE_FILE)                ||
           strEqual (Auth_Tx.CAF01_details.authorization_method,NEGATIVE_WITH_OFFUS_VELOCITY) ||
           strEqual (Auth_Tx.CAF01_details.authorization_method,M_NEGATIVE_OFFUS_NEGATIVE_FRAUD))
		{
			bReturnCode = TxDataServer_Get_NEG01_Details () ;
			if (bReturnCode == false) 
				return SYSTEM_ERROR ;
			return NO_AUTH_ERROR ;
		}

      if ( strEqual (Auth_Tx.CAF01_details.authorization_method,OFFUS_VELOCITY) && txservice_If_Tx_Qualifies_For_Card_Velocity_Checking ())/* IA added for offline velocity */
           
		{
			bReturnCode = TxDataServer_Get_Offus_Velocity_Details () ;
			if (bReturnCode == false) 
				return SYSTEM_ERROR ;
			return NO_AUTH_ERROR ;
		}


		ReturnCode = TxDataServer_Continue_With_Local_Processing () ;
		return ReturnCode ;
		
	}
	else if (caf_context == ST2_DB_SELECT_CAF01_PAF01)
	{
		ReturnCode = 
			TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_PAF01) ;	
		return ReturnCode ;
	}
	else if (caf_context == ST2_DB_SELECT_CAF01_CCF02P)
	{
		ReturnCode = 
			TxDataServer_Continue_With_CCF02P_Processing (ST2_DB_SELECT_CAF01_CCF02P) ;
		return ReturnCode ;
	}
	return NO_AUTH_ERROR ;
} /* TxDataServer_Process_CAF_Found (BYTE caf_context) */

BYTE TxDataServer_Retrieve_CAF_Profile (BYTE* caf_profile_id,
													 BYTE* type,
													 BYTE caf_context) 
{
	BYTE ReturnCode = 0 ;
	/* Retrieve the CAF01 record using the the caf_profile_id in the bin table 
	(Auth_Tx.BIN01_details.caf_profile_id) */
	/* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
	ReturnCode = TxDataServer_Get_CAF01_Details (SHARED_MEMORY_READ, 
												caf_profile_id, 
							                    type,
												caf_context) ;
																
	if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY)
	{
		ReturnCode = TxDataServer_Process_CAF_Found (caf_context) ;
	}
	else
	{
		ReturnCode = TxDataServer_Get_CAF01_Details (DATABASE_READ, 
													caf_profile_id, 
													type,
													caf_context) ;
	}
	return ReturnCode ;
	
} /* TxDataServer_Retrieve_CAF_Profile */

BYTE TxDataServer_Continue_With_ACF01_Processing (BYTE MessageSubType2) 
{
	double nConvertedTxAmount = 0.0 ;
	
	BOOLEAN bReturnCode = true ;
	BYTE ReturnCode = 0 ;


    if ( (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY) &&
         (MessageSubType2 != BALANCE_ACCOUNT_TYPE_IS_DEFERRED)   )
    {
        Auth_Tx.available_balance_sign = '+' ;

        /* Use '+3' to account for the differences in size of the 2 fields. */
        memcpy( Auth_Tx.available_balance, Auth_Tx.ACF01_details.available+3,
                sizeof(Auth_Tx.available_balance) );

        strcpy (Auth_Tx.TLF01_details.conversion_rate, "1") ;
        bReturnCode = TxDataServer_Get_CCF03_Details() ;

        /* details about the CCF03 table */
        if (false == bReturnCode) /* XIPC Error */
           return SYSTEM_ERROR ;
/*		if (txservice_If_Tx_Requires_Pin_Validation())
		{
			if (strlen(Auth_Tx.TLF01_details.pin_block) > 0 ) 
			{
				if (!txservice_If_Tx_Cannot_Be_Declined())
				{
					ReturnCode = TxAuthorize_Process_Security (PIN_VERIFICATION) ;
					if(ReturnCode != NO_AUTH_ERROR)
						txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, "96") ;
					return NO_AUTH_ERROR ;
				}
			}
		}
        txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
		return NO_AUTH_ERROR ;
*/
    }

	else if (MessageSubType2 == BALANCE_ACCOUNT_TYPE_IS_DEFERRED)
	{
		/* Verify that funds are available in the DEFERRED balance account */
		/* nConvertedTxAmount will be calculated inside the function */
      if ((!txservice_If_Tx_Is_Treated_Like_A_Reversal ()) &&
          (!txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override ()))
      {
         /* Validate deferred amounts only if the transaction is a DEFERRED_PURCHASE_AUTHORIZATION */
         /* Not a REVERSAL or a OVERRIDE of a DEFERRED purchase */
		   ReturnCode = TxAuthorize_Perform_Balance_Account_Checks (nConvertedTxAmount) ;

		   if (ReturnCode != NO_AUTH_ERROR) /* funds are not available */
			   return ReturnCode ;
      }
		
		/* Since funds are available for the deferred gross amount, we have 
			to check if the funds are available in the PESO balance account. Please
			note that if the card has a deferred acccount, it also must have a PESO 
			account. The PESO balance account must have funds equal to
			Gross Amount/Deferred Term
		*/
			
		/*	Retrieve the ACF01 record using this nbr and 
			PESO_CURRENCY_TYPE and store it in Auth_Tx.ACF01_details 
		*/
		bReturnCode = 
			TxDataServer_Get_ACF01_Details (PESO_ACCOUNT_TYPE,
													  Auth_Tx.ACF01_details.primary_key.account_nbr,
													  BALANCE_ACCOUNT_TYPE_IS_OTHER) ;
		if (false == bReturnCode) /* XIPC error */
			return SYSTEM_ERROR ;
	}
	else
	{
		strcpy (Auth_Tx.TLF01_details.conversion_rate, "1") ;
		bReturnCode = TxDataServer_Get_CCF03_Details() ; 
		/* details about the CCF03 table */
		if (false == bReturnCode) /* XIPC Error */
			return SYSTEM_ERROR ;
	}
	return NO_AUTH_ERROR ;
} /* TxDataServer_Continue_With_ACF01_Processing */



BYTE TxDataServer_Get_Bin_Level_Profile (void) 
{
	BYTE ReturnCode = true ;
	char strTemp[256] = {0} ;
 
	if (Auth_Tx.BIN01_details.caf_profile_id[0] != 0)
	{
		ReturnCode = 
			TxDataServer_Retrieve_CAF_Profile (Auth_Tx.BIN01_details.caf_profile_id, 
														  Auth_Tx.BIN01_details.primary_key.type,
														  ST2_DB_SELECT_CAF01_BIN01) ;
		return ReturnCode ;
	}
	else
	{
		/* caf_profile_id cannot be NULL at the bin level */
		/******/
		strcpy (strTemp, "caf_profileid cannotbe NULL at binlevel\n") ;
		ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
															  "Get_Bin_Level_Profile",
															  GS_TXN_INVALID_CAF, 
															  REFER_TO_CARD_ISSUER) ;
		return ReturnCode ;
		/*****/
		
	}
	
	return NO_AUTH_ERROR ;

} /* TxDataServer_Get_Bin_Level_Profile */

BYTE TxDataServer_Continue_With_Pin_Processing (void)
{
   BYTE  ReturnCode = 0;
   BYTE  uag_rcode = 0;
   BYTE doCvv = 0;
   CHAR strTemp[512] = {0};
   int index= 0;
   int total_count = 0;
   char gen_cvv2_value[4]={0};
   int jcb3ds2_total_count=0;
   CHAR gen_av_op[4] = {0};
   BOOLEAN rc=0;

	memset(Auth_Tx.TLF01_details.product_codes[12].code,0x00,
			sizeof(Auth_Tx.TLF01_details.product_codes[12].code));

   /* In case of STIP , For AMEX -------- TF Phani
   If it 4CSC or 4DBC, the CSC validation code should always be Ã¯Â¿Â½YÃ¯Â¿Â½.
   If the setup in the auth profileÃ¯Â¿Â½s CVV Processing is no validation,CSC validation code should always be Ã¯Â¿Â½YÃ¯Â¿Â½.
   */
   if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN) == 0 &&
      (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )
	 )
   {  
		if ( (Auth_Tx.EMV_details.emv_block[0] != 0x00) ||
		(Auth_Tx.EMV_details.emv_block[1] != 0x00)  )
		{
			if(strlen(Auth_Tx.EMV_details.app_crypto) > 0 )// Check if EMV data is there
			{
				ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_ARQC) ;
				//ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
				return ReturnCode;
			}
			else
			{
					strcpy(strTemp, "Invalid EMV transaction,No ARPC present");
					strncpy(Auth_Tx.TLF01_details.security_response_code,
							DINERS_RACAL_DECLINE_RESPONSE,
							DINERS_RACAL_RESPONSE_LEN);
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		 "TxDataServer_Continue_With_Pin_Processing",
																		 GS_TXN_TRANSACTION_NOT_ALLOWED,
																		 DO_NOT_HONOR) ;

				return ReturnCode;
			}
		}
		else  if(Auth_Tx.CAF01_details.cvv_processing[0] == '0')
		{
			strcpy(Auth_Tx.TLF01_details.security_response_code,"00");
			 /* Get the MCC Limits, then authorize the transaction. */
			ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
			return ReturnCode;
		}		/*Check the version field to process 3DS2 */
		else if(true == txservice_check_from_Amex_3DS2_txn())//AMEX 3DS2.0
		{

			if(true == txservice_check_txn_for_fully_authentication())
			{
				ReturnCode = TxAuthorize_Process_Security (MT_NSP_VERIFY_3D_SECURE_2);
					return ReturnCode ;

			}
			else if(true == txservice_check_txn_for_authentication_attempt())
			{
				if(amex_3ds2.authentication_attempt_verify_flag[0] == 'A')
				{
					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
											return ReturnCode;
				}
				else
				{
					strcpy(Auth_Tx.TLF01_details.response_text,
													"AuthN attempt rjctd/could not be prfmd");
					strcpy(strTemp,Auth_Tx.TLF01_details.response_text);
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
									"TxDataServer_Continue_With_Pin_Processing",
									GS_TXN_TRANSACTION_NOT_ALLOWED,
									DO_NOT_HONOR ) ;
					strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
							amex_3ds2.authentication_attempt_ac,3);
					return ReturnCode ;
				}

			}
			else if (true == txservice_Check_AEVVResultCode_value_to_directly_decline_txn())
			{
				// AEVV verification was done by AMEX and its failed. Decline the transaction
				strcpy(Auth_Tx.TLF01_details.response_text,
												"Authentication rjctd/could not be prfmd");
				strcpy(strTemp,Auth_Tx.TLF01_details.response_text);
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
												"TxDataServer_Continue_With_Pin_Processing",
												GS_TXN_TRANSACTION_NOT_ALLOWED,
												DO_NOT_HONOR ) ;
				strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
						amex_3ds2.authentication_result_code_ac,3);
				return ReturnCode ;

			}
			else if (true == txservice_Check_AEVVResultCode_value_to_skip_AEVV_validation())
			{
				ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
				return ReturnCode;
			}
			else if(strlen(Auth_Tx.TLF01_details.interest_rate) > 2)
			{
					 ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
					return ReturnCode;
			}
		}
		else if(strlen(Auth_Tx.TLF01_details.interest_rate) > 2)
		{
		         ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
			 	return ReturnCode;
		}
#if 0
	   else if((Auth_Tx.TLF01_details.interest_rate[0] != 0x00) && 
			   strcmp(Auth_Tx.TLF01_details.product_codes[19].quantity, "44") == 0 &&
			   strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"06",2) != 0 /* If Card swipe */
		      )
	   {	// if 4 CSC 
			if(strlen(Auth_Tx.TLF01_details.interest_rate) == 4 || 
			   strlen(Auth_Tx.TLF01_details.interest_rate) == 5	)
			{
				if( (Auth_Tx.TLF01_details.interest_rate[0] != 'S') && 
					strlen(Auth_Tx.TLF01_details.interest_rate) == 4
				   )
				{ // 4CSC
						strcpy(Auth_Tx.TLF01_details.security_response_code,"00");
						 /* Get the MCC Limits, then authorize the transaction. */
						ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
						return ReturnCode;
				}
				if( (Auth_Tx.TLF01_details.interest_rate[0] == 'S') && 
					strlen(Auth_Tx.TLF01_details.interest_rate) == 5
				   )
				{ // 4CSC
						strcpy(Auth_Tx.TLF01_details.security_response_code,"00");
						 /* Get the MCC Limits, then authorize the transaction. */
						ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
						return ReturnCode;
				}
			}
	   }
#endif	   
   }
   /* POS AsiaPay transaction with 3D secure data transaction should be sent to racal for
    * CAVV validation, hence check whether transaction ic from DCPISO and Auth_Tx.int_net_facility_len not equal to null.
    * Auth_Tx.int_net_facility_len will have the 3D secure data length and data will be copied to auth_tx.int_net_facility_data
    * */
	if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",5) == 0 && strlen(Auth_Tx.int_net_facility_len) != 0 )
	{
		strncpy(Auth_Tx.TLF01_details.nfi_end_date,"1",1);
	}
   /* Do not process CVV if the transaction has been manually entered
    * and is from a MOTO or a T&E merchant and is not a VOICE authorized 
    * transaction.
    */
	//
	//if(strcmp(Auth_Tx.industry_code,"700")== 0 &&  strcmp(Auth_Tx.TLF01_details.nfi_end_date,"1") == 0)// E-coomerce Validate 3D secure value

/* MC -3 Applicable only for E-commerce transaction
 *
 * */
	if( strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
		strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"081",3) == 0)
	{
		if	(true == txservice_MC_check_for_OBS_Digital_Payment_Data_Validation())
		{
			rc = txservice_read_OBS_record_for_MC();
			if( rc == PTEMSG_OK && obs01_data.response_code[0] != '\0')
			{

				if(0 != strncmp(obs01_data.response_code,TRANSACTION_APPROVED,2))
				{
					sprintf(strTemp, "OBS_Digital_Payment_Data_Validation section, Txn decline based on OBS rule, OB service: %s, OB result: %s",
							Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		 "TxDataServer_Continue_With_Pin_Processing",
																		 GS_TXN_TRANSACTION_NOT_ALLOWED,
																		 obs01_data.response_code) ;

					memset(Auth_Tx.TLF01_details.response_text,0x00,sizeof(Auth_Tx.TLF01_details.response_text));
					strncpy(Auth_Tx.TLF01_details.response_code,obs01_data.response_code,2);
					strncpy(Auth_Tx.TLF01_details.response_text,obs01_data.description,40);

					return ReturnCode;
				}
				else
				{
					 /*BAU, OBS configured has approved for 54 and D, so transaction fall under BAU*/
				}

			}
			else
			{
				sprintf(strTemp, "OBS_Digital_Payment_Data_Validation fail and OBS is not configured in GUI, Txn decline using default RC configured in tf.ini, OB service: %s, OB result: %s",
				Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																	 "TxDataServer_Continue_With_Pin_Processing",
																	 GS_TXN_TRANSACTION_NOT_ALLOWED,
																	 mc_default_rc_for_digital_payment_data) ;
				return ReturnCode;
			}
		}
	}

    if( true == txservice_Check_If_ECOM_TRAN())
	{
    	//strcpy(Auth_Tx.TLF01_details.voice_auth_text,Auth_Tx.TLF01_details.dcf01_retired_cwk);
		//for VISA DE25 should be 59
    	if ((strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA) == 0) &&
		((strncmp(Auth_Tx.TLF01_details.pos_condition_code,ECOM_POS_CC,2) == 0)||
		(strncmp(Auth_Tx.TLF01_details.pos_condition_code,ECOM_POS_CC_VISA,2) == 0)))
		{
			strcpy(Auth_Tx.TLF01_details.voice_auth_text,Auth_Tx.TLF01_details.dcf01_retired_cwk);
			if( 0 == (strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis",5)))
			{

		       /*Separated the code for processing for VISA 3DS1 and 3DS2.
		        * This is for Visa 3DS2 processing. Self validation should happen only when 126.20 value is not 0.
		        */
				if(Auth_Tx.TLF01_details.product_codes[0].code[2]!='0' &&
						Auth_Tx.TLF01_details.product_codes[0].code[2]!='\0')
				{
					if(true == txservice_Is_Txn_Qualify_For_Self_CAVV_Validation() ||
					   true == txservice_Is_Txn_Qualify_for_DAF_with_CAVV_validation() ||
					   true == txservice_Is_Txn_Qualify_for_nonDAF_with_CAVV_validation())
					{
						if ( true == txservice_Is_Txn_Qualify_For_Account_Verification() ||
							 true == txservice_verify_VISA_self_validation())
						{
							//call to racal
							ReturnCode = TxAuthorize_Process_Security (MT_NSP_VERIFY_3D_SECURE_2) ;
							return ReturnCode ;
						}
						else
						{
							sprintf(strTemp, "Txn declined for Cavv value: %s",Auth_Tx.TLF01_details.cvc);
							ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		"TxDataServer_Continue_With_Pin_Processing",
																			 GS_TXN_TRANSACTION_NOT_ALLOWED,
																			 DO_NOT_HONOR);
							return ReturnCode ;
						}
					}
					else if (true == txservice_Is_Txn_Qualify_for_Visa_Secure_Smart_Attempts_Service() )
					{
						if (true == txservice_Is_Txn_Qualify_for_Smart_Service_with_passed_CAVV_validation())
						{
							/* Check for CVV2 presennce indicator in the posn 1 of F126.10
							    '1' - CVV2 present, perform CVV2 validation
								'2' - CVV2 missing, do not perform CVV2 validation */
							if (Auth_Tx.TLF01_details.product_codes[15].code[0] == '1')
							{
								ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
								return ReturnCode ;
							}
							else
							{
								TxAuthorize_Update_Private_Ccf03p_Structure () ;
								ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
								return ReturnCode ;
							}
						}
						else if (true == txservice_Is_Txn_Qualify_for_Smart_Service_with_failed_CAVV_validation())
						{
							sprintf(strTemp, "CAVV Results Code: %c (CAVV failed attempt)",Auth_Tx.TLF01_details.cvc);
							ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																					"TxDataServer_Continue_With_Pin_Processing",
																						 GS_TXN_TRANSACTION_NOT_ALLOWED,
																						 SUSPECTED_FRAUD);
							return ReturnCode ;
						}
					}
								/* VS-6: APR MNDT 2025, ECI 05				 *
								 * Frictionless Flow using FIDO standards or *
								 * Visa Secure On-Behalf-Of Issuer services  */
					else if (true == txservice_Is_Txn_Qualify_for_Frictionless_Flow_using_FIDO_standards() ||
							 true == txservice_Is_Txn_Qualify_for_Frictionless_Flow_Visa_secure_services())
					{
						/* Check for CVV2 presennce indicator in the posn 1 of F126.10
						    '1' - CVV2 present, perform CVV2 validation
							'2' - CVV2 missing, do not perform CVV2 validation */
						if (Auth_Tx.TLF01_details.product_codes[15].code[0] == '1')
						{
							ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							return ReturnCode ;
						}
						else
						{
							TxAuthorize_Update_Private_Ccf03p_Structure () ;
							ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							return ReturnCode ;
						}
					}
						/* VS-7: APR MNDT 2025, Visa Data only txns for ECI 07 */
					else if (true == txservice_Is_Txn_Qualify_for_Visa_Data_Only_transactions())
					{
						if (true == txservice_Is_Txn_Qualify_for_Visa_Data_Only_with_CAVV_passed_valdn())
						{
							/* Check for CVV2 presennce indicator in the posn 1 of F126.10
							    '1' - CVV2 present, perform CVV2 validation
								'2' - CVV2 missing, do not perform CVV2 validation */
							if (Auth_Tx.TLF01_details.product_codes[15].code[0] == '1')
							{
								ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
								return ReturnCode ;
							}
							else
							{
								TxAuthorize_Update_Private_Ccf03p_Structure () ;
								ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
								return ReturnCode ;
							}
						}
						else if (true == txservice_Is_Txn_Qualify_for_Visa_Data_Only_with_CAVV_failed_valdn())
						{
							sprintf(strTemp, "CAVV Results Code: %c (CAVV failed verification-attempt authn)",Auth_Tx.TLF01_details.cvc);
							ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																					"TxDataServer_Continue_With_Pin_Processing",
																						 GS_TXN_TRANSACTION_NOT_ALLOWED,
																						 SUSPECTED_FRAUD);
							return ReturnCode ;
						}
					}
						/* skip CAVV validation for VISA 3DS2 */
					else if (true == txservice_Is_Txn_Qualify_for_DAF_without_CAVV_validation() ||
							 true == txservice_Is_Txn_Qualify_for_nonDAF_without_CAVV_validation())
					{
						TxAuthorize_Update_Private_Ccf03p_Structure () ;
						ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
						return ReturnCode ;
					}
						/* VS-8: APR MNDT 2025, Visa Intelligent Data Exchange (IDX) txns for ECI 07 */
					else if (true == txservice_Is_Txn_Qualify_for_Visa_IDX_transactions())
					{
						/* Check for CVV2 presennce indicator in the posn 1 of F126.10
						    '1' - CVV2 present, perform CVV2 validation
							'2' - CVV2 missing, do not perform CVV2 validation */
						if (Auth_Tx.TLF01_details.product_codes[15].code[0] == '1')
						{
							ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							return ReturnCode ;
						}
						else
						{
							TxAuthorize_Update_Private_Ccf03p_Structure () ;
							ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							return ReturnCode ;
						}
					}
					else if(' ' != Auth_Tx.TLF01_details.cvc && '\0' != Auth_Tx.TLF01_details.cvc)  //null is changed to space in ncvisa
					{
						total_count = atoi(visa3ds2_list.num_returned);
						for(index =0; index<total_count; index++)
						{
							if(Auth_Tx.TLF01_details.cvc == visa3ds2_list.visa3ds2_record[index].cavv[0])
							{
								strncpy(Auth_Tx.TLF01_details.response_code,visa3ds2_list.visa3ds2_record[index].response_code,2);
								strncpy(Auth_Tx.TLF01_details.response_text,visa3ds2_list.visa3ds2_record[index].description,40);
								if(0 != strncmp(Auth_Tx.TLF01_details.response_code,TRANSACTION_APPROVED,2))
								{
									/*sprintf(strTemp, "Txn declined for Cavv value: %s",Auth_Tx.TLF01_details.cvc);*/
									strcpy(strTemp, Auth_Tx.TLF01_details.response_text);
									ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																						"TxDataServer_Continue_With_Pin_Processing",
																						GS_TXN_TRANSACTION_NOT_ALLOWED,
																						Auth_Tx.TLF01_details.response_code) ;
									return ReturnCode;
								}
									TxAuthorize_Update_Private_Ccf03p_Structure () ;
									ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
									return ReturnCode;
							}

						}
						if (Auth_Tx.TLF01_details.cvc == '8' || Auth_Tx.TLF01_details.cvc == 'A')
						{
							TxAuthorize_Update_Private_Ccf03p_Structure () ;
							ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
						}
						else
						{
							sprintf(strTemp, "Txn dclnd for undfnd Cavv rslt code: %s",Auth_Tx.TLF01_details.cvc);
							ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																				"TxDataServer_Continue_With_Pin_Processing",
																				GS_TXN_TRANSACTION_NOT_ALLOWED,
																				DO_NOT_HONOR) ;

						}
							return ReturnCode;
					}
				}

				//VISA 3DS1 Validation processing
				else if( 0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk + VISA_CAVV_USAGE_3_POS,
								 VISA_CAVV_VALIDATION_KEY1,
								 VISA_CAVV_VALIDATION_KEY_LEN)||
					0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk + VISA_CAVV_USAGE_3_POS,
								 VISA_CAVV_VALIDATION_KEY2,
								 VISA_CAVV_VALIDATION_KEY_LEN))
				{
					// Lets check CAVV result code for Visa failed

					if(true == txservice_Check_CAVVResultCode_value_to_directly_decline_txn())
					{
						// CAVV verification was done by visa and its failed. Decline the transaction
						strcpy(Auth_Tx.TLF01_details.response_text,
								"VISA attempt CAVV Failed verification");
						strcpy(strTemp,Auth_Tx.TLF01_details.response_text);
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			"TxDataServer_Continue_With_Pin_Processing",
																			GS_TXN_TRANSACTION_NOT_ALLOWED,
																			DO_NOT_HONOR) ;
						return ReturnCode ;

					}
					// Lets check CAVV result code for Visa success
					else if(true == txservice_Check_CAVVResultCode_value_to_skip_CAVV_validation())
					{
						// CAVV verification was done by visa and its successful.Proceed with authorization
						 /* Repopulate the private ccf03p structure. */
						 TxAuthorize_Update_Private_Ccf03p_Structure () ;
						 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					}
					else
					{
						ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
					}
				}
				else
				{
					ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
				}
			}
			else
			{
				ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;  // All brands except VISA
			}
		}
		else if (strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
			     (Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5' ||
			     ((strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"081",3) == 0) || strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"082",3) == 0)))
		{
			/* Coded as per Marvins �mail(Re: MasterCard 3DS 2.0: OBS 05 Transaction Declines resolution) dated 16/04/2024 comment and JIRA ID:BDOPS2024-28
			 *
			 * DE48 SE42 and SE43 are present
			 * DE61.4 =5 txn qualified for 3DS validation
			 * */
			/* MC 3DS 2.0 handling changed as per the NEW BR(April Mandated-2023)
			 * Please refer JIRA ticket ID BDOR-46 for complete functionality
			 *
			 *
			 * */
			sprintf(strTemp, "Transaction qualified for 3DS 2 validation POS entry mode: %s, DE61.4 %c",
					Auth_Tx.TLF01_details.pos_entry_mode,Auth_Tx.TLF01_details.product_codes[16].amount[3]);
			txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
			/*print the OBS values*/

			sprintf(strTemp, "Txn fall under OBS section, Txn decline based on OBS rule, OB service: %s, OB result: %s",
												Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
			txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );

			/*  ***** IMPORTANT *************
			 *
			 * line number 5377 to 5519.
			 *
			 *  THE BELOW COMMENTED CODE IS TO VALIDATE THE PROGRAM PROTOCOL VERSION DE48 SE66 SF1, THIS CODE IS NOT REQUIRED AS PER THE
			 *   COMMENTS FROM MARVIN, PRIMARY PRIORITY IS OBS ONLY, SO DIRECLTY CHECK OBS FIRST THEN FOLLOWED BY OTHER SLI COMBINATIONS
			 *
			 * Auth_Tx.TLF01_details.product_codes[11].quantity holds program protocol/ 3DS version DE48 SE66 SF1
			 *
			 * if present and not valid means other than 1 to 9, validate OBS only take decision based on OBS,
			 * bcz OBS is sent by MC bay validation the 3DS details
			 *
			 * if valid or not present, go to else
			 * */
/*
			if( Auth_Tx.TLF01_details.product_codes[11].quantity[0] != '\0' &&
			   (false == isdigit(Auth_Tx.TLF01_details.product_codes[11].quantity[0]) ||
				'0'  ==	Auth_Tx.TLF01_details.product_codes[11].quantity[0]))
			{
				if	(true == txservice_MC_check_for_OBS_validation())
				{
					rc = txservice_read_OBS_record_for_MC();
					if( rc == PTEMSG_OK && obs01_data.response_code[0] != '\0')
					{
						if(0 != strncmp(obs01_data.response_code,TRANSACTION_APPROVED,2))
						{
							sprintf(strTemp, "Txn fall under OBS section, Txn decline based on OBS rule, OB service: %s, OB result: %s",
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
							ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
																				 "TxDataServer_Continue_With_Pin_Processing",
																				 GS_TXN_TRANSACTION_NOT_ALLOWED,
																				 obs01_data.response_code) ;

							memset(Auth_Tx.TLF01_details.response_text,0x00,sizeof(Auth_Tx.TLF01_details.response_text));
							strncpy(Auth_Tx.TLF01_details.response_code,obs01_data.response_code,2);
							strncpy(Auth_Tx.TLF01_details.response_text,obs01_data.description,40);
							return ReturnCode;
						}
						else
						{
							if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
							{
								memset(strTemp,0x00,sizeof(strTemp));
								sprintf(strTemp,"Transaction fall under OBS approved and cvv2  present, SLI=(%s), control byte is:(%s) "
										" OB service: %s, OB result: %s, OB response code: (%s)",
										Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
										Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
								txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
								ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
								return ReturnCode;
							}
							else
							{
								memset(strTemp,0x00,sizeof(strTemp));
								sprintf(strTemp,"Transaction fall under OBS approved and cvv2 not present  falls in BAU processing, SLI=(%s), control byte is:(%s),"
										" OB service: %s, OB result: %s, OB response code: (%s)",
										Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
										Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
								txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
								 TxAuthorize_Update_Private_Ccf03p_Structure () ;
								 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
								 return ReturnCode;
							}
						}
					}
					else
					{
#ifdef MC05_OCT2023
						//October 2023 mandated changes
						if(Auth_Tx.TLF01_details.product_codes[16].quantity[0] == 'E')
						{
							strncpy(Auth_Tx.TLF01_details.response_code,CVV2_MISMATCH,2);
							strncpy(Auth_Tx.TLF01_details.response_text,"Security violation",18);

							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp, "Security violation, OB service value(%.2s), OB result 1(%c)",
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity[0]);

							ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
																				 "TxDataServer_Continue_With_Pin_Processing",
																				  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
							return ReturnCode;
						}
#endif
#ifdef MC03_APRIL2024
						//April 2024 mandated changes
						else if(Auth_Tx.TLF01_details.product_codes[16].quantity[0] == 'F')
						{
							strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);
							strncpy(Auth_Tx.TLF01_details.response_text,"Failed MAC key validation",25);

							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp, "Failed MAC key validation, OB service value(%.2s), OB result (%c)",
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity[0]);

							ReturnCode = txservice_Generate_Usage_Error_Message (1,"",
																				 "TxDataServer_Continue_With_Pin_Processing",
																				  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
							strncpy(Auth_Tx.TLF01_details.response_text,"Failed MAC key validation",25);
							return ReturnCode;

						}
						else if(Auth_Tx.TLF01_details.product_codes[16].quantity[0] == 'P' )
						{
							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp,"Transaction fall under Passed mac key validation, SLI=(%s), control byte is:(%s) ,"
									" OB service: %s, OB result: %s, OB response code: (%s)",
									Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
							txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );

						}
#endif
						else
						{
							 // OBS configuration not present in DB
							strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);
							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp, "OBS config not present in DB (%.2s):%c",
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity[0]);
							strncpy(Auth_Tx.TLF01_details.response_text,strTemp,40);
							ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
																				 "TxDataServer_Continue_With_Pin_Processing",
																				  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
							return ReturnCode;

						}
					}
				}
				else
				{
					strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);
					memset(strTemp,0x00,sizeof(strTemp));
					sprintf(strTemp, "OBS value is other than 05 or 06:(%.2s)",
							Auth_Tx.TLF01_details.product_codes[19].quantity);
					strncpy(Auth_Tx.TLF01_details.response_text,strTemp,40);
					ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
																		 "TxDataServer_Continue_With_Pin_Processing",
																		  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
					return ReturnCode;
				}

			}
		    else */

		    if	(true == txservice_MC_check_for_OBS_validation())
			{ /*Check OBS presence first then follow the BR */
				rc = txservice_read_OBS_record_for_MC();
				if( rc == PTEMSG_OK && obs01_data.response_code[0] != '\0')
				{
					if(0 != strncmp(obs01_data.response_code,TRANSACTION_APPROVED,2))
					{
						sprintf(strTemp, "Txn fall under OBS section, Txn decline based on OBS rule, OB service: %s, OB result: %s",
								Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			 "TxDataServer_Continue_With_Pin_Processing",
																			 GS_TXN_TRANSACTION_NOT_ALLOWED,
																			 obs01_data.response_code) ;

						memset(Auth_Tx.TLF01_details.response_text,0x00,sizeof(Auth_Tx.TLF01_details.response_text));
						strncpy(Auth_Tx.TLF01_details.response_code,obs01_data.response_code,2);
						strncpy(Auth_Tx.TLF01_details.response_text,obs01_data.description,40);
						return ReturnCode;
					}
					else
					{/*Approved RC*/
						if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
						{
							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp,"Transaction fall under OBS approved and cvv2  present, SLI=(%s), control byte is:(%s) "
									" OB service: %s, OB result: %s, OB response code: (%s)",
									Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
							txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
							ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							return ReturnCode;
						}
						else
						{
							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp,"Transaction fall under OBS approved and cvv2 not present  falls in BAU processing, SLI=(%s), control byte is:(%s),"
									" OB service: %s, OB result: %s, OB response code: (%s)",
									Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
							txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
							 TxAuthorize_Update_Private_Ccf03p_Structure () ;
							 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							 return ReturnCode;
						}
					}
				}
				else
				{
#ifdef MC05_OCT2023
					if(Auth_Tx.TLF01_details.product_codes[16].quantity[0] == 'E')
					{
						strncpy(Auth_Tx.TLF01_details.response_code,CVV2_MISMATCH,2);
						strncpy(Auth_Tx.TLF01_details.response_text,"Security violation",18);

						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp, "Security violation, OB service value(%.2s), OB result 1(%c)",
								Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity[0]);

						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			 "TxDataServer_Continue_With_Pin_Processing",
																			  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
						return ReturnCode;
					}
#endif
#ifdef MC03_APRIL2024
					else if(Auth_Tx.TLF01_details.product_codes[16].quantity[0] == 'F')
					{
						strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);
						strncpy(Auth_Tx.TLF01_details.response_text,"Failed MAC key validation",25);

						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp, "Failed MAC key validation, OB service value(%.2s), OB result (%c)",
								Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity[0]);

						ReturnCode = txservice_Generate_Usage_Error_Message (2,"",
																			 "TxDataServer_Continue_With_Pin_Processing",
																			  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
						strncpy(Auth_Tx.TLF01_details.response_text,"Failed MAC key validation",25);
						return ReturnCode;

					}
					else if(Auth_Tx.TLF01_details.product_codes[16].quantity[0] == 'P' )
					{
						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp,"Transaction fall under Passed mac key validation, SLI=(%s), control byte is:(%s) ,"
								" OB service: %s, OB result: %s, OB response code: (%s)",
								Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
								Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
						txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
						/*For approved case check for CVV2 if present validate else normal BAU */
						if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
						{
							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp,"Transaction fall under OBS approved and cvv2  present, SLI=(%s), control byte is:(%s) "
									" OB service: %s, OB result: %s, OB response code: (%s)",
									Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
							txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
							ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							return ReturnCode;
						}
						else
						{
							memset(strTemp,0x00,sizeof(strTemp));
							sprintf(strTemp,"Transaction fall under OBS approved and cvv2 not present  falls in BAU processing, SLI=(%s), control byte is:(%s),"
									" OB service: %s, OB result: %s, OB response code: (%s)",
									Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,
									Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity,Auth_Tx.TLF01_details.response_code);
							txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
							 TxAuthorize_Update_Private_Ccf03p_Structure () ;
							 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							 return ReturnCode;
						}

					}
#endif
					else
					{
						 /* OBS configuration not present in DB*/
						strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);
						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp, "OBS config not present in DB (%.2s):%c",
								Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity[0]);
						strncpy(Auth_Tx.TLF01_details.response_text,strTemp,40);
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			 "TxDataServer_Continue_With_Pin_Processing",
																			  GS_TXN_TRANSACTION_NOT_ALLOWED,Auth_Tx.TLF01_details.response_code) ;
						return ReturnCode;

					}
				}
			}
			else if(true == txservice_MC_check_control_byte_and_SLI_combo_for_declining_3DS2())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);

				sprintf(strTemp, "Could not be authenticated either by ACS or MC Authentication Stand in Attempts. SLI(2%s),control byte(%s)",
						Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity);
				txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																	 "TxDataServer_Continue_With_Pin_Processing",
																	 GS_TXN_TRANSACTION_NOT_ALLOWED,
																	 Auth_Tx.TLF01_details.response_code) ;

				return ReturnCode;
			}
			else if(true == txservice_MC_check_control_byte_and_SLI_combo_for_AAV_validation())
			{
				ReturnCode = txservice_self_AAV_Validation_For_MC();
				return ReturnCode;
			}
			else if(true == txservice_MC_check_control_byte_and_SLI_combo_for_CVC2_and_BAU_validation())
			{
				if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
				{
					sprintf(strTemp, "Transaction fall under control byte and SLI combo for CVV2 followed by BAU processing. SLI(2%s),control byte(%s)",
											Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity);
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					/*  check for proper MT for CVC2 validation*/
					ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION);
					return ReturnCode;
				}
				else
				{
					sprintf(strTemp, "Transaction fall under control byte and SLI combo for BAU and CVV2 not present. SLI(2%s),control byte(%s)",
							Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity);
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );

					 TxAuthorize_Update_Private_Ccf03p_Structure () ;
					 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					 return ReturnCode;
				}
			}
			else
			{
				if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
				{
					sprintf(strTemp, "Transaction fall under CVV2 followed by BAU processing. SLI(2%s),control byte(%s)",
											Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity);
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
/*  check for proper MT for CVC2 validation*/
					ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION);
					return ReturnCode;
				}
				/*else if(Auth_Tx.TLF01_details.product_codes[11].quantity[0] == '\0' &&
						(strlen(Auth_Tx.TLF01_details.workstation) == 0 || Auth_Tx.TLF01_details.workstation[0] == '\0') )
				{ // pr gram protocol not available in the transaction then go for 3DS 1.0
					sprintf(strTemp, "Transaction fall under 3DS 1, DE48 SE66 program protocol not present");
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
				}*/
				else
				{
					sprintf(strTemp, "Transaction fall under BAU and CVV2 not present. SLI(2%s),control byte(%s)",
							Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity);
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );

					 TxAuthorize_Update_Private_Ccf03p_Structure () ;
					 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					 return ReturnCode;
				}
			}


			 /*
			if(Auth_Tx.TLF01_details.product_codes[11].quantity[0] == '2')
			{ // MC 3D 2.0
				txcntl_log_message( 1, 1,"MC 3DS 2.0 transaction", "TxDataServer_Continue_With_Pin_Processing", 1 );
				if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_FULLYAUTHENTICATED,2) &&
					true ==	txservice_check_control_bytes_validation())
				{
					//It is an temporary work around for MC HMAC validation, it will be used until
					 //HMAC validation functionality working as expected
					memset(strTemp,0x00,sizeof(strTemp));
					sprintf(strTemp,"Transaction fall under work around changes, SLI=(%s) and control byte is:(%s)",Auth_Tx.TLF01_details.profile,
							Auth_Tx.TLF01_details.product_codes[5].quantity);
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode=	txservice_self_AAV_Validation_For_MC();
				}
				else if(true == txservice_Check_AVV_IVV_value_to_skip_CAVV_validation())
				{
					// Skip the UCAF validation based on control bytes

					 // if case is approved add cvc2 validation if present

					if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
					{
						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp,"Transaction fall under control byte section and cvc2 present, SLI=(%s), control byte is:(%s) and cvc2_data is:(%s)",
								Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,Auth_Tx.TLF01_details.cvc_data);
						txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
						ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
					}
					else
					{
						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp,"Transaction fall under control byte section and cvc2 not present, falls in BAU processing, SLI=(%s), control byte is:(%s) and cvc2_data is:(%s)",
								Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,Auth_Tx.TLF01_details.cvc_data);
						txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
						 TxAuthorize_Update_Private_Ccf03p_Structure () ;
						 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					}
				}
				else if('\0' != Auth_Tx.TLF01_details.product_codes[19].quantity[0] &&
						'\0' != Auth_Tx.TLF01_details.product_codes[16].quantity[0])
				{
					// OBS details are present
					total_count = atoi(obs01_list.num_returned);
					for(index =0; index<total_count; index++)
					{
						if( 0 == strncmp(Auth_Tx.TLF01_details.product_codes[19].quantity,obs01_list.obs01_record[index].on_behalf_service,2) &&
								obs01_list.obs01_record[index].on_behalf_result[0] == Auth_Tx.TLF01_details.product_codes[16].quantity[0] )
						{
							strncpy(Auth_Tx.TLF01_details.response_code,obs01_list.obs01_record[index].response_code,2);
							strncpy(Auth_Tx.TLF01_details.response_text,obs01_list.obs01_record[index].description,40);
							if(0 != strncmp(Auth_Tx.TLF01_details.response_code,TRANSACTION_APPROVED,2))
							{
								sprintf(strTemp, "Txn fall under OBS section, Txn decline based on OBS rule, OB service: %s, OB result: %s",
										Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
								txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
								ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
																					 "TxDataServer_Continue_With_Pin_Processing",
																					 GS_TXN_TRANSACTION_NOT_ALLOWED,
																					 Auth_Tx.TLF01_details.response_code) ;

								return ReturnCode;
							}

							 //if case is approved add cvc2 validation if present

							if(Auth_Tx.TLF01_details.cvc_data[0] != '\0')
							{
								memset(strTemp,0x00,sizeof(strTemp));
									sprintf(strTemp,"Transaction fall under OBS section section and cvc2  present, SLI=(%s), control byte is:(%s) and cvc2_data is:(%s),"
											" OB service: %s, OB result: %s",
											Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,Auth_Tx.TLF01_details.cvc_data,
											Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
									txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
								ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							}
							else
							{
								memset(strTemp,0x00,sizeof(strTemp));
								sprintf(strTemp,"Transaction fall under OBS section and cvc2 not present  falls in BAU processing, SLI=(%s), control byte is:(%s) and cvc2_data is:(%s),"
										" OB service: %s, OB result: %s",
										Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,Auth_Tx.TLF01_details.cvc_data,
										Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
								txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
								 TxAuthorize_Update_Private_Ccf03p_Structure () ;
								 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							}
							break;
						}
					}
					if(index >= total_count)
					{
						// the OBS value not present in the OBS01 table
						memset(strTemp,0x00,sizeof(strTemp));
						sprintf(strTemp, "Transaction fall under OBS section, OBS rule not present in DB, go for IAV validation followed by cvc2, OB service: %s, OB result: %s",
								Auth_Tx.TLF01_details.product_codes[19].quantity,Auth_Tx.TLF01_details.product_codes[16].quantity);
						txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
						ReturnCode=	txservice_self_AAV_Validation_For_MC();
					}
				}
				else
				{
					memset(strTemp,0x00,sizeof(strTemp));
					sprintf(strTemp,"Transaction did not fall under any section, go for IAV validation followed by cvc2, SLI=(%s), control byte is:(%s) and cvc2_data is:(%s)",
							Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[5].quantity,Auth_Tx.TLF01_details.cvc_data);
						txcntl_log_message( 1, 1,"MC 3DS 1.0 transaction", "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode =txservice_self_AAV_Validation_For_MC();
				}
			}
			else
			{ // MC 3D 1.0
				txcntl_log_message( 1, 1,"MC 3DS 1.0 transaction", "TxDataServer_Continue_With_Pin_Processing", 1 );
				if((0 == strncmp(Auth_Tx.TLF01_details.profile,"11",2) &&
				   '1' == Auth_Tx.TLF01_details.product_codes[4].quantity[1]))
				{
					// CAVV verification was done by MC and its successful.Proceed with authorization
					 // Repopulate the private ccf03p structure.
					 txcntl_log_message( 1, 1,"MC 3DS 1.0 transaction fall under BAU processing", "TxDataServer_Continue_With_Pin_Processing", 1 );
					 TxAuthorize_Update_Private_Ccf03p_Structure () ;
					 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );

				}
				else
				{
					txcntl_log_message( 1, 1,"MC 3DS 1.0 transaction fall under HSM validation", "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
				}
			}
		*/
		}

    	/*
    	 * For ECI 05: if,
    	 *
    	 * 	 F60.2.8 ECI = 05 (issuer has authenticated through UnionPay 3-D Secure Authentication Service)
    	 * 	 F61.6 Usage 5 (AM) Byte 11 AV Verification = 1 (AV present)
		 *	 F61.6 Usage 5 (AM) Authentication Value AV generator indicator= 1 (issuer ACS)
		 *	 The AV flag in the GUI is enabled.
    	 *
    	 * ATP Must do AVN validation. If the AVN validation is successful, ATP will then check the CVN2 flag, if it is enabled then ATP will do CVN2 validation.
    	 * If it is disabled, ATP will proceed with BAU.
    	 *
    	 * For ECI06: if,
    	 *    ECI = 06 (merchant attempt to authenticate)
 	 	 *	  AV Authentication Result = 06 (attempt authentication)
 	 	 *	  AV generator indicator = 2 (UnionPay attempts service)
 	 	 *	  AV verification indicator = 2 (2- AV present, validated by UnionPay (The validated AV is transmitted to the Issuer),
 	 	 *	  								  - As this value is expected for UPI when doing an attempt)
    	 *
    	 * The AVN validation has already in done by UPI, ATP will not validate AVN. If the Authentication verify attempt flag is set to enabled.
    	 * ATP will check if the CVN2 is present in the transaction. If present, ATP does the CVN2 validation. If not, ATP proceeds with BAU.
    	 * If Authentication verify attempt flag is set to disabled, ATP will decline the transaction with the RC configured in the GUI.
    	 *
    	 * ECI 07: For eci07, ATP will check if CVN2 is present, if present it will validate and if not it will follow BAU.
    	 *
    	 * Then CUP3DS is taken care
    	 *
    	 *
    	 *
    	 */
		else if ((0 == strncmp(Auth_Tx.BIN01_details.card_family,CARD_CUP, 2)))
		{

			if((true == txservice_Is_Txn_Qualify_For_Self_AV_Validation()))
			{
				ReturnCode = TxAuthorize_Process_Security (MT_NSP_VERIFY_3D_SECURE_2) ;
				return ReturnCode ;

			}
			else if(true == txservice_check_txn_for_CUP_authentication_attempt())
			{
				if(cup_3ds2.cup_authentication_attempt_verify_flag[0] == 'A')
				{
					if(Auth_Tx.TLF01_details.resp_source[CUP_CVN2_INDICATOR_POS] == '1' &&
					   Auth_Tx.future_use_2[CUP_DE61_CVN2_POS] != '\0' )
					{
						ReturnCode = TxAuthorize_Process_Security (NSP_CUP_ECOM_CVN2) ;
						return ReturnCode ;
					}
					else
					{
					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					return ReturnCode;
				}
				}
				else
				{
						strncpy(Auth_Tx.TLF01_details.response_code,cup_3ds2.authentication_attempt_rc,2);
						strcpy(Auth_Tx.TLF01_details.response_text,"AuthN attempt rjctd/could not be prfmd");
						strcpy(strTemp,Auth_Tx.TLF01_details.response_text);
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
												"TxDataServer_Continue_With_Pin_Processing",
												GS_TXN_TRANSACTION_NOT_ALLOWED,
												Auth_Tx.TLF01_details.response_code ) ;

						return ReturnCode ;

				}
			}
			else if(0 == strncmp(Auth_Tx.TLF01_details.profile,"07",2))
			{
				if(Auth_Tx.TLF01_details.resp_source[CUP_CVN2_INDICATOR_POS] == '1' &&
				   Auth_Tx.future_use_2[CUP_DE61_CVN2_POS] != '\0' )
				{
					ReturnCode = TxAuthorize_Process_Security (NSP_CUP_ECOM_CVN2) ;
					return ReturnCode ;
				}
				else
				{
					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
									return ReturnCode;
				}

			}
			else if(true == txservice_Check_If_CUP_ECOM())
			{
				ReturnCode = txservice_Validate_CUP_ECOM_TRAN();
			}
			else
			{
				ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							    return ReturnCode;
			}

		}
		else if ((0 == strncmp(Auth_Tx.BIN01_details.card_family,CARD_DINERS, 2)) &&
						 (true == txservice_Check_If_DCI_ECOM_Productbuy()))
		{
			memset(Auth_Tx.TLF01_details.product_codes[12].code,0x00,
								sizeof(Auth_Tx.TLF01_details.product_codes[12].code));
			if (true == txservice_Check_from_DCI_Protectbuy_3DS2_txn())
			{
				//Protect Buy 3DS 2.0
				/* 1. self validation
				 * 	a. success, go ahead for CVV2 validation
				 * 	b. failure, decline the transaction
				 * */
				/* As of now its declined right away
				 * once command found the below logic will be changed */

				if (true == txservice_Check_DCI_3DS2_txn_for_fully_authentication())
				{
					ReturnCode = TxAuthorize_Process_Security (MT_NSP_VERIFY_3D_SECURE_2) ;
					return ReturnCode;
				}
				else if (true == txservice_Check_DE122_AuthnResultCode_value_to_directly_decline_txn())
				{
					// CAVV verification was done by DCI and its failed. Decline the transaction
					strcpy(Auth_Tx.TLF01_details.response_text,
													"Authentication rjctd/could not be prfmd");
					strcpy(strTemp,Auth_Tx.TLF01_details.response_text);
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
													"TxDataServer_Continue_With_Pin_Processing",
													GS_TXN_TRANSACTION_NOT_ALLOWED,
													DO_NOT_HONOR ) ;
					strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
							diners_3ds2.authentication_result_code_ac, 3);
					return ReturnCode ;

				}
				else if (true == txservice_Check_DE122_AuthnResultCode_value_to_skip_CAVV_validation())
				{

					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					return ReturnCode;

				}
				else if (true == txservice_Check_DCI_3DS2_txn_for_authentication_attempt())
				{

					if(diners_3ds2.authentication_attempt_verify_flag[0] == 'A')
					{
						    ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							return ReturnCode;
					}
					else
					{
							strcpy(Auth_Tx.TLF01_details.response_text,
													"AuthN attempt rjctd/could not be prfmd");
							strcpy(strTemp,Auth_Tx.TLF01_details.response_text);
							ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
												"TxDataServer_Continue_With_Pin_Processing",
												GS_TXN_TRANSACTION_NOT_ALLOWED,
												DO_NOT_HONOR ) ;
							strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
									diners_3ds2.authentication_attmpt_ac, 3);
							return ReturnCode ;
					}

				}
				else
				{
					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					return ReturnCode;

				}

			}
			else
			{
				//Protect Buy 3DS 1.0.2
				ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
			}

		}
#if JCB_3DS2
		else if (0 == strncmp(Auth_Tx.BIN01_details.card_family,CARD_JCB, 2))
		{
			if(0 == strncmp(Auth_Tx.TLF01_details.profile,"05",2) || 0 == strncmp(Auth_Tx.TLF01_details.profile,"06",2) ||
			   0 == strncmp(Auth_Tx.TLF01_details.profile,"07",2))
			{
				if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"081",3) == 0 ||
				   strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"001",3) == 0 ||
				    strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"010",3) == 0)
				{
					/*its a 3DS transaction go for further processing
					 * DE48 tag 02 is present then POS entry mode should be either Xâ€™01â€™(Manual entry), Xâ€™81â€™(E-comerce) or Xâ€™10(card on file)
					 * */
				}
				else
				{
					sprintf(strTemp, "Invalid POS entry mode, ECI val is:%s,pos entry mode is:%s",Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.pos_entry_mode);
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		 "TxDataServer_Continue_With_Pin_Processing",
																		 GS_TXN_TRANSACTION_NOT_ALLOWED,
																		 cavv_verification_rc) ;
					return ReturnCode ;
				}
			}

			/*
			 * ECI 05:

				Case 1: If,
				â€¢	Bit 48 Tag 02 CAVV Position 1 is â€œ00â€�,
				â€¢	Bit48 Tag02 CAVV Position 2 is either 01,02,03,04 or 05,
				â€¢	Bit48 Tag02 CAVV Position 3 is 01

				ATP will do the CAVV validation. After the CAVV processing ATP will follow below steps:

				â€¢	On CAVV validation failure ATP will decline the transaction with RC configured in the tf.ini.
				â€¢	On CAVV validation success, ATP will check the CAV2 validation flag configured in tf.ini, if enabled and CAV2 is present ATP will do CAV2 validation. If flag disabled ATP will go with normal BAU.

				Case 2: If transaction does not qualify for the CAVV validation, ATP will check the Bit 48 Tag 02 CAVV Position 1(3D Secure Authentication Results Code) stored in the DB and does the validation as mentioned below:
				â€¢	3D Secure Authentication Results Code received in the transaction is present in DB:
				o	If RC configured is APPROVED: ATP will check the CAV2 validation flag configured in tf.ini, if enabled and CAV2 is present ATP will do CAV2 validation. If flag disabled ATP will go with normal BAU.
				o	If RC configured is Declined: ATP will decline the transaction with configured RC.
				Case 3: If transaction does not qualify for any of the above conditions, ATP will decline the transaction with configured RC in the tf.ini.

				ECI 06:

				Case 1: If,
				â€¢	Bit 48 Tag 02 ECI = 06 (attempt) and Bit 48 Tag 02 CAVV Position 1 (3D Secure authentication results code) value of 07 (attempt processed by ACS) or 08 (attempt processed by DS)
				â€¢	Bit 48 Tag 02 CAVV Position 2 (Second Factor Authentication) = 09 (EMV 3DS attempt).

				ATP checks the response code configured in the tf.ini(AUTHENTICATION_ATTEMPT_RESULT_CODE),

				â€¢	If RC is APPROVED(â€œ00â€�):
				o	ATP will proceed with CAV2 validation if present else ATP will do normal BAU.
				â€¢	If RC is not APPROVED:
				o	ATP will directly decline the transaction with comment of "3DS 2.0 Attempt decline is active".

				Case 2: If transaction does not qualify for above conditions, ATP will decline the transaction with configured RC in tf.ini.

				ECI 07:

				â€¢	ATP will proceed with CAV2 validation if present else ATP will do normal BAU.

						ECI is other than 05,06 and 07, ATP will do 3DS 1 validation.
			 * */


			if(0 == strncmp(Auth_Tx.TLF01_details.profile,"05",2))
			{
				/* CAVV verification followed by CAV2 if present
				 *
				 */
				if(true == txservice_Is_Txn_Qualify_For_Self_JCB_CAVV_Validation())
				{
					sprintf(strTemp, "Txn fall under CAVV verification case, ECI(%s), 1st 12 byte from CAVV(%s)",Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[3].amount);
					txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = TxAuthorize_Process_Security (MT_NSP_VERIFY_3D_SECURE_2) ;
					return ReturnCode ;
				}
				else if(Auth_Tx.TLF01_details.product_codes[3].amount[0] != '\0' &&
						Auth_Tx.TLF01_details.product_codes[3].amount[1] != '\0')
				{
					sprintf(strTemp, "Txn fall under 3D Secure Authentication Results Code check, ECI(%s),  CAVV pos 1(%.2s)",Auth_Tx.TLF01_details.profile,Auth_Tx.TLF01_details.product_codes[3].amount);
					txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					jcb3ds2_total_count = atoi(jcb3ds2_list.num_returned);
					for(index =0; index<jcb3ds2_total_count; index++)
					{
							if( 0 == strncmp(Auth_Tx.TLF01_details.product_codes[3].amount,jcb3ds2_list.jcb3ds2_record[index].auth_result_code_value,2))
							{
								if(0== strncmp(jcb3ds2_list.jcb3ds2_record[index].response_code,"00",2))
								{
									if(cav2_validation_flag == '1')
									{
										if(Auth_Tx.TLF01_details.cvc_data[0] != '\0' )
										{
											sprintf(strTemp, "ECI is 05, txn did not qualify for CAVV but for ATP-3DS-3 req, CVA2 is present, cavv pos 1 is(%.2s)",Auth_Tx.TLF01_details.product_codes[3].amount);
											txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
											ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
											return ReturnCode ;

										}
										else
										{
											sprintf(strTemp, "ECI is 05, CAV2 not present, cavv pos 1 is(%.2s)",Auth_Tx.TLF01_details.product_codes[3].amount);
											txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
											ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
											return ReturnCode;
										}
									}
									else
									{
										sprintf(strTemp, "ECI is 05, CAV2 flag disabled, going for BAU, cavv pos 1 is(%.2s)",Auth_Tx.TLF01_details.product_codes[3].amount);
										txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
										ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
										return ReturnCode;
									}
								}
								else
								{
										strncpy(Auth_Tx.TLF01_details.response_code,jcb3ds2_list.jcb3ds2_record[index].response_code,2);
										sprintf(strTemp, "ECI 05,Auth res chk val is in DB,decline");
										ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																									 "TxDataServer_Continue_With_Pin_Processing",
																									 GS_TXN_TRANSACTION_NOT_ALLOWED,
																									 Auth_Tx.TLF01_details.response_code) ;
										return ReturnCode;
								}

							}
					}
					if(index >= jcb3ds2_total_count )
					{
						sprintf(strTemp, "ECI 05,3D Auth res chk value not in DB, CAVV pos 1 not present in DB(%.2s)",Auth_Tx.TLF01_details.product_codes[3].amount);
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			 "TxDataServer_Continue_With_Pin_Processing",
																			 GS_TXN_TRANSACTION_NOT_ALLOWED,
																			 cavv_verification_rc) ;
						return ReturnCode;
					}
				}
				else
				{
						sprintf(strTemp, "ECI 05, txn not qlfy CAVV & Auth res chk");
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																				 "TxDataServer_Continue_With_Pin_Processing",
																				 GS_TXN_TRANSACTION_NOT_ALLOWED,
																				 cavv_verification_rc) ;
						return ReturnCode;
				}
			}
			else if(0 == strncmp(Auth_Tx.TLF01_details.profile,"06",2))
			{
				if(true == txservice_check_txn_for_JCB_authentication_attempt())
				{
					if(0 != strncmp(Authentication_attempt_result_code,"00",2))
					{
						strncpy(Auth_Tx.TLF01_details.response_code,Authentication_attempt_result_code,2);
						sprintf(strTemp, "ECI is 06,rc is decline, pos 1 & 2 (%.4)",Auth_Tx.TLF01_details.product_codes[3].amount);
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
														"TxDataServer_Continue_With_Pin_Processing",
																	GS_TXN_TRANSACTION_NOT_ALLOWED,
																	Auth_Tx.TLF01_details.response_code ) ;
						return ReturnCode;
					}
					else
					{
						if(Auth_Tx.TLF01_details.cvc_data[0] != '\0' )
						{
							sprintf(strTemp, "ECI is 06, txn did not qualify authentication_attempt validation, CAV2 present");
							txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
							ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							return ReturnCode ;

						}
						else
						{
							sprintf(strTemp, "ECI is 06, txn did not qualify authentication_attempt validation, CAV2 not present");
							txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
							ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
							return ReturnCode;
						}
					}
				}
				else
				{

					strncpy(Auth_Tx.TLF01_details.response_code,cavv_verification_rc,2);
					sprintf(strTemp, "ECI is 06, does not qualify for attempt validation",cavv_verification_rc);
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
													"TxDataServer_Continue_With_Pin_Processing",
																GS_TXN_TRANSACTION_NOT_ALLOWED,
																Auth_Tx.TLF01_details.response_code ) ;
					return ReturnCode ;
				}

			}
			else if(0 == strncmp(Auth_Tx.TLF01_details.profile,"07",2))
			{
				if(Auth_Tx.TLF01_details.cvc_data[0] != '\0' )
				{
					sprintf(strTemp, "ECI is 07, CAV2 present ");
					txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
					return ReturnCode ;

				}
				else
				{
					sprintf(strTemp, "ECI is 07, CAV2 not present go with normal BAU");
					txcntl_log_message( 2, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
					return ReturnCode;
				}
			}
			else
			{
				/*3DS 1*/
				ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
				return ReturnCode ;
			}
		}
#endif
		else
		{
			if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			{
				if( Auth_Tx.TLF01_details.cvc_data[0] != '\0')
				{
					memset(strTemp,0x00,sizeof(strTemp));
					strcpy(strTemp, "CAVV present, did not fall in main MC block,but CVV2 is present");
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION);
					return ReturnCode;
				}
				else
				{
					/*decline
					 *
					 * CAVV is present but DE61.4 !=5 or POS entry mode is != 081 or 082 and CVV2 is also not present hence decline
					 * */
					strncpy(Auth_Tx.TLF01_details.response_code,mc_3ds2_delcine_rc,2);
					memset(strTemp,0x00,sizeof(strTemp));
										strcpy(strTemp, "CAVV present, did not fall in main MC block");
					txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
					ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																		 "TxDataServer_Continue_With_Pin_Processing",
																		 GS_TXN_TRANSACTION_NOT_ALLOWED,
																		 Auth_Tx.TLF01_details.response_code) ;
					return ReturnCode;
				}
			}
			else
			{
				ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_3D_SECURE) ;
			}
		}
	}
     else if ( (Auth_Tx.EMV_details.emv_block[0] != 0x00) ||
        (Auth_Tx.EMV_details.emv_block[1] != 0x00)  )
	 {
			if(strlen(Auth_Tx.EMV_details.app_crypto) > 0 )// Check if EMV data is there
			{
				ReturnCode = TxAuthorize_Process_Security (NSP_VERIFY_ARQC) ;
				//ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
			}
			else
			{
					strcpy(strTemp, "Invalid EMV transaction,No ARPC present");
					strncpy(Auth_Tx.TLF01_details.security_response_code,
							DINERS_RACAL_DECLINE_RESPONSE,
							DINERS_RACAL_RESPONSE_LEN);
	            	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
	            	   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
					{
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			 "TxDataServer_Continue_With_Pin_Processing",
																			 GS_TXN_TRANSACTION_NOT_ALLOWED,
																			 mc_3ds2_delcine_rc) ;
					}
					else
					{
						ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																			 "TxDataServer_Continue_With_Pin_Processing",
																			 GS_TXN_TRANSACTION_NOT_ALLOWED,
																			 DO_NOT_HONOR) ;
					}


				return ReturnCode;
			}
	  }
  	else if ((Auth_Tx.TLF01_details.verify_mag_stripe[0] == 'Y')  &&  //check mag stripe is yes
       (Auth_Tx.CAF01_details.cvv_processing[0]    != '0'))     // GUI says Yes
   {
     
	  /* The Security module will parse the track1 or track2 data
       * according to the settings in the card profile.
       */
      if (!txservice_If_Tx_Cannot_Be_Declined() &&
           txservice_If_Tx_Requires_CVV_Validation() )
      {
         /* ReturnCode will be NO_AUTH_ERROR when successfully sent to the 
          * crypto processor else will be SYSTEM_ERROR or 
          * ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR
          */
         ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
      }
      else
      {
         /* Get the MCC Limits, then authorize the transaction. */
         ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
      }
   }
  	else if((true == txservice_Check_If_Validate_CVN2_FOR_CUP_Non_ECOM_TRAN()) &&
			(true == txservice_Check_If_CUP_Transaction()) &&
			(Auth_Tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY))
	{
		ReturnCode = TxAuthorize_Process_Security (NSP_CUP_ECOM_CVN2) ;
		return ReturnCode ;
	}
	else if((Auth_Tx.CAF01_details.last_maint[0] & 0x40) == 0x40) // We need to check POS entry mode check and see of POS ENTRY MODES are mathcing
   {
	  
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0010") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x01) == 0x01)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0011") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x02) == 0x02)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0012") == 0)
	  {
		  if(0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)&&
			 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6))
		  {
			  	 //Its RPS transaction , we dont need to CVV validation
			  doCvv = 0;
		  }
		  else if((Auth_Tx.CAF01_details.last_maint[1] & 0x04) == 0x04)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0020") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x08) == 0x08)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x10) == 0x10)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x20) == 0x20)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0050") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x40) == 0x40)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0051") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x01) == 0x01)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0052") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x02) == 0x02)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0800") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x04) == 0x04)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0801") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x08) == 0x08)
		  {
			  doCvv = 1;
		  }
	  }
	  if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0802") == 0)
	  {
		  if((Auth_Tx.CAF01_details.last_maint[1] & 0x10) == 0x10)
		  {
			  doCvv = 1;
		  }
	  }
	  if(doCvv == 1)
	  {
		ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
	  }
	  else
	  {
		 strcpy(Auth_Tx.TLF01_details.security_response_code,"00");
        ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
	  }
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY || Auth_Tx.TLF01_details.tx_key == AUTH_RELOAD)
   {
      strcpy(Auth_Tx.TLF01_details.total_amount, Auth_Tx.ACF01_details.available);
      txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
      uag_rcode = TxAuthorize_GetAuthId( ST2_NONE );
      if ( PTEMSG_OK != uag_rcode )
      {
         /* XIPC Error */
         ReturnCode = SYSTEM_ERROR;
      }
      else
      {
         TxDataServer_DBReply_Continue();

         /* There is a path through CCF03P that may try to send to the
          * originator again.  Set tran_id to '1' as a flag so it knows
          * not to send it.  It was sent by the above statement.
          */
         strcpy( Auth_Tx.TLF01_details.primary_key.transaction_id, "1" );
         ReturnCode = NO_AUTH_ERROR;
      }
   }
   else
   {
      /* Get the MCC Limits, then authorize the transaction. */
      ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
   }
   return ReturnCode ;
} /* TxDataServer_Continue_With_Pin_Processing */

BYTE TxDataServer_Validate_Tran_Limit()
{
   BYTE ReturnCode = true;

   double  nTlf01_Total_Amt          = 0.0;
   double  nCcf02p_Peso_Tran_Limit   = 0.0;
   double  nCcf02p_Dollar_Tran_Limit = 0.0;

   CHAR    strTemp[256] = {0};

   nTlf01_Total_Amt = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

   /*Verify the dollar or peso ccf02p per transaction limit is not exceeded*/
   /*Use Dollar limit for US currency, else for all other currencies, use the Peso limit*/
   if(strcmp(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY) == 0)
   {
      /*If dollar tran limit is empty or blank, bypass check*/
      if(Auth_Tx.CCF02P_details.dollar_tran_limit[0] != 0x20 &&
         Auth_Tx.CCF02P_details.dollar_tran_limit[0] != 0x00)
      {
         nCcf02p_Dollar_Tran_Limit = (strtod (Auth_Tx.CCF02P_details.dollar_tran_limit, 0) / 100.0) ;

         if (nTlf01_Total_Amt > nCcf02p_Dollar_Tran_Limit)
            ReturnCode = false;
      }
   }
   else
   {
      /*If peso tran limit is empty or blank, bypass check*/
      if(Auth_Tx.CCF02P_details.peso_tran_limit[0] != 0x20 &&
         Auth_Tx.CCF02P_details.peso_tran_limit[0] != 0x00)
      {
         nCcf02p_Peso_Tran_Limit = (strtod (Auth_Tx.CCF02P_details.peso_tran_limit, 0) / 100.0) ;

         if(nTlf01_Total_Amt > nCcf02p_Peso_Tran_Limit)
         ReturnCode = false;
      }
   }

   if(ReturnCode == false)
   {
      sprintf( strTemp,
              "Card Amt Limit Failed for MCC %s",
               Auth_Tx.TLF01_details.category_code );

      ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                "Validate_Purchase Card Amount_Limit",
                                 GS_TXN_PURCHASE_CARD_CHECK_FAILED, 
                                 DO_NOT_HONOR );
      return ReturnCode ;
   }

   return NO_AUTH_ERROR ;

} /*TxDataServer_Validate_Tran_Limit */


/*
BYTE TxDataServer_Validate_Category_Code_Range (CCF04 sCCF04) 
{
	int nIndex = 0 ;
	int nCategoryCodeFound = false ;
	BYTE card_category_code [10] = {0} ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;

	strcpy (card_category_code, Auth_Tx.TLF01_details.category_code) ;

  	for (nIndex = 0 ; nIndex < 20 && !nCategoryCodeFound ; nIndex++)
	{
		if ((atoi(card_category_code) >= atoi(sCCF04.range[nIndex].mcc_lo_range)) &&
			 (atoi(card_category_code) <= atoi(sCCF04.range[nIndex].mcc_hi_range)) )
        {
		    // check validated 
		    nCategoryCodeFound = true ;
        }
	}
	if (!nCategoryCodeFound)
	{
		sprintf (strTemp, 
					"card check failed,category code %s\n", 
					card_category_code) ;
		//*****
		ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp, 
														  "Validate_Category_Code_Range",
														  GS_TXN_PURCHASE_CARD_CHECK_FAILED, 
														  REFER_TO_CARD_ISSUER) ;
		return ReturnCode ;
		//*****
		
	}
	return NO_AUTH_ERROR ;
}*/ /* TxDataServer_Validate_Category_Code_Range */

BYTE TxDataServer_Continue_With_Transaction_Processing (void)
{
	BYTE ReturnCode = 0 ;
	BOOLEAN bReturnCode = true ;
	BYTE TransactionOrigin = txservice_GetTransactionOrigin() ;

	if (TransactionOrigin == LOCAL_ORIGIN)
	{
      	if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE || 
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND ||
         Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE	||
         Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND ||
		 Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD)
		{
			/* This is because only local origin transactions would be
			   in BCH10 - the transaction would terminate at this stage
				regardless of whether it was found in BCH10. If it was found
				then then the response code would be 00 otherwise an error response
				code */
		ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
		return ReturnCode ;

			/*txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
			return NO_AUTH_ERROR ;*/
		}
	}

	// Cadencie changes : ThoughtFocus Nirbhay : commented as 302 msg is routed directly from ncvis2 to ncvisa.
	//if(strncmp(Auth_Tx.TLF01_details.handler_queue, cadencie_host_que_name, 6) == 0 &&	// nirbhay Thoughtfocus: cadencie changes
	//	strcmp(Auth_Tx.TLF01_details.message_type, "0302") == 0)
	//{
	//	ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
	//	return ReturnCode ;
	//}

	if (txservice_Tx_Qualifies_To_Be_Completed_Before_Authorization())
	{
		/* These transactions are all approved by the authorizer */
		txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
		return NO_AUTH_ERROR ;
	}
	
	else if (txservice_If_Tx_Is_Treated_Like_A_Reversal ()) /* including Reversal Advices */
	{

		/* If the message type is a REVERSAL and the original Tx is a Sale type transaction 
			(first two bytes of the processing code of the REVERSAL tx is either "00",
			"30",or "01"), we check if the original Sale type transaction exists in BCH10 or NCF30
			with a "00" response code. If the original transaction is not a SALE type, then
			we do not have to reverse anything and we just return.

			Sale type transactions include AUTH_AUTHORIZATION, AUTH_PRE_AUTHORIZATION,
			AUTH_SALE, AUTH_CASH_ADVANCE, AUTH_QUASI_CASH, AUTH_OFFLINE_SALE.

			If the original Sale type transaction does not exist, it implies that the transaction
			never really made to completion, so the reversal does not have to undo the effect
			from that transaction and as such we return.

            If the original Sale type transaction exists, the dataserver returns a PTEMSG_OK
            to txcntl and also removes the original Sale type transaction from either BCH10 
            (for locally acquired transactions) or NCF30 (for switched-in transactions). The 
            reversal transaction never gets written to BCH10 or NCF30 (but transaction gets written 
            to TLF01)
		*/
		
		if (txservice_If_Reversed_Tx_Is_A_Sale_Type_Tx ())
		{
		    /* Reversal of a AUTH_AUTHORIZATION, AUTH_PRE_AUTHORIZATION,
			   AUTH_SALE, AUTH_CASH_ADVANCE, AUTH_QUASI_CASH, AUTH_OFFLINE_SALE 
			*/
            TransactionOrigin = txservice_GetTransactionOrigin () ;
            if (TransactionOrigin == LOCAL_ORIGIN)
            {
                if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                {
                    /* Reversal of a VOICE AUTHORIZED purchase, cash-advance, quasi-cash and deferred 
                       purchase transactions */
                    /* Retrieve the original transaction for the VOICE AUTHORIZATION transaction */
                    ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
                }
                else
                {
							if (txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice ())
							{
								/* The original transaction for the advice was processed
								   so we do not have to continue the transaction */
								txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED,TRANSACTION_APPROVED) ;
							}

                     ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
							return ReturnCode ;
                }
            }
            else if (TransactionOrigin == REMOTE_ORIGIN)
            {
				if (txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice ())
				{
					/* The original transaction for the advice was processed
					   so we do not have to continue the transaction */
					txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED,TRANSACTION_APPROVED) ;
					return NO_AUTH_ERROR ;
				}

                ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
				return ReturnCode ;
                /* bReturnCode = TxDataServer_Get_NCF30_Details (ST2_DB_SELECT_TLF01_BY_ORIGINAL_TRAN) ; */
            }
            
			return NO_AUTH_ERROR ;
		}
		else
		{
			/* Reversal of a AUTH_REFUND, AUTH_VOID_SALE, AUTH_VOID_REFUND
			   AUTH_OFFLINE_REFUND, AUTH_SALE_ADJUSTMENT, AUTH_REFUND_ADJUSTMENT etc */
			/* Complete the transaction and return */

         /* SCR 12502 - ThoughtFocus
          * Changing the behaviour of REVERSAL to process VOIDs
          */
         char strProcessingCode[3] = {0};
         strncpy(strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;

            /* VOID_SALE, OFFLINE_VOID_SALE, SALE_ADJUSTMENT  */ 
         if ( strEqual( strProcessingCode,"02" ) ||
            /* VOID_REFUND, OFFLINE_VOID_REFUND, REFUND_ADJUSTMENT, VOID_RELOAD */
              strEqual( strProcessingCode,"22" ) )
         {
            ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
            return ReturnCode ;
         }

			txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
			return NO_AUTH_ERROR ;
		}
	}

	else if (txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice ())
	{
			/* If the first two bytes are not "04" and last two bytes of the message type is 
				either "20" or "21" or "01", it implies that the transaction is a non-reversal
				advice (All message types that end with 20, 21, or 01 is an advice)
				
				We then check the TFL01 table to see if the advice has already
				been processed (because we could continue to get advices till we respond - 
				eg repeats). If the record was found, it means that the advice was already
				procesed and we just complete the transaction and return.
				
				If the advice was not found, it implies that the advice was not previously processed.
				We then check if there exists a 100 transaction in TLF01 with the same authorization
				id as the advice. If the record exists, it implies that the advice is really a 
				SALES COMPLETION transaction. We then have to use the transaction amount from the
				0100 along with the transaction amount in the SALES COMPLETION to update the 
				balance
				
				If yes then we just return - If no, we continue to process the transaction 
			*/

         ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
			if (bReturnCode == false) 
				return SYSTEM_ERROR ;
			return NO_AUTH_ERROR ;
	}

	else /* for all other transactions */
	{
		ReturnCode = TxDataServer_Get_Bin_Level_Profile () ;
		return ReturnCode ;
	}
	return NO_AUTH_ERROR ;
} /* TxDataServer_Continue_With_Transaction_Processing */

BYTE TxDataServer_Continue_With_Authorizing_Transaction ()
{
   BYTE  ReturnCode;
   BYTE  uag_rcode;

   ReturnCode = TxAuthorize_Authorize_Transaction();

   /* If currency was converted from USD to Peso for JCB or AMEX,
    * need to set it back to USD for logging into TLF01. The flag
    * that determines this is host_start_time set to 0.000.
    */
   if ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000") )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
      strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
	  memset( Auth_Tx.TLF01_details.host_start_time,0x00,
	  		(sizeof(Auth_Tx.TLF01_details.host_start_time)-1));
   }/* copy the currency back */
   else if ( strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP) )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code,   PESO_CURRENCY );
      strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
   }
   else if ( strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP) )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
      strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
   }

   if (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3) &&
	   0 == strncmp("08",Auth_Tx.TLF01_details.pos_condition_code,2))
	{	/*TF Raghu - APR 2018 MC and VISA Mandated changes: RPS Case : 001x to 10xx change */

	   	   	if( txservice_Check_If_VISA_Transaction() == true)
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[17].code,"10",2);
				Auth_Tx.TLF01_details.product_codes[17].code[2] = Auth_Tx.TLF01_details.pos_entry_mode[3];
			}
	   	   	else if(txservice_Check_If_MC_Transaction() == true)
	   	   	{
				strncpy(Auth_Tx.TLF01_details.product_codes[9].code,"10",2);
				Auth_Tx.TLF01_details.product_codes[9].code[2] = Auth_Tx.TLF01_details.pos_entry_mode[3];
	   	   	}
	}

   if (ReturnCode == NO_AUTH_ERROR)
   {

      /* All checks satisfied at this point */
      txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_COMPLETED,
                                               TRANSACTION_APPROVED );

      /* Retrieve the current alphanumeric authorization_id from UAG01 */
      if ( strlen (Auth_Tx.TLF01_details.auth_number) > 0
           &&
		  (
             txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override () ||
             txservice_If_Tx_Is_Treated_Like_A_Offline_Sale ()        ||
             txservice_If_Tx_Is_Treated_Like_A_Adjustment ()          ||
             strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX)
           )
         )
      {
         /* All done, so send response back to originator.
          * Do not calculate a auth_id if the auth_id exists AND
          * (the transaction is a voice overridden transaction OR
          * the transaction is an offline sale OR card is Amex).
          */
         txservice_Forward_Auth_Tx_To_Originator(Auth_Tx.TLF01_details.handler_queue,
                                                 MT_AUTH_RESPONSE, 0, 0);

         /* There is a path through CCF03P that may try to resend to the
          * originator.  Set tran_id to '1' as a flag so it knows not to
          * resend the response.
          */
         strcpy( Auth_Tx.TLF01_details.primary_key.transaction_id, "1" );
      }
      else
      {
    	 if(0 == strncmp(Auth_Tx.TLF01_details.message_type, "0130", 4) &&
    	 	(0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncmcrd", 6) ||
			 0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncmcr3", 6)))
    	 {
    		 /* do not generate auth number for advice */
    	 }
    	 else
    	 {
             uag_rcode = TxAuthorize_GetAuthId( ST2_NONE );


			 if ( PTEMSG_OK != uag_rcode )
			 {
				/* XIPC Error */
				ReturnCode = SYSTEM_ERROR;
			 }
			 else
			 {
				TxDataServer_DBReply_Continue();

				/* There is a path through CCF03P that may try to send to the
				 * originator again.  Set tran_id to '1' as a flag so it knows
				 * not to send it.  It was sent by the above statement.
				 */
				strcpy( Auth_Tx.TLF01_details.primary_key.transaction_id, "1" );
				ReturnCode = NO_AUTH_ERROR;
			 }
    	 }
      }
   }
   return ReturnCode ;
} /* TxDataServer_Continue_With_Authorizing_Transaction */

BOOLEAN TxDataServer_Get_NEG01_Details (void)
{
   BYTE     ReturnCode;
   BOOLEAN  bReturnCode;
   NEG01    sNEG01 = {0} ;

   /* Do not select record from NEG01 if card number is longer than 16 bytes.
    * This will cause a database error because the field is only 16 bytes.
    */
   if ( strlen(Auth_Tx.TLF01_details.card_num) <= 16 )
   {
      /* Select by card_nbr only */
      strcpy (sNEG01.card_nbr, Auth_Tx.TLF01_details.card_num) ;

      bReturnCode = txservice_Send_Request_To_Service( network_dataserver_que_name,
                                                       MT_DB_REQUEST, 
                                                       ST1_DB_SELECT, 
                                                       ST2_DB_SELECT_NEG_FILE_BY_CARD_NBR,
                                                       NEG01_DATA,
                                                       (pBYTE) &sNEG01,
                                                       sizeof(NEG01) ) ;
   }
   else
   {
      /* Skip the negative file. This card number is too long.
       * It will not be in there. Treat it as a success.  So
       * either continue with local processing or perform
       * velocity checks.
       */
      if ( (strEqual(Auth_Tx.CAF01_details.authorization_method,NEGATIVE_WITH_OFFUS_VELOCITY) ||
            strEqual(Auth_Tx.CAF01_details.authorization_method,M_NEGATIVE_OFFUS_NEGATIVE_FRAUD))
           &&
           txservice_If_Tx_Qualifies_For_Card_Velocity_Checking() )
      {
         bReturnCode = TxDataServer_Get_Offus_Velocity_Details () ;
      }
      else
      {
         ReturnCode = TxDataServer_Continue_With_Local_Processing();
         if ( ReturnCode == NO_AUTH_ERROR )
            bReturnCode = true;
         else
            bReturnCode = false;
      }
   }
   return bReturnCode ;
}

BYTE TxDataServer_Continue_With_Local_Processing (void) 
{
   BYTE ReturnCode = 0 ;
   char strTemp[256] = {0} ;

   /*if the transaction amount is greater than the floor limit
        if the authorization_method is NEGATIVE
           if authorization location is not LOCAL
              get the <CAF01.destination_name>	
              switch out to the remote host
              return NO_AUTH_ERROR
           else if authorization_method is LOCAL
              decline the transaction
              return ERROR BUT FORWARD TO UPDATE PROCESSOR to Txcntl.c
     if the authorization_method is POSITIVE or POSITIVE_WITH_BALANCE
        regardless of authorization_location proceed with getting CCF02P
   */
   ReturnCode = TxDataServer_Validate_Profile_For_Floor_Limit () ;
   if (ReturnCode == TRANSACTION_WITHIN_FLOOR_LIMIT &&
       (strEqual (Auth_Tx.CAF01_details.authorization_method, NEGATIVE_FILE) ||
        strEqual (Auth_Tx.CAF01_details.authorization_method, NEGATIVE_WITH_OFFUS_VELOCITY) ||
        strEqual (Auth_Tx.CAF01_details.authorization_method, M_NEGATIVE_OFFUS_NEGATIVE_FRAUD)))
   {
      /* Approve the transaction */
      if ( strlen ( Auth_Tx.TLF01_details.auth_number ) == 0 )
      {
         /* Generate a numeric authorization id */
         (void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
      }

      txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
      ReturnCode = txservice_Process_Bin_Range_Switch_Out (BIN01_DATA,
                                            Auth_Tx.CAF01_details.destination_name,
                                            "" );
      ReturnCode = NO_AUTH_ERROR ;
   }
   else
   {	/* transaction amount is greater than the floor limit */
      if ( strEqual(Auth_Tx.CAF01_details.authorization_method, NEGATIVE_FILE) ||
           strEqual(Auth_Tx.CAF01_details.authorization_method, NEGATIVE_WITH_OFFUS_VELOCITY) ||
           strEqual(Auth_Tx.CAF01_details.authorization_method, M_NEGATIVE_OFFUS_NEGATIVE_FRAUD) ||
		   strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0 )
      {
         if (!strEqual (Auth_Tx.CAF01_details.authorization_location, LOCAL_AUTHORIZATION))
         {
            if ( (strEqual(Auth_Tx.CAF01_details.authorization_method,NEGATIVE_WITH_OFFUS_VELOCITY) ||
                  strEqual(Auth_Tx.CAF01_details.authorization_method,M_NEGATIVE_OFFUS_NEGATIVE_FRAUD)||
                  strEqual(Auth_Tx.CAF01_details.authorization_method,POSITIVE_WITH_BALANCE_FILE))
                 &&
                 txservice_If_Tx_Qualifies_For_Card_Velocity_Checking() )
            {
               ReturnCode = TxDataServer_Get_Offus_Velocity_Details () ;
            }
            else
            {
               ReturnCode = txservice_Process_Bin_Range_Switch_Out (BIN01_DATA,
                                            Auth_Tx.CAF01_details.destination_name,
                                            "" );
            }
         }
         else
         {
            strcpy (strTemp,"Tx amount is above floor limit") ;

            ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                  "TxDataServer_Continue_With_Local_Processing",
                                   GS_TXN_INVALID_TRANSACTION_AMOUNT,
                                   RCI_SPECIAL_CONDITION) ;
         }
      }
      else
      {
         /*
            POSITIVE_FILE or POSITIVE_WITH_BALANCE_FILE or		
            POSITIVE_WITH_PRIMARY_BALANCE_FILE 	
          */
         ReturnCode = TxDataServer_Get_CCF02_Details () ;
      }
   }
   return ReturnCode ;
}

BYTE TxDataServer_Validate_Profile_For_Floor_Limit (void) 
{
   LONG  nFloorLimit = 0;
   LONG  nTotalAmount = 0;
   LONG  nProfileFloorLimit = 0;
   LONG  nMerchantCardFloorLimit = 0;
   BYTE  retval;

   if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
      nTotalAmount = atol( Auth_Tx.TLF01_details.reversal_amount );
   else
      nTotalAmount = atol( Auth_Tx.TLF01_details.total_amount );

   nFloorLimit = atol( Auth_Tx.CAF01_details.floor_limit );

   /* The following functionality is no longer used.
    * Save for future reference.
    *
   nMerchantCardFloorLimit = (strtod (Auth_Tx.MCT01_details.floor_limit, 0) / 100.0) ;

   if (nProfileFloorLimit != 0 && nMerchantCardFloorLimit == 0.0)
      nFloorLimit = nProfileFloorLimit ;
   else
      nFloorLimit = nMerchantCardFloorLimit ;*/

   if (nTotalAmount <= nFloorLimit)
      retval = TRANSACTION_WITHIN_FLOOR_LIMIT ;
   else
      retval = TRANSACTION_OUTSIDE_FLOOR_LIMIT ;

   return( retval );
}


BOOLEAN TxDataServer_Get_Neg_Fraud_Details( void )
{
   BOOLEAN           bReturnCode;
   CHAR              strBuffer[30] = "";

   /* Select by card number and MCC. */
   strcpy( strBuffer, Auth_Tx.TLF01_details.card_num );
   memcpy( strBuffer+20,
           Auth_Tx.TLF01_details.category_code,
           sizeof(Auth_Tx.TLF01_details.category_code) );

   bReturnCode = txservice_Send_Request_To_Service( tran_dataserver_que_name,
                                                    MT_DB_REQUEST, 
                                                    ST1_DB_SELECT, 
                                                    ST2_NONE,
                                                    NEGATIVE_FRAUD_DATA,
                                                    (pBYTE)strBuffer,
                                                    sizeof(strBuffer) ) ;
   return bReturnCode ;
}


/******************************************************************************
NAME:	       TxDataServer_Get_Cntry_Limits_Details
DESCRIPTION: Contains logic for requesting a record from the country_amt_limits table
INPUTS:      MessageSubType2 sent in to be passed to ACF01 after country amt limits record processed
OUTPUTS:     None
RETURNS:       false  database or system error
               true   database record retrieved - no error
AUTHOR:      LMitchell 3/21/05
MODIFIED BY:	
*******************************************************************************/
BOOLEAN TxDataServer_Get_Cntry_Limits_Details(BYTE MessageSubType2)
{
   COUNTRY_AMT_LIMITS  sCOUNTRY_AMT_LIMITS;
   BOOLEAN             bReturnCode = true;
   BYTE                temp_cc_alpha[4];
   BYTE                temp_cc_num[4];
/*   BYTE                ReturnCode;*/
/*   CHAR                strTemp[256];*/

   memset (&sCOUNTRY_AMT_LIMITS, 0, sizeof (COUNTRY_AMT_LIMITS)) ;
   strcpy (sCOUNTRY_AMT_LIMITS.primary_key.caf_profile_id, Auth_Tx.CAF01_details.primary_key.caf_profile_id) ;
   strcpy (sCOUNTRY_AMT_LIMITS.primary_key.type, Auth_Tx.CAF01_details.primary_key.type);

   /* If transaction incoming from a remote network, the origin currency code may be in tlf01.nfi_seq_nbr. 
    * Check this first. If currency code not defined there, use currency code in tlf01.currency_code.
    * If txn is from a network, check to see if it is from MasterCard.  If yes,
    * need to use the Alpha country code, convert it to a numeric code, then
    * then get the limits.
    */

   if (strlen(Auth_Tx.TLF01_details.nfi_seq_nbr) > 0)
   {
      if ( Auth_Tx.TLF01_details.product_codes[18].quantity[0] == 'M' )
      {
         memset( temp_cc_num,   0x00, sizeof(temp_cc_num)   );
         memset( temp_cc_alpha, 0x00, sizeof(temp_cc_alpha) );

         /* Alpha Country Code */
         strcpy( temp_cc_alpha, Auth_Tx.TLF01_details.ncf01_retired_cwk+13 );

         /* Convert to Numeric */
         genutil_get_country_code( A2N, temp_cc_alpha, temp_cc_num );
         if ( temp_cc_num[0] == 0x00 )
         {
            /* Country Code was not found.  Is it a US State? */
            if ( temp_cc_alpha[2] == ' ' )
               temp_cc_alpha[2] = 0x00;
            if ( true == genutil_is_US_state_code_valid(temp_cc_alpha) )
            {
               /* Yes - use country code for US. */
               strcpy( temp_cc_num, "840" );
            }
            else
            {
				 /*8/2/05:  EB request if cntry code is blank/invalid, bypass country limits check and continue */
				strcpy(temp_cc_num, "   ");
            }
         }

         /* Set Country Code for Limit checking. */         
         strcpy( sCOUNTRY_AMT_LIMITS.country_code, temp_cc_num );
      }
      else
      {
        // strcpy(sCOUNTRY_AMT_LIMITS.country_code, Auth_Tx.TLF01_details.nfi_seq_nbr);
		  /* Request by Kenneth TF PHANI 
		  Validation should be with Country code Filed 19, Not currency code*/
			strcpy(sCOUNTRY_AMT_LIMITS.country_code, Auth_Tx.country_code);
      }
   }
   else
   {
      //strcpy(sCOUNTRY_AMT_LIMITS.country_code, Auth_Tx.TLF01_details.currency_code);
	   // TF PHANI
		strcpy(sCOUNTRY_AMT_LIMITS.country_code, Auth_Tx.country_code);
   }
	/* If Country code in NULL, Need to fill with Currency code..TF PHANI*/
   if(!FieldIsNumeric(sCOUNTRY_AMT_LIMITS.country_code))
   {
		strcpy(sCOUNTRY_AMT_LIMITS.country_code, Auth_Tx.TLF01_details.currency_code);
   }
   /* Store Numeric Country Code into TLF01 for debug/report purposes. */
   if ( bReturnCode == true )
   {
      strcpy( Auth_Tx.TLF01_details.product_codes[18].code,
              sCOUNTRY_AMT_LIMITS.country_code );

      bReturnCode = txservice_Send_Request_To_Service(tran_dataserver_que_name,
                                                      MT_DB_REQUEST, 
                                                      ST1_DB_SELECT,
                                                      MessageSubType2, 
                                                      COUNTRY_AMT_LIMITS_DATA, 
                                              (pBYTE) &sCOUNTRY_AMT_LIMITS,
                                                      sizeof(sCOUNTRY_AMT_LIMITS));
   }
   return bReturnCode ;
} /* TxDataServer_Get_Cntry_Limits_Details */


/*****************************************************************************

  Function:    TxDataServer_Get_Ncf01_With_Wait

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously.

  Author: 
      BGable     6/30/98
  Inputs:
      pncf01    - pointer to the requested ncf01 structure
  Outputs:
      pncf01    - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      true      - No errors
      false     - Error
******************************************************************************/
INT TxDataServer_Get_Ncf01_With_Wait( pNCF01 pncf01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   INT           ret_val = true;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX) + sizeof(NCF01)];
   AUTH_TX       auth_tx;

   memset( buffer,0, sizeof(buffer) );
   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );

   memcpy( buffer, &auth_tx, sizeof(AUTH_TX) );
   memcpy( buffer+sizeof(AUTH_TX),pncf01, sizeof(NCF01));

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 NCF01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message." );
      ret_val = false;
   }
   else
   {
      if(PTEMSG_OK == (rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
                                                                       &p_msg_in,
                                                                        network_dataserver_que_name,
                                                                        error_msg )) )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( pncf01, p_data+sizeof(AUTH_TX), sizeof(NCF01) );
      }

      if ( PTEMSG_IPC_ERROR != rcode )
         free( p_msg_in );
      else
         ret_val = false;
   }
   return ( ret_val );
}

/*****************************************************************************

  Function:    SEND_INTERACTIVE_DB_REQUEST

  Description: This function will send a request to the dataserver.
               It uses the interactive queue.
  Author: 
      BGable     6/30/98
  Inputs:
      p_msg_out - a pointer to the outgoing message (the request to dataserver)
      p_msg_in  - a pointer to the incoming message (from the dataserver)
      que_name  - the name of the queue to send the request to
  Outputs:
      error_msg - the errror message from the dataserver
  Return values:
      dberror - the type of error
  Modified by:
******************************************************************************/
BYTE TxDataServer_Send_Interactive_DB_Request( pPTE_MSG  p_msg_out,
                                               pPTE_MSG *p_msg_in,
                                               pCHAR     que_name,
                                               pCHAR     error_msg )
{
   pBYTE           p_data;
   pPTE_MSG_DATA   p_msg_data;
   CHAR            temp_str[100] = {0};
   LONG            retcode;
   BYTE            dberror;

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to %s: %s",
               que_name, temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+sizeof(AUTH_TX), sizeof(temp_str)-1);
      sprintf( error_msg, "Database Error: %s", temp_str );
   }
   return dberror;
}

/*****************************************************************************

  Function:    TxDataServer_Get_Uag01_With_Wait

  Description: This function will send a request to the dataserver to select 
               UAG01.  This is done syncronously.

  Author: 
      BGable     6/30/98
  Inputs:
      puag01    - pointer to the requested uag01 structure
      subtype2  - PTE Message value for subtype 2
  Outputs:
      puag01    - pointer to the returned uag01 structure
      error_msg - error message returned by the data server
  Return values:
      true      - No errors
      false     - Error
******************************************************************************/
INT TxDataServer_Get_Uag01_With_Wait( pUAG01 puag01,
                                      BYTE   subtype2,
                                      pCHAR  error_msg )
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   INT           ret_val = true;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX) + sizeof(UAG01)];
   AUTH_TX       auth_tx;

   memset( buffer,0, sizeof(buffer) );
   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );

   memcpy( buffer, &auth_tx, sizeof(AUTH_TX) );
   memcpy( buffer+sizeof(AUTH_TX),puag01, sizeof(UAG01));

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT_AUTH_ID,
                                 subtype2,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 UAG01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message." );
      ret_val = false;
   }
   else
   {
      rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
                                                      &p_msg_in,
                                                       tran_dataserver_que_name,
                                                       error_msg );
      if ( rcode == PTEMSG_OK )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( puag01, p_data+sizeof(AUTH_TX), sizeof(UAG01) );
      }

      if ( PTEMSG_IPC_ERROR != rcode )
         free( p_msg_in );
      else
         ret_val = false;
   }
   return ( ret_val );
}
void Get_MCC_Allowable_values_From_DB()
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   INT           i;
   INT           retcode;
   INT           retval = true;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX) + sizeof(MCC01_LIST)];
   AUTH_TX       auth_tx;
   CHAR          error_msg[100]      = "",strTemp[100];
   MCC01_LIST		 mcc01_list;	
   /* Initialize the global variables. */
   memset( buffer,   0x00, sizeof(buffer ) );
   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
   memset( &mcc01_list,   0x00, sizeof(MCC01_LIST  ) );

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_MCC_CODES,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 MCC01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message." );
      retval = false;
   }
   else
   {
      rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
                                                      &p_msg_in,
                                                       tran_dataserver_que_name,
                                                       error_msg );
      if ( rcode == PTEMSG_OK )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( &mcc01_list, p_data+sizeof(AUTH_TX), sizeof(MCC01_LIST) );
         free( p_msg_in );
      }
	No_of_MCC_codes = atoi(mcc01_list.num_returned);
	sprintf(strTemp,
          "NO of MCC code read from DB is - : %s",
           mcc01_list.num_returned);

   txcntl_log_message( 1, 1,strTemp, "Get_MCC_Allowable_values_From_DB", 0 );
	//printf("No_of_MCC_codes = %d\n",No_of_MCC_codes);
	memset(&mcc_check_list,0,(sizeof(MCC_CHECK) * MCC_MAX_CODES));
	// Fill mcc_check_list.low and mcc_check_list.high ...
	for (i=0;i<No_of_MCC_codes ; i++ )
	{
		mcc_check_list[i].low = atoi(mcc01_list.mcc01_pk[i].primary_key.low);
		mcc_check_list[i].high = atoi(mcc01_list.mcc01_pk[i].primary_key.high);
	//	strcpy(mcc_check_list[i].low, mcc01_list.mcc01_pk[i].primary_key.low);
	//	strcpy(mcc_check_list[i].high ,mcc01_list.mcc01_pk[i].primary_key.high);
	//	printf("LOW %d\nHIGH %d\n",mcc_check_list[i].low,mcc_check_list[i].high);

	}
   	}

	return;
}


void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"AMEX",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for Amex" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &amex_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for AMEX\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for AMEX NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX", 0 );
	}
}

void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_CUP()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"CUP",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for CUP" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &cup_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for CUP\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for CUP NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_CUP", 0 );
	}

}

void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_DCI()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"DCI",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for DCI" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &dci_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for DCI\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for DCI NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_DCI", 0 );
	}

}

void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_JCB()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"JCB",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for JCB" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &jcb_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for JCB\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for JCB NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_JCB", 0 );
	}

}
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_MC()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"MC",2);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for MC" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &mc_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for MC\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for MC NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_MC", 0 );
	}

}
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_VISA()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"VISA",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for Visa" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &visa_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for VISA\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for VISA NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_VISA", 0 );
	}

}
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR()
{


	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"MP/PL/FC",8);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for MP/PL/FC" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &iftr_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for IFTR\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for IFTR NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR", 0 );
	}



}

void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX)+sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"AMEX",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for Amex" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &amex_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(amex_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for AMEX\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for AMEX NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_AMEX", 0 );
	}

}
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_CUP()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"CUP",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for CUP" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &cup_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(cup_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for cup\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for CUP NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_CUP", 0 );
	}


}
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_DCI()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"DCI",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for DCI" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &dci_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(dci_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for dci\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for DCI NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_DCI", 0 );
	}


}
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_JCB()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"JCB",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for JCB" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &jcb_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(jcb_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for jcb\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for JCB NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_JCB", 0 );
	}


}
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_MC()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"MC",2);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for MC" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &mc_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );
		}
		if(atoi(mc_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for mc\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for MC NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_MC", 0 );
	}


}
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_VISA()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"VISA",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for Visa" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &visa_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );
		}
		if(atoi(visa_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for visa\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for VISA NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_VISA", 0 );
	}


}
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_PL()
{


	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"MP/PL/FC",8);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for MP/PL/FC" );
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &pl_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );
		}
		if(atoi(pl_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " fetched FRULE01  details for  MP/PL/FC\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01  details for  MP/PL/FC NOT FOUND\n");
		}
		txcntl_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRULE_From_DB_For_PL", 0 );
	}



}

/*
 * Reading FCLF01 tabel for all tables
 * */
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands()
{
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX();
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_CUP();
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_DCI();
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_JCB();
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_MC();
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_VISA();
	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR();
}
/*
 *Reading FRULE01 tabel for all tables
 * */
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands()
{
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX();
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_CUP();
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_DCI();
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_JCB();
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_MC();
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_VISA();
	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_PL();
}
/*****************************************************************************

  Function:    TxDataServer_initAuthNumber

  Description: This function will send a request to the dataserver to select
               the most recent authorization number from TLF01. This is done
               syncronously. Once an auth number is determined it is uploaded
               into the Auth Table in shared memory.

  Author:      D. Irby

  Inputs:      None

  Outputs:     AuthNumberChanged - (Global) Flag indicating auth # change

  Return vals: None

******************************************************************************/
INT TxDataServer_initAuthNumber()
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   INT           i;
   INT           retcode;
   INT           retval = true;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX) + sizeof(TLF01)];
   AUTH_TX       auth_tx;
   UAG01         uag01;
   TLF01         tlf01;
   CHAR          error_msg[100]      = "";
   CHAR          AuthNumber[7]       = "";
   CHAR          temp_auth_number[7] = "";

   /* Initialize the global variables. */
   AuthNumberChanged = false;
   strcpy( AuthNumber, "A00001" );

   memset( buffer,   0x00, sizeof(buffer ) );
   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
   memset( &uag01,   0x00, sizeof(UAG01  ) );
   memset( &tlf01,   0x00, sizeof(TLF01  ) );

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_LAST_AUTH_ID,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 TLF01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message." );
      retval = false;
   }
   else
   {
      rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
                                                      &p_msg_in,
                                                       device_dataserver_que_name,
                                                       error_msg );
      if ( rcode == PTEMSG_OK )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( &tlf01, p_data+sizeof(AUTH_TX), sizeof(TLF01) );
         strcpy( uag01.auth_id, tlf01.auth_number );
         free( p_msg_in );
      }
      else
      {
         /* Couldn't get most recent from TLF01. Get Auth Number from UAG01. */
         retval = TxDataServer_Get_Uag01_With_Wait(&uag01,ST2_NONE,error_msg);
      }

      if ( retval == true )
      {
         /* Validate the auth number. */
         strcpy( temp_auth_number, uag01.auth_id );

         if ( strlen(temp_auth_number) == 6 )
         {
            if ( isalnum(temp_auth_number[0]) )
            {
               temp_auth_number[0] = toupper( temp_auth_number[0] );

               for( i=1; i<6; i++ )
               {
                  if ( !isdigit(temp_auth_number[i]) )
                     break;
               }
               if ( i == 6 )
                  strcpy( AuthNumber, temp_auth_number );
            }
         }
      }
   }

			if (!isalphanum(AuthNumber))
			{								// DEBUG
				sprintf( error_msg,
                       "Auth Number is null %s Porblem with reading DB..Fn: TxDataServer_initAuthNumber made NULL",
                        Auth_Tx.TLF01_details.sys_trace_audit_num );
				txcntl_log_message( 1, 1,error_msg, "TxDataServer_initAuthNumber", 1 );
			}

   /* Initialize the Auth Table in shared memory with this auth number. */
   retcode = WriteAuthTable( AuthNumber, UNLOCK );
   if ( retcode < 0 )
   {
      retval = false;
      memset( buffer, 0x00, sizeof(buffer) );
      sprintf( buffer,
              "Unable to initialize Auth Number in shared memory. Xipc err: %d",
               retcode );
      txcntl_log_message( 2, 2, buffer, "TxDataServer_initAuthNumber",0);
   }

   if ( 0 == strcmp(ServiceName, "txcntl") )
   {
      /* Don't log this for multiple instances. */
      memset( buffer, 0x00, sizeof(buffer) );
      sprintf( buffer, "Most recent Auth Number was %s", AuthNumber );
      txcntl_log_message( 2, 1, buffer, "TxDataServer_initAuthNumber",0);
   }
   return ( retval );
}


/*****************************************************************************

  Function:    TxDataServer_DBReply_Continue

  Description: This function continues processing after a response from
               TxDataServer_Process_Db_Reply when the response value is
               NO_AUTH_ERROR.

               The purpose of this function is to either forward a request
               to a network controller or to send it to the originator.

               The reason this code was moved from MT_DB_REPLY is because
               the auth number is no longer obtained from the database
               (UAG01). So there is no database response to get into
               function Process_Db_Reply. So after the auth number is
               obtained from shared memory, we need a way to perform the
               code within this function.

  Author:      D. Irby

  Inputs:      None

  Outputs:     None

  Return vals: None

******************************************************************************/
void TxDataServer_DBReply_Continue()
{
   BOOLEAN  bReturnCode = true ;
   BYTE     ReturnCode = 0 ;
   CHAR     strTemp[256] = {0} ;
   int dest_len = 0;

   if ( (strEqual(Auth_Tx.TLF01_details.response_code, TRANSACTION_APPROVED)) &&
        (Auth_Tx.CAF01_details.authorization_location[0] != 0x00) )
   {
      /* AT THIS STAGE THE TRANSACTION HAS BEEN ACCEPTED - If the
       * profile's authorization location is LOCAL/REMOTE OVERRIDE
       * APPROVALS we still have to send it out to the host, as the
       * host could override this approval.
       */

      /* If profile auth location is REMOTE we send tx to remote host
       * If profile auth location is REMOTE/LOCAL STAND-IN we send tx
       * to remote host.  If profile auth location is LOCAL we send
       * the transaction to the Updater
       */

      /*  REMOTE */
      if (strEqual(Auth_Tx.CAF01_details.authorization_location, "R") ||

          /* LOCAL/REMOTE OVERRIDE APPROVALS */
          strEqual (Auth_Tx.CAF01_details.authorization_location, "A") ||

          /* REMOTE/LOCAL STAND-IN */
         (strEqual (Auth_Tx.CAF01_details.authorization_location, "S")) && 
		   /* Transction Should be request. These two conditions added for STAND IN processing.TF PHANI */
		  Auth_Tx.TLF01_details.tx_key < 100							&& 	
		   /* TRasaction should not be in STAND IN PROCESSING */
		  strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN) != 0) // TF PHANI
      {
         /* Determine if transaction qualifies for Cash Bonus
          * Program prior to sending it out to a remote host.
          */
		
         strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );
         if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
         {
            /* It does qualify; set the flag for updatds. */
            ReturnCode = TxAuthorize_Update_AuthTx_Structures(0, 1, 0);
         }

         /* Forward txn to a network controller. */
		if ( 0 != strcmp(Auth_Tx.TLF01_details.primary_key.transaction_id, "1"))
         {
			 /* The tran_id is set to '1' if already forwarded to NC */
			 dest_len = strlen(Auth_Tx.CAF01_details.destination_name);
			 if (Auth_Tx.CAF01_details.destination_name[dest_len - 1] == 'A')
				bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
								 (Auth_Tx.CAF01_details.destination_name, 
								  MT_AUTH_REQUEST, 
								  0, 
								  0) ;
			 else 
				 bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
								 (strcat(Auth_Tx.CAF01_details.destination_name, "A"), 
								  MT_AUTH_REQUEST, 
								  0, 
								  0) ;
		}

         if (false == bReturnCode)
         {
            sprintf( strTemp,
                    "Problems sending to remote host %s",
                     Auth_Tx.CAF01_details.destination_name );

            if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
               strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

            txcntl_log_message( 1, 2,strTemp, "TxDataServer_DBReply_Continue", 1 );
            txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                          MT_AUTH_RESPONSE, 
                                          0, 
                                          0) ;
         }
      }
      else 
      {
         /* Local Authorization - respond to originator. *

         /* The tran_id is set to '1' if response already sent. */
         if ( 0 != strcmp(Auth_Tx.TLF01_details.primary_key.transaction_id, "1") )
         {
            bReturnCode = txservice_Forward_Auth_Tx_To_Originator 
                                  (Auth_Tx.TLF01_details.handler_queue,
                                   MT_AUTH_RESPONSE,
                                   0, 
                                   0) ;
         }
      }
   }
   else
   {
      /* Transaction was not approved. */
      if ( Auth_Tx.TLF01_details.general_status == GS_TXN_INVALID_CAF_STATUS )
      {
         /* Auth profile status is not active. Respond to originator.
          * Will come here if CAF01 record is found in shared memory,
          * but not if it is obtained from the database.
          */
         if ( !txservice_If_Tx_Cannot_Be_Declined() )
         {
            memset( Auth_Tx.TLF01_details.auth_number, 0x00,
                    sizeof( Auth_Tx.TLF01_details.auth_number) );
         }
         bReturnCode = txservice_Forward_Auth_Tx_To_Originator 
                             (Auth_Tx.TLF01_details.handler_queue,
                              MT_AUTH_RESPONSE, 
                              0, 
                              0) ;
      }
   }
   return;
}

void TxDataServer_Sha256_Encryption(char * Supplemental_output)
{
	char Input_Sha256_Buffer[50] ={0};
	int len=0;
	char HexBuffer[500]={0};
	unsigned char Output_Sha256_Buffer[100]={0};


	char strSystemDate[9] = {0};
	long juldate_today = 0;
	char julian_year[3]={0};
	char strSystemDateTime[50] = {0};
	char juldate_today_str[50]={0};
	char year[5]={0};
	char month[3]={0};
	char day[3]={0};
	int iyear=0,yyyy=0,mm=0,dd=0,JulianDate=0;
	char julday_req[5]={0};
	char Supplemental_data_out[65]={0};
	int Date_compare=0;
	/*char Supplemental_output[40]={0};*/

	/*Prepare the Input to the Sha256 encryption
	*1.15 digits of card num+non zero digit of the eci value appended.
	* (126.9 subfield 6( supplement data)
	*2.purchase amount 12 bytes
	*3.puchase currency code
	*4.Authentication date Julian date(yyddd) 11 november 2021( 21315)
	*/

	genutil_format_date (strSystemDateTime) ;

	strncpy(strSystemDate,strSystemDateTime,8);
	strncpy(year,strSystemDate,4);
	strncpy(month,strSystemDate+4,2);
	strncpy(day,strSystemDate+6,2);
	yyyy=atoi(year);
	mm=atoi(month);
	dd=atoi(day);

	JulianDate=calculate_julian_date(mm,dd,yyyy);
	strncpy(julday_req,&Auth_Tx.TLF01_details.dcf01_retired_cwk[27],3);

	sprintf(juldate_today_str,"%03d",JulianDate);

    Date_compare=strncmp(juldate_today_str,julday_req,3);

	if(Date_compare>=0)
	{

	  strncpy(julian_year,year+2,2);

	}
	else
	{
		strncpy(julian_year,year+2,2);
		iyear=atoi(julian_year)-1;
		sprintf(julian_year,"%d",iyear);

	}
	strncpy(Input_Sha256_Buffer,Auth_Tx.TLF01_details.card_num,15);
	strncat(Input_Sha256_Buffer,Auth_Tx.TLF01_details.profile+1,1);
	strcat(Input_Sha256_Buffer,"00");
	strncat(Input_Sha256_Buffer,&Auth_Tx.TLF01_details.dcf01_retired_cwk[14],10);
	strncat(Input_Sha256_Buffer,&Auth_Tx.TLF01_details.dcf01_retired_cwk[24],3);
	strncat(Input_Sha256_Buffer,julian_year,2);
	strncat(Input_Sha256_Buffer,&Auth_Tx.TLF01_details.dcf01_retired_cwk[27],3);

	len= strlen(Input_Sha256_Buffer);
	char * hash= sha256(Input_Sha256_Buffer, len, Output_Sha256_Buffer);

	 string2hexString(hash,HexBuffer);
	 GetOnlyNumber(HexBuffer,Supplemental_data_out);
	 strncpy(Supplemental_output,Supplemental_data_out,16);
	 strncat(Supplemental_output,&Auth_Tx.TLF01_details.dcf01_retired_cwk[10],4);
	 strncat(Supplemental_output,&Auth_Tx.TLF01_details.dcf01_retired_cwk[1],1);
	 strncat(Supplemental_output,&Auth_Tx.TLF01_details.dcf01_retired_cwk[2],2);
	 strcat(Supplemental_output,PADDED_ZEROES);



}

INT isLeap(int year)
{
	if (((year % 4   == 0)    &&
         (year % 100 != 0)) ||
         (year % 400 == 0))
	{
		return true;
	}
	return false;
}



int calculate_julian_date(int mm,int dd, int year )
{
	int month[12] ={31,28,31,30,31,30,31,31,30,31,30,31};
	int Julian_date=0;

	for(int i=1;i<mm;i++)
	{
		Julian_date = Julian_date+month[i];
	}

	if (true==isLeap(year))
	{
		if(mm==2 && dd==29)
		{
			Julian_date= Julian_date+dd+1;
		}
		else if(mm> 2)
		{
			Julian_date= Julian_date+dd+1;
		}
	}
	else
	{
		Julian_date= Julian_date+dd;
	}

	return Julian_date;

}

void Txdataserver_RemoveHyphes(char *input, char *output)
{
	int i=0,j=0;

	for( i=0;i<strlen(input);i++)
	{
		if(input[i]!='-')
		{
			output[j]=input[i];
			j++;
		}
	}
	output[i]='\0';
}

BOOLEAN Txdataserver_Derive_DSTransactionID()
{
	 long double decnum=0.00f;
		long double decnum_parta=0.00f;
		long double rem= 0.00f;
	    long double total = 0.00f;

	    int i=0;
		int len=0;
		int length=0;
		int Length_of_string=0;
		int last_four_digit=0;

	    char hexnum[30] = {0};
	    char hexnum_parta[10] ={0};
	    char string[25] = {0};
	    char string_parta[25] = {0};
	    char string_original[25] ={0};
		char string_original_new[25];
		char string_last_four_bytes[5] = {0};
	    char string_parta_original[25] = {0};
		char string_last_four_bytes_parta[5] = {0};
		char Derived_DS_Transaction_ID_Value[5] = {0};
		char Transaction_ID_input[37]={0};
		char Transaction_ID[37]={0};
		char input_desEncryption[50]={0};

	    printf("Enter any Hexadecimal Number: ");
	    scanf("%s", hexnum);
		strncpy(Transaction_ID_input,Auth_Tx.TLF01_details.product_codes[13].amount,12);
		strncat(Transaction_ID_input,Auth_Tx.TLF01_details.product_codes[18].amount,12);
		strncat(Transaction_ID_input,Auth_Tx.TLF01_details.product_codes[19].amount,12);
		Txdataserver_RemoveHyphes(Transaction_ID_input,Transaction_ID);
		strncpy(hexnum,Transaction_ID+12,20);
	    while(hexnum[i]!='\0')
	    {
	        len++;
	        i++;
	    }
	    length = len-8;

	    printf("Entered Hexadecimal Number[%s]: ",hexnum);
	    strncpy(hexnum_parta,hexnum+length,8);
	    memcpy(hexnum+length,"00000000",8);

	    len--;
	    i=0;
	    printf("Length[%d]",len);
	    while(len>=0)
	    {
	        rem = hexnum[len];
	        if(rem>=48 && rem<=57)
	            rem = rem-48;
	        else if(rem>=65 && rem<=70)
	            rem = rem-55;
	        else if(rem>=97 && rem<=102)
	            rem = rem-87;
	        else
	        {
	            printf("\nYou've entered an invalid Hexadecimal digit");
	            return 0;
	        }
	        decnum = decnum + (rem*pow(16.0, i));
	        len--;
	        i++;
	    }
	        printf("\nEquivalent Decimal Value = %Lf", decnum);

	   length = 8 ;
	   length--;
	   i=0;

	   while(length>=0)
	    {
	        rem = hexnum_parta[length];
	        if(rem>=48 && rem<=57)
	            rem = rem-48;
	        else if(rem>=65 && rem<=70)
	            rem = rem-55;
	        else if(rem>=97 && rem<=102)
	            rem = rem-87;
	        else
	        {
	            printf("\nYou've entered an invalid Hexadecimal digit");
	            return 0;
	        }
	        decnum_parta = decnum_parta + (rem*pow(16, i));
	        length--;
	        i++;
	    }
	        printf("\nEquivalent Decimal Value = %Lf", decnum_parta);

	    sprintf(string,"%Lf",decnum);
	    sprintf(string_parta,"%Lf",decnum_parta);

	    for(int j=0;j<strlen(string);j++)
	    {
	       if(string[j] == '.')
	           {
	                        break;
	           }
	           else
	           {
	                strncpy(string_original+j,string+j,1);
	           }
	    }
	    /*printf("\nstring_original[%s]",string_original);*/

	   for(int j=0;j<strlen(string_parta);j++)
	    {
	       if(string_parta[j] == '.')
	           {
	                        break;
	           }
	       else
	           {
	                        strncpy(string_parta_original+j,string_parta+j,1);
	           }
	    }
	   /*printf("\nstring_parta[%s]",string_parta_original);*/
	   if(strlen(string_original) > 20 )
	   {
	     Length_of_string = strlen(string_original);
	     memcpy(string_original_new,string_original+(Length_of_string -20),20);
	     memcpy(string_last_four_bytes,string_original_new+(strlen(string_original_new) - 4 ),4);
	     memcpy(string_last_four_bytes_parta,string_parta_original+(strlen(string_parta_original) - 4 ),4);
	     last_four_digit = (atoi(string_last_four_bytes) + atoi(string_last_four_bytes_parta));
	     if(last_four_digit > 9999)
	         {
	                last_four_digit = last_four_digit - 10000;
	         }
	     sprintf(Derived_DS_Transaction_ID_Value,"%04d", last_four_digit);
	     printf("\nLast Four Digit Value[%s]", Derived_DS_Transaction_ID_Value);

	   }

	   strncpy(input_desEncryption,Auth_Tx.TLF01_details.card_num,16);
	   strncat(input_desEncryption,Derived_DS_Transaction_ID_Value,4);
	   strncat(input_desEncryption,Auth_Tx.TLF01_details.product_codes[1].amount+1,1);
	   strncat(input_desEncryption,"0",4);
	   strncat(input_desEncryption,Auth_Tx.TLF01_details.product_codes[1].amount+3,1);
	   if (true ==txservice_CUP_get_AVN_from_DESEncrypn(input_desEncryption))
		  return true;
	   else
		   return false;

}


BOOLEAN Txdataserver_Verify_AVN_Value()
{
	/* long double decnum=0.00f;
		long double decnum_parta=0.00f;
		long double rem= 0.00f;
	    long double total = 0.00f;

	    int i=0;
		int len=0;
		int length=0;
		int Length_of_string=0;
		int last_four_digit=0;

	    char hexnum[30] = {0};
	    char hexnum_parta[10] ={0};
	    char string[25] = {0};
	    char string_parta[25] = {0};
	    char string_original[25] ={0};
		char string_original_new[25];
		char string_last_four_bytes[5] = {0};
	    char string_parta_original[25] = {0};
		char string_last_four_bytes_parta[5] = {0};
		char Derived_DS_Transaction_ID_Value[5] = {0};
		char Transaction_ID_input[37]={0};
		char Transaction_ID[37]={0};
		char input_desEncryption[50]={0};

	    printf("Enter any Hexadecimal Number: ");
	    scanf("%s", hexnum);
		strncpy(Transaction_ID_input,Auth_Tx.TLF01_details.product_codes[13].amount,12);
		strncat(Transaction_ID_input,Auth_Tx.TLF01_details.product_codes[18].amount,12);
		strncat(Transaction_ID_input,Auth_Tx.TLF01_details.product_codes[19].amount,12);
		Txdataserver_RemoveHyphes(Transaction_ID_input,Transaction_ID);
		strncpy(hexnum,Transaction_ID+12,20);
	    while(hexnum[i]!='\0')
	    {
	        len++;
	        i++;
	    }
	    length = len-8;

	    printf("Entered Hexadecimal Number[%s]: ",hexnum);
	    strncpy(hexnum_parta,hexnum+length,8);
	    memcpy(hexnum+length,"00000000",8);

	    len--;
	    i=0;
	    printf("Length[%d]",len);
	    while(len>=0)
	    {
	        rem = hexnum[len];
	        if(rem>=48 && rem<=57)
	            rem = rem-48;
	        else if(rem>=65 && rem<=70)
	            rem = rem-55;
	        else if(rem>=97 && rem<=102)
	            rem = rem-87;
	        else
	        {
	            printf("\nYou've entered an invalid Hexadecimal digit");
	            return 0;
	        }
	        decnum = decnum + (rem*pow(16.0, i));
	        len--;
	        i++;
	    }
	        printf("\nEquivalent Decimal Value = %Lf", decnum);

	   length = 8 ;
	   length--;
	   i=0;

	   while(length>=0)
	    {
	        rem = hexnum_parta[length];
	        if(rem>=48 && rem<=57)
	            rem = rem-48;
	        else if(rem>=65 && rem<=70)
	            rem = rem-55;
	        else if(rem>=97 && rem<=102)
	            rem = rem-87;
	        else
	        {
	            printf("\nYou've entered an invalid Hexadecimal digit");
	            return 0;
	        }
	        decnum_parta = decnum_parta + (rem*pow(16, i));
	        length--;
	        i++;
	    }
	        printf("\nEquivalent Decimal Value = %Lf", decnum_parta);

	    sprintf(string,"%Lf",decnum);
	    sprintf(string_parta,"%Lf",decnum_parta);

	    for(int j=0;j<strlen(string);j++)
	    {
	       if(string[j] == '.')
	           {
	                        break;
	           }
	           else
	           {
	                strncpy(string_original+j,string+j,1);
	           }
	    }
	    printf("\nstring_original[%s]",string_original);

	   for(int j=0;j<strlen(string_parta);j++)
	    {
	       if(string_parta[j] == '.')
	           {
	                        break;
	           }
	       else
	           {
	                        strncpy(string_parta_original+j,string_parta+j,1);
	           }
	    }
	   printf("\nstring_parta[%s]",string_parta_original);
	   if(strlen(string_original) > 20 )
	   {
	     Length_of_string = strlen(string_original);
	     memcpy(string_original_new,string_original+(Length_of_string -20),20);
	     memcpy(string_last_four_bytes,string_original_new+(strlen(string_original_new) - 4 ),4);
	     memcpy(string_last_four_bytes_parta,string_parta_original+(strlen(string_parta_original) - 4 ),4);
	     last_four_digit = (atoi(string_last_four_bytes) + atoi(string_last_four_bytes_parta));
	     if(last_four_digit > 9999)
	         {
	                last_four_digit = last_four_digit - 10000;
	         }
	     sprintf(Derived_DS_Transaction_ID_Value,"%04d", last_four_digit);
	     printf("\nLast Four Digit Value[%s]", Derived_DS_Transaction_ID_Value);

	   }

	   strncpy(input_desEncryption,Auth_Tx.TLF01_details.card_num,16);
	   strncat(input_desEncryption,Derived_DS_Transaction_ID_Value,4);
	   strncat(input_desEncryption,Auth_Tx.TLF01_details.product_codes[1].amount+1,1);
	   strncat(input_desEncryption,"0",1);
	   strncat(input_desEncryption,Auth_Tx.TLF01_details.product_codes[1].amount+3,1);
	   if (true ==txservice_CUP_get_AVN_from_DESEncrypn(input_desEncryption))
	   {
		  return true;
	   }
	   else
	   {
		   return false;
	   }*/

	 return true;

}


/*****************************************************************************/
INT TxDataServer_Check_Exception_Card_Num(CR01 sCR01)
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
               if(( strcmp(sCR01.cardnum_blocking[i].exception_cardnum, Auth_Tx.TLF01_details.card_num) == 0))
               {
                  ret_val = BLOCKED ;
                  break;
               }
            }


   }
   return( ret_val );
}

BYTE TxDataServer_Perform_Single_amount_limit (CR01 sCR01)
{
	double nSingle_AMTLimit = 0.0 ;
	double nTotalAmount =0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;


	nTotalAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;
		nSingle_AMTLimit = (strtod (sCR01.single_amtlimit, 0) / 100.0) ;

		if (nTotalAmount > nSingle_AMTLimit)
		{
			sprintf (strTemp,"Chip fallback decline rule %s",sCR01.rule_id) ;

			ReturnCode = txservice_Generate_Usage_Error_Message( 2,
	                                                      strTemp,
	                                                     "TxDataServer_Perform_Single_amount_limit",
	                                                      GS_TXN_TRANSACTION_NOT_ALLOWED,
														  sCR01.response_code) ;
			return ReturnCode ;
		}
	return NO_AUTH_ERROR ;
}

/*#ifdef MC07_OCT2023

void TxDataServer_separateNames(const char *full_name, char *first_name, char *last_name)
{
    const char *space_position = strchr(full_name, ' ');

    if (space_position != NULL)
    {
        // Calculate the index of the space character
        int space_index = space_position - full_name;

        // Extract first name and last name
        strncpy(first_name, full_name, space_index);
        first_name[space_index] = '\0'; // Null-terminate the first name

        strcpy(last_name, space_position + 1); // Start after the space
    }
    else
    {
        // In case no space is found, set both first name and last name to the input
        strcpy(first_name, full_name);
        last_name[0] = '\0'; // Empty last name
    }
}

#endif*/
