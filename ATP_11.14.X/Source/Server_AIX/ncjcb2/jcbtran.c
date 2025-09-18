/*******************************************************************************
* Copyright (c) 2007, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      jcbtran.c
*  
* TITLE:       
*  
* DESCRIPTION: This module will parse and manipulate all incoming 
*               messages from the terminal and the host for the 
*               JCB host.
*
* APPLICATION: 
*
* AUTHOR:      B. Gable

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

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "jcbmain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "echo_timer.h"
#include "jcb0302.h"
#include "jcbutil.h"
#include "jcbtran.h"

#define SME3 1
#define HCTS4 	1
/**********************************
      FUNCTION PROTOTYPES
**********************************/

/* functions local */

INT    alter_transaction_data(pAUTH_TX, pAUTH_TX);
void   assign_next_stan();
INT    get_term_status(BYTE[]);
INT    verify_host_app_status();
INT    perform_host_logon_request(CHAR[],INT,pAUTH_TX);
INT    perform_op_net_mngmnt_request(CHAR[],pAUTH_TX);
INT    perform_op_net_mngmnt_response(CHAR[],pAUTH_TX);
BYTE   match_request_to_response(BYTE);
void   perform_error_processing(pAUTH_TX);
INT	 get_jcb_data (pAUTH_TX);
void	 calc_julian_day(CHAR[],CHAR[]); 
void   perform_standin_processing(CHAR[], BYTE, pAUTH_TX);
BOOLEAN send_response_to_host_with_rc_success(CHAR rsp_code[3], pAUTH_TX p_auth_tx );

/* global variables */
extern NCF01       ncf01_i;
extern NCF01       ncf01_a;
extern TLF01       tlf01;
extern struct iso_msg_rec HOST_MESSAGE;
extern INT           buflen;                  /* total size of buffer */
extern BYTE ReqRsp_Buffer [COMM_BUF_SZE];
extern CHAR          SHARED_MEMORY_TBL[];

extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/* Database structure sizes */
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;

BYTE   orig_host_start_time[17];
CHAR   trands_que_name[]  = "trandsA";
CHAR   netds_que_name[]   = "netdsA";
CHAR   timerds_que_name[] = "timerdsA";
CHAR   devds_que_name[]   = "devdsA";
CHAR   updatds_que_name[] = "updatdsA";
CHAR   jcb_que_name[] =		"ncjcb3A";
extern dialog_que_name[];

extern CHAR        authorizer_que_name[];
extern CHAR        cur_app_name[];
extern CHAR        text_txds_queue [];
extern CHAR        oprmon_que_name[];
extern CHAR        AppName[];

extern CHAR        Dest_Stat_Id[];
extern CHAR        applnk_que_name[];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* Define direction of transaction flow - switched in or switched out. */
extern INT  Tran_Flow;

/* Constants & variables used for processing of LOGONs */ 
//the next set of variables were modified from char to CHAR.
CHAR vLogon_NMI_Code[5];
CHAR vLogon_Resp_Indicator[6];



CHAR  LOGON_NMI_START[4] =          "001";
CHAR  LOGON_NMI_ECHO_TEST[5] =      "0301";
CHAR  LOGON_NMI_LOGON[5] =          "0001";
CHAR  LOGON_NMI_LOGOFF[5] =         "0002";

CHAR  LOGON_REQUEST[6] =            "LOGON";
CHAR  LOGON_RESPONSE[16] =          "LOGON RESPONSE";
CHAR  ECHO_REQUEST[11] =            "ECHO";
CHAR  ECHO_RESPONSE[20] =           "LOGON ECHO RESPONSE";
CHAR  ECHO_TIMER_KEY[21] =          "LOGON ECHO TIMER KEY";
CHAR  LOGOFF_REQUEST[7] =           "LOGOFF";
CHAR  LOGOFF_RESPONSE[16] =         "LOGOFF RESPONSE";
CHAR  LOGON_RESP_INDICATOR[6] =     "00002";

#define CWK_KEK       1
#define CWK_MFK       2

CHAR  echo_lookup_key[15]        = "ECHO_JCB";
CHAR  echo_request_timer_key[15] = "ECHO_REQ_JCB";

#define SEND_TO_TIMERDS  1
#define SEND_RETURN_MSG  0 

extern WORD  echo_timer_length;

extern JCB_ONLINE_UPDATES  Online_Updates;

extern CHAR  A2E[];    /*  ASCII to EBCDIC conversion */
extern CHAR  E2A[];    /*  EBCDIC to ASCII conversion */


CHAR  ServiceCode[SERVICE_CODE_SIZE_PLUS_1];
extern INT rvrsl_attmpts_value_fr_timeout_jcb;
extern INT rvrsl_attmpts_value_fr_laterspns_jcb;
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
   CHAR	      Buffer[512] = {0};
   CHAR	      ErrorMsg[512] = {0};
   CHAR        sender_que_name[17] = {0};

   strcpy(p_auth_tx->TLF01_details.response_code, resp_code);
   p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);


	   strcpy(sender_que_name, authorizer_que_name);
	//   if (blank_string(p_auth_tx->TLF01_details.last_host_app_queue, strlen(p_auth_tx->TLF01_details.last_host_app_queue)) == false)
	//      strcpy(sender_que_name, p_auth_tx->TLF01_details.last_host_app_queue);
	   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
												  ST1_NONE,
												  ST2_NONE,
												  sender_que_name,
														  application_que_name,
												  (pBYTE)p_auth_tx,
												  Auth_Tx_Size,
												  0 );

   if ( NULL_PTR == p_msg_term_handler_out )
	{
		 memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		 sprintf(Log_Bufr," Insufficient Memory to build response to txcntl Term Id:%s ,Merch Id: %s RRN: %s ",
							p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		 ncjcb2_log_message( 3, 2, Log_Bufr , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
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
      ncjcb2_log_message( 3, 3, Buffer , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      free(p_msg_term_handler_out);
      return(false);
   }
   else
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Response sent successfully to terminal handler Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   free(p_msg_term_handler_out);
return(true);
}


/*****************************************************************************

  Function:    incoming_translate_request 

  Description: This function will bring in an incoming translate request and
               send it to crypto for translation.
  Author: 
      unknown
  Inputs:
      p_msg - a pointer to the incoming message
  Outputs:
      None
  Return values:
      true  - indicates that the message was sent successfully to crypto.
      false - indicates that the message could not be sent successfully to crypto.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT incoming_translate_request(pPTE_MSG p_msg)
{
   //** NOTE:  This function hasn't been tested since remove global auth_tx

/*   AUTH_TX         auth_data_in;
   AUTH_TX         auth_tx;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   pBYTE           p_msg_data_data = NULL_PTR;
   LONG            data_len;
   CHAR            ErrorMsg[512] = {0};
   pPTE_MSG        p_crypto = NULL_PTR;
   LONG            ret_code = 0;
	CHAR		Buffer[512] = {0};


   ncjcb2_log_message( 3, 1, "incoming_translate_request" , "incoming_translate_request" , 0 ,"");

   memset(&auth_data_in, 0x00, Auth_Tx_Size); // clear the structure 
	p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   p_msg_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
	memcpy(&auth_data_in, p_msg_data_data, sizeof(auth_data_in));
   
   p_crypto = crypto_process_request(application_que_name, crypto_que_name, (pBYTE)&auth_data_in.TLF01_details, sizeof(auth_data_in.TLF01_details),
                             (pBYTE)&crypto, sizeof(crypto), auth_data_in.TLF01_details.remote_nsp_type[0], &ret_code);
   if( 0L == ret_code )
   {

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Error received from crypto process Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncjcb2_log_message( 3, 2, Log_Bufr , "incoming_translate_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      strcpy(auth_data_in.TLF01_details.response_code, "58");
	   return (false);
   }
   ret_code = pteipc_send(p_crypto, crypto_que_name);
   if(ret_code < 0)
   {
      pteipc_get_errormsg(ret_code, ErrorMsg);
      sprintf( Buffer, "error on pteipc_send to que %s: %s", crypto_que_name, ErrorMsg );
	 memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	 sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
						p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	 strcat(ErrorMsg,Log_Bufr);
	 ncjcb2_log_message( 3, 3, ErrorMsg , "incoming_translate_request" , 0 ,p_auth_tx->TLF01_details.card_num);

//      strcpy(auth_data_in.TLF01_details.response_code, "58");
      memcpy(&auth_tx, &auth_data_in, Auth_Tx_Size);
//      send_it_back_to_terminal("58");  // this should be done in "main"
      free(p_crypto);
      return(false);
   }
   free(p_crypto); */
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
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   BYTE          temp_tx_key;
   pPTE_MSG      p_msg_remove_auth = NULL_PTR;
   BYTE          temp_resp[3]= {0};
   CHAR          Buffer[512] = {0};

   if ((p_auth_tx->TLF01_details.general_status > GS_NSP_KEYCHANGE_FAILED) &&
       ( p_auth_tx->TLF01_details.general_status <= GS_NSP_OUTOFSERVICE))
   {
      if(send_it_back_to_terminal("58", p_auth_tx ))
         return(true);
      return(false);
   }
	
   /* Populate some fields in auth_tx. */
   (void)get_jcb_data (p_auth_tx);

   /* update the general_status for auth_tx */
   strcpy(p_auth_tx->TLF01_details.acquirer_id, AppName);
   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;

   /************************************************************************
    *
	 * Check if we do standin processing or if we need to go online
    * to the host for this transaction 
    *
    ************************************************************************/
   temp_tx_key = p_auth_tx->TLF01_details.tx_key;
   switch ( temp_tx_key)
   {
#if BDOR_62_PREAUTH_VOID
   	  case AUTH_VOID_PRE_AUTHORIZATION:
      {
         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,"Preauth Cancellation going to Vertexon. ");

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buffer , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
	  break;
#endif
      case AUTH_OFFLINE_SALE: 
      {   /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
         perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
         return(true); 
      }
      case AUTH_REFUND: 
      { /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
         perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
         return(true);    
      }
      case AUTH_OFFLINE_REFUND: 
      { /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
         perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
         return(true);    
      }
      case AUTH_VOID_SALE:
      {  /* Void Sales will be sent to JCB as Reversals. */
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
         strcpy(temp_resp, p_auth_tx->TLF01_details.response_code);
         perform_standin_processing("00", GS_TXN_PERF_REQ, p_auth_tx);

         // DD 3/8/00 sending "17" as response code to host for terminal initiated reversals.
         strcpy(p_auth_tx->TLF01_details.response_code,CANCELLATION);
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE;
         break;
      }
      case AUTH_VOID_REFUND:
      {	/* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE;
         perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);		 
         return(true);    
      }
      case AUTH_SALE_ADJUSTMENT: 
      { /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE;
         perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
         return(true);   
      }
      case AUTH_REVERSAL: 
      { /* Create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
         strcpy(temp_resp, p_auth_tx->TLF01_details.response_code);
#if BDOR_62_PREAUTH_VOID
         if(strncmp(p_auth_tx->TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
		 {
        	 /* Don't respond here for parvbh tool */
		 }
         else if(false == verify_host_app_status(p_auth_tx) &&
        		 (p_auth_tx->TLF01_details.saf[0] == PRIMARY_AUTH ||
        		  p_auth_tx->TLF01_details.saf[0] == INCREMENTAL_AUTH))
         {
        	 /* Don't respond here for reversal void pre-auth tool when host is offline
        	  * handling after switch case */
         }
         else
#endif
         {
        	 perform_standin_processing("00", GS_TXN_PERF_REQ, p_auth_tx);
         }

         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Reversal going to JCB." );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buffer , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num);

         // DD 3/8/00 sending "17" as response code to host for terminal initiated reversals.
         strcpy(p_auth_tx->TLF01_details.response_code,CANCELLATION);
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
			break;
      }
      case AUTH_REFUND_ADJUSTMENT: 
      { /* create transaction type string and store it in the auth_tx_block */
         p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_ADJUSTMENT_RESPONSE;
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
            p_auth_tx->TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;
			/*TF Praneeth Modified to support tx_key revert back while stip transaction */
			p_auth_tx->TLF01_details.voice_auth_text[0] = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE;
            break;
      }
      case AUTH_AUTHORIZATION_OVERRIDE: 
      {
            p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION;
			/*TF Praneeth Modified to support tx_key revert back while stip transaction */
			p_auth_tx->TLF01_details.voice_auth_text[0] = AUTH_AUTHORIZATION_OVERRIDE;
            break;
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

   /***************************************************************************/

   /* confirm host is available */
   if (false == verify_host_app_status(p_auth_tx))
   {
      /* build response_for_terminal */
      strcpy( p_auth_tx->TLF01_details.response_text, "Host Inactive" );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Host Inactive Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num);

      strcpy( p_auth_tx->TLF01_details.response_text, "Host Inactive" );
	  /*TF Praneeth revert back tx_key  while stip transaction */
	  if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
		  p_auth_tx->TLF01_details.tx_key = p_auth_tx->TLF01_details.voice_auth_text[0];
		  memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
	  }
      p_auth_tx->TLF01_details.tx_key =
                    match_request_to_response(p_auth_tx->TLF01_details.tx_key);

      if(p_auth_tx->TLF01_details.tx_key != AUTH_REVERSAL_ADVICE)
         perform_standin_processing("01", GS_TXN_DROPPED, p_auth_tx);

      return(true);
   }
/* Don't validate RPS cards.*/
   else if(false == validate_date_yymm(p_auth_tx->TLF01_details.exp_date,
                                       p_auth_tx->TLF01_details.tx_key,Buffer) && p_auth_tx->MCF01_details.rps_merchant[0] !='Y' )
   {
      /* Build response_for_terminal */
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num);
	  /*TF Praneeth revert back tx_key  while stip transaction */
	  if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
		  p_auth_tx->TLF01_details.tx_key = p_auth_tx->TLF01_details.voice_auth_text[0];
		  memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
	  }
      p_auth_tx->TLF01_details.tx_key =
                    match_request_to_response(p_auth_tx->TLF01_details.tx_key);

      if(p_auth_tx->TLF01_details.tx_key != AUTH_REVERSAL_ADVICE)
         perform_standin_processing(EXPIRED_CARD, GS_TXN_DROPPED, p_auth_tx);

      return(true);
   }
   else
   {
      /* Set this for Logons. */
      memset(vLogon_NMI_Code, 0x00, sizeof(vLogon_NMI_Code));
	  /*TF Praneeth clearing buffer variable used to store tx_key  while stip transaction */
	  if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
		  memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
	  }
   	if (false == send_request_to_host(p_auth_tx) )
			return(false);
   }
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
******************************************************************************/
BOOLEAN process_logon_response(CHAR nmi_code[5], BYTE response_code[3])
{
   pPTE_MSG  p_msg_host_app = NULL_PTR;
   CHAR      ErrorMsg[512] = {0};
   BYTE      net_consec_tmouts_ctr[4];
   BYTE      active_txns_ctr[4];
   BYTE      current_state[2];
   INT       update_flg = false;
   

   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );
   strcpy(net_consec_tmouts_ctr, "0");
   if (strncmp(LOGON_NMI_LOGON,nmi_code,4) == 0)
   {
      ncjcb2_log_message( 3, 1, "Received Log ON response. Setting state online." , "process_logon_response" , 0 ,"");

      if (0 == strcmp("00",response_code))
      {
         strcpy(current_state, ONLINE);
         update_flg = true;

         /* Set the echo timer. */
         (void)set_echo_timer( echo_lookup_key,
                               echo_timer_length,
                   (pPTE_MSG *)NULL_PTR,
                               application_que_name,
                               SEND_TO_TIMERDS );
      }
   }
   else if (strncmp(LOGON_NMI_LOGOFF,nmi_code,4) ==0 )
   {
	   ncjcb2_log_message( 3, 1, "Received Log OFF response. Setting state offline." , "process_logon_response" , 0 ,"");
      if (0 == strcmp("00",response_code))
      {
         strcpy(current_state, OFFLINE);
         update_flg = true;

         /* if host state goes to off-line, then clear echo timer. */
         if(false == clear_echo_timer(echo_lookup_key,(pPTE_MSG *)NULL_PTR,
                                      application_que_name, SEND_TO_TIMERDS ))
         {
            ncjcb2_log_message( 3, 3, "Error clearing the echo_timer" , "process_logon_response" , 0 ,"");
         }
      }
   }
   else if (strncmp(LOGON_NMI_ECHO_TEST,nmi_code,4) == 0)
   {
      /* For ECHOs, if offline, set state Online since we just got a response.*/
      if ( current_state[0] == cOFFLINE )
      {
         current_state[0] = cONLINE;
         update_flg = true;
         ncjcb2_log_message( 3, 3, "Received response to echo. Going back Online" , "process_logon_response" , 0 ,"");
      }

      /* Clear the echo request timer. */
      (void)clear_echo_timer(echo_request_timer_key,(pPTE_MSG *)NULL_PTR,
                             application_que_name, SEND_TO_TIMERDS );
   }

   if ( update_flg == true )
   {
      /* Update host state and counters */
      WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                         active_txns_ctr, current_state );

      send_network_status_to_monitor();
   }
   

   return(true);
}

