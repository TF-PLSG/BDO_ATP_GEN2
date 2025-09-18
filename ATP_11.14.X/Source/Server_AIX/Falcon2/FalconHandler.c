
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
#include "FalconHandler.h"
#include "global_definitions.h"
#include "Falcon.h"
#include "timings.h"
#include "pte.h"
#include "ptestats.h"

#define   AMX_INVALID_TXN                "12"
#ifndef  false
#define  false    0x00
#endif

#ifndef  true
#define  true     0x01
#endif
INT FALCON_read_CUP_merchant_indicator( void );
void Falcon_build_CRTRAN_field_31_MC();
void Falcon_build_CRTRAN_field_31_VISA();
void Falcon_build_CRTRAN_field_31_CUP();
void Falcon_build_CRTRAN_field_31_JCB();
void Falcon_build_CRTRAN_field_31_DINERS();
void Falcon_build_CRTRAN_field_31_AMEX();
void Falcon_build_CRTRAN_field_31_Fleet();
void RT_Falcon_build_CRTRAN_field_31_MC();
void RT_Falcon_build_CRTRAN_field_31_VISA();
void RT_Falcon_build_CRTRAN_field_31_CUP();
void RT_Falcon_build_CRTRAN_field_31_JCB();
void RT_Falcon_build_CRTRAN_field_31_DINERS();
void RT_Falcon_build_CRTRAN_field_31_AMEX();
void RT_Falcon_build_CRTRAN_field_31_Fleet();
void FALCON_build_request_Online_HeaderFile();
void FALCON_build_request_RealTime_HeaderFile();
INT FALCON_Read_BDO_PAY_QR_NII_value(void);
INT Check_If_CUP_Transaction(void);
INT Check_If_VISA_Transaction (void);
INT Check_If_MC_Transaction (void);
INT Check_If_JCB_Transaction (void);
INT Check_If_AMEX_Transaction (void);
INT Check_If_DINERS_Transaction (void);
void convert_to_ascii(  pBYTE source, pBYTE dest, INT len );
void init_character_code_maps( void );

BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ;

CHAR  CUP_merchant_indicator[8]={0};
CHAR  CUP_MP_Bin_range_1[11]={0};
CHAR  CUP_MP_Bin_range_2[11]={0};
CHAR  CUP_MP_Bin_range_3[11]={0};
CHAR  CUP_MP_Bin_range_4[11]={0};
CHAR  CUP_MP_Bin_range_5[11]={0};
INT   CUP_MP_Bin_range_len_1 = 0;
INT   CUP_MP_Bin_range_len_2 = 0;
INT   CUP_MP_Bin_range_len_3 = 0;
INT   CUP_MP_Bin_range_len_4 = 0;
INT   CUP_MP_Bin_range_len_5 = 0;

/*BDO pay QR transaction*/
CHAR  BDO_PAY_QR_NII_1[5]={0};
CHAR  BDO_PAY_QR_NII_2[5]={0};
CHAR  BDO_PAY_QR_NII_3[5]={0};
CHAR  BDO_PAY_QR_NII_4[5]={0};
CHAR  BDO_PAY_QR_NII_5[5]={0};

#define MAX_EH_EXT_HDR_LEN	1024
#define SCR_HDR_LEN			152

FTLF01       Ftlf01;
AUTH_TX     Auth_Tx;
#define AUTH_TX_SIZE	sizeof(Auth_Tx)
#define FTLF01_SIZE		sizeof(Ftlf01)


CHAR	ServiceCode[4];
CHAR   TimerAppData2[MAX_APP_DATA_SIZE];
CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/* QUEUES */
CHAR  AppName[80];
CHAR  updateds_que_name[]   = "updatdsA";
CHAR  fconds_que_name  []  = "fcondsA";
CHAR  trands_que_name []    = "trandsA";
CHAR  netds_que_name []     = "netdsA";
CHAR  ftimer_que_name []   = "ftimerA";
CHAR  nsp_que_name []       = "racalA";
CHAR  authorizer_que_name[] = "txcntlA";
CHAR  falcon_controller_que_name[] = "falcntlA";
CHAR  applnk_que_name []    = "applnkA";
CHAR  oprmon_que_name[]     = "oprmonI" ;
CHAR  dialog_que_name []    = "dialogA";
CHAR  mp_pos_que_name [] 	= "dcpimpA" ;
CHAR  mp_nclink_que_name [] 	= "nciftrA" ;
CHAR  voice_handler_que_name[]  = "VOICE";

/* STRUCTURES */

NCF01                Ncf01_I;
NCF01                Ncf01_A;
NCF01                Ncf01_Nccup3_I;
NCF01                Ncf01_Nccup_I;
NCF01                Ncf01_Nccup_A;
NCF30                Ncf30;
CRF01                Crf01;
CHAR    			 tpdu_id[16];

/* STRUCTURE SIZES */
INT  Auth_Tx_Size = AUTH_TX_SIZE;
INT  Ncf01_Size;

INT t_secs =0;
INT t_id = 0;
PRIVATE INT ret_val 	= 0 ;
/*#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif */

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
			char EH_EXT_HDR							[MAX_EH_EXT_HDR_LEN + 1];
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

#define INTIALIZE_QUEUES_NO_ERROR 00
#define INTIALIZE_QUEUES_ERROR 01
#define INTIALIZE_EXCEPTION_ERROR 02
#define MAX_INSTANCE_NAME_SIZE   51
#define FIELD67_SIZE 			 5
#define SPACES 					 ' '
/* Currency Codes */
#define  PESO     "608"
#define  USD      "840"
#define CURRENCY_LENGTH 3
CHAR strAppName [80];
CHAR ServiceName[80];

char javaEndProcessSignalled[1];

CHAR  Version[] = "ATP_11.7.0";
static CHAR message[1061]= {0};
static long count = 0;
INT  MainProcessDone;
INT  EndProcessSignalled;
CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";
CHAR  time_milliseconds[4] = "";
CHAR profile_str [17]="";

double   conv_rate   = 0.0;
double temp_conv_amount = 0.0;
char temp_without_decimal[13] = {0};
int len_billing_amount = 0;
int len_billing_currency_code = 0;
int len_settle_currency_code = 0;
double billing_amount = 0.0;
double settlement_total = 0.0;
char last2_decimal[3]={0};
char amount_digit[11]= {0};
char str_temp_amount[13] = {0};
double temp_amount = 0.0;
char temp_iso_ration[25] = {0};
BYTE bcd_eci[5]="";
BYTE ascii_eci[4]="";


BYTE      a2e[257];
BYTE      e2a[257];
BOOLEAN   maps_initialised = false;

CHAR  falcon_Erro_warning_Filename[256] = {0};
CHAR  falcon_module_error_warning_file_name[256] = {0};
CHAR  falcon_error_warning_file_path[256] = {0};
UINT Max_File_Size_Defined = 0 ;
CHAR  App_Name[7] = "falcon";

void init_character_code_maps()
{

   memset( a2e, 0, sizeof( a2e ) );

   a2e[0x30]=0xF0; a2e[0x31]=0xF1; a2e[0x32]=0xF2; a2e[0x33]=0xF3; a2e[0x34]=0xF4;
   a2e[0x35]=0xF5; a2e[0x36]=0xF6; a2e[0x37]=0xF7; a2e[0x38]=0xF8; a2e[0x39]=0xF9;
   a2e[0x61]=0x81; a2e[0x62]=0x82; a2e[0x63]=0x83; a2e[0x64]=0x84; a2e[0x65]=0x85;
   a2e[0x66]=0x86; a2e[0x67]=0x87; a2e[0x68]=0x88; a2e[0x69]=0x89; a2e[0x6A]=0x91;
   a2e[0x6B]=0x92; a2e[0x6C]=0x93; a2e[0x6D]=0x94; a2e[0x6E]=0x95; a2e[0x6F]=0x96;
   a2e[0x70]=0x97; a2e[0x71]=0x98; a2e[0x72]=0x99; a2e[0x73]=0xA2; a2e[0x74]=0xA3;
   a2e[0x75]=0xA4; a2e[0x76]=0xA5; a2e[0x77]=0xA6; a2e[0x78]=0xA7; a2e[0x79]=0xA8;
   a2e[0x7A]=0xA9; a2e[0x41]=0xC1; a2e[0x42]=0xC2; a2e[0x43]=0xC3; a2e[0x44]=0xC4;
   a2e[0x45]=0xC5; a2e[0x46]=0xC6; a2e[0x47]=0xC7; a2e[0x48]=0xC8; a2e[0x49]=0xC9;
   a2e[0x4A]=0xD1; a2e[0x4B]=0xD2; a2e[0x4C]=0xD3; a2e[0x4D]=0xD4; a2e[0x4E]=0xD5;
   a2e[0x4F]=0xD6; a2e[0x50]=0xD7; a2e[0x51]=0xD8; a2e[0x52]=0xD9; a2e[0x53]=0xE2;
   a2e[0x54]=0xE3; a2e[0x55]=0xE4; a2e[0x56]=0xE5; a2e[0x57]=0xE6; a2e[0x58]=0xE7;
   a2e[0x59]=0xE8; a2e[0x5A]=0xE9; a2e[0x20]=0x40; a2e[0x2E]=0x4B; a2e[0x3C]=0x4C;
   a2e[0x28]=0x4D; a2e[0x2B]=0x4E; a2e[0x7C]=0x4F; a2e[0x26]=0x50; a2e[0x21]=0x5A;
   a2e[0x24]=0x5B; a2e[0x2A]=0x5C; a2e[0x29]=0x5D; a2e[0x3B]=0x5E; a2e[0x2D]=0x60;
   a2e[0x2F]=0x61; a2e[0x2C]=0x6B; a2e[0x25]=0x6C; a2e[0x5F]=0x6D; a2e[0x3E]=0x6E;
   a2e[0x3F]=0x6F; a2e[0x3A]=0x7A; a2e[0x23]=0x7B; a2e[0x40]=0x7C; a2e[0x27]=0x7D;
   a2e[0x3D]=0x7E; a2e[0x22]=0x7F; a2e[0x5E]=0x5F;


   memset( e2a, 0, sizeof( e2a ) );

   e2a[0xF0]=0x30; e2a[0xF1]=0x31; e2a[0xF2]=0x32; e2a[0xF3]=0x33; e2a[0xF4]=0x34;
   e2a[0xF5]=0x35; e2a[0xF6]=0x36; e2a[0xF7]=0x37; e2a[0xF8]=0x38; e2a[0xF9]=0x39;
   e2a[0x81]=0x61; e2a[0x82]=0x62; e2a[0x83]=0x63; e2a[0x84]=0x64; e2a[0x85]=0x65;
   e2a[0x86]=0x66; e2a[0x87]=0x67; e2a[0x88]=0x68; e2a[0x89]=0x69; e2a[0x91]=0x6A;
   e2a[0x92]=0x6B; e2a[0x93]=0x6C; e2a[0x94]=0x6D; e2a[0x95]=0x6E; e2a[0x96]=0x6F;
   e2a[0x97]=0x70; e2a[0x98]=0x71; e2a[0x99]=0x72; e2a[0xA2]=0x73; e2a[0xA3]=0x74;
   e2a[0xA4]=0x75; e2a[0xA5]=0x76; e2a[0xA6]=0x77; e2a[0xA7]=0x78; e2a[0xA8]=0x79;
   e2a[0xA9]=0x7A; e2a[0xC1]=0x41; e2a[0xC2]=0x42; e2a[0xC3]=0x43; e2a[0xC4]=0x44;
   e2a[0xC5]=0x45; e2a[0xC6]=0x46; e2a[0xC7]=0x47; e2a[0xC8]=0x48; e2a[0xC9]=0x49;
   e2a[0xD1]=0x4A; e2a[0xD2]=0x4B; e2a[0xD3]=0x4C; e2a[0xD4]=0x4D; e2a[0xD5]=0x4E;
   e2a[0xD6]=0x4F; e2a[0xD7]=0x50; e2a[0xD8]=0x51; e2a[0xD9]=0x52; e2a[0xE2]=0x53;
   e2a[0xE3]=0x54; e2a[0xE4]=0x55; e2a[0xE5]=0x56; e2a[0xE6]=0x57; e2a[0xE7]=0x58;
   e2a[0xE8]=0x59; e2a[0xE9]=0x5A; e2a[0x40]=0x20; e2a[0x4B]=0x2E; e2a[0x4C]=0x3C;
   e2a[0x4D]=0x28; e2a[0x4E]=0x2B; e2a[0x4F]=0x7C; e2a[0x50]=0x26; e2a[0x5A]=0x21;
   e2a[0x5B]=0x24; e2a[0x5C]=0x2A; e2a[0x5D]=0x29; e2a[0x5E]=0x3B; e2a[0x60]=0x2D;
   e2a[0x61]=0x2F; e2a[0x6B]=0x2C; e2a[0x6C]=0x25; e2a[0x6D]=0x5F; e2a[0x6E]=0x3E;
   e2a[0x6F]=0x3F; e2a[0x7A]=0x3A; e2a[0x7B]=0x23; e2a[0x7C]=0x40; e2a[0x7D]=0x27;
   e2a[0x7E]=0x3D; e2a[0x7F]=0x22; e2a[0x5F]=0x5E;

   maps_initialised = true;

} /* init_character_code_maps */

void format_date(pCHAR temp)
{
   struct timeb   time_structure;   /* Time for ftime*/
   time_t long_time_since_1970;     /* Number of seconds since 01/01/1970 A.D. 12:00 A.M.*/
   struct tm *current_time;         /* Current time structure*/
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);
   return;
}



/*************************************************************************************
       NAME:               convert_to_ascii

       DESCRIPTION:        converts a stream of bytes from EBCDIC to ASCII

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:
           None.

       AUTHOR:             Ram Malathkar
*************************************************************************************/
void convert_to_ascii( pBYTE source, pBYTE dest, INT len )
{
   INT  temp;
   BYTE temp_byte;

   if( !maps_initialised )
      init_character_code_maps();

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = e2a[temp_byte];
   }

   return;
} /* convert_to_ascii */


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



/*FILE *fp1;
char *fpath ="/home/veena/ascendent/log/Recdsent_java.log";

FILE *fp2;
char *fpath1 ="/home/veena/ascendent/log/Recdrec_java.log";*/



/*CHAR mssg_type[8] = {"0500","0320","0800","0400","0220","0830","0530","0200"};*/
/*CHAR pr_cd[10]={"920000","960000","910000","990000","040000","900000","950000","930000","200000","000000");*/

char *doubletostr(double f)
{

     static char s[13];
     int i, j = 0;
     sprintf(s,"%012.2lf",f);
     for ( i = 0; s[i] != '\0'; i++ )
     {
         if ( s[i] != '.')
         {
             s[j++] = s[i];
         }
     }
     s[j] = '\0';
     return s;
}
INT isalphanum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

INT Is_BDO_Acquiring_Transaction(void)
{
	if((strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0)||
		(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) == 0))
	 {
	    return true;
	 }
	 else
	 {
		 return false;
	 }

}

INT Is_BDO_Pos_Acquiring_Transaction(void)
{
	if((strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0))
	{

		return true;
	}
	else
	{
		return false;
	}

}

INT Falcon_is_Issuing_Transaction(void)
{
	if((strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) != 0) &&
			(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) != 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT FALCON_is_Credential_On_File_VISA(pCHAR temp_pos_entry_mode)
{

	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"10",2)== 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2)==0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}


}

INT FALCON_is_Credential_On_File_MC(pCHAR temp_pos_entry_mode)
{

	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"10",2) == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2)==0) &&
					(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}


}

INT FALCON_is_Credential_On_File_AMEX(pCHAR temp_pos_entry_mode)
{
	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if( (Auth_Tx.amex_pos_entry_mode[6]== 'A' ))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2)==0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}
}

INT FALCON_is_Credential_On_File_DINERS(pCHAR temp_pos_entry_mode)
{

	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if((Auth_Tx.TLF01_details.product_codes[16].amount[6])== 'B' )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2) == 0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

INT FALCON_is_Credential_On_File_CUP(pCHAR temp_pos_entry_mode)
{

	if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2) == 0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

INT FALCON_is_Credential_On_File_JCB(pCHAR temp_pos_entry_mode)
{

	if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2) == 0) &&
				(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}

void Falcon_AMEX_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
		if((Falcon_is_Issuing_Transaction()) == true)
		{
			if( (Auth_Tx.amex_pos_entry_mode[5]== 'X' ) && (Auth_Tx.amex_pos_entry_mode[6] == '2'))
			{
				strncpy(temp_pos_entry_mode,"2W",2); /* Contact less mag stripe*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[4]  == 'S') ||
					(Auth_Tx.amex_pos_entry_mode[4]  == 'T'))
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if (Auth_Tx.amex_pos_entry_mode[6]  == '5')
			{
				strncpy(temp_pos_entry_mode,"05",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[6] == 'W' || Auth_Tx.amex_pos_entry_mode[6] == '2') )
			{
				strncpy(temp_pos_entry_mode,"90",2); /* Contact less mag stripe*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[6]== '9')	)
			{
				strncpy(temp_pos_entry_mode,"80",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[5]== 'X' && Auth_Tx.amex_pos_entry_mode[6] == '5'))
			{
				strncpy(temp_pos_entry_mode,"X5",2); /* Contact less micro chip*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[5]== '1' && Auth_Tx.amex_pos_entry_mode[6] == '5'))
			{
				strncpy(temp_pos_entry_mode,"05",2); /*  chip*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '2') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'W') ||
			 (Auth_Tx.amex_pos_entry_mode[6] == '9'))
			{
				strncpy(temp_pos_entry_mode,"80",2); /* Fall back Mag stripe*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '1') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'V'))
			{
				strncpy(temp_pos_entry_mode,"79",2); /* Fall back keyed entry*/
			}
			else if( Auth_Tx.amex_pos_entry_mode[0] ==  '1' ||
			 Auth_Tx.amex_pos_entry_mode[6] ==  '6' ||
			 Auth_Tx.amex_pos_entry_mode[6] ==  'S' ||
			 Auth_Tx.amex_pos_entry_mode[6] ==  'V' )
			{
				strncpy(temp_pos_entry_mode,"01",2); /* keyed in*/
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"6609S06S4140",12)== 0 ||
			strncmp(Auth_Tx.amex_pos_entry_mode,"6609S0SS4140",12)== 0 ||
			strncmp(Auth_Tx.amex_pos_entry_mode,"660101654140",12)== 0 ||
			strncmp(Auth_Tx.amex_pos_entry_mode,"2009106S0040",12)== 0 ||
			(Auth_Tx.amex_pos_entry_mode[4]== 'S'))
			{
				strncpy(temp_pos_entry_mode,"81",2); /* E-commerce5*/
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"660101S54140",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660390664130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660390S64130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660320664130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660320S64130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660101S54130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660101V54130",12)== 0)
			{
				strncpy(temp_pos_entry_mode,"01",2);
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"260101254140",12)== 0 ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"260101W54140",12)== 0	   ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"201101W00001",12)== 0)
			{
				strncpy(temp_pos_entry_mode,"90",2);
			}
			else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0072") == 0 ||
					strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0070") == 0 )
			{
				strncpy(temp_pos_entry_mode,"01",2);
			}
			else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0 ||
					strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0062") == 0)
			{
				strncpy(temp_pos_entry_mode,"90",2);
			}
			else if( (strcmp(temp_pos_entry_mode,"01") == 0 || strcmp(temp_pos_entry_mode,"07") == 0)  &&
					strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0)
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '1' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
					(Auth_Tx.amex_pos_entry_mode[0]== '6' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
					(Auth_Tx.amex_pos_entry_mode[0]== '1' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
					(Auth_Tx.amex_pos_entry_mode[0]== '6' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
					(Auth_Tx.amex_pos_entry_mode[4]== 'S'))
			{
				strncpy(temp_pos_entry_mode,"81",2); /* E-commerce*/
			}
			else if(strncmp(temp_pos_entry_mode,"00",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"81",2); /* E-commerce*/
			}
		}
		else
		{
			if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 )
			{
				strncpy(temp_pos_entry_mode,"07",2);
			}
			else if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 &&
			 strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2)== 0)
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"80",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"80",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"09",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"79",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"01",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"01",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"90",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"05",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"05",2);
			}
			else
			{
				strcpy(temp_pos_entry_mode,"");
			}
		}

}

void Falcon_DINERS_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
			if((Is_BDO_Acquiring_Transaction()) == true)
			{
				if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"81",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"80",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"09",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"01",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"05",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}
			else
			{
				if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '7' ||
					Auth_Tx.TLF01_details.product_codes[16].amount[6] == 'T' ||
					Auth_Tx.TLF01_details.product_codes[16].amount[6] == 't' )
				{
					strncpy(temp_pos_entry_mode,"91",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == 'S')
				{
					strncpy(temp_pos_entry_mode,"92",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[4] == '9')
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '9')
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '1' ||
						 Auth_Tx.TLF01_details.product_codes[16].amount[6] == '6' )
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '2')
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '5')
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}

}

void Falcon_CUP_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
			if((Is_BDO_Acquiring_Transaction()) == true)
			{
				if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "07" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if (	strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "80" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "01" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "05" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "81" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if(strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "02" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2); /* U*/
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "09" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}
			else
			{
				if( strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "98" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"91",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "07" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "01" ,2) == 0 &&
						 strncmp(Auth_Tx.TLF01_details.pos_condition_code , "42" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if((strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "02" ,2) == 0 ||
						 strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "90" ,2) == 0) &&
						(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"052",3)== 0))
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "01" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "02" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"02",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "90" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "05" ,2) == 0 ||
						 strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "95" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}

}

void Falcon_JCB_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{

			if((Is_BDO_Acquiring_Transaction()) == true)
			{
				if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"81",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"80",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"09",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"01",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"05",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}
			else
			{
				if( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"07",2) == 0 ||
					strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"92",2) == 0 ||
					strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"06",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"81",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"97",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"79",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"01",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"02",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"02",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"90",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"05",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}

}

void Falcon_MC_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
		if((Is_BDO_Acquiring_Transaction())== true)
		{
			if((strcmp(temp_pos_entry_mode,"01") == 0) && (strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0))
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
		}
}



void Falcon_VISA_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
				if((strcmp(temp_pos_entry_mode,"01") == 0) && (strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0))
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0)
				{
						strncpy(temp_pos_entry_mode,"90",2); /* U*/
				}
				else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
				{
					if( Auth_Tx.TLF01_details.product_codes[16].amount[2] == '1' ||
					    Auth_Tx.TLF01_details.product_codes[16].amount[2] == '2')
					{
						strncpy(temp_pos_entry_mode,"80",2); /* F */
					}
					else if (Auth_Tx.TLF01_details.product_codes[16].amount[2] == '0')
					{
						strncpy(temp_pos_entry_mode,"90",2); /* U */
					}
				}
				/* Need to check the DE60.3 chip condition code: If 1 0r 2 : then it should be fall back :  F
				else if 60.3 = 0 then value should be U*/


}
BOOLEAN  Falcon_Check_If_CUP_RPS_TRN(void)
{
	if((0 == strncmp(Auth_Tx.TLF01_details.merchant_id,
			   	   	 CUP_MID_INDICATOR_DEFAULT,
					 CUP_MID_INDICATOR_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			   	   	 CUP_RPS_POS_ENTRY_MODE,
					 CUP_RPS_POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
			   	   	 CUP_ISO_RPS_POS_CONDITION_CODE,
					 CUP_DE25_SIZE))&&
	   (0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					  "dcp",3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN Falcon_Check_If_CUP_BDO_PAY(void)
{
	if((0 == strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
					  CUP_BDO_PAY_POS_ENTRY_MODE,
					  CUP_BDO_POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp( Auth_Tx.TLF01_details.pos_condition_code,
			   	      CUP_BDO_PAY_POS_CONDITION_CODE,
					  CUP_DE25_SIZE))&&
	   (0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
				  	  "dcp",3)))
		{
			return true;
		}
		else
		{
			return false;
		}
}

BOOLEAN Falcon_Check_If_CUP_MOTO_TRN(void)
{
	if((0 == strncmp(Auth_Tx.industry_code,CUP_MOTO_INDUSTRY_CODE,INDUSTRY_CODE_CODE))&&
	   ((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))||
	    (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"001",3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT Falcon_check_DINERS_emv_capability_device(void)
{
	/*BDO have discarded this functionality so always return fasle*/
	return false;
	if( 0 == strncmp(Auth_Tx.TLF01_details.terminal_type ,
					 DINERS_EMV_CAPABILITY_INDICATOR,DINERS_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN Falcon_check_is_Diners_Magstrip_pos_transaction(void)
{
	if(  (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
		 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP2,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
		 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP3,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
		 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP4,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)))
	{
		return true;
	}
	return false;
}

BOOLEAN Falcon_check_is_Diners_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN Falcon_check_is_Diners_Fallback_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN Falcon_check_is_Diners_Contactless_Mastripe_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN Falcon_check_is_Diners_Contactless_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN Falcon_check_is_voice_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   voice_handler_que_name,
					   DINERS_VOICE_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}
BOOLEAN Falcon_check_is_Diners_Manual_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_MANUAL,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT Falcon_check_is_Diners_Asiapay_Ecommerce_transaction (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION,
					DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Falcon_check_is_Diners_RPS_Transaction(void)
{
	if( 0 == strncmp(Auth_Tx.industry_code, DINERS_RPS_INDUSTRY_CODE,
					 DINERS_RPS_INDUSTRY_CODE_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Falcon_check_is_Diners_Asiapay_Recurring (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Falcon_check_is_Diners_BDO_PAY (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_BDO_PAY_TRANSACTION,
					 DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}
INT Falcon_Check_If_DCI_pin_block_exists()
{
   INT  ret_val = true;

   if ( 0x00 == Auth_Tx.TLF01_details.pin_block[0] &&
		0x00 == Auth_Tx.TLF01_details.pin_block[1] )
   {
      ret_val = false;
   }
   else if (Falcon_Check_If_blank_string(Auth_Tx.TLF01_details.pin_block,
                         sizeof(Auth_Tx.TLF01_details.pin_block)-1) )
   {
      ret_val = false;
   }

   return( ret_val );
}

INT Falcon_Check_If_blank_string(pCHAR astr, INT max_len)
{
   INT  i = 0;
   INT  ret_val = true;

   if (0 == strlen(astr))
      ret_val = true;

   else
   {
      for(i = 0;i < max_len; i++)
      {
         if (astr[i] != ' ')
         {
            ret_val = false;
            break;
         }
      }
   }
   return( ret_val );
}

INT Falcon_Parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   INT    bf2_max_len=19;
   pBYTE  field_separator   = NULL_PTR;

   if ( p_auth_tx->TLF01_details.track2[0] != 0x00 )
   {
	  /* Locate the field separator. */
	  field_separator = p_auth_tx->TLF01_details.track2;
	  for(track2_idx=0;track2_idx <= bf2_max_len;track2_idx++,field_separator++)
	  {
		 if ( !isdigit( *field_separator ) )
			break;
	  }

	  if ( track2_idx > bf2_max_len )
	  {
		 ret_val = false;
		 sprintf( error_msg,
				 "Unable to parse track2. No field separator. Track2: %s",
				  p_auth_tx->TLF01_details.track2 );
	  }
	  else
	  {
		 /* Put Card Number into auth_tx. */
		 if ( p_auth_tx->TLF01_details.card_num[0] == 0x00 )
		 {
			card_num_len = field_separator - p_auth_tx->TLF01_details.track2;

		   /* If track2 length is odd, ignore leading zero on card number. */
			   memcpy( p_auth_tx->TLF01_details.card_num,
					   p_auth_tx->TLF01_details.track2, card_num_len);

			sprintf( p_auth_tx->TLF01_details.card_num_len,"%02d",card_num_len);
		 }

		 /* Get the expiration date. */
		 if ( p_auth_tx->TLF01_details.exp_date[0] == 0x00 )
		 {
			memcpy( p_auth_tx->TLF01_details.exp_date, field_separator+1, 4 );
		 }

		 /* Get the service code. */
		 memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
		 memcpy( ServiceCode, field_separator+5, 3 );
	  }
   }
   return( ret_val );
}

BYTE Falcon_IsFieldNull (char str[])
{
	if (strlen (str) == 0)
		return 1 ;
	return 0 ;
}

BOOLEAN Falcon_Is_AsiaPay_Transaction()
{
	if((0==strncmp(Auth_Tx.TLF01_details.message_type, "0200",4)) &&
	   (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC,2)) &&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,ECOM_POS_ENTRYMODE,3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Falcon_Prepare_Pos_Data_AMEX(void)
{
	INT	 ret_val = true;
	CHAR  temp_entry_mode[4] = {0};
	memset(Auth_Tx.TLF01_details.product_codes[16].amount,0x30,12);

	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode) > 3)
	{
		strncpy(temp_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],3);
	}
	else
	{
		strcpy(temp_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode);
	}

	if(strncmp(temp_entry_mode,"02",2) == 0 ||
	   strncmp(temp_entry_mode,"06",2) == 0 ||
	   strncmp(temp_entry_mode,"08",2) == 0 ||
	   strncmp(temp_entry_mode,"90",2) == 0 )
	{
		/*Mag stripe*/
		if(temp_entry_mode[2] == '1')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_MAGSTRIP_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
		}
	}

	else if( strncmp(temp_entry_mode,"05",2) == 0 )
	{
		/* EMV - Contact*/
		if(temp_entry_mode[2] == '1')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_CHIP_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_CHIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
		}
	}

	else if( strncmp(temp_entry_mode,"80",2) == 0 )
	{	/* POS 80X */
		if(temp_entry_mode[2] == '1')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_FALLBACK_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_FALLBACK_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
		}
	}

	else if( strncmp(temp_entry_mode,"01",2) == 0 )
	{	/*Manual - VOICE- ECOM  - Recurring - RPS*/
			/*VOICE*/
		if(Auth_Tx.TLF01_details.voice_auth[0] == 'Y')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"600001S00100",AMEX_POS_DATA_LEN);

			/* For position seven, need to know if 4DBC/4CSC was entered.
			* Security control information is coming in
			* auth_Tx.tlf01_details.interest_rate.
			*/
			if ( Auth_Tx.TLF01_details.interest_rate[0] == 0x00 )
			{
				/* It was not entered, so change 'S' to '6'. */
				Auth_Tx.TLF01_details.product_codes[16].amount[6] = '6';
			}
		}

		else if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2) == 0)
		{
			/* For E-commerce tx*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA,AMEX_POS_DATA_LEN);
			/*if(Auth_Tx.TLF01_details.interest_rate[0] != 0x00 )
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"100SS0S00100",AMEX_POS_DATA_LEN);
			}*/
		}
		else if(0 == strcmp(Auth_Tx.industry_code,"800") &&
				(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0)
				)
		{
			/*RPS*/
			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					AMEX_RPS_ASIAPAY_RPS_POS_DATA,AMEX_POS_DATA_LEN);
		}
		else if ( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2))
		{
			/*BDO PAY*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					AMEX_BDO_PAY_POS_DATA,AMEX_POS_DATA_LEN);
		}
		else
		{
			/*Manual entry*/
			if(temp_entry_mode[2] == '1')
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
			}
			else
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
			}
		}
	}

	else if( strncmp(temp_entry_mode,"07",2) == 0)
	{
		 /*  07X with out POS, Special case */
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REFUND)
		{
		   memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"660101654140",AMEX_POS_DATA_LEN);
		}
		else if((strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0)
				&& (0 == strcmp(Auth_Tx.industry_code,"800")) )
		{
		   memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"600090S04100",AMEX_POS_DATA_LEN);
		}
		else
		{
			if(temp_entry_mode[2] == '1')
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
			}
			else
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
			}
		}
	}
	else if(strncmp(temp_entry_mode,"91",2) == 0)
	{
		/*Special case for COntact less */
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
			   AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
	}
	else
	{	 /* POS 00X */
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"010110054140",AMEX_POS_DATA_LEN);
	}
}

