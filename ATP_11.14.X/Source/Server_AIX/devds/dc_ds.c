/******************************************************************************

   Module: devds.c

   Title: Transaction Server Dataserver Routines

   Description: 

******************************************************************************/

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
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "dc_database.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "genutil.h"
#include "timings.h"
#include "app_info.h"

#define AMEX03B_APRIL2024 1
#define BDOR_62_PREAUTH_VOID 1
#define OCT_MAN_2024_BLD_2 1

char last_date[9]={0};
char acq_mc_variable_auth_flag;
char acq_visa_variable_auth_flag;
char acq_jcb_variable_auth_flag;
char acq_dci_variable_auth_flag;

/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
CHAR   oprmon_que_name[MAX_APP_NAME_SIZE];


#if BDOR_62_PREAUTH_VOID

#define ISO_RESPONSE_CODE_LEN 2
#define MAX_PREAUTH_VALIDITY_DAYS 30

CHAR maximum_validity_of_preauth_in_days_for_MC[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_VISA[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_AMEX[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_DCI[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_JCB[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_UPI[3]={0};

#endif

#if OCT_MAN_2024_BLD_2

#define SMCC_MAX_PREAUTH_VALIDITY_DAYS				05
#define SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN 			2
#define MID_MCC_LEN									4

CHAR smcc_maximum_validity_of_preauth_in_days_for_MC[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_VISA[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_AMEX[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_DCI[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_JCB[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_UPI[3]={0};

#endif

#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"
INT DB_NullCheck_Flag;

/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
INT DB_NullCheck_Flag;

/* End   */
/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;

UINT Max_File_Size_Defined = 0 ;


/*Merchant offset in croosponding tables*/
#define DCF01_MERCHANT_ID_OFFSET      96
#define BCH01_MERCHANT_ID_OFFSET      16
#define BCH01_MP_MERCHANT_ID_OFFSET   16
#define BCH10_MERCHANT_ID_OFFSET      29
#define BCH11_MERCHANT_ID_OFFSET	  29
#define BCH20_MERCHANT_ID_OFFSET      29


#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

/**********Primary Key Size*********************/
#define BCH01_PRIMARY_KEY_SIZE 32 // Device id + Batch_number + Merchant ID.
#define BCH01_MP_PRIMARY_KEY_SIZE 32 // Device id + Batch_number + Merchant ID.

/* Reading data from tf.ini*/
#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20
/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

/* Function protype */
BYTE process_alldata_msg( pPTE_MSG  );
BYTE process_alldata_mp_msg( pPTE_MSG p_msg_in );
pPTE_MSG process_check_data_in_tables( pPTE_MSG );
void process_db_cleanup( pPTE_MSG ); 
void TrapFunction( INT signum );

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename( );

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);

void devds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);

INT devds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);
#if AMEX03B_APRIL2024
INT get_variable_auth_config_details();
INT get_last_dated_date_only();
void create_system_date( pBYTE system_date );
INT devds_check_If_AMEX_Transaction (pCHAR card_num);
#endif
INT devds_check_If_MC_Transaction (pCHAR card_num);


PRIVATE union
{
  
  DCF01   dcf01;
  BCH01   bch01;
  BCH10   bch10;
  MCF01   mcf01;
  OCF01   ocf01;
  OCF20   ocf20;
  MCF20   mcf20;
  TLF01   tlf01;
  NMF01   nmf01;
  NBF01   nbf01;
  NGF01   ngf01;
  NFI01   nfi01;
  NFI02   nfi02;
  NTF01   ntf01;
  BCH11   bch11;
  BCH20   bch20;
  BCH01_MP bch01_mp;
  MGF01   mgf01;
  RPS01		rps01;
 } db_recs;

PRIVATE union
{
   NGF01_LIST          Ngf01_List;
   NFI01_LIST          Nfi01_List;
   NBF01_LIST          Nbf01_List;
   NMF01_LIST          Nmf01_List;
   BCH10_LIST          Bch10_List;
   BCH11_LIST          Bch11_List;
   BCH01_LIST          Bch01_List;
   BCH01_MP_LIST       Bch01_Mp_list;
   TLF01_LIST          Tlf01_List;
   BCH10_GUI_LIST      Bch10_Gui_List;
   BCH11_GUI_LIST      Bch11_Gui_List;
   BCH01_GUI_LIST      Bch01_Gui_List;
   BCH01_MP_GUI_LIST   Bch01_Mp_Gui_List;
   MCF01_GUI_LIST      Mcf01_Gui_List;
   DCF01_GUI_LIST      Dcf01_Gui_List;
   NBF01_GUI_LIST      Nbf01_Gui_List;
   NGF01_GUI_LIST      Ngf01_Gui_List;
   TLF01_GUI_LIST      Tlf01_Gui_List;
   MGF01_GUI_LIST      Mgf01_Gui_List;
   MGF01_GUI_DROPLIST Mgf01_Gui_Droplist;

}db_lists;


PRIVATE LONG         Bin_Count;
PRIVATE LONG         Caf_Count;
PRIVATE LONG         Faf_Count;
PRIVATE LONG         Crf_Count;
PRIVATE LONG         Csf_Count;
PRIVATE BOOLEAN      Bin01InMem = false;
PRIVATE BOOLEAN      Caf01InMem = false;
PRIVATE BOOLEAN      Faf01InMem = false;
PRIVATE BOOLEAN      Crf01InMem = false;
PRIVATE BOOLEAN      Csf01InMem = false;
PRIVATE BOOLEAN      OprMonInMem= false;  
PRIVATE BOOLEAN      TranDetailInMem = false;
PRIVATE LONG         MemSize;
PRIVATE LONG         MemAvailable;

PRIVATE BYTE         MsgSubtype2;
PRIVATE BYTE         MsgSubtype1;
PRIVATE BYTE         result_code;
PRIVATE BOOLEAN      updates_to_db_made = false;
PRIVATE CHAR         ErrorMsg  [100];
        CHAR         AppName   [MAX_APP_NAME_SIZE];
PRIVATE CHAR         last_trid [MAX_TRAN_ID_SIZE];
PRIVATE pCHAR        reply_que;
PRIVATE CHAR         orig_comm_inf[MAX_ORIG_INFO_SIZE];
PRIVATE CHECKPOINT   checkpoint_rec;
PRIVATE CHAR         AtpBuffer[30000];
PRIVATE BYTE         app_data_type;
PRIVATE CHAR         applnk_que_name[8]="applnkA";

BOOLEAN   in_refeed_mode    = false;
CHAR      Version[] = "ATP_11.14.0";

CHAR  CUP_merchant_indicator[8]={0};

INT ds_timer_flag;
INT ds_timer_value;

AUTH_TX auth_tx;

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
double        NextReportTime;
CHAR          ReportTime[5];


#ifdef _DEBUG
   #define PRINT( X ) printf( X )
#else
   #define PRINT( X )
#endif


/*
 * These defines control whether the dataserver is using logger-reply
 * and whether it should be doing checkpointing.
 *
 * Possible combinations:
 *    a) using logger-reply with checkpoints
 *    b) using logger-reply without checkpoints
 *    c) using neither.
 *
 * note: cannot use checkpoints without logger-reply!!!!!
 *
 */ 
// 12-07-98 gabriel
//#define USING_LOGGER_REPLY
//#define USING_CHECKPOINTS

#ifndef USING_LOGGER_REPLY
   #undef USING_CHECKPOINTS
#endif

/*************************************************************************************/
/*************************************************************************************/
LONG CreateTable(BYTE AppData)
{
   
   char table_count[5];
   char SectionName[13] = "";
   INT  num_rec, rec_size;
   
   memset(ErrorMsg, 0, sizeof(ErrorMsg));
   memset(table_count, 0, sizeof(table_count));
   if(AppData == OPRMON_DATA)
   {
      return(CreateXipcTable(1, MAX_ORIG_INFO_SIZE, "MonitorTable"));
   }
   else
      return(-1L);
   return(CreateXipcTable(num_rec, rec_size, SectionName));
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
/*************************************************************************************/
/*************************************************************************************/
LONG  PopulateTable(BYTE  AppData)
{
   char  SectionName[13] = "";
   LONG  TableSize;
   
   BYTE  OpMonFlag = 0;
   pBYTE TableList;

   if(AppData == OPRMON_DATA)
   {
       return(WriteTable("MonitorTable", MAX_ORIG_INFO_SIZE + 1, &OpMonFlag));
   }
   else
      return(-1L);
   return(WriteTable(SectionName, TableSize, TableList));
}

/*************************************************************************************/
/*************************************************************************************/
BYTE  VerifyMem(LONG ReservedMem, LONG NeededMem)
{
   LONG Result;

   Result = ReservedMem - NeededMem;
   if (Result < 0)
      return(MEM_ERR);
   if( (Result * 100) < (ReservedMem * MEM_PERCENT))
      return(MEM_ENOUGH);
   return(MEM_OK);
}

/******************************************************************************
 *
 *  NAME:         devds_read_CUP_merchant_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT devds_read_CUP_merchant_indicator( void )
{
   DWORD rc = 0;
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


INT devds_Check_If_CUP_Transaction (pAUTH_TX auth_tx)
{

	if ((0 == strncmp(auth_tx->TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx->TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT devds_check_If_Diners_Transaction (pAUTH_TX auth_tx)
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



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_move_trans(pPTE_MSG p_msg_in)
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data  + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case BCH10_DATA:

         PRINT( "request to move bch10 records to bch11\n" );
         result_code = db_move_bch10_to_bch11((pCHAR)p_data, ErrorMsg);
         break;
      case BCH11_DATA:

         PRINT( "request to move bch11 records to bch10\n" );
         result_code = db_move_bch11_to_bch10((pCHAR)p_data, ErrorMsg);
         break;

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Move request from %s", reply_que);
         break;
   }
   if( result_code == PTEMSG_OK )
   {
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX),
                                    app_data_type);
   

      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build settle totals response to %s", reply_que);
      }
   }

   return (p_msg_out);
}


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_insert_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pAUTH_TX       auth_tx;
   
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   auth_tx = (pAUTH_TX)p_data;
   p_data = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case BCH01_BCH10_TLF01_DATA:
         PRINT( "request to insert tlf01, bch01, bch10 record\n" );
         result_code = db_update_bch01_insert_bch10_insert_tlf01( ( pAUTH_TX)p_data, ErrorMsg );
       
		  break;

      case TLF01_DATA:

			 PRINT( "request to insert tlf01 record\n" );

			 memset( &db_recs.tlf01, 0, sizeof( db_recs.tlf01 ) );
			 memcpy( &db_recs.tlf01, ( pTLF01)p_data, sizeof( TLF01 ) );
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinTLF01 (&db_recs.tlf01,MODE_INSERT);
      		}
			 result_code = db_insert_tlf01( &db_recs.tlf01, ErrorMsg );
/*			 memset( &db_recs.tlf01_stip, 0, sizeof( db_recs.tlf01_stip ) );
			 memcpy( &db_recs.tlf01_stip, ( pTLF01_STIP)p_data, sizeof( TLF01_STIP ) );*/

			 if((strncmp(db_recs.tlf01.response_code,"00",2) == 0) && 
			 (strncmp(db_recs.tlf01.product_codes[1].code,STANDIN,6)== 0)) /* Girija Y */
		 {
			 PRINT( "request to insert tlf01_stip record\n" );

			 /* retrieve the data into a ocf20 structure and insert into database */
			 
			 result_code = db_insert_tlf01_stip( &db_recs.tlf01, ErrorMsg );

		 }
		
         break;
      
      

     case NFI01_DATA:

         PRINT( "request to insert nfi01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.nfi01, 0, sizeof( db_recs.nfi01 ) );
         memcpy( &db_recs.nfi01, ( pNFI01)p_data, sizeof( NFI01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNFI01 (&db_recs.nfi01,MODE_INSERT);
  		}
         result_code = db_insert_nfi01( &db_recs.nfi01, ErrorMsg );
         break;

      
      case NBF01_DATA:

         PRINT( "request to insert nbf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.nbf01, 0, sizeof( db_recs.nbf01 ) );
         memcpy( &db_recs.nbf01, ( pNBF01)p_data, sizeof( NBF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNBF01 (&db_recs.nbf01,MODE_INSERT);
  		}
         result_code = db_insert_nbf01( &db_recs.nbf01, ErrorMsg );
         break;

      case NGF01_DATA:

         PRINT( "request to insert ngf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ngf01, 0, sizeof( db_recs.ngf01 ) );
         memcpy( &db_recs.ngf01, ( pNGF01)p_data, sizeof( NGF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNGF01 (&db_recs.ngf01,MODE_INSERT);
  		}
         result_code = db_insert_ngf01( &db_recs.ngf01, ErrorMsg );
         break;

      case NTF01_DATA:

         PRINT( "request to insert ntf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ntf01, 0, sizeof( db_recs.ntf01 ) );
         memcpy( &db_recs.ntf01, ( pNTF01)p_data, sizeof( NTF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNTF01 (&db_recs.ntf01,MODE_INSERT);
  		}
         result_code = db_insert_ntf01( &db_recs.ntf01, ErrorMsg );
         break;

      case BCH01_DATA:

         PRINT( "request to insert bch01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.bch01, 0, sizeof( db_recs.bch01 ) );
         memcpy( &db_recs.bch01, ( pBCH01)p_data, sizeof( BCH01 ) );
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_INSERT_BCH01_UPDATE_DCF01)
         {
            result_code = db_insert_bch01_update_dcf01( &db_recs.bch01, ErrorMsg );
         }
         else
         {
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_INSERT);
      		}
            result_code = db_insert_bch01( &db_recs.bch01, ErrorMsg );
         }
            break;

      case NFI02_DATA:

         PRINT( "request to insert nfi02 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.nfi02, 0, sizeof( db_recs.nfi02 ) );
         memcpy( &db_recs.nfi02, ( pNFI02)p_data, sizeof( NFI02 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNFI02 (&db_recs.nfi02,MODE_INSERT);
  		}
         result_code = db_insert_nfi02( &db_recs.nfi02, ErrorMsg );
         break;

      
      case MCF20_DATA:

         PRINT( "request to insert mcf20 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.mcf20, 0, sizeof( db_recs.mcf20 ) );
         memcpy( &db_recs.mcf20, ( pMCF20 )p_data, sizeof( MCF20 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinMCF20 (&db_recs.mcf20,MODE_INSERT);
  		}
         result_code = db_insert_mcf20( &db_recs.mcf20, ErrorMsg );

         break;

      case OCF20_DATA:

         PRINT( "request to insert ocf20 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ocf20, 0, sizeof( db_recs.ocf20 ) );
         memcpy( &db_recs.ocf20, ( pOCF20 )p_data, sizeof( OCF20 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinOCF20 (&db_recs.ocf20,MODE_INSERT);
  		}
         result_code = db_insert_ocf20( &db_recs.ocf20, ErrorMsg );
         break;

      
      case OCF01_DATA:

         PRINT( "request to insert ocf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.ocf01, 0, sizeof( db_recs.ocf01 ) );
         memcpy( &db_recs.ocf01, ( pOCF01 )p_data, sizeof( OCF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinOCF01 (&db_recs.ocf01,MODE_INSERT);
  		}
         result_code = db_insert_ocf01( &db_recs.ocf01, ErrorMsg );
         break;

      
      case MCF01_DATA:

         PRINT( "request to insert mcf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.mcf01, 0, sizeof( db_recs.mcf01 ) );
         memcpy( &db_recs.mcf01, ( pMCF01 )p_data, sizeof( MCF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinMCF01 (&db_recs.mcf01,MODE_INSERT);
  		}
         result_code = db_insert_mcf01( &db_recs.mcf01, ErrorMsg );
         break;

      case MGF01_DATA:

	  PRINT( "request to insert mgf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.mgf01, 0, sizeof( db_recs.mgf01 ) );
         memcpy( &db_recs.mgf01, ( pMGF01 )p_data, sizeof( MGF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinMGF01 (&db_recs.mgf01,MODE_INSERT);
  		}
         result_code = db_insert_mgf01( &db_recs.mgf01, ErrorMsg );
         break;

		 
      case BCH10_DATA:

         PRINT( "request to insert bch10 record\n" );

         /* retrieve the data into a ucf01 structure and insert into database */
         memset( &db_recs.bch10, 0, sizeof( db_recs.bch10 ) );
         memcpy( &db_recs.bch10, ( pBCH10 )p_data, sizeof( BCH10 ) );
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_INSERT_BCH10_SETTLE )
         {
		 /* TF Phani- For Amex Cards we need to get DE31 value from TLF01->visa_tran_id*/
		 /*	 if card number start with 3, then call below function to get TID
		 */
        	if(true == devds_check_If_Diners_Transaction(auth_tx))
        	{
        		result_code = db_select_DCI_for_batch_upload( &db_recs.bch10, ErrorMsg );
        	}
        	else if(db_recs.bch10.card_nbr[0] == '3')
        	{
        		result_code = db_select_DE31_for_amex( &db_recs.bch10, ErrorMsg );
        	}
			else if (true == devds_Check_If_CUP_Transaction(auth_tx))
			{
				if( db_recs.bch10.card_nbr[0] == '6')
				{
					result_code = db_select_CUP_EPOS_Field_V9( &db_recs.bch10, ErrorMsg );
				}
				else
				{
					result_code = db_select_CUP_Co_Branded_EPOS_Field_V8( &db_recs.bch10, ErrorMsg );
				}
			}
          /*TF Phani-  if Card number belongs to Master card..retrive few deatils from TLF01
			Which requires in EPOS file*/
			else
			 result_code = db_select_DE63_for_MC( &db_recs.bch10, ErrorMsg );
			

	 	/*For OFFLINE SALE transactions update EMV RAW DATA Transaction ID*/
		
			 if (strncmp(&db_recs.bch10.message_type,"0220",4)== 0)
		     {
		       				        
		         db_update_emv_raw_by_bch20_auth_number(&db_recs.bch10,&db_recs.bch10.transaction_id,ErrorMsg);
			 }

			updates_to_db_made = true;
			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
			}
            result_code = db_insert_bch10_settle( &db_recs.bch10, ErrorMsg );
         }
         else
         {
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
      		}
        	 result_code = db_insert_bch10( &db_recs.bch10, ErrorMsg );
         }
         break;

      
       case DCF01_DATA:

         PRINT( "request to insert ucf01 record\n" );

         /* retrieve the data into a dcf01 structure and insert into database */
         memset( &db_recs.dcf01, 0, sizeof( db_recs.dcf01 ) );
         memcpy( &db_recs.dcf01, ( pDCF01 )p_data, sizeof( DCF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinDCF01 (&db_recs.dcf01,MODE_INSERT);
  		}
         result_code = db_insert_dcf01( &db_recs.dcf01, ErrorMsg );
         break;

      
     default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Insert request from %s", reply_que);
         break;
   }


   if (result_code == PTEMSG_OK)
   {
      
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Insert response to %s", reply_que);
      }
   }

   return (p_msg_out);
}
/*************************************************************************************/



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_insert_mp_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case BCH01_BCH10_TLF01_DATA:
         PRINT( "request to insert tlf01, bch01_mp, bch10 record\n" );
         result_code = db_update_bch01_mp_insert_bch10_insert_tlf01( ( pAUTH_TX)p_data, ErrorMsg );
       
		  break;

      case TLF01_DATA:

			 PRINT( "request to insert tlf01 record\n" );

			 memset( &db_recs.tlf01, 0, sizeof( db_recs.tlf01 ) );
			 memcpy( &db_recs.tlf01, ( pTLF01)p_data, sizeof( TLF01 ) );
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinTLF01 (&db_recs.tlf01,MODE_INSERT);
      		}
			 result_code = db_insert_tlf01( &db_recs.tlf01, ErrorMsg );
			
/*			 memset( &db_recs.tlf01_stip, 0, sizeof( db_recs.tlf01_stip ) );
			 memcpy( &db_recs.tlf01_stip, ( pTLF01_STIP)p_data, sizeof( TLF01_STIP ) );*/

			 if((strncmp(db_recs.tlf01.response_code,"00",2) == 0) && 
			 (strncmp(db_recs.tlf01.product_codes[1].code,STANDIN,6)== 0)) /* Girija Y */
		 {
			 PRINT( "request to insert tlf01_stip record\n" );

			 /* retrieve the data into a ocf20 structure and insert into database */
			 
			 result_code = db_insert_tlf01_stip( &db_recs.tlf01, ErrorMsg );
		 }
		
         break;
      
      

     case NFI01_DATA:

         PRINT( "request to insert nfi01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.nfi01, 0, sizeof( db_recs.nfi01 ) );
         memcpy( &db_recs.nfi01, ( pNFI01)p_data, sizeof( NFI01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNFI01 (&db_recs.nfi01,MODE_INSERT);
  		}
         result_code = db_insert_nfi01( &db_recs.nfi01, ErrorMsg );
         break;

      
      case NBF01_DATA:

         PRINT( "request to insert nbf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.nbf01, 0, sizeof( db_recs.nbf01 ) );
         memcpy( &db_recs.nbf01, ( pNBF01)p_data, sizeof( NBF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNBF01 (&db_recs.nbf01,MODE_INSERT);
  		}
         result_code = db_insert_nbf01( &db_recs.nbf01, ErrorMsg );
         break;

      case NGF01_DATA:

         PRINT( "request to insert ngf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ngf01, 0, sizeof( db_recs.ngf01 ) );
         memcpy( &db_recs.ngf01, ( pNGF01)p_data, sizeof( NGF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNGF01 (&db_recs.ngf01,MODE_INSERT);
  		}
         result_code = db_insert_ngf01( &db_recs.ngf01, ErrorMsg );
         break;

      case NTF01_DATA:

         PRINT( "request to insert ntf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ntf01, 0, sizeof( db_recs.ntf01 ) );
         memcpy( &db_recs.ntf01, ( pNTF01)p_data, sizeof( NTF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNTF01 (&db_recs.ntf01,MODE_INSERT);
  		}
         result_code = db_insert_ntf01( &db_recs.ntf01, ErrorMsg );
         break;
	
      case BCH01_MP_DATA:

         PRINT( "request to insert bch01_mp record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.bch01_mp, 0, sizeof( db_recs.bch01_mp ) );
         memcpy( &db_recs.bch01_mp, ( pBCH01_MP)p_data, sizeof( BCH01_MP ) );
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_INSERT_BCH01_UPDATE_DCF01)
            result_code = db_insert_bch01_mp_update_dcf01( &db_recs.bch01_mp, ErrorMsg );
         else
         {
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_INSERT);
      		}
        	 result_code = db_insert_bch01_mp( &db_recs.bch01_mp, ErrorMsg );
         }
            break;

      case NFI02_DATA:

         PRINT( "request to insert nfi02 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.nfi02, 0, sizeof( db_recs.nfi02 ) );
         memcpy( &db_recs.nfi02, ( pNFI02)p_data, sizeof( NFI02 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinNFI02 (&db_recs.nfi02,MODE_INSERT);
  		}
         result_code = db_insert_nfi02( &db_recs.nfi02, ErrorMsg );
         break;

      
      case MCF20_DATA:

         PRINT( "request to insert mcf20 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.mcf20, 0, sizeof( db_recs.mcf20 ) );
         memcpy( &db_recs.mcf20, ( pMCF20 )p_data, sizeof( MCF20 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinMCF20 (&db_recs.mcf20,MODE_INSERT);
  		}
         result_code = db_insert_mcf20( &db_recs.mcf20, ErrorMsg );
         break;

      case OCF20_DATA:

         PRINT( "request to insert ocf20 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ocf20, 0, sizeof( db_recs.ocf20 ) );
         memcpy( &db_recs.ocf20, ( pOCF20 )p_data, sizeof( OCF20 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinOCF20 (&db_recs.ocf20,MODE_INSERT);
  		}
         result_code = db_insert_ocf20( &db_recs.ocf20, ErrorMsg );
         break;

      
      case OCF01_DATA:

         PRINT( "request to insert ocf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.ocf01, 0, sizeof( db_recs.ocf01 ) );
         memcpy( &db_recs.ocf01, ( pOCF01 )p_data, sizeof( OCF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinOCF01 (&db_recs.ocf01,MODE_INSERT);
  		}
         result_code = db_insert_ocf01( &db_recs.ocf01, ErrorMsg );
         break;

      
      case MCF01_DATA:

         PRINT( "request to insert mcf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.mcf01, 0, sizeof( db_recs.mcf01 ) );
         memcpy( &db_recs.mcf01, ( pMCF01 )p_data, sizeof( MCF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinMCF01 (&db_recs.mcf01,MODE_INSERT);
  		}
         result_code = db_insert_mcf01( &db_recs.mcf01, ErrorMsg );
         break;
		 
	 case MGF01_DATA:
	 
		  PRINT( "request to insert mgf01 record\n" );
	 
			 /* retrieve the data into a mcf01 structure and insert into database */
			 memset( &db_recs.mgf01, 0, sizeof( db_recs.mgf01 ) );
			 memcpy( &db_recs.mgf01, ( pMGF01 )p_data, sizeof( MGF01 ) );
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinMGF01 (&db_recs.mgf01,MODE_INSERT);
      		}
			 result_code = db_insert_mgf01( &db_recs.mgf01, ErrorMsg );
			 break;

      
      case BCH10_DATA:

         PRINT( "request to insert bch10 record\n" );

         /* retrieve the data into a ucf01 structure and insert into database */
         memset( &db_recs.bch10, 0, sizeof( db_recs.bch10 ) );
         memcpy( &db_recs.bch10, ( pBCH10 )p_data, sizeof( BCH10 ) );
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_INSERT_BCH10_SETTLE )
         {
	
            result_code = db_insert_bch10_mp_settle( &db_recs.bch10, ErrorMsg );
         }
         else
         {
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
      		}
        	 result_code = db_insert_bch10( &db_recs.bch10, ErrorMsg );
         }
            break;

      
       case DCF01_DATA:

         PRINT( "request to insert ucf01 record\n" );

         /* retrieve the data into a dcf01 structure and insert into database */
         memset( &db_recs.dcf01, 0, sizeof( db_recs.dcf01 ) );
         memcpy( &db_recs.dcf01, ( pDCF01 )p_data, sizeof( DCF01 ) );
  		if (DB_NullCheck_Flag==1)
  		{
  			CheckNULLTerminatedinDCF01 (&db_recs.dcf01,MODE_INSERT);
  		}
         result_code = db_insert_dcf01( &db_recs.dcf01, ErrorMsg );
         break;

      
     default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Insert request from %s", reply_que);
         break;
   }


   if (result_code == PTEMSG_OK)
   {
      
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Insert response to %s", reply_que);
      }
   }

   return (p_msg_out);
}



/*************************************************************************************/
pPTE_MSG process_update_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   LONG           data_length;
   BCH10          bch10;
   AUTH_TX		  auth_tx;
   int            standin_flag=0;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);
   data_length = sizeof(AUTH_TX);
   
	/****************************************************************/
	// Retive Auth Tx to chech its an MP or Fleet card transaction.
	memset (&auth_tx, 0, sizeof (auth_tx)) ;
	memcpy (&auth_tx, (AUTH_TX*) AtpBuffer, sizeof (auth_tx)) ;
	 if((strncmp(auth_tx.TLF01_details.response_code,"00",2) == 0) && 
			 (strncmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN,6)== 0)) 
	 {
			standin_flag = 1;
	 }
	/*****************************************************************/

   
   switch( app_data_type )
   {

      case TLF01_DATA:


         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.tlf01, 0, sizeof( db_recs.tlf01 ) );
         memcpy( &db_recs.tlf01, ( pTLF01 )p_data, sizeof( TLF01 ) );
		 if( strncmp(db_recs.tlf01.originator_queue,"stpmgr",6) == 0)
		 {
			 PRINT( "Request to Update TLF01_STIP recoed\n" );
			 result_code = db_update_tlf01_stip(&db_recs.tlf01,ErrorMsg);
		 }
		 else
		 {
			 PRINT( "request to update tlf01 record\n" );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinTLF01 (&db_recs.tlf01,MODE_UPDATE);
		 }
         result_code = db_update_tlf01( &db_recs.tlf01, ErrorMsg );
		 }
         break;

      
         
      case NMF01_DATA:

         PRINT( "request to update nmf01 record\n" );

         
         memset( &db_recs.nmf01, 0, sizeof( db_recs.nmf01 ) );
         memcpy( &db_recs.nmf01, ( pNMF01 )p_data, sizeof( NMF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinNMF01 (&db_recs.nmf01,MODE_UPDATE);
		 }
         result_code = db_update_nmf01( &db_recs.nmf01, ErrorMsg );
         memcpy(AtpBuffer + sizeof(AUTH_TX), &db_recs.nmf01, sizeof(NMF01));
         break;

      
         
      case NFI02_DATA:

         PRINT( "request to update nfi02 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.nfi02, 0, sizeof( db_recs.nfi02 ) );
         memcpy( &db_recs.nfi02, ( pNFI02 )p_data, sizeof( NFI02 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinNFI02 (&db_recs.nfi02,MODE_UPDATE);
		 }
         result_code = db_update_nfi02( &db_recs.nfi02, ErrorMsg );
         break;

      
      case NBF01_DATA:

         PRINT( "request to update nbf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.nbf01, 0, sizeof( db_recs.nbf01 ) );
         memcpy( &db_recs.nbf01, ( pNBF01 )p_data, sizeof( NBF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinNBF01 (&db_recs.nbf01,MODE_UPDATE);
		 }
         result_code = db_update_nbf01( &db_recs.nbf01, ErrorMsg );
         break;

      case NGF01_DATA:

         PRINT( "request to update ngf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.ngf01, 0, sizeof( db_recs.ngf01 ) );
         memcpy( &db_recs.ngf01, ( pNGF01 )p_data, sizeof( NGF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinNGF01 (&db_recs.ngf01,MODE_UPDATE);
		 }
         result_code = db_update_ngf01( &db_recs.ngf01, ErrorMsg );
         break;

      
      case MCF20_DATA:

         PRINT( "request to update mcf20 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.mcf20, 0, sizeof( db_recs.mcf20 ) );
         memcpy( &db_recs.mcf20, ( pMCF20 )p_data, sizeof( MCF20 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMCF20 (&db_recs.mcf20,MODE_UPDATE);
		 }
         result_code = db_update_mcf20( &db_recs.mcf20, ErrorMsg );


         break;

      case OCF20_DATA:

         PRINT( "request to update ocf20 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.ocf20, 0, sizeof( db_recs.ocf20 ) );
         memcpy( &db_recs.ocf20, ( pOCF20 )p_data, sizeof( OCF20 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinOCF20 (&db_recs.ocf20,MODE_UPDATE);
		 }
         result_code = db_update_ocf20( &db_recs.ocf20, ErrorMsg );
         break;

     

      case OCF01_DATA:

         PRINT( "request to update ocf01 record\n" );

         /* retrieve the data into a acf01 structure and update the database */
         memset( &db_recs.ocf01, 0, sizeof( db_recs.ocf01 ) );
         memcpy( &db_recs.ocf01, ( pOCF01 )p_data, sizeof( OCF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinOCF01 (&db_recs.ocf01,MODE_UPDATE);
		 }
         result_code = db_update_ocf01( &db_recs.ocf01, ErrorMsg );
         break;

      
      case MCF01_DATA:

         PRINT( "request to update mcf01 record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.mcf01, 0, sizeof( db_recs.mcf01 ) );
         memcpy( &db_recs.mcf01, ( pMCF01 )p_data, sizeof( MCF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMCF01 (&db_recs.mcf01,MODE_UPDATE);
		 }
         result_code = db_update_mcf01( &db_recs.mcf01, ErrorMsg );

         break;

	  case MGF01_DATA:
	  
		 PRINT( "request to update mgf01 record\n" );

		 /* retrieve the data into a bin01 structure and update the database */
		 memset( &db_recs.mgf01, 0, sizeof( db_recs.mgf01 ) );
		 memcpy( &db_recs.mgf01, ( pMGF01 )p_data, sizeof( MGF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMGF01 (&db_recs.mgf01,MODE_UPDATE);
		 }
		 result_code = db_update_mgf01( &db_recs.mgf01, ErrorMsg );
		 break;

      

      case BCH01_DATA:

         PRINT( "request to update bch01 record\n" );
         memset( &db_recs.bch01, 0, sizeof( db_recs.bch01 ) );
         memcpy( &db_recs.bch01, ( pBCH01 )p_data, sizeof( BCH01 ) );
         if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_GOOD_SETTLE)
            result_code = db_update_bch01_good_settle( &db_recs.bch01, ErrorMsg );
         else if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_DELETE_BCH10)
            result_code = db_update_bch01_delete_bch10( &db_recs.bch01, ErrorMsg );
         else 
            if(MsgSubtype2 == ST2_DB_UPDATE_MOVE_DELETE )
               result_code = db_update_bch01_move_bch10tobch11_delete_bch10( &db_recs.bch01, ErrorMsg);
         else
            if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_TO_CLOSE_BATCH)
               result_code = db_update_bch01_to_close_batch( &db_recs.bch01, ErrorMsg  );
         else
            if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_INSERT_BCH10)
            {
               p_data = p_data + sizeof(BCH01);
               memset( &bch10, 0, sizeof( BCH10 ) );
               memcpy( &bch10, ( pBCH10 )p_data, sizeof( BCH10 ) );
               printf("bch10_device_id: %s bch01_device_id: %s\n", bch10.primary_key.device_id,db_recs.bch01.primary_key.device_id);
               printf("bch10_open_date: %s bch01_open_date: %s\n", bch10.open_date,db_recs.bch01.open_date);
               printf("bch10_open_time: %s bch01_open_time: %s\n", bch10.open_time,db_recs.bch01.open_time);
               result_code = db_update_bch01_insert_bch10( &db_recs.bch01, &bch10,  ErrorMsg  );
            }
         else
         {
    		 if (DB_NullCheck_Flag==1)
    		 {
    			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
    		 }
            result_code = db_update_bch01( &db_recs.bch01, ErrorMsg );
         }
            break;

     
	case BCH01_MP_DATA:
	 
		  PRINT( "request to update bch01_mp record\n" );
		  memset( &db_recs.bch01_mp, 0, sizeof( db_recs.bch01_mp ) );
		  memcpy( &db_recs.bch01_mp, ( pBCH01_MP )p_data, sizeof( BCH01_MP ) );
		  if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_GOOD_SETTLE)
			 result_code = db_update_bch01_mp_good_settle( &db_recs.bch01_mp,standin_flag, ErrorMsg );
		  else if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_DELETE_BCH10)
			 result_code = db_update_bch01_mp_delete_bch10( &db_recs.bch01_mp, ErrorMsg );
		  else 
			 if(MsgSubtype2 == ST2_DB_UPDATE_MOVE_DELETE )
				result_code = db_update_bch01_mp_move_bch10tobch11_delete_bch10( &db_recs.bch01_mp, ErrorMsg);
		  else
			 if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_TO_CLOSE_BATCH)
				result_code = db_update_bch01_mp_to_close_batch( &db_recs.bch01_mp, ErrorMsg	);
		  else
			 if(MsgSubtype2 == ST2_DB_UPDATE_BCH01_INSERT_BCH10)
			 {
				p_data = p_data + sizeof(BCH01_MP);
				memset( &bch10, 0, sizeof( BCH10 ) );
				memcpy( &bch10, ( pBCH10 )p_data, sizeof( BCH10 ) );
				printf("bch10_device_id: %s bch01_mp_device_id: %s\n", bch10.primary_key.device_id,db_recs.bch01_mp.primary_key.device_id);
				printf("bch10_open_date: %s bch01_mp_open_date: %s\n", bch10.open_date,db_recs.bch01_mp.open_date);
				printf("bch10_open_time: %s bch01_mp_open_time: %s\n", bch10.open_time,db_recs.bch01_mp.open_time);
				result_code = db_update_bch01_mp_insert_bch10( &db_recs.bch01_mp, &bch10,  ErrorMsg  );
			 }
		  else
		  {
				 if (DB_NullCheck_Flag==1)
				 {
					CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
				 }
			  result_code = db_update_bch01_mp( &db_recs.bch01_mp, ErrorMsg );
		  }
			 break;
		  

      case DCF01_DATA:

         PRINT( "request to update dcf01 record\n" );

         /* retrieve the data into a ucf01 structure and update the database */
         memset( &db_recs.dcf01, 0, sizeof( db_recs.dcf01 ) );
         memcpy( &db_recs.dcf01, ( pDCF01 )p_data, sizeof( DCF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinDCF01 (&db_recs.dcf01,MODE_UPDATE);
		 }
         result_code = db_update_dcf01( &db_recs.dcf01, ErrorMsg );

         break;

      case ALL_DATA:
         PRINT( "request to update ALL_DATA record\n");
		 /*Check if its a MP or fllet card tansaction */
		if((0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))||
	   	   (0 == strncmp(auth_tx.TLF01_details.card_num, "7",1)))
		{
			 result_code = process_alldata_mp_msg(p_msg_in );    
		}
		else
		{
        	 result_code = process_alldata_msg(p_msg_in );  
		}
		break;

      case BCH20_DATA:

		  PRINT( "request to update dcf01 record\n" );

		   /* retrieve the data into a ucf01 structure and update the database */
		   memset( &db_recs.bch20, 0, sizeof( db_recs.bch20 ) );
		   memcpy( &db_recs.bch20, ( pBCH20 )p_data, sizeof( BCH20 ) );
		   if(MsgSubtype2 == ST2_DB_SELECT_BCH20_FOR_PREAUTH_REVERSE)
		   {
			   result_code = db_update_bch20_preauth_flag_by_card_num_revers( &db_recs.bch20, ErrorMsg );
		   }
		   else
		   {
			   result_code = db_update_bch20_preauth_flag_by_card_num( &db_recs.bch20, ErrorMsg );
		   }
         break;

/*	  case TLF01_STIP_DATA:
		  PRINT( "request to update tlf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
 /*        memset( &db_recs.tlf01_stip, 0, sizeof( db_recs.tlf01_stip ) );
         memcpy( &db_recs.tlf01_stip, ( pTLF01_STIP )p_data, sizeof( TLF01_STIP ) );
		 result_code = db_update_tlf01_stip( &db_recs.tlf01_stip,ErrorMsg );
		  break;*/

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type: %s for Update request from %s", app_data_type,reply_que);
         break;
   }


   if (result_code == PTEMSG_OK)
   {
      
      /*if((app_data_type == CAF01_DATA) && (Caf01InMem == true))
      {
         OffSet = FindMemRecord(p_data, sizeof(db_recs.caf01.primary_key), sizeof(CAF01), "Caf01Table");
         if(OffSet >= 0L)
            OffSet = UpdateMemRecord("Caf01Table", OffSet, sizeof(CAF01), p_data);
      }*/     
      
      updates_to_db_made = true;
      if(app_data_type == NMF01_DATA) 
            data_length = sizeof(AUTH_TX) + sizeof(NMF01);

       p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    data_length,
                                    app_data_type);
  
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Update response to %s", reply_que);
      }
   }

   return (p_msg_out);
}

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_negative_file( pPTE_MSG p_msg_in ) 
{
   pBYTE          p_data = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR;
   pBYTE          p_current = NULL_PTR;
   CHAR           temp[150];
   pPTE_MSG       p_msg_out = NULL_PTR;
  

   p_msg_out     = NULL_PTR;
  
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);
   printf("IN Process_negative_file. \n");

   
   if(ptemsg_get_msg_subtype1 (p_msg_in) ==  ST1_PROCESS_START)
   {
      PRINT( "Delete NMF01 = D and modify other recs. to have Action_code = P \n" );

     if( (result_code = db_update_delete_nmf01_action_code((pNMF01) p_data,ErrorMsg ) ) == PTEMSG_OK )
     {
         memcpy(p_current, (pBYTE )p_data, sizeof( NMF01 ));
         p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX) + sizeof(NMF01),
                                    NMF01_DATA);

         sprintf(temp, "NMF01 seq_nbr : %s\n", ((pNMF01)p_data)->primary_key.seq_nbr);
         PRINT(temp);
         if (p_msg_out == NULL_PTR)
         {
            result_code = PTEMSG_INSUFFICIENT_MEMORY;
            sprintf (ErrorMsg, "Insufficient Memory to build Delete response to %s", reply_que);
         }
     }
   }
   else  if(ptemsg_get_msg_subtype1 (p_msg_in) ==  ST1_PROCESS_COMPLETE)
   {
      PRINT("Modify Action_code to R \n");
      result_code = db_update_nmf01_action_code(ErrorMsg );
   }
      
   
   return(p_msg_out);
}
/*************************************************************************************/
/* Function    :  process_db_cleanup
   Parameters  :  pointer to a message of type pPTEMSG
   Return      :  None  
   Description :  Depending on the data type of the message( tlf01,ngf01 etc...), delete 
                  all the records that are older than DATE (sent as p_data)in the
                  corresponding table 
   Authour     :  Nov 17th'98, Supriya.
*/
/*************************************************************************************/
/*************************************************************************************/
void process_db_cleanup( pPTE_MSG p_msg_in ) 
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pBYTE          p_current;
  
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case TLF01_DATA:

         PRINT( "request to delete tlf01 records less than date\n" );

         result_code = db_delete_tlf01_by_date( ( pCHAR )p_data, ErrorMsg );
         break;
      case NFI01_DATA:

         PRINT( "request to delete nfi01 records less than date\n" );

         result_code = db_delete_nfi01_by_date( ( pCHAR )p_data, ErrorMsg );
         break;

      
      case DRF01_DATA:

         PRINT( "request to delete drf01 records less than date\n" );

         result_code = db_delete_drf01_by_date( ( pCHAR )p_data, ErrorMsg );
         break;

      
      case BCH01_DATA:
         PRINT(" request to delete bch01, bch10, bch11 records by date \n");
         
         result_code =  db_delete_bchrecs_for_cleanup( (pCHAR )p_data, ErrorMsg );

         break;

	case BCH01_MP_DATA:
         PRINT(" request to delete bch01_MP, bch10, bch11 records by date \n");
         
         result_code =  db_delete_bchrecs_mp_for_cleanup( (pCHAR )p_data, ErrorMsg );

         break;
      
      default:
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for CleanUp request from %s", reply_que);
         break;
   }

}
/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_delete_msg( pPTE_MSG p_msg_in ) 
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
  
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) );
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case TLF01_DATA:

         PRINT( "request to delete tlf01 record\n" );
		 if( ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_TLF01_STANDIN )
		 {
			 result_code = db_delete_tlf01_by_standin( ( pTLF01 )p_data, ErrorMsg);
		 }
         /* retrieve the clf01 name and delete record from the database */
		 else
		 {
			result_code = db_delete_tlf01( ( pTLF01 )p_data, ErrorMsg );

		 }
         break;

      
       case NFI01_DATA:

         PRINT( "request to delete nfi01 record\n" );

         /* retrieve the nfi01 filename and delete record from the database */
         result_code = db_delete_nfi01( ( pNFI01 )p_data, ErrorMsg );
         break;

      
      case NBF01_DATA:

         PRINT( "request to delete nbf01 record\n" );

         /* retrieve the nbf01 name and delete record from the database */
         result_code = db_delete_nbf01( ( pNBF01 )p_data, ErrorMsg );
         break;

      case NGF01_DATA:

         PRINT( "request to delete ngf01 record\n" );

         /* retrieve the ngf01 name and delete record from the database */
         result_code = db_delete_ngf01( ( pNGF01 )p_data, ErrorMsg );
         break;

      case NTF01_DATA:

        if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_DELETE_ALL_NTF01_RECORDS)
        {
            PRINT( "request to delete all records in ntf01 table\n" );

            result_code = db_delete_all_ntf01(  ErrorMsg );
        }
         break;

      
      case MCF20_DATA:

         PRINT( "request to delete mcf20 record\n" );

         /* retrieve the mcf20 name and delete record from the database */
         result_code = db_delete_mcf20( ( pMCF20 )p_data, ErrorMsg );
         break;

      case OCF20_DATA:

         PRINT( "request to delete ocf20 record\n" );

         /* retrieve the ocf20 name and delete record from the database */
         result_code = db_delete_ocf20( ( pOCF20 )p_data, ErrorMsg );
         break;

      

      case OCF01_DATA:

         PRINT( "request to delete ocf01 record\n" );

         /* retrieve the ocf01 name and delete record from the database */
         result_code = db_delete_ocf01( ( pOCF01 )p_data, ErrorMsg );
         break;

      
      case MCF01_DATA:

         PRINT( "request to delete mcf01 record\n" );

         /* retrieve the mcf01 name and delete record from the database */
         result_code = db_delete_mcf01( ( pMCF01 )p_data, ErrorMsg );
         break;

	  case MGF01_DATA:
	  
	     PRINT( "request to delete mgf01 record\n" );
  
	     /* retrieve the mgf01 name and delete record from the database */
	     result_code = db_delete_mgf01( ( pMGF01 )p_data, ErrorMsg );
	     break;

      
      case BCH10_DATA:

         PRINT( "request to delete bch10 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_bch10( ( pBCH10 )p_data, ErrorMsg );
         break;

      
      case DCF01_DATA:

         PRINT( "request to delete dcf01 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_dcf01( ( pDCF01 )p_data, ErrorMsg );
         break;

      
      case NFI02_DATA:

         PRINT( "request to delete nfi02 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_nfi02( ( pNFI02 )p_data, ErrorMsg );
         break;

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Delete request from %s", reply_que);
         break;

   }


   if (result_code == PTEMSG_OK)
   {
      /* shared mem deletes moved to function PinnacleMsgHdlr */      
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Delete response to %s", reply_que);
      }
   }

   return (p_msg_out);
}

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_select_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   CHAR           temp[150];
   pBYTE          p_current;
   pGENERIC_KEYS  p_GenericKeys;
   pAUTH_TX       auth_tx;
   BYTE           subtype2;
   BYTE           tranid[21];
   EMV_RAW        emvraw;
   CHAR current_system_date[9]={0};

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   
   auth_tx = (pAUTH_TX)p_data;
   p_data = p_data + sizeof(AUTH_TX);
   switch( app_data_type )
   {
	  case BCH20_DATA:
			if(ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER_CARD_NBR)
			{
			   if( ( result_code = db_select_bch20_by_auth_nbr_card_nbr( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.bch20),
                                          BCH20_DATA);

                   sprintf(temp, "select BCH20 by auth number & card nbr: %s\n", db_recs.bch20.auth_number);
                   PRINT(temp);
			   }
			 }

		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER)
		   {
			   if( ( result_code = db_select_bch20_by_auth_nbr( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.bch20),
                                          BCH20_DATA);

                   sprintf(temp, "select BCH20 by auth number : %s\n", db_recs.bch20.auth_number);
                   PRINT(temp);
			   }
		   }
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_BY_STAN)
		   {
			   if( ( result_code = db_select_bch20_by_stan( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.bch20),
                                          BCH20_DATA);

                   sprintf(temp, "select BCH20 by stan : %s\n", db_recs.bch20.auth_number);
                   PRINT(temp);
			   }
		   }
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_CUP)
		   {
			   if( ( result_code = db_select_bch20_for_CUP( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.bch20),
                                          BCH20_DATA);

                   sprintf(temp, "select BCH20 FOR CUP Card:%s Amount:%\n", db_recs.bch20.card_nbr,db_recs.bch20.tran_amount);
                   PRINT(temp);
			   }
		   }
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_BY_RRN)
		   {
			   if( ( result_code = db_select_bch20_by_rrn( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.bch20),
                                          BCH20_DATA);

                   sprintf(temp, "select BCH20 by rrn : %s\n", db_recs.bch20.auth_number);
                   PRINT(temp);
			   }
		   }
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_PREAUTH ||
				   ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH)
		   {
#if AMEX03B_APRIL2024
				create_system_date(current_system_date );
				if(0 != memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
				{
					//get only last updated date ,
					 //the last update date should be equal to current system date,
					get_last_dated_date_only();
					if(0 == memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
					{
						get_variable_auth_config_details();
					}
				}
#endif
			   if( ( result_code = db_select_bch20_for_preauth( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											ptemsg_get_msg_subtype1 (p_msg_in),
											ptemsg_get_msg_subtype2 (p_msg_in),
											reply_que,
											application_que_name,
											(pBYTE)AtpBuffer,
											sizeof(AUTH_TX) + sizeof(db_recs.bch20),
											BCH20_DATA);

					 sprintf(temp, "select BCH20 For Preauth : %s\n", db_recs.bch20.auth_number);
					 PRINT(temp);
			   }
		   }
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_INCREMENTAL_PREAUTH)
		   {
			   if( ( result_code = db_select_bch20_for_incremental_preauth( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											ptemsg_get_msg_subtype1 (p_msg_in),
											ptemsg_get_msg_subtype2 (p_msg_in),
											reply_que,
											application_que_name,
											(pBYTE)AtpBuffer,
											sizeof(AUTH_TX) + sizeof(db_recs.bch20),
											BCH20_DATA);

					 sprintf(temp, "select BCH20 For Preauth : %s\n", db_recs.bch20.auth_number);
					 PRINT(temp);
			   }
		   }
		  /* else if(ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH)
		   {
				create_system_date(current_system_date );
				if(0 != memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
				{
					//get only last updated date ,
					 //the last update date should be equal to current system date,
					get_last_dated_date_only();
					if(0 == memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
					{
						get_variable_auth_config_details();
					}
				}
			   if( ( result_code = db_select_bch20_for_AMEX_preauth( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											ptemsg_get_msg_subtype1 (p_msg_in),
											ptemsg_get_msg_subtype2 (p_msg_in),
											reply_que,
											application_que_name,
											(pBYTE)AtpBuffer,
											sizeof(AUTH_TX) + sizeof(db_recs.bch20),
											BCH20_DATA);

					 sprintf(temp, "select BCH20 For Preauth : %s\n", db_recs.bch20.auth_number);
					 PRINT(temp);
			   }
		   }*/
#if AMEX03B_APRIL2024
		   else if(ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_ESTIMATED_AUTHORIZATION)
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

				if(( result_code =db_select_bch20_for_estimated_preauth( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK)
				{
					   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
					   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
												ptemsg_get_msg_subtype1 (p_msg_in),
												ptemsg_get_msg_subtype2 (p_msg_in),
												reply_que,
												application_que_name,
												(pBYTE)AtpBuffer,
												sizeof(AUTH_TX) + sizeof(db_recs.bch20),
												BCH20_DATA);

						 sprintf(temp, "select BCH20 For Preauth : %s\n", db_recs.bch20.auth_number);
						 PRINT(temp);
				}
		   }
#endif

#if BDOR_62_PREAUTH_VOID
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_PREAUTH_VOID)
		   {
			   if( ( result_code = db_select_bch20_for_preauth_void( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											ptemsg_get_msg_subtype1 (p_msg_in),
											ptemsg_get_msg_subtype2 (p_msg_in),
											reply_que,
											application_que_name,
											(pBYTE)AtpBuffer,
											sizeof(AUTH_TX) + sizeof(db_recs.bch20),
											BCH20_DATA);

					 sprintf(temp, "select BCH20 For Preauth Void : %s\n", db_recs.bch20.auth_number);
					 PRINT(temp);
			   }
		   }
		   else if (ptemsg_get_msg_subtype2(p_msg_in) == ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT)
		   {
			   if( ( result_code = db_select_bch20_for_preauth_total_amount( ( pBCH20)p_data, &db_recs.bch20, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_recs.bch20, sizeof( db_recs.bch20 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											ptemsg_get_msg_subtype1 (p_msg_in),
											ptemsg_get_msg_subtype2 (p_msg_in),
											reply_que,
											application_que_name,
											(pBYTE)AtpBuffer,
											sizeof(AUTH_TX) + sizeof(db_recs.bch20),
											BCH20_DATA);

					 sprintf(temp, "select BCH20 For Preauth Void : %s\n", db_recs.bch20.auth_number);
					 PRINT(temp);
			   }
		   }

#endif

         break;

      case TLF01_DATA:

         subtype2 = ptemsg_get_msg_subtype2( p_msg_in );
         switch( subtype2 )
         {
            case ST2_NONE:
               result_code = db_select_tlf01( (pCHAR)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_BY_RRN:
               result_code = db_select_tlf01_by_rrn( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_BY_AUTH_NBR:
				if((0 == strncmp(auth_tx->TLF01_details.product_codes[2].quantity,"1",1))||
	   	   		   (0 == strncmp(auth_tx->TLF01_details.card_num, "7",1)))
				   {
                   		result_code = db_select_tlf01_mp_by_auth_nbr( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
				   }
				   else
				   {
						result_code = db_select_tlf01_by_auth_nbr( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
					}
               break;

            case ST2_DB_SELECT_TLF01_BY_STAN:
               result_code = db_select_tlf01_by_stan( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_BY_TERMINAL_ID:
               result_code = db_select_tlf01_by_terminal_id( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_BY_ORIGINAL_TRAN:
               result_code = db_select_tlf01_by_original_tran( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_BY_REVERSED_TRAN:
               result_code = db_select_tlf01_by_reversed_tran( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_FOR_SALES_COMPLETION:
               result_code = db_select_tlf01_for_sales_completion( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_FOR_TERM_LENGTH:
               result_code = db_select_tlf01_by_auth_term_nbr( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TLF01_FOR_VISA_REJECTS:
               result_code = db_select_tlf01_for_visa_rejects( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_TRAN_BY_CARD_NUM:
               result_code = db_select_tlf01_for_void_reload( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;

            case ST2_DB_SELECT_LAST_AUTH_ID:
               result_code = db_select_tlf01_last_auth_nbr( (pTLF01)p_data, &db_recs.tlf01, ErrorMsg );
               break;
         }

         if( result_code == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.tlf01, sizeof( db_recs.tlf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.tlf01),
                                          TLF01_DATA);
         }

         break;

         case TLF01_SAF_DATA:

         PRINT( "request to retrieve TLF01_SAF record\n" );

         PRINT(p_data);
         PRINT("\n");

         if(ptemsg_get_msg_subtype2 (p_msg_in) == 0)
         {
            if( ( result_code = db_select_tlf01_saf( ( pTLF01)p_data, &db_recs.tlf01, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.tlf01, sizeof( db_recs.tlf01));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.tlf01),
                                          TLF01_SAF_DATA);
            }
         }
         break;


      case NFI02_DATA:

         if(ptemsg_get_msg_subtype2 (p_msg_in) == 0)
         {
            if( ( result_code = db_select_nfi02( ( pCHAR)p_data, &db_recs.nfi02, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.nfi02, sizeof( db_recs.nfi02 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.nfi02),
                                          NFI02_DATA);
            }
         }
         break;


      case BCH10_DATA:

         subtype2 = ptemsg_get_msg_subtype2(p_msg_in);
         switch( subtype2 )
         {
            case ST2_DB_SELECT_BCH10_BY_RRN:

                     result_code = db_select_bch10_by_rrn((pBCH10)p_data, &db_recs.bch10, ErrorMsg);

                     if ( result_code == PTEMSG_OK )
                     {
                        /* Fetch any EMV data associated with this txn. */
                        memset( &emvraw, 0x00, sizeof(EMV_RAW) );
                        memset( tranid, 0x00, sizeof(tranid) );
                        memcpy( tranid, db_recs.bch10.transaction_id, sizeof(tranid));
                        result_code = db_select_emvraw(tranid,&emvraw,ErrorMsg);
                        if ( result_code == PTEMSG_OK )
                        {
                           memcpy( auth_tx->EMV_details.emv_block,
                                   emvraw.RAW_DATA,
                                   atoi(emvraw.DATA_LENGTH) );

                           memcpy( AtpBuffer, auth_tx, sizeof(AUTH_TX) );
                        }
                        else
                        {
                           /* Orig txn has been found, so even if
                            * EMV data is not found, still return success.
                            */
                           result_code = PTEMSG_OK;
                        }
                     }
                     break;

            case ST2_DB_SELECT_TLF01_BY_AUTH_NBR:
				if((0 == strncmp(auth_tx->TLF01_details.product_codes[2].quantity,"1",1))||
	   	   		    (0 == strncmp(auth_tx->TLF01_details.card_num, "7",1)))
					{
						result_code = db_select_bch10_mp_by_auth_nbr((pBCH10)p_data, &db_recs.bch10, ErrorMsg);
					}
					else
					{
                    	 result_code = db_select_bch10_by_auth_nbr((pBCH10)p_data, &db_recs.bch10, ErrorMsg);
					}
                    break;

            case ST2_DB_SELECT_TLF01_BY_ORIGINAL_TRAN:

                     result_code = db_select_bch10_by_original_transaction((pBCH10)p_data, &db_recs.bch10, ErrorMsg);
                     break;

            case ST2_NONE:

                     result_code = db_select_bch10((pBCH10)p_data, &db_recs.bch10, ErrorMsg);
                     break;

            case ST2_DB_SELECT_ACTIVE_BCH10:

                     result_code = db_select_active_bch10((pBCH10)p_data, &db_recs.bch10, ErrorMsg);
                     break;

            case ST2_DB_SELECT_BCH10_BY_STAN:

                     result_code = db_select_bch10_by_stan((pBCH10)p_data, &db_recs.bch10, ErrorMsg);

                     if ( result_code == PTEMSG_OK )
                     {
                        /* Fetch any EMV data associated with this txn. */
                        memset( &emvraw, 0x00, sizeof(EMV_RAW) );
                        memset( tranid, 0x00, sizeof(tranid) );
                        memcpy( tranid, db_recs.bch10.transaction_id, sizeof(tranid));
                        result_code = db_select_emvraw(tranid,&emvraw,ErrorMsg);
                        if ( result_code == PTEMSG_OK )
                        {
                        	if (true == devds_check_If_Diners_Transaction(auth_tx) &&
                        	   (strlen(auth_tx->EMV_details.emv_block) > 0 )	   &&
                        		   0 == strncmp(auth_tx->TLF01_details.message_type, "0400", 4))
                        	{
                        		/* Do not copy emv_raw for Diners */
                        	}
                        	else
                        	{
							   memcpy( auth_tx->EMV_details.emv_block,
									   emvraw.RAW_DATA,
									   atoi(emvraw.DATA_LENGTH) );

							   memcpy( AtpBuffer, auth_tx, sizeof(AUTH_TX) );
                        	}
                        }
                        else
                        {
                           /* Orig txn has been found, so even if
                            * EMV data is not found, still return success.
                            */
                           result_code = PTEMSG_OK;
                        }
                     }
                     break;

			case ST2_DB_SELECT_BCH10_FOR_CUP:
					 result_code = db_select_bch10_for_CUP((pBCH10)p_data, &db_recs.bch10, ErrorMsg);

                     if ( result_code == PTEMSG_OK )
                     {
                        /* Fetch any EMV data associated with this txn. */
                        memset( &emvraw, 0x00, sizeof(EMV_RAW) );
                        memset( tranid, 0x00, sizeof(tranid) );
                        memcpy( tranid, db_recs.bch10.transaction_id, sizeof(tranid));
                        result_code = db_select_emvraw(tranid,&emvraw,ErrorMsg);
                        if ( result_code == PTEMSG_OK )
                        {
							if(0 != strncmp(auth_tx->TLF01_details.message_type,"04",2))
							{
								memcpy( auth_tx->EMV_details.emv_block,
										emvraw.RAW_DATA,
										atoi(emvraw.DATA_LENGTH) );

								memcpy( AtpBuffer, auth_tx, sizeof(AUTH_TX) );
							}
                        }
                        else
                        {
                           /* 
                           					Orig txn has been found, so even if EMV data is not found, still return success.
                           				*/
                           result_code = PTEMSG_OK;
                        }
                     }
                     break;

            case ST2_DB_SELECT_BCH10_BY_BATCH_TID_STAN:

                     p_GenericKeys = ( pGENERIC_KEYS )p_data;
					 if((0 == strncmp(auth_tx->TLF01_details.product_codes[2].quantity,"1",1))||
	   	   				(0 == strncmp(auth_tx->TLF01_details.card_num, "7",1)))
						{
                     		result_code = db_select_bch10_mp_detail_by_batch_tid_stan( p_GenericKeys->key1,p_GenericKeys->key2,p_GenericKeys->key3, &db_recs.bch01_mp, ErrorMsg);
					 	}
					    else
					    {
							result_code = db_select_bch10_detail_by_batch_tid_stan( p_GenericKeys->key1,p_GenericKeys->key2,p_GenericKeys->key3, p_GenericKeys->key4, &db_recs.bch01, ErrorMsg);
						}
                     break;

            case ST2_DB_SELECT_BCH10_BY_BATCH_NBR:

                     result_code = db_select_bch10_by_batch_nbr((pCHAR)p_data, &db_recs.bch10, ErrorMsg);
                     break;

            case ST2_DB_SELECT_BCH10_DUPLICATE_TXN:

                     result_code = db_select_bch10_duplicate_txn((pBCH10)p_data, &db_recs.bch10, ErrorMsg);
                     break;
         }

         if ( result_code == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.bch10, sizeof( db_recs.bch10 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          subtype2,
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.bch10),
                                          BCH10_DATA);
         }
         break;

      case BCH11_DATA:
         PRINT( "request to retrieve bch11 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH11_BY_BATCH_NBR)
         {
            if( ( result_code = db_select_bch11_by_batch_nbr( ( pCHAR )p_data, &db_recs.bch11, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.bch11, sizeof( db_recs.bch11 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.bch11),
                                             BCH11_DATA);

               sprintf(temp,"BCH11 device_id : %s merchant_id : %s sys_trace_audit_num: %s batch_nbr : %s\n", db_recs.bch11.primary_key.device_id,db_recs.bch11.merchant_id,db_recs.bch11.sys_trace_audit_num,db_recs.bch11.primary_key.batch_nbr);
               PRINT(temp);
            }
         }
         break;
      
         case NBF01_DATA:

         PRINT( "request to retrieve nbf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_nbf01( ( pCHAR)p_data, &db_recs.nbf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.nbf01, sizeof( db_recs.nbf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.nbf01),
                                          NBF01_DATA);

            sprintf(temp, "NBF01 low : %s High : %s\n", db_recs.nbf01.primary_key.low, db_recs.nbf01.primary_key.high);
            PRINT(temp);
         }
         break;
         case NGF01_DATA:

         PRINT( "request to retrieve ngf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ngf01( ( pCHAR)p_data, &db_recs.ngf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ngf01, sizeof( db_recs.ngf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.ngf01),
                                          NGF01_DATA);

            sprintf(temp, "NGF01 negative_group_id : %s\n", db_recs.ngf01.primary_key.negative_group_id);
            PRINT(temp);
         }
         break;


      
      case MCF20_DATA:

         PRINT( "request to retrieve mcf20 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_mcf20( ( pCHAR)p_data, &db_recs.mcf20, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.mcf20, sizeof( db_recs.mcf20 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.mcf20),
                                          MCF20_DATA);

            sprintf(temp, "MCF20 organization_id : %s merchant_id : %s yyyymmdd : %s\n", db_recs.mcf20.primary_key.organization_id, db_recs.mcf20.primary_key.merchant_id, db_recs.mcf20.primary_key.yyyymmdd);
            PRINT(temp);
         }
         break;

      case OCF20_DATA:

         PRINT( "request to retrieve ocf20 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ocf20( ( pCHAR)p_data, &db_recs.ocf20, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ocf20, sizeof( db_recs.ocf20 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.ocf20),
                                          OCF20_DATA);

            sprintf(temp, "OCF20 organization_id : %s yyyymmdd : %s\n", db_recs.ocf20.primary_key.organization_id, db_recs.ocf20.primary_key.yyyymmdd);
            PRINT(temp);
         }
         break;

      
      case OCF01_DATA:

         PRINT( "request to retrieve ocf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ocf01( ( pCHAR)p_data, &db_recs.ocf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ocf01, sizeof( db_recs.ocf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.ocf01),
                                          OCF01_DATA);

            sprintf(temp, "OCF01 organization_id : %s\n", db_recs.ocf01.primary_key.organization_id);
            PRINT(temp);
         }
         break;

      
      case MCF01_DATA:

         PRINT( "request to retrieve mcf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
		 /*
			check if its a CUP transaction or not
		 */

         if (true == devds_Check_If_CUP_Transaction(auth_tx))
		 {
			if( ( result_code = db_select_mcf01_CUP( ( pCHAR)p_data, &db_recs.mcf01, ErrorMsg ) ) == PTEMSG_OK )
	         {
	            memcpy(p_current, (BYTE *)&db_recs.mcf01, sizeof( db_recs.mcf01 ));
	            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	                                          ptemsg_get_msg_subtype1 (p_msg_in),
	                                          ptemsg_get_msg_subtype2 (p_msg_in),
	                                          reply_que,
	                                          application_que_name,
	                                          (pBYTE)AtpBuffer,
	                                          sizeof(AUTH_TX) + sizeof(db_recs.mcf01),
	                                          MCF01_DATA);

	            sprintf(temp, "MCF01 organization_id : %s merchant_id : %s\n", db_recs.mcf01.primary_key.organization_id, db_recs.mcf01.primary_key.merchant_id);
	            PRINT(temp);
	         }
		 }
		 else
		 {
         	if( ( result_code = db_select_mcf01( ( pCHAR)p_data, &db_recs.mcf01, ErrorMsg ) ) == PTEMSG_OK )
         	{
         		/*IF tx is RPS and VISA get the VISA Tran ID*/
         		if(auth_tx->TLF01_details.card_num[0] == '4')
				{	/*If tx is RPS*/
         			if('Y' == db_recs.mcf01.rps_merchant[0] &&
         			   'C' != auth_tx->TLF01_details.product_codes[7].quantity[0] &&
					   'R' != auth_tx->TLF01_details.product_codes[7].quantity[0])
         			{
         				result_code = db_select_RPS01_for_VISA(auth_tx, &db_recs.rps01, ErrorMsg);
         				memcpy( AtpBuffer, auth_tx, sizeof(AUTH_TX) );
         			}
				}
	            memcpy(p_current, (BYTE *)&db_recs.mcf01, sizeof( db_recs.mcf01 ));
	            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	                                          ptemsg_get_msg_subtype1 (p_msg_in),
	                                          ptemsg_get_msg_subtype2 (p_msg_in),
	                                          reply_que,
	                                          application_que_name,
	                                          (pBYTE)AtpBuffer,
	                                          sizeof(AUTH_TX) + sizeof(db_recs.mcf01),
	                                          MCF01_DATA);

	            sprintf(temp, "MCF01 organization_id : %s merchant_id : %s\n", db_recs.mcf01.primary_key.organization_id, db_recs.mcf01.primary_key.merchant_id);
	            PRINT(temp);
         	}
		 }
         break;

	case MGF01_DATA:

         PRINT( "request to retrieve mgf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_mgf01( ( pCHAR)p_data, &db_recs.mgf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.mgf01, sizeof( db_recs.mgf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.mgf01),
                                          MGF01_DATA);

            sprintf(temp, "MGF01 merchant_id : %s\n", db_recs.mgf01.primary_key.merchant_group_id);
            PRINT(temp);
         }
         break;

     
     case BCH01_DATA:

         PRINT( "request to retrieve bch01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if((ptemsg_get_msg_subtype2 (p_msg_in) == 0) ||
			(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_BCH01_BY_BATCH_NBR))
         {
            //if( ( result_code = db_select_bch01( ( pCHAR )p_data, &db_recs.bch01, ErrorMsg ) ) == PTEMSG_OK )
			if( ( result_code = db_select_bch01_duplicate_batch_nbr( ( pCHAR )p_data, &db_recs.bch01, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.bch01, sizeof( db_recs.bch01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.bch01),
                                             BCH01_DATA);

               sprintf(temp, "BCH01 device_id : %s  batch_nbr : %s\n", db_recs.bch01.primary_key.device_id, db_recs.bch01.primary_key.batch_nbr);
               PRINT(temp);
            }
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH01_BY_STATUS)
         {
            if( ( result_code = db_select_bch01_by_status( ( pBCH01 )p_data, &db_recs.bch01, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.bch01, sizeof( db_recs.bch01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.bch01),
                                             BCH01_DATA);

               sprintf(temp, "BCH01 device_id : %s  batch_nbr : %s\n", db_recs.bch01.primary_key.device_id, db_recs.bch01.primary_key.batch_nbr);
               PRINT(temp);
            }
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH01_BY_DATE)
         {
            if( ( result_code = db_select_bch01_by_date( ( pBCH01 )p_data, &db_recs.bch01,ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.bch01, sizeof( db_recs.bch01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.bch01),
                                             BCH01_DATA);

               sprintf(temp, "BCH01 device_id : %s  batch_nbr : %s open_date : %s open_time %s \n", db_recs.bch01.primary_key.device_id, db_recs.bch01.primary_key.batch_nbr, db_recs.bch01.open_date, db_recs.bch01.open_time);
               PRINT(temp);
            }
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_RETURN_BCH01_DELETE_BCH10)
         {
            if( ( result_code = db_return_bch01_delete_bch10( ( pBCH01 )p_data, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)p_data, sizeof( BCH01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(BCH01),
                                             BCH01_DATA);

               sprintf(temp, "BCH01 device_id : %s  batch_nbr : %s \n", ((pBCH10)p_data)->primary_key.device_id,((pBCH10)p_data)->primary_key.batch_nbr);
               PRINT(temp);
            }
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH01_BY_STATUS_AND_TID)
         {
            if( ( result_code = db_select_bch01_by_status_and_tid( ( pBCH01 )p_data, &db_recs.bch01, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_recs.bch01, sizeof( db_recs.bch01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.bch01),
                                             BCH01_DATA);

               sprintf(temp, "BCH01 device_id : %s  batch_nbr : %s\n", db_recs.bch01.primary_key.device_id, db_recs.bch01.primary_key.batch_nbr);
               PRINT(temp);
            }
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS)
         {
            result_code = db_select_or_insert_bch01_by_status( ( pBCH01 )p_data, &db_recs.bch01, ErrorMsg );
            printf("result_code for sel_insert : %d", result_code);
            if( result_code == PTEMSG_OK)
            {
               memcpy(p_current, (BYTE *)&db_recs.bch01, sizeof( db_recs.bch01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.bch01),
                                             BCH01_DATA);

               sprintf(temp, "BCH01 device_id : %s  batch_nbr : %s\n", db_recs.bch01.primary_key.device_id, db_recs.bch01.primary_key.batch_nbr);
               PRINT(temp);
			   updates_to_db_made = true ; /* if the record had been inserted */
            }
            else if(result_code == PTEMSG_NOT_FOUND)
            {
               PRINT( "request to insert bch01 record\n" );
               memset( &db_recs.bch01, 0, sizeof( db_recs.bch01 ) );
               strcpy(db_recs.bch01.primary_key.device_id ,auth_tx->TLF01_details.terminal_id);                                
					strcpy(db_recs.bch01.primary_key.batch_nbr,"ACTIVE");
              	strcpy(db_recs.bch01.batch_status,"A");

               strcpy(db_recs.bch01.open_date ,auth_tx->TLF01_details.date_yyyymmdd);                                
              	strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
              	if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                  strcpy(db_recs.bch01.visa_merchant_id,"GES");
               else
              	   strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
               strcpy(db_recs.bch01.open_time ,auth_tx->TLF01_details.time_hhmmss);
        		if (DB_NullCheck_Flag==1)
        		{
        			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_INSERT);
        		}
               result_code = db_insert_bch01( &db_recs.bch01, ErrorMsg );
               printf("\nreturn value after BCH01 insert: %d", result_code);
               if ( result_code == PTEMSG_OK )
                  p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX)+ sizeof(db_recs.bch01),
                                    BCH01_DATA);
               else
                   result_code = PTEMSG_INSERT_FAILED;
            }
         }

         break;

	  
	 case BCH01_MP_DATA:
	  
	   PRINT( "request to retrieve bch01_mp record\n" );

	   PRINT(p_data);
	   PRINT("\n");
	   if((ptemsg_get_msg_subtype2 (p_msg_in) == 0) ||
		  (ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_BCH01_BY_BATCH_NBR))
	   {
		  if( ( result_code = db_select_bch01_mp_duplicate_batch_nbr( ( pCHAR )p_data, &db_recs.bch01_mp, ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)&db_recs.bch01_mp, sizeof( db_recs.bch01_mp ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   sizeof(AUTH_TX) + sizeof(db_recs.bch01_mp),
										   BCH01_MP_DATA);

			 sprintf(temp, "BCH01_MP device_id : %s  batch_nbr : %s\n", db_recs.bch01_mp.primary_key.device_id, db_recs.bch01_mp.primary_key.batch_nbr);
			 PRINT(temp);
		  }
	   }
	   else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH01_BY_STATUS)
	   {
		  if( ( result_code = db_select_bch01_mp_by_status( ( pBCH01_MP )p_data, &db_recs.bch01_mp, ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)&db_recs.bch01_mp, sizeof( db_recs.bch01_mp ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   sizeof(AUTH_TX) + sizeof(db_recs.bch01_mp),
										   BCH01_MP_DATA);

			 sprintf(temp, "BCH01_MP device_id : %s  batch_nbr : %s\n", db_recs.bch01_mp.primary_key.device_id, db_recs.bch01_mp.primary_key.batch_nbr);
			 PRINT(temp);
		  }
	   }
	   else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH01_BY_DATE)
	   {
		  if( ( result_code = db_select_bch01_mp_by_date( ( pBCH01_MP )p_data, &db_recs.bch01_mp,ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)&db_recs.bch01_mp, sizeof( db_recs.bch01_mp ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   sizeof(AUTH_TX) + sizeof(db_recs.bch01_mp),
										   BCH01_MP_DATA);

			 sprintf(temp, "BCH01_MP device_id : %s  batch_nbr : %s open_date : %s open_time %s \n", db_recs.bch01_mp.primary_key.device_id, db_recs.bch01_mp.primary_key.batch_nbr, db_recs.bch01_mp.open_date, db_recs.bch01_mp.open_time);
			 PRINT(temp);
		  }
	   }
	   else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_RETURN_BCH01_DELETE_BCH10)
	   {
		  if( ( result_code = db_return_bch01_mp_delete_bch10( ( pBCH01_MP )p_data, ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)p_data, sizeof( BCH01_MP ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   sizeof(AUTH_TX) + sizeof(BCH01_MP),
										   BCH01_MP_DATA);

			 sprintf(temp, "BCH01_MP device_id : %s  batch_nbr : %s \n", ((pBCH10)p_data)->primary_key.device_id,((pBCH10)p_data)->primary_key.batch_nbr);
			 PRINT(temp);
		  }
	   }
	   else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_BCH01_BY_STATUS_AND_TID)
	   {
		  if( ( result_code = db_select_bch01_mp_by_status_and_tid( ( pBCH01_MP )p_data, &db_recs.bch01_mp, ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)&db_recs.bch01_mp, sizeof( db_recs.bch01_mp ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   sizeof(AUTH_TX) + sizeof(db_recs.bch01_mp),
										   BCH01_MP_DATA);

			 sprintf(temp, "BCH01_MP device_id : %s  batch_nbr : %s\n", db_recs.bch01_mp.primary_key.device_id, db_recs.bch01_mp.primary_key.batch_nbr);
			 PRINT(temp);
		  }
	   }
	   else if(ptemsg_get_msg_subtype2 (p_msg_in) ==  ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS)
	   {
		  result_code = db_select_or_insert_bch01_mp_by_status( ( pBCH01_MP )p_data, &db_recs.bch01_mp, ErrorMsg );
		  printf("result_code for sel_insert : %d \n", result_code);
		  if( result_code == PTEMSG_OK)
		  {
			 printf("We are able to reterive data correctly send to dcpimp\n");
			 memcpy(p_current, (BYTE *)&db_recs.bch01_mp, sizeof( db_recs.bch01_mp ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   sizeof(AUTH_TX) + sizeof(db_recs.bch01_mp),
										   BCH01_MP_DATA);

			 sprintf(temp, "BCH01_MP device_id : %s  batch_nbr : %s\n", db_recs.bch01_mp.primary_key.device_id, db_recs.bch01_mp.primary_key.batch_nbr);
			 PRINT(temp);
			 printf("%s\n",temp);
			 updates_to_db_made = true ; /* if the record had been inserted */
		  }
		  else if(result_code == PTEMSG_NOT_FOUND)
		  {
			  printf("We are unable to reterive data correctly send to dcpimp\n");
			 PRINT( "request to insert bch01_mp record\n" );
			 memset( &db_recs.bch01_mp, 0, sizeof( db_recs.bch01_mp ) );
			 strcpy(db_recs.bch01_mp.primary_key.device_id ,auth_tx->TLF01_details.terminal_id);								 
				  strcpy(db_recs.bch01_mp.primary_key.batch_nbr,"ACTIVE");
			  strcpy(db_recs.bch01_mp.batch_status,"A");

			 strcpy(db_recs.bch01_mp.open_date ,auth_tx->TLF01_details.date_yyyymmdd); 							   
			  strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
			  if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
				strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
			 else
				 strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
			 strcpy(db_recs.bch01_mp.open_time ,auth_tx->TLF01_details.time_hhmmss);
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_INSERT);
      		}
			 result_code = db_insert_bch01_mp( &db_recs.bch01_mp, ErrorMsg );
			 printf("\nreturn value after BCH01_MP insert: %d\n", result_code);
			 if ( result_code == PTEMSG_OK )
				p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
								  ptemsg_get_msg_subtype1 (p_msg_in),
								  ptemsg_get_msg_subtype2 (p_msg_in),
								  reply_que,
								  application_que_name,
								  (pBYTE)AtpBuffer,
								  sizeof(AUTH_TX)+ sizeof(db_recs.bch01_mp),
								  BCH01_MP_DATA);
			 else
				 result_code = PTEMSG_INSERT_FAILED;
		  }
	   }

	   break;
	  
      
      case DCF01_DATA:

         PRINT( "request to retrieve dcf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
		 /*
			Find if its a CUP transaction or not.
		 */

         if (true == devds_Check_If_CUP_Transaction(auth_tx))
   		 {
   		 	    if(( result_code = db_select_dcf01_CUP( ( pCHAR )p_data, &db_recs.dcf01, ErrorMsg )) == PTEMSG_OK )
   		 	    {
					memcpy(p_current, (BYTE *)&db_recs.dcf01, sizeof( db_recs.dcf01 ));
	           		p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	                                          ptemsg_get_msg_subtype1 (p_msg_in),
	                                          ptemsg_get_msg_subtype2 (p_msg_in),
	                                          reply_que,
	                                          application_que_name,
	                                          (pBYTE)AtpBuffer,
	                                          sizeof(AUTH_TX) + sizeof(db_recs.dcf01),
	                                          DCF01_DATA);

	            	sprintf(temp, "DCF01 device_id : %s\n", db_recs.dcf01.primary_key.device_id);
	            	PRINT(temp);
   		 	    }
   		 }
         else 
         {
         	if(( result_code = db_select_dcf01( ( pCHAR )p_data, &db_recs.dcf01, ErrorMsg )) == PTEMSG_OK )
         	{
            	memcpy(p_current, (BYTE *)&db_recs.dcf01, sizeof( db_recs.dcf01 ));
          	    p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.dcf01),
                                          DCF01_DATA);

            	sprintf(temp, "DCF01 device_id : %s\n", db_recs.dcf01.primary_key.device_id);
            	PRINT(temp);
         	}
         }
         break;

      

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Select request from %s", reply_que);
         break;

   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build Select response to %s", reply_que);
   }

   return (p_msg_out);
}



/****************************added *************************************/
/****************************added *************************************/

BYTE process_alldata_msg( pPTE_MSG p_msg_in )
{
	BYTE           app_data_type, result, temp_tx_key;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pAUTH_TX		   auth_tx;
   BCH10          bch10;
   CHAR           primary_key[50] = "";
   CHAR           transaction_id [21] = "";
   CHAR           date_time[16] = "";
   CHAR           actual_amt[13];
   CHAR           adjust_amt[13];
   
   long           total, diff ;
   CHAR           bch01_open_date[9]="";
   CHAR           bch01_open_time[7]="";
   int            cnt;
   int            i;
   

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX); 
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   auth_tx = (pAUTH_TX)p_data;
   
   if (strcmp(auth_tx->TLF01_details.response_code ,"00") == 0)
   {   
      
        
      
      
      switch(auth_tx->TLF01_details.tx_key)
       {
         case AUTH_SALE_RESPONSE:
         case AUTH_CASH_ADVANCE_RESPONSE:
         case AUTH_SALES_COMPLETION_RESPONSE:
         case AUTH_OFFLINE_SALE_RESPONSE:
         case AUTH_QUASI_CASH_RESPONSE:
            PRINT("In SALE_RESPONSE \n");

            
            //BCH01
            strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
            strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
			strncpy(primary_key + BCH01_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
           
            PRINT("Before BCH01");
            //result_code = db_select_bch01(primary_key,&db_recs.bch01,ErrorMsg);
            result_code = db_select_bch01(primary_key,&db_recs.bch01,ErrorMsg);
            //printf("Result code for BCH01: %d",result_code);
            
            //create and Insert into BCH01, delete any ACTIVE trans. for this terminal 
            if(result_code == PTEMSG_NOT_FOUND)
				{
               //Delete BCH10 if any ACTIVE
               memset(&db_recs.bch10, 0, sizeof(BCH10));
				   strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
			   	strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
               result = db_delete_active_bch10(&db_recs.bch10,ErrorMsg);
               
               memcpy(&db_recs.bch01,primary_key,BCH01_PRIMARY_KEY_SIZE);

               //check if CREDIT sale or DEBIT sale
               if(auth_tx->bin_type =='D')
               {
                  strcpy(db_recs.bch01.total_debit_amt, auth_tx->TLF01_details.total_amount);
                  if(atoi(db_recs.bch01.total_debit_amt) != 0)
   						strcpy(db_recs.bch01.total_debit_cnt, "0001");
               }
               else if(auth_tx->bin_type == 'C')
               {
                  strcpy(db_recs.bch01.total_credit_amt, auth_tx->TLF01_details.total_amount);
                  if(atoi(db_recs.bch01.total_credit_amt) != 0)
   						strcpy(db_recs.bch01.total_credit_cnt, "0001");
               }
               genutil_format_date(date_time);
               if(strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
   					strcpy(db_recs.bch01.open_date ,auth_tx->TLF01_details.date_yyyymmdd);
               else
               {
                  memset(db_recs.bch01.open_date, 0, sizeof(db_recs.bch01.open_date));
   					strncpy(db_recs.bch01.open_date ,date_time, 8);
               }
               if(strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
   					strcpy(db_recs.bch01.open_time ,auth_tx->TLF01_details.time_hhmmss);
               else
               {
                  memset(db_recs.bch01.open_time, 0, sizeof(db_recs.bch01.open_time));
   					strncpy(db_recs.bch01.open_time ,date_time + 8, 6);
               }
   				strcpy(bch01_open_time,db_recs.bch01.open_time );
   				strcpy(bch01_open_date,db_recs.bch01.open_date );
					strcpy(db_recs.bch01.batch_status,"A");
					strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
               if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                  strcpy(db_recs.bch01.visa_merchant_id,"GES");
               else
              	   strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);

               //copy values into temporary variables
    
               PRINT("Before Insert into BCH01");
        		if (DB_NullCheck_Flag==1)
        		{
        			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_INSERT);
        		}
					result_code = db_insert_bch01(&db_recs.bch01,ErrorMsg);
               PRINT(ErrorMsg);
               PRINT("\nAfter Insert into BCH01");

					if(result_code != PTEMSG_OK)
						return(PTEMSG_INSERT_FAILED);
				}
				else if(result_code == PTEMSG_OK)
				{
					if(auth_tx->bin_type=='D')
               {
                  total = atol(db_recs.bch01.total_debit_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
                  if(atol(db_recs.bch01.total_debit_amt) != 0L)
                     cnt = atoi(db_recs.bch01.total_debit_cnt) + 1;
                  sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);
               }
               else
                if(auth_tx->bin_type=='C')
               {
                  total=atol(db_recs.bch01.total_credit_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
                  if(atol(db_recs.bch01.total_credit_amt) != 0L)
                     cnt = atoi(db_recs.bch01.total_credit_cnt) + 1;
                  sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);
               }

               //copy values into temporary variables
   				strcpy(bch01_open_time,db_recs.bch01.open_time );
   				strcpy(bch01_open_date,db_recs.bch01.open_date );

               PRINT("Before update for BCH01");
      		 if (DB_NullCheck_Flag==1)
      		 {
      			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
      		 }
					if(result_code = db_update_bch01(&db_recs.bch01,ErrorMsg) != PTEMSG_OK)
                         return(PTEMSG_UPDATE_FAILED);
               PRINT("After update for BCH01");

				}
            else
               return(PTEMSG_UPDATE_FAILED);
				  //return(p_msg_out);
              //PRINT("ERROR in BCH01 update/insert");  
            memset(&db_recs.bch10, 0, sizeof(BCH10));
   			//Insert into BCH10
				strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
				strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
				strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);

            strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
				strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
				strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
            strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
            strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
            strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
            strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
            strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
            strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
            strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
            strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
            strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
            strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
            db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
            db_recs.bch10.bin_type = auth_tx->bin_type;
            for( i =0; i <20; i++)
            {
               strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				   strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				   strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
            }
			strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
             strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB
            strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
            strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
            strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
            strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
            strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
            strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
            strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
            strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
            strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
            strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr);
            
            strcpy(db_recs.bch10.cash_bonus,auth_tx->TLF01_details.cash_bonus);/*** added 01-29-99 by IG ***/
            strcpy(db_recs.bch10.open_date,bch01_open_date);
            strcpy(db_recs.bch10.open_time,bch01_open_time);
             
             
            PRINT("Before insert call to BCH10....");
     		if (DB_NullCheck_Flag==1)
     		{
     			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
     		}
            if((result_code= db_insert_bch10(&db_recs.bch10,ErrorMsg))!= PTEMSG_OK)
              return(PTEMSG_INSERT_FAILED);
            PRINT("After insert into BCH10...");
            break;            
            
            
               
            case AUTH_AUTHORIZATION_RESPONSE:
                        
                break;
                
            case AUTH_REFUND_RESPONSE:
            case AUTH_OFFLINE_REFUND_RESPONSE:
               //Subtract Debit or Credit totals for REFUND                
               PRINT("\nIn AUTH_REFUND...");
               strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
               strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
			   strncpy(primary_key + BCH01_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
               memset(&db_recs.bch01,0,sizeof(BCH01));
               PRINT("\nBefore select BCH01....");
               result_code = db_select_bch01(primary_key,&db_recs.bch01,ErrorMsg);
               if(result_code == PTEMSG_OK)                 
				   {
                  PRINT("\nAfter select BCH01 for REFUND....");
					   if(auth_tx->bin_type=='D')
                  {
                  total = atol(db_recs.bch01.total_debit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01.total_debit_refund_amt, "%012d",total);
                  if(atol(db_recs.bch01.total_debit_refund_amt) != 0L)
                     cnt = atoi(db_recs.bch01.total_debit_refund_cnt) + 1;
                  sprintf(db_recs.bch01.total_debit_refund_cnt, "%04d",cnt);
                  }
                  else
                  if(auth_tx->bin_type=='C')
                  {
                  total = atol(db_recs.bch01.total_credit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
                  if(atol(db_recs.bch01.total_credit_refund_amt) != 0L)
                     cnt = atoi(db_recs.bch01.total_credit_refund_cnt) + 1;
                  sprintf(db_recs.bch01.total_credit_refund_cnt, "%04d",cnt);
                  }

                 //copy values into temporary variables
   				   strcpy(bch01_open_time,db_recs.bch01.open_time );
   				   strcpy(bch01_open_date,db_recs.bch01.open_date );

                  PRINT("\nBefore update BCH01....");
                  if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                     strcpy(db_recs.bch01.visa_merchant_id,"GES");
                  else
              	      strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
         		 if (DB_NullCheck_Flag==1)
         		 {
         			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
         		 }
					   if(result_code = db_update_bch01(&db_recs.bch01,ErrorMsg) != PTEMSG_OK)           
                     return(PTEMSG_UPDATE_FAILED);
                  PRINT("\nAfter update BCH01....");
                   
				   }
               else if(result_code == PTEMSG_NOT_FOUND)
				   {
                  //Delete BCH10 if any ACTIVE
                  memset(&db_recs.bch10, 0, sizeof(BCH10));
				      strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
			   	   strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
                  result = db_delete_bch10(&db_recs.bch10,ErrorMsg);

                  //create and Insert  BCH01
                  memcpy(&db_recs.bch01,primary_key,BCH01_PRIMARY_KEY_SIZE); //copy the primary Key
                  
                  //check if CREDIT refund or DEBIT refund
                  if(auth_tx->bin_type =='D')
                  {
                     strcpy(db_recs.bch01.total_debit_refund_amt, auth_tx->TLF01_details.total_amount);
                     if(atoi(db_recs.bch01.total_debit_refund_amt) != 0)
   						   strcpy(db_recs.bch01.total_debit_refund_cnt, "0001");
                  }
                  else if(auth_tx->bin_type == 'C')
                  {
                     strcpy(db_recs.bch01.total_credit_refund_amt, auth_tx->TLF01_details.total_amount);
                     if(atoi(db_recs.bch01.total_credit_refund_amt) != 0)
   						   strcpy(db_recs.bch01.total_credit_refund_cnt, "0001");
                  }  
                  genutil_format_date(date_time);
                  if(strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
   					   strcpy(db_recs.bch01.open_date ,auth_tx->TLF01_details.date_yyyymmdd);
                  else
                  {
                     memset(db_recs.bch01.open_date, 0, sizeof(db_recs.bch01.open_date));
   					   strncpy(db_recs.bch01.open_date ,date_time, 8);
                  }
                  if(strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
   					   strcpy(db_recs.bch01.open_time ,auth_tx->TLF01_details.time_hhmmss);
                  else
                  {
                     memset(db_recs.bch01.open_time, 0, sizeof(db_recs.bch01.open_time));
   					   strncpy(db_recs.bch01.open_time ,date_time + 8, 6);
                  }
   				   strcpy(bch01_open_time,db_recs.bch01.open_time );
   				   strcpy(bch01_open_date,db_recs.bch01.open_date );
					   strcpy(db_recs.bch01.batch_status,"A");
					   strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
                  if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                     strcpy(db_recs.bch01.visa_merchant_id,"GES");
                  else
              	      strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
    
                  //copy values into temporary variables

                  PRINT("Before Insert into BCH01");    
           		if (DB_NullCheck_Flag==1)
           		{
           			CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_INSERT);
           		}
					   result_code = db_insert_bch01(&db_recs.bch01,ErrorMsg);
                  PRINT(ErrorMsg);
                  PRINT("\nAfter Insert into BCH01");

					   if(result_code != PTEMSG_OK)
						   return(PTEMSG_INSERT_FAILED);
				   }
               else
					   return(PTEMSG_INSERT_FAILED);
                 //Insert REFUND transaction into BCH10 
                  memset(&db_recs.bch10, 0, sizeof(BCH10));
				      strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
					   strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
				      strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);

                  strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
				      strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
				      strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				      strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				      strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
                  strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
                  strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
                  strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
                  strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
                  strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
                  strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
                  strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
                  strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
                  strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
                  strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
                  db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
                  db_recs.bch10.bin_type = auth_tx->bin_type;
                  for( i =0; i <20; i++)
                  {
                     strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				         strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				         strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
                  }
				  strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
                 strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB

                  strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
                  strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
                  strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
                  strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
                  strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
                  strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
                  strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
                  strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
                  strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
                  strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr);
                  strcpy(db_recs.bch10.open_date,bch01_open_date);
                  strcpy(db_recs.bch10.open_time,bch01_open_time);
				  
                  PRINT("\nBefore Insert into BCH10 for REFUND...");
           		if (DB_NullCheck_Flag==1)
           		{
           			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
           		}
                  if((result_code= db_insert_bch10(&db_recs.bch10,ErrorMsg))!= PTEMSG_OK)
                    return(PTEMSG_INSERT_FAILED);
                  PRINT("\nAfter Insert into BCH10 for REFUND...");              
                         
               
             break;

            case AUTH_VOID_SALE_RESPONSE:
            case AUTH_VOID_REFUND_RESPONSE:
            case AUTH_VOID_CASH_ADVANCE_RESPONSE:
            case AUTH_SALE_ADJUSTMENT_RESPONSE: 
            case AUTH_REFUND_ADJUSTMENT_RESPONSE: 
               //Select original transaction in BCH10 and update
               memset(&db_recs.bch10, 0, sizeof(BCH10));
               memset(&bch10,0,sizeof(BCH10));
               strcpy(bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
					strcpy(bch10.primary_key.batch_nbr , "ACTIVE");
               strcpy(bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);

               if((result_code = db_select_active_bch10(&bch10,&db_recs.bch10,ErrorMsg)) == PTEMSG_OK)                 
               {  
                  PRINT("After select for bch10 for VOID\n");
                  strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
                  strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
                  strcpy(actual_amt, db_recs.bch10.tran_amount);
				      strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
                  strcpy(adjust_amt, db_recs.bch10.tran_amount);
				      strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				      strcpy(auth_tx->TLF01_details.auth_number,db_recs.bch10.auth_number); // Orig auth # goes back to terminal
                  strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
                  strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
                  strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
                  strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
                  strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
                  strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
                  strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
                  strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
                  strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
                  strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
                  db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
                  db_recs.bch10.bin_type = auth_tx->bin_type;
                  for( i =0; i <20; i++)
                  {
                     strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				         strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				         strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
                  }
				  strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
                  strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB     
                  strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
                  strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
                  strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
                  strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
                  strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
                  strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
                  strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
                  strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
                  strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date); 
                  strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr); 
                  
                  strcpy(db_recs.bch10.cash_bonus,auth_tx->TLF01_details.cash_bonus);/*** added 01-29-99 by IG ***/
                    
                  
                  PRINT("\nBefore update BCH10 for VOID....");
         		 if (DB_NullCheck_Flag==1)
         		 {
         			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
         		 }
                  if((result_code= db_update_bch10(&db_recs.bch10,ErrorMsg))!= PTEMSG_OK)
                  {
                      PRINT("After Update BCH10 for VOID....");
                     return(PTEMSG_UPDATE_FAILED);   
                  }
                  else
                  { 
                     PRINT("After Update BCH10 for VOID....");
                     //update totals in BCH01
                     strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
                     strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
					 strncpy(primary_key + BCH01_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
                     memset(&db_recs.bch01,0,sizeof(BCH01));
                     PRINT("Before Select BCH01....");
                     if(result_code = db_select_bch01(primary_key,&db_recs.bch01,ErrorMsg) == PTEMSG_OK)                 
                     {
                        PRINT("After Select BCH01....");                       
                        if(auth_tx->bin_type =='D')
                        {
                           total = atol(db_recs.bch01.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
                           sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
                           cnt = atoi(db_recs.bch01.total_debit_cnt ) - 1;
                           sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);                        
                        }
                        else
                        {
                          if( (auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
                               ||(auth_tx->TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE) 
                              )
                          {
                            total = atol(db_recs.bch01.total_void_credit_amt) + atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01.total_void_credit_amt, "%012d",total); 
                            cnt = atoi(db_recs.bch01.total_void_credit_cnt ) + 1;
                            sprintf(db_recs.bch01.total_void_credit_cnt, "%04d",cnt); 
                              
                            total = atol(db_recs.bch01.total_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
                            cnt = atoi(db_recs.bch01.total_credit_cnt ) - 1;
                            sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);         
                          }
                          else if(auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE)
                          {
                            total = atol(db_recs.bch01.total_void_credit_refund_amt) + atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01.total_void_credit_refund_amt, "%012d",total);                                                   
                            cnt = atoi(db_recs.bch01.total_void_credit_refund_cnt ) + 1;
                            sprintf(db_recs.bch01.total_void_credit_refund_cnt, "%04d",cnt);  
                              
                            total = atol(db_recs.bch01.total_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
                            cnt = atoi(db_recs.bch01.total_credit_refund_cnt ) - 1;
                            sprintf(db_recs.bch01.total_credit_refund_cnt, "%04d",cnt);      
                          }
                          else if(auth_tx->TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE)
                          {
                             diff = atol(actual_amt) - atol(adjust_amt);
                             total = atol(db_recs.bch01.total_credit_amt) - ( diff );
                             sprintf(db_recs.bch01.total_credit_amt, "%012d",total);                                                   
                          }
                          else if(auth_tx->TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT_RESPONSE)
                          {
                             diff = atol(actual_amt) - atol(adjust_amt);
                             total = atol(db_recs.bch01.total_credit_refund_amt) - ( diff );
                             sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);                                                   
                          }
                        }
                        
                      PRINT("\nBefore update BCH01....");
                      strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
                      if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                        strcpy(db_recs.bch01.visa_merchant_id,"GES");
                      else
              	         strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
						 if (DB_NullCheck_Flag==1)
						 {
							CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
						 }
					       if(result_code = db_update_bch01(&db_recs.bch01,ErrorMsg) != PTEMSG_OK)           
                         return(PTEMSG_UPDATE_FAILED);
                      PRINT("\nAfter update BCH01....");
                   
                     }
                     else
                     {
                        PRINT("\nBCH01 select failed for VOID...");
                      //  return(PTEMSG_SELECT_FAILED);
                     }
                  }
               }
               else
               {
                  //select BCH10 failed
                  return(PTEMSG_NOT_FOUND);
               }
               break;
  

         case AUTH_REVERSAL_RESPONSE:
                 
               //If record exists in BCH10, then Update totals in BCH01
               memset(&db_recs.bch10, 0, sizeof(BCH10));
               memset(&bch10,0,sizeof(BCH10));
               strcpy(bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
					strcpy(bch10.primary_key.batch_nbr , "ACTIVE");
               strcpy(bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
               strcpy(bch10.merchant_id,auth_tx->TLF01_details.merchant_id);

               PRINT("BEFORE select BCH10......for REVERSAL");
               if((result_code = db_select_bch10_by_stan(&bch10,&db_recs.bch10,ErrorMsg))==PTEMSG_OK)
               {  PRINT("After Select BCH10 by STAN......\n");
                  temp_tx_key = db_recs.bch10.tx_key;
                  switch(db_recs.bch10.tx_key)
                  {
                     case AUTH_SALE_RESPONSE:
                     case AUTH_REFUND_RESPONSE:
                     case AUTH_CASH_ADVANCE_RESPONSE:
                     case AUTH_OFFLINE_SALE_RESPONSE:
                     case AUTH_AUTHORIZATION_RESPONSE:
                     case AUTH_PRE_AUTHORIZATION_RESPONSE:
			         case AUTH_QUASI_CASH_RESPONSE:
                     
          
                        PRINT("Before update bch01 for Reversal.......");
                        strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
                        strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
                        memset(&db_recs.bch01,0,sizeof(BCH01));
						strncpy(primary_key + BCH01_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
                        if(result_code = db_select_bch01(primary_key,&db_recs.bch01,ErrorMsg) == PTEMSG_OK)                 
                        {
                           if (auth_tx->bin_type=='D')
                           {
                              if (temp_tx_key  == AUTH_REFUND_RESPONSE)
                              {
                                 total = atol(db_recs.bch01.total_debit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01.total_debit_refund_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01.total_debit_refund_cnt ) - 1;
                                 sprintf(db_recs.bch01.total_debit_refund_cnt, "%04d",cnt); 
                              }
                              else
                              {
                                 total = atol(db_recs.bch01.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01.total_debit_cnt ) - 1;
                                 sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt); 
                              }
                           }
                           else  
                           if (auth_tx->bin_type=='C')
                           {
                              if (temp_tx_key  == AUTH_REFUND_RESPONSE)
                              {
                                 total = atol(db_recs.bch01.total_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01.total_credit_refund_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01.total_credit_refund_cnt ) - 1;
                                 sprintf(db_recs.bch01.total_credit_refund_cnt, "%04d",cnt); 
                              }
                              else
                              {
                                 total = atol(db_recs.bch01.total_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01.total_credit_amt, "%012d",total);                                                   
                                 cnt = atoi(db_recs.bch01.total_credit_cnt ) - 1;
                                 sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt); 
                              }
                           }
                           PRINT("\nBefore update BCH01....");

                           if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                              strcpy(db_recs.bch01.visa_merchant_id,"GES");
                           else
              	               strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
							 }
					            if(result_code = db_update_bch01(&db_recs.bch01,ErrorMsg) != PTEMSG_OK)           
                              return(PTEMSG_UPDATE_FAILED);
                           PRINT("\nAfter update BCH01....");
                   
                        }
                        else
                        {
                        //return(PTEMSG_SELECT_FAILED);
                          PRINT("Select BCH01 failed...\n");
                          return(PTEMSG_UPDATE_FAILED);
                        }

                        //Delete record from BCH10
                        PRINT("Before delete BCH10...");
                        if(db_delete_bch10_by_stan(&bch10,ErrorMsg) != PTEMSG_OK)
                           return(PTEMSG_DELETE_FAILED);
                        PRINT("After delete BCH10...\n");
                        break;

                     case AUTH_VOID_SALE_RESPONSE:
                     case AUTH_VOID_REFUND_RESPONSE:
                     case AUTH_VOID_CASH_ADVANCE_RESPONSE:
                   
                        
                        // If reversal for void_sale happens, 
                        //then reset the tx_key for the transaction in BCH10 to SALE from VOID_SALE
                          
                        strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
				            strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
				            strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				            strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				            strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
                        strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
                        strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
                        strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
                        strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
                        strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
                        strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
                        strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
                        strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
                        strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
                        strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
                        db_recs.bch10.bin_type = auth_tx->bin_type;
                        for( i =0; i <20; i++)
                        {
                           strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				               strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				               strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
                        }
                        strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
                        strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
                        strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
                         strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
                        strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
                        strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
                        strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
                        strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
                        strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
                        strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr);
						strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
                        strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB

                        //If for VOID_SALE change it back to SALE etc...
                        if(db_recs.bch10.tx_key == AUTH_VOID_SALE_RESPONSE)
                           db_recs.bch10.tx_key = AUTH_SALE_RESPONSE;
                        else  if(db_recs.bch10.tx_key == AUTH_VOID_REFUND_RESPONSE)
                                 db_recs.bch10.tx_key = AUTH_REFUND_RESPONSE;
                        else  if(db_recs.bch10.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE)
                           db_recs.bch10.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
                        //update BCH10 to reflect VOID_SALE instead of SALE
                        PRINT("Before update BCH10 (stan)for Reversal.......");
						 if (DB_NullCheck_Flag==1)
						 {
							CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
						 }
                        if((result_code= db_update_bch10(&db_recs.bch10, ErrorMsg ))!= PTEMSG_OK)
                           return(PTEMSG_UPDATE_FAILED);
                        PRINT("After update BCH10....\n");

                        PRINT("Before select bch01 for Reversal.......");
                        strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
                        strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
						strncpy(primary_key + BCH01_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
                        memset(&db_recs.bch01,0,sizeof(BCH01));
                        if( (result_code = db_select_bch01(primary_key,&db_recs.bch01,ErrorMsg) )== PTEMSG_OK)                 
                        {
                           PRINT("After select bch01 for Reversal......\n");
                           if (auth_tx->bin_type=='D')
                           {
                              total = atol(db_recs.bch01.total_debit_amt) + atol(auth_tx->TLF01_details.total_amount);
                              sprintf(db_recs.bch01.total_debit_amt, "%012d",total);
                              cnt = atoi(db_recs.bch01.total_debit_cnt ) + 1;
                              sprintf(db_recs.bch01.total_debit_cnt, "%04d",cnt);
                           }
                           else  
                           if (auth_tx->bin_type=='C')
                           {
                              total = atol(db_recs.bch01.total_credit_amt) + atol(auth_tx->TLF01_details.total_amount);
                              sprintf(db_recs.bch01.total_credit_amt, "%012d",total);
                              cnt = atoi(db_recs.bch01.total_credit_cnt ) + 1;
                              sprintf(db_recs.bch01.total_credit_cnt, "%04d",cnt);

                              /* Deduct the void totals also */
                              if(auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
                              {
                                 total =  atol(db_recs.bch01.total_void_credit_amt) - atol(auth_tx->TLF01_details.total_amount)  ;
                                 sprintf(db_recs.bch01.total_void_credit_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01.total_void_credit_cnt ) - 1;
                                 sprintf(db_recs.bch01.total_void_credit_cnt, "%04d",cnt);                                                   
                              }
                              else if(auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE)
                              {
                                 total = atol(db_recs.bch01.total_void_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01.total_void_credit_refund_amt, "%012d",total); 
                                 cnt = atoi(db_recs.bch01.total_void_credit_refund_cnt ) - 1;
                                 sprintf(db_recs.bch01.total_void_credit_refund_cnt, "%04d",cnt);                                                   
                              }

                           }
                           PRINT("\nBefore update BCH01....");

                           if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                              strcpy(db_recs.bch01.visa_merchant_id,"GES");
                           else
              	               strcpy(db_recs.bch01.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
                           strcpy(db_recs.bch01.merchant_id,auth_tx->TLF01_details.merchant_id);
							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinBCH01 (&db_recs.bch01,MODE_UPDATE);
							 }

                           if(result_code = db_update_bch01(&db_recs.bch01,ErrorMsg) != PTEMSG_OK)           
                               return(PTEMSG_UPDATE_FAILED);
                           PRINT("\nAfter update BCH01....");
                   
                        }
                        else
                        //return(PTEMSG_SELECT_FAILED);
                        PRINT("Select BCH01 failed...\n");
    
                       break;
                     default:
                        PRINT("Unknown Data type for Reversal..\n");
                        return( PTEMSG_INVALID_DATATYPE);
                        break;

                  }//tx_key in BCH10
               }//end_BCH10 exists
 
              break;
         default:
            PRINT("\ndefault case for processing BCH recs ..\n");
            result_code = PTEMSG_INVALID_DATATYPE;
            sprintf (ErrorMsg, "Unknown type %c: for ALL_DATA request from TRANDS",auth_tx->TLF01_details.tx_key);
            break;

           } 
           
        }
     return (PTEMSG_OK);

}

/*************************************************************************************/



/*************************************************************************************/
/****************************added *************************************/

BYTE process_alldata_mp_msg( pPTE_MSG p_msg_in )
{
	BYTE           app_data_type, result, temp_tx_key;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pAUTH_TX		   auth_tx;
   BCH10          bch10;
   CHAR           primary_key[50] = "";
   CHAR           transaction_id [21] = "";
   CHAR           date_time[16] = "";
   CHAR           actual_amt[13];
   CHAR           adjust_amt[13];
   
   long           total, diff ;
   CHAR           bch01_mp_open_date[9]="";
   CHAR           bch01_mp_open_time[7]="";
   int            cnt;
   int            i;
   

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX); 
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   auth_tx = (pAUTH_TX)p_data;
   
   if (strcmp(auth_tx->TLF01_details.response_code ,"00") == 0)
   {   
      
        
      
      
      switch(auth_tx->TLF01_details.tx_key)
       {
         case AUTH_SALE_RESPONSE:
         case AUTH_CASH_ADVANCE_RESPONSE:
         case AUTH_SALES_COMPLETION_RESPONSE:
         case AUTH_OFFLINE_SALE_RESPONSE:
         case AUTH_QUASI_CASH_RESPONSE:
            PRINT("In SALE_RESPONSE \n");

            
            //BCH01_MP
            strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
            strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id), "ACTIVE");
			strncpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
           
            PRINT("Before BCH01_MP");
            //result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,ErrorMsg);
            result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,ErrorMsg);
            //printf("Result code for BCH01_MP: %d",result_code);
            
            //create and Insert into BCH01_MP, delete any ACTIVE trans. for this terminal 
            if(result_code == PTEMSG_NOT_FOUND)
				{
               //Delete BCH10 if any ACTIVE
               memset(&db_recs.bch10, 0, sizeof(BCH10));
				   strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
			   	strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
               result = db_delete_active_bch10(&db_recs.bch10,ErrorMsg);
               
               memcpy(&db_recs.bch01_mp,primary_key,BCH01_MP_PRIMARY_KEY_SIZE);

               //check if CREDIT sale or DEBIT sale
               if(auth_tx->bin_type =='D')
               {
                  strcpy(db_recs.bch01_mp.total_debit_amt, auth_tx->TLF01_details.total_amount);
                  if(atoi(db_recs.bch01_mp.total_debit_amt) != 0)
   						strcpy(db_recs.bch01_mp.total_debit_cnt, "0001");
               }
               else if(auth_tx->bin_type == 'C')
               {
                  strcpy(db_recs.bch01_mp.total_credit_amt, auth_tx->TLF01_details.total_amount);
                  if(atoi(db_recs.bch01_mp.total_credit_amt) != 0)
   						strcpy(db_recs.bch01_mp.total_credit_cnt, "0001");
               }
               genutil_format_date(date_time);
               if(strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
   					strcpy(db_recs.bch01_mp.open_date ,auth_tx->TLF01_details.date_yyyymmdd);
               else
               {
                  memset(db_recs.bch01_mp.open_date, 0, sizeof(db_recs.bch01_mp.open_date));
   					strncpy(db_recs.bch01_mp.open_date ,date_time, 8);
               }
               if(strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
   					strcpy(db_recs.bch01_mp.open_time ,auth_tx->TLF01_details.time_hhmmss);
               else
               {
                  memset(db_recs.bch01_mp.open_time, 0, sizeof(db_recs.bch01_mp.open_time));
   					strncpy(db_recs.bch01_mp.open_time ,date_time + 8, 6);
               }
   				strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time );
   				strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date );
					strcpy(db_recs.bch01_mp.batch_status,"A");
					strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
               if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                  strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
               else
              	   strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);

               //copy values into temporary variables
    
               PRINT("Before Insert into BCH01_MP");
         		if (DB_NullCheck_Flag==1)
         		{
         			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_INSERT);
         		}
					result_code = db_insert_bch01_mp(&db_recs.bch01_mp,ErrorMsg);
               PRINT(ErrorMsg);
               PRINT("\nAfter Insert into BCH01_MP");

					if(result_code != PTEMSG_OK)
						return(PTEMSG_INSERT_FAILED);
				}
				else if(result_code == PTEMSG_OK)
				{
					if(auth_tx->bin_type=='D')
               {
                  total = atol(db_recs.bch01_mp.total_debit_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
                  if(atol(db_recs.bch01_mp.total_debit_amt) != 0L)
                     cnt = atoi(db_recs.bch01_mp.total_debit_cnt) + 1;
                  sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt);
               }
               else
                if(auth_tx->bin_type=='C')
               {
                  total=atol(db_recs.bch01_mp.total_credit_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);
                  if(atol(db_recs.bch01_mp.total_credit_amt) != 0L)
                     cnt = atoi(db_recs.bch01_mp.total_credit_cnt) + 1;
                  sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt);
               }

               //copy values into temporary variables
   				strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time );
   				strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date );

               PRINT("Before update for BCH01_MP");
				 if (DB_NullCheck_Flag==1)
				 {
					CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
				 }
					if(result_code = db_update_bch01_mp(&db_recs.bch01_mp,ErrorMsg) != PTEMSG_OK)
                         return(PTEMSG_UPDATE_FAILED);
               PRINT("After update for BCH01_MP");

				}
            else
               return(PTEMSG_UPDATE_FAILED);
				  //return(p_msg_out);
              //PRINT("ERROR in BCH01_MP update/insert");  
            memset(&db_recs.bch10, 0, sizeof(BCH10));
   			//Insert into BCH10
				strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
				strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
				strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);

            strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
				strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
				strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
            strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
            strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
            strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
            strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
            strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
            strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
            strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
            strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
            strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
            strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
            db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
            db_recs.bch10.bin_type = auth_tx->bin_type;
            for( i =0; i <20; i++)
            {
               strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				   strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				   strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
            }
			strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
             strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB
            strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
            strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
            strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
            strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
            strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
            strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
            strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
            strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
            strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
            strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr);
            
            strcpy(db_recs.bch10.cash_bonus,auth_tx->TLF01_details.cash_bonus);/*** added 01-29-99 by IG ***/
            strcpy(db_recs.bch10.open_date,bch01_mp_open_date);
            strcpy(db_recs.bch10.open_time,bch01_mp_open_time);
             
            PRINT("Before insert call to BCH10....");
      		if (DB_NullCheck_Flag==1)
      		{
      			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
      		}
            if((result_code= db_insert_bch10(&db_recs.bch10,ErrorMsg))!= PTEMSG_OK)
              return(PTEMSG_INSERT_FAILED);
            PRINT("After insert into BCH10...");
            break;            
            
            
               
            case AUTH_AUTHORIZATION_RESPONSE:
                        
                break;
                
            case AUTH_REFUND_RESPONSE:
            case AUTH_OFFLINE_REFUND_RESPONSE:
               //Subtract Debit or Credit totals for REFUND                
               PRINT("\nIn AUTH_REFUND...");
               strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
               strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
			   strncpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
               memset(&db_recs.bch01_mp,0,sizeof(BCH01_MP));
               PRINT("\nBefore select BCH01_MP....");
               result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,ErrorMsg);
               if(result_code == PTEMSG_OK)                 
				   {
                  PRINT("\nAfter select BCH01_MP for REFUND....");
					   if(auth_tx->bin_type=='D')
                  {
                  total = atol(db_recs.bch01_mp.total_debit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01_mp.total_debit_refund_amt, "%012d",total);
                  if(atol(db_recs.bch01_mp.total_debit_refund_amt) != 0L)
                     cnt = atoi(db_recs.bch01_mp.total_debit_refund_cnt) + 1;
                  sprintf(db_recs.bch01_mp.total_debit_refund_cnt, "%04d",cnt);
                  }
                  else
                  if(auth_tx->bin_type=='C')
                  {
                  total = atol(db_recs.bch01_mp.total_credit_refund_amt)+atol(auth_tx->TLF01_details.total_amount);
                  sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
                  if(atol(db_recs.bch01_mp.total_credit_refund_amt) != 0L)
                     cnt = atoi(db_recs.bch01_mp.total_credit_refund_cnt) + 1;
                  sprintf(db_recs.bch01_mp.total_credit_refund_cnt, "%04d",cnt);
                  }

                 //copy values into temporary variables
   				   strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time );
   				   strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date );

                  PRINT("\nBefore update BCH01_MP....");
                  if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                     strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
                  else
              	      strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
					 if (DB_NullCheck_Flag==1)
					 {
						CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
					 }

					   if(result_code = db_update_bch01_mp(&db_recs.bch01_mp,ErrorMsg) != PTEMSG_OK)           
                     return(PTEMSG_UPDATE_FAILED);
                  PRINT("\nAfter update BCH01_MP....");
                   
				   }
               else if(result_code == PTEMSG_NOT_FOUND)
				   {
                  //Delete BCH10 if any ACTIVE
                  memset(&db_recs.bch10, 0, sizeof(BCH10));
				      strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
			   	   strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
                  result = db_delete_bch10(&db_recs.bch10,ErrorMsg);

                  //create and Insert  BCH01_MP
                  memcpy(&db_recs.bch01_mp,primary_key,BCH01_MP_PRIMARY_KEY_SIZE); //copy the primary Key
                  
                  //check if CREDIT refund or DEBIT refund
                  if(auth_tx->bin_type =='D')
                  {
                     strcpy(db_recs.bch01_mp.total_debit_refund_amt, auth_tx->TLF01_details.total_amount);
                     if(atoi(db_recs.bch01_mp.total_debit_refund_amt) != 0)
   						   strcpy(db_recs.bch01_mp.total_debit_refund_cnt, "0001");
                  }
                  else if(auth_tx->bin_type == 'C')
                  {
                     strcpy(db_recs.bch01_mp.total_credit_refund_amt, auth_tx->TLF01_details.total_amount);
                     if(atoi(db_recs.bch01_mp.total_credit_refund_amt) != 0)
   						   strcpy(db_recs.bch01_mp.total_credit_refund_cnt, "0001");
                  }  
                  genutil_format_date(date_time);
                  if(strlen(auth_tx->TLF01_details.date_yyyymmdd) != 0)
   					   strcpy(db_recs.bch01_mp.open_date ,auth_tx->TLF01_details.date_yyyymmdd);
                  else
                  {
                     memset(db_recs.bch01_mp.open_date, 0, sizeof(db_recs.bch01_mp.open_date));
   					   strncpy(db_recs.bch01_mp.open_date ,date_time, 8);
                  }
                  if(strlen(auth_tx->TLF01_details.time_hhmmss) != 0)
   					   strcpy(db_recs.bch01_mp.open_time ,auth_tx->TLF01_details.time_hhmmss);
                  else
                  {
                     memset(db_recs.bch01_mp.open_time, 0, sizeof(db_recs.bch01_mp.open_time));
   					   strncpy(db_recs.bch01_mp.open_time ,date_time + 8, 6);
                  }
   				   strcpy(bch01_mp_open_time,db_recs.bch01_mp.open_time );
   				   strcpy(bch01_mp_open_date,db_recs.bch01_mp.open_date );
					   strcpy(db_recs.bch01_mp.batch_status,"A");
					   strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
                  if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                     strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
                  else
              	      strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
    
                  //copy values into temporary variables

                  PRINT("Before Insert into BCH01_MP");    
           		 if (DB_NullCheck_Flag==1)
           		 {
           			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_INSERT);
           		 }
					   result_code = db_insert_bch01_mp(&db_recs.bch01_mp,ErrorMsg);
                  PRINT(ErrorMsg);
                  PRINT("\nAfter Insert into BCH01_MP");

					   if(result_code != PTEMSG_OK)
						   return(PTEMSG_INSERT_FAILED);
				   }
               else
					   return(PTEMSG_INSERT_FAILED);
                 //Insert REFUND transaction into BCH10 
                  memset(&db_recs.bch10, 0, sizeof(BCH10));
				      strcpy(db_recs.bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
					   strcpy(db_recs.bch10.primary_key.batch_nbr , "ACTIVE");
				      strcpy(db_recs.bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.retrieval_ref_num);

                  strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
				      strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
				      strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				      strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				      strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
                  strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
                  strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
                  strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
                  strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
                  strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
                  strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
                  strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
                  strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
                  strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
                  strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
                  db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
                  db_recs.bch10.bin_type = auth_tx->bin_type;
                  for( i =0; i <20; i++)
                  {
                     strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				         strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				         strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
                  }
				  strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
                 strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB

                  strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
                  strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
                  strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
                  strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
                  strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
                  strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
                  strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
                  strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
                  strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
                  strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr);
                  strcpy(db_recs.bch10.open_date,bch01_mp_open_date);
                  strcpy(db_recs.bch10.open_time,bch01_mp_open_time);
				  
                  PRINT("\nBefore Insert into BCH10 for REFUND...");
          		 if (DB_NullCheck_Flag==1)
          		 {
          			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_INSERT);
          		 }
                  if((result_code= db_insert_bch10(&db_recs.bch10,ErrorMsg))!= PTEMSG_OK)
                    return(PTEMSG_INSERT_FAILED);
                  PRINT("\nAfter Insert into BCH10 for REFUND...");              
                         
               
             break;

            case AUTH_VOID_SALE_RESPONSE:
            case AUTH_VOID_REFUND_RESPONSE:
            case AUTH_VOID_CASH_ADVANCE_RESPONSE:
            case AUTH_SALE_ADJUSTMENT_RESPONSE: 
            case AUTH_REFUND_ADJUSTMENT_RESPONSE: 
               //Select original transaction in BCH10 and update
               memset(&db_recs.bch10, 0, sizeof(BCH10));
               memset(&bch10,0,sizeof(BCH10));
               strcpy(bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
					strcpy(bch10.primary_key.batch_nbr , "ACTIVE");
               strcpy(bch10.primary_key.retrieval_ref_num,auth_tx->TLF01_details.orig_retrieval_ref_num);

               if((result_code = db_select_active_bch10(&bch10,&db_recs.bch10,ErrorMsg)) == PTEMSG_OK)                 
               {  
                  PRINT("After select for bch10 for VOID\n");
                  strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
                  strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
                  strcpy(actual_amt, db_recs.bch10.tran_amount);
				      strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
                  strcpy(adjust_amt, db_recs.bch10.tran_amount);
				      strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				      strcpy(auth_tx->TLF01_details.auth_number,db_recs.bch10.auth_number); // Orig auth # goes back to terminal
                  strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
                  strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
                  strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
                  strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
                  strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
                  strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
                  strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
                  strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
                  strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
                  strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
                  db_recs.bch10.tx_key = auth_tx->TLF01_details.tx_key;
                  db_recs.bch10.bin_type = auth_tx->bin_type;
                  for( i =0; i <20; i++)
                  {
                     strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				         strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				         strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
                  }
				  strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
                  strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB     
                  strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
                  strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
                  strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
                  strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
                  strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
                  strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
                  strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
                  strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
                  strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date); 
                  strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr); 
                  
                  strcpy(db_recs.bch10.cash_bonus,auth_tx->TLF01_details.cash_bonus);/*** added 01-29-99 by IG ***/
                    
                  
                  PRINT("\nBefore update BCH10 for VOID....");
         		 if (DB_NullCheck_Flag==1)
         		 {
         			CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
         		 }
                  if((result_code= db_update_bch10(&db_recs.bch10,ErrorMsg))!= PTEMSG_OK)
                  {
                      PRINT("After Update BCH10 for VOID....");
                     return(PTEMSG_UPDATE_FAILED);   
                  }
                  else
                  { 
                     PRINT("After Update BCH10 for VOID....");
                     //update totals in BCH01_MP
                     strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
                     strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
					 strncpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
                     memset(&db_recs.bch01_mp,0,sizeof(BCH01_MP));
                     PRINT("Before Select BCH01_MP....");
                     if(result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,ErrorMsg) == PTEMSG_OK)                 
                     {
                        PRINT("After Select BCH01_MP....");                       
                        if(auth_tx->bin_type =='D')
                        {
                           total = atol(db_recs.bch01_mp.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
                           sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
                           cnt = atoi(db_recs.bch01_mp.total_debit_cnt ) - 1;
                           sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt);                        
                        }
                        else
                        {
                          if( (auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
                               ||(auth_tx->TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE) 
                              )
                          {
                            total = atol(db_recs.bch01_mp.total_void_credit_amt) + atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01_mp.total_void_credit_amt, "%012d",total); 
                            cnt = atoi(db_recs.bch01_mp.total_void_credit_cnt ) + 1;
                            sprintf(db_recs.bch01_mp.total_void_credit_cnt, "%04d",cnt); 
                              
                            total = atol(db_recs.bch01_mp.total_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);
                            cnt = atoi(db_recs.bch01_mp.total_credit_cnt ) - 1;
                            sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt);         
                          }
                          else if(auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE)
                          {
                            total = atol(db_recs.bch01_mp.total_void_credit_refund_amt) + atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01_mp.total_void_credit_refund_amt, "%012d",total);                                                   
                            cnt = atoi(db_recs.bch01_mp.total_void_credit_refund_cnt ) + 1;
                            sprintf(db_recs.bch01_mp.total_void_credit_refund_cnt, "%04d",cnt);  
                              
                            total = atol(db_recs.bch01_mp.total_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                            sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
                            cnt = atoi(db_recs.bch01_mp.total_credit_refund_cnt ) - 1;
                            sprintf(db_recs.bch01_mp.total_credit_refund_cnt, "%04d",cnt);      
                          }
                          else if(auth_tx->TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE)
                          {
                             diff = atol(actual_amt) - atol(adjust_amt);
                             total = atol(db_recs.bch01_mp.total_credit_amt) - ( diff );
                             sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);                                                   
                          }
                          else if(auth_tx->TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT_RESPONSE)
                          {
                             diff = atol(actual_amt) - atol(adjust_amt);
                             total = atol(db_recs.bch01_mp.total_credit_refund_amt) - ( diff );
                             sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);                                                   
                          }
                        }
                        
                      PRINT("\nBefore update BCH01_MP....");
                      strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
                      if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                        strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
                      else
              	         strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
             		 if (DB_NullCheck_Flag==1)
             		 {
             			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
             		 }
					       if(result_code = db_update_bch01_mp(&db_recs.bch01_mp,ErrorMsg) != PTEMSG_OK)           
                         return(PTEMSG_UPDATE_FAILED);
                      PRINT("\nAfter update BCH01_MP....");
                   
                     }
                     else
                     {
                        PRINT("\nBCH01_MP select failed for VOID...");
                      //  return(PTEMSG_SELECT_FAILED);
                     }
                  }
               }
               else
               {
                  //select BCH10 failed
                  return(PTEMSG_NOT_FOUND);
               }
               break;
  

         case AUTH_REVERSAL_RESPONSE:
                 
               //If record exists in BCH10, then Update totals in BCH01_MP
               memset(&db_recs.bch10, 0, sizeof(BCH10));
               memset(&bch10,0,sizeof(BCH10));
               strcpy(bch10.primary_key.device_id,auth_tx->TLF01_details.terminal_id);
					strcpy(bch10.primary_key.batch_nbr , "ACTIVE");
               strcpy(bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
               strcpy(bch10.merchant_id,auth_tx->TLF01_details.merchant_id);

               PRINT("BEFORE select BCH10......for REVERSAL");
               if((result_code = db_select_bch10_by_stan(&bch10,&db_recs.bch10,ErrorMsg))==PTEMSG_OK)
               {  PRINT("After Select BCH10 by STAN......\n");
                  temp_tx_key = db_recs.bch10.tx_key;
                  switch(db_recs.bch10.tx_key)
                  {
                     case AUTH_SALE_RESPONSE:
                     case AUTH_REFUND_RESPONSE:
                     case AUTH_CASH_ADVANCE_RESPONSE:
                     case AUTH_OFFLINE_SALE_RESPONSE:
                     case AUTH_AUTHORIZATION_RESPONSE:
                     case AUTH_PRE_AUTHORIZATION_RESPONSE:
			         case AUTH_QUASI_CASH_RESPONSE:
                     
          
                        PRINT("Before update bch01_mp for Reversal.......");
                        strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
                        strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
						strncpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
                        memset(&db_recs.bch01_mp,0,sizeof(BCH01_MP));
                        if(result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,ErrorMsg) == PTEMSG_OK)                 
                        {
                           if (auth_tx->bin_type=='D')
                           {
                              if (temp_tx_key  == AUTH_REFUND_RESPONSE)
                              {
                                 total = atol(db_recs.bch01_mp.total_debit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01_mp.total_debit_refund_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01_mp.total_debit_refund_cnt ) - 1;
                                 sprintf(db_recs.bch01_mp.total_debit_refund_cnt, "%04d",cnt); 
                              }
                              else
                              {
                                 total = atol(db_recs.bch01_mp.total_debit_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01_mp.total_debit_cnt ) - 1;
                                 sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt); 
                              }
                           }
                           else  
                           if (auth_tx->bin_type=='C')
                           {
                              if (temp_tx_key  == AUTH_REFUND_RESPONSE)
                              {
                                 total = atol(db_recs.bch01_mp.total_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01_mp.total_credit_refund_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01_mp.total_credit_refund_cnt ) - 1;
                                 sprintf(db_recs.bch01_mp.total_credit_refund_cnt, "%04d",cnt); 
                              }
                              else
                              {
                                 total = atol(db_recs.bch01_mp.total_credit_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);                                                   
                                 cnt = atoi(db_recs.bch01_mp.total_credit_cnt ) - 1;
                                 sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt); 
                              }
                           }
                           PRINT("\nBefore update BCH01_MP....");

                           if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                              strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
                           else
              	               strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
                  		 if (DB_NullCheck_Flag==1)
                  		 {
                  			CheckNULLTerminatedinBCH01_MP (&db_recs.bch01_mp,MODE_UPDATE);
                  		 }
					            if(result_code = db_update_bch01_mp(&db_recs.bch01_mp,ErrorMsg) != PTEMSG_OK)           
                              return(PTEMSG_UPDATE_FAILED);
                           PRINT("\nAfter update BCH01_MP....");
                   
                        }
                        else
                        {
                        //return(PTEMSG_SELECT_FAILED);
                          PRINT("Select BCH01_MP failed...\n");
                          return(PTEMSG_UPDATE_FAILED);
                        }

                        //Delete record from BCH10
                        PRINT("Before delete BCH10...");
                        if(db_delete_bch10_by_stan(&bch10,ErrorMsg) != PTEMSG_OK)
                           return(PTEMSG_DELETE_FAILED);
                        PRINT("After delete BCH10...\n");
                        break;

                     case AUTH_VOID_SALE_RESPONSE:
                     case AUTH_VOID_REFUND_RESPONSE:
                     case AUTH_VOID_CASH_ADVANCE_RESPONSE:
                   
                        
                        // If reversal for void_sale happens, 
                        //then reset the tx_key for the transaction in BCH10 to SALE from VOID_SALE
                          
                        strcpy(db_recs.bch10.merchant_id,auth_tx->TLF01_details.merchant_id);
				            strcpy(db_recs.bch10.processing_code,auth_tx->TLF01_details.processing_code);
				            strcpy(db_recs.bch10.tran_amount,auth_tx->TLF01_details.total_amount);
				            strcpy(db_recs.bch10.response_code,auth_tx->TLF01_details.response_code);
				            strcpy(db_recs.bch10.auth_number,auth_tx->TLF01_details.auth_number);
                        strcpy(db_recs.bch10.tran_date,auth_tx->TLF01_details.date_yyyymmdd);
                        strcpy(db_recs.bch10.tran_time,auth_tx->TLF01_details.time_hhmmss);
                        strcpy(db_recs.bch10.transaction_id,auth_tx->TLF01_details.primary_key.transaction_id);
                        strcpy(db_recs.bch10.message_type,auth_tx->TLF01_details.message_type);
                        strcpy(db_recs.bch10.card_nbr,auth_tx->TLF01_details.card_num);
                        strcpy(db_recs.bch10.invoice_nbr,auth_tx->TLF01_details.invoice_number);
                        strcpy(db_recs.bch10.settle_file_prefix,auth_tx->BIN01_details.settle_file_prefix);
                        strcpy(db_recs.bch10.product_code,auth_tx->TLF01_details.product_code);
                        strcpy(db_recs.bch10.odometer,auth_tx->TLF01_details.odometer);
                        strcpy(db_recs.bch10.sys_trace_audit_num,auth_tx->TLF01_details.sys_trace_audit_num);
                        db_recs.bch10.bin_type = auth_tx->bin_type;
                        for( i =0; i <20; i++)
                        {
                           strcpy(db_recs.bch10.product_codes[i].code,auth_tx->TLF01_details.product_codes[i].code);
				               strcpy(db_recs.bch10.product_codes[i].quantity,auth_tx->TLF01_details.product_codes[i].quantity);
				               strcpy(db_recs.bch10.product_codes[i].amount,auth_tx->TLF01_details.product_codes[i].amount);
                        }
                        strcpy(db_recs.bch10.down_payment,auth_tx->TLF01_details.down_payment);
                        strcpy(db_recs.bch10.period,auth_tx->TLF01_details.period);
                        strcpy(db_recs.bch10.interest_rate,auth_tx->TLF01_details.interest_rate);
                         strcpy(db_recs.bch10.total_interest,auth_tx->TLF01_details.total_interest);
                        strcpy(db_recs.bch10.redemption_amount,auth_tx->TLF01_details.redemption_amount);
                        strcpy(db_recs.bch10.vehicle_number,auth_tx->TLF01_details.vehicle_number);
                        strcpy(db_recs.bch10.pos_entry_mode,auth_tx->TLF01_details.pos_entry_mode);
                        strcpy(db_recs.bch10.profile,auth_tx->TLF01_details.profile);
                        strcpy(db_recs.bch10.exp_date,auth_tx->TLF01_details.exp_date);
                        strcpy(db_recs.bch10.ticket_nbr,auth_tx->TLF01_details.ticket_nbr);
						strcpy(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.pos_condition_code );  //Girija GB
                        strcat(db_recs.bch10.product_codes[6].amount,auth_tx->TLF01_details.service_code );     //Girija GB

                        //If for VOID_SALE change it back to SALE etc...
                        if(db_recs.bch10.tx_key == AUTH_VOID_SALE_RESPONSE)
                           db_recs.bch10.tx_key = AUTH_SALE_RESPONSE;
                        else  if(db_recs.bch10.tx_key == AUTH_VOID_REFUND_RESPONSE)
                                 db_recs.bch10.tx_key = AUTH_REFUND_RESPONSE;
                        else  if(db_recs.bch10.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE)
                           db_recs.bch10.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
                        //update BCH10 to reflect VOID_SALE instead of SALE
                        PRINT("Before update BCH10 (stan)for Reversal.......");
						 if (DB_NullCheck_Flag==1)
						 {
							CheckNULLTerminatedinBCH10 (&db_recs.bch10,MODE_UPDATE);
						 }
                        if((result_code= db_update_bch10(&db_recs.bch10, ErrorMsg ))!= PTEMSG_OK)
                           return(PTEMSG_UPDATE_FAILED);
                        PRINT("After update BCH10....\n");

                        PRINT("Before select bch01_mp for Reversal.......");
                        strcpy(primary_key,auth_tx->TLF01_details.terminal_id);
                        strcpy(primary_key + sizeof(db_recs.tlf01.terminal_id),"ACTIVE");
						strncpy(primary_key + BCH01_MP_MERCHANT_ID_OFFSET,   auth_tx->TLF01_details.merchant_id, 15 );
                        memset(&db_recs.bch01_mp,0,sizeof(BCH01_MP));
                        if( (result_code = db_select_bch01_mp(primary_key,&db_recs.bch01_mp,ErrorMsg) )== PTEMSG_OK)                 
                        {
                           PRINT("After select bch01_mp for Reversal......\n");
                           if (auth_tx->bin_type=='D')
                           {
                              total = atol(db_recs.bch01_mp.total_debit_amt) + atol(auth_tx->TLF01_details.total_amount);
                              sprintf(db_recs.bch01_mp.total_debit_amt, "%012d",total);
                              cnt = atoi(db_recs.bch01_mp.total_debit_cnt ) + 1;
                              sprintf(db_recs.bch01_mp.total_debit_cnt, "%04d",cnt);
                           }
                           else  
                           if (auth_tx->bin_type=='C')
                           {
                              total = atol(db_recs.bch01_mp.total_credit_amt) + atol(auth_tx->TLF01_details.total_amount);
                              sprintf(db_recs.bch01_mp.total_credit_amt, "%012d",total);
                              cnt = atoi(db_recs.bch01_mp.total_credit_cnt ) + 1;
                              sprintf(db_recs.bch01_mp.total_credit_cnt, "%04d",cnt);

                              /* Deduct the void totals also */
                              if(auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
                              {
                                 total =  atol(db_recs.bch01_mp.total_void_credit_amt) - atol(auth_tx->TLF01_details.total_amount)  ;
                                 sprintf(db_recs.bch01_mp.total_void_credit_amt, "%012d",total);
                                 cnt = atoi(db_recs.bch01_mp.total_void_credit_cnt ) - 1;
                                 sprintf(db_recs.bch01_mp.total_void_credit_cnt, "%04d",cnt);                                                   
                              }
                              else if(auth_tx->TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE)
                              {
                                 total = atol(db_recs.bch01_mp.total_void_credit_refund_amt) - atol(auth_tx->TLF01_details.total_amount);
                                 sprintf(db_recs.bch01_mp.total_void_credit_refund_amt, "%012d",total); 
                                 cnt = atoi(db_recs.bch01_mp.total_void_credit_refund_cnt ) - 1;
                                 sprintf(db_recs.bch01_mp.total_void_credit_refund_cnt, "%04d",cnt);                                                   
                              }

                           }
                           PRINT("\nBefore update BCH01_MP....");

                           if( strcmp(auth_tx->TLF01_details.organization_id, "GES") == 0 )
                              strcpy(db_recs.bch01_mp.visa_merchant_id,"GES");
                           else
              	               strcpy(db_recs.bch01_mp.visa_merchant_id,auth_tx->TLF01_details.visa_merchant_id);
                           strcpy(db_recs.bch01_mp.merchant_id,auth_tx->TLF01_details.merchant_id);
							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinBCH01 (&db_recs.bch01_mp,MODE_UPDATE);
							 }
                           if(result_code = db_update_bch01(&db_recs.bch01_mp,ErrorMsg) != PTEMSG_OK)           
                               return(PTEMSG_UPDATE_FAILED);
                           PRINT("\nAfter update BCH01_MP....");
                   
                        }
                        else
                        //return(PTEMSG_SELECT_FAILED);
                        PRINT("Select BCH01_MP failed...\n");
    
                       break;
                     default:
                        PRINT("Unknown Data type for Reversal..\n");
                        return( PTEMSG_INVALID_DATATYPE);
                        break;

                  }//tx_key in BCH10
               }//end_BCH10 exists
 
              break;
         default:
            PRINT("\ndefault case for processing BCH recs ..\n");
            result_code = PTEMSG_INVALID_DATATYPE;
            sprintf (ErrorMsg, "Unknown type %c: for ALL_DATA request from TRANDS",auth_tx->TLF01_details.tx_key);
            break;

           } 
           
        }
     return (PTEMSG_OK);

}

/*************************************************************************************/



/*************************************************************************************/
pPTE_MSG process_get_gui_list_msg( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) );
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data		 = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case TLF01_DATA:

         PRINT( "request to retrieve tlf01 gui list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_tlf01_gui_list_dynamic( ( pTLF01_LIMITS )p_data, &db_lists.Tlf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Tlf01_Gui_List, sizeof( db_lists.Tlf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Tlf01_Gui_List),
                                          TLF01_DATA);
         }
         break;
      case BCH01_DATA:

         PRINT( "request to retrieve bch01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bch01_gui_list( ( pBCH01 )p_data, &db_lists.Bch01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch01_Gui_List, sizeof( db_lists.Bch01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch01_Gui_List),
                                          BCH01_DATA);
         }
         break;

	  case BCH01_MP_DATA:

         PRINT( "request to retrieve bch01_mp list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bch01_mp_gui_list( ( pBCH01_MP )p_data, &db_lists.Bch01_Mp_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch01_Mp_Gui_List, sizeof( db_lists.Bch01_Mp_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch01_Mp_Gui_List),
                                          BCH01_MP_DATA);
         }
         break;

      case NBF01_DATA:      
         
               PRINT( "request to retrieve nbf01 gui list\n" );
               // copy the last ucf02.group_id retrieved into the structure 
               if( ( result_code = db_get_nbf01_gui_list( ( pNBF01 )p_data, &db_lists.Nbf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Nbf01_Gui_List, sizeof( db_lists.Nbf01_Gui_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Nbf01_Gui_List),
                                          NBF01_DATA);
               }
         break;

      case NGF01_DATA:

          if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_NGF01_LIST_BY_BINRANGE)
		   {
            PRINT( "request to retrieve ngf01 list by binrange\n" );
            // copy the last ucf02.group_id retrieved into the structure 
            if( ( result_code = db_get_ngf01_list_by_binrange( ( pNGF01 )p_data, &db_lists.Ngf01_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Ngf01_List, sizeof( db_lists.Ngf01_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Ngf01_List),
                                          NGF01_DATA);
            }
         }
         else
         {
            PRINT( "request to retrieve ngf01 list\n" );
            // copy the last ucf02.group_id retrieved into the structure 
            if( ( result_code = db_get_ngf01_gui_list( ( pCHAR )p_data, &db_lists.Ngf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Ngf01_Gui_List, sizeof( db_lists.Ngf01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Ngf01_Gui_List),
                                          NGF01_DATA);
            }
          }

         break;


      case BCH10_DATA:

         PRINT( "request to retrieve bch10 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bch10_gui_list( ( pBCH10 )p_data, &db_lists.Bch10_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch10_Gui_List, sizeof( db_lists.Bch10_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch10_Gui_List),
                                          BCH10_DATA);
         }
         break;

     
      case BCH11_DATA:

         PRINT( "request to retrieve bch11 gui list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bch11_gui_list( ( pBCH11 )p_data, &db_lists.Bch11_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch11_Gui_List, sizeof( db_lists.Bch11_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch11_Gui_List),
                                          BCH11_DATA);
         }
         break;

      case DCF01_DATA:

         PRINT( "request to retrieve dfc01 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_dcf01_gui_list( ( pCHAR )p_data, &db_lists.Dcf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Dcf01_Gui_List, sizeof( db_lists.Dcf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Dcf01_Gui_List),
                                          DCF01_DATA);
         }
         break;

      case MCF01_DATA:

         if(ptemsg_get_msg_subtype2 (p_msg_in) != ST2_DB_SELECT_BY_ORG_ID)
         {
            PRINT( "request to retrieve mfc01 list\n" );
            // copy the last caf01.name retrieved into the structure 
            if( ( result_code = db_get_mcf01_gui_list( ( pCHAR )p_data, 0, &db_lists.Mcf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Mcf01_Gui_List, sizeof( db_lists.Mcf01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_lists.Mcf01_Gui_List),
                                             MCF01_DATA);
            }
         }
         else
         {
            PRINT( "request to retrieve mfc01 list\n" );
            // copy the last caf01.name retrieved into the structure 
            if( ( result_code = db_get_mcf01_gui_list( ( pCHAR )p_data, 1, &db_lists.Mcf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Mcf01_Gui_List, sizeof( db_lists.Mcf01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             ptemsg_get_msg_subtype2 (p_msg_in),
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_lists.Mcf01_Gui_List),
                                             MCF01_DATA);
            }
         }
         break;

	case MGF01_DATA:

		if ( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
        {
	         PRINT( "request to retrieve MGF01 gui list\n" );
	         // copy the last ptf01.name retrieved into the structure 
	         if( ( result_code = db_get_mgf01_list( &db_lists.Mgf01_Gui_Droplist, ErrorMsg ) ) == PTEMSG_OK )
	         {
	            memcpy(p_current, (BYTE *)&db_lists.Mgf01_Gui_Droplist, sizeof( db_lists.Mgf01_Gui_Droplist ));
	            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	                                          ptemsg_get_msg_subtype1 (p_msg_in),
	                                          ptemsg_get_msg_subtype2 (p_msg_in),
	                                          reply_que,
	                                          application_que_name,
	                                          (pBYTE)AtpBuffer,
	                                          sizeof(AUTH_TX) + sizeof(db_lists.Mgf01_Gui_Droplist),
	                                          MGF01_DATA);
	         }
       }
	   else
	   {
	        PRINT( "request to retrieve mgf01 list\n" );

	        if( ( result_code = db_get_mgf01_gui_list( ( pCHAR )p_data, 1, &db_lists.Mgf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
	        {
	           memcpy(p_current, (BYTE *)&db_lists.Mgf01_Gui_List, sizeof( db_lists.Mgf01_Gui_List ));
	           p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	                                         ptemsg_get_msg_subtype1 (p_msg_in),
	                                         ptemsg_get_msg_subtype2 (p_msg_in),
	                                         reply_que,
	                                         application_que_name,
	                                         (pBYTE)AtpBuffer,
	                                         sizeof(AUTH_TX) + sizeof(db_lists.Mgf01_Gui_List),
	                                         MGF01_DATA);
	        }
	   	}
     break;

      default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_service_list_msg( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {

      

      case BCH10_DATA:

         PRINT( "request to retrieve bch10 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bch10_service_list( ( pBCH10 )p_data, &db_lists.Bch10_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch10_List, sizeof( db_lists.Bch10_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch10_List),
                                          BCH10_DATA);
         }
         break;

      case BCH11_DATA:

         PRINT( "request to retrieve bch11 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bch11_service_list( ( pBCH11 )p_data, &db_lists.Bch11_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch11_List, sizeof( db_lists.Bch11_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch11_List),
                                          BCH11_DATA);
         }
         break;

     case BCH01_DATA:

         PRINT( "request to retrieve bch01 list\n" );
        
         if( ( result_code = db_get_bch01_service_list( ( pBCH01 )p_data, &db_lists.Bch01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bch01_List, sizeof( db_lists.Bch01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Bch01_List),
                                          BCH01_DATA);
         }
         break;

	  case BCH01_MP_DATA:
	 
			 PRINT( "request to retrieve bch01_mp list\n" );
			
			 if( ( result_code = db_get_bch01_mp_service_list( ( pBCH01_MP )p_data, &db_lists.Bch01_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Bch01_List, sizeof( db_lists.Bch01_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  sizeof(AUTH_TX) + sizeof(db_lists.Bch01_List),
											  BCH01_MP_DATA);
			 }
			 break;

		 
     case TLF01_DATA:

         PRINT( "request to retrieve tlf01 list\n" );
        
         if( ( result_code = db_get_tlf01_service_list( ( pTLF01 )p_data, &db_lists.Tlf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Tlf01_List, sizeof( db_lists.Tlf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Tlf01_List),
                                          TLF01_DATA);
         }
         break;
      
     case NFI01_DATA:

         PRINT( "request to retrieve nfi01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_nfi01_service_list( ( pNFI01 )p_data, &db_lists.Nfi01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Nfi01_List, sizeof( db_lists.Nfi01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Nfi01_List),
                                          NFI01_DATA);
         }
         break;
     case NMF01_DATA:

         PRINT( "request to retrieve nmf01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_nmf01_list( ( pNMF01_REQUEST )p_data, &db_lists.Nmf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Nmf01_List, sizeof( db_lists.Nmf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Nmf01_List),
                                          NMF01_DATA);
         }
         break;
     case NBF01_DATA:
        
         PRINT( "request to retrieve nbf01 service list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_nbf01_service_list( ( pNBF01 )p_data, &db_lists.Nbf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Nbf01_List, sizeof( db_lists.Nbf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX) + sizeof(db_lists.Nbf01_List),
                                    NBF01_DATA);
         }
         break;

      default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_count( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pBYTE          p_data;
   CHAR           table_count[5] = "";

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   switch( app_data_type )
   {

      
      case NBF01_DATA:

         PRINT( "request to retrieve nbf01 count\n" );
         if( ( result_code = db_get_nbf01_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, table_count, strlen(table_count) + 1);
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + strlen(table_count) + 1,
                                          NBF01_DATA);
         }
         break;

       
     default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_echo_msg ( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;


   PRINT ("processing Echo request\n");

   p_msg_data     = ptemsg_get_pte_msg_data              (p_msg_in);
   p_data         = ptemsg_get_pte_msg_data_data         (p_msg_data);
   data_len       = ptemsg_get_pte_msg_data_data_length  (p_msg_data);
   app_data_type  = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);


   p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                 ptemsg_get_msg_subtype1 (p_msg_in),
                                 ptemsg_get_msg_subtype2 (p_msg_in),
                                 reply_que,
                                 application_que_name,
                                 p_data,
                                 data_len,
                                 app_data_type);

   if (p_msg_out == NULL_PTR)
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build Echo response to %s", reply_que);
   }

   return (p_msg_out);
}



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_db_request ( pPTE_MSG p_msg_in )
{
   pPTE_MSG       p_msg_out;
   CHAR           query_start_time[20]="";
   CHAR           query_end_time  [20]="";
   CHAR           errbuf[120];
   INT            ret_val;
   double         start_time;
   double         end_time;
   double         duration;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   AUTH_TX		  auth_tx;

   p_msg_out     = NULL_PTR;

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   
   /****************************************************************/
   // Retive Auth Tx to chech its an MP or Fleet card transaction.
   memset (&auth_tx, 0, sizeof (auth_tx)) ;
   memcpy (&auth_tx, (AUTH_TX*) p_data, sizeof (auth_tx)) ;
   /*****************************************************************/

   MsgSubtype2 = ptemsg_get_msg_subtype2(p_msg_in);
   MsgSubtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   /* For DB statics added by raghu*/
   DB_MsgSubtype2 = '0';
   DB_MsgSubtype1 = '0';
   DB_MsgSubtype2 = MsgSubtype2 ;
   DB_MsgSubtype1 = MsgSubtype1 ;
         /*end*/
   memset(AtpBuffer, 0, sizeof(AtpBuffer));
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

   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_DB_SELECT            : p_msg_out = process_select_msg       (p_msg_in);
                                      break;
      case ST1_DB_UPDATE            : p_msg_out = process_update_msg       (p_msg_in);
                                      PRINT("returned from process_update_msg\n");                  
                                      break;
      case ST1_DB_INSERT            : /*Check if its a MP or fllet card tansaction */
									if((0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))||
								   	   (0 == strncmp(auth_tx.TLF01_details.card_num, "7",1)))
									{ 
	  										 p_msg_out = process_insert_mp_msg       (p_msg_in);
									}
									else
									{
										p_msg_out = process_insert_msg		 (p_msg_in);

									}
                                      break;
      case ST1_DB_DELETE            : p_msg_out = process_delete_msg       (p_msg_in);
                                      break;
      case ST1_DB_GET_GUI_LIST      : p_msg_out = process_get_gui_list_msg (p_msg_in);
                                      break;
      case ST1_DB_GET_SERVICE_LIST  : p_msg_out = process_get_service_list_msg (p_msg_in);
                                      break;
                                       
      case ST1_DB_ECHO              : p_msg_out = process_echo_msg         (p_msg_in);
                                      break;
      case ST1_DB_GET_COUNT         : p_msg_out = process_get_count        (p_msg_in);
                                      break;
      case ST1_DB_MOVE_TRANS        : p_msg_out = process_move_trans       (p_msg_in);
                                      break;
      case ST1_PROCESS_START        : p_msg_out = process_negative_file    (p_msg_in);
                                      break;
      case ST1_PROCESS_COMPLETE     : p_msg_out = process_negative_file    (p_msg_in);
                                      break;
      /*case ST1_DELETE_BY_DATE       : p_msg_out = process_db_cleanup       (p_msg_in);
                                      break;*/
      default:
         p_msg_out   = NULL_PTR;
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
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
                                                     AppName,
                                                     errbuf );
      if ( ret_val == false )
      {
         devds_log_message(2, 2, errbuf, "process_db_request");
      }
   }
   return (p_msg_out);
}


