/*********************************************************************************

                MERCHBATCH - EFTPOS - UTILITY - THOUGHTFOCUS 

********************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  

#define MC02_APR2024 1

#define SWIPE_CARD   "0022"
#define MANUAL_ENTRY "0012"


/* ThoughtFocus - Chandra */

/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

#define BDOR_68_REQ_2_VISA 1
#define BDOR_68_REQ_4_VISA 1
#define BDOR_68_REQ_6_VISA 1

#define BDOR_68_REQ_2_MC 1
#define BDOR_68_REQ_4_MC 1
#define BDOR_68_REQ_6_MC 1

#define BDOR_68_REQ_2_JCB 1
#define BDOR_68_REQ_4_JCB 1
#define BDOR_68_REQ_6_JCB 1

#define BDOR_68_REQ_2_DCI 1
#define BDOR_68_REQ_4_DCI 1
#define BDOR_68_REQ_6_DCI 1

#define BDOR_68_REQ_2_UPI 1
#define BDOR_68_REQ_4_UPI 1
#define BDOR_68_REQ_6_UPI 1

#define BDOR_68_REQ_2_AMEX 1
#define BDOR_68_REQ_4_AMEX 1
#define BDOR_68_REQ_6_AMEX 1

#define POS_ENTRY_MODE_LEN												4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM						"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM					"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM					"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM				"0032"
#define POS_ENTRY_MODE_FOR_ECOM_TXN										"0012"

#define POS_CONDITION_CODE_LEN											2
#define POS_ECOM_POS_CONDITION_CODE   									"59"
/* Length for printing Coustmer identifier */
#define PERCH_ID_LENGTH 32
#define CUSTOM_IDENTIFIER_TYPE_LENGTH 6 /*Custom Identifier Type*/
#define EPOS_POS_DATA_SIZE 12

/* Duplicate Extraction flags - TF Phani*/
#define ALLOW_DUPLICATES		0
#define DONT_ALLOW_DUPLICATES	1

#define  TF_INI_FILE   "tf.ini"

/*Visa EPOS Defineation*/
#define VISA_POS_DATA_LEN_FOR_EPOS									 	10
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA 				"072980000"
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA 				"072980000"
#define VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA	            "032030000"
#define VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA	        "032030000"
#define VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA					"012510007"

/*MC EPOS Defineation*/
#define MC_POS_DATA_LEN_FOR_EPOS		 								12
#define MC_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA 					"073000019300"
#define MC_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA 				"072000019300"
#define MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA 				"031000000300"
#define MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA 			"032000000300"
#define MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"812125106600"

/*JCB EPOS Defineation*/
#define JCB_POS_DATA_LEN_FOR_EPOS	 									10
#define JCB_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA 					"072000022  "
#define JCB_MOBILE_NFC_TAP2PHONE_TXN_TXN_WITHOUT_PIN_POS_DATA 			"072000022  "
#define JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA 				"012000022  "
#define JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA 			"012000022  "
#define JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"812000012  "

/*DCI EPOS Defineation*/
#define DCI_POS_DATA_LEN_FOR_EPOS		 								12
#define DCI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA 					"960101S51141"
#define DCI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA 				"960101S51140"
#define DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA 				"360101354141"
#define DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA 			"360101354140"
#define DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"660490195110"

/*UPI EPOS Defineation*/

#define CUP_MAGAGSTRIPE_SWIPE 		"05000300000"
#define CUP_CHIP_CONTACT     		"05000300000"
#define CUP_CHIP_CONTACTLESS		"06000300000"
#define CUP_CHIP_FALLBACK			"05200300000"
#define CUP_POS_MANUAL_KEYIN		"05000300000"
#define CUP_RPS_ASIAPAY_RECURRING   "90001400000"
#define CUP_BDO_PAY					"00000800000"
#define CUP_EPOS_POS_DATA_LEN 		11

#define UPI_POS_DATA_LEN_FOR_EPOS		 								12
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA 					"06002300000"
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA 				"06002300000"
#define UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA 				"06000800000"
#define UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA 			"06000800000"
#define UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"00000810101"

/*AMEX EPOS Defineation*/
#define AMEX_MAGSTRIP_POS_DATA_WITH_PIN									"560101254041"
#define AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN								"560101254041"

#define AMEX_CHIP_POS_DATA_WITH_PIN										"560101554041"
#define AMEX_CHIP_POS_DATA_WITHOUT_PIN									"560101554041"

#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN							"56010X554041"
#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN						"56010X554041"

#define AMEX_FALLBACK_POS_DATA_WITH_PIN									"560101954041"
#define AMEX_FALLBACK_POS_DATA_WITHOUT_PIN								"560101954041"

#define AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN					"56010X254041"

#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN							"560110664041"
#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN						"560110664041"

#define AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA								"100SS0S00110"
#define AMEX_RPS_ASIAPAY_RPS_POS_DATA									"600090100110"
#define AMEX_BDO_PAY_POS_DATA											"660400600130"

