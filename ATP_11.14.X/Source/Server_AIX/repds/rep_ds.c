/******************************************************************************

   Module: repds.c

   Title: Report Dataserver Routines

   Description: 

   Revision History
   ----------------
	  Rev 1.32   Nov 06 2008   Girija Y ThoughtFocus,Bangalore
   Updated version to 4.4.1.9
   Added the function to get GUI list from TLF01_STIP table.
   Rel 08-33

      Rev 1.32   Aug 24 2007   dirby
   Updated version to 4.4.1.7
   SCR 25033

      Rev 1.31   Mar 08 2007 10:29:34   DIrby
   Fixed bug when checking subtype 2 on a ping; changed '==' to '!='.
   SCR 24081
   
      Rev 1.30   Mar 02 2007 11:10:10   DIrby
   Added support for every SQL to be stored into shared memory
   in table SQL01 prior to being executed.
   SCR 24081
   
      Rev 1.29   Dec 06 2006 10:16:44   DIrby
   Updated  version to 4.4.1.5
   
      Rev 1.28   Jul 07 2006 14:17:38   dirby
   Implemented functionality to gather statistics on database SQL calls.
   SCR 12299
   
      Rev 1.27   Jul 14 2005 11:55:28   dirby
   Added a subtype2 parameter to the function to get a GUI list for
   Transaction Lookup inquiry.  This subtype indicates whether to get
   all failed, all successful, or just get all fraudguard transactions that
   meet the rest of the search criteria.
   SCR 14421
   
      Rev 1.26   Jun 24 2005 12:53:06   dirby
   Added support for lookup of Fraudguard transactions.
   SCR 14421
   
      Rev 1.25   Jun 14 2005 16:57:18   chuang
   Version 4.4.1.2. Fix SCR 15819(Support for viewing EMV Raw Data).
   
      Rev 1.24   Apr 05 2005 15:40:22   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.23   Jul 08 2004 17:17:08   dirby
   Updated to 4.4.0.1
   SCRs 1287 & 1388
   
      Rev 1.22   May 27 2004 17:19:38   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.21   Feb 17 2003 09:59:46   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.20   Oct 28 2002 10:21:06   dirby
   Added code pertaining to starting a 2nd instance of repds.
   At start up, repds checks to see if an instance is already running,
   also if its queues already exist.  Action is taken depending on the
   status.  There are four cases:
   1.  No instance, no queues = startup
   2.  No instance, yes queues = destroy queues, startup
   3.  Yes instance, no queues = kill instance then startup
   4.  Yes instance, yes queues = do not start up
   All of the above pertains only to Unix, because Unix commands
   are used.
   SCR 818
   
      Rev 1.19   Oct 18 2002 10:41:52   dirby
   Modified to slow down messages to syslog and applnk when
   the application's queue is not accessible.  There will be 1
   second between each message, every 10th message, the
   operator is asked to shut down the application.
   SCR 818
   
      Rev 1.18   Aug 16 2002 18:04:24   dirby
   Added functionality to display version when pinged.
   SCR 255
   
      Rev 1.17   Aug 16 2002 17:37:14   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.16   Jul 18 2002 15:16:00   dirby
   Added functionality to support timing of queries and report on
   any that take too long.  This is configurable in TF.INI.
   SCR 818
   
      Rev 1.15   Apr 26 2002 17:19:52   dirby
   Modified to call function 'db_get_tlf01_gui_list_dynamic_new'
   instead of function 'db_get_tlf01_gui_list_dynamic' for Tran
   Lookup.  The difference is the new one builds SQL on the fly;
   the other one uses a huge inefficient SQL all the time.  The
   purpose is to speed up Tran Lookup.
   SCR 698
   
      Rev 1.14   Nov 03 2001 12:11:48   dirby
   Updated version to 4.0.0.1     SCR 256
   
      Rev 1.13   Aug 13 2001 13:15:50   dirby
   Updated version to 3.2.3.3 for Alpha Lookup timing improvements in rep_database.pc
   
      Rev 1.12   Jul 11 2001 12:25:52   dirby
   Updated version to 3.2.3.2
   
      Rev 1.11   Jun 13 2001 14:57:06   dirby
   Updated version to 3.2.3.1
   
      Rev 1.10   Jun 12 2001 10:31:18   dirby
   Updated to version 3.2.2.4
   
      Rev 1.9   Jan 09 2001 14:16:46   dirby
   Updated version number.  This marks the release where the production
   stability issue has been resolved.  Some debug code has been removed and
   added #ifndef around some Unix signal code so the data server will compile
   on NT.
   
      Rev 1.8   Oct 16 2000 12:26:44   dirby
   1.  Updated version number to 3.2.2.2 - TEST.
   2.  Added code to support XipcSystemErrReport in the event of XIPC problems.
   3.  Removed catching of OS signals CHILD and HangUP so the daemon process can catch them.
   
      Rev 1.7   Oct 05 2000 15:04:26   dirby
   Updated the version number to '3.2.2.1 - TEST'.
   This version is compiled with more diagnostic stuff
   in daemon.c.
   
      Rev 1.6   Sep 01 2000 16:19:40   dirby
   Added TrapFunction to catch Unix signals.  This will not compile under NT.
   
      Rev 1.5   Aug 31 2000 17:30:18   dirby
   Appended "TEST" to the version number to account for some
   daemon testing getting linked in.
   
      Rev 1.4   Jul 30 2000 15:37:06   dirby
   Modified to display version number at startup and shutdown.
   
      Rev 1.3   Jun 21 2000 14:14:48   lmitchel
   Select chain name(mcf01.name02) for alpha lookup
   
      Rev 1.2   Mar 22 2000 13:25:20   iarustam
   fixed the select statements for report lists 
   and added get_record for tlf01
   
      Rev 1.1   Mar 16 2000 10:34:38   iarustam
   Removed all DEVDS functionality.
   Added functions for alpha lookup gui
   
      Rev 1.1   Jan 06 2000 17:31:36   npogosya
   Modified get_tlf01_gui_list_dynamic
   
   
      Rev 1.0   Sep 21 1999 17:04:44   iarustam
   Initial Release
   
  


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
#include "rep_database.h"

#include "dbcommon.h"
#include "app_info.h"
#include "equitdb.h"
#include "tx_dbstruct.h"
#include "nc_dbstruct.h"

#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"

#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

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
PRIVATE CHAR         AtpBuffer[65000];
PRIVATE BYTE         app_data_type;
PRIVATE CHAR         applnk_que_name[8]="applnkA";

BOOLEAN   in_refeed_mode    = false;
CHAR      Version[] = "ATP_11.10.0";

INT ds_timer_flag;
INT ds_timer_value;

BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];

UINT Max_File_Size_Defined = 0 ;
/* End   */
/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
INT DB_NullCheck_Flag;
double db_error_statics_value;

