/******************************************************************************

 	fgrdutil.c

   Copyright (c) 2006, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  Equitable Fraudguard Handler Driver

  	This module is the handler for the Equitable Fraudguard host.
   It will process all messages destined to, or originated from,
   the Equitable host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\fguard\fgrdutil.c  $
   
      Rev 1.11   Nov 17 2006 14:20:24   DIrby
   Changed the transaction request delay from .75 secs to .075 secs
   SCR 23657
   
      Rev 1.10   Nov 06 2006 13:15:32   DIrby
   Modified to get number of messages in the FSoap dialog connection
   queue.  If the count is too high, do not send request to FG.  Put out
   a warning message every 25 txns.
   SCR 23657
   
      Rev 1.9   Oct 27 2006 13:42:40   DIrby
   Made the following bug fixes for Issuer txns:
   1.  Issuer labels to be lower case
   2.  Card Currency field needs to be always present
   3.  For local txns, never convert f4, f6, f49, and f51.
   4.  F43 should always be present; f43c defaults to PHL for local txns.
   5.  If f19 not present, populate it from field 49.
   6.  Field 25 should always be present. Use default values is missing.
   7.  If fields 5/6 and 50/51 are missing, populate using field 4 and 49.
   SCR 23522
   
      Rev 1.8   Oct 10 2006 13:48:00   DIrby
   Added capability to send Issuer (ATP approves the txns) to Fraudguard.
   SCR 21851
   
      Rev 1.7   Sep 26 2006 14:13:06   dirby
   Added a 3/4 second wait prior to sending a txn to FSoap.  This is
   because the FSoap connection does not seem to handle heavy volume.
   SCR 21292
   
      Rev 1.6   May 11 2006 14:15:34   dirby
   Fixed a typo: was using acquirer_id for field 32. Need acquiring_id.
   SCR 21188
   
      Rev 1.5   Feb 27 2006 14:05:14   dirby
   Modified to use hardcoded values for Acquirer ID if one is not present.
   SCR 19054
   
      Rev 1.4   Dec 21 2005 15:58:34   dirby
   1.  Changed the amount fields to show the decimal point.
   2.  Added code to convert special XML characters so that XML
       can read them as if they were normal.  Else, XML will error out.
   SCR 19054
   
      Rev 1.3   Sep 09 2005 16:01:32   dirby
   Modified to not send the 'escaped' values to Fraudguard.
   SCR 16789
   
      Rev 1.2   Aug 23 2005 16:54:14   dirby
   Modified to escape the angle brackets so they do not get converted
   by Microsoft at Fraudguard's end.
   SCR 16789
   
      Rev 1.1   Jul 19 2005 11:29:52   dirby
   Added code to speed up processing by not parsing XML tags
   that do not have a value.
   SCR 14421
   
      Rev 1.0   Jul 08 2005 09:15:46   dirby
   Initial revision.
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
#include "tx_dbstruct.h"
#include "ntutils.h"
#include "txutils.h"
#include "memmnger.h"
#include "fgrd_constants.h"
#include "fgrd_prototypes.h"
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
extern CHAR       dialog_que_name[];

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
extern BYTE   current_state[2];
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];

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
extern BYTE   Orig_Queue[ ];
extern BYTE   Orig_Info[];

extern INT       Card_Family;
extern INT       Authorizer;
extern INT       ConvRateFlag;
extern double    USD_to_Peso_Conv_Rate;
extern ISS_AMTS  Iss_Amts;

/* REQUEST BUILD BUFFER */
extern BYTE   ReqRsp_Buffer[];
extern BYTE   XML_ReqRsp_Buffer[];
extern INT    BufLen;


/******************************************************************************
 *
 *  NAME:         LOG_MESSAGE
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
void log_message( INT dest, INT sev, pCHAR msg, pCHAR func )
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]="";
   CHAR text_message[100]="";

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
   if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;

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

   /* Call function to post the message. */
   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
                                 eventlog_flag, msg_type, func,
                                 severity, log_type,
                                 Auth_Tx.TLF01_details.terminal_id,
                                 Auth_Tx.TLF01_details.card_num,
                                 Auth_Tx.TLF01_details.merchant_id );

   return;
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
void copy_to_req_resp_buffer( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA    p_msg_data  = NULL_PTR;
   pBYTE            p_data      = NULL_PTR;
   LONG             msglen;
   CHAR             buffer[100];

   memset( ReqRsp_Buffer, 0x00, COMM_BUF_SZE );

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   msglen     = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   if ( msglen > COMM_BUF_SZE )
   {
      sprintf( buffer,
              "Message from Fraudguard too large (%ld bytes). Truncating it.",
               msglen );
      log_message( 3, 2, buffer, "copy_to_req_resp_buffer" );
      msglen = COMM_BUF_SZE - 1;
   }

   memcpy(ReqRsp_Buffer, p_data, msglen);
   BufLen = msglen;

   return;
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
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,BufLen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         log_message( 2, 1, err_buf, "process_host_msg" );
         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         log_message( 2, 1, err_buf, "process_host_msg" );

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            log_message( 2, 1, err_buf, "process_host_msg" );
         }
      }
   }
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
INT determine_txn_flow( pCHAR transaction, INT source )
{
   INT  tran_flow;
   CHAR buffer[500] = "";

   /* In order to avoid searching the long string for a match that
    * might not be there, just search the first portion for a match.
    */
   memcpy( buffer, transaction, sizeof(buffer)-1 );

   if ( NULL != strstr(buffer, FG_RESULT_GROUP_TAG) )
   {
      /* This is a response.  Where did it come from? */
      if ( source == HOST )
         tran_flow = SWITCHED_OUT;
      else
         tran_flow = SWITCHED_IN;
   }
   else
   {
      /* This is a request from Fraudguard. */
      if ( source == HOST )
         tran_flow = SWITCHED_IN;
      else
         tran_flow = SWITCHED_OUT;
   }
   return( tran_flow );
}


/******************************************************************************
 *
 *  NAME:         PARSE_REQUEST_MSG
 *
 *  DESCRIPTION:  This function parses request messages from Fraudguard.
 *
 *  INPUTS:       txn - XML message
 *
 *  OUTPUTS:      Fguard - (Global) XML Transaction Structure
 *                len    - Number of bytes parsed
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_request_msg( pCHAR txn, pINT plen )
{
   INT   retval = false;
   pCHAR pTag = txn;
   CHAR  tag[50];
   CHAR  tag_value[200];

   while( pTag != NULL )
   {
      memset( tag,       0x00, sizeof(tag)       );
      memset( tag_value, 0x00, sizeof(tag_value) );

      pTag = get_next_tag( pTag, tag, tag_value );
      if ( pTag != NULL )
      {
         if ( 0 == strcmp(tag, FG_REQUEST_MSG_END_TAG) )
         {
            /* Done with this message. */
            *plen = pTag - txn;
            break;
         }
         else
         {
            copy_xml_to_fguard( tag, tag_value );
            pTag++;
         }
      }
   }

   if ( tag[0] != 0x00 )
   {
      /* This means we found some data somewhere in the loop. */
      retval = true;
   }
   return( retval );
}


/******************************************************************************
 *
 *  NAME:         PARSE_RESPONSE_MSG
 *
 *  DESCRIPTION:  This function parses response messages from Fraudguard.
 *
 *  INPUTS:       txn - XML message
 *
 *  OUTPUTS:      Fguard - (Global) XML Transaction Structure
 *                len    - Number of bytes parsed
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_response_msg( pCHAR txn, pINT plen )
{
   INT   retval = false;
   pCHAR pTag = txn;
   pCHAR pMsgEnd = FG_RESULT_END_TAG + 1;
   CHAR  tag[50];
   CHAR  tag_value[200];

   *plen = 0;
   while( pTag != NULL )
   {
      memset( tag,       0x00, sizeof(tag)       );
      memset( tag_value, 0x00, sizeof(tag_value) );

      pTag = get_next_tag( pTag, tag, tag_value );
      if ( pTag != NULL )
      {
         if ( 0 == strcmp(tag, pMsgEnd) )
         {
            /* Done with this message. */
            break;
         }
         else
         {
            if ( tag_value[0] != 0x00 )
            {
               copy_xml_to_fguard( tag, tag_value );
            }
            pTag++;
         }
      }
   }

   /* Calculate number of bytes that have been processed. */
   pTag = strstr( txn, FG_RESULT_END_TAG );
   if ( pTag == NULL )
   {
      /* End tag is missing. Error out. */
      retval = false;
   }
   else
   {
      *plen = pTag - txn + strlen(FG_RESULT_END_TAG);
      retval = true;
   }
   return( retval );
}


