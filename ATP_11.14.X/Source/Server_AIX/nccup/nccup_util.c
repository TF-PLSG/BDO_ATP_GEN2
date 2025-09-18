/******************************************************************************
 
 	ncequtil.c
 
    Copyright (c) 2004, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  CUP Host Handler Driver
 
 	This module is the host handler for the Equitable host.  It
    will process all messages destined to, or originated from,
    the CUP host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
AUTHOR : Abhishek Verma
   
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
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
#include "cup.h"
#include "nccup_prototypes.h"
#include "nccup_bitfields.h"
#include "txtrace.h"


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
extern CHAR    oprmon_que_name[];
extern CHAR    applnk_que_name[];
extern CHAR    authorizer_que_name[];
extern CHAR    timerds_que_name[];
extern CHAR    dialog_que_name[];
extern CHAR    nccup_issuing_link_name[];
extern CHAR    pos_handler_module_name[];
extern CHAR    voice_handler_que_name[];
extern CHAR    sbatch_handler_que_name[];
extern CHAR    cadencie_cup_queue_name[];



/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF01   Ncf01_Nccup3_I;
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
extern CHAR  LOGON_NMI_ECHO_TEST[5];
extern CHAR  LOGON_NMI_LOGON[5];
extern CHAR  LOGON_NMI_LOGOFF[5];

/*Flags*/
extern INT extendedbit;
extern INT Txn_Is_A_Reject;
extern INT tpdu_id_flag ;
extern CHAR static_Rtdcwk_Key[49];
extern CHAR MAC[17];
extern CHAR  nccup_Erro_warning_Filename[256] ;
extern CHAR  nccup_module_error_warning_file_name[256];
extern CHAR  nccup_error_warning_file_path[256];
UINT Max_File_Size_Defined = 0 ;

/* LOCAL VARIABLES */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    BitFieldSize;

extern CUP3DS2 cup3ds2;
char cup_scheme_timeout_rc [3]={0};
char cup_scheme_fai_rc [3]={0};
CHAR tran_dataserver_que_name [] = "trandsA" ;

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
 * AUTHOR:      Abhishek Verma
 *
 * MODIFICATIONS:
 ******************************************************************************/
