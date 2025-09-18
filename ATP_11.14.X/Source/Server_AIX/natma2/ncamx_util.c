/******************************************************************************
 
 	ncequtil.c
 
    Copyright (c) 2004, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Amex Host Handler Driver
 
 	This module is the host handler for the Equitable host.  It
    will process all messages destined to, or originated from,
    the Amex host.

    APPLICATION:  Advanced Transaction Processor (ATP)

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
#include "ntutils.h"
#include "txutils.h"
#include "memmnger.h"
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"
#include "ncamx_bitfields.h"
#include "txtrace.h"

#define  ECOM_POS_CC   "59"
#define  ECOM_POS_ENTRYMODE "001"

#define HCTS4	1

void create_system_date( pBYTE  );
/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern CHAR    ServiceName[];
  

/* QUEUES */
extern CHAR    updateds_que_name[];
extern CHAR    nsp_que_name[];
extern CHAR    trands_que_name[];
extern CHAR    netds_que_name[];
extern CHAR    devds_que_name[];
extern CHAR    oprmon_que_name[];
extern CHAR    applnk_que_name[];
extern CHAR    authorizer_que_name[];
extern CHAR    timerds_que_name[];
extern CHAR    dialog_que_name[];
extern CHAR cadencie_atm_que_name[] ;


UINT Max_File_Size_Defined = 0 ;

extern CHAR acq_amex_variable_auth_flag;
extern char last_date[9];

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
extern BYTE    Orig_Queue[ ];
extern BYTE    Orig_Info[];

/* Network Type: Issuer, Acquirer, or Both */
extern INT     NetworkType;
extern INT     TranFlow;

extern CHAR    TimerAppData2[];

INT rvrsl_attmpts_value_fr_timeout_amex=0;
INT rvrsl_attmpts_value_fr_laterspns_amex=0;

/* LOCAL VARIABLES */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    BitFieldSize;
INT	   field55_for_pos;

extern CHAR  natma2_Erro_warning_Filename[256] ;
extern CHAR  natma2_module_error_warning_file_name[256] ;
extern CHAR  natma2_error_warning_file_path[256] ;

/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE   ReqRsp_Buffer [COMM_BUF_SZE];

struct trantype_struct 
{
   BYTE iso_msg_type[5];
   BYTE iso_proc_code[7];
   BYTE host2_msg_type[3];
   BYTE host2_tran_code[5];
   BYTE atp_tx_key;
}; 


const struct isobit BITTAB[128+1] =
{
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },    /* bit 1  */   { 0, 0x40 },    /* bit 2  */   { 0, 0x20 },    /* bit 3  */   { 0, 0x10 },    /* bit 4  */
   { 0, 0x08 },    /* bit 5  */   { 0, 0x04 },    /* bit 6  */   { 0, 0x02 },    /* bit 7  */   { 0, 0x01 },    /* bit 8  */
   { 1, 0x80 },    /* bit 9  */   { 1, 0x40 },    /* bit 10 */   { 1, 0x20 },    /* bit 11 */   { 1, 0x10 },    /* bit 12 */
   { 1, 0x08 },    /* bit 13 */   { 1, 0x04 },    /* bit 14 */   { 1, 0x02 },    /* bit 15 */   { 1, 0x01 },    /* bit 16 */
   { 2, 0x80 },    /* bit 17 */   { 2, 0x40 },    /* bit 18 */   { 2, 0x20 },    /* bit 19 */   { 2, 0x10 },    /* bit 20 */
   { 2, 0x08 },    /* bit 21 */   { 2, 0x04 },    /* bit 22 */   { 2, 0x02 },    /* bit 23 */   { 2, 0x01 },    /* bit 24 */
   { 3, 0x80 },    /* bit 25 */   { 3, 0x40 },    /* bit 26 */   { 3, 0x20 },    /* bit 27 */   { 3, 0x10 },    /* bit 28 */
   { 3, 0x08 },    /* bit 29 */   { 3, 0x04 },    /* bit 30 */   { 3, 0x02 },    /* bit 31 */   { 3, 0x01 },    /* bit 32 */
   { 4, 0x80 },    /* bit 33 */   { 4, 0x40 },    /* bit 34 */   { 4, 0x20 },    /* bit 35 */   { 4, 0x10 },    /* bit 36 */
   { 4, 0x08 },    /* bit 37 */   { 4, 0x04 },    /* bit 38 */   { 4, 0x02 },    /* bit 39 */   { 4, 0x01 },    /* bit 40 */
   { 5, 0x80 },    /* bit 41 */   { 5, 0x40 },    /* bit 42 */   { 5, 0x20 },    /* bit 43 */   { 5, 0x10 },    /* bit 44 */
   { 5, 0x08 },    /* bit 45 */   { 5, 0x04 },    /* bit 46 */   { 5, 0x02 },    /* bit 47 */   { 5, 0x01 },    /* bit 48 */
   { 6, 0x80 },    /* bit 49 */   { 6, 0x40 },    /* bit 50 */   { 6, 0x20 },    /* bit 51 */   { 6, 0x10 },    /* bit 52 */
   { 6, 0x08 },    /* bit 53 */   { 6, 0x04 },    /* bit 54 */   { 6, 0x02 },    /* bit 55 */   { 6, 0x01 },    /* bit 56 */
   { 7, 0x80 },    /* bit 57 */   { 7, 0x40 },    /* bit 58 */   { 7, 0x20 },    /* bit 59 */   { 7, 0x10 },    /* bit 60 */
   { 7, 0x08 },    /* bit 61 */   { 7, 0x04 },    /* bit 62 */   { 7, 0x02 },    /* bit 63 */   { 7, 0x01 },    /* bit 64 */

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



/*****************************************************************************
 *
 * NAME:        calc_julian_day
 *
 * DESCRIPTION: This module calculates Julian Day ( DDD ) from the 
 *              input.
 * 
 * INPUTS:      date_str   - input string format  = YYYYMMDD...
 *		
 * OUTPUTS:     julian_str - output string format = DDD
 *
 * RTRN VALUE:  None
 *
 * AUTHOR:      Dennis Irby
 *
 * MODIFICATIONS:
 ******************************************************************************/
