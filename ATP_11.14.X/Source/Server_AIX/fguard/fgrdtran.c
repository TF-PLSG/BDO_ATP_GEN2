/******************************************************************************
 
 	fgrdtran.c
 
   Copyright (c) 2006, Hypercom, Inc.
   All Rights Reserved.
 
   TITLE:  Equitable Fraudguard Handler Driver
 
 	This module is the Fraudguard handler for the Equitable host.
   It will process all messages destined to, or originated from,
   the Fraudguard.

   APPLICATION:  Advanced Transaction Processor (ATP)
 
   REVISION HISTORY
 
   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\fguard\fgrdtran.c  $
   
      Rev 1.5   Nov 17 2006 13:55:10   DIrby
   Modified to not report queues backing up until the queue count is 80 instead of 10.
   SCR 23657
   
      Rev 1.4   Nov 06 2006 13:15:32   DIrby
   Modified to get number of messages in the FSoap dialog connection
   queue.  If the count is too high, do not send request to FG.  Put out
   a warning message every 25 txns.
   SCR 23657
   
      Rev 1.3   Oct 09 2006 14:25:32   DIrby
   Added capability to send Issuer (ATP approves the txns) to Fraudguard.
   SCR 21851
   
      Rev 1.2   Jul 19 2005 11:27:14   dirby
   Bug fixes before initial release to customer:
   1.  Added code to respond to TranLookup on batch resends.
   2.  Added code to populate ReqRsp_Buffer on batch resends.
   SCR 14421
   
      Rev 1.1   Jul 15 2005 09:52:32   dirby
   Added code to copy transaction Id into Auth Tx for messages from
   Fraudguard.  Also break out of the parsing loop when parsing
   responses from Fraudguard if there is an error.
   SCR 14421
   
      Rev 1.0   Jul 08 2005 09:15:46   dirby
   Initial revision.

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
#include "fgrd_constants.h"
#include "fgrd_prototypes.h"
#include "txutils.h"
#include "txtrace.h"

/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

extern CHAR    AppName[];

/* QUEUES */
extern CHAR       updateds_que_name[];
extern CHAR       trands_que_name[];
extern CHAR       netds_que_name[];
extern CHAR       applnk_que_name[];
extern CHAR       app_name[];
extern CHAR       authorizer_que_name[];
extern CHAR       timerds_que_name [];

/* Database structures */
extern AUTH_TX   Auth_Tx;
extern CRF01     Crf01;
extern FG01      Fg01;
extern FGUARD    Fguard;
extern FG_RAW    Fgraw;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Crf01_Size;
extern INT  Fguard_Size;
extern INT  Fgraw_Size;
extern INT  Fg01_Size;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;
extern INT    QueMsgCnt_A;
extern INT    QueMsgCnt_I;

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
extern BYTE   Orig_Queue[];
extern BYTE   Orig_Info[];

extern ISS_AMTS  Iss_Amts;

extern double    USD_to_Peso_Conv_Rate;
extern INT       ConvRateFlag;
extern INT       Authorizer;
extern INT       Card_Family;
extern INT       TranFlow;
extern BYTE      ReqRsp_Buffer[];
extern INT       BufLen;

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
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   /* Copy host message into a character string. */
   copy_to_req_resp_buffer( p_msg_in );

   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Determine if switched in request,
    * or response to a switched out txn.
    */
   TranFlow = determine_txn_flow( ReqRsp_Buffer, HOST );

   if ( TranFlow == SWITCHED_IN )
      process_request_from_host();
   else
      process_response_from_host();

   return;
}