#define AMEX_POS_DATA_LEN_FOR_EPOS		 								12
#define AMEX_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA 				"56010X554041"
#define AMEX_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PINPOS_DATA 				"56010X554041"
#define AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA				"360101354140"
#define AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PINPOS_DATA			"360101354140"
#define AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA					"100SS0S00110"

/*** Common structure for current terminal batch ***/

typedef struct
{
   CHAR batch_number                      [7];
   CHAR terminal_id                       [9];
   CHAR merchant_id                       [16];
   CHAR open_date                         [9];
   CHAR open_time                         [7];
   CHAR number_credit_tran_for_batch      [11];
   CHAR number_debit_tran_for_batch       [11];
   CHAR totals_credit_for_batch           [16];
   CHAR totals_debit_for_batch            [16];

     
}  TC_TOTALS ;

typedef struct
{
   BYTE  num_returned     [5];
   EMV_RAW batch_list       [BATCH_SERVICE_LIST_SIZE];
}EMV_LIST,*pEMV_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   TLF01 batch_list       [BATCH_SERVICE_LIST_SIZE];
}TLOG01_LIST,*pTLOG01_LIST;


/*** Function Prototypes ***/
extern BYTE db_set_rollback_segment_online(pCHAR );
extern BYTE db_set_rollback_segment_offline(pCHAR );

void 	MainProcessor(void);
LONG 	calc_bonus_amount( pBON03_LIST bon03_ptr );
void 	calc_julian_day(char date[], char julian_str[]);
INT  	create_credit_detail(void);
INT  	create_credit_detail_arc(void);
INT  	create_debit_detail(void);
INT  	create_debit_detail_arc(void);
void 	create_headers(void);
void 	create_trailers(void);
BOOLEAN bch01_handler(void);
BOOLEAN create_settle_files(pCHAR merchant_id);
void 	display_settle_info(void);
void 	EndProcess(void);
BOOLEAN get_currency_code(void);
BOOLEAN open_credit_file(void);
BOOLEAN open_debit_file(void);
BOOLEAN open_empty_raffle_file(void);
void 	returnCurrentTime(char *currentTime);
void	Rj_with_lead_zeros(pCHAR str, INT FieldWidth);
void 	set_date(void);
BOOLEAN write_to_CUP_credit_settle_file(void);
BOOLEAN write_to_credit_settle_file(void);
BOOLEAN write_to_debit_settle_file(void);
void 	Get_duplicate_restrict_flag_from_ini( void );
BYTE 	db_update_bch01_batch_Status_After_Writing_File(pBCH01 bch01_ptr, pCHAR ErrorMsg);
BYTE 	db_get_tlf_log_service_list( pTLF01 tlf01_ptr,pTLOG01_LIST tlog01_list, pCHAR ErrorMsg );
void    write_to_dump_file(CHAR *);
void   get_reload_confirm( pBCH10 bch10_ptr );
void    get_date( pCHAR time_date, pCHAR date_str );
INT     db_process_bch10_records( pCHAR ErrorMsg );
INT     db_insert_bch11_delete_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg );
BYTE    db_select_curr_code( pCHAR   curr_code, pCHAR  device_type, 
                             pDCF01  dcf01_ptr, pCHAR CUP_Merchaant_ID,pCHAR  ErrorMsg );
BYTE 	db_get_bon03_service_list( pCHAR tran_id, pBON03_LIST bon03_list, pCHAR ErrorMsg );
BYTE 	db_get_emv_service_list( pBCH10 bch10_ptr, pEMV_LIST Emv_List,pCHAR ErrorMsg );
BYTE 	db_get_emv_arc_service_list( pBCH11 bch11_ptr, pEMV_LIST Emv_List,pCHAR ErrorMsg );
BYTE 	db_get_bch01_service_list1( pBCH01 bch01_ptr,pBCH01_LIST Bch01_List,pCHAR ErrorMsg );
void 	parseEmvDataToSegment(INT elength,  CHAR emv_seg13[], CHAR emv_seg14[], CHAR emv_seg15[]);
void 	Get_Epos_Exception_CUP_flag_from_ini( void );
void 	Get_EPOS_Settle_percentage_Value_CUP_from_ini( void );
void 	Get_Incremental_Auth_flag_CUP_from_ini( void );
BOOLEAN write_to_CUP_Exception_file(void);
BOOLEAN open_exception_file(void);
BOOLEAN create_write_exception_headers(void);
void 	Strrev( char * string);
void 	trimleadingZero(char * string1, char * string2);
void 	formatAmount(char * string1, char * string2);
BOOLEAN create_trailer_exception_headers(void);
BOOLEAN write_to_CUP_Exception_file_arc(void);
void 	merchbatch_get_error_warning_file_name_path(void );
void 	merchbatch_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT 	merchbatch_Log_error_warning_to_File(pCHAR Error_Buf,int sev,  pCHAR func,int detail);
void 	merchbatch_create_Error_Filename(  );

