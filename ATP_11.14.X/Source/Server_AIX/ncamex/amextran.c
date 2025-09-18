/*******************************************************************************
* Copyright (c) 2006, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      amextran.c
*  
* TITLE:       
*  
* DESCRIPTION: This module will parse and manipulate all incoming 
*               messages from the terminal and the host for the 
*               AMEX host.
*
* APPLICATION: 
*
* AUTHOR:      B. Gable
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncamex\amextran.c  $
   
       Rev 1.85   Jul 14 2009 16:23:32   TF-Ajay
   Changed some of the ISO response codes that Amex response
   codes map to.  This is to allow for ATM response codes.
   match_ISO_to_AMEX_response_code()

		Rev 1.84   Jul 13 2009 13:51:30   TF-Ajay
   copy the STIP txn staus for Reversal advice txn
   update_auth_tx_with_ncf30_data()
   

		Rev 1.83   Feb 28 2006 09:51:30   dirby
   Changed Amex response code for 100 to 107 when ISO response
   code is 05.
   SCR 20104
   
      Rev 1.82   Nov 15 2005 16:37:36   dirby
   Added functionality to block ATM transactions (based on MCC) for
   locally acquired transactions (DE49=608 or DE19=608).
   SCR 17015
   
      Rev 1.81   Nov 15 2005 13:25:38   dirby
   Changed Amex response code from 111 to 100.  This is to use
   the generic response code (100) in place of the response code
   for an invalid account - at the request of Amex.
   SCR 12466
   
      Rev 1.80   Nov 09 2005 16:39:34   dirby
   Added a conversion of response code 58 to amex's response
   code 100.  Also made the default amex response code 100, in
   case there is no conversion available.
   SCR 17010
   
      Rev 1.79   Jan 16 2004 13:36:24   dirby
   Modified to use timerds to clear a timer.
   SCR 1200
   
      Rev 1.78   Jan 15 2004 16:40:34   dirby
   Implemented Online Card Updates (1304/1314).
   SCR 1200
   
      Rev 1.77   Jul 03 2002 15:39:08   dirby
   Added code to display time out message on Monitor.
   SCR 621
   
      Rev 1.76.1.1   May 30 2002 20:00:16   dirby
   Added code to retrieve conversion rate from auth_tx where it
   was stored in case txcntl wrote over the rate in TLF01.
   SCR 745
   
      Rev 1.76.1.0   Apr 18 2002 17:40:48   dirby
   Added handling for response code "189".  This response code
   is sent in an 1120 advice where Amex stood in for us.  It is a 
   new response code, meaning Invalid "SE" or Service Establishment.
   It needs to be logged into TLF01 as 03 - Invalid Merchant.
   SCR 708
   
      Rev 1.76   Jan 23 2002 14:39:30   dirby
   Same as previous revision - forgot to do reset_transaction_statistics
   
      Rev 1.75   Jan 23 2002 14:11:22   dirby
   1.  Initialized some string arrays in send_transaction_statistics.
        Not initializing them caused ncamex to not know who to 
       respond to.
   2.  Updated version to 4.0.1.2
   SCR 592
   
      Rev 1.74   Jan 16 2002 15:13:16   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.73   Jan 14 2002 18:08:20   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.72   Mar 02 2001 10:59:10   dirby
   Moved the 'get_originator_info' out of function 'init_txn'.
   Moved it to the start of 'incoming_host_message'.  This
   way it is there for any error conditions as well as successful
   requests.
   
      Rev 1.71   Feb 06 2001 08:58:10   dirby
   Modified destination queue for 'responses' to the host so that it goes
   to the queue of whichever dialog acquired the request.  So instead of
   going to 'dialogA', it might go to 'hstampos'.
   
      Rev 1.70   Jan 26 2001 14:42:16   dirby
   1.  Added function 'process_logon_request'.
   2.  Added functions 'get_originator_info' and 'set_originator_info'.  This was done
   in order to modulize the getting and setting of this information for switched in txns.
   3.  Removed some obsolete comments.
   
      Rev 1.69   Aug 28 2000 10:07:36   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.68   Aug 28 2000 09:06:48   dirby
   Changed the MONITOR_STATS structure from 'long's to 'char's.
   Unix seems to translate longs differently than NT does.
   
   
      Rev 1.67   Aug 23 2000 12:46:34   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.66   Aug 15 2000 15:07:04   dirby
   Modified log on/off code to be more efficient and display
   accurate messages.
   
      Rev 1.65   Aug 15 2000 11:23:10   dirby
   Made some adjustments so message type can be either
   Amex style or ISO8583.
   
      Rev 1.64   Aug 14 2000 18:55:36   dirby
   Reworked some of the Log ON/OFF logic.  Were getting errors
   when an 1804 request came in from the host, because ncamex
   was not identifying the transaction as a log on or log off.
   
      Rev 1.63   Aug 08 2000 13:36:42   dirby
   Changed response code from 91 to 54 for invalid effective date.
   
      Rev 1.62   Aug 01 2000 13:41:30   dirby
   Added function 'validate_host_msg' to weed out garbage
   messages that might find their way through the dialog manager.
   
      Rev 1.61   Jul 10 2000 15:53:18   dirby
   Removed functionality that retrieved NCF01 from the database during
   every online transaction.  Also removed some functions that were completely
   commented out.  They have never been used, and never will.
   
      Rev 1.60   Jul 10 2000 14:22:04   dirby
   1.  Added call to init_txn to initialize txn prior to error checking.
   2.  Moved the line of code that populates tran_finish_time to right before
       txn gets sent to host.
   
      Rev 1.59   Apr 11 2000 11:31:38   ddabberu
    removed the duplication of fld 48 from amextran.c
   
   
      Rev 1.58   Apr 10 2000 14:48:18   ddabberu
   Globals Variables si, tpdu_id defined.
   and checks made based on them.
   
   
      Rev 1.57   Apr 07 2000 18:17:26   ddabberu
    
   
      Rev 1.56   Apr 07 2000 15:25:46   ddabberu
   1. Fld 38 changes for pos response.
   2. For 1120 , we respond back with success.
   3. Code Optimization for match_amex_iso_response()
   
      Rev 1.55   Apr 06 2000 15:24:24   ddabberu
   1.changed the network_type from 'A' to 'I' for logon/off.
   2. Added isalphanumSpace()
   3. Responding to the host, if validation of auth_tx fails.
   
   
      Rev 1.54   Mar 31 2000 14:43:18   ddabberu
   1.Trk1 data getFldSep() changed.
   2.SignOff, fld48 sent.
   3.ATM error code for exp date changed to 100.
   
      Rev 1.53   Mar 21 2000 17:07:28   ddabberu
   update_database if respose code is 00/55
   
      Rev 1.52   Mar 16 2000 21:37:46   ddabberu
   modified to support AMEX 99.2 specs
   
      Rev 1.51   Mar 01 2000 13:09:56   ddabberu
   update_auth_Tx_with_ncf30 modified
   scr #457
   
      Rev 1.50   Feb 24 2000 15:21:48   ddabberu
   1.added 912 to auth_logon_amex[] equivalent iso response code= 91 (new amex spec)
   2. commented the msg to monitor in process_logon_response() function
   
      Rev 1.49   Feb 21 2000 16:27:28   ddabberu
   In incoming_host_message_ncf01_reply()
   the effective date comparision is changed from 9000 to 7000.
   
   #SCR 423
   
      Rev 1.48   Feb 11 2000 17:09:00   svelaga
   1. Incorporated the design changes due to
   SCR #374.
   2. Fix for SCR #391.
   
      Rev 1.47   Feb 07 2000 10:16:04   ddabberu
   1.transmission_timestamp added tp ncf30 structure
   in recon_update,if tlf01 doesnt have one, create a timestamp and insert into ncf30
   
      Rev 1.46   Feb 05 2000 15:29:14   ddabberu
   transmission_timestamp added tp ncf30 structure
   in recon_update
   
      Rev 1.45   Jan 25 2000 10:32:32   dirby
   Added standin processing for some void transactions.
   
      Rev 1.44   Jan 24 2000 17:39:30   ddabberu
   restored back the process_timeout_msg from ver 1.42
   
      Rev 1.43   Jan 18 2000 10:43:36   dirby
   1.  Removed the code to clear a timer when there is no response from the host.
       Did this because the timer gets cleared when it times out.
   2.  Modified the function that converts request tx_keys to responses so that the
       default will add 100 to the request, making it a response.
   
      Rev 1.42   Jan 14 2000 15:21:42   dirby
   Set default to the original tx_key in the function that converts tx_key
   from a request to a response.
   
      Rev 1.41   Jan 07 2000 16:44:48   ddabberu
   1.Changed hhutil_db_ok() to handle auth_advices
   2. Removed unwanted code from recon_update()
   
      Rev 1.40   Jan 03 2000 17:06:08   ddabberu
   modified process_auth_advices to restore the
   txn_key
   
      Rev 1.39   Jan 03 2000 15:27:10   ddabberu
   code modifications related to new Auth_advice process
   and AUTHORIZATION .
   
      Rev 1.38   Jan 03 2000 09:06:12   dirby
   Modified check of effective date to compare using
   4 digit years, instead of 2 digit years.
   
      Rev 1.37   Dec 23 1999 09:20:56   dirby
   Modified function 'update_auth_tx_with_ncf30_data'
   to check for existing values prior to copying into
   auth_tx.
   
      Rev 1.36   Dec 22 1999 17:16:36   dirby
   Fixed various bugs in auth advice processing.
   
      Rev 1.35   Dec 22 1999 10:00:06   dirby
   Modified to separate out 'advice' processing from the 'reversal' processing.
   
      Rev 1.34   Dec 16 1999 11:31:52   dirby
   Modified 'recon_update' to use the original rrn when
   deleting NCF30 records.
   
      Rev 1.33   Dec 03 1999 14:59:00   svelaga
   reshuffled the code to make the execution path
   of 1804 proper.
   
      Rev 1.32   Nov 18 1999 09:30:46   dirby
   Fixed a couple of compile bugs.
   
      Rev 1.31   Nov 17 1999 14:37:00   dirby
   Corrected some compiler errors.
   
      Rev 1.30   Nov 17 1999 14:21:28   dirby
   Modified code for the following reasons:
   1.  Use Update Data Server for updating non-network tables.
   2.  Implement new reversal strategy.
   
      Rev 1.29   Aug 30 1999 15:49:18   egable
   Implement new reversal design.
   
      Rev 1.28   Aug 10 1999 16:08:22   egable
   Change recon_update.
   
      Rev 1.27   Jul 29 1999 21:48:58   egable
   Correct errors found when upgrading to 
   Developer Studio 6.0
   
      Rev 1.26   Jul 16 1999 16:15:14   egable
   Prevent an insert TLF01 error for monitor
   initiated logons by creating the transaction
   id.
   
      Rev 1.25   Jul 09 1999 13:50:26   egable
   Send all messages to event log and don't 
   validate effective date unless it has data.
   
      Rev 1.24   Jul 07 1999 15:54:16   egable
   Stop processing if processing code, amounts,
   or dates aren't numeric, also stop processing if
   tx_key is AUTH_INVALID.
   
      Rev 1.23   Jun 18 1999 10:06:52   egable
   Correct compiler error.
   
      Rev 1.22   Jun 15 1999 16:26:14   svelaga
   bug fix : 08xx messages received from the Amex network are logged into TLF01
   
      Rev 1.21   Apr 21 1999 15:47:30   egable
   Correct response code.
   
      Rev 1.20   Apr 21 1999 01:14:36   egable
   Correct retrieval reference number.
   
      Rev 1.19   Apr 20 1999 13:00:30   egable
   Update status for failed echo test, correct logoff
   message, remove extra error messages.
   
      Rev 1.18   19 Apr 1999 09:53:16   dirby
   1.  Added override transaction processing.
   2.  Added function 'create_rrn'.
   
      Rev 1.17   Apr 09 1999 15:34:20   egable
   Correct RRN...again.
   
      Rev 1.16   Apr 09 1999 15:10:04   egable
   Correct RRN.
   
      Rev 1.15   Apr 09 1999 11:48:36   egable
   Create RRN differently.
   
      Rev 1.14   Mar 31 1999 15:32:42   egable
   Allow logoffs and echo tests.
   
      Rev 1.13   Mar 25 1999 14:06:08   egable
   Update host_start_time, host_finish_time and
   correct reversal logic (re: NCF30).
   
      Rev 1.12   Mar 19 1999 11:24:00   egable
   Output field 12 for an 1804 message.
   
      Rev 1.11   Mar 19 1999 10:47:54   egable
   Remove debugging error messages and add
   user messages.
   
      Rev 1.10   Mar 16 1999 13:06:40   egable
   Changes to debug amex problems
   
      Rev 1.9   Mar 12 1999 08:08:50   egable
   Check for valid currency code and add error
   messages for debugging.
   
      Rev 1.8   Mar 10 1999 08:34:24   egable
   Correct problems with amounts in TLF01
   for reversals.
   
      Rev 1.7   Mar 09 1999 13:05:12   egable
   Correct an error with processing code.
   
      Rev 1.6   Mar 07 1999 10:44:12   egable
   Verify that the effective date is valid.
   
      Rev 1.5   Mar 04 1999 14:39:04   egable
   Correct error processing paths.
   
      Rev 1.4   Mar 02 1999 16:01:36   egable
   Correct a response code and advice messages.
   
      Rev 1.3   Feb 27 1999 16:10:22   egable
   Correct an error in the error processing code.
   
      Rev 1.2   Feb 26 1999 11:29:58   egable
   Correct error in path where monitor sends
   a log in to host message
   
      Rev 1.1   Feb 25 1999 15:27:02   egable
   Process a host logon sent from ATP, with response
   from host.
   
      Rev 1.0   Feb 23 1999 09:17:48   egable
   Initial Release
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
#include "amexmain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "echo_timer.h"


/**********************************
      FUNCTION PROTOTYPES
**********************************/

/* functions local */
INT	 send_request_to_host();

INT    alter_transaction_data(pAUTH_TX, pAUTH_TX);
INT    send_response_to_term_app(pAUTH_TX);
void   assign_next_stan();
INT    get_term_status(BYTE[]);
INT    verify_host_app_status();
INT    perform_host_logon_request(CHAR[],INT,pAUTH_TX);
INT    perform_op_net_mngmnt_request(CHAR[],pAUTH_TX);
INT    perform_op_net_mngmnt_response(CHAR[],pAUTH_TX);
BYTE   match_request_to_response(BYTE);
void   perform_error_processing(pAUTH_TX);
INT    get_amex_data (pAUTH_TX);
void   calc_julian_day(CHAR[],CHAR[]); 
void   perform_standin_processing(CHAR[], BYTE, pAUTH_TX);


/* global variables */
extern NCF01       ncf01;
extern TLF01       tlf01;
extern struct iso_msg_rec HOST_MESSAGE;
extern INT           buflen;                  /* total size of buffer */
extern BYTE ReqRsp_Buffer [COMM_BUF_SZE];
extern CHAR          SHARED_MEMORY_TBL[];
INT  i;
CHAR    tpdu_id[16];
INT     si;
BYTE    orig_host_start_time[17];
CHAR    trands_que_name[]   = "trandsA";
CHAR    netds_que_name[]    = "netdsA";
CHAR    devds_que_name[]    = "devdsA";
CHAR    timerds_que_name[]  = "timerdsA";
CHAR  Amex_Atm_que_name []  = "ncatmaA";

CHAR  ncamex_Error_warning_Filename[256]={0};
CHAR  ncamex_module_error_warning_file_name[256]={0};
CHAR  ncamex_error_warning_file_path[256]={0};
extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

