/******************************************************************************
 
 	CIRSTRAN.C
 
    Copyright (c) 2003, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Cirrus Host Handler Transaction Processor
 
    This module is a part of the Cirrus Host Handler.  It will parse and
    manipulate all incoming messages from the terminal and from the host.
    Cirrus is the MasterCard Debit Switch.
 
    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nccirrus\Cirstran.c  $
   
      Rev 1.91   Jun 17 2005 11:41:54   chuang
   Version 4.1.1.4. Fix scr 15143, log reject reason code and 
   detail code into TLF01.vehicle_number. log issuer_id. Fix 
   parsing error of field 62.
   
      Rev 1.90   May 17 2005 18:00:16   dirby
   Modified to stop standing in for Auth and Cash Overrides.
   SCR 15420
   
      Rev 1.89   Jun 08 2004 13:50:28   dirby
   Copy text into response_text when host inactive - for Voice GUI.
   Release 4.3.0-1
   
      Rev 1.88   Jul 23 2003 16:11:04   dirby
   Modified to always copy the auth number from the host response
   to auth_tx.  Previously, it would not overwrite an existing auth
   number.  Had to correct this because txcntl is populating auth
   number before txn gets approved and before it gets switched out
   for local profiles with remote destinations.
   SCR 1111
   
      Rev 1.87   Apr 02 2003 14:12:52   dirby
   Added support in function 'process_reconciliation' for 0522 txns.
   SCR 1072
   
      Rev 1.86   Apr 01 2003 14:44:56   dirby
   Added a call to function 'process_issuer_reversals' to handle
   0422/0432 switched in transactions.
   SCR 1073
   
      Rev 1.85   Mar 26 2003 12:44:46   dirby
   Modified to copy account balance information from the host
   into auth_tx so that nceqit can read it.
   SCR 1066
   
      Rev 1.84   Mar 24 2003 17:17:34   dirby
   Removed the use of def_gross_amt field as storage for
   settlement date.
   SCR 1063
   
      Rev 1.83   Mar 24 2003 14:24:54   dirby
   Modified to not copy NULL into the auth_number field upon a
   response from Cirrus.  This was discovered as a result of
   looking into SCR 1064
   
      Rev 1.82   Mar 19 2003 20:30:46   dirby
   Modified to store settlement date in BCH10 (in the def_gross_amt
   field) to be used in reversals.
   SCR 1063
   
      Rev 1.81   Mar 18 2003 16:51:18   dirby
   Removed a 'temp_auth_tx' that was not being used correctly.
   It may have been the source of garbage in field 2 for logons.
   SCR 1057
   
      Rev 1.80   Feb 25 2003 10:09:58   dirby
   Removed a redundant error message for key change failures.
   Enhanced another key change error message.
   SCR 955
   
      Rev 1.79   Feb 17 2003 14:03:08   dirby
   Modified to support single, double, and triple length encryption
   keys for a dynamic key change command.
   SCR 955
   
      Rev 1.78   Aug 21 2002 10:57:10   dirby
   Corrected some bugs that were preventing a key change from
   being stored into NCF01.  A value from a blank auth_tx was
   being used.  Modified timeouts to not process a key change
   time out.  Modified timer reply to not process a response to a
   clear timer command for key change.  Reformatted some
   comments to be consistent with other comment formats.
   SCR 881
   
      Rev 1.77   Aug 09 2002 14:08:56   dirby
   Modified to always send NMI code for Group logons and Group
   logoffs.  Specification indicates to always use "061" and "062".
   SCR 870
   
      Rev 1.76   Aug 02 2002 16:18:18   dirby
   Removed code so that timeout of echoes do not increment the
   timeout counter.  Only do this for financial transactions.  Do not
   want to go offline just because too many echoes time out.
   SCR 594
   
      Rev 1.75   Jul 31 2002 06:44:18   dirby
   Removed code that updated time out count for Monitor when
   an echo request times outs.  This is at the request of EB.
   SCR 594
   
      Rev 1.74   Jul 29 2002 11:05:40   dirby
   Modified to allow echoes to continue to go out when offline and 
   set host state online if a response is received.
   SCR 594
   
      Rev 1.73   Jul 07 2002 03:32:40   dirby
   Fixed bug - Return false if host not online when checking status.
   SCR 594
   
      Rev 1.72   Jul 07 2002 02:59:26   dirby
   Reimplemented going offline when max time outs occur.
   SCR 594
   
      Rev 1.71   Jul 03 2002 17:29:16   dirby
   1.  Modified to use pre-calculated sizes instead of always recalculating a structure size.
   2.  Remove use of read_timer.
   3.  Provide details of errors from timerds.
   4.  Added code to display time outs and reversals on Monitor.
   5.  Version 4.0.3.1
   SCR 621
   
      Rev 1.70   Jan 23 2002 14:40:52   dirby
   Initialized buffers in transaction statistic functions.
   
      Rev 1.69   Jan 16 2002 16:53:56   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.68   Jan 14 2002 18:11:06   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.67   Dec 11 2001 14:53:38   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.66   Oct 18 2000 14:28:14   dirby
   Modified code to be a little more efficient in gathering
   host start and finish times.
   
      Rev 1.65   Sep 29 2000 14:51:26   dirby
   Added a line to initialize a local data structure prior to populating it.
   
      Rev 1.64   Sep 18 2000 16:46:12   dirby
   Changed code where an echo timer expires.  Changed it from
   clearing and setting the echo timer to just setting it.  When it
   expires, the echo timer is automatically cleared.  This change
   eliminates an error message from timerds.
   
      Rev 1.63   Aug 28 2000 10:16:06   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.62   Aug 28 2000 09:19:42   dirby
   Changed the MONITOR_STATS structure from 'long's to 'char's.
   Unix seems to translate longs differently than NT does.
   
   
      Rev 1.61   Aug 23 2000 13:37:46   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.60   Aug 08 2000 13:03:14   dirby
   Removed the logic of fetching NCF01 during every online txn.
   
      Rev 1.59   Jul 13 2000 16:15:24   dirby
   Modified the standin processing function to not
   overwrite the auth_number with the time if the auth
   number already exists.
   
      Rev 1.58   Feb 23 2000 15:58:34   ddabberu
   process_reject() updated, For 0620 messages if we dont get banket data
   as it is mandatory for 0630, i am sending a 0
   
      Rev 1.57   Feb 23 2000 12:43:24   ddabberu
   process_reject() modified
   #scr 427
   
      Rev 1.56   Feb 14 2000 14:56:24   svelaga
   Modified the logic to maintain the transaction
   integrity.
   
      Rev 1.55   Jan 27 2000 13:55:02   dirby
   1.  Removed the 'clear timer' command when SAF timer expires.
   2.  Removed echo test messages going to Monitor.
   
      Rev 1.54   Jan 27 2000 09:27:22   dirby
   Modified to not update NCF21 and NCF30 for Balance Inquiries.
   
      Rev 1.53   Jan 25 2000 14:57:58   dirby
   Changed a clear-and-set command for the echo timer
   to a clear followed by a set command.  This way, if the clear
   fails, the set still works.
   
      Rev 1.52   Jan 25 2000 10:02:14   dirby
   Added standin processing for some void transactions.
   
      Rev 1.51   Jan 24 2000 14:57:44   dirby
   1.  Put code back in to clear a timer after it has expired.
   2.  Modified the 'set echo timer' to be 'clear and set echo timer' when
       an echo timer expires.
   
      Rev 1.50   Jan 23 2000 16:36:08   dirby
   Incorporated echo testing logic.
   
      Rev 1.49   Jan 18 2000 12:05:42   dirby
   In function 'alter_transaction_data', modified the check for balance inquiry
   to use p_auth_tx_out, instead of p_auth_tx_in which is always zero.
   
      Rev 1.48   Jan 18 2000 11:50:18   ddabberu
      1.  Removed the code to clear a timer when there is no response from the host.
          Did this because the timer gets cleared when it times out.
      2.  Modified the function that converts request tx_keys to responses so that the
          default will add 100 to the request, making it a response.
   
   
      Rev 1.47   Jan 17 2000 10:34:24   ddabberu
   When an (Network mgmt) 820 class 1 message is sent,the ncf01 table is updated with the CWK from field48 .
   1.process_network_advice() function is updated to do the above functionality.
   2.incoming_host_message_timer_reply() is updated
   
      Rev 1.46   Jan 14 2000 15:25:04   dirby
   Set default to original tx_key in function that converts a tx_key
   from a request to a response.
   
      Rev 1.45   Dec 28 1999 13:04:40   dirby
   Changed the timer so it will not repeat if it is not cleared.
   
      Rev 1.44   Nov 19 1999 09:32:58   dirby
   Modified a call to read the timer to use the Timer Data Server.
   
      Rev 1.43   Nov 10 1999 14:29:36   dirby
   Removed the condition to only send responses to txcntl if they
   are not reversal.  So now reversals also have their responses
   sent to txcntl.
   
      Rev 1.42   23 Aug 1999 17:08:36   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.41   05 Aug 1999 13:34:02   dirby
   Added current balance and available balance to function
   alter_transaction_data for balance inquiries.
   
      Rev 1.40   05 Aug 1999 12:13:56   dirby
   Removed code that put the phrase "Problem with key data" into
   field 44 when key change failed with response code of 96.
   
      Rev 1.39   30 Jul 1999 16:50:16   dirby
   Changed general status from DROPPED to SYSTEM_ERROR.  The Dropped
   status acts as a Line Feed in the database, causing display nuances.
   
      Rev 1.38   14 Jul 1999 16:58:44   dirby
   Fixed 2 typos dealing with line indentation.
   
      Rev 1.37   14 Jul 1999 08:21:40   dirby
   Hard coded the acquirer id to be 'nccirs' in auth_tx.  This is so Racal
   knows the txn is for Cirrus.
   
      Rev 1.36   Jun 25 1999 15:06:34   svelaga
   Fixed bug : 
   
   1. Key change request - Host sends 0800 message.
      sometimes we respond with 0810. sometimes we don't.
   
   2. Changed all "TO" response codes to "19".
   
      Rev 1.35   21 Jun 1999 16:36:34   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.34   21 Jun 1999 16:09:36   dirby
   Added line of code to get tpdu id from NCF01 for messages
   from the host.  Needed for the response.
   
      Rev 1.33   16 Jun 1999 15:54:48   dirby
   Modified to preserve original request message type
   when sending response to tran control for switched out txns.
   
      Rev 1.32   Jun 14 1999 13:56:02   svelaga
   bug fix : 08xx messages received from the master card network are logged into TLF01
   
      Rev 1.31   Jun 10 1999 10:20:24   svelaga
   length of the ZMK, which is copied to PTE message for racal is adjusted to
   16 instead of 32.
   
      Rev 1.30   02 Jun 1999 15:28:24   dirby
   Set message type to be 0810 for logon responses.
   
      Rev 1.29   28 May 1999 17:58:36   dirby
   Added functionality to fill the response text field in auth_tx.
   
      Rev 1.28   10 May 1999 16:22:02   dirby
   Added code to handle terminal Ids that are less than
   8 characters long.  This included modifying the way
   a timer lookup key is created.
   
      Rev 1.27   30 Apr 1999 14:37:20   dirby
   Changed acquirer_id from 'cur_app_name' to 'AppName'.
   
      Rev 1.26   21 Apr 1999 10:03:48   dirby
   Changed function 'create_rrn' to use 'sprintf' instead
   of 'strncpy' and 'strcat's.
   
      Rev 1.25   19 Apr 1999 16:53:14   dirby
   Added a line to flag host line down when echo test fails.
   
      Rev 1.23   14 Apr 1999 17:05:18   dirby
   Added cases to process 3 offline OVERRIDE transactions
   that might come from voice authorization.
   
      Rev 1.22   14 Apr 1999 11:05:28   dirby
   Added check to set request_timer to the default value of 15
   if the value in NCF01 is NULL.
   
      Rev 1.21   13 Apr 1999 16:39:48   dirby
   Added check to not send a timeout response to txcntl if
   message type is 08xx.
   
      Rev 1.20   09 Apr 1999 15:44:52   dirby
   Changed the format of RRN when creating it.
   
      Rev 1.19   Mar 30 1999 14:19:54   egable
   Only set the timer if the host message is
   built successfully.  Error report #438.
   
      Rev 1.18   24 Mar 1999 16:29:40   dirby
   Adjusted some of the shared memory function calls.
   
      Rev 1.17   23 Mar 1999 10:10:08   dirby
   Added code to fill 'acquirer_id', 'host_start_time', and 'host_finish_time'.
   
      Rev 1.16   22 Mar 1999 10:27:02   dirby
   Added ZMK to security request for key change commands.
   
      Rev 1.15   19 Mar 1999 17:12:42   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.14   18 Mar 1999 15:34:24   dirby
   Created a separate function to send a PTE message to a destination queue.
   This new function, 'send_msg' calls 'pteipc_send' and handles send errors.
   This eliminates lots of repetitive code, especially on the error handling.
   
      Rev 1.13   17 Mar 1999 10:02:12   dirby
   Modified 'perform_host_logon_request' SOURCE_IS_HOST to respond
   to the host regardless of what current_state is.  Then set current_state
   to zero (logged on) if not already set.
   
      Rev 1.12   16 Mar 1999 15:46:50   dirby
   Removed call to function 'perform_error_processing' for
   messages from the host in function cirsutil_parse_db_error.
   
   Changed network_type to BYTE in get_ncf01( ).
   
      Rev 1.11   16 Mar 1999 12:10:56   dirby
   Initialized buffer1 prior to use, in function process_network_advice_ncf01_reply.
   
      Rev 1.10   15 Mar 1999 16:28:18   dirby
   Added code to finish the requirement of pin key changes.
   
      Rev 1.9   12 Mar 1999 16:42:56   dirby
   Added functions to perform pin key exchange.
   Only the code to handle the initial request from the
   host is in place.  Still need to code the response.
   
      Rev 1.8   11 Mar 1999 14:22:08   dirby
   Created a separate function to get NCF01 record.
   Added some error handling capability for reversals.
   
      Rev 1.7   10 Mar 1999 17:02:26   dirby
   Removed use of 'get_tran_type'.
   Added AUTH types of RECONCILE_ADVICE and ADMIN_ADVICE
   in function 'match_request_to_response'.
   
      Rev 1.6   09 Mar 1999 16:32:38   dirby
   Added 2 lines to set SAF flag to "Y" if a reversal times out.
   
      Rev 1.5   08 Mar 1999 15:22:48   dirby
   Rearranged some code in function 'incoming_host_message_timer_reply'
   to only change host state if not in SAF mode.
   
      Rev 1.4   05 Mar 1999 16:59:20   dirby
   Modified timeout code to not send a response to the terminal
   if a SAF transaction timed out.
   
      Rev 1.3   04 Mar 1999 16:47:12   dirby
   Updated for AIX port.
   
      Rev 1.2   03 Mar 1999 15:49:30   dirby
   Added special processing for reversal when sending
   a message to the host, but the host status is not ready.
   
      Rev 1.1   25 Feb 1999 16:46:46   dirby
   Modified the host timer setting and clearing
   for log on messages.
   
      Rev 1.0   19 Feb 1999 16:19:54   dirby
   Initial version
   
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#endif
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_database.h"
#include "nc_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "memmnger.h"
#include "cirsmain.h"
#include "cirsutil.h"
#include "echo_timer.h"
#include "txutils.h"


/* Global Variables */
extern CHAR   Settlement_ID[12];
extern CHAR   Stan[7];
extern CHAR   Logon_NMI_Code[4];
extern CHAR   AppName[8];