/*****************************************************************************

  Function:    process_reversal_advice

  Description: This function executes the following steps.
               1.Get the Original TXN from NCF30
               2.Updata the fields in auth_tx with information from the Orig Txn
               3.Returns control
  Author: 
      DURGA
  Inputs:
      p_auth_tx- pointer to auth_tx,this gets updated with NCF30 data
      pCurrent - a pointer to the reply message from the data server

  Outputs:
      None
  Return values:
      true  - always returns true
  Modified by:
      
******************************************************************************/
 BOOLEAN process_reversal_advice(pAUTH_TX p_auth_tx)
{
   pPTE_MSG  p_msg_ins = NULL_PTR;
   NCF30     ncf30;

   strcpy(p_auth_tx->TLF01_details.issuer_id, AppName);
   memset (&ncf30, 0, Ncf30_Size);
   return get_orig_data_from_NCF30(p_auth_tx,&ncf30);
}

/*****************************************************************************

  Function:    send_response_to_host_with_rc_success

  Description: This function will send a response to the host
               with response_code= approved ('00')
  Author: 
      DURGA
  Inputs:
      rsp_code   - the response code for the message
      p_auth_tx  - the auth_tx structure for the message
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
BOOLEAN send_response_to_host_with_rc_success(CHAR rsp_code[3], pAUTH_TX p_auth_tx )
{
   pPTE_MSG	p_msg_insert_auth       = NULL_PTR;
   pPTE_MSG p_msg_delete_auth       = NULL_PTR;
   pPTE_MSG p_msg_host_out	        = NULL_PTR;

   LONG   ret_code;
   CHAR	  Buffer[512]={0};
   CHAR	  ErrorMsg[512]={0};
   CHAR   temp_orig_que[15] = {0};
   BYTE   buffer1[sizeof(AUTH_TX)];
   CHAR   jcb_que_name[] = "ncjcbA";

  /* create response code and store it in the auth_tx block  */
   strcpy(p_auth_tx->TLF01_details.response_code, rsp_code);
   p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);

   /* change status to completed and store it in the auth_tx block  
   p_auth_tx->TLF01_details.general_status = gen_status;
   */

   /* build the message to be sent to the host */
   //strcpy(temp_orig_que, authorizer_que_name);
   
   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1,p_auth_tx, Auth_Tx_Size);
   p_msg_host_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              jcb_que_name,
								                      application_que_name,
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1),
                                              0 );

   ret_code = pteipc_send (p_msg_host_out, jcb_que_name); 
   if ( ret_code < 0 )
   {
 		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s", jcb_que_name, ErrorMsg );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "send_response_to_host_with_rc_success" , 0 ,p_auth_tx->TLF01_details.card_num);
      free(p_msg_host_out);
      return (false);
   }
   else
   {
      ncjcb2_log_message( 1, 1, "Response sent successfully to host" , "send_response_to_host_with_rc_success" , 0 ,"");
   }

   free(p_msg_host_out);
	return (true) ;
} /* send_response_to_host_with_rc_success */


