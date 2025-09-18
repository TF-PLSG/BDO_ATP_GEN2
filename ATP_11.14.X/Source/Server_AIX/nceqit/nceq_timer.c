/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCEQ_TIMER.C
 *  
 * TITLE:       Equitable Network Controller Timer Processor
 *  
 * DESCRIPTION: This module contains functions that have anything to do
 *              with a timer.  These can be transaction timeouts, echo
 *              timers timing out, or the time out of any other kind of
 *              timer that might be used.  It also includes functions
 *              that set and clear timers.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq_timer.c  $

	  Rev 1.17   Jan 28 2009 16:10:00   Girija Y, TF
   Added code, not to send reversal for 0422( Charge back ) messages in case of time-outs.
   ATP- EQIT Interface
   
      Rev 1.16   Oct 12 2005 14:48:42   dirby
   Modified to use orig stan for timer keys for voids.
   SCR 17660
   
      Rev 1.15   Oct 01 2004 16:31:28   dirby
   Modified creation of request timer key to use original stan for Reload Voids.
   This is so we can find timer key on the response from EB Host.
   SCR 12626
   
      Rev 1.14   Sep 21 2004 12:00:04   dirby
   Modified to not send reversals to host when reload confirms timeout.
   SCR 12505
   
      Rev 1.13   Jul 14 2004 14:29:16   dirby
   Added support for reload txns during timeouts.
   SCR 1287
   
      Rev 1.12   Sep 02 2003 16:41:20   svelaga
   AUTH_ADMINISTRATIVE messages are directly sent to ncvsms.
   
      Rev 1.11   Aug 02 2002 15:35:06   dirby
   Removed code so time outs of echo requests do not increment
    the timeout counter.  This is so we do not go offline due to too
   many echo request time outs.  Only increment the timeout
   counter for timeouts of financial transactions.
   SCR 594
   
      Rev 1.10   Jul 31 2002 06:51:14   dirby
   Removed code that updated time out counts for Monitor when
   an echo request times out.  This is at the request of EB.
   SCR 594
   
      Rev 1.9   Jul 26 2002 13:25:46   dirby
   Modified to allow 0800 echo requests to go out when host
   state is Offline due to Maximum Time Outs.  Will go back Online
   when a response to the echo request is received.
   SCR 594
   
      Rev 1.8   Jul 12 2002 13:12:20   dirby
   Modified to update network status is Max Timeouts occurs.
   SCR 594
   
      Rev 1.7   Jul 05 2002 13:14:52   dirby
   Added code to display a message for time outs.
   SCT 621
   
      Rev 1.6   Jun 17 2002 23:12:46   dirby
   Added code to trim trailing space from the TID when creating
   the timer lookup key.  Lookup keys for TIDs with spaces were
   being created differently when setting the timer than when
   clearing the timer.
   SCR 789
   
      Rev 1.5   Jan 18 2002 11:24:34   dirby
   Corrected some minor bugs.
   SCRs 488 and 484
   
      Rev 1.4   Jan 14 2002 18:14:34   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.3   Oct 11 2001 12:08:04   dirby
   Added code to set currency code and amount back to the
   original values prior to sending a time out response to the
   authorizer.  SCR 426
   
      Rev 1.2   Oct 10 2001 17:23:00   dirby
   Added a line of code to set response code to 68 for reversals
   of timed out transactions.  SCR 510
   
      Rev 1.1   Aug 20 2001 17:11:54   dirby
   Removed code that filled the pin block with zeroes when a txn
   timed out and we are preparing for a reversal.  This gets
   handled when field 52 is built for the reversal.  SCR 440.
   
      Rev 1.0   Aug 06 2001 09:48:36   dirby
   Initial revision.
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "ptetimer.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "nceq_constants.h"
#include "nceq_prototypes.h"
#include "txutils.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern INT     Echo_Timer_Flag;

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

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Atm01_Size;
extern INT  Tlf01_Size;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

