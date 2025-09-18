/******************************************************************************

   Module: netds.c

   Title: Network Controller Dataserver Routines

   Description: 

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\netds\nc_ds.c  $

      Rev 1.48   Jun 07 2007  DIrby
   Updated version to 4.4.1.5
   SCR 12299

      Rev 1.47   Mar 08 2007 10:30:22   DIrby
   Fixed bug when checking subtype 2 on a ping; changed '==' to '!='.
   SCR 24081
   
      Rev 1.46   Mar 02 2007 11:06:48   DIrby
   Added support for every SQL to be stored into shared memory
   in table SQL01 prior to being executed.
   SCR 24081
   
      Rev 1.45   Jul 07 2006 13:48:28   dirby
   Implemented functionality to gather statistics on database SQL calls.
   SCR 12299
   
      Rev 1.41   May 27 2004 17:16:16   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.40   Apr 10 2003 10:14:00   dirby
   Updated version to 4.2.0.2
   SCR 485
   
      Rev 1.39   Apr 09 2003 11:50:30   svelaga
   SCR 485: Multiple monitor prevention fix.
   
      Rev 1.38   Feb 14 2003 17:30:30   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.37   Aug 16 2002 17:28:12   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.36   Jul 18 2002 15:06:12   dirby
   Added functionality to support timing of queries and reporting
   on any that take too long.  This is configurable in TF.INI.
   SCR 818
   
      Rev 1.35   Jul 09 2002 14:25:02   dirby
   1.  Added code to get NCF30 record for Voice txns.
   2.  Updated version to 4.0.3.2
   SCR 810
   
      Rev 1.34   Jul 03 2002 14:33:54   dirby
   Updated version to 4.0.3.1
   SCR 803
   
      Rev 1.33   May 17 2002 11:25:28   dirby
   Updated version to 4.0.2.1
   Added functionality to display version when pinged.
   SCR 623
   
      Rev 1.32   Nov 03 2001 11:32:38   dirby
   Updated version to 4.0.0.1
   
      Rev 1.30   Jun 08 2001 13:19:56   SYARLAGA
   Modification  in  Process_alldata_msg,rollback, when any one of the query is failed ( either NCF21 / NCF30 )
   
      Rev 1.29   Jan 09 2001 14:16:44   dirby
   Updated version number.  This marks the release where the production
   stability issue has been resolved.  Some debug code has been removed and
   added #ifndef around some Unix signal code so the data server will compile
   on NT.
   
      Rev 1.28   Oct 16 2000 12:26:06   dirby
   1.  Updated version number to 3.2.2.2 - TEST.
   2.  Added code to support XipcSystemErrReport in the event of XIPC problems.
   3.  Removed catching of OS signals CHILD and HangUP so the daemon process can catch them.
   
      Rev 1.27   Oct 05 2000 15:05:36   dirby
   Updated the version number to '3.2.2.1 - TEST'.
   This version is compiled with more diagnostic stuff
   in daemon.c.
   
      Rev 1.26   Sep 01 2000 16:19:38   dirby
   Added TrapFunction to catch Unix signals.  This will not compile under NT.
   
      Rev 1.25   Aug 31 2000 17:30:16   dirby
   Appended "TEST" to the version number to account for some
   daemon testing getting linked in.
   
      Rev 1.24   Jul 30 2000 15:37:06   dirby
   Modified to display version number at startup and shutdown.
   
      Rev 1.23   Jun 24 2000 23:32:36   dirby
   Added function 'db_select_ncf30_by_rrn_and_terminal'.
   
      Rev 1.22   Feb 14 2000 16:14:58   svelaga
   Making sure that we update right transactions
   in NCF30 with new batch number, after updating
   NCF21 with new batch number.
   
   
      Rev 1.21   Feb 04 2000 11:05:28   svelaga
   Modified process_update_msg() changed to increment the batch_number. 
   
   
      Rev 1.20   Feb 03 2000 21:22:14   ddabberu
   1.process_alldata_msg() changed to correct the totals
   2.process_update_msg() changed to increment the batch_number
   of ncf21 till the 'ACTIVE' batch updated to the batch number.
   
      Rev 1.19   Feb 03 2000 18:40:30   ddabberu
   grand_totals ,
   
      Rev 1.18   Feb 02 2000 15:32:28   ddabberu
   process_alldata_msg modified to correct the totals
   
      Rev 1.17   Feb 01 2000 14:49:18   svelaga
   Rollback when any one of the updating / inserting 
   queries failed for ALL_DATA message.
   
   
      Rev 1.16   Nov 18 1999 10:45:44   dirby
   Added function to select neg01 by card number.
   
      Rev 1.15   20 Aug 1999 13:47:28   dirby
   Added reversal amount to SAF01.
   
      Rev 1.14   20 Aug 1999 10:20:32   dirby
   Modified function 'copy_auth_tx_to_saf01' to use 'orig...' fields.
   
      Rev 1.13   19 Aug 1999 14:32:36   dirby
   Modified function insert_saf01 so the input is from auth_tx.
   Added a function to copy data from auth_tx into saf01 data structure.
   
      Rev 1.12   19 Aug 1999 12:43:04   dirby
   Added function 'db_select_ncf30_by_merchant_id'.
   
      Rev 1.11   18 Aug 1999 14:01:52   dirby
   1.  Added functionality to select records from NCF30
        by stan, by device Id, and by card number.
   2.  Added currency_code, acquiring_inst_id_code,
        settlement_date, and transmission_timestamp
        to NCF30.
   
      Rev 1.10   Aug 17 1999 15:13:50   egable
   Make netds a multi instance app and remove
   the timer code.
   
      Rev 1.9   17 Aug 1999 10:22:00   dirby
   Added case statements and functions to process database table
   SAF01 for Store and Forward processing.
   
      Rev 1.8   03 Aug 1999 15:54:24   dirby
   Added function 'process_alldata_delete_msg'.  This function
   is used to delete a record in NCF30 (due to a reversal) and
   update NCF21 accordingly.  Use subtype1 of ST1_DB_DELETE
   and data type of ALL_DATA.
   
      Rev 1.7   02 Aug 1999 14:55:56   dirby
   Reworked the way ALL_DATA determines debit vs credit.
   If position 3 of processing code is 1 or 2, it is considered debit.
   
      Rev 1.6   Apr 09 1999 16:54:02   iarustam
   Added ncf_service_list
   
      Rev 1.5   04 Mar 1999 17:11:50   dirby
   Updated for AIX port.
   
      Rev 1.4   19 Feb 1999 14:42:00   dirby
   Modified 'process_update_msg' for NCF21_DATA
   to put a newly calculated batch number into ncf21_atp.
   This way the new batch number gets sent back to the originator.
   
      Rev 1.3   18 Feb 1999 15:04:04   dirby
   Made modifications to update NCF21_DATA so that
   2 structures will be returned to originator.
   
      Rev 1.2   17 Feb 1999 12:14:12   dirby
   1.  Date-Time generation function was only returning the year.
   2.  In function 'process_alldata_msg', added statement to credit-debit
        determination to make default account a credit.
   3.  In same function, corrected some bugs in calculating grand total.
   
      Rev 1.1   12 Feb 1999 09:16:02   dirby
   Stripped out none network controller functions.
   Added functionality for tables NCF21, NCF30, NEG01, and ATM01.
   
      Rev 1.0   25 Jan 1999 15:19:54   jhapitas
   Initial Revision
   
      Rev 1.0   17 Dec 1998 17:18:38   jhapitas
   Initial Equitable Revision

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <signal.h>

#include "basictyp.h"
#include "app_info.h"
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
#include "nc_dbstruct.h"
#include "nc_database.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"

#define AMEX03A
/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;

/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
/* End   */
/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;
INT LOOKBACK_DAYS =0;
char last_date[9]={0};
UINT Max_File_Size_Defined = 0 ;

/*Check null*/
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"
INT DB_NullCheck_Flag;

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename( );

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
void create_system_date( pBYTE system_date ) ;
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
void netds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);

INT netds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);

#define ST2_MONITOR_ALREADY_RUNNING 1
#define ST2_MONITOR_NOT_RUNNING 2
#define ST2_MONITOR_OVERRIDE 3
#define ST2_MONITOR_RECONNECT 4
#define ST2_MONITOR_READ_ONLY 5	// Read only monitor connection - used as a response subtype2

#define ST2_MONITOR_CLEARED	6
#define ST1_MONITOR_CLEAR_SOCKETS	101
#define MT_MONITOR_CLEAR_SOCKETS	100

#define COMMA ','

#define MONITORSUPPORTTABLE "MonSupportTab"
#define MONITORUSERTABLE "MonitorUserTable"
#define MONITORUSERTABLESIZE 21

#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

void TrapFunction( INT signum );

PRIVATE union
{
  NCF01   ncf01;
  NCF02   ncf02;
  NCF20   ncf20;
  NCF21   ncf21;
  NCF30   ncf30;
  NEG01   neg01;
  ATM01   atm01;
  SAF01   saf01;
  FG01    fg01;
  TLF01   tlf01;
  OBS01	  obs01;
  VISA3DS2 visa3ds2;
  NCF01_KEYBLOCK Ncf01_keyblock;
} db_recs;

PRIVATE union
{
   NCF01_GUI_LIST      Ncf01_Gui_List;
   NCF01_LIST          Ncf01_List;
   NCF02_GUI_LIST      Ncf02_Gui_List;
   NCF30_BATCH_LIST    Ncf30_List;
   FG01_GUI_LIST       Fg01_Gui_List;
   OBS01_GUI_LIST      OBS01_Gui_List;
   VISA3DS2_GUI_LIST   VISA3DS2_Gui_List;
   NCF01_KEYBLOCK_GUI_LIST Ncf01_Keyblock_Gui_List;
}db_lists;

MT_USER_INFO sMonitorUserInfo;

PRIVATE BOOLEAN      Ncf01InMem = false;
PRIVATE BOOLEAN      TranDetailInMem = false;
PRIVATE LONG         MemSize;
PRIVATE LONG         MemAvailable;

PRIVATE BYTE         MsgSubtype2;
PRIVATE BYTE         MsgSubtype1;
PRIVATE BYTE         result_code;
PRIVATE BOOLEAN      updates_to_db_made = false;
PRIVATE CHAR         ErrorMsg [100];
        CHAR         AppName   [MAX_APP_NAME_SIZE];
PRIVATE CHAR         last_trid [MAX_TRAN_ID_SIZE];
PRIVATE pCHAR        reply_que;
PRIVATE CHAR         orig_comm_inf[MAX_ORIG_INFO_SIZE];
PRIVATE CHECKPOINT   checkpoint_rec;
PRIVATE CHAR         AtpBuffer[30000];
PRIVATE BYTE         app_data_type;
PRIVATE CHAR         applnk_que_name[8]="applnkA";

BOOLEAN   in_refeed_mode    = false;
CHAR Version[] = "ATP_11.12.0";

INT ds_timer_flag;
INT ds_timer_value;

NCF21   ncf21;

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


/*** Monitor Support Table functions *****/
LONG WriteMemMonSupportTable(  pBYTE data)
{
   XINT Rt, Ret, result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   
   Rt = MemAccess(MONITORSUPPORTTABLE);
   if(Rt < 0)
      return(Rt);

   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   Ret = MemWrite(Rt, Current_Offset + 1, strlen((char *)data), data, MEM_WAIT);

   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   return(0L);
}

XINT ReadMemMonSupportTable(pCHAR MonitorMemAdd)
{

   XINT           Rt, Ret;
   CHAR strTemp[MAX_ORIG_INFO_SIZE+1] = {0} ;
 
   Rt = MemAccess(MONITORSUPPORTTABLE);
   if(Rt < 0)
      return(MEMACCESSERROR);
   Ret = MemRead(Rt, 0L, MAX_ORIG_INFO_SIZE + 1, strTemp, MEM_WAIT);
   strcpy (MonitorMemAdd, strTemp+1) ;
   return(0);
      
}
LONG WriteMemMonUserTable(  pBYTE data)
{
   XINT Rt, Ret, result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   
   Rt = MemAccess(MONITORUSERTABLE);
   if(Rt < 0)
      return(Rt);

   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   Ret = MemWrite(Rt, Current_Offset + 1, strlen((char *)data), data, MEM_WAIT);

   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   return(0L);
}

XINT ReadMemMonUserTable(pCHAR MonitorMemAdd)
{

   XINT           Rt, Ret;
   CHAR strTemp[MONITORUSERTABLESIZE+1] = {0} ;
 
   Rt = MemAccess(MONITORUSERTABLE);
   if(Rt < 0)
      return(MEMACCESSERROR);
   Ret = MemRead(Rt, 0L, MONITORUSERTABLESIZE+ 1, strTemp, MEM_WAIT);
   strcpy (MonitorMemAdd, strTemp+1) ;
   return(0);
      
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
 *  MODIFICATIONS:
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

} /* genutil_format_date */

