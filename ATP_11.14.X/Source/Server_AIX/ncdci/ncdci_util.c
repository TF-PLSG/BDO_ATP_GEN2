/******************************************************************************
 
 	ncdci_util.c
 
    Copyright (c) 2004, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  ncdci Host Handler Driver
 
 	This module is the host handler for the Equitable host.  It
    will process all messages destined to, or originated from,
    the ncdci host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
AUTHOR : Abhishek Verma
   
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
#include "txtrace.h"
#include "ncdci_bitfields.h"
#include "diners.h"
#include "ncdci_prototypes.h"
#include <time.h>

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
extern CHAR    ncdci_issuing_link_name[];
extern CHAR    pos_handler_module_name[];
extern CHAR    voice_handler_que_name[];
extern CHAR    sbatch_handler_que_name[];
extern CHAR    cadencie_dci_queue_name[];
extern CHAR    ncdci2_fileupdate_link_que_name[];


/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern NCF01_KEYBLOCK     Ncf01_Keyblock;

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
extern CHAR  ncdci_Erro_warning_Filename[256] ;
extern CHAR  ncdci_module_error_warning_file_name[256];
extern CHAR  ncdci_error_warning_file_path[256];
long int Max_File_Size_Defined = 0 ;

/*Flags*/
extern INT extendedbit;
extern INT Txn_Is_A_Reject;
extern INT tpdu_id_flag ;

char dci_scheme_timeout_rc [4]={0};
char dci_scheme_fai_rc [4]={0};
/* LOCAL VARIABLES */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    BitFieldSize;

extern keyblock_enable_flag_for_DCI;

/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE   ReqRsp_Buffer [DINERS_COMM_BUF_SZE];

struct trantype_struct 
{
   BYTE iso_msg_type[5];
   BYTE iso_proc_code[7];
   BYTE host2_msg_type[3];
   BYTE host2_tran_code[5];
   BYTE atp_tx_key;
}; 

extern DCI3DS2 diners_3ds2;

CHAR tran_dataserver_que_name [] = "trandsA" ;

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
 * NAME:        ncdci_calc_julian_day
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
void ncdci_calc_julian_day( pCHAR date_str, pCHAR julian_str )
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
 *  NAME:         ncdci_get_date
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
void ncdci_get_date( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
}
   

/******************************************************************************
 *
 *  NAME:         ncdci_get_time
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
void ncdci_get_time( pCHAR time_date, pCHAR time_str )
{
   memset ( time_str,  0x00,          7 );
   strncpy( time_str, &time_date[11], 2 );
   strncat( time_str, &time_date[14], 2 );
   strncat( time_str, &time_date[17], 2 );
}


/******************************************************************************
 *
 *  NAME:         ncdci_create_rrn
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
void ncdci_create_rrn( pCHAR rrn )
{
   CHAR    time_date[25] = {0};
   CHAR    date_str[9] = {0};
   CHAR    time_str[7] = {0};
   CHAR    julian_day[4] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   ncdci_get_date( time_date, date_str );
   ncdci_get_time( time_date, time_str );

   /* Change date to julian format - DDD */
   ncdci_calc_julian_day( date_str, julian_day );

   /* Format RRN: "YDDDhmmssjjj" */
   sprintf( rrn, "%1.1s%3.3s%5.5s%3.3s",time_date+3, julian_day,
                  time_str+1, time_date+20 );
}

/******************************************************************************
 *
 *  NAME:         ncdci_copy_to_req_resp_buffer
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
void ncdci_copy_to_req_resp_buffer ( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA    p_msg_data  = NULL_PTR;
   pBYTE            p_data      = NULL_PTR;
   LONG             msglen = 0;

   memset(ReqRsp_Buffer, 0x00, DINERS_COMM_BUF_SZE);   /* zero out the buffer */

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   msglen     = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   reqidx = 0;
   buflen = msglen;
	if(buflen >= DINERS_COMM_BUF_SZE)
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
 *  RTRN VALUE:   DINERS_SWITCHED_IN  or  DINERS_SWITCHED_OUT
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_determine_txn_flow( pCHAR msgtype, INT source )
{
   INT  tran_flow = 0;

   /* Third character of msgtype is 1 or 3 for responses. */
   if ( (msgtype[2] == '1') || (msgtype[2] == '3') )
   {
      /* This is a response.  Where did it come from? */
      if ( source == DINERS_HOST )
         tran_flow = DINERS_SWITCHED_OUT;
      else
         tran_flow = DINERS_SWITCHED_IN;
   }
   else
   {
      /* This is a request. */
      if ( source == DINERS_HOST )
         tran_flow = DINERS_SWITCHED_IN;
      else
         tran_flow = DINERS_SWITCHED_OUT;
   }
   return( tran_flow );
}


/******************************************************************************
 *
 *  NAME:         ncdci_send_response_to_host
 *
 *  DESCRIPTION:  This function sends a response to the host.  It gets the
 *                response code and other information from Auth_Tx.
 *
 *  INPUTS:       send_flag - Used to determine if a resp should go to host
 *                            Values:
 *                               DINERS_SEND_UNCONDITIONALLY = Always send
 *                               DINERS_SEND_IF_NOT_REVERSAL = Do not send reversals
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_send_response_to_host( INT send_flag )
{
   CHAR  err_buf[100] = {0};
   BYTE  temp_tx_key = 0;
   INT buf_temp_len = 0;

   TranFlow = DINERS_SWITCHED_IN;
   if ( (send_flag == DINERS_SEND_UNCONDITIONALLY) || ((send_flag == DINERS_SEND_IF_NOT_REVERSAL) ) )
   {
      if ( ncdci_is_reversal() )
         temp_tx_key = Auth_Tx.TLF01_details.tx_key;

      /* Make sure the tx_key is for a response. */
      if ( Auth_Tx.TLF01_details.tx_key < 100 )
         Auth_Tx.TLF01_details.tx_key += 100;

      if ( true == ncdci_build_response_msg() )
      {
		 buf_temp_len = atoi(Auth_Tx.resp_data_len);
		 if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
		 {	
			if(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "ncdci2",6))
			{
				buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
				sprintf (err_buf, "Sending Cadencie response to ncdci Host, message type :%s", 
				Auth_Tx.TLF01_details.message_type) ;
				ncdci_log_message( 2, 1,err_buf , "ncdci_send_response_to_host",1 );
			}
		 }      
         if ( false == ncdci_send_message_to_host(err_buf,false) )//praneeth added false as parameter
            ncdci_log_message(1, 2, err_buf, "ncdci_send_response_to_host",1 );
      }

      (void)ncdci_insert_db_record( TLF01_DATA );

      if ( ncdci_is_reversal() )
         Auth_Tx.TLF01_details.tx_key = temp_tx_key;
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_send_request_to_host
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_send_request_to_host( pCHAR timer_resp_flag )
{
	INT   ret_val;
	INT   seconds,buf_temp_len=0;
	CHAR  err_buf[100]  = {0};
	CHAR  lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  gmt_time[20] = {0};
	CHAR  test_buf[100] = {0};
	CHAR  temp_timeout_sec[3] = {0};

	TranFlow = DINERS_SWITCHED_OUT;
	Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;

	/* Current Greenwich Mean Time for the transmission-to-host time. */
	if ( 0 == strcmp( NULL_STR,
					 Auth_Tx.TLF01_details.transmission_timestamp ) )
	{
		pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
		strncpy( Auth_Tx.TLF01_details.transmission_timestamp,
				 &gmt_time[4], 10 );  /* MMDDhhmmss */
	}

	ret_val = ncdci_build_request_msg( err_buf );
	if ( true == ret_val )
	{
		ptetime_get_strtime( Auth_Tx.TLF01_details.host_start_time );
		/* Set the request timer. */
		(void)ncdci_create_request_timer_key( lookup_key );
		strncpy(temp_timeout_sec,Ncf01_I.request_timer,2);
		seconds = atoi(temp_timeout_sec);
		//seconds = atoi(Ncf01_I.request_timer);
		if(0 == seconds || seconds > 30)
		{
			// Set request timer as 25 sec.
			strcpy(test_buf,"unable to retrive request timer value from NCF01 table");
			ncdci_log_message(2,2,test_buf,ncdci_send_request_to_host,1);
			seconds = 20;
		}
		strcpy( TimerAppData2, timer_resp_flag );
		ret_val = ncdci_set_timer( lookup_key, seconds, DINERS_REQUEST_TIMER );
		if ( true == ret_val )
		{
			/* Send the transaction request to the host. */
			ret_val = ncdci_send_message_to_host( err_buf,false );//praneeth added false as parameter
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
		ncdci_log_message( 1, 2, err_buf, "ncdci_send_request_to_host",1 );
		ret_val = ncdci_perform_standin_processing( DINERS_SYSTEM_ERROR,
													GS_TXN_DROPPED,
													err_buf );
		if ( false == ret_val )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_send_request_to_host",1 );
		}
	}

	return;
}



/******************************************************************************
 *
 *  NAME:         ncdci_send_file_request_to_host
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_send_file_request_to_host( pCHAR timer_resp_flag )
{
	INT   ret_val;
	INT   seconds,buf_temp_len=0;
	CHAR  err_buf[100]  = {0};
	CHAR  lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  gmt_time[20] = {0};
	CHAR  mesbuf[100] = {0};

	TranFlow = DINERS_SWITCHED_OUT;
	Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;

	ncdci_build_request_msg( err_buf );
	ptetime_get_strtime( Auth_Tx.TLF01_details.host_start_time );
	/* Set the request timer. */
	(void)ncdci_create_request_timer_key( lookup_key );
	seconds = atoi(Ncf01_I.request_timer);
	strcpy( TimerAppData2, timer_resp_flag );
	ret_val = ncdci_set_timer( lookup_key, seconds, DINERS_REQUEST_TIMER );
	if ( true == ret_val )
	{
		/* Send the transaction request to the host. */
		buf_temp_len = atoi(Auth_Tx.resp_data_len);
		if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
		{
			buflen = buf_temp_len;
			memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
			sprintf (mesbuf, "Sending Message coming from Cadencie to DCI, message type :%s",
					 Auth_Tx.TLF01_details.message_type) ;
			strcat( err_buf,mesbuf);
			ncdci_log_message( 2,1, err_buf, "ncdci_send_file_request_to_host",1 );
		}
		ret_val = ncdci_send_message_to_host( err_buf,false );//praneeth added false as parameter
	}
	else
	{
		sprintf( err_buf,
				"Cannot process %s txn request to host. Unable to set timer.",
				Auth_Tx.TLF01_details.message_type );
	}

	if ( false == ret_val )
	{
		/* Since the txn request did not go to the
		* host, we need to initiate a response.
		*/
		ncdci_log_message( 1, 2, err_buf, "ncdci_send_request_to_host",1 );
		ret_val = ncdci_perform_standin_processing( DINERS_SYSTEM_ERROR,
													GS_TXN_DROPPED,
													err_buf );
		if ( false == ret_val )
		{
			ncdci_log_message( 1, 2, err_buf, "ncdci_send_request_to_host",1 );
		}
	}

	return;
}

/******************************************************************************
 *
 *  NAME:         ncdci2_send_response_to_host
 *
 *  DESCRIPTION:  This function sends a response to the host.  It gets the
 *                response code and other information from Auth_Tx.
 *
 *  INPUTS:       send_flag - Used to determine if a resp should go to host
 *                            Values:
 *                               DINERS_SEND_UNCONDITIONALLY = Always send
 *                               DINERS_SEND_IF_NOT_REVERSAL = Do not send reversals
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_sendfile_response_to_host( INT send_flag )
{
   CHAR  err_buf[100] = {0};
   BYTE  temp_tx_key = {0};
   INT buf_temp_len = 0;

   buf_temp_len = atoi(Auth_Tx.resp_data_len);
   if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
   {
		buflen = buf_temp_len;
		memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
		sprintf (err_buf, "Sending file response coming Diners Host response to Cadencie, message type :%s",
				Auth_Tx.TLF01_details.message_type) ;
		ncdci_log_message( 1, 1, err_buf, "ncdci_sendfile_response_to_host",1 );
		if ( false == ncdci_send_message_to_host(err_buf,false))
		{
			 ncdci_log_message(1, 2, err_buf, "ncdci_sendfile_response_to_host",1 );
		}
   }
   else
   {
	   sprintf (err_buf, "Error in sending Diners Host response to Cadencie, message type :%s",
	   					  Auth_Tx.TLF01_details.message_type) ;
	   			ncdci_log_message( 1, 1, err_buf, "ncdci_sendfile_response_to_host",1 );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_send_msg_to_authorizer
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
INT ncdci_send_msg_to_authorizer( BYTE msgtype )
{
   INT   ret_val = false;
   CHAR  err_buf[100] = {0};
   CHAR  Buffer[200] = {0};

#if BDOR_62_PREAUTH_VOID
	if(strncmp(Auth_Tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
	{
		ret_val = ncdci_build_and_send_request_message( msgtype,
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
			ncdci_log_message( 1, 2, Buffer, "ncdci_send_msg_to_authorizer",1 );
		}
	}
	else
#endif
	{
	ret_val = ncdci_build_and_send_request_message( msgtype,
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
      ncdci_log_message( 1, 3, Buffer, "ncdci_send_msg_to_authorizer",1 );
   }
	}

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_and_send_request_message
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
INT ncdci_build_and_send_request_message( BYTE     msgtype,
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
      if ( length > DINERS_MAX_HOST_MSG_SIZE )
         ret_code = false;

   else if ( length > DINERS_MAX_APPL_MSG_SIZE )
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
         ncdci_write_to_txn_file();
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
         ret_code = ncdci_send_msg( p_msg, que_name, err_msg );
         free(p_msg);
      }
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         ncdci_send_msg
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
INT ncdci_send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg )
{
   INT      ret_val = true;
   LONG     ret_code;
   CHAR     Buff[256] = {0};
   CHAR     pte_err_mesg[200] = {0};

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
 *  NAME:         ncdci_init_Auth_Tx
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *
 *  INPUTS:       Auth_Tx - (Global)Data structure containing transaction info.
 *
 *  OUTPUTS:      Auth_Tx - (Global)Structure is updated with new information.
 *
 *  RTRN VALUE:   False if invalid transaction key (tx_key), else true
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_init_Auth_Tx(void)
{
   INT    ret_val = true;
   CHAR   unique_tran_str_id[21] = {0};
   CHAR   date_str [ 9] = {0};
   CHAR   time_str [ 7] = {0};
   CHAR   time_date[25] = {0};
   CHAR   temp_unique_tran[15] = {0};
   INT    tid_len = 0;
   INT    unique_tran_str_id_len=0;

   CHAR   temp_dest_pin_format_keyblock[2] = "1";
   CHAR   temp_source_pin_format_keyblock[2] = "1";

   /* Get local time. */
   if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
	 	ptetime_get_timestamp( time_date );
	 	ncdci_get_date( time_date, date_str );
	 	ncdci_get_time( time_date, time_str );
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
      ncdci_create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
   }

   /* Populate the source encryption key and pin format from the NCF01 record */

   if(keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
   {
		memcpy(Auth_Tx.TLF01_details.source_pin_format, temp_source_pin_format_keyblock,1)  ;
		memcpy(Auth_Tx.TLF01_details.dest_pin_format, temp_dest_pin_format_keyblock,1) ;
		memcpy(Auth_Tx.future_use_2,Ncf01_Keyblock.iss_cwk_keyblock,99);
		memcpy(Auth_Tx.future_use_3,Ncf01_Keyblock.iss_cwk_keyblock +99,29);
   }
   memcpy( Auth_Tx.TLF01_details.source_key, Ncf01_I.cwk, sizeof(Ncf01_I.cwk) );

   strcpy( Auth_Tx.TLF01_details.source_pin_format, Ncf01_I.pin_type );

   /* Determine Transaction Type */
   ret_val = ncdci_determine_tx_key();

   /* store acquiring id for TLF01 */
	  strcpy(Auth_Tx.TLF01_details.acquiring_id,Auth_Tx.acq_inst_id_cd);

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_determine_tx_key
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_determine_tx_key( )
{
   INT   ret_val = true;
   CHAR  err_buf[100] = {0};
   CHAR  msgtype[5] = {0};
   CHAR  procode[7] = {0};

   memcpy( msgtype, Auth_Tx.TLF01_details.message_type,   DINERS_MESSAGE_TYPE_SIZE );
   memcpy( procode, Auth_Tx.TLF01_details.processing_code, DINERS_DE3_SIZE);

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
      if ( TranFlow == DINERS_SWITCHED_IN )
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
      ncdci_log_message( 1, 2, err_buf, "ncdci_determine_tx_key",1 );
      Auth_Tx.TLF01_details.tx_key = AUTH_INVALID;
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_set_tx_key_to_response
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
void ncdci_set_tx_key_to_response()
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

  Function:    ncdci_vBCDMove

  Description: This function will move data from thencdci_HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
     ncdci_HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      Abhishek Verma      6/30/98
******************************************************************************/
void ncdci_vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);

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
 *  NAME:         ncdci_vEBCDICMove
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen,i;
   INT                  ret_val = true;
   BYTE 				temp_bcd_len[5] = {0};
   BYTE                 temp_ascii_len[5]= {0};

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
   	   if(fieldid == 55)
		{
			memcpy(temp_bcd_len,ncdci_HOST_MESSAGE.IccData_len,pBF->bd_fieldlen);
			// convert EBCD to ascii
			for( i=0; i<pBF->bd_fieldlen; i++ )
		    {
		        ncdci_convert_to_ascii( temp_bcd_len, temp_ascii_len,pBF->bd_fieldlen );
		        
		    }
			movelen = atoi(temp_ascii_len);
			movelen += pBF->bd_fieldlen;
			memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen);

			reqidx = reqidx + movelen;   /* increment buffer pointer */
	   }	
	   else if(pBF->p_bd_pdata[0] != 0x00)
	   {
			movelen  = ncdci_get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
			movelen += pBF->bd_fieldlen;

			ncdci_convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
			reqidx += movelen;
	   }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end vEBCDICMove */



