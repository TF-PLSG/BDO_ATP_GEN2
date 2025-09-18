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
 * AUTHOR:      Raghavendra

 ****************************************************************************/


/* Network Types */
#define   PULSE_ISSUER   1
#define   PULSE_ACQUIRER 2
#define   PULSE_BOTH     3

/* Currency Codes */
#define  PULSE_PESO     "608"
#define  PULSE_USD      "840"

/* Related Account Types */
#define PULSE_DOLLAR_ACCOUNT_TYPE  '5'
#define PULSE_PESO_ACCOUNT_TYPE    '6'


/* TRANSACTION SOURCE */
#define   PULSE_HOST  1

/* ASCII and EBCDIC conversion */
#define   A2E    "A2E"    /*  ASCII to EBCDIC conversion */
#define   E2A    "E2A"    /*  EBCDIC to ASCII conversion */

/* Error Codes */
#define   PULSE_TOO_MANY_ACTIVE_TRANSACTIONS   		"96"
#define   PULSE_TOO_MANY_TIMEOUTS              		"96"
#define   PULSE_HOST_UNAVAILABLE               		"91"
#define   PULSE_ISSUER_INOPERATIVE             		"96"
#define   PULSE_SYSTEM_ERROR                   		"96"
#define   PULSE_TO                             	  	"19"
#define   PULSE_TIMEOUT                        		"68"
#define   PULSE_INVALID_TXN                    		"58"
#define   PULSE_SECURITY_FAILURE               		"55"
#define   PULSE_AMX_INVALID_TXN                		"12"
#define   PULSE_FORMAT_ERROR                   		"30"
#define   PULSE_ISO_RESP_LEN 							2
#define   PULSE_APPROVED                      		"00"
#define   PULSE_NETMGMT_APPROVED              		"00"
#define   PULSE_FIELD_EDIT_ERROR              		"304"
#define   PULSE_HOST_UNAVAILABLE              		"912"
#define   PULSE_RACAL_APPROVED                		"00"
#define   PULSE_ISO_APPROVED                  		"00"

/* Country Codes */
#define   PULSE_USA_CURRENCY_CODE    				"840"
#define   PULSE_PESO_CURRENCY_CODE   				"608"


/* Transaction Flow */
#define   PULSE_SWITCHED_IN    						0
#define   PULSE_SWITCHED_OUT   						1

/* 08XX Request Types */
#define   PULSE_LOGON        						0
#define   PULSE_LOGOFF        						1
#define   PULSE_ECHO          						2
#define   PULSE_KEY_EXCHANGE						3
#define   PULSE_KEY_TEST  							4


/* 0800 function codes for DE70 Assuming ncpul as an ACQ only */
#define   PULSE_NMI_LOGON                 			"071"
#define   PULSE_NMI_LOGOFF                			"072"
#define   PULSE_NMI_LOGON_HOST                 		"061"
#define   PULSE_NMI_LOGOFF_HOST                		"062"
#define   PULSE_NMI_KEYCHANGE             			"171"
#define   PULSE_NMI_KEYTEST              			"181"
#define   PULSE_NMI_SYSTEM_ECHO                     "371"
#define   PULSE_NETWORK_FUNCTION_CODE_LEN			3
#define   PULSE_NMI_KEYCHANGE_ISS					"161"
#define   PULSE_NMI_ISS_ECHO						"361"



/* Echo Timer Set/Reset Values */
#define   PULSE_ECHO_TIMER_IS_SET      				1
#define   PULSE_ECHO_TIMER_IS_CLEARED  				0



/* Structure Sizes */
#define   AUTH_TX_SIZE         sizeof(AUTH_TX)
#define   NCF01_SIZE           sizeof(NCF01)
#define   NCF30_SIZE           sizeof(NCF30)
#define   ATM01_SIZE           sizeof(ATM01)
#define   TLF01_SIZE           sizeof(TLF01)
#define   CRF01_SIZE           sizeof(CRF01)
#define   ONLINE_UPDATES_SIZE  sizeof(AMEX_ONLINE_UPDATES)

/* Message Sizes */
#define   PULSE_MAX_HOST_MSG_SIZE    				750
#define   PULSE_MAX_APPL_MSG_SIZE  					20000

/* Transaction Tracing Flags */
#define  PULSE_TRACE_OFF      						0
#define  PULSE_TRACE_ON       						1

/* TIMER TYPES */
#define  PULSE_REQUEST_TIMER  0
#define  PULSE_ECHO_TIMER     1

/* TIMER APPS */
#define  PULSE_TIMER_REVERSAL   					"Reversal"
#define  PULSE_TIMER_FINANCIAL  					"Financial"
#define  PULSE_TIMER_08X0       					"0800"
#define  PULSE_TIMER_ECHO      		 				"Echo"