void calc_julian_day( pCHAR date_str, pCHAR julian_str )
{
   CHAR  year[5];
   CHAR  month[3];
   CHAR  day[3];
   INT   iYear;
   INT   iMonth;
   INT   iDay;
   INT   julian_day;
   INT   i;
   INT   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


   strncpy( year, date_str, 4 );
   iYear  = atoi( year );

   strncpy( month, &date_str[4], 2 );
   iMonth = atoi( month );

   strncpy( day, &date_str[6], 2 );
   iDay   = atoi( day );

   for( i=0,julian_day=0; i<iMonth-1 && i<12; i++ )
   {
      julian_day += DaysOfMonth[i];
   }
   julian_day += iDay;

   if ( 2 < iMonth )
      if ( 0 == ( iYear % 4 ) )
         julian_day++;

   sprintf( julian_str, "%03d", julian_day );
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
 *  MODIFICATIONS:
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
 *  MODIFICATIONS:
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
   sprintf( rrn, "%1.1s%3.3s%5.5s%3.3s",time_date+3, julian_day,
                  time_str+1, time_date+20 );
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
void copy_to_req_resp_buffer ( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA    p_msg_data  = NULL_PTR;
   pBYTE            p_data      = NULL_PTR;
   LONG             msglen;

   memset(ReqRsp_Buffer, 0x00, COMM_BUF_SZE);   /* zero out the buffer */

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   msglen     = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   reqidx = 0;
   buflen = msglen;
   memcpy(ReqRsp_Buffer, p_data, msglen);
   invalid_msg_format = false;

   /* Retrieve originator info from incoming message */
   strcpy( Orig_Info, ptemsg_get_msg_orig_comms_info( p_msg_in ) );
   strcpy( Orig_Queue, ptemsg_get_msg_orig_queue( p_msg_in ) );

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
INT determine_txn_flow( pCHAR msgtype, INT source )
{
   INT  tran_flow;

   /* Third character of msgtype is 1 or 3 for responses. */
   if ( (msgtype[2] == '1') || (msgtype[2] == '3') )
   {
      /* This is a response.  Where did it come from? */
      if ( source == HOST )
         tran_flow = SWITCHED_OUT;
      else
         tran_flow = SWITCHED_IN;
   }
   else
   {
      /* This is a request. */
      if ( source == HOST )
         tran_flow = SWITCHED_IN;
      else
         tran_flow = SWITCHED_OUT;
   }
   return( tran_flow );
}


/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_HOST
 *
 *  DESCRIPTION:  This function sends a response to the host.  It gets the
 *                response code and other information from Auth_Tx.
 *
 *  INPUTS:       send_flag - Used to determine if a resp should go to host
 *                            Values:
 *                               SEND_UNCONDITIONALLY = Always send
 *                               SEND_IF_NOT_REVERSAL = Do not send reversals
 *                Auth_Tx - (Global) Transaction information
 *
 *                NOTE: The purpose of the send_flag is because reversal
 *                responses are sent to the host as soon as the request
 *                is received from the host.  So as processing continues,
 *                if there are any error conditions, we do not want to
 *                send a second response back to the host.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void send_response_to_host( INT send_flag )
{
   CHAR  err_buf[100]="";
   BYTE  temp_tx_key;

   TranFlow = SWITCHED_IN;

   if(true == nposa2_check_for_cadencieacq_transaction())
      {
   	  /* Make sure the tx_key is for a response. */
   	  if ( Auth_Tx.TLF01_details.tx_key < 100 )
   		 Auth_Tx.TLF01_details.tx_key += 100;

   	  	  send_message_to_host(err_buf,false);
    	  return;
      }

   if ( (send_flag == SEND_UNCONDITIONALLY) ||
       ((send_flag == SEND_IF_NOT_REVERSAL) && (false == is_reversal() )) )
   {
      if ( is_reversal() )
         temp_tx_key = Auth_Tx.TLF01_details.tx_key;

      /* Make sure the tx_key is for a response. */
      if ( Auth_Tx.TLF01_details.tx_key < 100 )
         Auth_Tx.TLF01_details.tx_key += 100;

      if ( true == build_response_msg() )
      {
         if ( false == send_message_to_host(err_buf,false) )//praneeth added false as parameter
            natma2_log_message(1, 2, err_buf, "send_response_to_host" ,1);
      }

      (void)insert_db_record( TLF01_DATA );

      if ( is_reversal() )
         Auth_Tx.TLF01_details.tx_key = temp_tx_key;
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         SEND_REQUEST_TO_HOST
 *
 *  DESCRIPTION:  This function sends a request to the host.  It gets the
 *                information it needs from Auth_Tx and Ncf01.
 *
 *  INPUTS:       timer_resp_flag - Used to control flow upon receipt of timer
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void send_request_to_host( pCHAR timer_resp_flag )
{
   INT   ret_val;
   INT   seconds,buf_temp_len=0;
   CHAR  err_buf[100]  ="";
   CHAR  lookup_key[LOOKUP_KEY_SIZE]="";

   TranFlow = SWITCHED_OUT;
   Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;


  /* if(true==nposa2_check_for_cadencieacq_transaction())
   {
	   ret_val = send_message_to_host( err_buf,false );
	   return;
   }*/
   ret_val = build_request_msg( err_buf );
   if ( true == ret_val )
   {
      ptetime_get_strtime( Auth_Tx.TLF01_details.host_start_time );
		/*Send whatever msg coming from AMEX HOST. Make sure we have valid msg from HOST*/
        /* We will confirm when to send the same msg, restrict to 0420/1420 now */
		 if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0 && strcmp(ServiceName,"natma2") != 0)
		{
			 if ((0 == strncmp(Auth_Tx.TLF01_details.message_type, "0420", 4) ||
				  0 == strncmp(Auth_Tx.TLF01_details.message_type, "1420", 4)) &&
				  0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncpos", 5) &&
				  0 != strncmp(Auth_Tx.function_code, PARTIALREVERSAL_FUNC_CD, 3))
			 {
				 /*do not copy the original message to ReqRspBuffer*/
			 }
			 else if (((!is_reversal()) && pin_block_exists()) &&
					  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5)))
			 {
				 /*do not copy the original message to ReqRspBuffer*/
			 }
			 else
			 {
				 buf_temp_len = atoi(Auth_Tx.req_data_len);
				 if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
				 {
					 buflen = buf_temp_len;
					 memcpy(ReqRsp_Buffer,Auth_Tx.req_buffer,buflen);
				 }
			 }

	          /* 1. After sending the msg to cadencie, clear the
	             * original msg present in the auth_tx Req_buffer,as it is not used after sending it to host.
	             * */
	        /* memset(Auth_Tx.req_data_len,0x00,sizeof(Auth_Tx.req_data_len));*/
		 }

#ifdef HCTS4

      if(strncmp(Auth_Tx.TLF01_details.handler_queue, "sbatch", 6) != 0)
      {
#endif
		  /* Set the request timer. */
		  (void)create_request_timer_key( lookup_key );
		  seconds = atoi(Ncf01_I.request_timer);
		  strcpy( TimerAppData2, timer_resp_flag );
		  ret_val = set_timer( lookup_key, seconds, REQUEST_TIMER );
#ifdef HCTS4
      }
#endif
      if ( true == ret_val )
      {
         /* Send the transaction request to the host. */
         ret_val = send_message_to_host( err_buf,false );//praneeth added false as parameter
      }
      else
      {
         sprintf( err_buf,
                 "Cannot process %s txn request to host. Unable to set timer.",
                  Auth_Tx.TLF01_details.message_type );
      }
   }

   if ( false == ret_val )
   {
      /* Since the txn request did not go to the
       * host, we need to initiate a response.
       */
      natma2_log_message( 1, 2, err_buf, "send_request_to_host" ,1);
      ret_val = perform_standin_processing( SYSTEM_ERROR,
                                            GS_TXN_DROPPED,
                                            err_buf );
      if ( false == ret_val )
         natma2_log_message( 1, 2, err_buf, "send_request_to_host",1 );
   }
   if(true==nposa2_check_for_cadencieacq_transaction())
   {
	   //for cadencie acquired transaction, the record was not being stored in tlf01. Hence added it.
   nposa2_insert_tlf01(&Auth_Tx );
   }

   return;
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG_TO_AUTHORIZER
 *
 *  DESCRIPTION:  This function sends a PTE message to the authorizer.  It
 *                will be either a request or a response.
 *
 *  INPUTS:       msgtype - Either MT_AUTH_REQUEST or MT_AUTH_RESPONSE
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_msg_to_authorizer( BYTE msgtype )
{
   INT   ret_val;
   CHAR  err_buf[100]="";
   CHAR  Buffer[200];

#if BDOR_62_PREAUTH_VOID
	if(strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
	{
		ret_val = build_and_send_request_message( msgtype,
												  0,
												  0,
												  (pBYTE)parvbh_que_name,
												  (pBYTE)&Auth_Tx,
												  Auth_Tx_Size,
												  0,
												  NULL_STR,
												  err_buf   );

		if ( ret_val == false )
		{
			memset( Buffer, 0x00, sizeof(Buffer) );
			sprintf( Buffer,
					"Unable to forward txn to tool, %s.  Error = %s",
					parvbh_que_name, err_buf );
			natma2_log_message( 1, 2, Buffer, "send_msg_to_authorizer",1 );
		}
	}
	else
#endif
	{
	ret_val = build_and_send_request_message( msgtype,
												 0,
												 0,
										  (pBYTE)authorizer_que_name,
										  (pBYTE)&Auth_Tx,
												 Auth_Tx_Size,
												 0,
												 NULL_STR,
												 err_buf   );

   if ( ret_val == false )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Unable to forward txn to authorizer, %s.  Error = %s",
               authorizer_que_name, err_buf );
      natma2_log_message( 1, 2, Buffer, "send_msg_to_authorizer",1 );
   }
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
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;

   /* Verify message size. */
   if ( length < 0 )
      ret_code = false;

   else if ( msgtype == MT_OUTGOING )
      if ( length > MAX_HOST_MSG_SIZE )
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
            buflen = 350;
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
 *  NAME:         init_Auth_Tx
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *
 *  INPUTS:       Auth_Tx - (Global)Data structure containing transaction info.
 *
 *  OUTPUTS:      Auth_Tx - (Global)Structure is updated with new information.
 *
 *  RTRN VALUE:   False if invalid transaction key (tx_key), else true
 *
 *  AUTHOR:       S. Yarlagadda
 *
 ******************************************************************************/
INT init_Auth_Tx(void)
{
   INT	  ret_val;
   CHAR   unique_tran_str_id[21] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT	   tid_len = 0;
   INT	   unique_tran_str_id_len=0;

   /* Get local time. */
   if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
		ptetime_get_timestamp( time_date );
		get_date( time_date, date_str );
		get_time( time_date, time_str );
		strcpy( Auth_Tx.TLF01_details.date_yyyymmdd, date_str );
		strcpy( Auth_Tx.TLF01_details.time_hhmmss,	 time_str );
   }

   /* Create a unique transaction id for the new auth_tx record. */
   ptetime_get_strtime( unique_tran_str_id );
   memset( Auth_Tx.TLF01_details.primary_key.transaction_id, 0,
		   sizeof(Auth_Tx.TLF01_details.primary_key.transaction_id) );
   tid_len= strlen(Auth_Tx.TLF01_details.terminal_id);
   if(tid_len == 8)
   {
   		memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
		   		Auth_Tx.TLF01_details.terminal_id,
		   		sizeof( Auth_Tx.TLF01_details.terminal_id) - 1 );
   		strncat( Auth_Tx.TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+2, 12 );
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
				 Auth_Tx.TLF01_details.date_yyyymmdd+2, 6);
		strncat (unique_tran_str_id,temp_unique_tran,14);
		
		unique_tran_str_id_len= strlen(unique_tran_str_id);
		memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
				Auth_Tx.TLF01_details.terminal_id, tid_len );
		strncat( Auth_Tx.TLF01_details.primary_key.transaction_id,
				 unique_tran_str_id+tid_len, 
				 (unique_tran_str_id_len-tid_len) );
		Auth_Tx.TLF01_details.primary_key.transaction_id[20]=0x00;
   
	}

   /* Initialize some of the auth_tx fields. */
   Auth_Tx.TLF01_details.general_status 	 =	GS_TXN_NEW;
   Auth_Tx.TLF01_details.entry_type 		 =	FINANCIAL;

   
   strcpy( Auth_Tx.TLF01_details.dispensation.origin, "RO" );
   

   strcpy( Auth_Tx.TLF01_details.handler_queue, 	  application_que_name);
   strncpy( Auth_Tx.TLF01_details.tran_start_time,	  unique_tran_str_id,16  );

   Auth_Tx.TLF01_details.verify_mag_stripe[0] = 'Y';

   /* Set issuer Id for reporting purposes. */
   strncpy( Auth_Tx.TLF01_details.issuer_id, AppName,
			sizeof(Auth_Tx.TLF01_details.issuer_id) );

   /* Create RRN */
   if ( Auth_Tx.TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
	  create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
   }

   /* Populate the source encryption key and pin format from the NCF01 record */
   memcpy( Auth_Tx.TLF01_details.source_key, Ncf01_I.cwk, sizeof(Ncf01_I.cwk) );
   strcpy( Auth_Tx.TLF01_details.source_pin_format, Ncf01_I.pin_type );

   /* Determine Transaction Type */
   ret_val = determine_tx_key();

   /* store acquiring id for TLF01 */
	  strcpy(Auth_Tx.TLF01_details.acquiring_id,Auth_Tx.acq_inst_id_cd);

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         DETERMINE_TX_KEY
 *
 *  DESCRIPTION:  This function determines the tx_key based on message type
 *                and processing code.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if unable to determine tx_key, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT determine_tx_key( )
{
   INT   ret_val = true;
   CHAR  err_buf[100] 	= {0};
   CHAR  msgtype[5] 	= {0};
   CHAR  procode[7] 	= {0};

   memcpy( msgtype, Auth_Tx.TLF01_details.message_type,    4 );
   memcpy( procode, Auth_Tx.TLF01_details.processing_code, 6 );

   /* 11xx */
   if ( msgtype[1] == '1' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
         if ( strcmp(procode,"014000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE;
         else if ( strcmp(procode,"004000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION;
         else if(strcmp(procode,"200000") == 0)
        	 Auth_Tx.TLF01_details.tx_key = AUTH_REFUND;
	#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
         else if ( strcmp(procode,"334000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION;
	#endif
         else
            ret_val = false;
      }
      else if( msgtype[2] == '1')
      {
         /* Response */
          if ( strcmp(procode,"014000") == 0)
          {
            Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
          }
          else if ( strcmp(procode,"004000") == 0 || strcmp(procode,"004800") == 0)
          {
            Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
          }
          else if ( 0 == strncmp(procode,"014008",6))
          {
        	  Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
          }
          else if(0 == strncmp(procode,"200000",6)||0 == strncmp(procode,"200800",6))
          {
        	  Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
          }
          else if(0 == strncmp(procode,"204000",6))
		  {
			  Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
		  }
	#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
          else if(0 == strncmp(procode,"334000",6))
		  {
			  Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
		  }
	#endif
         else
            ret_val = false;
      }
	  else if ( msgtype[2] == '2' )
      {
         /* Advice Request */
         if ( strcmp(procode,"000000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE;
         else if ( strcmp(procode,"204000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_REFUND;
	#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
		 else if ( strcmp(procode,"334000") == 0)
			  Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE;
	#endif
         else
            ret_val = false;
      }
      else if ( msgtype[2] == '3' )
      {
         /* Advice Response */
         if ( strcmp(procode,"000000") == 0)
			 Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
         else if ( strcmp(procode,"204000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
         else if(0 == strncmp(procode,"200000",6))
         {
        	 Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
         }
	#ifdef AMX1_APR2024 /*https://tf-bdosupport.atlassian.net/browse/TFV119AMC2-11*/
         else if ( strcmp(procode,"334000") == 0)
		 {
			 Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
		 }
	#endif
         else
            ret_val = false;
      }
	  else
			ret_val = false;
   }

   /* 12xx */
   else if ( msgtype[1] == '2' )
   {
      if ( msgtype[2]=='0')
         Auth_Tx.TLF01_details.tx_key = AUTH_SALE_CASH;
      else if ( msgtype[2] == '1' )
      {
		 Auth_Tx.TLF01_details.tx_key = AUTH_SALE_CASH_RESPONSE;
      }
      else
         ret_val = false;

   }

   /* 03xx */
   else if ( msgtype[1] == '3' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_CARD_UPDATE;
      }
      else
      {
         /* Response */
         Auth_Tx.TLF01_details.tx_key = AUTH_CARD_UPDATE_RESPONSE;
      }
   }

   /* 04xx */
   else if ( msgtype[1] == '4' )
   {
      if (( msgtype[2] == '2' ) || ( msgtype[2] == '0' ))
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
      }
      else
      {
         /* Response */
         Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
      }
   }

   /* 08xx */
   else if ( msgtype[1] == '8' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
      }
      else if ( msgtype[2] == '1' )
      {
         /* Response */
         Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
      }
      else
         ret_val = false;
   }
    
   /* Is it a valid tx_key? */
   if ( ret_val == false )
   {
      if ( TranFlow == SWITCHED_IN )
      {
         sprintf( err_buf,
              "Unsupported txn from host: msgtype = %s, processing code = %s",
               msgtype, Auth_Tx.TLF01_details.processing_code );
      }
      else
      {
         sprintf( err_buf,
              "Unsupported txn from ATP: msgtype = %s, processing code = %s",
               msgtype, Auth_Tx.TLF01_details.processing_code );
      }
      natma2_log_message( 1, 2, err_buf, "determine_tx_key",1 );
      Auth_Tx.TLF01_details.tx_key = AUTH_INVALID;
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SET_TX_KEY_TO_RESPONSE
 *
 *  DESCRIPTION:  This procedure will return a response transaction type
 *                based on a request transaction type.  For example, a
 *                transaction request of AUTH_SALE will return a response
 *                type of AUTH_SALE_RESPONSE.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction type of request transaction
 *
 *  OUTPUTS:      Auth_Tx - (Global) Transaction type is updated to response
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void set_tx_key_to_response()
{
   /* Make sure this is a request.  If it is not, then it is a response
    * and we need to make sure we return what was sent in.   Note: Anything
    * greater than AUTH_INVALID is a response, not a request.
    */
   if ( Auth_Tx.TLF01_details.tx_key < AUTH_INVALID )
      Auth_Tx.TLF01_details.tx_key += 100;

   return;
}

/******************************************************************************
 *
 *  NAME:         vEBCDICMove
 *
 *  DESCRIPTION:  This function converts a variable length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.  The length is also copied
 *                into the buffer in EBCDIC format.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen,i;
   INT                  ret_val = true;
   BYTE 				temp_bcd_len[5]={0},temp_ascii_len[5]={0};
   int ind=0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
   	   if(fieldid == 55 )
		{
   		   if (field55_for_pos == 1)
   		   {
				memcpy(temp_bcd_len,HOST_MESSAGE.IccData_len,pBF->bd_fieldlen);

				movelen = genutil_bcd_to_int(temp_bcd_len, 2);

				memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, 2);
				reqidx = reqidx + 2;

				memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata +pBF->bd_fieldlen , movelen);
   		   }
   		   else
   		   {
				memcpy(temp_bcd_len,HOST_MESSAGE.IccData_len,pBF->bd_fieldlen);
				// convert EBCD to ascii
				for( i=0; i<pBF->bd_fieldlen; i++ )
				{
					convert_to_ascii( temp_bcd_len, temp_ascii_len,pBF->bd_fieldlen );

				}
				movelen = atoi(temp_ascii_len);
				movelen += pBF->bd_fieldlen;
				memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen);
   		   }

			reqidx = reqidx + movelen;   /* increment buffer pointer */
	   }
#if AMEX03B_APRIL2024
	   else if(fieldid == 60)
	   {
		   memcpy(temp_bcd_len,HOST_MESSAGE.priv60_len,pBF->bd_fieldlen);
			// convert EBCD to ascii
			for( ind=0; ind<pBF->bd_fieldlen; ind++ )
			{
				convert_to_ascii( temp_bcd_len, temp_ascii_len,pBF->bd_fieldlen );

			}
			movelen = atoi(temp_ascii_len);
			movelen += pBF->bd_fieldlen;
			memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen);
			reqidx = reqidx + movelen;   /* increment buffer pointer */
	   }
#endif
   	   else if(fieldid == 60)
   	   {
   		   memcpy(temp_bcd_len,HOST_MESSAGE.priv60_len,pBF->bd_fieldlen);
			// convert EBCD to ascii
			for( i=0; i<pBF->bd_fieldlen; i++ )
			{
				convert_to_ascii( temp_bcd_len, temp_ascii_len,pBF->bd_fieldlen );

			}
			movelen = atoi(temp_ascii_len);
			movelen += pBF->bd_fieldlen;
			memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen);
			reqidx = reqidx + movelen;   /* increment buffer pointer */
   	   }
   	   else if	(fieldid == 61 )
	   	{
		   	movelen = pBF->bd_fieldlen;

			convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );

			reqidx += movelen;

			movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );

			memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata+pBF->bd_fieldlen, movelen);

			reqidx = reqidx + movelen;   /* increment buffer pointer */
	   	}
	   else if(pBF->p_bd_pdata[0] != 0x00)
	   {
			movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
			movelen += pBF->bd_fieldlen;

			convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
			reqidx += movelen;
	   }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end vEBCDICMove */



/******************************************************************************
 *
 *  NAME:         EBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
      reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

} /* end EBCDICMove */



/******************************************************************************
 *
 *  NAME:         RspEBCDICvMove
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.  The length is
 *                also copied.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi(temp_string_len) + FieldLen;

      if((fieldid == 55 || fieldid == 60)) //EMV data, AMEX03B_APRIL2024 60 added as part of AMEX03B_APRIL2024 april 2024
		{
			 memcpy(pBF->p_bd_pdata,&ReqRsp_Buffer[reqidx],movelen);
			 reqidx += movelen;
		 }
		else if ( (reqidx + movelen) <= buflen )
       {
         /* Move the data, converting it from EBCDIC to ASCII. */
         convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICvMove */


/******************************************************************************
 *
 *  NAME:         RspEBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICMove */


/***********************************************
   GenericMove
************************************************/
INT GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;

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
   else
      ret_val = false;

   return( ret_val );

} /* end GenericMove */



/***********************************************
   RspMoveGeneric
************************************************/
INT RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
      {
         memcpy(pBF->p_bd_pdata,               /* to data pointer          */
                &ReqRsp_Buffer[reqidx],        /* move from request buffer */
                pBF->bd_fieldlen);             /* for length of field      */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspMoveGeneric */




/******************************************************************************
 *
 *  NAME:         Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specified HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table( fieldid );
   if (pBF != NULL)
   {
      /*---------------------------------------------------*/
      /* in any byte non-zero, it's there, return true     */
      /*---------------------------------------------------*/
      for ( i=0; i < pBF->bd_fieldlen; i++ )
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return( true );
      }
   }
   return( false );
} /* end Move_IfThere */


/******************************************************************************
 *
 *  NAME:         Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved from HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end Move_Always */


/******************************************************************************
 *
 *  NAME:         get_variable_length
 *
 *  DESCRIPTION:  This function converts a variable length character string
 *                into an integer.
 *
 *  INPUTS:       DataField - Pointer to ASCII character string
 *                FieldLen  - Length of 'DataField'
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Integer value of 'DataField'
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
int get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i;
   double len=0;

   if ( 0 < FieldLen )
   {
      for( i=0; i<FieldLen; i++ )
      {
         len += (int)( DataField[i] - 0x30 ) * (pow(10,(double)(FieldLen-i-1)));
      }
   }
   return( (int)len );
}


/******************************************************************************
 *
 *  NAME:         Find_Tran_Table
 *
 *  DESCRIPTION:  This procedure finds a transaction type in the transaction
 *                table where the transactions are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific transaction definition in the
 *                transaction table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
struct trandef *Find_Tran_Table( pAUTH_TX p_auth_tx )
{
   INT  i;            /* loop index  */
   BYTE trantype;
   CHAR Buffer[100];

   /* CASH ADVANCE with origin as "RO"  (from ATM ) will be treated as 1200 message. 
	  CASH ADVANCE with origin as "LO"  (from POS ) will be treated as 1100 message.*/
   if((p_auth_tx->TLF01_details.tx_key == AUTH_CASH_ADVANCE) && 
	   (strncmp(p_auth_tx->TLF01_details.dispensation.origin,"RO",2) == 0))
	   trantype = AUTH_SALE_CASH;
   else
	   trantype = p_auth_tx->TLF01_details.tx_key;
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); i++ )
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         return( &TRANTABLE[i] );
         break;
      }
   }

   /* Did not find trantype in table - log error & return NULL */
   sprintf( Buffer, "This trantype, %d, not found in TRANTABLE", trantype );
   natma2_log_message( 1, 2, Buffer, "FIND_TRAN_TABLE" ,0);

   return NULL;
} /* end Find_Tran_Table */   