extern CHAR        updateds_que_name[];
extern CHAR        authorizer_que_name[];
extern CHAR        cur_app_name[];
extern CHAR        oprmon_que_name[];
extern CHAR        AppName[];

extern CHAR        Dest_Stat_Id[];
extern CHAR        applnk_que_name[];
extern INT         Field63_present;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Constants & variables used for processing of LOGONs */ 
CHAR vLogon_NMI_Code[4];
CHAR vLogon_Resp_Indicator[6];

extern CHAR   ServiceName[12];

CHAR  LOGON_NMI_START[4] =          "001";
CHAR  LOGON_NMI_ECHO_TEST[5] =      "831";
CHAR  LOGON_NMI_LOGON[5] =          "801";
CHAR  LOGON_NMI_LOGOFF[5] =         "802";

CHAR  LOGON_REQUEST[6] =            "LOGON";
CHAR  LOGON_RESPONSE[16] =          "LOGON RESPONSE";
CHAR  ECHO_REQUEST[11] =              "ECHO";
CHAR  ECHO_RESPONSE[20] =     "LOGON ECHO RESPONSE";
CHAR  ECHO_TIMER_KEY[21] =    "LOGON ECHO TIMER KEY";
CHAR  LOGOFF_REQUEST[7] =           "LOGOFF";
CHAR  LOGOFF_RESPONSE[16] =         "LOGOFF RESPONSE";
CHAR  LOGON_RESP_INDICATOR[6] =     "00002";

#define CWK_KEK       1
#define CWK_MFK       2

CHAR  echo_request_timer_key[15] = "ECHO_REQ_AMEX";
CHAR  echo_lookup_key[15]        = "ECHO_AMEX";
extern WORD  echo_timer_length;

/*****************************************************************************

  Function:    send_it_back_to_terminal

  Description: This function will send the auth_tx buffer back to the authorizer.
  Author: 
      unknown
  Inputs:
      resp_code - The response code to be sent to the terminal
      p_auth_tx - A pointer to the auth_tx buffer to be sent to the authorizer.
  Outputs:
      None
  Return values:
      true  - indicates that the message was sent successfully.
      false - indicates that the message could not be sent successfully.
  Modified by:
      BGable      6/30/98
******************************************************************************/
BOOLEAN send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx )
{
   pPTE_MSG    p_msg_term_handler_out = NULL_PTR;
   LONG        ret_code;
   CHAR	       Buffer[512] = {0};
   CHAR	       ErrorMsg[512] = {0};
   CHAR        sender_que_name[17] = {0};

   strcpy(p_auth_tx->TLF01_details.response_code, resp_code);
   p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);
   strcpy(sender_que_name, authorizer_que_name);
   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              sender_que_name,
								              application_que_name,
                                              (pBYTE)p_auth_tx,
                                              sizeof(AUTH_TX),
                                              0 );
   if ( NULL_PTR == p_msg_term_handler_out )
	{
	   	memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   	sprintf(Log_Bufr," Insufficient Memory to build response to txcntl Term Id:%s ,Merch Id: %s RRN: %s ",
	   			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   	ncamex_log_message( 3, 2, Log_Bufr , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
	   	return(false);
	}
   ret_code = pteipc_send (p_msg_term_handler_out, sender_que_name );
   if ( ret_code < 0 )
   {
 		pteipc_get_errormsg( ret_code, ErrorMsg );
 		sprintf( Buffer, "error on pteipc_send to que %s: %s", sender_que_name, ErrorMsg );
 		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
 		sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
 				p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
 		strcat(Buffer,Log_Bufr);
 		ncamex_log_message( 3, 3, Buffer , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
 		free(p_msg_term_handler_out);
 		return(false);
   }
   else
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Response sent successfully to terminal handler Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 1, 1, Buffer , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   free(p_msg_term_handler_out);
   return(true);
}


/*****************************************************************************

  Function:    incoming_terminal_request

  Description: This function will process an incoming message from the terminal 
               (via the authorizer)
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the incoming auth_tx
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
  Modified by:
      BGable      6/30/98
      DI - 04/14/99 - Added offline cases for override transactions.
******************************************************************************/
INT incoming_terminal_request(pAUTH_TX p_auth_tx)
{
   pPTE_MSG_DATA		p_msg_data = NULL_PTR;
   BYTE					temp_tx_key;
   BYTE              	current_state[2];


   if ((p_auth_tx->TLF01_details.general_status > GS_NSP_KEYCHANGE_FAILED) && ( p_auth_tx->TLF01_details.general_status <= GS_NSP_OUTOFSERVICE))
   {
      if(send_it_back_to_terminal("58", p_auth_tx ))
         return(true);
      return(false);
   }
	
	/*  Visa and Mcard applications will need to read the terminal and Merchant tables
       for any possible additional information as to how this transaction is to be 
	    processed, and for any additional fields that are required in the request to 
	    host. 
   */
	if (false == get_amex_data (p_auth_tx) )
	{
      	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      	  sprintf(Log_Bufr," Error trying to update AMEX information Term Id:%s ,Merch Id: %s RRN: %s ",
      			  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      	  ncamex_log_message( 3, 2, Log_Bufr , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      	  return(false);
	} 

   /* update the general_status for auth_tx */
   strcpy(p_auth_tx->TLF01_details.acquirer_id, AppName);
   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;


   /*****************************************************************************************
   /
	/ Check if we do standin processing or if we need to go online
   / to the host for this transaction 
   /
   /*****************************************************************************************/
   temp_tx_key = p_auth_tx->TLF01_details.tx_key;
	switch ( temp_tx_key)
   {
      case AUTH_OFFLINE_SALE: 
		{	/* create transaction type string and store it in the auth_tx_block */
			p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
		   return(true); 
		}
      case AUTH_REFUND: 
		{	/* create transaction type string and store it in the auth_tx_block */
			if(false==ncpos_check_for_cadencieacq_transaction(p_auth_tx))
			{
			p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
		   return(true);    
		}
			break;
		}
      case AUTH_OFFLINE_REFUND: 
		{	/* create transaction type string and store it in the auth_tx_block */
			p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
		   return(true);    
		}
      case AUTH_VOID_SALE:
      {	/* create transaction type string and store it in the auth_tx_block */
			p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
		   return(true);    
		}
      case AUTH_VOID_REFUND:
      {	/* create transaction type string and store it in the auth_tx_block */
			p_auth_tx->TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
		   return(true);    
		}
		case AUTH_SALE_ADJUSTMENT: 
		{	/* Delete the original transaction from the auth_tx_table if it is there */
         /* create transaction type string and store it in the auth_tx_block      */
		
			p_auth_tx->TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
		   return(true);   
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
         p_auth_tx->TLF01_details.tx_key =
                     AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
      case AUTH_AUTHORIZATION_OVERRIDE: 
      {
         p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_OVERRIDE_RESPONSE;
         perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
         return( true );
      }
	   case AUTH_REVERSAL: 
	   case AUTH_REVERSAL_ADVICE: 
      {	/* Create transaction type string and store it in the auth_tx_block */
    	  if(true==ncpos_check_for_cadencieacq_transaction(p_auth_tx))
    	{

    	  }
    	  else
    	 {
			p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
			perform_standin_processing("00", GS_TXN_PERF_REQ, p_auth_tx);
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
         if (0 != atoi(p_auth_tx->TLF01_details.reversal_amount))
         {
            strcpy(p_auth_tx->TLF01_details.response_code, "96");
         }
    	 }
			break;
		}
		case AUTH_REFUND_ADJUSTMENT: 
		{	/* Delete the original transaction from the auth_tx_table if it is there */
         /* create transaction type string and store it in the auth_tx_block */

		   /* create transaction type string and store it in the auth_tx_block */
			p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_ADJUSTMENT_RESPONSE;
			perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
		   return(true);    
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
   
   /*************************************************************************************/
   
   /* confirm host is available */
   if (false == verify_host_app_status(p_auth_tx))
   {
      /* build response_for_terminal */
      ncamex_log_message( 3, 2, "Host Inactive" , "incoming_terminal_request" , 0 ,"");
      p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);

	   if (0 == strcmp(current_state,"2"))  /* check if we are error state */
      {
         ncamex_log_message( 3, 2, "Host Inactive: current_state!=0" , "incoming_terminal_request" , 0 ,"");

         /* set up a logon request to the host in temp_auth_tx, increment host_app */
         perform_host_logon_request(LOGON_REQUEST, SOURCE_IS_FSVISA, p_auth_tx);
	      /* build PTEMSG's to be sent */
		   if (false == send_request_to_host(p_auth_tx))
			   return(false);
      }

      perform_standin_processing("58", GS_TXN_DROPPED, p_auth_tx);
		return(true);
   }

   memset(vLogon_NMI_Code, 0x00, sizeof(vLogon_NMI_Code));  /* only set for a logon */
   
   /* build PTEMSG's to be sent */
	if (false == send_request_to_host(p_auth_tx) )
			return(false);
   
	return(true);
} /* incoming_terminal_request */

/*****************************************************************************

  Function:    process_logon_response

  Description: This function will zero out all counters used in the host handler
               for determining the host's status.  (These counters are in shared
               memory).
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      true  - always returns true
  Modified by:
      BGable      6/30/98
	  SVelaga	 6/15/99   1. Added an extra parameter for the function.
									2. Incorporated the logic for logging the message into TLF01.
									
******************************************************************************/
BOOLEAN process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx)
{
   pPTE_MSG  p_msg_host_app = NULL_PTR;
   CHAR      ErrorMsg[200] = {0};
   BYTE      net_consec_tmouts_ctr[4] = {0};
   BYTE      active_txns_ctr[4] = {0};
   BYTE      current_state[2]	= {0};
   CHAR      unique_tran_str_id[17] = {0};
   INT       update_host_status = false;

   INT        insert_flag = 0;
   BYTE       buffer1[sizeof(AUTH_TX)+sizeof(TLF01)];
   pPTE_MSG   p_msg_ins = NULL_PTR;
   LONG       ret_code;
   INT        i;

   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );
   strcpy(net_consec_tmouts_ctr, "0");

   if (strcmp(LOGON_NMI_LOGON ,p_auth_tx->function_code) == 0)
   {
      if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
      {
         strcpy(current_state, "0");
         update_host_status = true;
         ncamex_log_message( 1, 1, "LOGON Request successful." , "process_logon_response" , 0 ,"");
      }
      else
      {
         ncamex_log_message( 1, 1, "LOGON Request DENIED." , "process_logon_response" , 0 ,"");
      }
   }
	else if (strcmp(LOGON_NMI_LOGOFF,p_auth_tx->function_code) == 0)
   {
      if ( strcmp( p_auth_tx->TLF01_details.response_code, "00" ) )
      {
         strcpy(current_state, "1");
         update_host_status = true;
         ncamex_log_message( 1, 1, "LOGOFF Request successful." , "process_logon_response" , 0 ,"");
      }
      else
      {
         ncamex_log_message( 1, 1, "LOGOFF Request DENIED." , "process_logon_response" , 0 ,"");
      }
   }

   /* Transaction is completed.  Insert it into the database. */
   for( i=0; i<4; i++ )
         p_auth_tx->TLF01_details.message_type[i] =
                           ascii_ebcdic_conv(ReqRsp_Buffer[i], "E2A");

   ptetime_get_strtime(unique_tran_str_id);
   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id, p_auth_tx->TLF01_details.terminal_id, sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
   strncat(p_auth_tx->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);

   memset(buffer1, 0, sizeof(buffer1) );
   memcpy(buffer1+sizeof(AUTH_TX),&p_auth_tx->TLF01_details,sizeof(TLF01));

   p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
                                ST1_DB_INSERT, 
                                ST2_NONE,
                                updateds_que_name, 
                                application_que_name,
                                (pBYTE)&buffer1,
                                sizeof(buffer1), 
                                TLF01_DATA); 
   if ( NULL_PTR == p_msg_ins )
   {
	   ncamex_log_message( 3, 3, "Insufficient Memory to build insert TLF01 message" , "process_logon_response" , 0 ,"");
       return(false);
   }
   else
   {
      ret_code = pteipc_send( p_msg_ins, updateds_que_name );
      free( p_msg_ins );
   }

   if ( update_host_status == true )
   {
      /* Update host state and counters */
      WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                         active_txns_ctr, current_state );
      (void)send_network_status_to_monitor();
   }
   return(true);
}

/*****************************************************************************

  Function:    process_logon_request

  Description: This function handles network management request messages.
               The host state in shared memory is updated accordingly.  No
               update is made for echoes.

  Author:      D. Irby

  Inputs:      nmi_code  - Code to indicate logon, logoff, or echo.
               p_auth_tx - Transaction & originator information

  Outputs:     p_auth_tx - Updated transaction information (response code)

  Return values: Always returns true
									
******************************************************************************/
BOOLEAN process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx)
{
   CHAR      info_buffer[512] = {0};
   BYTE      net_consec_tmouts_ctr[4] = {0};
   BYTE      active_txns_ctr[4]  = {0};
   BYTE      host_state[2] = {0};
   INT       update_host_status = false;


   /* Check for an echo request. */
   if ( 0 != strcmp(LOGON_NMI_ECHO_TEST, p_auth_tx->function_code) )
   {
      /* It is not an echo.
       *
       * Get host state and counters.
       */
      memset( net_consec_tmouts_ctr, 0x00, sizeof(net_consec_tmouts_ctr) );
      memset( active_txns_ctr,       0x00, sizeof(active_txns_ctr      ) );
      memset( host_state,            0x00, sizeof(host_state           ) );
      memset( info_buffer,           0x00, sizeof(info_buffer          ) );

      ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, host_state );

      /* Set consecutive timeout counter to zero.
       * Set number of active transactions to zero.
       * We are either logging on or logging off.
       */
      strcpy( net_consec_tmouts_ctr, "0" );
      strcpy( active_txns_ctr,       "0" );

      if ( 0 == strcmp(LOGON_NMI_LOGON, p_auth_tx->function_code) )
      {
         /* Request to Log ON. */
         strcpy( info_buffer, "Received Log ON request from host." );

         if ( 0 != strcmp( host_state, ONLINE ) )
         {
            strcat( info_buffer, "  Going ONLINE." );
            strcpy( host_state, ONLINE );
            update_host_status = true;
         }
         else
         {
            strcat( info_buffer, "  Already online." );
         }
      }
      else if ( 0 == strcmp(LOGON_NMI_LOGOFF, p_auth_tx->function_code) )
      {
         /* Request to Log OFF. */
         strcpy( info_buffer, "Received Log OFF request from host." );

         if ( 0 != strcmp( host_state, OFFLINE ) )
         {
            strcat( info_buffer, "  Going OFFLINE." );
            strcpy( host_state, OFFLINE );
            update_host_status = true;
         }
         else
         {
            strcat( info_buffer, "  Already offline." );
         }
      }

      /* Update host state and counters */
      if ( update_host_status == true )
      {
         WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                            active_txns_ctr, host_state );
         (void)send_network_status_to_monitor();
      }
      ncamex_log_message( 1, 1, info_buffer , "process_logon_request" , 0 ,"");
   }

   /* Prepare the response and sent it. */
   strcpy( p_auth_tx->TLF01_details.response_code, "00" );
   p_auth_tx->TLF01_details.tx_key =
                  match_request_to_response( p_auth_tx->TLF01_details.tx_key );
   (void)incoming_other_host_response( p_auth_tx );
   return(true);
}

