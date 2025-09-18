/******************************************************************************

   MODULE:      general_utilities.c

   Copyright (c) 2008, Hypercom, Inc.
   All Rights Reserved. 

   TITLE:       Utility file.

   DESCRIPTION:  

   APPLICATION: TXCNTL

   AUTHOR:  Sanjoy Dasgupta

   ----------------
   REVISION HISTORY
   ----------------

   Rev 1.13   Jan 09 2008  dirby
      Changed the format of the RRN:
         from   YDDDhmmssjjj
         to     YDDDhhmmsscc, where cc is a counter from 00 to 99 
   SCR 25046

      Rev 1.12   Dec 06 2006 08:16:16   DIrby
   Added a I/O buffer flush after every write to the raw data trace file.
   This is done because with multiple instances of dcpiso, the instances
   were not always being flushed consistently.  So the entries in the
   raw data trace file were out of order.
   SCR 23864
   
      Rev 1.11   Feb 24 2006 16:37:50   dirby
   Added utility functions used in parsing CVV2 data (DE48).
   SCR 19670
   
      Rev 1.10   Jul 13 2004 17:06:08   mbysani
   Added new functionality for AUTO INIT and RELOAD transaction s
   
      Rev 1.9   Jan 23 2002 14:53:00   dirby
   Initialized buffers in the transaction statistic functions.
   
      Rev 1.8   Jan 16 2002 14:25:04   dirby
   Added transaction trace functionality.
   SCR 546
   
      Rev 1.7   Jan 14 2002 18:05:16   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.6   Oct 08 2001 10:36:42   dirby
   Created function 'is_txn_voice_from_eft( )'.  Special processing
   is required for voice transactions from EFT Master.  SCR 507.
   
      Rev 1.5   Aug 28 2000 10:22:00   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.4   Aug 28 2000 08:55:42   dirby
   Changed the MONITOR_STATS structure from 'long's to 'char's.
   Unix seems to translate longs differently than NT does.
   
      Rev 1.3   Aug 23 2000 16:36:02   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.2   Dec 02 1999 17:16:48   sdasgupt
   Updates
   
      Rev 1.1   Nov 22 1999 17:52:00   sdasgupt
    
   
      Rev 1.0   Nov 17 1999 13:18:16   sdasgupt
   Initial Release
   
      
****************************************************************************************/

#include <string.h>
#include "global_definitions.h"
#include "general_utilities.h"
#include <stdlib.h>	
#include <stdio.h>
#include <time.h>
#include "pte.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ntutils.h"
#include "app_info.h"
#include "txutils.h"
#include "txtrace.h"
#include "diskio.h"

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;
extern BYTE   ReqRsp_Buffer [];
extern INT    buflen;

extern AUTH_TX auth_tx ;
extern CHAR    ServiceName[];


/* RRN counter to maintain a unique value per second (0 - 99). */
extern INT  RRNctr;


int strEqual (char* str1, char* str2)
{
	if (!strcmp (str1, str2))
		return 1 ;
	else
		return 0 ;
} /* strEqual */

void strTrim (char* strSource, char* strDestination)
{
	int index1 = 0, index2 = 0, CurrentChar = 0, firstValidCharacter = 0 ;
	memset (strDestination, 0, sizeof (strDestination)) ;
	
	while ((CurrentChar = strSource[index1++]) != 0)
	{
		if (CurrentChar > 0x20) // valid character
		{
			firstValidCharacter = 1 ;
			strDestination[index2++] = CurrentChar ;
		}
		else // control character or a space
		{
			if (!firstValidCharacter)
				// skip leading control characters or spaces
				continue ;
			break ;
		}
	}
	strDestination[index2] = 0 ;
} /* strTrim */
/*****************************************************************************
*
*  FUNCTION:         Rj_with_lead_zeros
*
*  DESCRIPTION:      This function will right justify a string and
*                    add leading zeros
*
*  INPUTS:           str - the ascii string to be right justified
*                    FieldWidth
*
*  OUTPUTS:          str - the right justified ascii string
*
*  RETURN VALUE:     None
*
*  AUTHOR:           Phani
*
******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
{
   INT               i;
   INT               dif;
   INT               strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }

} /* Rj_with_lead_zeros */
/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
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


INT isnum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);
   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}


/*****************************************************************************
NAME:			Convert_Amt_To_Auth_Tx_String_Format
DESCRIPTION:	Converts from a double value to a string value
				
INPUTS:			
				nAmount:	the value to be converted in double
OUTPUTS:		strAmount	the pointer to the string that contains the converted amount
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*****************************************************************************/
int Convert_Amt_To_Auth_Tx_String_Format 
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount*100, 15, strTemp) ; // choice of 15 is arbitrary

	nStringLength = strlen (strTemp) ;
	// Discard all characters after the decimal point, if present
	for (nIndex = 0; nIndex < nStringLength ; nIndex++)
	{
		if ('.' == strTemp[nIndex])
		{
			strTemp[nIndex] = 0 ;
			break ;
		}
	}

	strcpy (strAmount, strTemp) ;
	return 1 ;
} /* Convert_Amt_To_Auth_Tx_String_Format */