/******************************************************************************
 *
 *  NAME:         Find_Bitfield_Table
 *
 *  DESCRIPTION:  This procedure finds a bitfield definition in the bitfield
 *                table where the bitfields are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       bitfield - ID of bitfield definition we are looking for
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific bitfield definition in the
 *                bitfield table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
struct bitfield_data *Find_Bitfield_Table( enum bitfields bitfield )
{
   INT      i;                    /* loop index */
   CHAR     temp_str[60];

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */

   /* Field not found in table - log error and return NULL */
   sprintf(temp_str, "This bitfield not found in BFData, bitfield: %d",
                      bitfield);
   natma2_log_message( 1, 2, temp_str, "Find_Bitfield_Table",0 );
   return NULL;

} /* end Find_Bitfield_Table */




/******************************************************************************
 *
 *  NAME:         BUILD_HOST_MSG
 *
 *  DESCRIPTION:  This procedure copies the information from the HOST_MESSAGE
 *                structure into a character string.  The character string is
 *                then shipped to the host.
 *
 *  INPUTS:       Auth_Tx      - (Global)Transaction record
 *                HOST_MESSAGE - (Global)Transaction data destined for the host
 *
 *  OUTPUTS:      err_buf - Text message in the event of an error
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_host_msg( pCHAR err_buf )
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;     /* index to bitfields for this transaction */
   INT                  bitMapsize;
   BYTE                 ReqRsp_Buffer_bitmap_hex[17]="";
   BYTE                 ReqRsp_Buffer_bitmap_ebcdic[33]="";

   /* Get pointer to transaction table */
   pTranDef = Find_Tran_Table( &Auth_Tx );

   if ( NULL == pTranDef )
   {
      if ( TranFlow == SWITCHED_OUT )
      {
         sprintf( err_buf,
              "Request message %s, processing code %s, to host not supported.",
               Auth_Tx.TLF01_details.message_type,
               Auth_Tx.TLF01_details.processing_code);
      }
      else
      {
         sprintf( err_buf,
              "Response message %s, processing code %s, to host not supported.",
               Auth_Tx.TLF01_details.message_type,
               Auth_Tx.TLF01_details.processing_code);
      }
      return false; /* Did not find TX type - so quit */
   }

   /*-----------------------------------------*/
   /* Copy in Message Id for this transaction */
   /*-----------------------------------------*/
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   bitmapidx = 4;                            /* point to bitmap     */

   
   /* A single bit map */
   reqidx = 12;
   bitMapsize = 8;
   BitFieldSize = NORMAL_BITFIELD;
   

   memcpy(ReqRsp_Buffer, pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   /*-----------------------------------------------------*/
   /* For all requested bitfields for this transaction... */
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

         if ( NULL == pBF )
         {
            sprintf( err_buf,
                    "Unable to find bitfield (%d) to build %s msg for host.",
                     bfidx, Auth_Tx.TLF01_details.message_type );
            return false; /* Invalid bit was set - so quit. */
         }

         /*-----------------------------------------------------------*/
         /* if conditional routine returns true, move the bitfield... */
         /*-----------------------------------------------------------*/
         if ((pBF->bd_BFCond)(pBF->bd_fieldid,Auth_Tx.TLF01_details.tx_key) == true)
         {
            /*------------------------------*/
            /* set the bit in the bitmap... */
            /*------------------------------*/
            ReqRsp_Buffer[bitmapidx +BITTAB[pTranDef->td_bitfields[bfidx]].byte_no] |=
                  BITTAB[pTranDef->td_bitfields[bfidx]].bit_mask;

            /*--------------------------*/
            /* call the move routine... */
            /*--------------------------*/
            (pBF->bd_BFMove)(pBF->bd_fieldid);

         }  /* end if */
      }
      else
      {
         bfidx = BitFieldSize;
      }  /* end if */
   } /* end for */

   buflen = reqidx;
   return( true );
}

