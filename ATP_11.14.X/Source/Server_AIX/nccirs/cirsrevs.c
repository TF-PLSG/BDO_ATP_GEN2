/******************************************************************************
 
 	CIRSREVS.C
 
    Copyright (c) 2003, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Cirrus Host Handler Reversal Processor
 
    This module is a part of the Cirrus Host Handler.  It handles transaction
	reversal processing for the Cirrus host.
 
    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\PVCS\PTE\Equitable\nccirrus\Cirsrevs.c  $
   
      Rev 1.18   Apr 01 2003 14:43:00   dirby
   Added function 'process_issuer_reversals' to handle
   0422/0432 transactions.
   SCR 1073
   
      Rev 1.17   Jul 03 2002 17:29:08   dirby
   1.  Modified to use pre-calculated sizes instead of always recalculating a structure size.
   2.  Remove use of read_timer.
   3.  Provide details of errors from timerds.
   4.  Added code to display time outs and reversals on Monitor.
   5.  Version 4.0.3.1
   SCR 621
   
      Rev 1.16   Dec 11 2001 14:53:28   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.15   Aug 08 2000 13:03:12   dirby
   Removed the logic of fetching NCF01 during every online txn.
   
      Rev 1.14   Jan 23 2000 16:32:42   dirby
   Changed the priority of a message from ALERT to INFO.
   It is a message to inform an operator of a SAF request.
   
      Rev 1.13   Dec 28 1999 13:04:06   dirby
   Changed function 'process_outgoing_reversals' to use
   an auth_tx passed to it, instead of one created inside it.
   
      Rev 1.12   Nov 10 1999 14:28:12   dirby
   Removed the line of code that inserts a transaction into TLF01.
   
      Rev 1.11   23 Aug 1999 17:08:36   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.10   Jun 30 1999 15:56:56   svelaga
   Fixed bug for 
   Field 15 : Settlement time. 
   
      Rev 1.9   21 Jun 1999 16:36:34   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.8   19 Mar 1999 17:12:40   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.7   18 Mar 1999 15:34:22   dirby
   Created a separate function to send a PTE message to a destination queue.
   This new function, 'send_msg' calls 'pteipc_send' and handles send errors.
   This eliminates lots of repetitive code, especially on the error handling.
   
      Rev 1.6   11 Mar 1999 14:20:48   dirby
   Added functionality to get NCF01 record if needed.
   Added error handling functionality - if a reversal fails, it must
   be set to SAF and send a valid response to originator.  Must not
   send to originator if in SAF mode.
   
      Rev 1.5   10 Mar 1999 17:01:00   dirby
   Added use of 'orig_message' and 'orig_sys_trace_num'.
   These are required for field 90 of reversals.
   
      Rev 1.4   04 Mar 1999 16:47:10   dirby
   Updated for AIX port.
   
      Rev 1.3   03 Mar 1999 15:45:14   dirby
   Added default value for settlement date since it is mandatory.
   
      Rev 1.2   26 Feb 1999 16:45:50   dirby
   Modified call to get original txn for a reversal so
   terminal_id, card_num, and total_amount are the keys.
   Also changed subtype 2  from SELECT_BY_STAN to
   ...SELECT_TLF01_BY_TERMINAL_ID.
   
      Rev 1.1   25 Feb 1999 16:47:56   dirby
   Added code to set authorizing_host_queue in TLF01
   prior to sending a request for a Store and Forward
   transaction from the data server.
   
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
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "memmnger.h"
#include "txdsapi.h"
#include "cirsmain.h"

/* Global Variables */
extern CHAR  AppName[8];

/* Database structures */
extern NCF01             ncf01;
extern NCF21             ncf21;
extern NCF30             ncf30;
extern SAF01             saf01;
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

extern CHAR  reject_advice_code[4];
extern CHAR  reject_detail_code[5];



/******************************************************************************
 *
 *  NAME:         SAF_SEND_REQUEST_FOR_RECORD
 *
 *  DESCRIPTION:  This function sends a request to the data server to retrieve
 *                a transaction record that has the store for forwarding
 *                for this network.
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
 *     DI - 02/25/99 - Copy authorizing_host_queue name into tlf01.
 ******************************************************************************/