/******************************************************************************
 *
 *  NAME:         process_insert_msg
 *
 *  DESCRIPTION:  This function inserts a record into a network table.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_insert_msg( pPTE_MSG p_msg_in )
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
   CHAR   buffer[526]= {0};

   switch( app_data_type )
   {
      case NCF01_DATA:

         /* retrieve the data into a ncf01 structure and insert into database */
         memset( &db_recs.ncf01, 0, sizeof( db_recs.ncf01 ) );
         memcpy( &db_recs.ncf01, ( pNCF01)p_data, sizeof( NCF01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF01 (&db_recs.ncf01,MODE_INSERT);
		  }
         result_code = db_insert_ncf01( &db_recs.ncf01, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
          /* NewLog, Insert failed into NCF01 */
        	 memset(buffer,0x00,sizeof(buffer));
        	 sprintf(buffer,"Insert into ncf01 table failed, network id:%s, Network type:%s",
        			 db_recs.ncf01.primary_key.network_id,db_recs.ncf01.primary_key.network_type);
        	 netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case NCF02_DATA:

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.ncf02, 0, sizeof( db_recs.ncf02 ) );
         memcpy( &db_recs.ncf02, ( pNCF02)p_data, sizeof( NCF02 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF02 (&db_recs.ncf02,MODE_INSERT);
		  }
         result_code = db_insert_ncf02( &db_recs.ncf02, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Insert failed into NCF02 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Insert into ncf02 table failed, network id:%s,Network type:%s",
            		  db_recs.ncf02.primary_key.network_id,db_recs.ncf02.primary_key.network_type);
              netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case NCF20_DATA:

         /* retrieve the data into a ncf20 structure and insert into database */
         memset( &db_recs.ncf20, 0, sizeof( db_recs.ncf20 ) );
         memcpy( &db_recs.ncf20, ( pNCF20)p_data, sizeof( NCF20 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF20 (&db_recs.ncf20,MODE_INSERT);
		  }
         result_code = db_insert_ncf20( &db_recs.ncf20, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
              /* NewLog, Insert failed into NCF20 */
               memset(buffer,0x00,sizeof(buffer));
               sprintf(buffer,"Insert into ncf20 table failed, network id:%s",db_recs.ncf20.primary_key.network_id);
               netds_log_message(2,2,buffer,"process_insert_msg");

          }
         break;

      case NCF21_DATA:

         /* retrieve the data into a ncf21 structure and insert into database */
         memset( &db_recs.ncf21, 0, sizeof( db_recs.ncf21 ) );
         memcpy( &db_recs.ncf21, ( pNCF21)p_data, sizeof( NCF21 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF21 (&db_recs.ncf21,MODE_INSERT);
		  }
         result_code = db_insert_ncf21( &db_recs.ncf21, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Insert failed into NCF21 */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Insert into ncf21 table failed,network id:%s,Network type:%s,batch number:%s",
            		 db_recs.ncf21.primary_key.network_id,db_recs.ncf21.primary_key.network_type,db_recs.ncf21.primary_key.batch_number);
             netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case NCF01_KEYBLOCK01_DATA:

         /* retrieve the data into a ncf01_keyblock structure and insert into database */
         memset( &db_recs.Ncf01_keyblock, 0, sizeof( db_recs.Ncf01_keyblock ) );
         memcpy( &db_recs.Ncf01_keyblock, ( pNCF01_KEYBLOCK)p_data, sizeof( NCF01_KEYBLOCK ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF01_Keyblock (&db_recs.Ncf01_keyblock,MODE_INSERT);
		  }
         result_code = db_insert_ncf01_keyblock( &db_recs.Ncf01_keyblock, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Insert failed into NCF01_KEYBLOCK */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Insert into ncf01_keyblock table failed,Network id:%s",
            		 db_recs.Ncf01_keyblock.network_id);
             netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case NCF30_DATA:

         /* retrieve the data into a ncf30 structure and insert into database */
         memset( &db_recs.ncf30, 0, sizeof( db_recs.ncf30 ) );
         memcpy( &db_recs.ncf30, ( pNCF30)p_data, sizeof( NCF30 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF30 (&db_recs.ncf30,MODE_INSERT);
		  }
         result_code = db_insert_ncf30( &db_recs.ncf30, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
            /* NewLog, Insert failed into NCF30 */
            memset(buffer,0x00,sizeof(buffer));
            sprintf(buffer,"Insert into ncf30 table failed, batch num:%s,Retrieval ref num:%s, network id:%s",
            		db_recs.ncf30.primary_key.batch_number,db_recs.ncf30.primary_key.retrieval_ref_num,db_recs.ncf30.primary_key.network_id);
            netds_log_message(2,2,buffer,"process_insert_msg");

                  }
         break;

      case NEG01_DATA:

         /* retrieve the data into a neg01 structure and insert into database */
         memset( &db_recs.neg01, 0, sizeof( db_recs.neg01 ) );
         memcpy( &db_recs.neg01, ( pNEG01)p_data, sizeof( NEG01 ) );
         result_code = db_insert_neg01( &db_recs.neg01, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Insert failed into NEG01 */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Insert into NEG01 table failed, card num::%s",db_recs.neg01.card_nbr);
             netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case ATM01_DATA:

         /* retrieve the data into a atm01 structure and insert into database */
         memset( &db_recs.atm01, 0, sizeof( db_recs.atm01 ) );
         memcpy( &db_recs.atm01, ( pATM01)p_data, sizeof( ATM01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinATM01 (&db_recs.atm01,MODE_INSERT);
		  }
         result_code = db_insert_atm01( &db_recs.atm01, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
            /* NewLog, Insert failed into ATM01 */
            memset(buffer,0x00,sizeof(buffer));
            sprintf(buffer,"Insert into atm01 table failed, Terminal id:%s",db_recs.atm01.primary_key.termnumr);
            netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case SAF01_DATA:

         /* retrieve the data into a saf01 structure and insert into database */
         memset( &db_recs.saf01, 0, sizeof( db_recs.saf01 ) );
         copy_auth_tx_to_saf01( (pAUTH_TX)AtpBuffer, &db_recs.saf01 );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinSAF01 (&db_recs.saf01,MODE_INSERT);
		  }
         result_code = db_insert_saf01( &db_recs.saf01, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
            /* NewLog, Insert failed into SAF01 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Insert into saf01 table failed,card num:%s,device id:%s,Merchant id:%s,STAN:%s",
                 			 db_recs.saf01.card_num,db_recs.saf01.device_id,db_recs.saf01.merchant_id,db_recs.saf01.sys_trace_audit_nbr);
              netds_log_message(2,2,buffer,"process_insert_msg");

         }
         break;

      case FG01_DATA:

         /* Retrieve the data into an fg01 structure and insert into database */
         memset( &db_recs.fg01, 0, sizeof( db_recs.fg01 ) );
         memcpy( &db_recs.fg01, ( pFG01)p_data, sizeof( FG01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinFG01 (&db_recs.fg01,MODE_INSERT);
		  }
         result_code = db_insert_fg01( &db_recs.fg01, ErrorMsg );
		          if(result_code != PTEMSG_OK)
         {
            /* NewLog, Insert failed into fg01 */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Insert into fg01 failed, Username:%s",db_recs.fg01.username);
             netds_log_message(2,2,buffer,"process_insert_msg");

         }

         break;

      case OBS01_DATA:

         /* Retrieve the data into an obs01 structure and insert into database */
         memset( &db_recs.obs01, 0, sizeof( db_recs.obs01 ) );
         memcpy( &db_recs.obs01, ( pFG01)p_data, sizeof( OBS01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinOBS01 (&db_recs.obs01,MODE_INSERT);
		  }
         result_code = db_insert_obs01( &db_recs.obs01, ErrorMsg );
         break;

      case VISA3DS2_DATA:

               /* Retrieve the data into an visa3ds2 structure and insert into database */
               memset( &db_recs.visa3ds2, 0, sizeof( db_recs.visa3ds2 ) );
               memcpy( &db_recs.visa3ds2, ( pVISA3DS2)p_data, sizeof( VISA3DS2 ) );
      		  if (DB_NullCheck_Flag==1)
      		  {
      			  CheckNULLTerminatedinVISA3DS2 (&db_recs.visa3ds2,MODE_INSERT);
      		  }
               result_code = db_insert_visa3ds2( &db_recs.visa3ds2, ErrorMsg );
               if(result_code != PTEMSG_OK)
               {
                    /* NewLog, Insert failed into visa3ds2 */
                    memset(buffer,0x00,sizeof(buffer));
                    sprintf(buffer,"Insertion of visa3ds2 failed, cavv:%s,response_code:%s",
                                    		 db_recs.visa3ds2.cavv,db_recs.visa3ds2.response_code);
                     netds_log_message(2,2,buffer,"process_insert_msg");

               }

         break;

     default:

         /* Unknown app_data_type received.
          * Log Event and return error message to client.
          */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf( ErrorMsg, "Unknown app_data_type for Insert request from %s",
                  reply_que);
         /* NewLog, Logging */
         netds_log_message(2,2,ErrorMsg,"process_insert_msg");
         break;
   }

   if (result_code == PTEMSG_OK)
   {
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1(p_msg_in),
                                    ptemsg_get_msg_subtype2(p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf(ErrorMsg, "Insufficient Memory to build Insert response to %s",
                 reply_que);
      }
   }

   free(p_msg_data);
   return( p_msg_out );
}

/******************************************************************************
 *
 *  NAME:         process_update_msg
 *
 *  DESCRIPTION:  This function updates a record in a network table.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 *      DI - 02/18/99 - Made mods to NCF21 to keep and return 2 structures
 *                      to the originator.
 *      DI - 02/19/99 - Added strcpy to put newly calculated batch number
 *                      into ncf21_atp.  This way the new batch number gets
 *                      sent back to the originator.
 ******************************************************************************/
pPTE_MSG process_update_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   LONG           data_length;
   INT            batch_num_len;
   CHAR           BatchNumber[7];
   CHAR           NetworkId[11];
   CHAR           NetworkType[2];
   CHAR           date_time[16] = "";
   NCF21          ncf21_host;
   NCF21          ncf21_atp;
   CHAR			  OpenDate[9];
   CHAR			  OpenTime[7];

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = p_data +  sizeof(AUTH_TX);
   data_length   = sizeof(AUTH_TX);
   CHAR   buffer[526]= {0};

   switch( app_data_type )
   {

      case NCF01_DATA:

         /* Retrieve the data into a ncf01 structure and update the database */
         memset( &db_recs.ncf01, 0, sizeof( db_recs.ncf01 ) );
         memcpy( &db_recs.ncf01, ( pNCF01 )p_data, sizeof( NCF01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF01 (&db_recs.ncf01,MODE_UPDATE);
		  }
         result_code = db_update_ncf01( &db_recs.ncf01, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
             /* NewLog, Update failed into NCF01 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Update into ncf01 table failed, network id:%s, Network type:%s",
                 			 db_recs.ncf01.primary_key.network_id,db_recs.ncf01.primary_key.network_type);
              netds_log_message(2,2,buffer,"process_update_msg");

          }
         break;

      case NCF02_DATA:

         /* Retrieve the data into a ncf02 structure and update the database */
         memset( &db_recs.ncf02, 0, sizeof( db_recs.ncf02 ) );
         memcpy( &db_recs.ncf02, ( pNCF02 )p_data, sizeof( NCF02 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF02 (&db_recs.ncf02,MODE_UPDATE);
		  }
         result_code = db_update_ncf02( &db_recs.ncf02, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
              /* NewLog, Update failed into NCF02 */
               memset(buffer,0x00,sizeof(buffer));
               sprintf(buffer,"Update into ncf02 table failed, network id:%s,Network type:%s",
                     		  db_recs.ncf02.primary_key.network_id,db_recs.ncf02.primary_key.network_type);
               netds_log_message(2,2,buffer,"process_update_msg");

          }
         break;

      case NCF20_DATA:

         /* Retrieve the data into a ncf20 structure and update the database */
         memset( &db_recs.ncf20, 0, sizeof( db_recs.ncf20 ) );
         memcpy( &db_recs.ncf20, ( pNCF20 )p_data, sizeof( NCF20 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF20 (&db_recs.ncf20,MODE_UPDATE);
		  }
         result_code = db_update_ncf20( &db_recs.ncf20, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
                /* NewLog, Update failed into NCF20 */
                memset(buffer,0x00,sizeof(buffer));
                sprintf(buffer,"Update into ncf20 table failed, network id:%s",db_recs.ncf20.primary_key.network_id);
                netds_log_message(2,2,buffer,"process_update_msg");

          }
         break;

      case NCF21_DATA:

         /* Save this data to return to originator. */
         memset( &ncf21_host, 0x00, sizeof(NCF21) );
         memset( &ncf21_atp,  0x00, sizeof(NCF21) );
         memcpy( &ncf21_host, AtpBuffer, sizeof(NCF21) );
         memcpy( &ncf21_atp,  p_data, sizeof(NCF21) );

         /* Close a batch */
         memset( &db_recs.ncf21, 0, sizeof( db_recs.ncf21 ) );
         memcpy( &ncf21, p_data, sizeof( NCF21 ) );

         /* Get last batch number */
         result_code = db_select_max_batch_ncf21( (pCHAR)p_data,
                                                   BatchNumber, ErrorMsg );
         if ( PTEMSG_OK == result_code )
         {
            /* Increment batch number by 1. */
            batch_num_len = atol( BatchNumber );
            batch_num_len++;
            if ( 999999 < batch_num_len )
               batch_num_len = 1;
            
            
            
            sprintf( BatchNumber, "%06d", batch_num_len );
            strcpy( ncf21.primary_key.batch_number, BatchNumber );
            strcpy( ncf21_atp.primary_key.batch_number, BatchNumber );

            /* Get system date and time. */
            genutil_format_date( date_time );
            strncpy(ncf21.release_date ,date_time, 8);
            strncpy(ncf21.release_time ,date_time + 8, 6);

            /* Update batch number and release time in NCF21. */
  		  if (DB_NullCheck_Flag==1)
  		  {
  			  CheckNULLTerminatedinNCF21 (&ncf21,MODE_UPDATE);
  		  }
            result_code = db_update_ncf21_batch_number(&ncf21,ErrorMsg);
            if ( PTEMSG_OK == result_code )
            {
               /* Update batch number in NCF30. */
               strcpy( NetworkId,   ncf21.primary_key.network_id   );
               strcpy( NetworkType, ncf21.primary_key.network_type );
			   strcpy( OpenDate, ncf21.open_date);
			   strcpy( OpenTime, ncf21.open_time);
               result_code = db_update_ncf30_batch_nbr(NetworkId,NetworkType,
                                                       BatchNumber,OpenDate,OpenTime, ErrorMsg);
               if(result_code != PTEMSG_OK)
               {
                  /* NewLog, Update of batch num into NCF21 is successful, but insert record into NCF30 table failed */
                  memset(buffer,0x00,sizeof(buffer));
                  sprintf(buffer,"Update of batch num into NCF21 is successful, but insert record into NCF30 table failed, batch num:%s,card num:%s,device id:%s,merchant id:%s,STAN:%s, network id:%s",
                  db_recs.ncf30.primary_key.batch_number,db_recs.ncf30.card_nbr,db_recs.ncf30.device_id,db_recs.ncf30.merchant_id,db_recs.ncf30.sys_trace_audit_nbr,db_recs.ncf30.primary_key.network_id);
                  netds_log_message(2,2,buffer,"process_update_msg");

               }

            }
            else
            {
            	/* NewLog, Update of batch number failed into NCF21 */
            	  memset(buffer,0x00,sizeof(buffer));
            	  sprintf(buffer,"Update of batch number into ncf21 table failed,network id:%s,Network type:%s,batch number:%s",
            	            		 db_recs.ncf21.primary_key.network_id,db_recs.ncf21.primary_key.network_type,db_recs.ncf21.primary_key.batch_number);
            	  netds_log_message(2,2,buffer,"process_update_msg");


            }
		
         }

         /* Copy original NCF21 with new batch number. */
         memcpy( AtpBuffer, &ncf21_host, sizeof(NCF21) );
         memcpy( AtpBuffer + sizeof(AUTH_TX), &ncf21_atp, sizeof(NCF21) );
         data_length += sizeof(NCF21);
         break;

      case NCF30_DATA:

         /* Retrieve the data into a ncf30 structure and update the database */
         memset( &db_recs.ncf30, 0, sizeof( db_recs.ncf30 ) );
         memcpy( &db_recs.ncf30, ( pNCF30 )p_data, sizeof( NCF30 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF30 (&db_recs.ncf30,MODE_UPDATE);
		  }
         result_code = db_update_ncf30( &db_recs.ncf30, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
            /* NewLog, Update failed into NCF30 */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Update into ncf30 table failed, batch num:%s,card num:%s,device id:%s,merchant id:%s,STAN:%s, network id:%s",
            db_recs.ncf30.primary_key.batch_number,db_recs.ncf30.card_nbr,db_recs.ncf30.device_id,db_recs.ncf30.merchant_id,db_recs.ncf30.sys_trace_audit_nbr,db_recs.ncf30.primary_key.network_id);
             netds_log_message(2,2,buffer,"process_update_msg");

         }
                  break;
         break;

      case NCF01_KEYBLOCK01_DATA:

         /* Retrieve the data into a ncf01_keyblock structure and update the database */
         memset( &db_recs.Ncf01_keyblock, 0, sizeof( db_recs.Ncf01_keyblock ) );
         memcpy( &db_recs.Ncf01_keyblock, ( pNCF01_KEYBLOCK )p_data, sizeof( NCF01_KEYBLOCK ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF01_Keyblock (&db_recs.Ncf01_keyblock,MODE_UPDATE);
		  }
         result_code = db_update_ncf01_keyblock( &db_recs.Ncf01_keyblock, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
                /* NewLog, Update failed into NCF01_KEYBLOCK */
                memset(buffer,0x00,sizeof(buffer));
                sprintf(buffer,"Update into ncf01_keyblock table failed, network id:%s",db_recs.Ncf01_keyblock.network_id);
                netds_log_message(2,2,buffer,"process_update_msg");

          }
         break;
      case NEG01_DATA:

         /* Retrieve the data into a neg01 structure and update the database */
         memset( &db_recs.neg01, 0, sizeof( db_recs.neg01 ) );
         memcpy( &db_recs.neg01, ( pNEG01 )p_data, sizeof( NEG01 ) );
         result_code = db_update_neg01( &db_recs.neg01, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Update failed into NEG01 */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Update into NEG01 table failed, card num::%s",db_recs.neg01.card_nbr);
             netds_log_message(2,2,buffer,"process_update_msg");

         }
         break;

      case ATM01_DATA:

         /* Retrieve the data into a atm01 structure and update the database */
         memset( &db_recs.atm01, 0, sizeof( db_recs.atm01 ) );
         memcpy( &db_recs.atm01, ( pATM01 )p_data, sizeof( ATM01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinATM01 (&db_recs.atm01,MODE_UPDATE);
		  }
         result_code = db_update_atm01( &db_recs.atm01, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
             /* NewLog, Update failed into ATM01 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Update into atm01 table failed, Terminal id:%s",db_recs.atm01.primary_key.termnumr);
              netds_log_message(2,2,buffer,"process_update_msg");

         }
         break;

      case SAF01_DATA:

         /* Retrieve the data into a saf01 structure and update the database */
         memset( &db_recs.saf01, 0, sizeof( db_recs.saf01 ) );
         memcpy( &db_recs.saf01, ( pSAF01 )p_data, sizeof( SAF01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinSAF01 (&db_recs.saf01,MODE_UPDATE);
		  }
         result_code = db_update_saf01( &db_recs.saf01, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Update failed into SAF01 */
             memset(buffer,0x00,sizeof(buffer));
             sprintf(buffer,"Update into saf01 table failed,card num:%s,device id:%s,Merchant id:%s,STAN:%s",
                          			 db_recs.saf01.card_num,db_recs.saf01.device_id,db_recs.saf01.merchant_id,db_recs.saf01.sys_trace_audit_nbr);
             netds_log_message(2,2,buffer,"process_update_msg");

         }
         break;

      case ALL_DATA:
         result_code = process_alldata_msg(p_msg_in );                 
         break;

      case FG01_DATA:

         /* Retrieve the data into an fg01 structure and update the database */
         memset( &db_recs.fg01, 0, sizeof( db_recs.fg01 ) );
         memcpy( &db_recs.fg01, ( pFG01 )p_data, sizeof( FG01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinFG01 (&db_recs.fg01,MODE_UPDATE);
		  }
         result_code = db_update_fg01( &db_recs.fg01, ErrorMsg );
		          if(result_code != PTEMSG_OK)
         {
             /* NewLog, Update failed into fg01 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Update into fg01 failed, Username:%s",db_recs.fg01.username);
              netds_log_message(2,2,buffer,"process_update_msg");

         }
         break;

      case OBS01_DATA:

          memset( &db_recs.obs01, 0, sizeof( db_recs.obs01 ) );
          memcpy( &db_recs.obs01, ( pOBS01 )p_data, sizeof( OBS01 ) );
 		  if (DB_NullCheck_Flag==1)
 		  {
 			  CheckNULLTerminatedinOBS01 (&db_recs.obs01,MODE_UPDATE);
 		  }
          result_code = db_update_obs01( &db_recs.obs01, ErrorMsg );
          break;

      case VISA3DS2_DATA:

                memset( &db_recs.visa3ds2, 0, sizeof( db_recs.visa3ds2 ) );
                memcpy( &db_recs.visa3ds2, ( pVISA3DS2)p_data, sizeof( VISA3DS2 ) );
       		  if (DB_NullCheck_Flag==1)
       		  {
       			  CheckNULLTerminatedinVISA3DS2(&db_recs.visa3ds2,MODE_UPDATE);
       		  }
                result_code = db_update_visa3ds2( &db_recs.visa3ds2, ErrorMsg );
                if(result_code != PTEMSG_OK)
                {
                     /* NewLog, Delete failed into visa3ds2 */
                     memset(buffer,0x00,sizeof(buffer));
                     sprintf(buffer,"Updating of visa3ds2 failed, cavv:%s,response_code:%s",
                                     		 db_recs.visa3ds2.cavv,db_recs.visa3ds2.response_code);
                     netds_log_message(2,2,buffer,"process_update_msg");

                }

         break;

      default:

         /* Unknown app_data_type received.
          * Log Event and return error message to client.
          */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf(ErrorMsg,"Unknown app_data_type: %s for Update request from %s",
                 app_data_type,reply_que);
         /* NewLog, Logging */
          netds_log_message(2,2,ErrorMsg,"process_insert_msg");

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
                                    data_length,
                                    app_data_type);

      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf(ErrorMsg, "Insufficient Memory to build Update response to %s",
                 reply_que);
      }
   }
   return( p_msg_out );
}

/******************************************************************************
 *
 *  NAME:         process_delete_msg
 *
 *  DESCRIPTION:  This function deletes a record from a network table.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_delete_msg( pPTE_MSG p_msg_in ) 
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
   p_data        = p_data + sizeof(AUTH_TX);
   CHAR   buffer[526]= {0};

   switch( app_data_type )
   {
      case NCF01_DATA:

         /* Retrieve the ncf01 name and delete record from the database */
         result_code = db_delete_ncf01( ( pNCF01 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
            /* NewLog, Delete failed into NCF01 */
            memset(buffer,0x00,sizeof(buffer));
            sprintf(buffer,"Deletion of ncf01 table failed, network id:%s, Network type:%s",
                          			 db_recs.ncf01.primary_key.network_id,db_recs.ncf01.primary_key.network_type);
            netds_log_message(2,2,buffer,"process_delete_msg");

         }
         break;

      case NCF02_DATA:

         /* Retrieve the clf01 name and delete record from the database */
         result_code = db_delete_ncf02( ( pNCF02 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
              /* NewLog, Delete failed into NCF02 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Deletion of ncf02 table failed, network id:%s,Network type:%s",
                             		  db_recs.ncf02.primary_key.network_id,db_recs.ncf02.primary_key.network_type);
              netds_log_message(2,2,buffer,"process_delete_msg");

           }
         break;

      case NCF20_DATA:

         /* Retrieve the ncf20 name and delete record from the database */
         result_code = db_delete_ncf20( ( pNCF20 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
              /* NewLog, Delete failed into NCF20 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Deletion of ncf20 table failed, network id:%s",db_recs.ncf20.primary_key.network_id);
              netds_log_message(2,2,buffer,"process_delete_msg");

          }
         break;

      case NCF21_DATA:

         /* Retrieve the ncf21 name and delete record from the database */
         result_code = db_delete_ncf21( ( pNCF21 )p_data, ErrorMsg );
         break;

      case NCF01_KEYBLOCK01_DATA:

         /* Retrieve the ncf01_keyblock table and delete record from the database */
         result_code = db_delete_ncf01_keyblock( ( pNCF01_KEYBLOCK )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
          {
              /* NewLog, Delete failed into NCF01_KEYBLOCK */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Deletion of ncf01_keyblock table failed, network id:%s",db_recs.Ncf01_keyblock.network_id);
              netds_log_message(2,2,buffer,"process_delete_msg");

          }
         break;

      case NCF30_DATA:

         /* Retrieve the ncf30 name and delete record from the database */
         result_code = db_delete_ncf30( ( pNCF30 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
            /* NewLog, Delete failed into NCF30 */
            memset(buffer,0x00,sizeof(buffer));
            sprintf(buffer,"Deletion of ncf30 table failed, batch num:%s,card num:%s,device id:%s,merchant id:%s,STAN:%s, network id:%s",
            		db_recs.ncf30.primary_key.batch_number,db_recs.ncf30.card_nbr,db_recs.ncf30.device_id,db_recs.ncf30.merchant_id,db_recs.ncf30.sys_trace_audit_nbr,db_recs.ncf30.primary_key.network_id);
            netds_log_message(2,2,buffer,"process_delete_msg");

         }
         break;

      case NEG01_DATA:

         /* Retrieve the neg01 name and delete record from the database */
         result_code = db_delete_neg01( ( pNEG01 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Delete failed into NEG01 */
        	   memset(buffer,0x00,sizeof(buffer));
               sprintf(buffer,"Deletion of NEG01 table failed, card num::%s",db_recs.neg01.card_nbr);
               netds_log_message(2,2,buffer,"process_delete_msg");

         }
         break;

      case ATM01_DATA:

         /* Retrieve the ncf01 name and delete record from the database */
         result_code = db_delete_atm01( ( pATM01 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
           /* NewLog, Delete failed into ATM01 */
        	 	memset(buffer,0x00,sizeof(buffer));
        	 	sprintf(buffer,"Deletion of atm01 table failed, Terminal id:%s",db_recs.atm01.primary_key.termnumr);
                netds_log_message(2,2,buffer,"process_delete_msg");

         }
         break;

      case SAF01_DATA:

         /* Retrieve the saf01 name and delete record from the database */
         result_code = db_delete_saf01( ( pSAF01 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
             /* NewLog, Delete failed into SAF01 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Deletion of saf01 table failed,card num:%s,device id:%s,Merchant id:%s,STAN:%s",
                                  			 db_recs.saf01.card_num,db_recs.saf01.device_id,db_recs.saf01.merchant_id,db_recs.saf01.sys_trace_audit_nbr);
              netds_log_message(2,2,buffer,"process_delete_msg");

         }
         break;

      case ALL_DATA:
         result_code = process_alldata_delete_msg(p_msg_in );                 
         break;

      case FG01_DATA:

         /* Retrieve the fg01 name and delete record from the database */
         result_code = db_delete_fg01( ( pFG01 )p_data, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
               /* NewLog, Delete failed into fg01 */
                memset(buffer,0x00,sizeof(buffer));
                sprintf(buffer,"Deletion of fg01 failed, Username:%s",db_recs.fg01.username);
                netds_log_message(2,2,buffer,"process_delete_msg");

         }
         break;

      case OBS01_DATA:
          result_code = db_delete_obs01( ( pFG01 )p_data, ErrorMsg );
          if(result_code != PTEMSG_OK)
          {
                /* NewLog, Delete failed into fg01 */
                 memset(buffer,0x00,sizeof(buffer));
                 sprintf(buffer,"Deletion of obs01 failed, on_behalf_service:%s,on_behalf_result:%s",
                		 db_recs.obs01.on_behalf_service,db_recs.obs01.on_behalf_result);
                 netds_log_message(2,2,buffer,"process_delete_msg");

          }
          break;

      case VISA3DS2_DATA:
               result_code = db_delete_visa3ds2( ( pVISA3DS2 )p_data, ErrorMsg );
               if(result_code != PTEMSG_OK)
               {
                     /* NewLog, Delete failed into visa3ds2 */
                      memset(buffer,0x00,sizeof(buffer));
                      sprintf(buffer,"Deletion of visa3ds2 failed, cavv:%s,response_code:%s",
                     		 db_recs.visa3ds2.cavv,db_recs.visa3ds2.response_code);
                      netds_log_message(2,2,buffer,"process_delete_msg");

               }
         break;

      default:

         /* Unknown app_data_type received.
          * Log Event and return error message to client.
          */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf( ErrorMsg, "Unknown app_data_type for Delete request from %s",
                  reply_que);
         /* NewLog, Logging */
         netds_log_message(2,2,ErrorMsg,"process_delete_msg");
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
         sprintf(ErrorMsg, "Insufficient Memory to build Delete response to %s",
                 reply_que);
      }
   }
   return( p_msg_out );
}

/******************************************************************************
 *
 *  NAME:         process_select_msg
 *
 *  DESCRIPTION:  This function will select a record from the database. The
 *                table name is in app_data_type, and the record keys are in
 *                p_data.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_select_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pAUTH_TX       auth_tx;
   BYTE           subtype2;
   char current_system_date[9]={0};

   /* Parse information from the PTE message. */
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   auth_tx       = (pAUTH_TX)p_data;
   p_data        = p_data + sizeof(AUTH_TX);

   /* Determine which data base table to hit. */
   switch( app_data_type )
   {
      case NCF01_DATA:

         if( PTEMSG_OK == ( result_code = db_select_ncf01( (pCHAR)p_data,
                                                           &db_recs.ncf01,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.ncf01, sizeof( db_recs.ncf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.ncf01),
                                          NCF01_DATA);

         }
         break;

      case NCF02_DATA:

         if ( 0 == ptemsg_get_msg_subtype2( p_msg_in ))
         {
            if( PTEMSG_OK == ( result_code = db_select_ncf02( (pCHAR)p_data,
                                                               &db_recs.ncf02,
                                                               ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf02, sizeof(db_recs.ncf02));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.ncf02),
                                          NCF02_DATA);

            }
         }
         break;

      case NCF20_DATA:

         if( PTEMSG_OK == ( result_code = db_select_ncf20( (pCHAR)p_data,
                                                           &db_recs.ncf20,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.ncf20, sizeof( db_recs.ncf20 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.ncf20),
                                          NCF20_DATA);

         }
         break;

      case NCF21_DATA:

         if( PTEMSG_OK == ( result_code = db_select_ncf21( (pCHAR)p_data,
                                                           &db_recs.ncf21,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.ncf21, sizeof( db_recs.ncf21 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.ncf21),
                                          NCF21_DATA);
         }
         break;

      case NCF01_KEYBLOCK01_DATA:

         if( PTEMSG_OK == ( result_code = db_select_ncf01_keyblock( (pCHAR)p_data,
                                                           &db_recs.Ncf01_keyblock,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.Ncf01_keyblock, sizeof( db_recs.Ncf01_keyblock ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.Ncf01_keyblock),
										  NCF01_KEYBLOCK01_DATA);
         }
         break;

      case NCF30_DATA:

         subtype2 = ptemsg_get_msg_subtype2 (p_msg_in);
         if ( ST2_NONE == subtype2 )
         {
            /* Select NCF30 record based on the table's keys. */
            if( PTEMSG_OK == ( result_code = db_select_ncf30( (pCHAR)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          subtype2,
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.ncf30),
                                          NCF30_DATA);
            }
         }
         else if ( ST2_DB_SELECT_TRAN_BY_STAN == subtype2 )
         {
        	/* Let us check if its a CUP transaction*/
        	if( 0 == strncmp(reply_que,"nccup",5))
        	{
        		if( PTEMSG_OK == (result_code = db_select_ncf30_by_stan_for_CUP( (pNCF30)p_data,
        																			&db_recs.ncf30,
																					ErrorMsg ) ) )
				{
				   memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
												 ptemsg_get_msg_subtype1 (p_msg_in),
												 subtype2,
												 reply_que,
												 application_que_name,
												 (pBYTE)AtpBuffer,
												 sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
												 NCF30_DATA);
				}
        	}
        	else if( PTEMSG_OK == (result_code = db_select_ncf30_by_stan( (pNCF30)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
                                             NCF30_DATA);
            }
         }
         else if ( ST2_DB_SELECT_TRAN_BY_TERMINAL_ID == subtype2 )
         {
            if( PTEMSG_OK == (result_code = db_select_ncf30_by_termid( (pNCF30)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
                                             NCF30_DATA);
            }
         }
         else if ( ST2_DB_SELECT_TRAN_BY_CARD_NUM == subtype2 )
         {
            if( PTEMSG_OK == (result_code = db_select_ncf30_by_card( (pNCF30)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
                                             NCF30_DATA);
            }
         }
         else if ( ST2_DB_SELECT_TRAN_BY_MERCHANT_ID == subtype2 )
         {
        	 /* Let us check if its a CUP transaction*/
			if( 0 == strncmp(reply_que,"nccup",5))
			{
				if( PTEMSG_OK == (result_code = db_select_ncf30_by_merchant_id_for_CUP( (pNCF30)p_data,
				                                                              &db_recs.ncf30,
				                                                              ErrorMsg ) ) )
					{
					   memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
					   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
													 ptemsg_get_msg_subtype1 (p_msg_in),
													 subtype2,
													 reply_que,
													 application_que_name,
													 (pBYTE)AtpBuffer,
													 sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
													 NCF30_DATA);
					}
			}
			else if( PTEMSG_OK == (result_code = db_select_ncf30_by_merchant_id( (pNCF30)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
                                             NCF30_DATA);
            }
         }
         else if ( ST2_DB_SELECT_NCF30_BY_RRN_AND_TERMINAL == subtype2 )
         {
            if( PTEMSG_OK == (result_code = db_select_ncf30_by_rrn_and_terminal( (pNCF30)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
                                             NCF30_DATA);
            }
         }
         else if ( ST2_DB_SELECT_NCF30_FOR_VOICE == subtype2 )
         {
            if( PTEMSG_OK == (result_code = db_select_ncf30_for_voice( (pNCF30)p_data,
                                                              &db_recs.ncf30,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             ptemsg_get_msg_subtype1 (p_msg_in),
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
                                             NCF30_DATA);
            }
         }
#ifdef AMEX03A
         else if(ST2_DB_SELECT_NCF30_INCREMENTAL_AUTHORIZATION == subtype2)
         {
        	 /* update the lookbacks if the data is updated in tf.ini*/
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

        	 if( PTEMSG_OK == (result_code = db_select_and_update_ncf30_for_Incrementalauthorization( (pNCF30)p_data,
        	                                                               &db_recs.ncf30,
        	                                                               ErrorMsg ) ) )
			 {
        		 updates_to_db_made = true;
				memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  subtype2,
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
											  NCF30_DATA);
			 }
         }
         else if(ST2_DB_SELECT_NCF30_PARTIAL_REVERSAL_AUTHORIZATION == subtype2)
         {
        	 /* update the lookbacks if the data is updated in tf.ini*/
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
        	 if( PTEMSG_OK == (result_code = db_select_and_update_ncf30_for_Partialreversalauthorization( (pNCF30)p_data,
        	                                                               &db_recs.ncf30,
        	                                                               ErrorMsg ) ) )
			 {
        		 updates_to_db_made = true;
				memcpy(p_current, (BYTE *)&db_recs.ncf30, sizeof( db_recs.ncf30 ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  subtype2,
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  sizeof(AUTH_TX) + sizeof(db_recs.ncf30),
											  NCF30_DATA);
			 }
         }
#endif
         break;

      case ATM01_DATA:

         if( PTEMSG_OK == ( result_code = db_select_atm01( (pCHAR)p_data,
                                                           &db_recs.atm01,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.atm01, sizeof( db_recs.atm01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.atm01),
                                          ATM01_DATA);

         }
         break;

      case NEG01_DATA:

         subtype2 = ptemsg_get_msg_subtype2 (p_msg_in);
         if ( ST2_DB_SELECT_NEG_FILE_BY_CARD_NBR == subtype2 )
         {
            /* Select NEG01 record based on card number only. */
            if( PTEMSG_OK ==
                ( result_code = db_select_neg01_by_card_num( (pCHAR)p_data,
                                                              &db_recs.neg01,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.neg01, sizeof( db_recs.neg01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.neg01),
                                          NEG01_DATA);
            }
         }
         else
         {
            if( PTEMSG_OK == ( result_code = db_select_neg01( (pCHAR)p_data,
                                                              &db_recs.neg01,
                                                              ErrorMsg ) ) )
            {
               memcpy(p_current, (BYTE *)&db_recs.neg01, sizeof( db_recs.neg01 ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.neg01),
                                          NEG01_DATA);
            }
         }
         break;

      case SAF01_DATA:

         if( PTEMSG_OK == ( result_code = db_select_saf01( (pSAF01)p_data,
                                                           &db_recs.saf01,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.saf01, sizeof( db_recs.saf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.saf01),
                                          SAF01_DATA);

         }
         break;

      case FG01_DATA:

         if( PTEMSG_OK == ( result_code = db_select_fg01( (pCHAR)p_data,
                                                           &db_recs.fg01,
                                                           ErrorMsg ) ) )
         {
            memcpy(p_current, (BYTE *)&db_recs.fg01, sizeof( db_recs.fg01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1( p_msg_in ),
                                          ptemsg_get_msg_subtype2( p_msg_in ),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)+sizeof(db_recs.fg01),
                                          FG01_DATA);

         }
         break;

      case TLF01_DATA :

          subtype2 = ptemsg_get_msg_subtype2 (p_msg_in);
          if ( ST2_DB_SELECT_TLF01_FOR_REVERSAL_PUL == subtype2 )
    	  {
              if( PTEMSG_OK == ( result_code = db_select_tlf01_for_pulse_reversal( auth_tx,
                                                                &db_recs.tlf01,
                                                                ErrorMsg ) ) )
              {
                 memcpy(auth_tx->TLF01_details.ncf01_retired_cwk,db_recs.tlf01.ncf01_retired_cwk,sizeof(db_recs.tlf01.ncf01_retired_cwk));
                 memcpy(AtpBuffer,auth_tx,sizeof(AUTH_TX));
                 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                               ptemsg_get_msg_subtype1( p_msg_in ),
                                               ptemsg_get_msg_subtype2( p_msg_in ),
                                               reply_que,
                                               application_que_name,
                                               (pBYTE)AtpBuffer,
                                               sizeof(AUTH_TX)+sizeof(db_recs.tlf01),
                                               TLF01_DATA);

              }

    	  }
          else if ( ST2_DB_SELECT_TLF01_FOR_PURCHANGE_RETURN == subtype2 )
		  {
			  if( PTEMSG_OK == ( result_code = db_select_tlf01_for_MC_purchase_return( auth_tx,
																&db_recs.tlf01,
																ErrorMsg ) ) )
			  {
				 memcpy(AtpBuffer,auth_tx,sizeof(AUTH_TX));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											   ptemsg_get_msg_subtype1( p_msg_in ),
											   ptemsg_get_msg_subtype2( p_msg_in ),
											   reply_que,
											   application_que_name,
											   (pBYTE)AtpBuffer,
											   sizeof(AUTH_TX)+sizeof(db_recs.tlf01),
											   TLF01_DATA);

			  }

		  }
    	  break;

      case OBS01_DATA:
          if( PTEMSG_OK == ( result_code = db_select_obs01( (pCHAR)p_data,
                                                            &db_recs.obs01,
                                                            ErrorMsg ) ) )
          {
             memcpy(p_current, (BYTE *)&db_recs.obs01, sizeof( db_recs.obs01 ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                           ptemsg_get_msg_subtype1( p_msg_in ),
                                           ptemsg_get_msg_subtype2( p_msg_in ),
                                           reply_que,
                                           application_que_name,
                                           (pBYTE)AtpBuffer,
                                           sizeof(AUTH_TX)+sizeof(db_recs.obs01),
                                           OBS01_DATA);

          }
          break;

      case VISA3DS2_DATA:
                if( PTEMSG_OK == ( result_code = db_select_visa3ds2( (pCHAR)p_data,
                                                                  &db_recs.visa3ds2,
                                                                  ErrorMsg ) ) )
                {
                   memcpy(p_current, (BYTE *)&db_recs.visa3ds2, sizeof( db_recs.visa3ds2 ));
                   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                 ptemsg_get_msg_subtype1( p_msg_in ),
                                                 ptemsg_get_msg_subtype2( p_msg_in ),
                                                 reply_que,
                                                 application_que_name,
                                                 (pBYTE)AtpBuffer,
                                                 sizeof(AUTH_TX)+sizeof(db_recs.visa3ds2),
												 VISA3DS2_DATA);

                }
                break;

      default:

         /* Unknown app_data_type received.
          * Log Event and return error message to client.
          */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf( ErrorMsg, "Unknown app_data_type for Select request from %s",
                  reply_que);
         break;
   }


   if ((PTEMSG_OK == result_code) && (NULL_PTR == p_msg_out) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf( ErrorMsg, "Insufficient Memory to build Select response to %s",
               reply_que);
   }
   return( p_msg_out );
}

/******************************************************************************
 *
 *  NAME:         process_alldata_msg
 *
 *  DESCRIPTION:  This function inserts a network control record into NCF30,
 *                then it updates the totals in NCF21.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pass or fail code
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 *      DI - 02/17/99 - Added statement to credit-debit determination to
 *                      make default account a credit.
 ******************************************************************************/
BYTE process_alldata_msg( pPTE_MSG p_msg_in )
{
   BYTE           app_data_type;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pNCF30         p_ncf30;
   CHAR           date_time[16] = "";
   long           total;
   long           grand_total;
   CHAR           ncf21_open_date[9]="";
   CHAR           ncf21_open_time[7]="";
   CHAR           proc_code[7];
   CHAR           ncf21_keys[sizeof(ncf21.primary_key)];
   int            cnt;
   BOOLEAN        debit    = false;
   BOOLEAN        credit   = false;
   BOOLEAN        both     = false;
   BOOLEAN        reversal = false;
   BOOLEAN        new      = false;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + sizeof(NCF30));

   p_current     = AtpBuffer + sizeof(AUTH_TX); 
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = p_data + sizeof(AUTH_TX);
   p_ncf30       = (pNCF30)p_data;
   CHAR   buffer[526]= {0};

   /* Set up keys for NCF21 record. */
   strncpy( ncf21_keys, p_ncf30->primary_key.network_id,
            sizeof(p_ncf30->primary_key.network_id));

   strncpy( ncf21_keys+sizeof(p_ncf30->primary_key.network_id),
            p_ncf30->primary_key.network_type,
            sizeof(p_ncf30->primary_key.network_type));

   strncpy( ncf21_keys+sizeof(p_ncf30->primary_key.network_id)
                      +sizeof(p_ncf30->primary_key.network_type),
            p_ncf30->primary_key.batch_number,
            sizeof(p_ncf30->primary_key.batch_number));

   /* Select totals record from NCF21 */
   result_code = db_select_ncf21( ncf21_keys, &db_recs.ncf21, ErrorMsg );
   if ( PTEMSG_NOT_FOUND == result_code )
   {
      new = true;
      memset( &db_recs.ncf21, 0x00, sizeof(db_recs.ncf21) );
      strcpy( db_recs.ncf21.primary_key.network_id,    p_ncf30->primary_key.network_id   );
      strcpy( db_recs.ncf21.primary_key.network_type,  p_ncf30->primary_key.network_type );
      strcpy( db_recs.ncf21.primary_key.batch_number, "ACTIVE" );
      strcpy( db_recs.ncf21.credit_cnt,     "0" );
      strcpy( db_recs.ncf21.credit_rev_cnt, "0" );
      strcpy( db_recs.ncf21.credit_amt,     "0" );
      strcpy( db_recs.ncf21.credit_rev_amt, "0" );
      strcpy( db_recs.ncf21.debit_cnt,      "0" );
      strcpy( db_recs.ncf21.debit_rev_cnt,  "0" );
      strcpy( db_recs.ncf21.debit_amt,      "0" );
      strcpy( db_recs.ncf21.debit_rev_amt,  "0" );

      /* Get system date and time. */
      genutil_format_date( date_time );
      strncpy(db_recs.ncf21.open_date ,date_time, 8);
      strncpy(db_recs.ncf21.open_time ,date_time + 8, 6);

      result_code = PTEMSG_OK;
   }
   if ( PTEMSG_OK == result_code )
   {
      /* Determine if Credit, Debit, or Both (Transfers) */
      strcpy( proc_code, p_ncf30->processing_code );
      if ( (proc_code[2] == '1')  ||  (proc_code[2] == '2') )
         debit = true;
      else
         credit = true;

      /* Determine if Reversal */
      if ( 0 == strncmp( "04", p_ncf30->message_type, 2 ) )
         reversal = true;

      /* Get grand total. */
      grand_total = atol( db_recs.ncf21.total_amt );
      if ( strncmp(db_recs.ncf21.total_amt_type,"C",1) != 0)
      {
        grand_total=-grand_total;
      }
      /* Update totals according to credit or debit. */
      if ( credit || both )
      {
         if ( reversal )
         {
            cnt   = atoi( db_recs.ncf21.credit_rev_cnt );
            total = atol( db_recs.ncf21.credit_rev_amt );
            cnt++;
            total       = total+ atol( p_ncf30->tran_amount );
            grand_total = grand_total - atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.credit_rev_cnt, "%04d",  cnt         );
            sprintf( db_recs.ncf21.credit_rev_amt, "%015d", total       );
         }
         else
         {
            cnt   = atoi( db_recs.ncf21.credit_cnt );
            total = atol( db_recs.ncf21.credit_amt );
            cnt++;
            total  = total+ atol( p_ncf30->tran_amount );
            grand_total= grand_total+ atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.credit_cnt, "%04d",  cnt   );
            sprintf( db_recs.ncf21.credit_amt, "%015d", total );
         }
      }
      if ( debit || both )
      {
         if ( reversal )
         {
            cnt   = atoi( db_recs.ncf21.debit_rev_cnt );
            total = atol( db_recs.ncf21.debit_rev_amt );
            cnt++;
            total = total + atol( p_ncf30->tran_amount );
            grand_total=grand_total + atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.debit_rev_cnt, "%04d",  cnt   );
            sprintf( db_recs.ncf21.debit_rev_amt, "%015d", total );
         }
         else
         {
            cnt   = atoi( db_recs.ncf21.debit_cnt );
            total = atol( db_recs.ncf21.debit_amt );
            cnt++;
            total = total + atol( p_ncf30->tran_amount );
            grand_total = grand_total - atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.debit_cnt, "%04d",  cnt   );
            sprintf( db_recs.ncf21.debit_amt, "%015d", total );
         }
      }
      sprintf( db_recs.ncf21.total_amt, "%016d", grand_total );
      if ( 0 <= grand_total )
         strcpy( db_recs.ncf21.total_amt_type, "C" ); /* Credit */
      else
         strcpy( db_recs.ncf21.total_amt_type, "D" ); /* Debit  */

      if ( new )
      {
         /* Insert new record into NCF21. */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF21 (&db_recs.ncf21,MODE_INSERT);
		  }
         result_code = db_insert_ncf21( &db_recs.ncf21, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
              /* NewLog, Its a new record so insert it into ncf21 */
              memset(buffer,0x00,sizeof(buffer));
              sprintf(buffer,"Inserting a new record into ncf21 table failed,network id:%s,Network type:%s,batch number:%s",
                     		 db_recs.ncf21.primary_key.network_id,db_recs.ncf21.primary_key.network_type,db_recs.ncf21.primary_key.batch_number);
              netds_log_message(2,2,buffer,"process_alldata_msg");

          }

      }
      else
      {
         /* Update record in NCF21. */
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinNCF21 (&db_recs.ncf21,MODE_UPDATE);
		  }
         result_code = db_update_ncf21( &db_recs.ncf21, ErrorMsg );
         if(result_code != PTEMSG_OK)
         {
         /* NewLog, Its an existing record so update it into ncf21 */
            memset(buffer,0x00,sizeof(buffer));
            sprintf(buffer,"Updating an existingrecord into ncf21 table failed,network id:%s,Network type:%s,batch number:%s",
                              		 db_recs.ncf21.primary_key.network_id,db_recs.ncf21.primary_key.network_type,db_recs.ncf21.primary_key.batch_number);
            netds_log_message(2,2,buffer,"process_alldata_msg");



      }
   }

   /* Insert record into NCF30 */
   if ( PTEMSG_OK == result_code )
   {
      strcpy( p_ncf30->open_date, db_recs.ncf21.open_date );
      strcpy( p_ncf30->open_time, db_recs.ncf21.open_time );
	  if (DB_NullCheck_Flag==1)
	  {
		  CheckNULLTerminatedinNCF30 (&db_recs.ncf30,MODE_INSERT);
	  }
      result_code = db_insert_ncf30( p_ncf30, ErrorMsg );
      if(result_code != PTEMSG_OK)
      {
          /* NewLog, Insert failed into NCF30 */
          memset(buffer,0x00,sizeof(buffer));
          sprintf(buffer,"Insert into ncf30 table failed, batch num:%s,Retrieval ref num:%s, network id:%s",
                  		db_recs.ncf30.primary_key.batch_number,db_recs.ncf30.primary_key.retrieval_ref_num,db_recs.ncf30.primary_key.network_id);
          netds_log_message(2,2,buffer,"process_alldata_msg");

       }

   }

   /* rollback, when any one the query is failed ( either NCF21 / NCF30 ) */
   if(result_code != PTEMSG_OK)
   {
		dbcommon_rollback ();
   }

   return ( result_code );
}
}


/******************************************************************************
 *
 *  NAME:         process_alldata_delete_msg
 *
 *  DESCRIPTION:  This function deletes a network control record from NCF30,
 *                then it updates the totals in NCF21.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pass or fail code
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE process_alldata_delete_msg( pPTE_MSG p_msg_in )
{
   BYTE           app_data_type;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pNCF30         p_ncf30;
   CHAR           date_time[16] = "";
   long           total;
   long           grand_total;
   CHAR           ncf21_open_date[9]="";
   CHAR           ncf21_open_time[7]="";
   CHAR           proc_code[7];
   CHAR           ncf21_keys[sizeof(ncf21.primary_key)];
   int            cnt;
   BOOLEAN        debit    = false;
   BOOLEAN        credit   = false;
   BOOLEAN        both     = false;
   BOOLEAN        reversal = false;
   

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + sizeof(NCF30));

   p_current     = AtpBuffer + sizeof(AUTH_TX); 
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = p_data + sizeof(AUTH_TX);
   p_ncf30       = (pNCF30)p_data;

   /* Set up keys for NCF21 record. */
   strncpy( ncf21_keys, p_ncf30->primary_key.network_id,
            sizeof(p_ncf30->primary_key.network_id));

   strncpy( ncf21_keys+sizeof(p_ncf30->primary_key.network_id),
            p_ncf30->primary_key.network_type,
            sizeof(p_ncf30->primary_key.network_type));

   strncpy( ncf21_keys+sizeof(p_ncf30->primary_key.network_id)
                      +sizeof(p_ncf30->primary_key.network_type),
            p_ncf30->primary_key.batch_number,
            sizeof(p_ncf30->primary_key.batch_number));

   /* Select totals record from NCF21 */
   result_code = db_select_ncf21( ncf21_keys, &db_recs.ncf21, ErrorMsg );
   if ( PTEMSG_OK == result_code )
   {
      /* Determine if Credit, Debit, or Both (Transfers) */
      strcpy( proc_code, p_ncf30->processing_code );
      if ( (proc_code[2] == '1')  ||  (proc_code[2] == '2') )
         debit = true;
      else
         credit = true;

      /* Determine if Reversal */
      if ( 0 == strncmp( "04", p_ncf30->message_type, 2 ) )
         reversal = true;

      /* Get grand total. */
      grand_total = atol( db_recs.ncf21.total_amt );

      /* Update totals according to credit or debit. */
      if ( credit || both )
      {
         if ( reversal )
         {
            cnt   = atoi( db_recs.ncf21.credit_rev_cnt );
            total = atol( db_recs.ncf21.credit_rev_amt );
            cnt++;
            total += atol( p_ncf30->tran_amount );
            grand_total -= atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.credit_rev_cnt, "%04d",  cnt         );
            sprintf( db_recs.ncf21.credit_rev_amt, "%015d", total       );
         }
         else
         {
            cnt   = atoi( db_recs.ncf21.credit_cnt );
            total = atol( db_recs.ncf21.credit_amt );
            cnt++;
            total  += atol( p_ncf30->tran_amount );
            grand_total += atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.credit_cnt, "%04d",  cnt   );
            sprintf( db_recs.ncf21.credit_amt, "%015d", total );
         }
      }
      if ( debit || both )
      {
         if ( reversal )
         {
            cnt   = atoi( db_recs.ncf21.debit_rev_cnt );
            total = atol( db_recs.ncf21.debit_rev_amt );
            cnt++;
            total += atol( p_ncf30->tran_amount );
            grand_total += atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.debit_rev_cnt, "%04d",  cnt   );
            sprintf( db_recs.ncf21.debit_rev_amt, "%015d", total );
         }
         else
         {
            cnt   = atoi( db_recs.ncf21.debit_cnt );
            total = atol( db_recs.ncf21.debit_amt );
            cnt++;
            total += atol( p_ncf30->tran_amount );
            grand_total -= atol( p_ncf30->tran_amount );
            sprintf( db_recs.ncf21.debit_cnt, "%04d",  cnt   );
            sprintf( db_recs.ncf21.debit_amt, "%015d", total );
         }
      }
      sprintf( db_recs.ncf21.total_amt, "%016d", grand_total );
      if ( 0 <= grand_total )
         strcpy( db_recs.ncf21.total_amt_type, "C" ); /* Credit */
      else
         strcpy( db_recs.ncf21.total_amt_type, "D" ); /* Debit  */

      /* Update record in NCF21. */
	  if (DB_NullCheck_Flag==1)
	  {
		  CheckNULLTerminatedinNCF21 (&db_recs.ncf21,MODE_UPDATE);
	  }
      result_code = db_update_ncf21( &db_recs.ncf21, ErrorMsg );
   }

   /* Delete record from NCF30 */
   if ( PTEMSG_OK == result_code )
   {
      strcpy( p_ncf30->open_date, db_recs.ncf21.open_date );
      strcpy( p_ncf30->open_time, db_recs.ncf21.open_time );
      result_code = db_delete_ncf30( p_ncf30, ErrorMsg );
   }

   return ( result_code );
}

/******************************************************************************
 *
 *  NAME:         copy_auth_tx_to_saf01
 *
 *  DESCRIPTION:  This function copies data from data fields in auth_tx into
 *                data fields in saf01.
 *
 *  INPUTS:       p_auth_tx - auth_tx data structure
 *
 *  OUTPUTS:      p_saf01   - saf01 data structure
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void copy_auth_tx_to_saf01( pAUTH_TX p_auth_tx, pSAF01 p_saf01 )
{
   memcpy( p_saf01->primary_key.transaction_id, p_auth_tx->TLF01_details.primary_key.transaction_id,
           sizeof(p_saf01->primary_key.transaction_id) );
   memcpy( p_saf01->card_num, p_auth_tx->TLF01_details.card_num,
           sizeof(p_saf01->card_num) );

   memcpy( p_saf01->tran_amount, p_auth_tx->TLF01_details.orig_amount,
           sizeof(p_saf01->tran_amount) );
   memcpy( p_saf01->device_id, p_auth_tx->TLF01_details.terminal_id,
           sizeof(p_saf01->device_id) );

   memcpy( p_saf01->message_type, p_auth_tx->TLF01_details.orig_message,
           sizeof(p_saf01->message_type) );
   memcpy( p_saf01->processing_code, p_auth_tx->TLF01_details.processing_code,
           sizeof(p_saf01->processing_code) );

   memcpy( p_saf01->sys_trace_audit_nbr, p_auth_tx->TLF01_details.orig_sys_trace_num,
           sizeof(p_saf01->sys_trace_audit_nbr) );
   memcpy( p_saf01->tran_date, p_auth_tx->TLF01_details.date_yyyymmdd,
           sizeof(p_saf01->tran_date) );

   memcpy( p_saf01->tran_time, p_auth_tx->TLF01_details.time_hhmmss,
           sizeof(p_saf01->tran_time) );
   memcpy( p_saf01->retrieval_ref_num, p_auth_tx->TLF01_details.orig_retrieval_ref_num,
           sizeof(p_saf01->retrieval_ref_num) );

   memcpy( p_saf01->merchant_id, p_auth_tx->TLF01_details.merchant_id,
           sizeof(p_saf01->merchant_id) );
   memcpy( p_saf01->auth_number, p_auth_tx->TLF01_details.auth_number,
           sizeof(p_saf01->auth_number) );

   memcpy( p_saf01->response_code, p_auth_tx->TLF01_details.response_code,
           sizeof(p_saf01->response_code) );
   memcpy( p_saf01->invoice_nbr, p_auth_tx->TLF01_details.invoice_number,
           sizeof(p_saf01->invoice_nbr) );

   memcpy( p_saf01->network_data, p_auth_tx->TLF01_details.mcard_banknet,
           sizeof(p_saf01->network_data) );
   memcpy( p_saf01->currency_code, p_auth_tx->TLF01_details.currency_code,
           sizeof(p_saf01->currency_code) );

   memcpy( p_saf01->transmission_timestamp, p_auth_tx->TLF01_details.transmission_timestamp,
           sizeof(p_saf01->transmission_timestamp) );
   memcpy( p_saf01->exp_date, p_auth_tx->TLF01_details.exp_date,
           sizeof(p_saf01->exp_date) );

   memcpy( p_saf01->settlement_amount, p_auth_tx->TLF01_details.settlement_total,
           sizeof(p_saf01->settlement_amount) );
   memcpy( p_saf01->settlement_date, p_auth_tx->TLF01_details.settlement_date,
           sizeof(p_saf01->settlement_date) );

   memcpy( p_saf01->settlement_conv_rate, p_auth_tx->settlement_conv_rate,
           sizeof(p_saf01->settlement_conv_rate) );
   memcpy( p_saf01->settlement_curr_code, p_auth_tx->settlement_curr_code,
           sizeof(p_saf01->settlement_curr_code) );

   memcpy( p_saf01->billing_amount, p_auth_tx->ch_billing_amt,
           sizeof(p_saf01->billing_amount) );
   memcpy( p_saf01->billing_conv_rate, p_auth_tx->ch_billing_conv_rate,
           sizeof(p_saf01->billing_conv_rate) );

   memcpy( p_saf01->billing_curr_code, p_auth_tx->ch_billing_curr_code,
           sizeof(p_saf01->billing_curr_code) );
   memcpy( p_saf01->tran_fee_amount, p_auth_tx->TLF01_details.tran_fee_amount,
           sizeof(p_saf01->tran_fee_amount) );

   memcpy( p_saf01->handler_queue, p_auth_tx->TLF01_details.handler_queue,
           sizeof(p_saf01->handler_queue) );
   memcpy( p_saf01->acquiring_inst_id_code, p_auth_tx->acq_inst_id_cd,
           sizeof(p_saf01->acquiring_inst_id_code) );

   memcpy( p_saf01->forwarding_inst_id_code, p_auth_tx->forwarding_inst_id_cd,
           sizeof(p_saf01->forwarding_inst_id_code) );
   memcpy( p_saf01->reversal_amount, p_auth_tx->TLF01_details.reversal_amount,
           sizeof(p_saf01->reversal_amount) );

   p_saf01->tx_key = p_auth_tx->TLF01_details.tx_key;
}


/******************************************************************************
 *
 *  NAME:         process_get_gui_list_msg
 *
 *  DESCRIPTION:  This function selects a list of records from a network table.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      (Global) db_lists - Selected records
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
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

   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX));
   p_current     = AtpBuffer + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case NCF01_DATA:
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_NCF01_LIST_BY_TYPE)
         {
            // copy the last ncf01.group_id retrieved into the structure 
            if( PTEMSG_OK == ( result_code =
                               db_get_ncf01_gui_list_by_type( ( pCHAR )p_data,
                                           &db_lists.Ncf01_Gui_List, ErrorMsg)))
            {
               memcpy( p_current,
                       (BYTE *)&db_lists.Ncf01_Gui_List,
                       sizeof( db_lists.Ncf01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.Ncf01_Gui_List),
                              NCF01_DATA);
            }
         }
         else
         {
            // copy the last ncf01.group_id retrieved into the structure 
            if( PTEMSG_OK == ( result_code =
                               db_get_ncf01_gui_list( (pNCF01)p_data,
                                      &db_lists.Ncf01_Gui_List, ErrorMsg)))
            {
               memcpy(p_current,
                      (BYTE *)&db_lists.Ncf01_Gui_List,
                      sizeof( db_lists.Ncf01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.Ncf01_Gui_List),
                              NCF01_DATA);
            }
         }
         break;

      case NCF01_KEYBLOCK01_DATA:
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_NCF01_LIST_BY_TYPE)
         {
            // copy the last ncf01_keyblock.group_id retrieved into the structure
            if( PTEMSG_OK == ( result_code =
                               db_get_ncf01_keyblock_gui_list_by_type( ( pCHAR )p_data,
                                           &db_lists.Ncf01_Keyblock_Gui_List, ErrorMsg)))
            {
               memcpy( p_current,
                       (BYTE *)&db_lists.Ncf01_Keyblock_Gui_List,
                       sizeof( db_lists.Ncf01_Keyblock_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.Ncf01_Keyblock_Gui_List),
							  NCF01_KEYBLOCK01_DATA);
            }
         }
         else
         {
            // copy the last ncf01_keyblock.group_id retrieved into the structure
            if( PTEMSG_OK == ( result_code =
                               db_get_ncf01_keyblock_gui_list( (pNCF01)p_data,
                                      &db_lists.Ncf01_Keyblock_Gui_List, ErrorMsg)))
            {
               memcpy(p_current,
                      (BYTE *)&db_lists.Ncf01_Keyblock_Gui_List,
                      sizeof( db_lists.Ncf01_Keyblock_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.Ncf01_Keyblock_Gui_List),
							  NCF01_KEYBLOCK01_DATA);
            }
         }
         break;

      case NCF02_DATA:
         if( PTEMSG_OK == ( result_code =
                            db_get_ncf02_gui_list( (pNCF02)p_data,
                                          &db_lists.Ncf02_Gui_List, ErrorMsg )))
         {
            memcpy(p_current,
                   (BYTE *)&db_lists.Ncf02_Gui_List,
                   sizeof( db_lists.Ncf02_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.Ncf02_Gui_List),
                              NCF02_DATA);
         }
         break;

      case OBS01_DATA:


         if( PTEMSG_OK == ( result_code =
                            db_get_obs01_gui_list( (pOBS01)p_data,
                                          &db_lists.OBS01_Gui_List, ErrorMsg )))
         {
            memcpy(p_current,
                   (BYTE *)&db_lists.OBS01_Gui_List,
                   sizeof( db_lists.OBS01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.OBS01_Gui_List),
                              OBS01_DATA);
         }
         break;

      case VISA3DS2_DATA:


               if( PTEMSG_OK == ( result_code =
                                  db_get_visa3ds2_gui_list( (pVISA3DS2)p_data,
                                                &db_lists.VISA3DS2_Gui_List, ErrorMsg )))
               {
                  memcpy(p_current,
                         (BYTE *)&db_lists.VISA3DS2_Gui_List,
                         sizeof( db_lists.VISA3DS2_Gui_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    sizeof(AUTH_TX) + sizeof(db_lists.VISA3DS2_Gui_List),
									VISA3DS2_DATA);
               }
               break;

      case FG01_DATA:
         if( PTEMSG_OK == ( result_code =
                            db_get_fg01_gui_list( (pFG01)p_data,
                                   &db_lists.Fg01_Gui_List, ErrorMsg)))
         {
            memcpy(p_current,
                      (BYTE *)&db_lists.Fg01_Gui_List,
                      sizeof( db_lists.Fg01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                              ptemsg_get_msg_subtype1 (p_msg_in),
                              ptemsg_get_msg_subtype2 (p_msg_in),
                              reply_que,
                              application_que_name,
                              (pBYTE)AtpBuffer,
                              sizeof(AUTH_TX) + sizeof(db_lists.Fg01_Gui_List),
                              FG01_DATA);
         }
         break;

      default:

         /* Unknown app_data_type received.
          * Log Event and return error message to client.
          */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf( ErrorMsg, "Unknown datatype for GetList request from %s",
                  reply_que);
         break;
   }

   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf( ErrorMsg, "Insufficient Memory to build GetAll response to %s",
               reply_que);
   }

   return( p_msg_out );
}

/******************************************************************************
 *
 *  NAME:         process_get_service_list_msg
 *
 *  DESCRIPTION:  This function selects a group of records for
 *                for batch processing.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
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

      case NCF30_DATA:

         if( PTEMSG_OK == ( result_code = 
                            db_get_ncf30_service_list( (pNCF30)p_data,
                                                       &db_lists.Ncf30_List,
                                                       ErrorMsg ) ))
         {
            memcpy(p_current, (BYTE *)&db_lists.Ncf30_List,
                   sizeof( db_lists.Ncf30_List ));

            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Ncf30_List),
                                          NCF30_DATA);
         }
         break;

		 case NCF01_DATA:

         PRINT( "request to retrieve ncf01 service list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_ncf01_service_list( ( pNCF01 )p_data, &db_lists.Ncf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Ncf01_List, sizeof( db_lists.Ncf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Ncf01_List),
                                          NCF01_DATA);
         }
         break;

		 case OBS01_DATA:

         PRINT( "request to retrieve obs01 service list\n" );
         if( ( result_code = db_get_obs01_service_list( ( pOBS01 )p_data, &db_lists.OBS01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.OBS01_Gui_List, sizeof( db_lists.OBS01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.OBS01_Gui_List),
                                          OBS01_DATA);
         }
         break;
		 case VISA3DS2_DATA:

		          PRINT( "request to retrieve visa3ds2 service list\n" );
		          if( ( result_code = db_get_visa3ds2_service_list( ( pVISA3DS2 )p_data, &db_lists.VISA3DS2_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
		          {
		             memcpy(p_current, (BYTE *)&db_lists.VISA3DS2_Gui_List, sizeof( db_lists.VISA3DS2_Gui_List ));
		             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
		                                           ptemsg_get_msg_subtype1 (p_msg_in),
		                                           ptemsg_get_msg_subtype2 (p_msg_in),
		                                           reply_que,
		                                           application_que_name,
		                                           (pBYTE)AtpBuffer,
		                                           sizeof(AUTH_TX) + sizeof(db_lists.VISA3DS2_Gui_List),
												   VISA3DS2_DATA);
		          }
		          break;

      default:

         // Unknown app_data_type received.
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg,
                 "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s",
               reply_que);
   }

   return (p_msg_out);
}


/******************************************************************************
 *
 *  NAME:         process_echo_msg
 *
 *  DESCRIPTION:  This function echos a message back to the originator.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_echo_msg ( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;


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
      sprintf( ErrorMsg, "Insufficient Memory to build Echo response to %s",
               reply_que);
   }
   return( p_msg_out );
}

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


      case OBS01_DATA:

         PRINT( "request to retrieve obs01 count\n" );
         if( ( result_code = db_get_obs01_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, table_count, strlen(table_count) + 1);
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + strlen(table_count) + 1,
                                          OBS01_DATA);
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

/******************************************************************************
 *
 *  NAME:         process_db_request
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                application.  It will switch a message type of database
 *                request, by subtype 1.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_db_request ( pPTE_MSG p_msg_in )
{
   pPTE_MSG       p_msg_out=NULL_PTR;
   CHAR           query_start_time[20]="";
   CHAR           query_end_time  [20]="";
   CHAR           errbuf[120];
   INT            ret_val;
   double         start_time;
   double         end_time;
   double         duration;
  
   MsgSubtype2 = ptemsg_get_msg_subtype2( p_msg_in );
   MsgSubtype1 = ptemsg_get_msg_subtype1( p_msg_in );
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
      case ST1_DB_SELECT:
               p_msg_out = process_select_msg( p_msg_in );
               break;

      case ST1_DB_UPDATE:
               p_msg_out = process_update_msg( p_msg_in );
               break;

      case ST1_DB_INSERT:
               p_msg_out = process_insert_msg( p_msg_in );
               break;

      case ST1_DB_DELETE:
               p_msg_out = process_delete_msg( p_msg_in );
               break;

      case ST1_DB_GET_GUI_LIST:
               p_msg_out = process_get_gui_list_msg( p_msg_in );
               break;

      case ST1_DB_GET_SERVICE_LIST:
               p_msg_out = process_get_service_list_msg( p_msg_in );
               break;

      case ST1_DB_ECHO:
               p_msg_out = process_echo_msg( p_msg_in );
               break;

      case ST1_DB_GET_COUNT:
    	  	  p_msg_out = process_get_count(p_msg_in);
              break;
      default:
               p_msg_out   = NULL_PTR;
               result_code = PTEMSG_INVALID_SUBTYPE1;
               sprintf(ErrorMsg,"Unknown subtype1 for database request from %s",
                       reply_que);
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
         netds_log_message(2, 2, errbuf, "process_db_request");
      }
   }
   return( p_msg_out );
}

/*************************************************************************************/
/*************************************************************************************/

void process_monitor_request(pPTE_MSG p_msg_in )
{
   CHAR           monitor_flag[100]={0};
   pCHAR          orig_comm;
   CHAR           MonAdd[100]={0};
   CHAR		  *ptr = NULL;
   CHAR tmp[300] = {0x00};
   CHAR prevOrigCommsInfo[300] = {0x00};
   pPTE_MSG  p_msg = 0;
   INT firstTimeMonitorRunning = 0;
   INT secondTimeMonitorRunning = 0;
   INT monitorRunningOverride = 0;
   INT monitorTryingToReconnect = 0;
   BYTE subType2 = 0;
   CHAR supportTable[MAX_ORIG_INFO_SIZE] = {0x00};
   CHAR empty[MAX_ORIG_INFO_SIZE] = {0x00};
   
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
	   LONG 		   data_len;
	   LONG 		   info_len;
	   pBYTE		   p_data_data = NULL_PTR;
	   MT_USER_INFO	 tmpUserInfo;

	int Rt = MemAccess(MONITORSUPPORTTABLE);
	if(Rt < 0)
	{
		CreateXipcTable(1, MAX_ORIG_INFO_SIZE, MONITORSUPPORTTABLE);
	}
	Rt = MemAccess(MONITORUSERTABLE);
	if(Rt < 0)
	{
		CreateXipcTable(1, MONITORUSERTABLESIZE, MONITORUSERTABLE);
	}
   
	   memset(&tmpUserInfo, 0x00, sizeof(MT_USER_INFO));

   if(p_msg_in != NULL_PTR)
   {
		p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
	   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
	   info_len = data_len;
	   if(sizeof(MT_USER_INFO) < data_len)
		  info_len = sizeof(MT_USER_INFO);
	   p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
	   if(p_data_data != NULL)
	   {
		   memcpy(&tmpUserInfo, p_data_data, info_len);
	   }
   }
   memset(AtpBuffer, 0, sizeof(AtpBuffer));
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
       case ST1_MONITOR_UP   : 
			orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
			subType2 = ptemsg_get_msg_subtype2(p_msg_in);
			ReadMemMonitorTable(prevOrigCommsInfo);
			if (subType2 != ST2_MONITOR_OVERRIDE) 
			{
				if( subType2 == ST2_MONITOR_RECONNECT)
					monitorTryingToReconnect = 1;
				else
				if (!strlen(prevOrigCommsInfo))
				{
					/* no other existing monitor is running */
					firstTimeMonitorRunning = 1;
					/*Clear Monitor support table*/
					WriteMemMonSupportTable(empty);
				}
				else
				{
					secondTimeMonitorRunning = 1; // We will check below whether this is actually second time or third time...
				}
			}
			else
				/* client wants to override the fact that the monitor was running previously
				and wants to override the MONITOR_TABLE with the new orig_comms_info */
				monitorRunningOverride = 1;
			
			// ASSUMPTION - Override is not used in the source code of BDO anywhere. If override is
			// used, it will knock off all existing Monitor connections. Ensure that you use override, when
			// you actually want to knock off all existing connections.
			if (firstTimeMonitorRunning || monitorRunningOverride)
			{
				memcpy(&sMonitorUserInfo, &tmpUserInfo, sizeof(MT_USER_INFO));
				WriteMemMonUserTable(tmpUserInfo.user_name);
				
					// ignore any previous value in Monitor table, if present, and write the comms info from the current connection.
				p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);
				WriteMemMonitorTable( (BYTE *)orig_comm );
				WriteMemMonSupportTable((BYTE *)orig_comm );
			}
			else if ( secondTimeMonitorRunning )
			{	// Allow a read only connection for second monitor. New response subtype1 defined - ST2_MONITOR_UP_READ_ONLY.
				// Check if already two monitors are connected? If yes, then reject this connection.
				ptr = strchr(prevOrigCommsInfo, COMMA);
				if ( ptr )
				{	// first monitor connection present.
					ptr++;
					ptr = strchr(ptr, COMMA);
					if ( ptr )
					{	// two monitor connections already present - cannot allow this one. Reject it.
						/* send message to monitor to indicate all existing monitor slots are full */
						p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, ST2_MONITOR_ALREADY_RUNNING,applnk_que_name, application_que_name, NULL, 0,0);
					}
					else
					{	// Only one connection present
						//Now Check whether the one connection is primary or secondary
						memset(supportTable, 0x00, MAX_ORIG_INFO_SIZE);
						ReadMemMonSupportTable(supportTable);
						if(strlen(supportTable) == 0)
						{
							//No primary monitor. Previously running primary monitor has been closed
							WriteMemMonSupportTable((BYTE *)orig_comm);
							WriteMemMonUserTable(tmpUserInfo.user_name);
							p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);

							strcpy(tmp, prevOrigCommsInfo);
							strcpy(prevOrigCommsInfo,orig_comm);
							strcat(prevOrigCommsInfo,",");
							strcat(prevOrigCommsInfo,tmp);
							WriteMemMonitorTable( (BYTE *)prevOrigCommsInfo );
							
						}
						else
						{
							ReadMemMonUserTable(sMonitorUserInfo.user_name);
							//Primary monitor is present, So this connection is secondary for sure.
							p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, ST2_MONITOR_READ_ONLY,applnk_que_name, application_que_name, &sMonitorUserInfo,sizeof(MT_USER_INFO),0);
							// Append this connection information, with the existing connection information of first Monitor.
							strcat(prevOrigCommsInfo, ",");
							strcat(prevOrigCommsInfo, orig_comm);
							WriteMemMonitorTable( (BYTE *)prevOrigCommsInfo );
						}
					}
				}
				else // Something seriously wrong
				{
					// Invalid previous comms info.
					sprintf (ErrorMsg, "Invalid existing comms info in MonitorTable (socket ID not found) - disregarding it and using the current comms info for a read-only connection. Note that this may disconnect an existing Primary Monitor connection.");
					netds_log_message(2,3,ErrorMsg,"process_monitor_request");
					p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, ST2_MONITOR_ALREADY_RUNNING,applnk_que_name, application_que_name, NULL, 0,0);
					WriteMemMonitorTable( (BYTE *)orig_comm );
					// NOTE: There is only one read-only monitor connection now and previous monitor shall have to send a re-connect request to get re-connected.
				}
			}
			else if ( monitorTryingToReconnect )
			{	// its a reconnect - search for comm_info of this connection, in Monitor table.
				// If present, don't do anything - If not present add it (provided there was only upto one comm_info before).
				// TODO: If not present, and already two comm_info are present... what should we do?
				if ( strlen (prevOrigCommsInfo) == 0 )
				{	// No previous connection, so write the current connection info.
					p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);
					WriteMemMonitorTable( (BYTE *)orig_comm );
					WriteMemMonSupportTable((pBYTE) orig_comm );
									}
				else if (strncmp ( prevOrigCommsInfo, orig_comm, strlen(orig_comm)) == 0 ) // is it the first one?
				{
					// Nothing to change in Monitor Table. Just send a response back.
					p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);
				}
				else
				{
					// Check if there is another secondary monitor connection present. If yes, compare with the current comm info.
					ptr = strchr(prevOrigCommsInfo, COMMA);
					if ( ptr )
					{	// first monitor connection present.
						ptr++;
						ptr = strchr(ptr, COMMA);
						if ( ptr )
						{	// two monitor connections already present - compare the current comms info of re-connect, with the second connection data.
							ptr++;
							if ( strncmp(ptr, orig_comm, strlen(orig_comm)) == 0 ) // if the connection info matches with the second stored connection, send a positive response.
								p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);
							else		// Should ideally never come here... reconnect request is coming from a monitor that was never connected. Reject it.
								p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, ST2_MONITOR_ALREADY_RUNNING,applnk_que_name, application_que_name, NULL, 0,0);
						}
						else
						{	// Only one connection present and that is not the current one - we have already tried the comparison above.
							p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);
							// Append this connection information, with the existing connection information of first Monitor.
							strcat(prevOrigCommsInfo, ",");
							strcat(prevOrigCommsInfo, orig_comm);
							WriteMemMonitorTable( (BYTE *)prevOrigCommsInfo );
						}
					}
					else // Something seriously wrong
					{
						// Invalid previous comms info.
						sprintf (ErrorMsg, "Invalid existing comms info in MonitorTable (socket ID not found) for a reconnect request - disregarding existing comms-info and using the current comms info. Note that this may disconnect an existing Primary Monitor connection.");
						netds_log_message(2,3,ErrorMsg,"process_monitor_request");
						p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, subType2,applnk_que_name, application_que_name, NULL, 0,0);
						WriteMemMonitorTable( (BYTE *)orig_comm );
						// NOTE: There is only one monitor connection now and previous monitor shall have to send a re-connect request to get re-connected.
					}
				}
			}
			else		// send message to monitor to indicate an existing monitor is already running
				p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_MONITOR_UP, ST2_MONITOR_ALREADY_RUNNING,applnk_que_name, application_que_name, NULL, 0,0);

			ptemsg_set_orig_comms_info(p_msg, orig_comm);
			pteipc_send(p_msg, applnk_que_name);
			free(p_msg);
		break;
      case ST1_MONITOR_DOWN:

		// NOTE that we are allowing two monitors to connect simultaneously now. First one is the primary
		// fully functional monitor, second one will be a read only connection.
	  	//
	  	// VERIFY FIRST , WHETHER MONITOR DOWN REQUEST IS COMING FROM A LOGGED-IN MONITOR
	  	// It can also come from a Monitor that tried to connect but couldn't as two monitors were already
	  	// connected.
		ReadMemMonitorTable(prevOrigCommsInfo);
//	printf("\nST1_MONITOR_DOWN - Monitor Sockets - %s\n",prevOrigCommsInfo);
		orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
		ReadMemMonitorTable(prevOrigCommsInfo);
		ptr = strchr(prevOrigCommsInfo, COMMA);
		if (ptr)
		{
			ptr++;
			// Now ptr is pointing to the socket ID of primary comms info. search for
			// comma once more to reach the secondary comms_info, if present.
			ptr = strchr(ptr, COMMA);
			if (ptr)
			{
				ptr++;
				// Now ptr is pointing to the secondary comms info, when it is present.
			}
			else
				ptr = NULL;
		}
		else
			ptr = NULL;

		// Now ptr is pointing to the start of secondary comms info, when it is present, or it is NULL.

		// If prevOrigCommsInfo is NULL - zero monitors were connected
		// If prevOrigCommsInfo is NOT NULL, but ptr is NULL - 1 monitor was connected
		// If prevOrigCommsInfo and ptr, both are NOT NULL - 2 monitors were earlier connected
		// Check which one is trying to disconnect?

		if	( strncmp(orig_comm, prevOrigCommsInfo, strlen(orig_comm)) == 0 	// matches with primary comms info
			&& (prevOrigCommsInfo[strlen(orig_comm)] == '\0' || prevOrigCommsInfo[strlen(orig_comm)] == COMMA ) ) // This condition is required else a socket of 1 and 12 ("applnkA,1" and "applnkA,12"), both will match.
		{
			// Adjust the monitor table connection data (comms info). Handle presence of a secondary connection.
			if ( ptr == NULL )
			{	// Secondary connection not present
				WriteMemMonitorTable((BYTE *)monitor_flag);
				WriteMemMonSupportTable((BYTE *)monitor_flag);				
			}
			else
			{	// Secondary connection present
				memcpy (monitor_flag, ptr, strlen(ptr)); // ptr is pointing to secondary connection data (comms info).
				WriteMemMonitorTable((BYTE *)monitor_flag);
				WriteMemMonSupportTable((BYTE *)empty);
			}
		}
		else if ( ptr != NULL && strncmp(orig_comm, (char*)ptr, strlen(orig_comm)) == 0 ) // Or, matches with secondary comms info when present
		{	// Request came from primary Monitor connection, so shut down and delete connection info.
			memcpy (monitor_flag, prevOrigCommsInfo, ((char*)ptr - (char*)prevOrigCommsInfo) - 1 ); // Copy only the primary comms info
			WriteMemMonitorTable((BYTE *)monitor_flag);
		}
		else
		{		// Not same - so request is coming from a rogue Monitor GUI
			// Nothing to do here. Drop this request.
		}
         break;
	case ST1_MONITOR_CLEAR_SOCKETS:
			ReadMemMonitorTable(prevOrigCommsInfo);
//	printf("\nMonitor Sockets - %s",prevOrigCommsInfo);
			orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
			subType2 = ptemsg_get_msg_subtype2(p_msg_in);
			memset(MonAdd,0,100);

			p_msg = ptemsg_build_msg(MT_MONITOR_CLEAR_SOCKETS, ST1_MONITOR_CLEAR_SOCKETS, ST2_MONITOR_CLEARED	,applnk_que_name, application_que_name,NULL, 0,0);

			//Send monitor close to any open monitors
			ptemsg_set_orig_comms_info(p_msg, prevOrigCommsInfo);
			pteipc_send(p_msg, applnk_que_name);
			//Clear the table
			WriteMemMonitorTable((BYTE *)MonAdd);
//			printf("\nMonitor table cleared - %s\n",orig_comm);
			//Set back the original socket information for the request
			ptemsg_set_orig_comms_info(p_msg, orig_comm);
			pteipc_send(p_msg, applnk_que_name);
			free(p_msg);
			break;
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
         break;
   }

   free(p_msg);
   free(p_msg_data);
}


/******************************************************************************
 *
 *  NAME:         process_logger_reply
 *
 *  DESCRIPTION:  This function is used to communicate with Logger / Reply.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void process_logger_reply( pPTE_MSG p_msg_in )
{
   pCHAR       p_msg_trid;


   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_LOG_REFEED_COMPLETE : 
         in_refeed_mode = false;
         p_msg_trid     = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy (checkpoint_rec.checkpoint_trid, p_msg_trid,
                     sizeof(checkpoint_rec.checkpoint_trid));
            strncpy (last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint(&checkpoint_rec, ErrorMsg);

            if (result_code == PTEMSG_OK)
            {
               dbcommon_commit ();
            }
            else
            {
               dbcommon_rollback ();
               netds_log_message(1,3, ErrorMsg, "process_logger_reply");

               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
                  netds_log_message(1, 3, ErrorMsg, "process_logger_reply");
               }
            }
         }
                                     
         break;
      
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for logger reply from %s", reply_que);
         netds_log_message(1, 2, ErrorMsg, "process_logger_reply");
         break;
   }
}



/******************************************************************************
 *
 *  NAME:         perform_refeed
 *
 *  DESCRIPTION:  This function will re-feed a command to Logger / Reply.
 *
 *  INPUTS:       trid - Transaction Id
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True is successful, else False
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN perform_refeed ( pCHAR trid )
{
   CHAR     Buffer[256];


   if (trid [0] != '\0')
   {
      /* send refeed command to TFLogger */
      if (SendRefeedCmd (trid, ErrorMsg) == false)
      {
         sprintf  (Buffer, "Error sending refeed command to TFLogger: %s", ErrorMsg);
         netds_log_message(1, 3, ErrorMsg, "perform_refeed");
         return (false);
      }

      in_refeed_mode = true;

   }

   return (true);
}



/******************************************************************************
 *
 *  NAME:         validate_last_trid
 *
 *  DESCRIPTION:  
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Code indicating disposition of the validation
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
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
       netds_log_message(1, 2, ErrorMsg, "validate_last_trid");
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
         netds_log_message(1, 2, ErrorMsg, "validate_last_trid");
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
         netds_log_message(1, 2, ErrorMsg, "validate_last_trid");
    	 pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }

   return (result_code);
}



/******************************************************************************
 *
 *  NAME:         rollback_the_update
 *
 *  DESCRIPTION:  Roll back the database changes.  Do not commit them.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void rollback_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* First, rollback the database update(s) */
      dbcommon_rollback ();

      updates_to_db_made = false;
   }
}  /* rollback_the_update */


