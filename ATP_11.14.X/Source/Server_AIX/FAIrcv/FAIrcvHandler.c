
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <signal.h>
#include "ptemsg.h"
#include "pteipc.h"
#include "txtrace.h"
#include "ptesystm.h"
#include "ntutils.h"
#include "txutils.h"
#include "memmnger.h"
#include "genutil.h"
#include "txutils.h"
#include "basictyp.h" 
#include "app_info.h"
#include "equitdb.h"
#include "tx_dbstruct.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "FAIrcvHandler.h"
#include "global_definitions.h"
#include "Faircv.h"
#include "timings.h"
#include "pte.h"
#include "ptestats.h"

#ifndef  false
#define  false    0x00
#endif

#ifndef  true
#define  true     0x01
#endif


BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ;

FTLF01       Ftlf01;
AUTH_TX     Auth_Tx;
#define AUTH_TX_SIZE	sizeof(Auth_Tx)
#define FTLF01_SIZE		sizeof(Ftlf01)


/* QUEUES */
CHAR  AppName[80];
CHAR  faircv_controller_que_name[] = "falcntlA";
CHAR  txcntl_que_name [MAX_QUE_NAME_SIZE+1] = "txcntlA";

/* STRUCTURE SIZES */
INT  Auth_Tx_Size = AUTH_TX_SIZE;

INT t_secs =0;
INT t_id = 0;
PRIVATE INT ret_val 	= 0 ;