extern CHAR    TimerAppData2[];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Counters */
extern INT    MaxConsecTimeouts;
extern INT    Echo_Timer_Count;

/******************************************************************************
 *
 *  NAME:         PROCESS_TIMEOUT_MSG
 *
 *  DESCRIPTION:  This function deals with a timer that has expired. Currently
 *                there are two timers that might expire: transaction request
 *                to a host, and an echo timer.
 *
 *                If the expired timer is a transaction request, then check
 *                to see if a repeat is required.  If yes, repeat the request
 *                to the host.  If not, time out the transaction by sending a
 *                timeout response back to the authorizer.  Do not do this for
 *                timeouts of repeat transactions (only send timeout response
 *                once).  When a transaction request times out, increment the
 *                number of consecutive timeouts in shared memory.
 *
 *                If an echo timer expires, issue an 0800 echo request to the
 *                host.  Then reset the echo timer.
 *
 *  INPUTS:       p_msg - This is the timeout msg from timerds
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_timeout_msg( pPTE_MSG p_msg )
{
   BYTE        buffer2[AUTH_TX_SIZE];
   CHAR        lookup_key[MAX_TIMER_KEY_SIZE];
   CHAR        app_data1[MAX_APP_DATA_SIZE];
   CHAR        app_data2[MAX_APP_DATA_SIZE];
   CHAR		   err_buf[100];
   CHAR        echo_timer_key[LOOKUP_KEY_SIZE]= "";
   CHAR        echo_req_timer_key[LOOKUP_KEY_SIZE]= "";
   BYTE        temp_tx_key;
   LONG        timer_reply_size = 0;
   INT         repeat_ctr;
   INT         max_repeats;

   /* Get timeout values */
   timer_reply_size = ptetimer_get_timer_reply( p_msg, lookup_key,
                                                app_data1, app_data2,
                                                buffer2,Auth_Tx_Size );

   /* Determine what timed out: echo timer or txn request. */
   memset( &Auth_Tx, 0x00, Auth_Tx_Size );
   create_echo_timer_key( echo_timer_key );
   create_echo_req_timer_key( echo_req_timer_key );
   if ( 0 == strcmp(lookup_key, echo_timer_key) )
	{
      /* 1 . Echo timer is automatically cleared when it times out
       * 2 . Send ECHO request to host (this sets the echo timer)
       * 3 . Flag timer code that echo timer is not set.
       */
      Echo_Timer_Flag = ECHO_TIMER_IS_CLEARED;
      (void)issue_0800( ECHO );
	}
   else if (0 == strcmp(lookup_key, echo_req_timer_key) )
   {
      /* An Echo request has timed out. Take no action here  */
      /* Allow the echoes to continue going out as normal.   */
      /* --------------------------------------------------- */
      (void)decrement_active_count( err_buf );
   }
	else
	{
      if ( timer_reply_size != Auth_Tx_Size )
	   {
         sprintf( err_buf,
                 "Error retrieving auth_tx from timer message. Lookup key = %s",
                  lookup_key );
         nceqit_log_message( 1, 2, err_buf, "process_timeout_msg",0 );
	   }
	   else
      {
         /* Get Auth_Tx buffer from timeout message. */
         memcpy( &Auth_Tx, buffer2, Auth_Tx_Size );

         /* Update transaction statistics - update for a timeout response. */
         strcpy( Auth_Tx.TLF01_details.response_code, TO );
         update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

         /* Display time out message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants.
          */
         memset( err_buf, 0x00, sizeof(err_buf) );
         sprintf( err_buf,
                 "Txn to EQIT timed out.Merchant %s",
                  Auth_Tx.TLF01_details.merchant_id );
         nceqit_log_message( 1, 1, err_buf, "process_timeout_msg",0 );

         /* Only send timeout response if txn has not been repeated. */
         memset( err_buf, 0x00, sizeof(err_buf) );
         repeat_ctr = atoi( Auth_Tx.advice_repeat_count );
         if ( repeat_ctr == 0 )
         {
            /* Do not send response if we generated the reversal
             * due to an 0200 timeout.
             */
            if ( 0 != strcmp(app_data2, TIMER_REVERSAL) )
            {
               temp_tx_key = Auth_Tx.TLF01_details.tx_key;
               if ( false == send_timeout_response(err_buf) )
                  nceqit_log_message( 1, 2, err_buf, "process_timeout_msg",0 );
               Auth_Tx.TLF01_details.tx_key = temp_tx_key;
            }
         }

         /* Increment number of consecutive timeouts.
          * It returns false if max timeouts is reached.
          */
         if ( false == increment_timeout_count(err_buf) )
         {
            /* Either max timeouts has been reached,
             * or there is a problem with shared memory.
             * Either way, log the problem and discontinue processing.
             */
            nceqit_log_message( 1, 1, err_buf, "process_timeout_msg" ,0);
            (void)send_network_status_to_monitor();
         }
         else
         {
            /* Reversals need to be repeated if they time out. */
            if ( is_reversal() )
            {
            	if(0==strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6) &&
            		0==strncmp(Auth_Tx.TLF01_details.message_type,"0420",4)	)
            	{
            		//For 0420 visa timoeut reversal, Send only 1 reversal.

            	}
            	else
            	{
            		/* This is a reversal. Resend it to the host. */
            		max_repeats = atoi( Ncf01_I.advice_repeat_counter );
            		if ( repeat_ctr < max_repeats )
            		{
            			repeat_ctr++;
            			if ( repeat_ctr > 99 )
            				repeat_ctr = 99;
            			sprintf( Auth_Tx.advice_repeat_count, "%02d", repeat_ctr );

            			strcpy( Auth_Tx.TLF01_details.message_type, "0421" );
            			Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT;

            			/* Use TIMER_REVERSAL so response does not go to txcntl. */
            			(void)send_request_to_host( TIMER_REVERSAL );
            		}
            	}
            }
            else
            {
               /* When an 0200 txn times out, need to issue 0420 to host.
                * As this txn is already logged to TLF01, we can change
                * whatever is necessary to transform it into a reversal.
                */

				/* Dont send 0420 to host for 0422 msg request in case of time-out */
				/* Girija Y TF, ATP-EQIT Interface, Jan 28, 2009 */
               if ((Auth_Tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY) &&
                   (Auth_Tx.TLF01_details.tx_key != AUTH_RELOAD_REPEAT  ) &&
                   (Auth_Tx.TLF01_details.tx_key != AUTH_RELOAD_CONFIRM ) &&
                   (Auth_Tx.TLF01_details.tx_key != AUTH_RELOAD         ) && 
				   (Auth_Tx.TLF01_details.tx_key != AUTH_CHARGEBACK     ) &&
				   (Auth_Tx.TLF01_details.tx_key != AUTH_CHARGEBACK_REVERSAL ) )
               {
               	if(0==strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6) &&
               		0==strncmp(Auth_Tx.TLF01_details.message_type,"0120",4))
               	{
               		//For 0120 timeout.. dont send the 0420 to IST

               	}
               	else
               	{
                  (void)transform_0200_into_0420();
                  (void)send_request_to_host( TIMER_REVERSAL );
               	}
               }
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         SEND_TIMEOUT_RESPONSE
 *
 *  DESCRIPTION:  This function takes care of the details of sending a
 *                response (of a transaction that timed out) to the
 *                authorizer.  It sets the general status and response
 *                code and tx_key.  Then is sends the response.  It also
 *                decrements the number of active txns.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Textual message when there is an error
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_timeout_response( pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE temp_cur_code[4];
   BYTE temp_amount[13];


   /* Set tx_key to be for a response. */
   if ( Auth_Tx.TLF01_details.tx_key < 100 )
      Auth_Tx.TLF01_details.tx_key += 100;

   if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y' )
   {
      strcpy(Auth_Tx.TLF01_details.voice_auth_text, "Trnsxn Timed Out");
   }

   /* Restore original amount, if currency conversion took place. */
   if ( Auth_Tx.reconciliation_amount[0] != 0x00 )
   {
      memset( temp_cur_code, 0x00, sizeof(temp_cur_code) );
      memset( temp_amount,   0x00, sizeof(temp_amount  ) );

      strcpy( temp_cur_code, Auth_Tx.TLF01_details.currency_code );
      strcpy( temp_amount,   Auth_Tx.TLF01_details.total_amount  );

      memcpy( Auth_Tx.TLF01_details.currency_code,
              Auth_Tx.reconciliation_cur_code,
              sizeof(Auth_Tx.TLF01_details.currency_code));
      memcpy( Auth_Tx.TLF01_details.total_amount,
              Auth_Tx.reconciliation_amount,
              sizeof(Auth_Tx.TLF01_details.total_amount));
   }

   /* Send response to authorizer. */
   if(0==strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6) &&
		  ( AUTH_REVERSAL_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key ||
		   AUTH_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key))
   {
	   ret_val = perform_standin_processing( VISA_TO, GS_TXN_TIMEDOUT, err_buf );
   }
   else
   {
	   ret_val = perform_standin_processing( TO, GS_TXN_TIMEDOUT, err_buf );
   }
   if (ret_val == true )
      ret_val = decrement_active_count( err_buf );

   /* Need to put these values back for the reversal to the host. */
   if ( Auth_Tx.reconciliation_amount[0] != 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code, temp_cur_code );
      strcpy( Auth_Tx.TLF01_details.total_amount,  temp_amount   );
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         TRANSFORM_0200_INTO_0420
 *
 *  DESCRIPTION:  This function takes an Auth_Tx structure containing an 0200
 *                transaction and changes fields in such a way as to make it
 *                an 0420 transaction.  Fields such as message_type, tx_key,
 *                and 'original' fields are changed or populated.
 *
 *  INPUTS:       Auth_Tx - (Global) 0200 transaction info
 *
 *  OUTPUTS:      Auth_Tx - (Global) 0420 transaction info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void transform_0200_into_0420()
{
  if ( Auth_Tx.TLF01_details.processing_code[0] == '2' && Auth_Tx.TLF01_details.processing_code[1] == '6' )	
  {Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;}
   else if ( Auth_Tx.TLF01_details.processing_code[0] == '2' )
   {
      /* Processing code for Reloads is 21nnnn. */
      Auth_Tx.TLF01_details.tx_key = AUTH_VOID_RELOAD;
   }
   else
      Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;

   strcpy( Auth_Tx.TLF01_details.orig_message,
           Auth_Tx.TLF01_details.message_type );

   strcpy( Auth_Tx.TLF01_details.message_type, "0420" );

   strcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
           Auth_Tx.TLF01_details.sys_trace_audit_num );

   strcpy( Auth_Tx.orig_local_time_date,
           Auth_Tx.TLF01_details.transmission_timestamp );

   strcpy( Auth_Tx.orig_acq_inst_id, Auth_Tx.acq_inst_id_cd );

   strcpy( Auth_Tx.TLF01_details.response_code, EQIT_TIMEOUT );

   /* Need to create track2 if not there already. */
   if ( Auth_Tx.TLF01_details.track2[0] == 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.track2, Auth_Tx.TLF01_details.card_num );
      strcat( Auth_Tx.TLF01_details.track2, "="                            );
      strcat( Auth_Tx.TLF01_details.track2, Auth_Tx.TLF01_details.exp_date );
   }

   /* Amount dispensed is zero because txn timed out. */
   memset( Auth_Tx.TLF01_details.actual_amount, '0', 12 );

   return;
}

