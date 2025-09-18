/******************************************************************************
 
    CIRSUTIL.C
 
    Copyright (c) 2003, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Cirrus Host Handler Utilities
 
    This module is a part of the Cirrus Host Handler.  It contains all
    the utilities used by the host handler to parse and manipulate all incoming
    messages from the terminal and from the host.  Cirrus is the MasterCard
    Debit Switch.
 
    APPLICATION:  Advanced Transaction Processor (ATP)
 
    AUTHOR:  Dennis Irby
 
    REVISION HISTORY

    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nccirrus\Cirsutil.c  $
   
      Rev 1.91   Jun 17 2005 11:41:54   chuang
   Version 4.1.1.4. Fix scr 15143, log reject reason code and 
   detail code into TLF01.vehicle_number. log issuer_id. Fix 
   parsing error of field 62.
   
      Rev 1.90   Oct 22 2004 12:11:46   dirby
   Modified field 61 to set positions 17-26 to zeroes when merchant
   is not configured with postal information.
   SCR 12897
   
      Rev 1.89   Feb 09 2004 14:21:54   dirby
   Modified field 90.3 to get original transmission timestamp from
   auth_tx.orig_local_time_date.  This is where nceqit puts it.
   SCR 1331
   
      Rev 1.88   May 05 2003 22:34:04   dirby
   Added an error message if updating NCF01 on key change fails.
   
      Rev 1.87   Apr 02 2003 14:14:38   dirby
   Added support in function 'process_reconciliation' for 0522 txns.
   SCR 1072
   
      Rev 1.86   Apr 02 2003 10:44:06   dirby
   Corrected bugs related to 0422/0432 transactions.  These include
   using a 2nd bit map for 0432 and determining transaction flow
   (switched in or out) in order to populate various fields correctly.
   SCR 1073
   
      Rev 1.85   Apr 01 2003 14:47:50   dirby
   Added data element fields necessary to support 0422/0432
   issuer reversal transactions.
   SCR 1073
   
      Rev 1.84   Mar 26 2003 13:52:42   dirby
   Added field 99 to build_host_structure so it can go out in 0530 messages.
   SCR 1070
   
      Rev 1.83   Mar 26 2003 12:47:26   dirby
   Modified to copy account balance information from the host
   into auth_tx so that nceqit can read it.
   SCR 1066
   
      Rev 1.82   Mar 24 2003 14:30:50   dirby
   Cleaned up a lot of code on database responses, getting rid of
   lots that is no longer used.  Modified to look for database results
   from NCF21 instead of ALL_DATA.
   SCR 1064
   
      Rev 1.81   Mar 19 2003 20:30:38   dirby
   Modified to store settlement date in BCH10 (in the def_gross_amt
   field) to be used in reversals.
   SCR 1063
   
      Rev 1.80   Mar 18 2003 17:07:42   dirby
   Corrected a bug where a sprintf was overwriting a value in
   HOST_MESSAGE.pan.
   SCR 1057
   
      Rev 1.79   Mar 18 2003 16:54:32   dirby
   Rewrote the building of Field 2 to be more concise and to flow
   a little smoother.  This will hopefully reduce/eliminate logons
   that have garbage in them.
   SCR 1057
   
      Rev 1.78   Mar 17 2003 16:47:46   dirby
   Changed building of DE95 to only be used during reversals,
   and to store the 'actual' amount in that field - that is the amount
   that was dispensed.
   SCR 1056
   
      Rev 1.77   Mar 14 2003 15:37:26   dirby
   Reworked the way DE2 gets populated for Log ON/OFF.  It was
   not properly populated if the Forwarding Inst ID Code did not
   exist.  This is in Build Host Structure.
   SCR 1055
   
      Rev 1.76   Nov 12 2002 13:27:06   dirby
   Added code to not insert record into TLF01 if it has already
   been inserted by the update_all process.
   SCR 845
   
      Rev 1.75   Aug 07 2002 10:28:40   dirby
   1.  Removed DE15 for outgoing requests.                SCR 864
   2.  Modified DE22 to set 3 digit to '1' if a pin exists.  SCR 863
   3.  Removed Msg ID from error msgs to be consistent with other
   network controllers.   SCR 87
   
      Rev 1.74   Jul 29 2002 11:05:48   dirby
   Modified to allow echoes to continue to go out when offline and 
   set host state online if a response is received.
   SCR 594
   
      Rev 1.73   Jul 11 2002 14:13:52   dirby
   Added code to log DE43 to TLF01.
   SCR 775
   
      Rev 1.72   Jul 07 2002 02:56:54   dirby
   Reimplemented going offline when max time outs occur.
   SCR 594
   
      Rev 1.71   Jul 03 2002 17:29:00   dirby
   1.  Modified to use pre-calculated sizes instead of always recalculating a structure size.
   2.  Remove use of read_timer.
   3.  Provide details of errors from timerds.
   4.  Added code to display time outs and reversals on Monitor.
   5.  Version 4.0.3.1
   SCR 621
   
      Rev 1.70   Jan 16 2002 16:53:48   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.69   Jan 14 2002 18:11:14   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.68   Dec 11 2001 14:53:46   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.67   Dec 03 2001 16:23:06   dirby
   Modified fields 32 and 33 so they get stored into TLF01 only if
   they are not already populated.     SCR 414
   
      Rev 1.66   Aug 15 2001 14:32:18   SYARLAGA
   Modified field 32 and field 33 in  build_host_structure function.
   
      Rev 1.65   Oct 18 2000 14:30:00   dirby
   Move tran_finish_time to the function that inserts into TLF01.
   This eliminates the need for multiple calculations of this value.
   
      Rev 1.64   Sep 29 2000 14:52:18   dirby
   Modified the 'set_host_state' logic to always update status on Monitor
   if the state changes.
   
      Rev 1.63   Sep 18 2000 16:48:40   dirby
   Moved the function that updates Monitor's status into
   'set_host_state'.  In 'write_to_shared_mem_table' this
   status would be sent a couple of times for every txn.
   
      Rev 1.62   Aug 23 2000 13:37:50   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.61   Aug 17 2000 15:11:16   dirby
   Added a line to trim trailing blanks from track2, as nceqit lets them come through.
   
      Rev 1.60   Aug 08 2000 16:59:44   dirby
   Corrected some indentation.
   
      Rev 1.59   Aug 08 2000 13:03:14   dirby
   Removed the logic of fetching NCF01 during every online txn.
   
      Rev 1.58   Aug 07 2000 14:54:28   dirby
   Made field 96 conditional; it is to be sent out only with
   Log ONs and Log OFFs, not echoes or anything else.
   
      Rev 1.57   Mar 21 2000 11:27:36   ddabberu
   error_msg displayed on update fail
   
      Rev 1.56   Feb 23 2000 12:44:22   ddabberu
   build_host_request() modified
   
   scr # 428
   
      Rev 1.55   Feb 14 2000 14:56:26   svelaga
   Modified the logic to maintain the transaction
   integrity.
   
      Rev 1.54   Jan 31 2000 11:42:20   ddabberu
   cirsutil_parse_db_error () modified to fix
   scr #332
   
      Rev 1.53   Jan 27 2000 13:53:46   dirby
   Made function 'CondPan' non-static.
   
      Rev 1.52   Jan 24 2000 10:52:40   svelaga
   Fixed bug : SCR# 284
   
      Rev 1.51   Jan 23 2000 16:36:10   dirby
   Incorporated echo testing logic.
   
      Rev 1.50   Jan 13 2000 11:46:02   ddabberu
   Added Code to do auto_logon on service start_up.
   1. Modifed MainProcessor()
   2. Added hhutil_process_response()
   3. Added hhutil_get_ncf01_cfg()
   
      Rev 1.49   Dec 28 1999 14:14:38   dirby
   Modified how field 11 is built in Build_Host_Structure.
   
      Rev 1.48   Nov 10 1999 15:19:26   dirby
   Changed function 'insert_into_tlf01' to use queue name for UpdateDS
   instead of queue name for DevDS.
   
      Rev 1.47   23 Aug 1999 17:08:38   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.46   16 Aug 1999 09:53:20   dirby
   Modified field 43 to get CITY from MCF01_details.city.
   
      Rev 1.45   11 Aug 1999 13:16:28   dirby
   Changed EBCDIC value of '^' from 0x9A to 0x5F in the EBCDIC to ASCII
   conversion table.
   
      Rev 1.44   05 Aug 1999 13:27:58   dirby
   Changed field 48, incoming from host, to check for TCC
   if message type is 01xx or 02xx.
   
      Rev 1.43   03 Aug 1999 13:34:36   dirby
   Modified field 52 (pin number) to convert it to binary
   prior to sending it out.
   
      Rev 1.42   30 Jul 1999 14:40:46   dirby
   Added parameter to function find_cvc_flag to indicate if field 48
   contains a TCC.  Added code to this function to clean local variable.
   
      Rev 1.41   14 Jul 1999 16:57:54   dirby
   Modified field 62 in function Move_ISO... to only copy
   the field if data exists.  Otherwise, existing data gets overwritten.
   
      Rev 1.40   14 Jul 1999 08:22:50   dirby
   1.  Changed alot of 'strncpy's to 'memcpy's.
   2.  Made sure strings are initialized prior to use.
   These changes are part of the ongoing effort to
   data proof the code.
   
      Rev 1.39   Jun 30 1999 16:00:08   svelaga
   Fixed bug:
    For the processing code 173000 on cash advance, kindly change it to 013000.  
   
      Rev 1.38   28 Jun 1999 12:56:32   dirby
   1.  Modified processing code to be '01x000' for debit withdrawals.
   2.  Modified processing code to be '173000' for credit cash advances.
   3.  Modified fields 12 and 13 to get system time, not the time out of the transaction.
   4.  Modified field 43 to get name and city from the transaction.
   
      Rev 1.37   Jun 23 1999 10:52:16   egable
   Correct field 61.
   
      Rev 1.36   Jun 23 1999 09:38:22   egable
   Change the processing code for industry
   codes 900, 901 and 902 from 17 to 01.
   
      Rev 1.35   21 Jun 1999 16:36:36   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.34   21 Jun 1999 16:10:30   dirby
   Changed what goes into originator comm info to be tpdu id
   for responses to the host.
   
      Rev 1.33   18 Jun 1999 15:55:24   dirby
   Reworked field 43 in 'build_host_structure'.
   
      Rev 1.32   17 Jun 1999 14:08:38   dirby
   Changed 'response_text' field to 'voice_auth_text' in the function
   that stores text into this field.
   
      Rev 1.31   Jun 14 1999 13:50:00   svelaga
   removed static qualifier for ascii_ebcdic_conv function.
   
      Rev 1.30   Jun 09 1999 12:03:08   svelaga
   In Build_host_Structure  for Field 13:
   In strcmp call second parameter is changed from
   p_auth_tx->TLF01_details.time_hhmmss TO
    p_auth_tx->TLF01_details.date_yyyymmdd
   
   
      Rev 1.29   08 Jun 1999 10:45:26   dirby
   Added flag 'invalid_msg_format' to indicate if the given length of a message from the host
   is not the actual length of the message.
   
      Rev 1.28   02 Jun 1999 15:27:48   dirby
   Modified field 7 to be echoed out for log on responses.
   Modified field 96 to be binary instead of ascii.
   
      Rev 1.27   28 May 1999 17:58:38   dirby
   Added functionality to fill the response text field in auth_tx.
   
      Rev 1.26   19 May 1999 16:15:42   dirby
   Added '^' as 0x9A EBCDIC to the EBCDIC-ASCII
   conversion function.  This is for Track 1 data.
   
      Rev 1.25   10 May 1999 16:22:04   dirby
   Added code to handle terminal Ids that are less than
   8 characters long.  This included modifying the way
   a timer lookup key is created.
   
      Rev 1.24   06 May 1999 09:46:44   dirby
   Fixed processing code.
   
      Rev 1.23   04 May 1999 10:51:06   dirby
   1.  Modified the processing code to be based on
        Industry code instead of transaction type.
   2.  Corrected a bug in function 'set_host_state'.
   
      Rev 1.22   30 Apr 1999 14:44:32   dirby
   Modified tran_finish_time.
   
      Rev 1.21   29 Apr 1999 16:47:58   dirby
   Modified the way a transaction_id is created to give
   it more granularity.
   
      Rev 1.20   16 Apr 1999 16:18:42   dirby
   Added function 'create_rrn' which will create a retrieval
   reference number that meets 3 special criteria:
   1.  Be numeric
   2.  Be unique
   3.  First 4 characters are YDDD - last digit of year + julian day.
   
      Rev 1.19   14 Apr 1999 11:07:32   dirby
   Removed some debug test messages.
   
      Rev 1.18   13 Apr 1999 16:53:02   dirby
   Added check around field 50 in build_host_structure to
   only move data if there is data to move, as this field is conditional.
   
      Rev 1.17   10 Apr 1999 15:49:40   dirby
   1.  Modified field 7 in Build_Host_Structure to always calculate
   GMT if it does not exist.
   2.  In same function, modified PAN to echo back the value from
   a request.
   
      Rev 1.16   09 Apr 1999 15:50:20   dirby
   1.  Added code to create RRN.
   2.  Added case in function Build_Host_Structure, field 3, for Cash Advance.
   3.  In function Build_Host_Structure, field 48, changed default value to 'U'.
   
      Rev 1.15   Mar 30 1999 14:30:50   egable
   Treat an industry code of 902 like a 900.
   Error report #390.
   
      Rev 1.14   Mar 29 1999 16:42:44   egable
   Correct the POS entry mode (field 22) to work
   according to the spec.  This code is from DBS.
   Error Report #388.
   
      Rev 1.13   19 Mar 1999 17:12:44   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.12   18 Mar 1999 15:34:26   dirby
   Created a separate function to send a PTE message to a destination queue.
   This new function, 'send_msg' calls 'pteipc_send' and handles send errors.
   This eliminates lots of repetitive code, especially on the error handling.
   
      Rev 1.11   16 Mar 1999 15:46:52   dirby
   Removed call to function 'perform_error_processing' for
   messages from the host in function cirsutil_parse_db_error.
   
   Changed network_type to BYTE in get_ncf01( ).
   
      Rev 1.10   16 Mar 1999 09:59:30   dirby
   Added string length checks where 'sprintf's are used to
   copy data lengths into HOST_MESSAGE.
   
      Rev 1.9   15 Mar 1999 16:28:20   dirby
   Added code to finish the requirement of pin key changes.
   
      Rev 1.8   12 Mar 1999 16:42:58   dirby
   Added functions to perform pin key exchange.
   Only the code to handle the initial request from the
   host is in place.  Still need to code the response.
   
      Rev 1.7   11 Mar 1999 14:24:12   dirby
   Changed 'actual_amount' to 'reversal_amount' in field 95 of
   function 'move_iso_response_to_auth_struct'.
   
   Added code in 'parse_db_ok' to handle ST2_REVERSAL, and
   added some error handling capabilities for reversals.
   
      Rev 1.6   10 Mar 1999 17:03:32   dirby
   Added use of 'orig_message' and 'orig_sys_trace_audit_num'
   in field 90 for reversals.
   
      Rev 1.5   09 Mar 1999 16:06:38   dirby
   Modified all calls to Find_Bitfield_Table to check for NULL
   upon return.
   
      Rev 1.4   08 Mar 1999 15:23:32   dirby
   Removed code that igores reversals.  That was left
   over from MasterCard.
   
      Rev 1.3   04 Mar 1999 16:47:12   dirby
   Updated for AIX port.
   
      Rev 1.2   03 Mar 1999 15:47:50   dirby
   Fixed bug in field 90 of build_host_structure.
   Fwd Id was overwriting Acq Id.
   
      Rev 1.1   25 Feb 1999 16:45:56   dirby
   Added return value checking to 'build_host_request'.
   Added code to set host timer for log on messages.
   
      Rev 1.0   19 Feb 1999 16:19:56   dirby
   Initial version
   
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#endif
#include <string.h>
#include <time.h>
#include <math.h>
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
#include "txutils.h"
#include "cirsmain.h"
#include "cirsutil.h"
#include "echo_timer.h"
#include "txtrace.h"


/* Global Variables */
extern CHAR  Settlement_ID[12];
extern CHAR  Stan[7];
extern CHAR  Logon_NMI_Code[4];
extern INT   DE96_State;
extern CHAR  AppName[8];

