/******************************************************************************

   MCRDMAIN.C

   Copyright (c) 2007, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  MasterCard Host Handler Driver

   This module is the host handler for the MasterCard host. It
   will process all messages destined to, or originated from,
   the MasterCard host.

   APPLICATION:  Advanced Transaction Processor (ATP)

   ----------------
   REVISION HISTORY
   ----------------
    
	  Rev 1.90   Nov 23 2009   TF-Ajay 
    Updated version to 4.4.1.26
    Changed code to send void sale also as reversal(0400)
	Reversal timestamp should be the same as the original 0100 message.
	Rel 09-76
	
	  Rev 1.89   Nov 10 2009   TF-Ajay 
    Updated version to 4.4.1.25
    Added code to send reversal and void txn to MC host.
	to forward switched_in txn also
	Rel 09-76


	  Rev 1.88   Jul 13 2009   TF-Ajay 
   Updated version to 4.4.1.24
    Added code to copy the STIP txn status from ncf30 


   Rev 1.87   Aug 19 2008  Phani TF
      Updated version to 4.4.1.21
   Porduction issues: 
	1) Field 61.11, 61.6 not passing corectly for EMV and RPS transactions--Fixed
	2) Field 51, EMV data not passed correclty - Fixed
   Rev 1.86   Dec 10 2007  dirby
      Updated version to 4.4.1.18
   SCR 25043

   Rev 1.85   Dec 06 2007  dirby
      Updated version to 4.4.1.17
   SCR 25030

   Rev 1.84   Nov 27 2007  dirby
      Updated version to 4.4.1.16
   SCR 25030

   Rev 1.83   Nov 02 2007  dirby
      Updated version to 4.4.1.15
   SCR 25041

   Rev 1.82   Sep 12 2007  dirby
      Updated version to 4.4.1.14
   SCR 25030

      Rev 1.81   Jul 19 2007  dirby
   Updated version to 4.4.1.13
   SCR 25027

      Rev 1.80   Sep 18 2006 11:54:34   dirby
   Updated version to 4.4.1.12
   SCR 23187
   
      Rev 1.79   May 23 2006 17:00:40   dirby
   Updated version to 4.4.1.11
   SCR 20924
   
      Rev 1.78   May 01 2006 16:57:06   dirby
   1.  Updated version to 4.4.1.10
   2.  Added code to prevent runaway log messages when XIPC problems.
   SCR 21062
   
      Rev 1.77   Feb 17 2006 09:40:08   dirby
   Updated version to 4.4.1.9
   SCR 19844
   
      Rev 1.76   Dec 14 2005 13:37:04   dirby
   Updated version to 4.4.1.8
   SCR 12418
   
      Rev 1.75   Aug 26 2005 15:35:32   dirby
   Updated version to 4.4.1.7
   SCR 16692
   
      Rev 1.74   Jul 28 2005 16:02:40   dirby
   Updated version to 4.4.1.6
   SCR 16652
   
      Rev 1.73   Jul 14 2005 13:32:56   dirby
   Updated version to 4.4.1.5
   SCR 15918
   
      Rev 1.72   Jun 17 2005 11:38:28   chuang
   Version 4.1.1.4, log reject reason code and detail code into TLF01.vehicle_number.
   
      Rev 1.71   Jun 10 2005 16:59:32   dirby
   Updated version to 4.4.1.3
   SCR 115725
   
      Rev 1.70   May 17 2005 17:54:12   dirby
   Updated version to 4.4.1.2
   SCR 15420
   
      Rev 1.69   Apr 05 2005 15:18:52   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.68   Apr 05 2005 11:53:40   dirby
   Updated version to 4.4.0.17
   SCR 15059
   
      Rev 1.67   Mar 30 2005 17:19:34   dirby
   Updated version to 4.4.0.16
   SCR 15006
   
      Rev 1.66   Mar 30 2005 10:15:46   dirby
   Updated version to 4.4.0.15
   SCRs 15006 & 15020
   
      Rev 1.65   Mar 17 2005 15:16:50   dirby
   Updated version to 4.4.0.14
   SCRs 14026 and 14677
   
      Rev 1.64   Mar 14 2005 14:16:34   dirby
   Updated version to 4.4.0.13
   SCR 14026
   
      Rev 1.63   Mar 01 2005 13:21:38   dirby
   Updated version to 4.4.0.12
   SCR 14473
   
      Rev 1.62   Feb 17 2005 14:21:12   mbysani
   scr 13560,  EMV Support
   
      Rev 1.61   Feb 11 2005 17:32:50   dirby
   Modified to copy decline/failure messages into TLF01's response text.
   SCR 12340
   
      Rev 1.60   Feb 04 2005 17:22:50   dirby
   Update to version 4.4.0.10
   
      Rev 1.59   Jan 10 2005 14:57:10   dirby
   Updated version to 4.4.0.9
   SCR 12475
   
      Rev 1.58   Dec 10 2004 10:48:16   dirby
   Updated version to 4.4.0.8
   SCR 12475
   
      Rev 1.57   Nov 15 2004 13:12:34   dirby
   Updated version to 4.4.0.7
   SCR 12483
   
      Rev 1.56   Oct 27 2004 17:42:58   dirby
   Updated version to 4.4.0.6
   SCR 12725
   
      Rev 1.55   Oct 20 2004 15:09:38   dirby
   Updated version to 4.4.0.5
   SCR 12481
   
      Rev 1.54   Aug 31 2004 18:01:04   dirby
   Updated version to 4.4.0.4
   SCR 1475
   
      Rev 1.53   Aug 23 2004 12:26:50   dirby
   Updated version to 4.4.0.3
   SCR 1469
   
      Rev 1.52   Aug 17 2004 11:23:34   dirby
   Updated version to 4.4.0.2
   SCR 1443
   
      Rev 1.51   Jul 08 2004 17:42:56   dirby
   Updated to 4.4.0.1
   SCRs 1287 & 1388
   
      Rev 1.50   May 27 2004 17:57:32   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.49   Mar 05 2004 14:19:12   dirby
   Updated version to 4.2.0.14
   SCR 1356
   
      Rev 1.48   Mar 04 2004 17:13:10   dirby
   Updated version to 4.2.0.13
   SCR 1356
   
      Rev 1.47   Mar 03 2004 15:43:58   dirby
   Updated version to 4.2.0.12
   SCR 1356
   
      Rev 1.46   Feb 24 2004 14:27:50   dirby
   Updated version to 4.2.0.11
   SCR 1167
   
      Rev 1.45   Feb 19 2004 17:16:30   dirby
   Updated version to 4.2.0.10
   SCR 1328
   
      Rev 1.44   Jan 21 2004 17:48:40   dirby
   Updated version to 4.2.0.9
   SCR 1117
   
      Rev 1.43   Oct 09 2003 13:51:56   dirby
   Updated version to 4.2.0.8
   SCR 1224
   
      Rev 1.42   Aug 26 2003 14:06:20   mbysani
   added support for VbV
   
      Rev 1.41   Aug 01 2003 11:37:44   dirby
   Updated version to 4.2.0.6
   SCR 1196
   
      Rev 1.40   Jun 09 2003 08:42:00   dirby
   Updated version to 4.2.0.5
   SCR 1153
   
      Rev 1.39   Jun 06 2003 17:05:58   dirby
   Updated version to 4.2.0.4
   SCR 1152
   
      Rev 1.38   Jun 03 2003 11:46:30   dirby
   Updated version to 4.2.0.3
   SCR 1097
   
      Rev 1.37   May 06 2003 16:07:34   dirby
   Updated version to 4.2.0.2
   SCR 1097
   
      Rev 1.36   Mar 03 2003 11:15:28   dirby
   Combined the 2 MCC Gui structures into a single structure.
   SCR 891
   
      Rev 1.35   Feb 19 2003 17:02:10   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.34   Feb 07 2003 14:18:34   dirby
   Updated version to 4.1.1.4.  Removed redundant error messages.
   SCR 1000
   
      Rev 1.33   Nov 08 2002 11:46:30   dirby
   Updated version to 4.1.1.3
   SCR 845
   
      Rev 1.32   Sep 14 2002 16:31:26   dirby
   Updated version to 4.1.1.2
   SCR 967
   
      Rev 1.31   Aug 22 2002 12:50:02   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.30   Jul 29 2002 17:13:10   dirby
   1.  Updated version to 4.0.3.2
   2.  Modified timer-reply to act on Clear and not Read.
   SCR 575
   
      Rev 1.29   Jul 09 2002 15:35:10   dirby
   Added code to display version when pinged.
   SCR 621
   
      Rev 1.28   Jul 07 2002 01:36:22   dirby
   Implemented use of precalculated sizeof values instead of
   recalculating structure sizes over and over and over.
   SCR 594
   
      Rev 1.27   Jul 06 2002 23:33:56   dirby
   Updated version to 4.0.3.1
   SCR 621
   
      Rev 1.26   Mar 05 2002 15:46:54   dirby
   Added startup value to set the MCC validation either on or off.
   Updated version to 4.0.2.2
   
      Rev 1.25   Jan 16 2002 16:05:58   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.24   Jan 14 2002 18:20:40   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.23   Dec 12 2001 08:47:34   dirby
   Set host state to Down prior to exiting Xipc in order to access
   shared memory.     SCR 488
   
      Rev 1.22   Dec 10 2001 16:53:20   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.21   Nov 03 2001 14:15:54   dirby
   Updated version to 4.0.0.1
   
      Rev 1.20   Aug 06 2001 08:59:00   dirby
   Updated version to 3.2.3.1  This is for changes made in
   revision 1.19
   
      Rev 1.19   Jul 13 2001 11:57:52   lmitchel
   Modified mcrdutil.c, build_host_struture(): set field 48_tag87 with tlf01.detail.cvc value for switched in
   
      Rev 1.18   Jan 09 2001 13:45:52   dirby
    
   
      Rev 1.17   Oct 18 2000 14:52:06   dirby
   Updated version number to 3.2.2.4
   
      Rev 1.16   Sep 25 2000 09:50:06   dirby
   Updated the version number to 3.2.2.3
   
      Rev 1.15   Aug 21 2000 15:21:00   dirby
   Added code to support transaction statistics for Monitor.
   
      Rev 1.14   Aug 10 2000 12:19:04   dirby
   Updated version number to 3.2.2.1
   
      Rev 1.13   Jul 30 2000 15:37:02   dirby
   Modified to display version number at startup and shutdown.
   
      Rev 1.12   Jun 21 2000 14:25:46   dirby
   Added a parameter to function 'write_to_shared_mem_table' to indicate
   whether or not to send a status update to Monitor.
   
      Rev 1.11   Nov 18 1999 11:42:08   svelaga
   Modified to accomodate the new reversal logic for
   switched-in transactions.
   
      Rev 1.10   Nov 05 1999 12:50:50   dirby
   Modified to enhance ncmcrd by making it more modular.
   
      Rev 1.9   26 Aug 1999 09:45:20   dirby
   Added definition of timer data server queue name.
   
      Rev 1.8   18 Jun 1999 16:26:22   dirby
   Added code to route debit transactions to Cirrus.
   
      Rev 1.7   08 Jun 1999 10:27:56   dirby
   Added flag 'invalid_msg_format' to indicate if the data length of a message
   from the host does not match the actual message length.
   
      Rev 1.6   04 Jun 1999 16:46:24   dirby
   Added functions to reject unsupported transactions for Equitable.
   
      Rev 1.5   27 May 1999 15:04:28   dirby
   Added case in main 'switch' to process Store and Forward requests
   from the monitor.
   
      Rev 1.4   Mar 31 1999 15:36:28   egable
   Update the monitor to show offline when 
   shut down.  Change the response to monitor 
   for status query.
   
   
      Rev 1.3   19 Mar 1999 17:12:44   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.2   17 Mar 1999 16:27:26   dirby
   Fixed bugs in the 0302/0312 message processing.
   Removed use of global variables orig_queue and originator_info.
   
      Rev 1.1   04 Mar 1999 16:57:48   dirby
   Updated for AIX port.
   
      Rev 1.0   19 Feb 1999 15:10:16   dirby
   Initial version
   
      Rev 1.0   Dec 04 1998 14:50:10   MSALEH
   initial release
 
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
#include "mcrdmain.h"
#include "ncutils.h"
#include "txtrace.h"
#include "diskio.h"
#include "txutils.h"
#include "tx_dbstruct.h"

#define ST1_ACQUIRER_OFF	150
#define ST1_ACQUIRER_ON		151

/* GLOBAL VARIABLES */
CHAR  Stan[7]           = {0};
CHAR  Logon_NMI_Code[4] = {0};
CHAR  ncmcrd_Error_warning_Filename[256]={0};
CHAR  ncmcrd_module_error_warning_file_name[256]={0};
CHAR  ncmcrd_error_warning_file_path[256]={0};
CHAR response_code_for_Incoming_timeout_Transaction[3]={0};
CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/* Queue definitions */
CHAR  cur_app_name [10];
CHAR  updateds_que_name[]          = "updatdsA";
CHAR  authorizer_que_name[]        = "txcntlA";
CHAR  oprmon_que_name[]            = "oprmonI";
CHAR  dialog_que_name []           = "dialogA";
CHAR  applnk_que_name []           = "applnkA";
CHAR  trands_que_name[]            = "trandsA";
CHAR  devds_que_name[]             = "devdsA";
CHAR  netds_que_name[]             = "netdsA";
CHAR  security_que_name[]          = "racalA";
CHAR  cirrus_que_name[]            = "nccirsA";
CHAR  timerds_que_name[]           = "timerdsA";
CHAR  AppName[8] = "";
CHAR Version[] = "ATP_11.13.0";
							

