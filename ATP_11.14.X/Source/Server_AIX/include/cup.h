/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup_constants.h
 *  
 * TITLE:       BDO Cadencie Host Network Control Macros and Constants
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the network controller.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Abhishek Verma

 ****************************************************************************/

#ifndef CUP_CONSTANTS_H
#define CUP_CONSTANTS_H

/* Network Types */
#define   CUP_ISSUER   1
#define   CUP_ACQUIRER 2
#define   CUP_BOTH     3

/* Currency Codes */
#define  CUP_PESO     "608"
#define  CUP_USD      "840"

/* Related Account Types */
#define CUP_DOLLAR_ACCOUNT_TYPE  '5'
#define CUP_PESO_ACCOUNT_TYPE    '6'

/* TRANSACTION SOURCE */
#define   CUP_HOST  1
#define   CUP_ATP   2

#define BDOR_68_REQ_1_UPI 1
#define BDOR_68_REQ_3_UPI 1
#define BDOR_68_REQ_5_UPI 1

/* Response/Error Codes */
#define   CUP_RESPONSEP_CODE_LEN             2
#define   CUP_TOO_MANY_ACTIVE_TRANSACTIONS   "96"
#define   CUP_TOO_MANY_TIMEOUTS              "96"
#define   CUP_HOST_UNAVAILABLE               "91"
#define   CUP_ISSUER_INOPERATIVE             "96"
#define   CUP_SYSTEM_ERROR                   "96"
#define   ISO_TO                             "19"
#define   CUP_TIMEOUT                   	 "68"
#define   CUP_INVALID_TXN                    "58"
#define   CUP_SECURITY_FAILURE               "55"
#define   CUP_AMX_INVALID_TXN                "12"
#define   CUP_FORMAT_ERROR                   "30"
#define   CUP_APPROVED                       "00"
#define   CUP_NETMGMT_APPROVED               "800"
#define   CUP_FIELD_EDIT_ERROR               "304"
#define   CUP_AMX_HOST_UNAVAILABLE           "912"
#define   CUP_RACAL_APPROVED                 "00"
#define   CUP_ISO_APPROVED                   "00"
#define   CUP_KEY_TRANSLATION_ERROR      	 "A7"
#define   CUP_CARD_HAS_NO_MOBILE_NUM	     "P1"
#define   CUP_MOBILE_VALIDATION_FALED        "05"
#define   CUP_BALANCE_INQ_WITHOUT_MOBILE     "01"
#define   CUP_DO_NOT_HONOR                   "05"

#define AVN_INDICATOR_POS                   10
#define CVN2_INDICATOR_POS                  5
#define CVN2_VALUE_POS                      27
#define CVN2_VALUE_LEN                      3

#define CUP_AV_AUTHENTICATION_RESULT_POS                   0
#define CUP_AV_AUTHENTICATION_RESULT_LEN                   2
#define CUP_AV_GENERATOR_INDICATOR_POS                     4

#define CUP_DE125_TAG01    "380136"
#define CUP_DE125_TAG01LEN   6

#define CUP_DE55_HDR_LEN	4
#define CUP_DE55_HDR_VER	2
#define CUP_EMV_MAX_LEN			   255

/* Transaction Flow */
#define   SWITCHED_IN    0
#define   SWITCHED_OUT   1

/* 0800 Request Types */
#define   CUP_LOGON         0
#define   CUP_LOGOFF        1
#define   CUP_ECHO          2
#define   CUP_KEY_EXCHANGE  3

/* 1800 function codes for DE24 */
#define   CUP_NMI_LOGON                 "001"
#define   CUP_NMI_LOGOFF                "002"
#define   CUP_NMI_ECHO                  "301"
#define   CUP_NMI_KEYCHANGE             "101"
#define   CUP_NMI_CUTOFF                "201"
#define   CUP_NMI_SAF                   "360"
#define   CUP_NMI_STR_DATE_SWITCH       "201"
#define   CUP_NMI_END_DATE_SWITCH       "202"
#define   CUP_STND_IN_AUTH_ECHO         "401"
#define   CUP_STND_IN_AUTH_TRA_ACT      "501"
#define   CUP_STND_IN_PARA_UPD          "902"