struct
{
	char    HeaderFile                      [70];
	char	WORKFLOW_XCD					[16];
	char	RECORD_TYPE_XCD					[8];
	char	DATA_SPECIFICATION_VERSION_XCD	[5];
	char	CLIENT_XID						[16];
	char	RCD_CRT_DATE					[8];
	char	RCD_CRT_TIME					[6];
	char	RCD_CRT_MSCND					[3];
	char	GMT_OFFSET						[6];
	char	CUSTOMER_XID					[20];
	char	ACCOUNT_REFERENCE_XID			[40];
	char	EXTERNAL_TRANSACTION_XID		[32];
	char	SCORE_CUSTOMER_ACCOUNT_XID		[19];
	char	AUTHORIZATION_POSTING_XCD		[1];
	char	CARD_POSTAL_XCD					[9];
	char	CARD_COUNTRY_XCD				[3];
	char	CARD_OPEN_DT					[8];
	char	CARD_ISSUE_DT					[8];
	char	CARD_ISSUE_TYPE_XCD				[1];
	char	ACCOUNT_EXPIRATION_DT			[8];
	char	CARD_EXPIRATION_DT				[8];	
	char	PRIOR_CREDIT_LIMIT_AMT			[10];
	char	CARD_CREDIT_LIMIT_AMT			[10];
	char	CUSTOMER_GENDER_XCD				[1];
	char	CUSTOMER_BIRTH_DT				[8];
	char	CUSTOMER_CARD_CNT				[3];
	char	INCOME_AMT						[10];
	char	CARD_TYPE_XCD					[1];
	char	PRIMARY_CARD_USE_XCD			[1];
	char	TRANSACTION_DATE				[8];
	char	TRANSACTION_TIME				[6];
	char	TRANSACTION_AMT					[13];
	char	TRANSACTION_ISO_CURRENCY_XCD	[3];
	char	TRAN_ISO_CURRENCY_CNVRSN_RATIO	[13];
	char	DECISION_XCD					[1];
	char	TRANSACTION_TYPE_XCD			[1];
	char	MERCHANT_CATEGORY_XCD			[4];
	char	MERCHANT_POSTAL_XCD				[9];
	char	MERCHANT_COUNTRY_XCD			[3];
	char	TRANSACTION_PIN_VERIFY_XCD		[1];
	char	CVV_VERIFY_XCD					[1];
	char	TRANSACTION_POSTING_ENTRY_XFLG	[1];
	char	TRANSACTION_POSTING_DT			[8];
	char	AUTHORIZATION_POSTING_MIS_XCD	[1];
	char	TRANS_POSTING_MISMATCH_XFLG		[1];
	char	CREATE_CASE_XFLG				[1];
	char	USER_INDICATOR_1_XCD			[1];
	char	USER_INDICATOR_2_XCD			[1];
	char	USER_DATA_1_STRG				[10];
	char	USER_DATA_2_STRG				[10];
	char	MERCHANT_XIDD					[10];
	char	MERCHANT_DATA_XFLG				[1];
	char	CARD_DATA_AVAILABLE_XFLG		[1];
	char	EXTERNAL_1_SCR					[4];
	char	EXTERNAL_2_SCR					[4];
	char	EXTERNAL_3_SCR					[4];
	char	CARDHOLDER_PRESENT_XFLG			[1];
	char	CAT_TYPE_XFLG					[1];
	char	TESTING_RANDOM_DIGITS_STRG		[2];
	char	PORTFOLIO_NAME					[14];
	char	CLIENT_2_XID					[14];
	char	INTERBANK_CARD_ASSOCIATION_NUM	[6];
	char	MERCHANT_NAME					[40];
	char	MERCHANT_CITY_NAME				[30];
	char	MERCHANT_STATE_XCD				[3];
	char	SUPPRES_CASE_XFLG				[1];
	char	USER_INDICATOR_3_XCD			[5];
	char	USER_INDICATOR_4_XCD			[5];
	char	USER_DATA_3_STRG				[15];
	char	USER_DATA_4_STRG				[20];
	char	USER_DATA_5_STRG				[40];
	char	REAL_TIME_REQUEST_XFLG			[1];
	char	PRIOR_ACTION_DATABASE_XCD		[1];
	char	PRIOR_ACTION_DB_EXPIRATION_DT	[8];
	char	MASTER_ACCOUNT_NUMBER_XID		[19];
	char	CARD_OFFLN_STATIC_AUTHCTN_XFLG	[1];
	char	CARD_DYNAMIC_AUTHCTN_XCD		[1];
	char	RESERVED_02						[1];
	char	CARD_AIP_VERIFICATION_XFLG		[1];
	char	CARD_AIP_RISK_XFLG				[1];
	char	CARD_ISSUER_AUTH_SUPPORT_XFLG	[1];
	char	CARD_COMBINED_AUTHCTN_XFLG		[1];
	char	TRANSACTION_DELINQUENT_CYC		[1];
	char	TRANS_TOTAL_DELINQUENT_AMT		[13];
	char	CRD_STATEMENT_CASH_BALANCE_AMT	[13];
	char	MERCHANT_BALANCE_AMT			[13];
	char	CUSTOMER_POSTAL_XCD				[12];
	char	CARD_MEDIA_TYPE_XCD				[1];
	char	CVV2_PRESENT_XFLG				[1];
	char	CVV2_RESPONSE_XFLG				[1];
	char	AVS_RESPONSE_XCD				[1];
	char	TRANSACTION_CATEGORY_XCD		[1];
	char	ACQUIRER_XID					[12];
	char	ACQUIRER_COUNTRY_XCD			[3];
	char	TERMINAL_XID					[16];
	char	TERMINAL_TYPE_XCD				[1];
	char	TERMINAL_ENTRY_CAPABILITY_XCD	[1];
	char	TRANSACTION_POS_CONDITION_XCD	[2];
	char	ATM_NETWORK_XID					[1];
	char	RESERVED_01						[1];
	char	CHECK_NUM						[6];
	char	TERML_VERIFICATION_RESULTS_XCD	[10];
	char	CARD_VERIFICATION_RESULTS_XCD	[10];
	char	AUTHZN_RQST_CRYPTO_VALID_XFLG	[1];
	char	ATC_CARD_CNT					[5];
	char	ATC_HOST_CNT					[5];
	char	RESERVED_03						[2];
	char	TERML_TO_CRD_OFFLINE_LIMIT_XCD	[2];
	char	TERMINAL_OFFLINE_LIMIT_MAX_XCD	[2];
	char	RECURRING_AUTHORIZATION_XCD		[2];
	char	RECURRING_AUTHZN_EXPIRATION_DT	[8];
	char	CARD_ASSOCIATION_XFLG			[1];
	char	CARD_INCENTIVE_XCD				[1];
	char	CARD_STATUS_XCD					[2];
	char	CARD_STATUS_DT					[8];
	char	PROCESSOR_REASON_XCD			[5];
	char	TRANSACTION_ADVICE_XCD			[1];
	char	MERCHANT_XID					[16];
	char	CARD_ORDER_TYPE_XCD				[1];
	char	TRANSACTION_CASHBACK_AMT		[13];
	char	USER_DATA_6_NUM					[13];
	char	USER_DATA_7_STRG				[40];
	char	PAYMENT_INSTRUMENT_XID			[30];
	char	AVS_REQUEST_XCD					[1];
	char	CVR_OFFLINEPIN_VERIFYPERF_XFLG	[1];
	char	CVR_OFFLINEPIN_VERIFYSTAT_XFLG	[1];
	char	CVR_PIN_TRYLIMIT_EXCEED_XFLG	[1];
	char	POS_TERMINAL_ATTEND_XFLG		[1];
	char	POS_OFF_PREMISES_XFLG			[1];
	char	POS_CARD_CAPTURE_XFLG			[1];
	char	POS_SECURITY_XFLG				[1];
	char	AUTHORIZATION_XID				[6];
	char	USER_DATA_8_STRG				[10];
	char	USER_DATA_9_STRG				[10];
	char	USER_INDICATOR_5_XCD			[1];
	char	USER_INDICATOR_6_XCD			[1];
	char	USER_INDICATOR_7_XCD			[5];
	char	USER_INDICATOR_8_XCD			[5];
	char	MODEL_CONTROL_1_XCD				[1];
	char	MODEL_CONTROL_2_XCD				[1];
	char	MODEL_CONTROL_3_XCD				[1];
	char	MODEL_CONTROL_4_XCD				[1];
	char	AUTHORIZATION_EXPIRE_DT_XFLG	[1];
	char	AUTH_SECONDARY_VERIFY_XCD		[1];
	char	AUTHORIZATION_RESPONSE_XCD		[1];
	char	SYSTEM_TRACE_NUM		        [6];
}Cr_Fld;