/* Database structures */
NCF01               ncf01_i;
NCF01               ncf01_a;
NCF30               ncf30;
NEG01               neg01;
TLF01               tlf01;
MCC_ONLINE_UPDATES  Mcc_Gui;
BDKKEY01  Bdkkey01;

/* Database structure sizes */
INT    Tlf01_Size;
INT    Ncf01_Size;
INT    Ncf30_Size;
INT    Auth_Tx_Size;
INT    Mcc_Gui_Size;
INT	   Bdkkey01_Size;

/* Shared memory variables */
BYTE   net_consec_tmouts_ctr[4];
BYTE   active_txns_ctr[4];
BYTE   current_state[2];
CHAR   SHARED_MEMORY_TBL[15];

/* Counters */
INT    MaxActiveTxns;
INT    MaxConsecTimeouts;

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

/* These are used for reject 0620/0630 messages. */
CHAR   fwd_inst_id_len[3];
CHAR   fwd_inst_id[12];
CHAR   addl_data_len[4];
CHAR   addl_data[999];
CHAR   advice_len[4];
CHAR   advice[61];
CHAR   recv_inst_len[3];
CHAR   recv_inst[12];
CHAR   F60_len[4];
CHAR   reject_advice_code[4];
CHAR   reject_detail_code[5];
CHAR   reject_text[54];

