/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup2_constants.h
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

#ifndef NCEQ_CONSTANTS_H
#define NCEQ_CONSTANTS_H

/* Network Types */
#define   ISSUER   1
#define   ACQUIRER 2
#define   BOTH     3

/* Currency Codes */
#define  PESO     608
#define  USD      840

/* Related Account Types */
#define DOLLAR_ACCOUNT_TYPE  '5'
#define PESO_ACCOUNT_TYPE    '6'


/* TRANSACTION SOURCE */
#define   HOST  1
#define   ATP   2

/* ASCII and EBCDIC conversion */
#define   A2E    "A2E"    /*  ASCII to EBCDIC conversion */
#define   E2A    "E2A"    /*  EBCDIC to ASCII conversion */

/* Error Codes */
#define   TOO_MANY_ACTIVE_TRANSACTIONS   "96"
#define   TOO_MANY_TIMEOUTS              "96"
#define   HOST_UNAVAILABLE               "91"
#define   ISSUER_INOPERATIVE             "96"
#define   SYSTEM_ERROR                   "96"
#define   TO                             "19"
#define   CUP_TIMEOUT                   "68"
#define   INVALID_TXN                    "58"
#define   SECURITY_FAILURE               "55"
#define   AMX_INVALID_TXN                "12"
#define   FORMAT_ERROR                   "30"

#define   APPROVED                      "00"
#define   NETMGMT_APPROVED              "800"
#define   FIELD_EDIT_ERROR              "304"
#define   AMX_HOST_UNAVAILABLE          "912"
#define   RACAL_APPROVED                 "00"
#define   ISO_APPROVED                   "00"


// DE55

#define DE55_HDR_LEN	4
#define DE55_HDR_VER	2

#define CUP_EMV_MAX_LEN			   255

/* Result codes of the security module */
#define   SECURITY_FAILED    0
#define   SECURITY_SUCCESS   1

/* Result codes of the currency conversion module */
#define   CONVERSION_FAILED    0
#define   CONVERSION_SUCCESS   1

/* Country Codes */
#define   USA_CURRENCY_CODE    840
#define   PESO_CURRENCY_CODE   608
#define   RUB_CURRENCY_CODE    643


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
#define   NMI_LOGON                 "001"
#define   NMI_LOGOFF                "002"
#define   NMI_ECHO                  "301"
#define   NMI_KEYCHANGE             "101"
#define   NMI_CUTOFF                "201"
#define   NMI_SAF                   "360"
#define NMI_STR_DATE_SWITCH         "201"
#define NMI_END_DATE_SWITCH         "202"
#define STND_IN_AUTH_ECHO           "401"
#define STND_IN_AUTH_TRA_ACT        "501"
#define STND_IN_PARA_UPD            "902"


/* Function codes ( DE24 ) for financial transaction */
#define   AUTHORIZATION_FUNC_CD		"100"
#define   INQUIRY_FUNC_CD			"108"
#define   CASHSALE_FUNC_CD			"200"
#define   REVERSAL_FUNC_CD			"400"
#define   PARTIALREVERSAL_FUNC_CD	"401"


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
#define   ONLINE_UPDATES_SIZE  sizeof(AMEX_ONLINE_UPDATES)

/* Message Sizes */
#define   MAX_HOST_MSG_SIZE    750
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
#define  COMM_BUF_SZE          1500
#define  EXTENDED_BITFIELD       128
#define  NORMAL_BITFIELD         64
#define  ATM_POS_ENTRY_MODE    "021"
#define  LOOKUP_KEY_SIZE         22

// DE55

#define DE55_HDR_LEN	4
#define DE55_HDR_VER	2

