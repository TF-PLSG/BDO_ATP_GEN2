/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci_constants.h
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

#ifndef DCI_CONSTANTS_H
#define DCI_CONSTANTS_H

#define BDOR_68_REQ_1_DCI 1
#define BDOR_68_REQ_3_DCI 1
#define BDOR_68_REQ_5_DCI 1

#define  COMM_BUF_SZE  1500
/* Network Types */
#define   DINERS_ISSUER   1
#define   DINERS_ACQUIRER 2
#define   DINERS_BOTH     3

/* Currency Codes */
#define  DINERS_PESO     "608"
#define  DINERS_USD      "840"

/* Related Account Types */
#define DINERS_DOLLAR_ACCOUNT_TYPE  '5'
#define DINERS_PESO_ACCOUNT_TYPE    '6'


/* TRANSACTION SOURCE */
#define   DINERS_HOST  1

/* ASCII and EBCDIC conversion */
#define   A2E    "A2E"    /*  ASCII to EBCDIC conversion */
#define   E2A    "E2A"    /*  EBCDIC to ASCII conversion */

#define MAX_INI_INFO_SIZE   20

/* Error Codes */
#define   DINERS_ISO_RESP_LEN 							2
#define   DINERS_TOO_MANY_ACTIVE_TRANSACTIONS   		"96"
#define   DINERS_TOO_MANY_TIMEOUTS              		"96"
#define   DINERS_HOST_UNAVAILABLE_ISO              		"91"
#define   DINERS_ISSUER_INOPERATIVE             		"96"
#define   DINERS_SYSTEM_ERROR                   		"96"
#define   DINERS_TO                             	  	"19"
#define   DINERS_TIMEOUT                        		"68"
#define   DINERS_INVALID_TXN                    		"58"
#define   DINERS_SECURITY_FAILURE               		"55"
#define   DINERS_AMX_INVALID_TXN                		"12"
#define   DINERS_FORMAT_ERROR                   		"30"
#define   DINERS_APPROVED                      			"000"
#define   DINERS_NETMGMT_APPROVED              			"800"
#define   DINERS_FIELD_EDIT_ERROR              			"304"
#define   DINERS_HOST_UNAVAILABLE              			"912"
#define   DINERS_RACAL_APPROVED                			"00"
#define   DINERS_ISO_APPROVED                  			"00"
#define   DINERS_KEY_TRANSLATE_FAILED_RESP_CODE         "96"
#define   DINERS_REVERSAL_MESSAGE_RESPONE               "400"
#define   DINERS_RESPONSE_APPROVED_ISSUER		        "081"
#define   DINERS_ADVICE_MESSAGE_RESPONSE                "082"
#define   DINERS_EMV_VALIDATION_FAILED_RESP_CODE        "100"
#define   DINERS_RACAL_DECLINE_RESPONSE                 "01"
#define   DINERS_CARD_ACCT_VERIFICATION_SUCCESS         "086"
#define   DINERS_CARD_ACCT_VERIFICATION_FAILURE         "197"
#define   DINERS_RACAL_RESPONSE_LEN                     2
/* Country Codes */
#define   DINERS_USA_CURRENCY_CODE    					"840"
#define   DINERS_PESO_CURRENCY_CODE   					"608"
#define   DINERS_SIERRA_LEONE_CODE						"925"
#define   DINERS_ICELAND_KRONA_CODE						"352"


/* Transaction Flow */
#define   DINERS_SWITCHED_IN    						0
#define   DINERS_SWITCHED_OUT   						1

/* 18XX Request Types */
#define   DINERS_LOGON        							0
#define   DINERS_LOGOFF        							1
#define   DINERS_ECHO          							2
#define   DINERS_KEY_EXCHANGE							3
#define   DINERS_KEY_TEST  								4


/* 1800 function codes for DE24 */
#define   DINERS_NMI_LOGON                 				"801"
#define   DINERS_NMI_LOGOFF                				"802"
#define   DINERS_NMI_KEYCHANGE             				"811"
#define   DINERS_NMI_KEYTEST              				"814"
#define   DINERS_NMI_SYSTEM_ECHO                   		"831"
#define   DINERS_NETWORK_FUNCTION_CODE_LEN				3
#define   DINERS_KEY_LEN                                32
#define   DINERS_KEY_CHECK_LEN							4

/* Echo Timer Set/Reset Values */
#define   DINERS_ECHO_TIMER_IS_SET      				1
#define   DINERS_ECHO_TIMER_IS_CLEARED  				0

/* Structure Sizes */
#define   AUTH_TX_SIZE         		sizeof(AUTH_TX)
#define   NCF01_SIZE           		sizeof(NCF01)
#define   NCF30_SIZE           		sizeof(NCF30)
#define   ATM01_SIZE           		sizeof(ATM01)
#define   TLF01_SIZE           		sizeof(TLF01)
#define   CRF01_SIZE           		sizeof(CRF01)
#define   NCF01_KEYBLOCK01_SIZE		sizeof(NCF01_KEYBLOCK)
#define   ONLINE_UPDATES_SIZE  sizeof(AMEX_ONLINE_UPDATES)

/* Message Sizes */
#define   DINERS_MAX_HOST_MSG_SIZE    					750
#define   DINERS_MAX_APPL_MSG_SIZE  					20000

/* Transaction Tracing Flags */
#define  DINERS_TRACE_OFF      							0
#define  DINERS_TRACE_ON       							1

/* TIMER TYPES */
#define  DINERS_REQUEST_TIMER  							0
#define  DINERS_ECHO_TIMER     							1

/* TIMER APPS */
#define  DINERS_TIMER_REVERSAL   					"Reversal"
#define  DINERS_TIMER_FINANCIAL  					"Financial"
#define  DINERS_TIMER_08X0       					"0800"
#define  DINERS_TIMER_ECHO      		 			"Echo"

