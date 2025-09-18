/******************************************************************************

 	MCRDTRAN.C

   Copyright (c) 2007, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  MasterCard Host Handler Transaction Processor

   This module is a part of the MasterCard Host Handler.  It will parse and
   manipulate all incoming messages from the terminal and from the host.

   APPLICATION:  Advanced Transaction Processor (ATP)

   ----------------
   REVISION HISTORY
   ----------------

   Rev 1.118   Nov 11 2009   TF-Ajay 
    Added code to not handle void and reversal txn here.
	it has to forward to host 
	Rel 09-76
	
	  
		Rev 1.117   Jul 13 2009   TF-Ajay 
    Added code to copy the STIP txn status from ncf30 
	

      Rev 1.116   May 01 2006 16:58:04   dirby
   Modified to store EMV response info into the EMV structure's
   future fields to pass to the device controller without causing an
   overflow when there is too much request and response info.
   SCR 21062
   
      Rev 1.115   Jul 28 2005 16:00:56   dirby
   Added code in alter_transaction_data for the emv_block.  Modified
   it to put in the response emv block with the request emv block
   appended.  This way, all of the emv block is logged into the database.
   SCR 16652
   
      Rev 1.114   Jul 14 2005 13:30:42   dirby
   Modified to not copy the emv data block from the host response
   message into the original Auth Tx if it is empty.
   SCR 15918
   
      Rev 1.113   Jun 17 2005 11:38:28   chuang
   Version 4.1.1.4, log reject reason code and detail code into TLF01.vehicle_number.
   
      Rev 1.112   Jun 10 2005 16:57:54   dirby
   Modified to copy response EMV data into the request Auth Tx so
   that the response information will get back to the terminal.
   SCR 15725
   
      Rev 1.111   May 17 2005 17:52:48   dirby
   Modified to stop standing in for Auth and Cash Overrides.
   SCR 15420
   
      Rev 1.110   Mar 17 2005 15:10:40   dirby
   Modified to send EMV block from MC to the terminal.
   SCR 14677
   
      Rev 1.109   Feb 11 2005 17:32:50   dirby
   Modified to copy decline/failure messages into TLF01's response text.
   SCR 12340
   
      Rev 1.108   Oct 20 2004 15:14:00   dirby
   Modified to not set response text unless txn is a Voice auth.  This
   is because the response text goes into the Voice Auth Text field.
   But this field is now reserved for CAVV info, which is non-existent
   in Voice Auth transactions.
   SCR 12481
   
      Rev 1.107   Aug 23 2004 12:29:40   dirby
   Modified to perform pin translation for pin-txns not going to Cirrus.
   SCR 1469
   
      Rev 1.106   Aug 17 2004 11:46:00   dirby
   Changed criteria for routing to Cirrus.  Used to be if a pin existed,
   route to Cirrus.  But now, only route ATMs to Cirrus.
   SCR 1443
   
      Rev 1.105   Jun 08 2004 13:04:36   dirby
   Added a log message when host is inactive.
   Release 4.3.0-1
   
      Rev 1.104   Oct 10 2003 09:14:40   dirby
   Modified 0302 processing; changed number of regions from
   INT to CHAR.  This is to avoid conversion of integers from Windows NT
   to AIX.  This value, set as (for example) 2 by the GUI, but ncmcrd
   would translate it as 100662296.  Not sure why, but it doesn't
   need to be a number, so this change works.
   SCR 1224
   
      Rev 1.103   Oct 09 2003 13:55:50   dirby
   Modified to make sure the following fields get logged into TLF01:
      Stop Regions
      Workstation
      Operator ID
      Terminal ID
      Reason Code
   
      Rev 1.102   Jun 03 2003 14:35:58   dirby
   Removed a line that was setting response code to 05 when
   CVC1 failed.  Need to just pass the Issuer's response through.
   SCR 1134
   
      Rev 1.101   Mar 03 2003 11:21:22   dirby
   Modified code for 0302/0312 transactions to handle response
   information in a more useful and operator friendly form.
   SCR 891
   
      Rev 1.100   Feb 07 2003 14:25:08   dirby
   Modified function move_iso_response_to_auth_struct to return
   an error code.  And if an error is encountered, call new function
   reject_switched_in_txn to send a decline back the host.
   SCR 1000
   
      Rev 1.99   Jul 29 2002 17:39:40   dirby
   Added a couple of lines of code to copy the workstation and
   operator id into the host-TLF01 structure so they get stored into TLF01.
   SCR 575
   
      Rev 1.98   Jul 29 2002 17:16:06   dirby
   Added Workstation ID and Operator ID to TLF01 for 0302 txns.
   SCR 757
   
      Rev 1.97   Jul 12 2002 12:42:34   dirby
   Corrected bug where function 'process_timeout_msg' would
   return NOT_OK under normal circumstances.  Modified it to
   return OK.
   SCR 594
   
      Rev 1.96   Jul 07 2002 03:10:12   dirby
   Fixed bug - return false if host is not online when checking its status.
   SCR 594
   
      Rev 1.95   Jul 07 2002 01:37:44   dirby
   1.  Reimplemented going offline when max times occur.
   2.  Implemented use of precalculated sizeof values instead of
   recalculating structure sizes over and over and over.
   SCR 594
   
      Rev 1.94   Jul 06 2002 23:30:38   dirby
   1.  Added code to display a message for time outs and reversals.   SCR 621
   2.  Added function 'txn_is_blocked'.  This determines if a txn from Visa is not permitted.  SCR 703
   
      Rev 1.93   Mar 05 2002 15:43:18   dirby
   Modified the MCC validation to be set either on or off, based on
   a flag set at startup.  Right now, Equitable wants it off.  Do not
   validate MCC for switched in transactions.
   SCR 648
   
      Rev 1.92   Jan 23 2002 14:44:04   dirby
   Initialized buffers in transaction statistic functions.
   
      Rev 1.91   Jan 17 2002 11:12:10   dirby
   Removed code that cleared the timer after a time out.  The
   time out automatically clears the timer.   SCR 565
   
      Rev 1.90   Jan 16 2002 16:06:14   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.89   Jan 14 2002 18:20:24   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.88   Dec 10 2001 16:53:12   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.87   Nov 15 2001 17:08:16   dirby
   Added code to copy response_text field from host txn into the
   original auth_tx.  This is for DE48, subfield 84 - Merchant
   Advice codes.     SCR 531
   
      Rev 1.86   Oct 18 2000 14:56:52   dirby
   Made the code for tran and host start and finish times more efficient.
   
      Rev 1.85   Sep 25 2000 09:56:14   dirby
   1.  Reworked some indentation issues.
   2.  Added code to check results of CVC validation and to set RC accordingly.
   3.  Removed some unnecessary code from 'alter_transaction_data'.
   
      Rev 1.84   Aug 28 2000 09:53:14   dirby
   Type cast some counts to unsigned long.
   
      Rev 1.83   Aug 28 2000 09:45:56   dirby
   Changed the MONITOR_STATS structure from 'long's to 'char's.
   Unix seems to translate longs differently than NT does.
   
   
      Rev 1.82   Aug 21 2000 15:21:06   dirby
   Added code to support transaction statistics for Monitor.
   
      Rev 1.81   Aug 11 2000 16:28:40   dirby
   Modified to respond with "01" when host is unavailable and to not
   issue a logon.
   
      Rev 1.80   Jul 31 2000 15:31:00   dirby
   Modified the lookup keys for records in NCF30.
   
      Rev 1.79   Jul 13 2000 16:16:30   dirby
   Modified the standin processing function to not
   overwrite the auth_number with the time if the auth
   number already exists.
   
      Rev 1.78   Jun 22 2000 15:43:14   dirby
   1.  Added functions 'map_0302_to_tlf01' and map_0312_to_tlf01'.  These are
   for logging an 0302 request to TLF01.
   2.  Changed the lookup key to get original txns for 0420s from auth_number to
   retrieval_ref_num.  If a txn did not reach the acquiring side, the auth_number
   may not exist.
   
      Rev 1.77   Jun 20 2000 14:21:00   dirby
   Added 'auth_number' as a search criteria for reversals when
   subtype 2 is ST2_DB_SELECT_TRAN_BY_CARD_NUM.
   
   
      Rev 1.76   Jun 20 2000 12:28:32   dirby
   Made some duplicate message IDs unique.
   
      Rev 1.75   Jun 15 2000 16:41:52   dirby
   Added code to populate the reversal_amount field
   in TLF01 for reversals and to initialize switched in
   reversal transactions.
   
      Rev 1.74   Jun 14 2000 13:03:06   dirby
   Changed response code for invalid MCC when no track data
   from DO_NOT_HONOR (05) to CALL_ISSUER (01).
   
      Rev 1.73   Jun 13 2000 17:02:18   dirby
   Added code to validate MCC if there is no track data.
   Changed hardcoded response codes to macros.
   Modified code so fields 7 & 15 are echoed properly for reversals.
   
      Rev 1.72   Mar 21 2000 17:34:10   ddabberu
   update database for 00/55 response code
   
      Rev 1.71   Feb 16 2000 10:44:14   svelaga
   Fixed all the SCRs on File update 302/312 transactions.
   
   Incorporated the design changes to maintian
   the transaction integrity as well as database integrity.
   
      Rev 1.70   Jan 25 2000 10:16:20   dirby
   Added standin processing for some void transactions.
   
      Rev 1.69   Jan 18 2000 11:20:16   ddabberu
   match_request_to_response() modified
   
   
      Rev 1.68   Jan 14 2000 15:17:30   dirby
   Set default to the original tx_key in the function that converts a tx_key
   from a request to a response.
   
      Rev 1.67   Jan 13 2000 15:53:28   dirby
   Added a check in 'send_response_to_GUI' for errors.
   Set response code to approved for errors, and copy
   error message into 'inquiry_record' so the GUI will
   display it.
   
      Rev 1.66   Jan 12 2000 16:08:06   dirby
   Added auth_tx as a parameter to 'send_response_to_GUI'.
   This is to use the originator information from auth_tx.
   
      Rev 1.65   Jan 04 2000 15:45:00   svelaga
   Modifications : for processing auth advices.
   
      Rev 1.64   Dec 30 1999 15:26:36   dirby
   Modified to update the transaction control database tables
   when a switched in authorization is successfully completed.
   
      Rev 1.63   Dec 29 1999 14:43:48   dirby
   Corrected a bug: was copying the wrong information into
   the transaction fee amount from NCF30 into auth_tx.
   
      Rev 1.62   Dec 29 1999 09:09:42   svelaga
   Changed the code for finding the original transaction from
   NCF30.
   
      Rev 1.61   Dec 27 1999 13:19:22   svelaga
   Changed from auth_tx to p_auth_tx at some places.
    
   
      Rev 1.60   Dec 22 1999 14:02:40   dirby
   Modified to separate advice processing from reversal processing.
   
      Rev 1.59   Dec 17 1999 14:12:06   dirby
   Added code to use auth_tx.acct_id_1 to store the app_data_type
   on an 0302 request from GUI.  This gets echoed back to the GUI
   in the response.
   
      Rev 1.58   Dec 14 1999 10:20:00   dirby
   Modified to not look for a specific lookup key
   for logon messages.  Use appname, terminal
   ID (which will be null), and stan, just like normal
   messages.
   
      Rev 1.57   Dec 11 1999 11:43:10   ddabberu
   Fixed bugs during integration testing.

      Rev 1.56   Nov 24 1999 16:49:54   svelaga
   Fixed a small bug.
   else if condition to match '0800' message.
   
      Rev 1.55   Nov 18 1999 11:42:10   svelaga
   Modified to accomodate the new reversal logic for
   switched-in transactions.
   
      Rev 1.54   Nov 05 1999 14:57:30   dirby
   Added code to make use of a control flag that either allows or disallows
   negative file processing.
   
      Rev 1.53   Nov 05 1999 12:50:52   dirby
   Modified to enhance ncmcrd by making it more modular.
   
      Rev 1.52   26 Aug 1999 12:19:40   dirby
   Added code to get original transaction when a reversal advice comes in
   from MasterCard; insert reversal into TLF01; and delete original txn from NCF30.
   
      Rev 1.51   26 Aug 1999 09:46:08   dirby
   Changed timer messages to go to timer data server.
   
      Rev 1.50   10 Aug 1999 16:29:40   dirby
   Added subtype_2 and network_type as input parameters to recon_update.
   
      Rev 1.49   09 Aug 1999 14:23:02   dirby
   Changed 'recon_update' to create RRN, date, and time if they do not
   exist.  Also changed it to prevent writing out of bounds to network_data;
   the size in auth_tx is greater the in NCF30 and the array was being
   written past the end of the variable length.
   
      Rev 1.48   03 Aug 1999 18:31:14   dirby
   Removed some reversal processing for switched out transactions.
   The previous change makes this code obsolete.
   
      Rev 1.47   03 Aug 1999 17:53:12   dirby
   Added check of transactions coming from TXCNTL (switched out) for reversals.
   If it is a reversal, store into TLF01 and exit.  Ncmcrd does not send reversals to host.
   
      Rev 1.46   02 Aug 1999 14:32:46   dirby
   Changed application update_recon to use 'AppName' for the
   network_id instead of 'issuer_id'.  This is because, issuer_id
   may not always be for both switched in and switched out txns.
   
      Rev 1.45   02 Aug 1999 10:48:08   dirby
   Added code to insert transaction into NCF30, updating NCF21
   when the response to a switched out transaction is sent to
   txcntl.
   
      Rev 1.44   02 Aug 1999 10:14:14   dirby
   Reworked function 'incoming_other_host_response' to
   update NCF21/NCF30 prior to sending out the response
   to the host.
   
      Rev 1.43   Jun 24 1999 14:55:50   egable
   Change the response code from "TO" to "19" for timeouts.
   
      Rev 1.42   18 Jun 1999 16:26:22   dirby
   Added code to route debit transactions to Cirrus.
   
      Rev 1.41   16 Jun 1999 15:54:40   dirby
   Modified to preserve original request message type
   when sending response to tran control for switched out txns.
   
      Rev 1.40   Jun 11 1999 16:40:24   svelaga
   bug fix : 08xx messages received from the master card network are logged into TLF01
   
      Rev 1.39   04 Jun 1999 16:46:26   dirby
    
   
      Rev 1.38   28 May 1999 17:44:24   dirby
   Added functionality to fill the response text field in auth_tx.
   
      Rev 1.37   28 May 1999 15:43:06   dirby
   1.  Corrected bug on reversal advise function.  Sent wrong structure to authorizer.
   2.  In Admin Advice processing, added code to echo fields 7, 33, and 62.
   
      Rev 1.36   27 May 1999 15:07:24   dirby
   Added Store and Forward (SAF) request processing.
   Added code to ignore reversal responses from the authorizer.  When we get
   an 0420 from the host, we immediately resopnd with an 0430.  Then we ship
   an 0400 to the authorizer.  Need to ignore its response, else we send two responses
   to the host.
   
      Rev 1.35   24 May 1999 14:58:56   dirby
   Added function 'perform_data_checks' and a call
   to it prior to sending a request to the host.  This
   function currently just verifies a Merchant Category
   Code (field 18) exists.  If a 0100  transaction goes out without
   it, MasterCard may charge $2,500 dollars.
   
      Rev 1.34   23 May 1999 13:47:40   dirby
   Added trantype CASH ADVANCED AUTHORIZATION.
   
      Rev 1.33   22 May 1999 15:09:16   dirby
   Moved the code that saves the FWD INST ID Code for a msg from host.
   
      Rev 1.32   12 May 1999 16:28:58   dirby
   Made modifications to fix bugs in creating 0430 messages.
   
      Rev 1.31   10 May 1999 16:20:34   dirby
   Added code to handle terminal Ids that are less than
   8 characters long.  This included modifying the way
   a timer lookup key is created.
   
      Rev 1.30   30 Apr 1999 14:35:08   dirby
   Changed issuer_id from 'cur_app_name' to 'AppName'.
   
      Rev 1.29   21 Apr 1999 12:48:42   dirby
   1.  Added function 'process_0190'.
   2.  Added settlement_date and exp_date to auth responses.
   
      Rev 1.28   Apr 21 1999 01:18:56   egable
   Correct RRN
   
      Rev 1.27   19 Apr 1999 16:47:28   dirby
   Added a line of code to flag the host link as down when
   an echo test fails.
   
      Rev 1.26   16 Apr 1999 16:18:46   dirby
   Added function 'create_rrn' which will create a retrieval
   reference number that meets 3 special criteria:
   1.  Be numeric
   2.  Be unique
   3.  First 4 characters are YDDD - last digit of year + julian day.
   
      Rev 1.25   14 Apr 1999 16:59:42   dirby
   Added offline OVERRIDE transaction processing for voice
   authorization overrides.
   
      Rev 1.24   14 Apr 1999 11:05:30   dirby
   Added check to set request_timer to the default value of 15
   if the value in NCF01 is NULL.
   
      Rev 1.23   13 Apr 1999 16:39:40   dirby
   Added check to not send a timeout response to txcntl if
   message type is 08xx.
   
      Rev 1.21   10 Apr 1999 15:50:42   dirby
   In 'incoming_host_message', change code that saves forwarding_inst_id_code
   to only save it if it does not already exist in auth_tx.
   
      Rev 1.20   09 Apr 1999 15:45:02   dirby
   Changed the format of RRN when creating it.
   
      Rev 1.19   07 Apr 1999 15:22:04   dirby
   Added code to send valid response to txn originator
   for reversals, then drop the transaction.  Removed
   code that ignored reversals.
   
      Rev 1.18   Apr 05 1999 15:09:28   egable
   Correct errors with logon and logoff
   
   
      Rev 1.17   Mar 31 1999 15:37:02   egable
   Change send_status_to_network to update the
   'A' and 'I' items.
   
   
      Rev 1.16   Mar 30 1999 14:19:42   egable
   Only set the timer if the host message is
   built successfully.  Error report #438.
   
      Rev 1.15   26 Mar 1999 16:31:30   dirby
   Added Reversal Advice (0420) processing.
   
      Rev 1.14   23 Mar 1999 16:35:12   dirby
   Reworked some of the shared memory counters.
   
      Rev 1.13   23 Mar 1999 14:32:14   dirby
   Fixed minor bug when checking for issuer or acquirer.
   
      Rev 1.12   23 Mar 1999 10:13:42   dirby
   Added code to fill fields:
      acquirer_id and issuer_id
      host_start_time and host_finish_time
      tran_start_time.
   
      Rev 1.11   22 Mar 1999 15:55:16   dirby
   Fixed bug in 'process_logon_response'.  I was checking twice for
   log offs, and never for log ons.
   
      Rev 1.10   19 Mar 1999 17:12:46   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.9   18 Mar 1999 11:50:36   dirby
   Created a separate function to send PTE messages
   to a destination queue.  This eliminates a lot of very
   repetitive code, especially the error handling portion.
   The new function's name is 'send_msg'.
   
      Rev 1.8   17 Mar 1999 16:27:28   dirby
    
   
      Rev 1.7   17 Mar 1999 10:02:00   dirby
   Modified 'perform_host_logon_request' SOURCE_IS_HOST to respond
   to the host regardless of what current_state is.  Then set current_state
   to zero (logged on) if not already set.
   
      Rev 1.6   16 Mar 1999 15:38:58   dirby
   Changed network_type to BYTE in get_ncf01( ).
   
      Rev 1.5   16 Mar 1999 13:52:36   dirby
   Fixed bugs for the 0302 message processing (Card File Updates).
   
      Rev 1.4   12 Mar 1999 09:35:46   dirby
   Created a separate function to get NCF01 records.
   
      Rev 1.3   04 Mar 1999 16:57:52   dirby
   Updated for AIX port.
   
      Rev 1.2   26 Feb 1999 16:33:28   dirby
   Modified call to get original txn for reversals to use
   ST2_DB_SELECT_TLF01_BY_TERMINAL_ID, using
   terminal_id, card_num, and total_amount as keys.
   
      Rev 1.1   25 Feb 1999 16:36:46   dirby
   Added code to set and clear host timer for
   log on messages.
   
      Rev 1.0   19 Feb 1999 15:10:22   dirby
   Initial version
   
      Rev 1.2   21 Dec 1998 10:35:46   dirby
   Fixed memory leak in function 'send_network_status_to_monitor'.
   
      Rev 1.1   07 Dec 1998 14:19:34   dirby
   Changed the read_timer code to send to Tflogger's queue.
   It was previously sending to trands.
   
      Rev 1.0   Dec 04 1998 14:50:12   MSALEH
   initial release
 
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
#include "genutil.h"
#include "mcrdmain.h"
#include "mcrdutil.h"
#include "ncutils.h"
#include "txutils.h"
#include "tx_dbstruct.h"



/* GLOBAL VARIABLES */
extern CHAR  Stan[7];
extern CHAR  Logon_NMI_Code[4];
extern INT   Tran_Flow;

