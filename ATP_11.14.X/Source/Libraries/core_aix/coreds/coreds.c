/******************************************************************************
  
        Module: coreds.c
  
         Title: System Core Dataserver
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\COREDS\COREDS.C  $
   
      Rev 1.21   Jan 17 2000 17:01:46   MSALEH
   Changed BOOL to BOOLEAN
   
      Rev 1.20   Nov 09 1999 15:09:56   MSALEH
   remove C++ stype comments
   
      Rev 1.19   Oct 15 1999 15:37:04   MSALEH
   change the login to core/pinnacle
   
      Rev 1.18   Oct 15 1999 11:21:04   MSALEH
   Move SSL_SUPPORT to preprocessor definitions
   in the project files
   
      Rev 1.17   Oct 14 1999 17:35:18   MSALEH
   SQL Server fix
   
      Rev 1.13   Jun 16 1999 14:15:52   MSALEH
   disable using logger/reply
   
      Rev 1.12   Apr 27 1999 15:07:48   MSALEH
   added support to have the dm_config table
   either from database or from ini file.
   
      Rev 1.11   Mar 03 1999 10:51:44   MSALEH
   re-enable logger/reply, coreds is tightly
   connected to these apps
   
      Rev 1.10   Feb 01 1999 10:26:12   MSALEH
   Disable using logger/reply because we are not caching
   any tables
   
      Rev 1.9   Dec 04 1998 09:21:20   drodden
   Changed informational printf statements back to the PRINT macro so
   they only get executed in console/debug mode.
   
   
      Rev 1.8   Nov 03 1998 17:26:30   MSALEH
   increased sleep time to 3 seconds when 
   attempting to reconnect to db
   
      Rev 1.7   Nov 03 1998 15:34:10   MSALEH
   Implement reconnect to database
   mechanism
   
      Rev 1.6   Oct 22 1998 15:44:18   drodden
   Changed the subtype1 db-echo-test to just be db-echo.
   
   
      Rev 1.5   Aug 10 1998 13:51:32   drodden
   Changed to pass true in the dstimer_startup call.
   
   
      Rev 1.4   10 Aug 1998 09:41:32   rmalathk
   Modified PinnacleMsgHandler to retrieve
   orig_comms_info field from an incoming
   message and send it back in the response.
   
      Rev 1.3   Jul 16 1998 09:19:22   drodden
   Changed to check the originating system name in the incoming 
   request.  If the request did not originate on this system, no 
   responses are sent or messages forwarded.
   
   
      Rev 1.2   May 08 1998 14:01:46   drodden
   Changed to login to the database as "core/pinnacle".  The core tables
   are expected to be owned by that user.  The CORE_110.SQL script will
   create that user, tables, and copy data from an existing scheme/user.
   Uncommented the timer code.  Core applications may, at some point, 
   want to start timers, etc.
   
   
      Rev 1.1   Apr 29 1998 09:10:52   drodden
   Changed to login as pinnacle/pinnacle rather than unimed/unimed
   so the core dataserver isn't tied to the unimed project.
   
   
      Rev 1.0   Apr 23 1998 13:57:00   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include "basictyp.h"
#include "pte.h"

#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "logger.h"
#include <string.h>

#include "coreinfo.h"
#include "coredb.h"
#include "dbcommon.h"
#include "dstimer.h"
#include "dm_config.h"

/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;


PRIVATE union
{
   ROUTER_TABLE      Router;
   TPDU              Tpdu;
   DM_CONFIG         DmConfig;
#ifdef SSL_SUPPORT   
   CM_CONFIG		 CmConfig;
#endif   
} db_recs;


PRIVATE union
{
   ROUTER_TABLE_LIST    RouterTable_List;
   TPDU_TABLE_LIST      TpduTable_List;
   DM_CONFIG_LIST       DMConfig_List;
#ifdef SSL_SUPPORT  
   CM_CONFIG_LIST		CMConfig_List;
#endif  
} db_lists;


PRIVATE BYTE         result_code;
PRIVATE BOOLEAN      updates_to_db_made = false;
PRIVATE CHAR         ErrorMsg  [100];
PRIVATE CHAR         AppName   [8];
PRIVATE CHAR         last_trid [MAX_TRAN_ID_SIZE];
PRIVATE pCHAR        reply_que;
PRIVATE CHECKPOINT   checkpoint_rec;


BOOLEAN   in_refeed_mode    = false;

CHAR    Version[]              = "ATP_11.1.0";

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

/* #define USING_LOGGER_REPLY
   #define USING_CHECKPOINTS
*/