/*Message Type*/
#define DINERS_MESSAGE_TYPE_SIZE               		 4
#define DINERS_FILE_TRN_MESSAGE_TYPE_LEN			 2
#define DINERS_NETWORK_REQ_MESSAGE					 "1804"
#define DINERS_NETWORK_RESP_MESSAGE					 "1814"
#define DINERS_SALE_REQ_MESSAGE						 "1100"
#define DINERS_SALE_RESP_MESSAGE					 "1110"
#define DINERS_ADVICE_REQ_MESSAGE					 "1120"
#define DINERS_ADVICE_RESP_MESSAGE					 "1130"
#define DINERS_REVERSAL_REQ_MESSAGE					 "1420"
#define DINERS_REVERSAL_RESP_MESSAGE				 "1430"
#define DINERS_ISO_SALE_REQ_MESSAGE_TYPE             "0200"
#define DINERS_ISO_SALE_RESP_MESSAGE_TYPE            "0210"
#define DINERS_ISO_AUTH_REQ_MESSAGE_TYPE             "0100"
#define DINERS_ISO_AUTH_RESP_MESSAGE_TYPE            "0110"
#define DINERS_ISO_REVESAL_REQ_MESSAGE_TYPE          "0400"
#define DINERS_ISO_REVESAL_RESP_MESSAGE_TYPE         "0410"
#define DINERS_ISO_REVESAL_ADVICE_REQ_MESSAGE_TYPE   "0420"
#define DINERS_ISO_SALE_COMPLETION_MESSAGE_TYPE      "0220"
#define ISO_ADVICE_REQ_MESSAGE					 	 "0120"
#define ISO_ADVICE_RESP_MESSAGE					 	 "0130"
#define DINERS_FILE_UPDATE_REQ_MESSAGE_TYPE          "1304"
#define DINERS_FILE_UPDATE_RESP_MESSAGE_TYPE         "1314"
#define DINERS_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE "1324"
#define DINERS_NEGATIVE_FILE_UPDATE_RESP_MESSAGE_TYPE"1334"
#define DINERS_FILE_TRN_MESSAGE_TYPE				 "13"
#define DINERS_VOID_PREAUTH_REQ_MESSAGE_TYPE		 "1420"
#define DINERS_VOID_PREAUTH_RESP_MESSAGE_TYPE		 "1430"
/* Shared Memory Tables */
#define  CRF01_TABLE    "Crf01Table"

/* Flags used when sending responses to host */
#define  DINERS_SEND_UNCONDITIONALLY  				   0
#define  DINERS_SEND_IF_NOT_REVERSAL  				   1
#define  DINERS_SEND_IF_KEY_TRAN_FAILED  			   2
#define  DINERS_SEND_IF_KEY_TRAN_SUCCES  			   3

/* Miscellaneous */
#define  DINERS_COMM_BUF_SZE          				    1500
#define  DINERS_EXTENDED_BITFIELD_SIZE  				128
#define  DINERS_EXTENDED_BITMAP_SIZE					16
#define  DINERS_NORMAL_BITFIELD_SIZE    				64
#define  DINERS_NORMAL_BITMAP_SIZE					    8
#define  DINERS_LOOKUP_KEY_SIZE         				22
#define  DINERS_TOTALL_MEG_LEN						    4
#define  DINERS_ETX_TRN_TEXT							"0x03"
#define  DINERS_KEY_SIZE								48

/* TO PROCESS PREAUTH CANCEL */
#define PRIMARY_AUTH 									'P'
#define INCREMENTAL_AUTH 								'I'
#define ESTIMATED_AUTH 									'E'
#define PARTIAL_REVERSAL 								'R'

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
#define DINERS_DE2_LEN_SIZE   						2
#define DINERS_MAX_DE2_SIZE  						19
#define DINERS_DE3_SIZE       						6
#define DINERS_DE4_SIZE      						12
#define DINERS_DE6_SIZE	  							12
#define DINERS_DE7_SIZE      						10
#define DINERS_DE11_SIZE      						6
#define DINERS_DE12_SIZE     						12
#define DINERS_DE13_SIZE      						4
#define DINERS_DE14_SIZE      						4
#define DINERS_DE22_SIZE      						12
#define DINERS_DE23_SIZE	   						3
#define DINERS_DE24_SIZE	   						3
#define DINERS_DE25_SIZE      						4
#define DINERS_DE26_SIZE	   						4
#define DINERS_DE30_SIZE	   						24
#define DINERS_DE32_LEN_SIZE   						2
#define DINERS_MAX_DE32_SIZE  						11
#define DINERS_DE33_LEN_SIZE      					2
#define DINERS_MAX_DE33_SIZE  						11
#define DINERS_DE35_LEN_SIZE  						2
#define DINERS_MAX_DE35_SIZE  						37
#define DINERS_DE37_SIZE	   						12
#define DINERS_DE38_SIZE      						6
#define DINERS_DE39_SIZE     						3
#define DINERS_DE40_SIZE      						3
#define DINERS_DE41_SIZE      						8
#define DINERS_DE42_SIZE      						15
#define DINERS_DE43_LEN_SIZE  						2
#define DINERS_MAX_DE43_SIZE  						99
#define DINERS_DE44_LEN_SIZE  						2
#define DINERS_MAX_DE44_SIZE  						22
#define DINERS_DE45_LEN_SIZE  						2
#define DINERS_MAX_DE45_SIZE  						76
#define DINERS_DE46_LEN_SIZE  						3
#define DINERS_MAX_DE46_SIZE  						11
#define DINERS_DE48_LEN_SIZE  						3
#define DINERS_MAX_DE48_SIZE  						999
#define DINERS_DE49_SIZE      						3
#define DINERS_DE51_SIZE      						3
#define DINERS_DE52_SIZE      						8
#define DINERS_DE54_LEN_SIZE  						3
#define DINERS_MAX_DE54_SIZE 						120
#define DINERS_DE55_LEN_SIZE  						3
#define DINERS_MAX_DE55_SIZE  						255
#define DINERS_DE56_LEN_SIZE						2
#define DINERS_DE56_MAX_SIZE						33
#define DINERS_DE58_SIZE_LEN 						2
#define DINERS_DE58_MAX_SIZE_LEN 					11
#define DINERS_DE58_SIZE      						11
#define DINERS_DE59_LEN_SIZE  						3
#define DINERS_MAX_DE59_SIZE  						999
#define DINERS_DE62_LEN_SIZE  						3
#define DINERS_MAX_DE62_SIZE  						999
#define DINERS_DE64_SIZE							8
#define DINERS_DE72_LEN_SIZE  						3
#define DINERS_MAX_DE72_SIZE  						999
#define DINERS_DE92_SIZE							3
#define DINERS_DE93_LEN_SIZE  						2
#define DINERS_MAX_DE93_SIZE  						11
#define DINERS_DE94_LEN_SIZE  						2
#define DINERS_MAX_DE94_SIZE  						11
#define DINERS_DE96_LEN_SIZE  						3
#define DINERS_MAX_DE96_SIZE  						36
#define DINERS_DE100_SIZE_LEN 						2
#define DINERS_MAX_DE100_SIZE 						11
#define DINERS_DE101_SIZE_LEN 						2
#define DINERS_MAX_DE101_SIZE 						17
#define DINERS_DE106_SIZE_LEN 						3
#define DINERS_MAX_DE106_SIZE 						999
#define DINERS_DE122_SIZE_LEN 						3
#define DINERS_MAX_DE122_SIZE 						100
#define DINERS_DE123_SIZE_LEN 						3
#define DINERS_MAX_DE123_SIZE 						15
#define DINERS_DE124_SIZE_LEN 						3
#define DINERS_MAX_DE124_SIZE 						999
#define DINERS_DE125_SIZE_LEN 						3
#define DINERS_MAX_DE125_SIZE 						8