/* Database structures */
extern NCF01             ncf01;
extern NCF21             ncf21;
extern NCF30             ncf30;
extern TLF01             tlf01;

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf21_Size;
extern INT    Ncf30_Size;
extern INT    Saf01_Size;
extern INT    Auth_Tx_Size;

/* Shared memory variables */
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];
extern BYTE   current_state[2];

/* Shared Memory Table Name */
extern CHAR  SHARED_MEMORY_TBL[15];

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* These are used for reject 0620/0630 messages. */
extern CHAR   addl_data_len[4];
extern CHAR   addl_data[101];
extern CHAR   advice_len[4];
extern CHAR   advice[61];
extern CHAR   recv_inst_len[3];
extern CHAR   recv_inst[12];
extern CHAR   F60_len[4];
extern CHAR   reject_advice_code[4];
extern CHAR   reject_detail_code[5];
extern CHAR   reject_text[54];


/* Queue definitions */
extern CHAR  cur_app_name [];
extern CHAR  authorizer_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  applnk_que_name [];
extern CHAR  oprmon_que_name[];
extern CHAR  trands_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  netds_que_name[];
extern CHAR  security_que_name[];
extern CHAR  timerds_que_name[];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

extern INT   reqidx;                 /* next open position in request buffer */
extern INT   buflen;                 /* total size of buffer */
extern INT   bitmapidx;              /* offset of bitmap... */

extern BYTE ReqRsp_Buffer [COMM_BUF_SZE];
extern int  BitFieldSize;

extern char  A2E[4];    /*  ASCII to EBCDIC conversion */
extern char  E2A[4];    /*  EBCDIC to ASCII conversion */


/******************************************************************************
 *
 *  NAME:         SEND_IT_BACK_TO_TERMINAL
 *
 *  DESCRIPTION:  This function will build a message to send to the terminal
 *                handler.  This is used for when a transaction has been
 *                rejected before being sent to the host.
 *
 *  INPUTS:       resp_code - Response code to be placed into p_auth_tx
 *                p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN send_it_back_to_terminal( pCHAR resp_code, pAUTH_TX p_auth_tx )
{
   BOOLEAN   ret_code = false;

   strcpy( p_auth_tx->TLF01_details.response_code, resp_code );
   p_auth_tx->TLF01_details.tx_key = 
             match_request_to_response( p_auth_tx->TLF01_details.tx_key );

   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE );
   return( ret_code );
}


/******************************************************************************
 *
 *  NAME:         INCOMING_TERMINAL_REQUEST
 *
 *  DESCRIPTION:  Process a transaction from a terminal.  At this point, we
 *                have all the data we need from the transaction and from the
 *                data server.  So all that is left to do is to verify the host
 *                is up, then put the message together and send it to the host.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/03/99 - Added special processing section for reversals.
 *     DI - 04/14/99 - Added offline cases for override transactions.
 ******************************************************************************/
INT incoming_terminal_request( pAUTH_TX p_auth_tx )
{
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   CHAR            ErrorMsg[200] = "";
   BYTE            temp_tx_key;

   /* See if the transaction failed security for some reason. */
   if( ( GS_NSP_KEYCHANGE_FAILED <  p_auth_tx->TLF01_details.general_status ) &&
       ( GS_NSP_OUTOFSERVICE     >= p_auth_tx->TLF01_details.general_status )  )
   {
      /* It failed.  Send it back to the terminal. */
      if( send_it_back_to_terminal( "58", p_auth_tx ) )
         return( true );
      return( false );
   }

   /* Set general_status for auth_tx to NEW. */
   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;

   /*+-------------------------------------------------+
     | Check if we do standin processing or if we need |
     | to go online to the host for this transaction.  |
     +-------------------------------------------------+*/
   temp_tx_key = p_auth_tx->TLF01_details.tx_key;
   switch ( temp_tx_key )
   {
      case AUTH_OFFLINE_SALE: 
      {  /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_REFUND: 
      {  /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_OFFLINE_REFUND: 
      {  /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_VOID_SALE:
      {  /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
          return( true );
      }
      case AUTH_VOID_REFUND:
      {  /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );		 
         return( true );
      }
      case AUTH_SALE_ADJUSTMENT: 
      {
         p_auth_tx->TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_REFUND_ADJUSTMENT: 
      {
         /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_ADJUSTMENT_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE: 
      {
         p_auth_tx->TLF01_details.tx_key =
                     AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE: 
		{
            p_auth_tx->TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;
            break;
		}
      case AUTH_AUTHORIZATION_OVERRIDE: 
		{
            p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION;
            break;
		}
      case AUTH_VOID_CASH_ADVANCE: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_VOID_CASH_ADVANCE_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_SALES_COMPLETION: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_SALES_COMPLETION_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_OFFLINE_VOID_SALE: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_SALE_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_OFFLINE_VOID_REFUND: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_REFUND_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      default:  /* online transaction to host */
      {
      }
   } /* end switch statement */ 
   
   /***************************************************************************/


   /* Confirm host is available */
   if( false == verify_host_app_status( p_auth_tx ) )
   {
      read_shared_mem_table( net_consec_tmouts_ctr,
                             active_txns_ctr, current_state );

      /* Do special processing for reversals. */
      if ( AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key )
      {
         if ( 0 == strcmp( "Y", p_auth_tx->TLF01_details.saf ) )
         {
            return( send_request_to_host( p_auth_tx ) );
         }
         else
            /* Must store reversal for later forwarding. */
            strcpy( p_auth_tx->TLF01_details.saf, "Y" );
      }

      /* Build response_for_terminal */
      strcpy( p_auth_tx->TLF01_details.response_text, "Host Inactive" );
      cirsutil_error_handler(1,
                             "Host Inactive",
                             0, WARN_MSG, "incoming_terminal_request", 2,
                             WARNING_ERROR,p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "404");
      p_auth_tx->TLF01_details.tx_key = 
                   match_request_to_response( p_auth_tx->TLF01_details.tx_key );

      perform_error_processing( p_auth_tx, "91" );

      /* Check if we are in start up state */
      if( 0 == strcmp( current_state, OFFLINE ) )
      {
         cirsutil_error_handler(1,
                                "Host Inactive: current_state is Offline",
                                0, WARN_MSG, "incoming_terminal_request", 2,
                                WARNING_ERROR, p_auth_tx->TLF01_details.terminal_id,
                                p_auth_tx->TLF01_details.card_num,
                                p_auth_tx->TLF01_details.merchant_id,
                                "405");

         /* Set up a logon request to the host in temp_auth_tx. */
         perform_host_logon_request(NULL_STR,SOURCE_IS_HOST_HANDLER,p_auth_tx);
         if ( false == send_request_to_host( p_auth_tx ) )
            return( false );
      }
	  return( true );
   }
   else
   {
      /* Host is working.  Build PTEMSG's to be sent */
      if( false == send_request_to_host( p_auth_tx ) )
         return( false );
   }
   return( true );
} /* incoming_terminal_request */

/******************************************************************************
 *
 *  NAME:         PROCESS_INCOMING_MESSAGE
 *
 *  DESCRIPTION:  Process a message coming in from a terminal.  But process
 *                only up to the point where a call is sent to the data server
 *                to get specific information for this host handler.  Once the
 *                call is made to the data server, the function exits.  The
 *                processing will be picked up in 'process_incoming_message_
 *                ncf01_reply'.
 *
 *  INPUTS:       p_msg - This is the incoming message from a terminal handler.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/11/99 - Moved NCF01 code to a separate function.
 *     DI - 03/23/99 - Added acquirer id.
 *     DI - 07/13/99 - Hard coded acquirer id so Racal knows its a Cirrus txn.
 ******************************************************************************/
INT process_incoming_message( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   LONG            data_len;
   LONG            auth_len;
   AUTH_TX         auth_tx;
   INT             ret_val = true;
   CHAR            Buff[100];

   memset( &auth_tx, 0x00, Auth_Tx_Size );

   /* Copy transaction record into auth_tx structure. */
   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   auth_len   = data_len;
   if ( Auth_Tx_Size < data_len )
      auth_len = Auth_Tx_Size;
   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &auth_tx, p_data_data, auth_len );
   p_data_data = p_data_data + Auth_Tx_Size;

   /* Set acquirer Id */
   strcpy( auth_tx.TLF01_details.acquirer_id, "nccirs" );

   /* Store the Dialog connection into auth_tx. */
   strcpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id );
   
   /* Go to a separate function to process reversals. */
   if ( AUTH_REVERSAL == auth_tx.TLF01_details.tx_key )
   {
      /* Display reversal message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants or to networks.
       */
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff,
              "Reversal going to Cirrus. Card %s, Merchant %s",
               auth_tx.TLF01_details.card_num,
               auth_tx.TLF01_details.merchant_id );

      TxUtils_Send_Msg_To_Operator (1, Buff, 1, INFO_MSG, 
                                   "process_incoming_message", 2, INFO_ERROR,
                                    auth_tx.TLF01_details.terminal_id,
                                    auth_tx.TLF01_details.card_num,
                                    auth_tx.TLF01_details.merchant_id );

      ret_val = (INT)process_out_going_reversal( &auth_tx );
      if ( false == ret_val )
         (void)insert_into_saf01( &auth_tx );
   }
   else
   {
      /* It is not a reversal.  Process the transaction.
       *
       * Check to see if PIN translation is required.
       */
      if ( true == cirsutil_verify_pin_block(&auth_tx) )
      {
         /* PIN Translation is required */

         /* Store the keys for PIN translation. */
         strcpy( auth_tx.TLF01_details.dest_key,        ncf01.cwk );
         strcpy( auth_tx.TLF01_details.dest_pin_format, ncf01.pin_type );

         ret_val = security_request( &auth_tx, MT_NSP_TRANSLATE_PIN );
      }
      else
      {
         /* Do not perform PIN translation. */
         ret_val = process_incoming_message_continued( &auth_tx );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_INCOMING_MESSAGE_CONTINUED
 *
 *  DESCRIPTION:  This function continues the processing of an incoming
 *                message.  It is used as a common stepping stone to get
 *                to 'incoming_terminal_request'.  It is called after NCF01
 *                data is obtained, and also after PIN verification has
 *                been performed.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_incoming_message_continued( pAUTH_TX p_auth_tx )
{
   if ( ( AUTH_SALE    <= p_auth_tx->TLF01_details.tx_key    ) &&
        ( AUTH_INVALID >  p_auth_tx->TLF01_details.tx_key )  )
   {
      if ( incoming_terminal_request( p_auth_tx ) )
         return( true );
   }
   else
   {
      cirsutil_error_handler(1, "Invalid transaction key", 0, WARN_MSG,
                             "process_incoming_message_continued", 2,
                             WARNING_ERROR, p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "531");
   }
   return( false );
}

/******************************************************************************
 *
 *  NAME:         SECURITY_REQUEST
 *
 *  DESCRIPTION:  This function builds a PTE message for the crypto gear.
 *                It uses message type to determine if the request is for pin
 *                translation, CVC verification, or key change.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *                msg_type  - Indicates purpose of security message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if PTE message was successfully built and sent,
 *                else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT security_request( pAUTH_TX p_auth_tx, BYTE msg_type )
{
   pPTE_MSG        p_msg_ins = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   BOOLEAN         ret_code = false;

   /* Build message for crypto. */
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );

   if ( MT_NSP_KEYCHANGE == msg_type )
      memcpy( buffer1+Auth_Tx_Size, ncf01.zmk, 48 );
   else
      memcpy( buffer1+Auth_Tx_Size,&ncf01, Ncf01_Size );

   p_msg_ins = ptemsg_build_msg( msg_type, 
                                 ST1_NONE,
                                 ST2_NONE,
                                 security_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 0 );
   if ( NULL_PTR == p_msg_ins  )
   {
      cirsutil_error_handler(1, "Insufficient Memory for crypto message",
                             1, ALERT_MSG, "security_request", 4, FATAL_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "506");
   }
   else
   {
      /* Send message to crypto gear */
      ret_code = send_msg( p_msg_ins, security_que_name, p_auth_tx );
      free( p_msg_ins );
   }
   return( (INT)ret_code );
}

/******************************************************************************
 *
 *  NAME:         SECURITY_REPLY
 *
 *  DESCRIPTION:  This function continues the processing of a transaction after
 *                having the pin translated or the CVC verified.
 *
 *  INPUTS:       p_msg_in  - Pte message from the crypto application.
 *                msg_type  - Indicates what functionality security performed
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT security_reply( pPTE_MSG p_msg_in, BYTE msg_type )
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   AUTH_TX       auth_tx;
   INT           ret_val = true;

   /* If message came from crypto, get it from the buffer. */
   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   pCurrent   = ptemsg_get_pte_msg_data_data( p_msg_data );

   memset( &auth_tx, 0x00,     Auth_Tx_Size );
   memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

   if ( MT_NSP_KEYCHANGE_RESPONSE == msg_type )
   {
      if ( 0 == strcmp( "00", auth_tx.TLF01_details.security_response_code ) )
      {
         (void)create_key_change_response( &auth_tx );
      }
      else
      {
         /* PIN Key Exchange failed. Security module informed Monitor. */
         process_unsuccessful_key_change( &auth_tx, "96" );
         ret_val = false;
      }
   }
   else if ( MT_NSP_TRANSLATE_PIN_RESPONSE == msg_type )
   {
      if ( 0 == strcmp( NULL_STR, ncf01.primary_key.network_id ) )
      {
         memset( &ncf01, 0x00, Ncf01_Size );
         memcpy( &ncf01, pCurrent+(Auth_Tx_Size), Ncf01_Size);
      }

      if ( 0 != strcmp( "00", auth_tx.TLF01_details.security_response_code ) )
      {
         /* PIN Translation failed. Security module informed Monitor. */
         perform_error_processing( &auth_tx, "96" );
         cirsutil_error_handler(1,
                           "Pin translation failed. Cannot process request.",
                            1, WARN_MSG, "security_reply", 4,
                            WARNING_ERROR, auth_tx.TLF01_details.terminal_id,
                            auth_tx.TLF01_details.card_num,
                            auth_tx.TLF01_details.merchant_id,
                            "540");
         ret_val = false;
      }
      else
         ret_val = process_incoming_message_continued( &auth_tx );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PERFORM_STANDIN_PROCESSING
 *
 *  DESCRIPTION:  This procedure takes care of off line transactions that
 *                do not require host authorization.
 *
 *  INPUTS:       rsp_code   - Response code to place into the auth_tx block
 *                gen_status - Txn status to place into the auth_tx block
 *                p_auth_tx  - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void perform_standin_processing( pCHAR rsp_code, BYTE gen_status,
                                 pAUTH_TX p_auth_tx )
{
   BOOLEAN ret_code = false;


   /* Create response code and store it in the auth_tx block  */
   strcpy(p_auth_tx->TLF01_details.response_code, rsp_code);
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.auth_number ) )
   {
      strcpy(p_auth_tx->TLF01_details.auth_number,
             p_auth_tx->TLF01_details.time_hhmmss);
   }

   /* Change status to completed and store it in the auth_tx block */
   p_auth_tx->TLF01_details.general_status = gen_status;

   /* Set transaction key to be that of a response for the authorizer's sake */
   p_auth_tx->TLF01_details.tx_key = 
             match_request_to_response( p_auth_tx->TLF01_details.tx_key );

   /* Send message to the authorizer */
   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE );

} /* perform_standin_processing */


/******************************************************************************
 *
 *  NAME:         PROCESS_LOGON_RESPONSE
 *
 *  DESCRIPTION:  This function will process a log on response by resetting
 *                the host app fields to there original values.
 *
 *  INPUTS:       p_auth_tx - Transaction response from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BOOLEAN process_logon_response( pAUTH_TX p_auth_tx )
{
   CHAR     Buf[100];
   INT      insert_flag = 0;
   BOOLEAN  ret_code = false;
   INT      i;
   WORD     timer_length;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];

   /* Act slightly differently for Log ONs versus Log OFFs */
   if ( 0 == strncmp(Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF, 3)   )
   {
      /* LOG OFF */
      if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, "00", 2 ) )
      {
         set_host_state( OFFLINE );
         cirsutil_error_handler(1,
                                "Log OFF Successful. Setting Host State to 2",
                                0, INFO_MSG, "process_logon_response", 0,
                                INFO_ERROR, NULL_STR, NULL_STR,
                                NULL_STR, "488");
         (void)saf_clear_timer();
      }
      else
      {
         insert_flag = 1;
         cirsutil_error_handler(1,
                                "Log Off attempt Failed",
                                0, WARN_MSG, "process_logon_response", 2,
                                WARNING_ERROR, NULL_STR, NULL_STR,
                                NULL_STR,"489");
      }
   }
   else if ( 0 == strncmp(Logon_NMI_Code, LOGON_NMI_GROUP_ID, 3)   )
   {
      /* LOG ON */
      if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, "00", 2 ) )
      {
         set_host_state( ONLINE );
         strcpy( Buf, "Log ON Successful. Setting Host State to ONLINE. " );
         strcat( Buf, " Checking for Store and Forward." );
         cirsutil_error_handler(1, Buf, 0, INFO_MSG, "process_logon_response",0,
                                INFO_ERROR, NULL_STR, NULL_STR,
                                NULL_STR, "444");

         /* Send request to perform store and forward. */
         if ( false == saf_send_request_for_record() )
         {
            insert_flag = 1;
            cirsutil_error_handler(1,
                                "Unable to send request for Store and Forward.",
                                0, WARN_MSG, "process_logon_response", 2,
                                WARNING_ERROR, NULL_STR, NULL_STR,
                                NULL_STR,"555");
         }
      }
      else
      {
         insert_flag = 1;
         cirsutil_error_handler(1,
                                "Log On attempt Failed",
                                0, WARN_MSG, "process_logon_response", 2,
                                WARNING_ERROR, NULL_STR, NULL_STR,
                                NULL_STR, "487");
      }
   }
   else
   {
      insert_flag = 1;
      /* ECHO TEST */

      ret_code = read_shared_mem_table( timeout_cnt, active_cnt, host_state );
      if ( ret_code == true )
      {
         if ( host_state[0] == cONLINE )
         {
            if ( 0 != strncmp( p_auth_tx->TLF01_details.response_code, "00", 2))
            {
               set_host_state( OFFLINE );
               cirsutil_error_handler(1,
                                      "Log On ECHO TEST FAILED!!",
                                      0, WARN_MSG, "process_logon_response", 2,
                                      WARNING_ERROR, NULL_STR, NULL_STR,
                                      NULL_STR, "406");
            }

            /* Received a message from host, clear and set idle line timer. */
            (void)clear_echo_timer( ECHO_TIMER_KEY,
                                    (pPTE_MSG *)NULL_PTR,
                                    application_que_name, 1 );
            timer_length = atoi( ncf01.echo_interval );
            if((0 != strcmp(NULL_STR, ncf01.echo_interval)) && timer_length > 0)
            {
               if ( false == set_echo_timer( ECHO_TIMER_KEY, timer_length,
                                             (pPTE_MSG*)NULL_PTR,
                                             application_que_name, 1 ) )
               {
                  cirsutil_error_handler(1,"Unable to set idle line timer.",
                                         1,ERROR_MSG,"process_logon_response",3,
                                         WARNING_ERROR, NULL_STR, NULL_STR,
                                         NULL_STR, "803" );
               }
            }
         }
         else if ( host_state[0] == cOFFLINE )
         {
            /* Echo response: if offline, set state Online. */
            set_host_state( ONLINE );
            cirsutil_error_handler(1,
                                 "Received response to echo. Going back Online",
                                   1, INFO_MSG, "process_logon_response", 1,
                                   INFO_ERROR, NULL_STR, NULL_STR,
                                   NULL_STR, "");

            /* Clear the Echo Request timer. */
            (void)clear_echo_timer( ECHO_REQ_TIMER_KEY,
                                    (pPTE_MSG *)NULL_PTR,
                                    application_que_name, 1 );
         }
      }
   }
   reset_timeout_count();
   strcpy( Logon_NMI_Code, NULL_STR );

   if(insert_flag == 0)
   {
	  /* Transaction is completed.  Insert it into the database. */
      for( i=0; i<4; i++ )
            p_auth_tx->TLF01_details.message_type[i] =
                           ascii_ebcdic_conv(ReqRsp_Buffer[i], E2A);

      (void)insert_into_tlf01( p_auth_tx );
   }
   return( true );
}