void calc_julian_day( pCHAR date_str, pCHAR julian_str )
{
   CHAR  year[5] = {0};
   CHAR  month[3] = {0};
   CHAR  day[3] = {0};
   INT   iYear = 0;
   INT   iMonth = 0;
   INT   iDay = 0;
   INT   julian_day = 0;
   INT   i = 0;
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
 *  AUTHOR:       Abhishek Verma
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
 *  AUTHOR:       Abhishek Verma
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
 *  AUTHOR:       Abhishek Verma
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
 *  AUTHOR:       Abhishek Verma
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
	if(buflen >= COMM_BUF_SZE)
	{
		invalid_msg_format = true;
	}else
	{ memcpy(ReqRsp_Buffer, p_data, msglen);
		invalid_msg_format = false;
	}
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT determine_txn_flow( pCHAR msgtype, INT source )
{
   INT  tran_flow;

   /* Third character of msgtype is 1 or 3 for responses. */
   if ( (msgtype[2] == '1') || (msgtype[2] == '3') )
   {
      /* This is a response.  Where did it come from? */
      if ( source == CUP_HOST )
         tran_flow = SWITCHED_OUT;
      else
         tran_flow = SWITCHED_IN;
   }
   else
   {
      /* This is a request. */
      if ( source == CUP_HOST )
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
   INT buf_temp_len=0;

   TranFlow = SWITCHED_IN;
   if ( (send_flag == SEND_UNCONDITIONALLY) || ((send_flag == SEND_IF_NOT_REVERSAL) ) )
   {
      if ( is_reversal() )
         temp_tx_key = Auth_Tx.TLF01_details.tx_key;

      /* Make sure the tx_key is for a response. */
      if ( Auth_Tx.TLF01_details.tx_key < 100 )
         Auth_Tx.TLF01_details.tx_key += 100;

      if ( true == build_response_msg() )
      {
		 buf_temp_len = atoi(Auth_Tx.resp_data_len);
		 if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
		 {	
			if(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "nccup2",6))
			{
				buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
				sprintf (err_buf, "Sending Cadencie response to CUP Host, message type :%s", 
					Auth_Tx.TLF01_details.message_type) ;
				nccup_log_message( 2,1,err_buf,"send_response_to_host",1 );
			}
		 }      
         if ( false == send_message_to_host(err_buf,false) )//praneeth added false as parameter
            nccup_log_message(1, 2, err_buf, "send_response_to_host",1 );
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
   INT   seconds = 0 ;
   INT   buf_temp_len = 0;
   CHAR  err_buf[100]  = {0};
   CHAR  lookup_key[LOOKUP_KEY_SIZE] = {0};
   CHAR  gmt_time[20] = {0};
	
   TranFlow = SWITCHED_OUT;
   Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;

   
	/* Current Greenwich Mean Time for the transmission-to-host time. */
	if ( 0 == strcmp( NULL_STR,
					Auth_Tx.TLF01_details.transmission_timestamp ) )
	{
	 pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
	 strncpy( Auth_Tx.TLF01_details.transmission_timestamp,
			  &gmt_time[4], 10 );  /* MMDDhhmmss */
	}

   ret_val = build_request_msg( err_buf );
   if ( true == ret_val )
   {
		ptetime_get_strtime( Auth_Tx.TLF01_details.host_start_time );

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

   if ( false == ret_val )
   {
      /* Since the txn request did not go to the
       * host, we need to initiate a response.
       */
      nccup_log_message( 1, 2, err_buf, "send_request_to_host",1 );
      ret_val = perform_standin_processing( CUP_SYSTEM_ERROR,
                                            GS_TXN_DROPPED,
                                            err_buf );
      if ( false == ret_val )
         nccup_log_message( 2, 2, err_buf, "send_request_to_host",1 );
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT send_msg_to_authorizer( BYTE msgtype )
{
   INT   ret_val;
   CHAR  err_buf[100]="";
   CHAR  Buffer[200];

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
      nccup_log_message( 1, 3, Buffer, "send_msg_to_authorizer",1 );
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
 *  AUTHOR:       Abhishek Verma
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
      if ( length > CUP_MAX_HOST_MSG_SIZE )
         ret_code = false;

   else if ( length > CUP_MAX_APPL_MSG_SIZE )
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
 *  AUTHOR:      Abhishek Verma
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
   INT    ret_val;
   CHAR   unique_tran_str_id[21] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT     tid_len = 0;
   INT     unique_tran_str_id_len=0;

   /* Get local time. */
   if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
	 	ptetime_get_timestamp( time_date );
	 	get_date( time_date, date_str );
	 	get_time( time_date, time_str );
	 	strcpy( Auth_Tx.TLF01_details.date_yyyymmdd, date_str );
	 	strcpy( Auth_Tx.TLF01_details.time_hhmmss,   time_str );
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
				 Auth_Tx.TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
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
   Auth_Tx.TLF01_details.general_status      =  GS_TXN_NEW;
   Auth_Tx.TLF01_details.entry_type          =  FINANCIAL;

   
   strcpy( Auth_Tx.TLF01_details.dispensation.origin, "RO" );
   

   strcpy( Auth_Tx.TLF01_details.handler_queue,       application_que_name);
   strncpy( Auth_Tx.TLF01_details.tran_start_time,     unique_tran_str_id,16  );

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
   CHAR  err_buf[100];
   CHAR  msgtype[5] = "";
   CHAR  procode[7] = "";

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
         else
            ret_val = false;
      }
      else if( msgtype[2] == '1')
      {
         /* Response */
          if ( strcmp(procode,"014000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_RESPONSE;
          else if ( strcmp(procode,"004000") == 0)
            Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
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
      nccup_log_message( 1, 2, err_buf, "determine_tx_key",1 );
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
 *  AUTHOR:      Abhishek Verma
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


/*****************************************************************************

  Function:    vBCDMove 

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer[reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen = (movelen / 2);

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vBCDMove */

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

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
   	   if(fieldid == 55)
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
   BYTE                 temp_string_len[4] = {0};

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

	  	if(fieldid == 55) //EMV data
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



/*****************************************************************************

  Function:    

  Description: This function will move data from the raw ISO message to 
               the HOST_MESSAGE structure.  
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure for the outgoing ISO message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if ( (ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x02) )
      {
         /* Convert length from BCD to binary. */
         movelen  = ((ReqRsp_Buffer[reqidx] & 0xf0) >> 4) * 10;
         movelen +=   ReqRsp_Buffer[reqidx] & 0x0f;
      }
      else
         movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = movelen;
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      if (( reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */



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


/*****************************************************************************

  Function:    vDataMove

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
 void vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
  
     movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
	 memcpy(&ReqRsp_Buffer[reqidx],&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
     reqidx =reqidx + pBF->bd_fieldlen;

     memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[pBF->bd_fieldlen], movelen);

     reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vDataMove */



/*****************************************************************************

  Function:    RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to the HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
 void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
     
	 memcpy(&pBF->p_bd_pdata[0],&ReqRsp_Buffer[reqidx],pBF->bd_fieldlen);
	 reqidx =reqidx + pBF->bd_fieldlen;
	
  	 movelen = atoi(pBF->p_bd_pdata);
     memcpy(&pBF->p_bd_pdata[pBF->bd_fieldlen], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end RspMovevData */


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
 *  AUTHOR:      Abhishek Verma
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

   trantype = Auth_Tx.TLF01_details.tx_key;
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
   nccup_log_message( 2, 2, Buffer, "FIND_TRAN_TABLE",0 );

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
   nccup_log_message( 2, 2, temp_str, "Find_Bitfield_Table",0 );
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
 *  AUTHOR:       Abhishek Verma
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
   BYTE                 temp_len[5]="";
   BYTE                 temp_msg_type[5]="";
   BYTE                 temp_source_id[12]="";
   BYTE                 temp_destination_id[12]="";
   INT                  source_id_len = 0;
   INT                  destination_id_len = 0;
   CHAR                 msg_chg[2]="1";
   CHAR                 temp_sale_msgtype[5]="0100";
 
   /* Prepare Header of the message*/
    /*-----------------------------------------*/
   /* Copy in Message Id for this transaction */
   /*-----------------------------------------*/
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   
   /* Header length */
 
   ReqRsp_Buffer[4] = CUP_MSG_HEADER_SUCC_DATA;

   /* Header Flag and Version */
  // ReqRsp_Buffer[5] = MSG_HEADER_FLAG_PROD_VER;
    //ReqRsp_Buffer[5] = MSG_HEADER_FLAG_CAD;
	ReqRsp_Buffer[5] = CUP_MSG_HEADER_FLAG_FOR_CUP_HOST;
   /* Destination ID*/
   destination_id_len= strlen(Ncf01_I.receiving_inst_id_code);
   if(destination_id_len == 0)
   {
	   strncpy(temp_destination_id,"00010344   ",CUP_DEST_ID_LEN);
   }
   else if ( destination_id_len <11 )
   {
       	 strcat(temp_destination_id,Ncf01_I.receiving_inst_id_code);
		 for(destination_id_len; destination_id_len < CUP_DEST_ID_LEN ; destination_id_len ++)
		 {
			  strcat(temp_destination_id," ");
		 }
   }
   else
   {
	   strncpy(temp_destination_id,Ncf01_I.receiving_inst_id_code,CUP_DEST_ID_LEN);
   }
  // strncpy( &ReqRsp_Buffer[10],"00010344   ",DEST_ID_LEN);
   memcpy( &ReqRsp_Buffer[10],temp_destination_id,CUP_DEST_ID_LEN);
  
   /*Source ID */
   source_id_len= strlen(Ncf01_I.acquirer_inst_id_code);
   if(source_id_len == 0)
   {
	   strncpy(temp_source_id,"27240608   ",CUP_SOURCE_ID_LEN);
   }
   else if ( source_id_len <11 )
   {
       	 strcat(temp_source_id,Ncf01_I.acquirer_inst_id_code);
		 for(source_id_len; source_id_len < CUP_SOURCE_ID_LEN ; source_id_len ++)
		 {
			  strcat(temp_source_id," ");
		 }

   }
   else
   {
	   strncpy(temp_source_id,Ncf01_I.acquirer_inst_id_code,CUP_SOURCE_ID_LEN);
   }
   
  memcpy( &ReqRsp_Buffer[21],temp_source_id,CUP_SOURCE_ID_LEN);
  //memcpy( &ReqRsp_Buffer[21],"27240608   ",DEST_ID_LEN);
  
   /* Reserve for use*/
   ReqRsp_Buffer[32]=0x00;
   ReqRsp_Buffer[33]=0x00;
   ReqRsp_Buffer[34]=0x00;

   /*Batch Number*/
   ReqRsp_Buffer[35]= CUP_BATCH_NUM;

   /* Transaction Information */
   memcpy( &ReqRsp_Buffer[36],CUP_TRAN_INFO_DATA, CUP_TRAN_INFO_LEN);
  
   /* User Info  */
   ReqRsp_Buffer[44]= CUP_USER_INFO_DATA;
   
   /*Reject code*/
   memcpy( &ReqRsp_Buffer[45],CUP_REJECT_CODE_DATA,CUP_REJECT_CODE_LEN);
  
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

   /* point to bitmap     */
   bitmapidx = CUP_HDR_SIZE_SUCC + CUP_MESSAGE_TYPE_SIZE + 4;

   memcpy(&ReqRsp_Buffer[CUP_HDR_SIZE_SUCC+4], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   if((0 == strncmp(Auth_Tx.TLF01_details.processing_code,"01",2)) &&
		 (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4)))
	   {
		   /* We Need to change the Message type from 0200 to 0100 as we are using same key AUTH_CASH_ADVANCE for 
		   Message type 200 with Processing code 01 and  Message type 0100 with Processing code 01
		   Message type will start at 50th lindex so will make 51 index to 1 i.e 2 to 1*/
	  	 ReqRsp_Buffer[51]= msg_chg[0];			   
	   }
   if( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))&&
   	  (Auth_Tx.TLF01_details.tx_key == AUTH_SALE)||
   	  (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE))
   {
		// We need to change the message type to 100 as its coming from POS.
		//Over ride the message type.
		memcpy(&ReqRsp_Buffer[CUP_HDR_SIZE_SUCC+4], temp_sale_msgtype, CUP_MESSAGE_TYPE_SIZE);
    }

   memcpy(temp_msg_type,&ReqRsp_Buffer[CUP_HDR_SIZE_SUCC +4],CUP_MESSAGE_TYPE_SIZE);
    /* A single bit map or Double */
	 /*As of Now we are doing only for 800 and 0820 message) */
	 if ( (0 == strncmp(temp_msg_type,"0800",4)) || (0 == strncmp(temp_msg_type,"0820",4)))
	 {
		bitMapsize = 16;
		ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
		BitFieldSize = EXTENDED_BITFIELD;
	 }
	 else if (extendedbit == true)
	 {
	 	bitMapsize = 16;
		ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
		BitFieldSize = EXTENDED_BITFIELD;
	 }
	 else
	 {
		  bitMapsize = 8;
		  BitFieldSize = NORMAL_BITFIELD;
	 }
   
	 //bitMapsize = 16;
	 //BitFieldSize = EXTENDED_BITFIELD;
   
   reqidx = bitmapidx + bitMapsize;

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

   /*Prepare the total message length in header*/
   sprintf(temp_len,"%04d",(reqidx-4));
   memcpy(&ReqRsp_Buffer[6],temp_len,CUP_TOL_MEG_LEN);

   memcpy(&ReqRsp_Buffer[0],temp_len,CUP_TOL_MEG_LEN);
  
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
   extendedbit= false;

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
   memcpy(temp_message_type, Auth_Tx.TLF01_details.message_type,4);
   if ( TranFlow == SWITCHED_OUT )
   {
    /* SWITCHED OUT Transactions */
      if (temp_message_type[1] == '1') 
      {
         if(temp_message_type[2] == '0') 
         {
        	 if(0!=strncmp(Auth_Tx.TLF01_details.pos_condition_code,"73",2))
        	 {
            /* Build fields necessary for 0100 REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
            (void)build_request_field_18( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
            (void)build_request_field_22( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
			(void)build_request_field_26( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );	
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
            (void)build_request_field_43( &result_code, err_buf );
			(void)build_request_field_45( &result_code, err_buf ); 
			(void)build_request_field_48( &result_code, err_buf );
			(void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_52( &result_code, err_buf );
			(void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
            (void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_61( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_122( &result_code, err_buf );
			(void)build_request_field_125( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );
        	 }
        	 else
        	 {
            /* Build fields necessary for 0100 REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
            (void)build_request_field_18( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
            (void)build_request_field_22( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
			(void)build_request_field_26( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );	
        		(void)build_request_field_38( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
            (void)build_request_field_43( &result_code, err_buf );
			(void)build_request_field_45( &result_code, err_buf ); 
			(void)build_request_field_48( &result_code, err_buf );
			(void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_52( &result_code, err_buf );
			(void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
            (void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_61( &result_code, err_buf );
        		(void)build_request_field_90( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_122( &result_code, err_buf );
			(void)build_request_field_125( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );
          }
     }
     }

     else if(temp_message_type[1] == '2') 
     {
          if(temp_message_type[2] == '0');
		  {
        	/* Build fields necessary for  0200 REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );  
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
            (void)build_request_field_18( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_22( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_26( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
            (void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );
			(void)build_request_field_38( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
			(void)build_request_field_43( &result_code, err_buf );
            (void)build_request_field_45( &result_code, err_buf );
			(void)build_request_field_48( &result_code, err_buf ); 
            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_52( &result_code, err_buf ); 
			(void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_61( &result_code, err_buf );
			(void)build_request_field_90( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_122( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );
         }
     }
     else if(temp_message_type[1] == '2') 
     {
      	  if(temp_message_type[2] == '2')
      	  {
         /* Build fields necessary for 0220  REQUEST messages. */
		 (void)build_request_field_2(  &result_code, err_buf );
         (void)build_request_field_3(  &result_code, err_buf );
         (void)build_request_field_4(  &result_code, err_buf );
         (void)build_request_field_7(  &result_code, err_buf );
         (void)build_request_field_11( &result_code, err_buf );
         (void)build_request_field_12( &result_code, err_buf );
		 (void)build_request_field_13( &result_code, err_buf );  
		 (void)build_request_field_14( &result_code, err_buf );  
		 (void)build_request_field_18( &result_code, err_buf );  
         (void)build_request_field_19( &result_code, err_buf );
         (void)build_request_field_22( &result_code, err_buf );
		 (void)build_request_field_23( &result_code, err_buf );
         (void)build_request_field_25( &result_code, err_buf );
         (void)build_request_field_32( &result_code, err_buf );
         (void)build_request_field_33( &result_code, err_buf );
         (void)build_request_field_35( &result_code, err_buf );
         (void)build_request_field_37( &result_code, err_buf );
		 (void)build_request_field_38( &result_code, err_buf );  
         (void)build_request_field_41( &result_code, err_buf );
		 (void)build_request_field_42( &result_code, err_buf );  
		 (void)build_request_field_43( &result_code, err_buf );
		 (void)build_request_field_45( &result_code, err_buf );  
		 (void)build_request_field_48( &result_code, err_buf );  
         (void)build_request_field_49( &result_code, err_buf );
		 (void)build_request_field_55( &result_code, err_buf );
         (void)build_request_field_60( &result_code, err_buf );
		 (void)build_request_field_100( &result_code, err_buf );
         (void)build_request_field_122( &result_code, err_buf );
 		 (void)build_request_field_128( &result_code, err_buf );
	   }
     }
     else if ( is_reversal() )
     {
		/*  If Original txn process as STIP and later send into host.
			After sent host If ATP gets reversal txn for that.
			It should go as 420 msg - TF-Ajay 08-Jul-09 */
		if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED)==0) 
		{
				Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE; 
				strcpy(Auth_Tx.TLF01_details.message_type, "0420" ); 
			
		}
        if( (strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0) && 
		 ( 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) ))
		{
            /* Build fields necessary for 0420/0400  ATM reversal REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
			(void)build_request_field_3(  &result_code, err_buf ); // TF Phani
            (void)build_request_field_4(  &result_code, err_buf );
            (void)build_request_field_5(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
            (void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_37( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
			(void)build_request_field_42( &result_code, err_buf ); // TF Phani

            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
            

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
            /* Build fields necessary for 0420 POS reversal REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_5(  &result_code, err_buf );
			(void)build_request_field_6(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );
			(void)build_request_field_9(  &result_code, err_buf );
			(void)build_request_field_10( &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
			(void)build_request_field_13( &result_code, err_buf );
			(void)build_request_field_15( &result_code, err_buf );
			(void)build_request_field_16( &result_code, err_buf );
			(void)build_request_field_18( &result_code, err_buf );
			(void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_22( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_37( &result_code, err_buf );
			(void)build_request_field_38( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
			(void)build_request_field_42( &result_code, err_buf );// TF Phani
            (void)build_request_field_43( &result_code, err_buf );
            (void)build_request_field_44( &result_code, err_buf );
			(void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_50( &result_code, err_buf );
			(void)build_request_field_51( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_90( &result_code, err_buf );
			(void)build_request_field_100(&result_code, err_buf );
			(void)build_request_field_121(&result_code, err_buf );
			(void)build_request_field_122(&result_code, err_buf );
			(void)build_request_field_128(&result_code, err_buf );

            /* Need to make sure the RRN of a reversal is the
             * same as the RRN of the original transaction.
             * EB needs it logged into TLF01 same as original RRN.
             */
            memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
                    Auth_Tx.TLF01_details.orig_retrieval_ref_num,
                    12 );
         }
      }
	 else if(temp_message_type[1] == '8') 
     {
      	if(temp_message_type[2] == '2')
      	{
            /* Sign on message 0820*/
			(void)build_request_field_7( &result_code, err_buf );
			(void)build_request_field_11( &result_code, err_buf );
			(void)build_request_field_15( &result_code, err_buf );
			(void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
			(void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_70( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
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
   
   else if ( TranFlow == SWITCHED_IN )
   {
      /* SWITCHED IN Transactions */
      if (temp_message_type[1] == '8') 
      {
         if(temp_message_type[2] == '2') 
         {
            /* Message 0820*/
            (void)build_request_field_7( &result_code, err_buf );
			(void)build_request_field_11( &result_code, err_buf );
			(void)build_request_field_15( &result_code, err_buf );
			(void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
			(void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_70( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
         }
		 else if ( temp_message_type[2] == '0')
		 {
			/* Message 0800*/
            (void)build_request_field_7( &result_code, err_buf );
			(void)build_request_field_11( &result_code, err_buf );
			(void)build_request_field_39( &result_code, err_buf );
			(void)build_request_field_53( &result_code, err_buf );
			(void)build_request_field_70( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );
		 }
		 else if ( temp_message_type[2] == '3')
		 {
			/* Message 0830*/
            (void)build_request_field_7( &result_code, err_buf );
			(void)build_request_field_11( &result_code, err_buf );
			(void)build_request_field_15( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
			(void)build_request_field_39( &result_code, err_buf );
			(void)build_request_field_70( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
		 }
      }
	  
	  else if (temp_message_type[1] == '1') 
      {
         if(temp_message_type[2] == '0') 
         {
            /* Build fields necessary for 0100 messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
			(void)build_request_field_15( &result_code, err_buf );
            (void)build_request_field_18( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
			(void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );	
			(void)build_request_field_38( &result_code, err_buf );	
			(void)build_request_field_39( &result_code, err_buf );	
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
            (void)build_request_field_44( &result_code, err_buf );
			(void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_54( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_57( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_61( &result_code, err_buf );
            (void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_121( &result_code, err_buf );
			(void)build_request_field_123( &result_code, err_buf );
			(void)build_request_field_125( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );
          }
     }
	 else if(temp_message_type[1] == '2') 
     {
          if(temp_message_type[2] == '0');
		  {
        	/* Build fields necessary for  0200 messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );  
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_14( &result_code, err_buf );
            (void)build_request_field_15( &result_code, err_buf );
            (void)build_request_field_18( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_22( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
            (void)build_request_field_33( &result_code, err_buf );
			(void)build_request_field_35( &result_code, err_buf );
			(void)build_request_field_37( &result_code, err_buf );
			(void)build_request_field_38( &result_code, err_buf );
			(void)build_request_field_39( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
            (void)build_request_field_42( &result_code, err_buf );
			(void)build_request_field_44( &result_code, err_buf );
            (void)build_request_field_49( &result_code, err_buf );
            (void)build_request_field_52( &result_code, err_buf );
            (void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_57( &result_code, err_buf ); 
			(void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_61( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_121( &result_code, err_buf );
			(void)build_request_field_122( &result_code, err_buf );
			(void)build_request_field_123( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );
         }
     }
	 else if(temp_message_type[1] == '2') 
     {
      	  if(temp_message_type[2] == '2')
      	  {
         /* Build fields necessary for 0220  REQUEST messages. */
		 (void)build_request_field_2(  &result_code, err_buf );
         (void)build_request_field_3(  &result_code, err_buf );
         (void)build_request_field_4(  &result_code, err_buf );
         (void)build_request_field_5(  &result_code, err_buf );
         (void)build_request_field_6(  &result_code, err_buf );
         (void)build_request_field_7(  &result_code, err_buf );
		 (void)build_request_field_9(  &result_code, err_buf );
         (void)build_request_field_10(  &result_code, err_buf );
         (void)build_request_field_11( &result_code, err_buf );
         (void)build_request_field_12( &result_code, err_buf );
		 (void)build_request_field_13( &result_code, err_buf );  
		 (void)build_request_field_15( &result_code, err_buf );  
		 (void)build_request_field_18( &result_code, err_buf );  
         (void)build_request_field_19( &result_code, err_buf );
		 (void)build_request_field_23( &result_code, err_buf );
         (void)build_request_field_25( &result_code, err_buf );
         (void)build_request_field_32( &result_code, err_buf );
         (void)build_request_field_33( &result_code, err_buf );
		 (void)build_request_field_35( &result_code, err_buf );
         (void)build_request_field_37( &result_code, err_buf );
		 (void)build_request_field_38( &result_code, err_buf );  		 
		 (void)build_request_field_39( &result_code, err_buf );  
         (void)build_request_field_41( &result_code, err_buf );
		 (void)build_request_field_42( &result_code, err_buf );  
		 (void)build_request_field_49( &result_code, err_buf );
		 (void)build_request_field_50( &result_code, err_buf );  
		 (void)build_request_field_51( &result_code, err_buf );  
		 (void)build_request_field_55( &result_code, err_buf );
         (void)build_request_field_57( &result_code, err_buf );
         (void)build_request_field_60( &result_code, err_buf );
         (void)build_request_field_100( &result_code, err_buf );
 		 (void)build_request_field_121( &result_code, err_buf ); 
		 (void)build_request_field_122( &result_code, err_buf );
		 (void)build_request_field_128( &result_code, err_buf );
		 
	   }
     }
	 else if ( is_reversal() )
     {
		/*  If Original txn process as STIP and later send into host.
			After sent host If ATP gets reversal txn for that.
			It should go as 420 msg - TF-Ajay 08-Jul-09 */
		if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED)==0) 
		{
				Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE; 
				strcpy(Auth_Tx.TLF01_details.message_type, "0420" ); 
			
		}
        if( (strncmp(Auth_Tx.TLF01_details.dispensation.origin, "RO",2) == 0) && 
		 ( 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) ))
		{
            /* Build fields necessary for 0420/0400  ATM reversal REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
			(void)build_request_field_3(  &result_code, err_buf ); // TF Phani
            (void)build_request_field_4(  &result_code, err_buf );
            (void)build_request_field_5(  &result_code, err_buf );
            (void)build_request_field_6(  &result_code, err_buf );
            (void)build_request_field_7(  &result_code, err_buf );
            (void)build_request_field_9(  &result_code, err_buf );
            (void)build_request_field_10(  &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
            (void)build_request_field_13( &result_code, err_buf );
            (void)build_request_field_15( &result_code, err_buf );
            (void)build_request_field_16( &result_code, err_buf );
            (void)build_request_field_18( &result_code, err_buf );
            (void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
            (void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_37( &result_code, err_buf );
			(void)build_request_field_39( &result_code, err_buf );		
            (void)build_request_field_41( &result_code, err_buf );
			(void)build_request_field_42( &result_code, err_buf ); // TF Phani

            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );
            (void)build_request_field_100( &result_code, err_buf );
            (void)build_request_field_121( &result_code, err_buf );
            (void)build_request_field_122( &result_code, err_buf );
			(void)build_request_field_123( &result_code, err_buf );
			(void)build_request_field_128( &result_code, err_buf );

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
            /* Build fields necessary for 0420 POS reversal REQUEST messages. */
            (void)build_request_field_2(  &result_code, err_buf );
            (void)build_request_field_3(  &result_code, err_buf );
            (void)build_request_field_4(  &result_code, err_buf );
			(void)build_request_field_5(  &result_code, err_buf );
			(void)build_request_field_7(  &result_code, err_buf );
			(void)build_request_field_9(  &result_code, err_buf );
			(void)build_request_field_10( &result_code, err_buf );
            (void)build_request_field_11( &result_code, err_buf );
            (void)build_request_field_12( &result_code, err_buf );
			(void)build_request_field_13( &result_code, err_buf );
			(void)build_request_field_15( &result_code, err_buf );
			(void)build_request_field_16( &result_code, err_buf );
			(void)build_request_field_18( &result_code, err_buf );
			(void)build_request_field_19( &result_code, err_buf );
			(void)build_request_field_23( &result_code, err_buf );
            (void)build_request_field_25( &result_code, err_buf );
            (void)build_request_field_32( &result_code, err_buf );
			(void)build_request_field_33( &result_code, err_buf );
            (void)build_request_field_37( &result_code, err_buf );
			(void)build_request_field_38( &result_code, err_buf );			
			(void)build_request_field_39( &result_code, err_buf );
            (void)build_request_field_41( &result_code, err_buf );
			(void)build_request_field_42( &result_code, err_buf );// TF Phani
            (void)build_request_field_49( &result_code, err_buf );
			(void)build_request_field_50( &result_code, err_buf );
			(void)build_request_field_55( &result_code, err_buf );
			(void)build_request_field_60( &result_code, err_buf );
			(void)build_request_field_100( &result_code, err_buf );
			(void)build_request_field_121(&result_code, err_buf );
			(void)build_request_field_122(&result_code, err_buf );
			(void)build_request_field_123( &result_code, err_buf );
			(void)build_request_field_128(&result_code, err_buf );

            /* Need to make sure the RRN of a reversal is the
             * same as the RRN of the original transaction.
             * EB needs it logged into TLF01 same as original RRN.
             */
            memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
                    Auth_Tx.TLF01_details.orig_retrieval_ref_num,
                    12 );
         }
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
   INT                   i= 0;
   BYTE                  ErrorMsg[100]="";

   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   // Header len is 46 for success header
   if (0x2E == ReqRsp_Buffer[4])
   {
      //Txn_Is_A_Reject = false;
      //hdr_size = CUP_HDR_SIZE_SUCC;

	  /* Header length */
      memcpy( HOST_MESSAGE.hdr_len,&ReqRsp_Buffer[4],
    		  CUP_MSG_HEADER_LEN );

	  /* Header Flag and Version */
	  memcpy( HOST_MESSAGE.hdr_flag_ver, &ReqRsp_Buffer[5],
			  CUP_MSG_HEADER_FLAG_VER_LEN);
	  
	  /* Total messsage length */
	  memcpy( HOST_MESSAGE.tot_msg_len,&ReqRsp_Buffer[6], 
			  CUP_TOL_MEG_LEN);
	  
	  /* Destination ID*/
	  memcpy( HOST_MESSAGE.dest_stat_id, &ReqRsp_Buffer[10],
			  CUP_DEST_ID_LEN);

	  /*Source ID */
	  memcpy( HOST_MESSAGE.source_id, &ReqRsp_Buffer[21],
			  CUP_SOURCE_ID_LEN);
	  
	  /* Reserve for use*/
	  memcpy( HOST_MESSAGE.res_for_use,&ReqRsp_Buffer[32],
			  CUP_RESV_FOR_USE_LEN);

	  /*Batch Number*/
	  memcpy( HOST_MESSAGE.batch_no,&ReqRsp_Buffer[35],
			  CUP_BATCH_NO_LEN);

	  /* Transaction Information */
	  memcpy( HOST_MESSAGE.tran_info, &ReqRsp_Buffer[36],
			  CUP_TRAN_INFO_LEN);
	  
	  /* User Info  */
	  memcpy( HOST_MESSAGE.user_info,&ReqRsp_Buffer[44],
			  CUP_USER_INFO_LEN);
	  /*Reject code*/
	  memcpy( HOST_MESSAGE.rej_code, &ReqRsp_Buffer[45],
			  CUP_REJECT_CODE_LEN);
	  Txn_Is_A_Reject = false;
	  if(0 == strncmp(HOST_MESSAGE.rej_code,"00000",CUP_REJECT_CODE_LEN))
	  {
	  	  reqidx =  CUP_HDR_SIZE_SUCC + 4;       /* point to message type */
	  }
	  else
	  {
	  	// Error rejected code is their
	  	reqidx =  CUP_HDR_SIZE_SUCC + CUP_HDR_SIZE_SUCC + 4;
		/*Find message Type*/
	    memcpy(HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], 4);
       	strcpy( err_buf, "Received header with message type:  " );
      	strcat( err_buf, HOST_MESSAGE.msgtype );
	  	strcat( err_buf, " reject code: " );
	  	strncat( err_buf, &ReqRsp_Buffer[45],CUP_REJECT_CODE_LEN);
		strcat( err_buf, " parse_host_msg ");
	  	return false;
	  }

	  /*Find message Type*/
	  memcpy(HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], 4);
      reqidx = reqidx +4;

  
   	}
    else if (0 == strncmp( ReqRsp_Buffer, "0000", 4)|| 
			         (strlen(ReqRsp_Buffer)<46))
	{
		return false;
	}
    else
   	{
	   strcpy( ErrorMsg, "Unknown header type :" );
	   nccup_log_message( 1, 3, ErrorMsg, "parse_host_msg",0 );
	   Txn_Is_A_Reject = true;
   	}


   bitmap = ReqRsp_Buffer[reqidx];
   bitmapidx = reqidx;

   /* Point past bitmap, but 1st check for extended bitmap. */
   
   if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = reqidx + 16;                         /* Extended     */
	 
	  /*ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 32 );*/
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
     reqidx = reqidx + 8;                         /* Not Extended */
	 
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
               nccup_log_message( 1, 2, err_buf, "parse_host_msg",0 );
               break;
            }
            else
            {
               sprintf( err_buf,
                    "Unable to parse field %d in msg from host, MsgType: %4.4s",
                     bfidx, HOST_MESSAGE.msgtype );
               nccup_log_message( 1, 2, err_buf, "parse_host_msg",0 );
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
      if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         if (Auth_Tx.TLF01_details.message_type[2] == '0')
		 {
		   /* Move fields of 0800 Key reset REQUEST messages. */
			 (void)move_field_7(   &result_code, err_buf );
			 (void)move_field_11(  &result_code, err_buf );
			 (void)move_field_48(  &result_code, err_buf );
			 (void)move_field_53(  &result_code, err_buf );
			 (void)move_field_70(  &result_code, err_buf );
			 (void)move_field_96(  &result_code, err_buf );
			 (void)move_field_100( &result_code, err_buf );
			 (void)move_field_128( &result_code, err_buf );
		 }
      	 else if(Auth_Tx.TLF01_details.message_type[2] == '2')
      	 {
         	/* Move fields of 0820 network REQUEST messages. */
	         (void)move_field_7(   &result_code, err_buf );
    	     (void)move_field_11(  &result_code, err_buf );
     	     (void)move_field_15(  &result_code, err_buf );
     	     (void)move_field_32(  &result_code, err_buf );
        	 (void)move_field_33(  &result_code, err_buf );
          	 (void)move_field_53(  &result_code, err_buf );
	         (void)move_field_70(  &result_code, err_buf );
    	     (void)move_field_100( &result_code, err_buf );
      	 }
		 strncpy(NMI_Code,Auth_Tx.function_code,3);
      }
	 
	  else if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))
      {
  	 	/* Move fields necessary for 0100 request messages. */
  		(void)move_field_2(  &result_code, err_buf );
        (void)move_field_3(  &result_code, err_buf );
        (void)move_field_4(  &result_code, err_buf );
        (void)move_field_6(  &result_code, err_buf );
        (void)move_field_7(  &result_code, err_buf );
        (void)move_field_10(  &result_code, err_buf );
        (void)move_field_11( &result_code, err_buf );
        (void)move_field_12( &result_code, err_buf );
        (void)move_field_13( &result_code, err_buf );
        (void)move_field_14( &result_code, err_buf );
        (void)move_field_15(  &result_code, err_buf );
		(void)move_field_18( &result_code, err_buf );
        (void)move_field_19( &result_code, err_buf );
		(void)move_field_22( &result_code, err_buf );
		(void)move_field_23( &result_code, err_buf );
		(void)move_field_25( &result_code, err_buf );
		(void)move_field_26( &result_code, err_buf );
        (void)move_field_32( &result_code, err_buf );
        (void)move_field_33( &result_code, err_buf );			
		(void)move_field_35( &result_code, err_buf );
        (void)move_field_37( &result_code, err_buf );
        (void)move_field_38( &result_code, err_buf );
        (void)move_field_41( &result_code, err_buf );
        (void)move_field_42( &result_code, err_buf );
		(void)move_field_43( &result_code, err_buf );
		(void)move_field_45( &result_code, err_buf );
        (void)move_field_48( &result_code, err_buf );
		(void)move_field_49( &result_code, err_buf );
        (void)move_field_51(  &result_code, err_buf );
		(void)move_field_52( &result_code, err_buf );
		(void)move_field_53( &result_code, err_buf );
		(void)move_field_55( &result_code, err_buf );
		(void)move_field_60( &result_code, err_buf );
		(void)move_field_61( &result_code, err_buf );
		(void)move_field_90( &result_code, err_buf );
		(void)move_field_100( &result_code, err_buf );
		(void)move_field_104( &result_code, err_buf );
		(void)move_field_117(&result_code, err_buf );
		(void)move_field_122( &result_code, err_buf );
		(void)move_field_125( &result_code, err_buf );
		(void)move_field_128( &result_code, err_buf );
	  }
	  else if (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0200",4))
	  {
		/* Move fields necessary for 0200 Request messages. */
		(void)move_field_2( &result_code, err_buf );
        (void)move_field_3( &result_code, err_buf );
        (void)move_field_4( &result_code, err_buf );
		(void)move_field_5( &result_code, err_buf );
        (void)move_field_6( &result_code, err_buf );
        (void)move_field_7( &result_code, err_buf );
		(void)move_field_9( &result_code, err_buf );
        (void)move_field_10( &result_code, err_buf );
        (void)move_field_11( &result_code, err_buf );
        (void)move_field_12( &result_code, err_buf );
        (void)move_field_13( &result_code, err_buf );
        (void)move_field_14( &result_code, err_buf );
        (void)move_field_15( &result_code, err_buf );
        (void)move_field_16( &result_code, err_buf );
		(void)move_field_18( &result_code, err_buf );
        (void)move_field_19( &result_code, err_buf );
        (void)move_field_22( &result_code, err_buf );
		(void)move_field_23( &result_code, err_buf );
		(void)move_field_25( &result_code, err_buf );
		(void)move_field_26( &result_code, err_buf );
		(void)move_field_28( &result_code, err_buf );
		(void)move_field_32( &result_code, err_buf );
        (void)move_field_33( &result_code, err_buf );
        (void)move_field_35( &result_code, err_buf );
        (void)move_field_37( &result_code, err_buf );
        (void)move_field_41( &result_code, err_buf );
        (void)move_field_42( &result_code, err_buf );
        (void)move_field_43( &result_code, err_buf );
        (void)move_field_45( &result_code, err_buf );
        (void)move_field_48( &result_code, err_buf );
		(void)move_field_49( &result_code, err_buf );
		(void)move_field_50( &result_code, err_buf );
		(void)move_field_51( &result_code, err_buf );
        (void)move_field_52( &result_code, err_buf );
		(void)move_field_53( &result_code, err_buf );
		(void)move_field_55( &result_code, err_buf );
		(void)move_field_60( &result_code, err_buf );
        (void)move_field_61( &result_code, err_buf );
		(void)move_field_100( &result_code, err_buf );
		(void)move_field_104( &result_code, err_buf );
		(void)move_field_117(&result_code, err_buf );
        (void)move_field_122( &result_code, err_buf );
        (void)move_field_128( &result_code, err_buf );
	  }

	  else if (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0220",4))
	  {
		   /* Move fields necessary for 0220 request messages. */
		   (void)move_field_2(	&result_code, err_buf );
		   (void)move_field_3(	&result_code, err_buf );
		   (void)move_field_4(	&result_code, err_buf );
		   (void)move_field_7(	&result_code, err_buf );
		   (void)move_field_11( &result_code, err_buf );
		   (void)move_field_12( &result_code, err_buf );
		   (void)move_field_13( &result_code, err_buf );
		   (void)move_field_14( &result_code, err_buf );
		   (void)move_field_18( &result_code, err_buf );
		   (void)move_field_19( &result_code, err_buf );
		   (void)move_field_22( &result_code, err_buf );
		   (void)move_field_25( &result_code, err_buf );
		   (void)move_field_32( &result_code, err_buf );
		   (void)move_field_33( &result_code, err_buf );		   
		   (void)move_field_35( &result_code, err_buf );
		   (void)move_field_37( &result_code, err_buf );
		   (void)move_field_38( &result_code, err_buf );
		   (void)move_field_41( &result_code, err_buf );
		   (void)move_field_42( &result_code, err_buf );
		   (void)move_field_43( &result_code, err_buf );
		   (void)move_field_45( &result_code, err_buf );
		   (void)move_field_48( &result_code, err_buf );
		   (void)move_field_49( &result_code, err_buf );
  		   (void)move_field_55( &result_code, err_buf );
		   (void)move_field_60( &result_code, err_buf );
		   (void)move_field_104( &result_code, err_buf );
		   (void)move_field_117(&result_code, err_buf );
		   (void)move_field_122( &result_code, err_buf );
		   (void)move_field_128( &result_code, err_buf );
	  }
	  else if ((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0420",4))||
	  	         (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0421",4))||
	  	         (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0410",4))||
	  	         (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0411",4)))
	  {
		 /* Move fields necessary for 0420 . */
		  (void)move_field_2(  &result_code, err_buf );
		  (void)move_field_3(  &result_code, err_buf );
		  (void)move_field_4(  &result_code, err_buf );
		  (void)move_field_5(  &result_code, err_buf );
		  (void)move_field_6(  &result_code, err_buf );
		  (void)move_field_7(  &result_code, err_buf );
		  (void)move_field_9(  &result_code, err_buf );
		  (void)move_field_10(	&result_code, err_buf );
		  (void)move_field_11( &result_code, err_buf );
		  (void)move_field_12( &result_code, err_buf );
		  (void)move_field_13( &result_code, err_buf );
		  (void)move_field_15( &result_code, err_buf );
		  (void)move_field_16( &result_code, err_buf );
		  (void)move_field_18(	&result_code, err_buf );
		  (void)move_field_19(	&result_code, err_buf );
		  (void)move_field_22(	&result_code, err_buf );
  		  (void)move_field_23( &result_code, err_buf );
		  (void)move_field_25(	&result_code, err_buf );
		  (void)move_field_32( &result_code, err_buf );
		  (void)move_field_33( &result_code, err_buf );
		  (void)move_field_37( &result_code, err_buf );
		  (void)move_field_38( &result_code, err_buf );
		  (void)move_field_39( &result_code, err_buf );
		  (void)move_field_41( &result_code, err_buf );
		  (void)move_field_42( &result_code, err_buf );
		  (void)move_field_43(	&result_code, err_buf );
		  (void)move_field_44(	&result_code, err_buf );
		  (void)move_field_49( &result_code, err_buf );
		  (void)move_field_50( &result_code, err_buf );
		  (void)move_field_51(	&result_code, err_buf );
  		  (void)move_field_55( &result_code, err_buf );
		  (void)move_field_60(	&result_code, err_buf );
		  (void)move_field_61(	&result_code, err_buf );
		  (void)move_field_90(	&result_code, err_buf );
		  (void)move_field_100(  &result_code, err_buf );
		  (void)move_field_104( &result_code, err_buf );
		  (void)move_field_117(&result_code, err_buf );
		  (void)move_field_121( &result_code, err_buf );
		  (void)move_field_122( &result_code, err_buf );
		  (void)move_field_125( &result_code, err_buf );
		  (void)move_field_128( &result_code, err_buf );
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
            /* Move fields necessary for 0110 RESPONSE messages. */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_7(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_13( &result_code, err_buf );
            (void)move_field_14( &result_code, err_buf );
			(void)move_field_15( &result_code, err_buf );
			(void)move_field_18( &result_code, err_buf );
            (void)move_field_19( &result_code, err_buf );
			(void)move_field_25( &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_33( &result_code, err_buf );			
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_38( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
			(void)move_field_44( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
			(void)move_field_51( &result_code, err_buf );
			(void)move_field_54( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
			(void)move_field_57( &result_code, err_buf );
			(void)move_field_60( &result_code, err_buf );
			(void)move_field_61( &result_code, err_buf );
			(void)move_field_100( &result_code, err_buf );
			(void)move_field_121( &result_code, err_buf );
			(void)move_field_123( &result_code, err_buf );
			(void)move_field_128( &result_code, err_buf );
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
	  	if(Auth_Tx.TLF01_details.message_type[2] == '1')
	  	{
        /* Move fields necessary for 0210 RESPONSE messages. */
            (void)move_field_2( &result_code, err_buf );
            (void)move_field_3( &result_code, err_buf );
            (void)move_field_4( &result_code, err_buf );
            (void)move_field_7( &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_13( &result_code, err_buf );
            (void)move_field_14( &result_code, err_buf );
            (void)move_field_15( &result_code, err_buf );
			(void)move_field_18( &result_code, err_buf );
            (void)move_field_19( &result_code, err_buf );
			(void)move_field_25( &result_code, err_buf );
			(void)move_field_32( &result_code, err_buf );
            (void)move_field_33( &result_code, err_buf );
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_38( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_44( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
            (void)move_field_54( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
			(void)move_field_57( &result_code, err_buf );
			(void)move_field_60( &result_code, err_buf );
            (void)move_field_61( &result_code, err_buf );
            (void)move_field_100( &result_code, err_buf );
            (void)move_field_121( &result_code, err_buf );
            (void)move_field_123( &result_code, err_buf );
            (void)move_field_128( &result_code, err_buf );
         }
		 else if (Auth_Tx.TLF01_details.message_type[2] == '3')
		 {
			/* Move fields necessary for 0230 RESPONSE messages. */
			(void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
			(void)move_field_5(  &result_code, err_buf );
			(void)move_field_6(  &result_code, err_buf );
            (void)move_field_7(  &result_code, err_buf );
			(void)move_field_9(  &result_code, err_buf );
			(void)move_field_10(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_13( &result_code, err_buf );
			(void)move_field_15( &result_code, err_buf );
			(void)move_field_16(  &result_code, err_buf );
			(void)move_field_18( &result_code, err_buf );
            (void)move_field_19( &result_code, err_buf );
			(void)move_field_25( &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_33( &result_code, err_buf );			
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_38( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
			(void)move_field_50( &result_code, err_buf );
			(void)move_field_51(  &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
			(void)move_field_57( &result_code, err_buf );
			(void)move_field_60( &result_code, err_buf );
			(void)move_field_100( &result_code, err_buf );
			(void)move_field_121( &result_code, err_buf );
			(void)move_field_122( &result_code, err_buf );
			(void)move_field_128( &result_code, err_buf );
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
      else if ( is_reversal() )
      {
             /* Move fields necessary for 0430 POS reversal RESPONSE messages. */
            (void)move_field_2(  &result_code, err_buf );
            (void)move_field_3(  &result_code, err_buf );
            (void)move_field_4(  &result_code, err_buf );
            (void)move_field_5(  &result_code, err_buf );
            (void)move_field_7(  &result_code, err_buf );
            (void)move_field_9(  &result_code, err_buf );
            (void)move_field_11( &result_code, err_buf );
            (void)move_field_12( &result_code, err_buf );
            (void)move_field_13( &result_code, err_buf );
            (void)move_field_15( &result_code, err_buf );
			(void)move_field_16( &result_code, err_buf );
			(void)move_field_18(  &result_code, err_buf );
            (void)move_field_19(  &result_code, err_buf );
            (void)move_field_25(  &result_code, err_buf );
            (void)move_field_32( &result_code, err_buf );
            (void)move_field_33( &result_code, err_buf );
            (void)move_field_37( &result_code, err_buf );
            (void)move_field_39( &result_code, err_buf );
            (void)move_field_41( &result_code, err_buf );
            (void)move_field_42( &result_code, err_buf );
            (void)move_field_49( &result_code, err_buf );
			(void)move_field_50( &result_code, err_buf );
			(void)move_field_55( &result_code, err_buf );
			(void)move_field_60(  &result_code, err_buf );
            (void)move_field_100(  &result_code, err_buf );
            (void)move_field_121(  &result_code, err_buf );
            (void)move_field_122( &result_code, err_buf );
            (void)move_field_123( &result_code, err_buf );
            (void)move_field_128( &result_code, err_buf );
		
		
      }
      else if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
      {
         if (Auth_Tx.TLF01_details.message_type[2] == '1')
		 {
		   /* Move fields of 0810 Key reset REQUEST messages. */
			 (void)move_field_7(   &result_code, err_buf );
			 (void)move_field_11(  &result_code, err_buf );
			 (void)move_field_39(  &result_code, err_buf );
			 (void)move_field_53(  &result_code, err_buf );
			 (void)move_field_70(  &result_code, err_buf );
			 (void)move_field_100( &result_code, err_buf );
			 (void)move_field_128( &result_code, err_buf );
		 }
		 
         else if(Auth_Tx.TLF01_details.message_type[2] == '3')
      	 {
         /* Move fields necessary for 082n network RESPONSE messages. */
	         (void)move_field_7(   &result_code, err_buf );
    	     (void)move_field_11(  &result_code, err_buf );
			 (void)move_field_15(  &result_code, err_buf );
     	     (void)move_field_32(  &result_code, err_buf );
         	 (void)move_field_33(  &result_code, err_buf );
        	 (void)move_field_39(  &result_code, err_buf );
         	 (void)move_field_53(  &result_code, err_buf );
	         (void)move_field_70(  &result_code, err_buf );
    	     (void)move_field_100(  &result_code, err_buf );
      	 }

         strncpy( NMI_Code, Auth_Tx.function_code, 3 );
      }
      else if(Auth_Tx.TLF01_details.message_type[1] == '3') 
      {
         /* Move fields necessary for 13nn response*/
         (void)move_field_2(  &result_code, err_buf );
         (void)move_field_11( &result_code, err_buf );
         (void)move_field_12( &result_code, err_buf );
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
      for(track2_idx=0;track2_idx <= CUP_MAX_DE2_SIZE;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( (track2_idx > CUP_MAX_DE2_SIZE) || (*field_separator == 0x00) )
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
      nccup_log_message( 1, 2, err_buf, "build_response_message",1 );

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
		  if(0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) )
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void create_stan( pBYTE stan )
{
   INT   len;
	BYTE  time_in_seconds[30]={0};
	BYTE  stan_usec [7]= {0};

	struct timeval current_time;

	// Get the number of seconds and microseconds since the Epoch
	gettimeofday(&current_time, NULL);

	sprintf(stan_usec,"%06jd",(intmax_t)current_time.tv_usec);

	memcpy( stan, stan_usec, 6 );

	/*ptetime_get_strtime( time_in_seconds );
   len = strlen( time_in_seconds );
   if ( len < 10 )
      len = 10;

   /* Time is in format nnnnnnnnn.nnn
    * I want the last 6 n's before the decimal point.
    */
    /*memcpy( stan, &time_in_seconds[len-10], 6 );*/
   return;
}

/******************************************************************************
 *
 *  NAME:         nccup_log_message
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void nccup_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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

   /* Make sure text message is not too long. */
   memcpy( text_message, msg, sizeof(text_message)-1 );
   nccup_Log_error_warning_to_File(text_message,sev,func,details);
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
 *  AUTHOR:       Abhishek Verma
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

   ////////////////////////////////
   //We need to remove this 
   //current_state[0]='0';
   ////////////////////////////////

   /* Get Monitor connection information from shared memory. */
   memset (ConnectionId, 0, sizeof (ConnectionId)) ;
   ReturnCode = ReadMemMonitorTable (ConnectionId) ;
   if (ConnectionId[0] != 0)
   {
      if ( NetworkType == CUP_ACQUIRER  ||  NetworkType == CUP_BOTH )
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
            nccup_log_message( 2, 2, err_msg, "send_network_status_to_monitor",0 );
         }
      }
      if ( ((NetworkType == CUP_ISSUER) || (NetworkType == CUP_BOTH)) && ret_code )
      {
         if ( NetworkType == CUP_BOTH )
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
            nccup_log_message( 2, 2, err_msg, "send_network_status_to_monitor",0 );
      }
   }
   else
   {
      nccup_log_message( 2, 2, "Unable to get connection info for Monitor.",
                  "send_network_status_to_monitor" ,0);
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
   CHAR message[100]="";
   CHAR  host_tpdu_id [20]="";
   CHAR gmt_time[20]="";
   
  /* Current Greenwich Mean Time for the transmission-to-host time. */
  if ( 0 == strcmp( NULL_STR,
				   Auth_Tx.TLF01_details.transmission_timestamp ) )
  {
		pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
	   strncpy( Auth_Tx.TLF01_details.transmission_timestamp,
			 &gmt_time[4], 10 );  /* MMDDhhmmss */
   }

   if (strlen(Ncf01_I.tpdu_id) > 0)
   {
		strncpy(host_tpdu_id,Ncf01_I.tpdu_id, strlen(Ncf01_I.tpdu_id));
   }
   else if( 0 == strncmp(AppName,"nccup3",6))
   {
		strncpy(host_tpdu_id,"CUP3",4);
   }
   else 
   {
		strncpy(host_tpdu_id,"CUP",3);
   }

   ret_val = build_and_send_request_message( MT_OUTGOING,
                                             ST1_NONE,
                                             ST2_NONE,
                                             dialog_que_name,
                                             ReqRsp_Buffer,
                                             buflen,
                                             0,
                                             host_tpdu_id,
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
            nccup_log_message(2, 2, err_buf, "send_message_to_host",0 );
      }

      /* Cycle the echo timer because traffic went out over the line. */
      if ( false == reset_echo_timer(err_buf) )
         nccup_log_message(2, 2, err_buf, "send_message_to_host",0 );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         SEND_MESSAGE_TO_HOST1
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
INT send_message_to_host1( pCHAR err_buf,INT mgt ) //praneeth added bool mgt as parameter
{
   INT   ret_val;
   CHAR  buffer[100]="";
   BYTE tpdu_id[16]="";
   INT  moniter_request_flag = 0;

   /* Check if its a request  or response message*/

   if((Auth_Tx.TLF01_details.tx_key >= AUTH_SALE) &&
      (Auth_Tx.TLF01_details.tx_key <= AUTH_INVALID))
   {
  		/*Its Request message*/

  		/*Check is the transction is coming from dcpiso.*/
    	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",7))
   		{
   			if (tpdu_id_flag== false)
   			{
				strcpy(tpdu_id,Ncf01_I.tpdu_id);
				tpdu_id_flag=true;
   			}
   			else
   			{
				strcpy(tpdu_id,Ncf01_I.tpdu_id);
				strcat(tpdu_id,"1");
				tpdu_id_flag=true;
   			}
   		}

		/*	- When logon , logoff etc request come form Moniter nccup will be handler queue*/

		if ( Auth_Tx.TLF01_details.message_type[1] == '8' )
		{
			/*Check if it coming from monitor*/
			if( 0 != strncmp(Auth_Tx.TLF01_details.originator_queue,"hstcup",5))
			{
				/* This request is coming from moniter*/
				/* We have to send newtoek message two both the links hstcup and hstcup1*/
				moniter_request_flag = 1;
				strcpy(tpdu_id,Ncf01_I.tpdu_id);
			}
			
		}

  	}
   
    else if((Auth_Tx.TLF01_details.tx_key >= (AUTH_SALE + 100)) &&
     	    (Auth_Tx.TLF01_details.tx_key <= (AUTH_INVALID + 100)))
    {
    	/*Its a respone message*/
		/*Check who is the originator. forward respone to them*/
  		if(0 == strncmp(Auth_Tx.TLF01_details.originator_queue, "hstcup1A",8))
		{
			strcpy(tpdu_id,Ncf01_I.tpdu_id);
			strcat(tpdu_id,"1");
		}
		else 
		{
			strcpy(tpdu_id,Ncf01_I.tpdu_id);
		}
		
    }

	if( 0 == strlen(tpdu_id))
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.originator_queue, "hstcup1A",8))
		{
			strcpy(tpdu_id,Ncf01_I.tpdu_id);
			strcat(tpdu_id,"1");
		}
		else 
		{
			strcpy(tpdu_id,Ncf01_I.tpdu_id);
		}
	}

   ret_val = build_and_send_request_message( MT_OUTGOING,
                                             ST1_NONE,
                                             ST2_NONE,
                                             dialog_que_name,
                                             ReqRsp_Buffer,
                                             buflen,
                                             0,
                                             tpdu_id,
                                             buffer );

   if ((moniter_request_flag == 1) && (ret_val != false))
   {

    	sprintf( err_buf,
             	"Message %s is send to host:%s.",
               	 Auth_Tx.TLF01_details.message_type, tpdu_id );
	   nccup_log_message(1, 1, err_buf, "send_message_to_host",1 );
	   strcat(tpdu_id,"1");
	   ret_val = build_and_send_request_message( MT_OUTGOING,
                                             	 ST1_NONE,
                                             	 ST2_NONE,
                                            	 dialog_que_name,
                                            	 ReqRsp_Buffer,
                                             	 buflen,
                                             	 0,
                                            	 tpdu_id,
                                             	 buffer );
	   
   }
   if ( ret_val == false )
   {
      sprintf( err_buf,
              "Unable to send %s msg to host: %s. Err: %s",
               Auth_Tx.TLF01_details.message_type,tpdu_id, buffer );
   }
   else
   {
	   if(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0820",4))
	   {
   			sprintf( err_buf,
            		"Message %s is send to host:%s.",
             		Auth_Tx.TLF01_details.message_type, tpdu_id );
	  	 	nccup_log_message(1, 1, err_buf, "send_message_to_host",1 );
   	   }
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
            nccup_log_message(1, 2, err_buf, "send_message_to_host",1 );
      }

      /* Cycle the echo timer because traffic went out over the line. */
      if ( false == reset_echo_timer(err_buf) )
         nccup_log_message(2, 2, err_buf, "send_message_to_host",1 );
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
 *  AUTHOR:       Abhishek Verma
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

            nccup_log_message( 2, 2, err_buf, "verify_host_status",1 );

            ret_val = false;
            if ( false == set_host_state(OFFLINE,err_buf) )
               nccup_log_message( 2, 3, err_buf, "verify_host_status",1 );
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
            nccup_log_message( 1, 2, err_buf, "verify_host_status",1 );
         }
      }
   }
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
   CHAR       date_str [ 9] = {0};
   CHAR       time_str [ 7] = {0};
   CHAR       time_date[25] = {0};

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

   if ( 0 == strcmp(rsp_code, CUP_ISO_APPROVED) )
   {
	  if(0 != strncmp(Auth_Tx.TLF01_details.message_type,"0220",4))
	  {
	   strcpy(Auth_Tx.TLF01_details.response_text, "APPROVED_STAND_IN");
	  }
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
 *  AUTHOR:       Abhishk Verma
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
   return( ret_val );
}

/*****************************************************************************

  Function:   ascii_to_bin_with_length

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
void ascii_to_bin_with_length( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
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
 *  AUTHOR:       Abhishek Verma
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
 *  AUTHOR:       Abhishek Verma
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
         nccup_log_message( 2, 1, err_buf, "process_host_msg",0 );
         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         nccup_log_message( 2, 1, err_buf, "process_host_msg",0 );

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            nccup_log_message( 2, 1, err_buf, "process_host_msg",0 );
         }
      }
      else
      {
    	  flush_file(ppTraceFile_fp);
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         RJ_WITH_LEAD_ZEROS
 *
 *  DESCRIPTION:  This procedure right justifies a string, padding leading
 *                zeros.
 *
 *  INPUTS:       str      - Pointer to string to be right justified
 *                FieldLen - Length of str
 *
 *  OUTPUTS:      str - Contents of str is modified.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldLen )
{
   int i;
   int dif;
   int strlength;

   /* Convert any leading blanks to zero. */
   for( i=0; i< FieldLen; i++ )
   {
      if ( 0x20 == str[i] )
         memset( &str[i], 0x30, 1 );
      else
         break;
   }

   /* Remove any trailing blanks. */
   for( i=FieldLen-1; i>=0; i-- )
   {
      if ( 0x20 == str[i] )
         memset( &str[i], 0x00, 1 );
      else
         break;
   }

   /* Right justify, then prepend leading zeros. */
   strlength = strlen( str );
   dif       = FieldLen - strlength;

   if ( strlength < FieldLen )
   {
      for( i=FieldLen-1; i>=0; i-- )
      {
         if ( i >= dif )
            strncpy( &str[i], &str[i-dif], 1 );  /* Shift string to the right */
         else
            memset( &str[i], 0x30, 1 );          /* Prepend leading zeros     */
      }
   }
}


/*****************************************************************************

  Function:    hhutil_process_host_request

  Description: This function will process a host request by filling the auth_tx
               structure with data.
  Author: 
      Abhishek Verma
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      p_auth_tx - pointer to the auth_tx structure
  Return values:
      true - successful
      false - unsuccessful

******************************************************************************/
BYTE hhutil_process_host_request(pAUTH_TX p_auth_tx)
{
   CHAR           ErrorMsg[200] = "";   
   pPTE_MSG       p_msg_ins = NULL_PTR;

   NCF30          ncf30;
   CHAR           Buff[256];
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   LONG           ret_code; 

   PRINT("hhutil_process_host_request: RECEIVED DIALOG MESSAGE\n");

   /* Tell the authorizer to check cvv */
   if ( (p_auth_tx->TLF01_details.track2[0] != 0x00) ||
        (p_auth_tx->TLF01_details.track1[0] != 0x00) ||
		(p_auth_tx->TLF01_details.interest_rate[0] != 0x00)	) 		
   {	/* TF Phani.. Add DE53(interst rate) also..As an issuer ATP needs to validate 3 CSC /4 CSC code */
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
   }

   if ( (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key) ||
         (AUTH_ADVICE          == p_auth_tx->TLF01_details.tx_key) ||
         (AUTH_PRE_AUTHORIZATION_CANCLE == p_auth_tx->TLF01_details.tx_key))
   {
      if ( AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key )
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0420");

         /* Display reversal message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants or to networks.
          */
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff,"Reversal from CUP:" );
         nccup_log_message( 2,1,Buff,"hhutil_process_host_request",1 );
      }

	  else if( AUTH_PRE_AUTHORIZATION_CANCLE == p_auth_tx->TLF01_details.tx_key)
   	  {
		memset( Buff, 0x00, sizeof(Buff) );
		strcpy( Buff,"PreAuth Cancel from CUP:");
		nccup_log_message( 1,1,Buff,"hhutil_process_host_request",1 );
	 }
  
	    /* Must get the original transaction for reversals and advices.
	       * If the original transaction is not found, reply to the host
	       * and don't send the transaction to the authorizer, except for
	       * for advices that were approved by the host.
	       */
	      memset( &ncf30, 0x00, sizeof(NCF30) );
	      strcpy( ncf30.primary_key.network_id,    AppName ); 
	      strcpy( ncf30.primary_key.network_type, "A"      ); 
	      if (0 != strlen(p_auth_tx->TLF01_details.orig_sys_trace_num))
	      {
	         strcpy( ncf30.primary_key.batch_number, "ACTIVE" ); 
	         strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
	         strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
	         strcpy( ncf30.sys_trace_audit_nbr,
	                 p_auth_tx->TLF01_details.orig_sys_trace_num );

	         /* Build message to get the original transaction from data server. */
	         memcpy( buffer1,p_auth_tx, sizeof(AUTH_TX) );
	         memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );

	         p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
	                                       ST1_DB_SELECT, 
	                                       ST2_DB_SELECT_TRAN_BY_STAN,
	                                       netds_que_name, 
	                                       application_que_name,
	                                       (pBYTE)&buffer1,
	                                       sizeof(buffer1), 
	                                       NCF30_DATA );
	      }
      else
      {
	         strcpy( ncf30.card_nbr,      p_auth_tx->TLF01_details.card_num      );
	         strcpy( ncf30.tran_amount,   p_auth_tx->TLF01_details.total_amount  );
	         strcpy( ncf30.merchant_id,   p_auth_tx->TLF01_details.merchant_id   );
	         strcpy( ncf30.currency_code, p_auth_tx->TLF01_details.currency_code );

	         /* Build message to get the original transaction from data server. */
	         memcpy( buffer1,p_auth_tx, sizeof(AUTH_TX) );
	         memcpy( buffer1+sizeof(AUTH_TX),&ncf30, sizeof(NCF30) );

	         p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
	                                       ST1_DB_SELECT, 
	                                       ST2_DB_SELECT_TRAN_BY_MERCHANT_ID,
	                                       netds_que_name, 
	                                       application_que_name,
	                                       (pBYTE)&buffer1,
	                                       sizeof(buffer1), 
	                                       NCF30_DATA );
      }
	      if ( NULL_PTR == p_msg_ins  )
	      {
	    	  nccup_log_message( 1,3,"Insufficient Memory for select ncf30 message",
	    			  	  	  	  	  "hhutil_process_host_request",1 );
	         return( false );
	      }

	      /* Send message to get host specific data from data server. */
	      ret_code = pteipc_send( p_msg_ins, netds_que_name );
	      free( p_msg_ins );
	      if ( 0 > ret_code )
	      {
	         pteipc_get_errormsg( ret_code, ErrorMsg );
	         sprintf( Buff,"Failed transmit to queue: %s. %s",
	                  netds_que_name,ErrorMsg );
	         nccup_log_message( 1,3,Buff,"hhutil_process_host_request",1 );
	         return( false );
	      }
	      return( true );
   	}
     
   else if (false == hhutil_transfer_ctl_to_auth_app(p_auth_tx))
   {
	   nccup_log_message( 1,3,"Unable to send the request to the authorizor application",
			   	   	   	   	   "hhutil_process_host_request",1 );
      return(false);
   }
   return(true);
}



/*****************************************************************************

  Function:    hhutil_transfer_ctl_to_auth_app

  Description: This function will transfer the auth_tx to the authorizer.  
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_host_out = NULL_PTR;
   LONG   ret_code;
   CHAR	 Buffer[256];
   CHAR	 ErrorMsg[100];

     p_msg_host_out = ptemsg_build_msg (MT_AUTH_REQUEST, ST1_NONE, ST2_NONE, authorizer_que_name, application_que_name,
                                      (pBYTE)p_auth_tx, sizeof(AUTH_TX), 0);
   if ( NULL_PTR == p_msg_host_out )
	{
	   	nccup_log_message( 1,3,"Insufficient Memory to build host message",
	   			   	   	   	   "hhutil_transfer_ctl_to_auth_app",1 );
		return(false);
	}
	ret_code = pteipc_send(p_msg_host_out, authorizer_que_name);

	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "hhutil_transfer_ctl: error on pteipc_send to que %s: %s", authorizer_que_name, ErrorMsg );
		nccup_log_message( 1,3,Buffer,"hhutil_transfer_ctl_to_auth_app",1 );
		free( p_msg_host_out );
		return(false);
	}
	
   free( p_msg_host_out );
   return(true);
}


/*****************************************************************************

  Function:    INIT_TXN

  Description: This function initializes auth_tx for switched in transactions.

  Author:      D. Irby

  Inputs:      None

  Outputs:     p_auth_tx - Initialized auth_tx

  Return values:
      Always returns true
******************************************************************************/
INT init_txn( pAUTH_TX p_auth_tx )
{
   CHAR           unique_tran_str_id[21] = "";
   CHAR           temp[17] = "";
   INT            length;
   INT            size;
   INT            i;
   CHAR   temp_unique_tran[15]="";
   INT     tid_len = 0;
   INT     unique_tran_str_id_len=0;

   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type     = FINANCIAL;

   strcpy(p_auth_tx->TLF01_details.handler_queue, application_que_name);

   strcpy(p_auth_tx->TLF01_details.dispensation.origin,"RO");

    /* Set local date and time. */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ) )
   {
      genutil_format_date(temp);
      memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, 8); 
      memcpy(p_auth_tx->TLF01_details.time_hhmmss, temp + 8, 6);
   }
   
   /* Create a unique transaction id for the new auth_tx record. */
   memset(unique_tran_str_id, 0x00, sizeof(unique_tran_str_id));
   ptetime_get_strtime(unique_tran_str_id);
   strncpy( p_auth_tx->TLF01_details.tran_start_time, unique_tran_str_id,16 );

   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0x00,
          sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
   if(tid_len == 8)
   {
  		 memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id,
          		p_auth_tx->TLF01_details.terminal_id,
          		sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
   		strncat(p_auth_tx->TLF01_details.primary_key.transaction_id,
           		unique_tran_str_id+2, 12);
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
				  p_auth_tx->TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
		 strncat (unique_tran_str_id,temp_unique_tran,14);
		 
		 unique_tran_str_id_len= strlen(unique_tran_str_id);
		 memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
				 p_auth_tx->TLF01_details.terminal_id, tid_len );
		 strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				  unique_tran_str_id+tid_len, 
				  (unique_tran_str_id_len-tid_len) );
		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
   
	 }
   

   /* Set the message type for ISO8583. */
   if (AUTH_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0100");
   }
   else if (AUTH_SALE == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0200");
   }
   else if (AUTH_SALES_COMPLETION == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0220");
   }
   else if (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0420");
   }
   else if (AUTH_LOGON == p_auth_tx->TLF01_details.tx_key)
   {
      strcpy(p_auth_tx->TLF01_details.message_type,"0800");
   }

   return( true );
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
   CHAR      info_buffer[100];
   BYTE      net_consec_tmouts_ctr[4];
   BYTE      active_txns_ctr[4];
   BYTE      host_state[2];
   INT       update_host_status = false;

   CHAR 	table_name[15];
   
   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );


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

      ReadGenericTable( table_name, net_consec_tmouts_ctr, 
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
         WriteGenericTable( table_name, net_consec_tmouts_ctr, 
                            active_txns_ctr, host_state );
         (void)send_network_status_to_monitor();
      }
      nccup_log_message( 1,3,info_buffer,"info_buffer",1 );
   }

   /* Prepare the response and sent it. */
   strcpy( p_auth_tx->TLF01_details.response_code, "00" );
   p_auth_tx->TLF01_details.tx_key =
                  match_request_to_response( p_auth_tx->TLF01_details.tx_key );
   (void)incoming_other_host_response( p_auth_tx );
   return(true);
}


   
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

      case AUTH_SALE:
	     return(AUTH_SALE_RESPONSE); 
        break;

      case AUTH_SALES_COMPLETION:
	     return(AUTH_SALES_COMPLETION_RESPONSE); 
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
	  case AUTH_VOID_SALE:
		 return (AUTH_VOID_SALE_RESPONSE);
		 break;

	  default:
         return( request+100 ); /* Convert from request to a response. */
        break;

   }
}; /* match_request_to_response */




/*****************************************************************************

  Function:    incoming_other_host_response

  Description: This function will process an incoming response from another host
               that needs to go to the CUP host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx of the response message
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
INT incoming_other_host_response(pAUTH_TX p_auth_tx)
{
   INT    ret_code;
   NCF30  ncf30;

   PRINT("incoming_other_host_response\n");

   /* Responses to reversals and advices must be approved. */
   if ( (AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key )||
        (AUTH_REVERSAL_RESPONSE        == p_auth_tx->TLF01_details.tx_key )|| 
        (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)  )

   {
      strcpy( p_auth_tx->TLF01_details.response_code, "00" );
      memset( &ncf30, 0x00, sizeof(NCF30) );
      strcpy( ncf30.primary_key.network_id, AppName );
      strcpy( ncf30.primary_key.network_type, "A" );
      strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
      strcpy( ncf30.primary_key.retrieval_ref_num,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num );

      ret_code = delete_orig_ncf30( ncf30, p_auth_tx );
      if ( false == ret_code )
      {
    	  nccup_log_message( 1,1,"Unable to delete reversal's original txn in NCF3",
    			  	  	  	  	  "incoming_other_host_response",1 );
      }
   }
   
   /* Continue to send response to host. */
   return( incoming_other_host_response_db_reply( ) );
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
 *                NOTE:  This function assumes a txn switched in from Cup
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
                    "Blocked txn from CUP: MCC=%s, %s, Stan=%s, RRN=%s",
                     p_auth_tx->TLF01_details.category_code, CC,
                     p_auth_tx->TLF01_details.sys_trace_audit_num,
                     p_auth_tx->TLF01_details.retrieval_ref_num );

            strcpy( p_auth_tx->TLF01_details.response_text,
                   "Blocked txn from cup" );

            break;
         }
         i++;
      }
   }
   return( ret_val );
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
 *  NAME:         incoming_other_host_response_db_reply
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_p_auth_tx->  This function picks
 *                up where 'incoming_other_host_response' left off.  You come
 *                here after updating NCF21 and NCF30 if they required updating.
 *
 *  INPUTS:      None.
 *                
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT incoming_other_host_response_db_reply( )
{
   CHAR       temp[17] = {0};
   CHAR       err_mesg[200] = {0};
   CHAR       cup_response_code[4] = {0};
   INT        total_int;INT buf_temp_len = 0;
   INT        reversal_int;
   INT        actual_int;
   CHAR       temp_amount[13] = {0};
   CHAR       temp_cur_code[4] = {0};
   CHAR  	  err_buf[100]  = {0};
	  


   if (AUTH_AUTHORIZATION_RESPONSE == Auth_Tx.TLF01_details.tx_key && 
   	strcmp(Auth_Tx.TLF01_details.message_type,"0200")==0)
   {
       /* Indicates POS reversal Advice response */
         Auth_Tx.TLF01_details.tx_key = AUTH_SALE_RESPONSE;
   }
   
   // We need to change the key if ATP is declined the Preauth Canceleation becasue of 3rd currency
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))&&
	   (Auth_Tx.TLF01_details.tx_key== AUTH_REVERSAL_RESPONSE))
	{
		Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE;
	}

   build_host_structure(err_buf);
   
   if (true == build_host_msg(err_buf))
   {
   	 /* Forward Msg coming from Cadience HOST */
		 if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0 &&
		 	(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"nccup2",6) )
	  		)
		{
			buf_temp_len = atoi(Auth_Tx.resp_data_len);
			if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
			{	buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
				nccup_log_message( 2,1,"Sending Cadencie response to CUP Host",
				    			  	  	"incoming_other_host_response",1 );
			}
		 }
      		hhutil_send_host_response(&Auth_Tx);
     }
   else
   {
	   nccup_log_message( 1,1,"Error building response for the host",
	   				    		"incoming_other_host_response",1 );
      return(false);
   }
   

   if (AUTH_LOGON_RESPONSE == Auth_Tx.TLF01_details.tx_key)
   {
      return(true);
   }

   // We need to change the key so that we can display the trantype as reversal
   	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))&&
   	   (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))&&
   	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))&&
   	   (Auth_Tx.TLF01_details.tx_key== AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE))
   	{
   		Auth_Tx.TLF01_details.tx_key =  AUTH_REVERSAL_RESPONSE;
   		if(0 == strlen(Auth_Tx.TLF01_details.reversal_amount))
   		{
   			// Original transaction is not in DB. Tranlookup need to show without update
   			memset(Auth_Tx.TLF01_details.orig_message,0x00,
   					(sizeof(Auth_Tx.TLF01_details.orig_message))-1);
   		}
   	}
   insert_tlf01(&Auth_Tx );
   return(true);
}