/******************************************************************************
 *
 *  NAME:         ncdci_EBCDICMove
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      ncdci_convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
      reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

} /* end EBCDICMove */


/******************************************************************************
 *
 *  NAME:         ncdci_RspEBCDICvMove
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in thencdci_HOST_MESSAGE data structure.  The length is
 *                also copied.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen = 0;
   INT                  movelen = 0;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4] = {0};

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      ncdci_convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi(temp_string_len) + FieldLen;

	  	if(fieldid == 55) //EMV data
		{
			 memcpy(pBF->p_bd_pdata,&ReqRsp_Buffer[reqidx],movelen);
			 reqidx += movelen;
		 }
		else if ( (reqidx + movelen) <= buflen )
       {
         /* Move the data, converting it from EBCDIC to ASCII. */
         ncdci_convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
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
 *  NAME:         ncdci_RspEBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in thencdci_HOST_MESSAGE data structure.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci_RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         ncdci_convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
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

  Function:    ncdci_RspMovevBCD

  Description: This function will move data from the raw ISO message to 
               thencdci_HOST_MESSAGE structure.  
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
     ncdci_HOST_MESSAGE (global)  - structure for the outgoing ISO message
  Return values:
      None
  Modified by:
      Abhishek Verma      6/30/98
******************************************************************************/
void ncdci_RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);

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
   ncdci_GenericMove
************************************************/
INT ncdci_GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
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

} /* end ncdci_GenericMove */


/*****************************************************************************

  Function:    ncdci_vDataMove

  Description: This function will move data from thencdci_HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
     ncdci_HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
 void ncdci_vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
  
     movelen  = ncdci_get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
	 memcpy(&ReqRsp_Buffer[reqidx],&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
     reqidx =reqidx + pBF->bd_fieldlen;

     memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[pBF->bd_fieldlen], movelen);

     reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end ncdci_vDataMove */



/*****************************************************************************

  Function:    ncdci_RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to thencdci_HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
     ncdci_HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
 void ncdci_RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
     
	 memcpy(&pBF->p_bd_pdata[0],&ReqRsp_Buffer[reqidx],pBF->bd_fieldlen);
	 reqidx =reqidx + pBF->bd_fieldlen;
	
  	 movelen = atoi(pBF->p_bd_pdata);
     memcpy(&pBF->p_bd_pdata[pBF->bd_fieldlen], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end ncdci_RspMovevData */


/***********************************************
   ncdci_RspMoveGeneric
************************************************/
INT ncdci_RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
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

} /* end ncdci_RspMoveGeneric */




/******************************************************************************
 *
 *  NAME:         ncdci_Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specifiedncdci_HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table( fieldid );
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
} /* end ncdci_Move_IfThere */


/******************************************************************************
 *
 *  NAME:         ncdci_Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved fromncdci_HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci_Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end ncdci_Move_Always */


/******************************************************************************
 *
 *  NAME:         ncdci_get_variable_length
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
int ncdci_get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i = 0;
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
 *  NAME:         ncdci_Find_Tran_Table
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
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
struct trandef *ncdci_Find_Tran_Table( pAUTH_TX p_auth_tx )
{
   INT  i = 0;            /* loop index  */
   BYTE trantype = 0;
   CHAR Buffer[100] = {0};

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
   ncdci_log_message( 1, 2, Buffer, "FIND_TRAN_TABLE" , 0);

   return NULL;
} /* end ncdci_Find_Tran_Table */   


/******************************************************************************
 *
 *  NAME:         ncdci_Find_Bitfield_Table
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
struct bitfield_data *ncdci_Find_Bitfield_Table( enum bitfields bitfield )
{
   INT      i = 0;                    /* loop index */
   CHAR     temp_str[60] = {0};

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */

   /* Field not found in table - log error and return NULL */
   sprintf(temp_str, "This bitfield not found in BFData, bitfield: %d",
                      bitfield);
   ncdci_log_message( 1, 2, temp_str, "ncdci_Find_Bitfield_Table",0 );
   return NULL;

} /* end ncdci_Find_Bitfield_Table */




