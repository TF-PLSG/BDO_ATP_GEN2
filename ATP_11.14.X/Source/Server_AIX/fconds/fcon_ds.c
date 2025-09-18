/******************************************************************************

   Module: fconds_ds.c

   Title: FAI RT Dataserver Routines

   Description: 
	
	AUTHOR : Raghu

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
#include "nc_dbstruct.h"

TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;


/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
CHAR   oprmon_que_name[MAX_APP_NAME_SIZE];

/*Merchant offset in croosponding tables*/
#define DCF01_MERCHANT_ID_OFFSET      96
#define BCH01_MERCHANT_ID_OFFSET      16
#define BCH01_MP_MERCHANT_ID_OFFSET   16
#define BCH10_MERCHANT_ID_OFFSET      29
#define BCH11_MERCHANT_ID_OFFSET	  29
#define BCH20_MERCHANT_ID_OFFSET      29
#define  TF_INI_FILE   "tf.ini"
#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033
/**********Primary Key Size*********************/
#define BCH01_PRIMARY_KEY_SIZE 32 // Device id + Batch_number + Merchant ID.
#define BCH01_MP_PRIMARY_KEY_SIZE 32 // Device id + Batch_number + Merchant ID.

#define MAX_DATASERVER_BUFFER_LENGTH 25000
#define SYSTEM_MALFUNCTION          "96"
#define DEVICE_ERROR 15
CHAR mracon_que_name  [MAX_QUE_NAME_SIZE+1] = "mraconA" ;
CHAR    fconds_que_name[]       = "fcondsA" ;
INT Mracon_Log_Enable_Flag = 0;



BYTE  DB_MsgSubtype2 ;
BYTE  DB_MsgSubtype1 ;
CHAR  DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
/* End   */
/* This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT    db_error_statics_flag;
double db_error_statics_value;
INT DB_NullCheck_Flag = 0;
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"

UINT Max_File_Size_Defined = 0 ;


/* Function protype */
BYTE process_alldata_msg( pPTE_MSG  );
void TrapFunction( INT signum );

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
BYTE Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   BYTE general_status, BYTE* response_code, pAUTH_TX auth_tx);
void fconds_reset_stats( pTXN_COUNTS p_txn_cnts );
BYTE fconds_send_transaction_statistics(pPTE_MSG p_msg );
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2,
                                    pBYTE    que_name,
                                    pBYTE    buffer,
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );
INT get_MRACON_state_from_ini( void );


PRIVATE union
{
  FCLF01  fclf01;
  FRULE01 frule01;
  FTLF01  ftlf01;
  MRA01    mra01;
} db_recs;

PRIVATE union
{
   FCLF01_GUI_LIST	   fclf01_List;
   FRULE01_GUI_LIST	   frule01_List;
}db_lists;


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
CHAR      Version[] = "ATP_11.6.0";

CHAR  fconds_Erro_warning_Filename[256] = {0};
CHAR  fconds_module_error_warning_file_name[256] = {0};
CHAR  fconds_error_warning_file_path[256] = {0};

