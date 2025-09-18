/******************************************************************************
 
 	nceqtran.c
 
    Copyright (c) 2005, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Equitable Host Handler Driver
 
 	This module is the host handler for the Equitable host.  It
    will process all messages destined to, or originated from,
    the Equitable host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nceqit_ncmlnk_iso_format\nceqtran.c  $
   
       Rev 1.39   Oct 08 2009 17:05:54   TF Phani
   Modified code to copy response source into Auth_tx.
   Response source contains CVV resutlt code 
   
	 Rev 1.38   Oct 11 2005 16:30:54   dirby
   Modified to allow voids through for credit subtypes of Prepaid.
   SCR 17660
   
      Rev 1.37   Oct 05 2005 13:51:06   dirby
   Modified to only process void for Visa Electron cards.
   SCR 17660
   
      Rev 1.36   Oct 05 2005 12:15:16   dirby
   Changed so that Voids (Sale and Cash) are treated like reversals
   and sent to the host as 0420's.
   SCR 17660
   
      Rev 1.35   Sep 23 2004 16:51:28   dirby
   Modified to copy orig_amount from host response structure into
   auth_tx structure.  This contains balance information for reloads.
   SCR 12503
   
      Rev 1.34   Sep 02 2004 17:10:42   dirby
   Modified to support extremely large amounts.
   SCR 1471
   
      Rev 1.33   Jul 14 2004 14:34:56   dirby
   Added support for Reload transactions.
   SCR 12887
   
      Rev 1.32   Jul 14 2004 10:18:14   dirby
   Modified field 43 to look for a second Bank Code (MID) for field
   43; also added "PCI" in the name/location field.
   SCR 1425
   
      Rev 1.31   Jul 12 2004 13:24:38   dirby
   When getting the results of ATM01 table, changed code to set
   address to "Megalink Bank ATM" when ATM01 record is not found
   unless merchant Id is "13", in which case address stays as
   "Equitable Bank ATM".
   SCR 1425
   
      Rev 1.30   Jun 08 2004 13:41:10   dirby
   Copy text into response_text when host inactive - for Voice GUI.
   Release 4.3.0-1
   
      Rev 1.29   Jan 28 2004 11:54:46   dirby
   Added code to get orig tran date and time on reversals.
   SCR 1327
   
      Rev 1.28   Sep 17 2003 09:48:24   dirby
   Modified response from ATM, when not found, to only use
   Acquirer ID as merchant Id if a merchant Id does not exist.
   SCR 1083
   
      Rev 1.27   Sep 11 2003 16:30:58   dirby
   Added a call to function map_response_code for locally
   acquired transactions.
   SCR 1186
   
      Rev 1.26   Sep 02 2003 16:41:16   svelaga
   AUTH_ADMINISTRATIVE messages are directly sent to ncvsms.
   
      Rev 1.25   May 07 2003 12:46:40   dirby
   Modified to not overwrite total_amount with reconciliation amount
   if reconciliation amount is null.  This was preventing balance
   amount from being sent to device controller.
   SCR 1092
   
      Rev 1.24   Apr 10 2003 13:45:36   dirby
   Modified to not copy the auth_number from the host response
   into auth_tx if the host did not send an auth number.
   SCR 1089
   
      Rev 1.23   Aug 09 2002 16:13:02   dirby
   Modified to not send a response to a reversal immediately.
   Instead, send it after we get the original txn from NCF30.  This
   is so the Merchant ID can be populated from the NCF30 record
   into TLF01.
   SCR 871
   
      Rev 1.22   Aug 02 2002 15:34:58   dirby
   Removed code so time outs of echo requests do not increment
    the timeout counter.  This is so we do not go offline due to too
   many echo request time outs.  Only increment the timeout
   counter for timeouts of financial transactions.
   SCR 594
   
      Rev 1.21   Jul 26 2002 13:26:10   dirby
   Modified to allow 0800 echo requests to go out when host
   state is Offline due to Maximum Time Outs.  Will go back Online
   when a response to the echo request is received.
   SCR 594
   
      Rev 1.20   Jul 05 2002 13:17:22   dirby
   1.  Added code to display a message for reversals.  SCR 621
   2.  Added code to copy balance currency code from host
       message into Auth_Tx.  SCR 823
   
      Rev 1.19   Jun 25 2002 16:47:52   dirby
   Modified to copy balance information into TLF01 fields so Equitable
   can report on them.  Balances copy in as:
     Current Balance    --> TLF01.outstanding_balance
     Available Balance --> TLF01.credit_limit
   SCR 796
   
      Rev 1.18   Jun 18 2002 17:02:28   dirby
   Modified to set global variable TranFlow to SWITCHED_IN for
   responses going to the host.
   SCR 789
   
      Rev 1.17   Jun 14 2002 12:08:24   dirby
   Modified to compare, on reversals, the merchant Id of the 0420
   to that of the original txn.  If different, use the one from the original
   txn.  This is for switched in txns.
   SCR 784
   
      Rev 1.16   Jun 13 2002 14:41:58   dirby
   Modified function 'copy_resp_into_request' so that the balances
   are transferred from the host response into auth_tx regardless
   of message type.  Previously, it was just for balance inquiry, but
   cash transactions can have balance info also.
   SCR 780
   
      Rev 1.15   May 24 2002 16:17:20   dirby
   Modified a check on responses from the host that sees if
   currency conversion has taken place.  It was using the reconciliation
   variables, but these can be populated without conversion taking
   place.  Modified check to use Ncf01.currency_code_cash.  If
   empty, then no conversion took place.
   SCR 745
   
      Rev 1.14   May 15 2002 12:13:26   dirby
   Added mcard_banknet and invoice_number to the list of fields
   that are copied from NCF30 into Auth_Tx for reversals.  These
   2 fields contain Visa CPS information for Visa DE62.
   SCR 725
   
      Rev 1.13   Apr 24 2002 15:28:10   dirby
   Modified to not automatically send reversal to Visa is txn timed out.
   Visa does this on its own.
   SCR 701
   
      Rev 1.12   Apr 15 2002 16:06:22   dirby
   Modified to always copy transmission timestamp from NCF30
   into Auth_Tx, for reversals.  Previously, it would not copy the field
   over if field was already populated.  The reason for this is because
   the value from EB Host is in local time.  Do not want to send
   that to the authorizing host.
   SCR 700
   
      Rev 1.11   Apr 12 2002 16:44:40   dirby
   Modified to get ATM01 record for reversals.
   SCR 699
   
      Rev 1.10   Apr 11 2002 12:29:46   dirby
   Modified so that the default values when a record is not found
   in ATM01 are not all uppercase.
   SCR 681
   
      Rev 1.9   Apr 11 2002 11:47:56   dirby
   Modified the default values used when a record is not found in
   ATM01:
      Merchant ID = Data from DE32
      Merchant Address = EQUITABLE BANK ATM
      Merchant City          = METRO MANILA
      Merchant Intl Code = PH
   SCR 681
   
      Rev 1.8   Apr 04 2002 18:19:12   dirby
   Updated to version 4.0.2.4
   Fixed bug where function blank_string is called.  Need to send
   size of field minus 1, otherwise the trailing null is checked.
   SCR 681
   
      Rev 1.7   Jan 16 2002 12:31:20   dirby
   Added transaction tracing capability.     SCR 546
   
      Rev 1.6   Jan 14 2002 18:14:58   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.5   Dec 14 2001 14:53:14   dirby
   Added some nceqit_log_message calls for some error messages
   that were being formed, but not logged.
   SCR 87
   
      Rev 1.4   Dec 10 2001 14:07:56   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.3   Sep 27 2001 09:48:02   dirby
   Modified to not overwrite any values in the reconciliation variables
   when doing currency conversion.  SCR 447
   
      Rev 1.2   Aug 17 2001 13:03:24   dirby
   Eliminated a comment within a comment (to get a clean compile).
   
      Rev 1.1   Aug 13 2001 17:30:08   SYARLAGA
   Modified process_request_for_host_after_security() and process_request_for_host functions.
   
      Rev 1.1   Aug 13 2001 17:25:32   SYARLAGA
   Modified process_request_for_host  function.
   
      Rev 1.0   Aug 06 2001 09:48:58   dirby
   Initial revision.
   
      Rev 1.47   Dec 01 2000 17:23:34   dirby
   Modified so that if a switched in txn times out going to a different host,
   set rc=19 (9019) and respond to the host.  Was not doing this previously.
   
      Rev 1.46   Oct 28 2000 15:44:38   dirby
   Made a change to create a date only if it does not exist already.
   
      Rev 1.45   Oct 18 2000 15:04:22   dirby
   Made the tran and host start and end times more efficient.
   
      Rev 1.44   Aug 28 2000 10:17:06   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.43   Aug 28 2000 09:27:52   dirby
   Changed the MONITOR_STATS structure from 'long's to 'char's.
   Unix seems to translate longs differently than NT does.
   
   
      Rev 1.42   Aug 23 2000 14:10:54   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.41   Jul 21 2000 09:03:18   dirby
   Made the following change per EB's instructions for ITEM #23:
   1.  Use Acquirer CB Code in Host2 message for Merchant ID
   2.  Use ATM01.brchname for Merchant Address01
   3.  Use ATM01.termcimu for Merchant City
   If no ATM record is found, hardcode:
   1.  Merchant Address01 to be "HEAD OFFICE"
   2.  Merchant City to be "METRO MANILA"
   
      Rev 1.40   Jul 19 2000 10:44:06   dirby
   Added transaction type AUTHORIZATION.  This is for Visa Electron transactions.
   
      Rev 1.39   Jun 21 2000 22:35:08   dirby
   1.  Removed some code that cleared the timer after a time out.
   This is no longer necessary because the repeat value was set
   to zero, so the time out message will not repeat.  It is cleared
   when it times out.
   2.  Modified code to only update Monitor's network status if the
   status has changed.  Before, every time out would update Monitor's
   network status.
   
      Rev 1.38   Jun 21 2000 22:20:06   dirby
   Changed parameter to repeat a time out msg from 10 seconds to none.
   This is so it will not be repeated.
   
      Rev 1.37   Jun 21 2000 21:27:38   dirby
   1.  Added tx_key AUTH_SALE, per equitable issue 4, to support additional processing codes.
   2.  Removed a line of code that was actually clearing the ncf01 structure.
   
      Rev 1.36   Jun 08 2000 16:45:02   dirby
   Removed an unused local variable.
   Made currency code so we do not hard code to 608
   on requests unless it is not in the request.
   
      Rev 1.35   Mar 21 2000 17:41:00   ddabberu
   update database for txns response code is 00/55
   
      Rev 1.34   Feb 15 2000 16:48:00   svelaga
   Date_yyyymmdd is populated with current date
   in TLF01 before inserting it into TLF01.
   
      Rev 1.33   Feb 13 2000 14:47:24   svelaga
   Incorporated the changes due to new design
   to the SCR 374.
   
   
      Rev 1.32   Feb 09 2000 09:34:54   ddabberu
   recon_update
   process_reversal_response_db_reply
   
   modified
   
      Rev 1.31   Feb 01 2000 17:21:20   ddabberu
   modified process_request_from_host to 
   add leading zeros top_auth_tx->TLF01_details.reversal_amount
   
      Rev 1.30   Jan 27 2000 08:51:24   dirby
   Added a line of code to insert a record into TLF01 when nceqit is not
   able to send a response to the Host2.
   
      Rev 1.29   Jan 24 2000 17:32:38   ddabberu
   restored back process_timeout_msg of ver 1.27
   
      Rev 1.28   Jan 18 2000 11:45:02   ddabberu
      1.  Removed the code to clear a timer when there is no response from the host.
          Did this because the timer gets cleared when it times out.
      2.  Modified the function that converts request tx_keys to responses so that the
          default will add 100 to the request, making it a response.
   
   
      Rev 1.27   Jan 14 2000 15:32:02   dirby
   Set default to original tx_key in the function that converts tx_key
   from a request to a response.
   
      Rev 1.26   Jan 05 2000 10:43:22   dirby
   Added functionality to update transaction control database
   tables after an authorization and reversal.
   
      Rev 1.25   Dec 29 1999 12:37:48   dirby
   1.  Removed some debug messages that were going to Monitor.
   2.  Changed all C++ style comments to C style comments.
   3.  Fixed a BUG - after selecting an NCF30 record, the transaction amount
        was being written into the transaction-fee amount.  Since the fields are
        different sizes, this was overwriting the currency code, not to mention
        it was setting transaction fee amount to the wrong value.
   
      Rev 1.24   Dec 28 1999 12:32:48   dirby
   Moved some code to make sure reversals have a category code.
   
      Rev 1.23   Dec 16 1999 11:47:18   ddabberu
   1.changed process_auth_response_message, to
    accomodate reversal logic.
   2. Changed process_reversal_response_db_reply
    to send a message to insert into tlf01
      
       Rev 1.22   Dec 14 1999 17:06:50   ddabberu
   added transaction_id if not there, before sending 
   a request to insert into tlf01

       Rev 1.21   Dec 11 1999 11:53:58   ddabberu
    

       Rev 1.20   Dec 10 1999 11:26:02   dirby
   Updated function 'recon_update' to populate additional fields
   that were added to NCF30.
   
      Rev 1.19   Nov 18 1999 13:04:52   svelaga
   Changed the code to facilitate the new reversal 
   logic for switched-in transactions.
   
      Rev 1.18   Aug 30 1999 15:52:48   egable
   Implement new reversal design.
   
      Rev 1.17   Aug 25 1999 17:31:30   egable
   Use timerds instead of netds for queue for 
   timers.
   
      Rev 1.16   Aug 10 1999 19:53:32   egable
   Make changes to the logic when response 
   code = "TO"
   
      Rev 1.15   Aug 10 1999 13:26:08   svelaga
   Added code for handling incoming message with response code "TO"
   
      Rev 1.14   Aug 05 1999 09:04:10   egable
   Undo the change that swapped the reversal_amout
   and actual_amount.
   
      Rev 1.13   Aug 03 1999 15:25:34   svelaga
   modified the if statement to enable updating
   NCF21/NCF30 for both 0100 and 0200 messages.
   
      Rev 1.12   Aug 03 1999 14:14:58   svelaga
   Reworked function 'process_auth_response_message' to
    update NCF21/NCF30 prior to sending out the response
    to the host.
   
      Rev 1.11   Aug 03 1999 11:42:42   egable
   Swap the actual_amount and reversal_amount
   for reversals.
   
      Rev 1.10   Jul 27 1999 15:44:02   egable
   Create auth_tx.transmission_time.  Used by NCVISA.
   
      Rev 1.9   Jul 21 1999 13:22:46   svelaga
   added some traces for ncequit
   
      Rev 1.8   Jul 14 1999 18:28:34   rmalathk
   several changes per EB's request.
   1. On reversal, respond to EB host immediately
       and ignore response from txcntl.
   2. changed tran_codes for reversals.
   3. changed format of acquirer_cb_code and
      terminal_id when receiving request from Eb
      host.
   
      Rev 1.7   Jun 18 1999 15:17:38   rmalathk
   when the EB host sends us a transaction,
   always read ATM01 record and populate 
   merchant_id field.
   
      Rev 1.6   Jun 17 1999 16:18:40   rmalathk
   1. create and use our own rrn instead of using
       rrn sent by the EB host.
   2. use src_date and src_time to populate TLF01
       instead of local_date and local_time
   3. update NCF30 only for AUTH_CASH_ADVANCE.
   
   
      Rev 1.5   May 12 1999 15:02:32   rmalathk
   corrected que name for Racal application.
   
      Rev 1.4   01 Apr 1999 11:24:46   rmalathk
   1. minor bug fixes and changes to populate
       host_start_time, finish_time, acquirer_id fields
       in TLF01.
   2. minor changes for sending the network
       status to monitor.
   
      Rev 1.3   17 Mar 1999 11:09:18   rmalathk
   Several changes made to message format
   after reviewing traces from Equitable.
   
      Rev 1.2   04 Mar 1999 16:19:56   rmalathk
   Added more error-checking, modified build settings.
   
      Rev 1.1   26 Feb 1999 11:31:00   rmalathk
   minor bug fixes: populate balance fields
   in auth_tx after response received from
   host.
   
      Rev 1.0   24 Feb 1999 16:32:52   rmalathk
   Initial Revision
   
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "nceq_constants.h"
#include "nceq_prototypes.h"
#include "txutils.h"
#include "txtrace.h"

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
extern CHAR    timerds_que_name [];

extern CHAR		  visasms_que_name [];

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern ATM01   Atm01;
extern CRF01   Crf01;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Atm01_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Miscellaneous */
extern INT     TranFlow;
extern CHAR    TimerAppData2[];

