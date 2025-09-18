/**************************************************************************************
 
MODULE:      myutils.C
Copyright (c) 2005, Hypercom, Inc.
All Rights Reserved. 

TITLE:       Utility file.
 
DESCRIPTION:  

APPLICATION: TXCNTL

AUTHOR:  Sanjoy Dasgupta

REVISION HISTORY:


$Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\txcntl\myutils.c  $
   
      Rev 1.10   Dec 09 2005 15:39:16   dirby
   Changed strEqual from a function to a Macro - more efficient.
   SCR 18700
   
      Rev 1.9   Dec 30 1999 13:31:52   ddabberu
   aix-build
   
      Rev 1.8   Jul 12 1999 19:03:00   sdasgupt
    
   
      Rev 1.7   Jun 01 1999 17:14:12   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.6   May 13 1999 14:53:10   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.5   Apr 29 1999 15:35:08   sdasgupt
   Bug fixes
   
      Rev 1.4   Apr 15 1999 15:33:36   sdasgupt
   1. Voice-auth Overrides
   2. Voice-auth Reversals
   3. Added Voice-auth error messages
   4. Fixed Deferred balance account update
   5. Fixed error message with the deferred min amount
   6. Fixed bug where a Cash Advance transaction was ignoring the Merchant Capabilites screen
   
      Rev 1.3   Apr 02 1999 16:59:46   sdasgupt
   change the displayed output to the MONITOR
   for float type values.
   
   gb
   
      Rev 1.2   Apr 01 1999 11:20:38   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.1   Feb 23 1999 11:11:26   sdasgupt
    
   
      Rev 1.0   Feb 12 1999 18:54:24   sdasgupt
   Initial version
****************************************************************************************/

#include "string.h"
#include "myutils.h"
#include <stdlib.h>	

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

int Convert_Amt_To_Auth_Tx_String_Format_CF
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount, 13, strTemp) ; /*choice of 15 is arbitrary*/

	nStringLength = strlen (strTemp) ;
	 /*Discard all characters after the decimal point, if present*/
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
    sprintf( rrn, "%01.1s%03.3s%05.5s%03.3s",time_date+3, julian_day,
                 time_str+1, time_date+20 );
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

BYTE FieldIsNumeric (char str[])
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


     