INT ds_timer_flag;
INT ds_timer_value;

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
/*************************************************************************************/
void fconds_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( fconds_Erro_warning_Filename, fconds_error_warning_file_path );
    strcat( fconds_Erro_warning_Filename, fconds_module_error_warning_file_name );
	strcat( fconds_Erro_warning_Filename, ".EW." );
    strncat(fconds_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(fconds_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(fconds_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}
/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void fconds_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   fconds_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(fconds_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  fconds_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(fconds_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   if((strlen(fconds_error_warning_file_path) > 0) &&
	  (strlen(fconds_module_error_warning_file_name)> 0))
   {
	   fconds_create_Error_Filename();
   }
}

/******************************************************************************
 *
 *  NAME:         fconds_log_message
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void fconds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
   INT  Appname_len = 0;

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
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   fconds_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	 TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
								   eventlog_flag, msg_type, func,
								   severity, log_type,
								   NULL, NULL,NULL );
   }

   return;
}
INT fconds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(fconds_Erro_warning_Filename);
    path_len = strlen(fconds_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&fconds_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	fconds_create_Error_Filename();
    }
	if((fp = fopen(fconds_Erro_warning_Filename,"a+b"))==NULL)
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


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_insert_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pAUTH_TX auth_tx;
   CHAR strTemp[256]= {0};

   
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   auth_tx = (pAUTH_TX)p_data;
   p_data = p_data + sizeof(AUTH_TX);

   sprintf(strTemp, "In Insert Fun %d ", app_data_type);
   fconds_log_message( 2, 1,strTemp,"process_insert_msg", 0 );
   memset (strTemp, 0x00, sizeof(strTemp));
   switch( app_data_type )
   {
       case FCLF01_DATA:

         memset( &db_recs.fclf01, 0, sizeof( db_recs.fclf01 ) );
         memcpy( &db_recs.fclf01, ( pFCLF01 )p_data, sizeof( FCLF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			 CheckNULLTerminatedinFCLF01 (&db_recs.fclf01,MODE_INSERT);
		 }
         result_code = db_insert_fclf01( &db_recs.fclf01, ErrorMsg );
         break;

       case FRULE01_DATA:

         memset( &db_recs.frule01, 0, sizeof( db_recs.frule01 ) );
         memcpy( &db_recs.frule01, ( pFRULE01 )p_data, sizeof( FRULE01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			 CheckNULLTerminatedinFRULE01 (&db_recs.frule01,MODE_INSERT);
		 }
         result_code = db_insert_frule01( &db_recs.frule01, ErrorMsg );
         break;

       case FTLF01_DATA:

         memset( &db_recs.ftlf01, 0, sizeof( db_recs.ftlf01 ) );
         memcpy( &db_recs.ftlf01, ( pFTLF01 )p_data, sizeof( FTLF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			 CheckNULLTerminatedinFTLF01 (&db_recs.ftlf01,MODE_INSERT);
		 }
         result_code = db_insert_ftlf01( &db_recs.ftlf01, ErrorMsg );
         break;
       case MRA01_DATA:
           /* retrieve the data into a mra01 structure and insert into database */
    	   memset(&db_recs.mra01,0x00,sizeof(db_recs.mra01));
    	   memcpy( &db_recs.mra01, ( pMRA01 )p_data, sizeof( MRA01 ) );
    	   fconds_log_message( 2, 1,"MRA01 DATA","process_insert_msg", 0 );
    	   result_code =  db_select_mra01(&db_recs.mra01,ErrorMsg);
    	   if(result_code == PTEMSG_OK)
    	   {
			   	 result_code = db_insert_mra01( &db_recs.mra01, ErrorMsg );
				 if(result_code != PTEMSG_OK)
				 {

					 sprintf(strTemp, "Insert to MRA01 Failed", reply_que);
					 fconds_log_message( 2, 3,strTemp,"process_insert_msg", 0 );
				 }
				 else
				 {
					 fconds_log_message( 2, 1,"Successfully Inserted to MRA01 Table","process_insert_msg", 0 );
				 }
    	   }
    	   else if(result_code == PTEMSG_NOT_FOUND)
		   {
			   //sprintf(ptr_MRA01->transaction_number,"%09ld",txn_cnt);
			   sprintf(strTemp, "Select from MRA01 Failed due to absence of the record on specified date", reply_que);
			   fconds_log_message( 2, 1,strTemp,"process_insert_msg", 0 );
			   result_code = db_insert_mra01( &db_recs.mra01, ErrorMsg );
				if(result_code != PTEMSG_OK)
				 {

					 sprintf(strTemp, "Insert to MRA01 Failed", reply_que);
					 fconds_log_message( 2, 3,strTemp,"process_insert_msg", 0 );
				 }
				 else
				 {

					 fconds_log_message( 2, 1,"Successfully Inserted to MRA01 Table","process_insert_msg", 0 );
				 }
		   }
    	   else if(result_code != PTEMSG_OK)
		   {
    		   sprintf(strTemp, "Select from MRA01 Failed", reply_que);
			   fconds_log_message( 2, 3,strTemp,"process_insert_msg", 0 );
		   }
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
pPTE_MSG process_update_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   LONG           data_length;
   BCH10         bch10;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);
   data_length = sizeof(AUTH_TX);
   switch( app_data_type )
   {

      case FCLF01_DATA:

         PRINT( "request to update fclf01 record\n" );

         /* retrieve the data into a ucf01 structure and update the database */
         memset( &db_recs.fclf01, 0, sizeof( db_recs.fclf01 ) );
         memcpy( &db_recs.fclf01, ( pDCF01 )p_data, sizeof( DCF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			 CheckNULLTerminatedinFCLF01 (&db_recs.fclf01,MODE_UPDATE);
		 }
         result_code = db_update_fclf01( &db_recs.fclf01, ErrorMsg );
         break;

      case FRULE01_DATA:

         PRINT( "request to update dcf01 record\n" );

         /* retrieve the data into a ucf01 structure and update the database */
         memset( &db_recs.frule01, 0, sizeof( db_recs.frule01 ) );
         memcpy( &db_recs.frule01, ( pDCF01 )p_data, sizeof( FRULE01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			 CheckNULLTerminatedinFRULE01 (&db_recs.frule01,MODE_UPDATE);
		 }
         result_code = db_update_frule01( &db_recs.frule01, ErrorMsg );
         break;

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



BYTE Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   BYTE general_status, BYTE* response_code, pAUTH_TX auth_tx)
{
    if (strncmp (auth_tx->TLF01_details.handler_queue, "VOICE",5)==0)
        strncpy (auth_tx->TLF01_details.voice_auth_text, strTemp,strlen(strTemp)) ;

    strncpy(auth_tx->TLF01_details.response_text,strTemp,
        		(sizeof(auth_tx->TLF01_details.response_text)-1));


	//Prepare_AuthTx_For_Forwarding (general_status,response_code) ;
	strcpy (auth_tx->TLF01_details.response_code, response_code) ;
	auth_tx->TLF01_details.general_status = general_status ;

	return DEVICE_ERROR ;
}
/**********************************************************/

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

      case FCLF01_DATA:

         result_code = db_delete_fclf01( ( pFCLF01 )p_data, ErrorMsg );
         break;

      case FRULE01_DATA:

         result_code = db_delete_frule01( ( pFRULE01 )p_data, ErrorMsg );
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
	  
      case FCLF01_DATA:

  		if( ( result_code = db_select_fclf01( ( pCHAR)p_data, &db_recs.fclf01, ErrorMsg ) ) == PTEMSG_OK )
  		{
  			memcpy(p_current, (BYTE *)&db_recs.fclf01, sizeof( db_recs.fclf01 ));
  			p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
  										  ptemsg_get_msg_subtype1 (p_msg_in),
  										  ptemsg_get_msg_subtype2 (p_msg_in),
  										  reply_que,
  										  application_que_name,
  										  (pBYTE)AtpBuffer,
  										  sizeof(AUTH_TX) + sizeof(db_recs.fclf01),
  										  FCLF01_DATA);

  		}
    	break;

      case FRULE01_DATA:

    	  	if( ( result_code = db_select_frule01( ( pCHAR)p_data, &db_recs.frule01, ErrorMsg ) ) == PTEMSG_OK )
    		{
    			memcpy(p_current, (BYTE *)&db_recs.frule01, sizeof( db_recs.frule01 ));
    			p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
    										  ptemsg_get_msg_subtype1 (p_msg_in),
    										  ptemsg_get_msg_subtype2 (p_msg_in),
    										  reply_que,
    										  application_que_name,
    										  (pBYTE)AtpBuffer,
    										  sizeof(AUTH_TX) + sizeof(db_recs.frule01),
											  FRULE01_DATA);

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

      case FCLF01_DATA:

         if( ( result_code = db_get_fclf01_gui_list( ( pCHAR )p_data, &db_lists.fclf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.fclf01_List, sizeof( db_lists.fclf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.fclf01_List),
										  FCLF01_DATA);
         }
         break;

      case FRULE01_DATA:

		 if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND)
		 {
	         if( ( result_code = db_get_frule01_gui_list_by_card_brand( ( pCHAR )p_data, &db_lists.frule01_List, ErrorMsg ) ) == PTEMSG_OK )
	         {
	            memcpy(p_current, (BYTE *)&db_lists.frule01_List, sizeof( db_lists.frule01_List ));
	            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	                                          ptemsg_get_msg_subtype1 (p_msg_in),
	                                          ptemsg_get_msg_subtype2 (p_msg_in),
	                                          reply_que,
	                                          application_que_name,
	                                          (pBYTE)AtpBuffer,
	                                          sizeof(AUTH_TX) + sizeof(db_lists.frule01_List),
	                                          FRULE01_DATA);
	         }
		 }
		 else
		 {
			 if( ( result_code = db_get_frule01_gui_list( ( pCHAR )p_data, &db_lists.frule01_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.frule01_List, sizeof( db_lists.frule01_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  sizeof(AUTH_TX) + sizeof(db_lists.frule01_List),
											  FRULE01_DATA);
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

   MsgSubtype2 = ptemsg_get_msg_subtype2(p_msg_in);
   MsgSubtype1 = ptemsg_get_msg_subtype1(p_msg_in);

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
		case ST1_DB_INSERT            : p_msg_out = process_insert_msg       (p_msg_in);
									  break;
		case ST1_DB_DELETE            : p_msg_out = process_delete_msg       (p_msg_in);
									  break;
		case ST1_DB_GET_GUI_LIST      : p_msg_out = process_get_gui_list_msg (p_msg_in);
									  break;
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
         /* The query exceeded the time criteria.  Log informational message. */
         fconds_log_message( 2, 2, errbuf, "process_db_request", 0);
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
               fconds_log_message( 1, 3, ErrorMsg, "process_logger_reply", 0 );

               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
                  fconds_log_message( 1, 3, ErrorMsg, "process_logger_reply", 0 );
               }
            }
         }
                                     
         break;
      
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for logger reply from %s", reply_que);
         fconds_log_message( 1, 2, ErrorMsg, "process_logger_reply", 0 );
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
         fconds_log_message( 1, 3, ErrorMsg, "perform_refeed", 0 );
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
      fconds_log_message( 1, 2, ErrorMsg, "validate_last_trid", 0 );
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
         fconds_log_message( 1, 2, ErrorMsg, "validate_last_trid", 0 );
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
         fconds_log_message( 1, 2, ErrorMsg, "validate_last_trid", 0 );
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
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
                                       fconds_log_message( 1, 1, temp_str, "PinnacleMsgHandler", 0 );
                                       memset( ReportTime, 0x00, sizeof(ReportTime) );
                                       memset( temp_str,   0x00, sizeof(temp_str)   );
                                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                                          ReportTime, 
                                                                         &ForceReport,
                                                                          temp_str );
                                        fconds_log_message( 2, 1, temp_str, "PinnacleMsgHandler", 0);
                                        if ( DB_Timing_Flag == true )
                                        {
                            
                                           NextReportTime = set_next_report_time_at_startup( ReportTime );

                                           if ( ForceReport == true )
                                           {
                                              num_sql = log_timing_statistics_report( &TimingStats );
                                              memset( temp_str, 0x00, sizeof(temp_str) );
                                              sprintf( temp_str,
                                                      "%s logged stats for %d SQLs",
                                                       AppName, num_sql );
                                              fconds_log_message( 2, 1, temp_str, "PinnacleMsgHandler", 0);
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

         case MT_GET_STATS 		: fconds_send_transaction_statistics(p_cur_msg);
         	 	 	 	 	 	  break;

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
		if(strcmp(reply_que,"txcntlA")== 0 || strcmp(reply_que,"dcpisoA")==0)
		{
			 fconds_log_message( 1, 2, ErrorMsg, "PinnacleMsgHandler", 0 );
			 return (PTEMSG_OK);
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
         fconds_log_message( 1, 2, ErrorMsg, "PinnacleMsgHandler", 0 );
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build error response to %s", reply_que);
      }
   }
   else
   {
		commit_the_update ();
		if (p_tmp_msg != NULL_PTR)
		{
			free (p_tmp_msg);
			p_tmp_msg = NULL_PTR;
		}

		if(0 == strncmp(reply_que,"mraconA",7) && app_data_type == MRA01_DATA &&
		   ST1_DB_INSERT  ==		ptemsg_get_msg_subtype1 (p_cur_msg))
		{
			if (p_chn_out != NULL_PTR)
			{
			  free (p_chn_out);
			  p_chn_out = NULL_PTR;
			}
			if (p_new_chn != NULL_PTR)
			{
			  free (p_new_chn);
			  p_new_chn = NULL_PTR;
			}//p_cur_msg
			if (p_cur_msg != NULL_PTR)
			{
				free (p_cur_msg);
				p_cur_msg = NULL_PTR;
			 }//p_cur_msg
			return (PTEMSG_OK);
		}
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
         fconds_log_message( 1, 2, ErrorMsg, "PinnacleMsgHandler", 0 );
         result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
      printf   ("%s\n", ErrorMsg);
      fconds_log_message( 2, 2, ErrorMsg, "PinnacleMsgHandler", 0);
   }*/

   if(DB_reconnect == 1)
   {
	   dbcommon_disconnect (ErrorMsg);
	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
	   {
		   strcat (ErrorMsg, " DB Reconnection failed");
		   fconds_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler",0);
			return (false);
	   }
	   else
	   {
		   fconds_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler",0);
	   }

   }
   else
   {
	   DB_reconnect = 0;
   }
   return (result_code);
}
/* 
[MRA_DATA_SOURCE]
MRA_SERVICE_NAME=
MRA_USER_ID=
MRA_PASSWORD=
  
*/



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
      fconds_log_message( 1, 3, ErrorMsg, "perform_startup", 0 );
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

   //12-07-98 gabriel multiple instance

   //if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName, XipcInstanceName ) )
   {     
      fconds_log_message( 1, 3, "Failed to create XIPC queues", "perform_startup", 0 );
      pteipc_shutdown_multiple_instance_app();
      dbcommon_disconnect (ErrorMsg);
      return (false);
   }
   
   printf ("pteipc init successful\n");


   /* Get Timer Parameters to flag Queries that take too long. */
   ret_val = get_dataserver_ini_timer_value( &ds_timer_flag, &ds_timer_value,
                                             AppName,         Buffer );
   if ( false == ret_val )
   {
      fconds_log_message( 2, 2, Buffer, "perform_startup", 0);
   }
   else if ( ds_timer_flag == DS_TIMER_ON )
   {
      sprintf( Buffer,
              "%s:Data Server timing to flag long queries is turned on, value is %d",
               AppName, ds_timer_value );
      fconds_log_message( 2, 1, Buffer, "perform_startup", 0);
   }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   fconds_log_message( 2, 1, Buffer, "perform_startup", 0);

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
   fconds_log_message( 2, 3, Buffer, "TrapFunction", 0);
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
   INT      ret_val = 0 ;
	
   GetAppName( AppName );
   fconds_get_error_warning_file_name_path();
   rt = get_MRACON_state_from_ini();
   #ifdef _DEBUG
      pteipc_trace_on();
   #endif

  ret_val = get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                              ServiceName,     Buffer );
   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
		  fconds_log_message( 2, 1, Buffer, "MainProcessor", 0);
			printf( "%s\n", Buffer );

      return;
   }

   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   fconds_log_message( 2, 1, Buffer, "MainProcessor", 0);
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
         fconds_log_message( 1, 2, Buffer, "MainProcessor", 0 );
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
            fconds_log_message( 1, 3, strError, "MainProcessor", 0 );
            #ifdef WIN32
               Sleep (5000);
            #else
               usleep (5000000);
            #endif
         }

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
            fconds_log_message( 2, 1, Buffer, "MainProcessor", 0);
            reset_timing_stats( &TimingStats );
            NextReportTime = set_next_report_time_at_startup( ReportTime );
         }
      }

      dstimer_check_for_timeouts ();
   }

   dstimer_shutdown ();

   /* shut down ipc */
   
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
   fconds_log_message( 2, 1, Buffer, "EndProcess", 0);
   strcat( Buffer, "\n" );
   PRINT( Buffer );

   pteipc_sleep (3000);
}