INT    reqidx;                 /* next open position in request buffer */
INT    buflen;                 /* total size of buffer */
INT    bitmapidx;              /* offset of bitmap... */
INT    invalid_msg_format;     /* flag used in parsing host message */

BYTE   ReqRsp_Buffer [COMM_BUF_SZE];
INT    BitFieldSize = NORMAL_BITFIELD;

CHAR   A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
CHAR   E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */

/* Define direction of transaction flow - switched in or switched out. */
INT    Tran_Flow;

INT    MCC_Validation_Functionality;

/* System variables */
extern INT  EndProcessSignalled;
extern INT  MainProcessDone;
extern CHAR ServiceName[12];
 char mc_scheme_fai_rc [3];
 char mc_acceptor_country_code[4];
CHAR Last_Tran_Logging_Filename[256];

char name_validation_flag;

WORD  echo_timer_length = 0;
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
 *  MODIFICATIONS:
 ******************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pPTE_MSG          p_cur_msg;
   pPTE_MSG          p_msg_out;
   pBYTE             p_data_data = NULL_PTR;
   BYTE              subtype1;
   AUTH_TX           auth_tx;

   pBYTE       pCurrent;
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   BYTE        timer_result;
   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   LONG        ret_code;
   CHAR        err_mesg[512] = {0};
   CHAR        Buff[512] = {0};
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   XINT        ReturnCode = 0 ;
   CHAR        ErrMsg[100] = "";

   p_cur_msg = p_msg_in;
   p_msg_out = NULL_PTR;

   ptestats_accumulate_msg_stats(p_msg_in);

   /* Determine message type so we know which module to send it to. */
   switch( ptemsg_get_msg_type(p_cur_msg) )
   {

      case MT_AUTH_REQUEST:

         /* Request coming in from the terminal handler */
         (void)process_incoming_message( p_msg_in );
      break;

      case MT_AUTH_RESPONSE: 

         /* Request coming in from the authorizer */
         (void)process_incoming_message( p_msg_in );
      break;

      case MT_INCOMING: 

         /* Reply from the external host */
         (void)incoming_host_message( p_msg_in );
      break;

      case MT_DB_REPLY:
         /* Message coming from the data server */
         if ( PTEMSG_OK == ptemsg_get_result_code(p_msg_in) )
            mcrdutil_parse_db_ok(p_msg_in);
         else
            mcrdutil_parse_db_error(p_msg_in);
      break;

      case MT_TIMER_TIMEOUT: 
         /* Reply about timeout of ptemsg */
         if ( OK != process_timeout_msg(p_msg_in) )
         	 ncmcrd_log_message( 1, 3, "Error processing timeout message" , "PinnacleMsgHandler" , 0 ,"");
      break;

      case MT_INCOMING_GUI:
         /* GUI is initiating an 0302 message for MasterCard. */
         if ( OK != process_file_update_txn( p_msg_in ) )
        	 ncmcrd_log_message( 1, 3, "Error processing a file update txn" , "PinnacleMsgHandler" , 0 ,"");
      break;

      case MT_SYS_MONITOR:

         memset( &auth_tx, 0, Auth_Tx_Size);
         switch( ptemsg_get_msg_subtype1( p_cur_msg ) )
         {
			 
            case ST1_LOGON:
 
               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( LOGON_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  ncmcrd_log_message( 1, 3, "Error executing host LogOn request" , "PinnacleMsgHandler" , 0 ,"");
               }
               if (send_network_status_to_monitor() != OK )
               {
                  ncmcrd_log_message( 1, 1, "Error sending network status to monitor", "PinnacleMsgHandler" , 0,"" );
               }
            break;

            case ST1_LOGOFF:
 
               /* Request from Opermon to send LogOff */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( LOGOFF_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  ncmcrd_log_message( 1, 1, "Error executing host LogOff request", "PinnacleMsgHandler" , 0 ,"");
               }
               if (send_network_status_to_monitor() != OK )
               {
                  ncmcrd_log_message( 1, 1, "Error sending network status to monitor", "PinnacleMsgHandler" , 0 ,"");
               }
            break;
			case ST1_ACQUIRER_OFF:
				 /* Request from Opermon to send LogOff */

			printf("ST1_ACQUIRER_OFF\n");
			read_shared_mem_table( AppName, net_consec_tmouts_ctr, 
                                       active_txns_ctr, current_state, err_mesg );
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
				if ( 0 != strcmp( OFFLINE, current_state ) )
                  {
                     set_host_state( AppName, OFFLINE, ErrMsg );
                     ncmcrd_log_message( 1, 1, "Acquirer OFF Successful. Setting Host OFFLINE", "PinnacleMsgHandler" , 0 ,"");
                  }
               if (send_network_status_to_monitor() != OK )
               {
                  ncmcrd_log_message( 1, 1, "Error sending network status to monitor", "PinnacleMsgHandler" , 0 ,"");
               }
				 break;
		case ST1_ACQUIRER_ON:
				 /* Request from Opermon to send LogOff */

			printf("ST1_ACQUIRER_ON\n");
			read_shared_mem_table( AppName, net_consec_tmouts_ctr, 
                                       active_txns_ctr, current_state, err_mesg );
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
				if ( 0 == strcmp( OFFLINE, current_state ) )
                  {
                     set_host_state( AppName, ONLINE, ErrMsg );
                     ncmcrd_log_message( 1, 1, "Acquirer ON Successful. Setting Host ONLINE", "PinnacleMsgHandler" , 0 ,"");
                  }
               if (send_network_status_to_monitor() != OK )
               {
                  ncmcrd_log_message( 1, 1, "Error sending network status to monitor", "PinnacleMsgHandler" , 0 ,"");
               }
				 break;

            case ST1_ECHOTEST:

               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               //HCTS1-D
               ncmcrd_log_message( 1, 1, "Sending ECHO TEST request to host", "PinnacleMsgHandler" , 0 ,"");
               if ( OK != perform_host_logon_request( ECHO_REQUEST,
                                                      SOURCE_IS_OPERATOR,
                                                      &auth_tx ) )
               {
                  ncmcrd_log_message( 1, 3, "Error executing host Echo Test", "PinnacleMsgHandler" , 0 ,"");
               }
            break;

            case ST1_SAF_REQUEST:
 
               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( SAF_REQUEST,
                                                      SOURCE_IS_OPERATOR,
                                                      &auth_tx ) )
               {
                  ncmcrd_log_message( 1, 3, "Error executing host SAF request", "PinnacleMsgHandler" , 0 ,"");
               }
               if ( send_network_status_to_monitor() != OK )
               {
                  ncmcrd_log_message( 2, 1, "Error sending network status to monitor", "PinnacleMsgHandler" , 0 ,"");
               }
            break;

            case ST1_NETWORK_QUERY:
                /* Request from Monitor to send status information */
                read_shared_mem_table( AppName, net_consec_tmouts_ctr, 
                                       active_txns_ctr, current_state, err_mesg );
                pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg_in));
                memset( &ncf01, 0,        Ncf01_Size );
                memcpy( &ncf01, pCurrent, Ncf01_Size );
                strcpy( ncf01.status,     current_state );
 
                p_msg_ins = ptemsg_build_msg( MT_SYS_MONITOR,
                                              ST1_NETWORK_UPDATE, 
                                              ST2_NONE,
                                              applnk_que_name, 
                                              application_que_name,
                                              (pBYTE)&ncf01,
                                              Ncf01_Size, 
                                              0 );
                if ( NULL_PTR == p_msg_ins )
                {
                   ncmcrd_log_message( 3, 2, "Insufficient memory to create msg", "PinnacleMsgHandler" , 0 ,"");
                }

 	             memset (ConnectionId, 0, sizeof (ConnectionId)) ;
 	             ReturnCode = ReadMemMonitorTable (ConnectionId) ;
 	             if (ConnectionId[0] != 0)
                {
                   ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
                   ret_code = pteipc_send( p_msg_ins, applnk_que_name );
                   if ( 0 > ret_code )
                   {
                      pteipc_get_errormsg( ret_code, err_mesg );
                      sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
                      ncmcrd_log_message( 3, 3,Buff, "PinnacleMsgHandler" , 0 ,"");
                    }
                }
                free( p_msg_ins );
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
                  sprintf( Buff,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     ServiceName );
                  ncmcrd_log_message( 3, 1,Buff , "PinnacleMsgHandler" , 0 ,"");
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( Buff,
                    "Pinged -> Network Controller Service: %s, version %s",
                     AppName, Version );
            ncmcrd_log_message( 3, 1,Buff , "PinnacleMsgHandler" , 0 ,"");
         }
         else if(ST1_LOAD_TO_RC_VALUES == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 Read_Incoming_TIMEOUT_Response_Code();
        	 send_trace_response(p_msg_in);
        	 sprintf( err_mesg,
        	          "mc_scheme_timeout_rc: %s",
					  response_code_for_Incoming_timeout_Transaction);
        	 ncmcrd_log_message( 1, 1, err_mesg, "PinnacleMsgHandler" ,0);
         }
         else if(ST1_LOAD_RC_FOR_FAI == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 ncmcrd_read_fai_rc_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( err_mesg,
        	          "mc_scheme_fai_rc: %s",
					  mc_scheme_fai_rc);
        	 ncmcrd_log_message( 1, 1, err_mesg, "PinnacleMsgHandler" ,0,"");
         }
      break;

      case MT_SYSTEM_REPLY: 
      break;

      case MT_TIMER_REPLY:
         /* Message to host has timed out */
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

            memset( err_mesg, 0x00, sizeof(err_mesg) );
            sprintf( err_mesg,
                    "Error from Timerds, result code = %c, stan = %s",
                     timer_result, auth_tx.TLF01_details.sys_trace_audit_num );

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
            		auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            strcat(err_mesg,Log_Bufr);
            ncmcrd_log_message( 3, 2, err_mesg , "PinnacleMsgHandler" , 0 ,auth_tx.TLF01_details.card_num);
         }
      break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE:
         if ( OK != security_reply(p_msg_in) )
         {
            ncmcrd_log_message( 1, 3,"Error processing pin translation reply", "PinnacleMsgHandler" , 0 ,"");
         }
         break;
      case MT_NSP_DECRYPT_TRANSLATE_PIN_RESPONSE:
          if ( OK != security_reply_for_decrypn_encrypn(p_msg_in) )
          {
             ncmcrd_log_message( 1, 3,"Error processing pin translation reply", "PinnacleMsgHandler" , 0 ,"");
          }
          break;
      case MT_GET_STATS:
         (void)send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:
         (void)reset_transaction_statistics( p_msg_in );
         break;

      default :
         ncmcrd_log_message( 3, 3,"Unknown message type received", "PinnacleMsgHandler" , 0 ,"");
      break;
   }  /* switch msg_type */

} /* PinnacleMsgHandler */