/******************************************************************************
 *
 *  NAME:         GET_NEXT_TAG
 *
 *  DESCRIPTION:  This function gets the first XML tag label from a string.
 *
 *  INPUTS:       txn - XML message
 *
 *  OUTPUTS:      tag       - The tag without opening bracket
 *                tag_value - Value between start and end tags
 *
 *  RTRN VALUE:   Pointer to 'tag' (including opening bracket)
 *                NULL if no tag found or if tag or tag_value not found
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
pCHAR get_next_tag( pCHAR p_txn, pCHAR tag, pCHAR tag_value )
{
   pCHAR  p_tag   = NULL;
   pCHAR  p_temp1 = NULL;
   pCHAR  p_temp2 = NULL;

   p_tag = strchr( p_txn, '<' );
   if ( NULL != p_tag )
   {
      if ( p_tag[1] == '/' )
      {
         /* This is an end tag. Do not want it. Get next tag. */
         p_tag = strchr( p_tag+1, '<' );
      }

      if ( NULL != p_tag )
      {
         /* Get Tag Name */
         p_temp1 = strchr(p_tag, '>');
         if ( p_temp1 == NULL )
         {
            p_tag = NULL;
         }
         else
         {
            memcpy( tag, p_tag+1, p_temp1 - p_tag );

            /* Get Tag Value */
            p_temp1++;
            p_temp2 = strchr(p_temp1, '<');
            if ( p_temp2 == NULL )
            {
               p_tag = NULL;
            }
            else
            {
               memcpy( tag_value, p_temp1, p_temp2 - p_temp1 );
            }
         }
      }
   }

   return( p_tag );
}


/******************************************************************************
 *
 *  NAME:         COPY_XML_TO_FGUARD
 *
 *  DESCRIPTION:  This function will copy the value of a tag into
 *                the FGUARD data structure.  It will not overwrite
 *                an existing value.
 *
 *  INPUTS:       tag       - The tag without brackets
 *                tag_value - Value between start and end tags
 *
 *  OUTPUTS:      Fguard - (Global) XML Transaction Structure
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void copy_xml_to_fguard( pCHAR tag, pCHAR tag_value )
{
   INT    i;
   INT    len;
   double amount;
   CHAR   tempstr[200];

   if ( 0 == strcmp(tag, FG_TAG_TIMESTAMP) )
   {
      if ( Fguard.timestamp[0] == 0x00 )
         strcpy( Fguard.timestamp, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I000) )
   {
      if ( Fguard.message_type[0] == 0x00 )
         strcpy( Fguard.message_type, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I002) )
   {
      if ( Fguard.card_num[0] == 0x00 )
         strcpy( Fguard.card_num, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I003) )
   {
      if ( Fguard.processing_code[0] == 0x00 )
         strcpy( Fguard.processing_code, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I004) )
   {
      if ( Fguard.amount_peso[0] == 0x00 )
      {
         amount = strtod( tag_value, 0 ) * 100.0;
         sprintf( Fguard.amount_peso, "%012.0f", amount );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_I006) )
   {
      if ( Fguard.amount_usd[0] == 0x00 )
      {
         amount = strtod( tag_value, 0 ) * 100.0;
         sprintf( Fguard.amount_usd, "%012.0f", amount );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_I007) )
   {
      if ( Fguard.gmt[0] == 0x00 )
         strcpy( Fguard.gmt, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I011) )
   {
      if ( Fguard.sys_trace_audit_num[0] == 0x00 )
         strcpy( Fguard.sys_trace_audit_num, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I018) )
   {
      if ( Fguard.mcc[0] == 0x00 )
         strcpy( Fguard.mcc, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I019) )
   {
      if ( Fguard.country_code[0] == 0x00 )
         strcpy( Fguard.country_code, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I022) )
   {
      if ( Fguard.pos_entry_mode[0] == 0x00 )
         strcpy( Fguard.pos_entry_mode, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I025) )
   {
      if ( Fguard.pos_condition_code[0] == 0x00 )
         strcpy( Fguard.pos_condition_code, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I032) )
   {
      if ( Fguard.acquiring_id[0] == 0x00 )
         strcpy( Fguard.acquiring_id, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I038) )
   {
      if ( Fguard.auth_number[0] == 0x00 )
         strcpy( Fguard.auth_number, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I039) )
   {
      if ( Fguard.response_code[0] == 0x00 )
         strcpy( Fguard.response_code, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I041) )
   {
      if ( Fguard.terminal_id[0] == 0x00 )
         strcpy( Fguard.terminal_id, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I042) )
   {
      if ( Fguard.merchant_id[0] == 0x00 )
         strcpy( Fguard.merchant_id, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I043a) )
   {
      if ( Fguard.card_acceptor_name[0] == 0x00 )
      {
         memset( tempstr, 0x00, sizeof(tempstr) );
         unconvert_xml_chars( tag_value, tempstr );
         strcpy( Fguard.card_acceptor_name, tempstr );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_I043b) )
   {
      if ( Fguard.card_acceptor_city[0] == 0x00 )
      {
         memset( tempstr, 0x00, sizeof(tempstr) );
         unconvert_xml_chars( tag_value, tempstr );
         strcpy( Fguard.card_acceptor_city, tempstr );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_I043c) )
   {
      if ( Fguard.card_acceptor_country[0] == 0x00 )
      {
         memset( tempstr, 0x00, sizeof(tempstr) );
         unconvert_xml_chars( tag_value, tempstr );
         strcpy( Fguard.card_acceptor_country, tempstr );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_I049) )
   {
      if ( Fguard.currency_code_txn[0] == 0x00 )
         strcpy( Fguard.currency_code_txn, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I051) )
   {
      if ( Fguard.currency_code_billing[0] == 0x00 )
         strcpy( Fguard.currency_code_billing, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_I060) )
   {
      if ( Fguard.pos_info[0] == 0x00 )
         strcpy( Fguard.pos_info, tag_value );
   }
   else if ( 0 == strcmp(tag, FG_TAG_RESULT_CODE) )
   {
      if ( Fguard.result_code[0] == 0x00 )
      {
         strcpy( Fguard.result_code, tag_value );
         len = strlen( tag_value );
         for( i=0; i<len; i++ )
            Fguard.result_code[i] = toupper( Fguard.result_code[i] );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_APPLICATION) )
   {
      if ( Fguard.application[0] == 0x00 )
      {
         len = strlen( tag_value );
         if ( len > (sizeof(Fguard.application)-1) )
            len = sizeof(Fguard.application)-1;
         memcpy( Fguard.application, tag_value, len );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_RULE_DOMAIN) )
   {
      if ( Fguard.rule_domain[0] == 0x00 )
      {
         len = strlen( tag_value );
         if ( len > (sizeof(Fguard.rule_domain)-1) )
            len = sizeof(Fguard.rule_domain)-1;
         memcpy( Fguard.rule_domain, tag_value, len );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_CALLER_ID) )
   {
      if ( Fguard.caller_id[0] == 0x00 )
      {
         len = strlen( tag_value );
         if ( len > (sizeof(Fguard.caller_id)-1) )
            len = sizeof(Fguard.caller_id)-1;
         memcpy( Fguard.caller_id, tag_value, len );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_CALLER_MSG_ID) )
   {
      if ( Fguard.caller_msg_id[0] == 0x00 )
      {
         len = strlen( tag_value );
         if ( len > (sizeof(Fguard.caller_msg_id)-1) )
            len = sizeof(Fguard.caller_msg_id)-1;

         memcpy( Fguard.caller_msg_id,  tag_value, len );
         memcpy( Fguard.transaction_id, tag_value, len );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_RESULT_TEXT) )
   {
      if ( Fguard.result_text[0] == 0x00 )
      {
         len = strlen( tag_value );
         if ( len > (sizeof(Fguard.result_text)-1) )
            len = sizeof(Fguard.result_text)-1;
         memcpy( Fguard.result_text, tag_value, len );
      }
   }
   else if ( 0 == strcmp(tag, FG_TAG_PRODUCT) )
   {
      if ( Fguard.product[0] == 0x00 )
      {
         len = strlen( tag_value );
         if ( len > (sizeof(Fguard.product)-1) )
            len = sizeof(Fguard.product)-1;
         memcpy( Fguard.product, tag_value, len );
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         POPULATE_FGRAW
 *
 *  DESCRIPTION:  This function populates the FG_RAW structure.
 *
 *  INPUTS:       p_txn         - Pointer in ReqRsp_Buffer to txn
 *                txn_type      - FG_REQUEST or FG_RESPONSE
 *                Fguard        - (Global) XML Transaction Structure
 *                ReqRsp_Buffer - (Global) Raw data message
 *
 *  OUTPUTS:      Fgraw - (Global) Raw data structure for the database
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void populate_fgraw( pCHAR p_txn, INT txn_type )
{
   LONG  len;
   pCHAR txn_end     = NULL;
   CHAR  end_tag[30] = "";

   memset( &Fgraw, 0x00, sizeof(FG_RAW) );
   strcpy( Fgraw.transaction_id, Fguard.transaction_id );

   if ( txn_type == FG_RESPONSE )
      strcpy( end_tag, FG_RESULT_END_TAG );
   else
      strcpy( end_tag, FG_REQUEST_MSG_END_TAG );

   /* Find transaction ending tag. */
   txn_end = strstr( p_txn, end_tag );
   if ( txn_end != NULL )
   {
      /* Get length of transaction = end - start + tag_length. */
      len = txn_end - p_txn + strlen(end_tag);
   }
   else
   {
      /* End tag not found.  Log whole message. */
      len = strlen( p_txn );
   }

   sprintf( Fgraw.length, "%ld", len );
   if ( len > (sizeof(Fgraw.raw_data)-1) )
      len = sizeof(Fgraw.raw_data)-1;

   memcpy( Fgraw.raw_data, p_txn, len );

   return;
}


