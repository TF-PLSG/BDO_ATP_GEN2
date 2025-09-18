/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCAMX_CONSTANTS.H
 *  
 * TITLE:       Equitable Network Control Macros and Constants
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the network controller.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 ****************************************************************************/

#ifndef NCEQ_CONSTANTS_H
#define NCEQ_CONSTANTS_H

/* Network Types */
#define   ISSUER   1
#define   ACQUIRER 2
#define   BOTH     3

/* TRANSACTION SOURCE */
#define   HOST  1
#define   ATP   2

/* ASCII and EBCDIC conversion */
#define   A2E    "A2E"    /*  ASCII to EBCDIC conversion */
#define   E2A    "E2A"    /*  EBCDIC to ASCII conversion */

#define AMEX03B_APRIL2024 1
#define AMEX03A
#define AMX1_APR2024 1
#define BDOR_68_REQ_1_AMEX 1
#define BDOR_68_REQ_3_AMEX 1
#define BDOR_68_REQ_5_AMEX 1

/* Error Codes */
#define   TOO_MANY_ACTIVE_TRANSACTIONS   "96"
#define   TOO_MANY_TIMEOUTS              "96"
#define   HOST_UNAVAILABLE               "91"
#define   ISSUER_INOPERATIVE             "96"
#define   SYSTEM_ERROR                   "96"
#define   TO                             "19"
#define   AMEX_TIMEOUT                   "68"
#define   INVALID_TXN                    "58"
#define   SECURITY_FAILURE               "55"
#define   AMX_INVALID_TXN                "12"
#define   FORMAT_ERROR                   "30"

#define   APPROVED                      "000"
#define   NETMGMT_APPROVED              "800"
#define   FIELD_EDIT_ERROR              "304"
#define   AMX_HOST_UNAVAILABLE          "912"
#define   RACAL_APPROVED                 "00"
#define   ISO_APPROVED                   "00"
#define   ISO_STIP_RC                    "01"

#define  PESO     "608"
#define  USD      "840"

#define DE55_HDR_LEN	4
#define DE55_HDR_VER	2

#define AMEX_EMV_MAX_LEN			   255

/* Result codes of the security module */
#define   SECURITY_FAILED    0
#define   SECURITY_SUCCESS   1

/* Result codes of the currency conversion module */
#define   CONVERSION_FAILED    0
#define   CONVERSION_SUCCESS   1

/* Country Codes */
#define   USA_CURRENCY_CODE    "840"
#define   PESO_CURRENCY_CODE   "608"

/* Industry Codes */
#define   ATM_INDUSTRY_CODE    "901"

/* Condition Codes */
#define   sATM_POSCC   "02"
#define   ATM_POSCC      2
#define   POS_POSCC     51
#define   sPOS_POSCC   "51"
#define   NORMAL_POSCC	 0
#define   sNORMAL_POSCC	"00"


/* Acquiring Institution ID Codes */
#define   LOCAL_POS      "00"
#define   INTL_ATM_POS   "95"

/* Transaction Origins */
#define   LOCAL_ORIGIN   "LO"
#define   REMOTE_ORIGIN  "RO"

/* Transaction Flow */
#define   SWITCHED_IN    0
#define   SWITCHED_OUT   1

/* 0800 Request Types */
#define   LOGON         0
#define   LOGOFF        1
#define   ECHO          2
#define   KEY_EXCHANGE  3

/* 1800 function codes for DE24 */
#define   NMI_LOGON     "801"
#define   NMI_LOGOFF    "802"
#define   NMI_ECHO      "831"
#define   NMI_KEYCHANGE "101"
#define   NMI_CUTOFF    "201"
#define   NMI_SAF       "360"
#define   ESTIMATED_AUTHORIZATION   "101"
#define   INCREMENTAL_AUTHORIZATION "106"

/* TO PROCESS PREAUTH CANCEL */
#define PRIMARY_AUTH 										'P'
#define INCREMENTAL_AUTH 									'I'
#define ESTIMATED_AUTH 										'E'
#define PARTIAL_REVERSAL 									'R'

/* Function codes ( DE24 ) for financial transaction */
#define   AUTHORIZATION_FUNC_CD		"100"
#define   INQUIRY_FUNC_CD			"108"
#define   CASHSALE_FUNC_CD			"200"
#define   REVERSAL_FUNC_CD			"400"
#define   PARTIALREVERSAL_FUNC_CD	"401"
#define   ESTIMATED_AUTH_FUNC_CD    "101"
#define   INCREMENTAL_AUTH_FUNC_CD  "106"

/* Acquiring Institution ID Code (DE32) for financial transaction */
#define 	ACQ_INSTN_ID			"06370100"

/* Echo Timer Set/Reset Values */
#define   ECHO_TIMER_IS_SET      1
#define   ECHO_TIMER_IS_CLEARED  0

/* Key Change Flags */
#define   KEY_CHANGE_ENABLED    'E'

/* Structure Sizes */
#define   AUTH_TX_SIZE         sizeof(AUTH_TX)
#define   NCF01_SIZE           sizeof(NCF01)
#define   NCF30_SIZE           sizeof(NCF30)
#define   ATM01_SIZE           sizeof(ATM01)
#define   TLF01_SIZE           sizeof(TLF01)
#define   CRF01_SIZE           sizeof(CRF01)
#define	  BDKKEY01_SIZE		   sizeof(BDKKEY01)
#define   ONLINE_UPDATES_SIZE  sizeof(AMEX_ONLINE_UPDATES)

/* Message Sizes */
#define   MAX_HOST_MSG_SIZE    500
#define   MAX_APPL_MSG_SIZE  20000

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
#define  PROC_CODE_SAVINGS  '1'
#define  PROC_CODE_CURRENT  '2'
#define  PROC_CODE_CREDIT   '3'
#define  PROC_CODE_UNKNOWN  '0'

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

