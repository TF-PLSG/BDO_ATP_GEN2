/****************************************************************************
*  
* MODULE:      Txcntl_constants.h
*  
* TITLE:       
*  
* DESCRIPTION:  
*
* APPLICATION: TXCNTL
*

*******************************************************************************/

#define BDOR_68_REQ_1_VISA 1
#define BDOR_68_REQ_3_VISA 1
#define BDOR_68_REQ_5_VISA 1

#define BDOR_68_REQ_1_MC 1
#define BDOR_68_REQ_3_MC 1
#define BDOR_68_REQ_5_MC 1

#define BDOR_68_REQ_1_AMEX 1
#define BDOR_68_REQ_3_AMEX 1
#define BDOR_68_REQ_5_AMEX 1

#define BDOR_68_REQ_1_JCB 1
#define BDOR_68_REQ_3_JCB 1
#define BDOR_68_REQ_5_JCB 1

#define BDOR_68_REQ_1_UPI 1
#define BDOR_68_REQ_3_UPI 1
#define BDOR_68_REQ_5_UPI 1

#define BDOR_68_REQ_1_DCI 1
#define BDOR_68_REQ_3_DCI 1
#define BDOR_68_REQ_5_DCI 1

#define TRANSACTION_APPROVED         "00"
#define REFER_TO_CARD_ISSUER         "01"
#define RCI_SPECIAL_CONDITION        "02"
#define INVALID_MERCHANT             "03"
#define DO_NOT_HONOR                 "05"
#define INVALID_TRANSACTION          "12"
#define INVALID_AMOUNT               "13"
#define INVALID_ACCOUNT_NUMBER       "14"
#define REENTER_TRANSACTION          "19"
#define LOST_CARD                    "41"
#define NO_SUFFICIENT_FUNDS          "51"
#define EXPIRED_CARD                 "54"
#define INCORRECT_PIN                "55"
#define TXN_NOT_PERMITTED_FOR_CARD   "57"
#define TRANSACTION_NOT_PERMITTED    "58"
#define SUSPECTED_FRAUD				 "59"	/* Visa Secure Samrt Service, Frictionless */
#define EXCEEDS_AMOUNT_LIMIT         "61"
#define EXCEEDS_WITHDRAWAL_LIMIT     "65"
#define PIN_TRIES_EXCEEDED           "75"
#define INCORRECT_CVV                "82"
#define CANNOT_VERIFY_PIN            "86"
#define ACCOUNT_VERIFIED			 "85" /*Account verification.. No reason to decline, Equal to Approve*/
/*#define DESTINATION_UNAVAILABLE    "91"*/
#define DESTINATION_UNAVAILABLE      "01"
#define BDO_VISA_DEBITHOST_UNAVAILABLE "96"  /* oct mand 2022 */
#define SYSTEM_MALFUNCTION           "96"
#define CVV2_FAILURE                 "N7"
#define CVV2_MISMATCH                "63"
#define MC_INC_ARQC_FAILURE		 	 "88"
#define MC_DECLINE_RC           	 "05"
#define CCF02P_STATUS_OK 			 "00"
#define TXN_NOT_PERMITTED_CASH_ADV	 "91"

/* 212 is the actual value and so on for other ECI values, first byte "2" is a fixed value*/
#define ECI_SLI_FULLYAUTHENTICATED "12"
#define ECI_SLI_RECURRING_MIT	   "17"
#define ECI_SLI_MCIDENTITYCHECK_INSIGHTS  "14"
#define ECI_SLI_AUTHENTICATED_ACQUIRERSCAEXEMPTION "16"
#define ECI_SLI_ATTEMPT_AUTHENTICATION  "11"
#define ECI_SLI_UNATHENTICATED          "10"

#define  DEFAULT_MC_3DS2_DELCINE_RC "63"

#define VISA7 1
#ifdef VISA7
#define CATEGORY_CODE_REATTEMPT_REACHED "04"
#endif

#define HCTS2 1

#define NO_PIN_ERROR 1
#define PIN_ERROR 2
#define PIN_SENT_FOR_VERIFICATION 3

#define NO_AUTH_ERROR 0
#define FOUND_BIN_RANGE_IN_SHARED_MEMORY 1
#define SHARED_MEMORY_ERROR_SO_FORWARDING_TO_DATASERVER 2
#define ERROR_BUT_DO_NOT_FORWARD_TO_UPDATE_PROCESSOR 3
#define ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR 4
#define PROCESS_BIN_RANGE_ERROR 5
#define SYSTEM_ERROR 6
#define NO_UPDATER_ERROR 7
#define UPDATER_ERROR 8
#define MID_BIN_BLK_OK 9