/*****************************************************************************

  Function:    get_orig_data_from_NCF30

  Description: This function gets oringinal data from NCF30
  Author: 
      DURGA
  Inputs:
      pncf30     - the pncf30  structure for the message
      p_auth_tx  - the auth_tx structure for the message
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
BOOLEAN get_orig_data_from_NCF30(pAUTH_TX p_auth_tx,pNCF30 pncf30)
{
   BOOLEAN        retval = true;
   CHAR           unique_tran_str_id[17] = {0};
   CHAR           ErrorMsg[512] = {0};
   CHAR           temp[17] = {0};
   pPTE_MSG       p_msg_ins = NULL_PTR;
   CHAR           Buff[512]={0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   BYTE           subtype2;
   LONG           ret_code;

   ptetime_get_strtime(unique_tran_str_id);

   strcpy(p_auth_tx->TLF01_details.handler_queue, application_que_name);
   strcpy(p_auth_tx->TLF01_details.dispensation.origin,"RO");
   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id, p_auth_tx->TLF01_details.terminal_id, sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
   strncat(p_auth_tx->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);
   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type = FINANCIAL;

   strcpy( pncf30->primary_key.network_id, AppName);
   strcpy( pncf30->primary_key.network_type, "A");
   if (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key) 
   {
      /* Must get the original transaction for a reversal. If the original
       * transaction is not found, reply to the host and don't send the
       * transaction to the authorizer. 
       */
      subtype2 = ST2_DB_SELECT_TRAN_BY_STAN;
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL;
      strcpy(p_auth_tx->TLF01_details.message_type, "0420");
      strcpy( pncf30->card_nbr, p_auth_tx->TLF01_details.card_num );
      strcpy( pncf30->sys_trace_audit_nbr,p_auth_tx->TLF01_details.orig_sys_trace_num );
   }
   else if( AUTH_ADVICE        == p_auth_tx->TLF01_details.tx_key ||
            AUTH_ADVICE_REPEAT == p_auth_tx->TLF01_details.tx_key  )
   {
      /* Must get the original transaction for an auth advice. If the original
       * transaction is not found, reply to the host then send the transaction
       * to the authorizer to be treated as a regular authorization.
       */
      subtype2 = ST2_DB_SELECT_NCF30_BY_RRN_AND_TERMINAL;
      strcpy( pncf30->device_id, p_auth_tx->TLF01_details.terminal_id );
      strcpy( pncf30->primary_key.retrieval_ref_num,
              p_auth_tx->TLF01_details.retrieval_ref_num );
   }

   // Build message to get the original transaction from data server. 
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,pncf30, Ncf30_Size );

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
      ncjcb2_log_message( 3, 2, "Insufficient Memory for select ncf30 message" , "get_orig_data_from_NCF30" , 0 ,"");
      retval = false;
   }
   else
   {
      // Send message to get host specific data from data server. 
      ret_code = pteipc_send( p_msg_ins, netds_que_name );
      if ( 0 > ret_code )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buff,"Failed transmit to queue: %s. %s",
                  netds_que_name,ErrorMsg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buff , "get_orig_data_from_NCF30" , 0 ,p_auth_tx->TLF01_details.card_num);
         memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
         retval = false;
      }
      free( p_msg_ins );
   }

   return( retval );
}
/*****************************************************************************

  Function:    hhutil_transfer_302_msg_to_JCB

  Description: This function will process a message received from the host to JCB.

  Author: 
      Phani TF
  Inputs:
      p_msg  - incomin host message
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
 
******************************************************************************/
BYTE hhutil_transfer_302_msg_to_JCB( pPTE_MSG p_msg )
{
   INT       len;
   BYTE      retval              = true;
   pPTE_MSG  p_msg_out           = NULL_PTR;
   LONG      retCode             = 0L;
   CHAR      strError[512]       = {0};
   BYTE      originator_queue[17]= {0};
   BYTE      originator_info[33] = {0};
   CHAR jcb_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncjcbA" ;
   #define  ORIG_QUEUE  "ORIGQ"
   #define  ORIG_INFO   "ORIGINFO"


   strcpy( originator_queue, ptemsg_get_msg_orig_queue(p_msg) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info(p_msg) );

  // Append originator Q to end of ReqRsp_Buffer: Tag, Length, Data.
   len = strlen( ORIG_QUEUE );
   memcpy( ReqRsp_Buffer+buflen, ORIG_QUEUE, len );
   buflen += len;

   len = strlen( originator_queue );
   ReqRsp_Buffer[buflen++] = len;
   memcpy( ReqRsp_Buffer+buflen, originator_queue, len );
   buflen += len;

   //Append originator Info to end of ReqRsp_Buffer: Tag, Length, Data. 
   len = strlen( ORIG_INFO );
   memcpy( ReqRsp_Buffer+buflen, ORIG_INFO, len );
   buflen += len;

   len = strlen( originator_info );
   ReqRsp_Buffer[buflen++] = len;
   memcpy( ReqRsp_Buffer+buflen, originator_info, len );
   buflen += len;

   p_msg_out = ptemsg_build_msg( MT_INCOMING_GUI,
                                 ST1_NONE,
                                 ST2_NONE,
                                 jcb_host_que_name,
                                 application_que_name,
                                 ReqRsp_Buffer,
                                 buflen,
                                 0 );

   if ( p_msg_out == NULL_PTR )
   {
      sprintf( strError,
              "Insufficient Memory to build ncjcb request message. Size: %d",
               buflen );
      ncjcb2_log_message( 3, 3, strError , "hhutil_transfer_302_msg_to_JCB" , 0 ,"");
      retval = false;
   }
   else
   {
      ptemsg_set_orig_comms_info( p_msg_out, originator_info );
      retCode = pteipc_send( p_msg_out, jcb_host_que_name );
      free (p_msg_out);

      if ( retCode < 0 )
      {
         pteipc_get_errormsg (retCode, strError);
         sprintf( strError,
                 "Error on pteipc_send to que %s: %s",
                  originator_queue, strError );
         ncjcb2_log_message( 3, 3, strError , "hhutil_transfer_302_msg_to_JCB" , 0 ,"");
         retval = false;
      }
   }
   return( retval );
}  /* send_0302_to_network_controller */

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
   pPTE_MSG_DATA     p_msg_data      = NULL_PTR;
   pPTE_MSG          p_msg_timer_out = NULL_PTR;
   pPTE_MSG          p_msg_chain     = NULL_PTR;
   pPTE_MSG			 p_msg_ins   = NULL_PTR;
   LONG				   ret_code;
   CHAR				   Buffer[512]={0};
   CHAR				   ErrorMsg[512]={0};
   CHAR              nmi_code[5];
   CHAR				   lookup_key[22];
   AUTH_TX           auth_tx;
   BYTE              tran_type;
   BYTE              buffer[sizeof(AUTH_TX)];
   CHAR				 error_msg[100];
   INT 				data_len=0;

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   /* clear and set the echo timer  */
   if ( echo_timer_length > 0 )
   {
      (void)clear_echo_timer(echo_lookup_key,(pPTE_MSG *)NULL_PTR,
                             application_que_name, SEND_TO_TIMERDS );

      /* Set the echo timer. */
      if ( false == set_echo_timer(echo_lookup_key, echo_timer_length,
                                  (pPTE_MSG *)NULL_PTR,application_que_name,
                                   SEND_TO_TIMERDS) )
      {
         ncjcb2_log_message( 3, 3, "Error setting the echo_timer" , "incoming_host_message" , 0 ,"");
         return false;
      }
   }

   memset (&auth_tx, 0, Auth_Tx_Size);
   
   /* get the p_msg data from p_msg */
   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   /* copy data to ReqRsp_Buffer */
   copy_to_req_resp_buffer( ptemsg_get_pte_msg_data_data(p_msg_data),
                            ptemsg_get_pte_msg_data_data_length(p_msg_data));
    
   /* Log transaction to trace file. */
   write_to_txn_file();

   /* copy ISO bitfield data field to HOST_MESSAGE.msgtype and associated data */
   if (false == hhutil_parse_host_response_msg(&auth_tx))
   {
	   	return(true);
   }

   if(data_len < COMM_BUF_SZE && data_len > 0 )
   	{
   		sprintf(auth_tx.resp_data_len,"%04d", data_len);
   		memcpy(auth_tx.resp_buffer,ReqRsp_Buffer,data_len);
   	}
   /* put iso data into auth_tx struct */
   if (false == move_iso_response_to_auth_struct(nmi_code, &auth_tx))
   {
	   // Prevents sending response to host.
      // Can't send a response to a request we don't understand.
	   return(true);
   }

   // This may not be correct for sale vs auth, so don't
   // use tran_type to determine between the two.  Later
   // in the code this gets fixed with get_tran_type.
   tran_type = get_tran_type_2(auth_tx.TLF01_details.tx_key);
   auth_tx.TLF01_details.tx_key = tran_type;

   if(AUTH_INVALID == auth_tx.TLF01_details.tx_key)
   {
      /*+-------------------------+
        | Send out error message. |
        +-------------------------+*/
      strcpy( Buffer, "Unsupported transaction from JCB. Message type = ");
      strcat( Buffer,  auth_tx.TLF01_details.message_type );
      strcat( Buffer, ", Processing code = " );
      strcat( Buffer,  auth_tx.TLF01_details.processing_code );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
      memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
	   // Prevents sending response to host.
      // Can't send a response to a request we don't understand.
		return(true);
   }
  
	if(0 == strcmp(auth_tx.TLF01_details.message_type, "0810"))
   {  
      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, &auth_tx, NA );

      process_logon_response(nmi_code, auth_tx.TLF01_details.response_code );
	  /* Send Host Logon Response to Txcntl to Support Stand-In Tx - ThoughtFocus Girija Y */
	   if( strncmp(nmi_code,LOGON_NMI_LOGON,4) == 0)
	   {
		   if ( 0 == strncmp( auth_tx.TLF01_details.response_code, APPROVED, 2 ) )
		   {
				strcpy(auth_tx.TLF01_details.originator_queue,application_que_name);
				p_msg_ins = ptemsg_build_msg( MT_LOGON_RESPONSE,
												  ST1_NONE, 
												  ST2_NONE,
												  application_que_name, 
												  authorizer_que_name,
												  (pBYTE)&auth_tx,
												  sizeof(AUTH_TX), 
												  0 );
				if ( NULL_PTR == p_msg_ins )
				{
					strcpy( error_msg, "Insufficient memory to build request message" ); 
					return( false );
				}
				ret_code = pteipc_send (p_msg_ins, authorizer_que_name);  
			   if ( ret_code < 0 )
			   {
 				  pteipc_get_errormsg( ret_code, ErrorMsg );
				  sprintf( Buffer, "pteipc error sending to que %s: %s", authorizer_que_name, ErrorMsg );
				  ncjcb2_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,"");
				  memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
				  free(p_msg_ins);
				  return(false);
			   }
			   free(p_msg_ins);
		   }
	   }

      return(true);
   }
  	else if(0 == strcmp(auth_tx.TLF01_details.message_type, "0800"))
   {  
      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, &auth_tx, NA );

      perform_host_logon_request(nmi_code, SOURCE_IS_HOST, &auth_tx);  
      return(true);
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, &auth_tx, NA );

   /* Determine if Switched IN or Switched OUT. */
   if((tran_type >= AUTH_SALE) && (tran_type <= AUTH_INVALID))
   {
      Tran_Flow = SWITCHED_IN;
      strcpy(auth_tx.TLF01_details.originator_queue, ptemsg_get_msg_orig_queue ( p_msg ));
      strcpy(auth_tx.TLF01_details.originator_info, ptemsg_get_msg_orig_comms_info (p_msg));
      ptetime_get_strtime(auth_tx.TLF01_details.tran_start_time);
      strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_a.tpdu_id, sizeof(ncf01_a.tpdu_id));
   }
   else
   {
      Tran_Flow = SWITCHED_OUT;
      ptetime_get_strtime(auth_tx.TLF01_details.host_finish_time);
      strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id, sizeof(ncf01_i.tpdu_id));
   }

   /*
    * Compare for reversal advices/repeats(0420-0421)
    */
   if(0 == strncmp(auth_tx.TLF01_details.message_type, "042",3))
   {  
      return (process_reversal_advice(&auth_tx));
   }

   /*
    * Compare for auth advices/repeats(0120-0121)
    */
   if(0 == strncmp(auth_tx.TLF01_details.message_type, "012",3))
   {  
      return (process_reversal_advice(&auth_tx));
   }

   if (AUTH_ADMIN_ADVICE == auth_tx.TLF01_details.tx_key)
   {
      sprintf(Buffer,"Advice Message Received");
      ncjcb2_log_message( 3, 1, Buffer , "incoming_host_message" , 0 ,"");
      auth_tx.TLF01_details.tx_key = match_request_to_response(auth_tx.TLF01_details.tx_key);
      if (incoming_other_host_response(&auth_tx) == false)
         return(false);
      return(true);
   }

   if( Tran_Flow == SWITCHED_IN )
   {
      strcpy(auth_tx.TLF01_details.issuer_id, AppName);
      strcpy(auth_tx.TLF01_details.source_key, ncf01_a.cwk);
      strcpy(auth_tx.TLF01_details.source_pin_format, ncf01_a.pin_type);
      if(hhutil_process_host_request(&auth_tx))
         return(true);
      return(false);
   }

   memset (lookup_key, 0, sizeof(lookup_key));
   create_timer_lookup_key( &auth_tx, lookup_key );
   
   /* build PTEMSG to get Timer Data from the Host Data Server */ 
   p_msg_timer_out = ptetimer_bld_read_timer_req( lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   { 
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build read timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }
   
   memset(buffer, 0, sizeof(buffer));
   memcpy(buffer,&auth_tx, Auth_Tx_Size);
   p_msg_chain = ptemsg_chain_new_message (p_msg_timer_out,
                                           MT_DB_REQUEST,
                                           ST1_DB_ECHO,
                                           ST2_NONE,
                                           timerds_que_name,
                                           application_que_name,
                                           buffer,
                                           sizeof(buffer),
                                           0);
                                           

   /* get Timer message from Host Data Server now  */
   ret_code = pteipc_send( p_msg_chain, timerds_que_name);
   free( p_msg_chain);
   if(ret_code < 0)
   {   /* failed getting timer data */
       pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "pteipc error sending to que %s: %s", netds_que_name, ErrorMsg );
       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
       					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
       strcat(Buffer,Log_Bufr);
       ncjcb2_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
       memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
       return(false);
   }
   return(true);
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
   CHAR			Buffer[512] = {0};
   CHAR			ErrorMsg[512] = {0};
   LONG			ret_code;
   LONG        data_len;
   pPTE_MSG    p_msg_ins = NULL_PTR;
   AUTH_TX     auth_tx;
   AUTH_TX     temp_auth_tx;
   BYTE        tran_type;


   timer_reply_size = ptetimer_get_timer_reply(p_msg_in, lookup_key, app_data1, app_data2, buffer, sizeof(buffer));
   if (timer_reply_size == sizeof(buffer))
   {
      memset(&temp_auth_tx, 0, Auth_Tx_Size);
      memcpy(&temp_auth_tx, buffer, Auth_Tx_Size);
   }
   else
   {
      sprintf( Buffer,
              "Did not receive auth_tx data with timer reply. Key = %s",
               lookup_key );
      memcpy( temp_auth_tx.TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }

   p_echo_msg  = ptemsg_get_chained_pte_msg (p_msg_in,2);
   p_msg_data  = ptemsg_get_pte_msg_data (p_echo_msg);
   p_data      = ptemsg_get_pte_msg_data_data (p_msg_data);
   data_len    = ptemsg_get_pte_msg_data_data_length (p_msg_data);

   if (data_len == sizeof(buffer))
   {
       memset(&auth_tx, 0, Auth_Tx_Size);
       memcpy(&auth_tx, p_data, Auth_Tx_Size);
   }
   else
   {
      sprintf( Buffer,
              "Did not receive chained host response data with timer reply. Key = %s",
               lookup_key );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);

      memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
      return(false);
   }

   /* build PTEMSG to get Timer Data from the Host Data Server */ 
   p_msg_timer_out = ptetimer_bld_clear_timer_req( lookup_key,timerds_que_name,
                                                  application_que_name );
   if ( NULL_PTR == p_msg_timer_out )
   { 
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr, "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }
   
   /* get Timer message from Host Data Server now  */
   ret_code = pteipc_send( p_msg_timer_out, timerds_que_name);
   
   if(ret_code < 0)
   {   /* failed getting timer data */
       pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "pteipc error sending to que %s: %s", timerds_que_name, ErrorMsg );
       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
       					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
       ncjcb2_log_message( 3, 3, Buffer , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
       memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
       return(false);
   }
   free( p_msg_timer_out);

   tran_type = match_request_to_response(temp_auth_tx.TLF01_details.tx_key);
   temp_auth_tx.TLF01_details.tx_key = tran_type;


  if (((strncmp(temp_auth_tx.TLF01_details.handler_queue,"sbatch",6)!= 0) &&
       (strncmp(temp_auth_tx.TLF01_details.handler_queue,"ncjcb",5) != 0) &&
	   (strncmp(temp_auth_tx.TLF01_details.handler_queue,parvbh_que_name,7)!= 0)) &&
	   (AUTH_REVERSAL_ADVICE_RESPONSE == tran_type 					   ||
	    AUTH_REVERSAL_REPEAT_RESPONSE == tran_type 					   ||
	    AUTH_VOID_SALE_RESPONSE       == tran_type)  )
   {
      // Already responded.
      return(true);
   }

   /* Copy response data into tmp_auth_tx.
    * auth_tx = response; temp_auth_tx = orig trans
    */
   (void)alter_transaction_data(&auth_tx, &temp_auth_tx);

   strcpy(temp_auth_tx.TLF01_details.dispensation.auth_2,"RA");
   if ( 0 == strcmp(temp_auth_tx.TLF01_details.voice_auth, "Y"))
   {
      hhutil_get_response_text(&temp_auth_tx);
   }

   if(strncmp(temp_auth_tx.TLF01_details.handler_queue,"sbatch",6) == 0 )
   {
	   strcpy( temp_auth_tx.TLF01_details.dispensation.auth_2, "RA" );

	   if ( true != send_response_to_sbatch( &temp_auth_tx,MT_AUTH_RESPONSE,
               application_que_name, Buffer ))
	   {
		  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		  sprintf(Log_Bufr," Unable to send message to sbatch Msg_Type: %s , Term Id:%s ,Merch Id: %s RRN: %s ",
				  temp_auth_tx.TLF01_details.message_type,temp_auth_tx.TLF01_details.terminal_id,
				  temp_auth_tx.TLF01_details.merchant_id,temp_auth_tx.TLF01_details.retrieval_ref_num);
		  memcpy( temp_auth_tx.TLF01_details.response_text, "Unable to send message Sbatch",
		                sizeof(temp_auth_tx.TLF01_details.response_text)-1 );
		  ncjcb2_log_message( 1, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,temp_auth_tx.TLF01_details.card_num);
		  return( false );
	   }
   }
#if BDOR_62_PREAUTH_VOID
   else if(strncmp(temp_auth_tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0 )
   {
	   strcpy( temp_auth_tx.TLF01_details.dispensation.auth_2, "RA" );

	   if ( true != send_response_to_parvbh( &temp_auth_tx,MT_AUTH_RESPONSE,
               application_que_name, Buffer ))
	   {
		  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		  sprintf(Log_Bufr," Unable to send message to parvbh Msg_Type: %s , Term Id:%s ,Merch Id: %s RRN: %s ",
				  temp_auth_tx.TLF01_details.message_type,temp_auth_tx.TLF01_details.terminal_id,
				  temp_auth_tx.TLF01_details.merchant_id,temp_auth_tx.TLF01_details.retrieval_ref_num);
		  memcpy( temp_auth_tx.TLF01_details.response_text, "Unable to send message parvbh",
		                sizeof(temp_auth_tx.TLF01_details.response_text)-1 );
		  ncjcb2_log_message( 1, 3, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,temp_auth_tx.TLF01_details.card_num);
		  return( false );
	   }
   }
#endif
   else if ( temp_auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
      (void)process_file_update_response( &temp_auth_tx );

   else if (false == send_response_to_term_app(&temp_auth_tx))
   {
      sprintf( Buffer,
              "Unable to complete processing of response msg. MsgType: %s, processing code: %s, stan: %s",
               auth_tx.TLF01_details.message_type,
               auth_tx.TLF01_details.processing_code,
               auth_tx.TLF01_details.sys_trace_audit_num );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to send message to authorizer Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      return(false);
   }

   PRINT("Processing host response: end \n");

   return(true);

} /* incoming_host_message_timer_reply */


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
   CHAR        lookup_key[MAX_TIMER_KEY_SIZE];
   CHAR        app_data1[MAX_APP_DATA_SIZE];
   CHAR        app_data2[MAX_APP_DATA_SIZE];
   CHAR        tempstr[10];
   CHAR	       Buffer[256] = "";
   CHAR		   ErrorMsg[100] = "";
   CHAR        unique_tran_str_id[17] = "";
   CHAR        gmt_time[20];
   AUTH_TX     auth_tx;
   BYTE        buffer2[sizeof(AUTH_TX)];
   BYTE        to_rc[3] = "";
   BYTE        net_consec_tmouts_ctr[4]="";
   BYTE        active_txns_ctr[4]="";
   BYTE        current_state[2]="";
   LONG        timer_reply_size = 0;
   INT         tempint;
   INT         count;
   INT         update_flag = false;
   INT         max_repeats = 0;
   #ifdef SME3
   CHAR		   masked_cardnbr[30] = {0};
   CHAR		   requestMti[5]	  = {0};
   #endif

   /* get timeout values */
   timer_reply_size = ptetimer_get_timer_reply(p_msg, lookup_key, app_data1,
                                             app_data2,buffer2,sizeof(buffer2));

   if(strcmp(lookup_key,echo_lookup_key) == 0)
   {
      /* 1. Echo timer is automatically cleared when it expires.
       * 2. Set echo timer
       * 3. Issue echo request to host
       */

      /* Set the echo timer. */
      if ( false == set_echo_timer(echo_lookup_key, echo_timer_length,
                                   (pPTE_MSG *)NULL_PTR,application_que_name,
                                   SEND_TO_TIMERDS) )
      {
         ncjcb2_log_message( 3, 3, "Error setting the echo_timer" , "process_timeout_msg" , 0 ,"");
      }

      memset (&auth_tx, 0, Auth_Tx_Size);
      if (false == perform_host_logon_request(ECHO_REQUEST,
                                              SOURCE_IS_OPERATOR, &auth_tx))
      {
         ncjcb2_log_message( 3, 1, "Error executing host Echo Test" , "process_timeout_msg" , 0 ,"");
      }
      return( true );
   }
 	else if(strcmp(lookup_key,echo_request_timer_key) == 0)
   {
      /* An Echo request has timed out. Take no action here  */
      /* Allow the echoes to continue going out as normal.   */
      /* --------------------------------------------------- */

      return true;
   }
   else
   {
      if (timer_reply_size == sizeof(buffer2))
      {
         memset(&auth_tx, 0, Auth_Tx_Size);
         memcpy(&auth_tx, buffer2, Auth_Tx_Size);

         /* Display time out message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants.
          */
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Txn to JCB Issuer Host(Vertexon) timed out." );

         memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 1, Buffer , "process_timeout_msg" , 0 ,auth_tx.TLF01_details.card_num);
      }
      else
      {
         sprintf( ErrorMsg,
                 "Error retrieving auth_tx from timer message. Lookup key = %s",
                  lookup_key );
         memcpy( auth_tx.TLF01_details.response_text, ErrorMsg, 40 );
         ncjcb2_log_message( 3, 2, ErrorMsg , "process_timeout_msg" , 0 ,"");
         return(false);
      }
   }

   /* update the transaction in the auth_tx table to TIMED OUT */
   auth_tx.TLF01_details.general_status = GS_TXN_TIMEDOUT;
   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id, sizeof(ncf01_i.tpdu_id));

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

   /* Increment the number of times the host has failed */
   if ( MaxConsecTimeouts > 0 )
   {
      if (atoi(net_consec_tmouts_ctr) >= MaxConsecTimeouts)
      {
         strcpy(current_state,OFFLINE); 
         update_flag = true;

         memcpy( auth_tx.TLF01_details.response_text,
                "Maximum consecutive timeouts reached.", 40 );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		 #ifndef SME3
         sprintf(Log_Bufr," Maximum consecutive timeouts reached. Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
		 ncjcb2_log_message( 3, 2, Log_Bufr , "process_timeout_msg" , 0 ,auth_tx.TLF01_details.card_num);
		 #else
    	 /*BR-RT_3317570-ATPEnhancements_v7, SME-3 changes - Amith*/
    	 mask_card_number(masked_cardnbr, auth_tx.TLF01_details.card_num, strlen(auth_tx.TLF01_details.card_num));
    	 if(strlen(auth_tx.TLF01_details.orig_message) > 0)
    		 memcpy(requestMti, auth_tx.TLF01_details.orig_message, 4);
    	 else
    		 memcpy(requestMti, auth_tx.TLF01_details.message_type, 4);
    	 sprintf(Log_Bufr,
    			 " Maximum consecutive timeouts reached."
    			 " MTI: %s STAN: %s RRN: %s CARD: %s",
				 requestMti,
				 auth_tx.TLF01_details.sys_trace_audit_num,
				 auth_tx.TLF01_details.retrieval_ref_num,
				 masked_cardnbr);
    	 if(atoi(net_consec_tmouts_ctr) == MaxConsecTimeouts)
    		 ncjcb2_log_message( 3, 2, Log_Bufr , "process_timeout_msg" , 0 ,"");
         #endif
      }

   }

   /* Increment the number of consecutive time outs. */
   memset( tempstr, 0x00, sizeof(tempstr) );
   tempint = atoi(net_consec_tmouts_ctr) + 1;
   if ( tempint > 999 )
      tempint = 1;
   itoa( tempint, tempstr, 10 );
   strcpy( net_consec_tmouts_ctr, tempstr );

   //multiple timeout prompts due to shm update delay, adding sleep
   if(tempint == (MaxConsecTimeouts+1))
   {
	   /* Update counters */
	   WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr,
                   	      active_txns_ctr, current_state );
	   sleep(2);
   }
   else
   {
	   WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr,
                   	   	  active_txns_ctr, current_state );
   }

   /* Update Monitor's display. */
   if ( update_flag == true )
      (void)send_network_status_to_monitor();

   /* Update transaction statistics. */
   strcpy( to_rc, auth_tx.TLF01_details.response_code );
   strcpy( auth_tx.TLF01_details.response_code, TIMED_OUT );
   update_txn_stats( &Txn_Counts, &auth_tx, NA );
   strcpy( auth_tx.TLF01_details.response_code, to_rc );