void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character) 
{
   unsigned char strTemp[256] = {0} ;
   int len = 0 ;
   memset (strTemp, fill_character, sizeof strTemp) ;
   
   len = strlen (strAmount) ;
   strTemp [destination_array_size - len] = 0 ;
   strcat (strTemp, strAmount) ;
   strcpy (strAmount, strTemp) ;
}

/******************************************************************************
*
*  NAME:         CREATE_RRN
*
*  DESCRIPTION:  This procedure creates a retrieval reference number 
*                in the
*    OLD            format of YDDDhmmssjjj, that is:
*                     Y   = last digit of the year
*                     DDD = Julian day
*                     hh  = current system hour based on 24 hour clock
*                     mm  = current system minutes
*                     ss  = current system seconds
*                     cc  = counter ranging 00 to 99
*	New Format - TF Phani
*				YDDDSSSSSCCI
*                     Y   = last digit of the year
*                     DDD = Julian day
*                    sssss= seconds till today
*                     cc  = counter ranging 00 to 99
*						I = Instance number of DCPISO
*
*  INPUTS:       RRNctr - (Global) Counter to make RRN unique per second
*
*  OUTPUTS:      rrn    - Formatted as mentioned above in the description
*                         RRN should be a 13 character string.
*                RRNctr - (Global) Counter will be incremented by 1
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
   LONG	   seconds_for_today;
   CHAR	   instanceNo[2]={0};
   INT		len=0;


   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Change date to julian format - DDD */
   calc_julian_day( date_str, julian_day );

   /*OLD Format RRN: "YDDDhhmmsscc" */	
	/*   sprintf( rrn,
           "%01.1s%03.3s%06.6s%02d",
            time_date+3, julian_day, time_str, RRNctr++ ); */
   /* NEW RRN to avoid duplicate RRN */
   /* NEW FORMAT YDDDSSSSSCCI - TF PHANI */
     len = strlen(ServiceName);
     if(strcmp(ServiceName,"dcpiso") == 0)
		 strcpy(instanceNo,"0");
	 else if(isdigit(ServiceName[len-1]))
	 {	
		 strcpy(instanceNo,&ServiceName[len-1]);
	 }
	 else
	 {	// Waring Message to check the INSTACE name

		 strcpy(instanceNo,"9");
	 }

	  seconds_for_today= (LONG)(ptetime_get_time() - SECONDS_IN_30_YRS);
	  seconds_for_today = seconds_for_today%86400;

	  sprintf( rrn,
			   "%01.1s%03.3s%05ld%02d%01s",
				time_date+3, julian_day, seconds_for_today, RRNctr++,instanceNo );


   if ( RRNctr > 99 )
      RRNctr = 0;

   return;
}

/******************************************************************************
*
*  NAME:         GET_DATE
*
*  DESCRIPTION:  This procedure takes an input string that contains a 
                date and
*                time.  It copies the date portion into an output 
                string,
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
*  DESCRIPTION:  This procedure takes an input string that contains a 
date and
*                time.  It copies the time portion into an output 
string,
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
 
INT calc_seconds( pCHAR time_str)
{
	INT seconds = 0;
	
	INT hour = 0;
	INT minutes = 0;
	INT secs = 0;
	CHAR temp[50] = {0} ;

	if (strlen(time_str) < 6 )
	{
		return seconds;
	}
	strncpy(temp,time_str,2);
	hour = atoi(temp);	
	memset(temp, 0x00, sizeof temp);
	strncpy(temp,&time_str[2],2);
	minutes = atoi(temp);	

	memset(temp, 0x00, sizeof temp);
	strncpy(temp,&time_str[4],2);
	secs = atoi(temp);	

	seconds =  (hour * 60 * 60 ) +  (minutes  * 60 )  + secs;

	return seconds;
}

time_t convert_stringDate_timeStruct(pCHAR datetime)
{
	struct tm tm_datetime;
	time_t long_time =0;
	CHAR  year[5], month[3], day[3], hour[3], minute[3], second[3];
	INT   nYear, nMonth, nDay, nHour, nMinute, nSecond;

   /* SCR 16379 - ThoughtFocus
    * The following changes are done so that char arrays are always NULL terminated.
    */
   memset( year, 0x00, sizeof year);
   memset( month, 0x00, sizeof month);
   memset( day, 0x00, sizeof day);
   memset( hour, 0x00, sizeof hour);
   memset( minute, 0x00, sizeof minute);
   memset( second, 0x00, sizeof second);

	if (strlen (datetime) < 14  )
		return long_time;

	strncpy( year, datetime, 4 );
	nYear  = atoi( year ) - 1900;

	strncpy( month, &datetime[4], 2 );
	nMonth = atoi( month );

	strncpy( day, &datetime[6], 2 );
	nDay   = atoi( day );
	
	strncpy( hour, &datetime[8], 2 );
	nHour   = atoi( hour );

	strncpy( minute, &datetime[10], 2 );
	nMinute   = atoi( minute );

	strncpy( second, &datetime[12], 2 );
	nSecond   = atoi( second );

	tm_datetime.tm_sec = nSecond;
	tm_datetime.tm_min = nMinute ;
	tm_datetime.tm_hour = nHour;
	tm_datetime.tm_mday = nDay;
	tm_datetime.tm_mon = nMonth;
	tm_datetime.tm_year = nYear;
	long_time = mktime(&tm_datetime );	

	return long_time;
}