/* Database structures */
extern NCF01             ncf01;
extern NCF21             ncf21;
extern NCF30             ncf30;
extern TLF01             tlf01;

/* Shared memory variables */
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];
extern BYTE   current_state[2];

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf21_Size;
extern INT    Ncf30_Size;
extern INT    Saf01_Size;
extern INT    Auth_Tx_Size;

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
extern CHAR  updateds_que_name[];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;

extern INT   reqidx;                 /* next open position in request buffer */
extern INT   buflen;                 /* total size of buffer */
extern INT   bitmapidx;              /* offset of bitmap... */
extern INT   invalid_msg_format;     /* flag used in parsing host message */

extern BYTE ReqRsp_Buffer [COMM_BUF_SZE];
extern int  BitFieldSize;

extern char  A2E[4];    /*  ASCII to EBCDIC conversion */
extern char  E2A[4];    /*  EBCDIC to ASCII conversion */


const struct isobit BITTAB[128+1] =
{
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },    /* bit 1  */   { 0, 0x40 },    /* bit 2  */   { 0, 0x20 },    /* bit 3  */   { 0, 0x10 },    /* bit 4  */
   { 0, 0x08 },    /* bit 5  */   { 0, 0x04 },    /* bit 6  */   { 0, 0x02 },    /* bit 7  */   { 0, 0x01 },    /* bit 8  */
   { 1, 0x80 },    /* bit 9  */   { 1, 0x40 },    /* bit 10 */   { 1, 0x20 },    /* bit 11 */   { 1, 0x10 },    /* bit 12 */
   { 1, 0x08 },    /* bit 13 */   { 1, 0x04 },    /* bit 14 */   { 1, 0x02 },    /* bit 15 */   { 1, 0x01 },    /* bit 16 */
   { 2, 0x80 },    /* bit 17 */   { 2, 0x40 },    /* bit 18 */   { 2, 0x20 },    /* bit 19 */   { 2, 0x10 },    /* bit 20 */
   { 2, 0x08 },    /* bit 21 */   { 2, 0x04 },    /* bit 22 */   { 2, 0x02 },    /* bit 23 */   { 2, 0x01 },    /* bit 24 */
   { 3, 0x80 },    /* bit 25 */   { 3, 0x40 },    /* bit 26 */   { 3, 0x20 },    /* bit 27 */   { 3, 0x10 },    /* bit 28 */
   { 3, 0x08 },    /* bit 29 */   { 3, 0x04 },    /* bit 30 */   { 3, 0x02 },    /* bit 31 */   { 3, 0x01 },    /* bit 32 */
   { 4, 0x80 },    /* bit 33 */   { 4, 0x40 },    /* bit 34 */   { 4, 0x20 },    /* bit 35 */   { 4, 0x10 },    /* bit 36 */
   { 4, 0x08 },    /* bit 37 */   { 4, 0x04 },    /* bit 38 */   { 4, 0x02 },    /* bit 39 */   { 4, 0x01 },    /* bit 40 */
   { 5, 0x80 },    /* bit 41 */   { 5, 0x40 },    /* bit 42 */   { 5, 0x20 },    /* bit 43 */   { 5, 0x10 },    /* bit 44 */
   { 5, 0x08 },    /* bit 45 */   { 5, 0x04 },    /* bit 46 */   { 5, 0x02 },    /* bit 47 */   { 5, 0x01 },    /* bit 48 */
   { 6, 0x80 },    /* bit 49 */   { 6, 0x40 },    /* bit 50 */   { 6, 0x20 },    /* bit 51 */   { 6, 0x10 },    /* bit 52 */
   { 6, 0x08 },    /* bit 53 */   { 6, 0x04 },    /* bit 54 */   { 6, 0x02 },    /* bit 55 */   { 6, 0x01 },    /* bit 56 */
   { 7, 0x80 },    /* bit 57 */   { 7, 0x40 },    /* bit 58 */   { 7, 0x20 },    /* bit 59 */   { 7, 0x10 },    /* bit 60 */
   { 7, 0x08 },    /* bit 61 */   { 7, 0x04 },    /* bit 62 */   { 7, 0x02 },    /* bit 63 */   { 7, 0x01 },    /* bit 64 */

   { 8, 0x80 },    /* bit 1  */   { 8, 0x40 },    /* bit 2  */   { 8, 0x20 },    /* bit 3  */   { 8, 0x10 },    /* bit 4  */
   { 8, 0x08 },    /* bit 5  */   { 8, 0x04 },    /* bit 6  */   { 8, 0x02 },    /* bit 7  */   { 8, 0x01 },    /* bit 8  */
   { 9, 0x80 },    /* bit 9  */   { 9, 0x40 },    /* bit 10 */   { 9, 0x20 },    /* bit 11 */   { 9, 0x10 },    /* bit 12 */
   { 9, 0x08 },    /* bit 13 */   { 9, 0x04 },    /* bit 14 */   { 9, 0x02 },    /* bit 15 */   { 9, 0x01 },    /* bit 16 */
   { 10, 0x80 },   /* bit 17 */   { 10, 0x40 },   /* bit 18 */   { 10, 0x20 },   /* bit 19 */   { 10, 0x10 },   /* bit 20 */
   { 10, 0x08 },   /* bit 21 */   { 10, 0x04 },   /* bit 22 */   { 10, 0x02 },   /* bit 23 */   { 10, 0x01 },   /* bit 24 */
   { 11, 0x80 },   /* bit 25 */   { 11, 0x40 },   /* bit 26 */   { 11, 0x20 },   /* bit 27 */   { 11, 0x10 },   /* bit 28 */
   { 11, 0x08 },   /* bit 29 */   { 11, 0x04 },   /* bit 30 */   { 11, 0x02 },   /* bit 31 */   { 11, 0x01 },   /* bit 32 */
   { 12, 0x80 },   /* bit 33 */   { 12, 0x40 },   /* bit 34 */   { 12, 0x20 },   /* bit 35 */   { 12, 0x10 },   /* bit 36 */
   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit 40 */
   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit 44 */
   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit 48 */
   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit 52 */
   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit 56 */
   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit 60 */
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 64 */
};


/*****************************************************************************

  Function:   ascii_to_bin

  Description:
      Convert an ascii string to a binary string
  Author: 
      SVelaga   6/21/99
  Inputs:
      astr      - pointer to the incoming ascii string
      ascii_len - length of astr
      blen      - length of bstring
  Outputs:
      binary_string  - pointer to the outgoing binary string
  Return values:
      NONE
  Modified by:

******************************************************************************/
void ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
{

	BYTE temp_char;
	INT i , j = 0;

	
	memset(binary_string, 0x00, blen);
	

	for(i=0, j=0; ((i < blen ) && (j < ascii_len)); i++, j+=2)
	{
		 	
		if( astr[j] <= '9' )
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1] & 0x0f ));
			else
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));

		}
		else 
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1]  & 0x0f ));
			else
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));
		}
		 
			binary_string[i] = temp_char;
	}

}

/******************************************************************************
 *
 *  NAME:         ASCII_EBCDIC_CONV
 *
 *  DESCRIPTION:  This function converts an ASCII character into and an EBCDIC
 *                character, or visa versa.
 *
 *                    ascii_ebcdic_conv - IBM EBCDIC
 *                    ------------------------------
 *                    Convert ASCII to EBCDIC -or- EBCDIC to ASCII.
 *                    Use input parameter 2, either "A2E" or "E2A".
 *
 *  INPUTS:       conv_char - Character to be converted
 *                conv_type - A2E or E2A: indicates which way to convert
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Converted character
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 05/19/99 - Added '^' to the charts.
 *     DI - 08/11/99 - Changed EBCDIC value of '^' from 0x9A to 0x5F.
 ******************************************************************************/
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{

#define  CHART_SIZE  88

   INT   i;
   INT   found = 0;
   BYTE  ReturnChar;


      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[CHART_SIZE] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x5F
      };

   static unsigned char ascii_chart[CHART_SIZE] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      for( i=0; i<CHART_SIZE  &&  !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      for( i=0; i<CHART_SIZE  &&  !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}


/******************************************************************************
 *
 *  NAME:         vEBCDICMove
 *
 *  DESCRIPTION:  This function converts a variable length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.  The length is also copied
 *                into the buffer in EBCDIC format.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         cFieldData = ascii_ebcdic_conv((unsigned char)(pBF->p_bd_pdata[i]),A2E);
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }
   }

} /* end vEBCDICMove */



/******************************************************************************
 *
 *  NAME:         EBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         cFieldData = ascii_ebcdic_conv((unsigned char)(pBF->p_bd_pdata[i]),A2E);
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }
   }

} /* end EBCDICMove */



/******************************************************************************
 *
 *  NAME:         RspEBCDICvMove
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.  The length is
 *                also copied.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      for( i=0,movelen=0; i<FieldLen; i++ )
      {
         cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
         movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
      }
      movelen += pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }

} /* end RspEBCDICvMove */


/******************************************************************************
 *
 *  NAME:         RspEBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }

} /* end RspEBCDICMove */


/***********************************************
   GenericMove
************************************************/
void GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
             pBF->p_bd_pdata,         /* from data pointer      */
             pBF->bd_fieldlen);      /* for length of field    */

      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end GenericMove */



//added by farad on 8/13/98.

/***********************************************
   RspMoveGeneric
************************************************/
void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
      {
         memcpy(pBF->p_bd_pdata,         /* to data pointer          */
                &ReqRsp_Buffer[reqidx],  /* move from request buffer */
                pBF->bd_fieldlen);       /* for length of field      */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMoveGeneric */




/******************************************************************************
 *
 *  NAME:         get_variable_length
 *
 *  DESCRIPTION:  This function converts a variable length character string
 *                into an integer.
 *
 *  INPUTS:       DataField - Pointer to ASCII character string
 *                FieldLen  - Length of 'DataField'
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Integer value of 'DataField'
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
static int get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i;
   double len=0;

   if ( 0 < FieldLen )
   {
      for( i=0; i<FieldLen; i++ )
      {
         len += (int)( DataField[i] - 0x30 ) * (pow(10,(double)(FieldLen-i-1)));
      }
   }
   return( (int)len );
}


/******************************************************************************
 *
 *  NAME:         RJ_WITH_LEAD_ZEROS
 *
 *  DESCRIPTION:  This procedure right justifies a string, padding leading
 *                zeros.
 *
 *  INPUTS:       str      - Pointer to string to be right justified
 *                FieldLen - Length of str
 *
 *  OUTPUTS:      str - Contents of str is modified.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldLen )
{
   int i;
   int dif;
   int strlength;

   /* Convert any leading blanks to zero. */
   for( i=0; i< FieldLen; i++ )
   {
      if ( 0x20 == str[i] )
         memset( &str[i], 0x30, 1 );
      else
         break;
   }

   /* Remove any trailing blanks. */
   for( i=FieldLen-1; i>=0; i-- )
   {
      if ( 0x20 == str[i] )
         memset( &str[i], 0x00, 1 );
      else
         break;
   }

   /* Right justify, then prepend leading zeros. */
   strlength = strlen( str );
   dif       = FieldLen - strlength;

   if ( strlength < FieldLen )
   {
      for( i=FieldLen-1; i>=0; i-- )
      {
         if ( i >= dif )
            strncpy( &str[i], &str[i-dif], 1 );  /* Shift string to the right */
         else
            memset( &str[i], 0x30, 1 );          /* Prepend leading zeros     */
      }
   }
}

/******************************************************************************
 *
 *  NAME:         RspIgnore - Do not copy response field
 *
 *  DESCRIPTION:  This function is called when a host response is being parsed
 *                and we do not care to copy the contents of the field into
 *                the HOST_MESSAGE structure from the ReqRsp_Buffer.  The only
 *                thing that happens here is the ReqRsp_Buffer is incremented
 *                to point past this data.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end RspIgnore */


/******************************************************************************
 *
 *  NAME:         vRspIgnore - Do not copy variable length response field
 *
 *  DESCRIPTION:  This function is called when a host response is being parsed
 *                and we do not care to copy the contents of the field into
 *                the HOST_MESSAGE structure from the ReqRsp_Buffer.  The only
 *                thing that happens here is the ReqRsp_Buffer is incremented
 *                to point past this data.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      if ( (reqidx + FieldLen) <= buflen )
      {
         /* Determine number of bytes to move. */
         for( i=0,movelen=0; i<FieldLen; i++ )
         {
            cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
            movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
         }
         movelen += pBF->bd_fieldlen;

         if ( (reqidx + movelen) <= buflen )
            reqidx = reqidx + (int)movelen;   /* Increment buffer pointer */
         else
            invalid_msg_format = true;
      }
      else
      {
         invalid_msg_format = true;
      }
   }
} /* end vRspIgnore */


/******************************************************************************
 *
 *  NAME:         CondPan
 *
 *  DESCRIPTION:  This function copies the PAN field from HOST_MESSAGE into
 *                ReqRsp_Buffer if the PAN field has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT CondPan( enum bitfields bitfield, BYTE p_trantype )
{
   if ( 0x00 != HOST_MESSAGE.pan_len [0] )
       return( Move_IfThere(bitfield, p_trantype) );

   return( false );
} /* end CondPan */


/******************************************************************************
 *
 *  NAME:         CondExpd
 *
 *  DESCRIPTION:  This function copies the date expiration field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the date expiration field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT CondExpd( enum bitfields bitfield, BYTE p_trantype )
{
   /* if track2 data here, no need for expdate field */
   if ( 0x00 != HOST_MESSAGE.date_expd [1] )
      return( true );

   return( false );
} /* end CondExpd */


/******************************************************************************
 *
 *  NAME:         CondProc
 *
 *  DESCRIPTION:  This function copies the processing code field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the processing code field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
static  INT CondProc(enum bitfields bitfield, BYTE p_trantype)
{
   if ( 0x04 != HOST_MESSAGE.msgtype [0] )
      return ( true );

   return( false);
} /* end CondProc */



/******************************************************************************
 *
 *  NAME:         CondTrack1
 *
 *  DESCRIPTION:  This function copies the track 1 field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the track 1 field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT CondTrack1( enum bitfields bitfield, BYTE p_trantype )
{
   if ( 0x00 != HOST_MESSAGE.track1_len [0] )
       return( true );

   return( false );
} /* end CondTrack1 */


/******************************************************************************
 *
 *  NAME:         CondDE96
 *
 *  DESCRIPTION:  This function returns the state of the global variable,
 *                DE96_State.  If it is YES, this means to send field 96
 *                in a message to the host.  If it is NO, do not send it.
 *
 *  INPUTS:       (Global) DE96_State
 *                The parameter list is ignored.  It is there only to be
 *                implemented as a variable function.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if DE96_State = YES, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT CondDE96( enum bitfields bitfield, BYTE p_trantype )
{
   INT  ret_val = false;

   if ( DE96_State == YES )
      ret_val = true;

   return( ret_val );
} /* end CondDE96 */


/******************************************************************************
 *
 *  NAME:         Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specified HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table( fieldid );
   if (pBF != NULL)
   {

      /*---------------------------------------------------*/
      /* in any byte non-zero, it's there, return true     */
      /*---------------------------------------------------*/
      for ( i=0; i < pBF->bd_fieldlen; i++ )
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
         return( true );
      }
   }
   return( false );
} /* end Move_IfThere */


/******************************************************************************
 *
 *  NAME:         Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved from HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end Move_Always */


/******************************************************************************
 *
 *  NAME:         Move_Never
 *
 *  DESCRIPTION:  This function returns false.  It is used to indicate that data
 *                SHOULD NOT be moved from HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
static  INT Move_Never( enum bitfields bitfield, BYTE p_trantype )
{
   return( false );
} /* end Move_Never */