#if BDOR_62_PREAUTH_VOID
   if(strncmp(auth_tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
   {
	   /* sending timeout response is not required for parvbh tool request */
	   return (true);
   }
#endif

   if ( (AUTH_REVERSAL_ADVICE == auth_tx.TLF01_details.tx_key) || 
        (AUTH_REVERSAL_REPEAT == auth_tx.TLF01_details.tx_key) ||
        (AUTH_VOID_SALE       == auth_tx.TLF01_details.tx_key)
#if BDOR_62_PREAUTH_VOID
	 || (AUTH_VOID_PRE_AUTHORIZATION == auth_tx.TLF01_details.tx_key)
#endif
      )
   {
      if ( update_flag == false )
      {
         /* This means we have not reached max timeouts yet.
          * Continue sending reversals.
          */
    	  count = atoi( auth_tx.advice_repeat_count );

   	   if(rvrsl_attmpts_value_fr_timeout_jcb<=0)
   	   {
   		   rvrsl_attmpts_value_fr_timeout_jcb=atoi(ncf01_i.advice_repeat_counter);
   	   }

   	   if(rvrsl_attmpts_value_fr_laterspns_jcb <=0)
   	   {
   		   rvrsl_attmpts_value_fr_laterspns_jcb=atoi(ncf01_i.advice_repeat_counter);
   	   }

   	   if(0 == strncmp(auth_tx.TLF01_details.response_text,"LR",2))
	   {
		  max_repeats=rvrsl_attmpts_value_fr_laterspns_jcb;
	   }
	   else
	   {
		  max_repeats=rvrsl_attmpts_value_fr_timeout_jcb;
	   }

   	   if ( count < max_repeats )
         {
            /* Increment the advice count */
            count++;
            itoa(count, auth_tx.advice_repeat_count, 10);

            /* Send a reversal repeat message to the host */
            auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT;
            strcpy(auth_tx.TLF01_details.message_type, "0421");
            auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
            strcpy( auth_tx.TLF01_details.response_code, LATE_RESPONSE );
            send_request_to_host(&auth_tx);
            if (0 == strncmp(auth_tx.TLF01_details.handler_queue,"sbatch", 6))
                              {
					/*Lets log this transaction in ewlog for MC Duplicate PROD issue*/

					memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
					sprintf(Log_Bufr,
							" Timeout Revrsal Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
							auth_tx.TLF01_details.message_type,
							auth_tx.TLF01_details.sys_trace_audit_num,
							auth_tx.TLF01_details.terminal_id,
							auth_tx.TLF01_details.merchant_id,
							auth_tx.TLF01_details.retrieval_ref_num);

					ncjcb2_log_message(2, 1, Log_Bufr, "process_timeout_msg",
							0, auth_tx.TLF01_details.card_num);
				}
         }
      }
   }
   else
   {
      if ( auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE )
      {
         strcpy( auth_tx.TLF01_details.response_code, TIMED_OUT );
         auth_tx.TLF01_details.general_status = GS_TXN_TIMEDOUT;
         auth_tx.TLF01_details.tx_key = AUTH_CARD_UPDATE_RESPONSE;

         memset( &Online_Updates, 0x00, sizeof(JCB_ONLINE_UPDATES) );
         strcpy( ErrorMsg, "Request to host timed out." );
         issue_0302_response( HOST_UNAVAILABLE, &auth_tx, ErrorMsg );
      }
      else if (AUTH_LOGON != auth_tx.TLF01_details.tx_key)
      {
         /* Prepare time out message for terminal handler. */
          if(auth_tx.TLF01_details.tx_key == '\0')
          {
          	 auth_tx.TLF01_details.tx_key = get_tran_type( &auth_tx );
          }

         /* Send time out message to terminal handler. */
         perform_standin_processing( TIMED_OUT, GS_TXN_TIMEDOUT, &auth_tx);

         if ( update_flag == false )
         {
            /* This means we have not reached max timeouts yet.
             * Send repeating reversals.
             */

            // Set reversal counter to one
            itoa(1, auth_tx.advice_repeat_count, 10);

            // Send a reversal message to the host 
            auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
            auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;

            strcpy(auth_tx.orig_local_time_date,
                   auth_tx.TLF01_details.transmission_timestamp);

            strcpy(auth_tx.TLF01_details.orig_sys_trace_num,
                   auth_tx.TLF01_details.sys_trace_audit_num);

            strcpy(auth_tx.TLF01_details.orig_message, "0100" );
            strcpy(auth_tx.TLF01_details.message_type, "0420");
            strcpy(auth_tx.TLF01_details.response_code,LATE_RESPONSE );

            // Indicates that entire amount is reversed
            strcpy(auth_tx.TLF01_details.actual_amount,"000000000000" );

            // Indicates that entire amount is reversed
            strcpy(auth_tx.TLF01_details.reversal_amount,
                   auth_tx.TLF01_details.total_amount );

            ptetime_get_strtime(unique_tran_str_id);
            memset(auth_tx.TLF01_details.sys_trace_audit_num, 0,
                   sizeof(auth_tx.TLF01_details.sys_trace_audit_num));

            /* Create System Trace Audit Number. */
            get_stan( auth_tx.TLF01_details.sys_trace_audit_num );

            pintime_get_gmt( gmt_time );  // YYYYMMDDhhmmss 
            send_request_to_host(&auth_tx); 
            if (0== strncmp(auth_tx.TLF01_details.handler_queue,"sbatch", 6)) {
					/*Lets log this transaction in ewlog for MC Duplicate PROD issue*/

					memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
					sprintf(Log_Bufr,
							" Timeout Revrsal Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
							auth_tx.TLF01_details.message_type,
							auth_tx.TLF01_details.sys_trace_audit_num,
							auth_tx.TLF01_details.terminal_id,
							auth_tx.TLF01_details.merchant_id,
							auth_tx.TLF01_details.retrieval_ref_num);

					ncjcb2_log_message(2, 1, Log_Bufr, "process_timeout_msg",
							0, auth_tx.TLF01_details.card_num);
				}
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
   CHAR	      Buffer[512]={0};
   CHAR	      ErrorMsg[512]={0};
   INT         tempint;
   CHAR        tempstr[10];
   CHAR        temp_orig_que[15] = {0};
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

   /* build the message to be sent to the terminal handler */
   strcpy(temp_orig_que, authorizer_que_name);

//   if (blank_string(p_auth_tx->TLF01_details.last_host_app_queue, strlen(p_auth_tx->TLF01_details.last_host_app_queue)) == false)
//      strcpy(temp_orig_que, p_auth_tx->TLF01_details.last_host_app_queue);
   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1, p_auth_tx, Auth_Tx_Size);

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
      ncjcb2_log_message( 3, 2, Buffer , "send_response_to_term_app" , 0 ,p_auth_tx->TLF01_details.card_num);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      free(p_msg_term_handler_out);
      return(false);
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
INT alter_transaction_data(pAUTH_TX p_auth_tx, pAUTH_TX p_temp_auth_tx)
{
   INT   len;   /* Length of EMV block in response message. */

   if ( AUTH_INVALID == p_temp_auth_tx->TLF01_details.tx_key )
   {
      /* this transaction is not understood by the server and will be dropped */
      /* fsvisa code says to set terminal status to GS_STATUS_IDLE */
	   return (false);
   }   
   
   if(strncmp(p_auth_tx->TLF01_details.processing_code,"30",2) == 0 &&
	  strncmp(p_auth_tx->TLF01_details.message_type,"0110",4)  == 0)
   {
	   if(strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"dcp",3) == 0)
	   {
		   p_temp_auth_tx->TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_RESPONSE;
	   }
	   else
	   {
		   p_temp_auth_tx->TLF01_details.tx_key = AUTH_BALANCE_INQUIRY_RESPONSE ;
	   }
   }
   if ( AUTH_BALANCE_INQUIRY_RESPONSE == p_temp_auth_tx->TLF01_details.tx_key )
   {   /* replace the amount in the auth_tx table with the amount in the response */
	   strcpy(p_temp_auth_tx->TLF01_details.total_amount, p_auth_tx->TLF01_details.total_amount);
   }
   
   // TF Phani .. Copy original Transaction amount,currncy code and nfiseq_nbr
   /* This is only for Swithced out ..coming from JCB Host ..req by Kenneth*/
   if(0 == strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"ncjcbA",5))
   {
	 /* Note:  need to copy nfi_seq_nbr ..Req By Kenneth,,16th OCt 2008*/
	  memcpy (p_temp_auth_tx->TLF01_details.nfi_seq_nbr, p_auth_tx->TLF01_details.nfi_seq_nbr,
           sizeof(p_temp_auth_tx->TLF01_details.nfi_seq_nbr));

	 memcpy(p_temp_auth_tx->TLF01_details.currency_code,p_temp_auth_tx->reconciliation_cur_code, sizeof(p_temp_auth_tx->TLF01_details.currency_code));
	 memcpy(p_temp_auth_tx->TLF01_details.total_amount,p_temp_auth_tx->reconciliation_amount, sizeof(p_temp_auth_tx->TLF01_details.total_amount));
   }

   p_temp_auth_tx->TLF01_details.general_status = GS_TXN_COMPLETED;
   strcpy(p_temp_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx->TLF01_details.retrieval_ref_num);

   if ( p_temp_auth_tx->TLF01_details.tx_key != AUTH_VOID_SALE_RESPONSE )
      strcpy(p_temp_auth_tx->TLF01_details.sys_trace_audit_num, p_auth_tx->TLF01_details.sys_trace_audit_num);

   strcpy(p_temp_auth_tx->TLF01_details.settlement_date, p_auth_tx->TLF01_details.settlement_date);

   if (p_auth_tx->TLF01_details.auth_number [0] != 0x00)
       strcpy(p_temp_auth_tx->TLF01_details.auth_number, p_auth_tx->TLF01_details.auth_number );

   if(strlen(p_auth_tx->TLF01_details.response_code)> 0)
   {
	   strcpy(p_temp_auth_tx->TLF01_details.response_code, p_auth_tx->TLF01_details.response_code);
   }

   if (p_temp_auth_tx->TLF01_details.date_yyyymmdd [0] == 0x00)
      strcpy(p_temp_auth_tx->TLF01_details.date_yyyymmdd, p_auth_tx->TLF01_details.date_yyyymmdd);

   if (p_auth_tx->TLF01_details.time_hhmmss [0] != 0x00)
      strcpy(p_temp_auth_tx->TLF01_details.time_hhmmss, p_auth_tx->TLF01_details.time_hhmmss);

	strcpy(p_temp_auth_tx->TLF01_details.resp_source_len, p_auth_tx->TLF01_details.resp_source_len);
	strcpy(p_temp_auth_tx->TLF01_details.resp_source, p_auth_tx->TLF01_details.resp_source);
	p_temp_auth_tx->TLF01_details.cvc = p_auth_tx->TLF01_details.cvc;
	strcpy(p_temp_auth_tx->TLF01_details.mcard_banknet_len, p_auth_tx->TLF01_details.mcard_banknet_len);
	strcpy(p_temp_auth_tx->TLF01_details.mcard_banknet, p_auth_tx->TLF01_details.mcard_banknet);
	strcpy(p_temp_auth_tx->TLF01_details.agent_id_len, p_auth_tx->TLF01_details.agent_id_len);
	strcpy(p_temp_auth_tx->TLF01_details.agent_id, p_auth_tx->TLF01_details.agent_id);
	strcpy(p_temp_auth_tx->TLF01_details.host_finish_time, p_auth_tx->TLF01_details.host_finish_time);

   if ( p_temp_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      if ( p_auth_tx->TLF01_details.processing_code[0] == '5' )
      {
         /* This is an inquiry.  Copy over the response information. */

         /* Action Code */
         strcpy( p_temp_auth_tx->TLF01_details.type_of_data,
                 p_auth_tx->TLF01_details.type_of_data );

         /* Purge Date */
         strcpy( p_temp_auth_tx->TLF01_details.product_codes[0].amount,
                 p_auth_tx->TLF01_details.product_codes[0].amount );

         /* Stop Regions */
         strcpy( p_temp_auth_tx->TLF01_details.product_codes[1].amount,
                 p_auth_tx->TLF01_details.product_codes[1].amount );
      }
   }

   if ( (p_auth_tx->EMV_details.emv_block[0] != 0x00) ||
        (p_auth_tx->EMV_details.emv_block[1] != 0x00)  )
   {
      /*+---------------------------------------------------------------+
        | Copy the response EMV data from host into the 'future' fields |
        | of the EMV structure.  There are 3 of them.  Bytes 1 and 2 of |
        | future field1 will hold the total length.                     |
        +---------------------------------------------------------------+*/

      /* Get length of EMV response from host. */
	   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcbA",5))
		   len = p_auth_tx->EMV_details.emv_block[1];
	   else
		   len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );

      memcpy( p_temp_auth_tx->EMV_details.future_field1,
              p_auth_tx->EMV_details.emv_block,
              sizeof(p_temp_auth_tx->EMV_details.future_field1)-1 );

      if ( len > (sizeof(p_temp_auth_tx->EMV_details.future_field1)-3) )
      {
         memcpy( p_temp_auth_tx->EMV_details.future_field2,
                 p_auth_tx->EMV_details.emv_block+2
                 + sizeof(p_temp_auth_tx->EMV_details.future_field1)-3,
                 sizeof(p_temp_auth_tx->EMV_details.future_field2)-1 );

         if ( len > ((sizeof(p_temp_auth_tx->EMV_details.future_field1)-3)
                    +(sizeof(p_temp_auth_tx->EMV_details.future_field2)-1)) )
         {
            memcpy( p_temp_auth_tx->EMV_details.future_field3,
                    p_auth_tx->EMV_details.emv_block+2
                    + sizeof(p_temp_auth_tx->EMV_details.future_field1)-3
                    + sizeof(p_temp_auth_tx->EMV_details.future_field2)-1,
                    sizeof(p_temp_auth_tx->EMV_details.future_field3)-1 );
         }
      }
   }

   // TF Phani . Copy original msg len and msg
	if(atoi(p_auth_tx->resp_data_len) > 0 && atoi(p_auth_tx->resp_data_len) <  COMM_BUF_SZE )
	{
		memcpy(p_temp_auth_tx->resp_data_len,p_auth_tx->resp_data_len,sizeof(p_auth_tx->resp_data_len));
		memcpy(p_temp_auth_tx->resp_buffer,p_auth_tx->resp_buffer,sizeof(p_auth_tx->resp_buffer));
	}
	/* DE54 received form host, copy it, need to send it to IST in 0210 response */
   if(p_auth_tx->future_use_1[0] != '\0')
   {
		  memcpy( p_temp_auth_tx->future_use_1, p_auth_tx->future_use_1, p_auth_tx->future_use_1[0]+1 );
   }
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
   pPTE_MSG	p_msg_set_timer     = NULL_PTR;
   pPTE_MSG p_msg_term_hdlr     = NULL_PTR;
   CHAR     lookup_key[22];
   LONG     ret_code;
   CHAR     ErrorMsg[512] = {0};
   CHAR     Buffer[512] = {0};
   BYTE     buffer1[sizeof(AUTH_TX)];
   CHAR     orig_que_name[17]   = {0};
   WORD     timer_length;
   pPTE_MSG p_msg_host_out      = NULL_PTR;
   CHAR     gmt_time[20]={0};
   INT 		buf_temp_len=0;
   /* build PTEMSG for AUTH_TX table */
   if (GS_TXN_NEW == p_auth_tx->TLF01_details.general_status)
      p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;

   /* Current Greenwich Mean Time for the transmission-to-host time. */
   ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);

   if ( 0 == strcmp( NULL_STR,
                     p_auth_tx->TLF01_details.transmission_timestamp ) )
   {
      get_gmt_time( p_auth_tx->TLF01_details.transmission_timestamp );
   }

   build_host_structure(p_auth_tx);
   if (Build_Host_Request( p_auth_tx ))
   {
	  /* Don't send the p_auth_tx->req_data_len to timerds,
	   * If length sent during TO , original sale will be sent instead of TO reversal during process time out*/
	  buf_temp_len = atoi(p_auth_tx->req_data_len);
	  if(buf_temp_len > 0)
	  {
		//do nothing
	  }
	  else
	  {
		buf_temp_len = 0;
	  }

	  memset(p_auth_tx->req_data_len,0x00,sizeof(p_auth_tx->req_data_len));
#ifdef HCTS4
   //don't set timer for sbatch request
   if(strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatch", 6) != 0 )
   {
#endif
      memset(lookup_key, 0x00, sizeof(lookup_key));
      create_timer_lookup_key( p_auth_tx, lookup_key );
      memcpy(buffer1,p_auth_tx, Auth_Tx_Size);

      /* Build Start Timer message to send to Host Data Server */		
      timer_length = atoi(ncf01_i.request_timer);
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
          sprintf(Log_Bufr," Insufficient Memory to build timer message Term Id:%s ,Merch Id: %s RRN: %s ",
          					p_auth_tx->TLF01_details.terminal_id,
          					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
          ncjcb2_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
          return(false);
      }    

      /* We can chain the request_timer message with set &
       * clear echo timer request to timerds
       */
      if(false == chain_echo_host(echo_lookup_key, echo_timer_length,
                                  application_que_name,p_msg_set_timer))
      {
         ncjcb2_log_message( 3, 3, "Error chaining the timer request with clear & set the echo_timer" , "send_request_to_host" , 0 ,"");
      }
#ifdef HCTS4
   }
#endif

      /* Forward Msg coming from Host */
	 if(strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0 &&
		0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "ncjcb", 5))
	{
		 /*In the beginig befor sending to TIMERDS ,buf_temp_len is copied and p_auth_tx->req_data_len
		  * made null to avoid the Multiple sale issue during TO.*/
		//buf_temp_len = atoi(p_auth_tx->req_data_len);
		if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
		{	buflen = buf_temp_len;
			memcpy(ReqRsp_Buffer,p_auth_tx->req_buffer,buflen);
		}
	  /* 1. After sending the msg to cadencie, clear the
		 * original msg present in the auth_tx Req_buffer,as it is not used after sending it to host.
		 * */
		memset(p_auth_tx->req_data_len,0x00,sizeof(p_auth_tx->req_data_len));
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
         sprintf( Buffer,
                 "Unable to send %s msg to host. Err: Insufficient memory",
                  p_auth_tx->TLF01_details.message_type );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncjcb2_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
         return(false);
      }

      ret_code = pteipc_send(p_msg_host_out, "dialogA");
      if( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer, "pteipc error sending to que %s: %s",
                 "dialogA", ErrorMsg );
         memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 3, Buffer , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
         free( p_msg_host_out );
         return(false);
      }
      else
      {
         /* Update transaction statistics. */
         update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

         /* Log transaction to trace file. */
         write_to_txn_file();
      }
      free( p_msg_host_out );

   }
   else
      return(false);
   return(true);
}
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
      true  - if host is online, else false
   Modified by:
      BGable      6/30/98