/*Time and date related values*/
#define DINERS_MAX_MONTH_IN_YEAR					12
#define DINERS_YY_LEN								2
#define DINERS_MM_LEN								2
#define DINERS_DD_LEN								2
#define DINERS_MMDD_LEN 							4
#define DINERS_YYMMDD_LEN 							6
#define DINERS_YYYY_LEN 							4
#define DINERS_MMDD_START_POS 						4
#define DINERS_YY_START_POS 						2
#define DINERS_DD_START_POS 						6
#define DINERS_HHMMSS_LEN 							6
#define DINERS_HHMMSS_START_POS						6
#define DINERS_YYYYMMDD_LEN							8

/*Handler Queue */
#define DINERS_POS_HANDLER_MODULE_COMP_LEN     		3
#define DINERS_ISSUING_MODULE_COMP_LEN  			5
#define DINERS_VOICE_MODULE_COMP_LEN    			5
#define DINERS_SBATCH_MODULE_COMP_LEN    			4
#define DINERS_CANDENCIE_LINK_QUEUE_NAME_LEN        6
#define IST_MODULE_COMP_LEN  						5

/*Transaction type*/
#define DINERS_RPS_INDUSTRY_CODE 			      "800"
#define DINERS_RPS_INDUSTRY_CODE_LEN 			   3

/*Fields related definition*/

/*DE 3 Related definition  */
/*=============================================================================================*/
#define DINERS_DE3_PROC_CODE_PIN_CHANGE 					"98"
#define DINERS_DE3_PROC_CODE_PIN_UNBLOCK					"99"
#define DINERS_DE3_PROC_CODE_BALANCE_INQ					"31"
#define DINERS_DE3_PROC_CODE_CARD_ACC_VERIFICATION_REQ      "18"
#define DINERS_DE3_PROC_CODE_ISO_AUTH_AUTHORIZATION_VAL		"00"
#define DINERS_DE3_PROC_CODE_ISO_AUTH_SALE_VAL				"00"
#define DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL			"02"
#define DINERS_DE3_PROC_CODE_ISO_AUTH_BAL_INQ_VAL			"30"
#define DINERS_DE3_PROC_CODE_ISO_CARD_VERIFICATION_INQ_VAL  "38"
#define DINERS_DE3_PROC_CODE_ISO_PRE_AUTH_VAL  				"30"
#define DINERS_DE3_PROC_CODE_ISO_AUTH_CASH_ADV_VAL			"01"
#define DINERS_DE3_PROC_CODE_ISO_AUTH_QUASI_CASH_VAL		"11"
#define DINERS_DE3_PROC_CODE_REFUND_REQ						"20"
#define DINERS_DE3_PROC_CODE_ISO_VOID_PRE_AUTH_VAL			"02"
#define DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN				2

/*=============================================================================================*/

/*DE 22 Point Of Service Data Code  */
/*=============================================================================================*/
/*ISO BDO POS TERMINAL*/
#define DINERS_BDO_POS_ENTRY_MODE_LEN										3
#define DINERS_BDO_POS_ENTRY_MODE_UNKNOW									"000"
#define DINERS_BDO_POS_ENTRY_MODE_MANUAL									"001"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1									"002"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP2									"006"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP3									"090"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP4									"008"
#define DINERS_BDO_POS_ENTRY_MODE_ICC_EMV									"005"
#define DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV							"007"
#define DINERS_BDO_POS_ENTRY_MODE_FALLBACK									"080"
#define DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE						"091"
#define DINERS_BDO_POS_ENTRY_MODE_ECOM_INCOMING_TRN							"081"
#define DINERS_BDO_POS_ENTRY_MODE_WITH_PIN									"1"
#define DINERS_BDO_POS_ENTRY_MODE_WITHOUT_PIN								"2"
#define DINERS_BDO_POS_ENTRY_MODE_PIN_LEN									1
#define DINERS_BDO_POS_ENTRY_MODE_FOR_RPS									"0012"
#define DINERS_BDO_POS_ENTRY_MODE_FOR_RPS_LEN								4

/***********************************************************************************/
/*POS 1*/
/***********************************************************************************/
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_UNKNOW						'0'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MANUAL						'1'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP					'2'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_BARCODE					'3'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_OCT						'4'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_ICC						'5'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_KET_ENTERED				'6'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS				'8'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL				'9'
/***********************************************************************************/
/***********************************************************************************/
/*POS 2*/
#define DINERS_DE22_POST_2_CARDHOLDER_AUTHENTICATION_CAPABILITY_PIN		'1'
/***********************************************************************************/
/***********************************************************************************/
/*POS 4*/
#define DINERS_DE22_POST_4_OPERATING_ENV_UNKNOWN							'9'
/***********************************************************************************/

/***********************************************************************************/
/*POS 5*/
#define DINERS_DE22_POST_5_INTERNET											'9'
/***********************************************************************************/
/*POS 6*/
/***********************************************************************************/
#define DINERS_DE22_POST_6_CARD_NOT_PRESENT								'0'
#define DINERS_DE22_POST_6_CARD_PRESENT									'1'
#define DINERS_DE22_POST_6_CARD_PRESENT_UNKNOW							'8'
/***********************************************************************************/
/*POS 7*/
/***********************************************************************************/
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_UNSPECIFIED					'0'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MANUAL						'1'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_READ				'2'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_BAR_CODE					'3'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_OCR							'4'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_ICC							'5'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_KEY_ENTERED					'6'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_FALLBACK				'9'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_CARD_DATA_RFID			'S'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_DATA_RFID			'T'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CONTACTLESS_CHANGED_TO_ICC	'U'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_RESERVED_ISO_USE			'7'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_RESERVED_NATION_USE			'8'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_M_COMMERCE					'V'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_STORED_CARD_ACCT_INFO		'B'
/***********************************************************************************/

/*MAGSTIPR POS DATA FOR DCI*/
/***********************************************************************************/
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP_STR				"2"
#define DINERS_DE22_POST_1_
/***********************************************************************************/

/*POS DATA MAPPING FOR ACQUIRED TRANSACTION*/
/***********************************************************************************/

/*Magstrip Acquired transaction 021*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN 						"960101255141"
#define DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN 					"960101255140"
#define DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN 					"960101255141"
#define DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN  				"960101255140"
/***********************************************************************************/

/*Chip Dip Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN  						"960101551141"
#define DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN   					"960101551140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN   				"960101551141"
#define DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN  				"960101551140"
/***********************************************************************************/

/*Fall Back Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN   					"960101955141"
#define DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN   					"960101955140"
#define DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN   				"960101955141"
#define DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN   				"960101955140"
/***********************************************************************************/

/*Contactless Magstrip Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN   		"960101T15940"
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN   		"960101T65140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN   	"960101T15940"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN	"960101T65140"
/***********************************************************************************/