#ifndef USING_LOGGER_REPLY
   #undef USING_CHECKPOINTS
#endif



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_insert_msg( pPTE_MSG p_msg_in )
{
   BYTE           app_data_type;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   switch( app_data_type )
   {
      case ROUTER_DATA:

         PRINT( "request to insert router_table record\n" );

         /* retrieve the data into a router structure and insert into database */
         memset( &db_recs.Router, 0, sizeof( db_recs.Router ) );
         memcpy( &db_recs.Router, ( pROUTER_TABLE )p_data, sizeof( ROUTER_TABLE ) );
         result_code = db_insert_router_record( &db_recs.Router, ErrorMsg );
         break;

      case TPDU_DATA:

         PRINT( "request to insert tpdu record\n" );

         /* retrieve the data into a tpdu structure and insert into database */
         memset( &db_recs.Tpdu, 0, sizeof( db_recs.Tpdu ) );
         memcpy( &db_recs.Tpdu, ( pTPDU )p_data, sizeof( TPDU ) );
         result_code = db_insert_tpdu_record( &db_recs.Tpdu, ErrorMsg );
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
                                    NULL_PTR,
                                    0,
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
   BYTE           app_data_type;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   switch( app_data_type )
   {
      case DM_CONFIG_DATA:

         PRINT( "request to update dm_config record\n" );

         memset( &db_recs.DmConfig, 0, sizeof( db_recs.DmConfig ) );
         memcpy( &db_recs.DmConfig, ( pDM_CONFIG )p_data, sizeof( DM_CONFIG ) );
         result_code = db_update_dmconfig_record( &db_recs.DmConfig, ErrorMsg );
         break;

      case ROUTER_DATA:

         PRINT( "request to update router_table record\n" );

         /* retrieve the data into a router structure and update database */
         memset( &db_recs.Router, 0, sizeof( db_recs.Router ) );
         memcpy( &db_recs.Router, ( pROUTER_TABLE )p_data, sizeof( ROUTER_TABLE ) );
         result_code = db_update_router_record( &db_recs.Router, ErrorMsg );
         break;

      case TPDU_DATA:

         PRINT( "request to update tpdu record\n" );

         /* retrieve the data into a tpdu structure and update database */
         memset( &db_recs.Tpdu, 0, sizeof( db_recs.Tpdu ) );
         memcpy( &db_recs.Tpdu, ( pTPDU )p_data, sizeof( TPDU ) );
         result_code = db_update_tpdu_record( &db_recs.Tpdu, ErrorMsg );
         break;
         
#ifdef SSL_SUPPORT
      /* added for SSL 06/23/99 - jjj */
	  case CM_CONFIG_DATA:

         PRINT( "request to update cm_config record\n" );

         memset( &db_recs.CmConfig, 0, sizeof( db_recs.CmConfig ) );
         memcpy( &db_recs.CmConfig, ( pCM_CONFIG )p_data, sizeof( CM_CONFIG ) );
         result_code = db_update_cmconfig_record( &db_recs.CmConfig, ErrorMsg );
         break;
#endif

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Update request from %s", reply_que);
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
                                    NULL_PTR,
                                    0,
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
   BYTE           app_data_type;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   /* pGENERIC_KEYS  p_GenericKeys; */


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   switch( app_data_type )
   {
      case ROUTER_DATA:

         PRINT( "request to delete router_table record\n" );

         /* retrieve the router id and delete record from the database */
         result_code = db_delete_router_record( ( pCHAR )p_data, ErrorMsg );
         break;

      case TPDU_DATA:

         PRINT( "request to delete tpdu record\n" );

         /* retrieve the tpdu id and delete record from the database */
         result_code = db_delete_tpdu_record( ( pCHAR )p_data, ErrorMsg );
         break;

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Delete request from %s", reply_que);
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
                                    NULL_PTR,
                                    0,
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
   BYTE           app_data_type;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   /* pGENERIC_KEYS  p_GenericKeys; */


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   switch( app_data_type )
   {
      case DM_CONFIG_DATA:

         PRINT( "request to retrieve dm_config record\n" );

         if( ( result_code = db_select_dmconfig_record( ( pCHAR )p_data, &db_recs.DmConfig, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_recs.DmConfig,
                                          sizeof( db_recs.DmConfig ),
                                          DM_CONFIG_DATA );
         }
         break;

      case ROUTER_DATA:

         PRINT( "request to retrieve router record\n" );

         /* copy the router identifier into the structure and try to retrieve the record */
         if( ( result_code = db_select_router_record( ( pCHAR )p_data, &db_recs.Router, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_recs.Router,
                                          sizeof( db_recs.Router ),
                                          ROUTER_DATA );
         }
         break;

      case TPDU_DATA:

         PRINT( "request to retrieve tpdu record\n" );

         /* copy the tpdu id into the structure and try to retrieve the record */
         if( ( result_code = db_select_tpdu_record( ( pCHAR )p_data, &db_recs.Tpdu, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_recs.Tpdu,
                                          sizeof( db_recs.Tpdu ),
                                          TPDU_DATA );
         }
         break;

#ifdef SSL_SUPPORT
      /* added for SSL 06/23/99 - jjj */
      case CM_CONFIG_DATA:

         PRINT ("request to retrieve cm_config record\n");

         if ((result_code = db_select_cmconfig_record ((pCHAR)p_data,&db_recs.CmConfig,ErrorMsg)) == PTEMSG_OK)
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_recs.CmConfig,
                                          sizeof( db_recs.CmConfig ),
                                          CM_CONFIG_DATA );
         }
         break;
#endif

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
pPTE_MSG process_getall_msg( pPTE_MSG p_msg_in )
{
   BYTE           app_data_type;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   /* pGENERIC_KEYS  p_GenericKeys; */

   char strTemp[256]; memset (strTemp, 0, sizeof(strTemp)) ;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   

   switch( app_data_type )
   {
      case ROUTER_DATA:

         PRINT( "request to retrieve router table\n" );

         /* get the complete router table */
         if( ( result_code = db_get_router_table( &db_lists.RouterTable_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_lists.RouterTable_List,
                                          sizeof( db_lists.RouterTable_List ),
                                          ROUTER_DATA );
         }
         break;

      case TPDU_DATA:

         PRINT( "request to retrieve tpdu table\n" );

         /* get the complete router table */
         if( ( result_code = db_get_tpdu_table( &db_lists.TpduTable_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_lists.TpduTable_List,
                                          sizeof( db_lists.TpduTable_List ),
                                          TPDU_DATA );
         }
         break;
      case DM_CONFIG_DATA:

         PRINT( "request to retrieve dm_config table\n" );

         /* get the complete dm_config table */
         if( ( result_code = db_get_dm_config_table( &db_lists.DMConfig_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_lists.DMConfig_List,
                                          sizeof( db_lists.DMConfig_List ),
                                          DM_CONFIG_DATA );
         }
         break;

#ifdef SSL_SUPPORT
	  /* added for SSL - 06/23/99 jjj */
	  case CM_CONFIG_DATA:
		  
		  PRINT ("request to retrieve cm_config table\n");
		 
		  /* get the complete cm_config table */
         if( ( result_code = db_get_cm_config_table( &db_lists.CMConfig_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (BYTE *)&db_lists.CMConfig_List,
                                          sizeof( db_lists.CMConfig_List ),
                                          CM_CONFIG_DATA );
         }
         break;
         
#endif

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for GetAll request from %s", reply_que);
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
   pPTE_MSG       p_msg_out;


   PRINT ("processing Echo request\n");

   p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                 ptemsg_get_msg_subtype1 (p_msg_in),
                                 ptemsg_get_msg_subtype2 (p_msg_in),
                                 reply_que,
                                 application_que_name,
                                 NULL_PTR,
                                 0,
                                 0);

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


   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_DB_SELECT     : p_msg_out = process_select_msg     ( p_msg_in );
                               break;
      case ST1_DB_UPDATE     : p_msg_out = process_update_msg     ( p_msg_in );
                               break;
      case ST1_DB_INSERT     : p_msg_out = process_insert_msg     ( p_msg_in );
                               break;
      case ST1_DB_DELETE     : p_msg_out = process_delete_msg     ( p_msg_in );
                               break;
      case ST1_DB_GET_ALL    : p_msg_out = process_getall_msg     ( p_msg_in );
                               break;
      case ST1_DB_ECHO       : p_msg_out = process_echo_msg       ( p_msg_in );
                               break;
      default:
         p_msg_out   = NULL_PTR;
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
         break;
   }

   return (p_msg_out);
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

               printf   ("%s\n",   ErrorMsg);
               LogEvent (ErrorMsg, ERROR_MSG);

               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
                  printf   ("%s\n",   ErrorMsg);
                  LogEvent (ErrorMsg, ERROR_MSG);
               }
            }
         }
                                     
         break;
      
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for logger reply from %s", reply_que);

         printf   ("%s\n", ErrorMsg);
         LogEvent (ErrorMsg, WARN_MSG);
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
         printf   ("%s\n", Buffer);
         LogEvent (Buffer, ERROR_MSG);
         return (false);
      }

      in_refeed_mode = true;

      printf ("refeed command sent to logger\n");
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
      printf   ("%s\n", ErrorMsg);
      LogEvent (ErrorMsg, WARN_MSG);
   
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
         printf   ("%s\n", ErrorMsg);
         LogEvent (ErrorMsg, WARN_MSG);
         return (result_code);
      }

      ptemsg_set_result_code (p_msg_out, result_code);

      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_msg_out, reply_que);

      free (p_msg_out);

      if (ipc_rcode < 0)
      {
         printf   ("%s\n", ErrorMsg);
         LogEvent (ErrorMsg, WARN_MSG );

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
   LONG        i, j;
   LONG        num_messages;
   LONG        ipc_rcode;
#ifdef WIN32
   BOOLEAN     reconnected = false;
#else
   BOOLEAN     reconnected = FALSE;
#endif
   CHAR        temp_str [80];
   pPTE_MSG    p_cur_msg;
   pPTE_MSG    p_tmp_msg;
   pPTE_MSG    p_chn_out;
   pPTE_MSG    p_new_chn;
   pCHAR       orig_info;
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

   while ( (i <= num_messages) && (p_cur_msg != NULL_PTR) )
   {
      reply_que = ptemsg_get_msg_orig_queue (p_cur_msg);

      switch( ptemsg_get_msg_type( p_cur_msg ) )
      {
         case MT_DB_REQUEST     : p_tmp_msg = process_db_request (p_cur_msg);
            break ;
          
         case MT_TIMER_REQUEST  : p_tmp_msg = dstimer_process_request (p_cur_msg,
                                                                       &updates_to_db_made);
                                  break;            
         case MT_SYSTEM_REQUEST : ptesystm_system_msg_handler (p_cur_msg);
                                  break;
         case MT_LOGGER_REPLY   : process_logger_reply (p_cur_msg);
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
               if ( (in_refeed_mode == false) &&
                    (strcmp (ptemsg_get_msg_orig_system (p_msg_in), 
                             GetThisSystemsName()) == 0)             )
               {
                  sprintf (temp_str, "forwarding message to queue %s\n", 
                           ptemsg_get_msg_dest_queue (p_cur_msg));
                  printf (temp_str);

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
         if (result_code == PTEMSG_NOT_CONNECTED_TO_DB)
         {
            /* Try up to three times to reconnect, if it fails, get out */
            for (j = 1; j <= 3; j++)
            {
               printf   ( "Reconnect to Database, Attempt #%d\n", j );
               LogEvent ( "Attempting to Reconnect to Database", WARN_MSG );
               if (dbcommon_connect ("core", "pinnacle", CNX_ALIAS, DB_NAME, ErrorMsg) == PTEMSG_OK)
               {
#ifdef WIN32
                  reconnected = true;
#else
				  reconnected = TRUE;
#endif
                  break;
               }

               /* Wait some time for the database to complete startup  */
               pteipc_sleep (3000);
            }

            if (reconnected)
            {
               strcpy(ErrorMsg, "Reconnected to Database");
               printf   (ErrorMsg);
               LogEvent (ErrorMsg, INFO_MSG );
               continue;
            }
            else
               break;
         }
         else
            break;
      }

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

            result_code = db_update_checkpoint (&checkpoint_rec, ErrorMsg);
         }
      }
   #endif

   /*
    * If there was an error, back out the changes.
    * Otherwise commit the changes.
    */
   reply_que = ptemsg_get_msg_orig_queue (p_msg_in);
   orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );

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
      p_chn_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_cur_msg),
                                    ptemsg_get_msg_subtype2 (p_cur_msg),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) ErrorMsg,
                                    strlen(ErrorMsg) + 1,
                                    0);

      if (p_chn_out == NULL_PTR)
      {
         printf   ("%s\n", ErrorMsg);
         LogEvent (ErrorMsg, WARN_MSG );

         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build error response to %s", reply_que);
      }
   }
   else
      commit_the_update ();


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

      /*
       *  Only send a response message if it originated on this system.
       */
      if (strcmp (ptemsg_get_msg_orig_system (p_msg_in), GetThisSystemsName()) != 0)
      {
         free (p_chn_out);
         return (PTEMSG_OK);
      }

      ptemsg_set_result_code (p_chn_out, result_code);
      ptemsg_set_orig_comms_info( p_chn_out, orig_info );

      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_chn_out, reply_que);

      free (p_chn_out);

      if (ipc_rcode < 0)
      {
         printf   ("%s\n", ErrorMsg);
         LogEvent (ErrorMsg, WARN_MSG );

         result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }

   if (result_code != PTEMSG_OK)
   {
      printf   ("%s\n", ErrorMsg);
      LogEvent (ErrorMsg, WARN_MSG );
   }

   return (result_code);
}