******************************************************************************/
INT  verify_host_app_status(pAUTH_TX p_auth_tx)
{
   INT   tempint;
   CHAR  tempstr[14];
   CHAR  err_buf[512]={0};
   BYTE  net_consec_tmouts_ctr[4];
   BYTE  active_txns_ctr[4];
   BYTE  current_state[2];
   INT   ret_code = true;


   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state );

   if ( current_state[0] != cONLINE )
      ret_code = false;

   /* Check number of consecutive time outs.
    * If too many, issue a warning.
    */
   if ( (MaxConsecTimeouts > 0) && (ret_code == true) )
   {
      if (atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts )
      {
         strcpy( err_buf,
             "Maximum consecutive timeouts reached.");

         strcpy(current_state,OFFLINE); 
         ret_code = false;

         /* Update host counters */
         WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                         active_txns_ctr, current_state );

         /* Update Monitor's display. */
         (void)send_network_status_to_monitor();

         memcpy( p_auth_tx->TLF01_details.response_text, err_buf, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(err_buf,Log_Bufr);
         ncjcb2_log_message( 3, 2, err_buf , "verify_host_app_status" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   /* Check number of active transactions.
    * If too many, issue a warning.
    */
   if ( ret_code == true )
   {
      tempint = atoi(active_txns_ctr) + 1;
      if ( MaxActiveTxns > 0 )
      {
         if( tempint >= MaxActiveTxns )
         {
            strcpy( err_buf,
                   "Maximum active txns reached. Continuing to process." );

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(err_buf,Log_Bufr);
            ncjcb2_log_message( 3, 2, err_buf , "verify_host_app_status" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      /* Increment the number of active transactions for this host */
      if (p_auth_tx->TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE)
      {
         if ( tempint > 999 )
            tempint = 1;
         memset(&tempstr, 0x00, sizeof(tempstr));
         itoa(tempint,tempstr,10);
         strcpy(active_txns_ctr, tempstr ); 
         WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                            active_txns_ctr, current_state );
      }
   }

   return( ret_code );
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
   char tempstr[10] = {0};
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];

   memset( &vLogon_Resp_Indicator, 0x00, sizeof(vLogon_Resp_Indicator));
	//strcpy(temp_auth_tx.TLF01_details.processing_code, "920000");
	/****************
	this increment was already done in the verify_host_app_status functions 
	****************/
   /* increment the active transactions counter field
   tempint = atoi(active_txns_ctr) + 1;
   itoa(tempint, tempstr, 10);
   strcpy(active_txns_ctr,tempstr); */

	   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   switch (source)
	{
		case SOURCE_IS_FSVISA:
		{
			if (2 == atoi(current_state))
			{
				ncjcb2_log_message( 3, 1, "Sending logon request to host:" , "perform_host_logon_request" , 0 ,"");
				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
				//strcpy(vLogon_NMI_Code, LOGON_NMI_START);
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
				ncjcb2_log_message( 1, 1, "Sending logon request to host:" , "perform_host_logon_request" , 0 ,"");
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
			if (0 == strncmp(LOGON_NMI_LOGON, request_type, 4))
			{
				ncjcb2_log_message( 1, 1, "Received Log ON request from host.", "perform_host_logon_request" , 0 ,"");
 				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				strcpy(vLogon_NMI_Code, LOGON_NMI_START);
				strcpy(p_auth_tx->TLF01_details.response_code, "00");
				perform_op_net_mngmnt_response(request_type, p_auth_tx);
				strcpy( current_state, ONLINE );
			}
			else if (0 == strncmp(LOGON_NMI_LOGOFF, request_type, 4))
			{
				ncjcb2_log_message( 1, 1, "Received Log OFF request from host.", "perform_host_logon_request" , 0 ,"");
 				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				strcpy(vLogon_NMI_Code, LOGON_NMI_LOGOFF);
				strcpy(p_auth_tx->TLF01_details.response_code, "00");
				perform_op_net_mngmnt_response(request_type, p_auth_tx);
				strcpy( current_state, OFFLINE );
			}
			else if (0 == strncmp(LOGON_NMI_ECHO_TEST, request_type, 2))
			{
				//ncjcb2_log_message( 1, 1, "Received network request from host:", "perform_host_logon_request" , 0 ,"");
				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
				strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);
				strcpy(vLogon_Resp_Indicator, LOGON_RESP_INDICATOR);
				strcpy(p_auth_tx->TLF01_details.response_code, "00");
				perform_op_net_mngmnt_response(request_type, p_auth_tx);
			}
			WriteGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr,
                     active_txns_ctr, current_state );
         (void)send_network_status_to_monitor();
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
   CHAR        Buff[512] = {0};
   CHAR ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   XINT ReturnCode = 0 ;
   int nIndex = 0 ;
 

	   /* Get host state and counters */
   ReadGenericTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                     active_txns_ctr, current_state );

   memset( &ncf01, 0, Ncf01_Size );
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
      ncjcb2_log_message( 2, 2, "Insufficient Memory to create network query message", "send_network_status_to_monitor" , 0 ,"");
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
         sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
         ncjcb2_log_message( 3, 3, Buff , "send_network_status_to_monitor" , 0 ,"");
         free( p_msg_ins );
         return( false );
      }
   }
   free( p_msg_ins );

#ifdef WIN32
   Sleep (500);
#else
   usleep (500000);
#endif
   memset( &ncf01, 0, Ncf01_Size );
   strcpy( ncf01.primary_key.network_id,    AppName      );
   strcpy( ncf01.primary_key.network_type, "I"           );
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
      ncjcb2_log_message( 3, 3, "Insufficient Memory to create network query message" , "send_network_status_to_monitor" , 0 ,"");
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
         sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
         ncjcb2_log_message( 3, 3, Buff, "send_network_status_to_monitor" , 0 ,"");
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
 ******************************************************************************/
INT perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx )
{
   CHAR     gmt_time[15];
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   WORD     timer_length;

   strncpy(p_auth_tx->TLF01_details.tpdu_id,
           ncf01_i.tpdu_id, sizeof(ncf01_i.tpdu_id));
   strncpy(p_auth_tx->forwarding_inst_id_cd, ncf01_i.forwarding_inst_id_code,
           sizeof(ncf01_i.forwarding_inst_id_code));
   strncpy(p_auth_tx->receiving_inst_id_code, ncf01_i.receiving_inst_id_code,
           sizeof(ncf01_i.receiving_inst_id_code));

   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;

   /* Current Time for the transmission-to-host time (YYYYMMDDhhmmss) */
   ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);

   /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }

   /* Use the time hhmmss for sys_trace_audit_num */
   memset(gmt_time, 0, sizeof(gmt_time));
   ptetime_get_strtime (gmt_time);
   Rj_with_lead_zeros(gmt_time, sizeof(gmt_time)-1);
   memcpy (p_auth_tx->TLF01_details.sys_trace_audit_num, gmt_time + 7, 3); 
   strncat(p_auth_tx->TLF01_details.sys_trace_audit_num, gmt_time + 11,3);

   /* Get host state and counters */

   if (0 == strcmp(request_type, LOGON_REQUEST))
   {
      ncjcb2_log_message( 3, 1, "Sending LOG ON request to host." , "perform_op_net_mngmnt_request" , 0 ,"");
      strcpy(vLogon_NMI_Code, LOGON_NMI_LOGON);
   }
   else if (0 == strcmp(request_type, LOGOFF_REQUEST))
   {
       ncjcb2_log_message( 3, 1, "Sending LOG OFF request to host." , "perform_op_net_mngmnt_request" , 0 ,"");
       strcpy(vLogon_NMI_Code, LOGON_NMI_LOGOFF);
   }
   else 
   {
      strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);

      /* Set Request Timer for Echoes. */

      /* Build Start Timer message to send to Host Data Server */		
      timer_length = atoi(ncf01_i.request_timer);
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
         sprintf(Log_Bufr," Insufficient Memory to build start timer message Term Id:%s ,Merch Id: %s RRN: %s echo_request_timer_key : %s",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num,echo_request_timer_key);
         ncjcb2_log_message( 3, 2, Log_Bufr , "perform_op_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);

         return(false);
      }

      /* We can chain the request_timer message with set & clear
       * echo timer request to timerds 
       */
      if(false == chain_echo_host( echo_lookup_key, echo_timer_length,
                                   application_que_name, p_msg_set_timer) )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Error chaining the timer request with clear & set the echo_timer Term Id:%s ,Merch Id: %s RRN: %s echo_request_timer_key : %s",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num,echo_request_timer_key);
         ncjcb2_log_message( 3, 2, Log_Bufr , "perform_op_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   if (false == hhutil_send_net_mngmnt_request(p_auth_tx))
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
INT perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx)
{

   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01_a.tpdu_id);

   strcpy(p_auth_tx->TLF01_details.sys_trace_audit_num,
          p_auth_tx->TLF01_details.sys_trace_audit_num); 
   strcpy(p_auth_tx->TLF01_details.retrieval_ref_num,
          p_auth_tx->TLF01_details.retrieval_ref_num);

   if (0 == strncmp(nmi_code, LOGON_NMI_LOGON, 4))
   {
      ncjcb2_log_message( 1, 1, "Sending Log ON response to host." , "perform_op_net_mngmnt_response" , 0 ,"");
      strcpy(vLogon_Resp_Indicator, LOGON_RESP_INDICATOR);  
   }
   else if (0 == strncmp(nmi_code, LOGON_NMI_LOGOFF, 4))
   {
      ncjcb2_log_message( 1, 1, "Sending Log OFF response to host." , "perform_op_net_mngmnt_response" , 0 ,"");
   }

   strncpy(vLogon_NMI_Code, nmi_code, 4);

   if (false == hhutil_send_net_mngmnt_response(p_auth_tx))
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
      case AUTH_SALE:
	     return(AUTH_SALE_RESPONSE); 
        break;

      case AUTH_REFUND:
	     return(AUTH_REFUND_RESPONSE);  
        break;

      case AUTH_CASH_ADVANCE:
	     return(AUTH_CASH_ADVANCE_RESPONSE); 
        break;


      case AUTH_PAYMENT: 
	     return(AUTH_PAYMENT_RESPONSE); 
        break;


      case AUTH_ADVICE: 
	     return(AUTH_ADVICE_RESPONSE); 
        break;

      case AUTH_ADMIN_ADVICE: 
	     return(AUTH_ADMIN_ADVICE_RESPONSE); 
        break;

      case AUTH_ADVICE_REPEAT: 
	     return(AUTH_ADVICE_REPEAT_RESPONSE); 
        break;


      case AUTH_OFFLINE_SALE: 
	     return(AUTH_OFFLINE_SALE_RESPONSE); 
        break;


      case AUTH_OFFLINE_REFUND:
	     return(AUTH_OFFLINE_REFUND_RESPONSE); 
        break;


      case AUTH_SALE_ADJUSTMENT:
	     return(AUTH_SALE_ADJUSTMENT_RESPONSE); 
        break;


      case AUTH_REFUND_ADJUSTMENT:
	     return(AUTH_REFUND_ADJUSTMENT_RESPONSE); 
        break;


	  case AUTH_PAYMENT_ADJUSTMENT:
	     return(AUTH_PAYMENT_ADJUSTMENT_RESPONSE); 
        break;


      case AUTH_PRE_AUTHORIZATION:
	     return(AUTH_PRE_AUTHORIZATION_RESPONSE); 
        break;