/*Contactless CHIP Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN				"960101S51141"
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN			"960101S51140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN			"960101S51141"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN		"960101S51140"
/***********************************************************************************/
/*Manual Keyed in, RPS , ECOM, Voice Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM							"660490195110"
#define DCI_POS_DATA_DC_PRESENT_RPS										"160040B95110"
#define DCI_POS_DATA_DC_PRESENT_ASIAPAY_RPS								"160040195110"
#define DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN							"960101664141"
#define DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN						"960101664140"
#define DCI_POS_DATA_DC_PRESENT_VOICE									"660001695130"
#define DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM						"660490195110"
#define DCI_POS_DATA_DC_NOT_PRESENT_RPS									"160040B95110"
#define DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_RPS							"160040195110"
#define DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN						"660101664141"
#define DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN					"660101664140"
#define DCI_POS_DATA_DC_NOT_PRESENT_VOICE								"660001695130"
#define DCI_POS_DATA_FOR_BDO_PAY										"160000195130"
/***********************************************************************************/
/*Manual Keyed in, RPS , ECOM, Voice Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_UNKNOWN									"06093S8065110"
#define DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN								"06093S8065110"
/***********************************************************************************/
/*MobilePOS-NFC Terminal for Tap2Phone Transactions*/
/***********************************************************************************/
#define DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN 					"960101S51141"
#define DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN 				"960101S51140"

/***********************************************************************************/

/*Swiftpass BDO Pay Credit Trx*/
/***********************************************************************************/
#define DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN 				"360101354141"
#define DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN 			"360101354140"
/***********************************************************************************/

/*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
/***********************************************************************************/
#define DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA 					"660490195110"
/***********************************************************************************/

/*=============================================================================================*/

/*DE 23 Card Sequence Number  */
/*=============================================================================================*/
#define DINERS_DE23_CARD_SEQ_POS_IN_STR										1
/*=============================================================================================*/

/*DE 24 Function Code Related definition  */
/*=============================================================================================*/
/*MTI 1100*/
/***********************************************************************************/
#define DINERS_DE24_FUNCION_CODE_MTI_1100_100								"100"
#define DINERS_DE24_FUNCION_CODE_MTI_1100_108								"108"
#define DINERS_DE24_FUNCION_CODE_MTI_1100_180								"180"
/***********************************************************************************/

/*MTI 1120*/
/***********************************************************************************/
#define DINERS_DE24_FUNCION_CODE_MTI_1120_100								"100"
#define DINERS_DE24_FUNCION_CODE_MTI_1120_108								"108"
#define DINERS_DE24_FUNCION_CODE_MTI_1120_180								"180"
#define DINERS_DE24_FUNCION_CODE_MTI_1120_190								"190"
/***********************************************************************************/

/*MTI 13XX*/
/***********************************************************************************/
#define DINERS_DE24_FUNCION_CODE_MTI_13XX_301								"301"
#define DINERS_DE24_FUNCION_CODE_MTI_13XX_302								"302"
#define DINERS_DE24_FUNCION_CODE_MTI_13XX_303								"303"
#define DINERS_DE24_FUNCION_CODE_MTI_13XX_304								"304"
#define DINERS_DE24_FUNCION_CODE_MTI_13XX_305								"305"
#define DINERS_DE24_FUNCION_CODE_MTI_13XX_309								"309"
/***********************************************************************************/

/*MTI 1420*/
/***********************************************************************************/
#define DINERS_DE24_FUNCION_CODE_MTI_1420_400								"400"
#define DINERS_DE24_FUNCION_CODE_MTI_1420_401								"401"
#define DINERS_DE24_FUNCION_CODE_MTI_1420_440								"440"
/***********************************************************************************/

/*MTI 1624*/
/***********************************************************************************/
#define DINERS_DE24_FUNCION_CODE_MTI_1624_640								"640"
/***********************************************************************************/

/*MTI 1804*/
/***********************************************************************************/
#define DINERS_DE24_FUNCION_CODE_MTI_1804_801								"801"
#define DINERS_DE24_FUNCION_CODE_MTI_1804_802								"802"
#define DINERS_DE24_FUNCION_CODE_MTI_1804_811								"811"
#define DINERS_DE24_FUNCION_CODE_MTI_1804_814								"814"
#define DINERS_DE24_FUNCION_CODE_MTI_1804_831								"831"
#define DINERS_DE24_FUNCION_CODE_MTI_1804_832								"832"
/***********************************************************************************/
/*=============================================================================================*/


/*DE 25 Message Reason Code  */
/*=============================================================================================*/
#define DINERS_DE25_MESG_REASON_CODE_REVERSAL_REQUEST						"4005"
#define DINERS_DE25_MESG_REASON_CODE_ADVICE_AUTHORIZATION					"4006"
#define DINERS_DE25_MESG_REASON_CODE_TEMP_NEG_FILE_ADD_ADVICE				"1411"
#define DINERS_DE25_MESG_REASON_CODE_NEGT_FILE_HIT_ADVICE					"1412"
#define DINERS_DE25_MESG_REASON_CODE_ISSUER_UNAVAILABLE						"0066"
#define DINERS_DE25_MESG_REASON_CODE_CAVV_VERIFICATION_FAILED				"0069"
#define DINERS_DE25_MESG_REASON_CODE_COUNTRY_BLOCK_BY_ISSUER				"1378"
#define DINERS_DE25_MESG_REASON_CODE_TRN_BLOCK_SUSPECTED_FRAUD				"1379"
#define DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION                     		"59"
#define DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN       		2
#define DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION                     		"08"
#define DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION_INDICATOR_LEN				2
#define DINERS_DE25_RECURRING_TRANSACTION                     				"08"
#define DINERS_DE25_RECURRING_TRANSACTION_INDICATOR_LEN      		 		2
#define DINERS_DE25_BDO_PAY_TRANSACTION                     				"05"
#define DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN      		 			2
/*=============================================================================================*/

/*DE 39 Action Code */
/*=============================================================================================*/
#define  DINERS_NUMBER_OF_ACTION_CODES   68
/*=============================================================================================*/

/*DE 43 Card Acceptor Name/location */
/*=============================================================================================*/
#define DINERS_DE43_NAME_STREET_CITY_SEPARATOR_SYSMOL							"/"
#define DINERS_DE43_POSTAL_CODE_POSTION 										83
#define DINERS_DE43_POSTAL_CODE_LEN 											10
#define DINERS_DE43_REGION_POSTION 												93
#define DINERS_DE43_REGION_LEN 													3
#define DINERS_DE43_COUNTRY_CODE_POSTION										96
#define DINERS_DE43_COUNTRY_CODE_LEN											3
#define DINERS_DE43_MAX_SIZE  													"99"
/*=============================================================================================*/