/* Queue definitions */
extern CHAR  updateds_que_name[];
extern CHAR  cur_app_name [];
extern CHAR  authorizer_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  applnk_que_name [];
extern CHAR  trands_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  netds_que_name[];
extern CHAR  security_que_name[];
extern CHAR  cirrus_que_name[];
extern CHAR  timerds_que_name[];
extern CHAR  AppName[8];

extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
/* Database structures */
extern NCF01               ncf01_i;
extern NCF01               ncf01_a;
extern NCF30               ncf30;
extern NEG01               neg01;
extern TLF01               tlf01;
extern MCC_ONLINE_UPDATES  Mcc_Gui;
extern BDKKEY01  Bdkkey01;

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;
extern INT    Mcc_Gui_Size;
extern INT	   Bdkkey01_Size;

/* Shared memory variables */
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];
extern BYTE   current_state[2];
extern CHAR   SHARED_MEMORY_TBL[15];

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* These are used for reject 0620/0630 messages. */
extern CHAR   fwd_inst_id_len[3];
extern CHAR   fwd_inst_id[12];
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

extern INT    reqidx;                 /* next open position in request buffer */
extern INT    buflen;                 /* total size of buffer */
extern INT    bitmapidx;              /* offset of bitmap... */

extern BYTE   ReqRsp_Buffer[COMM_BUF_SZE];
extern int    BitFieldSize;

extern char   A2E[4];    /*  ASCII to EBCDIC conversion */
extern char   E2A[4];    /*  EBCDIC to ASCII conversion */

extern INT    MCC_Validation_Functionality;

extern CHAR Last_Tran_Logging_Filename[256];

CHAR  echo_lookup_key[15]        = "ECHO_MCRD";
CHAR  echo_request_timer_key[15] = "ECHO_REQ_MCRD";
extern WORD  echo_timer_length;
CHAR  last_lookup_key[ MAX_TIMER_KEY_SIZE ];
CHAR  clear_timer_key[ MAX_TIMER_KEY_SIZE ];
/******************************************************************************
 *
 *  NAME:         SEND_IT_BACK_TO_TERMINAL
 *
 *  DESCRIPTION:  This function will build a message to send to the authorizer.
 *                This is used for when a transaction has been rejected before
 *                being sent to the host.
 *
 *  INPUTS:       resp_code - Response code to be placed into p_auth_tx
 *                p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_it_back_to_terminal( pCHAR resp_code, pAUTH_TX p_auth_tx )
{
   INT   ret_code;
   CHAR  err_msg[512]={0};

   strcpy( p_auth_tx->TLF01_details.response_code, resp_code );
   p_auth_tx->TLF01_details.tx_key = 
             match_request_to_response( p_auth_tx->TLF01_details.tx_key );

   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE,
                                      application_que_name, err_msg );
   if ( OK != ret_code )
   {
	   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
	   					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
	                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
	   strcat(err_msg,Log_Bufr);
	   ncmcrd_log_message( 3, 3, err_msg , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
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
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 04/14/99 - Added offline cases for override transactions.
 ******************************************************************************/
INT incoming_terminal_request( pAUTH_TX p_auth_tx )
{
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   CHAR            ErrorMsg[512] = {0};
   BYTE            temp_tx_key;
   INT             ret_code;

   /* See if the transaction failed security for some reason. */
   if( ( GS_NSP_KEYCHANGE_FAILED <  p_auth_tx->TLF01_details.general_status ) &&
       ( GS_NSP_OUTOFSERVICE     >= p_auth_tx->TLF01_details.general_status )  )
   {
      /* It failed.  Send it back to the terminal. */
      ret_code = send_it_back_to_terminal( "58", p_auth_tx );
   }
   else
   {
      /* Set general_status for auth_tx to NEW. */
      p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;

      /*+-------------------------------------------------+
        | Check if we do standin processing or if we need |
        | to go online to the host for this transaction.  |
        +-------------------------------------------------+*/
      ret_code = OK;
      temp_tx_key = p_auth_tx->TLF01_details.tx_key;
      switch ( temp_tx_key )
      {
         case AUTH_OFFLINE_SALE: 
         case AUTH_REFUND: 
         case AUTH_OFFLINE_REFUND: 
			 /* Don't process void sale txna as locally. send it to MC host (09-76) 10 Nov, 2009 TF-Ajay */
        // case AUTH_VOID_SALE:
         case AUTH_VOID_REFUND:
         case AUTH_SALE_ADJUSTMENT: 
         case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE: 
         case AUTH_REFUND_ADJUSTMENT: 
         case AUTH_VOID_CASH_ADVANCE: 
         case AUTH_SALES_COMPLETION: 
         case AUTH_OFFLINE_VOID_SALE: 
         case AUTH_OFFLINE_VOID_REFUND: 
            p_auth_tx->TLF01_details.tx_key = 
                   match_request_to_response( p_auth_tx->TLF01_details.tx_key );
            perform_standin_processing( APPROVED, GS_TXN_COMPLETED_OFF, p_auth_tx );
            ret_code = OK;
            break;

         case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
         case AUTH_AUTHORIZATION_OVERRIDE:
			/*TF Praneeth added to support tx_key revert back while stip transaction */
			p_auth_tx->TLF01_details.voice_auth_text[0] = temp_tx_key;
            if ( p_auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE)
               p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION;
            else
               p_auth_tx->TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;

            /* Do not break out of switch.  Fall thru to online processing. */

         default:
         {
            /*+---------------------------------------+
              | Process all OnLine transactions here. |
              +---------------------------------------+*/

            /* Confirm host is available */
            Tran_Flow = SWITCHED_OUT;
            if( OK != verify_host_app_status( p_auth_tx ) )
            {
               /* HOST IS NOT AVAILABLE. */
               strcpy( p_auth_tx->TLF01_details.response_text, "Host Inactive");
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf( Log_Bufr," Host Inactive Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
               		    p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               ncmcrd_log_message( 3, 2, Log_Bufr , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num);
			   /*TF Praneeth revert back tx_key  while stip transaction */
			   if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
			   {
				   p_auth_tx->TLF01_details.tx_key = p_auth_tx->TLF01_details.voice_auth_text[0];
				   memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
			   }
               /* Build response and send to terminal */
               p_auth_tx->TLF01_details.tx_key = 
                   match_request_to_response( p_auth_tx->TLF01_details.tx_key );

               /* Send error response back to originator. */
               perform_error_processing( p_auth_tx, "01" );  
               ret_code = OK;
            }
            else
            {
				/*TF Praneeth clearing buffer variable used to store tx_key  while stip transaction */
				if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
				{
					memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
				}
               /* HOST IS AVAILABLE. */
               if ( DO_NEG_FILE_PROCESSING == YES )
               {
                  /* Go check for the card number in negative file. */
                  ret_code = process_neg_file( p_auth_tx );
               }
               else
               {
                  /* Functionality to check neg file is not turned on.
                   * Send request to host.
                   */
                  ret_code = send_request_to_host( p_auth_tx );
               }
            }
         }  /* End DEFAULT case of Switch statement. */
      }  /* End SWITCH statement. */
   }
   return( ret_code );
}  /* incoming_terminal_request */  

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
 *                This function also processes a response destined for a host.
 *
 *  INPUTS:       p_msg - This is the incoming message from a terminal handler
 *                        or a request from a host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/12/99 - Moved NCF01 code to a separate function.
 *     DI - 03/26/99 - Added code to ignore reversals.
 *     DI - 04/07/99 - Removed code that ignored reversals.  They must be
 *                     processed. This gets done in the ...ncf01_reply function.
 *     DI - 05/27/99 - Added line to ignore reversal responses.  When an 0420
 *                     comes from the host, we respond immediately, then ship
 *                     an 0400 to the authorizer.  The authorizer responds with
 *                     an 0410.  We need to ignore it, or else we will send a
 *                     second response to the host.
 ******************************************************************************/
INT process_incoming_message( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   LONG            data_len;
   LONG            auth_len;
   AUTH_TX         auth_tx;
   INT             ret_code;
   CHAR            Buff[512]={0};
   char			  last_tran__msg[256] = {0};
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr= NULL_PTR;

   memset( &auth_tx, 0x00, Auth_Tx_Size );

   /* Copy transaction record from PTE message into auth_tx structure. */
   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   auth_len   = data_len;
   if ( Auth_Tx_Size < data_len )
      auth_len = Auth_Tx_Size;
   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &auth_tx, p_data_data, auth_len );
   p_data_data = p_data_data + Auth_Tx_Size;

   /*************Last transaction to be process in DB************/
    lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
   	if(lastmsgptr != NULL_PTR)
   	{
   		sprintf( last_tran__msg,
   			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
   			   auth_tx.TLF01_details.card_num,
   			   auth_tx.TLF01_details.sys_trace_audit_num,
   			   auth_tx.TLF01_details.terminal_id,
   			   auth_tx.TLF01_details.merchant_id,
   			   auth_tx.TLF01_details.acquirer_id,
   			   auth_tx.TLF01_details.handler_queue);

   		Last_Message_len=strlen(last_tran__msg);

   		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
   		fclose(lastmsgptr);
   	}

	 /* Added for April mandated 2020 CRF document.
      * The dcpiso module is not used to store details from MCF01 table to TLF01 table
      * Hence copied here
      */

   if((0 == strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3)) || ((0 == strncmp(auth_tx.TLF01_details.handler_queue,"VOICE",5)) && (AUTH_AUTHORIZATION == auth_tx.TLF01_details.tx_key)) &&
   		   auth_tx.MCF01_details.password[0]!= '\0')
       {
	   if(MCO_LENGTH == strlen(auth_tx.MCF01_details.password) &&
		  ' ' 	!= auth_tx.MCF01_details.password[0] &&
		  true	== isnum( auth_tx.MCF01_details.password, 3))
	   {
       	strncpy(auth_tx.TLF01_details.orig_auth_number,auth_tx.MCF01_details.password,3 );
       }
	   else
	   {
		   memset(auth_tx.TLF01_details.orig_auth_number,0x00,sizeof(auth_tx.TLF01_details.orig_auth_number) );
	   }

   }

   /* Ignore reversal and auth advice responses from the authorizer. */
      /* So far we  are not send response. so send response to MC host 13 Nov,2009 TF-Ajay*/
	if(ncmcrd_Check_Reversal_Advice_Iss_txns ( &auth_tx) == true)
   {

	      /* Retreive the response code sent by MC to insert in TLF01 for Auth  Advice*/
	   	if(0 != strncmp(auth_tx.TLF01_details.product_codes[13].quantity, "00", 2))
	   	{
	   		strncpy(auth_tx.TLF01_details.response_code, auth_tx.TLF01_details.product_codes[13].quantity, 2);
	   	}

       update_database(&auth_tx );
       ret_code = OK;
   }
  
   /* MasterCard supports Reversal txn. don't process as standin. Send it to host (09-76) 02 Nov,2009 TF-Ajay*/

   /*else if ( AUTH_REVERSAL == auth_tx.TLF01_details.tx_key )
   {
      // This is a reversal request from the authorizer.
       // MasterCard does not process reversals.
       
      update_txn_stats( &Txn_Counts, &auth_tx, NA );  // Update reversal cnt. 

      (void)perform_standin_processing( APPROVED, GS_TXN_COMPLETED_OFF,
                                         &auth_tx );
      ret_code = OK;

      /* Display reversal message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants or to networks.
       
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff,
              "Reversal for MC." );

		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Unable to update database tables. Term Id:%s ,Card Num:%s ,Merch Id: %s RRN: %s ",
				auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.card_num,
				auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
		strcat(Buff,Log_Bufr);
		ncmcrd_log_message( 3, 3, Buff , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
   }*/

   else
   {
      /* Store TPDU ID so Dialog knows where to route our request. */
      strncpy( auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,
               sizeof(ncf01_i.tpdu_id));


    /* Switched in transactions are requests from the host that ATP approves.
       * Switched out transactions are requests from ATP to the host.
       * Here we determine which type it is, based on tx_key.
       */
      if ( ( AUTH_SALE    > auth_tx.TLF01_details.tx_key ) || 
           ( AUTH_INVALID < auth_tx.TLF01_details.tx_key )  )
      {
         Tran_Flow = SWITCHED_IN;
         if((0 == strcmp(auth_tx.TLF01_details.response_code, APPROVED)) ||
            (0 == strcmp(auth_tx.TLF01_details.response_code, INVALID_PIN)))
         {
         	if( (auth_tx.acceptor_term_name[6]== '8') && (0== strncmp(auth_tx.TLF01_details.visa_merchant_id,"01",2)))
			{
				strncpy( auth_tx.TLF01_details.response_code,"00",2 );
			}
            if ( false == update_database( &auth_tx ))
            {
               strcpy(auth_tx.TLF01_details.response_code,"96");
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to update database tables. Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
               					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               memcpy( auth_tx.TLF01_details.response_text, "Unable to update database tables.",
                             sizeof(auth_tx.TLF01_details.response_text)-1 );
               ncmcrd_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);

               /* Unable to complete a transaction. Inform host; log to TLF01.*/
               if(false == incoming_other_host_response_db_reply( &auth_tx ))
               {
                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                  sprintf(Log_Bufr," Unable to  send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
                  					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
                  memcpy( auth_tx.TLF01_details.response_text, "Unable to  send response to host.",
                                sizeof(auth_tx.TLF01_details.response_text)-1 );
                 ncmcrd_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0,auth_tx.TLF01_details.card_num );
               }
            }
         }
         else
         {
            if(false == incoming_other_host_response_db_reply( &auth_tx ))
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to  send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",	auth_tx.TLF01_details.terminal_id,
               					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               memcpy( auth_tx.TLF01_details.response_text, "Unable to  send response to host.",
                             sizeof(auth_tx.TLF01_details.response_text)-1 );
               ncmcrd_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }
         return(true);
      }
      else
      {
         Tran_Flow = SWITCHED_OUT;

         /* Set acquirer Id. */
         strncpy( auth_tx.TLF01_details.acquirer_id, AppName,
                  sizeof(auth_tx.TLF01_details.acquirer_id) );

          if ( OK == mcrdutil_verify_pin_block(&auth_tx) )
         {
        	  if (true == ncmcrd_is_POS_PIN_transaction(&auth_tx))
			  {
        		  /* Perform Decryption and PIN translation */
        		  strncpy( auth_tx.TLF01_details.dcf01_retired_cwk, Bdkkey01.bdk_key,
        				  	  	  	  	  	  sizeof(auth_tx.TLF01_details.dcf01_retired_cwk)-1);
				  ret_code = perform_decrypn_encrypn_request( &auth_tx );
			  }
        	  else
        	  {
                  /* Perform PIN translation. */
                  ret_code = security_request( &auth_tx );
        	  }

         }
         else
         {
            /* Do not perform PIN translation; route to MasterCard. */
            ret_code = incoming_terminal_request( &auth_tx );
         }
      }
   }
   return( ret_code );
}


/******************************************************************************
 *
 *  NAME:         SECURITY_REQUEST
 *
 *  DESCRIPTION:  This function builds a PTE message for the crypto gear.
 *                It uses sub_type2 to determine purpose of the request.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if PTE message was successfully built and sent,
 *                else NOT_OK
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT security_request( pAUTH_TX p_auth_tx )
{
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   INT             ret_code = NOT_OK;
   CHAR            err_msg[512]={0};

   strcpy( p_auth_tx->TLF01_details.dest_key,        ncf01_i.cwk      );
   strcpy( p_auth_tx->TLF01_details.dest_pin_format, ncf01_i.pin_type );

   /* Build message for crypto. */
   memset( err_msg, 0x00, sizeof(err_msg) );
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf01_i, Ncf01_Size );

   ret_code = build_and_send_request_message( MT_NSP_TRANSLATE_PIN, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              security_que_name, 
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1), 
                                              0,
                                              NULL_STR,
                                              err_msg );
   if ( OK != ret_code  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.card_num,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_msg,Log_Bufr);
      ncmcrd_log_message( 3, 3, err_msg , "security_request" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
}