/*************************************************************************************/
/*************************************************************************************/
void process_monitor_request(pPTE_MSG p_msg_in )
{
   CHAR           monitor_flag[100]="";
   pCHAR          orig_comm;
   CHAR           MonAdd[100]="";
   //pPTE_MSG       p_msg;
   //LONG           ipc_rcode;
   
   memset(AtpBuffer, 0, sizeof(AtpBuffer));
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
       case ST1_MONITOR_UP   : 
         orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
         WriteMemMonitorTable( (BYTE *)orig_comm );  
         /*
         if(ReadMemMonitorTable() != NULL)
         {
            strcpy(MonAdd, ReadMemMonitorTable());
            p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_OPRMSG, 0,applnk_que_name, application_que_name, (pBYTE) "Dataserver ready", 16,0);
            ptemsg_set_orig_comms_info(p_msg, MonAdd);
            ipc_rcode = pteipc_send(p_msg, applnk_que_name);
            free(p_msg);
       
         }
         else
            printf("Null value in MonAdd\n");*/
         break;
      case ST1_MONITOR_DOWN  :
         WriteMemMonitorTable((BYTE *)monitor_flag); 
         break;
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
         break;
   }
}


/*************************************************************************************/
/*************************************************************************************/
void process_logger_reply ( pPTE_MSG p_msg_in )
{
   pCHAR       p_msg_trid;


   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_LOG_REFEED_COMPLETE : 
         in_refeed_mode = false;
         p_msg_trid     = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy (checkpoint_rec.checkpoint_trid, p_msg_trid, sizeof(checkpoint_rec.checkpoint_trid));
            strncpy (last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint (&checkpoint_rec, ErrorMsg);

            if (result_code == PTEMSG_OK)
            {
               printf ("Refeed is complete; ready for normal database activity\n");
               dbcommon_commit ();
            }
            else
            {
               dbcommon_rollback ();
               devds_log_message(1, 3,ErrorMsg , "process_logger_reply");

               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
            	   devds_log_message(1, 3, ErrorMsg, "process_logger_reply");
               }
            }
         }
                                     
         break;
      
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for logger reply from %s", reply_que);
         devds_log_message(1, 2, ErrorMsg, "process_logger_reply");
         break;
   }
}