#define SYSTEM_PROBLEM_READING_SHARED_MEMORY 10
#define RECORD_NOT_FOUND_IN_SHARED_MEMORY 11
#define RECORD_FOUND_IN_SHARED_MEMORY 12
#define SHARED_MEMORY_READ 13
#define DATABASE_READ 14
#define VALID_EXPIRY_DATE_SPECIFIED 15
#define INVALID_EXPIRY_DATE_SPECIFIED 16
#define INVALID_EXPIRY_DATE_LOCATION_SPECIFIED 17
#define LOCAL_ORIGIN 18
#define REMOTE_ORIGIN 19
#define FLEET_CARD 20

/*Queue Status */
#define QUE_OFFLINE				-1001


/* position in track2 */
#define TRACK2_ISSUER_IDENTIFIER_POS		1-6
#define TRACK2_ACCOUNT_NUMBER_POS			7-16
#define TRACK2_CHECK_DIGIT_POS				17
#define TRACK2_FIELD_SEPARATOR_POS			18
#define TRACK2_EXPIRY_DATE_POS				19-22
#define TRACK2_INTERCHANGE_DESIGNATOR_POS	23
#define TRACK2_SERVICE_CODE1_POS			24
#define TRACK2_SERVICE_CODE2_POS			25
#define TRACK2_PIN_OFFSET_POS				26-29
#define TRACK2_LANGUAGE_CODE				30
#define TRACK2_FORMAT_CODE_POS				31
#define TRACK2_CARD_SUBTYPE_POS				32
#define TRACK2_INFORMATION_REQUIREMENT_POS	33
#define TRACK2_TERMINAL_TYPE_POS			34
#define TRACK2_TYPE_OF_VEHICLE_POS			35
#define TRACK2_TIME_RESTRICTION_POS			36
#define TRACK2_PIN_INDICATOR_POS			37	
		

#define REJECT_IF_NOT_PRESENT		'0'
#define REJECT_IF_PRESENT			'1'

/* device_type */
#define CRIND          "01"
#define EDC            "02"
#define EFTPOS_ISO     "03"
#define EFTPOS_VISA_1  "04"
#define VAT            "05"
#define CAT            "06"

/* authorization_method in CAF01 */
#define POSITIVE_FILE	"P"
#define NEGATIVE_FILE	"N"
#define POSITIVE_WITH_BALANCE_FILE "B"
#define POSITIVE_WITH_PRIMARY_BALANCE_FILE "D"
#define NEGATIVE_WITH_OFFUS_VELOCITY "O"
#define OFFUS_VELOCITY "V"

/* authorization location in CAF01 */
#define LOCAL_AUTHORIZATION "L"
#define REMOTE_AUTHORIZATION "R"
#define CAF_AUTHORIZE_LOCALLY_AND_SEND_TO_REMOTE "O"
#define CAF_AUTHORIZE_LOCALLY_AND_SEND_TO_REMOTE_ONLY_IF_TX_IS_ACCEPTED "A"
#define CAF_AUTHORIZE_LOCALLY_AND_SEND_TO_REMOTE_ONLY_IF_TX_IS_DECLINED "D"

/* credit/fleet */
#define CREDIT_PURCHASE			0
#define CREDIT_CASH_ADVANCE		1
#define CREDIT_RETURN			2
#define	CREDIT_INQUIRY			3
#define CREDIT_PAYMENT			4

#define OFFUS		0
#define ONUS		1


/* debit */
#define DEBIT_PURCHASE			5
#define DEBIT_CASH_WITHDRAWAL	6
#define DEBIT_RETURN			7
#define	DEBIT_INQUIRY			8
#define DEBIT_DEPOSIT			9
#define BALANCE_TRANSFER		10

/*loyalty */
#define STORED_VALUE_PURCHASE	11
#define STORED_VALUE_RETURN		12
#define STORED_VALUE_INQUIRY	13
#define STORED_VALUE_LOAD		14
#define STORED_VALUE_UNLOAD		15

/* Card Types */
#define TYPE_CREDIT    'C'
#define TYPE_DEBIT     'D'
#define TYPE_DEFERRED  'E'

/* RSB Guarantor Card */
#define RSB_CARD   'G'
#define RSB_FLAG   "RS"

/* 3rd digit of the processing code */
#define DEFAULT_ACCOUNT "00"
#define SAVINGS_ACCOUNT "01"
#define CHECKING_ACCOUNT "02"
#define CREDIT_FACILITY "03"
#define UNIVERSAL_ACCOUNT "04"