/******************************************************************************
 *
 *  NAME:         commit_the_update
 *
 *  DESCRIPTION:  Commit any database changes.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void commit_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* commit the database update(s) */
      dbcommon_commit ();

      updates_to_db_made = false;
   }
}  /* commit_the_update */



/******************************************************************************
 *
 *  NAME:         PinnacleMsgHandler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                application.  It will transfer control to the appropriate
 *                module, based on message type.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE PinnacleMsgHandler ( pPTE_MSG p_msg_in )
{
   LONG           i;
   LONG           num_messages;
   LONG           ipc_rcode;
   CHAR           temp_str [80];
   pPTE_MSG       p_cur_msg=NULL_PTR;
   pPTE_MSG       p_tmp_msg=NULL_PTR;
   pPTE_MSG       p_chn_out=NULL_PTR;
   pPTE_MSG       p_new_chn=NULL_PTR;
   pPTE_MSG_DATA  p_msg_data;
   pBYTE          p_data;
   INT            ret_val;
   INT            num_sql;
   int            refresh_flag = 0;  /* If an error in update_mem_record or
                                      * delete_mem_record, delete all recs. in
                                      * mem and recreate all tables over again 
                                      * from the database.
                                      */
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
   p_data = p_data +  sizeof(AUTH_TX);


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
         case MT_SYSTEM_REQUEST :
                  ptesystm_system_msg_handler (p_cur_msg);
                  if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
                  {
                     if ( ST2_NO_VERSION != ptemsg_get_msg_subtype2(p_msg_in) )
                     {
                        /* When pinged, display application version on Monitor. */
                        sprintf( temp_str,
                                "Pinged -> Network Data Server: %s, version %s",
                                 ServiceName, Version );
                       netds_log_message(1, 1, temp_str, "PinnacleMsgHandler");

                        /* Get Database Timing Parameters to gather
                         * statistics about each query.
                         */
                        memset( ReportTime, 0x00, sizeof(ReportTime) );
                        memset( temp_str,   0x00, sizeof(temp_str)   );
                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                          ReportTime, 
                                                         &ForceReport,
                                                          temp_str );
                        netds_log_message(2,1,temp_str,"process_monitor_request");
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
                              netds_log_message(2,1,temp_str,"process_monitor_request");
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
            if ( (0 == strncmp (ptemsg_get_msg_dest_queue( p_cur_msg ),
                                              application_que_name,
                                              strlen(application_que_name)) ) ||
                 (0 == strncmp (ptemsg_get_msg_dest_queue (p_cur_msg),
                                              control_que_name,
                                              strlen(control_que_name)) )   )
            {
               result_code = PTEMSG_INVALID_MSGTYPE;
               sprintf (ErrorMsg,
                       "%d Unknown msgtype for request from %s",ptemsg_get_msg_type( p_cur_msg ), reply_que);
            }
            else
            {
               /*
                * Forward the request to its destination (if not in refeed mode)
                */
               if (in_refeed_mode == false)
               {
                  ptemsg_set_orig_comms_info( p_cur_msg, orig_comm_inf );
                  ipc_rcode = pteipc_send (p_cur_msg,
                                           ptemsg_get_msg_dest_queue(p_cur_msg));
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
               sprintf( ErrorMsg, "Insufficient Memory to chain response to %s",
                        reply_que );
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
            strncpy( checkpoint_rec.checkpoint_trid, p_msg_trid,
                     sizeof(checkpoint_rec.checkpoint_trid));
            strncpy( last_trid, p_msg_trid, sizeof(last_trid));

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
      memcpy(AtpBuffer + sizeof(AUTH_TX), ErrorMsg, strlen(ErrorMsg) + 1);
      p_chn_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_cur_msg),
                                    ptemsg_get_msg_subtype2 (p_cur_msg),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) AtpBuffer,
                                    strlen(ErrorMsg) + 1 + sizeof(AUTH_TX),
                                    app_data_type);

      if (p_chn_out == NULL_PTR)
      {
         netds_log_message(1, 2, ErrorMsg, "PinnacleMsgHandler");
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf( ErrorMsg, "Insufficient Memory to build error response to %s",
                  reply_que );
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
      	{ free (p_chn_out);
	  		free(p_new_chn);
	  		free(p_cur_msg);
         return (PTEMSG_OK);
      	}

      /*
       * If the result is OK, don't send the response if they don't want it.
       */
      if((PTEMSG_OK                == result_code  ) &&
        (PTEMSG_REPLY_ONLY_IF_FAILS==ptemsg_get_type_of_reply_needed(p_msg_in)))
      {
         free (p_chn_out);
		 free(p_new_chn);
         return (PTEMSG_OK);
      }

      ptemsg_set_result_code (p_chn_out, result_code);
      ptemsg_set_orig_comms_info( p_chn_out, orig_comm_inf );

      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_chn_out, reply_que);

      free (p_chn_out);
	  free(p_new_chn);
      if (ipc_rcode < 0)
      {
         netds_log_message(1, 2, ErrorMsg, "PinnacleMsgHandler");
         result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf(ErrorMsg,"error sending response to %s: %s",reply_que,temp_str);
      }
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
      printf   ("%s\n", ErrorMsg);
      netds_log_message(2,2,ErrorMsg,"PinnacleMsgHandler");
   }*/
	free(p_new_chn);
    free(p_cur_msg);
    free (p_chn_out);
    free(p_tmp_msg);
   
    if(DB_reconnect == 1)
    {
 	   dbcommon_disconnect (ErrorMsg);
 	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
 	   {
 		   strcat (ErrorMsg, " DB Reconnection failed");
 		   netds_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler");
 			return (false);
 	   }
 	   else
 	   {
 		   netds_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler");
 	   }

    }
    else
    {
 	   DB_reconnect = 0;
    }
   return (result_code);
}