/******************************************************************************
 *
 *  NAME:         CONVERT_FROM_JULIAN_DAY
 *
 *  DESCRIPTION:  This procedure converts a date in format YYDDD into a date
 *                format of MMDD.
 *
 *  INPUTS:       julian_str - Date string in format YYDDD
 *
 *  OUTPUTS:      date_time  - Date string in format MMDD
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void convert_from_julian_day( char *julian_str, char *date_time )
{
   char  Year[3];
   char  Month[3];
   char  Days[4];
   int   iYear;
   int   NumDays;
   int   JulianDay;
   int   iMonth;
   int   i;
   int   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   /* Account for Leap Year */
   strncpy( Year, julian_str, 2 );
   iYear = atoi( Year );
   if ( 0 == ( iYear % 4 ) )
      DaysOfMonth[1]++;

   strncpy( Days, &julian_str[2], 3 );
   JulianDay = atoi( Days );
   NumDays = 0;
   for( i=0; i<12; i++ )
   {
      if ( NumDays+DaysOfMonth[i] >= JulianDay )
      {
         iMonth = i+1;
         NumDays = JulianDay - NumDays;
         break;
      }
      else
      {
         NumDays += DaysOfMonth[i];
      }
   }

   sprintf( Days, "%02d", NumDays );
   sprintf( Month, "%02d", iMonth );

   strncpy( date_time, Month, 2 );
   strncat( date_time, Days,  2 );
}


/******************************************************************************
 *
 *  NAME:         CIRSUTIL_SEND_NET_MNGMNT_RESPONSE
 *
 *  DESCRIPTION:  This procedure will send the network management response to
 *                the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record going to the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if unsuccessful
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT cirsutil_send_net_mngmnt_response( pAUTH_TX p_auth_tx )
{
   pPTE_MSG  p_msg_host_out = NULL_PTR;
   BOOLEAN   ret_code = false;

   build_host_structure( p_auth_tx );
   if ( false == build_host_request( p_auth_tx ) )
   {
      cirsutil_error_handler(1,
                          "Failed to build host request message",
                          0, INFO_MSG, "cirsutil_send_net_mngmnt_response",
                          2, INFO_ERROR,
                          p_auth_tx->TLF01_details.terminal_id,
                          p_auth_tx->TLF01_details.card_num,
                          p_auth_tx->TLF01_details.merchant_id,
                          "520");
   }
   else
   {
      /* Build PTE message. */
      p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
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
                             1, ERROR_MSG, "cirsutil_send_net_mngmnt_response",
                             4, FATAL_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "507");
      }
      else
      {
         ptemsg_set_orig_comms_info( p_msg_host_out,
                                     p_auth_tx->TLF01_details.tpdu_id);
         ret_code = send_msg( p_msg_host_out, dialog_que_name, p_auth_tx );
         free( p_msg_host_out );
         if ( true == ret_code )
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

            /* Log transaction to trace file. */
            write_to_txn_file();

            cirsutil_error_handler(1,
                          "Network Management response message sent to host",
                          0, INFO_MSG, "cirsutil_send_net_mngmnt_response",
                          0, INFO_ERROR,
                          p_auth_tx->TLF01_details.terminal_id,
                          p_auth_tx->TLF01_details.card_num,
                          p_auth_tx->TLF01_details.merchant_id,
                          "509");
         }
      }
   }
   return( (INT)ret_code );
} /* cirsutil_send_net_mngmnt_response */


/******************************************************************************
 *
 *  NAME:         CIRSUTIL_SEND_NET_MNGMNT_REQUEST
 *
 *  DESCRIPTION:  This procedure will send the network management request to
 *                the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record going to the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if unsuccessful
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT cirsutil_send_net_mngmnt_request( pAUTH_TX p_auth_tx )
{
   pPTE_MSG	 p_msg_host_out = NULL_PTR;
   BOOLEAN   ret_code = false;

   build_host_structure( p_auth_tx );
   if ( false == build_host_request( p_auth_tx ) )
   {
      cirsutil_error_handler(1,
                          "Failed to build host request message",
                          0, INFO_MSG, "cirsutil_send_net_mngmnt_request",
                          2, INFO_ERROR,
                          p_auth_tx->TLF01_details.terminal_id,
                          p_auth_tx->TLF01_details.card_num,
                          p_auth_tx->TLF01_details.merchant_id,
                          "429");
   }
   else
   {
      /* Build PTE message. */
      p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
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
                             1, ALERT_MSG, "cirsutil_send_net_mngmnt_request",
                             4, FATAL_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "510");
      }
      else
      {
         /* Send message to host. */
         ptemsg_set_orig_comms_info( p_msg_host_out,
                                     p_auth_tx->TLF01_details.tpdu_id);
         ret_code = send_msg( p_msg_host_out, dialog_que_name, p_auth_tx );
         free( p_msg_host_out );
         if ( ret_code == true )
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

            /* Log transaction to trace file. */
            write_to_txn_file();
         }
      }
   }
   return( (INT)ret_code );
} /* cirsutil_send_net_mngmnt_request */


/******************************************************************************
 *
 *  NAME:         FIND_CVC_FLAG
 *
 *  DESCRIPTION:  This procedure parses field 48 of a 0110 response message
 *                looking for Tag 87.  This tag contains a 1 byte flag:
 *                Magnetic Stripe / CVC Invalid.
 *
 *  FIELD 48 FORMAT:
 *
 *     -------+------+---------+---------+---------+---------+-----
 *      Total |Trans | First   | First   | First   | Second  |
 *       Len  | Cat. |SubField |SubField |SubField |SubField | . . .
 *            | Code |  Tag    | Length  |  Data   |  Tag    |
 *     -------+------+---------+---------+---------+---------+-----
 * BYTES: 3       1       2         2         x         2
 *
 *
 *  INPUTS:       src_len - Length of 'src'.
 *                src     - String containing field 48 data
 *
 *  OUTPUTS:      dest - String containing the Tag 87 flag
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void find_cvc_flag( char *src_len, char *src,  char *dest, INT IndexStart_Value)
{
   #define  FIELD48_MAX_LEN  100
   #define  FIELD48_TAG      "87"
   INT  sub_field_len;
   CHAR tag[3];
   CHAR len[3];
   INT  MainIndex;
   INT  TotalLen;

   TotalLen = atoi( src_len );

   /* MainIndex starts at 1 to skip over the Transaction Category Code
    * for 0100 messages and responses.  All other message types will
    * not have a Transaction Category Code, so MainIndex starts at zero.
    */
   for( MainIndex=IndexStart_Value; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;

      memset( len, 0x00, sizeof(len) );
      memcpy( len, &src[MainIndex], 2 );
      sub_field_len = atoi( len );
      MainIndex += 2;

      if ( strcmp( tag, FIELD48_TAG ) == 0 )
      {
         strncpy( dest, &src[MainIndex], sub_field_len );
         break;
      }
      else
      {
         MainIndex += sub_field_len;
      }
   }
}  /* End find_cvc_flag */


/******************************************************************************
 *
 *  NAME:         Find_Tran_Table
 *
 *  DESCRIPTION:  This procedure finds a transaction type in the transaction
 *                table where the transactions are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific transaction definition in the
 *                transaction table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
static  struct trandef *Find_Tran_Table( pAUTH_TX p_auth_tx )
{
   INT  i;            /* loop index  */
   BYTE trantype;

   trantype = p_auth_tx->TLF01_details.tx_key;
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); i++ )
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         return( &TRANTABLE[i] );
         break;
      }
   }

   /* Did not find trantype in table - log error & return NULL */
   cirsutil_error_handler(1,
                          "This trantype not found in TRANTABLE",
                          0, ERROR_MSG, "FIND_TRAN_TABLE",
                          2, WARNING_ERROR,
                          p_auth_tx->TLF01_details.terminal_id,
                          p_auth_tx->TLF01_details.card_num,
                          p_auth_tx->TLF01_details.merchant_id,
                          "440");

   return NULL;
} /* end Find_Tran_Table */   


/******************************************************************************
 *
 *  NAME:         Find_Bitfield_Table
 *
 *  DESCRIPTION:  This procedure finds a bitfield definition in the bitfield
 *                table where the bitfields are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       bitfield - ID of bitfield definition we are looking for
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific bitfield definition in the
 *                bitfield table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
static  struct bitfield_data *Find_Bitfield_Table( enum bitfields bitfield )
{
   int      i;                    /* loop index */
   char     temp_str[60];

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */

   /* Field not found in table - log error and return NULL */
   sprintf(temp_str, "This bitfield not found in BFData, bitfield: %d",
                      bitfield);
   cirsutil_error_handler(1,
                          temp_str,
                          0, ERROR_MSG, "Find_Bitfield_Table",
                          2, WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"441");
   return NULL;

} /* end Find_Bitfield_Table */


/******************************************************************************
 *
 *  NAME:         format_date
 *
 *  DESCRIPTION:  This function accepts a pointer to an empty character
 *                string.  The string will be filled by this function.  It
 *                will fill it with a date in the format:yyyymmddhhmmss
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      temp - String containing system date: YYYYMMDDhhmmss
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************
void format_date(pCHAR temp)
{
   struct   _timeb   time_structure;// Time for ftime
   time_t long_time_since_1970;     // Number of seconds since 01/01/1970 A.D. 12:00 A.M.
   struct tm *current_time;         // Current time structure
   char temp1[4] = "0";

   _ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);

} */


/******************************************************************************
*
* FUNCTION:     cirsutil_send_host_response
*
* DESCRIPTION:  Sends response back to host.
*
* INPUTS:       p_auth_tx - Transaction record
*
* OUTPUTS:      None
*
* RETURN:       true or false
*
* AUTHOR:       Dennis Irby
*
* MODIFICATIONS
*******************************************************************************/
BOOLEAN cirsutil_send_host_response( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_out;
   BOOLEAN    ret_code = false;
   WORD       timer_length;

   p_msg_out = ptemsg_build_msg( MT_OUTGOING,
                                 ST1_NONE,
                                 ST2_NONE,
                                 dialog_que_name,
                                 application_que_name,
                                 ReqRsp_Buffer,
                                 buflen,
                                 0); 
   if ( NULL_PTR == p_msg_out )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory to build host message",
                             1, ALERT_MSG, "cirsutil_send_host_response",
                             4, FATAL_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id, "442" );
   }
   else
   {
      ptemsg_set_orig_comms_info( p_msg_out,
                                  p_auth_tx->TLF01_details.tpdu_id );
      ret_code = send_msg( p_msg_out, dialog_que_name, p_auth_tx );
      free( p_msg_out );
      if ( ret_code == true )
      {
         /* Update transaction statistics */
         update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

         /* Log transaction to trace file. */
         write_to_txn_file();
      }

      /* Clear and set the idle line timer. */
      timer_length = atoi( ncf01.echo_interval );
      if ( (0 == strcmp(NULL_STR, ncf01.echo_interval)) || timer_length <=0 )
      {
         if ( false == chain_clear_set_echo_timer( ECHO_TIMER_KEY, timer_length,
                                                   application_que_name ) )
         {
            cirsutil_error_handler(1,
                     "Problem with clearing and setting idleline timer.",
                      1, WARN_MSG, "cirsutil_send_host_response", 3,
                      WARNING_ERROR, p_auth_tx->TLF01_details.terminal_id,
                      p_auth_tx->TLF01_details.card_num,
                      p_auth_tx->TLF01_details.merchant_id, "803" );
         }
      }
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         BUILD_HOST_REQUEST
 *
 *  DESCRIPTION:  This procedure copies the information from the HOST_MESSAGE
 *                structure into a character string.  The character string is
 *                then shipped to the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if unsuccessful
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/03/99 - Changed AUTH_REVERSAL to AUTH_REVERSAL_ADVICE.
 ******************************************************************************/
INT build_host_request( pAUTH_TX p_auth_tx )
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   BYTE                 trantype;
   CHAR                 Buff[100];
   int                  bfidx;         /* index to bitfields for this transaction */

   /* Get pointer to transaction table */
     trantype = p_auth_tx->TLF01_details.tx_key;
   pTranDef = Find_Tran_Table( p_auth_tx );

   if ( NULL == pTranDef )
      return false; /* Did not find TX type - so quit */
       
   /*-----------------------------------------*/
   /* Copy in Message Id for this transaction */
   /*-----------------------------------------*/
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   bitmapidx = 4;                            /* point to bitmap     */

   /* Point past bitmap   */
   switch( trantype )
   {
      case AUTH_REVERSAL_ADVICE:
      case AUTH_ISSUER_REVERSAL_ADVICE:
      case AUTH_LOGON:
      case AUTH_ADMIN_ADVICE:

      case AUTH_REVERSAL_ADVICE_RESPONSE:
      case AUTH_ISSUER_REVERSAL_ADVICE_RESPONSE:
      case AUTH_LOGON_RESPONSE:
      case AUTH_ADMIN_ADVICE_RESPONSE:
      case AUTH_RECONCILE_ADVICE_RESPONSE:
      case AUTH_ISSUER_RECONCILE_ADVICE_RESPONSE:

         reqidx = 20;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
         break;

      default:

         reqidx = 12;
         BitFieldSize = NORMAL_BITFIELD;
         break;
   }

   memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   /*-----------------------------------------------------*/
   /* For all requested bitfields for this transaction... */
   /*-----------------------------------------------------*/
   for (bfidx=0; bfidx < BitFieldSize; bfidx++)
   {
      /*----------------------------*/
      /* if not end of bitfields... */
      /*----------------------------*/
      if (pTranDef->td_bitfields[bfidx] != bf_done)
      {
         /*---------------------------------------------------*/
         /* find pointer to bitfield info for this bitfield...*/
         /*---------------------------------------------------*/
         pBF = Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

		 if ( NULL == pBF )
         {
            sprintf( Buff,"Unable to find bitfield (%d) to build msg for host.",
                     bfidx );
            cirsutil_error_handler(1,
                             Buff,
                             1, ALERT_MSG, "build_host_request", 4,
                             FATAL_ERROR, p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id, "502" );
            return false; /* Invalid bit was set - so quit. */
		 }

         /*-----------------------------------------------------------*/
         /* if conditional routine returns true, move the bitfield... */
         /*-----------------------------------------------------------*/
         if ((pBF->bd_BFCond)(pBF->bd_fieldid,trantype) == true)
         {
            /*------------------------------*/
            /* set the bit in the bitmap... */
            /*------------------------------*/
            ReqRsp_Buffer[bitmapidx + BITTAB[pTranDef->td_bitfields[bfidx]].byte_no] |=
                  BITTAB[pTranDef->td_bitfields[bfidx]].bit_mask;

            /*--------------------------*/
            /* call the move routine... */
            /*--------------------------*/
            (pBF->bd_BFMove)(pBF->bd_fieldid);
         }  /* end if */
      }
      else
      {
         bfidx = BitFieldSize;
      }  /* end if */
   } /* end for */
   
   buflen = reqidx;
   return( true );
} /* end build_host_request */



/******************************************************************************
 *
 *  NAME:         CIRSUTIL_PARSE_HOST_RESPONSE_MSG
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into the HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if unsuccessful
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/08/99 - Removed code that ignored reversals.  This was left
 *                     over from MasterCard code.
 ******************************************************************************/
INT cirsutil_parse_host_response_msg()
{
   struct bitfield_data *pBF;
   CHAR                  Buff[100];
   INT                   bfidx;    /* Index to bitfields for this transaction */
   INT                   i;


   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   memcpy( HOST_MESSAGE.msgtype, &ReqRsp_Buffer[0],
           sizeof(HOST_MESSAGE.msgtype) );

   /* Convert Message Type from EBCDIC to ASCII. */
   for( i=0; i<4; i++ )
      HOST_MESSAGE.msgtype[i] = ascii_ebcdic_conv(HOST_MESSAGE.msgtype[i], E2A);

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 20;                         /* Extended     */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      reqidx = 12;                        /* Not Extended */
      BitFieldSize = NORMAL_BITFIELD;
   }

   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      if ( ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table(bfidx);

         if ( NULL == pBF )
         {
            sprintf( Buff, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( Buff, " MsgType = " );
            strncat( Buff, HOST_MESSAGE.msgtype, 4 );
            cirsutil_error_handler(1,
                             Buff,
                             1, ALERT_MSG, "cirsutil_parse_host_response_msg",4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "501" );
            return false; /* Invalid field in message - so quit. */
		 }

         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         (pBF->bd_BFRsp)(pBF->bd_fieldid);
         if ( true == invalid_msg_format )
         {
            strcpy( Buff, "Invalid Cirrus response message format." );
            strcat( Buff, " MsgType = " );
            strncat( Buff, HOST_MESSAGE.msgtype, 4 );
            cirsutil_error_handler(1, Buff,
                             1, ALERT_MSG, "cirsutil_parse_host_response_msg",4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "597" );
            return( false );
         }
      }
   }
   return( true );
} /* cirsutil_parse_host_response_msg */   