extern int IST_0120_FLAG;
CHAR masked_card_num[20] = {0};

/***************************************************************************
 *
 *  NAME:         process_host_msg
 *
 *  DESCRIPTION:  Process a message coming in from the host.  Parse it and
 *                populate a host structure with the information.  Then
 *                determine what type of message it is. Call the appropriate
 *                function to deal with it.
 *
 *  INPUTS:       p_msg_in - This is the incoming message from the host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ***************************************************************************/
void process_host_msg( pPTE_MSG p_msg_in )
{
   CHAR    err_buf[256]="";
   INT     ret_val = true;

   /* Update transaction statistics. */
//praneeth   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   /* Cycle the echo timer because traffic came over the line. */
   if ( false == reset_echo_timer(err_buf) )
      nceqit_log_message(1, 2, err_buf, "process_response_from_host",0 );

   /* Copy host message into a character string. */
   copy_to_req_resp_buffer( p_msg_in );

   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Parse message from host. */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   if ( !parse_host_msg( err_buf ) )
      ret_val = false;

   /* Get message type. */
   else if ( !validate_msgtype( Auth_Tx.TLF01_details.message_type, err_buf ) )
      ret_val = false;

   else
   {
      /* Determine if switched in request,
       * or response to a switched out txn.
       */
      TranFlow = determine_txn_flow( Auth_Tx.TLF01_details.message_type, HOST );

      /* Populate Auth_tx structure with data from host. */
      if ( !populate_auth_tx(err_buf) )
         ret_val = false;

      else
      {
         if ( TranFlow == SWITCHED_IN )
            process_request_from_host();
         else
            process_response_from_host();
      }
   }

   /* Log any errors. */
   if ( ret_val == false )
   {
      nceqit_log_message( 1, 2, err_buf, "process_host_msg",0 );

	   /* In case of REVERSALS, if invalid field is there then respond with "30" to EQIT, EQIT INTERFACES, Dec 22 2008, Girija Y, TF */
	   if(is_reversal())
	   {
			/* Determine Transaction Type */
			ret_val = determine_tx_key();
		//	Auth_Tx.TLF01_details.tx_key = ret_val;
			ret_val = init_Auth_Tx(); // TF PHANI ....Init AUTH_TX for proper logging in to TLF01
			strcpy( Auth_Tx.TLF01_details.response_code, INVALID_FIELD );
			send_response_to_host(SEND_UNCONDITIONALLY);
	   }
   }

   return;
}