/*************************************************************************************/
/*************************************************************************************/
BOOLEAN perform_refeed ( pCHAR trid )
{
   CHAR     Buffer[256];


   if (trid [0] != '\0')
   {
      /* send refeed command to TFLogger */
      if (SendRefeedCmd (trid, ErrorMsg) == false)
      {
         sprintf  (Buffer, "Error sending refeed command to TFLogger: %s", ErrorMsg);
         devds_log_message(1, 3, Buffer, "process_response_from_host");
         return (false);
      }

      in_refeed_mode = true;

      PRINT ("refeed command sent to logger\n");
   }

   return (true);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE validate_last_trid ( pPTE_MSG p_msg_in )
{
   int         cmp_result;
   LONG        ipc_rcode;
   CHAR        temp_str [80];
   pCHAR       p_msg_last_trid;
   pPTE_MSG    p_msg_out;


   /* 
    * get a pointer to the msg last_trid.  if the pointer is null, error 
    */
   p_msg_last_trid = ptemsg_get_last_msg_tran_id (p_msg_in);

   /*
    * If the msg last_trid is empty, OK.  This is a request that came 
    * directly from an application (select, etc).  If it is not empty,
    * need to compare it with my last_trid.
    */
   if (p_msg_last_trid [0] != '\0')
   {
      if (last_trid [0] == '\0')
      {
         /*
          * My last_trid is empty.  Update it from this message.
          */
         strncpy (last_trid, p_msg_last_trid, sizeof(last_trid));
      }
      else
      {
         /*
          * My last_trid is not empty.  If it doesn't match the msg last_trid,
          * we have an out of sequence error.  Need to initiate a refeed from
          * my last_trid.
          */
         cmp_result = strncmp (last_trid, p_msg_last_trid, sizeof(last_trid));

         if (cmp_result != 0)
         {
            perform_refeed (last_trid);

            result_code = PTEMSG_INVALID_TRID;
            sprintf  (ErrorMsg, "invalid last-trid received: %s.   expected: %s",
                      p_msg_last_trid, last_trid); 
         }
      }
   }
   else
   {
      if (in_refeed_mode == true)
      {
         result_code = PTEMSG_IN_REFEED_MODE;
         strcpy (ErrorMsg, 
                 "In refeed mode; cannot accept application requests until complete");
      }
   }

   if (result_code != PTEMSG_OK)
   {
	   devds_log_message(1, 2, ErrorMsg, "validate_last_trid");
	   /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).
       */
      reply_que = ptemsg_get_msg_orig_queue (p_msg_in);

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) ErrorMsg,
                                    strlen(ErrorMsg) + 1,
                                    0);

      if (p_msg_out == NULL_PTR)
      {
         sprintf  (ErrorMsg, "Insufficient Memory to build error response to %s", reply_que);
         devds_log_message(1, 2, ErrorMsg, "validate_last_trid");
         return (result_code);
      }

      ptemsg_set_result_code (p_msg_out, result_code);
      ptemsg_set_orig_comms_info( p_msg_out, orig_comm_inf );
      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_msg_out, reply_que);

      free (p_msg_out);

      if (ipc_rcode < 0)
      {
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
         devds_log_message(1, 2, ErrorMsg, "validate_last_trid");
      }
   }

   return (result_code);
}