#define EMV_CUP_APP_CRYPTO_LEN 	   8
#define EMV_CUP_UNPREDICTABLE_NBR_LEN 4
#define EMV_CUP_ATC_LEN			   2
#define EMV_CUP_TVR_LEN 			   5
#define EMV_CUP_TX_DATE_LEN 		   3
#define EMV_CUP_TX_TYPE_LEN 		   1
#define EMV_CUP_TX_AMOUNT_LEN 		   6
#define EMV_CUP_TX_CURRENCY_CODE_LEN  2
#define EMV_CUP_POS_COUNTRY_CODE_LEN  2
#define EMV_CUP_AIP_LEN 			   2
#define EMV_CUP_AMT_OTHER_LEN 		   6
#define EMV_CUP_PAN_SEQ_NBR_LEN 	   1
#define EMV_CUP_CRYPTO_INFO_LEN 	   1

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
#define DE6_SIZE	  12
#define DE7_SIZE      10
#define DE9_SIZE	   8
#define DE10_SIZE	   8
#define DE11_SIZE      6
#define DE12_SIZE      6
#define DE13_SIZE      4
#define DE14_SIZE      4
#define DE15_SIZE      4
#define DE16_SIZE	   4
#define DE18_SIZE	   4
#define DE19_SIZE	   3
#define DE22_SIZE      3
#define DE23_SIZE	   3
#define DE25_SIZE      2
#define DE26_SIZE	   2
#define DE28_SIZE	   8
#define DE32_SIZE      2
#define MAX_DE32_SIZE  11
#define DE33_SIZE      2
#define MAX_DE33_SIZE  11
#define DE35_LEN_SIZE  2
#define MAX_DE35_SIZE  37
#define DE37_SIZE	   12
#define DE38_SIZE      6
#define DE39_SIZE      2
#define DE41_SIZE      8
#define DE42_SIZE      15
#define DE43_SIZE      40		
#define DE44_LEN_SIZE  2       
#define MAX_DE44_SIZE  25
#define DE45_LEN_SIZE  2
#define MAX_DE45_SIZE  79
#define DE48_SIZE     38
#define DE48_LEN_SIZE  3       
#define MAX_DE48_SIZE  512
#define DE49_SIZE      3
#define DE50_SIZE      3
#define DE51_SIZE      3
#define DE52_SIZE      8
#define DE53_SIZE      16
#define DE54_LEN_SIZE  3       
#define MAX_DE54_SIZE  40
#define DE55_LEN_SIZE  3       
#define MAX_DE55_SIZE  255
#define DE57_LEN_SIZE  3
#define MAX_DE57_SIZE  100
#define DE60_LEN_SIZE  3
#define DE60_1_SIZE    4
#define DE61_2_SIZE    11
#define MAX_DE60_SIZE  100
#define DE61_LEN_SIZE  3
#define MAX_DE61_SIZE  200
#define DE62_LEN_SIZE  3
#define MAX_DE62_SIZE  200
#define DE70_SIZE      3
#define DE96_SIZE      8
#define DE100_SIZE_LEN 2
#define MAX_DE100_SIZE 11
#define DE121_SIZE_LEN 3
#define MAX_DE121_SIZE 100
#define DE123_SIZE_LEN 3
#define MAX_DE123_SIZE 100


#define DE128_SIZE     8

#define ACQ_INST_ID_CODE_LEN 			2 // 2-digit length value  Acquiring Institution Identification Code length (feild 32)
#define FORW_INST_ID_CODE_LEN 			2 // 2-digit length value  Forwarding Institution Identification Code length(feild 33)
#define TRACK_2_DATA_LEN				2 // 2-digit length value   Track 2 Data length (feild 35)
#define TRACK_3_DATA_LEN				2 // 2-digit length value   Track 3 Data length (feild 36)

/*Time and date related values*/

#define MMDD_LEN 4    //MMDD feild length 
#define YYYY_LEN 4   //YYYY len
#define MMDD_START_POS 4 //MMDD starting position in array 
#define HHMMSS_LEN 6  //hhmmss feild length 
#define CITY_LEN_FEILD_43 12
#define MER_NAME_LEN_FEILF_43 25 //Merchant name length for feild 43
#define ATM_LOC_LEN_FEILD_43 25  // ATM location length for feild 43
#define COUNTRY_CODE_FEILD_43 "PHL" // Country code for feild 43
#define COUNTY_CODE_LEN_FEILD_43 3 //Country code length for feild 43
#define MESSAGE_REPONSE_CODE_FEILD_60_1 "0000" // 
#define MESSAGE_REPONSE_CODE_FEILD_60_1_LEN 4

#define MESSAGE_TYPE_SIZE               4

/*FIELD 48 */
#define  ACQ_ADD_INFO_FUNC_CODE              "AA100"   //Original authorization - fixed
#define  ACQ_PREAUTH_ADD_INFO_FUNC_CODE      "AA101"   //Preaauth authorization - variable
#define  ACQ_ADD_INFO_FUNC_CODE_LEN		5


/*CUP header*/
#define CUP_HDR_SIZE_SUCC 				46
#define MSG_HEADER_LEN 				 	1
#define MSG_HEADER_SUCC_DATA            0x2E
#define MSG_HEADER_FLAG_VER_LEN		 	0x01
#define MSG_HEADER_FLAG_TEST_VER		0x00
#define MSG_HEADER_FLAG_PROD_VER		0x01
#define MSG_HEADER_FLAG_CAD             0x02
#define MSG_HEADER_FLAG_KEY_SEND        0x81
#define TOL_MEG_LEN                  	4
#define DEST_ID_LEN						11
#define CUPS_ID                         "    00010344"
#define SOURCE_ID_LEN					11
#define RESV_FOR_USE_LEN				3
#define BATCH_NO_LEN					1
#define BATCH_NUM                       0x00
#define TRAN_INFO_LEN					8
#define TRAN_INFO_DATA                  "00000000"
#define USER_INFO_LEN					1
#define USER_INFO_DATA                  0x00
#define REJECT_CODE_LEN					5
#define REJECT_CODE_DATA                "00000"
#define CUP_HDR_SIZE_REJECTED           46

/*CUP Preauth Related Changes*/
#define CUP_PREAUTH_PROC_CODE          "00"
#define CUP_PREAUTH_POS_COND_CODE      "00"

#endif
