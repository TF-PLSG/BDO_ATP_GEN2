#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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

#include "dbcommon.h"
#include "tx_database.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"

#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20
#define   MAX_MERCHANTS		 100
#define   VALIDATION_ERROR	 -2
#define   ERROR				 10
#define   UPDATED            11
#define   DELETED			 12
#define   VISA				 "V"
#define   MC				 "M"

/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

extern CHAR ServiceName[];
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
UINT  Max_File_Size_Defined = 0 ;

typedef struct
{
	BYTE 	scheme			[2];
	BYTE	catgeory		[2];
	BYTE 	enable			[2];
	BYTE	description 	[26];
	BYTE 	alw_reattempts	[3];
	BYTE	reset_days		[3];
	BYTE	response_codes 	[101];
	BYTE	refresh_date	[9];

}Scheme_Integrity_Category, *pScheme_Integrity_Category;

//common functions declarations
INT   get_dataserver_ini_db_error_statics_value(pINT, double *, pCHAR, pCHAR  );
void  Create_Db_Statics_And_Db_Oracle_Error_Filename();
void  Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR);
void  mparam_log_message(INT, INT, pCHAR, pCHAR);
INT   mparam_Log_error_warning_to_File(pCHAR, int, pCHAR);
char* mask_card_number(char *);
void  get_date( pCHAR, pCHAR);
void  get_end_date(char *, int);
void  get_beyond_start_date(char *, int);
void  updates_count(char*, char*, int);
INT   merchant_status_update();
char* get_complete_reattempts_date(char *, int);
void  EndProcess();

//visa functions declarations
void  visa_integrity_checks();
INT   read_system_integrity_parameters_for_VISA( void );
void  read_scheme_integrity_category_visa();
void  merchant_integrity_param_checks_visa();
INT   merchant_integrity_param_reattempt_checks_visa();
INT   verify_and_update_reattempt_date_visa(MERCH_INT_PARAMS);
void  updates_count_visa(pMERCH_INT_PARAMS, int);

//mc functions declarations
void  mc_integrity_checks();
void  read_scheme_integrity_category_mc();
void  merchant_integrity_param_checks_mc();
INT   merchant_integrity_param_reattempt_checks_mc();
INT   verify_and_update_reattempt_date_mc(MERCH_INT_PARAMS);
void  updates_count_mc(pMERCH_INT_PARAMS, int);

//mc mac functions declarations
void mc_mac_checks();
