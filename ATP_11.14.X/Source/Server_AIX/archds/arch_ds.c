/******************************************************************************

   Module: archds.c

   Title: Archive Server Dataserver Routines

   Description: 

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\archds\arch_ds.c  $1025

 
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
#include "arch_database.h"
#include "dc_dbstruct.h"
#include "dc_database.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"

#include "archds.h"

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
CHAR      Version[] = "ATP_11.1.0";

CHAR  archds_Erro_warning_Filename[256] = {0} ;
CHAR  archds_module_error_warning_file_name[256] = {0};
CHAR  archds_error_warning_file_path[256] = {0};

UINT Max_File_Size_Defined = 0 ;

INT ds_timer_flag;
INT ds_timer_value;

INT db_error_statics_flag;
INT DB_NullCheck_Flag;

PRIVATE union
{
  
   ARCHIVE_TLF01_GUI_LIST      Tlf01_Gui_List;
   ARCHIVE_BCH01_GUI_LIST	   Bch01_Gui_List;
   ARCHIVE_BCH10_GUI_LIST	   Bch10_Gui_List;
   ARCHIVE_BCH11_GUI_LIST	   Bch11_Gui_List;
   ARCHIVE_BCH20_GUI_LIST	   Bch20_Gui_List;
}db_lists;


PRIVATE struct
{
   ARCHIVE_TLF01    tlf01;
   FGUARD   fguard;
   EMV_RAW  emvraw;
   FG_RAW   fgraw;

} db_recs;

extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;
extern CHAR ServiceName[]; //TF PHANI
void TrapFunction( INT signum );

BYTE db_get_tlf01_gui_list_dynamic_new (pARCHIVE_TLF01_LIMITS    Tlf01_ptr, 
                                        pARCHIVE_TLF01_GUI_LIST  Tlf01_Gui_List,
                                        pCHAR            ErrorMsg );


BYTE db_get_tlf01_stip_gui_list_for_standin (pARCHIVE_TLF01_LIMITS    Tlf01_ptr,        // ThoughtFocus Girija Y
                                        pARCHIVE_TLF01_GUI_LIST  Tlf01_Gui_List,
                                        pCHAR            ErrorMsg );
INT  does_process_exist( pCHAR appname, pINT pid );

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
double        NextReportTime;
CHAR          ReportTime[5];

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

/*************************************************************************************
 NAME:           GetArchiveServiceName()
 DESCRIPTION:    This function will Get  Archive Service Name from tf.ini
 INPUTS:           pCHAR archds_srvc_name --> to store the Archive Service Name from tf.ini
 OUTPUTS:        None                                
 RETURNS:        1 .                       
                               
 AUTHOR:		Abhishek Verma.          
 MODIFIED BY:     

/*************************************************************************************/

BOOLEAN GetArchiveServiceName ( pCHAR archds_srvc_name )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "ARCHIVE_SERVICE_NAME",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(archds_srvc_name, tmpstr);

   return 1;
} /* end of GetAppServiceName */


/*************************************************************************************
 NAME:           GetArchivePassword()
 DESCRIPTION:    This function will get from Archive Password from tf.ini
 INPUTS:          pCHAR archds_password --> to store Archive Password from tf.ini
 OUTPUTS:        None                                
 RETURNS:        1                       
                               
 AUTHOR:		Abhishek Verma.          
 MODIFIED BY:     

/*************************************************************************************/

BOOLEAN GetArchivePassword ( pCHAR archds_password )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "ARCHIVE_PASSWORD",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   DecryptPassword(tmpstr, archds_password);

   return 1;
}

/*************************************************************************************
 NAME:           GetArchiveUserId()
 DESCRIPTION:    This function will Get Archive UserId from tf.ini
 INPUTS:          pCHAR archds_user_id  -> to store the Archive UserId from tf.ini
 OUTPUTS:       None                                
 RETURNS:       1                    
                              
 AUTHOR:		Abhishek Verma.          
 MODIFIED BY:     

/*************************************************************************************/


BOOLEAN GetArchiveUserId ( pCHAR archds_user_id )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "ARCHIVE_USER_ID",             /*  points to key name  */
    "CORE",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(archds_user_id, tmpstr);

   return 1;


}


