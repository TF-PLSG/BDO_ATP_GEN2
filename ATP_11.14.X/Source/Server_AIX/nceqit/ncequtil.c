/******************************************************************************
 
 	ncequtil.c
 
    Copyright (c) 2004, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Equitable Host Handler Driver
 
 	This module is the host handler for the Equitable host.  It
    will process all messages destined to, or originated from,
    the Equitable host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nceqit_ncmlnk_iso_format\ncequtil.c  $

	Rev 1.44   Jan 22 2009 14:45:00   Girija Y TF
   1) Reverted back changes for Fld 48 for 0422 request msg
   08-40 ATP - EQIT INTERFACE

	Rev 1.43   Jan 21 2009 14:10:00   Girija Y TF
   1) Added code to pass 0432 response from host
   2) Added code to pass Fld 48 for 0422 request msg
   08-40 ATP - EQIT INTERFACE
   
      Rev 1.42   Oct 12 2005 14:51:38   dirby
   Added function 'is_void'
   SCR 17660
   
      Rev 1.41   Oct 05 2005 12:15:14   dirby
   Changed so that Voids (Sale and Cash) are treated like reversals
   and sent to the host as 0420's.
   SCR 17660
   
      Rev 1.40   Jul 14 2004 14:36:10   dirby
   Added support for Reload transactions.
   SCR 1287
   
      Rev 1.39   Jun 08 2004 13:41:10   dirby
   Copy text into response_text when host inactive - for Voice GUI.
   Release 4.3.0-1
   
      Rev 1.38   Mar 24 2004 15:14:26   dirby
   Added fields 36, 102, and 103 to 0200 request messages.
   Added field 36 to reversal requests.
   SCR 1352
   
      Rev 1.37   Mar 01 2004 08:36:02   dirby
   Modified to always use 2 bitmaps for Megalink.
   SCR 1352
   
      Rev 1.36   Sep 29 2003 13:51:38   dirby
   Added support for Fee Collection and Funds Disbursement for
   Acquirers.
   SCR 1083
   
      Rev 1.35   Sep 11 2003 17:20:08   svelaga
   Backoffice adjustmenet, representments are directly sent to 
   ncvsms.
   
      Rev 1.34   Sep 11 2003 15:57:16   dirby
   Added function map_response_code.
   SCR 1186
   
      Rev 1.33   Sep 02 2003 16:41:08   svelaga
   AUTH_ADMINISTRATIVE messages are directly sent to ncvsms.
   
      Rev 1.32   Aug 28 2003 14:30:20   svelaga
   Fixed bugs/issues with 0600 message.
   
      Rev 1.31   Aug 25 2003 13:47:58   svelaga
   Fixed
   1. Added support for Field 62
   2. No check for valid NMI code for Administrative text message.
   3. Backoffice adjustment and Representment are distinguised based on Field 25 ( pos condition code ) instead of processing code.
   
      Rev 1.30   Aug 05 2003 10:29:08   svelaga
   VISASMS Specification Changes: New Private field 62 and some changes to Private field 63.
   
      Rev 1.29   Jul 31 2003 16:44:10   svelaga
   Visa SMS QA bug fixes.
   
      Rev 1.28   Jul 30 2003 14:45:50   svelaga
   Administrative message ( 0620 ) message added.
   
      Rev 1.27   Jul 25 2003 17:33:06   svelaga
   SMS projects changes -
   1. Support for BACKOFFICE_ADJUSTMENT, REPRESENTMENT transactions.
   2. Added to parse and build some extra fields like DE48, DE63,
   DE18,DE19 etc...
   
      Rev 1.26   Apr 10 2003 14:53:42   dirby
   Modified to set RRN equal to the original RRN for reversals.
   This is because for Plus Transactions, EB wants/needs the
   RRN in TLF01 to be the same as the original txn.  They look at
   this in the auth_log file.
   SCR 1089
   
      Rev 1.25   Oct 16 2002 11:51:00   dirby
   Made sure global variable TranFlow is correctly set prior to
   sending a request or response to the host.  Its setting was
   being lost during a pin translation operation.
   SCR 820
   
      Rev 1.24   Aug 09 2002 16:13:10   dirby
   Modified to not send a response to a reversal immediately.
   Instead, send it after we get the original txn from NCF30.  This
   is so the Merchant ID can be populated from the NCF30 record
   into TLF01.
   SCR 871
   
      Rev 1.23   Jul 07 2002 03:28:38   dirby
   Fixed bug - Return false if host is not online when checking status.
   SCR 594
   
      Rev 1.22   Jul 07 2002 02:36:34   dirby
   Reimplemented going offline when max time outs occur.
   SCR 594
   
      Rev 1.21   Jun 24 2002 11:46:06   dirby
   Changed End Sentinel in Track2 from 'f' to '?'.
   SCR 790
   
      Rev 1.20   Jun 24 2002 11:09:00   dirby
   Added function 'remove_track2_end_sentinel'. This is used to
   avoid Visa rejects 0027.
   SCR 790
   
      Rev 1.19   Jun 18 2002 17:00:18   dirby
   Removed diagnostic information that is no longer needed.
   SCR 789
   
      Rev 1.18   Jun 18 2002 03:01:54   dirby
   Added yet some more diagnostics to show data from the field
   that is causing the error.
   SCR 789
   
      Rev 1.17   Jun 17 2002 22:48:26   dirby
   Added error message to be logged if, while building a request
   message, the message index goes negative.  However, processing continues.  This problem will be captured again later.
   SCR 789
   
      Rev 1.16   Jun 17 2002 20:03:56   dirby
   Corrected some logic bugs in 'build_and_send_request_message'.
   SCR 789
   
      Rev 1.15   Jun 17 2002 16:08:16   dirby
   Modified to log txn to txn trace file even if length is negative.
   SCR 789
   
      Rev 1.14   Jun 17 2002 14:29:08   dirby
   Modified to log ReqRsp_Buffer to txn trace file if invalid size.
   SCR 789
   
      Rev 1.13   Jun 17 2002 13:39:00   dirby
   Modified to validate a message before attempting to build a
   PTE message.  This is done by checking the buffer size.  This
   stems from EB seeing some error messages for insufficient
   memory when sending a request to the EB host.
   SCR 789
   
      Rev 1.12   May 24 2002 16:22:12   dirby
   Modified to store partial reversal amount into refund_amount
   and total_amount into sales_amount.  They were using nfi fields
   in TLF01, but they were not large enough.
   SCR 745
   
      Rev 1.11   Apr 04 2002 18:19:28   dirby
   Updated to version 4.0.2.4
   Fixed bug where function blank_string is called.  Need to send
   size of field minus 1, otherwise the trailing null is checked.
   SCR 681
   
      Rev 1.10   Mar 08 2002 13:46:04   dirby
   Modified to not build 0200 message when receiving an ATM
   Confirm (0102) from Visa.  Needs to be built as a reversal.
   Previously, a reversal (0420) was going out, but the reversal
   fields DE90/95 were not being built; so they were nulls.
   SCR 653
   
      Rev 1.9   Jan 18 2002 11:24:26   dirby
   Corrected some minor bugs.
   SCRs 488 and 484
   
      Rev 1.8   Jan 17 2002 16:38:16   dirby
   Added code to log original amount and currency code to TLF01
   prior to them being modified for currency conversion.     SCR 565
   
      Rev 1.7   Jan 16 2002 12:31:28   dirby
   Added transaction tracing capability.     SCR 546
   
      Rev 1.6   Jan 14 2002 18:15:06   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.5   Dec 14 2001 14:53:22   dirby
   Added some nceqit_log_message calls for some error messages
   that were being formed, but not logged.
   SCR 87
   
      Rev 1.4   Dec 10 2001 14:07:48   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.3   Aug 20 2001 17:13:48   dirby
   Corrected some indentation issues.
   
      Rev 1.2   Aug 14 2001 10:28:28   SYARLAGA
   Modified  parse_host_msg function.
   
      Rev 1.1   Aug 13 2001 17:39:28   SYARLAGA
   Modified init_Auth_Tx , build_host_msg andparse_host_msg
   functions.
   
      Rev 1.0   Aug 06 2001 09:49:02   dirby
   Initial revision.
   
      Rev 1.27   Oct 18 2000 15:03:58   dirby
   Made the POS Entry Mode a little bit more efficient.
   
      Rev 1.26   Aug 23 2000 14:10:56   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.25   Jul 21 2000 09:05:36   dirby
   Modified to use Acquirer CB Code in request from HOST2
   as the Merchant ID.  Also modified code to use the 1st two
   characters of Acquirer CB Code (instead of 2nd and 3rd chars)
   as the key for ATM01.
   
      Rev 1.24   Jul 19 2000 10:44:08   dirby
   Added transaction type AUTHORIZATION.  This is for Visa Electron transactions.
   
      Rev 1.23   Jul 18 2000 16:07:10   dirby
   Added debit processing codes for 0100 messages
   to the processing code / transaction type table.
   
      Rev 1.22   Jul 14 2000 11:15:28   dirby
   1.  Added code to get Acceptor Term Name and Acceptor City Name from MCF01,
       if they do not exist where we are currently looking for them.  Will also set the
       Acceptor Country to PH.
   2.  Modified Local Trans Date from 'yymm' format into 'mmdd' format.
   3.  Added code to set 'Trans Amount' to 'Acquirer Transaction Amt.' if it does not exist.
   
      Rev 1.21   Jun 21 2000 21:29:58   dirby
   Added tx_key AUTH_SALE to support additional processing codes, per equitable issue #4.
   Changed tx_key in the trantable from Cash Advance to Sale for processing codes of '00xxxx'.
   
      Rev 1.20   Jun 08 2000 16:46:36   dirby
   When parsing amount fields, NCEQIT would crash due to
   using a strlen where there is no NULL.  Changed the way
   amounts are parsed to not use strlen.
   
      Rev 1.19   Jun 07 2000 17:08:44   dirby
   Added processing codes in 'trantype_table'.
   
      Rev 1.18   May 10 2000 13:47:56   svelaga
   Changes according to new host2 format.
   
      Rev 1.17   Jan 14 2000 08:57:58   dirby
   Disabled a message length check, at customer's request.
   
      Rev 1.16   Jul 30 1999 09:42:06   egable
   Add processing codes xx4xxx to the tran
   table.
   
      Rev 1.15   Jul 27 1999 19:01:46   egable
   Change the processing code for balance
   inquiry.
   
      Rev 1.14   Jul 26 1999 18:33:00   egable
   Add 0224, 0424, 0524 to the trantable
   
      Rev 1.13   Jul 14 1999 18:28:34   rmalathk
   several changes per EB's request.
   1. On reversal, respond to EB host immediately
       and ignore response from txcntl.
   2. changed tran_codes for reversals.
   3. changed format of acquirer_cb_code and
      terminal_id when receiving request from Eb
      host.
   
      Rev 1.12   Jul 01 1999 14:04:40   svelaga
   fixed bug :
   For  actual amount: for reversal only
   by modifying the memcpy statement
   
      Rev 1.11   Jun 17 1999 16:18:42   rmalathk
   1. create and use our own rrn instead of using
       rrn sent by the EB host.
   2. use src_date and src_time to populate TLF01
       instead of local_date and local_time
   3. update NCF30 only for AUTH_CASH_ADVANCE.
   
   
      Rev 1.10   Jun 16 1999 16:46:34   rmalathk
   1. Added response code of 'TO' to the response code list.
   2. use pos_entry_mode of 0021 when populating
       TLF01.
   3. changed error message to display both msg_type
       and processing_code when we cannot build
       a message to the EB host.
   
      Rev 1.9   Jun 15 1999 12:46:16   rmalathk
   strip out all trailing spaces in the terminal_id
   field when processing incoming transaction
   from EB host.
   
      Rev 1.8   Jun 14 1999 15:15:14   rmalathk
   right justify and zero-fill the amount fields
   when populating TLF01
   
      Rev 1.7   Jun 10 1999 15:15:44   rmalathk
   1. changes to accomodate different message
       lengths for reversals and authorization request.
   2. changes to add 2 more fields in the response
       message.
   
      Rev 1.6   May 25 1999 10:30:06   rmalathk
   1. fix to filter out semi-colon when extracting
       card number from track2.
   2. fix to remove null byte when building response
       to EB host.
   
      Rev 1.4   01 Apr 1999 11:24:48   rmalathk
   1. minor bug fixes and changes to populate
       host_start_time, finish_time, acquirer_id fields
       in TLF01.
   2. minor changes for sending the network
       status to monitor.
   
      Rev 1.3   17 Mar 1999 11:09:20   rmalathk
   Several changes made to message format
   after reviewing traces from Equitable.
   
      Rev 1.2   04 Mar 1999 16:19:58   rmalathk
   Added more error-checking, modified build settings.
   
      Rev 1.1   26 Feb 1999 11:31:02   rmalathk
   minor bug fixes: populate balance fields
   in auth_tx after response received from
   host.
   
      Rev 1.0   24 Feb 1999 16:32:54   rmalathk
   Initial Revision
   
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "ptetime.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "ntutils.h"
#include "txutils.h"
#include "memmnger.h"
#include "nceq_constants.h"
#include "nceq_prototypes.h"
#include "nceq_bitfields.h"
#include "txtrace.h"
#include "nceq_bitfields.h"

/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

extern CHAR    AppName[];

/* QUEUES */
extern CHAR    updateds_que_name[];
extern CHAR    nsp_que_name[];
extern CHAR    trands_que_name[];
extern CHAR    netds_que_name[];
extern CHAR    devds_que_name[];
extern CHAR    oprmon_que_name[];
extern CHAR    applnk_que_name[];
extern CHAR    authorizer_que_name[];
extern CHAR    timerds_que_name[];
extern CHAR    dialog_que_name[];