/*DE 44 Additional Response Data */
/*=============================================================================================*/
/***********************************************************************************/
/* For CVV indicator*/
#define DINERS_DE44_CVVV1_ON_STRIPE_CORRECT										"001"
#define DINERS_DE44_CVVV1_ON_STRIPE_NOT_CORRECT									"002"
#define DINERS_DE44_CVVV1_MISSING												"003"
#define DINERS_DE44_CVVV2_CORRECT												"006"
#define DINERS_DE44_CVVV2_NOT_CORRECT											"007"
#define DINERS_DE44_CVVV1_CVV2_CORRECT											"010"
#define DINERS_DE44_CVVV1_CVV2_NOT_CORRECT										"011"
#define DINERS_DE44_CVVV1_CORRECT_CVV2_NOT_CORRECT								"012"
#define DINERS_DE44_CVVV1_NOT_CORRECT_CVV2_CORRECT								"013"
#define DINERS_DE44_CVVV1_MISSING_CVV2_CORRECT									"014"
#define DINERS_DE44_CVVV1_MISSING_CVV2_NOT_CORRECT								"015"
#define DINERS_DE44_DCVVV_VALID													"016"
#define DINERS_DE44_DCVVV_INVALID												"017"
#define DINERS_DE44_UNABLE_TO_VALIDATE_CVV										"018"
#define DINERS_DE44_NEITHER_CVV_WAS_CHACKED										""
#define DINERS_DE44_CVV_INDICATOR_LEN											 3
/***********************************************************************************/
/***********************************************************************************/
/* CAVV Result Code */
#define DINERS_DE44_CAVV_PASSED_VERIFICATION_AUTHENTICATION						"01"
#define DINERS_DE44_CAVV_FAILED_VERIFICATION_AUTHENTICATION						"02"
#define DINERS_DE44_CAVV_PASSED_VALIDATION_ATTEMPT								"03"
#define DINERS_DE44_CAVV_FAILED_VALIDATION_ATTEMPT								"04"
#define DINERS_DE44_CAVV_NOT_VALIDATED_ISSUER_NOT_PARTICIPATING					"05"										"04"
#define DINERS_DE44_CAVV_UNABLE_TO_PERFORM_AUTHENTICATION						"06"										"04"
#define DINERS_DE44_CAVV_RESULT_CODE_LEN										 2
/***********************************************************************************/

/***********************************************************************************/
/* Error Data element */
#define DINERS_DE44_NON_NUMERIC_CHAR_FOUND										"10"
#define DINERS_DE44_INVALID_NUMBER_FORMAT										"11"
#define DINERS_DE44_REQ_OWNER_CURRENCY_CONVERSION_AMOUT_T0_LARGE				"15"
#define DINERS_DE44_TRUNCATION_NO_MOVEMENT_DATA									"20"
#define DINERS_DE44_INVALID_INFORMATION_CHECK_DATA_ELEMENT						"61"
#define DINERS_DE44_ACCOUNT_NUMBER_INVALID										"A1"
#define DINERS_DE44_ACCOUNT_NUMBER_FAILD_MOD_10_CHECK							"A2"
#define DINERS_DE44_LOCAL_ONLY_CYCLE_RANGE										"A3"
#define DINERS_DE44_UNKNOWN_CYCLE_RANGE											"A4"
#define DINERS_DE44_EDIT_ERROR_DATA_ELEMENT_CONTAINS_OUT_OF_RANGE_VALUE			"E1"
#define DINERS_DE44_INVALID_FUNCTION_CODE										"E4"
#define DINERS_DE44_INVALID_DATE_TIME											"E5"
#define DINERS_DE44_VALUE_NOT_ALLOWED_TO_REQUESTER								"E6"
#define DINERS_DE44_NEGATIVE_AMOUNT												"F3"
#define DINERS_DE44_INVALID_ABC													"F9"
#define DINERS_DE44_ORIGINAL_VALUE_INVALID										"FA"
#define DINERS_DE44_ACTION_NOT_ALLOWED											"G0"
#define DINERS_DE44_INVALID_REQUEST_REQUESTER_MUST_BE_OWNER						"G1"
#define DINERS_DE44_WARN_BULLETINE_CONTAINS_INVALID_DISTRIBUTION				"G2"
#define DINERS_DE44_FUNC_NOT_EXPECT_DATA_ELEMENT_TO_CONTAIN_VALUE				"G3"
#define DINERS_DE44_DUPLICATE_STAN												"G5"
#define DINERS_DE44_MESSAGE_NOT_AGREE_WITH_LINE_OWNER							"G6"
#define DINERS_DE44_MANDATORY_DATA_ELEMENT_NOT_PRESENT							"G7"
#define DINERS_DE44_INVALID_STAN_IN_LIMIT										"L2"
#define DINERS_DE44_INVALID_ACQ_IIC												"L3"
#define DINERS_DE44_RECORD_NOT_FOUND											"R1"
#define DINERS_DE44_RECORD_ALREADY_EXISTS										"R2"
#define DINERS_DE44_ENTRY_NOT_FOUND												"R3"
#define DINERS_DE44_XPRESS_SYSTEM_ERROR											"X0"

/***********************************************************************************/

/***********************************************************************************/
/* Cryptogram Validation Result Code */
#define DINERS_DE44_CRYPTOGRAM_VALIDATION_PASS									"01"
#define DINERS_DE44_CRYPTOGRAM_VALIDATION_FAILED								"02"
#define DINERS_DE44_CRYPTOGRAM_UNABLE_TO_VALIDAT								"03"
#define DINERS_DE44_CRYPTOGRAM_VALIDATION_RESULT_CODE							 2
/***********************************************************************************/

/***********************************************************************************/
/* TVR Check Result Code */
#define DINERS_DE44_TVR_CHECK_NOT_TESTED										"0"
#define DINERS_DE44_TVR_CHECK_PASS												"1"
#define DINERS_DE44_TVR_CHECK_FAILED											"2"
#define DINERS_DE44_TVR_CHECK_RESULT_LEN										 2
/***********************************************************************************/

/***********************************************************************************/
/* CVR Check Result Code */
#define DINERS_DE44_CVR_CHECK_NOT_TESTED										"0"
#define DINERS_DE44_CVR_CHECK_PASS												"1"
#define DINERS_DE44_CVR_CHECK_FAILED											"2"
#define DINERS_DE44_CVR_CHECK_RESULT_LEN										 2
/***********************************************************************************/

/*=============================================================================================*/

/*DE 54 Card Acceptor Name/location */
/*=============================================================================================*/
#define DINERS_DE54_ACCOUNT_TYPE_STARTING_POS									 2
#define DINERS_DE54_ACCOUNT_TYPE_LEN											 2
#define DINERS_DE54_AMOUNT_TYPE													"02"
#define DINERS_DE54_AMOUNT_TYPE_LEN												 2
#define DINERS_DE54_POSITIVE_AVAILABLE_BALANCE									"C"
#define DINERS_DE54_NEGATIVE_AVAILABLE_BALANCE									"D"
#define DINERS_DE54_AVAILABLLE_BALANCE_LEN										 1
/*=============================================================================================*/


/*DE 96 Key Management Data */
/*=============================================================================================*/
#define DINERS_DE96_KEY_LEN									 				 	32
#define DINERS_DE96_KEY_CHECK_LEN											 	4
/*=============================================================================================*/




/***********************************************************************************************/
/*Format Error Conditions , Codes and  Error data*/
/*Primary Account Number DE 2*/
/*=============================================================================================*/
/* For ALL MTI*/
/***********************************************************************************/
#define DINERS_DE2_INVALID_NON_DIGIT_PAN		                        		"904"
#define DINERS_DE2_INVALID_NON_DIGIT_PAN_ERROR_DATA		            			"002A1"
/***********************************************************************************/

