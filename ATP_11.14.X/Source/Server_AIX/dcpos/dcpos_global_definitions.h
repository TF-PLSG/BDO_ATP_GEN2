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

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

#define BDOR_62_PREAUTH_VOID 1

#define MAX_DATASERVER_BUFFER_LENGTH 25000 

#define TRANSACTION_APPROVED        "00"
#define REFER_TO_CARD_ISSUER        "01"
#define INVALID_MERCHANT            "03"
#define INVALID_TRANSACTION         "12"
#define INVALID_AMOUNT              "13"
#define INVALID_ACCOUNT_NUMBER      "14"
#define REENTER_TRANSACTION         "19"
#define NO_SUFFICIENT_FUNDS         "51"
#define EXPIRED_CARD                "54"
#define INCORRECT_PIN               "55"
#define TRANSACTION_NOT_PERMITTED   "57"
#define EXCEEDS_AMOUNT_LIMIT        "61"
#define EXCEEDS_WITHDRAWAL_LIMIT    "65"
#define PIN_TRIES_EXCEEDED          "75"
#define INCORRECT_CVV               "82"
#define CANNOT_VERIFY_PIN           "86"
#define DESTINATION_UNAVAILABLE     "91"
#define SYSTEM_MALFUNCTION          "96"
#define INVALID_TERMINAL_ID         "89"
#define FORMAT_ERROR                "30"
#define INITIATE_BATCH_UPLOAD       "95"

/* VISAK */
#define INCONSISTENT_DATA           "77"

#define FOUND_BIN_RANGE_IN_SHARED_MEMORY 1
#define SYSTEM_ERROR 6
#define RECORD_FOUND_IN_SHARED_MEMORY_CREDIT 9
#define SYSTEM_PROBLEM_READING_SHARED_MEMORY 10
#define RECORD_NOT_FOUND_IN_SHARED_MEMORY 11
#define RECORD_FOUND_IN_SHARED_MEMORY 12
#define SHARED_MEMORY_READ 13
#define DATABASE_READ 14
#define DEVICE_ERROR 15
#define DEVICE_OK 1
#define SEND_RESPONSE_TO_ORIGINATOR 20
#define ISO_PARSE_ERROR 21
#define TERMINAL_STATISTICS_SUCCESSFUL 25
#define SET_SYSTEM_MONITOR 1


/* Results for checking for duplicate batches */
#define  RC_INVALID_TXN        "58"


#define  ST2_DB_SELECT_DCF01             98
#define  ST2_DB_SELECT_DCF01_AUTO_INIT   99

#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20

/* SCR 22830 - ThoughtFocus */
#define E_COMMERCE_TRANSACTION 59

/*Card Type*/
#define CUP_CARD_NUM   "6"

#define RESPONSE_CODE_LEN 								2

/*Debit Magstripe transaction*/
#define DEBIT_MAGSTRIPE_TRANSACTION_RESPONSE_CODE   	"69"
#define DEBIT_MAGSTRIPE_TRANSACTION_RESPONSE_TEXT   	"Debit Magstripe transaction"

/*Debit EMV transaction*/
#define DEBIT_EMV_TRANSACTION_RESPONSE_CODE         	"70"
#define DEBIT_EMV_TRANSACTION_RESPONSE_TEXT         	"Debit EMV transaction"

/* DCC Transaction defination  */
#define DCC_TERMINAL_STARTING_PREFIX_1 					"60"
#define DCC_TERMINAL_STARTING_PREFIX_2 					"10"
#define DCC_TERMINAL_STARTING_PREFIX_3 					"16"
#define DCC_TERMINAL_STARTING_PREFIX_4 					"13"
#define DCC_TERMINAL_STARTING_PREFIX_5 					"  "
#define DCC_TERMINAL_STARTING_PREFIX_6 					"  "
#define DCC_TERMINAL_STARTING_PREFIX_7 					"  "
#define DCC_TERMINAL_STARTING_PREFIX_8 					"  "
#define DCC_TERMINAL_STARTING_PREFIX_9 					"  "
#define DCC_TERMINAL_STARTING_PREFIX_10 				"  "

#define DCC_TERMINAL_STSRTING_PREFIX_LEN 				2
#define DCC_TRANSACTION_RESPONSE_CODE 					"71"
#define DCC_TRANSACTION_RESPONSE_TEXT 					"DCC Transaction"
#define CARD_DCC              							5

/*Dinner transaction*/
#define CARD_DINERS           							6
#define DINERS_TRANSACTION_RESPONSE_CODE            	"72"
#define DINERS_TRANSACTION_RESPONSE_TEXT            	"Dinner transaction"

/*Fleet Card transaction*/
#define CARD_FLEET            							8
#define FLEET_CARD_TRANSACTION_RESPONSE_CODE        	"73"
#define FLEET_CARD_TRANSACTION_RESPONSE_TEXT        	"Fleet Card transaction"

/*CUP Magstripe transaction*/
#define CUP_UPI_MAGSTRIPE_TRANSACTION_RESPONSE_CODE 	"74"
#define CUP_UPI_MAGSTRIPE_TRANSACTION_RESPONSE_TEXT 	"CUP Magstripe transaction"
#define CARD_CUP             							4

/*Dual Brand debit transaction*/
#define DUAL_BRAND_DEBIT_TRANSACTION_RESPONSE_CODE   	"60"
#define DUAL_BRAND_DEBIT_TRANSACTION_RESPONSE_TEXT   	"Dual Brand debit transaction"
#define CARD_DUAL             							7

/*Dual Brand debit transaction*/
#define AMEX_TRANSACTION_ON_BIN_NII_RESPONSE_CODE   	"79"
#define AMEX_TRANSACTION_ON_BIN_NII_RESPONSE_TEXT   	"Amex transaction on BIN NII"

/*No Bin Found*/
#define NO_BIN_FOUND_RESPONSE_ERROR_CODE   	            "05"
#define NO_BIN_FOUND_RESPONSE_ERROR_TEXT   	            "No Bin Found"

#define   TPOS01_SIZE           					sizeof(TPOS01)
#define   AUTH_TX_SIZE         						sizeof(AUTH_TX)
#define   SYSTEM_DATE_LEN           				8
#define   SALE_TRAN_MSG_TYPE                        "0200"
#define   PREAUTH_TRAN_MSG_TYPE                     "0100"
#define   ISO_MSG_TYPE_LEN                        	4
#define   ISO_VOID_PROCESSING_CODE					"02"
#define   ISO_VOID_PROCESSING_CODE_LEN				2
#endif 