void Falcon_Prepare_Pos_Data_VISA(void)
{

	CHAR   	tempstr [256] = {0};
	BYTE 	emv_chip_code;
	INT  	pos_entry_type = POS_ENTRY_NORMAL;
	CHAR   	temp_str[100] = {0};
	BYTE 	eCommerce_Indcator[3] ={0} ;
	INT    	RPS_flag = 0;
	INT    	iIndustry;
	INT  	emv_txn;
	INT  	emv_len;
	CHAR    temp_len[5] = {0};
	CHAR    error_str[100] = {0};

	strncpy(eCommerce_Indcator,Auth_Tx.TLF01_details.profile,2);
	iIndustry = atoi( Auth_Tx.industry_code );
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
	   /* Determine if txn is EMV (Smart Card). */
	   if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
			(Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
		  emv_txn = false;
	   else
	   {
		  emv_txn = true;
		  if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0)
			  emv_len = Auth_Tx.EMV_details.emv_block[0];
			  else
			  emv_len = genutil_bcd_to_int(Auth_Tx.EMV_details.emv_block, 2 );
	   }

		/* Set the POS Entry Mode flag for use in other fields. */
		  if ( emv_txn == true )
			 pos_entry_type = POS_ENTRY_EMV_CHIP;
	   else if( 0 == strncmp("0802",Auth_Tx.TLF01_details.pos_entry_mode,4) || (0 == strncmp("0801",Auth_Tx.TLF01_details.pos_entry_mode,4) ))
			  pos_entry_type = POS_ENTRY_FALLBACK;
		  else if ( 0 == strncmp("080",Auth_Tx.TLF01_details.pos_entry_mode,3) )
			 pos_entry_type = POS_ENTRY_EMV_MAGSTRIPE;

		  else if ( 0 == strncmp("008",Auth_Tx.TLF01_details.pos_entry_mode,3) )
			 pos_entry_type = POS_ENTRY_EMV_FALLBACK;

		  /* FIELD 60: ADDITIONAL POS INFORMATION */
		 memset( tempstr, 0x00, sizeof(tempstr) );
		 memcpy(tempstr,Auth_Tx.TLF01_details.nfi_name,sizeof(Auth_Tx.TLF01_details.nfi_name));
		 if(!(Falcon_IsFieldNull(Auth_Tx.TLF01_details.nfi_name)))
		 {
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,Auth_Tx.TLF01_details.nfi_name,
				   (sizeof(Auth_Tx.TLF01_details.nfi_name)-1));
		 }
		 else
		 {
		  if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
		  {
			 /* Determine chip condition code (if relevant) for DE60.3 */
			 (void)Falcon_Parse_track2(&Auth_Tx,error_str); /* Gets service code */
			 if ( (ServiceCode[0] == '2') || (ServiceCode[0] == '6') )
			 {
				if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
				{
				   emv_chip_code = Auth_Tx.EMV_details.chip_condition_code[0];
				   Auth_Tx.EMV_details.chip_condition_code[0] = '1';
				}
				else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
				{
				   emv_chip_code = '2';
				   Auth_Tx.EMV_details.chip_condition_code[0] = '2';
				}
				else if ( pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE )
				{
				   emv_chip_code = Auth_Tx.EMV_details.chip_condition_code[0];
				}
				else
				{
				   emv_chip_code = '0';
				}

				/* Make sure chip code is not 0 for Service Code 2 or 6. */
				if ( (emv_chip_code != '1') && (emv_chip_code != '2') )
				   emv_chip_code = '1';

				if ( Auth_Tx.EMV_details.chip_condition_code[0] == 0x00 )
				   Auth_Tx.EMV_details.chip_condition_code[0] = '1';
			 }
			 else
			 {
				emv_chip_code = '0';  /* Always for Service Code not 2 or 6 */
				if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
				{
				   Auth_Tx.EMV_details.chip_condition_code[0] = '1';
				}
				else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
				{
				   Auth_Tx.EMV_details.chip_condition_code[0] = '2';
				}
				else if ( pos_entry_type != POS_ENTRY_EMV_MAGSTRIPE )
				{
				   Auth_Tx.EMV_details.chip_condition_code[0] = '0';
				}
			 }
		  }

		   if ( iIndustry == ECOMMERCE || (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC,2)))  /* Verified by visa VbV */
		   {
			 memset( tempstr, 0x00, sizeof(tempstr) );
			strncpy( tempstr, "500000000040",12 );

			 if ( eCommerce_Indcator[0] == 0x00 )
			 {
				if ( Auth_Tx.TLF01_details.message_type[1] == '4' )
				{
				   /* This is a reversal. Need to echo ECI.
					* It gets stored in settlement date (TLF01);
					* It is stored in BCH10 in product code 20.
					*/
				   strcpy(eCommerce_Indcator,Auth_Tx.TLF01_details.settlement_date);
				}
				else
				{
				   /* Not a reversal. Use default value. */
				   strcpy( eCommerce_Indcator, "07" );
				}
			 }

			 strncpy( tempstr+8, eCommerce_Indcator,2);

			 /* Save ECI for reversals. */
			 strcpy( Auth_Tx.TLF01_details.settlement_date, eCommerce_Indcator );

			 if ( Auth_Tx.TLF01_details.terminal_type[1] == '7' )
				tempstr[1] = '5'; /* Terminal has Chip Read capability. */

			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,tempstr,
					strlen(tempstr));

		   }
		  else if ( emv_txn == true || ( 0 == strncmp("005",Auth_Tx.TLF01_details.pos_entry_mode,3) )
				  || ( 0 == strncmp("007",Auth_Tx.TLF01_details.pos_entry_mode,3) ))
		  {
			 memset( tempstr, 0x00, sizeof(tempstr) );
			  if( 0 == strncmp("0912",Auth_Tx.TLF01_details.pos_entry_mode,4))
			  {
				  strncpy( tempstr, "912000100010" ,12);
			  }
			  else if(pos_entry_type == POS_ENTRY_EMV_CHIP ||
					  ( 0 == strncmp("007",Auth_Tx.TLF01_details.pos_entry_mode,3)))
			  {

				 strncpy( tempstr, "050000100010" ,12);
			  }

			 /* 60.3 = Chip Condition Code */
			 if ( emv_chip_code == 0x00 )
			 {
				/* EB wants this field to default to zero. */
				emv_chip_code = '0';
				Auth_Tx.EMV_details.chip_condition_code[0] = '0';
			 }

			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,tempstr,
				sizeof(Auth_Tx.TLF01_details.product_codes[16].amount)-1);

		  }
		  else if ('Y' != Auth_Tx.TLF01_details.voice_auth[0])
		  {
			 if (0 == strcmp(Auth_Tx.TLF01_details.terminal_type,"01"))
			 {
				/* Unattended gas pump */
				 Auth_Tx.TLF01_details.product_codes[16].amount[0] = '3';
			 }
			 else
			 {
				/* Unspecified */
				 Auth_Tx.TLF01_details.product_codes[16].amount[0] = '0';
			 }

			 /* Ignore the following for Ecommerce. */
			 if ( iIndustry != ECOMMERCE )
			 {
				if ( 0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0012",4) &&
					 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)) /*RPS*/
				{
					RPS_flag = 1;
				   strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,"010000000000",12);
				}
				else if (Auth_Tx.TLF01_details.pos_entry_mode[2] == '2')
				{
					if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))
					{
					 /* EMV capability */
						strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,"050000100010",12);  /*code change same as ncvisa*/
					}

				}
				else if ((Auth_Tx.TLF01_details.pos_entry_mode[2] == '1') &&  (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2)))
				{
					if (Auth_Tx.TLF01_details.pos_entry_mode[1] == '9')
				   /* Magnetic stripe read capability */
						strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "912000100010", 12 );
					else
						/* Manual key in */
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "050000100010", 12 );
				}

				else if ( pos_entry_type != POS_ENTRY_NORMAL )
				{
					Auth_Tx.TLF01_details.product_codes[16].amount[1] = '5';
					Auth_Tx.TLF01_details.product_codes[16].amount[2] = emv_chip_code;
				}

				else if((Auth_Tx.TLF01_details.pos_entry_mode[2] == '1') &&
				(0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2)))
				{
							   /* BDO PAY QR*/
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "010000000000", 12 );
				}
				else
				{
				   /* No magnetic stripe read capability */
					Auth_Tx.TLF01_details.product_codes[16].amount[1] = '1';
				}

				if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK ||
					 pos_entry_type == POS_ENTRY_FALLBACK ||
					 pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE)
				 {
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "052000000010", 12 );
					}

				 }
			 }			/* Oct Mandate changes 2021 */
		  	 else if ('Y' == Auth_Tx.TLF01_details.voice_auth[0])
		 	 {
		 		strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "010000000000", 12 );
		 	 }


			 if ( Auth_Tx.TLF01_details.terminal_type[1] == '7' && RPS_flag == 0)
			 {
				/* Terminal has Chip Read capability. */
				 Auth_Tx.TLF01_details.product_codes[16].amount[1] = '5';
			 }

			 /* Store here for Fraudguard. */
			 memcpy( Auth_Tx.TLF01_details.nfi_name,
					 Auth_Tx.TLF01_details.product_codes[16].amount,
			 					 sizeof(Auth_Tx.TLF01_details.nfi_name)-1 );
		  }
		}

}

void Falcon_Prepare_Pos_Data_MC(void)
{
   BOOLEAN e_commerce;
   INT iIndustry;
   CHAR    temp_str3[4] = {0};
   INT     itemp;

   memset( temp_str3, 0x00, sizeof(temp_str3) );

   if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
   {
	   /* Its Acq transaction */
	   iIndustry = atoi( Auth_Tx.industry_code );
	   memcpy( temp_str3, Auth_Tx.TLF01_details.pos_entry_mode +1, 3 );

	   /* Some fields need to know if this txn is an e-commerce txn. */
	   if( (iIndustry == 700) ||
		   (0==strcmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC)) )
	   {
		   e_commerce = true;
	   }
	   else
		   e_commerce = false;

	   if ( e_commerce )
	   {
			  strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "102510000660",12);
	   }

	   else if ( ('Y' == Auth_Tx.TLF01_details.voice_auth[0]) ||
				 ('y' == Auth_Tx.TLF01_details.voice_auth[0])   )
		  strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "20311000006" ,11);

	   else
	   {
		  /* RPS 203110000010 */
		   if (Auth_Tx.MCF01_details.rps_merchant[0]=='Y' || iIndustry == 800 ||
				   (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "203410000000",12 );
		   }

		   else if(0 == strncmp(temp_str3, "01", 2) && 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2) )
		   { /* Manual Keyed in */
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000111000030",12 );
		   }
		   else if(0 == strncmp(temp_str3, "91", 2) ||
				   0 == strncmp(temp_str3, "07", 2))
		   {/* Contact less magstripe and chip */
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000001000030",12 );
		   }
		   else if(0 == strncmp(temp_str3, "02", 2) ||
				   0 == strncmp(temp_str3, "05", 2) ||
				   0 == strncmp(temp_str3, "80", 2) )
		   { /* Magstripe swipe, Chip, Chip Fallbak */
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000001000030",12 );
		   }
		   else if(0 == strncmp(temp_str3, "01", 2) &&
			   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2)))
		   {
			   /* BDO PAY QR */
			   strcpy( Auth_Tx.TLF01_details.product_codes[16].amount, "203110000100" );
		   }
		   else if(iIndustry == 500 )
		   {	/* MOTO */
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "20321000000",11 );
		   }
		   else
		   {/* chip, contact, mag stripe */
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000001000030",12 );
		   }
	   }
	   /*
			TF - Abhishek For Card Ver/ Pre-Auth and Processing code 38, 30, DE61 Subfield 7
			shoud be 4. This is done for April 2016 mandated changes.
			In dcpiso we are storing original key for card verification in auth_tx.mac_key[0] filed.
		*/
		if( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"30",2))||
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"38",2))||
			( Auth_Tx.mac_key[0] == AUTH_CARD_VERIFICATION))
			{
				Auth_Tx.TLF01_details.product_codes[16].amount[6]  = '4';
			}


	   /* ----------------------------------------------------- */
	   /* POS Card Data Terminal Input Capability = Field 61.11 */
	   /* ----------------------------------------------------- */

	   /********************************************************************
		*
		*  Set field 61.11 based on field 22 as follows. The
		*  value of this field is NOT dependent on terminal type.
		*
		*  +----------------+-------------+-----------------------------+
		*  | POS Entry Mode | Field 61.11 | Description (61.11) 		|
		*  +----------------+-------------+-----------------------------+
		*  |	05X 		|	  8 	  | Mag stripe, key entry, chip |
		*  |	01X 		|	  8 	  | 							|
		*  |	80X 		|	  8 	  | 							|
		*  |	90X 		|	  8 	  | 							|
		*  |	072 		|	  3 	  | Contactless M/Chip			|
		*  |	912 		|	  3 	  | 							|
		*  +----------------+-------------+-----------------------------+
		*
		********************************************************************
		*/

	   if ( (0 == strncmp(temp_str3, "90", 2)) ||
			(0 == strncmp(temp_str3, "05", 2)) ||
			(0 == strncmp(temp_str3, "01", 2)) ||
			(0 == strncmp(temp_str3, "80", 2))
		  )
	   {
		  /* Mag stripe, key entry, and EMV-Compatible chip reader */
			if( ((Auth_Tx.TLF01_details.product_codes[16].amount[3] == '0') && (Auth_Tx.TLF01_details.product_codes[16].amount[4] == '1')))
			{
				/* To avoid 8.h edit error */
				Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '7';
			}

	   }
	   /* Check for Ecomerce and RPS , 61.11 should be "0" .. TF Phani */
		if(0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
		{
			Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '1';
		}


	   /* Edit error 8.e check */
	   if ( (0 == strncmp(temp_str3, "00", 2)))
	   {
			if((Auth_Tx.TLF01_details.product_codes[16].amount[10]=='2')||
			   (Auth_Tx.TLF01_details.product_codes[16].amount[10]=='5')||
			   (Auth_Tx.TLF01_details.product_codes[16].amount[10]=='7')||
			   (Auth_Tx.TLF01_details.product_codes[16].amount[10]=='8'))
			{
				Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '0';
			}
	   }

	   /* Edit error 8.d check */
	   if ( (0 == strncmp(temp_str3, "00", 2))||
			(0 == strncmp(temp_str3, "01", 2)))
	   {
			if(Auth_Tx.TLF01_details.product_codes[16].amount[10]=='1')
			{

				Auth_Tx.TLF01_details.product_codes[16].amount[0]='2';
				Auth_Tx.TLF01_details.product_codes[16].amount[2]='3';
			}
	   }

	   /* For automated fuel dispenser transactions, subfield 10 must be '2' */
	   if ( 0 == strncmp( AUTO_FUEL_MCC, Auth_Tx.TLF01_details.category_code,4 ))
	   {
		  /* Industry code 600 should take care of this,
		   * but just in case, do it here, else MasterCard charges a fee.
		   */
		  memset( &Auth_Tx.TLF01_details.product_codes[16].amount[9], 0x32, 1 );
	   }
	   Auth_Tx.TLF01_details.product_codes[16].amount[12]=0x00;
   }

}

void Falcon_Prepare_Pos_Data_CUP(void)
{
	/*
		Refer build_request_field_60 in nccup for more info
	*/
	INT	ret_val = true;
	INT	len = 0;
	INT	len_60_3 = 0;
	CHAR temp_len[4] = {0};
	BYTE temp_pos[4] = {0};
	INT temp_posentry = 0;
	CHAR HOSTMESSAGE_posentry[4] = {0};
	CHAR HOSTMESSAGE_card_seq_num[4] = {0};
	CHAR    temp_str[4] = {0};
	CHAR    temp_str1[4] = {0};
	INT     itemp;
	CHAR  team_field_23[4] = {0};
	INT    temp_len_field_23 = 0;

    if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{

	}
	else
	{
		/*Prepare feild 60.2.1*/
		if((true == Falcon_Check_If_CUP_RPS_TRN()) ||
		   (true == Falcon_Check_If_CUP_MOTO_TRN()))
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"9",1);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount ,"0",1);
		}
		/*Prepare PAN Scequence Number*/
		if (strlen(Auth_Tx.host2_message_seq_nbr) > 0)
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy(temp_str,Auth_Tx.host2_message_seq_nbr+1,CUP_DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOSTMESSAGE_card_seq_num,temp_str1,CUP_DE23_SIZE );
		}
		else if (strlen(Auth_Tx.EMV_details.pan_sequence_number)> 0)
		{
			  memcpy(temp_str,Auth_Tx.EMV_details.pan_sequence_number,CUP_DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOSTMESSAGE_card_seq_num, temp_str1,CUP_DE23_SIZE );
		}

		/*Prepare POS ENTRY MODE*/
		strncpy(HOSTMESSAGE_posentry,Auth_Tx.TLF01_details.pos_entry_mode +1 ,CUP_DE22_SIZE);
		strncpy(temp_pos,HOSTMESSAGE_posentry,2);
		temp_posentry=atoi(temp_pos);

		switch(temp_posentry)
		{
			case 02:
			case 90:
			case 06:
				/*Prepare feild 60.2.2*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount+ 1,"5",1);
				/*Prepare feild 60.2.3*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			case 05:
			case 95:
			case 97:
			case 80:
				if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
				{
					/*Prepare feild 60.2.2*/
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
					/*Prepare feild 60.2.3*/
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
				}
				else
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
					/*Prepare feild 60.2.3*/
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
				}
			break;

			case 07:
			case 91:
			case 96:
			case 98:
				/*Prepare feild 60.2.2*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"6",1);
				/*Prepare feild 60.2.3*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			case 01:
				/*Prepare feild 60.2.2*/
				if((0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)) ||
				   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2)))
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				}
				else if(true == Falcon_Check_If_CUP_MOTO_TRN())
				{
					if( 0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
									   voice_handler_que_name,
									   CUP_VOICE_MODULE_COMP_LEN) )
					{
						memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1 ,"0",1);
					}
					else
					{
						memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
					}
				}
				else if (true == Falcon_Check_If_CUP_RPS_TRN()||
						(true == Falcon_Check_If_CUP_BDO_PAY()))
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				}
				else
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
				}
				/*Prepare feild 60.2.3*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			default:
				/*Prepare feild 60.2.2*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				/*Prepare feild 60.2.3*/
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;
		}

		/*Prepare feild 60.2.4 Reserved filled in with 0*/
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 3,"0",1);

		/*Prepare feild 60.2.2 for Void and Reversal for Magstip Trasaction*/
		if( temp_posentry == 05 ||
			temp_posentry == 12 ||
			temp_posentry == 01 ||
			temp_posentry == 80 ||
			temp_posentry == 95 ||
			temp_posentry == 97 ||
			temp_posentry == 07 ||
			temp_posentry == 91 ||
			temp_posentry == 96 ||
			temp_posentry == 98)
			{
				/* Do nothing Its EMV Trasaction*/
			}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
				Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE
			)
		{
			/* Its Magstrip Transaction*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"2",1);
		}

		/* Prepare field 60.2.3 for Reversal and Fallback reversal*/
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE )
		{
			if(Auth_Tx.TLF01_details.pos_entry_mode[1]== '8')
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"2",1);
			}
			else
			{
				/* Normal  Reversal */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			}
		}
		/* Prepare filed 60.2.3 for POS EMV fallback transaction*/
		else if( Auth_Tx.TLF01_details.pos_entry_mode[1]== '8')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"2",1);
		}

		/*Prepare feild 60.2.5 Transaction Channel Type*/
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"08", 2);/*Mobile phone Voice transaction */
			}
			else if(true == Falcon_Check_If_CUP_MOTO_TRN())
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"00", 2);/*MOTO Voice transaction */
			}
			else
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"07", 2);/*PC Voice transaction */
			}
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"3", 1);
		}
		else if (true == Falcon_Check_If_CUP_RPS_TRN())
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"14", 2);/* RPS transaction */
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}
		else if (true == Falcon_Check_If_CUP_BDO_PAY())
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"08", 2);/* RPS transaction */
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"03", 2);/* POS-Point Of Sale 03*/
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}

		/*Prepare feild 60.2.6 Signature-based Credit Card Network Identifier*/
		if(true == Falcon_Check_If_CUP_MOTO_TRN())
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 6,"1", 1);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 6,"0", 1);
		}
		/*Prepare feild 60.2.7 Chip Card Authentication Reliability Indicato*/
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 7,"0", 1);
		/*Prepare feild 60.2.8	Electronic Commerce Identification*/
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 8,"00", 2);
	}

}

void Falcon_Prepare_Pos_Data_DINERS(void)
{
	if(true == Falcon_check_DINERS_emv_capability_device ())
	{
		if(true == Falcon_check_is_Diners_Magstrip_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Chip_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Fallback_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Contactless_Chip_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_voice_transaction())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_VOICE,
					DINERS_DE22_SIZE);
		}
		else if(true == Falcon_check_is_Diners_Manual_pos_transaction())
		{
			if(true == Falcon_check_is_Diners_Asiapay_Ecommerce_transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == Falcon_check_is_Diners_RPS_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Falcon_check_is_Diners_Asiapay_Recurring())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_ASIAPAY_RPS	,
						DINERS_DE22_SIZE);
			}
			else if (true == Falcon_check_is_Diners_BDO_PAY())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY	,
						DINERS_DE22_SIZE);
			}
			else if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
	else
	{
		if(true == Falcon_check_is_Diners_Magstrip_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Chip_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN ,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Fallback_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_Diners_Contactless_Chip_pos_transaction())
		{
			if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Falcon_check_is_voice_transaction())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_NOT_PRESENT_VOICE,
					DINERS_DE22_SIZE);
		}
		else if(true == Falcon_check_is_Diners_Manual_pos_transaction())
		{
			if(true == Falcon_check_is_Diners_Asiapay_Ecommerce_transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == Falcon_check_is_Diners_RPS_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Falcon_check_is_Diners_Asiapay_Recurring())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Falcon_check_is_Diners_BDO_PAY())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY	,
						DINERS_DE22_SIZE);
			}
			else if(true == Falcon_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
}

void Falcon_Prepare_Pos_Data_JCB(void)
{
	CHAR   temp_str[50] = {0};
	INT    iIndustry;
	INT    mcc;
	CHAR   tempstr [256] = {0};
	CHAR   num_cc[4] = {0};


	/* all the data is in auth_tx.int_net_facility_data */
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
		iIndustry = atoi( Auth_Tx.industry_code );

		if((Falcon_Is_AsiaPay_Transaction() == true))
		{
			iIndustry=ECOMMERCE;
		}

		/* FIELD 61: POINT OF SERVICE INFORMATION */
	   /* Default Values */
	   memset( temp_str, 0x00, sizeof(temp_str) );
	   strcpy( temp_str, "222" );

	   /* Subfield 61.1 - Mail/Telephone Order Transaction Sign */
	   if ( iIndustry == MAIL_PHONE )
	   {
		  temp_str[0] = '2';
	   }
	   /* Subfield 61.2 - Recurring Transaction Sign */
	   else if ((strncmp(Auth_Tx.TLF01_details.message_type,"0100", 4) == 0 ||
			   	   strncmp(Auth_Tx.TLF01_details.message_type,"0200", 4) == 0) &&
			   (strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0) &&
			   Auth_Tx.MCF01_details.rps_merchant[0] == 'Y')
	   {
		  temp_str[0] = '2' ;
		  temp_str[1] = '1';

		  /* Check for MOTO Merchant */
		  mcc = atoi( Auth_Tx.TLF01_details.category_code );
		  if ( (MCC_RANGE2_LOW <= mcc)	&&	(mcc <= MCC_RANGE2_HIGH) )
			 temp_str[0] = '1';
	   }
	   else if((strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2) == 0) ||
			   (strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2) ==0)) /*bdoo PAY QR and ECOm*/
	   { /* If it JCB RPS transaction send 10 in firt 2 bytes of POS data, HA issue fix, April 2019 Raised by Anna.*/
		   temp_str[0] = '1' ;

	   }

	   /* Subfield 61.3 - Pre Authorization Sign */
	   if ( (Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION) ||
			(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)||
			(Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION) ||
			(Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION_RESPONSE)||
			(Auth_Tx.mac_key[0]			 == AUTH_CARD_VERIFICATION)  )
	   {
		  /* Note: Dcpiso changes Card Verifies to Auths. The original tx_key is stored in mac_key.   */
		  temp_str[2] = '1';
	   }

	   /* Subfield 61.4 - Numeric Country Code where terminal is located. */
	   memset( tempstr, 0x00, sizeof(tempstr) );
	   memcpy( tempstr, Auth_Tx.MCF01_details.intl_code, 2 );

		  strcpy( num_cc, "608" );
	   memcpy( temp_str+3, num_cc, 3 );

	  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount, temp_str, 6 );
	}

}


void Falcon_Prepare_Pos_Data_F121(void)
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		Falcon_Prepare_Pos_Data_AMEX();
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA) == 0)
	{
		Falcon_Prepare_Pos_Data_VISA();
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		Falcon_Prepare_Pos_Data_MC();
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		Falcon_Prepare_Pos_Data_CUP();
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
		Falcon_Prepare_Pos_Data_DINERS();
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB) == 0)
	{
		Falcon_Prepare_Pos_Data_JCB();
	}
}

INT FALCON_is_BDO_PAY_QR_Transaction(void)
{

	if(0 == strlen(Auth_Tx.TLF01_details.nii))
	{
		return false;
	}
	else if( (0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_1,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_2,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_3,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_4,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_5,3) ) )
	{
		return true;
	}
	else
	{
		return false;

	}
}

INT Falcon_Is_Advice_or_Reversal_Transaction(void)
{
	if(	strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"0130",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1120",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1130",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"0420",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"0430",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1420",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1430",4) == 0 )
	{
		return true;
	}
	else if((Check_If_VISA_Transaction() == true) && (Falcon_is_Issuing_Transaction() ==true) &&
			(strncmp(Auth_Tx.TLF01_details.message_type,"0400",4) == 0 ))
	{
		return true;
	}
	else
	{
		return false;
	}
}


INT FALCON_is_Credential_On_File(void)
{

	char temp_pos_entry_mode[3]={0};

	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)== 3)
	{
		strncpy(temp_pos_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,2);
	}
	else
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}

		if((Check_If_AMEX_Transaction())== true)
		{
			FALCON_is_Credential_On_File_AMEX(temp_pos_entry_mode);
		}
		else if((Check_If_DINERS_Transaction())== true)
		{
			FALCON_is_Credential_On_File_DINERS(temp_pos_entry_mode);
		}
		else if((Check_If_CUP_Transaction())== true)
		{
			FALCON_is_Credential_On_File_CUP(temp_pos_entry_mode);
		}
		else if((Check_If_JCB_Transaction())== true)
		{
			FALCON_is_Credential_On_File_JCB(temp_pos_entry_mode);
		}
		else if((Check_If_MC_Transaction())== true)
		{
			FALCON_is_Credential_On_File_MC(temp_pos_entry_mode);
		}
		else if((Check_If_VISA_Transaction())== true)
		{
			FALCON_is_Credential_On_File_VISA(temp_pos_entry_mode);
		}
}

INT isnum(pCHAR in_str)
{
   INT i;
   INT length =0;

   length = (INT)strlen(in_str);
	if(length == 0)
	{
		return(false);
	}
	for (i = 0; i < length; i++)
	{
	  if (!isdigit(in_str[i]))
	    return(false);
	}
   return(true);
}



void RqMssg_Online(pPTE_MSG p_msg_in)
{

	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[9]={0};
	UINT  item_month=0;
	UINT  item_year=0;
	UINT  icvc_data=0;
	char dollar_peso[8]="0.023889";
	char temp_totalamt[14]={0};
	CHAR msgBuffer [400] = {0} ;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	CHAR key[100]={0};



	int i =0;
	int len=0;
	char msg[5]={0};
	char temp_iso_ration[25] = {0};
	char temp_iso_ration_value[13] = {0};
	double temp_iso = 0.0;

	CRF01  crf01 = {0} ;  LONG  lReturnCode;

	char temp_Tran_id[33]={0};


	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
	memset(&Cr_Fld,0,sizeof(Cr_Fld));
	memset(&message,0,sizeof(message));



	memset( &crf01, 0x00, sizeof(CRF01) );

	strcpy( crf01.primary_key.currency_code, "608");
	lReturnCode = FindMemRecord((pBYTE)&crf01, sizeof(crf01.primary_key.currency_code),sizeof(CRF01), "Crf01Table");
	if (lReturnCode >= 0)
	{
			conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
			sprintf(temp_iso_ration,"%f",1/conv_rate);
	}

	pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;

	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );
	strcpy(key,time_date);
	strcat(key,".");
	strcat(key,Auth_Tx.TLF01_details.sys_trace_audit_num);
	strcat(key,Auth_Tx.TLF01_details.card_num);
	strcat(key,Auth_Tx.TLF01_details.total_amount);
	strcat(key,Auth_Tx.TLF01_details.terminal_id);
	strcat(key,Auth_Tx.TLF01_details.merchant_id);
	strcat(key,Auth_Tx.TLF01_details.auth_number);
	strcat(key,Auth_Tx.TLF01_details.nii);
/*	sprintf(msgBuffer,"CRTRAN message built with a unique Key:%s\n",key);
	falcon_log_message(2,1,msgBuffer,"RqMssg_Online",0);*/

	FALCON_read_CUP_merchant_indicator();
	FALCON_read_CUP_MP_Bin_range_len_1();
	FALCON_read_CUP_MP_Bin_range_len_2();
	FALCON_read_CUP_MP_Bin_range_len_3();
	FALCON_read_CUP_MP_Bin_range_len_4();
	FALCON_read_CUP_MP_Bin_range_len_5();
	FALCON_read_CUP_MP_Bin_range_1_indicator();
	FALCON_read_CUP_MP_Bin_range_2_indicator();
	FALCON_read_CUP_MP_Bin_range_3_indicator();
	FALCON_read_CUP_MP_Bin_range_4_indicator();
	FALCON_read_CUP_MP_Bin_range_5_indicator();

	/*sprintf(msgBuffer,"Bin Values read for CUP MP from tf.ini: \nBin_range_1_indicator:%s , Bin_range_1_len:%d\n Bin_range_2_indicator:%s , Bin_range_2_len:%d\n Bin_range_3_indicator:%s , Bin_range_3_len:%d\n Bin_range_4_indicator:%s , Bin_range_4_len:%d\n Bin_range_5_indicator:%s , Bin_range_5_len:%d\n",
			CUP_MP_Bin_range_1,CUP_MP_Bin_range_len_1,CUP_MP_Bin_range_2,CUP_MP_Bin_range_len_2,CUP_MP_Bin_range_3,CUP_MP_Bin_range_len_3,
			CUP_MP_Bin_range_4,CUP_MP_Bin_range_len_4,CUP_MP_Bin_range_5,CUP_MP_Bin_range_len_5);
	falcon_log_message(2,1,msgBuffer,"RqMssg_Online",0);*/


	FALCON_Read_BDO_PAY_QR_NII_value();

	FALCON_build_request_Online_HeaderFile();
	FALCON_build_request_field_1( );
	FALCON_build_request_field_2( );
	FALCON_build_request_field_3( );
	FALCON_build_request_field_4( );
	FALCON_build_request_field_5( );
	FALCON_build_request_field_6( );
	FALCON_build_request_field_7( );
	FALCON_build_request_field_8( );
	FALCON_build_request_field_9( );
	FALCON_build_request_field_10( );
	FALCON_build_request_field_11( );
	FALCON_build_request_field_12( );
	FALCON_build_request_field_13( );
	FALCON_build_request_field_14( );
	FALCON_build_request_field_15( );
	FALCON_build_request_field_16( );
	FALCON_build_request_field_17( );
	FALCON_build_request_field_18( );
	FALCON_build_request_field_19( );
	FALCON_build_request_field_20( );
	FALCON_build_request_field_21( );
	FALCON_build_request_field_22( );
	FALCON_build_request_field_23( );
	FALCON_build_request_field_24( );
	FALCON_build_request_field_25( );
	FALCON_build_request_field_26( );
	FALCON_build_request_field_27( );
	FALCON_build_request_field_28( );
	FALCON_build_request_field_29( );
	FALCON_build_request_field_30( );
	FALCON_build_request_field_31( );
	FALCON_build_request_field_32( );
	FALCON_build_request_field_33( );
	FALCON_build_request_field_34( );
	FALCON_build_request_field_35( );
	FALCON_build_request_field_36( );
	FALCON_build_request_field_37( );
	FALCON_build_request_field_38( );
	FALCON_build_request_field_39( );
	FALCON_build_request_field_40( );
	FALCON_build_request_field_41( );
	FALCON_build_request_field_42( );
	FALCON_build_request_field_43( );
	FALCON_build_request_field_44( );
	FALCON_build_request_field_45( );
	FALCON_build_request_field_46( );
	FALCON_build_request_field_47( );
	FALCON_build_request_field_48( );
	FALCON_build_request_field_49( );
	FALCON_build_request_field_50( );
	FALCON_build_request_field_51( );
	FALCON_build_request_field_52( );
	FALCON_build_request_field_53( );
	FALCON_build_request_field_54( );
	FALCON_build_request_field_55( );
	FALCON_build_request_field_56( );
	FALCON_build_request_field_57( );
	FALCON_build_request_field_58( );
	FALCON_build_request_field_59( );
	FALCON_build_request_field_60( );
	FALCON_build_request_field_61( );
	FALCON_build_request_field_62( );
	FALCON_build_request_field_63( );
	FALCON_build_request_field_64( );
	FALCON_build_request_field_65( );
	FALCON_build_request_field_66( );
	FALCON_build_request_field_67( );
	FALCON_build_request_field_68( );
	FALCON_build_request_field_69( );
	FALCON_build_request_field_70( );
	FALCON_build_request_field_71( );
	FALCON_build_request_field_72( );
	FALCON_build_request_field_73( );
	FALCON_build_request_field_74( );
	FALCON_build_request_field_75( );
	FALCON_build_request_field_76( );
	FALCON_build_request_field_77( );
	FALCON_build_request_field_78( );
	FALCON_build_request_field_79( );
	FALCON_build_request_field_80( );
	FALCON_build_request_field_81( );
	FALCON_build_request_field_82( );
	FALCON_build_request_field_83( );
	FALCON_build_request_field_84( );
	FALCON_build_request_field_85( );
	FALCON_build_request_field_86( );
	FALCON_build_request_field_87( );
	FALCON_build_request_field_88( );
	FALCON_build_request_field_89( );
	FALCON_build_request_field_90( );
	FALCON_build_request_field_91( );
	FALCON_build_request_field_92( );
	FALCON_build_request_field_93( );
	FALCON_build_request_field_94( );
	FALCON_build_request_field_95( );
	FALCON_build_request_field_96( );
	FALCON_build_request_field_97( );
	FALCON_build_request_field_98( );
	FALCON_build_request_field_99( );
	FALCON_build_request_field_100( );
	FALCON_build_request_field_101( );
	FALCON_build_request_field_102( );
	FALCON_build_request_field_103( );
	FALCON_build_request_field_104( );
	FALCON_build_request_field_105( );
	FALCON_build_request_field_106( );
	FALCON_build_request_field_107( );
	FALCON_build_request_field_108( );
	FALCON_build_request_field_109( );
	FALCON_build_request_field_110( );
	FALCON_build_request_field_111( );
	FALCON_build_request_field_112( );
	FALCON_build_request_field_113( );
	FALCON_build_request_field_114( );
	FALCON_build_request_field_115( );
	FALCON_build_request_field_116( );
	FALCON_build_request_field_117( );
	FALCON_build_request_field_118( );
	FALCON_build_request_field_119( );
	FALCON_build_request_field_120( );
	FALCON_build_request_field_121( );
	FALCON_build_request_field_122( );
	FALCON_build_request_field_123( );
	FALCON_build_request_field_124( );
	FALCON_build_request_field_125( );
	FALCON_build_request_field_126( );
	FALCON_build_request_field_127( );
	FALCON_build_request_field_128( );
	FALCON_build_request_field_129( );
	FALCON_build_request_field_130( );
	FALCON_build_request_field_131( );
	FALCON_build_request_field_132( );
	FALCON_build_request_field_133( );
	FALCON_build_request_field_134( );
	FALCON_build_request_field_135( );
	FALCON_build_request_field_136( );
	FALCON_build_request_field_137( );
	FALCON_build_request_field_138( );
	FALCON_build_request_field_139( );
	FALCON_build_request_field_140( );
	FALCON_build_request_field_141( );
	FALCON_build_request_field_142( );
	FALCON_build_request_field_143( );
	FALCON_build_request_field_144( );
	FALCON_build_request_field_145( );
}