INT perform_decrypn_encrypn_request( pAUTH_TX p_auth_tx )
{
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   INT             ret_code = NOT_OK;
   CHAR            err_msg[512] = {0};

   strcpy( p_auth_tx->TLF01_details.dest_key,        ncf01_i.cwk      );
   strcpy( p_auth_tx->TLF01_details.dest_pin_format, ncf01_i.pin_type );

/*   strcpy( p_auth_tx->TLF01_details.source_key, Bdkkey01.bdk_key );	*/

   /* Build message for crypto. */
   memset( err_msg, 0x00, sizeof(err_msg) );
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf01_i, Ncf01_Size );

   ret_code = build_and_send_request_message( MT_NSP_DECRYPT_TRANSLATE_PIN,
                                              ST1_NONE,
                                              ST2_NONE,
                                              security_que_name,
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1),
                                              0,
                                              NULL_STR,
                                              err_msg );
   if ( OK != ret_code  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_msg,Log_Bufr);
      ncmcrd_log_message( 3, 3, err_msg , "security_request" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         SECURITY_REPLY
 *
 *  DESCRIPTION:  This function continues the processing of a transaction after
 *                having the pin translated or the CVC verified.
 *
 *  INPUTS:       p_msg_in  - Pte message from the crypto application.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT security_reply( pPTE_MSG p_msg_in )
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   AUTH_TX       auth_tx;
   INT           ret_val;
   char			  last_tran__msg[256] = {0};
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr= NULL_PTR;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   pCurrent   = ptemsg_get_pte_msg_data_data( p_msg_data );

   memset( &auth_tx, 0x00,     Auth_Tx_Size );
   memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

   /*************Last transaction to be updated in DB************/
    lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
   	if(lastmsgptr != NULL_PTR)
   	{
   		sprintf( last_tran__msg,
   			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
   			   auth_tx.TLF01_details.card_num,
   			   auth_tx.TLF01_details.sys_trace_audit_num,
   			   auth_tx.TLF01_details.terminal_id,
   			   auth_tx.TLF01_details.merchant_id,
   			   auth_tx.TLF01_details.acquirer_id,
   			   auth_tx.TLF01_details.handler_queue);

   		Last_Message_len=strlen(last_tran__msg);

   		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
   		fclose(lastmsgptr);
   	}

   Tran_Flow = SWITCHED_OUT;
   if ( 0 != strcmp( APPROVED, auth_tx.TLF01_details.security_response_code ) )
   {
      /* PIN Translation failed. Security module informed Monitor. */
      perform_error_processing( &auth_tx, "96" );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Pin translation failed. Cannot process request. Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
      					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      memcpy( auth_tx.TLF01_details.response_text, "Pin translation failed. Cannot process request.",
                    sizeof(auth_tx.TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "security_reply" , 0,auth_tx.TLF01_details.card_num );
      ret_val = NOT_OK;
   }
   else
   {
      ret_val = incoming_terminal_request( &auth_tx );
   }
   return( ret_val );
}

INT security_reply_for_decrypn_encrypn( pPTE_MSG p_msg_in )
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   AUTH_TX       auth_tx;
   INT           ret_val;
   char			  last_tran__msg[256] = {0};
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr= NULL_PTR;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   pCurrent   = ptemsg_get_pte_msg_data_data( p_msg_data );

   memset( &auth_tx, 0x00,     Auth_Tx_Size );
   memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

   /*************Last transaction to be updated in DB************/
    lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
   	if(lastmsgptr != NULL_PTR)
   	{
   		sprintf( last_tran__msg,
   			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
   			   auth_tx.TLF01_details.card_num,
   			   auth_tx.TLF01_details.sys_trace_audit_num,
   			   auth_tx.TLF01_details.terminal_id,
   			   auth_tx.TLF01_details.merchant_id,
   			   auth_tx.TLF01_details.acquirer_id,
   			   auth_tx.TLF01_details.handler_queue);

   		Last_Message_len=strlen(last_tran__msg);

   		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
   		fclose(lastmsgptr);
   	}

   Tran_Flow = SWITCHED_OUT;
   if ( 0 != strcmp( APPROVED, auth_tx.TLF01_details.security_response_code ) )
   {
      /* PIN Translation failed. Security module informed Monitor. */
      perform_error_processing( &auth_tx, "96" );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Pin translation failed. Cannot process request. Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
      					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      memcpy( auth_tx.TLF01_details.response_text, "Pin translation failed. Cannot process request.",
                    sizeof(auth_tx.TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "security_reply" , 0,auth_tx.TLF01_details.card_num );
      ret_val = NOT_OK;
   }
   else
   {
      ret_val = incoming_terminal_request( &auth_tx );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         PROCESS_NEG_FILE
 *
 *  DESCRIPTION:  This function builds a PTE message for the data server.
 *                Its purpose is to check the negative file for the card
 *                number in auth_tx.  If it is in the negative, it means
 *                we should not send this transaction to the host for approval.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if PTE message was successfully built and sent,
 *                else NOT_OK
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_neg_file( pAUTH_TX p_auth_tx )
{
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NEG01)+1];
   INT             ret_code;
   CHAR            err_msg[512] = {0};

   /* Build message for data server. */
   memset( err_msg, 0x00, sizeof(err_msg) );
   memset( buffer1, 0x00,      sizeof( buffer1 ) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );

   memset( &neg01, 0x00, sizeof(NEG01));
   strcpy( neg01.card_nbr, p_auth_tx->TLF01_details.card_num);
   strcpy( neg01.card_type, "M");
   memcpy( buffer1+Auth_Tx_Size, &neg01, sizeof(NEG01) );

   ret_code = build_and_send_request_message( MT_DB_REQUEST, 
                                              ST1_DB_SELECT, 
                                              ST2_NEG_FILE,
                                              netds_que_name, 
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1), 
                                              NEG01_DATA,
                                              NULL_STR,
                                              err_msg );
   if ( OK != ret_code  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_msg,Log_Bufr);
      ncmcrd_log_message( 3, 2, err_msg , "process_neg_file" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_NEG_FILE_REPLY
 *
 *  DESCRIPTION:  This function continues the processing of a transaction after
 *                checking the negative file for the card number.
 *
 *  INPUTS:       pCurrent   - String of data from the data server.
 *
 *  OUTPUTS:      card_status - true if not in NEG01
 *                              false if found in NEG01
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_neg_file_reply( pBYTE pCurrent, BOOLEAN card_status )
{
   AUTH_TX   auth_tx;


   /* If message came from data server, get it from the buffer. */
   if ( NULL != pCurrent )
   {
      memset( &auth_tx, 0x00,     Auth_Tx_Size );
      memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
   }

   if ( true == card_status )
   {
      /* Card number was NOT found in negative file.
       * This is good!  Continue processing to the host.
       */
      Tran_Flow = SWITCHED_OUT;
      return( send_request_to_host( &auth_tx ) );
   }
   else
   {
      /* Card number is in the negative file.  This means it
       * should not be processed ( lost or stolen, etc. ).
       * Inform monitor and send it back to terminal.
       */
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Card number found in Negative File - reject it Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
      					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      memcpy( auth_tx.TLF01_details.response_text, "Card number found in Negative File - reject it",
                    sizeof(auth_tx.TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "process_neg_file_reply" , 0 ,auth_tx.TLF01_details.card_num);

      perform_error_processing( &auth_tx, "05" );
   }
   return( OK );
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
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT perform_standin_processing( CHAR rsp_code[3], BYTE gen_status,
                                pAUTH_TX p_auth_tx )
{
   INT   ret_code;
   CHAR  err_msg[512] = {0};

   /* Create response code and store it in the auth_tx block  */
   strcpy(p_auth_tx->TLF01_details.response_code, rsp_code);
   
   if(strcmp(rsp_code,"00")== 0)
   strcpy(p_auth_tx->TLF01_details.response_text, "APPROVED_STAND_IN");
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

   /* Send response to the authorizer */
   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE,
                                      application_que_name, err_msg );
   if ( OK != ret_code )
   {
	   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
	   					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
	                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
	   strcat(err_msg,Log_Bufr);
	   ncmcrd_log_message( 3, 2, err_msg , "perform_standin_processing" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
} /* perform_standin_processing */


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
 *                This could be a response to a request (Acquirer) or a request
 *                for us from the host (Issuer).
 *
 *  INPUTS:       p_msg - This is the incoming message from the host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT incoming_host_message( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   AUTH_TX           auth_tx;
   BYTE              resp_code[3]="";
   INT               			ret_code = 0;
   BYTE 						temp_error_bit[4] = {0};
   char			  last_tran__msg[256] = {0};
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr= NULL_PTR;

   //HCTS1-A
   /* clear and set the echo timer  */
   if ( echo_timer_length > 0 )
   {
      (void)clear_echo_timer(echo_lookup_key,(pPTE_MSG *)NULL_PTR,
                             application_que_name, 1 );

      /* Set the echo timer. */
      if ( false == set_echo_timer(echo_lookup_key, echo_timer_length,
                                   (pPTE_MSG *)NULL_PTR,application_que_name,
                                   1) )
      {
         ncmcrd_log_message( 3, 3, "Error setting the echo_timer" , "incoming_host_message" , 0 ,"");
      }
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   /* Get the PTE message data from p_msg */
   p_msg_data = ptemsg_get_pte_msg_data( p_msg );  

   /* Copy data to ReqRsp_Buffer */
   copy_to_req_resp_buffer( ptemsg_get_pte_msg_data_data( p_msg_data ),
                            ptemsg_get_pte_msg_data_data_length( p_msg_data ) );

   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Parse message from host. */
   ret_code = mcrdutil_parse_host_response_msg();
   if ( OK == ret_code )
   {
      /* The transaction from host was successfully parsed.
       * Put information from host into auth_tx structure.
       */
      memset( &auth_tx, 0x00, Auth_Tx_Size );
	  	/*Copy orginal msg in req buffer*/
		
		if(buflen < COMM_BUF_SZE && buflen > 0)
		{
			sprintf(auth_tx.req_data_len,"%04d", buflen);
			memcpy(auth_tx.req_buffer,ReqRsp_Buffer,buflen);
		}
		ret_code = move_iso_response_to_auth_struct(&auth_tx,resp_code);
	   /*************Last transaction to be process in DB************/
		lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
		if(lastmsgptr != NULL_PTR)
		{
			sprintf( last_tran__msg,
				   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
				   auth_tx.TLF01_details.card_num,
				   auth_tx.TLF01_details.sys_trace_audit_num,
				   auth_tx.TLF01_details.terminal_id,
				   auth_tx.TLF01_details.merchant_id,
				   auth_tx.TLF01_details.acquirer_id,
				   auth_tx.TLF01_details.handler_queue);

			Last_Message_len=strlen(last_tran__msg);

			fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
			fclose(lastmsgptr);
		 }
      if ( OK != ret_code)
      {
         /* We only see an error like this for a switched in txn.
          * Need to send a declined response to host.
          */
    	 sprintf(temp_error_bit, "%03d",ret_code);
    	 strncpy(auth_tx.TLF01_details.num_sales,temp_error_bit,3);
         reject_switched_in_txn( &auth_tx, resp_code );
         ret_code = NOT_OK;
      }

      else if ( 0 == strcmp( "0820", auth_tx.TLF01_details.message_type ) )
      {
         /* Process Network Management Advice messages - 0820.
          * The only reason MCard would send this would be to
          * indicate the end of a SAF session.
          */
         process_network_advice( &auth_tx );
         ret_code = OK;
      }

      else if ( 0 == strcmp( "0190", auth_tx.TLF01_details.message_type ) )
      {
         /* Process MCard negative acknowledgement messages then exit.
          * This means MCard did not like our request for some reason.
          * Could be due to a missing data element or an ill-formatted value.
          */
         process_0190( &auth_tx );
         ret_code = OK;
      }

      /* Determine transaction type. */
      else if ( OK != get_tran_type_2( &auth_tx ) )
         ret_code = NOT_OK;
      else
      {
         /* Update transaction statistics. */
         update_txn_stats( &Txn_Counts, &auth_tx, NA );

         /* It's a valid txn.  Verify that we support it for this project. */
         if ( OK != verify_processing_code( &auth_tx ) )
         {
            /* We do not support it. */
            process_unsupported_txn( &auth_tx );
            ret_code = OK;
         }

         /*+-----------------------------------------------------------+
           | Transaction has been validated.  Go ahead and process it. |
           +-----------------------------------------------------------+*/

         /* Switched in transactions are requests from a host that ATP approves.
          * Switched out transactions are requests from ATP to the host.
          * Here we determine which type it is, based on tx_key.
          */
         else if ( (AUTH_SALE    <= auth_tx.TLF01_details.tx_key) &&
                   (AUTH_INVALID >= auth_tx.TLF01_details.tx_key) )
         {
            ret_code = process_switched_in_msg( p_msg, &auth_tx );
         }
         else
         {
            ret_code = process_response_of_switched_out_msg( &auth_tx );
         }
      }
   }
   return( ret_code );
}


/******************************************************************************
 *
 *  NAME:         PROCESS_RESPONSE_OF_SWITCHED_OUT_MSG
 *
 *  DESCRIPTION:  Process a transaction response.  A request had gone to the
 *                host for approval; here is the host's response.
 *
 *                The application will be exited here.  When the timer info
 *                returns to this application, processing gets picked up in
 *                function INCOMING_HOST_MESSAGE_TIMER_REPLY.
 *
 *  INPUTS:       p_auth_tx - Contains the host's response information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT process_response_of_switched_out_msg( pAUTH_TX p_auth_tx )
{
   CHAR       err_msg[512] = {0};
   INT        ret_code;
   BOOLEAN    ret_code1;

   /* This is the response from the host to a switched out transaction. */
   Tran_Flow = SWITCHED_OUT;

   /* Set host finish time. */
   ptetime_get_strtime( p_auth_tx->TLF01_details.host_finish_time );

   if ( 0 == strcmp( "0810", p_auth_tx->TLF01_details.message_type ) )
   {
      /* Log the message in monitor and clear the timerds request timer */
      process_logon_response( p_auth_tx );
   }
   else
   {
	   /* Adjust the counters appropriately for a response from the host. */
	   decrement_active_count( AppName, err_msg );
   }
   reset_timeout_count( AppName, err_msg );
   /* Send a PTE message to clear the request timer.  This will also
    * fetch the original auth_tx that gets stored in memory with the timer.
    */

   ret_code = clear_request_timer( p_auth_tx );

   return( ret_code );
}


/******************************************************************************
 *
 *  NAME:         CLEAR_REQUEST_TIMER
 *
 *  DESCRIPTION:  This function will clear a request timer that gets set when
 *                a transaction request is sent to the host.  If the timer
 *                does not get cleared, a time out message will eventually be
 *                issued.
 *
 *                A request to get auth_tx is chained to the clear timer
 *                message.  Auth_tx is stored with the timer when it is set.
 *
 *  INPUTS:       p_auth_tx - Contains the host's response information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT clear_request_timer( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_timer_out = NULL_PTR;
   pPTE_MSG   p_msg_chain     = NULL_PTR;
   CHAR       lookup_key[22];
   CHAR       err_msg[512] = {0};
   INT        ret_code;

   /* Build look up key to stop the host response timer. */
   memset( lookup_key, 0x00, sizeof(lookup_key));
   strcpy( lookup_key, AppName);
   strcat( lookup_key, p_auth_tx->TLF01_details.terminal_id         );
   strcat( lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num );

   /* Build PTEMSG to get Timer Data from the Host Data Server.
    * This allows us to get the original auth_tx structure; the
    * one that was built prior to sending it to the host.
    */
   p_msg_timer_out = ptetimer_bld_clear_timer_req(lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Insufficient Memory to build clear timer message",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "clear_request_timer" , 0 ,p_auth_tx->TLF01_details.card_num);
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
                                      timerds_que_name,  /* Data server   */
                                      application_que_name,
                                      (pBYTE)p_auth_tx,  /* Data to chain */
                                      Auth_Tx_Size,
                                      0 );
      if ( NULL_PTR == p_msg_chain )
	  {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Insufficient memory to build chain message Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         memcpy( p_auth_tx->TLF01_details.response_text, "Insufficient memory to build chain message",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcrd_log_message( 3, 2, Log_Bufr , "clear_request_timer" , 0 ,p_auth_tx->TLF01_details.card_num);
	  }
      else
	  {
         /* Send chain of timer data and auth_tx to data server. */
         memset( err_msg, 0x00, sizeof(err_msg) );
         ret_code = send_msg( p_msg_chain, timerds_que_name, err_msg );
         free( p_msg_chain );
         if ( OK != ret_code )
		 {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf( Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
            	     p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
            strcat(err_msg,Log_Bufr);
            ncmcrd_log_message( 3, 3, err_msg , "clear_request_timer" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }
   return( ret_code );
}

INT clear_timeout_timer( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_timer_out = NULL_PTR;
   CHAR       lookup_key[22];
   CHAR       err_msg[512] = {0};
   INT        ret_code;

   /* Build look up key to stop the host response timer. */
   memset( lookup_key, 0x00, sizeof(lookup_key));
   strcpy( lookup_key, AppName);
   strcat( lookup_key, p_auth_tx->TLF01_details.terminal_id         );
   strcat( lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num );

   /* Build PTEMSG to get Timer Data from the Host Data Server.
    * This allows us to get the original auth_tx structure; the
    * one that was built prior to sending it to the host.
    */
   p_msg_timer_out = ptetimer_bld_clear_timer_req(lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Insufficient Memory to build clear timer message",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "clear_timeout_timer" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
	   /* Send chain of timer data and auth_tx to data server. */
	   memset( err_msg, 0x00, sizeof(err_msg) );
	   ret_code = send_msg( p_msg_timer_out, timerds_que_name, err_msg );
	   free( p_msg_timer_out );
	   if ( OK != ret_code )
	   {
		   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		   sprintf( Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
				    p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		   memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
				   sizeof(p_auth_tx->TLF01_details.response_text)-1 );
		   strcat(err_msg,Log_Bufr);
		   ncmcrd_log_message( 3, 3, err_msg , "clear_timeout_timer" , 0 ,p_auth_tx->TLF01_details.card_num);
	   }
   }
   return( ret_code );
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
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
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
   AUTH_TX        orig_auth_tx;
   AUTH_TX        host_auth_tx;
   BOOLEAN		  ret_code=0;
   INT            ret_val=OK;
   char			  last_tran__msg[256] = {0};
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr= NULL_PTR;
   CHAR			  err_msg[256] = {0};

   /* Get Timer message, including lookup key. */
   timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
                                                app_data2, buffer,
                                                sizeof(buffer) );
   if ( (0 == strcmp(lookup_key,echo_lookup_key))       ||
        (0 == strcmp(lookup_key,echo_request_timer_key)) )
   {
      /* No need to process response to a command to clear the echo timer. */
	   return( OK );
   }
   else
   if ( timer_reply_size == sizeof(buffer) )
   {
      /* Get original auth_tx record prior to going to the host. */
      memset( &orig_auth_tx, 0x00,   Auth_Tx_Size );
      memcpy( &orig_auth_tx, buffer, Auth_Tx_Size );
   }
   else
   {
	   if(strncmp(lookup_key, last_lookup_key, strlen(lookup_key)) != 0 &&
		  strncmp(lookup_key, clear_timer_key, strlen(lookup_key)) != 0)
	   {
		  /* We did not get our original data with the timer message. */
		  memcpy( orig_auth_tx.TLF01_details.response_text, "Did not receive data with timer reply record.",
						sizeof(orig_auth_tx.TLF01_details.response_text)-1 );
		  ncmcrd_log_message( 3, 3, "Did not receive data with timer reply record." , "incoming_host_message_timer_reply" , 0 ,"");

	   }
	   else
	   {
		   ncmcrd_log_message( 3, 1, "Cleared timerds request for repeated timeouts" , "incoming_host_message_timer_reply" , 0 ,"");
	   }

	   return( NOT_OK );
   }

   /* Get auth_tx record that was chained to the timer message.
    * This is the auth_tx record built from response data from
    * the MasterCard host.
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
	   if(strncmp(lookup_key, last_lookup_key, strlen(lookup_key)) != 0 &&
		  strncmp(lookup_key, clear_timer_key, strlen(lookup_key)) != 0)
	   {
		  /* We did not get our original data with the timer message. */
		  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		  sprintf(Log_Bufr," Did not receive data with timer reply record. Term Id:%s ,Merch Id: %s RRN: %s ",
				  orig_auth_tx.TLF01_details.terminal_id,orig_auth_tx.TLF01_details.merchant_id,orig_auth_tx.TLF01_details.retrieval_ref_num);
		  memcpy( orig_auth_tx.TLF01_details.response_text, "Did not receive data with timer reply record.",
						sizeof(orig_auth_tx.TLF01_details.response_text)-1 );
		  ncmcrd_log_message( 3, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,orig_auth_tx.TLF01_details.card_num);
	   }
	   else
	   {
		   ncmcrd_log_message( 3, 1, "Cleared timerds request for repeated timeouts" , "incoming_host_message_timer_reply" , 0 ,"");
	   }

	   return( NOT_OK );
   }

   /*************Last transaction to be updated in DB************/
    lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
   	if(lastmsgptr != NULL_PTR)
   	{
   		sprintf( last_tran__msg,
   			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
			   orig_auth_tx.TLF01_details.card_num,
			   orig_auth_tx.TLF01_details.sys_trace_audit_num,
			   orig_auth_tx.TLF01_details.terminal_id,
			   orig_auth_tx.TLF01_details.merchant_id,
			   orig_auth_tx.TLF01_details.acquirer_id,
			   orig_auth_tx.TLF01_details.handler_queue);

   		Last_Message_len=strlen(last_tran__msg);

   		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
   		fclose(lastmsgptr);
   	}

   /* Process File Update Responses */
   if ( 0 == strncmp( host_auth_tx.TLF01_details.message_type, "0312", 4  ) )
   {
      strcpy( orig_auth_tx.TLF01_details.response_code,
              host_auth_tx.TLF01_details.response_code );

      strcpy( orig_auth_tx.TLF01_details.resp_source,
              host_auth_tx.TLF01_details.resp_source );

      strcpy( orig_auth_tx.TLF01_details.host_finish_time,
              host_auth_tx.TLF01_details.host_finish_time );

	     orig_auth_tx.TLF01_details.tx_key =
           match_request_to_response( orig_auth_tx.TLF01_details.tx_key );

	  memcpy(orig_auth_tx.TLF01_details.message_type, host_auth_tx.TLF01_details.message_type, 4);
	  memcpy(orig_auth_tx.req_data_len, host_auth_tx.req_data_len, sizeof(host_auth_tx.req_data_len) );
	  memcpy(orig_auth_tx.req_buffer,   host_auth_tx.req_buffer, sizeof(host_auth_tx.req_buffer) );

      return( process_file_update_response( &orig_auth_tx ) );
   }

   /* Get original communications information. */
   if ( 0 == strcmp( orig_auth_tx.TLF01_details.message_type, "0800"  ))
   {
     strcpy( orig_auth_tx.TLF01_details.originator_info,
             host_auth_tx.TLF01_details.originator_info );
   }

   orig_auth_tx.TLF01_details.tx_key =
           match_request_to_response( orig_auth_tx.TLF01_details.tx_key );

   /* Check for log on messages. */
   if ( 0 == strcmp( host_auth_tx.TLF01_details.message_type, "0810"  ))
   {
	   /* Already handled the logon responses before sending timerds clear request */
	   /*process_logon_response( &host_auth_tx );*/
	   ncmcrd_log_message( 3, 1, "Cleared the timerds request for Network Mgmt(0810) responses", "incoming_host_message_timer_reply", 0 , "");

	   return( OK );
   }

   /* Copy data from the host into the original auth_tx record for
    * the database and for the response to the terminal.
    */
   if ( OK != alter_transaction_data( &host_auth_tx, &orig_auth_tx ) )
      return( NOT_OK );

   /* Check result of CVC validation. */
   if ( orig_auth_tx.TLF01_details.cvc == INVALID_CVC )
   {
      orig_auth_tx.TLF01_details.general_status = GS_MAGSTRIPE_FAILED;
   }

#if BDOR_62_PREAUTH_VOID
   if(strncmp(orig_auth_tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0 )
   {
	   if ( OK != send_response_to_parvbh( &orig_auth_tx,MT_AUTH_RESPONSE,
               	   	   	   	   	   	   	   application_que_name, err_msg ))
	   {
		   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		   sprintf(Log_Bufr," Unable to send message to parvbh Msg_Type:%s, Term Id:%s, Merch Id:%s, RRN:%s ",
				   orig_auth_tx.TLF01_details.message_type,orig_auth_tx.TLF01_details.terminal_id,
				   orig_auth_tx.TLF01_details.merchant_id,orig_auth_tx.TLF01_details.retrieval_ref_num);
		   memcpy( orig_auth_tx.TLF01_details.response_text, "Unable to send message Parvbh",
				   sizeof(orig_auth_tx.TLF01_details.response_text)-1 );
		   ncmcrd_log_message( 1, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,orig_auth_tx.TLF01_details.card_num);
		   return( NOT_OK );
	   }
	   return( OK );
   }
#endif

   /* Add response code text. */
   if ( orig_auth_tx.TLF01_details.voice_auth[0] == 'Y' )
      mcrdutil_get_response_text( &orig_auth_tx );

   strcpy( orig_auth_tx.TLF01_details.dispensation.auth_2, "RA" );

   if(0 == strncmp(orig_auth_tx.TLF01_details.product_codes[8].code,"PA",2) &&
	  0 == strncmp(orig_auth_tx.TLF01_details.message_type,"0400",2))
   {
	   strcpy(orig_auth_tx.TLF01_details.response_text,"Reversal for partially approved txn");
	   if ( false == insert_tlf01( &orig_auth_tx ))
	   {
			strcpy(orig_auth_tx.TLF01_details.response_code,"96");
			memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			sprintf(Log_Bufr," failed to send to send the txn for update. Term Id:%s ,Merch Id: %s RRN: %s ",orig_auth_tx.TLF01_details.terminal_id,
					orig_auth_tx.TLF01_details.merchant_id,orig_auth_tx.TLF01_details.retrieval_ref_num);
			memcpy( orig_auth_tx.TLF01_details.response_text, "Unable to update database tables.",
						sizeof(orig_auth_tx.TLF01_details.response_text)-1 );
			ncmcrd_log_message( 3, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,orig_auth_tx.TLF01_details.card_num);
	   }
	   return( OK );
   }
   if(true == ncmcrd_check_for_acq_refund_transaction(&orig_auth_tx))
   { /* send resposne directly to ncmcr2*/
	   memcpy(orig_auth_tx.req_data_len,host_auth_tx.req_data_len,sizeof(host_auth_tx.req_data_len));
	   memcpy(orig_auth_tx.req_buffer,host_auth_tx.req_buffer,sizeof(host_auth_tx.req_buffer));
	   ret_code = ncmcrd_transfer_refund_response_msg_to_MC2(&orig_auth_tx);
	   return ret_code;
   }
   if(true == ncmcrd_check_for_partial_approved_txn(&orig_auth_tx))
   {
	   strncpy(orig_auth_tx.TLF01_details.product_codes[13].quantity,host_auth_tx.TLF01_details.response_code,2);
	   strncpy(orig_auth_tx.TLF01_details.response_code,"05",2);
	   strcpy(orig_auth_tx.TLF01_details.response_text,"partially approved txn");
   }

   /*
    * store the rc for MC12_oct2023
    * */
   if(0 == strncmp(host_auth_tx.TLF01_details.response_code,"87",2))
   {
	   strncpy(orig_auth_tx.TLF01_details.product_codes[13].quantity,host_auth_tx.TLF01_details.response_code,2);
   }

   if ( OK != send_response_to_term_app( &orig_auth_tx ) )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to send message(s) Term Id:%s ,Merch Id: %s RRN: %s ",
    		  orig_auth_tx.TLF01_details.terminal_id,orig_auth_tx.TLF01_details.merchant_id,orig_auth_tx.TLF01_details.retrieval_ref_num);
      memcpy( orig_auth_tx.TLF01_details.response_text, "Unable to send message(s)",
                    sizeof(orig_auth_tx.TLF01_details.response_text)-1 );
      ncmcrd_log_message( 1, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,orig_auth_tx.TLF01_details.card_num);
      return( NOT_OK );
   }

   if(true == ncmcrd_check_for_partial_approved_txn(&orig_auth_tx))
   { // TXn partially approved BY MC, send reversal to MC
	   strncpy(orig_auth_tx.TLF01_details.product_codes[8].code,"PA",2);
	   ret_val = ncmcrd_send_reversal_to_host(&orig_auth_tx);
	   if(NOT_OK == ret_val)
	   {
		   // failed to send th reveersal to host
	   }
   }



   return( OK );

} /* incoming_host_message_timer_reply */


/******************************************************************************
 *
 *  NAME:         PROCESS_SWITCHED_IN_MSG
 *
 *  DESCRIPTION:  Process a transaction request or advice from the host.
 *                This is the transaction's entry into ATP.
 *
 *  INPUTS:       p_msg     - This is the incoming message from the host.
 *                p_auth_tx - Contains the data portion of p_msg.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_switched_in_msg( pPTE_MSG p_msg, pAUTH_TX p_auth_tx )
{
   NCF30          ncf30;
   pPTE_MSG       p_msg_ins = NULL_PTR;
   CHAR           Buff[512] = {0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   BYTE           subtype2;
   LONG           ret_code; 
   CHAR           ErrorMsg[512] = {0};
  
   /* This is the start of a switched in transaction. */
   Tran_Flow = SWITCHED_IN;

   /* Save originator information so we can find our way back to the host. */
   strcpy( p_auth_tx->TLF01_details.originator_queue,
           ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( p_auth_tx->TLF01_details.originator_info,
           ptemsg_get_msg_orig_comms_info (p_msg) );

   (void)mcrdutil_init_txn( p_auth_tx );

   /* Are we blocking this transaction?
    * Commented as per the april mandated BR 2020.
   if ( true == txn_is_blocked(p_auth_tx,ErrorMsg) )
   {
      // Txn is blocked.  Log message and respond to host.
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
    		  p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(ErrorMsg,Log_Bufr);
      ncmcrd_log_message( 3, 2, ErrorMsg , "process_switched_in_msg" , 0 ,p_auth_tx->TLF01_details.card_num);

      strcpy( p_auth_tx->TLF01_details.response_code, NOT_PERMITTED );
      p_auth_tx->TLF01_details.tx_key += 100;

      //Build response message for the host.
      build_host_structure( p_auth_tx );
      build_host_request( p_auth_tx );

      // Send response to the host.
      ret_code = mcrdutil_send_host_response(p_auth_tx);
      (void)insert_into_TLF01( p_auth_tx );
   }
   else */
   if( AUTH_ADVICE  == p_auth_tx->TLF01_details.tx_key )
   {
	  // (void)process_auth_advices( &auth_tx, FOUND, ncf30 );
	   strcpy(p_auth_tx->TLF01_details.message_type,"0120");
	   (void)process_auth_advices(p_auth_tx,FOUND,ncf30);
	   return (true);

   }
   else if(ncmcrd_Check_for_Reversal_txn(p_auth_tx) == true)
   {
 	  /*Sends the
 	   * 1. Reponse for reversal and advice to HOST.
 	   * 2. Request to Txcntl
 	   * */
 	 ncmcrd_process_reversal_transactions(p_auth_tx,FOUND);
   }
   /* Handle Reversal Advices here, then exit. */
   else if ( AUTH_VOID_SALE  == p_auth_tx->TLF01_details.tx_key )
   {
      /* Must get the original transaction for a reversal and advice.
       * If the original transaction is not found, reply to the
       * host for reversals and don't send the transaction to the authorizer.
       */
	   	 memset( &ncf30, 0x00, Ncf30_Size );
		 strcpy( p_auth_tx->TLF01_details.reversal_amount,
				 p_auth_tx->TLF01_details.total_amount );
		 strcpy(p_auth_tx->TLF01_details.message_type,"0100");
		 subtype2 = ST2_DB_SELECT_TRAN_BY_CARD_NUM;
		 strcpy( ncf30.primary_key.network_id, AppName );
		 strcpy( ncf30.primary_key.network_type, "A" );
		 strcpy( ncf30.primary_key.retrieval_ref_num,
				 p_auth_tx->TLF01_details.retrieval_ref_num);
		 strcpy( ncf30.card_nbr,       p_auth_tx->TLF01_details.card_num );
		 strcpy( ncf30.tran_amount,    p_auth_tx->TLF01_details.total_amount );
		 strcpy( ncf30.processing_code,SALEPROCCODE);
		 strcpy( ncf30.currency_code,  p_auth_tx->TLF01_details.currency_code);
		 memset( Buff, 0x00, sizeof(Buff) );
		  sprintf( Buff,
			  "Void sale for MC. Merchant %s",
			   p_auth_tx->TLF01_details.merchant_id );

		  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
							p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		  strcat(Buff,Log_Bufr);
		  ncmcrd_log_message( 3, 1, Buff , "process_switched_in_msg" , 0 ,p_auth_tx->TLF01_details.card_num);



      /* Build message to get the original transaction from data server. */
      memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
      memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );

      p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                    ST1_DB_SELECT, 
                                    subtype2,
                                    netds_que_name, 
                                    application_que_name,
                                    (pBYTE)&buffer1,
                                    sizeof(buffer1), 
                                    NCF30_DATA );
      if ( NULL_PTR == p_msg_ins  )
      {
         ncmcrd_log_message( 3, 2, "Insufficient Memory for select ncf30 message" , "process_switched_in_msg" , 0 ,"");
         return( false );
      }

      /* Send message to get host specific data from data server. */
      ret_code = pteipc_send( p_msg_ins, netds_que_name );
      free( p_msg_ins );
      if ( 0 > ret_code )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buff,"Failed transmit to queue: %s. %s",
                  netds_que_name,ErrorMsg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
         					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncmcrd_log_message( 3, 3, Buff , "process_switched_in_msg" , 0 ,p_auth_tx->TLF01_details.card_num);
         return( false );
      }
      return( true );
   }  /* Process MCard reject messages then exit. */

   else if ( AUTH_PRE_AUTHORIZATION_CANCLE  == p_auth_tx->TLF01_details.tx_key )
   {
		/* Must get the original transaction for a Purchange return.
		* If the original transaction is not found, reply to the
		* host for reversals and don't send the transaction to the authorizer.*/

		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Auth Refund txn with Term Id:%s ,Merch Id: %s RRN: %s ",
				p_auth_tx->TLF01_details.terminal_id,
				p_auth_tx->TLF01_details.merchant_id,
				p_auth_tx->TLF01_details.retrieval_ref_num);
		strcat(Buff,Log_Bufr);
		ncmcrd_log_message( 3, 1, Buff , "process_switched_in_msg" , 0 ,
							p_auth_tx->TLF01_details.card_num);
		/* Build message to get the original transaction from data server. */
		memcpy( buffer1,p_auth_tx, Auth_Tx_Size );

		p_msg = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_SELECT,
								  ST2_DB_SELECT_TLF01_FOR_PURCHANGE_RETURN,
								  netds_que_name,
								  application_que_name,
								  (pBYTE)&buffer1,
								  sizeof(buffer1),
								  TLF01_DATA );

		if ( NULL_PTR == p_msg  )
		{
			ncmcrd_log_message( 3, 2, "Insufficient Memory for select ncf30 message" ,
								"process_switched_in_msg" , 0 ,"");
			return( false );
		}

		/* Send message to get host specific data from data server. */
		ret_code = pteipc_send( p_msg, netds_que_name );
		free( p_msg );
		if ( 0 > ret_code )
		{
			pteipc_get_errormsg( ret_code, ErrorMsg );
			sprintf( Buff,"Failed transmit to queue: %s. %s",netds_que_name,ErrorMsg );
			memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
					p_auth_tx->TLF01_details.terminal_id,
					p_auth_tx->TLF01_details.merchant_id,
					p_auth_tx->TLF01_details.retrieval_ref_num);
			strcat(Buff,Log_Bufr);
			ncmcrd_log_message( 3, 3, Buff , "process_switched_in_msg" , 0 ,
								p_auth_tx->TLF01_details.card_num);
			return( false );
		}
		return( true );
   }  /* Process MCard reject messages then exit. */

   else if ( 0 == strcmp( "0620", p_auth_tx->TLF01_details.message_type ) )
   {
      /* Note:  These must be processed prior to leaving the
       * application because global variables are used.  We
       * cannot recover the original message that caused a 0620
       * because no terminal Id is returned with it.
       */
      process_reject( p_auth_tx );
      ret_code = OK;
   }

   /* If the MCard host sent a logon request, handle it here. */
   else if ( 0 == strncmp( p_auth_tx->TLF01_details.message_type, "08", 2 ) )
   {
      ret_code = perform_host_logon_request(NULL_STR,SOURCE_IS_HOST,p_auth_tx);
   }

   else
   {
      if ( MCC_Validation_Functionality == MCC_VALIDATION_ON )
      {
         /* Validate Merchant Category Code (MCC) */
         ret_code = validate_mcc( p_auth_tx );
         if ( ret_code == true )
         {
            /* Process the authorization request from the host. */
            ret_code = mcrdutil_process_host_request( p_auth_tx );
         }
         else
         {
            /* Invalid MCC.  Decline and return to sender. */
            strcpy( p_auth_tx->TLF01_details.response_code, CALL_ISSUER );
            p_auth_tx->TLF01_details.tx_key += 100;

            /* Build response message for the host. */
            build_host_structure( p_auth_tx );
            build_host_request( p_auth_tx );

            /* Send response to the host. */
            ret_code = mcrdutil_send_host_response(p_auth_tx);
            (void)insert_into_TLF01( p_auth_tx );
         }
      }
      else
      {
         /* Process the authorization request from the host. */
         ret_code = mcrdutil_process_host_request( p_auth_tx );
      }
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_TIMEOUT_MSG
 *
 *  DESCRIPTION:  Process a timeout message.  This means the host did not
 *                respond to a request from ATP within the allotted amount
 *                of time.  Here we clear the timer (else it will continue
 *                to inform us of the time out), decrement the active txn
 *                count, and perform some error processing.  Then the txn
 *                gets sent to the authorizer.
 *
 *  INPUTS:       p_msg - This is the incoming message from the timer.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_timeout_msg( pPTE_MSG p_msg )
{
   BYTE        buffer2[ sizeof(AUTH_TX) ];
   CHAR        lookup_key[ MAX_TIMER_KEY_SIZE ];
   CHAR        app_data1[ MAX_APP_DATA_SIZE ];
   CHAR        app_data2[ MAX_APP_DATA_SIZE ];
   CHAR        Buff[512] = {0};
   INT         ret_code = OK;
   LONG        timer_reply_size = 0;
   AUTH_TX     auth_tx;
   char		   last_tran__msg[256] = {0};
   INT 		   Last_Message_len = 0;
   FILE 	   *lastmsgptr= NULL_PTR;

   /* Get timeout values */
   timer_reply_size = ptetimer_get_timer_reply( p_msg, lookup_key,
                                                app_data1, app_data2, buffer2,
                                                sizeof(buffer2) );

   /* Handle Echo Timer */
   /* ----------------- */
   if(strcmp(lookup_key,echo_lookup_key) == 0)
   {
      /* 1. Echo timer is automatically cleared
       * 2. Set Echo timer
       * 3. Send ECHO request to host
       */

      if((set_echo_timer(echo_lookup_key,echo_timer_length,NULL_PTR,application_que_name,1)) == false)
      {
		  strcpy( Buff, "Failed to send ECHO TEST to host." );
		  return(NOT_OK);
	  }

      memset (&auth_tx, 0, Auth_Tx_Size);
      if ( OK != perform_host_logon_request( ECHO_REQUEST,
                                             SOURCE_IS_OPERATOR,
                                             &auth_tx ) )
      {
         ncmcrd_log_message( 1, 3, "Error executing host Echo Test", "process_timeout_msg" , 0 ,"");
         return(NOT_OK);
      }

      return OK;
   }
   else if(strcmp(lookup_key,echo_request_timer_key) == 0)
   {
      /* An Echo request has timed out. Take no action here  */
      /* Allow the echoes to continue going out as normal.   */
      /* --------------------------------------------------- */

      return OK;
   }

   if ( timer_reply_size == sizeof(buffer2) )
   {
      memset( &auth_tx, 0,       Auth_Tx_Size );
      memcpy( &auth_tx, buffer2, Auth_Tx_Size );
   }
   else
      return(NOT_OK);

#ifdef MC11_OCT2023
   if( AUTH_LOGON == auth_tx.TLF01_details.tx_key &&
	  (strncmp(auth_tx.function_code, ECHO_NMI, 3) == 0) )
   {
	   memset( Buff, 0x00, sizeof(Buff) );
	   sprintf( Buff, "ECHO TEST response not received, Host status not changed as per AN7623 OCT23" );
	   //ncmcrd_log_message( 3, 1, Buff , "process_timeout_msg" , 0 , "");
	   return OK;
   }
#endif
   else if( AUTH_LOGON == auth_tx.TLF01_details.tx_key &&
		    ((0 == strncmp( auth_tx.function_code, LOGON_NMI_BIN,      3)) ||
		     (0 == strncmp( auth_tx.function_code, LOGON_NMI_GROUP_ID, 3)))
		  )
   {
	   memset( Buff, 0x00, sizeof(Buff) );
	   sprintf( Buff, "Logon to MC timed out " );
	   ncmcrd_log_message( 3, 1, Buff , "process_timeout_msg" , 0 , "");
	   return OK;
   }

   /* Display time out message on Monitor.
    * This message is used to isolate problems to selected bins
    * or to certain merchants.
    */

   /*************Last transaction to be updated in DB************/
    lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
   	if(lastmsgptr != NULL_PTR)
   	{
   		sprintf( last_tran__msg,
   			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
   			   auth_tx.TLF01_details.card_num,
   			   auth_tx.TLF01_details.sys_trace_audit_num,
   			   auth_tx.TLF01_details.terminal_id,
   			   auth_tx.TLF01_details.merchant_id,
   			   auth_tx.TLF01_details.acquirer_id,
   			   auth_tx.TLF01_details.handler_queue);

   		Last_Message_len=strlen(last_tran__msg);

   		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
   		fclose(lastmsgptr);
   	}
   if(true == ncmcrd_check_for_acq_refund_transaction(&auth_tx))
   {
	   /* Dont print for refund txn received by cadencie*/
   }
   else if(0 ==  strncmp(auth_tx.TLF01_details.product_codes[8].code,"PA",2))
   {
	   // drop the TO reversal sent for partially approved transaction
		strcpy(auth_tx.TLF01_details.response_text,"TO Reversal for partially approved txn");
		strncpy(auth_tx.TLF01_details.response_code,"01",2);
		if ( false == insert_tlf01( &auth_tx ))
		{
			strcpy(auth_tx.TLF01_details.response_code,"96");
			memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			sprintf(Log_Bufr," failed to send to send the txn for update. Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
			memcpy( auth_tx.TLF01_details.response_text, "Unable to update database tables.",
						sizeof(auth_tx.TLF01_details.response_text)-1 );
			ncmcrd_log_message( 3, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
		}
	   return OK;
   }
   else
   {
	   if(strncmp(lookup_key, last_lookup_key, strlen(lookup_key)) != 0 &&
	      strncmp(lookup_key, clear_timer_key, strlen(lookup_key)) != 0)
	   {
		   memset( Buff, 0x00, sizeof(Buff) );
		   sprintf( Buff,
				   "Txn to MC timed out. " );
		   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
							auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
		   strcat(Buff,Log_Bufr);
		   ncmcrd_log_message( 3, 1, Buff , "process_timeout_msg" , 0 ,auth_tx.TLF01_details.card_num);
	   }
   }

   /* Update the transaction in the auth_tx table to TIMED OUT */
   auth_tx.TLF01_details.general_status = GS_TXN_TIMEDOUT;

   /* Decrement the active transactions counter field */
   (void)decrement_active_count( AppName, Buff );

   /* Increment the number of times the host has failed */
   (void)increment_timeout_count( AppName, Buff, &auth_tx.TLF01_details);

   /* Update transaction statistics. */
   strcpy( auth_tx.TLF01_details.response_code, "19" );
   update_txn_stats( &Txn_Counts, &auth_tx, NA );

   /* If this is a Card File Update msg, let GUI know it timed out. */
   if ( 0 == strcmp( "0302", auth_tx.TLF01_details.message_type ) )
   {
      strcpy( auth_tx.TLF01_details.resp_source, "No response from host." );
      return( process_file_update_response( &auth_tx ) );
   }

#if BDOR_62_PREAUTH_VOID
   if(strncmp(auth_tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
   {
	   /* reponse sending is not required for parvbh request */
	   return (OK);
   }
#endif

   /* Do not perform standin processing for Log On/Off messages. */
   if ( AUTH_LOGON != auth_tx.TLF01_details.tx_key )
   {
	   if(true == ncmcrd_check_for_acq_refund_transaction(&auth_tx))
	   {
		   auth_tx.TLF01_details.tx_key =
		             match_request_to_response( auth_tx.TLF01_details.tx_key );

		   ret_code = ncmcrd_transfer_refund_response_msg_to_MC2(&auth_tx);
		   return ret_code;
	   }
	   else
	   {
		   if(strncmp(lookup_key, last_lookup_key, strlen(lookup_key)) == 0 ||
		      strncmp(lookup_key, clear_timer_key, strlen(lookup_key)) == 0)
		   {
			   /* Adjust the counters appropriately */
			   reset_timeout_count( AppName, Buff );

			   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			   sprintf(Log_Bufr,"Clearing timerds for repeated timeouts, Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
			   					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
			   ncmcrd_log_message( 3, 1, Log_Bufr, "process_timeout_msg" , 0, auth_tx.TLF01_details.card_num);
			   /* Send a PTE message to clear the request timer. */
			   if(clear_timeout_timer( &auth_tx ) == OK)
			   {
				   strncpy(clear_timer_key, lookup_key, strlen(lookup_key));
			   }
			   else
			   {
				   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				   sprintf(Log_Bufr,"Failed to clear the timerds for repeated timeouts, Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
				   					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				   ncmcrd_log_message( 3, 3, Log_Bufr, "process_timeout_msg" , 0, auth_tx.TLF01_details.card_num);
			   }
		   }
		   else
		   {
			   /* Prepare time out message for terminal handler. */
			   auth_tx.TLF01_details.tx_key = get_tran_type( &auth_tx );

			   /* Send time out message to authorizer. */
			   perform_standin_processing( "19", GS_TXN_TIMEDOUT, &auth_tx );

			   strncpy(last_lookup_key, lookup_key, strlen(lookup_key));
			   /* Send a PTE message to clear the request timer. */
			   if(clear_timeout_timer( &auth_tx ) == OK)
			   {
				   strncpy(clear_timer_key, lookup_key, strlen(lookup_key));
				   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				   sprintf(Log_Bufr,"Clearing timerds after timeout, Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
				   					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				   ncmcrd_log_message( 3, 1, Log_Bufr, "process_timeout_msg" , 0, auth_tx.TLF01_details.card_num);
			   }
			   else
			   {
				   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				   sprintf(Log_Bufr,"Failed to clear the timerds after timeout, Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
				   					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				   ncmcrd_log_message( 3, 3, Log_Bufr, "process_timeout_msg" , 0, auth_tx.TLF01_details.card_num);
			   }
		   }
	   }
   }
   return( ret_code );
 }  /* process_timeout_msg */


/******************************************************************************
 *
 *  NAME:         PROCESS_FILE_UPDATE_TXN
 *
 *  DESCRIPTION:  This function begins the process of sending an 0302 message
 *                to MasterCard.
 *
 *  INPUTS:       p_msg - This is the incoming message from the GUI.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT process_file_update_txn( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   LONG            data_len;
   INT             ret_code;
   CHAR            err_mesg[512] = {0};
   CHAR            time_str [ 7] = {0};
   CHAR            time_date[25] = {0};
   AUTH_TX         auth_tx;
   LONG            iStan;
   char			  last_tran__msg[256] = {0};
   INT 			  Last_Message_len = 0;
   FILE 		  *lastmsgptr= NULL_PTR;

   /* Copy transaction record into data structure. */
   memset( &Mcc_Gui, 0x00, Mcc_Gui_Size );
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);

   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &auth_tx, p_data_data,Auth_Tx_Size);	
   memcpy( &Mcc_Gui, p_data_data+(Auth_Tx_Size), Mcc_Gui_Size );

   /* Initialize the transaction. */
   (void)mcrdutil_init_txn( &auth_tx );

   /*************Last transaction to be updated in DB************/
    lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
   	if(lastmsgptr != NULL_PTR)
   	{
   		sprintf( last_tran__msg,
   			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
   			   auth_tx.TLF01_details.card_num,
   			   auth_tx.TLF01_details.sys_trace_audit_num,
   			   auth_tx.TLF01_details.terminal_id,
   			   auth_tx.TLF01_details.merchant_id,
   			   auth_tx.TLF01_details.acquirer_id,
   			   auth_tx.TLF01_details.handler_queue);

   		Last_Message_len=strlen(last_tran__msg);

   		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
   		fclose(lastmsgptr);
   	}

   /* Get originating queue name. */
   strcpy( auth_tx.TLF01_details.originator_queue,
           ptemsg_get_msg_orig_queue ( p_msg )    );
   strcpy( auth_tx.TLF01_details.originator_info,
           ptemsg_get_msg_orig_comms_info( p_msg ));
   strcpy( auth_tx.acct_id_1, Mcc_Gui.msg_type );

   /* Store data into auth_tx for function Build_Host_Structure. */
   (void)map_0302_to_tlf01( &auth_tx );
   strncpy( auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,
            sizeof(ncf01_i.tpdu_id) );
   strcpy( auth_tx.TLF01_details.message_type, "0302" );
	if(0 == strcmp( NULL_STR, auth_tx.TLF01_details.sys_trace_audit_num ))
	{
	   if ( 0 == strcmp( NULL_STR, Stan ) )
	   {
		  ptetime_get_timestamp( time_date );
		  get_time( time_date, time_str );
		  strcpy( Stan, time_str );
	   }
	   else
	   {
		  iStan = atol( Stan ) + 1;
		  sprintf( Stan, "%06ld", iStan );
	   }
	   Rj_with_lead_zeros( Stan, sizeof(Stan)-1 );
	   strncpy( auth_tx.TLF01_details.sys_trace_audit_num, Stan, sizeof(Stan)-1 );
	}
   auth_tx.TLF01_details.general_status = GS_TXN_NEW;
   auth_tx.TLF01_details.tx_key         = AUTH_CARD_UPDATE;

   /* Confirm host is available */
   ret_code = verify_host_app_status( &auth_tx );
   if( OK != ret_code )
   {
      /* Host is not available.  Send response to GUI. */
      strcpy( auth_tx.TLF01_details.response_text, "Host Inactive" );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Host Inactive Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
      					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(err_mesg,Log_Bufr);
      ncmcrd_log_message( 3, 2, err_mesg , "process_file_update_txn" , 0 ,auth_tx.TLF01_details.card_num);

      strcpy( Mcc_Gui.response_code, "96" );
      strcpy( Mcc_Gui.err_msg1, "Host is either unavailable or too busy.\n" );
      strcpy( Mcc_Gui.err_msg2, "Try again later or may need to log on."    );
	  auth_tx.TLF01_details.tx_key         = AUTH_CARD_UPDATE+100;     
	  ret_code = send_response_to_GUI( auth_tx.acct_id_1, &auth_tx );
	  if(0 == strncmp( "ncmcr2A", auth_tx.TLF01_details.originator_queue,6 ))
		  return(ret_code);
      /* Log to TLF01 */
      (void)map_0312_to_tlf01( &auth_tx );
      (void)insert_into_TLF01( &auth_tx );
   }
   else
   {
      /* Host is working.  Build PTEMSG's to be sent */
      ret_code = send_request_to_host( &auth_tx );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_FILE_UPDATE_RESPONSE
 *
 *  DESCRIPTION:  This function handles 0312 messages.  These are the responses
 *                to 0302 messages - File Update Requests where card holder
 *                records can be added, deleted, updated, or inquired.
 *
 *  INPUTS:       p_auth_tx - Structure containing most of the transacation data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT process_file_update_response( pAUTH_TX p_auth_tx )
{
   CHAR  err_mesg[512] = {0};

   strcpy( Mcc_Gui.response_code, p_auth_tx->TLF01_details.response_code );

   if ( 0 != strcmp(Mcc_Gui.response_code, APPROVED) )
   {
      if ( 0 == strcmp(Mcc_Gui.response_code,"19") )
      {
         strcpy( Mcc_Gui.err_msg1, "No response from host" );
         strcpy( err_mesg, Mcc_Gui.err_msg1 );
      }
      else
      {
         /* Error with File Update Request - Field 120.
          * See error code in field 44.
          */
         (void)get_file_update_err_msg( p_auth_tx );
         sprintf( err_mesg,"File update request format error: %s",
                  p_auth_tx->TLF01_details.resp_source );
      }
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, err_mesg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_mesg,Log_Bufr);
      ncmcrd_log_message( 3, 2, err_mesg , "process_file_update_response" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   /* Log to TLF01 */
   (void)map_0312_to_tlf01( p_auth_tx );
   (void)insert_into_TLF01( p_auth_tx );

   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.tx_key     = AUTH_CARD_UPDATE+100;

   /* Send message to GUI - even if there was an error. */
   return( send_response_to_GUI( p_auth_tx->acct_id_1, p_auth_tx ) );
}


/******************************************************************************
 *
 *  NAME:         MAP_0302_TO_TLF01
 *
 *  DESCRIPTION:  This function copies information stored in the 0302 message
 *                structure into the TLF01 structure for logging purposes.
 *
 *  INPUTS:       (Global) Mcc_Gui
 *
 *  OUTPUTS:      p_auth_Tx
 *
 *  RTRN VALUE:   Always returns OK
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT map_0302_to_tlf01( pAUTH_TX p_auth_tx )
{
   INT  purge_date_size;

   purge_date_size = sizeof( Mcc_Gui.region_us.purge_date ) - 1;

 if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.card_num ) )
   memcpy( p_auth_tx->TLF01_details.card_num, Mcc_Gui.card_nbr, 
           sizeof(p_auth_tx->TLF01_details.card_num)-1 );

   memcpy( p_auth_tx->TLF01_details.processing_code, Mcc_Gui.msg_type,
           sizeof(p_auth_tx->TLF01_details.processing_code)-1 );

   strcpy( p_auth_tx->TLF01_details.terminal_id, Mcc_Gui.terminal_id );

   p_auth_tx->TLF01_details.nii[0] = Mcc_Gui.choice;

   p_auth_tx->TLF01_details.update_mask[0] = Mcc_Gui.reason;

   memcpy( p_auth_tx->TLF01_details.product_codes[0].code, Mcc_Gui.product_code,
           sizeof(Mcc_Gui.product_code) );

   memcpy( p_auth_tx->TLF01_details.agent_id,   Mcc_Gui.activity_date, 6 );
   memcpy( p_auth_tx->TLF01_details.agent_id+6, Mcc_Gui.activity_time, 4 );

   memcpy( p_auth_tx->TLF01_details.total_amount, Mcc_Gui.vip_limit_amt,
           sizeof(p_auth_tx->TLF01_details.total_amount)-1 );

   memcpy( p_auth_tx->TLF01_details.currency_code, Mcc_Gui.vip_currency_code,
           sizeof(p_auth_tx->TLF01_details.currency_code)-1 );

   memcpy( p_auth_tx->TLF01_details.nbr_of_prod_codes,
           Mcc_Gui.number_of_regions, 2 );

   memcpy( p_auth_tx->TLF01_details.product_codes[0].amount,
           Mcc_Gui.region_us.purge_date,
           purge_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[1].amount,
           Mcc_Gui.region_canada.purge_date,
           purge_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[2].amount,
           Mcc_Gui.region_sa.purge_date,
           purge_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[3].amount,
           Mcc_Gui.region_asia.purge_date,
           purge_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[4].amount,
           Mcc_Gui.region_europe.purge_date,
           purge_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[5].amount,
           Mcc_Gui.region_africa.purge_date,
           purge_date_size );

   /* Workstation and Operator ID */
   strcpy( p_auth_tx->TLF01_details.workstation, Mcc_Gui.workstation );
   strcpy( p_auth_tx->TLF01_details.operator_id, Mcc_Gui.operator_id );

   return(OK);
}


/******************************************************************************
 *
 *  NAME:         MAP_0312_TO_TLF01
 *
 *  DESCRIPTION:  This function copies information stored in the 0312 message
 *                structure into the TLF01 structure for logging purposes.
 *
 *  INPUTS:       (Global) Mcc_Gui
 *
 *  OUTPUTS:      p_auth_tx
 *
 *  RTRN VALUE:   Always returns OK
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT map_0312_to_tlf01( pAUTH_TX p_auth_tx )
{
   INT  chargeback_date_size;
   INT  purge_date_size;

   purge_date_size      = sizeof( Mcc_Gui.region_us.purge_date      ) - 1;
   chargeback_date_size = sizeof( Mcc_Gui.region_us.chargeback_date ) - 1;

   if ( p_auth_tx->TLF01_details.product_codes[0].code[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[0].code,
              Mcc_Gui.product_code,
              sizeof(Mcc_Gui.product_code) );

   if ( Mcc_Gui.activity_date[0] != 0x00 )
   {
      memcpy( p_auth_tx->TLF01_details.agent_id,   Mcc_Gui.activity_date, 6 );
      memcpy( p_auth_tx->TLF01_details.agent_id+6, Mcc_Gui.activity_time, 4 );
   }

   if ( Mcc_Gui.vip_limit_amt[0] != 0x00 )
      memcpy( p_auth_tx->TLF01_details.total_amount, Mcc_Gui.vip_limit_amt,
              sizeof(p_auth_tx->TLF01_details.total_amount)-1 );

   if ( Mcc_Gui.vip_currency_code[0] != 0x00 )
      memcpy( p_auth_tx->TLF01_details.currency_code, Mcc_Gui.vip_currency_code,
              sizeof(p_auth_tx->TLF01_details.currency_code)-1 );

   if ( p_auth_tx->TLF01_details.nbr_of_prod_codes[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.nbr_of_prod_codes,
              Mcc_Gui.number_of_regions, 2 );


   if ( p_auth_tx->TLF01_details.product_codes[0].amount[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[0].amount,
              Mcc_Gui.region_us.purge_date,
              purge_date_size );

   if ( p_auth_tx->TLF01_details.product_codes[1].amount[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[1].amount,
              Mcc_Gui.region_canada.purge_date,
              purge_date_size );

   if ( p_auth_tx->TLF01_details.product_codes[2].amount[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[2].amount,
              Mcc_Gui.region_sa.purge_date,
              purge_date_size );

   if ( p_auth_tx->TLF01_details.product_codes[3].amount[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[3].amount,
              Mcc_Gui.region_asia.purge_date,
              purge_date_size );

   if ( p_auth_tx->TLF01_details.product_codes[4].amount[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[4].amount,
              Mcc_Gui.region_europe.purge_date,
              purge_date_size );

   if ( p_auth_tx->TLF01_details.product_codes[5].amount[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.product_codes[5].amount,
              Mcc_Gui.region_africa.purge_date,
              purge_date_size );


   /* Response Information */

   /* Error Message */
   memcpy( p_auth_tx->TLF01_details.response_text, Mcc_Gui.err_msg1,
           sizeof(p_auth_tx->TLF01_details.response_text)-1 );

   /* Chargeback Dates */
   memcpy( p_auth_tx->TLF01_details.product_codes[6].amount,
           Mcc_Gui.region_us.chargeback_date,
           chargeback_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[7].amount,
           Mcc_Gui.region_canada.chargeback_date,
           chargeback_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[8].amount,
           Mcc_Gui.region_sa.chargeback_date,
           chargeback_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[9].amount,
           Mcc_Gui.region_asia.chargeback_date,
           chargeback_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[10].amount,
           Mcc_Gui.region_europe.chargeback_date,
           chargeback_date_size );

   memcpy( p_auth_tx->TLF01_details.product_codes[11].amount,
           Mcc_Gui.region_africa.chargeback_date,
           chargeback_date_size );

   return(OK);
}


/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_TERM_APP
 *
 *  DESCRIPTION:  This function builds and sends a message to the terminal
 *                handler.  It sends what is in auth_tx.  Typically this
 *                is a response from the MasterCard host.
 *
 *  INPUTS:       p_auth_tx - Transaction record for terminal handler
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/23/99 - Removed code to adjust shared memory counters.
 ******************************************************************************/
INT send_response_to_term_app( pAUTH_TX p_auth_tx )
{
   INT   ret_code;
   CHAR  err_msg[512]={0};

   /* Send reponse to the authorizer */
   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE,
                                      application_que_name, err_msg );
   if ( OK != ret_code )
   {
	   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
						p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
					 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
	   strcat(err_msg,Log_Bufr);
	   ncmcrd_log_message( 3, 2, err_msg , "send_response_to_term_app" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
} /* send_response_to_term_app */


/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_GUI
 *
 *  DESCRIPTION:  This function builds and sends a message to the applink
 *                queue.  It sends what is in Mcc_Gui.  Typically this
 *                is a response from the MasterCard host to an 0302 msg.
 *
 *  INPUTS:       filename - MCC103 or MCC102
 *                p_auth_tx- contains originator information
 *                Mcc_Gui  - (Global) Account Management response info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_response_to_GUI( pBYTE filename, pAUTH_TX p_auth_tx )
{
   INT         ret_code  = NOT_OK;
   CHAR        err_msg[512]={0};
   BYTE        app_data_type;
   BYTE        buffer1[sizeof(AUTH_TX)+sizeof(MCC_ONLINE_UPDATES)];

   /* Determime application data type to send with response. */
   if ( 0 == strcmp( filename, "MCC102" ) )
      app_data_type = MCC102_DATA;
   else if ( 0 == strcmp( filename, "MCC103" ) )
      app_data_type = MCC103_DATA;
   else
      app_data_type = 0;

   /* Build the message to be sent to applink */
   memset( buffer1, 0, sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );	
   memcpy( buffer1+Auth_Tx_Size, &Mcc_Gui, Mcc_Gui_Size );
   ret_code = build_and_send_request_message( MT_INCOMING_GUI, 
                                      ST1_NONE,
                                      ST2_NONE,
                                      p_auth_tx->TLF01_details.originator_queue,
                                      (pBYTE)&buffer1,
                                      sizeof( buffer1 ),
                                      app_data_type,
                                      p_auth_tx->TLF01_details.originator_info,
                                      err_msg );

   if ( OK != ret_code )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s Gui card nbr : %s",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num,Mcc_Gui.card_nbr);
      memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_msg,Log_Bufr);
      ncmcrd_log_message( 3, 3, err_msg , "send_response_to_GUI" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
} /* send_response_to_GUI */


/******************************************************************************
 *
 *  NAME:         PROCESS_REVERSAL_ADVICE
 *
 *  DESCRIPTION:  This procedure handles 0420 messages sent by the MasterCard
 *                host.  We respond here with an 0430 and pass a reversal
 *                message to Transaction Control.  Transaction Control's
 *                response will be ignored.
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
void process_reversal_advice( pAUTH_TX p_auth_tx )
{
   char      tempstr[100];
   AUTH_TX   auth_tx;      /* This one will get sent to the host. */

   /*+------------------------------------+
     | Display reason for advice message. |
     +------------------------------------+*/
   strcpy( tempstr, "** Reversal Advice 0420 Err = " );
   strcat( tempstr, reject_advice_code );

   if ( 0 != strcmp( NULL_STR, reject_detail_code ) )
   {
      strcat( tempstr, ", Detail Code = " );
      strcat( tempstr, reject_detail_code );
      if ( 0 != strcmp( NULL_STR, reject_text ) )
      {
         strcat( tempstr, ", "        );
         strcat( tempstr, reject_text );
      }
   }

   memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcrd_log_message( 1, 1, tempstr , "process_reversal_advice" , 0 ,"");

   /*+--------------------------------------------+
     | Build a 0430 message to reply to the 0420. |
     +--------------------------------------------+*/
   strncpy( p_auth_tx->TLF01_details.message_type, "0430", 4 );
   p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   strcpy( p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id );
   strcpy( p_auth_tx->TLF01_details.response_code, APPROVED    );
   Tran_Flow = SWITCHED_IN;

   /*+------------------------+
     | Send 0430 to the host. |
     +------------------------+*/
   if ( OK != incoming_other_host_response( p_auth_tx ) )
   {
      strcpy( tempstr, "Unable to send 0430 message to MCard host." );
      sprintf(Log_Bufr," Unable to send 0430 message to MCard host.  Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 1, 2, Log_Bufr , "process_reversal_advice" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   /*+--------------------------+
     | Send 0400 to authorizer. |
     +--------------------------+*/

   /* Initialize the transaction. */
   memset( &auth_tx, 0x00, Auth_Tx_Size );
   (void)mcrdutil_init_txn( &auth_tx );

   strcpy( auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id );
   strncpy( auth_tx.TLF01_details.message_type, "0400", 4 );
   auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;

   strcpy( auth_tx.TLF01_details.card_num, p_auth_tx->TLF01_details.card_num );
   strcpy( auth_tx.TLF01_details.processing_code,
           p_auth_tx->TLF01_details.processing_code );
   strcpy( auth_tx.TLF01_details.total_amount,
           p_auth_tx->TLF01_details.total_amount );
   strcpy( auth_tx.TLF01_details.transmission_timestamp,
           p_auth_tx->TLF01_details.transmission_timestamp );
   strcpy( auth_tx.TLF01_details.acquiring_id,
           p_auth_tx->TLF01_details.acquiring_id );
   strcpy( auth_tx.TLF01_details.forwarding_institution_id,
           p_auth_tx->TLF01_details.forwarding_institution_id );
   strcpy( auth_tx.TLF01_details.retrieval_ref_num,
           p_auth_tx->TLF01_details.retrieval_ref_num );
   strcpy( auth_tx.TLF01_details.currency_code,
           p_auth_tx->TLF01_details.currency_code );
   strncpy( auth_tx.TLF01_details.sys_trace_audit_num,
            p_auth_tx->TLF01_details.sys_trace_audit_num, bf11_len );
   strncpy( auth_tx.TLF01_details.response_code, APPROVED, 2 );
   strncpy( auth_tx.TLF01_details.mcard_banknet_len,
            p_auth_tx->TLF01_details.mcard_banknet_len,
            sizeof( HOST_MESSAGE.banknet_len ) );
   strcpy( auth_tx.TLF01_details.mcard_banknet,
           p_auth_tx->TLF01_details.mcard_banknet );

   strcpy( auth_tx.TLF01_details.originator_queue,
              p_auth_tx->TLF01_details.originator_queue );
   strcpy( auth_tx.TLF01_details.originator_info,
              p_auth_tx->TLF01_details.originator_info );

   /* Auth_Tx has been built.  Send request to get original transaction. */
   (void)get_orig_txn( &auth_tx );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_REVERSAL_ADVICE_DB_REPLY
 *
 *  DESCRIPTION:  This procedure is called after the original transaction has
 *                has been obtained from NCF30.  It passes auth_tx on to
 *                transaction control.  Then deletes the original transaction
 *                from NCF30.  Transaction Control's response will be ignored.
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
void process_reversal_advice_db_reply( pBYTE pCurrent )
{
   AUTH_TX     auth_tx;
   INT         ret_code;
   CHAR        tempstr[100];

   if ( NULL != pCurrent )
   {
      /* Get data server message */
      memset( &ncf30, 0, Ncf30_Size);
      memcpy( &ncf30, pCurrent+(Auth_Tx_Size), Ncf30_Size);

      memset( &auth_tx, 0x00, Auth_Tx_Size );
      memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
   }

   /* Copy data from NCF30's original txn into auth_tx. */
   memcpy( auth_tx.TLF01_details.orig_amount, ncf30.tran_amount,
           sizeof(auth_tx.TLF01_details.orig_amount) );
   memcpy( auth_tx.TLF01_details.time_hhmmss, ncf30.tran_time,
           sizeof(auth_tx.TLF01_details.time_hhmmss) );

   memcpy( auth_tx.TLF01_details.date_yyyymmdd, ncf30.tran_date,
           sizeof(auth_tx.TLF01_details.date_yyyymmdd) );
   memcpy( auth_tx.TLF01_details.orig_retrieval_ref_num,
           ncf30.primary_key.retrieval_ref_num,
           sizeof(auth_tx.TLF01_details.orig_retrieval_ref_num) );

   memcpy( auth_tx.TLF01_details.auth_number,ncf30.auth_number,
           sizeof(auth_tx.TLF01_details.auth_number) );
   memcpy( auth_tx.TLF01_details.response_code,ncf30.response_code,
           sizeof(auth_tx.TLF01_details.response_code) );

   memcpy( auth_tx.TLF01_details.terminal_id,ncf30.device_id,
           sizeof(auth_tx.TLF01_details.terminal_id) );
   memcpy( auth_tx.TLF01_details.orig_message,ncf30.message_type,
           sizeof(auth_tx.TLF01_details.orig_message) );

   memcpy( auth_tx.TLF01_details.orig_sys_trace_num,ncf30.sys_trace_audit_nbr,
           sizeof(auth_tx.TLF01_details.orig_sys_trace_num) );
   memcpy( auth_tx.TLF01_details.settlement_date,ncf30.settlement_date,
           sizeof(auth_tx.TLF01_details.settlement_date) );

   memcpy( auth_tx.TLF01_details.tran_fee_amount,ncf30.tran_fee_amount,
           sizeof(auth_tx.TLF01_details.tran_fee_amount) );
   memcpy( auth_tx.TLF01_details.mcard_banknet,ncf30.network_data,
           sizeof(auth_tx.TLF01_details.mcard_banknet) );

   memcpy( auth_tx.TLF01_details.transmission_timestamp,
           ncf30.transmission_timestamp,
           sizeof(auth_tx.TLF01_details.transmission_timestamp) );


   /* Send reversal to the authorizer */
   ret_code = mcrdutil_transfer_ctl_to_auth_app( &auth_tx );
   if ( OK != ret_code )
   {
      strcpy( tempstr, "Unable to send reversal request to authorizer." );
      memcpy( auth_tx.TLF01_details.response_text, tempstr,
                    sizeof(auth_tx.TLF01_details.response_text)-1 );
      ncmcrd_log_message( 1, 2, tempstr , "process_reversal_advice_db_reply" , 0 ,"");
   }

   /* Delete original transaction from NCF30. */
   (void)delete_ncf30( &ncf30 );

}  /* process_reversal_advice_db_reply */


/******************************************************************************
 *
 *  NAME:         GET_ORIG_TXN
 *
 *  DESCRIPTION:  This procedure builds and sends the request to the data
 *                server to fetch a transaction that is being reversed.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT  get_orig_txn( pAUTH_TX  p_auth_tx )
{
   INT             ret_code = NOT_OK;
   NCF30           ncf30;
   CHAR            err_msg[100];
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];

   /* Set up network identifying information for this transaction */
   memset( err_msg, 0x00, sizeof(err_msg) );
   memset( &ncf30, 0, Ncf30_Size );
   strcpy( ncf30.primary_key.network_id, AppName );
   strcpy( ncf30.primary_key.network_type, "A" );
   strcpy( ncf30.primary_key.retrieval_ref_num,
           p_auth_tx->TLF01_details.retrieval_ref_num );
   strcpy( ncf30.currency_code,   p_auth_tx->TLF01_details.currency_code   );
   strcpy( ncf30.processing_code, p_auth_tx->TLF01_details.processing_code );
   strcpy( ncf30.tran_amount,     p_auth_tx->TLF01_details.total_amount    );
   strcpy( ncf30.card_nbr,        p_auth_tx->TLF01_details.card_num        );

   /* Build message to get host specific data from data server. */
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );

   ret_code = build_and_send_request_message( MT_DB_REQUEST, 
                                              ST1_DB_SELECT, 
                                              ST2_DB_SELECT_TRAN_BY_CARD_NUM,
                                              netds_que_name, 
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1), 
                                              NCF30_DATA,
                                              NULL_STR,
                                              err_msg );
   if ( OK != ret_code  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_msg,Log_Bufr);
      ncmcrd_log_message( 3, 3, err_msg , "get_orig_txn" , 0 ,p_auth_tx->TLF01_details.card_num);

   }
   return( ret_code );
}


/******************************************************************************
 *
 *  NAME:         DELETE_NCF30
 *
 *  DESCRIPTION:  This procedure builds and sends the request to the data
 *                server to fetch a transaction that is being reversed.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT  delete_ncf30( pNCF30  p_ncf30 )
{
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = NOT_OK;
   CHAR            err_msg[512]={0};
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];

   /* Build message to get host specific data from data server. */
   memcpy( buffer1+Auth_Tx_Size,p_ncf30, Ncf30_Size );

   ret_code = build_and_send_request_message( MT_DB_REQUEST, 
                                              ST1_NONE, 
                                              ST2_NONE,
                                              netds_que_name, 
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1), 
                                              ALL_DATA,
                                              NULL_STR,
                                              err_msg );
   if ( OK != ret_code  )
   {
      ncmcrd_log_message( 3, 3, err_msg , "delete_ncf30" , 0 ,"");
   }
   return( ret_code );
}



/******************************************************************************
 *
 *  NAME:         PROCESS_REJECT
 *
 *  DESCRIPTION:  This procedure is called if MasterCard replies to us
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

   memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcrd_log_message( 1, 2, tempstr , "process_reject" , 0 ,"");

   /* The STAN is passed to us in 0620 messages. */
   strcpy( tempstr, "System Trace Audit Number of rejected message is: " );
   strcat( tempstr, p_auth_tx->TLF01_details.sys_trace_audit_num );

   sprintf(Log_Bufr," System Trace Audit Number of rejected message is:  STAN : %s , Term Id:%s ,Merch Id: %s RRN: %s ",
		   p_auth_tx->TLF01_details.sys_trace_audit_num,p_auth_tx->TLF01_details.terminal_id,
		   p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
   memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcrd_log_message( 1, 2, Log_Bufr , "process_reject" , 0 ,p_auth_tx->TLF01_details.card_num);

   /*+--------------------------------------------+
     | Build a 0630 message to reply to the 0620. |
     +--------------------------------------------+*/
   decrement_active_count( AppName, tempstr );
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   /* Initialize the transaction. */
   (void)mcrdutil_init_txn( &auth_tx );
   auth_tx.TLF01_details.entry_type = ADMINISTRATIVE;

   strncpy( auth_tx.TLF01_details.message_type, "0630", 4 );

   strncpy( auth_tx.TLF01_details.transmission_timestamp,
            p_auth_tx->TLF01_details.transmission_timestamp, bf7_len );

   strncpy( auth_tx.TLF01_details.sys_trace_audit_num,
            p_auth_tx->TLF01_details.sys_trace_audit_num, bf11_len );

   strcpy( auth_tx.TLF01_details.forwarding_institution_id,
           p_auth_tx->TLF01_details.forwarding_institution_id );

   strncpy( auth_tx.TLF01_details.response_code, APPROVED, 2 );

   strcpy( auth_tx.int_net_facility_data, p_auth_tx->int_net_facility_data );

   strncpy( auth_tx.TLF01_details.mcard_banknet_len,
            p_auth_tx->TLF01_details.mcard_banknet_len,
            sizeof( HOST_MESSAGE.banknet_len ) );
   strcpy( auth_tx.TLF01_details.mcard_banknet,
           p_auth_tx->TLF01_details.mcard_banknet );

   auth_tx.TLF01_details.tx_key = AUTH_ADMIN_ADVICE_RESPONSE;
   strcpy( auth_tx.TLF01_details.originator_queue, dialog_que_name );
   strcpy( auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id );

	/*Use vehicle_number field to save reject reason code and detail code*/
   memcpy( auth_tx.TLF01_details.vehicle_number, reject_advice_code, 3 );
   memcpy( &auth_tx.TLF01_details.vehicle_number[3], reject_detail_code, 4 );

   /*+------------------------+
     | Send 0630 to the host. |
     +------------------------+*/
   strcpy( auth_tx.TLF01_details.originator_queue,
              p_auth_tx->TLF01_details.originator_queue );
   strcpy( auth_tx.TLF01_details.originator_info,
              p_auth_tx->TLF01_details.originator_info );
   if ( OK != incoming_other_host_response( &auth_tx ) )
   {
      strcpy( tempstr, "Unable to send 0630 message to MCard host." );
      sprintf(Log_Bufr," Unable to send 0630 message to MCard host. Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 1, 2, Log_Bufr , "process_reject" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

}  /* process_reject */


/******************************************************************************
 *
 *  NAME:         PROCESS_0190
 *
 *  DESCRIPTION:  This procedure is called when an 0190 message is received
 *                from the host.  This is a authorization response negative
 *                acknowledgement message generated by the host.  This function
 *                will display the response code and any textual message that
 *                was received in field 44.  No further processing is required.
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
void process_0190( pAUTH_TX p_auth_tx )
{
   char      tempstr[100];

   /*+------------------+
     | Display message. |
     +------------------+*/
   strcpy( tempstr, "** M/C NEG ACK (0190) - Response code = " );
   strcat( tempstr, p_auth_tx->TLF01_details.response_code );
   strcat( tempstr, ".  " );
   strcat( tempstr, p_auth_tx->TLF01_details.resp_source );
   strcat( tempstr, ". Field 11=" );
   strcat( tempstr, p_auth_tx->TLF01_details.sys_trace_audit_num );

   memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcrd_log_message( 1, 2, tempstr , "process_0190" , 0 ,"");

}  /* process_network_advice */


/******************************************************************************
 *
 *  NAME:         PROCESS_UNSUPPORTED_TXN
 *
 *  DESCRIPTION:  This procedure is called to handle unsupported transactions
 *                from the network.
 *                
 *
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_unsupported_txn( pAUTH_TX p_auth_tx )
{
   CHAR      tempstr[512] = {0};
   AUTH_TX   auth_tx;
   INT       ret_val = OK;

   /*+-------------------------+
     | Send out error message. |
     +-------------------------+*/
   strcpy( tempstr, "Unsupported transaction from MasterCard. Message type = ");
   strcat( tempstr, p_auth_tx->TLF01_details.message_type );
   strcat( tempstr, ", Processing code = " );
   strcat( tempstr, p_auth_tx->TLF01_details.processing_code );

   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
   					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
   memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   strcat(tempstr,Log_Bufr);
   ncmcrd_log_message( 1, 1, tempstr , "process_unsupported_txn" , 0 ,p_auth_tx->TLF01_details.card_num);

   /*+------------------------+
     | Build a reply message. |
     +------------------------+*/
   Tran_Flow = SWITCHED_IN;
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   /* Initialize the transaction. */
   (void)mcrdutil_init_txn( &auth_tx );
   p_auth_tx->TLF01_details.entry_type = FINANCIAL;

   strncpy( p_auth_tx->TLF01_details.message_type, "0110", 4 );

   strncpy( p_auth_tx->TLF01_details.response_code, "12", 2 );

   p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
   p_auth_tx->TLF01_details.general_status = auth_tx.TLF01_details.general_status;
   strcpy( p_auth_tx->TLF01_details.originator_queue, dialog_que_name );
   strcpy( p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id );
   strcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
           auth_tx.TLF01_details.primary_key.transaction_id );
   strcpy( p_auth_tx->TLF01_details.dispensation.origin, "RO" );
   strcpy( p_auth_tx->TLF01_details.dispensation.auth_2, "RA" );
   strcpy( p_auth_tx->TLF01_details.handler_queue, application_que_name);
   strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, auth_tx.TLF01_details.date_yyyymmdd);
   strcpy( p_auth_tx->TLF01_details.time_hhmmss,   auth_tx.TLF01_details.time_hhmmss);


   /*+----------------------------+
     | Send response to the host. |
     +----------------------------+*/
   if ( OK != incoming_other_host_response( p_auth_tx ) )
   {
      strcpy( tempstr, "Unable to send response message to MCard host." );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(tempstr,Log_Bufr);
      ncmcrd_log_message( 1, 1, tempstr , "process_unsupported_txn" , 0 ,p_auth_tx->TLF01_details.card_num);
      ret_val = NOT_OK;
   }
   return( ret_val );
}  /* process_reject */


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
 *  RTRN VALUE:   OK if all goes as expected
 *                NOT_OK if tran_type is invalid
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT alter_transaction_data( pAUTH_TX p_auth_tx_in, pAUTH_TX p_auth_tx_out )
{
   INT   len;   /* Length of EMV block in response message. */

   /* Check for a valid transaction. */
   if ( AUTH_INVALID == p_auth_tx_in->TLF01_details.tx_key )
   {
      /* This transaction is not understood by the server and will be dropped */
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx_in->TLF01_details.terminal_id,
    		  p_auth_tx_in->TLF01_details.merchant_id,p_auth_tx_in->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx_in->TLF01_details.response_text, "Invalid transaction key",
                    sizeof(p_auth_tx_in->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 1, 2, Log_Bufr , "alter_transaction_data" , 0 ,p_auth_tx_in->TLF01_details.card_num);

	  return( NOT_OK );
   }

   if ( AUTH_BALANCE_INQUIRY_RESPONSE == p_auth_tx_in->TLF01_details.tx_key )
   {
      /* Put new amount into the auth_tx table. */
      strcpy( p_auth_tx_out->TLF01_details.total_amount,
              p_auth_tx_in->TLF01_details.total_amount );
   }

   /* Update the original auth_tx structure. */
   p_auth_tx_out->TLF01_details.general_status = GS_TXN_COMPLETED;

   /*
    * Copy essential data from auth_tx_in into auth_tx_out.
    */
   if(p_auth_tx_in->TLF01_details.auth_number[0] != '\0')
   {
	   strncpy( p_auth_tx_out->TLF01_details.auth_number,
	           p_auth_tx_in->TLF01_details.auth_number ,6);

   }

   strcpy( p_auth_tx_out->TLF01_details.response_code,
           p_auth_tx_in->TLF01_details.response_code );

   if ( p_auth_tx_in->TLF01_details.response_text[0] != 0x00 )
      strcpy( p_auth_tx_out->TLF01_details.response_text,
              p_auth_tx_in->TLF01_details.response_text );

   if ( 0 != strcmp( p_auth_tx_in->MCF01_details.currency_code, NULL_STR ) )
      strcpy( p_auth_tx_out->MCF01_details.currency_code,
              p_auth_tx_in->MCF01_details.currency_code );

   strcpy( p_auth_tx_out->TLF01_details.resp_source_len,
           p_auth_tx_in->TLF01_details.resp_source_len );

   strcpy( p_auth_tx_out->TLF01_details.resp_source,
           p_auth_tx_in->TLF01_details.resp_source );

   p_auth_tx_out->TLF01_details.cvc = p_auth_tx_in->TLF01_details.cvc;

   if(p_auth_tx_in->TLF01_details.mcard_banknet_len[0] != '\0' ||
	  p_auth_tx_in->TLF01_details.mcard_banknet[0] != '\0' )
   {
	   strcpy( p_auth_tx_out->TLF01_details.mcard_banknet_len,
	           p_auth_tx_in->TLF01_details.mcard_banknet_len );
	   strncpy( p_auth_tx_out->TLF01_details.mcard_banknet,
	           p_auth_tx_in->TLF01_details.mcard_banknet , 9);
   }

   strcpy( p_auth_tx_out->TLF01_details.agent_id_len,
           p_auth_tx_in->TLF01_details.agent_id_len );

   strcpy( p_auth_tx_out->TLF01_details.agent_id,
           p_auth_tx_in->TLF01_details.agent_id);

   strcpy( p_auth_tx_out->TLF01_details.host_finish_time,
           p_auth_tx_in->TLF01_details.host_finish_time );

   if ( strlen(p_auth_tx_in->TLF01_details.settlement_date)>0 )
      memcpy( p_auth_tx_out->TLF01_details.settlement_date,
              p_auth_tx_in->TLF01_details.settlement_date, 4 );

   if ( p_auth_tx_out->TLF01_details.exp_date[0] == 0x00 )
      memcpy( p_auth_tx_out->TLF01_details.exp_date,
              p_auth_tx_in->TLF01_details.exp_date, 4 );

   if ( (p_auth_tx_in->EMV_details.emv_block[0] != 0x00) ||
        (p_auth_tx_in->EMV_details.emv_block[1] != 0x00)  )
   {
      /*+-----------------------------------------------------------+
        | Put EMV block from response into emv_block of original    |
        | request message.  Then append the block from the request. |
        +-----------------------------------------------------------+*/

      /* Get length of EMV response from host. */
      len = genutil_bcd_to_int( p_auth_tx_in->EMV_details.emv_block, 2 );

      /* Copy the response EMV data from host into the 'future' fields
       * of the EMV structure.  There are 3 of them.  Bytes 1 and 2 of
       * future field1 will hold the total length.
       */
      memcpy( p_auth_tx_out->EMV_details.future_field1,
              p_auth_tx_in->EMV_details.emv_block,
              sizeof(p_auth_tx_out->EMV_details.future_field1)-1 );

      if ( len > (sizeof(p_auth_tx_out->EMV_details.future_field1)-3) )
      {
         memcpy( p_auth_tx_out->EMV_details.future_field2,
                 p_auth_tx_in->EMV_details.emv_block+2
                 + sizeof(p_auth_tx_out->EMV_details.future_field1)-3,
                 sizeof(p_auth_tx_out->EMV_details.future_field2)-1 );

         if ( len > ((sizeof(p_auth_tx_out->EMV_details.future_field1)-3)
                    +(sizeof(p_auth_tx_out->EMV_details.future_field2)-1)) )
         {
            memcpy( p_auth_tx_out->EMV_details.future_field3,
                    p_auth_tx_in->EMV_details.emv_block+2
                    + sizeof(p_auth_tx_out->EMV_details.future_field1)-3
                    + sizeof(p_auth_tx_out->EMV_details.future_field2)-1,
                    sizeof(p_auth_tx_out->EMV_details.future_field3)-1 );
         }
      }
   }

   memcpy( p_auth_tx_out->EMV_details.issuer_script_1,
           p_auth_tx_in->EMV_details.issuer_script_1,
           sizeof(p_auth_tx_in->EMV_details.issuer_script_1) );

   memcpy( p_auth_tx_out->EMV_details.issuer_script_2,
           p_auth_tx_in->EMV_details.issuer_script_2,
           sizeof(p_auth_tx_in->EMV_details.issuer_script_2) );

   memcpy( p_auth_tx_out->EMV_details.issuer_auth_data,
           p_auth_tx_in->EMV_details.issuer_auth_data,
           sizeof(p_auth_tx_in->EMV_details.issuer_auth_data) );

   //DE 48 TAG 42 Sub Field 1
   if(p_auth_tx_out->TLF01_details.profile[0] == 0x00)
	   strcpy( p_auth_tx_out->TLF01_details.profile,
	             p_auth_tx_in->TLF01_details.profile );
   //DE 48 TAG 42 Sub Field 2

   if(p_auth_tx_in->TLF01_details.product_codes[14].code[0] != 0x00)
   strcpy( p_auth_tx_out->TLF01_details.product_codes[14].code,
   	             p_auth_tx_in->TLF01_details.product_codes[14].code );

   //DE 48 TAG 42 Sub Field 3
   strcpy( p_auth_tx_out->TLF01_details.product_codes[14].quantity,
   	             p_auth_tx_in->TLF01_details.product_codes[14].quantity );

   if(p_auth_tx_in->TLF01_details.visa_validation_code[0] != 0x00)
	   strncpy( p_auth_tx_out->TLF01_details.visa_validation_code,
   	             p_auth_tx_in->TLF01_details.visa_validation_code,
				3);

#ifdef MC02_APR2024
   //DE105 in 0110 response from Mastercard
   if(p_auth_tx_in->TLF01_details.product_codes[19].amount[0] != 0x00)
   {
	   if((strncmp(p_auth_tx_out->TLF01_details.handler_queue, "dcpiso", 6) == 0) ||
		  (strncmp(p_auth_tx_out->TLF01_details.handler_queue, "VOICE", 5)  == 0))
	   {
		   strncpy( p_auth_tx_out->TLF01_details.product_codes[10].amount,
				    p_auth_tx_in->TLF01_details.product_codes[19].amount, 10 );
		   memset(p_auth_tx_in->TLF01_details.product_codes[19].amount, 0x00,
				  sizeof(p_auth_tx_in->TLF01_details.product_codes[19].amount));
	   }
	   else
	   {
		   strcpy( p_auth_tx_out->TLF01_details.product_codes[19].amount,
				   p_auth_tx_in->TLF01_details.product_codes[19].amount );
	   }
	   strncpy( p_auth_tx_out->TLF01_details.product_codes[1].amount,
			    p_auth_tx_in->TLF01_details.product_codes[1].amount, 12 );

   }
#endif

   return( OK );
}  /* alter_transaction_data */
   



/******************************************************************************
 *
 *  NAME:         SEND REQUEST TO HOST
 *
 *  DESCRIPTION:  This procedure creates the ISO message for the MasterCard
 *                host.  It calls the necessary routines to put it together
 *                and send it.
 *
 *  INPUTS:       p_auth_tx - Transaction record for the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/19/99 - Create call to 'send_msg_to_authorizer' used
 *                     here for dropped messages.
 *     DI - 03/23/99 - Added start host time.
 *     DI - 05/24/99 - Added data check function.
 ******************************************************************************/
INT send_request_to_host( pAUTH_TX p_auth_tx )
{
   pPTE_MSG p_msg_host_out = NULL_PTR;
   CHAR     gmt_time[20]={0};
   CHAR     err_msg[512]={0};
   INT      ret_code = 0;
   WORD     request_timer = 0;
   INT      buf_temp_len=0;

   /*+--------------------------------------------------+
     | Send response to authorizer for failed messages. |
     +--------------------------------------------------+*/
   if ( GS_TXN_DROPPED == p_auth_tx->TLF01_details.general_status )
   {
      memset( err_msg, 0x00, sizeof(err_msg) );
      ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_RESPONSE,
                                         application_que_name, err_msg );
      if ( OK != ret_code )
      {
    	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
    	  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
    	  					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
    	  memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
    	                sizeof(p_auth_tx->TLF01_details.response_text)-1 );
    	  strcat(err_msg,Log_Bufr);
    	  ncmcrd_log_message( 3, 3, err_msg , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
      return( ret_code );
   }

   /* Current Greenwich Mean Time for the transmission-to-host time. */
   if ( 0 == strcmp( NULL_STR,
                     p_auth_tx->TLF01_details.transmission_timestamp ) )
   {
      pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
      strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
               &gmt_time[4], 10 );  /* MMDDhhmmss */
   }

   /* Set host start time with current local time. */
   ptetime_get_strtime( p_auth_tx->TLF01_details.host_start_time );

   /* Build PTEMSG for AUTH_TX table */
   if ( GS_TXN_NEW == p_auth_tx->TLF01_details.general_status )
      p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;

   /*+--------------------+
     | Build Host Message |
     +--------------------+*/
   if ( GS_TXN_PERF_REQ == p_auth_tx->TLF01_details.general_status )
   {
      build_host_structure( p_auth_tx );
      if ( OK == build_host_request( p_auth_tx ) )
      {
        /* Check for valid data. */
        if ( OK != perform_data_checks( p_auth_tx ) )
        {
           perform_error_processing( p_auth_tx, "03" );
           return( NOT_OK );
        }

        /*+---------------------------+
          | Build Start Timer Message |
          +---------------------------+*/
         request_timer = (WORD)(atoi(ncf01_i.request_timer));
         if ( OK != set_host_timer( p_auth_tx, application_que_name,
                                    request_timer, err_msg ) )
         {
            return( NOT_OK );
         }

		if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcpiso",6) &&
		   0 == strncmp(p_auth_tx->TLF01_details.response_code,"10",2))
		{
			//dont increment active count for reversal sent for partial approved txns.
		}
		else
		{
         (void)increment_active_count( AppName, err_msg );
		}

		 if(true == ncmcrd_check_for_acq_refund_transaction(p_auth_tx))
		 {
			 	 buf_temp_len = atoi(p_auth_tx->resp_data_len);
				if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
				{	buflen = buf_temp_len;
					memcpy(ReqRsp_Buffer,p_auth_tx->resp_buffer,buflen);
				}
		 }
         ret_code = build_and_send_request_message(MT_OUTGOING, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              dialog_que_name,
                                              ReqRsp_Buffer,
                                              buflen,
                                              0,
                                              p_auth_tx->TLF01_details.tpdu_id,
                                              err_msg );
         if ( OK != ret_code )
         {
			  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
								p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
			  memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
							sizeof(p_auth_tx->TLF01_details.response_text)-1 );
			  strcat(err_msg,Log_Bufr);
			  ncmcrd_log_message( 3, 3, err_msg , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
         else
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

            /* Log transaction to trace file. */
            write_to_txn_file();
         }
      }
      else
      {
		  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		  sprintf(Log_Bufr," Unable to send request to host, tran table error Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
				p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		  memcpy( p_auth_tx->TLF01_details.response_text, "Unable to send request to host, tran table error",
						sizeof(p_auth_tx->TLF01_details.response_text)-1 );
		  strcat(err_msg,Log_Bufr);
		  ncmcrd_log_message( 3, 3, err_msg , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }
   return( ret_code );
}  /* send_request_to_host */


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
 *  RTRN VALUE:   OK if host is ONLINE, else NOT_OK
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT  verify_host_app_status( pAUTH_TX p_auth_tx )
{
   CHAR  err_msg[512]={0};
   INT   ret_val;

   /* Get the shared memory values. */
   ret_val = read_shared_mem_table(AppName, net_consec_tmouts_ctr,
                                   active_txns_ctr, current_state, err_msg );

   if ( current_state[0] != cONLINE )
      ret_val = NOT_OK;

   sprintf( err_msg,
   		    "Max Timeouts Count : %s, Max active txns count : %s ", net_consec_tmouts_ctr, active_txns_ctr);
   ncmcrd_log_message( 2, 1, err_msg , "verify_host_app_status" , 0 , p_auth_tx->TLF01_details.card_num);

   if ( ret_val == OK )
   {
      /* Check number of consecutive time outs.  If too many, Post warning. */
      if ( MaxConsecTimeouts > 0 )
      {
         if (atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts )
         {
            ret_val = write_to_shared_mem_table( AppName, net_consec_tmouts_ctr,
                                                 active_txns_ctr, OFFLINE,
                                                 HOST_STATE_CHANGED, err_msg );
            sprintf( err_msg,
            		 "Maximum consecutive timeouts reached : %s", net_consec_tmouts_ctr);

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
            		p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
            strcat(err_msg,Log_Bufr);
            ncmcrd_log_message( 1, 1, err_msg , "verify_host_app_status" , 0 ,p_auth_tx->TLF01_details.card_num);

           ret_val = false;
         }
      }

      /* Check number of active transactions.
       * If too many, issue a warning.
       */
      if ( (MaxActiveTxns > 0) && (ret_val == true) )
      {
         if( atoi(active_txns_ctr) >= MaxActiveTxns )
         {
            sprintf( err_msg,
                     "Maximum active txns reached. Continuing to process : %s", active_txns_ctr);

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
            		p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
            strcat(err_msg,Log_Bufr);
            ncmcrd_log_message( 1, 1, err_msg , "verify_host_app_status" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }
   return( ret_val );
}  /* verify_host_app_status */


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
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
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
   INT         ret_code = NOT_OK;
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   CHAR        err_msg[512]={0};
   XINT        ReturnCode = 0 ;
   int         nIndex = 0 ;
   AUTH_TX     auth_tx;
 

   memset( &ncf01, 0, Ncf01_Size );
   
   read_shared_mem_table( AppName, net_consec_tmouts_ctr,active_txns_ctr, current_state, err_msg);
   strcpy( ncf01.primary_key.network_id,    AppName      );
   strcpy( ncf01.primary_key.network_type, "A"           );
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
      ncmcrd_log_message( 1, 1, "Insufficient Memory to create message" , "send_network_status_to_monitor" , 0 ,"");
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;

         /* Send message to get host specific data from data server. */
         memset( &auth_tx,0x00, Auth_Tx_Size );
         memset( err_msg, 0x00, sizeof(err_msg) );
         ret_code = send_msg( p_msg_ins, applnk_que_name, err_msg );
         if ( OK != ret_code )
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
            		auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            memcpy( auth_tx.TLF01_details.response_text, err_msg,
                          sizeof(auth_tx.TLF01_details.response_text)-1 );
            strcat(err_msg,Log_Bufr);
            ncmcrd_log_message( 3, 3, err_msg , "send_network_status_to_monitor" , 0 ,auth_tx.TLF01_details.card_num);
         }
      }
      free( p_msg_ins );
   }

   /* The time to sleep is the same here, just different units for NT vs UNIX. */
   #ifdef WIN32
      Sleep( 500 );
   #else
      usleep( 500000 );
   #endif

   strcpy( ncf01.primary_key.network_type, "I"           );

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
      ncmcrd_log_message( 1, 1, "Insufficient Memory to create message" , "send_network_status_to_monitor" , 0 ,"");
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;

         /* Send message to get host specific data from data server. */
         memset( &auth_tx,0x00, Auth_Tx_Size );
         memset( err_msg, 0x00, sizeof(err_msg) );
         ret_code = send_msg( p_msg_ins, applnk_que_name, err_msg );
         if ( OK != ret_code )
		 {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
            		auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            memcpy( auth_tx.TLF01_details.response_text, err_msg,
                          sizeof(auth_tx.TLF01_details.response_text)-1 );
              strcat(err_msg,Log_Bufr);
            ncmcrd_log_message( 3, 3, err_msg , "send_network_status_to_monitor" , 0 ,auth_tx.TLF01_details.card_num);
		 }
      }
      free( p_msg_ins );
   }
   return( ret_code );

}  /* send_network_status_to_monitor */


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

      case AUTH_CASH_ADVANCE_AUTHORIZATION:
         return( AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE ); 
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

      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
         return( AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE );
      break;

      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
         return( AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE );
      break;

      case AUTH_AUTHORIZATION_OVERRIDE:
         return( AUTH_AUTHORIZATION_OVERRIDE_RESPONSE );
      break;

      case AUTH_VOID_SALE:
         return( AUTH_VOID_SALE_RESPONSE );
      break;

      case AUTH_VOID_REFUND: 
         return( AUTH_VOID_REFUND_RESPONSE );
      break;

      case AUTH_REVERSAL:
         return( AUTH_REVERSAL_RESPONSE );
      break;