/*****************************************************************************
 *
 * NAME:        calc_julian_day
 *
 * DESCRIPTION: This module calculates Julian Day ( DDD ) from the 
 *              input.
 * 
 * INPUTS:      date_str   - input string format  = YYYYMMDD...
 *		
 * OUTPUTS:     julian_str - output string format = DDD
 *
 * RTRN VALUE:  None
 *
 * AUTHOR:      Dennis Irby
 *
 * MODIFICATIONS:
 ******************************************************************************/
void calc_julian_day( pCHAR date_str, pCHAR julian_str )
{
   CHAR  year[5];
   CHAR  month[3];
   CHAR  day[3];
   INT   iYear;
   INT   iMonth;
   INT   iDay;
   INT   julian_day;
   INT   i;
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
}


/******************************************************************************
 *
 *  NAME:         INCOMING_HOST_MESSAGE
 *
 *  DESCRIPTION:  Process a message coming in from the host.  But process
 *                only up to the point where a call is sent to the data server
 *                to get specific information for this host handler.  Once the
 *                call is made to the data server, the function exits.  The
 *                processing will be picked up in 'incoming_host_message_
 *                ncf01_reply'.
 *
 *  INPUTS:       p_msg - This is the incoming message from the host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT incoming_host_message( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   AUTH_TX           auth_tx;
   WORD              timer_length;

   pPTE_MSG          p_msg_timer_out   = NULL_PTR;
   pPTE_MSG          p_msg_chain       = NULL_PTR;
   CHAR              lookup_key[22];
   BOOLEAN           ret_code = false;

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   /* Get the p_msg data from p_msg */
   p_msg_data = ptemsg_get_pte_msg_data( p_msg );  
   
   /* Copy data to ReqRsp_Buffer */
   copy_to_req_resp_buffer( ptemsg_get_pte_msg_data_data( p_msg_data ),
                            ptemsg_get_pte_msg_data_data_length( p_msg_data ) );

   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Parse message from host. */
   if ( false  == cirsutil_parse_host_response_msg() )
      return( false );

   /* Put information from host into auth_tx structure. */
   memset( &auth_tx, 0x00, Auth_Tx_Size );
   move_iso_response_to_auth_struct( &auth_tx );

   /* Set host finish time. */
   ptetime_get_strtime( auth_tx.TLF01_details.host_finish_time );

   /* Check for log on messages. */
   if( 0 == strcmp( auth_tx.TLF01_details.message_type, "0810" ))
   {
      auth_tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;

      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, &auth_tx, NA );

      process_logon_response( &auth_tx );
      return( true );
   }

   /* Received a message from host, so clear then set the idle line timer. */
   (void)clear_echo_timer( ECHO_TIMER_KEY,
                           (pPTE_MSG *)NULL_PTR,
                           application_que_name, 1 );
   timer_length = atoi( ncf01.echo_interval );
   if ( (0 != strcmp(NULL_STR, ncf01.echo_interval)) && timer_length > 0 )
   {
      if ( false == set_echo_timer( ECHO_TIMER_KEY, timer_length,
                                    (pPTE_MSG*)NULL_PTR,
                                    application_que_name, 1 ) )
      {
         cirsutil_error_handler(1, "Unable to clear and set idle line timer.",
                                1, ERROR_MSG, "incoming_host_message", 3,
                                WARNING_ERROR,
                                auth_tx.TLF01_details.terminal_id,
                                auth_tx.TLF01_details.card_num,
                                auth_tx.TLF01_details.merchant_id, "803");
      }
   }

   /* Process Issuer Reversal Advice messages then exit. */
   if ( 0 == strcmp("0422", auth_tx.TLF01_details.message_type) )
   {
      process_issuer_reversals( &auth_tx );
      return( true );
   }

   /* Process Cirrus reconciliation messages then exit. */
   if ( 0 == strncmp("05", auth_tx.TLF01_details.message_type, 2) )
   {
      process_reconciliation( &auth_tx );
      return( true );
   }

   /* Process Cirrus reject messages then exit. */
   if ( 0 == strcmp( "0620", auth_tx.TLF01_details.message_type ) )
   {
      /* Note:  These must be processed prior to leaving the
       * application because global variables are used.  We
       * cannot recover the original message that caused a 0620
       * because no terminal Id is returned with it.
       */
      process_reject( &auth_tx );
      return( true );
   }

   /* Process Cirrus Administrative Advice messages - 0644 */
   if ( 0 == strcmp( "0644", auth_tx.TLF01_details.message_type ) )
   {
      /* Display advice message and then exit. */
      process_admin_advice( &auth_tx );
      return( true );
   }

   /* Process Network Management Advice messages - 0820 */
   if ( 0 == strcmp( "0820", auth_tx.TLF01_details.message_type ) )
   {
      /* Display advice message and then exit. */
      (void)process_network_advice( &auth_tx );
      return( true );
   }

   /* Adjust the counters appropriately for a response from the host. */
   decrement_active_count();
   reset_timeout_count();

   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

   /* If Cirrus sent a logon or key change request, handle it here. */
   if( 0 == strcmp( auth_tx.TLF01_details.message_type, "0800" ) )
   {
      strcpy( auth_tx.TLF01_details.originator_info,
              ptemsg_get_msg_orig_comms_info( p_msg ) );

      strncpy( Logon_NMI_Code, auth_tx.function_code, 3 );
      if ( 0 == strncmp( KEY_CHANGE_REQUEST, Logon_NMI_Code, 3 ) )
         return( process_key_change_request(&auth_tx) );
      else
         return( perform_host_logon_request(NULL_STR, SOURCE_IS_HOST,&auth_tx));
   }
  
   /* Update transaction statistics. */
   auth_tx.TLF01_details.tx_key = 100; /* Fake a response tx_key for stats */
   update_txn_stats( &Txn_Counts, &auth_tx, NA );

   /* Build look up key to stop the host response timer. */
   memset( lookup_key, 0x00, sizeof(lookup_key));
   strcpy( lookup_key, AppName);
   strcat( lookup_key, auth_tx.TLF01_details.terminal_id );
   strcat( lookup_key, auth_tx.TLF01_details.sys_trace_audit_num );

   /* Build PTEMSG to get Timer Data from the Host Data Server.
    * This allows us to get the original auth_tx structure; the
    * one that was built prior to sending it to the host.
    */
   p_msg_timer_out = ptetimer_bld_clear_timer_req( lookup_key,timerds_que_name,
                                                   application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory to build clear timer message",
                             1, ALERT_MSG, "incoming_host_message", 4,
                             FATAL_ERROR, auth_tx.TLF01_details.terminal_id,
                             auth_tx.TLF01_details.card_num,
                             auth_tx.TLF01_details.merchant_id,
                             "407");
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
      p_msg_chain = ptemsg_chain_new_message( p_msg_timer_out,
                                              MT_DB_REQUEST,
                                              ST1_DB_ECHO,
                                              ST2_NONE,
                                              timerds_que_name,
                                              application_que_name,
                                              (pBYTE)&auth_tx,
                                              Auth_Tx_Size,
                                              0 );
      if ( NULL_PTR == p_msg_chain )
      {
         cirsutil_error_handler(1,"Insufficient memory for chain message",
                             1, ALERT_MSG, "incoming_host_message", 4,
                             FATAL_ERROR, auth_tx.TLF01_details.terminal_id,
                             auth_tx.TLF01_details.card_num,
                             auth_tx.TLF01_details.merchant_id,
                             "408");
      }
      else
      {
         /* Send chain of timer data and auth_tx to data server. */
         ret_code = send_msg( p_msg_chain, timerds_que_name, &auth_tx );
         free( p_msg_chain );
      }
   }
   return( (INT)ret_code );
}