#if BDOR_62_PREAUTH_VOID
      case AUTH_VOID_PRE_AUTHORIZATION:
    	 return(AUTH_VOID_PRE_AUTHORIZATION_RESPONSE);
    	break;
#endif

      case AUTH_AUTHORIZATION:
	     return(AUTH_AUTHORIZATION_RESPONSE); 
        break;

      case AUTH_BALANCE_INQUIRY:
	     return(AUTH_BALANCE_INQUIRY_RESPONSE); 
        break;

      case AUTH_VOID_SALE:
	     return(AUTH_VOID_SALE_RESPONSE); 
        break;

      case AUTH_VOID_REFUND: 
	     return(AUTH_VOID_REFUND_RESPONSE);
        break;

      case AUTH_REVERSAL:
	     return(AUTH_REVERSAL_RESPONSE); 
        break;

      case AUTH_REVERSAL_ADVICE:
	     return(AUTH_REVERSAL_ADVICE_RESPONSE); 
        break;


      case AUTH_REVERSAL_REPEAT:
	     return(AUTH_REVERSAL_REPEAT_RESPONSE); 
        break;

      case AUTH_LOGON:
	     return(AUTH_LOGON_RESPONSE); 
        break;

      case AUTH_TEST:
	     return(AUTH_TEST_RESPONSE); 
        break;

      case AUTH_CARD_VERIFICATION:
        return(AUTH_CARD_VERIFICATION_RESPONSE);
        break;

      case AUTH_QUASI_CASH:
        return(AUTH_QUASI_CASH_RESPONSE);
        break;

      case AUTH_CARD_UPDATE:
        return(AUTH_CARD_UPDATE_RESPONSE);
        break;

      default:
        return( request+100 );
        break;

   }
}; /* match_request_to_response */


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

   LONG        ret_code;
   CHAR	      Buffer[512]={0};
   CHAR	      ErrorMsg[512]={0};
   BYTE        buffer1[sizeof(AUTH_TX)];

   if ( AUTH_REVERSAL_RESPONSE != p_auth_tx->TLF01_details.tx_key )
   {
      if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.auth_number ) )
      {
         strcpy(p_auth_tx->TLF01_details.auth_number,
                p_auth_tx->TLF01_details.time_hhmmss);
      }
   }

   /* create response code and store it in the auth_tx block  */
   strcpy(p_auth_tx->TLF01_details.response_code, rsp_code);
   
   if(strcmp(rsp_code,"00")== 0 )
   { if(false == verify_host_app_status(p_auth_tx))
		strcpy(p_auth_tx->TLF01_details.response_text, "APPROVED_STAND_IN");
	else
		strcpy(p_auth_tx->TLF01_details.response_text, "APPROVED");
   }
   p_auth_tx->TLF01_details.tx_key = match_request_to_response(p_auth_tx->TLF01_details.tx_key);

   /* change status to completed and store it in the auth_tx block */ 
   p_auth_tx->TLF01_details.general_status = gen_status;

   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1,p_auth_tx, Auth_Tx_Size);

		p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
												  ST1_NONE,
												  ST2_NONE,
												  authorizer_que_name,
														  application_que_name,
												  (pBYTE)&buffer1,
												  sizeof(buffer1),
												  0 );


   if ( NULL_PTR == p_msg_term_handler_out  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to forward txn to authorizer. Err: Insufficient Memory to build PTE message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "perform_standin_processing" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      ret_code = pteipc_send (p_msg_term_handler_out, authorizer_que_name); 
      if ( ret_code < 0 )
      {
 		   pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer, "pteipc error sending to que %s: %s",
                  authorizer_que_name, ErrorMsg );

         memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 3, Buffer , "perform_standin_processing" , 0 ,p_auth_tx->TLF01_details.card_num);

         sprintf( Buffer,
                 "Unable to forward txn to authorizer, %s.",
                  authorizer_que_name );

         memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 3, Buffer , "perform_standin_processing" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
      free(p_msg_term_handler_out);
   }

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
   char       date_str [ 9] = {0};
   char       time_str [ 7] = {0};
   char       time_date[25] = {0};
   char       resp_code[3] = "58";
   char       unique_tran_str_id[17] = {0};


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


   /* Create date of transaction and store it in the auth_tx table,
   * format "YYYY0M0D".
   */
   if (p_auth_tx_in->TLF01_details.date_yyyymmdd [0] == '\0')
      strcpy( p_auth_tx_in->TLF01_details.date_yyyymmdd, date_str );

   /* Create time of transaction and store it in the auth_tx 
   table,"0H240I0S" */
   if (p_auth_tx_in->TLF01_details.time_hhmmss [0] == '\0')
      strcpy( p_auth_tx_in->TLF01_details.time_hhmmss, time_str );

   if (p_auth_tx_in->TLF01_details.primary_key.transaction_id [0] == '\0')
   {
      ptetime_get_strtime(unique_tran_str_id);
      memset(p_auth_tx_in->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx_in->TLF01_details.primary_key.transaction_id));
      memcpy(p_auth_tx_in->TLF01_details.primary_key.transaction_id, p_auth_tx_in->TLF01_details.terminal_id, sizeof(p_auth_tx_in->TLF01_details.terminal_id) - 1);
      strncat(p_auth_tx_in->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);
   }

   p_auth_tx_in->TLF01_details.general_status = GS_TXN_DROPPED;
     
}; /* perform_error_processing */