/******************************************************************************
 *
 *  NAME:         BUILD_HOST_STRUCTURE
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                a request or response message destined for the host.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_host_structure( pCHAR err_buf )
{
   INT     ret_val = true;
   INT     result_code = 0;  /* 0 = no error, 1 = error */

   CHAR     err_buf1[100] = {0};
   CHAR     temp_message_type[5] = {0};
   CHAR current_system_date[9]={0};


   if ( map_iso_to_amex_message_type(err_buf1,temp_message_type) == false )
   {
      ret_val = false;
      sprintf( err_buf,
              "build_host_structure: (%s).",
               err_buf1);
      return ret_val;
   }

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   if ( TranFlow == SWITCHED_IN )
   {
      if (temp_message_type[1] == '8' )
	  { 
		  /* Logon request message from host*/
		 (void)build_response_field_3(   &result_code, err_buf );
		 (void)build_response_field_11(  &result_code, err_buf );
		 (void)build_response_field_12(  &result_code, err_buf );
		 (void)build_response_field_24(  &result_code, err_buf );
		 (void)build_response_field_33(  &result_code, err_buf );
		 (void)build_response_field_39(  &result_code, err_buf );
	  }
	  /* Girija Y, ThoughtFocus - Added these methods to send the response to cadencie host */
	   else if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1314",4))					
	  {
		  /* File Update response message to cadencie host */
		 (void)build_response_field_2(   &result_code, err_buf );
		 (void)build_response_field_11(  &result_code, err_buf );
		 (void)build_response_field_12(  &result_code, err_buf );
		 (void)build_response_field_14(  &result_code, err_buf );
		 (void)build_response_field_24(  &result_code, err_buf );
		 (void)build_response_field_25(  &result_code, err_buf );
		 (void)build_response_field_33(  &result_code, err_buf );
		 //(void)build_response_field_39(  &result_code, err_buf );
		 /* Copy response code(Action code) - ThoughtFocus Girija Y, Phani */
		 memcpy( HOST_MESSAGE.responsecd,
                 Auth_Tx.accptor_bus_code,
                 DE39_SIZE );

	  }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s).",
                  Auth_Tx.TLF01_details.message_type );
         strcat( err_buf, "  Unable to build response message." );
      }
   }
   else  /* SWITCHED OUT Transactions */
   {
#if AMEX03B_APRIL2024
		create_system_date(current_system_date );
		if(0 != memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
		{
			//get only last updated date ,
			 //the last update date should be equal to current system date,
			get_last_dated_date_only();
			if(0 == memcmp(current_system_date,last_date,sizeof(current_system_date)-1))
			{
				get_variable_auth_config_details();
			}
		}
#endif
      if (temp_message_type[1] == '1') 
      {
         if(temp_message_type[2] == '0') 
         {
            /* Build fields necessary for 1100 REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_5(  &result_code, err_buf );  // ThoughtFocus Girija Y: Added function to send reconciliation amount 	
            (void)build_request_field_7(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
            (void)build_request_field_22( &result_code, err_buf );
            (void)build_request_field_24( &result_code, err_buf );
            (void)build_request_field_26( &result_code, err_buf );
            (void)build_request_field_31( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
            (void)build_request_field_43( &result_code, err_buf );
			(void)build_request_field_45( &result_code, err_buf ); 
            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_50( &result_code, err_buf ); // ThoughtFocus Girija Y: Added function to send reconciliation currency code
			if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncpos", 5) &&
				true == pin_block_exists())
			{
				 (void)build_request_field_52( &result_code, err_buf );
			}
            (void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
#if AMEX03B_APRIL2024
			if(acq_amex_variable_auth_flag =='Y')
				(void)build_request_field_60( &result_code, err_buf );
#endif
			(void)build_request_field_61( &result_code, err_buf );
			/* TF Phani- Make sure AUTH number is null- before we send to Cadiece*/
			if(true==nposa2_check_for_cadencieacq_transaction())
			{

			}
			else
			{
			memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
         }
         }
         else if(temp_message_type[2] == '2') 
         {
            /* Build fields necessary for  1120 REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_5(  &result_code, err_buf );  // TF Girija Y: Added function to pass reconciliation amount, March 04, 2009 	
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
			(void)build_request_field_19( &result_code, err_buf );
            (void)build_request_field_22( &result_code, err_buf );
            (void)build_request_field_24( &result_code, err_buf );
            (void)build_request_field_26( &result_code, err_buf );
            (void)build_request_field_31( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );
            (void)build_request_field_38( &result_code, err_buf );
            (void)build_request_field_39( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
			(void)build_request_field_43( &result_code, err_buf );
            (void)build_request_field_45( &result_code, err_buf );
			(void)build_request_field_48( &result_code, err_buf ); // TF Girija Y: Added function to send additional data, March 04, 2009 		
            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_50( &result_code, err_buf ); // TF Girija Y: Added function to send reconciliation currency code, March 04, 2009 		
			//(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );
         }
         else
         {
            /* Unsupported message type.  Log an error. */
            ret_val = false;
            sprintf( err_buf,
                    "Unsupported message type (%s).",
                     Auth_Tx.TLF01_details.message_type );
            strcat( err_buf, "  Unable to build request message." );
         }
      }
      else if(temp_message_type[1] == '2') 
      {
         /* Build fields necessary for 1200  REQUEST messages. */
         (void)build_request_field_3(  &result_code, err_buf );
         (void)build_request_field_4(  &result_code, err_buf );
         (void)build_request_field_5(  &result_code, err_buf );
         (void)build_request_field_7(  &result_code, err_buf );
         (void)build_request_field_11( &result_code, err_buf );
         (void)build_request_field_12( &result_code, err_buf );
		 (void)build_request_field_13( &result_code, err_buf );  // ThoughtFocus Girija Y: Added function to send effective date to Ncatma2			   
		 (void)build_request_field_14( &result_code, err_buf );  // ThoughtFocus Girija Y: Added function to send Expiry date to Ncatma2		
         (void)build_request_field_19( &result_code, err_buf );
         (void)build_request_field_22( &result_code, err_buf );
         (void)build_request_field_24( &result_code, err_buf );
         (void)build_request_field_26( &result_code, err_buf );
         (void)build_request_field_31( &result_code, err_buf );
         (void)build_request_field_32( &result_code, err_buf );
         (void)build_request_field_33( &result_code, err_buf );
         (void)build_request_field_35( &result_code, err_buf );
         (void)build_request_field_37( &result_code, err_buf );
         (void)build_request_field_41( &result_code, err_buf );
		 (void)build_request_field_43( &result_code, err_buf );
         (void)build_request_field_49( &result_code, err_buf );
         (void)build_request_field_50( &result_code, err_buf );
         (void)build_request_field_52( &result_code, err_buf );
 		 (void)build_request_field_55( &result_code, err_buf );
		 (void)build_request_field_61( &result_code, err_buf );
      }
      else if ( is_reversal() )
      {
		/*  If Original txn process as STIP and later send into host.
			After sent host If ATP gets reversal txn for that.
			It should go as 420 msg - TF-Ajay 08-Jul-09 */
			if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED)==0) 
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE; 
				strcpy(Auth_Tx.TLF01_details.message_type, "1420" ); 
			
			}
         if( (strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0) && 
			 ( 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"ncposA",6) &&
			   0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos2A",7) ))
		 {
            /* Build fields necessary for 1420/1400  ATM reversal REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            /* Bit field 3 is not required for ATM reversal request */
			//(void)build_request_field_3(  &result_code, err_buf ); // TF Phani
            /*send bit 3 for incoming partial reversal transactions AMEX03A*/
            if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatchA",6))
            {
            	(void)build_request_field_3(  &result_code, err_buf );
            }
            (void)build_request_field_4(  &result_code, err_buf );
            (void)build_request_field_5(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
            (void)build_request_field_24( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_26( &result_code, err_buf );
            (void)build_request_field_30( &result_code, err_buf );  
            (void)build_request_field_31( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
            (void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_37( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
			(void)build_request_field_42( &result_code, err_buf ); // TF Phani

            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_50( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
            (void)build_request_field_56( &result_code, err_buf );

            /* Need to make sure the RRN of a reversal is the
             * same as the RRN of the original transaction.
             * EB needs it logged into TLF01 same as original RRN.
             */
            memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
                    Auth_Tx.TLF01_details.orig_retrieval_ref_num,
                    12 );
         }
         else 
         {
            /* Build fields necessary for 1420/1400 POS reversal REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
            (void)build_request_field_5(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_24( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
#if AMEX03B_APRIL2024
            if(acq_amex_variable_auth_flag == 'Y'||
               acq_amex_variable_auth_flag == 'N')
            {
            	(void)build_request_field_26( &result_code, err_buf );
            	(void)build_request_field_30( &result_code, err_buf );
            }
#endif
            else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION &&
            	Auth_Tx.TLF01_details.saf[0] == INCREMENTAL_AUTH)
            {
                (void)build_request_field_26( &result_code, err_buf );
                (void)build_request_field_30( &result_code, err_buf );
            }
            (void)build_request_field_31( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
            (void)build_request_field_37( &result_code, err_buf );
            (void)build_request_field_38( &result_code, err_buf );
			(void)build_request_field_41( &result_code, err_buf );// TF Phani
            (void)build_request_field_42( &result_code, err_buf );
            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_50( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_56( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );

            /* Need to make sure the RRN of a reversal is the
             * same as the RRN of the original transaction.
             * EB needs it logged into TLF01 same as original RRN.
             */
            memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
                    Auth_Tx.TLF01_details.orig_retrieval_ref_num,
                    12 );
         }
      }
       else if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1304",4))	/* Girija Y, ThoughtFocus - Added these 
		                                                                    methods to process the request from cadencie host */
      {
         /* Build fields necessary for 1304  REQUEST messages. */
         (void)build_request_field_2(  &result_code, err_buf );
         (void)build_request_field_11( &result_code, err_buf );
         (void)build_request_field_12( &result_code, err_buf );
         (void)build_request_field_14( &result_code, err_buf );
         (void)build_request_field_24( &result_code, err_buf );
         (void)build_request_field_25( &result_code, err_buf );
         (void)build_request_field_33( &result_code, err_buf );
      }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s).",
                  Auth_Tx.TLF01_details.message_type );
         strcat( err_buf, "  Unable to build request message." );
      }
   }

   /* Flag any errors that occurred while building the fields. */
   if ( result_code == 1 )
      ret_val = false;

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         PARSE_HOST_MSG
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into the HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_host_msg( pCHAR err_buf )
{
   struct bitfield_data *pBF;
   INT                   bfidx;    /* Index to bitfields for this transaction */
   INT                   ret_val = true;
   INT                   BitFieldSize;
   BYTE                  ReqRsp_Buffer_hex[17]="";
   BYTE                  bitmap;

   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   convert_to_ascii( ReqRsp_Buffer, HOST_MESSAGE.msgtype, 4 );
   /*ebcdic_to_hex(&ReqRsp_Buffer[4],&bitmap, 1 );*/
   bitmap = ReqRsp_Buffer[4];
     
   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   
   if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 20;                         /* Extended     */
	 
	  /*ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 32 );*/
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      reqidx = 12;                         /* Not Extended */
	 
	  /*ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 16 );*/
      BitFieldSize = NORMAL_BITFIELD;
   }

  
   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      //if ( ReqRsp_Buffer_hex[BITTAB[bfidx].byte_no] &
	   if ( ReqRsp_Buffer[bitmapidx + BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table(bfidx);

         if ( NULL == pBF )
         {
            sprintf( err_buf, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( err_buf, " MsgType = " );
            strncat( err_buf, HOST_MESSAGE.msgtype, 4 );
            ret_val = false; /* Invalid field in message - so quit. */
            break;
         }

         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         if ( false == (ret_val = (pBF->bd_BFRsp)(pBF->bd_fieldid)) )
         {
            if ( true == invalid_msg_format )
            {
               strcpy( err_buf, "Invalid  message format." );
               strcat( err_buf, " MsgType = " );
               strncat( err_buf, HOST_MESSAGE.msgtype, 4 );
               break;
            }
            else
            {
               sprintf( err_buf,
                    "Unable to parse field %d in msg from host, MsgType: %4.4s",
                     bfidx, HOST_MESSAGE.msgtype );
            }
         }
      }
   }
   return( ret_val );
} /* parse_host_msg */  

