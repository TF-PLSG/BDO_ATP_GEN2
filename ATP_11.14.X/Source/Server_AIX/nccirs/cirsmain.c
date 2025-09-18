/******************************************************************************
 
 	CIRSMAIN.C
 
    Copyright (c) 2003, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Cirrus Host Handler Driver
 
 	This module is the host handler for the Cirrus host.  It
    will process all messages destined to, or originated from,
    the Cirrus host, also known as the MasterCard Debit Switch.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nccirrus\Cirsmain.c  $
   
      Rev 1.45   Jun 17 2005 11:41:54   chuang
   Version 4.1.1.4. Fix scr 15143, log reject reason code and 
   detail code into TLF01.vehicle_number. log issuer_id. Fix 
   parsing error of field 62.
   
      Rev 1.44   May 17 2005 18:01:02   dirby
   Updated version to 4.4.1.2
   SCR 15420
   
      Rev 1.43   Apr 05 2005 15:01:44   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.42   Oct 22 2004 12:06:02   dirby
   Updated version to 4.4.0.2
   SCR 12897
   
      Rev 1.41   Jul 08 2004 17:35:50   dirby
   Updated to 4.4.0.1
   SCRs 1287 & 1388
   
      Rev 1.40   May 27 2004 17:48:22   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.39   Feb 09 2004 14:17:50   dirby
   Updated version to 4.2.0.9
   SCR 1331
   
      Rev 1.38   Jul 23 2003 16:06:54   dirby
   Updated version to 4.2.0.8
   SCR 1111
   
      Rev 1.37   Apr 01 2003 09:06:36   dirby
   Updated version to 4.2.0.7
   SCR 1073
   
      Rev 1.36   Mar 24 2003 14:14:08   dirby
   Updated version to 4.2.0.6
   SCR 1064
   
      Rev 1.35   Mar 19 2003 20:30:54   dirby
   Modified to store settlement date in BCH10 (in the def_gross_amt
   field) to be used in reversals.
   SCR 1063
   
      Rev 1.34   Mar 18 2003 16:48:38   dirby
   Updated version to 4.2.0.4
   SCR 1057
   
      Rev 1.33   Mar 17 2003 16:43:14   dirby
   Updated version to 4.2.0.3
   SCR 1056
   
      Rev 1.32   Mar 14 2003 15:34:24   dirby
   Updated version to 4.2.0.2
   SCR 1055
   
      Rev 1.31   Feb 25 2003 10:07:52   dirby
   Removed a redundant error message for key change failures.
   SCR 955
   
      Rev 1.30   Feb 17 2003 13:58:18   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.29   Nov 12 2002 13:22:30   dirby
   Updated version to 4.1.1.2
   SCR 845
   
      Rev 1.28   Aug 22 2002 12:37:30   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.27   Aug 21 2002 10:52:32   dirby
   Updated version to 4.0.3.4
   SCR 881
   
      Rev 1.26   Aug 07 2002 10:20:08   dirby
   Updated version to 4.0.3.3
   SCRs 863/864
   
      Rev 1.25   Jul 29 2002 10:59:24   dirby
   Updated version to 4.0.3.2
   SCR 594
   
      Rev 1.24   Jul 03 2002 17:29:32   dirby
   1.  Modified to use pre-calculated sizes instead of always recalculating a structure size.
   2.  Remove use of read_timer.
   3.  Provide details of errors from timerds.
   4.  Added code to display time outs and reversals on Monitor.
   5.  Version 4.0.3.1
   SCR 621
   
      Rev 1.23   Jan 16 2002 16:54:12   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.22   Jan 14 2002 18:10:58   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.21   Dec 11 2001 14:53:20   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.20   Nov 03 2001 14:02:30   dirby
   Updated version to 4.0.0.1
   
      Rev 1.19   Jan 09 2001 13:45:50   dirby
    
   
      Rev 1.18   Oct 18 2000 14:35:20   dirby
   Updated version number to 3.2.2.5
   
      Rev 1.17   Sep 18 2000 16:43:52   dirby
   Added some ShutDown code to clear the SAF and Echo timers at
   service shut down.  Also remove code that sent status updates to
   Monitor after log on/off messages; this code was redundant; that
   functionality is performed when the logon response is handled.
   
      Rev 1.16   Aug 23 2000 13:37:44   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.15   Aug 17 2000 15:10:24   dirby
   Updated version number to 3.2.2.2
   
      Rev 1.14   Aug 08 2000 12:57:00   dirby
   Removed a global variable definition that is no longer used.
   Changed some code to call 'set_host_state'; was doing it manually.
   
      Rev 1.13   Aug 07 2000 14:51:58   dirby
   Added a global variable to indicate the state of field 96,
   should it go with the message to host or not.
   Also, corrected some indentation problems.
   
      Rev 1.12   Jul 30 2000 15:37:00   dirby
   Modified to display version number at startup and shutdown.
   
      Rev 1.11   Jan 23 2000 16:33:26   dirby
   Removed a global variable that is no longer used.
   
      Rev 1.10   Jan 13 2000 11:46:02   ddabberu
   Added Code to do auto_logon on service start_up.
   1. Modifed MainProcessor()
   2. Added hhutil_process_response()
   3. Added hhutil_get_ncf01_cfg()
   
      Rev 1.9   Nov 12 1999 10:38:08   dirby
   Corrected the Update Data Server queue name from 'updatedsA' to 'updatdsA'.
   
      Rev 1.8   Nov 10 1999 15:16:44   dirby
   Added queue name for UPDATEDS.
   
      Rev 1.7   23 Aug 1999 17:08:34   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.6   21 Jun 1999 16:36:32   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.5   08 Jun 1999 10:45:26   dirby
   Added flag 'invalid_msg_format' to indicate if the given length of a message from the host
   is not the actual length of the message.
   
      Rev 1.4   Mar 31 1999 15:29:54   egable
   Update the monitor to show offline when shut down.
   
      Rev 1.3   19 Mar 1999 17:12:28   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.2   12 Mar 1999 16:42:54   dirby
   Added functions to perform pin key exchange.
   Only the code to handle the initial request from the
   host is in place.  Still need to code the response.
   
      Rev 1.1   04 Mar 1999 16:47:04   dirby
   Updated for AIX port.
   
      Rev 1.0   19 Feb 1999 16:19:46   dirby
   Initial version
   
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#endif
#include <string.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "memmnger.h"
#include "cirsmain.h"
#include "echo_timer.h"
#include "txutils.h"
#include "diskio.h"
#include "txtrace.h"


/* Global Variables */
CHAR  Settlement_ID[12];
CHAR  Stan[7] = "";
CHAR  Logon_NMI_Code[4] = "";
INT   DE96_State = NO;
CHAR  AppName[8] = "";
CHAR  Version[] = "ATP_11.1.0";