/******************************************************************************
 *
 *  NAME:         perform_startup
 *
 *  DESCRIPTION:  This function performs basic startup functions such as
 *                logging into the database and initializing XIPC.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN perform_startup ()
{
   CHAR     XipcInstanceName[12];
   INT      ret_val;
   CHAR     Buffer[300] = "";


   memset (last_trid, 0, sizeof(last_trid));

   /* Try to connect to the database */

   /* The fourth parameter is not used (ORCL) !!! Don't ask why. */
   if ( PTEMSG_OK != 
        dbcommon_connect("equitable","equitable","equitable","ORCL",ErrorMsg) )
   {
	   netds_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler");
	   return( false );
   }

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


   /*12-07-98 gabriel multiple instance */

   /*if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)*/
   if( !pteipc_init_multiple_instance_app(AppName,ServiceName,XipcInstanceName))
   {
      netds_log_message(1, 3, "Failed to create XIPC queues", "perform_startup");
      pteipc_shutdown_multiple_instance_app();
      dbcommon_disconnect (ErrorMsg);
      return (false);
   }
   

   #ifdef USING_LOGGER_REPLY
      /* send restart command to TFLogger */
      if (SendRestartCmd (ErrorMsg) == false)
      {
         netds_log_message(1, 3, "Error sending restart command to TFLogger", "perform_startup");
         pteipc_shutdown_multiple_instance_app();
         dbcommon_disconnect (ErrorMsg);
         return (false);
      }

      #ifdef USING_CHECKPOINTS
         /* get the checkpoint record from the database */
         if (PTEMSG_OK != 
             dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg))
         {
        	 netds_log_message(1, 3,ErrorMsg, "perform_startup");
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
            //pteipc_shutdown_single_instance_app();
            pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }
      #endif
   #endif

   ret_val = get_dataserver_ini_timer_value( &ds_timer_flag, &ds_timer_value,
                                             AppName,         Buffer );
   if ( false == ret_val )
   {
      netds_log_message(2,2,Buffer,"perform_startup");
   }
   else if ( ds_timer_flag == DS_TIMER_ON )
   {
      sprintf( Buffer,
              "%s:Data Server timing of queries is turned on, value is %d",
               AppName, ds_timer_value );
      netds_log_message(2,1,Buffer,"perform_startup");
   }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   netds_log_message(2,1,Buffer,"perform_startup");

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
      netds_log_message(2,2,Buffer,"perform_startup");
   }

   return (true);
}