/* card status */
#define CARD_ACTIVE_STATUS "0"
#define CARD_LOCKED_STATUS	"1"
#define CARD_LOCKED_PERMANENT_STATUS	"2"
#define CARD_LOCKED_TEMPORARY_STATUS	"3"

/* merchant status */
#define MERCHANT_ACTIVE_STATUS "A"
#define MERCHANT_INACTIVE_STATUS "I"
#define MERCHANT_RESTRICTED_STATUS "R"

/* caf status */
#define CAFPROFILE_ACTIVE_STATUS "A"

/* account status */
#define ACCOUNT_ACTIVE_STATUS "A"

/* processing code */
#define AUTH_AFT				"10"
#define AUTH_OCT				"26"
#define AUTH_CASH_ADV			"01"

/* currency codes */
#define MYR "458"
#define SGD "702"
#define PESO_CURRENCY "608"
#define USDOLLAR_CURRENCY "840"
#define RUB_CURRENCY_CODE    "643"
#define SIERRA_LEONE_CURRENCY    "925"
#define ICE_KRONA_CURRENCY		 "352"
#define CURRENCY_LEN 3

/* Related Account Types */
#define DOLLAR_ACCOUNT_TYPE       "05"
#define PESO_ACCOUNT_TYPE         "06"
#define DEFERRED_ACCOUNT_TYPE     "07"
#define COUNTRY_CLUB_ACCOUNT_TYPE "08"

#define cDOLLAR_ACCT_TYPE          '5'
#define cPESO_ACCT_TYPE            '6'
#define cDEFERRED_ACCT_TYPE        '7'
#define cCOUNTRY_CLUB_ACCT_TYPE    '8'
/* End Related Account Types */


#define EXPIRATION_DATE_FROM_CARD           '0'
#define EXPIRATION_DATE_FROM_CARDHOLDER     '1'
#define EXPIRATION_DATE_FROM_BOTH           '2'
#define EXPIRATION_DATE_FROM_BOTH_REVERSED  '3'

/* Expiration Date Check Options */
#define CHECK_EXPIRY_NO       'N'
#define CHECK_EXPIRY_YES      'Y'
#define CHECK_EXPIRY_EXPANDED 'E'


#define BALANCE_ACCOUNT_TYPE_IS_DEFERRED 0
#define BALANCE_ACCOUNT_TYPE_IS_OTHER    1

#define sREMOTE_PROCESSING_INDICATOR "0"
#define REMOTE_PROCESSING_INDICATOR 0
#define LOCAL_PROCESSING_INDICATOR  1

#define PIN_VERIFICATION 0
#define MAG_STRIP_VERIFICATION 1
#define NSP_VERIFY_ARQC 2
#define NSP_VERIFY_3D_SECURE 3
#define NSP_CUP_ECOM_CVN2    4
#define MAX_DATASERVER_BUFFER_LENGTH 25000

#define TRANSACTION_WITHIN_FLOOR_LIMIT 0
#define TRANSACTION_OUTSIDE_FLOOR_LIMIT 1

/* Visa Electron */
#define VISA_ELECTRON_BIN         "455968"
#define VISA_ELECTRON_SERVICE_CODE   "126"
#define MAX_CARD_NUM_LEN               19
#define MAX_CARD_HOLDER_LEN            45

/* Response Texts for RPS */
#define O2_MAX_TXNS         "02-Exceeds Max Txn Limit for Merchant"
#define O2_MAX_REF          "02-Exceeds Max Refund Limit for Merchant"
#define O2_INSUFFIENT_FUNDS "02-Insufficient Funds"
#define O2_DEF_BAL_FAIL     "02-Deferred Balance Limit Failure"
#define O2_CARD_USE_FAIL    "02-Card Usage Check Failure"
#define O2_CAT_USE_FAIL     "02-Category Usage Check Failure"
#define O2_NO_BAL_ACCT      "02-Balance Account not found"

#define O3_NO_EXP_DATE      "03-Missing Expiration Date"
#define O3_EXPD_CARD        "03-Expired Card"
#define O3_INV_EXP_DATE     "03-Invalid Expiration Date"
#define O3_HOT_CARD         "03-Hot Card"
#define O3_LOST_CARD        "03-Lost Card"
#define O3_INV_CARD_STAT    "03-Invalid Card Status"
#define O3_INV_PRIM_ACCT    "03-Invalid Primary Account Status"
#define O3_INV_BAL_ACCT     "03-Invalid Balance Account Status"
#define O3_INV_PROF_STAT    "03-Invalid Profile Status"