/* Database structures */
NCF01             ncf01;
NCF21             ncf21;
NCF30             ncf30;
SAF01             saf01;
TLF01             tlf01;

/* Database structure sizes */
INT    Tlf01_Size;
INT    Ncf01_Size;
INT    Ncf21_Size;
INT    Ncf30_Size;
INT    Saf01_Size;
INT    Auth_Tx_Size;

/* Shared memory variables */
BYTE   net_consec_tmouts_ctr[4];
BYTE   active_txns_ctr[4];
BYTE   current_state[2];

/* Shared Memory Table Name */
CHAR  SHARED_MEMORY_TBL[15];

/* These are used for reject 0620/0630 messages. */
CHAR   addl_data_len[4];
CHAR   addl_data[101];
CHAR   advice_len[4];
CHAR   advice[61];
CHAR   recv_inst_len[3];
CHAR   recv_inst[12];
CHAR   F60_len[4];
CHAR   reject_advice_code[4];
CHAR   reject_detail_code[5];
CHAR   reject_text[54];

FILE     *Totals_fp   =  NULL;
FILE     **pTotals_fp =  &(Totals_fp);
FILE     *Settle_fp   =  NULL;
FILE     **pSettle_fp =  &(Settle_fp);

/* Queue definitions */
CHAR  cur_app_name []              = "nccirs";
CHAR  authorizer_que_name[]        = "txcntlA";
CHAR  dialog_que_name []           = "dialogA";
CHAR  applnk_que_name []           = "applnkA";
CHAR  oprmon_que_name[]            = "oprmonI" ;
CHAR  trands_que_name[]            = "trandsA";
CHAR  devds_que_name[]             = "devdsA";
CHAR  netds_que_name[]             = "netdsA";
CHAR  security_que_name[]          = "racalA";
CHAR  timerds_que_name[]           = "timerdsA";
CHAR  updateds_que_name[]          = "updatdsA";