/******************************************************************************
 *
 *  NAME:         POPULATE_FGUARD_REQUEST
 *
 *  DESCRIPTION:  This function will populate FGUARD with values from Auth_Tx.
 *
 *  INPUTS:       Auth_Tx - (Global) ATP Transaction Structure
 *
 *  OUTPUTS:      Fguard - (Global) XML Transaction Structure
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT populate_fguard_request()
{
   INT   retval       = true;
   BYTE  peso_amt[13] = "";
   BYTE  usd_amt [13] = "";
   CHAR  gmt_time[15];

   memset( &Fguard, 0x00, Fguard_Size );
   create_timestamp( Fguard.timestamp );

   memcpy( Fguard.message_type,    Auth_Tx.TLF01_details.message_type,     4 );
   memcpy( Fguard.card_num,        Auth_Tx.TLF01_details.card_num,        19 );
   memcpy( Fguard.processing_code, Auth_Tx.TLF01_details.processing_code,  6 );

   /* Continue if error so txn can be logged into database. */
   if ( Authorizer == HOST )
   {
      retval = get_amounts( peso_amt, usd_amt );
      if ( retval == true )
      {
         memcpy( Fguard.amount_peso, peso_amt, 12 );
         memcpy( Fguard.amount_usd,  usd_amt,  12 );
      }
   }
   else
   {
      /* Get the Issuer Amounts */
      get_field4_amount();
      get_field6_amount();
      get_amt_center();
      get_amt_card();
      get_otb_amt_card();
      get_country_code();
      get_field25();
   }

   if ( Auth_Tx.TLF01_details.transmission_timestamp[0] == 0x00 )
   {
      memset( gmt_time, 0x00, sizeof(gmt_time) );

      /* Get Greenwich Mean Time - YYYYMMDDhhmmss */
      pintime_get_gmt( gmt_time );

      /* MMDDhhmmss  */
      memcpy( Fguard.gmt, &gmt_time[4], 10 );
   }
   else
      strcpy( Fguard.gmt, Auth_Tx.TLF01_details.transmission_timestamp );

   strcpy( Fguard.sys_trace_audit_num,   Auth_Tx.TLF01_details.sys_trace_audit_num);
   strcpy( Fguard.mcc,                   Auth_Tx.TLF01_details.category_code );
   strcpy( Fguard.country_code,          Auth_Tx.country_code );
   strcpy( Fguard.pos_entry_mode,        Auth_Tx.TLF01_details.pos_entry_mode );
   strcpy( Fguard.pos_condition_code,    Auth_Tx.TLF01_details.pos_condition_code );

   if ( Auth_Tx.TLF01_details.acquiring_id[0] == 0x00 )
   {
      /* This field is required, so if empty, use values given by EB. */
      if ( Auth_Tx.TLF01_details.card_num[0] == '4' )
         strcpy( Fguard.acquiring_id, "492164" );

      else if ( Auth_Tx.TLF01_details.card_num[0] == '5' )
      {
         if ( Authorizer == HOST )
            strcpy( Fguard.acquiring_id, "001788" );  // Acquirer
         else
            strcpy( Fguard.acquiring_id, "541788" );  // Issuer
      }
      else if ( Auth_Tx.TLF01_details.card_num[0] == '3' )
      {
         /* First digit of card number is 3. */
         if ( Auth_Tx.TLF01_details.card_num[1] == '5' )
         {
            /* Second digit of card number is 5 (JCB). */
            strcpy( Fguard.acquiring_id, "356277" );
         }
         else if ( (Auth_Tx.TLF01_details.card_num[1] == '4') ||
                   (Auth_Tx.TLF01_details.card_num[1] == '7')  )
         {
            /* Second digit of card number if 4 or 7 (Amex). */
            strcpy( Fguard.acquiring_id, "377670" );
         }
      }
   }
   else
   {
      strcpy( Fguard.acquiring_id, Auth_Tx.TLF01_details.acquiring_id );
   }

   strcpy( Fguard.auth_number,   Auth_Tx.TLF01_details.auth_number   );
   strcpy( Fguard.response_code, Auth_Tx.TLF01_details.response_code );
   strcpy( Fguard.terminal_id,   Auth_Tx.TLF01_details.terminal_id   );
   strcpy( Fguard.merchant_id,   Auth_Tx.TLF01_details.merchant_id   );

   if ( Auth_Tx.MCF01_details.name02[0] == 0x00 )
      memcpy( Fguard.card_acceptor_name,
              Auth_Tx.TLF01_details.card_holder_name,
              sizeof(Fguard.card_acceptor_name)-1 );
   else
      memcpy( Fguard.card_acceptor_name,
              Auth_Tx.MCF01_details.name02,
              sizeof(Fguard.card_acceptor_name)-1 );

   memcpy( Fguard.card_acceptor_city,    Auth_Tx.TLF01_details.ncf01_retired_cwk, 13);
   memcpy( Fguard.card_acceptor_country, Auth_Tx.TLF01_details.ncf01_retired_cwk+13,3);

   if ( Authorizer == HOST )
   {
      strcpy( Fguard.currency_code_txn,     PESO );
      strcpy( Fguard.currency_code_billing, USD  );
   }
   else
   {
      get_fields_49_and_51();
      strcpy( Fguard.currency_code_txn,     Iss_Amts.f49 );
      strcpy( Fguard.currency_code_billing, Iss_Amts.f51 );
   }
   strcpy( Fguard.pos_info,       Auth_Tx.TLF01_details.nfi_name );
   strcpy( Fguard.caller_msg_id,  Auth_Tx.TLF01_details.primary_key.transaction_id );
   strcpy( Fguard.transaction_id, Fguard.caller_msg_id );

   /* Configuration Info */
   strcpy( Fguard.caller_id,   Fg01.caller_id   );
   strcpy( Fguard.application, Fg01.application );
   strcpy( Fguard.rule_domain, Fg01.rule_domain );
   strcpy( Fguard.product,     Fg01.product     );

   return( retval );
}