/*Message Type*/
#define PULSE_MESSAGE_TYPE_SIZE               		4
#define PULSE_NETWORK_REQ_MESSAGE					"0800"
#define PULSE_NETWORK_RESP_MESSAGE					"0810"
#define PULSE_SALE_REQ_MESSAGE						"0100"
#define PULSE_SALE_RESP_MESSAGE						"0110"
#define PULSE_ADVICE_REQ_MESSAGE					"0120"
#define PULSE_ADVICE_RESP_MESSAGE					"0130"
#define PULSE_REVERSAL_REQ_MESSAGE					"0420"
#define PULSE_REVERSAL_RESP_MESSAGE					"0430"
#define PULSE_ISO_SALE_REQ_MESSAGE_TYPE             "0200"
#define PULSE_ISO_SALE_RESP_MESSAGE_TYPE            "0210"
#define PULSE_ISO_AUTH_REQ_MESSAGE_TYPE             "0100"
#define PULSE_ISO_AUTH_RESP_MESSAGE_TYPE            "0110"
#define PULSE_ISO_REVESAL_REQ_MESSAGE_TYPE          "0400"
#define PULSE_ISO_REVESAL_RESP_MESSAGE_TYPE         "0410"
#define PULSE_ISO_SALE_COMPLETION_MESSAGE_TYPE      "0220"
#define PULSE_ADMINISTRATIVE_ADVICE_REQ_MESSAGE		"0620"
#define PULSE_ADMINISTRATIVE_ADVICE_RESP_MESSAGE	"0630"
#define PULSE_FILE_UPDATE_REQ_MESSAGE_TYPE          "1304"
#define PULSE_FILE_UPDATE_RESP_MESSAGE_TYPE         "1314"
#define PULSE_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE "1324"
#define PULSE_NEGATIVE_FILE_UPDATE_RESP_MESSAGE_TYPE"1334"
#define PULSE_CASH_ADVANCE_REQ_MESSAGE_TYPE         "0200"
#define PULSE_CASH_ADVANCE_RESP_MESSAGE_TYPE        "0210"
#define PULSE_BALANCE_INQUIRY_REQ_MESSAGE_TYPE		"0100"
#define PULSE_BALANCE_INQUIRY_RESP_MESSAGE_TYPE     "0110"
/* Shared Memory Tables */
#define  CRF01_TABLE    "Crf01Table"

/* Flags used when sending responses to host */
#define  PULSE_SEND_UNCONDITIONALLY  				0
#define  PULSE_SEND_IF_NOT_REVERSAL  				1

/* Miscellaneous */
#define  PULSE_COMM_BUF_SZE          				1500
#define  PULSE_EXTENDED_BITFIELD_SIZE  				128
#define  PULSE_EXTENDED_BITMAP_SIZE					16
#define  PULSE_NORMAL_BITFIELD_SIZE    				64
#define  PULSE_NORMAL_BITMAP_SIZE					8
#define  PULSE_LOOKUP_KEY_SIZE         				22
#define  PULSE_TOTALL_MEG_LEN						4
#define  PULSE_ETX_TRN_TEXT							"0x03"
#define  PULSE_KEY_SIZE								48

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

#define PULSE_DE2_LEN_SIZE   						2
#define PULSE_MAX_DE2_SIZE  						19
#define PULSE_DE3_SIZE       						6
#define PULSE_DE4_SIZE      						12
#define PULSE_DE5_SIZE 								4
#define PULSE_DE6_SIZE	  							12
#define PULSE_DE7_SIZE      						10
#define PULSE_DE9_SIZE								8
#define PULSE_DE10_SIZE								8
#define PULSE_DE12_SIZE								6
#define PULSE_DE11_SIZE      						6
#define PULSE_DE12_SIZE     						6
#define PULSE_DE13_SIZE      						4
#define PULSE_DE14_SIZE      						4
#define PULSE_DE15_SIZE 							4
#define PULSE_DE16_SIZE								4
#define PULSE_DE17_SIZE 							4
#define PULSE_DE18_SIZE								4
#define PULSE_DE19_SIZE								3
#define PULSE_DE22_SIZE     						3
#define PULSE_DE23_SIZE	   							3
#define PULSE_DE25_SIZE      						4
#define PULSE_DE26_SIZE	   							2
#define PULSE_DE28_SIZE								8
#define PULSE_DE29_SIZE								8
#define PULSE_DE39_SIZE     						2
#define PULSE_DE30_SIZE	   							24
#define PULSE_DE32_LEN_SIZE   						2
#define PULSE_MAX_DE32_SIZE  						11
#define PULSE_DE33_LEN_SIZE      					2
#define PULSE_MAX_DE33_SIZE  						11
#define PULSE_DE35_LEN_SIZE  						2
#define PULSE_MAX_DE35_SIZE  						37
#define PULSE_DE37_SIZE	   							12
#define PULSE_DE38_SIZE      						6
#define PULSE_DE39_SIZE     						2
#define PULSE_DE40_SIZE      						3
#define PULSE_DE41_SIZE      						8
#define PULSE_DE42_SIZE      						15
#define PULSE_DE43_LEN_SIZE							3
#define PULSE_DE43_SIZE								40
#define PULSE_DE48_SIZE				    			25
#define PULSE_DE44_LEN_SIZE							2
#define PULSE_DE46_LEN_SIZE							3
#define PULSE_DE47_LEN_SIZE							3
#define PULSE_DE48_LEN_SIZE							3
#define PULSE_DE49_SIZE								3
#define PULSE_DE50_SIZE			        			3
#define PULSE_DE51_SIZE								3
#define PULSE_DE52_SIZE								16
#define PULSE_DE54_LEN_SIZE							3
#define PULSE_DE55_LEN_SIZE							3
#define PULSE_DE56_LEN_SIZE							2
#define PULSE_DE58_SIZE								11
#define PULSE_DE60_LEN_SIZE							3
#define PULSE_DE63_LEN_SIZE							3
#define PULSE_DE70_SIZE								3
#define PULSE_DE96_SIZE								8
#define PULSE_DE114_LEN_SIZE						3