/*************************************************************************************/
/*************************************************************************************/
void rollback_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* first, rollback the database update(s) */
      dbcommon_rollback ();

      /* then rollback any timer requests (makes sure memory table matches db) */
      dstimer_rollback ();

      updates_to_db_made = false;
   }
}  /* rollback_the_update */



/*************************************************************************************/
/*************************************************************************************/
void commit_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* commit the database update(s) */
      dbcommon_commit ();

      /* let the timer know a commit has occurred */
      dstimer_commit ();

      updates_to_db_made = false;
   }
}  /* rollback_the_update */



/*************************************************************************************/
/*************************************************************************************/
BYTE PinnacleMsgHandler ( pPTE_MSG p_msg_in )
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
   pPTE_MSG_DATA  p_msg_data;
   INT            ret_val;
   INT            num_sql;
   INT         refresh_flag = 0;  // If an error in update_mem_record or delete_mem_record,
                              // delete all recs. in mem and recreate all tables over again 
                              // from the database.
   static DB_reconnect = 0;

   #ifdef USING_CHECKPOINTS
      pCHAR       p_msg_trid;
   #endif


   ptestats_accumulate_msg_stats (p_msg_in);

   result_code = PTEMSG_OK;

   #ifdef USING_CHECKPOINTS
      if ( (result_code = validate_last_trid (p_msg_in)) != PTEMSG_OK )
         return (result_code);
   #endif

   p_cur_msg    = p_msg_in;
   p_tmp_msg    = NULL_PTR;
   p_chn_out    = NULL_PTR;
   i            = 1;
   num_messages = ptemsg_get_num_chained_messages (p_msg_in);
   /* Added for UpdateMemRecord stuff */
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);


   while ( (i <= num_messages) && (p_cur_msg != NULL_PTR) )
   {
      reply_que = ptemsg_get_msg_orig_queue (p_cur_msg);
      strcpy(orig_comm_inf, ptemsg_get_msg_orig_comms_info( p_cur_msg ));
      app_data_type = 0;
      DB_reconnect = 0;
      switch( ptemsg_get_msg_type( p_cur_msg ) )
      {
         case MT_DB_REQUEST     : p_tmp_msg = process_db_request (p_cur_msg);
                                  break;
         case MT_TIMER_REQUEST  : p_tmp_msg = dstimer_process_request (p_cur_msg, &updates_to_db_made);       
                                  break;

         case MT_SYSTEM_REQUEST : ptesystm_system_msg_handler (p_cur_msg);
                                  if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
                                  {
                                     if ( ST2_NO_VERSION != ptemsg_get_msg_subtype2(p_msg_in) )
                                     {
                                        /* When pinged, display application version on Monitor. */
                                        sprintf( temp_str,
                                                "Pinged -> Data Server: %s, version %s",
                                                 ServiceName, Version );
                                        devds_log_message(1, 1, temp_str, "PinnacleMsgHandler");

                                        /* Get Database Timing Parameters to gather
                                         * statistics about each query.
                                         */
                                        memset( ReportTime, 0x00, sizeof(ReportTime) );
                                        memset( temp_str,   0x00, sizeof(temp_str)   );
                                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                                          ReportTime, 
                                                                         &ForceReport,
                                                                          temp_str );
                                        devds_log_message(2, 1, temp_str, "PinnacleMsgHandler");
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
                                                       AppName, num_sql );
                                              devds_log_message(2, 1, temp_str, "PinnacleMsgHandler");
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
         case MT_LOGGER_REPLY   : process_logger_reply (p_cur_msg);
                                  break;
         case MT_SYS_MONITOR    : process_monitor_request (p_cur_msg);
                                  break;
         case MT_CLEANUP        : process_db_cleanup    (p_cur_msg);
                                  break;
/*         case MT_NEGATIVE_FILE  : p_tmp_msg = process_negative_file (p_cur_msg);
                                  break;*/
         default :
            /*
             * Received an unknown msg type.  If the request was destined for
             * this dataserver, log an error because I can't process it.
             * Otherwise, try to forward it on to the actual destination.
             */
            if ( (strncmp (ptemsg_get_msg_dest_queue (p_cur_msg),
                           application_que_name, strlen(application_que_name)) == 0) ||
                 (strncmp (ptemsg_get_msg_dest_queue (p_cur_msg),
                           control_que_name,     strlen(control_que_name)    ) == 0)   )
            {
               result_code = PTEMSG_INVALID_MSGTYPE;
               sprintf (ErrorMsg, "Unknown msgtype for request from %s", reply_que);
            }
            else
            {
               /*
                * Forward the request to its destination (if not in refeed mode).
                */
               if (in_refeed_mode == false)
               {
                  sprintf (temp_str, "forwarding message to queue %s\n", 
                           ptemsg_get_msg_dest_queue (p_cur_msg));
                  PRINT (temp_str);

                  ptemsg_set_orig_comms_info( p_cur_msg, orig_comm_inf );
                  ipc_rcode = pteipc_send (p_cur_msg, ptemsg_get_msg_dest_queue (p_cur_msg));

                  if (ipc_rcode < 0)
                  {
                     result_code = PTEMSG_COULD_NOT_FORWARD;
                     pteipc_get_errormsg (ipc_rcode, temp_str);
                     sprintf  (ErrorMsg, "error forwarding to queue %s: %s", 
                               ptemsg_get_msg_dest_queue (p_cur_msg), temp_str);
                  }
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
         result_code = ptemsg_get_result_code (p_tmp_msg);
         printf("Result code in is PinnacleMsgHandler %d\n",result_code);
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
            free (p_chn_out);
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
            p_new_chn = ptemsg_chain_message (p_chn_out, p_tmp_msg);

            if (p_new_chn == NULL_PTR)
            {
               result_code = PTEMSG_INSUFFICIENT_MEMORY;
               sprintf  (ErrorMsg, "Insufficient Memory to chain response to %s", reply_que);
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
         p_cur_msg = ptemsg_get_chained_pte_msg (p_msg_in, i);
   }  /* while */


   /*
    * Be sure to free up the temp message.
    */
   if (p_tmp_msg != NULL_PTR)
   {
      free (p_tmp_msg);
      p_tmp_msg = NULL_PTR;
   }

   #ifdef USING_CHECKPOINTS
      /*
       * Update the checkpoint.
       */
      if (result_code == PTEMSG_OK)
      {
         p_msg_trid = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy (checkpoint_rec.checkpoint_trid, p_msg_trid, sizeof(checkpoint_rec.checkpoint_trid));
            strncpy (last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint (&checkpoint_rec, ErrorMsg);
         }
      }
   #endif

   /*
    * If there was an error, back out the changes.
    * Otherwise commit the changes.
    */
   reply_que = ptemsg_get_msg_orig_queue (p_msg_in);

   if (result_code != PTEMSG_OK)
   {
      rollback_the_update ();

      if (p_chn_out != NULL_PTR)
      {
         free (p_chn_out);
         p_chn_out = NULL_PTR;
      }

      /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).
       */
      memcpy(AtpBuffer + sizeof(AUTH_TX) , ErrorMsg, strlen(ErrorMsg) + 1);
      p_chn_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_cur_msg),
                                    ptemsg_get_msg_subtype2 (p_cur_msg),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) AtpBuffer,
                                    strlen(ErrorMsg) + 1 + sizeof(AUTH_TX) ,
                                    app_data_type);

      if (p_chn_out == NULL_PTR)
      {
         devds_log_message(1, 2, ErrorMsg, "PinnacleMsgHandler");
    	 result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build error response to %s", reply_que);

      }
   }
   else
   {
      commit_the_update ();
    
      
   }

   if (p_chn_out != NULL_PTR)
   {
      /*
       * If in refeed mode, don't send the response.
       */
      if (in_refeed_mode == true)
         return (PTEMSG_OK);

      /*
       * If the result is OK, don't send the response if they don't want it.
       */
      if ( (result_code                                == PTEMSG_OK                 ) &&
           (ptemsg_get_type_of_reply_needed (p_msg_in) == PTEMSG_REPLY_ONLY_IF_FAILS) )
      {
         free (p_chn_out);
         return (PTEMSG_OK);
      }

      ptemsg_set_result_code (p_chn_out, result_code);
      ptemsg_set_orig_comms_info( p_chn_out, orig_comm_inf );

      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_chn_out, reply_que);

      free (p_chn_out);

      if (ipc_rcode < 0)
      {
         devds_log_message(1, 2, ErrorMsg, "PinnacleMsgHandler");
    	 result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);

      }
   }

   if(DB_reconnect == 1)
   {
	   dbcommon_disconnect (ErrorMsg);
	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
	   {
		    strcat (ErrorMsg, " DB Reconnection failed");
			devds_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler");
			return (false);
	   }
	   else
	   {
		   devds_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler");
	   }

   }
   else
   {
	   DB_reconnect = 0;
   }
   return (result_code);
}



