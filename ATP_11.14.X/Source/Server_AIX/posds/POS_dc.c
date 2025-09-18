/******************************************************************************

   Module: POS_dc.s

   Title: POS Dataserver Routines

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


/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
CHAR   oprmon_que_name[MAX_APP_NAME_SIZE];


/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
/* End   */

/* * This variable will get the value from the tf.ini
 * from the section DATASERVER with key name
 * DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;

UINT Max_File_Size_Defined = 0 ;

/*Check null*/
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"
INT DB_NullCheck_Flag;

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


#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

/* Function protype */
BYTE process_alldata_msg( pPTE_MSG  );
BYTE process_alldata_mp_msg( pPTE_MSG p_msg_in );
pPTE_MSG process_check_data_in_tables( pPTE_MSG );

void TrapFunction( INT signum );

INT  get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
void posds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);
INT  posds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);



PRIVATE union
{

  TPOS01   tpos01;

 } db_recs;

PRIVATE union
{
	TPOS01_GUI_LIST      tpos01_Gui_List;

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
CHAR      Version[] = "ATP_11.4.0";

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
/*************************************************************************************/
pPTE_MSG process_insert_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   AUTH_TX        auth_tx;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

   memset( &auth_tx, 0x00, sizeof(  auth_tx ) );
   memcpy( &auth_tx, p_data , sizeof( AUTH_TX )  );

   memset( &db_recs.tpos01, 0x00, sizeof(  db_recs.tpos01 ) );
   memcpy( &db_recs.tpos01, p_data + sizeof( AUTH_TX ) ,sizeof( db_recs.tpos01) );

   memcpy(AtpBuffer, p_data, (sizeof(AUTH_TX) + sizeof(TPOS01)));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
       case TPOS01_DATA:

			 PRINT( "request to insert tpos01 record\n" );
			 if(DB_NullCheck_Flag==1)
			 {
			 	CheckNULLTerminatedinTPOS01 (&db_recs.tpos01,MODE_INSERT);
			 }
			 result_code = db_insert_tpos01( &db_recs.tpos01, ErrorMsg );
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
   BCH10          bch10;
   AUTH_TX		  auth_tx;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + sizeof(AUTH_TX);
   data_length = sizeof(AUTH_TX);

	/****************************************************************/

	memset (&auth_tx, 0, sizeof (auth_tx)) ;
	memcpy (&auth_tx, (AUTH_TX*) AtpBuffer, sizeof (auth_tx)) ;


   switch( app_data_type )
   {

      case TPOS01_DATA:

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.tpos01, 0, sizeof( db_recs.tpos01 ) );
         memcpy( &db_recs.tpos01, ( pTPOS01 )p_data, sizeof( pTPOS01 ) );
		 PRINT( "request to update tpos01 record\n" );
		 if(DB_NullCheck_Flag==1)
		 {
		 	CheckNULLTerminatedinTPOS01 (&db_recs.tpos01,MODE_UPDATE);
		 }
         result_code = db_update_tpos01( &db_recs.tpos01, ErrorMsg );
         break;

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type: %s for Update request from %s", app_data_type,reply_que);
         break;
   }


   if (result_code == PTEMSG_OK)
   {
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
      case TPOS01_DATA:

         PRINT( "request to delete tpos01 record\n" );
		 result_code = db_delete_tpos01( ( pTPOS01 )p_data, ErrorMsg );
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

      case TPOS01_DATA:

         subtype2 = ptemsg_get_msg_subtype2( p_msg_in );
         switch( subtype2 )
         {
            case ST2_NONE:
               result_code = db_select_tpos01( (pCHAR)p_data, &db_recs.tpos01, ErrorMsg );
               break;

            default:

                /* unknown app_data_type received. Log Event and return error message to client */
                result_code = PTEMSG_INVALID_DATATYPE;
                sprintf (ErrorMsg, "Unknown app_data_type for Delete request from %s", reply_que);
                break;

         }

         if( result_code == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.tpos01, sizeof( db_recs.tpos01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.tpos01),
                                          TPOS01_DATA);
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
      case TPOS01_DATA:

         PRINT( "request to retrieve tpos01 gui list\n" );
         // copy the last ucf02.group_id retrieved into the structure
         if( ( result_code = db_get_tpos01_gui_list_dynamic( ( pTLF01_LIMITS )p_data, &db_lists.tpos01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.tpos01_Gui_List, sizeof( db_lists.tpos01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.tpos01_Gui_List),
                                          TPOS01_DATA);
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

      case ST1_DB_INSERT            : p_msg_out = process_insert_msg		 (p_msg_in);
                                      break;

      case ST1_DB_DELETE            : p_msg_out = process_delete_msg       (p_msg_in);
                                      break;

      case ST1_DB_GET_GUI_LIST      : p_msg_out = process_get_gui_list_msg (p_msg_in);
                                      break;


      case ST1_DB_ECHO              : p_msg_out = process_echo_msg         (p_msg_in);
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
    	  posds_log_message(2,2,errbuf,"process_db_request");
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
               posds_log_message(1,3,ErrorMsg,"process_logger_reply");

               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
                   posds_log_message(1,3,ErrorMsg,"process_logger_reply");
               }
            }
         }

         break;

      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for logger reply from %s", reply_que);
         posds_log_message(1,2,ErrorMsg,"process_logger_reply");
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
         posds_log_message(1,3,ErrorMsg,"perform_refeed");
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
	   posds_log_message(1,2,ErrorMsg,"validate_last_trid");
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
         posds_log_message(1,2,ErrorMsg,"validate_last_trid");
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
         posds_log_message(1,2,ErrorMsg,"validate_last_trid");
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
                                       posds_log_message(1,1,temp_str,"PinnacleMsgHandler");

                                        /* Get Database Timing Parameters to gather
                                         * statistics about each query.
                                         */
                                        memset( ReportTime, 0x00, sizeof(ReportTime) );
                                        memset( temp_str,   0x00, sizeof(temp_str)   );
                                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                                          ReportTime,
                                                                         &ForceReport,
                                                                          temp_str );
                                        posds_log_message(2,1,temp_str,"PinnacleMsgHandler");
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
                                              posds_log_message(2,1,temp_str,"PinnacleMsgHandler");
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
         posds_log_message(1,2,ErrorMsg,"PinnacleMsgHandler");
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
         posds_log_message(1,2,ErrorMsg,"PinnacleMsgHandler");
    	 result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
       posds_log_message(2,2,ErrorMsg,"PinnacleMsgHandler");
   }*/
   if(DB_reconnect == 1)
   {
	   dbcommon_disconnect (ErrorMsg);
	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
	   {
		   strcat (ErrorMsg, " DB Reconnection failed");
		   posds_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler");
			return (false);
	   }
	   else
	   {
		   posds_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler");
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
      printf   ( "%s", ErrorMsg );
      posds_log_message(1,3,ErrorMsg,"perform_startup");
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
      posds_log_message(1,3,"Failed to create XIPC queues","perform_startup");
	  pteipc_shutdown_multiple_instance_app();
      dbcommon_disconnect (ErrorMsg);
      return (false);
   }

   printf ("pteipc init successful\n");


   #ifdef USING_LOGGER_REPLY
      /* send restart command to TFLogger */
      if (SendRestartCmd (ErrorMsg) == false)
      {
         posds_log_message(1,3,"Error sending restart command to TFLogger","perform_startup");
         dstimer_shutdown ();
         //pteipc_shutdown_single_instance_app();
         pteipc_shutdown_multiple_instance_app();
         dbcommon_disconnect (ErrorMsg);
         return (false);
      }

      printf ("restart command sent to logger\n");

      #ifdef USING_CHECKPOINTS
         /* get the checkpoint record from the database */
         if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
         {
            posds_log_message(1,3,ErrorMsg,"perform_startup");
            dstimer_shutdown ();
            //pteipc_shutdown_single_instance_app();
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
      posds_log_message(2,2,Buffer,"perform_startup");
   }
   else if ( ds_timer_flag == DS_TIMER_ON )
   {
      sprintf( Buffer,
              "%s:Data Server timing to flag long queries is turned on, value is %d",
               AppName, ds_timer_value );
      posds_log_message(2,1,Buffer,"perform_startup");
   }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   posds_log_message(2,1,Buffer,"perform_startup");

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
   posds_log_message(2,3,Buffer,"TrapFunction");
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
   INT 		ret_val = 0;

   GetAppName( AppName );
   ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                  ServiceName,     Buffer );

   #ifdef _DEBUG
      pteipc_trace_on();
   #endif
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
	posds_log_message(2,1,Buffer,"MainProcessor");
  	printf( "%s\n", Buffer );
   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
		  posds_log_message(2,1,Buffer,"MainProcessor");
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
         posds_log_message(2,2,Buffer,"MainProcessor");
      }
   }

   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   posds_log_message(2,1,Buffer,"MainProcessor");
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
         posds_log_message(1,2,Buffer,"MainProcessor");
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
            posds_log_message(1,3,Buffer,"MainProcessor");
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
         posds_log_message(2,3,XipcErrBuffer,"MainProcessor");
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
            posds_log_message(2,1,Buffer,"MainProcessor");
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
   posds_log_message(2,1,Buffer,"EndProcess");
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
   CHAR  posds_error_warning_file_size[5] = {0};

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
							  posds_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(posds_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(posds_error_warning_file_size);
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

	/* Get system time stamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	strcpy(Buffer,time_date);
   /* Pick out the month and day to compare to filename extension. */
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
/******************************************************************************
 *
 *  NAME:         posds_log_message
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
void posds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func)
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
   posds_Log_error_warning_to_File(text_message,sev,func);
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

INT posds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system time stamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extension. */
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
	if((fp = fopen(DB_Logging_Filename,"a+b"))== NULL)
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


/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/