struct
{
	char    HeaderFile                      [101];
	char	WORKFLOW_XCD					[16];
	char	RECORD_TYPE_XCD					[8];
	char	DATA_SPECIFICATION_VERSION_XCD	[5];
	char	CLIENT_XID						[16];
	char	RCD_CRT_DATE					[8];
	char	RCD_CRT_TIME					[6];
	char	RCD_CRT_MSCND					[3];
	char	GMT_OFFSET						[6];
	char	CUSTOMER_XID					[20];
	char	ACCOUNT_REFERENCE_XID			[40];
	char	EXTERNAL_TRANSACTION_XID		[32];
	char	SCORE_CUSTOMER_ACCOUNT_XID		[19];
	char	AUTHORIZATION_POSTING_XCD		[1];
	char	CARD_POSTAL_XCD					[9];
	char	CARD_COUNTRY_XCD				[3];
	char	CARD_OPEN_DT					[8];
	char	CARD_ISSUE_DT					[8];
	char	CARD_ISSUE_TYPE_XCD				[1];
	char	ACCOUNT_EXPIRATION_DT			[8];
	char	CARD_EXPIRATION_DT				[8];
	char	PRIOR_CREDIT_LIMIT_AMT			[10];
	char	CARD_CREDIT_LIMIT_AMT			[10];
	char	CUSTOMER_GENDER_XCD				[1];
	char	CUSTOMER_BIRTH_DT				[8];
	char	CUSTOMER_CARD_CNT				[3];
	char	INCOME_AMT						[10];
	char	CARD_TYPE_XCD					[1];
	char	PRIMARY_CARD_USE_XCD			[1];
	char	TRANSACTION_DATE				[8];
	char	TRANSACTION_TIME				[6];
	char	TRANSACTION_AMT					[13];
	char	TRANSACTION_ISO_CURRENCY_XCD	[3];
	char	TRAN_ISO_CURRENCY_CNVRSN_RATIO	[13];
	char	DECISION_XCD					[1];
	char	TRANSACTION_TYPE_XCD			[1];
	char	MERCHANT_CATEGORY_XCD			[4];
	char	MERCHANT_POSTAL_XCD				[9];
	char	MERCHANT_COUNTRY_XCD			[3];
	char	TRANSACTION_PIN_VERIFY_XCD		[1];
	char	CVV_VERIFY_XCD					[1];
	char	TRANSACTION_POSTING_ENTRY_XFLG	[1];
	char	TRANSACTION_POSTING_DT			[8];
	char	AUTHORIZATION_POSTING_MIS_XCD	[1];
	char	TRANS_POSTING_MISMATCH_XFLG		[1];
	char	CREATE_CASE_XFLG				[1];
	char	USER_INDICATOR_1_XCD			[1];
	char	USER_INDICATOR_2_XCD			[1];
	char	USER_DATA_1_STRG				[10];
	char	USER_DATA_2_STRG				[10];
	char	MERCHANT_XIDD					[10];
	char	MERCHANT_DATA_XFLG				[1];
	char	CARD_DATA_AVAILABLE_XFLG		[1];
	char	EXTERNAL_1_SCR					[4];
	char	EXTERNAL_2_SCR					[4];
	char	EXTERNAL_3_SCR					[4];
	char	CARDHOLDER_PRESENT_XFLG			[1];
	char	CAT_TYPE_XFLG					[1];
	char	TESTING_RANDOM_DIGITS_STRG		[2];
	char	PORTFOLIO_NAME					[14];
	char	CLIENT_2_XID					[14];
	char	INTERBANK_CARD_ASSOCIATION_NUM	[6];
	char	MERCHANT_NAME					[40];
	char	MERCHANT_CITY_NAME				[30];
	char	MERCHANT_STATE_XCD				[3];
	char	SUPPRES_CASE_XFLG				[1];
	char	USER_INDICATOR_3_XCD			[5];
	char	USER_INDICATOR_4_XCD			[5];
	char	USER_DATA_3_STRG				[15];
	char	USER_DATA_4_STRG				[20];
	char	USER_DATA_5_STRG				[40];
	char	REAL_TIME_REQUEST_XFLG			[1];
	char	PRIOR_ACTION_DATABASE_XCD		[1];
	char	PRIOR_ACTION_DB_EXPIRATION_DT	[8];
	char	MASTER_ACCOUNT_NUMBER_XID		[19];
	char	CARD_OFFLN_STATIC_AUTHCTN_XFLG	[1];
	char	CARD_DYNAMIC_AUTHCTN_XCD		[1];
	char	RESERVED_02						[1];
	char	CARD_AIP_VERIFICATION_XFLG		[1];
	char	CARD_AIP_RISK_XFLG				[1];
	char	CARD_ISSUER_AUTH_SUPPORT_XFLG	[1];
	char	CARD_COMBINED_AUTHCTN_XFLG		[1];
	char	TRANSACTION_DELINQUENT_CYC		[1];
	char	TRANS_TOTAL_DELINQUENT_AMT		[13];
	char	CRD_STATEMENT_CASH_BALANCE_AMT	[13];
	char	MERCHANT_BALANCE_AMT			[13];
	char	CUSTOMER_POSTAL_XCD				[12];
	char	CARD_MEDIA_TYPE_XCD				[1];
	char	CVV2_PRESENT_XFLG				[1];
	char	CVV2_RESPONSE_XFLG				[1];
	char	AVS_RESPONSE_XCD				[1];
	char	TRANSACTION_CATEGORY_XCD		[1];
	char	ACQUIRER_XID					[12];
	char	ACQUIRER_COUNTRY_XCD			[3];
	char	TERMINAL_XID					[16];
	char	TERMINAL_TYPE_XCD				[1];
	char	TERMINAL_ENTRY_CAPABILITY_XCD	[1];
	char	TRANSACTION_POS_CONDITION_XCD	[2];
	char	ATM_NETWORK_XID					[1];
	char	RESERVED_01						[1];
	char	CHECK_NUM						[6];
	char	TERML_VERIFICATION_RESULTS_XCD	[10];
	char	CARD_VERIFICATION_RESULTS_XCD	[10];
	char	AUTHZN_RQST_CRYPTO_VALID_XFLG	[1];
	char	ATC_CARD_CNT					[5];
	char	ATC_HOST_CNT					[5];
	char	RESERVED_03						[2];
	char	TERML_TO_CRD_OFFLINE_LIMIT_XCD	[2];
	char	TERMINAL_OFFLINE_LIMIT_MAX_XCD	[2];
	char	RECURRING_AUTHORIZATION_XCD		[2];
	char	RECURRING_AUTHZN_EXPIRATION_DT	[8];
	char	CARD_ASSOCIATION_XFLG			[1];
	char	CARD_INCENTIVE_XCD				[1];
	char	CARD_STATUS_XCD					[2];
	char	CARD_STATUS_DT					[8];
	char	PROCESSOR_REASON_XCD			[5];
	char	TRANSACTION_ADVICE_XCD			[1];
	char	MERCHANT_XID					[16];
	char	CARD_ORDER_TYPE_XCD				[1];
	char	TRANSACTION_CASHBACK_AMT		[13];
	char	USER_DATA_6_NUM					[13];
	char	USER_DATA_7_STRG				[40];
	char	PAYMENT_INSTRUMENT_XID			[30];
	char	AVS_REQUEST_XCD					[1];
	char	CVR_OFFLINEPIN_VERIFYPERF_XFLG	[1];
	char	CVR_OFFLINEPIN_VERIFYSTAT_XFLG	[1];
	char	CVR_PIN_TRYLIMIT_EXCEED_XFLG	[1];
	char	POS_TERMINAL_ATTEND_XFLG		[1];
	char	POS_OFF_PREMISES_XFLG			[1];
	char	POS_CARD_CAPTURE_XFLG			[1];
	char	POS_SECURITY_XFLG				[1];
	char	AUTHORIZATION_XID				[6];
	char	USER_DATA_8_STRG				[10];
	char	USER_DATA_9_STRG				[10];
	char	USER_INDICATOR_5_XCD			[1];
	char	USER_INDICATOR_6_XCD			[1];
	char	USER_INDICATOR_7_XCD			[5];
	char	USER_INDICATOR_8_XCD			[5];
	char	MODEL_CONTROL_1_XCD				[1];
	char	MODEL_CONTROL_2_XCD				[1];
	char	MODEL_CONTROL_3_XCD				[1];
	char	MODEL_CONTROL_4_XCD				[1];
	char	AUTHORIZATION_EXPIRE_DT_XFLG	[1];
	char	AUTH_SECONDARY_VERIFY_XCD		[1];
	char	AUTHORIZATION_RESPONSE_XCD		[1];
	char	SYSTEM_TRACE_NUM		        [6];
}Cr_Fld_RT;