/******************************************************************************
 *
 *  NAME:         CREATE_XML_REQUEST
 *
 *  DESCRIPTION:  This function will create a single transaction XML request
 *                message to be sent to Fraudguard.
 *
 *  INPUTS:       Fguard - (Global) XML Transaction Structure
 *                Fg01   - (Global) Fguard configuration information
 *
 *  OUTPUTS:      ReqRsp_Buffer - (Global) Formatted message for Fraudguard
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_xml_request()
{
   CHAR  tempstr[200];

   memset( ReqRsp_Buffer, 0x00, COMM_BUF_SZE );
   strcpy( ReqRsp_Buffer, FG_MESSAGE_GROUP   );
   strcat( ReqRsp_Buffer, FG_REQUEST_MSG_TAG );

   populate_tag( FG_TAG_CALLER_ID,     Fg01.caller_id,        TAG_MAN );
   populate_tag( FG_TAG_CALLER_MSG_ID, Fguard.transaction_id, TAG_MAN );

   if ( Authorizer == HOST )
   {
      /* Off Us Transactions = Acquirer */
      populate_tag( FG_TAG_SOURCE,      Fg01.source,                TAG_MAN );
      populate_tag( FG_TAG_USERNAME,    Fg01.username ,             TAG_MAN );
      populate_tag( FG_TAG_APPLICATION, Fg01.application,           TAG_MAN );
      populate_tag( FG_TAG_RULE_DOMAIN, Fg01.rule_domain,           TAG_MAN );
      populate_tag( FG_TAG_PRODUCT,     Fg01.product,               TAG_MAN );
      populate_tag( FG_TAG_PRIORITY,    Fg01.priority,              TAG_MAN );
      populate_tag( FG_TAG_TIMESTAMP,   Fguard.timestamp,           TAG_MAN );
      populate_tag( FG_TAG_I000,        Fguard.message_type,        TAG_MAN );
      populate_tag( FG_TAG_I002,        Fguard.card_num,            TAG_MAN );
      populate_tag( FG_TAG_I003,        Fguard.processing_code,     TAG_OPT );
      populate_tag( FG_TAG_I004,        Fguard.amount_peso,         TAG_OPT );
      populate_tag( FG_TAG_I006,        Fguard.amount_usd,          TAG_OPT );
      populate_tag( FG_TAG_I007,        Fguard.gmt,                 TAG_OPT );
      populate_tag( FG_TAG_I011,        Fguard.sys_trace_audit_num, TAG_OPT );
      populate_tag( FG_TAG_I018,        Fguard.mcc,                 TAG_MAN );
      populate_tag( FG_TAG_I019,        Fguard.country_code,        TAG_OPT );
      populate_tag( FG_TAG_I022,        Fguard.pos_entry_mode,      TAG_OPT );
      populate_tag( FG_TAG_I025,        Fguard.pos_condition_code,  TAG_OPT );
      populate_tag( FG_TAG_I032,        Fguard.acquiring_id,        TAG_MAN );
      populate_tag( FG_TAG_I038,        Fguard.auth_number,         TAG_OPT );
      populate_tag( FG_TAG_I039,        Fguard.response_code,       TAG_MAN );
      populate_tag( FG_TAG_I041,        Fguard.terminal_id,         TAG_OPT );
      populate_tag( FG_TAG_I042,        Fguard.merchant_id,         TAG_OPT );

      /* Check for special XML characters and convert so XML can read them. */
      memset( tempstr, 0x00, sizeof(tempstr) );
      convert_xml_chars( Fguard.card_acceptor_name, tempstr );
      populate_tag( FG_TAG_I043a,         tempstr,                      TAG_OPT );

      memset( tempstr, 0x00, sizeof(tempstr) );
      convert_xml_chars( Fguard.card_acceptor_city, tempstr );
      populate_tag( FG_TAG_I043b,         tempstr,                      TAG_OPT );

      memset( tempstr, 0x00, sizeof(tempstr) );
      convert_xml_chars( Fguard.card_acceptor_country, tempstr );
      populate_tag( FG_TAG_I043c,         tempstr,                      TAG_OPT );

      populate_tag( FG_TAG_I049,          Fguard.currency_code_txn,     TAG_OPT );
      populate_tag( FG_TAG_I051,          Fguard.currency_code_billing, TAG_OPT );
      populate_tag( FG_TAG_I060,          Fguard.pos_info,              TAG_OPT );
   }
   else
   {
      /* On Us Transactions = Issuer */
      populate_tag( FG_TAG_SOURCE,      "",                            TAG_MAN);
      populate_tag( FG_TAG_USERNAME,    "",                            TAG_MAN);
      populate_tag( FG_TAG_APPLICATION, "FRAUDI",                      TAG_MAN);
      populate_tag( FG_TAG_RULE_DOMAIN,  Fg01.rule_domain,             TAG_MAN);
      populate_tag( FG_TAG_PRODUCT,      Fg01.product,                 TAG_MAN);
      populate_tag( FG_TAG_PRIORITY,     Fg01.priority,                TAG_MAN);
      populate_tag( FG_TAG_AMT_CENTER,   Iss_Amts.amt_center,          TAG_OPT);
      populate_tag( FG_TAG_AMT_CARD,     Iss_Amts.amt_card,            TAG_OPT);
      populate_tag( FG_TAG_OTB_AMT_CARD, Iss_Amts.otb_amt_card,        TAG_OPT);
      populate_tag( FG_TAG_TIMESTAMP,    Fguard.timestamp,             TAG_MAN);
      populate_tag( FGi_TAG_I000,        Fguard.message_type,          TAG_MAN);
      populate_tag( FGi_TAG_I002,        Fguard.card_num,              TAG_MAN);
      populate_tag( FGi_TAG_I003,        Fguard.processing_code,       TAG_OPT);
      populate_tag( FGi_TAG_I004,        Iss_Amts.f4,                  TAG_OPT);
      populate_tag( FGi_TAG_I006,        Iss_Amts.f6,                  TAG_OPT);
      populate_tag( FGi_TAG_I007,        Fguard.gmt,                   TAG_OPT);
      populate_tag( FGi_TAG_I011,        Fguard.sys_trace_audit_num,   TAG_OPT);
      populate_tag( FGi_TAG_I018,        Fguard.mcc,                   TAG_MAN);
      populate_tag( FGi_TAG_I019,        Iss_Amts.country_code,        TAG_OPT);
      populate_tag( FGi_TAG_I022,        Fguard.pos_entry_mode,        TAG_OPT);
      populate_tag( FGi_TAG_I025,        Iss_Amts.f25,                 TAG_MAN);
      populate_tag( FGi_TAG_I032,        Fguard.acquiring_id,          TAG_MAN);
      populate_tag( FGi_TAG_I038,        Fguard.auth_number,           TAG_OPT);
      populate_tag( FGi_TAG_I039,        Fguard.response_code,         TAG_MAN);
      populate_tag( FGi_TAG_I041,        Fguard.terminal_id,           TAG_OPT);
      populate_tag( FGi_TAG_I042,        Fguard.merchant_id,           TAG_OPT);

      /* Check for special XML characters and convert so XML can read them. */
      memset( tempstr, 0x00, sizeof(tempstr) );
      convert_xml_chars( Fguard.card_acceptor_name, tempstr );
      populate_tag( FGi_TAG_I043a,       tempstr,                      TAG_MAN);

      memset( tempstr, 0x00, sizeof(tempstr) );
      convert_xml_chars( Fguard.card_acceptor_city, tempstr );
      populate_tag( FGi_TAG_I043b,       tempstr,                      TAG_MAN);

      memset( tempstr, 0x00, sizeof(tempstr) );
      if ( Fguard.card_acceptor_country[0] == 0x00 )
         if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
            strcpy( Fguard.card_acceptor_country, "PHL" );

      convert_xml_chars( Fguard.card_acceptor_country, tempstr );
      populate_tag( FGi_TAG_I043c,       tempstr,                      TAG_MAN);

      populate_tag( FGi_TAG_I049,        Fguard.currency_code_txn,     TAG_OPT);
      populate_tag( FGi_TAG_I051,        Fguard.currency_code_billing, TAG_OPT);
      populate_tag( FGi_TAG_I060,        Fguard.pos_info,              TAG_OPT);
   }

   strcat( ReqRsp_Buffer, FG_REQUEST_MSG_END_TAG );
   strcat( ReqRsp_Buffer, FG_MESSAGE_GROUP_END   );

   BufLen = strlen( ReqRsp_Buffer );
   PRINT( ReqRsp_Buffer );
   return;
}


