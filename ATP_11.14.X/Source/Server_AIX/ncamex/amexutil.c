/*******************************************************************************
* Copyright (c) 2006, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       amexutil.c
*  
* TITLE:       
*  
*	DESCRIPTION: This module contains all the utilities used by the 
*               host handler to  parse and manipulate all incoming 
*               messages from the terminal and the host.
*
* APPLICATION: 
*
* AUTHOR:       B. Gable
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncamex\amexutil.c  $

	  Rev 1.104 Mar 04, 2009 Girija Y - ThoughtFocus, Bangalore
   Changed the code to pass flds 48 for 1120 msg.

	  Rev 1.103   Sep 05 2008 17:50:00   Girija Y - ThoughtFocus, Bangalore
	  Added the code to pass Field 25 to host.
   Rel 08-11,modified version to 4.4.1.6
   
	  Rev 1.102   Aug 26 2008 13:00:00   Girija Y - ThoughtFocus, Bangalore
   Changed Move_IfThere to vMove_IfThere and EBCDICMove to vEBCDICMove in 
   bitfield_data BFData[] structure declaration. Added code to receive 
   security related control information that is field 53 in move_iso_response_to_auth_struct().
   Rel 08-11,modified version to 4.4.1.5

      Rev 1.101   Jan 11 2006 14:56:18   dirby
   Changed to set the CSC flag to 'Y' only if track data is present.
   This prevents going to racal for manual txns, which do not have
   CSC2.
   SCR 19211
   
      Rev 1.100   Nov 16 2005 16:47:34   dirby
   Added functionality to get currency conversion records from the
   database table CRF01 when unable to get them from shared memory.
   SCR 18025
   
      Rev 1.99   Jan 16 2004 13:34:52   dirby
   Added a line to store Amex Response Code (3-char) into the
   Auth Number field for Online Updates.
   SCR 1200
   
      Rev 1.98   Jan 15 2004 16:40:36   dirby
   Implemented Online Card Updates (1304/1314).
   SCR 1200
   
      Rev 1.97   Feb 05 2003 14:27:56   dirby
   Modified to check to see if we have date/time for DE12 before
   populating it.  If we do not have it, get it using the system time.
   This is so it goes out in 1804 sign-on messages.
   SCR 1009
   
      Rev 1.96   Feb 04 2003 16:15:12   dirby
   Modified to support any non-numeric character as a field
   separator in track 2.
   SCR 1009
   
      Rev 1.95   Dec 13 2002 14:49:10   dirby
   Modified to set network Id and type into NCF30 structure when
   preparing to find original txn for an auth advice.  It was not
   being set; original was never being found; auth advice always
   was being processed (even if original auth had already been
   processed); would then be worked out in settlement.
   This was fixed in conjunction with SCR 845
   
      Rev 1.94   Dec 04 2002 11:01:38   dirby
   Modified to always use system time to populate date_yyyymmdd and time_hhmmss.
   The value from DE12 is stored in auth_tx.local_date_time so it
   can be echoed back in the response.  This is to make the dates
   consistent in TLF01 and NCF30.
   SCR 940
   
      Rev 1.93   Nov 12 2002 13:35:46   dirby
   Added code to not insert record into TLF01 if it has already
   been inserted by the update_all process.
   SCR 845
   
      Rev 1.92   Jul 11 2002 13:48:18   dirby
   Added code to log DE43 to TLF01.
   SCR 775
   
      Rev 1.91   Jul 03 2002 15:41:52   dirby
   Added code to display reversal message on Monitor.
   SCR 621
   
      Rev 1.90.1.0   May 30 2002 20:05:52   dirby
   1.  Changed name of field in TLF01 where original amount and
   partial reversal amount are stored from nfi_start_date and
   nfi_end_date to sales_amount and refund_amount.  These
   fields are large enough to hold the entire amounts; the 'nfi...'
   fields were not.
   2.  Added code to save conversion rate in auth_tx in case txcntl
   overwrites the value in TLF01.  This way it can be stored into
   the database correctly.
   SCR 745
   
      Rev 1.90   Jan 17 2002 15:00:10   dirby
   Added code to log original amount and currency code to TLF01
   before they get modified for currency conversion.     SCR 565
   
      Rev 1.89   Jan 16 2002 15:13:40   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.88   Jan 14 2002 18:08:28   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.87   Nov 16 2001 13:05:22   dirby
   Changed the validity check of RRN to allow spaces.     SCR 543
   
      Rev 1.86   Aug 15 2001 14:24:48   SYARLAGA
   Modified field 32 and field 33 in move_iso_response_to_auth_struct and    build_host_structure functions.
   
      Rev 1.85   Mar 14 2001 14:29:50   dirby
   Removed a 'memset' command from 'move_iso_...' function.
   It was overwriting originator information in auth_tx.
   
      Rev 1.84   Mar 02 2001 10:59:10   dirby
   Moved the 'get_originator_info' out of function 'init_txn'.
   Moved it to the start of 'incoming_host_message'.  This
   way it is there for any error conditions as well as successful
   requests.
   
      Rev 1.83   Feb 06 2001 08:58:44   dirby
   Modified destination queue for 'responses' to the host so that it goes
   to the queue of whichever dialog acquired the request.  So instead of
   going to 'dialogA', it might go to 'hstampos'.
   
      Rev 1.82   Jan 26 2001 14:44:54   dirby
   1.  Modified to use functions 'get and set_originator_info' when interfacing with
   a PTE message concerning originator communications information.
   2.  Removed some log on/off handling code.  This is now done in function
   'process_logon_request'.
   
      Rev 1.81   Sep 07 2000 13:00:36   dirby
   Added a line to initialize a string prior to it being used.
   
      Rev 1.80   Aug 31 2000 16:33:50   dirby
   Added error catching code for function Build_Host_Request.
   
      Rev 1.79   Aug 25 2000 13:56:26   dirby
   Added a path to error out if unable to build a message due
   to an unsupported bit field.
   
      Rev 1.78   Aug 23 2000 12:46:36   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.77   Aug 15 2000 15:07:06   dirby
   Modified log on/off code to be more efficient and display
   accurate messages.
   
      Rev 1.76   Aug 15 2000 11:23:12   dirby
   Made some adjustments so message type can be either
   Amex style or ISO8583.
   
      Rev 1.75   Aug 14 2000 18:55:38   dirby
   Reworked some of the Log ON/OFF logic.  Were getting errors
   when an 1804 request came in from the host, because ncamex
   was not identifying the transaction as a log on or log off.
   
      Rev 1.74   Aug 01 2000 13:41:30   dirby
   Added function 'validate_host_msg' to weed out garbage
   messages that might find their way through the dialog manager.
   
      Rev 1.73   Jul 10 2000 15:53:20   dirby
   Removed functionality that retrieved NCF01 from the database during
   every online transaction.  Also removed some functions that were completely
   commented out.  They have never been used, and never will.
   
      Rev 1.72   Jul 10 2000 15:06:58   dirby
   Changed ISO8583 response code from 12 (Invalid txn) to 30 (format error) for
   Amex response codes of 181 (format error).
   
      Rev 1.71   Jul 10 2000 14:23:42   dirby
   Created function init_txn to initialize auth_tx for switched in txns.
   
      Rev 1.70   May 10 2000 13:53:22   svelaga
   Modified the code : to log the transaction even if the
   processing fails.
   
      Rev 1.69   Apr 18 2000 14:31:34   svelaga
   Added 1814 to the currency code condition, when
   parsing the response from host.
   
      Rev 1.68   Apr 17 2000 17:46:18   svelaga
   Fixed the Equitable issue :
   
   invalid currency code, when logon / logoff request comes in.
   
   
      Rev 1.67   Apr 14 2000 11:23:32   ddabberu
     stan is copied into orig stan if it is null
         acc to new spec fld 56 Optional for pos reversals
       
   
      Rev 1.66   Apr 11 2000 15:52:52   ddabberu
   1. ATP will always receive Bit 5 in US Dollar for
   both ATM (1200) and POS (1100) messages.  Bit 50 will always be 840 in this
   case.  While Bit 4 will always be in the third country currency amount and
   its currency specified in Bit 49.
   
   1-ATP will check if Bit 49 = 608 (Phil Peso), then use Bit 4 amount to
   authorize & debit the cardholder's account record. 
   
   2-If Bit 49 NOT= 608, ATP should use Bit 5 amount, do USD to Peso currency
   conversion, then use converted Peso amount to authorize & debit cardholder's
   account record.
   
      Rev 1.65   Apr 11 2000 11:31:54   ddabberu
    removed the duplication of fld 48 from amextran.c
   
   
      Rev 1.64   Apr 10 2000 14:48:42   ddabberu
   Globals Variables si, tpdu_id defined.
   and checks made based on them.
   
   
      Rev 1.63   Apr 07 2000 15:25:56   ddabberu
   1. Fld 38 changes for pos response.
   2. For 1120 , we respond back with success.
   3. Code Optimization for match_amex_iso_response()
   
      Rev 1.62   Apr 06 2000 15:24:44   ddabberu
   1.changed the network_type from 'A' to 'I' for logon/off.
   2. Added isalphanumSpace()
   3. Responding to the host, if validation of auth_tx fails.
   
   
      Rev 1.61   Apr 05 2000 17:31:56   ddabberu
   responding to the host if we see !608 in fld 49 and
   exp_date is null on manual entry
   
      Rev 1.60   Mar 31 2000 16:37:32   ddabberu
      1.Trk1 data getFldSep() changed.
      2.SignOff, fld48 sent.
      3.ATM error code for exp date changed to 100.
   
   
    
   
      Rev 1.59   Mar 21 2000 10:47:38   ddabberu
    
   
      Rev 1.58   Mar 17 2000 15:39:16   ddabberu
   changed move_iso_to_auth_struct()
   for fld45, moved fld2 below fld45.
   
      Rev 1.57   Mar 16 2000 23:48:48   ddabberu
   minor fix for fld44
   
      Rev 1.56   Mar 16 2000 21:37:48   ddabberu
   modified to support AMEX 99.2 specs
   
      Rev 1.55   Feb 24 2000 15:15:40   ddabberu
   1.fld2 made mandatory for 1210 msg, new amex spec
   2.checking for 1810 msg type while popultaing the auth_Tx response code in the move_iso_to auth_tx () function.
   
   
      Rev 1.54   Feb 11 2000 17:09:02   svelaga
   1. Incorporated the design changes due to
   SCR #374.
   2. Fix for SCR #391.
   
      Rev 1.53   Feb 07 2000 16:20:50   ddabberu
   move_iso_response_to_auth_struct() is modified
   to handle reversal_amount(partial/full)
   
      Rev 1.52   Feb 05 2000 19:27:58   ddabberu
   incoming_other_host_response()
   changed, for the reversals ,recon_update() is
   called to update ncf21,and inserts a dummy record into ncf30
   
      Rev 1.51   Feb 05 2000 18:33:26   ddabberu
   hhutil_process_host_request() is modified
    commented the code which copies stan to orig_stan
   
      Rev 1.50   Jan 07 2000 16:44:50   ddabberu
   1.Changed hhutil_db_ok() to handle auth_advices
   2. Removed unwanted code from recon_update()
   
      Rev 1.49   Jan 04 2000 14:13:48   ddabberu
   modified incoming_other_host_response()
   to handle advices and reversals.
   
      Rev 1.48   Jan 03 2000 15:27:12   ddabberu
   code modifications related to new Auth_advice process
   and AUTHORIZATION .
   
      Rev 1.47   Dec 23 1999 11:19:26   dirby
   Added the primary keys as part of the index
   when deleting a record from NCF30.
   
      Rev 1.46   Dec 22 1999 17:16:36   dirby
   Fixed various bugs in auth advice processing.
   
      Rev 1.45   Dec 22 1999 10:00:06   dirby
   Modified to separate out 'advice' processing from the 'reversal' processing.
   
      Rev 1.44   Dec 16 1999 11:34:52   dirby
   Modified to update tx_key to a response tx_key when
   an original txn is not found for an advice or reversal.
   
      Rev 1.43   Dec 03 1999 14:59:54   svelaga
   Added code to change the status to 
   'logon/logoff' when the switched logon/logoff
   transaction arrives.
   Also added code to send the status to monitor.
   
      Rev 1.42   Nov 17 1999 14:37:02   dirby
   Corrected some compiler errors.
   
      Rev 1.41   Nov 17 1999 14:21:32   dirby
   Modified code for the following reasons:
   1.  Use Update Data Server for updating non-network tables.
   2.  Implement new reversal strategy.
   
      Rev 1.40   Aug 30 1999 15:50:02   egable
   Implement new reversal design.
   
      Rev 1.39   Aug 19 1999 11:46:36   egable
   Correct an error in the PAN when have track2.
   
      Rev 1.38   Aug 13 1999 13:32:54   egable
   Correct the insert NCF30 paths.
   
      Rev 1.37   Aug 10 1999 16:09:34   egable
   Add code to insert NCF30 before sending
   the response to the host.
   
      Rev 1.36   Jul 29 1999 21:48:24   egable
   Correct errors found when upgrading to 
   Developer Studio 6.0
   
      Rev 1.35   Jul 09 1999 13:59:30   egable
   Send all messages to event log; if an unknown
   bit is found in bitmap, send msg and stop
   processing; change strncpy to memcpy in 
   move_iso..; initialize temporary variable
   before using them; verify lengths for variable
   length fields are valid; stop processing if
   conversion rate is 0; verify that select data
   is numeric or alphanumeric; don't get NCF30;
   add missing error paths; add a free where needed.
   
      Rev 1.34   Jul 07 1999 15:55:50   egable
   Verify that processing code, dates and amounts
   are numeric, also correct an error path.
   
      Rev 1.33   Jun 15 1999 16:30:30   svelaga
   modified the signature of ascii_ebcdic_conv function : removed static qualifier
   
      Rev 1.32   Jun 10 1999 16:19:32   egable
   Make field 26 the category code.
   
      Rev 1.31   Jun 10 1999 14:12:32   egable
   Store the current ATP date/time into 
   date_yyyymmdd and time_hhmmss before
   inserting TLF01 into the database.
   
      Rev 1.30   Apr 29 1999 14:33:06   egable
   Include milliseconds in the transaction id.
   
      Rev 1.29   Apr 22 1999 13:55:42   egable
   Remove debugging error message and undo
   the changes to transaction id.  (The problem I
   was trying to fix turned out not to involve 
   transaction id).
   
      Rev 1.28   Apr 21 1999 20:24:04   egable
   Work on fixing transaction id.
   
      Rev 1.27   Apr 21 1999 19:30:46   egable
   Another fix.
   
      Rev 1.25   Apr 21 1999 17:54:02   egable
   Correct the transaction id
   
      Rev 1.24   Apr 21 1999 09:23:50   egable
   Correct error message for error when inserting
   TLF01.
   
      Rev 1.23   Apr 20 1999 13:02:16   egable
   Correct logoff message, allow any field 
   separator for track 1, remove error messages.
   
      Rev 1.22   Apr 19 1999 16:34:02   egable
   Corrected an error message that appeared 
   when track2 not present.
   
      Rev 1.21   19 Apr 1999 09:55:08   dirby
   Changed the way track 2 is parsed to look for the first
   non-alphanumeric character.
   
      Rev 1.20   Apr 09 1999 11:49:58   egable
   Remove test code.
   
      Rev 1.19   Mar 25 1999 14:08:04   egable
   Update host_start_time, host_finish_time, 
   transaction_start_time, transaction_finish_time
   and correct reversal logic (re: NCF30).
   
      Rev 1.18   Mar 19 1999 10:48:16   egable
   Remove debugging error messages and fix
   track 1 processing.
   
      Rev 1.17   Mar 17 1999 17:20:30   egable
   Correct a problem with track1 and zero fill the
   transaction id.
   
      Rev 1.16   Mar 16 1999 13:06:42   egable
   Changes to debug amex problems
   
      Rev 1.15   Mar 12 1999 08:12:24   egable
   Change the 1804/1814 message (per AMEX),
   Set total amount = 0 when cash_advance is
   rejected, and add error messages.
   
      Rev 1.14   Mar 10 1999 08:34:26   egable
   Correct problems with amounts in TLF01
   for reversals.
   
      Rev 1.13   Mar 09 1999 13:06:06   egable
   Correct an error where a NULL pointer is
   returned from Find_Bitfield_Table.
   
      Rev 1.12   Mar 07 1999 10:46:10   egable
   If necessary, convert total amount from USD
   to Pesos.  Corrected the reversal totals logic.
   
      Rev 1.11   Mar 04 1999 14:40:34   egable
   Correct pin block conversion, correct error
   processing, correctly convert ^ from EBCDIC.
   
      Rev 1.10   Mar 04 1999 08:22:54   egable
   Add a POS reversal.
   
      Rev 1.9   Mar 03 1999 15:14:14   egable
   Correct an error with field 48.
   
      Rev 1.8   Mar 02 1999 16:03:36   egable
   Only move RRN if it is there, store the incoming
   processing code in host_processing_code to
   echo, correct authcode.
   
      Rev 1.7   Mar 02 1999 08:58:00   egable
   Change fields 32 and 33 to "move if there".
   
      Rev 1.6   Feb 27 1999 16:10:24   egable
   Correct an error in the error processing code.
   
      Rev 1.5   Feb 26 1999 14:35:22   egable
   Correct use of 'free'.
   
      Rev 1.4   Feb 26 1999 08:44:46   egable
   Correct error with pin block (it's not EBCDIC)
   
      Rev 1.3   Feb 25 1999 15:28:14   egable
   Add a logon request to host.
   
      Rev 1.2   Feb 24 1999 20:46:54   egable
   Protect against unexpected field code incoming
   message, correct card #, remove update of 
   response code in incoming message, correcte
   response code in outgoing message.
   
      Rev 1.1   Feb 23 1999 10:56:38   egable
   Remove test code 
   
      Rev 1.0   Feb 23 1999 09:17:50   egable
   Initial Release
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <time.h>
//#include <timeb.h>
#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "amexutil.h"
#include "amextran.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "amexmain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "txtrace.h"



/* functions in other parts of host handler */
extern void   perform_error_processing(pAUTH_TX);
extern BOOLEAN send_it_back_to_terminal(BYTE[3], pAUTH_TX);


/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
CHAR   applnk_que_name [] = "applnkA" ;
CHAR   security_que_name [] = "racalA" ;


/* global variables */
extern CHAR  ServiceName[];

extern NCF01    ncf01;
extern BYTE     current_state[2];

extern CHAR  netds_que_name [];
extern CHAR  updateds_que_name[];
extern CHAR  devds_que_name [];
extern CHAR  authorizer_que_name [];
extern CHAR  cur_app_name[];
extern CHAR  oprmon_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  trands_que_name[];
extern CHAR  vLogon_Resp_Indicator[];
extern CHAR  vLogon_NMI_Code[];
extern BYTE  orig_host_start_time[];
extern CHAR  SHARED_MEMORY_TBL[];
extern CHAR  AppName[];
extern CHAR  tpdu_id[];
extern INT     si;
INT          Field63_present;

extern CHAR  ncamex_Error_warning_Filename[256];
extern CHAR  ncamex_module_error_warning_file_name[256];
extern CHAR  ncamex_error_warning_file_path[256];
extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
CHAR masked_card_num[20] = {0};

UINT Max_File_Size_Defined = 0 ;
char amex_scheme_timeout_rc [4]={0};
char amex_scheme_fai_rc [4]={0};
CHAR Dest_Stat_Id[7];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;


/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE ReqRsp_Buffer [COMM_BUF_SZE];

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */


/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 73			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char registration_flag [2];
	char account_selection [2];
	char card_data         [17];
	char challenge1        [5];
	char challenge2        [5];
	char cresponse         [3];
}Table73_Type;
  
/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 74			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char challenge1        [5];
	char challenge2        [5];
}Table74_Type;
  


/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec HOST_MESSAGE;



//farad 8/10/1998 Added the totals_record sturcture, becuase it did not exist in mcrc.

static  char proccode_str [7];

static  int fs_challenge_number = 1;

struct totals_record
   {
   BYTE captured_sales_count   [3];
   BYTE captured_sales_amount  [12];
   BYTE captured_refund_count  [3];
   BYTE captured_refund_amount [12];
   BYTE debit_sales_count      [3];
   BYTE debit_sales_amount     [12];
   BYTE debit_refund_count     [3];
   BYTE debit_refund_amount    [12];
   };


/*******************************************************/
/*  TRANSACTION ENUMERATION                            */
/*******************************************************/
enum trantypes {
   tt_sale,             /* sale */
   tt_check_dl,         /* check - driv lice verify */
   tt_debit,            /* debit */
   tt_offline,          /* offline txn */
   tt_refund,           /* refund */
   tt_settle,           /* settlement */
   tt_void,             /* void */
   tt_test,             /* test */
   tt_sale_response     /* Sale Response , Chipstipe Demo */
   };

enum tranopts1 {
   topt1_notip       = 1,      /* no tip amount */
   topt1_noacct      = 2,      /* no account required in proc code */
   topt1_nocapture   = 4,      /* no capture required */
   topt1_nosystime   = 8,      /* no system time required */
   topt1_credit      = 16,     /* credit type transaction */
   topt1_preadv      = 32,     /* send all advises prior */
   topt1_settle      = 64,     /* settle type transaction */
   topt1_postadv     = 128     /* send single advise after */
   };

enum tranopts2 {
   topt2_noprerev     = 1,      /* no reversal prior */
   topt2_crfl         = 2,      /* CRFL processing allowed */
   topt2_floor        = 4,      /* floor limit processing allowed */
   topt2_referral     = 8,      /* referral processing allowed */
   topt2_batpend      = 16,     /* set batch pending flag for this transaction */
   topt2_debit        = 32,     /* debit transaction */
   topt2_reversal     = 64,     /* send reversal if failure */
   topt2_new_proccode = 128     /* use msg's original proccode */
   };


enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_5recon_amt     = 5,      /* reconcilliation amount */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_18sicnumber    = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* pan extended, country code */
   bf_22pose         = 22,     /* pos entry mode */
   bf_24funccd       = 24,     /* function code */
   bf_25reasoncd     = 25,     /* reason code */
   bf_26accptcd      = 26,     /* acceptor code */
   bf_30origamt      = 30,     /* original amount   */
   bf_31tranid		 = 31,	   /* Acquirer reference data, Transaction Identifier */
   bf_32idcode       = 32,     /* Acquiring institution's ID code */
   bf_33fwdidcode    = 33,     /* Forwarding institution's ID code */
   bf_34panext       = 34,     /* pan extended */
   bf_35track2       = 35,     /* track 2 data */
   bf_37rrn          = 37,     /* retrieval reference number */
   bf_38authcode     = 38,     /* auth code   */
   bf_39respcode     = 39,     /* response code*/
   bf_40src          = 40,     /* service restriction code */
   bf_41termid       = 41,     /* terminal id  */
   bf_42cardacc      = 42,     /* card acceptor id */
   bf_43cardaccname  = 43,     /* card acceptor name/location */
   bf_44address_verif= 44,     /* additional response data */
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addl_data    = 48,     /* Additional Data - Private */
   bf_49trncurcd     = 49,     /* transaction currency code */
   bf_50recncurcd    = 50,     /* reconcilliation currency code */
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_55IccData		 = 55,
   bf_56orgdata      = 56,     /* original data elements */
   bf_59posdata      = 59,     /* national point-of-service geographic data */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_70nmicode      = 70,     /* network management information code */
   bf_73action       = 73,     /* Date, action */
   bf_90orgdata      = 90,     /* original data elements   */
   bf_91fileupdate   = 91,     /* file update code         */
   bf_93rspind       = 93,     /* response indicator       */
   bf_95replamts     = 95,     /* replacement amounts      */
   bf_96msgsec       = 96,     /* message security code    */
   bf_99stlinstid    = 99,     /* settlement institution id code */
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120orgmsgid    = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,     /* free form text - Japan   */
   bf_127filerecs    = 127     /* free form text - Japan   */
   }; 

/* LENGTHs FOR EACH BIT FIELD */
#define bf2_len       2
#define bf3_len       6
#define bf4_len      12
#define bf5_len      12
#define bf6_len      12
#define bf7_len      10
#define bf9_len       8
#define bf10_len      8
#define bf11_len      6
#define bf14_len      4
#define bf15_len      4
#define bf16_len      4
#define bf18_len      4
#define bf20_len      3
#define bf22_len      3
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf35_len      2
#define bf37_len     12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len     15
#define bf44_address_verif_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf54_len      3
#define bf60_len      3
#define bf61_len      3
#define bf62_len      3
#define bf63_len      3
#define bf70_len      3
#define bf90_len     42
#define bf94_len      7
#define bf102_len     2
#define bf103_len     2
#define bf120_len     3
#define bf121_len     3
#define bf127_len     3


       
#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64
int     BitFieldSize = NORMAL_BITFIELD;


struct trandef
{
   BYTE   td_trantype;  /* tran type for this record */
   BYTE   td_msgid[4];   /* iso 8583 message id       */
   BYTE   td_proccode[6]; /* iso 8583 processing code  */
   enum	  tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in message */
};

static  struct trandef TRANTABLE[] = {
      { 
      /********************************************************
         A U T H   A U T H O R I Z A T I O N   R E S P O N S E                                      
      *********************************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0xF1,0xF1,0xF1,0xF0,               /* message id */
      0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,     /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,		 /* MAND */
      bf_4amount, 	 /* MAND */
      bf_5recon_amt,  /* OPT  */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
      bf_24funccd,    /* COND */
	  bf_31tranid,
      bf_32idcode,    /* MAND */
      bf_37rrn,       /* OPT  */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,	 /* OPT  */
      bf_42cardacc,	 /* MAND */
      bf_44address_verif, /*COND*/
      bf_48addl_data, /* COND */    
      bf_49trncurcd,  /* MAND */
      bf_50recncurcd, /* OPT  */
	  bf_55IccData,
	  bf_60priv60,
      bf_61priv61,
	  /*bf_62priv62,*/
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
         A U T H   A D V I C E   R E S P O N S E                                      
      *********************************************************/
      AUTH_ADVICE_RESPONSE,
      0xF1,0xF1,0xF3,0xF0,               /* message id */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,     /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,		 /* MAND */
      bf_4amount, 	 /* MAND */
      bf_5recon_amt,    /*MAND */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_39respcode,  /* MAND */
      bf_42cardacc,	 /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_50recncurcd, /* MAND */
	  bf_60priv60,
     
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
         A U T H   C A S H   A D V A N C E   R E S P O N S E                                      
      *********************************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0xF1,0xF2,0xF1,0xF0,               /* message id */
      0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,     /* processing code */
      /*0xF0,0xF1,0xF4,0xF9,0xF0,0xF0,    * other processing code */
      /*0xF1,0xF7,0xF0,0xF0,0xF0,0xF0,    * other processing code */
      /*0xF1,0xF7,0xF0,0xF8,0xF0,0xF0,    * other processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,		 /* MAND */
      bf_4amount, 	 /* MAND */
      bf_5recon_amt,  /* MAND */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
      bf_19ctrycd,     /* MAND */
      bf_22pose,      /* MAND */
      bf_24funccd,    /* MAND */
      bf_26accptcd,   /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* OPT  */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,	 /* OPT  */
      bf_49trncurcd,  /* MAND */
      bf_50recncurcd, /* OPT  */
	  bf_55IccData,
      bf_61priv61,
      bf_done      /* end of fields to move */
      },