/*MTI_11NN */
/***********************************************************************************/
#define DINERS_DE2_INVALID_FAILS_MOD_10_CHECK_MTI_11NN                   		"111"
#define DINERS_DE2_INVALID_FAILS_MOD_10_CHECK_ERROR_DATA_MTI_11NN        		"002A2"
#define DINERS_DE2_INVALID_PAN_LEN_MTI_11NN                      				"111"
#define DINERS_DE2_INVALID_PAN_LEN_ERROR_DATA_MTI_11NN			 				"002A1"
#define DINERS_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_MTI_11NN           		"111"
#define DINERS_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_ERROR_DATA_MTI_11NN		"002A4"
#define DINERS_DE2_INVALID_CYCLE_RANGE_SUSPENDED_MTI_11NN                     	"118"
#define DINERS_DE2_INVALID_CYCLE_RANGE_SUSPENDED_ERROR_DATA_MTI_11NN          	"002A4"
#define DINERS_DE2_INVALID_CYCLE_RANGE_CANCLE_MTI_11NN                      	"111"
#define DINERS_DE2_INVALID_CYCLE_RANGE_CANCLE_ERROR_DATA_MTI_11NN           	"002A4"
#define DINERS_DE2_INVALID_CARD_LOCAL_ONLY_MTI_11NN								"185"
#define DINERS_DE2_INVALID_CARD_LOCAL_ONLY_ERROR_DATA_MTI_11NN  				""
/***********************************************************************************/

/* MTI_13NN */
/***********************************************************************************/
#define DINERS_DE2_INVALID_FAILS_MOD_10_CHECK_MTI_13NN                   		"304"
#define DINERS_DE2_INVALID_FAILS_MOD_10_CHECK_ERROR_DATA_MTI_13NN        		"002A2"
#define DINERS_DE2_INVALID_PAN_LEN_MTI_13NN                      				"304"
#define DINERS_DE2_INVALID_PAN_LEN_ERROR_DATA_MTI_13NN			 				"002A1"
/***********************************************************************************/

/* MTI_14NN*/
/***********************************************************************************/
#define DINERS_DE2_INVALID_FAILS_MOD_10_CHECK_MTI_14NN                   		"482"
#define DINERS_DE2_INVALID_FAILS_MOD_10_CHECK_ERROR_DATA_MTI_14NN        		"002A2"
#define DINERS_DE2_INVALID_PAN_LEN_MTI_14NN                      				"481"
#define DINERS_DE2_INVALID_PAN_LEN_ERROR_DATA_MTI_14NN			 				"002A1"
#define DINERS_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_MTI_14NN           		"483"
#define DINERS_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_ERROR_DATA_MTI_14NN		"002A4"
/***********************************************************************************/

/* For other MTI*/
/***********************************************************************************/
#define DINERS_DE2_INVALID_PAN_LEN_MTI_OTH_TYPE									"904"
#define DINERS_DE2_INVALID_PAN_LEN_ERROR_DATA		            				"002A1"
/***********************************************************************************/
/*=============================================================================================*/

/*Processing Code DE 3*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE3_INVALID_PROCESSING_CODE										"904"
#define DINERS_DE3_INVALID_PROCESSING_CODE_ERROR_DATA							"003E1"
/***********************************************************************************/
/*=============================================================================================*/

/*Transaction Amount DE 4*/
/*=============================================================================================*/
/* For ALL MTI*/
/***********************************************************************************/
#define DINERS_DE4_INVALID_NON_DIGITS_AMT										"904"
#define DINERS_DE4_INVALID_NON_DIGITS_AMT_ERROR_DATA							"00461"
#define DINERS_DE4_INVALID_AMOUNT_EMV											"194"
#define DINERS_DE4_INVALID_AMOUNT_EMV_ERROR_DATA								"004E1"
/***********************************************************************************/

/*MTI_1100 */
/***********************************************************************************/
#define DINERS_DE4_INVALID_AMOUNT_MTI_1100										"110"
#define DINERS_DE4_INVALID_AMOUNT_ERROR_DATA_MTI_1100							"004E1"
/***********************************************************************************/

/*MTI_1200 */
/***********************************************************************************/
#define DINERS_DE4_INVALID_AMOUNT_MTI_1200										"110"
#define DINERS_DE4_INVALID_AMOUNT_ERROR_DATA_MTI_1200							"004F3"
/***********************************************************************************/

/*MTI_1420 */
/***********************************************************************************/
#define DINERS_DE4_INVALID_AMOUNT_MTI_1420										"484"
#define DINERS_DE4_INVALID_AMOUNT_ERROR_DATA_MTI_1420							"00461"
/***********************************************************************************/
/*=============================================================================================*/

/*Cardmember Billing Amount DE 6*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE6_INVALID_NON_DIGIT_CARDMEMBER_BILLING_AMOUNT					"904"
#define DINERS_DE6_INVALID_NON_DIGIT_CARDMEMBER_BILLING_AMOUNT_ERROR_DATA		"00661"
/***********************************************************************************/
/*=============================================================================================*/

/*Transmission Date And Time DE 7*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE7_INVALID_TRANSMISSION_DATE_TIME								"904"
#define DINERS_DE7_INVALID_TRANSMISSION_DATE_TIME_ERROR_DATA					"007E5"
/***********************************************************************************/
/*=============================================================================================*/

/*System Trace Audit Number DE 11*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE11_INVALID_NON_NUMERIC_STAN									"904"
#define DINERS_DE11_INVALID_NON_NUMERIC_STAN_ERROR_DATA							"01110"
/***********************************************************************************/
/*=============================================================================================*/

/*Local Transaction Date And Time DE 12*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE12_INVALID_LOCAL_TRANSMISSION_DATE_TIME						"904"
#define DINERS_DE12_INVALID_LOCAL_TRANSMISSION_DATE_TIME_ERROR_DATA				"012E5"
/***********************************************************************************/
/*=============================================================================================*/

/*Effective Date DE 13*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE13_INVALID_NON_NUMERIC_EFFECTIVE_DATE							"904"
#define DINERS_DE13_INVALID_NON_NUMERIC_EFFECTIVE_DATE_ERROR_DATA				"01361"
#define DINERS_DE13_INVALID_EFFECTIVE_DATE										"904"
#define DINERS_DE13_INVALID_EFFECTIVE_DATE_ERROR_DATA							"013E5"
/***********************************************************************************/
/*=============================================================================================*/

/*Expiration Date DE 14*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE14_INVALID_EXPIRY_DATE											"904"
#define DINERS_DE14_INVALID_EXPIRY_DATE_ERROR_DATA								"014E5"
/***********************************************************************************/
/*=============================================================================================*/

/*Card Acceptor Business Code DE 26*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE26_INVALID_NON_NUMERIC_CARD_ABC								"904"
#define DINERS_DE26_INVALID_NON_NUMERIC_CARD_ABC_ERROR_DATA						"02610"
/***********************************************************************************/
/*=============================================================================================*/

