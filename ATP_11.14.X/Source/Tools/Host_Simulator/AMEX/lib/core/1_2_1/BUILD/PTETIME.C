/******************************************************************************
  
        Module: ptetime.c
  
         Title: Pinnacle Time APIs
  
   Description: This module provides a set of APIs to access the C time
                routines.  

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTETIME.C  $
   
      Rev 1.3   May 27 1998 13:59:30   drodden
   Added a function to return a timestamp of the current Greenwich Mean (UTC) time.
   
   
      Rev 1.2   27 May 1998 11:34:00   rmalathk
   replaced _ftime and _timeb for portability.
   
      Rev 1.1   Apr 17 1998 10:06:36   drodden
   Fixed the month returned in the timestamp.  It was zero relative.
   Removed the microseconds from the timestamp.  It really wasn't
   microseconds but an internal counter that gave a ficticious value.
   
   
      Rev 1.0   Oct 23 1997 11:42:36   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      (came from Rev 1.12  of pintime.c)
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

#include "basictyp.h"


/******************************************************************************
*  function ptetime_get_time
*
*  This function returns the current time as a double precision real number
*  in the form  ssssssssss.jjj  where:
*     ssssssssss  = seconds since 1-1-1970.
*     jjj         = milliseconds after the second
*
*  Parameters: none.
*
******************************************************************************/
double ptetime_get_time ()
{
   struct   timeb   temptime;
   double            tempdbl;

   ftime (&temptime);
   tempdbl = (double)temptime.millitm;
   tempdbl/= (double)1000;
   tempdbl+= (double)temptime.time;

   return (tempdbl);
}



/******************************************************************************
*  function ptetime_get_strtime
*
*  Same as above except the time is returned in the string specified.
*
*  Parameters:
*     dest_str.....string to place the time into (should be at least 14 chars).
*
******************************************************************************/
void ptetime_get_strtime ( pCHAR dest_str )
{
   sprintf (dest_str, "%.3f", ptetime_get_time());
}



/******************************************************************************
*  function ptetime_get_timestamp
*
*  This function generates a timestamp for representing the clock time at
*  execution.  The format of the timestamp is ISO standard (???) and has this
*  format:
*     YYYY-MM-DD-HH.II.SS.JJJ
*     where:
*        YYYY   = The year in 4 digits
*        MM     = The month with zero padding, e.g. Feb is 02
*        DD     = The day of the month with zero padding as well
*        HH     = The hour of the day on a 24 hour clock
*        II     = The minute of the hour with zero padding
*        SS     = The seconds after the minute
*        JJJ    = The milliseconds after the second
*
*  Parameters:
*     dest_str.....string to place the timestamp into (should be at least 24 chars).
*
******************************************************************************/
void ptetime_get_timestamp ( pCHAR dest_str )
{
   struct   timeb   time_structure;   /* Time for ftime */
   struct   tm       *current_time;    /* Current time structure */


   /* Get the current time from the operating system and convert to local time.*/
   ftime (&time_structure);
   current_time = localtime (&time_structure.time);
   current_time->tm_mon++;    /* month is 0..11; change to 1..12 */

   /*
      Year 2000 work around: The present call does not indicate the century value.
      Therefore, we bias the year for the current century - i.e. adding 1900 or 2000.
      This will change in the future.
   */
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;

   /* Generate the timestamp string */
   if (dest_str != NULL_PTR)
      sprintf (dest_str, "%04d-%02d-%02d-%02d.%02d.%02d.%03d",
               current_time->tm_year,
               current_time->tm_mon,
               current_time->tm_mday,
               current_time->tm_hour,
               current_time->tm_min,
               current_time->tm_sec,
               time_structure.millitm);
}  /* end of ptetime_get_timestamp */



/******************************************************************************
*  function ptetime_get_gmt
*
*  This function generates a timestamp for the current Greenwich Mean Time.
*
*  The format of the timestamp is:
*     YYYYMMDDHHIISSJJJ
*     where:
*        YYYY   = The year in 4 digits
*        MM     = The month with zero padding, e.g. Feb is 02
*        DD     = The day of the month with zero padding as well
*        HH     = The hour of the day on a 24 hour clock
*        II     = The minute of the hour with zero padding
*        SS     = The seconds after the minute
*        JJJ    = The milliseconds after the second
*
*  Parameters:
*     dest_str.....string to place the timestamp in (should be at least 18 chars).
*
******************************************************************************/
void pintime_get_gmt ( pCHAR dest_str )
{
   struct   timeb    time_structure;  // Time for ftime
   struct   tm       *current_time;   // Current time structure


   /* Get the current GMT time from the operating system */
   ftime (&time_structure);
   current_time = gmtime (&time_structure.time);
   current_time->tm_mon++;    /* month is 0..11; change to 1..12 */
   
   /*
    * Year 2000 work around: The present call does not indicate the century value.
    * Therefore, we bias the year for the current century - i.e. adding 1900 or 2000.
    * This will change in the future.
    */
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;

   /* Generate the timestamp string */
   sprintf (dest_str, "%04d%02d%02d%02d%02d%02d%03d",
            current_time->tm_year,
            current_time->tm_mon,
            current_time->tm_mday,
            current_time->tm_hour,
            current_time->tm_min,
            current_time->tm_sec,
            time_structure.millitm);
}  /* pintime_get_gmt */



