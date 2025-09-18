/******************************************************************************
 
 	ncamx_tran.c
 
    Copyright (c) 2005, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Amex Acquiring Host Handler Driver
 
 	This module is the host handler for the Amex host.  It
    will process all messages destined to, or originated from,
    the Equitable host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncjcb\ncamx_tran.c  $
   
      Rev 1.8   May 17 2005 18:11:24   dirby
   Modified to process Auth and Cash Overrides.
   SCR 15420
   
      Rev 1.7   Dec 16 2003 15:23:12   dirby
   Modified to not attempt to translate pin for reversals.
   SCR 1177
   
      Rev 1.6   Dec 02 2003 15:24:50   dirby
   Modified to copy the Amex response code into the original
   auth_tx structure.  We previously were converting it to an ISO
   response code and not saving the original value.
   SCR 1200
   
      Rev 1.5   Nov 21 2003 13:18:16   dirby
   Modifications to support Online Updates (1304/1314).
   SCR 1200
   
      Rev 1.4   Oct 07 2003 16:38:12   svelaga
   4.2.0.10: Transaction time out: Automatic reversal to host should not have authorization number ( field 38 ) populated.
   Automatic revesal transaction is also logged into TLF01.
   
   
      Rev 1.3   Oct 03 2003 17:27:58   svelaga
   4.2.0.9: Routing of ATM transactions to Amex ATM Acquiring network controller ( ncatma ).
   
      Rev 1.2   Aug 28 2003 11:18:52   svelaga
   Timerkey is structure changed.
   For refunds, reversals - merchant_id ( first 8 char) are used as
   part of key.
   for rest of txns - terminal_id is used as part of key.
   
      Rev 1.1   Aug 27 2003 09:05:18   svelaga
   Fixed issues with 1804 messages coming from host.
   Also added support for POS Sale, voids.
   
   
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
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"
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
extern CHAR		Amex_Atm_que_name [];

extern CHAR ServiceName[12];


/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern CRF01   Crf01;
extern BDKKEY01	 Bdkkey01;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;
extern INT  Bdk01_Size;

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
      ncamaq_log_message(1, 2, err_buf, "process_host_msg",0 );

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
      ncamaq_log_message( 1, 2, err_buf, "process_host_msg",0 );

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
		  /* This is Amex Acquiring interface. Only 18xx messages are allowed from host.*/
		ret_val = false;      
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
   INT			   ret_val;
   CHAR				err_buf[100]="";
   CHAR				Buffer[200];


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

   if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6) )
   	{
   		if ( (!is_reversal()) && pin_block_exists() && true == verify_host_status(err_buf) ) /*Check 64 in POS message */
   		{
			strcpy( Auth_Tx.TLF01_details.dest_key,        Ncf01_I.cwk      );
			strcpy( Auth_Tx.TLF01_details.dest_pin_format, Ncf01_I.pin_type );
			/* New Function for Decrypt & Encrypt for POS*/
			if (true == ncamaq_is_Online_PIN_transction())
			{
				//check pre conditions for Online PIN
				strncpy( Auth_Tx.TLF01_details.dcf01_retired_cwk, Bdkkey01.bdk_key, sizeof(Auth_Tx.TLF01_details.dcf01_retired_cwk)-1);
				(void)decrypt_pin();
			}
			else
			{
				(void)translate_pin();
			}
		}
   		else
   		{
   			(void)process_request_for_host_after_security();
   		}
   	}

   /* Route ATM transactions to the Amex ATM Acquiring module - ncatma. */
   else if ( ((Auth_Tx.TLF01_details.processing_code[2] == '1') ||
         (Auth_Tx.TLF01_details.processing_code[2] == '2')) ||
        pin_block_exists())
   {
      /* This source code serves as ATM module and POS module.
       * Need to check service name to see which one we are.
       */
      if(strncmp(ServiceName,"ncatma",6) != 0)
      {
         ret_val = build_and_send_request_message( MT_AUTH_REQUEST,
                                                   0,
                                                   0,
                                                   (pBYTE)Amex_Atm_que_name,
                                                   (pBYTE)&Auth_Tx,
                                                   Auth_Tx_Size,
                                                   0,
                                                   NULL_STR,
                                                   err_buf   );
         if ( ret_val == false )
         {
            memset( Buffer, 0x00, sizeof(Buffer) );
            sprintf( Buffer,
                    "Unable to forward txn to Amex ATM Acquiring, %s.  Error = %s",
                     Amex_Atm_que_name, err_buf );
            ncamaq_log_message( 1, 2, Buffer, "process_request_for_host",0 );
         }
      }
      else
      {
         /* This is the ATM module.  Translate pin except for reversals. */
         if ( (!is_reversal()) && pin_block_exists() )
         {
            strcpy( Auth_Tx.TLF01_details.dest_key,        Ncf01_I.cwk      );
            strcpy( Auth_Tx.TLF01_details.dest_pin_format, Ncf01_I.pin_type );
            (void)translate_pin();
         }
         else
            (void)process_request_for_host_after_security();
      }
   }
   else
      (void)process_request_for_host_after_security();

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
   DWORD  txn_amt;
   DWORD  converted_amt;

   /* Process based on transaction type. */
   tx_key = Auth_Tx.TLF01_details.tx_key;
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
                 "Reversal going to AMEX: %s.  Merchant %s",
                  AppName,
                  Auth_Tx.TLF01_details.merchant_id );
         ncamaq_log_message( 3, 2, Buff, "process_request_for_host_after_security",0 );
      case AUTH_VOID_PRE_AUTHORIZATION:
    	  if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
    	  {
    	  /* Display void preauth message on Monitor */
          memset( Buff, 0x00, sizeof(Buff) );
          sprintf( Buff,
                  "Void Preauth going to AMEX: %s.  Merchant %s",
                   AppName,
                   Auth_Tx.TLF01_details.merchant_id );
          ncamaq_log_message( 3, 2, Buff, "process_request_for_host_after_security",0 );
    	  }

      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
	  case AUTH_CASH_ADVANCE_AUTHORIZATION:
      case AUTH_AUTHORIZATION_OVERRIDE:

            if ( Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE)
			{
               Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION;
			   /*TF Praneeth Modified to support tx_key revert back while stip transaction */
			   Auth_Tx.TLF01_details.voice_auth_text[0] = AUTH_AUTHORIZATION_OVERRIDE;
			}
            else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION )/*TF Phani- Added CA AUTH*/
               Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
            else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE )
			{
               Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
			   /*TF Praneeth Modified to support tx_key revert back while stip transaction */
			   Auth_Tx.TLF01_details.voice_auth_text[0] = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE;
			}
            /* Do not break out of switch.  Fall thru to online processing. */


      case AUTH_SALE_CASH: 
      case AUTH_SALE:
      case AUTH_AUTHORIZATION: 
      case AUTH_CASH_ADVANCE: 
      case AUTH_ADVICE:
      case AUTH_REFUND:
      case AUTH_VOID_CASH_ADVANCE:
      case AUTH_VOID_REFUND:
      case AUTH_VOID_SALE:
      case AUTH_QUASI_CASH:	  
      case AUTH_BALANCE_INQUIRY:

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
               txn_amt = atol(Auth_Tx.TLF01_details.total_amount);
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
                  sprintf( Auth_Tx.TLF01_details.total_amount, "%012d",
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
                        ncamaq_log_message( 1, 2, err_buf,
                                    "process_request_for_host_after_security",0 );
                     }
                  }
               }
               else
               {
                  ncamaq_log_message( 1, 2, err_buf,
                              "process_request_for_host_after_security",0 );
               }
            }
          break;

      case AUTH_OFFLINE_SALE:

			  perform_standin_processing( APPROVED, GS_TXN_COMPLETED_OFF, err_buf );
			  /* offline sale just drop here */
			  ret_val = false;
			  break;

      default:

         /* Unknown transaction, reject it. Send response to authorizer. */
         memset( err_buf, 0x00, sizeof(err_buf) );
         sprintf( err_buf,
                 "Invalid txn. Cannot process. TX_KEY = %d",
                  tx_key );
         ncamaq_log_message( 1, 2, err_buf,
                     "process_request_for_host_after_security",0 );
		 /*TF Praneeth revert back tx_key  while stip transaction */
		 if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
		 {
			 Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.voice_auth_text[0];
			 memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
		 }
         ret_val = perform_standin_processing( INVALID_TXN,
                                               GS_TXN_TRANSACTION_NOT_ALLOWED,
                                               err_buf );
         if ( false == ret_val )
            ncamaq_log_message(1,2,err_buf,"process_request_for_host_after_security",0);

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
   CHAR err_buf[100]="";

   /* Check the status of the host.  Are we online? */
   ret_val = verify_host_status( err_buf );
   if ( false == ret_val )
   {
      ncamaq_log_message( 1, 3, err_buf, "process_request_for_host_after_currency" ,1);
	  /*TF Praneeth revert back tx_key  while stip transaction */
	  if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
		  Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.voice_auth_text[0];
		  memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
	  }
      ret_val = perform_standin_processing( HOST_UNAVAILABLE,
                                            GS_TXN_DROPPED,
                                            err_buf );
      if ( false == ret_val )
         ncamaq_log_message(1,2,err_buf,"process_request_for_host_after_currency",1);
   }
   else
   {
	   /*TF Praneeth clearing buffer variable used to store tx_key  while stip transaction */
	   if ((Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (Auth_Tx.TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	   {
		   memset(Auth_Tx.TLF01_details.voice_auth_text,0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text));
	   }
      (void)send_request_to_host( TIMER_FINANCIAL );
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
      ncamaq_log_message(1, 3, err_buf, "process_response_from_host",1 );

   /* Reset number of consecutive timeouts to zero. */
   if ( false == reset_timeout_count( err_buf ) )
      ncamaq_log_message(1, 3, err_buf, "process_response_from_host",1 );

   /* Get tx_key, making sure it is valid. */
   if ( true == determine_tx_key() )
   {
      /* Handle Network Management transactions separately. */
      if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
		 update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, MGT );//praneeth added line with MGT
         process_net_management_from_host();
      }
      else
      {
         /* Update transaction statistics. */
         update_txn_stats( &Txn_Counts, &Auth_Tx, RECEIVED );//praneeth modified NA as RECEIVED

         /* Need to clear the timer; so create the lookup key. */
         create_request_timer_key( lookup_key );

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
            ncamaq_log_message(1, 2, err_buf, "process_response_from_host",1 );
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
   INT			  len = 0;
   
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
         ncamaq_log_message( 1, 3, err_buf, "process_clear_timer_reply",1 );
      }

      /* Verify that the data size returned matches auth_tx size */
      else if ( timer_reply_size != Auth_Tx_Size )
      {
         sprintf( err_buf,
                 "Did not receive auth_tx data with timer reply. Key = %s",
                  lookup_key );
         ncamaq_log_message( 1, 2, err_buf, "process_clear_timer_reply",0 );
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
            ncamaq_log_message( 1, 2, err_buf, "process_clear_timer_reply",0 );
         }
         else if(0 == strcmp(TIMER_REVERSAL, app_data2))
         {
            /* Log the reversal caused due to timeout into TLF01.
             * No need to send response to originator; a timeout response 
             * should already have been sent back. Need to change transaction id
             * to avoid unique constraint violation with the timed out transaction.
             * Increase the last digit by 1.
             */
            len = strlen( Auth_Tx.TLF01_details.primary_key.transaction_id);

            if(Auth_Tx.TLF01_details.primary_key.transaction_id[len - 1] == '9')
               Auth_Tx.TLF01_details.primary_key.transaction_id[len - 1]  = '0';
            else
               Auth_Tx.TLF01_details.primary_key.transaction_id[len - 1]  += 1;

            (void)insert_db_record( TLF01_DATA );
         }
#if AMEX03B_APRIL2024
         else if(Auth_Tx.TLF01_details.saf[0] == PARTIAL_REVERSAL)
         {
			 strncpy(Auth_Tx.TLF01_details.response_text,"Partially reversal txn by ATP",29);
			 insert_db_record(TLF01_DATA);
         }
#endif
         else
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

            if ( Auth_Tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
               (void)process_file_update_response();

            /* Forward the response to the authorizer. */
            else if ( false == send_msg_to_authorizer(MT_AUTH_RESPONSE) )
            {
               sprintf( err_buf,
                    "Unable to complete processing of response msg. MsgType: %s, processing code: %s, stan: %s",
                     Auth_Tx.TLF01_details.message_type,
                     Auth_Tx.TLF01_details.processing_code,
                     Auth_Tx.TLF01_details.sys_trace_audit_num );
               ncamaq_log_message( 1, 3, err_buf, "process_clear_timer_reply",0 );
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
   if ( AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key )
   {
      strcpy( Auth_Tx.TLF01_details.total_amount,
              host_auth_tx->TLF01_details.total_amount);
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

   if ( 0 != strcmp(Auth_Tx.TLF01_details.response_code,ISO_APPROVED) )
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
   if ( Auth_Tx.TLF01_details.visa_char_indicator == 0x00 )
   {
      Auth_Tx.TLF01_details.visa_char_indicator =
                                host_auth_tx->TLF01_details.visa_char_indicator;
   }
   
      strcpy( Auth_Tx.TLF01_details.visa_tran_id,
              host_auth_tx->TLF01_details.visa_tran_id );
   
   if ( Auth_Tx.TLF01_details.visa_validation_code[0] == 0x00 )
   {
      strcpy( Auth_Tx.TLF01_details.visa_validation_code,
              host_auth_tx->TLF01_details.visa_validation_code );
   }

   /* Copy Account Identification information. */
   strcpy( Auth_Tx.acct_id_1, host_auth_tx->acct_id_1 );
   strcpy( Auth_Tx.acct_id_2, host_auth_tx->acct_id_2 );

   /* Amex Response Code - used for Online Updates (1304/1314) */
   strcpy( Auth_Tx.accptor_bus_code, host_auth_tx->accptor_bus_code );
   
   /* TF Phani - For VOICE CA- Trasnctions should go as AUTH-
	During request we have changed to CA and changes back to CA_AUTH		
   */
    if ( Auth_Tx.TLF01_details.voice_auth[0] == 'Y'  && Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
		Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;

	// Copy EMV fields
	memcpy(Auth_Tx.EMV_details.future_field1,host_auth_tx->EMV_details.future_field1,
	sizeof(Auth_Tx.EMV_details.future_field1));
	//Copy action code.
    memcpy(Auth_Tx.TLF01_details.product_codes[12].code,
		   host_auth_tx->TLF01_details.product_codes[12].code,
		   sizeof(host_auth_tx->TLF01_details.product_codes[12].code));

   return;
}
   