/******************************************************************************
 *
 *  NAME:         MOVE_ISO_RESPONSE_TO_AUTH_STRUCT
 *
 *  DESCRIPTION:  This procedure copies information from the HOST_MESSAGE
 *                structure into the auth_tx structure.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      p_auth_tx - Transaction record containing data from host
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void move_iso_response_to_auth_struct( pAUTH_TX p_auth_tx )
{
   CHAR  tempstr[100];
   CHAR  itemp[4];
   CHAR  field_len[4];
   CHAR  date_str [ 9];
   CHAR  time_date[25];
   INT   total_amount;
   INT   actual_amount;
   INT   rev_amount;
   INT   ctr;
   INT   iF60_len;
   INT   F54_len;
   INT   TCC;    /* Used for field 48. */

   /* MESSAGE TYPE */
   memcpy( p_auth_tx->TLF01_details.message_type,
           HOST_MESSAGE.msgtype, sizeof(HOST_MESSAGE.msgtype ) );

   /* FIELD 2: PAN */
   memcpy( p_auth_tx->TLF01_details.card_num_len,HOST_MESSAGE.pan_len,bf2_len);
   memcpy( p_auth_tx->TLF01_details.card_num, HOST_MESSAGE.pan,
           sizeof(HOST_MESSAGE.pan) );

   /* FIELD 3: PROCESSING CODE */
   memcpy( p_auth_tx->TLF01_details.processing_code,
           HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode) );

   /* Hypercom proc code for Balance Inquiry is 31nnnn, for Cirrus 30nnnn. */
   if ( 0 == strncmp( "02", p_auth_tx->TLF01_details.message_type, 2 ) &&
        0 == strncmp( "30", p_auth_tx->TLF01_details.processing_code, 2 ) )
      memcpy( p_auth_tx->TLF01_details.processing_code, "31", 2 );


   /* FIELD 4: AMOUNT  in case this is a Balance Inquiry transaction. */
   memcpy( p_auth_tx->TLF01_details.total_amount,
           HOST_MESSAGE.amount, bf4_len );

   /* FIELD 5: SETTLEMENT AMOUNT */
   if ( HOST_MESSAGE.settle_amt[0] != 0x00 )
      memcpy( p_auth_tx->TLF01_details.settlement_total,
              HOST_MESSAGE.settle_amt, bf5_len );

   /* FIELD 7: TRANSMISSION TIME - MUST KEEP IT FOR 0620's */
   memset(  p_auth_tx->TLF01_details.transmission_timestamp, 0x00,
            sizeof(p_auth_tx->TLF01_details.transmission_timestamp));
   memcpy( p_auth_tx->TLF01_details.transmission_timestamp,
           HOST_MESSAGE.gmt_time, bf7_len );

   /* FIELD 9: SETTLEMENT CONVERSION RATE */
   if ( HOST_MESSAGE.conv_rate_stl[0] != 0x00 )
      memcpy( p_auth_tx->settlement_conv_rate,
              HOST_MESSAGE.conv_rate_stl, bf9_len );

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   memcpy( p_auth_tx->TLF01_details.sys_trace_audit_num,
           HOST_MESSAGE.stan, sizeof(HOST_MESSAGE.stan) );

   /* FIELD 12: LOCAL TRANSACTION TIME */
   if ( HOST_MESSAGE.time[0] != 0x00 )
      memcpy(p_auth_tx->TLF01_details.time_hhmmss, HOST_MESSAGE.time, bf12_len);

   /* FIELD 13: LOCAL TRANSACTION DATE */
   if ( HOST_MESSAGE.date[0] != 0x00 )
   {
      /* Get local time in order to get the year. */
      memset( date_str,  0x00, sizeof(date_str)  );
      memset( time_date, 0x00, sizeof(time_date) );
      ptetime_get_timestamp( time_date );
      get_date( time_date, date_str );

      memset( p_auth_tx->TLF01_details.date_yyyymmdd, '0', 4 );
      memcpy( p_auth_tx->TLF01_details.date_yyyymmdd+4,
              HOST_MESSAGE.date, bf13_len );
      memcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str, 4 ); /* Year */
   }

   /* FIELD 15: SETTLEMENT DATE (MMDD) */
   memcpy( p_auth_tx->TLF01_details.settlement_date,
           HOST_MESSAGE.date_settle, bf15_len );

   /* FIELD 16: CONVERSION DATE (MMDD) */
   if ( HOST_MESSAGE.conv_date[0] != 0x00 )
      memcpy( p_auth_tx->conv_date, HOST_MESSAGE.conv_date, bf16_len );

   /* FIELD 18: MERCHANT TYPE / ACQUIRER SIC NUMBER */
   memcpy( p_auth_tx->TLF01_details.category_code,
           HOST_MESSAGE.sic_number, bf18_len );

   /* FIELD 28: TRANSACTION FEE AMOUNT */
   if ( HOST_MESSAGE.tran_fee[0] != 0x00 )
   {
      p_auth_tx->TLF01_details.cvc_data[0] = HOST_MESSAGE.tran_fee[0];
      memcpy( p_auth_tx->TLF01_details.tran_fee_amount,
             &HOST_MESSAGE.tran_fee[1], bf28_len-1 );
   }

   /* FIELD 29: SETTLEMENT FEE AMOUNT */
   if ( HOST_MESSAGE.setl_fee[0] != 0x00 )
      memcpy( p_auth_tx->ch_billing_amt, HOST_MESSAGE.setl_fee, bf29_len );

   /* FIELD 32: ACQUIRING INSTITUTION IDENTIFICATION CODE */
   memcpy( p_auth_tx->TLF01_details.acquiring_id,
           HOST_MESSAGE.acq_inst_id, bf32data_len );

   /* FIELD 33: FORWARDING INSTITUTION IDENTIFICATION CODE */
   memcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
           HOST_MESSAGE.fwd_inst_id, bf33data_len );

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy( p_auth_tx->TLF01_details.retrieval_ref_num,
           HOST_MESSAGE.rrn, bf37_len );

   /* FIELD 38: AUTH NUMBER */
   memcpy( p_auth_tx->TLF01_details.auth_number,
           HOST_MESSAGE.authcode, bf38_len );

   /* FIELD 39: RESPONSE CODE */
   memcpy( p_auth_tx->TLF01_details.response_code,
           HOST_MESSAGE.responsecd, bf39_len );

   /* FIELD 41: TERMINAL ID */
   memcpy( p_auth_tx->TLF01_details.terminal_id,
           HOST_MESSAGE.termid, bf41_len );

   /* Do this in case terminal Id is not 8 characters.
    * For example, DBS has 7 character terminal Ids.
    */
   trim_spaces( p_auth_tx->TLF01_details.terminal_id );

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
   memcpy( p_auth_tx->TLF01_details.resp_source_len,
           HOST_MESSAGE.addl_rsp_len, sizeof(HOST_MESSAGE.addl_rsp_len) );
   memcpy( p_auth_tx->TLF01_details.resp_source,
           HOST_MESSAGE.addlrspdata, sizeof(HOST_MESSAGE.addlrspdata) );

   /* FIELD 48: ADDITIONAL DATA - ONLY NEED THE CVC STATUS FLAG. */
   memset( tempstr, 0x00, sizeof( tempstr ) );
   memset( field_len,   0x00, sizeof( field_len   ) );
   memcpy( field_len,HOST_MESSAGE.addl_data_len,sizeof(HOST_MESSAGE.addl_data_len));

   if ( (0 == strncmp( "01", p_auth_tx->TLF01_details.message_type, 2 ))  ||
        (0 == strncmp( "02", p_auth_tx->TLF01_details.message_type, 2 ))    )
      TCC = 1;   /* Transaction Category Code should be in field 48. */
   else
      TCC = 0;   /* Field 48 will not have TCC. */

   find_cvc_flag( field_len, HOST_MESSAGE.addl_data, tempstr, TCC );
   p_auth_tx->TLF01_details.cvc = tempstr[0];

   /* FIELD 48: ADDITIONAL DATA FOR REJECT MESSAGES */
   memset( addl_data_len, 0x00, sizeof( addl_data_len ) );
   memset( addl_data,     0x00, sizeof( addl_data     ) );

   memcpy( addl_data_len, HOST_MESSAGE.addl_data_len, 3 );
   memcpy( addl_data, HOST_MESSAGE.addl_data, sizeof(HOST_MESSAGE.addl_data) );

   if ( 0 == strncmp( "08", p_auth_tx->TLF01_details.message_type, 2 ))
   {
      /* The INF field is not used for 0800s, so save this info here. */
      strcpy( p_auth_tx->int_net_facility_len,  addl_data_len );
      strcpy( p_auth_tx->int_net_facility_data, addl_data     );
   }

   /* FIELD 49: TRANSACTION CURRENCY CODE */
   memcpy( p_auth_tx->TLF01_details.currency_code,
           HOST_MESSAGE.tran_curr_cd, bf49_len );

   /* FIELD 50: SETTLEMENT CURRENCY CODE */
   memcpy( p_auth_tx->settlement_curr_code,
           HOST_MESSAGE.settle_cur_cd, bf50_len );

   /* FIELD 54: ADDITIONAL AMOUNT */
   if ( 0 == strcmp( "0210", p_auth_tx->TLF01_details.message_type ) )
   {
      /*
       +----------------------------------------------------------------+
       | Just get current and available amounts:                        |
       |    Current amount   -> Amount Type = '01'                      |
       |    Available amount -> Amount Type = '02'                      |
       |                                                                |
       | Field 54 is made up of subfields as follows:                   |
       |      +-----------+----------+-----------+--------+--------+    |
       |      | Acct Type | Amt Type | Curr Code | D or C | Amount |    |
       |      +-----------+----------+-----------+--------+--------+    |
       |   Position: 1-2     3-4        5-7         8        9-20       |
       |   where                                                        |
       |      Acct Type = Account Type                                  |
       |                     10 = Savings                               |
       |                     20 = Checking                              |
       |                     30 = Credit card                           |
       |      Amt Type  = Amount Type                                   |
       |                     01 = Ledger Balance (Current balance)      |
       |                     02 = Available Balance                     |
       |                     40 = Cash-back                             |
       |                     90 = Available Credit                      |
       |                     91 = Credit Limit                          |
       |      Currency Code                                             |
       |      D or C    = D is Debit (negative), C is Credit (positive) |
       |      Amount    = Account balance                               |
       +----------------------------------------------------------------+
      */
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.addl_len, bf54_len );
      F54_len = atoi(tempstr);
      if ( 120 < F54_len )
         F54_len = 120;
      for( ctr=2; ctr<F54_len; ctr+=20 )
      {
         if ( 0 == strncmp( "01", &HOST_MESSAGE.addlamts[ctr], 2 ) )
         {
            /* Current Balance */
            if ( 'D' == HOST_MESSAGE.addlamts[ctr+5] )
               p_auth_tx->current_balance_sign = '-';
            else
               p_auth_tx->current_balance_sign = '+';

            memcpy( p_auth_tx->current_balance,
                    &HOST_MESSAGE.addlamts[ctr+9], 9 );

            /* Copy into this field for nceqit. */
            memcpy( p_auth_tx->ACF01_details.cash_available,
                    p_auth_tx->current_balance, 9 );
         }
         else if ( 0 == strncmp( "02", &HOST_MESSAGE.addlamts[ctr], 2 ) )
         {
            /* Available Balance */
            if ( 'D' == HOST_MESSAGE.addlamts[ctr+5] )
               p_auth_tx->available_balance_sign = '-';
            else
               p_auth_tx->available_balance_sign, '+';

            memcpy( p_auth_tx->available_balance,
                    &HOST_MESSAGE.addlamts[ctr+9], 9 );

            /* Copy into this field for nceqit. */
            memcpy( p_auth_tx->ACF01_details.cash_limit,
                    p_auth_tx->available_balance, 9 );
         }
      }
   }

   /* FIELD 60: ADVICE REASON CODE */
   memset( F60_len,            0x00, sizeof( F60_len )            );
   memset( reject_advice_code, 0x00, sizeof( reject_advice_code ) );
   memset( reject_detail_code, 0x00, sizeof( reject_detail_code ) );
   memset( reject_text,        0x00, sizeof( reject_text )        );

   memcpy( F60_len, HOST_MESSAGE.advice_cd_len, 3 ); 
   iF60_len = atoi( F60_len );
   if ( iF60_len < 0  ||  iF60_len >= 63 )
      iF60_len = 0;
   memcpy( reject_advice_code, HOST_MESSAGE.advice_code, 3 );
   if ( 3 < iF60_len )
   {
      memcpy( reject_detail_code, &HOST_MESSAGE.advice_code[3], 4 );
   }
   if ( 7 < iF60_len )
   {
      memcpy( reject_text, &HOST_MESSAGE.advice_code[7], iF60_len-7 );
   }

   /* FIELD 62: INTERMEDIATE NETWORK FACILITY (INF) DATA */
   if ( 0 != strcmp( NULL_STR, HOST_MESSAGE.inf_data ) )
      memcpy( p_auth_tx->int_net_facility_data, HOST_MESSAGE.inf_data,
              sizeof(HOST_MESSAGE.inf_data) );

   /* FIELD 63: BANKNET DATA */
   memcpy( p_auth_tx->TLF01_details.mcard_banknet_len,
           HOST_MESSAGE.banknet_len, sizeof( HOST_MESSAGE.banknet_len ) );
   memcpy( p_auth_tx->TLF01_details.mcard_banknet,
           &HOST_MESSAGE.banknet[0],
           sizeof( p_auth_tx->TLF01_details.mcard_banknet ) );

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   memset( Logon_NMI_Code, 0x00, sizeof(Logon_NMI_Code) );
   memcpy(Logon_NMI_Code,HOST_MESSAGE.nmi_code,sizeof(HOST_MESSAGE.nmi_code));
   if ( 0 == strncmp( "08", p_auth_tx->TLF01_details.message_type, 2 ) )
      /* Need to preserve it for 0800 messages. */
      strcpy( p_auth_tx->function_code, Logon_NMI_Code );

   /* RECONCILIATION TOTALS - 052n MESSAGES */
   if ( 0 == strncmp("052", p_auth_tx->TLF01_details.message_type,3) )
   {
      memset( &ncf21, 0x00, sizeof(NCF21) );

      /* FIELD 74: NUMBER OF CREDITS */
      memcpy( ncf21.credit_cnt, HOST_MESSAGE.credit_cnt, bf74_len );

      /* FIELD 75: NUMBER OF CREDIT REVERSALS */
      memcpy( ncf21.credit_rev_cnt, HOST_MESSAGE.crd_rev_cnt, bf75_len );

      /* FIELD 76: NUMBER OF DEBITS */
      memcpy( ncf21.debit_cnt, HOST_MESSAGE.debit_cnt, bf76_len );

      /* FIELD 77: NUMBER OF DEBITS REVERSALS */
      memcpy( ncf21.debit_rev_cnt, HOST_MESSAGE.dbt_rev_cnt, bf77_len );

      /* FIELD 86: CREDIT AMOUNT */
      memcpy( ncf21.credit_amt, HOST_MESSAGE.credit_amt, bf86_len );

      /* FIELD 87: CREDIT REVERSAL AMOUNT */
      memcpy( ncf21.credit_rev_amt, HOST_MESSAGE.crd_rev_amt, bf87_len );

      /* FIELD 88: DEBIT AMOUNT */
      memcpy( ncf21.debit_amt, HOST_MESSAGE.debit_amt, bf88_len );

      /* FIELD 89: DEBIT REVERSAL AMOUNT */
      memcpy( ncf21.debit_rev_amt, HOST_MESSAGE.dbt_rev_amt, bf89_len );

      /* FIELD 97: NET SETTLEMENT AMOUNT */
      memcpy( ncf21.total_amt_type, HOST_MESSAGE.net_setl_code, 1 );
      memcpy( ncf21.total_amt, HOST_MESSAGE.net_setl, bf97_len );

      /* FIELD 99: SETTLEMENT INSTITUTION ID CODE */
      memset( Settlement_ID, 0x00, sizeof(Settlement_ID) );
      memset( itemp, 0x00, sizeof( itemp ) );
      strncpy( itemp, HOST_MESSAGE.setl_id_len, bf99_len );
      memcpy( Settlement_ID, HOST_MESSAGE.setl_id, atoi(itemp) );
   }

   /* FIELD 90: ORIGINAL DATA ELEMENTS */
   if ( HOST_MESSAGE.orig_data[0] != 0x00 )
   {
      /* 
        +-------------------------------------------------+
        | BYTES  1- 4:  Original Message Type             |
        | BYTES  5-10:  STAN of Original message          |
        | BYTES 11-20:  Trans date & time of original msg |
        | BYTES 21-31:  Acquiring Institution Code        |
        | BYTES 32-42:  Forwarding Institution Code       |
        +-------------------------------------------------+
      */
      memcpy( p_auth_tx->TLF01_details.orig_message,
              HOST_MESSAGE.orig_data, 4 );

      memcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
             &HOST_MESSAGE.orig_data[4], 6 );

      memcpy( p_auth_tx->local_date_time, &HOST_MESSAGE.orig_data[10], 10 );
   }

   /* FIELD 95:  REPLACEMENT AMOUNTS */
   if ( HOST_MESSAGE.replace_amts[0] != 0x00 )
   {
      memcpy(p_auth_tx->TLF01_details.actual_amount,HOST_MESSAGE.replace_amts,12);
      actual_amount = atol( p_auth_tx->TLF01_details.actual_amount );
      total_amount  = atol( p_auth_tx->TLF01_details.total_amount  );
      rev_amount    = total_amount - actual_amount;
      sprintf( p_auth_tx->TLF01_details.reversal_amount, "%012ld", rev_amount );
   }

   /* FIELD 100: RECEIVING INSTITUTION IDENTIFICATION CODE */
   memset( &recv_inst_len, 0x00, sizeof(recv_inst_len) );
   memset( &recv_inst,     0x00, sizeof(recv_inst    ) );

   memcpy( recv_inst_len, HOST_MESSAGE.recv_len, bf100_len );
   memcpy( recv_inst, HOST_MESSAGE.recvid, sizeof(HOST_MESSAGE.recvid) );


   /* FIELD 121: AUTHORIZING AGENT IDENTIFICATION CODE */
   memcpy( p_auth_tx->TLF01_details.agent_id_len,
           HOST_MESSAGE.mc_agent_len, sizeof( HOST_MESSAGE.mc_agent_len ) );
   memcpy( p_auth_tx->TLF01_details.agent_id,
           HOST_MESSAGE.mc_agent_id, sizeof( HOST_MESSAGE.mc_agent_id ) );

}  /* End move_iso_response_to_auth_struct */