void RqMssg_RealTime(pPTE_MSG p_msg_in)
{

	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[9]={0};
	UINT  item_month=0;
	UINT  item_year=0;
	UINT  icvc_data=0;
	char dollar_peso[8]="0.023889";
	char temp_totalamt[14]={0};
	CHAR msgBuffer [400] = {0} ;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	CHAR key[100]={0};



	int i =0;
	int len=0;
	char msg[5]={0};
	char temp_iso_ration[25] = {0};
	char temp_iso_ration_value[13] = {0};
	double temp_iso = 0.0;

	CRF01  crf01 = {0} ;  LONG  lReturnCode;

	char temp_Tran_id[33]={0};


	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
	memset(&Cr_Fld_RT,0,sizeof(Cr_Fld_RT));
	memset(&message,0,sizeof(message));

	memset( &crf01, 0x00, sizeof(CRF01) );

	strcpy( crf01.primary_key.currency_code, "608");
	lReturnCode = FindMemRecord((pBYTE)&crf01, sizeof(crf01.primary_key.currency_code),sizeof(CRF01), "Crf01Table");
	if (lReturnCode >= 0)
	{
			conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
			sprintf(temp_iso_ration,"%f",1/conv_rate);
	}

	pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;

	falcon_log_message(2,1,"FalconRT: New request from falcon controller","RqMssg_RealTime",1);

	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );
	strcpy(key,time_date);
	strcat(key,".");
	strcat(key,Auth_Tx.TLF01_details.sys_trace_audit_num);
	strcat(key,Auth_Tx.TLF01_details.card_num);
	strcat(key,Auth_Tx.TLF01_details.total_amount);
	strcat(key,Auth_Tx.TLF01_details.terminal_id);
	strcat(key,Auth_Tx.TLF01_details.merchant_id);
	strcat(key,Auth_Tx.TLF01_details.auth_number);
	strcat(key,Auth_Tx.TLF01_details.nii);
	/*sprintf(msgBuffer,"CRTRAN message built with a unique Key:%s\n",key);
	falcon_log_message(2,1,msgBuffer,"RqMssg_RealTime",0);*/

	FALCON_read_CUP_merchant_indicator();
	FALCON_read_CUP_MP_Bin_range_len_1();
	FALCON_read_CUP_MP_Bin_range_len_2();
	FALCON_read_CUP_MP_Bin_range_len_3();
	FALCON_read_CUP_MP_Bin_range_len_4();
	FALCON_read_CUP_MP_Bin_range_len_5();
	FALCON_read_CUP_MP_Bin_range_1_indicator();
	FALCON_read_CUP_MP_Bin_range_2_indicator();
	FALCON_read_CUP_MP_Bin_range_3_indicator();
	FALCON_read_CUP_MP_Bin_range_4_indicator();
	FALCON_read_CUP_MP_Bin_range_5_indicator();
	/*sprintf(msgBuffer,"Bin Values read for CUP MP from tf.ini: \nBin_range_1_indicator:%s , Bin_range_1_len:%d\n Bin_range_2_indicator:%s , Bin_range_2_len:%d\n Bin_range_3_indicator:%s , Bin_range_3_len:%d\n Bin_range_4_indicator:%s , Bin_range_4_len:%d\n Bin_range_5_indicator:%s , Bin_range_5_len:%d\n",
			CUP_MP_Bin_range_1,CUP_MP_Bin_range_len_1,CUP_MP_Bin_range_2,CUP_MP_Bin_range_len_2,CUP_MP_Bin_range_3,CUP_MP_Bin_range_len_3,
			CUP_MP_Bin_range_4,CUP_MP_Bin_range_len_4,CUP_MP_Bin_range_5,CUP_MP_Bin_range_len_5);
	falcon_log_message(2,1,msgBuffer,"RqMssg_RealTime",0);*/


	FALCON_Read_BDO_PAY_QR_NII_value();

	FALCON_build_request_RealTime_HeaderFile();
	RT_FALCON_build_request_field_1( );
	RT_FALCON_build_request_field_2( );
	RT_FALCON_build_request_field_3( );
	RT_FALCON_build_request_field_4( );
	RT_FALCON_build_request_field_5( );
	RT_FALCON_build_request_field_6( );
	RT_FALCON_build_request_field_7( );
	RT_FALCON_build_request_field_8( );
	RT_FALCON_build_request_field_9( );
	RT_FALCON_build_request_field_10( );
	RT_FALCON_build_request_field_11( );
	RT_FALCON_build_request_field_12( );
	RT_FALCON_build_request_field_13( );
	RT_FALCON_build_request_field_14( );
	RT_FALCON_build_request_field_15( );
	RT_FALCON_build_request_field_16( );
	RT_FALCON_build_request_field_17( );
	RT_FALCON_build_request_field_18( );
	RT_FALCON_build_request_field_19( );
	RT_FALCON_build_request_field_20( );
	RT_FALCON_build_request_field_21( );
	RT_FALCON_build_request_field_22( );
	RT_FALCON_build_request_field_23( );
	RT_FALCON_build_request_field_24( );
	RT_FALCON_build_request_field_25( );
	RT_FALCON_build_request_field_26( );
	RT_FALCON_build_request_field_27( );
	RT_FALCON_build_request_field_28( );
	RT_FALCON_build_request_field_29( );
	RT_FALCON_build_request_field_30( );
	RT_FALCON_build_request_field_31( );
	RT_FALCON_build_request_field_32( );
	RT_FALCON_build_request_field_33( );
	RT_FALCON_build_request_field_34( );
	RT_FALCON_build_request_field_35( );
	RT_FALCON_build_request_field_36( );
	RT_FALCON_build_request_field_37( );
	RT_FALCON_build_request_field_38( );
	RT_FALCON_build_request_field_39( );
	RT_FALCON_build_request_field_40( );
	RT_FALCON_build_request_field_41( );
	RT_FALCON_build_request_field_42( );
	RT_FALCON_build_request_field_43( );
	RT_FALCON_build_request_field_44( );
	RT_FALCON_build_request_field_45( );
	RT_FALCON_build_request_field_46( );
	RT_FALCON_build_request_field_47( );
	RT_FALCON_build_request_field_48( );
	RT_FALCON_build_request_field_49( );
	RT_FALCON_build_request_field_50( );
	RT_FALCON_build_request_field_51( );
	RT_FALCON_build_request_field_52( );
	RT_FALCON_build_request_field_53( );
	RT_FALCON_build_request_field_54( );
	RT_FALCON_build_request_field_55( );
	RT_FALCON_build_request_field_56( );
	RT_FALCON_build_request_field_57( );
	RT_FALCON_build_request_field_58( );
	RT_FALCON_build_request_field_59( );
	RT_FALCON_build_request_field_60( );
	RT_FALCON_build_request_field_61( );
	RT_FALCON_build_request_field_62( );
	RT_FALCON_build_request_field_63( );
	RT_FALCON_build_request_field_64( );
	RT_FALCON_build_request_field_65( );
	RT_FALCON_build_request_field_66( );
	RT_FALCON_build_request_field_67( );
	RT_FALCON_build_request_field_68( );
	RT_FALCON_build_request_field_69( );
	RT_FALCON_build_request_field_70( );
	RT_FALCON_build_request_field_71( );
	RT_FALCON_build_request_field_72( );
	RT_FALCON_build_request_field_73( );
	RT_FALCON_build_request_field_74( );
	RT_FALCON_build_request_field_75( );
	RT_FALCON_build_request_field_76( );
	RT_FALCON_build_request_field_77( );
	RT_FALCON_build_request_field_78( );
	RT_FALCON_build_request_field_79( );
	RT_FALCON_build_request_field_80( );
	RT_FALCON_build_request_field_81( );
	RT_FALCON_build_request_field_82( );
	RT_FALCON_build_request_field_83( );
	RT_FALCON_build_request_field_84( );
	RT_FALCON_build_request_field_85( );
	RT_FALCON_build_request_field_86( );
	RT_FALCON_build_request_field_87( );
	RT_FALCON_build_request_field_88( );
	RT_FALCON_build_request_field_89( );
	RT_FALCON_build_request_field_90( );
	RT_FALCON_build_request_field_91( );
	RT_FALCON_build_request_field_92( );
	RT_FALCON_build_request_field_93( );
	RT_FALCON_build_request_field_94( );
	RT_FALCON_build_request_field_95( );
	RT_FALCON_build_request_field_96( );
	RT_FALCON_build_request_field_97( );
	RT_FALCON_build_request_field_98( );
	RT_FALCON_build_request_field_99( );
	RT_FALCON_build_request_field_100( );
	RT_FALCON_build_request_field_101( );
	RT_FALCON_build_request_field_102( );
	RT_FALCON_build_request_field_103( );
	RT_FALCON_build_request_field_104( );
	RT_FALCON_build_request_field_105( );
	RT_FALCON_build_request_field_106( );
	RT_FALCON_build_request_field_107( );
	RT_FALCON_build_request_field_108( );
	RT_FALCON_build_request_field_109( );
	RT_FALCON_build_request_field_110( );
	RT_FALCON_build_request_field_111( );
	RT_FALCON_build_request_field_112( );
	RT_FALCON_build_request_field_113( );
	RT_FALCON_build_request_field_114( );
	RT_FALCON_build_request_field_115( );
	RT_FALCON_build_request_field_116( );
	RT_FALCON_build_request_field_117( );
	RT_FALCON_build_request_field_118( );
	RT_FALCON_build_request_field_119( );
	RT_FALCON_build_request_field_120( );
	RT_FALCON_build_request_field_121( );
	RT_FALCON_build_request_field_122( );
	RT_FALCON_build_request_field_123( );
	RT_FALCON_build_request_field_124( );
	RT_FALCON_build_request_field_125( );
	RT_FALCON_build_request_field_126( );
	RT_FALCON_build_request_field_127( );
	RT_FALCON_build_request_field_128( );
	RT_FALCON_build_request_field_129( );
	RT_FALCON_build_request_field_130( );
	RT_FALCON_build_request_field_131( );
	RT_FALCON_build_request_field_132( );
	RT_FALCON_build_request_field_133( );
	RT_FALCON_build_request_field_134( );
	RT_FALCON_build_request_field_135( );
	RT_FALCON_build_request_field_136( );
	RT_FALCON_build_request_field_137( );
	RT_FALCON_build_request_field_138( );
	RT_FALCON_build_request_field_139( );
	RT_FALCON_build_request_field_140( );
	RT_FALCON_build_request_field_141( );
	RT_FALCON_build_request_field_142( );
	RT_FALCON_build_request_field_143( );
	RT_FALCON_build_request_field_144( );
	RT_FALCON_build_request_field_145( );
}
void FALCON_build_request_Online_HeaderFile( )
{
	memcpy(Cr_Fld.HeaderFile,"O000009710000000000102tdriver   PMAX      0000000000                  ",70);
	sprintf(message,"%-70s\n",Cr_Fld.HeaderFile);
}

void FALCON_build_request_RealTime_HeaderFile( )
{
	char temp_hdr[101] = {0};
	char transcn_id[20] = {0};
	char buffer[256]={0};

	strncpy( transcn_id, Auth_Tx.TLF01_details.primary_key.transaction_id, TRNID_LEN_BEFOREDECIMAL);
    strncpy(transcn_id+TRNID_LEN_BEFOREDECIMAL, Auth_Tx.TLF01_details.primary_key.transaction_id+TRNID_LEN_AFTERDECIMAL,TRNDID_AFTERDECIMAL );

    strncpy(temp_hdr, "R000009710031100000102tdriver   PMAX      0000000000 ", 53);
	strncat(temp_hdr, transcn_id, TRANSCN_ID_LEN);
	strncat(temp_hdr, Auth_Tx.TLF01_details.retrieval_ref_num, RRN_LEN);
	strncat(temp_hdr, "                 ", 17);

	memcpy(Cr_Fld_RT.HeaderFile, temp_hdr, FALCON_RT_HDR_LEN);
	sprintf(message,"%-101s\n",Cr_Fld_RT.HeaderFile);

	sprintf(buffer,"FalconRT: Request Tranid+RRN (%s%.12s)",transcn_id,Auth_Tx.TLF01_details.retrieval_ref_num);
	falcon_log_message(2,1,buffer,"FALCON_build_request_Online_HeaderFile",1);
}

/* field 1*/
void FALCON_build_request_field_1( )
{
	memcpy(Cr_Fld.WORKFLOW_XCD,profile_str,16);
	sprintf(&message[70],"%-16s\n",Cr_Fld.WORKFLOW_XCD);
}
void FALCON_build_request_field_2( )
{
	memcpy(Cr_Fld.RECORD_TYPE_XCD,"CRTRAN24",8);
	sprintf(&message[86],"%-8s\n",Cr_Fld.RECORD_TYPE_XCD);
}
void FALCON_build_request_field_3(  )
{
	memcpy(Cr_Fld.DATA_SPECIFICATION_VERSION_XCD,"2.4",5);
	sprintf(&message[94],"%-5s\n",Cr_Fld.DATA_SPECIFICATION_VERSION_XCD);
}
void FALCON_build_request_field_4( )
{
	memcpy(Cr_Fld.CLIENT_XID,"DT_BDO_CR",16);
	sprintf(&message[99],"%-16s\n",Cr_Fld.CLIENT_XID);
}
void FALCON_build_request_field_5( )
{
	memcpy(Cr_Fld.RCD_CRT_DATE,date_str,8);
	sprintf(&message[115],"%-8s\n",Cr_Fld.RCD_CRT_DATE);
}
void FALCON_build_request_field_6( )
{
	memcpy(Cr_Fld.RCD_CRT_TIME,time_str,6);
	sprintf(&message[123],"%-6s\n",Cr_Fld.RCD_CRT_TIME);
}
void FALCON_build_request_field_7( )
{
	memcpy(Cr_Fld.RCD_CRT_MSCND,time_milliseconds,3);
	sprintf(&message[129],"%-3s\n",Cr_Fld.RCD_CRT_MSCND);
}
void FALCON_build_request_field_8( )
{
	memcpy(Cr_Fld.GMT_OFFSET,"8.00",6);
	sprintf(&message[132],"%-6s\n",Cr_Fld.GMT_OFFSET);
}
void FALCON_build_request_field_9( )
{
	memcpy(Cr_Fld.CUSTOMER_XID,Auth_Tx.TLF01_details.card_num,20);
	sprintf(&message[138],"%-20s\n",Cr_Fld.CUSTOMER_XID);
}
void FALCON_build_request_field_10( )
{
	memcpy(Cr_Fld.ACCOUNT_REFERENCE_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[158],"%-40s\n",Cr_Fld.ACCOUNT_REFERENCE_XID);
}
void FALCON_build_request_field_11( )
{
	char temp_Tran_id[33]={0};
	static INT   falcon_tx_counter=0;
	memset(temp_Tran_id,' ',sizeof(temp_Tran_id)-1);
	sprintf(temp_Tran_id,"%-20s-CRT-%07d",Auth_Tx.TLF01_details.primary_key.transaction_id,falcon_tx_counter++);
	/*strncpy(temp_Tran_id,Auth_Tx.TLF01_details.primary_key.transaction_id,20);
	strcat(temp_Tran_id,"-CRT-");
	sprintf(&temp_Tran_id[25],"%07d",falcon_tx_counter++);*/
	strncpy(Cr_Fld.EXTERNAL_TRANSACTION_XID,temp_Tran_id,32);
	sprintf(&message[198],"%-32s\n",Cr_Fld.EXTERNAL_TRANSACTION_XID);
}
void FALCON_build_request_field_12( )
{
	memcpy(Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[230],"%-19s\n",Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID);
}
void FALCON_build_request_field_13( )
{
	memcpy(Cr_Fld.AUTHORIZATION_POSTING_XCD,"A",1);
	sprintf(&message[249],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_XCD);
}
void FALCON_build_request_field_14( )
{
	memset(Cr_Fld.CARD_POSTAL_XCD,' ',9);
	sprintf(&message[250],"%-9s\n",Cr_Fld.CARD_POSTAL_XCD);
}
void FALCON_build_request_field_15( )
{
	memset(Cr_Fld.CARD_COUNTRY_XCD,' ',3);
	sprintf(&message[259],"%-3s\n",Cr_Fld.CARD_COUNTRY_XCD);
}
void FALCON_build_request_field_16( )
{
	memset(Cr_Fld.CARD_OPEN_DT,' ',8);
	sprintf(&message[262],"%-8s\n",Cr_Fld.CARD_OPEN_DT);
}
void FALCON_build_request_field_17( )
{
	memset(Cr_Fld.CARD_ISSUE_DT,' ',8);
	sprintf(&message[270],"%-8s\n",Cr_Fld.CARD_ISSUE_DT);
}
void FALCON_build_request_field_18( )
{
	memset(Cr_Fld.CARD_ISSUE_TYPE_XCD,' ',1);
	sprintf(&message[278],"%-1s\n",Cr_Fld.CARD_ISSUE_TYPE_XCD);
}
void FALCON_build_request_field_19( )
{
	memset(Cr_Fld.ACCOUNT_EXPIRATION_DT,' ',8);
	sprintf(&message[279],"%-8s\n",Cr_Fld.ACCOUNT_EXPIRATION_DT);
}

void FALCON_build_request_field_20( )
{

	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[8]={0};

	strncpy(tem_month,&Auth_Tx.TLF01_details.exp_date[2],2);
	strncpy(temp_exp,"20",2);
	if(strlen(Auth_Tx.TLF01_details.exp_date) > 0)
	{
		strncat(temp_exp,Auth_Tx.TLF01_details.exp_date,4);
	}
	else
	{
		strncpy(temp_exp,Auth_Tx.TLF01_details.date_yyyymmdd,6);
		strncpy(tem_month,&Auth_Tx.TLF01_details.date_yyyymmdd[4],2);
	}
	if( strncmp(tem_month,"",2) == 0 || strncmp(tem_month,"  ",2) == 0 || strncmp(tem_month,"00",2) == 0)
	{
		strncpy(temp_exp,"        ",8);
	}
	else if(strncmp(tem_month,"02",2) == 0)
	{
		strncat(temp_exp,"28",2);
	}
	else
	{
		strncat(temp_exp,"30",2);
	}

	strncpy(Cr_Fld.CARD_EXPIRATION_DT,temp_exp,8);
	sprintf(&message[287],"%-8s\n",Cr_Fld.CARD_EXPIRATION_DT);
}
void FALCON_build_request_field_21( )
{
	if(strcmp(Auth_Tx.ACF01_details.available,"") != 0)
	{
		char temp_avail_amt[11]="";
		strncpy(temp_avail_amt,&Auth_Tx.ACF01_details.available[2],10);
		sprintf(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,"%07d.%02d",atoi(temp_avail_amt)/100, atoi(temp_avail_amt)%100);
		sprintf(&message[295],"%-10s\n",Cr_Fld.PRIOR_CREDIT_LIMIT_AMT);
	}
	else
	{
		strcpy(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,"0000000.00");
		sprintf(&message[295],"%-10s\n",Cr_Fld.PRIOR_CREDIT_LIMIT_AMT);
	}
}
void FALCON_build_request_field_22( )
{
	memset(Cr_Fld.CARD_CREDIT_LIMIT_AMT,' ',10);
	sprintf(&message[305],"%-10s\n",Cr_Fld.CARD_CREDIT_LIMIT_AMT);
}
void FALCON_build_request_field_23( )
{
	memset(Cr_Fld.CUSTOMER_GENDER_XCD,' ',1);
	sprintf(&message[315],"%-1s\n",Cr_Fld.CUSTOMER_GENDER_XCD);
}
void FALCON_build_request_field_24( )
{
	memset(Cr_Fld.CUSTOMER_BIRTH_DT,' ',8);
	sprintf(&message[316],"%-8s\n",Cr_Fld.CUSTOMER_BIRTH_DT);
}
void FALCON_build_request_field_25( )
{
	memset(Cr_Fld.CUSTOMER_CARD_CNT,' ',3);
	sprintf(&message[324],"%-3s\n",Cr_Fld.CUSTOMER_CARD_CNT);
}
void FALCON_build_request_field_26( )
{
	memset(Cr_Fld.INCOME_AMT,' ',10);
	sprintf(&message[327],"%-10s\n",Cr_Fld.INCOME_AMT);
}
void FALCON_build_request_field_27( )
{
	memset(Cr_Fld.CARD_TYPE_XCD,' ',1);
	sprintf(&message[337],"%-1s\n",Cr_Fld.CARD_TYPE_XCD);
}
void FALCON_build_request_field_28( )
{
	memset(Cr_Fld.PRIMARY_CARD_USE_XCD,' ',1);
	sprintf(&message[338],"%-1s\n",Cr_Fld.PRIMARY_CARD_USE_XCD);
}
void FALCON_build_request_field_29( )
{
	memcpy(Cr_Fld.TRANSACTION_DATE,Auth_Tx.TLF01_details.date_yyyymmdd,8);
	sprintf(&message[339],"%-8s\n",Cr_Fld.TRANSACTION_DATE);
}
void FALCON_build_request_field_30( )
{
	memcpy(Cr_Fld.TRANSACTION_TIME,Auth_Tx.TLF01_details.time_hhmmss,6);
	sprintf(&message[347],"%-6s\n",Cr_Fld.TRANSACTION_TIME);
}

void FALCON_build_request_field_31( )
{
    if (Check_If_MC_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_MC();
	}
    else if (Check_If_VISA_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_VISA();
	}
    else if (Check_If_CUP_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_CUP();
	}
    else if (Check_If_JCB_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_JCB();
	}
    else if (Check_If_DINERS_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_DINERS();
	}
    else if (Check_If_AMEX_Transaction ()    == true)
	{

    	len_billing_amount = strlen(Auth_Tx.TLF01_details.settlement_total);

			if(len_billing_amount == 0)
			{
				billing_amount = atoi(Auth_Tx.reconciliation_amount);
				if(billing_amount > 0)
				{
					len_billing_amount = strlen(Auth_Tx.reconciliation_amount);
					strncpy(amount_digit,Auth_Tx.reconciliation_amount,10);
					strncpy(last2_decimal,Auth_Tx.reconciliation_amount+10,2);
					/*billing_amount = (strtod((char *)amount_digit,0));*/
					billing_amount = (strtod(Auth_Tx.reconciliation_amount, 0));
				}
				else
				{
					strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
					strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
					/*billing_amount = (strtod(amount_digit,0));*/
					billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
				}
			}
			else
			{
				  strncpy(amount_digit,Auth_Tx.TLF01_details.settlement_total,10);
				  strncpy(last2_decimal,Auth_Tx.TLF01_details.settlement_total+10,2);
				  /*billing_amount = (strtod(amount_digit,0));*/
				  billing_amount = (strtod(Auth_Tx.TLF01_details.settlement_total, 0));
			}

    	Falcon_build_CRTRAN_field_31_AMEX();
	}
    else if (Check_If_Fleet_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_Fleet();
	}
    else
    {
    	strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
    }

    sprintf(&message[353],"%-13s\n",Cr_Fld.TRANSACTION_AMT);

}

void FALCON_build_request_field_32( )
{
	strncpy(Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD,"608",3);
	sprintf(&message[366],"%-3s\n",Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD);
}

void FALCON_build_request_field_33( )
{
	char temp_iso_ration_value[13] = {0};

	strncpy(temp_iso_ration_value,"00000",5);
	strncat(temp_iso_ration_value,temp_iso_ration,8);
	strncpy(Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO,temp_iso_ration_value,13);
	sprintf(&message[369],"%-13s\n",Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO);
}


	/*
	Pls. map:
	A = 00, 10, 11, 32, 85
	D = 03, 05, 06, 12, 13, 14, 19, 20, 30, 33, 39, 42, 44, 51, 52, 53, 54, 55, 56, 57, 58, 61, 63, 64, 82, 92, N7
	P = 04, 07, 41, 43
	R = 01, 02, 08,34, 35, 36, 37, 38, 60, 62, 65, 66, 75,
	Else �BLANK�
	*/
void FALCON_build_request_field_34( )
{
	if( strncmp(Auth_Tx.TLF01_details.response_code,"00",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"10",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"11",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"32",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"85",2)== 0 )
	{
		strncpy(Cr_Fld.DECISION_XCD,"A",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"03",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"05",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"06",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"12",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"13",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"14",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"19",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"20",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"30",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"33",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"39",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"42",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"51",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"52",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"53",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"54",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"55",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"56",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"57",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"58",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"61",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"63",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"64",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"82",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"92",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"N7",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"44",2)== 0)
	{
			strncpy(Cr_Fld.DECISION_XCD,"D",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"04",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"07",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"41",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"43",2)== 0	)
	{
		strncpy(Cr_Fld.DECISION_XCD,"P",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"01",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"02",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"08",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"34",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"35",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"36",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"37",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"38",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"60",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"62",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"65",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"66",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"75",2)== 0	)
	{
		strncpy(Cr_Fld.DECISION_XCD,"R",1);
	}
	else
	{
		strncpy(Cr_Fld.DECISION_XCD," ",1);
	}

	sprintf(&message[382],"%-1s\n",Cr_Fld.DECISION_XCD);
}
void FALCON_build_request_field_35()
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_SALE          || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE        || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE)
	{
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"M",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE ||
		    Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE)
	{/* CASH advance*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"C",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE || Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
	{/*Reversal*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"V",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
	{/*VOID */
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"X",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)
	{/*Pre AUTH*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"P",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
	{/*Offline sale*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"D",1);
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD," ",1);
	}

	sprintf(&message[383],"%-1s\n",Cr_Fld.TRANSACTION_TYPE_XCD);
}
void FALCON_build_request_field_36()
{
	memcpy(Cr_Fld.MERCHANT_CATEGORY_XCD,Auth_Tx.TLF01_details.category_code,4);
	sprintf(&message[384],"%-4s\n",Cr_Fld.MERCHANT_CATEGORY_XCD);
}/*******memcpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);*****/
void FALCON_build_request_field_37()
{
	if(strncmp(Auth_Tx.MCF01_details.zip_code,"9999",4)== 0)
	{
		memset(Cr_Fld.MERCHANT_POSTAL_XCD,' ',9);
	}
	else
	{
		strncpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);
	}
	sprintf(&message[388],"%-9s\n",Cr_Fld.MERCHANT_POSTAL_XCD);
}
void FALCON_build_request_field_38()
{
	if(strlen(Auth_Tx.MCF01_details.primary_key.merchant_id)> 0)
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,"608",3);
	}
	else if(strlen(Auth_Tx.reconciliation_cur_code)> 0)/*reconciliation_cur_code*/
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
	}
	else if(strlen(Auth_Tx.TLF01_details.num_dbsales)> 0)/*nfi_seq_nbr*/
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.num_dbsales,3);
	}
	else
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);

	sprintf(&message[397],"%-3s\n",Cr_Fld.MERCHANT_COUNTRY_XCD);
}
void FALCON_build_request_field_39()
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(strcmp(Auth_Tx.TLF01_details.response_code, "00") == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}

	}
	else if(strncmp(Cr_Fld.TRANSACTION_TYPE_XCD,"C",1)== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.response_code, "55" ,2) == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}
		else
		{
				memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
	}
	else
	{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",1);
	}

	sprintf(&message[400],"%-1s\n",Cr_Fld.TRANSACTION_PIN_VERIFY_XCD);
}
/*switch(Auth_Tx.TLF01_details.cvc)
{
case 'Y': memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		  break;
case 'N': memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		  break;
case 'M': memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		  break;
default:memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
		sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		break;
}*/
void FALCON_build_request_field_40()
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		/* mapping variable need to check*/
		if(Auth_Tx.TLF01_details.cvc == 'Y' || Auth_Tx.TLF01_details.cvc == 'N' || Auth_Tx.TLF01_details.cvc == 'E' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"N",1);
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"M",1);
		}
		else
		{
			memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
			memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		/* mapping variable need to check*/

		if(Auth_Tx.TLF01_details.cvc == '1')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"N",1);
		}
		else if(Auth_Tx.TLF01_details.cvc == '2' || Auth_Tx.TLF01_details.cvc == '3' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"M",1);
		}
		else
		{
			memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
			memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if (Auth_Tx.future_use_2[22] == '1' || Auth_Tx.future_use_2[23] == '1')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
		}
		else if(Auth_Tx.future_use_2[22] == '2' || Auth_Tx.future_use_2[23] == '2')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		}
		else if (Auth_Tx.future_use_2[22] == '3' || Auth_Tx.future_use_2[23] == '3'||
				 Auth_Tx.future_use_2[22] == ' ' || Auth_Tx.future_use_2[23] == ' ')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD," ",1);
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if( strncmp(&Auth_Tx.acct_id_1[2],"001",3) == 0 ||
		    strncmp(&Auth_Tx.acct_id_1[2],"016",3) == 0	)
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
		}
		else if( strncmp(&Auth_Tx.acct_id_1[2],"002",3) == 0 ||
			     strncmp(&Auth_Tx.acct_id_1[2],"017",3) == 0 )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		}
		else
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD," ",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0 ||
			strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
	}
	else
	{
		memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
	}

	 sprintf(&message[401],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
}

/*	Please refer to POS netry mode mapping doc : # */
void FALCON_build_request_field_41()
{
	char temp_pos_entry_mode[3]={0};
	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)== 3)
	{
		strncpy(temp_pos_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,2);
	}
	else
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}

	if((Check_If_AMEX_Transaction())== true)
	{
		Falcon_AMEX_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_DINERS_Transaction())== true)
	{
		Falcon_DINERS_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_CUP_Transaction())== true)
	{
		Falcon_CUP_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_JCB_Transaction()) == true)
	{
		Falcon_JCB_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_MC_Transaction())== true)
	{
		Falcon_MC_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_VISA_Transaction())== true)
	{
		Falcon_VISA_posEntryMode_F41(temp_pos_entry_mode);
	}

	if(((strncmp(temp_pos_entry_mode,"02",2) == 0  ||
			  strncmp(temp_pos_entry_mode,"90",2) == 0) &&
			  strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0 ))
	{ /* POS 02 or 90 + Field 60.2.2 = 5 or 6 + Field 60.2.3 = 2 */
		if( strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"052",3)== 0 ||
			(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"062",3)== 0))
		{
			strncpy(temp_pos_entry_mode,"80",2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[10],"4",1)== 0)
		{
			strncpy(temp_pos_entry_mode,"91",2);
		}
		else
		{
			strncpy(temp_pos_entry_mode,"90",2);
		}
	}
	else if( strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
	{
		strncpy(temp_pos_entry_mode,"90",2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"092",3) == 0  ||
			strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0092",4) == 0 )
	{
			strncpy(temp_pos_entry_mode,"79",2);
	}
	else if((strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)&&
			(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0) )
	{
		strncpy(temp_pos_entry_mode,"91",2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3) == 0 )
	{
		strncpy(temp_pos_entry_mode,"80",2);
	}
	else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0 )
	{
		strncpy(temp_pos_entry_mode,"80",2);
	}

	if(FALCON_is_BDO_PAY_QR_Transaction() == true)
	{
		if((Is_BDO_Pos_Acquiring_Transaction()) == true)
		{
			strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"X",1);
		}
	}

	else if(strncmp(temp_pos_entry_mode,"91",2) == 0 || strncmp(temp_pos_entry_mode,"2W",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	}
	else if(strncmp(temp_pos_entry_mode,"92",2) == 0   ||
			strncmp(temp_pos_entry_mode,"07",2) == 0   ||
			strncmp(temp_pos_entry_mode,"06",2) == 0   ||
			strncmp(temp_pos_entry_mode,"X5",2) == 0	)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
	}
	else if((FALCON_is_Credential_On_File() ==true) ||(strncmp(temp_pos_entry_mode,"81",2) == 0))
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1);
	}
	else if(strncmp(temp_pos_entry_mode,"80",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	}
	else if(strncmp(temp_pos_entry_mode,"79",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1);
	}
	else if(strncmp(temp_pos_entry_mode,"01",2) == 0 )
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	}
	else if(strncmp(temp_pos_entry_mode,"02",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1);
	}
	else if(strncmp(temp_pos_entry_mode,"90",2) == 0 ||
			strncmp(temp_pos_entry_mode,"97",2) == 0 )
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1);
	}
	else if(strncmp(temp_pos_entry_mode,"05",2) == 0 || strncmp(temp_pos_entry_mode,"52",2) == 0 ||
			strncmp(temp_pos_entry_mode,"51",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1);
	}
	else if(strncmp(temp_pos_entry_mode,"09",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	}

	else
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG," ",1);
	}

	sprintf(&message[402],"%-1s\n",Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG);
}
void FALCON_build_request_field_42( )
{
	if(Cr_Fld.AUTHORIZATION_POSTING_XCD == "P")
	{
		memcpy(Cr_Fld.TRANSACTION_POSTING_DT,Auth_Tx.TLF01_details.date_yyyymmdd,8);
		sprintf(&message[403],"%-8s\n",Cr_Fld.TRANSACTION_POSTING_DT);
	}
	else
	{
		memset(Cr_Fld.TRANSACTION_POSTING_DT,' ',8);
		sprintf(&message[403],"%-8s\n",Cr_Fld.TRANSACTION_POSTING_DT);
	}
}
void FALCON_build_request_field_43( )
{
	memset(Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD,' ',1);
	sprintf(&message[411],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD);
}
void FALCON_build_request_field_44( )
{
	memset(Cr_Fld.TRANS_POSTING_MISMATCH_XFLG,' ',1);
	sprintf(&message[412],"%-1s\n",Cr_Fld.TRANS_POSTING_MISMATCH_XFLG);
}
void FALCON_build_request_field_45( )
{
	memset(Cr_Fld.CREATE_CASE_XFLG,' ',1);
	sprintf(&message[413],"%-1s\n",Cr_Fld.CREATE_CASE_XFLG);
}
void FALCON_build_request_field_46( )
{
	memset(Cr_Fld.USER_INDICATOR_1_XCD,' ',1);
	sprintf(&message[414],"%-1s\n",Cr_Fld.USER_INDICATOR_1_XCD);
}
void FALCON_build_request_field_47( )
{
	memset(Cr_Fld.USER_INDICATOR_2_XCD,' ',1);
	sprintf(&message[415],"%-1s\n",Cr_Fld.USER_INDICATOR_2_XCD);
}
void FALCON_build_request_field_48( )
{
	memset(Cr_Fld.USER_DATA_1_STRG,' ',10); /* UCAF indicator DE48 SE43*/

	/*Field 48 for Terminal type*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.additional_pos_information[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.acceptor_term_name[9],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		strncpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.amex_pos_entry_mode[3],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)== 0)
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"08", 2);/*Mobile phone Voice transaction*/
			}
			else
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"07", 2);/*PC Voice transaction*/
			}
		}
		else
		{
			memcpy(Cr_Fld.USER_DATA_1_STRG,"03", 2);/*POS-Point Of Sale 03*/
		}
	}

	sprintf(&message[416],"%-10s\n",Cr_Fld.USER_DATA_1_STRG);
}
void FALCON_build_request_field_49( )
{
	memset(Cr_Fld.USER_DATA_2_STRG,' ',10);
	/*Field 49 for Terminal Entry capa*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[1],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.acceptor_term_name[10],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		/*if(Auth_Tx.TLF01_details.product_codes[16].amount[0]=='5')
			strncpy(Cr_Fld.USER_DATA_2_STRG,"C",1);
		else*/
			memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.TLF01_details.product_codes[16].amount[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0  ||
			strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1) == 0 ||
			strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"T",1) == 0 ||
			strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1) == 0)
		{
			strncpy(Cr_Fld.USER_DATA_2_STRG,"2",1);
		}
		else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0)
		{
			strncpy(Cr_Fld.USER_DATA_2_STRG,"5",1);
		}
		else
		{
			strncpy(Cr_Fld.USER_DATA_2_STRG,"0",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)
	{
		Cr_Fld.USER_DATA_2_STRG[0] = Auth_Tx.amex_pos_entry_mode[0] ;
		Cr_Fld.USER_DATA_2_STRG[1] = '\0';
	}
	/*if(strcmp(Auth_Tx.BON01_details.minimum_amount,"")!= 0)
	{
		strncpy(Cr_Fld.USER_DATA_2_STRG,Auth_Tx.BON01_details.minimum_amount,10);
	}*/
	sprintf(&message[426],"%-10s\n",Cr_Fld.USER_DATA_2_STRG);
}
void FALCON_build_request_field_50( )
{
	memset(Cr_Fld.MERCHANT_XIDD,' ',10);
	sprintf(&message[436],"%-10s\n",Cr_Fld.MERCHANT_XIDD);
}
void FALCON_build_request_field_51( )
{
	memset(Cr_Fld.MERCHANT_DATA_XFLG,' ',1);
	sprintf(&message[446],"%-1s\n",Cr_Fld.MERCHANT_DATA_XFLG);
}
void FALCON_build_request_field_52( )
{
	memset(Cr_Fld.CARD_DATA_AVAILABLE_XFLG,' ',1);
	sprintf(&message[447],"%-1s\n",Cr_Fld.CARD_DATA_AVAILABLE_XFLG);
}
void FALCON_build_request_field_53( )
{
	memcpy(Cr_Fld.EXTERNAL_1_SCR,"0000",4);
	sprintf(&message[448],"%-4s\n",Cr_Fld.EXTERNAL_1_SCR);
}
void FALCON_build_request_field_54( )
{
	memcpy(Cr_Fld.EXTERNAL_2_SCR,"0000",4);
	sprintf(&message[452],"%-4s\n",Cr_Fld.EXTERNAL_2_SCR);
}
void FALCON_build_request_field_55( )
{
	memcpy(Cr_Fld.EXTERNAL_3_SCR,"0000",4);
	sprintf(&message[456],"%-4s\n",Cr_Fld.EXTERNAL_3_SCR);

}
void FALCON_build_request_field_56( )
{
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"T",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0)
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"Y",1);
		memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",1);
	}
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0 )
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"N",1);
		if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",1);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",1);
		}
	}
	else
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG," ",1);
		memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);
	}

	sprintf(&message[460],"%-1s\n",Cr_Fld.CARDHOLDER_PRESENT_XFLG);
}