#if BDOR_62_PREAUTH_VOID
      case AUTH_VOID_PRE_AUTHORIZATION:
    	  return (AUTH_VOID_PRE_AUTHORIZATION_RESPONSE);
      break;
#endif
      case AUTH_LOGON:
         return( AUTH_LOGON_RESPONSE );
      break;

      case AUTH_TEST:
         return( AUTH_TEST_RESPONSE );
      break;
	  
	  case AUTH_QUASI_CASH:
		 return(AUTH_QUASI_CASH_RESPONSE);
		 break;

      default:
         return( request+100 );
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
 *                resp_code    - Response code to send to msg initiator
 *
 *  OUTPUTS:      p_auth_tx_in - Structure containing the updated txn record
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 04/09/99 - Changed the format of RRN.
 *     DI - 04/16/99 - Changed format of RRN again.
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
   if ( 0 == strcmp( NULL_STR, p_auth_tx_in->TLF01_details.retrieval_ref_num))
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

   p_auth_tx_in->TLF01_details.general_status = GS_TXN_DROPPED;

   send_it_back_to_terminal( resp_code, p_auth_tx_in );
     
}  /* perform_error_processing */


/******************************************************************************
 *
 *  NAME:         PERFORM_DATA_CHECKS
 *
 *  DESCRIPTION:  This procedure validates data field prior to sending a
 *                transaction (can be request or response) to a host.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if data is good, else NOT_OK
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT perform_data_checks( pAUTH_TX p_auth_tx )
{
   CHAR    msg_type[5];
   INT     i;
   INT     ret_val = OK;

   /* Get message type of transaction going to host. */
   memset( msg_type, 0x00, sizeof(msg_type) );
   for( i=0; i<4; i++ )
      msg_type[i] = ascii_ebcdic_conv(ReqRsp_Buffer[i], E2A);

   if ( 0 == strcmp( "0100", msg_type ) )
   {
      /* Merchant Category Code cannot be blank. */
      if ( 0 >= strlen(p_auth_tx->TLF01_details.category_code) )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Merchant Category Code is empty. Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
         		p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         memcpy( p_auth_tx->TLF01_details.response_text, "Merchant Category Code is empty.",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcrd_log_message( 1, 2, Log_Bufr , "perform_data_checks" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = NOT_OK;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ROUTE_TO_CIRRUS
 *
 *  DESCRIPTION:  This procedure routes a message to the Cirrus network
 *                controller.  This is used when ncmcrd receives a debit txn.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if able to forward transaction on to Cirrus, else NOT_OK
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT route_to_cirrus( pAUTH_TX p_auth_tx )
{
   INT        ret_code = NOT_OK;
   CHAR       err_msg[512]={0};

   ret_code = build_and_send_request_message( MT_AUTH_REQUEST, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              cirrus_que_name,
                                              (pBYTE)p_auth_tx,
                                              Auth_Tx_Size,
                                              0,
                                              NULL_STR,
                                              err_msg );
   if ( OK != ret_code )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(err_msg,Log_Bufr);
      ncmcrd_log_message( 3, 3, err_msg , "route_to_cirrus" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_code );
}  /* route_to_cirrus */

/******************************************************************************
 *
 *  NAME:         UPDATE_AUTH_TX_WITH_NCF30_DATA
 *
 *  DESCRIPTION:  This function takes data from ncf30 and puts it into auth_tx
 *
 *  INPUTS:       pncf30 - Pointer to NCF30 structure
 *
 *  OUTPUTS:      auth_tx - with data from NCF30
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Beth Gable
 *
 *  MODIFICATIONS:
 ******************************************************************************/
VOID update_auth_tx_with_ncf30_data(pNCF30 pncf30, pAUTH_TX p_auth_tx)
{
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.card_num ) )
      strcpy( p_auth_tx->TLF01_details.card_num, pncf30->card_nbr ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.processing_code ) )
      strcpy( p_auth_tx->TLF01_details.processing_code, pncf30->processing_code );

   strcpy( p_auth_tx->TLF01_details.orig_amount,   pncf30->tran_amount ); 
   strcpy( p_auth_tx->TLF01_details.time_hhmmss,   pncf30->tran_time   ); 
   strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, pncf30->tran_date   ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_retrieval_ref_num))
      strcpy( p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              pncf30->primary_key.retrieval_ref_num ); 

   strcpy( p_auth_tx->TLF01_details.auth_number,   pncf30->auth_number   ); 
   strcpy( p_auth_tx->TLF01_details.response_code, pncf30->response_code ); 
   strcpy( p_auth_tx->TLF01_details.terminal_id,   pncf30->device_id     ); 
   strcpy( p_auth_tx->TLF01_details.currency_code, pncf30->currency_code ); 
   strcpy( p_auth_tx->TLF01_details.orig_message,  pncf30->message_type  ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_sys_trace_num ))
      strcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
              pncf30->sys_trace_audit_nbr );

   strcpy( p_auth_tx->TLF01_details.tran_fee_amount,
           pncf30->tran_fee_amount ); 

   /* If there is something in this field, keep the value. */
   if ( p_auth_tx->TLF01_details.settlement_date[0] == 0x00 )
      strcpy(p_auth_tx->TLF01_details.settlement_date, pncf30->settlement_date);

   /* If there is something in this field, keep the value. */
   if ( p_auth_tx->TLF01_details.transmission_timestamp[0] == 0x00 )
      strcpy( p_auth_tx->TLF01_details.transmission_timestamp,
              pncf30->transmission_timestamp ); 
   /* ATP needs for STIP txn TF-Ajay 13Jul2009 */
   strcpy( p_auth_tx->TLF01_details.product_codes[1].code, pncf30->invoice_nbr );
}