/**************************************************************************
 *
 *  NAME:         process_request_from_host
 *
 *  DESCRIPTION:  This function is the driver for requests from the host.
 *                It begins the process of gathering information from the
 *                database and forwarding on the request to the authorizer.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ***************************************************************************/
void process_request_from_host()
{
   INT   ret_val;
   CHAR  Buff[100];
	BYTE  mid[16];

   /* Create things such as transaction id, date and time. */
   ret_val = init_Auth_Tx();

   if ( ret_val == true )
   {
      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

      /* Handle Network Management transactions separately. */
      if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         process_net_management_from_host();
      }
      else
      {
         /* Validate information from host. */
         ret_val = validate_host_msg();
         if ( ret_val == true )
         {
            if ( is_reversal() )
            {
               /* REVERSAL: Go to database to get NCF30 record.
                * Processing will continue with process_db_response_ncf30.
                */
               ret_val = get_db_record( NCF30_DATA,
                                        ST2_DB_SELECT_TRAN_BY_STAN,
                                        "A" );

               /* Display reversal message on Monitor.
                * This message is used to isolate problems to selected bins
                * or to certain merchants or to networks.
                */
               memset( Buff, 0x00, sizeof(Buff) );
               sprintf( Buff,
                       "Reversal from EQIT: %s. Merchant %s",
                        AppName,
                        Auth_Tx.TLF01_details.merchant_id );
               nceqit_log_message( 2, 1, Buff, "process_request_from_host",0 );
            }
            else
            {
               /* Go to database to get ATM01 record.
                * Processing will continue with process_db_response_atm.
                */
              // ret_val = get_db_record( ATM01_DATA, 0, AppName );
			   // TF Phani- IST host is not able to send DE43
               /* Request From BDO -to remove ATM01 checking and adding logic to parse DE43*/ 
		/*		strcpy( Auth_Tx.MCF01_details.address01, Auth_Tx.acceptor_term_name );	
				strcpy( Auth_Tx.MCF01_details.city,Auth_Tx.acceptor_city_name); */
			      if ( blank_string((pCHAR)Auth_Tx.TLF01_details.merchant_id,
                        sizeof(Auth_Tx.TLF01_details.merchant_id)-1) )
					 strcpy( Auth_Tx.TLF01_details.merchant_id, Auth_Tx.acq_inst_id_cd );

				  memset( mid, 0x00, sizeof(mid) );
				  strcpy( mid, Auth_Tx.TLF01_details.merchant_id );
				  trim_trailing_spaces( mid );

				  /* Card Acceptor Name and Location */
					 strcpy( Auth_Tx.MCF01_details.address01, "BDO Bank ATM        " );
					if ( blank_string((pCHAR)Auth_Tx.acceptor_term_name,
                        sizeof(Auth_Tx.acceptor_term_name)-1) )
						strcpy( Auth_Tx.acceptor_term_name, "BDO Bank ATM     " );
					if ( blank_string((pCHAR)Auth_Tx.acceptor_city_name,
                        sizeof(Auth_Tx.acceptor_city_name)-1) )
						strcpy( Auth_Tx.acceptor_city_name, "Manila" );
					if ( blank_string((pCHAR)Auth_Tx.acceptor_country,
                        sizeof(Auth_Tx.acceptor_country)-1) )
						strcpy( Auth_Tx.acceptor_country, "PHL" );
				strcpy( Auth_Tx.MCF01_details.address01, Auth_Tx.acceptor_term_name );	
				strcpy( Auth_Tx.MCF01_details.city,Auth_Tx.acceptor_city_name);

				/* strore in TLF01 for Tranlookup and CR-Tran Falcon
				 * if DE43 is not present in the ATM request */
				strncpy(Auth_Tx.TLF01_details.card_holder_name, Auth_Tx.MCF01_details.address01, 25);
				strncpy(Auth_Tx.TLF01_details.ncf01_retired_cwk, Auth_Tx.MCF01_details.city, 13);
				strncpy(Auth_Tx.TLF01_details.ncf01_retired_cwk + 13, "PHL", 3);

			   /* Send request to authorizer for routing or authorizing. */
			   if ( false == send_msg_to_authorizer( MT_AUTH_REQUEST ) )
			   {
				  strcpy( Auth_Tx.TLF01_details.response_code, SYSTEM_ERROR );
				  strcpy( Auth_Tx.TLF01_details.response_text, "SYSTEM MALFUNCTION" );
				  send_response_to_host( SEND_IF_NOT_REVERSAL );
			   }
			   return; 

            }
         }
      }
   }

   if ( ret_val == false )
   {
      /* Unable to continue processing transaction.
       * Need to send a declined response to the host.
       */
      strcpy( Auth_Tx.TLF01_details.response_code, INVALID_TXN );
      send_response_to_host( SEND_UNCONDITIONALLY );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_DB_RESPONSE_ATM
 *
 *  DESCRIPTION:  This function picks up processing of a request from the
 *                host after the ATM01 record has been found in the database.
 *                More fields are now populated in Auth_Tx.  If the record
 *                not found in the database, these fields are hard coded.
 *                The transaction request from the host is then forwarded
 *                on to the authorizer.
 *
 *  INPUTS:       p_data_stream - Data from database and echo of Auth_Tx
 *                record_found  - Flag that indicates if the record was found
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_db_response_atm( pBYTE p_data_stream, INT record_found )
{
   BYTE  mid[16];

   memset( &Atm01,   0x00, Atm01_Size   );
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   memcpy( &Atm01,   p_data_stream+Auth_Tx_Size, Atm01_Size   );
   memcpy( &Auth_Tx, p_data_stream,              Auth_Tx_Size );

   /* Copy ATM01 information into Auth_Tx. */
   if( record_found )
   {
      /* ------------------- */
      /* ATM01 record found. */
      /* ------------------- */
      strncpy( Auth_Tx.acceptor_term_name, Atm01.termaddr,    25 );
      strncpy( Auth_Tx.acceptor_city_name, Atm01.termaddr+25, 13 );
      strncpy( Auth_Tx.acceptor_country,   Atm01.termaddr+38,  2 );

      if ( blank_string((pCHAR)Auth_Tx.TLF01_details.merchant_id,
                        sizeof(Auth_Tx.TLF01_details.merchant_id)-1) )
         strcpy( Auth_Tx.TLF01_details.merchant_id, Atm01.mnemonic );

      strncpy( Auth_Tx.MCF01_details.address01, Atm01.brchname, 15 );
      strncpy( Auth_Tx.MCF01_details.city,      Atm01.termcimu, 20 );
   }
   else
   {
      /* ------------------------------------------------- */
      /* ATM01 record not found for this ATM. Use defaults */
      /* ------------------------------------------------- */

      /* Merchant ID is from DE32 if one does not exist. */
      if ( blank_string((pCHAR)Auth_Tx.TLF01_details.merchant_id,
                        sizeof(Auth_Tx.TLF01_details.merchant_id)-1) )
         strcpy( Auth_Tx.TLF01_details.merchant_id, Auth_Tx.acq_inst_id_cd );

      memset( mid, 0x00, sizeof(mid) );
      strcpy( mid, Auth_Tx.TLF01_details.merchant_id );
      trim_trailing_spaces( mid );

	  // Same 
	if ( blank_string((pCHAR)Auth_Tx.acceptor_term_name,
                        sizeof(Auth_Tx.acceptor_term_name)-1) )
						strcpy( Auth_Tx.acceptor_term_name, "BDO Bank ATM     " );

	 strcpy( Auth_Tx.MCF01_details.address01, Auth_Tx.acceptor_term_name );
		if ( blank_string((pCHAR)Auth_Tx.acceptor_city_name,
                        sizeof(Auth_Tx.acceptor_city_name)-1) )
						strcpy( Auth_Tx.acceptor_city_name, "Manila" );
						
		strcpy( Auth_Tx.MCF01_details.city,Auth_Tx.acceptor_city_name);

    
   }

	

   /* Send request to authorizer for routing or authorizing. */
   if ( false == send_msg_to_authorizer( MT_AUTH_REQUEST ) )
   {
      strcpy( Auth_Tx.TLF01_details.response_code, SYSTEM_ERROR );
      strcpy( Auth_Tx.TLF01_details.response_text, "SYSTEM MALFUNCTION" );
      send_response_to_host( SEND_IF_NOT_REVERSAL );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_DB_RESPONSE_NCF30
 *
 *  DESCRIPTION:  This function picks up processing of a reversal request
 *                from the host after the NCF30 record has been found in
 *                the database.  It will merge information from the original
 *                transaction into Auth_Tx, then call the function to
 *                continue processing a request from the host.
 *
 *  INPUTS:       p_data_stream - Data from database and echo of Auth_Tx
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_db_response_ncf30( pBYTE p_data_stream )
{
   memset( &Ncf30,   0x00, Ncf30_Size   );
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   memcpy( &Ncf30,   p_data_stream+Auth_Tx_Size, Ncf30_Size   );
   memcpy( &Auth_Tx, p_data_stream,              Auth_Tx_Size );

   /* Merge information from original txn into Auth_Tx. */
   update_auth_tx_with_ncf30_data();

   /* Send approved response to host. */
   strcpy( Auth_Tx.TLF01_details.response_code, APPROVED );
   send_response_to_host( SEND_UNCONDITIONALLY );

   /* Go to database to get ATM01 record.
    * Processing will continue with process_db_response_atm.
    */
   (void)get_db_record( ATM01_DATA, 0, AppName );

   return;   
}

/******************************************************************************
 *
 *  NAME:         UPDATE_AUTH_TX_WITH_NCF30_DATA
 *
 *  DESCRIPTION:  This function takes data from ncf30 and puts it into auth_tx
 *
 *  INPUTS:       NCF30   - (GLOBAL) Original txn information
 *
 *  OUTPUTS:      Auth_Tx - (Global) Transaction info updated with orig txn
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Beth Gable
 *
 ******************************************************************************/
void update_auth_tx_with_ncf30_data()
{
   CHAR  unique_tran_str_id[20] = "";

   if ( Auth_Tx.TLF01_details.card_num[0] == 0x00 )
      strcpy( Auth_Tx.TLF01_details.card_num, Ncf30.card_nbr ); 

   if ( Auth_Tx.TLF01_details.processing_code[0] == 0x00 )
      strcpy( Auth_Tx.TLF01_details.processing_code, Ncf30.processing_code );

   if ( Auth_Tx.TLF01_details.orig_retrieval_ref_num[0] == 0x00 )
      strcpy( Auth_Tx.TLF01_details.orig_retrieval_ref_num,
              Ncf30.primary_key.retrieval_ref_num ); 

   strcpy( Auth_Tx.TLF01_details.orig_amount,   Ncf30.tran_amount );
   strcpy( Auth_Tx.TLF01_details.auth_number,   Ncf30.auth_number   );
   strcpy( Auth_Tx.TLF01_details.response_code, Ncf30.response_code );
   strcpy( Auth_Tx.TLF01_details.currency_code, Ncf30.currency_code );
   strcpy( Auth_Tx.TLF01_details.orig_message,  Ncf30.message_type  );
   strcpy( Auth_Tx.TLF01_details.mcard_banknet, Ncf30.network_data  );
   strcpy( Auth_Tx.TLF01_details.invoice_number,Ncf30.invoice_nbr   );

   if ( Auth_Tx.TLF01_details.orig_sys_trace_num[0] == 0x00 )
      strcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
              Ncf30.sys_trace_audit_nbr );

   strcpy( Auth_Tx.orig_local_time_date, Ncf30.transmission_timestamp ); 
   strcpy( Auth_Tx.local_date_time,      Ncf30.tran_date+2 ); 
   strcat( Auth_Tx.local_date_time,      Ncf30.tran_time   ); 

   strcpy( Auth_Tx.TLF01_details.settlement_date, Ncf30.settlement_date );
   strcpy( Auth_Tx.TLF01_details.tran_fee_amount,
           Ncf30.tran_fee_amount );

   strcpy(Auth_Tx.TLF01_details.product_codes[12].amount, Ncf30.transmission_timestamp);// For DCI reversal we are storing original transaction sent timestamp(DE7)

   /* Need to compare Merchant IDs.  Since Eqit may or may not send a
    * Merchant ID, we need to compare.  If they are different, use the
    * one from NCF30, as that is the one that was used with the authorization
    * transaction.  Different Merchant IDs means either the 0200 or the 0420
    * transaction did not have a Merchant ID, so values from database table
    * ATM01 were used.  The point here is to make sure the same Merchant ID
    * is used during the authorization and the reversal; because Eqit host
    * might send different ones.  I do not know why.  This was discovered
    * during EB's testing - it is probably just a test scenario.  Later note:
    * this is occurring in Production also!!
    */
   trim_trailing_spaces( Auth_Tx.TLF01_details.merchant_id );
   trim_trailing_spaces( Ncf30.merchant_id );
   if ( 0 != strcmp(Auth_Tx.TLF01_details.merchant_id,Ncf30.merchant_id) )
   {
      /* Used to log a warning here, but it happens
       * so often, I removed the log entry.
       */

      /* Use the Merchant ID from NCF30. */
      memcpy( Auth_Tx.TLF01_details.merchant_id, Ncf30.merchant_id, DE42_SIZE );
   }

   /* Set tx_key so authorizing network controllers (ncvisa, ncjcb, nccirs)
    * can all handle the reversal.
    */
   Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL;

   return;
}

/****************************************************************************
 *
 *  NAME:         process_response_for_host
 *
 *  DESCRIPTION:  This function handles responses from the authorizer; it
 *                begins the process of sending a response to the host.
 *
 *  INPUTS:       p_msg_in - PTE Message from authorizer containing txn info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ****************************************************************************/
void process_response_for_host( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data;
   LONG            data_len, auth_len;

   /* Copy transaction record into auth_tx structure. */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   auth_len   = data_len;
   if ( Auth_Tx_Size < data_len )
      auth_len = Auth_Tx_Size;
   p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &Auth_Tx, p_data, auth_len );

   /* If txn is approved, we must update the database. */
   TranFlow = SWITCHED_IN;
   if( (0 == strcmp(Auth_Tx.TLF01_details.response_code, APPROVED)) ||
       (0 == strcmp(Auth_Tx.TLF01_details.response_code, "55"    ))  )
   {
      if ( Auth_Tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY_RESPONSE )
      {
         if ( false == update_database() )
         {
            /* Unable to complete a transaction.  Inform host; log to TLF01. */
            nceqit_log_message( 1, 3, "Unable to update database tables",
                        "process_response_for_host",0 );
            strcpy( Auth_Tx.TLF01_details.response_code, SYSTEM_ERROR );
            strcpy( Auth_Tx.TLF01_details.response_text, "SYSTEM MALFUNCTION" );
            send_response_to_host( SEND_IF_NOT_REVERSAL );
         }
      }
      else
      {
         /* No need to update database for Balance Inquiries;
          * just send back response to host and insert into TLF01.
          */
        send_response_to_host( SEND_UNCONDITIONALLY );
      }
   }
   else
   {
      /* Txcntl did not approved the transaction.
       * Need to send the response back to the host.
       */
      send_response_to_host( SEND_IF_NOT_REVERSAL );

      /* Requirement of Equitable Bank: if 0200 txn from EB to other host
       * times out, then issue a reversal to the other host.  These hosts
       * can be Visa, JCB, or Cirrus.  The network controllers for JCB &
       * Visa do this automatically.  But for Cirrus, need to form the
       * reversal here and send it out to txcntl.
       */
      if ( false == is_reversal() )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.response_code,TO) )
         {
            /* Txn timed out. */
            if ( NULL_PTR != strstr(Auth_Tx.TLF01_details.authorizing_host_queue,"cirs")  )
            {
               /* Txn went to cirrus. */
               (void)transform_0200_into_0420();
               Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL;
               strcpy( Auth_Tx.TLF01_details.message_type, "0400" );
               (void)send_msg_to_authorizer( MT_AUTH_REQUEST );
            }
         }
      }
   }
   return;
}