void FALCON_build_request_field_57( )
{
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
	{
		memcpy(Cr_Fld.CAT_TYPE_XFLG,"E",1);/* POS Condition code*/
	}
	else
	{
		memcpy(Cr_Fld.CAT_TYPE_XFLG,"S",1);/* POS Condition code*/
	}
	sprintf(&message[461],"%-1s\n",Cr_Fld.CAT_TYPE_XFLG);
}
void FALCON_build_request_field_58( )
{
	memset(Cr_Fld.TESTING_RANDOM_DIGITS_STRG,' ',2);
	sprintf(&message[462],"%-2s\n",Cr_Fld.TESTING_RANDOM_DIGITS_STRG);
}
void FALCON_build_request_field_59( )
{
	memset(Cr_Fld.PORTFOLIO_NAME,' ',14);
	sprintf(&message[464],"%-14s\n",Cr_Fld.PORTFOLIO_NAME);
}
void FALCON_build_request_field_60( )
{
	memset(Cr_Fld.CLIENT_2_XID,' ',14);
	sprintf(&message[478],"%-14s\n",Cr_Fld.CLIENT_2_XID);
}
void FALCON_build_request_field_61( )
{
	memset(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,' ',6);
	sprintf(&message[492],"%-6s\n",Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM);
}
void FALCON_build_request_field_62( )
{
	if(strlen(Auth_Tx.MCF01_details.name02)> 0)
	{
		memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
	}
	else if(strlen(Auth_Tx.TLF01_details.card_holder_name) > 0)
	{
		memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
	}

	sprintf(&message[498],"%-40s\n",Cr_Fld.MERCHANT_NAME);
}
void FALCON_build_request_field_63( )
{
	if(strlen(Auth_Tx.MCF01_details.city)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.TLF01_details.ncf01_retired_cwk,13);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.MCF01_details.city,20);
	}
	sprintf(&message[538],"%-30s\n",Cr_Fld.MERCHANT_CITY_NAME);
}
void FALCON_build_request_field_64( )
{
	if(strlen(Auth_Tx.MCF01_details.intl_code)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+13,3);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.MCF01_details.intl_code,3);
	}
	sprintf(&message[568],"%-3s\n",Cr_Fld.MERCHANT_STATE_XCD);
}
void FALCON_build_request_field_65( )
{
	memset(Cr_Fld.SUPPRES_CASE_XFLG,' ',1);
	sprintf(&message[571],"%-1s\n",Cr_Fld.SUPPRES_CASE_XFLG);
}
void FALCON_build_request_field_66( )
{
	/***memcpy(Cr_Fld.USER_INDICATOR_3_XCD,' ' ,5);****/
	memcpy(Cr_Fld.USER_INDICATOR_3_XCD,"",5);
	sprintf(&message[572],"%-5s\n",Cr_Fld.USER_INDICATOR_3_XCD);
}
	/*********************UserIndicator04****************************/
void FALCON_build_request_field_67( )
{
	int len_curr_code, len_TLF_curr_code = 0, len_recon_curr_code=0 ;

	len_TLF_curr_code = strlen(Auth_Tx.TLF01_details.currency_code);
	len_curr_code = strlen(Auth_Tx.MCF01_details.currency_code);
	len_recon_curr_code = strlen(Auth_Tx.reconciliation_cur_code);

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	    	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if(len_recon_curr_code > 0)
		{
			strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.reconciliation_cur_code,3);
		}
		else if (len_TLF_curr_code > 0)
		{
			strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
		}

	}
	else if(len_curr_code>0)
	{
		strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.MCF01_details.currency_code,3);
	}
	else if (len_TLF_curr_code > 0)
	{
		strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
	}
	else
	{
		memset(Cr_Fld.USER_INDICATOR_4_XCD,SPACES,FIELD67_SIZE);
	}
	sprintf(&message[577],"%-5s\n",Cr_Fld.USER_INDICATOR_4_XCD);
	/***************************************************************/
}

	/* Dual currrecny logic*/

	/**len**strcpy(Cr_Fld.USER_DATA_3_STRG,"000");**/
void FALCON_build_request_field_68( )
{
	char temp_totalamt[14]={0};
	strncpy(Cr_Fld.USER_DATA_3_STRG,"00",2);
	if((Check_If_DINERS_Transaction ()  == true)||
		(Check_If_CUP_Transaction ()  == true))
	{
		int original_tx_amt = 0;
		if(strlen(Auth_Tx.TLF01_details.orig_amount)>0)
		{
			original_tx_amt = atoi(Auth_Tx.TLF01_details.orig_amount);
		}
		else
		{
			original_tx_amt = atoi(Auth_Tx.TLF01_details.total_amount);
		}
		sprintf(temp_totalamt,"%010d.%02d",original_tx_amt/100, original_tx_amt%100);
	}
	else if((Check_If_VISA_Transaction () == true)||
			(Check_If_JCB_Transaction ()  == true)||
			(Check_If_MC_Transaction ()  == true)||
			(Check_If_Fleet_Transaction() == true)||
			 Check_If_AMEX_Transaction ()==true)
	{
		if(strlen(Auth_Tx.reconciliation_amount) > 0)
		{
			sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.reconciliation_amount)/100, atoi(Auth_Tx.reconciliation_amount)%100);
		}
		else
		{
			sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);
		}
	}
	strncat(Cr_Fld.USER_DATA_3_STRG,temp_totalamt,13);
	sprintf(&message[582],"%-15s\n",Cr_Fld.USER_DATA_3_STRG);
}
void FALCON_build_request_field_69( )
{
	if(strlen(Auth_Tx.TLF01_details.response_text)>0)
	{
		strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.response_text,20);
	}
	else
	{
		memset(Cr_Fld.USER_DATA_4_STRG,SPACES,20);
	}
	sprintf(&message[597],"%-20s\n",Cr_Fld.USER_DATA_4_STRG);
}
void FALCON_build_request_field_70( )
{
	CHAR resp_buf[15] = {0} ;
	memset(Cr_Fld.USER_DATA_5_STRG,' ',40);
	if ((Auth_Tx.TLF01_details.tx_key) > 100)
	{
		strncpy (resp_buf , " RESPONSE",10);
	}
	else
	{
		memset(resp_buf,0x00,sizeof(resp_buf)-1);
	}

	switch(Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_INVALID							  :

			strcpy(Cr_Fld.USER_DATA_5_STRG,"INVALID");
		break ;

		case AUTH_SALE                     	     	  :
		case AUTH_SALE_RESPONSE                       :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE");
		break ;

		case AUTH_AUTHORIZATION						  :
		case AUTH_AUTHORIZATION_RESPONSE			  :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"AUTHORIZATION");
		break ;

		case AUTH_CASH_ADVANCE				          :
		case AUTH_CASH_ADVANCE_RESPONSE               :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"CASH ADVANCE");
		break ;

		case AUTH_OFFLINE_SALE                        :
		case AUTH_OFFLINE_SALE_RESPONSE               :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"OFFLINE SALE");
		break ;

		case AUTH_SALE_ADJUSTMENT                     :
		case AUTH_SALE_ADJUSTMENT_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE ADJUSTMENT");
		break ;

		case AUTH_BALANCE_INQUIRY                     :
		case AUTH_BALANCE_INQUIRY_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"BALANCE INQUIRY");
		break ;

		case AUTH_PRE_AUTHORIZATION                   :
		case AUTH_PRE_AUTHORIZATION_RESPONSE          :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"PRE AUTHORIZATION");
		break ;

		case AUTH_VOID_SALE                           :
		case AUTH_VOID_SALE_RESPONSE                  :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID SALE");
		break ;

		case AUTH_VOID_REFUND                         :
		case AUTH_VOID_REFUND_RESPONSE                :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID REFUND");
		break ;

		case AUTH_REVERSAL                            :
		case AUTH_REVERSAL_RESPONSE                   :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"REVERSAL");
		break ;

		case AUTH_VOID_CASH_ADVANCE                   :
		case AUTH_VOID_CASH_ADVANCE_RESPONSE          :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID CASH ADVANCE");
		break ;

		case AUTH_SALE_CASH                           :
		case AUTH_SALE_CASH_RESPONSE                  :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE CASH");
		break ;

		case AUTH_SALES_COMPLETION                    :
		case AUTH_SALES_COMPLETION_RESPONSE           :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE COMPLETION");
		break ;

		case AUTH_QUASI_CASH                          :
		case AUTH_QUASI_CASH_RESPONSE                 :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"QUASI CASH");
		break ;

		case AUTH_ADVICE                              :
		case AUTH_ADVICE_RESPONSE                     :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"ADVICE");
		break ;

		case AUTH_REVERSAL_ADVICE                     :
		case AUTH_REVERSAL_ADVICE_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"REVERSAL ADVICE");
		break ;

		case AUTH_ADVICE_REPEAT                       :
		case AUTH_ADVICE_REPEAT_RESPONSE              :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"ADVICE REPEAT");
		break ;

		case AUTH_REVERSAL_REPEAT                     :
		case AUTH_REVERSAL_REPEAT_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"REVERSAL REPEAT");
		break ;

		case AUTH_CASH_ADVANCE_AUTHORIZATION          :
		case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"CASH ADVANCE AUTHORIZATION");
		break ;

		case AUTH_AUTHORIZATION_OVERRIDE              :
		case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE     :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"AUTHORIZATION OVERRIDE");
		break ;

		case AUTH_OFFLINE_VOID_SALE                   :
		case AUTH_OFFLINE_VOID_SALE_RESPONSE          :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"OFFLINE VOID SALE");
		break ;

		case AUTH_PRE_AUTHORIZATION_CANCLE            :
		case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE   :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"PRE AUTHORIZATION CANCLE");
		break ;

	}
	strcat(Cr_Fld.USER_DATA_5_STRG,resp_buf);


	sprintf(&message[617],"%-40s\n",Cr_Fld.USER_DATA_5_STRG);
}
void FALCON_build_request_field_71( )
{
	memset(Cr_Fld.REAL_TIME_REQUEST_XFLG,' ',1);
	sprintf(&message[657],"%-1s\n",Cr_Fld.REAL_TIME_REQUEST_XFLG);
}
void FALCON_build_request_field_72( )
{
	memset(Cr_Fld.PRIOR_ACTION_DATABASE_XCD,' ',1);
	sprintf(&message[658],"%-1s\n",Cr_Fld.PRIOR_ACTION_DATABASE_XCD);
}
void FALCON_build_request_field_73( )
{
	memset(Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT,' ',8);
	sprintf(&message[659],"%-8s\n",Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT);
}
void FALCON_build_request_field_74( )
{
	memset(Cr_Fld.MASTER_ACCOUNT_NUMBER_XID,' ',19);
	sprintf(&message[667],"%-19s\n",Cr_Fld.MASTER_ACCOUNT_NUMBER_XID);
}
void FALCON_build_request_field_75( )
{
	memset(Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG,' ',1);
	sprintf(&message[686],"%-1s\n",Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG);
}
void FALCON_build_request_field_76( )
{
	memset(Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD,' ',1);
	sprintf(&message[687],"%-1s\n",Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD);
}
void FALCON_build_request_field_77( )
{
	memset(Cr_Fld.RESERVED_02,' ',1);
	sprintf(&message[688],"%-1s\n",Cr_Fld.RESERVED_02);
}
void FALCON_build_request_field_78( )
{
	memset(Cr_Fld.CARD_AIP_VERIFICATION_XFLG,' ',1);
	sprintf(&message[689],"%-1s\n",Cr_Fld.CARD_AIP_VERIFICATION_XFLG);
}
void FALCON_build_request_field_79( )
{
	memset(Cr_Fld.CARD_AIP_RISK_XFLG,' ',1);
	sprintf(&message[690],"%-1s\n",Cr_Fld.CARD_AIP_RISK_XFLG);
}
void FALCON_build_request_field_80( )
{
	memset(Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG,' ',1);
	sprintf(&message[691],"%-1s\n",Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG);
}
void FALCON_build_request_field_81( )
{
	memset(Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG,' ',1);
	sprintf(&message[692],"%-1s\n",Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG);
}
void FALCON_build_request_field_82( )
{
	memset(Cr_Fld.TRANSACTION_DELINQUENT_CYC,' ',1);
	sprintf(&message[693],"%-1s\n",Cr_Fld.TRANSACTION_DELINQUENT_CYC);
}
void FALCON_build_request_field_83( )
{
	memset(Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT,' ',13);
	sprintf(&message[694],"%-13s\n",Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT);
}
void FALCON_build_request_field_84( )
{
	memset(Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT,' ',13);
	sprintf(&message[707],"%-13s\n",Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT);
}
void FALCON_build_request_field_85( )
{
	memset(Cr_Fld.MERCHANT_BALANCE_AMT,' ',13);
	sprintf(&message[720],"%-13s\n",Cr_Fld.MERCHANT_BALANCE_AMT);
}
void FALCON_build_request_field_86( )
{
	memset(Cr_Fld.CUSTOMER_POSTAL_XCD,' ',12);
	sprintf(&message[733],"%-12s\n",Cr_Fld.CUSTOMER_POSTAL_XCD);
}
void FALCON_build_request_field_87( )
{
	memset(Cr_Fld.CARD_MEDIA_TYPE_XCD,' ',1);
	sprintf(&message[745],"%-1s\n",Cr_Fld.CARD_MEDIA_TYPE_XCD);
}
void FALCON_build_request_field_88( )
{
	if (strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(Auth_Tx.TLF01_details.cvc == 'P')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '0' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M' ||
				Auth_Tx.TLF01_details.cvc == 'N')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '1' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'U')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = ' ' ;
		}
		else
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '9' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'C' ||
		    Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'D' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'K' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'L' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'M' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'N' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'P')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '1';
		}
		else if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'U')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '9';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0)
	{
		Cr_Fld.CVV2_PRESENT_XFLG[0] = ' ';
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		/*Not getting value*/
	}


	sprintf(&message[746],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
}
void FALCON_build_request_field_89( )
{
	if (strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(Auth_Tx.TLF01_details.cvc == 'P')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'P' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'M' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'N')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N' ;
		}
		else
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = ' ' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'C' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'K' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'M' )
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'M';
		}
		else if ( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'D' ||
				  Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'L' ||
				  Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'N')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N';
		}
		else if ( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'P')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'P';
		}
		else if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'S')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'S';
		}
		else if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'U')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'U';
		}
		else
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = ' ';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0)
	{
		Cr_Fld.CVV2_RESPONSE_XFLG[0] = ' ';
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		/* Not getting value*/
	}


	sprintf(&message[747],"%-1s\n",Cr_Fld.CVV2_RESPONSE_XFLG);
}
void FALCON_build_request_field_90( )
{
	/*****memset(Cr_Fld.AVS_RESPONSE_XCD,' ',1);*******/
	memset(Cr_Fld.AVS_RESPONSE_XCD,' ',1);
	sprintf(&message[748],"%-1s\n",Cr_Fld.AVS_RESPONSE_XCD);
	/***memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);*****/
}
void FALCON_build_request_field_91( )
{
	if(strlen(Cr_Fld.TRANSACTION_CATEGORY_XCD) == 0 || strcmp(Cr_Fld.TRANSACTION_CATEGORY_XCD, " ") == 0)
	strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);
	sprintf(&message[749],"%-1s\n",Cr_Fld.TRANSACTION_CATEGORY_XCD);
	/*Auth_Tx.TLF01_details.acquirer_id*/
	/******memset(Cr_Fld.ACQUIRER_XID,' ',12);*****/
}
void FALCON_build_request_field_92( )
{
	strncpy(Cr_Fld.ACQUIRER_XID,Auth_Tx.TLF01_details.acquiring_id,12);
	sprintf(&message[750],"%-12s\n",Cr_Fld.ACQUIRER_XID);
	/*If it's a local transactions*/
}
void FALCON_build_request_field_93( )
{
	if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5) == 0)
	{
		strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,"608",3);
	}
	else
	{
		if(isnum(Auth_Tx.country_code))
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.country_code,3);
		}
		else if (isnum(Auth_Tx.reconciliation_cur_code))
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
		}
		else if (isnum(Auth_Tx.TLF01_details.currency_code))
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);
		}
		else
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,"608",3);
		}
	}
	sprintf(&message[762],"%-3s\n",Cr_Fld.ACQUIRER_COUNTRY_XCD);
}
void FALCON_build_request_field_94( )
{
	if(strcmp(Auth_Tx.TLF01_details.terminal_id,"") != 0)
	{
		memcpy(Cr_Fld.TERMINAL_XID,Auth_Tx.TLF01_details.terminal_id,16);
		sprintf(&message[765],"%-16s\n",Cr_Fld.TERMINAL_XID);
	}
	else
	{
		memset(Cr_Fld.TERMINAL_XID,' ',16);
		sprintf(&message[765],"%-16s\n",Cr_Fld.TERMINAL_XID);
	}
}
void FALCON_build_request_field_95( )
{
	memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1' ||
			Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '7' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"N",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7' )
	   {
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
	   }
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,'U',1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.amex_pos_entry_mode[3],1);
		if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"X",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"5",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"Z",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"S",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"T",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"0",1) == 0 ||
			strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"9",1) == 0)
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "03" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "06" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "11" ,2)== 0 )
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'L' ;
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "01" ,2)== 0 )
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'U' ;
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "05" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "07" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "08" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "09" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "12" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "14" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "16" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "17" ,2)== 0)
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'I' ;
		}
		else if ( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "00" ,2)== 0)
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = ' ' ;
		}
		else
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'O' ;
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '3'  )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '9')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}

	sprintf(&message[781],"%-1s\n",Cr_Fld.TERMINAL_TYPE_XCD);
}

	/*********memcpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);*****************/
	/*Field 96 Terminal Entry capability */
void FALCON_build_request_field_96( )
{
	memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '8')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'D' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '9')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'K' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'Z' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ' ;
		}
		else
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
	   if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '5' ||
		  Auth_Tx.TLF01_details.product_codes[16].amount[10] == '8'	||
		  Auth_Tx.TLF01_details.product_codes[16].amount[10] == '9'	 )
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'D' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'K' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
			   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'Z' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ' ;
	   }
	   else
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
	   }
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"C",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"K",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"M",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M';
		}
		else if (Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1]  == '6')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C';
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ';
		}
		else
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"C",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '8')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"D",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"K",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"M",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);
		}
	}
	sprintf(&message[782],"%-1s\n",Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD);
}
void FALCON_build_request_field_97( )
{
	strncpy(Cr_Fld.TRANSACTION_POS_CONDITION_XCD,Auth_Tx.TLF01_details.pos_condition_code,2);

	sprintf(&message[783],"%-2s\n",Cr_Fld.TRANSACTION_POS_CONDITION_XCD);
	/***************memcpy(Cr_Fld.ATM_NETWORK_XID,,);*************/
}
void FALCON_build_request_field_98( )
{
	memcpy(Cr_Fld.ATM_NETWORK_XID," ",1);
	sprintf(&message[785],"%-1s\n",Cr_Fld.ATM_NETWORK_XID);
}
void FALCON_build_request_field_99( )
{
	memset(Cr_Fld.RESERVED_01,' ',1);
	sprintf(&message[786],"%-1s\n",Cr_Fld.RESERVED_01);
}
void FALCON_build_request_field_100( )
{
	memset(Cr_Fld.CHECK_NUM,' ',6);
	sprintf(&message[787],"%-6s\n",Cr_Fld.CHECK_NUM);
}
void FALCON_build_request_field_101( )
{
	memset(Cr_Fld.TERML_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[793],"%-10s\n",Cr_Fld.TERML_VERIFICATION_RESULTS_XCD);
}
void FALCON_build_request_field_102( )
{
	memset(Cr_Fld.CARD_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[803],"%-10s\n",Cr_Fld.CARD_VERIFICATION_RESULTS_XCD);
}
	/*********memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);**********/
void FALCON_build_request_field_103( )
{
	memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);
	sprintf(&message[813],"%-1s\n",Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG);
}
	/************memset(Cr_Fld.ATC_CARD_CNT,' ',5);***************/
void FALCON_build_request_field_104( )
{
	memset(Cr_Fld.ATC_CARD_CNT,' ',5);
	sprintf(&message[814],"%-5s\n",Cr_Fld.ATC_CARD_CNT);
}
	/*********memset(Cr_Fld.ATC_HOST_CNT,' ',5);**************/
void FALCON_build_request_field_105( )
{
	memset(Cr_Fld.ATC_HOST_CNT,' ',5);
	sprintf(&message[819],"%-5s\n",Cr_Fld.ATC_HOST_CNT);
}
void FALCON_build_request_field_106( )
{
	memset(Cr_Fld.RESERVED_03,' ',2);
	sprintf(&message[824],"%-2s\n",Cr_Fld.RESERVED_03);
}
void FALCON_build_request_field_107( )
{
	memset(Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD,' ',2);
	sprintf(&message[826],"%-2s\n",Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD);
}
void FALCON_build_request_field_108( )
{
	memset(Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD,' ',2);
	sprintf(&message[828],"%-2s\n",Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD);
}
void FALCON_build_request_field_109( )
{
	memset(Cr_Fld.RECURRING_AUTHORIZATION_XCD,' ',2);
	sprintf(&message[830],"%-2s\n",Cr_Fld.RECURRING_AUTHORIZATION_XCD);
}
void FALCON_build_request_field_110( )
{
	memset(Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT,' ',8);
	sprintf(&message[832],"%-8s\n",Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT);
}
void FALCON_build_request_field_111( )
{
	memset(Cr_Fld.CARD_ASSOCIATION_XFLG,' ',1);
	sprintf(&message[840],"%-1s\n",Cr_Fld.CARD_ASSOCIATION_XFLG);
}
void FALCON_build_request_field_112( )
{
	memset(Cr_Fld.CARD_INCENTIVE_XCD,' ',1);
	sprintf(&message[841],"%-1s\n",Cr_Fld.CARD_INCENTIVE_XCD);
}
void FALCON_build_request_field_113( )
{
	memset(Cr_Fld.CARD_STATUS_XCD,' ',2);
	sprintf(&message[842],"%-2s\n",Cr_Fld.CARD_STATUS_XCD);
}
void FALCON_build_request_field_114( )
{
	memset(Cr_Fld.CARD_STATUS_DT,' ',8);
	sprintf(&message[844],"%-8s\n",Cr_Fld.CARD_STATUS_DT);
}
	/********memset(Cr_Fld.PROCESSOR_REASON_XCD,' ',5);*****/
	/*****memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);**************/

void FALCON_build_request_field_115( )
{
		memset(Cr_Fld.PROCESSOR_REASON_XCD,' ',5);

	if(Falcon_is_Issuing_Transaction()== true)
	{
		if(Falcon_Is_Advice_or_Reversal_Transaction() == true)
		{
			if(true == Check_If_AMEX_Transaction())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[2], Auth_Tx.TLF01_details.product_codes[12].code,3);
			}

			else if( true == Check_If_DINERS_Transaction())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[2], Auth_Tx.TLF01_details.product_codes[12].code, 3);

			}
			else if( true == Check_If_CUP_Transaction() )
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
			else if( true == Check_If_MC_Transaction ())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
			else if(true == Check_If_VISA_Transaction())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
			else if(true == Check_If_JCB_Transaction ())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
		}
		else
		{
			memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);
		}
	}
	else
	{
		memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);
	}

	 sprintf(&message[852],"%-5s\n",Cr_Fld.PROCESSOR_REASON_XCD);
}
void FALCON_build_request_field_116( )
{
	if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,"stndin") == 0)
	{
		memcpy(Cr_Fld.TRANSACTION_ADVICE_XCD,"A",1);
		sprintf(&message[857],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
	else
	{
		memset(Cr_Fld.TRANSACTION_ADVICE_XCD,' ',1);
		sprintf(&message[857],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
}
void FALCON_build_request_field_117( )
{
	memset(Cr_Fld.MERCHANT_XID,' ',16);
	memcpy(Cr_Fld.MERCHANT_XID,Auth_Tx.TLF01_details.merchant_id,15);
	sprintf(&message[858],"%-16s\n",Cr_Fld.MERCHANT_XID);
}
void FALCON_build_request_field_118( )
{
	memset(Cr_Fld.CARD_ORDER_TYPE_XCD,' ',1);
	sprintf(&message[874],"%-1s\n",Cr_Fld.CARD_ORDER_TYPE_XCD);
}
void FALCON_build_request_field_119( )
{
	memset(Cr_Fld.TRANSACTION_CASHBACK_AMT,' ',13);
	sprintf(&message[875],"%-13s\n",Cr_Fld.TRANSACTION_CASHBACK_AMT);
}
	/***********memset(Cr_Fld.USER_DATA_6_NUM,' ',13);************/
	/*Field 120 ECI value*/
void FALCON_build_request_field_120( )
{
	memset(Cr_Fld.USER_DATA_6_NUM,' ',13);

	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		memcpy( bcd_eci,&Auth_Tx.int_net_facility_data[5],2);
		convert_to_ascii(bcd_eci,ascii_eci,2);
		memcpy( Auth_Tx.TLF01_details.profile,ascii_eci,2);
		memcpy(Cr_Fld.USER_DATA_6_NUM,Auth_Tx.TLF01_details.profile,2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0 ||
			strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	    == 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,&Auth_Tx.TLF01_details.profile,2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,"00",2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,&Auth_Tx.TLF01_details.product_codes[16].amount[1],2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)	== 0)
	{
		memset(Cr_Fld.USER_DATA_6_NUM,' ',13);
	}
	else
	{
		Cr_Fld.USER_DATA_6_NUM[0]= '2';
		strncpy(&Cr_Fld.USER_DATA_6_NUM[1],Auth_Tx.TLF01_details.profile,2);
	}


	sprintf(&message[888],"%-13s\n",Cr_Fld.USER_DATA_6_NUM);
	/*************memset(Cr_Fld.USER_DATA_7_STRG,' ',40);********/
}
	/* User data 7 filled with AMEX POS DATA - Auth_Tx.amex_pos_entry_mode*/
void FALCON_build_request_field_121( )
{
	memset(Cr_Fld.USER_DATA_7_STRG,' ',40);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.amex_pos_entry_mode,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA) == 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
			strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB) == 0)
	{
			strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,strlen(Auth_Tx.TLF01_details.product_codes[16].amount));
	}

	sprintf(&message[901],"%-40s\n",Cr_Fld.USER_DATA_7_STRG);
}
void FALCON_build_request_field_122( )
{
	memset(Cr_Fld.PAYMENT_INSTRUMENT_XID,' ',30);
	sprintf(&message[941],"%-30s\n",Cr_Fld.PAYMENT_INSTRUMENT_XID);
}
void FALCON_build_request_field_123( )
{
	memset(Cr_Fld.AVS_REQUEST_XCD,' ',1);
	sprintf(&message[971],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);
}
void FALCON_build_request_field_124( )
{
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG,' ',1);
	sprintf(&message[972],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG);
}
void FALCON_build_request_field_125( )
{
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG,' ',1);
	sprintf(&message[973],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG);
}
void FALCON_build_request_field_126( )
{
	memcpy(Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG,"0",1);
	sprintf(&message[974],"%-1s\n",Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG);
}
void FALCON_build_request_field_127( )
{
	memset(Cr_Fld.POS_TERMINAL_ATTEND_XFLG,' ',1);
	sprintf(&message[975],"%-1s\n",Cr_Fld.POS_TERMINAL_ATTEND_XFLG);
}
void FALCON_build_request_field_128( )
{
	memset(Cr_Fld.POS_OFF_PREMISES_XFLG,' ',1);
	sprintf(&message[976],"%-1s\n",Cr_Fld.POS_OFF_PREMISES_XFLG);
}
void FALCON_build_request_field_129( )
{
	memset(Cr_Fld.POS_CARD_CAPTURE_XFLG,' ',1);
	sprintf(&message[977],"%-1s\n",Cr_Fld.POS_CARD_CAPTURE_XFLG);
}
void FALCON_build_request_field_130( )
{
	memset(Cr_Fld.POS_SECURITY_XFLG,' ',1);
	sprintf(&message[978],"%-1s\n",Cr_Fld.POS_SECURITY_XFLG);
}
void FALCON_build_request_field_131( )
{
	memcpy(Cr_Fld.AUTHORIZATION_XID,Auth_Tx.TLF01_details.auth_number,6);
	sprintf(&message[979],"%-6s\n",Cr_Fld.AUTHORIZATION_XID);
}
void FALCON_build_request_field_132( )
{
	memset(Cr_Fld.USER_DATA_8_STRG,' ',10);

	memcpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode,4);
	sprintf(&message[985],"%-10s\n",Cr_Fld.USER_DATA_8_STRG);
}
void FALCON_build_request_field_133( )
{
	memset(Cr_Fld.USER_DATA_9_STRG,' ',10);
	sprintf(&message[995],"%-10s\n",Cr_Fld.USER_DATA_9_STRG);
}
void FALCON_build_request_field_134( )
{
	memset(Cr_Fld.USER_INDICATOR_5_XCD,' ',1);
	sprintf(&message[1005],"%-1s\n",Cr_Fld.USER_INDICATOR_5_XCD);
}
void FALCON_build_request_field_135( )
{
	memset(Cr_Fld.USER_INDICATOR_6_XCD,' ',1);
	sprintf(&message[1006],"%-1s\n",Cr_Fld.USER_INDICATOR_6_XCD);
}

	/*Field 136 Service code*/
void FALCON_build_request_field_136( )
{
	memset(Cr_Fld.USER_INDICATOR_7_XCD,' ',5);

	strncpy(Cr_Fld.USER_INDICATOR_7_XCD,Auth_Tx.TLF01_details.service_code,3);
	sprintf(&message[1007],"%-5s\n",Cr_Fld.USER_INDICATOR_7_XCD);
}
void FALCON_build_request_field_137( )
{
	memset(Cr_Fld.USER_INDICATOR_8_XCD,' ',5);
	if(Is_BDO_Pos_Acquiring_Transaction() == true)
	{
		if(FALCON_is_BDO_PAY_QR_Transaction() ==true )
		{
			strncpy(Cr_Fld.USER_INDICATOR_8_XCD,Auth_Tx.TLF01_details.nii+1,3);
		}
	}
	sprintf(&message[1012],"%-5s\n",Cr_Fld.USER_INDICATOR_8_XCD);
}
void FALCON_build_request_field_138( )
{
	memset(Cr_Fld.MODEL_CONTROL_1_XCD,' ',1);
	sprintf(&message[1017],"%-1s\n",Cr_Fld.MODEL_CONTROL_1_XCD);
}
void FALCON_build_request_field_139( )
{
	memset(Cr_Fld.MODEL_CONTROL_2_XCD,' ',1);
	sprintf(&message[1018],"%-1s\n",Cr_Fld.MODEL_CONTROL_2_XCD);
}
void FALCON_build_request_field_140( )
{
	memset(Cr_Fld.MODEL_CONTROL_3_XCD,' ',1);
	sprintf(&message[1019],"%-1s\n",Cr_Fld.MODEL_CONTROL_3_XCD);
}
void FALCON_build_request_field_141( )
{
	memset(Cr_Fld.MODEL_CONTROL_4_XCD,' ',1);
	sprintf(&message[1020],"%-1s\n",Cr_Fld.MODEL_CONTROL_4_XCD);
}
void FALCON_build_request_field_142( )
{
	if(strcmp(Auth_Tx.TLF01_details.response_code,"54") == 0)
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"I",1);
	}
	else
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"V",1);
	}

	sprintf(&message[1021],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);
}
void FALCON_build_request_field_143( )
{
	memcpy(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD," ",1);

	sprintf(&message[1022],"%s\n",Cr_Fld.AUTH_SECONDARY_VERIFY_XCD);
}
void FALCON_build_request_field_144( )
{
	strncpy(Cr_Fld.AUTHORIZATION_RESPONSE_XCD," ",1);
	sprintf(&message[1023],"%s",Cr_Fld.AUTHORIZATION_RESPONSE_XCD);
}
void FALCON_build_request_field_145( )
{
	strncpy(Cr_Fld.SYSTEM_TRACE_NUM,Auth_Tx.TLF01_details.sys_trace_audit_num,6);
	sprintf(&message[1024],"%s",Cr_Fld.SYSTEM_TRACE_NUM);
}

INT Check_If_CUP_Transaction(void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpimp",6))
	{
		if ((0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_1,CUP_MP_Bin_range_len_1))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_2,CUP_MP_Bin_range_len_2))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_3,CUP_MP_Bin_range_len_3))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_4,CUP_MP_Bin_range_len_4))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_5,CUP_MP_Bin_range_len_5)))
		{
			return true ;
		}
		else
		{
			return false;
		}
	}
	else
	{
	if (0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return true ;
	}
	else if (0 == strncmp(Auth_Tx.TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		if((true == Check_If_VISA_Transaction ()) ||
		   (true == Check_If_MC_Transaction()))
		{
			return true ;
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2);
			return true ;
		}
	}
/*check for VOICE Txn in GUI*/
	else if(strncmp(Auth_Tx.BIN01_details.card_family,CARD_CUP,2)==0)
	{
		return true ;
	}
	else
	{
		return false;
	}
}
}