/*************************************************************************************/
/*************************************************************************************/
BOOLEAN perform_startup ()
{
   CHAR     XipcInstanceName[12];
   INT      ret_val;
   CHAR     Buffer[300] = "";

   memset (last_trid, 0, sizeof(last_trid));

   /* Try to connect to the database */
   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
   {
      devds_log_message(1, 3, ErrorMsg, "perform_startup");
      return (false);
   }

   printf ("Connected to ORACLE\n");


   /* Try to initialise the xipc stuff */
   GetAppName          (AppName);
   GetXipcInstanceName (XipcInstanceName);

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

   /*if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)*/
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName, XipcInstanceName ) )
   {     
      devds_log_message(1, 3, ErrorMsg, "perform_startup");
	  pteipc_shutdown_multiple_instance_app();
      dbcommon_disconnect (ErrorMsg);

      return (false);
   }
   
   printf ("pteipc init successful\n");


   #ifdef USING_LOGGER_REPLY
      /* send restart command to TFLogger */
      if (SendRestartCmd (ErrorMsg) == false)
      {
         devds_log_message(1, 3, ErrorMsg, "perform_startup");
         dstimer_shutdown ();
         pteipc_shutdown_multiple_instance_app();
         dbcommon_disconnect (ErrorMsg);

         return (false);
      }

      printf ("restart command sent to logger\n");

      #ifdef USING_CHECKPOINTS
         /* get the checkpoint record from the database */
         if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
         {
            devds_log_message(1, 3, ErrorMsg, "perform_startup");
            dstimer_shutdown ();
            pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);

            return (false);
         }

         /* 
          * refeed from my last known trid.
          */
         strncpy (last_trid, checkpoint_rec.checkpoint_trid, sizeof(last_trid));

         if (perform_refeed (checkpoint_rec.checkpoint_trid) == false)
         {
            dstimer_shutdown ();
            //pteipc_shutdown_single_instance_app();
            pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }
      #endif
   #endif

   /* Get Timer Parameters to flag Queries that take too long. */
   ret_val = get_dataserver_ini_timer_value( &ds_timer_flag, &ds_timer_value,
                                             AppName,         Buffer );
   if ( false == ret_val )
   {
       devds_log_message(2, 2, Buffer, "perform_startup");
   }
   else if ( ds_timer_flag == DS_TIMER_ON )
   {
      sprintf( Buffer,
              "%s:Data Server timing to flag long queries is turned on, value is %d",
               AppName, ds_timer_value );
      devds_log_message(2, 1, Buffer, "perform_startup");
   }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   devds_log_message(2, 1, Buffer, "perform_startup");

   /* Get the time for when the DB Timing Statistical Report is to be logged. */
   if ( DB_Timing_Flag == true )
   {
      reset_timing_stats( &TimingStats );
      NextReportTime = set_next_report_time_at_startup( ReportTime );
   }
   return (true);
}
/*************************************************************************************/
/*************************************************************************************/