PRIVATE union
{
  
   TLF01_GUI_LIST      Tlf01_Gui_List;
   MCF01_REPORT_LIST   Mcf01_Gui_List;
   CCF01_REPORT_LIST   Ccf01_Gui_List;
   PAF01_REPORT_LIST   Paf01_Gui_List;
   FGUARD_GUI_LIST     Fguard_Gui_List;

   
}db_lists;


PRIVATE struct
{
   TLF01    tlf01;
   FGUARD   fguard;
   EMV_RAW  emvraw;
   FG_RAW   fgraw;
   MRA01    mra01;
   FTLF01   ftlf01;
   PTSN01   ptsn01;
} db_recs;

extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
extern CHAR ServiceName[]; //TF PHANI
void TrapFunction( INT signum );

BYTE db_get_tlf01_gui_list_dynamic_new (pTLF01_LIMITS    Tlf01_ptr, 
                                        pTLF01_GUI_LIST  Tlf01_Gui_List,
                                        pCHAR            ErrorMsg );

BYTE db_get_fguard_gui_list_dynamic( pTLF01_LIMITS    Tlf01_ptr,
                                     pFGUARD_GUI_LIST Fguard_Gui_List,
                                     BYTE             subtype2,
                                     pCHAR            ErrorMsg );

BYTE db_get_tlf01_stip_gui_list_for_standin (pTLF01_LIMITS    Tlf01_ptr,        // ThoughtFocus Girija Y
                                        pTLF01_GUI_LIST  Tlf01_Gui_List,
                                        pCHAR            ErrorMsg );
INT  does_process_exist( pCHAR appname, pINT pid );
INT  do_queues_already_exist( pCHAR appname );
INT  destroy_queues( pCHAR appname );
INT  kill_process( pCHAR appname, INT pid );

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename(  );

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
void repds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);
INT repds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);

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