/******************************************************************************
 *
 *  NAME:         POPULATE_AUTH_TX
 *
 *  DESCRIPTION:  This function transfers information from the HOST_MESSAGE
 *                structure that contains data from the host into Auth_Tx.
 *
 *  INPUTS:       HOST_MESSAGE - (Global)Data from host
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *                Auth_Tx - (Global)Transaction data
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred, such as
 *                a numeric field is not numeric
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT populate_auth_tx( pCHAR err_buf )
{
   INT     ret_val = true;
   INT     result_code = 0;  /* 0 = no error, 1 = error */

   if ( TranFlow == SWITCHED_IN )
   {
      /*----------------------------------*/
      /* This is a request from the host. */
      /*----------------------------------*/
	   if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1100",4) )
	   {

		   (void)move_field_2(  &result_code, err_buf );
		   (void)move_field_3(  &result_code, err_buf );
		   (void)move_field_4(  &result_code, err_buf );
		   (void)move_field_5(  &result_code, err_buf );
		   (void)move_field_11( &result_code, err_buf );
		   (void)move_field_12( &result_code, err_buf );
		   (void)move_field_14( &result_code, err_buf );
		   (void)move_field_19( &result_code, err_buf );
		   (void)move_field_22( &result_code, err_buf );
		   (void)move_field_24( &result_code, err_buf );
		   (void)move_field_26( &result_code, err_buf );
		   (void)move_field_31( &result_code, err_buf );
		   (void)move_field_32( &result_code, err_buf );
		   (void)move_field_37( &result_code, err_buf );
		   (void)move_field_41( &result_code, err_buf );
		   (void)move_field_42( &result_code, err_buf );
		  // (void)move_field_43( &result_code, err_buf );
		   (void)move_field_49( &result_code, err_buf );
		   (void)move_field_50( &result_code, err_buf );
		   //(void)move_field_53( &result_code, err_buf );
		   (void)move_field_55( &result_code, err_buf );
		   //(void)move_field_61( &result_code, err_buf );
		   //(void)move_field_62( &result_code, err_buf );

	   }
	   else if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         /* Move fields of 18nn network REQUEST messages. */
         (void)move_field_3(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_12(  &result_code, err_buf );
         (void)move_field_24(  &result_code, err_buf );
         (void)move_field_33(  &result_code, err_buf );

         strncpy(NMI_Code,Auth_Tx.function_code,3);
      }
	  else if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1304",4))
	  {
		  /* Move fields of 1304 File Update REQUEST message */
		 (void)move_field_2(   &result_code, err_buf );
         (void)move_field_11(  &result_code, err_buf );
         (void)move_field_12(  &result_code, err_buf );
		 (void)move_field_14(  &result_code, err_buf );
         (void)move_field_24(  &result_code, err_buf );
		 (void)move_field_25(  &result_code, err_buf );
         (void)move_field_33(  &result_code, err_buf );
	  }
	  else if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1420",4))
	  {
		  (void)move_field_2(  &result_code, err_buf );
		  (void)move_field_3(  &result_code, err_buf );
		  (void)move_field_4(  &result_code, err_buf );
		  (void)move_field_11( &result_code, err_buf );
		  (void)move_field_12( &result_code, err_buf );
		  (void)move_field_14( &result_code, err_buf );
		  (void)move_field_19( &result_code, err_buf );
		  (void)move_field_22( &result_code, err_buf );
		  (void)move_field_24( &result_code, err_buf );
		  (void)move_field_25( &result_code, err_buf );
		  (void)move_field_31( &result_code, err_buf );
		  (void)move_field_32( &result_code, err_buf );
		  (void)move_field_37( &result_code, err_buf );
		  (void)move_field_38( &result_code, err_buf );
		  (void)move_field_41( &result_code, err_buf );// TF Phani
		  (void)move_field_42( &result_code, err_buf );
		  (void)move_field_49( &result_code, err_buf );
		  (void)move_field_50( &result_code, err_buf );
		  (void)move_field_55( &result_code, err_buf );
		  (void)move_field_56( &result_code, err_buf );
	  }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s) from host.",
                  Auth_Tx.TLF01_details.message_type );
      }
   }
   else  /* SWITCHED OUT Transactions */
   {
      /*-----------------------------------*/
      /* This is a response from the host. */
      /*-----------------------------------*/

      if (Auth_Tx.TLF01_details.message_type[1] == '1') 
      {
         if(Auth_Tx.TLF01_details.message_type[2] == '1')
         {
            /* Move fields necessary for 111n RESPONSE messages. */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_7(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_24( &result_code, err_buf );
            (void)move_field_31( &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_38( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
			(void)move_field_44( &result_code, err_buf ); /* Girija Y - ThoughtFocus */
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
         }
         else if(Auth_Tx.TLF01_details.message_type[2] == '3')
         {
            /* Move fields necessary for 113n RESPONSE messages. */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_31( &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
         }
         else
         {
            /* Unsupported message type.  Log an error. */
            ret_val = false;
            sprintf( err_buf,
                    "Unsupported message type (%s) from host.",
                     Auth_Tx.TLF01_details.message_type );
         }
      }
      else if(Auth_Tx.TLF01_details.message_type[1] == '2') 
      {
         /* Move fields necessary for 12nn response*/
         (void)move_field_2(  &result_code, err_buf );
         (void)move_field_3(  &result_code, err_buf );
         (void)move_field_4(  &result_code, err_buf );
         (void)move_field_5(  &result_code, err_buf );
         (void)move_field_11( &result_code, err_buf );
         (void)move_field_12( &result_code, err_buf );
         (void)move_field_19( &result_code, err_buf );
         (void)move_field_22( &result_code, err_buf );
         (void)move_field_24( &result_code, err_buf );
         (void)move_field_26( &result_code, err_buf );
         (void)move_field_31( &result_code, err_buf );
         (void)move_field_32( &result_code, err_buf );
         (void)move_field_33( &result_code, err_buf );
         (void)move_field_37( &result_code, err_buf );
         (void)move_field_38( &result_code, err_buf );
         (void)move_field_39( &result_code, err_buf );
         (void)move_field_41( &result_code, err_buf );
         (void)move_field_49( &result_code, err_buf );
         (void)move_field_50( &result_code, err_buf );
		 (void)move_field_55( &result_code, err_buf );
      }
      else if ( is_reversal() )
      {
         if( 0 == strncmp(ServiceName,cadencie_atm_que_name,6) ) // TF Phani ..Now check with cadience quename
         {
            /* Move fields necessary for 14nn ATM reversal RESPONSE messages. */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_5(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_19( &result_code, err_buf );
            (void)move_field_31( &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_33( &result_code, err_buf );
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
            (void)move_field_50( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );

			(void)move_field_56( &result_code, err_buf );
         }
         else
         {
            /* Move fields necessary for 14nn POS reversal RESPONSE messages. */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_31( &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
         }
      }
      else if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         /* Move fields necessary for 18nn network RESPONSE messages. */
         (void)move_field_3(  &result_code, err_buf );
         (void)move_field_11( &result_code, err_buf );
         (void)move_field_12( &result_code, err_buf );
         (void)move_field_24( &result_code, err_buf );
         (void)move_field_33( &result_code, err_buf );
         (void)move_field_39( &result_code, err_buf );

         strncpy( NMI_Code, Auth_Tx.function_code, 3 );
      }
      else if(Auth_Tx.TLF01_details.message_type[1] == '3') 
      {
         /* Move fields necessary for 13nn response*/
         (void)move_field_2(  &result_code, err_buf );
         (void)move_field_11( &result_code, err_buf );
         (void)move_field_12( &result_code, err_buf );
         (void)move_field_24( &result_code, err_buf );
         (void)move_field_33( &result_code, err_buf );
         (void)move_field_39( &result_code, err_buf );
      }
      else
      {
         /* Unsupported message type.  Log an error. */
         ret_val = false;
         sprintf( err_buf,
                 "Unsupported message type (%s) from host.",
                  Auth_Tx.TLF01_details.message_type );
      }
   }

   /* Flag any errors that occurred while building the fields. */
   if ( result_code == 1 )
      ret_val = false;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PARSE_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 data.  It looks for the first
 *                non-numeric character to determine the field separator.
 *                It copies the card number & expiration date into auth_tx.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      Auth_Tx - (Global)Updated with card number and exp date
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_track2()
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   pBYTE  field_separator = NULL_PTR;

   if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = Auth_Tx.TLF01_details.track2;
      for(track2_idx=0;track2_idx <= MAX_DE2_SIZE;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( (track2_idx > MAX_DE2_SIZE) || (*field_separator == 0x00) )
      {
         ret_val = false;
      }
      else
      {
         /* Put Card Number into auth_tx. */
         card_num_len = field_separator - Auth_Tx.TLF01_details.track2;
         strncpy( Auth_Tx.TLF01_details.card_num,
                  Auth_Tx.TLF01_details.track2,
                  card_num_len );

         /* Get the expiration date. */
         memcpy( Auth_Tx.TLF01_details.exp_date, field_separator+1, 4 );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         REMOVE_TRACK2_END_SENTINEL
 *
 *  DESCRIPTION:  This function checks the last non-space character of the
 *                input string for the End Sentinal character. If it exists
 *                it is changed to a NULL.
 *
 *  INPUTS:       track2 - String containing Track 2 data
 *
 *  OUTPUTS:      track2 - Same string, but without the End Sentinel
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void remove_track2_end_sentinel( pBYTE track2 )
{
   #define  END_SENTINEL  '?'

   INT  len;

   /* Trim trailing spaces */
   trim_trailing_spaces( track2 );

   len = strlen( track2 );

   if ( track2[len-1] == END_SENTINEL )
   {
      /* End Sentinel does exist. Change it to NULL. */
      track2[len-1] = 0x00;
   }
   return;
}



/******************************************************************************
 *
 *  NAME:         BUILD_RESPONSE_MSG
 *
 *  DESCRIPTION:  This function calls 2 functions: one to gather the response
 *                data from Auth_Tx or create it and place it into structure
 *                HOST_MESSAGE, then another to take the data from HOST_
 *                MESSAGE and copy it into the data string that will be
 *                going to the host.  This second function will also insert
 *                the message type and create the bit map.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      ReqRsp_Buffer - (Global)Data stream to go to host
 *
 *  RTRN VALUE:   True if no problems, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_msg()
{
   INT   ret_val=true;
   CHAR  err_buf[100]="";

   if ( true == (ret_val = build_host_structure(err_buf)) )
   {
      ret_val = build_host_msg( err_buf );
   }

   /* Log any errors. */
   if ( ret_val == false )
      natma2_log_message( 1, 2, err_buf, "build_response_message",1 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         BUILD_REQUEST_MSG
 *
 *  DESCRIPTION:  This function calls 2 functions: one to gather the txn
 *                data from Auth_Tx or create it and place it into structure
 *                HOST_MESSAGE, then another to take the data from HOST_
 *                MESSAGE and copy it into the data string that will be
 *                going to the host.  This second function will also insert
 *                the message type and create the bit map.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      err_buf       - Error message
 *                ReqRsp_Buffer - (Global)Data stream to go to host
 *
 *  RTRN VALUE:   True if no problems, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_msg( pCHAR err_buf )
{
   INT   ret_val=true;

   if ( true == (ret_val = build_host_structure(err_buf)) )
   {
	  // TF Phani ..change the tx_key .. Changeing to diffretiate the POS REVRSAL and ATM reversals.
	  if(((Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
         (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)) 
		  && (strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0))

	  { /*Only for ATM reversal should be changed to ATM CONFIRM*/
		  if(0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"ncposA",6) &&
			 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos2A",6) &&
			 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"sbatch",6))
		  {
				Auth_Tx.TLF01_details.tx_key = AUTH_ATM_CONFIRM; 
		  }
	  } 

      ret_val = build_host_msg( err_buf );
   }

   return( ret_val );
 
}

/******************************************************************************
 *
 *  NAME:         CREATE_STAN
 *
 *  DESCRIPTION:  This function creates a unique 6-digit system trace audit
 *                number (stan).  It uses the last 6 digits of the time -
 *                number of seconds since 1-1-1970.  This is unique for a
 *                cycle of 11 days.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      stan - Six digit stan
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_stan( pBYTE stan )
{
   INT   len;
   BYTE  time_in_seconds[30]="";

   ptetime_get_strtime( time_in_seconds );
   len = strlen( time_in_seconds );
   if ( len < 10 )
      len = 10;

   /* Time is in format nnnnnnnnn.nnn
    * I want the last 6 n's before the decimal point.
    */
   memcpy( stan, &time_in_seconds[len-10], 6 );
   return;
}

/******************************************************************************
 *
 *  NAME:         natma2_log_message
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
void natma2_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]="";
   CHAR text_message[512]="";
   INT  Appname_len = 0;

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

   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   /* Make sure text message is not too long. */
   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len) );
   natma2_Log_error_warning_to_File(text_message,sev,func,details);
   /* Call function to post the message. */
   if(monitor_flag == 1)
   {
   	   if(details == 1)
       {
   		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
                                 eventlog_flag, msg_type, func,
                                 severity, log_type,
                                 Auth_Tx.TLF01_details.terminal_id,
                                 Auth_Tx.TLF01_details.card_num,
                                 Auth_Tx.TLF01_details.merchant_id );
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

/******************************************************************************
 *
 *  NAME:         send_network_status_to_monitor
 *
 *  DESCRIPTION:  This procedure will send a message to the monitor to inform
 *                it of a change in host status.
 *
 *  INPUTS:       NetworkType - (Global) Issuer, Acquirer, or BOTH
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 ******************************************************************************/
INT send_network_status_to_monitor()
{

   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   INT         ret_code = true;
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   CHAR        err_msg[100];
   XINT        ReturnCode = 0 ;
   int         nIndex = 0 ;
 
   /* Initialize structure to be sent to Monitor. */
   memset( &ncf01, 0, sizeof(NCF01) );

   /* Get the current state (online or offline) from shared memory. */
   read_shared_mem_table( net_consec_tmouts_ctr,active_txns_ctr,
                          current_state, err_msg);

   /* Get Monitor connection information from shared memory. */
   memset (ConnectionId, 0, sizeof (ConnectionId)) ;
   ReturnCode = ReadMemMonitorTable (ConnectionId) ;
   if (ConnectionId[0] != 0)
   {
      if ( NetworkType == ACQUIRER  ||  NetworkType == BOTH )
      {
         strcpy( ncf01.primary_key.network_id,    AppName      );
         strcpy( ncf01.primary_key.network_type, "A"           );
         strcpy( ncf01.status,                   current_state );

         ret_code = build_and_send_request_message( MT_SYS_MONITOR,
                                                    ST1_NETWORK_UPDATE,
                                                    ST2_NONE, 
                                                    applnk_que_name, 
                                                    (pBYTE)&ncf01, 
                                                    Ncf01_Size,
                                                    0,
                                                    ConnectionId,
                                                    err_msg );
         if ( ret_code == false )
         {
            natma2_log_message( 1, 2, err_msg, "send_network_status_to_monitor",0 );
         }
      }
      if ( ((NetworkType == ISSUER) || (NetworkType == BOTH)) && ret_code )
      {
         if ( NetworkType == BOTH )
         {
            /* The time to sleep is the same here,
             * just different units for NT vs UNIX.
             * The purpose is to not flood Monitor.
             */
            #ifdef WIN32
               Sleep( 500 );
            #else
               usleep( 500000 );
            #endif
         }

         strcpy( ncf01.primary_key.network_id,    AppName      );
         strcpy( ncf01.primary_key.network_type, "I"           );
         strcpy( ncf01.status,                   current_state );

         ret_code = build_and_send_request_message( MT_SYS_MONITOR,
                                                    ST1_NETWORK_UPDATE,
                                                    ST2_NONE, 
                                                    applnk_que_name, 
                                                    (pBYTE)&ncf01, 
                                                    Ncf01_Size,
                                                    0,
                                                    ConnectionId,
                                                    err_msg );
         if ( ret_code == false )
            natma2_log_message( 1, 2, err_msg, "send_network_status_to_monitor",0 );
      }
   }
   else
   {
      natma2_log_message( 1, 2, "Unable to get connection info for Monitor.",
                  "send_network_status_to_monitor",0 );
   }
   return( ret_code );

}  /* send_network_status_to_monitor */

/******************************************************************************
 *
 *  NAME:         SEND_MESSAGE_TO_HOST
 *
 *  DESCRIPTION:  This function sends a message to the host.
 *
 *  INPUTS:       ReqRsp_Buffer   - (Global)Data stream going to the host
 *                buflen          - (Global)Length of ReqRsp_Buffer
 *                Ncf01_I.tpdu_id - (Global)Comm info
 *                dialog_que_name - (Global)Queue name of dialog service
 *
 *  OUTPUTS:      err_buf - Text message if an error occurs
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_message_to_host( pCHAR err_buf,INT mgt ) //praneeth added bool mgt as parameter
{
   INT   ret_val;
   CHAR  buffer[100]="";
   INT      buf_temp_len=0;


   if(true == nposa2_check_for_cadencieacq_transaction())
   {
		buf_temp_len = atoi(Auth_Tx.req_data_len);
		if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
		{
			buflen = buf_temp_len;
			memcpy(ReqRsp_Buffer,Auth_Tx.req_buffer,buflen);
		}
   }

   ret_val = build_and_send_request_message( MT_OUTGOING,
                                             ST1_NONE,
                                             ST2_NONE,
                                             dialog_que_name,
                                             ReqRsp_Buffer,
                                             buflen,
                                             0,
                                             Ncf01_I.tpdu_id,
                                             buffer );

   if ( ret_val == false )
   {
      sprintf( err_buf,
              "Unable to send %s msg to host. Err: %s",
               Auth_Tx.TLF01_details.message_type, buffer );
   }
   else
   {
	   if(!mgt)
	   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, &Auth_Tx, SENT );
	   }
	   else
	   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, &Auth_Tx, MGT );
	   }

      /* Log transaction to trace file. */
      write_to_txn_file();

      /* If a request, need to increment number of txns
       * for which we have not received a response yet.
       */
      if ( Auth_Tx.TLF01_details.tx_key < AUTH_INVALID )
      {
         /* This is a request. */
         if ( false == increment_active_count(err_buf) )
            natma2_log_message(1, 2, err_buf, "send_message_to_host" ,0);
      }

      /* Cycle the echo timer because traffic went out over the line. */
      if ( false == reset_echo_timer(err_buf) )
         natma2_log_message(1, 2, err_buf, "send_message_to_host",0 );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PIN_BLOCK_EXISTS
 *
 *  DESCRIPTION:  This function determines if a pin block exists.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if pin exists, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT pin_block_exists()
{
   INT  ret_val = true;

   if ( 0 == strlen(Auth_Tx.TLF01_details.pin_block) )
      ret_val = false;

   else if (blank_string(Auth_Tx.TLF01_details.pin_block,
                         sizeof(Auth_Tx.TLF01_details.pin_block)-1) )
      ret_val = false;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PERFORM_ERROR_PROCESSING
 *
 *  DESCRIPTION:  This procedure takes care of transactions that were unable
 *                to get sent to the host due to some type of error.  The
 *                auth_tx structure gets updated in here and then the function
 *                returns.
 *
 *  INPUTS:       Auth_Tx   - (Global)Transaction Info
 *                resp_code - Response code to send to msg initiator
 *
 *  OUTPUTS:      Auth_Tx - (Global)Structure containing the updated txn record
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void perform_error_processing( pCHAR resp_code )   
{
   CHAR       date_str [ 9] = "";
   CHAR       time_str [ 7] = "";
   CHAR       time_date[25] = "";

   /* Get local time. */
   ptetime_get_timestamp( time_date );
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Create RRN and store it in the auth_tx block. */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.retrieval_ref_num) )
   {
      create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
   }

   /* Create response code and store it in the auth_tx block */
   strcpy( Auth_Tx.TLF01_details.response_code, resp_code );
   strncpy(  Auth_Tx.TLF01_details.auth_number, "",6);
   /* Create date of transaction and store it in the auth_tx table,
    * format "YYYY0M0D".
    */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.date_yyyymmdd) )
      strcpy( Auth_Tx.TLF01_details.date_yyyymmdd, date_str );

   /* Create time of transaction and store it in the auth_tx table,"0H240I0S" */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.time_hhmmss) )
      strcpy( Auth_Tx.TLF01_details.time_hhmmss, time_str );

   Auth_Tx.TLF01_details.general_status = GS_TXN_DROPPED;

   /* Make sure the tx_key is for a response. */
   if ( Auth_Tx.TLF01_details.tx_key < 100 )
      Auth_Tx.TLF01_details.tx_key += 100;

   (void)send_msg_to_authorizer( MT_AUTH_RESPONSE );
   return;
}

/******************************************************************************
 *
 *  NAME:         PERFORM_STANDIN_PROCESSING
 *
 *  DESCRIPTION:  This procedure processes transactions that cannot or will
 *                not go to the host.  This can be offline txns that should
 *                not go to the host.  Or they can be txns that will not go
 *                due to too many timeouts or too many active txns.
 *
 *  INPUTS:       rsp_code   - Response code to place into the auth_tx block
 *                gen_status - Txn status to place into the auth_tx block
 *                Auth_Tx    - (Global)Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err_buf )
{
   INT   ret_val;
   CHAR  time_date[25] = "";

   /* Copy in response code and create an auth number if approved. */
   strcpy( Auth_Tx.TLF01_details.response_code, rsp_code );

   if ( 0 == strcmp(rsp_code, ISO_APPROVED) )
   {
	   
	   strcpy(Auth_Tx.TLF01_details.response_text, "APPROVED_STAND_IN");
      if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
      {
         if ( Auth_Tx.TLF01_details.time_hhmmss[0] != 0x00 )
         {
            strcpy( Auth_Tx.TLF01_details.auth_number,
                    Auth_Tx.TLF01_details.time_hhmmss );
         }
         else
         {
            /* Get local time. */
            ptetime_get_timestamp( time_date );
            get_time( time_date, Auth_Tx.TLF01_details.auth_number );
         }
      }
   }

   /* Change status to completed and store it in the auth_tx block */
   Auth_Tx.TLF01_details.general_status = gen_status;

   /* Set transaction key to be that of a response for the authorizer's sake */
   (void)set_tx_key_to_response();

   /* Send response to the authorizer */
   ret_val = send_msg_to_authorizer( MT_AUTH_RESPONSE );
   if ( false == ret_val )
   {
      sprintf( err_buf,
              "Unable to send response to txcntl. MsgType: %s, Proc Code: %s",
               Auth_Tx.TLF01_details.message_type,
               Auth_Tx.TLF01_details.processing_code );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         VERIFY_HOST_STATUS
 *
 *  DESCRIPTION:  This procedure will check on all the counters and the state
 *                of the host application to determine whether we can send this
 *                request to the host or not.
 *
 *                If maximum number of consecutive timeouts is configured to
 *                zero, ignore the count.  Same for maximum active transactions.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction record
 *
 *  OUTPUTS:      err_buf - Text message in case of errors
 *
 *  RTRN VALUE:   False if host is not online.  Else true.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT  verify_host_status( pCHAR err_buf )
{
   INT   ret_val;

   /* Make sure host is active. */
   ret_val = read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                                    current_state, err_buf );
   if ( current_state[0] != cONLINE )
   {
	  strcpy( err_buf,
                   "Host is not Online." );
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Check number of consecutive time outs.
       * If too many, issue a warning.
       */
      if ( MaxConsecTimeouts > 0 )
      {
         if (atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts )
         {
            strcpy( err_buf,
                "Maximum consecutive timeouts reached.");

            natma2_log_message( 1, 2, err_buf, "verify_host_status",1 );

            ret_val = false;
            if ( false == set_host_state(OFFLINE,err_buf) )
               natma2_log_message( 1, 3, err_buf, "verify_host_status" ,1);
         }
      }

      /* Check number of active transactions.
       * If too many, issue a warning.
       */
      if ( (MaxActiveTxns > 0) && (ret_val == true) )
      {
         if( atoi(active_txns_ctr) >= MaxActiveTxns )
         {
            strcpy( err_buf,
                   "Maximum active txns reached. Continuing to process." );
            natma2_log_message( 1, 2, err_buf, "verify_host_status",1 );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         IS_REVERSAL
 *
 *  DESCRIPTION:  This function determines if a transaction is a reversal.
 *                Note that an ATM Confirm (message type 0102) is treated
 *                like a reversal - a partial reversal from Visa.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing txn information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if txn is a reversal, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_reversal()
{
   INT   ret_val = false;

   if ( Auth_Tx.TLF01_details.message_type[1] == '4' )
      ret_val = true;
   else if((Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE) || 
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM))
	   ret_val = true;
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
	   ret_val = true;
   return( ret_val );
}

INT natma2_is_POS_Online_PIN_transction()
{
	INT ret_val = true;

	/* Check if txn has F52, F64 and F22 pos 3 is '1' */
	if (true == pin_block_exists() &&
		'\0' != Auth_Tx.TLF01_details.product_codes[4].amount[0] &&
		 '1' == Auth_Tx.TLF01_details.pos_entry_mode[3] )
	{
		ret_val = true;
	}
	else
	{
		ret_val = false;
	}
	return (ret_val );
}


INT natma2_is_Incoming_Online_PIN_transction()
{
	INT ret_val = true;

	/* Check if txn is coming from AMEX Scheme, F52 and F22 pos 3 is '1' */
	if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncpos", 5) &&
		true == pin_block_exists() &&
		 '1' == Auth_Tx.TLF01_details.pos_entry_mode[3] )
	{
		ret_val = true;
	}
	else
	{
		ret_val = false;
	}
	return (ret_val );
}
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
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,buflen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         natma2_log_message( 1, 1, err_buf, "process_host_msg",1 );
         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         natma2_log_message( 1, 1, err_buf, "process_host_msg",1 );

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            natma2_log_message( 1, 1, err_buf, "process_host_msg",1 );
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         MAP_ISO_TO_AMEX_MESSAGE_TYPE
 *
 *  DESCRIPTION:  This function converts the iso message types to Amex message types.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      Amex message type
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Sree Velaga
 *
 ******************************************************************************/
INT map_iso_to_amex_message_type( pCHAR err_buf,pCHAR amex_message_type)
{
   INT     ret_val = true;

   if ( strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1100",4) == 0 )
   {
	   if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
		   strcpy(amex_message_type,"1420");
	   else
		   strcpy(amex_message_type,"1100");
   }
   else if ((Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND)       ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
            (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE))
   {
      strcpy(amex_message_type,"1420");
   }
   else if(strncmp(Auth_Tx.TLF01_details.message_type,"0200",4) == 0)
   {
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REFUND) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE)  )
      {
         strcpy(amex_message_type,"1120");
      }
      else if (Auth_Tx.TLF01_details.tx_key == AUTH_SALE ||
    		  Auth_Tx.TLF01_details.tx_key ==  AUTH_QUASI_CASH)
         strcpy(amex_message_type,"1100");
      else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE)
      {
         if( strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0)
            strcpy(amex_message_type,"1200");
         else
            strcpy(amex_message_type,"1100");
      }
      else
         strcpy(amex_message_type,"1200");
   }
   else if ( (strncmp(Auth_Tx.TLF01_details.message_type,"0400",4) == 0) ||
             (strncmp(Auth_Tx.TLF01_details.message_type,"0420",4) == 0)  )
   {
      strcpy(amex_message_type,"1420");
   }

   else if ( strncmp(Auth_Tx.TLF01_details.message_type,"0800",4) == 0 )
   {
      strcpy(amex_message_type,"1804");
   }
   else if ( strncmp(Auth_Tx.TLF01_details.message_type,"1304",4) == 0 )
   {
      strcpy(amex_message_type,"1304");
   }
   else if ( (strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) == 0) ||
   		   (strncmp(Auth_Tx.TLF01_details.message_type,"0121",4) == 0))
   {
	    strcpy(amex_message_type,"1120");
   }
   else if ( strncmp(Auth_Tx.TLF01_details.message_type,"1314",4) == 0 ) /* Girija Y - ThoughtFocus */
   {
	    strcpy(amex_message_type,"1314");
   }
   else if (strncmp(Auth_Tx.TLF01_details.message_type,"1420",4) == 0) // TF phani - for 1420
   {
	    strcpy(amex_message_type,"1420");
   }
   else
   {
      ret_val = false;
      sprintf( err_buf,
              "Unsupported message type (%s). map_iso_to_amex_message_type",
               Auth_Tx.TLF01_details.message_type );
   }

   return ret_val;
}