/*****************************************************************************

  Function:    process_incoming_message_ncf30_reply

  Description: This function will processin an incoming message from the data
               server request for a select of ncf30.  
  Author: 
      E Gable
  Inputs:
      pCurrent - a pointer to the reply message from the data server
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
******************************************************************************/
INT process_incoming_message_ncf30_reply(pBYTE pCurrent)
{
   AUTH_TX   auth_tx;
   pPTE_MSG  p_msg_ins = NULL_PTR;
   NCF30     ncf30;


   memset(&ncf30,   0, Ncf30_Size);
   memset(&auth_tx, 0, Auth_Tx_Size);
   memcpy(&ncf30,   pCurrent+(Auth_Tx_Size), Ncf30_Size);
   memcpy(&auth_tx, pCurrent, Auth_Tx_Size);

   /* Original txn was found in NCF30 for an advice or reversal. */
   if ( ncmcrd_Check_Reversal_Advice_Iss_txns(&auth_tx) == true  )
   {
      /* Treat advices differently. */
	   /* The below flow commented because, we have already replied back to MC
	   	* before sending request to Netds for ncf30 .*/
     // (void)process_auth_advices( &auth_tx, FOUND, ncf30 );
   }
   else
   {
      /* Tell txcntl to go process the reversal. */
	   if(auth_tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE)
	   {
		   // We have found the original purchange return. AR indicate authorization return
		   strncpy(auth_tx.TLF01_details.product_codes[3].quantity,"AR",2);
		   strncpy(auth_tx.TLF01_details.orig_message,auth_tx.TLF01_details.message_type,4);
	   }
	   else
	   {
	   update_auth_tx_with_ncf30_data(&ncf30, &auth_tx);
	   }
      (void)mcrdutil_transfer_ctl_to_auth_app(&auth_tx);
   }
   return(true);
}  