struct
{
		struct {
			char EH_APP_DATA_LEN					[9];
			char EH_EXT_HDR_LEN						[5];
			char EH_TRAN_CODE						[10];
			char EH_SOURCE							[11];
			char EH_DEST							[11];
			char EH_ERROR							[11];
			char EH_FILLER							[2];
			char EH_EXT_HDR							[MAX_EH_EXT_HDR_LEN	 + 1];
		}Scr_Rsp_Hdr;

		struct {
			char RESPONSE_RECORD_VERSION			[2];
			char SCORE_COUNT						[3];
			char SCORE_NAME_1						[23];
			char ERROR_CODE_1						[5];
			char SCORE_1							[5];
			char REASON_1_1							[5];
			char REASON_1_2							[5];
			char REASON_1_3							[5];
			char SCORE_NAME_2						[23];
			char ERROR_CODE_2						[5];
			char SCORE_2							[5];
			char REASON_2_1							[5];
			char REASON_2_2							[5];
			char REASON_2_3							[5];
			char SCORE_NAME_3						[23];
			char ERROR_CODE_3						[5];
			char SCORE_3							[5];
			char REASON_3_1							[5];
			char REASON_3_2							[5];
			char REASON_3_3							[5];
			char SCORE_NAME_4						[23];
			char ERROR_CODE_4						[5];
			char SCORE_4							[5];
			char REASON_4_1							[5];
			char REASON_4_2							[5];
			char REASON_4_3							[5];
			char SCORE_NAME_5						[23];
			char ERROR_CODE_5						[5];
			char SCORE_5							[5];
			char REASON_5_1							[5];
			char REASON_5_2							[5];
			char REASON_5_3							[5];
			char SCORE_NAME_6						[23];
			char ERROR_CODE_6						[5];
			char SCORE_6							[5];
			char REASON_6_1							[5];
			char REASON_6_2							[5];
			char REASON_6_3							[5];
			char SCORE_NAME_7						[23];
			char ERROR_CODE_7						[5];
			char SCORE_7							[5];
			char REASON_7_1							[5];
			char REASON_7_2							[5];
			char REASON_7_3							[5];
			char SCORE_NAME_8						[23];
			char ERROR_CODE_8						[5];
			char SCORE_8							[5];
			char REASON_8_1							[5];
			char REASON_8_2							[5];
			char REASON_8_3							[5];
			char SEGMENT_ID_1						[9];
			char SEGMENT_ID_2						[9];
			char SEGMENT_ID_3						[9];
			char FILLER1_1							[3];
			char FILLER1_2							[5];
			char FILLER1_3							[3];
			char SEGMENT_ID_4						[9];
			char SEGMENT_ID_5						[9];
			char SEGMENT_ID_6						[9];
			char SEGMENT_ID_7						[9];
			char FILLER2_1							[5];
			char FILLER2_2							[5];
			char SEGMENT_ID_8						[9];
			char FILLER3							[5];
			char DECISION_COUNT						[3];
			char DECISION_TYPE_1					[33];
			char DECISION_CODE_1					[33];
			char DECISION_TYPE_2					[33];
			char DECISION_CODE_2					[33];
			char DECISION_TYPE_3					[33];
			char DECISION_CODE_3					[33];
			char DECISION_TYPE_4					[33];
			char DECISION_CODE_4					[33];
			char DECISION_TYPE_5					[33];
			char DECISION_CODE_5					[33];
			char DECISION_TYPE_6					[33];
			char DECISION_CODE_6					[33];
			char DECISION_TYPE_7					[33];
			char DECISION_CODE_7					[33];
			char DECISION_TYPE_8					[33];
			char DECISION_CODE_8					[33];
			char DECISION_TYPE_9					[33];
			char DECISION_CODE_9					[33];
			char DECISION_TYPE_10					[33];
			char DECISION_CODE_10					[33];
			char SCORING_SERVER_ID					[5];
		}Scr_Rsp_Dat;

}Rsp_msg;

CHAR strAppName [80];
CHAR ServiceName[80];

char javaEndProcessSignalled[1];

CHAR  Version[] = "ATP_11.14.4";
static CHAR message[1061]= {0};