/*****************************************************************************

  Function:    incoming_host_message

  Description: This function will process a message received from the host.

  Author: 
      unknown
  Inputs:
      p_msg  - incomin host message
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT incoming_host_message(pPTE_MSG p_msg)
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pPTE_MSG          p_msg_chain = NULL_PTR;
   pPTE_MSG          p_msg_timer_out = NULL_PTR;
   LONG              ret_code;
   CHAR              Buffer[512] = {0};
   CHAR              ErrorMsg[512] = {0};
   AUTH_TX           auth_tx;
   BYTE              tran_type;
   CHAR              nmi_code[4];
   BYTE              buffer[sizeof(AUTH_TX)];
   //CHAR				   lookup_key[22];
   CHAR              temp_eff_date[7];
   INT               idate;
   INT               pan_len;
   CHAR              tid[9] = {0};
   CHAR  lookup_key[LOOKUP_KEY_SIZE]={0};

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
    	  ncamex_log_message( 3, 3, "Error setting the echo_timer" , "incoming_host_message" , 0 ,"");
      }
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   memset (&auth_tx, 0, sizeof(AUTH_TX));
   
   /* get the p_msg data from p_msg */
   p_msg_data = ptemsg_get_pte_msg_data(p_msg);  
   
   /* copy data to ReqRsp_Buffer */
   copy_to_req_resp_buffer( ptemsg_get_pte_msg_data_data(p_msg_data),
                            ptemsg_get_pte_msg_data_data_length(p_msg_data));

   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Gather originator information */
   (void)get_originator_info( p_msg, auth_tx.TLF01_details.originator_queue,
                                     auth_tx.TLF01_details.originator_info );

   if ( false == validate_host_msg() )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Received unknown message from host, len = (%d), Msg = (%20s)",
               buflen, ReqRsp_Buffer );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 2, 2, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);

      return( true );
   }
    
   /* Copy ISO bitfield data field to HOST_MESSAGE.msgtype and associated data*/
   if (false == hhutil_parse_host_response_msg(&auth_tx))
   {
      /* Prevents sending response to host (can't send a response to
       * a request we don't understand).
       */
      return(true);
   }
   else
   {
	   /* parsing of message is done successful*/
	   /*Copy the host message in raw formate */
	   /*Copy response msg from cadice to req buffer- TF Abhishek*/
	
		if(buflen < COMM_BUF_SZE && buflen>0)
		{	sprintf(auth_tx.req_data_len,"%04d", buflen);
			memcpy(auth_tx.req_buffer,ReqRsp_Buffer,buflen);
		}
   }

   /* put iso data into auth_tx struct */

   if (false == move_iso_response_to_auth_struct(nmi_code, &auth_tx, Field63_present ))
   {
      /* Prevents sending response to host (can't send a response to
       * a request we don't understand).
       */
      return(true);
   }

   tran_type = get_tran_type_2(auth_tx.TLF01_details.tx_key);
   auth_tx.TLF01_details.tx_key = tran_type;

   if(AUTH_INVALID == auth_tx.TLF01_details.tx_key)
   {
      /*+-------------------------+
        | Send out error message. |
        +-------------------------+*/
      strcpy( Buffer, "Unsupported transaction from AMEX. Message type = ");
      strcat( Buffer,  auth_tx.TLF01_details.message_type );
      strcat( Buffer, ", Processing code = " );
      strcat( Buffer,  auth_tx.TLF01_details.processing_code );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
      return(true);
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, &auth_tx, NA );

   if(0 == strcmp(auth_tx.TLF01_details.message_type, "1814"))
   {  
      process_logon_response(nmi_code,&auth_tx);
      return(true);
   }

   (void)init_txn( &auth_tx );

   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
   strncpy(tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

   if(0 == strcmp(auth_tx.TLF01_details.message_type, "0800"))
   {  
      process_logon_request(nmi_code,&auth_tx);
      return(true);
   }

   if ( auth_tx.TLF01_details.tx_key != AUTH_CARD_UPDATE_RESPONSE )
   {
      if (0 != strncmp(auth_tx.TLF01_details.currency_code, PESO,3))
      {
         if (0 != strncmp(auth_tx.settlement_curr_code, USD, 3))
         {
            perform_error_processing( &auth_tx );
            strcpy(auth_tx.TLF01_details.response_code,"91");
            auth_tx.TLF01_details.tx_key =
                     match_request_to_response(auth_tx.TLF01_details.tx_key);

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Invalid currency code Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
            incoming_other_host_response(&auth_tx);
            return(false);
         }
      }
   }
/* Dont check effective date for message going to Cadencie from Amex Host */
   if(0 != strncmp(auth_tx.TLF01_details.message_type,"0100",4))  
   {
	   if (0 != strlen(auth_tx.effective_date))
	   {
		  /* Create 4 digit year for effective date. */
		  memset( temp_eff_date, 0x00, sizeof(temp_eff_date) );
		  idate = atoi( auth_tx.effective_date );
		  if ( idate > 7000 )
			 strcpy( temp_eff_date, "19" );
		  else
			 strcpy( temp_eff_date, "20" );
		  memcpy( temp_eff_date+2, auth_tx.effective_date, 4 );


		  /* Effective date is the date a card becomes available for use.
		   * It must be less than today's date.
		   */
		  if (0 < strncmp(temp_eff_date, auth_tx.TLF01_details.date_yyyymmdd,6 ))
		  {
			 memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			 sprintf(Log_Bufr," Invalid effective date Term Id:%s ,Merch Id: %s RRN: %s ",
			 		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
			 ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
			 /*perform_error_processing( &auth_tx );
			 strcpy(auth_tx.TLF01_details.response_code,"54");
			 auth_tx.TLF01_details.tx_key =
						 match_request_to_response(auth_tx.TLF01_details.tx_key);
			 incoming_other_host_response(&auth_tx);
			 return(false);*/
		  }
	   }

     
   }
   

   /* Send switched in request to the authorizer. */
   if((auth_tx.TLF01_details.tx_key >= AUTH_SALE) &&
      (auth_tx.TLF01_details.tx_key <= AUTH_INVALID))
   {
      strcpy(auth_tx.TLF01_details.issuer_id, AppName);
      strcpy(auth_tx.TLF01_details.source_key, ncf01.cwk);
      strcpy(auth_tx.TLF01_details.source_pin_format, ncf01.pin_type);

      /* Are we blocking this transaction? */
      if ( true == txn_is_blocked(&auth_tx,Buffer) )
      {
         /* Txn is blocked.  Log message and respond to host. */
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncamex_log_message( 3, 2, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);

         perform_error_processing( &auth_tx );
         strcpy(auth_tx.TLF01_details.response_code,"57");
         auth_tx.TLF01_details.tx_key =
                     match_request_to_response(auth_tx.TLF01_details.tx_key);
         incoming_other_host_response(&auth_tx);
         return(false);
      }
      else if(false == hhutil_process_host_request(&auth_tx))
      {
         perform_error_processing( &auth_tx );
         strcpy(auth_tx.TLF01_details.response_code,"91");
         auth_tx.TLF01_details.tx_key =
                     match_request_to_response(auth_tx.TLF01_details.tx_key);
         incoming_other_host_response(&auth_tx);
         return(false);
      }
      return(true);
   }
  
   /***********************************************************************************
           THIS AMEX NETWORK HANDLER IS ONLY CODED TO PROVIDE ISSUING (INCOMING
           REQUEST FROM HOST), THE CODE BELOW IS THE ACQUIRING PATH AND ISN'T 
           GUARANTEED TO WORK AT ALL!!!!!!!!!!!!!
    ************************************************************************************/
   if(ncpos_check_for_cadencieacq_transaction(&auth_tx))
   {
   // this is an amex acquiring refund transaction response.

     /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, &auth_tx, RECEIVED );//praneeth modified NA as RECEIVED

       /* Need to clear the timer; so create the lookup key. */
       create_request_timer_key( lookup_key , &auth_tx);

              /* Now clear the request timer;
               * exit code while waiting for timer's response.
               * Timer's response will contain original auth_tx.
               */
       if ( false == clear_request_timer(lookup_key,&auth_tx) )
       {
                 sprintf( Buffer,
                         "Unable to clear request on response from host. Msg type: %s, DE7: %s",
  					   auth_tx.TLF01_details.message_type,
  					   auth_tx.TLF01_details.transmission_timestamp );
                 ncamex_log_message(1, 2, Buffer, "incoming_host_message" ,1,"");
                 return(false);
      }


       return(true);
   }
   else
   {
   memset (lookup_key, 0, sizeof(lookup_key));
   strcpy (lookup_key, cur_app_name);
   if ( auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      // There is no terminal Id, so use last 8 digits of pan
      pan_len = strlen( auth_tx.TLF01_details.card_num );
      memcpy( tid, &auth_tx.TLF01_details.card_num[pan_len - 8], 8 );
      strcat( lookup_key, tid );
   }
   else
   {
      strncat(lookup_key, auth_tx.TLF01_details.terminal_id,         (sizeof(lookup_key) - strlen(lookup_key)) - 1);
   }
   strncat(lookup_key, auth_tx.TLF01_details.sys_trace_audit_num, (sizeof(lookup_key) - strlen(lookup_key)) - 1);

   //build PTEMSG to get Timer Data from the Host Data Server
   p_msg_timer_out = ptetimer_bld_read_timer_req( lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build read timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }
   
   memset(buffer, 0, sizeof(buffer));
   memcpy(buffer,&auth_tx, sizeof(AUTH_TX));
   p_msg_chain = ptemsg_chain_new_message (p_msg_timer_out,
                                           MT_DB_REQUEST,
                                           ST1_DB_ECHO,
                                           ST2_NONE,
                                           timerds_que_name,
                                           application_que_name,
                                           buffer,
                                           sizeof(buffer),
                                           0);
                                           

   // get Timer message from Host Data Server now
   ret_code = pteipc_send( p_msg_chain, timerds_que_name);
   
   if(ret_code < 0)
   {   // failed getting timer data
       pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "pteipc error sending to que %s: %s", timerds_que_name, ErrorMsg );

       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
       			auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
       strcat(Buffer,Log_Bufr);
       ncamex_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
   }
   free( p_msg_chain);
   return(true);
}


}

/*****************************************************************************

  Function:    incoming_host_message_timer_reply

  Description: This function will receive a reply from the data server in 
               response to a a read timer in the incoming host path.
  Author: 
      unknown
  Inputs:
      p_msg_in -  a pointer to the incoming timer message
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT incoming_host_message_timer_reply(pPTE_MSG p_msg_in)
{  
   pPTE_MSG    p_msg_timer_out = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR;
   pPTE_MSG    p_msg_out = NULL_PTR;
   pPTE_MSG    p_echo_msg = NULL_PTR;
   pBYTE       p_data;
   LONG        timer_reply_size = 0;
   CHAR        app_data1[32];
   CHAR        app_data2[32];
   CHAR			lookup_key[22];
   BYTE        buffer[sizeof(AUTH_TX)];
   CHAR			Buffer[512]={0};
   CHAR			ErrorMsg[512]={0};
   LONG			ret_code;
   LONG        data_len;
   pPTE_MSG    p_msg_ins = NULL_PTR;
   AUTH_TX     auth_tx;
   AUTH_TX     temp_auth_tx;
   BYTE        tran_type;


   timer_reply_size = ptetimer_get_timer_reply(p_msg_in, lookup_key, app_data1, app_data2, buffer, sizeof(buffer));
   if (timer_reply_size == sizeof(buffer))
   {
      memset(&temp_auth_tx, 0, sizeof(AUTH_TX));
      memcpy(&temp_auth_tx, buffer, sizeof(AUTH_TX));
   }
   else
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," No data returned with the get timer message Term Id:%s ,Merch Id: %s RRN: %s ",
    		  temp_auth_tx.TLF01_details.terminal_id,temp_auth_tx.TLF01_details.merchant_id,temp_auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 2, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,temp_auth_tx.TLF01_details.card_num);
      return(false);
   }

   p_echo_msg  = ptemsg_get_chained_pte_msg (p_msg_in,2);
   p_msg_data  = ptemsg_get_pte_msg_data (p_echo_msg);
   p_data      = ptemsg_get_pte_msg_data_data (p_msg_data);
   data_len    = ptemsg_get_pte_msg_data_data_length (p_msg_data);

   if (data_len == sizeof(buffer))
   {
       memset(&auth_tx, 0, sizeof(AUTH_TX));
       memcpy(&auth_tx, p_data, sizeof(AUTH_TX));
   }
   else
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," No data returned with the get timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }

   p_msg_timer_out = ptetimer_bld_clear_timer_req( lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }
   

   ret_code = pteipc_send( p_msg_timer_out, timerds_que_name);
   

   if(ret_code < 0)
   {
       pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "pteipc error sending to que %s: %s", timerds_que_name, ErrorMsg );
       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
       		   auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
       strcat(Buffer,Log_Bufr);
       ncamex_log_message( 3, 3, Buffer , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
       return(false);
   }
   free( p_msg_timer_out);


   if ( true == ncpos_check_for_cadencieacq_transaction(&auth_tx))
   {

   }
   else
   {
   tran_type = match_request_to_response(temp_auth_tx.TLF01_details.tx_key);
   temp_auth_tx.TLF01_details.tx_key = tran_type;
   }

   if (AUTH_REVERSAL_ADVICE_RESPONSE == tran_type)
   {
      return(true);
   }

   /* stuff some data into tmp_auth_tx */
   if (false == alter_transaction_data(&auth_tx, &temp_auth_tx) )
      return(false);
  
   if ( temp_auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      (void)process_file_update_response( &temp_auth_tx );
	   //return true;
   }

   else if ( true == ncpos_check_for_cadencieacq_transaction(&auth_tx))
   {
   	/* send repsonse directly to nposa2
   	 * */
	   memcpy(temp_auth_tx.req_data_len,auth_tx.req_data_len,sizeof(auth_tx.req_data_len));
	   memcpy(temp_auth_tx.req_buffer,auth_tx.req_buffer,sizeof(auth_tx.req_buffer));
	   ncpos_transfer_response_msg_to_nposa2(&temp_auth_tx);

   }
   /* Forward the response to the authorizer. */
   else if (false == send_response_to_term_app(&temp_auth_tx))
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to send message to authorizer Term Id:%s ,Merch Id: %s RRN: %s ",
      		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }

   PRINT("Processing host response: end \n");

   return(true);

} /* incoming_host_response_timer_reply */