	  /*****************************************************
	  	 	  		AUTH REFUND REQUEST
	  	 	  		*******************************************************/
	  	 	  		{
	  	 	  			AUTH_REFUND,
						0xF1,0xF1,0xF0,0xF0,               /* message id */
						0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,     /* processing code */
	  	 	  			/**** OPTIONS ****/
	  	 	  			topt1_postadv,
	  	 	  			topt2_referral+topt2_reversal+topt2_floor,
	  	 	  			/**** BIT FIELDS ****/
						  bf_2pan,				/* MAND */
								  bf_3proc,				/* MAND */
								  bf_4amount,			/* MAND */
								  bf_7trntime,			/* OPT */
								  bf_11stan,			/* MAND */
								  bf_12time,			/* MAND */
								  bf_13date,			/* OPT */
								  bf_14expiry,			/* OPT */
								  bf_19ctrycd,			/* MAND */
								  bf_22pose,		/* MAND */
								  bf_26accptcd,			/* MAND */
								  bf_32idcode,			/* MAND */
								  bf_35track2,			/* COND */
								  bf_37rrn,				/* OPT */
								  bf_41termid,			/* OPT */
								  bf_42cardacc,			/* MAND */
								  bf_43cardaccname,	/* OPT */
								  bf_45track1,		    /* COND */
								  bf_49trncurcd,		/* MAND */
								  bf_53seccntl,			/* COND */
								  bf_55IccData,
								  bf_61priv61,
								  bf_done			/* end of fields to move */
	  	 	  		},

	  	 	  		/***********************************************
	  	 	  		 *	AUTH REFUND  RESPONSE
	  	 	  		***********************************************/
	  	 	  		{
	  	 	  			AUTH_REFUND_RESPONSE,
						0xF1,0xF1,0xF1,0xF0,               /* message id */
						0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,     /* processing code */
	  	 	  			/**** OPTIONS ****/
	  	 	  			topt1_postadv,
	  	 	  			topt2_referral+topt2_reversal+topt2_floor,
	  	 	  			/**** BIT FIELDS ****/
						   bf_2pan,        /* MAND */
						      bf_3proc,		 /* MAND */
						      bf_4amount, 	 /* MAND */
						      bf_5recon_amt,  /* OPT  */
						      bf_11stan,	    /* MAND */
						      bf_12time,	    /* MAND */
						      bf_24funccd,    /* COND */
						      bf_32idcode,    /* MAND */
						      bf_37rrn,       /* OPT  */
						      bf_38authcode,  /* COND */
						      bf_39respcode,  /* MAND */
						      bf_41termid,	 /* OPT  */
						      bf_42cardacc,	 /* MAND */
						      bf_44address_verif, /*COND*/
						      bf_48addl_data, /* COND */
						      bf_49trncurcd,  /* MAND */
						      bf_50recncurcd, /* OPT  */
							  bf_55IccData,
						      bf_61priv61,
							  bf_62priv62,
						      bf_done      /* end of fields to move */
	  	 	  		},
      {
      /*******************************************************************
         A U T H   A T M   R E V E R S A L   A D V I C E   R E S P O N S E                                      
      ********************************************************************/
      AUTH_REVERSAL_ADVICE_RESPONSE,
      0xF1,0xF4,0xF3,0xF0,               /* message id */
      0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,     /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
	  bf_3proc,		 /* MAND */
      bf_4amount, 	 /* MAND */
      bf_5recon_amt,  /* MAND */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
      bf_19ctrycd,     /* MAND */
	  bf_24funccd,
	  bf_25reasoncd,    /* COND */  /* ThoughtFocus Girija Y, Phani */
	  bf_26accptcd,
	  bf_30origamt,
	  bf_31tranid,
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* COND */
      bf_39respcode,  /* MAND */
	  bf_41termid,     /* MAND */  /* ThoughtFocus Girija Y, Phani */
	  bf_42cardacc,	 /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_50recncurcd, /* MAND */
      bf_56orgdata,   /* MAND */
      bf_done      /* end of fields to move */
      },
      {
      /***********************************************************
         A U T H   P O S   R E V E R S A L   R E S P O N S E                                      
      ************************************************************/
      AUTH_REVERSAL_REPEAT_RESPONSE,
      0xF1,0xF4,0xF3,0xF0,               /* message id */
      0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,     /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,		 /* MAND */
      bf_4amount, 	 /* MAND */
      bf_5recon_amt,  /* MAND */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
	  bf_25reasoncd,    /* COND */	/* ThoughtFocus Girija Y, Phani */
      bf_32idcode,    /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,     /* MAND */  /* ThoughtFocus Girija Y, Phani */
      bf_42cardacc,	 /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_50recncurcd, /* MAND */
	  bf_60priv60,
       bf_61priv61,
      bf_done      /* end of fields to move */
      },

	  {
	  		  /********************************
	  			 R E V E R S A L  A D V I C E
	  		  **********************************/
	  		  AUTH_REVERSAL_ADVICE,
	  		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
	  		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
	  		  /**** OPTIONS ****/
	  		  topt1_postadv,
	  		  topt2_referral+topt2_reversal+topt2_floor,
	  		  /**** BIT FIELDS ****/
	  		  bf_2pan,				/* MAND */
	  		  bf_3proc,				/* MAND */
	  		  bf_4amount,			/* MAND */
	  		  bf_5recon_amt,		/* COND */	// Tf Phani
	  		  bf_11stan,			/* MAND */
			  bf_12time,			/* MAND */
	  		  bf_24funccd,			/* MAND */
			  bf_25reasoncd,			/* MAND */
	  		 // bf_30origamt,			/* MAND */
	  		  bf_31tranid,			/* COND */
	  		  bf_32idcode,			/* OPT */
			  bf_33fwdidcode,   /* MAND */  // TF PHANI
	  		  bf_37rrn,				/* OPT */
	  		  bf_38authcode,		/* OPT */
	  		  bf_41termid,			/* OPT */ // TF PHani
	  		  bf_42cardacc,			/* MAND */
	  		  bf_49trncurcd,		/* MAND */
	  		  bf_50recncurcd,		/* COND */  /* ThoughtFocus Girija Y */
	  		  bf_56orgdata,			/* OPT */
	  		  bf_done			/* end of fields to move */
	        },

      {
         /********************************
             O N L I N E   U P D A T E
          ********************************/
         AUTH_CARD_UPDATE,
         0xF1,0xF3,0xF0,0xF4,                  /* message id, 1304 */
         0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
         /**** OPTIONS ****/
         topt1_postadv,
         topt2_referral+topt2_reversal+topt2_floor,
         /**** BIT FIELDS ****/
         bf_2pan,         /* MAND */
         bf_11stan,       /* MAND */
         bf_12time,       /* MAND */ 
         bf_14expiry,     /* MAND */
         bf_24funccd,     /* MAND */
         bf_25reasoncd,   /* MAND */
         bf_33fwdidcode,  /* MAND */
         bf_done			/* end of fields to move */
      },

      {
      /***********************************************************
         A U T H   L O G O N   R E S P O N S E                                      
      ************************************************************/
      AUTH_LOGON,
      0xF1,0xF8,0xF0,0xF4,               /* message id */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,     /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		 /* MAND */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
      bf_24funccd,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_48addl_data, /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /***********************************************************
         A U T H   L O G O N   R E S P O N S E                                      
      ************************************************************/
      AUTH_LOGON_RESPONSE,
      0xF1,0xF8,0xF1,0xF4,               /* message id */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,     /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		 /* MAND */
      bf_11stan,	    /* MAND */
      bf_12time,	    /* MAND */
      bf_24funccd,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_39respcode,  /* MAND */
      bf_48addl_data, /* COND */
      bf_done      /* end of fields to move */
      }
   };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

/********************************************************/
/* BIT SETTING TABLE                                    */
/********************************************************/
struct isobit 
   {
   INT byte_no;         /* byte number for this bit field */
   INT bit_mask;        /* bit to set in this byte        */
   };

const struct isobit BITTAB[128+1] =
   {
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },   /* bit 1  */   { 0, 0x40 },   /* bit 2  */   { 0, 0x20 },   /* bit 3  */   { 0, 0x10 },   /* bit 4  */
   { 0, 0x08 },   /* bit 5  */   { 0, 0x04 },   /* bit 6  */   { 0, 0x02 },   /* bit 7  */   { 0, 0x01 },   /* bit 8  */
   { 1, 0x80 },   /* bit 9  */   { 1, 0x40 },   /* bit 10 */   { 1, 0x20 },   /* bit 11 */   { 1, 0x10 },   /* bit 12 */
   { 1, 0x08 },   /* bit 13 */   { 1, 0x04 },   /* bit 14 */   { 1, 0x02 },   /* bit 15 */   { 1, 0x01 },   /* bit 16 */
   { 2, 0x80 },   /* bit 17 */   { 2, 0x40 },   /* bit 18 */   { 2, 0x20 },   /* bit 19 */   { 2, 0x10 },   /* bit 20 */
   { 2, 0x08 },   /* bit 21 */   { 2, 0x04 },   /* bit 22 */   { 2, 0x02 },   /* bit 23 */   { 2, 0x01 },   /* bit 24 */
   { 3, 0x80 },   /* bit 25 */   { 3, 0x40 },   /* bit 26 */   { 3, 0x20 },   /* bit 27 */   { 3, 0x10 },   /* bit 28 */
   { 3, 0x08 },   /* bit 29 */   { 3, 0x04 },   /* bit 30 */   { 3, 0x02 },   /* bit 31 */   { 3, 0x01 },   /* bit 32 */
   { 4, 0x80 },   /* bit 33 */   { 4, 0x40 },   /* bit 34 */   { 4, 0x20 },   /* bit 35 */   { 4, 0x10 },   /* bit 36 */
   { 4, 0x08 },   /* bit 37 */   { 4, 0x04 },   /* bit 38 */   { 4, 0x02 },   /* bit 39 */   { 4, 0x01 },   /* bit 40 */
   { 5, 0x80 },   /* bit 41 */   { 5, 0x40 },   /* bit 42 */   { 5, 0x20 },   /* bit 43 */   { 5, 0x10 },   /* bit 44 */
   { 5, 0x08 },   /* bit 45 */   { 5, 0x04 },   /* bit 46 */   { 5, 0x02 },   /* bit 47 */   { 5, 0x01 },   /* bit 48 */
   { 6, 0x80 },   /* bit 49 */   { 6, 0x40 },   /* bit 50 */   { 6, 0x20 },   /* bit 51 */   { 6, 0x10 },   /* bit 52 */
   { 6, 0x08 },   /* bit 53 */   { 6, 0x04 },   /* bit 54 */   { 6, 0x02 },   /* bit 55 */   { 6, 0x01 },   /* bit 56 */
   { 7, 0x80 },   /* bit 57 */   { 7, 0x40 },   /* bit 58 */   { 7, 0x20 },   /* bit 59 */   { 7, 0x10 },   /* bit 60 */
   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 },    /* bit 64 */

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


/************************************************
  Bit Field Data Record
*************************************************/
struct bitfield_data 
   {
   enum bitfields  bd_fieldid;                           /* id for this field */
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };

/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */
#ifdef USE_ORIGINAL
BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
BYTE getFldSep( unsigned char* resp_buff, int index );
INT CondProc(enum bitfields, BYTE);
INT CondExpd(enum bitfields, BYTE);
INT CondPan(enum bitfields, BYTE);
INT CondTrack1(enum bitfields, BYTE);
INT Move_Always(enum bitfields, BYTE);
INT Move_IfThere(enum bitfields, BYTE);
INT Move_IfThere_Spaces(enum bitfields, BYTE); /* Girija Y TF, Feb 25, 2009 */
INT vMove_IfThere(enum bitfields, BYTE);
INT Move_Never(enum bitfields, BYTE);
void EBCDICMove(enum bitfields);
void vEBCDICMove(enum bitfields);
void RspEBCDICvMove(enum bitfields);
void RspEBCDICMove(enum bitfields);
void RspIgnore(enum bitfields);
void vRspIgnore(enum bitfields);
void Rj_with_lead_zeros( pCHAR str, INT FieldLen );


void asc_to_vbcd (pCHAR, INT, BYTE [] );
void asc_to_bcd (pCHAR, INT, BYTE [] );
void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);

void find_cvc_flag( pCHAR, pCHAR, pCHAR);

void Field62Response(enum bitfields);

#else
	static BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
	static BYTE getFldSep( unsigned char* resp_buff, int index );
	static INT CondProc(enum bitfields, BYTE);
	static INT CondExpd(enum bitfields, BYTE);
	static INT CondPan(enum bitfields, BYTE);
	static INT CondTrack1(enum bitfields, BYTE);
	static INT Move_Always(enum bitfields, BYTE);
	static INT Move_IfThere(enum bitfields, BYTE);
	static INT Move_IfThere_Spaces(enum bitfields, BYTE); /* Girija Y TF, Feb 25, 2009 */
	static INT vMove_IfThere(enum bitfields, BYTE);
	static INT Move_Never(enum bitfields, BYTE);
	static void EBCDICMove(enum bitfields);
	static void vEBCDICMove(enum bitfields);
	static void RspEBCDICvMove(enum bitfields);
	static void RspEBCDICMove(enum bitfields);
	static void RspIgnore(enum bitfields);
	static void vRspIgnore(enum bitfields);
	void Rj_with_lead_zeros( pCHAR str, INT FieldLen );


	static void asc_to_vbcd (pCHAR, INT, BYTE [] );
	static void asc_to_bcd (pCHAR, INT, BYTE [] );
	static void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
	static void vBCDMove(enum bitfields fieldid);
	static void RspMovevBCD (enum bitfields fieldid);
	static void GenericMove(enum bitfields fieldid);
	static void RspMoveGeneric(enum bitfields fieldid);
	static void vDataMove(enum bitfields fieldid);
	static void RspMovevData (enum bitfields fieldid);

	void find_cvc_flag( pCHAR, pCHAR, pCHAR);

	static void Field62Response(enum bitfields);
#endif

static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount_bin,      /* data to move */
   12,                           /* # of bytes in  field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_5recon_amt,                /* field */
   HOST_MESSAGE.reconcil_amt,    /* data to move */
   12,                           /* # of bytes in  field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   10,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan_bin,        /* data to move */
   6,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time_bin,        /* data to move */
   12,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date_bin,        /* data to move */
   4,                            /* # of bytes in field */
   Move_Never,                   /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in field */
   CondExpd,                     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   12,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_24funccd,                  /* field */
   HOST_MESSAGE.func_cd,         /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_25reasoncd,                /* field */
   HOST_MESSAGE.reasoncd,        /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_26accptcd,                 /* field */
   HOST_MESSAGE.accpt_bus_cd,    /* data to move */
   4,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_30origamt,                 /* field */
   HOST_MESSAGE.orig_amt,        /* data to move */
   24,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_31tranid,                  /* field */
   HOST_MESSAGE.tranid_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in field */
   vMove_IfThere,                /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdidcode,               /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in field */
   vMove_IfThere,                /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in len field */
   vMove_IfThere,                /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in field */
   Move_IfThere_Spaces,                 /* conditional */ /* Added function to allow spaces, Girija Y TF , Feb 25, 2009 */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccname,             /* field */
   HOST_MESSAGE.cardaccname_len, /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_44address_verif,             /* field */
   HOST_MESSAGE.address_verif_len, /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in len field */
   CondTrack1,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addl_data,               /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   3,                            /* # of bytes in len field */
   Move_IfThere,                 /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50recncurcd,               /* field */
   HOST_MESSAGE.recn_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pin_block,       /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl_len,    /* data to move */
   2,                            /* # of bytes in field */
   vMove_IfThere,                   /* mandatory    */       /* Changed Move_IfThere to vMove_IfThere - ThoughtFocus Girija Y*/
   vEBCDICMove,                   /* move routine */       /* Changed EBCDICmove to vEBCDICMove - ThoughtFocus Girija Y*/
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_55IccData,                 /* field */
   HOST_MESSAGE.IccData_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_56orgdata,                 /* field */
   HOST_MESSAGE.orig_data_len,   /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60priv60,                 /* field */
   HOST_MESSAGE.priv60_len,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_61priv61,                 /* field */
   HOST_MESSAGE.priv61_len,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_62priv62,                 /* field */
   HOST_MESSAGE.priv62_len,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_63priv63,                 /* field */
   HOST_MESSAGE.priv63_len,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
 
   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
   }; /* end BFData */


#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )
          

/*******************************************************************************************************/
/* ROUTINES                                                                                            */
/*******************************************************************************************************/
#ifdef USE_ORIGINAL
	struct bitfield_data *Find_Bitfield_Table (enum bitfields);
	struct trandef       *Find_Tran_Table     (BYTE trantype);
#else
	static struct bitfield_data *Find_Bitfield_Table (enum bitfields);
	static struct trandef       *Find_Tran_Table     (BYTE trantype);

#endif

BOOLEAN ncamex_check_for_inc_3DS2_txn(pAUTH_TX p_auth_tx)
{
	/* ATP will not do validation for the 3DS version and restore it when required.*/
	/*if(strncmp(p_auth_tx->TLF01_details.workstation,"02", 2) == 0 &&
		   strncmp(p_auth_tx->TLF01_details.handler_queue, "ncposA",5) == 0)
	 {
		return true;
	 }*/

	if(strncmp(p_auth_tx->TLF01_details.handler_queue, "ncposA",5) == 0)
	{
		return true;
	}
	 return false;
}

BOOLEAN ncamex_check_for_inc_TO_txn(pAUTH_TX p_auth_tx)
{
	if(strncmp(p_auth_tx->TLF01_details.response_code,AMEX_TIMEOUT, 2) == 0 &&
		   strncmp(p_auth_tx->TLF01_details.handler_queue, "ncposA",5) == 0)
	 {
		return true;
	 }
	 return false;
}


/*****************************************************************************

  Function:    GenericMove

  Description: This function is the generic move function that will move
               data from the HOST_MESSAGE structure to the the raw ISO message.
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
******************************************************************************/
static  void GenericMove(enum bitfields fieldid)
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

/*****************************************************************************

  Function:    bcd_to_asc

  Description: This function will convert bcd string to an ascii string 
  Author: 
      Farad       8/13/98
  Inputs:
      bstring  - the bcd string to conver to ascii
      blen     - the length of bstring
  Outputs:
      astr     - the new ascii string
      astr_len - the length of astr
  Return values:
      None
  Modified by:
******************************************************************************/
static void bcd_to_asc (bstring, blen, astr, astr_len)
   BYTE bstring[];
   INT          blen;
   pCHAR        astr;
   INT          astr_len;
{
   int  i = 0;

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }

   astr [(blen * 2)] = '\0';
} /* bcd_to_asc */

/*****************************************************************************

  Function:    RspMoveGeneric

  Description: This function is the generic move function that will move
               data from the raw ISO message to the HOST_MESSAGE structure.
  Author: 
      Farad       8/13/98
  Inputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
      fieldid                - the bit field of the data to be moved
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data from the incominging ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      memcpy(pBF->p_bd_pdata,         /* to data pointer      */
             &ReqRsp_Buffer[reqidx],      /* move from request buffer */
             pBF->bd_fieldlen);      /* for length of field    */
   
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end RspMoveGeneric */

/*****************************************************************************

  Function:    vEBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->p_db_data[0].
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
******************************************************************************/
static  void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;
   CHAR                 tempstr[4];

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine the value of the variable length field.  Note: for
      AMEX, even the length field is in EBCDIC.  For Visa, the
      length comes in as binary. */

   if (pBF != NULL)
   {
		if(fieldid == 55 || fieldid == 61 || fieldid == 62 || fieldid == 60 )/*AMEX03A*/
		{
			memset (tempstr, 0, sizeof(tempstr));
			
			for( i=0; i<pBF->bd_fieldlen; i++ )
			{
					 tempstr[i] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], E2A);
					 
			}
			movelen = atoi(tempstr) + pBF->bd_fieldlen ;
			

			memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen);

			reqidx = reqidx + movelen;   /* increment buffer pointer */
		}
		else
		{  memset (tempstr, 0, sizeof(tempstr));
	      strncpy(tempstr, pBF->p_bd_pdata,pBF->bd_fieldlen);  // needed because no nulls in HOST_MESSAGE
	      movelen = atoi(tempstr);
	      for( i=0; i<pBF->bd_fieldlen; i++ )
	      {
	         ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
	         reqidx++;
	      }

	      for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++ )
	      {
	         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
	         reqidx++;
	      }
		}
   }
} /* end vEBCDICMove */

/*****************************************************************************

  Function:    EBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->bd_fieldlen.
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
******************************************************************************/
static  void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }
   }
} /* end EBCDICMove */

/*****************************************************************************

  Function:    RspEBCDICvMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed 
               to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;
   char                 templen[5];
   BYTE    fld_sep;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine the value of the variable length field.  Note: for
      AMEX, even the length field is in EBCDIC.  For Visa, the
      length comes in as binary. */

   if (pBF != NULL)
   {
      memset( templen, 0x00, sizeof(templen) );
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         templen[i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         reqidx++;
      }
      movelen = atoi(templen);
      strcpy(pBF->p_bd_pdata, templen);
     
      if(fieldid == 45) //TRACK1
      {
         //call the function that returns the first non-numeric character
         fld_sep= getFldSep( ReqRsp_Buffer, reqidx); 
          /* Move the data, converting it from EBCDIC to ASCII. */
          for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
             pBF->p_bd_pdata[i] = ascii_ebcdic_conv_trk1 (ReqRsp_Buffer[reqidx], E2A,fld_sep);
             reqidx++;
          }

      }else  if(fieldid == 55 || fieldid == 61 || fieldid == 62|| fieldid == 60) //EMV data
      { /*AMEX03A-APRILMANDATE 2024 60*/
        memcpy(pBF->p_bd_pdata+i,&ReqRsp_Buffer[reqidx],movelen);
		reqidx += movelen;
      }
	   else
      {
          /* Move the data, converting it from EBCDIC to ASCII. */
          for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
             pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
             reqidx++;
          }
      }
   }
} /* end RspEBCDICvMove */


  
  
BYTE getFldSep( unsigned char* resp_buff, int index )
{
   INT   flg=1,cnt=0;
   BYTE  returnChar=0x00;

      cnt=index;
      while( flg == 1)
      {
        switch( resp_buff[cnt])
        {
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xC2: cnt++;break; //"B" Format Specifier
        default:flg=0; returnChar=resp_buff[cnt];break;
        }
      }
      return( returnChar );
}   //end of getFldSep

BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep)
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
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[89] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0xB0, 0x8A
      };

   static unsigned char ascii_chart[89] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
	  for( i=0; i<89 && !found; i++ )
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
      for( i=0; i<89 && !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }

      }
 /*     if(fldSep==conv_char)
      {
        ReturnChar = 0x5E;
        found = 1;
      }*/
/* Support ANS X4.16 and ISO 7813 format - Girija Y*/
	  if(conv_char == 0x5F)
	  {
		  ReturnChar = 0x5E;
		  found=1;
	  }

   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}   //end of the ascii_ebcdic_conv_trk1
  
  
  /*****************************************************************************

  Function:    RspEBCDICMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed
               to by pBF->bd_fieldlen.
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
static  void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      /* Move the data, converting it from EBCDIC to ASCII. */
      for( i=0; i<movelen; i++)
      {
         pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         reqidx++;
      }
   }
} /* end RspEBCDICMove */

/*****************************************************************************

  Function:    Rj_with_lead_zeros

  Description: This function will right justify a string and add leading zeros
  Author: 
      unknown
  Inputs:
      str        - the ascii string to be right justified 
      FieldWidth
  Outputs:
      str        - the right justified ascii string
  Return values:
      None
  Modified by:
******************************************************************************/
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

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end RspIgnore */

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the variable length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
      Farad      8/13/98
******************************************************************************/
static  void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vRspIgnore */

/*****************************************************************************

  Function:    bcd_to_int   

  Description: This function will convert a bcd string into an integer. 
  Author: 
      unknown
  Inputs:
      bcd_data  - a pointer to the bcd string to be converted
      bcd_len   - the length of the string pointed to by bcd_data
  Outputs:
      None
  Return values:
      INT - the integer created by converting bcd_data to an integer.
  Modified by:
******************************************************************************/
static  INT bcd_to_int ( BYTE *bcd_data,
                 int           bcd_len    )
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
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

      //if ( 60 == fieldid )
      //   movelen *= 2;

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
      movelen = ReqRsp_Buffer[reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      //if ( 60 == fieldid )
      //   movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end RspMovevBCD */

/*****************************************************************************

  Function:    vDataMove

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
******************************************************************************/
static  void vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata [0];
      ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
      reqidx++;

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vDataMove */

/*****************************************************************************

  Function:    RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to the HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer [reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer [reqidx];
      reqidx++;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end RspMovevData */

/*****************************************************************************

  Function:    CondPan   

  Description: This function will move the account number if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - account number moved
      false - account number not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondPan(enum bitfields bitfield, BYTE p_trantype)
{
/* JH Apr 29  changed procedure to only send the pan if it is there */
   if (HOST_MESSAGE.pan_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(Move_IfThere(bitfield, p_trantype));
  
return(false);
} /* end CondPan */

/*****************************************************************************

  Function:    CondExpd

  Description: This function will move the expiration dat if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - expiration date moved
      false - expiration date not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondExpd(enum bitfields bitfield, BYTE p_trantype)
{
        /* if track2 data here, no need for expdate field */
   if (HOST_MESSAGE.date_expd [1] != 0x00)    /* JH Apr 29  Fixed bug */
       return(true);
  
return(false);
} /* end CondExpd */

/*****************************************************************************

  Function:    CondProc

  Description: This function will determine whether the message type is not 
               a reversal. 
  Author: 
      unknown
  Inputs:
      bitfield - the bit field of the account number 
      trantype - tran type
  Outputs:
      None
  Return values:
      true  - not reversal
      false - reversal
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondProc(enum bitfields bitfield, BYTE trantype)
{
   if (HOST_MESSAGE.msgtype [0] != 0x04)
      return (true);

   return(false);
} /* end CondProc */

