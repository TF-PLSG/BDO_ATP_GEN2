/***************************************************************************
*
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      NCUTILS.C
*  
* TITLE:       Network Control Library Functions      
*  
* DESCRIPTION: This module contains functions commonly used by all or most
*              of the network control services.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      D. Irby
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncmcrd\ncutils.c  $
   
      Rev 1.11   Feb 11 2005 17:32:52   dirby
   Modified to copy decline/failure messages into TLF01's response text.
   SCR 12340
   
      Rev 1.10   Aug 23 2004 12:43:52   dirby
   Added function to convert an ASCII string to binary.
   SCR 1469
   
      Rev 1.9   Oct 09 2003 13:47:14   dirby
   Modified 'set_host_timer' to not use terminal Id as part of the
   lookup key for 0302/0312 messages.
   SCR 1224
   
      Rev 1.8   Sep 14 2002 16:45:10   dirby
   Moved memmnger.h in the list of includes so that it is below
   equitdb.h; needs to be there to compile.
   SCR 867
   
      Rev 1.7   Aug 06 2002 15:17:30   dirby
   Removed the message Id from error log entries.  This is so
   ncmcrd messages will be similar to those of other network controllers.
   SCR 87
   
      Rev 1.6   Jul 07 2002 01:33:26   dirby
   Implemented use of precalculated sizeof values instead of
   recalculating structure sizes over and over and over.
   SCR 594
   
      Rev 1.5   Jan 14 2002 18:19:58   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.4   Dec 10 2001 16:53:04   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.3   Jun 21 2000 14:25:46   dirby
    
   
      Rev 1.2   Dec 29 1999 09:10:26   svelaga
   Changed the code for finding the original transaction from
   NCF30.
   
      Rev 1.1   Nov 24 1999 16:50:48   svelaga
   Initialized the MemInfo structure variable with 
   zeros as per the suggestion of Gabriel.
   
      Rev 1.0   Nov 05 1999 12:50:06   dirby
   Initial Version
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"

#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "txutils.h"
#include "memmnger.h"
#include "ntutils.h"
#include "ncutils.h"
#include "mcrdmain.h"
#include "echo_timer.h"

/* Queue definitions */
extern CHAR  cur_app_name [];
extern CHAR  authorizer_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  applnk_que_name [];
extern CHAR  trands_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  netds_que_name[];
extern CHAR  timerds_que_name[];
extern CHAR  AppName[8];

/* Database structures */
extern NCF01  ncf01_i;
extern NCF01  ncf01_a;

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;
extern INT    GUI_Input_Size;
extern INT    GUI_Response_Size;

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

extern CHAR  echo_lookup_key[15];
extern CHAR  echo_request_timer_key[15];
extern WORD  echo_timer_length;

/******************************************************************************
 *
 *  NAME:         GET_NCF01
 *
 *  DESCRIPTION:  This function makes a call to the data server to select the
 *                NCF01 record for given application.
 *
 *  INPUTS:       p_auth_tx    - Transaction record to be echoed back
 *                source_que   - Queue name of calling application
 *                network_id   - Name of network controller application
 *                network_type - 'I' for Issuer, 'A' for Acquirer
 *                subtype2     - Flag to let us know where to reenter the code
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT get_ncf01( pAUTH_TX p_auth_tx,  pCHAR source_que,
               pBYTE    network_id, pBYTE network_type,
               BYTE     subtype2,   pCHAR error_msg )
{
   pPTE_MSG    p_msg   = NULL_PTR;
   INT         ret_val = NOT_OK;
   NCF01       ncf01;
   BYTE        buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];

   /* Set up network identifying information for this transaction */
   memset( &ncf01, 0x00, Ncf01_Size );
   strcpy( ncf01.primary_key.network_id,   network_id   );
   strcpy( ncf01.primary_key.network_type, network_type );

   /* Build message to get host specific data from data server. */
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &ncf01, Ncf01_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_SELECT, 
                             subtype2,
                             netds_que_name, 
                             source_que,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF01_DATA );
   if ( NULL_PTR == p_msg  )
   {
      strcpy( error_msg, "Insufficient Memory to build NCF01 message" );
   }
   else
   {
      /* Send message to get host specific data from data server. */
      ret_val = send_msg( p_msg, netds_que_name, error_msg );
      free( p_msg );
   }
   return( ret_val );
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
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg )
{
   INT      ret_val = OK;
   LONG     ret_code;
   CHAR     Buff[256];
   CHAR     pte_err_mesg[200] = "";

   ret_code = pteipc_send( p_msg, que_name );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, pte_err_mesg );
      sprintf(Buff,"pteipc error sending to que: %s. %s",que_name,pte_err_mesg);
      memcpy( error_msg, Buff, 99 );
      ret_val = NOT_OK;
   }
   return( ret_val );
}