/****************************************************************************
 *
 *  NAME:         process_request_for_host
 *
 *  DESCRIPTION:  This function handles requests from the authorizer; it
 *                begins the process of sending a request to the host.
 *
 *  INPUTS:       p_msg_in - PTE Message from authorizer containing txn info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ****************************************************************************/
void process_request_for_host( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data;
   LONG            data_len, auth_len;

   /* Copy transaction record into auth_tx structure. */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   auth_len   = data_len;
   if ( Auth_Tx_Size < data_len )
      auth_len = Auth_Tx_Size;
   p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &Auth_Tx, p_data, auth_len );

   /* Set Auth_Tx values. */
   strcpy(Auth_Tx.TLF01_details.tpdu_id,      Ncf01_I.tpdu_id );
   strcpy( Auth_Tx.TLF01_details.acquirer_id, AppName         );
   Auth_Tx.TLF01_details.general_status = GS_TXN_NEW;
   TranFlow = SWITCHED_OUT;


   /* Translate the pin into host format. */
   if ( ( !is_reversal() ) && pin_block_exists() )
   {
      /* Only translate for non-reversals.
       * Pin exists, so go ahead and have security translate it.
       */
      strcpy( Auth_Tx.TLF01_details.dest_key,        Ncf01_I.cwk      );
      strcpy( Auth_Tx.TLF01_details.dest_pin_format, Ncf01_I.pin_type );
      (void)translate_pin();
   }
   else
   {
      (void)process_request_for_host_after_security();
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_REQUEST_FOR_HOST_AFTER_SECURITY
 *
 *  DESCRIPTION:  Continue processing a request for the host. Check to see
 *                if we need to perform currency conversion.
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
void process_request_for_host_after_security()
{
   BYTE   tx_key;
   INT    ret_val = true;
   CHAR   err_buf[100]="";
   CHAR   Buff[100];
   double txn_amt;
   double converted_amt;

   /* Process based on transaction type. */
   tx_key = Auth_Tx.TLF01_details.tx_key;

   /* Set TX_KEY to reversal for VE Voids (for non-prepaid). */
   if ( (tx_key == AUTH_VOID_SALE) || (tx_key == AUTH_VOID_CASH_ADVANCE) )
   {
	   /* Send out all void transactions as reversal to EQIT host.EQIT changes,Girija Y, TF,Dec 18 2008 */
 /*     if ( (Auth_Tx.BIN01_details.credit_subtype[0] == 'E') || 
           (Auth_Tx.BIN01_details.credit_subtype[0] == 'P')  ) */
         tx_key = AUTH_REVERSAL;
   }

   switch ( tx_key )
   {
      case AUTH_REVERSAL:
      case AUTH_REVERSAL_ADVICE:

         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( Buff, 0x00, sizeof(Buff) );
         sprintf( Buff,
                 "Reversal going to EQIT: %s. Merchant %s",
                  AppName,
                  Auth_Tx.TLF01_details.merchant_id );
         nceqit_log_message( 3, 2, Buff, "process_request_for_host_after_security",1 );

   	  case AUTH_REFUND:
   	  case AUTH_PRE_AUTHORIZATION_CANCLE:
          memset( Buff, 0x00, sizeof(Buff) );
          sprintf( Buff,
                  "Refund going to EQIT: %s.Merchant %s",
                   AppName,
                   Auth_Tx.TLF01_details.merchant_id );
          nceqit_log_message( 3, 2, Buff, "process_request_for_host_after_security",1 );

   	  case AUTH_ADVICE:
   		  memset( Buff, 0x00, sizeof(Buff) );
   		  sprintf( Buff,
   		                  "Advice going to EQIT: %s. Merchant %s",
   		                   AppName,
   		                   Auth_Tx.TLF01_details.merchant_id );
   		  nceqit_log_message( 3, 2, Buff, "process_request_for_host_after_security",1 );

      case AUTH_SALE: 
      case AUTH_AUTHORIZATION: 
      case AUTH_CASH_ADVANCE: 
      case AUTH_ATM_CONFIRM:
      case AUTH_QUASI_CASH:
      case AUTH_CASH_ADVANCE_AUTHORIZATION:
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
      case AUTH_AUTHORIZATION_OVERRIDE:
	  /* Define these cases for forwarding charge back request to EB host from ncvsms, Girija Y - TF */
	  case AUTH_CHARGEBACK:
	  case AUTH_CHARGEBACK_REVERSAL:
		   case AUTH_OCT_TX:
	  case AUTH_FAST_FUND:
	  case AUTH_MONEY_SEND:
         /* Before sending to the host, verify that the amounts are in 
          * the correct currency code. If not, we need to convert them.
          */
         if ( 0 != strcmp(Auth_Tx.TLF01_details.currency_code,
                          Ncf01_I.currency_code_cash) )
         {
            /* Currency codes are different. Need to convert.
             * First, save the amount and currency code.
             */
            if ( Auth_Tx.reconciliation_cur_code[0] == 0x00 )
            {
               memcpy( Auth_Tx.reconciliation_cur_code,
                       Auth_Tx.TLF01_details.currency_code,
                       sizeof(Auth_Tx.TLF01_details.currency_code));
            }

            if ( Auth_Tx.reconciliation_amount[0] == 0x00 )
            {
               memcpy( Auth_Tx.reconciliation_amount,
                       Auth_Tx.TLF01_details.total_amount,
                       sizeof(Auth_Tx.TLF01_details.total_amount));
            }

            /* Now convert it. */
            txn_amt = atof(Auth_Tx.TLF01_details.total_amount);
            ret_val = perform_currency_conversion(
                               txn_amt,
                               Auth_Tx.TLF01_details.currency_code,
                               Ncf01_I.currency_code_cash,
                               &converted_amt, err_buf );

            if ( true == ret_val )
            {
               /* Conversion was successful.
                * Store the converted amount in Auth_Tx.
                * Need to do it again for reversals for actual amount field.
                */
               sprintf( Auth_Tx.TLF01_details.total_amount, "%012.0f",
                        converted_amt );

               strcpy( Auth_Tx.TLF01_details.conversion_rate,
                       Crf01.conversion_rate );

               /* If reversal, also convert the actual amount field */
               if ( is_reversal() )
               {
                  txn_amt = atol(Auth_Tx.TLF01_details.actual_amount);
                  ret_val = perform_currency_conversion(
                                  txn_amt,
                                  Auth_Tx.TLF01_details.currency_code,
                                  Ncf01_I.currency_code_cash,
                                  &converted_amt, err_buf );

                  if ( false == ret_val )
                  {
                     nceqit_log_message( 1, 2, err_buf,
                                 "process_request_for_host_after_security" ,1);
                  }
               }
            }
            else
            {
               nceqit_log_message( 1, 2, err_buf,
                           "process_request_for_host_after_security" ,1);
            }
         }
         break;

      case AUTH_BALANCE_INQUIRY:
      case AUTH_RELOAD:
      case AUTH_RELOAD_REPEAT:
      case AUTH_RELOAD_CONFIRM:
      case AUTH_VOID_RELOAD:

         /* No currency conversion required for these. */
         break;

      default:

         /* Unknown transaction, reject it. Send response to authorizer. */
         memset( err_buf, 0x00, sizeof(err_buf) );
         sprintf( err_buf,
                 "Invalid txn. Cannot process. TX_KEY = %d",
                  tx_key );
         nceqit_log_message( 1, 2, err_buf,
                     "process_request_for_host_after_security",1 );

         ret_val = perform_standin_processing( INVALID_TXN,
                                               GS_TXN_TRANSACTION_NOT_ALLOWED,
                                               err_buf );
         if ( false == ret_val )
            nceqit_log_message(1,2,err_buf,"process_request_for_host_after_security",1);

         /* Make sure transaction processing does not continue. */
         ret_val = false;
         break;
   }

   /* Continue processing the request for the host. */
   if ( false != ret_val )
      (void)process_request_for_host_after_currency();

   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_REQUEST_FOR_HOST_AFTER_CURRENCY
 *
 *  DESCRIPTION:  Continue processing a request for the host.  At this point,
 *                we have all the data we need from the transaction and from
 *                the data server.  So all that is left to do is to verify the
 *                host is up, then put the message together and send it to the
 *                host.
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
void process_request_for_host_after_currency()
{
   INT  ret_val = true;
   CHAR err_buf[1025]="";


   /* Check the status of the host.  Are we online? */
   ret_val = verify_host_status( err_buf );
   if ( false == ret_val )
   {
      strncpy( Auth_Tx.TLF01_details.response_text, err_buf, 39 );
      nceqit_log_message( 1, 3, err_buf, "process_request_for_host_after_currency" ,1);

      ret_val = perform_standin_processing( HOST_UNAVAILABLE,
                                            GS_TXN_DROPPED,
                                            err_buf );
      if ( false == ret_val )
         nceqit_log_message(1,2,err_buf,"process_request_for_host_after_currency",1);
   }
   else
   {
	   if(Auth_Tx.TLF01_details.tx_key==AUTH_ADVICE && 0==strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6))
	   {
		   if(IST_0120_FLAG==1)
		   {
			   (void)send_request_to_host( TIMER_FINANCIAL );
		   }
		   else
		   {
			   Masked_card_number(Auth_Tx.TLF01_details.card_num);
	            sprintf( err_buf,
	                    "The IST 0120 Flag is Disabled in tf.ini.The advice transaction is dropped and will not be posted to IST. "
	                    "Card_num: %s, Message_type=%s,STAN: %s,IST_0120_FLAG=%d",
						masked_card_num,
						 Auth_Tx.TLF01_details.message_type,
	                     Auth_Tx.TLF01_details.sys_trace_audit_num,
						 IST_0120_FLAG);
	            nceqit_log_message(2, 1, err_buf, "process_response_from_host",1 );
		   }
	   }
	   else
	   {
		   (void)send_request_to_host( TIMER_FINANCIAL );
	   }
   }

   return;
}

/**************************************************************************
 *
 *  NAME:         process_response_from_host
 *
 *  DESCRIPTION:  This function is the driver for responses from the host.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ***************************************************************************/
void process_response_from_host()
{
   CHAR  err_buf[100]="";
   CHAR  lookup_key[LOOKUP_KEY_SIZE]="";

   /* Get host finish time, for statistic purposes. */
   ptetime_get_strtime( Auth_Tx.TLF01_details.host_finish_time );

   /* Decrement number of txns that are outstanding with the host. */
   if ( false == decrement_active_count( err_buf ) )
      nceqit_log_message(1, 3, err_buf, "process_response_from_host" ,1);

   /* Reset number of consecutive timeouts to zero. */
   if ( false == reset_timeout_count( err_buf ) )
      nceqit_log_message(1, 3, err_buf, "process_response_from_host",1 );

   /* Get tx_key, making sure it is valid. */
   if ( true == determine_tx_key() )
   {
      /* Map the host's response code into an ATP Hypercom ISO code. */
      if (0 == strcmp(Auth_Tx.TLF01_details.dispensation.origin, LOCAL_ORIGIN) )
      {
         /* Only do this for locally acquired transactions. */
         map_response_code( Auth_Tx.TLF01_details.response_code,
                            Auth_Tx.TLF01_details.response_code );
      }

      /* Handle Network Management transactions separately. */
      if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
		 update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, MGT );//added line by praneeth with MGT
         process_net_management_from_host();
      }
      else
      {
         /* Update transaction statistics. */
         update_txn_stats( &Txn_Counts, &Auth_Tx, RECEIVED );//praneeth modified NA as RECEIVED

         if (true == nceqit_check_if_Visa_transaction())
         {
        	 create_request_timer_key_for_Visa(lookup_key);
         }
         else
         {
        	 /* Need to clear the timer; so create the lookup key. */
        	 create_request_timer_key( lookup_key );
         }

         /* Now clear the request timer;
          * exit code while waiting for timer's response.
          * Timer's response will contain original auth_tx.
          */
         if ( false == clear_request_timer(lookup_key) )
         {
            sprintf( err_buf,
                    "Unable to clear request on response from host. Msg type: %s, DE7: %s",
                     Auth_Tx.TLF01_details.message_type,
                     Auth_Tx.TLF01_details.transmission_timestamp );
            nceqit_log_message(1, 2, err_buf, "process_response_from_host",1 );
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         process_clear_timer_reply
 *
 *  DESCRIPTION:  Process a reply from the timer.  This function continues
 *                the processing started by 'process_response_from_host'.
 *                It will complete the processing of a response that was
 *                received from a host.
 *
 *  INPUTS:       p_msg_in - This is the message from the timer.  It contains
 *                           two auth_tx buffers: the original auth_tx from
 *                           the acquiring application (dcpiso, ...) and an
 *                           auth_tx containing the host response information.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_clear_timer_reply( pPTE_MSG p_msg_in )
{
   pBYTE          p_data            = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
   pPTE_MSG       p_msg_out         = NULL_PTR;
   pPTE_MSG       p_echo_msg        = NULL_PTR;
   LONG           timer_reply_size  = 0;
   LONG           data_len;
   CHAR           app_data1[32];
   CHAR           app_data2[32];
   CHAR           lookup_key[LOOKUP_KEY_SIZE]="";
   CHAR           echo_key[LOOKUP_KEY_SIZE]  ="";
   CHAR           echo_req_key[LOOKUP_KEY_SIZE]  ="";
   CHAR           err_buf[100];
   AUTH_TX        host_auth_tx;
   
   /* First, extract timer reply from incoming message.
    * This also gets the original Auth_Tx containing all
    * transaction request information.
    */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
                                                app_data2, (pBYTE)&Auth_Tx,
                                                Auth_Tx_Size );

   /* Do not do anything if this is a response to a clear command
    * for the echo timer or for an 0800 echo request.
    */
   create_echo_timer_key( echo_key );
   create_echo_req_timer_key( echo_req_key );
   if ( (0 != strcmp(echo_key,     lookup_key)) &&
        (0 != strcmp(echo_req_key, lookup_key))  )
   {
      /* Verify that timerds was able to successfully
       * retrieve timer and our original auth_tx data.
       * This is the auth_tx we sent along with the
       * 'start timer' request prior to going to the host.
       */
      if( ptemsg_get_result_code( p_msg_in ) == PTEMSG_NOT_FOUND )
      {
         /* Timerds was unable to retrieve our
          * data for the lookup key we provided.
          */
         sprintf( err_buf,
                 "Unable to retrieve timer data for lookup key %s",
                  lookup_key );
         nceqit_log_message( 1, 3, err_buf, "process_clear_timer_reply",1 );
      }

      /* Verify that the data size returned matches auth_tx size */
      else if ( timer_reply_size != Auth_Tx_Size )
      {
         sprintf( err_buf,
                 "Did not receive auth_tx data with timer reply. Key = %s",
                  lookup_key );
         nceqit_log_message( 1, 2, err_buf, "process_clear_timer_reply",1 );
      }
      else
      {
         /* Get auth_tx record that was chained to the 'clear timer' message.
          * This is the auth_tx record built from response data from the
          * host.
          */
         p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
         p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
         p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
         data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

         if ( data_len != Auth_Tx_Size )
         {
            sprintf( err_buf,
                 "Did not receive chained host response data with timer reply. Key = %s",
                  lookup_key );
            nceqit_log_message( 1, 2, err_buf, "process_clear_timer_reply",1 );
         }

         /* No need to forward responses of reversals,
          * if we generated the reversal due to an 0200 timeout.
          */
         else if ( 0 != strcmp(TIMER_REVERSAL, app_data2) )
         {
            /* Get auth_tx record that was built from host response data. */
            memset( &host_auth_tx, 0x00,   Auth_Tx_Size );
            memcpy( &host_auth_tx, p_data, Auth_Tx_Size );

            /* Move response info from host into Auth_Tx. */
            (void)copy_resp_into_request( &host_auth_tx );

            /* Restore original amount, if currency conversion took place. */
            if ( Ncf01_I.currency_code_cash[0] != 0x00 )
            {
               if ( Auth_Tx.reconciliation_cur_code[0] != 0x00 )
                  memcpy( Auth_Tx.TLF01_details.currency_code,
                          Auth_Tx.reconciliation_cur_code,
                          sizeof(Auth_Tx.TLF01_details.currency_code));

               if ( Auth_Tx.reconciliation_amount[0] != 0x00 )
                  memcpy( Auth_Tx.TLF01_details.total_amount,
                          Auth_Tx.reconciliation_amount,
                          sizeof(Auth_Tx.TLF01_details.total_amount));
            }

            /* Indicate a Remote Authorization, not local. */
            strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );

            /* Set tx_key to a 'response' value. */
            (void)set_tx_key_to_response();

            /* Update the general status. */
            Auth_Tx.TLF01_details.general_status = GS_TXN_COMPLETED;

            /* Forward the response to the authorizer. */
            if ( false == send_msg_to_authorizer(MT_AUTH_RESPONSE) )
            {
               sprintf( err_buf,
                    "Unable to complete processing of response msg. MsgType: %s, processing code: %s, stan: %s",
                     Auth_Tx.TLF01_details.message_type,
                     Auth_Tx.TLF01_details.processing_code,
                     Auth_Tx.TLF01_details.sys_trace_audit_num );
               nceqit_log_message( 1, 3, err_buf, "process_clear_timer_reply" ,1);
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         COPY_RESP_INTO_REQUEST
 *
 *  DESCRIPTION:  This procedure copies information that came from the host in
 *                a response into the Auth_Tx buffer.
 *
 *  INPUTS:       host_auth_tx - Transaction response record from host
 *
 *  OUTPUTS:      Auth_Tx - (Global) Txn record is updated with response info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void copy_resp_into_request( pAUTH_TX host_auth_tx )
{
	INT emv_response_len = 0;
	INT actuval_EMV_resp_len =0,temp_int;
	BYTE temp_len[5]={0};
	INT DE104_len=0;
   if ( (AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key) ||
        (AUTH_RELOAD          == Auth_Tx.TLF01_details.tx_key) ||
        (AUTH_RELOAD_REPEAT   == Auth_Tx.TLF01_details.tx_key) ||
        (AUTH_RELOAD_CONFIRM  == Auth_Tx.TLF01_details.tx_key)  )
   {
      strcpy( Auth_Tx.TLF01_details.total_amount,
              host_auth_tx->TLF01_details.total_amount);
      strcpy( Auth_Tx.TLF01_details.orig_amount,
              host_auth_tx->TLF01_details.orig_amount);
   }

   /* Copy balance info for Balance Inquiries and Cash Txns. */
   strcpy( Auth_Tx.current_balance,   host_auth_tx->current_balance   );
   strcpy( Auth_Tx.available_balance, host_auth_tx->available_balance );

   Auth_Tx.current_balance_sign   = host_auth_tx->current_balance_sign;
   Auth_Tx.available_balance_sign = host_auth_tx->available_balance_sign;

   /* Copy balances into these fields; ncvisa looks for them here. */
   strcpy( Auth_Tx.ACF01_details.cash_available, Auth_Tx.current_balance );
   strcpy( Auth_Tx.ACF01_details.cash_limit, Auth_Tx.available_balance );

   /* Copy balances into these fields for Equitable's proprietary reports. */
   strcpy( Auth_Tx.TLF01_details.outstanding_balance,
           host_auth_tx->TLF01_details.outstanding_balance );
   strcpy( Auth_Tx.TLF01_details.credit_limit,
           host_auth_tx->TLF01_details.credit_limit );

   /* Copy balance currency code. */
   strcpy( Auth_Tx.host2_src_fi_nbr, host_auth_tx->host2_src_fi_nbr );

   if ( host_auth_tx->TLF01_details.auth_number[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.auth_number,
              host_auth_tx->TLF01_details.auth_number );
   }

   strcpy( Auth_Tx.TLF01_details.response_code,
           host_auth_tx->TLF01_details.response_code );

   if ( 0 != strcmp(Auth_Tx.TLF01_details.response_code,APPROVED) )
   {
      strcpy( Auth_Tx.TLF01_details.response_text,
              host_auth_tx->TLF01_details.response_text );

      if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y' )
      {
         /* Txn is a Voice Auth. Copy response text to voice text field.*/
         strcpy( Auth_Tx.TLF01_details.voice_auth_text,
                 host_auth_tx->TLF01_details.voice_auth_text );
      }
   }

   strcpy( Auth_Tx.TLF01_details.host_finish_time,
           host_auth_tx->TLF01_details.host_finish_time );

   /* Copy VISA CPS information */
   if ( Auth_Tx.TLF01_details.visa_char_indicator != 0x00 )
   {
      Auth_Tx.TLF01_details.visa_char_indicator =
                                host_auth_tx->TLF01_details.visa_char_indicator;
   }
   if ( Auth_Tx.TLF01_details.visa_tran_id[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.visa_tran_id,
              host_auth_tx->TLF01_details.visa_tran_id );
   }
   if ( Auth_Tx.TLF01_details.visa_validation_code[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.visa_validation_code,
              host_auth_tx->TLF01_details.visa_validation_code );
   }
  /* copy the Addl response data. because ATP needs to send CVV result code into VISA host. 
    If CVV validation done by Eqit host TF Phani 08 Oct,2009 */
	memcpy(Auth_Tx.TLF01_details.resp_source,host_auth_tx->TLF01_details.resp_source,MAX_DE44_RESPONSE_LEN);

   /* Copy Account Identification information. */
   strcpy( Auth_Tx.acct_id_1, host_auth_tx->acct_id_1 );
   strcpy( Auth_Tx.acct_id_2, host_auth_tx->acct_id_2 );

   /* Copy Network ID for sending back to originator for response,EQIT changes, Dec 18 2008, Girija Y, TF */
   if( Auth_Tx.MCF01_details.vip_code[0] == 0x00 )
   {
	   memcpy( Auth_Tx.MCF01_details.vip_code,host_auth_tx->MCF01_details.vip_code,4);
   }
   // Copy EMV response from IST
    if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",3)== 0)
   {
	   emv_response_len = host_auth_tx->EMV_details.emv_block[0];
	   if(emv_response_len > 0 && emv_response_len < 255)
	      {
				actuval_EMV_resp_len= emv_response_len -3;
				if (actuval_EMV_resp_len < 80)
				{
				sprintf(temp_len,"%04d",actuval_EMV_resp_len);
				genutil_asc_to_bin(temp_len,host_auth_tx->EMV_details.future_field1,&temp_int);
				memcpy( Auth_Tx.EMV_details.future_field1+2,
						host_auth_tx->EMV_details.emv_block+4,
				actuval_EMV_resp_len );

				}
	      }
   }
   else if(host_auth_tx->EMV_details.emv_block[0] != 0x00 &&
	  host_auth_tx->EMV_details.emv_block[1] != 0x00 )
   {
	   memcpy( Auth_Tx.EMV_details.emv_block,host_auth_tx->EMV_details.emv_block,
			   sizeof(Auth_Tx.EMV_details.emv_block));
   }

    if(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"10",2))
    {
    	DE104_len=host_auth_tx->future_use_2[0];
    	memcpy(Auth_Tx.future_use_2,host_auth_tx->future_use_2,DE104_len+1);
    }
   return;
}
   

void Masked_card_number(char * card_num)
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