INT incoming_host_message_timer_clear(pPTE_MSG p_msg_in)
{
   pPTE_MSG    p_msg_timer_out = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR;
   pPTE_MSG    p_msg_out = NULL_PTR;
   pPTE_MSG    p_echo_msg = NULL_PTR;
   pBYTE       p_data;
   LONG        timer_reply_size = 0;
   CHAR        app_data1[32];
   CHAR        app_data2[32];
   CHAR			lookup_key[22];
   BYTE        buffer[sizeof(AUTH_TX)];
   CHAR			Buffer[512]={0};
   CHAR			ErrorMsg[512]={0};
   LONG			ret_code;
   LONG        data_len;
   pPTE_MSG    p_msg_ins = NULL_PTR;
   AUTH_TX     auth_tx;
   AUTH_TX     temp_auth_tx;
   BYTE        tran_type;


   timer_reply_size = ptetimer_get_timer_reply(p_msg_in, lookup_key, app_data1, app_data2, buffer, sizeof(buffer));
   if (timer_reply_size == sizeof(buffer))
   {
      memset(&temp_auth_tx, 0, sizeof(AUTH_TX));
      memcpy(&temp_auth_tx, buffer, sizeof(AUTH_TX));
   }
   else
   {

      return(false);
   }

   p_echo_msg  = ptemsg_get_chained_pte_msg (p_msg_in,2);
   p_msg_data  = ptemsg_get_pte_msg_data (p_echo_msg);
   p_data      = ptemsg_get_pte_msg_data_data (p_msg_data);
   data_len    = ptemsg_get_pte_msg_data_data_length (p_msg_data);

   if (data_len == sizeof(buffer))
   {
       memset(&auth_tx, 0, sizeof(AUTH_TX));
       memcpy(&auth_tx, p_data, sizeof(AUTH_TX));
   }
   else
   {
      return(false);
   }

   p_msg_timer_out = ptetimer_bld_clear_timer_req( lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_clear" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }


   ret_code = pteipc_send( p_msg_timer_out, timerds_que_name);


   if(ret_code < 0)
   {
       pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "pteipc error sending to que %s: %s", timerds_que_name, ErrorMsg );
       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
       		   auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
       strcat(Buffer,Log_Bufr);
       ncamex_log_message( 3, 3, Buffer , "incoming_host_message_timer_clear" , 0 ,auth_tx.TLF01_details.card_num);
       return(false);
   }
   free( p_msg_timer_out);


   if ( true == ncpos_check_for_cadencieacq_transaction(&auth_tx))
   {

   }
   else
   {
	   tran_type = match_request_to_response(temp_auth_tx.TLF01_details.tx_key);
	   temp_auth_tx.TLF01_details.tx_key = tran_type;
   }

   if (AUTH_REVERSAL_ADVICE_RESPONSE == tran_type)
   {
      return(true);
   }

   /* stuff some data into tmp_auth_tx */
   if (false == alter_transaction_data(&auth_tx, &temp_auth_tx) )
      return(false);

   if ( temp_auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
	   (void)process_file_update_response( &temp_auth_tx );
	   //return true;
   }

   else if ( true == ncpos_check_for_cadencieacq_transaction(&auth_tx))
   {
   	/* send repsonse directly to nposa2
   	 * */
	   memcpy(temp_auth_tx.req_data_len,auth_tx.req_data_len,sizeof(auth_tx.req_data_len));
	   memcpy(temp_auth_tx.req_buffer,auth_tx.req_buffer,sizeof(auth_tx.req_buffer));
	   ncpos_transfer_response_msg_to_nposa2(&temp_auth_tx);

   }
   	   	   /* Forward the response to the authorizer. */
   else if (false == send_response_to_term_app(&temp_auth_tx))
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to send message to authorizer Term Id:%s ,Merch Id: %s RRN: %s ",
      		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_clear" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }

   return(true);

} /* incoming_host_response_timer_reply */


/*****************************************************************************

  Function:    process_timeout_msg

  Description: This function will process a timeout message by getting the auth_tx
               from the timer timeout message and creating a timeout response to the
               terminal.
  Author: 
      unknown
  Inputs:
      p_msg - the incoming timeout message from the timer
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT process_timeout_msg(pPTE_MSG p_msg)
{
   BYTE        buffer2[sizeof(AUTH_TX)];
   CHAR        lookup_key[MAX_TIMER_KEY_SIZE];
   CHAR        app_data1[MAX_APP_DATA_SIZE];
   CHAR        app_data2[MAX_APP_DATA_SIZE];
   CHAR	      	Buffer[512] = {0};
   CHAR		   ErrorMsg[512] = {0};
   AUTH_TX     auth_tx;
   LONG        timer_reply_size = 0;
   INT         tempint;
   CHAR        tempstr[10];
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   //HCTS1-A
   CHAR        echo_timer_key[LOOKUP_KEY_SIZE]= "";
   CHAR        echo_req_timer_key[LOOKUP_KEY_SIZE]= "";
   
	/* get timeout values */
   timer_reply_size = ptetimer_get_timer_reply(p_msg, lookup_key, app_data1, app_data2,buffer2,sizeof(buffer2));

   //HCTS1-A
   if ( 0 == strcmp(lookup_key, echo_lookup_key) )
   {
      /* 1 . Echo timer is automatically cleared when it times out
       * 2 . Send ECHO request to host (this sets the echo timer)
       * 3 . Flag timer code that echo timer is not set.
       */
	   if((set_echo_timer(echo_lookup_key,echo_timer_length,NULL_PTR,application_que_name,1)) == false)
	   {
		   ncamex_log_message( 3, 1, "Failed to send ECHO TEST to host.", "process_timeout_msg" , 0 ,"");
	   }

	   memset (&auth_tx, 0, sizeof(auth_tx));
	   if (false == perform_op_net_mngmnt_request(ECHO_REQUEST, &auth_tx))
	   {
	       ncamex_log_message( 3, 1, "Error executing host Echo Test" , "process_timeout_msg" , 0 ,"");
	   }

	   return(true);
   }
   else if (0 == strcmp(lookup_key, echo_request_timer_key) )
   {
       /* An Echo request has timed out. Take no action here  */
       /* Allow the echoes to continue going out as normal.   */
       /* --------------------------------------------------- */

	   return(true);
   }

   if (timer_reply_size == sizeof(buffer2))
   {
      memset(&auth_tx, 0, sizeof(auth_tx));
      memcpy(&auth_tx, buffer2, sizeof(AUTH_TX));

      /* Display time out message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants.
       */
      if (true == ncpos_check_for_cadencieacq_transaction(&auth_tx))
      {
     	 // dont print error message
      }
      else
      {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Txn to Amex timed out. Card %s, Merchant %s",
               auth_tx.TLF01_details.card_num,
               auth_tx.TLF01_details.merchant_id );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 1, Buffer , "process_timeout_msg" , 0 ,auth_tx.TLF01_details.card_num);
   }   
   }   
   else
   {
      ncamex_log_message( 3, 2, "Error retrieving auth_tx from timer message" , "process_timeout_msg" , 0 ,"");
      return(false);
   }
  

   /* update the transaction in the auth_tx table to TIMED OUT */
   auth_tx.TLF01_details.general_status = GS_TXN_TIMEDOUT;


   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   /* decrement the active transactions counter field */
   tempint = atoi(active_txns_ctr) -1;
   if (tempint < 0)
   { /* if less than zero set it to zero */
      strcpy(active_txns_ctr,"0"); 
   }
   else
   {
      itoa(tempint, tempstr, 10);
      strcpy(active_txns_ctr,tempstr); 
   }

   /* increment the number of times the host has failed */

   if (atoi(net_consec_tmouts_ctr) >= atoi(ncf01.max_consecutive_timeouts))
   { /* set state to force logon at next incoming */
      strcpy(current_state,"2");
   }
   else
   {
      tempint = atoi(net_consec_tmouts_ctr) + 1;
      itoa(tempint, tempstr, 10);
      strcpy(net_consec_tmouts_ctr,tempstr);
   }

   /* Update host state and counters */
   WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   send_network_status_to_monitor();

   if ( (AUTH_REVERSAL_ADVICE == auth_tx.TLF01_details.tx_key))
   {
///      if (count = atoi(auth_tx.advice_repeat_count) < atoi(ncf01.advice_repeat_counter))
      {
         /* Increment the advice count */
///         count++;
///         itoa(count, auth_tx.advice_repeat_count, 10);

         /* Send a reversal repeat message to the host */
         auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT;
         strcpy(auth_tx.TLF01_details.message_type, "0421");
         send_request_to_host(&auth_tx);
      }
   }
   else
   {
      if ( (AUTH_LOGON                != auth_tx.TLF01_details.tx_key) &&
           (AUTH_CARD_UPDATE          != auth_tx.TLF01_details.tx_key) &&
           (AUTH_CARD_UPDATE_RESPONSE != auth_tx.TLF01_details.tx_key)  )
      {
         /* Prepare time out message for terminal handler. */
         auth_tx.TLF01_details.tx_key = get_tran_type( &auth_tx );

         /* Send time out message to terminal handler. */
         perform_standin_processing( "TO", GS_TXN_TIMEDOUT, &auth_tx);

         /* Send a reversal message to the host */
     	if (true == ncpos_check_for_cadencieacq_transaction(&auth_tx))
     	{
     			/* Dont take any action for cadencie acquired txn
     			 * */
     	}
     	else
     	{
         auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
         strcpy(auth_tx.TLF01_details.message_type, "0420");
         strcpy(auth_tx.TLF01_details.response_code,"68" );
         strcpy(auth_tx.TLF01_details.reversal_amount,"0" );  // Indicates that entire amount is reversed

         send_request_to_host(&auth_tx);
      }
   }
   }

   return(true);
 }  /* process_timeout_msg */

/*****************************************************************************

  Function:    send_response_to_term_app

  Description: This function will update the transaction counters (number of
               active transactions and number of timed out transactions) and 
               sends the message to the authorizer.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure to be sent to the authorizer
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_response_to_term_app(pAUTH_TX p_auth_tx)
{
   pPTE_MSG    p_msg_term_handler_out = NULL_PTR;
   LONG        ret_code;
   CHAR	      Buffer[512] = {0};
   CHAR	      ErrorMsg[512] = {0};
   INT         tempint;
   CHAR        tempstr[10];
   CHAR        temp_orig_que[15] = "";
  	BYTE        buffer1[sizeof(AUTH_TX)];
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];


   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );
   /* decrement the active transactions counter field */
   tempint = atoi(active_txns_ctr) -1;
   if (tempint < 0)
   { /* if less than zero set it to zero */
      strcpy(active_txns_ctr,"0"); 
   }
   else
   {
      itoa(tempint, tempstr, 10);
      strcpy(active_txns_ctr,tempstr); 
   }

   /* this transactions came back so the host is responding 
      so reset the consective timeouts counter */
   strcpy(net_consec_tmouts_ctr,"0");

   /* Update host state and counters */
   WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   (void)send_network_status_to_monitor();

   /* build the message to be sent to the terminal handler */
   strcpy(temp_orig_que, authorizer_que_name);

   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1, p_auth_tx, sizeof(AUTH_TX));
   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              temp_orig_que,
								                      application_que_name,
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1),
                                              0 );

   ret_code = pteipc_send (p_msg_term_handler_out, authorizer_que_name);  
   if ( ret_code < 0 )
   {
 		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s", authorizer_que_name, ErrorMsg );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 3, Buffer , "send_response_to_term_app" , 0 ,p_auth_tx->TLF01_details.card_num);
      free(p_msg_term_handler_out);
      return(false);
   }
   else
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Response sent successfully to authorizer Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncamex_log_message( 2, 1, Log_Bufr , "send_response_to_term_app" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   free(p_msg_term_handler_out);
   return(true);
} /* send_response_to_term_app */

/*****************************************************************************

  Function:    alter_transaction_data

  Description: This function will update the original message's auth_tx 
               structure with the data returned by the host.
  Author: 
      unknown
  Inputs:
      p_auth_tx  - the auth_tx structure from the host
      p_temp_auth_tx - the auth_tx structure from the original message.
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
int alter_transaction_data(pAUTH_TX p_auth_tx, pAUTH_TX p_temp_auth_tx)
{                                     
   INT    ret_value = true;

   if ( AUTH_INVALID == p_temp_auth_tx->TLF01_details.tx_key )
   {
      /* this transaction is not understood by the server and will be dropped */
      /* fsvisa code says to set terminal status to GS_STATUS_IDLE */
	   return (false);
   }   
   
   if ( AUTH_BALANCE_INQUIRY_RESPONSE == p_temp_auth_tx->TLF01_details.tx_key )
   {   /* replace the amount in the auth_tx table with the amount in the response */
	   strcpy(p_temp_auth_tx->TLF01_details.total_amount, p_auth_tx->TLF01_details.total_amount);
   }
   

   p_temp_auth_tx->TLF01_details.general_status = GS_TXN_COMPLETED;
   
   strcpy(p_temp_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx->TLF01_details.retrieval_ref_num);
	strcpy(p_temp_auth_tx->TLF01_details.sys_trace_audit_num, p_auth_tx->TLF01_details.sys_trace_audit_num);
   strcpy(p_temp_auth_tx->TLF01_details.settlement_date, p_auth_tx->TLF01_details.settlement_date);

   if (p_auth_tx->TLF01_details.auth_number [0] != '\0')
       strcpy(p_temp_auth_tx->TLF01_details.auth_number, p_auth_tx->TLF01_details.auth_number );

   strcpy(p_temp_auth_tx->TLF01_details.response_code, p_auth_tx->TLF01_details.response_code);

   if (p_auth_tx->TLF01_details.product_codes[12].code [0] != '\0')
   strcpy(p_temp_auth_tx->TLF01_details.product_codes[12].code, p_auth_tx->TLF01_details.product_codes[12].code);

   strcpy( p_temp_auth_tx->TLF01_details.response_text,
           p_auth_tx->TLF01_details.response_text );

   if (p_auth_tx->TLF01_details.date_yyyymmdd [0] != '\0')
      strcpy(p_temp_auth_tx->TLF01_details.date_yyyymmdd, p_auth_tx->TLF01_details.date_yyyymmdd);

   if (p_auth_tx->TLF01_details.time_hhmmss [0] != '\0')
      strcpy(p_temp_auth_tx->TLF01_details.time_hhmmss, p_auth_tx->TLF01_details.time_hhmmss);

	if (p_auth_tx->TLF01_details.currency_code [0] != '\0')
	{
		if(true==ncpos_check_for_cadencieacq_transaction(p_auth_tx))
		{

		}
		else
		{
      strcpy(p_temp_auth_tx->TLF01_details.currency_code, p_auth_tx->MCF01_details.currency_code);
		}
	}

	
	strcpy(p_temp_auth_tx->TLF01_details.resp_source_len, p_auth_tx->TLF01_details.resp_source_len);
	strcpy(p_temp_auth_tx->TLF01_details.resp_source, p_auth_tx->TLF01_details.resp_source);
	p_temp_auth_tx->TLF01_details.cvc = p_auth_tx->TLF01_details.cvc;
	strcpy(p_temp_auth_tx->TLF01_details.mcard_banknet_len, p_auth_tx->TLF01_details.mcard_banknet_len);
	strcpy(p_temp_auth_tx->TLF01_details.mcard_banknet, p_auth_tx->TLF01_details.mcard_banknet);
	strcpy(p_temp_auth_tx->TLF01_details.agent_id_len, p_auth_tx->TLF01_details.agent_id_len);
	strcpy(p_temp_auth_tx->TLF01_details.agent_id, p_auth_tx->TLF01_details.agent_id);
	
   ptetime_get_strtime(p_temp_auth_tx->TLF01_details.host_finish_time);

   /* Amex Response Code - used for Online Updates (1304/1314) */
   strcpy( p_temp_auth_tx->accptor_bus_code, p_auth_tx->accptor_bus_code );
   /* Copy DE25 reason code from response TF GIRIJA, PHANI */
   strcpy(p_temp_auth_tx->TLF01_details.product_codes[13].code, p_auth_tx->TLF01_details.product_codes[13].code);
    /* TF Phani- Copy TID */   
	strcpy(p_temp_auth_tx->TLF01_details.visa_tran_id, p_auth_tx->TLF01_details.visa_tran_id);
	strcpy(p_temp_auth_tx->TLF01_details.visa_validation_code, p_auth_tx->TLF01_details.visa_validation_code);

	return(true);

}  /* alter_transaction_data */