/*****************************************************************************

  Function:    get_tran_type_2

  Description: This function will get the transaction type using the HOST_MESSAGE
               structure
  Author: 
      unknown
  Inputs:
      tx_key - the current transaction type
      HOST_MESSAGE (global) - the request buffer containing the message from the host
  Outputs:
      None
  Return values:
      BYTE - the calculated transaction type
  Modified by:
      FArad     8/13/98
******************************************************************************/
BYTE  get_tran_type_2(BYTE tx_key) 
{
	BYTE  ret_val;
	CHAR Errorbuf[100]="";

	/* Check for an Authorization request from another host */
	if (('1' == HOST_MESSAGE.msgtype [1] ) && ('0' == HOST_MESSAGE.msgtype [2])) 
	{
		if ( 0 == strncmp( HOST_MESSAGE.proccode, "00", 2 ) )  
		{
			return(AUTH_AUTHORIZATION);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "20", 2 ) )  
		{	
			return(AUTH_PRE_AUTHORIZATION_CANCLE);
		}

		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "30", 2 ) )	
		{   
			return(AUTH_BALANCE_INQUIRY);
		}
		//AUTH_PRE_AUTHORIZATION
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "03", 2 ) ) 
		{
			return (AUTH_PRE_AUTHORIZATION);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "01", 2 ) )
		{
			return(AUTH_CASH_ADVANCE);
		}
		else if (0 == strncmp(HOST_MESSAGE.proccode, "33", 2))
		{
			return (AUTH_CARD_VERIFICATION);
		}
	}
	
	/* response.  These are sent from the CUP host as 0110's.          */
	else if ( ('1' == HOST_MESSAGE.msgtype [1]) && ('1' == HOST_MESSAGE.msgtype [2]) )
	{
		/* now verify the procesing code to know what kind */
		if ( 0 == strncmp( HOST_MESSAGE.proccode, "00", 2 ) )  
		{
			return(AUTH_AUTHORIZATION_RESPONSE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "20", 2 ) )  
		{	
			if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp",3))
			{
				return(AUTH_VOID_SALE_RESPONSE);
			}
			else
			{
				return(AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE);
			}
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "30", 2 ) )	
		{   
			return(AUTH_BALANCE_INQUIRY_RESPONSE);
		}
		
		//AUTH_PRE_AUTHORIZATION
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "03", 2 ) ) 
		{
			return (AUTH_PRE_AUTHORIZATION);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "01", 2 ) )
		{
			return(AUTH_CASH_ADVANCE_RESPONSE);
		}
		else if (0 == strncmp(HOST_MESSAGE.proccode, "33", 2))
			{
				return (AUTH_CARD_VERIFICATION_RESPONSE);
			}
	}

	/* Check for an Purchage request from another host  0200 */
	else if (('2' == HOST_MESSAGE.msgtype [1] ) && ('0' == HOST_MESSAGE.msgtype [2])) 
	{
		if ( 0 == strncmp( HOST_MESSAGE.proccode, "00", 2 ) )  
		{
			return(AUTH_SALE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "20", 2 ) )	
		{	
			return(AUTH_VOID_SALE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "30", 2 ) )	
		{	
			return(AUTH_BALANCE_INQUIRY);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "01", 2 ) )
		{
			return(AUTH_CASH_ADVANCE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "02", 2 ) )
		{
			return(AUTH_VOID_SALE);
		}
	}

	/* response.  These are sent from the CUP host as 0210's.          */
	if ( ('2' == HOST_MESSAGE.msgtype [1]) && ('1' == HOST_MESSAGE.msgtype [2]) )
	{
		/* now verify the procesing code to know what kind */
		if ( 0 == strncmp( HOST_MESSAGE.proccode, "00", 2 ) )  
		{
			return(AUTH_SALE_RESPONSE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "20", 2 ) )  
		{	
			return(AUTH_VOID_SALE_RESPONSE);
		}

		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "30", 2 ) )	
		{   
			return(AUTH_BALANCE_INQUIRY_RESPONSE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "01", 2 ) )
		{
			return(AUTH_CASH_ADVANCE_RESPONSE);
		}
		else if ( 0 == strncmp( HOST_MESSAGE.proccode, "02", 2 ) )
		{
			return(AUTH_VOID_SALE_RESPONSE);
		}
	}

	/* or  an advise request 0220 */
	else if ( ('2' == HOST_MESSAGE.msgtype [1]) && ('2' == HOST_MESSAGE.msgtype [2]) )
	{

		if ( 0 == strncmp( HOST_MESSAGE.proccode, "00", 2 ) )  
		{	
			return(AUTH_OFFLINE_SALE);
		}
	}

	/* OFF LINE or  an advise response 0230 */
	else if ( ('2' == HOST_MESSAGE.msgtype [1]) && ('3' == HOST_MESSAGE.msgtype [2]) )
	{

		if ( 0 == strncmp( HOST_MESSAGE.proccode, "00", 2 ) )  
		{	
			return(AUTH_OFFLINE_SALE_RESPONSE);
		}
	} 

	/* or a Reversal response */  
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0410",4))
	{
		return( AUTH_REVERSAL_RESPONSE);
	}	
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0411",4))
	{
		return( AUTH_REVERSAL_REPEAT_RESPONSE);
	}
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0430",4))
	{
			return( AUTH_REVERSAL_ADVICE_RESPONSE);
	}
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0431",4))
	{
		return( AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE);
	}
		
	/* Check for a Reversal request from another host */ 
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0400",4))
	{
		return( AUTH_REVERSAL );
	}	
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0401",4))
	{
		return( AUTH_REVERSAL_REPEAT );
	}
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0420",4))
	{
			return( AUTH_REVERSAL_ADVICE );
	}
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0421",4))
	{
		return( AUTH_REVERSAL_ADVICE_REPEAT);
	}

    /*Advince Message */
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0120",4))
	{
		return( AUTH_ADVICE );
	}	
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0130",4))
	{
		return( AUTH_ADVICE_RESPONSE );
	}
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0121",4))
	{
			return( AUTH_ADVICE_REPEAT );
	}
	else if ( 0 == strncmp( HOST_MESSAGE.msgtype,"0131",4))
	{
		return( AUTH_ADVICE_REPEAT_RESPONSE);
	}

	
	/* or a ATM confirmation  request */  
	else if ( (0x01 == HOST_MESSAGE.msgtype [0]) && (0x02 == HOST_MESSAGE.msgtype[1]) )
		return( AUTH_ATM_CONFIRM );

	else if ( (0x06 == HOST_MESSAGE.msgtype [0]) && (0x20 == HOST_MESSAGE.msgtype[1]) )
		return( AUTH_ADMIN_ADVICE );

	/* ADMIN TEXT MESSAGE - TF PHANI*/
	else if ( (0x06 == HOST_MESSAGE.msgtype [0]) && (0x00 == HOST_MESSAGE.msgtype[1]) )
		return( AUTH_ADMINISTRATIVE_TEXT );

	/* Check for a batch upload from another host */
	else if ( (0x03 == HOST_MESSAGE.msgtype [0]) && (0x20 == HOST_MESSAGE.msgtype [1]))
		return( AUTH_BATCH_UPLOAD ); 

	/* Check for a settlement transaction from another host */
	else if ( ('5' == HOST_MESSAGE.msgtype [1]) && ('0' == HOST_MESSAGE.msgtype [2]))
	{

	}

	/* Check for a Key reset transaction  0800 message type*/
	else if ( ('8' == HOST_MESSAGE.msgtype [1]) && ('0' == HOST_MESSAGE.msgtype [2]))
	{
		return( AUTH_PIN_CHANGE );
	}
	else if ( ('8' == HOST_MESSAGE.msgtype [1]) && ('2' == HOST_MESSAGE.msgtype [2]))
	{
		return( AUTH_LOGON );
	}

	else if ( ('8' == HOST_MESSAGE.msgtype [1]) && ('3' == HOST_MESSAGE.msgtype [2]))
	{
		return( AUTH_LOGON_RESPONSE );
	}

	/*-----------------------------------------------------------------*/
	/* Check for response message types when dealing with other hosts. */
	/*-----------------------------------------------------------------*/
	else if ( ('2' == HOST_MESSAGE.msgtype [1]) && ('0' == HOST_MESSAGE.msgtype [2]))
	{
	}  

	/* or a transaction that the financial server does not understand */
	else
	{

		sprintf(Errorbuf,"Tran Type not supported for Message type : & and Processing code : %s",
		HOST_MESSAGE.msgtype ,HOST_MESSAGE.proccode);
		nccup_log_message( 1,2,Errorbuf,"get_tran_type_2",1 );
	}
}