/*************************************************************************************/
/*************************************************************************************/
BOOLEAN perform_startup ()
{
   CHAR     XipcInstanceName[12];
   INT      ret_val;
   INT      instance_exists = false;
   INT      queues_exist    = false;
   INT      continue_with_startup = true;
   INT      pid=0;
   CHAR     Buffer[300] = "";

   memset (last_trid, 0, sizeof(last_trid));

   /* Try to initialise the xipc stuff */
   GetAppName          (AppName);
   GetXipcInstanceName (XipcInstanceName);

 //  #ifndef WIN32

      /* Since this is a single instance application, check to make sure
       * another instance of this application is not already running.
       */
 /*     instance_exists = does_process_exist( AppName, &pid );
      queues_exist    = do_queues_already_exist( AppName ); */

      /*+--------------------------------------------------------------+
        | We have the information we need.  Now decide on a course of  |
        | action based on the value of the 2 flags that were just set. |
        |                                                              |
        | 1. No Instance, No Queues                                    |
        |       Action: Continue with startup                          |
        |                                                              |
        | 2. No Instance, Yes Queues                                   |
        |       Action: Destroy queues, then continue with startup     |
        |                                                              |
        | 3. Yes Instance, No Queues                                   |
        |       Kill the instance, then continue with startup          |
        |                                                              |
        | 4. Yes Instance, Yes Queues                                  |
        |       Action: Do not startup                                 |
        +--------------------------------------------------------------+*/

  /*    if ( !instance_exists  &&  !queues_exist )
      {
         continue_with_startup = true;
      }
      else if ( !instance_exists  &&  queues_exist )
      {
         if ( false == destroy_queues(AppName) )
            continue_with_startup = false;
      }
      else if ( instance_exists  &&  !queues_exist )
      {
         if ( false == kill_process(AppName,pid) )
            continue_with_startup = false;
      }
      else
      {
         continue_with_startup = false;
      }

   #endif*/

//   if ( continue_with_startup )
//   {
      /* Try to connect to the database */
      if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
      {
         repds_log_message(1, 3, ErrorMsg, "perform_startup");
         return (false);
      }
      printf ("Connected to ORACLE\n");

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

     
#ifdef _DEBUG   
   if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)
#else
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName, XipcInstanceName ) )
#endif
      {     
       	   repds_log_message(1, 3, "Failed to create XIPC queues", "perform_startup");
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
         dbcommon_disconnect (ErrorMsg);
         return (false);
      }

      #ifdef USING_LOGGER_REPLY

         /* send restart command to TFLogger */
         if (SendRestartCmd (ErrorMsg) == false)
         {
             repds_log_message(1, 3, "Error sending restart command to TFLogger", "perform_startup");

            dstimer_shutdown ();
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }

         #ifdef USING_CHECKPOINTS

            /* get the checkpoint record from the database */
            if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
            {
            	repds_log_message(1, 3, ErrorMsg, "perform_startup");
            	dstimer_shutdown ();
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
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
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
               dbcommon_disconnect (ErrorMsg);
               return (false);
            }
         #endif
      #endif

      ret_val = get_dataserver_ini_timer_value( &ds_timer_flag, &ds_timer_value,
                                                AppName,         Buffer );
      if ( false == ret_val )
      {
         repds_log_message(2,2,Buffer,"perform_startup");
      }
      else if ( ds_timer_flag == DS_TIMER_ON )
      {
         sprintf( Buffer,
                 "%s:Data Server timing of queries is turned on, value is %d",
                  AppName, ds_timer_value );
         repds_log_message(2,1,Buffer,"perform_startup");
      }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   repds_log_message(2,1,Buffer,"perform_startup");

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
      repds_log_message(2,2,Buffer,"perform_startup");
   }

   return( true );
}
      