/******************************************************************************
*
*  NAME:         PROCESS_AUTH_ADVICES
*
*  DESCRIPTION:  This function processes authorization advices.  An advice
*                means the host did not receive a response to an authorization.
*                So they stood in for us.  The advice tells us the disposition
*                of the stand in.  This function checks 2 things: Was the
*                original authorization found in NCF30 (which means ATP approved
*                it), Did the host approve or decline the authorization?
*                The advice contains the authorization number and the response
*                code of the original authorization.
*
*                Here is what happens:
*                1.  Always resond to the host with an 0130
*                2.  Always log advice to TLF01
*                3.  If the original authorization found and the advice contains
*                       a declined response code, then send advice to txcntl and
*                       drop txcntl's response.  Txcntl will treat it as a
*                       reversal.
*                4.  If the original authorization was not found and the advice
*                       contains an approved response code, then do the same as
*                       in step 3.  Txcntl will treat it as an authorization.
*
*  INPUTS:       p_auth_tx - Transaction record
*                status    - Status of the select for the original authorization
*                            FOUND or NOT_FOUND
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred 
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:

********************************************************************** ********/
INT process_auth_advices_old (pAUTH_TX p_auth_tx, INT status , NCF30 ncf30 )
{
   INT   ret_val;
   char  resp_code[4]; //temp data

   /* Send response to host and log to TLF01.
    * Need to do this prior to going to txcntl where the auth_number
    * and response code may get overwritten.
    */
   p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
   ret_val = incoming_other_host_response_db_reply( p_auth_tx );
   strncpy(resp_code,p_auth_tx->TLF01_details.response_code,2);

   if ( true == ret_val )
   {
      /* See if we need to send to txcntl. */
      if ( ( (FOUND == status) &&
             (0 != strcmp(p_auth_tx->TLF01_details.response_code, APPROVED )) )
           ||
           ( (NOT_FOUND == status) &&
             (0 == strcmp(p_auth_tx->TLF01_details.response_code, APPROVED )) )
         )
      {
         /* Send advice to transaction control. */
          if ( FOUND == status )
         {
            update_auth_tx_with_ncf30_data( &ncf30, p_auth_tx );
           //restoring back the response code
           strncpy(p_auth_tx->TLF01_details.response_code,resp_code,2);
         }
         p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE;
		 // TF Phani..store response code in voice Auth text and restore in ncmcr2
		 memcpy(p_auth_tx->TLF01_details.voice_auth_text,p_auth_tx->TLF01_details.response_code,2);
         ret_val = mcrdutil_transfer_ctl_to_auth_app( p_auth_tx );
      }
   }
   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf( Log_Bufr," Unable to complete processing of an advice (1120). Term Id:%s ,Merch Id: %s RRN: %s ",
      		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Unable to complete processing of an advice (1120).",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 3, Log_Bufr , "process_auth_advices_old" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_val );
 }


/******************************************************************************
*
*  NAME:         PROCESS_AUTH_ADVICES
*
*  DESCRIPTION:  This function processes authorization advices.  An advice
*                means the host did not receive a response to an authorization.
*                So they stood in for us.  The advice tells us the disposition
*                of the stand in.  This function checks 2 things: Was the
*                original authorization found in NCF30 (which means ATP approved
*                it), Did the host approve or decline the authorization?
*                The advice contains the authorization number and the response
*                code of the original authorization.
*
*                Here is what happens:
*                1.  Always resond to the host with an 0130
*                2.  Always log advice to TLF01
*                3.  send tx to TXCNTL
*
*  INPUTS:       p_auth_tx - Transaction record
*                status    - Status of the select for the original authorization
*                            FOUND or NOT_FOUND
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred
*
*  AUTHOR:       Phani
*
*  MODIFICATIONS:

********************************************************************** ********/
INT process_auth_advices (pAUTH_TX p_auth_tx, INT status , NCF30 ncf30 )
{
   INT   ret_val;
   char  resp_code[4] = {0}; //temp data

   /* Send response to host and log to TLF01.
    * Need to do this prior to going to txcntl where the auth_number
    * and response code may get overwritten.
    */
   p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
   ret_val = incoming_other_host_response_db_reply( p_auth_tx );
   strncpy(resp_code,p_auth_tx->TLF01_details.response_code,2);

   if ( true == ret_val )
   {
      /*  we need to send to txcntl. */
      //restoring back the response code
           strncpy(p_auth_tx->TLF01_details.response_code,resp_code,2);
         p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE;
		 // TF Phani..store response code in voice Auth text and restore in ncmcr2
		 memcpy(p_auth_tx->TLF01_details.voice_auth_text,p_auth_tx->TLF01_details.response_code,2);
         ret_val = mcrdutil_transfer_ctl_to_auth_app( p_auth_tx );

   }
   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to complete processing of an advice (1120). Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Unable to complete processing of an advice (1120).",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "process_auth_advices" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_val );
 }

/******************************************************************************
*
*  NAME:         PROCESS_REVERSAL_TRANSACTIONS
*
*  DESCRIPTION:  This function processes all type of reversal transactions.
*  				 Before sending for ncf30 we do below things.
*
*                Here is what happens:
*                1.  Always resond to the host with an 0130
*                2.  Always log advice to TLF01
*                3.  send tx to TXCNTL
*
*  INPUTS:       p_auth_tx - Transaction record
*                status    - Status of the select for the original authorization
*                            FOUND or NOT_FOUND
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred
*
*  AUTHOR:       Raghavendra
*
*  MODIFICATIONS:

********************************************************************** ********/
INT ncmcrd_process_reversal_transactions (pAUTH_TX p_auth_tx, INT status )
{
   INT   ret_val = 0;
   char  resp_code[4] = {0}; //temp data
   BYTE  tx_key_for_txcntl =0;

   /* Send response to host and log to TLF01.
    * Need to do this prior to going to txcntl where the auth_number
    * and response code may get overwritten.
    */
   tx_key_for_txcntl = p_auth_tx->TLF01_details.tx_key;
   if(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)
   {

	   p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
   }
   else
   {
	   p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   }
   strcpy( p_auth_tx->TLF01_details.reversal_amount,
			 p_auth_tx->TLF01_details.total_amount );
   ret_val = incoming_other_host_response_db_reply( p_auth_tx );

   //MC LOOPING
   memcpy(p_auth_tx->TLF01_details.orig_message,       p_auth_tx->TLF01_details.dcf01_retired_cwk,   4);
   memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, p_auth_tx->TLF01_details.dcf01_retired_cwk+4, 6);
   memcpy(p_auth_tx->TLF01_details.product_codes[5].amount, p_auth_tx->TLF01_details.dcf01_retired_cwk+10, 10);

   strncpy(resp_code,p_auth_tx->TLF01_details.response_code,2);

   if ( true == ret_val )
   {
      /*  we need to send to txcntl. */
      //restoring back the response code
         strncpy(p_auth_tx->TLF01_details.response_code,resp_code,2);
         p_auth_tx->TLF01_details.tx_key = tx_key_for_txcntl;
		 // TF Phani..store response code in voice Auth text and restore in ncmcr2
		 memcpy(p_auth_tx->TLF01_details.voice_auth_text,p_auth_tx->TLF01_details.response_code,2);
         ret_val = mcrdutil_transfer_ctl_to_auth_app( p_auth_tx );

   }
   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to complete processing of an reversal. Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Unable to complete processing of an reversal.",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "process_reversal_transactions" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_val );
 }