INT  MainProcessDone;
INT  EndProcessSignalled;
CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";
CHAR  time_milliseconds[4] = "";


CHAR  faircv_Erro_warning_Filename[256] = {0};
CHAR  faircv_module_error_warning_file_name[256] = {0};
CHAR  faircv_error_warning_file_path[256] = {0};
UINT  Max_File_Size_Defined = 0 ;
CHAR  App_Name[7] = "faircv";

/******************************************************************************
*
*  NAME:         GET_DATE
*
*  DESCRIPTION:  This procedure takes an input string that contains a date and
*                time.  It copies the date portion into an output string,
*                formatted slightly different.
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
*
*  OUTPUTS:      date_str  : Format = "YYYY0M0D"
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Dennis Irby
*
******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
}

/******************************************************************************
*
*  NAME:         GET_TIME
*
*  DESCRIPTION:  This procedure takes an input string that contains a 
date and
*                time.  It copies the time portion into an output 
string,
*                formatted slightly different.
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
*
*  OUTPUTS:      time_str  : Format = "0H0I0S"
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:
******************************************************************************/
 void get_time( pCHAR time_date, pCHAR time_str )
 {
    memset ( time_str,0x00,7 );
    memset(time_milliseconds, 0x00,4 );
    strncpy( time_str, &time_date[11], 2 );
    strncat( time_str, &time_date[14], 2 );
    strncat( time_str, &time_date[17], 2 );
    strncpy( time_milliseconds, &time_date[20], 3 );
 }

/******************************************************************************
 *
 *  NAME:         faircv_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Raghu
 *
 ******************************************************************************/
void faircv_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [2000] = {0};
   INT  Appname_len = 0;

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag
   if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;*/

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",App_Name);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   faircv_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
		if(details == 1)
		{
			TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 Auth_Tx.TLF01_details.terminal_id,
										 Auth_Tx.TLF01_details.card_num,
										 Auth_Tx.TLF01_details.merchant_id );
		}
		else
		{
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
		}
   }
   return;
}