/*Original Amounts DE 30*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE30_INVALID_NON_NUMERIC_ORIGINAL_AMOUNTS						"904"
#define DINERS_DE30_INVALID_NON_NUMERIC_ORIGINAL_AMOUNTS_ERROR_DATA				"03061"
/***********************************************************************************/
/*=============================================================================================*/

/*Acquiring Institution Identification Code (IIC) DE 32*/
/*=============================================================================================*/
/***********************************************************************************/
/*MTI  ALL */
/***********************************************************************************/
#define DINERS_DE32_INVALID_NON_NUMERIC_ACQ_IIC									"904"
#define DINERS_DE32_INVALID_NON_NUMERIC_ACQ_IIC_ERROR_DATA						"03210"
/***********************************************************************************/

/*MTI_11NN */
/***********************************************************************************/
#define DINERS_DE32_INVALID_ACQ_NOT_KNOWN_MTI_11NN								"905"
#define DINERS_DE32_INVALID_ACQ_NOT_KNOWN_ERROR_DATA_MTI_11NN					"032L3"
/***********************************************************************************/
/*=============================================================================================*/

/*Forwarding Institution Identification Code (IIC) DE 33*/
/*=============================================================================================*/
/*MTI  ALL */
/***********************************************************************************/
#define DINERS_DE33_INVALID_NON_NUMERIC_FORWARDING_IIC							"904"
#define DINERS_DE33_INVALID_NON_NUMERIC_FORWARDING_IIC_ERROR_DATA				"03310"
/***********************************************************************************/

/*MTI_11NN */
/***********************************************************************************/
#define DINERS_DE33_INVALID_FORWARD_NOT_KNOWNP_MTI_11NN							"905"
#define DINERS_DE33_INVALID_FORWARD_NOT_KNOWNP_ERROR_DATA_MTI_11NN				"033L3"
#define DINERS_DE33_INVALID_FORWARD_NOT_VALID_ON_LINK_MTI_11NN					"905"
#define DINERS_DE33_INVALID_FORWARD_NOT_VALID_ON_LINK_ERROR_DATA_MTI_11NN		"033G6"
/***********************************************************************************/
/*=============================================================================================*/

/*Track 2 Data DE 35 */
/*=============================================================================================*/
/*MTI_1100 */
/***********************************************************************************/
#define DINERS_DE35_INVALID_PAN_MTI_1100										"129"
#define DINERS_DE35_INVALID_PAN_ERROR_DATA_MTI_1100								""
#define DINERS_DE35_INVALID_EXPIRY_DATE_MTI_1100								"129"
#define DINERS_DE35_INVALID_EXPIRY_DATE_ERROR_DATA_MTI_1100						""
#define DINERS_DE35_INVALID_EFFICTIVE_DATE_MTI_1100								"129"
#define DINERS_DE35_INVALID_EFFICTIVE_DATE_ERROR_DATA_MTI_1100					""
#define DINERS_DE35_INVALID_SERVICE_CODE_MTI_1100								"129"
#define DINERS_DE35_INVALID_SERVICE_CODE_ERROR_DATA_MTI_1100					""
/***********************************************************************************/
/*=============================================================================================*/

/*Approval Code DE 38*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE38_INVALID_APROVAL_CODE_RC_NOT_000_083_086_099					"904"
#define DINERS_DE38_INVALID_APROVAL_CODE_RC_NOT_000_083_086_099_ERROR_DATA		"038G3"
/***********************************************************************************/
/*=============================================================================================*/

/*Action Code DE 39*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE39_INVALID_NON_NUMERIC_ACTION_CODE								"904"
#define DINERS_DE39_INVALID_NON_NUMERIC_ACTION_CODE_ERROR_DATA					"03961"
/***********************************************************************************/
/*=============================================================================================*/

/*Track 1 Data DE 45*/
/*=============================================================================================*/
/*MTI_1100 */
/***********************************************************************************/
#define DINERS_DE45_INVALID_PAN_MTI_1100										"129"
#define DINERS_DE45_INVALID_PAN_ERROR_DATA_MTI_1100								""
#define DINERS_DE45_INVALID_EXPIRY_DATE_MTI_1100								"129"
#define DINERS_DE45_INVALID_EXPIRY_DATE_ERROR_DATA_MTI_1100						""
#define DINERS_DE45_INVALID_EFFICTIVE_DATE_MTI_1100								"129"
#define DINERS_DE45_INVALID_EFFICTIVE_DATE_ERROR_DATA_MTI_1100					""
#define DINERS_DE45_INVALID_SERVICE_CODE_MTI_1100								"129"
#define DINERS_DE45_INVALID_SERVICE_CODE_ERROR_DATA_MTI_1100					""
/***********************************************************************************/
/*=============================================================================================*/

/*Additional Data�Private DE 48*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE48_INVALID_NON_NUMERIC_TIMER									"904"
#define DINERS_DE48_INVALID_NON_NUMERIC_TIMER_ERROR_DATA						"04861"
/***********************************************************************************/
/*=============================================================================================*/

/*Transaction Currency Code DE 49*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE49_INVALID_UNKNOW_CURRENCY										"904"
#define DINERS_DE49_INVALID_UNKNOW_CURRENCY_ERROR_DATA							"04961"
/***********************************************************************************/
/*=============================================================================================*/

/*Personal Identification Number (PIN) Data DE 52*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE52_INVALID_FALIED_TO_TRANSLATE_PIN								"909"
#define DINERS_DE52_INVALID_FALIED_TO_TRANSLATE_PIN_ERROR_DATA					"052X0"
/***********************************************************************************/
/*=============================================================================================*/

/*Additional Amounts DE 54*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE54_INVALID_FORMATING											"904"
#define DINERS_DE54_INVALID_FORMATING_ERROR_DATA								"05461"
/***********************************************************************************/
/*=============================================================================================*/



/*Additional Auth Data DE 62*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE62_TAG05_VALUE_FOR_RPS											"DF051R"
#define DINERS_DE62_TAG05_VALUE_FOR_RPS_LEN											6

#define DINERS_DE62_TAG05_VALUE_FOR_PRE_AUTH									"DF0514"
#define DINERS_DE62_TAG05_VALUE_FOR_INC_PRE_AUTH								"DF051I"

#define DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR								"DF011T"
#define DINERS_DE62_TAG01_VALUE_FOR_TAP_ON_MOBILR_LEN								6
/***********************************************************************************/
/*=============================================================================================*/


/*Message Authentication Code (MAC) DE 64*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE64_INVALID_MAK_KEY												"916"
#define DINERS_DE64_INVALID_MAK_KEY_ERROR_DATA									"06461"
/***********************************************************************************/
/*=============================================================================================*/