/**************************************************************************
 *
 *  NAME:         process_request_from_host
 *
 *  DESCRIPTION:  This function is the driver for requests from the host.
 *                Right now, its only purpose is to log the request.
 *
 *  INPUTS:       ReqRsp_Buffer - (Global) Transaction data
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
   INT    ret_val;
   INT    len;
   INT    more_txns = true;
   pCHAR  pTxn;
   CHAR   errbuf[100];


   pTxn = ReqRsp_Buffer;
   while( more_txns )
   {
      memset( &Fguard, 0x00, Fguard_Size );
      if ( NULL != strstr(pTxn, FG_REQUEST_MSG_TAG) )
      {
         ret_val = parse_request_msg( pTxn, &len );
         if ( ret_val == true )
         {
            generate_tran_id( Fguard.transaction_id );

            if ( Auth_Tx.TLF01_details.primary_key.transaction_id[0] == 0x00 )
            {
               memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
                       Fguard.transaction_id,
                       sizeof(Fguard.transaction_id) );
            }
            Fguard.status = FG_REQUEST_RECEIVED;
            populate_fgraw( pTxn, FG_REQUEST );
            ret_val = insert_db_record( FGUARD_DATA );
         }
         else
         {
            strcpy( errbuf, "Unable to parse request from Fraudguard." );
            log_message( 3, 2, errbuf, "process_request_from_host" );
         }
         pTxn += len;
      }
      else
      {
         more_txns = false;
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
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   INT            continue_process = true;

   p_msg_data    = ptemsg_get_pte_msg_data(p_msg_in);
   p_data        = ptemsg_get_pte_msg_data_data(p_msg_data);

   memcpy( &Auth_Tx, p_data, Auth_Tx_Size );

  
      /* Acquirer */
      Authorizer = HOST;

      /* See if acquirer dialog connection is queuing up. */
      if ( 10 < get_queue_msg_count("hstfgrdA") )
      {
         /* Queue count is more than 10.  Do not send to FG. */
         continue_process = false;
         if ( QueMsgCnt_A > 25 )
         {
            /* Queue count is staying above 10. */
            QueMsgCnt_A = 0;

            /* Display a message every 25 txns to reset FSOAP and connection */
            log_message( 3, 3, 
                        "*** Hstfgrd is queuing up. Reset FSOAP then hstfgrd ***",
                        "process_request_for_host" );
         }
         else
            QueMsgCnt_A++;
      }
      else
      {
         /* Queue Message Count is OK.  Reset counter. */
         QueMsgCnt_A = 0;
      }

   if ( continue_process )
      process_single_txn();

   return;
}

/****************************************************************************
 *
 *  NAME:         process_single_txn
 *
 *  DESCRIPTION:  This function sends a single transaction to Fraudguard
 *                using data from Auth Tx.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction Structure
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ****************************************************************************/
void process_single_txn()
{
   INT   retval;
   CHAR  errbuf[200] = "";
   pBYTE p_ptr = NULL;

   retval = populate_fguard_request();
   create_xml_request();

   /* Point past Group tag */
   p_ptr = strchr( ReqRsp_Buffer, '>' );
   if ( p_ptr == NULL )
      populate_fgraw( ReqRsp_Buffer, FG_REQUEST );
   else
      populate_fgraw( p_ptr+1, FG_REQUEST );

   if ( retval == true )
   {
      if ( true == send_to_fraudguard(errbuf) )
      {
         Fguard.status = FG_REQUEST_SENT;
      }
      else
      {
         Fguard.status = FG_SEND_REQUEST_FAILED;
         strcpy( Fguard.result_text, errbuf );
         log_message( 3, 3, errbuf, "process_single_txn" );
      }
   }
   else
   {
      Fguard.status = FG_FAILED;
      strcpy(Fguard.result_text,"Request not sent. Currency conversion failed");
   }
   (void)insert_db_record( FGUARD_DATA );

   return;
}