/*****************************************************************************

  Function:    get_jcb_data

  Description: This function will put the jcb specific data into the auth_tx
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
INT get_jcb_data (pAUTH_TX p_auth_tx)
{
   CHAR temp_str1[15];
   CHAR temp_str2[12];
   CHAR time_date[27];

   strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id);

	/* Get LOCAL Date and Time */
   ptetime_get_timestamp(time_date);

	if (0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd))
   {
      memset(&temp_str1, 0x00, sizeof(temp_str1));
      memset(&temp_str2, 0x00, sizeof(temp_str2));
      strncpy(temp_str1, time_date, 4);
      strncpy(temp_str2, (time_date + 5), 2);
      strcat (temp_str1, temp_str2);
	   strncpy(temp_str2, (time_date + 8), 2);
	   strcat (temp_str1, temp_str2);
	   strcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp_str1);
   }
   
	if (0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.time_hhmmss))
   {
      memset(&temp_str1, 0x00, sizeof(temp_str1));
      memset(&temp_str2, 0x00, sizeof(temp_str2));
      strncpy(temp_str1, (time_date + 11), 2);
      strncpy(temp_str2, (time_date + 14), 2);
      strcat (temp_str1, temp_str2);
	   strncpy(temp_str2, (time_date + 17), 2);
	   strcat (temp_str1, temp_str2);
   	  if ( p_auth_tx->TLF01_details.time_hhmmss[0] == 0x00 )
	   strcpy(p_auth_tx->TLF01_details.time_hhmmss, temp_str1);   
   }

   /* Get Retrieval Reference Number => YDDDnnnnnnnn */
   if ( AUTH_REVERSAL != p_auth_tx->TLF01_details.tx_key )
   {
      /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
      if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
      {
         create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
      }
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
   #define  FIELD48_TAG87      "87"
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

      if ( 0 == strcmp( tag, FIELD48_TAG87 ) )
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
 *  NAME:         FIND_CAV2
 *
 *  DESCRIPTION:  This function will parse field 48 of an 0100 or 0120
 *                message from the JCB host.  It will return the CAV2
 *                value (if it is present).  CAV2 is like Visa's CVV2 and
 *                MasterCard's CVC2.
 *
 *                The tags and length bytes are in EBCDIC.  So they must
 *                be converted into ASCII.
 *
 *  FIELD 48 FORMAT:
 *            -------+---------+---------+---------+---------+-----
 *             Total | First   | First   | First   | Second  |
 *              Len  |SubField |SubField |SubField |SubField | . . .
 *                   |  Tag    | Length  |  Data   |  Tag    |
 *            -------+---------+---------+---------+---------+-----
 *      BYTES:   1        2         2         x         2
 *
 *
 *  INPUTS:       src_len - Length of src
 *                src     - Field 48
 *
 *  OUTPUTS:      dest - The CAV2 value, or null
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void find_cav2( BYTE src_len, pCHAR src,  pCHAR dest )
{
   INT  sub_field_len;
   CHAR tag[3];
   CHAR len[3];
   CHAR ascii_tag[3] = "";
   CHAR ascii_len[3] = "";
   INT  MainIndex;
   INT  TotalLen;

   TotalLen = src_len;

   dest[0] = 0x00;
   for( MainIndex=0; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      memset( tag, 0x00, sizeof( tag ) );
      memcpy( tag, &src[MainIndex], 2 );
      ascii_tag[0] = ascii_ebcdic_conv( tag[0], E2A );
      ascii_tag[1] = ascii_ebcdic_conv( tag[1], E2A );
      MainIndex += 2;

      memset( len, 0x00, sizeof(len));
      memcpy( len, &src[MainIndex], 2 );
      ascii_len[0] = ascii_ebcdic_conv( len[0], E2A );
      ascii_len[1] = ascii_ebcdic_conv( len[1], E2A );

      sub_field_len = atoi( ascii_len );
      MainIndex += 2;

      if ( 0 == strcmp( ascii_tag, CAV2_TAG_01 ) )
      {
         /* Found it.  Convert from EBCDIC to ASCII. */
         dest[0] = ascii_ebcdic_conv( src[MainIndex  ], E2A );
         dest[1] = ascii_ebcdic_conv( src[MainIndex+1], E2A );
         dest[2] = ascii_ebcdic_conv( src[MainIndex+2], E2A );
         break;
      }
      else
      {
         MainIndex += sub_field_len;
      }
   }
   return;
}


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
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   LONG            data_len;
   LONG            auth_len;
   AUTH_TX         auth_tx;

   memset(&auth_tx, 0x00, Auth_Tx_Size);

   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   auth_len = data_len;
   if(Auth_Tx_Size < data_len)
      auth_len = Auth_Tx_Size;
   p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
   memcpy(&auth_tx, p_data_data, auth_len);
   p_data_data = p_data_data + Auth_Tx_Size;

/* Store originator_info coming from Applnk for sending back response to stpmgr - ThoughtFocus Girija Y*/
   if( 0 == strncmp(auth_tx.TLF01_details.originator_queue,"stpmgr",6))
   {
	   strcpy( auth_tx.TLF01_details.originator_info,
              ptemsg_get_msg_orig_comms_info (p_msg)) ;
   }

   /* No need to continue for authorization advices.
    * They have already been answered.
    */
   if ( (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE) ||
        (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE) )
   {
      //Send a msg to updateds to restore the balances.
      update_database(&auth_tx );
      return( true );
   }

   if(!(auth_tx.TLF01_details.tx_key == AUTH_LOGON_RESPONSE))
   {
      if((auth_tx.TLF01_details.tx_key < AUTH_SALE) ||
         (auth_tx.TLF01_details.tx_key > AUTH_INVALID))
      {
         Tran_Flow = SWITCHED_IN;
         strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_a.tpdu_id,
                 sizeof(ncf01_a.tpdu_id));

         auth_tx.TLF01_details.tx_key =
                        match_request_to_response(auth_tx.TLF01_details.tx_key);

         if((0 == strcmp(auth_tx.TLF01_details.response_code, "00")) ||
            (0 == strcmp(auth_tx.TLF01_details.response_code, "55")))
         {
            if ( false == update_database( &auth_tx ))
            {
               memcpy( auth_tx.TLF01_details.response_text,
                      "Unable to update database tables.", 40 );

               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to update database tables. Term Id:%s ,Merch Id: %s RRN: %s ",
               					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncjcb2_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);

               strcpy(auth_tx.TLF01_details.response_code,"96");

               /* Unable to complete a transaction. Inform host; log to TLF01.*/
               if( false == incoming_other_host_response_db_reply( &auth_tx ))
               {
                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                  sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
                  					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
                  ncjcb2_log_message( 3, 2, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
               }
            }
         }
         else
         {
            if( false == incoming_other_host_response_db_reply( &auth_tx ))
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
               					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncjcb2_log_message( 3, 2, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }
         return(true);
      }
   }

   Tran_Flow = SWITCHED_OUT;
   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,
           sizeof(ncf01_i.tpdu_id));

   if ( (AUTH_REVERSAL  == auth_tx.TLF01_details.tx_key) ||
        (AUTH_VOID_SALE == auth_tx.TLF01_details.tx_key)
#if BDOR_62_PREAUTH_VOID
	 || (AUTH_VOID_PRE_AUTHORIZATION == auth_tx.TLF01_details.tx_key)
#endif
		)
   {
	  if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"ncjcbA",5))
	  {		  //
	  }else
	  {
      strcpy( auth_tx.orig_acq_inst_id, ncf01_i.acquirer_inst_id_code );
      strcpy( auth_tx.amex_pos_entry_mode, ncf01_i.forwarding_inst_id_code );
	  }
      strcpy( auth_tx.TLF01_details.retrieval_ref_num,
              auth_tx.TLF01_details.orig_retrieval_ref_num );

      /* If the transaction was approved, but we get a reversal,
       * output a response code of 17.  If the transaction timed
       * out, output a response code of 68.
       */
      if ( 0 == strcmp("00", auth_tx.TLF01_details.response_code)
#if BDOR_62_PREAUTH_VOID
	  || ( AUTH_VOID_PRE_AUTHORIZATION  == auth_tx.TLF01_details.tx_key )
#endif
		 )
      {
         strcpy( auth_tx.TLF01_details.response_code, CANCELLATION);
      }
      else if ( 0 == strcmp("TO", auth_tx.TLF01_details.response_code))
      {
         strcpy( auth_tx.TLF01_details.response_code, LATE_RESPONSE);
      }

      if ( 0 == strcmp(CANCELLATION, auth_tx.TLF01_details.response_code))   
      {
         get_gmt_time( auth_tx.TLF01_details.transmission_timestamp );
      }
   }
   else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"ncjcb",5))
   {
	   //Don't do PIN translation if tx is coming from NCJCB incoming
   }
   else if (true == hhutil_verify_pin_block(&auth_tx) && true == verify_host_app_status(&auth_tx))
   {	/*TF Phani: ATP should not be doing any PIN transaclation during offline mode,since during STIP,
			   ATP will be the one validating the cards.ATP directly validates the PIN*/
      
      /* PIN Translation is required */
      strcpy(auth_tx.TLF01_details.dest_key, ncf01_i.cwk);
      strcpy(auth_tx.TLF01_details.dest_pin_format, ncf01_i.pin_type);
      if(false == hhutil_process_debit_transaction(&auth_tx))
      {
         return(false);
      }
      return(true);
   }

   /* Do not perform PIN translation. */
   strcpy(auth_tx.TLF01_details.security_response_code, "00");
   if ( false == process_incoming_message_continued( &auth_tx ) )
      return( false );
   return(true);
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
   INT  ret_val;
   if (0 == strcmp(p_auth_tx->TLF01_details.security_response_code, "00"))
   {
      ret_val = incoming_terminal_request( p_auth_tx );
   }
   else 
   {
      memcpy( p_auth_tx->TLF01_details.response_text,
             "Pin translation failed.", 40 );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Pin translation failed. Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "process_incoming_message_continued" , 0 ,p_auth_tx->TLF01_details.card_num);
      perform_error_processing( p_auth_tx );
      send_it_back_to_terminal("96", p_auth_tx);
      ret_val = false;
   }
   return( ret_val );
}

