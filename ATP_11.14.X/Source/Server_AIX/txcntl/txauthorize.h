/**************************************************************************************************
*  
* MODULE:      Txauthorize.h
*  
* TITLE:       ATP Standard - Process Incoming Request.
*  
* DESCRIPTION:  
*
* APPLICATION: TXCNTL
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*
*
* $Log:   N:\PVCS\PTE\Equitable\txcntl\txauthorize.h  $

      Rev 1.24   Sep 08 2009  Ajay ThoughtFocus
   Added two macros for STIP currency conversion when account not found 
   Rel 09-57
   
	 Rev 1.23   May 31 2007  DIrby
   Added function prototypes for getting an authorization number.
   SCR 12299

      Rev 1.22   Jun 18 2004 14:49:46   dirby
   Broke out MCC amount check into Peso and USD amount checks.
   SCR 1380
   
      Rev 1.21   Jan 08 2004 11:43:12   dirby
   Added prototype 'get_conv_rate_from_database'.
   Added include file TxCntl.h, which defines CRF01 for this prototype.
   SCR 1293
   
      Rev 1.20   Nov 05 2003 14:42:10   dirby
   Created an input parameter for function update_ccf03_structure.
   This parameter is a flag indicating whether or not  to update the date
   and time in ccf03.
   SCR 939
   
      Rev 1.19   Nov 29 2001 10:31:30   dirby
   Modified function Calculate_RSB_Amounts to return a double.
   It returns the Deferred Gross Amount.     SCR 256
   
      Rev 1.18   Nov 07 2001 13:36:52   dirby
   Added function prototype for TxAuthorize_Calculate_RSB_Amounts.     SCR 256
   
      Rev 1.17   Apr 27 2001 13:39:36   dirby
   Added original amount as a parameter to the function that
   converts USD to PESO.  This is so voids update balance
   accounts when amount is converted.
   
      Rev 1.16   Apr 18 2001 12:42:04   dirby
   Added function prototype 'TxAuthorize_Convert_USD_to_Peso'.
   
      Rev 1.15   Apr 17 2000 09:39:58   iarustam
   added bin_blocking and offus velocity functionality
   
      Rev 1.14   Dec 30 1999 13:32:50   ddabberu
   aix_build
   
      Rev 1.13   Dec 17 1999 19:18:30   iarustam
   fixed offline voids
   
      Rev 1.12   Jun 01 1999 17:14:14   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.11   May 28 1999 16:03:50   sdasgupt
   Manually keyed-in transactions originating from a Non MOTO or a non T&E merchant are declined
   
   
      Rev 1.10   May 14 1999 17:37:28   sdasgupt
    
   
      Rev 1.9   May 13 1999 14:53:14   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.8   May 04 1999 15:53:54   sdasgupt
   1. Failed pin attempts resets to 0 on a new transaction day
   2. Expiration date checking now made Y2K compliant
   3. New messages added
   4. On voice transactions, users see same messages as that in the 
      monitor
   
      Rev 1.7   Apr 15 1999 15:33:38   sdasgupt
   1. Voice-auth Overrides
   2. Voice-auth Reversals
   3. Added Voice-auth error messages
   4. Fixed Deferred balance account update
   5. Fixed error message with the deferred min amount
   6. Fixed bug where a Cash Advance transaction was ignoring the Merchant Capabilites screen
   
      Rev 1.6   Apr 01 1999 11:20:40   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.5   Feb 24 1999 17:09:40   sdasgupt
    
   
      Rev 1.4   Feb 23 1999 18:34:16   sdasgupt
    
   
      Rev 1.3   Feb 23 1999 11:11:30   sdasgupt
    
   
      Rev 1.2   Feb 17 1999 18:13:18   sdasgupt
   Program Changes
   
      Rev 1.1   Feb 16 1999 17:50:30   sdasgupt
   TxCntl Changes
   
      Rev 1.0   Feb 12 1999 18:54:28   sdasgupt
   Initial version
*******************************************************************************/
#ifndef _TxAuthorize
#define _TxAuthorize

#include <stdio.h>
#include <string.h>
#include "app_info.h"
#include "txcntl_constants.h"
#include "txfleet.h"
#include "txcntl.h"

#define PESO 1
#define DOLLAR 2