/*****************************************************************************

  Function:     insert_tlf01()

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
INT insert_tlf01(pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   LONG       ret_code;
   CHAR       Buff[256];
   CHAR       err_mesg[200] = "";

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
	   nccup_log_message( 1,3,"Insufficient Memory to build insert TLF01 message",
			   	   	   	   	   "insert_tlf01",1 );
      return(false);
   }
   ret_code = pteipc_send(p_msg_ins, updateds_que_name); 
   if(ret_code < 0)
   {
      pteipc_get_errormsg(ret_code, err_mesg);
      sprintf(Buff, "failed transmit to que: %s. %s",
              updateds_que_name, err_mesg);
      nccup_log_message( 1,3,Buff, "insert_tlf01",1 );
      free(p_msg_ins);
      return(false);
   }
   free(p_msg_ins);
   return(true);
}


/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_request

  Description: This function will send the network management request to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR cup_response_code[4])
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[100];
   CHAR	    Buffer[256];
  
   build_host_structure(ErrorMsg);
   if ( false == build_host_msg(ErrorMsg) )
   {
	   nccup_log_message( 1,3,"Invalid bit field: building network management request msg.",
			   	   	   	   	   "hhutil_send_net_mngmnt_request",0 );
       return(false);
   }

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								      dialog_que_name,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	(void)set_originator_info( p_auth_tx, p_msg_host_out );
     
   if ( NULL_PTR == p_msg_host_out )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build host message",
	  			   	   	   	   "hhutil_send_net_mngmnt_request",0 );
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );

   free( p_msg_host_out );

 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "pteipc error sending to que %s: %s",
				 dialog_que_name, ErrorMsg );
		nccup_log_message( 1,3,Buffer,"hhutil_send_net_mngmnt_request",0 );
		return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return(true);
} /* hhutil_send_net_mngmnt_request */