/******************************************************************************
 *
 *  NAME:         BUILD_HOST_STRUCTURE
 *
 *  DESCRIPTION:  This procedure uses the data in the auth_tx entry to build
 *                the HOST MESSAGE structure that will be required to build
 *                the Cirrus message for the host.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void build_host_structure( pAUTH_TX p_auth_tx )
{
   INT    itemp;
   CHAR   gmt_time[20];
   CHAR   cIndustry[4];
   INT    iIndustry;
   INT    actual_amount;
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_str [45] = "";
   CHAR   orig_data[42];
   CHAR   amount[13]    = "";
   INT    iCalc_timedate = 0;
   INT    pin_exists = true;
   INT    tran_flow;

   /* Initialize the host structure before filling it. */
   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   if ( p_auth_tx->TLF01_details.tx_key > AUTH_INVALID )
      tran_flow = SWITCHED_IN;
   else
      tran_flow = SWITCHED_OUT;

   /*+--------------------------------------------------------------+
     | FIELD 2: PAN  -  Right justify the length portion of the PAN |
     | ------------------------------------------------------------ |
     | If this is a log on/off message by Group Id, then move the   |
     | ICA into field 2.  Check Log on NMI global variable to       |
     | determine if this is the case.                               |
     +--------------------------------------------------------------+*/
   itemp = strlen( p_auth_tx->TLF01_details.card_num );
   if ( itemp > 0 )
   {
      if ( 0 == strcmp(Logon_NMI_Code, ECHO_NMI) )
      {
         /* Do not send out field 2 for echoes. */
         itemp = 0;
      }
      else
         memcpy( temp_str, p_auth_tx->TLF01_details.card_num, itemp );
   }
   else
   {
      /* No PAN */
      if ( 0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_ID     )  ||
           0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF )   )
      {
         /* It is a Log ON/OFF by Group Id */
         itemp = strlen( ncf01.forwarding_inst_id_code );
         if ( itemp > 0 )
         {
            if ( itemp > sizeof(HOST_MESSAGE.fwd_inst_id) )
               itemp = sizeof(HOST_MESSAGE.fwd_inst_id);

            memcpy( temp_str, ncf01.forwarding_inst_id_code, itemp );
         }
         else
         {
            /* No Forwarding Inst. Id code.  Use the Acquiring Inst. Id code. */
            itemp = strlen( ncf01.acquirer_inst_id_code );
            if ( itemp > 0 )
            {
               if ( itemp > sizeof(HOST_MESSAGE.acq_inst_id) )
                  itemp = sizeof(HOST_MESSAGE.acq_inst_id);

               memcpy( temp_str, ncf01.acquirer_inst_id_code, itemp );
            }
         }
      }
   }

   /* Right justify length. */
   if ( itemp > 0 )
   {
      sprintf( HOST_MESSAGE.pan_len, "%02d", itemp );
      memcpy( HOST_MESSAGE.pan, temp_str, itemp );
   }


   /* FIELD 3: PROCESSING CODE */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.processing_code ) )
   {
      strcpy( p_auth_tx->TLF01_details.processing_code, "000000" );
      memcpy( HOST_MESSAGE.proccode, "000000", 6 );
   }
   else
   {
      memcpy( HOST_MESSAGE.proccode,p_auth_tx->TLF01_details.processing_code,6);

      if ( tran_flow == SWITCHED_OUT )
      {
         /* Hypercom proc code for Bal Inquiry is 31nnnn, for Cirrus 30nnnn. */
         if ( AUTH_BALANCE_INQUIRY == p_auth_tx->TLF01_details.tx_key )
            memcpy( HOST_MESSAGE.proccode, "30", 2 );
         else 
            memcpy(HOST_MESSAGE.proccode,"01",2);
      }
   }

   /* FIELD 4: TRANSACTION AMOUNT */
   if ( tran_flow == SWITCHED_OUT )
   {
      if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
         strcpy( amount, p_auth_tx->TLF01_details.orig_amount );
      else
         strcpy( amount, p_auth_tx->TLF01_details.total_amount );
   }
   else
   {
      strcpy( amount, p_auth_tx->TLF01_details.total_amount );
   }
   memcpy( HOST_MESSAGE.amount, amount, bf4_len );

   /* FIELD 5: SETTLEMENT AMOUNT */
   if ( p_auth_tx->TLF01_details.settlement_total[0] != 0x00 )
      memcpy( HOST_MESSAGE.settle_amt,
              p_auth_tx->TLF01_details.settlement_total, bf5_len );

   /* FIELD 7: TRANSACTION DATE & TIME */
   /* Current Greenwich Mean Time for the transmission-to-host time. */
   if ( tran_flow == SWITCHED_IN )
   {
      memcpy( HOST_MESSAGE.gmt_time,
              p_auth_tx->TLF01_details.transmission_timestamp,
              bf7_len );
   }
   else if ( 0 == strncmp( "04", p_auth_tx->TLF01_details.message_type, 2 ) )
   {
      pintime_get_gmt( gmt_time );                         /* YYYYMMDDhhmmss */
      memcpy( HOST_MESSAGE.gmt_time, &gmt_time[4], bf7_len ); /* MMDDhhmmss  */
   }
   else
   {
      if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.transmission_timestamp ))
      {
         pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
         strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
                  &gmt_time[4], bf7_len ); /* MMDDhhmmss */
      }
      memcpy( HOST_MESSAGE.gmt_time,
              p_auth_tx->TLF01_details.transmission_timestamp,
              bf7_len );
   }

   /* FIELD 9: SETTLEMENT CONVERSION RATE */
   if ( p_auth_tx->settlement_conv_rate[0] != 0x00 )
      memcpy( HOST_MESSAGE.conv_rate_stl,
              p_auth_tx->settlement_conv_rate, bf9_len );

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   if ( AUTH_LOGON          == p_auth_tx->TLF01_details.tx_key ||
        AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key )
      memcpy( HOST_MESSAGE.stan, Stan, bf11_len );
   else if(0 == strcmp(NULL_STR,p_auth_tx->TLF01_details.sys_trace_audit_num))
      memcpy( HOST_MESSAGE.stan, &gmt_time[8], bf11_len );
   else
      memcpy( HOST_MESSAGE.stan,
              p_auth_tx->TLF01_details.sys_trace_audit_num,
              bf11_len );

   /* FIELD 12: LOCAL TRANSACTION TIME */
   if ( 0 != strncmp( "04", p_auth_tx->TLF01_details.message_type, 2 ) )
   {
	  if(strcmp(p_auth_tx->TLF01_details.time_hhmmss,NULL_STR) == 0)
	  {
		  ptetime_get_timestamp( time_date );
		  iCalc_timedate = 1;
		  get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss );
	  }
   }
   memcpy( HOST_MESSAGE.time, p_auth_tx->TLF01_details.time_hhmmss, bf12_len );

   /* FIELD 13: LOCAL TRANSACTION DATE */
   if ( 0 != strncmp( "04", p_auth_tx->TLF01_details.message_type, 2 ) )
   {
      if(strcmp(p_auth_tx->TLF01_details.date_yyyymmdd,NULL_STR) == 0)
      {
         if(iCalc_timedate == 0)
            ptetime_get_timestamp( time_date );
         get_date( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
      }
   }
   memcpy( HOST_MESSAGE.date,
          &p_auth_tx->TLF01_details.date_yyyymmdd[4], bf13_len );

   /* FIELD 14: EXPIRATION DATE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.exp_date ) )
      memcpy( HOST_MESSAGE.date_expd,
              p_auth_tx->TLF01_details.exp_date,
              bf14_len );

   /* FIELD 15: SETTLEMENT DATE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.settlement_date ) )
      memcpy( HOST_MESSAGE.date_settle,
              p_auth_tx->TLF01_details.settlement_date,
              bf15_len );

   /* FIELD 16: CONVERSION DATE (MMDD) */
   if ( p_auth_tx->conv_date[0] != 0x00 )
      memcpy( HOST_MESSAGE.conv_date, p_auth_tx->conv_date, bf16_len );

   /* FIELD 18: MERCHANT TYPE / ACQUIRER SIC NUMBER */
   memcpy( HOST_MESSAGE.sic_number,
           p_auth_tx->TLF01_details.category_code,
           bf18_len );

   /* FIELD 22: POINT OF SERVICE ENTRY MODE */
   pin_exists = cirsutil_verify_pin_block(p_auth_tx);
   if ( p_auth_tx->TLF01_details.pos_entry_mode[0] != 0x00 )
   {                                                                     
      /* If Mag Stripe Swipe, Cirrus needs to see 90 in 1st two positions */
      if ( NULL == strchr(p_auth_tx->TLF01_details.pos_entry_mode, '2') )
      {
         memcpy( HOST_MESSAGE.posentry,
                &p_auth_tx->TLF01_details.pos_entry_mode[1],
                 bf22_len );
      }
      else
      {
         memcpy( HOST_MESSAGE.posentry, "900", bf22_len );
      }

      /* Last position must be '1' for Cirrus if pin is present. */
      if ( pin_exists )
         HOST_MESSAGE.posentry[2] = '1';
   }

   /* FIELD 28: TRANSACTION FEE AMOUNT */
   if ( p_auth_tx->TLF01_details.tran_fee_amount[0] != 0x00 )
   {
      HOST_MESSAGE.tran_fee[0] = p_auth_tx->TLF01_details.cvc_data[0];
      memcpy( &HOST_MESSAGE.tran_fee[1],
              p_auth_tx->TLF01_details.tran_fee_amount,
              bf28_len-1 );
   }

   /* FIELD 29: SETTLEMENT FEE AMOUNT */
   if ( p_auth_tx->ch_billing_amt[0] != 0x00 )
      memcpy( HOST_MESSAGE.setl_fee, p_auth_tx->ch_billing_amt, bf29_len );

   /* FIELD 32: ACQUIRING INSTITUTION IDENTIFICATION CODE */
   if ( tran_flow == SWITCHED_IN )
   {
      itemp = strlen( p_auth_tx->TLF01_details.acquiring_id );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.acq_inst_len, "%02d", itemp );
         memcpy( HOST_MESSAGE.acq_inst_id,
                 p_auth_tx->TLF01_details.acquiring_id,
                 bf32data_len );
      }
   }
   else
   {
      itemp = strlen( ncf01.acquirer_inst_id_code );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.acq_inst_len, "%02d", itemp );
         memcpy( HOST_MESSAGE.acq_inst_id, ncf01.acquirer_inst_id_code,
                 sizeof(HOST_MESSAGE.acq_inst_id) );

         /* Store acquiring id for TLF01. */
         if ( p_auth_tx->TLF01_details.acquiring_id[0] == 0x00 )
            strcpy(p_auth_tx->TLF01_details.acquiring_id,HOST_MESSAGE.acq_inst_id);
      }
   }

   /* FIELD 33: FORWARDING INSTITUTION IDENTIFICATION CODE */
   if ( tran_flow == SWITCHED_IN )
   {
      /* Use value from a request from the host. */
      itemp = strlen( p_auth_tx->TLF01_details.forwarding_institution_id );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.fwd_inst_len, "%02d", itemp );
         strcpy(  HOST_MESSAGE.fwd_inst_id,
                  p_auth_tx->TLF01_details.forwarding_institution_id );
      }
   }
   else if ( 0 != strcmp( NULL_STR, ncf01.forwarding_inst_id_code ) )
   {
      itemp = strlen( ncf01.forwarding_inst_id_code );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.fwd_inst_len, "%02d", itemp );
         memcpy( HOST_MESSAGE.fwd_inst_id, ncf01.forwarding_inst_id_code,
                 sizeof(HOST_MESSAGE.fwd_inst_id));

         /* Log to TLF01. */
         if ( p_auth_tx->TLF01_details.forwarding_institution_id[0] == 0x00 )
            strcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
                    HOST_MESSAGE.fwd_inst_id );
      }
   }
   else
   {
      /* No Forwarding Inst. Id code.  Use the Acquiring Inst. Id code. */
      memcpy( HOST_MESSAGE.fwd_inst_len, HOST_MESSAGE.acq_inst_len, bf33_len );
      strcpy( HOST_MESSAGE.fwd_inst_id,  HOST_MESSAGE.acq_inst_id            );
   }
   Rj_with_lead_zeros( HOST_MESSAGE.fwd_inst_len, bf33_len );

   /* FIELD 35: TRACK 2 */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.track2 ) )
   {
      trim_spaces( p_auth_tx->TLF01_details.track2 );
      itemp = strlen( p_auth_tx->TLF01_details.track2 );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.track2_len, "%02d", itemp );
         strcpy( HOST_MESSAGE.track2, p_auth_tx->TLF01_details.track2 );
      }
   }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   if ( 0 == strncmp( "04", p_auth_tx->TLF01_details.message_type, 2 ) )
   {
      if ( p_auth_tx->TLF01_details.orig_retrieval_ref_num[0] != 0x00 )
         memcpy( HOST_MESSAGE.rrn,
                 p_auth_tx->TLF01_details.orig_retrieval_ref_num,
                 bf37_len );
      else
         memcpy( HOST_MESSAGE.rrn,
                 p_auth_tx->TLF01_details.retrieval_ref_num,
                 bf37_len );
   }
   else
   {
      if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num ) )
         memcpy( HOST_MESSAGE.rrn,
                 p_auth_tx->TLF01_details.retrieval_ref_num,
                 bf37_len );
      else
         memcpy( HOST_MESSAGE.rrn,
                 p_auth_tx->TLF01_details.orig_retrieval_ref_num,
                 bf37_len );
   }

   /* FIELD 38:AUTHORIZATION ID RESPONSE */
   memcpy(HOST_MESSAGE.authcode,p_auth_tx->TLF01_details.auth_number,bf38_len);

   /* FIELD 39: RESPONSE CODE */
   memcpy( HOST_MESSAGE.responsecd, 
           p_auth_tx->TLF01_details.response_code,
           bf39_len );

   /* FIELD 41: CARD ACCEPTOR TERMINAL IDENTIFICATION */
   memset( HOST_MESSAGE.termid, 0x20, bf41_len );
   memcpy( HOST_MESSAGE.termid,
           p_auth_tx->TLF01_details.terminal_id,
           strlen(p_auth_tx->TLF01_details.terminal_id) );

   /* FIELD 42: CARD ACCEPTOR IDENTIFICATION CODE */
   memset( HOST_MESSAGE.cardaccid, 0x20, bf42_len );
   memcpy( HOST_MESSAGE.cardaccid,
           p_auth_tx->TLF01_details.merchant_id,
           strlen(p_auth_tx->TLF01_details.merchant_id) );

   /* FIELD 43:	CARD ACCEPTOR NAME / LOCATION */
   memset( HOST_MESSAGE.cardaccloc, 0x20, bf43_len );
   itemp = strlen( p_auth_tx->acceptor_term_name );
   memcpy( HOST_MESSAGE.cardaccloc, p_auth_tx->acceptor_term_name, 22 );
   if ( 22 > itemp )
   {
      /* Make sure spaces follow the merchant name. */
      memset( &HOST_MESSAGE.cardaccloc[itemp], 0x20, 22-itemp );
   }
   itemp = strlen( p_auth_tx->MCF01_details.city );
   memcpy( &HOST_MESSAGE.cardaccloc[23], p_auth_tx->MCF01_details.city, 13 );
   if ( 13 > itemp )
   {
      /* Make sure spaces follow the merchant's city. */
      memset( &HOST_MESSAGE.cardaccloc[23+itemp], 0x20, 13-itemp );
   }
   /* Country code - 3 alpha characters */
   memcpy( &HOST_MESSAGE.cardaccloc[37], "PHL", 3 );

   /* Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   memcpy( p_auth_tx->TLF01_details.card_holder_name,
           HOST_MESSAGE.cardaccloc,
           22 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
           HOST_MESSAGE.cardaccloc+23,
           13 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk+13, "PHL", 3 );

   /* FIELD 44: Additional Response Data */
   itemp = strlen( p_auth_tx->TLF01_details.resp_source );
   if ( itemp > 0 )
   {
      sprintf( HOST_MESSAGE.addl_rsp_len, "%02d", itemp );
      memcpy( HOST_MESSAGE.addlrspdata, p_auth_tx->TLF01_details.resp_source,
              itemp );
   }


   /* FIELD 45: TRACK 1 */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.track1 ) )
   {
      itemp = strlen( p_auth_tx->TLF01_details.track1 );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.track1_len, "%02d", itemp );
         strcpy( HOST_MESSAGE.track1, p_auth_tx->TLF01_details.track1 );
      }
   }

   /* FIELD 48: ADDITIONAL DATA */
   if ( 0 == strncmp( "08", p_auth_tx->TLF01_details.message_type, 2 ) )
   {
      /* Send pin key for pin key change messages. */
      memcpy( HOST_MESSAGE.addl_data_len, p_auth_tx->int_net_facility_len, 3 );
      strcpy( HOST_MESSAGE.addl_data, p_auth_tx->int_net_facility_data );
      Rj_with_lead_zeros( HOST_MESSAGE.addl_data_len, bf48_len );
   }
   else
   {
      iIndustry = atoi( cIndustry );
      switch( iIndustry )
      {
         case 100:
         case 600:  memcpy( HOST_MESSAGE.addl_data_len, "001R", 4 ); break;
         case 101:  memcpy( HOST_MESSAGE.addl_data_len, "001F", 4 ); break;
         case 110:  memcpy( HOST_MESSAGE.addl_data_len, "001O", 4 ); break;
         case 200:  memcpy( HOST_MESSAGE.addl_data_len, "001X", 4 ); break;
         case 300:  memcpy( HOST_MESSAGE.addl_data_len, "001H", 4 ); break;
         case 400:  memcpy( HOST_MESSAGE.addl_data_len, "001A", 4 ); break;
         case 500:  memcpy( HOST_MESSAGE.addl_data_len, "001T", 4 ); break;
         case 900:  memcpy( HOST_MESSAGE.addl_data_len, "001C", 4 ); break;
         case 902:  memcpy( HOST_MESSAGE.addl_data_len, "001C", 4 ); break;
         default:   memcpy( HOST_MESSAGE.addl_data_len, "001U", 4 ); break;
      }
   }

   /* FIELD 49: TRANSACTION CURRENCY CODE */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.currency_code ) )
   {
      memcpy( HOST_MESSAGE.tran_curr_cd, 
              p_auth_tx->MCF01_details.currency_code,
              bf49_len );
   }
   else
   {
      memcpy( HOST_MESSAGE.tran_curr_cd, 
              p_auth_tx->TLF01_details.currency_code,
              bf49_len );
   }
   Rj_with_lead_zeros( HOST_MESSAGE.tran_curr_cd, bf49_len );

   /* FIELD 50: SETTLEMENT CURRENCY CODE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->settlement_curr_code ) )
   {
      memcpy(HOST_MESSAGE.settle_cur_cd,p_auth_tx->settlement_curr_code,bf50_len);
      Rj_with_lead_zeros( HOST_MESSAGE.settle_cur_cd, bf50_len );
   }

   /* FIELD 52: PIN BLOCK */
   if ( pin_exists )
   {
      memset(temp_str, 0, sizeof(temp_str));
      ascii_to_bin( p_auth_tx->TLF01_details.pin_block,
                    strlen(p_auth_tx->TLF01_details.pin_block),
                    &temp_str[0], sizeof(temp_str));
      memcpy (HOST_MESSAGE.pinblock, temp_str, sizeof(HOST_MESSAGE.pinblock));
   }

   /* FIELD 60: ADVICE REASON CODE */
   if ( 0 == strcmp( NULL_STR, reject_advice_code ) )
   {
      memcpy( HOST_MESSAGE.advice_cd_len, "003", bf60_len );
      strcpy( HOST_MESSAGE.advice_code,   "401"           );
   }
   else
   {
      memcpy( HOST_MESSAGE.advice_cd_len, "007", bf60_len  );
      strcpy( HOST_MESSAGE.advice_code, reject_advice_code );
      strcat( HOST_MESSAGE.advice_code, reject_detail_code );
   }

   /* FIELD 61: POINT OF SERVICE DATA */
   memcpy( HOST_MESSAGE.pos_data_len, "026", bf61_len );
   memset( HOST_MESSAGE.pos_data, 0x30, 26 );
   memcpy( HOST_MESSAGE.pos_data, CIRRUS_POS_DATA, atoi(CIRRUS_POS_DATA_LEN) );
   itemp = strlen( p_auth_tx->MCF01_details.zip_code );
   memcpy( &HOST_MESSAGE.pos_data[16], p_auth_tx->MCF01_details.zip_code,itemp);

   /* FIELD 62:  INTERMEDIATE NETWORK FACILITY (INF) DATA */
   itemp = strlen( p_auth_tx->int_net_facility_data );
   if ( itemp > 0 )
   {
      sprintf( HOST_MESSAGE.inf_data_len, "%03d", itemp );
      memcpy( HOST_MESSAGE.inf_data, p_auth_tx->int_net_facility_data, itemp );
   }

   /* FIELD 63: BANKNET */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.mcard_banknet ) );
   {
      itemp = strlen( p_auth_tx->TLF01_details.mcard_banknet );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.banknet_len, "%03d", itemp );
         memcpy( HOST_MESSAGE.banknet,
                 p_auth_tx->TLF01_details.mcard_banknet,
                 itemp );
      }
   }

   /* FIELD 66: SETTLEMENT CODE */
   memcpy( HOST_MESSAGE.setl_code, SETTLEMENT_CODE, strlen(SETTLEMENT_CODE) );

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   memcpy( HOST_MESSAGE.nmi_code, Logon_NMI_Code, bf70_len );
   strcpy( Logon_NMI_Code, NULL_STR );  // Reset for next transaction

   /* FIELD 90: ORIGINAL DATA ELEMENTS - Used for Reversals. */
   /* 
     +-------------------------------------------------+
     | BYTES  1- 4:  Original Message Type             |
     | BYTES  5-10:  STAN of Original message          |
     | BYTES 11-20:  Trans date & time of original msg |
     | BYTES 21-31:  Acquiring Institution Code        |
     | BYTES 32-42:  Forwarding Institution Code       |
     +-------------------------------------------------+
   */
   memset( orig_data, '0', sizeof(orig_data));
   memcpy( orig_data, p_auth_tx->TLF01_details.orig_message, 4 );
   memcpy( &orig_data[4], p_auth_tx->TLF01_details.orig_sys_trace_num, 6 );

   /* Time of original tx (MMDDhhmmss). */
   memcpy( &orig_data[10], p_auth_tx->orig_local_time_date, 10);

   /* Acquiring Institution ID Code */
   memset( temp_str, 0x00, sizeof( temp_str ) );
   memcpy( temp_str, ncf01.acquirer_inst_id_code, bf32data_len );
   Rj_with_lead_zeros( temp_str, bf32data_len );
   memcpy( &orig_data[20], temp_str, bf32data_len );

   /* Forwarding Institution ID Code */
   memset( temp_str, 0x00, sizeof( temp_str ) );
   memcpy( temp_str, ncf01.forwarding_inst_id_code, bf32data_len );
   Rj_with_lead_zeros( temp_str, bf32data_len );
   memcpy( &orig_data[31], temp_str, bf32data_len );

   memcpy( HOST_MESSAGE.orig_data, orig_data, bf90_len );

   /* FIELD 94: SERVICE INDICATOR */
   memcpy( HOST_MESSAGE.service_ind, "CA0    ", bf94_len );

   /* FIELD 95:  REPLACEMENT AMOUNTS */
   memset( HOST_MESSAGE.replace_amts, '0', sizeof(HOST_MESSAGE.replace_amts) );
   if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
   {
      /* This field should hold the amount dispensed, for reversals.
       * This is different than Visa. In Visa, DE95 is the amount not
       * dispensed. In Visa it is the amount to be reversed out of the
       * cardholder's account. In Cirrus, it is the amount that gets
       * authorized on the cardholder's account.
       */
      actual_amount = atol(p_auth_tx->TLF01_details.actual_amount);

      sprintf(p_auth_tx->TLF01_details.actual_amount, "%012d", actual_amount );

      memcpy( HOST_MESSAGE.replace_amts,
              p_auth_tx->TLF01_details.actual_amount,
              12 );
   }

   /* FIELD 96: */
   if ( 0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_ID     ) ||
        0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF ) )
   {
      /* Send field 96 in request to host. */
      DE96_State = YES;
   }
   else
      DE96_State = NO;

   /* FIELD 97: NET SETTLEMENT AMOUNT */
   memcpy( HOST_MESSAGE.net_setl_code, ncf21.total_amt_type, 1 );
   memcpy( HOST_MESSAGE.net_setl, ncf21.total_amt, bf97_len );

   /* FIELD 99: SETTLEMENT INSTITUTION ID CODE */
   itemp = strlen( Settlement_ID );
   if ( itemp > 0 )
   {
      sprintf( HOST_MESSAGE.setl_id_len, "%02d", itemp );
      memcpy( HOST_MESSAGE.setl_id, Settlement_ID, itemp );
      memset( Settlement_ID, 0x00, sizeof(Settlement_ID) );
   }

   /* FIELD 100: RECEIVING INSTITUTION ID CODE */
   memcpy( HOST_MESSAGE.recv_len, recv_inst_len, bf100_len );
   Rj_with_lead_zeros( HOST_MESSAGE.recv_len, bf100_len );
   memcpy( HOST_MESSAGE.recvid,   recv_inst, sizeof(HOST_MESSAGE.recvid) );

   /* FIELD 121: AUTHORIZING AGENT IDENTIFICATION CODE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.agent_id ) )
   {
      itemp = strlen( p_auth_tx->TLF01_details.agent_id );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.mc_agent_len, "%03d", itemp );
         strcpy( HOST_MESSAGE.mc_agent_id, p_auth_tx->TLF01_details.agent_id );
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         COPY_TO_REQ_RESP_BUFFER
 *
 *  DESCRIPTION:  This function clears ReqRsp_Buffer, then copies an input
 *                string into it, for a given length.
 *
 *  INPUTS:       indata - A string to be copied into ReqRsp_Buffer
 *                msglen - Length of data in 'indata' to be copied
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void copy_to_req_resp_buffer( pCHAR indata, INT msglen )
{
   memset( ReqRsp_Buffer, 0, COMM_BUF_SZE );   /* zero out the buffer */
   buflen = msglen;
   reqidx = 0; 
   memcpy( ReqRsp_Buffer, indata, msglen );
   invalid_msg_format = false;
} /* end copy_to_req_resp_buffer */