/******************************************************************************
 *
 *  NAME:         INCOMING_HOST_MESSAGE_TIMER_REPLY
 *
 *  DESCRIPTION:  Process a reply from the timer.  This function continues
 *                the processing started by 'incoming_host_message_ncf_reply'.
 *                It will complete the processing of a message that was received
 *                from a host.
 *
 *  INPUTS:       p_msg_in - This is the message from the timer.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT incoming_host_message_timer_reply( pPTE_MSG p_msg_in )
{
   pBYTE          p_data            = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
   pPTE_MSG       p_msg_out         = NULL_PTR;
   pPTE_MSG       p_echo_msg        = NULL_PTR;
   LONG           timer_reply_size  = 0;
   LONG           data_len;
   CHAR           app_data1[32];
   CHAR           app_data2[32];
   CHAR           lookup_key[22];
   BYTE           buffer[sizeof(AUTH_TX)];
   BOOLEAN        ret_code = false;
   AUTH_TX        orig_auth_tx;
   AUTH_TX        host_auth_tx;


   /* Get Timer message, including lookup key. */
   timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
                                                app_data2, buffer,
                                                sizeof(buffer) );
   if ( timer_reply_size == Auth_Tx_Size )
   {
      /* Get original auth_tx record prior to going to the host. */
      memset( &orig_auth_tx, 0x00,   Auth_Tx_Size );
      memcpy( &orig_auth_tx, buffer, Auth_Tx_Size );

      if ( 0 == strncmp( KEY_CHANGE_REQUEST, orig_auth_tx.function_code, 3 ) )
      {
         /* We cleared the timer for a Key Change; continue processing it. */
         process_network_advice_timer_reply( &orig_auth_tx );
         return( true );
      }
   }
   else if ( (0 == strcmp( SAF_TIMER_KEY,      lookup_key )) ||
             (0 == strcmp( ECHO_TIMER_KEY,     lookup_key )) ||
             (0 == strcmp( ECHO_REQ_TIMER_KEY, lookup_key )) )
   {
      /* No need to process the results of a clear SAF or ECHO timer. */
      return( true );
   }
   else
   {
      /* We did not get our original data with the timer message. */
      cirsutil_error_handler(1,
                             "Did not receive data with timer reply record.",
                             1, ALERT_MSG, "incoming_host_message", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "483" );
      return( false );
   }

   /* Get auth_tx record that was chained to the timer message.
    * This is the auth_tx record built from response data from
    * the Cirrus host.
    */
   p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
   p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
   p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

   if ( data_len == Auth_Tx_Size )
   {
      /* Get auth_tx record that was built from host response data. */
      memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
      memcpy( &host_auth_tx, p_data, Auth_Tx_Size );
   }
   else
   {
      /* We did not get our original data with the timer message. */
      cirsutil_error_handler(1,
                             "Did not receive second part of chained message.",
                             1, ALERT_MSG, "incoming_host_message", 4,
                             FATAL_ERROR,orig_auth_tx.TLF01_details.terminal_id,
                             orig_auth_tx.TLF01_details.card_num,
                             orig_auth_tx.TLF01_details.merchant_id,
                             "484");
      return( false );
   }

   orig_auth_tx.TLF01_details.tx_key =
           match_request_to_response( orig_auth_tx.TLF01_details.tx_key );

   /* Copy data from the host into the original auth_tx record for
    * the database and for the response to the terminal.
    */
   if ( false == alter_transaction_data( &host_auth_tx, &orig_auth_tx ) )
      return( false );

   /* Add response code text. */
   cirsutil_get_response_text( &orig_auth_tx );

   strcpy( orig_auth_tx.TLF01_details.dispensation.auth_2, "RA" );

   if ( 0 == strcmp( orig_auth_tx.TLF01_details.saf, "Y") )
   {
      /* This is a transaction that was stored and forwarded. */
      if ( false == process_reversal_response( &orig_auth_tx ) )
         return( false );
   }

   /* Send record to data server to track totals for reconciliation. */
   if ( 0 == strcmp( "00", orig_auth_tx.TLF01_details.response_code ) )
   {
      if ( orig_auth_tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY_RESPONSE )
      {
         if( recon_update( &orig_auth_tx ) == false )
         {
            /* Send response message to transaction control. */
            strcpy(orig_auth_tx.TLF01_details.response_code,"96");
            if ( false == send_response_to_term_app( &orig_auth_tx ) )
            {
               cirsutil_error_handler(1,
                                     "Unable to send message",
                                      0, ERROR_MSG, "incoming_host_message", 3,
                                      WARNING_ERROR,
                                      orig_auth_tx.TLF01_details.terminal_id,
                                      orig_auth_tx.TLF01_details.card_num,
                                      orig_auth_tx.TLF01_details.merchant_id,
                                     "409");
               return( false );
            }
         }
      }
      else
      {
         if ( false == send_response_to_term_app( &orig_auth_tx ) )
         {
            cirsutil_error_handler(1,
                                  "Unable to send message",
                                   0, ERROR_MSG, "incoming_host_message", 3,
                                   WARNING_ERROR,
                                   orig_auth_tx.TLF01_details.terminal_id,
                                   orig_auth_tx.TLF01_details.card_num,
                                   orig_auth_tx.TLF01_details.merchant_id,
                                  "409");
            return( false );
         }
      }
   }
   else
   {
      /* Host responded with the response code other than 00.
       * Since this is a failure, no need to update the database.
       * Send response message to transaction control.
       */
      if ( false == send_response_to_term_app( &orig_auth_tx ) )
      {
         cirsutil_error_handler(1,
                               "Unable to send message",
                                0, ERROR_MSG, "incoming_host_message", 3,
                                WARNING_ERROR,
                                orig_auth_tx.TLF01_details.terminal_id,
                                orig_auth_tx.TLF01_details.card_num,
                                orig_auth_tx.TLF01_details.merchant_id,
                               "409");
         return( false );
      }
   }
   return( true );

} /* incoming_host_message_timer_reply */


/******************************************************************************
 *
 *  NAME:         PROCESS_TIMEOUT_MSG
 *
 *  DESCRIPTION:  Process a timeout message. 
 *
 *  INPUTS:       p_msg - This is the incoming message from the timer.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT process_timeout_msg( pPTE_MSG p_msg )
{
   BYTE        buffer2[ sizeof(AUTH_TX) ];
   CHAR        lookup_key[ MAX_TIMER_KEY_SIZE ];
   CHAR        app_data1[ MAX_APP_DATA_SIZE ];
   CHAR        app_data2[ MAX_APP_DATA_SIZE ];
   CHAR        Buff[256];
   pPTE_MSG    p_msg_ins         = NULL_PTR;
   pPTE_MSG    p_msg_timer_out   = NULL_PTR;
   pPTE_MSG    p_msg_update_auth = NULL_PTR;
   BOOLEAN     ret_code = false;
   LONG        timer_reply_size = 0;
   WORD        echo_interval;
   AUTH_TX     auth_tx;


   /* Get timeout values */
   timer_reply_size = ptetimer_get_timer_reply( p_msg, lookup_key,
                                                app_data1, app_data2, buffer2,
                                                sizeof(buffer2) );
   memset( &auth_tx, 0, Auth_Tx_Size );
   if ( timer_reply_size == sizeof(buffer2) )
   {
      memcpy( &auth_tx, buffer2, Auth_Tx_Size );
   }
   /* Store and Forward sets a timer without a buffer.
    * So does echo test and an echo request.
    */
   else if ( (0 != strcmp( SAF_TIMER_KEY,          lookup_key )) &&
             (0 != strcmp( ECHO_TIMER_KEY,         lookup_key )) &&
             (0 != strcmp( ECHO_REQ_TIMER_KEY,     lookup_key )) &&
             (0 != strcmp( LOGON_TIMER_LOOKUP_KEY, lookup_key )) )
	   return(false);

   if ( 0 == strcmp( ECHO_TIMER_KEY, lookup_key ) )
   {
      /* Logon echo timer has expired.  Reset it.  */
      /* Send out a logon echo test message.       */
      /* ----------------------------------------- */
      echo_interval = atoi( ncf01.echo_interval );
      (void)set_echo_timer( ECHO_TIMER_KEY, echo_interval,
                           (pPTE_MSG*)NULL_PTR, application_que_name, 1 );

      memset( &auth_tx, 0x00, Auth_Tx_Size );
      (void)perform_host_logon_request(NULL_STR,SOURCE_IS_STATE,&auth_tx);
      return( true );
   }
   else if ( 0 == strcmp( ECHO_REQ_TIMER_KEY, lookup_key ) )
   {
      /* An Echo request has timed out. Take no action here  */
      /* Allow the echoes to continue going out as normal.   */
      /* --------------------------------------------------- */

      /* Decrement the active transactions counter field */
      decrement_active_count();
      return( true );
   }
   else if ( 0 == strcmp( SAF_TIMER_KEY, lookup_key ) )
   {
      /* Process stored and forward transactions. */
      (void)saf_send_request_for_record();
      return( true );
   }
   else if ( 0 == strcmp( auth_tx.TLF01_details.saf, "Y") )
   {
      strcpy( Buff, "Store and Forward - send to host timed out. Will try again later, Terminal ID: " );
      strcat( Buff, auth_tx.TLF01_details.terminal_id );
   }
   else
   {
      /* Display time out message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants.
       */
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff,
              "Txn to Cirrus timed out. Card %s, Merchant %s",
               auth_tx.TLF01_details.card_num,
               auth_tx.TLF01_details.merchant_id );
   }
   cirsutil_error_handler(1, Buff, 0, INFO_MSG, "process_timeout_message", 0,
                          INFO_ERROR, auth_tx.TLF01_details.terminal_id,
                          auth_tx.TLF01_details.card_num,
                          auth_tx.TLF01_details.merchant_id, "413");

   /* Update the transaction in the auth_tx table to TIMED OUT */
   auth_tx.TLF01_details.general_status = GS_TXN_TIMEDOUT;

   /* Reversal timeout processing */
   if(auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE  ||
      auth_tx.TLF01_details.tx_key == AUTH_REVERSAL           )
   {
      auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;
      if ( 0 == strcmp( auth_tx.TLF01_details.saf, "Y") )
      {
         /* We are in SAF mode and timed out.
          * Attempt SAF again later.
          */

         /* SAF timer has been cleared.  Need to restart it. */
         (void)saf_initiate_timer();
      }
   }

   /* Decrement the active transactions counter field */
   decrement_active_count();

   /* Increment the number of times the host has failed */
   increment_timeout_count();

   /* If in SAF mode, exit here; do not respond to terminal. */
   if ( 0 == strcmp( "Y", auth_tx.TLF01_details.saf ) )
   {
      /* Update transaction statistics. */
      strcpy( auth_tx.TLF01_details.response_code, "19" );
      update_txn_stats( &Txn_Counts, &auth_tx, NA ); /* Increment time out cnt*/

      return( true );
   }

   /* If this is a reversal, must store it for later forwarding. */
   else if ( AUTH_REVERSAL == auth_tx.TLF01_details.tx_key )
      (void)insert_into_saf01( &auth_tx );

   /* Do not perform standin processing for Log On/Off messages. */
   else if ( (AUTH_LOGON          != auth_tx.TLF01_details.tx_key) &&
             (AUTH_LOGON_RESPONSE != auth_tx.TLF01_details.tx_key)  )
   {
      /* Send time out message to terminal handler. */
      perform_standin_processing( "19", GS_TXN_TIMEDOUT, &auth_tx );
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, &auth_tx, NA );

   return( true );
 }  /* process_timeout_msg */


/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_TERM_APP
 *
 *  DESCRIPTION:  This function builds and sends a message to the terminal
 *                handler.  It sends what is in temp_auth_tx.  Typically this
 *                is a response from the Cirrus host.
 *
 *  INPUTS:       p_auth_tx - Transaction record for terminal handler
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_response_to_term_app( pAUTH_TX p_auth_tx )
{
   BOOLEAN     ret_code = false;

   /* Decrement the active transactions counter field */
   decrement_active_count();

   /* This transaction has come back, so the host is responding ,
    * so reset the consective timeouts counter.
    */
   reset_timeout_count();

   /* Send message to the authorizer */
   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE );

   return( (INT)ret_code );
} /* send_response_to_term_app */