/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_response

  Description: This function will send the network management response to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   CHAR	    ErrorMsg[100] = {0};
   CHAR	    Buffer[256] = {0};
   LONG      ret_code = true;

   build_host_structure(ErrorMsg);
   if ( false == build_host_msg(ErrorMsg) )
   {
	   nccup_log_message( 1,3,"Invalid bit field: building network management response msg.",
			   	   	   	   	   "hhutil_send_net_mngmnt_request",0 );
	   return(false);
   }

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              p_auth_tx->TLF01_details.originator_queue,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	(void)set_originator_info( p_auth_tx, p_msg_host_out );
     
   if ( NULL_PTR == p_msg_host_out )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build host message",
	   			   	   	   	   	"hhutil_send_net_mngmnt_request",0 );
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, p_auth_tx->TLF01_details.originator_queue );

   free (p_msg_host_out);

 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "pteipc error sending to que %s: %s",
				p_auth_tx->TLF01_details.originator_queue, ErrorMsg );
		nccup_log_message( 1,3,Buffer,"hhutil_send_net_mngmnt_request",0 );
		free(p_msg_host_out);
		return(false);
	}
   else
   {
   		 sprintf(Buffer,"Sending network managemnt response Message type: %s  and NMI Code:  %s to host",
	 				p_auth_tx->TLF01_details.message_type,p_auth_tx->function_code);
   		 nccup_log_message( 1,1,Buffer,"hhutil_send_net_mngmnt_request",1 );
   		 /* Update transaction statistics */
   		 update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

   		 /* Log transaction to trace file. */
   		 write_to_txn_file();
   }
   free(p_msg_host_out);
   return(true);
} /* hhutil_send_net_mngmnt_response */