#define PESO_COMP "5.000"
#define USD_COMP "7.000"
typedef struct 
{
	int day_period ;

	int nbr_usage_peso ;
	int nbr_usage_dollar ;

	double amt_usage_peso ;	// 8 bytes // 15 digits
	double amt_usage_dollar ;

} CARD_VELOCITY_FROM_PROFILE ;

typedef struct 
{
	long peso_julian_date ;
	long dollar_julian_date ;

	double daily_amt_usage ;

	int failed_pin_attempts ;

	struct
	{
		int nbr_usage ;
		double amt_usage ;	// 8 bytes // 15 digits
	} peso_usage[14] ;

	struct
	{
		int nbr_usage ;
		double amt_usage ;	// 8 bytes // 15 digits
	} dollar_usage[14] ;

	struct
	{
		BYTE category_code[5] ;
		int  usage ;
	} mcc[20] ;

	struct
	{
		BYTE merchant_id[16] ;
		int nbr_uses ;
	} repeat [20] ;


} PRIVATE_CCF03P_STRUCT ;
//
// MCC  struct Variables
#define EACH_MCC_CODE 250
#define MAX_MCC_BUFFER 800
#define MCC_MAX_CODES MAX_MCC_ALLOWED_LIST
typedef struct
{	INT low;
	INT high;
}MCC_CHECK;


typedef struct
{
	CHAR MCCstring[5];
	INT  Intmcc;
}MCC_VALUE;

INT Totol_numof_MCC;

BYTE TxAuthorize_Process_VerifyPin_Response (pPTE_MSG p_msg_in) ;
BYTE TxAuthorize_Process_ARQC_Response (pPTE_MSG p_msg_in) ;
BYTE TxAuthorize_Process_3D_Secure_Response(pPTE_MSG p_msg_in) ;
BYTE TxAuthorize_Process_3D_Secure_Response( pPTE_MSG p_msg_in );
BYTE TxAuthorize_Process_3D_Secure_2_Response( pPTE_MSG p_msg_in );
BYTE TxAuthorize_Process_3D_Secure_Response_2( pPTE_MSG p_msg_in );
BYTE TxAuthorize_Process_VerifyMagstripe_Response (pPTE_MSG p_msg_in) ;
BYTE TxAuthorize_Authorize_Transaction (void) ;
BYTE TxAuthorize_Validate_Expiry_Date (BYTE* strExpiryDate) ;
/* TF-Ajay Additional expiry date checking 01-Nov-2010 */
BYTE TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date (BYTE* strPrimaryExpiryDate, BYTE* strSecondaryExpiryDate);
BYTE TxAuthorize_Process_Security (BYTE verification_type) ;
BYTE TxAuthorize_Validate_TransactionType (void) ;
BYTE TxAuthorize_Validate_TransactionAmount(double) ;
BYTE TxAuthorize_Perform_Card_Velocity_Checks (double nConvertedTxAmount, CR01 sCR01);
BYTE TxAuthorize_Perform_Offus_Card_Velocity_Checks () ;

BYTE TxAuthorize_Perform_Card_Limit_Checks (double nConvertedTxAmount) ;
BYTE TxAuthorize_Perform_Balance_Account_Checks (double nConvertedTxAmount) ;
BYTE TxAuthorize_Update_AuthTx_Structures (BYTE UpdateCCF03PStruct,
										   BYTE UpdateCashBonusStruct,
										   BYTE UpdateACF01Struct) ;
BYTE TxAuthorize_Perform_Merchant_Max_Credit_Amt (double nTotalAmount) ;
BYTE TxAuthorize_Perform_Merchant_Max_Auth_Amt (double nTotalAmount) ;
BYTE TxAuthorize_Perform_Merchant_Max_Return_Amt (double nTotalAmount) ;
BYTE TxAuthorize_Perform_Merchant_Max_CashAdvance_Amt (double nTotalAmount) ;

// returns ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR on a violation
BYTE TxAuthorize_Perform_Merchant_Velocity_Checks (void) ;
BYTE TxAuthorize_Validate_Deferred_Amounts (char* deferred_min_amount, char* deferred_max_amount) ;

