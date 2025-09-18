
/******************************************************************************

   Module: updatds.c

   Title:  Update Dataserver

   Description: Dataserver Routines to update database for each transaction

   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

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
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "update_db.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"
#include "diners.h"

#define VISA7 1
#define MC12_OCT2023 1
#define MC13_OCT2023 1

/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
CHAR         AppName   [MAX_APP_NAME_SIZE];

/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR DB_module_file_name[256];
CHAR DB_file_path[256];
CHAR Last_Tran_Logging_Filename[256];
/* End   */

UINT Max_File_Size_Defined = 0 ;

/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;

AUTH_TX auth_tx;

#ifdef VISA7
CHAR Visa_cat1_rc[51][3]={0};
CHAR Visa_cat2_rc[51][3]={0};
CHAR Visa_cat3_rc[51][3]={0};

INT total_cat1_visa_rc=0;
INT total_cat2_visa_rc=0;
INT total_cat3_visa_rc=0;
/* Merchant integrity parameters */
CHAR last_data_update_date[9]={0};
BYTE allowable_reattempts_234[3]={0};
BYTE reset_days_1[2]={0};
BYTE reset_days_234[3]={0};
BYTE cat1_MID_integrity_validation_flag=0;
BYTE cat2_MID_integrity_validation_flag=0;
BYTE cat3_MID_integrity_validation_flag=0;
BYTE cat4_MID_integrity_validation_flag=0;
BYTE process_merchant_integrity_transactions (pTLF01 tlf01);
#endif

#define  MERCHANT_INTEGRITY_INI_FILE   "merchant_integrity.ini"

BYTE process_merchant_integrity_transactions (pTLF01 tlf01);

#ifdef MC12_OCT2023
/*
 * MC Transaction program excellence, excessive declines
 * */
BYTE excessive_declines_flag=0;
BYTE mc_last_data_update_date[9]={0};
BYTE allowable_declines_cat1[3]={0};
BYTE allowable_declines_cat2[3]={0};
BYTE mc_reset_days_1[2]={0};
BYTE mc_reset_days_2[3]={0};
BYTE cat1_excessive_declines_validation_flag=0;
BYTE cat2_excessive_declines_validation_flag=0;

/*Not used as of now*/
CHAR mc_cat1_rc[51][3]={0};
CHAR mc_cat2_rc[51][3]={0};
#endif

#ifdef MC13_OCT2023
/*
 * MAC Transaction Processing Excellence Program
 * */
BYTE mc_mac_code[3]					  = {0};
INT  total_mac_codes     			  = 0;
BYTE mac_cat1_validation_flag		  = 0;
BYTE mac_cat2_validation_flag		  = 0;
BYTE mac_codes[15][3]				  = {0};
BYTE mac_last_data_update_date[9]	  = {0};
BYTE mac_allowable_reattempts_cat1[3] = {0};
BYTE mac_allowable_reattempts_cat2[3] = {0};
BYTE mac_reset_days_cat1[3]			  = {0};
BYTE mac_reset_days_cat2[3]			  = {0};
BYTE mac_pos_entry_mode_cat1[5] 	  = {0};
BYTE mac_pos_cond_code_cat1[3] 		  = {0};
BYTE mac_pos_entry_mode_cat2[5] 	  = {0};
BYTE mac_pos_cond_code_cat2[3] 		  = {0};
#endif

void TrapFunction( INT signum );
void create_system_time( pCHAR system_time );
void commit_the_update();

/*Check null*/
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"

#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

CHAR smcc_id1[5]={0};
CHAR smcc_id2[5]={0};
CHAR smcc_id3[5]={0};
CHAR smcc_id4[5]={0};
CHAR smcc_id5[5]={0};
CHAR smcc_id6[5]={0};
CHAR smcc_id7[5]={0};
CHAR smcc_id8[5]={0};
CHAR smcc_id9[5]={0};
CHAR smcc_id10[5]={0};

INT DB_NullCheck_Flag;

Offus_velocity  offus_velocity;
CCF03   ccf03p;

PRIVATE union
{
	DCF01   dcf01;
	BCH01   bch01;
	BCH01_MP   bch01_mp;
	BCH10   bch10;
	MCF01   mcf01;
	MCF20   mcf20;
	TLF01   tlf01;
	BCH11   bch11;
	BCH20   bch20;
    NCF21   ncf21;
    NCF30   ncf30;
    RPS01   rps01;
    MCF03P  mcf03p;
    MERCHANT_INTEGRITY_PARAM merchant_intgerity_data;
    PTSN01 ptsn01;
}db_recs;

PRIVATE BYTE         msg_sub_type2;
PRIVATE BYTE         msg_sub_type1;
PRIVATE BYTE         app_data_type;
PRIVATE BYTE         result_code;
PRIVATE BOOLEAN      updates_to_db_made = false;
PRIVATE CHAR         err_msg[1024];
        CHAR         app_name[MAX_APP_NAME_SIZE];
PRIVATE pCHAR        reply_que;
PRIVATE CHAR         orig_comm_inf[MAX_ORIG_INFO_SIZE];
PRIVATE CHAR         atp_buffer[30000];
PRIVATE CHAR         update_all_status[48]; /* Status of alldata func in tlf01*/
CHAR                 Version[] = "ATP_11.14.0";
BYTE                 OrigTxKey;

/*Merchant offset in croosponding tables*/
#define DCF01_MERCHANT_ID_OFFSET      96
#define BCH01_MERCHANT_ID_OFFSET      16
#define BCH01_MP_MERCHANT_ID_OFFSET   16
#define BCH10_MERCHANT_ID_OFFSET      29
#define BCH11_MERCHANT_ID_OFFSET	  29
#define BCH20_MERCHANT_ID_OFFSET      29

/**********Primary Key Size*********************/
#define BCH01_PRIMARY_KEY_SIZE 32 // Device id + Batch_number + Merchant ID.
#define BCH01_MP_PRIMARY_KEY_SIZE 32 // Device id + Batch_number + Merchant ID.

/* Reading data from tf.ini*/
#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20
/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

/* Globals for Cash Bonus Program */
INT     CashBonusWinner;
CHAR    CashBonusNonWinnerMsg[121];

CHAR masked_card_num[20] = {0};
INT ds_timer_flag;
INT ds_timer_value;

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
double        NextReportTime;
CHAR          ReportTime[5];
CHAR  CUP_merchant_indicator[8]={0};

CHAR	pos_res_txt[40] = {0};

#ifdef _DEBUG
   #define PRINT(X) printf(X)
#else
   #define PRINT(X)
#endif

// fuctions

#ifdef MC12_OCT2023
INT updatds_read_execssive_delcines_parameter_for_mc()
{
    DWORD rc;
    CHAR filename     [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
    CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
    CHAR rc_data[256] ={0};
    INT  ret_val = true;
    INT index=0,rc_index=0;

    excessive_declines_flag='\0'; cat1_excessive_declines_validation_flag='\0'; cat2_excessive_declines_validation_flag='\0';

    memset(mc_last_data_update_date,0x00,sizeof(mc_last_data_update_date));
    memset(allowable_declines_cat1,0x00,sizeof(allowable_declines_cat1));
    memset(allowable_declines_cat2,0x00,sizeof(allowable_declines_cat2));
    memset(mc_reset_days_1,0x00,sizeof(mc_reset_days_1));
    memset(mc_reset_days_2,0x00,sizeof(mc_reset_days_2));
    memset(mc_cat1_rc,0x00,sizeof(mc_cat1_rc));
    memset(mc_cat2_rc,0x00,sizeof(mc_cat2_rc));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "EXCESSIVE_DECLINES_FLAG",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"EXCESSIVE_DECLINES_FLAG not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	excessive_declines_flag = ini_info[0];

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "LAST_DATE",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"LAST_DATE not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	strncpy(mc_last_data_update_date ,ini_info,sizeof(mc_last_data_update_date)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "ALLOWABLE_DECLINES_CAT_1",
                "10",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"ALLOWABLE_DECLINES_CAT_1 not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	strncpy(allowable_declines_cat1 ,ini_info,sizeof(allowable_declines_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "ALLOWABLE_DECLINES_CAT_2",
                "35",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"ALLOWABLE_DECLINES_CAT_2 not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	strncpy(allowable_declines_cat2 ,ini_info,sizeof(allowable_declines_cat2)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "RESET_DAYS_CAT_1",
                "1",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"RESET_DAYS_CAT_1 not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	mc_reset_days_1[0]=ini_info[0];

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "RESET_DAYS_CAT_2",
                "30",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"RESET_DAYS_CAT_2 not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	strncpy(mc_reset_days_2 ,ini_info,sizeof(mc_reset_days_2)-1);

    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "CATEGORY1",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"CATEGORY1 validation flag not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	cat1_excessive_declines_validation_flag=ini_info[0];

    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "CATEGORY2",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   updateds_log_message(2,2,"CATEGORY1 validation flag not read","updatds_read_execssive_delcines_parameter_for_mc");
	}

	cat2_excessive_declines_validation_flag=ini_info[0];

	return ret_val;
}

INT updatds_read_last_updated_date_parameter_for_MC()
{
    DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	CHAR rc_data[256] ={0};
	INT  ret_val = true;

	memset(mc_last_data_update_date,0x00,sizeof(mc_last_data_update_date));
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
	             "MERCHANT_INTEGRITY_MC",     /* points to section name       */
	             "LAST_DATE",  							/* points to key name           */
	             "",                       		/* points to default string     */
	             ini_info,                      /* points to destination buffer */
	             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	             filename                       /* points to ini filename       */
	);
	if(rc != 1)
	{
		updateds_log_message(2,2,"mc_last_data_update_date not read","updatds_read_last_updated_date_parameter_for_MC");
	}

	strncpy( mc_last_data_update_date, ini_info,strlen(ini_info));

	return ret_val;
}

void process_delcined_transactions_from_mc_cat1(pAUTH_TX auth_tx)
{
	INT decline_count_1day=0;
	INT  i_allowable_reattempts_cat1=0;
	BYTE start_date[9]={0}, current_system_date[9]={0};
	BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;
	CHAR local_reattempts_date[7]={0};
	INT  reattempts_date_len = 0;

	create_system_date(current_system_date );
	strncpy(start_date,current_system_date,8);
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);

	if( db_recs.merchant_intgerity_data.c1_block_start_date[0] == '\0' &&
		db_recs.merchant_intgerity_data.c1_count[0] == '\0')
	{
		strncpy(db_recs.merchant_intgerity_data.c1_block_start_date,current_system_date,8);
		decline_count_1day = 1;
		sprintf(db_recs.merchant_intgerity_data.c1_count,"%02d",decline_count_1day);
	}
	else
	{
		decline_count_1day=atoi(db_recs.merchant_intgerity_data.c1_count);
		sprintf(db_recs.merchant_intgerity_data.c1_count,"%02d",(decline_count_1day+1));
	}

	db_recs.merchant_intgerity_data.c1_block[0]='N';

	sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
	reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
	strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));

	i_allowable_reattempts_cat1 = atoi(allowable_declines_cat1);

	if (atoi(db_recs.merchant_intgerity_data.c1_count) >= i_allowable_reattempts_cat1)
	{
			db_recs.merchant_intgerity_data.c1_block[0] = 'Y';
			julian_end_date = atoi(mc_reset_days_1)+julian_start_date;
			Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

			strncpy(db_recs.merchant_intgerity_data.c1_block_start_date,start_date,8);
			strncpy(db_recs.merchant_intgerity_data.c1_block_end_date,end_date,8);

			if(0 == strncmp(auth_tx->TLF01_details.response_code,"05",2) &&
					0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity,"10",2))
			{ /* for Partially approved transaction MC will send 10, ATP is chnaging it to 05,
				added this condition to store the actual RC sent by MC
			  */
				strncpy(db_recs.merchant_intgerity_data.last_declined_rc,auth_tx->TLF01_details.product_codes[13].quantity,2);
			}
			else
			{
				strncpy(db_recs.merchant_intgerity_data.last_declined_rc,auth_tx->TLF01_details.response_code,2);
			}
	}
	return;
}

void process_delcined_transactions_from_mc_cat2(pAUTH_TX auth_tx)
{
	INT decline_count_day=0;
	INT  i_allowable_reattempts_cat2=0;
	BYTE start_date[9]={0}, current_system_date[9]={0};
	BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;
	CHAR local_reattempts_date[7]={0};
	INT  reattempts_date_len = 0;

	create_system_date(current_system_date );
	strncpy(start_date,current_system_date,8);
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);


	decline_count_day=atoi(db_recs.merchant_intgerity_data.c2_count);
	sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",(decline_count_day+1));

	db_recs.merchant_intgerity_data.c2_block[0]='N';

	sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
	reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
	strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));

	i_allowable_reattempts_cat2 = atoi(allowable_declines_cat2);

	if (atoi(db_recs.merchant_intgerity_data.c2_count) >= i_allowable_reattempts_cat2)
	{
			db_recs.merchant_intgerity_data.c2_block[0] = 'Y';
			julian_end_date = atoi(mc_reset_days_2)+julian_start_date;
			Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

			strncpy(db_recs.merchant_intgerity_data.c2_block_start_date,start_date,8);
			strncpy(db_recs.merchant_intgerity_data.c2_block_end_date,end_date,8);

			if(0 == strncmp(auth_tx->TLF01_details.response_code,"05",2) &&
					0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity,"10",2))
			{ /* for Partially approved transaction MC will send 10, ATP is chnaging it to 05,
				added this condition to store the actual RC sent by MC
			  */
				strncpy(db_recs.merchant_intgerity_data.last_declined_rc,auth_tx->TLF01_details.product_codes[13].quantity,2);
			}
			else
			{
				strncpy(db_recs.merchant_intgerity_data.last_declined_rc,auth_tx->TLF01_details.response_code,2);
			}
	}


	return;
}

void process_delcined_transactions_from_mc_cat1_cat2(pAUTH_TX auth_tx)
{
	INT decline_count_1day=0, merge_decline_count=0;
	INT  i_allowable_reattempts_cat1=0, i_allowable_reattempts_cat2=0;
	BYTE start_date[9]={0}, current_system_date[9]={0};
	BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;
	CHAR local_reattempts_date[7]={0};
	INT  reattempts_date_len = 0;

	create_system_date(current_system_date );
	strncpy(start_date,current_system_date,8);
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);
	if( db_recs.merchant_intgerity_data.c1_block_start_date[0] == '\0' &&
	    db_recs.merchant_intgerity_data.c1_count[0] == '\0')
	{
		strncpy(db_recs.merchant_intgerity_data.c1_block_start_date,current_system_date,8);
		decline_count_1day = 1;
		sprintf(db_recs.merchant_intgerity_data.c1_count,"%02d",decline_count_1day);

	}
	else
	{
		decline_count_1day=atoi(db_recs.merchant_intgerity_data.c1_count);
		sprintf(db_recs.merchant_intgerity_data.c1_count,"%02d",(decline_count_1day+1));

	}
	merge_decline_count=atoi(db_recs.merchant_intgerity_data.c2_count);
	sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",(merge_decline_count+1));

	db_recs.merchant_intgerity_data.c1_block[0]='N';
	db_recs.merchant_intgerity_data.c2_block[0]='N';

	sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
	reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
	strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));

	i_allowable_reattempts_cat1 = atoi(allowable_declines_cat1);
	i_allowable_reattempts_cat2 = atoi(allowable_declines_cat2);


	if( atoi(db_recs.merchant_intgerity_data.c1_count) >= i_allowable_reattempts_cat1 &&
		atoi(db_recs.merchant_intgerity_data.c2_count) < i_allowable_reattempts_cat2 )
	{
		db_recs.merchant_intgerity_data.c1_block[0] = 'Y';
		julian_end_date = atoi(mc_reset_days_1)+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(db_recs.merchant_intgerity_data.c1_block_start_date,start_date,8);
		strncpy(db_recs.merchant_intgerity_data.c1_block_end_date,end_date,8);

	}
	else if (atoi(db_recs.merchant_intgerity_data.c2_count) >= i_allowable_reattempts_cat2)
	{
		db_recs.merchant_intgerity_data.c2_block[0] = 'Y';
		julian_end_date = atoi(mc_reset_days_2)+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(db_recs.merchant_intgerity_data.c2_block_start_date,start_date,8);
		strncpy(db_recs.merchant_intgerity_data.c2_block_end_date,end_date,8);

	}

	if('Y' == db_recs.merchant_intgerity_data.c1_block[0] ||
	   'Y' == db_recs.merchant_intgerity_data.c2_block[0]	)
	{
		if(0 == strncmp(auth_tx->TLF01_details.response_code,"05",2) &&
				0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity,"10",2))
		{ /* for Partially approved transaction MC will send 10, ATP is chnaging it to 05,
			added this condition to store the actual RC sent by MC
		  */
			strncpy(db_recs.merchant_intgerity_data.last_declined_rc,auth_tx->TLF01_details.product_codes[13].quantity,2);
		}
		else
		{
			strncpy(db_recs.merchant_intgerity_data.last_declined_rc,auth_tx->TLF01_details.response_code,2);
		}
	}

	return;
}

void prepare_MIP_data_for_insert_MC(pAUTH_TX auth_tx)
{
	int count,i_reset_days=0,i_allowable_reattempts=0;
	CHAR error_buf[256]={0};
	BYTE rc=0;
	BYTE start_date[9]={0};

	memset(db_recs.merchant_intgerity_data,0x00,sizeof(db_recs.merchant_intgerity_data));

	memcpy(db_recs.merchant_intgerity_data.merchant_id,auth_tx->TLF01_details.merchant_id,15);
	memcpy(db_recs.merchant_intgerity_data.card_nbr,auth_tx->TLF01_details.card_num,strlen(auth_tx->TLF01_details.card_num));
	memcpy(db_recs.merchant_intgerity_data.category_cd,ALL_CATEGORY_CODE,4);

	count=1;

	/*calculate the start and end date*/
	create_system_date(start_date );
	if( cat1_excessive_declines_validation_flag == 'Y' &&
		cat2_excessive_declines_validation_flag == 'Y')
	{
		db_recs.merchant_intgerity_data.c1_block[0]='N';
		db_recs.merchant_intgerity_data.c2_block[0]='N';

		/*daily count*/
		sprintf(db_recs.merchant_intgerity_data.c1_count,"%02d",count);
		/*merge count*/
		sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",count);
		strncpy(db_recs.merchant_intgerity_data.c1_block_start_date,start_date,8);

	}
	else if(cat1_excessive_declines_validation_flag == 'Y' && cat2_excessive_declines_validation_flag != 'Y')
	{
		db_recs.merchant_intgerity_data.c1_block[0]='N';

		/*daily count*/
		sprintf(db_recs.merchant_intgerity_data.c1_count,"%02d",count);
		strncpy(db_recs.merchant_intgerity_data.c1_block_start_date,start_date,8);
	}
	else
	{
		db_recs.merchant_intgerity_data.c2_block[0]='N';
		sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",count);
	}


	strncpy(db_recs.merchant_intgerity_data.first_reattempt_date,start_date,8);
	sprintf(db_recs.merchant_intgerity_data.reattempts_date,"%4s;",start_date+4);

	return;
}

BYTE process_merchant_integrity_transactions_for_mc(pAUTH_TX auth_tx)
{
	CHAR error_buf[256]={0};
    CHAR merchant_integrity_key[41]={0};
	BYTE rc=PTEMSG_OK;
	INT  rc_category=0;
	INT  rc_count=0;
	INT decline_count_1day=0, merge_decline_count=0;
	INT  i_allowable_reattempts_cat1=0, i_allowable_reattempts_cat2=0;
	BYTE start_date[9]={0};
	BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;
	CHAR resp_code[3]={0};
	CHAR current_system_date[9]={0};
	CHAR local_reattempts_date[7], temp_date_yyyymmdd[9]={0};
	INT  reattempts_date_len = 0;
	char month_from_reattempt_date[3]={0}, currentMonth[3]={0}, currentYear[5]={0};
    MCF01 MCF01_details;

	/* get the latest config details if the details are updated in the merchant_integrity.ini file*/
	create_system_date(current_system_date );
	if(0 != memcmp(current_system_date,mc_last_data_update_date,sizeof(current_system_date)-1))
	{
		/* get only last updated date ,
		 * the last update date should be equal to current system date, */
		updatds_read_last_updated_date_parameter_for_MC();
		if(0 == memcmp(current_system_date,mc_last_data_update_date,sizeof(current_system_date)-1))
		{
			updatds_read_execssive_delcines_parameter_for_mc();
			/*updated details from tf.ini should be logged here
			 * */
			sprintf(err_msg,"Scheme integrity parameter updated main flag(%c) cat1 flag(%c) and cat2 flag(%c),"
					"mc_last_data_update_date(%s), allowable_declines_cat1(%s), allowable_declines_cat2(%s),mc_reset_days_1(%s), mc_reset_days_2(%s)"
					"card_num:(%s),stan:(%s),mid:(%s)",
					excessive_declines_flag,cat1_excessive_declines_validation_flag,cat2_excessive_declines_validation_flag,mc_last_data_update_date,
					allowable_declines_cat1,allowable_declines_cat2,mc_reset_days_1,mc_reset_days_2,
					auth_tx->TLF01_details.card_num,auth_tx->TLF01_details.sys_trace_audit_num,auth_tx->TLF01_details.merchant_id );
			updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions_for_mc");
		}
	}

	memset(&db_recs.merchant_intgerity_data, 0x00, sizeof(db_recs.merchant_intgerity_data));
	memcpy(merchant_integrity_key,auth_tx->TLF01_details.merchant_id, 15);
	memcpy(merchant_integrity_key + sizeof(auth_tx->TLF01_details.merchant_id), auth_tx->TLF01_details.card_num, sizeof(auth_tx->TLF01_details.card_num));
	memcpy(merchant_integrity_key + sizeof(auth_tx->TLF01_details.card_num)+sizeof(auth_tx->TLF01_details.merchant_id), ALL_CATEGORY_CODE, 4);

	/*
	 * 0100 MT with pC:28
	 * check for the approved response code types
	 * â€œ00â€�, â€œ08â€�, â€œ10â€�, â€œ85â€� and â€œ87â€� these are considered approved RC
	 * */
	if( 0 == strncmp(auth_tx->TLF01_details.response_code,APPROVED,2) ||
	    0 == strncmp(auth_tx->TLF01_details.response_code,HONOR_WITH_ID_RC,2) ||
	   (0 == strncmp(auth_tx->TLF01_details.response_code,"05",2) &&
		0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity  ,PARTIAL_APPROVAL_RC,2))||
	    0 == strncmp(auth_tx->TLF01_details.response_code,ZERO_AMOUNT_RC,2) ||
	    (0 == strncmp(auth_tx->TLF01_details.response_code,"96",2) &&
	    0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity  ,PURCHASE_AMOUNT_RC,2)))
	{
		db_delete_merchant_integrity_param_for_mc(&merchant_integrity_key,  error_buf);
	}
	else
	{
		if(excessive_declines_flag == 'Y')
		{
			if( cat1_excessive_declines_validation_flag == 'Y' &&
					cat2_excessive_declines_validation_flag == 'Y')
			{
				sprintf(err_msg,"Both category flags are enabled cat1 flag(%c) and cat2 flag(%c),card_num:%s,stan:%s,mid:%s",
				cat1_excessive_declines_validation_flag,cat2_excessive_declines_validation_flag,auth_tx->TLF01_details.card_num,auth_tx->TLF01_details.sys_trace_audit_num,auth_tx->TLF01_details.merchant_id );
				updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions_for_mc");

				rc = db_select_merchant_integrity_param(merchant_integrity_key,&db_recs.merchant_intgerity_data,error_buf);

				if(rc ==PTEMSG_OK)
				{
					process_delcined_transactions_from_mc_cat1_cat2(auth_tx);

					/*
					 * first 4 bytes of reattempt date and last 4 byte of first reattempt date should be same,
					 * else take first 4 bytes of reattempt date append year and copy into first reattempt date
					 * */
					if(0 != strncmp(db_recs.merchant_intgerity_data.reattempts_date,db_recs.merchant_intgerity_data.first_reattempt_date+4,4))
					{
						/* add*/
						sprintf(error_buf, "first 4 byte of reattempt date(%.4s) and first_reattempt_date(%s) are not same ",db_recs.merchant_intgerity_data.reattempts_date,
								db_recs.merchant_intgerity_data.first_reattempt_date);
						updateds_log_message(2,2,error_buf,"prepare_merchant_intgerity_param_for_insert");

						memset(db_recs.merchant_intgerity_data.first_reattempt_date,0x00,sizeof(db_recs.merchant_intgerity_data.first_reattempt_date));

						 memcpy(currentYear,  current_system_date,   4);
						 memcpy(currentMonth, current_system_date+4, 2);
						 strncpy(month_from_reattempt_date,db_recs.merchant_intgerity_data.reattempts_date,2);
						if(atoi(month_from_reattempt_date) <= atoi(currentMonth))
						{
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, currentYear, strlen(currentYear));
							strncat(db_recs.merchant_intgerity_data.first_reattempt_date, db_recs.merchant_intgerity_data.reattempts_date, 4);
						}
						else
						{
							sprintf(temp_date_yyyymmdd, "%d", atoi(currentYear)-1);
							strncat(temp_date_yyyymmdd, db_recs.merchant_intgerity_data.reattempts_date, 4);
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, temp_date_yyyymmdd, 8);
						}
					}

					db_update_mip_for_mc(&db_recs.merchant_intgerity_data,  error_buf);
				}
				else
				{
					/*First decline insert the details
					 * */
					prepare_MIP_data_for_insert_MC(auth_tx);
					rc = db_insert_merchant_integrity_param(&db_recs.merchant_intgerity_data,error_buf,0);
				}
			}
			else if(cat1_excessive_declines_validation_flag == 'Y' && cat2_excessive_declines_validation_flag != 'Y')
			{
				sprintf(err_msg,"Only cat1 flag(%c) enabled,card_num:%s,stan:%s,mid:%s",
				cat1_excessive_declines_validation_flag,auth_tx->TLF01_details.card_num,auth_tx->TLF01_details.sys_trace_audit_num,auth_tx->TLF01_details.merchant_id );
				updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions_for_mc");

				rc = db_select_merchant_integrity_param(merchant_integrity_key,&db_recs.merchant_intgerity_data,error_buf);

				if(rc ==PTEMSG_OK)
				{
					process_delcined_transactions_from_mc_cat1(auth_tx);

					/*
					 * first 4 bytes of reattempt date and last 4 byte of first reattempt date should be same,
					 * else take first 4 bytes of reattempt date append year and copy into first reattempt date
					 * */
					if(0 != strncmp(db_recs.merchant_intgerity_data.reattempts_date,db_recs.merchant_intgerity_data.first_reattempt_date+4,4))
					{
						/* add*/
						sprintf(error_buf, "first 4 byte of reattempt date(%.4s) and first_reattempt_date(%s) are not same ",db_recs.merchant_intgerity_data.reattempts_date,
								db_recs.merchant_intgerity_data.first_reattempt_date);
						updateds_log_message(2,2,error_buf,"prepare_merchant_intgerity_param_for_insert");

						memset(db_recs.merchant_intgerity_data.first_reattempt_date,0x00,sizeof(db_recs.merchant_intgerity_data.first_reattempt_date));

						 memcpy(currentYear,  current_system_date,   4);
						 memcpy(currentMonth, current_system_date+4, 2);
						 strncpy(month_from_reattempt_date,db_recs.merchant_intgerity_data.reattempts_date,2);
						if(atoi(month_from_reattempt_date) <= atoi(currentMonth))
						{
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, currentYear, strlen(currentYear));
							strncat(db_recs.merchant_intgerity_data.first_reattempt_date, db_recs.merchant_intgerity_data.reattempts_date, 4);
						}
						else
						{
							sprintf(temp_date_yyyymmdd, "%d", atoi(currentYear)-1);
							strncat(temp_date_yyyymmdd, db_recs.merchant_intgerity_data.reattempts_date, 4);
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, temp_date_yyyymmdd, 8);
						}
					}
					db_update_mip_for_mc(&db_recs.merchant_intgerity_data,  error_buf);
				}
				else
				{
					/*First decline insert the details
					 * */
					prepare_MIP_data_for_insert_MC(auth_tx);
					rc = db_insert_merchant_integrity_param(&db_recs.merchant_intgerity_data,error_buf,0);
				}
			}
			else if(cat1_excessive_declines_validation_flag != 'Y' && cat2_excessive_declines_validation_flag == 'Y')
			{
				sprintf(err_msg,"Only cat2 flag(%c) enabled,card_num:%s,stan:%s,mid:%s",
				cat2_excessive_declines_validation_flag,auth_tx->TLF01_details.card_num,auth_tx->TLF01_details.sys_trace_audit_num,auth_tx->TLF01_details.merchant_id );
				updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions_for_mc");

				rc = db_select_merchant_integrity_param(merchant_integrity_key,&db_recs.merchant_intgerity_data,error_buf);

				if(rc ==PTEMSG_OK)
				{
					process_delcined_transactions_from_mc_cat2(auth_tx);
					/*
					 * first 4 bytes of reattempt date and last 4 byte of first reattempt date should be same,
					 * else take first 4 bytes of reattempt date append year and copy into first reattempt date
					 * */
					if(0 != strncmp(db_recs.merchant_intgerity_data.reattempts_date,db_recs.merchant_intgerity_data.first_reattempt_date+4,4))
					{
						/* add*/
						sprintf(error_buf, "first 4 byte of reattempt date(%.4s) and first_reattempt_date(%s) are not same ",db_recs.merchant_intgerity_data.reattempts_date,
								db_recs.merchant_intgerity_data.first_reattempt_date);
						updateds_log_message(2,2,error_buf,"prepare_merchant_intgerity_param_for_insert");

						memset(db_recs.merchant_intgerity_data.first_reattempt_date,0x00,sizeof(db_recs.merchant_intgerity_data.first_reattempt_date));

						 memcpy(currentYear,  current_system_date,   4);
						 memcpy(currentMonth, current_system_date+4, 2);
						 strncpy(month_from_reattempt_date,db_recs.merchant_intgerity_data.reattempts_date,2);
						if(atoi(month_from_reattempt_date) <= atoi(currentMonth))
						{
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, currentYear, strlen(currentYear));
							strncat(db_recs.merchant_intgerity_data.first_reattempt_date, db_recs.merchant_intgerity_data.reattempts_date, 4);
						}
						else
						{
							sprintf(temp_date_yyyymmdd, "%d", atoi(currentYear)-1);
							strncat(temp_date_yyyymmdd, db_recs.merchant_intgerity_data.reattempts_date, 4);
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, temp_date_yyyymmdd, 8);
						}
					}
					db_update_mip_for_mc(&db_recs.merchant_intgerity_data,  error_buf);
				}
				else
				{
					/*First decline insert the details
					 * */
					prepare_MIP_data_for_insert_MC(auth_tx);
					rc = db_insert_merchant_integrity_param(&db_recs.merchant_intgerity_data,error_buf,0);
				}
			}
			else
			{
				sprintf(err_msg,"MC excessive main flag enabled(%c), category flags are disabled cat1 flag(%c) and cat2 flag(%c),card_num:%s,stan:%s,mid:%s",excessive_declines_flag,
						cat1_excessive_declines_validation_flag,cat2_excessive_declines_validation_flag,auth_tx->TLF01_details.card_num,auth_tx->TLF01_details.sys_trace_audit_num,auth_tx->TLF01_details.merchant_id );
				updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions_for_mc");
			}
			if( db_recs.merchant_intgerity_data.c1_block[0] == 'Y' ||
			    db_recs.merchant_intgerity_data.c2_block[0] == 'Y' )
			{
				memset(&MCF01_details,0x00,sizeof(MCF01_details));
				strncpy(MCF01_details.primary_key.merchant_id,auth_tx->TLF01_details.merchant_id,sizeof(MCF01_details.primary_key.merchant_id));
				strncpy(MCF01_details.primary_key.organization_id,"0000",sizeof(MCF01_details.primary_key.organization_id));
				rc = db_update_mcf01(&MCF01_details,error_buf);
			}
		}
		else
		{
			/*
			 *  Excessive decline functionality disabled continue with transaction processing.
			 * */
			/*sprintf(err_msg,"MC excessive decline validation disabled,main flag(%c), cat1 flag(%c) and cat2 flag(%c),card_num:%s,stan:%s,mid:%s",excessive_declines_flag,
					cat1_excessive_declines_validation_flag,cat2_excessive_declines_validation_flag,auth_tx->TLF01_details.card_num,auth_tx->TLF01_details.sys_trace_audit_num,auth_tx->TLF01_details.merchant_id );
			updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions_for_mc");*/
		}

	}
	return PTEMSG_OK;
}
#endif


#ifdef MC13_OCT2023
BYTE process_merchant_integrity_transactions_for_mac(pAUTH_TX auth_tx)
{
	CHAR error_buf[256] = {0};
    CHAR merchant_integrity_key[41] = {0};
	BYTE rc = PTEMSG_OK;
	CHAR current_system_date[9]={0};
	int c1Count = 0, c2Count = 0;
	int mac_catgeory = MAC_NOT_DEFINED;
    MCF01 MCF01_details;

	/* get the latest config details if the details are updated in the merchant_integrity.ini file*/
	create_system_date(current_system_date );
	if(0 != memcmp(current_system_date, mac_last_data_update_date, sizeof(current_system_date)-1))
	{
		/* get only last updated date ,
		 * the last update date should be equal to current system date, */
		updatds_read_last_updated_date_parameter_for_MAC();
		if(0 == memcmp(current_system_date, mac_last_data_update_date, sizeof(current_system_date)-1))
		{
			updatds_read_merchant_advice_code_parameters();
			/*updated details from tf.ini should be logged here
			 * */
			sprintf(err_msg,"MERCHANT_ADVICE_CODE updated cat1 flag(%c) and cat2 flag(%c),"
					"mc_last_data_update_date(%s), allowable_reattempts_cat1(%s), "
					"allowable_reattempts_cat2(%s),mac_reset_days_1(%s), mac_reset_days_2(%s)"
					"card_num:(%s),stan:(%s),mid:(%s)",
					mac_cat1_validation_flag, mac_cat2_validation_flag,
					mac_last_data_update_date,
					mac_allowable_reattempts_cat1, mac_allowable_reattempts_cat2,
					mac_reset_days_cat1, mac_reset_days_cat2,
					auth_tx->TLF01_details.card_num, auth_tx->TLF01_details.sys_trace_audit_num,
					auth_tx->TLF01_details.merchant_id );
			updateds_log_message(2, 2, err_msg, "process_merchant_integrity_transactions_for_mac");
		}
	}

	memset(&db_recs.merchant_intgerity_data, 0x00, sizeof(db_recs.merchant_intgerity_data));
	memcpy(merchant_integrity_key, auth_tx->TLF01_details.merchant_id, 15);
	memcpy(merchant_integrity_key + sizeof(auth_tx->TLF01_details.merchant_id),
			auth_tx->TLF01_details.card_num, sizeof(auth_tx->TLF01_details.card_num));
	memcpy(merchant_integrity_key + sizeof(auth_tx->TLF01_details.card_num) + sizeof(auth_tx->TLF01_details.merchant_id),
			MAC_CATEGORY_CODE, 4);

	/*
	 * check for the approved response code types
	 * 00, 08, 10, 85 and 87 these are considered approved RC
	 * */
	if(0 == strncmp(auth_tx->TLF01_details.response_code,				APPROVED,		  	 2) ||
	   0 == strncmp(auth_tx->TLF01_details.response_code,				HONOR_WITH_ID_RC,    2) ||
	   0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity,  PARTIAL_APPROVAL_RC, 2) ||
	   0 == strncmp(auth_tx->TLF01_details.response_code,				ZERO_AMOUNT_RC,		 2) ||
	   0 == strncmp(auth_tx->TLF01_details.response_code,				PURCHASE_AMOUNT_RC,	 2))
	{
		db_delete_merchant_integrity_param_for_mc(&merchant_integrity_key,  error_buf);
		return PTEMSG_OK;
	}

	mac_catgeory = updatds_check_mac_category(auth_tx->TLF01_details.pos_entry_mode,
											  auth_tx->TLF01_details.pos_condition_code,
											  auth_tx->TLF01_details.handler_queue);
	if( (mac_catgeory == MAC_CAT_1 && mac_cat1_validation_flag == 'Y') ||
		(mac_catgeory == MAC_CAT_2 && mac_cat2_validation_flag == 'Y') )
	{
		rc = db_select_merchant_integrity_param_mac(&auth_tx->TLF01_details, &db_recs.merchant_intgerity_data, error_buf);

		if(rc == PTEMSG_OK)
		{
			if(db_recs.merchant_intgerity_data.c1_block[0] != 'Y' || db_recs.merchant_intgerity_data.c2_block[0] != 'Y')
			{
				if(mac_catgeory == MAC_CAT_1)
				{
					rc = prepare_MIP_data_for_update_MAC(auth_tx, MAC_CAT_1);
				}
				else if(mac_catgeory == MAC_CAT_2)
				{
					rc = prepare_MIP_data_for_update_MAC(auth_tx, MAC_CAT_2);
				}

				if(rc == PTEMSG_OK)
					rc = db_update_mip_for_mc(&db_recs.merchant_intgerity_data, error_buf);
			}
			else
			{
				sprintf(error_buf, "MAC Record already blocked cannot insert/update record for Card:%s, Merchant:%s",
						auth_tx->TLF01_details.card_num, auth_tx->TLF01_details.merchant_id);
				updateds_log_message(2, 3, error_buf, "process_merchant_integrity_transactions_for_mac");

				return PTEMSG_INSERT_FAILED;
			}
		}
		else
		{
			if(mac_catgeory == MAC_CAT_1)
			{
				rc = prepare_MIP_data_for_insert_MAC(auth_tx, MAC_CAT_1);
			}
			else if(mac_catgeory == MAC_CAT_2)
			{
				rc = prepare_MIP_data_for_insert_MAC(auth_tx, MAC_CAT_2);
			}
			if(rc == PTEMSG_OK)
				rc = db_insert_merchant_integrity_param(&db_recs.merchant_intgerity_data, error_buf, 0);
		}

		if( db_recs.merchant_intgerity_data.c1_block[0] == 'Y' ||
		    db_recs.merchant_intgerity_data.c2_block[0] == 'Y' )
		{
			memset(&MCF01_details,0x00,sizeof(MCF01_details));
			strncpy(MCF01_details.primary_key.merchant_id, auth_tx->TLF01_details.merchant_id, sizeof(MCF01_details.primary_key.merchant_id));
			strncpy(MCF01_details.primary_key.organization_id, "0000", sizeof(MCF01_details.primary_key.organization_id));
			rc = db_update_mcf01(&MCF01_details, error_buf);
		}
	}
	else
	{
		sprintf(err_msg,"MAC validation disabled, card_num:%s,stan:%s,mid:%s",
				auth_tx->TLF01_details.card_num, auth_tx->TLF01_details.sys_trace_audit_num,
				auth_tx->TLF01_details.merchant_id );
		updateds_log_message(2, 2, err_msg, "process_merchant_integrity_transactions_for_mac");
	}

	return PTEMSG_OK;
}

int prepare_MIP_data_for_insert_MAC(pAUTH_TX auth_tx, int category)
{
	int count = 1, i_reset_days=0, i_allowable_reattempts=0;
	CHAR error_buf[256]={0};
	BYTE rc=0;
	BYTE start_date[9]={0}; BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;

	memset(&db_recs.merchant_intgerity_data,            0x00,								sizeof(db_recs.merchant_intgerity_data));
	memcpy(db_recs.merchant_intgerity_data.merchant_id, auth_tx->TLF01_details.merchant_id, 15);
	memcpy(db_recs.merchant_intgerity_data.card_nbr,    auth_tx->TLF01_details.card_num,    strlen(auth_tx->TLF01_details.card_num));
	memcpy(db_recs.merchant_intgerity_data.category_cd, MAC_CATEGORY_CODE, 4);

	/*calculate the start and end date*/
	create_system_date(start_date );
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);

	if( category == MAC_CAT_1)
	{
		if(atoi(mac_allowable_reattempts_cat1) > 1)
		{
			db_recs.merchant_intgerity_data.c1_block[0] = 'N';
			sprintf(db_recs.merchant_intgerity_data.c1_count, "%02d", count);
		}
		else
		{
			db_recs.merchant_intgerity_data.c1_block[0] = 'Y';
			sprintf(db_recs.merchant_intgerity_data.c1_count, "%02d", count);
			i_reset_days = atoi(mac_reset_days_cat1);
			julian_end_date = i_reset_days + julian_start_date;
			Txutils_Calculate_Gregorian_Date(julian_end_date, end_date);
			strncpy(db_recs.merchant_intgerity_data.c1_block_start_date, start_date, 8);
			strncpy(db_recs.merchant_intgerity_data.c1_block_end_date,   end_date,   8);
		}
	}
	else if( category == MAC_CAT_2)
	{
		if(atoi(mac_allowable_reattempts_cat2) > 1)
		{
			db_recs.merchant_intgerity_data.c2_block[0] = 'N';
			sprintf(db_recs.merchant_intgerity_data.c2_count, "%02d", count);
		}
		else
		{
			db_recs.merchant_intgerity_data.c2_block[0] = 'Y';
			sprintf(db_recs.merchant_intgerity_data.c2_count, "%02d", count);
			i_reset_days = atoi(mac_reset_days_cat2);
			julian_end_date = i_reset_days + julian_start_date;
			Txutils_Calculate_Gregorian_Date(julian_end_date, end_date);
			strncpy(db_recs.merchant_intgerity_data.c2_block_start_date, start_date, 8);
			strncpy(db_recs.merchant_intgerity_data.c2_block_end_date,   end_date,   8);
		}
	}

	/* for Partially approved transaction MC will send 10, ATP is changing it to 05,
			added this condition to store the actual RC sent by MC
	*/
	if(0 == strncmp(auth_tx->TLF01_details.response_code, "05", 2) &&
	   0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity, "10", 2))
	{
		strncpy(db_recs.merchant_intgerity_data.last_declined_rc, auth_tx->TLF01_details.product_codes[13].quantity, 2);
	}
	else
	{
		strncpy(db_recs.merchant_intgerity_data.last_declined_rc, auth_tx->TLF01_details.response_code, 2);
	}

	memcpy(db_recs.merchant_intgerity_data.block_reset_date,     "MAC",      3);
	memcpy(db_recs.merchant_intgerity_data.first_reattempt_date, mc_mac_code, strlen(mc_mac_code));
	memcpy(db_recs.merchant_intgerity_data.reattempts_date,      auth_tx->TLF01_details.total_amount,
																 strlen(auth_tx->TLF01_details.total_amount));

	return PTEMSG_OK;
}

int prepare_MIP_data_for_update_MAC(pAUTH_TX auth_tx, int category)
{
	int i_reset_days=0, i_allowable_reattempts=0;
	int c1Count = 0, c2Count = 0;
	BYTE start_date[9]={0}, end_date[9]={0};
	LONG julian_start_date=0, julian_end_date=0;

	c1Count = atoi(db_recs.merchant_intgerity_data.c1_count);
	c2Count = atoi(db_recs.merchant_intgerity_data.c2_count);

	/*calculate the start and end date*/
	create_system_date(start_date);
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);

	if(category == MAC_CAT_1)
	{
		c1Count++;
		if(c1Count < atoi(mac_allowable_reattempts_cat1))
		{
			db_recs.merchant_intgerity_data.c1_block[0] = 'N';
			sprintf(db_recs.merchant_intgerity_data.c1_count, "%02d", c1Count);
		}
		else
		{
			db_recs.merchant_intgerity_data.c1_block[0] = 'Y';
			sprintf(db_recs.merchant_intgerity_data.c1_count, "%02d", c1Count);
			i_reset_days = atoi(mac_reset_days_cat1);
			julian_end_date = i_reset_days + julian_start_date;
			Txutils_Calculate_Gregorian_Date(julian_end_date, end_date);
			strncpy(db_recs.merchant_intgerity_data.c1_block_start_date, start_date, 8);
			strncpy(db_recs.merchant_intgerity_data.c1_block_end_date,   end_date,   8);
		}
	}
	else if(category == MAC_CAT_2)
	{
		c2Count++;
		if(c2Count < atoi(mac_allowable_reattempts_cat2))
		{
			db_recs.merchant_intgerity_data.c2_block[0] = 'N';
			sprintf(db_recs.merchant_intgerity_data.c2_count, "%02d", c2Count);
		}
		else
		{
			db_recs.merchant_intgerity_data.c2_block[0] = 'Y';
			sprintf(db_recs.merchant_intgerity_data.c2_count, "%02d", c2Count);
			i_reset_days = atoi(mac_reset_days_cat2);
			julian_end_date = i_reset_days + julian_start_date;
			Txutils_Calculate_Gregorian_Date(julian_end_date, end_date);
			strncpy(db_recs.merchant_intgerity_data.c2_block_start_date, start_date, 8);
			strncpy(db_recs.merchant_intgerity_data.c2_block_end_date,   end_date,   8);
		}
	}

	/* for Partially approved transaction MC will send 10, ATP is changing it to 05,
			added this condition to store the actual RC sent by MC
	*/
	if(0 == strncmp(auth_tx->TLF01_details.response_code, "05", 2) &&
	   0 == strncmp(auth_tx->TLF01_details.product_codes[13].quantity, "10", 2))
	{
		strncpy(db_recs.merchant_intgerity_data.last_declined_rc, auth_tx->TLF01_details.product_codes[13].quantity, 2);
	}
	else
	{
		strncpy(db_recs.merchant_intgerity_data.last_declined_rc, auth_tx->TLF01_details.response_code, 2);
	}

	memcpy(db_recs.merchant_intgerity_data.block_reset_date,     "MAC",       3);
	memcpy(db_recs.merchant_intgerity_data.first_reattempt_date, mc_mac_code, strlen(mc_mac_code));
	memcpy(db_recs.merchant_intgerity_data.reattempts_date,      auth_tx->TLF01_details.total_amount,
																 strlen(auth_tx->TLF01_details.total_amount));

	return PTEMSG_OK;
}

int updatds_check_mac_category(char *posEntry, char *posCond, char *hand_queue)
{
	if(strcmp(posEntry, mac_pos_entry_mode_cat1) == 0 &&
	   strcmp(posCond,  mac_pos_cond_code_cat1)  == 0)
	{
		return MAC_CAT_1;
	}
	else if(strcmp(posEntry, mac_pos_entry_mode_cat2) == 0 &&
	        strcmp(posCond,  mac_pos_cond_code_cat2)  == 0)
	{
		return MAC_CAT_2;
	}
	else if(strncmp(hand_queue, "VOICE", 5) == 0)
	{
		//Voice is considered as Manual Key-in transactions
		//Manual key-in falls under category 1
		return MAC_CAT_1;
	}
	return MAC_NOT_DEFINED;
}
#endif


#ifdef VISA7
INT updatds_check_RC_category(char * resp_code)
{
	int index=0;
	for(index=0;index<total_cat1_visa_rc;index++)
	{
		if(0 == strncmp(Visa_cat1_rc[index],resp_code,2))
		{
			return 1;
		}
	}
	for(index=0;index<total_cat2_visa_rc;index++)
	{
		if(0 == strncmp(Visa_cat2_rc[index],resp_code,2))
		{
			return 2;
		}
	}
	for(index=0;index<total_cat3_visa_rc;index++)
	{
		if(0 == strncmp(Visa_cat3_rc[index],resp_code,2))
		{
			return 3;
		}
	}
	return 4;
}
#endif

#ifdef VISA7
INT updatds_check_if_txn_falls_under_merchant_ingerity_validation(pTLF01 tlf01)
{
	if( ((0 == strncmp(tlf01->handler_queue,"dcpiso",6)) &&
	     ((0 == strncmp(tlf01->message_type,"0200",4))   ||
	      ((0 == strncmp(tlf01->message_type,"0100",4))  &&
	       ((0 == strncmp(tlf01->processing_code,"30",2)) || (0 == strncmp(tlf01->processing_code,"38",2))))) )
		||
	    ((0 == strncmp(tlf01->handler_queue,"VOICE",5)) && (0 == strncmp(tlf01->message_type,"0100",4)))
	  )
	{
		return 1;
	}

	return 0;
}
#endif

#ifdef MC13_OCT2023
INT updatds_check_if_txn_falls_under_mac_validation(pAUTH_TX p_auth_tx)
{
	char cnp_indicator[2] = {0};
	char rspTextBuffer[41] = {0};

	/* DE61 - POS Data is stored in acceptor_term_name and
	 * 		  CNP indicator(61.5) is present at 4th index position.
	 * DE48 SE84 - MAC Code is stored in response text along with Tag value(DE48 SE84:)
	 * 			   so MAC is present at 10th and 11th index positions.
	 * */

	//check the response text len and string DE48 SE84
	memcpy(rspTextBuffer, p_auth_tx->TLF01_details.response_text, strlen(p_auth_tx->TLF01_details.response_text));
	if(strlen(rspTextBuffer) == 12 && (strstr(rspTextBuffer, "DE48 SE84:") != NULL))
	{
		memcpy(mc_mac_code, rspTextBuffer+10, 2);

		//replace the response_text with voice_auth_text and vice-versa for VOICE txn updated in VoiceAuth.cpp
		if( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "VOICE", 5) )
		{
			memset(p_auth_tx->TLF01_details.response_text, 0, sizeof(p_auth_tx->TLF01_details.response_text));
			memcpy(p_auth_tx->TLF01_details.response_text, p_auth_tx->TLF01_details.voice_auth_text,
				   strlen(p_auth_tx->TLF01_details.voice_auth_text));
			memset(p_auth_tx->TLF01_details.voice_auth_text, 0, sizeof(p_auth_tx->TLF01_details.voice_auth_text));
			memcpy(p_auth_tx->TLF01_details.voice_auth_text, rspTextBuffer, strlen(rspTextBuffer));
		}
	}
	else
		return 0;

	//check the acceptor_term_name len
	if(strlen(p_auth_tx->acceptor_term_name) >  4)
		cnp_indicator[0] = p_auth_tx->acceptor_term_name[4];
	else
		return 0;

	if( (updatds_validate_mac_validation1(&p_auth_tx->TLF01_details))   &&
		(cnp_indicator[0] == '1')
	  )
	{
		for(int i=0; i<total_mac_codes; i++)
		{
			if( strncmp(mc_mac_code, mac_codes[i], 2) == 0 )
			{
				return 1;
			}
		}
	}

	return 0;
}

INT updatds_validate_mac_validation1(pTLF01 tlf01)
{
	if( ( ((0 == strncmp(tlf01->handler_queue,       "dcpiso",                6)) ||
		   (0 == strncmp(tlf01->handler_queue,       "dcpos",                 5)))    &&
		  (((0 == strncmp(tlf01->message_type,       "0200",                  4)) &&
		    (0 == strncmp(tlf01->processing_code,    "00",                    2)))  ||
		   ((0 == strncmp(tlf01->message_type,       "0100",                  4)) &&
		    ((0 == strncmp(tlf01->processing_code,   "30",                    2)) ||
		     (0 == strncmp(tlf01->processing_code,   "38",                    2)))))   &&
		  (((0 == strncmp(tlf01->pos_entry_mode,     mac_pos_entry_mode_cat1, 3)) &&
		    (0 == strncmp(tlf01->pos_condition_code, mac_pos_cond_code_cat1,  2)))  ||
		   ((0 == strncmp(tlf01->pos_entry_mode,     mac_pos_entry_mode_cat2, 3)) &&
		    (0 == strncmp(tlf01->pos_condition_code, mac_pos_cond_code_cat2,  2)))) ) ||
	    ( (0 == strncmp(tlf01->handler_queue, "VOICE", 5)) &&
		  (0 == strncmp(tlf01->message_type,  "0100",  4)) )
	  )
	{
		return 1;
	}

	return 0;
}
#endif

#ifdef VISA7
INT updatds_read_last_updated_date_parameter()
{
    DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	CHAR rc_data[256] ={0};
	INT  ret_val = true;

	memset(last_data_update_date,0x00,sizeof(last_data_update_date));
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
	             "MERCHANT_INTEGRITY_VISA",     /* points to section name       */
	             "LAST_DATE",  							/* points to key name           */
	             "",                       		/* points to default string     */
	             ini_info,                      /* points to destination buffer */
	             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	             filename                       /* points to ini filename       */
	);
	if(rc != 1)
	{
		updateds_log_message(2,2,"system_integrity_parameters LAST_DATE not read","updatds_read_system_integrity_parameters");
	}

	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( last_data_update_date, tmpstring,strlen(tmpstring));

	return ret_val;
}


INT updatds_read_system_integrity_parameters( void )
{
    DWORD rc;
    CHAR filename     [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
    CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
    CHAR rc_data[256] ={0};
    INT  ret_val = true;
    INT index=0,rc_index=0;
    /* Determine directory and filename of the .ini file. */

    total_cat1_visa_rc=0,total_cat2_visa_rc=0,total_cat3_visa_rc=0;
    cat1_MID_integrity_validation_flag='\0',cat2_MID_integrity_validation_flag='\0';
    cat3_MID_integrity_validation_flag='\0',cat4_MID_integrity_validation_flag='\0';
    memset(allowable_reattempts_234,0x00,sizeof(allowable_reattempts_234));
    memset(reset_days_1,0x00,sizeof(reset_days_1));
    memset(reset_days_234,0x00,sizeof(reset_days_234));
    memset(last_data_update_date,0x00,sizeof(last_data_update_date));

    memset(Visa_cat1_rc,0x00,sizeof(Visa_cat1_rc));
    memset(Visa_cat2_rc,0x00,sizeof(Visa_cat2_rc));
    memset(Visa_cat3_rc,0x00,sizeof(Visa_cat3_rc));

    memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "ALLOWABLE_REATTEMPTS_CAT_2_3_4",
             "15",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
    	updateds_log_message(2,2,"system_integrity_parameters ALLOWABLE_REATTEMPTS not read","updatds_read_system_integrity_parameters");
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    strncpy( allowable_reattempts_234, tmpstring,sizeof(allowable_reattempts_234)-1);

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "RESET_DAYS_CAT_1",
             "5",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );
    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters RESET_DAYS for category 1 not read","updatds_read_system_integrity_parameters");
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    strncpy( reset_days_1, tmpstring,sizeof(reset_days_1)-1);

    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "RESET_DAYS_CAT_2_3_4",
             "30",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );
    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters RESET_DAYS for category 2,3 and 4 not read","updatds_read_system_integrity_parameters");
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    strncpy( reset_days_234, tmpstring,sizeof(reset_days_234)-1);

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "LAST_DATE",
             "",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );
    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters LAST_DATE not read","updatds_read_system_integrity_parameters");
    }
    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    strncpy( last_data_update_date, tmpstring,sizeof(last_data_update_date)-1);
    /* if null use system date*/

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY1",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY1 not read","updatds_read_system_integrity_parameters");
    }
    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat1_MID_integrity_validation_flag = tmpstring[0];

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY2",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );
    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY2 not read","updatds_read_system_integrity_parameters");
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat2_MID_integrity_validation_flag = tmpstring[0];

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY3",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY3 not read","updatds_read_system_integrity_parameters");
    }
    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat3_MID_integrity_validation_flag = tmpstring[0];

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY4",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY4 not read","updatds_read_system_integrity_parameters");
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat4_MID_integrity_validation_flag = tmpstring[0];

    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY1_RC",
             "",
			 rc_data,
             sizeof(rc_data) - 1,
             filename
    );

    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY1_RC not read","updatds_read_system_integrity_parameters");
    }

    if(cat1_MID_integrity_validation_flag == 'Y' && strlen(rc_data) > 0 && rc_data[(strlen(rc_data)-1)] == ';')
    {
		while(rc_data[index] != ';')
		{
			if(index == 0)
			{
				strncpy( Visa_cat1_rc[rc_index],&rc_data[index],2);
				index+=2;
			}
			else
			{
				strncpy( Visa_cat1_rc[rc_index],&rc_data[index+1],2);
				index+=3;
			}
			rc_index++;
		}
		total_cat1_visa_rc=rc_index;
    }

    rc_index=0,index=0,memset(rc_data,0x00,sizeof(rc_data));

    rc = GetPrivateProfileString(
              "MERCHANT_INTEGRITY_VISA",
              "CATEGORY2_RC",
              "",
 			  rc_data,
              sizeof(rc_data) - 1,
              filename
     );

    if(rc != 1)
    {
	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY2_RC not read","updatds_read_system_integrity_parameters");
    }

    if(cat2_MID_integrity_validation_flag == 'Y' && strlen(rc_data) > 0 && rc_data[(strlen(rc_data)-1)] == ';')
    {
		while(rc_data[index] != ';')
		{
			if(index == 0)
			{
				strncpy( Visa_cat2_rc[rc_index],&rc_data[index],2);
				index+=2;
			}
			else
			{
				strncpy( Visa_cat2_rc[rc_index],&rc_data[index+1],2);
				index+=3;
			}
			rc_index++;
		}
		total_cat2_visa_rc=rc_index;
    }

    rc_index=0,index=0,memset(rc_data,0x00,sizeof(rc_data));

    rc = GetPrivateProfileString(
               "MERCHANT_INTEGRITY_VISA",
               "CATEGORY3_RC",
               "",
  			   rc_data,
               sizeof(rc_data) - 1,
               filename
    );

    if(rc != 1)
    {
 	    updateds_log_message(2,2,"system_integrity_parameters CATEGORY3_RC not read","updatds_read_system_integrity_parameters");
    }

    if(cat3_MID_integrity_validation_flag == 'Y' && strlen(rc_data) > 0 && rc_data[(strlen(rc_data)-1)] == ';')
    {
		while(rc_data[index] != ';')
		{
			if(index == 0)
			{
				strncpy( Visa_cat3_rc[rc_index],&rc_data[index],2);
				index+=2;
			}
			else
			{
				strncpy( Visa_cat3_rc[rc_index],&rc_data[index+1],2);
				index+=3;
			}
			rc_index++;
		 }
		 total_cat3_visa_rc=rc_index;
    }

    return( ret_val );
}
#endif

#ifdef MC13_OCT2023
INT updatds_read_last_updated_date_parameter_for_MAC()
{
    DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	CHAR rc_data[256] ={0};
	INT  ret_val = true;

	memset(mc_last_data_update_date,0x00,sizeof(mc_last_data_update_date));
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
	             "MC_MERCHANT_ADVICE_CODE",     /* points to section name       */
	             "LAST_DATE",  							/* points to key name           */
	             "",                       		/* points to default string     */
	             ini_info,                      /* points to destination buffer */
	             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	             filename                       /* points to ini filename       */
	);
	if(rc != 1)
	{
		updateds_log_message(2, 2, "LAST_DATE not read", "updatds_read_last_updated_date_parameter_for_MAC");
	}

	strncpy( mac_last_data_update_date, ini_info, strlen(ini_info));

	return ret_val;
}

INT updatds_read_merchant_advice_code_parameters()
{
    DWORD rc;
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};
    CHAR mac_data  [256] 			   = {0};
    INT  ret_val = true;
    INT  index = 0, mac_index = 0;

    mac_cat1_validation_flag='\0';
    mac_cat2_validation_flag='\0';
    memset(mac_codes,  					  0x00, sizeof(mac_codes));
    memset(mac_last_data_update_date,     0x00, sizeof(mac_last_data_update_date));
    memset(mac_allowable_reattempts_cat1, 0x00, sizeof(mac_allowable_reattempts_cat1));
    memset(mac_allowable_reattempts_cat2, 0x00, sizeof(mac_allowable_reattempts_cat2));
    memset(mac_reset_days_cat1,			  0x00, sizeof(mac_reset_days_cat1));
    memset(mac_reset_days_cat2,		      0x00, sizeof(mac_reset_days_cat2));
    memset(mac_pos_entry_mode_cat1, 	  0x00, sizeof(mac_pos_entry_mode_cat1));
    memset(mac_pos_cond_code_cat1,  	  0x00, sizeof(mac_pos_cond_code_cat1));
    memset(mac_pos_entry_mode_cat2, 	  0x00, sizeof(mac_pos_entry_mode_cat2));
    memset(mac_pos_cond_code_cat2,  	  0x00, sizeof(mac_pos_cond_code_cat2));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "CATEGORY1",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "CATEGORY1 not read", "updatds_read_merchant_advice_code_parameters");
	}

	mac_cat1_validation_flag = ini_info[0];

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "CATEGORY2",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "CATEGORY2 not read", "updatds_read_merchant_advice_code_parameters");
	}

	mac_cat2_validation_flag = ini_info[0];

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "LAST_DATE",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "LAST_DATE not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_last_data_update_date ,ini_info, sizeof(mac_last_data_update_date)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "ALLOWABLE_REATTEMPTS_CAT1",
                "1",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "ALLOWABLE_REATTEMPTS_CAT1 not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_allowable_reattempts_cat1 ,ini_info, sizeof(mac_allowable_reattempts_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "ALLOWABLE_REATTEMPTS_CAT2",
                "1",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "ALLOWABLE_REATTEMPTS_CAT2 not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_allowable_reattempts_cat2 ,ini_info, sizeof(mac_allowable_reattempts_cat2)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "RESET_DAYS_CAT1",
                "30",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "RESET_DAYS_CAT1 not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_reset_days_cat1 ,ini_info, sizeof(mac_reset_days_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "RESET_DAYS_CAT2",
                "30",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "RESET_DAYS_CAT2 not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_reset_days_cat2 ,ini_info, sizeof(mac_reset_days_cat2)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_ENTRY_MODE_CAT1",
                "0012",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "POS_ENTRY_MODE_CAT1 validation flag not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_pos_entry_mode_cat1 ,ini_info, sizeof(mac_pos_entry_mode_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_COND_CODE_CAT1",
                "00",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "POS_COND_CODE_CAT1 validation flag not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_pos_cond_code_cat1 ,ini_info, sizeof(mac_pos_cond_code_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_ENTRY_MODE_CAT2",
                "0012",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "POS_ENTRY_MODE_CAT2 validation flag not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_pos_entry_mode_cat2 ,ini_info, sizeof(mac_pos_entry_mode_cat2)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_COND_CODE_CAT2",
                "08",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   updateds_log_message(2, 2, "POS_COND_CODE_CAT2 validation flag not read", "updatds_read_merchant_advice_code_parameters");
	}

	strncpy(mac_pos_cond_code_cat2 ,ini_info, sizeof(mac_pos_cond_code_cat2)-1);

    rc = GetPrivateProfileString(
             "MC_MERCHANT_ADVICE_CODE",
             "MAC",
             "",
			 mac_data,
             sizeof(mac_data) - 1,
             filename
    );

    if(rc != 1)
    {
	    updateds_log_message(2, 2, "MAC codes not read", "updatds_read_merchant_advice_code_parameters");
    }

    index = 0; mac_index = 0;
    if( mac_cat1_validation_flag == 'Y' && mac_cat2_validation_flag == 'Y' &&
        strlen(mac_data) > 0 && mac_data[(strlen(mac_data)-1)] == ';' )
    {
		while(mac_data[index] != ';')
		{
			if(index == 0)
			{
				strncpy(mac_codes[mac_index], &mac_data[index], 2);
				index += 2;
			}
			else
			{
				strncpy(mac_codes[mac_index], &mac_data[index+1], 2);
				index += 3;
			}
			mac_index++;
		}
		total_mac_codes = mac_index;
    }

	return ret_val;
}
#endif

INT updatds_read_CUP_merchant_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMIDSECTION",     /* points to section name       */
             "CUP_MID_INDICATOR",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
   if(strlen(CUP_merchant_indicator)==0)
   {
	   strncpy( CUP_merchant_indicator,
			    CUP_MID_INDICATOR_DEFAULT,
			    CUP_MID_INDICATOR_LEN);// Default Value
   }
   return( ret_val );
}

void updatds_load_MID_CPNG_response_text()
{
	DWORD rc;
	CHAR  filename   [80] = {0};
	CHAR  tmpstr     [80] = {0};
	CHAR  tmpstring	[20] = {0};
	CHAR  tmp_ini    [40] = {0};

	memset( tmpstr, 0x00, sizeof(tmpstr) );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	rc = GetPrivateProfileString("MID_RESPONSE_CODE",   	/* points to section name         */
								 "RESPONSE_TEXT",       	/* points to key name             */
								 "0",  						/* Default string                 */
							     tmp_ini,              		/* points to destination buffer   */
								 sizeof(tmp_ini)-1,   	 	/* size of destination buffer     */
								 filename                  	/* points to ini filename         */
							 	 );

	strncpy( pos_res_txt, tmp_ini, sizeof(tmp_ini)-1);
}

INT updatds_Check_If_CUP_Transaction (pAUTH_TX auth_tx)
{
	if ((0 == strncmp(auth_tx->TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx->BIN01_details.card_family,CARD_CUP,CARD_TYPE_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT updatds_Check_If_AMEX_Transaction (pAUTH_TX auth_tx)
{
	if((auth_tx->TLF01_details.card_num[0]=='3') &&
	   ((auth_tx->TLF01_details.card_num[1] == '4') ||
		(auth_tx->TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else if(0 == strcmp(auth_tx->BIN01_details.card_family,CARD_AMEX))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT get_variable_auth_config_details()
{
	   CHAR  look_back_days[3]={0};
	   CHAR  date[9]={0};
	   INT   ret_val = true;
	   CHAR  filename   [80];
	   CHAR  tmpstr     [80];

	   /* Get path to the tf.ini file.*/
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   memset(look_back_days,0x00,sizeof(look_back_days));
	   memset(last_date,0x00,sizeof(last_date));
	  /* Variable authorization configuration
	    * */
	   ret_val = GetPrivateProfileString(
								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
								  "LOOKBACK_DAYS",       	/* points to key name             */
								  "30",  					/* Default string                 */
								  look_back_days,              	/* points to destination buffer   */
								  sizeof(look_back_days)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	   LOOKBACK_DAYS=atoi(look_back_days);


	   ret_val = GetPrivateProfileString(
								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
								  "LAST_DATE",       	/* points to key name             */
								  "",  					/* Default string                 */
								  date,              	/* points to destination buffer   */
								  sizeof(date)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	   strncpy(last_date,date,8);

	   return ret_val;
}

INT get_last_dated_date_only()
{
	   CHAR  date[9]={0};
	   INT   ret_val = true;
	   CHAR  filename   [80];
	   CHAR  tmpstr     [80];

	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   memset(last_date,0x00,sizeof(last_date));

	   ret_val = GetPrivateProfileString(
								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
								  "LAST_DATE",       	/* points to key name             */
								  "",  					/* Default string                 */
								  date,              	/* points to destination buffer   */
								  sizeof(date)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	   strncpy(last_date,date,8);
	   return ret_val;
}

INT updatds_check_If_Tra_qualify_MC_512_incremental_Preauth (pAUTH_TX auth_tx)
{

	return false;
	if(	0 == strncmp(auth_tx->TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return false;
	}

	else if((true == updatds_Check_If_MC_Transaction(auth_tx)) &&
			((0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcrd",6))||
			 (0 == strncmp(auth_tx->BIN01_details.destination_name,"ncmcrd",6))||
			 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcr3",6))||
			 (0 == strncmp(auth_tx->BIN01_details.destination_name,"ncmcr3",6))))
	{
		return true;
	}
	else if((true == updatds_Check_If_VISA_Transaction(auth_tx)) &&
			((0 == strncmp(auth_tx->CAF01_details.destination_name,"ncvisa",6))||
			 (0 == strncmp(auth_tx->BIN01_details.destination_name,"ncvisa",6))))
	{
		return true;
	}
	else
	{
		return false;
	}
}
#ifdef VISA7
INT updatds_Check_If_VISA_Transaction (pTLF01 tlf01)
{
	if (tlf01->card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}
#endif

INT updatds_Check_If_MC_Transaction (pAUTH_TX auth_tx)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,auth_tx->TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	auth_tx->TLF01_details.card_num[0]=='5'||
		(0 == strcmp(auth_tx->BIN01_details.card_family,CARD_MASTERCARD)))
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT updatds_Check_If_Diners_Transaction (pAUTH_TX auth_tx)
{
	if((auth_tx->TLF01_details.card_num[0]=='3') &&
	   ((auth_tx->TLF01_details.card_num[1] == '0') ||
		(auth_tx->TLF01_details.card_num[1] == '6') ||
		(auth_tx->TLF01_details.card_num[1] == '8') ||
		(auth_tx->TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((auth_tx->TLF01_details.card_num[0]=='6') &&
			 ((auth_tx->TLF01_details.card_num[1] == '0') ||
			 (auth_tx->TLF01_details.card_num[1] == '4') ||
			 (auth_tx->TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT updatds_Check_If_JCB_Transaction (pAUTH_TX auth_tx)
{
	if((auth_tx->TLF01_details.card_num[0]=='3') &&
	   (auth_tx->TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

void get_time( pCHAR time_date, pCHAR time_str )
{
   memset ( time_str,  0x00,          7 );
   strncpy( time_str, &time_date[11], 2 );
   strncat( time_str, &time_date[14], 2 );
   strncat( time_str, &time_date[17], 2 );
}

/* TF-Phani*/
BYTE IsFieldNull (char str[])
{
	if (strlen (str) == 0)
		return 1 ;
	return 0 ;
}

void calc_julian_day( pCHAR date_str, pCHAR julian_str )
{
   CHAR  year[5]  = {0};
   CHAR  month[3] = {0};
   CHAR  day[3]   = {0};
   INT   iYear	  =  0;
   INT   iMonth   =  0;
   INT   iDay	  =  0;
   INT   julian_day = 0;
   INT   i 			= 0;
   INT   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   strncpy( year, date_str, 4 );
   iYear  = atoi( year );

   strncpy( month, &date_str[4], 2 );
   iMonth = atoi( month );

   strncpy( day, &date_str[6], 2 );
   iDay   = atoi( day );

   for( i=0,julian_day=0; i<iMonth-1 && i<12; i++ )
   {
      julian_day += DaysOfMonth[i];
   }
   julian_day += iDay;

   if ( 2 < iMonth )
      if ( 0 == ( iYear % 4 ) )
         julian_day++;

   sprintf( julian_str, "%03d", julian_day );
   return;
}


/******************************************************************************
 *
 *  NAME:         CREATE_RRN
 *
 *  DESCRIPTION:  This procedure creates a retrieval reference number in the
 *                format of YDDDhmmssjjj, that is:
 *                     Y   = last digit of the year
 *                     DDD = Julian day
 *                     h   = last digit of the 24 hour clock (current time)
 *                     mm  = current system minutes
 *                     ss  = current system seconds
 *                     jjj = current system milliseconds
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      rrn - Formatted as mentioned above in the description
 *                      RRN should be a 13 character string.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_rrn( pCHAR rrn )
{
   CHAR    julian_day[4];
   CHAR    date_str[9];
   CHAR    time_str[7];
   CHAR    time_date[25]; 

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   (void) ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Change date to julian format - DDD */
   calc_julian_day( date_str, julian_day );

   /* Format RRN: "YDDDhmmssjjj" */
   sprintf( rrn, "%1.1s%3.3s%5.5s%3.3s",time_date+3, julian_day,
                time_str+1, time_date+20 );
   return;
}/* This function is mainly for reversal. It is a generic function, but only a few cases is in use*/

BYTE get_orig_response_tx_key_for_reversal(char * msg_type, char * processing_code, char * voice_auth)
{  
   BYTE  ret_val;

   if (strcmp(msg_type, "0100") == 0)
   {
      if (strncmp(processing_code,      "00",2) == 0) ret_val = AUTH_AUTHORIZATION_RESPONSE;
      else if (strncmp(processing_code, "30",2) == 0) ret_val = AUTH_PRE_AUTHORIZATION_RESPONSE;
      else if (strncmp(processing_code, "31",2) == 0) ret_val = AUTH_BALANCE_INQUIRY_RESPONSE;
      else if (strncmp(processing_code, "38",2) == 0) ret_val = AUTH_CARD_VERIFICATION_RESPONSE;
      else if (strncmp(processing_code, "04",2) == 0) ret_val = AUTH_CHECK_VERIFICATION_RESPONSE;
      else if (strncmp(processing_code, "02",2) == 0) ret_val = AUTH_VOID_PRE_AUTHORIZATION_RESPONSE;
      else if (strncmp(processing_code, "11",2) == 0) ret_val = AUTH_QUASI_CASH_RESPONSE;
      else ret_val = AUTH_INVALID;
   } 
   else if (strcmp(msg_type, "0200") == 0)
   {
      if (strncmp(processing_code,      "00",2) == 0) ret_val = AUTH_SALE_RESPONSE;
      else if (strncmp(processing_code, "20",2) == 0) ret_val = AUTH_REFUND_RESPONSE;
      else if (strncmp(processing_code, "02",2) == 0) ret_val = AUTH_VOID_SALE_RESPONSE;
      else if (strncmp(processing_code, "22",2) == 0) ret_val = AUTH_VOID_REFUND_RESPONSE;
      else if (strncmp(processing_code, "01",2) == 0) ret_val = AUTH_CASH_ADVANCE_RESPONSE;
      else if (strncmp(processing_code, "04",2) == 0) ret_val = AUTH_CHECK_VERIFICATION_RESPONSE;
      else if (strncmp(processing_code, "09",2) == 0) ret_val = AUTH_SALE_CASH_RESPONSE;
      else if (strncmp(processing_code, "11",2) == 0) ret_val = AUTH_QUASI_CASH_RESPONSE;
      else ret_val = AUTH_INVALID;
   } 
   else if (strcmp(msg_type, "0220") == 0)
   {
      if (strncmp(processing_code,      "00",2) == 0) ret_val = AUTH_OFFLINE_SALE_RESPONSE;
      else if (strncmp(processing_code, "20",2) == 0) ret_val = AUTH_OFFLINE_REFUND_RESPONSE;
      else if (strncmp(processing_code, "02",2) == 0) ret_val = AUTH_SALE_ADJUSTMENT_RESPONSE;
      else if (strncmp(processing_code, "22",2) == 0) ret_val = AUTH_REFUND_ADJUSTMENT_RESPONSE;
      else if (strncmp(processing_code, "42",2) == 0) ret_val = AUTH_PAYMENT_ADJUSTMENT_RESPONSE;
      else ret_val = AUTH_INVALID;
   } 
   else if (strcmp(msg_type, "0320") == 0)
   {
      if (strncmp(processing_code, "90",2) == 0) ret_val = AUTH_SIGNATURE_DATA_RESPONSE; 
      ret_val = AUTH_BATCH_UPLOAD_RESPONSE;
   }
   else if (strcmp(msg_type, "0400") == 0) ret_val = AUTH_REVERSAL_RESPONSE;
   else if (strcmp(msg_type, "0500") == 0)
   {
      if (strncmp(processing_code,      "92",2) == 0) ret_val = AUTH_SETTLE_RESPONSE;
      else if (strncmp(processing_code, "96",2) == 0) ret_val = AUTH_SETTLE_TRAILER_RESPONSE;
      else if (strncmp(processing_code, "95",2) == 0) ret_val = AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE;
      else ret_val = AUTH_INVALID;
   } 
   else if (strcmp(msg_type, "0800") == 0)
   {
      if (strncmp(processing_code,      "90",2) == 0) ret_val = AUTH_PIN_CHANGE_RESPONSE;
      else if (strncmp(processing_code, "91",2) == 0) ret_val = AUTH_STATISTICS_RESPONSE;
      else if (strncmp(processing_code, "92",2) == 0) ret_val = AUTH_LOGON_RESPONSE;
      else if (strncmp(processing_code, "93",2) == 0) ret_val = AUTH_INITIALIZATION_RESPONSE;
      else if (strncmp(processing_code, "99",2) == 0) ret_val = AUTH_TEST_RESPONSE;
      else ret_val = AUTH_INVALID;
   } 
   else if (strcmp(msg_type, "0830") == 0) ret_val = AUTH_PLEASE_WAIT_ADVICE_RESPONSE;
   else ret_val = AUTH_INVALID ;

   return( ret_val );
}

void format_date(pCHAR temp)
{
   struct timeb   time_structure;   // Time for ftime
   time_t long_time_since_1970;     // Number of seconds since 01/01/1970 A.D. 12:00 A.M.
   struct tm *current_time;         // Current time structure
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);
   return;
} 

void prepare_bch10_for_update(pAUTH_TX auth_tx)
{
   BYTE temp_msg[100] = {0};
   int DCI_NRID_len = 0;
   INT Max_DCI_NRID_len = 15;
   INT original_pem_flag_for_offline_sale = 0;

   strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
   strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
   strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
   strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
   strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
   if ( AUTH_SALE_ADJUSTMENT == auth_tx->TLF01_details.tx_key ||
		AUTH_SALE_ADJUSTMENT_RESPONSE == auth_tx->TLF01_details.tx_key)
   {
	   strncpy(db_recs.bch10.processing_code,"000000",
			   (sizeof(auth_tx->TLF01_details.processing_code) -1 ));
   }
   else
   {
		/* IN cash-adv->void sale-> void rversal scenrio processing code of cash advance will be missed
		 * therefore store original proc code and tx key during void sale, if void reversal comes then update original
		 * proc code and tx key*/
		if(strncmp(auth_tx->TLF01_details.handler_queue,"dcpiso",6) == 0)
		{
			if((auth_tx->TLF01_details.tx_key == AUTH_CASH_ADVANCE || auth_tx->TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE))
			{
				db_recs.bch10.product_codes[7].quantity[0] = AUTH_CASH_ADVANCE;
				strncpy(db_recs.bch10.product_codes[7].amount,auth_tx->TLF01_details.processing_code,6);
			}
			/* This section will copy processing code as it is, below is the special code to hnadle
			 * cash advance->void sale-> void reversal case*/
			strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);

		}
		else
		{
			strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
		}
   }
   strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
   strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
   strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
   strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
   strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
   strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
   strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
   strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
   db_recs.bch10.bin_type = auth_tx->bin_type;
   if (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
   {
	   if (strlen(auth_tx->TLF01_details.product_codes[10].code)>=4)
	   {
		   strncpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.product_codes[10].code,4);
		   original_pem_flag_for_offline_sale = 1;
	   }
	   else
	   {
		   strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
	   }
   }
   else
   {
	   strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
   }
   strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
   strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
   strcpy(db_recs.bch10.transmission_timestamp,auth_tx->TLF01_details.transmission_timestamp);
   strcpy(db_recs.bch10.mcard_banknet,auth_tx->TLF01_details.mcard_banknet);
   if(IsFieldNull(db_recs.bch10.mcard_banknet)) /* To store AMEX tran ID -DE31*/
   {
	   strcpy(db_recs.bch10.mcard_banknet,auth_tx->TLF01_details.visa_tran_id);
   }
   strcpy (db_recs.bch10.cash_bonus, auth_tx->TLF01_details.cash_bonus);
   strcpy (db_recs.bch10.ticket_nbr, auth_tx->TLF01_details.ticket_nbr);
   strcpy (db_recs.bch10.odometer, auth_tx->TLF01_details.odometer);
   strcpy( db_recs.bch10.def_gross_amt, auth_tx->TLF01_details.def_gross_amt );
   strcpy( db_recs.bch10.term, auth_tx->TLF01_details.deferred_term_length );
   strcpy( db_recs.bch10.product_codes[19].code, auth_tx->TLF01_details.settlement_date );
   strcpy( db_recs.bch10.product_codes[6].amount, auth_tx->TLF01_details.pos_condition_code);/*Girija GB*/
   if(0 == strlen(db_recs.bch10.product_codes[6].amount))
   {
	  strncpy(db_recs.bch10.product_codes[6].amount,
				  "  ", (sizeof(auth_tx->TLF01_details.pos_condition_code)-1));
   }
   strcat( db_recs.bch10.product_codes[6].amount, auth_tx->TLF01_details.service_code );
   strcpy( db_recs.bch10.product_codes[16].amount, auth_tx->TLF01_details.product_codes[16].amount );
   /* Store Stand in flag in BCH10 also, This will be useful for VOID/Reversal/Adjustment transactions*/
   strcpy( db_recs.bch10.product_codes[1].code, auth_tx->TLF01_details.product_codes[1].code);/*TF Phani*/
   if((auth_tx->TLF01_details.tx_key== AUTH_VOID_SALE_RESPONSE ) && 
	  (strcmp(auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0))
   {
	   strcpy( db_recs.bch10.product_codes[1].code, VOIDED);
   }
	/* TF Phani-  Copy DE63 table Id 12- value */
	strcpy( db_recs.bch10.product_codes[14].amount, auth_tx->TLF01_details.product_codes[14].amount);/*TF PHani*/

	/* TF Abhishek copy field 61 for CUP and CUP co branded card */
	if (true == updatds_Check_If_CUP_Transaction (auth_tx))
	{
		strncpy( db_recs.bch10.product_codes[9].quantity, auth_tx->TLF01_details.product_code,2 );
		printf("Transmission timestamp and Service code for STAN %s is :%s   :%s in prepare_bch10_for_update \n",auth_tx->TLF01_details.sys_trace_audit_num,
															auth_tx->TLF01_details.transmission_timestamp,
															auth_tx->TLF01_details.service_code);

		/* We have to store original STAN of Preauth so that we can place it for CUP EPOS file for OFFLINE sale*/
		if (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
		{
			strncpy (db_recs.bch10.product_codes[9].code,auth_tx->TLF01_details.orig_sys_trace_num, (sizeof(auth_tx->TLF01_details.orig_sys_trace_num)-1)     ) ;
		}
		if( 0 == strncmp(auth_tx->TLF01_details.pos_entry_mode,"001",3))
		{ /* Changes are done as per Anna(BDO) request on 9th Nov 2018
			*/
			if (original_pem_flag_for_offline_sale == 1)
			{
				/*Its a offline same and PEM is set for the original transaction So need to update it again*/
			}
			else
			{
				strncpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode,4);/*For epos generation */
			}
		}
		if(0 == strlen(auth_tx->TLF01_details.pos_entry_mode))
		{
			strncpy(db_recs.bch10.pos_entry_mode,"0022",4);
		}
	}
	strncpy( db_recs.bch10.product_codes[19].code, auth_tx->TLF01_details.settlement_date, (sizeof(auth_tx->TLF01_details.settlement_date)-1) );

	if(true ==updatds_Check_If_Diners_Transaction(auth_tx) )
	{
		DCI_NRID_len = strlen(auth_tx->TLF01_details.resp_source);
		if(DCI_NRID_len > 0)
		{
			if(DCI_NRID_len > 6)
			{
				memcpy(	db_recs.bch10.product_codes[13].code,
						auth_tx->TLF01_details.resp_source,6);
				memcpy(	db_recs.bch10.product_codes[13].amount  ,
						auth_tx->TLF01_details.resp_source + 6,
						(Max_DCI_NRID_len - 6));
			}
			else
			{
				memcpy(	db_recs.bch10.product_codes[13].code,
						auth_tx->TLF01_details.resp_source,
						DCI_NRID_len);
			}

		}
		memcpy( db_recs.bch10.product_codes[12].amount ,
				auth_tx->TLF01_details.product_codes[12].amount,
				DINERS_DE12_SIZE);
		if(strlen(auth_tx->host2_message_seq_nbr)> 0)
		{
			memcpy( db_recs.bch10.product_codes[11].code,
					auth_tx->host2_message_seq_nbr + 1,
					PAN_SEQ_NUM);
		}
	}

	/*Pos entry mode chnages for EPOS file creation VISA mandated - 2018
	 * Stor the POS entry mode sending to host*/
	if(updatds_Check_If_MC_Transaction(auth_tx) == true)
	{
		memcpy( db_recs.bch10.product_codes[9].code,
				auth_tx->TLF01_details.product_codes[9].code,
				3);
#ifdef MC02_APR2024
	if((auth_tx->TLF01_details.tx_key == AUTH_SALE 					||
		auth_tx->TLF01_details.tx_key == AUTH_SALE_RESPONSE			||
		auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE 			||
		auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE ||
		auth_tx->TLF01_details.tx_key == AUTH_REVERSAL 				||
		auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE 	||
		auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE 			||
		auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE 	||
		auth_tx->TLF01_details.tx_key == AUTH_REFUND				||
		auth_tx->TLF01_details.tx_key == AUTH_REFUND_RESPONSE) &&
	   strlen(auth_tx->TLF01_details.product_codes[10].amount) > 0)
	{
		memcpy(db_recs.bch10.product_codes[10].amount,auth_tx->TLF01_details.product_codes[10].amount,10);
		memcpy(db_recs.bch10.product_codes[1].amount,auth_tx->TLF01_details.product_codes[1].amount,12);
	}
#endif
	}
	memcpy( db_recs.bch10.product_codes[17].code,
			auth_tx->TLF01_details.product_codes[17].code,
			3);

	// The changes done should affct only off-us transactions according to the april mandated BR in 9.14v
	if(true == updatds_Check_If_VISA_Transaction(auth_tx) &&
		(0 == strncmp(auth_tx->CAF01_details.destination_name,"ncvisa",6)))
	{
		db_recs.bch10.product_codes[0].code[0] = auth_tx->TLF01_details.resp_source[0];
		db_recs.bch10.product_codes[0].code[1] = '\0';
	}
    // Let us store if its ON US of OFF card
	if ((0 == strncmp(auth_tx->CAF01_details.authorization_location,"S",1)) &&
		((0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcr2",6)) ||
		 (0 == strncmp(auth_tx->CAF01_details.destination_name,"nccup2",6)) ||
		 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncjcb2",6)) ||
		 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncdci2",6)) ||
		 (0 == strncmp(auth_tx->CAF01_details.destination_name,"nposa2",6)) ||
		 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncvis2",6))))
	{
		// Its ON US tranasction
		memcpy( db_recs.bch10.product_codes[5].code,"ONUS",4);
	}
	else
	{
		// Its OFF US tranasction
		memcpy( db_recs.bch20.product_codes[5].code,"OFFUS",5);
	}

	if( (0 == strncmp(auth_tx->TLF01_details.handler_queue,"dcpiso",6)) &&
	   ((0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcr2",6)) ||
	   (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcrd",6))  ||
	   (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcr3",6))))
	{
		/* store the ECI value sent to cadencie*/
		strncpy(db_recs.bch20.product_codes[13].quantity ,auth_tx->TLF01_details.product_codes[12].quantity,2);
	}

	if(true == updatds_Check_If_VISA_Transaction(auth_tx) &&
		(0 == strncmp(auth_tx->CAF01_details.destination_name,"ncvisa",6)))
	{
		memcpy(db_recs.bch10.product_codes[1].amount, auth_tx->TLF01_details.product_codes[1].amount,10);
		memcpy(db_recs.bch10.product_codes[5].quantity, &auth_tx->TLF01_details.kek[4], 1);		/* copy F111 DS56 TG85 */
		strncat(db_recs.bch10.product_codes[5].quantity, &auth_tx->TLF01_details.kek[0], 1);		/* copy F111 DS56 TG80 */
		memcpy(db_recs.bch10.product_codes[7].code, auth_tx->TLF01_details.product_codes[7].code,3);
		memcpy(db_recs.bch10.down_payment, auth_tx->TLF01_details.add_amounts, 9);
		memcpy(db_recs.bch10.interest_rate, auth_tx->TLF01_details.visa_validation_code, 4);
		memcpy(db_recs.bch10.term, auth_tx->TLF01_details.currency_code, 3);
	}

   return;
} 

void prepare_bch10_for_update_MP(pAUTH_TX auth_tx)
{
   BYTE temp_msg[100] 	= {0};
   int DCI_NRID_len 	= 0;
   INT Max_DCI_NRID_len = 15;

   strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
   strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
   strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
   strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
   strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
   strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
   strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
   strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
   strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
   strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
   strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
   strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
   strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
   strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
   db_recs.bch10.bin_type = auth_tx->bin_type;
   strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
   strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
   strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
   strcpy(db_recs.bch10.transmission_timestamp,auth_tx->TLF01_details.transmission_timestamp);
   strcpy(db_recs.bch10.mcard_banknet,auth_tx->TLF01_details.mcard_banknet);
   if(IsFieldNull(db_recs.bch10.mcard_banknet)) /* To store AMEX tran ID -DE31*/
   {
	   strcpy(db_recs.bch10.mcard_banknet,auth_tx->TLF01_details.visa_tran_id);
   }
   strcpy (db_recs.bch10.cash_bonus, auth_tx->TLF01_details.cash_bonus);
   strcpy (db_recs.bch10.ticket_nbr, auth_tx->TLF01_details.ticket_nbr);
   strcpy (db_recs.bch10.odometer, auth_tx->TLF01_details.odometer);
   strcpy( db_recs.bch10.product_codes[19].code, auth_tx->TLF01_details.settlement_date );
   strcpy( db_recs.bch10.product_codes[6].amount, auth_tx->TLF01_details.pos_condition_code);/*Girija GB*/
   if(0 == strlen(db_recs.bch10.product_codes[6].amount))
   {
	  strncpy(db_recs.bch10.product_codes[6].amount,
				  "  ", (sizeof(auth_tx->TLF01_details.pos_condition_code)-1));
   }
   strcat( db_recs.bch10.product_codes[6].amount, auth_tx->TLF01_details.service_code );

   /* Store Stand in flag in BCH10 also, This will be useful for VOID/Reversal/Adjustment transactions*/
   strcpy( db_recs.bch10.product_codes[1].code, auth_tx->TLF01_details.product_codes[1].code);/*TF Phani*/
   if((auth_tx->TLF01_details.tx_key== AUTH_VOID_SALE_RESPONSE ) &&
	  (strcmp(auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0))
   {
	   strcpy( db_recs.bch10.product_codes[1].code, VOIDED);
   }
	/* TF Phani-  Copy DE63 table Id 12- value */
	strcpy( db_recs.bch10.product_codes[14].amount, auth_tx->TLF01_details.product_codes[14].amount);/*TF PHani*/
	strncpy( db_recs.bch10.product_codes[19].code, auth_tx->TLF01_details.settlement_date, (sizeof(auth_tx->TLF01_details.settlement_date)-1) );
	strcpy( db_recs.bch10.def_gross_amt, auth_tx->TLF01_details.def_gross_amt );
	strcpy( db_recs.bch10.term, auth_tx->TLF01_details.deferred_term_length );

	/*Purchase ID Type and Value change*/
	memcpy( db_recs.bch10.product_codes[17].code,
			auth_tx->TLF01_details.product_codes[17].code,
			PURCHASE_ID_TYPE_LEN);
	memcpy( db_recs.bch10.product_codes[17].amount,
			auth_tx->TLF01_details.product_codes[17].amount,
			12);

	memcpy( db_recs.bch10.product_codes[18].amount,
			auth_tx->TLF01_details.product_codes[18].amount,
			12);
	memcpy( db_recs.bch10.product_codes[18].code,
			auth_tx->TLF01_details.product_codes[18].code,
			1);
   return;
}

BYTE process_sale_trans(pAUTH_TX auth_tx)
{
   CHAR   primary_key[50] 	 = {0};
   CHAR   date_time[16] 	 = {0};
   CHAR   bch01_open_date[9] = {0};
   CHAR   bch01_open_time[7] = {0};
   LONG   total 			 = 0;
   INT    cnt   			 = 0;
   BYTE   auth_number[7]  	 = {0};
   BYTE   rrn[13]            = {0};
   TLF01  tlf01;
   CHAR   temp_service_code[4]={0};
   double start_time = 0;
   double end_time   = 0;
   double duration   = 0;
   CHAR   buffer[526]= {0};
   CHAR   MCF03P_primary_key[24] = {0};
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_aprvd_amount = 0;
   LONG   total_aprvd_transcn_count =0;

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();
   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Update Auth_Tx with original's auth number and RRN.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
     /* memcpy( auth_tx->TLF01_details.auth_number, auth_number, 6 );
      memcpy( auth_tx->TLF01_details.retrieval_ref_num,  rrn, 12 );

      rollback_the_update();
      return PTEMSG_OK;
   } End of comments*/
	strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
	strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
	strcpy(primary_key + BCH01_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);

	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}

	result_code = db_select_bch01(primary_key,&db_recs.bch01,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, BCH01_DATA,
						  (float)duration, &TimingStats );
	}

	/*create and Insert into BCH01, delete any ACTIVE trans. for this terminal */
	if (result_code == PTEMSG_NOT_FOUND)
	{
		/*Delete BCH10 if any ACTIVE */
		memset(&db_recs.bch10, 0, sizeof(BCH10));
		strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
		strcpy(db_recs.bch10.merchant_id , auth_tx->TLF01_details.merchant_id);

		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_delete_active_bch10(&db_recs.bch10,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_DELETE, 2, BCH10_DATA,
								 (float)duration, &TimingStats );
		}

		memcpy(&db_recs.bch01,primary_key, BCH01_PRIMARY_KEY_SIZE);

		/*check if CREDIT sale or DEBIT sale */
		if (auth_tx->bin_type =='D')
		{
			strcpy(db_recs.bch01.total_debit_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01.total_debit_amt) != 0)
			{
				strcpy(db_recs.bch01.total_debit_cnt, "0001");
			}
		}
		else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		{
			strcpy(db_recs.bch01.total_credit_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01.total_credit_amt) != 0)
			{
				strcpy(db_recs.bch01.total_credit_cnt, "0001");
			}
		}

		format_date(date_time);
		if (strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
		{
			strcpy(db_recs.bch01.open_date, auth_tx->TLF01_details.date_yyyymmdd);
		}
		else
		{
			memset(db_recs.bch01.open_date, 0, sizeof(db_recs.bch01.open_date));
			strncpy(db_recs.bch01.open_date, date_time, 8);
		}

		if (strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
		{
			strcpy(db_recs.bch01.open_time, auth_tx->TLF01_details.time_hhmmss);
		}
		else
		{
			memset(db_recs.bch01.open_time, 0, sizeof(db_recs.bch01.open_time));
			strncpy(db_recs.bch01.open_time, date_time + 8, 6);
		}

		strcpy(bch01_open_time,db_recs.bch01.open_time);
		strcpy(bch01_open_date,db_recs.bch01.open_date);
		strcpy(db_recs.bch01.batch_status,"A");
		strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);

		if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
		{
			strcpy(db_recs.bch01.visa_merchant_id,"GES");
		}
		else
		{
			strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
		}

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_INSERT);
		}

		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_insert_bch01(&db_recs.bch01,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_INSERT, 2, BCH01_DATA,
								 (float)duration, &TimingStats );
		}

		if (result_code != PTEMSG_OK)
		{
			/* NewLog, new batch creation, bch01 failed*/
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"New batch creation, bch01 insert failed, device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans");
			return(PTEMSG_INSERT_FAILED);
		}
		updates_to_db_made = true;
	}

	/* bch01 record is found */
	else if (result_code == PTEMSG_OK)
	{
		 if (auth_tx->bin_type == 'D')
		 {
			total = atol(db_recs.bch01.total_debit_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
			if (atol(db_recs.bch01.total_debit_amt) != 0L)
			{
				cnt = atoi(db_recs.bch01.total_debit_cnt) + 1;
			}
			sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);
		 }
		 else if ( (auth_tx->bin_type=='C') || (auth_tx->bin_type=='E') )
		 {
			total = atol(db_recs.bch01.total_credit_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
			if (atol(db_recs.bch01.total_credit_amt) != 0L)
			{
				cnt = atoi(db_recs.bch01.total_credit_cnt) + 1;
			}
			sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);
		 }

		 /*copy date and time values into temporary variables */
		 strcpy(bch01_open_time,db_recs.bch01.open_time);
		 strcpy(bch01_open_date,db_recs.bch01.open_date);


		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
		  }
		 if ( DB_Timing_Flag == true )
		 {
				start_time = ptetime_get_time();
		 }

		 if (result_code = db_update_bch01(&db_recs.bch01,err_msg) != PTEMSG_OK)
		 {
			 /* NewLog, bch01 is their for the batch, but update failed.
			  * */
			 memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH01 available for the batch and update failed, device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans");
			return(PTEMSG_UPDATE_FAILED);
		 }
		 if ( DB_Timing_Flag == true )
		 {
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH01_DATA,
								 (float)duration, &TimingStats );
		 }
		 updates_to_db_made = true;
	}
	else
	{
		/* NewLog, bch01 selection failed for new sale txn processing
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Checking for batch details, bch01 selection failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_sale_trans");
		return(PTEMSG_UPDATE_FAILED);
	}
	/*For OFFLINE SALE transactions update EMV RAW DATA Transaction ID*/
	if (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
	{
		memset(&db_recs.bch20, 0, sizeof(BCH20));
		strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
		strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
		strcpy(db_recs.bch20.auth_number, auth_tx->TLF01_details.auth_number);
		strcpy(db_recs.bch20.card_nbr, auth_tx->TLF01_details.card_num);

		/*memcpy(db_recs.bch20.product_codes[1].amount, auth_tx->TLF01_details.product_codes[1].amount,10);
		memcpy(db_recs.bch20.product_codes[5].quantity, auth_tx->TLF01_details.product_codes[5].quantity, 2);
		memcpy(db_recs.bch20.product_codes[7].code, auth_tx->TLF01_details.product_codes[7].code,3);
		memcpy(db_recs.bch20.down_payment, auth_tx->TLF01_details.add_amounts, 9);
		memcpy(db_recs.bch20.interest_rate, auth_tx->TLF01_details.visa_validation_code, 4);
		memcpy(db_recs.bch20.term, auth_tx->TLF01_details.currency_code, 3);*/

		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		db_update_emv_raw_by_bch20_auth_number(&db_recs.bch20,auth_tx->TLF01_details.primary_key.transaction_id,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH20_DATA,
								 (float)duration, &TimingStats );
			strncpy(temp_service_code,db_recs.bch20.product_codes[6].amount+2,3);
			if(strlen(temp_service_code)>0)
			{
				strncpy(auth_tx->TLF01_details.service_code,db_recs.bch20.product_codes[6].amount+2,3);
			}
			/*if(strlen(db_recs.bch20.pos_entry_mode) > 0)
			{
				strncpy(auth_tx->TLF01_details.pos_entry_mode,db_recs.bch20.pos_entry_mode,4);
			}*/
			if(strlen(db_recs.bch20.mcard_banknet ) > 0)
			{
				strncpy(auth_tx->TLF01_details.mcard_banknet,db_recs.bch20.mcard_banknet,
						(sizeof(auth_tx->TLF01_details.mcard_banknet)-1));
			}

		}
		/* Lets update the flag to indicate that this preauth is already processed.*/
		memset(&db_recs.bch20, 0, sizeof(BCH20));
		strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
		strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
		strcpy(db_recs.bch20.card_nbr, auth_tx->TLF01_details.card_num);
		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinBCH20 (&db_recs.bch20,MODE_UPDATE);
		}
		/* We will not call this function MC 512 */
		if(false == updatds_check_If_Tra_qualify_MC_512_incremental_Preauth(auth_tx))
		{
			db_update_bch20_preauth_flag_by_card_num(&db_recs.bch20,err_msg);

		}
	}
	memset(&db_recs.bch10, 0, sizeof(BCH10));
	strcpy(db_recs.bch10.open_date,bch01_open_date);
	strcpy(db_recs.bch10.open_time,bch01_open_time);
	strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);
	strcpy(db_recs.bch10.primary_key.batch_nbr,  "ACTIVE");
	db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
	prepare_bch10_for_update(auth_tx);

	if ((true == updatds_Check_If_AMEX_Transaction (auth_tx) ||
	     true == updatds_Check_If_Diners_Transaction (auth_tx)) &&
		 (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE))
	{
		strncpy(auth_tx->TLF01_details.pos_entry_mode, auth_tx->TLF01_details.product_codes[14].code, 4);
	}

	/*copy procd to bch10 only when offline sale is present for original pre-auth for ony CUP transactions.*/
	if( 0==strncmp(db_recs.bch10.message_type,"0220",4)&&
			0==strncmp(db_recs.bch10.processing_code,"000000",4)&&
			0 == strncmp(db_recs.bch10.product_code,CUP_CARD_INDICATOR_FLAG,2) ||
			0 == strncmp(auth_tx->TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		strcpy(db_recs.bch10.product_codes[4].code, "procsd");
	}
	/* Convert message type to a request type, for standardization. */
	convert_msgtype_to_request( db_recs.bch10.message_type );
	if (DB_NullCheck_Flag==1)
	{
	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
	}
	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}

	if ((result_code = db_insert_bch10(&db_recs.bch10,err_msg)) != PTEMSG_OK)
	{
		/* NewLog, bch10 insert failed while processing the offline sale txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Bacth(bch01) details updated but BCH10 insert failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_sale_trans");
		return(PTEMSG_INSERT_FAILED);
	}

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_INSERT, 2, BCH10_DATA,
						  (float)duration, &TimingStats );
	}

	updates_to_db_made = true;
	if (auth_tx->TLF01_details.tx_key == AUTH_SALES_COMPLETION_RESPONSE)
	{
		memset(&db_recs.bch20, 0, sizeof(BCH20));
		strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
		strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
		strcpy(db_recs.bch20.sys_trace_audit_num, auth_tx->TLF01_details.sys_trace_audit_num);

		if (DB_NullCheck_Flag==1)
		{
		  CheckNULLTerminatedinBCH20 (&db_recs.bch20,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		db_update_bch20_by_stan(&db_recs.bch20, err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH20_DATA,
								 (float)duration, &TimingStats );
		}
	}
	/*If Tx is VISA RPS tx, Store in RPS01 , Later Tx need to be verified and TID should be used
	* TF Phani*/
	if(auth_tx->TLF01_details.card_num[0] == '4' &&
		  auth_tx->MCF01_details.rps_merchant[0]== 'Y')
	{
		memset(&db_recs.rps01, 0, sizeof(RPS01));

		strcpy(db_recs.rps01.merchant_id, auth_tx->TLF01_details.merchant_id);
		strcpy(db_recs.rps01.transaction_id, auth_tx->TLF01_details.primary_key.transaction_id);
		strcpy(db_recs.rps01.card_num, auth_tx->TLF01_details.card_num);
		strcpy(db_recs.rps01.date_yyyymmdd, auth_tx->TLF01_details.date_yyyymmdd);
		strcpy(db_recs.rps01.visa_tran_id, auth_tx->TLF01_details.visa_tran_id);
		strcpy(db_recs.rps01.currency_code, auth_tx->TLF01_details.currency_code);
		strcpy(db_recs.rps01.total_amount, auth_tx->TLF01_details.total_amount);
		create_system_date( db_recs.rps01.system_date );
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}
	
		db_insert_RPS01(&db_recs.rps01, err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_INSERT, 2, RPS01_DATA,
							   (float)duration, &TimingStats );
		}
	}

	/******************MID Cappinng code change *********
	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
	if(auth_tx->TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx->TLF01_details.system_date );
	}
	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
							  (float)duration, &TimingStats );
		}


		if (result_code == PTEMSG_NOT_FOUND)
		{
			memset(&db_recs.mcf03p, 0, sizeof(MCF03P));
			memcpy(&db_recs.mcf03p.merch_id, auth_tx->TLF01_details.merchant_id,15);
			memcpy(&db_recs.mcf03p.system_date,auth_tx->TLF01_details.system_date,8);
			memcpy(&db_recs.mcf03p.total_amount,auth_tx->TLF01_details.total_amount,12);
			memcpy(&db_recs.mcf03p.total_transcn_cnt,"000000000001",12);
			memcpy(&db_recs.mcf03p.total_aprvd_amount,auth_tx->TLF01_details.total_amount,12);
			memcpy(&db_recs.mcf03p.total_reversed_amount,"000000000000",12);
			memcpy(&db_recs.mcf03p.total_aprvd_transcn_count, "000000000001",12);
			memcpy(&db_recs.mcf03p.total_reversed_transcn_count, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_1, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_2, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_3, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_4, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_5, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_6, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_7, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_8, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_9, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_10, "000000000000", 12);

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_INSERT);
			}

			if ( DB_Timing_Flag == true )
			{
				start_time = ptetime_get_time();
			}

			result_code = db_insert_mcf03p(&db_recs.mcf03p,err_msg);
			if ( DB_Timing_Flag == true )
			{
				end_time = ptetime_get_time();
				duration = end_time - start_time;
				update_timing_stats( ST1_DB_INSERT, 2, MCF03P_DATA,
									 (float)duration, &TimingStats );
			}

			if (result_code != PTEMSG_OK)
			{

				memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"New Record  for mcf03p insert failed, card num: %s, device id: %s"
						"merch id: %s,stan: %s",auth_tx->TLF01_details.card_num,
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_sale_trans_mp");
				return(PTEMSG_INSERT_FAILED);
			 }

			 updates_to_db_made = true;
		}
		else if (result_code == PTEMSG_OK)// MCF03P record is found
		{
			total_amount = atol(db_recs.mcf03p.total_amount)+
						   atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

			total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
			sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

			total_aprvd_amount = atol(db_recs.mcf03p.total_aprvd_amount)+
								 atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.mcf03p.total_aprvd_amount, "%012d",total_aprvd_amount);

			total_aprvd_transcn_count= atol(db_recs.mcf03p.total_aprvd_transcn_count)+ 1;
			sprintf(db_recs.mcf03p.total_aprvd_transcn_count,"%012d",total_aprvd_transcn_count);

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
			}
			if ( DB_Timing_Flag == true )
			{
				start_time = ptetime_get_time();
			}

			if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
			{

				 memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
						"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_sale_trans_mp");
				return(PTEMSG_UPDATE_FAILED);
			}
			if ( DB_Timing_Flag == true )
			{
				end_time = ptetime_get_time();
				duration = end_time - start_time;
				update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
									 (float)duration, &TimingStats );
			}
			updates_to_db_made = true;
		}
		else
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans");
			return(PTEMSG_UPDATE_FAILED);
		}

	******************MID Capping code change End *********/
	return (PTEMSG_OK);
}

BYTE process_sale_trans_mp(pAUTH_TX auth_tx)
{
   CHAR   primary_key[50] 		= "";
   CHAR   MCF03P_primary_key[24] = {0};
   CHAR   date_time[16] 		= "";
   CHAR   bch01_mp_open_date[9] = "";
   CHAR   bch01_mp_open_time[7] = "";
   LONG   total 			= 0;
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_aprvd_amount = 0;
   LONG   total_aprvd_transcn_count =0;
   INT    cnt 				= 0;
   BYTE   auth_number[7]  	= "";
   BYTE   rrn[13]         	= "";
   TLF01  tlf01;
   double         start_time = 0;
   double         end_time 	 = 0;
   double         duration	 = 0;
   CHAR   buffer[526]= {0};

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Update Auth_Tx with original's auth number and RRN.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
   /* memcpy( auth_tx->TLF01_details.auth_number, auth_number, 6 );
      memcpy( auth_tx->TLF01_details.retrieval_ref_num,  rrn, 12 );

      rollback_the_update();
      return PTEMSG_OK;
   } end of comments*/

	strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
	strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
	strcpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);

	if ( DB_Timing_Flag == true )
	{
		 start_time = ptetime_get_time();
	}

	result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, BCH01_MP_DATA,
						  (float)duration, &TimingStats );
	}

	//create and Insert into BCH01, delete any ACTIVE trans. for this terminal
	if (result_code == PTEMSG_NOT_FOUND)
	{
		 //Delete BCH10 if any ACTIVE
			memset(&db_recs.bch10, 0, sizeof(BCH10));
			strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
			strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
			strcpy(db_recs.bch10.merchant_id , auth_tx->TLF01_details.merchant_id);

		 if ( DB_Timing_Flag == true )
		 {
			 start_time = ptetime_get_time();
		 }

		 result_code = db_delete_active_bch10(&db_recs.bch10,err_msg);
		 if ( DB_Timing_Flag == true )
		 {
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_DELETE, 2, BCH10_DATA,
								 (float)duration, &TimingStats );
		 }

		 memcpy(&db_recs.bch01_mp,primary_key, BCH01_MP_PRIMARY_KEY_SIZE);

		 //check if CREDIT sale or DEBIT sale
		 if (auth_tx->bin_type =='D')
		 {
			strcpy(db_recs.bch01_mp.total_debit_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01_mp.total_debit_amt) != 0)
			{
				strcpy(db_recs.bch01_mp.total_debit_cnt, "0001");
			}
		 }
		 else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		 {
			strcpy(db_recs.bch01_mp.total_credit_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01_mp.total_credit_amt) != 0)
			{
				strcpy(db_recs.bch01_mp.total_credit_cnt, "0001");
			}
		 }

		 format_date(date_time);
		 if (strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
		 {
			 strcpy(db_recs.bch01_mp.open_date, auth_tx->TLF01_details.date_yyyymmdd);
		 }
		 else
		 {
			memset(db_recs.bch01_mp.open_date, 0, sizeof(db_recs.bch01_mp.open_date));
			strncpy(db_recs.bch01_mp.open_date, date_time, 8);
		 }

		 if (strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
		 {
			 strcpy(db_recs.bch01_mp.open_time, auth_tx->TLF01_details.time_hhmmss);
		 }
		 else
		 {
			memset(db_recs.bch01_mp.open_time, 0, sizeof(db_recs.bch01_mp.open_time));
			strncpy(db_recs.bch01_mp.open_time, date_time + 8, 6);
		 }

		 strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time);
		 strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date);
		 strcpy(db_recs.bch01_mp.batch_status,"A");
		 strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);

		 if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
		 {
			 strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
		 }
		 else
		 {
			 strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
		 }

		 if (DB_NullCheck_Flag==1)
		 {
			 CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_INSERT);
		 }

		 if ( DB_Timing_Flag == true )
		 {
			 start_time = ptetime_get_time();
		 }

		 result_code = db_insert_bch01_mp(&db_recs.bch01_mp,err_msg);
		 if ( DB_Timing_Flag == true )
		 {
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_INSERT, 2, BCH01_MP_DATA,
								 (float)duration, &TimingStats );
		 }

		 if (result_code != PTEMSG_OK)
		 {
			 /* NewLog, new batch created, bch01 insert failed.*/
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"New batch creation, bch01 insert failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans_mp");
			return(PTEMSG_INSERT_FAILED);
		 }

		 updates_to_db_made = true;
	}

	// bch01_mp record is found
	else if (result_code == PTEMSG_OK)
	{
		if (auth_tx->bin_type == 'D')
		{
			total = atol(db_recs.bch01_mp.total_debit_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
			if (atol(db_recs.bch01_mp.total_debit_amt) != 0L)
			   cnt = atoi(db_recs.bch01_mp.total_debit_cnt) + 1;
			sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt);
		}
		else if ( (auth_tx->bin_type=='C') || (auth_tx->bin_type=='E') )
		{
			total = atol(db_recs.bch01_mp.total_credit_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);
			if (atol(db_recs.bch01_mp.total_credit_amt) != 0L)
			   cnt = atoi(db_recs.bch01_mp.total_credit_cnt) + 1;
			sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt);
		}

		//copy date and time values into temporary variables
		strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time);
		strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if (result_code = db_update_bch01_mp(&db_recs.bch01_mp,err_msg) != PTEMSG_OK)
		{
		 /* NewLog, bch01 is their for the batch, but update failed.
		  * */
			 memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH01 available for the batch and update failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH01_MP_DATA,
								 (float)duration, &TimingStats );
		}
		updates_to_db_made = true;
	}
	else
	{
		/* NewLog, bch01 selection failed for new sale txn processing
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Checking for batch details, bch01 selection failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_sale_trans");
		return(PTEMSG_UPDATE_FAILED);
	}
	/*For OFFLINE SALE transactions update EMV RAW DATA Transaction ID*/
	if (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
	{
		memset(&db_recs.bch20, 0, sizeof(BCH20));
		strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
		strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
		strcpy(db_recs.bch20.auth_number, auth_tx->TLF01_details.auth_number);
		strcpy(db_recs.bch20.card_nbr, auth_tx->TLF01_details.card_num);


		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		db_update_emv_raw_by_bch20_auth_number(&db_recs.bch20,auth_tx->TLF01_details.primary_key.transaction_id,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH20_DATA,
								 (float)duration, &TimingStats );
			strncpy(auth_tx->TLF01_details.service_code,db_recs.bch20.product_codes[6].amount,3);
		}
	}
	memset(&db_recs.bch10, 0, sizeof(BCH10));
	strcpy(db_recs.bch10.open_date,bch01_mp_open_date);
	strcpy(db_recs.bch10.open_time,bch01_mp_open_time);
	strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);
	strcpy(db_recs.bch10.primary_key.batch_nbr,  "ACTIVE");
	db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
	//Store Monthly and factor details in bch10
	strcat( db_recs.bch10.product_codes[15].amount, auth_tx->TLF01_details.monthly_amort_amt);/*Store Monthly Amount*/
	strcat( db_recs.bch10.product_codes[16].amount, auth_tx->TLF01_details.deferred_factor );/*Store factor rate*/
	prepare_bch10_for_update_MP(auth_tx);

	/* Convert message type to a request type, for standardization. */
	convert_msgtype_to_request( db_recs.bch10.message_type );
	if (DB_NullCheck_Flag==1)
	{
	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
	}
	if ( DB_Timing_Flag == true )
	{
		 start_time = ptetime_get_time();
	}

	if ((result_code = db_insert_bch10_MP(&db_recs.bch10,err_msg)) != PTEMSG_OK)
	{
		/* NewLog, bch10 insert failed while processing the offline sale txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Bacth(bch01) details updated but BCH10 insert failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_sale_trans_mp");
		return(PTEMSG_INSERT_FAILED);
	}

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_INSERT, 2, BCH10_DATA,
						  (float)duration, &TimingStats );
	}

	updates_to_db_made = true;
	if (auth_tx->TLF01_details.tx_key == AUTH_SALES_COMPLETION_RESPONSE)
	{
		memset(&db_recs.bch20, 0, sizeof(BCH20));
		strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
		strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
		strcpy(db_recs.bch20.sys_trace_audit_num, auth_tx->TLF01_details.sys_trace_audit_num);

		if (DB_NullCheck_Flag==1)
		{
		  CheckNULLTerminatedinBCH20 (&db_recs.bch20,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		db_update_bch20_by_stan(&db_recs.bch20, err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH20_DATA,
								 (float)duration, &TimingStats );
		}
	}
	/******************MID Cappinng code change *********
	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
	if(auth_tx->TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx->TLF01_details.system_date );
	}
	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
							  (float)duration, &TimingStats );
		}

		//create and Insert into MCF03P for this merchant
		if (result_code == PTEMSG_NOT_FOUND)
		{
			memset(&db_recs.mcf03p, 0, sizeof(MCF03P));
			memcpy(&db_recs.mcf03p.merch_id, auth_tx->TLF01_details.merchant_id,15);
			memcpy(&db_recs.mcf03p.system_date,auth_tx->TLF01_details.system_date,8);
			memcpy(&db_recs.mcf03p.total_amount,auth_tx->TLF01_details.total_amount,12);
			memcpy(&db_recs.mcf03p.total_transcn_cnt,"000000000001",12);
			memcpy(&db_recs.mcf03p.total_aprvd_amount,auth_tx->TLF01_details.total_amount,12);
			memcpy(&db_recs.mcf03p.total_reversed_amount,"000000000000",12);
			memcpy(&db_recs.mcf03p.total_aprvd_transcn_count, "000000000001",12);
			memcpy(&db_recs.mcf03p.total_reversed_transcn_count, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_1, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_2, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_3, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_4, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_5, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_6, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_7, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_8, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_9, "000000000000", 12);
			memcpy(&db_recs.mcf03p.field_10, "000000000000", 12);

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_INSERT);
			}

			if ( DB_Timing_Flag == true )
			{
				start_time = ptetime_get_time();
			}

			result_code = db_insert_mcf03p(&db_recs.mcf03p,err_msg);
			if ( DB_Timing_Flag == true )
			{
				end_time = ptetime_get_time();
				duration = end_time - start_time;
				update_timing_stats( ST1_DB_INSERT, 2, MCF03P_DATA,
									 (float)duration, &TimingStats );
			}

			if (result_code != PTEMSG_OK)
			{

				memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"New Record  for mcf03p insert failed, card num: %s, device id: %s"
						"merch id: %s,stan: %s",auth_tx->TLF01_details.card_num,
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_sale_trans_mp");
				return(PTEMSG_INSERT_FAILED);
			 }

			 updates_to_db_made = true;
		}
		else if (result_code == PTEMSG_OK)// MCF03P record is found
		{
			total_amount = atol(db_recs.mcf03p.total_amount)+
						   atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

			total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
			sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

			total_aprvd_amount = atol(db_recs.mcf03p.total_aprvd_amount)+
								 atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.mcf03p.total_aprvd_amount, "%012d",total_aprvd_amount);

			total_aprvd_transcn_count= atol(db_recs.mcf03p.total_aprvd_transcn_count)+ 1;
			sprintf(db_recs.mcf03p.total_aprvd_transcn_count,"%012d",total_aprvd_transcn_count);

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
			}
			if ( DB_Timing_Flag == true )
			{
				start_time = ptetime_get_time();
			}

			if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
			{

				 memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
						"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_sale_trans_mp");
				return(PTEMSG_UPDATE_FAILED);
			}
			if ( DB_Timing_Flag == true )
			{
				end_time = ptetime_get_time();
				duration = end_time - start_time;
				update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
									 (float)duration, &TimingStats );
			}
			updates_to_db_made = true;
		}
		else
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans");
			return(PTEMSG_UPDATE_FAILED);
		}

	/******************MID Capping code change End *********/

	return PTEMSG_OK;
}


BYTE process_auth_trans(pAUTH_TX auth_tx)
{
   BYTE   retval 		  = PTEMSG_OK;
   CHAR   primary_key[50] = {0};
   CHAR   date_time[16]   = {0};
   BYTE   auth_number[7]  = {0};
   BYTE   rrn[13]         = {0};
   INT    visa_trn_id     = {0};
   INT    visa_trn_id_len = 0;
   INT 	  DCI_NRID_len 	  = 0;
   INT    Max_DCI_NRID_len= 15;
   TLF01  tlf01;
   double start_time 	  = 0;
   double end_time 	 	  = 0;
   double duration 	      = 0;
   CHAR   buffer[526]	  = {0};

   CHAR   MCF03P_primary_key[24] = {0};
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_aprvd_amount = 0;
   LONG   total_aprvd_transcn_count =0;

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }
   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Update Auth_Tx with original's auth number and RRN.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*memcpy( auth_tx->TLF01_details.auth_number, auth_number, 6 );
      memcpy( auth_tx->TLF01_details.retrieval_ref_num,  rrn, 12 );

      rollback_the_update();
   } comment ends*/

	memset(&db_recs.bch20, 0, sizeof(BCH20));
	strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
	strcpy(db_recs.bch20.primary_key.retrieval_ref_num, auth_tx->TLF01_details.retrieval_ref_num);

	format_date(date_time);

	strncpy(db_recs.bch20.open_date, date_time, 8);
	strncpy(db_recs.bch20.open_time,  date_time + 8, 6);

	strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
	strcpy(db_recs.bch20.transaction_id, auth_tx->TLF01_details.primary_key.transaction_id);
	strcpy(db_recs.bch20.tran_date, auth_tx->TLF01_details.date_yyyymmdd);
	strcpy(db_recs.bch20.tran_time, auth_tx->TLF01_details.time_hhmmss);
	strcpy(db_recs.bch20.tran_amount, auth_tx->TLF01_details.total_amount);
	strcpy(db_recs.bch20.processing_code, auth_tx->TLF01_details.processing_code);
	strcpy(db_recs.bch20.response_code, auth_tx->TLF01_details.response_code);
	strcpy(db_recs.bch20.auth_number, auth_tx->TLF01_details.auth_number);
	strcpy(db_recs.bch20.message_type, auth_tx->TLF01_details.message_type);
	strcpy(db_recs.bch20.card_nbr, auth_tx->TLF01_details.card_num);
	strcpy(db_recs.bch20.invoice_nbr, auth_tx->TLF01_details.invoice_number);
	strcpy(db_recs.bch20.settle_file_prefix, auth_tx->BIN01_details.settle_file_prefix);
	strcpy(db_recs.bch20.product_code, auth_tx->TLF01_details.product_code);
	strcpy(db_recs.bch20.sys_trace_audit_num, auth_tx->TLF01_details.sys_trace_audit_num);
	strcpy(db_recs.bch20.ticket_nbr, auth_tx->TLF01_details.ticket_nbr);
	/* TF Phani-  Copy DE63 table Id 12- value */
	strcpy( db_recs.bch20.product_codes[14].amount, auth_tx->TLF01_details.product_codes[14].amount);/*TF PHani*/
	strcpy(db_recs.bch20.odometer, auth_tx->TLF01_details.odometer);
	db_recs.bch20.bin_type = auth_tx->bin_type;
	db_recs.bch20.tx_key=auth_tx->TLF01_details.tx_key;
	strcpy(db_recs.bch20.pos_entry_mode, auth_tx->TLF01_details.pos_entry_mode);
	strcpy(db_recs.bch20.profile, auth_tx->TLF01_details.profile);
	strcpy(db_recs.bch20.exp_date, auth_tx->TLF01_details.exp_date);
	strcpy(db_recs.bch20.transmission_timestamp, auth_tx->TLF01_details.transmission_timestamp);
	strcpy(db_recs.bch20.mcard_banknet, auth_tx->TLF01_details.mcard_banknet);
	strcpy(db_recs.bch20.processed, "N");
	strcpy( db_recs.bch20.def_gross_amt, auth_tx->TLF01_details.def_gross_amt );
	strcpy( db_recs.bch20.term, auth_tx->TLF01_details.deferred_term_length );
	strcpy( db_recs.bch20.product_codes[1].code, auth_tx->TLF01_details.product_codes[1].code);/*TF Ajay*/

	strncpy(db_recs.bch20.product_codes[1].amount, auth_tx->TLF01_details.product_codes[1].amount,10);
	strncpy(db_recs.bch20.product_codes[5].quantity, auth_tx->TLF01_details.product_codes[5].quantity, 2);
	strncpy(db_recs.bch20.product_codes[8].code, auth_tx->TLF01_details.product_codes[7].code,3);
	strncpy(db_recs.bch20.down_payment, auth_tx->TLF01_details.add_amounts, 9);
	strncpy(db_recs.bch20.interest_rate, auth_tx->TLF01_details.visa_validation_code, 4);
	strncpy(db_recs.bch20.term, auth_tx->TLF01_details.currency_code, 3);

	if(IsFieldNull(db_recs.bch20.mcard_banknet)) /* To store AMEX tran ID -DE31*/
	{
		strcpy(db_recs.bch20.mcard_banknet,auth_tx->TLF01_details.visa_tran_id);
	}
	if(0 == strlen(auth_tx->TLF01_details.pos_condition_code))
	{
	  strncpy(db_recs.bch20.product_codes[6].amount,
				  "  ", (sizeof(auth_tx->TLF01_details.pos_condition_code)-1));
	}
	else
	{
		strncpy(db_recs.bch20.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code,2);
	}
	strcat( db_recs.bch20.product_codes[6].amount, auth_tx->TLF01_details.service_code );


	/* Convert message type to a request type, for standardization. */
	convert_msgtype_to_request( db_recs.bch20.message_type );

	strcpy( db_recs.bch20.product_codes[16].amount, auth_tx->TLF01_details.product_codes[16].amount );

	// Store settlement date
	strncpy( db_recs.bch20.product_codes[19].code, auth_tx->TLF01_details.settlement_date, (sizeof(auth_tx->TLF01_details.settlement_date)-1) );

	printf("Transmission timestamp for STAN %s is %s in process_auth_trans\n",auth_tx->TLF01_details.sys_trace_audit_num,
															auth_tx->TLF01_details.transmission_timestamp);
	if(true == updatds_Check_If_VISA_Transaction(auth_tx) )
	{
		visa_trn_id_len = strlen(auth_tx->TLF01_details.visa_tran_id);
		if(visa_trn_id_len > 0)
		{
			if(visa_trn_id_len > 6)
			{
				memcpy(	db_recs.bch20.product_codes[13].code,
						auth_tx->TLF01_details.visa_tran_id,6);
				memcpy(	db_recs.bch20.product_codes[13].amount  ,
						auth_tx->TLF01_details.visa_tran_id + 6,
						(visa_trn_id_len - 6));
			}
			else
			{
				memcpy(	db_recs.bch20.product_codes[13].code,
						auth_tx->TLF01_details.visa_tran_id,
						visa_trn_id_len);
			}

		}

		db_recs.bch20.product_codes[0].code[0] = auth_tx->TLF01_details.resp_source[0];
		db_recs.bch20.product_codes[0].code[1] = '\0';

	}
	if(true ==updatds_Check_If_Diners_Transaction(auth_tx) )
	{
		DCI_NRID_len = strlen(auth_tx->TLF01_details.resp_source);
		if(DCI_NRID_len > 0)
		{
			if(DCI_NRID_len > 6)
			{
				memcpy(	db_recs.bch20.product_codes[13].code,
						auth_tx->TLF01_details.resp_source,6);
				memcpy(	db_recs.bch20.product_codes[13].amount  ,
						auth_tx->TLF01_details.resp_source + 6,
						(Max_DCI_NRID_len - 6));
			}
			else
			{
				memcpy(	db_recs.bch20.product_codes[13].code,
						auth_tx->TLF01_details.resp_source,
						DCI_NRID_len);
			}

		}
		memcpy( db_recs.bch20.product_codes[12].amount ,
				auth_tx->TLF01_details.product_codes[12].amount,
				DINERS_DE12_SIZE);
		if(strlen(auth_tx->host2_message_seq_nbr)> 0)
		{
			memcpy( db_recs.bch20.product_codes[11].code,
					auth_tx->host2_message_seq_nbr + 1,
					PAN_SEQ_NUM);
		}
	}
#ifdef MC02_APR2024
	if(true == updatds_Check_If_MC_Transaction(auth_tx) &&
	   (auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
	    auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE ||
		auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION ||
		auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE) &&
	   strlen(auth_tx->TLF01_details.product_codes[10].amount) > 0)
	{
		memcpy(db_recs.bch20.product_codes[10].amount, auth_tx->TLF01_details.product_codes[10].amount, 10);
		memcpy(db_recs.bch20.product_codes[1].amount,  auth_tx->TLF01_details.product_codes[1].amount, 12);
	}
#endif
	/* Let us store if its ON US card */
	if ((0 == strncmp(auth_tx->CAF01_details.authorization_location,"S",1)) &&
	((0 == strncmp(auth_tx->CAF01_details.destination_name,"ncmcr2",6)) ||
	 (0 == strncmp(auth_tx->CAF01_details.destination_name,"nccup2",6)) ||
	 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncjcb2",6)) ||
	 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncdci2",6)) ||
	 (0 == strncmp(auth_tx->CAF01_details.destination_name,"nposa2",6)) ||
	 (0 == strncmp(auth_tx->CAF01_details.destination_name,"ncvis2",6))))
	{
	/* Its ON US tranasction */
		memcpy( db_recs.bch20.product_codes[5].code,"ONUS",4);
	}
	else
	{
	/* Its OFF US tranasction */
		memcpy( db_recs.bch20.product_codes[5].code,"OFFUS",5);
	}

#if BDOR_62_PREAUTH_VOID


		if(auth_tx->TLF01_details.saf[0]== INCREMENTAL_AUTH)
		{
			memcpy( db_recs.bch20.product_codes[7].code,
					INCREMENTAL_PREAUTH_TRA,
					PREAUTH_TRA_LEN);
			memcpy( db_recs.bch20.product_codes[4].quantity,
					INITIAL_PREAUTH_TRA_WITH_INCREMENTAL,
					PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG);

			if(0 == strncmp(auth_tx->TLF01_details.product_codes[3].quantity,
							INITIAL_PREAUTH_TRA_WITH_INCREMENTAL,
							PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG))
			{
				/* Its Preauth with Incremental trx No need to update the
				 * Original BCH20 details as its already have data updated*/
			}
			else if(0 == strncmp(auth_tx->TLF01_details.product_codes[3].quantity,
								INITIAL_PREAUTH_TRA_WITHOUT_INCREMENTAL,
								PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG))
			{
				/* Its 1st incremental Preauth trx, we need to update
				 *  the Original BCH20 details */
				if ((result_code= db_update_bch20_original_preauth_txn(&db_recs.bch20, err_msg)) != PTEMSG_OK)
				{
					memset(buffer,0x00,sizeof(buffer));
					sprintf(buffer,"BCH20 original txn update failed, card num: %s, device id: %s"
							"merch id: %s, rrn: %s, stan: %s",auth_tx->TLF01_details.card_num,
							auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
							db_recs.bch20.primary_key.retrieval_ref_num,auth_tx->TLF01_details.sys_trace_audit_num);
					updateds_log_message(2,2,buffer,"process_auth_trans");
					retval = PTEMSG_UPDATE_FAILED;
				}
			}
		}
		else
		{
			memcpy( db_recs.bch20.product_codes[7].code,
					INITIAL_PREAUTH_TRA,
					PREAUTH_TRA_LEN);
			memcpy( db_recs.bch20.product_codes[4].quantity,
					INITIAL_PREAUTH_TRA_WITHOUT_INCREMENTAL,
					PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG);
			auth_tx->TLF01_details.saf[0]=PRIMARY_AUTH; /*PRIMARY authorization*/
		}

		//if (0 != strncmp(auth_tx->TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
		{
			if(true == updatds_Check_If_MC_Transaction(auth_tx))
			{
				strcpy(db_recs.bch20.product_codes[2].code, "MC");
			}
			else if(true == updatds_Check_If_VISA_Transaction(&auth_tx->TLF01_details))
			{
				strcpy(db_recs.bch20.product_codes[2].code, "VISA");
			}
			else if(true == updatds_Check_If_JCB_Transaction(auth_tx))
			{
				strcpy(db_recs.bch20.product_codes[2].code, "JCB");
			}
			else if(true == updatds_Check_If_Diners_Transaction(auth_tx))
			{
				strcpy(db_recs.bch20.product_codes[2].code, "DCI");
			}
			else if(true == updatds_Check_If_AMEX_Transaction(auth_tx))
			{
				strcpy(db_recs.bch20.product_codes[2].code, "AMEX");
			}
			else if(true == updatds_Check_If_CUP_Transaction(auth_tx))
			{
				strcpy(db_recs.bch20.product_codes[2].code, "CUP");
			}
		}

		if (true == updatds_check_if_mid_is_smcc(auth_tx))
		{
			strcpy(db_recs.bch20.product_codes[1].quantity, "CF");/* Update SMCC FLag */
		}

#endif

	if (DB_NullCheck_Flag==1)
	{
	  CheckNULLTerminatedinBCH20 (&db_recs.bch20,MODE_INSERT);
	}
	if ( DB_Timing_Flag == true )
	{
	   start_time = ptetime_get_time();
	}

	if ((result_code= db_insert_bch20(&db_recs.bch20, err_msg)) != PTEMSG_OK)
	{
		/* NewLog, bch20 insert fail
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH20 insert failed, card num: %s, device id: %s"
				"merch id: %s, rrn: %s, stan: %s",auth_tx->TLF01_details.card_num,
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch20.primary_key.retrieval_ref_num,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_auth_trans");
		retval = PTEMSG_INSERT_FAILED;
	}
	else
	{
		 updates_to_db_made = true;
	}

	if ( DB_Timing_Flag == true )
	{
		 end_time = ptetime_get_time();
		 duration = end_time - start_time;
		 update_timing_stats( ST1_DB_INSERT, 2, BCH20_DATA,
							  (float)duration, &TimingStats );
	}

	/******************MID Cappinng code change *********
	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
	if(auth_tx->TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx->TLF01_details.system_date );
	}
	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
						  (float)duration, &TimingStats );
	}

	//create and Insert into MCF03P for this merchant
	if (result_code == PTEMSG_NOT_FOUND)
	{
		memset(&db_recs.mcf03p, 0, sizeof(MCF03P));
		memcpy(&db_recs.mcf03p.merch_id, auth_tx->TLF01_details.merchant_id,15);
		memcpy(&db_recs.mcf03p.system_date,auth_tx->TLF01_details.system_date,8);
		memcpy(&db_recs.mcf03p.total_amount,auth_tx->TLF01_details.total_amount,12);
		memcpy(&db_recs.mcf03p.total_transcn_cnt,"000000000001",12);
		memcpy(&db_recs.mcf03p.total_aprvd_amount,auth_tx->TLF01_details.total_amount,12);
		memcpy(&db_recs.mcf03p.total_reversed_amount,"000000000000",12);
		memcpy(&db_recs.mcf03p.total_aprvd_transcn_count, "000000000001",12);
		memcpy(&db_recs.mcf03p.total_reversed_transcn_count, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_1, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_2, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_3, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_4, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_5, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_6, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_7, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_8, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_9, "000000000000", 12);
		memcpy(&db_recs.mcf03p.field_10, "000000000000", 12);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_INSERT);
		}

		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_insert_mcf03p(&db_recs.mcf03p,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_INSERT, 2, MCF03P_DATA,
								 (float)duration, &TimingStats );
		}

		if (result_code != PTEMSG_OK)
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"New Record  for mcf03p insert failed, card num: %s, device id: %s"
					"merch id: %s,stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans_mp");
			return(PTEMSG_INSERT_FAILED);
		 }

		 updates_to_db_made = true;
	}
	else if (result_code == PTEMSG_OK)// MCF03P record is found
	{
		total_amount = atol(db_recs.mcf03p.total_amount)+
					   atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

		total_aprvd_amount = atol(db_recs.mcf03p.total_aprvd_amount)+
							 atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_aprvd_amount, "%012d",total_aprvd_amount);

		total_aprvd_transcn_count= atol(db_recs.mcf03p.total_aprvd_transcn_count)+ 1;
		sprintf(db_recs.mcf03p.total_aprvd_transcn_count,"%012d",total_aprvd_transcn_count);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
		{

			 memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_sale_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
								 (float)duration, &TimingStats );
		}
		updates_to_db_made = true;
	}
	else
	{

		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
				"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_sale_trans");
		return(PTEMSG_UPDATE_FAILED);
	}

	/******************MID Capping code change End *********/

	return( retval );
}

#if BDOR_62_PREAUTH_VOID
BYTE process_void_preauth_trans(pAUTH_TX auth_tx)
{
    BYTE   retval 		  = PTEMSG_OK;
    double start_time 	  = 0;
    double end_time 	  = 0;
    double duration 	  = 0;
    CHAR   buffer[526]	  = {0};

	memset(&db_recs.bch20, 0, sizeof(BCH20));
	strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
	strcpy(db_recs.bch20.primary_key.retrieval_ref_num, auth_tx->TLF01_details.retrieval_ref_num);
	strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
	strcpy(db_recs.bch20.auth_number, auth_tx->TLF01_details.auth_number);
	strcpy(db_recs.bch20.message_type, auth_tx->TLF01_details.message_type);
	strcpy(db_recs.bch20.card_nbr, auth_tx->TLF01_details.card_num);
	strcpy(db_recs.bch20.sys_trace_audit_num, auth_tx->TLF01_details.sys_trace_audit_num);
	db_recs.bch20.tx_key = auth_tx->TLF01_details.tx_key;
	strcpy(db_recs.bch20.processing_code, auth_tx->TLF01_details.processing_code);

	/* Convert message type to a request type, for standardization. */
	convert_msgtype_to_request( db_recs.bch20.message_type );

	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinBCH20 (&db_recs.bch20,MODE_INSERT);
	}
	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}
	if ((result_code= db_update_void_preauth_bch20(&db_recs.bch20, err_msg)) != PTEMSG_OK)
	{
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH20 update failed, card num: %s, device id: %s"
				"merch id: %s, rrn: %s, stan: %s",auth_tx->TLF01_details.card_num,
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch20.primary_key.retrieval_ref_num,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2, 2, buffer, "process_void_preauth_trans");
		retval = PTEMSG_UPDATE_FAILED;
	}
	else
	{
		updates_to_db_made = true;
	}

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_INSERT, 2, BCH20_DATA,
							  (float)duration, &TimingStats );
	}

	return( retval );
}

BYTE process_reversal_void_preauth_trans(pAUTH_TX auth_tx)
{
   double start_time	= 0;
   double end_time		= 0;
   double duration		= 0;
   CHAR  buffer[526]    = {0};

   memset(&db_recs.bch20, 0, sizeof(BCH20));
   strcpy(db_recs.bch20.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
   strcpy(db_recs.bch20.primary_key.batch_nbr, "ACTIVE");
   strcpy(db_recs.bch20.primary_key.retrieval_ref_num, auth_tx->TLF01_details.orig_retrieval_ref_num);
   strcpy(db_recs.bch20.card_nbr, auth_tx->TLF01_details.card_num);
   strcpy(db_recs.bch20.merchant_id, auth_tx->TLF01_details.merchant_id);
   strcpy( db_recs.bch20.processing_code, auth_tx->TLF01_details.processing_code);
   if(strcmp(auth_tx->TLF01_details.product_codes[1].code, STANDIN) == 0)
   {
	   strcpy(db_recs.bch20.product_codes[1].code, STANDIN);
   }
   else
   {
	   strcpy(db_recs.bch20.product_codes[1].code, "");
   }
   strcpy(db_recs.bch20.product_codes[4].code, "");
   db_recs.bch20.tx_key = AUTH_PRE_AUTHORIZATION_RESPONSE;
   memcpy( db_recs.bch20.processing_code, "30", 2 );

   if(DB_NullCheck_Flag==1)
   {
 	  CheckNULLTerminatedinBCH20 (&db_recs.bch20, MODE_UPDATE);
   }

   if( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   if((result_code= db_update_reversal_void_preauth_bch20(&db_recs.bch20, err_msg))!= PTEMSG_OK)
   {
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer, "BCH20 update failed, device id: %s"
						"merch id: %s, batch num: %s, stan: %s",
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						db_recs.bch20.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2, 2, buffer, "process_reversal_void_preauth_trans");

		return(PTEMSG_UPDATE_FAILED);
   }

   return PTEMSG_OK;
}
#endif

BYTE process_refund_trans(pAUTH_TX auth_tx)
{
   CHAR primary_key[50]		= {0};
   LONG total				=  0;
   CHAR bch01_open_date[9]	= {0};
   CHAR bch01_open_time[7]	= {0};
   CHAR date_time[16] 		= {0};
   INT cnt					=  0;
   BYTE   auth_number[7]  	= {0};
   BYTE   rrn[13]         	= {0};
   TLF01  tlf01;
   double         start_time=  0;
   double         end_time  =  0;
   double         duration  =  0;
   CHAR   buffer[526]= {0};
   CHAR   MCF03P_primary_key[24] = {0};
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_reversed_amount = 0;
   LONG   total_reversed_transcn_count = 0;
   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Update Auth_Tx with original's auth number and RRN.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
     /* memcpy( auth_tx->TLF01_details.auth_number, auth_number, 6 );
      memcpy( auth_tx->TLF01_details.retrieval_ref_num,  rrn, 12 );

      rollback_the_update();
      return PTEMSG_OK;
   } comment ends*/

	strcpy(primary_key, auth_tx->TLF01_details.terminal_id);
	strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
	strcpy(primary_key + BCH01_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
	memset(&db_recs.bch01, 0, sizeof(BCH01));

	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}

	result_code = db_select_bch01(primary_key, &db_recs.bch01, err_msg);
	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, BCH01_DATA,
						  (float)duration, &TimingStats );
	}

	if (result_code == PTEMSG_OK)
	{
		if (auth_tx->bin_type=='D')
		{
			total = atol(db_recs.bch01.total_debit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01.total_debit_refund_amt, "%012d",total);
			if (atol(db_recs.bch01.total_debit_refund_amt) != 0L)
			{
				cnt = atoi(db_recs.bch01.total_debit_refund_cnt) + 1;
			}
			sprintf(db_recs.bch01.total_debit_refund_cnt, "%04d",cnt);
		}
		else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		{
			total = atol(db_recs.bch01.total_credit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
			if (atol(db_recs.bch01.total_credit_refund_amt) != 0L)
			{
				cnt = atoi(db_recs.bch01.total_credit_refund_cnt) + 1;
			}
			sprintf(db_recs.bch01.total_credit_refund_cnt, "%04d",cnt);
		}

		//copy values into temporary variables
		strcpy(bch01_open_time,db_recs.bch01.open_time);
		strcpy(bch01_open_date,db_recs.bch01.open_date);
		if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
		{
			strcpy(db_recs.bch01.visa_merchant_id,"GES");
		}
		else
		{
			strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
		}

		if (DB_NullCheck_Flag==1)
		{
		  CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if (result_code = db_update_bch01(&db_recs.bch01,err_msg) != PTEMSG_OK)
		{
		 /* NewLog,  batch available, bch01 update failed.*/
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"Batch details available, bch01 update failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans");
			return(PTEMSG_UPDATE_FAILED);
		}

		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH01_DATA,
								 (float)duration, &TimingStats );
		}
		updates_to_db_made = true;
	}
	else if (result_code == PTEMSG_NOT_FOUND)
	{
		//Delete BCH10 if any ACTIVE
		memset(&db_recs.bch10, 0, sizeof(BCH10));
		strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch10.primary_key.batch_nbr,  "ACTIVE");

		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_delete_bch10(&db_recs.bch10,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_DELETE, 2, BCH10_DATA,
							 (float)duration, &TimingStats );
		}

		//create and Insert  BCH01
		memcpy(&db_recs.bch01,primary_key,sizeof(primary_key)); //copy the primary Key

		//check if CREDIT refund or DEBIT refund
		if (auth_tx->bin_type =='D')
		{
			strcpy(db_recs.bch01.total_debit_refund_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01.total_debit_refund_amt) != 0)
			{
				strcpy(db_recs.bch01.total_debit_refund_cnt, "0001");
			}
		}
		else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		{
			strcpy(db_recs.bch01.total_credit_refund_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01.total_credit_refund_amt) != 0)
			{
				strcpy(db_recs.bch01.total_credit_refund_cnt, "0001");
			}
		}
		format_date(date_time);
		if (strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
		{
			strcpy(db_recs.bch01.open_date, auth_tx->TLF01_details.date_yyyymmdd);
		}
		else
		{
			memset(db_recs.bch01.open_date, 0, sizeof(db_recs.bch01.open_date));
			strncpy(db_recs.bch01.open_date, date_time, 8);
		}
		if (strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
		{
			strcpy(db_recs.bch01.open_time, auth_tx->TLF01_details.time_hhmmss);
		}
		else
		{
			memset(db_recs.bch01.open_time, 0, sizeof(db_recs.bch01.open_time));
			strncpy(db_recs.bch01.open_time,  date_time + 8, 6);
		}
		strcpy(bch01_open_time,db_recs.bch01.open_time);
		strcpy(bch01_open_date,db_recs.bch01.open_date);
		strcpy(db_recs.bch01.batch_status,"A");
		strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
		if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
		{
			strcpy(db_recs.bch01.visa_merchant_id,"GES");
		}
		else
		{
			strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
		}

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_INSERT);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_insert_bch01(&db_recs.bch01,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_INSERT, 2, BCH01_DATA,
							 (float)duration, &TimingStats );
		}

		if (result_code != PTEMSG_OK)
		{
		 /* NewLog
		  * */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"New batch creation, bch01 insert failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans");
			return(PTEMSG_INSERT_FAILED);
		}
		updates_to_db_made = true;
	}
	else
	{
		/*NewLog
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Batch selection failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_refund_trans");
		return(PTEMSG_INSERT_FAILED);
	}

	//Insert REFUND transaction into BCH10
	memset(&db_recs.bch10, 0, sizeof(BCH10));
	strcpy(db_recs.bch10.open_date,bch01_open_date);
	strcpy(db_recs.bch10.open_time,bch01_open_time);
	strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);
	strcpy(db_recs.bch10.primary_key.batch_nbr,  "ACTIVE");
	db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
	prepare_bch10_for_update(auth_tx);

	/* Convert message type to a request type, for standardization. */
	convert_msgtype_to_request( db_recs.bch10.message_type );

	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
	}
	if ( DB_Timing_Flag == true )
		start_time = ptetime_get_time();

	if ((result_code= db_insert_bch10(&db_recs.bch10,err_msg))!= PTEMSG_OK)
	{
		/* NewLog, bch10 insert failed while processing the offline sale txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Bacth(bch01) details updated but BCH10 insert failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_refund_trans");
		return(PTEMSG_INSERT_FAILED);
	}
	if ( DB_Timing_Flag == true )
	{
		 end_time = ptetime_get_time();
		 duration = end_time - start_time;
		 update_timing_stats( ST1_DB_INSERT, 2, BCH01_DATA,
							  (float)duration, &TimingStats );
	}
	updates_to_db_made = true;

	/******************MID Cappinng code change *********
	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
	if(auth_tx->TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx->TLF01_details.system_date );
	}
	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
						  (float)duration, &TimingStats );
	}

	if (result_code == PTEMSG_OK)// MCF03P record is found
	{
		total_amount = atol(db_recs.mcf03p.total_amount)-
					   atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
							 	atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
								 (float)duration, &TimingStats );
			}
			updates_to_db_made = true;
		}
		else
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}

	/******************MID Capping code change End *********/

	   return PTEMSG_OK;
}

BYTE process_refund_trans_mp(pAUTH_TX auth_tx)
{
   CHAR primary_key[50] 		= "";
   CHAR   MCF03P_primary_key[24] = {0};
   LONG total					= 0;
   CHAR bch01_mp_open_date[9]	="";
   CHAR bch01_mp_open_time[7]	="";
   CHAR date_time[16] 			= "";
   INT cnt						= 0;
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_reversed_amount = 0;
   LONG   total_reversed_transcn_count =0;

   BYTE   auth_number[7]  		= "";
   BYTE   rrn[13]         		= "";
   TLF01  tlf01;
   double         start_time 	= 0;
   double         end_time   	= 0;
   double         duration   	= 0;
   CHAR   buffer[526]= {0};
   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
    *

   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Update Auth_Tx with original's auth number and RRN.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*memcpy( auth_tx->TLF01_details.auth_number, auth_number, 6 );
      memcpy( auth_tx->TLF01_details.retrieval_ref_num,  rrn, 12 );

      rollback_the_update();
      return PTEMSG_OK;
   }*/

	strcpy(primary_key, auth_tx->TLF01_details.terminal_id);
	strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
	strcpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
	memset(&db_recs.bch01_mp, 0, sizeof(BCH01));

	if ( DB_Timing_Flag == true )
	{
		 start_time = ptetime_get_time();
	}

	result_code = db_select_bch01_mp(primary_key, &db_recs.bch01_mp, err_msg);
	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, BCH01_MP_DATA,
						  (float)duration, &TimingStats );
	}

	if (result_code == PTEMSG_OK)
	{
		if (auth_tx->bin_type=='D')
		{
			total = atol(db_recs.bch01_mp.total_debit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01_mp.total_debit_refund_amt, "%012d",total);
			if (atol(db_recs.bch01_mp.total_debit_refund_amt) != 0L)
			   cnt = atoi(db_recs.bch01_mp.total_debit_refund_cnt) + 1;
			sprintf(db_recs.bch01_mp.total_debit_refund_cnt, "%04d",cnt);
		}
		else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		{
			total = atol(db_recs.bch01_mp.total_credit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
			sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
			if (atol(db_recs.bch01_mp.total_credit_refund_amt) != 0L)
			   cnt = atoi(db_recs.bch01_mp.total_credit_refund_cnt) + 1;
			sprintf(db_recs.bch01_mp.total_credit_refund_cnt, "%04d",cnt);
		}

		//copy values into temporary variables
		strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time);
		strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date);
		if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
		{
			strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
		}
		else
		{
			strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
		}

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if (result_code = db_update_bch01_mp(&db_recs.bch01_mp,err_msg) != PTEMSG_OK)
		{
		 /* NewLog,  batch available, bch01 update failed.*/
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"Batch details available, bch01 update failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01_mp.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}

		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, BCH01_MP_DATA,
								 (float)duration, &TimingStats );
		}
		updates_to_db_made = true;
	}
	else if (result_code == PTEMSG_NOT_FOUND)
	{
		//Delete BCH10 if any ACTIVE
		memset(&db_recs.bch10, 0, sizeof(BCH10));
		strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
		strcpy(db_recs.bch10.primary_key.batch_nbr,  "ACTIVE");

		if ( DB_Timing_Flag == true )
			start_time = ptetime_get_time();

		result_code = db_delete_bch10(&db_recs.bch10,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_DELETE, 2, BCH10_DATA,
								 (float)duration, &TimingStats );
		}

		//create and Insert  BCH01
		memcpy(&db_recs.bch01_mp,primary_key,sizeof(primary_key)); //copy the primary Key

		//check if CREDIT refund or DEBIT refund
		if (auth_tx->bin_type =='D')
		{
			strcpy(db_recs.bch01_mp.total_debit_refund_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01_mp.total_debit_refund_amt) != 0)
			{
				strcpy(db_recs.bch01_mp.total_debit_refund_cnt, "0001");
			}
		}
		else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		{
			strcpy(db_recs.bch01_mp.total_credit_refund_amt, auth_tx->TLF01_details.total_amount);
			if (atoi(db_recs.bch01_mp.total_credit_refund_amt) != 0)
			{
				strcpy(db_recs.bch01_mp.total_credit_refund_cnt, "0001");
			}
		}
		format_date(date_time);
		if (strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
		{
			strcpy(db_recs.bch01_mp.open_date, auth_tx->TLF01_details.date_yyyymmdd);
		}
		else
		{
			memset(db_recs.bch01_mp.open_date, 0, sizeof(db_recs.bch01_mp.open_date));
			strncpy(db_recs.bch01_mp.open_date, date_time, 8);
		}
		if (strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
		{
			strcpy(db_recs.bch01_mp.open_time, auth_tx->TLF01_details.time_hhmmss);
		}
		else
		{
			memset(db_recs.bch01_mp.open_time, 0, sizeof(db_recs.bch01_mp.open_time));
			strncpy(db_recs.bch01_mp.open_time,  date_time + 8, 6);
		}
		strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time);
		strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date);
		strcpy(db_recs.bch01_mp.batch_status,"A");
		strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
		if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
		{
			strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
		}
		else
		{
			strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
		}

		if (DB_NullCheck_Flag==1)
		{
		  CheckNULLTerminatedinBCH01_MP(&db_recs.bch01_mp,MODE_INSERT);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_insert_bch01_mp(&db_recs.bch01_mp,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_INSERT, 2, BCH01_MP_DATA,
								 (float)duration, &TimingStats );
		}

		if (result_code != PTEMSG_OK)
		{
			/* NewLog
			* */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"New batch creation, bch01 insert failed,  device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			db_recs.bch01_mp.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_INSERT_FAILED);
		}
		updates_to_db_made = true;
	}
	else
	{
		/*NewLog
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Batch selection failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_refund_trans_mp");
		return(PTEMSG_INSERT_FAILED);
	}

	//Insert REFUND transaction into BCH10
	memset(&db_recs.bch10, 0, sizeof(BCH10));
	strcpy(db_recs.bch10.open_date,bch01_mp_open_date);
	strcpy(db_recs.bch10.open_time,bch01_mp_open_time);
	strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);
	strcpy(db_recs.bch10.primary_key.batch_nbr,  "ACTIVE");
	db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
	prepare_bch10_for_update_MP(auth_tx);

	/* Convert message type to a request type, for standardization. */
	convert_msgtype_to_request( db_recs.bch10.message_type );

	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
	}
	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}

	if ((result_code= db_insert_bch10(&db_recs.bch10,err_msg))!= PTEMSG_OK)
	{
		/* NewLog, bch10 insert failed while processing the offline sale txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Bacth(bch01) details updated but BCH10 insert failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_refund_trans_mp");
		return(PTEMSG_INSERT_FAILED);
	}
	if ( DB_Timing_Flag == true )
	{
		 end_time = ptetime_get_time();
		 duration = end_time - start_time;
		 update_timing_stats( ST1_DB_INSERT, 2, BCH01_MP_DATA,
							  (float)duration, &TimingStats );
	}
	updates_to_db_made = true;

	/******************MID Cappinng code change *********
	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
	if(auth_tx->TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx->TLF01_details.system_date );
	}
	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
						  (float)duration, &TimingStats );
	}

	if (result_code == PTEMSG_OK)// MCF03P record is found
	{
		total_amount = atol(db_recs.mcf03p.total_amount)-
					   atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
							 	atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
		{

			 memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
								 (float)duration, &TimingStats );
			}
			updates_to_db_made = true;
		}
		else
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}

	/******************MID Capping code change End *********/

	return PTEMSG_OK;
}


BYTE process_void_trans(pAUTH_TX auth_tx)
{
	long total 				=  0;
	CHAR primary_key[50] 	= {0};
	int cnt					=  0;
	BYTE   auth_number[7]   = {0};
	BYTE   rrn[13]          = {0};
	TLF01  tlf01;
	double         start_time  = 0;
	double         end_time	   = 0;
	double         duration	   = 0;
	CHAR   buffer[526]		   = {0};
	CHAR   MCF03P_primary_key[24] = {0};
	LONG   total_transcn_cnt = 0;
	LONG   total_amount      = 0;
	LONG   total_reversed_amount = 0;
	LONG   total_reversed_transcn_count = 0;
	/* See if this is a duplicate. */
	memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

	   /* starting of comment
	    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
	    * even thoug host sent different.
	    * Date: Dec 11 2019 commented by Raghavendra H R
	    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
	    * add system date and authnumber as another creteria.
	    */
	    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
	    1. Day1 txn done and settled,
	    2. Day 4 same txn with same details hit ATP.
	    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
	    copied the day 1's auth id and RRN.
	    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
	    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
	    */
	   /*
	if ( DB_Timing_Flag == true )
	start_time = ptetime_get_time();

	result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
		(float)duration, &TimingStats );
	}

	if ( result_code == PTEMSG_OK)
	{
		/* A duplicate has been found.
		 * Rollback any previous DB updates (i.e. ACF01)
		 * Return success!
		 */
		/*rollback_the_update();
		return PTEMSG_OK;
	}Comment ends	*/
	memset(&db_recs.bch10, 0, sizeof(BCH10));
	strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
	strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
	strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);
	db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
	prepare_bch10_for_update(auth_tx);

	/*
	*	For CUP , once void transaction is processed successful,
	*	we should store a flag value to indicate ATP already got response for
	*	Void transaction.*/
	if (true == updatds_Check_If_CUP_Transaction (auth_tx))
	{
		strncpy( db_recs.bch10.product_codes[10].quantity, CUP_VOID_RESP_FLAG_VAL,
				 CUP_VOID_RESP_FLAG_VAL_LEN );
		strncpy( db_recs.bch10.product_codes[10].code,
				 auth_tx->TLF01_details.orig_sys_trace_num,
				 CUP_ORI_STAN_LEN );
	}

	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
	}
	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}

	if ((result_code= db_update_bch10(&db_recs.bch10, err_msg)) != PTEMSG_OK)
	{
		/* NewLog, bch10 update failed while processing void txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH10 update failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_void_trans");
		return(PTEMSG_UPDATE_FAILED);
	}
	else
	{
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
			(float)duration, &TimingStats );
		}

		updates_to_db_made = true;
		strcpy(primary_key, auth_tx->TLF01_details.terminal_id);
		strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
		strcpy(primary_key + BCH01_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
		memset(&db_recs.bch01,0,sizeof(BCH01));

		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		result_code = db_select_bch01(primary_key,&db_recs.bch01,err_msg);
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
			(float)duration, &TimingStats );
		}

		if ( result_code == PTEMSG_OK )
		{
			if (auth_tx->bin_type =='D')
			{
				total = atol(db_recs.bch01.total_debit_amt) - atol(auth_tx->TLF01_details.orig_amount);
				sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
				cnt = atoi(db_recs.bch01.total_debit_cnt) - 1;
				sprintf(db_recs.bch01.total_debit_cnt, "%04d", cnt);
			}
			else
			{
				if ((auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)         ||
					(auth_tx->TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE) ||
					(auth_tx->TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE)       ||
					(auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE_RESPONSE))
				{
					total = atol(db_recs.bch01.total_void_credit_amt) + atol(auth_tx->TLF01_details.orig_amount);
					sprintf(db_recs.bch01.total_void_credit_amt, "%012d",total);
					cnt = atoi(db_recs.bch01.total_void_credit_cnt) + 1;
					sprintf(db_recs.bch01.total_void_credit_cnt, "%04d",cnt);

					total = atol(db_recs.bch01.total_credit_amt) - atol(auth_tx->TLF01_details.orig_amount);
					sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
					cnt = atoi(db_recs.bch01.total_credit_cnt) - 1;
					sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);
				}
				else if( (auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE) ||
						(auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND_RESPONSE))
				{
					total = atol(db_recs.bch01.total_void_credit_refund_amt) + atol(auth_tx->TLF01_details.orig_amount);
					sprintf(db_recs.bch01.total_void_credit_refund_amt, "%012d",total);
					cnt = atoi(db_recs.bch01.total_void_credit_refund_cnt) + 1;
					sprintf(db_recs.bch01.total_void_credit_refund_cnt, "%04d",cnt);

					total = atol(db_recs.bch01.total_credit_refund_amt) - atol(auth_tx->TLF01_details.orig_amount);
					sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
					cnt = atoi(db_recs.bch01.total_credit_refund_cnt) - 1;
					sprintf(db_recs.bch01.total_credit_refund_cnt, "%04d",cnt);
				}
			}

			strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
			if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
			{
				strcpy(db_recs.bch01.visa_merchant_id,"GES");
			}
			else
			{
				strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
			}

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
			}
			if ( DB_Timing_Flag == true )
			{
				start_time = ptetime_get_time();
			}

			if (result_code = db_update_bch01(&db_recs.bch01,err_msg) != PTEMSG_OK)
			{
				/* NewLog
				 * */
				memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"BCH01 update failed, device id: %s"
						"merch id: %s, batch num: %s, stan: %s",
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_void_trans");
				return(PTEMSG_UPDATE_FAILED);
			}
			if ( DB_Timing_Flag == true )
			{
				end_time = ptetime_get_time();
				duration = end_time - start_time;
				update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
				(float)duration, &TimingStats );
			}
			updates_to_db_made = true;
		}
	}
   /******************MID Cappinng code change *********
	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
	if(auth_tx->TLF01_details.system_date[0] == '\0')
	{
		create_system_date( auth_tx->TLF01_details.system_date );
	}
	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
						  (float)duration, &TimingStats );
	}

	if (result_code == PTEMSG_OK)// MCF03P record is found
	{
		total_amount = atol(db_recs.mcf03p.total_amount)-
					   atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
								atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if (result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg) != PTEMSG_OK)
		{

			 memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_void_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
								 (float)duration, &TimingStats );
		}
		updates_to_db_made = true;
	}
	else
	{

		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
				"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_void_trans_mp");
		return(PTEMSG_UPDATE_FAILED);
	}

	/******************MID Capping code change End *********/

	return PTEMSG_OK;
}

BYTE process_void_trans_mp(pAUTH_TX auth_tx)
{
   long total 			= 0;
   CHAR   MCF03P_primary_key[24] = {0};
   CHAR primary_key[50] = "";
   int cnt				= 0;
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_reversed_amount = 0;
   LONG   total_reversed_transcn_count =0;
   BYTE   auth_number[7]= "";
   BYTE   rrn[13]       = "";
   TLF01  tlf01 ;
   double start_time	= 0;
   double end_time		= 0;
   double duration		= 0;
   CHAR   buffer[526]= {0};

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
     /* rollback_the_update();
      return PTEMSG_OK;
   } comment ends*/
   memset(&db_recs.bch10, 0, sizeof(BCH10));
   strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
   strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
   strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
   prepare_bch10_for_update_MP(auth_tx);

	if (DB_NullCheck_Flag==1)
	{
	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
	}
	if ( DB_Timing_Flag == true )
	{
	   start_time = ptetime_get_time();
	}

   if ((result_code= db_update_bch10(&db_recs.bch10, err_msg)) != PTEMSG_OK)
   {
		/* NewLog, bch10 update failed while processing void txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH10 update failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_void_trans_mp");
		return(PTEMSG_UPDATE_FAILED);
   }
   else
   { 
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                              (float)duration, &TimingStats );
      }

      updates_to_db_made = true;
      strcpy(primary_key, auth_tx->TLF01_details.terminal_id);
      strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
	  strcpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET,auth_tx->TLF01_details.merchant_id);
      memset(&db_recs.bch01_mp,0,sizeof(BCH01));

      if ( DB_Timing_Flag == true )
      {
          start_time = ptetime_get_time();
      }

      result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,err_msg);
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                              (float)duration, &TimingStats );
      }
      if ( result_code == PTEMSG_OK )
      {
         if (auth_tx->bin_type =='D')
         {
            total = atol(db_recs.bch01_mp.total_debit_amt) - atol(auth_tx->TLF01_details.orig_amount);
            sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
            cnt = atoi(db_recs.bch01_mp.total_debit_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d", cnt);                        
         }
         else
         {
            if ((auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)         ||
                (auth_tx->TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE) ||
                (auth_tx->TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE)       ||
                (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE_RESPONSE))
            {
               total = atol(db_recs.bch01_mp.total_void_credit_amt) + atol(auth_tx->TLF01_details.orig_amount);
               sprintf(db_recs.bch01_mp.total_void_credit_amt, "%012d",total); 
               cnt = atoi(db_recs.bch01_mp.total_void_credit_cnt) + 1;
               sprintf(db_recs.bch01_mp.total_void_credit_cnt, "%04d",cnt); 

               total = atol(db_recs.bch01_mp.total_credit_amt) - atol(auth_tx->TLF01_details.orig_amount);
               sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);
               cnt = atoi(db_recs.bch01_mp.total_credit_cnt) - 1;
               sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt);         
            }
            else if( (auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE) ||
                     (auth_tx->TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND_RESPONSE))
            {
               total = atol(db_recs.bch01_mp.total_void_credit_refund_amt) + atol(auth_tx->TLF01_details.orig_amount);
               sprintf(db_recs.bch01_mp.total_void_credit_refund_amt, "%012d",total);                                                   
               cnt = atoi(db_recs.bch01_mp.total_void_credit_refund_cnt) + 1;
               sprintf(db_recs.bch01_mp.total_void_credit_refund_cnt, "%04d",cnt);  

               total = atol(db_recs.bch01_mp.total_credit_refund_amt) - atol(auth_tx->TLF01_details.orig_amount);
               sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
               cnt = atoi(db_recs.bch01_mp.total_credit_refund_cnt) - 1;
               sprintf(db_recs.bch01_mp.total_credit_refund_cnt, "%04d",cnt);      
            }
         }

         strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
         if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
         {
             strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
         }
         else
         {
             strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
         }

		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
		  }
         if ( DB_Timing_Flag == true )
         {
             start_time = ptetime_get_time();
         }

         if (result_code = db_update_bch01_mp(&db_recs.bch01_mp,err_msg) != PTEMSG_OK)
         {
				/* NewLog
				 * */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH01 update failed, device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01_mp.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_void_trans");
            return(PTEMSG_UPDATE_FAILED);
         }
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                                 (float)duration, &TimingStats );
         }
         updates_to_db_made = true;
      }
   }

   /******************MID Cappinng code change *********
   	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
   	if(auth_tx->TLF01_details.system_date[0] == '\0')
   	{
   		create_system_date( auth_tx->TLF01_details.system_date );
   	}
   	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

   	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

   	if ( DB_Timing_Flag == true )
   	{
   		end_time = ptetime_get_time();
   		duration = end_time - start_time;
   		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
   						  (float)duration, &TimingStats );
   	}

   	if (result_code == PTEMSG_OK)// MCF03P record is found
   	{
   		total_amount = atol(db_recs.mcf03p.total_amount)-
   					   atol(auth_tx->TLF01_details.total_amount);
   		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

   		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
   		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

   		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
   							 	atol(auth_tx->TLF01_details.total_amount);
   		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

   		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
   		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

   		if (DB_NullCheck_Flag==1)
   		{
   			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
   		}
   		if ( DB_Timing_Flag == true )
   		{
   			start_time = ptetime_get_time();
   		}

   		if (result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg) != PTEMSG_OK)
   		{

   			 memset(buffer,0x00,sizeof(buffer));
   			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
   					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
   					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
   					auth_tx->TLF01_details.sys_trace_audit_num);
   			updateds_log_message(2,2,buffer,"process_void_trans_mp");
   			return(PTEMSG_UPDATE_FAILED);
   		}
   		if ( DB_Timing_Flag == true )
   		{
   			end_time = ptetime_get_time();
   			duration = end_time - start_time;
   			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
   								 (float)duration, &TimingStats );
   		}
   		updates_to_db_made = true;
   	}
   	else
   	{

   		memset(buffer,0x00,sizeof(buffer));
   		sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
   				"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
   				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
   				auth_tx->TLF01_details.sys_trace_audit_num);
   		updateds_log_message(2,2,buffer,"process_void_trans_mp");
   		return(PTEMSG_UPDATE_FAILED);
   	}

   	/******************MID Capping code change End *********/
   return PTEMSG_OK;
}


BYTE process_adjustments(pAUTH_TX auth_tx)
{
   CHAR orig_amount[13]		= {0};
   CHAR new_amount[13]		= {0};
   long diff				=  0;
   long total 				=  0;
   CHAR primary_key[50] 	= {0};
   int cnt					=  0;
   BYTE   auth_number[7]    = {0};
   BYTE   rrn[13]           = {0};
   TLF01  tlf01;
   double         start_time= 0;
   double         end_time  = 0;
   double         duration  = 0;
   CHAR   buffer[526]= {0};
   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*rollback_the_update();
      return PTEMSG_OK;
   } comment ends*/
   strcpy(orig_amount, auth_tx->TLF01_details.orig_amount);
   strcpy(new_amount, auth_tx->TLF01_details.total_amount);
   memset(&db_recs.bch10, 0, sizeof(BCH10));
   strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
   strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
   strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   strcpy(auth_tx->TLF01_details.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
   prepare_bch10_for_update(auth_tx);

   if (DB_NullCheck_Flag==1)
   {
 	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
   }
   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   if ((result_code= db_update_bch10(&db_recs.bch10,err_msg))!= PTEMSG_OK)
   {
		/* NewLog, bch10 update failed while processing void txn,
		 *
		 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH10 update failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_adjustments");
		return(PTEMSG_UPDATE_FAILED);
   }
   else
   { 
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_UPDATE, 2, BCH10_DATA,
                              (float)duration, &TimingStats );
      }

      updates_to_db_made = true;
      strcpy(primary_key, auth_tx->TLF01_details.terminal_id);
      strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
      strcpy(primary_key + BCH01_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
      memset(&db_recs.bch01,0,sizeof(BCH01));

      if ( DB_Timing_Flag == true )
      {
          start_time = ptetime_get_time();
      }
      result_code = db_select_bch01(primary_key, &db_recs.bch01,err_msg);
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                              (float)duration, &TimingStats );
      }

      if ( result_code == PTEMSG_OK )
      {
         if (auth_tx->bin_type =='D')
         {
            total = atol(db_recs.bch01.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
            cnt = atoi(db_recs.bch01.total_debit_cnt) - 1;
            sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);                        
         }
         else
         {
            if (auth_tx->TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE)
            {
               diff = atol(orig_amount) - atol(new_amount);
               total = atol(db_recs.bch01.total_credit_amt) - (diff);
               sprintf(db_recs.bch01.total_credit_amt, "%012d",total);                                                   
            }
            else if (auth_tx->TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT_RESPONSE)
            {
               diff = atol(orig_amount) - atol(new_amount);
               total = atol(db_recs.bch01.total_credit_refund_amt) - (diff);
               sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
            }
         }

         strcpy(db_recs.bch01.merchant_id, auth_tx->TLF01_details.merchant_id);
         if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
         {
             strcpy(db_recs.bch01.visa_merchant_id,"GES");
         }
         else
         {
        	 strcpy(db_recs.bch01.visa_merchant_id, auth_tx->TLF01_details.visa_merchant_id);
         }
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
		  }
         if ( DB_Timing_Flag == true )
         {
             start_time = ptetime_get_time();
         }

         if (result_code = db_update_bch01(&db_recs.bch01, err_msg) != PTEMSG_OK)
         {
     		/* NewLog, bch01 update failed.
     		 *
     		 * */
     		memset(buffer,0x00,sizeof(buffer));
     		sprintf(buffer,"bch01 update failed,  device id: %s"
     				"merch id: %s, batch num: %s, stan: %s",
     				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
     				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
     		updateds_log_message(2,2,buffer,"process_adjustments");
            return(PTEMSG_UPDATE_FAILED);
         }
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, 2, BCH01_MP_DATA,
                                 (float)duration, &TimingStats );
         }
         updates_to_db_made = true;
      }
   }
   return PTEMSG_OK;
}


BYTE process_adjustments_mp(pAUTH_TX auth_tx)
{
   CHAR orig_amount[13]		= {0};
   CHAR new_amount[13]		= {0};
   long diff				=  0;
   long total 				=  0;
   CHAR primary_key[50] 	= {0};
   int  cnt					=  0;
   BYTE   auth_number[7]  	= {0};
   BYTE   rrn[13]         	= {0};
   TLF01  tlf01;
   double         start_time= 0;
   double         end_time  = 0;
   double         duration	= 0;
   CHAR   buffer[526]= {0};
   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Roll back any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*rollback_the_update();
      return PTEMSG_OK;
   }comment ends*/
   strcpy(orig_amount, auth_tx->TLF01_details.orig_amount);
   strcpy(new_amount, auth_tx->TLF01_details.total_amount);
   memset(&db_recs.bch10, 0, sizeof(BCH10));
   strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
   strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
   strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   strcpy(auth_tx->TLF01_details.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
   prepare_bch10_for_update_MP(auth_tx);

	if (DB_NullCheck_Flag==1)
	{
	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
	}
	if ( DB_Timing_Flag == true )
	{
		start_time = ptetime_get_time();
	}

   if ((result_code= db_update_bch10(&db_recs.bch10,err_msg))!= PTEMSG_OK)
   {
	   /*NewLog
	    * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH10 update failed,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_adjustments_mp");
		return(PTEMSG_UPDATE_FAILED);
   }
   else
   { 
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_UPDATE, 2, BCH10_DATA,
                              (float)duration, &TimingStats );
      }

      updates_to_db_made = true;
      strcpy(primary_key, auth_tx->TLF01_details.terminal_id);
      strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
      strcpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
      memset(&db_recs.bch01_mp,0,sizeof(BCH01));

      if ( DB_Timing_Flag == true )
      {
          start_time = ptetime_get_time();
      }

      result_code = db_select_bch01_mp(primary_key, &db_recs.bch01_mp,err_msg);
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                              (float)duration, &TimingStats );
      }

      if ( result_code == PTEMSG_OK )
      {
         if (auth_tx->bin_type =='D')
         {
            total = atol(db_recs.bch01_mp.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
            cnt = atoi(db_recs.bch01_mp.total_debit_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt);                        
         }
         else
         {
            if (auth_tx->TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE)
            {
               diff = atol(orig_amount) - atol(new_amount);
               total = atol(db_recs.bch01_mp.total_credit_amt) - (diff);
               sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);                                                   
            }
            else if (auth_tx->TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT_RESPONSE)
            {
               diff = atol(orig_amount) - atol(new_amount);
               total = atol(db_recs.bch01_mp.total_credit_refund_amt) - (diff);
               sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
            }
         }

         strcpy(db_recs.bch01_mp.merchant_id, auth_tx->TLF01_details.merchant_id);
         if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
         {
             strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
         }
         else
         {
             strcpy(db_recs.bch01_mp.visa_merchant_id, auth_tx->TLF01_details.visa_merchant_id);
         }

		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
		  }
         if ( DB_Timing_Flag == true )
         {
             start_time = ptetime_get_time();
         }

         if (result_code = db_update_bch01_mp(&db_recs.bch01_mp, err_msg) != PTEMSG_OK)
         {
        	 /* NewLog
        	  * */
     		memset(buffer,0x00,sizeof(buffer));
     		sprintf(buffer,"BCH01_MP update failed,  device id: %s"
     				"merch id: %s, batch num: %s, stan: %s",
     				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
     				db_recs.bch01_mp.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
     		updateds_log_message(2,2,buffer,"process_adjustments_mp");
            return(PTEMSG_UPDATE_FAILED);
         }
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, 2, BCH01_DATA,
                                 (float)duration, &TimingStats );
         }
         updates_to_db_made = true;
      }
   }
   return PTEMSG_OK;
}

BYTE process_reversal_non_void_trans(pAUTH_TX auth_tx, BYTE temp_tx_key)
{
   LONG total				=  0 ;
   CHAR primary_key[50] 	= {0};
   INT cnt					=  0;
   BYTE   auth_number[7]  	= {0};
   BYTE   rrn[13]         	= {0};
   TLF01  tlf01;
   double         start_time= 0;
   double         end_time  = 0;
   double         duration  = 0;
   CHAR buffer[526]			= {0};

   CHAR   MCF03P_primary_key[24] = {0};
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_reversed_amount = 0;
   LONG   total_reversed_transcn_count = 0;

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*rollback_the_update();
      return PTEMSG_OK;
   }comment ends*/
   strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
   strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
   strcpy(primary_key + BCH01_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
   memset(&db_recs.bch01, 0, sizeof(BCH01));

   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   result_code = db_select_bch01(primary_key, &db_recs.bch01, err_msg);                 
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, BCH01_DATA,
                           (float)duration, &TimingStats );
   }
   if ( result_code == PTEMSG_OK )
	{
		  if (auth_tx->bin_type == 'D')
		  {
			 if (temp_tx_key == AUTH_REFUND_RESPONSE)
			 {
				total = atol(db_recs.bch01.total_debit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
				sprintf(db_recs.bch01.total_debit_refund_amt, "%012d",total);
				cnt = atoi(db_recs.bch01.total_debit_refund_cnt) - 1;
				sprintf(db_recs.bch01.total_debit_refund_cnt, "%04d",cnt);
			 }
			 else
			 {
				total = atol(db_recs.bch01.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
				sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
				cnt = atoi(db_recs.bch01.total_debit_cnt) - 1;
				sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);
			 }
		  }
		  else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
		  {
			 if (temp_tx_key == AUTH_REFUND_RESPONSE)
			 {
				total = atol(db_recs.bch01.total_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
				sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
				cnt = atoi(db_recs.bch01.total_credit_refund_cnt) - 1;
				sprintf(db_recs.bch01.total_credit_refund_cnt, "%04d",cnt);
			 }
			 else
			 {
				total = atol(db_recs.bch01.total_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
				sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
				cnt = atoi(db_recs.bch01.total_credit_cnt) - 1;
				sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);
			 }
		  }

		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
		  }
		  if ( DB_Timing_Flag == true )
		  {
			  start_time = ptetime_get_time();
		  }

		  if (result_code = db_update_bch01(&db_recs.bch01,err_msg) != PTEMSG_OK)
		  {
			  /* NewLog
			   * */
				memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"BCH01 update failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_reversal_non_void_trans");
				return(PTEMSG_UPDATE_FAILED);
		  }

		  if ( DB_Timing_Flag == true )
		  {
			 end_time = ptetime_get_time();
			 duration = end_time - start_time;
			 update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
								  (float)duration, &TimingStats );
		  }
		  updates_to_db_made = true;
	}
	else
	{
	  /* NewLog
	   * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH01 select failed, device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_reversal_non_void_trans");
		return(PTEMSG_UPDATE_FAILED);
	}

   /******************MID Cappinng code change *********
   	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
   	if(auth_tx->TLF01_details.system_date[0] == '\0')
   	{
   		create_system_date( auth_tx->TLF01_details.system_date );
   	}
   	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

   	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

   	if ( DB_Timing_Flag == true )
   	{
   		end_time = ptetime_get_time();
   		duration = end_time - start_time;
   		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
   						  (float)duration, &TimingStats );
   	}

   	if (result_code == PTEMSG_OK)// MCF03P record is found
   	{
   		total_amount = atol(db_recs.mcf03p.total_amount)-
   					   atol(auth_tx->TLF01_details.total_amount);
   		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

   		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
   		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

   		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
   							 	atol(auth_tx->TLF01_details.total_amount);
   		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

   		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
   		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

   		if (DB_NullCheck_Flag==1)
   		{
   			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
   		}
   		if ( DB_Timing_Flag == true )
   		{
   			start_time = ptetime_get_time();
   		}

   		if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
   		{

   			 memset(buffer,0x00,sizeof(buffer));
   			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
   					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
   					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
   					auth_tx->TLF01_details.sys_trace_audit_num);
   			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
   			return(PTEMSG_UPDATE_FAILED);
   		}
   		if ( DB_Timing_Flag == true )
   		{
   			end_time = ptetime_get_time();
   			duration = end_time - start_time;
   			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
   								 (float)duration, &TimingStats );
   			}
   			updates_to_db_made = true;
   		}
   		else
   		{

   			memset(buffer,0x00,sizeof(buffer));
   			sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
   					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
   					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
   					auth_tx->TLF01_details.sys_trace_audit_num);
   			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
   			return(PTEMSG_UPDATE_FAILED);
   		}

   	/******************MID Capping code change End *********/
    return PTEMSG_OK;
}

BYTE process_reversal_non_void_trans_mp(pAUTH_TX auth_tx, BYTE temp_tx_key)
{
   LONG total				=  0;
   CHAR primary_key[50] 	= {0};
   INT cnt					=  0;
   BYTE   auth_number[7]  	= {0};
   BYTE   rrn[13]         	= {0};
   TLF01  tlf01;
   double         start_time= 0;
   double         end_time	= 0;
   double         duration  = 0;
   CHAR buffer[526]			= {0};
   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*rollback_the_update();
      return PTEMSG_OK;
   }*/
   strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
   strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
   strcpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
   memset(&db_recs.bch01_mp, 0, sizeof(BCH01));

   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   result_code = db_select_bch01_mp(primary_key, &db_recs.bch01_mp, err_msg);                 
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, BCH01_MP_DATA,
                           (float)duration, &TimingStats );
   }
   if ( result_code == PTEMSG_OK )
   {
      if (auth_tx->bin_type == 'D')
      {
         if (temp_tx_key == AUTH_REFUND_RESPONSE)
         {
            total = atol(db_recs.bch01_mp.total_debit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_debit_refund_amt, "%012d",total);
            cnt = atoi(db_recs.bch01_mp.total_debit_refund_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_debit_refund_cnt, "%04d",cnt); 
         }
         else
         {
            total = atol(db_recs.bch01_mp.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
            cnt = atoi(db_recs.bch01_mp.total_debit_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt); 
         }
      }
      else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
      {
         if (temp_tx_key == AUTH_REFUND_RESPONSE)
         {
            total = atol(db_recs.bch01_mp.total_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
            cnt = atoi(db_recs.bch01_mp.total_credit_refund_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_credit_refund_cnt, "%04d",cnt); 
         }
         else
         {
            total = atol(db_recs.bch01_mp.total_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);                                                   
            cnt = atoi(db_recs.bch01_mp.total_credit_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt); 
         }
      }
	  if (DB_NullCheck_Flag==1)
	  {
		  CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
	  }
      if ( DB_Timing_Flag == true )
      {
          start_time = ptetime_get_time();
      }
      if (result_code = db_update_bch01_mp(&db_recs.bch01_mp,err_msg) != PTEMSG_OK)
      {
		  /* NewLog
		   * */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH01 update failed, device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_reversal_non_void_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
      }

      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                              (float)duration, &TimingStats );
      }
      updates_to_db_made = true;
   }
   else
   {
		  /* NewLog
		   * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH01 select failed, device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_reversal_non_void_trans_mp");
		return(PTEMSG_UPDATE_FAILED);
   }
   return PTEMSG_OK;
}


BYTE process_reversal_void_trans(pAUTH_TX auth_tx)
{
   int cnt				= 0;
   long total 			= 0;
   CHAR primary_key[50] = {0};
   BYTE   auth_number[7]= {0};
   BYTE   rrn[13]       = {0};
   TLF01  tlf01;
   double start_time	= 0;
   double end_time		= 0;
   double duration		= 0;
   CHAR  buffer[526]    = {0};

   CHAR   MCF03P_primary_key[24] = {0};
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_reversed_amount = 0;
   LONG   total_reversed_transcn_count = 0;

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*rollback_the_update();
      return PTEMSG_OK;
   }comment ends*/
   memset(&db_recs.bch10, 0, sizeof(BCH10));
   strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
   strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
   strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   prepare_bch10_for_update(auth_tx);
   if (OrigTxKey == AUTH_VOID_SALE_RESPONSE)
   {

		if((auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE || auth_tx->TLF01_details.tx_key == AUTH_REVERSAL ) &&
		   (strncmp(auth_tx->TLF01_details.product_codes[11].amount,"01",2) == 0))
		{
			db_recs.bch10.tx_key = AUTH_CASH_ADVANCE;
			strncpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.product_codes[11].amount,6);
		}
		else
		{
      db_recs.bch10.tx_key = AUTH_SALE_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "00", 2 );
   }

   }
   else if (OrigTxKey == AUTH_VOID_REFUND_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_REFUND_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "20", 2 );
   }
   else if (OrigTxKey == AUTH_VOID_CASH_ADVANCE_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "01", 2 );
   }

   /* SCR 12502 -ThoughtFocus */
   else if (OrigTxKey == AUTH_VOID_RELOAD_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_RELOAD_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "21", 2 );
   }
   else if (OrigTxKey == AUTH_OFFLINE_VOID_SALE_RESPONSE ||
            OrigTxKey == AUTH_SALE_ADJUSTMENT_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "00", 2 );
   }
   else if (OrigTxKey == AUTH_OFFLINE_VOID_REFUND_RESPONSE ||
            OrigTxKey == AUTH_REFUND_ADJUSTMENT_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "20", 2 );
   }

   if (DB_NullCheck_Flag==1)
   {
 	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
   }
   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   if( true == updatds_Check_If_CUP_Transaction(auth_tx))
   {
	   if ((result_code= db_update_bch10_for_CUP_VOID_REV(&db_recs.bch10, err_msg))!= PTEMSG_OK)
	   {
			  /* NewLog
			   * */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH10 update failed for cup,  device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_reversal_void_trans");
		    return(PTEMSG_UPDATE_FAILED);
	   }
   }
   else if ((result_code= db_update_bch10(&db_recs.bch10, err_msg))!= PTEMSG_OK)
   {
		  /* NewLog
		   * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH10 update failed, device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_reversal_void_trans");
        return(PTEMSG_UPDATE_FAILED);
   }

   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_UPDATE, 2, BCH10_DATA,
                           (float)duration, &TimingStats );
   }

   updates_to_db_made = true;
   strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
   strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
   strcpy(primary_key + BCH01_MERCHANT_ID_OFFSET,auth_tx->TLF01_details.merchant_id);
   memset(&db_recs.bch01,0,sizeof(BCH01));

   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   result_code = db_select_bch01(primary_key,&db_recs.bch01,err_msg);
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, BCH01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK )
   {
      if (auth_tx->bin_type=='D')
      {
         total = atol(db_recs.bch01.total_debit_amt) + atol(auth_tx->TLF01_details.total_amount);
         sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
         cnt = atoi(db_recs.bch01.total_debit_cnt) + 1;
         sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);
      }
      else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
      {
         total = atol(db_recs.bch01.total_credit_amt) + atol(auth_tx->TLF01_details.total_amount);
         sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
         cnt = atoi(db_recs.bch01.total_credit_cnt) + 1;
         sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);

         /* Deduct the void totals also */
		 /* SCR 12502 -ThoughtFocus */
       /* if (auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)*/
		 if (auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE ||
             auth_tx->TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE)
         {
            total = atol(db_recs.bch01.total_void_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01.total_void_credit_amt, "%012d",total);
            cnt = atoi(db_recs.bch01.total_void_credit_cnt) - 1;
            sprintf(db_recs.bch01.total_void_credit_cnt, "%04d",cnt);                                                   
         }
         else if (auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE)
         {
            total = atol(db_recs.bch01.total_void_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01.total_void_credit_refund_amt, "%012d",total); 
            cnt = atoi(db_recs.bch01.total_void_credit_refund_cnt) - 1;
            sprintf(db_recs.bch01.total_void_credit_refund_cnt, "%04d",cnt);                                                   
         }
      }
      if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
      {
          strcpy(db_recs.bch01.visa_merchant_id,"GES");
      }
      else
      {
          strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
      }

      strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
	  if (DB_NullCheck_Flag==1)
	  {
		  CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
	  }
      if ( DB_Timing_Flag == true )
      {
    	  start_time = ptetime_get_time();
      }

      if (result_code = db_update_bch01(&db_recs.bch01,err_msg) != PTEMSG_OK)
      {
		  /* NewLog
		   * */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH01 update failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					db_recs.bch01.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_reversal_void_trans");
			return(PTEMSG_UPDATE_FAILED);
      }
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_UPDATE, 2, BCH01_DATA,
                              (float)duration, &TimingStats );
      }
      updates_to_db_made = true;
   }

   /******************MID Cappinng code change *********
   	strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
   	if(auth_tx->TLF01_details.system_date[0] == '\0')
   	{
   		create_system_date( auth_tx->TLF01_details.system_date );
   	}
   	strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

   	result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

   	if ( DB_Timing_Flag == true )
   	{
   		end_time = ptetime_get_time();
   		duration = end_time - start_time;
   		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
   						  (float)duration, &TimingStats );
   	}

   	if (result_code == PTEMSG_OK)// MCF03P record is found
   	{
   		total_amount = atol(db_recs.mcf03p.total_amount)-
   					   atol(auth_tx->TLF01_details.total_amount);
   		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

   		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
   		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

   		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
   							 	atol(auth_tx->TLF01_details.total_amount);
   		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

   		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
   		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

   		if (DB_NullCheck_Flag==1)
   		{
   			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
   		}
   		if ( DB_Timing_Flag == true )
   		{
   			start_time = ptetime_get_time();
   		}

   		if ((result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg)) != PTEMSG_OK)
   		{

   			 memset(buffer,0x00,sizeof(buffer));
   			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
   					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
   					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
   					auth_tx->TLF01_details.sys_trace_audit_num);
   			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
   			return(PTEMSG_UPDATE_FAILED);
   		}
   		if ( DB_Timing_Flag == true )
   		{
   			end_time = ptetime_get_time();
   			duration = end_time - start_time;
   			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
   								 (float)duration, &TimingStats );
   			}
   			updates_to_db_made = true;
   		}
   		else
   		{

   			memset(buffer,0x00,sizeof(buffer));
   			sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
   					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
   					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
   					auth_tx->TLF01_details.sys_trace_audit_num);
   			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
   			return(PTEMSG_UPDATE_FAILED);
   		}

   	/******************MID Capping code change End *********/

   return PTEMSG_OK;
}

BYTE process_reversal_void_trans_mp(pAUTH_TX auth_tx)
{
   int 	cnt				= 0;
   long total 			= 0;
   CHAR primary_key[50] = "";
   BYTE   auth_number[7]= "";
   BYTE   rrn[13]       = "";
   TLF01  tlf01;
   double  start_time	= 0;
   double  end_time	    = 0;
   double  duration		= 0;
   CHAR  buffer[526]    = {0};
   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
     /* rollback_the_update();
      return PTEMSG_OK;
   } comment ends*/
   memset(&db_recs.bch10, 0, sizeof(BCH10));
   strcpy(db_recs.bch10.primary_key.device_id, auth_tx->TLF01_details.terminal_id);
   strcpy(db_recs.bch10.primary_key.batch_nbr, "ACTIVE");
   strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);     
   prepare_bch10_for_update_MP(auth_tx);
   if (OrigTxKey == AUTH_VOID_SALE_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_SALE_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "00", 2 );
   }
   else if (OrigTxKey == AUTH_VOID_REFUND_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_REFUND_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "20", 2 );
   }
   else if (OrigTxKey == AUTH_VOID_CASH_ADVANCE_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "01", 2 );
   }

   /* SCR 12502 -ThoughtFocus */
   else if (OrigTxKey == AUTH_VOID_RELOAD_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_RELOAD_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "21", 2 );
   }
   else if (OrigTxKey == AUTH_OFFLINE_VOID_SALE_RESPONSE ||
            OrigTxKey == AUTH_SALE_ADJUSTMENT_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "00", 2 );
   }
   else if (OrigTxKey == AUTH_OFFLINE_VOID_REFUND_RESPONSE ||
            OrigTxKey == AUTH_REFUND_ADJUSTMENT_RESPONSE)
   {
      db_recs.bch10.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
      memcpy( db_recs.bch10.processing_code, "20", 2 );
   }


   if (DB_NullCheck_Flag==1)
   {
 	  CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
   }
   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   if ((result_code= db_update_bch10(&db_recs.bch10, err_msg))!= PTEMSG_OK)
   {
		  /* NewLog
		   * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH10 update failed,  device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			db_recs.bch10.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_reversal_void_trans_mp");
		return(PTEMSG_UPDATE_FAILED);
   }

   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_UPDATE, 2, BCH10_DATA,
                           (float)duration, &TimingStats );
   }

   updates_to_db_made = true;
   strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
   strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
   strcpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET , auth_tx->TLF01_details.merchant_id);
   memset(&db_recs.bch01_mp,0,sizeof(BCH01));

   if ( DB_Timing_Flag == true )
   {
	   start_time = ptetime_get_time();
   }

   result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,err_msg);
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, BCH01_MP_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK )
   {
      if (auth_tx->bin_type=='D')
      {
         total = atol(db_recs.bch01_mp.total_debit_amt) + atol(auth_tx->TLF01_details.total_amount);
         sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
         cnt = atoi(db_recs.bch01_mp.total_debit_cnt) + 1;
         sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt);
      }
      else if ( (auth_tx->bin_type == 'C') || (auth_tx->bin_type == 'E') )
      {
         total = atol(db_recs.bch01_mp.total_credit_amt) + atol(auth_tx->TLF01_details.total_amount);
         sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);
         cnt = atoi(db_recs.bch01_mp.total_credit_cnt) + 1;
         sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt);

         /* Deduct the void totals also */
		 /* SCR 12502 -ThoughtFocus */
       /* if (auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)*/
		 if (auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE ||
             auth_tx->TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE)
         {
            total = atol(db_recs.bch01_mp.total_void_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_void_credit_amt, "%012d",total);
            cnt = atoi(db_recs.bch01_mp.total_void_credit_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_void_credit_cnt, "%04d",cnt);                                                   
         }
         else if (auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE)
         {
            total = atol(db_recs.bch01_mp.total_void_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
            sprintf(db_recs.bch01_mp.total_void_credit_refund_amt, "%012d",total); 
            cnt = atoi(db_recs.bch01_mp.total_void_credit_refund_cnt) - 1;
            sprintf(db_recs.bch01_mp.total_void_credit_refund_cnt, "%04d",cnt);                                                   
         }
      }
      if (strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0)
      {
          strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
      }
      else
      {
          strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
      }

      strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);

	  if (DB_NullCheck_Flag==1)
	  {
		  CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
	  }
      if ( DB_Timing_Flag == true )
      {
          start_time = ptetime_get_time();
      }

      if (result_code = db_update_bch01_mp(&db_recs.bch01_mp,err_msg) != PTEMSG_OK)
      {
		  /* NewLog
		   * */
			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"BCH01_MP update failed, device id: %s"
				"merch id: %s, batch num: %s, stan: %s",
				auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
				db_recs.bch01_mp.primary_key.batch_nbr,auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_reversal_void_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
      }
      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_UPDATE, 2, BCH01_MP_DATA,
                              (float)duration, &TimingStats );
      }
      updates_to_db_made = true;
   }
   return PTEMSG_OK;
}


BYTE process_reversal(pAUTH_TX auth_tx)
{
   BYTE orig_tx_key  = 0;
   CHAR   MCF03P_primary_key[24] = {0};
   BCH10 bch10 ;
   BCH20 bch20 ;
   BYTE   auth_number[7]  = {0};
   BYTE   rrn[13]         = {0};
   TLF01  tlf01;
   double         start_time;
   double         end_time;
   double         duration;
   LONG   total_transcn_cnt = 0;
   LONG   total_amount      = 0;
   LONG   total_reversed_amount = 0;
   LONG   total_reversed_transcn_count =0;
   CHAR buffer[526] = {0};

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );

   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
      /*rollback_the_update();
      return PTEMSG_OK;
   } comment ends*/

   result_code = PTEMSG_OK;

   if ( 0 == strcmp(auth_tx->TLF01_details.handler_queue,"VOICE") )
   {
      if ( DB_Timing_Flag == true )
         start_time = ptetime_get_time();

      if (db_delete_bch20_by_auth_nbr(&auth_tx->TLF01_details,err_msg) != PTEMSG_OK)
      {
		  /* NewLog
		   * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH20 delete by auth number failed,  device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_reversal");
         return(PTEMSG_DELETE_FAILED);
      }

      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_DELETE, 2, BCH20_DATA,
                              (float)duration, &TimingStats );
      }
   }
   else
   {
      orig_tx_key = get_orig_response_tx_key_for_reversal(auth_tx->TLF01_details.orig_message,
                                                          auth_tx->TLF01_details.processing_code,
                                                          auth_tx->TLF01_details.voice_auth);
      OrigTxKey = orig_tx_key;
      if ( DB_Timing_Flag == true )
         start_time = ptetime_get_time();

      switch (orig_tx_key)
      {
#if BDOR_62_PREAUTH_VOID
      	 case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
      		  result_code = process_reversal_void_preauth_trans(auth_tx);
      		  break;
#endif
         case AUTH_AUTHORIZATION_RESPONSE:
         case AUTH_PRE_AUTHORIZATION_RESPONSE:

            memset(&bch20,0,sizeof(BCH20));
            strcpy(bch20.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
            strcpy(bch20.primary_key.batch_nbr, "ACTIVE");
            strcpy(bch20.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
            strcpy(bch20.merchant_id,auth_tx->TLF01_details.merchant_id);
			
			/*
				For CUP reversal , we will locate original transaction with the help of
				card number , terminal id, merchant id, and amount with latest one in search
			*/
			if (true == updatds_Check_If_CUP_Transaction( auth_tx))
			{
				strcpy(bch20.card_nbr,auth_tx->TLF01_details.card_num);
           		strcpy(bch20.tran_amount,auth_tx->TLF01_details.total_amount);
				strcpy(bch20.tran_date,auth_tx->TLF01_details.date_yyyymmdd );
				strcpy(bch20.invoice_nbr,auth_tx->TLF01_details.invoice_number);
				 if (db_delete_bch20_for_CUP (&bch20,err_msg) != PTEMSG_OK)
				 {
					  /* NewLog
					   * */
					memset(buffer,0x00,sizeof(buffer));
					sprintf(buffer,"BCH20 delete failed for CUP,  device id: %s"
						"merch id: %s, batch num: %s, stan: %s",
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
					updateds_log_message(2,2,buffer,"process_reversal");
               		result_code = PTEMSG_DELETE_FAILED;
			}

			}
			else
			{
            	if (db_delete_bch20_by_stan (&bch20,err_msg) != PTEMSG_OK)
            	{
            		 /* NewLog
            				   * */
					memset(buffer,0x00,sizeof(buffer));
					sprintf(buffer,"BCH20 delete by stan failed,  device id: %s"
						"merch id: %s, batch num: %s, stan: %s",
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
					updateds_log_message(2,2,buffer,"process_reversal");
               		result_code = PTEMSG_DELETE_FAILED;
			}

			}
            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_DELETE, 2, BCH20_DATA,
                                    (float)duration, &TimingStats );
            }
            break;


         case AUTH_SALE_RESPONSE:
         case AUTH_REFUND_RESPONSE:
         case AUTH_CASH_ADVANCE_RESPONSE:
         case AUTH_OFFLINE_SALE_RESPONSE:
         case AUTH_QUASI_CASH_RESPONSE:

            result_code = process_reversal_non_void_trans(auth_tx,orig_tx_key);
            memset(&bch10,0,sizeof(BCH10));
            strcpy(bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
            strcpy(bch10.primary_key.batch_nbr, "ACTIVE");
            strcpy(bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
            strcpy(bch10.merchant_id,auth_tx->TLF01_details.merchant_id);

			/*
				For CUP reversal , we will locate original transaction with the help of
				card number , terminal id, merchant id, and amount with latest one in search
			*/
			if (true == updatds_Check_If_CUP_Transaction (auth_tx))
			{
				strcpy(bch10.card_nbr,auth_tx->TLF01_details.card_num);
           		strcpy(bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				strcpy(bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
				strcpy(bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
				if (db_delete_bch10_for_CUP (&bch10,err_msg) != PTEMSG_OK)
				{
					/* NewLog
           			 * */
					memset(buffer,0x00,sizeof(buffer));
					sprintf(buffer,"BCH10 delete failed, device id: %s"
						"merch id: %s, batch num: %s, stan: %s",
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
					updateds_log_message(2,2,buffer,"process_reversal");
              		 result_code = PTEMSG_DELETE_FAILED;
			}

			}
    	    else
			{
				if (db_delete_bch10_by_stan (&bch10,err_msg) != PTEMSG_OK)
				{
					/* NewLog
           			 * */
					memset(buffer,0x00,sizeof(buffer));
					sprintf(buffer,"BCH10 delete by stan failed, device id: %s"
						"merch id: %s, batch num: %s, stan: %s",
						auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
						auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
					updateds_log_message(2,2,buffer,"process_reversal");
               		result_code = PTEMSG_DELETE_FAILED;
    	    }
    	    }

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_DELETE, 2, BCH10_DATA,
                                    (float)duration, &TimingStats );
            }
            break;

         case AUTH_VOID_SALE_RESPONSE:
         case AUTH_VOID_REFUND_RESPONSE:
         case AUTH_VOID_CASH_ADVANCE_RESPONSE:
         /* SCR 12502 - ThoughtFocus
		  * Adding the case statement, to enable REVERSAL of AUTH_VOID_REFUND
		  * AUTH_OFFLINE_VOID_SALE, AUTH_OFFLINE_VOID_REFUND,
		  * AUTH_SALE_ADJUSTMENT, AUTH_REFUND_ADJUSTMENT 
		  */
		 case AUTH_VOID_RELOAD_RESPONSE:
		 case AUTH_OFFLINE_VOID_SALE_RESPONSE:
		 case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
         /* SCR 12502 - ThoughtFocus
		  * Since AUTH_SALE_ADJUSTMENT, AUTH_REFUND_ADJUSTMENT function similar to
		  * VOIDs, process_reversal_void_trans is called for these transactions also.
		  */
		 case AUTH_SALE_ADJUSTMENT_RESPONSE:
         case AUTH_REFUND_ADJUSTMENT_RESPONSE:
            result_code = process_reversal_void_trans(auth_tx);
            break;

         default: 
            sprintf( err_msg,
                    "Unknown Message Type: %s, Processing Code: %s (%s)",
                     auth_tx->TLF01_details.message_type,
                     auth_tx->TLF01_details.processing_code,
                     ServiceName );
            result_code = PTEMSG_INVALID_DATATYPE;
            break;
       }
   }

	/******************MID Cappinng code change *********

   strncpy (MCF03P_primary_key,auth_tx->TLF01_details.merchant_id,15);
   if(auth_tx->TLF01_details.system_date[0] == '\0')
   {
	   create_system_date( auth_tx->TLF01_details.system_date );
   }
   strncat(MCF03P_primary_key,auth_tx->TLF01_details.system_date, 8);

   result_code = db_select_mcf03p(MCF03P_primary_key,&db_recs.mcf03p,err_msg);

	if ( DB_Timing_Flag == true )
	{
		end_time = ptetime_get_time();
		duration = end_time - start_time;
		update_timing_stats( ST1_DB_SELECT, 2, MCF03P_DATA,
				  (float)duration, &TimingStats );
	}

	if (result_code == PTEMSG_OK)// MCF03P record is found
	{
		total_amount = atol(db_recs.mcf03p.total_amount)-
					   atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_amount, "%012d",total_amount);

		total_transcn_cnt= atol(db_recs.mcf03p.total_transcn_cnt)+ 1;
		sprintf(db_recs.mcf03p.total_transcn_cnt,"%012d",total_transcn_cnt);

		total_reversed_amount = atol(db_recs.mcf03p.total_reversed_amount)+
								atol(auth_tx->TLF01_details.total_amount);
		sprintf(db_recs.mcf03p.total_reversed_amount, "%012d",total_reversed_amount);

		total_reversed_transcn_count= atol(db_recs.mcf03p.total_reversed_transcn_count)+ 1;
		sprintf(db_recs.mcf03p.total_reversed_transcn_count,"%012d",total_reversed_transcn_count);

		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCF03P (&db_recs.mcf03p,MODE_UPDATE);
		}
		if ( DB_Timing_Flag == true )
		{
			start_time = ptetime_get_time();
		}

		if (result_code = db_update_mcf03p(&db_recs.mcf03p,err_msg) != PTEMSG_OK)
		{

			memset(buffer,0x00,sizeof(buffer));
			sprintf(buffer,"MCF03P update failed, card num: %s, device id: %s"
					"merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.sys_trace_audit_num);
			updateds_log_message(2,2,buffer,"process_refund_trans_mp");
			return(PTEMSG_UPDATE_FAILED);
		}
		if ( DB_Timing_Flag == true )
		{
			end_time = ptetime_get_time();
			duration = end_time - start_time;
			update_timing_stats( ST1_DB_UPDATE, 2, MCF03P_DATA,
						   	   (float)duration, &TimingStats );
		}
		updates_to_db_made = true;
	}
	else
	{

	   memset(buffer,0x00,sizeof(buffer));
	   sprintf(buffer,"Checking for MCF03P selection failed, card num: %s, device id: %s"
			   "merch id: %s, stan: %s",auth_tx->TLF01_details.card_num,
			   auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			   auth_tx->TLF01_details.sys_trace_audit_num);
	   updateds_log_message(2,2,buffer,"process_refund_trans_mp");
	   return(PTEMSG_UPDATE_FAILED);
	}

	/******************MID Capping code change End *********/

   return result_code;
}

BYTE process_reversal_mp(pAUTH_TX auth_tx)
{
   BYTE orig_tx_key ;
   BCH10 bch10 ;
   BCH20 bch20 ;
   BYTE   auth_number[7]  = "";
   BYTE   rrn[13]         = "";
   TLF01  tlf01;
   double         start_time;
   double         end_time;
   double         duration;
   CHAR buffer[526] = {0};

   /* See if this is a duplicate. */
   memcpy( &tlf01, &auth_tx->TLF01_details, sizeof(TLF01) );
   /* starting of comment
    * Marvin, Anna and Nlson raised the issue, ATP populating duplicate auth id and rrn
    * even thoug host sent different.
    * Date: Dec 11 2019 commented by Raghavendra H R
    * if again any issue occured regarding no duplicate check, enable this code and alter the query,
    * add system date and authnumber as another creteria.
    */
    /* The below code is commented to avoid duplication of auth number and RRN, scenario is:
    1. Day1 txn done and settled,
    2. Day 4 same txn with same details hit ATP.
    3. Bcz of below function ATP found TLF01 detail in DB and assumed that day4 txn as duplicate and
    copied the day 1's auth id and RRN.
    Bcz of this for day4 txn ATP is sending day1's auth id and rrn, as we are checking the duplicate in
    DCPISO with BCH10 and based on the discussion, decision made to comment below duplicate check code.
    */
   /*
   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   result_code = db_look_for_duplicate( &tlf01, auth_number, rrn );
   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, 2, TLF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( result_code == PTEMSG_OK)
   {
      /* A duplicate has been found.
       * Rollback any previous DB updates (i.e. ACF01)
       * Return success!
       */
     /* rollback_the_update();
      return PTEMSG_OK;
   }*/

   result_code = PTEMSG_OK;

   if ( 0 == strcmp(auth_tx->TLF01_details.handler_queue,"VOICE") )
   {
      if ( DB_Timing_Flag == true )
         start_time = ptetime_get_time();

      if (db_delete_bch20_by_auth_nbr(&auth_tx->TLF01_details,err_msg) != PTEMSG_OK)
      {
			/* NewLog
 			 * */
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer,"BCH20 delete by auth number failed,  device id: %s"
			"merch id: %s, batch num: %s, stan: %s",
			auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
			auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
		updateds_log_message(2,2,buffer,"process_reversal_mp");
         return(PTEMSG_DELETE_FAILED);
      }

      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_DELETE, 2, BCH20_DATA,
                              (float)duration, &TimingStats );
      }
   }
   else
   {
      orig_tx_key = get_orig_response_tx_key_for_reversal(auth_tx->TLF01_details.orig_message,
                                                          auth_tx->TLF01_details.processing_code,
                                                          auth_tx->TLF01_details.voice_auth);
      OrigTxKey = orig_tx_key;
      if ( DB_Timing_Flag == true )
         start_time = ptetime_get_time();

      switch (orig_tx_key)
      {
         case AUTH_AUTHORIZATION_RESPONSE:
         case AUTH_PRE_AUTHORIZATION_RESPONSE:

            memset(&bch20,0,sizeof(BCH20));
            strcpy(bch20.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
            strcpy(bch20.primary_key.batch_nbr, "ACTIVE");
            strcpy(bch20.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
            strcpy(bch20.merchant_id,auth_tx->TLF01_details.merchant_id);

            if (db_delete_bch20_by_stan (&bch20,err_msg) != PTEMSG_OK)
            {
    			/* NewLog
     			 * */
				memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"BCH20 delete by stan failed,  device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_reversal_mp");
               result_code = PTEMSG_DELETE_FAILED;
            }


            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_DELETE, 2, BCH20_DATA,
                                    (float)duration, &TimingStats );
            }
            break;


         case AUTH_SALE_RESPONSE:
         case AUTH_REFUND_RESPONSE:
         case AUTH_CASH_ADVANCE_RESPONSE:
         case AUTH_OFFLINE_SALE_RESPONSE:
         case AUTH_QUASI_CASH_RESPONSE:

            result_code = process_reversal_non_void_trans_mp(auth_tx,orig_tx_key);
            memset(&bch10,0,sizeof(BCH10));
            strcpy(bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
            strcpy(bch10.primary_key.batch_nbr, "ACTIVE");
            strcpy(bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
            strcpy(bch10.merchant_id,auth_tx->TLF01_details.merchant_id);

            if (db_delete_bch10_by_stan (&bch10,err_msg) != PTEMSG_OK)
            {
    			/* NewLog
     			 * */
				memset(buffer,0x00,sizeof(buffer));
				sprintf(buffer,"BCH10 delete by stan failed, device id: %s"
					"merch id: %s, batch num: %s, stan: %s",
					auth_tx->TLF01_details.terminal_id,auth_tx->TLF01_details.merchant_id,
					auth_tx->TLF01_details.batch_number,auth_tx->TLF01_details.sys_trace_audit_num);
				updateds_log_message(2,2,buffer,"process_reversal_mp");
               result_code = PTEMSG_DELETE_FAILED;
            }

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_DELETE, 2, BCH10_DATA,
                                    (float)duration, &TimingStats );
            }
            break;

         case AUTH_VOID_SALE_RESPONSE:
         case AUTH_VOID_REFUND_RESPONSE:
         case AUTH_VOID_CASH_ADVANCE_RESPONSE:
         /* SCR 12502 - ThoughtFocus
		  * Adding the case statement, to enable REVERSAL of AUTH_VOID_REFUND
		  * AUTH_OFFLINE_VOID_SALE, AUTH_OFFLINE_VOID_REFUND,
		  * AUTH_SALE_ADJUSTMENT, AUTH_REFUND_ADJUSTMENT 
		  */
		 case AUTH_VOID_RELOAD_RESPONSE:
		 case AUTH_OFFLINE_VOID_SALE_RESPONSE:
		 case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
         /* SCR 12502 - ThoughtFocus
		  * Since AUTH_SALE_ADJUSTMENT, AUTH_REFUND_ADJUSTMENT function similar to
		  * VOIDs, process_reversal_void_trans is called for these transactions also.
		  */
		 case AUTH_SALE_ADJUSTMENT_RESPONSE:
         case AUTH_REFUND_ADJUSTMENT_RESPONSE:
            result_code = process_reversal_void_trans_mp(auth_tx);
            break;

         default: 
            sprintf( err_msg,
                    "Unknown Message Type: %s, Processing Code: %s (%s)",
                     auth_tx->TLF01_details.message_type,
                     auth_tx->TLF01_details.processing_code,
                     ServiceName );
            result_code = PTEMSG_INVALID_DATATYPE;
            break;
       }
   }
   return result_code;
}





pPTE_MSG process_insert_msg(pPTE_MSG p_msg_in)
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   BYTE           subtype2;
   BYTE           table_name[20]="";
   BYTE           rc;
   INT            len;
   CHAR           buffer[256];
   AUTH_TX        auth_tx;
   EMV_RAW        emvraw;
   double         start_time;
   double         end_time;
   double         duration;int itemp=0;
   BYTE           stip_result_code;
   CHAR			  amountbuff[13] = "";//praneeth added for converted amount swapping
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr=NULL_PTR;
   char			  last_tran__msg[256] = {0};
   char			  rspText[41] = {0};

   int serial_number_len=0;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data(p_msg_in);
   p_data        = ptemsg_get_pte_msg_data_data(p_msg_data);
   memcpy( atp_buffer, p_data, sizeof(AUTH_TX) );
   memcpy( &auth_tx,   p_data, sizeof(AUTH_TX) );
   p_current     = atp_buffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   p_data = p_data + sizeof(AUTH_TX);
   subtype2 = ptemsg_get_msg_subtype2( p_msg_in );

	/*************Last transaction to be updated in DB************/
	lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
	if(lastmsgptr != NULL_PTR)
	{
		sprintf( last_tran__msg,
			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
			   auth_tx.TLF01_details.card_num,
			   auth_tx.TLF01_details.sys_trace_audit_num,
			   auth_tx.TLF01_details.terminal_id,
			   auth_tx.TLF01_details.merchant_id,
			   auth_tx.TLF01_details.acquirer_id,
			   auth_tx.TLF01_details.handler_queue);

		Last_Message_len=strlen(last_tran__msg);

		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
		fclose(lastmsgptr);
	}


#ifdef MC12_OCT2023
	if( updatds_Check_If_MC_Transaction(&auth_tx) 			 &&
	   (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"ncmcrd",6)) &&
		updatds_check_if_txn_falls_under_merchant_ingerity_validation((pTLF01)p_data) &&
	   (0 != strncmp(auth_tx.TLF01_details.response_code,"19",2) &&
		GS_TXN_TIMEDOUT	!= auth_tx.TLF01_details.general_status	))
	{
		/* Timeout scenarios need not be counted for MC excessive decline count */
		result_code= process_merchant_integrity_transactions_for_mc(&auth_tx);
		/*Updating or inserting the merchant_ingerity_param should not block or
		 * decline the transaction
		 * */
		result_code =PTEMSG_OK;
	}

#endif

#ifdef MC13_OCT2023
	if( updatds_Check_If_MC_Transaction(&auth_tx) 			 			  &&
	    (0 == strncmp(auth_tx.TLF01_details.acquirer_id, "ncmcrd", 6)) &&
		updatds_check_if_txn_falls_under_mac_validation(&auth_tx) )
	{
		result_code= process_merchant_integrity_transactions_for_mac(&auth_tx);
		/*Updating or inserting the merchant_ingerity_param should not block or
		 * decline the transaction
		 * */
		result_code =PTEMSG_OK;
	}

#endif

#ifdef VISA7
    if( updatds_Check_If_VISA_Transaction((pTLF01)p_data) 			 &&
	    (0 == strncmp(auth_tx.TLF01_details.acquirer_id,"ncvisa",6)) &&
	    updatds_check_if_txn_falls_under_merchant_ingerity_validation((pTLF01)p_data))
    {
	    result_code= process_merchant_integrity_transactions((pTLF01)p_data);
	    /*Updating or inserting the merchant_ingerity_param should not block or
		 * decline the transaction
		 * */
	    result_code =PTEMSG_OK;
    }
#endif
   if (app_data_type  == TLF01_DATA)
   {
      /* Populate system_date field = date txn is inserted into database. */
      create_system_date( ((pTLF01)p_data)->system_date );

      memset(&db_recs.tlf01, 0, sizeof(db_recs.tlf01));
      memcpy(&db_recs.tlf01, (pTLF01)p_data, sizeof(TLF01));
/*	  memset(&db_recs.tlf01_stip,0, sizeof(db_recs.tlf01_stip));
	  memcpy(&db_recs.tlf01_stip, (pTLF01_STIP)p_data, sizeof(TLF01_STIP));*/

      /* Convert message type to a request type, for standardization. */
      convert_msgtype_to_request( db_recs.tlf01.message_type );

	  //response text retain for OMD Approval for ACQ. - Amith k
	  if((strncmp(db_recs.tlf01.response_code, "00", 2) 			== 0) &&
		 (strncmp(db_recs.tlf01.product_codes[18].code, "OMD", 3)   == 0) &&
		 ((strncmp(db_recs.tlf01.handler_queue, "dcpisoA", 7) == 0) ||
		  (strncmp(db_recs.tlf01.handler_queue, "VOICE", 5)   == 0)) 	  &&
		 ((strcmp(db_recs.tlf01.response_text, "APPROVED_STAND_IN") == 0) ||
		  (strcmp(db_recs.tlf01.response_text, "Approved") 		    == 0) ||
		  (strcmp(db_recs.tlf01.response_text, "") 	                == 0)))
	  {
		  if(strncmp(db_recs.tlf01.vehicle_number, "MID_BYPS", 8) == 0)
		  {
			  if(strlen(pos_res_txt) > 0)
				  strcpy(rspText, pos_res_txt);
			  else
				  strcpy(rspText, "Txn/Refund limit exceeded for merchant");
		  }
		  else if(strncmp(db_recs.tlf01.vehicle_number, "NBR_BYPS", 8) == 0)
		  {
			  if(strncmp(db_recs.tlf01.currency_code, "608", 3) == 0)
				  strcpy(rspText, "Fail PesoUsage(Nbr)chk for period");
			  else if(strncmp(db_recs.tlf01.currency_code, "840", 3) == 0)
				  strcpy(rspText, "Fail dollarUsage(Nbr)chk for period");
		  }
		  else if(strncmp(db_recs.tlf01.vehicle_number, "AMT_BYPS", 8) == 0)
		  {
			  if(strncmp(db_recs.tlf01.currency_code, "608", 3) == 0)
				  strcpy(rspText, "Fail PesoUsage(Amt)chk for period");
			  else if(strncmp(db_recs.tlf01.currency_code, "840", 3) == 0)
				  strcpy(rspText, "Fail dollarUsage(Amt)chk for period");
		  }
		  else if(strncmp(db_recs.tlf01.vehicle_number, "MCC_BYPS", 8) == 0)
		  {
			  strcpy(rspText, "Txn category usage check failed");
		  }

		  if(strlen(rspText) > 0)
		  {
			  memset(db_recs.tlf01.response_text, 0, sizeof(db_recs.tlf01.response_text));
			  memcpy(db_recs.tlf01.response_text, rspText, strlen(rspText));
		  }
	  }

      if ( subtype2 == ST2_DB_UPDATE_TC_TABLES_ONLY )
      {
         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update EMV01 and insert into TLF01. */
         result_code = db_process_tc_crypto( &db_recs.tlf01,
                                             &auth_tx.EMV_details,
                                              table_name,
                                              err_msg );
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, subtype2, EMV01_DATA,
                                 (float)duration, &TimingStats );
         }

         if ( result_code != PTEMSG_OK )
         {
            memset( buffer, 0x00, sizeof(buffer) );
            sprintf( buffer,
                    "Failed database update of TC Upload, Table=%s, RRN=%s",
                     table_name,
                     auth_tx.TLF01_details.retrieval_ref_num );

            updateds_log_message(2,2,buffer,"process_insert_msg");
            updateds_log_message(2,2,err_msg,"process_insert_msg");
         }
         else
         {
            /* RAW EMV DATA */
            memset( &emvraw, 0x00, sizeof(EMV_RAW) );

            /* Determine the block size */
            len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 ) + 2;

            /* Don't copy the length */
            memcpy( emvraw.RAW_DATA, auth_tx.EMV_details.emv_block, len );
            sprintf( emvraw.DATA_LENGTH, "%d", len );

            strcpy(emvraw.transaction_id, auth_tx.TLF01_details.primary_key.transaction_id);
            strcpy(emvraw.system_date,    ((pTLF01)p_data)->system_date );

            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            rc = db_insert_emvraw( &emvraw, err_msg );

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, subtype2, EMV01_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( rc != PTEMSG_OK )
            {
               memset( buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "TC Upload: Insert raw EMV data failed" );
               updateds_log_message(2,2,buffer,"process_insert_msg");
               updateds_log_message(2,2,err_msg,"process_insert_msg");
            }
         }
      }
      else
      {
       	  if (DB_NullCheck_Flag==1)
       	  {
       		  CheckNULLTerminatedinTLF01 (&db_recs.tlf01,MODE_INSERT);
       	  }
    	  if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         result_code = db_insert_tlf01(&db_recs.tlf01, err_msg);

		 if(((true == Updatds_Is_MCRefund_Transaction(&db_recs.tlf01))||
			(strcmp(db_recs.tlf01.response_code,"00") == 0 )) &&
			(strcmp(db_recs.tlf01.product_codes[1].code,STANDIN) == 0))
		 { 
			 strcpy(db_recs.tlf01.product_codes[7].code,auth_tx.ch_billing_amt);
			 strcpy(db_recs.tlf01.product_codes[7].amount,auth_tx.ch_billing_curr_code);
			 /* Store DE10 Billing Conversion rate in .TF Phani*/
			 strcpy(db_recs.tlf01.product_codes[8].amount, auth_tx.ch_billing_conv_rate);
			  	/* Store industry_code .TF Phani*/
			 strcpy(db_recs.tlf01.product_codes[8].code, auth_tx.industry_code);
			 /* TF phani - Updates for 0120 MCARD fields*/
			 /* DE 61 value - which need to be populated during 0120 from sbtach*/
			 strcpy(db_recs.tlf01.source_key,auth_tx.acceptor_term_name);
			  /* Store DE43 here so it gets logged to TLF01:
				*   Merchant Name         --> TLF01.card_holder_name
				*   City and Country Code --> TLF01.ncf01_retired_cwk
				*  TF Phani
				*/
			   if ( auth_tx.TLF01_details.card_holder_name[0] == 0x00 )
			   {
				  memcpy( auth_tx.TLF01_details.card_holder_name,
						  auth_tx.MCF01_details.name02,
						  22 );
			   }

			   if ( auth_tx.TLF01_details.ncf01_retired_cwk[0] == 0x00 )
			   {
				   memset(db_recs.tlf01.ncf01_retired_cwk,0x20,
						   sizeof(db_recs.tlf01.ncf01_retired_cwk)-1);
				  memcpy( auth_tx.TLF01_details.ncf01_retired_cwk,
						  auth_tx.MCF01_details.city,
						  strlen(auth_tx.MCF01_details.city));
				  itemp = strlen(auth_tx.MCF01_details.city);
				 // memset(&auth_tx.TLF01_details.ncf01_retired_cwk[itemp],0x20,13-itemp);
				  memcpy( auth_tx.TLF01_details.ncf01_retired_cwk+13, "PHL", 3 );
			   }

			   //Storing converted currency and amount while dual currency //praneeth
			   if ( 0 == strcmp(db_recs.tlf01.host_start_time, "7.000") )
			   {
					strcpy( db_recs.tlf01.currency_code, "840" );
					if (strcmp(db_recs.tlf01.product_codes[9].amount, "")) 
					{
						strcpy(amountbuff,db_recs.tlf01.total_amount);
						strcpy(db_recs.tlf01.total_amount,db_recs.tlf01.product_codes[9].amount);
						strcpy(db_recs.tlf01.product_codes[9].amount,amountbuff);
					}
			   }
			   else if ( 0 == strcmp(db_recs.tlf01.host_start_time, "5.000") )
			   {
   					strcpy( db_recs.tlf01.currency_code, "608" );
					if (strcmp(db_recs.tlf01.product_codes[9].amount, "") )
					{
						strcpy(amountbuff,db_recs.tlf01.total_amount);
						strcpy(db_recs.tlf01.total_amount,db_recs.tlf01.product_codes[9].amount);
						strcpy(db_recs.tlf01.product_codes[9].amount,amountbuff);
					}
			   }
			 /* Need to update STAND in flag in case of VOID / Reverse.. TF PHANI */
			 if( db_recs.tlf01.tx_key == AUTH_VOID_SALE_RESPONSE || db_recs.tlf01.tx_key == AUTH_VOID_SALE )
			 {
				 strcpy(db_recs.tlf01.product_codes[1].code,VOIDED);
				 stip_result_code = db_update_tlf01_stip_void(&db_recs.tlf01, err_msg);

			 }
#if BDOR_62_PREAUTH_VOID
			 else if(db_recs.tlf01.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE || db_recs.tlf01.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
			 {
				 strcpy(db_recs.tlf01.product_codes[1].code,VOIDED);
				 stip_result_code = db_update_tlf01_stip_void_preauth(&db_recs.tlf01, err_msg);
				 if(stip_result_code != PTEMSG_OK)
				 {
					 strcpy(err_msg, "");
					 stip_result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
				 }
			 }
#endif
			 else if(db_recs.tlf01.tx_key == AUTH_REVERSAL_RESPONSE  || db_recs.tlf01.tx_key == AUTH_REVERSAL)
			 {
			   	  if(0 != strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1) &&
			   	     0 != strncmp(auth_tx.TLF01_details.card_num,"7",1))//MP & Fleet should not update STIP table
				  {
			   		  /*Lets store a new entry in TLF01_STIP table for Partial Reversal*/
			   		  if(0==strncmp(db_recs.tlf01.product_codes[3].quantity,"PR",2))
			   		  {
			 			strncpy(db_recs.tlf01.product_codes[1].code,PARREVSAL,TRAN_STATUS_LEN);
			   			stip_result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
			   		  }
			   		  else
			   		  {
			   		  strcpy(db_recs.tlf01.product_codes[1].code,REVERSED);
			   		  stip_result_code = db_update_tlf01_stip_reversal(&db_recs.tlf01, err_msg);
				  }
				  }

			 }
			 else if(db_recs.tlf01.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE || db_recs.tlf01.tx_key == AUTH_SALE_ADJUSTMENT)
			 {
				 if(db_recs.tlf01.product_codes[6].quantity[0]      == 'R')
				 {
					 strcpy(db_recs.tlf01.product_codes[1].code,POSTED);
				 }
				 stip_result_code = db_update_tlf01_stip_adjust(&db_recs.tlf01, err_msg);
			 }
			 else
			 {
				 if(db_recs.tlf01.product_codes[6].quantity[0]      == 'R')
				 {
					 strcpy(db_recs.tlf01.product_codes[1].code,POSTED);
				 }
				stip_result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
			 }
		 }
		 else if(( strcmp(db_recs.tlf01.response_code,"95") == 0 ) &&
				  (strcmp(db_recs.tlf01.product_codes[1].code,STANDIN) == 0)&&
				  (strcmp(db_recs.tlf01.acquirer_id,"nciftr") == 0))
		 {
			stip_result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);				
		 }
			
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_INSERT, subtype2, TLF01_DATA,
                                 (float)duration, &TimingStats );
         }


         /* If EMV txn, insert into EMV01 table. */
         memcpy( &auth_tx, atp_buffer, sizeof(AUTH_TX) );

         if( strncmp(auth_tx.TLF01_details.handler_queue,"nceq",4) ==0 ||
        		 strncmp(auth_tx.TLF01_details.handler_queue,"neqi",4)==0 )
         {
        	 // Its ATM transaction no need to update
         }
         else if ( auth_tx.EMV_details.currency_code[0] != 0x00 )
		 {
			/* There is EMV data.  Insert it.
			 * But do not change result code from TLF01 insert.
			 */
			if ( DB_Timing_Flag == true )
			   start_time = ptetime_get_time();

			rc = store_emv_records( &auth_tx );

			if ( DB_Timing_Flag == true )
			{
			   end_time = ptetime_get_time();
			   duration = end_time - start_time;
			   update_timing_stats( ST1_DB_INSERT, subtype2, EMV01_DATA,
									(float)duration, &TimingStats );
			}

			/* Update Chip Condition Code in DCF01. */
			if ( DB_Timing_Flag == true )
			   start_time = ptetime_get_time();

			rc = update_chip_condition( &auth_tx );

			if ( DB_Timing_Flag == true )
			{
			   end_time = ptetime_get_time();
			   duration = end_time - start_time;
			   update_timing_stats( ST1_DB_UPDATE, subtype2, DCF01_DATA,
									(float)duration, &TimingStats );
			}
		 }


      }
   }
   else if (app_data_type  == FGUARD_DATA)
   {
		  result_code = db_insert_fguard( p_data, err_msg );
   }
   else
   {
      result_code = PTEMSG_INVALID_DATATYPE;
      sprintf(err_msg, "Unknown app_data_type for Insert request from %s , stan: %s", reply_que,auth_tx.TLF01_details.sys_trace_audit_num);
      updateds_log_message(2,2,err_msg,"process_insert_msg");
   }

   if(auth_tx.future_use_3[0]!='\0' &&
 		  0==strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3))
   {
	   memset( &db_recs.ptsn01, 0, sizeof( db_recs.ptsn01 ) );
 	  strcpy(db_recs.ptsn01.transaction_id,auth_tx.TLF01_details.primary_key.transaction_id);
 	  strcpy(db_recs.ptsn01.system_date,db_recs.tlf01.system_date);
 	  strncpy(db_recs.ptsn01.length_serial_no,auth_tx.future_use_3+2,2);
 	  strncpy(db_recs.ptsn01.total_length,auth_tx.future_use_3,2);
 	  strncpy(db_recs.ptsn01.table_id,auth_tx.future_use_3+4,2);
 	  serial_number_len=atoi(db_recs.ptsn01.total_length)-2;
 	  strncpy(db_recs.ptsn01.serial_number,auth_tx.future_use_3+6,serial_number_len);

 	  result_code =db_insert_ptsn01(&db_recs.ptsn01, err_msg);

 	  //dbcommon_commit();
   }


   if (result_code == PTEMSG_OK)
   {
      updates_to_db_made = true;
      p_msg_out = ptemsg_build_msg(MT_DB_REPLY,
                                   msg_sub_type1,
                                   msg_sub_type2,
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)atp_buffer,
                                   sizeof(AUTH_TX),
                                   app_data_type);

      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf(err_msg, "Insufficient Memory to build Insert response to %s ,  stan: %s", reply_que,auth_tx.TLF01_details.sys_trace_audit_num);
         updateds_log_message(2,2,err_msg,"process_insert_msg");
      }
   }
   return(p_msg_out);
}

/******************************************************************************
 *
 *  NAME:         genutil_format_date
 *
 *  DESCRIPTION:  This function formats a date into YYYYMMDD format.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      temp - Formatted date
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 ******************************************************************************/
void genutil_format_date(pCHAR temp)
{
   struct timeb   time_structure;// Time for ftime
   time_t long_time_since_1970;  // Num of secs since 01/01/1970 A.D. 12:00 A.M.
   struct tm *current_time;      // Current time structure
   CHAR   temp1[3] = "";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   sprintf( temp, "%d", current_time->tm_year );
   sprintf( temp1,"%02d", current_time->tm_mon  );
   strcat( temp, temp1 );

   memset(temp1, 0, sizeof(temp1));
   sprintf( temp1, "%02d", current_time->tm_mday );
   strcat( temp, temp1 );

   memset(temp1, 0, sizeof(temp1));
   sprintf( temp1, "%02d", current_time->tm_hour );
   strcat( temp, temp1 );

   memset(temp1, 0, sizeof(temp1));
   sprintf( temp1, "%02d", current_time->tm_min );
   strcat( temp, temp1 );

   memset(temp1, 0, sizeof(temp1));
   sprintf( temp1, "%02d", current_time->tm_sec );
   strcat( temp, temp1 );

   return;
} /* genutil_format_date */

/*****************************************************************************

  Function:   genutil_bcd_to_int

  Description:
      Convert a bcd string into an integer value.
  Author: 
      unknown
  Inputs:
      p_bcd_data  -  pointer to the incoming bcd string
      bcd_len     -  length of the bcd string
  Outputs:
  Return values:
      INT         -  integer value of the bcd string
  Modified by:
      BGable      6/26/98
******************************************************************************/
INT genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len)
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (p_bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (p_bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
} /*genutil_bcd_to_int */
void rollback_the_update()
{
   dbcommon_rollback();
   dstimer_rollback();
   updates_to_db_made = false;

   return;
}  

#ifdef VISA7
void prepare_merchant_intgerity_param_for_update(pTLF01 tlf01,pMERCHANT_INTEGRITY_PARAM pmerchant_intgerity_data,INT rc_category)
{
	int count,i_reset_days=0,i_allowable_reattempts=0;
	CHAR error_buf[256]={0};
	BYTE rc=0;
	BYTE start_date[9]={0}; BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;

	/*calculate the start and end date*/
	create_system_date(start_date );
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);

	if(rc_category == RC_CAT_1)
	{
		i_reset_days = atoi(reset_days_1);
		julian_end_date = i_reset_days+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(pmerchant_intgerity_data->c1_block_start_date,start_date,sizeof(pmerchant_intgerity_data->c1_block_start_date));
		strncpy(pmerchant_intgerity_data->c1_block_end_date,end_date,sizeof(pmerchant_intgerity_data->c1_block_end_date));
	}
	else if(rc_category == RC_CAT_2)
	{
		i_reset_days = atoi(reset_days_234);
		julian_end_date = i_reset_days+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(pmerchant_intgerity_data->c2_block_start_date,start_date,sizeof(pmerchant_intgerity_data->c2_block_start_date));
		strncpy(pmerchant_intgerity_data->c2_block_end_date,end_date,sizeof(pmerchant_intgerity_data->c2_block_end_date));
	}
	else if(rc_category == RC_CAT_3)
	{
		i_reset_days = atoi(reset_days_234);
		julian_end_date = i_reset_days+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(pmerchant_intgerity_data->c3_block_start_date,start_date,sizeof(pmerchant_intgerity_data->c3_block_start_date));
		strncpy(pmerchant_intgerity_data->c3_block_end_date,end_date,sizeof(pmerchant_intgerity_data->c3_block_end_date));
	}
	else
	{
		i_reset_days = atoi(reset_days_234);
		julian_end_date = i_reset_days+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(pmerchant_intgerity_data->c4_block_start_date,start_date,sizeof(pmerchant_intgerity_data->c4_block_start_date));
		strncpy(pmerchant_intgerity_data->c4_block_end_date,end_date,sizeof(pmerchant_intgerity_data->c4_block_end_date));
	}

	/* numeric or alpha numeri, 2 byte length, resposne and product_codes[13].quantity comparision,
	 * if same use resposne code only else use product_codes[13].quantity*/

	if ( tlf01->product_codes[13].quantity[0] != '\0' && 0 != strncmp(tlf01->response_code,tlf01->product_codes[13].quantity,2) &&
		 isalnum(tlf01->product_codes[13].quantity[0]) && isalnum(tlf01->product_codes[13].quantity[1]) && 2 == strlen(tlf01->product_codes[13].quantity))
	{
		strncpy(pmerchant_intgerity_data->last_declined_rc,tlf01->product_codes[13].quantity,2);
		sprintf(error_buf, "Response code(%s) and product_codes[13].quantity(%s) are not same", tlf01->response_code,tlf01->product_codes[13].quantity);
		updateds_log_message(2,2,err_msg,"prepare_merchant_intgerity_param_for_update");
	}
	else if (tlf01->response_code[0] != '\0' &&  isalnum(tlf01->response_code[0]) && isalnum(tlf01->response_code[1]) &&
			  2 == strlen(tlf01->response_code))
	{
		strncpy(pmerchant_intgerity_data->last_declined_rc,tlf01->response_code,2);
		sprintf(error_buf, "Response code(%s) and product_codes[13].quantity(%s) are same", tlf01->response_code,tlf01->product_codes[13].quantity);
		updateds_log_message(2,2,err_msg,"prepare_merchant_intgerity_param_for_update");
	}
	else
	{
		strncpy(pmerchant_intgerity_data->last_declined_rc,CATEGORY_CODE_REATTEMPT_REACHED,2);
		updateds_log_message(2,2,"Copied default RC 04" ,"prepare_merchant_intgerity_param_for_update");
	}
}
#endif
#ifdef VISA7
void prepare_merchant_intgerity_param_for_insert(pTLF01 tlf01,pMERCHANT_INTEGRITY_PARAM pmerchant_intgerity_data,INT rc_category)
{
	int count,i_reset_days=0,i_allowable_reattempts=0;
	CHAR error_buf[256]={0};
	BYTE rc=0;
	BYTE start_date[9]={0}; BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;

	memset(pmerchant_intgerity_data,0x00,sizeof(pmerchant_intgerity_data));

	memcpy(pmerchant_intgerity_data->merchant_id,tlf01->merchant_id,sizeof(pmerchant_intgerity_data->merchant_id));
	memcpy(pmerchant_intgerity_data->card_nbr,tlf01->card_num,sizeof(pmerchant_intgerity_data->card_nbr));
	memcpy(pmerchant_intgerity_data->category_cd,ALL_CATEGORY_CODE,sizeof(pmerchant_intgerity_data->category_cd));

	/*calculate the start and end date*/
	create_system_date(start_date );
	julian_start_date =  Txutils_Calculate_Julian_Date (start_date);
	if(rc_category == RC_CAT_1)
	{
		pmerchant_intgerity_data->c1_block[0]='Y';
		count=0;
		sprintf(pmerchant_intgerity_data->c1_count,"%02d",count);

		i_reset_days = atoi(reset_days_1);
		julian_end_date = i_reset_days+julian_start_date;
		Txutils_Calculate_Gregorian_Date(julian_end_date,end_date);

		strncpy(pmerchant_intgerity_data->c1_block_start_date,start_date,sizeof(pmerchant_intgerity_data->c1_block_start_date));
		strncpy(pmerchant_intgerity_data->c1_block_end_date,end_date,sizeof(pmerchant_intgerity_data->c1_block_end_date));

		/* numeric or alpha numeri , 2 digit, compare the response code and  product_codes[13].quantity
		 * if they are same use response code only else use product_codes[13].quantity
		 * printf
		 * */
		if ( tlf01->product_codes[13].quantity[0] != '\0' && 0 != strncmp(tlf01->response_code,tlf01->product_codes[13].quantity,2) &&
			 isalnum(tlf01->product_codes[13].quantity[0]) && isalnum(tlf01->product_codes[13].quantity[1]) && 2 == strlen(tlf01->product_codes[13].quantity))
		{
			strncpy(pmerchant_intgerity_data->last_declined_rc,tlf01->product_codes[13].quantity,2);
			sprintf(error_buf, "Response code(%s) and product_codes[13].quantity(%s) are not same", tlf01->response_code,tlf01->product_codes[13].quantity);
		    updateds_log_message(2,2,err_msg,"prepare_merchant_intgerity_param_for_insert");
		}
		else if (tlf01->response_code[0] != '\0' &&  isalnum(tlf01->response_code[0]) && isalnum(tlf01->response_code[1]) &&
		         2 == strlen(tlf01->response_code))
		{
			strncpy(pmerchant_intgerity_data->last_declined_rc,tlf01->response_code,2);
			sprintf(error_buf, "Response code(%s) and product_codes[13].quantity(%s) are same", tlf01->response_code,tlf01->product_codes[13].quantity);
			updateds_log_message(2,2,err_msg,"prepare_merchant_intgerity_param_for_insert");
		}
		else
		{
			strncpy(pmerchant_intgerity_data->last_declined_rc,CATEGORY_CODE_REATTEMPT_REACHED,2);
			updateds_log_message(2,2,"Copied default RC 04" ,"prepare_merchant_intgerity_param_for_insert");
		}
	}
	else if(rc_category == RC_CAT_2)
	{
		pmerchant_intgerity_data->c2_block[0]='N';
		count=0;
		sprintf(pmerchant_intgerity_data->c2_count,"%02d",count);
	}
	else if(rc_category == RC_CAT_3)
	{
		pmerchant_intgerity_data->c3_block[0]='N';
		count=0;
		sprintf(pmerchant_intgerity_data->c3_count,"%02d",count);
	}
	else
	{
		pmerchant_intgerity_data->c4_block[0]='N';
		count=0;
		sprintf(pmerchant_intgerity_data->c4_count,"%02d",count);
	}
}
#endif
#ifdef VISA7
BYTE process_merchant_integrity_transactions (pTLF01 tlf01)
{
	CHAR error_buf[256]={0};
    CHAR merchant_integrity_key[41]={0};
	BYTE rc=PTEMSG_OK;
	INT  rc_category=0;
	INT  rc_count=0;
	INT  i_allowable_reattempts=0;
	BYTE start_date[9]={0};
	BYTE end_date[9]={0};
	LONG julian_start_date=0,julian_end_date=0;
	CHAR resp_code[3]={0};
	CHAR current_system_date[9]={0};
	CHAR local_reattempts_date[7], temp_date_yyyymmdd[9]={0};
	INT  reattempts_date_len = 0;
	char month_from_reattempt_date[3]={0}, currentMonth[3]={0}, currentYear[5]={0};
    MCF01 MCF01_details;

	/* get the latest config details if the details are updated in the merchant_integrity.ini file*/
	create_system_date(current_system_date );
	if(0 != memcmp(current_system_date,last_data_update_date,sizeof(current_system_date)-1))
	{
		/* get only last updated date ,
		 * the last update date should be equal to current system date, */
		updatds_read_last_updated_date_parameter();
		if(0 == memcmp(current_system_date,last_data_update_date,sizeof(current_system_date)-1))
		{
			updatds_read_system_integrity_parameters();
		}
	}

	memset(&db_recs.merchant_intgerity_data, 0x00, sizeof(db_recs.merchant_intgerity_data));
	memcpy(merchant_integrity_key,tlf01->merchant_id, sizeof(tlf01->merchant_id));
	memcpy(merchant_integrity_key + sizeof(tlf01->merchant_id), tlf01->card_num, sizeof(tlf01->card_num));
	memcpy(merchant_integrity_key + sizeof(tlf01->card_num)+sizeof(tlf01->merchant_id), ALL_CATEGORY_CODE, 4);

    if(0 == strncmp(tlf01->response_code,APPROVED,2))
    {
		db_delete_merchant_integrity_param_for_cat1(merchant_integrity_key,error_buf);
		//memset cat1 data when it is having some count in other categories
		db_update_merchant_integrity_param_for_cat1(merchant_integrity_key,error_buf);

		db_delete_merchant_integrity_param_for_cat_2_3_4(merchant_integrity_key,error_buf);
    }
	else
	{
		/*numeric or alpha numeric, length , if response code and product_codes[13].quantity are same use response code only,
		 * else use product_codes[13].quantity
		 * */

		if ( tlf01->product_codes[13].quantity[0] != '\0' && 0 != strncmp(tlf01->response_code,tlf01->product_codes[13].quantity,2) &&
			 isalnum(tlf01->product_codes[13].quantity[0]) && isalnum(tlf01->product_codes[13].quantity[1]) && 2 == strlen(tlf01->product_codes[13].quantity))
		{
			strncpy(resp_code,tlf01->product_codes[13].quantity,2);
			sprintf(error_buf, "Response code(%s) and product_codes[13].quantity(%s) are not same", tlf01->response_code,tlf01->product_codes[13].quantity);
			updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions");
		}
		else if (tlf01->response_code[0] != '\0' &&  isalnum(tlf01->response_code[0]) && isalnum(tlf01->response_code[1]) &&
				 2 == strlen(tlf01->response_code))
		{
			strncpy(resp_code,tlf01->response_code,2);
			sprintf(error_buf, "Response code(%s) and product_codes[13].quantity(%s) are same", tlf01->response_code,tlf01->product_codes[13].quantity);
			updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions");
		}

		rc_category = updatds_check_RC_category(resp_code);
		//now cat3, cat4 needs to be defined
		if((rc_category == RC_CAT_1 && cat1_MID_integrity_validation_flag =='Y') ||
		   (rc_category == RC_CAT_2 && cat2_MID_integrity_validation_flag =='Y') ||
		   (rc_category == RC_CAT_3 && cat3_MID_integrity_validation_flag =='Y') ||
		   (rc_category == RC_CAT_4 && cat4_MID_integrity_validation_flag =='Y'))
		{
			i_allowable_reattempts=atoi(allowable_reattempts_234);

			rc=db_select_merchant_integrity_param(merchant_integrity_key,&db_recs.merchant_intgerity_data,error_buf);
			if(rc == PTEMSG_OK)
			{
				if(rc_category == RC_CAT_1)
				{
					if( db_recs.merchant_intgerity_data.c1_block[0] != 'Y')
					{
						db_recs.merchant_intgerity_data.c1_block[0] = 'Y';
						prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,rc_category);
					}
				}
				else if(rc_category == RC_CAT_2)
				{
					rc_count=0;
					if( atoi(db_recs.merchant_intgerity_data.c2_count) >= 0 )
					{
						rc_count=atoi(db_recs.merchant_intgerity_data.c2_count);
						rc_count++;
					}
					else
					{
						rc_count = 0;
					}

					if(rc_count < i_allowable_reattempts)
					{
						db_recs.merchant_intgerity_data.c2_block[0]='N';
						sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",rc_count);
						reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
						if( reattempts_date_len == 0)
						{
							sprintf(local_reattempts_date,"%4s;",current_system_date+4);
							strncpy(db_recs.merchant_intgerity_data.reattempts_date,local_reattempts_date,strlen(local_reattempts_date));
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date,current_system_date,8);
						}
						else
						{
							sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
							strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));
						}

						rc_count = 0;//increment c3 count as well if exists
						if(db_recs.merchant_intgerity_data.c3_count[0] != '\0' &&
								cat3_MID_integrity_validation_flag =='Y')
						{
							rc_count=atoi(db_recs.merchant_intgerity_data.c3_count);
							rc_count++;
							sprintf(db_recs.merchant_intgerity_data.c3_count,"%02d",rc_count);

							if(rc_count >= i_allowable_reattempts)
							{
									db_recs.merchant_intgerity_data.c3_block[0]='Y';
									prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,3);
									rc_category=3;
							}
						}

						rc_count=0;//increment c4 count as well if exists
						if(db_recs.merchant_intgerity_data.c4_count[0] != '\0' &&
								cat4_MID_integrity_validation_flag =='Y')
						{
							rc_count=atoi(db_recs.merchant_intgerity_data.c4_count);
							rc_count++;
							sprintf(db_recs.merchant_intgerity_data.c4_count,"%02d",rc_count);

							if(rc_count >= i_allowable_reattempts)
							{
									db_recs.merchant_intgerity_data.c4_block[0]='Y';
									prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,4);
									rc_category=4;
							}
						}
					}
					else
					{
						db_recs.merchant_intgerity_data.c2_block[0]='Y';
						sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",rc_count);
						sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
						reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
						strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));
						prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,rc_category);
					}
				}
				else if(rc_category == RC_CAT_3)
				{
					if( atoi(db_recs.merchant_intgerity_data.c3_count) >= 0 )
					{
						rc_count=atoi(db_recs.merchant_intgerity_data.c3_count);
						rc_count++;
					}
					else
					{
						rc_count = 0;
					}

					if(rc_count < i_allowable_reattempts)
					{
						db_recs.merchant_intgerity_data.c3_block[0]='N';
						sprintf(db_recs.merchant_intgerity_data.c3_count,"%02d",rc_count);
						reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
						if( reattempts_date_len == 0)
						{
							sprintf(local_reattempts_date,"%4s;",current_system_date+4);
							strncpy(db_recs.merchant_intgerity_data.reattempts_date,local_reattempts_date,strlen(local_reattempts_date));
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date,current_system_date,8);
						}
						else
						{
							sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
							strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));
						}

						rc_count = 0;//increment c2 count as well if exists
						if(db_recs.merchant_intgerity_data.c2_count[0] != '\0' &&
								cat2_MID_integrity_validation_flag =='Y')
						{
							rc_count=atoi(db_recs.merchant_intgerity_data.c2_count);
							rc_count++;
							sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",rc_count);

							if(rc_count >= i_allowable_reattempts)
							{
									db_recs.merchant_intgerity_data.c2_block[0]='Y';
									prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,2);
									rc_category=2;
							}
						}

						rc_count=0;//increment c4 count as well if exists
						if(db_recs.merchant_intgerity_data.c4_count[0] != '\0' &&
								cat4_MID_integrity_validation_flag =='Y')
						{
							rc_count=atoi(db_recs.merchant_intgerity_data.c4_count);
							rc_count++;
							sprintf(db_recs.merchant_intgerity_data.c4_count,"%02d",rc_count);

							if(rc_count >= i_allowable_reattempts)
							{
								db_recs.merchant_intgerity_data.c4_block[0]='Y';
								prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,4);
								rc_category=4;
							}
						}
					}
					else
					{
						db_recs.merchant_intgerity_data.c3_block[0]='Y';
						sprintf(db_recs.merchant_intgerity_data.c3_count,"%02d",rc_count);
						reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
						sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
						strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));
						prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,rc_category);
					}
				}
				else if(rc_category == RC_CAT_4)
				{
					if( atoi(db_recs.merchant_intgerity_data.c4_count) >= 0 )
					{
						rc_count=atoi(db_recs.merchant_intgerity_data.c4_count);
						rc_count++;
					}
					else
					{
						rc_count = 0;
					}

					if(rc_count < i_allowable_reattempts)
					{
						db_recs.merchant_intgerity_data.c4_block[0]='N';
						sprintf(db_recs.merchant_intgerity_data.c4_count,"%02d",rc_count);
						reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
						if( reattempts_date_len == 0)
						{
							sprintf(local_reattempts_date,"%4s;",current_system_date+4);
							strncpy(db_recs.merchant_intgerity_data.reattempts_date,local_reattempts_date,strlen(local_reattempts_date));
							strncpy(db_recs.merchant_intgerity_data.first_reattempt_date,current_system_date,8);
						}
						else
						{
							sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
							strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));
						}

						rc_count=0;//increment c2 count as well if exists
						if(db_recs.merchant_intgerity_data.c2_count[0] != '\0' &&
								cat2_MID_integrity_validation_flag =='Y')
						{
							rc_count=atoi(db_recs.merchant_intgerity_data.c2_count);
							rc_count++;
							sprintf(db_recs.merchant_intgerity_data.c2_count,"%02d",rc_count);

							if(rc_count >= i_allowable_reattempts)
							{
									db_recs.merchant_intgerity_data.c2_block[0]='Y';
									prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,2);
									rc_category=2;
							}
						}

						rc_count = 0;//increment c3 count as well if exists
						if(db_recs.merchant_intgerity_data.c3_count[0] != '\0' &&
								cat3_MID_integrity_validation_flag =='Y')
						{
							rc_count=atoi(db_recs.merchant_intgerity_data.c3_count);
							rc_count++;
							sprintf(db_recs.merchant_intgerity_data.c3_count,"%02d",rc_count);

							if(rc_count >= i_allowable_reattempts)
							{
									db_recs.merchant_intgerity_data.c3_block[0]='Y';
									prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,3);
									rc_category=3;
							}
						}
					}
					else
					{
						db_recs.merchant_intgerity_data.c4_block[0]='Y';
						sprintf(db_recs.merchant_intgerity_data.c4_count,"%02d",rc_count);
						reattempts_date_len = strlen(db_recs.merchant_intgerity_data.reattempts_date);
						sprintf(local_reattempts_date,"|%4s;",current_system_date+4);
						strncpy(&db_recs.merchant_intgerity_data.reattempts_date[reattempts_date_len-1],local_reattempts_date,strlen(local_reattempts_date));
						prepare_merchant_intgerity_param_for_update(tlf01,&db_recs.merchant_intgerity_data,rc_category);
					}
				}
				/*
				 * first 4 bytes of reattempt date and last 4 byte of first reattempt date should be same,
				 * else take first 4 bytes of reattempt date append year and copy into first reattempt date
				 * */
				if(0 != strncmp(db_recs.merchant_intgerity_data.reattempts_date,db_recs.merchant_intgerity_data.first_reattempt_date+4,4))
				{
					/* add*/
					sprintf(error_buf, "first 4 byte of reattempt date(%.4s) and first_reattempt_date(%s) are not same ",db_recs.merchant_intgerity_data.reattempts_date,
							db_recs.merchant_intgerity_data.first_reattempt_date);
					updateds_log_message(2,2,error_buf,"prepare_merchant_intgerity_param_for_insert");

					memset(db_recs.merchant_intgerity_data.first_reattempt_date,0x00,sizeof(db_recs.merchant_intgerity_data.first_reattempt_date));

					 memcpy(currentYear,  current_system_date,   4);
					 memcpy(currentMonth, current_system_date+4, 2);
					 strncpy(month_from_reattempt_date,db_recs.merchant_intgerity_data.reattempts_date,2);
					if(atoi(month_from_reattempt_date) <= atoi(currentMonth))
					{
						strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, currentYear, strlen(currentYear));
						strncat(db_recs.merchant_intgerity_data.first_reattempt_date, db_recs.merchant_intgerity_data.reattempts_date, 4);
					}
					else
					{
						sprintf(temp_date_yyyymmdd, "%d", atoi(currentYear)-1);
						strncat(temp_date_yyyymmdd, db_recs.merchant_intgerity_data.reattempts_date, 4);
						strncpy(db_recs.merchant_intgerity_data.first_reattempt_date, temp_date_yyyymmdd, 8);
					}
				}

				//update all
				rc = db_update_merchant_integrity_param(&db_recs.merchant_intgerity_data,error_buf,rc_category);
			}
			else
			{
				/*Prepare_merchant_intgerity_param_for_insert*/
				prepare_merchant_intgerity_param_for_insert(tlf01,&db_recs.merchant_intgerity_data,rc_category);
				rc = db_insert_merchant_integrity_param(&db_recs.merchant_intgerity_data,error_buf,rc_category);
			}

			if( db_recs.merchant_intgerity_data.c1_block[0] == 'Y' ||
			    db_recs.merchant_intgerity_data.c2_block[0] == 'Y' ||
				db_recs.merchant_intgerity_data.c3_block[0] == 'Y' ||
				db_recs.merchant_intgerity_data.c4_block[0] == 'Y')
			{
				memset(&MCF01_details,0x00,sizeof(MCF01_details));
				strncpy(MCF01_details.primary_key.merchant_id,tlf01->merchant_id,sizeof(MCF01_details.primary_key.merchant_id));
				strncpy(MCF01_details.primary_key.organization_id,"0000",sizeof(MCF01_details.primary_key.organization_id));
				rc = db_update_mcf01(&MCF01_details,error_buf);
			}
		}
		else
		{
			sprintf(err_msg,"Category code %d resp_code:%s, rc validation disabled, txn details,card_num:%s,stan:%s,mid:%s",rc_category,resp_code,
			   	    tlf01->card_num,tlf01->sys_trace_audit_num,tlf01->merchant_id );
			updateds_log_message(2,2,err_msg,"process_merchant_integrity_transactions");
		}
	}

	return rc;
}
#endif

BYTE process_network_request ( pAUTH_TX auth_tx , BYTE msg_sub_type2 )
{
   NCF30          ncf30;
   CHAR           date_time[16] = "";
   LONG           total;
   LONG           grand_total;
   CHAR           ncf21_open_date[9]="";
   CHAR           ncf21_open_time[7]="";
   CHAR           proc_code[7];
   CHAR           ncf21_keys[50] = "";
   CHAR           network_id[11] = "";
   CHAR           type[2] = "";
   INT            cnt;
   INT            len;
   BOOLEAN        cirrus   = false;
   BOOLEAN        debit    = false;
   BOOLEAN        credit   = false;
   BOOLEAN        both     = false;
   BOOLEAN        reversal = false;
   BOOLEAN        new      = false;
   double         start_time;
   double         end_time;
   double         duration;
   PAD_RAW        pad_raw_data;
   BYTE ret_code=0;
   char current_system_date[9]={0};

   /* Set up keys for NCF21 record. */
   if ( msg_sub_type2 == ST2_ACQUIRER )
   {
      len = strlen (auth_tx->TLF01_details.handler_queue);
      strncpy( network_id, auth_tx->TLF01_details.handler_queue,
            len - 1 );

      strcat( type, "A"  );
   }
   else    if ( msg_sub_type2 == ST2_ISSUER )
   {
      strncpy( network_id, auth_tx->TLF01_details.authorizing_host_queue,
            sizeof(auth_tx->TLF01_details.authorizing_host_queue) - 2 );
      strcat( type, "I" );

   }

   if ( 0 == strcmp("VOICE",auth_tx->TLF01_details.handler_queue) )
   {
      strcpy( network_id, auth_tx->TLF01_details.issuer_id );
      type[0] = auth_tx->TLF01_details.entry_type;
      msg_sub_type2 = ST2_ACQUIRER;
   }

   /* Determine if Reversal */
   if ( (0 == strncmp( "04", auth_tx->TLF01_details.message_type, 2)) ||
        ( auth_tx->TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE)  )
      reversal = true;
   else if ( 0 == strncmp(auth_tx->BIN01_details.card_type,"CUP",3)&&
   			 (auth_tx->TLF01_details.tx_key ==AUTH_REVERSAL_RESPONSE))
   	{
   		reversal = true;
   	}
   	
	/* Get system date and time for NCF30 record. */
	memset( &db_recs.ncf21, 0x00, sizeof(NCF21) );
	genutil_format_date( date_time );
	memcpy(db_recs.ncf21.open_date ,date_time, 8);
	memcpy(db_recs.ncf21.open_time ,date_time + 8, 6);
	result_code = PTEMSG_OK;

   /* Insert record into NCF30 */
   if ( PTEMSG_OK == result_code )
   {
      memset ( &ncf30, 0, sizeof ( NCF30 ) );
      strcpy ( ncf30.primary_key.network_id, network_id );
      strcpy ( ncf30.primary_key.network_type, type );
      strcpy ( ncf30.primary_key.retrieval_ref_num, auth_tx->TLF01_details.retrieval_ref_num );
      strcpy ( ncf30.card_nbr, auth_tx->TLF01_details.card_num );
      strcpy ( ncf30.primary_key.batch_number, "ACTIVE" );

      if ( reversal && (msg_sub_type2 == ST2_ACQUIRER))
      {
    	  if((strncmp(auth_tx->TLF01_details.handler_queue,"ncmcrd",6) == 0 ||
    		  strncmp(auth_tx->TLF01_details.handler_queue,"ncmcr3",6) == 0)||
    	     strncmp(auth_tx->TLF01_details.handler_queue,"ncjcb",5) == 0 	  )
    	  {/* will include all ncmcrd and ncmcr3*/
    		  // Do nothing.
    	  }
    	  else if (true == updatds_Check_If_Diners_Transaction(auth_tx))
    	  {
    		  if ( DB_Timing_Flag == true )
    		  {
    			  start_time = ptetime_get_time();
    		  }
    		  result_code = db_delete_ncf30_DCI( &ncf30, err_msg );
    		  if ( DB_Timing_Flag == true )
			  {
				  end_time = ptetime_get_time();
				  duration = end_time - start_time;
				  update_timing_stats( ST1_DB_DELETE, 3, NCF30_DATA,
									   (float)duration, &TimingStats );
			  }
    	  }
    	  else
    	  {
    		  strcpy ( ncf30.primary_key.retrieval_ref_num, auth_tx->TLF01_details.orig_retrieval_ref_num );

    		  if ( DB_Timing_Flag == true )
    		  {
    			  start_time = ptetime_get_time();
    		  }

    		  result_code = db_delete_ncf30( &ncf30, err_msg );

    		  if ( DB_Timing_Flag == true )
    		  {
    			  end_time = ptetime_get_time();
    			  duration = end_time - start_time;
    			  update_timing_stats( ST1_DB_DELETE, 3, NCF30_DATA,
                                 	   (float)duration, &TimingStats );
    		  }
    	  }
      }
      else
      {
#ifdef AMEX03A
    	  if(0 == strncmp(auth_tx->TLF01_details.handler_queue,"ncpos",5) &&
    	     (0== strncmp(auth_tx->function_code,"101",3) ||
    	      0== strncmp(auth_tx->function_code,"106",3) ||
			  0== strncmp(auth_tx->function_code,"401",3)))
    	  { /* Function code defines the type of transaction
    	     101: estimated authorization
    	     106: Incremental authorization
    	     401: Partial reversal
    	    */
    		  if( 0== strncmp(auth_tx->function_code,"106",3))
    		  {
        		  strncpy ( ncf30.tran_fee_amount, auth_tx->TLF01_details.visa_merchant_id,9 );
        		  strcpy ( ncf30.open_time, auth_tx->TLF01_details.visa_merchant_id+9, 6 );
    		  }
    		  else
    		  {
        		  strncpy ( ncf30.tran_fee_amount, auth_tx->TLF01_details.visa_tran_id,9 );
        		  strcpy ( ncf30.open_time, auth_tx->TLF01_details.visa_tran_id+9, 6 );
    		  }

    		  if( 0== strncmp(auth_tx->function_code,"101",3))
    		  {
    			  strncpy ( ncf30.open_date, "PRIMARY", 7 );
    		  }
    		  else if(0== strncmp(auth_tx->function_code,"106",3))
    		  {
    			  strncpy ( ncf30.open_date, "INCREMNT", 8 );
    		  }
    		  else if(0== strncmp(auth_tx->function_code,"401",3))
    		  {
    			  strncpy ( ncf30.open_date, "PARTIAL", 7);
    		  }
    	  }
#endif
    	  else
    	  {
    	         strcpy ( ncf30.open_date, db_recs.ncf21.open_date );
    	         strcpy ( ncf30.open_time, db_recs.ncf21.open_time );
    	         strcpy ( ncf30.tran_fee_amount, auth_tx->TLF01_details.tran_fee_amount );
    	  }

         strcpy ( ncf30.merchant_id, auth_tx->TLF01_details.merchant_id );
         strcpy ( ncf30.transaction_id, auth_tx->TLF01_details.primary_key.transaction_id );
         strcpy ( ncf30.device_id , auth_tx->TLF01_details.terminal_id );
         strcpy ( ncf30.card_nbr, auth_tx->TLF01_details.card_num );
         strcpy ( ncf30.message_type, auth_tx->TLF01_details.message_type );

#ifdef AMEX03A
         if((0 == strncmp(auth_tx->TLF01_details.handler_queue,"ncpos",5) &&
			 (0== strncmp(auth_tx->function_code,"101",3) ||
			  0== strncmp(auth_tx->function_code,"106",3) ||
			  0== strncmp(auth_tx->function_code,"401",3))))
         {
        	  if( 0== strncmp(auth_tx->function_code,"101",3) || 0== strncmp(auth_tx->function_code,"106",3))
			  {
        		 strncpy ( ncf30.processing_code, auth_tx->TLF01_details.product_codes[17].amount,6 );
			  }
			  else if(0== strncmp(auth_tx->function_code,"401",3))
			  {
				  strncpy ( ncf30.processing_code, auth_tx->TLF01_details.product_codes[17].amount ,6);
			  }
         }
#endif
         else
         {
             strcpy ( ncf30.processing_code, auth_tx->TLF01_details.processing_code );
         }

         if( strcmp("",auth_tx->TLF01_details.reversal_amount) == 0)
            strcpy( ncf30.tran_amount, auth_tx->TLF01_details.total_amount );
         else
            strcpy( ncf30.tran_amount, auth_tx->TLF01_details.reversal_amount ); 

         strcpy ( ncf30.auth_number, auth_tx->TLF01_details.auth_number );
         strcpy ( ncf30.response_code, auth_tx->TLF01_details.response_code );
         strcpy ( ncf30.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
         
         
         strcpy ( ncf30.tran_time, auth_tx->TLF01_details.time_hhmmss );
         strcpy ( ncf30.sys_trace_audit_nbr, auth_tx->TLF01_details.sys_trace_audit_num );
			if(auth_tx->TLF01_details.tx_key >= AUTH_QUASI_CASH_RESPONSE)
			ncf30.tx_key = (auth_tx->TLF01_details.tx_key - 100);
			else
			ncf30.tx_key = auth_tx->TLF01_details.tx_key;

         strcpy ( ncf30.invoice_nbr, auth_tx->TLF01_details.invoice_number );
		 /* store STIP approved txn as stndin in invoice no. 
		 Its usefull when aTP get Vodi/Reversal/Adjust txn for that. TF-Ajay 08-Jul-09 */
		 strcpy ( ncf30.invoice_nbr,auth_tx->TLF01_details.product_codes[1].code);
		 if((0 == updatds_Check_If_CUP_Transaction(auth_tx))&&
		    (0 == strncmp(auth_tx->TLF01_details.handler_queue,"nccup",5)))
		 {
			 strncpy ( ncf30.network_data, auth_tx->ch_billing_amt,12 );
		 }
#ifdef AMEX03A
		 else if(0 == strncmp(auth_tx->TLF01_details.handler_queue,"ncpos",5) &&
				 0 == strncmp(auth_tx->function_code,"101",3))
		 {
			 strncpy ( ncf30.network_data, auth_tx->TLF01_details.total_amount,12 );
		 }
#endif
		 else
		 {
			 strncpy( ncf30.network_data, auth_tx->TLF01_details.mcard_banknet,12 );
		 }
		 strcpy ( ncf30.currency_code, auth_tx->TLF01_details.currency_code );
         strcpy ( ncf30.acquiring_inst_id_code, auth_tx->TLF01_details.acquiring_id );
         strcpy ( ncf30.settlement_date, auth_tx->TLF01_details.settlement_date );
         strcpy ( ncf30.transmission_timestamp, auth_tx->TLF01_details.transmission_timestamp );

         if( ncf30.primary_key.retrieval_ref_num[0] == 0x00)
         {
            create_rrn( ncf30.primary_key.retrieval_ref_num );
         }

         /* Convert message type to a request type, for standardization. */
         convert_msgtype_to_request( ncf30.message_type );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF30 (&db_recs.ncf30,MODE_INSERT);
		  }
         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         result_code = db_insert_ncf30( &ncf30, err_msg );

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_INSERT, 3, NCF30_DATA,
                                 (float)duration, &TimingStats );
         }


         /*
          * For ONLINE processed incremental and partial reversal transaction
          * do the amount calculation in updatds and update the details into estimated authorization NCF30
          * */
#ifdef AMEX03A
        if(0 != strncmp(auth_tx->TLF01_details.product_codes[1].code,"stndin",6) &&
          (updatds_check_for_amex_incremental_transaction(auth_tx) ||
           updatds_check_for_amex_partial_reversal_transaction(auth_tx)))
        {
				create_system_date(current_system_date );
				if(0 != memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
				{
					/* get only last updated date ,
					 * the last update date should be equal to current system date, */
					get_last_dated_date_only();
					if(0 == memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
					{
						get_variable_auth_config_details();
					}
				}
            	 memset(&ncf30,0x00,sizeof(NCF30));

            	 strncpy( ncf30.card_nbr, auth_tx->TLF01_details.card_num,atoi(auth_tx->TLF01_details.card_num_len));
         		 strncpy( ncf30.merchant_id, auth_tx->TLF01_details.merchant_id,15);
         		 strncpy( ncf30.tran_amount, auth_tx->TLF01_details.orig_amount,12);

         		if(true == updatds_check_for_amex_incremental_transaction(auth_tx))
         		{
    	        	strncpy( ncf30.tran_fee_amount, auth_tx->TLF01_details.visa_merchant_id,9);
    	        	strncpy( ncf30.open_time, auth_tx->TLF01_details.visa_merchant_id+9,6);
    	        	/* send the transcation amount, its required to keep the total amount details*/
    	        	strncpy( ncf30.network_data, auth_tx->TLF01_details.total_amount,12);
    	        	ret_code= db_select_and_update_ncf30_for_Incrementalauthorization(&ncf30,err_msg);
         		}
         		else
         		{
    	        	strncpy( ncf30.tran_fee_amount, auth_tx->TLF01_details.visa_tran_id,9);
    	        	strncpy( ncf30.open_time, auth_tx->TLF01_details.visa_tran_id+9,6);
    	        	/* send the actual transaction amount of partial reversal, its required to keep the total amount details*/
    	        	strncpy( ncf30.network_data, auth_tx->TLF01_details.actual_amount,12);
    	        	ret_code=db_select_and_update_ncf30_for_Partialreversalauthorization(&ncf30,err_msg);
         		}

         		if(ret_code == PARTIAL_AMOUNT_EXCEEDED_TOTAL_AMOUNT)
         		{
         			 updateds_log_message(2,2,"Partial reversal amount is greater than the earmarked amount, NCF30 not updated","process_network_request");
         		}
         		else if(ret_code == TOTAL_AMOUNT_EMPTY)
         		{
         			updateds_log_message(2,2,"Total amount is not available in the estimated auth NCF30 and  NCF30 not updated","process_network_request");
         		}

        }
#endif
      }
      if((0 == strncmp(auth_tx->TLF01_details.product_codes[0].quantity,"56",2))&&
    	 (0 == strncmp(auth_tx->TLF01_details.handler_queue,"ncvis",5)))
      {
    	  // Lets us check if PAD records is there to insert.
    	  if(auth_tx->future_use_1[0]> 0)
    	  {
    		  INT temp_len = 0;
			  temp_len = auth_tx->future_use_1[0];
			  if ( temp_len > 0)
			  {
				  memset( &pad_raw_data, 0x00, sizeof(pad_raw_data) );
				  memcpy( pad_raw_data.RAW_DATA, auth_tx->future_use_1 + 1 , temp_len );
				  sprintf( pad_raw_data.DATA_LENGTH, "%d", temp_len );
				  strcpy(pad_raw_data.transaction_id, auth_tx->TLF01_details.primary_key.transaction_id);
				  strcpy(pad_raw_data.system_date,    auth_tx->TLF01_details.system_date );
				  db_insert_pad_raw( &pad_raw_data, err_msg);
   }
    	  }
      }
   }

   /* Rollback when any query fails ( either NCF21 / NCF30 ) */
   if ( result_code != PTEMSG_OK )
   {
      updates_to_db_made = true;
      rollback_the_update();
   }
   return ( result_code );
}

BYTE process_alldata_msg( pAUTH_TX p_auth_tx )
{
   TLF01          tlf01, tlf01_temp;
   CHAR           primary_key[50] = "";
   CHAR           transaction_id [21] = "";
   CHAR           date_time[16] = "";
   CHAR           bch01_open_date[9] = "";
   CHAR           bch01_open_time[7] = "";
   BYTE           deferred_txn;
   BYTE           rc;
   INT rc_category=0;
   double         start_time;
   double         end_time;
   double         duration;
   int			  itemp;		
   int serial_number_len=0;

   double nConvertedTxAmount = 0.0 ;
   CHAR   strProcessingCode[20] = {0} ;
   BYTE           stip_result_code;
   CHAR			  amountbuff[13] = "";//praneeth added for converted amount swapping
   CHAR			  rspText[41] = {0};

   strncpy(strProcessingCode, p_auth_tx->TLF01_details.processing_code, 2) ;
   result_code = PTEMSG_OK;

   /* First check for a cash bonus program winner. If not a winner,
    * this portion of the update-all will be committed by itself. Only
    * the txn ctr in the cash bonus program will be updated. If there
    * is a problem with the rest of the update-all and a rollback is
    * performed, txn ctr does not get rolled back. But this is okay.
    *
    * If this is a cash bonus program winner, it does not get committed
    * until the update-all completes. This way, it can be rolled back
    * if needed.
    */
   if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,"00") )
   {
      /* Transaction is approved. */
      if (( 0 == strcmp(p_auth_tx->TLF01_details.dispensation.origin, "LO" )) &&
          ( msg_sub_type2 == 0 ))
      {
         /* Local origin. See if txn is cash bonus eligible.
          * If yes, check for a winner.
          */
         if ( !get_cash_bonus_result( p_auth_tx ) )
         {
            /* False = not a winner. Commit the change to the txn ctr. */
            updates_to_db_made = true;
            commit_the_update(); 
            updates_to_db_made = false;
         }
      }

  	 /* See if OMC01 record needs to be updated. */
  	 if (0 == strcmp(p_auth_tx->TLF01_details.product_codes[18].code,"OMD"))
  	 {
		   if( atoi(p_auth_tx->TLF01_details.nbr_of_prod_codes) > 0)
		   {
			   result_code = db_update_OMC01_Repeats(p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.card_num,p_auth_tx->TLF01_details.nbr_of_prod_codes, err_msg);
		   	//memset(p_auth_tx->TLF01_details.nbr_of_prod_codes,0x00,sizeof(p_auth_tx->TLF01_details.nbr_of_prod_codes));
	   		}	   
	  		if ( result_code == PTEMSG_OK )
	  		{
				strcat( update_all_status, "T1" );
	  		}
	   		else
	   		{
         		strcat( update_all_status, "T0" );
	   		}

	  		//response text retain for OMD Approval only for ACQ.
	  		//RC 00 & OMD flag already validated above - changes by Amith k
	  		if(( (strcmp(p_auth_tx->TLF01_details.response_text, "APPROVED_STAND_IN") == 0)
	  		  || (strcmp(p_auth_tx->TLF01_details.response_text, "Approved") 		  == 0)
			  || (strcmp(p_auth_tx->TLF01_details.response_text, "") 		 		  == 0)) &&
	  		  (  (strncmp(p_auth_tx->TLF01_details.handler_queue, "dcpisoA", 7) == 0)
	  		  || (strncmp(p_auth_tx->TLF01_details.handler_queue, "VOICE", 5)   == 0)))
	  		{
	  			if(strncmp(p_auth_tx->TLF01_details.vehicle_number, "MID_BYPS", 8) == 0)
	  			{
	  				if(strlen(pos_res_txt) > 0)
	  					strcpy(rspText, pos_res_txt);
	  				else
	  					strcpy(rspText, "Txn/Refund limit exceeded for merchant");
	  			}
	  			else if(strncmp(p_auth_tx->TLF01_details.vehicle_number, "NBR_BYPS", 8) == 0)
	  			{
	  				if(strncmp(p_auth_tx->TLF01_details.currency_code, "608", 3) == 0)
	  					strcpy(rspText, "Fail PesoUsage(Nbr)chk for period");
	  				else if(strncmp(p_auth_tx->TLF01_details.currency_code, "840", 3) == 0)
	  					strcpy(rspText, "Fail dollarUsage(Nbr)chk for period");
	  			}
	  			else if(strncmp(p_auth_tx->TLF01_details.vehicle_number, "AMT_BYPS", 8) == 0)
	  			{
	  				if(strncmp(p_auth_tx->TLF01_details.currency_code, "608", 3) == 0)
	  					strcpy(rspText, "Fail PesoUsage(Amt)chk for period");
	  				else if(strncmp(p_auth_tx->TLF01_details.currency_code, "840", 3) == 0)
	  					strcpy(rspText, "Fail dollarUsage(Amt)chk for period");
	  			}
	  			else if(strncmp(p_auth_tx->TLF01_details.vehicle_number, "MCC_BYPS", 8) == 0)
	  			{
	  				strcpy(rspText, "Txn category usage check failed");
	  			}

	  			if(strlen(rspText) > 0)
	  			{
	  				memset(p_auth_tx->TLF01_details.response_text, 0, sizeof(p_auth_tx->TLF01_details.response_text));
	  				memcpy(p_auth_tx->TLF01_details.response_text, rspText, strlen(rspText));
	  			}
	  		}
   	   }

   }

   /* See if card record needs to be updated. */
   if (p_auth_tx->TLF01_details.update_mask[0] & CCF02P_MASK)
   {
      /* Yes */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinCCF02P (&p_auth_tx->CCF02P_details,MODE_UPDATE);
		  }
	   if ( DB_Timing_Flag == true )
         start_time = ptetime_get_time();

      result_code = db_update_ccf02p(&p_auth_tx->CCF02P_details, err_msg);

      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_UPDATE, 1, CCF02P_DATA,
                              (float)duration, &TimingStats );
      }

      if ( result_code == PTEMSG_OK )
         strcat( update_all_status, "A1" );
      else
         strcat( update_all_status, "A0" );
   }

   if ((p_auth_tx->TLF01_details.update_mask[0]) & CCF03P_MASK)
   {
      if ( result_code == PTEMSG_OK )
      {
         /* Depending on ccf03_insert flag, either do a insert or an update */
         if (p_auth_tx->ccf03_insert)
         {
     		  if (DB_NullCheck_Flag==1)
     		  {
     			  CheckNULLTerminatedinCCF03 (&p_auth_tx->CCF03P_details,MODE_INSERT);
     		  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_insert_ccf03p(&p_auth_tx->CCF03P_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, 1, CCF03P_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "B1" );
            else
               strcat( update_all_status, "B0" );
         }
         else
         {
     		  if (DB_NullCheck_Flag==1)
     		  {
     			  CheckNULLTerminatedinCCF03 (&p_auth_tx->CCF03P_details,MODE_UPDATE);
     		  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_update_ccf03p(&p_auth_tx->CCF03P_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, 1, CCF03P_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "C1" );
            else
               strcat( update_all_status, "C0" );
         }
      }
   }

   if ((p_auth_tx->TLF01_details.update_mask[0]) & OFFUS_VELOCITY_MASK)
   {
      if ( result_code == PTEMSG_OK )
      {
         /* Depending on ccf03_insert flag, either do a insert or an update */
         if (p_auth_tx->offus_velocity_insert)
         {
   		  if (DB_NullCheck_Flag==1)
   		  {
   			  CheckNULLTerminatedinOffus_velocity (&p_auth_tx->offus_velocity_details,MODE_INSERT);
   		  }
            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_insert_offus_velocity(&p_auth_tx->offus_velocity_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, 1, OFFUS_VELOCITY_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "D1" );
            else
               strcat( update_all_status, "D0" );
         }
         else
         {
     		  if (DB_NullCheck_Flag==1)
     		  {
     			  CheckNULLTerminatedinOffus_velocity (&p_auth_tx->offus_velocity_details,MODE_UPDATE);
     		  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_update_offus_velocity(&p_auth_tx->offus_velocity_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, 1, OFFUS_VELOCITY_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "E1" );
            else
               strcat( update_all_status, "E0" );
         }
      }
   }

   if (p_auth_tx->TLF01_details.update_mask[0] & ACF01_MASK)
   {
      if ( result_code == PTEMSG_OK )
      {
         /* Update BALANCE ACCOUNT(S) */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinACF01 (&p_auth_tx->ACF01_details,MODE_UPDATE);
		  }
    	  if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         result_code = db_update_acf01(&p_auth_tx->ACF01_details, err_msg) ;

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, 1, ACF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (result_code == PTEMSG_OK)
         {
            /* Update Deferred Balance Account for any of the following txn types. */
            strcat( update_all_status, "F1" );
            if ((p_auth_tx->TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE) ||
                (p_auth_tx->TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE) ||
                (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE) ||
                (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE) )
            {
               deferred_txn = false;

               /* If reversal or void, check to see if it is of a deferred txn. */
               if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE ) ||
                    (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)  )
               {
                  if ( (p_auth_tx->TLF01_details.deferred_term_length[0] != 0x00) ||
                       (!strcmp(strProcessingCode, "70"))  )
                  {
                     /* Do this check because RSB Guarantor cards might have
                      * deferred data, but they are not treated as deferreds.
                      */
                     if ( p_auth_tx->BIN01_details.credit_subtype[0] != 'G' )
                     {
                        /* It is not an RSB card, so it is a deferred txn. */
                        deferred_txn = true;
                     }
                  }
               }
               else
               {
                  /* The other txn types are definitely deferreds. */
                  deferred_txn = true;
               }

               if ( deferred_txn == true )
               {
                  /* Update Deferred Balance Account */
                  if ( DB_Timing_Flag == true )
                     start_time = ptetime_get_time();

                  result_code = db_update_acf01_for_deferred_account( p_auth_tx,
                                                                      &p_auth_tx->ACF01_details,
                                                                      err_msg );

                  if ( DB_Timing_Flag == true )
                  {
                     end_time = ptetime_get_time();
                     duration = end_time - start_time;
                     update_timing_stats( ST1_DB_INSERT, 1, ACF01_DATA,
                                          (float)duration, &TimingStats );
                  }

                  if ( result_code == PTEMSG_OK )
                     strcat( update_all_status, "G1" );
                  else
                     strcat( update_all_status, "G0" );
               }
            }
         }
         else
            strcat( update_all_status, "F0" );
      }
   }

   if ( result_code != PTEMSG_OK )
   {
      result_code = PTEMSG_UPDATE_FAILED;
   }
   else if (strcmp(p_auth_tx->TLF01_details.response_code,"00")== 0)
   {
      if((strcmp(p_auth_tx->TLF01_details.dispensation.origin, "LO" ) == 0) &&
                (msg_sub_type2 == 0))
      {
         switch(p_auth_tx->TLF01_details.tx_key)
         {
            case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE:

                  /* Deferred transactions from Voice are treated as Auths.
                   * Online deferreds are treated as credit sales.
                   */
                  if (0==strcmp(p_auth_tx->TLF01_details.handler_queue,"VOICE"))
                  {
                     result_code = process_auth_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "H1" );
                     else
                        strcat( update_all_status, "H0" );
                  }
                  else
                  {
                     result_code = process_sale_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "I1" );
                     else
                        strcat( update_all_status, "I0" );
                  }
            break;

            case AUTH_AUTHORIZATION_RESPONSE:
            case AUTH_PRE_AUTHORIZATION_RESPONSE:
            case AUTH_CARD_VERIFICATION_RESPONSE:
            case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE:
            case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:		
            case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
            case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE	:	            
                     result_code = process_auth_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "J1" );
                     else
                        strcat( update_all_status, "J0" );
            break;

            case AUTH_SALE_RESPONSE:
            case AUTH_CASH_ADVANCE_RESPONSE:
            case AUTH_SALES_COMPLETION_RESPONSE:
            case AUTH_OFFLINE_SALE_RESPONSE:
            case AUTH_QUASI_CASH_RESPONSE:
            case AUTH_RELOAD_RESPONSE:
                     result_code = process_sale_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "K1" );
                     else
                        strcat( update_all_status, "K0" );
            break;

            case AUTH_REFUND_RESPONSE:
            case AUTH_OFFLINE_REFUND_RESPONSE:           
                     result_code = process_refund_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "L1" );
                     else
                        strcat( update_all_status, "L0" );
            break;

            /* 12/06/99 according to Gabriel for VOID and ADJUSTMENT
             * we do not need to update RRN(replacing original with the new one)
             */
            case AUTH_VOID_SALE_RESPONSE:
            case AUTH_VOID_REFUND_RESPONSE:
            case AUTH_VOID_CASH_ADVANCE_RESPONSE:
            case AUTH_OFFLINE_VOID_SALE_RESPONSE:
            case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
            case AUTH_VOID_RELOAD_RESPONSE:
                     if ((p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE) &&
                         (p_auth_tx->TLF01_details.deferred_term_length[0] != 0x00  )  )
                     {
                        /* This is the void of a deferred.
                         * If it is from Voice, do nothing here because it
                         * is in BCH20.
                         */
                        if (0!=strcmp(p_auth_tx->TLF01_details.handler_queue,"VOICE"))
                        {
                           result_code = process_void_trans(p_auth_tx);
                           if ( result_code == PTEMSG_OK )
                              strcat( update_all_status, "M1" );
                           else
                              strcat( update_all_status, "M0" );
                        }
                     }
                     else
                     {
                        result_code = process_void_trans(p_auth_tx);
                        if ( result_code == PTEMSG_OK )
                           strcat( update_all_status, "N1" );
                        else
                           strcat( update_all_status, "N0" );
                     }
            break;

#if BDOR_62_PREAUTH_VOID
            case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
                     if(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE)
                     {
                         result_code = process_void_preauth_trans(p_auth_tx);
                         if ( result_code == PTEMSG_OK )
                            strcat( update_all_status, "N3" );
                         else
                            strcat( update_all_status, "N2" );
                     }
                break;
#endif

            case AUTH_SALE_ADJUSTMENT_RESPONSE: 
            case AUTH_REFUND_ADJUSTMENT_RESPONSE: 
                     result_code = process_adjustments(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "O1" );
                     else
                        strcat( update_all_status, "O0" );
            break;

            case AUTH_REVERSAL_RESPONSE:
                     result_code = process_reversal(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "P1" );
                     else
                        strcat( update_all_status, "P0" );
            break;

            case AUTH_RELOAD_CONFIRM_RESPONSE:
                     /* Do not want to error out on Reload Confirms. */
                     memcpy( &tlf01, &p_auth_tx->TLF01_details, sizeof(TLF01) );

                     if ( DB_Timing_Flag == true )
                        start_time = ptetime_get_time();

                     if ( PTEMSG_OK != db_update_reload_confirm( &tlf01, err_msg))
                       updateds_log_message(1,2,err_msg,"process_alldata_msg");

                     if ( DB_Timing_Flag == true )
                     {
                        end_time = ptetime_get_time();
                        duration = end_time - start_time;
                        update_timing_stats( ST1_DB_UPDATE, 1, TLF01_DATA,
                                             (float)duration, &TimingStats );
                     }
            break;

            default:
                     PRINT("unknown response received by updateds\n");
                     result_code = PTEMSG_INVALID_DATATYPE;
                     strcat( update_all_status, "Q0" );
                     sprintf( err_msg,
                             "Unknown tran type %c: for ALL_DATA request",
                              p_auth_tx->TLF01_details.tx_key);
                     updateds_log_message(2,2,err_msg,"process_alldata_msg");
            break;
         }
      }
      else if ((strcmp(p_auth_tx->TLF01_details.dispensation.origin, "RO" ) == 0) ||
               (msg_sub_type2 != 0 ))
      {
         result_code = process_network_request( p_auth_tx ,msg_sub_type2);
         if ( result_code == PTEMSG_OK )
            strcat( update_all_status, "R1" );
         else
            strcat( update_all_status, "R0" );
      }
      else
      {
         result_code = PTEMSG_INVALID_DATATYPE;
         strcat( update_all_status, "S0" );
      }
   }

#ifdef MC12_OCT2023
	if( updatds_Check_If_MC_Transaction(p_auth_tx) 			 &&
	   (0 == strncmp(p_auth_tx->TLF01_details.acquirer_id,"ncmcrd",6)) &&
		updatds_check_if_txn_falls_under_merchant_ingerity_validation(&p_auth_tx->TLF01_details) &&
	   (0 != strncmp(p_auth_tx->TLF01_details.response_code,"19",2) &&
		GS_TXN_TIMEDOUT	!= p_auth_tx->TLF01_details.general_status	))
	{
		/* Timeout scenarios need not be counted for MC excessive decline count */
		result_code= process_merchant_integrity_transactions_for_mc(p_auth_tx);
		/*Updating or inserting the merchant_ingerity_param should not block or
		 * decline the transaction
		 * */
		result_code =PTEMSG_OK;
	}

#endif

#ifdef MC13_OCT2023
	if( updatds_Check_If_MC_Transaction(p_auth_tx) 			 			  &&
	    (0 == strncmp(p_auth_tx->TLF01_details.acquirer_id, "ncmcrd", 6)) &&
		updatds_check_if_txn_falls_under_mac_validation(p_auth_tx) )
	{
		result_code= process_merchant_integrity_transactions_for_mac(p_auth_tx);
		/*Updating or inserting the merchant_ingerity_param should not block or
		 * decline the transaction
		 * */
		result_code =PTEMSG_OK;
	}

#endif

   /**
    * Merchant integrity coding
    * */
#ifdef VISA7
   if( updatds_Check_If_VISA_Transaction(&p_auth_tx->TLF01_details) &&
       0 == strncmp(p_auth_tx->TLF01_details.acquirer_id,"ncvisa",6) &&
	   updatds_check_if_txn_falls_under_merchant_ingerity_validation(&p_auth_tx->TLF01_details))
   {
	   result_code= process_merchant_integrity_transactions(&p_auth_tx->TLF01_details);
	   /*Updating or inserting the merchant_ingerity_param should not block or
	    * decline the transaction
	    * */
	   result_code =PTEMSG_OK;
   }
#endif
   if ( result_code == PTEMSG_OK )
   {
      /* The above updates/inserts were successful.
       * Insert into TLF01 if record has not already been inserted.
       * Insert into TLF01 is part of update_all because EB uses
       * TLF01 as a source for settlement data.  So we cannot respond
       * to the device/network until we know the database has been
       * updated properly.  If the above updates/inserts succeed, but
       * insert into TLF01 fails, rollback the above updates/inserts;
       * transaction will be declined.
       */

      if ( p_auth_tx->host2_acquirer_cb_code[0] != '*' )
      {
         /* Populate system_date field - date txn is inserted into database. */
         memcpy( &tlf01, &p_auth_tx->TLF01_details, sizeof(TLF01) );
         create_system_date( tlf01.system_date );

         if ( p_auth_tx->TLF01_details.tran_finish_time[0] == 0x00 )
            ptetime_get_strtime( tlf01.tran_finish_time );

         /* Store status info for audit trail.*/
         if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis3",6) 	&&
        	0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN,6))||
            0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5) ||
			0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncdci",5))
         {
             memcpy( tlf01.down_payment,
                     update_all_status,
                     sizeof(tlf01.down_payment)-1 );
         }
         else
         {
         memcpy( tlf01.dcf01_retired_cwk,
                 update_all_status,
                 sizeof(update_all_status)-1 );
         }


         memset(&db_recs.tlf01, 0, sizeof(db_recs.tlf01));
         memcpy(&db_recs.tlf01, &tlf01, sizeof(TLF01));

         /* Convert message type to a request type, for standardization. */
         convert_msgtype_to_request( db_recs.tlf01.message_type );

         /* Set flag to indicate to device/network
          * controllers TLF01 has been inserted.
          */
         p_auth_tx->host2_acquirer_cb_code[0] = '*';

         /* Insert into TLF01. */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinTLF01 (&db_recs.tlf01,MODE_INSERT);
		  }
         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();
		 
	/*	#ifdef _DEBUG
         CheckNULLTerminatedinTLF01 (&db_recs.tlf01);
		#endif*/

         if(true ==updatds_Check_If_Diners_Transaction(p_auth_tx) )
         {
        	if (strlen(p_auth_tx->host2_message_seq_nbr) > 0)
		    {
			    strncpy(db_recs.tlf01.product_codes[11].code,
			 		    p_auth_tx->host2_message_seq_nbr + 1,3);
		    }
		    else if (strlen(p_auth_tx->EMV_details.pan_sequence_number)> 0)
		    {
			   strncpy(db_recs.tlf01.product_codes[11].code,
					  p_auth_tx->EMV_details.pan_sequence_number,3);
		   }
         }
         if((0 == strncmp(db_recs.tlf01.handler_queue,"ncmcrd",6)  ||
        	 0 == strncmp(db_recs.tlf01.handler_queue,"ncmcr3",6)) ||
        	0 == strncmp(db_recs.tlf01.handler_queue,"ncvis",5))
         {/* will include all ncmcrd and ncmcr3*/
			 strcpy(db_recs.tlf01.product_codes[7].amount, p_auth_tx->ch_billing_amt);
			 strcpy(db_recs.tlf01.product_codes[7].code, p_auth_tx->ch_billing_curr_code);
         }
         result_code = db_insert_tlf01(&db_recs.tlf01, err_msg);

	/* Insert into TLF01_STIP table as well when inserting into TLF01 for STIP Tx - Girija Y ThoughtFocus */
		 if(((true == Updatds_Is_MCRefund_Transaction(&db_recs.tlf01))||
			(strcmp(db_recs.tlf01.response_code,"00") == 0 )) &&
			(strcmp(db_recs.tlf01.product_codes[1].code,STANDIN) == 0))
		 {
			 /* Need to update STAND in flag in case of VOID / Reverse.. TF PHANI */
			 if((true == updatds_Check_If_CUP_Transaction (p_auth_tx)) &&
				(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"nccup",5)))
			 {
				 /* No need to override db_recs.tlf01.product_codes[7].amount
				 	 as we are stroning De117 for incoming tranasction
				 */
			 }
			 else
			 {
				 strcpy(db_recs.tlf01.product_codes[7].amount, p_auth_tx->ch_billing_amt);
				 strcpy(db_recs.tlf01.product_codes[7].code, p_auth_tx->ch_billing_curr_code);
			 }

			/* Store DE10 Billing Conversion rate in .TF Phani*/
			 /* HCTS4 - stroing the VISA MVV in product_codes[2].amount, so commented the below conditions
			 if((true == updatds_Check_If_VISA_Transaction (p_auth_tx)) &&
				(0==strncmp(db_recs.tlf01.handler_queue,"dcpiso",6) ||
				 0==strncmp(db_recs.tlf01.handler_queue,"VOICE",5)) &&
				0==strncmp(db_recs.tlf01.product_codes[1].code,"stndin",6) &&
				db_recs.tlf01.product_codes[8].amount[0]!='\0' &&
				strncmp(db_recs.tlf01.response_code,"00",2) == 0 )
			 {
				 	 //let it flow down to normal flow. 10 digit VISAA MVV Value is stored already insidert into db.
			 }
			 else*/
			 {
				 strcpy(db_recs.tlf01.product_codes[8].amount, p_auth_tx->ch_billing_conv_rate);  //applies for visa incoming
			 }


			 	/* Store industry_code .TF Phani*/
			 strcpy(db_recs.tlf01.product_codes[8].code, p_auth_tx->industry_code);
			 /* TF phani - Updates for 0120 MCARD fields*/
			 /* DE 61 value - which need to be populated during 0120 from sbtach*/
			 strcpy(db_recs.tlf01.source_key,p_auth_tx->acceptor_term_name);
	 //
			 /* Store DE43 here so it gets logged to TLF01:
				*   Merchant Name         --> TLF01.card_holder_name
				*   City and Country Code --> TLF01.ncf01_retired_cwk
				*  TF Phani
				*/
			   if ( db_recs.tlf01.card_holder_name[0] == 0x00 )
			   {
				  memcpy( db_recs.tlf01.card_holder_name,
						  p_auth_tx->MCF01_details.name02,
						  22 );
			   }

			   if ( db_recs.tlf01.ncf01_retired_cwk[0] == 0x00 )
			   {

				   memset(db_recs.tlf01.ncf01_retired_cwk,
						   0x20,sizeof(db_recs.tlf01.ncf01_retired_cwk)-1);
				  memcpy( db_recs.tlf01.ncf01_retired_cwk,
						  p_auth_tx->MCF01_details.city,
						  strlen(p_auth_tx->MCF01_details.city));
				   itemp = strlen(p_auth_tx->MCF01_details.city);
				   				//  memset(&db_recs.tlf01.ncf01_retired_cwk[itemp],0x20,13-itemp);
				   	memcpy(&db_recs.tlf01.ncf01_retired_cwk[13], "PHL", 3 );
			   }


			   //Storing converted currency and amount while dual currency //praneeth
			   if ( 0 == strcmp(db_recs.tlf01.host_start_time, "7.000") )
			   {
					strcpy( db_recs.tlf01.currency_code, "840" );
					if (strcmp(db_recs.tlf01.product_codes[9].amount, "")) 
					{
						strcpy(amountbuff,db_recs.tlf01.total_amount);
						strcpy(db_recs.tlf01.total_amount,db_recs.tlf01.product_codes[9].amount);
						strcpy(db_recs.tlf01.product_codes[9].amount,amountbuff);
					}
			   }
			   else if ( 0 == strcmp(db_recs.tlf01.host_start_time, "5.000") )
			   {
   					strcpy( db_recs.tlf01.currency_code, "608" );
					if (strcmp(db_recs.tlf01.product_codes[9].amount, "") )
					{
						strcpy(amountbuff,db_recs.tlf01.total_amount);
						strcpy(db_recs.tlf01.total_amount,db_recs.tlf01.product_codes[9].amount);
						strcpy(db_recs.tlf01.product_codes[9].amount,amountbuff);
					}
			   }

			 if( (db_recs.tlf01.tx_key == AUTH_VOID_SALE)||(db_recs.tlf01.tx_key ==AUTH_VOID_SALE_RESPONSE))
			 {
				 strcpy(db_recs.tlf01.product_codes[1].code,VOIDED);
				 result_code = db_update_tlf01_stip_void(&db_recs.tlf01, err_msg);

			 }
#if BDOR_62_PREAUTH_VOID
			 else if((db_recs.tlf01.tx_key == AUTH_VOID_PRE_AUTHORIZATION)||(db_recs.tlf01.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE))
			 {
				 strcpy(db_recs.tlf01.product_codes[1].code,VOIDED);
				 result_code = db_update_tlf01_stip_void_preauth(&db_recs.tlf01, err_msg);

				 if(result_code != PTEMSG_OK  )
				 {
					 strcpy(err_msg,"");
					 result_code =db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
				 }
			 }
#endif
			 else if((db_recs.tlf01.tx_key == AUTH_REVERSAL)||(db_recs.tlf01.tx_key == AUTH_REVERSAL_RESPONSE ))
			 {
				  if((strncmp(db_recs.tlf01.handler_queue,"ncmcrd",6) == 0  ||
					  strncmp(db_recs.tlf01.handler_queue,"ncmcr3",6) == 0) ||
					 strncmp(db_recs.tlf01.handler_queue,"ncjcb",5)  == 0 ||
					 strncmp(db_recs.tlf01.handler_queue,"ncvis",5)  == 0 ||
					 strncmp(db_recs.tlf01.handler_queue,"ncpos",5)  == 0)
				  {/* will include all ncmcrd and ncmcr3*/
					  // For Master card and JCB do nothing
					  // We need to store original time for incoming Amex transaction so that send it durring sbatch.
					  if( (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5)) &&
						  ((0 == strncmp(p_auth_tx->TLF01_details.message_type,"1420",4)) ||
						  (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0420",4))))
					  {
						  strncpy(db_recs.tlf01.product_codes[1].amount,p_auth_tx->orig_local_time_date,12);
					  }
				  }
				  else
				  {
					  strcpy(db_recs.tlf01.product_codes[1].code,REVERSED);
				  }
				  /* For voice txn the stan is diffrent for sale and cancel sale txn
				  The query which is their in db_update_tlf01_stip_reversal not able to update original txn
				  so copy the original STAN into txn STAN TF-Ajay 10-Jul-2009 */
				  if(strcmp(db_recs.tlf01.handler_queue, "VOICE")==0)
				  {
					  strcpy(db_recs.tlf01.sys_trace_audit_num,db_recs.tlf01.orig_sys_trace_num);
				  }
#if BDOR_62_PREAUTH_VOID
				  if(OrigTxKey == AUTH_VOID_PRE_AUTHORIZATION || OrigTxKey == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE)
				  {
					  memset(&tlf01_temp, 0x00, sizeof(TLF01));
					  memcpy(&tlf01_temp, &db_recs.tlf01, sizeof(TLF01));
					  strcpy(tlf01_temp.product_codes[1].code, STANDIN);
					  strncpy(tlf01_temp.processing_code, "30", 2);
					  tlf01_temp.tx_key = AUTH_PRE_AUTHORIZATION_RESPONSE;
					  result_code = db_update_tlf01_stip_void_preauth(&tlf01_temp, err_msg);
				  }
				  else
#endif
				  {
					  result_code = db_update_tlf01_stip_reversal(&db_recs.tlf01, err_msg);
				  }

				  if(result_code != PTEMSG_OK  )
				  {/* TF Phani - This could be an Online approval/Offline reversal case*/
					  strcpy(err_msg,"");
					  result_code =db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
				  }
			 }
			 else if((db_recs.tlf01.tx_key == AUTH_SALE_ADJUSTMENT)||(db_recs.tlf01.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE))
			 {
				 if(db_recs.tlf01.product_codes[6].quantity[0]      == 'R')
				 {
					 strcpy(db_recs.tlf01.product_codes[1].code,POSTED);
				 }
				 stip_result_code = db_update_tlf01_stip_adjust(&db_recs.tlf01, err_msg);
			 }
			 else
			 {
				 if(db_recs.tlf01.product_codes[6].quantity[0]      == 'R')
				 {
					 strcpy(db_recs.tlf01.product_codes[1].code,POSTED);
				 }

				 if(true == updatds_check_for_amex_partial_reversal_transaction(p_auth_tx))
				 {
					 strncpy(db_recs.tlf01.product_codes[1].amount,p_auth_tx->orig_local_time_date,12);
				 }
			     stip_result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
			 }

	         if(strncmp(db_recs.tlf01.handler_queue,"ncvis",5) == 0)
	         {
	        	 if(strncmp(db_recs.tlf01.product_codes[0].quantity,"56",2) == 0)
	        	 {
	        		 rc = store_pad_raw_records(p_auth_tx);
	                 /* Commit the EMV inserts. */
	                 dbcommon_commit();
	                 dstimer_commit();
	        	 }
	         }
		 }
		 
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_INSERT, 1, TLF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if ( result_code == PTEMSG_OK )
            strcat( update_all_status, "U1" );
         else
            strcat( update_all_status, "U0" );

         /* If EMV txn, insert into EMV01 table. */
         if ( p_auth_tx->EMV_details.currency_code[0] != 0x00 )
         {
            /* There is EMV data.  Commit the updates so far
             * to avoid a large rollback.  Then insert EMV records.
             * But do not change result code from TLF01 insert.
             */
            commit_the_update(); 

            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            rc = store_emv_records( p_auth_tx );

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, 1, EMV01_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( rc == PTEMSG_OK )
               strcat( update_all_status, "V1" );
            else
               strcat( update_all_status, "V0" );

            /* Update Chip Condition Code in DCF01. */
            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            rc = update_chip_condition( p_auth_tx );

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, 1, DCF01_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( rc == PTEMSG_OK )
               strcat( update_all_status, "W1" );
            else
               strcat( update_all_status, "W0" );

            /* Commit the EMV inserts. */
            dbcommon_commit();
            dstimer_commit();
         }

      }
	 else
	 {
   		 if((strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0))
			 //p_auth_tx->
		{
			 if(p_auth_tx->TLF01_details.system_date[0] == '\0')
			 {
				create_system_date( p_auth_tx->TLF01_details.system_date );
			 }
			 if( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)||(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE))
			 {
				  if(strncmp(db_recs.tlf01.handler_queue,"ncvis",6) == 0 ||
					 strncmp(db_recs.tlf01.handler_queue,"ncjcb",5) == 0 ||
					 strncmp(db_recs.tlf01.handler_queue,"ncmcrd",6)== 0 ||
					 strncmp(db_recs.tlf01.handler_queue,"ncmcr3",6)== 0)
				  {/* will include all ncmcrd and ncmcr3*/
					  // For Master card and JCB do nothing
					  // We need to store original time for incoming Amex transaction so that send it durring sbatch.
					  if( (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5)) &&
						  ((0 == strncmp(p_auth_tx->TLF01_details.message_type,"1420",4)) ||
						  (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0420",4))))
					  {
						  strncpy(db_recs.tlf01.product_codes[1].amount,p_auth_tx->orig_local_time_date,12);
					  }
				  }
				  else
				  {

					  strcpy(db_recs.tlf01.product_codes[1].code,REVERSED);
				  }
#if BDOR_62_PREAUTH_VOID
				  if(OrigTxKey == AUTH_VOID_PRE_AUTHORIZATION || OrigTxKey == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE)
				  {
					  memset(&tlf01_temp, 0x00, sizeof(TLF01));
					  memcpy(&tlf01_temp, &p_auth_tx->TLF01_details, sizeof(TLF01));
					  strcpy(tlf01_temp.product_codes[1].code, STANDIN);
					  strncpy(tlf01_temp.processing_code, "30", 2);
					  tlf01_temp.tx_key = AUTH_PRE_AUTHORIZATION_RESPONSE;

					  result_code = db_update_tlf01_stip_void_preauth(&tlf01_temp, err_msg);
				  }
				  else
#endif
				  if((true == updatds_Check_If_CUP_Transaction (p_auth_tx)) &&
					 (0 == strncmp(db_recs.tlf01.handler_queue,"dcp",3)))
				  {
					  result_code = db_update_tlf01_stip_reversal_for_cup_Acq(&p_auth_tx->TLF01_details, err_msg);
				  }
				  else
				  {
					  result_code = db_update_tlf01_stip_reversal(&p_auth_tx->TLF01_details, err_msg);
				  }
				 if(result_code!= PTEMSG_OK)
				 {
					 result_code = db_check_tlf01_stip_reversal(&p_auth_tx->TLF01_details, err_msg);
					 if(result_code== PTEMSG_OK)
					 {

						//dont insert into tlf01_stip again.
					 }

				 }
			 }
			 else if( (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE)||(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE))
			 {
				 strcpy(p_auth_tx->TLF01_details.product_codes[1].code,VOIDED);
				 result_code = db_update_tlf01_stip_void(&p_auth_tx->TLF01_details, err_msg);
			 }
#if BDOR_62_PREAUTH_VOID
			 else if((p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)||(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE))
			 {
				 strcpy(p_auth_tx->TLF01_details.product_codes[1].code,VOIDED);
				 result_code = db_update_tlf01_stip_void(&p_auth_tx->TLF01_details, err_msg);

				 if(result_code != PTEMSG_OK  )
				 {
					 strcpy(err_msg,"");
					 result_code = db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
				 }
			 }
#endif
			 else if( (p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE)||(p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE_RESPONSE))
		 	 {
				 if(db_recs.tlf01.product_codes[6].quantity[0]      == 'R')
				 {
					 strcpy(db_recs.tlf01.product_codes[1].code,POSTED);
				 }
		 	 	 stip_result_code = db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
		 	 }
			 if(result_code != PTEMSG_OK  )
			 {/* TF Phani - This could be an Online approval/Offline reversal case*/
				strcpy(err_msg,"");
				 if(db_recs.tlf01.product_codes[6].quantity[0]      == 'R')
				 {
					 strcpy(db_recs.tlf01.product_codes[1].code,POSTED);
				 }
				result_code =db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
			 }
				       dbcommon_commit();
		}

		if (((p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE)||(p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE_RESPONSE)) &&
			  (true == updatds_Check_If_VISA_Transaction (&p_auth_tx->TLF01_details)) &&
			  (db_recs.tlf01.acquirer_id[0] == '\0'))
		{
			strcpy(db_recs.tlf01.acquirer_id, p_auth_tx->TLF01_details.acquirer_id);
		    result_code = db_update_tlf01( &p_auth_tx->TLF01_details, err_msg );
		    dbcommon_commit();
		}


	}
      if(p_auth_tx->future_use_3[0]!='\0' &&
    		  0==strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3))
      {
    	  memset( &db_recs.ptsn01, 0, sizeof( db_recs.ptsn01 ) );
    	  strcpy(db_recs.ptsn01.transaction_id,p_auth_tx->TLF01_details.primary_key.transaction_id);
    	  strcpy(db_recs.ptsn01.system_date,db_recs.tlf01.system_date);
    	  strncpy(db_recs.ptsn01.total_length,p_auth_tx->future_use_3,2);
    	  strncpy(db_recs.ptsn01.length_serial_no,p_auth_tx->future_use_3+2,2);
    	  strncpy(db_recs.ptsn01.table_id,p_auth_tx->future_use_3+4,2);
    	  serial_number_len=atoi(db_recs.ptsn01.total_length)-2;
    	  strncpy(db_recs.ptsn01.serial_number,p_auth_tx->future_use_3+6,serial_number_len);


    	  result_code =db_insert_ptsn01(&db_recs.ptsn01, err_msg);
    	 // dbcommon_commit();
      }


   }
   else
   {
      /* The above updates/inserts failed.  Do not insert into TLF01 here,
       * because it will get rolled back.  Leave it to the calling application
       * set the response code to decline and insert into TLF01.
       */
   }

   /* Store status info for audit trail.
   memcpy( p_auth_tx->TLF01_details.dcf01_retired_cwk,
           update_all_status,
           sizeof(update_all_status)-1 );*/

   return( result_code );
}

BYTE process_alldata_msg_mp( pAUTH_TX p_auth_tx )
{
   TLF01          tlf01;
   CHAR           primary_key[50] = "";
   CHAR           transaction_id [21] = "";
   CHAR           date_time[16] = "";
   CHAR           bch01_mp_open_date[9] = "";
   CHAR           bch01_mp_open_time[7] = "";
   BYTE           deferred_txn;
   BYTE           rc;
   double         start_time;
   double         end_time;
   double         duration;
   int			  itemp;		
   double nConvertedTxAmount = 0.0 ;
   CHAR   strProcessingCode[20] = {0} ;
   BYTE           stip_result_code;
   CHAR			  amountbuff[13] = "";//praneeth added for converted amount swapping
   int serial_number_len=0;

   strncpy(strProcessingCode, p_auth_tx->TLF01_details.processing_code, 2) ;
   result_code = PTEMSG_OK;

   /* First check for a cash bonus program winner. If not a winner,
    * this portion of the update-all will be committed by itself. Only
    * the txn ctr in the cash bonus program will be updated. If there
    * is a problem with the rest of the update-all and a rollback is
    * performed, txn ctr does not get rolled back. But this is okay.
    *
    * If this is a cash bonus program winner, it does not get committed
    * until the update-all completes. This way, it can be rolled back
    * if needed.
    */
   if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,"00") )
   {
      /* Transaction is approved. */
      if (( 0 == strcmp(p_auth_tx->TLF01_details.dispensation.origin, "LO" )) &&
          ( msg_sub_type2 == 0 ))
      {
         /* Local origin. See if txn is cash bonus eligible.
          * If yes, check for a winner.
          */
         if ( !get_cash_bonus_result( p_auth_tx ) )
         {
            /* False = not a winner. Commit the change to the txn ctr. */
            updates_to_db_made = true;
            commit_the_update(); 
            updates_to_db_made = false;
         }
      }

  	 /* See if OMC01 record needs to be updated. */
  	 if (0 == strcmp(p_auth_tx->TLF01_details.product_codes[18].code,"OMD"))
  	 {
		   if( atoi(p_auth_tx->TLF01_details.nbr_of_prod_codes) > 0)
		   {
			   result_code = db_update_OMC01_Repeats(p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.card_num,p_auth_tx->TLF01_details.nbr_of_prod_codes, err_msg);
		   	//memset(p_auth_tx->TLF01_details.nbr_of_prod_codes,0x00,sizeof(p_auth_tx->TLF01_details.nbr_of_prod_codes));
	   		}	   
	  		if ( result_code == PTEMSG_OK )
	  		{
				strcat( update_all_status, "T1" );
	  		}
	   		else
	   		{
         		strcat( update_all_status, "T0" );
	   		}
   	   }

   }

   /* See if card record needs to be updated. */
   if (p_auth_tx->TLF01_details.update_mask[0] & CCF02P_MASK)
   {
      /* Yes */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinCCF02P (&p_auth_tx->CCF02P_details,MODE_UPDATE);
		  }
	   if ( DB_Timing_Flag == true )
         start_time = ptetime_get_time();

      result_code = db_update_ccf02p(&p_auth_tx->CCF02P_details, err_msg);

      if ( DB_Timing_Flag == true )
      {
         end_time = ptetime_get_time();
         duration = end_time - start_time;
         update_timing_stats( ST1_DB_UPDATE, 1, CCF02P_DATA,
                              (float)duration, &TimingStats );
      }

      if ( result_code == PTEMSG_OK )
         strcat( update_all_status, "A1" );
      else
         strcat( update_all_status, "A0" );
   }

   if ((p_auth_tx->TLF01_details.update_mask[0]) & CCF03P_MASK)
   {
      if ( result_code == PTEMSG_OK )
      {
         /* Depending on ccf03_insert flag, either do a insert or an update */
         if (p_auth_tx->ccf03_insert)
         {
			  if (DB_NullCheck_Flag==1)
			  {
				  CheckNULLTerminatedinCCF03 (&p_auth_tx->CCF03P_details,MODE_INSERT);
			  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_insert_ccf03p(&p_auth_tx->CCF03P_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, 1, CCF03P_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "B1" );
            else
               strcat( update_all_status, "B0" );
         }
         else
         {
   		  if (DB_NullCheck_Flag==1)
   		  {
   			  CheckNULLTerminatedinCCF03 (&p_auth_tx->CCF03P_details,MODE_UPDATE);
   		  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_update_ccf03p(&p_auth_tx->CCF03P_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, 1, CCF03P_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "C1" );
            else
               strcat( update_all_status, "C0" );
         }
      }
   }

   if ((p_auth_tx->TLF01_details.update_mask[0]) & OFFUS_VELOCITY_MASK)
   {
      if ( result_code == PTEMSG_OK )
      {
         /* Depending on ccf03_insert flag, either do a insert or an update */
         if (p_auth_tx->offus_velocity_insert)
         {
			  if (DB_NullCheck_Flag==1)
			  {
				  CheckNULLTerminatedinOffus_velocity(&p_auth_tx->offus_velocity_details,MODE_INSERT);
			  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_insert_offus_velocity(&p_auth_tx->offus_velocity_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, 1, OFFUS_VELOCITY_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "D1" );
            else
               strcat( update_all_status, "D0" );
         }
         else
         {
     		  if (DB_NullCheck_Flag==1)
     		  {
     			  CheckNULLTerminatedinOffus_velocity (&p_auth_tx->offus_velocity_details,MODE_UPDATE);
     		  }
        	 if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            result_code = db_update_offus_velocity(&p_auth_tx->offus_velocity_details, err_msg) ;

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, 1, OFFUS_VELOCITY_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( result_code == PTEMSG_OK )
               strcat( update_all_status, "E1" );
            else
               strcat( update_all_status, "E0" );
         }
      }
   }

   if (p_auth_tx->TLF01_details.update_mask[0] & ACF01_MASK)
   {
      if ( result_code == PTEMSG_OK )
      {
         /* Update BALANCE ACCOUNT(S) */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinACF01 (&p_auth_tx->ACF01_details,MODE_UPDATE);
		  }
    	  if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         result_code = db_update_acf01(&p_auth_tx->ACF01_details, err_msg) ;

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, 1, ACF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (result_code == PTEMSG_OK)
         {
	          strcat( update_all_status, "F1" );
         }     
         else
         {
         	   strcat( update_all_status, "F0" );
         }
      }
   }

   if ( result_code != PTEMSG_OK )
   {
      result_code = PTEMSG_UPDATE_FAILED;
   }
   else if (strcmp(p_auth_tx->TLF01_details.response_code,"00")== 0)
   {
      if((strcmp(p_auth_tx->TLF01_details.dispensation.origin, "LO" ) == 0) &&
                (msg_sub_type2 == 0))
      {
         switch(p_auth_tx->TLF01_details.tx_key)
         {
            case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE:

                  /* Deferred transactions from Voice are treated as Auths.
                   * Online deferreds are treated as credit sales.
                   */
                  if (0==strcmp(p_auth_tx->TLF01_details.handler_queue,"VOICE"))
                  {
                     result_code = process_auth_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "H1" );
                     else
                        strcat( update_all_status, "H0" );
                  }
                  else
                  {
                     result_code = process_sale_trans_mp(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "I1" );
                     else
                        strcat( update_all_status, "I0" );
                  }
            break;

            case AUTH_AUTHORIZATION_RESPONSE:
            case AUTH_PRE_AUTHORIZATION_RESPONSE:
            case AUTH_CARD_VERIFICATION_RESPONSE:
            case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE:
            case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:		
            case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
            case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE	:	            
                     result_code = process_auth_trans(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "J1" );
                     else
                        strcat( update_all_status, "J0" );
            break;

            case AUTH_SALE_RESPONSE:
            case AUTH_CASH_ADVANCE_RESPONSE:
            case AUTH_SALES_COMPLETION_RESPONSE:
            case AUTH_OFFLINE_SALE_RESPONSE:
            case AUTH_QUASI_CASH_RESPONSE:
            case AUTH_RELOAD_RESPONSE:
                     result_code = process_sale_trans_mp(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "K1" );
                     else
                        strcat( update_all_status, "K0" );
            break;

            case AUTH_REFUND_RESPONSE:
            case AUTH_OFFLINE_REFUND_RESPONSE:           
                     result_code = process_refund_trans_mp(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "L1" );
                     else
                        strcat( update_all_status, "L0" );
            break;

            /* 12/06/99 according to Gabriel for VOID and ADJUSTMENT
             * we do not need to update RRN(replacing original with the new one)
             */
            case AUTH_VOID_SALE_RESPONSE:
            case AUTH_VOID_REFUND_RESPONSE:
            case AUTH_VOID_CASH_ADVANCE_RESPONSE:
            case AUTH_OFFLINE_VOID_SALE_RESPONSE:
            case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
            case AUTH_VOID_RELOAD_RESPONSE:
                     if ((p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE) &&
                         (p_auth_tx->TLF01_details.deferred_term_length[0] != 0x00  )  )
                     {
                        /* This is the void of a deferred.
                         * If it is from Voice, do nothing here because it
                         * is in BCH20.
                         */
                        if (0!=strcmp(p_auth_tx->TLF01_details.handler_queue,"VOICE"))
                        {
                           result_code = process_void_trans_mp(p_auth_tx);
                           if ( result_code == PTEMSG_OK )
                              strcat( update_all_status, "M1" );
                           else
                              strcat( update_all_status, "M0" );
                        }
                     }
                     else
                     {
                        result_code = process_void_trans_mp(p_auth_tx);
                        if ( result_code == PTEMSG_OK )
                           strcat( update_all_status, "N1" );
                        else
                           strcat( update_all_status, "N0" );
                     }
            break;

            case AUTH_SALE_ADJUSTMENT_RESPONSE: 
            case AUTH_REFUND_ADJUSTMENT_RESPONSE: 
                     result_code = process_adjustments_mp(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "O1" );
                     else
                        strcat( update_all_status, "O0" );
            break;

            case AUTH_REVERSAL_RESPONSE:
                     result_code = process_reversal_mp(p_auth_tx);
                     if ( result_code == PTEMSG_OK )
                        strcat( update_all_status, "P1" );
                     else
                        strcat( update_all_status, "P0" );
            break;

            case AUTH_RELOAD_CONFIRM_RESPONSE:
                     /* Do not want to error out on Reload Confirms. */
                     memcpy( &tlf01, &p_auth_tx->TLF01_details, sizeof(TLF01) );

                     if ( DB_Timing_Flag == true )
                        start_time = ptetime_get_time();

                     if ( PTEMSG_OK != db_update_reload_confirm( &tlf01, err_msg))
                       updateds_log_message(1,2,err_msg,"process_alldata_msg");

                     if ( DB_Timing_Flag == true )
                     {
                        end_time = ptetime_get_time();
                        duration = end_time - start_time;
                        update_timing_stats( ST1_DB_UPDATE, 1, TLF01_DATA,
                                             (float)duration, &TimingStats );
                     }
            break;

            default:
                     PRINT("unknown response received by updateds\n");
                     result_code = PTEMSG_INVALID_DATATYPE;
                     strcat( update_all_status, "Q0" );
                     sprintf( err_msg,
                             "Unknown tran type %c: for ALL_DATA request",
                              p_auth_tx->TLF01_details.tx_key);
                     updateds_log_message(2,2,err_msg,"process_alldata_msg_MP");
            break;
         }
      }
      else if ((strcmp(p_auth_tx->TLF01_details.dispensation.origin, "RO" ) == 0) ||
               (msg_sub_type2 != 0 ))
      {
         result_code = process_network_request( p_auth_tx ,msg_sub_type2);
         if ( result_code == PTEMSG_OK )
            strcat( update_all_status, "R1" );
         else
            strcat( update_all_status, "R0" );
      }
      else
      {
         result_code = PTEMSG_INVALID_DATATYPE;
         strcat( update_all_status, "S0" );
      }
   }

   if ( result_code == PTEMSG_OK )
   {
      /* The above updates/inserts were successful.
       * Insert into TLF01 if record has not already been inserted.
       * Insert into TLF01 is part of update_all because EB uses
       * TLF01 as a source for settlement data.  So we cannot respond
       * to the device/network until we know the database has been
       * updated properly.  If the above updates/inserts succeed, but
       * insert into TLF01 fails, rollback the above updates/inserts;
       * transaction will be declined.
       */

      if ( p_auth_tx->host2_acquirer_cb_code[0] != '*' )
      {
         /* Populate system_date field - date txn is inserted into database. */
         memcpy( &tlf01, &p_auth_tx->TLF01_details, sizeof(TLF01) );
         create_system_date( tlf01.system_date );

         if ( p_auth_tx->TLF01_details.tran_finish_time[0] == 0x00 )
            ptetime_get_strtime( tlf01.tran_finish_time );

         /* Store status info for audit trail. */
         if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis3") &&
        	0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN,6))
         {
             memcpy( tlf01.down_payment,
                     update_all_status,
                     sizeof(tlf01.down_payment)-1 );
         }
         else
         {
         memcpy( tlf01.dcf01_retired_cwk,
                 update_all_status,
                 sizeof(update_all_status)-1 );
         }

         memset(&db_recs.tlf01, 0, sizeof(db_recs.tlf01));
         memcpy(&db_recs.tlf01, &tlf01, sizeof(TLF01));

         /* Convert message type to a request type, for standardization. */
         convert_msgtype_to_request( db_recs.tlf01.message_type );

         /* Set flag to indicate to device/network
          * controllers TLF01 has been inserted.
          */
         p_auth_tx->host2_acquirer_cb_code[0] = '*';

         /* Insert into TLF01. */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinTLF01 (&db_recs.tlf01,MODE_INSERT);
		  }
         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         result_code = db_insert_tlf01(&db_recs.tlf01, err_msg);

	/* Insert into TLF01_STIP table as well when inserting into TLF01 for STIP Tx - Girija Y ThoughtFocus */
		 if(( strcmp(db_recs.tlf01.response_code,"00") == 0 ) &&
			(strcmp(db_recs.tlf01.product_codes[1].code,STANDIN) == 0))
		 {
			 /* Need to update STAND in flag in case of VOID / Reverse.. TF PHANI */
			 strcpy(db_recs.tlf01.product_codes[7].amount, p_auth_tx->ch_billing_amt);
			 strcpy(db_recs.tlf01.product_codes[7].code, p_auth_tx->ch_billing_curr_code);
			/* Store DE10 Billing Conversion rate in .TF Phani*/
			 strcpy(db_recs.tlf01.product_codes[8].amount, p_auth_tx->ch_billing_conv_rate);
			 	/* Store industry_code .TF Phani*/
			 strcpy(db_recs.tlf01.product_codes[8].code, p_auth_tx->industry_code);
			 /* TF phani - Updates for 0120 MCARD fields*/
			 /* DE 61 value - which need to be populated during 0120 from sbtach*/
			 strcpy(db_recs.tlf01.source_key,p_auth_tx->acceptor_term_name);
	 //
			 /* Store DE43 here so it gets logged to TLF01:
				*   Merchant Name         --> TLF01.card_holder_name
				*   City and Country Code --> TLF01.ncf01_retired_cwk
				*  TF Phani
				*/
			   if ( db_recs.tlf01.card_holder_name[0] == 0x00 )
			   {
				  memcpy( db_recs.tlf01.card_holder_name,
						  p_auth_tx->MCF01_details.name02,
						  22 );
			   }

			   if ( db_recs.tlf01.ncf01_retired_cwk[0] == 0x00 )
			   {
				  memcpy( db_recs.tlf01.ncf01_retired_cwk,
						  p_auth_tx->MCF01_details.city,
						  13 );
				  itemp = strlen(p_auth_tx->MCF01_details.city);
				//  memset(&db_recs.tlf01.ncf01_retired_cwk[itemp],0x20,13-itemp);	
				  memcpy(&db_recs.tlf01.ncf01_retired_cwk[13], "PHL", 3 );
			   }


			   //Storing converted currency and amount while dual currency //praneeth
			   if ( 0 == strcmp(db_recs.tlf01.host_start_time, "7.000") )
			   {
					strcpy( db_recs.tlf01.currency_code, "840" );
					if (strcmp(db_recs.tlf01.product_codes[9].amount, "")) 
					{
						strcpy(amountbuff,db_recs.tlf01.total_amount);
						strcpy(db_recs.tlf01.total_amount,db_recs.tlf01.product_codes[9].amount);
						strcpy(db_recs.tlf01.product_codes[9].amount,amountbuff);
					}
			   }
			   else if ( 0 == strcmp(db_recs.tlf01.host_start_time, "5.000") )
			   {
   					strcpy( db_recs.tlf01.currency_code, "608" );
					if (strcmp(db_recs.tlf01.product_codes[9].amount, "") )
					{
						strcpy(amountbuff,db_recs.tlf01.total_amount);
						strcpy(db_recs.tlf01.total_amount,db_recs.tlf01.product_codes[9].amount);
						strcpy(db_recs.tlf01.product_codes[9].amount,amountbuff);
					}
			   }


				// We will add batch number as active for stip MP transaction only.
				if(strcmp(db_recs.tlf01.acquirer_id,"nciftr") == 0)
				{
				   strcpy(db_recs.tlf01.batch_number,"ACTIVE");
				}

			 // No Need to insert RC 96 records in TLF01_Stip table.
			 //They might be send again if it was send online after it failed durring stip
			 if( 0 == strncmp(p_auth_tx->TLF01_details.response_code,"96",2))
			 {
				 // Do nothing , no need to updated in DB.
			 }
			 else if( (db_recs.tlf01.tx_key == AUTH_VOID_SALE)||(db_recs.tlf01.tx_key ==AUTH_VOID_SALE_RESPONSE))
			 {
				 strcpy(db_recs.tlf01.product_codes[1].code,VOIDED);
				 result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);

			 }
			 else if((db_recs.tlf01.tx_key == AUTH_REVERSAL)||(db_recs.tlf01.tx_key == AUTH_REVERSAL_RESPONSE))
			 {
				  strcpy(db_recs.tlf01.product_codes[1].code,REVERSED);
				  /* For voice txn the stan is diffrent for sale and cancel sale txn
				  The query which is their in db_update_tlf01_stip_reversal not able to update original txn
				  so copy the original STAN into txn STAN TF-Ajay 10-Jul-2009 */
				  if(strcmp(db_recs.tlf01.handler_queue, "VOICE")==0)
				  {
					  strcpy(db_recs.tlf01.sys_trace_audit_num,db_recs.tlf01.orig_sys_trace_num);
				  }
				   result_code =db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
				  }
			 /*else if((db_recs.tlf01.tx_key == AUTH_SALE_ADJUSTMENT)||(db_recs.tlf01.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE))
			 {
				 stip_result_code = db_update_tlf01_stip_adjust(&db_recs.tlf01, err_msg);
			 }*/
			 else
			 {
			     stip_result_code = db_insert_tlf01_stip(&db_recs.tlf01, err_msg);
			 }
		 }
		 
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_INSERT, 1, TLF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if ( result_code == PTEMSG_OK )
            strcat( update_all_status, "U1" );
         else
            strcat( update_all_status, "U0" );

         /* If EMV txn, insert into EMV01 table. */
         if ( p_auth_tx->EMV_details.currency_code[0] != 0x00 )
         {
            /* There is EMV data.  Commit the updates so far
             * to avoid a large rollback.  Then insert EMV records.
             * But do not change result code from TLF01 insert.
             */
            commit_the_update(); 

            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            rc = store_emv_records( p_auth_tx );

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_INSERT, 1, EMV01_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( rc == PTEMSG_OK )
               strcat( update_all_status, "V1" );
            else
               strcat( update_all_status, "V0" );

            /* Update Chip Condition Code in DCF01. */
            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

            rc = update_chip_condition( p_auth_tx );

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, 1, DCF01_DATA,
                                    (float)duration, &TimingStats );
            }

            if ( rc == PTEMSG_OK )
               strcat( update_all_status, "W1" );
            else
               strcat( update_all_status, "W0" );

            /* Commit the EMV inserts. */
            dbcommon_commit();
            dstimer_commit();
         }
      }
	  else
	  {
   		 if((strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0))
			 //p_auth_tx->
		{
   			 // No Need to insert RC 96 records in TLF01_Stip table.
   			 //They might be send again if it was send online after it failed durring stip
   			 create_system_date( p_auth_tx->TLF01_details.system_date );
   			 if( 0 == strncmp(p_auth_tx->TLF01_details.response_code,"96",2))
   			 {
   				 // Do nothing , no need to updated in DB.
   			 }
			 if( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)||(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE))
			 {
				 strcpy(p_auth_tx->TLF01_details.product_codes[1].code,REVERSED);
				 result_code = db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
			 }
			 else if( (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE)||(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE))
			 {
				 strcpy(p_auth_tx->TLF01_details.product_codes[1].code,VOIDED);
				 result_code = db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
			 }
			 else if( (p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE)||(p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE_RESPONSE))
		 	 {
		 	 	 stip_result_code = db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
		 	 }
			 if(result_code != PTEMSG_OK  )
			 {/* TF Phani - This could be an Online approval/Offline reversal case*/
				strcpy(err_msg,"");
				result_code =db_insert_tlf01_stip(&p_auth_tx->TLF01_details, err_msg);
			 }
				       dbcommon_commit();
		}

	}

      if(p_auth_tx->future_use_3[0]!='\0' &&
    		  0==strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3))
      {
    	  memset( &db_recs.ptsn01, 0, sizeof( db_recs.ptsn01 ) );
    	  strcpy(db_recs.ptsn01.transaction_id,p_auth_tx->TLF01_details.primary_key.transaction_id);
    	  strcpy(db_recs.ptsn01.system_date,db_recs.tlf01.system_date);
    	  strncpy(db_recs.ptsn01.total_length,p_auth_tx->future_use_3,2);
    	  strncpy(db_recs.ptsn01.length_serial_no,p_auth_tx->future_use_3+2,2);
    	  strncpy(db_recs.ptsn01.table_id,p_auth_tx->future_use_3+4,2);
    	  serial_number_len=atoi(db_recs.ptsn01.total_length)-2;
    	  strncpy(db_recs.ptsn01.serial_number,p_auth_tx->future_use_3+6,serial_number_len);


    	  result_code =db_insert_ptsn01(&db_recs.ptsn01, err_msg);
    	 // dbcommon_commit();
      }

   }
   else
   {
      /* The above updates/inserts failed.  Do not insert into TLF01 here,
       * because it will get rolled back.  Leave it to the calling application
       * set the response code to decline and insert into TLF01.
       */
   }

   /* Store status info for audit trail. */
   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis3",6) &&
  	0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN,6))
   {
       memcpy( tlf01.down_payment,
               update_all_status,
               sizeof(tlf01.down_payment)-1 );
   }
   else
   {
       memcpy( tlf01.dcf01_retired_cwk,
           update_all_status,
           sizeof(update_all_status)-1 );
   }

   return( result_code );
}


pPTE_MSG process_update_msg(pPTE_MSG p_msg_in)
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   INT            retry;
   LONG           data_length;
   AUTH_TX        auth_tx;
   FILE 		  *lastmsgptr=NULL_PTR;
   INT 			  Last_Message_len = 0;
   char			  last_tran__msg[256] = {0};

   data_length   = sizeof(AUTH_TX);
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data(p_msg_in);
   p_data        = ptemsg_get_pte_msg_data_data(p_msg_data);
   memcpy( atp_buffer, p_data, data_length );
   memcpy( &auth_tx, p_data, data_length );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);

   /*************Last transaction to be updated in DB************/
   lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
	if(lastmsgptr != NULL_PTR)
	{

		sprintf( last_tran__msg,
			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
			   auth_tx.TLF01_details.card_num,
			   auth_tx.TLF01_details.sys_trace_audit_num,
			   auth_tx.TLF01_details.terminal_id,
			   auth_tx.TLF01_details.merchant_id,
			   auth_tx.TLF01_details.acquirer_id,
			   auth_tx.TLF01_details.handler_queue);


		Last_Message_len=strlen(last_tran__msg);

		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
		fclose(lastmsgptr);
	}

   if (app_data_type == ALL_DATA)
   {
   	  if((0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))||
	     (0 == strncmp(auth_tx.TLF01_details.card_num, "7",1)))
	  {
      		result_code = process_alldata_msg_mp( &auth_tx );
   	  }
	  else
	  {
			result_code = process_alldata_msg( &auth_tx );
	  }
   	/*  memcpy( auth_tx.TLF01_details.dcf01_retired_cwk,
              update_all_status,
   	          sizeof(update_all_status)-1 ); */

      memcpy( atp_buffer, &auth_tx, data_length );
   }
   else if ( app_data_type == NCF21_DATA )
   {
      result_code = process_network_request( &auth_tx, msg_sub_type2 );
      memcpy( atp_buffer, &auth_tx, data_length );
   }
   else if ( app_data_type == TLF01_DATA )
   {
	  if (DB_NullCheck_Flag==1)
	  {
		  CheckNULLTerminatedinTLF01 (&auth_tx.TLF01_details,MODE_UPDATE);
	  }
      result_code = db_update_tlf01( &auth_tx.TLF01_details, err_msg );
      memcpy( atp_buffer, &auth_tx, data_length );
   }
   else if ( app_data_type == FGUARD_DATA )
   {
      p_data = p_data + sizeof(AUTH_TX);

      /* This is a response from Fraudguard.
       * Update the record already in FGUARD from the request insert.
       * If record is not found, it means the insert has not yet
       * completed.  Wait, then try again.
       */
      retry = 0;
      do
      {
         if ( retry > 0 )
         {
            /* Wait 20 ms, then try again. */
            #ifdef WIN32
               Sleep( 20 );
            #else
               usleep( 20000 );
            #endif

            if ( retry == 5 )
            {
               #ifdef WIN32
                  Sleep( 100 );
               #else
                  usleep( 100000 );
               #endif
            }
         }
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinFGUARD ((pFGUARD)p_data,MODE_UPDATE);
		  }
         result_code = db_update_fguard( (pFGUARD)p_data, err_msg );

      }while( (result_code == PTEMSG_NOT_FOUND) && (++retry < 6) );
   }
   else
   {
      result_code = PTEMSG_INVALID_DATATYPE;
      sprintf( err_msg,
              "Unknown app_data_type: %s for Update request from %s, stan: %s",
               app_data_type,reply_que,auth_tx.TLF01_details.sys_trace_audit_num);
      updateds_log_message(2,2,err_msg,"process_update_msg");
   }

   if (result_code == PTEMSG_OK)
   {
      updates_to_db_made = true;
      p_msg_out = ptemsg_build_msg(MT_DB_REPLY,
                                   msg_sub_type1,
                                   msg_sub_type2,
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)&auth_tx,
                                   data_length,
                                   app_data_type);

      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf( err_msg,
                 "Insufficient Memory to build Update response to %s",
                  reply_que);
      }
   }
   return(p_msg_out);
}

/******************************************************************************
 *
 *  NAME:         CREATE_SYSTEM_DATE
 *
 *  DESCRIPTION:  This procedure creates the current system date into
 *                format YYYYMMDD.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       None
 *  OUTPUTS:      system_date  : Format = "YYYY0M0D"
 *  RTRN VALUE:   None
 *
 ******************************************************************************/
void create_system_date( pBYTE system_date ) 
{
   CHAR  time_date[25] = "";

   memset( system_date,  0x00, 9 );

   /* Get local time - Format = "YYYY-0M-0D-0H.0I.0S.JJJ" */
   ptetime_get_timestamp( time_date );

   /* Format it. */
   strncpy( system_date,  time_date,    4 ); 
   strncat( system_date, &time_date[5], 2 ); 
   strncat( system_date, &time_date[8], 2 );
   return;
}

void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
{
   INT i;
   INT dif;
   INT strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }
}
/******************************************************************************
 *
 *  NAME:         CREATE_SYSTEM_TIME
 *
 *  DESCRIPTION:  This procedure creates the current system time into
 *                format hhmmss.jjj.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       None
 *  OUTPUTS:      system_time  : Format = "hhmmss.jjj"
 *  RTRN VALUE:   None
 *
 ******************************************************************************/
void create_system_time( pCHAR system_time ) 
{
   CHAR  time_date[25] = "";

   memset( system_time,  0x00, 11 );

   /* Get local time - Format = "YYYY-0M-0D-0H.0I.0S.JJJ" */
   ptetime_get_timestamp( time_date );

   /* Format it. */
   strncpy( system_time, &time_date[11], 2 ); 
   strncat( system_time, &time_date[14], 2 ); 
   strncat( system_time, &time_date[17], 6 );
   return;
}
/******************************************************************************
 *
 *  NAME:         CONVERT_MSGTYPE_TO_REQUEST
 *
 *  DESCRIPTION:  This procedure converts a message type from a response
 *                type into a request type.  For example, a sale response
 *                of 0210 will be converted to a sale request of 0200.
 *                message type is already a request, then it is not modified.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       msgtype - Message type
 *  OUTPUTS:      msgtype - Modified message type
 *  RTRN VALUE:   None
 *
 ******************************************************************************/
void convert_msgtype_to_request( pBYTE msgtype ) 
{
   if ( msgtype[2] == '1' )
      msgtype[2] = '0';

   else if ( msgtype[2] == '3' )
      msgtype[2] = '2';
   return;
}


/**********************************************************************************
 *
 *
 *
 **************************************************************************************/
BYTE store_pad_raw_records(pAUTH_TX p_auth_tx)
{
	   BYTE   b_len[4]			= {0};
	   CHAR   temp_len[4]		= {0};
	   CHAR   temp_str[256]		= {0};
	   CHAR   time_date[25] 	= {0};
	   BYTE   system_date[9]	= {0};
	   BYTE   tempascii_len[10]	= {0};
	   BYTE   ErrorMsg[100]		= {0};
	   INT	  len				= {0};
	   PAD_RAW  padraw;
	   BYTE   rc 				= PTEMSG_OK;

	   memset(padraw.RAW_DATA,0x00,sizeof(padraw.RAW_DATA));
	   /* GET SYSTEM DATE */
	   ptetime_get_timestamp( time_date );
	   strncpy( system_date,  time_date,    4 ); /* YYYY-0M-0D-0H.0I.0S.JJJ */
	   strncat( system_date, &time_date[5], 2 );
	   strncat( system_date, &time_date[8], 2 ); /* YYYYMMDD */

	   /* Set the constants */
	   memset( padraw.transaction_id, 0x00, sizeof(padraw.transaction_id) );
	   memset( padraw.system_date,    0x00, sizeof(padraw.system_date)    );

	   memcpy( padraw.transaction_id,
	           p_auth_tx->TLF01_details.primary_key.transaction_id,
	           sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
	   memcpy( padraw.system_date, system_date, sizeof(system_date) );

	   len = p_auth_tx->future_use_1[0];
	   memcpy( padraw.RAW_DATA , p_auth_tx->future_use_1+1 , len  );
	   sprintf( padraw.DATA_LENGTH, "%d", len );


	   rc = db_insert_pad_raw( &padraw, ErrorMsg );

	   if ( rc != PTEMSG_OK )
	   {
	      memset( temp_str, 0x00, sizeof(temp_str) );
	      sprintf( temp_str,
	              "Insert PAD_RAW failed, for transaction id: %s, erro : %s ", p_auth_tx->TLF01_details.primary_key.transaction_id,ErrorMsg );
	      updateds_log_message(1,2,temp_str,"store_pad_raw_records");
	   }
	   return( rc );
}

/******************************************************************************
 *
 *  NAME:         STORE_EMV_RECORDS
 *
 *  DESCRIPTION:  This procedure stores each EMV tag as an individual
 *                record into the EMV01 table.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       p_auth_tx - Transaction Information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   0 if successful, else non-zero Oracle error code
 *
 ******************************************************************************/
BYTE store_emv_records( pAUTH_TX p_auth_tx )
{
   BYTE   b_len[4];
   CHAR   temp_len[4];
   CHAR   temp_str[256];
   CHAR   time_date[25] = "";
   BYTE   system_date[9]= "", tempascii_len[10]={0};
   BYTE   rc = PTEMSG_OK;
   INT    len;
   EMV01  emv01;
   EMV_RAW  emvraw;
   CHAR  temp_emv_len[4]={0};
#if MC06_OCT2023
   INT len_9F60=0,asci_len=0;
   char str_9F60[257]={0};
   char ascii_str_9F60[257]={0};
   char str_96[17]={0};
#endif
   /* GET SYSTEM DATE */
   ptetime_get_timestamp( time_date );
   strncpy( system_date,  time_date,    4 ); /* YYYY-0M-0D-0H.0I.0S.JJJ */
   strncat( system_date, &time_date[5], 2 ); 
   strncat( system_date, &time_date[8], 2 ); /* YYYYMMDD */

   /* Set the constants */
   memset( emv01.transaction_id, 0x00, sizeof(emv01.transaction_id) );
   memset( emv01.system_date,    0x00, sizeof(emv01.system_date)    );

   memset( &emvraw,  0x00, sizeof(EMV_RAW)    );

   memcpy( emv01.transaction_id,
           p_auth_tx->TLF01_details.primary_key.transaction_id,
           sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
   memcpy( emv01.system_date, system_date, sizeof(system_date) );

   memcpy( emvraw.transaction_id,
           p_auth_tx->TLF01_details.primary_key.transaction_id,
           sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
   memcpy( emvraw.system_date, system_date, sizeof(system_date) );

   /* ------------------------------------- */
   /* Check each tag to see if it has data. */
   /* ------------------------------------- */

   /* CURRENCY CODE */
   if ( p_auth_tx->EMV_details.currency_code[0] != 0x00 )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "5F2A" );
      len = strlen( p_auth_tx->EMV_details.currency_code );
      sprintf( emv01.len, "%d", len );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.currency_code, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* ISSUER SCRIPT 1 */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.issuer_script_1[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memset( temp_len,        0x00, sizeof(temp_len)       );
      memset( b_len,           0x00, sizeof(b_len)          );

      memcpy( emv01.tag, p_auth_tx->EMV_details.issuer_script_1+2, 2 );

      /* Skip length and tag bytes to get the ASCII length of tag data. */
      memcpy( temp_len, p_auth_tx->EMV_details.issuer_script_1+4, 2 );
      len = hex_to_int( temp_len, 2 );
      if ( len > 0 )
      {
         sprintf( emv01.len, "%d", len );

         len = strlen( p_auth_tx->EMV_details.issuer_script_1+6 );
         memcpy( emv01.emv_data, p_auth_tx->EMV_details.issuer_script_1+6, len );
         rc = db_insert_emv01( &emv01, err_msg );
      }
   }

   /* ISSUER SCRIPT 2 */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.issuer_script_2[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcb3",6))
      {
    	  strcpy( emv01.tag, "9F6E" );
    	  len = 8;
    	  sprintf( emv01.len, "%d", len );
    	  memcpy( emv01.emv_data, p_auth_tx->EMV_details.issuer_script_2, len );
    	  rc = db_insert_emv01( &emv01, err_msg );

    	  if(rc == PTEMSG_OK)
    	  {
    		  memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
    		  memset( &emv01.len,      0x00, sizeof(emv01.len)      );
    		  memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

    		  strcpy( emv01.tag, "9F7C" );
    		  len = strlen( p_auth_tx->EMV_details.issuer_script_2+8 );
    		  sprintf( emv01.len, "%d", len );
    		  memcpy( emv01.emv_data, p_auth_tx->EMV_details.issuer_script_2+8, len );
    		  rc = db_insert_emv01( &emv01, err_msg );

    	  }
      }
      else
      {
      strcpy( emv01.tag, "9F5B" );
      len = strlen( p_auth_tx->EMV_details.issuer_script_2+6 );
      if ( len > 0 )
      {
         sprintf( emv01.len, "%d", len );
         memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
         memcpy( emv01.emv_data, p_auth_tx->EMV_details.issuer_script_2+6, len );
         rc = db_insert_emv01( &emv01, err_msg );
      }
   }
   }

   /* INTERCHANGE PROFILE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.interchange_profile[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "82" );
      len = strlen( p_auth_tx->EMV_details.interchange_profile );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.interchange_profile, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* APPLICATION ID */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.app_id[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "84" );
      len = strlen( p_auth_tx->EMV_details.app_id );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.app_id, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* ISSUER AUTHENTICATION DATA */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.issuer_auth_data[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "91" );

      /* Skip binary length byte to get the ASCII length of tag data. */
      len = p_auth_tx->EMV_details.issuer_auth_data[0];
      sprintf( emv01.len, "%d", len*2 );

      len = strlen( p_auth_tx->EMV_details.issuer_auth_data+1 );
      if ( len > 0 )
      {
         memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
         memcpy( emv01.emv_data, p_auth_tx->EMV_details.issuer_auth_data+1, len );
         rc = db_insert_emv01( &emv01, err_msg );
      }
   }

   /* TERMINAL VERIFICATION RESULT */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.tvr[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "95" );
      len = strlen( p_auth_tx->EMV_details.tvr );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.tvr, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TRANSACTION DATE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.tran_date[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9A" );
      len = strlen( p_auth_tx->EMV_details.tran_date );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.tran_date, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TRANSACTION TYPE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.tran_type[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9C" );
      len = strlen( p_auth_tx->EMV_details.tran_type );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.tran_type, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TRANSACTION AMOUNT */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.tran_amount[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F02" );
      len = strlen( p_auth_tx->EMV_details.tran_amount );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.tran_amount, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* OTHER AMOUNT */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.other_amount[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F03" );
      len = strlen( p_auth_tx->EMV_details.other_amount );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.other_amount, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* VERSION */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.version[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F09" );
      len = strlen( p_auth_tx->EMV_details.version );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.version, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* ISSUER APPLICATION DATA */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.iad[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F10" );
      len = strlen( p_auth_tx->EMV_details.iad );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.iad, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* COUNTRY CODE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.country_code[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      /* Manually prepend a leading zero. Data structure is too small for it.*/
      strcpy( emv01.tag, "9F1A" );
      len = 4;
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      emv01.emv_data[0] = '0';
      memcpy( emv01.emv_data+1, p_auth_tx->EMV_details.country_code, 3 );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* SERIAL NUMBER */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.serial_number[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F1E" );
      len = strlen( p_auth_tx->EMV_details.serial_number );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.serial_number, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* APPLICATION CRYPTOGRAM */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.app_crypto[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F26" );
      len = strlen( p_auth_tx->EMV_details.app_crypto );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.app_crypto, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* CRYPTOGRAM INFORMATION DATA */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.crypto_info[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F27" );
      len = strlen( p_auth_tx->EMV_details.crypto_info );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.crypto_info, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TERMINAL CAPABILITIES */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.terminal_capabilities[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F33" );
      len = strlen( p_auth_tx->EMV_details.terminal_capabilities );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.terminal_capabilities,len);
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* CVM RESULTS */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.cvm_results[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F34" );
      len = strlen( p_auth_tx->EMV_details.cvm_results );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.cvm_results, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TERMINAL TYPE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.terminal_type[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F35" );
      len = strlen( p_auth_tx->EMV_details.terminal_type );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.terminal_type, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* APPLICATION TRANSACTION COUTER */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.atc[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F36" );
      len = strlen( p_auth_tx->EMV_details.atc );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.atc, len );
      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* UNPREDICTABLE NUMBER */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.unpredictable_number[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F37" );
      len = strlen( p_auth_tx->EMV_details.unpredictable_number );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.unpredictable_number, len);

      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TRANSACTION SEQUENCE COUNTER */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.tran_sequence_ctr[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F41" );
      len = strlen( p_auth_tx->EMV_details.tran_sequence_ctr );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.tran_sequence_ctr, len );

      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* TRANSACTION CATEGORY CODE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.tcc[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F53" );
      len = strlen( p_auth_tx->EMV_details.tcc );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.tcc, len );

      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* MERCHANT CATEGORY CODE */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.mcc[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "9F15" );
      len = strlen( p_auth_tx->EMV_details.mcc );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.mcc, len );

      rc = db_insert_emv01( &emv01, err_msg );
   }

   /* PAN SEQUENCE NUMBER */
   if ( (rc == PTEMSG_OK) &&
        (p_auth_tx->EMV_details.pan_sequence_number[0] != 0x00) )
   {
      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

      strcpy( emv01.tag, "5F34" );
      len = strlen( p_auth_tx->EMV_details.pan_sequence_number );
      sprintf( emv01.len, "%d", len );
      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
      memcpy( emv01.emv_data, p_auth_tx->EMV_details.pan_sequence_number, len );

      rc = db_insert_emv01( &emv01, err_msg );
   }
#if MC06_OCT2023
/* Authenticated Application Data (9F60)
 */
   if(0 == strncmp(p_auth_tx->future_use_4,"9F",2))
   {
	   len_9F60 = p_auth_tx->future_use_4[4] ;
		memcpy( str_9F60, p_auth_tx->future_use_4+5, len_9F60 );
		if( p_auth_tx->future_use_4[len_9F60+5] != '\0')
		{
			memcpy( str_96, p_auth_tx->future_use_4+(7+len_9F60), 16 );
		}
   }
   else if(0 == strncmp(p_auth_tx->future_use_4,"96",2))
   {
	   memcpy( str_96, p_auth_tx->future_use_4+2, 16 );
   }

   if ( (rc == PTEMSG_OK) &&
	    (len_9F60 > 0) )
   {
	      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
	      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
	      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

	      /*
	       * binary length will be stored to EMV01 table.
	       * ascii data will be stored in EMV01 table.
	       * Max data length of 9F60 is 128 byte in binary, in ascii its 256 bytes
	       * if ascii length is greater than the 128 store 9F60 in two rows,
	       * tag 9F601(128 ascii data) and 9F602( remaining data)
	       * */
	      genutil_bin_to_ascii (&str_9F60, len_9F60 , &ascii_str_9F60[0], sizeof(ascii_str_9F60));

	      asci_len=len_9F60*2;

	      if(asci_len <= 128)
	      {
	    	  strncpy( emv01.tag, "9F60" ,4);
	    	  sprintf( emv01.len, "%d", asci_len );
	    	  memcpy( emv01.emv_data, ascii_str_9F60, asci_len );
	    	  rc = db_insert_emv01( &emv01, err_msg );
	      }
	      else
	      {
	    	  strncpy( emv01.tag, "9F601" ,5);
	    	  sprintf( emv01.len, "%d", 128 );
	    	  memcpy( emv01.emv_data, ascii_str_9F60, 128 );

	    	  rc = db_insert_emv01( &emv01, err_msg );

	    	  if(rc== PTEMSG_OK)
	    	  {
		    	  strncpy( emv01.tag, "9F602" ,5);
		    	  sprintf( emv01.len, "%d", asci_len-128 );
		    	  memcpy( emv01.emv_data, ascii_str_9F60+128,(asci_len-128) );

		    	  rc = db_insert_emv01( &emv01, err_msg );
	    	  }
	      }
   }

   /*Kernel IdentifierT-erminal (96)*/
   if ( (rc == PTEMSG_OK) &&
	    (str_96[0] != 0x00) )
   {
	      memset( &emv01.tag,      0x00, sizeof(emv01.tag)      );
	      memset( &emv01.len,      0x00, sizeof(emv01.len)      );
	      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );

	      strncpy( emv01.tag, "96" ,2);
	      len = 16 ;
	      sprintf( emv01.len, "%d", len );
	      memset( &emv01.emv_data, 0x00, sizeof(emv01.emv_data) );
	      memcpy( emv01.emv_data, str_96, 16 );

	      rc = db_insert_emv01( &emv01, err_msg );
   }
#endif
   /* RAW EMV DATA */
   if ( (rc == PTEMSG_OK) &&
        ((p_auth_tx->EMV_details.emv_block[0] != 0x00)  ||
         (p_auth_tx->EMV_details.emv_block[1] != 0x00))  )
   {
      /* Determine the block size */

	  if(strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3)== 0 )   
	  {
	  len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 ) + 2;
	  }
	  else if (true == Check_msg_coming_from_IST(p_auth_tx))
	  {
		  len  = p_auth_tx->EMV_details.emv_block[0];
		  len++;
	  }
	  else
	  {
		  if ((strcmp(p_auth_tx->BIN01_details.card_family,CARD_MASTERCARD) == 0 ||
			    strcmp(p_auth_tx->BIN01_details.card_family,CARD_JCB) == 0))
	  	  {
			  len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 ) + 2;
	  	  }
		  else if ((strcmp(p_auth_tx->BIN01_details.card_family,CARD_CUP) == 0) ||
				  (strcmp(p_auth_tx->BIN01_details.card_family,CARD_DINERS) == 0))
				  {   
				    strncpy(tempascii_len,p_auth_tx->EMV_details.emv_block,3);
				  
				  	  len  = atoi(tempascii_len);	  
					  len = len+3;
				  }	
			else
		  {
			  len  = p_auth_tx->EMV_details.emv_block[0];
		  		len++;
		  	}
	  }
      /* Don't copy the length */
      memcpy( emvraw.RAW_DATA , p_auth_tx->EMV_details.emv_block , len  );
      sprintf( emvraw.DATA_LENGTH, "%d", len );
      rc = db_insert_emvraw( &emvraw, err_msg );
   }

   if ( rc != PTEMSG_OK )
   {
      memset( temp_str, 0x00, sizeof(temp_str) );
      sprintf( temp_str,
              "Update EMV01 failed, tag %s: %s, with txn details,  stan: %s",
               emv01.tag , err_msg,p_auth_tx->TLF01_details.sys_trace_audit_num );
      updateds_log_message(1,2,err_msg,"store_emv_records");
   }

   return( rc );
}

/******************************************************************************
 *
 *  NAME:         UPDATE_CHIP_CONDITION
 *
 *  DESCRIPTION:  This procedure stores the value from EMV.chip_condition_code
 *                into DCF01.additional_pos_information.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       p_auth_tx - Transaction Information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   0 if successful, else non-zero Oracle error code
 *
 ******************************************************************************/
BYTE update_chip_condition( pAUTH_TX p_auth_tx )
{
   BYTE  rc;
   CHAR  temp_str[256];

   rc = db_udpate_chip_condition(p_auth_tx, err_msg);
   if ( rc != PTEMSG_OK )
   {
      memset( temp_str, 0x00, sizeof(temp_str) );
      sprintf( temp_str,
              "Update chip condition failed, %s with txn details, stan: %s",
               err_msg,p_auth_tx->TLF01_details.sys_trace_audit_num  );
      updateds_log_message(2,2,err_msg,"update_chip_condition");
   }
   return( PTEMSG_OK );
}

pPTE_MSG process_db_request(pPTE_MSG p_msg_in)
{
   pPTE_MSG       p_msg_out;
   BYTE           MsgSubtype1;
   BYTE           MsgSubtype2;
   CHAR           query_start_time[20]="";
   CHAR           query_end_time  [20]="";
   CHAR           errbuf[120];
   INT            ret_val;
   double         start_time;
   double         end_time;
   double         duration;

   MsgSubtype2 = ptemsg_get_msg_subtype2(p_msg_in);
   MsgSubtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   /* For DB statics added by raghu*/
   DB_MsgSubtype2 = '0';
   DB_MsgSubtype1 = '0';
   DB_MsgSubtype2 = MsgSubtype2 ;
   DB_MsgSubtype1 = MsgSubtype1 ;
   /*end*/
   memset(atp_buffer, 0, sizeof(atp_buffer));
   if ( ds_timer_flag == DS_TIMER_ON )
   {
      /* The functionality to check the time of a query is turned on.
       * Get the time before the query. Will check it after the query.
       */
      ptetime_get_strtime( query_start_time );
   }

   if ( DB_Timing_Flag == true )
   {
      /* The functionality to gather DB Query Timing Statistics is ON.
       * Get system time, then do again after the query.  This is
       * different than the above timings.  That one flags long queries;
       * this one gather stats.
       */
      start_time = ptetime_get_time();
   }

   switch (msg_sub_type1)
   {
      case ST1_DB_UPDATE: p_msg_out = process_update_msg(p_msg_in);
                          break;
      case ST1_DB_INSERT: p_msg_out = process_insert_msg(p_msg_in);
                          break;
      default:
        p_msg_out = NULL_PTR;
        result_code = PTEMSG_INVALID_SUBTYPE1;
        sprintf(err_msg, "Unknown subtype1 for database request from %s", reply_que);
        break;
   }

   if ( DB_Timing_Flag == true )
   {
      /* The functionality to gather DB Query Timing Statistics is ON.
       * Start time has been obtained. Now get end time and calculate
       * the duration and update the statistics.
       */
      end_time = ptetime_get_time();

      duration = end_time - start_time;

      update_timing_stats( MsgSubtype1, MsgSubtype2, app_data_type,
                           (float)duration, &TimingStats );
   }

   if ( ds_timer_flag == DS_TIMER_ON )
   {
      /* The functionality to check the time of a query is turned on.
       * See if the query exceeded desired time length.
       */
      ptetime_get_strtime( query_end_time );
      memset( errbuf, 0x00, sizeof(errbuf) );
      ret_val = compare_query_time_against_criteria( query_start_time,
                                                     query_end_time,
                                                     ds_timer_value,
                                                     p_msg_in,
                                                     ServiceName,
                                                     errbuf );
      if ( ret_val == false )
      {
         updateds_log_message(1,2,errbuf,"Process_DB_Request");
      }
   }
   return(p_msg_out);
}

void process_monitor_request(pPTE_MSG p_msg_in)
{
   CHAR monitor_flag[100]="";
   pCHAR orig_comm;
   CHAR MonAdd[100]="";

   memset(atp_buffer, 0, sizeof(atp_buffer));
   switch (msg_sub_type1)
   {
      case ST1_MONITOR_UP: 
          orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
          WriteMemMonitorTable((BYTE *)orig_comm);  
          break;
      case ST1_MONITOR_DOWN  :
          WriteMemMonitorTable((BYTE *)monitor_flag); 
          break;
      default:
          result_code = PTEMSG_INVALID_SUBTYPE1;
          sprintf(err_msg, "Unknown subtype1 for database request from %s", reply_que);
          break;
   }
   return;
}


void commit_the_update()
{
   if (updates_to_db_made == true)
   {
      dbcommon_commit();
      dstimer_commit();
      updates_to_db_made = false;
   }
   return;
}

BYTE PinnacleMsgHandler(pPTE_MSG p_msg_in)
{
   LONG        i;
   LONG        num_messages;
   LONG        ipc_rcode;
   CHAR        temp_str [80];
   pPTE_MSG    p_cur_msg;
   pPTE_MSG    p_tmp_msg;
   pPTE_MSG    p_chn_out;
   pPTE_MSG    p_new_chn;
   pBYTE       p_data;
   pBYTE       p_data1;
   pPTE_MSG_DATA  p_msg_data;
   AUTH_TX     auth_tx;
   INT            ret_val;
   INT            num_sql;
   int         refresh_flag = 0;  /* If an error in update_mem_record or delete_mem_record,
                                     delete all recs. in mem and recreate all tables over again 
                                     from the database.
                                   */
   static DB_reconnect = 0;

   ptestats_accumulate_msg_stats(p_msg_in);
   result_code = PTEMSG_OK;

   p_cur_msg = p_msg_in;
   p_tmp_msg = NULL_PTR;
   p_chn_out = NULL_PTR;
   i = 1;

   num_messages = ptemsg_get_num_chained_messages(p_msg_in);
   p_msg_data    = ptemsg_get_pte_msg_data(p_msg_in);
   p_data        = ptemsg_get_pte_msg_data_data(p_msg_data);
   memcpy( &auth_tx, p_data, sizeof(AUTH_TX) );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   p_data = p_data + sizeof(AUTH_TX);

   memset( update_all_status, 0x00, sizeof(update_all_status) );
   while ((i <= num_messages) && (p_cur_msg != NULL_PTR))
   {
      reply_que = ptemsg_get_msg_orig_queue(p_cur_msg);
      strcpy(orig_comm_inf, ptemsg_get_msg_orig_comms_info(p_cur_msg));
      app_data_type = 0;
      DB_reconnect = 0;
      msg_sub_type1 = ptemsg_get_msg_subtype1(p_cur_msg);
      msg_sub_type2 = ptemsg_get_msg_subtype2(p_cur_msg);
      memset( err_msg, 0x00, sizeof(err_msg) );

      switch (ptemsg_get_msg_type(p_cur_msg))
      {
         case MT_DB_REQUEST: 
               p_tmp_msg  = process_db_request(p_cur_msg);
               p_msg_data = ptemsg_get_pte_msg_data(p_cur_msg);
               p_data1    = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data1, sizeof(AUTH_TX) );
               break;

         case MT_SYSTEM_REQUEST:
            ptesystm_system_msg_handler(p_cur_msg);
            if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
            {
               if ( ST2_NO_VERSION != ptemsg_get_msg_subtype2(p_msg_in) )
               {
                  /* When pinged, display application version on Monitor. */
                  sprintf( temp_str,
                          "Pinged -> Data Server: %s, version %s",
                           ServiceName, Version );
                 updateds_log_message(1,1,temp_str,"PinnacleMsgHandler");

                  /* Get Database Timing Parameters to gather
                   * statistics about each query.
                   */
                  memset( ReportTime, 0x00, sizeof(ReportTime) );
                  memset( temp_str,   0x00, sizeof(temp_str)   );
                  ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                    ReportTime, 
                                                   &ForceReport,
                                                    temp_str );
                  updateds_log_message(2,1,temp_str,"PinnacleMsgHandler");
                  if ( DB_Timing_Flag == true )
                  {
                     /* Get the time for when the DB Timing
                      * Statistical Report is to be logged.
                      */
                     NextReportTime = set_next_report_time_at_startup( ReportTime );

                     if ( ForceReport == true )
                     {
                        /* User wants to log the stats
                         * right now.  Log them then
                         * reset the statistics.
                         */
                        num_sql = log_timing_statistics_report( &TimingStats );
                        memset( temp_str, 0x00, sizeof(temp_str) );
                        sprintf( temp_str,
                                "%s logged stats for %d SQLs",
                                 app_name, num_sql );
                        updateds_log_message(2,1,temp_str,"PinnacleMsgHandler");

                        reset_timing_stats( &TimingStats );
                     }
                  }
               }
            }
            else if ( ST1_SYS_NULLCHECK_OFF == ptemsg_get_msg_subtype1(p_msg_in))
            {
          	  DB_NullCheck_Flag = 0 ;
          	  send_nullcheck_dbstatics_response(p_msg_in);
            }
            else if ( ST1_SYS_NULLCHECK_ON == ptemsg_get_msg_subtype1(p_msg_in))
            {
          	  DB_NullCheck_Flag = 1 ;
          	  send_nullcheck_dbstatics_response(p_msg_in);
            }
            else if ( ST1_SYS_DBSTATICS_OFF == ptemsg_get_msg_subtype1(p_msg_in))
            {
          	  db_error_statics_flag = 0 ;
          	  send_nullcheck_dbstatics_response(p_msg_in);
            }
            else if ( ST1_SYS_DBSTATICS_ON == ptemsg_get_msg_subtype1(p_msg_in))
            {
          	  db_error_statics_flag = 1 ;
          	  send_nullcheck_dbstatics_response(p_msg_in);
            }
            break;

         case MT_SYS_MONITOR: 
               process_monitor_request(p_cur_msg);
               break;

         default :
            /*
               Received an unknown msg type.  If the request was destined for
               this dataserver, log an error because I can't process it.
               Otherwise, try to forward it on to the actual destination.
            */
            if ((strncmp(ptemsg_get_msg_dest_queue(p_cur_msg), application_que_name, strlen(application_que_name)) == 0) ||
                (strncmp(ptemsg_get_msg_dest_queue(p_cur_msg), control_que_name, strlen(control_que_name)) == 0)
               )
            {
               result_code = PTEMSG_INVALID_MSGTYPE;
               sprintf(err_msg, "Unknown msgtype for request from %s", reply_que);
            }
            else
            {
               sprintf(temp_str, "forwarding message to queue %s\n", 
                       ptemsg_get_msg_dest_queue(p_cur_msg));
               ptemsg_set_orig_comms_info(p_cur_msg, orig_comm_inf);
               ipc_rcode = pteipc_send(p_cur_msg, ptemsg_get_msg_dest_queue(p_cur_msg));

               if (ipc_rcode < 0)
               {
                  result_code = PTEMSG_COULD_NOT_FORWARD;
                  pteipc_get_errormsg(ipc_rcode, temp_str);
                  sprintf(err_msg, "error forwarding to queue %s: %s", 
                  ptemsg_get_msg_dest_queue(p_cur_msg), temp_str);
               }
            }
            break;
      }  /* switch msg_type */

      if (result_code != PTEMSG_OK)
      {
     	 if( NOT_CONNECTED_TO_DB 				== result_code ||
     		 NOT_LOGGED_INTO_DB 				== result_code ||
			 TNS_PACKET_WRITER_FAILED 			== result_code ||
			 SHUTDOWN_OR_STARTUP_IN_PROGRESS	== result_code  ||
			 PTEMSG_NOT_CONNECTED_TO_DB			== result_code )
     	 {
     		 DB_reconnect = 1;
     	 }
     	 else
     	 {
     		DB_reconnect = 0;
     	 }
         break;
      }

      if (p_tmp_msg != NULL_PTR)
      {
         result_code = ptemsg_get_result_code(p_tmp_msg);

         if (result_code != PTEMSG_OK)
         {
        	 if( NOT_CONNECTED_TO_DB 				== result_code ||
        		 NOT_LOGGED_INTO_DB 				== result_code ||
				 TNS_PACKET_WRITER_FAILED 			== result_code ||
				 SHUTDOWN_OR_STARTUP_IN_PROGRESS	== result_code  ||
				 PTEMSG_NOT_CONNECTED_TO_DB			== result_code )
        	 {
        		 DB_reconnect = 1;
        	 }
        	 else
			 {
				DB_reconnect = 0;
			 }
            free(p_chn_out);
            p_chn_out = p_tmp_msg;
            p_tmp_msg = NULL_PTR;
            break;
         }

         if (p_chn_out == NULL_PTR)
         {
            p_chn_out = p_tmp_msg;
            p_tmp_msg = NULL_PTR;
         }
         else
         {
            p_new_chn = ptemsg_chain_message(p_chn_out, p_tmp_msg);

            if (p_new_chn == NULL_PTR)
            {
               result_code = PTEMSG_INSUFFICIENT_MEMORY;
               sprintf(err_msg, "Insufficient Memory to chain response to %s", reply_que);
               break;
            } 
            else
            {
               p_chn_out = p_new_chn;
               p_tmp_msg = NULL_PTR;
            }
         }
      }

      if ((++i) <= num_messages)
         p_cur_msg = ptemsg_get_chained_pte_msg(p_msg_in, i);
   }  /* while */


   /*
    * Be sure to free up the temp message.
    */
   if (p_tmp_msg != NULL_PTR)
   {
      free(p_tmp_msg);
      p_tmp_msg = NULL_PTR;
   }

   /*
    * If there was an error, back out the changes.
    * Otherwise commit the changes.
    */
   reply_que = ptemsg_get_msg_orig_queue(p_msg_in);

   if (result_code != PTEMSG_OK)
   {
      rollback_the_update();
      if (p_chn_out != NULL_PTR)
      {
         free(p_chn_out);
         p_chn_out = NULL_PTR;
      }

      /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).  Also log the error here
       * if this is a Voice transaction so that the error gets logged;
       * VoiceAuth does not log to syslog or Monitor.
       */
      if ( 0 == strcmp(auth_tx.TLF01_details.handler_queue, "VOICE") )
      {
         updateds_log_message(1,2,err_msg,"PinnacleMsgHandler");
      }

      memcpy(atp_buffer + sizeof(AUTH_TX),  err_msg, strlen(err_msg) + 1);
      p_chn_out = ptemsg_build_msg(MT_DB_REPLY,        
                                    msg_sub_type1,
                                    msg_sub_type2,
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) atp_buffer,
                                    strlen(err_msg) + 1 + sizeof(AUTH_TX), 
                                    app_data_type);

      if (p_chn_out == NULL_PTR)
      {
       	  updateds_log_message(1,3,err_msg,"PinnacleMsgHandler");
    	  result_code = PTEMSG_INSUFFICIENT_MEMORY;
    	  sprintf(err_msg, "Insufficient Memory to build error response to %s", reply_que);
      }
   }
   else
   {
      commit_the_update(); 
   }

   if (p_chn_out != NULL_PTR)
   {
      if ((result_code == PTEMSG_OK) &&
          (ptemsg_get_type_of_reply_needed(p_msg_in) == PTEMSG_REPLY_ONLY_IF_FAILS))
      {
         free(p_chn_out);
         return(PTEMSG_OK);
      }

      ptemsg_set_result_code(p_chn_out, result_code);
      ptemsg_set_orig_comms_info(p_chn_out, orig_comm_inf);

      ipc_rcode = pteipc_send(p_chn_out, reply_que);
      free(p_chn_out);
      if (ipc_rcode < 0)
      {
          updateds_log_message(1,2,err_msg,"PinnacleMsgHandler");
    	  result_code = PTEMSG_IPC_ERROR;
          pteipc_get_errormsg(ipc_rcode, temp_str);
          sprintf(err_msg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }

   if(DB_reconnect == 1)
   {
	   dbcommon_disconnect (err_msg);
	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", err_msg) != PTEMSG_OK)
	   {
		   strcat (err_msg, " DB Reconnection failed");
		   updateds_log_message(1, 3, err_msg, "PinnacleMsgHandler");
			return (false);
	   }
	   else
	   {
		   updateds_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler");
	   }

   }
   else
   {
	   DB_reconnect = 0;
   }

   return(result_code);
}

BOOLEAN perform_startup()
{
   CHAR XipcInstanceName[12];
   INT      ret_val;
   CHAR     Buffer[300] = "";

   if (dbcommon_connect("equitable", "equitable", "equitable", "ORC1", err_msg) != PTEMSG_OK)
   {
	   updateds_log_message(1,3,err_msg,"perform_startup");
   	   return(false);
   }
   #ifdef _DEBUG
    printf("Connected to ORACLE\n");
   #endif

    GetAppName(app_name);
    GetXipcInstanceName(XipcInstanceName);

#ifndef WIN32

   /* Diagnostic code to trap the following signals. */
   sigset( SIGALRM, TrapFunction );
   sigset( SIGBUS,  TrapFunction );
   sigset( SIGEMT,  TrapFunction );
   sigset( SIGFPE,  TrapFunction );
   sigset( SIGILL,  TrapFunction );
   sigset( SIGIOT,  TrapFunction );
   sigset( SIGKILL, TrapFunction );
   sigset( SIGPIPE, TrapFunction );
   sigset( SIGPOLL, TrapFunction );
   sigset( SIGPWR,  TrapFunction );
   sigset( SIGQUIT, TrapFunction );
   sigset( SIGSEGV, TrapFunction );
   sigset( SIGSYS,  TrapFunction );
   sigset( SIGTRAP, TrapFunction );
   sigset( SIGUSR1, TrapFunction );
   sigset( SIGUSR2, TrapFunction );

#endif

   //if (pteipc_init_single_instance_app (app_name, XipcInstanceName) == false)
   if (!pteipc_init_multiple_instance_app(app_name, ServiceName, XipcInstanceName))
   {
	   	updateds_log_message(1,3,"Failed to create XIPC queues","perform_startup");
	    pteipc_shutdown_multiple_instance_app();
	    dbcommon_disconnect(err_msg);
	    return(false);
   }

   ret_val = get_dataserver_ini_timer_value( &ds_timer_flag, &ds_timer_value,
                                             ServiceName,     Buffer );
   if ( false == ret_val )
   {
       updateds_log_message(2,2,Buffer,"perform_startup");

   }
   else if ( ds_timer_flag == DS_TIMER_ON )
   {
      sprintf( Buffer,
              "%s:Data Server timing of queries is turned on, value is %d",
               ServiceName, ds_timer_value );
      updateds_log_message(2,2,Buffer,"perform_startup");
   }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   updateds_log_message(2,2,Buffer,"perform_startup");

   /* Get the time for when the DB Timing Statistical Report is to be logged. */
   if ( DB_Timing_Flag == true )
   {
      reset_timing_stats( &TimingStats );
      NextReportTime = set_next_report_time_at_startup( ReportTime );
   }

   ret_val = CreateSQL01_Table();
   if ( ret_val < 0 )
   {
      sprintf(Buffer, "Unable to create SQL01 in memory. Xipc err %d", ret_val);
      updateds_log_message(2,2,Buffer,"perform_startup");
   }

   /* Get the message for cash bonus participants who do not win. */
   get_cash_bonus_ini_info();

   return(true);
}

#ifndef WIN32

void TrapFunction( INT signum )
{
   CHAR Buffer[100];

   XIPC_TRAP_FUNCTION_TEST( TrapFunction, signum );

   memset( Buffer, 0x00, sizeof(Buffer) );
   sprintf( Buffer, "%s:  Caught signal %d", app_name, signum );
   updateds_log_message(2,3,Buffer,"TrapFunction");
}

#endif

void MainProcessor()
{
   LONG     ipc_rcode;
   pPTE_MSG p_msg;
   CHAR     Buffer[256];
   CHAR     MonAdd[100]="";
   INT      num_sql;
   INT      err_ctr = 0;
   CHAR     strError[512] = {0} ;
   int 		ret_val=0;

	   GetAppName( AppName );
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
   updateds_log_message(2,1,Buffer,"MainProcessor");
	printf( "%s\n", Buffer );

   #ifdef _DEBUG
      pteipc_trace_on();
   #endif

   if (perform_startup() == false)
   {
      MainProcessDone = 1;
	  	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
		   updateds_log_message(2,1,Buffer,"MainProcessor");
			printf( "%s\n", Buffer );

      return;
   }
   ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                ServiceName,     Buffer );


   if ( false == ret_val )
   {
       updateds_log_message(2,2,Buffer,"MainProcessor");
   }
   else if ( db_error_statics_flag == DS_TIMER_ON )
   {
         sprintf( Buffer,
                 "%s:Data Server DB error statics for queries is turned on, value is %d",
                  ServiceName, db_error_statics_value );
         updateds_log_message(2,1,Buffer,"MainProcessor");
   }

   strcpy( Last_Tran_Logging_Filename, DB_file_path );
   strcat( Last_Tran_Logging_Filename, ServiceName );
   strcat( Last_Tran_Logging_Filename, "last_tran.log" );

   /* Read CUP MID indicator value from tf.ini*/
   updatds_read_CUP_merchant_indicator();
   updatds_read_system_integrity_parameters();
   updatds_read_execssive_delcines_parameter_for_mc();
   updatds_load_MID_CPNG_response_text();
   get_variable_auth_config_details();
   updatds_get_mcc_of_preauth_in_days_for_smcc();

#ifdef MC13_OCT2023
   updatds_read_merchant_advice_code_parameters();
#endif

   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   updateds_log_message(2,1,Buffer,"MainProcessor");
   printf( "%s\n", Buffer );

   #ifdef _DEBUG
      printf("DataServer is ready. press <ENTER> to exit\n");
   #endif

   while (!EndProcessSignalled)
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for the specified time, the blocking call returns   */
      p_msg = pteipc_receive(application_que_name, control_que_name, 
                             TIME_BETWEEN_CHECKS, &ipc_rcode);
      if (p_msg != NULL_PTR)
      {
         PinnacleMsgHandler(p_msg);
         free(p_msg);
      }
      else if (ipc_rcode != QUE_ER_TIMEOUT) 
      {
         pteipc_get_errormsg(ipc_rcode, err_msg);
         sprintf(Buffer, "Error on pteipc_receive %s", err_msg);
         updateds_log_message(1,2,Buffer,"MainProcessor");
         /* There is a problem with the queue.  Delay 1 second
          * so as to not flood Applnk's queue and syslog.
          */
         #ifdef WIN32
            Sleep( 1000 );
         #else
            usleep( 1000000 );
         #endif

         /* Inform Operators if service's queue is no longer readable. */
         if ( ++err_ctr > 10 )
         {
            err_ctr = 0;
            sprintf( strError,
                    "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
                     ServiceName );
            updateds_log_message(1,3,strError,"MainProcessor");
            #ifdef WIN32
               Sleep (5000);
            #else
               usleep (5000000);
            #endif
         }

         /* Comment the following out after the production stability
          * problem is resolved.
          */
         /* This line is to gather XIPC info *
         memset( XipcErrBuffer, 0x00, sizeof(XipcErrBuffer) );
         XipcSystemErrorReport( XipcErrBuffer, sizeof(XipcErrBuffer)-1 );
         updateds_log_message(2,3,XipcErrBuffer,"MainProcessor");
         exit(1);
         */
      }
      else
      {
         /* The queue read has timed out.  This means there is no
          * activity for this application.  Take this opportunity
          * to see if it is time to log the database timing stats.
          */
         if ( is_it_report_time(NextReportTime) )
         {
            /* Time to report the DB timing measurement statistics. */
            num_sql = log_timing_statistics_report( &TimingStats );

            memset( Buffer, 0x00, sizeof(Buffer) );
            sprintf( Buffer,
                    "%s logged stats for %d SQLs",
                     app_name, num_sql );
            updateds_log_message(2,1,Buffer,"MainProcessor");

            reset_timing_stats( &TimingStats );
            NextReportTime = set_next_report_time_at_startup( ReportTime );
         }
      }
      dstimer_check_for_timeouts();
   }
   dstimer_shutdown();

   pteipc_shutdown_multiple_instance_app(); 
   #ifdef _DEBUG
      printf("pteipc shutdown successful\n");
   #endif

   if (dbcommon_disconnect(err_msg) != PTEMSG_OK)
      printf("%s\n", err_msg);
   else
      printf("Disconnected from ORACLE\n");
   MainProcessDone = 1;
}

void EndProcess()
{
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   updateds_log_message(2,1,Buffer,"EndProcess");
   strcat( Buffer, "\n" );
   PRINT( Buffer );
   pteipc_sleep(3000);
}


/******************************************************************************
 *
 *  NAME:         GET_CASH_BONUS_RESULT
 *
 *  DESCRIPTION:  This function determines if a txn qualifies for the Cash
 *                Bonus program.  If yes, it calls a function to see if it
 *                is a winner.
 *
 *  INPUTS:       p_auth_tx - Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   true if txn is a Cash Bonus winner, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_cash_bonus_result( pAUTH_TX p_auth_tx )
{
   INT   retval = false;
   INT   cash_bonus_result;
   CHAR  errbuf[100];

   /* Does transaction qualify for the Cash Bonus program? */
   if (p_auth_tx->TLF01_details.update_mask[0] & BON01_MASK)
   {
      /* Initialize Global Variables */
      CashBonusWinner = 0;  /* Init to "not a winner". */

      /* Go get all Cash Bonus Programs from BON02. */
      cash_bonus_result = db_process_cash_bonus_programs( p_auth_tx, errbuf );

      if ( cash_bonus_result == NO_CASH_WINNER )
      {
         memcpy( p_auth_tx->TLF01_details.response_text,
                 CashBonusNonWinnerMsg,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      }
      else if ( cash_bonus_result == CASH_WINNER )
      {
         retval = true;
         if ( CashBonusWinner > 1 )
         {
            /* Multiple winner. Let's log the message going to terminal. */
            strcat( update_all_status, "T1" );
            updateds_log_message(1,2,p_auth_tx->TLF01_details.response_text,"get_cash_bonus_result");

         }
      }
      else if ( cash_bonus_result == CASH_PROBLEM )
      {
         strcat( update_all_status, "T0" );
         updateds_log_message(1,2,errbuf,"get_cash_bonus_result");

         strcpy( errbuf, "Warning: problem with Cash Bonus");
         updateds_log_message(1,2,errbuf,"get_cash_bonus_result");
      }
   }
   return( retval );
}


/******************************************************************************
 *
 *  NAME:         PERFORM_CASH_BONUS_PROCESSING
 *
 *  DESCRIPTION:  This function determines if a transaction is a winner in
 *                a Cash Bonus Program.
 *
 *  INPUTS:       p_auth_tx - Transaction information
 *                p_bon02   - Cash Bonus Program record
 *
 *  OUTPUTS:      err_msg - Text message of any system problems
 *
 *  RTRN VALUE:   CASH_WINNER, NO_CASH_WINNER, CASH_PROBLEM
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT perform_cash_bonus_processing(pAUTH_TX p_auth_tx,pBON02 p_bon02,pCHAR err_msg)
{
	#define  PESO     "608"
   #define  CRF01_TABLE  "Crf01Table"

   INT    retval = NO_CASH_WINNER;
   INT    amt_idx;
   INT    db_time_period;  // Indicates which time period is in database
   INT    cur_time_period; // Indicates which time period we are in
   XINT   rt_memaccess;
   LONG   rt_record;
   LONG   start_date; 
   LONG   end_date;
   LONG   tran_date;
   LONG   threshold;
   LONG   tran_ctr;
   LONG   tp;
   LONG   period_winners_max;
   LONG   period_winners_ctr;
   LONG   program_winners_max;
   LONG   program_winners_ctr;
   double low_amt;
   double high_amt;
   double conv_rate;
   double tran_amt;
   CHAR   system_date[50];
   CHAR   date_str[9];
   CRF01  crf01;
   INT    currency_code_size = sizeof(crf01.primary_key.currency_code);


   /*+-------------------------------------+
     | Check Cash Bonus Program date range |
     +-------------------------------------+*/

   /* Get Cash Bonus Program dates.
    * Switch date format from ddmmyyyy to yyyymmdd.
    */
   memset( date_str, 0x00, sizeof(date_str) );
   memcpy( date_str,   p_bon02->program_start_date+4, 4 ); /* YYYYY */
   memcpy( date_str+4, p_bon02->program_start_date+2, 2 ); /* MM    */
   memcpy( date_str+6, p_bon02->program_start_date,   2 ); /* DD    */
   start_date = Txutils_Calculate_Julian_Date( date_str );

   memset( date_str, 0x00, sizeof(date_str) );
   memcpy( date_str,   p_bon02->program_end_date+4, 4 ); /* YYYYY */
   memcpy( date_str+4, p_bon02->program_end_date+2, 2 ); /* MM    */
   memcpy( date_str+6, p_bon02->program_end_date,   2 ); /* DD    */
   end_date = Txutils_Calculate_Julian_Date( date_str );

   /* Get system date */
   memset( system_date, 0x00, sizeof(system_date) );
   memset( date_str,    0x00, sizeof(date_str   ) );
   ptetime_get_timestamp( system_date );
   get_date( system_date, date_str );
   tran_date = Txutils_Calculate_Julian_Date( date_str );

   /* Compare the dates. */
   if ( (tran_date >= start_date) && (tran_date <= end_date) )
   {
      /*+---------------------------------------------+
        | The Cash Bonus Program is currently active. |
        +---------------------------------------------+*/

      tran_ctr  = atol( p_bon02->tran_ctr );
      threshold = atol( p_bon02->tran_threshold );

      period_winners_max = atol( p_bon02->max_period_winners );
      period_winners_ctr = atol( p_bon02->period_win_ctr     );

      program_winners_max = atol( p_bon02->max_program_winners );
      program_winners_ctr = atol( p_bon02->program_win_ctr     );

      /* Do we look for a winner, or to reverse out a txn? */
      if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL) ||
           (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE) )
      {
         /* REVERSAL */
         if ( tran_ctr == 0 )
         {
            /* Last transaction was a winner. Decrement winner count. */
            tran_ctr = threshold - 1;

            period_winners_ctr--;
            if ( period_winners_ctr < 0 )
               period_winners_ctr = 0;

            sprintf( p_bon02->period_win_ctr,  "%d", period_winners_ctr  );

            program_winners_ctr--;
            if ( program_winners_ctr < 0 )
               program_winners_ctr = 0;

            sprintf( p_bon02->program_win_ctr, "%d", program_winners_ctr );
         }
         else
         {
            tran_ctr--;
         }

         if ( tran_ctr < 0 )
            tran_ctr = 0;

         sprintf( p_bon02->tran_ctr, "%d", tran_ctr );
      }
      else
      {
         /* Check to see if we have switched to a new time period. */
         db_time_period  = p_bon02->tag_id[0];
         cur_time_period = 1;
         tp = atoi( p_bon02->time_period );

         /* Take steps from start date until today.  Each step is
          * one time period.  See how many time periods it takes
          * to get to today.  That results in the current time period.
          */
         while( ((start_date + (tp-1)) + (tp * (cur_time_period-1))) < tran_date )
            cur_time_period++;

         /* Do we need to switch to a new time period? */
         if ( cur_time_period > db_time_period )
         {
            /* ------------------------------------------ */
            /* New Time Period. Reset Period Win Counter. */
            /* ------------------------------------------ */
            period_winners_ctr = 0;
            p_bon02->tag_id[0] = cur_time_period;
         }

         /* Check threshold to see if we have a winner. */
         tran_ctr++;
         if ( tran_ctr >= threshold )
         {
            /*+-----------------------------------------+
              |   WE  HAVE  A  POSSIBLE  WINNER !!!!!!  |
              +-----------------------------------------+*/

            /* Make sure we have not reached max winners for period. */
            if ( program_winners_ctr < program_winners_max )
            {
               if ( period_winners_ctr < period_winners_max )
               {
                  /* Winner count is OK.  Check purchase amounts.
                   * First convert to Peso if necessary.
                   */
                  tran_amt = atof(p_auth_tx->TLF01_details.total_amount)/100.0;
                  if ( 0 == strcmp("840",p_auth_tx->TLF01_details.currency_code))
                  {
                     /* --------------------------------- */
                     /* Need to convert from USD to Peso. */
                     /* --------------------------------- */
                     memset( &crf01, 0x00, sizeof(CRF01) );
                     rt_memaccess = MemAccess( CRF01_TABLE );
                     if ( rt_memaccess < 0)
                     {
                        conv_rate = atof(p_auth_tx->TLF01_details.conversion_rate);
                     }
                     else
                     {
                        strcpy( crf01.primary_key.currency_code, PESO );
                        rt_record = FindMemRecord( (pBYTE)&crf01, currency_code_size,
                                                   sizeof(CRF01), CRF01_TABLE );
                        if ( rt_record < 0 )
                        {
                           conv_rate = atof(p_auth_tx->TLF01_details.conversion_rate);
                        }
                        else
                        {
                           conv_rate = atof( crf01.conversion_rate );
                        }
                     }

                     if ( conv_rate > 0.0 )
                        tran_amt *= conv_rate;
                  }

                  /* Check amount against each BON02 amount range. */
                  for( amt_idx=0; amt_idx<5; amt_idx++ )
                  {
                     low_amt  = atof(p_bon02->amount_range[amt_idx].low_amount )/100.0;
                     high_amt = atof(p_bon02->amount_range[amt_idx].high_amount)/100.0;

                     if ( (low_amt <= tran_amt) && (tran_amt <= high_amt) )
                     {
                        /*+---------------------------------------------+
                          | WINNER - Need to check for previous winner? |
                          +---------------------------------------------+*/
                        retval = CASH_WINNER;
                        if ( p_bon02->multi_winners_allowed[0] != 'Y' )
                        {
                           /* Cardholder is not allowed to win this
                            * program more than once. Check for a
                            * previous win by this cardholder.
                            */
                           retval = db_select_bon03( p_auth_tx->TLF01_details.card_num,
                                                     p_bon02->cash_id );

                           if ( retval == PTEMSG_NOT_FOUND )
                           {
                              /* Cardholder has not won this program yet. */
                              retval = CASH_WINNER;
                           }
                           else
                           {
                              /* Cardholder has won this program previously.
                               * Not allowed to win more than once. Too bad!
                               * Reset the transaction counter so someone
                               * else can win.
                               */
                              tran_ctr--;
                              retval = NO_CASH_WINNER;
                           }
                        }

                        if ( retval == CASH_WINNER )
                        {
                           /*+-------------------------------------+
                             |   WE  HAVE  A  REAL  WINNER !!!!!!  |
                             +-------------------------------------+*/

                           p_auth_tx->TLF01_details.cash_bonus[0] = 'Y';

                           memcpy( p_auth_tx->TLF01_details.response_text,
                                   p_bon02->amount_range[amt_idx].resp_text,
                                   40 );

                           if(p_bon02->amount_range[amt_idx].win_type[0] == 'P')
                           {
                              /* Store Peso amount in reward amount. */
                              sprintf( p_auth_tx->BON01_details.maximum_reward,
                                      "%012.0f", tran_amt*100.0 );
                           }
                           else
                           {
                              strcpy( p_auth_tx->BON01_details.maximum_reward,
                                      p_bon02->amount_range[amt_idx].reward_amount );
                           }

                           /* Update the counters. */
                           tran_ctr = 0;

                           sprintf( p_bon02->period_win_ctr,  "%d", period_winners_ctr +1 );
                           sprintf( p_bon02->program_win_ctr, "%d", program_winners_ctr+1 );

                           updateds_log_message(1,1,p_bon02->amount_range[amt_idx].resp_text,"perform_cash_bonus_processing");
                        }
                     }
                  }
               }
            }
         }

         if ( tran_ctr > 9999999 )
            tran_ctr = 0;
      }

      sprintf( p_bon02->tran_ctr, "%d", tran_ctr );
   }
   return( retval );
}


/******************************************************************************
 *
 *  NAME:         STRING_REVERSE
 *
 *  DESCRIPTION:  This function reverses the order of characters within a
 *                string.  Similar to the non-ANSI function strrev.  Max
 *                length of string allowed here is 200 bytes.
 *
 *  INPUTS:       instr - string to be reversed.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   outstr - string with characters in reverse of instr
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
pCHAR string_reverse( pCHAR instr )
{
   INT   i;
   INT   len;
   CHAR  outstr[200]="";
   pCHAR p_str = NULL;

   len = strlen( instr );
   if ( len <= 200 )
   {
      p_str = instr+len-1;
      for( i=0; i<len; i++ )
      {
         outstr[i] = *p_str;
         p_str--;
      }
      p_str = outstr;
   }
   else
   {
      p_str = instr;
   }
   return( p_str );
}

/******************************************************************************
 *
 *  NAME:         GET_DATE
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the date portion into an output string,
 *                formatted slightly different.
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 *  OUTPUTS:      date_str  : Format = "YYYY0M0D"
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
}

/*****************************************************************************

  Function:   hex_to_int

  Description:
      This function will deocde the hex characters into an integer.
  Author: 
      unknown
  Inputs:
      buf_ptr  - pointer to the ascii hex digits ('0' to '9', 'A' to 'F').
                 this buffer is not assumed, nor expected, to be a null
                 terminated string.
      buf_len  - number of digits in the buffer.
  Outputs:
      None
  Return values:
      INT   -  the integer decode result.
******************************************************************************/
INT hex_to_int (pCHAR buf_ptr, INT buf_len  )
{
   INT     index;
   INT     n = 0;

   for (index = 0; index < buf_len; index++, buf_ptr++) 
   {
      if (*buf_ptr >= '0'  &&  *buf_ptr <= '9')
         n = (n * 16) + ((INT) (*buf_ptr - '0'));

      else if (*buf_ptr >= 'A'  &&  *buf_ptr <= 'F')
         n = (n * 16) + ((INT) (*buf_ptr - 'A') + 10);

      else if (*buf_ptr >= 'a'  &&  *buf_ptr <= 'f')
         n = (n * 16) + ((INT) (*buf_ptr - 'A') + 10);

      else
         return (-1);  /* error*/
   } /* end of for*/

  return( n );
} /* genutil_hex_to_int */


/******************************************************************************
 *
 *  NAME:         CREATE_BON03_RECORD
 *
 *  DESCRIPTION:  This function uses transaction data and Cash Bonus
 *                program data to populate a BON03 record.
 *
 *  INPUTS:       p_auth_tx - Transaction information
 *                p_bon02   - Cash Bonus program information
 *
 *  OUTPUTS:      p_bon03   - Record to be inserted into BON03
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_bon03_record( pAUTH_TX p_auth_tx, pBON02 p_bon02, pBON03 p_bon03 )
{
   strcpy( p_bon03->transaction_id,     p_auth_tx->TLF01_details.primary_key.transaction_id );
   strcpy( p_bon03->cash_id,            p_bon02->cash_id );
   strcpy( p_bon03->tag_id,             p_bon02->tag_id  );
   strcpy( p_bon03->program_start_date, p_bon02->program_start_date );
   strcpy( p_bon03->program_end_date,   p_bon02->program_end_date   );
   strcpy( p_bon03->card_nbr,           p_auth_tx->TLF01_details.card_num    );
   strcpy( p_bon03->merchant_id,        p_auth_tx->TLF01_details.merchant_id );
   strcpy( p_bon03->terminal_id,        p_auth_tx->TLF01_details.terminal_id );
   strcpy( p_bon03->auth_number,        p_auth_tx->TLF01_details.auth_number );
   strcpy( p_bon03->tran_date,          p_auth_tx->TLF01_details.date_yyyymmdd );
   strcpy( p_bon03->tran_time,          p_auth_tx->TLF01_details.time_hhmmss   );
   strcpy( p_bon03->tran_amount,        p_auth_tx->TLF01_details.total_amount  );
   strcpy( p_bon03->resp_text,          p_auth_tx->TLF01_details.response_text );
   strcpy( p_bon03->invoice_number,     p_auth_tx->TLF01_details.invoice_number);
   strcpy( p_bon03->retrieval_ref_num,  p_auth_tx->TLF01_details.retrieval_ref_num);
   strcpy( p_bon03->card_holder_name,   p_auth_tx->TLF01_details.card_holder_name );
   strcpy( p_bon03->reward_amount,      p_auth_tx->BON01_details.maximum_reward   );
   return;
}


/******************************************************************************
 *
 *  NAME:         get_cash_bonus_ini_info
 *
 *  DESCRIPTION:  This function reads information from the TF.ini file.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      CashBonusNonWinnerMsg - (Global)
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_cash_bonus_ini_info()
{
   INT   ret_code;
   CHAR  sectionname  [MAX_APP_NAME_SIZE] = "";
   CHAR  filename     [MAX_APP_NAME_SIZE] = "";
   CHAR  tmpstr       [MAX_APP_NAME_SIZE] = "";
   CHAR  message      [MAX_APP_NAME_SIZE] = "";
   CHAR  default_msg[] = "Thank You for using your Equitable Card";


   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_APP_NAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);

   /* Get the message to send to the terminal for non-winners. */
   strcpy( sectionname, "CASH_BONUS" );
   ret_code = GetPrivateProfileString(
                   sectionname,               /* points to section name       */
                  "NON_WINNER_MSG",           /* points to key name           */
                   default_msg,               /* points to default string     */
                   message,                   /* points to destination buffer */
                   sizeof(message) - 1,       /* size of destination buffer   */
                   filename                   /* points to ini filename       */
            );

   if ( message[0] == 0x00 )
      strcpy( CashBonusNonWinnerMsg, default_msg );
   else
      strcpy( CashBonusNonWinnerMsg, message );
   return;
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf )
{
   #define sON  "1"
   #define sOFF "0"
   #define sDEFAULT_DS_ERROR_STATICS_VALUE  "0.5"

   DWORD rc;
   CHAR  filename   [80];
   CHAR  tmpstr     [80];
   CHAR  timer_flag [5] = {0};
   CHAR  timer_value[5] = {0};
   INT   ret_val = true;
   CHAR db_nullchk_flag[5] = {0};
   CHAR  updatds_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

      memcpy( timer_value,
              sDEFAULT_DS_ERROR_STATICS_VALUE,
              sizeof(sDEFAULT_DS_ERROR_STATICS_VALUE) );

      rc = GetPrivateProfileString(
               "DATASERVERS",             /* points to section name         */
               "DB_STATICS_VALUE",       /* points to key name             */
                sDEFAULT_DS_ERROR_STATICS_VALUE,  /* Default string                 */
                timer_value,              /* points to destination buffer   */
                sizeof(timer_value)-1,    /* size of destination buffer     */
                filename                  /* points to ini filename         */
      );

      *ds_timer_value = atof(timer_value);
      if ( (*ds_timer_value < 0) || (*ds_timer_value > 999) )
      {
         sprintf( errbuf,
                 "%s:Invalid value DB error statics Value (%s) in tf.ini. Use 0 to 999",
                  appname, timer_value );
         *ds_timer_flag = DS_TIMER_OFF;
         ret_val = false;
      }


   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
                  "DATASERVERS",             /* points to section name         */
                  "DB_STATICS_PATH",       	/* points to key name             */
                   "",  					/* Default string                 */
				   DB_file_path,              	/* points to destination buffer   */
                   sizeof(DB_file_path)-1,   	 /* size of destination buffer     */
                   filename                  /* points to ini filename         */
         );

   rc = GetPrivateProfileString(
                      "ERROR_WARNING",             /* points to section name         */
					  appname,       	/* points to key name             */
                      "",  					/* Default string                 */
					  DB_module_file_name,              	/* points to destination buffer   */
                      sizeof(DB_module_file_name)-1,   	 /* size of destination buffer     */
                      filename                  /* points to ini filename         */
            );

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  updatds_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(updatds_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(updatds_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(DB_file_path) > 0) &&
	  (strlen(DB_module_file_name)> 0))
   {
	   Create_Db_Statics_And_Db_Oracle_Error_Filename();
   }

   return( ret_val );
}


void Create_Db_Statics_And_Db_Oracle_Error_Filename( )
{
	CHAR  system_date[25] = "";

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( DB_Logging_Filename, DB_file_path );
    strcat( DB_Logging_Filename, DB_module_file_name );
	strcat( DB_Logging_Filename, ".EW." );
    strncat(DB_Logging_Filename, system_date,   4 );  /* YYYY */
    strncat(DB_Logging_Filename, system_date+5, 2 );  /* MM   */
    strncat(DB_Logging_Filename, system_date+8, 2 );  /* DD   */

    return;
}

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf)
{
	INT   len=0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR path[256]={0};
	CHAR Buffer[1024]={0};
	int path_len=0;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	strcpy(Buffer,time_date);
   /* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(DB_Logging_Filename);
    path_len =strlen(DB_file_path);
    if(len==0 || path_len==0)
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return;
    }
    if ( 0 != strcmp(&DB_Logging_Filename[len-4], current_mmdd) )
    {

		/* Now create the new filename.
          * ----------------------------
          */
			Create_Db_Statics_And_Db_Oracle_Error_Filename();

	}
	if((fp = fopen(DB_Logging_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return;
	}

	strcat(Buffer,":");
	strcat(Buffer,Error_Buf);
	strcat(Buffer,"\n");
	len=0;
	len=strlen(Buffer);


	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return ;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return ;
}
/******************************************************************************
 *
 *  NAME:         updateds_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void updateds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func)
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
   /*if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;*/

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   memcpy( text_message, msg, sizeof(text_message)-1 );
   updateds_Log_error_warning_to_File(text_message,sev,func);
   /* Call function to post the message. */
   if(monitor_flag == 1)
   {

		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
		   										 eventlog_flag, msg_type, func,
		   										 severity, log_type,
		   										 NULL, NULL,NULL );

   }
   return;
}

INT updateds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR  card_tid_mid_details [256] = {0};
	INT   cardlen = 0;
	INT   nIndex = 0;
	CHAR  tempcard[21] = {0};

	FILE *fp;

	#define NUM_SIZE_WRITES  1

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(DB_Logging_Filename);
    path_len = strlen(DB_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&DB_Logging_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	Create_Db_Statics_And_Db_Oracle_Error_Filename();
    }
	if((fp = fopen(DB_Logging_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	if (strlen(auth_tx.TLF01_details.card_num)> 0)
			{
				/* Masking card number first 5 and last 4 in clear form remaining all masked. */
				cardlen = strlen( auth_tx.TLF01_details.card_num );
				strncpy( tempcard, auth_tx.TLF01_details.card_num, cardlen);
				for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
				{
					tempcard[nIndex] = '*';
				}
				sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
						tempcard,
						auth_tx.TLF01_details.terminal_id,
						auth_tx.TLF01_details.merchant_id,
						auth_tx.TLF01_details.sys_trace_audit_num,
						auth_tx.TLF01_details.retrieval_ref_num);
			}
			else
			{
				sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
						auth_tx.TLF01_details.terminal_id,
						auth_tx.TLF01_details.merchant_id,
						auth_tx.TLF01_details.sys_trace_audit_num,
						auth_tx.TLF01_details.retrieval_ref_num);
			}
			strcat(Buffer,card_tid_mid_details);
			strcat(Buffer," ");
	strcat(Buffer, func);
	strcat(Buffer,"\n");

	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}
	fclose(fp);
	return  0;
}

BOOLEAN Updatds_Is_MCRefund_Transaction(pTLF01 pTLF01_details)
{
	if((0==strncmp(pTLF01_details->handler_queue, "ncmcrd",6)  ||
		0==strncmp(pTLF01_details->handler_queue, "ncmcr3",6)) &&
	   (0==strncmp(pTLF01_details->message_type, "0100",4)) &&
	   (0==strncmp(pTLF01_details->processing_code, "20",2)))
	{/* will include all ncmcrd and ncmcr3*/
		return true;
	}
	return false;
}

INT pin_block_exists(pAUTH_TX p_auth_tx)
{
   INT  ret_val = true;

   if ( 0 == strlen(p_auth_tx->TLF01_details.pin_block) )
      ret_val = false;

   else if (blank_string(p_auth_tx->TLF01_details.pin_block,
                         sizeof(p_auth_tx->TLF01_details.pin_block)-1) )
      ret_val = false;

   return( ret_val );
}

INT Check_msg_coming_from_IST(pAUTH_TX p_auth_tx)
{
	if(((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit",6) ||
	     0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit2",6)) ||
	   ( 0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"nceqit",6) ||
	     0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"neqit2",6))) &&
	   ( true == pin_block_exists(p_auth_tx) &&
		 0 == strncmp(p_auth_tx->TLF01_details.security_response_code,"00",2)))
	{
		return true;
	}
	else if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit",6)  ||
		     0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit2",6)) &&
			 0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN,6))
	{
		return true;
	}
	return false;
}

INT blank_string(pCHAR astr, INT max_len)
{
   INT  i;
   INT  ret_val = true;

   if (0 == strlen(astr))
      ret_val = true;

   else
   {
      for(i = 0;i < max_len; i++)
      {
         if (astr[i] != ' ')
         {
            ret_val = false;
            break;
         }
      }
   }
   return( ret_val );
}

BOOLEAN updatds_check_for_amex_incremental_transaction(pAUTH_TX p_auth_tx)
{
	if( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5) &&
		(0 == strncmp(p_auth_tx->TLF01_details.message_type,"1100",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"1110",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4)) &&
		 0 == strncmp(p_auth_tx->function_code,"106",3) &&
		(0 == strncmp(p_auth_tx->host_processing_code,"004000",6) ||
	     0 == strncmp(p_auth_tx->host_processing_code,"004800",6) ) &&
		 0 != strncmp(p_auth_tx->TLF01_details.total_amount,"000000000000",12) )
	{
		return true;
	}

	return false;
}

BOOLEAN updatds_check_for_amex_partial_reversal_transaction(pAUTH_TX p_auth_tx)
{
	if( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5) &&
		(0 == strncmp(p_auth_tx->TLF01_details.message_type,"1420",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"1430",4)) &&
		 0 == strncmp(p_auth_tx->function_code,"401",3) &&
		0 == strncmp(p_auth_tx->host_processing_code,"024000",6) &&
		 0 != strncmp(p_auth_tx->TLF01_details.actual_amount,"000000000000",12) )
	{/* actual_amount holds the bit 4 for partial reversal*/
		return true;
	}

	return false;
}

/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/
void Mask_card_number(char * card_num)
{
	int cardlen = 0;
	int nIndex 	= 0;
	memset(masked_card_num ,0x00, sizeof(masked_card_num));
	cardlen = strlen( card_num );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, card_num, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}
}

INT updatds_get_mcc_of_preauth_in_days_for_smcc(void)
{
   CHAR  rc_value[5]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MID_MCC",             /* points to section name         */
									  "MID_MCC_01",       	/* points to key name             */
									  "",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);
   strncpy(smcc_id1,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_02",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id2,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_03",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id3,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_04",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id4,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_05",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id5,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_06",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id6,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_07",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id7,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_08",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id8,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_09",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id9,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_10",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id10,rc_value,MID_MCC_LEN);

}

INT updatds_check_if_mid_is_smcc (pAUTH_TX auth_tx)
{
	if(	(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id1, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id2, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id3, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id4, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id5, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id6, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id7, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id8, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id9, MID_MCC_LEN))||
		(0 == strncmp(auth_tx->TLF01_details.category_code,smcc_id10,MID_MCC_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