INT Check_If_VISA_Transaction(void)
{
	if (Auth_Tx.TLF01_details.card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_MC_Transaction(void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,Auth_Tx.TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	Auth_Tx.TLF01_details.card_num[0]=='5')
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT Check_If_JCB_Transaction(void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   (Auth_Tx.TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT Check_If_AMEX_Transaction(void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '4') ||
		(Auth_Tx.TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_DINERS_Transaction(void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '0') ||
		(Auth_Tx.TLF01_details.card_num[1] == '6') ||
		(Auth_Tx.TLF01_details.card_num[1] == '8') ||
		(Auth_Tx.TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((Auth_Tx.TLF01_details.card_num[0]=='6') &&
			 ((Auth_Tx.TLF01_details.card_num[1] == '0') ||
			 (Auth_Tx.TLF01_details.card_num[1] == '4') ||
			 (Auth_Tx.TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_Fleet_Transaction(void)
{
	char fleet_card_series[5]   = "7128";
	char private_card_series[5]  = "7188";
	int  fllet_Private_series  = 4 ;


	if(	0 == strncmp(Auth_Tx.TLF01_details.card_num,
					 fleet_card_series,fllet_Private_series))
	{
		return true ;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.card_num,
						 private_card_series,fllet_Private_series))
	{
		return true ;
	}
	else
	{
		return false;
	}
}


/******************************************************************************
 *
 *  NAME:        FALCON_read_CUP_merchant_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Ramya
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_merchant_indicator( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMIDSECTION",     /* points to section name       */
             "CUP_MID_INDICATOR",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
   if(strlen(CUP_merchant_indicator)==0)
   {
	   strncpy( CUP_merchant_indicator,
			    CUP_MID_INDICATOR_DEFAULT,
			    CUP_MID_INDICATOR_LEN);/* Default Value*/
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:        FALCON_read_Workflow_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_Workflow_indicator( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   GetPrivateProfileString(
   							"FALCON",		/* points to section name*/
   							"WORKFLOW", 	/* points to key name*/
   							"", 		/* points to default string*/
							tmpstring,	/* points to destination buffer*/
   							sizeof(tmpstring) - 1,/* size of destination buffer*/
							filename );

   /* Convert entry from a string into a number. */
   strncpy( profile_str, tmpstring, strlen(tmpstring) );
   if(strlen(profile_str)==0)
   {
	   strncpy( profile_str,
			   WORKFLOW_DEFAULT_STRING,
			   WORKFLOW_DEFAULT_LEN);/* Default Value*/
   }
   return( ret_val );
}

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount, 13, strTemp) ; /* choice of 15 is arbitrary*/

	nStringLength = strlen (strTemp) ;
	/* Discard all characters after the decimal point, if present*/
	for (nIndex = 0; nIndex < nStringLength ; nIndex++)
	{
		if ('.' == strTemp[nIndex])
		{
			strTemp[nIndex] = 0 ;
			break ;
		}
	}

	strcpy (strAmount, strTemp) ;
	return 1 ;
} /* Convert_Amt_To_Auth_Tx_String_Format */

void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character)
{
   unsigned char strTemp[256] = {0} ;
   int len = 0 ;
   memset (strTemp, fill_character, sizeof strTemp) ;

   len = strlen (strAmount) ;
   strTemp [destination_array_size - len] = 0 ;
   strcat (strTemp, strAmount) ;
   strncpy (strAmount, strTemp,destination_array_size) ;
}
/******************************************************************************
 *
 *  NAME:         falcon_log_message
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
void falcon_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   falcon_Log_error_warning_to_File(text_message,sev,func,details);
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

INT falcon_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail)
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
    len = strlen(falcon_Erro_warning_Filename);
    path_len = strlen(falcon_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&falcon_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	falcon_create_Error_Filename();
    }
	if((fp = fopen(falcon_Erro_warning_Filename,"a+b"))==NULL)
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

void falcon_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( falcon_Erro_warning_Filename, falcon_error_warning_file_path );
    strcat( falcon_Erro_warning_Filename, falcon_module_error_warning_file_name );
	strcat( falcon_Erro_warning_Filename, ".EW." );
    strncat(falcon_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(falcon_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(falcon_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
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

INT get_FRT_timeout_for_Falcon( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "FALCON_REAL_TIME",     /* points to section name       */
             "TIMEOUT",  /* points to key name           */
             "10",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   t_secs =  atoi(tmpstr);
   if(t_secs == 0)
   {
	   t_secs = 10;
   }
   return( ret_val );
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

/******************************************************************************
 *
 *  NAME:         SET_TIMER
 *
 *  DESCRIPTION:  This procedure builds a timer message and sends it to the
 *                timer data server.
 *
 *  INPUTS:       lookup_key    - Timer key for referencing
 *                timer_length  - Number of seconds to set the timer
 *                TimerAppData2 - Flag used to not forward reversals to txcntl
 *                type_of_timer - Flag to determine if Auth_Tx should be sent
 *                AppName       - (Global)Application name to create queue name
 *
 *                Note about 'TimerAppData2' -> If 0200 txns timeout, the a
 *                reversal is automatically sent to the host.  The reversal
 *                response should not be forwarded to the acquiring application,
 *                i.e. dcpiso or ncvisa or ncjcb.
 *
 *                Its values: TIMER_REVERSAL
 *                            TIMER_FINANCIAL
 *                            TIMER_0800
 *                            TIMER_ECHO
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer )
{
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   BYTE     source_que[20]  = "";
   BYTE     buffer1[AUTH_TX_SIZE];
   CHAR     err_buf[100]="";
   INT      ret_val = false;


   /* Send the whole transaction record. */
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, "falconA" );

   sprintf(err_buf,"lookupkey while sending to ftimer : %s",lookup_key);
   falcon_log_message( 2, 1, err_buf, "set_timer",0 );
   /* Determine if this is a timer for a transaction to the host or not. */
   if ( type_of_timer == REQUEST_TIMER )
   {
      /* Build PTE message that will start the timer. */

      /* This type of timer stores Auth_Tx with the timer. */
      p_msg_set_timer = ptetimer_bld_start_timer_req(
                            lookup_key,
                            Auth_Tx.TLF01_details.primary_key.transaction_id,
                            TimerAppData2,
                            ftimer_que_name,
                            source_que,
                            (WORD)timer_length,
                            0,
                            (pBYTE)&buffer1,
                            Auth_Tx_Size );
   }
   else /* Echo Timer */
   {
      /* This type of timer does not stores Auth_Tx with the timer. */
      p_msg_set_timer = ptetimer_bld_start_timer_req(
                            lookup_key,
                            NULL,
                            NULL,
                            ftimer_que_name,
                            source_que,
                            (WORD)timer_length,
                            0,
                            NULL_PTR,
                            0 );
   }

   if ( NULL_PTR == p_msg_set_timer )
   {
      strcpy( err_buf, "Insufficient Memory to build timer message" );
   }
   else
   {
      ret_val = send_msg(p_msg_set_timer, ftimer_que_name, err_buf);
      free( p_msg_set_timer );
   }

   if ( false == ret_val )
	   falcon_log_message( 2, 2, err_buf, "set_timer",0 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         TRIM_TRAILING_SPACES
 *
 *  DESCRIPTION:  This function sets spaces to nulls, starting from the end
 *                of a string stepping backwards to the first non-space char.
 *
 *  INPUTS:       p_string - Character string to be trimmed
 *
 *  OUTPUTS:      p_string - Modified string
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void trim_trailing_spaces( pBYTE p_string )
{
   INT   i;
   INT   len;

   len = strlen( p_string );
   if ( len > 0 )
   {
      for ( i = len-1; (i>=0) && (p_string[i] == ' '); i-- )
         p_string[i] = 0x00;
   }

   return;
}

void set_tx_key_to_response()
{
   /* Make sure this is a request.  If it is not, then it is a response
    * and we need to make sure we return what was sent in.   Note: Anything
    * greater than AUTH_INVALID is a response, not a request.
    */
   if ( Auth_Tx.TLF01_details.tx_key < AUTH_INVALID )
      Auth_Tx.TLF01_details.tx_key += 100;

   return;
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG_TO_AUTHORIZER
 *
 *  DESCRIPTION:  This function sends a PTE message to the authorizer.  It
 *                will be either a request or a response.
 *
 *  INPUTS:       msgtype - Either MT_AUTH_REQUEST or MT_AUTH_RESPONSE
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT send_msg_to_authorizer( BYTE msgtype )
{
   INT   ret_val;
   CHAR  err_buf[100]={0};
   CHAR  Buffer[200] = {0};

   if((0 == strncmp(Auth_Tx.TLF01_details.card_num,"7",1)) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp",3)))
   {
	   ret_val = build_and_send_request_message( msgtype,
												0,
												0,
												(pBYTE)authorizer_que_name,
												(pBYTE)&Auth_Tx,
												Auth_Tx_Size,
												NULL_STR,
												err_buf   );
	}

   else if((0 == strncmp(Auth_Tx.TLF01_details.product_codes[2].quantity,"1",1)) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp",3)))
   {
	   if (MT_RT_RESPONSE_DECLINE == msgtype)
	   {
		   ret_val = build_and_send_request_message( MT_RT_RESPONSE_DECLINE,
												  	  0,
													  0,
													  (pBYTE)mp_pos_que_name,
													  (pBYTE)&Auth_Tx,
													  Auth_Tx_Size,
													  0,
													  NULL_STR,
													  err_buf   );
	   }
	   else
	   {
		   ret_val = build_and_send_request_message( MT_AUTH_REQUEST,
													 0,
													 0,
													 (pBYTE)mp_nclink_que_name,
													 (pBYTE)&Auth_Tx,
													 Auth_Tx_Size,
													 0,
													 NULL_STR,
													 err_buf   );
	   }

   }
   else
   {
		ret_val = build_and_send_request_message( msgtype,
													0,
													0,
													(pBYTE)authorizer_que_name,
													(pBYTE)&Auth_Tx,
													Auth_Tx_Size,
													0,
													NULL_STR,
													err_buf   );
   }

   if ( ret_val == false )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Unable to forward txn to authorizer, %s.  Error = %s",
               authorizer_que_name, err_buf );
      falcon_log_message( 1, 2, Buffer, "send_msg_to_authorizer",1 );
   }

   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         COPY_RESP_INTO_REQUEST
 *
 *  DESCRIPTION:  This procedure copies information that came from the host in
 *                a response into the Auth_Tx buffer.
 *
 *  INPUTS:       host_auth_tx - Transaction response record from host
 *
 *  OUTPUTS:      Auth_Tx - (Global) Txn record is updated with response info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void copy_resp_into_request( pAUTH_TX host_auth_tx )
{
   if ( AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key )
   {
      strcpy( Auth_Tx.TLF01_details.total_amount,
              host_auth_tx->TLF01_details.total_amount);
   }

   /* Copy balance info for Balance Inquiries and Cash Txns. */
   strcpy( Auth_Tx.current_balance,   host_auth_tx->current_balance   );
   strcpy( Auth_Tx.available_balance, host_auth_tx->available_balance );

   Auth_Tx.current_balance_sign   = host_auth_tx->current_balance_sign;
   Auth_Tx.available_balance_sign = host_auth_tx->available_balance_sign;

   /* Copy balances into these fields; ncvisa looks for them here. */
   strcpy( Auth_Tx.ACF01_details.cash_available, Auth_Tx.current_balance );
   strcpy( Auth_Tx.ACF01_details.cash_limit, Auth_Tx.available_balance );

   /* Copy balances into these fields for Equitable's proprietary reports. */
   strcpy( Auth_Tx.TLF01_details.outstanding_balance,
           host_auth_tx->TLF01_details.outstanding_balance );
   strcpy( Auth_Tx.TLF01_details.credit_limit,
           host_auth_tx->TLF01_details.credit_limit );

   /* Copy balance currency code. */
   strcpy( Auth_Tx.host2_src_fi_nbr, host_auth_tx->host2_src_fi_nbr );

   if ( host_auth_tx->TLF01_details.auth_number[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.auth_number,
              host_auth_tx->TLF01_details.auth_number );
   }

   if(strlen( host_auth_tx->TLF01_details.response_code)> 0)
   {
	   strcpy( Auth_Tx.TLF01_details.response_code,
			   host_auth_tx->TLF01_details.response_code );
   }

   if ( 0 != strcmp(Auth_Tx.TLF01_details.response_code,"00") )
   {
      strcpy( Auth_Tx.TLF01_details.response_text,
              host_auth_tx->TLF01_details.response_text );

      if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y' )
      {
         /* Txn is a Voice Auth. Copy response text to voice text field.*/
         strcpy( Auth_Tx.TLF01_details.voice_auth_text,
                 host_auth_tx->TLF01_details.voice_auth_text );
      }
   }

   strcpy( Auth_Tx.TLF01_details.host_finish_time,
           host_auth_tx->TLF01_details.host_finish_time );

   /* Copy VISA CPS information */
   if ( Auth_Tx.TLF01_details.visa_char_indicator != 0x00 )
   {
      Auth_Tx.TLF01_details.visa_char_indicator =
                                host_auth_tx->TLF01_details.visa_char_indicator;
   }

      strcpy( Auth_Tx.TLF01_details.visa_tran_id,
              host_auth_tx->TLF01_details.visa_tran_id );

   if ( Auth_Tx.TLF01_details.visa_validation_code[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.visa_validation_code,
              host_auth_tx->TLF01_details.visa_validation_code );
   }

   /* Copy Account Identification information. */
   strcpy( Auth_Tx.acct_id_1, host_auth_tx->acct_id_1 );
   strcpy( Auth_Tx.acct_id_2, host_auth_tx->acct_id_2 );

   /* Copy field 44 - ThoughtFocus Girija Y, Phani */
   memcpy(Auth_Tx.TLF01_details.product_codes[19].code,
	      host_auth_tx->TLF01_details.product_codes[19].code,
		  sizeof(host_auth_tx->TLF01_details.product_codes[19].code));

   /* CUP Response Code - used for Online Updates (1304/1314) */
   strcpy( Auth_Tx.accptor_bus_code, host_auth_tx->accptor_bus_code );
   /* TF Phani - For VOICE CA- Trasnctions should go as AUTH-
	During request we have changed to CA and changes back to CA_AUTH
   */
    if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y'  && Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;

	if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y'  && Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE)
	   Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL;

	/* Copy DE22 back to AUTH_TX amex_pos_entry_mode*/
	/*if(strlen(Auth_Tx.amex_pos_entry_mode)== 0)
	strcpy(Auth_Tx.amex_pos_entry_mode,host_auth_tx->amex_pos_entry_mode);*/
	/*Copy Resp buffer into AUTH TX*/
	/*Copy resp buffer and length into outgoing message*/
	if(atoi(host_auth_tx->resp_data_len) > 0 && atoi(host_auth_tx->resp_data_len) <  COMM_BUF_SZE )
	{
		memcpy(Auth_Tx.resp_data_len,host_auth_tx->resp_data_len,sizeof(host_auth_tx->resp_data_len));
		memcpy(Auth_Tx.resp_buffer,host_auth_tx->resp_buffer,sizeof(host_auth_tx->resp_buffer));
	}

	/*Store Settlement Date coming from HOST*/
	memcpy(Auth_Tx.TLF01_details.settlement_date,host_auth_tx->TLF01_details.settlement_date,sizeof(Auth_Tx.TLF01_details.settlement_date));

	/* Store transmission time coming from HOST.*/
	if(strlen(host_auth_tx->TLF01_details.transmission_timestamp)>0)
	{
		memcpy(Auth_Tx.TLF01_details.transmission_timestamp,host_auth_tx->TLF01_details.transmission_timestamp,sizeof(Auth_Tx.TLF01_details.transmission_timestamp));
	}
	printf("Transmission timestamp for STAN%s is %s \n",Auth_Tx.TLF01_details.sys_trace_audit_num,
															Auth_Tx.TLF01_details.transmission_timestamp);


	/* Copy EMV fields*/
	memcpy(Auth_Tx.EMV_details.future_field1,host_auth_tx->EMV_details.future_field1,
	sizeof(Auth_Tx.EMV_details.future_field1));

   return;
}

void process_clear_timer_reply_for_approve_response( pPTE_MSG p_msg_in )
{
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_msg_out         = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len;
	CHAR           app_data1[32];
	CHAR           app_data2[32];
	CHAR           lookup_key[LOOKUP_KEY_SIZE]="";
	CHAR           echo_key[LOOKUP_KEY_SIZE]  ="";
	CHAR           echo_req_key[LOOKUP_KEY_SIZE]  ="";
	CHAR           err_buf[100];
	AUTH_TX        host_auth_tx;
	INT			  len = 0;

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );
	timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
	app_data2, (pBYTE)&Auth_Tx,
	Auth_Tx_Size );

	/* Get auth_tx record that was chained to the 'clear timer' message.
	* This is the auth_tx record built from response data from the
	* host.
	*/
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	if ( data_len != Auth_Tx_Size )
	{
		sprintf( err_buf,
				"Did not receive chained host response data with timer reply. Key = %s",
				lookup_key );
		falcon_log_message( 2, 2, err_buf, "process_clear_timer_reply_for_approve_response",0 );
	}

	/* Get auth_tx record that was built from host response data. */
	memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
	memcpy( &host_auth_tx, p_data, Auth_Tx_Size );

	/* Move response info from host into Auth_Tx. */
	(void)copy_resp_into_request( &host_auth_tx );

	/* Indicate a Remote Authorization, not local. */
	strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

	/* Set tx_key to a 'response' value. */
	/* (void)set_tx_key_to_response();*/

	if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_APPROVED) )
	{
		sprintf( err_buf,
				"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
				Auth_Tx.TLF01_details.message_type,
				Auth_Tx.TLF01_details.processing_code);
		falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_approve_response",0 );
	}
}

void process_clear_timer_reply_for_decline_response( pPTE_MSG p_msg_in )
{
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_msg_out         = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len;
	CHAR           app_data1[32];
	CHAR           app_data2[32];
	CHAR           lookup_key[LOOKUP_KEY_SIZE]="";
	CHAR           echo_key[LOOKUP_KEY_SIZE]  ="";
	CHAR           echo_req_key[LOOKUP_KEY_SIZE]  ="";
	CHAR           err_buf[100];

	AUTH_TX        host_auth_tx;
	INT			  len = 0;

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );

	timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
	app_data2, (pBYTE)&Auth_Tx,
	Auth_Tx_Size );

	/* Get auth_tx record that was chained to the 'clear timer' message.
	* This is the auth_tx record built from response data from the
	* host.
	*/
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	if ( data_len != Auth_Tx_Size )
	{
		sprintf( err_buf,
				"Did not receive chained host response data with timer reply. Key = %s",
				lookup_key );
		falcon_log_message( 2, 2, err_buf, "process_clear_timer_reply_for_decline_response",0 );
	}

	/* Get auth_tx record that was built from host response data. */
	memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
	memcpy( &host_auth_tx, p_data, Auth_Tx_Size );

	/* Move response info from host into Auth_Tx. */
	(void)copy_resp_into_request( &host_auth_tx );

	/* Indicate a Remote Authorization, not local. */
	strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

	if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_DECLINE) )
	{
		sprintf( err_buf,
				"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
				Auth_Tx.TLF01_details.message_type,
				Auth_Tx.TLF01_details.processing_code);
		falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_decline_response",0 );
	}
}

void process_clear_timer_reply_for_timeout_response( pPTE_MSG p_msg_in )
{
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_msg_out         = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len;
	CHAR           app_data1[32];
	CHAR           app_data2[32];
	CHAR           lookup_key[LOOKUP_KEY_SIZE]="";
	CHAR           echo_key[LOOKUP_KEY_SIZE]  ="";
	CHAR           echo_req_key[LOOKUP_KEY_SIZE]  ="";
	CHAR           err_buf[100];
	AUTH_TX        host_auth_tx;
	INT			  len = 0;

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );
	timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
	   app_data2, (pBYTE)&Auth_Tx,
	   Auth_Tx_Size );

	/* Get auth_tx record that was chained to the 'clear timer' message.
	* This is the auth_tx record built from response data from the
	* host.
	*/
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	if ( data_len != Auth_Tx_Size )
	{
		sprintf( err_buf,
				"Did not receive chained host response data with timer reply. Key = %s",
				lookup_key );
		falcon_log_message( 2, 2, err_buf, "process_clear_timer_reply_for_timeout_response",0 );
	}

	/* Get auth_tx record that was built from host response data. */
	memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
	memcpy( &host_auth_tx, p_data, Auth_Tx_Size );

	/* Move response info from host into Auth_Tx. */
	(void)copy_resp_into_request( &host_auth_tx );

	/* Indicate a Remote Authorization, not local. */
	strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

	if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_TIMEOUT) )
	{
		sprintf( err_buf,
				"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
				Auth_Tx.TLF01_details.message_type,
				Auth_Tx.TLF01_details.processing_code);
		falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_timeout_response",0 );
	}
}

void process_clear_timer_reply_for_response( pPTE_MSG p_msg_in )
{
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len;
	CHAR           app_data1[32];
	CHAR           app_data2[32];
	CHAR           lookup_key[LOOKUP_KEY_SIZE]="";
	CHAR           err_buf[100];
	INT 		   retVal		= 0 ;
	CHAR  		   system_date[25] = {0};
	CHAR  		   yyyymmdd[25] = {0};
	CHAR		   temp_secs[11] = {0};
	BYTE		   buffer[MAX_DATASERVER_BUFFER_LENGTH] = {0};
	AUTH_TX        host_auth_tx;

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );

	timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
													app_data2, (pBYTE)&Auth_Tx,
														(Auth_Tx_Size) );

	/* Get auth_tx record that was chained to the 'clear timer' message.
	* This is the auth_tx record built from response data from the
	* host.
	*/
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	if ( data_len != FTLF01_SIZE )
	{
		sprintf( err_buf,
				"Did not receive chained host response data with timer reply. Key = %s",
				lookup_key );
		falcon_log_message( 2, 2, err_buf, "process_clear_timer_reply_for_response",0 );
	}

	/* Read FTLF01 Data from Timer reply message*/
	memset(&Ftlf01, 0x00, sizeof(Ftlf01));
	memcpy(&Ftlf01, p_data, sizeof(Ftlf01));

	p_data = p_data + FTLF01_SIZE;

	/* Get auth_tx record that was built from host response data. */
	memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
	memcpy( &host_auth_tx, &Auth_Tx, Auth_Tx_Size );

	/* Move response info from host into Auth_Tx. */
	(void)copy_resp_into_request( &host_auth_tx );

	/* Indicate a Remote Authorization, not local. */
	strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

	memset (p_buffer, 0x00, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof(Auth_Tx)) ;

	if (strlen(Auth_Tx.TLF01_details.system_date) < 8 || (validate_date_yyyymmdd(Auth_Tx.TLF01_details.system_date) == false))
	{
	   memset( yyyymmdd, 0x00, sizeof(yyyymmdd) );
	   format_date(yyyymmdd);
	   strncpy(Auth_Tx.TLF01_details.system_date, yyyymmdd, sizeof(Auth_Tx.TLF01_details.system_date)-1);
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}
	else
	{
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}

	strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);

	strncpy(temp_secs, &Auth_Tx.TLF01_details.date_yyyymmdd[4], 4);
	strncat(temp_secs, Auth_Tx.TLF01_details.time_hhmmss, 6);
	strncpy(Ftlf01.tran_req_sent_time, temp_secs, sizeof(Ftlf01.tran_req_sent_time)-1);
	Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] ='\0';

	if (ret_val == RT_SYSTEM)
	{

		strcpy(Ftlf01.response_text, "failed to write into ActiveMQ");
		strncpy(Ftlf01.tran_resp_recv_mode, "SF", 2);
		(void)insert_db_record( FTLF01_DATA );
		if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_TIMEOUT) )
		{
			sprintf( err_buf,
					"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
					Auth_Tx.TLF01_details.message_type,
					Auth_Tx.TLF01_details.processing_code);
			falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_response",0 );
		}

	}
	else
	{
		strncpy(Ftlf01.tran_resp_recv_mode, "RT", 2);

		strcpy (err_buf,"Real Time Response Mode") ;
		falcon_log_message(1,1,err_buf,"process_clear_timer_reply_for_response",1);

		/* validate the decision code in response( appr or decl) */
		retVal = Check_Rsp_Msg_for_approval();
		if(true == retVal)
		{
			strcpy(Ftlf01.response_text, "Approved response from FAI");
			(void)insert_db_record( FTLF01_DATA );
			if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_APPROVED) )
			{
				sprintf( err_buf,
						"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
						Auth_Tx.TLF01_details.message_type,
						Auth_Tx.TLF01_details.processing_code);
				falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_response",0 );
			}
		}
		else if (false == retVal)
		{
			strcpy(Ftlf01.response_text, "Declined response from FAI");
			(void)insert_db_record( FTLF01_DATA );
			if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_DECLINE) )
			{
				sprintf( err_buf,
						"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
						Auth_Tx.TLF01_details.message_type,
						Auth_Tx.TLF01_details.processing_code);
				falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_response",0 );
			}
		}
	}

}

void process_clear_timer_reply_for_no_response( pPTE_MSG p_msg_in )
{
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len;
	CHAR           app_data1[32];
	CHAR           app_data2[32];
	CHAR           lookup_key[LOOKUP_KEY_SIZE]={0};
	CHAR           err_buf[100];
	CHAR  system_date[25] = {0};
	CHAR	date_str [ 9] = {0};
	CHAR	time_str [ 7] = {0};
	CHAR	time_date[25] = {0};
	CHAR	temp_time[11] = {0};
	CHAR	temp_secs[11] = {0};
	CHAR 	temp_tran[21] = {0};
	CHAR 	strError[512] = {0} ;
	CHAR    yyyymmdd[25] = {0};
	INT 	time_diff	 = 0;
	AUTH_TX        host_auth_tx;
	BYTE           buffer2[AUTH_TX_SIZE]= {0};

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );
	timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
	   app_data2, (pBYTE)&Auth_Tx,
	   Auth_Tx_Size );

	/* Get auth_tx record that was chained to the 'clear timer' message.
	* This is the auth_tx record built from response data from the
	* host.
	*/
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	/*if ( data_len != Auth_Tx_Size )
	{
		sprintf( err_buf,
				"Did not receive chained host response data with timer reply. Key = %s",
				lookup_key );
		falcon_log_message( 2, 2, err_buf, "process_clear_timer_reply_for_no_response",0 );
	}*/

	/* Get auth_tx record that was built from host response data. */
	memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
	memcpy( &host_auth_tx, &Auth_Tx, Auth_Tx_Size );

	/* Move response info from host into Auth_Tx. */
	(void)copy_resp_into_request( &host_auth_tx );

	/* Indicate a Remote Authorization, not local. */
	strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

	memset(&Ftlf01, 0x00, sizeof(Ftlf01));
	/*Prepare FTL01 Transaction ID from Auth Tx*/
	strncpy(Ftlf01.tran_id, Auth_Tx.TLF01_details.primary_key.transaction_id, sizeof(Ftlf01.tran_id) -1);

	strcpy (strError,"Timeout Mode - no response from FAI") ;
	falcon_log_message(1,1,strError,"process_clear_timer_reply_for_no_response",1);

	if (strlen(Auth_Tx.TLF01_details.system_date) < 8 || (validate_date_yyyymmdd(Auth_Tx.TLF01_details.system_date) == false))
	{
	   memset( yyyymmdd, 0x00, sizeof(yyyymmdd) );
	   format_date(yyyymmdd);

	   strncpy(Auth_Tx.TLF01_details.system_date, yyyymmdd, sizeof(Auth_Tx.TLF01_details.system_date)-1);
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}
	else
	{
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}

	/*strncpy(Auth_Tx.TLF01_details.transmission_timestamp, yyyymmdd, sizeof(Auth_Tx.TLF01_details.system_date)-1);*/

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );
	strncpy(temp_time, &date_str[4], 4);/* MMDD */
	strncat(temp_time, time_str, 6);	/* hhmmss   */
	strncpy( Ftlf01.tran_resp_recv_time, temp_time, sizeof(Ftlf01.tran_resp_recv_time)-1);
	Ftlf01.tran_resp_recv_time[sizeof(Ftlf01.tran_resp_recv_time)-1] ='\0';

	/*strncpy(temp_secs, &time_str[4],2);
	time_diff  = atoi(temp_secs) -3;
	sprintf(temp_secs, "%02d",time_diff );
	strncpy(&temp_time[8], temp_secs, 2 );

	strncpy( Ftlf01.tran_req_sent_time, temp_time, sizeof(Ftlf01.tran_req_sent_time)-1);
	Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] ='\0';*/

	strncpy(temp_secs, &Auth_Tx.TLF01_details.date_yyyymmdd[4], 4);
	strncat(temp_secs, Auth_Tx.TLF01_details.time_hhmmss, 6);
	strncpy(Ftlf01.tran_req_sent_time, temp_secs, sizeof(Ftlf01.tran_req_sent_time)-1);
	Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] ='\0';

	strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);

	/*copy resp recv mode */
	strncpy(Ftlf01.tran_resp_recv_mode, "TO", 2);
	strcpy(Ftlf01.response_text, "timeout, no response from FAI");

	/* copy decision codes */
	strncpy(Ftlf01.decision_code_1_value, 0x00, sizeof(Ftlf01.decision_code_1_value)-1);
	strncpy(Ftlf01.decision_code_2_value, 0x00, sizeof(Ftlf01.decision_code_2_value)-1);
	strncpy(Ftlf01.decision_code_3_value, 0x00, sizeof(Ftlf01.decision_code_3_value)-1);
	strncpy(Ftlf01.decision_code_4_value, 0x00, sizeof(Ftlf01.decision_code_4_value)-1);
	strncpy(Ftlf01.decision_code_5_value, 0x00, sizeof(Ftlf01.decision_code_5_value)-1);
	strncpy(Ftlf01.decision_code_6_value, 0x00, sizeof(Ftlf01.decision_code_6_value)-1);
	strncpy(Ftlf01.decision_code_7_value, 0x00, sizeof(Ftlf01.decision_code_7_value)-1);
	strncpy(Ftlf01.decision_code_8_value, 0x00, sizeof(Ftlf01.decision_code_8_value)-1);
	strncpy(Ftlf01.decision_code_9_value, 0x00, sizeof(Ftlf01.decision_code_9_value)-1);
	strncpy(Ftlf01.decision_code_10_value, 0x00, sizeof(Ftlf01.decision_code_10_value)-1);

	(void)insert_db_record( FTLF01_DATA );

	if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_TIMEOUT) )
	{
		sprintf( err_buf,
				"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
				Auth_Tx.TLF01_details.message_type,
				Auth_Tx.TLF01_details.processing_code);
		falcon_log_message( 1, 3, err_buf, "process_clear_timer_reply_for_no_response",0 );
	}
}


void create_request_timer_key( pCHAR timer_key )
{
	   CHAR   unique_tran_str_id[21] = "";
	   BYTE   tx_key;
	   BYTE   error_buffer[100] = {0};
	   CHAR   transcn_id[21] = {0};
	   CHAR   temp_mid[16]    = {0};
	   CHAR   rrn[12] = {0};
	   CHAR   temp_unique_tran[15] = "";
	   INT    tid_len = 0;
	   INT    mid_len = 0;
	   INT    pan_len;
	   INT    unique_tran_str_id_len=0;

	   if (0 == strlen(Auth_Tx.TLF01_details.primary_key.transaction_id))
	   {
		   /* Create a unique transaction id for the new auth_tx record. */
		      memset(unique_tran_str_id, 0x00, sizeof(unique_tran_str_id));
		      ptetime_get_strtime(unique_tran_str_id);
		      strncpy( Auth_Tx.TLF01_details.tran_start_time, unique_tran_str_id,16 );

		      memset(Auth_Tx.TLF01_details.primary_key.transaction_id, 0x00,
		             sizeof(Auth_Tx.TLF01_details.primary_key.transaction_id));
		      tid_len= strlen(Auth_Tx.TLF01_details.terminal_id);
		      if(tid_len == 8)
		      {
		     		 memcpy(Auth_Tx.TLF01_details.primary_key.transaction_id,
		     				Auth_Tx.TLF01_details.terminal_id,
		             		sizeof(Auth_Tx.TLF01_details.terminal_id) - 1);
		      		strncat(Auth_Tx.TLF01_details.primary_key.transaction_id,
		              		unique_tran_str_id+2, 12);
		      }
		      else
		      {
		   		 /* We have to now prepare 20 digit unique Transaction ID.
		   		   unique_tran_str_id will be having 14 digit length
		   		   now we will make it as 20 by adding YYMMDD in the beginning of it.
		   		   20 byte will look like
		   		   YYMMDD"unique_tran_str_id"
		   		   for example if unique_tran_str_id is "1444752896.334" then it will become
		   		   YYMMDD1444752896.334
		   		   Based on ther terminal length we will copy data from new unique_tran_str_id
		   		 */
		   		 strncpy(temp_unique_tran,unique_tran_str_id,14);
		   		 memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));

		   		 strncpy(unique_tran_str_id,
		   				Auth_Tx.TLF01_details.date_yyyymmdd+2, 6); /* YYMMDD */
		   		 strncat (unique_tran_str_id,temp_unique_tran,14);

		   		 unique_tran_str_id_len= strlen(unique_tran_str_id);
		   		 memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
		   				Auth_Tx.TLF01_details.terminal_id, tid_len );
		   		 strncat(Auth_Tx.TLF01_details.primary_key.transaction_id,
		   				  unique_tran_str_id+tid_len,
		   				  (unique_tran_str_id_len-tid_len) );
		   		Auth_Tx.TLF01_details.primary_key.transaction_id[20]=0x00;

		   	 }
	   }

	   strncpy( transcn_id, Auth_Tx.TLF01_details.primary_key.transaction_id, TRNID_LEN_BEFOREDECIMAL);
	   strncpy(&transcn_id[TRNID_LEN_BEFOREDECIMAL], &Auth_Tx.TLF01_details.primary_key.transaction_id[TRNID_LEN_BEFOREDECIMAL + 1], TRNDID_AFTERDECIMAL );
	   strncat(transcn_id, "F",1);
	   memcpy( rrn, &Auth_Tx.TLF01_details.retrieval_ref_num, 12);

	   strcat( timer_key, transcn_id );
	   /*strcat( timer_key, rrn );*/

	   return;
}

void send_request_to_timerds()
{
	INT   ret_val;
	INT   seconds,buf_temp_len=0;
	CHAR  err_buf[100]  ="";
	CHAR  lookup_key[LOOKUP_KEY_SIZE]="";
	CHAR  date_str [ 9] = "";
	CHAR  time_str [ 7] = "";
	CHAR  time_date[25] = "";
	char  yyyymmdd[25] = {0};

	memset (&Ftlf01, 0x00, sizeof(Ftlf01));


	sprintf (err_buf,"Sending the request from Servicename:%s,pid = %d, tid = %d", ServiceName,getpid(), t_id) ;
	falcon_log_message(1,1,err_buf,"send_request_to_timerds",1);

	/* Set the request timer. */
	(void)create_request_timer_key( lookup_key );
	/* seconds = atoi(Ncf01_I.request_timer); */
	   seconds = t_secs;

	/* strcpy( TimerAppData2, timer_resp_flag ); */
	ret_val = set_timer( lookup_key, seconds, REQUEST_TIMER );
	if (false == ret_val)
	{
		sprintf( err_buf,	"Cannot process %s txn request to host. Unable to set timer.",
							Auth_Tx.TLF01_details.message_type );
		falcon_log_message(2,1,err_buf,"send_request_to_timerds",0);

	}

}


void PinnacleMsgHandler(pPTE_MSG p_msg_in)
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pBYTE			 p_data;
   pPTE_MSG			 p_cur_msg = 0 ;
   pPTE_MSG			 p_msg_out = 0 ;
   pPTE_MSG			 p_msg_ins  = NULL_PTR;
   pBYTE			 p_data_data = NULL_PTR;
   pBYTE			 pCurrent;

   BYTE     MessageType = 0 ;
   BYTE     ReturnCode = 0 ;
   CHAR     destination_que[7];
   CHAR     strTemp[256] = {0} ;
   INT      len=0;int dest_len = 0;
   BYTE     subtype1;
   BYTE     subtype2;
   CHAR		cmd[100];
   CHAR		bindir[256];  
   
   INT      msglen=0;
   CHAR     msgBuffer [2000] = {0} ;
   int 		tracing;
   char 	buffer[150]={0};
   BYTE           net_consec_tmouts_ctr[4];
   BYTE           active_txns_ctr[4];
   BYTE           current_state[2];
   BYTE           prev_src_station[4];
   BYTE           timer_result;
   CHAR           ErrorMsg[200] = "";
   CHAR           Buff[256];
   CHAR           ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   LONG           ret_code;
   INT            nIndex = 0 ;
   INT            ret_val;
  

   memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
    
   p_cur_msg = p_msg_in ;

	
   FALCON_read_Workflow_indicator();
   MessageType = ptemsg_get_msg_type (p_cur_msg) ;
    switch (MessageType)
   {

   /*case MT_GET_STATS:
   	send_transaction_statistics( p_msg_in );
   	break;*/

    case MT_AUTH_REQUEST:/*name it as online*/
 					RqMssg_Online(p_cur_msg);
 					msglen=strlen(message);
 					sprintf(msgBuffer,"Sending message in Online Mode msglen:(%d) ",msglen);
 					falcon_log_message(2,1,msgBuffer,"PinnacleMsgHandler",1);
 					/*send_request_to_timerds(); */
 					break;

    case MT_AUTH_RT_REQUEST:/*name it as realtime*/
 	   	   	   	    RqMssg_RealTime(p_cur_msg);
 					msglen=strlen(message);
 					/*sprintf(msgBuffer,"Msg to JMQ ==> msglen:%d message:%s",msglen,message);
 					falcon_log_message(2,1,msgBuffer,"PinnacleMsgHandler",0);*/
 					/*send the req to Ftimerds
 					send_request_to_timerds();*/
 					break;

	case MT_SYSTEM_REPLY:
	break ;

	case MT_TIMER_REPLY:
			/*PRINT("MT_TIMER_REPLY  \n" );*/
			/*falcon_log_message(2,1,"MT_TIMER_REPLY","PinnacleMsgHandler",0);*/
			/*if (PTEMSG_OK == ptemsg_get_result_code(p_cur_msg))
			{
			   subtype1 = ptemsg_get_msg_subtype1(p_cur_msg);
			   subtype2 = ptemsg_get_msg_subtype2(p_cur_msg);

			   if (ST1_TIMER_RT_APPROVED == subtype1)
			   {
				  process_clear_timer_reply_for_approve_response(p_cur_msg);
			   }
			   else if ((ST1_TIMER_RT_DECLINED == subtype1) ||
					   (ST1_TIMER_RT_DECLINED == subtype2))
				{
				   process_clear_timer_reply_for_decline_response(p_cur_msg);
				}
			   else if (ST1_TIMER_RT_TIMEOUT == subtype1)
			   {
				   process_clear_timer_reply_for_timeout_response(p_cur_msg);
			   }

			}
			else
			{
				falcon_log_message(2,1,"MT_TIMER_REPLY result code fail ","PinnacleMsgHandler",0 );
			}*/
			if (PTEMSG_OK == ptemsg_get_result_code(p_cur_msg))
			{
			   subtype1 = ptemsg_get_msg_subtype1(p_cur_msg);
			   subtype2 = ptemsg_get_msg_subtype2(p_cur_msg);

			   if (ST1_TIMER_START == subtype1)
			   {
				  /* do nothing */
				  falcon_log_message(2,1,"ST1_TIMER_START ","PinnacleMsgHandler",0 );

			   }
			   else
			   {
				   falcon_log_message(2,1,"ST1_TIMER_REPLY ","PinnacleMsgHandler",0 );
				   process_clear_timer_reply_for_response(p_cur_msg);
			   }
			}

			break;

	case MT_TIMER_TIMEOUT:
		/* timeout senario */

		process_clear_timer_reply_for_no_response(p_cur_msg);

	    break ;

	case MT_DB_REPLY:
			break;

     case MT_SYSTEM_REQUEST:
          ptesystm_system_msg_handler (p_msg_in);
          switch (ptemsg_get_msg_subtype1 (p_msg_in))
          {
          	  case ST1_SYS_PING	:		if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
										{
												/* When pinged, display application version on Monitor. */
												sprintf( strTemp,
															"Pinged -> Falcon Service: %s, version %s",
															ServiceName, Version );
												TxUtils_Send_Msg_To_Operator( 1, strTemp, 1, INFO_MSG,
																	"PinnacleMsgHandler",
																	 0, INFO_ERROR, 0, 0, 0 );
										}
										break ;
          	  case ST1_SYS_IPC_TRACE_ON : if ( ST1_SYS_IPC_TRACE_ON == ptemsg_get_msg_subtype1(p_msg_in) )
										{
											pteipc_trace_on     ();
											memset(&message,0,sizeof(message));
											memcpy(message,"100",3);
										}
							            break;

          	  case ST1_SYS_IPC_TRACE_OFF: if ( ST1_SYS_IPC_TRACE_OFF == ptemsg_get_msg_subtype1(p_msg_in) )
										{
											pteipc_trace_off    ();
											memset(&message,0,sizeof(message));
											memcpy(message,"101",3);
										}
										break;
          	  	  default:
          	  		  break;
          }

      break;
      case MT_GET_STATS:
	         (void)send_transaction_statistics( p_msg_in );
	         break;


      default:
          strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
          TxUtils_Send_Msg_To_Operator 
				  (1, strTemp,
					1, ERROR_MSG,"FalconHandler.c", 4, FATAL_ERROR,0, 0, 0) ;
		break ;
    }  /* switch msg_type */
}
INT send_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100]        ="";

   /* Gather the statistics */
   /*(void)get_txn_stats( &Txn_Counts, &Monitor_Stats ); */

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE,
                                              ST1_NONE,
                                              ST2_NONE,
                                              originator_queue,
                                              0,
                                              0,
                                              0,
                                              originator_info,
                                              err_msg );
   if ( false == ret_code  )
   {
      falcon_log_message( 1, 2, err_msg, "send_transaction_statistics",1 );
   }
   return( ret_code );
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
   /*printf("Falcon:Recived Signal Handler\n");*/
   memcpy(javaEndProcessSignalled,"1",1);
   sleep( 1 );
}
 