/******************************************************************************
 *
 *  NAME:         MAP_AMEX_ACTION_CODE_TO_ISO_RESPONSE_CODE
 *
 *  DESCRIPTION:  This function converts the iso message
 *                types to Amex message types.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Sree Velaga
 *
 ******************************************************************************/
INT map_amex_action_code_to_iso_repsonse_code(pCHAR amex_response_code )
{
   #define  NUMBER_OF_ACTION_CODES   32

   INT     ret_val = true;
   INT     i;
   INT     found = false;

   CHAR    iso_response_codes[NUMBER_OF_ACTION_CODES][3] =
                             {"00",  "00",  "00",  "05",  "54",  "62",  "75",  "01",
                              "13",  "12",  "51" , "55",  "51",  "05", "05",   "57",
							  "43",  "62",  "00",  "00",  "00",  "30",  "91",  "91",
							  "94",  "40",  "00",  "40",  "25",  "80",  "27", "63"};

   CHAR    action_codes_amex[NUMBER_OF_ACTION_CODES][4] =
                             {"000", "001", "002", "100", "101", "104", "106", "107",
                              "110", "111", "116", "117", "121", "122", "125", "194",
							  "200", "290", "400", "800", "900", "904", "911", "912",
							  "913", "115", "300", "301", "302", "303", "304", "380"};


   if(amex_response_code[0] != 0x00)
   {
      for (i=0; i<NUMBER_OF_ACTION_CODES; i++)
      {
         if (0 == strncmp(amex_response_code, action_codes_amex[i], 3))
         {
            strncpy(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2);
            found = true;
            break;
         }
      }	

      if ( found == false )
         strncpy(Auth_Tx.TLF01_details.response_code,AMX_INVALID_TXN,2);
   }
   else
   {
      for (i=0; i<NUMBER_OF_ACTION_CODES; i++)
      {
         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, action_codes_amex[i], 3);
            break;
         }
      }	
   }

   return ret_val;
}