/******************************************************************************
 *
 *  NAME:         PROCESS_REJECT
 *
 *  DESCRIPTION:  This procedure is called if Cirrus replies to us
 *                with a reject message, 0620.  Here we print an error
 *                message to include the STAN.
 *
 *                We must reply with a 0630 message.  That is set up here
 *                and sent to the host.
 *
 *                This should cause a time out for the original message
 *                that caused the 0620 ( we can not stop the timer for we do
 *                not have a terminal Id any longer).
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void process_reject( pAUTH_TX p_auth_tx )
{
   char      tempstr[100];
   AUTH_TX   auth_tx;      /* This one will get sent to the host. */

   /*+-------------------------+
     | Send out error message. |
     +-------------------------+*/
   strcpy( tempstr, "** Reject 0620 Err = " );
   strcat( tempstr, reject_advice_code );
   strcat( tempstr, ", Detail Code = " );
   strcat( tempstr, reject_detail_code );
   strcat( tempstr, ", "               );
   strcat( tempstr, reject_text        );

   cirsutil_error_handler(1, tempstr,
                          0, WARN_MSG, "process_reject", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR, "462" );

   /* The STAN is passed to us in 0620 messages. */
   strcpy( tempstr, "System Trace Audit Number of rejected message is: " );
   strcat( tempstr, p_auth_tx->TLF01_details.sys_trace_audit_num );

   cirsutil_error_handler(1, tempstr,
                          0, WARN_MSG, "process_reject", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR, "463" );

   /*+--------------------------------------------+
     | Build a 0630 message to reply to the 0620. |
     +--------------------------------------------+*/
   decrement_active_count();
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   /* Initialize the transaction. */
   (void)cirsutil_init_txn( &auth_tx );

   strncpy( auth_tx.TLF01_details.message_type, "0630", 4 );
   strcpy(auth_tx.TLF01_details.transmission_timestamp,p_auth_tx->TLF01_details.transmission_timestamp);
   
   strncpy( auth_tx.TLF01_details.sys_trace_audit_num,
            p_auth_tx->TLF01_details.sys_trace_audit_num, bf11_len );

   strcpy(auth_tx.TLF01_details.forwarding_institution_id,p_auth_tx->TLF01_details.forwarding_institution_id);

   strncpy( auth_tx.TLF01_details.response_code, "00", 2 );
   
   if( atoi(p_auth_tx->TLF01_details.mcard_banknet_len) == 0)
   {
       strcpy( auth_tx.TLF01_details.mcard_banknet_len,"001" );
   }
   else
       strncpy( auth_tx.TLF01_details.mcard_banknet_len,p_auth_tx->TLF01_details.mcard_banknet_len,sizeof( HOST_MESSAGE.banknet_len ) );
   
   if(p_auth_tx->TLF01_details.mcard_banknet[0]== 0x00)
   {
       //send a 0 if fld63 info is not present
       strcpy( auth_tx.TLF01_details.mcard_banknet,"0" );
   }
   else
       strcpy( auth_tx.TLF01_details.mcard_banknet,p_auth_tx->TLF01_details.mcard_banknet );
   
   
   
   auth_tx.TLF01_details.tx_key = AUTH_ADMIN_ADVICE_RESPONSE;
   strcpy( auth_tx.TLF01_details.originator_queue, dialog_que_name );
   strcpy( auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id );

	/*Use vehicle_number field to save reject reason code and detail code*/
   memcpy( auth_tx.TLF01_details.vehicle_number, reject_advice_code, 3 );
   memcpy( &auth_tx.TLF01_details.vehicle_number[3], reject_detail_code, 4 );

   /*+------------------------+
     | Send 0630 to the host. |
     +------------------------+*/
   if ( false == incoming_other_host_response( &auth_tx ) )
   {
      strcpy( tempstr, "Unable to send 0630 message to Cirrus host." );
      cirsutil_error_handler(1, tempstr,
                             0, WARN_MSG, "process_reject", 3,
                             WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"485");
   }

}  /* process_reject */

/******************************************************************************
 *
 *  NAME:         PROCESS_ADMIN_ADVICE
 *
 *  DESCRIPTION:  This procedure is called when an 0644 message is received
 *                from the host.  This is an Administrative Advice generated
 *                by the host.  This function will display the reason for
 *                the advice from field 60.  No further processing is required.
 *                A response to the host is not expected.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void process_admin_advice( pAUTH_TX p_auth_tx )
{
   char      tempstr[100];

   /*+------------------+
     | Display message. |
     +------------------+*/
   strcpy( tempstr, "** Administrative Advice 0644 Code = " );
   strcat( tempstr, reject_advice_code );
   strcat( tempstr, ", Detail Code = " );
   strcat( tempstr, reject_detail_code );
   strcat( tempstr, ", "               );
   strcat( tempstr, reject_text        );

   cirsutil_error_handler(1, tempstr, 0, WARN_MSG, "process_reject", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR, "532" );

}  /* process_admin_advice */


/******************************************************************************
 *
 *  NAME:         PROCESS_KEY_CHANGE_REQUEST
 *
 *  DESCRIPTION:  This procedure is called when an 0800 message is received
 *                from the host with a Network Management Code of 161.  This
 *                is Cirrus's way of requesting a pin key exchange.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN process_key_change_request( pAUTH_TX p_auth_tx )
{
   key_block_st   key_block;
   BOOLEAN        ret_val = true;

   /* Parse the encryption data block from the host. */
   if ( false == parse_host_key_block( p_auth_tx, &key_block ) )
   {
      process_unsuccessful_key_change( p_auth_tx, "30" );
      ret_val = false;
   }
   else
   {
      /* Get required data for the security application. */
      strcpy( p_auth_tx->TLF01_details.source_key,        key_block.key  );
      strcpy( p_auth_tx->TLF01_details.source_pin_format, ncf01.pin_type );

      /* Send request to the security application. */
      if ( false == security_request( p_auth_tx, MT_NSP_KEYCHANGE ) )
      {
         /* Unable to perform key change operation. */
         process_unsuccessful_key_change( p_auth_tx, "96" );
         ret_val = false;
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         CREATE_KEY_CHANGE_RESPONSE
 *
 *  DESCRIPTION:  This procedure sends a key change response to the host.
 *                The host has requested a key change, and so far there are
 *                no problems preventing it from happening.
 *
 *  INPUTS:       p_auth_tx  - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void create_key_change_response( pAUTH_TX p_auth_tx )
{
   CHAR   tempstr[100];

   /*+--------------------------------------------+
     | Build a 0810 message to reply to the 0800. |
     +--------------------------------------------+*/
   (void)cirsutil_init_txn( p_auth_tx );
   strncpy( p_auth_tx->TLF01_details.message_type, "0810", 4 );
   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   strcpy( p_auth_tx->TLF01_details.originator_queue, dialog_que_name );
   strcpy( p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id );
   strcpy( Stan, p_auth_tx->TLF01_details.sys_trace_audit_num );
   strcpy( Logon_NMI_Code, p_auth_tx->function_code );
   strcpy( p_auth_tx->TLF01_details.response_code, "00" );
   p_auth_tx->TLF01_details.general_status = GS_TXN_COMPLETED;

   /*+------------------------+
     | Send 0810 to the host. |
     +------------------------+*/
   if ( false == set_host_timer( p_auth_tx ) )
   {
      process_unsuccessful_key_change( p_auth_tx, "96" );
   }
   else if ( false == incoming_other_host_response( p_auth_tx ) )
   {
      strcpy( tempstr, "Unable to send 0810 message to Cirrus host." );
      cirsutil_error_handler(1, tempstr,
                             0, WARN_MSG, "create_key_change_response", 3,
                             WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"572");
   }

}  /* create_key_change_response */

/******************************************************************************
 *
 *  NAME:         PROCESS_UNSUCCESSFUL_KEY_CHANGE
 *
 *  DESCRIPTION:  This procedure is called if there is a problem executing
 *                a key exchange request from the host.  This function
 *                responds to the host giving it an appropriate response
 *                code.
 *
 *  INPUTS:       p_auth_tx     - Transaction record from host
 *                response_code - Code indicating reason for failure
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void process_unsuccessful_key_change( pAUTH_TX p_auth_tx, pCHAR response_code )
{
   CHAR   tempstr[100];

   /* Prepare response information depending upon response code. */
   if ( 0 == strcmp( "30", response_code ) )
      strcpy( p_auth_tx->TLF01_details.resp_source,
              "48 - Unable to parse." );

   strcpy( p_auth_tx->TLF01_details.response_code, response_code );

   /*+-----------------+
     | Inform Monitor. |
     +-----------------+*/
   strcpy( tempstr, "ERROR: Cirrus Pin Key Exchange failure: response code = ");
   strcat( tempstr, response_code );

   cirsutil_error_handler(1, tempstr,
                          1, WARN_MSG, "process_unsuccessful_key_change", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR, "569" );

   /*+--------------------------------------------+
     | Build a 0810 message to reply to the 0800. |
     +--------------------------------------------+*/
   (void)cirsutil_init_txn( p_auth_tx );
   strncpy( p_auth_tx->TLF01_details.message_type, "0810", 4 );
   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   strcpy( p_auth_tx->TLF01_details.originator_queue, dialog_que_name );
   strcpy( p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id );
   strcpy( Stan, p_auth_tx->TLF01_details.sys_trace_audit_num );
   strcpy( Logon_NMI_Code, p_auth_tx->function_code );
   p_auth_tx->TLF01_details.general_status = GS_TXN_SYSTEM_ERROR;

   /*+------------------------+
     | Send 0810 to the host. |
     +------------------------+*/
   if ( false == incoming_other_host_response( p_auth_tx ) )
   {
      strcpy( tempstr, "Unable to send 0810 message to Cirrus host." );
      cirsutil_error_handler(1, tempstr,
                             0, WARN_MSG, "process_unsuccessful_key_change", 3,
                             WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"570");
   }

}  /* process_unsuccessful_key_change */

/******************************************************************************
 *
 *  NAME:         PROCESS_NETWORK_ADVICE
 *
 *  DESCRIPTION:  This procedure is called when an 0820 message is received
 *                from the host.  This is a Network Advice generated by the
 *                host.  This function checks the Network Management Info (NMI)
 *                code.  If it is a key change advice, the key change process
 *                continues by clearing the timer, then storing the key into
 *                the database, ncf01.cwk.  If it is not a key change advice,
 *                the NMI is displayed; no further processing is required.
 *                A response to the host is not expected.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   true if success, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BOOLEAN process_network_advice( pAUTH_TX p_auth_tx )
{
   pPTE_MSG  p_msg_timer_out = NULL_PTR;
   CHAR      lookup_key[22];
   BOOLEAN   ret_code = true;
   CHAR      tempstr[100];

   if ( 0 != strncmp( KEY_CHANGE_REQUEST, Logon_NMI_Code, 3 ) )
   {
      /*+------------------+
        | Display message. |
        +------------------+*/
      strcpy( tempstr, "** Cirrus NETWORK ADVICE (0820) - Field 70 code = " );
      strcat( tempstr, Logon_NMI_Code );
      strcat( tempstr, ",  " );
      strcat( tempstr, addl_data );

      cirsutil_error_handler(1, tempstr, 0, INFO_MSG, "process_network_advice",
                             2,INFO_ERROR, NULL_STR, NULL_STR, NULL_STR, "527");
   }
   else
   {
      /*+------------------------------------------------+
        | This is the final leg of a key change request  |
        | from Cirrus. Now we need to clear the timer to |
        | get the new key out of auth_tx and store it    |
        | into the database in NCF01.cwk.                |
        +------------------------------------------------+*/

      /* Build look up key to stop the host response timer. */
      memset( lookup_key, 0x00, sizeof(lookup_key));
      strcpy( lookup_key, AppName);
      strcat( lookup_key, LOGON_TIMER_LOOKUP_KEY );
      memset(tempstr,0,100);

      /* Clear timer or 0810 response to host. */
      ret_code = clear_timer(lookup_key);
   }
   return( ret_code );
}  /* process_network_advice */


/******************************************************************************
 *
 *  NAME:         PROCESS_NETWORK_ADVICE_TIMER_REPLY
 *
 *  DESCRIPTION:  This function updates the NCF01 record with the new pin key.
 *                It is called after an 0820 message is received from the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_network_advice_timer_reply( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   BOOLEAN         ret_code = false;

   /* Store the new key, from Cirrus, through Racal, into the database. */
   strcpy( ncf01.cwk, p_auth_tx->TLF01_details.dest_key );

   /* Build message to get host specific data from data server. */
   memset( buffer1, 0x00,     sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &ncf01, Ncf01_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF01_DATA );
   if ( NULL_PTR == p_msg  )
   {
      cirsutil_error_handler(1, "Insufficient Memory to build NCF01 message",
                             1, ALERT_MSG, "process_network_advice_timer_reply",
                             4, FATAL_ERROR, NULL_STR, NULL_STR,NULL_STR,"575");
   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = send_msg( p_msg, netds_que_name, p_auth_tx );
      free( p_msg );
   }
}


/******************************************************************************
 *
 *  NAME:         PROCESS_RECONCILIATION
 *
 *  DESCRIPTION:  This procedure is called if Cirrus sends us a reconciliation
 *                advice, 0520 or 0522.  We respond with an 053n message.  If
 *                0520, then kick off a message to the data server to initiate
 *                reconciliation of transactions since a previous 0520 message.
 *                That process creates 2 files: a totals file showing ATP's
 *                totals and Cirrus' totals, the second file is a settlement
 *                file of transaction details to support the totals file.
 *
 *                For 0522 advices, we do nothing except respond with an 0532.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_reconciliation( pAUTH_TX p_auth_tx )
{
   CHAR      tempstr[100]="";
   NCF21     ncf21_host_totals;

   (void)cirsutil_init_txn( p_auth_tx );
   memcpy( p_auth_tx->TLF01_details.response_code,   "00", 2 );
   strcpy( p_auth_tx->TLF01_details.originator_queue, dialog_que_name );
   strcpy( p_auth_tx->TLF01_details.tpdu_id,          ncf01.tpdu_id   );

   if ( 0 == strcmp(p_auth_tx->TLF01_details.message_type,"0520") )
   {
      /* This is an Acquirer Reconciliation Advice - 0520. */

      /*+-------------------------------------------------+
        | Inform Monitor about the Reconciliation Advice. |
        +-------------------------------------------------+*/
      strcpy( tempstr, "** Cirrus 0520 received.  Advice Reason Code = " );
      strcat( tempstr, reject_advice_code );
      cirsutil_error_handler(1, tempstr, 0, INFO_MSG,"process_reconciliation",0,
                             INFO_ERROR, NULL_STR, NULL_STR, NULL_STR, "537" );

      /*+--------------------------------------------+
        | Build a 0530 message to reply to the 0520. |
        +--------------------------------------------+*/
      strncpy( p_auth_tx->TLF01_details.message_type, "0530", 4 );
      p_auth_tx->TLF01_details.tx_key = AUTH_RECONCILE_ADVICE_RESPONSE;

      /*+------------------------+
        | Send 0530 to the host. |
        +------------------------+*/
      if ( false == incoming_other_host_response( p_auth_tx ) )
      {
         sprintf( tempstr,
                 "Unable to send 0530 msg to Cirrus host, stan: %s",
                  p_auth_tx->TLF01_details.sys_trace_audit_num );
         cirsutil_error_handler(1, tempstr, 1, WARN_MSG,
                               "process_reconciliation", 0, WARNING_ERROR,
                                p_auth_tx->TLF01_details.terminal_id,
                                NULL_STR, NULL_STR, NULL_STR );
      }
      else
      {
         /*+-----------------------------------------+
           | Send message to data server to initiate |
           | reconciliation processing.              |
           +-----------------------------------------+*/
         memcpy( &ncf21_host_totals, &ncf21, Ncf21_Size );
         (void)recon_initiate( &ncf21_host_totals );
      }
   }
   else
   {
      /* This must be an Issuer Reconciliation Advice - 0522. */
      memcpy( p_auth_tx->TLF01_details.message_type, "0532", 4 );
      p_auth_tx->TLF01_details.tx_key = AUTH_ISSUER_RECONCILE_ADVICE_RESPONSE;

      /*+------------------------+
        | Send 0532 to the host. |
        +------------------------+*/
      if ( false == incoming_other_host_response( p_auth_tx ) )
      {
         sprintf( tempstr,
                 "Unable to send 0532 msg to Cirrus host, stan: %s",
                  p_auth_tx->TLF01_details.sys_trace_audit_num );
         cirsutil_error_handler(1, tempstr, 1, WARN_MSG,
                               "process_reconciliation", 0, WARNING_ERROR,
                                p_auth_tx->TLF01_details.terminal_id,
                                NULL_STR, NULL_STR, NULL_STR );
      }
   }
   return;
}  /* process_reconciliation */