INT FalconHandler_initialize(CHAR* appName, CHAR* serName)
{
	INT ret_val = true;
	CHAR err_buf[100] = { 0 };
	CHAR tf_filename[256] = { 0 };
	CHAR tmpstr[256] = { 0 };
	/*long td_val;
	 td_val=pthread_self();

	 printf("\nIntialize:%ld\n",td_val);*/

	CHAR ini_info[MAX_INI_INFO_SIZE] = { 0 };
	CHAR filename[MAX_FILENAME_SIZE] = { 0 };
	CHAR tmpstring[MAX_INI_INFO_SIZE] = { 0 };

	GetAscendentConfigDirectory(tmpstr);
	sprintf(tf_filename, "%stf.ini", tmpstr);

	FALCON_read_CUP_merchant_indicator();
	falcon_get_error_warning_file_name_path();
	get_FRT_timeout_for_Falcon();

	/*get record length from ini file*/
	GetPrivateProfileString("FALCON", /* points to section name*/
	"WORKFLOW", /* points to key name*/
	"APC80", /* points to default string*/
	profile_str, /* points to destination buffer*/
	sizeof(profile_str) - 1,/* size of destination buffer*/
	tf_filename); /* points to initialization filename*/

	/* This peace of code is alredy there in FALCON_read_CUP_merchant_indicator function
	 GetPrivateProfileString(
	 "CUPMIDSECTION",
	 "CUP_MID_INDICATOR",
	 "0",
	 ini_info,
	 MAX_INI_INFO_SIZE - 1,
	 filename
	 );


	 memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	 strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	 strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
	 if(strlen(CUP_merchant_indicator)==0)
	 {
	 strncpy( CUP_merchant_indicator,
	 CUP_MID_INDICATOR_DEFAULT,
	 CUP_MID_INDICATOR_LEN);
	 }*/

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
	/*fp1 = fopen(fpath, "w");
	 fp2 = fopen(fpath1, "w");*/

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

	/*  Create a table to be used for shared memory between multiple
	 *  instances of this  Module.  It contains three fields:
	 *  # of consecutive timeouts, # of active transactions,
	 *  and the current state of the host.
	 */

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
		sprintf(strTemp, "Attempting to start %s, servcname %s, tid = %d, PID = %d", strAppName, ServiceName,t_id,getpid() );
		falcon_log_message(1,1,strTemp,"FalconHandler_initialize",1);

	}

	/**** ret_val = create_shared_mem_table( err_buf );*/

	sprintf(strTemp, "Started the FalconHandler Service: %s", ServiceName);
	LogEvent(strTemp, INFO_MSG);

	return INTIALIZE_QUEUES_NO_ERROR;
}


void falcon_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  falcon_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   falcon_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(falcon_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  App_Name,       	/* points to key name             */
								  "",  					/* Default string                 */
								  falcon_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(falcon_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  falcon_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(falcon_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(falcon_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(falcon_error_warning_file_path) > 0) &&
	  (strlen(falcon_module_error_warning_file_name)> 0))
   {
	   falcon_create_Error_Filename();
   }
}

CHAR* FalconHandler_sendRequestMessage(CHAR* srvNam)
{
	LONG lReturnCode = 0L;
	CHAR strTemp[256] = { 0 };
	pPTE_MSG p_msg = 0;
	INT err_ctr = 0;
	CHAR strBuffer[256] = { 0 };
	CHAR strVal[1] = { 0 };
	INT msglen1 = 0;
	CHAR fileinfo[256] = { 0 };
	CHAR ErrorMsg[100] = "";

	if (strcmp(javaEndProcessSignalled, "1") != 0) {
		memset(&message,0,sizeof(message));
		p_msg = pteipc_receive(application_que_name, control_que_name, 2,
				&lReturnCode);

		if (p_msg != NULL_PTR) {
			ptetime_get_timestamp(time_date);
			/*sprintf(strBuffer,"Transaction recieved with timestamp:%s",time_date);
			falcon_log_message(2,1,strBuffer,"FalconHandler_sendRequestMessage",0);*/
			count++;
			err_ctr = 0;
			PinnacleMsgHandler(p_msg);
			free(p_msg);
			return message;
		} else if (lReturnCode != QUE_ER_TIMEOUT) {

			pteipc_get_errormsg(lReturnCode, strTemp);
			sprintf(strBuffer,
					"Error on pteipc_receive OR Error in Receving Message %s , error code: %ld",
					strTemp, lReturnCode);
			LogEvent(strBuffer, ALERT_MSG);
			memcpy(message, strBuffer, sizeof(strBuffer));

			/* Inform Operators if service's queue is no longer readable. */
			if (++err_ctr > 10) {
				err_ctr = 0;
				sprintf(strBuffer, "%s. UNABLE TO ACCESS QUEUE!!!!",
						ServiceName);
				LogEvent(strBuffer, ALERT_MSG);

			}
		} else {

			memset(&message, 0, sizeof(message));

		}


	} else {
		pteipc_shutdown_multiple_instance_app();
		return javaEndProcessSignalled;
	}

}

void FalconHandler_receiveResponseMsg(char* ResponseMsg)
 {
	CHAR respbuffer[5000] = { 0 };
	CHAR respn_msg[5000] = { 0 };
	CHAR strTemp[256] = { 0 };
	INT msglen = 0;
	const char *str2;
	CHAR ErrorMsg[100] = "";

	if (strcmp(javaEndProcessSignalled, "1") != 0) {

		str2=ResponseMsg;
		if (str2 == NULL) {
			strcpy(strTemp,
					"receiveResponseMsg:Parameter passed from Java is NULL");
			LogEvent(strTemp, ERROR_MSG);
		}

		msglen = strlen(str2);

		sprintf(respbuffer, "msglen:%dResponseMsg:%s", msglen, str2);
		LogEvent(respbuffer, INFO_MSG);

	} else {
		/* Shutting down, need to change state before exiting Xipc. */
		pteipc_shutdown_multiple_instance_app();
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
	else
	{
		falcon_log_message(1,3,"FalconRT: not received response from FAI","Falcon_Send_Request_To_Service",1);
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

	sprintf(buffer,"FalconRT: Response Tranid+RRN (%.31s)",Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR);
	falcon_log_message(2,1,buffer,"Parse_Rsp_Msg",1);

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

 
void FalconHandler_terminate()
{

	CHAR strTemp[256] = { 0 };
	CHAR SerName[20] = { 0 };
	strcpy(SerName, "FalconHandler");
	sprintf(strTemp, "Shutting down the FalconHandler Service: %s", SerName);
	LogEvent(strTemp, INFO_MSG);

	/* Log out of XIPC
	 #ifdef _DEBUG*/
	pteipc_shutdown_multiple_instance_app();
	/* #endif*/
	MainProcessDone = 1;
}

INT FalconHandler_getLibFileVersion()
{
	CHAR ver_buf[5] = { 0 };
	sprintf(ver_buf, "Falcon_Version:%s", Version);
	LogEvent(ver_buf, INFO_MSG);
	return (1);

}

void Falcon_build_CRTRAN_field_31_MC()
{

	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		/*if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))*/

		if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_VISA()
{

	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
				/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}

		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{

				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_JCB()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_CUP()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	len_billing_currency_code = strlen(Auth_Tx.ch_billing_curr_code);
	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

			if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

			}
			else if((len_billing_currency_code > 0)&&
					((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
					(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3))))
			{
				if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
				}
				else
				{
					strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
				}
			}

			else if (len_billing_currency_code == 0)
			{
				if(strncmp(Auth_Tx.TLF01_details.currency_code,"608",3) == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
				else if ((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
						(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}

		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
		else if ((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}


}

void Falcon_build_CRTRAN_field_31_DINERS()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "nceqit", 5) ||
		   0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "neqit2", 5))
		{
			if(0 == strncmp(Auth_Tx.TLF01_details.currency_code,"608", 3))
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

			}
			else if(0 == strncmp(Auth_Tx.TLF01_details.currency_code,"840", 3))
			{
					if(len_billing_amount > 0 && billing_amount > 0)
					{
						temp_conv_amount = billing_amount *conv_rate;
						Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
						LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
						memset(last2_decimal,0x00,sizeof(last2_decimal));
						strncpy(last2_decimal,str_temp_amount+10,2);

						strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
						if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
						{

						}
						else
						{
							strncat(Cr_Fld.TRANSACTION_AMT,".",1);
						}
						strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
					}
					else
					{
						strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
					}

			}
		}

		else if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}

	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_AMEX()
{
	memset(Cr_Fld.TRANSACTION_AMT,0x00,sizeof(Cr_Fld.TRANSACTION_AMT));
	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

			len_settle_currency_code = strlen(Auth_Tx.settlement_curr_code);

			if(len_settle_currency_code == 0)
			{
				if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
				{
					strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
					strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

				}
				else if((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
						(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
					{
						if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
						}
						else
						{
							strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
						}
					}
			}

			else if(strcmp(Auth_Tx.settlement_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
			else if((strcmp(Auth_Tx.settlement_curr_code,"840") == 0)||
					(0 != strncmp(Auth_Tx.settlement_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}

	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 || billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}


}



void Falcon_build_CRTRAN_field_31_Fleet()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

INT FALCON_read_CUP_MP_Bin_range_len_1( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_1",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_1= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_2
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_2( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_2",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_2= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_3
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_3( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_3= atoi(tmpstring);
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_4
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_4( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_4",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_4= atoi(tmpstring);
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_5
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_5( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_5",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_5= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_1_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_1_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_1",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_1, tmpstring, CUP_MP_Bin_range_len_1 );
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_2_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_2_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_2",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_2, tmpstring, CUP_MP_Bin_range_len_2 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_3_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_3_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_3, tmpstring, CUP_MP_Bin_range_len_3 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_4_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_4_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_4",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_4, tmpstring, CUP_MP_Bin_range_len_4 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_5_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_5_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_5",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_5, tmpstring, CUP_MP_Bin_range_len_5 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_Read_BDO_PAY_QR_NII_value
 *
 *  DESCRIPTION:  This function reads the Network International Identifier value from
 *  			  the tf.ini file which will be used to identify whether an incoming
 *  			  transaction is a BDO Pay QR transaction or not.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Akshara Hanumesh
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_Read_BDO_PAY_QR_NII_value( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "FALCON",     /* points to section name       */
             "BDO_PAY_QR_NII_1",  /* points to key name           */
             "",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );


   strncpy(BDO_PAY_QR_NII_1, ini_info, strlen(ini_info));

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "FALCON",     /* points to section name       */
             "BDO_PAY_QR_NII_2",  /* points to key name           */
             "",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );


   strncpy(BDO_PAY_QR_NII_2, ini_info, strlen(ini_info));

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
                "FALCON",     /* points to section name       */
                "BDO_PAY_QR_NII_3",  /* points to key name           */
                "",                       		/* points to default string     */
                ini_info,                      /* points to destination buffer */
                MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
                filename                       /* points to ini filename       */
    );


    strncpy(BDO_PAY_QR_NII_3, ini_info, strlen(ini_info));

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                   "FALCON",     /* points to section name       */
                   "BDO_PAY_QR_NII_4",  /* points to key name           */
                   "",                       		/* points to default string     */
                   ini_info,                      /* points to destination buffer */
                   MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
                   filename                       /* points to ini filename       */
     );


     strncpy(BDO_PAY_QR_NII_4, ini_info, strlen(ini_info));

     memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
     rc = GetPrivateProfileString(
                      "FALCON",     /* points to section name       */
                      "BDO_PAY_QR_NII_5",  /* points to key name           */
                      "",                       		/* points to default string     */
                      ini_info,                      /* points to destination buffer */
                      MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
                      filename                       /* points to ini filename       */
      );


     strncpy(BDO_PAY_QR_NII_5, ini_info, strlen(ini_info));

   return( rc );
}

void create_request_timer_key_from_ResponseHeader( pCHAR timer_key )
{

	   strncpy(timer_key, Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, TRANSCN_ID_LEN);
	   strcat(timer_key, "F");

	   return;
}
/******************************************************************************
 *
 *  NAME:         INSERT_DB_RECORD
 *
 *  DESCRIPTION:  This function calls the data server to insert a record
 *                into a database table.
 *
 *  INPUTS:       data_type - Indicates which table to insert into
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT insert_db_record( BYTE data_type )
{
   INT    ret_val = true;
   INT    length;
   CHAR   db_table[6]  = "";
   CHAR   err_buf[100] = "";
   CHAR   Buffer[200];
   BYTE   db_buffer[AUTH_TX_SIZE + FTLF01_SIZE] = "";

   if ( data_type == FTLF01_DATA )
   {

         memcpy( db_buffer, &Auth_Tx, sizeof(Auth_Tx) );
         memcpy( db_buffer+Auth_Tx_Size, &Ftlf01, FTLF01_SIZE );
         length   = Auth_Tx_Size + FTLF01_SIZE;
         strcpy( db_table, "FTLF01" );
   }


      ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                                ST1_DB_INSERT,
                                                0,
												fconds_que_name,
                                                db_buffer,
                                                length,
                                                data_type,
                                                NULL_STR,
                                                err_buf   );
      if ( ret_val == false )
      {
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Unable to insert record into %s.  Error = %s",
                  db_table, err_buf );
         falcon_log_message( 1, 3, Buffer, "insert_db_record",1 );
      }


   return( ret_val );
}
INT clear_request_timer_for_timeout_response( pCHAR lookup_key )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val       = false;
   CHAR       source_que[30] = "";
   CHAR       err_buf[100]  = "";
   CHAR gmt_time[20] = {0};

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, "falconA" );

   sprintf(err_buf, "lookup key : %s", lookup_key);
   falcon_log_message( 2, 1, err_buf, "clear_request_timer_for_timeout_response",0 );

   /* Build PTEMSG to clear timer */
   p_timer_msg = ptetimer_bld_clear_timer_req_rt_timeout( lookup_key,
                                               ftimer_que_name,
                                               source_que);
   if ( NULL_PTR == p_timer_msg )
   {
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
    		  	  	  	  	  	  	  	  	  MT_DB_REQUEST,
											  ST1_DB_ECHO,
											  ST2_NONE,
                                              ftimer_que_name,
                                              application_que_name,
                                              (pBYTE)(&Auth_Tx),
                                              Auth_Tx_Size,
                                              0 );
      if ( NULL_PTR == p_chain_msg )
      {
         sprintf( err_buf, "Insufficient memory to chain clear timer message" );
         free( p_timer_msg );
      }
      else
      {
         /* Send clear timer request with chained data to data server.
          * Processing of this transaction will resume in function
          * process_clear_timer_reply when we receive the
          * response from timerds to our clear timer request.
          */
         ret_val = send_msg( p_chain_msg, ftimer_que_name, err_buf );
         free( p_chain_msg );
      }
   }

   if ( false == ret_val )
	   falcon_log_message( 2, 2, err_buf, "clear_request_timer_for_timeout_response",0 );

   return( ret_val );
}
INT clear_request_timer_for_decline_response( pCHAR lookup_key )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val       = false;
   CHAR       source_que[30]="";
   CHAR       err_buf[100]  ="";

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, "falconA" );

   sprintf(err_buf, "lookup key : %s", lookup_key);
   falcon_log_message( 2, 1, err_buf, "clear_request_timer_for_decline_response",0 );

   /* Build PTEMSG to clear timer */
   p_timer_msg = ptetimer_bld_clear_timer_req_rt_declined( lookup_key,
                                               ftimer_que_name,
                                               source_que);
   if ( NULL_PTR == p_timer_msg )
   {
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
    		  	  	  	  	  	  	  	  	  MT_DB_REQUEST,
											  ST1_DB_ECHO,
											  ST2_NONE,
											  ftimer_que_name,
                                              application_que_name,
                                              (pBYTE)(&Auth_Tx),
                                              Auth_Tx_Size,
                                              0 );
      if ( NULL_PTR == p_chain_msg )
      {
         sprintf( err_buf, "Insufficient memory to chain clear timer message" );
         free( p_timer_msg );
      }
      else
      {
         /* Send clear timer request with chained data to data server.
          * Processing of this transaction will resume in function
          * process_clear_timer_reply when we receive the
          * response from timerds to our clear timer request.
          */
         ret_val = send_msg( p_chain_msg, ftimer_que_name, err_buf );
         free( p_chain_msg );
      }
   }

   if ( false == ret_val )
	   falcon_log_message( 2, 2, err_buf, "clear_request_timer_for_decline_response",0 );

   return( ret_val );
}

INT clear_request_timer_for_approval_response( pCHAR lookup_key )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val       = false;
   CHAR       source_que[30]="";
   CHAR       err_buf[100]  ="";

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, "falconA" );

   sprintf(err_buf, "lookup key : %s", lookup_key);
   falcon_log_message( 2, 1, err_buf, "clear_request_timer_for_approval_response",0 );

   /* Build PTEMSG to clear timer */
   p_timer_msg = ptetimer_bld_clear_timer_req_rt_approved( lookup_key,
                                               ftimer_que_name,
                                               source_que);
   if ( NULL_PTR == p_timer_msg )
   {
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
    		  	  	  	  	  	  	  	  	  MT_DB_REQUEST,
											  ST1_DB_ECHO,
											  ST2_NONE,
                                              ftimer_que_name,
                                              application_que_name,
                                              (pBYTE)(&Auth_Tx),
                                              Auth_Tx_Size,
                                              0 );
      if ( NULL_PTR == p_chain_msg )
      {
         sprintf( err_buf, "Insufficient memory to chain clear timer message" );
         free( p_timer_msg );
      }
      else
      {
         /* Send clear timer request with chained data to data server.
          * Processing of this transaction will resume in function
          * process_clear_timer_reply when we receive the
          * response from timerds to our clear timer request.
          */
         ret_val = send_msg( p_chain_msg, ftimer_que_name, err_buf );
         free( p_chain_msg );
      }
   }

   if ( false == ret_val )
	   falcon_log_message( 2, 2, err_buf, "clear_request_timer_for_approval_response",0 );

   return( ret_val );
}

INT clear_request_timer_for_Received_Response( pCHAR lookup_key )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val       = false;
   CHAR       source_que[30]= {0};
   CHAR       err_buf[100]  = {0};

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, "falconA" );

   sprintf(err_buf, "lookup key : %s, pid = %d, tid = %d", lookup_key, getpid(), pthread_self());
   falcon_log_message( 2, 1, err_buf, "clear_request_timer_for_Received_Response",0 );

   /* Build PTEMSG to clear timer */
   p_timer_msg = ptetimer_bld_clear_timer_req( lookup_key,
                                               ftimer_que_name,
                                               source_que);
   if ( NULL_PTR == p_timer_msg )
   {
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
    		  	  	  	  	  	  	  	  	  MT_DB_REQUEST,
											  ST1_DB_ECHO,
											  ST2_NONE,
                                              ftimer_que_name,
                                              application_que_name,
                                              (pBYTE)(&Ftlf01),
											  FTLF01_SIZE,
                                              0 );
      if ( NULL_PTR == p_chain_msg )
      {
         sprintf( err_buf, "Insufficient memory to chain clear timer message" );
         free( p_timer_msg );
      }
      else
      {
         /* Send clear timer request with chained data to data server.
          * Processing of this transaction will resume in function
          * process_clear_timer_reply when we receive the
          * response from timerds to our clear timer request.
          */
         ret_val = send_msg( p_chain_msg, ftimer_que_name, err_buf );
         free( p_chain_msg );
      }
   }

   if ( false == ret_val )
	   falcon_log_message( 2, 2, err_buf, "clear_request_timer_for_Received_Response",0 );

   return( ret_val );
}

INT Check_Rsp_Msg_for_approval()
{
	 if(0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_1, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_2, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_3, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_4, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_5, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_6, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_7, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_8, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_9, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_10, RT_FAIL_D, FT_FAIL_CODE_LEN))
	 {
		 return false;
	 }

	 else if(0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_1, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_2, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_3, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_4, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_5, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_6, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_7, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_8, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_9, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_10, RT_FAIL_d, FT_FAIL_CODE_LEN))
	 {
		 return false;
	 }

	 else if(0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_1, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_2, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_3, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_4, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_5, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_6, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_7, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_8, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_9, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_10, RT_PASS_A, FT_APPROVED_CODE_LEN))
	 {
		return true;
	 }
	 else if(0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_1, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_2, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_3, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_4, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_5, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_6, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_7, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_8, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
		 	 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_9, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg.Scr_Rsp_Dat.DECISION_CODE_10, RT_PASS_a, FT_APPROVED_CODE_LEN))
	 {
		 return true;
	 }
	 else
	 {
		 /*default case: if decision codes are empty We will approve dit*/
		 return true;
	 }
}

BYTE Falcon_Send_Request_To_Service_Old (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	pPTE_MSG p_msg_timer_out = NULL_PTR;
	CHAR	 lookup_key[22] = {0};
	INT nBufferSize = 0 ;
	INT ret_val 	= 0 ;
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
	char  yyyymmdd[25] = {0};

	memset (p_buffer, 0x00, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof(Auth_Tx)) ;

	memset(&Ftlf01, 0x00, sizeof(Ftlf01));

	/* call the parse func */
	ret_val = Parse_Rsp_Msg(p_data);

	/* copy the transcn id */
	strncpy(temp_tran, Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, 16);
	strcat(temp_tran, ".");
	strncat(temp_tran, &Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR[16], 3);
	strncpy(Ftlf01.tran_id, temp_tran, sizeof(Ftlf01.tran_id)-1);

	if (strlen(Auth_Tx.TLF01_details.system_date) < 8 || (validate_date_yyyymmdd(Auth_Tx.TLF01_details.system_date) == false))
	{
	   memset( yyyymmdd, 0x00, sizeof(yyyymmdd) );
	   format_date(yyyymmdd);
	   strncpy(Auth_Tx.TLF01_details.system_date, yyyymmdd, sizeof(Auth_Tx.TLF01_details.system_date)-1);
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}
	else
	{
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}



   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */

	strncpy( transcn_id, Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, 19);

	/*strncpy(&transcn_id[16], &Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR[16], 3 );*/
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

	if (RT_RESPONSE == ret_val)
	{
		/* log msg */
		strcpy (strError,"Real Time Response Mode") ;
		falcon_log_message(1,3,strError,"Falcon_Send_Request_To_Service",1);

		strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);

		 /*copy resp recv mode */
		strncpy(Ftlf01.tran_resp_recv_mode, "RT", 2);
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

		/* validate the decision code in response( appr or decl) */
		retVal = Check_Rsp_Msg_for_approval();
		if(true == retVal)
		{
			/* insert FTLF01 table */
		    strcpy(Ftlf01.response_text, "Approved response from FAI");
			(void)insert_db_record( FTLF01_DATA );

			/* call timer approved */
			clear_request_timer_for_approval_response( lookup_key);

		}
		else if (false == retVal)
		{
			/* insert FTLF01 table */
			strcpy(Ftlf01.response_text, "Declined response from FAI");
			(void)insert_db_record( FTLF01_DATA );

			/* call timer decline */
			clear_request_timer_for_decline_response( lookup_key);
		}
	}
	else if (RT_TIMEOUT == ret_val)
	{
		/* log msg */
		strcpy (strError,"Timeout Mode - no response from FAI") ;
		falcon_log_message(1,3,strError,"Falcon_Send_Request_To_Service",1);

		strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);

		/*copy resp recv mode */
		strncpy(Ftlf01.tran_resp_recv_mode, "TO", 2);
		strcpy(Ftlf01.response_text, "timeout, no response from FAI");
		/* copy decision codes */
		strncpy(Ftlf01.decision_code_1_value, 0x00, sizeof(Ftlf01.decision_code_1_value)-1);
		strncpy(Ftlf01.decision_code_2_value, 0x00, sizeof(Ftlf01.decision_code_2_value)-1);
		strncpy(Ftlf01.decision_code_3_value, 0x00, sizeof(Ftlf01.decision_code_3_value)-1);
		strncpy(Ftlf01.decision_code_4_value, 0x00, sizeof(Ftlf01.decision_code_4_value)-1);
		strncpy(Ftlf01.decision_code_5_value, 0x00, sizeof(Ftlf01.decision_code_5_value)-1);
		strncpy(Ftlf01.decision_code_6_value, 0x00, sizeof(Ftlf01.decision_code_6_value)-1);
		strncpy(Ftlf01.decision_code_7_value, 0x00, sizeof(Ftlf01.decision_code_7_value)-1);
		strncpy(Ftlf01.decision_code_8_value, 0x00, sizeof(Ftlf01.decision_code_8_value)-1);
		strncpy(Ftlf01.decision_code_9_value, 0x00, sizeof(Ftlf01.decision_code_9_value)-1);
		strncpy(Ftlf01.decision_code_10_value, 0x00, sizeof(Ftlf01.decision_code_10_value)-1);

		(void)insert_db_record( FTLF01_DATA );

		clear_request_timer_for_timeout_response( lookup_key);

	}
	else if (RT_SYSTEM == ret_val)
	{
		/* log msg */
		strcpy (strError,"Falcon writes failed into ActiveMQ") ;
		falcon_log_message(1,3,strError,"Falcon_Send_Request_To_Service",1);

		strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);

		/*copy resp recv mode */
		strncpy(Ftlf01.tran_resp_recv_mode, "SF", 2);
		strcpy(Ftlf01.response_text, "Falcon writes failed into ActiveMQ");
		/* copy decision codes */
		strncpy(Ftlf01.decision_code_1_value, 0x00, sizeof(Ftlf01.decision_code_1_value)-1);
		strncpy(Ftlf01.decision_code_2_value, 0x00, sizeof(Ftlf01.decision_code_2_value)-1);
		strncpy(Ftlf01.decision_code_3_value, 0x00, sizeof(Ftlf01.decision_code_3_value)-1);
		strncpy(Ftlf01.decision_code_4_value, 0x00, sizeof(Ftlf01.decision_code_4_value)-1);
		strncpy(Ftlf01.decision_code_5_value, 0x00, sizeof(Ftlf01.decision_code_5_value)-1);
		strncpy(Ftlf01.decision_code_6_value, 0x00, sizeof(Ftlf01.decision_code_6_value)-1);
		strncpy(Ftlf01.decision_code_7_value, 0x00, sizeof(Ftlf01.decision_code_7_value)-1);
		strncpy(Ftlf01.decision_code_8_value, 0x00, sizeof(Ftlf01.decision_code_8_value)-1);
		strncpy(Ftlf01.decision_code_9_value, 0x00, sizeof(Ftlf01.decision_code_9_value)-1);
		strncpy(Ftlf01.decision_code_10_value, 0x00, sizeof(Ftlf01.decision_code_10_value)-1);

		(void)insert_db_record( FTLF01_DATA );

		clear_request_timer_for_timeout_response( lookup_key);

	}
	else
	{
		sprintf (strError,"%d Invalid Response type Received", ret_val) ;
		falcon_log_message(1,3,strError,"Falcon_Send_Request_To_Service",1);
	}

	return DEVICE_OK ;
} /* Send_Request_To_Service */


BYTE Falcon_Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
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

	retVal =	build_and_send_request_message( MT_AUTH_RESPONSE,
									0,
									0,
									(pBYTE)falcon_controller_que_name,
									(pBYTE)&buffer,
									(Auth_Tx_Size+sizeof(Rsp_msg)+1),
									0,
									NULL_STR,
									err_buf  );

	falcon_log_message(2,1,"FalconRT: Sending the response to falcon controller","Falcon_Send_Request_To_Service",1);

	return DEVICE_OK;

#if 0
	/* copy the transcn id */
	strncpy(temp_tran, Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, 16);
	strcat(temp_tran, ".");
	strncat(temp_tran, &Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR[16], 3);
	strncpy(Ftlf01.tran_id, temp_tran, sizeof(Ftlf01.tran_id)-1);


	sprintf (strError,"Recieved the response to Servicename:%s,pid: %d, tid: %d", ServiceName, getpid(), pthread_self() );
	falcon_log_message(1,1,strError,"Falcon_Send_Request_To_Service",1);

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
} /* Falcon_Send_Request_To_Service_New */




void RT_FALCON_build_request_field_1( )
{
	memcpy(Cr_Fld_RT.WORKFLOW_XCD,profile_str,16);
	sprintf(&message[101],"%-16s\n",Cr_Fld_RT.WORKFLOW_XCD);
}
void RT_FALCON_build_request_field_2( )
{
	memcpy(Cr_Fld_RT.RECORD_TYPE_XCD,"CRTRAN24",8);
	sprintf(&message[117],"%-8s\n",Cr_Fld_RT.RECORD_TYPE_XCD);
}
void RT_FALCON_build_request_field_3(  )
{
	memcpy(Cr_Fld_RT.DATA_SPECIFICATION_VERSION_XCD,"2.4",5);
	sprintf(&message[125],"%-5s\n",Cr_Fld_RT.DATA_SPECIFICATION_VERSION_XCD);
}
void RT_FALCON_build_request_field_4( )
{
	memcpy(Cr_Fld_RT.CLIENT_XID,"DT_BDO_CR",16);
	sprintf(&message[130],"%-16s\n",Cr_Fld_RT.CLIENT_XID);
}
void RT_FALCON_build_request_field_5( )
{
	memcpy(Cr_Fld_RT.RCD_CRT_DATE,date_str,8);
	sprintf(&message[146],"%-8s\n",Cr_Fld_RT.RCD_CRT_DATE);
}
void RT_FALCON_build_request_field_6( )
{
	memcpy(Cr_Fld_RT.RCD_CRT_TIME,time_str,6);
	sprintf(&message[154],"%-6s\n",Cr_Fld_RT.RCD_CRT_TIME);
}
void RT_FALCON_build_request_field_7( )
{
	memcpy(Cr_Fld_RT.RCD_CRT_MSCND,time_milliseconds,3);
	sprintf(&message[160],"%-3s\n",Cr_Fld_RT.RCD_CRT_MSCND);
}
void RT_FALCON_build_request_field_8( )
{
	memcpy(Cr_Fld_RT.GMT_OFFSET,"8.00",6);
	sprintf(&message[163],"%-6s\n",Cr_Fld_RT.GMT_OFFSET);
}
void RT_FALCON_build_request_field_9( )
{
	memcpy(Cr_Fld_RT.CUSTOMER_XID,Auth_Tx.TLF01_details.card_num,20);
	sprintf(&message[169],"%-20s\n",Cr_Fld_RT.CUSTOMER_XID);
}
void RT_FALCON_build_request_field_10( )
{
	memcpy(Cr_Fld_RT.ACCOUNT_REFERENCE_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[189],"%-40s\n",Cr_Fld_RT.ACCOUNT_REFERENCE_XID);
}
void RT_FALCON_build_request_field_11( )
{
	char temp_Tran_id[33]={0};
	static INT   falcon_tx_counter=0;
	memset(temp_Tran_id,' ',sizeof(temp_Tran_id)-1);
	sprintf(temp_Tran_id,"%-20s-CRT-%07d",Auth_Tx.TLF01_details.primary_key.transaction_id,falcon_tx_counter++);
	/*strncpy(temp_Tran_id,Auth_Tx.TLF01_details.primary_key.transaction_id,20);
	strcat(temp_Tran_id,"-CRT-");
	sprintf(&temp_Tran_id[25],"%07d",falcon_tx_counter++);*/
	strncpy(Cr_Fld_RT.EXTERNAL_TRANSACTION_XID,temp_Tran_id,32);
	sprintf(&message[229],"%-32s\n",Cr_Fld_RT.EXTERNAL_TRANSACTION_XID);
}
void RT_FALCON_build_request_field_12( )
{
	memcpy(Cr_Fld_RT.SCORE_CUSTOMER_ACCOUNT_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[261],"%-19s\n",Cr_Fld_RT.SCORE_CUSTOMER_ACCOUNT_XID);
}
void RT_FALCON_build_request_field_13( )
{
	memcpy(Cr_Fld_RT.AUTHORIZATION_POSTING_XCD,"A",1);
	sprintf(&message[280],"%-1s\n",Cr_Fld_RT.AUTHORIZATION_POSTING_XCD);
}
void RT_FALCON_build_request_field_14( )
{
	memset(Cr_Fld_RT.CARD_POSTAL_XCD,' ',9);
	sprintf(&message[281],"%-9s\n",Cr_Fld_RT.CARD_POSTAL_XCD);
}
void RT_FALCON_build_request_field_15( )
{
	memset(Cr_Fld_RT.CARD_COUNTRY_XCD,' ',3);
	sprintf(&message[290],"%-3s\n",Cr_Fld_RT.CARD_COUNTRY_XCD);
}
void RT_FALCON_build_request_field_16( )
{
	memset(Cr_Fld_RT.CARD_OPEN_DT,' ',8);
	sprintf(&message[293],"%-8s\n",Cr_Fld_RT.CARD_OPEN_DT);
}
void RT_FALCON_build_request_field_17( )
{
	memset(Cr_Fld_RT.CARD_ISSUE_DT,' ',8);
	sprintf(&message[301],"%-8s\n",Cr_Fld_RT.CARD_ISSUE_DT);
}
void RT_FALCON_build_request_field_18( )
{
	memset(Cr_Fld_RT.CARD_ISSUE_TYPE_XCD,' ',1);
	sprintf(&message[309],"%-1s\n",Cr_Fld_RT.CARD_ISSUE_TYPE_XCD);
}
void RT_FALCON_build_request_field_19( )
{
	memset(Cr_Fld_RT.ACCOUNT_EXPIRATION_DT,' ',8);
	sprintf(&message[310],"%-8s\n",Cr_Fld_RT.ACCOUNT_EXPIRATION_DT);
}