BOOLEAN update_ncf02(pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
 //  CHAR       temp[3] = "";
 //  CHAR       tran_finish_time[17] = ""; 
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(AUTH_TX)];
   LONG	     ret_code;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};
   
   if (strcmp(p_auth_tx->TLF01_details.response_code,"00"))
   {
      memset(buffer, 0, sizeof(buffer));
      memcpy(buffer+Auth_Tx_Size,&p_auth_tx, Auth_Tx_Size);

      p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
                                   ST1_DB_UPDATE, 
                                   ST2_NONE,
                                   netds_que_name, 
                                   application_que_name,
                                   (pBYTE)&buffer,
                                   sizeof(buffer), 
                                   NCF02_DATA); 
      if(NULL_PTR == p_msg_ins)
      {
         ncjcb2_log_message( 3, 2, "Insufficient Memory to build insert NCF02 message" , "update_ncf02" , 0 ,"");
         return(false);
      }
      //ret_code = pteipc_send(p_msg_ins, logger_que_name); 
      ret_code = pteipc_send(p_msg_ins, netds_que_name); 
      free(p_msg_ins);
      if(ret_code < 0)
      {
         pteipc_get_errormsg(ret_code, err_mesg);
         //sprintf(Buff, "failed transmit to logger: %s. %s", logger_que_name, err_mesg);
         sprintf(Buff, "failed transmit to que: %s. %s", netds_que_name, err_mesg);
         memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
         ncjcb2_log_message( 3, 3, Buff, "update_ncf02" , 0 ,"");
         return(false);
      }
   }
   return(true);
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

   memset (&ncf30, 0, Ncf30_Size);
   memcpy(&ncf30, pCurrent+(Auth_Tx_Size), Ncf30_Size);
   memset (&auth_tx, 0, Auth_Tx_Size);
   memcpy(&auth_tx, pCurrent, Auth_Tx_Size);
   /* Original txn was found in NCF30 for an advice or reversal. */
   if ( (auth_tx.TLF01_details.tx_key == AUTH_ADVICE) ||
        (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT) )
   {
       /* Treat advices differently. */
      (void)process_auth_advice( &auth_tx, FOUND, ncf30 );
   }
   else
   {
       update_auth_tx_with_ncf30_data(ncf30, &auth_tx);
      /* Tell txcntl to go process the reversal. */
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
INT process_auth_advice( pAUTH_TX p_auth_tx, INT status, NCF30 ncf30 )
{
   INT   ret_val;
   char  resp_code[4]; //temp data
   BYTE  txkey;

   /* Send response to host and log to TLF01.
    * Need to do this prior to going to txcntl where the auth_number
    * and response code may get overwritten.
    */
   txkey=p_auth_tx->TLF01_details.tx_key;
   p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
   ret_val = incoming_other_host_response_db_reply( p_auth_tx );
   
   strncpy(resp_code,p_auth_tx->TLF01_details.response_code,2);
   //RESTORE THE TX_KEY
   p_auth_tx->TLF01_details.tx_key=txkey;
   if ( true == ret_val )
   {
      /* See if we need to send to txcntl. */
      if ( ( (FOUND == status) &&
             (0 != strcmp(p_auth_tx->TLF01_details.response_code, "00" )) )
           ||
           ( (NOT_FOUND == status) &&
             (0 == strcmp(p_auth_tx->TLF01_details.response_code, "00" )) )
         )
      {
         /* Send advice to transaction control. */
         //DD 12/30 from amex
         if ( FOUND == status )
         {
            update_auth_tx_with_ncf30_data( ncf30, p_auth_tx );
           //restoring back the response code
           strncpy(p_auth_tx->TLF01_details.response_code,resp_code,2);
         }
         //DD 12/30 from amex  Commented the following line
         //strcpy( p_auth_tx->TLF01_details.response_code, "00" );
         ret_val = hhutil_transfer_ctl_to_auth_app( p_auth_tx );
      }
   }
   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to complete processing of an advice (1120). Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "process_auth_advice" , 0 ,p_auth_tx->TLF01_details.card_num);
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
   CHAR            Buff[512]={0};
   CHAR            err_mesg[512] = {0};
 
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
      ncjcb2_log_message( 3, 2, "Insufficient Memory to build NCF30 message" , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s", netds_que_name, err_mesg );
      memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
      ncjcb2_log_message( 3, 3, Buff , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }
   return(true);
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
 *              DURGA: 
                 sys_trace_audit_num -> sys_trace_audit_nbr
                 transaction_fee_amount -> tran_fee_amount
                 also lifted the comments
 ******************************************************************************/
VOID update_auth_tx_with_ncf30_data(NCF30 ncf30, pAUTH_TX p_auth_tx)
{
  strcpy( p_auth_tx->TLF01_details.card_num , ncf30.card_nbr ); 
  strcpy( p_auth_tx->TLF01_details.processing_code , ncf30.processing_code ); 
  strcpy( p_auth_tx->TLF01_details.orig_amount , ncf30.tran_amount ); 
  strcpy( p_auth_tx->TLF01_details.date_yyyymmdd , ncf30.tran_date ); 
  strcpy( p_auth_tx->TLF01_details.orig_retrieval_ref_num , ncf30.primary_key.retrieval_ref_num ); 
  strcpy( p_auth_tx->TLF01_details.auth_number , ncf30.auth_number ); 
  strcpy( p_auth_tx->TLF01_details.response_code , ncf30.response_code ); 
  strcpy( p_auth_tx->TLF01_details.terminal_id , ncf30.device_id ); 
  strcpy( p_auth_tx->TLF01_details.currency_code , ncf30.currency_code ); // DVD
  strcpy( p_auth_tx->TLF01_details.orig_message , ncf30.message_type ); 
  strcpy( p_auth_tx->TLF01_details.orig_sys_trace_num , ncf30.sys_trace_audit_nbr ); //DVD
  strcpy( p_auth_tx->TLF01_details.settlement_date , ncf30.settlement_date );  //DVD
  strcpy( p_auth_tx->TLF01_details.tran_fee_amount , ncf30.tran_fee_amount ); 
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
INT recon_update( pAUTH_TX p_auth_tx, BYTE subtype_2, pCHAR network_type )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)]; LONG            ret_code;
   CHAR            Buff[256];
   CHAR            err_mesg[200] = {0};
   NCF30           ncf30;
   CHAR            time_date[25];


   /* Build the NCF30 structure */
   memset( &ncf30, 0x00, Ncf30_Size );
   strcpy( ncf30.primary_key.network_id,    AppName ); 
   strcpy( ncf30.primary_key.network_type, network_type ); 
   strcpy( ncf30.primary_key.batch_number, "ACTIVE" ); 
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num);
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
   strcpy( ncf30.response_code, p_auth_tx->TLF01_details.response_code);
   strcpy( ncf30.currency_code, p_auth_tx->TLF01_details.currency_code );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ))
   {
      /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
      ptetime_get_timestamp( time_date );

      /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
      get_date( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
   	  if ( p_auth_tx->TLF01_details.time_hhmmss[0] == 0x00 )
      get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss   );
   }
   strcpy( ncf30.tran_date, p_auth_tx->TLF01_details.date_yyyymmdd ); 
   strcpy( ncf30.tran_time, p_auth_tx->TLF01_details.time_hhmmss   ); 
   strcpy( ncf30.sys_trace_audit_nbr, p_auth_tx->TLF01_details.sys_trace_audit_num );
   ncf30.tx_key = p_auth_tx->TLF01_details.tx_key;
   strcpy( ncf30.invoice_nbr, p_auth_tx->TLF01_details.invoice_number );
   strcpy( ncf30.network_data, p_auth_tx->TLF01_details.mcard_banknet );
   strcpy( ncf30.transmission_timestamp,p_auth_tx->TLF01_details.transmission_timestamp);

   /* Build message to get host specific data from data server. */ 
   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_UPDATE, 
                             subtype_2,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             ALL_DATA );
   if ( NULL_PTR == p_msg  )
   {
      ncjcb2_log_message( 3, 2, "Insufficient Memory to build update NCF30 message" , "recon_update" , 0 ,"");
      return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free(p_msg);
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for update ncf30: %s. %s", netds_que_name, err_mesg );
      memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
      ncjcb2_log_message( 3, 3, Buff, "recon_update" , 0 ,"");
      return( false );
   }
   return( true );
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
   BYTE       buffer[sizeof(AUTH_TX)];
   LONG       ret_code;
   INT        ret_val = true;
   CHAR       Buff[512] = {0};
   CHAR       err_mesg[512] = {0};

   memset(buffer, 0, sizeof(buffer));
   //memcpy(buffer+Auth_Tx_Size, p_auth_tx, Auth_Tx_Size); 
   memcpy(buffer, p_auth_tx, Auth_Tx_Size); // svelaga : 12/02

   p_msg = ptemsg_build_msg(MT_DB_REQUEST, 
                                ST1_DB_UPDATE, 
                                ST2_ACQUIRER,
                                updatds_que_name, 
                                application_que_name,
                                (pBYTE)&buffer,
                                sizeof(buffer), 
                                ALL_DATA); 
   if(NULL_PTR == p_msg)
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build UPDATE ALL message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
      ret_val = false;
   }
   else
   {
      ret_code = pteipc_send(p_msg, updatds_que_name); 
      free(p_msg);
      if(ret_code < 0)
	  {
         pteipc_get_errormsg(ret_code, err_mesg);
         sprintf(Buff,"Failed transmit to que: %s. %s",
                 updatds_que_name,err_mesg);
         memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buff , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = false;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         validate_date_yymm
 *
 *  DESCRIPTION:  This function validates that a date in the format of YYMM
 *                has a valid year (numeric and greater than 0) and a valid
 *                month (numeric and ranging from 01 to 12).
 *
 *  INPUTS:       date   - Date to be validated
 *                tx_key - Transaction type
 *
 *  OUTPUTS:      err_buf - Indicates what is wrong with the input
 *
 *  RTRN VALUE:   true if good date, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT validate_date_yymm( pBYTE date, BYTE tx_key, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  iYear;
   INT  iMonth;
   BYTE temp_year[3]     = "";
   BYTE temp_month[3]    = "";
   BYTE current_year[3]  = "";
   BYTE current_month[3] = "";
   CHAR time_date[25]    = "";
   INT  icurrent_year;
   INT  icurrent_month;

   /* Only validate expiration date for certain transactions. */
	switch (tx_key)
	{
		case AUTH_SALE:
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_AUTHORIZATION:
		case AUTH_QUASI_CASH:
		case AUTH_CASH_ADVANCE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
		  	break ;
		default:
			return true;
	}

   /* Get Current date. */
   ptetime_get_timestamp( time_date ); /* Format: "YYYY-0M-0D-0H.0I.0S.JJJ" */
   memcpy( current_year, time_date+2, 2 );
   memcpy( current_month,time_date+5, 2 );

   /* Convert current date to integer. */
   icurrent_year  = atoi( current_year  );
   icurrent_month = atoi( current_month );


   memcpy( temp_year,  date,   2 );
   memcpy( temp_month, date+2, 2 );

   iYear  = atoi( temp_year  );
   iMonth = atoi( temp_month );

   if ( false == isnum(temp_year) )
      sprintf(err_buf, "Expiration date - Invalid year, not numeric: %s", date);
   else if ( false == isnum(temp_month) )
      sprintf(err_buf,"Expiration date - Invalid month, not numeric: %s", date);
   else if ( (iYear < 0) || (iYear > 60 ) )
      sprintf( err_buf, "Card is expired: %s", date );
   else if ( (iMonth < 1) || (iMonth > 12) )
      sprintf( err_buf, "Expiration date - Invalid month: %s", date );
   else if ( 2 != strlen(temp_year) )
      sprintf(err_buf,"Expiration date - Invalid year, not 2 digits: %s", date);
   else if ( 2 != strlen(temp_month) )
      sprintf(err_buf,"Expiration date - Invalid month, not 2 digits: %s",date);

   /* Verify date is not expired. */
   else if ( icurrent_year > iYear )
      sprintf( err_buf, "Card is expired: %s", date );

   else if ( (icurrent_year == iYear) && ( icurrent_month > iMonth ) )
      sprintf( err_buf, "Card is expired: %s", date );

   else
      ret_val = true;

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
   BYTE  originator_queue[17]={0};
   BYTE  originator_info[33] ={0};
   CHAR  err_msg[256]={0};

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
   if ( false == ret_code  )
   {
      ncjcb2_log_message( 3, 2, err_msg , "send_transaction_statistics" , 0 ,"");
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
   BYTE  originator_queue[17]={0};
   BYTE  originator_info[33] ={0};
   CHAR  err_msg[256]={0};

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
   if ( false == ret_code  )
   {
      ncjcb2_log_message( 3, 2, err_msg , "reset_transaction_statistics" , 0 ,"");
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         CREATE_TIMER_LOOKUP_KEY
 *
 *  DESCRIPTION:  This function creates a lookup key for the timer data
 *                server.  The key consists of appname + last 8 digits of
 *                RRN + stan.  If txn is a card update (0302/0312 txn),
 *                then last 8 of card number is used in place of RRN.
 *
 *  INPUTS:       p_auth_tx - Transaction information
 *                AppName   - Name of application
 *
 *  OUTPUTS:      key - Lookup key (Should be at least 22 characters)
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_timer_lookup_key( pAUTH_TX p_auth_tx, pCHAR key )
{
   INT  len;

   strcpy( key, AppName );

   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE) )
   {
      if ( p_auth_tx->TLF01_details.card_num[0] != 0x00 )
      {
         len = strlen( p_auth_tx->TLF01_details.card_num );
         if ( len >= 8 )
            strncat( key, &p_auth_tx->TLF01_details.card_num[len-8], 8 );
      }
   }

   if (( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE ) ||
	   ( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL ) ||
	   (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
	   (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT) ||
	   (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION))
   {
      strcat( key, p_auth_tx->TLF01_details.orig_sys_trace_num );
	}
   else
      strcat( key, p_auth_tx->TLF01_details.sys_trace_audit_num );
   return;
}
/******************************************************************************
 *
 *  NAME:         send_request_to_host_for_late_response
 *
 *  DESCRIPTION:  This procedure creates the ISO message for the MasterCard
 *                host.  It calls the necessary routines to put it together
 *                and send it and will not keep timer for the transaction sent.
 *
 *  INPUTS:       p_auth_tx - Transaction record for the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Raghavendra H R
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncjcb2_send_reversal_to_host_for_late_response( pAUTH_TX p_auth_tx )
{
	  pPTE_MSG	p_msg_set_timer     = NULL_PTR;
   pPTE_MSG p_msg_host_out = NULL_PTR;
   CHAR     gmt_time[20];
   CHAR     err_msg[512] = {0};
   INT      ret_code;
   WORD     request_timer;
	INT		buf_temp_len=0;
	CHAR     lookup_key[22];
	BYTE     buffer1[sizeof(AUTH_TX)];
   /*+--------------------------------------------------+
     | Send response to authorizer for failed messages. |
     +--------------------------------------------------+*/

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
   strncpy(p_auth_tx->advice_repeat_count,"1",1);
   /*+--------------------+
     | Build Host Message |
     +--------------------+*/
   if ( GS_TXN_PERF_REQ == p_auth_tx->TLF01_details.general_status )
   {
      build_host_structure( p_auth_tx );
      if ( 1 == Build_Host_Request( p_auth_tx ) )
      {
   		/*+---------------------------+
             | Build Start Timer Message |
             +---------------------------+*/
            memset(lookup_key, 0x00, sizeof(lookup_key));
            create_timer_lookup_key( p_auth_tx, lookup_key );
            memcpy(buffer1,p_auth_tx, Auth_Tx_Size);

            request_timer = atoi(ncf01_i.request_timer);
            p_msg_set_timer = ptetimer_bld_start_timer_req(lookup_key,
                                        p_auth_tx->TLF01_details.primary_key.transaction_id,
                                        p_auth_tx->TLF01_details.terminal_id,
                                        timerds_que_name,
                                        application_que_name,
										request_timer,
                                        0,
                                        (pBYTE)&buffer1,
                                        sizeof(buffer1));

                  if ( NULL_PTR == p_msg_set_timer )
                  {
                      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                      sprintf(Log_Bufr," Insufficient Memory to build timer message Term Id:%s ,Merch Id: %s RRN: %s ",
                      					p_auth_tx->TLF01_details.terminal_id,
                      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
                      ncjcb2_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
                      return(false);
                  }
                  else
                  {
                	  ret_code=pteipc_send( p_msg_set_timer, "timerdsA");
                	  free( p_msg_set_timer );
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
			 if ( 1 != ret_code )
			 {
				  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
									p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
				  memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
								sizeof(p_auth_tx->TLF01_details.response_text)-1 );
				  strcat(err_msg,Log_Bufr);
				  ncjcb2_log_message( 3, 3, err_msg , "send_reversal_to_host_for_late_response" , 0 ,p_auth_tx->TLF01_details.card_num);
			 }
			 else
			 {

					// Update transaction statistics
					update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

					// Log transaction to trace file.
					write_to_txn_file();
			 }

      }
      else
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to send request to host, tran table error Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         memcpy( p_auth_tx->TLF01_details.response_text, "Unable to send request to host, tran table error",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncjcb2_log_message( 3, 2, Log_Bufr , "send_reversal_to_host_for_late_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }
   return( ret_code );
}  /* send_reversal_to_host_for_late_response */