/******************************************************************************
 *
 *  NAME:         CREATE_TIMESTAMP
 *
 *  DESCRIPTION:  This function uses the system time to create a time
 *                stamp in Fraudguard format.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      timestamp  : Format = YYYY-MM-DDTHH:MM:SS
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_timestamp( pBYTE timestamp )
{
   CHAR time_date[25] = "";

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   memcpy( timestamp, time_date, 19 );

   timestamp[10] = 'T';
   timestamp[13] = ':';
   timestamp[16] = ':';

   return;
}


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
 *  NAME:         GET_AMOUNTS
 *
 *  DESCRIPTION:  This procedure takes the transaction amount from Auth_tx
 *                and calculates (converts) it to Philippino Peso.  It and
 *                the USD amount used to convert it are returned.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction Structure
 *
 *  OUTPUTS:      peso - Amount in Peso
 *                usd  - Amount in U.S. Dollar
 *
 *  RTRN VALUE:   True if conversion is successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_amounts( pCHAR peso, pCHAR usd )
{
   INT    retval = true;
   double d_peso;
   double d_usd;
   CHAR   peso_conv[10] = "";

   if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
   {
      memcpy( peso, Auth_Tx.TLF01_details.total_amount, 12 );
      if ( Auth_Tx.TLF01_details.settlement_total[0] != 0x00 )
         memcpy( usd, Auth_Tx.TLF01_details.settlement_total, 12 );

      else if ( Auth_Tx.ch_billing_amt[0] != 0x0 )
         memcpy( usd, Auth_Tx.ch_billing_amt, 12 );

      else
      {
         /* Must convert Peso to get USD amount. */
         retval = currency_conversion( peso, PESO, TO_USD, usd );
      }
   }
   else if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, USD) )
   {
      /* Need to convert USD to get PESO amount. */
      memcpy( usd, Auth_Tx.TLF01_details.total_amount, 12 );
      retval = currency_conversion( usd, PESO, FROM_USD, peso );
   }
   else
   {
      /* Currency is neither PESO nor USD.  Get USD amount
       * and convert it to PESO.
       */
      if ( Auth_Tx.TLF01_details.settlement_total[0] != 0x00 )
         memcpy( usd, Auth_Tx.TLF01_details.settlement_total, 12 );

      else if ( Auth_Tx.ch_billing_amt[0] != 0x00 )
         memcpy( usd, Auth_Tx.ch_billing_amt, 12 );

      else
         memcpy( usd, Auth_Tx.TLF01_details.total_amount, 12 );

      retval = currency_conversion( usd, PESO, FROM_USD, peso );
   }

   /* Remove leading zeroes and insert decimal point. */
   d_peso = strtod( peso, 0 ) / 100.0;
   d_usd  = strtod( usd,  0 ) / 100.0;

   sprintf( peso, "%0.2f", d_peso );
   sprintf( usd,  "%0.2f", d_usd  );

   return( retval );
}


/******************************************************************************
 *
 *  NAME:         CURRENCY_CONVERSION
 *
 *  DESCRIPTION:  This procedure converts an amount into a USD amount OR
 *                it converts a USD amount into a different currency.
 *
 *  INPUTS:       in_amount = Amount to be converted
 *                curr_code = Currency Code to be converted
 *                direction = TO_USD or FROM_USD
 *
 *  OUTPUTS:      out_amount = Amount after it is converted
 *
 *  RTRN VALUE:   True if conversion is successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT currency_conversion( pCHAR in_amount, pCHAR curr_code,
                         INT   direction, pCHAR out_amount )
{
   INT    retval = true;
   LONG   returncode;
   LONG   l_amount;
   double d_amount;
   double conv_rate;
   CHAR   errbuf[100];

   memset( &Crf01, 0x00, Crf01_Size );
   memcpy( Crf01.primary_key.currency_code, curr_code, 3 );
   returncode = FindMemRecord( (pBYTE)&Crf01,
                                sizeof(Crf01.primary_key.currency_code),
                                Crf01_Size,
                               "Crf01Table" );

   if ( returncode < 0 )
   {
      /* There is either a problem with shared
       * memory or the record is missing.
       */
      memset( errbuf, 0x00, sizeof(errbuf) );
      sprintf( errbuf, "Unable to convert between %s and USD", curr_code );
      log_message( 3, 3, errbuf, "currency_conversion" );
      retval = false;
   }
   else
   {
      conv_rate = strtod( Crf01.conversion_rate, 0 );
      if ( conv_rate < 0.0 )
         conv_rate = 1.0;
      else
      {
         USD_to_Peso_Conv_Rate = conv_rate;
         ConvRateFlag = true;
      }

      l_amount = atol( in_amount );
      if ( direction == TO_USD )
         d_amount = ((double)l_amount / conv_rate);
      else
         d_amount = ((double)l_amount * conv_rate);
      l_amount = (LONG)d_amount;
      sprintf( out_amount, "%0ld", l_amount );
   }
   return( retval );
}


/******************************************************************************
 *
 *  NAME:         POPULATE_TAG
 *
 *  DESCRIPTION:  This function will put a start tag, its value, and the
 *                corresponding end tag into ReqRsp_Buffer at the next
 *                null position.
 *
 *  INPUTS:       tag       - Name of the tag
 *                tag_value - Value of the tag
 *                indicator - TAG_MAN or TAG_OPT (mandatory or optional)
 *
 *  OUTPUTS:      ReqRsp_Buffer - (Global) Formatted message for Fraudguard
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void populate_tag( pCHAR tag, pCHAR tag_value, INT indicator )
{
   if ( (indicator == TAG_MAN) || (tag_value[0] != 0x00) )
   {
      strcat( ReqRsp_Buffer, "<"       );
      strcat( ReqRsp_Buffer, tag       );
      strcat( ReqRsp_Buffer, tag_value );
      strcat( ReqRsp_Buffer, "</"      );
      strcat( ReqRsp_Buffer, tag       );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         CONVERT_XML_CHARS
 *
 *  DESCRIPTION:  This function searches a string for XML special characters
 *                such as '&' and converts them into a format XML will
 *                recognized.
 *
 *                Example:  instr:  "ABC & 123 Housing"
 *                          outstr: "ABC &amp; 123 Housing"
 *
 *                          & is converted into &amp;
 *
 *  INPUTS:       instr - String to be searched
 *
 *  OUTPUTS:      outstr - Converted string.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void convert_xml_chars( pCHAR instr, pCHAR outstr )
{
   INT  in, out;
   INT  len;
   CHAR tempstr[200] = "";

   len = strlen( instr );
   for ( in=0,out=0; in<len; in++ )
   {
      if ( instr[in] == '&' )
      {
         strcat( tempstr, "&amp;" );
         out += 5;
      }
      else if ( instr[in] == '<' )
      {
         strcat( tempstr, "&lt;" );
         out += 4;
      }
      else if ( instr[in] == '>' )
      {
         strcat( tempstr, "&gt;" );
         out += 4;
      }
      else if ( instr[in] == 0x22 )  /* Double Quote */
      {
         strcat( tempstr, "&quot;" );
         out += 6;
      }
      else if ( instr[in] == 0x27 )  /* Single Quote */
      {
         strcat( tempstr, "&#39;" );
         out += 5;
      }
      else
      {
         tempstr[out++] = instr[in];
      }
   }
   strcpy( outstr, tempstr );
   return;
}