/******************************************************************************
 *
 *  NAME:         update_ncf01
 *
 *  DESCRIPTION:  This function updates the NCF01 record with the new pin key.
 *                
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

void update_ncf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   BOOLEAN         ret_code = false;


   /* Build message to get host specific data from data server. */
   memset( buffer1, 0x00,     sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF01_DATA );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCF01 message",
			   	   	   	   	   "update_ncf01",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = send_msg( p_msg, netds_que_name, p_auth_tx );
      free( p_msg );
   }
}


/*****************************************************************************

  Function:    hhutil_send_host_response

  Description: This function will send the host response message out to the 
                host.
  Author: 
      Abhishek Kumar Verma
  Inputs:
      p_auth_tx - the auth_tx structure, used to get the tpdu_id 
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/

BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_out;
   LONG   ret_code;
   CHAR	 Buffer[256]="";
   CHAR	 ErrorMsg[100]="";

   ptetime_get_strtime(p_auth_tx->TLF01_details.tran_finish_time);
   p_msg_out = ptemsg_build_msg(MT_OUTGOING,
	         		            ST1_NONE,
						        ST2_NONE,
						        p_auth_tx->TLF01_details.originator_queue,
						        application_que_name,
                                ReqRsp_Buffer,
                                buflen,
						        0); 

   if ( NULL_PTR == p_msg_out )
	{
	   nccup_log_message( 1,3,"Insufficient Memory to build host message",
	   			   	   	   	 "hhutil_send_host_response",1 );
	   return(false);
	}
   (void)set_originator_info( p_auth_tx, p_msg_out );
	ret_code = pteipc_send(p_msg_out, p_auth_tx->TLF01_details.originator_queue);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s",
               p_auth_tx->TLF01_details.originator_queue, ErrorMsg );
      nccup_log_message( 1,3,Buffer,"hhutil_send_host_response",1 );
      free( p_msg_out );
      return(false);
	}
   else
   {
   	  strcat(Buffer,"Response message Send to host for message type : ");
	  strcat (Buffer ,Auth_Tx.TLF01_details.message_type);
	  nccup_log_message( 2,1,Buffer,"hhutil_send_host_response",1 );
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   free( p_msg_out );
   return(true);
}


void send_update_Key_message_to_nccup2( )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 			queue[10] = {0};
   BYTE            Error[100] = {0};
  

   strncpy(queue,"nccup2A",7);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_RESPONSE,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             queue, 
                             application_que_name,
                             NULL,
                             NULL, 
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP2 message",
			   	   	   	   "send_update_Key_message_to_nccup2",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void send_update_Pin_Transalate_info_to_nccup2( )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 			queue[10]="";
   BYTE            Error[100]="";


   strncpy(queue,"nccup2A",7);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
		   	   	   	   	   	 ST1_CUP_KEY_TRANSLATION_SUCCESS,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP2 message",
	   			   	   	   	   "send_update_Pin_Transalate_info_to_nccup2",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"Key Transalation is successful , Update NCCUP2 Info",
	  	   			   	   	  "send_update_Pin_Transalate_info_to_nccup2",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void nccup_process_logon_request_to_nccup3_after_key_transalation( )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};
  
   strncpy(queue,"nccup3A",7);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             queue, 
                             application_que_name,
                             NULL,
                             NULL, 
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
	  	  	   			   	   "nccup_process_logon_request_to_nccup3_after_key_transalation",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"Key Transalation is successful , Update NCCUP3 to go online",
	   	  	  	   		 "nccup_process_logon_request_to_nccup3_after_key_transalation",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}


void send_update_Key_message_to_nccup_ACQ( )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 			queue[10] ={0};
   BYTE            Error[100] = {0};
  

   strncpy(queue,"nccupA",6);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_RESPONSE,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             queue, 
                             application_que_name,
                             NULL,
                             NULL, 
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   	   nccup_log_message( 1,1,"Insufficient Memory to build insert NCCUP2 message",
	   	   	  	  	   		 	 	 "send_update_Key_message_to_nccup2",0 );
	   	   ret_code = false;
   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void nccup_process_logon_request_after_key_transalation_success( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};


   strncpy(queue,"nccup3A",7);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
		   	   	   	   	   	 ST1_CUP_KEY_TRANSLATION_SUCCESS,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
	   	   	   	  	  	   		"nccup_process_logon_request_after_key_transalation_success",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"Key transalation successful , sending message to nccup3 to go online ",
	   	   	   	   	  	  	   "nccup_process_logon_request_after_key_transalation_success",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void nccup_process_logon_request_after_key_transalation_failed( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"nccup3A",7);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
		   	   	   	   	   	 ST1_CUP_KEY_TRANSLATION_FAILED,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   	 nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message ",
	   	   	   	   	   	  	  	"nccup_process_logon_request_after_key_transalation_failed",0 );
         ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,3,"Key transalation failed , make nccup3 status as OFFLINE",
	   	   	   	   	   	   	  "nccup_process_logon_request_after_key_transalation_failed",0 );
       ret_code = send_msg( p_msg, queue, Error );
       free( p_msg );
   }
}


void nccup_send_cup_key_to_issuing_module( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)] = {0};

   strncpy(queue,"nccup3A",7);
   /* Build message to get host specific data from data server. */

   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_Nccup3_I, sizeof(Ncf01_Nccup3_I) );

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_UPDATE_REQUEST,
                             ST1_DB_UPDATE,
                             ST2_NONE,
							 queue,
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1),
							 CUP_KEY_UPDATE );

   if ( NULL_PTR == p_msg  )
   {
	   	 nccup_log_message( 1,3,"Insufficient Memory to Send key update request to NCCUP3",
	   	   	   	   	   	   	   	  "nccup_send_cup_key_to_issuing_module",0 );
         ret_code = false;

   }
   else
   {
	   nccup_log_message( 1,1,"Sending message to get new CUP key to NCCUP3",
	   	   	   	   	   	   	   "nccup_send_cup_key_to_issuing_module",0 );
      /* Send message to update host specific data with data server. */
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void nccup_send_logoff_message_to_nccup( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"nccupA",6);

   p_msg = ptemsg_build_msg( MT_SEND_LOG_OFF,
		   	   	   	   	   	 ST1_NONE,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             NULL );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP message",
	   	   	   	   	   	   	   "nccup_send_logoff_message_to_nccup",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"nccup3 received logoff message, sending nccup also to go OFFLINE",
	   	   	   	   	   	   "nccup_send_logoff_message_to_nccup",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void nccup_send_logoff_message_to_nccup3( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"nccup3A",7);

   p_msg = ptemsg_build_msg( MT_SEND_LOG_OFF,
		   	   	   	   	   	 ST1_NONE,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             NULL );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
	 	   	   	   	   	   	   "nccup_send_logoff_message_to_nccup3",0 );
	   ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"nccup received logoff message, sending nccup3 also to go OFFLINE",
	   	 	   	   	   	   	  "nccup_send_logoff_message_to_nccup3",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void nccup_send_logoff_message_to_nccup_after_key_translation_read_fail( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"nccupA",6);

   p_msg = ptemsg_build_msg( MT_SEND_LOG_OFF,
		   	   	   	   	   	 ST1_NONE,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             NULL );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP message",
	   	   	 	   	   	   	  "nccup_send_logoff_message_to_nccup",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"nccup3 going offline, sending nccup also to go OFFLINE",
	   	   	   	 	   	   	  "nccup_send_logoff_message_to_nccup",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