/*****************************************************************************

  Function:    CondTrack1

  Description: This function will determine whether or not there is a track1. 
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of track1
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - track1 
      false - no track1
  Return values:
      None
  Modified by:
******************************************************************************/
static  INT CondTrack1 (enum bitfields bitfield, BYTE p_trantype)
{
   if (HOST_MESSAGE.track1_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(true);

return(false);
} /* end CondTrack1 */

/*****************************************************************************

  Function:    Move_IfThere

  Description: This function will determine if the data for fieldid is there.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT Move_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
        INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      for (i=0; i < pBF->bd_fieldlen; i++)
         {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return(true);
         }
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move_IfThere_Spaces

  Description: This function will determine if the data for fieldid is there.
  Author: 
      Girija Y TF
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT Move_IfThere_Spaces(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
        INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      for (i=0; i < pBF->bd_fieldlen; i++)
         {
         if ( (pBF->p_bd_pdata [i] != 0) ) 
            return(true);
         }
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    vMove_IfThere 
  Description: This function will determine if the data for fieldid is there for
               fields with variable lengths.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT vMove_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* if length byte is non-zero */
   if (pBF != NULL)
   {
      if (pBF->p_bd_pdata [0] != 0)
         return(true);
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move_Always

  Description: This function will cause data to be move always by always 
               returning true.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field should be moved
  Modified by:
******************************************************************************/
static  INT Move_Always(enum bitfields bitfield, BYTE p_trantype)
{
   return(true);
} /* end Move_Always */

/*****************************************************************************

  Function:    Move_Never

  Description: This function will cause data to never be moved by always 
               returning false.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      false  - field should not be moved
  Modified by:
******************************************************************************/
static  INT Move_Never(enum bitfields bitfield, BYTE p_trantype)
{
   return(false);
} /* end Move_Never */

/*****************************************************************************

  Function:    Find_Tran_Table

  Description: This function will determine whether the given trantype is in
               the tran table.  If it is, it will return a pointer to the data
               for that trantype.
  Author: 
      unknown
  Inputs:
      trantype - the transaction type
  Outputs:
      None
  Return values:
      pointer to the trandef for the trantype
  Modified by:
******************************************************************************/
static  struct trandef *Find_Tran_Table (BYTE trantype)
{
   struct trandef *ptrandef = NULL;   /* point to return record */
   int      i;                        /* loop index */

   
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); 
         i++)
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         ptrandef = &TRANTABLE[i];
         return(ptrandef);
         break;
      }
   }
   if (NULL == ptrandef)
   {
      ncamex_log_message( 3, 2, "This trantype not found in TRANTABLE" , "Find_Tran_Table" , 0 ,"");
      return(ptrandef);
   }
   return(NULL);

} /* Find_Tran_Table */   

/*****************************************************************************

  Function:    Find_Bitfield_Table

  Description: This function will determine whether the bitfield is in the
               bitfield table.
  Author: 
      unknown
  Inputs:
      bitfield - the bitfield to search for
  Outputs:
      None
  Return values:
      pointer to the bitfield data for the bitfield entered.
  Modified by:
******************************************************************************/
static  struct bitfield_data *Find_Bitfield_Table (enum bitfields bitfield)
{
   struct bitfield_data *pBF = NULL;      /* point to return record       */
   INT      i;            /* loop index         */
   CHAR     temp_str[50] = "";

   for (i=0; i <= N_BFData; i++)
      if ( BFData[i].bd_fieldid == bitfield)
      {
         pBF = &BFData[i];
         return (pBF);
         break;
      }
 
   if (NULL == pBF)
   {
      sprintf(temp_str, "Bitfield: %d not found in BFData", bitfield);
      ncamex_log_message( 3, 2, temp_str , "Find_Bitfield_Table" , 0 ,"");
      return(NULL);
   };
   return(NULL);

} /* Find_Bitfield_Table */

/*****************************************************************************

  Function:    ascii_ebcdic_conv

  Description: This function will convert from ascii to EBCDIC or from 
               EBCDIC to ascii depending on conv_type.
  Author: 
      unknown
  Inputs:
      conv_char - the character to convert
      conv_type - A2E - convert from ascii to EBCDIC
                  E2A - convert from EBCDIC to ascii
  Outputs:
      None
  Return values:
      BYTE - the converted character
  Modified by:
      FArad      8/13/98
******************************************************************************/
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
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
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[89] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0xB0, 0x8A
      };

   static unsigned char ascii_chart[89] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
	  for( i=0; i<88 && !found; i++ )
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
      for( i=0; i<88 && !found; i++ )
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

/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_response

  Description: This function will send the network management response to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx, CHAR amex_response_code[4])
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[512]={0};
   CHAR	    Buffer[512]={0};

   build_host_structure(p_auth_tx, amex_response_code);
   if ( false == Build_Host_Request(p_auth_tx) )
   {
      ncamex_log_message( 3, 2, "Invalid bit field: building network management response msg." , "hhutil_send_net_mngmnt_response" , 0,"" );
      return(false);
   }

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              p_auth_tx->TLF01_details.originator_queue,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	(void)set_originator_info( p_auth_tx, p_msg_host_out );
     
   if ( NULL_PTR == p_msg_host_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);

	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, p_auth_tx->TLF01_details.originator_queue );

   free (p_msg_host_out);

 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s",
               p_auth_tx->TLF01_details.originator_queue, ErrorMsg );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      free(p_msg_host_out);
      return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   free(p_msg_host_out);
   return(true);
} /* hhutil_send_net_mngmnt_response */

/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_request

  Description: This function will send the network management request to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4])
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[512] = {0};
   CHAR	    Buffer[512] = {0};

   build_host_structure(p_auth_tx, amex_response_code);
   if ( false == Build_Host_Request(p_auth_tx) )
   {
      ncamex_log_message( 3, 2, "Invalid bit field: building network management request msg." , "hhutil_send_net_mngmnt_request" , 0 ,"");
      return(false);
   }

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              dialog_que_name,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	(void)set_originator_info( p_auth_tx, p_msg_host_out );
     
   if ( NULL_PTR == p_msg_host_out )
   {
       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
       ncamex_log_message( 3, 2, Log_Bufr , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );

   free( p_msg_host_out );

 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
        sprintf( Buffer, "pteipc error sending to que %s: %s",
               dialog_que_name, ErrorMsg );
        memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
        strcat(Buffer,Log_Bufr);
        ncamex_log_message( 3, 3, Buffer , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
       return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return(true);
} /* hhutil_send_net_mngmnt_request */


/*****************************************************************************

  Function:    Field62Response

  Description: This function will decode field 62 data and put it in HOST_MESSAGE
  Author: 
      unknown
  Inputs:
      fieldid - the index into the bitfield data table
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void Field62Response(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT             i;
   INT             Len;
   INT             TotalLen;
   BYTE   BitMap[8];
   BYTE   TempStr[20];

   /*+-------------------------------+
     | Determine length of field 62: |
     |    Length:    1 byte          |
     |    Bit map:   8 bytes         |
     |    Field 1:   1 byte   EBCDIC |
     |    Field 2:   8 bytes  BCD    |
     |    Field 3:   4 bytes  EBCDIC |
     +-------------------------------+*/

   pBF      = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      TotalLen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;
      if ( TotalLen > 0 )
      {
         for( i=1; i<9; i++ )
            BitMap[i-1] = ReqRsp_Buffer[reqidx+i];

         memcpy( &pBF->p_bd_pdata[0], &ReqRsp_Buffer[reqidx], 9 );
      }

      /* Only the first 3 fields are use in AMEX. */
      Len = 9;
      memset( TempStr, 0x00, sizeof( TempStr ) );
      if ( BitMap[0] & 0x80 )   /* Field 1 */
      {
         pBF->p_bd_pdata[Len] = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+Len], E2A );
         Len++;
      }

      if ( BitMap[0] & 0x40 )   /* Field 2 */
      {
         bcd_to_asc(&ReqRsp_Buffer[reqidx+Len], 8, &pBF->p_bd_pdata[Len], 16 );
         Len += 8;
      }

      if ( BitMap[0] & 0x20 )   /* Field 3 */
      {
         memcpy( TempStr, &ReqRsp_Buffer[reqidx+Len], 4 );
         for( i=0; i<4; i++ )
            TempStr[i] = ascii_ebcdic_conv( TempStr[i], E2A );
         Len += 8;
         memcpy( &pBF->p_bd_pdata[Len], TempStr, 4 );
      }
      reqidx += TotalLen;
   }
}

/*****************************************************************************

  Function:    convert_from_julian_day

  Description: This function will 
     convert_from_julian_day - Get MMDD from YYDDD
     input string format  = YYDDD
     output string format = MMDD
  Author: 
      unknown
  Inputs:
      julian_str - the julian date to be converted
  Outputs:
      date_time  - MMDD
  Return values:
      None
  Modified by:
******************************************************************************/
void convert_from_julian_day( pCHAR julian_str, pCHAR date_time )
{
   CHAR  Year[3];
   CHAR  Month[3];
   CHAR  Days[4];
   INT   iYear;
   INT   NumDays;
   INT   JulianDay;
   INT   iMonth;
   INT   i;
   INT   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   /* Account for Leap Year */
   strncpy( Year, julian_str, 2 );
   iYear = atoi( Year );
   if ( 0 == ( iYear % 4 ) )
      DaysOfMonth[1]++;

   strncpy( Days, julian_str+2, 3 );
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
	
	itoa(iMonth, Month, 10);
   Rj_with_lead_zeros( Month, 2 );
	itoa(NumDays, Days, 10);
   Rj_with_lead_zeros( Days, 2 );
	strncpy( date_time, Month, 2 );
   strncat( date_time+2, Days,  2 );
}
/*****************************************************************************

  Function:    Build_Host_Request

  Description: This function will build the host request buffer.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx)
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;         /* index to bitfields for this transaction */

   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);   /* get pointer to tran table */
   if (pTranDef != NULL)
   {    
      /*-----------------------------------------*/
      /* Copy in Message Id for this transaction */
      /*-----------------------------------------*/
      memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
      bitmapidx = 4;                            /* point to bitmap     */

      /* Point past bitmap, but 1st check for extended bitmap. */
      if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
      {
         reqidx = 20;                         /* Extended     */
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else
      {
         reqidx = 12;                        /* Not Extended */
         BitFieldSize = NORMAL_BITFIELD;
      }

      memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

      /*   
         if this msg doesn't require the original proccode (its already in the ISO
         field), overwrite it with the one from the trandef table.
       */

      if ((pTranDef->td_opts2 & topt2_new_proccode) != 0) 
         memcpy (&HOST_MESSAGE.proccode, pTranDef->td_proccode, sizeof(pTranDef->td_proccode));

      /*-----------------------------------------------------*/
      /* for all requested bitfields for this transaction... */
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

            if (pBF != NULL)
            {
               /*-----------------------------------------------------------*/
               /* if conditional routine returns true, move the bitfield... */
               /*-----------------------------------------------------------*/
               if ((pBF->bd_BFCond)(pBF->bd_fieldid,p_auth_tx->TLF01_details.tx_key) == true)
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
               return( false );
         }
         else
         {
            bfidx = BitFieldSize;
         }  /* end if */
      } /* end for */

      buflen = reqidx;
      return(true);
   }
   else
      return(false);

} /* end Build_Request_Message */

/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
      FArad      8/13/98
******************************************************************************/

INT hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx)
{
   struct bitfield_data *pBF;
   INT                  bfidx;    /* index to bitfields for this transaction */
   INT                  i;

   Field63_present = 0;
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   for( i=0; i<4; i++ )
   {
      HOST_MESSAGE.msgtype[i] = ascii_ebcdic_conv (ReqRsp_Buffer[i], E2A);
   }

   /* Ignore Responses to Reversals. */
   if ( 0 == strncmp( HOST_MESSAGE.msgtype, "04", 2 ) )
      return( true );

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
   for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
   {
       if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
      {
         /* Bit is on, field is there.... */
         if (bfidx == 63) Field63_present=1;
         pBF = Find_Bitfield_Table(bfidx); 
         if (pBF != NULL)       
         {
            /*---------------------------------------*/
            /* call the Response Handling routine... */
            /*---------------------------------------*/
	   
            (pBF->bd_BFRsp)(pBF->bd_fieldid);
         }
         else
         {
            return(false);
         }
      }
   }

   return(true);
} /* hhutil_parse_host_response_msg */


/*****************************************************************************

  Function:    generate_AMEX_EMV_data

  Description: This function will Store the evn raw data in p_auth_tx->EMV_details.emv_block
  			and ICC System Related Data in to related field in p_auth_tx->EMV_details in 
  			ASCII formate.
  Author:
     	TF - Abhishek
  Inputs:
     pAUTH_TX p_auth_tx : Pointer to AUTH_TX structure.
  Outputs:
      None
  Return values:
	None
  Modified by:
******************************************************************************/

void generate_AMEX_EMV_data(pAUTH_TX p_auth_tx)
{

	CHAR	IccData_Length[3]=""; 
	CHAR	IccRawData[256]=""; 
	INT		A2E_count =0;
	CHAR 	IccData_len_ebcdoc_formate[4]="";
	INT 	IccRawDataLen=0;
	CHAR 	IccAsciiformatedata[512]="";
	CHAR 	iadlen[3]="";
	INT 	iadlen_data_len = 0;
	INT 	iadlen_data_ascii_len = 0;
	INT 	ICCfeilddataIndex = 0;
	
	
	/******************************TF Abhishek*************************************/
		/*FEILD 55 EMV data*/
		/*A:Acquirer
			C: Chip Card
			T: Terminal
			F: Fixed
			V:Variable
		 Maximum Length Fixed/variable		Source		Feild Name
			4				F			A			Header Version Name "AGNS"
			2				F			A			Header Version Number  "0001"
			8				F			C			Application Cryptogram
			33				V			C			Issuer Application Data (IAD)
			4				F			T			Unpredictable Number
			2				F			C			Application Transaction Counter (ATC)
			5				F			T			Terminal Verification Results (TVR)
			3				F			T			Transaction Date
			1				F			T			Transaction Type
			6				F			T			Amount, Authorized
			2				F			T			Transaction Currency Code
			2				F			T			Terminal Country Code
			2				F			C			Application Interchange Profile (AIP)
			6				F			T			Amount, Other
			1				F			C			Application PAN Sequence Number
			1				F			C			Cryptogram Information Data
			
	***********************************************************************************/


	/* Copy IccData_len of 3 byte into local buffer */
	memcpy (IccData_Length,HOST_MESSAGE.IccData_len,3);

	/* Conver IccData_len into EBCDIC formate */
	memcpy(IccData_len_ebcdoc_formate,IccData_Length,3);

	for ( A2E_count = 0; A2E_count <3; A2E_count++)
		{
			IccData_len_ebcdoc_formate[A2E_count]=ascii_ebcdic_conv(IccData_len_ebcdoc_formate[A2E_count],A2E);
		}

	/*1st 3 bytes of emv_block in EVM_detalis structure is used to hold IccData_len_ebcdoc_formate 
	Store IccData_len to in to EVM_detalis.emv_block*/
	memcpy(p_auth_tx->EMV_details.emv_block,IccData_len_ebcdoc_formate,3);

	/* Copy Raw ICC data in to local buffer*/
	IccRawDataLen=atoi(IccData_Length);
	memcpy(IccRawData,HOST_MESSAGE.IccData,IccRawDataLen);

	/* Copy ICC Header Version Name & ICC Header Version Name in to EVM_detalis.future_field1*/
	memcpy(p_auth_tx->EMV_details.future_field1,IccRawData,6);

	/* Store Raw ICC data in to EVM_detalis.emv_block field*/
	memcpy(p_auth_tx->EMV_details.emv_block+3,IccRawData,IccRawDataLen);

	/* Convert Bin data to Ascii data*/
	genutil_bin_to_ascii(IccRawData+6,IccRawDataLen -6,IccAsciiformatedata,(2*IccRawDataLen)-6);

	/*Store Application Cryptogram in to EVM_detalis.app_crypto field*/
	memcpy(p_auth_tx->EMV_details.app_crypto,IccAsciiformatedata + ICCfeilddataIndex,APP_CRYT_DATA_ASCII_LEN);

	/* Move pointer to point Issuer Application Data (IAD)  in buffer */
	memcpy(iadlen,IccAsciiformatedata+ APP_CRYT_DATA_ASCII_LEN,2);/*Copy length of IAD*/
	iadlen_data_len = atoi(iadlen);
	iadlen_data_ascii_len = 2 *iadlen_data_len;
	ICCfeilddataIndex = APP_CRYT_DATA_ASCII_LEN +2;
	
	/*Store Issuer Application Data (IAD) in to EVM_detalis.iad field*/
	memcpy(p_auth_tx->EMV_details.iad,IccAsciiformatedata + ICCfeilddataIndex,iadlen_data_ascii_len);	

	/* Move pointer to point Unpredictable Number  in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + iadlen_data_ascii_len ;
	
	/*Store Unpredictable Number in to EVM_detalis.unpredictable_number field*/
	memcpy(p_auth_tx->EMV_details.unpredictable_number,IccAsciiformatedata + ICCfeilddataIndex,UNPRE_NUM_DATA_ASCII_LEN);

	/* Move pointer to point  Application Transaction Counter (ATC) in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + UNPRE_NUM_DATA_ASCII_LEN;
	
	/*Store  Application Transaction Counter (ATC) in to EVM_detalis.atc field*/
	memcpy(p_auth_tx->EMV_details.atc,IccAsciiformatedata + ICCfeilddataIndex,ATC_DATA_ASCII_LEN);

	/* Move pointer to point Terminal Verification Results (TVR) in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + ATC_DATA_ASCII_LEN;
	
	/*Store Terminal Verification Results (TVR)  in to EVM_detalis.tvr field*/
	memcpy(p_auth_tx->EMV_details.tvr,IccAsciiformatedata + ICCfeilddataIndex,TVR_DATA_ASCII_LEN);

	/* Move pointer to point Transaction Date in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex  + TVR_DATA_ASCII_LEN;
	
	/*Store Transaction Date in to EVM_detalis.tran_date field*/
	memcpy(p_auth_tx->EMV_details.tran_date,IccAsciiformatedata + ICCfeilddataIndex,TAR_DATE_DATA_ASCII_LEN);

	/* Move pointer to point Transaction Type in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TAR_DATE_DATA_ASCII_LEN;
	
	/*Store Transaction Type in to EVM_detalis.tran_type field*/
	memcpy(p_auth_tx->EMV_details.tran_type,IccAsciiformatedata + ICCfeilddataIndex,TAR_TYPE_DATA_ASCII_LEN);

	/* Move pointer to point Amount, Authorized in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TAR_TYPE_DATA_ASCII_LEN;
	
	/*Store Amount, Authorized in to EVM_detalis.tran_amount field*/
	memcpy(p_auth_tx->EMV_details.tran_amount,IccAsciiformatedata + ICCfeilddataIndex,AMT_AUTH_DATA_ASCII_LEN);

	/* Move pointer to point Transaction Currency Code in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + AMT_AUTH_DATA_ASCII_LEN;
	
	/*Store Transaction Currency Code in to EVM_detalis.currency_code field*/
	memcpy(p_auth_tx->EMV_details.currency_code,IccAsciiformatedata + ICCfeilddataIndex,TRA_CUR_CODE_DATA_ASCII_LEN);

	/* Move pointer to point Terminal Country Code in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TRA_CUR_CODE_DATA_ASCII_LEN;
	
	/*Store Transaction Currency Code in to EVM_detalis.country_code field*/
	memcpy(p_auth_tx->EMV_details.country_code,IccAsciiformatedata + ICCfeilddataIndex,TER_CUR_CODE_DATA_ASCII_LEN);

	/* Move pointer to point Application Interchange Profile (AIP) in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TER_CUR_CODE_DATA_ASCII_LEN;
	
	/*Store Application Interchange Profile (AIP) in to EVM_detalis.interchange_profile field*/
	memcpy(p_auth_tx->EMV_details.interchange_profile,IccAsciiformatedata + ICCfeilddataIndex,AIP_DATA_ASCII_LEN);

	/* Move pointer to point Amount, Other in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + AIP_DATA_ASCII_LEN;
	
	/*Store Amount, Other in to EVM_detalis.other_amount field*/
	memcpy(p_auth_tx->EMV_details.other_amount,IccAsciiformatedata + ICCfeilddataIndex,AMT_OTH_DATA_ASCII_LEN);

	/* Move pointer to point Application PAN Sequence Number in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + AMT_OTH_DATA_ASCII_LEN;
	
	/*Store Application PAN Sequence Number in to EVM_detalis.pan_sequence_number field*/
	memcpy(p_auth_tx->EMV_details.pan_sequence_number,IccAsciiformatedata + ICCfeilddataIndex,APP_PAN_SEQ_NUM_DATA_ASCII_LEN);

	/* Move pointer to point Cryptogram Information Data in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + APP_PAN_SEQ_NUM_DATA_ASCII_LEN;
	
	/*Store Cryptogram Information Data in to EVM_detalis.crypto_info field*/
	memcpy(p_auth_tx->EMV_details.crypto_info,IccAsciiformatedata + ICCfeilddataIndex,CRYPTO_INFO_DATA_ASCII_LEN);

/*****************************************************************************/

}

/*****************************************************************************

  Function:    move_iso_response_to_auth_struct

  Description: This function will convert ISO8583 Fields from their binary fields
               to their ascii fields... 
  Author:
      unknown
  Inputs:
      nmi_code  - 
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT move_iso_response_to_auth_struct(CHAR nmi_code[4], pAUTH_TX p_auth_tx, INT flag_63)
{
   CHAR   tempstr[512] = {0};
   pCHAR  tempptr;    
   CHAR   temp[17] = {0};
   INT    total_int = 0;
   INT    actual_int = 0;
   INT    reversal_int = 0;
   BOOLEAN pos11_pin_capable = 0;
   CHAR   pin_capable_list[] = "456789ABC";
   CHAR   amex_response_code[4] = {0};
   INT    i = {0};
   UINT   j, trk_len = 0;
   CRF01  crf01;
   LONG   returncode = 0;
   double conv_rate = 0;
   INT    int_settle_amount;
   INT    int_total_amount;
   double double_total_amount;
   CHAR   Buffer[512]={0};
   INT    int_length, flg =0;
   CHAR  temp_track1_card_num[24];   /* ThoughtFocus Girija Y */
//	pCHAR tempptr = NULL_PTR ;		/* ThoughtFocus Girija Y */
   INT TotalLen = 0;
   CHAR templenstr[4]={0};
   int len_field60=0;
   CHAR  time_date[25] = {0};
   BYTE  time[13] = {0};

#ifdef AMEX03A
    INT     BitMap_Len = 8;
	   BYTE    BitMap[8] ={0};
	   INT     cps_bit = 0;
	   INT     cps_byte = 0;
	   char bit60_data[356]={0};
	   INT bit60_flag=0;
	   char SF1_len[4]={0}; int intsf1_len=0; int bit_60len=0;
#endif

	 #define MAX_AMEX_1120_LEN 240

   /* 3DS 2.0 declaration */
   	BYTE AEVV_result_code[3] = {0};
	CHAR eci_ind[3]="",eci_ebcdic[3]="";
	CHAR aevv_data[100]="";
	BYTE aevv_buff[100]="";
	BYTE DE61_buff[100]="";
	CHAR aevv_value[5]={0};
	INT k=0;
	/* Declaration ends*/

   /* MESSAGE TYPE */
   memcpy (p_auth_tx->TLF01_details.message_type, HOST_MESSAGE.msgtype, sizeof(HOST_MESSAGE.msgtype));

   /* FIELD 2: PAN */
   //  This code is right after FIELD 35, don't move it back

   /* FIELD 3: PROCESSING CODE */
   memcpy (p_auth_tx->TLF01_details.processing_code, HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode));