/******************************************************************************
 *
 *  NAME:         ALTER_TRANSACTION_DATA
 *
 *  DESCRIPTION:  This procedure takes care of any special data that goes into
 *                the auth_tx block prior to shipping it back to the terminal.
 *
 *  INPUTS:       p_auth_tx_in  - Transaction record from host
 *
 *  OUTPUTS:      p_auth_tx_out - Transaction record for DB and terminal
 *
 *  RTRN VALUE:   True if all goes as expected
 *                False if tran_type is invalid
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/23/99 - Added host finish time.
 ******************************************************************************/
int alter_transaction_data( pAUTH_TX p_auth_tx_in, pAUTH_TX p_auth_tx_out )
{
   /* Check for a valid transaction. */
   if ( AUTH_INVALID == p_auth_tx_in->TLF01_details.tx_key )
   {
      /* This transaction is not understood by the server and will be dropped */
      cirsutil_error_handler(1, "Invalid transaction key", 0, WARN_MSG,
                            "alter_transaction_data", 3, WARNING_ERROR,
                             p_auth_tx_in->TLF01_details.terminal_id,
                             p_auth_tx_in->TLF01_details.card_num,
                             p_auth_tx_in->TLF01_details.merchant_id, "500" );

	  return( false );
   }

   if ( AUTH_BALANCE_INQUIRY_RESPONSE == p_auth_tx_out->TLF01_details.tx_key )
   {
      /* Replace the amt in the auth_tx table with the amt in the response */
      strcpy( p_auth_tx_out->TLF01_details.total_amount,
              p_auth_tx_in->TLF01_details.total_amount );
   }

   /* Copy balance info for Balance Inquiries and Cash Txns. */
   strcpy(p_auth_tx_out->current_balance,   p_auth_tx_in->current_balance  );
   strcpy(p_auth_tx_out->available_balance, p_auth_tx_in->available_balance);
   p_auth_tx_out->current_balance_sign   =  p_auth_tx_in->current_balance_sign;
   p_auth_tx_out->available_balance_sign =  p_auth_tx_in->available_balance_sign;

   /* Copy balances into these fields; nceqit looks for them here. */
   strcpy(p_auth_tx_out->ACF01_details.cash_available, p_auth_tx_in->ACF01_details.cash_available);
   strcpy(p_auth_tx_out->ACF01_details.cash_limit,     p_auth_tx_in->ACF01_details.cash_limit);

   /* Update the original auth_tx structure. */
   p_auth_tx_out->TLF01_details.general_status = GS_TXN_COMPLETED;

   if ( p_auth_tx_out->TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
      strcpy( p_auth_tx_out->TLF01_details.retrieval_ref_num,
              p_auth_tx_in->TLF01_details.retrieval_ref_num );
   }

   strcpy( p_auth_tx_out->TLF01_details.auth_number,
           p_auth_tx_in->TLF01_details.auth_number );

   strcpy( p_auth_tx_out->TLF01_details.response_code,
           p_auth_tx_in->TLF01_details.response_code );

   if ( 0 != strcmp( p_auth_tx_in->TLF01_details.date_yyyymmdd, NULL_STR ) )
      strcpy( p_auth_tx_out->TLF01_details.date_yyyymmdd,
              p_auth_tx_in->TLF01_details.date_yyyymmdd );

   if ( 0 != strcmp( p_auth_tx_in->TLF01_details.time_hhmmss, NULL_STR ) )
      strcpy( p_auth_tx_out->TLF01_details.time_hhmmss,
              p_auth_tx_in->TLF01_details.time_hhmmss );

   if( false == blank_string( 
                   p_auth_tx_in->TLF01_details.orig_retrieval_ref_num,
                   strlen(p_auth_tx_in->TLF01_details.orig_retrieval_ref_num)))
   {
      strcpy( p_auth_tx_out->TLF01_details.orig_retrieval_ref_num,
              p_auth_tx_in->TLF01_details.orig_retrieval_ref_num );
   }

   if ( 0 != strcmp( p_auth_tx_in->MCF01_details.currency_code, NULL_STR ) )
      strcpy( p_auth_tx_out->MCF01_details.currency_code,
               p_auth_tx_in->MCF01_details.currency_code );

   strcpy( p_auth_tx_out->TLF01_details.resp_source_len,
           p_auth_tx_in->TLF01_details.resp_source_len );

   strcpy( p_auth_tx_out->TLF01_details.resp_source,
           p_auth_tx_in->TLF01_details.resp_source );

   p_auth_tx_out->TLF01_details.cvc = p_auth_tx_in->TLF01_details.cvc;

   strcpy( p_auth_tx_out->TLF01_details.mcard_banknet_len,
           p_auth_tx_in->TLF01_details.mcard_banknet_len );

   strcpy( p_auth_tx_out->TLF01_details.mcard_banknet,
           p_auth_tx_in->TLF01_details.mcard_banknet );

   strcpy( p_auth_tx_out->TLF01_details.agent_id_len,
           p_auth_tx_in->TLF01_details.agent_id_len );

   strcpy( p_auth_tx_out->TLF01_details.agent_id,
           p_auth_tx_in->TLF01_details.agent_id);

   strcpy( p_auth_tx_out->TLF01_details.host_finish_time,
           p_auth_tx_in->TLF01_details.host_finish_time );

   memcpy( p_auth_tx_out->TLF01_details.settlement_date,
           p_auth_tx_in->TLF01_details.settlement_date, 4 );

   if ( p_auth_tx_out->TLF01_details.exp_date[0] == 0x00 )
      memcpy( p_auth_tx_out->TLF01_details.exp_date,
              p_auth_tx_in->TLF01_details.exp_date, 4 );

   return( true );
}  /* alter_transaction_data */
   



/******************************************************************************
 *
 *  NAME:         SEND REQUEST TO HOST
 *
 *  DESCRIPTION:  This procedure creates the ISO message for the Cirrus
 *                host.  It calls the necessary routines to put it together
 *                and send it.
 *
 *  INPUTS:       p_auth_tx - Transaction record for the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if there is a failure
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/19/99 - Created call to 'send_msg_to_authorizer' used
 *                     here for dropped messages.
 *     DI - 03/23/99 - Added start host time.
 ******************************************************************************/
INT send_request_to_host( pAUTH_TX p_auth_tx )
{
   pPTE_MSG p_msg_host_out = NULL_PTR;
   BOOLEAN  ret_code = false;

   /*+------------------------------------------------+
     | Send response to terminal for failed messages. |
     +------------------------------------------------+*/
   if ( GS_TXN_SYSTEM_ERROR == p_auth_tx->TLF01_details.general_status )
   {
      ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE );
      return( (INT)ret_code );
   }

   /* Set host start time with current local time. */
   ptetime_get_strtime( p_auth_tx->TLF01_details.host_start_time );

   /* Build PTEMSG for AUTH_TX table */
   if ( GS_TXN_NEW == p_auth_tx->TLF01_details.general_status )
      p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;


   /*+--------------------+
     | Build Host Message |
     +--------------------+*/
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,current_state);
   if ( GS_TXN_PERF_REQ == p_auth_tx->TLF01_details.general_status )
   {
      build_host_structure( p_auth_tx );
      if ( true == build_host_request( p_auth_tx ) )
      {
      /*+---------------------------+
        | Build Start Timer Message |
        +---------------------------+*/
         if ( false == set_host_timer( p_auth_tx ) )
            return( false );

         increment_active_count();
         p_msg_host_out = ptemsg_build_msg(MT_OUTGOING, 
                                           ST1_NONE,
                                           ST2_NONE,
                                           dialog_que_name,
                                           application_que_name,
                                           ReqRsp_Buffer,
                                           buflen,
                                           0 );
         if ( NULL_PTR == p_msg_host_out )
         {
            cirsutil_error_handler(1,
                                "Insufficient Memory to build host message",
                                1, ALERT_MSG, "send_request_to_host",
                                4, FATAL_ERROR,
                                p_auth_tx->TLF01_details.terminal_id,
                                p_auth_tx->TLF01_details.card_num,
                                p_auth_tx->TLF01_details.merchant_id,
                                "403");
         }
         else
         {
            ptemsg_set_orig_comms_info( p_msg_host_out,
                                        p_auth_tx->TLF01_details.tpdu_id );
            ret_code = send_msg( p_msg_host_out, dialog_que_name, p_auth_tx );
            free( p_msg_host_out );
            if ( ret_code == true )
            {
               /* Update transaction statistics */
               update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

               /* Log transaction to trace file. */
               write_to_txn_file();
            }
         }
      }
      else
      {
         cirsutil_error_handler(1,
                           "Unable to send request to host, tran table error",
                            1, ALERT_MSG, "send_request_to_host",
                            4, FATAL_ERROR,
                            p_auth_tx->TLF01_details.terminal_id,
                            p_auth_tx->TLF01_details.card_num,
                            p_auth_tx->TLF01_details.merchant_id,
                            "410");
      }
   }
   return( (INT)ret_code );
}  /* send_request_to_host */


/******************************************************************************
 *
 *  NAME:         SET_HOST_TIMER
 *
 *  DESCRIPTION:  This procedure builds a timer message and sends it to the
 *                network data server.  This is a timer used to time messages
 *                sent to a host.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if there is a failure
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 04/14/99 - Made modification to check for NULL in request_timer.
 ******************************************************************************/
BOOLEAN set_host_timer( pAUTH_TX p_auth_tx )
{
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   CHAR     lookup_key[22];
   BYTE     buffer1[sizeof(AUTH_TX)];
   BOOLEAN  ret_code = false;
   WORD     timer_length;

   /* Build lookup key. */
   memset( lookup_key, 0x00, sizeof(lookup_key) );
   strcpy( lookup_key, AppName);

   if ( ( AUTH_LOGON          == p_auth_tx->TLF01_details.tx_key ) ||
        ( AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key ) )
   {
      strcat( lookup_key, LOGON_TIMER_LOOKUP_KEY );
   }
   else
   {
      strcat( lookup_key, p_auth_tx->TLF01_details.terminal_id         );
      strcat( lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num );
   }

   /* Send the whole transaction record. */
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );

   /* Build Start Timer message to send to Host Data Server. */
   if ( 0 == strcmp( NULL_STR, ncf01.request_timer ) )
      timer_length = 15;
	  	 
   else
   {
      timer_length = atoi( ncf01.request_timer );
      if ( 0 >= timer_length  ||  1000 < timer_length )
         timer_length = 15;  /* Default value in case garbage is in ncf01 */
   }

   p_msg_set_timer = ptetimer_bld_start_timer_req(
                      lookup_key,
                      p_auth_tx->TLF01_details.primary_key.transaction_id,
                      p_auth_tx->TLF01_details.terminal_id,
                      timerds_que_name,
                      application_que_name,
                      timer_length,
                      0,
                      (pBYTE)&buffer1,  
                      sizeof(buffer1) );

   if ( NULL_PTR == p_msg_set_timer )
   {
      cirsutil_error_handler(1, "Insufficient Memory to build timer message",
                             1, ALERT_MSG, "set_host_timer", 4,
                             FATAL_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "419" );
   }
   else
   {
      timer_length = atoi( ncf01.echo_interval );
      if ( (0 == strcmp(NULL_STR, ncf01.echo_interval)) || timer_length <=0 )
      {
         ret_code = send_msg(p_msg_set_timer, timerds_que_name, p_auth_tx );
         free( p_msg_set_timer );
      }
      else
      {
         if ( false == chain_echo_host( ECHO_TIMER_KEY, timer_length,
                                   application_que_name, p_msg_set_timer ) )
         {
            ret_code = false;
            cirsutil_error_handler(1,
                     "Problem with setting request and idleline timer.",
                      1, ALERT_MSG, "set_host_timer", 4,
                      FATAL_ERROR, p_auth_tx->TLF01_details.terminal_id,
                      p_auth_tx->TLF01_details.card_num,
                      p_auth_tx->TLF01_details.merchant_id, "802" );
         }

         else
            ret_code = true;
      }


   }
   return( ret_code );
}