/*****************************************************************************

  Function:   send_request_to_host 

  Description: This function will start the timer and send a message to the host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx to be saved with the timer message.  Used to make the
                  message to the host.
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_request_to_host( pAUTH_TX p_auth_tx )
{
   pPTE_MSG	p_msg_set_timer = NULL_PTR;
   pPTE_MSG p_msg_term_hdlr = NULL_PTR;
   CHAR     lookup_key[22];
   LONG     ret_code;
   CHAR     ErrorMsg[512] = {0};
   CHAR     Buffer[512] = {0};
   BYTE     buffer1[sizeof(AUTH_TX)];
   WORD     timer_length;
   pPTE_MSG p_msg_host_out  = NULL_PTR;
   CHAR     gmt_time[20];
   INT      pan_len;
   CHAR     tid[9] = {0};
   BYTE   tx_key;
   BYTE   error_buffer[100] = {0};
   CHAR   temp_mid[16]    = {0};
   CHAR   key_last_six[7] = {0};
   INT    mid_len = 0;
   INT   buf_temp_len=0;
   WORD     request_timer = 0;
   CHAR     err_msg[512]={0};
   //INT buflen=0;
   /* Build PTEMSG */
   if (GS_TXN_NEW == p_auth_tx->TLF01_details.general_status)
      p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;

   /* Current Greenwich Mean Time for the transmission-to-host time. */
   if ( 0 == strcmp( NULL_STR,
                     p_auth_tx->TLF01_details.transmission_timestamp ) )
   {
      pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
      strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
               &gmt_time[4], 10 );  /* MMDDhhmmss */
   }
   ptetime_get_strtime( p_auth_tx->TLF01_details.host_start_time );
   /* Fill ReqRsp_Buffer from HOST_MESSAGE*/
   if ( p_auth_tx->TLF01_details.general_status == GS_TXN_PERF_REQ )
   {

      build_host_structure(p_auth_tx, "000");
      if (Build_Host_Request( p_auth_tx ))
      {
         /* This must be set prior to setting the timer; it gets saved there. */
         ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);

         if(true== ncpos_check_for_cadencieacq_transaction(p_auth_tx))
          {
                 		    buf_temp_len = atoi(p_auth_tx->resp_data_len);
                 		  	if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
                 		  	{	buflen = buf_temp_len;
                 		  		memcpy(ReqRsp_Buffer,p_auth_tx->resp_buffer,buflen);
                 		  	}

                 		  	create_request_timer_key(lookup_key,p_auth_tx);
          }
         else
         {
         /* Create the lookup key for the timer. */
         memset(lookup_key, 0x00, sizeof(lookup_key));
         strcpy(lookup_key, AppName);

         if ( (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE) ||
              (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE) )
         {
            /* There is no terminal Id, so use last 8 digits of pan. */
            pan_len = strlen( p_auth_tx->TLF01_details.card_num );
            memcpy( tid, &p_auth_tx->TLF01_details.card_num[pan_len - 8], 8 );
            strcat( lookup_key, tid );
         }
         else
         {
            strcat(lookup_key, p_auth_tx->TLF01_details.terminal_id);
         }
         strcat(lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num);

         }
         /* Build Start Timer message to send to Host Data Server */		
         memcpy(buffer1,p_auth_tx, sizeof(AUTH_TX) );
         timer_length = atoi(ncf01.request_timer);
         p_msg_set_timer = ptetimer_bld_start_timer_req(lookup_key,
                         p_auth_tx->TLF01_details.primary_key.transaction_id,
                         p_auth_tx->TLF01_details.terminal_id,
                         timerds_que_name,
                         application_que_name,
                         timer_length,
                         0,
                         (pBYTE)&buffer1,  
                         sizeof(buffer1)); 

         if ( NULL_PTR == p_msg_set_timer )
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Insufficient Memory to build start timer message Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            ncamex_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
            return(false);
         }

         /* We can chain the request_timer message with set & clear
          * echo timer request to timerds 
          */
         if(false == chain_echo_host(echo_lookup_key, echo_timer_length,application_que_name,p_msg_set_timer))
         {
             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
             sprintf(Log_Bufr," Error chaining the timer request with clear & set the echo_timer Term Id:%s ,Merch Id: %s RRN: %s ",
             		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            ncamex_log_message( 3, 3, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
         }



         p_msg_host_out = ptemsg_build_msg(MT_OUTGOING, 
                                           ST1_NONE,
                                           ST2_NONE,
                                          "dialogA",
                                           application_que_name,
                                           ReqRsp_Buffer,
                                           buflen,
                                           0 );
         ptemsg_set_orig_comms_info( p_msg_host_out,
                                     p_auth_tx->TLF01_details.tpdu_id );
         if ( p_msg_host_out == NULL_PTR )
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            ncamex_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
             return(false);
         }
		 
         ret_code = pteipc_send(p_msg_host_out, "dialogA");
         if( ret_code < 0 )
         {
            pteipc_get_errormsg( ret_code, ErrorMsg );
            sprintf( Buffer, "error on pteipc_send to que %s: %s",
                    "dialogA", ErrorMsg );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncamex_log_message( 3, 2, Buffer , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
            free( p_msg_host_out );
            return(false);
         }
         else
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, p_auth_tx, SENT );
    
            /* Log transaction to trace file. */
            write_to_txn_file();
         }
         free( p_msg_host_out );
      }
      else
      {
         return(false);
      }
   }

   return(true);
}

/*****************************************************************************

  Function:   send_request_to_host_timer_reply

  Description: This function will start the timer and send a message to the host.
  Author: 
      unknown
  Inputs:
      p_msg_in - a pointer to the structure that contains the auth_tx to be 
                 used to make the message to the host.
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_request_to_host_timer_reply( pPTE_MSG p_msg_in )
{
   pPTE_MSG p_msg_host_out    = NULL_PTR;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   pBYTE           pCurrent  = NULL_PTR;
   CHAR     orig_que_name[17]   = "";
   BYTE     net_consec_tmouts_ctr[4];
   BYTE     active_txns_ctr[4];
   BYTE     current_state[2];
   LONG     ret_code;
   CHAR     ErrorMsg[512] = {0};
   CHAR     Buffer[512]   = {0};
   AUTH_TX  auth_tx;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset (&auth_tx, 0, sizeof(AUTH_TX));
   memcpy(&auth_tx, pCurrent, sizeof(AUTH_TX));

   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   if (AUTH_REVERSAL == auth_tx.TLF01_details.tx_key)
   {
      auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
      strcpy(auth_tx.TLF01_details.message_type,"0420");
   }

   /* fill ReqRsp_Buffer from HOST_MESSAGE*//* during dropped use temp_auth_tx.TLF01_details.tx.key */
   if ((2 == atoi(current_state)) || (GS_TXN_PERF_REQ == auth_tx.TLF01_details.general_status))
   {
///  BETH, need to create the correct response code before sending to host
       //build_host_structure(&auth_tx);
       if (Build_Host_Request( &auth_tx ))
       {
//          Build_Host_Header(&auth_tx);                      No header for JCB
          p_msg_host_out = ptemsg_build_msg(MT_OUTGOING, 
                                               ST1_NONE,
                                               ST2_NONE,
								                       auth_tx.TLF01_details.originator_queue,
                                               application_que_name,
                                               ReqRsp_Buffer,
                                               buflen,
                                               0 );
          (void)set_originator_info( &auth_tx, p_msg_host_out );


          if ( NULL_PTR == p_msg_host_out )
	       {
             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
             sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
             		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
             ncamex_log_message( 3, 2, Log_Bufr , "send_request_to_host_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
	          return(false);
	       }

          ret_code = pteipc_send(p_msg_host_out, auth_tx.TLF01_details.originator_queue);
	       if( ret_code < 0 )
     	    {
	          pteipc_get_errormsg( ret_code, ErrorMsg );
             sprintf( Buffer, "error on pteipc_send to que %s: %s",
                      auth_tx.TLF01_details.originator_queue, ErrorMsg );

             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
             sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
             		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
             strcat(Buffer,Log_Bufr);
             ncamex_log_message( 3, 3, Buffer , "send_request_to_host_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
             free( p_msg_host_out );
             return(false);
	       }
          else
          {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, &auth_tx, SENT );

            /* Log transaction to trace file. */
            write_to_txn_file();
          }
	       free( p_msg_host_out );
       }
       else
       {
          return(false);
       }
       //HCTS1-A It keeps on displaying for echo msgs, commented in line no. 2295 below
       ncamex_log_message( 1, 1, "Transaction sent to host" , "send_request_to_host_timer_reply" , 0 ,"");
   }
   strcpy(orig_que_name, authorizer_que_name);
//   if (blank_string(auth_tx.TLF01_details.last_host_app_queue, strlen(auth_tx.TLF01_details.last_host_app_queue)) == false)
//      strcpy(orig_que_name, auth_tx.TLF01_details.last_host_app_queue);
   //ncamex_log_message( 1, 1, "Transaction sent to host" , "send_request_to_host_timer_reply" , 0 ,"");
   return(true);
}  /* send_request_to_host */

/*****************************************************************************

  Function:    verify_host_app_status

  Description: This function will check the active transaction counter
               and the host state (both stored in shared memory) to 
               determine whether the host is "up"
  Author: 
      unknown
  Inputs:
      p_auth_tx - used to determine whether this transaction is a reversal
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
   Modified by:
      BGable      6/30/98
******************************************************************************/
INT  verify_host_app_status(pAUTH_TX p_auth_tx)
{
   INT   tempint;
   CHAR  tempstr[14];
   BYTE  net_consec_tmouts_ctr[4];
   BYTE  active_txns_ctr[4];
   BYTE  current_state[2];


   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   if (strcmp(current_state,"0") != 0) 
   {
	   ncamex_log_message( 3, 2, "Host app not in state 0, cannot process tran" , "verify_host_app_status" , 0 ,"");
	   return(false);
   }

   if (2 == atoi(current_state))  
   {
	   ncamex_log_message( 3, 2, "Maximum consecutive timeouts reached" , "verify_host_app_status" , 0 ,"");
	   strcpy(current_state,"2"); 
	   return(false);
   }
   if (atoi(active_txns_ctr) >= atoi(ncf01.max_active_txns))
   {
      ncamex_log_message( 3, 2, "Maximum active transactions reached" , "verify_host_app_status" , 0 ,"");
	  return(false);
   }

   /* increment the number of active transactions for this host */
	if (p_auth_tx->TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE)
	{
		tempint = atoi(active_txns_ctr) + 1;
		memset(&tempstr, 0x00, sizeof(tempstr));
		itoa(tempint,tempstr,10);
		strcpy(active_txns_ctr, tempstr ); 
	}

	return(true);
}; /* verify_host_app_status */

/*****************************************************************************

  Function:    perform_host_logon_request

  Description: This function will send a logon request to the host to ensure
               that the host is up and running.
  Author: 
      unknown
  Inputs:
      request_type - the type of request
      source - indicates the source of the logon request
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT perform_host_logon_request(char request_type[], int source,
                               pAUTH_TX p_auth_tx)
{
   char tempstr[10] ="";
   BYTE net_consec_tmouts_ctr[4];
   BYTE active_txns_ctr[4];
   BYTE current_state[2];

   memset( &vLogon_Resp_Indicator, 0x00, sizeof(vLogon_Resp_Indicator));

   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   switch (source)
	{
		case SOURCE_IS_FSVISA:
		{
			if (2 == atoi(current_state))
			{
				ncamex_log_message( 3, 1, "Sending logon request to host:" , "perform_host_logon_request" , 0 ,"");
				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
				strcpy(vLogon_NMI_Code, LOGON_NMI_LOGON);

			}
			else
				return (true);
			break;
		} /* end case SOURCE_IS_FSVISA */

		case SOURCE_IS_STATE:
		{
			if (0 == atoi(current_state))
			{
            	ncamex_log_message( 1, 1, "Sending logon ECHO request to host:" , "perform_host_logon_request" , 0 ,"");
				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
				strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);
				send_request_to_host(p_auth_tx);
			}
			break;
		} /* end case SOURCE_IS_STATE */

      case SOURCE_IS_OPERATOR:
      {
         if (false == perform_op_net_mngmnt_request(request_type, p_auth_tx))
            return(false);
         else
            return(true);
      }
        
		case SOURCE_IS_HOST:
		{
			if (LOGON_NMI_START == request_type)
			{
			    if (0 == atoi(current_state))
				{
			    	ncamex_log_message( 1, 1, "Received logon request from host:" , "perform_host_logon_request" , 0 ,"");
 					p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					strcpy(vLogon_NMI_Code, LOGON_NMI_START);
                    perform_op_net_mngmnt_response(request_type, p_auth_tx);
				}
				else
					return(false);
			}
			else
			{
				if (0 == atoi(current_state))
				{
					//ncamex_log_message( 3, 1, "Received network request from host:" , "perform_host_logon_request" , 0 ,"");
					p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
					strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);
					strcpy(vLogon_Resp_Indicator, LOGON_RESP_INDICATOR);
					perform_op_net_mngmnt_response(request_type, p_auth_tx);
				}
				else
					return(false);
			}
			break;
		} /* end case SOURCE_IS_HOST */

   } /* end switch statement */

	return (true);

};  /* perform_host_logon_request */

/*****************************************************************************

  Function:    send_network_status_to_monitor

  Description: This function will send the network status to the monitor.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_network_status_to_monitor(void)
{

   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   LONG        ret_code;
   CHAR        err_mesg[512] = {0};
   CHAR        Buff[512]={0};
	CHAR ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
	XINT ReturnCode = 0 ;
	int nIndex = 0 ;
 

	   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   memset( &ncf01, 0, sizeof(NCF01) );
   strncpy( ncf01.primary_key.network_id,    AppName , strlen(AppName) );
   strcpy( ncf01.primary_key.network_type, "A"           );
   strcpy( ncf01.status,                   current_state );

   p_msg_ins = ptemsg_build_msg( MT_SYS_MONITOR,
                                 ST1_NETWORK_UPDATE, 
                                 ST2_NONE,
                                 applnk_que_name, 
                                 application_que_name,
                                 (pBYTE)&ncf01,
                                 sizeof(ncf01), 
                                 0 );
   if ( NULL_PTR == p_msg_ins )
   {
      ncamex_log_message( 3, 2, "Insufficient Memory to create network query message" , "send_network_status_to_monitor" , 0 ,"");
      return( false );
   }

	memset (ConnectionId, 0, sizeof (ConnectionId)) ;
	ReturnCode = ReadMemMonitorTable (ConnectionId) ;
	if (ConnectionId[0] != 0)
   {
      ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
      /* Send message to get host specific data from data server. */
      ret_code = pteipc_send( p_msg_ins, applnk_que_name );
      if ( 0 > ret_code )
      {
         pteipc_get_errormsg( ret_code, err_mesg );
         sprintf(Buff,"Failed transmit to queue: %s. %s",applnk_que_name,err_mesg);
         ncamex_log_message( 1, 3, Buff , "send_network_status_to_monitor" , 0 ,"");
         free( p_msg_ins );
         return( false );
      }
   }
   free( p_msg_ins );
   return (true);

};  /* send_network_status_to_monitor */

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
 *  MODIFICATIONS:
 ******************************************************************************/