#define PULSE_HHMMSS_LEN							6
#define PULSE_MMDD_LEN								4
#define PULSE_MMDD_START_POS						4
#define PULSE_COUNTRY_CODE_FEILD_43					"PH"
#define PULSE_APPROVED								"00"

#define PULSE_YYYY_LEN								4
#define PULSE_MAX_DE44_SIZE							25
#define PULSE_MAX_DE48_SIZE							88
#define PULSE_MAX_DE54_SIZE							120
#define PULSE_DE58_SIZE								11
/*DE 23 Card Sequence Number  */
//=============================================================================================
#define PULSE_DE23_CARD_SEQ_POS_IN_STR									1
//=============================================================================================


/*DE 43 Card Acceptor Name/location */
//=============================================================================================
#define PULSE_MER_NAME_LEN_FEILD_43							23
#define PULSE_CITY_LEN_FEILD_43 							12
#define PULSE_STATE_CODE_LEN_FEILD_43						2
#define PULSE_COUNTY_CODE_LEN_FEILD_43			 			2
#define PULSE_ATM_LOC_LEN_FEILD_43			 				23
//=============================================================================================

/*DE 54 Card Acceptor Name/location */
//=============================================================================================
#define PULSE_DE54_ACCOUNT_TYPE_STARTING_POS									 2
#define PULSE_DE54_ACCOUNT_TYPE_LEN											 2
#define PULSE_DE54_AMOUNT_TYPE												"02"
#define PULSE_DE54_AMOUNT_TYPE_LEN											 2
#define PULSE_DE54_POSITIVE_AVAILABLE_BALANCE									"C"
#define PULSE_DE54_NEGATIVE_AVAILABLE_BALANCE									"D"
#define PULSE_DE54_AVAILABLLE_BALANCE_LEN										 1
//=============================================================================================

/*Time and date related values*/
#define PULSE_MAX_MONTH_IN_YEAR					12
#define PULSE_YY_LEN								2
#define PULSE_MM_LEN								2
#define PULSE_DD_LEN								2
#define PULSE_MMDD_LEN 							4
#define PULSE_YYMMDD_LEN 							6
#define PULSE_YYYY_LEN 							4
#define PULSE_MMDD_START_POS 						4
#define PULSE_YY_START_POS 						2
#define PULSE_HHMMSS_LEN 							6
#define PULSE_YYYYMMDD_LEN							8

/*Handler Queue */
#define PULSE_POS_HANDLER_MODULE_COMP_LEN     		3
#define PULSE_ISSUING_MODULE_COMP_LEN  				5
#define PULSE_VOICE_MODULE_COMP_LEN    				5
#define PULSE_SBATCH_MODULE_COMP_LEN    			4
#define PULSE_CANDENCIE_LINK_QUEUE_NAME_LEN         6
#define IST_MODULE_COMP_LEN  						5

/*Transaction type*/
#define PULSE_RPS_INDUSTRY_CODE 			      "800"
#define PULSE_RPS_INDUSTRY_CODE_LEN 			   3

/*Fields related definition*/

/*DE 3 Related definition  */
//=============================================================================================
#define PULSE_DE3_PROC_CODE_PIN_CHANGE 						"98"
#define PULSE_DE3_PROC_CODE_PIN_UNBLOCK						"99"
#define PULSE_DE3_PROC_CODE_BALANCE_INQ						"31"
#define PULSE_DE3_PROC_CODE_CARD_ACC_VERIFICATION_REQ   	"18"
#define PULSE_DE3_PROC_CODE_ISO_AUTH_AUTHORIZATION_VAL		"00"
#define PULSE_DE3_PROC_CODE_ISO_AUTH_SALE_VAL				"00"
#define PULSE_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL			"02"
#define PULSE_DE3_PROC_CODE_ISO_AUTH_BAL_INQ_VAL			"30"
#define PULSE_DE3_PROC_CODE_ISO_CARD_VERIFICATION_INQ_VAL 	"38"
#define PULSE_DE3_PROC_CODE_ISO_PRE_AUTH_VAL  				"30"
#define PULSE_DE3_PROC_CODE_ISO_AUTH_CASH_ADV_VAL			"01"
#define PULSE_DE3_PROC_CODE_ISO_AUTH_QUASI_CASH_VAL			"11"
#define PULSE_DE3_PROC_CODE_FIRST_2_DIGIT_LEN				2
#define PULSE_DE3_PROC_CODE_CASH_ADV_VAL                	"01"
//=============================================================================================