#define MCC_ATM				"6011" /*manual cash disbursements*/


/* Fraudguard Functionality per Card Family */
#define FG_STATE_ACQ_AMEX   0x01
#define FG_STATE_ACQ_JCB    0x02
#define FG_STATE_ACQ_MC     0x04
#define FG_STATE_ACQ_VISA   0x08

#define FG_STATE_ISS_AMEX   0x10
#define FG_STATE_ISS_JCB    0x20
#define FG_STATE_ISS_MC     0x40
#define FG_STATE_ISS_VISA   0x80

/* Card Family Definitions for use with FraudGuard routing. */
#define CARD_FAMILY_AMEX  '0'
#define CARD_FAMILY_VISA  '1'
#define CARD_FAMILY_MC    '2'
#define CARD_FAMILY_JCB   '3'

/* Max number of NCF01 records. */
#define MAX_NCF01_RECS    20

/*POS DATA for MC VISA AMEX JCB CUP */
#define DE60_1_SIZE    4
#define DE61_2_SIZE    11
#define MESSAGE_REPONSE_CODE_FEILD_60_1 "0000"
#define MESSAGE_REPONSE_CODE_FEILD_60_1_LEN 4
#define DE22_SIZE_AMEX 12
#define POS_ENTRY_NORMAL         0
#define POS_ENTRY_EMV_CHIP       1
#define POS_ENTRY_EMV_MAGSTRIPE  2
#define POS_ENTRY_EMV_FALLBACK   3
#define POS_ENTRY_FALLBACK   	 4
#define  SERVICE_CODE_SIZE_PLUS_1        4
#define  ECOM_POS_CC   "59"
#define  ECOM_POS_ENTRYMODE "001"
#define  ECOM_POS_CC_VISA   "51"
#define  VISA_CAVV_USAGE_3_POS    4
#define  VISA_CAVV_VALIDATION_KEY1						"10"
#define  VISA_CAVV_VALIDATION_KEY2						"11"
#define  VISA_CAVV_VALIDATION_KEY_LEN					2
#define  VISA_CAVV_VALIDATION_RESULT_CODE_LEN  			1
#define  VISA_CAVV_SUCCESS_RESULT_CODE1					'8'
#define  VISA_CAVV_SUCCESS_RESULT_CODE2					'A'
#define  VISA_CAVV_PASS_VALIDATION_AUTH                 '2'
#define  VISA_CAVV_PASS_VALIDATION_ATTEMPT              '3'
#define  VISA_CAVV_PASS_VALIDATION_INFO					'B'
#define  VISA_CAVV_FAILED_RESULT_CODE1					'7'
#define  VISA_CAVV_FAILED_RESULT_CODE2					'9'
#define  VISA_CAVV_AUTH_RESULT_INVALID					'0'
#define  VISA_CAVV_AUTH_FAILED							'1'
#define  VISA_CAVV_FAILED_VALIDATION_ATMP				'4'
#define  VISA_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION	'6'
#define  VISA_CAVV_WAS_NOT_VALIDATE_ATTEMPT             'C'
#define  VISA_CAVV_WAS_NOT_VALIDATED					'D'

#define  AMEX_AEVV_VALIDATION_RESULT_CODE_LEN			 1
#define  AMEX_AEVV_SUCCESS_RESULT_CODE					'0'
#define	 AMEX_AEVV_AUTHN_NOT_PERFORMED					'5'
#define  AMEX_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION  '7'
#define  AMEX_AEVV_AUTN_ATTEMPT_ACS_NOT_AVAILABLE		'8'
#define  AMEX_AEVV_AUTHN_REJECT_BY_ISS					'9'
#define  AMEX_AEVV_FULLY_AUTHENTICATION                 '0'


#define JCB_TRA_EXPMT_CLAIMED                         '2'
#define JCB_DATA_SHARE_ONLY                           '3'
#define JCB_STRONG_CUST_AUTH_PERFORMED                         '4'
#define JCB_CARD_AUTHN_NOTDONE                         '5'
#define JCB_CARD_AUTHN_FAILED                        '9'


#define JCB_3DS_AUTHENTICATION_RESULT_POS    '0'
#define JCB_SECOND_FACTOR_AUTHENTICATION_POS     '2'
#define JCB_CAVV_KEY_INDICATOR_POS     '4'
#define JCB_3DS_AUTHENTICATION_RESULT_LEN     2
#define JCB_SECOND_FACTOR_AUTHENTICATION_LEN  2

