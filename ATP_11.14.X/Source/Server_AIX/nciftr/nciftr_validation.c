/***************************************************************************
*
* Copyright (c) 2004, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      nciftr_validation.c
*  
* TITLE:      CUP Network Control Data Validation Functions
*  
* DESCRIPTION: This file contains a list of functions used to validate
*              data fields.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:     Abhishek Verma
*
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "ptetime.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "nciftr_prototypes.h"
#include "nciftr_constants.h"
#include "nciftr_bitfields.h"

/* Database structures */
extern AUTH_TX   Auth_Tx;

extern CHAR      AppName[];

/******************************************************************************
 *
 *  NAME:         isnum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all numbers, else false 
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT isnum(pCHAR in_str)
{
   INT i;
   INT length;

   length = (INT)strlen(in_str);
	
   for (i = 0; i < length; i++)
   {
      if (!isdigit(in_str[i]))
        return(false);
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         isalphanum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers and / or
 *                alphabetical characters.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all alphanumeric, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isalphanum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         isalphanum_space
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers and / or
 *                alphabetical characters and / or spaces.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all alphanumeric/spaces, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isalphanum_space(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if ( string[i] != ' ' )
      {
         if (!isalnum(string[i]))
           return(false);
      }
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         validate_date_yymm
 *
 *  DESCRIPTION:  This function validates that a date in the format of YYMM
 *                has a valid year (numeric and greater than 0) and a valid
 *                month (numeric and ranging from 01 to 12).
 *
 *  INPUTS:       date - Date to be validated
 *
 *  OUTPUTS:      err_buf - Indicates what is wrong with the input
 *
 *  RTRN VALUE:   True if good date, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_date_yymm( pBYTE date, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  iYear;
   INT  iMonth;
   BYTE temp_year[3];
   BYTE temp_month[3];

   memset( temp_year,  0x00, sizeof(temp_year)  );
   memset( temp_month, 0x00, sizeof(temp_month) );

   memcpy( temp_year,  date,   2 );
   memcpy( temp_month, date+2, 2 );

   iYear  = atoi( temp_year  );
   iMonth = atoi( temp_month );

   if ( true != isnum(temp_year) )
      sprintf( err_buf, "Invalid year - not numeric: %s", temp_year );
   else if ( true != isnum(temp_month) )
      sprintf( err_buf, "Invalid month - not numeric: %s", temp_month );
   else if ( iYear < 0 )
      sprintf( err_buf, "Invalid year - negative: %s", temp_year );
   else if ( (iMonth < 1) || (iMonth > 12) )
      sprintf( err_buf, "Invalid month: %s", temp_month );
   else if ( 2 != strlen(temp_year) )
      sprintf( err_buf, "Invalid year - not 2 digits: %s", temp_year );
   else if ( 2 != strlen(temp_month) )
      sprintf( err_buf, "Invalid month - not 2 digits: %s", temp_month );
   else
      ret_val = true;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_DATE_YYYYMMDD
 *
 *  DESCRIPTION:  This function verifies a date to be valid.  The date must
 *                be in format YYYYMMDD.  The following checks are performed:
 *                   1.  Must be numeric
 *                   2.  Year must be in the range of 2000 to 2200
 *                   3.  Month must be from 1 to 12
 *                   4.  Day cannot be more than the maximum for the month,
 *                       leap year is taken into account for February.
 *
 *  INPUTS:       date_str - Date in YYYYMMDD format
 *
 *  OUTPUTS:      err_buf - Explanation of what is wrong with the date
 *
 *  RTRN VALUE:   True if valid date, false if invalid date
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  imonth, iday, iyear;
   CHAR temp_month[3]="";
   CHAR temp_day[3]  ="";
   CHAR temp_year[5] ="";

   /* Validate date is numeric. */
   if ( (true == isnum(date_str)) && (8 == strlen(date_str)) )
   {
      /* Validate month */
      memcpy( temp_month, date_str+4, 2 );
      imonth = atoi(temp_month);
      if ( (1 <= imonth) && (imonth <= 12) )
      {
         /* Month is valid; check the day. */
         memcpy( temp_day, date_str+6, 2 );
         iday = atoi(temp_day);
         if ( (1 <= iday) && (iday <= 31) )
         {
            if ( iday == 31 )
            {
               if ( (imonth == 1) || (imonth == 3) || (imonth == 5) ||
                    (imonth == 7) || (imonth == 8) || (imonth == 10)||
                    (imonth == 12) )
               {
                  ret_val = true;
               }
            }
            else if ( (iday == 30) && (imonth != 2) )
            {
               ret_val = true;
            }
            else if ( (iday == 29) && (imonth == 2) )
            {
               /* This must be a leap year to be valid. */
               memcpy( temp_year, date_str, 4 );
               iyear = atoi(temp_year);
               if ( (iyear%4) == 0 )
                  ret_val = true;
               else
                  sprintf( err_buf, "Too many days for February (%s)",date_str);
            }
            else
               ret_val = true;

            /* Validate the year. */
            memcpy( temp_year, date_str, 4 );
            iyear = atoi(temp_year);
            if ( (iyear < 2000) || (iyear > 2200) )
            {
               ret_val = false;
               sprintf( err_buf, "ERROR: Invalid date (%s). Bad Year",date_str);
            }
         }
         else
         {
            sprintf( err_buf, "ERROR: Invalid date (%s). Bad Day.", date_str );
         }
      }
      else
      {
         sprintf( err_buf, "ERROR: Invalid date (%s). Bad Month.", date_str );
      }
   }
   else
   {
      sprintf( err_buf, "ERROR: Invalid date (%s).", date_str );
   }
   if ( true != ret_val )
      strcat(  err_buf, " Must be in format YYYYMMDD." );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_TIME
 *
 *  DESCRIPTION:  This function verifies a time to be valid.  The time must
 *                be in format HHMMSS.  The following checks are performed:
 *                   1.  Must be numeric
 *                   2.  HH - Hour must be from 00 - 23
 *                   3.  MM - Minute must be from 00 - 59
 *                   4.  SS - Second must be from 00 - 59
 *
 *  INPUTS:       time_str - Time in hhmmss format
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if valid time, false if invalid time
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_time( pBYTE time_str )
{
   INT  ret_val = false;
   INT  ihour, iminute, isecond;
   CHAR temp_hour[3]   = "";
   CHAR temp_minute[3] = "";
   CHAR temp_second[3] = "";
   CHAR err_buf[100]   ="";

   /* Validate time is numeric. */
   if ( (true == isnum(time_str)) && (6 == strlen(time_str)) )
   {
      /* Validate hour */
      memcpy( temp_hour, time_str, 2 );
      ihour = atoi(temp_hour);
      if ( (0 <= ihour) && (ihour <= 23) )
      {
         /* Hour is valid; check the minute. */
         memcpy( temp_minute, time_str+2, 2 );
         iminute = atoi(temp_minute);
         if ( (0 <= iminute) && (iminute <= 59) )
         {
            /* Minute is valid; check the second. */
            memcpy( temp_second, time_str+4, 2 );
            isecond = atoi(temp_second);
            if ( (0 <= isecond) && (isecond <= 59) )
            {
               /* Second is valid; this means the time is valid. */
               ret_val = true;
            }
            else
               sprintf(err_buf,"ERROR: Invalid time (%s). Bad Second",time_str);
         }
         else
         {
            sprintf( err_buf, "ERROR: Invalid time (%s). Bad Minute.", time_str );
         }
      }
      else
      {
         sprintf( err_buf, "ERROR: Invalid time (%s). Bad Hour.", time_str );
      }
   }
   else
   {
      sprintf( err_buf, "ERROR: Invalid time (%s).", time_str );
   }
   if ( false == ret_val )
      strcat(  err_buf, " Must be in format HHMMSS." );

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         validate_date_time
 *
 *  DESCRIPTION:  This function validates that a date in the format of MMDD
 *                has a valid day (numeric and ranging from 01 to 31) and a
 *                valid month (numeric and ranging from 01 to 12).  It also
 *                validates time.
 *
 *  INPUTS:       date_time - Date/time string, Format: MMDDhhmmss
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if good date/time, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_date_time( pBYTE date_time )
{
   INT   ret_val;
   CHAR  err_buf[100]="";
   BYTE  date[9] = "";
   BYTE  time[7] = "";

   memcpy( date, date_time, 4 );
   ret_val = validate_date_mmdd( date );
   if ( ret_val == true )
   {
      memcpy( time, date_time+4, 6 );
      ret_val = validate_time( time );
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         VALIDATE_DATE_MMDD
 *
 *  DESCRIPTION:  This function verifies a date to be valid.  The date must
 *                be in format MMDD.  The following checks are performed:
 *                   1.  Must be numeric
 *                   2.  Month must be from 1 to 12
 *                   3.  Day cannot be more than the maximum for the month,
 *                       since we do not have the year, we cannot validate
 *                       February against leap year.
 *
 *  INPUTS:       date_str - Date in MMDD format
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if valid date, false if invalid date
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_date_mmdd( pBYTE date_str )
{
   INT  ret_val = false;
   INT  imonth, iday;
   CHAR temp_month[3]="";
   CHAR temp_day[3]  ="";
   CHAR err_buf[100] ="";

   /* Validate date is numeric. */
   if ( (true == isnum(date_str)) && (4 == strlen(date_str)) )
   {
      /* Validate month */
      memcpy( temp_month, date_str, 2 );
      imonth = atoi(temp_month);
      if ( (1 <= imonth) && (imonth <= 12) )
      {
         /* Month is valid; check the day. */
         memcpy( temp_day, date_str+2, 2 );
         iday = atoi(temp_day);
         if ( (1 <= iday) && (iday <= 31) )
         {
            if ( iday > 29 )
            {
               if ( imonth == 2 )
                  sprintf( err_buf, "Too many days for February (%s)",date_str);

               else if ( iday == 31 )
               {
                  if ( (imonth == 4) || (imonth == 6) ||
                       (imonth == 9) || (imonth == 11) )
                  {
                     sprintf( err_buf, "Too many days for the month (%s)",
                              date_str);
                  }
                  else
                     ret_val = true;
               }
               else
                  ret_val = true;
            }
            else
               ret_val = true;
         }
         else
         {
            sprintf( err_buf, "ERROR: Invalid date (%s). Bad Day.", date_str );
         }
      }
      else
      {
         sprintf( err_buf, "ERROR: Invalid date (%s). Bad Month.", date_str );
      }
   }
   else
   {
      sprintf( err_buf, "ERROR: Invalid date (%s).", date_str );
   }
   if ( true != ret_val )
      strcat(  err_buf, " Must be in format MMDD." );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_HOST_MSG
 *
 *  DESCRIPTION:  This function validates information in a request transaction
 *                from the host.  It makes sure of some basic necessities such
 *                as 0100's need card numbers and amounts, etc.  0400's need
 *                originator information, and so on.  This function is only
 *                called for 0100 and 04xx requests.
 *
 *  INPUTS:       Auth_Tx - Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if valid data, false if invalid data
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_host_msg()
{
   INT   ret_val = false;
   CHAR  err_buf[100]="";

   /* Check to make sure the following information exists.  Their values
    * have already been validated when they got moved into Auth_Tx.
    *
    *    Card Number
    *    Processing Code
    *    Transmission Date and Time
    *    System Trace Audit Number
    *    POS Entry Mode
    *    POS Condition Code
    *    Acquiring Institution ID Code
    *    TID and MID
    */
   if ( 0 == strlen( Auth_Tx.TLF01_details.card_num ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing card number, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.processing_code ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing processing code, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.transmission_timestamp ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing transmission timestamp, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.sys_trace_audit_num ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing System Trace Audit Number, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.pos_entry_mode ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing POS Entry Mode, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.pos_condition_code ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing POS Condition Code, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.acq_inst_id_cd ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing Acquiring Inst ID Code, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.terminal_id ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing Terminal ID, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( 0 == strlen( Auth_Tx.TLF01_details.merchant_id ) )
   {
      sprintf( err_buf,
              "Invalid txn from host: missing Merchant ID, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
   }
   else if ( Auth_Tx.TLF01_details.message_type[1] == '2' )
   {
      /* Also check currency code for 1200 txns. */
      if ( 0 == strlen( Auth_Tx.TLF01_details.currency_code ) )
      {
         sprintf( err_buf,
              "Invalid txn from host: missing Currency Code, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
      }
      else
         ret_val = true;
   }
   else
   {
      /* In addition to those fields checked above,
       * also check the following for reversals:
       *
       *    Amount
       *    Original Data Elements (Field 90)
       */
      if ( 0 == strlen( Auth_Tx.TLF01_details.total_amount ) )
      {
         sprintf( err_buf,
              "Invalid txn from host: missing Amount, MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
      }
      else if ( 0 == strlen( Auth_Tx.TLF01_details.orig_message ) )
      {
         sprintf( err_buf,
              "Invalid txn from host: missing Original Info (DE90), MsgType: %s",
               Auth_Tx.TLF01_details.message_type );
      }
      else
         ret_val= true;
   }

   if ( ret_val == false )
      nciftr_log_message( 1, 2, err_buf, "validate_host_msg",1 );

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         BLANK_STRING
 *
 *  DESCRIPTION:  This function determines if a string consists of all blanks.
 *
 *  INPUTS:       max_len - Number of bytes to check as blank or not
 *                astr    - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if 'astr' is all blank, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT blank_string(pCHAR astr, INT max_len)
{
   INT  i;
   INT  ret_val = true;

   if (0 == strlen(astr))
      ret_val = true;

   else
   {
      for(i = 0;i < max_len; i++)
      {
         if (astr[i] != ' ')
         {
            ret_val = false;
            break;
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_MSGTYPE
 *
 *  DESCRIPTION:  This function will validate the message type of a txn
 *                from the host to make sure we support it.  If we do,
 *                then the message type is returned.  If not, then an error
 *                is reported.
 *
 *  INPUTS:       HOST_MESSAGE.msgtype - (GLOBAL) Message type from host
 *
 *  OUTPUTS:      msgtype - Message type from HOST_MESSAGE
 *                err_buf - Text indicating what the invalid message type is
 *
 *  RTRN VALUE:   True if message type is supported, false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_msgtype( pCHAR msgtype, pCHAR err_buf )
{
   INT    ret_val = false;
   INT    i=0;

   CHAR   supported_msgtypes[40][10] = 
               {
                  "0100","0110",
				  "0120","0130",
				  "0121","0131",
                  "0220","0230",
                  "0200","0210",
                  "0320","0330",
                  "0400","0410",
                  "0401","0411",
                  "0420","0430",
                  "0420","0430",
                  "0800","0810",
                  "0820","0830",
                  "0500","0510",
                  "done"
               };

   do
   {
      memcpy( msgtype, HOST_MESSAGE.msgtype, 4 );
      if ( 0 == strcmp( supported_msgtypes[i++], msgtype ) )
      {
         ret_val = true;
      }
   }while( 0 != strcmp( supported_msgtypes[i], "done" ) );

   if ( ret_val == false )
   {
      sprintf( err_buf,
           "Unsupported message received by %s from host. Msg Type = %s",
            AppName, msgtype );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CHECK_END_OF_YEAR_ROLLOVER
 *
 *  DESCRIPTION:  This function checks the transaction date; since ATP sets
 *                the year, make sure that the time difference between txn
 *                origin and ATP allows for end-of-year rollover.
 *
 *                For example, a txn is initiated with mmdd of 0101 of year
 *                2002 early in the morning (like right after midnight).
 *                ATP has not rolled over from 2001 yet.  So when txn comes
 *                into ATP with mmdd date 0101, ATP will prepend year 2001.
 *                This is not good, so this function will set the year to
 *                2002.
 *
 *                Also, check against txn in Dec (e.g. 2001) and ATP is already
 *                on Jan (e.g. 2002).
 *
 *                NOTE: Only the year is updated; month and day are not touched.
 *
 *  INPUTS:       txn_mmdd - Txn date in MMDD format
 *
 *  OUTPUTS:      ATP_date_yyyymmdd - Txn date in YYYYMMDD format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void check_end_of_year_rollover( pBYTE txn_mmdd, pBYTE ATP_date_yyyymmdd )
{
   CHAR  date_yyyymmdd[9]="";
   CHAR  time_date[25];
   CHAR  temp_year[5];
   INT   year;

   ptetime_get_timestamp( time_date );
   get_date( time_date, date_yyyymmdd );

   if ( 0 == strncmp( txn_mmdd, "01", 2 ) )
   {
      /* Transaction month is January. */
      if ( 0 == strncmp( date_yyyymmdd+4, "12", 2 ) )
      {
         /* ATP month is December; need to up the txn year by one. */
         memset( temp_year, 0x00, sizeof(temp_year) );
         memcpy( temp_year, date_yyyymmdd, 4 );
         year = atoi( temp_year ) + 1;
         memset( temp_year, 0x00, sizeof(temp_year) );
         sprintf( temp_year, "%4d", year );
         memcpy( ATP_date_yyyymmdd, temp_year, 4 );
      }
   }
   else if ( 0 == strncmp( txn_mmdd, "12", 2 ) )
   {
      /* Transaction month is December. */
      if ( 0 == strncmp( date_yyyymmdd+4, "01", 2 ) )
      {
         /* ATP month is January; need to decrement txn year by one. */
         memset( temp_year, 0x00, sizeof(temp_year) );
         memcpy( temp_year, date_yyyymmdd, 4 );
         year = atoi( temp_year ) - 1;
         memset( temp_year, 0x00, sizeof(temp_year) );
         sprintf( temp_year, "%4d", year );
         memcpy( ATP_date_yyyymmdd, temp_year, 4 );
      }
   }
   return;
}