/*DE 22 Point Of Service Data Code  */
//=============================================================================================
/*ISO BDO POS TERMINAL*/
#define PULSE_BDO_POS_ENTRY_MODE_LEN										3
#define PULSE_BDO_POS_ENTRY_MODE_UNKNOW									"000"
#define PULSE_BDO_POS_ENTRY_MODE_MANUAL									"001"
#define PULSE_BDO_POS_ENTRY_MODE_MAGSTRIP1								"002"
#define PULSE_BDO_POS_ENTRY_MODE_MAGSTRIP2								"006"
#define PULSE_BDO_POS_ENTRY_MODE_MAGSTRIP3								"090"
#define PULSE_BDO_POS_ENTRY_MODE_MAGSTRIP4								"008"
#define PULSE_BDO_POS_ENTRY_MODE_ICC_EMV								"005"
#define PULSE_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV						"007"
#define PULSE_BDO_POS_ENTRY_MODE_FALLBACK								"080"
#define PULSE_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE					"091"
#define PULSE_BDO_POS_ENTRY_MODE_WITH_PIN								"1"
#define PULSE_BDO_POS_ENTRY_MODE_WITHOUT_PIN							"0"
#define PULSE_BDO_POS_ENTRY_MODE_PIN_LEN								1

/***********************************************************************************/
/*POS 1*/
/***********************************************************************************/
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_UNKNOW					'0'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_MANUAL					'1'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP					'2'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_BARCODE					'3'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_OCT						'4'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_ICC						'5'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_KET_ENTERED				'6'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS				'8'
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL			'9'
/***********************************************************************************/
/***********************************************************************************/
/*POS 2*/
#define PULSE_DE22_POST_2_CARDHOLDER_AUTHENTICATION_CAPABILITY_PIN		'1'
/***********************************************************************************/
/*POS 6*/
/***********************************************************************************/
#define PULSE_DE22_POST_6_CARD_NOT_PRESENT								'0'
#define PULSE_DE22_POST_6_CARD_PRESENT									'1'
#define PULSE_DE22_POST_6_CARD_PRESENT_UNKNOW							'8'
/***********************************************************************************/
/*POS 7*/
/***********************************************************************************/
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_UNSPECIFIED					'0'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_MANUAL						'1'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_READ				'2'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_BAR_CODE						'3'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_OCR							'4'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_ICC							'5'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_KEY_ENTERED					'6'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_FALLBACK				'9'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_CARD_DATA_RFID			'S'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_DATA_RFID			'T'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_CONTACTLESS_CHANGED_TO_ICC	'U'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_RESERVED_ISO_USE				'7'
#define PULSE_DE22_POST_7_CARD_DATA_INPUT_MODE_RESERVED_NATION_USE			'8'
/***********************************************************************************/

/*MAGSTIPR POS DATA FOR DCI*/
/***********************************************************************************/
#define PULSE_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP_STR				"2"
#define PULSE_DE22_POST_1_
/***********************************************************************************/
//=============================================================================================

/*DE 23 Card Sequence Number  */
//=============================================================================================
#define PULSE_DE23_CARD_SEQ_POS_IN_STR									1
//=============================================================================================

/*MTI 1420*/
/***********************************************************************************/
#define PULSE_DE24_FUNCION_CODE_MTI_1420_400							"400"
#define PULSE_DE24_FUNCION_CODE_MTI_1420_401							"401"
#define PULSE_DE24_FUNCION_CODE_MTI_1420_440							"440"
/***********************************************************************************/

/*MTI 1624*/
/***********************************************************************************/
#define PULSE_DE24_FUNCION_CODE_MTI_1624_640							"640"
/***********************************************************************************/

/*DE 39 Action Code */
//=============================================================================================
#define  PULSE_NUMBER_OF_ACTION_CODES   63
//=============================================================================================