/******************************************************************************
 *
 *  NAME:         ncdci_build_host_msg
 *
 *  DESCRIPTION:  This procedure copies the information from thencdci_HOST_MESSAGE
 *                structure into a character string.  The character string is
 *                then shipped to the host.
 *
 *  INPUTS:       Auth_Tx      - (Global)Transaction record
 *               ncdci_HOST_MESSAGE - (Global)Transaction data destined for the host
 *
 *  OUTPUTS:      err_buf - Text message in the event of an error
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_build_host_msg( pCHAR err_buf )
{
	struct trandef       *pTranDef;
	struct bitfield_data *pBF;
	INT                  bfidx = 0;     /* index to bitfields for this transaction */
	INT                  bitMapsize = 0;
	BYTE                 temp_len[5] = {0};
	INT			         total_tran_len = 0;

	memset(ReqRsp_Buffer, 0, DINERS_COMM_BUF_SZE);   /* zero out the buffer */

	/* Get pointer to transaction table */
	pTranDef = ncdci_Find_Tran_Table( &Auth_Tx );

	if ( NULL == pTranDef )
	{
		if ( TranFlow == DINERS_SWITCHED_OUT )
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
		/* Did not find TX type - so quit */
		return false;
	}

	/* point to bitmap     */
	bitmapidx = DINERS_TOTALL_MEG_LEN + DINERS_MESSAGE_TYPE_SIZE ;

	memcpy(&ReqRsp_Buffer[DINERS_TOTALL_MEG_LEN], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

	if (extendedbit == true)
	{
		bitMapsize = DINERS_EXTENDED_BITMAP_SIZE;
		ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
		BitFieldSize = DINERS_EXTENDED_BITFIELD_SIZE;
	}
	else
	{
		bitMapsize = DINERS_NORMAL_BITMAP_SIZE;
		BitFieldSize = DINERS_NORMAL_BITFIELD_SIZE;
	}

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
			pBF = ncdci_Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

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

			}
		}
		else
		{
			bfidx = BitFieldSize;
		}
	}

	/*Prepare the total message length in header*/
	// We are stroing response message after 1st 4 byte of ReqRsp_Buffer
	total_tran_len = reqidx - DINERS_TOTALL_MEG_LEN ;
	sprintf(temp_len,"%04d",total_tran_len);
	memcpy(&ReqRsp_Buffer[0],temp_len,DINERS_TOTALL_MEG_LEN);
	//AddETX chat at the end of the transaction
	//memcpy(&ReqRsp_Buffer[reqidx],DINERS_ETX_TRN_TEXT,DINERS_TOTALL_MEG_LEN);
	ReqRsp_Buffer[reqidx]=0x03;
	reqidx= reqidx + 1;

	buflen = reqidx;
	return( true );
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_host_structure
 *
 *  DESCRIPTION:  This function will populate thencdci_HOST_MESSAGE structure for
 *                a request or response message destined for the host.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:     ncdci_HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_build_host_structure( pCHAR err_buf )
{
	INT     ret_val = true;
	INT     result_code = 0;
	CHAR    err_buf1[100] = {0};
	CHAR    temp_message_type[5] = {0};
	INT     buf_temp_len = 0;
	extendedbit= false;

	memset( &ncdci_HOST_MESSAGE, 0x00, sizeof (ncdci_HOST_MESSAGE) );
	memcpy(temp_message_type, Auth_Tx.TLF01_details.message_type,4);

	if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
	{
		/* Msg type: 1420 */
		temp_message_type[1]= '4';
	}

	if ( TranFlow == DINERS_SWITCHED_OUT )
	{
		/* SWITCHED OUT Transactions  We are sending Request to Host*/
		if (temp_message_type[1] == '1')
		{
			if(temp_message_type[2] == '0')
			{
				/* Build fields necessary for 1100 REQUEST messages. */
				ncdci_build_request_field_2(  &result_code, err_buf );
				ncdci_build_request_field_3(  &result_code, err_buf );
				ncdci_build_request_field_4(  &result_code, err_buf );
				ncdci_build_request_field_7(  &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_23( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_35( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_40( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_45( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_52( &result_code, err_buf );
				ncdci_build_request_field_55( &result_code, err_buf );
				ncdci_build_request_field_56( &result_code, err_buf );
				ncdci_build_request_field_58( &result_code, err_buf );
				ncdci_build_request_field_59( &result_code, err_buf );
				ncdci_build_request_field_62( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_122(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
				ncdci_build_request_field_125(&result_code, err_buf );
			}

			else if(temp_message_type[2] == '2')
			{
				/* Build fields necessary for  1120 REQUEST messages. */
				ncdci_build_request_field_2(  &result_code, err_buf );
				ncdci_build_request_field_3(  &result_code, err_buf );
				ncdci_build_request_field_4(  &result_code, err_buf );
				ncdci_build_request_field_7(  &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_23( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_25( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_30( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_35( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_40( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_45( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_55( &result_code, err_buf );
				ncdci_build_request_field_56( &result_code, err_buf );
				ncdci_build_request_field_58( &result_code, err_buf );
				ncdci_build_request_field_62( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
				ncdci_build_request_field_124(&result_code, err_buf );
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
		else if (temp_message_type[1] == '2')
		{
			if(temp_message_type[2] == '0')
			{
				/* Build fields necessary for 1100 REQUEST messages. */
				ncdci_build_request_field_2(  &result_code, err_buf );
				ncdci_build_request_field_3(  &result_code, err_buf );
				ncdci_build_request_field_4(  &result_code, err_buf );
				ncdci_build_request_field_7(  &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_23( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_35( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_40( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_45( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_52( &result_code, err_buf );
				ncdci_build_request_field_55( &result_code, err_buf );
				ncdci_build_request_field_56( &result_code, err_buf );
				ncdci_build_request_field_59( &result_code, err_buf );
				ncdci_build_request_field_62( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_122(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
				ncdci_build_request_field_125(&result_code, err_buf );
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

		else if ( ncdci_is_reversal() )
		{
			/* Build fields necessary for 0420 POS reversal REQUEST messages. */
			ncdci_build_request_field_2(  &result_code, err_buf );
			ncdci_build_request_field_3(  &result_code, err_buf );
			ncdci_build_request_field_4(  &result_code, err_buf );
			ncdci_build_request_field_7(  &result_code, err_buf );
			ncdci_build_request_field_11( &result_code, err_buf );
			ncdci_build_request_field_12( &result_code, err_buf );
			ncdci_build_request_field_13( &result_code, err_buf );
			ncdci_build_request_field_14( &result_code, err_buf );
			ncdci_build_request_field_22( &result_code, err_buf );
			ncdci_build_request_field_23( &result_code, err_buf );
			ncdci_build_request_field_24( &result_code, err_buf );
			ncdci_build_request_field_25( &result_code, err_buf );
			ncdci_build_request_field_26( &result_code, err_buf );
			ncdci_build_request_field_30( &result_code, err_buf );
			ncdci_build_request_field_32( &result_code, err_buf );
			ncdci_build_request_field_33( &result_code, err_buf );
			ncdci_build_request_field_37( &result_code, err_buf );
			ncdci_build_request_field_38( &result_code, err_buf );
			ncdci_build_request_field_39( &result_code, err_buf );
			ncdci_build_request_field_40( &result_code, err_buf );
			ncdci_build_request_field_41( &result_code, err_buf );
			ncdci_build_request_field_42( &result_code, err_buf );
			ncdci_build_request_field_43( &result_code, err_buf );
			ncdci_build_request_field_44( &result_code, err_buf );
			ncdci_build_request_field_48( &result_code, err_buf );
			ncdci_build_request_field_49( &result_code, err_buf );
			ncdci_build_request_field_51( &result_code, err_buf );
			ncdci_build_request_field_55( &result_code, err_buf );
			ncdci_build_request_field_56( &result_code, err_buf );
			ncdci_build_request_field_58( &result_code, err_buf );
			ncdci_build_request_field_62( &result_code, err_buf );
			ncdci_build_request_field_64( &result_code, err_buf );
			ncdci_build_request_field_92( &result_code, err_buf );
			ncdci_build_request_field_93( &result_code, err_buf );
			ncdci_build_request_field_100(&result_code, err_buf );
			ncdci_build_request_field_123(&result_code, err_buf );
			ncdci_build_request_field_124( &result_code, err_buf );
			/* Need to make sure the RRN of a reversal is the
			* same as the RRN of the original transaction.
			* EB needs it logged into TLF01 same as original RRN.
			*/
			memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
					Auth_Tx.TLF01_details.orig_retrieval_ref_num,
					12 );
		}
		else if(temp_message_type[1] == '3')
		{
			if( temp_message_type[2] == '0'||
				temp_message_type[2] == '1')
			{
				/* Network message 1304 */
				ncdci_build_request_field_2( &result_code, err_buf );
				ncdci_build_request_field_3( &result_code, err_buf );
				ncdci_build_request_field_4( &result_code, err_buf );
				ncdci_build_request_field_7( &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_56( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_72( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_94( &result_code, err_buf );
				ncdci_build_request_field_100( &result_code, err_buf );
				ncdci_build_request_field_101( &result_code, err_buf );
			}
			else if(temp_message_type[2] == '2' ||
					temp_message_type[2] == '3')
			{
				/* Network message 1324 */
				ncdci_build_request_field_2( &result_code, err_buf );
				ncdci_build_request_field_3( &result_code, err_buf );
				ncdci_build_request_field_4( &result_code, err_buf );
				ncdci_build_request_field_7( &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_25( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_35( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_45( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_72( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_94( &result_code, err_buf );
				ncdci_build_request_field_100( &result_code, err_buf );
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
		else if(temp_message_type[1] == '8')
		{
			if(temp_message_type[2] == '0')
			{
				/* Network message 1804 */
				ncdci_build_request_field_7( &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_94( &result_code, err_buf );
				ncdci_build_request_field_96( &result_code, err_buf );
				ncdci_build_request_field_100( &result_code, err_buf );
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
		else
		{
			/* Unsupported message type.  Log an error. */
			ret_val = false;
			sprintf( err_buf,
			"Unsupported message type (%s).",
			Auth_Tx.TLF01_details.message_type );
			strcat( err_buf, "  Unable to build request message." );
		}

		/* Flag any errors that occurred while building the response fields. */
		if ( result_code == 1 || ret_val == false )
		{
			ret_val = false;
		}
	}

	else if ( TranFlow == DINERS_SWITCHED_IN )
	{
		/* Set the message type for DCI response. */
		if (AUTH_AUTHORIZATION_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_SALE_RESP_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		if (AUTH_PRE_AUTHORIZATION_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
			 strncpy(temp_message_type,
					 DINERS_SALE_RESP_MESSAGE,
					 DINERS_MESSAGE_TYPE_SIZE);
		}
		if (AUTH_CARD_VERIFICATION_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		 strncpy(temp_message_type,
				 DINERS_SALE_RESP_MESSAGE,
				 DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_SALE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_SALE_RESP_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if(AUTH_QUASI_CASH_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_SALE_RESP_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_SALES_COMPLETION_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_SALE_RESP_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_BALANCE_INQUIRY_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
			strncpy(temp_message_type,
					DINERS_SALE_RESP_MESSAGE,
					DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_CASH_ADVANCE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
			strncpy(temp_message_type,
					DINERS_SALE_RESP_MESSAGE,
					DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_ADVICE_RESP_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_REVERSAL_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key||
				AUTH_REVERSAL_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_REVERSAL_REQ_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
#if BDOR_62_PREAUTH_VOID
		else if (AUTH_VOID_PRE_AUTHORIZATION_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_VOID_PREAUTH_REQ_MESSAGE_TYPE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
#endif
		else if (AUTH_VOID_SALE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_REVERSAL_REQ_MESSAGE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_CARD_UPDATE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_FILE_UPDATE_RESP_MESSAGE_TYPE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_CARD_UPDATE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		{
		  strncpy(temp_message_type,
				  DINERS_FILE_UPDATE_RESP_MESSAGE_TYPE,
				  DINERS_MESSAGE_TYPE_SIZE);
		}
		else if (AUTH_REFUND_RESPONSE == Auth_Tx.TLF01_details.tx_key)
				{
				  strncpy(temp_message_type,
						  DINERS_SALE_RESP_MESSAGE,
						  DINERS_MESSAGE_TYPE_SIZE);
				}
		/* SWITCHED IN Transactions we are sending response*/
		if (temp_message_type[1] == '1')
		{
			if(temp_message_type[2] == '1')
			{
				/* Build fields necessary for 1100 RESPONSE messages. */
				ncdci_build_request_field_2(  &result_code, err_buf );
				ncdci_build_request_field_3(  &result_code, err_buf );
				ncdci_build_request_field_4(  &result_code, err_buf );
				ncdci_build_request_field_6(  &result_code, err_buf );
				ncdci_build_request_field_7(  &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_23( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_40( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_46( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_51( &result_code, err_buf );
				ncdci_build_request_field_54( &result_code, err_buf );
				ncdci_build_request_field_55( &result_code, err_buf );
				ncdci_build_request_field_58( &result_code, err_buf );
				ncdci_build_request_field_59( &result_code, err_buf );
				ncdci_build_request_field_62( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
				ncdci_build_request_field_124(&result_code, err_buf );
			}

			else if(temp_message_type[2] == '2' ||
					temp_message_type[2] == '3')
			{
				/* Build fields necessary for 1120 RESPONSE messages. */
				ncdci_build_request_field_2(  &result_code, err_buf );
				ncdci_build_request_field_3(  &result_code, err_buf );
				ncdci_build_request_field_4(  &result_code, err_buf );
				ncdci_build_request_field_7(  &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_23( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_25( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_30( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				/*ncdci_build_request_field_38( &result_code, err_buf );*/
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_40( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_46( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_51( &result_code, err_buf );
				ncdci_build_request_field_54( &result_code, err_buf );
				ncdci_build_request_field_55( &result_code, err_buf );
				ncdci_build_request_field_56( &result_code, err_buf );
				ncdci_build_request_field_58( &result_code, err_buf );
				ncdci_build_request_field_62( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
				ncdci_build_request_field_124(&result_code, err_buf );
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
		else if(temp_message_type[1] == '3')
		{
			if(temp_message_type[2] == '0' ||
			   temp_message_type[2] == '1')
			{
				/* Network message 1304 */
				ncdci_build_request_field_2( &result_code, err_buf );
				ncdci_build_request_field_3( &result_code, err_buf );
				ncdci_build_request_field_4( &result_code, err_buf );
				ncdci_build_request_field_7( &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_56( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_72( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_94( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
				ncdci_build_request_field_101( &result_code, err_buf );
			}
			else if(temp_message_type[2] == '2' ||
					temp_message_type[2] == '3')
			{
				/* Network message 1324 */
				ncdci_build_request_field_2( &result_code, err_buf );
				ncdci_build_request_field_3( &result_code, err_buf );
				ncdci_build_request_field_4( &result_code, err_buf );
				ncdci_build_request_field_7( &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_13( &result_code, err_buf );
				ncdci_build_request_field_14( &result_code, err_buf );
				ncdci_build_request_field_22( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_25( &result_code, err_buf );
				ncdci_build_request_field_26( &result_code, err_buf );
				ncdci_build_request_field_32( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_35( &result_code, err_buf );
				ncdci_build_request_field_38( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_41( &result_code, err_buf );
				ncdci_build_request_field_42( &result_code, err_buf );
				ncdci_build_request_field_43( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_45( &result_code, err_buf );
				ncdci_build_request_field_48( &result_code, err_buf );
				ncdci_build_request_field_49( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_72( &result_code, err_buf );
				ncdci_build_request_field_92( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_94( &result_code, err_buf );
				ncdci_build_request_field_100(&result_code, err_buf );
				ncdci_build_request_field_123(&result_code, err_buf );
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
		else if ( ncdci_is_reversal() )
		{
			/*  If Original txn process as STIP and later send into host.
			After sent host If ATP gets reversal txn for that.
			It should go as 420 msg - TF-Ajay 08-Jul-09 */
			if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED)==0)
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
				strcpy(Auth_Tx.TLF01_details.message_type, "0420" );
			}

			/* Build fields necessary for 0420  reversal RESPONSE messages. */
			ncdci_build_request_field_2(  &result_code, err_buf );
			ncdci_build_request_field_3(  &result_code, err_buf );
			ncdci_build_request_field_4(  &result_code, err_buf );
			ncdci_build_request_field_6(  &result_code, err_buf );
            ncdci_build_request_field_7(  &result_code, err_buf );
			ncdci_build_request_field_11( &result_code, err_buf );
			ncdci_build_request_field_12( &result_code, err_buf );
			ncdci_build_request_field_13( &result_code, err_buf );
			ncdci_build_request_field_14( &result_code, err_buf );
			ncdci_build_request_field_22( &result_code, err_buf );
			ncdci_build_request_field_23( &result_code, err_buf );
			ncdci_build_request_field_24( &result_code, err_buf );
			ncdci_build_request_field_25( &result_code, err_buf );
			ncdci_build_request_field_26( &result_code, err_buf );
			ncdci_build_request_field_30( &result_code, err_buf );
			ncdci_build_request_field_32( &result_code, err_buf );
			ncdci_build_request_field_33( &result_code, err_buf );
			ncdci_build_request_field_37( &result_code, err_buf );
			ncdci_build_request_field_38( &result_code, err_buf );
			ncdci_build_request_field_39( &result_code, err_buf );
			ncdci_build_request_field_40( &result_code, err_buf );
			ncdci_build_request_field_41( &result_code, err_buf );
			ncdci_build_request_field_42( &result_code, err_buf );
			ncdci_build_request_field_43( &result_code, err_buf );
			ncdci_build_request_field_44( &result_code, err_buf );
			ncdci_build_request_field_48( &result_code, err_buf );
			ncdci_build_request_field_49( &result_code, err_buf );
			ncdci_build_request_field_51( &result_code, err_buf );
			ncdci_build_request_field_55( &result_code, err_buf );
			ncdci_build_request_field_56( &result_code, err_buf );
			ncdci_build_request_field_58( &result_code, err_buf );
			ncdci_build_request_field_62( &result_code, err_buf );
			ncdci_build_request_field_64( &result_code, err_buf );
			ncdci_build_request_field_92( &result_code, err_buf );
			ncdci_build_request_field_93( &result_code, err_buf );
			ncdci_build_request_field_100(&result_code, err_buf );
			ncdci_build_request_field_123(&result_code, err_buf );
			ncdci_build_request_field_124( &result_code, err_buf );

			/* Need to make sure the RRN of a reversal is the
			* same as the RRN of the original transaction.
			* EB needs it logged into TLF01 same as original RRN.
			*/
			memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
					Auth_Tx.TLF01_details.orig_retrieval_ref_num,
					12 );
		}
		else if(temp_message_type[1] == '8')
		{
			if(temp_message_type[2] == '1')
			{
				/* Network message 1814 Response */
				ncdci_build_request_field_7(  &result_code, err_buf );
				ncdci_build_request_field_11( &result_code, err_buf );
				ncdci_build_request_field_12( &result_code, err_buf );
				ncdci_build_request_field_24( &result_code, err_buf );
				ncdci_build_request_field_33( &result_code, err_buf );
				ncdci_build_request_field_39( &result_code, err_buf );
				ncdci_build_request_field_44( &result_code, err_buf );
				ncdci_build_request_field_64( &result_code, err_buf );
				ncdci_build_request_field_93( &result_code, err_buf );
				ncdci_build_request_field_94( &result_code, err_buf );
				ncdci_build_request_field_96( &result_code, err_buf );
				ncdci_build_request_field_100( &result_code, err_buf );
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
		else
		{
			/* Unsupported message type.  Log an error. */
			ret_val = false;
			sprintf( err_buf,
			         "Unsupported message type (%s).",
					Auth_Tx.TLF01_details.message_type );
					strcat( err_buf, "  Unable to build request message." );
		}
		/* Flag any errors that occurred while building the respomse fields. */
		if ( result_code == 1 || ret_val == false )
		{
			if(true == ncdci_is_approved_by_Cadencie)
			{
				ret_val = true;
			}
			else
			{
				ret_val = false;
			}
		}
	}

	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         ncdci_parse_host_msg
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into thencdci_HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_parse_host_msg( pCHAR err_buf )
{
	struct bitfield_data *pBF;
	INT                   bfidx = 0;    /* Index to bitfields for this transaction */
	INT                   ret_val = true;
	INT                   BitFieldSize = 0;
	BYTE                  ReqRsp_Buffer_hex[17] = {0};
	BYTE                  bitmap = 0;
	INT                   i= 0;
	BYTE                  ErrorMsg[100] = {0};

	/* Initialize thencdci_HOST_MESSAGE structure. */
	memset(&ncdci_HOST_MESSAGE, 0x00, sizeof (ncdci_HOST_MESSAGE));

	/* Point to Message type*/
	reqidx =  DINERS_TOTALL_MEG_LEN ;

	/*Reterive message Type*/
	memcpy(ncdci_HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], DINERS_MESSAGE_TYPE_SIZE);

    /* Point to Bitmap postion*/
	reqidx = reqidx + DINERS_MESSAGE_TYPE_SIZE;
	bitmap = ReqRsp_Buffer[reqidx];
	bitmapidx = reqidx;

	/* Point past bitmap, but 1st check for extended bitmap. */

	if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
	{
		/* Extended     */
		reqidx = reqidx + DINERS_EXTENDED_BITMAP_SIZE;
		BitFieldSize = DINERS_EXTENDED_BITFIELD_SIZE;
	}
	else
	{
		reqidx = reqidx + DINERS_NORMAL_BITMAP_SIZE;
		BitFieldSize = DINERS_NORMAL_BITFIELD_SIZE;
	}

	/*------------------------------------------------------------------
	For every bitfield present in message ( except first bit ) . . .
	-------------------------------------------------------------------*/
	for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
	{
		if ( ReqRsp_Buffer[bitmapidx + BITTAB[bfidx].byte_no] &
			 BITTAB[bfidx].bit_mask )
		{
			/* Bit is on, field is there.... */
			pBF = ncdci_Find_Bitfield_Table(bfidx);

			if ( NULL == pBF )
			{
				sprintf( err_buf, "Unexpected bitfield (%d) in message from host.",
				bfidx );
				strcat( err_buf, " MsgType = " );
				strncat( err_buf,ncdci_HOST_MESSAGE.msgtype, 4 );
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
					strncat( err_buf,ncdci_HOST_MESSAGE.msgtype, 4 );
					ncdci_log_message( 1, 2, err_buf, "ncdci_parse_host_msg",0 );
					break;
				}
				else
				{
					sprintf( err_buf,
							"Unable to parse field %d in msg from host, MsgType: %4.4s",
							bfidx,ncdci_HOST_MESSAGE.msgtype );
					ncdci_log_message( 1, 2, err_buf, "ncdci_parse_host_msg",0 );
				}
			}
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_populate_auth_tx
 *
 *  DESCRIPTION:  This function transfers information from thencdci_HOST_MESSAGE
 *                structure that contains data from the host into Auth_Tx.
 *
 *  INPUTS:      ncdci_HOST_MESSAGE - (Global)Data from host
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *                Auth_Tx - (Global)Transaction data
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred, such as
 *                a numeric field is not numeric
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_populate_auth_tx( pCHAR err_buf )
{
	INT     ret_val = true;
	INT     result_code = 0;

	if ( TranFlow == DINERS_SWITCHED_IN )
	{
		/*----------------------------------*/
		/* This is a request from the host. */
		/*----------------------------------*/

		if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
						   DINERS_SALE_REQ_MESSAGE,
						   DINERS_MESSAGE_TYPE_SIZE))
		{
			/* Move fields necessary for 1100 request messages. */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_6(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_23( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_25( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_40( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_46( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_51( &result_code, err_buf );
			ncdci_move_field_52( &result_code, err_buf );
			ncdci_move_field_54( &result_code, err_buf );
			ncdci_move_field_55( &result_code, err_buf );
			ncdci_move_field_58( &result_code, err_buf );
			ncdci_move_field_59( &result_code, err_buf );
			ncdci_move_field_62( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
			ncdci_move_field_106(&result_code, err_buf );
			ncdci_move_field_122(&result_code, err_buf );
			ncdci_move_field_123(&result_code, err_buf );
			ncdci_move_field_124(&result_code, err_buf );
			ncdci_move_field_125(&result_code, err_buf );
		}

		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_ADVICE_REQ_MESSAGE,
								DINERS_MESSAGE_TYPE_SIZE))
		{
			/* Move fields necessary for 1120 Request messages. */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_6(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_23( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_25( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_30( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_40( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_46( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_51( &result_code, err_buf );
			ncdci_move_field_54( &result_code, err_buf );
			ncdci_move_field_55( &result_code, err_buf );
			ncdci_move_field_56( &result_code, err_buf );
			ncdci_move_field_58( &result_code, err_buf );
			ncdci_move_field_62( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
			ncdci_move_field_106(&result_code, err_buf );
			ncdci_move_field_122(&result_code, err_buf );
			ncdci_move_field_123(&result_code, err_buf );
			ncdci_move_field_124(&result_code, err_buf );
		}

		else if (0 == strncmp( Auth_Tx.TLF01_details.message_type,
							   DINERS_REVERSAL_REQ_MESSAGE,
							   DINERS_MESSAGE_TYPE_SIZE))
		{
			/* Move fields necessary for 1420 Request */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_6(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_23( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_25( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_30( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_40( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_51( &result_code, err_buf );
			ncdci_move_field_55( &result_code, err_buf );
			ncdci_move_field_56( &result_code, err_buf );
			ncdci_move_field_58( &result_code, err_buf );
			ncdci_move_field_62( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
			ncdci_move_field_106(&result_code, err_buf );
			ncdci_move_field_122(&result_code, err_buf );
			ncdci_move_field_123(&result_code, err_buf );
			ncdci_move_field_124(&result_code, err_buf );
		}

		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
							    DINERS_NETWORK_REQ_MESSAGE,
							    DINERS_MESSAGE_TYPE_SIZE ))
		{
			/* Move fields of 1804 Network REQUEST messages. */
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_94( &result_code, err_buf );
			ncdci_move_field_96( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
		}
		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_FILE_UPDATE_REQ_MESSAGE_TYPE,
								DINERS_MESSAGE_TYPE_SIZE ))
		{
			/* Move fields of 1304 Network REQUEST messages. */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_72( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_94( &result_code, err_buf );
			ncdci_move_field_100( &result_code, err_buf );
			ncdci_move_field_101(&result_code, err_buf );
		}
		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE,
								DINERS_MESSAGE_TYPE_SIZE ))
		{
			/* Move fields of 1304 Network REQUEST messages. */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_72( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_94( &result_code, err_buf );
			ncdci_move_field_100( &result_code, err_buf );
			ncdci_move_field_101(&result_code, err_buf );
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

		if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
						   DINERS_SALE_RESP_MESSAGE,
						   DINERS_MESSAGE_TYPE_SIZE))
		{
			/* Move fields necessary for 1110 response messages. */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_6(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_23( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_40( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_46( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_51( &result_code, err_buf );
			ncdci_move_field_54( &result_code, err_buf );
			ncdci_move_field_55( &result_code, err_buf );
			ncdci_move_field_58( &result_code, err_buf );
			ncdci_move_field_59( &result_code, err_buf );
			ncdci_move_field_62( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
			ncdci_move_field_123(&result_code, err_buf );
			ncdci_move_field_124(&result_code, err_buf );
		}

		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_ADVICE_RESP_MESSAGE,
								DINERS_MESSAGE_TYPE_SIZE))
		{
			/* Move fields necessary for 1120 Request messages. */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_6(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_23( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_25( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_30( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_40( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_46( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_51( &result_code, err_buf );
			ncdci_move_field_55( &result_code, err_buf );
			ncdci_move_field_56( &result_code, err_buf );
			ncdci_move_field_58( &result_code, err_buf );
			ncdci_move_field_62( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
			ncdci_move_field_123(&result_code, err_buf );
			ncdci_move_field_124(&result_code, err_buf );
		}

		else if (0 == strncmp( Auth_Tx.TLF01_details.message_type,
							   DINERS_REVERSAL_RESP_MESSAGE,
							   DINERS_MESSAGE_TYPE_SIZE))
		{
			/* Move fields necessary for 0420 Response */
			ncdci_move_field_2(  &result_code, err_buf );
			ncdci_move_field_3(  &result_code, err_buf );
			ncdci_move_field_4(  &result_code, err_buf );
			ncdci_move_field_6(  &result_code, err_buf );
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_23( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_25( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_30( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_37( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_40( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_51( &result_code, err_buf );
			ncdci_move_field_55( &result_code, err_buf );
			ncdci_move_field_56( &result_code, err_buf );
			ncdci_move_field_58( &result_code, err_buf );
			ncdci_move_field_62( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
			ncdci_move_field_123(&result_code, err_buf );
			ncdci_move_field_124(&result_code, err_buf );
		}

		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_NETWORK_RESP_MESSAGE,
								DINERS_MESSAGE_TYPE_SIZE ))
		{
			/* Move fields of 1804 Network REQUEST messages. */
			ncdci_move_field_7(  &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_94( &result_code, err_buf );
			ncdci_move_field_96( &result_code, err_buf );
			ncdci_move_field_100(&result_code, err_buf );
		}

		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_FILE_UPDATE_RESP_MESSAGE_TYPE,
								DINERS_MESSAGE_TYPE_SIZE ))
		{
			/* Move fields of 1314 FILE UPDATE Response messages. */
			ncdci_move_field_2( &result_code, err_buf );
			ncdci_move_field_3( &result_code, err_buf );
			ncdci_move_field_4( &result_code, err_buf );
			ncdci_move_field_7( &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_56( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_72( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_94( &result_code, err_buf );
			ncdci_move_field_100( &result_code, err_buf );
			ncdci_move_field_101( &result_code, err_buf );
		}
		else if ( 0 == strncmp( Auth_Tx.TLF01_details.message_type,
								DINERS_NEGATIVE_FILE_UPDATE_RESP_MESSAGE_TYPE,
								DINERS_MESSAGE_TYPE_SIZE ))
		{
			/* Move fields of 1334 Network RESPONSE messages. */
			ncdci_move_field_2( &result_code, err_buf );
			ncdci_move_field_3( &result_code, err_buf );
			ncdci_move_field_4( &result_code, err_buf );
			ncdci_move_field_7( &result_code, err_buf );
			ncdci_move_field_11( &result_code, err_buf );
			ncdci_move_field_12( &result_code, err_buf );
			ncdci_move_field_13( &result_code, err_buf );
			ncdci_move_field_14( &result_code, err_buf );
			ncdci_move_field_22( &result_code, err_buf );
			ncdci_move_field_24( &result_code, err_buf );
			ncdci_move_field_25( &result_code, err_buf );
			ncdci_move_field_26( &result_code, err_buf );
			ncdci_move_field_32( &result_code, err_buf );
			ncdci_move_field_33( &result_code, err_buf );
			ncdci_move_field_35( &result_code, err_buf );
			ncdci_move_field_38( &result_code, err_buf );
			ncdci_move_field_39( &result_code, err_buf );
			ncdci_move_field_41( &result_code, err_buf );
			ncdci_move_field_42( &result_code, err_buf );
			ncdci_move_field_43( &result_code, err_buf );
			ncdci_move_field_44( &result_code, err_buf );
			ncdci_move_field_45( &result_code, err_buf );
			ncdci_move_field_48( &result_code, err_buf );
			ncdci_move_field_49( &result_code, err_buf );
			ncdci_move_field_64( &result_code, err_buf );
			ncdci_move_field_72( &result_code, err_buf );
			ncdci_move_field_92( &result_code, err_buf );
			ncdci_move_field_93( &result_code, err_buf );
			ncdci_move_field_94( &result_code, err_buf );
			ncdci_move_field_100( &result_code, err_buf );
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

	/* Flag any errors that occurred while moving the fields. */
	if ( result_code == 1 )
	{
		ret_val = false;
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_parse_track2
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_parse_track2()
{
   INT    ret_val = true;
   INT    card_num_len = 0;
   INT    track2_idx = 0;
   pBYTE  field_separator = NULL_PTR;

   if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = Auth_Tx.TLF01_details.track2;
      for(track2_idx=0;track2_idx <= DINERS_MAX_DE2_SIZE;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( (track2_idx > DINERS_MAX_DE2_SIZE) || (*field_separator == 0x00) )
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
 *  NAME:         ncdci_remove_track2_end_sentinel
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_remove_track2_end_sentinel( pBYTE track2 )
{
   #define  END_SENTINEL  '?'

   INT  len = 0;

   /* Trim trailing spaces */
   ncdci_trim_trailing_spaces( track2 );

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
 *  NAME:         ncdci_build_response_msg
 *
 *  DESCRIPTION:  This function calls 2 functions: one to gather the response
 *                data from Auth_Tx or create it and place it into structure
 *               ncdci_HOST_MESSAGE, then another to take the data from HOST_
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_build_response_msg()
{
   INT   ret_val=true;
   CHAR  err_buf[100] = {0};

   if ( true == (ret_val = ncdci_build_host_structure(err_buf)) )
   {
      ret_val = ncdci_build_host_msg( err_buf );
   }

   /* Log any errors. */
   if ( ret_val == false )
      ncdci_log_message( 1, 2, err_buf, "build_response_message", 1 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_build_request_msg
 *
 *  DESCRIPTION:  This function calls 2 functions: one to gather the txn
 *                data from Auth_Tx or create it and place it into structure
 *               ncdci_HOST_MESSAGE, then another to take the data from HOST_
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_build_request_msg( pCHAR err_buf )
{
   INT   ret_val=true;

   if ( true == ncdci_build_host_structure(err_buf))
   {
      ret_val = ncdci_build_host_msg( err_buf );
   }
   else
   {
	   ret_val = false;
	   ncdci_log_message( 1, 2, err_buf, "ncdci_build_request_msg", 1 );
   }

   return( ret_val );
 
}

/******************************************************************************
 *
 *  NAME:         ncdci_create_stan
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
void ncdci_create_stan( pBYTE stan )
{
   INT   len = 0;
   BYTE  time_in_seconds[30] = {0};

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
 *  NAME:         ncdci_log_message
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
void ncdci_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
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

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   ncdci_Log_error_warning_to_File(text_message,func,sev,details);
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
 *  NAME:         ncdci_send_network_status_to_monitor
 *
 *  DESCRIPTION:  This procedure will send a message to the monitor to inform
 *                it of a change in host status.
 *
 *  INPUTS:       NetworkType - (Global) Issuer, Acquirer, or DINERS_BOTH
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_send_network_status_to_monitor()
{

   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   INT         ret_code = true;
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   CHAR        err_msg[100] = {0};
   XINT        ReturnCode = 0 ;
   int         nIndex = 0 ;
 
   /* Initialize structure to be sent to Monitor. */
   memset( &ncf01, 0, sizeof(NCF01) );

   /* Get the current state (online or offline) from shared memory. */
   ncdci_read_shared_mem_table( net_consec_tmouts_ctr,active_txns_ctr,
                          current_state, err_msg);

   /* Get Monitor connection information from shared memory. */
   memset (ConnectionId, 0, sizeof (ConnectionId)) ;
   ReturnCode = ReadMemMonitorTable (ConnectionId) ;
   if (ConnectionId[0] != 0)
   {
      if ( NetworkType == DINERS_ACQUIRER  ||  NetworkType == DINERS_BOTH )
      {
         strcpy( ncf01.primary_key.network_id,    AppName      );
         strcpy( ncf01.primary_key.network_type, "A"           );
         strcpy( ncf01.status,                   current_state );

         ret_code = ncdci_build_and_send_request_message( MT_SYS_MONITOR,
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
            ncdci_log_message( 2, 2, err_msg,
            						"ncdci_send_network_status_to_monitor", 0 );
         }
      }
      if ( ((NetworkType == DINERS_ISSUER) || (NetworkType == DINERS_BOTH)) && ret_code )
      {
         if ( NetworkType == DINERS_BOTH )
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

         ret_code = ncdci_build_and_send_request_message( MT_SYS_MONITOR,
                                                    ST1_NETWORK_UPDATE,
                                                    ST2_NONE, 
                                                    applnk_que_name, 
                                                    (pBYTE)&ncf01, 
                                                    Ncf01_Size,
                                                    0,
                                                    ConnectionId,
                                                    err_msg );
         if ( ret_code == false )
            ncdci_log_message( 2, 2, err_msg,
            						"ncdci_send_network_status_to_monitor",0 );
      }
   }
   else
   {
      ncdci_log_message( 2, 2, "Unable to get connection info for Monitor.",
    		  	  	  	  	  "ncdci_send_network_status_to_monitor",0 );
   }
   return( ret_code );

}  /* ncdci_send_network_status_to_monitor */

/******************************************************************************
 *
 *  NAME:         ncdci_send_message_to_host
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_send_message_to_host( pCHAR err_buf,INT mgt )
{
	INT   ret_val = false ;
	CHAR  buffer[100] = {0};
	CHAR  message[100] = {0};
	CHAR  host_tpdu_id [20] = {0};
	CHAR  gmt_time[20] = {0};
	INT      buf_temp_len=0;
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
	else if( 0 == strncmp(AppName,"ncdci3",6))
	{
		strncpy(host_tpdu_id,"ncdci3",4);
	}
	else
	{
		strncpy(host_tpdu_id,"ncdci",3);
	}
	if(true == ncdci_check_for_cadencieacq_transaction())
	{
		buf_temp_len = atoi(Auth_Tx.resp_data_len);
		if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
		{
			buflen = buf_temp_len;
			memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
		}
		sprintf(message, "Sending %s req to Host received from Vertexon", Auth_Tx.TLF01_details.message_type);
		ncdci_log_message(2, 1, message, "ncdci_send_message_to_host",0 );
	}
	ret_val = ncdci_build_and_send_request_message( MT_OUTGOING,
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
		ncdci_write_to_txn_file();

		/* If a request, need to increment number of txns
		* for which we have not received a response yet.
		*/
		if ( Auth_Tx.TLF01_details.tx_key < AUTH_INVALID )
		{
			/* This is a request. */
			if ( false == ncdci_increment_active_count(err_buf) )
				ncdci_log_message(2, 2, err_buf, "ncdci_send_message_to_host",0 );
		}

		/* Cycle the echo timer because traffic went out over the line. */
		if ( false == ncdci_reset_echo_timer(err_buf) )
		{
			ncdci_log_message(2, 2, err_buf, "ncdci_send_message_to_host",0 );
		}
	}
	return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         ncdci_pin_block_exists
 *
 *  DESCRIPTION:  This function determines if a pin block exists.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if pin exists, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_pin_block_exists()
{
   INT  ret_val = true;

   if ( 0x00 == Auth_Tx.TLF01_details.pin_block[0] &&
		0x00 == Auth_Tx.TLF01_details.pin_block[1] )
   {
      ret_val = false;
   }
   else if (ncdci_blank_string(Auth_Tx.TLF01_details.pin_block,
                         sizeof(Auth_Tx.TLF01_details.pin_block)-1) )
   {
      ret_val = false;
   }

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         ncdci_verify_host_status
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
INT  ncdci_verify_host_status( pCHAR err_buf )
{
   INT   ret_val = false;

   /* Make sure host is active. */
   ret_val = ncdci_read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
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

            ncdci_log_message( 1, 2, err_buf, "ncdci_verify_host_status", 0 );

            ret_val = false;
            if ( false == ncdci_set_host_state(OFFLINE,err_buf) )
               ncdci_log_message( 1, 3, err_buf, "ncdci_verify_host_status",0 );
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
            ncdci_log_message( 2, 2, err_buf, "ncdci_verify_host_status",0 );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_perform_error_processing
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_perform_error_processing( pCHAR resp_code )
{
   CHAR       date_str [ 9] = {0};
   CHAR       time_str [ 7] = {0};
   CHAR       time_date[25] = {0};

   /* Get local time. */
   ptetime_get_timestamp( time_date );
   ncdci_get_date( time_date, date_str );
   ncdci_get_time( time_date, time_str );

   /* Create RRN and store it in the auth_tx block. */
   if ( 0 == strcmp(NULL_STR, Auth_Tx.TLF01_details.retrieval_ref_num) )
   {
      ncdci_create_rrn( Auth_Tx.TLF01_details.retrieval_ref_num );
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

   (void)ncdci_send_msg_to_authorizer( MT_AUTH_RESPONSE );
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_perform_standin_processing
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err_buf )
{
   INT   ret_val = false;
   CHAR  time_date[25] = {0};

   /* Copy in response code and create an auth number if approved. */
   strcpy( Auth_Tx.TLF01_details.response_code, rsp_code );

   if ( 0 == strcmp(rsp_code, DINERS_ISO_APPROVED) )
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
            ncdci_get_time( time_date, Auth_Tx.TLF01_details.auth_number );
         }
      }
   }

   /* Change status to completed and store it in the auth_tx block */
   Auth_Tx.TLF01_details.general_status = gen_status;

   /* Set transaction key to be that of a response for the authorizer's sake */
   (void)ncdci_set_tx_key_to_response();

   if ( true == ncdci_check_for_cadencieacq_transaction())
   {
   	/* send repsonse directly to ncdci2
   	 * */
	   ncdci_transfer_response_msg_to_dci2();
	   return;
   }
   /* Send response to the authorizer */
   ret_val = ncdci_send_msg_to_authorizer( MT_AUTH_RESPONSE );
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
 *  NAME:         ncdci_is_reversal
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
INT ncdci_is_reversal()
{
   INT   ret_val = false;

   if ( Auth_Tx.TLF01_details.message_type[1] == '4' )
   {
      ret_val = true;
   }
   else if((Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE) || 
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM))
   {
	   ret_val = true;
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
   {
	   ret_val = true;
   }
   return( ret_val );
}

/*****************************************************************************

  Function:   ncdci_ascii_to_bin_with_length

  Description:
      Convert an ascii string to a binary string
  Author: 
      Abhishek Verma
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
void ncdci_ascii_to_bin_with_length( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
{
   BYTE temp_char;
   INT i = 0;
   INT j = 0;

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
 *  NAME:         ncdci_trim_trailing_spaces
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
void ncdci_trim_trailing_spaces( pBYTE p_string )
{
   INT   i = 0;
   INT   len = 0;

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
 *  NAME:         ncdci_write_to_txn_file
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
void ncdci_write_to_txn_file()
{
   INT  rc = 0;
   CHAR err_buf[256] = {0};

   /* Log transaction to trace file. */
   if ( Tracing == DINERS_TRACE_ON )
   {
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,buflen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = DINERS_TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         ncdci_log_message( 1, 3, err_buf, "ncdci_write_to_txn_file",0 );
         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         ncdci_log_message( 1, 3, err_buf, "ncdci_write_to_txn_file",0 );

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            ncdci_log_message( 2, 1, err_buf, "ncdci_write_to_txn_file",0 );
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_Rj_with_lead_zeros
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
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void ncdci_Rj_with_lead_zeros( pCHAR str, INT FieldLen )
{
   int i = 0;
   int dif = 0;
   int strlength = 0;

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

  Function:    ncdci_hhutil_process_host_request

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
BYTE ncdci_hhutil_process_host_request(pAUTH_TX p_auth_tx)
{
   CHAR           ErrorMsg[200] = {0};
   pPTE_MSG       p_msg_ins = NULL_PTR;

   NCF30          ncf30;
   CHAR           Buff[256] = {0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)] = {0};
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
		(AUTH_ADVICE == p_auth_tx->TLF01_details.tx_key) ||
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
			strcpy( Buff, "Reversal from ncdci");
			ncdci_log_message( 2, 1,Buff , "ncdci_hhutil_process_host_request",1 );
       }

	  else if( AUTH_PRE_AUTHORIZATION_CANCLE == p_auth_tx->TLF01_details.tx_key)
   	  {
			memset( Buff, 0x00, sizeof(Buff) );
			strcpy( Buff ,"PreAuth Cancel from ncdci" );
			ncdci_log_message( 1, 1,Buff , "ncdci_hhutil_process_host_request",1 );
	  }
      	  /* Must get the original transaction for reversals
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
	    	  ncdci_log_message( 1, 3,"Insufficient Memory for select ncf30 message" ,
	    			  	  	  	  	  "ncdci_hhutil_process_host_request",1 );
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
	         ncdci_log_message( 1, 3,Buff , "ncdci_hhutil_process_host_request",1 );
	         return( false );
	      }
	      return( true );
   	}
     
   else if (false == ncdci_hhutil_transfer_ctl_to_auth_app(p_auth_tx))
   {
	   ncdci_log_message( 1, 3, "Unable to send the request to the authorizor application",
			   	   	   	   	   	   "ncdci_hhutil_process_host_request",1 );
       return(false);
   }
   return(true);
}



/*****************************************************************************

  Function:    ncdci_hhutil_transfer_ctl_to_auth_app

  Description: This function will transfer the auth_tx to the authorizer.  
  Author: 
      Abhishek Verma
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN ncdci_hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_host_out = NULL_PTR;
   LONG   ret_code = false;
   CHAR	 Buffer[256] = {0};
   CHAR	 ErrorMsg[100] = {0};

     p_msg_host_out = ptemsg_build_msg (MT_AUTH_REQUEST, ST1_NONE, ST2_NONE, authorizer_que_name, application_que_name,
                                      (pBYTE)p_auth_tx, sizeof(AUTH_TX), 0);
   if ( NULL_PTR == p_msg_host_out )
	{
	   ncdci_log_message( 1, 3,"Insufficient Memory to build host message",
			   	   	   	   	   "ncdci_hhutil_transfer_ctl_to_auth_app" ,1);
       return(false);
	}
	ret_code = pteipc_send(p_msg_host_out, authorizer_que_name);

	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "hhutil_transfer_ctl: error on pteipc_send to que %s: %s", authorizer_que_name, ErrorMsg );
       ncdci_log_message( 1, 3,Buffer,"ncdci_hhutil_transfer_ctl_to_auth_app",1 );
       free( p_msg_host_out );
       return(false);
	}
	
   free( p_msg_host_out );
   return(true);
}


/*****************************************************************************

  Function:    ncdci_init_txn

  Description: This function initializes auth_tx for switched in transactions.

  Author:      Abhishek Verma

  Inputs:      None

  Outputs:     p_auth_tx - Initialized auth_tx

  Return values:
      Always returns true
******************************************************************************/
INT ncdci_init_txn( pAUTH_TX p_auth_tx )
{

   INT     tid_len = 0;
   INT     unique_tran_str_id_len=0;
   CHAR           	unique_tran_str_id[21] = {0};
   CHAR           	temp[17] = {0};
   CHAR  			temp_unique_tran[15] = {0};
   CHAR            	temp_dest_pin_format_keyblock[2] = "1";
   CHAR            	temp_source_pin_format_keyblock[2] = "1";

   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type     = FINANCIAL;

   strcpy(p_auth_tx->TLF01_details.handler_queue, application_que_name);

   strcpy(p_auth_tx->TLF01_details.dispensation.origin,"RO");

    /* Set local date and time. */
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ) )
   {
      genutil_format_date(temp);
      memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, DINERS_YYYYMMDD_LEN);
      memcpy( p_auth_tx->TLF01_details.time_hhmmss,
    		  temp + DINERS_YYYYMMDD_LEN,
			  DINERS_HHMMSS_LEN);
   }
   
   /* Create a unique transaction id for the new auth_tx record. */
   memset(unique_tran_str_id, 0x00, sizeof(unique_tran_str_id));
   ptetime_get_strtime(unique_tran_str_id);
   strncpy( p_auth_tx->TLF01_details.tran_start_time, unique_tran_str_id,16 );

   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0x00,
          sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
   if(tid_len == DINERS_DE41_SIZE)
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
      strncpy(p_auth_tx->TLF01_details.message_type,
    		  DINERS_ISO_AUTH_REQ_MESSAGE_TYPE,
			  DINERS_MESSAGE_TYPE_SIZE);
   }
   if (AUTH_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
   {
         strncpy(p_auth_tx->TLF01_details.message_type,
       		  DINERS_ISO_AUTH_REQ_MESSAGE_TYPE,
   			  DINERS_MESSAGE_TYPE_SIZE);
   }
   if (AUTH_CARD_VERIFICATION == p_auth_tx->TLF01_details.tx_key)
   {
	 strncpy(p_auth_tx->TLF01_details.message_type,
		  DINERS_ISO_AUTH_REQ_MESSAGE_TYPE,
		  DINERS_MESSAGE_TYPE_SIZE);
   }
   else if (AUTH_SALE == p_auth_tx->TLF01_details.tx_key)
   {
      strncpy(p_auth_tx->TLF01_details.message_type,
    		  DINERS_ISO_SALE_REQ_MESSAGE_TYPE,
			  DINERS_MESSAGE_TYPE_SIZE);
   }
   else if (AUTH_CASH_ADVANCE == p_auth_tx->TLF01_details.tx_key)
  	{
  		strncpy(p_auth_tx->TLF01_details.message_type,
  				DINERS_ISO_AUTH_REQ_MESSAGE_TYPE,
  			    DINERS_MESSAGE_TYPE_SIZE);
  	}
   else if(AUTH_QUASI_CASH == p_auth_tx->TLF01_details.tx_key)
   {
      strncpy(p_auth_tx->TLF01_details.message_type,
    		  DINERS_ISO_AUTH_REQ_MESSAGE_TYPE,
			  DINERS_MESSAGE_TYPE_SIZE);
   }
   else if (AUTH_SALES_COMPLETION == p_auth_tx->TLF01_details.tx_key)
   {
      strncpy(p_auth_tx->TLF01_details.message_type,
    		  DINERS_ISO_SALE_COMPLETION_MESSAGE_TYPE,
			  DINERS_MESSAGE_TYPE_SIZE);
   }
   else if (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key)
   {
      strncpy(p_auth_tx->TLF01_details.message_type,
    		  DINERS_ISO_REVESAL_REQ_MESSAGE_TYPE,
			  DINERS_MESSAGE_TYPE_SIZE);
   }
   else if (AUTH_LOGON == p_auth_tx->TLF01_details.tx_key)
   {
      strncpy(p_auth_tx->TLF01_details.message_type,
    		  DINERS_NETWORK_REQ_MESSAGE,
			  DINERS_MESSAGE_TYPE_SIZE);
   }
   if(keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
   {
		memcpy(Auth_Tx.TLF01_details.source_pin_format, temp_source_pin_format_keyblock,1)  ;
		memcpy(Auth_Tx.TLF01_details.dest_pin_format, temp_dest_pin_format_keyblock,1) ;

		memcpy(Auth_Tx.future_use_2,Ncf01_Keyblock.iss_cwk_keyblock,99);
		memcpy(Auth_Tx.future_use_3,Ncf01_Keyblock.iss_cwk_keyblock +99,29);
   }

   return( true );
}



/*****************************************************************************

  Function:    ncdci_process_logon_request

  Description: This function handles network management request messages.
               The host state in shared memory is updated accordingly.  No
               update is made for echoes.

  Author:      Abhishek Verma

  Inputs:      nmi_code  - Code to indicate logon, logoff, or echo.
               p_auth_tx - Transaction & originator information

  Outputs:     p_auth_tx - Updated transaction information (response code)

  Return values: Always returns true
									
******************************************************************************/
BOOLEAN ncdci_process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx)
{
	CHAR      info_buffer[100] = {0};
	BYTE      net_consec_tmouts_ctr[4] = {0};
	BYTE      active_txns_ctr[4] = {0};
	BYTE      host_state[2] = {0};
	INT       update_host_status = false;

	CHAR 	table_name[15] = {0};

	memset( table_name, 0x00, sizeof(table_name) );
	strcpy( table_name, AppName );
	strcat( table_name, "Table" );

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

	if ( 0 == strcmp(DINERS_NMI_LOGON, p_auth_tx->function_code) )
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
	else if ( 0 == strcmp(DINERS_NMI_LOGOFF, p_auth_tx->function_code) )
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
		(void)ncdci_send_network_status_to_monitor();
	}
	ncdci_log_message( 1, 1, info_buffer, "ncdci_process_logon_request", 0 );
	/* Prepare the response and sent it. */
	strcpy( p_auth_tx->TLF01_details.response_code, "00" );
	p_auth_tx->TLF01_details.tx_key =
	ncdci_match_request_to_response( p_auth_tx->TLF01_details.tx_key );
	(void)ncdci_incoming_other_host_response( p_auth_tx );
	return(true);
}


   
/*****************************************************************************

  Function:    ncdci_match_request_to_response

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
      Abhishek Verma      6/30/98
******************************************************************************/
BYTE ncdci_match_request_to_response(BYTE request)
{

   if ( request >= AUTH_INVALID )
      return( request );

   switch (request)
   {
      case AUTH_AUTHORIZATION:
	     return(AUTH_AUTHORIZATION_RESPONSE); 
        break;

      case AUTH_PRE_AUTHORIZATION:
    	  return (AUTH_PRE_AUTHORIZATION_RESPONSE);
    	  break;

      case AUTH_CARD_VERIFICATION:
		  return (AUTH_CARD_VERIFICATION_RESPONSE);
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
	  case AUTH_QUASI_CASH:
		  return (AUTH_QUASI_CASH_RESPONSE);
		  		 break;

	  default:
         return( request+100 ); /* Convert from request to a response. */
        break;

   }
}; /* ncdci_match_request_to_response */




/*****************************************************************************

  Function:    ncdci_incoming_other_host_response

  Description: This function will process an incoming response from another host
               that needs to go to the ncdci host.
  Author: 
      Abhishek Verma
  Inputs:
      p_auth_tx - a pointer to the auth_tx of the response message
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
INT ncdci_incoming_other_host_response(pAUTH_TX p_auth_tx)
{
   INT    ret_code = false;
   NCF30  ncf30;

   PRINT("ncdci_incoming_other_host_response\n");

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

      ret_code = ncdci_delete_orig_ncf30( ncf30, p_auth_tx );
      if ( false == ret_code )
      {
    	  ncdci_log_message( 1, 2, "Unable to delete reversal's original txn in NCF30",
    			  	  	  	  	  "ncdci_incoming_other_host_response",1 );
      }
   }

   
   /* Continue to send response to host. */
   return( ncdci_incoming_other_host_response_db_reply( ) );
}




/******************************************************************************
 *
 *  NAME:         ncdci_txn_is_blocked
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
 *                NOTE:  This function assumes a txn switched in from ncdci
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if transaction should be blocked (not allowed) else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   CHAR  BLOCKED_MCC[][5] = { "6012", "6051", "done" };

   INT   ret_val = false;
   INT   i = 0;
   CHAR  CC[9] = {0};  /* Country Code */

   /* Check if currency code is Phillippino Peso.
    * Reconciliation_cur_code contains the original value from DE49.
    */
   if ( 0 == strcmp(p_auth_tx->reconciliation_cur_code,DINERS_PESO_CURRENCY_CODE) )
      strcpy( CC, "DE49=608" );

   else if ( 0 == strcmp(p_auth_tx->country_code, DINERS_PESO_CURRENCY_CODE) )
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
                    "Blocked txn from ncdci: MCC=%s, %s, Stan=%s, RRN=%s",
                     p_auth_tx->TLF01_details.category_code, CC,
                     p_auth_tx->TLF01_details.sys_trace_audit_num,
                     p_auth_tx->TLF01_details.retrieval_ref_num );

            strcpy( p_auth_tx->TLF01_details.response_text,
                   "Blocked txn from ncdci" );

            break;
         }
         i++;
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         ncdci_get_originator_info
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info )
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
 *  NAME:         ncdci_incoming_other_host_response_db_reply
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_p_auth_tx->  This function picks
 *                up where 'ncdci_incoming_other_host_response' left off.  You come
 *                here after updating NCF21 and NCF30 if they required updating.
 *
 *  INPUTS:      None.
 *                
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_incoming_other_host_response_db_reply( )
{
   CHAR       temp[17] = {0};
   CHAR       err_mesg[200] = {0};
   CHAR       ncdci_response_code[4] = {0};
   CHAR       temp_amount[13] = {0};
   CHAR       temp_cur_code[4] = {0};
   CHAR       err_buf[100]  = {0};
   INT        total_int = 0;
   INT        buf_temp_len = 0;
   INT        reversal_int = 0;
   INT        actual_int = 0;
	  

   if (AUTH_REVERSAL_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
   {
      if (0 == strlen(Auth_Tx.forwarding_inst_id_cd))
      {
         /* Indicates POS reversal (not ATM reversal) */
         Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT_RESPONSE;
      }
   }
    if (AUTH_REVERSAL_RESPONSE== Auth_Tx.TLF01_details.tx_key)
   {
       /* Indicates POS reversal Advice response */
         Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT_RESPONSE;
   }
   if (AUTH_AUTHORIZATION_RESPONSE == Auth_Tx.TLF01_details.tx_key && 
   	strcmp(Auth_Tx.TLF01_details.message_type,DINERS_ISO_SALE_REQ_MESSAGE_TYPE)==0)
   {
       /* Indicates POS reversal Advice response */
         Auth_Tx.TLF01_details.tx_key = AUTH_SALE_RESPONSE;
   }
	   strncpy(temp_cur_code, Auth_Tx.TLF01_details.currency_code,
			   sizeof(Auth_Tx.TLF01_details.currency_code));
	   strncpy(temp_amount, Auth_Tx.TLF01_details.total_amount,
			   sizeof(Auth_Tx.TLF01_details.total_amount));

	if (strlen(Auth_Tx.reconciliation_cur_code) > 0)
	{
	   strncpy(Auth_Tx.TLF01_details.currency_code,
			   Auth_Tx.reconciliation_cur_code,
			   sizeof(Auth_Tx.TLF01_details.currency_code));
	}
	if (strlen (Auth_Tx.reconciliation_amount)> 0)
	{
	   strncpy(Auth_Tx.TLF01_details.total_amount,
			   Auth_Tx.reconciliation_amount,
			   sizeof(Auth_Tx.TLF01_details.total_amount));
	}

   
   ncdci_build_host_structure(err_buf);
   
   if (true == ncdci_build_host_msg(err_buf))
   {
   	 /* Forward Msg coming from Cadience HOST */
		 if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0 &&
		 	(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncdci2",6) )
	  		)
		{
			buf_temp_len = atoi(Auth_Tx.resp_data_len);
			if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
			{	buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,Auth_Tx.resp_buffer,buflen);
				ncdci_log_message( 2, 1, "Sending Cadencie response to ncdci Host",
									"ncdci_incoming_other_host_response_db_reply",1 );
			}
		 }
      		ncdci_hhutil_send_host_response(&Auth_Tx);
     }
   else
   {
	   ncdci_log_message( 1, 3, "Error building response for the host",
			   	   	   	   	   "ncdci_incoming_other_host_response_db_reply",1 );
       return(false);
   }
   
	strncpy(Auth_Tx.TLF01_details.currency_code,temp_cur_code,
		   sizeof(Auth_Tx.TLF01_details.currency_code));
   	strncpy(Auth_Tx.TLF01_details.total_amount,temp_amount,
		   sizeof(Auth_Tx.TLF01_details.total_amount));
	

   /* If ATM reversal */
   if (AUTH_REVERSAL_ADVICE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
   {
      reversal_int = atoi(Auth_Tx.TLF01_details.reversal_amount);
      actual_int = atoi(Auth_Tx.TLF01_details.actual_amount);
      total_int = reversal_int + actual_int;
      itoa(total_int, Auth_Tx.TLF01_details.total_amount,10);
      ncdci_Rj_with_lead_zeros(Auth_Tx.TLF01_details.total_amount, 12);
      strcpy(Auth_Tx.TLF01_details.currency_code, DINERS_PESO_CURRENCY_CODE);
   }

   /* If POS reversal */
   if (AUTH_REVERSAL_REPEAT_RESPONSE == Auth_Tx.TLF01_details.tx_key)
   {
      /* Make it look like a regular reversal to the DB */
      Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   }

   if (AUTH_LOGON_RESPONSE == Auth_Tx.TLF01_details.tx_key)
   {
      return(true);
   }

   ncdci_insert_tlf01(&Auth_Tx );
   return(true);
}

/*****************************************************************************

  Function:    ncdci_get_tran_type_2

  Description: This function will get the transaction type using thencdci_HOST_MESSAGE
               structure
  Author: 
      unknown
  Inputs:
      tx_key - the current transaction type
     ncdci_HOST_MESSAGE (global) - the request buffer containing the message from the host
  Outputs:
      None
  Return values:
      BYTE - the calculated transaction type
  Modified by:
      FArad     8/13/98
******************************************************************************/
BYTE  ncdci_get_tran_type_2(BYTE tx_key)
{
	CHAR Errorbuf[100] = {0};

	/* Check for an Authorization request 1100 */
	if (('1' ==ncdci_HOST_MESSAGE.msgtype [1] ) && ('0' ==ncdci_HOST_MESSAGE.msgtype [2])) 
	{
		if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
						   DINERS_DE3_PROC_CODE_ISO_AUTH_AUTHORIZATION_VAL,
						   DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_AUTHORIZATION);
		}

		else if ( (0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_BAL_INQ_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) ) ||
				(0 == strncmp(  ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_BALANCE_INQ,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) ))

		{   
			return(AUTH_BALANCE_INQUIRY);
		}

		//AUTH Void
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return (AUTH_VOID_SALE);
		}

		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_CASH_ADV_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_CASH_ADVANCE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_QUASI_CASH_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_QUASI_CASH);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_CARD_ACC_VERIFICATION_REQ,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_CARD_VERIFICATION);
		}
		else if(0 == strncmp( ncdci_HOST_MESSAGE.proccode,
				DINERS_DE3_PROC_CODE_REFUND_REQ,
				DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ))
		{
			return(AUTH_REFUND);
		}
	}
	
	/*  Check for an Authorization response 1110*/
	else if ( ('1' ==ncdci_HOST_MESSAGE.msgtype [1]) && ('1' ==ncdci_HOST_MESSAGE.msgtype [2]) )
	{
		/* now verify the procesing code to know what kind */
		if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
							DINERS_DE3_PROC_CODE_ISO_AUTH_AUTHORIZATION_VAL,
							DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_AUTHORIZATION_RESPONSE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{	
			return(AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE);
		}
		else if ( (0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_BAL_INQ_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) ) ||
				(0 == strncmp(  ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_BALANCE_INQ,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) ))
		{
			return(AUTH_BALANCE_INQUIRY_RESPONSE);
		}
		
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_CASH_ADV_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_CASH_ADVANCE_RESPONSE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_QUASI_CASH_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_QUASI_CASH_RESPONSE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_CARD_ACC_VERIFICATION_REQ,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_CARD_VERIFICATION);
		}
		else if(0 == strncmp( ncdci_HOST_MESSAGE.proccode,
				DINERS_DE3_PROC_CODE_REFUND_REQ,
				DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ))
		{
			return(AUTH_REFUND_RESPONSE);
		}
	}

	/* Check for an Purchage request from another host   */
	else if (('2' == ncdci_HOST_MESSAGE.msgtype [1] ) && ('0' ==ncdci_HOST_MESSAGE.msgtype [2]))
	{
		if ( 0 == strncmp(  ncdci_HOST_MESSAGE.proccode,
							DINERS_DE3_PROC_CODE_ISO_AUTH_SALE_VAL,
							DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_SALE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_BAL_INQ_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{	
			return(AUTH_BALANCE_INQUIRY);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_CASH_ADV_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_CASH_ADVANCE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_VOID_SALE_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_VOID_SALE);
		}
		else if ( 0 == strncmp( ncdci_HOST_MESSAGE.proccode,
								DINERS_DE3_PROC_CODE_ISO_AUTH_QUASI_CASH_VAL,
								DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN ) )
		{
			return(AUTH_QUASI_CASH);
		}
	}

	/* Check for a Reversal request from another host */
	else if ( 0 == strncmp( ncdci_HOST_MESSAGE.msgtype,
							DINERS_ISO_REVESAL_REQ_MESSAGE_TYPE,
							DINERS_MESSAGE_TYPE_SIZE))
	{
		return( AUTH_REVERSAL );
	}
	else if ( 0 == strncmp( ncdci_HOST_MESSAGE.msgtype,
							DINERS_REVERSAL_REQ_MESSAGE,
							DINERS_MESSAGE_TYPE_SIZE))
	{
			return( AUTH_REVERSAL_ADVICE );
	}

	/* Reversal response */
	else if ( 0 == strncmp( ncdci_HOST_MESSAGE.msgtype,
							DINERS_ISO_REVESAL_RESP_MESSAGE_TYPE,
							DINERS_MESSAGE_TYPE_SIZE))
	{
		return( AUTH_REVERSAL_RESPONSE);
	}	
	else if ( 0 == strncmp( ncdci_HOST_MESSAGE.msgtype,
							DINERS_REVERSAL_RESP_MESSAGE,
							DINERS_MESSAGE_TYPE_SIZE))
	{
			return( AUTH_REVERSAL_ADVICE_RESPONSE);
	}
		
    /*Advince Message */
	else if ( 0 == strncmp( ncdci_HOST_MESSAGE.msgtype,
							DINERS_ADVICE_REQ_MESSAGE,
							DINERS_MESSAGE_TYPE_SIZE))
	{
		return( AUTH_ADVICE );
	}	
	else if ( 0 == strncmp( ncdci_HOST_MESSAGE.msgtype,
							DINERS_ADVICE_RESP_MESSAGE,
							DINERS_MESSAGE_TYPE_SIZE))
	{
		return( AUTH_ADVICE_RESPONSE );
	}
	else if ( (0 == strncmp(ncdci_HOST_MESSAGE.msgtype,
							DINERS_FILE_UPDATE_REQ_MESSAGE_TYPE,
							DINERS_MESSAGE_TYPE_SIZE)) ||
			  (0 == strncmp(ncdci_HOST_MESSAGE.msgtype,
					  	  	DINERS_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE,
							DINERS_MESSAGE_TYPE_SIZE)))
	{
			return( AUTH_CARD_UPDATE );
	}
	else if ( (0 == strncmp(ncdci_HOST_MESSAGE.msgtype,
							DINERS_FILE_UPDATE_RESP_MESSAGE_TYPE,
							DINERS_MESSAGE_TYPE_SIZE)) ||
			  (0 == strncmp(ncdci_HOST_MESSAGE.msgtype,
					  	  	DINERS_NEGATIVE_FILE_UPDATE_RESP_MESSAGE_TYPE,
							DINERS_MESSAGE_TYPE_SIZE)))
	{
			return( AUTH_CARD_UPDATE_RESPONSE );
	}

	/* Check for a network message message type*/
	else if ( ('8' ==ncdci_HOST_MESSAGE.msgtype [1]) && ('0' ==ncdci_HOST_MESSAGE.msgtype [2]))
	{
		return( AUTH_PIN_CHANGE );
	}
	else if ( ('8' ==ncdci_HOST_MESSAGE.msgtype [1]) && ('1' ==ncdci_HOST_MESSAGE.msgtype [2]))
	{
		return( AUTH_LOGON );
	}

	/* or a transaction that the financial server does not understand */
	else
	{

		sprintf(Errorbuf,"Tran Type not supported for Message type : & and Processing code : %s",
		ncdci_HOST_MESSAGE.msgtype ,ncdci_HOST_MESSAGE.proccode);
		ncdci_log_message( 1, 3, Errorbuf, "ncdci_insert_tlf01",1 );
		return(AUTH_INVALID);
	}
}

/*****************************************************************************

  Function:     ncdci_insert_tlf01()

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
INT ncdci_insert_tlf01(pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)] = {0};
   LONG       ret_code = false;
   CHAR       Buff[256] = {0};
   CHAR       err_mesg[200] = {0};

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
	   ncdci_log_message( 1, 3, "Insufficient Memory to build insert TLF01 message",
			   	   	   	   "ncdci_insert_tlf01",1 );
       return(false);
   }
   ret_code = pteipc_send(p_msg_ins, updateds_que_name); 
   if(ret_code < 0)
   {
      pteipc_get_errormsg(ret_code, err_mesg);
      sprintf(Buff, "failed transmit to que: %s. %s",
              updateds_que_name, err_mesg);
      ncdci_log_message( 1, 3, Buff, "ncdci_insert_tlf01",1 );
      free(p_msg_ins);
      return(false);
   }
   free(p_msg_ins);
   return(true);
}


/*****************************************************************************

  Function:    ncdci_hhutil_send_net_mngmnt_request

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
INT ncdci_hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR ncdci_response_code[4])
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code = false;
   CHAR	     ErrorMsg[100] = {0};
   CHAR	     Buffer[256] = {0};
  
   ncdci_build_host_structure(ErrorMsg);
   if ( false == ncdci_build_host_msg(ErrorMsg) )
   {
	   ncdci_log_message( 1, 3, "Invalid bit field: building network management request msg.",
			   	   	   	   "ncdci_hhutil_send_net_mngmnt_request",1 );
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
       
	   
	(void)ncdci_set_originator_info( p_auth_tx, p_msg_host_out );
     
   if ( NULL_PTR == p_msg_host_out )
   {
	   ncdci_log_message( 1, 3, "Insufficient Memory to build host message",
			   	   	   	   "ncdci_hhutil_send_net_mngmnt_request",1 );
       return(false);
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );

   free( p_msg_host_out );

 	if( ret_code < 0 )
	{
 		pteipc_get_errormsg( ret_code, ErrorMsg );
 		sprintf( Buffer, "pteipc error sending to que %s: %s",
               dialog_que_name, ErrorMsg );
 		ncdci_log_message( 1, 3, Buffer, "ncdci_hhutil_send_net_mngmnt_request",1 );
 		return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      ncdci_write_to_txn_file();
   }
   return(true);
} /* ncdci_hhutil_send_net_mngmnt_request */


/*****************************************************************************

  Function:    ncdci_hhutil_send_net_mngmnt_response

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
INT ncdci_hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code = false;
   CHAR	     ErrorMsg[100] = {0};
   CHAR	     Buffer[256] = {0};

   ncdci_build_host_structure(ErrorMsg);
   if ( false == ncdci_build_host_msg(ErrorMsg) )
   {
	   ncdci_log_message( 1, 3, "Invalid bit field: building network management response msg",
			   	   	   	   	   "ncdci_hhutil_send_net_mngmnt_response",1 );
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
       
	   
	(void)ncdci_set_originator_info( p_auth_tx, p_msg_host_out );
     
   if ( NULL_PTR == p_msg_host_out )
   {
	   ncdci_log_message( 1, 3, "Insufficient Memory to build host message",
			   	   	   	   "ncdci_hhutil_send_net_mngmnt_response",0 );
       return(false);
	}
   
   ret_code = pteipc_send(p_msg_host_out, p_auth_tx->TLF01_details.originator_queue );

   free (p_msg_host_out);

 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s",
               p_auth_tx->TLF01_details.originator_queue, ErrorMsg );
      ncdci_log_message( 1, 3, Buffer, "ncdci_hhutil_send_net_mngmnt_response",0 );
      free(p_msg_host_out);
      return(false);
	}
   else
   {
   		 sprintf(Buffer,"Sending network managemnt response Message type: %s  and NMI Code:  %s to host",
	 				p_auth_tx->TLF01_details.message_type,p_auth_tx->function_code);
   		ncdci_log_message( 2, 1, Buffer, "ncdci_hhutil_send_net_mngmnt_response",0);
   		/* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      ncdci_write_to_txn_file();
   }

 	//HCTS1-A
 	/* Cycle the echo timer because traffic went out over the line. */
	if ( false == ncdci_reset_echo_timer(Buffer) )
	{
		ncdci_log_message(2, 2, Buffer, "ncdci_hhutil_send_net_mngmnt_response",0 );
	}

   free(p_msg_host_out);
   return(true);
} /* ncdci_hhutil_send_net_mngmnt_response */


/******************************************************************************
 *
 *  NAME:         ncdci_update_ncf01
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/

void ncdci_update_ncf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)] = {0};
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
	   ncdci_log_message( 1, 3, "Insufficient Memory to build insert NCF01 message",
			   	   	   	   	   "ncdci_hhutil_send_net_mngmnt_response" , 1);
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = ncdci_send_msg( p_msg, netds_que_name, p_auth_tx );
      free( p_msg );
   }
}


/******************************************************************************
 *
 *  NAME:         ncdci_update_ncf01
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/

void ncdci_update_ncf01_keyblock( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(Ncf01_Keyblock)] = {0};
   BOOLEAN         ret_code = false;


   /* Build message to get host specific data from data server. */
   memset( buffer1, 0x00,     sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_Keyblock, NCF01_KEYBLOCK01_SIZE );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             netds_que_name,
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1),
							 NCF01_KEYBLOCK01_DATA );
   if ( NULL_PTR == p_msg  )
   {
	   ncdci_log_message( 1, 3, "Insufficient Memory to build insert NCF01 message",
			   	   	   	   	   "ncdci_update_ncf01_keyblock" , 1);
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = ncdci_send_msg( p_msg, netds_que_name, p_auth_tx );
      free( p_msg );
   }
}

/*****************************************************************************

  Function:    ncdci_hhutil_send_host_response

  Description: This function will send the host response message out to the 
                host.
  Author: 
      Abhishek  Verma
  Inputs:
      p_auth_tx - the auth_tx structure, used to get the tpdu_id 
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/

BOOLEAN ncdci_hhutil_send_host_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_out;
   LONG   ret_code = false;
   CHAR	 Buffer[256] = {0};
   CHAR	 ErrorMsg[100] = {0};

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
	   ncdci_log_message( 1, 3, "Insufficient Memory to build host message",
			   	   	   	   "ncdci_hhutil_send_host_response",1 );
       return(false);
	}
   (void)ncdci_set_originator_info( p_auth_tx, p_msg_out );
	ret_code = pteipc_send(p_msg_out, p_auth_tx->TLF01_details.originator_queue);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s",
               p_auth_tx->TLF01_details.originator_queue, ErrorMsg );
      ncdci_log_message( 1, 3,Buffer , "ncdci_hhutil_send_host_response",1 );
      free( p_msg_out );
      return(false);
	}
   else
   {
   	  strcat(Buffer,"Response message Send to host for message type : ");
	  strcat (Buffer ,Auth_Tx.TLF01_details.message_type);
	  ncdci_log_message( 2, 3,Buffer , "ncdci_hhutil_send_host_response",1 );
   	  /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

      /* Log transaction to trace file. */
      ncdci_write_to_txn_file();
   }
   free( p_msg_out );
   return(true);
}

void ncdci_send_update_Key_message_to_ncdci_ACQ( )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};
  

   strncpy(queue,"ncdciA",6);

   p_msg = ptemsg_build_msg( MT_NSP_DINERS_KEY_RESPONSE,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             queue, 
                             application_que_name,
                             NULL,
                             NULL, 
							 DINERS_KEY_UPDATE );
   if ( NULL_PTR == p_msg  )
   {
	   ncdci_log_message( 1, 3,"Insufficient Memory to build insert ncdci2 message" ,
			   	   	   	   "ncdci_send_update_Key_message_to_ncdci_ACQ",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = ncdci_send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

void ncdci_send_update_Key_message_to_ncdci_ACQ_keyblock( )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};


   strncpy(queue,"ncdciA",6);

   p_msg = ptemsg_build_msg( MT_NSP_DINERS_KEY_RESPONSE_KEYBLOCK,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             queue,
                             application_que_name,
                             NULL,
                             NULL,
							 DINERS_KEY_UPDATE_KEYBLOCK );
   if ( NULL_PTR == p_msg  )
   {
	   ncdci_log_message( 1, 3,"Insufficient Memory to build insert ncdci2 message" ,
			   	   	   	   "ncdci_send_update_Key_message_to_ncdci_ACQ",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = ncdci_send_msg( p_msg, queue, Error );
      free( p_msg );
   }
}

INT ncdci_get_service_code( pCHAR service_code, pCHAR error_msg )
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
		for( track2_idx=0; track2_idx <= DINERS_MAX_DE2_SIZE;
		track2_idx++,field_separator++)
		{
			if ( !isdigit( *field_separator ) )
				break;
		}

		if ( track2_idx > DINERS_MAX_DE2_SIZE )
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
		if((field_separator==NULL) ||((((int)field_separator) - ((int)Auth_Tx.TLF01_details.track1)) > DINERS_MAX_DE2_SIZE))
		{
			strcpy( error_msg,"Unable to parse track1.  Cannot find field separator.");
		}
		else
		{
			field_separator1=strchr(field_separator+1,'^');

			if((field_separator1==NULL )||((field_separator1-(field_separator+1))>DINERS_MAX_DE2_SIZE))
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
/*****************************************************************************

  Function:    ncdci_blank_or_null_string

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

BOOLEAN ncdci_blank_or_null_string(pCHAR astr, INT max_len)
{
   int i = 0;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}


BOOLEAN ncdci_is_issuing_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   ncdci_issuing_link_name,
					   DINERS_ISSUING_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN ncdci_is_issuing_3DS2_transaction(void)
{
	if (true == ncdci_is_issuing_transaction() &&
		0    != strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, "00", 2))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN ncdci_is_POS_Acq_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
			   	   	   pos_handler_module_name,
					   DINERS_POS_HANDLER_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN ncdci_is_voice_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   voice_handler_que_name,
					   DINERS_VOICE_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}


BOOLEAN ncdci_is_sbatch_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
				       sbatch_handler_que_name,
					   DINERS_SBATCH_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN ncdci_is_approved_by_Cadencie(void)
{
	INT buf_temp_len = 0;

	buf_temp_len = atoi(Auth_Tx.resp_data_len);

    if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
    {
    	if(  0 == strncmp(  Auth_Tx.TLF01_details.authorizing_host_queue,
    						cadencie_dci_queue_name,
							DINERS_CANDENCIE_LINK_QUEUE_NAME_LEN) )
		{
			return true;
		}
		else
		{
			return false;
		}
    }
}

INT ncdci_check_Asiapay_Ecommerce_transaction (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION,
					 DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT ncdci_check_RPS_Transaction(void)
{
	if( (0 == strncmp(Auth_Tx.industry_code,
					  DINERS_RPS_INDUSTRY_CODE,
					  DINERS_RPS_INDUSTRY_CODE_LEN )) ||
		((0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					  DINERS_DE25_RECURRING_TRANSACTION,
					  DINERS_DE25_RECURRING_TRANSACTION_INDICATOR_LEN ))&&
		 (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
				 	   DINERS_BDO_POS_ENTRY_MODE_FOR_RPS,
					   DINERS_BDO_POS_ENTRY_MODE_FOR_RPS_LEN ))))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT ncdci_check_Asiapay_Recurring (void)
{
	if( (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					  DINERS_DE25_RECURRING_TRANSACTION,
					  DINERS_DE25_RECURRING_TRANSACTION_INDICATOR_LEN ))&&
		 (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_FOR_RPS,
					   DINERS_BDO_POS_ENTRY_MODE_FOR_RPS_LEN )))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT ncdci_check_BDO_PAY (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_BDO_PAY_TRANSACTION,
					 DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT ncdci_check_emv_capability_device(void)
{
	/*BDO have discarded this functionality so always return fasle*/
	return false;
	if( 0 == strncmp(Auth_Tx.TLF01_details.terminal_type ,
					 DINERS_EMV_CAPABILITY_INDICATOR,
					 DINERS_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

// We will use this in txcntl module as well
BOOLEAN ncdci_check_if_valid_zero_amount_transaction(void)
{
	//Check if its Balance Inquiry transaction
	if( 0 == strncmp( Auth_Tx.TLF01_details.processing_code,
					  DINERS_DE3_PROC_CODE_BALANCE_INQ,
					  DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
	{
		return true;
	}
	//Check if its Card Account Verification transaction
	else if( 0 == strncmp(  Auth_Tx.TLF01_details.processing_code,
							DINERS_DE3_PROC_CODE_CARD_ACC_VERIFICATION_REQ,
							DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
	{
		return true;
	}
	// Check if its PIN Change transaction
	else if( 0 == strncmp(  Auth_Tx.TLF01_details.processing_code,
							DINERS_DE3_PROC_CODE_PIN_CHANGE,
							DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
	{
		return true;
	}
	//Check if its PIN unblock transaction
	else if( 0 == strncmp(  Auth_Tx.TLF01_details.processing_code,
							DINERS_DE3_PROC_CODE_PIN_UNBLOCK,
							DINERS_DE3_PROC_CODE_FIRST_2_DIGIT_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ncdci_build_pos_data_from_iso_pos_entry_mode(void)
{
	if(true == ncdci_is_Magstrip_pos_transaction())
	{
		ncdci_build_Magstrip_pos_data_from_iso_pos_entry_mode();
		return;
	}
	else if(true == ncdci_is_Chip_pos_transaction())
	{
		ncdci_build_Chip_pos_data_from_iso_pos_entry_mode();
		return;
	}
	else if(true == ncdci_is_Fallback_pos_transaction())
	{
		ncdci_build_Fallback_pos_data_from_iso_pos_entry_mode();
		return;
	}
	else if(true == ncdci_is_Contactless_Mastripe_pos_transaction())
	{
		ncdci_build_Contactless_Mastripe_pos_data_from_iso_pos_entry_mode();
		return;
	}
	else if(true == ncdci_is_Contactless_Chip_pos_transaction())
	{
		ncdci_build_Contactless_Chip_pos_data_from_iso_pos_entry_mode();
		return;
	}
	else if(true == ncdci_is_voice_transaction())
	{
		ncdci_build_voice_transaction_pos_data_from_iso_pos_entry_mode();
		return;
	}

#if BDOR_68_REQ_5_DCI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
	else if(true == ncdci_check_if_trx_is_dci_ecommerce_swiftpass_BDOPay_trx ())
	{
		ncdci_build_ecommerce_swiftpass_BDOPay_trx_pos_data_from_iso_pos_entry_mode();
    }
#endif
	else if(true == ncdci_is_Manual_pos_transaction())
	{
		ncdci_build_Manual_pos_data_from_iso_pos_entry_mode();
		return;
	}
#if BDOR_68_REQ_1_DCI
	/* MobilePOS-NFC Terminal for Tap2Phone Transactions*/
	else if(true == ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_with_pin())
	{
		ncdci_build_MobilePOS_NFC_Tap2Phone_trx_with_pin_pos_data_from_iso_pos_entry_mode();
	}
	else if(true == ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_without_pin())
	{
		ncdci_build_MobilePOS_NFC_Tap2Phone_trx_without_pin_pos_data_from_iso_pos_entry_mode();
	}
#endif

#if BDOR_68_REQ_3_DCI
	/*Swiftpass BDO Pay Credit Transactions With Pin*/
	else if(true == ncdci_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_with_pin())
	{
		ncdci_build_Swiftpass_BDO_Pay_credit_trx_with_pin_pos_data_from_iso_pos_entry_mode();
	}
	else if(true == ncdci_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_without_pin())
	{
		ncdci_build_Swiftpass_BDO_Pay_credit_trx_without_pin_pos_data_from_iso_pos_entry_mode();
	}
#endif
	else
	{
		/*its unknown POS data*/
		ncdci_build_unknow_pos_data_from_iso_pos_entry_mode();
		return;
	}

}

void ncdci_populate_iso_pos_entry_mode()
{
	if( true == ncdci_is_Card_present_pos_transaction() )
	{
		if(true == ncdci_is_Magstrip_pos_transaction())
		{
			ncdci_build_Magstrip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if(true == ncdci_is_Chip_pos_transaction())
		{
			ncdci_build_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if(true == ncdci_is_Fallback_pos_transaction())
		{
			ncdci_build_Fallback_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if(true == ncdci_is_Contactless_Mastripe_pos_transaction())
		{
			ncdci_build_Contactless_Mastripe_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if(true == ncdci_is_Contactless_Chip_pos_transaction())
		{
			ncdci_build_Contactless_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if(true == ncdci_is_Manual_pos_transaction())
		{
			ncdci_build_Manual_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if (true == ncdci_is_ECOM_transaction())
		{
			ncdci_build_Ecom_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else
		{
			//its unknow pos data
			ncdci_build_unknow_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
	}
	else
	{
		if (true == ncdci_is_ECOM_transaction())
		{
			ncdci_build_Ecom_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else if(true == ncdci_is_Manual_pos_transaction())
		{
			ncdci_build_Manual_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
		else
		{
			//its unknow pos data
			ncdci_build_unknow_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
		}
	}

	/* Check if transaction is with PIN */
	if(true == ncdci_is_pin_transaction())
	{
		strncat(Auth_Tx.TLF01_details.pos_entry_mode,
				DINERS_BDO_POS_ENTRY_MODE_WITH_PIN,
				DINERS_BDO_POS_ENTRY_MODE_PIN_LEN);
	}
	else
	{
		strncat( Auth_Tx.TLF01_details.pos_entry_mode,
				DINERS_BDO_POS_ENTRY_MODE_WITHOUT_PIN,
				DINERS_BDO_POS_ENTRY_MODE_PIN_LEN);
	}
	return;
}


BOOLEAN ncdci_is_pin_transaction(void)
{
	if( DINERS_DE22_POST_2_CARDHOLDER_AUTHENTICATION_CAPABILITY_PIN == Auth_Tx.TLF01_details.product_codes[16].amount[1])
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN ncdci_is_Card_present_pos_transaction(void)
{
	if( DINERS_DE22_POST_6_CARD_PRESENT == Auth_Tx.TLF01_details.product_codes[16].amount[5] )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN ncdci_is_Magstrip_pos_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_READ == Auth_Tx.TLF01_details.product_codes[16].amount[6] ||
			DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_FALLBACK == Auth_Tx.TLF01_details.product_codes[16].amount[6] ||
			DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_DATA_RFID == Auth_Tx.TLF01_details.product_codes[16].amount[6] )
		{
			if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP == Auth_Tx.TLF01_details.product_codes[16].amount[0] ||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL == Auth_Tx.TLF01_details.product_codes[16].amount[0]||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_ICC == Auth_Tx.TLF01_details.product_codes[16].amount[0]||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS == Auth_Tx.TLF01_details.product_codes[16].amount[0])
			{
				return true;
			}
		}
	}
	else if( (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
							 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
			 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
					 	 	 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP2,
							 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
			 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
					 	 	 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP3,
							 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
			 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
					 	 	 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP4,
							 DINERS_BDO_POS_ENTRY_MODE_LEN)))
	{
		return true;
	}
	return false;
}

BOOLEAN ncdci_is_Chip_pos_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_ICC == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
			if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_ICC == Auth_Tx.TLF01_details.product_codes[16].amount[0] ||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL == Auth_Tx.TLF01_details.product_codes[16].amount[0])
			{
				return true;
			}
		}
	}
	else if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN ncdci_is_Fallback_pos_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if ( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_FALLBACK == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
			if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_ICC == Auth_Tx.TLF01_details.product_codes[16].amount[0] ||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL == Auth_Tx.TLF01_details.product_codes[16].amount[0] ||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS == Auth_Tx.TLF01_details.product_codes[16].amount[0])
			{
				return true;
			}
		}
	}
	else if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN ncdci_is_Contactless_Mastripe_pos_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_DATA_RFID == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
			if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS == Auth_Tx.TLF01_details.product_codes[16].amount[0] ||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL == Auth_Tx.TLF01_details.product_codes[16].amount[0])
			{
				return true;
			}
		}
	}
	else if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN ncdci_is_Contactless_Chip_pos_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_CARD_DATA_RFID == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
			if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS == Auth_Tx.TLF01_details.product_codes[16].amount[0] ||
				DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL == Auth_Tx.TLF01_details.product_codes[16].amount[0])
			{
				return true;
			}
		}
	}
	else if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN ncdci_is_Manual_pos_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MANUAL == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
			if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MANUAL == Auth_Tx.TLF01_details.product_codes[16].amount[0])
			{
				return true;
			}
		}
		else if( DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_KET_ENTERED == Auth_Tx.TLF01_details.product_codes[16].amount[0])
		{
			return true;
		}
		else if( DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_KEY_ENTERED == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
			return true;
		}
	}
	else if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_MANUAL,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN ncdci_is_ECOM_transaction(void)
{
	if(  true == ncdci_is_issuing_transaction() )
	{
		if( DINERS_DE22_POST_5_INTERNET == Auth_Tx.TLF01_details.product_codes[16].amount[4]||
		    DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_M_COMMERCE == Auth_Tx.TLF01_details.product_codes[16].amount[6] ||
			DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_STORED_CARD_ACCT_INFO == Auth_Tx.TLF01_details.product_codes[16].amount[6])
		{
					return true;
		}
	}
	return false;
}

INT ncdci_is_CAVV_Key_Indicator_set_for_BDO_CAVV(void)
{
	char CAVV_indicator [3] = {0};

	strncpy(CAVV_indicator,
			Auth_Tx.future_use_4 + DINERS_DE122_CAVV_KEY_INDICATOR_POS,
			DINERS_DE122_CAVV_KEY_INDICATOR_LEN);
	if( (0 == strncmp(CAVV_indicator,
					  DINERS_DE122_CAVV_KEY_SET_1_VALUE,
					  DINERS_DE122_CAVV_KEY_INDICATOR_LEN))||
		(0 == strncmp(CAVV_indicator,
					  DINERS_DE122_CAVV_KEY_SET_2_VALUE,
					  DINERS_DE122_CAVV_KEY_INDICATOR_LEN)))
	{
		return true;
	}
	else
	{
		return false;
	}

}


BOOLEAN ncdci_is_Advice_tranasction(void)
{
	if ( true == ncdci_is_Advice_tranasction_req() ||
		 true == ncdci_is_Advice_tranasction_resp ())
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN ncdci_is_Advice_tranasction_req(void)
{
	if ( Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE||
	    (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			 ISO_ADVICE_REQ_MESSAGE,
			 DINERS_MESSAGE_TYPE_SIZE))||
		 (0 == strncmp(Auth_Tx.TLF01_details.message_type,
				 	   DINERS_ADVICE_REQ_MESSAGE,
					   DINERS_MESSAGE_TYPE_SIZE)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN ncdci_is_Advice_tranasction_resp(void)
{
	if (  Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE ||
		  (0 == strncmp(Auth_Tx.TLF01_details.message_type,
						 ISO_ADVICE_RESP_MESSAGE,
						 DINERS_MESSAGE_TYPE_SIZE))||
		 (0 == strncmp(Auth_Tx.TLF01_details.message_type,
				 	   DINERS_ADVICE_RESP_MESSAGE,
					   DINERS_MESSAGE_TYPE_SIZE)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN ncdci_is_EMV_Trn(void)
{
	if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
		 (Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ncdci_build_Magstrip_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		if(true == ncdci_pin_block_exists())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
	}
	else if(true == ncdci_pin_block_exists())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Chip_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		if(true == ncdci_pin_block_exists())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
	}
	else if(true == ncdci_pin_block_exists())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN ,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Fallback_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		if(true == ncdci_pin_block_exists())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
	}
	else if(true == ncdci_pin_block_exists())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Contactless_Mastripe_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		if(true == ncdci_pin_block_exists())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
	}
	else if(true == ncdci_pin_block_exists())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Contactless_Chip_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		if(true == ncdci_pin_block_exists())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
	}
	else if(true == ncdci_pin_block_exists())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Manual_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		if(true == ncdci_check_Asiapay_Ecommerce_transaction())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM,
					DINERS_DE22_SIZE);
		}
		else if (true == ncdci_check_RPS_Transaction())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_RPS,
					DINERS_DE22_SIZE);
		}
		else if (true == ncdci_check_Asiapay_Recurring())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_ASIAPAY_RPS	,
					DINERS_DE22_SIZE);
		}
		else if (true == ncdci_check_BDO_PAY())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_FOR_BDO_PAY	,
					DINERS_DE22_SIZE);
		}
		else if(true == ncdci_pin_block_exists())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
	}
	else if(true == ncdci_check_Asiapay_Ecommerce_transaction())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM,
				DINERS_DE22_SIZE);
	}
	else if (true == ncdci_check_RPS_Transaction())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_RPS,
				DINERS_DE22_SIZE);
	}
	else if (true == ncdci_check_Asiapay_Recurring())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_RPS,
				DINERS_DE22_SIZE);
	}
	else if (true == ncdci_check_BDO_PAY())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_FOR_BDO_PAY	,
				DINERS_DE22_SIZE);
	}
	else if(true == ncdci_pin_block_exists())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_voice_transaction_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_PRESENT_VOICE,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_VOICE,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_unknow_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_PRESENT_UNKNOWN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_MobilePOS_NFC_Tap2Phone_trx_with_pin_pos_data_from_iso_pos_entry_mode ()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_MobilePOS_NFC_Tap2Phone_trx_without_pin_pos_data_from_iso_pos_entry_mode ()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Swiftpass_BDO_Pay_credit_trx_with_pin_pos_data_from_iso_pos_entry_mode ()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_Swiftpass_BDO_Pay_credit_trx_without_pin_pos_data_from_iso_pos_entry_mode ()
{
	if(true == ncdci_check_emv_capability_device ())
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
	else
	{
		strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN,
				DINERS_DE22_SIZE);
	}
}

void ncdci_build_ecommerce_swiftpass_BDOPay_trx_pos_data_from_iso_pos_entry_mode()
{
	if(true == ncdci_check_emv_capability_device ())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DINERS_DE22_SIZE);
		}
}

void ncdci_build_Magstrip_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_Fallback_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_Contactless_Mastripe_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_Contactless_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_Manual_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_MANUAL,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_Ecom_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_ECOM_INCOMING_TRN,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void ncdci_build_unknow_pos_data_in_iso_pos_entry_mode_format_from_pos_data()
{
	memcpy( Auth_Tx.TLF01_details.pos_entry_mode,
			DINERS_BDO_POS_ENTRY_MODE_UNKNOW,
			DINERS_BDO_POS_ENTRY_MODE_LEN );
}

void match_ISO_to_Diners_response_code(  pBYTE diners_response_code)
{
   #define NB 86

   INT  i;
   CHAR iso_response_codes [NB][2] =  {"00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                       "08",  "09",  "10",  "11",  "12",  "13",  "14",  "15",
                                       "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",
                                       "24",  "25",  "26",  "27",  "28",  "29",  "30",  "31",
                                       "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",
                                       "40",  "41",  "42",  "43",  "44",  "51",  "52",  "53",
									   "54",  "55",  "58",  "59",  "60",  "61",  "62",  "63",
									   "64",  "65",  "66",  "68",  "75",  "76",  "77",  "78",
									   "79",  "80",  "81",  "82",  "84",  "85",  "86",  "87",
									   "88",  "89",  "90",  "91",  "92",  "93",  "94",  "95",
									   "96",  "97",  "98",  "NO",  "N7",  "00"};

   CHAR DCI_response_code [NB][3] =    {"081", "182", "182", "109", "200", "182", "182", "200",
                                        "001", "182", "081", "081", "182", "110", "182", "182",
                                        "081", "182", "909", "182", "909", "182", "909", "182",
                                        "   ", "182", "   ", "   ", "182", "   ", "904", "905",
                                        "182", "101", "102", "182", "204", "203", "206", "182",
                                        "115", "208", "182", "209", "182", "182", "182", "182",
									    "101", "117", "182", "210", "203", "182", "104", "182",
									    "182", "182", "205", "182", "106", "182", "182", "182",
									    "182", "307", "909", "182", "182", "081", "909", "182",
									    "909", "182", "910", "182", "909", "124", "909", "909",
									    "909", "182", "182", "909", "182", "400"};

   if(3 == strlen(Auth_Tx.TLF01_details.product_codes[12].code))
   {
	   return;
   }
   strcpy( diners_response_code, "182" ); /* Default value */
	for (i=0; i<NB; i++)
	{
		if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
		{
			strncpy(diners_response_code, DCI_response_code[i], 3);
			break;
		}
	 }
	if(  Auth_Tx.TLF01_details.message_type[1] == '4' )
	{
		strncpy(diners_response_code,
				DINERS_REVERSAL_MESSAGE_RESPONE,
				DINERS_DE39_SIZE);
	}
	else if ( true == ncdci_is_Advice_tranasction())
	{
		strncpy(diners_response_code,
				DINERS_ADVICE_MESSAGE_RESPONSE,
				DINERS_DE39_SIZE);
	}
	else if (0 == strncmp(Auth_Tx.TLF01_details.response_code,
						  DINERS_ISO_APPROVED,
						  DINERS_ISO_RESP_LEN	))
	{
		// Already populated Action code with response code mapping
	}
	else if ( 0 == strncmp(Auth_Tx.TLF01_details.security_response_code,
						   DINERS_RACAL_DECLINE_RESPONSE,
						   DINERS_RACAL_RESPONSE_LEN	))
	{
		strncpy(diners_response_code,
				DINERS_EMV_VALIDATION_FAILED_RESP_CODE,
				DINERS_DE39_SIZE);
		ncdci_log_message( 2, 2,"EMV Validation is failed" ,
					   	   "match_ISO_to_Diners_response_code",1 );
	}
}

void match_DCI_to_ISO_response_code( pBYTE dci_response_code)
{
   INT  i;

   CHAR    iso_response_codes[DINERS_NUMBER_OF_ACTION_CODES][2] =
                             {"08",  "00",  "05",  "54",  "43", "05",  "75",
							  "03",  "13" , "12",  "55",  "05", "43",  "43",
							  "43",  "43",  "43",  "43",  "43", "43",  "80",
                              "30",  "31",  "  ",  "  ",  "  ", "  ",  "  ",
							  "88",  "92",  "94",  "95",  "96", "  ",  "  ",
							  "00",  "00",  "00",  "00",  "00", "00",  "00",
							  "05",  "05",  "05",  "05",  "05", "05",  "05",
							  "05",  "05",  "05",  "05",  "05", "05",  "05",
							  "54",  "54",  "54",  "54",  "54", "54",  "54",
							  "00",  "00",  "01",  "93",  "58"};

   CHAR    auth_advice_dci[DINERS_NUMBER_OF_ACTION_CODES][3] =
                             {"001", "081", "100", "101", "102", "104", "106",
							  "109", "110", "115", "117", "124", "200", "203",
							  "204", "205", "206", "208", "209", "210", "307",
                              "904", "905", "909", "909", "909", "909", "909",
							  "909", "909", "909", "909", "909", "909", "910",
							  "082", "083", "084", "085", "086", "087", "090",
							  "102", "104", "109", "115", "118", "119", "124",
							  "129", "140", "141", "163", "180", "181", "182",
							  "201", "202", "203", "204", "205", "206", "207",
							  "400", "800", "186", "103", "120"};



      for (i=0; i<DINERS_NUMBER_OF_ACTION_CODES; i++)
      {
         if (0 == strncmp(dci_response_code, auth_advice_dci[i], 3))
         {
            strncpy(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2);
            break;
         }
      }
      /* Store the response code sent by DCI for advice transactions,*/
      if(0 == strncmp(Auth_Tx.TLF01_details.message_type,"1120",4) ||
    	 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1420",4) )
      {
    	  memcpy(Auth_Tx.TLF01_details.product_codes[13].quantity, Auth_Tx.TLF01_details.response_code, 2);
      }

}

void ncdci_populate_DE43_name_street_city(char * temp_cardaccname)
{
	pCHAR  *token;
	CHAR symbol[2] ={0};
	int  Merchant_name_len = 0;

	symbol[0]=0x5c;

	token = strtok(temp_cardaccname,symbol);

	/* store Card Acceptor Name */
    strcpy(Auth_Tx.future_use_1,token);
    Merchant_name_len = strlen(Auth_Tx.future_use_1);
    if(Merchant_name_len > 30)
    {
    	strncpy( Auth_Tx.TLF01_details.card_holder_name,
    			Auth_Tx.future_use_1,
				(sizeof(Auth_Tx.TLF01_details.card_holder_name) -1));
    }
    else
    {
    	strncpy( Auth_Tx.TLF01_details.card_holder_name,
				Auth_Tx.future_use_1,
				Merchant_name_len);
    }
    token = strtok(NULL , symbol);

    /*store Card Acceptor street details
    strcpy(Auth_Tx.future_use_2,token);

    token = strtok(NULL , symbol);

 store Card Acceptor city details
    strcpy(Auth_Tx.future_use_3,token);*/
}

void ncdci_send_logoff_message_to_ncdci( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"ncdciA",6);

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
	   ncdci_log_message( 1, 3,"Insufficient Memory to build insert DINERS message" ,
			   	   	   	   "ncdci_send_logoff_message_to_ncdci",0 );
       ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
	   ncdci_log_message( 1, 3,"ncdci3 received logoff message, sending ncdci also to go OFFLINE" ,
			   	   	   	   "ncdci_send_logoff_message_to_ncdci",0 );
	   ret_code = ncdci_send_msg( p_msg, queue, Error );
       free( p_msg );
   }
}

void ncdci_send_logoff_message_to_ncdci3( void )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BOOLEAN         ret_code = false;
   CHAR 		   queue[10] = {0};
   BYTE            Error[100] = {0};

   strncpy(queue,"ncdci3A",7);

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
	   ncdci_log_message( 1, 3,"Insufficient Memory to build insert DINERS3 message" ,
			   	   	   	   "ncdci_send_logoff_message_to_ncdci3",0 );
       ret_code = false;
   }
   else
   {
      /* Send message to update host specific data with data server. */
	   ncdci_log_message( 1, 3,"ncncdci received logoff message, sending ncdci3 also to go OFFLINE" ,
			   	   	   	   "ncdci_send_logoff_message_to_ncdci3",0 );
	   ret_code = ncdci_send_msg( p_msg, queue, Error );
	   free( p_msg );
   }
}

INT ncdci_Sale_transactio (void)
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_SALE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE  )
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT ncdci_Auth_transactio (void)
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
		Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION ||
		Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION )
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT ncdci_Auth_Advice_transaction (void)
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE  )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void ncdci_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  ncdci_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   ncdci_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(ncdci_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  ncdci_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(ncdci_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  ncdci_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(ncdci_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(ncdci_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(ncdci_error_warning_file_path) > 0) &&
	  (strlen(ncdci_module_error_warning_file_name)> 0))
   {
	   ncdci_create_Error_Filename();
   }
}

INT ncdci_Log_error_warning_to_File(pCHAR Error_Buf,pCHAR func,int sev, int detail)
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
	long int file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(ncdci_Erro_warning_Filename);
    path_len = strlen(ncdci_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&ncdci_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	ncdci_create_Error_Filename();
    }
	if((fp = fopen(ncdci_Erro_warning_Filename,"a+b"))==NULL)
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

void ncdci_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( ncdci_Erro_warning_Filename, ncdci_error_warning_file_path );
    strcat( ncdci_Erro_warning_Filename, ncdci_module_error_warning_file_name );
	strcat( ncdci_Erro_warning_Filename, ".EW." );
    strncat(ncdci_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(ncdci_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(ncdci_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}


/******************************************************************************
 *
 *  NAME:         ncdci_send_response_to_GUI
 *
 *  DESCRIPTION:  This function builds and sends a message to the applink
 *                queue.  It sends the Online_Updates structure.  Typically
 *                this is a response from the ncdci host to a 1304 msg.
 *
 *  INPUTS:       (Global)Auth_Tx - contains originator information
 *                (Global)Online_Updates
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_send_response_to_GUI()
{
	CHAR  err_buf[100]  = {0};
	CHAR  lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  mesbuf[100] = {0};
	CHAR  gmt_time[20] = {0};
	INT   ret_val = false;
	INT   seconds = 0;
	INT   buf_temp_len=0;

    /*Send whatever msg coming from Cadeccie for HOST. Make sure we have valid msg from HOST*/
	buf_temp_len = atoi(Auth_Tx.req_data_len);
	if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
	{
		buflen = buf_temp_len;
		memcpy(ReqRsp_Buffer,Auth_Tx.req_buffer,buflen);
		sprintf (mesbuf, "Sending file response coming from Diners Host to Cadencie, message type :%s",
				 Auth_Tx.TLF01_details.message_type) ;
		strcat( err_buf,mesbuf);
		ncdci_log_message( 2,1, err_buf, " ncdci_send_response_to_GUI",1 );
		ret_val = ncdci_build_and_send_request_message( MT_INCOMING_GUI,
			                                      	  	ST1_NONE,
													    ST2_NONE,
													    ncdci2_fileupdate_link_que_name,  //ncdci2
													    (pBYTE)&Auth_Tx,
													    sizeof(Auth_Tx),
													    0,
													    NULL_STR,
													    err_buf );
		 if ( ret_val == false )
		 {
			 sprintf (mesbuf, "Error in Sending file response coming from Diners Host to Cadencie , message type :%s",
			 				 Auth_Tx.TLF01_details.message_type) ;
			 strcat( err_buf,mesbuf);
			 ncdci_log_message( 2,1, err_buf, " ncdci_send_response_to_GUI",1 );
		 }
	}
	else
	{
		sprintf (mesbuf, "Sending file response coming from Diners Host to Cadencie failed, message type :%s",
						 Auth_Tx.TLF01_details.message_type) ;
		strcat( err_buf,mesbuf);
		ncdci_log_message( 2,1, err_buf, " ncdci_send_response_to_GUI",1 );
	}


} /* send_response_to_GUI */


/******************************************************************************
 *
 *  NAME:         ncdci_send_file_request_to_host_1324
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_send_file_request_to_host_1324( pCHAR timer_resp_flag )
{
	CHAR  err_buf[100]  = {0};
	CHAR  lookup_key[DINERS_LOOKUP_KEY_SIZE] = {0};
	CHAR  mesbuf[100] = {0};
	CHAR  gmt_time[20] = {0};
	INT   ret_val = false;
	INT   seconds = 0;
	INT   buf_temp_len=0;

   /*Send whatever msg coming from DCI to Cadencie*/
	buf_temp_len = atoi(Auth_Tx.req_data_len);
	if(buf_temp_len < DINERS_COMM_BUF_SZE && buf_temp_len > 0)
	{
		buflen = buf_temp_len;
		memcpy(ReqRsp_Buffer,Auth_Tx.req_buffer,buflen);
		sprintf (mesbuf, "Sending File request Message coming from DCI to Cadencei , message type :%s",
				 Auth_Tx.TLF01_details.message_type) ;
		strcat( err_buf,mesbuf);
		ncdci_log_message( 2,1, err_buf, "ncdci_send_file_request_to_host_1324",1 );

	ret_val = ncdci_build_and_send_request_message( MT_INCOMING_GUI,
												   	 ST1_NONE,
												     ST2_NONE,
													 ncdci2_fileupdate_link_que_name,  //ncdci2
												     (pBYTE)&Auth_Tx,
												     sizeof(Auth_Tx),
												     0,
												     NULL_STR,
												     err_buf );
	}
	else
	{
		sprintf (mesbuf, "Sending File request Message coming from DCI to Cadencei failed , message type :%s",
				 Auth_Tx.TLF01_details.message_type) ;
		strcat( err_buf,mesbuf);
		ncdci_log_message( 2,1, err_buf, "ncdci_send_file_request_to_host_1324",1 );
	}

	return;
}

/*****************************************************************************

  Function:    ncdci_str_asc_to_bcd

  Description: This function will convert an ascii string to bcd.
  Author:
      unknown
  Inputs:
      p_astr - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string
  Return values:
      None
  Modified by:
      FArad     8/14/98
******************************************************************************/
void ncdci_str_asc_to_bcd (p_astr, how_many, bstring)
   pCHAR        p_astr;
   INT          how_many;
   BYTE			 bstring[];
{
   UINT  i = 0;
   UINT  start;

   if (strlen(p_astr) > (UINT)how_many)
      start = strlen(p_astr) - how_many;
   else
      start = 0;

   //for (i = (strlen(p_astr) - how_many); i < strlen(p_astr); i=i+2)
   for (i = start; i < strlen(p_astr); i=i+2)
   {
      /* if I hit a null terminator, quit.  */
      if ('\0' == p_astr[i])
         break;

      if (p_astr[i] != ' ')
         bstring[i/2] = ((p_astr[i]-'0')*16);

      if (((i+1) < strlen(p_astr)) && (p_astr[i+1] != ' ') )
		   bstring[i/2] += (p_astr[i+1]-'0');

   }

} /* str_asc_to_bcd */


/******************************************************************************
 *
 *  NAME:         GENUTIL_EMV_TO_AUTH_TX
 *
 *  DESCRIPTION:  Convert ISO8583 Field 55 (EMV Data) from their binary/bcd
 *                fields to their ASCII fields in Auth_Tx.
 *
 *  INPUTS:       emv_raw   - The EMV data in its raw format
 *                total_len - Length of the EMV Raw data
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure with EMV in ASCII format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Kumar Verma
 *
 ******************************************************************************/
void ncdci_genutil_EMV_To_Auth_Tx_ncdci_specific( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};


   dataIndex   = 0;

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
        	 // No Dci specefic tag is present.
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {

               case 0x5B:
            	   	 genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
					 memcpy(p_auth_tx->EMV_details.future_field3, tempstr,
							sizeof(p_auth_tx->EMV_details.future_field3)-1);
					 break;

               default:
                   break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x72:
				 /* Get overall length of this field - including tag. */
				 memset( a_string, 0x00, sizeof(a_string) );
				 memset( b_string, 0x00, sizeof(b_string) );
				 b_string[0] = fieldLength+2;
				 genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
				 memcpy( p_auth_tx->EMV_details.future_field2, a_string, 2 );

				 /* Get Tag ID */
				 memcpy(p_auth_tx->EMV_details.future_field2+2, "72", 2 ); /* Tag ID */

				 /* Get length of data for this tag. */
				 memset( a_string, 0x00, sizeof(a_string) );
				 memset( b_string, 0x00, sizeof(b_string) );
				 b_string[0] = fieldLength;
				 genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
				 memcpy( p_auth_tx->EMV_details.future_field2+4, a_string, 2 );

				 /* Finally put in the data. */
				 genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength ,
						 	 	 	   &tempstr[0], sizeof(tempstr));
				 memcpy(p_auth_tx->EMV_details.future_field2+6, tempstr,
						sizeof(p_auth_tx->EMV_details.future_field2)-7);

				 break;

            default:
				break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}

void ncdci_get_local_time(pCHAR localtime_str)
{
	struct tm *current_time ;
	time_t ltime;
	time(&ltime);
	current_time = localtime(&ltime);
	/*years since 1900*/
	if(current_time->tm_year > 100)
	{
		current_time->tm_year = current_time->tm_year %100;
	}
	else if (current_time->tm_year == 100)
	{
		current_time->tm_year = 00;
	}

	/*months since January*/
	current_time->tm_mon++;
	sprintf (localtime_str, "%02d%02d%02d%02d%02d%02d",
			 current_time->tm_year,
			 current_time->tm_mon,
			 current_time->tm_mday,
			 current_time->tm_hour,
			 current_time->tm_min,
			 current_time->tm_sec);
}

void ncdci_get_GMT_Time_MMDDhhmmss(pCHAR GMTtime_str_MMDDhhmmss)
{
	struct tm *current_time ;
	time_t ltime;

	time(&ltime);
	current_time = gmtime(&ltime);

	/*months since January*/
	current_time->tm_mon++;
	sprintf (GMTtime_str_MMDDhhmmss, "%02d%02d%02d%02d%02d",
			 current_time->tm_mon,
			 current_time->tm_mday,
			 current_time->tm_hour,
			 current_time->tm_min,
			 current_time->tm_sec);
}

BYTE Ncdci_Send_Interactive_DB_Request( pPTE_MSG  p_msg_out,
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
BOOLEAN ncdci_load_DCI3DS2_records_for_DCI()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(DCI3DS2)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &diners_3ds2,   0x00, sizeof(DCI3DS2) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	  ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
									 DINERS3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = Ncdci_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
					tran_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &diners_3ds2, p_data+sizeof(AUTH_TX), sizeof(DCI3DS2) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

void ncdci_read_TO_rc_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[4]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_TIMEOUT_RC",           /* points to section name         */
									  "SCHEME_TIMEOUT_RC_FOR_DCI",       	/* points to key name             */
									   "182",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(dci_scheme_timeout_rc,readvalue,3);
	   if(rc == 0)
	   {
		   ncdci_log_message( 2, 1, "SCHEME_TIMEOUT_RC_FOR_DCI Value not read from tf.ini","ncdci_read_TO_rc_from_tf_ini",0);
	   }

}
void ncdci_read_fai_rc_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[4]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",           /* points to section name         */
									  "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI",       	/* points to key name             */
									   "182",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   strncpy(dci_scheme_fai_rc,readvalue,3);
	   if(rc == 0)
	   {
		   ncdci_log_message( 2, 1, "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI Value not read from tf.ini","ncdci_read_fai_rc_from_tf_ini",0);
	   }

}

INT ncdci_check_if_trx_is_bdo_pos_acq_trx( )
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
INT ncdci_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( )
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

INT ncdci_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
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

INT ncdci_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( )
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
			   	   	 DCI_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 DCI_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))	 ||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	     DCI_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_with_pin(  )
{
	if((true == ncdci_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin()) &&
	   (true == ncdci_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == ncdci_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_without_pin(    )
{
	if((true == ncdci_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin()) &&
	   (true == ncdci_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == ncdci_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/*****************************************/

/*Swiftpass BDO Pay Credit Trx*/
INT ncdci_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( )
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

INT ncdci_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( )
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

INT ncdci_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( )
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
					 DCI_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 DCI_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))	 ||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 DCI_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT ncdci_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_with_pin(  )
{
	if((true == ncdci_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin()) &&
	   (true == ncdci_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == ncdci_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncdci_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_without_pin()
{
	if((true == ncdci_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin()) &&
	   (true == ncdci_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == ncdci_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
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

INT ncdci_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( )
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
					 DCI_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)) ||
		(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 DCI_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))	 ||
		(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 DCI_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncdci_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( )
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

INT ncdci_check_if_trx_is_dci_ecommerce_swiftpass_BDOPay_trx( )
{
	if((true == ncdci_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx()) &&
	   (true == ncdci_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == ncdci_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