/*************************************************************************************/
INT isnum(pCHAR string , INT length)
{
   INT i;

   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}

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

      /* Try to connect to the database */
      if (dbcommon_connect_Archds ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
      {
    	  archds_log_message( 1, 3, ErrorMsg, "perform_startup", 0 );
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
         archds_log_message( 1, 3, "Failed to create XIPC queues",
        		 	 	 	 	   "perform_startup", 0 );
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
            archds_log_message( 1, 3, "Error sending restart command to TFLogger",
                    		 	 	  "perform_startup", 0 );

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
               archds_log_message( 1, 3, ErrorMsg,"perform_startup", 0 );

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
         archds_log_message( 2, 2, Buffer, "perform_startup", 0 );
      }
      else if ( ds_timer_flag == DS_TIMER_ON )
      {
         sprintf( Buffer,
                 "%s:Data Server timing of queries is turned on, value is %d",
                  AppName, ds_timer_value );
         archds_log_message( 2, 1, Buffer, "perform_startup", 0 );
      }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   archds_log_message( 2, 1, Buffer, "perform_startup", 0 );
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
      archds_log_message( 2, 2, Buffer, "perform_startup", 0 );
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

   //printf(" Entering process_get_gui_list_msg\n");

   switch( app_data_type )
   {
      case ARCH01_DATA:
		 if( ptemsg_get_msg_subtype2 (p_msg_in) == TLF01_DATA )
		 {
			 PRINT( "request to retrieve tlf01 gui list\n" );
			 if( ( result_code = db_get_tlf01_gui_list_dynamic_new( (pARCHIVE_TLF01_LIMITS)p_data, &db_lists.Tlf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				 memcpy(p_current, (BYTE *)&db_lists.Tlf01_Gui_List, sizeof( db_lists.Tlf01_Gui_List ));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
					 ptemsg_get_msg_subtype1 (p_msg_in),
					 ptemsg_get_msg_subtype2 (p_msg_in),
					 reply_que,
					 application_que_name,
					 (pBYTE)AtpBuffer,
					 sizeof(AUTH_TX) + sizeof(db_lists.Tlf01_Gui_List),
					 ARCH01_DATA);
				 /*if (p_msg_out != NULL)
				 {
						 printf(" Data retrive from TLF01 successfully\n");
				 }
				 else
				 {
				 	printf(" Error in reteriving data from TLF01 \n");
        		 }*/
				 
			 }
			 else
			 {
				printf(" Error in reteriving data from TLF01:db_get_tlf01_gui_list_dynamic_new %d \n",result_code);
			 }
		 }
		 else if( ptemsg_get_msg_subtype2 (p_msg_in) == BCH01_DATA )
		 {
		 	if( ( result_code = db_get_bch01_gui_list( (pARCHIVE_BCH01) p_data, &db_lists.Bch01_Gui_List, ErrorMsg) ) == PTEMSG_OK )
	 		{
	 			memcpy(p_current, (BYTE *)&db_lists.Bch01_Gui_List, sizeof( db_lists.Bch01_Gui_List ));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
					 ptemsg_get_msg_subtype1 (p_msg_in),
					 ptemsg_get_msg_subtype2 (p_msg_in),
					 reply_que,
					 application_que_name,
					 (pBYTE)AtpBuffer,
					 sizeof(AUTH_TX) + sizeof(db_lists.Bch01_Gui_List),
					 ARCH01_DATA);
				 /* if (p_msg_out != NULL)
				 {
						 printf(" Data retrive from BCH01 successfully\n");
				 }
				 else
				 {
				 		printf(" Error in reteriving data from BCH01\n");
        		 }*/
				 
	 		}
			else
			{
				printf(" Error in reteriving data from BCH01:db_get_bch01_gui_list%d \n",result_code);
			}
		 }
         else if( ptemsg_get_msg_subtype2 (p_msg_in) == BCH11_DATA )
		 {
		 	if( ( result_code = db_get_bch11_gui_list( (pARCHIVE_BCH01) p_data, &db_lists.Bch11_Gui_List, ErrorMsg) ) == PTEMSG_OK )
	 		{
	 			memcpy(p_current, (BYTE *)&db_lists.Bch11_Gui_List, sizeof( db_lists.Bch11_Gui_List ));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
					 ptemsg_get_msg_subtype1 (p_msg_in),
					 ptemsg_get_msg_subtype2 (p_msg_in),
					 reply_que,
					 application_que_name,
					 (pBYTE)AtpBuffer,
					 sizeof(AUTH_TX) + sizeof(db_lists.Bch11_Gui_List),
					 ARCH01_DATA);
				   /*if (p_msg_out != NULL)
				   {
				 		 printf(" Data retrive from BCH11 successfully\n");
				   }
				   else
				   {
				 		printf(" Error in reteriving data  from BCH11 \n");
        		   }*/
				 
	 		}
			else
			{
				printf(" Error in reteriving data from BCH01:db_get_bch11_gui_list%d \n",result_code);
			}
		 }
         else if( ptemsg_get_msg_subtype2 (p_msg_in) == BCH20_DATA )
		 {
		 	if( ( result_code = db_get_bch20_gui_list( (pARCHIVE_BCH01) p_data, &db_lists.Bch20_Gui_List, ErrorMsg) ) == PTEMSG_OK )
	 		{
	 			memcpy(p_current, (BYTE *)&db_lists.Bch20_Gui_List, sizeof( db_lists.Bch20_Gui_List ));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
					 ptemsg_get_msg_subtype1 (p_msg_in),
					 ptemsg_get_msg_subtype2 (p_msg_in),
					 reply_que,
					 application_que_name,
					 (pBYTE)AtpBuffer,
					 sizeof(AUTH_TX) + sizeof(db_lists.Bch20_Gui_List),
					 ARCH01_DATA);
				   /*if (p_msg_out != NULL)
				   {
						 printf(" Data retrive from BCH20 successfully\n");
				   }
				   else
				   {
				 		printf(" Error in reteriving data from BCH20\n");
        		   }*/
				 
	 		}
			else
			{
				printf(" Error in reteriving data from BCH01:db_get_bch20_gui_list%d \n",result_code);
			}
		 }
		 else
		 {
		 	result_code = PTEMSG_INVALID_DATATYPE;
         	sprintf (ErrorMsg, "Unknown msg subtype %d for GetList request from %s", ptemsg_get_msg_subtype2 (p_msg_in), reply_que);
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

   /*if(p_msg_out==NULL_PTR)
   {
		printf("p_msg_out is NULL with result_code : %d \n",result_code);
   }
   else
   {
   		printf("process_get_gui_list_msg Successfull with result_code : %d \n",result_code);
   }*/
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
      case ARCH01_DATA:
         PRINT( "request to retrieve tlf01 record\n" );
         PRINT("\n");
         if(ptemsg_get_msg_subtype2 (p_msg_in) ==  TLF01_DATA)
         {
            if ( ( result_code = db_select_tlf01( ( pCHAR)p_data, &db_recs.tlf01, ErrorMsg ) ) == PTEMSG_OK )             
            {
            	/* continue to look for EMV data -  don't care success or not.
            	 * if fail, db_recs.emvraw will be all ZEROs.
            	 */
            	//db_select_emvraw( ( pCHAR)p_data, &db_recs.emvraw, ErrorMsg );
            	memset((BYTE *)&db_recs.emvraw, 0, sizeof( db_recs.emvraw ));

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
                                          ARCH01_DATA);

            	sprintf(temp, "TLF01 transaction_id : %s\n", db_recs.tlf01.primary_key.transaction_id);
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

   //printf(" Entering process_db_request \n");

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
         /* The query exceeded the time criteria.  Log informational message. */
         archds_log_message( 2, 1, errbuf, "process_db_request", 0 );
      }
   }

   /*if(p_msg_out==NULL_PTR)
   {
		  printf("p_msg_out is NULL with result_code : %d \n",result_code);
   }
   else
   {
	  printf("process_db_request Successfull \n");
   }*/

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
                                        archds_log_message( 1, 1, temp_str,"PinnacleMsgHandler", 0 );

                                        /* Get Database Timing Parameters to gather
                                         * statistics about each query.
                                         */
                                        memset( ReportTime, 0x00, sizeof(ReportTime) );
                                        memset( temp_str,   0x00, sizeof(temp_str)   );
                                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                                          ReportTime, 
                                                                         &ForceReport,
                                                                          temp_str );
                                        archds_log_message( 2, 1, temp_str, "PinnacleMsgHandler", 0 );
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
                                              archds_log_message( 2, 1, temp_str, "PinnacleMsgHandler", 0 );
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


      if (result_code != PTEMSG_OK)
      	{
      		//printf("result_code is not PTEMSG_OK and its value is :%d\n",result_code);
         	break;
      	}

	 /* if(p_tmp_msg == NULL_PTR)
	  {
			printf("p_tmp_msg is NULL\n");
			
	  }*/
      if (p_tmp_msg != NULL_PTR)
      {
         result_code = ptemsg_get_result_code (p_tmp_msg);
         
         if (result_code != PTEMSG_OK)
         {
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
	 //printf(" No of msgs : %d \n",num_messages );
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
   
   //printf(" Reply queue :  %s \n",reply_que );
   /*if(p_chn_out == NULL)
   {
   		printf("p_chn_out is NULL\n");
   }*/
   

   if (p_chn_out != NULL_PTR)
   {
      /*
       * If in refeed mode, don't send the response.
       */
      if (in_refeed_mode == true)
      	{
      	printf(" Exit at Refeed MODE \n" );
         return (PTEMSG_OK);
      	}
      /*
       * If the result is OK, don't send the response if they don't want it.
       */
      if ( (result_code                                == PTEMSG_OK                 ) &&
           (ptemsg_get_type_of_reply_needed (p_msg_in) == PTEMSG_REPLY_ONLY_IF_FAILS) )
      {
		 printf(" Exit at ONLY FAILS \n" );
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
         archds_log_message( 1, 2, ErrorMsg,"PinnacleMsgHandler", 0 );
         result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
	  /*else
	  {
			printf(" response send to %s with Result Code: %d \n",reply_que, result_code);
	  }*/
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
      archds_log_message( 2, 2, ErrorMsg, "PinnacleMsgHandler", 0 );
   }*/
	 //printf(" Exit at END , I am DONE .. \n" );
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
   archds_log_message( 2, 3, Buffer, "TrapFunction", 0 );
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
   INT ret_val=0;
   GetAppName( AppName );
   archds_get_error_warning_file_name_path();
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
   archds_log_message( 2, 1, Buffer, "MainProcessor", 0 );

   #ifdef _DEBUG
      pteipc_trace_on();
   #endif
    ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                            ServiceName,     Buffer );
   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	  	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s",
            AppName, Version );
		  archds_log_message( 2, 1, Buffer, "MainProcessor", 0 );

      return;
   }

   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            AppName, Version );
   archds_log_message( 2, 1, Buffer, "MainProcessor", 0 );

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
            archds_log_message( 1, 2, Buffer,"MainProcessor", 0 );
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
               archds_log_message( 1, 2, Buffer,"MainProcessor", 0 );
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
               archds_log_message( 2, 1, Buffer, "MainProcessor", 0 );
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
   archds_log_message( 2, 1, Buffer, "EndProcess", 0 );
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