#ifndef WIN32

void TrapFunction( INT signum )
{
   CHAR Buffer[100];

   XIPC_TRAP_FUNCTION_TEST( TrapFunction, signum );

   memset( Buffer, 0x00, sizeof(Buffer) );
   sprintf( Buffer, "%s:  Caught signal %d", AppName, signum );
   devds_log_message(2,2,Buffer,"TrapFunction");

}

#endif

void MainProcessor()
{
   LONG     ipc_rcode, xipc_code;
   pPTE_MSG p_msg;
   CHAR     Buffer[256];
   CHAR     MonAdd[100]="";
   INT      rt;
   INT      num_sql;
   INT      err_ctr = 0;
   CHAR     strError[512] = {0} ;
   INT 		ret_val=0;
	
   GetAppName( AppName );
   #ifdef _DEBUG
      pteipc_trace_on();
   #endif
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
   ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                         ServiceName,     Buffer );
			
   devds_log_message(2,2,Buffer,"MainProcessor");
#if AMEX03B_APRIL2024
   get_variable_auth_config_details();
#endif
#if BDOR_62_PREAUTH_VOID
   get_maximum_validity_of_preauth_in_days();
#endif
   get_maximum_validity_of_preauth_in_days_for_smcc ();
	printf( "%s\n", Buffer );
   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
		 devds_log_message(2,1,Buffer,"MainProcessor");
			printf( "%s\n", Buffer );

      return;
   }




   MemSize = GetMemorySize();
   MemAvailable = MemSize;
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(OPRMON_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(OPRMON_DATA);
        if(xipc_code >= 0L)
            OprMonInMem = true;
         else
            OprMonInMem = false;
      }
   }

   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(BIN01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(BIN01_DATA);
         if(xipc_code >= 0L)
            Bin01InMem = true;
         else
            Bin01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(CAF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(CAF01_DATA);
         if(xipc_code >= 0L)
            Caf01InMem = true;
         else
            Caf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(FAF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(FAF01_DATA);
         if(xipc_code >= 0L)
            Faf01InMem = true;
         else
            Faf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(CRF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(CRF01_DATA);
         if(xipc_code >= 0L)
            Crf01InMem = true;
         else
            Crf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(CSF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(CSF01_DATA);
         if(xipc_code >= 0L)
            Csf01InMem = true;
         else
            Csf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      rt = CreateSQL01_Table();
      if ( rt < 0 )
      {
         sprintf( Buffer, "Unable to create SQL01 in memory. Xipc err %d", rt );
         devds_log_message(2,2,Buffer,"MainProcessor");
      }
   }

   /* Read CUP MID indicator value from tf.ini*/
   devds_read_CUP_merchant_indicator();
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   devds_log_message(2,1,Buffer,"MainProcessor");
   printf( "%s\n", Buffer );

   printf ( "DataServer is ready. press <ENTER> to exit\n" );

   while ( !EndProcessSignalled )
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for the specified time, the blocking call returns   */
      p_msg = pteipc_receive( application_que_name, control_que_name, 
                              TIME_BETWEEN_CHECKS, &ipc_rcode );

      if( p_msg != NULL_PTR )
      {
         PinnacleMsgHandler( p_msg );
         free (p_msg);
      }
      else if( ipc_rcode != QUE_ER_TIMEOUT ) 
      {
         pteipc_get_errormsg ( ipc_rcode, ErrorMsg );
         sprintf ( Buffer, "Error on pteipc_receive %s", ErrorMsg );
         devds_log_message(1, 2, ErrorMsg, "MainProcessor");
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
            devds_log_message(1, 3, ErrorMsg, "MainProcessor");
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
          devds_log_message(2,3,XipcErrBuffer,"MainProcessor");
         exit(1);
         */
      }
      else
      {
         in_refeed_mode = false;

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
                     AppName, num_sql );
            devds_log_message(2,1,Buffer,"MainProcessor");
            reset_timing_stats( &TimingStats );
            NextReportTime = set_next_report_time_at_startup( ReportTime );
         }
      }

      dstimer_check_for_timeouts ();
   }

   dstimer_shutdown ();

   if(Bin01InMem)
   {
      xipc_code = DestroyXipcTable("Bin01Table");
   }
   if(Caf01InMem)
   {
      xipc_code = DestroyXipcTable("Caf01Table");
   }
   if(Faf01InMem)
   {
      xipc_code = DestroyXipcTable("Faf01Table");
   }
   if(Crf01InMem)
   {
      xipc_code = DestroyXipcTable("Crf01Table");
   }
   if(Csf01InMem)
   {
      xipc_code = DestroyXipcTable("Csf01Table");
   }
   if(OprMonInMem)
   {
      xipc_code = DestroyXipcTable("MonitorTable");
   }

   /* shut down ipc */
   //pteipc_shutdown_single_instance_app(); 
   pteipc_shutdown_multiple_instance_app(); 

   printf ("pteipc shutdown successful\n");

   /* disconnect from database */
   if (dbcommon_disconnect (ErrorMsg) != PTEMSG_OK)
      printf ( "%s\n", ErrorMsg );
   else
      printf ( "Disconnected from ORACLE\n" );

   /* signal that we are ready for shutdown */
   MainProcessDone = 1;
}



