#ifndef _GLOBALDEFINITION
#define _GLOBALDEFINITION
      
#include "app_info.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "equitdb.h"

#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  

#define BDOR_62_PREAUTH_VOID 1


/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1


#define MC02_APR2024	1
#define AMEX03B_APRIL2024 1
#define OCT_MAN_2024_BLD_2 1


#define MAX_DATASERVER_BUFFER_LENGTH 25000 

#define TRANSACTION_APPROVED        				"00"
#define REFER_TO_CARD_ISSUER       			 		"01"
#define RCI_SPECIAL_CONDITION      					"02"
#define INVALID_MERCHANT            				"03"
#define DO_NOT_HONOR                				"05"
#define INVALID_TRANSACTION         				"12"
#define INVALID_AMOUNT              				"13"
#define INVALID_ACCOUNT_NUMBER      				"14"
#define REENTER_TRANSACTION         				"19"
#define NO_SUFFICIENT_FUNDS         				"51"
#define EXPIRED_CARD                				"54"
#define INCORRECT_PIN               				"55"
#define TRANSACTION_NOT_PERMITTED   				"57"
#define EXCEEDS_AMOUNT_LIMIT        				"61"
#define EXCEEDS_WITHDRAWAL_LIMIT    				"65"
#define PIN_TRIES_EXCEEDED          				"75"
#define INCORRECT_CVV               				"82"
#define CANNOT_VERIFY_PIN           				"86"
#define DESTINATION_UNAVAILABLE     				"91"
#define SYSTEM_MALFUNCTION          				"96"
#define INVALID_TERMINAL_ID         				"89"
#define FORMAT_ERROR                				"30"
#define INITIATE_BATCH_UPLOAD       				"95"
#define UPI_PREAUTH_VOID_RESPONSE   				"12"
#define ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC 	"12"
#define INSUFFICIENT_PREAUTH_DECLINED_RC			"12"
#define EXPIRED_PREAUTH_DECLINED_RC					"12"

#define ISO_RESPONSE_CODE_LEN       				2

#define MAX_PREAUTH_VALIDITY_DAYS 	   				30
#define MAX_PREAUTH_VALIDITY_DAYS_LEN 				2
#define SMCC_MAX_PREAUTH_VALIDITY_DAYS				05
#define SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN 			2
#define MID_MCC_LEN									4
#define PARTIAL_REVERSAL_FLAG_OFF					"0"
#define PARTIAL_REVERSAL_FLAG_ON				    "1"
#define PARTIAL_REVERSAL_FLAG_LEN				    1

/* VISAK */
#define INCONSISTENT_DATA           "77"

#define FOUND_BIN_RANGE_IN_SHARED_MEMORY 1
#define SYSTEM_ERROR 6
#define SYSTEM_PROBLEM_READING_SHARED_MEMORY 10
#define RECORD_NOT_FOUND_IN_SHARED_MEMORY 11
#define RECORD_FOUND_IN_SHARED_MEMORY 12
#define SHARED_MEMORY_READ 13
#define DATABASE_READ 14
#define DEVICE_ERROR 15
#define DEVICE_OK 1
#define SEND_RESPONSE_TO_ORIGINATOR 20
#define ISO_PARSE_ERROR 21

#define BCH01_TOTALS_MATCHED 22
#define BCH01_TOTALS_NOT_MATCHED 23
#define BCH10_INSERT_SUCCESSFUL 24
#define TERMINAL_STATISTICS_SUCCESSFUL 25
#define UPI_AUTH_PRE_AUTHORIZATION_VOID_ERROR 26

#define SET_SYSTEM_MONITOR 1

#define RSB_CARD   "RS"

/* Bin Types */
#define CREDIT_BIN  'C'
#define DEBIT_BIN   'D'

/* Results for checking for duplicate batches */
#define  TRUE_DUPLICATE_BATCH   1
#define  FALSE_DUPLICATE_BATCH  2
#define  RC_INVALID_TXN        "58"
#define  RC_DUPLICATE_BATCH    "80"
#define  RC_DUPLICATE_BATCH_APPROVED   "Duplicate Batch - Accepted"
#define  RC_DUPLICATE_BATCH_DECLINED   "Duplicate Batch - Declined"

#define  ST2_DB_SELECT_DCF01             98
#define  ST2_DB_SELECT_DCF01_AUTO_INIT   99

#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20

/* SCR 22830 - ThoughtFocus */
#define E_COMMERCE_TRANSACTION 59


/*Preauth Validation parameter*/
#define RESPONSE_CODE_LEN 2
#define PERCENT_OVERLIMIT_VALUE_DEFAULT                    	"115"
#define PREAUTH_COMPLETION_VALIDATION_RESPONSE_ERROR_CODE  	"05"
#define EMV_CONTLESS_OFFLINE_SALE_AUTHID_VALUE             	"Y1"
#define PREAUTH_VALIDATION_PROCESSING_CODE                	"000000"
#define PROCESSING_CODE_LEN                                	6

#define AUTH_CARD_VERIFICATION_RESPONSE_PROC_CODE         	"380000"
#define AMEX_PREAUTH_VERIFICATION_RESPONSE_PROC_CODE      	"300000"

#define INITIAL_PREAUTH_TRA      						  	"PRIMRY"
#define INCREMENTAL_PREAUTH_TRA      					  	"INCPRA"
#define INITIAL_PREAUTH_TRA_WITH_INCREMENTAL                "PI"
#define INITIAL_PREAUTH_TRA_WITHOUT_INCREMENTAL             "WI"
#define PREAUTH_TRA_LEN  								  	6
#define PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG  				2

#define PRIMARY_AUTH 										'P'
#define INCREMENTAL_AUTH 									'I'
#define ESTIMATED_AUTH 										'E'
#define PARTIAL_REVERSAL 									'R'

#define  UPI_AUTH_PREAUT_VOID_ERROR_RESP_TEXT            	"Invalid Transaction"

// FOR CUP Void transaction handling
#define CUP_VOID_RESP_FLAG_VAL    	"VR"
#define CUP_VOID_RESP_FLAG_VAL_LEN  2
#define CUP_ORI_STAN_LEN            6

/*MP Purchase IT TYPE change*/
#define INVOICE_NUMBER_LEN 					6
#define PURCHASE_ID_TYPE_LEN  				6
#define PURCHASE_ID_DATA_POS				12
#define PURCHASE_ID_LEFTOVER_DATA_POS_1 	24
#define PURCHASE_ID_LEFTOVER_DATA_POS_2		36
#define PURCHASE_ID_DATA_LEN  				25

#endif 