/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_gui_list_msg( pPTE_MSG p_msg_in )
{
   BYTE           subtype2;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

   memcpy( AtpBuffer, p_data, sizeof(AUTH_TX) );

   p_current     = AtpBuffer + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = p_data + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case TLF01_DATA:
		 if( ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_STANDIN )

		  {
			 PRINT( "request to retrieve tlf01_stip gui list\n" );
			 if ( ( result_code = db_get_tlf01_stip_gui_list_for_standin( (pTLF01_LIMITS)p_data, &db_lists.Tlf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
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
		  }
		 else
		 {
         PRINT( "request to retrieve tlf01 gui list\n" );
         if( ( result_code = db_get_tlf01_gui_list_dynamic_new( (pTLF01_LIMITS)p_data, &db_lists.Tlf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
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
		 }
		 break;

      case TLF01_STIP_DATA :
 		 if( ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_STANDIN )

 		  {
 			 PRINT( "request to retrieve tlf01_stip gui list\n" );
 			 if ( ( result_code = db_get_tlf01_stip_gui_list_for_standin( (pTLF01_LIMITS)p_data, &db_lists.Tlf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
 			 {


             	memcpy(p_current, (BYTE *)&db_lists.Tlf01_Gui_List, sizeof( db_lists.Tlf01_Gui_List ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                           ptemsg_get_msg_subtype1 (p_msg_in),
                                           ptemsg_get_msg_subtype2 (p_msg_in),
                                           reply_que,
                                           application_que_name,
                                           (pBYTE)AtpBuffer,
                                           sizeof(AUTH_TX) + sizeof(db_lists.Tlf01_Gui_List),
										   TLF01_STIP_DATA);
 			 }
 		  }
 		 break;

      case MCF01_DATA:

         PRINT( "request to retrieve mcf01 gui list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_mcf01_gui_list( ( pMCF01 )p_data, &db_lists.Mcf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
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
         break;

       case CCF01_DATA:

         PRINT( "request to retrieve ccf01 gui list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_ccf01_gui_list( ( pCCF01 )p_data, &db_lists.Ccf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Ccf01_Gui_List, sizeof( db_lists.Ccf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Mcf01_Gui_List),
                                          CCF01_DATA);
         }
         break;

       case PAF01_DATA:

         PRINT( "request to retrieve paf01 gui list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_paf01_gui_list( ( pPAF01 )p_data, &db_lists.Paf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Paf01_Gui_List, sizeof( db_lists.Paf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Mcf01_Gui_List),
                                          PAF01_DATA);
         }
         break;

      case FGUARD_DATA:

         subtype2 = ptemsg_get_msg_subtype2( p_msg_in );
         result_code = db_get_fguard_gui_list_dynamic((pTLF01_LIMITS)p_data,
                                                      &db_lists.Fguard_Gui_List,
                                                       subtype2,
                                                       ErrorMsg );

         if( result_code  == PTEMSG_OK )
         {
            memcpy( p_current,
                   (pBYTE)&db_lists.Fguard_Gui_List,
                    sizeof(db_lists.Fguard_Gui_List) );

            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_lists.Fguard_Gui_List),
                                          FGUARD_DATA);
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
pPTE_MSG process_select_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   CHAR           temp[150];
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
      case TLF01_DATA:
         PRINT( "request to retrieve tlf01 record\n" );
         PRINT("\n");
         if(ptemsg_get_msg_subtype2 (p_msg_in) ==  0)
         {

            if ( ( result_code = db_select_tlf01( ( pCHAR)p_data, &db_recs.tlf01, ErrorMsg ) ) == PTEMSG_OK )             
            {
            	/* continue to look for EMV data -  don't care success or not.
            	 * if fail, db_recs.emvraw will be all ZEROs.
            	 */
            	db_select_emvraw( ( pCHAR)p_data, &db_recs.emvraw, ErrorMsg );

            	memcpy(p_current, (BYTE *)&db_recs.tlf01, sizeof( db_recs.tlf01 ));

				p_current = p_current + sizeof( db_recs.tlf01 );
				memcpy(p_current, (BYTE *)&db_recs.emvraw, sizeof( db_recs.emvraw ));
               
            	p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX) + sizeof(db_recs.tlf01) + sizeof( db_recs.emvraw ),
                                          TLF01_DATA);

            	sprintf(temp, "TLF01 transaction_id : %s\n", db_recs.tlf01.primary_key.transaction_id);
            	PRINT(temp);
            }
         }
         break;

      case TLF01_STIP_DATA :
    	  if ( ( result_code = db_select_tlf01_stip( ( pCHAR)p_data, &db_recs.tlf01, ErrorMsg ) ) == PTEMSG_OK )
		  {
    		      memcpy(p_current, (BYTE *)&db_recs.tlf01, sizeof( db_recs.tlf01 ));
				  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
						  	  	  	  	        ST1_DB_SELECT,
												0,
												reply_que,
												application_que_name,
												(pBYTE)AtpBuffer,
												sizeof(AUTH_TX)+ sizeof(db_recs.tlf01),
												TLF01_STIP_DATA);
		  }
       		 break;

      case FGUARD_DATA:

         result_code = db_select_fguard( (pCHAR)p_data, &db_recs.fguard, ErrorMsg );
         if ( result_code == PTEMSG_OK )             
         {
            /* Continue to look for XML raw data message.
             * No need to worry if it is not found.
             * If it fails, db_recs.fgraw will be all ZEROs.
             */
            db_select_fgraw( (pCHAR)p_data, &db_recs.fgraw, ErrorMsg );

            memcpy( p_current, (BYTE *)&db_recs.fguard, sizeof(db_recs.fguard) );

            p_current = p_current + sizeof( db_recs.fguard );
            memcpy( p_current, (BYTE *)&db_recs.fgraw, sizeof( db_recs.fgraw ));

            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          sizeof(AUTH_TX)
                                             + sizeof(db_recs.fguard)
                                             + sizeof(db_recs.fgraw ),
                                          FGUARD_DATA );
         }
         break;

      case MRA01_DATA:
    	  result_code = db_select_mra01( (pCHAR)p_data, &db_recs.mra01, ErrorMsg );
    	  if ( result_code == PTEMSG_OK  || result_code == PTEMSG_NOT_FOUND )
    	  {
			  memcpy(p_current, (BYTE *)&db_recs.mra01, sizeof( db_recs.mra01 ));
			  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											ST1_DB_SELECT,
											0,
											reply_que,
											application_que_name,
											(pBYTE)AtpBuffer,
											sizeof(AUTH_TX)+ sizeof(db_recs.mra01),
											MRA01_DATA);
		  }

    	  break;

      case FTLF01_DATA:
    	    result_code = db_select_ftlf01( ( pCHAR)p_data, &db_recs.ftlf01, ErrorMsg );
    		if ( result_code == PTEMSG_OK  || result_code == PTEMSG_NOT_FOUND )
    		{
    		      memcpy(p_current, (BYTE *)&db_recs.ftlf01, sizeof( db_recs.ftlf01 ));
				  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
						  	  	  	  	        ST1_DB_SELECT,
												0,
												reply_que,
												application_que_name,
												(pBYTE)AtpBuffer,
												sizeof(AUTH_TX)+ sizeof(db_recs.ftlf01),
												FTLF01_DATA);

				  if(result_code == PTEMSG_NOT_FOUND)
				  {
					  p_msg_out->result_code= PTEMSG_NOT_FOUND;
				  }
		  }
    	  break;

      case PTSN01_DATA:

  	    result_code = db_select_ptsn01( ( pCHAR)p_data, &db_recs.ptsn01, ErrorMsg );
  		if ( result_code == PTEMSG_OK  || result_code == PTEMSG_NOT_FOUND )
  		{
  		      memcpy(p_current, (BYTE *)&db_recs.ptsn01, sizeof( db_recs.ptsn01 ));
				  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
	  	  	  	  	        					ST1_DB_SELECT,
												0,
												reply_que,
												application_que_name,
												(pBYTE)AtpBuffer,
												sizeof(AUTH_TX)+ sizeof(db_recs.ptsn01),
												PTSN01_DATA);

				  if(result_code == PTEMSG_NOT_FOUND)
				  {
					  p_msg_out->result_code= PTEMSG_NOT_FOUND;
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
  
   MsgSubtype2 = ptemsg_get_msg_subtype2 (p_msg_in);
   MsgSubtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   DB_MsgSubtype2 = '0';
   DB_MsgSubtype1 = '0';
   DB_MsgSubtype2 = MsgSubtype2 ;
   DB_MsgSubtype1 = MsgSubtype1 ;

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

    case ST1_DB_GET_GUI_LIST      : p_msg_out = process_get_gui_list_msg (p_msg_in);
                                      break;
    case ST1_DB_SELECT            : p_msg_out = process_select_msg       (p_msg_in);
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
        repds_log_message(2, 2, errbuf, "process_db_request");
      }
   }
   return (p_msg_out);
}

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
   int         refresh_flag = 0;  // If an error in update_mem_record or delete_mem_record,
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
   //p_data = p_data + sizeof(AUTH_TX);
  
   while ( (i <= num_messages) && (p_cur_msg != NULL_PTR) )
   {
      reply_que = ptemsg_get_msg_orig_queue (p_cur_msg);
      strcpy(orig_comm_inf, ptemsg_get_msg_orig_comms_info( p_cur_msg ));
      app_data_type = 0;
      DB_reconnect = 0;
      switch( ptemsg_get_msg_type( p_cur_msg ) )
      {
         case MT_TIMER_REQUEST  : p_tmp_msg = dstimer_process_request (p_cur_msg,
                                                                       &updates_to_db_made);       
                                  break;            
         case MT_SYSTEM_REQUEST : ptesystm_system_msg_handler (p_cur_msg);
                                  if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
                                  {
                                     if ( ST2_NO_VERSION != ptemsg_get_msg_subtype2(p_msg_in) )
                                     {
                                        /* When pinged, display application version on Monitor. */
                                        sprintf( temp_str,
                                                "Pinged -> Data Server: %s, version %s",
                                                 AppName, Version );
                                        repds_log_message(1, 1, temp_str, "PinnacleMsgHandler");
                                        /* Get Database Timing Parameters to gather
                                         * statistics about each query.
                                         */
                                        memset( ReportTime, 0x00, sizeof(ReportTime) );
                                        memset( temp_str,   0x00, sizeof(temp_str)   );
                                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                                          ReportTime, 
                                                                         &ForceReport,
                                                                          temp_str );
                                        repds_log_message(2,1,temp_str,"PinnacleMsgHandler");

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
                                             repds_log_message(2,1,temp_str,"PinnacleMsgHandler");

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

         case MT_DB_REQUEST     : p_tmp_msg = process_db_request (p_cur_msg);
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

      if (result_code != PTEMSG_OK )
      {
    	  if( result_code == PTEMSG_NOT_FOUND &&
    		  (app_data_type == MRA01_DATA|| app_data_type == FTLF01_DATA || app_data_type == PTSN01_DATA))
    	  {
    		  // In not found case also reply must be sent.
    	  }
    	  else
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
      
   //}

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
         repds_log_message(1, 2, ErrorMsg, "PinnacleMsgHandler");
         result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
      repds_log_message(2,2,ErrorMsg,"PinnacleMsgHandler");
   }*/
   if(DB_reconnect == 1)
   {
	   dbcommon_disconnect (ErrorMsg);
	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
	   {
		   strcat (ErrorMsg, " DB Reconnection failed");
		   repds_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler");
			return (false);
	   }
	   else
	   {
		   repds_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler");
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

#ifndef WIN32

void TrapFunction( INT signum )
{
   CHAR Buffer[100];

   XIPC_TRAP_FUNCTION_TEST( TrapFunction, signum );

   memset( Buffer, 0x00, sizeof(Buffer) );
   sprintf( Buffer, "%s:  Caught signal %d", AppName, signum );
   repds_log_message(2,3,ErrorMsg,"TrapFunction");
}

#endif

void MainProcessor()
{
   LONG     ipc_rcode;//, xipc_code;
   pPTE_MSG p_msg;
   CHAR     Buffer[256];
   CHAR     MonAdd[100]="";
   INT      num_sql;
   INT      err_ctr = 0;
   CHAR     strError[512] = {0} ;
   INT 		ret_val=0;
   GetAppName( AppName );
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
			
	ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                      ServiceName,     Buffer );
   repds_log_message(2,1,Buffer,"MainProcessor");
	printf( "%s\n", Buffer );

   #ifdef _DEBUG
      pteipc_trace_on();
   #endif

   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	  	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
	  	    repds_log_message(2,1,Buffer,"MainProcessor");
			printf( "%s\n", Buffer );

      return;
   }

   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
   repds_log_message(2,1,Buffer,"MainProcessor");
   printf( "%s\n", Buffer );

   printf( "DataServer is ready. press <ENTER> to exit\n" );

   while( !EndProcessSignalled )
   {
      /* You are blocked here waiting for a message on either app queue or
       * control queue.  If there is no message on either queue for the
       * specified time, the blocking call returns.
       */
      p_msg = pteipc_receive( application_que_name, control_que_name, 
                              TIME_BETWEEN_CHECKS, &ipc_rcode );

      if( p_msg != NULL_PTR )
      {
         err_ctr = 0;
         PinnacleMsgHandler( p_msg );
         free (p_msg);
      }
      else
      {
         if( ipc_rcode != QUE_ER_TIMEOUT ) 
         {
            pteipc_get_errormsg ( ipc_rcode, ErrorMsg );
            sprintf ( Buffer,
                     "Error on pteipc_receive %s, error code: %ld",
                      ErrorMsg, ipc_rcode );
            repds_log_message(1, 3, Buffer, "MainProcessor");

            /* Comment the following out after the production stability
             * problem is resolved.
             */
            /* This line is to gather XIPC info *
            memset( XipcErrBuffer, 0x00, sizeof(XipcErrBuffer) );
            XipcSystemErrorReport( XipcErrBuffer, sizeof(XipcErrBuffer)-1 );
            repds_log_message(2,3,XipcErrBuffer,"MainProcessor");
            exit(1);
            */

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
               sprintf( Buffer,
                       "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
                        AppName );
               repds_log_message(1, 3, Buffer, "MainProcessor");
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
               repds_log_message(2,1,Buffer,"MainProcessor");
               reset_timing_stats( &TimingStats );
               NextReportTime = set_next_report_time_at_startup( ReportTime );
            }
         }
      }
      dstimer_check_for_timeouts ();
   }

   dstimer_shutdown ();

   /* shut down ipc */
//   pteipc_shutdown_single_instance_app();  /* Changed single to multiple instance - Girija Y ThoughtFocus */
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
            AppName, Version );
   repds_log_message(2,1,Buffer,"MainProcessor");
   strcat( Buffer, "\n" );
   PRINT( Buffer );

   pteipc_sleep (3000);
}

void get_epoch_time_from_normal_date(char *dateStr, char *timeStr, char *epochStr)
{
	time_t epoch;
	struct tm my_tm;
	char timeBuf[30] = "";
	char buffer[30] = "";
	char dateStr_mm[3] = {0}, dateStr_dd[3] = {0}, dateStr_yyyy[5] = {0};
	char timeStr_hh[3] = {0}, timeStr_mm[3] = {0};

    strncpy(dateStr_mm, dateStr+4, 2);
    strncpy(dateStr_dd, dateStr+6, 2);
    strncpy(dateStr_yyyy, dateStr, 4);

    strncpy(timeStr_hh, timeStr,   2);
    strncpy(timeStr_mm, timeStr+2, 2);

    memset(&my_tm, 0, sizeof(my_tm));
    my_tm.tm_mon  = atoi(dateStr_mm);
    my_tm.tm_mday = atoi(dateStr_dd);
    my_tm.tm_year = atoi(dateStr_yyyy);
    my_tm.tm_hour = atoi(timeStr_hh);
    my_tm.tm_min  = atoi(timeStr_mm);
    my_tm.tm_sec  = 00;

    my_tm.tm_isdst = -1;
    my_tm.tm_year -= 1900;
    my_tm.tm_mon -= 1;

    epoch = mktime(&my_tm);

    if (epoch == -1)
    {
    	repds_log_message(2,3,"Error: unable to make time using mktime","get_epoch_time_from_normal_date");
        return;
    }

    sprintf(buffer, "%ld", (long)epoch);
    memcpy(&epochStr[0], &buffer[0], strlen(buffer));
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
   CHAR  repds_error_warning_file_size[5] = {0};

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
							  repds_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(repds_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(repds_error_warning_file_size);
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
 *  NAME:         repds_log_message
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
void repds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func )
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
   repds_Log_error_warning_to_File(text_message,sev,func);
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

INT repds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	FILE *fp;


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
	fclose(fp);
	return  0;
}

#ifndef WIN32

/******************************************************************************
 *
 *  NAME:         DOES_PROCESS_EXIST
 *
 *  DESCRIPTION:  This function determines if this application is already
 *                running.  It does this by checking to see if there are
 *                more than 2 processes running by this name: one for this
 *                process and one for a previous process.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      pid = Process ID of a previous process already running.
 *
 *  RTRN VALUE:   True if there is another process running by the same name
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT does_process_exist( pCHAR appname, pINT pid )
{
   INT    ret_val = false;
   INT    ctr;
   INT    my_pid;
   INT    found_pid;
   FILE  *fptr;
   CHAR   s_pid[9]="";
   CHAR   list_processes[100]="";
   CHAR   get_pid_cmd[200]="";
   CHAR   buffer[200]="";

   /* ----------------------------------------------------------------------- */
   /* Determine if there are multiple process Ids out there for this process. */
   /* ----------------------------------------------------------------------- */

   /* Create Unix command to list all processes for this app. */
   sprintf( list_processes,
           "ps -ef | grep -i %s | grep -v grep",
            appname );

   /* Run the command, creating a file pointer to the output. */
   ctr = 0;
   if ((fptr = popen(list_processes,"r")) != NULL )
   {
      while( fgets(buffer,100,fptr) != NULL )
         ctr++;

      pclose( fptr );
   }

   if ( ctr > 1 )
   {
      /* ------------------------ */
      /* There are multiple PIDs. */
      /* ------------------------ */
      ret_val = true;

      /* There are more than 1 processes for this app.
       * 1.  First get pid of this app.
       * 2.  Get pid of other app.  Return this one.
       */
      my_pid = getpid();

      /* Get PID of app that was already running (assuming just 1 other). */
      sprintf( get_pid_cmd,
              "ps -ef | fgrep %s | fgrep -v grep \
               | awk '{ while( sub(/^[ ]/,\"\") );while (gsub(/[ ]{2,}/,\" \",$0));\
               ;split($0,a,/[ ]/); print a[2] }'",
               appname );

      /* Run the command to get the Process ID. */
      if ((fptr = popen(get_pid_cmd,"r")) != NULL )
      {
         while( fgets(s_pid,8,fptr) != NULL )
         {
            found_pid = atoi( s_pid );

            if ( my_pid != found_pid )
            {
               /* Found it. Let's get out of the loop. */
               *pid = found_pid;
               break;
            }
         }
         pclose( fptr );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         DO_QUEUES_ALREADY_EXIST
 *
 *  DESCRIPTION:  This function determines if there are XIPC queues for this
 *                application already created.  It logs into XIPC without
 *                creating queues; then attempts to access the 'A' queue.
 *                If it can access it, then the queues exist already.
 *
 *  INPUTS:       appname - Name of this application
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if queues already exist
 *                False if queues exist, or if unable to determine
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT do_queues_already_exist( pCHAR appname )
{
   INT    ret_val = false;
   BYTE   temp_inst[MAX_INSTANCE_NAME_SIZE]="";
   LONG   rcode;
   INT    application_que_id;
   CHAR   application_que_name[20]="";
   CHAR   ErrorMsg[100]="";

   /* Get the XIPC instance name, then log
    * into XIPC so we can check the queues. 
    */
   pteipc_format_xipc_instance_name( temp_inst );
   rcode = pteipc_xipclogin( temp_inst, appname );
   if( rcode < 0 )
   {
      sprintf( ErrorMsg, "Log into XIPC failed, %s", appname );
      repds_log_message( 1, 3, ErrorMsg, "do_queues_already_exist" );
   }
   else
   {
      /* Attempt to access the "A" queue. */
      sprintf( application_que_name, "%sA", appname );
      application_que_id = QueAccess( application_que_name );
      if( application_que_id >= 0 )
      {
         /* Queues already exist. */
         ret_val = true;
      }

      /* Log out of XIPC. */
      rcode = pteipc_xipclogout();
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         DESTROY_QUEUES
 *
 *  DESCRIPTION:  This function destroys its own XIPC queues.
 *                Need to log into XIPC to do this.
 *
 *  INPUTS:       appname - Name of this application
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if queues are successfully destroyed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT destroy_queues( pCHAR appname )
{
   INT   ret_val = true;
   BYTE   temp_inst[MAX_INSTANCE_NAME_SIZE]="";
   LONG   rcode;
   CHAR   ErrorMsg[100]="";

   INT    que_id;
   CHAR   que_name[20] ="";

   /* Get the XIPC instance name, then log
    * into XIPC so we can check the queues. 
    */
   pteipc_format_xipc_instance_name( temp_inst );
   rcode = pteipc_xipclogin( temp_inst, appname );
   if( rcode < 0 )
   {
      ret_val = false;
      sprintf( ErrorMsg, "Log into XIPC failed, %s", appname );
      repds_log_message( 1, 3, ErrorMsg, "destroy_queues" );
   }
   else
   {
      /* Destroy the "A" queue. */
      sprintf( que_name, "%sA", appname );
      que_id = QueAccess( que_name );
      if( que_id >= 0 )
         QueDestroy( que_id );

      /* Destroy the "I" queue. */
      sprintf( que_name, "%sI", appname );
      que_id = QueAccess( que_name );
      if( que_id >= 0 )
         QueDestroy ( que_id );

      /* Destroy the "C" queue. */
      sprintf( que_name, "%sC", appname );
      que_id = QueAccess( que_name );
      if( que_id >= 0 )
         QueDestroy ( que_id );

      /* Log out of XIPC. */
      rcode = pteipc_xipclogout();
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         KILL_PROCESS
 *
 *  DESCRIPTION:  This function kills the process of the given process Id.
 *
 *  INPUTS:       appname - Name of application to be killed
 *                pid     - Process ID of the process to be killed
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if process is successfully killed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT kill_process( pCHAR appname, INT pid )
{
   #define MAX_WAITS  6
   #define WAIT_INTERVAL 5000000  /* 5 seconds */

   INT   ret_val = true;
   INT   wait_ctr;
   INT   process_exists;
   CHAR  ErrorMsg[100]="";

   if ( pid > 1 )
   {
      if ( -1 == kill(pid, SIGTERM) )
      {
         /* Failed to kill process. */
         ret_val = false;
      }
      else
      {
         /* Successful kill, but make sure process is gone. */
         for( wait_ctr=0; wait_ctr < MAX_WAITS; wait_ctr++ )
         {
            process_exists = does_process_exist( appname, &pid );
            if ( true == process_exists )
            {
               /* Still running. Wait, then check again. */
               usleep( WAIT_INTERVAL );
            }
            else
               break;
         }

         if ( process_exists )
         {
            /* Process never died within allotted time. */
            ret_val = false;
         }
      }
   }
   else
   {
      ret_val = false;
   }

   if ( ret_val == false )
   {
      sprintf( ErrorMsg,
              "Unable to kill process %s, PID: %d",
               appname, pid );
      repds_log_message( 1, 3, ErrorMsg,"kill_process" );
   }
   return( ret_val );
}


#endif



/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/