#define  DCI_3DS2_CAVV_VALIDATION_RESULT_CODE_LEN			 2
#define  DCI_3DS2_CAVV_SUCCESSFULL_AUTHN					"00"
#define  DCI_3DS2_CAVV_AUTHN_NOT_PERFORMED					"05"
#define  DCI_3DS2_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION	"07"
#define	 DCI_3DS2_CAVV_AUTHN_ATTEMPT_ACS_NOT_AVAILABLE		"08"
#define  DCI_3DS2_CAVV_AUTHN_FAILED							"09"

#define CUP_AV_AUTHENTICATION_RESULT_POS                   '1'
#define CUP_AV_AUTHENTICATION_RESULT_LEN                    2
#define CUP_AV_GENERATOR_INDICATOR_POS                     '3'
#define CUP_DE61_CVN2_POS                                   27
#define CUP_CVN2_INDICATOR_POS								5




#define ID_MOBILE_POS_ACCP_DEVICES 9 /*Identification of Mobile POS Acceptance Devices*/
#define  AUTO_FUEL_MCC   "5542" /* Automated fuel dispenser Merchant cat code */
#define  MCC_RANGE2_LOW   5960
#define  MCC_RANGE2_HIGH  5969

/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

/*Card Type*/
#define CUP_CARD_NUM   "6"

#define VOICE_HANDLE_QUEUE_NAME                   5

#define CUP_DE23_SIZE 3
#define CUP_DE22_SIZE 3
#define AMEX_DE22_SIZE 12

#define  MC_FIELD48_MAX_LEN  100
#define  MC_DE48_TAG63    "6315"

/*AMEX ACQ POS data changes*/

#define AMEX_POS_DATA_LEN													12

#define AMEX_MAGSTRIP_POS_DATA_WITH_PIN										"560101254041"
#define AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN									"560101254041"

#define AMEX_CHIP_POS_DATA_WITH_PIN											"560101554041"
#define AMEX_CHIP_POS_DATA_WITHOUT_PIN										"560101554041"

#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN								"56010X554041"
#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN							"56010X554041"

#define AMEX_FALLBACK_POS_DATA_WITH_PIN										"560101954041"
#define AMEX_FALLBACK_POS_DATA_WITHOUT_PIN									"560101954041"

#define AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN						"56010X254041"

#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN								"560110664041"
#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN							"560110664041"

#define AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA									"100SS0S00110"
#define AMEX_RPS_ASIAPAY_RPS_POS_DATA										"600090100110"
#define AMEX_BDO_PAY_POS_DATA												"660400600130"

#define AMEX_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA								"56010X554041"
#define AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN					"360101354140"
#define AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN				"360101354140"
#define AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"100SS0S00110"


#define MC_REFUND_RC		"85"
#define VISA_REFUND_RC		"00"
#define AMEX_REFUND_RC		"00"
#define CUP_ATM_CASH_ADVANCE_RC		"05"

/*Macro for JCB 3DS 2*/
#define JCB_3DS2 1

#define CARD_NOT_PRESENT_MC_DE61_SE5 '1'

/*New 11.10.2 defination*/

#define ISO_MESSAGE_TYPE_LEN												4
#define POS_ACQ_SALE_MESSAGE_TYPE 											"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 											"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 										"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE								"0420"
#define POS_ACQ_SALE_COMPLETION_MESSAGE_TYPE								"0220"

#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN									3
#define BDO_POS_ACQ_TXN_HANDLER_QUEUE										"dcp"

#define POS_ENTRY_MODE_LEN 							    					4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 							"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 						"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM						"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM					"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM				"0012"

#define POS_CONDITION_CODE_LEN												2
#define POS_ECOM_CONDITION_CODE												"59"

#define VISA_POS_DATA_LEN													12
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN				 		"980000100010"
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN				 	"980000100010"
#define VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA							"030000000010"
#define VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"510000000740"

#define MC_POS_DATA_LEN														16
#define MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA							"0000000000300608"
#define MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA							"1025100006600608"
#define MC_CARDHOLDER_ACTIVATED_TERMINAL_LEVEL								'9'

#define JCB_POS_DATA_LEN													6
#define JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA								"222608"
#define JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA							"222608"
#define JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA							"122608"

#define UPI_POS_DATA_LEN													11
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA				 		"06002300000"
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA					"06002300000"
#define UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA							"06000800000"
#define UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA							"00000810101"

#define DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN 						"960101S51141"
#define DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN 					"960101S51140"
#define DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN 					"360101354141"
#define DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN 				"360101354140"
#define DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA 						"660490195110"

/***********************************************************************************/