/******************************************************************************
 *
 *  NAME:         UNCONVERT_XML_CHARS
 *
 *  DESCRIPTION:  This function searches a string for XML special characters
 *                such as '&amp;' that have been converted and so converts them
 *                into a regular ASCII character.  This function undo's what
 *                function convert_xml_chars does.
 *
 *                Example:  instr:  "ABC &amp; 123 Housing"
 *                          outstr: "ABC & 123 Housing"
 *
 *                          &amp; is converted into &
 *
 *  INPUTS:       instr - String to be searched
 *
 *  OUTPUTS:      outstr - Converted string.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void unconvert_xml_chars( pCHAR instr, pCHAR outstr )
{
   INT  in, out;
   INT  len;
   CHAR tempstr[200] = "";

   len = strlen( instr );
   for ( in=0,out=0; in<len; out++ )
   {
      if ( instr[in] == '&' )
      {
         if ( 0 == strncmp( &instr[in], "&amp;", 5) )
         {
            tempstr[out] = '&';
            in += 5;
         }
         else if ( 0 == strncmp( &instr[in], "&lt;", 4) )
         {
            tempstr[out] = '<';
            in += 4;
         }
         else if ( 0 == strncmp( &instr[in], "&gt;", 4) )
         {
            tempstr[out] = '>';
            in += 4;
         }
         else if ( 0 == strncmp( &instr[in], "&quot;", 6) )
         {
            tempstr[out] = 0x22;
            in += 6;
         }
         else if ( 0 == strncmp( &instr[in], "&#39;", 5) )
         {
            tempstr[out] = 0x27;
            in += 5;
         }
         else
         {
            tempstr[out] = instr[in++];
         }
      }
      else
      {
         tempstr[out] = instr[in++];
      }
   }
   strcpy( outstr, tempstr );
   return;
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
   pPTE_MSG    p_msg    = NULL_PTR;
   INT         ret_code = true;

   /* Verify message size. */
   if ( length < 0 )
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
            BufLen = 350;
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
 *  NAME:         SEND_TO_FRAUDGUARD
 *
 *  DESCRIPTION:  This function sends a message to the fraudguard host.
 *
 *  INPUTS:       ReqRsp_Buffer   - (Global)Data stream going to the host
 *                buflen          - (Global)Length of ReqRsp_Buffer
 *                Fg01.tpdu_id    - (Global)Comm info
 *                dialog_que_name - (Global)Queue name of dialog service
 *
 *  OUTPUTS:      err_buf - Text message if an error occurs
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_to_fraudguard( pCHAR err_buf )
{
   INT    ret_val;
   INT    XML_BufLen = 0;
   BYTE   tpdu[12]   = "";
   CHAR   buffer[100]= "";

   /* Wait a small amount of time between 2 consecutive txns.
    * This is because the FSoap connection doesn't seem to
    * be able to handle heavy volume.
    */
   #ifdef WIN32
      Sleep( 30 );      // 75 msecs = 0.075 seconds, Changed to 30 msec -TF Phani
   #else
      usleep( 30000 );  // 75 msecs = 0.075 seconds,Changed to 30 msec -TF Phani
   #endif

   /* Route based on Issuer or Acquirer */
   strcpy( tpdu, Fg01.tpdu_id );
   if ( Authorizer == ATP )
      strcat( tpdu, "I" );

   XML_BufLen = escape_angle_brackets( ReqRsp_Buffer, XML_ReqRsp_Buffer );
   ret_val = build_and_send_request_message( MT_OUTGOING,
                                             ST1_NONE,
                                             ST2_NONE,
                                             dialog_que_name,
                                           /*  XML_ReqRsp_Buffer, */
                                           /*  XML_BufLen, */
                                             ReqRsp_Buffer,
                                             BufLen,
                                             0,
                                             tpdu,
                                             buffer );

   if ( ret_val == false )
   {
      sprintf( err_buf,
              "Unable to send request msg to Fraudguard host. Err: %s",
               buffer );
   }
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         GENERATE_TRAN_ID
 *
 *  DESCRIPTION:  This function creates a unique transaction Id.  It uses
 *                time + a three character counter which is stored in the
 *                active txn counter field in shared memory.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      tran_id - Unique transaction identifier
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void generate_tran_id( pBYTE tran_id )
{
   INT   retval;
   INT   temp_ctr;
   BYTE  counter[4];
   BYTE  timeout_cnt[4];
   BYTE  host_state[2];
   CHAR  err_buf[100] = "";
   CHAR  unique_tran_str_id[20] = "";

   /* Get time in format:  ssssssssss.jjj
    *  where:
    *     ssssssssss  = seconds since 1-1-1970.
    *     jjj         = milliseconds after the second
    */
   ptetime_get_strtime( unique_tran_str_id );

   /* Read the current values in order to echo back the counts. */
   retval = read_shared_mem_table( timeout_cnt, counter,
                                   host_state,  err_buf );

   temp_ctr = atoi( counter ) + 1;
   sprintf( counter, "%03d", temp_ctr );

   strcat( unique_tran_str_id, counter );

   /* Update the counter in shared memory. Ignore errors. */
   retval = write_to_shared_mem_table( timeout_cnt, counter,
                                       host_state,  err_buf );
   strcpy( tran_id, unique_tran_str_id );
   return;
}

/******************************************************************************
 *
 *  NAME:         REMOVE_LEADING_ZEROES
 *
 *  DESCRIPTION:  This function strips a string of any leading zeroes. If
 *                there are any leading zeroes, they are removed, and all
 *                remaining characters are shifted left.
 *
 *  INPUTS:       string - String to be stripped
 *
 *  OUTPUTS:      string - Stripped string
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void remove_leading_zeroes( pCHAR string )
{
   INT  loc;
   INT  len;

   if ( string[0] == '0' )
   {
      len = strlen( string );
      loc = strspn( string, "0" );
      if ( loc < len )
         strcpy( string, string+loc );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         ESCAPE_ANGLE_BRACKETS
 *
 *  DESCRIPTION:  This function takes a string in and converts each left
 *                angle bracket ('<') to an XML escape sequence representing
 *                '<'.  Does the same for the right angle bracket.
 *
 *                < = &#60
 *                > = &#62
 *
 *  INPUTS:       strin  - Input string
 *
 *  OUTPUTS:      strout - Output string
 *
 *  RTRN VALUE:   Length of strout
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT escape_angle_brackets( pCHAR strin, pCHAR strout )
{
   #define  lab  '<'   /* Left  angle bracket */
   #define  rab  '>'   /* Right angle bracket */

   #define  xml_lab "&amp;lt;"  /* XML escaped left  angle bracket */
   #define  xml_rab "&amp;gt;"  /* XML escaped right angle bracket */

   INT   in_idx;
   INT   out_idx;
   INT   len;

   len = strlen( strin );
   for( in_idx=0, out_idx=0; in_idx < len; in_idx++ )
   {
      if ( strin[in_idx] == lab )
      {
         strcpy( strout+out_idx, xml_lab );
         out_idx += 8;
      }
      else if ( strin[in_idx] == rab )
      {
         strcpy( strout+out_idx, xml_rab );
         out_idx += 8;
      }
      else
      {
         strout[out_idx++] = strin[in_idx];
      }
   }
   return( out_idx );
}


/******************************************************************************
 *
 *  NAME:         GET_CARD_FAMILY
 *
 *  DESCRIPTION:  This function uses the BIN01 record to determine the card
 *                family: Amex, JCB, MC, or Visa.
 *
 *  INPUTS:       Auth_Tx - Transaction record
 *
 *  OUTPUTS:      Card_Family - Global
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_card_family()
{
   if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA) )
      Card_Family = CARD_FAMILY_VISA;

   else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) )
      Card_Family = CARD_FAMILY_MC;

   else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_AMEX) )
      Card_Family = CARD_FAMILY_AMEX;

   else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB) )
      Card_Family = CARD_FAMILY_JCB;

   return;
}


/******************************************************************************
 *
 *  NAME:         CONVERT_USD_TO_PESO
 *
 *  DESCRIPTION:  This function takes a string amount, assumes it is in USD,
 *                converts it into Peso, and returns the Peso amount.  The
 *                Peso amount will be without leading zeroes and with a
 *                decimal point.
 *
 *  INPUTS:       usd_amt
 *
 *  OUTPUTS:      peso_amt
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void convert_usd_to_peso( pBYTE usd_amt, pBYTE peso_amt )
{
   INT    retval;
   LONG   l_amount;
   double d_amount;

   if ( ConvRateFlag == false )
   {
      /* We do not have the conversion rate yet. Get it. */
      retval = currency_conversion( usd_amt, PESO, FROM_USD, peso_amt );
      if ( retval == false )
         strcpy( peso_amt, usd_amt );

      d_amount = strtod( peso_amt, 0 ) / 100.0;
   }
   else
   {
      /* We already have the conversion rate. */
      l_amount = atol( usd_amt );
      d_amount = ( (double)l_amount * USD_to_Peso_Conv_Rate )/100.0;
   }
   sprintf( peso_amt, "%0.2f", d_amount );
   return;
}