BYTE TxAuthorize_Check_Expiry_Date (void) ;
BYTE TxAuthorize_Perform_Cash_Bonus_Processing (double nConvertedTxAmount) ;
BYTE TxAuthorize_Update_CCF03_Structure (INT) ;
BYTE TxAuthorize_Update_Offus_Velocity_Structure (void) ;
BYTE TxAuthorize_Update_ACF01_Structure (void) ;
BYTE TxAuthorize_STIP_Update_ACF01_Structure (void);//TF PHANI
BYTE TxDataServer_Continue_With_Authorize_After_Override(double nConvertedTxAmount);
BYTE TxAuthorize_Update_BON01_Structure (void) ;
BYTE TxAuthorize_Perform_Authorization (double nConvertedTxAmount) ;
BYTE TxAuthorize_Get_Velocity_Limits_From_Profile (CARD_VELOCITY_FROM_PROFILE card_velocity[]) ;
BYTE TxAuthorize_Clear_Usage_Bucket (int Context) ;
BYTE TxAuthorize_Populate_Usage_Bucket (int Context) ;
BYTE TxAuthorize_Populate_Offus_Velocity_Usage_Bucket ( int Context );
BYTE TxAuthorize_Populate_Merchant_Usage_Bucket (void) ;
BYTE TxAuthorize_Populate_Category_Usage_Bucket (void) ;
BYTE TxAuthorize_Populate_Offus_Velocity_Merchant_Usage_Bucket (void) ;
BYTE TxAuthorize_Populate_Offus_Velocity_Category_Usage_Bucket (void) ;

BYTE TxAuthorize_Populate_Failed_Pin_Attempts (void) ;
BYTE TxAuthorize_Deduct_Amount_From_Usage_Bucket (int CurrencyType, double nConvertedTxAmount) ;
BYTE TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits 
												(CARD_VELOCITY_FROM_PROFILE card_velocity[],
												int DaysElapsedSinceAPesoTransactionLastOccurred,
												double nConvertedTxAmount) ;
BYTE TxAuthorize_Shift_Dollar_Usage_Bucket_Down_And_Verify_Limits 
												(CARD_VELOCITY_FROM_PROFILE card_velocity[],
												 int DaysElapsedSinceADollarTransactionLastOccurred,
												 double nConvertedTxAmount) ;
BYTE TxAxthorize_Clear_Merchant_Usage_Bucket (void) ;
BYTE TxAxthorize_Clear_Category_Usage_Bucket (void) ;
BYTE TxAuthorize_Validate_Merchant_Repeat_Checks (void) ;
BYTE TxAuthorize_Validate_Category_Count_Checks(INT);
BYTE TxAuthorize_Calculate_Deferred_Gross_Amount (double* pConvertedTxAmount) ;
BYTE TxAuthorize_Calculate_Deferred_Monthly_Amortized_Amount (double* pConvertedTxAmount) ;
double TxAuthorize_Calculate_RSB_Amounts (double* pConvertedTxAmount) ;
BYTE TxAuthorize_If_Transaction_Occurred_On_Same_Day (void) ;
BYTE TxAuthorize_Update_Private_Ccf03p_Structure () ;
BYTE TxAuthorize_Is_Transaction_Manually_Entered (void) ;
BYTE TxCheck_Allowable_MCC_From_Ini(void);
BYTE TxAuthorize_Is_Merchant_A_MOTO_TE_Merchant (void) ;
BYTE TxAuthorize_Update_Private_Offus_Velocity_Structure () ;

BOOLEAN TxAuthorize_Convert_USD_to_Peso(double* nAuthTxTotalAmount,double* nAuthTxRevAmount,double* nAuthTxOrigAmount);
BOOLEAN TxAuthorize_Convert_Peso_to_USD(double* nAuthTxTotalAmount,double* nAuthTxRevAmount,double* nAuthTxOrigAmount);
// TF Phani 
BOOLEAN TxAuthorizeCadience_Convert_USD_to_Peso(double* nAuthTxTotalAmount,double* nAuthTxRevAmount,double* nAuthTxOrigAmount);
BOOLEAN TxAuthorizeCadience_Convert_Peso_to_USD(double* nAuthTxTotalAmount,double* nAuthTxRevAmount,double* nAuthTxOrigAmount);

LONG get_conv_rate_from_database( pCRF01 p_crf01 );

BYTE TxAuthorize_GetAuthId( BYTE subtype2 );
BYTE TxAuthorize_GenerateNewAuthId(pCHAR pstrCurrentAuthId,pCHAR pstrNewAuthId);
INT TxAuthorize_Check_TxnType_is_ChipFallBack();

#endif