/* Statistics for Monitor */
TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
CHAR   DirPath[MAX_APP_NAME_SIZE];
CHAR   TraceFile[MAX_APP_NAME_SIZE];
INT    Tracing;
FILE   TraceFile_fp;
FILE   *pTraceFile_fp;
FILE   **ppTraceFile_fp;

/* Counters */
INT    MaxActiveTxns;
INT    MaxConsecTimeouts;

INT   reqidx;                 /* next open position in request buffer */
INT   buflen;                 /* total size of buffer */
INT   bitmapidx;              /* offset of bitmap... */
INT   invalid_msg_format;     /* flag used in parsing host message */

BYTE ReqRsp_Buffer [COMM_BUF_SZE];
int  BitFieldSize = NORMAL_BITFIELD;

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */


/* System variables */
extern INT  EndProcessSignalled;
extern INT  MainProcessDone;
extern CHAR ServiceName[12];


/******************************************************************************
 *
 *  NAME:         PinnacleMsgHandler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                host handler.  It will transfer control to the appropriate
 *                module, based on message type.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pPTE_MSG          p_cur_msg;
   pPTE_MSG          p_msg_out;
   pBYTE             p_data_data = NULL_PTR;
   BYTE              subtype1;
   BYTE              timer_result;
   AUTH_TX           auth_tx;
   CHAR              buffer[100];

   p_cur_msg = p_msg_in;
   p_msg_out = NULL_PTR;

   ptestats_accumulate_msg_stats(p_msg_in);

   /* Determine message type so we know which module to send it to. */
   switch( ptemsg_get_msg_type(p_cur_msg) )
   {
      case MT_NSP_KEYCHANGE_RESPONSE:
         (void)security_reply( p_msg_in, MT_NSP_KEYCHANGE_RESPONSE );
      break;

      case MT_AUTH_REQUEST:

         /* Request coming in from the terminal handler */
         if ( false == process_incoming_message(p_msg_in) )
            cirsutil_error_handler(1,
                                  "Error processing incoming terminal request",
                                   0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                   "468");
      break;

      case MT_AUTH_RESPONSE: 

         /* Request coming in from the terminal handler */
         if ( false == process_incoming_message(p_msg_in) )
            cirsutil_error_handler(1,
                                  "Error processing incoming terminal request",
                                   0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                   "469");
      break;

      case MT_INCOMING: 

         /* Reply from the external host */
         if ( false == incoming_host_message(p_msg_in) )
            cirsutil_error_handler(1,
                                  "Error processing incoming host response",
                                   0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                   "471");
      break;

      case MT_DB_REPLY:
         /* Message coming from the data server */
         if ( PTEMSG_OK == ptemsg_get_result_code(p_msg_in) )
            cirsutil_parse_db_ok(p_msg_in);
         else
            cirsutil_parse_db_error(p_msg_in);
      break;

      case MT_TIMER_TIMEOUT: 
         /* Reply about timeout of ptemsg */
         if ( false == process_timeout_msg(p_msg_in) )
            cirsutil_error_handler(1,
                                  "Error processing timeout message",
                                   0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                   "472");
      break;

      case MT_SYS_MONITOR:

         switch( ptemsg_get_msg_subtype1( p_cur_msg ) )
         {
            case ST1_LOGON:
 
               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( false == perform_host_logon_request( LOGON_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  cirsutil_error_handler(1,
                                        "Error executing host LogOn request",
                                         0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                         WARNING_ERROR, NULL_STR, NULL_STR,
                                         NULL_STR, "473");
               }
            break;

            case ST1_LOGOFF:
 
               /* Request from Opermon to send LogOff */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( false == perform_host_logon_request( LOGOFF_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  cirsutil_error_handler(1,
                                        "Error executing host LogOff request",
                                         0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                         WARNING_ERROR, NULL_STR, NULL_STR,
                                         NULL_STR, "474");
               }
            break;

            case ST1_ECHOTEST:

               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( false == perform_host_logon_request( ECHO_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  cirsutil_error_handler(1,
                                        "Error executing host Echo Test",
                                         0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                         WARNING_ERROR, NULL_STR, NULL_STR,
                                         NULL_STR, "475");
               }
            break;

            case ST1_NETWORK_QUERY:
                /* Request from Opermon to send status information */
                if (send_network_status_to_monitor() == false)
                {
                  cirsutil_error_handler(1,
                                        "Error sending network status to monitor",
                                         0, INFO_MSG, "PinnacleMsgHandler", 3,
                                         INFO_ERROR, NULL_STR, NULL_STR,
                                         NULL_STR, "458");
                }
                break;

            default:
               break;
         }  /* End switch on subtype */
         break;  /* End MT_SYS_MONITOR    */

      case MT_SYSTEM_REQUEST:
         ptesystm_system_msg_handler( p_msg_in );
		 process_encryption_flag( p_msg_in );
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            if ( Tracing == TRACE_ON )
            {
               /* Flush the trace file buffer when service is pinged. */
               if ( false == flush_file(ppTraceFile_fp) )
               {
                  Tracing = TRACE_OFF;
                  sprintf( buffer,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     ServiceName );
                  TxUtils_Send_Msg_To_Operator(1, buffer,
                                 1,INFO_MSG,"PinnacleMsgHandler",3,INFO_ERROR,
                                 0,0,0);
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Network Controller Service: %s, version %s",
                     ServiceName, Version );
            TxUtils_Send_Msg_To_Operator( 1, buffer, 1, INFO_MSG,
                                         "PinnacleMsgHandler",
                                          0, INFO_ERROR, 0, 0, 0 );
         }
      break;

      case MT_SYSTEM_REPLY: 
      break;

      case MT_TIMER_REPLY:
         if ( PTEMSG_OK == ptemsg_get_result_code(p_msg_in) )
         {
            subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
            if ( ST1_TIMER_CLEAR == subtype1 )
            {
                incoming_host_message_timer_reply( p_msg_in );
            }
         }
         else
         {
            p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
            p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
            memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
            timer_result = ptemsg_get_result_code(p_msg_in);

            memset( buffer, 0x00, sizeof(buffer) );
            sprintf( buffer,
                    "Error from Timerds, result code = %c, stan = %s",
                     timer_result, auth_tx.TLF01_details.sys_trace_audit_num );
            TxUtils_Send_Msg_To_Operator( 1, buffer, 1, WARN_MSG,
                                         "PinnacleMsgHandler",3,WARNING_ERROR,
                                          auth_tx.TLF01_details.terminal_id,
                                          auth_tx.TLF01_details.card_num,
                                          auth_tx.TLF01_details.merchant_id );
         }
      break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE:
         if ( false == security_reply(p_msg_in, MT_NSP_TRANSLATE_PIN_RESPONSE) )
         {
            cirsutil_error_handler(1,
                          "Error processing pin translation reply",
                           0, ERROR_MSG, "PinnacleMsgHandler",
                           3, WARNING_ERROR,
                           NULL_STR, NULL_STR, NULL_STR, "508" );
         }
         break;

      case MT_NSP_XLATETOANSI_RESPONSE:

         /* Request coming in from the cryptographer */
         if ( false == process_incoming_message(p_msg_in) )
            cirsutil_error_handler(1,
                                  "Error processing incoming crypto request",
                                   0, ERROR_MSG, "PinnacleMsgHandler", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                   "470");
      break;

      case MT_GET_STATS:
         (void)send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:
         (void)reset_transaction_statistics( p_msg_in );
         break;

      default :
         cirsutil_error_handler(1,
                               "Unknown message type received",
                                1, ERROR_MSG, "PinnacleMsgHandler", 3,
                                WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                "476");
      break;
   }  /* switch msg_type */

} /* PinnacleMsgHandler */


/******************************************************************************
 *
 *  NAME:         MainProcessor
 *
 *  DESCRIPTION:  This is the driver function for the Cirrus host handler.
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
   pPTE_MSG  p_msg;
   LONG      ret_code;
   INT       rc;
   CHAR      ErrorMsg[100] = "";
   CHAR      Buffer[256]   = "";
   CHAR     err_msg[200];
   AUTH_TX  auth_tx;

   GetAppName( AppName );
   sprintf( Buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   LogEvent( Buffer, INFO_MSG );
   printf( "%s\n", Buffer );


   /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***************/
   /* if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) ) */
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName,"pte_ipc"))
   {
      cirsutil_error_handler(1,
                            "Failed to create XIPC queues",
                             1, ALERT_MSG, "MainProcessor", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "477");

      /* Signal that we are ready to shut down and quit */
      /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***********/
      /* pteipc_shutdown_single_instance_app(); */
      pteipc_shutdown_multiple_instance_app();
      MainProcessDone = 1;
   }
   else
   {
      /* Calculate Data Structure sizes to save time during processing. */
      Tlf01_Size        = sizeof(TLF01);
      Ncf01_Size        = sizeof(NCF01);
      Ncf21_Size        = sizeof(NCF21);
      Ncf30_Size        = sizeof(NCF30);
      Saf01_Size        = sizeof(SAF01);
      Auth_Tx_Size      = sizeof(AUTH_TX);


      /* Create a table to be used for shared memory between multiple
       * instances of this Network Control Module.  It contains three
       * fields: # of consecutive timeouts, # of active transactions,
       * and the current state of the host.
       */
      (void)create_shared_mem_table( SHARED_MEMORY_TBL );

      /* Initialize transaction statistics */
      (void)reset_stats( &Txn_Counts );

      /* Determine if transaction tracing should be turned on. */
      rc = get_trace_ini_info( AppName, DirPath, ErrorMsg );
      if ( rc == 0 )
      {
         Tracing = TRACE_ON;

         /* Open the transaction trace file. */
         memset( TraceFile, 0x00, sizeof(TraceFile) );

         pTraceFile_fp  = &TraceFile_fp;
         ppTraceFile_fp = &pTraceFile_fp;

         if (false == open_trace_file(AppName,DirPath,ppTraceFile_fp,TraceFile))
         {
            Tracing = TRACE_OFF;
            sprintf( ErrorMsg,
                    "Unable to open trace file %s. Tracing is off",
                     TraceFile );
            TxUtils_Send_Msg_To_Operator(1,ErrorMsg,
                                      1,INFO_MSG,"MainProcessor",2,INFO_ERROR,
                                      0,0,0);
         }
         else
            TxUtils_Send_Msg_To_Operator(1,"Tracing is turned ON",
                                      1,INFO_MSG,"MainProcessor",2,INFO_ERROR,
                                      0,0,0);
      }
      else
      {
         Tracing = TRACE_OFF;
         if ( rc == -1 )
         {
            TxUtils_Send_Msg_To_Operator(1,ErrorMsg,
                                      1,INFO_MSG,"MainProcessor",2,INFO_ERROR,
                                      0,0,0);
         }
         TxUtils_Send_Msg_To_Operator(1,"Tracing is turned off",
                                   1,INFO_MSG,"MainProcessor",0,INFO_ERROR,
                                   0,0,0);
      }

      memset(&ncf01, 0, Ncf01_Size);
      strcpy(ncf01.primary_key.network_id, AppName);
      ncf01.primary_key.network_type[0] = 'I';
      if (hhutil_get_ncf01_cfg( &ncf01, err_msg) != PTEMSG_OK)
      {
         sprintf(Buffer, "Error on select of NCF01: %s", err_msg);
         TxUtils_Send_Msg_To_Operator (1, Buffer, 1, WARN_MSG, 
                                      "MainProcessor", 3, WARNING_ERROR,
                                       NULL_PTR, NULL_PTR, NULL_PTR);
      }
      else
      {
         /* Initialize configurable counters. */
         MaxActiveTxns     = atoi(ncf01.max_active_txns);
         MaxConsecTimeouts = atoi(ncf01.max_consecutive_timeouts);

         if (0 == strcmp(ncf01.auto_logon,"1"))
         {
            /* set up a logon request to the host in auth_tx  */
            memset(&auth_tx, 0, Auth_Tx_Size);
            perform_host_logon_request(LOGON_REQUEST, SOURCE_IS_OPERATOR,
                                       &auth_tx);
         }
      }
      (void)send_network_status_to_monitor();
      
      while( !EndProcessSignalled )
      {
         /* You are blocked here waiting for a message on either app queue or
          * control queue. If there is no message on either queue for 5 seconds,
          * the blocking call returns.
          *
          * The following line will be used to get messages from the queue.
          * For now the message is automatically created here, but this will
          * be removed.
          */
         p_msg = pteipc_receive( application_que_name, control_que_name,
                                 5, &ret_code ); 

         if ( NULL_PTR != p_msg )
         {
            PinnacleMsgHandler( p_msg );
            free( p_msg );
         }
         else if ( QUE_ER_TIMEOUT != ret_code ) 
         {
            pteipc_get_errormsg( ret_code, ErrorMsg );
            sprintf( Buffer, "Error on pteipc_receive %s", ErrorMsg );
            cirsutil_error_handler(1,
                                   Buffer,
                                   1, ALERT_MSG, "MainProcessor", 4,
                                   FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                                   "478");
         }
      }

      ShutDown();

      /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***********/
      /* pteipc_shutdown_single_instance_app(); */
      pteipc_shutdown_multiple_instance_app();
      MainProcessDone = 1;
   }
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
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void EndProcess()
{
   CHAR Buffer[100] = "";

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         TxUtils_Send_Msg_To_Operator(1,Buffer,
                                      1,INFO_MSG,"EndProcess",2,INFO_ERROR,
                                      0,0,0);
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   LogEvent( Buffer, INFO_MSG );
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}


/******************************************************************************
 *
 *  NAME:         ShutDown
 *
 *  DESCRIPTION:  This is the function used to clean up when the app is
 *                being exited.  All clean up code goes in here.  The 
 *                between this function and EndProcess is that this function
 *                is called before exiting XIPC.
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
void ShutDown()
{
   /* Clear the SAF timer. */
   (void)clear_echo_timer( SAF_TIMER_KEY, (pPTE_MSG *)NULL_PTR,
                           application_que_name, 1 );

   /* Set Logon Status to DOWN; this also clears echo timer. */
   (void)set_host_state( DOWN );
}

