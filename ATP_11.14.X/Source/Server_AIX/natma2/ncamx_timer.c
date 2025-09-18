/***************************************************************************
 *
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCAMX_TIMER.C
 *  
 * TITLE:       Amex Network Controller Timer Processor
 *  
 * DESCRIPTION: This module contains functions that have anything to do
 *              with a timer.  These can be transaction timeouts, echo
 *              timers timing out, or the time out of any other kind of
 *              timer that might be used.  It also includes functions
 *              that set and clear timers.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Sree Velaga
 *

 *
 * $Log:   N:\PVCS\PTE\Equitable\ncjcb\ncamx_timer.c  $
   

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
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"
#include "txutils.h"

#define SME3 1
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
extern CHAR    cadencie_atm_que_name[] ;

CHAR  Log_Bufr[512];

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
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

extern CHAR   ServiceName[12];
extern INT rvrsl_attmpts_value_fr_timeout_amex;
extern INT rvrsl_attmpts_value_fr_laterspns_amex;
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
   CHAR		   err_buf[200];
   CHAR        echo_timer_key[LOOKUP_KEY_SIZE]= "";
   CHAR        echo_req_timer_key[LOOKUP_KEY_SIZE]= "";
   BYTE        temp_tx_key;
   LONG        timer_reply_size = 0;
   INT         repeat_ctr;
   INT         max_repeats;
   #ifdef SME3
   CHAR		   err_buf2[100] 	  = {0};
   CHAR		   masked_cardnbr[30] = {0};
   CHAR		   requestMti[5]	  = {0};
   #endif

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
      (void)issue_1804( ECHO );
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
         natma2_log_message( 1, 2, err_buf, "process_timeout_msg",0 );
	   }
	   else
      {
         /* Get Auth_Tx buffer from timeout message. */
         memcpy( &Auth_Tx, buffer2, Auth_Tx_Size );

         /* Update transaction statistics - update for a timeout response. */
         strcpy( Auth_Tx.TLF01_details.response_code, TO );
         /*check for partial reversal time out sent by ATP*/
#if AMEX03B_APRIL2024
         if(Auth_Tx.TLF01_details.saf[0] == PARTIAL_REVERSAL)
         {
        	 strncpy(Auth_Tx.TLF01_details.response_text,"Partially reversal txn timed out",32);
        	 insert_db_record(TLF01_DATA);
        	 return;
         }
#endif
         update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

         /* Display time out message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants.
          */
         if (true == nposa2_check_for_cadencieacq_transaction())
         {
        	 // dont print error message
         }
         else
         {
         memset( err_buf, 0x00, sizeof(err_buf) );
         sprintf( err_buf,
                 "Txn to Amex Issuer Host(Vertexon) timed out.");
         natma2_log_message( 1, 1, err_buf, "process_timeout_msg" ,1);
         }

         /* Only send timeout response if txn has not been repeated. */
         memset( err_buf, 0x00, sizeof(err_buf) );
         repeat_ctr = atoi( Auth_Tx.advice_repeat_count );