#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
   if ((0 == strncmp(p_auth_tx->TLF01_details.processing_code, "334000", 6)) &&
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1100", 4)) )
   {
	   //don't alter the PC for ZVAV txn
   }
   else
#endif
   memcpy (p_auth_tx->TLF01_details.processing_code+2, "0",1);
   memcpy (p_auth_tx->host_processing_code, HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode));
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
   memcpy (p_auth_tx->TLF01_details.product_codes[17].amount, HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode));
   if (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4))
   {
      if (0 == strlen(p_auth_tx->host_processing_code))
      {
         memcpy (p_auth_tx->TLF01_details.processing_code, "000000", 6);
      }
   }

   /* FIELD 4: AMOUNT */
   memcpy (p_auth_tx->TLF01_details.total_amount, HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin));

   /* Store amt here as original amount prior to currency conversion. SCR 565 */
   memcpy( p_auth_tx->TLF01_details.sales_amount,
           p_auth_tx->TLF01_details.total_amount, 12 );

   /* FIELD 5: RECONCILIATION AMOUNT */
   memcpy (p_auth_tx->TLF01_details.settlement_total, HOST_MESSAGE.reconcil_amt, sizeof(HOST_MESSAGE.reconcil_amt));

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */
   memcpy( p_auth_tx->TLF01_details.transmission_timestamp, HOST_MESSAGE.gmt_time, sizeof(HOST_MESSAGE.gmt_time) );

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   memcpy (p_auth_tx->TLF01_details.sys_trace_audit_num, HOST_MESSAGE.stan_bin, sizeof(HOST_MESSAGE.stan_bin));

   /* FIELD 12: LOCAL TRANSACTION DATE AND TIME */
   {
      get_current_date_time( p_auth_tx->TLF01_details.date_yyyymmdd,
                             p_auth_tx->TLF01_details.time_hhmmss  );

      /* Save this to echo in the response. */
      memcpy( p_auth_tx->local_date_time, HOST_MESSAGE.time_bin, 12 );
   }
   if(HOST_MESSAGE.time_bin[0] != 0x00)
   {
 		ptetime_get_timestamp( time_date );
		memcpy( time, HOST_MESSAGE.time_bin, 12);
		//Store the Century details
		memcpy( p_auth_tx->TLF01_details.date_yyyymmdd, time_date, 2);
		memcpy( p_auth_tx->TLF01_details.date_yyyymmdd + 2, time, 6);
		memcpy( p_auth_tx->TLF01_details.time_hhmmss, time + 6,   6);
   }

   /* FIELD 13: EFFECTIVE DATE */
   memcpy (p_auth_tx->effective_date, HOST_MESSAGE.date_bin, sizeof(HOST_MESSAGE.date_bin));

   /* FIELD 14: DATE EXPIRED */
   memcpy (p_auth_tx->TLF01_details.exp_date, HOST_MESSAGE.date_expd, sizeof(HOST_MESSAGE.date_expd));

   /* FIELD 19: COUNTRY CODE */
   memcpy (p_auth_tx->country_code, HOST_MESSAGE.ctry_code, sizeof(HOST_MESSAGE.ctry_code));

   /* FIELD 22: POS ENTRY MODE  */
   // Create an ISO pos entry mode
   pos11_pin_capable = false;
   for (i = 0; i < 9; i++)
   {
      if (HOST_MESSAGE.posentry[9+2] == pin_capable_list[i])
         pos11_pin_capable = true;
   }
   strncpy(p_auth_tx->TLF01_details.pos_entry_mode, "0",1);
   if ( HOST_MESSAGE.posentry[6] == '1')
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"01",2);
   else if ( HOST_MESSAGE.posentry[6] == '2' || HOST_MESSAGE.posentry[6] == 'W')
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"02",2);
   else if ( HOST_MESSAGE.posentry[6] == '6' ||  HOST_MESSAGE.posentry[6] == 'V')
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"01",2);
   else if ( HOST_MESSAGE.posentry[6] == '5' && HOST_MESSAGE.posentry[5] == 'X')
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"07",2);
   else if ( HOST_MESSAGE.posentry[6] == '9' )
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"80",2);
   else if ( HOST_MESSAGE.posentry[6] == '5' )
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"05",2);
   else if ( HOST_MESSAGE.posentry[6] == '6' )
         strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"10",2);
   else if ( HOST_MESSAGE.posentry[1] == '1' && HOST_MESSAGE.posentry[4] == 'S')
        strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"01",2);
   else 
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"00",2);
   if ( HOST_MESSAGE.posentry[11] == '0')
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+3,"2",1);
   else if (true == pos11_pin_capable)
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+3,"1",1);
   else 
      strncpy(p_auth_tx->TLF01_details.pos_entry_mode+3,"0",1);
   //  Save the original for the response
   memcpy (p_auth_tx->amex_pos_entry_mode, HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry));
   memcpy (p_auth_tx->TLF01_details.product_codes[8].amount, HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry));	
   memcpy (p_auth_tx->TLF01_details.product_codes[16].amount, HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry));	
   /* FIELD 24: FUNCTION CODE */
   memcpy (p_auth_tx->function_code, HOST_MESSAGE.func_cd, sizeof(HOST_MESSAGE.func_cd));

   if (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4) ||
		   0 == strncmp(p_auth_tx->TLF01_details.message_type, "1120", 4) ||
		   0 == strncmp(p_auth_tx->TLF01_details.message_type, "1100", 4) )
      {
	   	   memcpy(p_auth_tx->TLF01_details.txn_cnt ,p_auth_tx->function_code,sizeof(p_auth_tx->function_code));
      }
   /* FIELD 25: REASON CODE */
   
   memcpy(p_auth_tx->TLF01_details.product_codes[13].code, HOST_MESSAGE.reasoncd, 
	      sizeof(HOST_MESSAGE.reasoncd)); /* Uncommented - ThoughtFocus Girija Y - required for reversals */

   /* FIELD 26: CARD ACCEPTOR BUSINESS CODE */
   memcpy (p_auth_tx->TLF01_details.category_code, HOST_MESSAGE.accpt_bus_cd, sizeof(HOST_MESSAGE.accpt_bus_cd));

   /* FIELD 30: ORIGINAL AMOUNT  */
   // Moved to below field 49
	/* FIELD 31: copy TID - TF Phani*/
   memset (tempstr, 0, sizeof(tempstr));
   memcpy (tempstr, HOST_MESSAGE.tranid_len, 2);
   int_length = atoi(tempstr);
   if(int_length > 15)
   {
    memcpy(p_auth_tx->TLF01_details.visa_tran_id,HOST_MESSAGE.tranid,15);
	memcpy(p_auth_tx->TLF01_details.visa_validation_code,&HOST_MESSAGE.tranid[15],int_length-15);
   }else
   memcpy(p_auth_tx->TLF01_details.visa_tran_id,HOST_MESSAGE.tranid,int_length);

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE  */  // This will always be 06370100
   memset (tempstr, 0, sizeof(tempstr));
   memcpy (tempstr, HOST_MESSAGE.acq_inst_len, 2);
   int_length = atoi(tempstr);
   if (int_length >= sizeof(p_auth_tx->acq_inst_id_cd)) 
      int_length = (sizeof(p_auth_tx->acq_inst_id_cd) - 1);
   memcpy (p_auth_tx->acq_inst_id_cd, HOST_MESSAGE.acq_inst_id, int_length); 

   /* Store here for TLF01. */
   strcpy(p_auth_tx->TLF01_details.acquiring_id, p_auth_tx->acq_inst_id_cd);
   
   /* FIELD 33: FORWARDING INSTITUTION ID CODE  */
   memset (tempstr, 0, sizeof(tempstr));
   memcpy (tempstr, HOST_MESSAGE.fwd_inst_len, 2);
   int_length = atoi(tempstr);
   if (int_length >= sizeof(p_auth_tx->forwarding_inst_id_cd)) 
      int_length = (sizeof(p_auth_tx->forwarding_inst_id_cd) - 1);
	memcpy (p_auth_tx->forwarding_inst_id_cd, HOST_MESSAGE.fwd_inst_id, int_length);

	/* Store here for TLF01. */
   strcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
           p_auth_tx->forwarding_inst_id_cd );

   /* FIELD 35: TRACK 2 */
   if (HOST_MESSAGE.track2_len[0] >= sizeof(p_auth_tx->TLF01_details.track2))
      memcpy (p_auth_tx->TLF01_details.track2, HOST_MESSAGE.track2, sizeof(p_auth_tx->TLF01_details.track2)-1);
   else
      memcpy (p_auth_tx->TLF01_details.track2, HOST_MESSAGE.track2, HOST_MESSAGE.track2_len[0]);

   if (0 != strcmp(NULL_STR, p_auth_tx->TLF01_details.track2))
   {
      /* Find field separator. */
      trk_len = strlen( p_auth_tx->TLF01_details.track2 );
      tempptr = p_auth_tx->TLF01_details.track2;
      for( j=0; j<trk_len; j++ )
      {
         if ( isdigit(*tempptr) )
            ++tempptr;
         else
            break;
      }
      if ( j >= trk_len )
      {
         ncamex_log_message( 3, 2, "AMEX Track2 field separator not found" , "move_iso_response_to_auth_struct" , 0 ,"");
      }
      else
      {
         memcpy (p_auth_tx->TLF01_details.exp_date, tempptr+1,
                 sizeof(p_auth_tx->TLF01_details.exp_date)-1);
         memcpy (p_auth_tx->TLF01_details.service_code,tempptr+5,
        		 sizeof(p_auth_tx->TLF01_details.service_code)-1);
         i = (((int)tempptr) - ((int)p_auth_tx->TLF01_details.track2));
         memcpy (p_auth_tx->TLF01_details.card_num,
                 p_auth_tx->TLF01_details.track2, i);
         itoa((((int)tempptr) - ((int)p_auth_tx->TLF01_details.track2)),
               p_auth_tx->TLF01_details.card_num_len,  10);

		 if( trk_len > 31) /* According to ISO 7813 - ThoughtFocus Girija Y , Phani*/
		 {
			memcpy (p_auth_tx->effective_date, tempptr+8,
					sizeof(p_auth_tx->effective_date)-1);
		 }
		 else /* According to ANSI X4.16 - ThoughtFocus Girija Y , Phani */ 
		 {
			 memcpy (p_auth_tx->effective_date, tempptr+5,
					sizeof(p_auth_tx->effective_date)-1);
		 }

      }
	  // strcpy(p_auth_tx->TLF01_details.product_codes[19].quantity, "44");// TF Phani..Track 2 contain CVV/4CSC,5CSC, We need to send DE44.4 
   }



   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy (p_auth_tx->TLF01_details.retrieval_ref_num, HOST_MESSAGE.rrn, sizeof(HOST_MESSAGE.rrn));

   /* FIELD 38: AUTH NUMBER */
   memcpy (p_auth_tx->TLF01_details.auth_number, HOST_MESSAGE.authcode, sizeof(HOST_MESSAGE.authcode));

   /* FIELD 39: RESPONSE CODE */
   if ( (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1120", 4)) ||
        (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1814", 4)) ||
        (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1314", 4)) ||
		(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4)) ||
		(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1430", 4)) ||
		(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1110", 4)))

   {
      memset (amex_response_code, 0x00, sizeof(amex_response_code));
      if(strlen(HOST_MESSAGE.responsecd)>0)
      {
    	  memcpy (amex_response_code, HOST_MESSAGE.responsecd, sizeof(HOST_MESSAGE.responsecd));
      }
      match_AMEX_to_ISO_response_code(p_auth_tx, amex_response_code);

      /* This is used for Online Updates (1304/1314) */
      if ( 0 == strncmp(p_auth_tx->TLF01_details.message_type, "1314", 4) )
         strcpy( p_auth_tx->accptor_bus_code, amex_response_code );

      //Store Action code so that we can show in transaction Lookup
      memcpy( p_auth_tx->TLF01_details.product_codes[12].code,
         		  HOST_MESSAGE.responsecd, 3 );
   }

   /* FIELD 41: TERMINAL ID */
   memcpy (p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid, sizeof(HOST_MESSAGE.termid));

   /* FIELD 42: CARD ACCEPTOR ID CODE */
   memcpy (p_auth_tx->TLF01_details.merchant_id, HOST_MESSAGE.cardaccid, sizeof(HOST_MESSAGE.cardaccid));

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */
   memset (tempstr, 0x00, sizeof(tempstr));
   memcpy (tempstr, HOST_MESSAGE.cardaccname, 40);
   memcpy (p_auth_tx->acceptor_term_name, tempstr,    25);
   memcpy (p_auth_tx->acceptor_city_name, tempstr+25, 13);
   memcpy (p_auth_tx->acceptor_country,   tempstr+38, 2); 

   /* Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   memcpy( p_auth_tx->TLF01_details.card_holder_name,
           HOST_MESSAGE.cardaccname,
           20 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
           HOST_MESSAGE.cardaccname+20,
           20 );
  // memset( p_auth_tx->TLF01_details.ncf01_retired_cwk+13, ' ', 1 );
   //memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk+14, HOST_MESSAGE.cardaccname+38,2 );

   /* FIELD 44: Address Verification */    /* Girija Y - ThoughtFocus */
/*   if ( 0x00 != HOST_MESSAGE.address_verif_len[0] )
   {
	   memcpy(p_auth_tx->TLF01_details.product_codes[16].code,HOST_MESSAGE.address_verif,
		      atoi(HOST_MESSAGE.address_verif_len));
   }*/

   /* FIELD 45: TRACK1 */
   /* If track2 and track1 are coming : track2 is considered */
 /*  if(strlen(p_auth_tx->TLF01_details.track2) == 0)
   {
	   if (strlen(HOST_MESSAGE.track1) > 0)
	   {
		  if (HOST_MESSAGE.track1_len[0] >= sizeof(p_auth_tx->TLF01_details.track1))
			 memcpy (p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1+1, (sizeof(p_auth_tx->TLF01_details.track1)-1));
             //memcpy (p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1, (sizeof(p_auth_tx->TLF01_details.track1)-1));
		  else
			 //memcpy (p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1+1, (HOST_MESSAGE.track1_len[0]-1));
             memcpy (p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1+1, (sizeof(p_auth_tx->TLF01_details.track1)-1));
		   
		  /* Find field separator. */
/*		  tempptr = p_auth_tx->TLF01_details.track1;
		  for( j=0; j<strlen(p_auth_tx->TLF01_details.track1); j++ )
		  {
			 if ( isalnum(*tempptr) )
				++tempptr;
			 else
				break;
		  }
		  if (j<strlen(p_auth_tx->TLF01_details.track1))
		  {
			 itoa((tempptr-p_auth_tx->TLF01_details.track1), p_auth_tx->TLF01_details.card_num_len,  10);
			 memcpy(p_auth_tx->TLF01_details.card_num, p_auth_tx->TLF01_details.track1, (tempptr-p_auth_tx->TLF01_details.track1));
		  }
		  memcpy(p_auth_tx->TLF01_details.exp_date, (p_auth_tx->TLF01_details.track1+strlen(p_auth_tx->TLF01_details.track1)-16), sizeof(p_auth_tx->TLF01_details.exp_date)-1);
		  memcpy (p_auth_tx->effective_date, (p_auth_tx->TLF01_details.track1+strlen(p_auth_tx->TLF01_details.track1)-9), sizeof(p_auth_tx->effective_date)-1);
	   }
   }*/
/* Support both ANSI X4.16 and ISO 7813 Format - ThoughtFocus Girija Y, Phani */

   memcpy( p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1, sizeof(HOST_MESSAGE.track1));

  if(strlen(p_auth_tx->TLF01_details.track2) == 0)
  {
	if (HOST_MESSAGE.track1_len[0] != 0x00)     
   {
      //memcpy( p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1, sizeof(HOST_MESSAGE.track1));

      /* Find 1st field separator; marks end of card number. */
      if ((tempptr = strchr(p_auth_tx->TLF01_details.track1,'^')) != 0x00)
      {
         int_length = (((int)tempptr) - ((int)p_auth_tx->TLF01_details.track1)) - 1;

         memset( temp_track1_card_num, 0x00, sizeof(temp_track1_card_num) );
         memcpy( temp_track1_card_num,
                 p_auth_tx->TLF01_details.track1 + 1,
                 int_length );

         /* Eliminate any spaces (Amex Track1 Card Num has spaces) */
         if ( NULL == strchr(temp_track1_card_num, ' ') )
         {
            /* No spaces */
            memcpy( p_auth_tx->TLF01_details.card_num,
                    temp_track1_card_num,
                    int_length );
         }
         else
         {
            /* Found spaces. Remove them for field 2, card num. */
            for( i=0,j=0; i<int_length; i++ )
            {
               if ( temp_track1_card_num[i] != ' ' )
               {
                  p_auth_tx->TLF01_details.card_num[j++] = temp_track1_card_num[i];
               }
            }
            int_length = j;
         }

         itoa( int_length, p_auth_tx->TLF01_details.card_num_len, 10 );
		 

         /* Find last field separator; marks start of expiry date. */
         if ((tempptr = strrchr(p_auth_tx->TLF01_details.track1,'^')) != 0x00)
         {
            memcpy( p_auth_tx->TLF01_details.exp_date,
                    tempptr+1,
                    sizeof(p_auth_tx->TLF01_details.exp_date)-1);
			if(HOST_MESSAGE.track1_len[0] == '6' && HOST_MESSAGE.track1_len[1] == '0'  )
            {// ISO format always length can be 60 bytes
				memcpy( p_auth_tx->effective_date,
                    tempptr+8,
                    sizeof(p_auth_tx->effective_date)-1);
			}
			else
			{	// ANSI format length will be 59 bytes.

				memcpy( p_auth_tx->effective_date,
                    tempptr+5,
                    sizeof(p_auth_tx->effective_date)-1);
			}

         }
      }
      tempptr = NULL_PTR;
   }
  }

   

   /* FIELD 2: PAN */
   if (0 != strlen(HOST_MESSAGE.pan_bin))
   {
      /* Card number came in; use it instead of track2 */
      memset (p_auth_tx->TLF01_details.card_num_len, 0,
              sizeof(p_auth_tx->TLF01_details.card_num_len));
      memset (p_auth_tx->TLF01_details.card_num, 0,
              sizeof(p_auth_tx->TLF01_details.card_num));
      memcpy (p_auth_tx->TLF01_details.card_num_len,
              HOST_MESSAGE.pan_len, bf2_len);
      if (atoi(p_auth_tx->TLF01_details.card_num_len) >= sizeof(p_auth_tx->TLF01_details.card_num))
         memcpy (p_auth_tx->TLF01_details.card_num, HOST_MESSAGE.pan_bin, (sizeof(p_auth_tx->TLF01_details.card_num)-1));
      else
         memcpy (p_auth_tx->TLF01_details.card_num, HOST_MESSAGE.pan_bin, atoi(p_auth_tx->TLF01_details.card_num_len));
   }

   /* FIELD 48: ADDITIONAL DATA  */
   /* Store the additional data for advice request, Girija Y,TF Mar 04,2009 */
    if( 0 == strncmp(p_auth_tx->TLF01_details.message_type, "1120",4))
	{
		memcpy( templenstr, HOST_MESSAGE.addl_data_len, 3 );
        TotalLen = atoi( templenstr );

		sprintf( Buffer,
                 "Processing Authrization advice request(1120),TotalLen = %003d ",TotalLen);
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncamex_log_message( 3, 1, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
		
		if(TotalLen > MAX_AMEX_1120_LEN)
		memcpy (p_auth_tx->TLF01_details.voice_auth_text, HOST_MESSAGE.addl_data,8);
		else 
		memcpy (p_auth_tx->TLF01_details.voice_auth_text, HOST_MESSAGE.addl_data,TotalLen);	
	}

   /* FIELD 49 TRANSACTION CURRENCY CODE */
	memcpy (p_auth_tx->TLF01_details.currency_code, HOST_MESSAGE.tran_curr_cd, sizeof(HOST_MESSAGE.tran_curr_cd));

   memcpy(p_auth_tx->reconciliation_cur_code,p_auth_tx->TLF01_details.currency_code, sizeof(p_auth_tx->TLF01_details.currency_code));
   memcpy(p_auth_tx->reconciliation_amount,p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));

   /* Store currency here as original, prior to currency conversion. SCR 565 */
   memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
           p_auth_tx->TLF01_details.currency_code, 3 );

   /* FIELD 50 RECONCILIATION CURRENCY CODE */
	memcpy (p_auth_tx->settlement_curr_code, HOST_MESSAGE.recn_curr_cd, sizeof(HOST_MESSAGE.recn_curr_cd));


   /*+------------------------------------------------------------------+
     | Here is a requirement unique to Equitable:                      
     |
     | ------------------------------------------                      
     |
     |   If field 49 is "608" (PESO), use field 4 for the amount field, 
     |   and use the PESO Balance Account for authorization purposes.  
     |
     |   If field 49 is not "608", use field 5 for the amount field, it 
     |   will be in US Dollars and must be converted to PESOs.  Use the 
     |   Peso Balance account for authorization purposes.
     |
     |   If field 50 is not 840 and fld 49 is not 608 then we respond with 
     |  Invalid currency code.
     |
     | For the AMEX Network handler, move the correct amount (after conversion,
     | if necessary and currency code into auth_tx.TLF01_details.total_amount 
     | and auth_tx.currency_code.  Transaction control will use  those 2 fields
     | for authorization.                               
     |
     | This is for switched in, or Issuing, transactions only.         
     |
     +------------------------------------------------------------------+*/

   if((strcmp(p_auth_tx->TLF01_details.message_type,"1804") != 0) &&
      (strcmp(p_auth_tx->TLF01_details.message_type,"1814") != 0) &&
      (strcmp(p_auth_tx->TLF01_details.message_type,"1314") != 0))
   {
      // all these happen only for messages other than logon/logoff

      /* Save the reversal amount into a field that does
       * not get modified.
       */
      memcpy( p_auth_tx->TLF01_details.refund_amount,
              p_auth_tx->TLF01_details.total_amount, 12 );

   // store the original total_amount and currency code
   if ( 0 != strcmp( PESO, p_auth_tx->TLF01_details.currency_code ) ) 
   {
      if(strcmp(p_auth_tx->settlement_curr_code,USD) != 0)
      {
         sprintf( Buffer,
                 "Amex does not support pos txns with currency code other than 608: Currency code : %s\n",
                  p_auth_tx->TLF01_details.currency_code);

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncamex_log_message( 3, 3, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         flg=1;
      }
      else
      {
         // convert the settlement_total (always USD) to PESOs
         strcpy( crf01.primary_key.currency_code, PESO);

         // get conversion value to convert from USD to PESOs
         returncode = FindMemRecord((pBYTE)&crf01,
                                     sizeof(crf01.primary_key.currency_code),
                                     sizeof(crf01),
                                    "Crf01Table");
         if (returncode < 0)
         {
            /* Unable to get currency conversion factor from shared
             * memory.  Get it from the database, interactively.
             */
            returncode = get_crf01( &crf01 );
         }

         if ( returncode >= 0 )
         {
            // currency code found in CRF01 table
            strcpy( p_auth_tx->TLF01_details.currency_code, PESO );
            strcpy( p_auth_tx->TLF01_details.conversion_rate, crf01.conversion_rate);

            /* Also store conversion rate here in case
             * txcntl overwrites the original.
             */
            strcpy( p_auth_tx->settlement_conv_rate, crf01.conversion_rate );

            conv_rate = strtod(crf01.conversion_rate, 0);
            if (conv_rate <= 0)
            {
               sprintf( Buffer,
                       "Conversion rate is 0 for currency code: %s",
                        p_auth_tx->TLF01_details.conversion_rate);

               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(Buffer,Log_Bufr);
               ncamex_log_message( 3, 3, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
               return(false);
            }
            memcpy( p_auth_tx->TLF01_details.actual_amount,
                    p_auth_tx->TLF01_details.total_amount,
                    sizeof(p_auth_tx->TLF01_details.actual_amount));

            int_settle_amount = atoi(p_auth_tx->TLF01_details.settlement_total);
            double_total_amount = ((double)int_settle_amount*conv_rate);
            int_total_amount = (int)double_total_amount;
            itoa(int_total_amount, p_auth_tx->TLF01_details.total_amount, 10);
            Rj_with_lead_zeros(p_auth_tx->TLF01_details.total_amount, 12);
            if (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4))
            {
               memcpy (p_auth_tx->TLF01_details.actual_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.actual_amount));

               /* FIELD 30: ORIGINAL AMOUNT  */
               if (0 != strlen(HOST_MESSAGE.orig_amt))
               {

                  memcpy (p_auth_tx->TLF01_details.total_amount, HOST_MESSAGE.orig_amt+12, 12); /* AMEX03A-mandate2024 future use, position 13 to 24 consists all zeros for partial reversal*/
				  memcpy (p_auth_tx->TLF01_details.orig_amount, HOST_MESSAGE.orig_amt, 12);			/* 1 to 12 contains original estimated transaction amount(DE4 of 1100)*/
                  int_total_amount = atoi(p_auth_tx->TLF01_details.total_amount);
                  double_total_amount = ((double)int_total_amount*conv_rate);
                  int_total_amount = (int)double_total_amount;
                  itoa(int_total_amount, p_auth_tx->TLF01_details.total_amount, 10);
                  Rj_with_lead_zeros(p_auth_tx->TLF01_details.total_amount, 12);
               }
            }
         }
         else if (-1L == returncode)
         {
            // currency code not found in CRF01 table
            sprintf( tempstr,
                    "Currency (%s) conversion record not found.",
                     crf01.primary_key.currency_code );

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(tempstr,Log_Bufr);
            ncamex_log_message( 3, 3, tempstr , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }
   else 
   {
      if (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4))
      {
         memcpy (p_auth_tx->TLF01_details.actual_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.actual_amount));
         if (0 != strlen(HOST_MESSAGE.orig_amt))
         {
            memcpy (p_auth_tx->TLF01_details.total_amount, HOST_MESSAGE.orig_amt, 12);
            memcpy (p_auth_tx->TLF01_details.orig_amount, HOST_MESSAGE.orig_amt, 12); /*copy the original amount of estimated authorization in TLF orig amount as well*/
         }
      }
   }
}

   if (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4))
   {
       if (0 == strncmp(p_auth_tx->function_code, "401", 3)) //Partial reversal
       {
          /*total_int = atoi(p_auth_tx->TLF01_details.total_amount);
          actual_int = atoi(p_auth_tx->TLF01_details.actual_amount);
          reversal_int = total_int - actual_int;
          itoa(reversal_int, p_auth_tx->TLF01_details.reversal_amount,10);
          Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);*/

    	   /* AMEX03A
    	   actual_amount holds the bit 4 of partial reversal,
    	    * total_amount holds the bit 30 (position 1 - 12 ) bit 4 of estimated authorization
    	    * in the reversal_amount we should store the amount to be partially reversed that is bit 4 of partial reversal transaction*/

    	   reversal_int = atoi(p_auth_tx->TLF01_details.actual_amount);
    	   itoa(reversal_int, p_auth_tx->TLF01_details.reversal_amount,10);
    	    Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);
        }
        else if (0 == strncmp(p_auth_tx->function_code, "400", 3)) //Full reversal
        {
          memcpy (p_auth_tx->TLF01_details.actual_amount, "000000000000", sizeof(p_auth_tx->TLF01_details.actual_amount));
          memcpy (p_auth_tx->TLF01_details.reversal_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.reversal_amount));
        }
   }


   /* FIELD 52 PIN DATA */
   if (false == blank_or_null_string(HOST_MESSAGE.pin_block, sizeof(HOST_MESSAGE.pin_block)))
   {
      memset (tempstr, 0x00, sizeof(tempstr));
      genutil_bin_to_ascii (HOST_MESSAGE.pin_block, sizeof(HOST_MESSAGE.pin_block), 
                 &tempstr[0], sizeof(tempstr));   
      memcpy (p_auth_tx->TLF01_details.pin_block, tempstr, sizeof(p_auth_tx->TLF01_details.pin_block)-1);
   }

/************************** ThoughtFocus Girija Y *****************************/

   /* FIELD 53: SECURITY RELATED CONTROL INFORMATION */ 
	if(HOST_MESSAGE.sec_cntl[0] != 0x00)
	{ 
		memcpy( p_auth_tx->TLF01_details.interest_rate,HOST_MESSAGE.sec_cntl,
				sizeof(p_auth_tx->TLF01_details.interest_rate)-1);
		//copy for 4CSC and 5CSC
		if((HOST_MESSAGE.sec_cntl[0] == 'S' && strlen(HOST_MESSAGE.sec_cntl) > 4) ||
				(HOST_MESSAGE.sec_cntl[0] != 'S' && strlen(HOST_MESSAGE.sec_cntl) > 4))
		{
			memcpy( p_auth_tx->TLF01_details.product_codes[18].quantity, &HOST_MESSAGE.sec_cntl[4],
							sizeof(p_auth_tx->TLF01_details.product_codes[18].quantity)-1);
		}
	
		 strcpy(p_auth_tx->TLF01_details.product_codes[19].quantity, "44");// TF Phani..We got 53 now we need send DE44
		 // Set this flag for 5 CSC we need not populate the Field 44 ..TF PHANi
		 if( HOST_MESSAGE.sec_cntl[0] != 'S' ) 
			
		 {
			 if( p_auth_tx->TLF01_details.product_codes[18].quantity[0] != '\0')
			 strcpy(p_auth_tx->TLF01_details.product_codes[17].quantity, "77");
		 }

		 if(HOST_MESSAGE.sec_cntl[0] == 'S' && strlen(HOST_MESSAGE.sec_cntl) == 6)
		 {
			 if (p_auth_tx->TLF01_details.product_codes[18].quantity[1] != '\0')
				 strcpy(p_auth_tx->TLF01_details.product_codes[17].quantity, "77");
		 }

		 /* Update POS entry mode
			1. Find wehether it is Manual or Swipe.
			2. If 3CSC/4CSC is their update posentry mocde 
			a.	For swipe with 3CSC/4CSC or 4DBC/5CSC pos entry mode should be 006X. 
			b.	For manual with 3CSC/4CSC or 4DBC/5CSC pos entry mode should be 007X

		 */
		 if ( HOST_MESSAGE.posentry[6] == '2' || HOST_MESSAGE.posentry[6] == 'W' )
		 {		/* Swipe*/
			strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"06",2);
		 }
		 else if ( HOST_MESSAGE.posentry[6] == '6' || HOST_MESSAGE.posentry[6] == 'V')
		 {    /* Manual entry*/
			strncpy(p_auth_tx->TLF01_details.pos_entry_mode+1,"07",2);
		 }

	}

	/************************** TF Abhishek *****************************/

	/* EVN ICC System Related Data bit 55*/
	if (atoi(HOST_MESSAGE.IccData_len) > 0 &&  atoi(HOST_MESSAGE.IccData_len) < AMEX_EMV_MAX_LEN )
	{
		generate_AMEX_EMV_data(p_auth_tx);
	}

   /* FIELD 56 ORIGINAL DATA */
   memset (tempstr, 0x00, sizeof(tempstr));
   memcpy(tempstr, HOST_MESSAGE.orig_data,  37);
   memcpy(p_auth_tx->TLF01_details.orig_message, tempstr, 4);
   memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, tempstr+4, 6);
   memcpy(p_auth_tx->orig_local_time_date, tempstr+10, 12);
   memcpy(p_auth_tx->orig_acq_inst_id, tempstr+22, 13);

   
   /* stan is copied into orig stan if it is null
      acc to new spec fld 56 Optional for pos reversals
    */
   if( (p_auth_tx->TLF01_details.orig_sys_trace_num[0] == 0x00) && (0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4)))
   {

       strcpy(p_auth_tx->TLF01_details.orig_sys_trace_num,p_auth_tx->TLF01_details.sys_trace_audit_num);
   }
   
   //FIELD 60