/******************************************************************************
*
*  NAME:         DELETE_ORIG_NCF30
*
*  DESCRIPTION:  This function sends a message to the data server to delete
*                a particular NCF30 record.  This will be used to delete 
*                transactions that are being reversed.
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

********************************************************************** ********/
INT delete_orig_ncf30( NCF30 ncf30, pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)]; 
   LONG            ret_code;
   CHAR            Buff[256]={0};
   CHAR            err_mesg[256] = {0};

   /* Set up ncf30 keys. */
   strcpy( ncf30.primary_key.network_id,   AppName );
   strcpy( ncf30.primary_key.network_type, "A"     );
   strcpy( ncf30.primary_key.batch_number, "ACTIVE");
   strcpy( ncf30.primary_key.retrieval_ref_num,
           p_auth_tx->TLF01_details.orig_retrieval_ref_num );
   strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
   

   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_DELETE, 
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF30_DATA );
   if ( NULL_PTR == p_msg  )
   {
      ncmcrd_log_message( 3, 2, "Insufficient Memory to build NCF30 message" , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s", netds_que_name, err_mesg );
      ncmcrd_log_message( 3, 3, Buff , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }
   return(true);
 }

/******************************************************************************
 *
 *  NAME:         UPDATE_DATABASE
 *
 *  DESCRIPTION:  This function will send a request to update transaction
 *                control database tables.  All the information to do so is
 *                stored in p_auth_tx.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT update_database( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)];
   LONG       ret_code;
   INT        ret_val = true;
   CHAR       Buff[256]={0};
   CHAR       err_mesg[256] = {0};

   memset(buffer, 0, sizeof(buffer));
   memcpy(buffer, p_auth_tx, Auth_Tx_Size);

   p_msg = ptemsg_build_msg(MT_DB_REQUEST, 
                            ST1_DB_UPDATE, 
                            ST2_ACQUIRER,
                            updateds_que_name, 
                            application_que_name,
                            (pBYTE)&buffer,
                            sizeof(buffer), 
                            ALL_DATA); 
   if(NULL_PTR == p_msg)
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build UPDATE ALL message Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncmcrd_log_message( 3, 2, Log_Bufr , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
      ret_val = false;
   }
   else
   {
      ret_code = pteipc_send(p_msg, updateds_que_name); 
      free(p_msg);
      if(ret_code < 0)
      {
         pteipc_get_errormsg(ret_code, err_mesg);
         sprintf(Buff,"Failed transmit to que: %s. %s",
                 updateds_que_name,err_mesg);
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
         		  p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncmcrd_log_message( 3, 2, Buff , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = false;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_MCC
 *
 *  DESCRIPTION:  This function verifies the merchant category code resides
 *                within a specified range, if there is no track data.  If
 *                there is track data, no validation is performed, except
 *                that it is numeric.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT validate_mcc( pAUTH_TX p_auth_tx )
{
   INT  ret_code = true;
   INT  mcc;
   INT  len;
   INT  i;
   CHAR tempstr[512]={0};

   len = strlen( p_auth_tx->TLF01_details.category_code );
   if ( len != bf18_len )
   {
      ret_code = false;
      memset( tempstr, 0x00, sizeof(tempstr) );
      sprintf(tempstr,"Invalid MCC: %s",p_auth_tx->TLF01_details.category_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(tempstr,Log_Bufr);
      ncmcrd_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      /* Make sure MCC is numeric. */
      for( i=0; i<4; i++ )
      {
         if ( !isdigit( p_auth_tx->TLF01_details.category_code[i] ) )
         {
            ret_code = false;
            memset( tempstr, 0x00, sizeof(tempstr) );
            sprintf(tempstr,"Invalid MCC: %s",
                    p_auth_tx->TLF01_details.category_code);
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
            					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
            strcat(tempstr,Log_Bufr);
            ncmcrd_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      if ( ret_code == true )
      {
         /* See if track data exists. */
         if ( (p_auth_tx->TLF01_details.track1[0] == 0x00) &&
              (p_auth_tx->TLF01_details.track2[0] == 0x00)  )
		 {
            /* No track data.  Need to verify the MCC. */
            mcc = atoi( p_auth_tx->TLF01_details.category_code );
            if ( ((MCC_RANGE1_LOW <= mcc)  &&  (mcc <= MCC_RANGE1_HIGH)) ||
                 ((MCC_RANGE2_LOW <= mcc)  &&  (mcc <= MCC_RANGE2_HIGH))  )
            {
               ret_code = true;
            }
            else
            {
               ret_code = false;
               memset( tempstr, 0x00, sizeof(tempstr) );
               sprintf(tempstr,"Invalid MCC: %s.  No Track Data.",
                       p_auth_tx->TLF01_details.category_code);
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
               					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                             sizeof(p_auth_tx->TLF01_details.response_text)-1 );
               strcat(tempstr,Log_Bufr);
               ncmcrd_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
            }
         }
      }
   }
   return(ret_code);
}