void RT_FALCON_build_request_field_20( )
{

	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[8]={0};

	strncpy(tem_month,&Auth_Tx.TLF01_details.exp_date[2],2);
	strncpy(temp_exp,"20",2);
	if(strlen(Auth_Tx.TLF01_details.exp_date) > 0)
	{
		strncat(temp_exp,Auth_Tx.TLF01_details.exp_date,4);
	}
	else
	{
		strncpy(temp_exp,Auth_Tx.TLF01_details.date_yyyymmdd,6);
		strncpy(tem_month,&Auth_Tx.TLF01_details.date_yyyymmdd[4],2);
	}
	if( strncmp(tem_month,"",2) == 0 || strncmp(tem_month,"  ",2) == 0 || strncmp(tem_month,"00",2) == 0)
	{
		strncpy(temp_exp,"        ",8);
	}
	else if(strncmp(tem_month,"02",2) == 0)
	{
		strncat(temp_exp,"28",2);
	}
	else
	{
		strncat(temp_exp,"30",2);
	}

	strncpy(Cr_Fld_RT.CARD_EXPIRATION_DT,temp_exp,8);
	sprintf(&message[318],"%-8s\n",Cr_Fld_RT.CARD_EXPIRATION_DT);
}
void RT_FALCON_build_request_field_21( )
{
	if(strcmp(Auth_Tx.ACF01_details.available,"") != 0)
	{
		char temp_avail_amt[11]="";
		strncpy(temp_avail_amt,&Auth_Tx.ACF01_details.available[2],10);
		sprintf(Cr_Fld_RT.PRIOR_CREDIT_LIMIT_AMT,"%07d.%02d",atoi(temp_avail_amt)/100, atoi(temp_avail_amt)%100);
		sprintf(&message[326],"%-10s\n",Cr_Fld_RT.PRIOR_CREDIT_LIMIT_AMT);
	}
	else
	{
		strcpy(Cr_Fld_RT.PRIOR_CREDIT_LIMIT_AMT,"0000000.00");
		sprintf(&message[326],"%-10s\n",Cr_Fld_RT.PRIOR_CREDIT_LIMIT_AMT);
	}
}
void RT_FALCON_build_request_field_22( )
{
	memset(Cr_Fld_RT.CARD_CREDIT_LIMIT_AMT,' ',10);
	sprintf(&message[336],"%-10s\n",Cr_Fld_RT.CARD_CREDIT_LIMIT_AMT);
}
void RT_FALCON_build_request_field_23( )
{
	memset(Cr_Fld_RT.CUSTOMER_GENDER_XCD,' ',1);
	sprintf(&message[346],"%-1s\n",Cr_Fld_RT.CUSTOMER_GENDER_XCD);
}
void RT_FALCON_build_request_field_24( )
{
	memset(Cr_Fld_RT.CUSTOMER_BIRTH_DT,' ',8);
	sprintf(&message[347],"%-8s\n",Cr_Fld_RT.CUSTOMER_BIRTH_DT);
}
void RT_FALCON_build_request_field_25( )
{
	memset(Cr_Fld_RT.CUSTOMER_CARD_CNT,' ',3);
	sprintf(&message[355],"%-3s\n",Cr_Fld_RT.CUSTOMER_CARD_CNT);
}
void RT_FALCON_build_request_field_26( )
{
	memset(Cr_Fld_RT.INCOME_AMT,' ',10);
	sprintf(&message[358],"%-10s\n",Cr_Fld_RT.INCOME_AMT);
}
void RT_FALCON_build_request_field_27( )
{
	memset(Cr_Fld_RT.CARD_TYPE_XCD,' ',1);
	sprintf(&message[368],"%-1s\n",Cr_Fld_RT.CARD_TYPE_XCD);
}
void RT_FALCON_build_request_field_28( )
{
	memset(Cr_Fld_RT.PRIMARY_CARD_USE_XCD,' ',1);
	sprintf(&message[369],"%-1s\n",Cr_Fld_RT.PRIMARY_CARD_USE_XCD);
}
void RT_FALCON_build_request_field_29( )
{
	memcpy(Cr_Fld_RT.TRANSACTION_DATE,Auth_Tx.TLF01_details.date_yyyymmdd,8);
	sprintf(&message[370],"%-8s\n",Cr_Fld_RT.TRANSACTION_DATE);
}
void RT_FALCON_build_request_field_30( )
{
	memcpy(Cr_Fld_RT.TRANSACTION_TIME,Auth_Tx.TLF01_details.time_hhmmss,6);
	sprintf(&message[378],"%-6s\n",Cr_Fld_RT.TRANSACTION_TIME);
}

void RT_FALCON_build_request_field_31( )
{
    if (Check_If_MC_Transaction ()    == true)
	{
    	RT_Falcon_build_CRTRAN_field_31_MC();
	}
    else if (Check_If_VISA_Transaction ()    == true)
	{
    	RT_Falcon_build_CRTRAN_field_31_VISA();
	}
    else if (Check_If_CUP_Transaction ()    == true)
	{
    	RT_Falcon_build_CRTRAN_field_31_CUP();
	}
    else if (Check_If_JCB_Transaction ()    == true)
	{
    	RT_Falcon_build_CRTRAN_field_31_JCB();
	}
    else if (Check_If_DINERS_Transaction ()    == true)
	{
    	RT_Falcon_build_CRTRAN_field_31_DINERS();
	}
    else if (Check_If_AMEX_Transaction ()    == true)
	{

    	len_billing_amount = strlen(Auth_Tx.TLF01_details.settlement_total);

			if(len_billing_amount == 0)
			{
				billing_amount = atoi(Auth_Tx.reconciliation_amount);
				if(billing_amount > 0)
				{
					len_billing_amount = strlen(Auth_Tx.reconciliation_amount);
					strncpy(amount_digit,Auth_Tx.reconciliation_amount,10);
					strncpy(last2_decimal,Auth_Tx.reconciliation_amount+10,2);
					/*billing_amount = (strtod((char *)amount_digit,0));*/
					billing_amount = (strtod(Auth_Tx.reconciliation_amount, 0));
				}
				else
				{
					strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
					strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
					/*billing_amount = (strtod(amount_digit,0));*/
					billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
				}
			}
			else
			{
				  strncpy(amount_digit,Auth_Tx.TLF01_details.settlement_total,10);
				  strncpy(last2_decimal,Auth_Tx.TLF01_details.settlement_total+10,2);
				  /*billing_amount = (strtod(amount_digit,0));*/
				  billing_amount = (strtod(Auth_Tx.TLF01_details.settlement_total, 0));
			}

    	RT_Falcon_build_CRTRAN_field_31_AMEX();
	}
    else if (Check_If_Fleet_Transaction ()    == true)
	{
    	RT_Falcon_build_CRTRAN_field_31_Fleet();
	}
    else
    {
    	strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
    }

    sprintf(&message[384],"%-13s\n",Cr_Fld_RT.TRANSACTION_AMT);

}

void RT_FALCON_build_request_field_32( )
{
	strncpy(Cr_Fld_RT.TRANSACTION_ISO_CURRENCY_XCD,"608",3);
	sprintf(&message[397],"%-3s\n",Cr_Fld_RT.TRANSACTION_ISO_CURRENCY_XCD);
}

void RT_FALCON_build_request_field_33( )
{
	char temp_iso_ration_value[13] = {0};

	strncpy(temp_iso_ration_value,"00000",5);
	strncat(temp_iso_ration_value,temp_iso_ration,8);
	strncpy(Cr_Fld_RT.TRAN_ISO_CURRENCY_CNVRSN_RATIO,temp_iso_ration_value,13);
	sprintf(&message[400],"%-13s\n",Cr_Fld_RT.TRAN_ISO_CURRENCY_CNVRSN_RATIO);
}


	/*
	Pls. map:
	A = 00, 10, 11, 32, 85
	D = 03, 05, 06, 12, 13, 14, 19, 20, 30, 33, 39, 42, 44, 51, 52, 53, 54, 55, 56, 57, 58, 61, 63, 64, 82, 92, N7
	P = 04, 07, 41, 43
	R = 01, 02, 08,34, 35, 36, 37, 38, 60, 62, 65, 66, 75,
	Else �BLANK�
	*/
void RT_FALCON_build_request_field_34( )
{
	if( strncmp(Auth_Tx.TLF01_details.response_code,"00",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"10",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"11",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"32",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"85",2)== 0 )
	{
		strncpy(Cr_Fld_RT.DECISION_XCD,"A",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"03",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"05",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"06",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"12",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"13",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"14",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"19",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"20",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"30",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"33",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"39",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"42",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"51",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"52",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"53",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"54",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"55",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"56",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"57",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"58",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"61",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"63",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"64",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"82",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"92",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"N7",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"44",2)== 0)
	{
			strncpy(Cr_Fld_RT.DECISION_XCD,"D",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"04",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"07",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"41",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"43",2)== 0	)
	{
		strncpy(Cr_Fld_RT.DECISION_XCD,"P",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"01",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"02",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"08",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"34",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"35",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"36",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"37",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"38",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"60",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"62",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"65",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"66",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"75",2)== 0	)
	{
		strncpy(Cr_Fld_RT.DECISION_XCD,"R",1);
	}
	else
	{
		strncpy(Cr_Fld_RT.DECISION_XCD," ",1);
	}

	sprintf(&message[413],"%-1s\n",Cr_Fld_RT.DECISION_XCD);
}
void RT_FALCON_build_request_field_35()
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_SALE          || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE        || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"M",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE ||
		    Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE)
	{/* CASH advance*/
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"C",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE || Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
	{/*Reversal*/
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"V",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
	{/*VOID */
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"X",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)
	{/*Pre AUTH*/
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"P",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
	{/*Offline sale*/
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"D",1);
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_TYPE_XCD," ",1);
	}

	sprintf(&message[414],"%-1s\n",Cr_Fld_RT.TRANSACTION_TYPE_XCD);
}
void RT_FALCON_build_request_field_36()
{
	memcpy(Cr_Fld_RT.MERCHANT_CATEGORY_XCD,Auth_Tx.TLF01_details.category_code,4);
	sprintf(&message[415],"%-4s\n",Cr_Fld_RT.MERCHANT_CATEGORY_XCD);
}/*******memcpy(Cr_Fld_RT.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);*****/
void RT_FALCON_build_request_field_37()
{
	if(strncmp(Auth_Tx.MCF01_details.zip_code,"9999",4)== 0)
	{
		memset(Cr_Fld_RT.MERCHANT_POSTAL_XCD,' ',9);
	}
	else
	{
		strncpy(Cr_Fld_RT.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);
	}
	sprintf(&message[419],"%-9s\n",Cr_Fld_RT.MERCHANT_POSTAL_XCD);
}
void RT_FALCON_build_request_field_38()
{
	if(strlen(Auth_Tx.MCF01_details.primary_key.merchant_id)> 0)
	{
		strncpy(Cr_Fld_RT.MERCHANT_COUNTRY_XCD,"608",3);
	}
	else if(strlen(Auth_Tx.reconciliation_cur_code)> 0)/*reconciliation_cur_code*/
	{
		strncpy(Cr_Fld_RT.MERCHANT_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
	}
	else if(strlen(Auth_Tx.TLF01_details.num_dbsales)> 0)/*nfi_seq_nbr*/
	{
		strncpy(Cr_Fld_RT.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.num_dbsales,3);
	}
	else
		strncpy(Cr_Fld_RT.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);

	sprintf(&message[428],"%-3s\n",Cr_Fld_RT.MERCHANT_COUNTRY_XCD);
}
void RT_FALCON_build_request_field_39()
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(strcmp(Auth_Tx.TLF01_details.response_code, "00") == 0)
		{
			memcpy(Cr_Fld_RT.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
		else
		{
			memcpy(Cr_Fld_RT.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}

	}
	else if(strncmp(Cr_Fld_RT.TRANSACTION_TYPE_XCD,"C",1)== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.response_code, "55" ,2) == 0)
		{
			memcpy(Cr_Fld_RT.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}
		else
		{
				memcpy(Cr_Fld_RT.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
	}
	else
	{
			memcpy(Cr_Fld_RT.TRANSACTION_PIN_VERIFY_XCD," ",1);
	}

	sprintf(&message[431],"%-1s\n",Cr_Fld_RT.TRANSACTION_PIN_VERIFY_XCD);
}
/*switch(Auth_Tx.TLF01_details.cvc)
{
case 'Y': memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"I",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld_RT.CVV_VERIFY_XCD);
		  break;
case 'N': memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"I",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld_RT.CVV_VERIFY_XCD);
		  break;
case 'M': memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"V",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld_RT.CVV_VERIFY_XCD);
		  break;
default:memset(Cr_Fld_RT.CVV_VERIFY_XCD,' ',1);
		sprintf(&message[400],"%-1s\n",Cr_Fld_RT.CVV_VERIFY_XCD);
		break;
}*/
void RT_FALCON_build_request_field_40()
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		/* mapping variable need to check*/
		if(Auth_Tx.TLF01_details.cvc == 'Y' || Auth_Tx.TLF01_details.cvc == 'N' || Auth_Tx.TLF01_details.cvc == 'E' )
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"I",1);
			memcpy(Cr_Fld_RT.CVV2_RESPONSE_XFLG,"N",1);
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"V",1);
			memcpy(Cr_Fld_RT.CVV2_RESPONSE_XFLG,"M",1);
		}
		else
		{
			memset(Cr_Fld_RT.CVV_VERIFY_XCD,' ',1);
			memset(Cr_Fld_RT.CVV2_RESPONSE_XFLG,' ',1);
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		/* mapping variable need to check*/

		if(Auth_Tx.TLF01_details.cvc == '1')
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"I",1);
			memcpy(Cr_Fld_RT.CVV2_RESPONSE_XFLG,"N",1);
		}
		else if(Auth_Tx.TLF01_details.cvc == '2' || Auth_Tx.TLF01_details.cvc == '3' )
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"V",1);
			memcpy(Cr_Fld_RT.CVV2_RESPONSE_XFLG,"M",1);
		}
		else
		{
			memset(Cr_Fld_RT.CVV_VERIFY_XCD,' ',1);
			memset(Cr_Fld_RT.CVV2_RESPONSE_XFLG,' ',1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if (Auth_Tx.future_use_2[22] == '1' || Auth_Tx.future_use_2[23] == '1')
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"V",1);
		}
		else if(Auth_Tx.future_use_2[22] == '2' || Auth_Tx.future_use_2[23] == '2')
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"I",1);
		}
		else if (Auth_Tx.future_use_2[22] == '3' || Auth_Tx.future_use_2[23] == '3'||
				 Auth_Tx.future_use_2[22] == ' ' || Auth_Tx.future_use_2[23] == ' ')
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD," ",1);
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if( strncmp(&Auth_Tx.acct_id_1[2],"001",3) == 0 ||
		    strncmp(&Auth_Tx.acct_id_1[2],"016",3) == 0	)
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"V",1);
		}
		else if( strncmp(&Auth_Tx.acct_id_1[2],"002",3) == 0 ||
			     strncmp(&Auth_Tx.acct_id_1[2],"017",3) == 0 )
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD,"I",1);
		}
		else
		{
			memcpy(Cr_Fld_RT.CVV_VERIFY_XCD," ",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0 ||
			strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		memset(Cr_Fld_RT.CVV_VERIFY_XCD,' ',1);
	}
	else
	{
		memset(Cr_Fld_RT.CVV_VERIFY_XCD,' ',1);
	}

	 sprintf(&message[432],"%-1s\n",Cr_Fld_RT.CVV_VERIFY_XCD);
}

/*	Please refer to POS netry mode mapping doc : # */
void RT_FALCON_build_request_field_41()
{
	char temp_pos_entry_mode[3]={0};
	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)== 3)
	{
		strncpy(temp_pos_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,2);
	}
	else
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}

	if((Check_If_AMEX_Transaction())== true)
	{
		Falcon_AMEX_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_DINERS_Transaction())== true)
	{
		Falcon_DINERS_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_CUP_Transaction())== true)
	{
		Falcon_CUP_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_JCB_Transaction()) == true)
	{
		Falcon_JCB_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_MC_Transaction())== true)
	{
		Falcon_MC_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_VISA_Transaction())== true)
	{
		Falcon_VISA_posEntryMode_F41(temp_pos_entry_mode);
	}

	if(((strncmp(temp_pos_entry_mode,"02",2) == 0  ||
			  strncmp(temp_pos_entry_mode,"90",2) == 0) &&
			  strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0 ))
	{ /* POS 02 or 90 + Field 60.2.2 = 5 or 6 + Field 60.2.3 = 2 */
		if( strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"052",3)== 0 ||
			(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"062",3)== 0))
		{
			strncpy(temp_pos_entry_mode,"80",2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[10],"4",1)== 0)
		{
			strncpy(temp_pos_entry_mode,"91",2);
		}
		else
		{
			strncpy(temp_pos_entry_mode,"90",2);
		}
	}
	else if( strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
	{
		strncpy(temp_pos_entry_mode,"90",2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"092",3) == 0  ||
			strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0092",4) == 0 )
	{
			strncpy(temp_pos_entry_mode,"79",2);
	}
	else if((strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)&&
			(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0) )
	{
		strncpy(temp_pos_entry_mode,"91",2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3) == 0 )
	{
		strncpy(temp_pos_entry_mode,"80",2);
	}
	else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0 )
	{
		strncpy(temp_pos_entry_mode,"80",2);
	}

	if(FALCON_is_BDO_PAY_QR_Transaction() == true)
	{
		if((Is_BDO_Pos_Acquiring_Transaction()) == true)
		{
			strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"X",1);
		}
	}

	else if(strncmp(temp_pos_entry_mode,"91",2) == 0 || strncmp(temp_pos_entry_mode,"2W",2) == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	}
	else if(strncmp(temp_pos_entry_mode,"92",2) == 0   ||
			strncmp(temp_pos_entry_mode,"07",2) == 0   ||
			strncmp(temp_pos_entry_mode,"06",2) == 0   ||
			strncmp(temp_pos_entry_mode,"X5",2) == 0	)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
	}
	else if((FALCON_is_Credential_On_File() ==true) ||(strncmp(temp_pos_entry_mode,"81",2) == 0))
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"E",1);
	}
	else if(strncmp(temp_pos_entry_mode,"80",2) == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	}
	else if(strncmp(temp_pos_entry_mode,"79",2) == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"G",1);
	}
	else if(strncmp(temp_pos_entry_mode,"01",2) == 0 )
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	}
	else if(strncmp(temp_pos_entry_mode,"02",2) == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"S",1);
	}
	else if(strncmp(temp_pos_entry_mode,"90",2) == 0 ||
			strncmp(temp_pos_entry_mode,"97",2) == 0 )
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"U",1);
	}
	else if(strncmp(temp_pos_entry_mode,"05",2) == 0 || strncmp(temp_pos_entry_mode,"52",2) == 0 ||
			strncmp(temp_pos_entry_mode,"51",2) == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"V",1);
	}
	else if(strncmp(temp_pos_entry_mode,"09",2) == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	}

	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG," ",1);
	}

	sprintf(&message[433],"%-1s\n",Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG);
}
void RT_FALCON_build_request_field_42( )
{
	if(Cr_Fld_RT.AUTHORIZATION_POSTING_XCD == "P")
	{
		memcpy(Cr_Fld_RT.TRANSACTION_POSTING_DT,Auth_Tx.TLF01_details.date_yyyymmdd,8);
		sprintf(&message[434],"%-8s\n",Cr_Fld_RT.TRANSACTION_POSTING_DT);
	}
	else
	{
		memset(Cr_Fld_RT.TRANSACTION_POSTING_DT,' ',8);
		sprintf(&message[434],"%-8s\n",Cr_Fld_RT.TRANSACTION_POSTING_DT);
	}
}
void RT_FALCON_build_request_field_43( )
{
	memset(Cr_Fld_RT.AUTHORIZATION_POSTING_MIS_XCD,' ',1);
	sprintf(&message[442],"%-1s\n",Cr_Fld_RT.AUTHORIZATION_POSTING_MIS_XCD);
}
void RT_FALCON_build_request_field_44( )
{
	memset(Cr_Fld_RT.TRANS_POSTING_MISMATCH_XFLG,' ',1);
	sprintf(&message[443],"%-1s\n",Cr_Fld_RT.TRANS_POSTING_MISMATCH_XFLG);
}
void RT_FALCON_build_request_field_45( )
{
	memset(Cr_Fld_RT.CREATE_CASE_XFLG,' ',1);
	sprintf(&message[444],"%-1s\n",Cr_Fld_RT.CREATE_CASE_XFLG);
}
void RT_FALCON_build_request_field_46( )
{
	memset(Cr_Fld_RT.USER_INDICATOR_1_XCD,' ',1);
	sprintf(&message[445],"%-1s\n",Cr_Fld_RT.USER_INDICATOR_1_XCD);
}
void RT_FALCON_build_request_field_47( )
{
	memset(Cr_Fld_RT.USER_INDICATOR_2_XCD,' ',1);
	sprintf(&message[446],"%-1s\n",Cr_Fld_RT.USER_INDICATOR_2_XCD);
}
void RT_FALCON_build_request_field_48( )
{
	memset(Cr_Fld_RT.USER_DATA_1_STRG,' ',10); /* UCAF indicator DE48 SE43*/

	/*Field 48 for Terminal type*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_1_STRG,&Auth_Tx.additional_pos_information[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_1_STRG,&Auth_Tx.acceptor_term_name[9],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_1_STRG,&Auth_Tx.amex_pos_entry_mode[3],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)== 0)
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				memcpy(Cr_Fld_RT.USER_DATA_1_STRG,"08", 2);/*Mobile phone Voice transaction*/
			}
			else
			{
				memcpy(Cr_Fld_RT.USER_DATA_1_STRG,"07", 2);/*PC Voice transaction*/
			}
		}
		else
		{
			memcpy(Cr_Fld_RT.USER_DATA_1_STRG,"03", 2);/*POS-Point Of Sale 03*/
		}
	}

	sprintf(&message[447],"%-10s\n",Cr_Fld_RT.USER_DATA_1_STRG);
}
void RT_FALCON_build_request_field_49( )
{
	memset(Cr_Fld_RT.USER_DATA_2_STRG,' ',10);
	/*Field 49 for Terminal Entry capa*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[1],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_2_STRG,&Auth_Tx.acceptor_term_name[10],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		/*if(Auth_Tx.TLF01_details.product_codes[16].amount[0]=='5')
			strncpy(Cr_Fld_RT.USER_DATA_2_STRG,"C",1);
		else*/
			memcpy(Cr_Fld_RT.USER_DATA_2_STRG,&Auth_Tx.TLF01_details.product_codes[16].amount[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		if(strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0  ||
			strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"S",1) == 0 ||
			strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"T",1) == 0 ||
			strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"U",1) == 0)
		{
			strncpy(Cr_Fld_RT.USER_DATA_2_STRG,"2",1);
		}
		else if(strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0)
		{
			strncpy(Cr_Fld_RT.USER_DATA_2_STRG,"5",1);
		}
		else
		{
			strncpy(Cr_Fld_RT.USER_DATA_2_STRG,"0",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)
	{
		Cr_Fld_RT.USER_DATA_2_STRG[0] = Auth_Tx.amex_pos_entry_mode[0] ;
		Cr_Fld_RT.USER_DATA_2_STRG[1] = '\0';
	}
	/*if(strcmp(Auth_Tx.BON01_details.minimum_amount,"")!= 0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_2_STRG,Auth_Tx.BON01_details.minimum_amount,10);
	}*/
	sprintf(&message[457],"%-10s\n",Cr_Fld_RT.USER_DATA_2_STRG);
}
void RT_FALCON_build_request_field_50( )
{
	memset(Cr_Fld_RT.MERCHANT_XIDD,' ',10);
	sprintf(&message[467],"%-10s\n",Cr_Fld_RT.MERCHANT_XIDD);
}
void RT_FALCON_build_request_field_51( )
{
	memset(Cr_Fld_RT.MERCHANT_DATA_XFLG,' ',1);
	sprintf(&message[477],"%-1s\n",Cr_Fld_RT.MERCHANT_DATA_XFLG);
}
void RT_FALCON_build_request_field_52( )
{
	memset(Cr_Fld_RT.CARD_DATA_AVAILABLE_XFLG,' ',1);
	sprintf(&message[478],"%-1s\n",Cr_Fld_RT.CARD_DATA_AVAILABLE_XFLG);
}
void RT_FALCON_build_request_field_53( )
{
	memcpy(Cr_Fld_RT.EXTERNAL_1_SCR,"0000",4);
	sprintf(&message[479],"%-4s\n",Cr_Fld_RT.EXTERNAL_1_SCR);
}
void RT_FALCON_build_request_field_54( )
{
	memcpy(Cr_Fld_RT.EXTERNAL_2_SCR,"0000",4);
	sprintf(&message[483],"%-4s\n",Cr_Fld_RT.EXTERNAL_2_SCR);
}
void RT_FALCON_build_request_field_55( )
{
	memcpy(Cr_Fld_RT.EXTERNAL_3_SCR,"0000",4);
	sprintf(&message[487],"%-4s\n",Cr_Fld_RT.EXTERNAL_3_SCR);

}
void RT_FALCON_build_request_field_56( )
{
	if(strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"C",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"D",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"F",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"G",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"S",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"T",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"U",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0)
	{
		memcpy(Cr_Fld_RT.CARDHOLDER_PRESENT_XFLG,"Y",1);
		memcpy(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD,"P",1);
	}
	else if(strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0 ||
	   strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0 )
	{
		memcpy(Cr_Fld_RT.CARDHOLDER_PRESENT_XFLG,"N",1);
		if(strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
		{
			memcpy(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD,"I",1);
		}
		else
		{
			memcpy(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD,"M",1);
		}
	}
	else
	{
		memcpy(Cr_Fld_RT.CARDHOLDER_PRESENT_XFLG," ",1);
		memcpy(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD,"O",1);
	}

	sprintf(&message[491],"%-1s\n",Cr_Fld_RT.CARDHOLDER_PRESENT_XFLG);
}

void RT_FALCON_build_request_field_57( )
{
	if(strncmp(Cr_Fld_RT.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
	{
		memcpy(Cr_Fld_RT.CAT_TYPE_XFLG,"E",1);/* POS Condition code*/
	}
	else
	{
		memcpy(Cr_Fld_RT.CAT_TYPE_XFLG,"S",1);/* POS Condition code*/
	}
	sprintf(&message[492],"%-1s\n",Cr_Fld_RT.CAT_TYPE_XFLG);
}
void RT_FALCON_build_request_field_58( )
{
	memset(Cr_Fld_RT.TESTING_RANDOM_DIGITS_STRG,' ',2);
	sprintf(&message[493],"%-2s\n",Cr_Fld_RT.TESTING_RANDOM_DIGITS_STRG);
}
void RT_FALCON_build_request_field_59( )
{
	memset(Cr_Fld_RT.PORTFOLIO_NAME,' ',14);
	sprintf(&message[495],"%-14s\n",Cr_Fld_RT.PORTFOLIO_NAME);
}
void RT_FALCON_build_request_field_60( )
{
	memset(Cr_Fld_RT.CLIENT_2_XID,' ',14);
	sprintf(&message[509],"%-14s\n",Cr_Fld_RT.CLIENT_2_XID);
}
void RT_FALCON_build_request_field_61( )
{
	memset(Cr_Fld_RT.INTERBANK_CARD_ASSOCIATION_NUM,' ',6);
	sprintf(&message[523],"%-6s\n",Cr_Fld_RT.INTERBANK_CARD_ASSOCIATION_NUM);
}
void RT_FALCON_build_request_field_62( )
{
	if(strlen(Auth_Tx.MCF01_details.name02)> 0)
	{
		memcpy(Cr_Fld_RT.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
	}
	else if(strlen(Auth_Tx.TLF01_details.card_holder_name) > 0)
	{
		memcpy(Cr_Fld_RT.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
	}

	sprintf(&message[529],"%-40s\n",Cr_Fld_RT.MERCHANT_NAME);
}
void RT_FALCON_build_request_field_63( )
{
	if(strlen(Auth_Tx.MCF01_details.city)== 0)
	{
		memcpy(Cr_Fld_RT.MERCHANT_CITY_NAME,Auth_Tx.TLF01_details.ncf01_retired_cwk,13);
	}
	else
	{
		memcpy(Cr_Fld_RT.MERCHANT_CITY_NAME,Auth_Tx.MCF01_details.city,20);
	}
	sprintf(&message[569],"%-30s\n",Cr_Fld_RT.MERCHANT_CITY_NAME);
}
void RT_FALCON_build_request_field_64( )
{
	if(strlen(Auth_Tx.MCF01_details.intl_code)== 0)
	{
		memcpy(Cr_Fld_RT.MERCHANT_STATE_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+13,3);
	}
	else
	{
		memcpy(Cr_Fld_RT.MERCHANT_STATE_XCD,Auth_Tx.MCF01_details.intl_code,3);
	}
	sprintf(&message[599],"%-3s\n",Cr_Fld_RT.MERCHANT_STATE_XCD);
}
void RT_FALCON_build_request_field_65( )
{
	memset(Cr_Fld_RT.SUPPRES_CASE_XFLG,' ',1);
	sprintf(&message[602],"%-1s\n",Cr_Fld_RT.SUPPRES_CASE_XFLG);
}
void RT_FALCON_build_request_field_66( )
{
	/***memcpy(Cr_Fld_RT.USER_INDICATOR_3_XCD,' ' ,5);****/
	memcpy(Cr_Fld_RT.USER_INDICATOR_3_XCD,"",5);
	sprintf(&message[603],"%-5s\n",Cr_Fld_RT.USER_INDICATOR_3_XCD);
}
	/*********************UserIndicator04****************************/
void RT_FALCON_build_request_field_67( )
{
	int len_curr_code, len_TLF_curr_code = 0, len_recon_curr_code=0 ;

	len_TLF_curr_code = strlen(Auth_Tx.TLF01_details.currency_code);
	len_curr_code = strlen(Auth_Tx.MCF01_details.currency_code);
	len_recon_curr_code = strlen(Auth_Tx.reconciliation_cur_code);

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	    	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if(len_recon_curr_code > 0)
		{
			strncpy(Cr_Fld_RT.USER_INDICATOR_4_XCD,Auth_Tx.reconciliation_cur_code,3);
		}
		else if (len_TLF_curr_code > 0)
		{
			strncpy(Cr_Fld_RT.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
		}

	}
	else if(len_curr_code>0)
	{
		strncpy(Cr_Fld_RT.USER_INDICATOR_4_XCD,Auth_Tx.MCF01_details.currency_code,3);
	}
	else if (len_TLF_curr_code > 0)
	{
		strncpy(Cr_Fld_RT.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
	}
	else
	{
		memset(Cr_Fld_RT.USER_INDICATOR_4_XCD,SPACES,FIELD67_SIZE);
	}
	sprintf(&message[608],"%-5s\n",Cr_Fld_RT.USER_INDICATOR_4_XCD);
	/***************************************************************/
}

	/* Dual currrecny logic*/

	/**len**strcpy(Cr_Fld_RT.USER_DATA_3_STRG,"000");**/
void RT_FALCON_build_request_field_68( )
{
	char temp_totalamt[14]={0};
	strncpy(Cr_Fld_RT.USER_DATA_3_STRG,"00",2);
	if((Check_If_DINERS_Transaction ()  == true)||
		(Check_If_CUP_Transaction ()  == true))
	{
		int original_tx_amt = 0;
		if(strlen(Auth_Tx.TLF01_details.orig_amount)>0)
		{
			original_tx_amt = atoi(Auth_Tx.TLF01_details.orig_amount);
		}
		else
		{
			original_tx_amt = atoi(Auth_Tx.TLF01_details.total_amount);
		}
		sprintf(temp_totalamt,"%010d.%02d",original_tx_amt/100, original_tx_amt%100);
	}
	else if((Check_If_VISA_Transaction () == true)||
			(Check_If_JCB_Transaction ()  == true)||
			(Check_If_MC_Transaction ()  == true)||
			(Check_If_Fleet_Transaction() == true)||
			 Check_If_AMEX_Transaction ()==true)
	{
		if(strlen(Auth_Tx.reconciliation_amount) > 0)
		{
			sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.reconciliation_amount)/100, atoi(Auth_Tx.reconciliation_amount)%100);
		}
		else
		{
			sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);
		}
	}
	strncat(Cr_Fld_RT.USER_DATA_3_STRG,temp_totalamt,13);
	sprintf(&message[613],"%-15s\n",Cr_Fld_RT.USER_DATA_3_STRG);
}
void RT_FALCON_build_request_field_69( )
{
	if(strlen(Auth_Tx.TLF01_details.response_text)>0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_4_STRG,Auth_Tx.TLF01_details.response_text,20);
	}
	else
	{
		memset(Cr_Fld_RT.USER_DATA_4_STRG,SPACES,20);
	}
	sprintf(&message[628],"%-20s\n",Cr_Fld_RT.USER_DATA_4_STRG);
}
void RT_FALCON_build_request_field_70( )
{
	CHAR resp_buf[15] = {0} ;
	memset(Cr_Fld_RT.USER_DATA_5_STRG,' ',40);
	if ((Auth_Tx.TLF01_details.tx_key) > 100)
	{
		strncpy (resp_buf , " RESPONSE",10);
	}
	else
	{
		memset(resp_buf,0x00,sizeof(resp_buf)-1);
	}

	switch(Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_INVALID							  :

			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"INVALID");
		break ;

		case AUTH_SALE                     	     	  :
		case AUTH_SALE_RESPONSE                       :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"SALE");
		break ;

		case AUTH_AUTHORIZATION						  :
		case AUTH_AUTHORIZATION_RESPONSE			  :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"AUTHORIZATION");
		break ;

		case AUTH_CASH_ADVANCE				          :
		case AUTH_CASH_ADVANCE_RESPONSE               :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"CASH ADVANCE");
		break ;

		case AUTH_OFFLINE_SALE                        :
		case AUTH_OFFLINE_SALE_RESPONSE               :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"OFFLINE SALE");
		break ;

		case AUTH_SALE_ADJUSTMENT                     :
		case AUTH_SALE_ADJUSTMENT_RESPONSE            :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"SALE ADJUSTMENT");
		break ;

		case AUTH_BALANCE_INQUIRY                     :
		case AUTH_BALANCE_INQUIRY_RESPONSE            :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"BALANCE INQUIRY");
		break ;

		case AUTH_PRE_AUTHORIZATION                   :
		case AUTH_PRE_AUTHORIZATION_RESPONSE          :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"PRE AUTHORIZATION");
		break ;

		case AUTH_VOID_SALE                           :
		case AUTH_VOID_SALE_RESPONSE                  :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"VOID SALE");
		break ;

		case AUTH_VOID_REFUND                         :
		case AUTH_VOID_REFUND_RESPONSE                :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"VOID REFUND");
		break ;

		case AUTH_REVERSAL                            :
		case AUTH_REVERSAL_RESPONSE                   :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"REVERSAL");
		break ;

		case AUTH_VOID_CASH_ADVANCE                   :
		case AUTH_VOID_CASH_ADVANCE_RESPONSE          :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"VOID CASH ADVANCE");
		break ;

		case AUTH_SALE_CASH                           :
		case AUTH_SALE_CASH_RESPONSE                  :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"SALE CASH");
		break ;

		case AUTH_SALES_COMPLETION                    :
		case AUTH_SALES_COMPLETION_RESPONSE           :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"SALE COMPLETION");
		break ;

		case AUTH_QUASI_CASH                          :
		case AUTH_QUASI_CASH_RESPONSE                 :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"QUASI CASH");
		break ;

		case AUTH_ADVICE                              :
		case AUTH_ADVICE_RESPONSE                     :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"ADVICE");
		break ;

		case AUTH_REVERSAL_ADVICE                     :
		case AUTH_REVERSAL_ADVICE_RESPONSE            :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"REVERSAL ADVICE");
		break ;

		case AUTH_ADVICE_REPEAT                       :
		case AUTH_ADVICE_REPEAT_RESPONSE              :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"ADVICE REPEAT");
		break ;

		case AUTH_REVERSAL_REPEAT                     :
		case AUTH_REVERSAL_REPEAT_RESPONSE            :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"REVERSAL REPEAT");
		break ;

		case AUTH_CASH_ADVANCE_AUTHORIZATION          :
		case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"CASH ADVANCE AUTHORIZATION");
		break ;

		case AUTH_AUTHORIZATION_OVERRIDE              :
		case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE     :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"AUTHORIZATION OVERRIDE");
		break ;

		case AUTH_OFFLINE_VOID_SALE                   :
		case AUTH_OFFLINE_VOID_SALE_RESPONSE          :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"OFFLINE VOID SALE");
		break ;

		case AUTH_PRE_AUTHORIZATION_CANCLE            :
		case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE   :
			strcpy(Cr_Fld_RT.USER_DATA_5_STRG,"PRE AUTHORIZATION CANCLE");
		break ;

	}
	strcat(Cr_Fld_RT.USER_DATA_5_STRG,resp_buf);


	sprintf(&message[648],"%-40s\n",Cr_Fld_RT.USER_DATA_5_STRG);
}
void RT_FALCON_build_request_field_71( )
{
	memset(Cr_Fld_RT.REAL_TIME_REQUEST_XFLG,' ',1);
	sprintf(&message[688],"%-1s\n",Cr_Fld_RT.REAL_TIME_REQUEST_XFLG);
}
void RT_FALCON_build_request_field_72( )
{
	memset(Cr_Fld_RT.PRIOR_ACTION_DATABASE_XCD,' ',1);
	sprintf(&message[689],"%-1s\n",Cr_Fld_RT.PRIOR_ACTION_DATABASE_XCD);
}
void RT_FALCON_build_request_field_73( )
{
	memset(Cr_Fld_RT.PRIOR_ACTION_DB_EXPIRATION_DT,' ',8);
	sprintf(&message[690],"%-8s\n",Cr_Fld_RT.PRIOR_ACTION_DB_EXPIRATION_DT);
}
void RT_FALCON_build_request_field_74( )
{
	memset(Cr_Fld_RT.MASTER_ACCOUNT_NUMBER_XID,' ',19);
	sprintf(&message[698],"%-19s\n",Cr_Fld_RT.MASTER_ACCOUNT_NUMBER_XID);
}
void RT_FALCON_build_request_field_75( )
{
	memset(Cr_Fld_RT.CARD_OFFLN_STATIC_AUTHCTN_XFLG,' ',1);
	sprintf(&message[717],"%-1s\n",Cr_Fld_RT.CARD_OFFLN_STATIC_AUTHCTN_XFLG);
}
void RT_FALCON_build_request_field_76( )
{
	memset(Cr_Fld_RT.CARD_DYNAMIC_AUTHCTN_XCD,' ',1);
	sprintf(&message[718],"%-1s\n",Cr_Fld_RT.CARD_DYNAMIC_AUTHCTN_XCD);
}
void RT_FALCON_build_request_field_77( )
{
	memset(Cr_Fld_RT.RESERVED_02,' ',1);
	sprintf(&message[719],"%-1s\n",Cr_Fld_RT.RESERVED_02);
}
void RT_FALCON_build_request_field_78( )
{
	memset(Cr_Fld_RT.CARD_AIP_VERIFICATION_XFLG,' ',1);
	sprintf(&message[720],"%-1s\n",Cr_Fld_RT.CARD_AIP_VERIFICATION_XFLG);
}
void RT_FALCON_build_request_field_79( )
{
	memset(Cr_Fld_RT.CARD_AIP_RISK_XFLG,' ',1);
	sprintf(&message[721],"%-1s\n",Cr_Fld_RT.CARD_AIP_RISK_XFLG);
}
void RT_FALCON_build_request_field_80( )
{
	memset(Cr_Fld_RT.CARD_ISSUER_AUTH_SUPPORT_XFLG,' ',1);
	sprintf(&message[722],"%-1s\n",Cr_Fld_RT.CARD_ISSUER_AUTH_SUPPORT_XFLG);
}
void RT_FALCON_build_request_field_81( )
{
	memset(Cr_Fld_RT.CARD_COMBINED_AUTHCTN_XFLG,' ',1);
	sprintf(&message[723],"%-1s\n",Cr_Fld_RT.CARD_COMBINED_AUTHCTN_XFLG);
}
void RT_FALCON_build_request_field_82( )
{
	memset(Cr_Fld_RT.TRANSACTION_DELINQUENT_CYC,' ',1);
	sprintf(&message[724],"%-1s\n",Cr_Fld_RT.TRANSACTION_DELINQUENT_CYC);
}
void RT_FALCON_build_request_field_83( )
{
	memset(Cr_Fld_RT.TRANS_TOTAL_DELINQUENT_AMT,' ',13);
	sprintf(&message[725],"%-13s\n",Cr_Fld_RT.TRANS_TOTAL_DELINQUENT_AMT);
}
void RT_FALCON_build_request_field_84( )
{
	memset(Cr_Fld_RT.CRD_STATEMENT_CASH_BALANCE_AMT,' ',13);
	sprintf(&message[738],"%-13s\n",Cr_Fld_RT.CRD_STATEMENT_CASH_BALANCE_AMT);
}
void RT_FALCON_build_request_field_85( )
{
	memset(Cr_Fld_RT.MERCHANT_BALANCE_AMT,' ',13);
	sprintf(&message[751],"%-13s\n",Cr_Fld_RT.MERCHANT_BALANCE_AMT);
}
void RT_FALCON_build_request_field_86( )
{
	memset(Cr_Fld_RT.CUSTOMER_POSTAL_XCD,' ',12);
	sprintf(&message[764],"%-12s\n",Cr_Fld_RT.CUSTOMER_POSTAL_XCD);
}
void RT_FALCON_build_request_field_87( )
{
	memset(Cr_Fld_RT.CARD_MEDIA_TYPE_XCD,' ',1);
	sprintf(&message[776],"%-1s\n",Cr_Fld_RT.CARD_MEDIA_TYPE_XCD);
}
void RT_FALCON_build_request_field_88( )
{
	if (strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(Auth_Tx.TLF01_details.cvc == 'P')
		{
			Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = '0' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M' ||
				Auth_Tx.TLF01_details.cvc == 'N')
		{
			Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = '1' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'U')
		{
			Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = ' ' ;
		}
		else
		{
			Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = '9' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'C' ||
		    Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'D' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'K' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'L' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'M' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'N' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'P')
		{
			Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = '1';
		}
		else if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'U')
		{
			Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = '9';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0)
	{
		Cr_Fld_RT.CVV2_PRESENT_XFLG[0] = ' ';
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		/*Not getting value*/
	}


	sprintf(&message[777],"%-1s\n",Cr_Fld_RT.CVV2_PRESENT_XFLG);
}
void RT_FALCON_build_request_field_89( )
{
	if (strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(Auth_Tx.TLF01_details.cvc == 'P')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'P' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'M' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'N')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'N' ;
		}
		else
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = ' ' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'C' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'K' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'M' )
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'M';
		}
		else if ( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'D' ||
				  Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'L' ||
				  Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'N')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'N';
		}
		else if ( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'P')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'P';
		}
		else if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'S')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'S';
		}
		else if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'U')
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = 'U';
		}
		else
		{
			Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = ' ';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0)
	{
		Cr_Fld_RT.CVV2_RESPONSE_XFLG[0] = ' ';
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		/* Not getting value*/
	}


	sprintf(&message[778],"%-1s\n",Cr_Fld_RT.CVV2_RESPONSE_XFLG);
}
void RT_FALCON_build_request_field_90( )
{
	/*****memset(Cr_Fld_RT.AVS_RESPONSE_XCD,' ',1);*******/
	memset(Cr_Fld_RT.AVS_RESPONSE_XCD,' ',1);
	sprintf(&message[779],"%-1s\n",Cr_Fld_RT.AVS_RESPONSE_XCD);
	/***memcpy(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD,"O",1);*****/
}
void RT_FALCON_build_request_field_91( )
{
	if(strlen(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD) == 0 || strcmp(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD, " ") == 0)
	strncpy(Cr_Fld_RT.TRANSACTION_CATEGORY_XCD,"O",1);
	sprintf(&message[780],"%-1s\n",Cr_Fld_RT.TRANSACTION_CATEGORY_XCD);
	/*Auth_Tx.TLF01_details.acquirer_id*/
	/******memset(Cr_Fld_RT.ACQUIRER_XID,' ',12);*****/
}
void RT_FALCON_build_request_field_92( )
{
	strncpy(Cr_Fld_RT.ACQUIRER_XID,Auth_Tx.TLF01_details.acquiring_id,12);
	sprintf(&message[781],"%-12s\n",Cr_Fld_RT.ACQUIRER_XID);
	/*If it's a local transactions*/
}
void RT_FALCON_build_request_field_93( )
{
	if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5) == 0)
	{
		strncpy(Cr_Fld_RT.ACQUIRER_COUNTRY_XCD,"608",3);
	}
	else
	{
		if(isnum(Auth_Tx.country_code))
		{
			strncpy(Cr_Fld_RT.ACQUIRER_COUNTRY_XCD,Auth_Tx.country_code,3);
		}
		else if (isnum(Auth_Tx.reconciliation_cur_code))
		{
			strncpy(Cr_Fld_RT.ACQUIRER_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
		}
		else if (isnum(Auth_Tx.TLF01_details.currency_code))
		{
			strncpy(Cr_Fld_RT.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);
		}
		else
		{
			strncpy(Cr_Fld_RT.ACQUIRER_COUNTRY_XCD,"608",3);
		}
	}
	sprintf(&message[793],"%-3s\n",Cr_Fld_RT.ACQUIRER_COUNTRY_XCD);
}
void RT_FALCON_build_request_field_94( )
{
	if(strcmp(Auth_Tx.TLF01_details.terminal_id,"") != 0)
	{
		memcpy(Cr_Fld_RT.TERMINAL_XID,Auth_Tx.TLF01_details.terminal_id,16);
		sprintf(&message[796],"%-16s\n",Cr_Fld_RT.TERMINAL_XID);
	}
	else
	{
		memset(Cr_Fld_RT.TERMINAL_XID,' ',16);
		sprintf(&message[796],"%-16s\n",Cr_Fld_RT.TERMINAL_XID);
	}
}
void RT_FALCON_build_request_field_95( )
{
	memset(Cr_Fld_RT.TERMINAL_TYPE_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1' ||
			Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2' )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '7' )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"I",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0' )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"N",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7' )
	   {
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"I",1);
	   }
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3' )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
		{
			memset(Cr_Fld_RT.TERMINAL_TYPE_XCD,'U',1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,&Auth_Tx.amex_pos_entry_mode[3],1);
		if(strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"1",1) == 0 ||
		   strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"3",1) == 0 ||
		   strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"X",1) == 0)
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"2",1) == 0 ||
		   strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"4",1) == 0 ||
		   strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"5",1) == 0 ||
		   strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"Z",1) == 0)
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"S",1) == 0 ||
		   strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"T",1) == 0)
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"I",1);
		}
		else if(strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"0",1) == 0 ||
			strncmp(Cr_Fld_RT.TERMINAL_TYPE_XCD,"9",1) == 0)
		{
			memset(Cr_Fld_RT.TERMINAL_TYPE_XCD,' ',1);
		}
		else
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"I",1);
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "03" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "06" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "11" ,2)== 0 )
		{
			Cr_Fld_RT.TERMINAL_TYPE_XCD[0] = 'L' ;
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "01" ,2)== 0 )
		{
			Cr_Fld_RT.TERMINAL_TYPE_XCD[0] = 'U' ;
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "05" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "07" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "08" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "09" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "12" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "14" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "16" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "17" ,2)== 0)
		{
			Cr_Fld_RT.TERMINAL_TYPE_XCD[0] = 'I' ;
		}
		else if ( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "00" ,2)== 0)
		{
			Cr_Fld_RT.TERMINAL_TYPE_XCD[0] = ' ' ;
		}
		else
		{
			Cr_Fld_RT.TERMINAL_TYPE_XCD[0] = 'O' ;
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '3'  )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5' )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '9')
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"O",1);
		}
	}

	sprintf(&message[812],"%-1s\n",Cr_Fld_RT.TERMINAL_TYPE_XCD);
}

	/*********memcpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);*****************/
	/*Field 96 Terminal Entry capability */