/******************************************************************************
 *
 *  NAME:         PROCESS_BATCH_SEND
 *
 *  DESCRIPTION:  Process one or more transactions from TranLookup for resend.
 *                Need to go get the transactions from TLF01.
 *
 *  INPUTS:       p_msg_in - PTE Message from TranLookup containing tran Ids
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void begin_batch_send( pPTE_MSG p_msg_in )
{
   INT            retval;
   LONG           ipc_rcode;
   pBYTE          p_data;
   pPTE_MSG       p_msg_out = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           app_data_type;
   BYTE           subtype2;
   pCHAR          reply_que;
   CHAR           orig_comm_inf[MAX_ORIG_INFO_SIZE];
   CHAR           errbuf[100] = "";

   p_msg_data    = ptemsg_get_pte_msg_data(p_msg_in);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = ptemsg_get_pte_msg_data_data(p_msg_data);
   p_data        = p_data + sizeof(AUTH_TX);

   subtype2 = ptemsg_get_msg_subtype2( p_msg_in );
   retval = get_db_record( FGRAW_DATA, subtype2, p_data );
   if ( retval == false )
   {
      log_message( 3, 3,
                  "Unable to send message to trands data server for a resend.",
                  "begin_batch_send" );
   }

   /* Reply to TranLookup so it does not time out. */
   reply_que = ptemsg_get_msg_orig_queue (p_msg_in);
   strcpy( orig_comm_inf, ptemsg_get_msg_orig_comms_info(p_msg_in) );

   p_data = NULL;
   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                 ptemsg_get_msg_subtype1 (p_msg_in),
                                 subtype2,
                                 reply_que,
                                 application_que_name,
                                 p_data,
                                 0,
                                 app_data_type );

   ptemsg_set_result_code( p_msg_out, PTEMSG_OK );
   ptemsg_set_orig_comms_info( p_msg_out, orig_comm_inf );

   /* Send the response. */
   ipc_rcode = pteipc_send( p_msg_out, reply_que );
   free( p_msg_out );

   if ( ipc_rcode < 0 )
   {
      sprintf( errbuf,
              "Unable to reply to TranLookup Resend request. Err code: %d",
               ipc_rcode );
   }

   return;
}