#ifdef AMEX03A
   if(atoi(HOST_MESSAGE.priv60_len) > 0)
   {

	   /* Add the comments about the parsing*/
	   strncpy(p_auth_tx->TLF01_details.product_codes[0].code,HOST_MESSAGE.priv60_len,sizeof(HOST_MESSAGE.priv60_len));
	   len_field60=atoi(p_auth_tx->TLF01_details.product_codes[0].code);

	   /*parse to get the bit60.7*/
	   for( i=0; i<5; i++)
	   {
		   bit60_data[i] = ascii_ebcdic_conv (HOST_MESSAGE.priv60[i], E2A);
	   }
	   if(0 == strncmp(bit60_data,BIT60_ADITIONAL_DATA_ID,5))
	   {
		   memset( BitMap, 0x00, sizeof(BitMap) );

		    for( k=0,i=5; k<BitMap_Len; i++ ,k++)
		    	BitMap[k] = HOST_MESSAGE.priv60[i];

		   for(i=9,k=5;i<len_field60;i++,k++)
		   {
			   bit60_data[k] = ascii_ebcdic_conv (HOST_MESSAGE.priv60[i], E2A);
		   }

		   bit_60len+=5;
		   for(cps_byte=1;cps_byte<=3;cps_byte++)
		   {
			   for(cps_bit=1; cps_bit<=8; cps_bit++)
			   {
				   if ( BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
				   {
					   switch(cps_byte)
					   {
					   	   case 1:

							   switch(cps_bit)
							   {
							   	   case 1:
							   		strncpy(SF1_len,&bit60_data[5],3);
							   		bit_60len+=3;
							   		bit_60len+=atoi(SF1_len);

							   		   break;
							   	   case 2:
							   		   bit_60len+=20;
							   		   break;
							   	   case 3:
							   		   bit_60len+=42;
									   break;
								   case 4:
									   bit_60len+=20;
									   break;
							   	   case 5:
							   		   bit_60len+=11;
									   break;
								   case 6:
									   bit_60len+=1;
									   break;
								   case 7:
									   bit60_flag=1;
									   strncpy(p_auth_tx->TLF01_details.visa_merchant_id,&bit60_data[bit_60len],15);
									   printf("bit 60 SF7");
								   break;
							   }

						  break;
					   }
				  }
				   if(bit60_flag == 1)
				   {
					   /*found the SF7 no need to parse other sub fields*/
					   break;

				   }
			   }
			   if(bit60_flag == 1)
			   {
				   /*found the SF7 no need to parse other sub fields*/
				   break;

			   }
		   }

		   /* now store the bit 60 for echo*/

		   {
			   if( len_field60 <= 100)
			   {
				   memcpy(p_auth_tx->future_use_2,bit60_data,5);
				   memcpy(p_auth_tx->future_use_2+5,bit60_data+5,len_field60-5);
			   }
			   else if(len_field60 > 100 && len_field60 <= 208 )
			   {
				   memcpy(p_auth_tx->future_use_2,bit60_data,100);
				   memcpy(p_auth_tx->future_use_3,bit60_data+100,len_field60-100);
			   }
			   memcpy(p_auth_tx->TLF01_details.product_codes[14].amount,BitMap,8);

		   }
	   }
	   else
	   {
		   strncpy(p_auth_tx->TLF01_details.product_codes[0].code,HOST_MESSAGE.priv60_len,sizeof(HOST_MESSAGE.priv60_len));
		   len_field60=atoi(p_auth_tx->TLF01_details.product_codes[0].code);

		   if( len_field60 <= 100)
		   {
			   memcpy(p_auth_tx->future_use_2,HOST_MESSAGE.priv60,len_field60);
		   }
		   else if(len_field60 > 100 && len_field60 <= 200 )
		   {
			   memcpy(p_auth_tx->future_use_2,HOST_MESSAGE.priv60,sizeof(p_auth_tx->future_use_2));
			   memcpy(p_auth_tx->future_use_3,HOST_MESSAGE.priv60+100,len_field60-100);
		   }
		   else if(len_field60 > 200 && len_field60 <= 210)
		   {
			   memcpy(p_auth_tx->future_use_2,HOST_MESSAGE.priv60,sizeof(p_auth_tx->future_use_2));
			   memcpy(p_auth_tx->future_use_3,HOST_MESSAGE.priv60+100,sizeof(p_auth_tx->future_use_2));
			   memcpy(p_auth_tx->TLF01_details.product_codes[14].amount,HOST_MESSAGE.priv60+200,len_field60-200);
		   }
	   }
   }
#endif
  /* if(atoi(HOST_MESSAGE.priv60_len) > 0)
   	{
	   strncpy(p_auth_tx->TLF01_details.product_codes[0].code,HOST_MESSAGE.priv60_len,sizeof(HOST_MESSAGE.priv60_len));
	   len_field60=atoi(p_auth_tx->TLF01_details.product_codes[0].code);

	   if( len_field60 <= 100)
	   {
		   memcpy(p_auth_tx->future_use_2,HOST_MESSAGE.priv60,len_field60);
	   }
	   else if(len_field60 > 100 && len_field60 <= 200 )
	   {
		   memcpy(p_auth_tx->future_use_2,HOST_MESSAGE.priv60,sizeof(p_auth_tx->future_use_2));
		   memcpy(p_auth_tx->future_use_3,HOST_MESSAGE.priv60+100,len_field60-100);
	   }
	   else if(len_field60 > 200 && len_field60 <= 210)
	   {
		   memcpy(p_auth_tx->future_use_2,HOST_MESSAGE.priv60,sizeof(p_auth_tx->future_use_2));
		   memcpy(p_auth_tx->future_use_3,HOST_MESSAGE.priv60+100,sizeof(p_auth_tx->future_use_2));
		   memcpy(p_auth_tx->TLF01_details.product_codes[14].amount,HOST_MESSAGE.priv60+200,len_field60-200);
	   }
   	}
   */
   /*Copy field 61 */
	if(atoi(HOST_MESSAGE.priv61_len) > 0 && atoi(HOST_MESSAGE.priv61_len) < 100)
	{
		strncpy(p_auth_tx->int_net_facility_len,HOST_MESSAGE.priv61_len,sizeof(HOST_MESSAGE.priv61_len));
		memcpy(p_auth_tx->int_net_facility_data,HOST_MESSAGE.priv61,atoi(HOST_MESSAGE.priv61_len));

		//Store ECI value in profile
		strncpy(eci_ebcdic, HOST_MESSAGE.priv61+5, 2);
		// it is in EBCDIC format, change to ASCII
		for( k=0; k<2; k++ )
	    {
	        eci_ind[k] = ascii_ebcdic_conv (eci_ebcdic[k], E2A);
	    }
		strncpy(p_auth_tx->TLF01_details.profile, &eci_ind[0], 2);

		//store AEVV result code in cvc
		memset(AEVV_result_code, 0x00, sizeof(AEVV_result_code));
		memcpy(AEVV_result_code, HOST_MESSAGE.priv61+(AEVV_VALUE_POSITION - 1), 1);
		genutil_bin_to_ascii(AEVV_result_code, 2, aevv_value, sizeof(aevv_value));
		p_auth_tx->TLF01_details.cvc = aevv_value[1];

		//Store AEVV value in dcf01
		memset(tempstr, 0x00, sizeof(tempstr));
		memcpy(tempstr, p_auth_tx->int_net_facility_data, sizeof(p_auth_tx->int_net_facility_data));

		memset(p_auth_tx->TLF01_details.dcf01_retired_cwk, 0x00, sizeof(p_auth_tx->TLF01_details.dcf01_retired_cwk));
		memset(p_auth_tx->TLF01_details.operator_id, 0x00, sizeof(p_auth_tx->TLF01_details.operator_id));
		memset(p_auth_tx->TLF01_details.workstation, 0x00, sizeof(p_auth_tx->TLF01_details.workstation));

		memcpy(aevv_buff, tempstr+(AEVV_VALUE_POSITION-1), 2*AEVV_VALUE_LEN);
		memset(aevv_data, 0x00, sizeof(aevv_data));
		genutil_bin_to_ascii(aevv_buff, 2*AEVV_VALUE_LEN, aevv_data, 2*AEVV_VALUE_LEN);
		memcpy(p_auth_tx->TLF01_details.dcf01_retired_cwk, aevv_data, 2*AEVV_VALUE_LEN);

		memset(aevv_buff, 0x00, sizeof(aevv_buff));
		memcpy(aevv_buff, HOST_MESSAGE.priv61+(SAFEKEY_POSITION-1), DE61_SF8_LEN);
		memset(aevv_data, 0x00, sizeof(aevv_data));
		genutil_bin_to_ascii(&HOST_MESSAGE.priv61[SAFEKEY_POSITION-1], DE61_SF8_LEN, aevv_data, DE61_SF8_LEN);
		//hex_to_ascii(aevv_buff, DE61_buff, SAFEKEY_VALUE_LEN);
		//rread 6 bytes and convert..
		//Store Transaction ID and 3DS version
		memcpy(p_auth_tx->TLF01_details.workstation, aevv_data, DE61_SF8_DATALEN_1);
		memcpy(p_auth_tx->TLF01_details.operator_id, aevv_data+DE61_SF8_DATALEN_1, DE61_SF8_DATALEN_2);

	}
	 /*Copy field 62*/
	if(atoi(HOST_MESSAGE.priv62_len) > 0 && atoi(HOST_MESSAGE.priv62_len) < 60)
	{
		strncpy(p_auth_tx->future_use_1,HOST_MESSAGE.priv62_len,sizeof(HOST_MESSAGE.priv62_len));
		memcpy(p_auth_tx->future_use_1+3,HOST_MESSAGE.priv62,atoi(HOST_MESSAGE.priv62_len));
			
	}
   
   //fld44 exists !!!!!!
   if(Field63_present ==1)  strcpy(p_auth_tx->TLF01_details.product_codes[19].quantity, "44");

   if(flg == 1)
   {
       //respond back to the host with response code 183
       p_auth_tx->TLF01_details.tx_key = get_tran_type_2(p_auth_tx->TLF01_details.tx_key);
       p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);
       incoming_other_host_response_on_error( p_auth_tx,"183");
       flg=0;
       return false;

   }
   /*
    * if trk1 and trk2 data are missing and exp_date is null
    * respond back to the terminal with 101
    */
   /* Dont check for expiry date when sending to Cadencie from Amex host - Girija Y */
  /* if( strcmp(p_auth_tx->TLF01_details.message_type,"1100") == 0)
   {
       if( (strlen(p_auth_tx->TLF01_details.track2) == 0) && (strlen(p_auth_tx->TLF01_details.track1) == 0))
       {
           if( p_auth_tx->TLF01_details.exp_date[0] == 0x00)
           {
               p_auth_tx->TLF01_details.tx_key = get_tran_type_2(p_auth_tx->TLF01_details.tx_key);
               p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);
               incoming_other_host_response_on_error( p_auth_tx,"101");
               return false;
           }
       }
   }*/
   if (false == verify_auth_tx_data(p_auth_tx))
      return(false);

return(true);
} /* move_iso_response_to_auth_struct */
#ifdef AMEX03A
BOOLEAN ncamex_check_for_estimated_transaction(pAUTH_TX p_auth_tx)
{
	if( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1100",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"1110",4)) &&
		 0 == strncmp(p_auth_tx->function_code,"101",3) &&
		(0 == strncmp(p_auth_tx->TLF01_details.product_codes[17].amount,"004000",6) ||
	     0 == strncmp(p_auth_tx->TLF01_details.product_codes[17].amount,"004800",6)) &&
		 0 != strncmp(p_auth_tx->TLF01_details.total_amount,"000000000000",12) )
	{
		return true;
	}

	return false;
}

BOOLEAN ncamex_check_for_incremental_transaction(pAUTH_TX p_auth_tx)
{
	if( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1100",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"1110",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4)) &&
		 0 == strncmp(p_auth_tx->function_code,"106",3) &&
		(0 == strncmp(p_auth_tx->host_processing_code,"004000",6) ||
	     0 == strncmp(p_auth_tx->host_processing_code,"004800",6) ) &&
		 0 != strncmp(p_auth_tx->TLF01_details.total_amount,"000000000000",12) )
	{
		return true;
	}

	return false;
}