/******************************************************************************
 *
 *  NAME:         CIRSUTIL_ERROR_HANDLER
 *
 *  DESCRIPTION:  This procedure is used to log errors, warnings, or
 *                informational messages to the error log and opermon.
 *                If debug is on, they will be displayed on the console.
 *
 *  INPUTS:       MonitorFlag - 0 = Do NOT write to Systems Monitor
 *                              1 = DO write to Systems Monitor
 *
 *                Message     - String of text to be displayed
 *
 *                EventLogFlag- 0 = Do NOT write to event log
 *                              1 = DO write to event log
 *
 *                LogMsgType  - INFO_MSG, WARN_MSG, ERROR_MSG, ALERT_MSG
 *
 *                FuncName    - Name of function calling this routine
 *
 *                Severity    - 0, 1, 2, 3, 4 (0 being the lowest severity)
 *
 *                MsgType     - INFO_ERROR, WARNING_ERROR, FATAL_ERROR
 *
 *                Term_Id     - Terminal ID
 *
 *                CardNum     - Card number
 *
 *                Merch_Id    - Merchant ID
 *
 *                Msg_Id      - Unique identifier to indicate where msg originated
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void cirsutil_error_handler( INT MonitorFlag, pBYTE Message,  INT  EventLogFlag,
                             INT LogMsgType,  pBYTE FuncName, BYTE Severity,
                             pBYTE MsgType,   pBYTE Term_Id,  pBYTE CardNum,
                             pBYTE Merch_Id,  pBYTE Msg_Id )
{
   CHAR      Buffer[100];

   /* Attach a unique identifier to the message.
    * Removed the message Id so the messages will
    * be similar to other network controllers.
    */
   memset( Buffer, 0, sizeof(Buffer) );
   memcpy( Buffer, Message, 99 );

   /* Call function to post the message. */
   TxUtils_Send_Msg_To_Operator(
         MonitorFlag, Buffer,  EventLogFlag, LogMsgType, FuncName,
         Severity,    MsgType, Term_Id,      CardNum,    Merch_Id );

} /* cirsutil_error_handler */


/*****************************************************************************
*
* FUNCTION:     cirsutil_get_strtime
*
* DESCRIPTION:  Function sets transaction start time
*               
*
* PARAMETERS:   CHAR*
*
* RETURN:       NONE
*
******************************************************************************/
void cirsutil_get_strtime ( pCHAR deststr )
{
   time_t seconds;

   seconds = time(NULL_PTR);      /* get the current date and time in seconds */
   sprintf( deststr, "%ld", seconds );
}

/******************************************************************************
*
* FUNCTION:     cirsutil_check_BIN_table
*
* DESCRIPTION:  check if card number is in bin table
*               
*
* PARAMETERS:   pCHAR application que_name
*
* RETURN:       void
*
*******************************************************************************/
/*void cirsutil_check_BIN_table(pCHAR dest_queue)
{
   CHAR err_mesg[200] = "";

   memset(&binn_range, 0, sizeof(BINN_RANGE));
   memset(dest_queue, 0, 9);
   if(txdsapi_check_bin_range_table(&binn_range, auth_tx.TLF01_details.card_num, err_mesg) != 0)
   {
      PRINT("thmain_check_BIN_table: Card number does not fall into any of the defined Binn Ranges\n");
      PRINT(err_mesg);
      PRINT("\n");
	   memcpy(dest_queue, "NONE", 5);
   }
   else
   {
      memcpy(dest_queue, binn_range.hostapp_name, sizeof(binn_range.hostapp_name));
	   strcat(dest_queue,"A");
      memcpy(auth_tx.TLF01_details.settle_file_prefix, binn_range.settle_file_prefix, sizeof(auth_tx.TLF01_details.settle_file_prefix));
      memcpy(auth_tx.TLF01_details.binn_range_type, binn_range.type, sizeof(auth_tx.TLF01_details.binn_range_type));
   }
}*/