void RT_FALCON_build_request_field_96( )
{
	memset(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '8')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'D' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '9')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'K' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'Z' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ' ;
		}
		else
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
	   if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '5' ||
		  Auth_Tx.TLF01_details.product_codes[16].amount[10] == '8'	||
		  Auth_Tx.TLF01_details.product_codes[16].amount[10] == '9'	 )
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3')
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'D' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6')
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'K' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
			   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7')
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4')
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'Z' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ' ;
	   }
	   else
	   {
		   Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
	   }
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"C",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' )
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"K",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"M",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld_RT.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M';
		}
		else if (Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1]  == '6')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C';
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0')
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ';
		}
		else
		{
			Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,"C",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '8')
		{
			strncpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,"D",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1')
		{
			strncpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,"K",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,"M",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);
		}
	}
	sprintf(&message[813],"%-1s\n",Cr_Fld_RT.TERMINAL_ENTRY_CAPABILITY_XCD);
}
void RT_FALCON_build_request_field_97( )
{
	strncpy(Cr_Fld_RT.TRANSACTION_POS_CONDITION_XCD,Auth_Tx.TLF01_details.pos_condition_code,2);

	sprintf(&message[814],"%-2s\n",Cr_Fld_RT.TRANSACTION_POS_CONDITION_XCD);
	/***************memcpy(Cr_Fld_RT.ATM_NETWORK_XID,,);*************/
}
void RT_FALCON_build_request_field_98( )
{
	memcpy(Cr_Fld_RT.ATM_NETWORK_XID," ",1);
	sprintf(&message[816],"%-1s\n",Cr_Fld_RT.ATM_NETWORK_XID);
}
void RT_FALCON_build_request_field_99( )
{
	memset(Cr_Fld_RT.RESERVED_01,' ',1);
	sprintf(&message[817],"%-1s\n",Cr_Fld_RT.RESERVED_01);
}
void RT_FALCON_build_request_field_100( )
{
	memset(Cr_Fld_RT.CHECK_NUM,' ',6);
	sprintf(&message[818],"%-6s\n",Cr_Fld_RT.CHECK_NUM);
}
void RT_FALCON_build_request_field_101( )
{
	memset(Cr_Fld_RT.TERML_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[824],"%-10s\n",Cr_Fld_RT.TERML_VERIFICATION_RESULTS_XCD);
}
void RT_FALCON_build_request_field_102( )
{
	memset(Cr_Fld_RT.CARD_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[834],"%-10s\n",Cr_Fld_RT.CARD_VERIFICATION_RESULTS_XCD);
}
	/*********memset(Cr_Fld_RT.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);**********/
void RT_FALCON_build_request_field_103( )
{
	memset(Cr_Fld_RT.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);
	sprintf(&message[844],"%-1s\n",Cr_Fld_RT.AUTHZN_RQST_CRYPTO_VALID_XFLG);
}
	/************memset(Cr_Fld_RT.ATC_CARD_CNT,' ',5);***************/
void RT_FALCON_build_request_field_104( )
{
	memset(Cr_Fld_RT.ATC_CARD_CNT,' ',5);
	sprintf(&message[845],"%-5s\n",Cr_Fld_RT.ATC_CARD_CNT);
}
	/*********memset(Cr_Fld_RT.ATC_HOST_CNT,' ',5);**************/
void RT_FALCON_build_request_field_105( )
{
	memset(Cr_Fld_RT.ATC_HOST_CNT,' ',5);
	sprintf(&message[850],"%-5s\n",Cr_Fld_RT.ATC_HOST_CNT);
}
void RT_FALCON_build_request_field_106( )
{
	memset(Cr_Fld_RT.RESERVED_03,' ',2);
	sprintf(&message[855],"%-2s\n",Cr_Fld_RT.RESERVED_03);
}
void RT_FALCON_build_request_field_107( )
{
	memset(Cr_Fld_RT.TERML_TO_CRD_OFFLINE_LIMIT_XCD,' ',2);
	sprintf(&message[857],"%-2s\n",Cr_Fld_RT.TERML_TO_CRD_OFFLINE_LIMIT_XCD);
}
void RT_FALCON_build_request_field_108( )
{
	memset(Cr_Fld_RT.TERMINAL_OFFLINE_LIMIT_MAX_XCD,' ',2);
	sprintf(&message[859],"%-2s\n",Cr_Fld_RT.TERMINAL_OFFLINE_LIMIT_MAX_XCD);
}
void RT_FALCON_build_request_field_109( )
{
	memset(Cr_Fld_RT.RECURRING_AUTHORIZATION_XCD,' ',2);
	sprintf(&message[861],"%-2s\n",Cr_Fld_RT.RECURRING_AUTHORIZATION_XCD);
}
void RT_FALCON_build_request_field_110( )
{
	memset(Cr_Fld_RT.RECURRING_AUTHZN_EXPIRATION_DT,' ',8);
	sprintf(&message[863],"%-8s\n",Cr_Fld_RT.RECURRING_AUTHZN_EXPIRATION_DT);
}
void RT_FALCON_build_request_field_111( )
{
	memset(Cr_Fld_RT.CARD_ASSOCIATION_XFLG,' ',1);
	sprintf(&message[871],"%-1s\n",Cr_Fld_RT.CARD_ASSOCIATION_XFLG);
}
void RT_FALCON_build_request_field_112( )
{
	memset(Cr_Fld_RT.CARD_INCENTIVE_XCD,' ',1);
	sprintf(&message[872],"%-1s\n",Cr_Fld_RT.CARD_INCENTIVE_XCD);
}
void RT_FALCON_build_request_field_113( )
{
	memset(Cr_Fld_RT.CARD_STATUS_XCD,' ',2);
	sprintf(&message[873],"%-2s\n",Cr_Fld_RT.CARD_STATUS_XCD);
}
void RT_FALCON_build_request_field_114( )
{
	memset(Cr_Fld_RT.CARD_STATUS_DT,' ',8);
	sprintf(&message[875],"%-8s\n",Cr_Fld_RT.CARD_STATUS_DT);
}
	/********memset(Cr_Fld_RT.PROCESSOR_REASON_XCD,' ',5);*****/
	/*****memcpy(&Cr_Fld_RT.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);**************/

void RT_FALCON_build_request_field_115( )
{
	memset(Cr_Fld_RT.PROCESSOR_REASON_XCD,' ',5);
	sprintf(&message[883],"%-5s\n",Cr_Fld_RT.PROCESSOR_REASON_XCD);
}
void RT_FALCON_build_request_field_116( )
{
	if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,"stndin") == 0)
	{
		memcpy(Cr_Fld_RT.TRANSACTION_ADVICE_XCD,"A",1);
		sprintf(&message[888],"%-1s\n",Cr_Fld_RT.TRANSACTION_ADVICE_XCD);
	}
	else
	{
		memset(Cr_Fld_RT.TRANSACTION_ADVICE_XCD,' ',1);
		sprintf(&message[888],"%-1s\n",Cr_Fld_RT.TRANSACTION_ADVICE_XCD);
	}
}
void RT_FALCON_build_request_field_117( )
{
	memset(Cr_Fld_RT.MERCHANT_XID,' ',16);
	memcpy(Cr_Fld_RT.MERCHANT_XID,Auth_Tx.TLF01_details.merchant_id,15);
	sprintf(&message[889],"%-16s\n",Cr_Fld_RT.MERCHANT_XID);
}
void RT_FALCON_build_request_field_118( )
{
	memset(Cr_Fld_RT.CARD_ORDER_TYPE_XCD,' ',1);
	sprintf(&message[905],"%-1s\n",Cr_Fld_RT.CARD_ORDER_TYPE_XCD);
}
void RT_FALCON_build_request_field_119( )
{
	memset(Cr_Fld_RT.TRANSACTION_CASHBACK_AMT,' ',13);
	sprintf(&message[906],"%-13s\n",Cr_Fld_RT.TRANSACTION_CASHBACK_AMT);
}
	/***********memset(Cr_Fld_RT.USER_DATA_6_NUM,' ',13);************/
	/*Field 120 ECI value*/
void RT_FALCON_build_request_field_120( )
{
	memset(Cr_Fld_RT.USER_DATA_6_NUM,' ',13);

	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		memcpy( bcd_eci,&Auth_Tx.int_net_facility_data[5],2);
		convert_to_ascii(bcd_eci,ascii_eci,2);
		memcpy( Auth_Tx.TLF01_details.profile,ascii_eci,2);
		memcpy(Cr_Fld_RT.USER_DATA_6_NUM,Auth_Tx.TLF01_details.profile,2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0 ||
			strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	    == 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_6_NUM,&Auth_Tx.TLF01_details.profile,2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_6_NUM,"00",2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)
	{
		memcpy(Cr_Fld_RT.USER_DATA_6_NUM,&Auth_Tx.TLF01_details.product_codes[16].amount[1],2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)	== 0)
	{
		memset(Cr_Fld_RT.USER_DATA_6_NUM,' ',13);
	}
	else
	{
		Cr_Fld_RT.USER_DATA_6_NUM[0]= '2';
		strncpy(&Cr_Fld_RT.USER_DATA_6_NUM[1],Auth_Tx.TLF01_details.profile,2);
	}


	sprintf(&message[919],"%-13s\n",Cr_Fld_RT.USER_DATA_6_NUM);
	/*************memset(Cr_Fld_RT.USER_DATA_7_STRG,' ',40);********/
}
	/* User data 7 filled with AMEX POS DATA - Auth_Tx.amex_pos_entry_mode*/
void RT_FALCON_build_request_field_121( )
{
	/*	Prepare POS data only for acquired transaction */
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
		Falcon_Prepare_Pos_Data_F121();
	}
	memset(Cr_Fld_RT.USER_DATA_7_STRG,' ',40);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_7_STRG,Auth_Tx.amex_pos_entry_mode,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA) == 0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		strncpy(Cr_Fld_RT.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
			strncpy(Cr_Fld_RT.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB) == 0)
	{
			strncpy(Cr_Fld_RT.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,strlen(Auth_Tx.TLF01_details.product_codes[16].amount));
	}

	sprintf(&message[932],"%-40s\n",Cr_Fld_RT.USER_DATA_7_STRG);
}
void RT_FALCON_build_request_field_122( )
{
	memset(Cr_Fld_RT.PAYMENT_INSTRUMENT_XID,' ',30);
	sprintf(&message[972],"%-30s\n",Cr_Fld_RT.PAYMENT_INSTRUMENT_XID);
}
void RT_FALCON_build_request_field_123( )
{
	memset(Cr_Fld_RT.AVS_REQUEST_XCD,' ',1);
	sprintf(&message[1002],"%-1s\n",Cr_Fld_RT.AVS_REQUEST_XCD);
}
void RT_FALCON_build_request_field_124( )
{
	memset(Cr_Fld_RT.CVR_OFFLINEPIN_VERIFYPERF_XFLG,' ',1);
	sprintf(&message[1003],"%-1s\n",Cr_Fld_RT.CVR_OFFLINEPIN_VERIFYPERF_XFLG);
}
void RT_FALCON_build_request_field_125( )
{
	memset(Cr_Fld_RT.CVR_OFFLINEPIN_VERIFYSTAT_XFLG,' ',1);
	sprintf(&message[1004],"%-1s\n",Cr_Fld_RT.CVR_OFFLINEPIN_VERIFYSTAT_XFLG);
}
void RT_FALCON_build_request_field_126( )
{
	memcpy(Cr_Fld_RT.CVR_PIN_TRYLIMIT_EXCEED_XFLG,"0",1);
	sprintf(&message[1005],"%-1s\n",Cr_Fld_RT.CVR_PIN_TRYLIMIT_EXCEED_XFLG);
}
void RT_FALCON_build_request_field_127( )
{
	memset(Cr_Fld_RT.POS_TERMINAL_ATTEND_XFLG,' ',1);
	sprintf(&message[1006],"%-1s\n",Cr_Fld_RT.POS_TERMINAL_ATTEND_XFLG);
}
void RT_FALCON_build_request_field_128( )
{
	memset(Cr_Fld_RT.POS_OFF_PREMISES_XFLG,' ',1);
	sprintf(&message[1007],"%-1s\n",Cr_Fld_RT.POS_OFF_PREMISES_XFLG);
}
void RT_FALCON_build_request_field_129( )
{
	memset(Cr_Fld_RT.POS_CARD_CAPTURE_XFLG,' ',1);
	sprintf(&message[1008],"%-1s\n",Cr_Fld_RT.POS_CARD_CAPTURE_XFLG);
}
void RT_FALCON_build_request_field_130( )
{
	memset(Cr_Fld_RT.POS_SECURITY_XFLG,' ',1);
	sprintf(&message[1009],"%-1s\n",Cr_Fld_RT.POS_SECURITY_XFLG);
}
void RT_FALCON_build_request_field_131( )
{
	memcpy(Cr_Fld_RT.AUTHORIZATION_XID,"      ",6);
	sprintf(&message[1010],"%-6s\n",Cr_Fld_RT.AUTHORIZATION_XID);
}
void RT_FALCON_build_request_field_132( )
{
	memset(Cr_Fld_RT.USER_DATA_8_STRG,' ',10);

	memcpy(Cr_Fld_RT.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode,4);
	sprintf(&message[1016],"%-10s\n",Cr_Fld_RT.USER_DATA_8_STRG);
}
void RT_FALCON_build_request_field_133( )
{
	memset(Cr_Fld_RT.USER_DATA_9_STRG,' ',10);
	sprintf(&message[1026],"%-10s\n",Cr_Fld_RT.USER_DATA_9_STRG);
}
void RT_FALCON_build_request_field_134( )
{
	memset(Cr_Fld_RT.USER_INDICATOR_5_XCD,' ',1);
	sprintf(&message[1036],"%-1s\n",Cr_Fld_RT.USER_INDICATOR_5_XCD);
}
void RT_FALCON_build_request_field_135( )
{
	memset(Cr_Fld_RT.USER_INDICATOR_6_XCD,' ',1);
	sprintf(&message[1037],"%-1s\n",Cr_Fld_RT.USER_INDICATOR_6_XCD);
}

	/*Field 136 Service code*/
void RT_FALCON_build_request_field_136( )
{
	memset(Cr_Fld_RT.USER_INDICATOR_7_XCD,' ',5);

	strncpy(Cr_Fld_RT.USER_INDICATOR_7_XCD,Auth_Tx.TLF01_details.service_code,3);
	sprintf(&message[1038],"%-5s\n",Cr_Fld_RT.USER_INDICATOR_7_XCD);
}
void RT_FALCON_build_request_field_137( )
{
	memset(Cr_Fld_RT.USER_INDICATOR_8_XCD,' ',5);
	if(Is_BDO_Pos_Acquiring_Transaction() == true)
	{
		if(FALCON_is_BDO_PAY_QR_Transaction() ==true )
		{
			strncpy(Cr_Fld_RT.USER_INDICATOR_8_XCD,Auth_Tx.TLF01_details.nii+1,3);
		}
	}
	sprintf(&message[1043],"%-5s\n",Cr_Fld_RT.USER_INDICATOR_8_XCD);
}
void RT_FALCON_build_request_field_138( )
{
	memset(Cr_Fld_RT.MODEL_CONTROL_1_XCD,' ',1);
	sprintf(&message[1048],"%-1s\n",Cr_Fld_RT.MODEL_CONTROL_1_XCD);
}
void RT_FALCON_build_request_field_139( )
{
	memset(Cr_Fld_RT.MODEL_CONTROL_2_XCD,' ',1);
	sprintf(&message[1049],"%-1s\n",Cr_Fld_RT.MODEL_CONTROL_2_XCD);
}
void RT_FALCON_build_request_field_140( )
{
	memset(Cr_Fld_RT.MODEL_CONTROL_3_XCD,' ',1);
	sprintf(&message[1050],"%-1s\n",Cr_Fld_RT.MODEL_CONTROL_3_XCD);
}
void RT_FALCON_build_request_field_141( )
{
	memset(Cr_Fld_RT.MODEL_CONTROL_4_XCD,' ',1);
	sprintf(&message[1051],"%-1s\n",Cr_Fld_RT.MODEL_CONTROL_4_XCD);
}
void RT_FALCON_build_request_field_142( )
{
	if(strcmp(Auth_Tx.TLF01_details.response_code,"54") == 0)
	{
		strncpy(Cr_Fld_RT.AUTHORIZATION_EXPIRE_DT_XFLG,"I",1);
	}
	else
	{
		strncpy(Cr_Fld_RT.AUTHORIZATION_EXPIRE_DT_XFLG,"V",1);
	}

	sprintf(&message[1052],"%-1s\n",Cr_Fld_RT.AUTHORIZATION_EXPIRE_DT_XFLG);
}
void RT_FALCON_build_request_field_143( )
{
	memcpy(Cr_Fld_RT.AUTH_SECONDARY_VERIFY_XCD," ",1);

	sprintf(&message[1053],"%s\n",Cr_Fld_RT.AUTH_SECONDARY_VERIFY_XCD);
}
void RT_FALCON_build_request_field_144( )
{
	strncpy(Cr_Fld_RT.AUTHORIZATION_RESPONSE_XCD," ",1);
	sprintf(&message[1054],"%s",Cr_Fld_RT.AUTHORIZATION_RESPONSE_XCD);
}
void RT_FALCON_build_request_field_145( )
{
	strncpy(Cr_Fld_RT.SYSTEM_TRACE_NUM,Auth_Tx.TLF01_details.sys_trace_audit_num,6);
	sprintf(&message[1055],"%s",Cr_Fld_RT.SYSTEM_TRACE_NUM);
}

void RT_Falcon_build_CRTRAN_field_31_MC()
{

	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		/*if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))*/

		if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}
		else if((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void RT_Falcon_build_CRTRAN_field_31_VISA()
{

	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
				/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}

		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{

				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void RT_Falcon_build_CRTRAN_field_31_JCB()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void RT_Falcon_build_CRTRAN_field_31_CUP()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	len_billing_currency_code = strlen(Auth_Tx.ch_billing_curr_code);
	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

			if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

			}
			else if((len_billing_currency_code > 0)&&
					((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
					(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3))))
			{
				if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
				}
				else
				{
					strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
				}
			}

			else if (len_billing_currency_code == 0)
			{
				if(strncmp(Auth_Tx.TLF01_details.currency_code,"608",3) == 0)
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
				else if ((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
						(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
		}

		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
		else if ((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}


}

void RT_Falcon_build_CRTRAN_field_31_DINERS()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "nceqit", 5) ||
		   0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "neqit2", 5))
		{
			if(0 == strncmp(Auth_Tx.TLF01_details.currency_code,"608", 3))
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

			}
			else if(0 == strncmp(Auth_Tx.TLF01_details.currency_code,"840", 3))
			{
				if(len_billing_amount > 0 && billing_amount > 0)
				{
					temp_conv_amount = billing_amount *conv_rate;
					Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
					LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
					memset(last2_decimal,0x00,sizeof(last2_decimal));
					strncpy(last2_decimal,str_temp_amount+10,2);

					strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
					if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
					{

					}
					else
					{
						strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
					}
					strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
				}
				else
				{
					strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
				}

			}
		}
		else if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}
		else if((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}

	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void RT_Falcon_build_CRTRAN_field_31_AMEX()
{
	memset(Cr_Fld_RT.TRANSACTION_AMT,0x00,sizeof(Cr_Fld_RT.TRANSACTION_AMT));
	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

			len_settle_currency_code = strlen(Auth_Tx.settlement_curr_code);

			if(len_settle_currency_code == 0)
			{
				if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
				{
					strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
					strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

				}
				else if((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
						(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
					{
						if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
						}
						else
						{
							strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
						}
					}
			}

			else if(strcmp(Auth_Tx.settlement_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}
			else if((strcmp(Auth_Tx.settlement_curr_code,"840") == 0)||
					(0 != strncmp(Auth_Tx.settlement_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}

	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 || billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}


}



void RT_Falcon_build_CRTRAN_field_31_Fleet()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
					strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
				}
				strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
			}
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld_RT.TRANSACTION_AMT,str_temp_amount,10);
			if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
			{

			}
			else
			{
				strncat(Cr_Fld_RT.TRANSACTION_AMT,".",1);
			}
    		strncat(Cr_Fld_RT.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld_RT.TRANSACTION_AMT,"0000000000.00",13);
	}

}

/*void format_date(pCHAR temp)
{
   struct timeb   time_structure;   // Time for ftime
   time_t long_time_since_1970;     // Number of seconds since 01/01/1970 A.D. 12:00 A.M.
   struct tm *current_time;         // Current time structure
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);
   return;
}*/

INT validate_date_yyyymmdd( pBYTE date_str)
{
   INT  ret_val = false;
   INT  imonth, iday, iyear;
   CHAR temp_month[3]="";
   CHAR temp_day[3]  ="";
   CHAR temp_year[5] ="";

   /* Validate date is numeric. */
   if ( (true == isnum(date_str)) && (8 == strlen(date_str)) )
   {
      /* Validate month */
      memcpy( temp_month, date_str+4, 2 );
      imonth = atoi(temp_month);
      if ( (1 <= imonth) && (imonth <= 12) )
      {
         /* Month is valid; check the day. */
         memcpy( temp_day, date_str+6, 2 );
         iday = atoi(temp_day);
         if ( (1 <= iday) && (iday <= 31) )
         {
            if ( iday == 31 )
            {
               if ( (imonth == 1) || (imonth == 3) || (imonth == 5) ||
                    (imonth == 7) || (imonth == 8) || (imonth == 10)||
                    (imonth == 12) )
               {
                  ret_val = true;
               }
            }
            else if ( (iday == 30) && (imonth != 2) )
            {
               ret_val = true;
            }
            else if ( (iday == 29) && (imonth == 2) )
            {
               /* This must be a leap year to be valid. */
               memcpy( temp_year, date_str, 4 );
               iyear = atoi(temp_year);
               if ( (iyear%4) == 0 )
                  ret_val = true;
               /*else
                  sprintf( err_buf, "Too many days for February (%s)",date_str);*/
            }
            else
               ret_val = true;

            /* Validate the year. */
            memcpy( temp_year, date_str, 4 );
            iyear = atoi(temp_year);
            if ( (iyear < 2000) || (iyear > 2200) )
            {
               ret_val = false;
              /*sprintf( err_buf, "ERROR: Invalid date (%s). Bad Year",date_str);*/
            }
         }
         else
         {
            /*sprintf( err_buf, "ERROR: Invalid date (%s). Bad Day.", date_str );*/
         }
      }
      else
      {
         /*sprintf( err_buf, "ERROR: Invalid date (%s). Bad Month.", date_str );*/
      }
   }
   else
   {
     /* sprintf( err_buf, "ERROR: Invalid date (%s).", date_str );*/
   }
   if ( true != ret_val )
      /*strcat(  err_buf, " Must be in format YYYYMMDD." );*/

   return( ret_val );
}