/*************************************************************************************/
/*************************************************************************************/
void EndProcess()
{
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   devds_log_message(2,1,Buffer,"EndProcess");
   strcat( Buffer, "\n" );
   PRINT( Buffer );

   pteipc_sleep (3000);
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
   CHAR  devds_error_warning_file_size[5] = {0};


   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*----------------------------------------*/
   /* Get Data Server error statics  Flag - On or Off */
   /*----------------------------------------*/

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
   								  devds_error_warning_file_size,              	/* points to destination buffer   */
   								  sizeof(devds_error_warning_file_size)-1,   	 /* size of destination buffer     */
   								  filename                  /* points to ini filename         */
   								);

   	/* File size size conversion */
   	Max_File_Size_Defined = atoi(devds_error_warning_file_size);
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


void Create_Db_Statics_And_Db_Oracle_Error_Filename()
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
	#define NUM_SIZE_WRITES  1
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
	strcat(Buffer, ServiceName );
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
	}

	fclose(fp);
	return ;
}

/******************************************************************************
 *
 *  NAME:         devds_log_message
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
void devds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func )
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;

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
   devds_Log_error_warning_to_File(text_message,sev,func);
   /* Call function to post the message. */
   if(monitor_flag == 1)
   {
	   /* as of now passing null instead of auth info's..
    	* later it can be improved if required
    	* */
	   text_message_len = strlen(text_message);
	   if(text_message_len > 255)
	   {
		   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
		   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
	   else
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
   }
   return;
}