INT send_response_to_parvbh ( pAUTH_TX p_auth_tx,BYTE msg_type,
        					  pCHAR    source_que, pCHAR error_msg)
{
	pPTE_MSG   p_msg   = NULL_PTR;
	INT        ret_val = NOT_OK;

	p_msg = ptemsg_build_msg( msg_type,
							 ST1_NONE,
							 ST2_NONE,
							 parvbh_que_name,
							 source_que,
							 (pBYTE)p_auth_tx,
							 Auth_Tx_Size,
							 0 );
	if ( NULL_PTR == p_msg )
	{
		strcpy( error_msg, "Insufficient Memory for auth response message" );
	}
	else
	{
		ret_val = send_msg( p_msg, parvbh_que_name, error_msg );
		free( p_msg );
	}

	return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         SET_HOST_TIMER
 *
 *  DESCRIPTION:  This procedure builds a timer message and sends it to the
 *                network data server.  This is a timer used to time messages
 *                sent to a host.
 *
 *  INPUTS:       p_auth_tx    - Transaction record
 *                source_que   - Queue name of calling application
 *                timer_length - Number of seconds to set the timer
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT set_host_timer( pAUTH_TX p_auth_tx,    pCHAR source_que,
                    WORD     timer_length, pCHAR error_msg )
{
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   CHAR     lookup_key[22];
   BYTE     buffer1[sizeof(AUTH_TX)];
   INT      ret_val = NOT_OK;

   /* Build lookup key. */
   memset( lookup_key, 0x00, sizeof(lookup_key) );
   strcpy( lookup_key, AppName );

   if ( AUTH_CARD_UPDATE != p_auth_tx->TLF01_details.tx_key )
      strcat( lookup_key, p_auth_tx->TLF01_details.terminal_id );

   strcat( lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num );

   memcpy(buffer1,p_auth_tx, Auth_Tx_Size);


   /* Send the whole transaction record. */
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );

   /* Check for valid timer value. */
   if ( timer_length <= 0  ||  timer_length > 3600 )
      timer_length = 25;

   p_msg_set_timer = ptetimer_bld_start_timer_req(
                         lookup_key,
                         p_auth_tx->TLF01_details.primary_key.transaction_id,
                         p_auth_tx->TLF01_details.terminal_id,
                         timerds_que_name,
                         source_que,
                         timer_length,
                         0,
                         (pBYTE)&buffer1,  
                         sizeof(buffer1) );

   if ( NULL_PTR == p_msg_set_timer )
   {
      strcpy( error_msg, "Insufficient Memory to build timer message" );
   }
   else
   {
	   //HCTS1-A
	   /* We can chain the request_timer message with set &
	    * clear echo timer request to timerds
	    */
	   if(false == chain_echo_host(echo_lookup_key, echo_timer_length,
	                               application_que_name,p_msg_set_timer))
	   {
		   strcpy( error_msg, "Error chaining the timer request with clear & set the echo_timer");
	   }

      ret_val = send_msg(p_msg_set_timer, timerds_que_name, error_msg);
      free( p_msg_set_timer );
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CLEAR_TIMER
 *
 *  DESCRIPTION:  This function clears the host state timer using a lookup
 *                key passed into it.
 *
 *  INPUTS:       lookup_key - Key to indicate which timer to clear
 *                source_que - Queue name of calling application
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT clear_timer( pCHAR lookup_key, pCHAR source_que, pCHAR error_msg )
{
   pPTE_MSG   p_msg   = NULL_PTR;
   INT        ret_val = NOT_OK;

   /* Build PTEMSG to clear timer */
   p_msg = ptetimer_bld_clear_timer_req(lookup_key,timerds_que_name,source_que);
   if ( NULL_PTR == p_msg )
   { 
      strcpy( error_msg, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Clear Timer message  */
      ret_val = send_msg(p_msg, timerds_que_name, error_msg);
      free( p_msg );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         GET_DATE
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the date portion into an output string, 
 *                formatted slightly different.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *  OUTPUTS:      date_str  : Format = "YYYY0M0D"
 *  RTRN VALUE:   None
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
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *  OUTPUTS:      time_str  : Format = "0H0I0S"
 *  RTRN VALUE:   None
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
 *  NAME:         CONVERT_FROM_JULIAN_DAY
 *
 *  DESCRIPTION:  This procedure converts a date in format YYDDD into a date
 *                format of MMDD.
 *
 *  INPUTS:       julian_str - Date string in format YYDDD
 *
 *  OUTPUTS:      date_time  - Date string in format MMDD
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void convert_from_julian_day( char *julian_str, char *date_time )
{
   char  Year[3];
   char  Month[3];
   char  Days[4];
   int   iYear;
   int   NumDays;
   int   JulianDay;
   int   iMonth;
   int   i;
   int   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   /* Account for Leap Year */
   strncpy( Year, julian_str, 2 );
   iYear = atoi( Year );
   if ( 0 == ( iYear % 4 ) )
      DaysOfMonth[1]++;

   strncpy( Days, &julian_str[2], 3 );
   JulianDay = atoi( Days );
   NumDays = 0;
   for( i=0; i<12; i++ )
   {
      if ( NumDays+DaysOfMonth[i] >= JulianDay )
      {
         iMonth = i+1;
         NumDays = JulianDay - NumDays;
         break;
      }
      else
      {
         NumDays += DaysOfMonth[i];
      }
   }

   sprintf( Days, "%02d", NumDays );
   sprintf( Month, "%02d", iMonth );

   strncpy( date_time, Month, 2 );
   strncat( date_time, Days,  2 );
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
   CHAR    time_milliseconds[30];
   CHAR    gmt_time[15];
   BYTE    transmission_timestamp[11];

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_milliseconds );  // This is used to get milliseconds

   pintime_get_gmt( gmt_time );  // YYYYMMDDhhmmss 
   strncpy( transmission_timestamp, &gmt_time[4], 10 ); // MMDDhhmmss

   memset(time_date, 0, sizeof(time_date));
   strncpy(time_date, gmt_time, 4);
   strncat(time_date, "-", 1);
   strncat(time_date, transmission_timestamp, 2);  //MM
   strncat(time_date, "-", 1);
   strncat(time_date, transmission_timestamp+2, 2); // DD
   strncat(time_date, "-", 1);
   strncat(time_date, transmission_timestamp+4, 2); // hh
   strncat(time_date, ".", 1);
   strncat(time_date, transmission_timestamp+6, 2); // mm
   strncat(time_date, ".", 1);
   strncat(time_date, transmission_timestamp+8, 2); // ss
   strncat(time_date, ".", 1);
   strncat(time_date, transmission_timestamp+10, 3); // ss


   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Change date to julian format - DDD */
   calc_julian_day( date_str, julian_day );

   /* Format RRN: "YDDDhmmssjjj" */
   sprintf(rrn, "%1.1s%3.3s%5.5s%3.3s",time_date+3, julian_day, time_str+1, time_milliseconds+20 );
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG_TO_AUTHORIZER
 *
 *  DESCRIPTION:  This function builds and send a PTE message to the
 *                authorizer queue using the AUTH_TX structure.  It sends
 *                a message type of MT_AUTH_RESPONSE or MT_AUTH_REQUEST.
 *
 *  INPUTS:       p_auth_tx  - Transaction structure
 *                msg_type   - Either MT_AUTH_RESPONSE or MT_AUTH_REQUEST
 *                source_que - Queue name of calling application
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_msg_to_authorizer( pAUTH_TX p_auth_tx,  BYTE msg_type,
                            pCHAR    source_que, pCHAR error_msg )
{
   pPTE_MSG   p_msg   = NULL_PTR;
   INT        ret_val = NOT_OK;

   memset(authorizer_que_name,0x00,10);

   strncpy(authorizer_que_name,"txcntlA",7);
   authorizer_que_name[7] = '\0';

   p_msg = ptemsg_build_msg( msg_type, 
                             ST1_NONE,
                             ST2_NONE,
                             authorizer_que_name,
                             source_que,
                             (pBYTE)p_auth_tx,
                             Auth_Tx_Size,
                             0 );
   if ( NULL_PTR == p_msg )
   {
      strcpy( error_msg, "Insufficient Memory for auth response message" );
   }
   else
   {
      ret_val = send_msg( p_msg, authorizer_que_name, error_msg );
      free( p_msg );
   }
   return( ret_val );
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

/******************************************************************************
 *
 *  NAME:         CREATE_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function creates a table in shared memory consisting
 *                of 3 values:
 *                             number of consecutive time outs
 *                             number of active transactions
 *                             host state
 *
 *                These values get initialized to: 0, 0, 1 respectively.
 *                Note that host state of '1' means not logged on yet.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT create_shared_mem_table( pCHAR AppName, pCHAR errbuf )
{
   INT      ret_val = NOT_OK;
   INT      ret_code;
   CHAR     table_name[15];

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = create_generic_table( table_name );
   if ( MEMACCESSERROR == ret_code )
   {
      /* Table already exist. */
      ret_val = OK;
   }
   else if ( 0 == ret_code )
   {
      /* Table was successfully created.
       * Initialize host counters and state upon startup.
       */
     ret_val = write_to_shared_mem_table( AppName, "0", "0", OFFLINE,
                                          HOST_STATE_CHANGED, errbuf );
   }
   else
   {
      /* Error creating the shared memory table. */
      sprintf( errbuf, "Unable to create shared memory table: %s.",
               table_name );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         WRITE_TO_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function writes to the shared memory table, defined
 *                with a global variable.  It writes 3 values to the table.
 *                These values are defined below in the INPUTS section.
 *
 *  INPUTS:       AppName - Name of application
 *                consec_time_out_count
 *                active_tran_count
 *                host_state
 *                host_state_change_flag:  HOST_STATE_CHANGED
 *                                         HOST_STATE_DID_NOT_CHANGE
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT write_to_shared_mem_table( pCHAR AppName,
                               pBYTE consec_time_out_count,
                               pBYTE active_tran_count,
                               pBYTE host_state,
                                BYTE host_state_change_flag,
                               pCHAR errbuf )
{
   INT   ret_val = NOT_OK;
   LONG  ret_code;
   CHAR  table_name[15];

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = WriteMemTable( table_name, consec_time_out_count,
                             active_tran_count, host_state );
   if ( 0 != ret_code )
   {
      /* Unable to write to shared table. */
      sprintf( errbuf, "Unable to write to shared memory table: %s.",
               table_name );
   }
   else
   {
      ret_val = OK;

      /* Only update the Monitor's display if the status changed. */
      if ( HOST_STATE_CHANGED == host_state_change_flag )
         (void)send_network_status_to_monitor();
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         READ_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function reads the shared memory table, defined
 *                with a global variable.  It reads 3 values from the table.
 *                These values are defined below in the OUTPUTS section.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      consec_time_out_count
 *                active_tran_count
 *                host_state
 *                errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT read_shared_mem_table( pCHAR AppName,
                           pBYTE consec_time_out_count,
                           pBYTE active_tran_count,
                           pBYTE host_state,
                           pCHAR errbuf )
{
   INT   ret_val = NOT_OK;
   INT   ret_code;
   CHAR  table_name[15];

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = ReadMemTable( table_name, consec_time_out_count,
                            active_tran_count, host_state );
   if ( 0 != ret_code )
   {
      sprintf( errbuf, "Unable to read shared memory table: %s",
               table_name );
   }
   else
   {
      ret_val = OK;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SET_HOST_STATE
 *
 *  DESCRIPTION:  This function sets the state of the host in shared memory.
 *
 *  INPUTS:       AppName - Name of application
 *                host_state - State to set the host
 *                                0 = Logged on
 *                                1 = Not lgged on
 *                                2 = Logged on, but busy
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT set_host_state( pCHAR AppName, pBYTE host_state, pCHAR errbuf )
{
   INT   ret_val;
   BYTE  active_cnt[4];
   BYTE  timeout_cnt[4];
   BYTE  temp_host_state[2];

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                    temp_host_state, errbuf );
   if ( true == ret_val )
   {
      ret_val = write_to_shared_mem_table( AppName, timeout_cnt,
                                           active_cnt, host_state,
                                           HOST_STATE_CHANGED, errbuf );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         DECREMENT_ACTIVE_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, decrements the
 *                active transaction counter, and stores those values back
 *                into shared memory.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT decrement_active_count( pCHAR AppName, pCHAR errbuf )
{
   INT      ret_val = NOT_OK;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( OK == ret_val )
   {
      /* Decrement the active transactions counter field */
      tempint = atoi(active_cnt) - 1;
      if ( 0 > tempint )
      {
         /* If less than zero set it to zero */
         strcpy( active_cnt,"0" );
      }
      else
      {
         sprintf( active_cnt, "%d", tempint );
      }

      /* Update the values into shared memory. */
      ret_val = write_to_shared_mem_table( AppName, timeout_cnt,
                                           active_cnt, host_state,
                                           HOST_STATE_DID_NOT_CHANGE, errbuf );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_ACTIVE_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                active transaction counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of active transactions to the max allowed.  If too high,
 *                a warning is posted, but processing continues.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT increment_active_count( pCHAR AppName, pCHAR errbuf )
{
   INT      ret_val = NOT_OK;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   BYTE     update_flag = HOST_STATE_DID_NOT_CHANGE;
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( OK == ret_val )
   {
      /* Increment the active transactions counter field */
      tempint = atoi(active_cnt) + 1;

      /* Check number of active transactions. */
      if ( (tempint >= MaxActiveTxns) && (MaxActiveTxns > 0) )
      {
         /* Transaction count is too high.
          * Post warning, but continue on.
          */
         memset( errbuf, 0x00, sizeof(errbuf) );
         strcpy( errbuf,
                "Active transaction count too high. Continuing to process.");
         ncmcrd_log_message( 3, 2, errbuf , "increment_active_count" , 0 ,"");
      }
      else
      {
         /* Don't let this value get too high. */
         if ( tempint > 999 )
            tempint = 1;
         sprintf( active_cnt, "%d", tempint );

         /* Update the values into shared memory. */
         ret_val = write_to_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                              host_state, update_flag, errbuf );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_TIMEOUT_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                transaction timeout counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of time outs to the max allowed.  If too high, the host
 *                state is set to 2, effectively logging us off.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT increment_timeout_count( pCHAR AppName, pCHAR errbuf, pAUTH_TX p_auth_tx )
{
   INT      ret_val = NOT_OK;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   BYTE     update_flag = HOST_STATE_DID_NOT_CHANGE;
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                    host_state, errbuf );

   sprintf(errbuf, "Incrementing Timeout count:[%d], MTI:%s, STAN:%s, MID:%s, CARD:%s, Handler_Queue:%s ",
		   atoi(timeout_cnt) + 1, p_auth_tx->TLF01_details.message_type, p_auth_tx->TLF01_details.sys_trace_audit_num,
		   p_auth_tx->TLF01_details.merchant_id, mask_card_number(&p_auth_tx->TLF01_details.card_num),
		   p_auth_tx->TLF01_details.handler_queue);
   ncmcrd_log_message( 3, 1, errbuf , "increment_timeout_count" , 0 ,"");

   if ( OK == ret_val )
   {
      /* Increment the consecutive timeout counter field */
      tempint = atoi(timeout_cnt) + 1;

      /* Check number of consecutive timeouts. */
      if ( MaxConsecTimeouts > 0 )
      {
         if ( tempint > MaxConsecTimeouts )
         {
            strcpy( host_state, OFFLINE );
            update_flag = HOST_STATE_CHANGED;

            strcpy( errbuf,
                "Maximum consecutive timeouts reached.");
            ncmcrd_log_message( 1, 2, errbuf , "increment_timeout_count" , 0 ,"");

         }
      }

      /* Don't let this value get too high. */
      if ( tempint > 999 )
         tempint = 1;
      sprintf( timeout_cnt, "%d", tempint );

      /* Update the values into shared memory. */
      ret_val = write_to_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                           host_state, update_flag, errbuf );
   }
   return( ret_val );
}

char* mask_card_number(char *cardNum)
{
	int cardlen = 0;
	int nIndex 	= 0;
	char masked_card_num[20] = {0};

	cardlen = strlen( cardNum );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, cardNum, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}

	return &masked_card_num;
}

/******************************************************************************
 *
 *  NAME:         RESET_TIMEOUT_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, resets the
 *                transaction timeout counter to zero, and stores those
 *                values back into shared memory.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT reset_timeout_count( pCHAR AppName, pCHAR errbuf )
{
   INT      ret_val = NOT_OK;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( AppName, timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( OK == ret_val )
   {
      /* Update the values into shared memory, setting time out count to 0. */
      ret_val = write_to_shared_mem_table( AppName, "0", active_cnt, host_state,
                                           HOST_STATE_DID_NOT_CHANGE, errbuf);
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ReadMemTable
 *
 *  DESCRIPTION:  This is the function that actually connects to the shared
 *                memory table and reads the information contained in it.
 *
 *  INPUTS:       TableName - Name of application
 *
 *  OUTPUTS:      tmouts    - Number of consecutive timeouts
 *                txns      - Number of active transactions
 *                state     - Host state
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
XINT ReadMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state )
{
   XINT           Rt, Ret;
   SHARED_MEM     mem_crdtbl;
   LONG           CurrentPosition = 0L;

 
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(MEMACCESSERROR);
   memset(&mem_crdtbl, 0, sizeof(SHARED_MEM));
   Ret = MemRead(Rt,CurrentPosition +1,sizeof(SHARED_MEM),&mem_crdtbl,MEM_WAIT);
   strcpy(tmouts, mem_crdtbl.net_consec_tmouts_ctr);
   strcpy(txns, mem_crdtbl.active_txns_ctr);
   strcpy(state, mem_crdtbl.host_state);

   return(0);
      
}


/******************************************************************************
 *
 *  NAME:         WriteMemTable
 *
 *  DESCRIPTION:  This is the function that actually connects to the shared
 *                memory table and writes information into it.
 *
 *  INPUTS:       TableName - Name of application
 *                tmouts    - Number of consecutive timeouts
 *                txns      - Number of active transactions
 *                state     - Host state
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
LONG WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state )
{
   XINT               Rt, Ret,result;
   SECTION            TempSec, RetSec;
   MEMINFOMEM         MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   SHARED_MEM         nclynktbl;
   
   memset(&MemInfo,0x00,sizeof(MEMINFOMEM)); // newly added 11/23/99
   
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   strcpy( nclynktbl.net_consec_tmouts_ctr, tmouts );
   strcpy( nclynktbl.active_txns_ctr, txns);
   strcpy( nclynktbl.host_state, state);
   Ret = MemWrite(Rt, Current_Offset + 1, sizeof(nclynktbl), &nclynktbl, MEM_WAIT);
   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   
   return(0L);
}


/******************************************************************************
 *
 *  NAME:         NC_ERROR_HANDLER
 *
 *  DESCRIPTION:  This procedure is used to log errors, warnings, or
 *                informational messages to the error log and opermon.
 *                If debug is on, they will be displayed on the console.
 *
 *  INPUTS:       MonitorFlag - 0 = Do NOT write to Systems Monitor
 *                              1 = DO write to Systems Monitor
 *
 *                Message     - String of text to be displayed
 *
 *                EventLogFlag- 0 = Do NOT write to event log
 *                              1 = DO write to event log
 *
 *                LogMsgType  - INFO_MSG, WARN_MSG, ERROR_MSG, ALERT_MSG
 *
 *                FuncName    - Name of function calling this routine
 *
 *                Severity    - 0, 1, 2, 3, 4 (0 being the lowest severity)
 *
 *                MsgType     - INFO_ERROR, WARNING_ERROR, FATAL_ERROR
 *
 *                Term_Id     - Terminal ID
 *
 *                CardNum     - Card number
 *
 *                Merch_Id    - Merchant ID
 *
 *                Msg_Id      - Unique identifier to indicate where msg originated
 *
 *                RespTextFlag- 1 = Copy Message into auth_tx, else don't
 *
 *                p_auth_tx   - pointer Auth_Tx structure
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void nc_error_handler( INT MonitorFlag, pBYTE Message,  INT  EventLogFlag,
                       INT LogMsgType,  pBYTE FuncName, BYTE Severity,
                       pBYTE MsgType,   pBYTE Term_Id,  pBYTE CardNum,
                       pBYTE Merch_Id,  pBYTE Msg_Id,   INT  RespTextFlag,
                       pAUTH_TX p_auth_tx )
{
   CHAR      Buffer[100] = {0};

   /* Attach a unique identifier to the message.
    * Removed this identifier to make all network
    * controller messages similar.
    */
   memset( Buffer, 0x00, sizeof(Buffer) );
   strncpy( Buffer, Message, 98 );

   /* Call function to post the message. */
   TxUtils_Send_Msg_To_Operator(
         MonitorFlag, Buffer,  EventLogFlag, LogMsgType, FuncName,
         Severity,    MsgType, Term_Id,      CardNum,    Merch_Id );

   if ( (RespTextFlag == 1) && (p_auth_tx != NULL_PTR) )
   {
      memcpy( p_auth_tx->TLF01_details.response_text, Message,
              sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         UPDATE_HOST_TOTALS     ( a.k.a  RECON_UPDATE )
 *
 *  DESCRIPTION:  This function sends auth_tx to the data server.
 *                Its purpose is to update totals and keep a running count.
 *                It also stores the transaction into NCF30.
 *
 *  INPUTS:       p_auth_tx    - Transaction record
 *                subtype_2    - Subtype 2 for the PTE message
 *                network_type - I or A for issuer or acquirer
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT update_host_totals( pAUTH_TX p_auth_tx, BYTE subtype_2, pCHAR network_type )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   INT             ret_code = NOT_OK;
   CHAR            time_date[25];
   CHAR            err_msg[100];
   NCF30           ncf30;


   /* Build the NCF30 structure */
   memset( &ncf30, 0x00, Ncf30_Size );
   strcpy( ncf30.primary_key.network_id, AppName );
   strcpy( ncf30.primary_key.network_type, network_type );
   strcpy( ncf30.primary_key.batch_number, "ACTIVE" );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }
   strcpy( ncf30.primary_key.retrieval_ref_num,
           p_auth_tx->TLF01_details.retrieval_ref_num );

   strcpy( ncf30.merchant_id, p_auth_tx->TLF01_details.merchant_id );
   strcpy( ncf30.transaction_id,
           p_auth_tx->TLF01_details.primary_key.transaction_id );
   strcpy( ncf30.device_id, p_auth_tx->TLF01_details.terminal_id );
   strcpy( ncf30.card_nbr,  p_auth_tx->TLF01_details.card_num  );
   strcpy( ncf30.message_type, p_auth_tx->TLF01_details.message_type );
   strcpy( ncf30.processing_code, p_auth_tx->TLF01_details.processing_code );
   strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
   strcpy( ncf30.auth_number, p_auth_tx->TLF01_details.auth_number );
   strcpy( ncf30.response_code, p_auth_tx->TLF01_details.response_code );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ))
   {
      /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
      ptetime_get_timestamp( time_date );

      /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
      get_date( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
      get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss   );
   }
   strcpy( ncf30.tran_date, p_auth_tx->TLF01_details.date_yyyymmdd );
   strcpy( ncf30.tran_time, p_auth_tx->TLF01_details.time_hhmmss   );
   strcpy( ncf30.sys_trace_audit_nbr, p_auth_tx->TLF01_details.sys_trace_audit_num );
   ncf30.tx_key = p_auth_tx->TLF01_details.tx_key;
   strcpy( ncf30.invoice_nbr, p_auth_tx->TLF01_details.invoice_number );
   memcpy( ncf30.network_data, p_auth_tx->TLF01_details.mcard_banknet,
           sizeof(ncf30.network_data)-1 );
   strcpy( ncf30.currency_code, p_auth_tx->TLF01_details.currency_code);

   /* Build message for data server to update network batch totals. */
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
      ncmcrd_log_message( 2, 2, "Insufficient Memory to build data server message" , "update_host_totals" , 0 ,"");
   }
   else
   {
      /* Send message data server. */
      memset( err_msg, 0x00, sizeof(err_msg) );
      ret_code = send_msg( p_msg, netds_que_name, err_msg );
      free( p_msg );
      if ( OK != ret_code )
      {
         ncmcrd_log_message( 2, 3, err_msg , "update_host_totals" , 0 ,"");
      }
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         RESET_TIMER
 *
 *  DESCRIPTION:  This procedure resets a timer.  The reset is accomplished
 *                by clearing the timer, then setting it.
 *
 *  INPUTS:       AppName      - Name of application
 *                source_que   - Queue name of calling application
 *                timer_id     - ID to help create lookup key
 *                timer_length - Timeout value
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT reset_timer( pCHAR AppName,  pCHAR source_que,
                 pCHAR timer_id, WORD  timer_length,
                 pCHAR error_msg )
{
   pPTE_MSG   p_msg   = NULL_PTR;
   INT        ret_val = OK;
   CHAR       lookup_key[22];
   AUTH_TX    auth_tx;

   /* Do nothing if timer length is zero and this is the idle line timer. */
   if ( (0!=strcmp(LOGON_TIMER_LOOKUP_KEY,timer_id)) || (0 < timer_length) )
   {
      /* Build lookup key. */
      memset( lookup_key, 0x00, sizeof(lookup_key) );
      strcpy( lookup_key, AppName );
      strcat( lookup_key, timer_id );

      ret_val = clear_timer( lookup_key, source_que, error_msg );
      if ( OK == ret_val )
      {
         if ( 0 < timer_length )
         {
            memset( &auth_tx, 0x00, Auth_Tx_Size );
            strcpy( auth_tx.TLF01_details.terminal_id, timer_id );
            ret_val =set_host_timer(&auth_tx,source_que,timer_length,error_msg);
         }
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
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 *  MODIFICATIONS:
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
   CHAR            Buff[512] = {0};
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = OK;
   LONG            msg_ret_code;

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
      strcpy( err_msg, "Insufficient Memory to build PTE message" );
      ret_code = NOT_OK;
   }
   else
   {
      /* Set originator communications information. */
      if ( 0 != strcmp( NULL_STR, comm_info ) )
         ptemsg_set_orig_comms_info( p_msg, comm_info );

      /* Send message que_name */
      msg_ret_code = pteipc_send(p_msg, que_name);
      if(msg_ret_code < 0)
	  {
         pteipc_get_errormsg(msg_ret_code, err_msg);
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff, "Failed transmit to queue: " );
         strcat( Buff, que_name );
         strncat(Buff, err_msg, 65 );
         ret_code = NOT_OK;
	  }
      free(p_msg);
   }
   return( ret_code );
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

}


/*****************************************************************************

  Function:    hhutil_verify_pin_block

  Description: This function will determine whether or not there is a pin block
               in the auth_tx
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - pin block
      false - no pin block 
  Modified by:
      BGable      6/30/98
*******************  ********************************/

BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx)
{
   if(0 == strlen(p_auth_tx->TLF01_details.pin_block))
      return(false);
   if(blank_string(p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block)))
      return(false);
   return(true);
}

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{  
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;
   
   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }
   
} /* asc_to_bcd */

BOOLEAN ncmcrd_Is_Refund_Transaction(pAUTH_TX p_auth_tx)
{
	if((0 ==strncmp(p_auth_tx->TLF01_details.message_type, "0100",4)) &&
	   (0 ==strncmp(p_auth_tx->TLF01_details.processing_code, "20",2))&&
	   (0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "ncmcrd",6)||
	    0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "ncmcr3",6)))
	{/* will include all ncmcrd and ncmcr3*/
		return true;
	}
	return false;
}