/******************************************************************************
 *
 *  NAME:         GET_FIELD4_AMOUNT
 *
 *  DESCRIPTION:  This is the transaction amount.  Typically it is
 *                total_amount.  But in some cases, it must be converted
 *                into Peso.  The conversion is always from USD.
 *
 *  INPUTS:       Auth_Tx - Transaction record
 *
 *  OUTPUTS:      Iss_Amts.f4 - The amount that goes into <i004>
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_field4_amount()
{
   double d_amount;
   BYTE   amount[13]      = "";

   strcpy( amount, Auth_Tx.TLF01_details.total_amount );

   if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
   {
      if ( Auth_Tx.TLF01_details.def_gross_amt[0] != 0x00 )
      {
         /* For deferred transactions, field 4 holds the def gross amt. */
         strcpy( amount, Auth_Tx.TLF01_details.def_gross_amt );
      }

      /* Remove leading zeroes, insert decimal. */
      d_amount = strtod( amount, 0 ) / 100.0;
      sprintf( amount, "%0.2f", d_amount );
   }
   else
   {
      /* For remote origin, get amount value from field 4 from the host. */
      if ( Card_Family == CARD_FAMILY_UNDEFINED )
        get_card_family();

      if ( Card_Family == CARD_FAMILY_VISA )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
            d_amount = strtod( Auth_Tx.TLF01_details.total_amount, 0 ) / 100.0;

         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
            d_amount = strtod( Auth_Tx.TLF01_details.total_amount, 0 ) / 100.0;

         else
            d_amount = strtod( Auth_Tx.TLF01_details.sales_amount, 0 ) / 100.0;
      }
      else
      {
         d_amount = strtod( Auth_Tx.TLF01_details.sales_amount, 0 ) / 100.0;
      }
      sprintf( amount, "%0.2f", d_amount );
   }

   strcpy( Iss_Amts.f4, amount );
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_FIELD6_AMOUNT
 *
 *  DESCRIPTION:  This is the US Dollar amount.  Depending on card type
 *                its value is found in either ch_billing_amt or settlement
 *                total.  These fields will be empty for local origin txns.
 *
 *  INPUTS:       Auth_Tx     - Transaction record
 *                Iss_Amts.f4 - Transaction Amount in FGUARD
 *
 *  OUTPUTS:      Iss_Amts.f6 - The amount that goes into <i006>
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_field6_amount()
{
   double d_amount;
   BYTE   amount[13]      = "";

   strcpy( amount, Auth_Tx.TLF01_details.total_amount );

   if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
   {
      /* For Local Origin, this is the same value as in field 4. */
      if ( Iss_Amts.f4[0] != 0x00 )
      {
         strcpy( Iss_Amts.f6, Iss_Amts.f4 );
      }
      else
      {
         /* Field 4 has not been populated yet. */
         if ( Auth_Tx.TLF01_details.def_gross_amt[0] != 0x00 )
         {
            /* For deferred transactions, field 4 holds the def gross amt. */
            strcpy( amount, Auth_Tx.TLF01_details.def_gross_amt );
         }

         /* Remove leading zeroes, insert decimal. */
         d_amount = strtod( amount, 0 ) / 100.0;
         sprintf( amount, "%0.2f", d_amount );

         strcpy( Iss_Amts.f6, amount );
      }
   }
   else
   {
      /* For remote origin, there is no conversion necessary, but the source
       * of the amount is dependent upon the card family.
       */
      if ( Card_Family == CARD_FAMILY_UNDEFINED )
        get_card_family();

      switch( Card_Family )
      {
         /* If amount is missing, use amount from field 4. */
         case CARD_FAMILY_VISA:
                  if ( Auth_Tx.ch_billing_amt[0] == 0x00 )
                     d_amount = strtod( amount, 0 ) / 100.0;
                  else
                     d_amount = strtod( Auth_Tx.ch_billing_amt, 0 ) / 100.0;
                  break;

         case CARD_FAMILY_MC:
         case CARD_FAMILY_AMEX:
                  if ( Auth_Tx.TLF01_details.settlement_total[0] == 0x00 )
                     d_amount = strtod( amount, 0 ) / 100.0;
                  else
                     d_amount = strtod(Auth_Tx.TLF01_details.settlement_total,0)/100.0;
                  break;

         case CARD_FAMILY_JCB:
                  if ( Auth_Tx.TLF01_details.sales_amount[0] == 0x00 )
                     d_amount = strtod( amount, 0 ) / 100.0;
                  else
                     d_amount = strtod(Auth_Tx.TLF01_details.sales_amount,0)/100.0;
                  break;

         default:
                  d_amount = strtod( amount, 0 ) / 100.0;
                  break;
      }

      sprintf( amount, "%0.2f", d_amount );
      strcpy( Iss_Amts.f6, amount );
   }

   return;
}