/*****************************************************/

/* Read the value of flag Mracon_Log_Enable_Flag from tf.ini*/

/*****************************************************/

INT get_MRACON_state_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
	#define MAX_FILE_NAME_SZIE 128

	CHAR filename     [MAX_FILE_NAME_SZIE];
	CHAR tmpstr       [MAX_FILE_NAME_SZIE];

	CHAR tmpstring    [MAX_INI_INFO_SIZE];
	CHAR ini_info     [MAX_INI_INFO_SIZE];

	   /* Determine directory and filename of the .ini file. */
	   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	/* See if send is turned on */
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

	GetPrivateProfileString(
			 "MRA_SECTION",                   /* points to section name       */
			 "MRACON_LOG_ENABLE_FLAG",       /* points to key name           */
			 "0",                            /* points to default string     */
			 ini_info,                       /* points to destination buffer */
			 MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
			 filename                        /* points to ini filename       */
			 );

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

	Mracon_Log_Enable_Flag = atoi(tmpstring);
}

/******************************************************************************
 *
 *  NAME:         fconds_send_transaction_statistics
 *
 *  DESCRIPTION:  This function will send transaction statistics to Monitor.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to send stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Phani Kumar
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE fconds_send_transaction_statistics(pPTE_MSG p_msg )
{
	BYTE  ret_code =  PTEMSG_OK;
	BYTE  originator_queue[17] = {0};
	BYTE  originator_info[33] = {0};
    CHAR  err_msg[100]        ={0};
    int   len_sent_count = 0;

   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );
   memset(&db_recs.mra01,0x00,sizeof(db_recs.mra01));
   ret_code =  db_select_mra01(&db_recs.mra01,err_msg);
	 //Reset Txn Count
	if(ret_code == PTEMSG_OK )
	{
		//(void)fconds_reset_stats( &Txn_Counts );
		 /* Copy sent count to TXN count*/
		 //len_sent_count = strlen(db_recs.mra01.transaction_number);
		 strncpy(Monitor_Stats.cnt_to_network,db_recs.mra01.transaction_number,sizeof(Monitor_Stats.cnt_to_network)-1);
	}
	else
	{
		// think
	}

	   /* Send statistics to Monitor */
	   memset( err_msg, 0x00, sizeof(err_msg) );
	   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE,
	                                              ST1_NONE,
	                                              ST2_NONE,
	                                              originator_queue,
	                                              (pBYTE)&Monitor_Stats,
	                                              sizeof(MONITOR_STATS),
	                                              0,
	                                              originator_info,
	                                              err_msg );

   return ret_code;
 }