void archds_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  archds_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   GetAppName (appname) ;

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   archds_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(archds_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  archds_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(archds_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  archds_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(archds_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(archds_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(archds_error_warning_file_path) > 0) &&
	  (strlen(archds_module_error_warning_file_name)> 0))
   {
	   archds_create_Error_Filename();
   }
}

INT archds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
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
    len = strlen(archds_Erro_warning_Filename);
    path_len = strlen(archds_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&archds_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	archds_create_Error_Filename();
    }
	if((fp = fopen(archds_Erro_warning_Filename,"a+b"))==NULL)
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

void archds_create_Error_Filename( )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( archds_Erro_warning_Filename, archds_error_warning_file_path );
    strcat( archds_Erro_warning_Filename, archds_module_error_warning_file_name );
	strcat( archds_Erro_warning_Filename, ".EW." );
    strncat(archds_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(archds_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(archds_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/******************************************************************************
 *
 *  NAME:         archds_log_message
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
void archds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
   INT  Appname_len = 0;
   CHAR  appname[512] = {0};

   GetAppName (appname) ;
   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

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
   sprintf(text_message,"%s ,",appname);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   archds_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
								 	 eventlog_flag, msg_type, func,
									 severity, log_type,
									 NULL, NULL,NULL );
   }

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



#endif



/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/