extern CHAR		  visasms_que_name [];


/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern ATM01   Atm01;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Atm01_Size;
extern INT  Tlf01_Size;

/* Network:  nceqit or ncmlnk */
extern INT  Network;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;

/* Originator Information */
extern BYTE    Orig_Queue[ ];
extern BYTE    Orig_Info[];

/* Network Type: Issuer, Acquirer, or Both */
extern INT     NetworkType;
extern INT     TranFlow;

extern CHAR    TimerAppData2[];
extern INT second_Bitmap;
/* LOCAL VARIABLES */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    BitFieldSize;
static void vBCDMove(enum bitfields fieldid);
static void RspMovevBCD (enum bitfields fieldid);
//static  BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type );

//ramya
extern CHAR  nceqit_Erro_warning_Filename[256] = {0};
extern CHAR  nceqit_module_error_warning_file_name[256] = {0};
extern CHAR  nceqit_error_warning_file_path[256] = {0};
UINT Max_File_Size_Defined = 0 ;
/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE   ReqRsp_Buffer [COMM_BUF_SZE];

struct trantype_struct 
{
   BYTE iso_msg_type[5];
   BYTE iso_proc_code[7];
   BYTE host2_msg_type[3];
   BYTE host2_tran_code[5];
   BYTE atp_tx_key;
}; 


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


/* The following are used for mapping host response codes into ATP codes. */
#define  NUM_RESP_CODES  75

BYTE Host_Resp_Codes[][3] = 
         { "00", "01", "02", "03", "04", "05", "07", "12", "13", "14",
           "15", "17", "19", "21", "22", "25", "27", "28", "30", "32",
           "33", "38", "39", "40", "41", "42", "43", "44", "51", "52",
           "53", "54", "55", "56", "58", "59", "61", "62", "63", "64",
           "65", "67", "68", "75", "77", "78", "79", "80", "81", "82",
           "83", "85", "86", "90", "91", "92", "93", "96", "N1", "N2",
           "N3", "N4", "N5", "NA", "NB", "NC", "ND", "NE", "NF", "NG",
           "NH", "NI", "NJ", "NK", "NL" };

BYTE ATP_Resp_Codes[][3] = 
         { "00", "01", "58", "03", "43", "57", "43", "12", "13", "14",
           "14", "25", "96", "96", "96", "14", "96", "96", "30", "00",
           "54", "75", "14", "58", "41", "12", "43", "12", "51", "52",
           "53", "54", "55", "14", "89", "58", "61", "14", "63", "13",
           "61", "41", "96", "75", "96", "63", "96", "58", "91", "02",
           "96", "96", "96", "96", "91", "96", "63", "96", "25", "96",
           "96", "91", "25", "14", "14", "14", "14", "58", "58", "14",
           "96", "14", "96", "91", "96" };


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

/*****************************************************************************

  Function:    vBCDMove 

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer[reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen = (movelen / 2);

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vBCDMove */