/*Transaction Originator Institution Identification Code (IIC) DE 94*/
/*=============================================================================================*/
/***********************************************************************************/
#define DINERS_DE94_INVALID_FORWARD_ON_THIS_LINK								"905"
#define DINERS_DE94_INVALID_FORWARD_ON_THIS_LINK_ERROR_DATA						"094G6"
#define DINERS_DE94_INVALID_FORWARD_UNKNOWN_TO_DCI								"905"
#define DINERS_DE94_INVALID_FORWARD_UNKNOWN_TO_DCI_ERROR_DATA					"094L3"
/***********************************************************************************/
/*=============================================================================================*/

/*File Name DE 101*/
/*=============================================================================================*/
/*MTI_13NN */
/***********************************************************************************/
#define DINERS_DE101_INVALID_FILE_NAME											"309"
#define DINERS_DE101_INVALID_FILE_NAME_ERROR_DATA								"101E1"
/***********************************************************************************/
/*=============================================================================================*/


/*File Name DE 101*/
/*=============================================================================================*/
/*MTI_13NN */
/***********************************************************************************/
#define DINERS_DE101_INVALID_FILE_NAME											"309"
#define DINERS_DE101_INVALID_FILE_NAME_ERROR_DATA								"101E1"
/***********************************************************************************/
/*=============================================================================================*/
/************************************************************************************************/


/*File Name DE 122*/
/*=============================================================================================*/
/*MT_1100 */
/***********************************************************************************/
#define DINERS_DE122_AUTH_DATA_TYPE_PRODUCTBUY_VALUE							"2"
#define DINERS_DE122_AUTH_DATA_TYPE_PRODUCTBUY_VALUE_LEN						1

#define DINERS_DE122_ECI_VALUE_POS												1
#define DINERS_DE122_ECI_UNSPECFIED_VALUE										"0"
#define DINERS_DE122_ECI_SEC_ECOM_TRAN_VALUE									"5"
#define DINERS_DE122_ECI_NON_AUTH_SEC_ECOM_TRAN_WITH_MID_CAPABLE_VALUE			"6"
#define DINERS_DE122_ECI_NON_AUTH_SEC_ECOM_TRAN_VALUE							"7"
#define DINERS_DE122_ECI_NON_SEC_ECOM_TRAN_VALUE								"8"
#define DINERS_DE122_ECI_RESRV_VALUE											"9"
#define DINERS_DE122_ECI_VALUE_LEN												1

#define DINERS_DE122_3D_SEC_AUTH_RESULT_CODE_POS								2
#define DINERS_DE122_3D_SEC_SUCCUSS_VALUE										"00"
#define DINERS_DE122_3D_SEC_AUTH_NOT_PERFORMED_VALUE							"05"
#define DINERS_DE122_3D_SEC_ACQ_ATTEMPTED_VALUE									"07"
#define DINERS_DE122_3D_SEC_ACQ_ATTEMPTED_ISS_ACS_NOT_AVALABLE_VALUE			"08"
#define DINERS_DE122_3D_SEC_FAILED_VALUE										"09"
#define DINERS_DE122_3D_SEC_AUTH_RESULT_CODE_VALUE_LEN							2

#define DINERS_DE122_SECOND_FAC_AUTH_RESULT_CODE_POS							4
#define DINERS_DE122_SECOND_FAC_NO_AUTH_VALUE									"00"
#define DINERS_DE122_SECOND_FAC_AUTH_STAT_PASS_CODE								"01"
#define DINERS_DE122_SECOND_FAC_AUTH_SMS_OTP									"02"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_RDR_OTP								"03"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_APP_OTP								"04"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_OTHER_OTP								"05"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_KBA									"06"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_OOB_BIOMETRIC							"07"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_OOB_APP_LOGIN							"08"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_OTHER_OOB								"09"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_OTHER_AUTHN							"10"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_PUSH_CFRM								"11"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_DECOUPLED								"12"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_WEB_AUTHN								"13"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_SPC									"14"
#define DINERS_DE122_SECOND_FAC_AUTH_CARD_BEHVRL_BIOMETRIC						"15"
#define DINERS_DE122_SECOND_FAC_AUTH_3DS2_FRCTNLES_FLOW_RBA						"97"
#define DINERS_DE122_SECOND_FAC_AUTH_3DS2_FRCTNLES_FLOW_ATMPT_SERVER			"98"
#define DINERS_DE122_SECOND_FAC_AUTH_3DS2_FRCTNLES_FLOW_RBA_2					"99"
#define DINERS_DE122_SECOND_FAC_AUTH_RESULT_CODE_LEN							2

#define DINERS_DE122_CAVV_KEY_INDICATOR_POS										6
#define DINERS_DE122_CAVV_KEY_SET_1_VALUE										"01"
#define DINERS_DE122_CAVV_KEY_SET_2_VALUE										"02"
#define DINERS_DE122_CAVV_DESC_KEY_SET_1_VALUE									"10"
#define DINERS_DE122_CAVV_DESC_KEY_SET_2_VALUE									"11"
#define DINERS_DE122_CAVV_KEY_INDICATOR_LEN										2

#define DINERS_DE122_CAVV_POS													9
#define DINERS_DE122_CAVV_LEN													3

#define DINERS_DE122_UNPRIDICATABLE_NUM_POS										12
#define DINERS_DE122_UNPRIDICATABLE_NUM_LEN										4

#define DINERS_DE122_AUTH_TRACKING_NUM_POS										16
#define DINERS_DE122_AUTH_TRACKING_NUM_LEN										16

#define DINERS_DE122_VERSION_POS												32
#define DINERS_DE122_VERSION_AUTH_CODE_IP_ADD_NOT_PRESENT_VALUE                 "0"
#define DINERS_DE122_VERSION_AUTH_CODE_IP_ADD_PRESENT_VALUE                     "1"
#define DINERS_DE122_VERSION_LEN												1

#define DINERS_DE122_AUTH_ACTION_POS											33
#define DINERS_DE122_STAN_AUTH_PERFORMED_VALUE   					            "0"
#define DINERS_DE122_ADS_REG_AUTH_PERFORMED_VALUE                     			"1"
#define DINERS_DE122_ADS_FORGOT_PASSWORD_REG_AUTH_PERFORMED_VALUE     			"2"
#define DINERS_DE122_AUTH_ACTION_LEN											1

#define DINERS_DE122_IP_ADD_POS													34
#define DINERS_DE122_IP_ADD_LEN													8

#define POS_ENTRY_MODE_LEN														4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 								"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 							"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM							"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM						"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM					"0012"

#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN										3
#define BDO_POS_ACQ_TXN_HANDLER_QUEUE											"dcp"

#define ISO_MESSAGE_TYPE_LEN													4
#define POS_ACQ_SALE_MESSAGE_TYPE 												"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 												"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 											"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE									"0420"
#define DCI_AUTH_MESSAGE_TYPE													"1100"
#define DCI_ADVICE_MESSAGE_TYPE													"1120"
#define DCI_REVERSAL_MESSAGE_TYPE												"1420"


#define POS_CONDITION_CODE_LEN													2
#define POS_ECOM_CONDITION_CODE													"59"

/***********************************************************************************/
/*=============================================================================================*/
/*******************************************************************************************************************/

#endif