INT perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx )
{
   CHAR     gmt_time[15];
   AUTH_TX  auth_tx;
   CHAR     amex_response_code[4];
   CHAR     temp[17] = "";
   //HCTS1-A
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   WORD     timer_length;

   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
   if (LOGON_REQUEST == request_type)
      strncpy( auth_tx.function_code, LOGON_NMI_LOGON, 3);  // 801 = sign on
   else if (LOGOFF_REQUEST == request_type)
   strncpy( auth_tx.function_code, LOGON_NMI_LOGOFF, 3);  // 802 = sign off
   else 
   strncpy( auth_tx.function_code, LOGON_NMI_ECHO_TEST, 3);  // 801 = echo test

   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
   strncpy(tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));
   if( strcmp(ncf01.logon_bin[0].identifier,"SI") == 0)
   {
       si=1;
   }
   else
       si=0;
   auth_tx.TLF01_details.tx_key = AUTH_LOGON;

   /* Current Time for the transmission-to-host time  */
   ptetime_get_strtime(auth_tx.TLF01_details.host_start_time);
   
   /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
   if ( 0 == strcmp( NULL_STR, auth_tx.TLF01_details.retrieval_ref_num))
   {
      create_rrn( auth_tx.TLF01_details.retrieval_ref_num );
   }

   /* Use the time hhmmss for sys_trace_audit_num */
   memset(gmt_time, 0, sizeof(gmt_time));
   ptetime_get_strtime (gmt_time);
   Rj_with_lead_zeros(gmt_time, sizeof(gmt_time)-1);
   memcpy (auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 7, 3); 
   strncat(auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 11,3);

   strncpy( amex_response_code, "000", 3);
   strcpy ( auth_tx.forwarding_inst_id_cd, ncf01.forwarding_inst_id_code);
	strcpy ( auth_tx.TLF01_details.processing_code, "000000");
	strcpy ( auth_tx.host_processing_code, "000000");
   genutil_format_date (temp);
   memcpy (auth_tx.TLF01_details.date_yyyymmdd, temp, 8);  
   memcpy (auth_tx.TLF01_details.time_hhmmss, temp+8, 6);

   if (0 == strcmp(request_type, LOGON_REQUEST))
   {
      ncamex_log_message( 1, 1, "Sending LOGON Request to host." , "" , 0 ,"");
      strcpy(vLogon_NMI_Code, LOGON_NMI_LOGON);
   }
   else if (0 == strcmp(request_type, LOGOFF_REQUEST))
   {
      ncamex_log_message( 1, 1, "Sending LOGOFF Request to host." , "perform_op_net_mngmnt_request" , 0 ,"");
      strcpy(vLogon_NMI_Code, LOGON_NMI_LOGOFF);
   }
   else 
   {
      strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);

      //HCTS1-A
      /* Build Start Timer message to send to Host Data Server */
      timer_length = atoi(ncf01.request_timer);
      p_msg_set_timer = ptetimer_bld_start_timer_req( echo_request_timer_key,
                            p_auth_tx->TLF01_details.primary_key.transaction_id,
                            p_auth_tx->TLF01_details.terminal_id,
                            timerds_que_name,
                            application_que_name,
                            timer_length,
                            0, "0", 1 );

      if ( NULL_PTR == p_msg_set_timer )
      {
    	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
    	  sprintf(Log_Bufr," Insufficient Memory to build start timer message Term Id:%s ,Merch Id: %s RRN: %s ,ECHO_TMR_KEY :%s",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num,echo_request_timer_key);
    	  ncamex_log_message( 3, 2, Log_Bufr , "perform_op_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);

    	  return(false);
      }

      /* We can chain the request_timer message with set & clear
       * echo timer request to timerds
       */
      if(false == chain_echo_host( echo_lookup_key, echo_timer_length,
                                   application_que_name, p_msg_set_timer) )
      {
    	  ncamex_log_message( 3, 3, "Error chaining the timer request with clear & set the echo_timer" , "perform_op_net_mngmnt_request" , 0 ,"");
      }
   }

   if (false == hhutil_send_net_mngmnt_request(&auth_tx, amex_response_code))
      return(false);
   else
      return(true);

} /* perform_op_net_mngmnt_request */

/*****************************************************************************

  Function:    perform_op_net_mngmnt_response

  Description: This function will 
  Author: 
      Jimmy Hapitas
  Inputs:
      nmi_code  - the nmi code
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
int perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx)
{
   CHAR amex_response_code[4];

   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id);
  
   ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);
   strcpy(p_auth_tx->TLF01_details.sys_trace_audit_num, p_auth_tx->TLF01_details.sys_trace_audit_num); 
   strcpy(p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx->TLF01_details.retrieval_ref_num);

   if (0 == strcmp(nmi_code, LOGON_NMI_LOGON))
   {
      ncamex_log_message( 1, 1, "Sending LOGON Response to host" , "perform_op_net_mngmnt_response" , 0 ,"");
      strcpy(vLogon_Resp_Indicator, LOGON_RESP_INDICATOR);  
   }
   else if (0 == strcmp(nmi_code, LOGON_NMI_LOGOFF))
   {
      ncamex_log_message( 1, 1, "Sending LOGOFF Response to host" , "perform_op_net_mngmnt_response" , 0 ,"");
   }
   else 
   {
      ncamex_log_message( 1, 1, "Sending ECHO Response to host" , "perform_op_net_mngmnt_response" , 0 ,"");
   }

   strcpy(vLogon_NMI_Code, nmi_code);

   strncpy( amex_response_code, "801", 3);  // 801 = sign on
   if (false == hhutil_send_net_mngmnt_response(p_auth_tx, amex_response_code))
      return(false);
   else
      return(true);

} /* perform_op_net_mngmnt_response */
   
/*****************************************************************************

  Function:    

  Description: This function will match the transaction type of a request message 
               to the matching response message.
  Author: 
      unknown
  Inputs:
      request - the transaction type of the incoming request message
  Outputs:
      None
  Return values:
      BYTE - the response transaction type
  Modified by:
      BGable      6/30/98
******************************************************************************/
BYTE match_request_to_response(BYTE request)
{

   if ( request >= AUTH_INVALID )
      return( request );

   switch (request)
   {
      case AUTH_AUTHORIZATION:
	     return(AUTH_AUTHORIZATION_RESPONSE); 
        break;

      case AUTH_ADVICE:
	     return(AUTH_ADVICE_RESPONSE); 
        break;

      case AUTH_CASH_ADVANCE:
	     return(AUTH_CASH_ADVANCE_RESPONSE); 
        break;

      case AUTH_REVERSAL_ADVICE:
	     return(AUTH_REVERSAL_ADVICE_RESPONSE); 
        break;

      case AUTH_REVERSAL:
	     return(AUTH_REVERSAL_RESPONSE); 
        break;

      case AUTH_LOGON:
	     return(AUTH_LOGON_RESPONSE); 
        break;

	  default:
         return( request+100 ); /* Convert from request to a response. */
        break;

   }
}; /* match_request_to_response */

VOID match_ISO_to_AMEX_response_code(pAUTH_TX p_auth_tx, pBYTE amex_response_code)
{
   #define NB 42

   INT  i;
   CHAR iso_response_codes [NB][2] =  {"00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                       "12",  "13",  "14",  "15",  "19",  "21",  "25",  "28",
                                       "39",  "41",  "43",  "51",  "52",  "53",  "54",  "55",
                                       "57",  "58",  "61",  "62",  "63",  "65",  "75",  "76",
                                       "77",  "80",  "81",  "82",  "85",  "86",  "91",  "92",
                                       "93",  "96"};
   CHAR auth_auth_amex [NB][3]     =  {"000", "100", "100", "100", "200", "125", "100", "100",
                                       "111", "110", "100", "100", "100", "100", "100", "100",
                                       "100", "100", "100", "116", "100", "100", "101", "100",
                                       "100", "100", "100", "100", "100", "100", "100", "100",
                                       "100", "100", "100", "100", "000", "100", "100", "100",
                                       "100", "100"};
   CHAR auth_advice_amex [NB][3]   =  {"900", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "900", "904", "904", "904",
                                       "904", "904"};
   CHAR auth_logon_amex [NB][3]    =  {"800", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "800", "899", "912", "899",
                                       "899", "899"};
   CHAR auth_cash_amex [NB][3]     =  {"000", "100", "100", "100", "100", "107", "100", "100",
                                       "100", "100", "100", "100", "100", "100", "100", "100",
                                       "100", "290", "290", "121", "100", "100", "100", "117",
                                       "100", "100", "121", "100", "100", "100", "106", "100",
                                       "100", "100", "100", "100", "000", "100", "100", "100",
                                       "100", "100"};
  CHAR auth_reversal_amex [NB][3]  =  {"400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400"};

  if(3 == strlen(p_auth_tx->TLF01_details.product_codes[12].code))
  {
	  return;
  }

  if (p_auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE ||
	   p_auth_tx->TLF01_details.tx_key == AUTH_REFUND_RESPONSE   )
   {
      strcpy( amex_response_code, "100" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_auth_amex[i], 3);
            break;
         }
      }
   }
   else if (p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE_RESPONSE)
   {
      strcpy( amex_response_code, "904" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_advice_amex[i], 3);
            break;
         }
      }
   }
   else if (p_auth_tx->TLF01_details.tx_key == AUTH_LOGON_RESPONSE)
   {
      strcpy( amex_response_code, "899" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_logon_amex[i], 3);
            break;
         }
      }
   }
   else if (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
   {
      strcpy( amex_response_code, "400" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_reversal_amex[i], 3);
            break;
         }
      }
   }
   else if (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE)
   {
      strcpy( amex_response_code, "400" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_reversal_amex[i], 3);
            break;
         }
      }
   }
   else if (p_auth_tx->TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE)
   {
      strcpy( amex_response_code, "100" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_cash_amex[i], 3);
            break;
         }
      }
   }
}

VOID match_AMEX_to_ISO_response_code(pAUTH_TX p_auth_tx, pBYTE amex_response_code)
{
   #define  NUMBER_OF_ACTION_CODES   28

   INT  i;

   CHAR    iso_response_codes[NUMBER_OF_ACTION_CODES][3] =
                             {"00",  "00",  "01",  "54",  "55","03",
									   "13",  "12" , "55",  "51",  "19", "05",  "43",  
									   "12",  "00",  "00",  "00",  "30", "91",  "91",
                              "40",  "00",  "40",  "25",  "80", "27",  "63", "04"};

   CHAR    auth_advice_amex[NUMBER_OF_ACTION_CODES][4] =
                             {"000", "001", "100", "101", "106","189",
									   "110", "111", "117", "116", "122", "125", "200",
									   "290", "400", "800", "900", "904", "911", "912",
                              "115", "300", "301", "302", "303", "304", "380","200"};



   if ( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1120",4)) ||
        (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4)) ||
        (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1314",4)) ||
		(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1420", 4))||
		(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1430", 4)) ||
		(0 == strncmp(p_auth_tx->TLF01_details.message_type, "1110", 4)))
   {
      for (i=0; i<NUMBER_OF_ACTION_CODES; i++)
      {
         if (0 == strncmp(amex_response_code, auth_advice_amex[i], 3))
         {
            strncpy(p_auth_tx->TLF01_details.response_code, iso_response_codes[i], 2);
            break;
         }
      }
      /* Store the response code sent by AMEX for advice transactions */
      memcpy(p_auth_tx->TLF01_details.product_codes[13].quantity, p_auth_tx->TLF01_details.response_code, 2);
   }

}

/*****************************************************************************

  Function:    perform_standin_processing

  Description: This function will perform the standin processing for those
               transactions that do not require host authorization (offline trans)
  Author: 
      unknown
  Inputs:
      rsp_code   - the response code for the message
      gen_status - the general status for the message
      p_auth_tx  - the auth_tx structure for the message
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void perform_standin_processing(CHAR rsp_code[3], BYTE gen_status,
                                pAUTH_TX p_auth_tx )
{     
   pPTE_MSG	p_msg_insert_auth       = NULL_PTR;
   pPTE_MSG p_msg_delete_auth       = NULL_PTR;
   pPTE_MSG p_msg_term_handler_out	= NULL_PTR;
   LONG       ret_code;
   CHAR	      Buffer[512] = {0};
   CHAR	      ErrorMsg[512] = {0};
   CHAR       temp_orig_que[15] = "";
   BYTE      buffer1[sizeof(AUTH_TX)];


   if ( AUTH_REVERSAL_RESPONSE != p_auth_tx->TLF01_details.tx_key )
   {
      strcpy(p_auth_tx->TLF01_details.auth_number, p_auth_tx->TLF01_details.time_hhmmss);
   }
   /* create response code and store it in the auth_tx block  */
   strcpy(p_auth_tx->TLF01_details.response_code, rsp_code);
   p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);

   /* change status to completed and store it in the auth_tx block */ 
   p_auth_tx->TLF01_details.general_status = gen_status;

   /* build the message to be sent to the terminal handler */
   strcpy(temp_orig_que, authorizer_que_name);
   
   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1,p_auth_tx, sizeof(AUTH_TX));
   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              temp_orig_que,
								                      application_que_name,
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1),
                                              0 );

   ret_code = pteipc_send (p_msg_term_handler_out, authorizer_que_name); 
   if ( ret_code < 0 )
   {
 		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s", authorizer_que_name, ErrorMsg );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncamex_log_message( 3, 2, Buffer , "perform_standin_processing" , 0 ,p_auth_tx->TLF01_details.card_num);
      free(p_msg_term_handler_out);
      return;
   }
   else
   {
      ncamex_log_message( 2, 1, "Response sent successfully to terminal handler" , "perform_standin_processing" , 0 ,"");
   }

   free(p_msg_term_handler_out);
	return; 
} /* perform_standin_processing */

/************************************************************************** ****
*
*  NAME:         GET_DATE
*
*  DESCRIPTION:  This procedure takes an input string that contains a date and
*                time.  It copies the date portion into an output string, 
*                formatted slightly different.
*
*  AUTHOR:       Dennis Irby
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ" *
*  OUTPUTS:      date_str  : Format = "YYYY0M0D" *
*  RTRN VALUE:   None
*
*  MODIFICATIONS:

*************************************************************************** ***/
void get_date( pCHAR time_date, pCHAR date_str ) 
{
   memset ( date_str,  0x00,         9 ); 
   strncpy( date_str,  time_date,    4 ); 
   strncat( date_str, &time_date[5], 2 ); 
   strncat( date_str, &time_date[8], 2 );
}
/************************************************************************** ****
*
*  NAME:         GET_TIME
*
*  DESCRIPTION:  This procedure takes an input string that contains a date and
*                time.  It copies the time portion into an output string, 
*                formatted slightly different.
*
*  AUTHOR:       Dennis Irby
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ" *
*  OUTPUTS:      time_str  : Format = "0H0I0S" *
*  RTRN VALUE:   None
*
*  MODIFICATIONS:

*************************************************************************** ***/
void get_time( pCHAR time_date, pCHAR time_str ) 
{
   memset ( time_str,  0x00,          7 ); 
   strncpy( time_str, &time_date[11], 2 ); 
   strncat( time_str, &time_date[14], 2 ); 
   strncat( time_str, &time_date[17], 2 );
}


/*****************************************************************************

  Function:    perform_error_processing

  Description: This function will take care of transactions that were unable
               to get sent to the host due to some type of error.
  Author: 
      unknown
  Inputs:
      p_auth_tx_in - the auth_tx structure to be processed.
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void perform_error_processing(pAUTH_TX p_auth_tx_in)   
{
   char       date_str [ 9] = "";
   char       time_str [ 7] = "";
   char       time_date[25] = "";
   char       resp_code[3] = "58";


   /* Get local time. */
   ptetime_get_timestamp( time_date );
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
   if ( 0 == strcmp( NULL_STR, p_auth_tx_in->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx_in->TLF01_details.retrieval_ref_num );
   }

   /* create response code and store it in the auth_tx block */
   strcpy(p_auth_tx_in->TLF01_details.response_code, resp_code );


   if ( 0 == strlen(p_auth_tx_in->TLF01_details.date_yyyymmdd))
   {
      /* Create date of transaction and store it in the auth_tx table,
      * format "YYYY0M0D".
      */
      strcpy( p_auth_tx_in->TLF01_details.date_yyyymmdd, date_str );
   }

   if ( 0 == strlen(p_auth_tx_in->TLF01_details.time_hhmmss))
   {
      /* Create time of transaction and store it in the auth_tx 
      table,"0H240I0S" */
      strcpy( p_auth_tx_in->TLF01_details.time_hhmmss, time_str );
   }
   p_auth_tx_in->TLF01_details.general_status = GS_TXN_DROPPED;
     
}; /* perform_error_processing */