BOOLEAN ncamex_check_for_partial_reversal_transaction(pAUTH_TX p_auth_tx)
{
	if( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1420",4) ||
		 0 == strncmp(p_auth_tx->TLF01_details.message_type,"1430",4)) &&
		 0 == strncmp(p_auth_tx->function_code,"401",3) &&
		0 == strncmp(p_auth_tx->host_processing_code,"024000",6) &&
		 0 != strncmp(p_auth_tx->TLF01_details.actual_amount,"000000000000",12) )
	{/* actual_amount holds the bit 4 for partial reversal*/
		return true;
	}

	return false;
}
#endif
/*****************************************************************************

  Function:    build_host_structure

  Description: This function will use the data in the auth_tx entry to build 
               the HOST_MESSAGE structure that will be required to build the 
               AMEX message for the host.

  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void build_host_structure( pAUTH_TX p_auth_tx, CHAR amex_response_code[4])
{	
   CHAR    temp_str[40];
   CHAR    time_date[25];
   CHAR    date_str[9];
   CHAR    time_str[7];INT  len = 0;
   CHAR	   temp_len[3]="";
   INT 	   emv_len=0,i;
   INT 		de55_len=0;
   BYTE 	msg_hdr[5]="AGNS";
   BYTE 	HeaderVer[5]="0001";
   BYTE 	Bin_hdr_ver[3]={0};
   BYTE 	Bin_hdr_name[3]={0};
	CHAR temp_len_de60[6]="";
	BYTE temp_str_de60[5]="";
	INT  len_DE60=0;
	#define DE55_HDR_LEN 4
	#define DE55_HDR_VER 2
    #define DE55_TAG91_LEN 1

#ifdef AMEX03A
	BYTE bit_60_data[356]={0};
	int bit60index=0; int bit60_1_index=0;
#endif


   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* FIELD 2: PAN  */
   memcpy (HOST_MESSAGE.pan_len, p_auth_tx->TLF01_details.card_num_len,bf2_len);
   Rj_with_lead_zeros(HOST_MESSAGE.pan_len, bf2_len);
   memcpy (HOST_MESSAGE.pan_bin, p_auth_tx->TLF01_details.card_num, atoi( p_auth_tx->TLF01_details.card_num_len ) );
	
   /* FIELD 3: PROCESSING CODE */
   memcpy (HOST_MESSAGE.proccode, p_auth_tx->host_processing_code, sizeof(HOST_MESSAGE.proccode));
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
   memcpy(p_auth_tx->TLF01_details.product_codes[17].amount,p_auth_tx->host_processing_code,sizeof(HOST_MESSAGE.proccode));


   /* FIELD 4: AMOUNT */
   memcpy (HOST_MESSAGE.amount_bin, p_auth_tx->TLF01_details.total_amount,sizeof(HOST_MESSAGE.amount_bin));
   if (0 != strncmp(amex_response_code,"000",3))
   {
      if (AUTH_CASH_ADVANCE_RESPONSE == p_auth_tx->TLF01_details.tx_key)
      {
         memcpy (HOST_MESSAGE.amount_bin, "000000000000" ,sizeof(HOST_MESSAGE.amount_bin));
      }
   }
   
   /* FIELD 5: RECONCILIATION AMOUNT - ThoughtFocus Girija Y, Phani */
   memcpy (HOST_MESSAGE.reconcil_amt, p_auth_tx->TLF01_details.settlement_total,sizeof(HOST_MESSAGE.reconcil_amt));
   if (0 != strncmp(amex_response_code,"000",3))
   {
	   if((0== strncmp(amex_response_code,"400",3)) &&  (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE) ||
			   (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE))
	   {

	   }
	   else
	   {
		   if ((AUTH_CASH_ADVANCE_RESPONSE == p_auth_tx->TLF01_details.tx_key) ||
		  (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE) || 
		  (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE))
		   {
			   memcpy (HOST_MESSAGE.reconcil_amt, "000000000000" ,sizeof(HOST_MESSAGE.amount_bin));
		   }
	   }
   }
   
   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */
   memcpy (HOST_MESSAGE.gmt_time, p_auth_tx->TLF01_details.transmission_timestamp, sizeof(HOST_MESSAGE.gmt_time));

   /* FIELD 11: STAN */
   memcpy (HOST_MESSAGE.stan_bin, p_auth_tx->TLF01_details.sys_trace_audit_num,sizeof(HOST_MESSAGE.stan_bin));

   /* FIELD 12: TIME */
   if ( p_auth_tx->local_date_time[0] == 0x00 )
   {
      /* This value does not exist. Need to create it.
       * This is system time in format: YYMMDDhhmmss.
       */

      /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
      ptetime_get_timestamp( time_date );

      /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
      get_date( time_date, date_str );
      get_time( time_date, time_str );

      /* Now format field 12: YYMMDDhhmmss. */
      memcpy( p_auth_tx->local_date_time,   date_str+2, 6 );
      memcpy( p_auth_tx->local_date_time+6, time_str,   6 );
   }
   memcpy( HOST_MESSAGE.time_bin, p_auth_tx->local_date_time, 12 );

   /* FIELD 14: EXPIRATION DATE */
   memcpy( HOST_MESSAGE.date_expd,
           p_auth_tx->TLF01_details.exp_date,
           4 );

   /* FIELD 19: Country Code */
   memcpy (HOST_MESSAGE.ctry_code, p_auth_tx->country_code,sizeof(HOST_MESSAGE.ctry_code));
   
   /* FIELD 22: POS ENTRY MODE */
   memcpy (HOST_MESSAGE.posentry, p_auth_tx->amex_pos_entry_mode,sizeof(HOST_MESSAGE.posentry));
   // Store POS entry mode 
	if((0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"VOICE",5)))
	{
   strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,HOST_MESSAGE.posentry,DE22_SIZE);
	}
   
   /* FIELD 24: FUNCTION CODE */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE )
   {
      memcpy( HOST_MESSAGE.func_cd, "30", 2 );
      HOST_MESSAGE.func_cd[2] = p_auth_tx->TLF01_details.processing_code[0];

      /* FIELD 25: REASON CODE - for Online Updates */
      memcpy( HOST_MESSAGE.reasoncd, p_auth_tx->TLF01_details.nii, 4 );
   }
   else
      memcpy (HOST_MESSAGE.func_cd, p_auth_tx->function_code, sizeof(HOST_MESSAGE.func_cd));

   /* Populate field 25 with value '4006' in case of Timeout - ThoughtFocus Girija Y, Phani */
   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE) || 
	    (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE)
	   )
   {
		if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,TO) ||
			0 == strcmp(p_auth_tx->TLF01_details.response_code,AMEX_TIMEOUT)
			)
				memcpy( HOST_MESSAGE.reasoncd,"4006",sizeof(HOST_MESSAGE.reasoncd));
   }

   /* FIELD 26: CARD ACCEPTOR BUSINESS CODE */
   memcpy (HOST_MESSAGE.accpt_bus_cd, p_auth_tx->TLF01_details.category_code, sizeof(HOST_MESSAGE.accpt_bus_cd));
   
   /*FIELD 30 orig amount: */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE &&
		   0 == strncmp(HOST_MESSAGE.func_cd,"401",3))
   {/*  AMEX03A before it was not built so building it only for partial reversal*/
	   strncpy(HOST_MESSAGE.orig_amt,p_auth_tx->TLF01_details.orig_amount,12);
	   strncpy(HOST_MESSAGE.orig_amt+12,"000000000000",12);
   }
	/* FIELD 31: T ID */
      /* Get the length. */
   len = strlen(p_auth_tx->TLF01_details.visa_tran_id);   
   if(len > 0)
   {
   sprintf( temp_len, "%02d", len );
   memcpy( HOST_MESSAGE.tranid_len, temp_len, 2 );
   memcpy(HOST_MESSAGE.tranid,p_auth_tx->TLF01_details.visa_tran_id,len);
   }

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE */
   if (AUTH_LOGON != p_auth_tx->TLF01_details.tx_key)
   {
      memset(temp_str, 0, sizeof(temp_str));
      itoa(strlen(p_auth_tx->acq_inst_id_cd),temp_str, 10);
      strcpy(HOST_MESSAGE.acq_inst_len,temp_str);
      Rj_with_lead_zeros(HOST_MESSAGE.acq_inst_len, 2);
      memcpy (HOST_MESSAGE.acq_inst_id, p_auth_tx->acq_inst_id_cd,strlen(p_auth_tx->acq_inst_id_cd));

	  /* Store here for TLF01. */
      strcpy( p_auth_tx->TLF01_details.acquiring_id, HOST_MESSAGE.acq_inst_id );
   }
   else
   {
      memset(temp_str, 0, sizeof(temp_str));
      itoa(strlen(ncf01.acquirer_inst_id_code ),temp_str, 10);
      strcpy(HOST_MESSAGE.acq_inst_len,temp_str);
      Rj_with_lead_zeros(HOST_MESSAGE.acq_inst_len, 2);
      memcpy (HOST_MESSAGE.acq_inst_id, ncf01.acquirer_inst_id_code,strlen(ncf01.acquirer_inst_id_code));
   }


   /* FIELD 33: FORWARDING INSTITUTION ID CODE */
   memset(temp_str, 0, sizeof(temp_str));
   itoa(strlen(p_auth_tx->forwarding_inst_id_cd),temp_str, 10);
   strcpy(HOST_MESSAGE.fwd_inst_len,temp_str);
   Rj_with_lead_zeros(HOST_MESSAGE.fwd_inst_len, 2);
   memcpy (HOST_MESSAGE.fwd_inst_id, p_auth_tx->forwarding_inst_id_cd,strlen(p_auth_tx->forwarding_inst_id_cd));

   /* Store here for TLF01. */
      strcpy( p_auth_tx->TLF01_details.forwarding_institution_id, HOST_MESSAGE.fwd_inst_id );

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy (HOST_MESSAGE.rrn, p_auth_tx->TLF01_details.retrieval_ref_num, sizeof(HOST_MESSAGE.rrn));

   /* FIELD 38: AUTHORIZATION IDENTIFICATION RESPONSE */
   if ((!strncmp(amex_response_code,"000",3)) ||
       (!strncmp(amex_response_code,"001",3)) ||
       (!strncmp(amex_response_code,"800",3)) ||
       (!strncmp(amex_response_code,"900",3))) 
   {
      memcpy (HOST_MESSAGE.authcode, p_auth_tx->TLF01_details.auth_number, sizeof(HOST_MESSAGE.authcode));
   }
   else
   {
      /* this must contain zeros if not approved
       * for pos messages it should be space  filled. new Spec
       */
       if( strcmp(AppName,"ncatm")== 0)
       {
          memcpy(HOST_MESSAGE.authcode, "000000",6);
       }
       if (true == ncamex_check_for_inc_3DS2_txn(p_auth_tx))
       {
    	   //Fill Auth number for approved transaction
    	   if  (0 == strncmp(p_auth_tx->TLF01_details.product_codes[12].code, "000", 3))
    	   {
    		   memcpy (HOST_MESSAGE.authcode, p_auth_tx->TLF01_details.auth_number, sizeof(HOST_MESSAGE.authcode));
    	   }
    	   else
    	   {
    		   memcpy(HOST_MESSAGE.authcode, "      ",6);
    		   memcpy(p_auth_tx->TLF01_details.auth_number, "      ",6);
    	   }
       }
       else
       {
           memcpy(HOST_MESSAGE.authcode, "      ",6);
           /*if sending spaces to amex insert spaces only to TLF01*/
           memcpy(p_auth_tx->TLF01_details.auth_number, "      ",6);
       }
/* TF Phani- Changed to Spaces- Req from BDO/AMEX- dt- 22nd AUG-2011*/
	/*   {
           memcpy(HOST_MESSAGE.authcode, "000000",6);
	   }
       else
           memcpy(HOST_MESSAGE.authcode, "000000",6); */

   }
			
   /* FIELD 39: RESPONSE CODE 
    * If we r responding to an auth_advice always send success.
    * 
    */

   if(AUTH_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      memcpy (HOST_MESSAGE.responsecd, "900", sizeof(HOST_MESSAGE.responsecd));	
   }
   else if(true == ncamex_check_for_inc_3DS2_txn(p_auth_tx) &&
		   p_auth_tx->TLF01_details.product_codes[12].code[0] != '\0')
   {
	   memcpy (HOST_MESSAGE.responsecd,p_auth_tx->TLF01_details.product_codes[12].code,3);
   }
   else if(true == ncamex_check_for_inc_TO_txn(p_auth_tx))
   {
	   memcpy (HOST_MESSAGE.responsecd,amex_scheme_timeout_rc,3);
   }
   else
       memcpy (HOST_MESSAGE.responsecd, amex_response_code, sizeof(HOST_MESSAGE.responsecd));	

   // Only for STIP transaction we shall copy the Action code here
   if(0 == strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN))
   {
		//Copy action code.
	   memcpy(p_auth_tx->TLF01_details.product_codes[12].code,
			   HOST_MESSAGE.responsecd,
			   sizeof(HOST_MESSAGE.responsecd));
   }
   /* FIELD 41: CARD ACCEPTOR TERMINAL ID */
	memcpy(HOST_MESSAGE.termid,p_auth_tx->TLF01_details.terminal_id, sizeof(HOST_MESSAGE.termid));

   /* FIELD 42: CARD ACCEPTOR IDENTIFICATION CODE */
   memcpy(HOST_MESSAGE.cardaccid, p_auth_tx->TLF01_details.merchant_id, strlen(p_auth_tx->TLF01_details.merchant_id));	

   /* Field 44 */

   /* From Cadience transactions */
   if( 0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"nposa2",6) && 
	   strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) != 0)/*If transaction is STANDIN , do not copy - TF Phani */
   {
	   if(p_auth_tx->TLF01_details.product_codes[19].code[1] == '1' )
	   memcpy(HOST_MESSAGE.address_verif_len,p_auth_tx->TLF01_details.product_codes[19].code,
		      3);
	   else if(p_auth_tx->TLF01_details.product_codes[19].code[1] == '2' )
	   memcpy(HOST_MESSAGE.address_verif_len,p_auth_tx->TLF01_details.product_codes[19].code,
		      4);
	   else
	   {// error
	   }
   }
	/* For ON US transactions */
   else if(strcmp(p_auth_tx->TLF01_details.product_codes[19].quantity, "44") == 0)
   {
       memcpy( HOST_MESSAGE.address_verif_len,"02", sizeof(HOST_MESSAGE.address_verif_len));
       HOST_MESSAGE.address_verif[0]=0x20;
	   
	   //  00 or 03 set to Y

		if(strcmp(p_auth_tx->TLF01_details.security_response_code,"00") == 0 ||
			strcmp(p_auth_tx->TLF01_details.security_response_code,"03") == 0 ||
			strlen(p_auth_tx->int_net_facility_data) > 8)
		{
			HOST_MESSAGE.address_verif[1] ='Y';
		}
		else if(strcmp(p_auth_tx->TLF01_details.security_response_code,"01") == 0 ||
			strcmp(p_auth_tx->TLF01_details.security_response_code,"02") == 0)
		{
			HOST_MESSAGE.address_verif[1] ='N';
		}
		else
		{	
			HOST_MESSAGE.address_verif[1] ='U';

		}
		/* See if it 5 CSC and ON US dont populate this filed.. TF PHANI*/
	   if(strcmp(p_auth_tx->TLF01_details.product_codes[17].quantity, "77") == 0)
	   {
		   memset( HOST_MESSAGE.address_verif_len,0x00, sizeof(HOST_MESSAGE.address_verif_len));
		   memset(HOST_MESSAGE.address_verif,0x00,sizeof(HOST_MESSAGE.address_verif));
	   }

   }
   /* STIP Phase2 changes - TF Phani*/
   /* In case of STIP : On AMEX 1110 response message field 44, CSC validation code. 
		If it 4CSC or 4DBC, the CSC validation code should always be �Y�. 
		If the setup in the auth profile�s CVV Processing is no validation, the CSC validation code should be �Y� whether 3, 4 or 5 CSC. Field 44 
   */
   if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0 &&
	  strcmp(p_auth_tx->TLF01_details.product_codes[19].quantity, "44") == 0	
	 )
   {
	   //In case of 4 CSC or 4DBC ,validation code should always be �Y 
	   if(((p_auth_tx->TLF01_details.interest_rate[0] != 'S') && strlen(p_auth_tx->TLF01_details.interest_rate) == 4) ||
		  ((p_auth_tx->TLF01_details.interest_rate[0] == 'S') &&
				  (strlen(p_auth_tx->TLF01_details.interest_rate) + strlen(p_auth_tx->TLF01_details.product_codes[18].quantity))== 5)
		 )
	   {   memcpy( HOST_MESSAGE.address_verif_len,"02", sizeof(HOST_MESSAGE.address_verif_len));
		   HOST_MESSAGE.address_verif[0]=0x20;	
		   HOST_MESSAGE.address_verif[1] ='Y';
	   }
	   // If CVV processing is no validation, validation code should always be �Y 
	   if(p_auth_tx->CAF01_details.cvv_processing[0] == '0')
	   {
		   memcpy( HOST_MESSAGE.address_verif_len,"02", sizeof(HOST_MESSAGE.address_verif_len));
		   HOST_MESSAGE.address_verif[0]=0x20;
		   HOST_MESSAGE.address_verif[1] ='Y';
	   }

#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
	   if( (0 == strncmp(p_auth_tx->host_processing_code, "334000", 6)) 	&&
		   ((0 == strncmp(p_auth_tx->TLF01_details.response_code, "00", 2)) ||
		    (0 == strncmp(p_auth_tx->TLF01_details.product_codes[12].code, "000", 3)))
		 )
	   {
		   memcpy( HOST_MESSAGE.address_verif_len,"02", sizeof(HOST_MESSAGE.address_verif_len));
		   HOST_MESSAGE.address_verif[0]=0x20;
		   HOST_MESSAGE.address_verif[1] ='U';
	   }
#endif
   }

   //Tell the host to Standin on LogOff
   if((AUTH_LOGON == p_auth_tx->TLF01_details.tx_key) && (strcmp(p_auth_tx->function_code,"802") == 0) && (si == 1))
   {

       strcpy(p_auth_tx->additional_pos_information,"SI99");
   }
   
   
   
   
   
   /* FIELD 48: ADDITIONAL DATA - PRIVATE */
   if (0 != strlen(p_auth_tx->additional_pos_information))
   {
      memset(temp_str, 0, sizeof(temp_str));
      itoa(strlen(p_auth_tx->additional_pos_information),temp_str, 8);
      strcpy(HOST_MESSAGE.addl_data_len,temp_str);
      Rj_with_lead_zeros(HOST_MESSAGE.addl_data_len, 3);
	   memcpy (HOST_MESSAGE.addl_data, p_auth_tx->additional_pos_information, sizeof(HOST_MESSAGE.addl_data));
   }
   /* FIELD 49: TRANSACTION CURRENCY CODE */
   memcpy( HOST_MESSAGE.tran_curr_cd, p_auth_tx->TLF01_details.currency_code, sizeof(HOST_MESSAGE.tran_curr_cd)*2);   

   /* FIELD 50: RECONCILIATION CURRENCY CODE */

   memcpy( HOST_MESSAGE.recn_curr_cd, p_auth_tx->settlement_curr_code, sizeof(HOST_MESSAGE.recn_curr_cd)*2);   
	//reconciliation_cur_code
   /* FIELD 55: EMV DATA */

    if ( (p_auth_tx->EMV_details.emv_block[0] != 0x00) &&
        (p_auth_tx->EMV_details.emv_block[1] != 0x00)  )
   	{
   		if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)  
		{
			if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)==0)
			{
				de55_len = 0;
			    emv_len= p_auth_tx->EMV_details.future_field1[0];
			    if(emv_len > 0  && emv_len < 12)
			    {
			    	/* ICC header name - AGNS */
			    	convert_to_ebcdic(msg_hdr,HOST_MESSAGE.IccData,DE55_HDR_LEN);
			    	de55_len += DE55_HDR_LEN;

			    	/* ICC header version - 0001 */
			    	ascii_to_bin(HeaderVer,strlen(HeaderVer),&Bin_hdr_ver[0],4);
			    	memcpy(&HOST_MESSAGE.IccData[de55_len],Bin_hdr_ver,DE55_HDR_VER);
			    	de55_len += DE55_HDR_VER;

			    	/* ICC ARPC and ARC 1 byte Data Length(10) */
			    	memcpy(&HOST_MESSAGE.IccData[de55_len],&p_auth_tx->EMV_details.future_field1[0],DE55_TAG91_LEN);
			    	de55_len += DE55_TAG91_LEN;

			    	/* ICC ARPC and ARC Data - 10 bytes */
			    	memcpy(&HOST_MESSAGE.IccData[de55_len],&p_auth_tx->EMV_details.future_field1[1],emv_len);

			    	/* ICC total data length, 17 bytes(34 bits) */
			    	HOST_MESSAGE.IccData_len[0]=0xF0;
			    	HOST_MESSAGE.IccData_len[1]=0xF1;
			    	HOST_MESSAGE.IccData_len[2]=0xF7;
			    }
			}
		}
   	}
	
   /* FIELD 56 ORIGINAL DATA */
   memcpy(HOST_MESSAGE.orig_data, p_auth_tx->TLF01_details.orig_message, 4);
   memcpy(HOST_MESSAGE.orig_data+4, p_auth_tx->TLF01_details.orig_sys_trace_num, 6);
   memcpy(HOST_MESSAGE.orig_data+10, p_auth_tx->orig_local_time_date, 12);
   memcpy(HOST_MESSAGE.orig_data+22, p_auth_tx->orig_acq_inst_id, 13);
   memset(temp_str, 0, sizeof(temp_str));
   itoa(strlen(HOST_MESSAGE.orig_data),temp_str, 10);
   memcpy(HOST_MESSAGE.orig_data_len,temp_str, 2);
   Rj_with_lead_zeros(HOST_MESSAGE.orig_data_len, 2);

   //DE60


	   if(p_auth_tx->TLF01_details.product_codes[0].code[0] != 0x00 )
	   {
#ifdef AMEX03A
		   if(0 == strncmp(p_auth_tx->future_use_2,"AXAAD",5))
		   {
			   strncpy(temp_len_de60,p_auth_tx->TLF01_details.product_codes[0].code,3);
			   len_DE60 = atoi(temp_len_de60);

			   if(len_DE60 <= 100)
			   {
				   memcpy(bit_60_data,p_auth_tx->future_use_2,len_DE60); /*copy to a local variable*/
			   }
			   else if(len_DE60  > 100 && len_DE60 <= 208 )
			   {
				   memcpy(bit_60_data,p_auth_tx->future_use_2,100); /*copy to a local variable*/
				   memcpy(bit_60_data+100,p_auth_tx->future_use_3,strlen(p_auth_tx->future_use_3));
			   }

			   /*copy AXAAD*/
				for( bit60index=0; bit60index<5; bit60index++ )
				 {
					HOST_MESSAGE.priv60[bit60index] = ascii_ebcdic_conv (bit_60_data[bit60index], A2E);
				 }
				/*copy bit map*/
				 char bitmap[9]={0}, binbitmap[9]={0};
				 int len=0;
				// strncpy(bitmap,"00000000",8);
				 genutil_bin_to_ascii(p_auth_tx->TLF01_details.product_codes[14].amount, 4, &bitmap, 8);
				genutil_asc_to_bin(&bitmap,&binbitmap,&len);
				 memcpy(HOST_MESSAGE.priv60+5,binbitmap,len);
				/*copy remaining data*/
				for( bit60_1_index=5,bit60index=9; bit60index<len_DE60; bit60_1_index++,bit60index++ )
				 {
					HOST_MESSAGE.priv60[bit60index] = ascii_ebcdic_conv (bit_60_data[bit60_1_index], A2E);
				 }
		   }
#endif
		   else
		   {
			   strncpy(temp_len_de60,p_auth_tx->TLF01_details.product_codes[0].code,3);

			   len_DE60 = atoi(temp_len_de60);

			   if(len_DE60 <= 100)
			   {
				   memcpy(HOST_MESSAGE.priv60,p_auth_tx->future_use_2,sizeof(p_auth_tx->future_use_2));
			   }
			   else if(len_DE60  > 100 && len_DE60 <= 200 )
			   {
				   memcpy(HOST_MESSAGE.priv60,p_auth_tx->future_use_2,sizeof(p_auth_tx->future_use_2));
				   memcpy(HOST_MESSAGE.priv60+100,p_auth_tx->future_use_3,sizeof(p_auth_tx->future_use_3));
			   }
			   else if(len_DE60 > 200 && len_DE60  <= 210)
			   {
				   memcpy(HOST_MESSAGE.priv60,p_auth_tx->future_use_2,sizeof(p_auth_tx->future_use_2));

				   memcpy(HOST_MESSAGE.priv60+100,p_auth_tx->future_use_3,sizeof(p_auth_tx->future_use_3));

				   memcpy(HOST_MESSAGE.priv60+200,p_auth_tx->TLF01_details.product_codes[14].amount,len_DE60-200);
			   }
		   }
		   sprintf(temp_str_de60,"%03d",len_DE60);
		   for( bit60index=0; bit60index<3; bit60index++ )
		   {
			   HOST_MESSAGE.priv60_len[bit60index] = ascii_ebcdic_conv (temp_str_de60[bit60index], A2E);
		   }
	   }



   /*FIELD 61 AMEX SAFE KEY */
   /*This is a E-commenrce tx, where AEVV value exists*/
   if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0)
   	{
	   if(atoi(p_auth_tx->int_net_facility_len) > 0 && atoi(p_auth_tx->int_net_facility_len) < 100)
	   	{
	   		CHAR eci_ind[3]="",eci_ebcdic[3]="",len_ebcdic[4]="";
			INT eci_value=0,k=0;
			INT AEVV_result_code;
			INT AEVV_key_indicator;
			CHAR AEVV_62_code[2]="";
			INT len_61=0;
			
			#define AEVV_AUTHENTICATED 05
			#define AEVV_ATTEMPTED	   06
			#define AEVV_NOT_AUTHENTICATED 07
			// 61.6.1
			#define AEVV_RESULT_CODE_POSITION 12
			// 61.6.3
			#define AEVV_KEY_IND_POSITION 14
			
			strncpy(eci_ebcdic,p_auth_tx->int_net_facility_data+5,2);
			// it is in EDBCD format, change to ASCII
			 for( k=0; k<2; k++ )
		     {
		         eci_ind[k] = ascii_ebcdic_conv (eci_ebcdic[k], E2A);
		        
		     }
			
			AEVV_result_code = p_auth_tx->int_net_facility_data[AEVV_RESULT_CODE_POSITION-1];
			AEVV_key_indicator = p_auth_tx->int_net_facility_data[AEVV_KEY_IND_POSITION-1];
			eci_value = atoi(eci_ind);
			if(strlen(p_auth_tx->future_use_1) !=0)
			memcpy(AEVV_62_code , &p_auth_tx->future_use_1[14],1);
			if(eci_value == AEVV_AUTHENTICATED)
			{
				memcpy(HOST_MESSAGE.priv61, p_auth_tx->int_net_facility_data, 5);
				if(true == ncamex_check_for_inc_3DS2_txn(p_auth_tx))
				{//3DS 2.0 and self validation is true
					if (p_auth_tx->current_balance_sign == 1 &&
							AEVV_result_code == 0)
					{
						if(AEVV_62_code[0] != 'V' && strlen(AEVV_62_code) != 0)
							HOST_MESSAGE.priv61[5]= 0xF2;
						else
							HOST_MESSAGE.priv61[5]= 0xF2;

						len_61 = 6;
					}
					else if (p_auth_tx->current_balance_sign == 0)
					{
						if(AEVV_62_code[0] != 'V' && strlen(AEVV_62_code) != 0)
							HOST_MESSAGE.priv61[5]= 0xF1;
						else
							HOST_MESSAGE.priv61[5]= 0xF1;
						len_61 = 6;
					}
					//itoa(61_len,temp_str, 10);
	   				sprintf(HOST_MESSAGE.priv61_len,"%03d", len_61);

				}
				else
				{//3DS 1.0

				if(AEVV_62_code[0] != 'V' && strlen(AEVV_62_code) != 0)
				memcpy(&HOST_MESSAGE.priv61[5], AEVV_62_code,1);
				else if(AEVV_key_indicator == 1)
				HOST_MESSAGE.priv61[5]= 0xF1;
				else if(AEVV_result_code == 0)
				HOST_MESSAGE.priv61[5]= 0xF2;
				else if(AEVV_result_code != 0)
				HOST_MESSAGE.priv61[5]= 0xF1;
				else
				HOST_MESSAGE.priv61[5]= 'U';

				len_61 = 6;
				//itoa(61_len,temp_str, 10);
   				sprintf(HOST_MESSAGE.priv61_len,"%03d", len_61);
				}
			}
			else if(eci_value == AEVV_ATTEMPTED)
			{
				memcpy(HOST_MESSAGE.priv61, p_auth_tx->int_net_facility_data, 5);
				if(true == ncamex_check_for_inc_3DS2_txn(p_auth_tx))
				{//3DS 2.0 and self validation is true
					if (AEVV_result_code == 7 ||
							AEVV_result_code == 8	)
					{
						HOST_MESSAGE.priv61[5] = 'U';
						HOST_MESSAGE.priv61[5]= ascii_ebcdic_conv (HOST_MESSAGE.priv61[5], A2E);
						len_61 = 6;
					}
					else if (AEVV_result_code == 0)
					{
						HOST_MESSAGE.priv61[5] = 'V';
						HOST_MESSAGE.priv61[5]= ascii_ebcdic_conv (HOST_MESSAGE.priv61[5], A2E);
						len_61 = 6;
					}
					//itoa(61_len,temp_str, 10);
					sprintf(HOST_MESSAGE.priv61_len,"%03d", len_61);

				}
				else
				{
					if(AEVV_62_code[0] != 'V' && strlen(AEVV_62_code) != 0)
					memcpy(&HOST_MESSAGE.priv61[5], AEVV_62_code,1);
					else if(AEVV_result_code == 0)
					HOST_MESSAGE.priv61[5]= 0xF3;
					else if(AEVV_result_code != 0)
					HOST_MESSAGE.priv61[5]= 0xF3;
					else
					HOST_MESSAGE.priv61[5]= 'U';
					len_61 = 6;
					//itoa(61_len,temp_str, 10);
					sprintf(HOST_MESSAGE.priv61_len,"%03d", len_61);
				}
			}
			else if(eci_value == AEVV_NOT_AUTHENTICATED)
			{
				// no need to send DE61
			}
			if(strlen(HOST_MESSAGE.priv61_len) > 0 )
			{
				for( k=0; k<3; k++ )
			     {
			         len_ebcdic[k] = ascii_ebcdic_conv (HOST_MESSAGE.priv61_len[k], A2E);
			        
			     }
				strncpy(HOST_MESSAGE.priv61_len,len_ebcdic,3);
				memcpy(HOST_MESSAGE.priv61, p_auth_tx->int_net_facility_data, 5);
			}
	   	}
	  }

   /* FIELD 62 AMEX FRAUD INDICATOR */
   /* Bit 62 FD3 is Safekey Transaction where the network created AEVV without ACS */
   if (true == ncamex_check_for_inc_3DS2_txn(p_auth_tx) &&
		   strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0 &&
		   strncmp(p_auth_tx->TLF01_details.message_type, 0110, 4) != 0)// Add standin flag
   {
	   CHAR len_ebcdic[4]  = "";
	   CHAR F62_ebcdic[10] = "";
	   BYTE amex_csc[5] = {0};
	   BYTE F62BitMap[5] = {0};
	   BYTE Bit62[10]    = {0};
	   INT k;
	   INT len_62 = 0;

	   strncpy(len_ebcdic, p_auth_tx->future_use_1, 3);
	   if(atoi(len_ebcdic) > 0 && atoi(len_ebcdic) < 60)
	   {
	   		/*strncpy(p_auth_tx->future_use_1,HOST_MESSAGE.priv62_len,sizeof(HOST_MESSAGE.priv62_len));
	   		memcpy(p_auth_tx->future_use_1+3,HOST_MESSAGE.priv62,atoi(HOST_MESSAGE.priv62_len)); */

		   strncpy(HOST_MESSAGE.priv62_len, p_auth_tx->future_use_1, 3);
		   memcpy(HOST_MESSAGE.priv62, p_auth_tx->future_use_1+3, atoi(HOST_MESSAGE.priv62_len));
		   len_62 = atoi(len_ebcdic);

		   	   //Check if it is 4CSC
		   if( ((p_auth_tx->TLF01_details.interest_rate[0] != 'S') && strlen(p_auth_tx->TLF01_details.interest_rate) == 4) ||
		   		  ((p_auth_tx->TLF01_details.interest_rate[0] == 'S') &&
		   				  (strlen(p_auth_tx->TLF01_details.interest_rate) + strlen(p_auth_tx->TLF01_details.product_codes[18].quantity))== 5) )
		   {
			   if (p_auth_tx->current_balance_sign == 0)
			   {
				   HOST_MESSAGE.priv62[6]	^=  0x08;
				   len_62 = atoi(len_ebcdic);
				   HOST_MESSAGE.priv62[len_62]		= 0xF0;
				   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF2;
				   len_62 +=2;
			   }
			   else if (p_auth_tx->TLF01_details.cvc == 'M' ||
					   p_auth_tx->TLF01_details.cvc == '0') //check cvc flag if it is M
			   {
				   HOST_MESSAGE.priv62[6]	^=  0x08;
				   len_62 = atoi(len_ebcdic);
				   HOST_MESSAGE.priv62[len_62]		= 0xF0;
				   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF0;
				   len_62 +=2;
			   }
			   else if (p_auth_tx->TLF01_details.cvc == 'Y' ||
			   	   	   	   p_auth_tx->TLF01_details.cvc == 'N')
			   {
				   HOST_MESSAGE.priv62[6]	^=  0x08;
				   len_62 = atoi(len_ebcdic);
				   HOST_MESSAGE.priv62[len_62]		= 0xF0;
				   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF2;
				   len_62 +=2;
			   }
		   }
		   sprintf(amex_csc,"%03d", len_62);
	   }
	   else
	   {
		   //Check if it is 4CSC
		   if( ((p_auth_tx->TLF01_details.interest_rate[0] != 'S') && strlen(p_auth_tx->TLF01_details.interest_rate) == 4) ||
				  ((p_auth_tx->TLF01_details.interest_rate[0] == 'S') &&
						  (strlen(p_auth_tx->TLF01_details.interest_rate) + strlen(p_auth_tx->TLF01_details.product_codes[18].quantity))== 5) )
		   {
			   strncpy(Bit62,"AXFD3", 5);
			   for( k=0; k<5; k++ )
				{
				   F62_ebcdic[k] = ascii_ebcdic_conv (Bit62[k], A2E);

				}

			   memcpy(HOST_MESSAGE.priv62, F62_ebcdic, 5);


			   if (p_auth_tx->current_balance_sign == 0)
			   {
				   memset( F62BitMap, 0x00, 4 );
				   len_62 = 5;
				   F62BitMap[0] ^= 0x00;
				   F62BitMap[1] ^= 0x08;      /* Set bit 13 to 1 for CSC */

				   memcpy( &HOST_MESSAGE.priv62[5], F62BitMap, 4 );
				   len_62 += 4;
				   HOST_MESSAGE.priv62[len_62]		= 0xF0;
				   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF2;
				   len_62 +=2;

				   sprintf(amex_csc,"%03d", len_62);
			   }
			   else if (p_auth_tx->current_balance_sign == 1 &&
					   	   p_auth_tx->TLF01_details.cvc == 'M') //check cvc flag if it is M
			   {
			   memset( F62BitMap, 0x00, 4 );
			   len_62 = 5;
			   F62BitMap[0] ^= 0x00;
			   F62BitMap[1] ^= 0x08;     /* Set bit 13 to 1 for CSC */

			   memcpy( &HOST_MESSAGE.priv62[5], F62BitMap, 4 );
			   len_62 += 4;
			   HOST_MESSAGE.priv62[len_62]		= 0xF0;
			   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF0;
			   len_62 += 2;

			   sprintf(amex_csc,"%03d", len_62);

			   }
			   else if ( p_auth_tx->TLF01_details.cvc == 'Y'||
						   p_auth_tx->TLF01_details.cvc == 'N')
			   {

			   memset( F62BitMap, 0x00, 4 );
			   len_62 = 5;
			   F62BitMap[0] ^= 0x00;
			   F62BitMap[1] ^= 0x08;      /* Set bit 13 to 1 for CSC */

			   memcpy( &HOST_MESSAGE.priv62[5], F62BitMap, 4 );
			   len_62 += 4;
			   HOST_MESSAGE.priv62[len_62]		= 0xF0;
			   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF2;
			   len_62 += 2;

			   sprintf(amex_csc,"%03d", len_62);
			   }
			   else if ( p_auth_tx->current_balance_sign == 1 &&
					   	   p_auth_tx->TLF01_details.cvc == '0')
			   {

			   memset( F62BitMap, 0x00, 4 );
			   len_62 = 5;
			   F62BitMap[0] ^= 0x00;
			   F62BitMap[1] ^= 0x08;    /* Set bit 13 to 1 for CSC */

			   memcpy( &HOST_MESSAGE.priv62[5], F62BitMap, 4 );
			   len_62 += 4;
			   HOST_MESSAGE.priv62[len_62]		= 0xF0;
			   HOST_MESSAGE.priv62[len_62 + 1]	= 0xF0;
			   len_62 += 2;

			   sprintf(amex_csc,"%03d", len_62);
			   }
		   }
	   }

	   if(strlen(amex_csc) > 0 )
	   {
	   		memset(len_ebcdic, 0x00, sizeof(len_ebcdic));
		   	for( k=0; k<3; k++ )
	        {
	   			len_ebcdic[k] = ascii_ebcdic_conv (amex_csc[k], A2E);

	        }
	   		strncpy(HOST_MESSAGE.priv62_len,len_ebcdic,3);
	   }

   }

}/* end Build_Host_Structure */

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
/*****************************************************************************

  Function:    copy_to_req_resp_buffer

  Description: This function will copy the incoming request buffer to the 
               ReqRsp_Buffer.
  Author: 
      unknown
  Inputs:
      indata - incoming request buffer.
      msglen - the length of indata.
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
void copy_to_req_resp_buffer (pCHAR indata, INT  msglen   )
{
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   buflen = msglen;
   reqidx = 0; 
   if (msglen > COMM_BUF_SZE)
   {
      memcpy(ReqRsp_Buffer, indata, COMM_BUF_SZE);
   }
   else
   {
      memcpy(ReqRsp_Buffer, indata, msglen);
   }
} /* end copy_to_req_resp_buffer */

/*****************************************************************************

  Function:    get_tran_type_2

  Description: This function will get the transaction type using the HOST_MESSAGE
               structure
  Author: 
      unknown
  Inputs:
      tx_key - the current transaction type
      HOST_MESSAGE (global) - the request buffer containing the message from the host
  Outputs:
      None
  Return values:
      BYTE - the calculated transaction type
  Modified by:
      FArad     8/13/98
******************************************************************************/
BYTE  get_tran_type_2(BYTE tx_key) 
{
   BYTE  ret_val;

   if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1110" ) )  
   {
	   if(0==strcmp(HOST_MESSAGE.proccode, "200000" ))
	   {
		   ret_val = AUTH_REFUND_RESPONSE;
	   }
	   else
	   {
      ret_val = AUTH_AUTHORIZATION_RESPONSE;
	   }
   }

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1130" ) )
      ret_val = AUTH_ADVICE_RESPONSE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1210" ) )
      ret_val = AUTH_CASH_ADVANCE_RESPONSE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1430" ) )
      ret_val = AUTH_REVERSAL_ADVICE_RESPONSE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1814" ) )
      ret_val = AUTH_LOGON_RESPONSE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1314" ) )
      ret_val = AUTH_CARD_UPDATE_RESPONSE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "0810" ) )
      ret_val = AUTH_LOGON_RESPONSE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1100" ) )
   {
	   if(0==strncmp(HOST_MESSAGE.proccode, "20" ,2))
	   {
		   ret_val = AUTH_REFUND;
	   }
	   else
	   {
         ret_val = AUTH_AUTHORIZATION;
	   }
   }
   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1120" ) )
      ret_val = AUTH_ADVICE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1200" ) )
      ret_val = AUTH_CASH_ADVANCE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1420" ) )
      ret_val = AUTH_REVERSAL_ADVICE;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "1804" ) )
	   ret_val = AUTH_LOGON;

   else if ( 0 == strcmp( HOST_MESSAGE.msgtype, "0800" ) )
	   ret_val = AUTH_LOGON;

  /* or a transaction that the financial server does not understand */
   else
      ret_val = AUTH_INVALID;

   return( ret_val );
}