#if BDOR_62_PREAUTH_VOID
         if(strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
         {
        	 /* sending response is not required for parvbh request timeout */
         }
#endif
         else if ( repeat_ctr == 0 )
         {
            /* Do not send response if we generated the reversal
             * due to an 0200 timeout.
             */
            if ( 0 != strcmp(app_data2, TIMER_REVERSAL) )
            {
               temp_tx_key = Auth_Tx.TLF01_details.tx_key;
               if ( false == send_timeout_response(err_buf) )
               {
            	   if(0 ==strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6))
            	   {
							/*Lets log this transaction in ewlog for AMEX Duplicate PROD issue*/

							memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
							sprintf(Log_Bufr,
									"  Advice_Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
									Auth_Tx.TLF01_details.message_type,
									Auth_Tx.TLF01_details.sys_trace_audit_num,
									Auth_Tx.TLF01_details.terminal_id,
									Auth_Tx.TLF01_details.merchant_id,
									Auth_Tx.TLF01_details.retrieval_ref_num);

							natma2_log_message(2, 1, Log_Bufr,
									"process_timeout_msg",
									Auth_Tx.TLF01_details.card_num);
            	   }
               }
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

			 #ifdef SME3
        	 /*BR-RT_3317570-ATPEnhancements_v7, SME-3 changes - Amith*/
        	 mask_card_number(masked_cardnbr, Auth_Tx.TLF01_details.card_num, strlen(Auth_Tx.TLF01_details.card_num));
        	 if(strlen(Auth_Tx.TLF01_details.orig_message) > 0)
        		 memcpy(requestMti, Auth_Tx.TLF01_details.orig_message, 4);
        	 else
        		 memcpy(requestMti, Auth_Tx.TLF01_details.message_type, 4);
        	 sprintf(err_buf2, " MTI: %s STAN: %s RRN: %s CARD: %s",
        			 requestMti,
					 Auth_Tx.TLF01_details.sys_trace_audit_num,
					 Auth_Tx.TLF01_details.retrieval_ref_num,
					 masked_cardnbr);
        	 strcat(err_buf, err_buf2);
        	 //only log the first max timeout msg in Monitor and EW file.
        	 if(GetTimeoutCount() == (MaxConsecTimeouts+1))
        		 natma2_log_message( 3, 2, err_buf, "process_timeout_msg",0 );
        	 #else
        	 natma2_log_message( 1, 1, err_buf, "process_timeout_msg",0 );
			 #endif

             (void)send_network_status_to_monitor();
         }
#if BDOR_62_PREAUTH_VOID
         else if(strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
         {
        	 return;
         }
#endif
         else
         {
            /* Reversals need to be repeated if they time out. */
            if ( is_reversal() )
            {
                if (true == nposa2_check_for_cadencieacq_transaction())
                {
               	 // dont print error message
                }
                else
                {
               /* This is a reversal. Resend it to the host. */

            	   repeat_ctr = atoi( Auth_Tx.advice_repeat_count );

             	   if(rvrsl_attmpts_value_fr_timeout_amex<=0)
             	   {
             		   rvrsl_attmpts_value_fr_timeout_amex=atoi(Ncf01_I.advice_repeat_counter);
             	   }

             	   if(rvrsl_attmpts_value_fr_laterspns_amex <=0)
             	   {
             		   rvrsl_attmpts_value_fr_laterspns_amex=atoi(Ncf01_I.advice_repeat_counter);
             	   }

             	   if(0 == strncmp(Auth_Tx.TLF01_details.response_text,"LR",2))
             	   {
             		  max_repeats=rvrsl_attmpts_value_fr_laterspns_amex;
             	   }
             	   else
             	   {
             		  max_repeats=rvrsl_attmpts_value_fr_timeout_amex;
             	   }

               if ( repeat_ctr < max_repeats )
               {
                  repeat_ctr++;
                  if ( repeat_ctr > 99 )
                     repeat_ctr = 99;
                  sprintf( Auth_Tx.advice_repeat_count, "%02d", repeat_ctr );

                  strcpy( Auth_Tx.TLF01_details.message_type, "1420" );
                  Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL;

                  /* Use TIMER_REVERSAL so response does not go to txcntl. */
						if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch", 6)) {
							/*Lets log this transaction in ewlog for AMEX Duplicate PROD issue*/

							memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
							sprintf(Log_Bufr,
									" Timeout Revrsal Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
									Auth_Tx.TLF01_details.message_type,
									Auth_Tx.TLF01_details.sys_trace_audit_num,
									Auth_Tx.TLF01_details.terminal_id,
									Auth_Tx.TLF01_details.merchant_id,
									Auth_Tx.TLF01_details.retrieval_ref_num);

							natma2_log_message(2, 1, Log_Bufr,
									"build_host_structure",
									Auth_Tx.TLF01_details.card_num);
						}
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
               if ( Auth_Tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY )
               {
            	   if(true==nposa2_check_for_cadencieacq_transaction())
            	   {

            	   }
            	   else
            	   {
            		   repeat_ctr++;
            		   sprintf( Auth_Tx.advice_repeat_count, "%02d", repeat_ctr );
                  (void)transform_0200_into_0420();
                  (void)send_request_to_host( TIMER_REVERSAL );
            	   }
                  if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch", 6))
                  {
							/*Lets log this transaction in ewlog for AMEX Duplicate PROD issue*/

							memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
							sprintf(Log_Bufr,
									" Timeout Revrsal Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
									Auth_Tx.TLF01_details.message_type,
									Auth_Tx.TLF01_details.sys_trace_audit_num,
									Auth_Tx.TLF01_details.terminal_id,
									Auth_Tx.TLF01_details.merchant_id,
									Auth_Tx.TLF01_details.retrieval_ref_num);

							natma2_log_message(2, 1, Log_Bufr,
									"build_host_structure",
									Auth_Tx.TLF01_details.card_num);
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
   if(true==nposa2_check_for_cadencieacq_transaction())
   {

   }
   else
   {
   ret_val = perform_standin_processing( TO, GS_TXN_TIMEDOUT, err_buf );
   if (ret_val == true )
      ret_val = decrement_active_count( err_buf );
   }

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
   Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;

   strcpy( Auth_Tx.TLF01_details.orig_message,
           Auth_Tx.TLF01_details.message_type );

   strcpy( Auth_Tx.TLF01_details.message_type, "0420" );

   strcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
           Auth_Tx.TLF01_details.sys_trace_audit_num );
	
	if(0 == strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num))
	{
		strncpy( Auth_Tx.TLF01_details.orig_retrieval_ref_num,
				Auth_Tx.TLF01_details.retrieval_ref_num,12 );
	}

   strcpy( Auth_Tx.orig_local_time_date,
           Auth_Tx.TLF01_details.transmission_timestamp );

   strcpy( Auth_Tx.orig_acq_inst_id, Auth_Tx.acq_inst_id_cd );

   strcpy( Auth_Tx.TLF01_details.response_code, AMEX_TIMEOUT );


   /* Need to create track2 if not there already. */
   if ( Auth_Tx.TLF01_details.track2[0] == 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.track2, Auth_Tx.TLF01_details.card_num );
      strcat( Auth_Tx.TLF01_details.track2, "="                            );
      strcat( Auth_Tx.TLF01_details.track2, Auth_Tx.TLF01_details.exp_date );
   }

   memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));

   /* Amount dispensed is zero because txn timed out. */
   memset( Auth_Tx.TLF01_details.actual_amount, '0', 12 );
   memset( Auth_Tx.TLF01_details.orig_amount, '0', 12 );

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
      natma2_log_message( 1, 2, err_buf, "set_timer" ,1);

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
      natma2_log_message( 1, 2, err_buf, "clear_timer" ,0);

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
      natma2_log_message( 1, 2, err_buf, "clear_timer" ,0);

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
            strcpy( err_buf,
                   "Unable to set echo timer. Problem with shared memory." );
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
void create_request_timer_key( pCHAR timer_key )
{
   BYTE   tx_key;
   BYTE   error_buffer[100] = {0};
   CHAR   tid[9] = {0};
   CHAR   temp_mid[16]    = {0};
   CHAR   key_last_six[7] = {0};
   INT    mid_len = 0;
   INT    pan_len;

   strcpy( timer_key, AppName );
   strcpy( key_last_six, Auth_Tx.TLF01_details.sys_trace_audit_num );

   tx_key = Auth_Tx.TLF01_details.tx_key;
   switch( tx_key )
   {
      case AUTH_REVERSAL:
      case AUTH_REVERSAL_ADVICE:
      case AUTH_REVERSAL_RESPONSE:
      case AUTH_REVERSAL_ADVICE_RESPONSE:
      case AUTH_VOID_PRE_AUTHORIZATION:
      case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
      case AUTH_ATM_CONFIRM:
      case AUTH_ATM_CONFIRM_RESPONSE:
      {
         /* For reversals, we send out a different stan (field 11) than what
          * might come from the terminal.  So we cannot use it as part of the
          * lookup key.  Instead, use the last 6 digits of the card number.
          * Other txns, like voids, are sent out as 1420, so when they come
          * back, we think they are reversal responses.  So the key creation
          * for them must be the same as for reversals.
          */
         pan_len = strlen( Auth_Tx.TLF01_details.card_num );
         memcpy( key_last_six, &Auth_Tx.TLF01_details.card_num[pan_len - 6], 6);

         if( 0 == strncmp(ServiceName,cadencie_atm_que_name,6) )
         {
            /* For ATM Reversals, use Terminal ID (MID doesn't go to Amex. */
            strcpy( tid, Auth_Tx.TLF01_details.terminal_id );
         }
         else
         {
            /* For POS Reversals, use Merchant ID (TID doesn't go to Amex. */
            strcpy( temp_mid, Auth_Tx.TLF01_details.merchant_id );
            trim_trailing_spaces( temp_mid );

            mid_len = strlen( temp_mid );
            if ( mid_len > 8 )
               strncpy( tid, &temp_mid[mid_len - 8],8);
            else
               strncpy( tid, temp_mid,8);
         }
         break;
      }
      case AUTH_VOID_CASH_ADVANCE:
      case AUTH_VOID_REFUND:
      case AUTH_VOID_SALE:
      case AUTH_VOID_CASH_ADVANCE_RESPONSE:
      case AUTH_VOID_REFUND_RESPONSE:
      case AUTH_VOID_SALE_RESPONSE:

         pan_len = strlen( Auth_Tx.TLF01_details.card_num );
         memcpy( key_last_six, &Auth_Tx.TLF01_details.card_num[pan_len - 6], 6);

         /* Do not 'break' here; continue with the TID portion. */

      case AUTH_ADVICE:
      case AUTH_REFUND:
      case AUTH_ADVICE_RESPONSE:
      case AUTH_REFUND_RESPONSE:
      {
         /* Trim the spaces on the merchant Id. */
         strcpy( temp_mid, Auth_Tx.TLF01_details.merchant_id );
         trim_trailing_spaces( temp_mid );

         mid_len = strlen( temp_mid );
         if ( mid_len > 8 )
            strncpy( tid, &temp_mid[mid_len - 8],8);
         else
            strncpy( tid, temp_mid,8);

         break;
      }

      case AUTH_CARD_UPDATE:
      case AUTH_CARD_UPDATE_RESPONSE:
      {
         /* Online Updates (1304/1314) do not have terminal Ids.
          * Use the last eight digits of the card number.
          */
         pan_len = strlen( Auth_Tx.TLF01_details.card_num );
         memcpy( tid, &Auth_Tx.TLF01_details.card_num[pan_len - 8], 8 );
         break;
      }

      default:
      {
         strcpy( tid, Auth_Tx.TLF01_details.terminal_id );
         break;
      }
   }

   /* Trim the spaces on the terminal Id. */
   trim_trailing_spaces( tid );

   strcat( timer_key, tid );
   strcat( timer_key, key_last_six );

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

