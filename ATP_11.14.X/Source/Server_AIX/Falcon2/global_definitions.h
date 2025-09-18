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

#define MAX_DATASERVER_BUFFER_LENGTH 25000 

#define TRANSACTION_APPROVED        "00"
#define REFER_TO_CARD_ISSUER        "01"
#define INVALID_MERCHANT            "03"
#define DO_NOT_HONOR                "05"
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

#define   WORKFLOW_DEFAULT_STRING 	"APC"
#define   WORKFLOW_DEFAULT_LEN 		3

/* SCR 22830 - ThoughtFocus */
#define E_COMMERCE_TRANSACTION 59

/*Card Type*/
#define CUP_CARD_NUM   "6"




#endif 