/*DE 44 Additional Response Data */
//=============================================================================================
/***********************************************************************************/
/* For CVV indicator*/
#define PULSE_DE44_CVVV1_ON_STRIPE_CORRECT										"001"
#define PULSE_DE44_CVVV1_ON_STRIPE_NOT_CORRECT									"002"
#define PULSE_DE44_CVVV1_MISSING												"003"
#define PULSE_DE44_CVVV2_CORRECT												"006"
#define PULSE_DE44_CVVV2_NOT_CORRECT											"007"
#define PULSE_DE44_CVVV1_CVV2_CORRECT											"010"
#define PULSE_DE44_CVVV1_CVV2_NOT_CORRECT										"011"
#define PULSE_DE44_CVVV1_CORRECT_CVV2_NOT_CORRECT								"012"
#define PULSE_DE44_CVVV1_NOT_CORRECT_CVV2_CORRECT								"013"
#define PULSE_DE44_CVVV1_MISSING_CVV2_CORRECT									"014"
#define PULSE_DE44_CVVV1_MISSING_CVV2_NOT_CORRECT								"015"
#define PULSE_DE44_DCVVV_VALID													"016"
#define PULSE_DE44_DCVVV_INVALID												"017"
#define PULSE_DE44_UNABLE_TO_VALIDATE_CVV										"018"
#define PULSE_DE44_NEITHER_CVV_WAS_CHACKED										""
#define PULSE_DE44_CVV_INDICATOR_LEN											 3
/***********************************************************************************/
/***********************************************************************************/
/* CAVV Result Code */
#define PULSE_DE44_CAVV_PASSED_VERIFICATION_AUTHENTICATION						"01"
#define PULSE_DE44_CAVV_FAILED_VERIFICATION_AUTHENTICATION						"02"
#define PULSE_DE44_CAVV_PASSED_VALIDATION_ATTEMPT								"03"
#define PULSE_DE44_CAVV_FAILED_VALIDATION_ATTEMPT								"04"
#define PULSE_DE44_CAVV_NOT_VALIDATED_ISSUER_NOT_PARTICIPATING					"05"										"04"
#define PULSE_DE44_CAVV_UNABLE_TO_PERFORM_AUTHENTICATION						"06"										"04"
#define PULSE_DE44_CAVV_RESULT_CODE_LEN										 2
/***********************************************************************************/

/***********************************************************************************/
/* Error Data element */
#define PULSE_DE44_NON_NUMERIC_CHAR_FOUND										"10"
#define PULSE_DE44_INVALID_NUMBER_FORMAT										"11"
#define PULSE_DE44_REQ_OWNER_CURRENCY_CONVERSION_AMOUT_T0_LARGE					"15"
#define PULSE_DE44_TRUNCATION_NO_MOVEMENT_DATA									"20"
#define PULSE_DE44_INVALID_INFORMATION_CHECK_DATA_ELEMENT						"61"
#define PULSE_DE44_ACCOUNT_NUMBER_INVALID										"A1"
#define PULSE_DE44_ACCOUNT_NUMBER_FAILD_MOD_10_CHECK							"A2"
#define PULSE_DE44_LOCAL_ONLY_CYCLE_RANGE										"A3"
#define PULSE_DE44_UNKNOWN_CYCLE_RANGE											"A4"
#define PULSE_DE44_EDIT_ERROR_DATA_ELEMENT_CONTAINS_OUT_OF_RANGE_VALUE			"E1"
#define PULSE_DE44_INVALID_FUNCTION_CODE										"E4"
#define PULSE_DE44_INVALID_DATE_TIME											"E5"
#define PULSE_DE44_VALUE_NOT_ALLOWED_TO_REQUESTER								"E6"
#define PULSE_DE44_NEGATIVE_AMOUNT												"F3"
#define PULSE_DE44_INVALID_ABC													"F9"
#define PULSE_DE44_ORIGINAL_VALUE_INVALID										"FA"
#define PULSE_DE44_ACTION_NOT_ALLOWED											"G0"
#define PULSE_DE44_INVALID_REQUEST_REQUESTER_MUST_BE_OWNER						"G1"
#define PULSE_DE44_WARN_BULLETINE_CONTAINS_INVALID_DISTRIBUTION					"G2"
#define PULSE_DE44_FUNC_NOT_EXPECT_DATA_ELEMENT_TO_CONTAIN_VALUE				"G3"
#define PULSE_DE44_DUPLICATE_STAN												"G5"
#define PULSE_DE44_MESSAGE_NOT_AGREE_WITH_LINE_OWNER							"G6"
#define PULSE_DE44_MANDATORY_DATA_ELEMENT_NOT_PRESENT							"G7"
#define PULSE_DE44_INVALID_STAN_IN_LIMIT										"L2"
#define PULSE_DE44_INVALID_ACQ_IIC												"L3"
#define PULSE_DE44_RECORD_NOT_FOUND												"R1"
#define PULSE_DE44_RECORD_ALREADY_EXISTS										"R2"
#define PULSE_DE44_ENTRY_NOT_FOUND												"R3"
#define PULSE_DE44_XPRESS_SYSTEM_ERROR											"X0"

/***********************************************************************************/