/******************************************************************************
 *
 *  NAME:         MainProcessor
 *
 *  DESCRIPTION:  This is the driver function for the MasterCard host handler.
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
   CHAR      ErrorMsg[512] = {0};
   CHAR      Buffer[512]   = {0};
   INT       ret_val;
   INT      err_ctr = 0;


   GetAppName( AppName );
   strcpy( cur_app_name, AppName );
   sprintf( Buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   ncmcrd_get_error_warning_file_name_path();
   ncmcrd_log_message( 2, 1, Buffer , "MainProcessor" , 0 ,"");
   printf( "%s\n", Buffer );

   strcpy( Last_Tran_Logging_Filename, ncmcrd_error_warning_file_path );
   strcat( Last_Tran_Logging_Filename, ServiceName );
   strcat( Last_Tran_Logging_Filename, "last_tran.log" );

     /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***************/
#ifdef _DEBUG   
   if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) ) 
#else
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName,"pte_ipc"))
#endif
   {
      ncmcrd_log_message( 3, 3, "Failed to create XIPC queues" , "MainProcessor" , 0 ,"");

      /* Signal that we are ready to shut down and quit */
      /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***********/
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
      MainProcessDone = 1;
   }
   else
   {
      /* MOVE ALL APP SPECIFIC INITIALIZATION FROM YOUR MAIN FUNCTION HERE */

	  /* Read tf.ini for configuration details */
	   ncmcrd_read_tf_ini();
	   Read_Incoming_TIMEOUT_Response_Code();
	   ncmcrd_read_fai_rc_from_tf_ini();
	   Get_Name_Validation_Parameters_FromTF_INI();
#ifdef MC7_APR2024
	   ncmcrd_read_country_code_from_tf_ini();
#endif

      /* Perform Service initialization. */
      ret_val = StartUp();
      if ( OK == ret_val )
      {
         while( !EndProcessSignalled )
         {
            /* You are blocked here waiting for a message on either app queue
             * or control queue. If there is no message on either queue for 5
             * seconds, the blocking call returns.
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
               ncmcrd_log_message( 3, 3, Buffer , "MainProcessor" , 0 ,"");

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
                           ServiceName );
                  ncmcrd_log_message( 3, 3, Buffer , "MainProcessor" , 0 ,"");
                   #ifdef WIN32
                     Sleep (5000);
                  #else
                     usleep (5000000);
                  #endif
               }
            }
         }
      }

      /* Shutting down, need to change state before exiting Xipc. */
      write_to_shared_mem_table( AppName, "0", "0", DOWN,
                                 HOST_STATE_CHANGED, Buffer );
      send_network_status_to_monitor();

      /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***********/
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
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
   CHAR Buffer[100] = {0};

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         ncmcrd_log_message( 3, 1, Buffer , "EndProcess" , 0 ,"");
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   ncmcrd_log_message( 2, 1, Buffer , "EndProcess" , 0 ,"");
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}


/******************************************************************************
 *
 *  NAME:         StartUp
 *
 *  DESCRIPTION:  This function performs required initialization when the
 *                service starts up.  It creates the shared memory table, gets
 *                NCF01, and may perform auto logon.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT StartUp()
{
   CHAR		Buffer[256]={0};
   CHAR     err_msg[256]={0};
   AUTH_TX  auth_tx;
   INT      ret_val;
   INT      rc;
   BYTE     ncf01_ret_val;

   /* Calculate Data Structure sizes to save time during processing. */
   Tlf01_Size   = sizeof(TLF01);
   Ncf01_Size   = sizeof(NCF01);
   Ncf30_Size   = sizeof(NCF30);
   Auth_Tx_Size = sizeof(AUTH_TX);
   Mcc_Gui_Size = sizeof(MCC_ONLINE_UPDATES);
   Bdkkey01_Size = sizeof(BDKKEY01);

   /*  Create a table to be used for shared memory between multiple
    *  instances of this Network Control Module.  It contains three 
    *  fields:  # of consecutive timerouts, # of active transactions, 
    *  and the current state of the host.
    */
   ret_val = create_shared_mem_table( AppName, err_msg );
   if ( OK != ret_val )
   {
      ncmcrd_log_message( 3, 3, err_msg , "StartUp" , 0 ,"");
   }
   else
   {
      /* Get NCF01 record for switched out transactions. */
      memset(&ncf01_i, 0, Ncf01_Size);
      strcpy(ncf01_i.primary_key.network_id, AppName);
      ncf01_i.primary_key.network_type[0] = 'I';
      ncf01_ret_val = get_ncf01_cfg( &ncf01_i, err_msg);
      if ( PTEMSG_OK != ncf01_ret_val )
      {
         ret_val = NOT_OK;
         memset( Buffer, 0x00, sizeof(Buffer) );
         strcpy( Buffer, "Error on select of NCF01 issuer: " );
         strncat( Buffer, err_msg, 50 );
         ncmcrd_log_message( 3, 3, Buffer , "StartUp" , 0 ,"");
      }
      else
      {
         /* Get NCF01 record for switched in transactions. */
         memset(&ncf01_a, 0, Ncf01_Size);
         strcpy(ncf01_a.primary_key.network_id, AppName);
         ncf01_a.primary_key.network_type[0] = 'A';
         ncf01_ret_val = get_ncf01_cfg( &ncf01_a, err_msg);
         if ( PTEMSG_OK != ncf01_ret_val )
         {
            ret_val = NOT_OK;
            memset( Buffer, 0x00, sizeof(Buffer) );
            strcpy( Buffer, "Error on select of NCF01 acquirer: " );
            strncat( Buffer, err_msg, 50 );
            ncmcrd_log_message( 3, 3, Buffer , "StartUp" , 0 ,"" );
         }
         else
         {
        	 /* Get BDKkey01 details for Online PIN validation */
             memset(&Bdkkey01, 0, Bdkkey01_Size);
             strcpy(Bdkkey01.card_brand, "MC");
             ncf01_ret_val = get_bdkkey01_cfg( &Bdkkey01, err_msg);
             if ( PTEMSG_OK != ncf01_ret_val )
             {
                ret_val = NOT_OK;
                memset( Buffer, 0x00, sizeof(Buffer) );
                strcpy( Buffer, "Error on select of BDKKEY01 table: " );
                strncat( Buffer, err_msg, 50 );
                ncmcrd_log_message( 3, 3, Buffer , "StartUp" , 0 ,"" );
             }

            /* Initialize transaction statistics */
            (void)reset_stats( &Txn_Counts );

            /* Initialize configurable counters. */
            MaxActiveTxns     = atoi(ncf01_i.max_active_txns);
            MaxConsecTimeouts = atoi(ncf01_i.max_consecutive_timeouts);
            //HCTS1
            echo_timer_length = atoi(ncf01_i.echo_interval);

            /* Determine if transaction tracing should be turned on. */
            rc = get_trace_ini_info( AppName, DirPath, err_msg );
            if ( rc == 0 )
            {
               Tracing = TRACE_ON;

               /* Open the transaction trace file. */
               memset( TraceFile, 0x00, sizeof(TraceFile) );

               pTraceFile_fp  = &TraceFile_fp;
               ppTraceFile_fp = &pTraceFile_fp;

               if (false == open_trace_file(AppName,DirPath,
                                            ppTraceFile_fp,TraceFile))
               {
                  Tracing = TRACE_OFF;
                  sprintf( err_msg,
                          "Unable to open trace file %s. Tracing is off",
                           TraceFile );
                  ncmcrd_log_message( 3, 1, err_msg , "MainProcessor" , 0 ,"");
               }
               else
            	   ncmcrd_log_message( 3, 1, "Tracing is turned ON" , "MainProcessor" , 0 ,"");
            }
            else
            {
               Tracing = TRACE_OFF;
               if ( rc == -1 )
               {
                  ncmcrd_log_message( 3, 1, err_msg , "MainProcessor" , 0 ,"");
               }
               ncmcrd_log_message( 3, 1, "Tracing is turned off" , "MainProcessor" , 0 ,"");
            }

            /* Check the 'Auto Logon' flag. */
            ret_val = OK;
            if (0 == strcmp(ncf01_i.auto_logon,"1"))
            {
               /* Set up a logon request to the host in auth_tx */
               memset( &auth_tx, 0, Auth_Tx_Size );
               perform_host_logon_request( LOGON_REQUEST, SOURCE_IS_OPERATOR,
                                           &auth_tx);
            }
            if (0 == strcmp( "L", ncf01_i.pad_char ) )
            {
               ret_val = write_to_shared_mem_table( AppName, "0", "0", ONLINE,
                                                    HOST_STATE_CHANGED,err_msg);
            }
         }

         /* Set MCC Validation either On or Off. */
         MCC_Validation_Functionality = MCC_VALIDATION_OFF;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         Read_Incoming_TIMEOUT_Response_Code
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
void Read_Incoming_TIMEOUT_Response_Code(void)
{
   DWORD rc = 0;
   CHAR filename     [80] = {0};
   CHAR tmpstr       [80] = {0};
   CHAR response_code_MC[3] = {0};

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr));
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini");

   memset( response_code_MC, 0x00, sizeof(response_code_MC) );
   rc = GetPrivateProfileString(
             "BRAND_INCOMING_TIMEOUT_RESPONSE_CODE",    /* points to section name       */
             "MC_INCOMING_TIMEOUT_RESPONSE_CODE",  /* points to key name           */
             "91",                       /* points to default string     */
			 response_code_MC,                      /* points to destination buffer */
             sizeof(response_code_MC)-1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );


   strncpy( response_code_for_Incoming_timeout_Transaction, response_code_MC, 2 );
   if(strlen(response_code_for_Incoming_timeout_Transaction)==0)
   {
	   strncpy( response_code_for_Incoming_timeout_Transaction,
			   MC_INCOMING_TIMEOUT_TRANSACTION_RESPONSE_CODE,
	   		    2);// Default Value
   }
}
void ncmcrd_read_fai_rc_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[3]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",           /* points to section name         */
									  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_MC",       	/* points to key name             */
									   "19",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(mc_scheme_fai_rc,readvalue,2);
	   if(rc == 0)
	   {
		   ncmcrd_log_message( 2, 1, "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_MC Value not read from tf.ini","ncmcrd_read_fai_rc_from_tf_ini",0);
	   }

}

void Get_Name_Validation_Parameters_FromTF_INI()
{
	   CHAR  filename     [256]={0};
	   CHAR  tmpstr       [256]={0};

	   GetAscendentConfigDirectory(tmpstr);
	   sprintf(filename, "%stf.ini", tmpstr);

	   GetPrivateProfileString(
	    "NAME_VALIDATION_PARAMETERS",	            /*  points to section name  */
	    "NAME_VALIDATION_FLAG",           /*  points to key name  */
	    "N",                      /*  points to default string  */
	    tmpstr,                         /*  points to destination buffer  */
	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	    filename                        /*  points to initialization filename  */
	   );

	   name_validation_flag=tmpstr[0];

}
#ifdef MC7_APR2024
void ncmcrd_read_country_code_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[4]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "ACCEPTOR_COUNTRY_CODE",           /* points to section name         */
									  "MC_COUNTRY_FIELD",       	/* points to key name             */
									   "PHL",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   if(strlen(readvalue) == 0)
		   strncpy(mc_acceptor_country_code,"PHL",3);
	   else
		   strncpy(mc_acceptor_country_code,readvalue,3);

	   if(rc == 0)
	   {
		   ncmcrd_log_message( 2, 1, "MC_ACCEPTOR_COUNTRY_CODE Value not read from tf.ini","ncmcrd_read_country_code_from_tf_ini",0);
	   }

}
#endif