/******************************************************************************
 *
 *  NAME:         PARSE_HOST_KEY_BLOCK
 *
 *  DESCRIPTION:  This function parses a block of data from the host containing
 *                pin key exchange information.
 *
 *                Here is the expected format:
 *                
 *     FIELD 48 FORMAT:
 *
 *     -------+---------+---------+---------+---------+-----
 *      Total | First   | First   | First   | Second  |
 *       Len  |SubField |SubField |SubField |SubField | . . .
 *            |  Tag    | Length  |  Data   |  Tag    |
 *     -------+---------+---------+---------+---------+-----
 * BYTES: 3       2         2         x         2
 *                
 *        We are looking for TAG '11'.  If tag 11 is the only tag in 
 *        field 48, here is what it would look like:
 *        
 *        0421138PK00nnxxxxxxxxxxxxxxxxyyyyyyyyyyyyyyyy
 *
 *        Total len = 042
 *        Tag       = 11
 *        Length of tag 11 data = 38
 *        Key Class Identifier  = PK (Pin Key Change)
 *        Key Index Number      = 00 (Constant value)
 *        Key Cycle Number      = nn (00 ... 99)
 *        Encrypted Key         = xxxxxxxxxxxxxxxx  (16 alphanumeric characters)
 *        Key Check Value       = yyyyyyyyyyyyyyyy  (16 alphanumeric characters)
 *
 *
 *  INPUTS:       p_auth_tx - Transaction record to be echoed back
 *
 *  OUTPUTS:      key_block - structure containing parsed information
 *
 *  RTRN VALUE:   True if successful, false if there is a failure
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN parse_host_key_block( pAUTH_TX p_auth_tx, p_key_block_st key_block )
{
   INT      field_len;
   INT      total_len;
   INT      key_len = 0;
   INT      i;
   CHAR     tag[3];
   CHAR     temp[3];
   CHAR     block[101];
   CHAR     errbuf[100] = "";
   BOOLEAN  ret_val = false;

   memset( key_block, 0x00, sizeof(key_block_st) );
   total_len = atoi(p_auth_tx->int_net_facility_len);
   memcpy( block, p_auth_tx->int_net_facility_data, 100 );

   /* Look for tag '11' */
   for( i=0; i<100  &&  i<total_len;  )
   {
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &block[i], 2 );
      i += 2;
      strncpy( temp, &block[i], 2 );
      field_len = atoi( temp );
      i += 2;

      if ( 0 == strcmp( "11", tag ) )
      {
         /* Found what we are looking for */
         ret_val = true;

         /* Key Class ID should always be PK = Pin Key. */
         if ( 0 != strncmp( "PK", &block[i], 2 ) )
         {
            ret_val = false;
            memcpy( temp, &block[i], 2 );
            sprintf( errbuf, 
                    "Cirrus Key Change: Invalid Key Class ID (%s), expected PK",
                     temp );
         }

         /* Key Index Number should always be 00. */
         else if ( 0 != strncmp( "00", &block[i+2], 2 ) )
         {
            ret_val = false;
            memcpy( temp, &block[i+2], 2 );
            sprintf( errbuf, 
                    "Cirrus Key Change: Invalid Key Index Nbr (%s), expected 00",
                     temp );
         }

         /* Check the Key Cycle Number. Numeric from 00 to 99. */
         else if ( !isdigit(block[i+4])  ||  !isdigit(block[i+5]) )
         {
            ret_val = false;
            memcpy( temp, &block[i+4], 2 );
            sprintf( errbuf, 
                    "Cirrus Key Change: Key Cycle Nbr (%s) not numeric.",
                     temp );
         }
         else
         {
            /* Everything is good so far. */
            strncpy( key_block->key_cycle_number, &block[i+4 ],  2 );


            /****************************************************
             *       The length of the PEK is related
             *       to the length of the subfield.
             *
             * +------------+-----------------+---------------+
             * | Key Length | Subfield Length | Length of PEK |
             * +------------+-----------------+---------------+
             * |  Single    |       38        |       16      |
             * |  Double    |       54        |       32      |
             * |  Triple    |       70        |       48      |
             * +------------+-----------------+---------------+
             *
             ****************************************************/

            switch( field_len )
            {
               case 38: key_len = 16; break;
               case 54: key_len = 32; break;
               case 70: key_len = 48; break;
            }

            if ( key_len == 0 )
            {
               ret_val = false;
               memset( errbuf, 0x00, sizeof(errbuf) );
               sprintf( errbuf,
                       "Cirrus Key Change: Invalid length of PEK. Tag 11 len=%d",
                        field_len );
            }
            else
            {
               memcpy( key_block->key,             &block[i+6 ],       key_len);
               memcpy( key_block->key_check_value, &block[i+key_len+6],16     );
            }
         }
         break;
      }
      i += field_len;
   }

   /* Log any error messages along with the full content of DE48. */
   if ( ret_val == false )
   {
      if ( errbuf[0] == 0x00 )
      {
         strcpy( errbuf,
                "Cirrus Key Change: Missing Key Change Block Data (Tag 11)");
      }
      cirsutil_error_handler( 1, errbuf, 1, WARN_MSG, "parse_host_key_block",
                              3, WARNING_ERROR, NULL_PTR, NULL_PTR, NULL_PTR,
                              "" );

      /* Display full DE48 */
      sprintf( errbuf, "Data Element 48: %s", block );
      cirsutil_error_handler( 1, errbuf, 1, WARN_MSG, "parse_host_key_block",
                              3, WARNING_ERROR, NULL_PTR, NULL_PTR, NULL_PTR,
                              "" );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         VERIFY_HOST_APP_STATUS
 *
 *  DESCRIPTION:  This procedure will check on all the counters and the state
 *                of the host application to determine whether we can send this
 *                request to the host or not.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT  verify_host_app_status( pAUTH_TX p_auth_tx )
{
   CHAR  err_buf[100]="";
   INT   ret_val;

   /* Get host state and counters */
   ret_val = read_shared_mem_table( net_consec_tmouts_ctr, 
                                    active_txns_ctr, current_state);

   if ( current_state[0] != cONLINE )
      ret_val = false;

   if ( ret_val == true )
   {
      /* Check number of consecutive time outs.
       * If too many, issue a warning.
       */
      if ( MaxConsecTimeouts > 0 )
      {
         if (atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts )
         {
            ret_val = false;
            (void)set_host_state( OFFLINE );

            strcpy( err_buf, "Maximum consecutive timeouts reached.");
            cirsutil_error_handler(1, err_buf,
                             1, INFO_MSG, "verify_host_app_status", 3,
                             INFO_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "423" );
         }
      }

      /* Check number of active transactions.
       * If too many, issue a warning.
       */
      if ( (MaxActiveTxns > 0) && (ret_val == true) )
      {
         if( atoi(active_txns_ctr) >= MaxActiveTxns )
         {
            strcpy( err_buf,
                   "Maximum active txns reached. Continuing to process." );

            cirsutil_error_handler(1, err_buf,
                             0, INFO_MSG, "verify_host_app_status", 2,
                             INFO_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "424" );
         }
      }
   }

   return( ret_val );
}  /* verify_host_app_status */


/******************************************************************************
 *
 *  NAME:         PERFORM_HOST_LOGON_REQUEST
 *
 *  DESCRIPTION:  This procedure will send a logon request to the host to
 *                ensure that the host is up and running.
 *
 *  INPUTS:       source - Indicates who called this function.
 *                         It could be called by:
 *                            Host Handler - Must log on before any transactions
 *                                           can be approved.
 *                            State Timer  - It has been a while since we talked
 *                                           to the host.  Must let it know we
 *                                           are still connected.
 *                            Cirrus Host  - The host can send a logon request
 *                                           in which case we must send an echo.
 *                                           The host can also respond to our
 *                                           logon request with an echo.
 *                            Operator     - A system operator can initiate a
 *                                           log ON / OFF request from Opermon.
 *
 *                request_type - Can be Log Off, Logon, or Echo Request or NULL
 *                p_auth_tx    - Transaction record, may be NULL
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if there is a failure
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/17/99 - Modified SOURCE_IS_HOST to not check current_state, but
 *                     set it to LIVE if not already set.
 ******************************************************************************/
INT perform_host_logon_request( pCHAR request_type, INT source,
                                pAUTH_TX p_auth_tx )
{
   INT   ret_val = true;
   CHAR  ErrMsg[100] = "";
   CHAR  param[6];
   CHAR  time_date[25] = "";

   /* Create a STAN unique for this day; use the local time. */
   ptetime_get_timestamp( time_date );
   get_time( time_date, Stan );

   /* Determine who called this function. */
   read_shared_mem_table(net_consec_tmouts_ctr, active_txns_ctr, current_state);
   switch( source )
   {
      case SOURCE_IS_HOST_HANDLER:

               /* We need to issue a logon request to the Cirrus host. */
               cirsutil_error_handler(1,
                                      "Sending logon request to host.",
                                      0, INFO_MSG, "perform_host_logon_request",
                                      0, INFO_ERROR,
                                      p_auth_tx->TLF01_details.terminal_id,
                                      p_auth_tx->TLF01_details.card_num,
                                      p_auth_tx->TLF01_details.merchant_id,
                                      "438" );
               p_auth_tx->TLF01_details.tx_key         = AUTH_LOGON;
               p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;
               strcpy( p_auth_tx->TLF01_details.processing_code, "920000" );
               strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_ID );
            break;

      case SOURCE_IS_HOST:

               /* The Cirrus host has sent a log on request to us.
                * We must send a response back.
                */
               cirsutil_error_handler(1,
                                   "Received logon request from host.",
                                   0, INFO_MSG, "perform_host_logon_request",
                                   0, INFO_ERROR,
                                   NULL_STR, NULL_STR, NULL_STR, "464" );

               strncpy( p_auth_tx->TLF01_details.message_type, "0810", 4 );
               p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
               strncpy( Stan,
                        p_auth_tx->TLF01_details.sys_trace_audit_num,
                        bf11_len );
               strncpy( p_auth_tx->TLF01_details.response_code, "00", 2 );

               /* Send response to host. */
               perform_op_net_mngmnt_response( Logon_NMI_Code, p_auth_tx );

               /* Update the host state in shared memory. */
               if ( 0 != strcmp( ONLINE, current_state ) )
               {
                  set_host_state( ONLINE );
                  cirsutil_error_handler(1,
                                "Log ON Successful. Setting Host State Online",
                                0, INFO_MSG, "perform_host_logon_request", 0,
                                INFO_ERROR, NULL_STR, NULL_STR,
                                NULL_STR, "490");
               
               }
               break;
               /* end case SOURCE_IS_HOST */

      case SOURCE_IS_OPERATOR:

               /* This is an operator initiated log on or off request. */
               if ( false == perform_op_net_mngmnt_request( request_type,
                                                            p_auth_tx ) )
                  return( false );
               else
                  return( true );
               break;

      case SOURCE_IS_STATE:

               /* This is the result of lack of activity to the Cirrus host.
                * The activity timer has expired.  Need to send Cirrus a
                * message to let them know we are still alive.
                */
               p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
               strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_ID );
               if ( false == perform_op_net_mngmnt_request( ECHO_REQUEST,
                                                            p_auth_tx ) )
                  return( false );
               else
                  return( true );
               break;
               /* end case SOURCE_IS_STATE */

      default:
               strcpy( ErrMsg, "Invalid param in call to function.  Param = ");
               sprintf( param, "%d", source );
               strcat( ErrMsg, param );
               cirsutil_error_handler(1,
                                      ErrMsg,
                                      0, INFO_MSG, "perform_host_logon_request",
                                      0, INFO_ERROR,
                                      p_auth_tx->TLF01_details.terminal_id,
                                      p_auth_tx->TLF01_details.card_num,
                                      p_auth_tx->TLF01_details.merchant_id,
                                      "439" );
            break;
   }
   return( ret_val );
}  /* perform_host_logon_request */


/******************************************************************************
 *
 *  NAME:         send_network_status_to_monitor
 *
 *  DESCRIPTION:  This procedure will send a message to the monitor to inform
 *                it of a change in host status.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful
 *                False if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 *  MODIFICATIONS:
 *     12/21/98 - DI - Fixed memory leak.
 ******************************************************************************/
INT send_network_status_to_monitor(void)
{

   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   BOOLEAN     ret_code = false;
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   XINT        ReturnCode = 0 ;
   int         nIndex = 0 ;
 

   /* Get current state of Cirrus Network Controller. */
   read_shared_mem_table( net_consec_tmouts_ctr,
                          active_txns_ctr, current_state );

   memset( &ncf01, 0x00, Ncf01_Size );
   strcpy( ncf01.primary_key.network_id,    AppName      );
   strcpy( ncf01.primary_key.network_type, "I"           );
   strcpy( ncf01.status,                   current_state );

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
      cirsutil_error_handler(1,
                             "Insufficient Memory to create message",
                             0, INFO_MSG, "send_network_status_to_monitor",
                             0, INFO_ERROR,
                             NULL_PTR, NULL_PTR, NULL_PTR, "512" );
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         /* Send message to get host specific data from data server. */
         ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
         ret_code = send_msg( p_msg_ins, applnk_que_name, (pAUTH_TX)NULL_PTR );
         free( p_msg_ins );
      }
   }
   return ( (INT)ret_code );

}  /* send_network_status_to_monitor */


/******************************************************************************
 *
 *  NAME:         PERFORM_OP_NET_MNGMNT_REQUEST
 *
 *  DESCRIPTION:  This procedure prepares a log on or log off request.
 *
 *  INPUTS:       request_type - Transaction type of request transaction
 *                p_auth_tx    - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful
 *                False if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT perform_op_net_mngmnt_request( pCHAR request_type, pAUTH_TX p_auth_tx )
{
   CHAR      gmt_time[18];
   AUTH_TX   auth_tx;
   pPTE_MSG  p_msg_set_timer = NULL_PTR;
   WORD      timer_length;
   WORD      echo_interval;
   INT       send_echo_request;


   memset( &auth_tx, 0x00, Auth_Tx_Size );
   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
   strncpy(auth_tx.TLF01_details.card_num,p_auth_tx->TLF01_details.card_num,19);
   auth_tx.TLF01_details.tx_key = AUTH_LOGON;

   /* Current Greenwich Mean Time for the transmission-to-host time.
    * Format is YYYYMMDDhhmmss.
    */
   pintime_get_gmt( gmt_time );
   memcpy( auth_tx.TLF01_details.host_start_time, gmt_time, 14 );

   /* Use the time hhmmss for sys_trace_audit_num */
   memcpy( auth_tx.TLF01_details.sys_trace_audit_num, gmt_time, 6 );

   /* Determine if LOG ON or LOG OFF request. */
   if ( 0 == strcmp(request_type, LOGON_REQUEST) )
   {
      strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_ID );
      cirsutil_error_handler(1,
                             "Sending LOGON Request to host.",
                             0, INFO_MSG,
                             "perform_op_net_mngmnt_request",
                             0, INFO_ERROR,
                             NULL_STR, NULL_STR, NULL_STR,
                             "432" );
   }
   else if ( 0 == strcmp(request_type, LOGOFF_REQUEST) )
   {
      strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF );
      cirsutil_error_handler(1,
                             "Sending LOGOFF Request to host.",
                             0, INFO_MSG,
                             "perform_op_net_mngmnt_request",
                             0, INFO_ERROR,
                             NULL_STR, NULL_STR, NULL_STR,
                             "502" );
   }
   else
   {
      /* Log ON ECHO TEST */
      strcpy( Logon_NMI_Code, ECHO_NMI );
      send_echo_request = true;
      (void)read_shared_mem_table( net_consec_tmouts_ctr,
                                   active_txns_ctr, current_state );

      /* Before sending Echo request, make sure
       * a bunch of them have not timed out.
       */
      if ( MaxConsecTimeouts > 0 )
      {
         if ( current_state[0] == cONLINE )
         {
            if ( atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts )
            {
               /* Transaction timeout count is above threshold. Post warning. */
               set_host_state( OFFLINE );
               TxUtils_Send_Msg_To_Operator( 1,
                           "Maximum consecutive timeouts reached",
                            1, WARN_MSG, "perform_op_net_mngmnt_request", 3,
                            WARNING_ERROR, NULL_PTR, ECHO_REQ_TIMER_KEY,
                            NULL_PTR );
            }
         }
         else if ( current_state[0] == cOFFLINE )
         {
            if ( atoi(net_consec_tmouts_ctr) < MaxConsecTimeouts )
            {
               /* We are offline due to a reason other than max time outs.
                * Do not send echo.
                */
               send_echo_request = false;
            }
         }
      }
      else if ( current_state[0] != cONLINE )
      {
         /* We are not online and not counting time outs.  Do not send echo. */
         send_echo_request = false;
      }

      if ( send_echo_request == true )
      {
         /* Set Request Timer for Echoes. */

         /* Build Start Timer message to send to Host Data Server */		
         timer_length = atoi(ncf01.request_timer);
         p_msg_set_timer = ptetimer_bld_start_timer_req( ECHO_REQ_TIMER_KEY,
                            p_auth_tx->TLF01_details.primary_key.transaction_id,
                            p_auth_tx->TLF01_details.terminal_id,
                            timerds_que_name,
                            application_que_name,
                            timer_length,
                            0, "0", 1 ); 

         if ( NULL_PTR == p_msg_set_timer )
         {
            TxUtils_Send_Msg_To_Operator( 1,
                           "Insufficient Memory to build start timer message",
                            1, WARN_MSG, "send_request_to_host", 3,
                            WARNING_ERROR, NULL_PTR, ECHO_REQ_TIMER_KEY,
                            NULL_PTR );
            return(false);
         }

         /* We can chain the request_timer message with set & clear
          * echo timer request to timerds 
          */
         echo_interval = atoi( ncf01.echo_interval );
         if(false == chain_echo_host( ECHO_REQ_TIMER_KEY, echo_interval,
                                      application_que_name, p_msg_set_timer) )
         {
            TxUtils_Send_Msg_To_Operator(1,
            "Error chaining the timer request with clear & set the echo_timer",
             1, ERROR_MSG, "perform_op_net_mngmnt_request", 4, FATAL_ERROR,
             NULL_PTR, ECHO_REQ_TIMER_KEY, NULL_PTR);
         }
      }
      else
      {
         return( false );
      }
   }

   if ( false == cirsutil_send_net_mngmnt_request( &auth_tx ) )
      return( false );
   else
      return( true );

} /* perform_op_net_mngmnt_request */

      