/******************************************************************************
 *
 *  NAME:         BLANK_STRING
 *
 *  DESCRIPTION:  This function determines if a string consists of all blanks.
 *
 *  INPUTS:       max_len - Number of bytes to check as blank or not
 *                astr    - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if 'astr' is all blank, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN blank_string(pCHAR astr, INT max_len)
{
   int i;

   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return( false );
   }
   return( true );
}

/******************************************************************************
 *
 *  NAME:         CIRSUTIL_VERIFY_PIN_BLOCK
 *
 *  DESCRIPTION:  This function verifies a PIN block by checking to if there
 *                is data in the field.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if PIN block exists,
 *                else false if field is null or all blanks
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BOOLEAN cirsutil_verify_pin_block( pAUTH_TX p_auth_tx )
{
   BOOLEAN  ret_val = true;

   if ( 0 == strlen(p_auth_tx->TLF01_details.pin_block) )
      ret_val = false;

   else if ( blank_string( p_auth_tx->TLF01_details.pin_block,
                     strlen(p_auth_tx->TLF01_details.pin_block)) )
      ret_val = false;
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         cirsutil_transfer_ctl_to_auth_app
 *
 *  DESCRIPTION:  This function builds the transaction Id into the XIPC buffer
 *                and sets the terminal status timer, and transfers control to
 *                one of the host authorizer modules.
 *
 *  INPUTS:       dest_queue - Destination of message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if all goes well, else false
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN cirsutil_transfer_ctl_to_auth_app( pAUTH_TX p_auth_tx )
{
   BOOLEAN  ret_code = false;

   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_REQUEST );
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         cirsutil_process_host_request
 *
 *  DESCRIPTION:  This function processes a request from a different host
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if all goes well, else false
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE cirsutil_process_host_request( pAUTH_TX p_auth_tx )
{
   CHAR   unique_tran_str_id[11] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   ErrorMsg[150] = "";

   /* Initialize the transaction. */
   (void)cirsutil_init_txn( p_auth_tx );

   /* Check to see if the PIN block has data in it. */
   if ( true == cirsutil_verify_pin_block(p_auth_tx) )
   {
      /* Store current working key and pin type in transaction. */
      strcpy( p_auth_tx->TLF01_details.source_key,        ncf01.cwk      );
      strcpy( p_auth_tx->TLF01_details.source_pin_format, ncf01.pin_type );
   }

   strcpy( p_auth_tx->TLF01_details.authorizing_host_queue, application_que_name );
   if ( false == cirsutil_transfer_ctl_to_auth_app(p_auth_tx) )
   {
      cirsutil_error_handler(1,
                             "Failed to process a request from Cirrus.",
                             0, ERROR_MSG,
                             "cirsutil_process_host_request",
                             3, WARNING_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id, "451" );
      return( false );
   }
   return( true );
}

/******************************************************************************
 *
 *  NAME:         cirsutil_init_txn
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *                
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Unknown
 *
 ******************************************************************************/
void cirsutil_init_txn( pAUTH_TX p_auth_tx )
{
   CHAR   unique_tran_str_id[20] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";

   /* Create a unique transaction id for the new auth_tx record. */
   ptetime_get_strtime( unique_tran_str_id );
   memset( p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
           sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
   memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
           p_auth_tx->TLF01_details.terminal_id, 
           sizeof( p_auth_tx->TLF01_details.terminal_id) - 1 );
   strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
            unique_tran_str_id+2, 12 );

   /* Initialize some of the auth_tx fields. */
   p_auth_tx->TLF01_details.general_status      =  GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type          =  ADMINISTRATIVE;
   strcpy( p_auth_tx->TLF01_details.dispensation.origin, "RO" ); /* Remote origin */

   strcpy( p_auth_tx->TLF01_details.tran_start_time,     unique_tran_str_id  );
   strcpy( p_auth_tx->TLF01_details.handler_queue,       application_que_name);

   /* Set issuer Id for reporting purposes. */
   strncpy( p_auth_tx->TLF01_details.issuer_id, AppName,
            sizeof(p_auth_tx->TLF01_details.issuer_id) );

   /*  Create RRN */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num ) )
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }

   /* Get local time. */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ) )
   {
      ptetime_get_timestamp( time_date );
      get_date( time_date, date_str );
      get_time( time_date, time_str );
      strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str );
      strcpy( p_auth_tx->TLF01_details.time_hhmmss,   time_str );
   }
}

/******************************************************************************
 *
 *  NAME:         incoming_other_host_response
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_auth_tx.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Unknown
 *
 ******************************************************************************/
INT incoming_other_host_response( pAUTH_TX p_auth_tx )
{
   BOOLEAN    ret_code = false;

   /* Build response message for the host. */
   build_host_structure( p_auth_tx );
   if ( false == build_host_request( p_auth_tx ) )
   {
      cirsutil_error_handler(1, "Unable to build host response message",
                             1, ALERT_MSG, "incoming_other_host_response", 4,
                             FATAL_ERROR, p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "452");
      return( false );
   }

   /* Send response to the host. */
   if ( false == cirsutil_send_host_response(p_auth_tx) )
      return( false );

   /* Transaction is completed.  Insert it into the database. */
   ret_code = insert_into_tlf01( p_auth_tx );

   return( (INT)ret_code );
}

/*****************************************************************************

  Function:   cirsutil_parse_db_ok

  Description: This function determines which type of database request the
               incoming message is from and routes it to the correct function.
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Author: 
      BGable   7/13/98
******************************************************************************/
void  cirsutil_parse_db_ok( pPTE_MSG p_msg_in )
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE          subtype1;
   BYTE          subtype2;
   BYTE          app_data_type;
   AUTH_TX       auth_tx;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   pCurrent   = ptemsg_get_pte_msg_data_data( p_msg_data );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
   subtype2 = ptemsg_get_msg_subtype2( p_msg_in );

   if ( ST1_DB_SELECT == subtype1 )
   {
      if ( NCF21_DATA == app_data_type )
      {
         if ( false == recon_close_batch(pCurrent) )
         {
            cirsutil_error_handler(1,
                          "Error closing Cirrus batch message",
                          0, ERROR_MSG, "cirsutil_parse_db_ok",
                          3, WARNING_ERROR,
                          NULL_STR, NULL_STR, NULL_STR, "541" );
         }
      }
      else if ( SAF01_DATA == app_data_type )
      {
         /* There is a reversal record to be forwarded. */
         (void)saf_process_db_response( pCurrent, PTEMSG_OK );
      }
      else
      {
         cirsutil_error_handler(1,
                                "Error processing incoming terminal message",
                                0, ERROR_MSG, "cirsutil_parse_db_ok",
                                3, WARNING_ERROR,
                                NULL_STR, NULL_STR, NULL_STR, "428" );
      }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( NCF21_DATA == app_data_type )
      {
         if ( subtype2 == ST2_ISSUER )
         {
            memset( &auth_tx, 0x00,     Auth_Tx_Size );
            memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
            if ( false == send_response_to_term_app( &auth_tx ) )
            {
               cirsutil_error_handler(1,
                                     "Unable to send message(s) to txcntl",
                                      0, ERROR_MSG, "cirsutil_parse_db_ok", 3,
                                      WARNING_ERROR,
                                      auth_tx.TLF01_details.terminal_id,
                                      auth_tx.TLF01_details.card_num,
                                      auth_tx.TLF01_details.merchant_id,
                                     "409");
            }
         }
         else
         {
            if ( false == recon_totals(pCurrent) )
            {
               cirsutil_error_handler(1,
                             "Error writing Cirrus Reonciliation totals.",
                             0, ERROR_MSG, "cirsutil_parse_db_ok",
                             3, WARNING_ERROR,
                             NULL_STR, NULL_STR, NULL_STR, "544" );
            }
         }
      }
   }
} /* cirsutil_parse_db_ok */


/*****************************************************************************

  Function:   cirsutil_parse_db_error

  Description: This function determines which type of database request the
               incoming error message is from and routes it to the correct
               function.
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Author: 
      BGable   7/13/98

******************************************************************************/
void  cirsutil_parse_db_error( pPTE_MSG p_msg_in )
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE          app_data_type;
   BYTE          subtype1;
   BYTE          subtype2;
   CHAR          error_msg[200] = "";
   AUTH_TX       auth_tx;
   CHAR          Buffer[360]  = "";
   CHAR          Buffer1[400] = "";

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent   = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset( &auth_tx, 0, Auth_Tx_Size );
   memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
   strcpy( error_msg, pCurrent+(Auth_Tx_Size) );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1      = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2      = ptemsg_get_msg_subtype2(p_msg_in);

   if ( ST1_DB_SELECT == subtype1 )
   {
      if ( NCF21_DATA == app_data_type )
      {
         /* Batch totals not found for reconciliation. */
         if ( false == recon_no_atp_totals(pCurrent) )
         {
            strcpy( Buffer, "Error writing Cirrus Reonciliation totals." );
         }

         sprintf( Buffer,
                 "Unable to get Cirrus Recon Totals: %s",
                  error_msg );
      }
      else if ( SAF01_DATA == app_data_type )
      {
         /* There are no more reversal records to be forwarded. */
         (void)saf_process_db_response( pCurrent, PTEMSG_NOT_FOUND );
      }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( (NCF21_DATA == app_data_type) && (subtype2 == ST2_ISSUER) )
      {
         /* Unable to insert into NCF30 or update NCF21. */
         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
         strcpy( auth_tx.TLF01_details.response_code,"96" );
         sprintf( Buffer, "%s", error_msg );

         if ( false == send_response_to_term_app( &auth_tx ) )
         {
            strcpy( Buffer, "Unable to send message to txcntl" );
         }
      }
      else if ( NCF01_DATA == app_data_type )
      {
         sprintf( Buffer,
                 "Error updating NCF01: %s",
                  error_msg );
      }
   }

   if ( Buffer[0] != 0x00 )
   {
      sprintf( Buffer1,
              "DB Err - DT: %d, ST1: %d, ST2: %d => ",
               app_data_type, subtype1, subtype2 );
      strcat( Buffer1, Buffer );
      Buffer1[99] = 0x00;
      cirsutil_error_handler( 1, Buffer1, 1, WARN_MSG, "cirsutil_parse_db_error",
                              3, WARNING_ERROR,
                              auth_tx.TLF01_details.terminal_id,
                              auth_tx.TLF01_details.card_num,
                              auth_tx.TLF01_details.merchant_id,
                              NULL_STR );
   }
   return;
} /*cirsutil_parse_db_error*/


/******************************************************************************
 *
 *  NAME:         Determine_Auth_Msg_Type
 *
 *  DESCRIPTION:  This function determines the type of message from the
 *                Cirrus Host where the response message type field
 *                contains "0110".  This function determines if it is really
 *                a "0210" message.  A 0110 can be either a real 0110 or a 0210.
 *
 *  INPUTS:       p_host_auth_tx - Transaction record response from the host
 *
 *  OUTPUTS:      p_orig_auth_tx - Transaction record prior to going to the host
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void Determine_Auth_Msg_Type( pAUTH_TX p_host_auth_tx, pAUTH_TX p_orig_auth_tx )
{
   INT  ProcCode;

   /* Only do this for '0110's.  Other message types are unique. */
   if ( 0 == strncmp( p_host_auth_tx->TLF01_details.message_type, "0110", 4 ) )
   {
      /* Determine message type based on processing code.  However, if it
       * is a processing code of 0, it could be a Sale or an Authorization.
       * In this case, use the message type of the original transaction.
       */
      ProcCode = atoi( p_host_auth_tx->TLF01_details.processing_code );
      ProcCode = ProcCode / 10000;   /* Get just the 1st 2 digits. */

      switch( ProcCode )
      {
         case 30:   /* Pre-Authorization  */
         case 31:   /* Balance Inquiry    */
         case  4:   /* Check Verification */
         case 38:   /* Card Verification  */
                  strncpy(p_orig_auth_tx->TLF01_details.message_type,"0110",4);
                  break;

         case 20:   /* Refund       */
         case  1:   /* Cash Advance */
         case 40:   /* Payment      */
         case  9:   /* Sale & Cash  */
         case  2:   /* Void Sale    */
         case 42:   /* Void Payment */
         case 22:   /* Void Refund  */
                  strncpy(p_orig_auth_tx->TLF01_details.message_type,"0210",4);
                  break;

         default:
                  /* Check to see if it is an Authorization. */
                  if ( 0 == strncmp( p_orig_auth_tx->TLF01_details.message_type,
                                     "0100", 4 ) )
                  {
                     /* It is an Authorization. */
                     strncpy( p_orig_auth_tx->TLF01_details.message_type,
                              "0110", 4 );
                  }
                  else
                  {
                     /* It is not an Authorization; it must be a Sale. */
                     strncpy( p_orig_auth_tx->TLF01_details.message_type,
                              "0210", 4 );
                  }
                  break;
      } /* End Switch */
   }
   else
   {
      /* This is not a message type of "0110".
       * So just copy it straight over.
       */
      strncpy( p_orig_auth_tx->TLF01_details.message_type,
               p_host_auth_tx->TLF01_details.message_type,
               4 );
   }
}