void natma2_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  natma2_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   natma2_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(natma2_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  natma2_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(natma2_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  natma2_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(natma2_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(natma2_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(natma2_error_warning_file_path) > 0) &&
	  (strlen(natma2_module_error_warning_file_name)> 0))
   {
	  // nciftr_create_Error_Filename();
	   natma2_create_Error_Filename();
   }
}

INT natma2_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
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
    len = strlen(natma2_Erro_warning_Filename);
    path_len = strlen(natma2_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&natma2_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	//nciftr_create_Error_Filename();
    	natma2_create_Error_Filename();
    }
	if((fp = fopen(natma2_Erro_warning_Filename,"a+b"))==NULL)
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
	if(detail == 1)
	{
		if (strlen(Auth_Tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( Auth_Tx.TLF01_details.card_num );
			strncpy( tempcard, Auth_Tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
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

void natma2_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( natma2_Erro_warning_Filename, natma2_error_warning_file_path );
    strcat( natma2_Erro_warning_Filename, natma2_module_error_warning_file_name );
	strcat( natma2_Erro_warning_Filename, ".EW." );
    strncat(natma2_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(natma2_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(natma2_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

BOOLEAN natma2_Is_AsiaPay_Transaction()
{
	if((0==strncmp(Auth_Tx.TLF01_details.message_type, "0200",4)) &&
	   (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC,2)) &&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,ECOM_POS_ENTRYMODE,3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
/*****************************************************************************

  Function:     nposa2_insert_tlf01()

  Description: This Function insert transaction details into tlf01 tables

  Author:
      Abhishek Verma
  Inputs:
      p_auth_tx - Transaction record
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
INT nposa2_insert_tlf01(pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)] = {0};
   CHAR       Buff[256] = {0};
   CHAR       err_mesg[200] = {0};
   LONG       ret_code = 0;

   /* Check to see if its already been inserted into TLF01. */
   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
      return(true);

   p_auth_tx->host2_acquirer_cb_code[0] = '*';

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
	   natma2_log_message( 1, 3, "Insufficient Memory to build insert TLF01 message",
	  			   				  "nposa2_insert_tlf01",1 );
      return(false);
   }
   ret_code = pteipc_send(p_msg_ins, updateds_que_name);
   if(ret_code < 0)
   {
      pteipc_get_errormsg(ret_code, err_mesg);
      sprintf(Buff, "failed transmit to que: %s. %s",
              updateds_que_name, err_mesg);
      natma2_log_message( 1, 3, Buff, "nposa2_insert_tlf01",1 );
      free(p_msg_ins);
      return(false);
   }
   free(p_msg_ins);
   return(true);
}

void natma2_read_TO_config_details_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[2]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION",           /* points to section name         */
									  "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION_AMEX",       	/* points to key name             */
									   "1",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   rvrsl_attmpts_value_fr_timeout_amex=atoi(readvalue);

	   if(rc == 0)
	   {
		   natma2_log_message( 2, 1, "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION_AMEX Value not read from tf.ini","natma2_read_tf_ini",0);
	   }

	   readvalue[0]='\0';
	   rc = GetPrivateProfileString(  "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION",              /* points to section name         */
									  "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION_AMEX",       	/* points to key name             */
									   "1",  														/* Default string                 */
									   readvalue,              										/* points to destination buffer   */
									   sizeof(readvalue),   	 									/* size of destination buffer     */
									   filename                  									/* points to ini filename         */
							 	   );

	   rvrsl_attmpts_value_fr_laterspns_amex=atoi(readvalue);

	   if(rc == 0)
	   {
		   natma2_log_message( 2, 1, "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION_AMEX Value not read from tf.ini","natma2_read_tf_ini",0);
	   }


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
void natma2_send_reversal_to_host_for_late_response( pCHAR timer_resp_flag )
{
	   INT   ret_val;
	   INT   seconds,buf_temp_len=0;
	   CHAR  err_buf[100]  ="";
	   CHAR  lookup_key[LOOKUP_KEY_SIZE]="";

	   TranFlow = SWITCHED_OUT;
	   Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
	   Auth_Tx.advice_repeat_count[0]='1';
	   ret_val = build_request_msg( err_buf );
	   if ( true == ret_val )
	   {
	      ptetime_get_strtime( Auth_Tx.TLF01_details.host_start_time );
			/*Send whatever msg coming from AMEX HOST. Make sure we have valid msg from HOST*/
			 if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0 && strcmp(ServiceName,"natma2") != 0)
			{
				buf_temp_len = atoi(Auth_Tx.req_data_len);
				if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
				{	buflen = buf_temp_len;
					memcpy(ReqRsp_Buffer,Auth_Tx.req_buffer,buflen);
				}
		          /* 1. After sending the msg to cadencie, clear the
		             * original msg present in the auth_tx Req_buffer,as it is not used after sending it to host.
		             * */
		        // memset(Auth_Tx.req_data_len,0x00,sizeof(Auth_Tx.req_data_len));
			 }



		  /* Set the request timer. */
	      (void)create_request_timer_key( lookup_key );
	      seconds = atoi(Ncf01_I.request_timer);
	      strcpy( TimerAppData2, timer_resp_flag );
	      ret_val = set_timer( lookup_key, seconds, REQUEST_TIMER );
	      if ( true == ret_val )
	      {
	         /* Send the transaction request to the host. */
	         ret_val = send_message_to_host( err_buf,false );//praneeth added false as parameter
	      }
	      else
	      {
	         sprintf( err_buf,
	                 "Cannot process %s txn request to host. Unable to set timer.",
	                  Auth_Tx.TLF01_details.message_type );
	      }
	   }

	   return;
}  /* send_reversal_to_host_for_late_response */


#if AMEX03B_APRIL2024
INT get_variable_auth_config_details()
{
	   CHAR  flag_value[2]="";
	   CHAR  date[9]={0};
	   INT   ret_val = true;
	   CHAR  filename   [80];
	   CHAR  tmpstr     [80];

	   /* Get path to the tf.ini file.*/
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   ret_val = GetPrivateProfileString(
	  								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
	  								  "ACQ_AMEX_VARIABLE_AUTH_FLAG",       	/* points to key name             */
	  								  "N",  					/* Default string                 */
									  flag_value,              	/* points to destination buffer   */
	  								  sizeof(flag_value)-1,   	 /* size of destination buffer     */
	  								  filename                  /* points to ini filename         */
	  								);

	   acq_amex_variable_auth_flag=flag_value[0];

	   ret_val = GetPrivateProfileString(
	 								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
	 								  "LAST_DATE",       	/* points to key name             */
	 								  "",  					/* Default string                 */
	 								  date,              	/* points to destination buffer   */
	 								  sizeof(date)-1,   	 /* size of destination buffer     */
	 								  filename                  /* points to ini filename         */
	 								);

	 	   strncpy(last_date,date,8);

	   return ret_val;
}
INT get_last_dated_date_only()
{
	   CHAR  date[9]={0};
	   INT   ret_val = true;
	   CHAR  filename   [80];
	   CHAR  tmpstr     [80];

	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   memset(last_date,0x00,sizeof(last_date));

	   ret_val = GetPrivateProfileString(
								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
								  "LAST_DATE",       	/* points to key name             */
								  "",  					/* Default string                 */
								  date,              	/* points to destination buffer   */
								  sizeof(date)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	   strncpy(last_date,date,8);
	   return ret_val;
}
void create_system_date( pBYTE system_date )
{
   CHAR  time_date[25] = "";

   memset( system_date,  0x00, 9 );

   /* Get local time - Format = "YYYY-0M-0D-0H.0I.0S.JJJ" */
   ptetime_get_timestamp( time_date );

   /* Format it. */
   strncpy( system_date,  time_date,    4 );
   strncat( system_date, &time_date[5], 2 );
   strncat( system_date, &time_date[8], 2 );
   return;
}
#endif

INT natma2_check_if_trx_is_bdo_pos_acq_trx( )
{
	 if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,
			 	 	  BDO_POS_ACQ_TXN_HANDLER_QUEUE,
					  BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


/*MobilePOS NFC Tap2Phone Trx*/
INT natma2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin()
{
	 if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					 POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
{
	if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			 	 	POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 AMEX_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 AMEX_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 AMEX_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT natma2_check_if_trx_is_amex_MobilePOS_NFC_Tap2Phone_trx_with_pin( )
{
	if((true == natma2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin()) &&
	   (true == natma2_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == natma2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_trx_is_amex_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
{
	if((true == natma2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin()) &&
	   (true == natma2_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == natma2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/*************************************/

/*Swiftpass BDO Pay Credit Trx*/
INT natma2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( )
{
	 if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			 	 	 POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( )
{
	if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 AMEX_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 AMEX_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 AMEX_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT natma2_check_if_trx_is_amex_Swiftpass_BDO_Pay_credit_trx_with_pin(  )
{
	if((true == natma2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin()) &&
	   (true == natma2_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == natma2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_trx_is_amex_Swiftpass_BDO_Pay_credit_trx_without_pin()
{
	if((true == natma2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin()) &&
	   (true == natma2_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == natma2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/*****************************/

/*Ecommerce Entry Mode From Swiftpass BDOPay Trx*/

INT natma2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 AMEX_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 AMEX_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 AMEX_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					 POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM,
					 POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
			   	   	 POS_ECOM_CONDITION_CODE,
					 POS_CONDITION_CODE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT natma2_check_if_trx_is_amex_ecommerce_swiftpass_BDOPay_trx( )
{
	if((true == natma2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx()) &&
	   (true == natma2_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == natma2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/***********************************************/