/******************************************************************************
 *
 *  NAME:         PERFORM_OP_NET_MNGMNT_RESPONSE
 *
 *  DESCRIPTION:  This procedure processes a log on or log off response.
 *
 *  INPUTS:       nmi_code  - Indicates if it is a log on, log off, or echo msg
 *                p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful
 *                False if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT perform_op_net_mngmnt_response( pCHAR nmi_code, pAUTH_TX p_auth_tx )
{
   strncpy( p_auth_tx->TLF01_details.tpdu_id,
            ncf01.tpdu_id,
            sizeof(ncf01.tpdu_id) );

   /* Determine type of message to send to OPERMON. */
   if ( 0 == strcmp( nmi_code, LOGON_NMI_GROUP_ID ) )
   {
      cirsutil_error_handler(1,
                             "Sending LOGON Response to host.",
                             0, INFO_MSG,
                             "perform_op_net_mngmnt_response",
                             0, INFO_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "504" );
   }
   else if ( 0 == strcmp( nmi_code, LOGON_NMI_GROUP_LOGOFF ) )
   {
      cirsutil_error_handler(1,
                             "Sending LOGOFF Response to host.",
                             0, INFO_MSG,
                             "perform_op_net_mngmnt_response",
                             0, INFO_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "505" );
   }
   else 
   {
      cirsutil_error_handler(1,
                             "Sending ECHO Response to host.",
                             0, INFO_MSG,
                             "perform_op_net_mngmnt_response",
                             0, INFO_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "506" );
   }

   if ( false == cirsutil_send_net_mngmnt_response( p_auth_tx ) )
      return( false );
   else
      return( true );

} /* perform_op_net_mngmnt_response */


/******************************************************************************
 *
 *  NAME:         MATCH_REQUEST_TO_RESPONSE
 *
 *  DESCRIPTION:  This procedure will return a response transaction type
 *                based on a request transaction type.  For example, a
 *                transaction request of AUTH_SALE will return a response
 *                type of AUTH_SALE_RESPONSE.
 *
 *  INPUTS:       request - Transaction type of request transaction
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Response transaction type
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE match_request_to_response( BYTE request )
{
   /* Make sure this is a request.  If it is not, then it is a response
    * and we need to make sure we return what was sent in.  Otherwise,
    * AUTH_INVALID will be return.  Note: Anything greater than AUTH_INVALID is
    * a response, not a request.
    */
   if ( AUTH_INVALID <= request  )
      return( request );

   /* This is a request, so find the corresponding response key. */
   switch( request )
   {
      case AUTH_SALE:
	     return( AUTH_SALE_RESPONSE );
      break;

      case AUTH_REFUND:
         return( AUTH_REFUND_RESPONSE );
      break;

      case AUTH_CASH_ADVANCE:
         return( AUTH_CASH_ADVANCE_RESPONSE ); 
      break;

      case AUTH_PAYMENT: 
         return( AUTH_PAYMENT_RESPONSE );
      break;

      case AUTH_OFFLINE_SALE: 
         return( AUTH_OFFLINE_SALE_RESPONSE );
      break;

      case AUTH_OFFLINE_REFUND:
         return( AUTH_OFFLINE_REFUND_RESPONSE );
      break;

      case AUTH_SALE_ADJUSTMENT:
         return( AUTH_SALE_ADJUSTMENT_RESPONSE );
      break;

      case AUTH_REFUND_ADJUSTMENT:
         return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
      break;

      case AUTH_PAYMENT_ADJUSTMENT:
         return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
      break;

      case AUTH_PRE_AUTHORIZATION:
         return( AUTH_PRE_AUTHORIZATION_RESPONSE );
      break;

      case AUTH_AUTHORIZATION:
         return( AUTH_AUTHORIZATION_RESPONSE );
      break;

      case AUTH_BALANCE_INQUIRY:
         return( AUTH_BALANCE_INQUIRY_RESPONSE );
      break;

      case AUTH_VOID_SALE:
         return( AUTH_VOID_SALE_RESPONSE );
      break;

      case AUTH_VOID_REFUND: 
         return( AUTH_VOID_REFUND_RESPONSE );
      break;

      case AUTH_REVERSAL:
      case AUTH_REVERSAL_ADVICE:
         return( AUTH_REVERSAL_RESPONSE );
      break;

      case AUTH_RECONCILE_ADVICE: 
         return( AUTH_RECONCILE_ADVICE_RESPONSE );
      break;

      case AUTH_ADMIN_ADVICE: 
         return( AUTH_ADMIN_ADVICE_RESPONSE );
      break;

      case AUTH_LOGON:
         return( AUTH_LOGON_RESPONSE );
      break;

      case AUTH_TEST:
         return( AUTH_TEST_RESPONSE );
      break;

      default:
         return( request+100);
      break;
   }
}  /* match_request_to_response */


/******************************************************************************
 *
 *  NAME:         PERFORM_ERROR_PROCESSING
 *
 *  DESCRIPTION:  This procedure takes care of transactions that were unable
 *                to get sent to the host due to some type of error.  The
 *                auth_tx structure gets updated in here and then the function
 *                returns.
 *
 *  INPUTS:       p_auth_tx_in - Structure containing the txn record
 *
 *  OUTPUTS:      p_auth_tx_in - Structure containing the updated txn record
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 04/09/99 - Changed the format of RRN.
 ******************************************************************************/
void perform_error_processing( pAUTH_TX p_auth_tx_in, pCHAR resp_code )   
{
   CHAR       date_str [ 9] = "";
   CHAR       time_str [ 7] = "";
   CHAR       time_date[25] = "";


   /* Get local time. */
   ptetime_get_timestamp( time_date );
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
   if ( 0 == strcmp( NULL_STR, p_auth_tx_in->TLF01_details.retrieval_ref_num ) )
   {
      create_rrn( p_auth_tx_in->TLF01_details.retrieval_ref_num );
   }

   /* Create response code and store it in the auth_tx block */
   strcpy( p_auth_tx_in->TLF01_details.response_code, resp_code );

   /* Create date of transaction and store it in the auth_tx table,
    * format "YYYY0M0D".
    */
   strcpy( p_auth_tx_in->TLF01_details.date_yyyymmdd, date_str );

   /* Create time of transaction and store it in the auth_tx table,"0H240I0S" */
   strcpy( p_auth_tx_in->TLF01_details.time_hhmmss, time_str );

   p_auth_tx_in->TLF01_details.general_status = GS_TXN_SYSTEM_ERROR;

   send_it_back_to_terminal( resp_code, p_auth_tx_in );
     
}  /* perform_error_processing */


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
 *  MODIFICATIONS:
 ******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
}
   

/******************************************************************************
 *
 *  NAME:         GET_TIME
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the time portion into an output string,
 *                formatted slightly different.
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 *  OUTPUTS:      time_str  : Format = "0H0I0S"
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void get_time( pCHAR time_date, pCHAR time_str )
{
   memset ( time_str,  0x00,          7 );
   strncpy( time_str, &time_date[11], 2 );
   strncat( time_str, &time_date[14], 2 );
   strncat( time_str, &time_date[17], 2 );
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG
 *
 *  DESCRIPTION:  This function sends a PTE message to a specified queue.
 *
 *  INPUTS:       p_msg     - Pointer to a PTE_MSG structure
 *                que_name  - Name of the destination queue
 *                p_auth_tx - Transaction structure used here for error msgs.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN send_msg( pPTE_MSG p_msg, pCHAR que_name, pAUTH_TX p_auth_tx )
{
   BOOLEAN  ret_val = true;
   LONG     ret_code;
   CHAR     Buff[256];
   CHAR     err_mesg[200]   = "";
   BYTE     terminal_id[9]  = "";
   BYTE     card_num[20]    = "";
   BYTE     merchant_id[16] = "";

   ret_code = pteipc_send( p_msg, que_name );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg );
      sprintf(Buff, "pteipc error sending to que: %s. %s", que_name, err_mesg );

      if ( NULL_PTR != p_auth_tx )
      {
         strcpy( terminal_id, p_auth_tx->TLF01_details.terminal_id );
         strcpy( card_num,    p_auth_tx->TLF01_details.card_num    );
         strcpy( merchant_id, p_auth_tx->TLF01_details.merchant_id );
      }
      cirsutil_error_handler(1, Buff, 0, WARN_MSG, "send_msg",
                             1, WARNING_ERROR, terminal_id,
                             card_num, merchant_id, "530");
      ret_val = false;
   }
   return( ret_val );

}

/******************************************************************************
 *
 *  NAME:         SEND_MSG_TO_AUTHORIZER
 *
 *  DESCRIPTION:  This function builds and send a PTE message to the
 *                authorizer queue using the AUTH_TX structure.  It sends
 *                a message type of MT_AUTH_RESPONSE or MT_AUTH_REQUEST.
 *
 *  INPUTS:       p_auth_tx - Transaction structure
 *                msg_type  - MT_AUTH_RESPONSE or MT_AUTH_REQUEST
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN send_msg_to_authorizer( pAUTH_TX p_auth_tx, BYTE msg_type )
{
   pPTE_MSG   p_msg    = NULL_PTR;
   BOOLEAN    ret_code = false;

   p_msg = ptemsg_build_msg( msg_type, 
                             ST1_NONE,
                             ST2_NONE,
                             authorizer_que_name,
                             application_que_name,
                             (pBYTE)p_auth_tx,
                             Auth_Tx_Size,
                             0 );
   if ( NULL_PTR == p_msg )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory for auth response message",
                             1, ALERT_MSG, "send_msg_to_authorizer", 4,
                             FATAL_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "411");
   }
   else
   {
      ret_code = send_msg( p_msg, authorizer_que_name, p_auth_tx );
      free( p_msg );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         CLEAR_TIMER
 *
 *  DESCRIPTION:  This function clears the host state timer using a lookup
 *                key passed into it.
 *
 *  INPUTS:       lookup_key - Key to indicate which timer to clear
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN clear_timer( pCHAR lookup_key )
{
   pPTE_MSG   p_msg    = NULL_PTR;
   BOOLEAN    ret_code = false;

   /* Build PTEMSG to clear timer */
   p_msg = ptetimer_bld_clear_timer_req( lookup_key,timerds_que_name,
                                         application_que_name );
   if ( NULL_PTR == p_msg )
   { 
      cirsutil_error_handler(1,
                             "Insufficient Memory to build clear timer message",
                             1, ALERT_MSG, "clear_timer", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "421" );
   }
   else
   {
      /* Clear Timer message  */
      ret_code = send_msg( p_msg, timerds_que_name, (pAUTH_TX)NULL_PTR );
      free( p_msg );
   }
   return( ret_code );
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
 *  MODIFICATIONS:
 ******************************************************************************/
void create_rrn( pCHAR rrn )
{
   CHAR    time_date[25];
   CHAR    date_str[9];
   CHAR    time_str[7];
   CHAR    julian_day[4];

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Change date to julian format - DDD */
   calc_julian_day( date_str, julian_day );

   /* Format RRN: "YDDDhmmssjjj" */
   sprintf( rrn, "%1.1s%3.3s%5.5s%3.3s",time_date+3, julian_day,
                time_str+1, time_date+20 );
}

/******************************************************************************
 *
 *  NAME:         SEND_TRANSACTION_STATISTICS
 *
 *  DESCRIPTION:  This function will send transaction statistics to Monitor.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to send stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100];

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

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
   if ( false == ret_code  )
   {
      TxUtils_Send_Msg_To_Operator(1, err_msg, 1, WARN_MSG,
                                  "send_transaction_statistics", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR);
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         RESET_TRANSACTION_STATISTICS
 *
 *  DESCRIPTION:  This function will set the transaction statistics to zero.
 *                This is the number of transactions received from the host,
 *                and the number of transactions this network controller has
 *                sent to the host.  These include number of approved, timed
 *                out, reversed, and declined txns.  Then this function sends
 *                these newly initialized values to the Monitor.
 *
 *                NOTE: The message being sent to the originator is a response
 *                to a get stats message.  This is the correct response to a
 *                reset stats message.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to reset stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT reset_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100];

   /* Reset the statistics */
   (void)reset_stats( &Txn_Counts );

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

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
   if ( false == ret_code  )
   {
      TxUtils_Send_Msg_To_Operator(1, err_msg, 1, WARN_MSG,
                                  "reset_transaction_statistics", 3,
                                   WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR);
   }
   return( ret_code );
}