/* Function codes ( DE24 ) for financial transaction */
#define   CUP_AUTHORIZATION_FUNC_CD		"100"
#define   CUP_INQUIRY_FUNC_CD			"108"
#define   CUP_CASHSALE_FUNC_CD			"200"
#define   CUP_REVERSAL_FUNC_CD			"400"
#define   CUP_PARTIALREVERSAL_FUNC_CD	"401"


/* Echo Timer Set/Reset Values */
#define   CUP_ECHO_TIMER_IS_SET      1
#define   CUP_ECHO_TIMER_IS_CLEARED  0

/* Key Change Flags */
#define   CUP_KEY_CHANGE_ENABLED    'E'

/* Structure Sizes */
#define   AUTH_TX_SIZE         sizeof(AUTH_TX)
#define   NCF01_SIZE           sizeof(NCF01)
#define   NCF30_SIZE           sizeof(NCF30)
#define   ATM01_SIZE           sizeof(ATM01)
#define   TLF01_SIZE           sizeof(TLF01)
#define   CRF01_SIZE           sizeof(CRF01)
#define   ONLINE_UPDATES_SIZE  sizeof(AMEX_ONLINE_UPDATES)

/* Message Sizes */
#define   CUP_MAX_HOST_MSG_SIZE    750
#define   CUP_MAX_APPL_MSG_SIZE  20000

/* Cash Advance Types for field 102. */
#define  ECN_VISA        'A'
#define  OFF_US_VISA     'B'  /* Credit */
#define  OFF_US_MC       'C'
#define  ECN_MC          'D'
#define  CIRRUS          'E'
#define  OFF_US_PLUS_CA  'F'  /* Current/Check */
#define  OFF_US_PLUS_SA  'G'  /* Savings       */
#define  ECN_JCB         'H'
#define  OFF_US_JCB      'I'
#define  ECN_AMEX        'J'
#define  OFF_US_AMEX     'K'

/* Processing Code Account Types (From) */
#define  CUP_PROC_CODE_SAVINGS  '1'
#define  CUP_PROC_CODE_CURRENT  '2'
#define  CUP_PROC_CODE_CREDIT   '3'
#define  CUP_PROC_CODE_UNKNOWN  '0'

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

/* TIMER TYPES */
#define  REQUEST_TIMER  0
#define  ECHO_TIMER     1

/* TIMER APPS */
#define  TIMER_REVERSAL   "Reversal"
#define  TIMER_FINANCIAL  "Financial"
#define  TIMER_08X0       "0800"
#define  TIMER_ECHO       "Echo"

/* Shared Memory Tables */
#define  CRF01_TABLE    "Crf01Table"

/* Flags used when sending responses to host */
#define  SEND_UNCONDITIONALLY  0
#define  SEND_IF_NOT_REVERSAL  1

/* Track Data Field Separators */
#define  TRK1_FS     '^'
#define  TRK2_FS     '='

/* Miscellaneous */
#define  COMM_BUF_SZE            1500
#define  EXTENDED_BITFIELD       128
#define  NORMAL_BITFIELD         64
#define  ATM_POS_ENTRY_MODE      "021"
#define  LOOKUP_KEY_SIZE         22

#define CUP_ISSUING_MODULE_COMP_LEN			5
#define CUP_POS_HANDLER_MODULE_COMP_LEN		3
#define CUP_VOICE_MODULE_COMP_LEN			5
#define CUP_SBATCH_MODULE_COMP_LEN			4
#define CUP_CANDENCIE_LINK_QUEUE_NAME_LEN	6

/* Debug Macros */
#ifdef _DEBUG
   #define PRINT( x ) printf( x )
#else
   #define PRINT( x )
#endif

#ifdef _DEBUG
   #define PRINT2( x, y ) printf( x, y )
#else
   #define PRINT2( x, y )
#endif