INT faircv_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[2000]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(faircv_Erro_warning_Filename);
    path_len = strlen(faircv_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&faircv_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	faircv_create_Error_Filename();
    }
	if((fp = fopen(faircv_Erro_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	if(detail == 1)
	{
		if (strlen(Auth_Tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( Auth_Tx.TLF01_details.card_num );
			strncpy( tempcard, Auth_Tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		/* Do nothing fall below */
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void faircv_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( faircv_Erro_warning_Filename, faircv_error_warning_file_path );
    strcat( faircv_Erro_warning_Filename, faircv_module_error_warning_file_name );
	strcat( faircv_Erro_warning_Filename, ".EW." );
    strncat(faircv_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(faircv_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(faircv_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}


void EndProcess()
{
      CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down %s Service, version %s",
            ServiceName, Version );
   LogEvent( Buffer, INFO_MSG );
   strcat( Buffer, "\n" );
   pteipc_shutdown_multiple_instance_app();
   pteipc_trace_off();

}

/******************************************************************************
 *
 *  NAME:         SEND_MSG
 *
 *  DESCRIPTION:  This function sends a PTE message to a specified queue.
 *
 *  INPUTS:       p_msg     - Pointer to a PTE_MSG structure
 *                que_name  - Name of the destination queue
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg )
{
   INT      ret_val = true;
   LONG     ret_code;
   CHAR     Buff[256];
   CHAR     pte_err_mesg[200] = "";

   ret_code = pteipc_send_falcon( p_msg, que_name, t_id);
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, pte_err_mesg );
      sprintf(Buff,"pteipc error sending to que: %s. %s, :%ld",que_name,pte_err_mesg, ret_code);
      memcpy( error_msg, Buff, 99 );
      ret_val = false;
   }
   return( ret_val );
}


INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2,
                                    pBYTE    que_name,
                                    pBYTE    buffer,
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg )
{
   CHAR            Buff[200];
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;
   LONG            msg_ret_code;

   p_msg = ptemsg_build_msg( msgtype,
                             subtype1,
                             subtype2,
                             que_name,
                             application_que_name,
                             buffer,
                             length,
                             app_type );

   if ( NULL_PTR == p_msg  )
   {
      strcpy( err_msg, "Insufficient Memory to build PTE message" );
      ret_code = false;
   }
   else
   {
      /* Set originator communications information. */
      if ( 0 != strcmp( NULL_STR, comm_info ) )
         ptemsg_set_orig_comms_info( p_msg, comm_info );

      /* Send message que_name */
      msg_ret_code = pteipc_send_falcon(p_msg, que_name,t_id);
      if(msg_ret_code < 0)
	  {
         pteipc_get_errormsg(msg_ret_code, err_msg);
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff, "Failed transmit to queue: " );
         strcat( Buff, que_name );
         strncat(Buff, err_msg, 65 );
         ret_code = false;
	  }
      free(p_msg);
   }
   return( ret_code );
}

void java_signal_handler( int signal )
{
   /*printf("faircv:Recived Signal Handler\n");*/
   memcpy(javaEndProcessSignalled,"1",1);
   sleep( 1 );
}
 

INT FAIrcvHandler_initialize(CHAR* appName, CHAR* serName)
{
	INT ret_val = true;
	CHAR err_buf[100] = { 0 };
	CHAR tf_filename[256] = { 0 };
	CHAR tmpstr[256] = { 0 };
	CHAR ini_info[MAX_INI_INFO_SIZE] = { 0 };
	CHAR filename[MAX_FILENAME_SIZE] = { 0 };
	CHAR tmpstring[MAX_INI_INFO_SIZE] = { 0 };

	GetAscendentConfigDirectory(tmpstr);
	sprintf(tf_filename, "%stf.ini", tmpstr);

	faircv_get_error_warning_file_name_path();

	memcpy(javaEndProcessSignalled, "0", 1);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN) {
		signal(SIGTERM, java_signal_handler);
		/*printf("Signal Handler Has Been Set\n");*/
	}
	EndProcessSignalled = 0;
	CHAR strTemp[256] = { 0 };
	memset(&Auth_Tx, 0, sizeof Auth_Tx);
	memset(strAppName, 0x00, sizeof(strAppName));
	memset(ServiceName, 0x00, sizeof(ServiceName));
	const char *str;
	const char *str1;

	char temp[200] = { 0 };
	memset(&message, 0, sizeof(message));
	str = appName;
	if (str == NULL) {
		return INTIALIZE_EXCEPTION_ERROR; /* OutOfMemoryError already thrown */
	}
	memcpy(strAppName, str, sizeof(str));

	str1 = serName;
	if (str1 == NULL) {
		return INTIALIZE_EXCEPTION_ERROR; /* OutOfMemoryError already thrown */
	}
	memcpy(ServiceName, str1, sizeof(str1));

	/* Display Application version. */
	sprintf(strTemp, "Attempting to start %s, version %s", strAppName, Version);
	LogEvent(strTemp, INFO_MSG);

	/*#ifdef _DEBUG*/
	if (!pteipc_init_multiple_instance_app(strAppName, ServiceName, "pte_ipc"))
	/*#endif*/
	{
		sprintf(strTemp, "%s: Intialize: Failed to Create XIPC queues",
				strAppName);
		LogEvent(strTemp, ERROR_MSG);
		/* signal that we are ready to shut down and quit */
		/*#ifdef _DEBUG*/
		pteipc_shutdown_multiple_instance_app();
		/* #endif*/
		MainProcessDone = 1;

		return INTIALIZE_QUEUES_ERROR;

	}

	if (t_id == 0)
	{
		t_id = pthread_self();
		memset(strTemp, 0x00, sizeof(strTemp));
		sprintf(strTemp, "Service_name %s, version %s", strAppName, Version);
		faircv_log_message(2,1,strTemp,"FAIrcvHandler_initialize",1);
		sprintf(strTemp, "Attempting to start %s, servcname %s, tid = %d, PID = %d", strAppName, ServiceName,t_id,getpid() );
		faircv_log_message(1,1,strTemp,"FAIrcvHandler_initialize",1);

	}

	/**** ret_val = create_shared_mem_table( err_buf );*/

	sprintf(strTemp, "Started the FAIrcvHandler Service: %s", ServiceName);
	LogEvent(strTemp, INFO_MSG);

	return INTIALIZE_QUEUES_NO_ERROR;
}


void faircv_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  faircv_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   faircv_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(faircv_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  App_Name,       	/* points to key name             */
								  "",  					/* Default string                 */
								  faircv_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(faircv_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  faircv_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(faircv_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(faircv_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(faircv_error_warning_file_path) > 0) &&
	  (strlen(faircv_module_error_warning_file_name)> 0))
   {
	   faircv_create_Error_Filename();
   }
}

INT Parse_Rsp_Msg(char * ResponseMsg)
{
	CHAR respbuffer[5000] = { 0 };
	CHAR respn_msg[5000] = { 0 };
	CHAR strTemp[256] = { 0 };
	INT msglen = 0;
	CHAR ErrorMsg[100] = "";
	const char *str2;
	INT len_ext_hdr = 0;
	INT tran_type = 0;
char buffer[256]={0};
	str2 = ResponseMsg;
	memset(&Rsp_msg, 0x00, sizeof(Rsp_msg));

	if (str2 == NULL)
	{
			strcpy(strTemp,
					"FaclonRT: receiveResponseMsg:Parameter passed from Java is NULL");
			LogEvent(strTemp, ERROR_MSG);
	}
#if 0
	/* Check the first byte is R or T */
	if ( 0 == strncmp(str2, "R", 1))

	{
		tran_type = RT_RESPONSE; /* response mode */
		str2 = str2 +1;
	}
	else if(0 == strncmp(str2, "T", 1))
	{
		tran_type = RT_TIMEOUT; /* timeout mode */
		str2 = str2 +1;

	}
	else if(0 == strncmp(str2, "S", 1))
	{
		tran_type = RT_SYSTEM; /* ActvMQ write fail mode */
		str2 = str2 +1;
	}
	else
	{
		return RT_UNKOWN;
	}
#endif

	if(0 == strncmp(str2, "S", 1))
	{
		tran_type = RT_SYSTEM; /* ActvMQ write fail mode */
		str2 = str2 +1;
	}
	else if(0 == strncmp(str2, " ", 1))
	{
		tran_type = RT_RESPONSE; /* Realtime response mode */
		str2 = str2 +1;
	}
	else if (0 == strncmp(str2, "R", 1))
	{
		tran_type = RT_RESPONSE; /* Realtime response R mode */
		str2 = str2 +1;
	}
	else if (0 == strncmp(str2, "I", 1))
	{
		tran_type = RT_RESPONSE; /* Realtime response R mode */
		str2 = str2 +1;
	}
	else
	{
		faircv_log_message(1,3,"faircvRT: not received response from FAI","faircv_Send_Request_To_Service",1);
		return RT_UNKOWN;
	}

				/* Parse Resp msg and mov to DataFlds */
	/* Field EH_APP_DATA_LEN */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_APP_DATA_LEN, str2, RSP_APP_DATA_LEN);
	str2 = str2 + RSP_APP_DATA_LEN;

	/* Field EH_EXT_HDR_LEN */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR_LEN, str2, RSP_EXT_HDR_LEN);
	str2 = str2 + RSP_EXT_HDR_LEN;

	/* Field EH_TRAN_CODE */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_TRAN_CODE, str2, RSP_TRAN_CODE_LEN);
	str2 = str2 + RSP_TRAN_CODE_LEN;

	/* Field EH_SOURCE */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_SOURCE, str2, RSP_SOURCE_LEN);
	str2 = str2 + RSP_SOURCE_LEN;

	/* Field EH_DEST */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_DEST, str2, RSP_DEST_LEN);
	str2 = str2 + RSP_DEST_LEN;

	/* Field EH_ERROR */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_ERROR, str2, RSP_ERROR_LEN);
	str2 = str2 + RSP_ERROR_LEN;

	/* Field EH_FILLER */
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_FILLER, str2, RSP_FILLER_LEN);
	str2 = str2 + RSP_FILLER_LEN;

	/* Field EH_EXT_HDR */
	len_ext_hdr = atoi(Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR_LEN);
	memcpy(Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, str2, len_ext_hdr);
	str2 = str2 + len_ext_hdr;

	sprintf(buffer,"faircvRT: Response Tranid+RRN (%.31s)",Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR);
	faircv_log_message(2,1,buffer,"Parse_Rsp_Msg",1);

	if (tran_type == RT_RESPONSE)
	{
		/* Field RESPONSE_RECORD_VERSION */
			memcpy(Rsp_msg.Scr_Rsp_Dat.RESPONSE_RECORD_VERSION, str2+SCR_HDR_LEN, RSP_RCRD_VER_LEN);
			str2 = str2+RSP_RCRD_VER_LEN;

			/* Field SCORE_COUNT */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_COUNT, str2, RSP_SCR_CNT_LEN);
			str2 += RSP_SCR_CNT_LEN;

			/* Field SCORE_NAME_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_1, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_1, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_1, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_1_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_1_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_1_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_1_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_1_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_1_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_2, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_2, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_2, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_2_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_2_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_2_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_2_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_2_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_2_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_3, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_3, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_3, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_3_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_3_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_3_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_3_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_3_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_3_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_4 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_4, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_4 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_4, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_4 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_4, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_4_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_4_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_4_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_4_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_4_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_4_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_5 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_5, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_5 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_5, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_5 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_5, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_5_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_5_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_5_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_5_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_5_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_5_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_6 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_6, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_6 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_6, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_6 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_6, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_6_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_6_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_6_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_6_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_6_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_6_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_7 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_7, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_7 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_7, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_7 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_7, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_7_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_7_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_7_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_7_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_7_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_7_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SCORE_NAME_8 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_NAME_8, str2, RSP_SCORE_NAME_LEN);
			str2 += RSP_SCORE_NAME_LEN;

			/* Field ERROR_CODE_8 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.ERROR_CODE_8, str2, RSP_ERROR_CODE_LEN);
			str2 += RSP_ERROR_CODE_LEN;

			/* Field SCORE_8 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORE_8, str2, RSP_SCORE_LEN);
			str2 += RSP_SCORE_LEN;

			/* Field REASON_8_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_8_1, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_8_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_8_2, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field REASON_8_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.REASON_8_3, str2, RSP_REASON_LEN);
			str2 += RSP_REASON_LEN;

			/* Field SEGMENT_ID_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_1, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field SEGMENT_ID_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_2, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field SEGMENT_ID_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_3, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field FILLER1_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.FILLER1_1, str2, RSP_FILLER1_1_3_LEN);
			str2 += RSP_FILLER1_1_3_LEN;

			/* Field FILLER1_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.FILLER1_2, str2, RSP_FILLER1_2_LEN);
			str2 += RSP_FILLER1_2_LEN;

			/* Field FILLER1_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.FILLER1_3, str2, RSP_FILLER1_1_3_LEN);
			str2 += RSP_FILLER1_1_3_LEN;

			/* Field SEGMENT_ID_4 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_4, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field SEGMENT_ID_5 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_5, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field SEGMENT_ID_6 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_6, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field SEGMENT_ID_7 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_7, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field FILLER2_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.FILLER2_1, str2, RSP_FILLER2_LEN);
			str2 += RSP_FILLER2_LEN;

			/* Field FILLER2_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.FILLER2_2, str2, RSP_FILLER2_LEN);
			str2 += RSP_FILLER2_LEN;

			/* Field SEGMENT_ID_8 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SEGMENT_ID_8, str2, RSP_SEGMENT_ID_LEN);
			str2 += RSP_SEGMENT_ID_LEN;

			/* Field FILLER3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.FILLER3, str2, RSP_FILLER3_LEN);
			str2 += RSP_FILLER3_LEN;

			/* Field DECISION_COUNT */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_COUNT, str2, RSP_DECISION_COUNT_LEN);
			str2 += RSP_DECISION_COUNT_LEN;

			/* Field DECISION_TYPE_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_1, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_1 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_1, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_2, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_2 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_2, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_3, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_3 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_3, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_4 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_4, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_4 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_4, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_5 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_5, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_5 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_5, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_6 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_6, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_6 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_6, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_7 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_7, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_7 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_7, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_8 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_8, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_8 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_8, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_9 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_9, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_9 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_9, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field DECISION_TYPE_10 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_TYPE_10, str2, RSP_DECISION_TYPE_LEN);
			str2 += RSP_DECISION_TYPE_LEN;

			/* Field DECISION_CODE_10 */
			memcpy(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_10, str2, RSP_DECISION_CODE_LEN);
			str2 += RSP_DECISION_CODE_LEN;

			/* Field SCORING_SERVER_ID */
			memcpy(Rsp_msg.Scr_Rsp_Dat.SCORING_SERVER_ID, str2, RSP_SCORING_SERVER_ID_LEN);

			/*Copy Trans req sent time for Apprvd/Decl caseto FTLF01*/
			strncpy(Ftlf01.tran_req_sent_time, str2+RSP_SCORING_SERVER_ID_LEN, sizeof(Ftlf01.tran_req_sent_time)-1);
			Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] = '\0';

	}
	else
	{
		/*Copy Trans req sent time for TO and SF case to FTLF01*/
		strncpy(Ftlf01.tran_req_sent_time, &str2[RSP_TR_RQ_TIME], sizeof(Ftlf01.tran_req_sent_time)-1);
		Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] = '\0';

	}


	return tran_type;
}

 
void FAIrcvHandler_terminate()
{

	CHAR strTemp[256] = { 0 };
	CHAR SerName[20] = { 0 };
	strcpy(SerName, "FAIrcvHandler");
	sprintf(strTemp, "Shutting down the FAIrcvHandler Service: %s", SerName);
	LogEvent(strTemp, INFO_MSG);

	/* Log out of XIPC
	 #ifdef _DEBUG*/
	pteipc_shutdown_multiple_instance_app();
	/* #endif*/
	MainProcessDone = 1;
}