/*
 * This function is used by nccup3 module send nccup2 module to go offline after CUP
 * Key transalation is failed
 *
 * */

void nccup_send_logoff_request_to_nccup2_after_key_transalation_failed( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"nccup2A",7);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
		   	   	   	   	   	 ST1_CUP_KEY_TRANSLATION_FAILED,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
	   	   	   	   	 	   	  "nccup_send_logoff_request_to_nccup3_after_key_transalation_failed",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,3,"Key transalation failed , make nccup2 status as OFFLINE",
	   	   	   	   	   	 	"nccup_send_logoff_request_to_nccup3_after_key_transalation_failed",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}


/*
 * This function read incoming CUP from nccup3 ncf01 issuing table
 */

INT nccup_read_incoming_cup_key_from_nccup3_ncf01_issuing_table(void)
{
	INT    ret_val = true;
	memset(static_Rtdcwk_Key,0x00, (sizeof(static_Rtdcwk_Key)-1));
	memset(MAC,0x00,16);

	ret_val = nccup_read_cup_key_for_nccup3_active_active_trigger();
	return ret_val;
}

/*
 * This function will send active active trigger to nccup module
 */

INT nccup_send_active_active_trigger_to_nccup_module( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"nccupA",6);

   p_msg = ptemsg_build_msg( MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
		   	   	   	   	   	 ST1_CUP_ACTIVE_ACTIVE_TRIGGER,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
                             CUP_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
	   	   	   	   	   	   	 	"nccup_send_active_active_trigger_to_nccup_module",0 );

         ret_code = false;
   }
   else
   {
      /* Send message to update host specific data with data server. */
	   nccup_log_message( 1,1,"Active Active trigger is sent to nccup module",
	   	   	   	   	   	   	   "nccup_send_active_active_trigger_to_nccup_module",0 );
      ret_code = send_msg( p_msg, queue, Error );
      free( p_msg );
      return ret_code;
   }
}


/*
 * This function will send active active trigger to nccup module
 */

INT nccup_send_active_active_trigger_to_nccup2_module( void )
{
	pPTE_MSG        p_msg = NULL_PTR;
	BOOLEAN         ret_code = false;
	CHAR 		   queue[10] = {0};
	BYTE            Error[100] = {0};

	strncpy(queue,"nccup2A",7);

	p_msg = ptemsg_build_msg(MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
							 ST1_CUP_ACTIVE_ACTIVE_TRIGGER,
							 ST2_NONE,
							 queue,
							 application_que_name,
							 NULL,
							 NULL,
							 CUP_KEY_UPDATE );
	if ( NULL_PTR == p_msg  )
	{
		nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
			   	   	   	   	  "nccup_send_active_active_trigger_to_nccup2_module",0 );
		ret_code = false;
	}
	else
	{

	/* Send message to update host specific data with data server. */
		nccup_log_message( 1,1,"Active Active trigger is sent to nccup module",
					   	   	   "nccup_send_active_active_trigger_to_nccup_module",0 );
		ret_code = send_msg( p_msg, queue, Error );
		free( p_msg );
	}
	return ret_code;
}

/*
 * This function will handle active active trigger for nccup module
 */

void nccup_activ_active_trigger_handler_for_nccup()
{
	CHAR	ErrorMsg[100] = {0};
	CHAR   	Buffer[100] = {0};
	INT    	ret_val = true;

	memset(static_Rtdcwk_Key,0x00,32);
	memset(MAC,0x00,16);

	strcpy(Buffer,"Received Active Active trigger from nccup3 module");
	nccup_log_message( 1, 1, Buffer,
				 "nccup_activ_active_trigger_handler_for_nccup",0 );

	if (true == nccup_Get_key_Transalation_Info_from_nccup_table())
	{
		memcpy(static_Rtdcwk_Key, Ncf01_I.retired_cwk,(sizeof(static_Rtdcwk_Key)-1));
		memcpy(MAC,Ncf01_I.kek, (sizeof(MAC)-1));
		strcpy(ErrorMsg,"Active Active Key update Successful ");
		nccup_log_message( 1, 1, ErrorMsg, "nccup_activ_active_trigger_handler_for_nccup",0 );
	}
	else
	{
		ret_val = false;
		strcpy(ErrorMsg,"Active Active Key update failed ");
		nccup_log_message( 1, 3, ErrorMsg, "nccup_activ_active_trigger_handler_for_nccup",0 );
		//Send nccup3 module to go offline as db read failed
		nccup_send_logof_to_nccup3_after_active_artive_triger_failed_from_nccup();
	}
}