/*****************************************************************************
*
* NAME:        get_timediff
*
* DESCRIPTION: Calculate the difference of the two datetime in seconds
*              
* 
* INPUTS:      current_datetime, old_datetime   - input string format  = YYYYMMDDhhMMss...
*         
* OUTPUTS:      
*
* RTRN VALUE:  None
*
* AUTHOR:      
*
* MODIFICATIONS:
******************************************************************************/

INT get_timediff(pCHAR new_datetime, pCHAR old_datetime)
{

	double    elapsed_time =0;	
	time_t new_time, old_time;

	
	if (strlen (new_datetime) < 14 ||  strlen (old_datetime) < 14)
		return 0;
	new_time = convert_stringDate_timeStruct( new_datetime);
	old_time = convert_stringDate_timeStruct( old_datetime);
	elapsed_time = difftime(  new_time, old_time );
	return (INT)elapsed_time;

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


BYTE IsFieldNumeric (char str[])
{
    int n = 0 ;
    int length = strlen (str) ;
    if (length == 0)
        return 0 ;
    for (n = 0 ; n < length ; n++)
    {
        if (str[n] < '0' || str[n] > '9')
            return 0 ;
    }
    return 1 ;
}

BYTE IsFieldBlank (char str[], int len)
{
	int i ;
	for (i=0; i < len; i++)
	{
		if (str[i] != 0x20)
			return 0 ;
	}
	return 1 ;
}

BYTE IsFieldNull (char str[])
{
	if (strlen (str) == 0)
		return 1 ;
	return 0 ;
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
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100]        ="";

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
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
      dcpiso_log_message( 1, 2, err_msg, "send_transaction_statistics",1 );
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
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100]        ="";

   /* Reset the statistics */
   (void)reset_stats( &Txn_Counts );

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
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
      dcpiso_log_message( 1, 2, err_msg, "reset_transaction_statistics",1 );
   }
   return( ret_code );
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
   CHAR            Buff[200];
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;
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
      ret_code = false;
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
         ret_code = false;
	  }
      free(p_msg);
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         IS_TXN_VOICE_FROM_EFT
 *
 *  DESCRIPTION:  This function determines if a transaction request came from
 *                EFT Master.  The criteria is:
 *                   Terminal ID is all zeroes
 *                   POS Entry Mode is all zeroes
 *                   System Trace Audit Number is all zeroes
 *                   Message type is 0100
 *
 *                Only the first two criteria are used by this function because
 *                ATP might generate a stan and the message type might be a
 *                response.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if transaction request came from EFT, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_txn_voice_from_eft()
{
   INT   ret_val = false;

   /* Check for POS Entry Mode to be all zeroes. */
   if ( 0 == strcmp(auth_tx.TLF01_details.pos_entry_mode,"0000") )
   {
      /* It is all zeroes.  Check Terminal ID. */
      if ( 0 == strcmp(auth_tx.TLF01_details.terminal_id,"00000000") )
      {
         /* This is a transaction from EFT.  So return true. */
         ret_val = true;
      }
   }
   return( ret_val );
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
         dcpiso_log_message( 1, 3, err_buf, "write_to_txn_file",0 );

         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         dcpiso_log_message( 1, 3, err_buf, "write_to_txn_file",0 );
         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            dcpiso_log_message( 1, 3, err_buf, "write_to_txn_file",0 );
         }
      }
      else
      {
         /* Flush the trace file buffer.  Need to do this after
          * every write or else the entries could be out of order
          * due to the buffers of multiple instances being flushed
          * at different times.
          */
         if ( false == flush_file(ppTraceFile_fp) )
         {
            Tracing = TRACE_OFF;
            sprintf( err_buf,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     ServiceName );
            dcpiso_log_message( 1, 3, err_buf, "write_to_txn_file",0 );
            (void)close_trace_file( ppTraceFile_fp );
         }
      }
   }
   return;
}

INT isalphanumSpace(pCHAR string, INT length)
{
   INT i;
   INT countAlphaNum = 0;

   for (i = 0; i < length; i++)
   {
   		if (!isalnum(string[i]))
  		{
  			if (string[i] == ' ')
			{
				if(countAlphaNum == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else
			{
				return(false);
  			}
  		}
		else if(isalnum(string[i]))
		{
			  	countAlphaNum ++;
		}
	}
   
   return(true);
}