/******************************************************************************
 *
 *  NAME:         GET_AMT_CENTER
 *
 *  DESCRIPTION:  This amount is basically transaction amount in Peso.
 *
 *  INPUTS:       Auth_Tx - Transaction record
 *
 *  OUTPUTS:      Iss_Amts.amt_center - The amount that goes into <amt_center>
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_amt_center()
{
   double d_amount;
   BYTE   amount[13] = "";

   strcpy( amount, Auth_Tx.TLF01_details.total_amount );

   if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
   {
      if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, USD) )
      {
         /* Must convert USD into Peso for all Local origin USD Txns. */
         convert_usd_to_peso( amount, amount );
      }
      else
      {
         /* For Peso and Deferred, remove leading zeroes, insert decimal. */
         d_amount = strtod( amount, 0 ) / 100.0;
         sprintf( amount, "%0.2f", d_amount );
      }
   }
   else
   {
      /* For remote origin:
       *      Peso  - Use field 4, no conversion, all card types.
       *      USD   - Use field 5 or 6, convert to Peso, all card types.
       *      Other:
       *           Visa - ch_billing_amt, convert to Peso
       *           MC   - settlement_total, convert to Peso
       *           AMEX - total_amount, no conversion (converted in ncamex)
       *           JCB  - Other currency codes are not allowed
       */

      if ( Card_Family == CARD_FAMILY_UNDEFINED )
        get_card_family();

      if ( Card_Family == CARD_FAMILY_VISA )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
            sprintf( amount, "%0.2f", d_amount );
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            convert_usd_to_peso( amount, amount );
         }
         else
         {
            /* Field 6 from host */
            convert_usd_to_peso( Auth_Tx.ch_billing_amt, amount );
         }
      }
      else if ( Card_Family == CARD_FAMILY_MC )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 5 from host */
            convert_usd_to_peso( Auth_Tx.TLF01_details.settlement_total, amount );
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            convert_usd_to_peso( Auth_Tx.TLF01_details.sales_amount, amount );
         }
         else
         {
            /* Field 5 from host */
            convert_usd_to_peso( Auth_Tx.TLF01_details.settlement_total, amount );
         }
      }
      else if ( Card_Family == CARD_FAMILY_AMEX )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
            sprintf( amount, "%0.2f", d_amount );
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            convert_usd_to_peso( amount, amount );
         }
         else
         {
            /* Field 6 from host */
            convert_usd_to_peso( Auth_Tx.TLF01_details.sales_amount, amount );
         }
      }
      else if ( Card_Family == CARD_FAMILY_JCB )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
            sprintf( amount, "%0.2f", d_amount );
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            convert_usd_to_peso( amount, amount );
         }
      }
   }

   strcpy( Iss_Amts.amt_center, amount );
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_AMT_CARD
 *
 *  DESCRIPTION:  This amount is basically amount in the Card's currency.
 *
 *  INPUTS:       Auth_Tx - Transaction record
 *
 *  OUTPUTS:      Iss_Amts.amt_card - The amount that goes into <amt_card>
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_amt_card()
{
   double d_amount;
   BYTE   amount[13] = "";

   strcpy( amount, Auth_Tx.TLF01_details.total_amount );

   if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
   {
      if ( Auth_Tx.TLF01_details.def_gross_amt[0] != 0x00 )
      {
         /* For deferred transactions, field 4 holds the def gross amt. */
         strcpy( amount, Auth_Tx.TLF01_details.def_gross_amt );
      }

      /* Remove leading zeroes, insert decimal. */
      d_amount = strtod( amount, 0 ) / 100.0;
      sprintf( amount, "%0.2f", d_amount );
   }
   else
   {
      /* REMOTE ORIGIN */
      if ( Card_Family == CARD_FAMILY_UNDEFINED )
        get_card_family();

      if ( Card_Family == CARD_FAMILY_VISA )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            d_amount = strtod( amount, 0 ) / 100.0;
         }
         else
         {
            /* USD or Other Currency */
            d_amount = strtod( Auth_Tx.ch_billing_amt, 0 ) / 100.0;
         }
         sprintf( amount, "%0.2f", d_amount );
      }
      else if ( Card_Family == CARD_FAMILY_AMEX )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
            sprintf( amount, "%0.2f", d_amount );
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            convert_usd_to_peso( amount, amount );
         }
         else
         {
            /* Field 6 from host */
            convert_usd_to_peso( Auth_Tx.TLF01_details.sales_amount, amount );
         }
      }
      else if ( Card_Family == CARD_FAMILY_JCB )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
            sprintf( amount, "%0.2f", d_amount );
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            convert_usd_to_peso( amount, amount );
         }
      }
      else if ( Card_Family == CARD_FAMILY_MC )
      {
         if ( 0 == strcmp(Auth_Tx.TLF01_details.currency_code, PESO) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
         }
         else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
         {
            /* Field 4 from host */
            d_amount = strtod( amount, 0 ) / 100.0;
         }
         else
         {
            /* Field 5 from host */
            d_amount = strtod( Auth_Tx.TLF01_details.settlement_total, 0)/100.0;
         }
         sprintf( amount, "%0.2f", d_amount );
      }
   }

   if ( amount[0] == 0x00 )
      strcpy( amount, "0.00" );  // Guarantees this field is always present.

   strcpy( Iss_Amts.amt_card, amount );
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_OTB_AMT_CARD
 *
 *  DESCRIPTION:  This amount is basically the account balance after
 *                the transaction.
 *
 *  INPUTS:       Auth_Tx - Transaction record
 *
 *  OUTPUTS:      Iss_Amts.otb_amt_card - The amount that goes into <otb_amt_card>
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_otb_amt_card()
{
   double d_amount;
   BYTE   amount[13] = "";

   strcpy( amount, Auth_Tx.TLF01_details.outstanding_balance );

   if ( Card_Family == CARD_FAMILY_UNDEFINED )
     get_card_family();

   if ( (Card_Family == CARD_FAMILY_VISA) ||
        (Card_Family == CARD_FAMILY_MC  )  )
   {
      /* Remove leading zeroes, insert decimal. */
      d_amount = strtod( amount, 0 ) / 100.0;
      sprintf( amount, "%0.2f", d_amount );
   }
   else
   {
      /* Remove leading zeroes, insert decimal. */
      d_amount = strtod( amount, 0 ) / 100.0;
      sprintf( amount, "%0.2f", d_amount );
   }

   strcpy( Iss_Amts.otb_amt_card, amount );
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_FIELDS_49_AND_51
 *
 *  DESCRIPTION:  Get value from fields 49 and 51 of the request from
 *                the host.
 *
 *  INPUTS:       Auth_Tx      - Transaction record
 *
 *  OUTPUTS:      Iss_Amts.f49 - Currency code of the card
 *                Iss_Amts.f51 - (Remote Origin) USD Currency Code
 *                               (Local  Origin) Same as field 49
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_fields_49_and_51()
{
   if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
   {
      /* For Local Origin, this is field 49; field 51 is always same as 49. */
      strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.currency_code );
      strcpy( Iss_Amts.f51, Auth_Tx.TLF01_details.currency_code );
   }
   else
   {
      /* Remote Origin */
      if ( Card_Family == CARD_FAMILY_UNDEFINED )
        get_card_family();

      switch( Card_Family )
      {
         case CARD_FAMILY_VISA:
               if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, USD) )
               {
                  strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.currency_code );
                  strcpy( Iss_Amts.f51, Auth_Tx.TLF01_details.nfi_seq_nbr );
               }
               else if ( 0 == strcmp(Auth_Tx.TLF01_details.nfi_seq_nbr, PESO) )
               {
                  strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.currency_code );
                  strcpy( Iss_Amts.f51, Auth_Tx.TLF01_details.nfi_seq_nbr );
               }
               else
               {
                  strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.nfi_seq_nbr );
                  strcpy( Iss_Amts.f51, USD );
               }
               break;

         case CARD_FAMILY_MC:
         case CARD_FAMILY_AMEX:
               strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.nfi_seq_nbr );
               strcpy( Iss_Amts.f51, Auth_Tx.settlement_curr_code      );
               break;

         case CARD_FAMILY_JCB:
               strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.nfi_seq_nbr );
               strcpy( Iss_Amts.f51, Auth_Tx.TLF01_details.nfi_seq_nbr );
               break;

         default:
               strcpy( Iss_Amts.f49, Auth_Tx.TLF01_details.currency_code );
               strcpy( Iss_Amts.f51, USD );
               break;
      }

      if ( Iss_Amts.f51[0] == 0x00 )
         strcpy( Iss_Amts.f51, Iss_Amts.f49 );
   }

   return;
}


/******************************************************************************
 *
 *  NAME:         GET_COUNTRY_CODE
 *
 *  DESCRIPTION:  For Issuer, get field 19 from the network/interchange.
 *                But since only Amex and Visa send this field, if it is
 *                not present, populate it using the currency code.  For
 *                transactions from ECN terminals, always use "608".
 *
 *  INPUTS:       Auth_Tx      - Transaction record
 *                Iss_Amts.f49 - Currency code of the card
 *
 *  OUTPUTS:      Iss_Amts.country_code - Country code of the card
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_country_code()
{
   if ( Auth_Tx.TLF01_details.dispensation.origin[0] == 'L' )
   {
      /* Local Origin */
      strcpy( Iss_Amts.country_code, PESO );
   }
   else
   {
      /* From a network */
      if ( Auth_Tx.country_code[0] == 0x00 )
      {
         /* Country code is not available. Use the currency
          * code from field 49 of the request from the host.
          */
         strcpy( Iss_Amts.country_code, Auth_Tx.TLF01_details.nfi_seq_nbr );
      }
      else
      {
         /* Country code is available. */
         strcpy( Iss_Amts.country_code, Auth_Tx.country_code );
      }
   }

   return;
}


/******************************************************************************
 *
 *  NAME:         GET_FIELD25
 *
 *  DESCRIPTION:  For Issuer, if pos condition code is not available,
 *                use the industry code to determine it.  This field
 *                is mandatory for Issuer.
 *
 *                Here are the default values to use:
 *
 *                   00 = Normal Purchase
 *                   02 = ATM
 *                   08 = RPS/MOTO
 *                   59 = E-Commerce
 *
 *  INPUTS:       Auth_Tx - Transaction record
 *
 *  OUTPUTS:      Iss_Amts.f25 - POS Condition Code
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_field25()
{
   if ( Auth_Tx.TLF01_details.pos_condition_code[0] == 0x00 )
   {
      /* POS Condition Code is not available. Use default values. */
      if ( Auth_Tx.industry_code[0] == '9' )
         strcpy( Iss_Amts.f25, "02" );  // ATM

      else if ( Auth_Tx.industry_code[0] == '8' )
         strcpy( Iss_Amts.f25, "08" );  // RPS

      else if ( Auth_Tx.industry_code[0] == '5' )
         strcpy( Iss_Amts.f25, "08" );  // MOTO

      else if ( Auth_Tx.industry_code[0] == '7' )
         strcpy( Iss_Amts.f25, "59" );  // E-Commerce

      else
         strcpy( Iss_Amts.f25, "00" );  // Normal Purchase
   }
   else
   {
      /* POS Condition Code is available. */
      strcpy( Iss_Amts.f25, Auth_Tx.TLF01_details.pos_condition_code );
   }

   return;
}


/******************************************************************************
 *
 *  NAME:         GET_QUEUE_MSG_COUNT
 *
 *  DESCRIPTION:  This function returns the number of messages in an
 *                XIPC queue.
 *
 *  INPUTS:       qname - Name of XIPC queue, such as fguardA
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Number of messages in qname
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_queue_msg_count( pCHAR qname )
{
   INT         msg_cnt = 0;
   LONG        tmpQueId;
   LONG        RetCode;
	QUEINFOQUE	QueData;

   /* Access the queue to get the ID. */
   tmpQueId = pteipc_queaccess ( qname );
   if ( tmpQueId > 0 ) 
   {
      memset( (pCHAR)&QueData, 0x00, sizeof(QUEINFOQUE) );
      RetCode = QueInfoQue( tmpQueId, &QueData );

      /* We now have the queue information. Extract the msg count. */
      msg_cnt = QueData.CountMessages;
   }
   return( msg_cnt );

}