/***********************************************************************************/
/* Cryptogram Validation Result Code */
#define PULSE_DE44_CRYPTOGRAM_VALIDATION_PASS									"01"
#define PULSE_DE44_CRYPTOGRAM_VALIDATION_FAILED									"02"
#define PULSE_DE44_CRYPTOGRAM_UNABLE_TO_VALIDAT									"03"
#define PULSE_DE44_CRYPTOGRAM_VALIDATION_RESULT_CODE							 2
/***********************************************************************************/

/***********************************************************************************/
/* TVR Check Result Code */
#define PULSE_DE44_TVR_CHECK_NOT_TESTED											"0"
#define PULSE_DE44_TVR_CHECK_PASS												"1"
#define PULSE_DE44_TVR_CHECK_FAILED												"2"
#define PULSE_DE44_TVR_CHECK_RESULT_LEN											 2
/***********************************************************************************/

/***********************************************************************************/
/* CVR Check Result Code */
#define PULSE_DE44_CVR_CHECK_NOT_TESTED											"0"
#define PULSE_DE44_CVR_CHECK_PASS												"1"
#define PULSE_DE44_CVR_CHECK_FAILED												"2"
#define PULSE_DE44_CVR_CHECK_RESULT_LEN											 2
/***********************************************************************************/

//=============================================================================================


/*DE 96 Key Management Data */
//=============================================================================================
#define PULSE_DE96_KEY_LEN									 				 32
#define PULSE_DE96_KEY_CHECK_LEN											 4
//=============================================================================================




/*******************************************************************************************************************/
/*Format Error Conditions , Codes and  Error data*/
/*Primary Account Number DE 2*/
//=============================================================================================
/* For ALL MTI*/
/***********************************************************************************/
#define PULSE_DE2_INVALID_NON_DIGIT_PAN		                        		"904"
#define PULSE_DE2_INVALID_NON_DIGIT_PAN_ERROR_DATA		            		"002A1"
/***********************************************************************************/

/*MTI_11NN */
/***********************************************************************************/
#define PULSE_DE2_INVALID_FAILS_MOD_10_CHECK_MTI_11NN                   		 "111"
#define PULSE_DE2_INVALID_FAILS_MOD_10_CHECK_ERROR_DATA_MTI_11NN        		 "002A2"
#define PULSE_DE2_INVALID_PAN_LEN_MTI_11NN                      				 "111"
#define PULSE_DE2_INVALID_PAN_LEN_ERROR_DATA_MTI_11NN			 				 "002A1"
#define PULSE_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_MTI_11NN           		 "111"
#define PULSE_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_ERROR_DATA_MTI_11NN		 "002A4"
#define PULSE_DE2_INVALID_CYCLE_RANGE_SUSPENDED_MTI_11NN                      	 "118"
#define PULSE_DE2_INVALID_CYCLE_RANGE_SUSPENDED_ERROR_DATA_MTI_11NN          	 "002A4"
#define PULSE_DE2_INVALID_CYCLE_RANGE_CANCLE_MTI_11NN                      		 "111"
#define PULSE_DE2_INVALID_CYCLE_RANGE_CANCLE_ERROR_DATA_MTI_11NN           		 "002A4"
#define PULSE_DE2_INVALID_CARD_LOCAL_ONLY_MTI_11NN								 "185"
#define PULSE_DE2_INVALID_CARD_LOCAL_ONLY_ERROR_DATA_MTI_11NN  					 ""
/***********************************************************************************/

/* MTI_13NN */
/***********************************************************************************/
#define PULSE_DE2_INVALID_FAILS_MOD_10_CHECK_MTI_13NN                   		"304"
#define PULSE_DE2_INVALID_FAILS_MOD_10_CHECK_ERROR_DATA_MTI_13NN        		"002A2"
#define PULSE_DE2_INVALID_PAN_LEN_MTI_13NN                      				"304"
#define PULSE_DE2_INVALID_PAN_LEN_ERROR_DATA_MTI_13NN			 				"002A1"
/***********************************************************************************/

/* MTI_14NN*/
/***********************************************************************************/
#define PULSE_DE2_INVALID_FAILS_MOD_10_CHECK_MTI_14NN                   		"482"
#define PULSE_DE2_INVALID_FAILS_MOD_10_CHECK_ERROR_DATA_MTI_14NN        		"002A2"
#define PULSE_DE2_INVALID_PAN_LEN_MTI_14NN                      				"481"
#define PULSE_DE2_INVALID_PAN_LEN_ERROR_DATA_MTI_14NN			 				"002A1"
#define PULSE_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_MTI_14NN           		"483"
#define PULSE_DE2_INVALID_CYCLE_RANGE_DOES_NOT_EXICTS_ERROR_DATA_MTI_14NN		"002A4"
/***********************************************************************************/

/* For other MTI*/
/***********************************************************************************/
#define PULSE_DE2_INVALID_PAN_LEN_MTI_OTH_TYPE									"904"
#define PULSE_DE2_INVALID_PAN_LEN_ERROR_DATA		            				"002A1"
/***********************************************************************************/
//=============================================================================================