/* Bit Field Sizes */
#define CUP_DE2_LEN_SIZE   2
#define CUP_MAX_DE2_SIZE  19
#define CUP_DE3_SIZE       6
#define CUP_DE4_SIZE      12
#define CUP_DE5_SIZE	  12
#define CUP_DE6_SIZE	  12
#define CUP_DE7_SIZE      10
#define CUP_DE9_SIZE	   8
#define CUP_DE10_SIZE	   8
#define CUP_DE11_SIZE      6
#define CUP_DE12_SIZE      6
#define CUP_DE13_SIZE      4
#define CUP_DE14_SIZE      4
#define CUP_DE15_SIZE      4
#define CUP_DE16_SIZE	   4
#define CUP_DE18_SIZE	   4
#define CUP_DE19_SIZE	   3
#define CUP_DE22_SIZE      3
#define CUP_DE23_SIZE	   3
#define CUP_DE25_SIZE      2
#define CUP_DE26_SIZE	   2
#define CUP_DE28_SIZE	   8
#define CUP_DE32_SIZE      2
#define CUP_MAX_DE32_SIZE  11
#define CUP_DE33_SIZE      2
#define CUP_MAX_DE33_SIZE  11
#define CUP_DE35_LEN_SIZE  2
#define CUP_MAX_DE35_SIZE  37
#define CUP_DE37_SIZE	   12
#define CUP_DE38_SIZE      6
#define CUP_DE39_SIZE      2
#define CUP_DE41_SIZE      8
#define CUP_DE42_SIZE      15
#define CUP_DE43_SIZE      40
#define CUP_DE44_LEN_SIZE  2
#define CUP_MAX_DE44_SIZE  25
#define CUP_DE45_LEN_SIZE  2
#define CUP_MAX_DE45_SIZE  79
#define CUP_DE48_SIZE     38
#define CUP_DE48_LEN_SIZE  3
#define CUP_MAX_DE48_SIZE  512
#define CUP_DE49_SIZE      3
#define CUP_DE50_SIZE      3
#define CUP_DE51_SIZE      3
#define CUP_DE52_SIZE      8
#define CUP_DE53_SIZE      16
#define CUP_DE54_LEN_SIZE  3
#define CUP_MAX_DE54_SIZE  40
#define CUP_DE55_LEN_SIZE  3
#define CUP_MAX_DE55_SIZE  255
#define CUP_DE57_LEN_SIZE  3
#define CUP_MAX_DE57_SIZE  100
#define CUP_DE60_LEN_SIZE  3
#define CUP_DE60_1_SIZE    4
#define CUP_DE61_2_SIZE    11
#define CUP_MAX_DE60_SIZE  100
#define CUP_DE61_LEN_SIZE  3
#define CUP_MAX_DE61_SIZE  200
#define CUP_DE62_LEN_SIZE  3
#define CUP_MAX_DE62_SIZE  200
#define CUP_DE70_SIZE      3
#define CUP_DE96_SIZE      8
#define CUP_DE100_SIZE_LEN 2
#define CUP_MAX_DE100_SIZE 11
#define CUP_DE117_LEN	   2
#define CUP_DE121_SIZE_LEN 3
#define CUP_MAX_DE121_SIZE 100
#define CUP_DE123_SIZE_LEN 3
#define CUP_MAX_DE123_SIZE 100
#define CUP_DE128_SIZE     8

#define CUP_ACQ_INST_ID_CODE_LEN 			2 /* 2-digit length value  Acquiring Institution Identification Code length (feild 32)*/
#define CUP_FORW_INST_ID_CODE_LEN 			2 /* 2-digit length value  Forwarding Institution Identification Code length(feild 33)*/
#define CUP_TRACK_2_DATA_LEN				2 /* 2-digit length value   Track 2 Data length (feild 35)*/
#define CUP_TRACK_3_DATA_LEN				2 /* 2-digit length value   Track 3 Data length (feild 36)*/

/*Time and date related values*/

#define CUP_MMDD_LEN 4    /*MMDD feild length*/
#define CUP_YYYY_LEN 4   /*YYYY len*/
#define CUP_MMDD_START_POS 4 /*MMDD starting position in array*/
#define CUP_HHMMSS_LEN 6  /*hhmmss feild length*/
#define CUP_CITY_LEN_FEILD_43 12
#define CUP_MER_NAME_LEN_FEILF_43 25 /*Merchant name length for feild 43*/
#define CUP_ATM_LOC_LEN_FEILD_43 25  /* ATM location length for feild 43*/
#define CUP_COUNTRY_CODE_FEILD_43 "PHL" /* Country code for feild 43*/
#define CUP_COUNTY_CODE_LEN_FEILD_43 3 /*Country code length for feild 43*/
#define CUP_MESSAGE_REPONSE_CODE_FEILD_60_1 "0000"
#define CUP_MESSAGE_REPONSE_CODE_FEILD_60_1_LEN 4