BYTE Faircv_read_response_from_FAI (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	pPTE_MSG p_msg_timer_out = NULL_PTR;
	CHAR	 lookup_key[22] = {0};
	INT nBufferSize = 0 ;
	INT retVal		= 0 ;
	INT   seconds = 0;
	CHAR strError[512] = {0} ;
	CHAR   transcn_id[21] = {0};
    CHAR logger_que_name[256] = {0} ;
	CHAR orig_comms_info [MAX_ORIG_INFO_SIZE] = {0} ;
	CHAR  system_date[25] = {0};
	CHAR	date_str [ 9] = {0};
	CHAR	time_str [ 7] = {0};
	CHAR	time_date[25] = {0};
	CHAR	temp_time[11] = {0};
	CHAR 	temp_tran[21] = {0};
	INT		tid	= 0;
	char  yyyymmdd[25] = {0};
	BYTE buffer [MAX_DATASERVER_BUFFER_LENGTH] ={0};
	char err_buf[256]={0};

	memset (p_buffer, 0x00, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof(Auth_Tx)) ;

	memset(&Ftlf01, 0x00, sizeof(Ftlf01));

	/* call the parse func */
	ret_val = Parse_Rsp_Msg(p_data);

	memcpy(buffer+sizeof(Auth_Tx),p_data,1);
	memcpy(buffer+sizeof(Auth_Tx)+1,&Rsp_msg,sizeof(Rsp_msg));
	/*
	 *
	 * Send the response recieved from FAI to the new service
	 * */

	if (p_data[0] == ' ')
	{
	retVal =	build_and_send_request_message( MT_AUTH_RESPONSE,
									0,
									0,
									(pBYTE)faircv_controller_que_name,
									(pBYTE)&buffer,
									(Auth_Tx_Size+sizeof(Rsp_msg)+1),
									0,
									NULL_STR,
									err_buf  );
	faircv_log_message(2,1,"faircvRT: Sending the response to faircv controller","faircv_Send_Request_To_Service",1);
	}
	else if (p_data[0] == 'I')
	{

	retVal =	build_and_send_request_message( MT_AUTH_RT_EMPTY_RESPONSE,
									0,
									0,
									(pBYTE)txcntl_que_name,
									(pBYTE)&buffer,
									(Auth_Tx_Size+sizeof(Rsp_msg)+1),
									0,
									NULL_STR,
									err_buf  );
	faircv_log_message(2,1,"faircvRT: Sending the empty response received as it is to faircv controller","faircv_Send_Request_To_Service",1);
	}



	return DEVICE_OK;

#if 0
	/* copy the transcn id */
	strncpy(temp_tran, Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, 16);
	strcat(temp_tran, ".");
	strncat(temp_tran, &Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR[16], 3);
	strncpy(Ftlf01.tran_id, temp_tran, sizeof(Ftlf01.tran_id)-1);


	sprintf (strError,"Recieved the response to Servicename:%s,pid: %d, tid: %d", ServiceName, getpid(), pthread_self() );
	faircv_log_message(1,1,strError,"faircv_Send_Request_To_Service",1);

	strncpy( transcn_id, Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, 19);

	strncat(transcn_id, "F",1);
	strncpy(lookup_key, transcn_id, sizeof(transcn_id));

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );
	strncpy(temp_time, &date_str[4], 4);/* MMDD */
	strncat(temp_time, time_str, 6);	/* hhmmss   */
	strncpy( Ftlf01.tran_resp_recv_time, temp_time, sizeof(Ftlf01.tran_resp_recv_time)-1);
	Ftlf01.tran_resp_recv_time[sizeof(Ftlf01.tran_resp_recv_time)-1] ='\0';
	/* copy decision codes */
	strncpy(Ftlf01.decision_code_1_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_1, sizeof(Ftlf01.decision_code_1_value)-1);
	strncpy(Ftlf01.decision_code_2_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_2, sizeof(Ftlf01.decision_code_2_value)-1);
	strncpy(Ftlf01.decision_code_3_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_3, sizeof(Ftlf01.decision_code_3_value)-1);
	strncpy(Ftlf01.decision_code_4_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_4, sizeof(Ftlf01.decision_code_4_value)-1);
	strncpy(Ftlf01.decision_code_5_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_5, sizeof(Ftlf01.decision_code_5_value)-1);
	strncpy(Ftlf01.decision_code_6_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_6, sizeof(Ftlf01.decision_code_6_value)-1);
	strncpy(Ftlf01.decision_code_7_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_7, sizeof(Ftlf01.decision_code_7_value)-1);
	strncpy(Ftlf01.decision_code_8_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_8, sizeof(Ftlf01.decision_code_8_value)-1);
	strncpy(Ftlf01.decision_code_9_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_9, sizeof(Ftlf01.decision_code_9_value)-1);
	strncpy(Ftlf01.decision_code_10_value, Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_10, sizeof(Ftlf01.decision_code_10_value)-1);

	/* call timer approved */
	clear_request_timer_for_Received_Response( lookup_key);

	return DEVICE_OK ;
#endif
} /* faircv_Send_Request_To_Service_New */

