#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <sys/timeb.h>
#include <signal.h>

#include "basictyp.h"
#include "pte.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "ntutils.h"
#include "logger.h"
#include "app_info.h"
#include "dbcommon.h"
#include "tx_database.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "global_definitions.h"
#include "txutils.h"
#include "dc_database.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "dbcommon.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"
#include "txtrace.h"

#define   MAX_MERCHANTS		 100
#define   VALIDATION_ERROR	 -2
#define   ERROR				 10
#define   UPDATED            11
#define   DELETED			 12
#define   VISA				 "V"
#define   MC				 "M"

#define MAXCOUNT	"2"
#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

#define BDOR_62_PREAUTH_VOID 1

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

/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

extern CHAR ExeName[100];
extern CHAR ServiceName[100];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
PRIVATE CHAR ErrorMsg  [100];
CHAR DB_Logging_Filename[256];
INT  db_error_statics_flag;
double db_error_statics_value;
CHAR  DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
CHAR  AppName[MAX_APP_NAME_SIZE];

#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"

#define  PESO     "608"
#define  USD      "840"

/*ISO Response Code*/
#define ISO_RESP_LEN 					2
#define TRANSACTION_APPROVED         "00"
#define REFER_TO_CARD_ISSUER         "01"
#define REENTER_TRANSACTION          "19"
#define DESTINATION_UNAVAILABLE      "91"
#define DUPLICATE_TRANSACTION        "94"

/*Action code for DCI and Amex*/
#define ACTION_CODE_LEN 				3
#define DUPLICATE_TRANSACTION_AMEX_ACTION_CODE        "913"
#define DUPLICATE_TRANSACTION_DCI_ACTION_CODE         "182"

static long	total_tx_num;
static long	advice_repeat_ctr;

#define ISO_RESPONSE_CODE_LEN 2
#define MAX_PREAUTH_VALIDITY_DAYS 30

#define SMCC_MAX_PREAUTH_VALIDITY_DAYS				05
#define SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN 			2


//common functions declarations
INT   get_dataserver_ini_db_error_statics_value(pINT, double *, pCHAR, pCHAR  );
void  Create_Db_Statics_And_Db_Oracle_Error_Filename();
void  Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR);
void  parvbh_log_message(INT, INT, pCHAR, pCHAR);
INT   parvbh_Log_error_warning_to_File(pCHAR, int, pCHAR);
char* mask_card_number(char *);
void  get_date( pCHAR, pCHAR);
void  get_beyond_start_date(char *, int);
void  EndProcess();

int  reponseWaitFlag;
void automatic_preauth_reversal_checks();
int  db_select_non_processed_preauths_count(char*, char*);
void get_max_retry_count();
void PinnacleMsgHandler(pPTE_MSG);
void shut_down(void);
BYTE Process_Preauth_Reversal_Response (pPTE_MSG);
BYTE Send_Request_To_Service (BYTE*, BYTE, BYTE, BYTE, BYTE, pBYTE, INT);
BYTE Check_Is_Host_Up (BYTE*);
void display_error_message(pBYTE, pBYTE);
BYTE update_atp_batch_tables();
int  get_network_name();

BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH];
BYTE db_select_non_processed_preauth(char*, char*, pBCH20);
BYTE db_select_bch20_for_preauth_total_amount(pBCH20, pCHAR);
int  Convert_Amt_To_Auth_Tx_String_Format(double, char*);
void get_time( pCHAR, pCHAR );
void LeftFill_String_With_Character (unsigned int, char*, unsigned char );
BYTE db_select_padraw( pCHAR, pPAD_RAW, pCHAR );