/*****************************************************************************

  Function:    get_tran_type

  Description: This function will gets the transaction type from a
                 message based on message type and processing code. 
  Author: 
      Dennis Irby
  Inputs:
      p_auth_tx - Transaction record
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE  get_tran_type( pAUTH_TX p_auth_tx ) 
{
   INT  ProcCode;
   BYTE TranType;

   ProcCode = atoi( p_auth_tx->TLF01_details.processing_code );
   ProcCode = ProcCode / 10000;   /* Get just the 1st 2 digits. */
   TranType = p_auth_tx->TLF01_details.tx_key;

   if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1110" ) )
   {
      return( AUTH_AUTHORIZATION_RESPONSE );
   }  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      return( AUTH_AUTHORIZATION_RESPONSE );
   }  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1130" ) )
   {
      return( AUTH_ADVICE_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0130" ) )
   {
      return( AUTH_ADVICE_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1210" ) )
   {
      return( AUTH_CASH_ADVANCE_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0210" ) )
   {
      return( AUTH_CASH_ADVANCE_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1430" ) )
   {
      return( AUTH_REVERSAL_ADVICE_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0430" ) )
   {
      return( AUTH_REVERSAL_ADVICE_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1814" ) )
   {
      return( AUTH_LOGON_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      return( AUTH_LOGON_RESPONSE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1100" ) )
   {
      return( AUTH_AUTHORIZATION );
   }  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0100" ) )
   {
      return( AUTH_AUTHORIZATION );
   }  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1120" ) )
   {
      return( AUTH_ADVICE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0120" ) )
   {
      return( AUTH_ADVICE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1200" ) )
   {
      return( AUTH_CASH_ADVANCE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0200" ) )
   {
      return( AUTH_CASH_ADVANCE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1420" ) )
   {
      return( AUTH_REVERSAL_ADVICE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0420" ) )
   {
      return( AUTH_REVERSAL_ADVICE );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "1804" ) )
   {
      return( AUTH_LOGON );
   } 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0800" ) )
   {
      return( AUTH_LOGON );
   } 
 
   /* Type Unknown */
   else
      return( AUTH_INVALID );
}


/*****************************************************************************

  Function:    str_asc_to_bcd

  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      p_astr - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad     8/14/98
******************************************************************************/
static  void str_asc_to_bcd (p_astr, how_many, bstring)
   pCHAR        p_astr;
   INT          how_many;
   BYTE			 bstring[];
{
   UINT  i = 0;
   UINT  start;

   if (strlen(p_astr) > (UINT)how_many)
      start = strlen(p_astr) - how_many;
   else
      start = 0;

   //for (i = (strlen(p_astr) - how_many); i < strlen(p_astr); i=i+2)
   for (i = start; i < strlen(p_astr); i=i+2)
   { 
      /* if I hit a null terminator, quit.  */
      if ('\0' == p_astr[i])
         break;

      if (p_astr[i] != ' ')
         bstring[i/2] = ((p_astr[i]-'0')*16);

      if (((i+1) < strlen(p_astr)) && (p_astr[i+1] != ' ') )
		   bstring[i/2] += (p_astr[i+1]-'0');

   }

} /* str_asc_to_bcd */

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{  
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;
   
   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }
   
} /* asc_to_bcd */

/*****************************************************************************

  Function:    

  Description: This function will convert a variable length ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      fld_len - the length of the field
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_vbcd (afield, fld_len, bcd_buf)
   pCHAR         afield;
   INT           fld_len;   
   BYTE bcd_buf[];
{
   pCHAR astr;
   INT  i = 0;
   

   astr = afield;

   for (i = 0; i < fld_len; i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if ('\0' == astr[i])
         break;

      bcd_buf [i/2] = ((astr[i]-'0')*16);

      if ((i+1) < fld_len)
         bcd_buf [i/2] += (astr[i+1]-'0');
      else
         bcd_buf [i/2] += 0x0f;
      }
} /* asc_to_vbcd */

/*****************************************************************************

  Function:    Build_Host_Header

  Description: This function will build the AMEX header used in messages to 
               the AMEX network.
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx used to build the host header
  Outputs:
      None
  Return values:
      None
  Modified by:
      FArad    8/19/98
******************************************************************************/
/*         No header for AMEX!!!!!!!!!!!!!!
 
void Build_Host_Header (pAUTH_TX p_auth_tx)
{
   struct trandef  *pTranDef;
   BYTE            cMsgLen[2];
   BYTE            StationId[7];


   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);            // Get pointer to tran table 
   //memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   memset(&ReqRsp_Buffer[0], 0x16, 1 );   //* Msg hdr len = 22,           Field 1 
   memset(&ReqRsp_Buffer[1], 0x01, 1 );   //* Hdr format type = binary 1, Field 2 
   memset(&ReqRsp_Buffer[2], 0x02, 1 );   //* Text format is of type 2,   Field 3 

                                          //* Length of message,          Field 4 
   cMsgLen[0] = ((buflen) & 0xff00)/256;
   cMsgLen[1] =  (buflen) & 0x00ff;
   memcpy(&ReqRsp_Buffer[3], cMsgLen, 2 );

   if (AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key )
	   str_asc_to_bcd (Dest_Stat_Id, sizeof( Dest_Stat_Id )-1, &ReqRsp_Buffer[5] );
   else
	   memset(&ReqRsp_Buffer[5], 0x00, 3 );   // Dest   station Id,          Field 5 


   // Source station Id - Alternate between 2 of them.                   Field 6 
   //the next line was replaced by the following line by farad on 8/19/98.
   //the host_app_src_station_flag was not found in the new design.  It is replaced with
   //host_app.prev_src_station.
   //if ( false == blank_field( host_app_src_station_flag ) )
   if(ncf01.prev_src_station != NULL_PTR)
   {
      memset( StationId, 0x00, sizeof( StationId ) );
      //the next line was replaced by the following line by farad on 8/19/98.
	  //if ( 0 == strncmp( ftos(host_app_src_station_flag), "1", 1 ))
      if ( '1' == ncf01.prev_src_station[0] )
	  {
         strcpy( StationId, ncf01.station_id1);
         p_auth_tx->ncf01_prev_src_station[0]  = '2';
      }
      else
      {
         strcpy( StationId, ncf01.station_id2);
         p_auth_tx->ncf01_prev_src_station[0]  = '1';
      }
      str_asc_to_bcd (StationId, sizeof( StationId )-1, &ReqRsp_Buffer[8] );
   }
   else
   {
	   //memset(&ReqRsp_Buffer[8],0x00, 3);
	   memset( StationId, 0x00, sizeof( StationId ) );
	   strcpy( StationId, ncf01.station_id1);
      p_auth_tx->ncf01_prev_src_station[0]  = '2';
   }
      

   memset(&ReqRsp_Buffer[11],0x00, 1 );   // Round Trip Control Info     Field 7 
   memset(&ReqRsp_Buffer[12],0x00, 2 );   // BASE II flags,              Field 8 
   memset(&ReqRsp_Buffer[14],0x00, 3 );   // Message Status Flags,       Field 9 
   memset(&ReqRsp_Buffer[17],0x00, 1 );   // Batch Number,               Field 10
   memset(&ReqRsp_Buffer[18],0x00, 3 );   // Reserved,                   Field 11
   memset(&ReqRsp_Buffer[21],0x00, 1 );   // User Information,           Field 12
   //I do not think that I need the next line.  Farad on 8/19/98.
   //update_host_app_data();
}*/

/*****************************************************************************

  Function:    blank_string

  Description: This function will determine if a string is blank
  Author: 
      unknown
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return(false);
   }
   return(true);    
}

/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}

/*****************************************************************************

  Function:    hhutil_get_strtime

  Description: This function will get the start time
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      deststr - the destination of the start time.
  Return values:
      None
  Modified by:
******************************************************************************/
void hhutil_get_strtime ( pCHAR deststr )
{
   time_t seconds;

   seconds = time(NULL_PTR);        /* get the current date and time in seconds */
   itoa(seconds, deststr, 10);    /* convert the long integer to a string */
}

/*****************************************************************************

  Function:    hhutil_verify_pin_block

  Description: This function will determine whether or not there is a pin block
               in the auth_tx
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - pin block
      false - no pin block 
  Modified by:
      BGable      6/30/98
******************************************************************************/
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx)
{
   if(0 == strlen(p_auth_tx->TLF01_details.pin_block))
      return(false);
   if(blank_string(p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block)))
      return(false);
   return(true);
}

/*****************************************************************************

  Function:    hhutil_process_debit_transaction

  Description: This function will request pin translation.  (The pin translation
               will convert the pin that is encrypted using the terminal key to 
               a pin encrypted using the host key).
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure 
  Outputs:
      None
  Return values:
      true  - successful 
      false - an error occurred when trying to send message to security function.
  Modified by:
******************************************************************************/
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg = NULL_PTR;
   LONG      ret_code;
   CHAR	    Buffer[512]   = {0};
   CHAR	    ErrorMsg[512] = {0};
 
   p_msg = ptemsg_build_msg(MT_NSP_XLATETOANSI, 
                            ST1_NONE, 
                            ST2_NONE, 
                            security_que_name, 
                            application_que_name, 
                            (pBYTE)p_auth_tx, 
                            sizeof(AUTH_TX), 
                            0);


   
   if ( NULL_PTR == p_msg )
	{
      	ncamex_log_message( 3, 2, "Insufficient Memory to build pin translation message" , "hhutil_process_debit_transaction" , 0 ,"");
		return(false);
	}
	ret_code = pteipc_send(p_msg, security_que_name);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s", security_que_name, ErrorMsg );
      ncamex_log_message( 3, 3, Buffer, "hhutil_process_debit_transaction" , 0 ,"");

      free( p_msg );
      return(false);
	}
	
   free( p_msg );
   return(true);
}

/*****************************************************************************

  Function:    hhutil_transfer_ctl_to_auth_app

  Description: This function will transfer the auth_tx to the authorizer.  
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_host_out = NULL_PTR;
   LONG   ret_code;
   CHAR	 Buffer[512]  ={0};
   CHAR	 ErrorMsg[512]={0};

   //CHAR   amex_que_name[] = "ncamexA";

   /************************************************************************
                 REMOVE THESE LINES      */
//   strcpy(p_auth_tx->TLF01_details.response_code, "13");
//   strcpy(p_auth_tx->TLF01_details.auth_number, "343434");
//   p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);
   /************************************************************************/

 //  p_msg_host_out = ptemsg_build_msg (MT_AUTH_RESPONSE, ST1_NONE, ST2_NONE, amex_que_name, application_que_name,
   p_msg_host_out = ptemsg_build_msg (MT_AUTH_REQUEST, ST1_NONE, ST2_NONE, authorizer_que_name, application_que_name,
                                      (pBYTE)p_auth_tx, sizeof(AUTH_TX), 0);
   if ( NULL_PTR == p_msg_host_out )
	{
	   	ncamex_log_message( 3, 2, "Insufficient Memory to build host message" , "hhutil_transfer_ctl_to_auth_app" , 0 ,"");
		return(false);
	}
	ret_code = pteipc_send(p_msg_host_out, authorizer_que_name);
//	ret_code = pteipc_send(p_msg_host_out, amex_que_name);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "hhutil_transfer_ctl: error on pteipc_send to que %s: %s", authorizer_que_name, ErrorMsg );
      ncamex_log_message( 3, 2, Buffer , "hhutil_transfer_ctl_to_auth_app" , 0 ,"");
      free( p_msg_host_out );
      return(false);
	}
	
   free( p_msg_host_out );
   return(true);
}

/*****************************************************************************

  Function:    hhutil_process_host_request

  Description: This function will process a host request by filling the auth_tx
               structure with data.
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      p_auth_tx - pointer to the auth_tx structure
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BYTE hhutil_process_host_request(pAUTH_TX p_auth_tx)
{
   CHAR           ErrorMsg[200] = "";   
   pPTE_MSG       p_msg_ins = NULL_PTR;

   NCF30          ncf30;
   CHAR           Buff[512] = {0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   LONG           ret_code; 

   PRINT("hhutil_process_host_request: RECEIVED DIALOG MESSAGE\n");

   /* Tell the authorizer to check cvv */
   if ( (p_auth_tx->TLF01_details.track2[0] != 0x00) ||
        (p_auth_tx->TLF01_details.track1[0] != 0x00) ||
		(p_auth_tx->TLF01_details.interest_rate[0] != 0x00)	) 		
   {	/* TF Phani.. Add DE53(interst rate) also..As an issuer ATP needs to validate 3 CSC /4 CSC code */
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
   }
#ifdef AMEX03A
   if(true == ncamex_check_for_incremental_transaction(p_auth_tx) ||
	  true == ncamex_check_for_partial_reversal_transaction(p_auth_tx))
   {
	   /* no need to check for the original transaction for partial reversal and incremental auth, send it to txcntl,
	    * original check will happen in nposa2 if vertexon is offline
	    * Original check should happen only during STIP
	    * */
	   if (false == hhutil_transfer_ctl_to_auth_app(p_auth_tx))
	   {
	         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	         sprintf(Log_Bufr," Unable to send the request to the authorizor application Term Id:%s ,Merch Id: %s RRN: %s ",
	         		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	         ncamex_log_message( 3, 2, Log_Bufr , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
	         return(false);
	   }
	   return(true);

   }
#endif
   else if ( (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key) ||
        (AUTH_ADVICE          == p_auth_tx->TLF01_details.tx_key)  )
   {
      if ( AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key )
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0420");

         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( Buff, 0x00, sizeof(Buff) );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr,"Reversal from Amex: Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff ,Log_Bufr);
         ncamex_log_message( 2, 1, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

      }
      else
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0120");
         //strcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,p_auth_tx->TLF01_details.sys_trace_audit_num );
      }

      /* Must get the original transaction for reversals and advices.
       * If the original transaction is not found, reply to the host
       * and don't send the transaction to the authorizer, except for
       * for advices that were approved by the host.
       */
      memset( &ncf30, 0x00, sizeof(NCF30) );
      strcpy( ncf30.primary_key.network_id,    AppName ); 
      strcpy( ncf30.primary_key.network_type, "A"      ); 
      if (0 != strlen(p_auth_tx->TLF01_details.orig_sys_trace_num))
      {
         strcpy( ncf30.primary_key.batch_number, "ACTIVE" ); 
         strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
         strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
         strcpy( ncf30.sys_trace_audit_nbr,
                 p_auth_tx->TLF01_details.orig_sys_trace_num );

         /* Build message to get the original transaction from data server. */
         memcpy( buffer1,p_auth_tx, sizeof(AUTH_TX) );
         memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );

         p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                       ST1_DB_SELECT, 
                                       ST2_DB_SELECT_TRAN_BY_STAN,
                                       netds_que_name, 
                                       application_que_name,
                                       (pBYTE)&buffer1,
                                       sizeof(buffer1), 
                                       NCF30_DATA );
      }
      else
      {
         strcpy( ncf30.card_nbr,      p_auth_tx->TLF01_details.card_num      );
         strcpy( ncf30.tran_amount,   p_auth_tx->TLF01_details.total_amount  );
         strcpy( ncf30.merchant_id,   p_auth_tx->TLF01_details.merchant_id   );
         strcpy( ncf30.currency_code, p_auth_tx->TLF01_details.currency_code );

         /* Build message to get the original transaction from data server. */
         memcpy( buffer1,p_auth_tx, sizeof(AUTH_TX) );
         memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );

         p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                       ST1_DB_SELECT, 
                                       ST2_DB_SELECT_TRAN_BY_MERCHANT_ID,
                                       netds_que_name, 
                                       application_que_name,
                                       (pBYTE)&buffer1,
                                       sizeof(buffer1), 
                                       NCF30_DATA );
      }
      if ( NULL_PTR == p_msg_ins  )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Insufficient Memory for select ncf30 message Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncamex_log_message( 3, 2, Log_Bufr , "2" , 0 ,p_auth_tx->TLF01_details.card_num);
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
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff ,Log_Bufr);
         ncamex_log_message( 3, 2, Buff , "hhutil_process_host_request " , 0 ,p_auth_tx->TLF01_details.card_num);
         return( false );
      }
      return( true );
   }
   else  if (false == hhutil_transfer_ctl_to_auth_app(p_auth_tx))
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to send the request to the authorizor application Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   return(true);
}

/*****************************************************************************

  Function:    hhutil_send_host_response

  Description: This function will send the host response message out to the 
               AMEX host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure, used to get the tpdu_id 
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_out;
   LONG   ret_code;
   CHAR	 Buffer[512]={0};
   CHAR	 ErrorMsg[512]={0};

   ptetime_get_strtime(p_auth_tx->TLF01_details.tran_finish_time);
   p_msg_out = ptemsg_build_msg(MT_OUTGOING,
	         		            ST1_NONE,
						            ST2_NONE,
						            p_auth_tx->TLF01_details.originator_queue,
						            application_que_name,
                              ReqRsp_Buffer,
                              buflen,
						            0); 

   if ( NULL_PTR == p_msg_out )
	{
       ncamex_log_message( 3, 2, "Insufficient Memory to build host message" , "hhutil_send_host_response" , 0 ,"");
	   return(false);
	}
   (void)set_originator_info( p_auth_tx, p_msg_out );
	ret_code = pteipc_send(p_msg_out, p_auth_tx->TLF01_details.originator_queue);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s",
               p_auth_tx->TLF01_details.originator_queue, ErrorMsg );
      ncamex_log_message( 3, 3, Buffer , "hhutil_send_host_response" , 0 ,"");
      free( p_msg_out );
      return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   free( p_msg_out );
   return(true);
}


/*****************************************************************************

  Function:    incoming_other_host_response

  Description: This function will process an incoming response from another host
               that needs to go to the AMEX host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx of the response message
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
INT incoming_other_host_response(pAUTH_TX p_auth_tx)
{
   INT    ret_code;
   NCF30  ncf30;

   PRINT("incoming_host_response\n");

   /* Responses to reversals and advices must be approved. */
   if ( (AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key )||
        (AUTH_REVERSAL_RESPONSE        == p_auth_tx->TLF01_details.tx_key )|| 
        (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)  )

   {
      strcpy( p_auth_tx->TLF01_details.response_code, "00" );
      memset( &ncf30, 0x00, sizeof(NCF30) );
      strcpy( ncf30.primary_key.network_id, AppName );
      strcpy( ncf30.primary_key.network_type, "A" );
      strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
      strcpy( ncf30.primary_key.retrieval_ref_num,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num );

      ret_code = delete_orig_ncf30( ncf30, p_auth_tx );
      if ( false == ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to delete reversal's original txn in NCF30 Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncamex_log_message( 3, 2, Log_Bufr , "incoming_other_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      }

   }

   //else if ( AUTH_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key )
    //  return( true );

  
   /* Continue to send response to host. */
   return( incoming_other_host_response_db_reply( p_auth_tx ) );
}

/******************************************************************************
 *
 *  NAME:         incoming_other_host_response_db_reply
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_auth_tx.  This function picks
 *                up where 'incoming_other_host_response' left off.  You come
 *                here after updating NCF21 and NCF30 if they required updating.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *                
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx)
{
   CHAR       temp[17] = "";
   CHAR       err_mesg[200] = "";
   CHAR       amex_response_code[4];
   INT        total_int;INT buf_temp_len=0;
   INT        reversal_int;
   INT        actual_int;
   CHAR       temp_amount[13];
   CHAR       temp_cur_code[4];
	  
// TF Phani .. Dont change Action code..For cadience send as it is
   if(0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"nposa2",6) ||
	  0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"natma2",6) 
	  )
   {
	    /* Copy response code in case of rejection, Girija Y TF, Feb 24, 2009 */
	    if( p_auth_tx->accptor_bus_code[0] == 0x00 )
		{
			strcpy(p_auth_tx->accptor_bus_code,p_auth_tx->TLF01_details.response_code);
			match_ISO_to_AMEX_response_code(p_auth_tx, amex_response_code);
			if (0 == strlen(p_auth_tx->TLF01_details.product_codes[12].code))
			{
				memcpy( p_auth_tx->TLF01_details.product_codes[12].code,
							amex_response_code,
							sizeof(amex_response_code));
			}
		}
	    else
		{
			strcpy(amex_response_code,p_auth_tx->accptor_bus_code);
		}
   }
   else
   {
		match_ISO_to_AMEX_response_code(p_auth_tx, amex_response_code);
		memcpy( p_auth_tx->TLF01_details.product_codes[12].code,
				amex_response_code,
				sizeof(amex_response_code));

   }

   if (AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
	   if(0 == strncmp(p_auth_tx->function_code,"401",3) &&
			   0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5))
	   {

	   }
	   else if (0 == strlen(p_auth_tx->forwarding_inst_id_cd))
      {
         /* Indicates POS reversal (not ATM reversal) */
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_REPEAT_RESPONSE;
      }
   }
    if (AUTH_REVERSAL_RESPONSE== p_auth_tx->TLF01_details.tx_key)
   {
       /* Indicates POS reversal Advice response */
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_REPEAT_RESPONSE;
   }
  
	   strncpy(temp_cur_code, p_auth_tx->TLF01_details.currency_code,
			   sizeof(p_auth_tx->TLF01_details.currency_code));
	   strncpy(temp_amount, p_auth_tx->TLF01_details.total_amount,
			   sizeof(p_auth_tx->TLF01_details.total_amount));

	   strncpy(p_auth_tx->TLF01_details.currency_code,
			   p_auth_tx->reconciliation_cur_code,
			   sizeof(p_auth_tx->TLF01_details.currency_code));
	   strncpy(p_auth_tx->TLF01_details.total_amount,
			   p_auth_tx->reconciliation_amount,
			   sizeof(p_auth_tx->TLF01_details.total_amount));

   
   build_host_structure(p_auth_tx, amex_response_code);
   
   if (true == Build_Host_Request(p_auth_tx))
   {
   	 /* Forward Msg coming from Cadience HOST */
		 if(strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0 &&
		 	(0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"nposa2",6) ||
	  		 0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"natma2",6) )
	  		)
		{
			buf_temp_len = atoi(p_auth_tx->resp_data_len);
			if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
			{	buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,p_auth_tx->resp_buffer,buflen);
			}
		 }
      if(false == hhutil_send_host_response(p_auth_tx))
	  {
		  //HCTS1-A
		  if(p_auth_tx->TLF01_details.tx_key != AUTH_LOGON &&
			p_auth_tx->TLF01_details.tx_key != AUTH_LOGON_RESPONSE)
		  {
			  insert_tlf01( p_auth_tx );
		  }
		  return(false);
	  }
   }
   else
   {
      ncamex_log_message( 3, 2, "Error building response for the host" , "incoming_other_host_response_db_reply" , 0 ,"");
      return(false);
   }
   
    if(0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"nposa2",6) ||
			0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"natma2",6) 
	       )
		 {
			 Tlf_Logging_ncamex_cadience(p_auth_tx);
	
		 }
	else
	{
 	   strncpy(p_auth_tx->TLF01_details.currency_code,temp_cur_code,
			   sizeof(p_auth_tx->TLF01_details.currency_code));
	   strncpy(p_auth_tx->TLF01_details.total_amount,temp_amount,
			   sizeof(p_auth_tx->TLF01_details.total_amount));
	}

   /* If ATM reversal */
   if (AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      reversal_int = atoi(p_auth_tx->TLF01_details.reversal_amount);
      actual_int = atoi(p_auth_tx->TLF01_details.actual_amount);
      total_int = reversal_int + actual_int;
      itoa(total_int, p_auth_tx->TLF01_details.total_amount,10);
      Rj_with_lead_zeros(p_auth_tx->TLF01_details.total_amount, 12);
      strcpy(p_auth_tx->TLF01_details.currency_code, PESO);
   }

   /* If POS reversal */
   if (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      /* Make it look like a regular reversal to the DB */
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   }

   if (AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      return(true);
   }

   insert_tlf01( p_auth_tx );
   return(true);
}

/**/

INT incoming_other_host_response_on_error( pAUTH_TX p_auth_tx,CHAR amex_response_code[4])
{
   CHAR       temp[17] = "";
   CHAR       err_mesg[200] = "";
   CHAR           unique_tran_str_id[17] = "";
   INT            length,size,i;
   INT			return_type = 0;   // 0 - for true
								   // 1 - for false

   if (AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      if (0 == strlen(p_auth_tx->forwarding_inst_id_cd))
      {
         /* Indicates POS reversal (not ATM reversal) */
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_REPEAT_RESPONSE;
      }
   }


   strncpy(p_auth_tx->TLF01_details.currency_code,
           p_auth_tx->reconciliation_cur_code,
           sizeof(p_auth_tx->TLF01_details.currency_code));
   strncpy(p_auth_tx->TLF01_details.total_amount,
           p_auth_tx->reconciliation_amount,
           sizeof(p_auth_tx->TLF01_details.total_amount));

   strcpy(p_auth_tx->TLF01_details.tpdu_id, tpdu_id);
   build_host_structure(p_auth_tx, amex_response_code);
   
   if (true == Build_Host_Request(p_auth_tx))
   {
      if(false == hhutil_send_host_response(p_auth_tx))
	  {

		  return_type = 1; // false
         //return(false);
	  }
   }
   else
   {
      ncamex_log_message( 3, 2, "Error building response for the host" , "incoming_other_host_response_on_error" , 0 ,"");
	  return_type = 1; // false
      //return(false);
   }
   
   
	if(strcmp(amex_response_code,"183") == 0)
	{
		  strcpy(p_auth_tx->TLF01_details.response_code,"IC");
	}
	else if(strcmp(amex_response_code,"101") == 0)
	{
		  strcpy(p_auth_tx->TLF01_details.response_code,"54");
	}
	else if(strcmp(amex_response_code,"181") == 0)
	{
		  strcpy(p_auth_tx->TLF01_details.response_code,"30");
	}
	else if(strcmp(amex_response_code,"111") == 0)
	{
		 strcpy(p_auth_tx->TLF01_details.response_code,"14");
	}
	else if(strcmp(amex_response_code,"110") == 0)
	{
		  strcpy(p_auth_tx->TLF01_details.response_code,"13");
	}
   


   /* generate the TXN ID */

   memset(unique_tran_str_id, 0, sizeof(unique_tran_str_id));
   ptetime_get_strtime(unique_tran_str_id);

   ptetime_get_strtime(p_auth_tx->TLF01_details.tran_start_time);
   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
          sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id,
          p_auth_tx->TLF01_details.terminal_id,
          sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
   strncat(p_auth_tx->TLF01_details.primary_key.transaction_id,
           unique_tran_str_id+2, 12);
   if ((length = strlen(p_auth_tx->TLF01_details.primary_key.transaction_id)) <
       (size = sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id)))
   {
      for (i = length; i < (size-1); i++)
         p_auth_tx->TLF01_details.primary_key.transaction_id[i] = '0';
   }

   insert_tlf01( p_auth_tx );

   if(return_type == 1)
		return (false);
   return(true);
}

/*****************************************************************************

  Function:   hhutil_parse_db_ok

  Description: This function determines which type of database request the
               incoming message is from and routes it to the correct function.
  Author: 
      BGable   7/13/98
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Modified by:

******************************************************************************/
void     hhutil_parse_db_ok(pPTE_MSG p_msg_in)
{
   pBYTE pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE  subtype1;
   BYTE  subtype2;
   BYTE  app_data_type;
   AUTH_TX auth_tx;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);
   
   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2 = ptemsg_get_msg_subtype2(p_msg_in);

   if (ST1_DB_SELECT == subtype1)
   {
	   if (NCF30_DATA == app_data_type)
      {
         /*  The reversal or advice record was found,
          *  send the transaction to the authorizer.
          */
         if (false == process_incoming_message_ncf30_reply(pCurrent))
         {
            ncamex_log_message( 3, 3, "Unable to send the request to the authorizor application" , "hhutil_parse_db_ok" , 0 ,"");
         } 
      }
      else
      {
         ncamex_log_message( 3, 2, "Invalid incoming select message type" , "hhutil_parse_db_ok" , 0 ,"");
      }
   }
   else if (ST1_DB_INSERT == subtype1)
   {
      if (TLF01_DATA == app_data_type)
      {
      }
      /* This is a reply to an insert TLF01 */
   }
   else if (ST1_DB_DELETE == subtype1)
   {
   }
   else if (ST1_DB_UPDATE == subtype1)
   {
      if (ALL_DATA == app_data_type)
      {
          memset( &auth_tx, 0,        sizeof(AUTH_TX) );
          memcpy( &auth_tx, pCurrent, sizeof(AUTH_TX) );
             
         /* if ( (0 == strncmp( "0130", auth_tx.TLF01_details.message_type, 4)) ||( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE ) )
          {
			     /* 
				 Already sent the respone to host.
				 Already logged into TLF01.
				 */
               /*  return; 
           }*/
          // else
           //{
				if(false == incoming_other_host_response_db_reply( &auth_tx ))
				{
			         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			         sprintf(Log_Bufr," Unable to send response to Amex host Term Id:%s ,Merch Id: %s RRN: %s ",
			        		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
					 ncamex_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
				}
           //}         
      }
   }
   else
   {
      ncamex_log_message( 3, 2, "Invalid incoming message type " , "hhutil_parse_db_ok" , 0 ,"");
   }
} /* hhutil_parse_db_ok */

/*****************************************************************************

  Function:   hhutil_parse_db_error

  Description: This function determines which type of database request the
               incoming error message is from and routes it to the correct
               function.
  Author: 
      BGable   7/13/98
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Modified by:
      
******************************************************************************/
void hhutil_parse_db_error(pPTE_MSG p_msg_in)
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE          app_data_type;
   BYTE          subtype1;
   BYTE          subtype2;
   CHAR          error_msg[512] = {0};
   AUTH_TX       auth_tx;
   CHAR          Buffer[512]={0};
   NCF30         ncf30;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);

   if (pCurrent != NULL)
   {
      memset(&auth_tx, 0, sizeof(auth_tx));
      memcpy(&auth_tx, pCurrent, sizeof(auth_tx));
      strncpy (error_msg, pCurrent+(sizeof(auth_tx)),190);

      app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
      subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
      subtype2 = ptemsg_get_msg_subtype2(p_msg_in);

      if (ST1_DB_SELECT == subtype1)
      {
    	  if (NCF30_DATA == app_data_type)
         {
            /* The original transaction for an 
             * advice or reversal was not found.
             */
            if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE ||
            	 auth_tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT)
            {
               memset( &ncf30, 0x00, sizeof(NCF30) );
               (void)process_auth_advices( &auth_tx, NOT_FOUND, ncf30 );
            }
            else
            {
               /*TF - ABHISHEK Changes for 420 messges*/
			   
				/*Reversal advice messages should be sent to Cadencie as long
				as Cadencie is online even original transaction for reversal was not found.

				If Cadencie is online, let Cadencie respond to the reversal
				advice messages.  It is only when Cadencie is offline and ATP is processing
				the Amex transactions on behalf of Cadencie than ATP should reply to the
				reversal advice messages..	 */
			
            	auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
            	hhutil_transfer_ctl_to_auth_app(& auth_tx );
            }
         }
      }
      else if ( ST1_DB_UPDATE == subtype1 )
      {
         if ( ALL_DATA == app_data_type )
         {
            /* This function will send response to host and log to TLF01. */
            memset( &auth_tx, 0,        sizeof(AUTH_TX) );
            memcpy( &auth_tx, pCurrent, sizeof(AUTH_TX) );
			memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			sprintf(Log_Bufr,"  Term Id:%s ,Merch Id: %s RRN: %s ",
					 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
			sprintf( Buffer,
						"Problems with updating the database tables: %s",
						 error_msg );
			strcat(Buffer,Log_Bufr);
			ncamex_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);

            if ((0 == strncmp("0130",auth_tx.TLF01_details.message_type, 4)) ||
                ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE ) )
            {
               /* Already sent the respone to host.
                * Already logged into TLF01.
                */
               return;
            }
            strcpy( auth_tx.TLF01_details.response_code, "96" );
            if(false == incoming_other_host_response_db_reply( &auth_tx ) )
            {
    			memset(Log_Bufr,0x00,sizeof(Log_Bufr));
    			sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
    					 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               strcpy(Buffer,"Unable to send response to Amex host." );
               ncamex_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }
      }
      else
      {
         if (TLF01_DATA == app_data_type)
         {
            sprintf(Buffer,"Insert TLF01 error: %s",error_msg);
            ncamex_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,"");
         }
      }
   }
} /*hhutil_parse_db_error*/

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

      strcpy (temp_str, p_data+sizeof(AUTH_TX));
		sprintf( error_msg, "Database Error: %s", temp_str );
		free( *p_msg_in );
	}

	return dberror;
}