#define CUP_MESSAGE_TYPE_SIZE               4
#define CUP_MAX_CARD_NUM_LEN               19
#define CUP_MAX_CARD_HOLDER_LEN            45

/*FIELD 48 */
#define  CUP_ACQ_ADD_INFO_FUNC_CODE              "AA100"   /*Original authorization - fixed*/
#define  CUP_ACQ_PREAUTH_ADD_INFO_FUNC_CODE      "AA101"   /*Preaauth authorization - variabled*/
#define  CUP_ACQ_ADD_INFO_FUNC_CODE_LEN		5
/*CUP header*/
#define CUP_HDR_SIZE_SUCC 				46
#define CUP_MSG_HEADER_LEN 				 	1
#define CUP_MSG_HEADER_SUCC_DATA            0x2E
#define CUP_MSG_HEADER_FLAG_VER_LEN		 	0x01
#define CUP_MSG_HEADER_FLAG_TEST_VER		0x00
#define CUP_MSG_HEADER_FLAG_PROD_VER		0x01
#define CUP_MSG_HEADER_FLAG_CAD             0x02
#define CUP_MSG_HEADER_FLAG_FOR_CUP_HOST    0x82
#define CUP_MSG_HEADER_FLAG_KEY_SEND        0x81
#define CUP_TOL_MEG_LEN                  	4
#define CUP_DEST_ID_LEN						11
#define CUP_CUPS_ID                         "    00010344"
#define CUP_SOURCE_ID_LEN					11
#define CUP_RESV_FOR_USE_LEN				3
#define CUP_BATCH_NO_LEN					1
#define CUP_BATCH_NUM                       0x00
#define CUP_TRAN_INFO_LEN					8
#define CUP_TRAN_INFO_DATA                  "00000000"
#define CUP_USER_INFO_LEN					1
#define CUP_USER_INFO_DATA                  0x00
#define CUP_REJECT_CODE_LEN					5
#define CUP_REJECT_CODE_DATA                "00000"
#define CUP_HDR_SIZE_REJECTED           	46

#define CUP_DE55_HDR_LEN	4
#define CUP_DE55_HDR_VER	2
#define CUP_EMV_CUP_APP_CRYPTO_LEN 	   		8
#define CUP_EMV_CUP_UNPREDICTABLE_NBR_LEN 	4
#define CUP_EMV_CUP_ATC_LEN			   		2
#define CUP_EMV_CUP_TVR_LEN 			   	5
#define CUP_EMV_CUP_TX_DATE_LEN 		   	3
#define CUP_EMV_CUP_TX_TYPE_LEN 		   	1
#define CUP_EMV_CUP_TX_AMOUNT_LEN 		   	6
#define CUP_EMV_CUP_TX_CURRENCY_CODE_LEN  	2
#define CUP_EMV_CUP_POS_COUNTRY_CODE_LEN  	2
#define CUP_EMV_CUP_AIP_LEN 			  	2
#define CUP_EMV_CUP_AMT_OTHER_LEN 		  	6
#define CUP_EMV_CUP_PAN_SEQ_NBR_LEN 	   	1
#define CUP_EMV_CUP_CRYPTO_INFO_LEN 	   	1


/*CUP Preauth Related Changes*/
#define CUP_PREAUTH_PROC_CODE          		"00"
#define CUP_PREAUTH_POS_COND_CODE      		"00"
#define CUP_MOTO_POS_COND_CODE              "08"