/*****************************************************************************

  Function:    get_amex_data

  Description: This function will put the amex specific data into the auth_tx
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure
  Outputs:
      p_auth_tx - the auth_tx structure
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT get_amex_data (pAUTH_TX p_auth_tx)
{
   CHAR temp_str1[15];
   CHAR temp_str2[12];
   CHAR time_date[27];

   strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id);

	/* Get LOCAL Date and Time */
   ptetime_get_timestamp(time_date);

	memset(&temp_str1, 0x00, sizeof(temp_str1));
   memset(&temp_str2, 0x00, sizeof(temp_str2));
   strncpy(temp_str1, time_date, 4);
   strncpy(temp_str2, (time_date + 5), 2);
   strcat (temp_str1, temp_str2);
	strncpy(temp_str2, (time_date + 8), 2);
	strcat (temp_str1, temp_str2);
	strcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp_str1);
   
	memset(&temp_str1, 0x00, sizeof(temp_str1));
   memset(&temp_str2, 0x00, sizeof(temp_str2));
   strncpy(temp_str1, (time_date + 11), 2);
   strncpy(temp_str2, (time_date + 14), 2);
   strcat (temp_str1, temp_str2);
	strncpy(temp_str2, (time_date + 17), 2);
	strcat (temp_str1, temp_str2);
	strcpy(p_auth_tx->TLF01_details.time_hhmmss, temp_str1);   

   
   /* Get Retrieval Reference Number => YDDDnnnnnnnn */
   if ( AUTH_REVERSAL != p_auth_tx->TLF01_details.tx_key )
   {
      memset (p_auth_tx->TLF01_details.retrieval_ref_num, 0, sizeof(p_auth_tx->TLF01_details.retrieval_ref_num));
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }
   return (true);
}

/*****************************************************************************

  Function:    find_cvc_flag

  Description: This function will parse field 48 of a 0110 response message 
  looking for Tag 87.  This tag contains a 1 byte flag: Magnetic Stripe / 
  CVC Invalid.
  
    FIELD 48 FORMAT:
 
      -------+------+---------+---------+---------+---------+-----
       Total |Trans | First   | First   | First   | Second  |
        Len  | Cat. |SubField |SubField |SubField |SubField | . . .
             | Code |  Tag    | Length  |  Data   |  Tag    |
      -------+------+---------+---------+---------+---------+-----
  BYTES: 3       1       2         2         x         2
 
  Author: 
      unknown
  Inputs:
      src_len - the length of src
      src - field 48
  Outputs:
      dest - the cvc flag
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void find_cvc_flag( pCHAR src_len, pCHAR src,  pCHAR dest )
{
   #define  FIELD48_MAX_LEN  100   
   #define  FIELD48_TAG      "87"
   INT  sub_field_len;
   CHAR tag[3];
   CHAR len[3];
   INT  MainIndex;
   INT  TotalLen;

   TotalLen = atoi( src_len );

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      memset( tag, 0x00, sizeof( tag ) );
      memcpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;
      memset( len, 0x00, sizeof(len));
      memcpy( len, &src[MainIndex], 2 );
      sub_field_len = atoi( len );
      MainIndex += 2;

      if ( 0 == strcmp( tag, FIELD48_TAG ) )
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


/*****************************************************************************

  Function:    calc_julian_day

  Description: This function will calculate the julian day
  Author: 
      unknown
  Inputs:
      date_str -  input string format  = YYYYMMDD...
  Outputs:
      julian_str - output string format = DDD
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void calc_julian_day(char date_str[], char julian_str[])
{
   char  year[5];
   char  month[3];
   char  day[3];
   int   iYear;
   int   iMonth;
   int   iDay;
   int   julian_day;
   int   i;
   int   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


   memset (&year, 0, sizeof(year));
   memset (&month, 0, sizeof(month));
   memset (&day,   0, sizeof(day));
   memcpy (&year,  &date_str[0], 4);
   memcpy (&month, &date_str[4], 2);
   memcpy (&day,   &date_str[6], 2);

   iYear  = atoi( year );
   iMonth = atoi( month );
   iDay   = atoi( day );

   for( i=0,julian_day=0; i<iMonth-1 && i<12; i++ )
   {
		julian_day += DaysOfMonth[i];
   }
   julian_day += iDay;

   if ( iMonth > 2 )
      if ( 0 == ( iYear % 4 ) )
          julian_day++;
   
   itoa(julian_day, julian_str, 10);

   Rj_with_lead_zeros (julian_str, 3);
}

/*****************************************************************************

  Function:    process_incoming_message

  Description: This function will process an incoming message from the authorizer,
               it may be a request or a response.
  Author: 
      unknown
  Inputs:
      pmsg - a pointer to the incoming message that contains the auth_tx
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT process_incoming_message(pPTE_MSG p_msg)
{
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   LONG            data_len;
   LONG            auth_len;
   pBYTE           p_data_data = NULL_PTR;
   AUTH_TX         auth_tx;
   char amex_response_code[4]={0};

   memset(&auth_tx, 0x00, sizeof(AUTH_TX));

	p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   auth_len = data_len;
   if(sizeof(auth_tx) < data_len)
      auth_len = sizeof(auth_tx);
   p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
   memcpy(&auth_tx, p_data_data, auth_len);
   p_data_data = p_data_data + sizeof(auth_tx);

   /* No need to continue for authorization advices.
    * They have already been answered.
    */
   /*if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE )
   {
    //Send a msg to updateds to restore the balances.
    update_database(&auth_tx );
    return( true );
   }*/
   /* TF PHANI..If it is ATM_CONFIRM it is reversal response */
   if(auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE)
   {
	  auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   }

   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id, sizeof(ncf01.tpdu_id));

   if(!(auth_tx.TLF01_details.tx_key == AUTH_LOGON_RESPONSE))
   {
      if((auth_tx.TLF01_details.tx_key < AUTH_SALE) ||
         (auth_tx.TLF01_details.tx_key > AUTH_INVALID))  /* a response */
      {
         auth_tx.TLF01_details.tx_key =
                        match_request_to_response(auth_tx.TLF01_details.tx_key);

         /* Restore the conversion rate. */
         if ( auth_tx.settlement_conv_rate[0] != 0x00 )
         {
            strcpy( auth_tx.TLF01_details.conversion_rate,
                    auth_tx.settlement_conv_rate );
         }
		/* Call function for TLF logginglogging*/
		 if(0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"nposa2",6) ||
			0 == strncmp(auth_tx.TLF01_details.authorizing_host_queue,"natma2",6) 
	       )
		 {
			 Tlf_Logging_ncamex_cadience(&auth_tx);
	
		 }
		
		 if(0 == strcmp(auth_tx.TLF01_details.product_codes[1].code,STANDIN)&&
				 (0 == strcmp(auth_tx.TLF01_details.response_code, "00")))
		 {

			 match_ISO_to_AMEX_response_code(&auth_tx, amex_response_code);
			 if (0 == strlen(auth_tx.TLF01_details.product_codes[12].code))
			 {
				 memcpy( auth_tx.TLF01_details.product_codes[12].code,
						 amex_response_code,
						 sizeof(amex_response_code));
			 }
		 }
		 if((0 == strcmp(auth_tx.TLF01_details.response_code, "00")) || (0 == strcmp(auth_tx.TLF01_details.response_code, "55")))
		 {
			 if ( false == update_database( &auth_tx ))
			 {
				memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				sprintf(Log_Bufr," Unable to update database tables. Term Id:%s ,Merch Id: %s RRN: %s ",
						auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				ncamex_log_message( 3, 2, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
				/* Unable to complete a transaction.  Inform host; log to TLF01. */
				if(false == incoming_other_host_response_db_reply( &auth_tx ) )
				{
					memset(Log_Bufr,0x00,sizeof(Log_Bufr));
					sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
							auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
					ncamex_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
				}
			 }
			 
		 }
		 else
		 {
			 /* 
				 since the transaction control responded with response code other than 00, 
				 Database is not updated. Sending back the response to Host. 
				 Insert into TLF01.
			 */
			 if(false == incoming_other_host_response_db_reply(&auth_tx))
			 {
				 	memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				 	sprintf(Log_Bufr," Unable to send response to host.  Term Id:%s ,Merch Id: %s RRN: %s ",
				 						auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				 	ncamex_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
			 }
		 }
		 return(true);
      }
   }

   strcpy(auth_tx.TLF01_details.dest_key, ncf01.cwk);
   strcpy(auth_tx.TLF01_details.dest_pin_format, ncf01.pin_type);
   if (true == hhutil_verify_pin_block(&auth_tx))
   {
      /* PIN Translation is required */
      if(false == hhutil_process_debit_transaction(&auth_tx))
      {
		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Failed to process debit transaction  Term Id:%s ,Merch Id: %s RRN: %s ",
				auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncamex_log_message( 3, 2, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
         perform_error_processing( &auth_tx );
         strcpy(auth_tx.TLF01_details.response_code,"91");
         auth_tx.TLF01_details.tx_key =
                       match_request_to_response(auth_tx.TLF01_details.tx_key);
         incoming_other_host_response(&auth_tx);
         return(false);
      }
      return(true);
     }
   else
   {
      /* Do not perform PIN translation. */
      if ( process_incoming_message_continued( &auth_tx ) )
         return( true );
      else
      {
         perform_error_processing( &auth_tx );
         strcpy(auth_tx.TLF01_details.response_code,"91");
         auth_tx.TLF01_details.tx_key =
                        match_request_to_response(auth_tx.TLF01_details.tx_key);
         incoming_other_host_response(&auth_tx);
         return( false );
      }
   }
   return( true );
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
   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   {
      if(incoming_terminal_request(p_auth_tx))
         return(true);
      return(false);
   }
   return(false);
   
}


/*****************************************************************************

  Function:    process_incoming_message_ncf30_reply

  Description: This function will process an incoming message from the data
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


   memset(&ncf30,   0, sizeof(NCF30));
   memset(&auth_tx, 0, sizeof(AUTH_TX));
   memcpy(&ncf30,   pCurrent+(sizeof(auth_tx)), sizeof(NCF30));
   memcpy(&auth_tx, pCurrent, sizeof(AUTH_TX));

   /* Original txn was found in NCF30 for an advice or reversal. */
   if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE )
   {
      /* Treat advices differently. */
      (void)process_auth_advices( &auth_tx, FOUND, ncf30 );
   }
   else
   {
      /* Tell txcntl to go process the reversal. */
      update_auth_tx_with_ncf30_data(ncf30, &auth_tx);
      (void)hhutil_transfer_ctl_to_auth_app(&auth_tx);
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
*                1.  Always resond to the host with an 1130
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
*                ncf30     - Original record found in NCF30
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred 
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:

********************************************************************** ********/
INT process_auth_advices( pAUTH_TX p_auth_tx, INT status, NCF30 ncf30 )
{
   INT   ret_val;
   char  resp_code[4]; //temp data
   BYTE  txkey;

   if ( ( (FOUND == status) &&
          (0 != strcmp(p_auth_tx->TLF01_details.response_code, "00" )) )
          ||
          ( (NOT_FOUND == status) &&
            (0 == strcmp(p_auth_tx->TLF01_details.response_code, "00" )) )
        )
   {
         /* Send advice to transaction control. */
        if ( FOUND == status )
        {
           update_auth_tx_with_ncf30_data( ncf30, p_auth_tx );
           //restoring back the response code
           strncpy(p_auth_tx->TLF01_details.response_code,resp_code,2);
         }
         p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE;
         ret_val = hhutil_transfer_ctl_to_auth_app( p_auth_tx );
   }
   else
	{
			/*TF - ABHISHEK Changes for 120 messges*/
			   
			/*Advice messages should be sent to Cadencie as long
			as Cadencie is online even there is no related sale transaction
			before 120 transaction?. 

			If Cadencie is online, let Cadencie respond to the
			advice messages.  It is only when Cadencie is offline and ATP is processing
			the Amex transactions on behalf of Cadencie than ATP should reply to the
			Advice messages..	 */
			p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE;
         	ret_val = hhutil_transfer_ctl_to_auth_app( p_auth_tx );
	  }
   //}
   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to complete processing of an advice (1120). Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 3, Log_Bufr , "process_auth_advices" , 0 ,p_auth_tx->TLF01_details.card_num);
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
   CHAR            Buff[512] = {0};
   CHAR            err_mesg[512] = {0};
 
   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, sizeof(AUTH_TX) );
   memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );

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
      ncamex_log_message( 3, 2, "Insufficient Memory to build NCF30 message" , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s", netds_que_name, err_mesg );
      ncamex_log_message( 3, 3, Buff , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }
   return(true);
 }

/******************************************************************************
*
*  NAME:         RECON_UPDATE
*
*  DESCRIPTION:  This function sends auth_tx to the data server.
*                Its purpose is to update totals and keep a running count.
*                It also stores the transaction into NCF30. 
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
INT recon_update( pAUTH_TX p_auth_tx ) 
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   LONG            ret_code;
   CHAR            Buff[512]={0};
   CHAR            err_mesg[512] = {0};
   NCF30           ncf30;
   CHAR            time_date[25];
   CHAR            gmt_time[20];

   /* Build the NCF30 structure */
   memset( &ncf30, 0x00, sizeof(NCF30) );
   strcpy( ncf30.primary_key.network_id,    p_auth_tx->TLF01_details.issuer_id ); 
   strcpy( ncf30.primary_key.network_type, "A"      ); 
   strcpy( ncf30.primary_key.batch_number, "ACTIVE" ); 
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }
   strcpy( ncf30.primary_key.retrieval_ref_num,p_auth_tx->TLF01_details.retrieval_ref_num );
   strcpy( ncf30.merchant_id, p_auth_tx->TLF01_details.merchant_id ); 
   strcpy( ncf30.transaction_id, p_auth_tx->TLF01_details.primary_key.transaction_id );
   strcpy( ncf30.device_id, p_auth_tx->TLF01_details.terminal_id ); 
   strcpy( ncf30.card_nbr,  p_auth_tx->TLF01_details.card_num  ); 
   strcpy( ncf30.message_type, p_auth_tx->TLF01_details.message_type );
   strcpy( ncf30.processing_code, p_auth_tx->TLF01_details.processing_code );
   strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount ); 
   strcpy( ncf30.auth_number, p_auth_tx->TLF01_details.auth_number ); 
   strcpy( ncf30.currency_code, p_auth_tx->TLF01_details.currency_code ); 
   strcpy( ncf30.response_code, p_auth_tx->TLF01_details.response_code);
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ))
   {
      /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
      ptetime_get_timestamp( time_date );

      /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
      get_date( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
      get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss   );
   }
   strcpy( ncf30.tran_date, p_auth_tx->TLF01_details.date_yyyymmdd ); 
   strcpy( ncf30.tran_time, p_auth_tx->TLF01_details.time_hhmmss   ); 
   strcpy( ncf30.sys_trace_audit_nbr, p_auth_tx->TLF01_details.sys_trace_audit_num );
   ncf30.tx_key = p_auth_tx->TLF01_details.tx_key;
   strcpy( ncf30.invoice_nbr, p_auth_tx->TLF01_details.invoice_number );
   strcpy( ncf30.network_data, p_auth_tx->TLF01_details.mcard_banknet );
   if ( 0 == strcmp( NULL_STR,p_auth_tx->TLF01_details.transmission_timestamp ) )
   {
       //create timestamp
      pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
      strncpy( ncf30.transmission_timestamp,&gmt_time[4], 10 );  /* MMDDhhmmss */
   }
   else
   strcpy( ncf30.transmission_timestamp,p_auth_tx->TLF01_details.transmission_timestamp);
   /* Build message to get host specific data from data server. */ 
   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, sizeof(AUTH_TX) );

   memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );
   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                            ST1_DB_UPDATE, 
                            ST2_NCF30_INSERT,
                            netds_que_name, 
                            application_que_name,
                            (pBYTE)&buffer1,
                            sizeof(buffer1), 
                            ALL_DATA );
   if ( NULL_PTR == p_msg  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build NCF30 message Term Id:%s ,Merch Id: %s RRN: %s ",
     		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncamex_log_message( 3, 2, Log_Bufr , "recon_update" , 0 ,p_auth_tx->TLF01_details.card_num);
      return( false );
   }

  /* Send message data server. */
  ret_code = pteipc_send( p_msg, netds_que_name ); 
  free( p_msg );
  if ( 0 > ret_code )
  {
     pteipc_get_errormsg( ret_code, err_mesg ); 
     sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s",
             netds_que_name, err_mesg );
     memset(Log_Bufr,0x00,sizeof(Log_Bufr));
     sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
     		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
     strcat(Buff,Log_Bufr);
     ncamex_log_message( 3, 2, Buff , "recon_update" , 0 ,p_auth_tx->TLF01_details.card_num);
     return( false );
  }


   return( true );
}