/******************************************************************************
 *
 *  NAME:         CONTINUE_RESEND_PROCESS
 *
 *  DESCRIPTION:  At this point, we are receiving responses from trands.
 *                This function decides if we are resending a single txn
 *                to Fraudguard, or more than one.
 *
 *  INPUTS:       p_data   - Results from request to get Fguard msgs to resend
 *                subtype2 - Echoed from data server
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void continue_resend_process( pBYTE p_data, BYTE subtype2 )
{
   if ( subtype2 == ST2_NONE )
      resend_single_txn( p_data );
   else
      resend_multiple_txns( p_data, subtype2 );

   return;
}

/******************************************************************************
 *
 *  NAME:         RESEND_MULTIPLE_TXNS
 *
 *  DESCRIPTION:  In the Fgraw List are transactions all ready to go to
 *                Fraudguard.  Here we will send them one at a time.
 *
 *  INPUTS:       p_data   - Results from request to get Fguard msgs to resend
 *                subtype2 - Echoed from data server
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void resend_multiple_txns( pBYTE p_data, BYTE subtype2 )
{
   INT             retval;
   INT             i;
   INT             len;
   INT             num_txns;
   INT             num_sent = 0;
   INT             num_fail = 0;
   CHAR            errbuf[100] = "";
   FGRAW_GUI_LIST  Fgraw_List;

   memset( &Fgraw, 0x00, Fgraw_Size );
   memset( &Fgraw_List, 0x00, sizeof(FGRAW_GUI_LIST) );
   memcpy( &Fgraw_List, p_data+Auth_Tx_Size, sizeof(FGRAW_GUI_LIST) );

   num_txns = atoi( Fgraw_List.num_returned );

   for( i=0; i<num_txns; i++ )
   {
      memcpy( &Fgraw, &Fgraw_List.FG_RAW_record[i], Fgraw_Size );

      memset( ReqRsp_Buffer, 0x00, COMM_BUF_SZE );
      len = atoi( Fgraw.length );
      if ( len > COMM_BUF_SZE )
         len = COMM_BUF_SZE;
      memcpy( ReqRsp_Buffer, Fgraw.raw_data, len );
      BufLen = len;

      if ( true == send_to_fraudguard(errbuf) )
      {
         num_sent++;
         memset( &Fguard, 0x00, sizeof(FGUARD) );
         memcpy( Fguard.transaction_id,
                 Fgraw_List.FG_RAW_record[i].transaction_id,
                 sizeof(Fgraw.transaction_id) );

         Fguard.status = FG_REQUEST_SENT;
      }
      else
      {
         num_fail++;
         Fguard.status = FG_SEND_REQUEST_FAILED;
         strcpy( Fguard.result_text, errbuf );
         log_message( 3, 3, errbuf, "resend_multiple_txns" );
      }
      (void)update_database( FGUARD_DATA );
   }

   /* This batch has been sent. */
   if ( num_sent > num_fail )
   {
      /* This tells us that at least most txns in batch were sent.
       * Do not want to send the rest of the batch if we are
       * encountering too many errors.
       */
      if ( (num_txns == 10) &&
           ((subtype2 == ST2_GET_ALL_FG_FAILED) ||
            (subtype2 == ST2_GET_10_MORE_FG   )  ))
      {
         /* We are in the process of getting all failed txns.
          * We get them 10 at a time. When num_txns is 10, it
          * means there might be more to get.  So we must ask
          * for the next batch. Give the transaction Id of the
          * last transaction processed.
          */
         retval = get_db_record( FGRAW_DATA,
                                 ST2_GET_10_MORE_FG,
                                 Fgraw.transaction_id );
         if ( retval == false )
         {
            log_message( 3, 3,
                        "Unable to send message to trands data server for a resend.",
                        "resend_multiple_txns" );
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         RESEND_SINGLE_TXN
 *
 *  DESCRIPTION:  The data server has returned one transaction that is to
 *                go to Fraudguard.  Will send it here and update its status
 *                in FGUARD.
 *
 *  INPUTS:       p_data   - Results from request to get Fguard msgs to resend
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void resend_single_txn( pBYTE p_data )
{
   INT   len;
   CHAR  errbuf[100] = "";

   memset( &Fgraw, 0x00, sizeof(FG_RAW) );
   memcpy( &Fgraw, p_data+Auth_Tx_Size, sizeof(FG_RAW) );

   memset( &Fguard, 0x00, sizeof(FGUARD) );
   memcpy( Fguard.transaction_id,
           Fgraw.transaction_id,
           sizeof(Fgraw.transaction_id) );

   memset( ReqRsp_Buffer, 0x00, COMM_BUF_SZE );
   len = atoi( Fgraw.length );
   if ( len > COMM_BUF_SZE )
      len = COMM_BUF_SZE;
   memcpy( ReqRsp_Buffer, Fgraw.raw_data, len );
   BufLen = len;

   if ( true == send_to_fraudguard(errbuf) )
   {
      Fguard.status = FG_REQUEST_SENT;
   }
   else
   {
      Fguard.status = FG_SEND_REQUEST_FAILED;
      strcpy( Fguard.result_text, errbuf );
      log_message( 3, 3, errbuf, "resend_single_txn" );
   }
   (void)update_database( FGUARD_DATA );

   return;
}

/**************************************************************************
 *
 *  NAME:         process_response_from_host
 *
 *  DESCRIPTION:  This function is the driver for responses from the host.
 *
 *  INPUTS:       ReqRsp_Buffer - (Global) Transaction data
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
   INT    ret_val;
   INT    len = 0;
   INT    more_txns = true;
   pCHAR  pTxn;
   pCHAR  pTranId = NULL;
   CHAR   errbuf[500];


   pTxn = ReqRsp_Buffer;
   while( more_txns )
   {
      memset( &Fguard,  0x00, Fguard_Size  );
      memset( &Auth_Tx, 0x00, Auth_Tx_Size );
      if ( NULL != strstr(pTxn, FG_RESULT_TAG) )
      {
         ret_val = parse_response_msg( pTxn, &len );
         if ( ret_val == true )
         {
            /* Set the Result Status according to the Result tag. */
            if ( 0 == strcmp(Fguard.result_code, "TRUE") )
               Fguard.status = FG_SUCCESS;
            else if ( 0 == strcmp(Fguard.result_code, "FALSE") )
            {
               Fguard.status = FG_FAILED;
               if ( Fguard.result_text[0] != 0x00 )
               {
                  sprintf( errbuf,
                          "Fraudguard Failure: %s",
                           Fguard.result_text );
                  log_message( 3, 2, errbuf, "process_response_from_host" );
               }
            }
            else
               Fguard.status = FG_RESULT_UNKNOWN;

            populate_fgraw( pTxn, FG_RESPONSE );
            memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
                    Fguard.caller_msg_id,
                    sizeof(Fguard.caller_msg_id) );

			ret_val = true;
			/*NO need to update the DB - TF PHANI*/
           // ret_val = update_database( FGUARD_DATA );
         }
         else
         {
            strcpy( errbuf, "Unable to parse response from Fraudguard." );
            log_message( 3, 2, errbuf, "process_response_from_host" );
            break;
         }
         pTxn += len;
      }
      else
      {
         more_txns = false;
      }
   }
   return;
}

   