/******************************************************************************
 *
 *  NAME:         SET_TIMER
 *
 *  DESCRIPTION:  This procedure builds a timer message and sends it to the
 *                timer data server.
 *
 *  INPUTS:       lookup_key    - Timer key for referencing
 *                timer_length  - Number of seconds to set the timer
 *                TimerAppData2 - Flag used to not forward reversals to txcntl
 *                type_of_timer - Flag to determine if Auth_Tx should be sent
 *                AppName       - (Global)Application name to create queue name
 *
 *                Note about 'TimerAppData2' -> If 0200 txns timeout, the a
 *                reversal is automatically sent to the host.  The reversal 
 *                response should not be forwarded to the acquiring application,
 *                i.e. dcpiso or ncvisa or ncjcb.
 *
 *                Its values: TIMER_REVERSAL
 *                            TIMER_FINANCIAL
 *                            TIMER_0800
 *                            TIMER_ECHO
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer )
{
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   BYTE     source_que[20]  = "";
   BYTE     buffer1[AUTH_TX_SIZE];
   CHAR     err_buf[100]="";
   INT      ret_val = false;

   /* Send the whole transaction record. */
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, AppName );
   strcat( source_que, "A"     );

   /* Determine if this is a timer for a transaction to the host or not. */
   if ( type_of_timer == REQUEST_TIMER )
   {
      /* Build PTE message that will start the timer. */

      /* This type of timer stores Auth_Tx with the timer. */
      p_msg_set_timer = ptetimer_bld_start_timer_req(
                            lookup_key,
                            Auth_Tx.TLF01_details.primary_key.transaction_id,
                            TimerAppData2,
                            timerds_que_name,
                            source_que,
                            (WORD)timer_length,
                            0,
                            (pBYTE)&buffer1,  
                            Auth_Tx_Size );
   }
   else // Echo Timer
   {
      /* This type of timer does not stores Auth_Tx with the timer. */
      p_msg_set_timer = ptetimer_bld_start_timer_req(
                            lookup_key,
                            NULL,
                            NULL,
                            timerds_que_name,
                            source_que,
                            (WORD)timer_length,
                            0,
                            NULL_PTR,  
                            0 );
   }

   if ( NULL_PTR == p_msg_set_timer )
   {
      strcpy( err_buf, "Insufficient Memory to build timer message" );
   }
   else
   {
      ret_val = send_msg(p_msg_set_timer, timerds_que_name, err_buf);
      free( p_msg_set_timer );
   }

   if ( false == ret_val )
      nceqit_log_message( 1, 2, err_buf, "set_timer",0 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CLEAR_TIMER
 *
 *  DESCRIPTION:  This function clears a timer using a lookup
 *                key passed into it.
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
INT clear_timer( pCHAR lookup_key )
{
   pPTE_MSG   p_msg   = NULL_PTR;
   INT        ret_val = false;
   CHAR       source_que[30]="";
   CHAR       err_buf[100]="";

   /* Set up the queue name for the reply from timerds. */
   strcpy( source_que, AppName );
   strcat( source_que, "A"     );

   /* Build PTEMSG to clear timer */
   p_msg = ptetimer_bld_clear_timer_req(lookup_key,timerds_que_name,source_que);
   if ( NULL_PTR == p_msg )
   { 
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Clear Timer message  */
      ret_val = send_msg(p_msg, timerds_que_name, err_buf);
      free( p_msg );
   }

   if ( false == ret_val )
      nceqit_log_message( 1, 2, err_buf, "clear_timer",0 );

   return( ret_val );
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
INT clear_request_timer( pCHAR lookup_key )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val       = false;
   CHAR       source_que[30]="";
   CHAR       err_buf[100]  ="";

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
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
                                              MT_DB_REQUEST,
                                              ST1_DB_ECHO,
                                              ST2_NONE,
                                              timerds_que_name,
                                              application_que_name,
                                              (pBYTE)(&Auth_Tx), 
                                              Auth_Tx_Size,
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
      }
   }

   if ( false == ret_val )
      nceqit_log_message( 1, 2, err_buf, "clear_timer",0 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         RESET_ECHO_TIMER
 *
 *  DESCRIPTION:  This function checks the echo interval.  If greater than
 *                zero, then the timer is cleared.  Then the timer is set.
 *                The purpose of this function is to recycle the echo timer
 *                whenever there is any activity with the host.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message in case of failure
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT reset_echo_timer( pCHAR err_buf )
{
   INT   ret_val = true;
   INT   echo_timer_seconds;

   echo_timer_seconds = atoi( Ncf01_I.echo_interval );
   if ( echo_timer_seconds > 0 )
   {
      ret_val = clear_echo_timer( err_buf );
      if ( true == ret_val )
         ret_val = set_echo_timer( err_buf );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SET_ECHO_TIMER
 *
 *  DESCRIPTION:  This function checks the echo interval.  If greater than
 *                zero, a timer key is created.  Then the timer is set.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message in case of failure
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT set_echo_timer( pCHAR err_buf )
{
   INT   ret_val = true;
   INT   timeouts;
   INT   echo_timer_seconds;
   CHAR  echo_timer_key[LOOKUP_KEY_SIZE];

   if ( Echo_Timer_Flag == ECHO_TIMER_IS_CLEARED )
   {
      echo_timer_seconds = atoi( Ncf01_I.echo_interval );
      if ( echo_timer_seconds > 0 )
      {
         /* Only set echo timer if ONLINE with host. */
         ret_val = read_shared_mem_table( net_consec_tmouts_ctr,active_txns_ctr,
                                          current_state, err_buf );
         if ( ret_val == true )
         {
            if( 0 == strcmp( current_state, ONLINE ) )
            {
               memset( echo_timer_key, 0x00, sizeof(echo_timer_key) );
               create_echo_timer_key( echo_timer_key );
               strcpy( TimerAppData2, TIMER_ECHO );
               if (false == set_timer(echo_timer_key,echo_timer_seconds,ECHO_TIMER))
               {
                  strcpy( err_buf, "Unable to set Echo timer." );
                  ret_val = false;
               }
               else
                  Echo_Timer_Flag = ECHO_TIMER_IS_SET;
            }
            else if ( 0 == strcmp(current_state, OFFLINE) )
            {
               timeouts = atoi(net_consec_tmouts_ctr);
               if ( timeouts >= MaxConsecTimeouts )
               {
                  /* We are Offline due to Max Timeouts.
                   * Continue to send echoes.
                   */
                  memset( echo_timer_key, 0x00, sizeof(echo_timer_key) );
                  create_echo_timer_key( echo_timer_key );
                  strcpy( TimerAppData2, TIMER_ECHO );
                  if (false == set_timer(echo_timer_key,echo_timer_seconds,ECHO_TIMER))
                  {
                     strcpy( err_buf, "Unable to set Echo timer." );
                     ret_val = false;
                  }
                  else
                     Echo_Timer_Flag = ECHO_TIMER_IS_SET;
               }
            }
         }
         else
         {
      	   sprintf(err_buf,"Unable to set echo timer. Problem with shared memory.Shutting down the service");
      	   	nceqit_log_message(1, 2, err_buf, "set_echo_timer", 0);
      	  shut_down();
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CLEAR_ECHO_TIMER
 *
 *  DESCRIPTION:  This function checks the echo interval.  If greater than
 *                zero, a timer key is created.  Then the timer is then
 *                cleared.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message in case of failure
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT clear_echo_timer( pCHAR err_buf )
{
   INT   ret_val = true;
   INT   echo_timer_seconds;
   CHAR  echo_timer_key[LOOKUP_KEY_SIZE];

   if ( Echo_Timer_Flag == ECHO_TIMER_IS_SET )
   {
      echo_timer_seconds = atoi( Ncf01_I.echo_interval );
      if ( echo_timer_seconds > 0 )
      {
         memset( echo_timer_key, 0x00, sizeof(echo_timer_key) );
         create_echo_timer_key( echo_timer_key );
         if ( false == clear_timer(echo_timer_key) )
         {
            strcpy( err_buf, "Unable to clear Echo timer." );
            ret_val = false;
         }
         else
            Echo_Timer_Flag = ECHO_TIMER_IS_CLEARED;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CREATE_ECHO_TIMER_KEY
 *
 *  DESCRIPTION:  This function creates a key for an application's echo timer.
 *                It appends the application name to the word ECHO.
 *
 *  INPUTS:       AppName - (Global)Application name
 *
 *  OUTPUTS:      echo_timer_key - String containing the timer key
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_echo_timer_key( pCHAR echo_timer_key )
{
   strcpy( echo_timer_key, "ECHO_" );
   strcat( echo_timer_key, AppName );
   return;
}

/******************************************************************************
 *
 *  NAME:         CREATE_REQUEST_TIMER_KEY
 *
 *  DESCRIPTION:  This function creates a key for a request going to the host.
 *                It uses the application name, terminal Id, and the system
 *                trace audit number.
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
void create_request_timer_key( pCHAR timer_key )
{
   CHAR  tid[9]  = "";
   CHAR  stan[7] = "";

   /* Trim the spaces on the terminal Id. */
   strcpy( tid, Auth_Tx.TLF01_details.terminal_id );
   trim_trailing_spaces( tid );

   /* Use the correct system trace audit number. */
   if ( is_void() )
      strcpy( stan, Auth_Tx.TLF01_details.orig_sys_trace_num );
   else
      strcpy( stan, Auth_Tx.TLF01_details.sys_trace_audit_num );

   strcpy( timer_key, AppName );
   strcat( timer_key, tid     );
   strcat( timer_key, stan    );
   return;
}


void create_request_timer_key_for_Visa( pCHAR timer_key )
{
   CHAR  tran_type[5]  = "";
   CHAR  tid[9]  = "";
   CHAR  stan[7] = "";
   CHAR  appname[3]  = "";

   if (0 == strncmp(AppName, "nceqit", 6))
   {
	   strcpy( appname, "E0" );
   }
   else if (0 == strncmp(AppName, "neqit2", 6))
   {
	   strcpy( appname, "E2" );
   }


   if (((Auth_Tx.TLF01_details.message_type[1] == '2')||
		 (Auth_Tx.TLF01_details.message_type[1] == '1'))&&
		((Auth_Tx.TLF01_details.message_type[2] == '0') ||
		 (Auth_Tx.TLF01_details.message_type[2] == '1')) )
   {
	   strcpy( tran_type, "AUTH" );
   }
   else if ((Auth_Tx.TLF01_details.message_type[1] == '1') &&
		    ((Auth_Tx.TLF01_details.message_type[2] == '2') ||
			(Auth_Tx.TLF01_details.message_type[2] == '3')))
   {
	   strcpy( tran_type, "ADVC" );
   }
   else if ( true == is_reversal() )
   {
	   strcpy( tran_type, "RVSL" );
   }

   /* Trim the spaces on the terminal Id. */
   strcpy( tid, Auth_Tx.TLF01_details.terminal_id );
   trim_trailing_spaces( tid );

   /* Use the correct system trace audit number. */
   if ( is_void() )
      strcpy( stan, Auth_Tx.TLF01_details.orig_sys_trace_num );
   else
      strcpy( stan, Auth_Tx.TLF01_details.sys_trace_audit_num );

   strcpy( timer_key, appname );
   strcat( timer_key, tran_type );
   strcat( timer_key, tid     );
   strcat( timer_key, stan    );
   return;
}

/******************************************************************************
 *
 *  NAME:         CREATE_ECHO_REQ_TIMER_KEY
 *
 *  DESCRIPTION:  This function creates a key for echo requests that are
 *                sent to the host.  It appends the application name to
 *                the word ECHO_REQ.
 *
 *  INPUTS:       AppName - (Global)Application name
 *
 *  OUTPUTS:      echo_req_timer_key - String containing the timer key
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_echo_req_timer_key( pCHAR echo_req_timer_key )
{
   strcpy( echo_req_timer_key, "ECHO_REQ_" );
   strcat( echo_req_timer_key, AppName );
   return;
}