/******************************************************************************
 *
 *  NAME:         UPDATE_AUTH_TX_WITH_NCF30_DATA
 *
 *  DESCRIPTION:  This function takes data from ncf30 and puts it into auth_tx
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      auth_tx - with data from NCF30
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Beth Gable
 *
 *  MODIFICATIONS:
 ******************************************************************************/
VOID update_auth_tx_with_ncf30_data(NCF30 ncf30, pAUTH_TX p_auth_tx)
{
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.card_num ) )
      strcpy( p_auth_tx->TLF01_details.card_num , ncf30.card_nbr );

   if ( 0 != strcmp( ncf30.processing_code, p_auth_tx->TLF01_details.processing_code ) )
      strcpy( p_auth_tx->TLF01_details.processing_code , ncf30.processing_code);

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_amount ) )
      strcpy( p_auth_tx->TLF01_details.orig_amount , ncf30.tran_amount );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.time_hhmmss ) )
      strcpy( p_auth_tx->TLF01_details.time_hhmmss , ncf30.tran_time );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ) )
      strcpy( p_auth_tx->TLF01_details.date_yyyymmdd , ncf30.tran_date );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.auth_number ) )
      strcpy( p_auth_tx->TLF01_details.auth_number , ncf30.auth_number );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.terminal_id ))
   {
	  if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type,"0420"))
	  {
		  /* Dont copy the Device ID from orginal..Send to Cadiece as it is..Req By KenneTh, .TF PHANI  */
	  }
	  else
	  {	
      strcpy( p_auth_tx->TLF01_details.terminal_id , ncf30.device_id );
	  }
   }

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.currency_code ) )
      strcpy( p_auth_tx->TLF01_details.currency_code , ncf30.currency_code );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_message ) )
      strcpy( p_auth_tx->TLF01_details.orig_message , ncf30.message_type );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.settlement_date ) )
      strcpy( p_auth_tx->TLF01_details.settlement_date , ncf30.settlement_date);

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_sys_trace_num ) )
      strcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
              ncf30.sys_trace_audit_nbr ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_retrieval_ref_num))
      strcpy( p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              ncf30.primary_key.retrieval_ref_num ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.transmission_timestamp))
      strcpy( p_auth_tx->TLF01_details.transmission_timestamp,
              ncf30.transmission_timestamp ); 
   /* ATP needs for STIP txn TF-Ajay 13Jul2009 */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.product_codes[1].code))
       strcpy( p_auth_tx->TLF01_details.product_codes[1].code,ncf30.invoice_nbr );

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
   sprintf(rrn, "%1.1s%3.3s%5.5s%3.3s",time_date+3, julian_day, time_str+1, time_date+20 );
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
   BYTE       buffer[sizeof(AUTH_TX)+1];
   LONG       ret_code;
   INT        ret_val = true;
   CHAR       Buff[512] = {0};
   CHAR       err_mesg[512] = {0};

   memset(buffer, 0, sizeof(buffer));
   //memcpy(buffer+sizeof(AUTH_TX), p_auth_tx, sizeof(AUTH_TX)); 
   	/*TF PHANI... Tempary fix for Amex 5CSC, since interst_rate filed has only 4 bytes, we need to fill null at end*/
   memset(&p_auth_tx->TLF01_details.interest_rate[4],'\0',1);
   memcpy(buffer, p_auth_tx, sizeof(AUTH_TX)); // svelaga : 12/02

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
      ncamex_log_message( 3, 2, Log_Bufr , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
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
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncamex_log_message( 3, 3, Buff , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = false;
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
   CHAR  err_msg[512]        ={0};

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   (void)get_originator_info( p_msg, originator_queue, originator_info );

   /* Send statistics to Monitor */
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
      ncamex_log_message( 3, 2, err_msg , "send_transaction_statistics" , 0 ,"");
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
   CHAR  err_msg[100]        ="";

   /* Reset the statistics */
   (void)reset_stats( &Txn_Counts );

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   (void)get_originator_info( p_msg, originator_queue, originator_info );

   /* Send statistics to Monitor */
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
      ncamex_log_message( 3, 2, err_msg , "reset_transaction_statistics" , 0 ,"");
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         GET_ORIGINATOR_INFO
 *
 *  DESCRIPTION:  This function extracts originator queue and information from
 *                a PTE message.  Stores them into input parameters.  The
 *                outputs are not overwritten if they have data in them.
 *
 *  INPUTS:       p_msg_in - PTE Message
 *
 *  OUTPUTS:      orig_queue - Originator queue from PTE message
 *                orig_info  - Originator information from PTE message
 *
 *  RTRN VALUE:   True if p_msg_in is not null, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info )
{
   INT  ret_val = true;

   if ( p_msg_in != NULL )
   {
      /* Gather originator information */
      if ( orig_queue[0] == 0x00 )
         strcpy( orig_queue, ptemsg_get_msg_orig_queue(p_msg_in) );

      if ( orig_info[0] == 0x00 )
         strcpy( orig_info,  ptemsg_get_msg_orig_comms_info(p_msg_in) );
   }
   else
   {
      ret_val = false;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SET_ORIGINATOR_INFO
 *
 *  DESCRIPTION:  This function set originator communications information
 *                into a PTE message.  For responses, tx_key > 100, the info
 *                stored in auth_tx is used.  For requests, the tpdu_id is
 *                used.
 *
 *  INPUTS:       p_auth_tx  - Transaction information
 *
 *  OUTPUTS:      p_msg_out  - PTE message going to dialog manager
 *
 *  RTRN VALUE:   True if p_msg_out is not null, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out )
{
   INT  ret_val = true;

   if ( p_msg_out != NULL )
   {
      if ( p_auth_tx->TLF01_details.tx_key > 100 )
      {
         /* This is a response. */
         (void)ptemsg_set_orig_comms_info( p_msg_out,
                                p_auth_tx->TLF01_details.originator_info );
      }
      else
      {
         /* This is a request. */
         (void)ptemsg_set_orig_comms_info( p_msg_out,
                                           p_auth_tx->TLF01_details.tpdu_id);
      }
   }
   else
   {
      ret_val = false;
   }
   return( ret_val );
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
 *                Currency Code (DE49) is 608 OR  Country Code (DE19) is 608
 *                   AND
 *                Merchant Category Code (MCC, DE18) is one of these:
 *                   6010, 6011, 6012, 6051
 *
 *                NOTE:  This function assumes a txn switched in from Amex
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if transaction should be blocked (not allowed) else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   CHAR  BLOCKED_MCC[][5] = { "6012", "6051", "done" };

   INT   ret_val = false;
   INT   i = 0;
   CHAR  CC[9] = "";  /* Country Code */

   /* Check if currency code is Phillippino Peso.
    * Reconciliation_cur_code contains the original value from DE49.
    */
   if ( 0 == strcmp(p_auth_tx->reconciliation_cur_code,PESO) )
      strcpy( CC, "DE49=608" );

   else if ( 0 == strcmp(p_auth_tx->country_code, PESO) )
      strcpy( CC, "DE19=608" );

   if ( CC[0] != 0x00 )
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
                    "Blocked txn from Amex: MCC=%s, %s, Stan=%s, RRN=%s",
                     p_auth_tx->TLF01_details.category_code, CC,
                     p_auth_tx->TLF01_details.sys_trace_audit_num,
                     p_auth_tx->TLF01_details.retrieval_ref_num );

            strcpy( p_auth_tx->TLF01_details.response_text,
                   "Blocked txn from Amex" );

            break;
         }
         i++;
      }
   }
   return( ret_val );
}
BOOLEAN ncpos_check_for_cadencieacq_transaction(pAUTH_TX p_auth_tx)
{
	   if( (0 == strncmp(p_auth_tx->TLF01_details.message_type,"1100",4)	||
			0 == strncmp(p_auth_tx->TLF01_details.message_type,"1110",4)   ||
		    0 == strncmp(p_auth_tx->TLF01_details.message_type,"1420",4)	||
			0 == strncmp(p_auth_tx->TLF01_details.message_type,"1430",4))  	&&
		   (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"nposa2",6) ||
			0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncpos",5)))
	   {
		   return true;
	   }
	   return false;
}

BOOLEAN ncpos_transfer_response_msg_to_nposa2(pAUTH_TX p_auth_tx)
 {

		BOOLEAN   retval              = true;
		pPTE_MSG  p_msg_out           = NULL_PTR;
		CHAR	  nposa2_host_que_name  [MAX_QUE_NAME_SIZE+1] = "nposa2A" ;
		CHAR      buffer[sizeof(AUTH_TX)+1]= {0};
		BYTE      originator_info[33] = {0};
		LONG      retCode             = 0L;
		CHAR      strError[512]       = {0};

	    memcpy(buffer , &p_auth_tx, sizeof(AUTH_TX) );

	    p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
	                                  ST1_NONE,
	                                  ST2_NONE,
									  nposa2_host_que_name,
	                                  application_que_name,
	                                  (pBYTE) p_auth_tx,
									  sizeof(AUTH_TX),
	                                  0 );

	    if ( p_msg_out == NULL_PTR )
	    {
	       sprintf( strError,
	               "Insufficient Memory to build DCI2 request message. Size: %d",
	                buflen );
	       ncamex_log_message( 3, 3, strError , "ncpos_transfer_response_msg_to_nposa2" , 0,"");
	       retval = false;
	    }
	    else
	    {
	       ptemsg_set_orig_comms_info( p_msg_out, originator_info );
	       retCode = pteipc_send( p_msg_out, nposa2_host_que_name );
	       free (p_msg_out);

	       if ( retCode < 0 )
	       {
	          pteipc_get_errormsg (retCode, strError);
	          sprintf( strError,
	                  "Error on pteipc_send to que %s: %s",
					  nposa2_host_que_name, strError );

	          ncamex_log_message( 3, 3, strError , "ncpos_transfer_response_msg_to_nposa2" , 0,"");
	          retval = false;
	       }
	    }

		return retval;
 }

/******************************************************************************
 *
 *  NAME:         CREATE_REQUEST_TIMER_KEY
 *
 *  DESCRIPTION:  This function creates a key for a request going to the host.
 *                It uses the application name, terminal Id, and the system
 *                trace audit number (or cardnumber's last six for reversals).
 *                For terminal Id, reversals use field 41. But voids do not
 *                have a field 41, so they use the last 8 of merchant Id.
 *
 *  INPUTS:       AppName - (Global)Application name
 *                Auth_Tx - (Global)Transaction Info
 *
 *  OUTPUTS:      timer_key - String containing the timer reference key
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_request_timer_key( pCHAR timer_key ,pAUTH_TX p_auth_tx )
{
   BYTE   tx_key;
   BYTE   error_buffer[100] = {0};
   CHAR   tid[9] = {0};
   CHAR   temp_mid[16]    = {0};
   CHAR   key_last_six[7] = {0};
   INT    mid_len = 0;
   INT    pan_len;

   strcpy( timer_key, AppName );
   strcat( timer_key, p_auth_tx->TLF01_details.sys_trace_audit_num );
   /* Trim the spaces on the merchant Id. */
            strcpy( temp_mid, p_auth_tx->TLF01_details.merchant_id );
            trim_trailing_spaces( temp_mid );

            mid_len = strlen( temp_mid );
            if ( mid_len > 8 )
               strncpy( tid, &temp_mid[mid_len - 8],8);
            else
               strncpy( tid, temp_mid,8);
            /* Trim the spaces on the terminal Id. */

              trim_trailing_spaces( tid );

              strcat( timer_key, tid );

   return;
}
/******************************************************************************
 *
 *  NAME:         CLEAR_REQUEST_TIMER
 *
 *  DESCRIPTION:  This function clears a timer for a request to the host.
 *                It chains the current Auth_Tx buffer to the timer request
 *                so that it will be echoed back.
 *
 *  INPUTS:       lookup_key - Key to indicate which timer to clear
 *                AppName    - (Global)Application name to create queue name
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT clear_request_timer( pCHAR lookup_key ,pAUTH_TX p_auth_tx  )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val ;
   CHAR       source_que[30]="";
   CHAR       err_buf[100]  ="";
   BYTE     buffer1[sizeof(AUTH_TX)];

   ptetime_get_strtime( p_auth_tx->TLF01_details.host_finish_time );

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, AppName );
   strcat( source_que, "A"     );

   /* Build PTEMSG to clear timer */
   p_timer_msg = ptetimer_bld_clear_timer_req( lookup_key,
                                               timerds_que_name,
                                               source_que);
   if ( NULL_PTR == p_timer_msg )
   {
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
	   memcpy(buffer1,p_auth_tx, sizeof(AUTH_TX) );
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
                                              MT_DB_REQUEST,
                                              ST1_DB_ECHO,
                                              ST2_NONE,
                                              timerds_que_name,
                                              application_que_name,
											  (pBYTE)p_auth_tx,
											  sizeof(AUTH_TX),
                                              0 );
      if ( NULL_PTR == p_chain_msg )
      {
         sprintf( err_buf, "Insufficient memory to chain clear timer message" );
         free( p_timer_msg );
      }
      else
      {
         /* Send clear timer request with chained data to data server.
          * Processing of this transaction will resume in function
          * process_clear_timer_reply when we receive the
          * response from timerds to our clear timer request.
          */
         ret_val = send_msg( p_chain_msg, timerds_que_name, err_buf );
         free( p_chain_msg );
         if ( true != ret_val )
         {
        	 memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	 sprintf( Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
        	            	     p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
        	  memcpy( p_auth_tx->TLF01_details.response_text, err_buf,
        	                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
        	  strcat(err_buf,Log_Bufr);
        	  ncamex_log_message( 3, 3, err_buf , "clear_request_timer" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }

   return( ret_val );
}

INT set_host_timer( pAUTH_TX p_auth_tx,    pCHAR source_que,
                    WORD     timer_length, pCHAR error_msg )
{
	pPTE_MSG p_msg_set_timer = NULL_PTR;
	CHAR     lookup_key[22];
	BYTE     buffer1[sizeof(AUTH_TX)];
	INT      ret_val = false;

	/* Build lookup key. */
	//memset( lookup_key, 0x00, sizeof(lookup_key) );
	//strcpy( lookup_key, AppName );

	create_request_timer_key(lookup_key,p_auth_tx);

	memcpy(buffer1,p_auth_tx, sizeof(AUTH_TX));

	/* Send the whole transaction record. */
	//memcpy( buffer1, p_auth_tx, sizeof(AUTH_TX) );

	/* Check for valid timer value. */
	/*if ( timer_length <= 0  ||  timer_length > 3600 )
	{
		timer_length = 25;
	}*/

	p_msg_set_timer = ptetimer_bld_start_timer_req(	lookup_key,
													p_auth_tx->TLF01_details.primary_key.transaction_id,
													p_auth_tx->TLF01_details.terminal_id,
													timerds_que_name,
													source_que,
													timer_length,
													0,
													(pBYTE)&buffer1,
													sizeof(buffer1) );

	if ( NULL_PTR == p_msg_set_timer )
	{
		strcpy( error_msg, "Insufficient Memory to build timer message" );
	}
	else
	{
		ret_val = send_msg(p_msg_set_timer, timerds_que_name, error_msg);
		free( p_msg_set_timer );
	}
	return( ret_val );
}