BOOLEAN saf_send_request_for_record()
{
   pPTE_MSG        p_msg_out = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(SAF01)];
   BOOLEAN         ret_code = false;


   /* Build message to get SAF transaction from data server. */
   memset( buffer1, 0x00, sizeof(buffer1) );
   memset( &saf01,   0x00, Saf01_Size   );
   strcpy( saf01.handler_queue, application_que_name );
   memcpy( buffer1+Auth_Tx_Size, &saf01, Saf01_Size );

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST, 
                                 ST1_DB_SELECT, 
                                 ST2_NONE,
                                 netds_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 SAF01_DATA );
   if ( NULL_PTR == p_msg_out  )
   {
      cirsutil_error_handler(1, "Insufficient Memory to build SAF01 message",
                             1, ALERT_MSG, "saf_send_request_for_record", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "554" );
   }
   else
   {
      /* Send message to get host specific data from data server. */
      ret_code = send_msg( p_msg_out, netds_que_name, (pAUTH_TX)NULL_PTR );
      free( p_msg_out );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         SAF_PROCESS_DB_RESPONSE
 *
 *  DESCRIPTION:  This function is called when a transaction that has been
 *                stored, for forwarding, is selected.  It is also called if
 *                one is not found.  If one is not found, it will set the host
 *                state to active.  If one is found, it will call the necessary
 *                functions to process the transaction.  In Cirrus's case, only
 *                reversals are stored and forwarded.
 *
 *  INPUTS:       pCurrent   - String containing transaction
 *                txn_status - OK or NOT_FOUND
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN saf_process_db_response( pBYTE pCurrent, BYTE txn_status )
{
   AUTH_TX     auth_tx;
   BOOLEAN     ret_val = true;


   if ( PTEMSG_OK != txn_status )
   {
      /* There are no transactions that were stored for forwarding. */

      /* Begin the timer to check again later. */
      ret_val = saf_initiate_timer();
   }
   else
   {
      /* Process the reversal transaction. */
      if ( NULL != pCurrent )
      {
         /* Get data server message */
         memset( &saf01, 0, Saf01_Size);
         memcpy( &saf01, pCurrent+(Auth_Tx_Size), Saf01_Size);

         /* Copy the SAF record into the auth_tx record. */
         memset( &auth_tx, 0x00, Auth_Tx_Size );
         (void)cirsutil_init_txn( &auth_tx );

         /* Inidicate we are in Store and Forward mode. */
         strcpy( auth_tx.TLF01_details.saf, "Y" );

         /* Copy data into auth_tx, and process the reversal. */
         copy_saf01_into_auth_tx( &saf01, &auth_tx );
         if ( false == process_out_going_reversal( &auth_tx ) )
            ret_val = false;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SAF_REMOVE_FROM_SAF01
 *
 *  DESCRIPTION:  This function sends a request to the data server to remove
 *                a record from database table SAF01.  The data server response
 *                will be ignored.  Note that auth_tx is sent to SAF01.  It
 *                contains the transaction id to be deleted.
 *
 *  INPUTS:       p_auth_tx - transaction to be removed.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN saf_remove_from_saf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg_out = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(SAF01)];
   BOOLEAN         ret_code = false;

   memset( &saf01, 0x00, Saf01_Size );
   memcpy( saf01.primary_key.transaction_id,
           p_auth_tx->TLF01_details.primary_key.transaction_id,
           sizeof(saf01.primary_key.transaction_id) );

   /* Copy transaction into message buffer. */
   memset( buffer1, 0, sizeof(buffer1) );
   memcpy( buffer1+Auth_Tx_Size, &saf01, Saf01_Size);

   /* Build message to get SAF transaction from data server. */
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST, 
                                 ST1_DB_DELETE, 
                                 ST2_NONE,
                                 netds_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 SAF01_DATA );
   if ( NULL_PTR == p_msg_out  )
   {
      cirsutil_error_handler(1,
                            "Insufficient Memory to build SAF01-delete message",
                             1, ALERT_MSG, "saf_remove_from_saf01", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "557" );
   }
   else
   {
      /* Send message to data server to update record. */
      ret_code = send_msg( p_msg_out, netds_que_name, p_auth_tx );
      free( p_msg_out );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_OUT_GOING_REVERSAL
 *
 *  DESCRIPTION:  This function does what is necessary to format and forward
 *                a reversal transaction to the host.
 *
 *  INPUTS:       p_auth_tx - transaction to be updated
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 02/26/99 - Modified to get original txn for reversals by include
 *                     card number and total_amount as keys.
 ******************************************************************************/
BOOLEAN process_out_going_reversal( pAUTH_TX p_auth_tx )
{
   INT       ret_val = true;

   /* Convert from a reversal to a reversal advice for Cirrus. */
   p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;

   /* Determine if Full or Partial reversal. */
   if ( 0 == strcmp( p_auth_tx->TLF01_details.total_amount,
                     p_auth_tx->TLF01_details.reversal_amount ) )
   {
      /* Full Reversal */
      strcpy( reject_advice_code, "450" );
      strcpy( reject_detail_code, "1018" );
   }
   else
   {
      /* Partial Reversal */
      strcpy( reject_advice_code, "451" );
      strcpy( reject_detail_code, "1010" );
   }

   /* Continue processing of reversal. */
   ret_val = incoming_terminal_request( p_auth_tx );

   /* Perform error processing if there is a problem and not in SAF mode. */
   if ( false == ret_val )
   {
      if ( 0 != strcmp( "Y", p_auth_tx->TLF01_details.saf) )
	  {
         strcpy( p_auth_tx->TLF01_details.saf, "Y" );
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
         perform_standin_processing ( "00", GS_TXN_COMPLETED, p_auth_tx );
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         PROCESS_REVERSAL_RESPONSE
 *
 *  DESCRIPTION:  This function handles the response of a reversal response
 *                from the host.  It removes the record from SAF01.  Then it
 *                makes a call to get the next SAF record from TLF01.
 *
 *                It is assumed that the calling function already checked to
 *                see that we are in SAF mode.  If not, we do not want to
 *                come here.  A reversal response would be handled as any
 *                other response.
 *
 *  INPUTS:       p_auth_tx - transaction returned from the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN process_reversal_response( pAUTH_TX p_auth_tx )
{
   CHAR  Buff[100];

   if ( 0 != strcmp( "00", p_auth_tx->TLF01_details.response_code ) )
   {
      strcpy( Buff, "Unable to forward message, tran id = " );
      strcat( Buff, p_auth_tx->TLF01_details.primary_key.transaction_id );
      cirsutil_error_handler(1, Buff, 1, WARN_MSG,
                             "process_reversal_response", 3, WARNING_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "565");

      strcpy( Buff, "Response code = " );
      strcat( Buff, p_auth_tx->TLF01_details.response_code );
      cirsutil_error_handler(1, Buff, 1, WARN_MSG,
                             "process_reversal_response", 3, WARNING_ERROR,
                             p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                             "566");
   }
   saf_remove_from_saf01( p_auth_tx );
   saf_send_request_for_record();
   return(true);
}

/******************************************************************************
 *
 *  NAME:         SAF_INITIATE_TIMER
 *
 *  DESCRIPTION:  This function sets a timer on which SAF will be initiated
 *                whenever the timer expires.
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
 *     DI - 03/04/99 - Set repeat timer to 0 so we do not get repeats.
 ******************************************************************************/
BOOLEAN saf_initiate_timer()
{
   CHAR     unique_tran_str_id[11] = "";
   pPTE_MSG p_msg_set_timer        = NULL_PTR;
   BOOLEAN  ret_code = false;

   /* Create a unique transaction id. */
   cirsutil_get_strtime( unique_tran_str_id );

   p_msg_set_timer = ptetimer_bld_start_timer_req(
                     SAF_TIMER_KEY,
                     unique_tran_str_id,
                     ARTIFICIAL_TERMINAL_ID,
                     timerds_que_name,
                     application_que_name,
                     300,                 /* 5 minutes */
                     0,                   /* Do not want repeat timeouts */
                     (pBYTE)0,
                     0 );

   if ( NULL_PTR == p_msg_set_timer )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory to build timer message",
                             1, ALERT_MSG, "saf_initiate_timer", 4,
                             FATAL_ERROR,
                             NULL_STR, NULL_STR, NULL_STR, "560" );
   }
   else
   {
      ret_code = send_msg(p_msg_set_timer, timerds_que_name, (pAUTH_TX)NULL_PTR );
      free( p_msg_set_timer );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         SAF_CLEAR_TIMER
 *
 *  DESCRIPTION:  This function clears the SAF timer, usually after a log off.
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
 *     DI - 03/19/99 - Changed code to call 'clear_timer'.
 ******************************************************************************/
BOOLEAN saf_clear_timer()
{
   BOOLEAN  ret_code = false;

   /* Build PTEMSG to clear timer */
   ret_code = clear_timer( SAF_TIMER_KEY );
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         COPY_SAF01_INT0_AUTH_TX
 *
 *  DESCRIPTION:  This function copies information from data fields in auth_tx
 *                into data fields in SAF01.
 *
 *  INPUTS:       p_saf01
 *
 *  OUTPUTS:      p_auth_tx
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void copy_saf01_into_auth_tx( pSAF01 p_saf01, pAUTH_TX p_auth_tx )
{
   CHAR      time_date[25] = "";
   INT       i;

   memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
           p_saf01->primary_key.transaction_id,
           sizeof(p_saf01->primary_key.transaction_id) );
   memcpy( p_auth_tx->TLF01_details.card_num, p_saf01->card_num,
           sizeof(p_saf01->card_num) );

   memcpy( p_auth_tx->TLF01_details.orig_amount, p_saf01->tran_amount,
           sizeof(p_saf01->tran_amount) );
   memcpy( p_auth_tx->TLF01_details.terminal_id, p_saf01->device_id,
           sizeof(p_saf01->device_id) );

   memcpy( p_auth_tx->TLF01_details.orig_message, p_saf01->message_type,
           sizeof(p_saf01->message_type) );
   memcpy( p_auth_tx->TLF01_details.message_type, "0400",
           sizeof(p_saf01->message_type) );
   memcpy( p_auth_tx->TLF01_details.processing_code,p_saf01->processing_code,
           sizeof(p_saf01->processing_code) );

   memcpy( p_auth_tx->TLF01_details.orig_sys_trace_num, p_saf01->sys_trace_audit_nbr,
           sizeof(p_saf01->sys_trace_audit_nbr) );
   memcpy( p_auth_tx->TLF01_details.date_yyyymmdd, p_saf01->tran_date,
           sizeof(p_saf01->tran_date) );

   memcpy( p_auth_tx->TLF01_details.time_hhmmss, p_saf01->tran_time,
           sizeof(p_saf01->tran_time) );
   memcpy( p_auth_tx->TLF01_details.orig_retrieval_ref_num, p_saf01->retrieval_ref_num,
           sizeof(p_saf01->retrieval_ref_num) );

   memcpy( p_auth_tx->TLF01_details.merchant_id, p_saf01->merchant_id,
           sizeof(p_saf01->merchant_id) );
   memcpy( p_auth_tx->TLF01_details.auth_number, p_saf01->auth_number,
           sizeof(p_saf01->auth_number) );

   if ( 0 == strcmp( NULL_STR, p_saf01->response_code ) )
      strcpy( p_saf01->response_code, "00" );
   memcpy( p_auth_tx->TLF01_details.response_code, p_saf01->response_code,
           sizeof(p_saf01->response_code) );

   memcpy( p_auth_tx->TLF01_details.mcard_banknet, p_saf01->network_data,
           sizeof(p_saf01->network_data) );
   i = strlen( p_saf01->network_data );
   sprintf( p_auth_tx->TLF01_details.mcard_banknet_len, "%03d", i );


   memcpy( p_auth_tx->TLF01_details.invoice_number, p_saf01->invoice_nbr,
           sizeof(p_saf01->invoice_nbr) );
   memcpy( p_auth_tx->TLF01_details.currency_code, p_saf01->currency_code,
           sizeof(p_saf01->currency_code) );

   memcpy( p_auth_tx->TLF01_details.transmission_timestamp, p_saf01->transmission_timestamp,
           sizeof(p_saf01->transmission_timestamp) );
   memcpy( p_auth_tx->TLF01_details.exp_date, p_saf01->exp_date,
           sizeof(p_saf01->exp_date) );

   memcpy( p_auth_tx->TLF01_details.settlement_total, p_saf01->settlement_amount,
           sizeof(p_saf01->settlement_amount) );

   if ( 0 == strcmp( NULL_STR, p_saf01->settlement_date ) )
   {
	   ptetime_get_timestamp( time_date );

	   strncpy(p_saf01->settlement_date,&time_date[5],2);
	   strncat(p_saf01->settlement_date,&time_date[8],2);
   }
   memcpy( p_auth_tx->TLF01_details.settlement_date, p_saf01->settlement_date,
           sizeof(p_saf01->settlement_date) );

   memcpy( p_auth_tx->settlement_conv_rate, p_saf01->settlement_conv_rate,
           sizeof(p_saf01->settlement_conv_rate) );
   memcpy( p_auth_tx->settlement_curr_code, p_saf01->settlement_curr_code,
           sizeof(p_saf01->settlement_curr_code) );

   memcpy( p_auth_tx->ch_billing_amt, p_saf01->billing_amount,
           sizeof(p_saf01->billing_amount) );
   memcpy( p_auth_tx->ch_billing_conv_rate, p_saf01->billing_conv_rate,
           sizeof(p_saf01->billing_conv_rate) );

   memcpy( p_auth_tx->ch_billing_curr_code, p_saf01->billing_curr_code,
           sizeof(p_saf01->billing_curr_code) );
   memcpy( p_auth_tx->TLF01_details.tran_fee_amount, p_saf01->tran_fee_amount,
           sizeof(p_saf01->tran_fee_amount) );

   memcpy( p_auth_tx->TLF01_details.handler_queue, p_saf01->handler_queue,
           sizeof(p_saf01->handler_queue) );
   memcpy( p_auth_tx->acq_inst_id_cd, p_saf01->acquiring_inst_id_code,
           sizeof(p_saf01->acquiring_inst_id_code) );

   memcpy( p_auth_tx->forwarding_inst_id_cd, p_saf01->forwarding_inst_id_code,
           sizeof(p_saf01->forwarding_inst_id_code) );
   memcpy( p_auth_tx->TLF01_details.reversal_amount, p_saf01->reversal_amount,
           sizeof(p_saf01->reversal_amount) );

   p_auth_tx->TLF01_details.tx_key = p_saf01->tx_key;
}

/******************************************************************************
 *
 *  NAME:         INSERT_INTO_SAF01
 *
 *  DESCRIPTION:  This function sends a request to the Network Data Server to
 *                insert auth_tx data into SAF01.  The data server takes care
 *                of converting auth_tx into saf01.
 *
 *  INPUTS:       p_auth_tx
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN insert_into_saf01( pAUTH_TX  p_auth_tx )
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BOOLEAN    ret_code  = false;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(SAF01)];
   SAF01      saf01;

   memset( &saf01, 0x00, Saf01_Size  );
   memset( buffer, 0,    sizeof(buffer) );
   memcpy( buffer, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer+Auth_Tx_Size,&saf01, Saf01_Size);

   p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
                                ST1_DB_INSERT, 
                                ST2_NONE,
                                netds_que_name, 
                                application_que_name,
                                (pBYTE)&buffer,
                                sizeof(buffer), 
                                SAF01_DATA); 
   if ( NULL_PTR == p_msg_ins )
   {
      cirsutil_error_handler(1,
                            "Insufficient Memory to build insert SAF01 message",
                             1, ALERT_MSG, "insert_into_saf01", 4,
                             FATAL_ERROR, p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id,
                            "604");
   }
   else
   {
      ret_code = send_msg( p_msg_ins, netds_que_name, p_auth_tx );
      free( p_msg_ins );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_ISSUER_REVERSALS
 *
 *  DESCRIPTION:  This procedure is called if Cirrus sends us an issuer
 *                reversal advicw, 0422.  We response with an 0432.  These are
 *                actually adjustments.  Right now, we don't do anything with
 *                it except respond.
 *
 *                There are actually 3 different kinds of issuer reversal
 *                advices: Chargeback, Adjustment, and Representment
 *
 *                Chargebacks (DE60=489) - This is an issuer-generated reversal
 *                advice message that informs an acquirer that a previously
 *                completed charge to the cardholder's account is not valid,
 *                and that the acquirer will be "charge back" that amount. A
 *                chargeback results in a credit to the issuer and a debit to
 *                the acquirer.
 *
 *                Adjustment (DE60=490) - This is an acquirer-generated reversal
 *                advice message that corrects the amount settled in a
 *                previously completed transaction.  An adjustment may result
 *                in either a debit or a credit to the issuer.
 *
 *                Representment (DE60=491) - This is an acquirer-generated
 *                reversal advice message that informs an issuer a previous
 *                chargeback from the issuer is not valid, and the transaction
 *                is being "represented" for settlement.  A representment
 *                results in a debit to the issuer and a credit to the acquirer.
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
void process_issuer_reversals( pAUTH_TX p_auth_tx )
{
   /*+--------------------------------------------+
     | Build a 0432 message to reply to the 0422. |
     +--------------------------------------------+*/
   (void)cirsutil_init_txn( p_auth_tx );

   strncpy( p_auth_tx->TLF01_details.message_type, "0432", 4 );
   strncpy( p_auth_tx->TLF01_details.response_code, "00", 2 );
   p_auth_tx->TLF01_details.tx_key = AUTH_ISSUER_REVERSAL_ADVICE_RESPONSE;
   strcpy( p_auth_tx->TLF01_details.originator_queue, dialog_que_name );
   strcpy( p_auth_tx->TLF01_details.tpdu_id, ncf01.tpdu_id );

   /*+------------------------+
     | Send 0432 to the host. |
     +------------------------+*/
   if ( false == incoming_other_host_response( p_auth_tx ) )
   {
      cirsutil_error_handler(1, "Unable to send 0432 message to Cirrus host.",
                             1, INFO_MSG, "process_issuer_reversals", 1,
                             INFO_ERROR, NULL_STR, NULL_STR, NULL_STR,"536");
   }
   return;
}  /* process_issuer_reversals */