/*****************************************************************************

  Function:    hhutil_get_ncf01_cfg

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously and should be used only by
               amexmain.c!!!
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

   memcpy(buffer,&auth_tx, sizeof(AUTH_TX));
   memcpy(buffer+sizeof(AUTH_TX),pncf01, sizeof(NCF01));

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

   memset (pncf01, 0, sizeof(NCF01));
   memcpy(pncf01, p_data+(sizeof(auth_tx)), sizeof(NCF01));
	free( p_msg_in );
	return ( PTEMSG_OK );
}

/*****************************************************************************

  Function:    get_crf01

  Description: This function will send a request to the dataserver to select 
               CRF01.  This is done syncronously.

  Author:      D. Irby
      
  Inputs:      pcrf01 - pointer to the requested crf01 structure

  Outputs:     pcrf01 - pointer to the returned crf01 structure

  Return values:  0 = Success
                 -1 = Failure

******************************************************************************/
LONG get_crf01( pCRF01 pcrf01 )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           retval = 0;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(CRF01)];
   CHAR           error_msg[512] = {0};

   memset( buffer, 0x00, sizeof(buffer) );
   memcpy( buffer+sizeof(AUTH_TX), pcrf01,  sizeof(CRF01)   );

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                (pBYTE) buffer,
                                 sizeof( buffer),
                                 CRF01_DATA );

   rcode = hhutil_process_request(p_msg_out,&p_msg_in,trands_que_name,error_msg);
   if ( rcode == PTEMSG_OK )
   {
      p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
      p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

      memset( pcrf01, 0x00, sizeof(CRF01));
      memcpy( pcrf01, p_data+sizeof(AUTH_TX), sizeof(CRF01) );
      free( p_msg_in );
   }
   else
   {
      retval = -1;
      ncamex_log_message( 3, 2, error_msg , "get_crf01" , 0 ,"");
   }
   return( retval );
}

INT isnum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}

INT isalphanum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}


INT isalphanumSpace(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
      {
       if( string[i] != ' ')
           return(false);
      }
   }
   return(true);
}




INT sendBkToHost(pAUTH_TX p_auth_tx,CHAR amex_response_code[4])
{
       p_auth_tx->TLF01_details.tx_key = get_tran_type_2(p_auth_tx->TLF01_details.tx_key);
       p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);
       incoming_other_host_response_on_error( p_auth_tx,amex_response_code);
       return true;
}


INT verify_auth_tx_data(pAUTH_TX p_auth_tx)
{

   CHAR  Buffer[512] = {0};
   
   
   /* FIELD 2: PAN */
   if( false == isnum(p_auth_tx->TLF01_details.card_num))
   {
      sprintf(Buffer, "Invalid data for card number: %s", p_auth_tx->TLF01_details.card_num);

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      //111
      sendBkToHost( p_auth_tx,"111");
      return(false);
   }
   /* FIELD 3: PROCESSING CODE */
   if( false == isnum(p_auth_tx->TLF01_details.processing_code))
   {
      sprintf(Buffer, "Invalid data for processing code: %s", p_auth_tx->TLF01_details.processing_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      //181
      sendBkToHost( p_auth_tx,"181");
      return(false);
   }
   if ((0 == strcmp(p_auth_tx->TLF01_details.message_type,"1200")) ||
       (0 == strcmp(p_auth_tx->TLF01_details.message_type,"1100")) ||
       (0 == strcmp(p_auth_tx->TLF01_details.message_type,"0200")) ||
       (0 == strcmp(p_auth_tx->TLF01_details.message_type,"0100"))  )
      if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"00", 2))
         if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"01", 2))
            if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"17",2))
            	if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"20",2))
#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
            		if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"33",2))
#endif
            {
               sprintf(Buffer, "Invalid data for processing code: %s", p_auth_tx->TLF01_details.processing_code);
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(Buffer,Log_Bufr);
               ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
               sendBkToHost( p_auth_tx,"111");
               return(false);
            }
   /* FIELD 4: TRANSACTION AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.total_amount))
   {
      sprintf(Buffer, "Invalid data for total amount: %s", p_auth_tx->TLF01_details.total_amount);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      //110
      sendBkToHost( p_auth_tx,"110");
      return(false);
   }
   /* FIELD 5: SETTLEMENT AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.settlement_total))
   {
      sprintf(Buffer, "Invalid data for settlement total: %s", p_auth_tx->TLF01_details.settlement_total);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      //110
      sendBkToHost( p_auth_tx,"110");
      return(false);
   }
   /* FIELD 7: TRANSMISSION DATE AND TIME */
   if( false == isnum(p_auth_tx->TLF01_details.transmission_timestamp))
   {
      sprintf(Buffer, "Invalid data for transmission timestamp: %s", p_auth_tx->TLF01_details.transmission_timestamp);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");
      return(false);
   }
   /* FIELD 11: STAN */
   if( false == isnum(p_auth_tx->TLF01_details.sys_trace_audit_num))
   {
      sprintf(Buffer, "Invalid data for system trace audit number: %s", p_auth_tx->TLF01_details.sys_trace_audit_num);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");
      return(false);
   }
   /* FIELD 12: LOCAL TIME */
   if( false == isnum(p_auth_tx->TLF01_details.date_yyyymmdd))
   {
      sprintf(Buffer, "Invalid data for local time");
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   if( false == isnum(p_auth_tx->TLF01_details.time_hhmmss))
   {
      sprintf(Buffer, "Invalid data for local time");
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 13: EFFECTIVE DATE */
   if( false == isnum(p_auth_tx->effective_date))
   {
      sprintf(Buffer, "Invalid data for effective date: %s", p_auth_tx->effective_date);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 14: EXPIRATION DATE */
   if( false == isnum(p_auth_tx->TLF01_details.exp_date))
   {
      sprintf(Buffer, "Invalid data for expiration date: %s", p_auth_tx->TLF01_details.exp_date);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 19: ACQ INST COUNTRY CODE */
   if( false == isnum(p_auth_tx->country_code))
   {
      sprintf(Buffer, "Invalid data for acqiring institution country code: %s", p_auth_tx->country_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 24: FUNCTION CODE */
   if( false == isalphanum(p_auth_tx->function_code))
   {
      sprintf(Buffer, "Invalid data for function code: %s", p_auth_tx->function_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 26: CARD ACCEPTOR BUSINESS CODE */
   if( false == isnum(p_auth_tx->TLF01_details.category_code))
   {
      sprintf(Buffer, "Invalid data for category code: %s", p_auth_tx->TLF01_details.category_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 32: ACQUIRING INST ID CODE */
   if( false == isnum(p_auth_tx->acq_inst_id_cd))
   {
      sprintf(Buffer, "Invalid data for acquiring institution ID code: %s", p_auth_tx->acq_inst_id_cd);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);

   }
   /* FIELD 33: FORWARDING INST ID CODE */
   if( false == isnum(p_auth_tx->forwarding_inst_id_cd))
   {
      sprintf(Buffer, "Invalid data for forwarding institution ID code: %s", p_auth_tx->forwarding_inst_id_cd);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 37: RRN */
   if( false == isalphanumSpace(p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      sprintf(Buffer, "Invalid data for retrieval reference number: %s", p_auth_tx->TLF01_details.retrieval_ref_num);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 38: AUTH CODE */
   if( false == isalphanumSpace(p_auth_tx->TLF01_details.auth_number))
   {
      sprintf(Buffer, "Invalid data for approval code: %s", p_auth_tx->TLF01_details.auth_number);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 39: REPONSE CODE */
   if( false == isnum(p_auth_tx->TLF01_details.response_code))
   {
      sprintf(Buffer, "Invalid data for action code: %s", p_auth_tx->TLF01_details.response_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 49: CURRENCY CODE, TRANSACTION */
   if( false == isnum(p_auth_tx->TLF01_details.currency_code))
   {
      sprintf(Buffer, "Invalid data for transaction currency code: %s", p_auth_tx->TLF01_details.currency_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   /* FIELD 50: CURRENCY CODE, SETTLEMENT */
   if( false == isalphanum(p_auth_tx->settlement_curr_code))
   {
      sprintf(Buffer, "Invalid data for settlement currency code: %s", p_auth_tx->settlement_curr_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      sendBkToHost( p_auth_tx,"181");      
      return(false);
   }
   return(true);
}

INT insert_tlf01(pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   LONG       ret_code;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};
   CHAR       amex_response_code[4] = {0};

   /* Check to see if its already been inserted into TLF01. */
   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
      return(true);

   // Only for STIP transaction we shall copy the Action code here
   if(0 == strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN))
   {
	   if ( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1120",4)) ||
	        (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4)) ||
	        (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1314",4)) ||
			(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4)))
	   {
		   match_AMEX_to_ISO_response_code(p_auth_tx, amex_response_code);
		   memcpy( p_auth_tx->TLF01_details.product_codes[12].code,
				   amex_response_code,
				   sizeof(amex_response_code));
	   }
   }

   p_auth_tx->host2_acquirer_cb_code[0] = '*';
   	/*TF PHANI... Tempary fix for Amex 5CSC, since interst_rate filed has only 4 bytes, we need to fill null at end*/
   memset(&p_auth_tx->TLF01_details.interest_rate[4],'\0',1);
   memset(buffer, 0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),&p_auth_tx->TLF01_details, sizeof(TLF01));

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
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build insert TLF01 message Term Id:%s ,Merch Id: %s RRN: %s ",
      			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "insert_tlf01" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   ret_code = pteipc_send(p_msg_ins, updateds_que_name); 
   if(ret_code < 0)
   {
      pteipc_get_errormsg(ret_code, err_mesg);
      sprintf(Buff, "failed transmit to que: %s. %s",
              updateds_que_name, err_mesg);
      ncamex_log_message( 3, 3, Buff , "insert_tlf01" , 0 ,"");
      free(p_msg_ins);
      return(false);
   }
   free(p_msg_ins);
   return(true);
}

/*****************************************************************************

  Function:    INIT_TXN

  Description: This function initializes auth_tx for switched in transactions.

  Author:      D. Irby

  Inputs:      None

  Outputs:     p_auth_tx - Initialized auth_tx

  Return values:
      Always returns true
******************************************************************************/
INT init_txn( pAUTH_TX p_auth_tx )
{
   CHAR           unique_tran_str_id[21] = "";
   CHAR           temp[17] = "";
   INT            length;
   INT            size;
   INT            i;
   CHAR   temp_unique_tran[15]="";
   INT	  tid_len = 0;
   INT	  unique_tran_str_id_len=0;

   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type     = FINANCIAL;

   strcpy(p_auth_tx->TLF01_details.handler_queue, application_que_name);

   strcpy(p_auth_tx->TLF01_details.dispensation.origin,"RO");
     
   /* Set local date and time. */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ) )
   {
      genutil_format_date(temp);
      memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, 8); 
      memcpy(p_auth_tx->TLF01_details.time_hhmmss, temp + 8, 6);
   }
   /*  Create transaction id, if it doesn't fill the array   */
   
   memset(unique_tran_str_id, 0x00, sizeof(unique_tran_str_id));
   ptetime_get_strtime(unique_tran_str_id);
   strncpy( p_auth_tx->TLF01_details.tran_start_time, unique_tran_str_id,16 );

   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0x00,
          sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
   if(tid_len == 8)
   {
  		 memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id,
          		p_auth_tx->TLF01_details.terminal_id,
          		sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
  		 strncat(p_auth_tx->TLF01_details.primary_key.transaction_id,
          		 unique_tran_str_id+2, 12);
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
				  p_auth_tx->TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
		 strncat (unique_tran_str_id,temp_unique_tran,14);
		 
		 unique_tran_str_id_len= strlen(unique_tran_str_id);
		 memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
				 p_auth_tx->TLF01_details.terminal_id, tid_len );
		 strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				  unique_tran_str_id+tid_len, 
				  (unique_tran_str_id_len-tid_len) );
		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
	}
 
   /* Set the message type for ISO8583. */
   if (AUTH_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0100");
   }
   else if (AUTH_CASH_ADVANCE == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0200");
   }
   else if (AUTH_LOGON == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0800");
   }

   return( true );
}


/*****************************************************************************

  Function:    VALIDATE_HOST_MSG

  Description: This function verifies a message from host is a message
               that is meant to get through to the network controller.

  Author:      D. Irby

  Inputs:
      None
  Outputs:
      p_auth_tx - Initialized auth_tx
  Return values:
      Always returns true
******************************************************************************/
INT validate_host_msg( )
{
   INT   ret_val = true;

   /* Verify the length of the message is at least as
    * as long as the shortest expected message.
    */
   if ( buflen < SHORTEST_MSG_LEN )
      ret_val = false;

   /* Verify 1st four characters are an expected message type. */
   else
   {
      /* First byte is always an EBCDIC '1'. */
      if ( ReqRsp_Buffer[0] != 0xf1 )
         ret_val = false;

      /* Second byte is always 1, 2, 3, 4, 8. */
      else if ( (ReqRsp_Buffer[1] != 0xf1) &&
                (ReqRsp_Buffer[1] != 0xf2) &&
                (ReqRsp_Buffer[1] != 0xf3) &&
                (ReqRsp_Buffer[1] != 0xf4) &&
                (ReqRsp_Buffer[1] != 0xf8) )
         ret_val = false;

      /* Third byte is 0, 1, 2, or 3. */
      else if ( (ReqRsp_Buffer[2] != 0xf0) &&
                (ReqRsp_Buffer[2] != 0xf1) &&
                (ReqRsp_Buffer[2] != 0xf2) &&
                (ReqRsp_Buffer[2] != 0xf3) )
         ret_val = false;

      /* Fourth byte is always 0 or 4. */
      else if ( (ReqRsp_Buffer[3] != 0xf0) &&
                (ReqRsp_Buffer[3] != 0xf4) )
         ret_val = false;
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
   CHAR            Buff[512] = {0};
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
   CHAR err_buf[512]={0};

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
         ncamex_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         ncamex_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            ncamex_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         GET_CURRENT_DATE_TIME
 *
 *  DESCRIPTION:  This function gets the system date and time.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      system_date - System date in yyyymmdd format
 *                system_time - System time in hhmmss format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void get_current_date_time( pCHAR system_date, pCHAR system_time )
{
   CHAR   date_time[17] = "";

   genutil_format_date( date_time );
   memcpy( system_date, date_time,     8 ); 
   memcpy( system_time, date_time + 8, 6 );

   return;
}
/******************************************************************************
 *
 *  NAME:         GET_CURRENT_DATE_TIME
 *
 *  DESCRIPTION:  This function gets the system date and time.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      system_date - System date in yyyymmdd format
 *                system_time - System time in hhmmss format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void Tlf_Logging_ncamex_cadience( pAUTH_TX p_auth_tx)
{

 /* Here is the reqerirement For Currency Logging-- Kenneth.. TF PHANI
   
	A)If transaction currency is dollar, then total_amount and currency_code should be from F4 
	  and F49, then sales_amount and num_dbsales should contain the field 5 and field 50. 
	B)If transaction currency is peso, then total_amount and currency_code should be from F4 
	  and F49, then sales_amount and num_dbsales should contain the field 5 and field 50. 
	C)If transaction currency not peso or dollar, then total_amount and currency_code 
	should be from F5 and F50, then sales_amount and num_dbsales should contain the field 4 and field 49. 
	                                                                 |
     | Requirements for Logging to TLF01:                               |
     | ----------------------------------                               |
     | 1.  If Currency Code (cc) is Peso                                |
     |     1.1  DE4  --> total_amount                                   |
     |     1.2  DE5  --> sales_amount  (not used, just logged)          |
     |     1.3  DE49 --> currency_code                                  |
     |     1.4  DE50 --> num_dbsales (not used, just logged)            |
     |                                                                  |
     | 2.  Else if cc is USD                                            |
     |     2.1  DE4  --> total_amount                                   |
     |     2.2  DE5  --> sales_amount               |
     |     2.3  DE49 --> currency_code                                  |
     |     2.4  DE50 --> num_dbsales (not used, just logged)            |
     |                                                                  |
     | 3.  Else (cc is other)                                           |
     |     3.1  DE4  --> sales_amount  (not used, just logged)          |
     |     3.2  DE5  --> total_amount                                   |
     |     3.3  DE49 --> num_dbsales (not used, just logged)            |
     |     3.4  DE50 --> currency_code                                  |
     |                                                                  |
     +------------------------------------------------------------------+*/
 
   if(0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"nposa2",6) ||
	  0 == strncmp(p_auth_tx->TLF01_details.authorizing_host_queue,"natma2",6) 
	  )
   {
		/* A currency code of Pesos */
      if ( 0 != strcmp( PESO, p_auth_tx->reconciliation_cur_code ) )
      {
         /* or dollars is allowed. */
         if ( 0 != strcmp( USD, p_auth_tx->reconciliation_cur_code ) )
         { 
			/* Currency Code is neither Peso nor USD.
                * Store according to comment box above.
                */

			  /* Copy DE4 to sales_amount.  */
               memcpy( p_auth_tx->TLF01_details.sales_amount,
                       p_auth_tx->reconciliation_amount,
                       12 );

               /* Copy DE5 to total_amount. */
               memcpy( p_auth_tx->TLF01_details.total_amount,
                       p_auth_tx->TLF01_details.settlement_total,
                       12 );

               /* Copy DE49 to num_dbsales. */
               memcpy( p_auth_tx->TLF01_details.num_dbsales,
                       p_auth_tx->reconciliation_cur_code,
                       3 );

               /* Copy DE50 to currency_code. */
               memcpy( p_auth_tx->TLF01_details.currency_code,
                       p_auth_tx->settlement_curr_code,
                       3 );

		 
		 }
		 else
		 { /* Currency Code is USD.  Store according to comment box above. */

			  /* Copy DE4 to total_amount. */
            memcpy( p_auth_tx->TLF01_details.total_amount,
                    p_auth_tx->reconciliation_amount,
                    12 );

            /* Copy DE49 to currecny_code. */
            memcpy( p_auth_tx->TLF01_details.currency_code,
                    p_auth_tx->reconciliation_cur_code,
                    3 );
			/* Copy DE5 to total_amount. */
            memcpy( p_auth_tx->TLF01_details.sales_amount,
                       p_auth_tx->TLF01_details.settlement_total,
                       12 );
		   /* Copy DE50 to num_dbsales. */
               memcpy( p_auth_tx->TLF01_details.num_dbsales,
                       p_auth_tx->settlement_curr_code,
                       3 );

		 }

	  }
	  else
	  {
		  /* Currency Code is PESO */
	  /* Copy DE4 to total_amount. */
            memcpy( p_auth_tx->TLF01_details.total_amount,
                    p_auth_tx->reconciliation_amount,
                    12 );

            /* Copy DE49 to currecny_code. */
            memcpy( p_auth_tx->TLF01_details.currency_code,
                    p_auth_tx->reconciliation_cur_code,
                    3 );
			/* Copy DE5 to total_amount. */
            memcpy( p_auth_tx->TLF01_details.sales_amount,
                       p_auth_tx->TLF01_details.settlement_total,
                       12 );
		   /* Copy DE50 to num_dbsales. */
               memcpy( p_auth_tx->TLF01_details.num_dbsales,
                       p_auth_tx->settlement_curr_code,
                       3 );
               if(0 == strncmp(p_auth_tx->country_code,
            		   	   	   p_auth_tx->TLF01_details.nfi_seq_nbr,3))
               {
            	   if(0 == strncmp( p_auth_tx->settlement_curr_code,
            			   	   	    p_auth_tx->country_code,
									3))
            	   {
            		   	  // Do nothing as settlement and original currency are same
            	   }
            	   else
            	   {
            		   memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
            				   p_auth_tx->settlement_curr_code, 3 );
            	   }
               }

	  }
   }
   
	return;
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void ncamex_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  ncamex_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   ncamex_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(ncamex_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  ncamex_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(ncamex_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  ncamex_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(ncamex_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(ncamex_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/
   if((strlen(ncamex_error_warning_file_path) > 0) &&
	  (strlen(ncamex_module_error_warning_file_name)> 0))
   {
	   ncamex_create_Error_Filename();
   }
}

INT ncamex_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int detail)
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
    len = strlen(ncamex_Error_warning_Filename);
    path_len = strlen(ncamex_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&ncamex_Error_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	ncamex_create_Error_Filename();
    }
	if((fp = fopen(ncamex_Error_warning_Filename,"a+b"))==NULL)
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

void ncamex_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( ncamex_Error_warning_Filename, ncamex_error_warning_file_path );
    strcat( ncamex_Error_warning_Filename, ncamex_module_error_warning_file_name );
	strcat( ncamex_Error_warning_Filename, ".EW." );
    strncat(ncamex_Error_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(ncamex_Error_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(ncamex_Error_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         ncamex_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = File/Event log, 3 = Both
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
void ncamex_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num)
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [512] = {0};
   INT  Appname_len = 0;
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;
   CHAR card_num_msg[56] = {0};
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
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   if (card_num[0] == NULL)
   {
	   // No card number, hence masking not required
   }
   else
   {
	   Mask_card_number(card_num);
	   sprintf(card_num_msg, "Card num : %s",masked_card_num);
	   strcat(text_message,card_num_msg);
   }
   /* Call function to post the message. */
   ncamex_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
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

   return;
}

void Mask_card_number(char * card_num)
{
	int cardlen = 0;
	int nIndex 	= 0;
	memset(masked_card_num ,0x00, sizeof(masked_card_num));
	cardlen = strlen( card_num );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, card_num, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}
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
void ncamex_read_TO_RC_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[4]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_TIMEOUT_RC",           /* points to section name         */
									  "SCHEME_TIMEOUT_RC_FOR_AMEX",       	/* points to key name             */
									   "911",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(amex_scheme_timeout_rc,readvalue,3);
	   if(rc == 0)
	   {
		   ncamex_log_message( 2, 1, "SCHEME_TIMEOUT_RC_FOR_AMEX Value not read from tf.ini","ncamex_read_TO_RC_from_tf_ini",0,0);
	   }

}

void ncamex_read_fai_RC_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[4]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",           /* points to section name         */
									  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX",       	/* points to key name             */
									   "100",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(amex_scheme_fai_rc,readvalue,3);
	   if(rc == 0)
	   {
		   ncamex_log_message( 2, 1, "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX Value not read from tf.ini","ncamex_read_fai_RC_from_tf_ini",0,0);
	   }

}