/*Processing Code DE 3*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE3_INVALID_PROCESSING_CODE										"904"
#define PULSE_DE3_INVALID_PROCESSING_CODE_ERROR_DATA							"003E1"
/***********************************************************************************/
//=============================================================================================

/*Transaction Amount DE 4*/
//=============================================================================================
/* For ALL MTI*/
/***********************************************************************************/
#define PULSE_DE4_INVALID_NON_DIGITS_AMT										"904"
#define PULSE_DE4_INVALID_NON_DIGITS_AMT_ERROR_DATA								"00461"
#define PULSE_DE4_INVALID_AMOUNT_EMV											"194"
#define PULSE_DE4_INVALID_AMOUNT_EMV_ERROR_DATA									"004E1"
/***********************************************************************************/

/*MTI_1100 */
/***********************************************************************************/
#define PULSE_DE4_INVALID_AMOUNT_MTI_1100										"110"
#define PULSE_DE4_INVALID_AMOUNT_ERROR_DATA_MTI_1100							"004E1"
/***********************************************************************************/

/*MTI_1200 */
/***********************************************************************************/
#define PULSE_DE4_INVALID_AMOUNT_MTI_1200										"110"
#define PULSE_DE4_INVALID_AMOUNT_ERROR_DATA_MTI_1200							"004F3"
/***********************************************************************************/

/*MTI_1420 */
/***********************************************************************************/
#define PULSE_DE4_INVALID_AMOUNT_MTI_1420										"484"
#define PULSE_DE4_INVALID_AMOUNT_ERROR_DATA_MTI_1420							"00461"
/***********************************************************************************/
//=============================================================================================

/*Cardmember Billing Amount DE 6*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE6_INVALID_NON_DIGIT_CARDMEMBER_BILLING_AMOUNT					"904"
#define PULSE_DE6_INVALID_NON_DIGIT_CARDMEMBER_BILLING_AMOUNT_ERROR_DATA		"00661"
/***********************************************************************************/
//=============================================================================================

/*Transmission Date And Time DE 7*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE7_INVALID_TRANSMISSION_DATE_TIME								"904"
#define PULSE_DE7_INVALID_TRANSMISSION_DATE_TIME_ERROR_DATA						"007E5"
/***********************************************************************************/
//=============================================================================================

/*System Trace Audit Number DE 11*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE11_INVALID_NON_NUMERIC_STAN									"904"
#define PULSE_DE11_INVALID_NON_NUMERIC_STAN_ERROR_DATA						"01110"
/***********************************************************************************/
//=============================================================================================

/*Local Transaction Date And Time DE 12*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE12_INVALID_LOCAL_TRANSMISSION_DATE_TIME						"904"
#define PULSE_DE12_INVALID_LOCAL_TRANSMISSION_DATE_TIME_ERROR_DATA			"012E5"
/***********************************************************************************/
//=============================================================================================

/*Effective Date DE 13*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE13_INVALID_NON_NUMERIC_EFFECTIVE_DATE							"904"
#define PULSE_DE13_INVALID_NON_NUMERIC_EFFECTIVE_DATE_ERROR_DATA				"01361"
#define PULSE_DE13_INVALID_EFFECTIVE_DATE										"904"
#define PULSE_DE13_INVALID_EFFECTIVE_DATE_ERROR_DATA							"013E5"
/***********************************************************************************/
//=============================================================================================

/*Expiration Date DE 14*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE14_INVALID_EXPIRY_DATE											"904"
#define PULSE_DE14_INVALID_EXPIRY_DATE_ERROR_DATA								"014E5"
/***********************************************************************************/
//=============================================================================================

/*Card Acceptor Business Code DE 26*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE26_INVALID_NON_NUMERIC_CARD_ABC								"904"
#define PULSE_DE26_INVALID_NON_NUMERIC_CARD_ABC_ERROR_DATA					"02610"
/***********************************************************************************/
//=============================================================================================

/*Original Amounts DE 30*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE30_INVALID_NON_NUMERIC_ORIGINAL_AMOUNTS						"904"
#define PULSE_DE30_INVALID_NON_NUMERIC_ORIGINAL_AMOUNTS_ERROR_DATA			"03061"
/***********************************************************************************/
//=============================================================================================

/*Acquiring Institution Identification Code (IIC) DE 32*/
//=============================================================================================
/***********************************************************************************/
/*MTI  ALL */
/***********************************************************************************/
#define PULSE_DE32_INVALID_NON_NUMERIC_ACQ_IIC									"904"
#define PULSE_DE32_INVALID_NON_NUMERIC_ACQ_IIC_ERROR_DATA						"03210"
/***********************************************************************************/

/*MTI_11NN */
/***********************************************************************************/
#define PULSE_DE32_INVALID_ACQ_NOT_KNOWN_MTI_11NN								"905"
#define PULSE_DE32_INVALID_ACQ_NOT_KNOWN_ERROR_DATA_MTI_11NN					"032L3"
/***********************************************************************************/
//=============================================================================================