#ifndef WIN32

void TrapFunction( INT signum )
{
   CHAR Buffer[100];

   XIPC_TRAP_FUNCTION_TEST( TrapFunction, signum );

   memset( Buffer, 0x00, sizeof(Buffer) );
   sprintf( Buffer, "%s:  Caught signal %d", AppName, signum );
   netds_log_message(2,3,Buffer,"perform_startup");
}

#endif

/******************************************************************************
 *
 *  NAME:         MainProcessor
 *
 *  DESCRIPTION:  This is the driver function for the Network Control
 *                Data Server.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void MainProcessor()
{
   LONG     ipc_rcode, xipc_code;
   pPTE_MSG p_msg;
   CHAR     Buffer[256];
   CHAR     MonAdd[100]="";
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
   netds_log_message(2,1,Buffer,"MainProcessor");
	printf( "%s\n", Buffer );

#ifdef AEX03A
	get_variable_auth_config_details();
#endif
   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	  	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
	  	    netds_log_message(2,1,Buffer,"MainProcessor");
			printf( "%s\n", Buffer );

      return;
   }



         if ( false == ret_val )
         {
             netds_log_message(2,1,Buffer,"MainProcessor");
         }
         else if ( db_error_statics_flag == DS_TIMER_ON )
         {
              sprintf( Buffer,
                     "%s:Data Server DB error statics for queries is turned on, value is %d",
                     ServiceName, db_error_statics_value );
             netds_log_message(2,1,Buffer,"MainProcessor");
         }

   // TEST GENERIC TABLE   //
   //WriteGenericTable("TEST", "1", "B", "C"); 
 
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   netds_log_message(2,1,Buffer,"MainProcessor");
   printf( "%s\n", Buffer );

// Need to clear the MonitorSupportTable.
   int Rt = MemAccess(MONITORSUPPORTTABLE);
	if(Rt < 0)
	{
		CreateXipcTable(1, MAX_ORIG_INFO_SIZE, MONITORSUPPORTTABLE);
	}
	Rt = MemAccess(MONITORUSERTABLE);
	if(Rt < 0)
	{
		CreateXipcTable(1, MONITORUSERTABLESIZE, MONITORUSERTABLE);
	}

   PRINT( "DataServer is ready. press <ENTER> to exit\n" );

   while ( !EndProcessSignalled )
   {
      /* You are blocked here waiting for a message on either app queue or
       * control queue.  If there is no message on either que for the specified
       * time, the blocking call returns.
       */
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
         netds_log_message(1, 2, Buffer, "MainProcessor");
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
            netds_log_message(1, 3, strError, "MainProcessor");
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
         netds_log_message(2,3,XipcErrBuffer,"MainProcessor");
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
            netds_log_message(2,1,Buffer,"MainProcessor");
            reset_timing_stats( &TimingStats );
            NextReportTime = set_next_report_time_at_startup( ReportTime );
         }
      }
   }

   if( Ncf01InMem )
   {
      xipc_code = DestroyXipcTable("Ncf01Table");
      //PRINT("Bin01 destroy %d\n", xipc_code);
   }

   /* Shut down ipc */
   //pteipc_shutdown_single_instance_app(); 
   pteipc_shutdown_multiple_instance_app(); 

   printf ("pteipc shutdown successful\n");

   /* Disconnect from database */
   if (dbcommon_disconnect (ErrorMsg) != PTEMSG_OK)
      printf ( "%s\n", ErrorMsg );
   else
      printf ( "Disconnected from ORACLE\n" );

   /* Signal that we are ready for shutdown */
   MainProcessDone = 1;
}


/******************************************************************************
 *
 *  NAME:         EndProcess
 *
 *  DESCRIPTION:  This is the function used to clean up when the app is
 *                being exited.  All clean up code goes in here.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void EndProcess()
{
   /* Send message to Monitor and EventLog. */
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   netds_log_message(2,1,Buffer,"MainProcessor");
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
   CHAR  netds_error_warning_file_size[5] = {0};


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
							  netds_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(netds_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(netds_error_warning_file_size);
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
#ifdef AMEX03A
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
#endif

/******************************************************************************
 *
 *  NAME:         netds_log_message
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
void netds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func)
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]="";
   CHAR text_message[512]="";

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
   netds_Log_error_warning_to_File(text_message,sev,func);
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

INT netds_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	FILE *fp;
	long int file_size = 0 ;
	char buf[256] = {0} ;

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