/*CUP E_COM Changes*/
#define CUP_DE60_TERM_TYPE_POS_IN_DE60     				8
#define CUP_DE60_TERM_TYPR_LEN			   				2
#define CUP_DE60_LEN_WITH_TERM_TYPR        				10
#define CUP_DE60_ECOM_IDENTIFER_POS_IN_DE60    		 	12
#define CUP_DE60_LEN_WITH_ECOM_IDENTIFER   				14
#define CUP_DE60_ECOM_IDENTIFER_LEN			   			2
#define CUP_ECOM_TERM_TYPE            					"07"
#define CUP_UPI3DS_AUTH_MODE_ISSUER             		"05"
#define CUP_UPI3DS_AUTH_MODE_UPI_ATTEMPT        		"06"
#define CUP_DE61_CVN2_POS								27
#define CUP_CVN2_LEN									3
#define CUP_DE61_CVN2_CHECK_RESULT_POS					30
#define CUP_DE61_CVN2_CHECK_RESULT_LEN					1
#define CUP_DE61_CVN2_TOTAL_LEN   						31
#define CUP_DE61_AM_USAGE_TAG_POS               		35
#define CUP_DE_61_AM_USAGE_TAG_LEN              		2
#define CUP_DE_61_AM_USAGE_BITMAP_POS           		37
#define CUP_DE_61_AM_USAGE_BITMAP_LEN           		16
#define CUP_DE61_AM_USAGE_TAG							"AM"
#define CUP_DE61_AM_USAGE_TAG_LEN						2
#define CUP_DE_61_AM_USAGE_MOBILE_NUM_LEN_POS   		53
#define CUP_DE_61_AM_USAGE_MOBILE_NUM_POS   			56
#define CUP_DE_61_AM_USAGE_MOBILE_NUM_LEN       		3
#define CUP_ECOM_MAX_MOBILE_NUM_LEN        				30
#define CUP_ECOM_AUTH_MODE                 	 			"09"
#define CUP_ECOM_NON_AUTH_MODE             	 			"10"
#define CUP_ECOM_DEFAULT_MODE              				"00"
#define CUP_ECOM_MODE_LEN                  				2
#define CUP_USER_MOBILE_NUM_POS_IN_CCF02P				20
#define CUP_USER_MOBILE_NUM_MAX_LEN						20
#define CUP_DE61_CARD_EXPIRY_VER_POS					1
#define CUP_DE61_CVN2_VER_POS							5
#define CUP_DE61_MOBILE_NUM_VER_POS						8

/*CUP MOTO Changes*/
#define CUP_MOTO_INDUSTRY_CODE 				"500"
#define INDUSTRY_CODE_CODE	 				4
#define CUP_MOTO_TRN_INITIATION_MODE        "3"
#define CUP_CARD_NOT_PRESENT_TRAN			"7"

#define CUP_MID_INDICATOR_DEFAULT 			"7240608"
#define CUP_MID_INDICATOR_LEN     			7
#define CUP_RPS_POS_ENTRY_MODE 				"0012"
#define CUP_RPS_POS_ENTRY_MODE_LEN 			4
#define CUP_ISO_RPS_POS_CONDITION_CODE 		"08"
#define CUP_UPI_RPS_POS_CONDITION_CODE 		"28"
#define CUP_BDO_PAY_POS_CONDITION_CODE      "05"
#define CUP_BDO_PAY_POS_ENTRY_MODE      	"0012"
#define CUP_BDO_POS_ENTRY_MODE_LEN 			4

#define DE125_USAGEIP "IP0050103"

/*CUP Payment Factor Information field details*/
#define CUP_DE117_SUBFIELD_TAG_LEN 				2
#define CUP_DE117_SUBFIELD_BYTE_LEN_POS			2
#define CUP_DE117_SUBFIELD_BYTE_LEN				3
#define CUP_DE117_PAYMENT_FACILTATOR_POS		5
#define CUP_DE117_PAYMENT_FACILTATOR_MAX_LEN 	10

#define POS_ENTRY_MODE_LEN												4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 						"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 					"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM					"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM				"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM			"0012"

#define BDO_POS_ACQ_TXN_HANDLER_QUEUE									"dcp"
#define BDO_SBATCH_TXN_HANDLER_QUEUE									"sbat"
#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN								3

#define POS_ACQ_SALE_MESSAGE_TYPE 										"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 										"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 									"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE							"0420"
#define ISO_MESSAGE_TYPE_LEN											4

#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM						"071"
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM					"072"
#define UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM					"031"
#define UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM				"032"
#define UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM						"012"

#define UPI_POS_DATA_LEN												11
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA				 	"06002300000"
#define UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA				"06002300000"
#define UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA						"06000800000"
#define UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"00000810101"

#define POS_CONDITION_CODE_LEN											2
#define POS_ECOM_CONDITION_CODE											"59"
#define UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_COND_CODE				"00"

#endif