/******************************************************************************
 *
 *  NAME:         RESET_STATS
 *
 *  DESCRIPTION:  This function will set the transaction statistics to zero.
 *                This is the number of transactions received from the host,
 *                and the number of transactions this network controller has
 *                sent to the host.  There are also stats for number of txns
 *                approved, declined, reversed, and timed out.
 *
 *  INPUTS:       p_txn_cnts - Structure containing the counts
 *
 *  OUTPUTS:      p_txn_cnts - Structure counts are updated to zero
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Ramya
 *
 ******************************************************************************/
void fconds_reset_stats( pTXN_COUNTS p_txn_cnts )
{
   p_txn_cnts->sent_count     = 0UL;
   p_txn_cnts->received_count = 0UL;
   p_txn_cnts->approved_count = 0UL;
   p_txn_cnts->declined_count = 0UL;
   p_txn_cnts->reversed_count = 0UL;
   p_txn_cnts->time_out_count = 0UL;
   p_txn_cnts->mgtrequest_count = 0UL;//praneeth
   p_txn_cnts->stip_sent_count = 0UL;	//ANNA
   p_txn_cnts->stip_recv_count = 0UL;	//ANNA
   return;
}


/******************************************************************************
 *
 *  NAME:         BUILD_AND_SEND_REQUEST_MESSAGE
 *
 *  DESCRIPTION:  This procedure builds and sends a request message to the
 *                desired queue.
 *
 *  INPUTS:       msgtype  - PTE message type
 *                subtype1 - Subtype1 for the message (see PTE.h)
 *                subtype2 - Subtype2 for the message (see PTE.h and APP_INFO.h)
 *                que_name - Name of the queue to send the message to
 *                buffer   - Buffer that contains the data to be sent,
 *                length   - Length of buffer
 *                app_type - App data type for the message (see APP_INFO.h)
 *                comm_info- string containing originator communications info
 *
 *  OUTPUTS:      err_msg  - Text to describe reason for failure
 *
 *  RTRN VALUE:   True if successful, False if a problem occurred
 *
 *  AUTHOR:       Ramya
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2,
                                    pBYTE    que_name,
                                    pBYTE    buffer,
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg )
{
   CHAR            Buff[200];
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;
   LONG            msg_ret_code;

   p_msg = ptemsg_build_msg( msgtype,
                             subtype1,
                             subtype2,
                             que_name,
                             application_que_name,
                             buffer,
                             length,
                             app_type );

   if ( NULL_PTR == p_msg  )
   {
      strcpy( err_msg, "Insufficient Memory to build PTE message" );
      ret_code = false;
   }
   else
   {
      /* Set originator communications information. */
      if ( 0 != strcmp( NULL_STR, comm_info ) )
         ptemsg_set_orig_comms_info( p_msg, comm_info );

      /* Send message que_name */
      msg_ret_code = pteipc_send(p_msg, que_name);
      if(msg_ret_code < 0)
	  {
         pteipc_get_errormsg(msg_ret_code, err_msg);
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff, "Failed transmit to queue: " );
         strcat( Buff, que_name );
         strncat(Buff, err_msg, 65 );
         ret_code = false;
	  }
      free(p_msg);
   }
   return( ret_code );
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
   CHAR  fconds_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*----------------------------------------*/
   /* Get Data Server error statics  Flag - On or Off */
   /*----------------------------------------*/
   /*   memcpy( timer_flag, sOFF, sizeof(sOFF) );
      rc = GetPrivateProfileString(
               "DATASERVERS",        // points to section name
               "DB_STATICS_FLAG",       // points to key name
                sOFF,                //Default string (Off)
                timer_flag,          //points to destination buffer
                sizeof(timer_flag)-1,//size of destination buffer
                filename             // points to ini filename
      );

       if ( 0 == strcmp(timer_flag, sOFF) )
      {

     	   *ds_timer_flag = DS_TIMER_OFF;
      }
      else if ( 0 == strcmp(timer_flag, sON) )
      {

         *ds_timer_flag = DS_TIMER_ON;*/
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
      /* }
       else
      {
      	// The tf.ini file contains an invalid value for the timer flag.
           // Log this fact, set timer off, and exit.

          sprintf( errbuf,
                 "%s:Invalid value (%s) in tf.ini file for Data Server DB_ERROR_STATICS_FLAG flag.",
                 appname, timer_flag );
          *ds_timer_flag = DS_TIMER_OFF;
          ret_val = false;
       }*/

         /*******************To check for NULL in every structure member************************/
          /*----------------------------------------*/
             /* Get Data Server error statics  Flag - On or Off */
             /*----------------------------------------*/
         /*    memcpy( db_nullchk_flag, 0x00, sizeof(db_nullchk_flag) );
             rc = GetPrivateProfileString(
                      "DBNULLCHECK",         		//points to section name
                      "DB_NULL_CHECK_FLAG",       //points to key name
                       "0",                 		//Default string (Off)
      				  db_nullchk_flag,          //points to destination buffer
                       sizeof(db_nullchk_flag)-1, //size of destination buffer
                       filename              		//points to ini filename
             );
             DB_NullCheck_Flag = atoi(db_nullchk_flag); */


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
							  fconds_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(fconds_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(fconds_error_warning_file_size);
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


/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/