/*****************************************************************************

  Function:    

  Description: This function will move data from the raw ISO message to 
               the HOST_MESSAGE structure.  
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure for the outgoing ISO message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if ( (ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x02) )
      {
         /* Convert length from BCD to binary. */
         movelen  = ((ReqRsp_Buffer[reqidx] & 0xf0) >> 4) * 10;
         movelen +=   ReqRsp_Buffer[reqidx] & 0x0f;
      }
      else
         movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = movelen;
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      if (( reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */

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
 *  NAME:         COPY_TO_REQ_RESP_BUFFER
 *
 *  DESCRIPTION:  This function will copy data from a PTE message into
 *                ReqRsp_Buffer.
 *
 *  INPUTS:       p_msg - This is the incoming message from the host.
 *
 *  OUTPUTS:      ReqRsp_Buffer - (Global)Character string with txn data
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void copy_to_req_resp_buffer ( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA    p_msg_data  = NULL_PTR;
   pBYTE            p_data      = NULL_PTR;
   LONG             msglen;

   memset(ReqRsp_Buffer, 0x00, COMM_BUF_SZE);   /* zero out the buffer */

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   msglen     = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   reqidx = 0;
   buflen = msglen;
   memcpy(ReqRsp_Buffer, p_data, msglen);
   invalid_msg_format = false;

   /* Retrieve originator info from incoming message */
   strcpy( Orig_Info, ptemsg_get_msg_orig_comms_info( p_msg_in ) );
   strcpy( Orig_Queue, ptemsg_get_msg_orig_queue( p_msg_in ) );

   return;
}

/******************************************************************************
 *
 *  NAME:         DETERMINE_TXN_FLOW
 *
 *  DESCRIPTION:  This function determines if a transaction is switched in
 *                from host, or switched out to the host for validation.
 *
 *  INPUTS:       source - Gives source of txn: HOST or ATP
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   SWITCHED_IN  or  SWITCHED_OUT
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT determine_txn_flow( pCHAR msgtype, INT source )
{
   INT  tran_flow;

   /* Third character of msgtype is 1 or 3 for responses. */
   if ( (msgtype[2] == '1') || (msgtype[2] == '3') )
   {
      /* This is a response.  Where did it come from? */
      if ( source == HOST )
         tran_flow = SWITCHED_OUT;
      else
         tran_flow = SWITCHED_IN;
   }
   else
   {
      /* This is a request. */
      if ( source == HOST )
         tran_flow = SWITCHED_IN;
      else
         tran_flow = SWITCHED_OUT;
   }
   return( tran_flow );
}


/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_HOST
 *
 *  DESCRIPTION:  This function sends a response to the host.  It gets the
 *                response code and other information from Auth_Tx.
 *
 *  INPUTS:       send_flag - Used to determine if a resp should go to host
 *                            Values:
 *                               SEND_UNCONDITIONALLY = Always send
 *                               SEND_IF_NOT_REVERSAL = Do not send reversals
 *                Auth_Tx - (Global) Transaction information
 *
 *                NOTE: The purpose of the send_flag is because reversal
 *                responses are sent to the host as soon as the request
 *                is received from the host.  So as processing continues,
 *                if there are any error conditions, we do not want to
 *                send a second response back to the host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void send_response_to_host( INT send_flag )
{
   CHAR  err_buf[100]="";
   BYTE  temp_tx_key;

   TranFlow = SWITCHED_IN;
   if ( (send_flag == SEND_UNCONDITIONALLY) ||
       ((send_flag == SEND_IF_NOT_REVERSAL) && (false == is_reversal() )) )
   {
      if ( is_reversal() )
         temp_tx_key = Auth_Tx.TLF01_details.tx_key;

      /* Make sure the tx_key is for a response. */
      if ( Auth_Tx.TLF01_details.tx_key < 100 )
         Auth_Tx.TLF01_details.tx_key += 100;

      if ( true == build_response_msg() )
      {
         if ( false == send_message_to_host(err_buf,false) )//praneeth added false as parameter
            nceqit_log_message(1, 2, err_buf, "send_response_to_host",1 );
      }

      (void)insert_db_record( TLF01_DATA );

      if ( is_reversal() )
         Auth_Tx.TLF01_details.tx_key = temp_tx_key;
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         SEND_REQUEST_TO_HOST
 *
 *  DESCRIPTION:  This function sends a request to the host.  It gets the
 *                information it needs from Auth_Tx and Ncf01.
 *
 *  INPUTS:       timer_resp_flag - Used to control flow upon receipt of timer
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void send_request_to_host( pCHAR timer_resp_flag )
{
   INT   ret_val;
   INT   seconds;
   CHAR  err_buf[100]  ="";
   CHAR  lookup_key[LOOKUP_KEY_SIZE]="";

   TranFlow = SWITCHED_OUT;
   Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
   ret_val = build_request_msg( err_buf );
   if ( true == ret_val )
   {
      ptetime_get_strtime( Auth_Tx.TLF01_details.host_start_time );

      if (true == nceqit_check_if_Visa_transaction())
      {
    	  create_request_timer_key_for_Visa(lookup_key);
      }
      else
      {
    	  /* Set the request timer. */
    	  (void)create_request_timer_key( lookup_key );
      }
      seconds = atoi(Ncf01_I.request_timer);
      strcpy( TimerAppData2, timer_resp_flag );
      ret_val = set_timer( lookup_key, seconds, REQUEST_TIMER );
      if ( true == ret_val )
      {
         /* Send the transaction request to the host. */
         ret_val = send_message_to_host( err_buf ,false);//praneeth added false as parameter
      }
      else
      {
         sprintf( err_buf,
                 "Cannot process %s txn request to host. Unable to set timer.",
                  Auth_Tx.TLF01_details.message_type );
      }
   }

   if ( false == ret_val )
   {
      /* Since the txn request did not go to the
       * host, we need to initiate a response.
       */
      nceqit_log_message( 1, 2, err_buf, "send_request_to_host" ,1);
      ret_val = perform_standin_processing( SYSTEM_ERROR,
                                            GS_TXN_DROPPED,
                                            err_buf );
      if ( false == ret_val )
         nceqit_log_message( 1, 2, err_buf, "send_request_to_host" ,1);
   }

   return;
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG_TO_AUTHORIZER
 *
 *  DESCRIPTION:  This function sends a PTE message to the authorizer.  It
 *                will be either a request or a response.
 *
 *  INPUTS:       msgtype - Either MT_AUTH_REQUEST or MT_AUTH_RESPONSE
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_msg_to_authorizer( BYTE msgtype )
{
   INT   ret_val;
   CHAR  err_buf[100]="";
   CHAR  Buffer[200];

   /* added chargeback and charge back reversals to send directly to ncvsms - Girija Y - TF */
   if( (Auth_Tx.TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT        ) ||
	    (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT      ) ||
	    (Auth_Tx.TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER    ) ||
	    (Auth_Tx.TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ACQUIRER) ||
	    (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT              ) ||
		(Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK_RESPONSE) ||
		(Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL_RESPONSE))
   {
      /* Send directly to Ncvsms */
      ret_val = build_and_send_request_message( msgtype,
                                                0,
                                                0,
                                         (pBYTE)visasms_que_name,
                                         (pBYTE)&Auth_Tx,
                                                Auth_Tx_Size,
                                                0,
                                                NULL_STR,
                                                err_buf   );
      if ( ret_val == false )
      {
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Unable to forward txn to visasms, %s.  Error = %s",
                  visasms_que_name, err_buf );
         nceqit_log_message( 1, 2, Buffer, "send_msg_to_authorizer",1 );
      }
   }
   else
   {
      ret_val = build_and_send_request_message( msgtype,
                                                0,
                                                0,
                                         (pBYTE)authorizer_que_name,
                                         (pBYTE)&Auth_Tx,
                                                Auth_Tx_Size,
                                                0,
                                                NULL_STR,
                                                err_buf   );
      if ( ret_val == false )
      {
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Unable to forward txn to authorizer, %s.  Error = %s",
                  authorizer_que_name, err_buf );
         nceqit_log_message( 1, 2, Buffer, "send_msg_to_authorizer" ,1);
      }
   }

   return( ret_val );
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
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;

   /* Verify message size. */
   if ( length < 0 )
      ret_code = false;

   else if ( msgtype == MT_OUTGOING )
      if ( length > MAX_HOST_MSG_SIZE )
         ret_code = false;

   else if ( length > MAX_APPL_MSG_SIZE )
         ret_code = false;

   if ( ret_code == false )
   {
      sprintf( err_msg, "Unreasonable message size: %d bytes", length );

      if ( msgtype == MT_OUTGOING )
      {
         /* Log transaction to trace file.
          * Set first byte of message type to 1 so we can easily find
          * this transaction in the trace file.
          */
         if ( length < 0 )
            buflen = 350;
         write_to_txn_file();
      }
   }
   else
   {
      /* Message size is ok.  Continue to send build and send it out. */
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
         sprintf( err_msg,
                 "Insufficient Mem to build PTE msg. Mem requested: %d",
                  length );
         ret_code = false;
      }
      else
      {
         /* Set originator communications information. */
         if ( 0 != strcmp( NULL_STR, comm_info ) )
            ptemsg_set_orig_comms_info( p_msg, comm_info );

         /* Send message que_name */
         ret_code = send_msg( p_msg, que_name, err_msg );
         free(p_msg);
      }
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG
 *
 *  DESCRIPTION:  This function sends a PTE message to a specified queue.
 *
 *  INPUTS:       p_msg     - Pointer to a PTE_MSG structure
 *                que_name  - Name of the destination queue
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg )
{
   INT      ret_val = true;
   LONG     ret_code;
   CHAR     Buff[256];
   CHAR     pte_err_mesg[200] = "";

   ret_code = pteipc_send( p_msg, que_name );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, pte_err_mesg );
      sprintf(Buff,"pteipc error sending to que: %s. %s",que_name,pte_err_mesg);
      memcpy( error_msg, Buff, 99 );
      ret_val = false;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         init_Auth_Tx
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *
 *  INPUTS:       Auth_Tx - (Global)Data structure containing transaction info.
 *
 *  OUTPUTS:      Auth_Tx - (Global)Structure is updated with new information.
 *
 *  RTRN VALUE:   False if invalid transaction key (tx_key), else true
 *
 *  AUTHOR:       S. Yarlagadda
 *
 ******************************************************************************/
INT init_Auth_Tx(void)
{
   INT	  ret_val;
   CHAR   unique_tran_str_id[20] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT	   tid_len = 0;
   INT	   unique_tran_str_id_len=0;

   /* Get local time. */
   if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
		ptetime_get_timestamp( time_date );
		get_date( time_date, date_str );
		get_time( time_date, time_str );
		strcpy( Auth_Tx.TLF01_details.date_yyyymmdd, date_str );
		strcpy( Auth_Tx.TLF01_details.time_hhmmss,	 time_str );
   }

   /* Create a unique transaction id for the new auth_tx record. */
   ptetime_get_strtime( unique_tran_str_id );
   memset( Auth_Tx.TLF01_details.primary_key.transaction_id, 0,
		   sizeof(Auth_Tx.TLF01_details.primary_key.transaction_id) );
   tid_len= strlen(Auth_Tx.TLF01_details.terminal_id);
   if(tid_len == 8)
   {
   memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
		   Auth_Tx.TLF01_details.terminal_id,
		   sizeof( Auth_Tx.TLF01_details.terminal_id) - 1 );
   strncat( Auth_Tx.TLF01_details.primary_key.transaction_id,
			unique_tran_str_id+2, 12 );
			Auth_Tx.TLF01_details.primary_key.transaction_id[20]=0x00;
   }
   else
   {
		/* We have to now prepare 20 digit unique Transaction ID.
		  unique_tran_str_id will be having 14 digit length
		  now we will make it as 20 by adding YYMMDD in the beginning of it.
		  20 byte will look like
		  YYMMDD"unique_tran_str_id"
		  for example if unique_tran_str_id is "1444752896.334" then it will become
		  YYMMDD1444752896.334
		  Based on ther terminal length we will copy data from new unique_tran_str_id
		*/
		strncpy(temp_unique_tran,unique_tran_str_id,14);
		memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));
	   
		strncpy(unique_tran_str_id,
				 Auth_Tx.TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
		strncat (unique_tran_str_id,temp_unique_tran,14);
		
		unique_tran_str_id_len= strlen(unique_tran_str_id);
		memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
				Auth_Tx.TLF01_details.terminal_id, tid_len );
		strncat( Auth_Tx.TLF01_details.primary_key.transaction_id,
				 unique_tran_str_id+tid_len, 
				 (unique_tran_str_id_len-tid_len) );
		Auth_Tx.TLF01_details.primary_key.transaction_id[20]=0x00;
   
	}

   /* Initialize some of the auth_tx fields. */
   Auth_Tx.TLF01_details.general_status 	 =	GS_TXN_NEW;
   Auth_Tx.TLF01_details.entry_type 		 =	FINANCIAL;
   strcpy( Auth_Tx.TLF01_details.dispensation.origin, "RO" );

   strcpy( Auth_Tx.TLF01_details.handler_queue, 	  application_que_name);
   strncpy( Auth_Tx.TLF01_details.tran_start_time,	  unique_tran_str_id,16  );

   Auth_Tx.TLF01_details.verify_mag_stripe[0] = 'Y';

   /* Set issuer Id for reporting purposes. */
   strncpy( Auth_Tx.TLF01_details.issuer_id, AppName,
			sizeof(Auth_Tx.TLF01_details.issuer_id) );

   /* Category code - hardcode to "6011" */
   strcpy( Auth_Tx.TLF01_details.category_code, "6011" );
	  
   /* Industry code */
   strcpy( Auth_Tx.industry_code, ATM_INDUSTRY_CODE );

   /* Create RRN */
   if ( Auth_Tx.TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
	  create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
   }

   /* Populate the source encryption key and pin format from the NCF01 record */
   memcpy( Auth_Tx.TLF01_details.source_key, Ncf01_I.cwk, sizeof(Ncf01_I.cwk) );
   strcpy( Auth_Tx.TLF01_details.source_pin_format, Ncf01_I.pin_type );

   /* Determine Transaction Type */
   ret_val = determine_tx_key();

   /* store acquiring id for TLF01 */
	  strcpy(Auth_Tx.TLF01_details.acquiring_id,Auth_Tx.acq_inst_id_cd);

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         DETERMINE_TX_KEY
 *
 *  DESCRIPTION:  This function determines the tx_key based on message type
 *                and processing code.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if unable to determine tx_key, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT determine_tx_key( )
{
   INT   ret_val = true;
   CHAR  err_buf[100];
   CHAR  msgtype[5] = "";
   CHAR  procode[3] = "";
   CHAR  poscc[3]   = "";

   memcpy( msgtype, Auth_Tx.TLF01_details.message_type,       4 );
   memcpy( procode, Auth_Tx.TLF01_details.processing_code,    2 );
   memcpy( poscc,   Auth_Tx.TLF01_details.pos_condition_code, 2 );

   /* 02xx */
   if ( msgtype[1] == '2' )
   {
      /* 020x */
      if ( msgtype[2] == '0' )
      {
         /* Request */
         if ( msgtype[3] == '0' )
         {
            /* 0200 */
            if ( procode[0] == '2' )
               Auth_Tx.TLF01_details.tx_key = AUTH_REFUND;
            else if ( procode[1] == '1' )
               Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
            else if ( procode[0] == '3' )
               Auth_Tx.TLF01_details.tx_key = AUTH_BALANCE_INQUIRY;
            else if ( procode[1] == '0' )
               Auth_Tx.TLF01_details.tx_key = AUTH_SALE;
            else
               ret_val = false;
         }
         else if ( msgtype[3] == '1' )
            Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_REPEAT;

         else if ( msgtype[3] == '2' )
            Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_CONFIRM;

         else
            ret_val = false;
      }

      /* 0220 */
      else if ( msgtype[2] == '2' )
      {
         if ( 0 == strcmp(procode,"19") )
            Auth_Tx.TLF01_details.tx_key = AUTH_FEE_COLLECTION_ACQUIRER;
         else if ( 0 == strcmp(procode,"29") )
            Auth_Tx.TLF01_details.tx_key = AUTH_FUNDS_DISBURSEMENT_ACQUIRER;

         else if (strncmp(poscc,"00",2) == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_BACKOFFICE_ADJUSTMENT;
         else if (strncmp(poscc,"13",2) == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_REPRESENTMENT;
         else
            ret_val = false;
      }
      else
      {
         /* Response */
         if ( procode[0] == '2' )
         {
        	if(procode[1] == '8')
        	{
        		Auth_Tx.TLF01_details.tx_key = AUTH_MONEY_SEND_RESPONSE;
        	}
        	else if ( msgtype[3] == '1' )
               Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_REPEAT_RESPONSE;

            else if ( msgtype[3] == '2' )
               Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_CONFIRM_RESPONSE;

            else if ( msgtype[3] == '0' )
               Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_RESPONSE;

            else
               ret_val = false;
         }
         else if ( procode[1] == '1' )
            Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
         else if ( procode[0] == '3' )
            Auth_Tx.TLF01_details.tx_key = AUTH_BALANCE_INQUIRY_RESPONSE;
         else if ( procode[1] == '0' )
            Auth_Tx.TLF01_details.tx_key = AUTH_SALE_RESPONSE;
         else
            ret_val = false;
      }
   }

   /* 01xx */
   else if ( msgtype[1] == '1' )
   {
      if ( msgtype[2]=='0' && msgtype[3]=='2' )
         Auth_Tx.TLF01_details.tx_key = AUTH_ATM_CONFIRM;

      else if ( msgtype[2] == '0' )
      {
         /* Request */
         if ( procode[0] == '3' )
            Auth_Tx.TLF01_details.tx_key = AUTH_BALANCE_INQUIRY;
         else
            ret_val = false;
      }
      else if(msgtype[2] == '3')
      {
    	  Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
      }
      else
      {
         /* Response */
         if ( procode[0] == '3' )
            Auth_Tx.TLF01_details.tx_key = AUTH_BALANCE_INQUIRY_RESPONSE;
         else
            ret_val = false;
      }
   }

   /* 04xx */
   else if ( msgtype[1] == '4' )
   {
      if ( msgtype[2] == '2' )
      {
         /* Request */
         if ( procode[0] == '2' )
            Auth_Tx.TLF01_details.tx_key = AUTH_VOID_RELOAD;
         else
            Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
      }
      else
      {
         /* Response */
         if ( procode[0] == '2' )
            Auth_Tx.TLF01_details.tx_key = AUTH_VOID_RELOAD_RESPONSE;
         else
            Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
      }
   }

   /* 08xx */
   else if ( msgtype[1] == '8' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
      }
      else if ( msgtype[2] == '1' )
      {
         /* Response */
         Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
      }
      else
         ret_val = false;
   }
   /* 06xx */
   else if ( msgtype[1] == '6' )
   {
      if ( (msgtype[2] == '0' ) || (msgtype[2] == '2' ))
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_ADMINISTRATIVE_TEXT;
      }
      else if (( msgtype[2] == '1') || ( msgtype[2] == '3'))
      {
         /* Response */
         Auth_Tx.TLF01_details.tx_key = AUTH_ADMINISTRATIVE_TEXT_RESPONSE;
      }
      else
         ret_val = false;
   }


   /* Is it a valid tx_key? */
   if ( ret_val == false )
   {
      if ( TranFlow == SWITCHED_IN )
      {
         sprintf( err_buf,
              "Unsupported txn from host: msgtype = %s, processing code = %s",
               msgtype, Auth_Tx.TLF01_details.processing_code );
      }
      else
      {
         sprintf( err_buf,
              "Unsupported txn from ATP: msgtype = %s, processing code = %s",
               msgtype, Auth_Tx.TLF01_details.processing_code );
      }
      nceqit_log_message( 1, 2, err_buf, "determine_tx_key" ,1);
      Auth_Tx.TLF01_details.tx_key = AUTH_INVALID;
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SET_TX_KEY_TO_RESPONSE
 *
 *  DESCRIPTION:  This procedure will return a response transaction type
 *                based on a request transaction type.  For example, a
 *                transaction request of AUTH_SALE will return a response
 *                type of AUTH_SALE_RESPONSE.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction type of request transaction
 *
 *  OUTPUTS:      Auth_Tx - (Global) Transaction type is updated to response
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void set_tx_key_to_response()
{
   /* Make sure this is a request.  If it is not, then it is a response
    * and we need to make sure we return what was sent in.   Note: Anything
    * greater than AUTH_INVALID is a response, not a request.
    */
   if ( Auth_Tx.TLF01_details.tx_key < AUTH_INVALID )
      Auth_Tx.TLF01_details.tx_key += 100;

   return;
}

INT RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
      INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
   return ret_val;
} /* end RspMovevData */
static  BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{
   INT            i;
   INT    found = 0;
   BYTE  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           !     "     #     $     %     &     '  |
        |   (     )     *     +     ,     -     .     /     :     ;  |
        |   <     =     >     ?     @     \     _     `     {     |  |
        |   }     ~                                                  |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
         0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61, 0x7A, 0x5E,
         0x4C, 0x7E, 0x6E, 0x6F, 0x7C, 0xE0, 0x6D, 0x79, 0xC0, 0x4F,
         0xD0, 0xA1, 0x5F
      };

   static unsigned char ascii_chart[] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
         0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x3A, 0x3B,
         0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x5C, 0x5F, 0x60, 0x7B, 0x7C,
         0x7D, 0x7E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      //the next line is modified by farad 8/13/98
	  for( i=0; i<93 && !found; i++ )
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
      //the next line is modified by farad 8/13/98
      for( i=0; i<93 && !found; i++ )
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
}   //end of the ascii_ebcdic_conv

 
INT vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
      INT                  ret_val = true;
	BYTE temp_len[3];
   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if(fieldid == 104)
	  {memcpy(temp_len,&pBF->p_bd_pdata [0],3);
		movelen = atoi(temp_len);
		//sprintf(temp_str,"%2x",movelen);
      ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv(temp_len [0],A2E);
      reqidx ++;
	  ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv(temp_len [1],A2E);
	   //ReqRsp_Buffer [reqidx] = temp_str[1];
	  reqidx ++;
      ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv(temp_len [2],A2E);
      reqidx ++;

	  memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[3], movelen);
	  }	
	  else
	  {movelen = (pBF->p_bd_pdata [0]);
	   ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
      reqidx++;
	    memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);
	  }
     // memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
   return ret_val;
} /* end vDataMove */

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
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

      convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
      reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

} /* end vEBCDICMove */


INT vEBCDICMove_F126( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

	  convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
	  if (ReqRsp_Buffer[reqidx + pBF->bd_fieldlen] == 0)
	  {
		  ReqRsp_Buffer[reqidx + pBF->bd_fieldlen] = 0xF0;
	  }
	  reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

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
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
      reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

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
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi(temp_string_len) + FieldLen;
	/*Temp fix for DE44 length, IST has to revise repsone format, Now it is coming as FIXED 5 byte length,
		Expected is LLVAR- Phani*/
		if(fieldid == bf_44respdata)
			movelen= 5;
      if ( fieldid == bf_126priv126 &&
    		  (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         //convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         memcpy(pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen);
         reqidx += movelen;
      }
      else if ( (reqidx + movelen) <= buflen)
      {
          /* Move the data, converting it from EBCDIC to ASCII. */
          convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
          reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICvMove */

INT RspEBCDICvMove_104 (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi(temp_string_len);

     // memcpy(pBF->p_bd_pdata, temp_string_len, FieldLen);
      sprintf(pBF->p_bd_pdata,"%03d",movelen);
      reqidx +=FieldLen;
      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
        // convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );

         memcpy(&pBF->p_bd_pdata[3], &ReqRsp_Buffer[reqidx], movelen);
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICvMove */

/******************************************************************************
 *
 *  NAME:         RspEBCDICvMove_44
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into ASCII.  there was many chnages for DE44 hence wrote separate function.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Raghu
 *
 ******************************************************************************/
INT RspEBCDICvMove_44 (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi(temp_string_len) + FieldLen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
    	  /* As length not available store parsed value in data part. hence +2 for pBF->p_bd_pdata */
         convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

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
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

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
         convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICMove */


/***********************************************
   GenericMove
************************************************/
INT GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;

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
   else
      ret_val = false;

   return( ret_val );

} /* end GenericMove */



/***********************************************
   RspMoveGeneric
************************************************/
INT RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
      {
         memcpy(pBF->p_bd_pdata,               /* to data pointer          */
                &ReqRsp_Buffer[reqidx],        /* move from request buffer */
                pBF->bd_fieldlen);             /* for length of field      */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspMoveGeneric */




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
 *  NAME:         Move_IfThere_44
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
 *  AUTHOR:       Raghu
 *
 ******************************************************************************/
INT Move_IfThere_44( enum bitfields fieldid, BYTE p_trantype )
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
         if ( (pBF->p_bd_pdata [i] != 0))
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
int get_variable_length( unsigned char *DataField, int FieldLen )
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
struct trandef *Find_Tran_Table( pAUTH_TX p_auth_tx )
{
   INT  i;            /* loop index  */
   BYTE trantype;
   CHAR Buffer[100];

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
   sprintf( Buffer, "This trantype, %d, not found in TRANTABLE", trantype );
   nceqit_log_message( 1, 2, Buffer, "FIND_TRAN_TABLE",0 );

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
 ******************************************************************************/
struct bitfield_data *Find_Bitfield_Table( enum bitfields bitfield )
{
   INT      i;                    /* loop index */
   CHAR     temp_str[60];

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */

   /* Field not found in table - log error and return NULL */
   sprintf(temp_str, "This bitfield not found in BFData, bitfield: %d",
                      bitfield);
   nceqit_log_message( 1, 2, temp_str, "Find_Bitfield_Table",0 );
   return NULL;

} /* end Find_Bitfield_Table */




/******************************************************************************
 *
 *  NAME:         BUILD_HOST_MSG
 *
 *  DESCRIPTION:  This procedure copies the information from the HOST_MESSAGE
 *                structure into a character string.  The character string is
 *                then shipped to the host.
 *
 *  INPUTS:       Auth_Tx      - (Global)Transaction record
 *                HOST_MESSAGE - (Global)Transaction data destined for the host
 *
 *  OUTPUTS:      err_buf - Text message in the event of an error
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_host_msg( pCHAR err_buf )
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;     /* index to bitfields for this transaction */
   INT                  bitMapsize;
   BYTE                 ReqRsp_Buffer_bitmap_hex[17]="";
   BYTE                 ReqRsp_Buffer_bitmap_ebcdic[33]="";

   /* Get pointer to transaction table */
   pTranDef = Find_Tran_Table( &Auth_Tx );

   if ( NULL == pTranDef )
   {
      if ( TranFlow == SWITCHED_OUT )
      {
         sprintf( err_buf,
              "Request message %s, processing code %s, to host not supported.",
               Auth_Tx.TLF01_details.message_type,
               Auth_Tx.TLF01_details.processing_code);
      }
      else
      {
         sprintf( err_buf,
              "Response message %s, processing code %s, to host not supported.",
               Auth_Tx.TLF01_details.message_type,
               Auth_Tx.TLF01_details.processing_code);
      }
      return false; /* Did not find TX type - so quit */
   }

   /*-----------------------------------------*/
   /* Copy in Message Id for this transaction */
   /*-----------------------------------------*/
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   bitmapidx = 4;                            /* point to bitmap     */

   /* Point past bitmap   */
   if ( Network == NCMLNK )
   {
      /* For MegaLink, always use 2 bitmaps, even if 2nd one is not used. */
      reqidx = 36;
      bitMapsize = 16;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if ( TranFlow == SWITCHED_IN )
   {
      if ((AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE       == Auth_Tx.TLF01_details.tx_key) ||
          (AUTH_REPRESENTMENT_RESPONSE               == Auth_Tx.TLF01_details.tx_key) ||
          (AUTH_BACKOFFICE_ADJUSTMENT                == Auth_Tx.TLF01_details.tx_key) ||
          (AUTH_REPRESENTMENT                        == Auth_Tx.TLF01_details.tx_key) ||
          (AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE     == Auth_Tx.TLF01_details.tx_key) ||
          (AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE == Auth_Tx.TLF01_details.tx_key)  )
      {
          reqidx = 20;
          bitMapsize = 8;
          BitFieldSize = NORMAL_BITFIELD;
      }
      else
      {
         /* Everything going to the host, all responses, have 2 bit maps. */
         reqidx = 36;
         bitMapsize = 16;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;
         BitFieldSize = EXTENDED_BITFIELD;
      }
   }
   else if ( ( AUTH_LOGON == Auth_Tx.TLF01_details.tx_key ) ||
	         ( AUTH_CHARGEBACK == Auth_Tx.TLF01_details.tx_key ) ||
			 ( AUTH_CHARGEBACK_REVERSAL == Auth_Tx.TLF01_details.tx_key ) ||
			 ( AUTH_OCT_TX == Auth_Tx.TLF01_details.tx_key ) ||
	         ( AUTH_FAST_FUND == Auth_Tx.TLF01_details.tx_key )||
	         second_Bitmap == true )
   {
      reqidx = 36;
      bitMapsize =16;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   
   else if ( is_reversal() )
   {
      /* All reversal type messages have 2 bit maps. */
      reqidx = 36;
      bitMapsize = 16;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      /* A single bit map */
     // reqidx = 20;
     // bitMapsize = 8;
     // BitFieldSize = NORMAL_BITFIELD;
    // printf("\n I am in Last Else \n");
	  reqidx = 36;
	  bitMapsize =16;
    	ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
			BitFieldSize = EXTENDED_BITFIELD;

   }

   memcpy(ReqRsp_Buffer, pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

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
            sprintf( err_buf,
                    "Unable to find bitfield (%d) to build %s msg for host.",
                     bfidx, Auth_Tx.TLF01_details.message_type );
            return false; /* Invalid bit was set - so quit. */
         }

         /*-----------------------------------------------------------*/
         /* if conditional routine returns true, move the bitfield... */
         /*-----------------------------------------------------------*/
         if ((pBF->bd_BFCond)(pBF->bd_fieldid,Auth_Tx.TLF01_details.tx_key) == true)
         {
            /*------------------------------*/
            /* set the bit in the bitmap... */
            /*------------------------------*/
            ReqRsp_Buffer[bitmapidx +BITTAB[pTranDef->td_bitfields[bfidx]].byte_no] |=
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

   /* converting bitmap from hex to ebcdic */
  	
   hex_to_ebcdic(&ReqRsp_Buffer[4],ReqRsp_Buffer_bitmap_ebcdic,bitMapsize);
   memcpy(&ReqRsp_Buffer[4],ReqRsp_Buffer_bitmap_ebcdic,(2*bitMapsize));
   buflen = reqidx;
   return( true );
}

/******************************************************************************
 *
 *  NAME:         BUILD_HOST_STRUCTURE
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                a request or response message destined for the host.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_host_structure( pCHAR err_buf )
{
   INT     ret_val = true;
   INT     result_code = 0;  /* 0 = no error, 1 = error */
   

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   if ( TranFlow == SWITCHED_IN )
   {
      if ( Auth_Tx.TLF01_details.message_type[1] == '2' )
      {
         if( (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE) ||
             (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) )
         {
            /* Build fields necessary for 02nn RESPONSE messages. */
            (void)build_response_field_2(  &result_code, err_buf );
            (void)build_response_field_3(  &result_code, err_buf );
            (void)build_response_field_4(  &result_code, err_buf );
            (void)build_response_field_5(  &result_code, err_buf );
            (void)build_response_field_7(  &result_code, err_buf );
            (void)build_response_field_9(  &result_code, err_buf );
            (void)build_response_field_11( &result_code, err_buf );
            (void)build_response_field_15( &result_code, err_buf );
            (void)build_response_field_16( &result_code, err_buf );
            (void)build_response_field_19( &result_code, err_buf );
            (void)build_response_field_20( &result_code, err_buf );
            (void)build_response_field_25( &result_code, err_buf );
            (void)build_response_field_32( &result_code, err_buf );
            (void)build_response_field_37( &result_code, err_buf );
            (void)build_response_field_39( &result_code, err_buf );
            (void)build_response_field_41( &result_code, err_buf );
            (void)build_response_field_42( &result_code, err_buf );
            (void)build_response_field_44( &result_code, err_buf );
            (void)build_SMS_response_field_48( &result_code, err_buf );
            (void)build_response_field_62( &result_code, err_buf );
            (void)build_response_field_63( &result_code, err_buf );
         }
         else if((Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT_RESPONSE) ||
                 (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT))
         {
            /* Build fields necessary for 02nn RESPONSE messages. */
            (void)build_response_field_2(  &result_code, err_buf );
            (void)build_response_field_3(  &result_code, err_buf );
            (void)build_response_field_4(  &result_code, err_buf );
            (void)build_response_field_7(  &result_code, err_buf );
            (void)build_response_field_9(  &result_code, err_buf );
            (void)build_response_field_11( &result_code, err_buf );
            (void)build_response_field_15( &result_code, err_buf );
            (void)build_response_field_16( &result_code, err_buf );
            (void)build_response_field_19( &result_code, err_buf );
            (void)build_response_field_20( &result_code, err_buf );
            (void)build_response_field_25( &result_code, err_buf );
            (void)build_response_field_32( &result_code, err_buf );
            (void)build_response_field_37( &result_code, err_buf );
            (void)build_response_field_39( &result_code, err_buf );
            (void)build_response_field_41( &result_code, err_buf );
            (void)build_response_field_42( &result_code, err_buf );
            (void)build_SMS_response_field_48(  &result_code, err_buf );
            (void)build_response_field_62( &result_code, err_buf );
            (void)build_response_field_63( &result_code, err_buf );
         }
         else if((Auth_Tx.TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE) ||
                 (Auth_Tx.TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE))
         {
            /* Build fields necessary for 0230 RESPONSE messages. */
            if ( 0 < strlen(Auth_Tx.TLF01_details.card_num) )
               (void)build_response_field_2(  &result_code, err_buf );

            (void)build_response_field_3(  &result_code, err_buf );
            (void)build_response_field_7(  &result_code, err_buf );
            (void)build_response_field_11( &result_code, err_buf );
            (void)build_response_field_15( &result_code, err_buf );
            (void)build_response_field_19( &result_code, err_buf );
			(void)build_response_field_23( &result_code, err_buf );
            (void)build_response_field_32( &result_code, err_buf );
            (void)build_response_field_37( &result_code, err_buf );
            (void)build_response_field_39( &result_code, err_buf );
			(void)build_response_field_55( &result_code, err_buf );
            (void)build_response_field_63( &result_code, err_buf );
         }
         else
         {
            /* Build fields necessary for 02nn RESPONSE messages. */
            (void)build_response_field_2(   &result_code, err_buf );
            (void)build_response_field_3(   &result_code, err_buf );
            (void)build_response_field_4(   &result_code, err_buf );
            (void)build_response_field_7(   &result_code, err_buf );
            (void)build_response_field_11(  &result_code, err_buf );
            (void)build_response_field_15(  &result_code, err_buf );
			(void)build_response_field_23(  &result_code, err_buf );
            (void)build_response_field_32(  &result_code, err_buf );
			(void)build_response_field_37(  &result_code, err_buf ); /* Girija Y - TF */
            (void)build_response_field_38(  &result_code, err_buf );
            (void)build_response_field_39(  &result_code, err_buf );
            (void)build_response_field_41(  &result_code, err_buf );
            (void)build_response_field_48(  &result_code, err_buf );
            /*(void)build_response_field_49(  &result_code, err_buf );*/
			(void)build_response_field_55(  &result_code, err_buf );
			/*(void)build_response_field_62(  &result_code, err_buf );*/
			(void)build_response_field_63(  &result_code, err_buf ); /* Girija Y, TF */
            (void)build_response_field_102( &result_code, err_buf );
            (void)build_response_field_103( &result_code, err_buf );
			
         }
      }
      else if ( is_reversal() )
      {
         /* Build fields necessary for 04nn reversal RESPONSE messages. */
         (void)build_response_field_2(   &result_code, err_buf );
         (void)build_response_field_3(   &result_code, err_buf );
         (void)build_response_field_4(   &result_code, err_buf );
         (void)build_response_field_7(   &result_code, err_buf );
         (void)build_response_field_11(  &result_code, err_buf );
         (void)build_response_field_15(  &result_code, err_buf );
         (void)build_response_field_32(  &result_code, err_buf );
		 (void)build_response_field_37(  &result_code, err_buf ); /* Girija Y - TF */
         (void)build_response_field_39(  &result_code, err_buf );
         (void)build_response_field_41(  &result_code, err_buf );
		 (void)build_response_field_55(  &result_code, err_buf ); 
         (void)build_response_field_90(  &result_code, err_buf );
         (void)build_response_field_95(  &result_code, err_buf );
         (void)build_response_field_128( &result_code, err_buf );
      }
      else if ( Auth_Tx.TLF01_details.message_type[1] == '6' )
      {
         /* Administrative text message */
         (void)build_response_field_7(   &result_code, err_buf );
         (void)build_response_field_11(  &result_code, err_buf );
         (void)build_response_field_21(  &result_code, err_buf );
         (void)build_response_field_37(  &result_code, err_buf );
         (void)build_response_field_39(  &result_code, err_buf );
         (void)build_SMS_Admin_response_field_48(  &result_code, err_buf );
         (void)build_admin_response_field_63( &result_code, err_buf );
         (void)build_admin_response_field_70( &result_code, err_buf );
         (void)build_response_field_100( &result_code, err_buf );
      }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s).",
                  Auth_Tx.TLF01_details.message_type );
         strcat( err_buf, "  Unable to build response message." );
      }
   }
   else  /* SWITCHED OUT Transactions */
   {
	  if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0422",4) )
	  {
		 /* Build fields necessary for 0422 REQUEST messages - Girija Y, TF */
         (void)build_request_field_2(   &result_code, err_buf );
         (void)build_request_field_3(   &result_code, err_buf );
         (void)build_request_field_4(   &result_code, err_buf );
		 (void)build_request_field_5(   &result_code, err_buf );
         (void)build_request_field_7(   &result_code, err_buf );
		 (void)build_request_field_9(   &result_code, err_buf );
         (void)build_request_field_11(  &result_code, err_buf );
         (void)build_request_field_13(  &result_code, err_buf );
         (void)build_request_field_14( &result_code, err_buf );
         (void)build_request_field_15(  &result_code, err_buf );
		 (void)build_request_field_16(  &result_code, err_buf );
		 (void)build_request_field_18(  &result_code, err_buf );
		 (void)build_request_field_19(  &result_code, err_buf );
         (void)build_request_field_20(  &result_code, err_buf );
		 (void)build_request_field_21(  &result_code, err_buf );
         (void)build_request_field_25(  &result_code, err_buf );
		 (void)build_request_field_28(  &result_code, err_buf );        
         (void)build_request_field_32(  &result_code, err_buf );
         (void)build_request_field_33(  &result_code, err_buf );
         (void)build_request_field_37(  &result_code, err_buf );
         (void)build_request_field_38(  &result_code, err_buf );
         (void)build_request_field_41(  &result_code, err_buf );
         (void)build_request_field_42(  &result_code, err_buf );

		// if (Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL) 
		
			(void)build_request_field_43(  &result_code, err_buf );
		 
	     /* Added function call to pass Fld 48, Girija Y TF, Jan 21, 2009 */
//		 (void)build_request_field_48(  &result_code, err_buf ); /* Currently not supported, Jan 22, 2009, Girija Y  */

		 if (Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK)  
		 {
			(void)build_request_field_62(  &result_code, err_buf );
		 }

		 (void)build_request_field_63(  &result_code, err_buf );
         (void)build_request_field_90(  &result_code, err_buf );
		 
	  }
      else if ( is_reversal() )
      {
         /* Build fields necessary for 04nn reversal REQUEST messages. */
         (void)build_request_field_2(   &result_code, err_buf );
         (void)build_request_field_3(   &result_code, err_buf );
         (void)build_request_field_4(   &result_code, err_buf );
         (void)build_request_field_7(   &result_code, err_buf );
         (void)build_request_field_11(  &result_code, err_buf );
         (void)build_request_field_12(  &result_code, err_buf );
         (void)build_request_field_13(  &result_code, err_buf );
	     (void)build_request_field_14( &result_code, err_buf );
         (void)build_request_field_15(  &result_code, err_buf );
         (void)build_request_field_22(  &result_code, err_buf );
         (void)build_request_field_25(  &result_code, err_buf );
		 (void)build_request_field_32(  &result_code, err_buf );
         (void)build_request_field_35(  &result_code, err_buf );
//         (void)build_request_field_36(  &result_code, err_buf ); /* Girija Y, TF */
		 (void)build_request_field_37( &result_code, err_buf ); /* Girija Y - TF */
         if(Auth_Tx.TLF01_details.auth_number[0] != 0x00)
		 {
        	 (void)build_request_field_38(  &result_code, err_buf );
		 }
         (void)build_request_field_39(  &result_code, err_buf );
         (void)build_request_field_41(  &result_code, err_buf );
         (void)build_request_field_42(  &result_code, err_buf );
         (void)build_request_field_52(  &result_code, err_buf );
		 (void)build_request_field_55( &result_code, err_buf );
 		 (void)build_request_field_60( &result_code, err_buf );
		 (void)build_request_field_62( &result_code, err_buf );
		 (void)build_request_field_63( &result_code, err_buf ); /* Girija Y - TF */
         (void)build_request_field_90(  &result_code, err_buf );
         (void)build_request_field_95(  &result_code, err_buf );
		 if(strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6)== 0)
		 {
		 (void)build_request_field_126(&result_code, err_buf );
		 }
         (void)build_request_field_128( &result_code, err_buf );

	     (void)build_request_field_43( &result_code, err_buf );
         (void)build_request_field_104(&result_code, err_buf );
         (void)build_request_field_115(&result_code, err_buf );

         /* Need to make sure the RRN of a reversal is the
          * same as the RRN of the original transaction.
          * EB needs it logged into TLF01 same as original RRN.
          */
         memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
                 Auth_Tx.TLF01_details.orig_retrieval_ref_num,
                 12 );
      }
      else if ( ((Auth_Tx.TLF01_details.message_type[1] == '2')  ||
                 (Auth_Tx.TLF01_details.message_type[1] == '1'))  )
      {
         /* Build fields necessary for 02nn and 01nn REQUEST messages. */
         (void)build_request_field_2(  &result_code, err_buf );
         (void)build_request_field_3(  &result_code, err_buf );
         (void)build_request_field_4(  &result_code, err_buf );
         (void)build_request_field_7(  &result_code, err_buf );
         (void)build_request_field_11( &result_code, err_buf );
         (void)build_request_field_12( &result_code, err_buf );
         (void)build_request_field_13( &result_code, err_buf );		 
         (void)build_request_field_14( &result_code, err_buf );
         (void)build_request_field_15( &result_code, err_buf );
		 (void)build_request_field_18( &result_code, err_buf );
         (void)build_request_field_22( &result_code, err_buf );
		 (void)build_request_field_23( &result_code, err_buf );
         (void)build_request_field_25( &result_code, err_buf );

         if (  0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncvis3", 6) &&
			   0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4) &&
			   0 != strlen(Auth_Tx.TLF01_details.tran_fee_amount) &&
			   ('1' == Auth_Tx.TLF01_details.product_codes[13].code[0]))
		{
        	 (void)build_request_field_28( &result_code, err_buf );
		}
		 (void)build_request_field_32( &result_code, err_buf );
         (void)build_request_field_35( &result_code, err_buf );
		 (void)build_request_field_37( &result_code, err_buf ); /* Girija Y - TF */
		 (void)build_request_field_39( &result_code, err_buf );
         (void)build_request_field_41( &result_code, err_buf );
         (void)build_request_field_42( &result_code, err_buf );
	     (void)build_request_field_43( &result_code, err_buf );
		 (void)build_request_field_45( &result_code, err_buf );
         (void)build_request_field_48( &result_code, err_buf );
         (void)build_request_field_49( &result_code, err_buf );
         (void)build_request_field_52( &result_code, err_buf );
		 (void)build_request_field_55( &result_code, err_buf );
		 (void)build_request_field_60( &result_code, err_buf ); /* Girija Y - TF */
		 (void)build_request_field_62(  &result_code, err_buf );
		 (void)build_request_field_63( &result_code, err_buf ); /* Girija Y - TF */
         (void)build_request_field_102(&result_code, err_buf );
         (void)build_request_field_103(&result_code, err_buf );
		 if(strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",4)== 0)
		  {
		  (void)build_request_field_44( &result_code, err_buf );
		  (void)build_request_field_126(&result_code, err_buf );
		  }
         (void)build_request_field_128(&result_code, err_buf );
		 if (Auth_Tx.TLF01_details.tx_key == AUTH_OCT_TX ||
			 Auth_Tx.TLF01_details.tx_key == AUTH_FAST_FUND )  
		 {
         (void)build_request_field_104(&result_code, err_buf );
         (void)build_request_field_115(&result_code, err_buf );
		 }
		 if(Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION )
		 {
			 (void)build_request_field_104(&result_code, err_buf );
		 }
      }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s).",
                  Auth_Tx.TLF01_details.message_type );
         strcat( err_buf, "  Unable to build request message." );
      }
	 /* Check if RPS- tx , dont puplate track2- req from Kenneth dt: 6thOct2010 -TF phani*/
	  if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
	  {
			memset( HOST_MESSAGE.track2, 0x00, MAX_DE35_SIZE );
			memset( HOST_MESSAGE.track2_len, 0x00, 2 );
	  }
   }

   /* Flag any errors that occurred while building the fields. */
   if ( result_code == 1 )
      ret_val = false;

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         PARSE_HOST_MSG
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into the HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_host_msg( pCHAR err_buf )
{
   struct bitfield_data *pBF;
   INT                   bfidx;    /* Index to bitfields for this transaction */
   INT                   ret_val = true;
   INT                   BitFieldSize;
   BYTE                  ReqRsp_Buffer_hex[17]="";
   BYTE                  bitmap;

   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   convert_to_ascii( ReqRsp_Buffer, HOST_MESSAGE.msgtype, 4 );
   ebcdic_to_hex(&ReqRsp_Buffer[4],&bitmap, 1 );
     
   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   
   if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 36;                         /* Extended     */
	 
	  ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 32 );
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      reqidx = 20;                         /* Not Extended */
	 
	  ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 16 );
      BitFieldSize = NORMAL_BITFIELD;
   }

  
   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      if ( ReqRsp_Buffer_hex[BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table(bfidx);

         if ( NULL == pBF )
         {
            sprintf( err_buf, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( err_buf, " MsgType = " );
            strncat( err_buf, HOST_MESSAGE.msgtype, 4 );
            ret_val = false; /* Invalid field in message - so quit. */
            break;
         }

         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         if ( false == (ret_val = (pBF->bd_BFRsp)(pBF->bd_fieldid)) )
         {
            if ( true == invalid_msg_format )
            {
               strcpy( err_buf, "Invalid  message format." );
               strcat( err_buf, " MsgType = " );
               strncat( err_buf, HOST_MESSAGE.msgtype, 4 );
               break;
            }
            else
            {
               sprintf( err_buf,
                    "Unable to parse field %d in msg from host, MsgType: %4.4s",
                     bfidx, HOST_MESSAGE.msgtype );
            }
         }
      }
   }
   return( ret_val );
} /* parse_host_msg */  

/******************************************************************************
 *
 *  NAME:         POPULATE_AUTH_TX
 *
 *  DESCRIPTION:  This function transfers information from the HOST_MESSAGE
 *                structure that contains data from the host into Auth_Tx.
 *
 *  INPUTS:       HOST_MESSAGE - (Global)Data from host
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *                Auth_Tx - (Global)Transaction data
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred, such as
 *                a numeric field is not numeric
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT populate_auth_tx( pCHAR err_buf )
{
   INT     ret_val = true;
   INT     result_code = 0;  /* 0 = no error, 1 = error */
   if ( TranFlow == SWITCHED_IN )
   {
      /*----------------------------------*/
      /* This is a request from the host. */
      /*----------------------------------*/

      if ( Auth_Tx.TLF01_details.message_type[1] == '2' )
      {
         if( Auth_Tx.TLF01_details.message_type[2] == '2' )
         {
            /*+------+
              | 0220 |
              +------+*/
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            if ( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"19",2)) ||
                 (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"29",2))  )
            {
                  /* Fee Collection or Funds Disbursement */
                  (void)move_field_4(  &result_code, err_buf );
                  (void)move_field_7(  &result_code, err_buf );
                  (void)move_field_11( &result_code, err_buf );
                  (void)move_field_19( &result_code, err_buf );
                  (void)move_field_21( &result_code, err_buf );
				  (void)move_field_28( &result_code, err_buf );            /* TF-Ajay Field 28 added */
                  (void)move_field_32( &result_code, err_buf );
                  (void)move_field_37( &result_code, err_buf );
                  (void)move_request_field_48( &result_code, err_buf, USAGE_5 );
                  (void)move_field_49( &result_code, err_buf );
                  (void)move_request_field_63(  &result_code, err_buf );
                  (void)move_field_100( &result_code, err_buf );
            }
            else
            {
               (void)move_field_25( &result_code, err_buf );
               if(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"00") == 0)
               {
                  /* Back office adjustment */
                  (void)move_field_4(  &result_code, err_buf );
                  (void)move_field_7(  &result_code, err_buf );
                  (void)move_field_11( &result_code, err_buf );
                  (void)move_field_13( &result_code, err_buf );
                  (void)move_field_18( &result_code, err_buf );
                  (void)move_field_19( &result_code, err_buf );
                  (void)move_field_21( &result_code, err_buf );
                  (void)move_field_22( &result_code, err_buf );
				  (void)move_field_28( &result_code, err_buf );           /* TF-Ajay Field 28 added */
                  (void)move_field_32( &result_code, err_buf );
                  (void)move_field_37( &result_code, err_buf );
                  (void)move_field_41( &result_code, err_buf );
                  (void)move_field_42( &result_code, err_buf );
                  (void)move_field_43( &result_code, err_buf );
                  (void)move_request_field_48(&result_code,err_buf,USAGE_NONE);
                  (void)move_field_49( &result_code, err_buf );
                  (void)move_request_field_62( &result_code, err_buf );
                  (void)move_request_field_63( &result_code, err_buf );
               }
               else if(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"13") == 0)
               {
                  /* Representment */
                  (void)move_field_4(  &result_code, err_buf );
                  (void)move_field_7(  &result_code, err_buf );
                  (void)move_field_11( &result_code, err_buf );
                  (void)move_field_13( &result_code, err_buf );
                  (void)move_field_18( &result_code, err_buf );
                  (void)move_field_19( &result_code, err_buf );
				  (void)move_field_28( &result_code, err_buf );         /* TF-Ajay Field 28 added */
                  (void)move_field_32( &result_code, err_buf );
                  (void)move_field_37( &result_code, err_buf );
                  (void)move_field_41( &result_code, err_buf );
                  (void)move_field_42( &result_code, err_buf );
                  (void)move_field_43( &result_code, err_buf );
                  (void)move_request_field_48(&result_code,err_buf,USAGE_NONE);
                  (void)move_field_49( &result_code, err_buf );
                  (void)move_request_field_62( &result_code, err_buf );
                  (void)move_request_field_63( &result_code, err_buf );
               }
               else
               {
                  /* Unsupported pos_condition_code.  Log an error. */
                  ret_val = false;
                  sprintf( err_buf,
                     "Unsupported adjustment pos condition code (%s) from host.",
                     Auth_Tx.TLF01_details.pos_condition_code );
               }
            }
         }
         else
         {
            /* Move fields of 02nn REQUEST messages. 0200 */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_7(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_13( &result_code, err_buf );
            (void)move_field_15( &result_code, err_buf );
            (void)move_field_22( &result_code, err_buf );
			(void)move_field_23( &result_code, err_buf );
            (void)move_field_25( &result_code, err_buf );
			(void)move_field_28( &result_code, err_buf );       /* TF-Ajay Field 28 added */
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_35( &result_code, err_buf );
			(void)move_field_37( &result_code, err_buf );  /* Girija Y, TF */
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_43( &result_code, err_buf );
            (void)move_request_field_48( &result_code, err_buf, USAGE_NONE );
            (void)move_field_49( &result_code, err_buf );
            (void)move_field_52( &result_code, err_buf );
			 (void)move_field_55( &result_code, err_buf );
			 (void)move_request_field_62( &result_code, err_buf );
			(void)move_request_field_63( &result_code, err_buf ); /* Girija Y - TF */
			if ( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"01",2)) ||
	             (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"30",2))  )
			{
				(void)move_field_126( &result_code, err_buf );
			}
		   }
      }
      else if ( is_reversal() )
      {
         /* Move fields of 04nn reversal REQUEST messages. */
         (void)move_field_2(   &result_code, err_buf );
         (void)move_field_3(   &result_code, err_buf );
         (void)move_field_4(   &result_code, err_buf );
         (void)move_field_7(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_12(  &result_code, err_buf );
         (void)move_field_13(  &result_code, err_buf );
         (void)move_field_15(  &result_code, err_buf );
         (void)move_field_22(  &result_code, err_buf );
         (void)move_field_25(  &result_code, err_buf );
		 (void)move_field_28(  &result_code, err_buf );                   /* TF-Ajay Field 28 added */
         (void)move_field_32(  &result_code, err_buf );
         (void)move_field_35(  &result_code, err_buf );
		 (void)move_field_37(  &result_code, err_buf );  /* Girija Y, TF */
         (void)move_field_38(  &result_code, err_buf );
         (void)move_field_39(  &result_code, err_buf );
         (void)move_field_41(  &result_code, err_buf );
         (void)move_field_42(  &result_code, err_buf );
         (void)move_field_43( &result_code, err_buf );
         (void)move_field_52(  &result_code, err_buf );
         (void)move_field_90(  &result_code, err_buf );
         (void)move_field_95(  &result_code, err_buf );
		if ( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"01",2)) ||
			 (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"30",2))  )
		{
			(void)move_field_126( &result_code, err_buf );
		}
         (void)move_field_128( &result_code, err_buf );

         /* Save the partial reversal amount into a field that does
          * not get modified.
          */
         memcpy( Auth_Tx.TLF01_details.refund_amount,
                 Auth_Tx.TLF01_details.reversal_amount, 12 );

      }
      else if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         /* Move fields of 08nn network REQUEST messages. */
         (void)move_field_7(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_15(  &result_code, err_buf );
         (void)move_request_field_48(  &result_code, err_buf, USAGE_NONE );
         (void)move_field_70(  &result_code, err_buf );
      }
      else if ( Auth_Tx.TLF01_details.message_type[1] == '6' )
      {
         /* Move fields of 08nn network REQUEST messages. */
         (void)move_field_7(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_21(  &result_code, err_buf );
         (void)move_field_37(  &result_code, err_buf );
         (void)move_admin_request_field_48(  &result_code, err_buf );
         (void)move_request_field_63(  &result_code, err_buf );
         (void)move_admin_field_70(  &result_code, err_buf );
         (void)move_field_100( &result_code, err_buf);
      }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s) from host.",
                  Auth_Tx.TLF01_details.message_type );
      }

      /* Store amt here as original amount prior to currency conversion.
       * SCR 565/745
       */
      memcpy( Auth_Tx.TLF01_details.sales_amount,
              Auth_Tx.TLF01_details.total_amount, 12 );

      /* Store currency here as original, prior to currency conversion.
       * SCR 565
       */
      memcpy( Auth_Tx.TLF01_details.nfi_seq_nbr,
              Auth_Tx.TLF01_details.currency_code, 3 );


   }
   else  /* SWITCHED OUT Transactions */
   {
      /*-----------------------------------*/
      /* This is a response from the host. */
      /*-----------------------------------*/

      if ( (Auth_Tx.TLF01_details.message_type[1] == '2') ||
           (Auth_Tx.TLF01_details.message_type[1] == '1')  )
      {
         /* Move fields necessary for 02nn and 01nn RESPONSE messages. */
         (void)move_field_2(  &result_code, err_buf );
         (void)move_field_3(  &result_code, err_buf );
         (void)move_field_4(  &result_code, err_buf );
         (void)move_field_7(  &result_code, err_buf );
         (void)move_field_11( &result_code, err_buf );
         (void)move_field_15( &result_code, err_buf );
         (void)move_field_32( &result_code, err_buf );
         (void)move_field_38( &result_code, err_buf );
         (void)move_field_39( &result_code, err_buf );
         (void)move_field_41( &result_code, err_buf );
		 (void)move_field_44( &result_code, err_buf );   /* Girija Y, TF */
         (void)move_response_field_48( &result_code, err_buf );
         (void)move_field_55( &result_code, err_buf );
		 (void)move_request_field_63( &result_code, err_buf ); /* Girija Y, TF */
         (void)move_field_102(&result_code, err_buf );
         (void)move_field_103(&result_code, err_buf );
         (void)move_field_104(&result_code, err_buf );

      }

	  /* Charge Back response, Girija Y TF */
	  else if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0432",4) )
	  {
		 (void)move_field_2(   &result_code, err_buf );
         (void)move_field_3(   &result_code, err_buf );
         (void)move_field_7(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_15(  &result_code, err_buf );
		 (void)move_field_19(  &result_code, err_buf );
		 (void)move_field_25(  &result_code, err_buf );
         (void)move_field_32(  &result_code, err_buf );
		 (void)move_field_37(  &result_code, err_buf );
         (void)move_field_39(  &result_code, err_buf );
         (void)move_field_41(  &result_code, err_buf );
		 (void)move_field_42(  &result_code, err_buf );
		 (void)move_request_field_63(  &result_code, err_buf );
         (void)move_field_90(  &result_code, err_buf );
  	  }

      else if ( is_reversal() )
      {
         /* Move fields necessary for 04nn reversal RESPONSE messages. */
         (void)move_field_2(   &result_code, err_buf );
         (void)move_field_3(   &result_code, err_buf );
         (void)move_field_4(   &result_code, err_buf );
         (void)move_field_7(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_15(  &result_code, err_buf );
         (void)move_field_32(  &result_code, err_buf );
         (void)move_field_39(  &result_code, err_buf );
         (void)move_field_41(  &result_code, err_buf );
         (void)move_field_90(  &result_code, err_buf );
         (void)move_field_95(  &result_code, err_buf );
         (void)move_field_128( &result_code, err_buf );
      }
      else if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         /* Move fields necessary for 08nn network RESPONSE messages. */
         (void)move_field_7(  &result_code, err_buf );
         (void)move_field_11( &result_code, err_buf );
         (void)move_field_70( &result_code, err_buf );
      }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s) from host.",
                  Auth_Tx.TLF01_details.message_type );
      }
   }

   /* Flag any errors that occurred while building the fields. */
   if ( result_code == 1 )
      ret_val = false;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PARSE_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 data.  It looks for the first
 *                non-numeric character to determine the field separator.
 *                It copies the card number & expiration date into auth_tx.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      Auth_Tx - (Global)Updated with card number and exp date
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_track2()
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   pBYTE  field_separator = NULL_PTR;

   if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = Auth_Tx.TLF01_details.track2;
      for(track2_idx=0;track2_idx <= MAX_DE2_SIZE;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( (track2_idx > MAX_DE2_SIZE) || (*field_separator == 0x00) )
      {
         ret_val = false;
      }
      else
      {
         /* Put Card Number into auth_tx. */
         card_num_len = field_separator - Auth_Tx.TLF01_details.track2;
         strncpy( Auth_Tx.TLF01_details.card_num,
                  Auth_Tx.TLF01_details.track2,
                  card_num_len );

         /* Get the expiration date. */
         memcpy( Auth_Tx.TLF01_details.exp_date, field_separator+1, 4 );
         /*Service code*/
         memcpy( Auth_Tx.TLF01_details.service_code, field_separator+5, 3 );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         REMOVE_TRACK2_END_SENTINEL
 *
 *  DESCRIPTION:  This function checks the last non-space character of the
 *                input string for the End Sentinal character. If it exists
 *                it is changed to a NULL.
 *
 *  INPUTS:       track2 - String containing Track 2 data
 *
 *  OUTPUTS:      track2 - Same string, but without the End Sentinel
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void remove_track2_end_sentinel( pBYTE track2 )
{
   #define  END_SENTINEL  '?'

   INT  len;
	BYTE *pTemp=0x00;
   /* Trim trailing spaces */
   trim_trailing_spaces( track2 );

   len = strlen( track2 );

 	/*change D to = */
	
	if ((pTemp = strchr(track2, 'D')) != 0x00)
		{
			*pTemp = '=';
		}		
   if ( track2[len-1] == END_SENTINEL )
   {
      /* End Sentinel does exist. Change it to NULL. */
      track2[len-1] = 0x00;
   }
   return;
}



/******************************************************************************
 *
 *  NAME:         BUILD_RESPONSE_MSG
 *
 *  DESCRIPTION:  This function calls 2 functions: one to gather the response
 *                data from Auth_Tx or create it and place it into structure
 *                HOST_MESSAGE, then another to take the data from HOST_
 *                MESSAGE and copy it into the data string that will be
 *                going to the host.  This second function will also insert
 *                the message type and create the bit map.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      ReqRsp_Buffer - (Global)Data stream to go to host
 *
 *  RTRN VALUE:   True if no problems, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_msg()
{
   INT   ret_val=true;
   CHAR  err_buf[100]="";

   if ( true == (ret_val = build_host_structure(err_buf)) )
   {
      ret_val = build_host_msg( err_buf );
   }

   /* Log any errors. */
   if ( ret_val == false )
      nceqit_log_message( 1, 2, err_buf, "build_response_message" ,0);

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         BUILD_REQUEST_MSG
 *
 *  DESCRIPTION:  This function calls 2 functions: one to gather the txn
 *                data from Auth_Tx or create it and place it into structure
 *                HOST_MESSAGE, then another to take the data from HOST_
 *                MESSAGE and copy it into the data string that will be
 *                going to the host.  This second function will also insert
 *                the message type and create the bit map.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      err_buf       - Error message
 *                ReqRsp_Buffer - (Global)Data stream to go to host
 *
 *  RTRN VALUE:   True if no problems, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_msg( pCHAR err_buf )
{
   INT   ret_val=true;

   if ( true == (ret_val = build_host_structure(err_buf)) )
   {
      ret_val = build_host_msg( err_buf );
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CREATE_STAN
 *
 *  DESCRIPTION:  This function creates a unique 6-digit system trace audit
 *                number (stan).  It uses the last 6 digits of the time -
 *                number of seconds since 1-1-1970.  This is unique for a
 *                cycle of 11 days.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      stan - Six digit stan
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_stan( pBYTE stan )
{
   INT   len;
   BYTE  time_in_seconds[30]="";

   ptetime_get_strtime( time_in_seconds );
   len = strlen( time_in_seconds );
   if ( len < 10 )
      len = 10;

   /* Time is in format nnnnnnnnn.nnn
    * I want the last 6 n's before the decimal point.
    */
   memcpy( stan, &time_in_seconds[len-10], 6 );
   return;
}

/******************************************************************************
 *
 *  NAME:         nceqit_log_message
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void nceqit_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
  /* if ( dest > 1 )
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
   nceqit_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
   	   if(details == 1)
   	   {
   /* Call function to post the message. */
   		   text_message_len = strlen(text_message);
   		   if(text_message_len > 255)
   		   {
   			   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
   			   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
   				   	   	   	   	   	   	 	 eventlog_flag, msg_type, func,
   											 severity, log_type,
   											 Auth_Tx.TLF01_details.terminal_id,
   											 Auth_Tx.TLF01_details.card_num,
   											 Auth_Tx.TLF01_details.merchant_id );
   		   }
   		   else
   		   {
   			   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
   				   	   	   	   	   	   	 	 eventlog_flag, msg_type, func,
   											 severity, log_type,
   											 Auth_Tx.TLF01_details.terminal_id,
   											 Auth_Tx.TLF01_details.card_num,
   											 Auth_Tx.TLF01_details.merchant_id );
   		   }
   	   }
   		else
   		{
   		   text_message_len = strlen(text_message);
   		   if(text_message_len > 255)
   		   {
   			   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
   			   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
   										 	 eventlog_flag, msg_type, func,
   											 severity, log_type,
   											 NULL, NULL,NULL );
   	     }
   		 else
   		 {
   			 TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
   											 eventlog_flag, msg_type, func,
   											 severity, log_type,
   											 NULL, NULL,NULL );
   		 }
   	}
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         send_network_status_to_monitor
 *
 *  DESCRIPTION:  This procedure will send a message to the monitor to inform
 *                it of a change in host status.
 *
 *  INPUTS:       NetworkType - (Global) Issuer, Acquirer, or BOTH
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 ******************************************************************************/
INT send_network_status_to_monitor()
{

   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   INT         ret_code = true;
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   CHAR        err_msg[100];
   XINT        ReturnCode = 0 ;
   int         nIndex = 0 ;
   INT   ret_val = true;
   /* Initialize structure to be sent to Monitor. */
   memset( &ncf01, 0, sizeof(NCF01) );

   /* Get the current state (online or offline) from shared memory. */
   ret_val=  read_shared_mem_table( net_consec_tmouts_ctr,active_txns_ctr,
                          current_state, err_msg);

   if(true==ret_val)
   {
   /* Get Monitor connection information from shared memory. */
   memset (ConnectionId, 0, sizeof (ConnectionId)) ;
   ReturnCode = ReadMemMonitorTable (ConnectionId) ;
		if (ConnectionId[0] != 0) {
			if (NetworkType == ACQUIRER || NetworkType == BOTH) {
         strcpy( ncf01.primary_key.network_id,    AppName      );
         strcpy( ncf01.primary_key.network_type, "A"           );
         strcpy( ncf01.status,                   current_state );

         ret_code = build_and_send_request_message( MT_SYS_MONITOR,
                                                    ST1_NETWORK_UPDATE,
				ST2_NONE, applnk_que_name, (pBYTE) &ncf01, Ncf01_Size, 0,
						ConnectionId, err_msg);
				if (ret_code == false) {
					nceqit_log_message(1, 2, err_msg,
							"send_network_status_to_monitor", 0);
         }
      }
			if (((NetworkType == ISSUER) || (NetworkType == BOTH))
					&& ret_code) {
				if (NetworkType == BOTH) {
            /* The time to sleep is the same here,
             * just different units for NT vs UNIX.
             * The purpose is to not flood Monitor.
             */
            #ifdef WIN32
               Sleep( 500 );
            #else
               usleep( 500000 );
            #endif
         }

         strcpy( ncf01.primary_key.network_id,    AppName      );
         strcpy( ncf01.primary_key.network_type, "I"           );
         strcpy( ncf01.status,                   current_state );

         ret_code = build_and_send_request_message( MT_SYS_MONITOR,
                                                    ST1_NETWORK_UPDATE,
				ST2_NONE, applnk_que_name, (pBYTE) &ncf01, Ncf01_Size, 0,
						ConnectionId, err_msg);
         if ( ret_code == false )
					nceqit_log_message(1, 2, err_msg,
							"send_network_status_to_monitor", 0);
			}
		} else {
			nceqit_log_message(1, 2,
					"Unable to get connection info for Monitor.",
					"send_network_status_to_monitor", 0);
      }
   }
   else
   {
	   sprintf(err_msg,"Unable to send network status to monitor. Problem with reading shared memory.Shutting down the service");
	   	nceqit_log_message(1, 2, err_msg, "send_network_status_to_monitor", 0);
	   	shut_down();

   }
   return( ret_code );

}  /* send_network_status_to_monitor */

/******************************************************************************
 *
 *  NAME:         SEND_MESSAGE_TO_HOST
 *
 *  DESCRIPTION:  This function sends a message to the host.
 *
 *  INPUTS:       ReqRsp_Buffer   - (Global)Data stream going to the host
 *                buflen          - (Global)Length of ReqRsp_Buffer
 *                Ncf01_I.tpdu_id - (Global)Comm info
 *                dialog_que_name - (Global)Queue name of dialog service
 *
 *  OUTPUTS:      err_buf - Text message if an error occurs
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_message_to_host( pCHAR err_buf ,INT mgt)//praneeth added bool mgt as parameter
{
   INT   ret_val;
   CHAR  buffer[100]="";

   ret_val = build_and_send_request_message( MT_OUTGOING,
                                             ST1_NONE,
                                             ST2_NONE,
                                             dialog_que_name,
                                             ReqRsp_Buffer,
                                             buflen,
                                             0,
                                             Ncf01_I.tpdu_id,
                                             buffer );

   if ( ret_val == false )
   {
      sprintf( err_buf,
              "Unable to send %s msg to host. Err: %s",
               Auth_Tx.TLF01_details.message_type, buffer );
   }
   else
   {
	   if(!mgt)
	   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, &Auth_Tx, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
	   }
	   else
	   {
		   /* Update transaction statistics */
		   update_txn_stats( &Txn_Counts, &Auth_Tx, MGT );
		   
		   /* Log transaction to trace file. */
			write_to_txn_file();
	   }

      /* If a request, need to increment number of txns
       * for which we have not received a response yet.
       */
      if ( Auth_Tx.TLF01_details.tx_key < AUTH_INVALID )
      {
         /* This is a request. */
         if ( false == increment_active_count(err_buf) )
            nceqit_log_message(1, 2, err_buf, "send_message_to_host" ,1);
      }

      /* Cycle the echo timer because traffic went out over the line. */
      if ( false == reset_echo_timer(err_buf) )
         nceqit_log_message(1, 2, err_buf, "send_message_to_host" ,1);
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PIN_BLOCK_EXISTS
 *
 *  DESCRIPTION:  This function determines if a pin block exists.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if pin exists, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT pin_block_exists()
{
   INT  ret_val = true;

   if ( 0 == strlen(Auth_Tx.TLF01_details.pin_block) )
      ret_val = false;

   else if (blank_string(Auth_Tx.TLF01_details.pin_block,
                         sizeof(Auth_Tx.TLF01_details.pin_block)-1) )
      ret_val = false;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PERFORM_ERROR_PROCESSING
 *
 *  DESCRIPTION:  This procedure takes care of transactions that were unable
 *                to get sent to the host due to some type of error.  The
 *                auth_tx structure gets updated in here and then the function
 *                returns.
 *
 *  INPUTS:       Auth_Tx   - (Global)Transaction Info
 *                resp_code - Response code to send to msg initiator
 *
 *  OUTPUTS:      Auth_Tx - (Global)Structure containing the updated txn record
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void perform_error_processing( pCHAR resp_code )   
{
   CHAR       date_str [ 9] = "";
   CHAR       time_str [ 7] = "";
   CHAR       time_date[25] = "";

   /* Get local time. */
   ptetime_get_timestamp( time_date );
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Create RRN and store it in the auth_tx block. */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.retrieval_ref_num) )
   {
      create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
   }

   /* Create response code and store it in the auth_tx block */
   strcpy( Auth_Tx.TLF01_details.response_code, resp_code );

   /* Create date of transaction and store it in the auth_tx table,
    * format "YYYY0M0D".
    */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.date_yyyymmdd) )
      strcpy( Auth_Tx.TLF01_details.date_yyyymmdd, date_str );

   /* Create time of transaction and store it in the auth_tx table,"0H240I0S" */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.time_hhmmss) )
      strcpy( Auth_Tx.TLF01_details.time_hhmmss, time_str );

   Auth_Tx.TLF01_details.general_status = GS_TXN_DROPPED;

   /* Make sure the tx_key is for a response. */
   if ( Auth_Tx.TLF01_details.tx_key < 100 )
      Auth_Tx.TLF01_details.tx_key += 100;

   (void)send_msg_to_authorizer( MT_AUTH_RESPONSE );
   return;
}

/******************************************************************************
 *
 *  NAME:         PERFORM_STANDIN_PROCESSING
 *
 *  DESCRIPTION:  This procedure processes transactions that cannot or will
 *                not go to the host.  This can be offline txns that should
 *                not go to the host.  Or they can be txns that will not go
 *                due to too many timeouts or too many active txns.
 *
 *  INPUTS:       rsp_code   - Response code to place into the auth_tx block
 *                gen_status - Txn status to place into the auth_tx block
 *                Auth_Tx    - (Global)Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err_buf )
{
   INT   ret_val;
   CHAR  time_date[25] = "";

   /* Copy in response code and create an auth number if approved. */
   strcpy( Auth_Tx.TLF01_details.response_code, rsp_code );

   if ( 0 == strcmp(rsp_code, APPROVED) )
   {
      if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
      {
         if ( Auth_Tx.TLF01_details.time_hhmmss[0] != 0x00 )
         {
            strcpy( Auth_Tx.TLF01_details.auth_number,
                    Auth_Tx.TLF01_details.time_hhmmss );
         }
         else
         {
            /* Get local time. */
            ptetime_get_timestamp( time_date );
            get_time( time_date, Auth_Tx.TLF01_details.auth_number );
         }
      }
   }

   /* Change status to completed and store it in the auth_tx block */
   Auth_Tx.TLF01_details.general_status = gen_status;

   /* Set transaction key to be that of a response for the authorizer's sake */
   (void)set_tx_key_to_response();

   /* Send response to the authorizer */
   ret_val = send_msg_to_authorizer( MT_AUTH_RESPONSE );
   if ( false == ret_val )
   {
      sprintf( err_buf,
              "Unable to send response to txcntl. MsgType: %s, Proc Code: %s",
               Auth_Tx.TLF01_details.message_type,
               Auth_Tx.TLF01_details.processing_code );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         VERIFY_HOST_STATUS
 *
 *  DESCRIPTION:  This procedure will check on all the counters and the state
 *                of the host application to determine whether we can send this
 *                request to the host or not.
 *
 *                If maximum number of consecutive timeouts is configured to
 *                zero, ignore the count.  Same for maximum active transactions.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction record
 *
 *  OUTPUTS:      err_buf - Text message in case of errors
 *
 *  RTRN VALUE:   False if host is not online.  Else true.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT  verify_host_status( pCHAR err_buf )
{
   INT   ret_val;

   /* Make sure host is active. */
   ret_val = read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                                    current_state, err_buf );

   if(true==ret_val)
   {
		if (current_state[0] != cONLINE) {
      strcpy( err_buf, "Host Inactive" );
      ret_val = false;
		} else {
      /* Check number of consecutive time outs.
       * If too many, issue a warning.
       */
			if (MaxConsecTimeouts > 0) {
				if (atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts) {
					strcpy(err_buf, "Maximum consecutive timeouts reached.");

            nceqit_log_message( 1, 2, err_buf, "verify_host_status",1 );

            ret_val = false;
            if ( false == set_host_state(OFFLINE,err_buf) )
						nceqit_log_message(1, 3, err_buf, "verify_host_status",
								1);
         }
      }

      /* Check number of active transactions.
       * If too many, issue a warning.
       */
			if ((MaxActiveTxns > 0) && (ret_val == true)) {
				if (atoi(active_txns_ctr) >= MaxActiveTxns) {
            strcpy( err_buf,
                   "Maximum active txns reached. Continuing to process." );
            nceqit_log_message( 1, 2, err_buf, "verify_host_status" ,1);
         }
      }
   }
	}
   else
   {
	   sprintf(err_buf,"Unable to verify host status. Problem with reading shared memory.Shutting down the service");
	   	nceqit_log_message(1, 2, err_buf, "verify_host_status", 0);
	   	shut_down();
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         IS_REVERSAL
 *
 *  DESCRIPTION:  This function determines if a transaction is a reversal.
 *                Note that an ATM Confirm (message type 0102) is treated
 *                like a reversal - a partial reversal from Visa.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if txn is a reversal, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_reversal()
{
   INT   ret_val = false;

   /* Changed code to handle charge back request from ncvsms - Girija Y, TF */
   if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0422",4))
    
   {
		ret_val = false;
   }

   else if ( Auth_Tx.TLF01_details.message_type[1] == '4' )
      ret_val = true;

   else if ((Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM               ) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE      ) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE                 ) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE        ) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE         ) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD               ) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE      )  )
   {
      ret_val = true;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         IS_VOID
 *
 *  DESCRIPTION:  This function determines if a transaction is a void.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if txn is a void, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_void()
{
   INT   ret_val;

   if ((Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE                 ) ||
       (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE        ) ||
       (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE         ) ||
       (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE) ||
       (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD               ) ||
       (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE      )  )
   {
      ret_val = true;
   }
   else
      ret_val = false;

   return( ret_val );
}

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
   return;
}

/******************************************************************************
 *
 *  NAME:         TRIM_TRAILING_SPACES
 *
 *  DESCRIPTION:  This function sets spaces to nulls, starting from the end
 *                of a string stepping backwards to the first non-space char.
 *
 *  INPUTS:       p_string - Character string to be trimmed
 *
 *  OUTPUTS:      p_string - Modified string
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void trim_trailing_spaces( pBYTE p_string )
{
   INT   i;
   INT   len;

   len = strlen( p_string );
   if ( len > 0 )
   {
      for ( i = len-1; (i>=0) && (p_string[i] == ' '); i-- )
         p_string[i] = 0x00;
   }

   return;
}


/*************************************************************************************
       NAME:           Isallzeros(pCHAR astr, INT max_len)

       DESCRIPTION:    This function checks whether the string contains all zeros in it.


       INPUTS: 		   string and its length


       OUTPUTS: 	true : if all the char in the string have value zero or it is null.
					false: If any one char have numeric value
       RETURNS:
	              BOOLEAN value

      AUTHOR:         Raghavendra H R .
      MODIFIED BY:
*************************************************************************************/

BOOLEAN Isallzeros(pCHAR astr , INT max_len)
{
	INT  i           = 0;
	INT int_val          = 0 ;
	BOOLEAN  ret_val = false;
	CHAR temp_str[256]= {0};

	if (max_len == 0)
	{
		return true;
	}
	else
	{
		strncpy(temp_str,astr,max_len);
	}

	if(isnum(temp_str) == true)
	{
		 int_val = atoi(temp_str);
		 if ( int_val == 0)
		 {
			ret_val = true;
		 }
		 else
		 {
			ret_val = false;
		 }
	}
	else
	{
		return true;
	}

	return( ret_val );
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
         nceqit_log_message( 1, 1, err_buf, "process_host_msg",0 );
         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         nceqit_log_message( 1, 1, err_buf, "process_host_msg",0 );

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            nceqit_log_message( 1, 1, err_buf, "process_host_msg",0 );
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         MAP_RESPONSE_CODE
 *
 *  DESCRIPTION:  This function maps a response code from Megalink or the
 *                Equitable host into an ATP (Hypercom ISO) response code.
 *                If a mapping is not found, the response code is unchanged.
 *
 *  INPUTS:       host_rc - Response code from host
 *
 *  OUTPUTS:      atp_rc  - Response code for Hypercom terminal
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void map_response_code( pBYTE host_rc, pBYTE atp_rc )
{
   INT  i;
   INT  found = false;

   /* Here are a couple of exceptions for reversal. */
   if ( is_reversal() )
   {
      if ( (0 == strcmp(host_rc, "22")) ||
           (0 == strcmp(host_rc, "32"))  )
      {
         strcpy( atp_rc, APPROVED );
         found = true;
      }
   }

   if ( found == false )
   {
      /* Search the global host response code structure for a match. */
      for ( i=0; i<NUM_RESP_CODES; i++ )
      {
         if ( 0 == strcmp(Host_Resp_Codes[i], host_rc) )
         {
            found = true;
            break;
         }
      }

      if ( found )
         strcpy( atp_rc, ATP_Resp_Codes[i] );
      else
         strcpy( atp_rc, host_rc );
   }
   return;
}


void nceqit_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  nceqit_error_warning_file_size[5] = {0};


   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   nceqit_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(nceqit_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  nceqit_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(nceqit_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  nceqit_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(nceqit_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(nceqit_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(nceqit_error_warning_file_path) > 0) &&
	  (strlen(nceqit_module_error_warning_file_name)> 0))
   {
	   nceqit_create_Error_Filename();
   }
}

INT nceqit_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail)
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
    len = strlen(nceqit_Erro_warning_Filename);
    path_len = strlen(nceqit_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&nceqit_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	nceqit_create_Error_Filename();
    }
	if((fp = fopen(nceqit_Erro_warning_Filename,"a+b"))==NULL)
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
	if(detail == 1)
	{
		if (strlen(Auth_Tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( Auth_Tx.TLF01_details.card_num );
			strncpy( tempcard, Auth_Tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
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

void nceqit_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( nceqit_Erro_warning_Filename, nceqit_error_warning_file_path );
    strcat( nceqit_Erro_warning_Filename, nceqit_module_error_warning_file_name );
	strcat( nceqit_Erro_warning_Filename, ".EW." );
    strncat(nceqit_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(nceqit_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(nceqit_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

INT nceqit_check_if_Visa_transaction()
{
	if ( Auth_Tx.TLF01_details.card_num[0]=='4' ||
		(0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA)||
		0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_DUAL)))

	{
		return true ;
	}
	else
	{
		return false;
	}


}