/*************************************************************************************/
/*************************************************************************************/
BOOLEAN perform_startup ()
{
   CHAR     XipcInstanceName[12];
         

   memset (last_trid, 0, sizeof(last_trid));

   /* Try to connect to the database */
   if (dbcommon_connect ("core", "pinnacle", CNX_ALIAS, DB_NAME, ErrorMsg) != PTEMSG_OK)
   {
      printf   ( "%s\n", ErrorMsg );
      LogEvent ( ErrorMsg, ERROR_MSG );
      return (false);
   }

   printf ("Connected to Database: %s \n", DB_NAME);


   /* Try to initialise the xipc stuff */
   GetAppName          (AppName);
   GetXipcInstanceName (XipcInstanceName);

   if (pteipc_init_single_instance_app (ServiceName, XipcInstanceName) == false)
   {     
      printf   ("Failed to Create XIPC queues");
      LogEvent ("Failed to create XIPC queues", ERROR_MSG);

      pteipc_shutdown_single_instance_app();
#ifdef SQLSERVER
      dbcommon_disconnect (CNX_ALIAS, ErrorMsg);
#else
      dbcommon_disconnect (ErrorMsg);
#endif
      return (false);
   }
   
   printf ("pteipc init successful\n");


   #ifdef USING_LOGGER_REPLY
      /* send restart command to TFLogger */
      if (SendRestartCmd (ErrorMsg) == false)
      {
         printf   ("Error sending restart command to TFLogger: %s\n", ErrorMsg);
         LogEvent ("Error sending restart command to TFLogger", ERROR_MSG);

         pteipc_shutdown_single_instance_app();
#ifdef SQLSERVER
         dbcommon_disconnect (CNX_ALIAS, ErrorMsg);
#else
         dbcommon_disconnect (ErrorMsg);
#endif
         return (false);
      }

      printf ("restart command sent to logger\n");

      #ifdef USING_CHECKPOINTS
         /* get the checkpoint record from the database */
         if (db_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
         {
            printf   ("%s\n",   ErrorMsg);
            LogEvent (ErrorMsg, ERROR_MSG);

            pteipc_shutdown_single_instance_app();
#ifdef SQLSERVER
            dbcommon_disconnect (CNX_ALIAS, ErrorMsg);
#else
            dbcommon_disconnect (ErrorMsg);
#endif
            return (false);
         }

         /* 
          * refeed from my last known trid.
          */
         strncpy (last_trid, checkpoint_rec.checkpoint_trid, sizeof(last_trid));

         if (perform_refeed (checkpoint_rec.checkpoint_trid) == false)
         {
            pteipc_shutdown_single_instance_app();
#ifdef SQLSERVER
            dbcommon_disconnect (CNX_ALIAS, ErrorMsg);
#else
            dbcommon_disconnect (ErrorMsg);
#endif
            return (false);
         }
      #endif
   #endif


   /* Try to initialise the timer stuff */
   dstimer_startup (true);


   return (true);
}



/*************************************************************************************/
/*************************************************************************************/
void MainProcessor()
{
   LONG     ipc_rcode;
   pPTE_MSG p_msg;
   CHAR     Buffer[256];

   printf   ( "Starting the Core DataServer Service\n" );
   LogEvent ( "Starting the Core DataServer Service", INFO_MSG );


   #ifdef _DEBUG
   pteipc_trace_on();
   #endif

   /* For dm_config, we may get the data from the db ot dm_config.ini */
   init_data_source();

   if (perform_startup () == false)
   {
      MainProcessDone = 1;
      return;
   }

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
      else 
         if( ipc_rcode != QUE_ER_TIMEOUT ) 
         {
            pteipc_get_errormsg ( ipc_rcode, ErrorMsg );
            sprintf ( Buffer, "Error on pteipc_receive %s", ErrorMsg );
            printf  ( "%s\n", Buffer );
            LogEvent( Buffer, WARN_MSG );
         }
         else
         {
            in_refeed_mode = false;
         }

      dstimer_check_for_timeouts ();
   }

   dstimer_shutdown ();

   /* shut down ipc */
   pteipc_shutdown_single_instance_app(); 

   printf ("pteipc shutdown successful\n");

   /* disconnect from database */
#ifdef SQLSERVER
   if (dbcommon_disconnect (CNX_ALIAS, ErrorMsg) != PTEMSG_OK)
      printf ( "%s\n", ErrorMsg );
   else
      printf ( "Disconnected from Database: %s\n", DB_NAME );
#else
   if (dbcommon_disconnect (ErrorMsg) != PTEMSG_OK)
      printf ( "%s\n", ErrorMsg );
   else
      printf ( "Disconnected from ORACLE\n" );
#endif

   /* signal that we are ready for shutdown */
   MainProcessDone = 1;
}



/*************************************************************************************/
/*************************************************************************************/
void EndProcess()
{
   printf   ( "Shutting down the Core DataServer Service\n" );
   LogEvent ( "Shutting down the Core DataServer Service", INFO_MSG );

   pteipc_sleep (3000);
}