#if AMEX03B_APRIL2024
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
								  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
								  "MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
								  "30",  					/* Default string                 */
								  look_back_days,              	/* points to destination buffer   */
								  sizeof(look_back_days)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	   if(strlen(look_back_days)==0)
	   {
		   strncpy( maximum_validity_of_preauth_in_days_for_AMEX,
				    MAX_PREAUTH_VALIDITY_DAYS,
					ISO_RESPONSE_CODE_LEN );
	   }
	   else
	   {
		   strncpy(maximum_validity_of_preauth_in_days_for_AMEX,look_back_days,ISO_RESPONSE_CODE_LEN);
	   }

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

#if BDOR_62_PREAUTH_VOID

INT get_maximum_validity_of_preauth_in_days()
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "MAX_PREAUTH_VALIDITY_DAYS_FOR_MC",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( maximum_validity_of_preauth_in_days_for_MC,
			    MAX_PREAUTH_VALIDITY_DAYS,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_MC,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_VISA",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( maximum_validity_of_preauth_in_days_for_VISA,
			    MAX_PREAUTH_VALIDITY_DAYS,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_VISA,rc_value,ISO_RESPONSE_CODE_LEN);
	}


	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_AMEX,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_AMEX,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_DCI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_DCI,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_DCI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "MAX_PREAUTH_VALIDITY_DAYS_FOR_JCB",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_JCB,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_JCB,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_UPI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_UPI,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_UPI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	return (ret_val);
}

#endif

#if OCT_MAN_2024_BLD_2
INT get_maximum_validity_of_preauth_in_days_for_smcc(void)
{
   CHAR  rc_value[3]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_MC",       	/* points to key name             */
									  "05",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( smcc_maximum_validity_of_preauth_in_days_for_MC,
			    SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_MC,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_VISA",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( smcc_maximum_validity_of_preauth_in_days_for_VISA,
			    SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_VISA,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}


	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_AMEX,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_AMEX,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_DCI",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_DCI,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_DCI,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_JCB",       	/* points to key name             */
									  "05",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_JCB,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_JCB,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_UPI",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_UPI,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_UPI,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	return (ret_val);
}

#endif

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
#endif

INT devds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;
	AUTH_TX Auth_Tx;
	CHAR  card_tid_mid_details [256] = {0};
	INT   cardlen = 0;
	INT   nIndex = 0;
	CHAR  tempcard[21] = {0};

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
	strcat(Buffer, ServiceName );
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
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}
INT devds_check_If_MC_Transaction (pCHAR card_num)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	card_num[0]=='5')
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
INT devds_check_If_VISA_Transaction (pCHAR card_num)
{
	if (card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT devds_check_If_JCB_Transaction (pCHAR card_num)
{
	if((card_num[0]=='3') &&
	   (card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT devds_check_If_Amex_Transaction (pCHAR card_num)
{
	if((card_num[0]=='3') &&
	   ((card_num[1] == '4') ||
		(card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT devds_check_If_DCI_Transaction (pCHAR card_num)
{
	if((card_num[0]=='3') &&
	   ((card_num[1] == '0') ||
		(card_num[1] == '6') ||
		(card_num[1] == '8') ||
		(card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((card_num[0]=='6') &&
			 ((card_num[1] == '0') ||
			 (card_num[1] == '4') ||
			 (card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT devds_Check_If_UPI_Transaction (pCHAR card_num)
{
	if ((card_num[0]=='6') &&
		((card_num[1] != '0') &&
		 (card_num[1] != '4') &&
		 (card_num[1] != '5')
		)
	   )
	{
		return true ;
	}
	else
	{
		return false;
	}
}
#if AMEX03B_APRIL2024
INT devds_check_If_AMEX_Transaction (pCHAR card_num)
{
	if((card_num[0]=='3') && ((card_num[1] == '4') || (card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
#endif

/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/