/* TIMER TYPES */
#define  REQUEST_TIMER  0
#define  ECHO_TIMER     1

/* TIMER APPS */
#define  TIMER_REVERSAL   "Reversal"
#define  TIMER_FINANCIAL  "Financial"
#define  TIMER_1804       "1804"
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
#define  COMM_BUF_SZE          1000
#define  EXTENDED_BITFIELD       128
#define  NORMAL_BITFIELD         64
#define  ATM_POS_ENTRY_MODE    "021"
#define  LOOKUP_KEY_SIZE         22

#define DE55_HDR_LEN	4
#define DE55_HDR_VER	2

#define DE61_ECI_VALUE_NOT_AUTHENTICATED		"07"

#define EMV_AMEX_APP_CRYPTO_LEN 	   8
#define EMV_AMEX_UNPREDICTABLE_NBR_LEN 4
#define EMV_AMEX_ATC_LEN			   2
#define EMV_AMEX_TVR_LEN 			   5
#define EMV_AMEX_TX_DATE_LEN 		   3
#define EMV_AMEX_TX_TYPE_LEN 		   1
#define EMV_AMEX_TX_AMOUNT_LEN 		   6
#define EMV_AMEX_TX_CURRENCY_CODE_LEN  2
#define EMV_AMEX_POS_COUNTRY_CODE_LEN  2
#define EMV_AMEX_AIP_LEN 			   2
#define EMV_AMEX_AMT_OTHER_LEN 		   6
#define EMV_AMEX_PAN_SEQ_NBR_LEN 	   1
#define EMV_AMEX_CRYPTO_INFO_LEN 	   1

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
#define DE2_LEN_SIZE   2
#define MAX_DE2_SIZE  19
#define DE3_SIZE       6
#define DE4_SIZE      12
#define DE5_SIZE	  12
#define DE7_SIZE      10
#define DE9_SIZE	   8
#define DE11_SIZE      6
#define DE12_SIZE      12
#define DE13_SIZE      4
#define DE14_SIZE      4
#define DE15_SIZE      4
#define DE16_SIZE	   4
#define DE18_SIZE	   4
#define DE19_SIZE	   3
#define DE20_SIZE      3
#define DE21_SIZE	   3
#define DE22_SIZE      12
#define DE24_SIZE	   3
#define DE25_SIZE      4
#define DE26_SIZE	   4
#define DE28_SIZE	   8
#define DE30_SIZE	   24
#define DE31_LEN_SIZE  2
#define MAX_DE31_SIZE  40
#define DE32_SIZE      2
#define MAX_DE32_SIZE  11
#define DE33_SIZE      2
#define MAX_DE33_SIZE  11
#define DE35_LEN_SIZE  2
#define MAX_DE35_SIZE  37
#define DE37_SIZE	   12
#define DE38_SIZE      6
#define DE39_SIZE      3
#define DE41_SIZE      8
#define DE42_SIZE      15
#define DE43_LEN_SIZE  2
#define MAX_DE43_SIZE  99
#define DE43_AMEX_SIZE 40		/* defined Macro to use it, only for ncposA and ncatmA are 
									handler queue - ThoughtFocus Girija Y */
#define DE44_SIZE      4       /* Girija Y - ThoughtFocus */
#define DE45_LEN_SIZE  2
#define MAX_DE45_SIZE  76
#define DE48_SIZE      38
#define DE49_SIZE      3
#define DE50_SIZE      3
#define DE52_SIZE      8
#define DE56_LEN_SIZE  2
#define MAX_DE56_SIZE 37

/*Amex EMV terminal indicator*/
#define AMEX_EMV_CAPABILITY_INDICATOR     "AC"
#define AMEX_DEVICE_EMV_CAPABILITY_LEN    2

/*AMEX ACQ POS data changes*/

#define AMEX_MAGSTRIP_POS_DATA_WITH_PIN							"560101254041"
#define AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN						"560101254041"

#define AMEX_CHIP_POS_DATA_WITH_PIN								"560101554041"
#define AMEX_CHIP_POS_DATA_WITHOUT_PIN							"560101554041"

#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN					"56010X554041"
#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN				"56010X554041"

#define AMEX_FALLBACK_POS_DATA_WITH_PIN							"560101954041"
#define AMEX_FALLBACK_POS_DATA_WITHOUT_PIN						"560101954041"

#define AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN			"56010X254041"

#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN					"560110664041"
#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN				"560110664041"

#define AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA						"100SS0S00110"
#define AMEX_RPS_ASIAPAY_RPS_POS_DATA							"600090100110"
#define AMEX_BDO_PAY_POS_DATA									"660400600130"

#define AMEX_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA					"56010X554041"
#define AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN		"360101354140"
#define AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN	"360101354140"
#define AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA			"100SS0S00110"

#define POS_ENTRY_MODE_LEN 							    			4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 					"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 				"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM				"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM			"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM		"0012"

#define BDO_POS_ACQ_TXN_HANDLER_QUEUE								"dcp"
#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN							3

#define ISO_MESSAGE_TYPE_LEN										4
#define POS_ACQ_SALE_MESSAGE_TYPE 									"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 									"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 								"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE						"0420"
#define AMEX_AUTH_MESSAGE_TYPE										"1100"
#define AMEX_ADVICE_MESSAGE_TYPE									"1120"
#define AMEX_REVERSAL_MESSAGE_TYPE									"1420"

#define POS_CONDITION_CODE_LEN										2
#define POS_ECOM_CONDITION_CODE										"59"

#endif