/******************************************************************************
 *
 *  NAME:         CREATE_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function creates a table in shared memory consisting
 *                of 3 values:
 *                             number of consecutive time outs
 *                             number of active transactions
 *                             host state
 *
 *                These values get initialized to: 0, 0, 2 respectively.
 *                Note that host state of '2' means not logged on yet.
 *
 *  INPUTS:       table_name - Name of table to be created in shared memory
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN create_shared_mem_table( pCHAR table_name )
{
   BOOLEAN  ret_val = false;
   INT      ret_code;
   CHAR     Buffer[76];

   memset( SHARED_MEMORY_TBL, 0, sizeof(SHARED_MEMORY_TBL) );
   strcpy( SHARED_MEMORY_TBL, cur_app_name );
   strcat( SHARED_MEMORY_TBL, "Table" );

   ret_code = create_generic_table( SHARED_MEMORY_TBL );
   if ( MEMACCESSERROR == ret_code )
   {
      /* Table already exist. */
      ret_val = true;
   }
   else if ( 0 == ret_code )
   {
      /* Table was successfully created.
       * Initialize host counters and state upon startup.
       */
     ret_val = write_to_shared_mem_table( "0", "0", OFFLINE );
   }
   else
   {
      /* Error creating the shared memory table. */
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to create shared memory table: %s.",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG,
                             "create_shared_mem_table", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "491");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         WRITE_TO_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function writes to the shared memory table, defined
 *                with a global variable.  It writes 3 values to the table.
 *                These values are defined below in the INPUTS section.
 *
 *  INPUTS:       consec_time_out_count
 *                active_tran_count
 *                host_state
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
BOOLEAN write_to_shared_mem_table( pBYTE consec_time_out_count,
                                   pBYTE active_tran_count,
                                   pBYTE host_state )
{
   BOOLEAN  ret_val = true;
   LONG     ret_code;
   CHAR     Buffer[76];


   ret_code = WriteGenericTable( SHARED_MEMORY_TBL, consec_time_out_count,
                                 active_tran_count, host_state );
   if ( 0 != ret_code )
   {
      /* Unable to write to shared table. */
      ret_val = false;
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to write to shared memory table: %s.",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG,
                             "write_to_shared_mem_table", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "492");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         READ_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function reads the shared memory table, defined
 *                with a global variable.  It reads 3 values from the table.
 *                These values are defined below in the OUTPUTS section.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      consec_time_out_count
 *                active_tran_count
 *                host_state
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
BOOLEAN read_shared_mem_table( pBYTE consec_time_out_count,
                               pBYTE active_tran_count,
                               pBYTE host_state )
{
   BOOLEAN  ret_val = true;
   INT      ret_code;
   CHAR     Buffer[76];

   ret_code = ReadGenericTable( SHARED_MEMORY_TBL, consec_time_out_count,
                                active_tran_count, host_state );
   if ( 0 != ret_code )
   {
      ret_val = false;
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to read shared memory table: %s",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG,
                             "read_shared_mem_table", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "580");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SET_HOST_STATE
 *
 *  DESCRIPTION:  This function sets the state of the host in shared memory.
 *
 *  INPUTS:       host_state - State to set the host
 *                                0 = Logged on
 *                                1 = Not logged on
 *                                2 = Logged on, but busy
 *                                3 = Service is down
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
BOOLEAN set_host_state( pBYTE host_state )
{
   BOOLEAN  ret_val = false;
   CHAR     Buffer[76];
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     temp_host_state[2];
   BYTE     update_monitor;
   WORD     echo_interval;
   INT      timeouts;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt, temp_host_state );
   if ( true == ret_val )
   {
      if ( 0 != strcmp(host_state,temp_host_state) )
         update_monitor = 1;
      else
         update_monitor = 0;

      /* Store new values to shared memory. */
      ret_val = write_to_shared_mem_table( timeout_cnt, active_cnt, host_state);
      if ( true == ret_val )
      {
         echo_interval = atoi( ncf01.echo_interval );
         if ( echo_interval > 0 )
         {
            if ( host_state[0] == cONLINE )
            {
               /* Since we just went online, start the idle line timer. */
               if ( false == set_echo_timer( ECHO_TIMER_KEY, echo_interval,
                                             (pPTE_MSG*)NULL_PTR,
                                             application_que_name, 1 ) )
               {
                  ret_val = true;
                  cirsutil_error_handler(1,
                           "Unable to set idle line timer", 1, WARN_MSG,
                           "set_host_state", 4, WARNING_ERROR, NULL_STR,
                            NULL_STR, NULL_STR, "800");
               }
            }
            else if ( host_state[0] == cDOWN )
            {
               /* Since we just went down, clear the idle line timer. */
               if ( false == clear_echo_timer( ECHO_TIMER_KEY,
                                               (pPTE_MSG *)NULL_PTR,
                                               application_que_name, 1 ) )
               {
                  ret_val = true;
                  cirsutil_error_handler(1,
                           "Unable to clear the echo timer", 1, WARN_MSG,
                           "set_host_state", 4, WARNING_ERROR, NULL_STR,
                            NULL_STR, NULL_STR, "801");
               }
            }
            else if ( host_state[0] == cOFFLINE )
            {
               /* Clear echo timer only if we did not go offline
                * due to max time outs.  To do this, clear it,
                * then reset it if offline due to max timeouts.
                */
               if ( false == clear_echo_timer( ECHO_TIMER_KEY,
                                               (pPTE_MSG *)NULL_PTR,
                                               application_que_name, 1 ) )
               {
                  cirsutil_error_handler(1,
                           "Unable to clear the echo timer", 1, WARN_MSG,
                           "set_host_state", 4, WARNING_ERROR, NULL_STR,
                            NULL_STR, NULL_STR, "803");
               }
               else if ( MaxConsecTimeouts > 0 )
               {
                  timeouts = atoi(timeout_cnt);
                  if ( timeouts >= MaxConsecTimeouts )
                  {
                     if ( false == set_echo_timer( ECHO_TIMER_KEY,echo_interval,
                                                   (pPTE_MSG*)NULL_PTR,
                                                   application_que_name, 1 ) )
                     {
                        cirsutil_error_handler(1,"Unable to set echo timer.",
                                               1, ERROR_MSG, "set_host_state",3,
                                               WARNING_ERROR, NULL_STR,
                                               NULL_STR, NULL_STR, "802");
                     }
                  }
               }
            }
         }
         if ( update_monitor == 1 )
            send_network_status_to_monitor();
      }
   }
   if ( false == ret_val )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to set host state in shared memory table: %s",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG, "set_host_state", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "581");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         DECREMENT_ACTIVE_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, decrements the
 *                active transaction counter, and stores those values back
 *                into shared memory.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN decrement_active_count( )
{
   BOOLEAN  ret_val = false;
   CHAR     Buffer[76];
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt, host_state );
   if ( true == ret_val )
   {
      /* Decrement the active transactions counter field */
      tempint = atoi(active_cnt) - 1;
      if ( 0 > tempint )
      {
         /* If less than zero set it to zero */
         strcpy( active_cnt,"0" );
      }
      else
      {
         sprintf( active_cnt, "%d", tempint );
      }

      /* Check number of active transactions. */
      if ( MaxActiveTxns > 0 )
      {
         if ( 0 == strcmp(host_state,QUERYING) )
         {
            if ( tempint < MaxActiveTxns )
            {
               /* No longer busy.  Set state to Online. */
               strcpy( host_state, ONLINE );
            }
         }
      }

      /* Update the values into shared memory. */
      ret_val = write_to_shared_mem_table( timeout_cnt, active_cnt, host_state);
   }

   if ( false == ret_val )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to update shared memory table: %s",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG, "decrement_active_count",
                             4, FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                            "582");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_ACTIVE_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                active transaction counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of active transactions to the max allowed.  If too high,
 *                a warning is posted, but processing continues.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN increment_active_count( )
{
   BOOLEAN  ret_val = false;
   CHAR     Buffer[76];
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt, host_state );
   if ( true == ret_val )
   {
      /* Increment the active transactions counter field */
      tempint = atoi(active_cnt) + 1;

      /* Check number of active transactions. */
      if ( (tempint >= MaxActiveTxns) && (MaxActiveTxns > 0) )
      {
         /* Transaction count is too high.
          * Set state to not allow any more.
          */
         memset( Buffer, 0x00, sizeof(Buffer) );
         (void)set_host_state( QUERYING );
         strcpy( Buffer,
                "Active transaction count too high. Continuing to process.");

         cirsutil_error_handler(1, Buffer, 1, ALERT_MSG,
                               "increment_active_count", 4, FATAL_ERROR,
                                NULL_STR, NULL_STR, NULL_STR, "584");
      }
      else
      {
         /* Don't let this value get too high. */
         if ( tempint > 999 )
            tempint = 1;
         sprintf( active_cnt, "%d", tempint );

         /* Update the values into shared memory. */
         ret_val = write_to_shared_mem_table(timeout_cnt,active_cnt,host_state);
      }
   }

   if ( false == ret_val )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to update shared memory table: %s",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG, "increment_active_count",
                             4, FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "584");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_TIMEOUT_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                transaction timeout counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of time outs to the max allowed.  If too high, a warning is
 *                posted.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
BOOLEAN increment_timeout_count( )
{
   BOOLEAN  ret_val = false;
   CHAR     Buffer[76];
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt, host_state );
   if ( true == ret_val )
   {
      /* Increment the consecutive timeout counter field */
      tempint = atoi(timeout_cnt) + 1;

      /* Check number of consecutive timeouts. */
      if ( MaxConsecTimeouts > 0 )
      {
         if ( host_state[0] == cONLINE )
         {
            if ( tempint > MaxConsecTimeouts )
            {
               host_state[0] = cOFFLINE;
               memset( Buffer, 0x00, sizeof(Buffer) );
               strcpy( Buffer, "Maximum consecutive timeouts reached.");
               cirsutil_error_handler(1, Buffer,
                                1, WARN_MSG, "increment_timeout_count",
                                3, WARNING_ERROR, NULL_STR, NULL_STR,
                                NULL_STR, "");
            }
         }
      }

      /* Don't let this value get too high. */
      if ( tempint > 999 )
         tempint = 1;
      sprintf( timeout_cnt, "%d", tempint );

      /* Update the values into shared memory. */
      ret_val = write_to_shared_mem_table( timeout_cnt, active_cnt, host_state);
   }

   if ( false == ret_val )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to update shared memory table: %s",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG, "increment_timeout_count",
                             4, FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "586");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         RESET_TIMEOUT_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, resets the
 *                transaction timeout counter to zero, and stores those
 *                values back into shared memory.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN reset_timeout_count( )
{
   BOOLEAN  ret_val = false;
   CHAR     Buffer[76];
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt, host_state );
   if ( true == ret_val )
   {
      /* Update the values into shared memory, setting time out count to 0. */
      ret_val = write_to_shared_mem_table( "0", active_cnt, host_state);
   }

   if ( false == ret_val )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Unable to update shared memory table: %s",
               SHARED_MEMORY_TBL );
      cirsutil_error_handler(1, Buffer, 1, ALERT_MSG, "reset_timeout_count",
                             4, FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                             "587");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         TRIM_SPACES
 *
 *  DESCRIPTION:  This function converts spaces at the end of a string into
 *                nulls.
 *
 *  INPUTS:       pString - String that does not want spaces at the end
 *
 *  OUTPUTS:      pString - Same string, but without trailing spaces
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void trim_spaces( pBYTE pString )
{

   INT len, position;

   len = strlen(pString) - 1;
   for(position = len; position >= 0; position--)
   {
      if(pString[position] == ' ')
         pString[position] = 0;
      else
         break;
   }
}


/*****************************************************************************

  Function:    mcrdutil_get_response_text

  Description: This function will fill the response_text field in auth_tx.TLF01.

  Author: 
      BGable     5/26/99
  Inputs:
     pAUTH_TX - pointer to auth_tx
  Outputs:
     pAUTH_TX - pointer to auth_tx (with the response text updated)
  Return values:
     None
  Modified by:
******************************************************************************/
VOID cirsutil_get_response_text( pAUTH_TX p_auth_tx)
{
   INT i;
   static unsigned char response[][45] = {
   {"00Approved"},
   {"01Refer to Card Issuer"},
   {"03Invalid Merchant"},   
   {"04Capture Card"},
   {"05Do Not Honor"},

   {"06Error"},   
   {"08Honor with identification"},   
   {"09Request In Progress"},   
   {"12Invalid transaction"},   
   {"13Invalid Amount"},   
   
   {"14Invalid Card Number"},   
   {"15No Such Issuer"},   
   {"25Unable to locate record on file"},   
   {"27File update field edit error"},   
   {"30Format Error"},   
   
   {"31No Bank Support by Switch"},   
   {"33Expired Card, Pick-Up"},   
   {"34Suspected Card, Pick-Up"},   
   {"36Restricted Card, Pick-Up"},   
   {"40Function Not Supported"},   
   
   {"41Lost, Stolen, Pick-Up"},   
   {"42Universal Acct Not Found"},   
   {"43Stolen Card, Pick-Up"},   
   {"51Not Sufficient Funds"},   
   {"54Expired Card, Pick-Up"},   
   
   {"55Incorrect PIN"},   
   {"56No Card Record"},   
   {"57Trnsxn Not Permitted To Card"},   
   {"58Trnsxn Not Permintted To Term"},   
   {"59Suspected Fraud, Not Pick-Up"},   
   
   {"61Exceeds Amount Limit"},   
   {"62Restricted Card, Not Pick-Up"},   
   {"68Response Received Too Late"},   
   
   {"71Message out of exception cycle sequence"},   
   {"72Exception cycle time expired"},   
   {"75# of PIN Tries Exceeded"},   
   {"76Invalid/nonexistent TO account specified"},   
   {"77Invalid/nonexistent FROM account specified"},   
   {"78Invalid/nonexistent account specified"},   
   {"79Key exchange validation failed"},   
   
   {"80System not available"},   
   {"84Invalid authorization Life Cycle"},   
   {"85Not declined"},   
   {"90Cutoff Is In Progress"},   
   {"91Issuer Or Switch Inoperative"},   
   
   {"92Institution Can't Be Routed To"},   
   {"94Duplicate Transmission"},   
   {"96System Malfunction"}   
   };

 
   for (i = 0; i < 47; i++)
   {
      if (0 == strncmp(p_auth_tx->TLF01_details.response_code, response[i], 2))
      {
         strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[i]+2);
         break;
      }

   }
   if ('\0' == p_auth_tx->TLF01_details.voice_auth_text[0])
   {
      strcpy(p_auth_tx->TLF01_details.voice_auth_text, "Not Approved" );
   }
}

/******************************************************************************
 *
 *  NAME:         INSERT_INTO_TLF01
 *
 *  DESCRIPTION:  This function sends a request to the Device Data Server to
 *                insert auth_tx.TLF01_details into TLF01.  Its response will
 *                be ignored.
 *
 *  INPUTS:       p_auth_tx
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
BOOLEAN insert_into_tlf01( pAUTH_TX  p_auth_tx )
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BOOLEAN    ret_code  = false;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];

   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
   {
      /* TLF01 has already been inserted. */
      ret_code = true;
   }
   else
   {
      /* Set transaction finish time. */
      ptetime_get_strtime(p_auth_tx->TLF01_details.tran_finish_time);

      p_auth_tx->host2_acquirer_cb_code[0] = '*';
      memset( buffer, 0, sizeof(buffer) );
      memcpy( buffer+Auth_Tx_Size,&p_auth_tx->TLF01_details, Tlf01_Size);

      p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
                                   ST1_DB_INSERT, 
                                   ST2_NONE,
                                   updateds_que_name, 
                                   application_que_name,
                                   (pBYTE)&buffer,
                                   sizeof(buffer), 
                                   TLF01_DATA); 
      if ( NULL_PTR == p_msg_ins )
      {
         cirsutil_error_handler(1,
                            "Insufficient Memory to build insert TLF01 message",
                             1, ALERT_MSG, "insert_into_tlf01", 4,
                             FATAL_ERROR, p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "504");
      }
      else
      {
         ret_code = send_msg( p_msg_ins, updateds_que_name, p_auth_tx );
         free( p_msg_ins );
      }
   }
   return( ret_code );
}


/*****************************************************************************

  Function:    hhutil_get_ncf01_cfg

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously and should be used only by
               visamain.c!!!
  Author: 
      BGable     6/30/98
  Inputs:
      pncf01 - pointer to the requested ncf01 structure
  Outputs:
      pncf01 - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	pPTE_MSG		p_msg_in;
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	BYTE			rcode;
	BYTE     buffer[sizeof(AUTH_TX)+sizeof(NCF01)];
   AUTH_TX  auth_tx;

   memset(buffer,0, sizeof(buffer));

   memcpy(buffer,&auth_tx, Auth_Tx_Size);
   memcpy(buffer+Auth_Tx_Size,&ncf01, Ncf01_Size);

	p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_SELECT,
								  0,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  NCF01_DATA );


	if( ( rcode = hhutil_process_request( p_msg_out, &p_msg_in, netds_que_name, error_msg ) ) != PTEMSG_OK )	
		return ( rcode );

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memset (pncf01, 0, Ncf01_Size);
   memcpy(pncf01, p_data+(sizeof(auth_tx)), Ncf01_Size);
	free( p_msg_in );
	return ( PTEMSG_OK );
}


/*****************************************************************************

  Function:    hhutil_process_request

  Description: This function will send a request to the dataserver.  It is used
               by hhutil_get_ncf01_cfg.
  Author: 
      unknown
  Inputs:
      p_msg_out - a pointer to the outgoing message (the request to the dataserver)
      p_msg_in  - a pointer to the incoming message (from the dataserver)
      que_name  - the name of the queue to send the request to
      error_msg - the errror message from the dataserver
  Outputs:
      None
  Return values:
      dberror - the type of error
  Modified by:
******************************************************************************/
BYTE hhutil_process_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR que_name, pCHAR error_msg )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[100];
	LONG			retcode;
	BYTE			dberror;

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name, interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
	{
		pteipc_get_errormsg( retcode, temp_str );
		sprintf( error_msg, "Communication Error during request to Terminal DataServer: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
	{
		p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

      strcpy (temp_str, p_data+Auth_Tx_Size);
		sprintf( error_msg, "Database Error: %s", temp_str );
		free( *p_msg_in );
	}

	return dberror;
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
 *  AUTHOR:       Beth Gable
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


/******************************************************************************
 *
 *  NAME:         WRITE_TO_TXN_FILE
 *
 *  DESCRIPTION:  This function calls the necessary library APIs to write a
 *                stream of data to the transaction trace file.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void write_to_txn_file()
{
   INT  rc;
   CHAR err_buf[256];

   /* Log transaction to trace file. */
   if ( Tracing == TRACE_ON )
   {
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,buflen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         TxUtils_Send_Msg_To_Operator(1,err_buf,
                                      1,INFO_MSG,"write_to_txn_file",
                                      2,INFO_ERROR, 0,0,0);
         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         TxUtils_Send_Msg_To_Operator(1,err_buf,
                                      1,INFO_MSG,"write_to_txn_file",
                                      2,INFO_ERROR, 0,0,0);

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            TxUtils_Send_Msg_To_Operator(1,err_buf,
                                         1,INFO_MSG,"write_to_txn_file",
                                         2,INFO_ERROR, 0,0,0);
         }
      }
   }
   return;
}
