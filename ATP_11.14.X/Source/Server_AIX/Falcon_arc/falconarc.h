/*********************************************************************************

                FALCON ARCHIVE UTILITY - FALCON_ARCHIVE - UTILITY - THOUGHTFOCUS 

/********************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  

/*Card Type*/
#define CUP_CARD_NUM   "6"
struct
{
	char    HeaderFile                      [69];
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
}Cr_Fld;

struct {

char      AUTH_SEQ_ID						[20];
char    ACCOUNT_NUMBER						[16];        
char    AUTH_ACCT_NUM						[20];
char    AUTH_AUTH_POST_FLAG					;		
char	    AUTH_EXP_DATE        			[10] ;
char   		 AUTH_CNTRY_CODE				[4]	;
char    AUTH_CARD_TYPE						;
char    AUTH_CARD_ASSOC						;
char    AUTH_TYPE							;
char    		AUTH_MCC					[5];
long	    AUTH_AMT						[13];
char    AUTH_CRNCY_CODE						[4];
int	    AUTH_CRNCY_RATE						;
char	    AUTH_CREATE_DATE				[25];
char        AUTH_CREATE_TIME				[25];
char	    AUTH_DATE						[25];
char        AUTH_TIME						[25];
char    AUTH_CATEGORY						;
char    AUTH_TERM_ID						[41];
char    AUTH_POS_CONDITION					[3];
char    AUTH_TERM_TYPE						;
char    AUTH_TERM_CAPAB						;
char    AUTH_POS_ENTRY						;
char    AUTH_CVV							;
char    AUTH_AVS							;
char    AUTH_CARD_MEDIA_TYPE				;
char    AUTH_CVV2_PRESENT					;
char    AUTH_CVV2_RESPONSE					;
char    AUTH_AVS_RESPONSE					;
char    AUTH_PIN_VER						;
char    AUTH_ACQ_REF_NUM					[24];
char    AUTH_FALCON_SCORE					[5];
char    AUTH_EXTERNAL_ID					[31];
char    AUTH_AUTH_DEC						;	
char    	   AUTH_BATCH_ID				[13];
char    AUTH_USER_IND_2						;
char    AUTH_USER_IND_3						;
char    AUTH_USER_IND_4						;
char    AUTH_USER_IND_5						;
char    AUTH_USER_SHORT_1					[16];
char    AUTH_USER_SHORT_2					[16];
char    AUTH_USER_SHORT_3					[16];
char    AUTH_USER_LONG_1					[41];	
char    AUTH_USER_LONG_2					[41];
char    AUTH_USER_LONG_3					[41];
int	    AUTH_USER_NUM_1						;
int	    AUTH_USER_NUM_2						;
int      AUTH_USER_NUM_3					;
char       AUTH_USER_DATE_1					[10];
char       AUTH_USER_DATE_2					[10];
char       AUTH_USER_DATE_3					[10];
char       MRA_TXN_START_TIME				[25];
char       MRA_TXN_END_TIME					[25];
char    MRA_RESULT_FLAG						;

}Mra_fld;






/*** Function Prototypes ***/
int MainProcessor(void);
void ProcessTransactions(void);
BOOLEAN *write_to_falcon_archive_file(char *);
BYTE db_get_tlf01_details_same_Date(pTLF01 tlf01_ptr,pCHAR fromdate,pCHAR todate,pCHAR fromtime,pCHAR totime,pCHAR ErrorMsg );
BYTE db_get_tlf01_details_diff_Date(pTLF01 tlf01_ptr,pCHAR fromdate,pCHAR todate,pCHAR fromtime,pCHAR totime,pCHAR ErrorMsg );
void RqMssg(void);
void get_time( pCHAR time_date, pCHAR time_str );
void get_date( pCHAR time_date, pCHAR date_str );