/*
* This function will send a message to nccup3 when nccup module fails to
* read key from DB for Active Active setup.
*/

INT nccup_send_logof_to_nccup3_after_active_artive_triger_failed_from_nccup( void )
{
	pPTE_MSG        p_msg = NULL_PTR;
	BOOLEAN         ret_code = false;
	CHAR 		    queue[10] = {0};
	BYTE            Error[100] = {0};

	strncpy(queue,"nccup3A",7);

	p_msg = ptemsg_build_msg(   MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
								ST1_CUP_ACTIVE_ACTIVE_TRIGGER_FAILED,
								ST2_NONE,
								queue,
								application_que_name,
								NULL,
								NULL,
								CUP_KEY_UPDATE );
	if ( NULL_PTR == p_msg  )
	{
		nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP message",
							   	"nccup_send_logof_to_nccup3_after_active_artive_triger_failed_from_nccup",0 );
		ret_code = false;
	}
	else
	{
		/* Send message to update host specific data with data server. */
		nccup_log_message( 1,3,"Key Reload failed for active active, make nccup3 status as OFFLINE",
								"nccup_send_logof_to_nccup3_after_active_artive_triger_failed_from_nccup",0 );
		ret_code = send_msg( p_msg, queue, Error );
		free( p_msg );
	}
}

/*
* This function will send a message to nccup2 when nccup3 module fails to
* read key from DB for Active Active setup.
*/

void nccup_send_logof_to_nccup2_after_active_artive_triger_failed_from_nccup3( void )
{
	pPTE_MSG        p_msg = NULL_PTR;
	BOOLEAN         ret_code = false;
	CHAR 		   queue[10] = {0};
	BYTE            Error[100] = {0};

	strncpy(queue,"nccup2A",7);

	p_msg = ptemsg_build_msg(   MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
								ST1_CUP_ACTIVE_ACTIVE_TRIGGER_FAILED,
								ST2_NONE,
								queue,
								application_que_name,
								NULL,
								NULL,
								CUP_KEY_UPDATE );
	if ( NULL_PTR == p_msg  )
	{
		nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
								"nccup_send_logof_to_nccup2_after_active_artive_triger_failed_from_nccup3",0 );
		ret_code = false;
	}
	else
	{
		/* Send message to update host specific data with data server. */
		nccup_log_message( 1,3,"Key Reload failed for active active, make nccup2 status as OFFLINE",
										"nccup_send_logof_to_nccup2_after_active_artive_triger_failed_from_nccup3",0 );
		ret_code = send_msg( p_msg, queue, Error );
		free( p_msg );
	}
}

/*
* This function will send a message to nccup when nccup3 module fails to
* read key from DB for Active Active setup.
* */
void nccup_send_logof_to_nccup_after_active_artive_triger_failed_from_nccup3( void )
{
	pPTE_MSG        p_msg = NULL_PTR;
	BOOLEAN         ret_code = false;
	CHAR 		   queue[10] = {0};
	BYTE            Error[100] = {0};

	strncpy(queue,"nccupA",7);

	p_msg = ptemsg_build_msg(   MT_NSP_CUP_KEY_TRANSLATE_RESPONSE,
								ST1_CUP_ACTIVE_ACTIVE_TRIGGER_FAILED,
								ST2_NONE,
								queue,
								application_que_name,
								NULL,
								NULL,
								CUP_KEY_UPDATE );
	if ( NULL_PTR == p_msg  )
	{
		nccup_log_message( 1,3,"Insufficient Memory to build insert NCCUP3 message",
								"nccup_send_logof_to_nccup_after_active_artive_triger_failed_from_nccup3",0 );
		ret_code = false;
	}
	else
	{
		/* Send message to update host specific data with data server. */
		nccup_log_message( 1,3,"Key Reload failed for active active, make nccup status as OFFLINE",
								"nccup_send_logof_to_nccup_after_active_artive_triger_failed_from_nccup3",0 );
		ret_code = send_msg( p_msg, queue, Error );
		free( p_msg );
	}
}


INT nccup_get_service_code( pCHAR service_code, pCHAR error_msg )
{
	pBYTE  field_separator = NULL_PTR;
	pBYTE  field_separator1 = NULL_PTR;
	INT    ret_val = false;
	INT    card_num_len;
	INT    track2_idx;

	if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
	{
		/* Locate the field separator. */
		field_separator = Auth_Tx.TLF01_details.track2;
		for( track2_idx=0; track2_idx <= CUP_MAX_CARD_NUM_LEN;
		track2_idx++,field_separator++)
		{
			if ( !isdigit( *field_separator ) )
				break;
		}

		if ( track2_idx > CUP_MAX_CARD_NUM_LEN )
		{
			strcpy( error_msg,"Unable to parse track2.  Cannot find field separator.");
		}
		else
		{
			/* Get the service code. */
			memcpy( service_code, field_separator+5, 3 );
			if(0 == strlen(Auth_Tx.TLF01_details.exp_date) )
			{
				memcpy(Auth_Tx.TLF01_details.exp_date,field_separator+1,4);

			}
			ret_val = true;
		}
	}

	else if ( Auth_Tx.TLF01_details.track1[0] != 0x00 )
	{
		/* Locate the field separator. */

		field_separator=strchr(Auth_Tx.TLF01_details.track1,'^');
		if((field_separator==NULL) ||((((int)field_separator) - ((int)Auth_Tx.TLF01_details.track1)) > CUP_MAX_CARD_NUM_LEN))
		{
			strcpy( error_msg,"Unable to parse track1.  Cannot find field separator.");
		}
		else
		{
			field_separator1=strchr(field_separator+1,'^');

			if((field_separator1==NULL )||((field_separator1-(field_separator+1))>CUP_MAX_CARD_HOLDER_LEN))
			{
				strcpy( error_msg,"Unable to parse track1.  Cannot find field separator.");
			}
		}
		if(*field_separator1 == '^')
		{
			memcpy( service_code, field_separator1+5, 3 );
			if(0 == strlen(Auth_Tx.TLF01_details.exp_date) )
			{
				memcpy(Auth_Tx.TLF01_details.exp_date,field_separator+1,4);

			}
			ret_val = true;
		}

	}
	return( ret_val );
}

INT nccup_check_emv_capability_device(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.terminal_type ,
					 CUP_EMV_CAPABILITY_INDICATOR,
					 CUP_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
	Abhishek
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/

BOOLEAN blank_or_null_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}


/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void nccup_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  nccup_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   nccup_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(nccup_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  nccup_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(nccup_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  nccup_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(nccup_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(nccup_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(nccup_error_warning_file_path) > 0) &&
	  (strlen(nccup_module_error_warning_file_name)> 0))
   {
	   nccup_create_Error_Filename();
   }
}

INT nccup_Log_error_warning_to_File(pCHAR Error_Buf, int sev,pCHAR func,int detail)
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
    len = strlen(nccup_Erro_warning_Filename);
    path_len = strlen(nccup_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&nccup_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	nccup_create_Error_Filename();
    }
	if((fp = fopen(nccup_Erro_warning_Filename,"a+b"))==NULL)
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

void nccup_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( nccup_Erro_warning_Filename, nccup_error_warning_file_path );
    strcat( nccup_Erro_warning_Filename, nccup_module_error_warning_file_name );
	strcat( nccup_Erro_warning_Filename, ".EW." );
    strncat(nccup_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(nccup_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(nccup_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

BOOLEAN nccup_is_issuing_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   nccup_issuing_link_name,
					   CUP_ISSUING_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN nccup_is_POS_Acq_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
			   	   	   pos_handler_module_name,
					   CUP_POS_HANDLER_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN nccup_is_voice_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   voice_handler_que_name,
					   CUP_VOICE_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN nccup_is_sbatch_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
				       sbatch_handler_que_name,
					   CUP_SBATCH_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN nccup_is_approved_by_Cadencie(void)
{
	INT buf_temp_len = 0;

	buf_temp_len = atoi(Auth_Tx.resp_data_len);

    if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
    {
    	if(  0 == strncmp(  Auth_Tx.TLF01_details.authorizing_host_queue,
    						cadencie_cup_queue_name,
							CUP_CANDENCIE_LINK_QUEUE_NAME_LEN) )
		{
			return true;
		}
		else
		{
			return false;
		}
    }
}

BOOLEAN nccup_Check_If_CUP_ECOM(void)
{
	if ( 0 == strncmp( Auth_Tx.TLF01_details.product_codes[3].quantity,
					   CUP_ECOM_TERM_TYPE,
					   CUP_DE60_TERM_TYPR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN nccup_Check_If_CUP_MOTO_TRN(void)
{
	if((0 == strncmp(Auth_Tx.industry_code,CUP_MOTO_INDUSTRY_CODE,INDUSTRY_CODE_CODE))&&
	   ((true == nccup_is_POS_Acq_transaction())||
		(true == nccup_is_voice_transaction()))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"001",3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN nccup_Check_If_CUP_RPS_TRN(void)
{
	if((0 == strncmp(Auth_Tx.TLF01_details.merchant_id,
			   	   	 CUP_MID_INDICATOR_DEFAULT,
					 CUP_MID_INDICATOR_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			   	   	 CUP_RPS_POS_ENTRY_MODE,
					 CUP_RPS_POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
			   	   	 CUP_ISO_RPS_POS_CONDITION_CODE,
					 CUP_DE25_SIZE))&&
		(true == nccup_is_POS_Acq_transaction()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN nccup_Check_If_CUP_BDO_PAY(void)
{
	if((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					 CUP_BDO_PAY_POS_ENTRY_MODE,
					 CUP_BDO_POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
			   	     CUP_BDO_PAY_POS_CONDITION_CODE,
					 CUP_DE25_SIZE))&&
		(true == nccup_is_POS_Acq_transaction()))
		{
			return true;
		}
		else
		{
			return false;
		}
}

void nccup_Compute_AVN_Value(char * AVN_value)
{
	    char data[29]={0};
		char          *out;
		size_t      out_len;
		char hexValue[100]={0};
		char data_len[4]={0};
		int AVN_Len=0;

		strncpy(data_len,Auth_Tx.future_use_2+53,3);
		AVN_Len=atoi(data_len);
		strncpy(data,Auth_Tx.future_use_2+56,AVN_Len);

		out_len = b64_decoded_size(data)+1;
		out = malloc(out_len);

		if(out == NULL) {
	    printf("Error! memory not allocated.");
	    exit(0);
	  }

		if (!b64_decode( (unsigned char *)data, (unsigned char *)out, out_len)) {
			printf("Decode Failure\n");
			return 1;
		}
		out[out_len] = '\0';
		Cup_string2hexString(out,hexValue);
		strncpy(AVN_value,hexValue,40);
		free(out);

		return 0;


}

BOOLEAN nccup_check_txn_for_CUP_authentication_attempt()
{
	if (0 == strncmp(Auth_Tx.TLF01_details.profile,"06",2) &&
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[1].amount[CUP_AV_AUTHENTICATION_RESULT_POS],"06",CUP_AV_AUTHENTICATION_RESULT_LEN) &&
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[1].amount[CUP_AV_GENERATOR_INDICATOR_POS],2,CUP_AV_AUTHENTICATION_RESULT_LEN))
	{
		return true;
	}
	return false;
}

BYTE Nccup_Send_Interactive_DB_Request( pPTE_MSG  p_msg_out,
                                               pPTE_MSG *p_msg_in,
                                               pCHAR     que_name,
                                               pCHAR     error_msg )
{
   pBYTE           p_data;
   pPTE_MSG_DATA   p_msg_data;
   CHAR            temp_str[100] = {0};
   LONG            retcode;
   BYTE            dberror;

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to %s: %s",
               que_name, temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+sizeof(AUTH_TX), sizeof(temp_str)-1);
      sprintf( error_msg, "Database Error: %s", temp_str );
   }
   return dberror;
}


BOOLEAN nccup_load_CUP3DS2_records_for_CUP()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(CUP3DS2)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &cup3ds2,   0x00, sizeof(CUP3DS2) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	  ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
	                                 CUP3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = Nccup_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													tran_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &cup3ds2, p_data+sizeof(AUTH_TX), sizeof(CUP3DS2) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

void nccup_read_TO_RC_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[3]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_TIMEOUT_RC",           /* points to section name         */
									  "SCHEME_TIMEOUT_RC_FOR_UPI",       	/* points to key name             */
									   "68",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(cup_scheme_timeout_rc,readvalue,2);
	   if(rc == 0)
	   {
		   nccup_log_message( 2, 1, "SCHEME_TIMEOUT_RC_FOR_UPI Value not read from tf.ini","nccup_read_TO_RC_from_tf_ini",0);
	   }

}

void nccup_read_fai_RC_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[3]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",           /* points to section name         */
									  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_UPI",       	/* points to key name             */
									   "68",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(cup_scheme_fai_rc,readvalue,2);
	   if(rc == 0)
	   {
		   nccup_log_message( 2, 1, "SCHEME_TIMEOUT_RC_FOR_UPI Value not read from tf.ini","nccup_read_fai_RC_from_tf_ini",0);
	   }

}

INT nccup_check_if_trx_is_bdo_pos_acq_trx( )
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

INT nccup_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( )
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

INT nccup_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
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

INT nccup_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( )
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
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT nccup_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_with_pin(  )
{
	if((true == nccup_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin()) &&
	   (true == nccup_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == nccup_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT nccup_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_without_pin(    )
{
	if((true == nccup_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin()) &&
	   (true == nccup_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == nccup_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/*****************************/

/*Swiftpass BDO Pay Credit Trx*/
INT nccup_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( )
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

INT nccup_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( )
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

INT nccup_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( )
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
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT nccup_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_with_pin(  )
{
	if((true == nccup_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin()) &&
	   (true == nccup_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == nccup_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT nccup_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_without_pin()
{
	if((true == nccup_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin()) &&
	   (true == nccup_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == nccup_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
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

INT nccup_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( )
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
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT nccup_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( )
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

INT nccup_check_if_trx_is_upi_ecommerce_swiftpass_BDOPay_trx( )
{
	if((true == nccup_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx()) &&
	   (true == nccup_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == nccup_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