/******************************************************************************
 *
 *  NAME:         TXN_IS_BLOCKED
 *
 *  DESCRIPTION:  This function determines is a transaction should be blocked.
 *                This is an Equitable specific requirement.  They do not want
 *                ATM or Cash Advance transactions coming from local banks in
 *                the Phillippines.  Block the transaction if:
 *
 *                Currency Code (DE49) is 608   AND
 *                Merchant Category Code (MCC, DE18) is one of these:
 *                   6010, 6011, 6012, 6051
 *
 *                NOTE:  This function assumes a txn switched in from Visa
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if transaction should be blocked (not allowed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   CHAR  BLOCKED_MCC[][5] = { "6010", "6012", "6051", "done" };

   INT   ret_val = false;
   INT   i = 0;

   /* Check if currency code is Phillippino Peso. */
   if ( 0 == strcmp(p_auth_tx->TLF01_details.currency_code,PESO) )
   {
      /* YES, transaction originated locally within the Phillippines.
       *
       * Loop through the list of blocked MCC's.
       */
      while( BLOCKED_MCC[i][0] != 'd' )
      {
         if ( 0 == strcmp(p_auth_tx->TLF01_details.category_code,BLOCKED_MCC[i]) )
         {
            /* Need to block this transaction. */
            ret_val = true;

            sprintf( error_msg,
                    "Blocked txn from MC: MCC=%s, CC=%s, Stan=%s, RRN=%s",
                     p_auth_tx->TLF01_details.category_code,
                     p_auth_tx->country_code,
                     p_auth_tx->TLF01_details.sys_trace_audit_num,
                     p_auth_tx->TLF01_details.retrieval_ref_num );

            break;
         }
         i++;
      }
   }
   return( ret_val );
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
   CHAR  err_msg[512]={0};

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
   if ( OK != ret_code  )
   {
      ncmcrd_log_message( 3, 2, err_msg , "send_transaction_statistics" , 0 ,"");
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
   CHAR  err_msg[100]={0};

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
   if ( OK != ret_code  )
   {
      ncmcrd_log_message( 3, 2, err_msg , "reset_transaction_statistics" , 0 ,"");
   }
   return( ret_code );
}

BOOLEAN ncmcrd_check_for_acq_refund_transaction(pAUTH_TX p_auth_tx)
{
	   if( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)	||
			0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4)  ||
			0 == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4)  ||
			0 == strncmp(p_auth_tx->TLF01_details.message_type,"0410",4))	&&
		    0 == strncmp(p_auth_tx->TLF01_details.processing_code,"20",2) 	&&
		    0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcr2",6))
	   {
		   return true;
	   }
	   return false;
}

BOOLEAN ncmcrd_transfer_refund_response_msg_to_MC2( pAUTH_TX p_auth_tx )
{
	BOOLEAN   retval              = true;
	pPTE_MSG  p_msg_out           = NULL_PTR;
	CHAR	  mc2_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncmcr2A" ;
	CHAR      buffer[sizeof(AUTH_TX)+1]= {0};
	BYTE      originator_info[33] = {0};
	LONG      retCode             = 0L;
	CHAR      strError[512]       = {0};
    memcpy(buffer , p_auth_tx, sizeof(AUTH_TX) );

    p_msg_out = ptemsg_build_msg( MT_AUTH_RESPONSE,
                                  ST1_NONE,
                                  ST2_NONE,
                                  mc2_host_que_name,
                                  application_que_name,
                                  (pBYTE) buffer,
								  sizeof(AUTH_TX),
                                  0 );

    if ( p_msg_out == NULL_PTR )
    {
       sprintf( strError,
               "Insufficient Memory to build MC2 request message. Size: %d",
                buflen );
       ncmcrd_log_message( 3, 3, strError , "ncmcrd_transfer_refund_msg_to_MC2" , 0 ,p_auth_tx->TLF01_details.card_num);
       retval = NOT_OK;
    }
    else
    {
       ptemsg_set_orig_comms_info( p_msg_out, originator_info );
       retCode = pteipc_send( p_msg_out, mc2_host_que_name );
       free (p_msg_out);

       if ( retCode < 0 )
       {
          pteipc_get_errormsg (retCode, strError);
          sprintf( strError,
                  "Error on pteipc_send to que %s: %s",
				  mc2_host_que_name, strError );

          ncmcrd_log_message( 3, 3, strError , "ncmcrd_transfer_refund_msg_to_MC2" , 0,p_auth_tx->TLF01_details.card_num );
          retval = NOT_OK;
       }
    }

	return retval;
}

INT insert_tlf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   LONG       ret_code;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};
   INT        ret_val;

   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
   {
      /* TLF01 has already been inserted. */
      ret_val = true;
   }
   else
   {
      p_auth_tx->host2_acquirer_cb_code[0] = '*';

      if ( p_auth_tx->TLF01_details.primary_key.transaction_id[0] == 0x00 )
      {
         /* Tran ID might be missing in the event of parsing errors. */
    	  mcrdutil_init_txn( p_auth_tx );
      }

      memset(buffer, 0, sizeof(buffer));
      ptetime_get_strtime(p_auth_tx->TLF01_details.tran_finish_time);
      memcpy( buffer, p_auth_tx, Auth_Tx_Size );
      memcpy(buffer+Auth_Tx_Size,&p_auth_tx->TLF01_details, Tlf01_Size);

      p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST,
                                   ST1_DB_INSERT,
                                   ST2_NONE,
                                   updateds_que_name,
                                   application_que_name,
                                   (pBYTE)&buffer,
                                   sizeof(buffer),
                                   TLF01_DATA);
      if(NULL_PTR == p_msg_ins)
      {
         ncmcrd_log_message( 3, 2, "Insufficient Memory to build insert TLF01 message" , "insert_tlf01" , 0 ,"");
         ret_val = false;
      }
      else
      {
         ret_code = pteipc_send(p_msg_ins, updateds_que_name);
         free(p_msg_ins);
         if(ret_code < 0)
         {
            pteipc_get_errormsg(ret_code, err_mesg);
            sprintf( Buff,
                    "Failed transmit to que: %s. %s",
                     updateds_que_name,err_mesg );
            ncmcrd_log_message( 3, 2, Buff , "insert_tlf01" , 0 ,"");
            ret_val = false;
         }
         else
            ret_val = true;
      }
   }
   return( ret_val );
}

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