/*Forwarding Institution Identification Code (IIC) DE 33*/
//=============================================================================================
/*MTI  ALL */
/***********************************************************************************/
#define PULSE_DE33_INVALID_NON_NUMERIC_FORWARDING_IIC							"904"
#define PULSE_DE33_INVALID_NON_NUMERIC_FORWARDING_IIC_ERROR_DATA				"03310"
/***********************************************************************************/

/*MTI_11NN */
/***********************************************************************************/
#define PULSE_DE33_INVALID_FORWARD_NOT_KNOWNP_MTI_11NN							"905"
#define PULSE_DE33_INVALID_FORWARD_NOT_KNOWNP_ERROR_DATA_MTI_11NN				"033L3"
#define PULSE_DE33_INVALID_FORWARD_NOT_VALID_ON_LINK_MTI_11NN					"905"
#define PULSE_DE33_INVALID_FORWARD_NOT_VALID_ON_LINK_ERROR_DATA_MTI_11NN		"033G6"
/***********************************************************************************/
//=============================================================================================

/*Track 2 Data DE 35 */
//=============================================================================================
/*MTI_1100 */
/***********************************************************************************/
#define PULSE_DE35_INVALID_PAN_MTI_1100										"129"
#define PULSE_DE35_INVALID_PAN_ERROR_DATA_MTI_1100							""
#define PULSE_DE35_INVALID_EXPIRY_DATE_MTI_1100								"129"
#define PULSE_DE35_INVALID_EXPIRY_DATE_ERROR_DATA_MTI_1100					""
#define PULSE_DE35_INVALID_EFFICTIVE_DATE_MTI_1100							"129"
#define PULSE_DE35_INVALID_EFFICTIVE_DATE_ERROR_DATA_MTI_1100				""
#define PULSE_DE35_INVALID_SERVICE_CODE_MTI_1100							"129"
#define PULSE_DE35_INVALID_SERVICE_CODE_ERROR_DATA_MTI_1100					""
/***********************************************************************************/
//=============================================================================================

/*Approval Code DE 38*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE38_INVALID_APROVAL_CODE_RC_NOT_000_083_086_099					"904"
#define PULSE_DE38_INVALID_APROVAL_CODE_RC_NOT_000_083_086_099_ERROR_DATA		"038G3"
/***********************************************************************************/
//=============================================================================================

/*Action Code DE 39*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE39_INVALID_NON_NUMERIC_ACTION_CODE								"904"
#define PULSE_DE39_INVALID_NON_NUMERIC_ACTION_CODE_ERROR_DATA					"03961"
/***********************************************************************************/
//=============================================================================================

/*Additional Data—Private DE 48*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE48_INVALID_NON_NUMERIC_TIMER									"904"
#define PULSE_DE48_INVALID_NON_NUMERIC_TIMER_ERROR_DATA							"04861"
/***********************************************************************************/
//=============================================================================================

/*Transaction Currency Code DE 49*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE49_INVALID_UNKNOW_CURRENCY										"904"
#define PULSE_DE49_INVALID_UNKNOW_CURRENCY_ERROR_DATA							"04961"
/***********************************************************************************/
//=============================================================================================

/*Personal Identification Number (PIN) Data DE 52*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE52_INVALID_FALIED_TO_TRANSLATE_PIN								"909"
#define PULSE_DE52_INVALID_FALIED_TO_TRANSLATE_PIN_ERROR_DATA					"052X0"
/***********************************************************************************/
//=============================================================================================

/*Additional Amounts DE 54*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE54_INVALID_FORMATING											"904"
#define PULSE_DE54_INVALID_FORMATING_ERROR_DATA									"05461"
/***********************************************************************************/
//=============================================================================================


/*Message Authentication Code (MAC) DE 64*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE64_INVALID_MAK_KEY												"916"
#define PULSE_DE64_INVALID_MAK_KEY_ERROR_DATA									"06461"
/***********************************************************************************/
//=============================================================================================

/*Transaction Originator Institution Identification Code (IIC) DE 94*/
//=============================================================================================
/***********************************************************************************/
#define PULSE_DE94_INVALID_FORWARD_ON_THIS_LINK									"905"
#define PULSE_DE94_INVALID_FORWARD_ON_THIS_LINK_ERROR_DATA						"094G6"
#define PULSE_DE94_INVALID_FORWARD_UNKNOWN_TO_DCI								"905"
#define PULSE_DE94_INVALID_FORWARD_UNKNOWN_TO_DCI_ERROR_DATA					"094L3"
/***********************************************************************************/
//=============================================================================================

/*File Name DE 101*/
//=============================================================================================
/*MTI_13NN */
/***********************************************************************************/
#define PULSE_DE101_INVALID_FILE_NAME											"309"
#define PULSE_DE101_INVALID_FILE_NAME_ERROR_DATA								"101E1"
/***********************************************************************************/
//=============================================================================================

/*******************************************************************************************************************/



